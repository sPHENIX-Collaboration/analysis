#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include <fun4all/Fun4AllUtils.h>
#include <G4_ActsGeom.C>
#include <G4_Global.C>
#include <G4_Magnet.C>
#include <G4_Mbd.C>
#include <GlobalVariables.C>
#include <QA.C>
#include <G4_TopoClusterReco.C>
#include <Trkr_Clustering.C>
#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>
#include <Trkr_TpcReadoutInit.C>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>


#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/recoConsts.h>

#include <cdbobjects/CDBTTree.h>

#include <tpccalib/PHTpcResiduals.h>

#include <trackingqa/InttClusterQA.h>
#include <trackingqa/MicromegasClusterQA.h>
#include <trackingqa/MvtxClusterQA.h>
#include <trackingqa/TpcClusterQA.h>
#include <tpcqa/TpcRawHitQA.h>
#include <trackingqa/TpcSeedsQA.h>
#include <trackingqa/SiliconSeedsQA.h>
#include <trackingqa/VertexQA.h>
#include <trackingdiagnostics/TrackResiduals.h>
#include <trackingdiagnostics/TrkrNtuplizer.h>
//#include <trackingdiagnostics/KshortReconstruction.h>
#include <siliconseedsana/SiliconSeedsAna.h>

#include <trackreco/PHActsTrackProjection.h>

// #include <caloreco/CaloGeomMapping.h>
// #include <caloreco/CaloGeomMappingv2.h>
// #include <caloreco/RawClusterBuilderTemplate.h>
// #include <caloreco/RawClusterBuilderTopo.h>

#include <simqa_modules/QAG4SimulationTracking.h>
#include <stdio.h>
#include <sys/stat.h>   // stat, mkdir
#include <limits.h>     // PATH_MAX

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wundefined-internal"

#include <kfparticle_sphenix/KFParticle_sPHENIX.h>
//#include <kfparticle_sphenix/KshortReconstruction_local.h>

#pragma GCC diagnostic pop

R__LOAD_LIBRARY(libkfparticle_sphenix.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)
R__LOAD_LIBRARY(libcdbobjects.so)
R__LOAD_LIBRARY(libmvtx.so)
R__LOAD_LIBRARY(libintt.so)
R__LOAD_LIBRARY(libtrack_reco.so)
  //R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libtpc.so)
R__LOAD_LIBRARY(libmicromegas.so)
R__LOAD_LIBRARY(libTrackingDiagnostics.so)
R__LOAD_LIBRARY(libtrackingqa.so)
R__LOAD_LIBRARY(libtpcqa.so)
R__LOAD_LIBRARY(libsiliconseedsana.so)
// For HepMC Hijing
// try inputFile = /sphenix/sim/sim01/sphnxpro/sHijing_HepMC/sHijing_0-12fm.dat
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

const std::string trkrdir = "/sphenix/lustre01/sphnxpro/mdc2/js_pp200_signal/nopileup/trkrcluster/run0022/detroit/";
const std::string calodir = "/sphenix/lustre01/sphnxpro/mdc2/js_pp200_signal/nopileup/calocluster/run0022/detroit/";
const std::string truthdir = "/sphenix/lustre01/sphnxpro/mdc2/js_pp200_signal/nopileup/trkrhit/run0022/detroit/";
const std::string trkrclusterfilename = "DST_TRKR_CLUSTER_pythia8_Detroit-0000000022-000000.root";
const std::string calofilename = "DST_CALO_CLUSTER_pythia8_Detroit-0000000022-000000.root";
const std::string truthfilename = "DST_TRUTH_pythia8_Detroit-0000000022-00000.root";
int Fun4All_PHYTIA_Silicon(std::string processID = "0")
{
  const int nEvents = 10;

  std::ostringstream pid;
  pid << std::setw(6) << std::setfill('0') << std::stoi(processID);
  std::string pid_str = pid.str();

  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == nullptr)
  {
	std::cerr << "Failed to get current working directory!" << std::endl;
	return 1;
  }

  std::string baseDir(cwd);
  std::string outDir  = baseDir + "/DST";
  std::string outDir2 = baseDir + "/ana";
//  std::string outDir = "/sphenix/user/jaein213/tracking/SiliconSeeding/MC/PYTHIA/macro/DST";
//  std::string outDir2 = "/sphenix/user/jaein213/tracking/SiliconSeeding/MC/PYTHIA/macro/ana";
  ensure_dir(outDir);
  ensure_dir(outDir + "/qa");
  ensure_dir(outDir2);

  bool useTopologicalCluster = false;

  std::string trkrclusterfilename = "DST_TRKR_CLUSTER_pythia8_Detroit-0000000022-" + pid_str + ".root";
  std::string calofilename = "DST_CALO_CLUSTER_pythia8_Detroit-0000000022-" + pid_str + ".root";
  std::string truthfilename = "DST_TRUTH_pythia8_Detroit-0000000022-" + pid_str + ".root";
  std::string inputclusterFile = trkrdir + trkrclusterfilename;
  std::string inputcaloclusterFile = calodir + calofilename;
  std::string inputtruthFile = truthdir + truthfilename;

  // Enable::MVTX_APPLYMISALIGNMENT = true;
  // ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;
  // ACTSGEOM::mvtxMisalignment = 100;
  // ACTSGEOM::inttMisalignment = 100.;
  // ACTSGEOM::tpotMisalignment = 100.;
  auto se = Fun4AllServer::instance();
  se->Verbosity(2);
  auto rc = recoConsts::instance();

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", "MDC2");
  rc->set_uint64Flag("TIMESTAMP", 6);
  rc->set_IntFlag("RUNNUMBER", 1);

  SyncReco *sync = new SyncReco();
  se->registerSubsystem(sync);

  HeadReco *head = new HeadReco();
  se->registerSubsystem(head);
  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);
  // std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");

  // Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
  // ingeo->AddFile(geofile);
  // se->registerInputManager(ingeo);

  G4MAGNET::magfield_rescale = 1;
  TrackingInit();

  auto hitsin = new Fun4AllDstInputManager("InputManager");
  hitsin->fileopen(inputclusterFile);
  se->registerInputManager(hitsin);
  auto hitsin_cluster = new Fun4AllDstInputManager("DSTin_cluster");
  hitsin_cluster->fileopen(inputcaloclusterFile);
  se->registerInputManager(hitsin_cluster);
  auto hitsin_truth = new Fun4AllDstInputManager("DSTin_truth");
  hitsin_truth->fileopen(inputtruthFile);
  se->registerInputManager(hitsin_truth);

  // RawClusterBuilderTemplate *ClusterBuilder = new RawClusterBuilderTemplate("EmcRawClusterBuilderTemplate");

  // ClusterBuilder->Detector("CEMC");
  // ClusterBuilder->set_threshold_energy(0.030); // This threshold should be the same as in CEMCprof_Thresh**.root file below
  // std::string emc_prof = getenv("CALIBRATIONROOT");
  // emc_prof += "/EmcProfile/CEMCprof_Thresh30MeV.root";
  // ClusterBuilder->LoadProfile(emc_prof);
  // ClusterBuilder->set_UseTowerInfo(1); // just use towerinfo
  // //    ClusterBuilder->set_UseTowerInfo(1); // to use towerinfo objects rather than old RawTower
  // se->registerSubsystem(ClusterBuilder);
  TopoClusterReco();

  auto silicon_Seeding = new PHActsSiliconSeeding;
  // silicon_Seeding->set_track_map_name("SvtxTrackSeedContainer");
  silicon_Seeding->Verbosity(0);
  // silicon_Seeding->searchInIntt();
  // silicon_Seeding->setinttRPhiSearchWindow(0.4);
  // silicon_Seeding->setinttZSearchWindow(1.6);
  silicon_Seeding->setinttRPhiSearchWindow(0.2);
  silicon_Seeding->setinttZSearchWindow(1.0);
  silicon_Seeding->seedAnalysis(false);
  se->registerSubsystem(silicon_Seeding);

  auto merger = new PHSiliconSeedMerger;
  // merger->trackMapName("SvtxTrackSeedContainer");
  merger->Verbosity(0);
  se->registerSubsystem(merger);

  // Default set to full SvtxTrackSeeds. Can be set to
  // SiliconTrackSeedContainer or TpcTrackSeedContainer
  auto converter = new TrackSeedTrackMapConverter("SiliconSeedConverter");
  converter->setTrackSeedName("SiliconTrackSeedContainer");
  // converter->setTrackMapName("SiliconSvtxTrackMap");

  converter->setFieldMap(G4MAGNET::magfield_tracking);
  converter->Verbosity(0);
  se->registerSubsystem(converter);

  auto finder = new PHSimpleVertexFinder;
  finder->Verbosity(0);
  finder->setDcaCut(0.075);
  finder->setTrackPtCut(0.1);
  finder->setBeamLineCut(1);
  finder->setTrackQualityCut(300);
  finder->setNmvtxRequired(3);
  finder->setOutlierPairCut(0.075);

  // finder->setTrackMapName("SiliconSvtxTrackMap");
  // finder->setVertexMapName("SiliconSvtxVertexMap");
  se->registerSubsystem(finder);
  // Tracking_Reco(); // Function used to setup the tracking reco. comment out here since we have setup already above

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

  //  build_truthreco_tables(); // Definition is below..
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
  // std::string outDir = "/sphenix/user/jaein213/tracking/SiliconSeeding/MC/macro/DST";

  auto projection = new PHActsTrackProjection("CaloProjection");
  se->registerSubsystem(projection);

  // Ensure output directories exist



  std::string outputName = outDir + "/DST_SiliconOnly_PHYTIA_";
  outputName += "Info_" + processID + ".root";
  std::string outputName2 = outDir2 + "/ana_" + processID + ".root";
  // SecondaryVertexTagger* myTagger = new SecondaryVertexTagger();
  // myTagger->Verbosity(2);
  // if (runTruth)
  //{
  //   myTagger->truthMode();
  // }
  // else
  //{
  //   myTagger->truthRecoMatch();
  // }
  // myTagger->setOutputName(outputName.c_str());
  // myTagger->saveOutput();
  // se->registerSubsystem(myTagger);
  // outputName = processID + "test.root";
  Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", outputName);
  se->registerOutputManager(out);
  QAG4SimulationTracking *qa = new QAG4SimulationTracking();
  qa->Verbosity(0);
//  se->registerSubsystem(qa);
  // outputName2 = processID + "ana.root";
  auto siana = new SiliconSeedsAna("SiliconSeedsAna");
  siana->setMC(true);
  siana->setTrackMapName("SvtxTrackMap");
  siana->setVertexMapName("SvtxVertexMap");
  siana->setTopoCluster(useTopologicalCluster);
  // siana->setVtxSkip(true);
  siana->setOutputFileName(outputName2);
  int startnumber = nEvents * std::stoi(processID);
  siana->setStartEventNumber(startnumber);
  siana->setEMCalClusterContainerName("CLUSTERINFO_CEMC");
  se->registerSubsystem(siana);
//  se->skip(900);
  se->run(nEvents);

  std::string qaFile = outDir + "/qa/singleElectron_QA_VTXfixed" + processID + ".root";
  QAHistManagerDef::saveQARootFile(qaFile);

  //-----
  // Exit
  //-----

  //  CDBInterface::instance()->Print(); // print used DB files
  se->End();
  std::cout << "All done" << std::endl;
  delete se;

  gSystem->Exit(0);
  return 0;
}
#endif
