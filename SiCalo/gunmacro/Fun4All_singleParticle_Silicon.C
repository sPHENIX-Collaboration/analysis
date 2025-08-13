#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include <sys/stat.h>
#include <limits.h>     // PATH_MAX
#include <unistd.h>
#include <GlobalVariables.C>

#include <DisplayOn.C>
#include <G4Setup_sPHENIX.C>
#include <G4_Mbd.C>
#include <G4_CaloTrigger.C>
#include <G4_Centrality.C>
#include <G4_DSTReader.C>
#include <G4_Global.C>
#include <G4_HIJetReco.C>
#include <G4_Input.C>
#include <G4_Jets.C>
#include <G4_KFParticle.C>
#include <G4_ParticleFlow.C>
#include <G4_Production.C>
#include <G4_TopoClusterReco.C>

#include <Trkr_RecoInit.C>
#include <Trkr_Clustering.C>
#include <Trkr_Reco.C>
#include <Trkr_Eval.C>
#include <Trkr_QA.C>

#include <Trkr_Diagnostics.C>
#include <G4_User.C>
#include <QA.C>
#include <trackreco/PHActsTrackProjection.h>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>
#include <fun4allutils/TimerStats.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <simqa_modules/QAG4SimulationTracking.h>
#include <qautils/QAHistManagerDef.h>
#include <siliconseedsana/SiliconSeedsAna.h>
#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wundefined-internal"


#pragma GCC diagnostic pop

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libsimqa_modules.so)
R__LOAD_LIBRARY(libsiliconseedsana.so)
R__LOAD_LIBRARY(libtrack_reco.so)

void ensure_dir(const std::string& path)
{
  struct stat info;

  if (stat(path.c_str(), &info) != 0)
  {
    std::cout << "Directory " << path << " does not exist. Creating..." << std::endl;
    if (mkdir(path.c_str(), 0777) != 0)
    {
      std::cerr << "Failed to create directory " << path << std::endl;
      exit(1);
    }
  }
  else if (!(info.st_mode & S_IFDIR))
  {
    std::cerr << "Path " << path << " exists but is not a directory!" << std::endl;
    exit(1);
  }
};


bool useTopologicalCluster = false;
bool jpsiTodielectronOnly = true;
int Fun4All_singleParticle_Silicon(std::string processID = "0")
{
  const int nEvents = 10;

  std::string particle_name = "J/psi";
//  particle_name = "eta";
  std::string particle_name_tag = (particle_name == "J/psi") ? "jpsi" : particle_name;
  std::ostringstream pid;
  pid << std::setw(6) << std::setfill('0') << std::stoi(processID);
  std::string pid_str = pid.str();

  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == nullptr)
  {
	std::cerr << "Failed to get current working directory!" << std::endl;
	return 1;
  }

//  std::string outDir = "/sphenix/user/jaein213/tracking/SiliconSeeding/MC/macro/DST/" + particle_name_tag;
//  std::string outDir2 = "/sphenix/user/jaein213/tracking/SiliconSeeding/MC/macro/ana/" + particle_name_tag;
  std::string baseDir(cwd);
  std::string outDir  = baseDir + "/DST_" + particle_name_tag;
  std::string outDir2 = baseDir + "/ana_" + particle_name_tag;
  std::string outDir3 = baseDir + "/jobtime_" + particle_name_tag;
  ensure_dir(outDir);
  ensure_dir(outDir2);
  ensure_dir(outDir3);
  ensure_dir(outDir + "/qa");
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(10);

  // Opt to print all random seed used for debugging reproducibility. Comment out to reduce stdout prints.
  PHRandomSeed::Verbosity(1);

  recoConsts *rc = recoConsts::instance();
  Input::VERBOSITY = 0;
  Input::SIMPLE = true;
  if(jpsiTodielectronOnly)
  {
    EVTGENDECAYER::DecayFile = "decayfile/JpsiDielectron.DEC";
  }
  InputInit();
  if (Input::SIMPLE)
  {
	INPUTGENERATOR::SimpleEventGenerator[0]->add_particles(particle_name, 1);
	INPUTGENERATOR::SimpleEventGenerator[0]->set_vertex_distribution_function(PHG4SimpleEventGenerator::Gaus,
		PHG4SimpleEventGenerator::Gaus,
		PHG4SimpleEventGenerator::Gaus);
	// INPUTGENERATOR::SimpleEventGenerator[0]->set_vertex_distribution_mean(0., 0., 0.);
	INPUTGENERATOR::SimpleEventGenerator[0]->set_vertex_distribution_width(0.01, 0.01, 5.);
	INPUTGENERATOR::SimpleEventGenerator[0]->set_vertex_distribution_width(0, 0, 0);
	INPUTGENERATOR::SimpleEventGenerator[0]->set_eta_range(-1.1, 1.1);
	INPUTGENERATOR::SimpleEventGenerator[0]->set_phi_range(-M_PI, M_PI);
	// INPUTGENERATOR::SimpleEventGenerator[0]->set_pt_range(0.1, 20.);
	INPUTGENERATOR::SimpleEventGenerator[0]->set_pt_range(0.3, 20.);
  }

  InputRegister();

  if (!Input::READHITS)
  {
	rc->set_IntFlag("RUNNUMBER", 1);

	SyncReco *sync = new SyncReco();
	se->registerSubsystem(sync);

	HeadReco *head = new HeadReco();
	se->registerSubsystem(head);
  }
  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);
  TimerStats *ts = new TimerStats();
  std::string jobtimeFile = outDir3 + "/jobtime_" + processID + ".root";
  ts->OutFileName(jobtimeFile);
  se->registerSubsystem(ts);
  // Simulation setup
  Enable::MBDFAKE = true;
  Enable::PIPE = true;
  Enable::PIPE_ABSORBER = true;
  Enable::MVTX = true;
  Enable::INTT = true;
  Enable::TPC = true;
  Enable::MICROMEGAS = true;

  Enable::MAGNET = true;
  Enable::MAGNET_ABSORBER = true;

  Enable::PLUGDOOR_ABSORBER = true;

  Enable::CEMC = true;
  Enable::CEMC_ABSORBER = true;
  Enable::CEMC_CELL = true;
  Enable::CEMC_TOWER = true;
  Enable::CEMC_CLUSTER = true;
  // Enable::CEMC_EVAL =  false;
  // Enable::CEMC_QA =  false;

  Enable::HCALIN = true;
  Enable::HCALIN_ABSORBER = true;
  Enable::HCALIN_CELL =  true;
  Enable::HCALIN_TOWER =  true;
  // Enable::HCALIN_CLUSTER =  true;
  // Enable::HCALIN_EVAL =  true;
  // Enable::HCALIN_QA =  true;

  Enable::HCALOUT = true;
  Enable::HCALOUT_ABSORBER = true;
  Enable::HCALOUT_CELL =  true;
  Enable::HCALOUT_TOWER =  true;
  // Enable::HCALOUT_CLUSTER = false;
  // Enable::HCALOUT_EVAL =  false;
  // Enable::HCALOUT_QA =  false;

  // Tracking setup

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", CDB::global_tag);
  rc->set_uint64Flag("TIMESTAMP", CDB::timestamp);

  MagnetInit();

  G4Init();
  G4Setup();

  Mbd_Reco();
  Mvtx_Cells();
  Intt_Cells();
  //TPC_Cells();
  //Micromegas_Cells();

  // TrackingInit();
  ACTSGEOM::ActsGeomInit();
  Mvtx_Clustering();
  Intt_Clustering();
  // TPC_Clustering();
  // Micromegas_Clustering();

  // // Calo setup
  CEMC_Cells();
  CEMC_Towers();
  CEMC_Clusters();
  TopoClusterReco();
  HCALInner_Cells();
  HCALOuter_Cells();
  HCALInner_Towers();
  // HCALInner_Clusters();
  HCALOuter_Towers();
  //  HCALOuter_Clusters();

  auto silicon_Seeding = new PHActsSiliconSeeding;
  // silicon_Seeding->set_track_map_name("SvtxTrackSeedContainer");
  silicon_Seeding->Verbosity(0);
  silicon_Seeding->setinttRPhiSearchWindow(0.2);
  silicon_Seeding->setinttZSearchWindow(1.0);
  silicon_Seeding->seedAnalysis(false);
  se->registerSubsystem(silicon_Seeding);

  auto merger = new PHSiliconSeedMerger;
  // merger->trackMapName("SvtxTrackSeedContainer");
  merger->Verbosity(0);
  se->registerSubsystem(merger);

  auto converter = new TrackSeedTrackMapConverter;
  // SiliconTrackSeedContainer or TpcTrackSeedContainer
  converter->setTrackSeedName("SiliconTrackSeedContainer");
  converter->setFieldMap(G4MAGNET::magfield_tracking);
  converter->Verbosity(10);
  se->registerSubsystem(converter);
  PHSimpleVertexFinder *finder = new PHSimpleVertexFinder;
  finder->Verbosity(0);
  finder->setDcaCut(0.1);
  finder->setTrackPtCut(0.);
  finder->setBeamLineCut(1);
  finder->setTrackQualityCut(20);
  finder->setNmvtxRequired(3);
  finder->setOutlierPairCut(0.1);
  se->registerSubsystem(finder);

  Global_Reco(); // Definition is below..
  /////////////////////
  // void Global_Reco()
  // {
  //   int verbosity = std::max(Enable::VERBOSITY, Enable::GLOBAL_VERBOSITY);

  //   Fun4AllServer* se = Fun4AllServer::instance();

  //   GlobalVertexReco* gblvertex = new GlobalVertexReco();
  //   gblvertex->Verbosity(verbosity);
  //   se->registerSubsystem(gblvertex);

  //   return;
  // }

  build_truthreco_tables(); // Definition is below..
  //////////////////////////////////
  // void build_truthreco_tables()
  // {
  //   int verbosity = std::max(Enable::VERBOSITY, Enable::TRACKING_VERBOSITY);
  //   Fun4AllServer* se = Fun4AllServer::instance();

  //   // this module builds high level truth track association table.
  //   // If this module is used, this table should be called before any evaluator calls.
  //   // Removing this module, evaluation will still work but trace truth association through the layers of G4-hit-cluster
  //   SvtxTruthRecoTableEval *tables = new SvtxTruthRecoTableEval();
  //   tables->Verbosity(verbosity);
  //   se->registerSubsystem(tables);

  //   return;
  // }
  /////////////////////////////////
  bool runTruth = false;
  auto ensure_dir = [](const std::string &path)
  {
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
	{
	  std::cout << "Directory " << path << " does not exist. Creating..." << std::endl;
	  mkdir(path.c_str(), 0777);
	}
	else if (!(info.st_mode & S_IFDIR))
	{
	  std::cerr << "Path " << path << " exists but is not a directory!" << std::endl;
	  exit(1);
	}
  };


  std::string outputName = outDir + "/DST_SiliconOnly_single_" + particle_name_tag + "_vtxfixed_Caloevents_";
  if (runTruth)
	outputName += "truth";
  else
	outputName += "reconstructed";
  outputName += "Info_" + processID + ".root";
  std::string outputName2 = outDir2 + "/ana_" + processID + ".root";

  Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", outputName);
  se->registerOutputManager(out);

  auto projection = new PHActsTrackProjection("CaloProjection");
  se->registerSubsystem(projection);
  QAG4SimulationTracking *qa = new QAG4SimulationTracking();
  qa->Verbosity(5);
  se->registerSubsystem(qa);
  auto siana = new SiliconSeedsAna("SiliconSeedsAna");
  siana->setMC(true);
  siana->setVtxSkip(true);
  siana->setTopoCluster(useTopologicalCluster);
  siana->setOutputFileName(outputName2);
  int startnumber = nEvents * std::stoi(processID);
  siana->setStartEventNumber(startnumber);
  se->registerSubsystem(siana);
  se->run(nEvents);

  std::string qaFile = outDir + "/qa/single_" + particle_name_tag + "_QA_VTXfixed" + processID + ".root";
  QAHistManagerDef::saveQARootFile(qaFile);

  //-----
  // Exit
  //-----

  //  CDBInterface::instance()->Print(); // print used DB files
  se->End();
  std::cout << "All done" << std::endl;
  delete se;
  if (Enable::PRODUCTION)
  {
	Production_MoveOutput();
  }

  gSystem->Exit(0);
  return 0;
}
#endif
