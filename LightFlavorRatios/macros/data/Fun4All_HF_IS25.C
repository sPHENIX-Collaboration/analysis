#include <fun4all/Fun4AllUtils.h>
#include <G4_ActsGeom.C>
#include <G4_Magnet.C>
#include <GlobalVariables.C>
#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>
#include <Trkr_TpcReadoutInit.C>
#include <QA.C>

#include <ffamodules/CDBInterface.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <trackreco/PHTrackPruner.h>

#include <phool/recoConsts.h>

#include <cdbobjects/CDBTTree.h>

#include <kfparticle_sphenix/KFParticle_sPHENIX.h>

#include <stdio.h>

#include "HF_selections_QM25.C"

#include <trackingdiagnostics/TrackResiduals.h>
#include <trackingdiagnostics/TrkrNtuplizer.h>

using namespace HeavyFlavorReco;

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)
R__LOAD_LIBRARY(libcdbobjects.so)
//R__LOAD_LIBRARY(libhf_trigger_data_to_json.so)

void Fun4All_HF_IS25(
    const int nEvents = 100,
    const std::string inputDST = "DST_TRKR_TRACKS_run2pp_ana506_2024p023_v001-00053877-00000.root",
    const std::string inputDir = "/sphenix/lustre01/sphnxpro/production/run2pp/physics/ana506_2024p023_v001/DST_TRKR_TRACKS/run_00053800_00053900/dst/",
    const int nSkip = 0)
{
  bool runTrkrNtuplizer = true;
  bool runQA = false;

  std::string inputTrackFile = inputDir + inputDST;

  std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(inputDST);
  int runnumber = runseg.first;
  std::stringstream nice_runnumber;
  nice_runnumber << std::setw(8) << std::setfill('0') << to_string(runnumber);

  int rounded_up = 100*(std::ceil((float) runnumber/100));
  std::stringstream nice_rounded_up;
  nice_rounded_up << std::setw(8) << std::setfill('0') << to_string(rounded_up);

  int rounded_down = 100*(std::floor((float) runnumber/100));
  std::stringstream nice_rounded_down;
  nice_rounded_down << std::setw(8) << std::setfill('0') << to_string(rounded_down);

  int segment = runseg.second;
  std::stringstream nice_segment;
  nice_segment << std::setw(5) << std::setfill('0') << to_string(segment);

  std::stringstream nice_skip;
  nice_skip << std::setw(5) << std::setfill('0') << to_string(nSkip);

  auto rc = recoConsts::instance();
  rc->set_IntFlag("RUNNUMBER", runnumber);

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");//"2024p023_v001");
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");

  TpcReadoutInit(runnumber);
  std::cout << " run: " << runnumber
            << " samples: " << TRACKING::reco_tpc_maxtime_sample
            << " pre: " << TRACKING::reco_tpc_time_presample
            << " vdrift: " << G4TPC::tpc_drift_velocity_reco
            << std::endl;

  //output_dir = "./"; //Top dir of where the output nTuples will be written
  trailer = "_" + nice_runnumber.str() + "_" + nice_segment.str() + "_" + nice_skip.str() + ".root";

  if (run_pipi_reco) create_hf_directories(pipi_reconstruction_name, pipi_output_dir, pipi_output_reco_file);
  if (run_ppi_reco) create_hf_directories(ppi_reconstruction_name, ppi_output_dir, ppi_output_reco_file);
  if (run_KK_reco) create_hf_directories(KK_reconstruction_name, KK_output_dir, KK_output_reco_file);

  auto se = Fun4AllServer::instance();
  se->Verbosity(1);

  G4TRACKING::SC_CALIBMODE = false;
  Enable::MVTX_APPLYMISALIGNMENT = true;
  ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;
  TRACKING::pp_mode = true;

  Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
  ingeo->AddFile(geofile);
  se->registerInputManager(ingeo);

  G4TPC::ENABLE_MODULE_EDGE_CORRECTIONS = true;

  // to turn on the default static corrections, enable the two lines below
  G4TPC::ENABLE_STATIC_CORRECTIONS = true;
  G4TPC::USE_PHI_AS_RAD_STATIC_CORRECTIONS = false;

  //to turn on the average corrections, enable the three lines below
  //note: these are designed to be used only if static corrections are also applied
  G4TPC::ENABLE_AVERAGE_CORRECTIONS = true;
  G4TPC::USE_PHI_AS_RAD_AVERAGE_CORRECTIONS = false;
   // to use a custom file instead of the database file:
  G4TPC::average_correction_filename = CDBInterface::instance()->getUrl("TPC_LAMINATION_FIT_CORRECTION");

  G4MAGNET::magfield_rescale = 1;

  TrackingInit();

  auto hitsintrack = new Fun4AllDstInputManager("TrackInputManager");
  hitsintrack->fileopen(inputTrackFile);
  se->registerInputManager(hitsintrack);

  //Cluster info
  std::string clus_file = "/sphenix/lustre01/sphnxpro/production/run2pp/physics/ana505_2024p023_v001/DST_TRKR_CLUSTER/run_"
                        + nice_rounded_down.str() + "_" + nice_rounded_up.str()
                        + "/dst/DST_TRKR_CLUSTER_run2pp_ana505_2024p023_v001-" + nice_runnumber.str() + "-" + nice_segment.str() + ".root";

  auto hitsinclus = new Fun4AllDstInputManager("ClusterInputManager");
  hitsinclus->fileopen(clus_file);
  se->registerInputManager(hitsinclus);

  //if (run_pipi_reco || run_Kpi_reco || run_Kpipi_reco || run_KKpi_reco || run_pKpi_reco || run_Lambdapi_reco) init_kfp_dependencies();

  if (run_pipi_reco) reconstruct_pipi_mass();
  if (run_ppi_reco) reconstruct_ppi_mass();
  if (run_KK_reco) reconstruct_KK_mass();

  string nTuplizer_dir = output_dir + "/nTP/";
  string NTP_output_reco_file = nTuplizer_dir + processing_folder + "nTuplizer_tracks" + trailer;

  if (runTrkrNtuplizer)
  {
    std::string makeDirectory = "mkdir -p " + nTuplizer_dir + processing_folder;
    system(makeDirectory.c_str());

    TrkrNtuplizer* eval = new TrkrNtuplizer("TrkrNtuplizer", NTP_output_reco_file, "SvtxTrackMap", 3, 4, 48);
    int do_default = 0;
    eval->Verbosity(0);
    eval->runnumber(runnumber);
    eval->segment(segment);
    eval->do_hit_eval(false);
    eval->do_cluster_eval(false);
    eval->do_clus_trk_eval(false);
    eval->do_vertex_eval(false);
    eval->do_track_eval(true);
    eval->do_tpcseed_eval(false);
    eval->do_siseed_eval(false);
    se->registerSubsystem(eval);
  }

  se->skip(nSkip);
  se->run(nEvents);
  
  if (runQA)
  {
    std::string output_dir_QA = output_dir + "/qaOut";
    std::string makeDirectory = "mkdir -p " + output_dir_QA;
    system(makeDirectory.c_str());
    QA_Output(output_dir_QA + "/qaHists" + trailer);
  }
  
  se->End();
  se->PrintTimer();

  if (runTrkrNtuplizer) end_kfparticle(NTP_output_reco_file, nTuplizer_dir);

  if (run_pipi_reco) end_kfparticle(pipi_output_reco_file, pipi_output_dir);
  if (run_ppi_reco) end_kfparticle(ppi_output_reco_file, ppi_output_dir);
  if (run_KK_reco) end_kfparticle(KK_output_reco_file, KK_output_dir);

  delete se;

  std::cout << "Finished" << std::endl;
  gSystem->Exit(0);
}
