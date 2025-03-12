/*
 * This macro shows a working example of running TrackSeeding over the cluster DST
 * This has track residuals as default output but has KFParticle set up with a togglable flag
 * with the default set up for K Short reconstruction
 */

#include <fun4all/Fun4AllUtils.h>
#include <G4_ActsGeom.C>
//#include <G4_Global.C>
#include <G4_Magnet.C>
#include <GlobalVariables.C>
#include <QA.C>
#include <Trkr_Clustering.C>
#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>
#include <Trkr_TpcReadoutInit.C>

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

#include <trackingqa/SiliconSeedsQA.h>
#include <trackingqa/TpcSeedsQA.h>
#include <trackingqa/TpcSiliconQA.h>

#include <trackingdiagnostics/TrackResiduals.h>
#include <trackingdiagnostics/TrkrNtuplizer.h>

#include <kfparticle_sphenix/KFParticle_sPHENIX.h>

#include <stdio.h>

#include "HF_selections_QM25.C"

using namespace HeavyFlavorReco;

R__LOAD_LIBRARY(libkfparticle_sphenix.so)
R__LOAD_LIBRARY(libcalotrigger.so)

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)
R__LOAD_LIBRARY(libcdbobjects.so)
R__LOAD_LIBRARY(libTrackingDiagnostics.so)
R__LOAD_LIBRARY(libtrackingqa.so)

void Fun4All_HF_QM25(
    const int nEvents = 5,
    const std::string inputDST = "DST_TRKR_SEED_run2pp_ana472_2024p014_v003-00053687-00055.root",
    const std::string inputDir = "/sphenix/lustre01/sphnxpro/production/run2pp/physics/ana472_2024p014_v003/DST_TRKR_SEED/run_00053600_00053700/dst/",
    const int nSkip = 0)
{
  std::string inputSeedFile = inputDir + inputDST;

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
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");

  TpcReadoutInit(runnumber);
  std::cout << " run: " << runnumber
            << " samples: " << TRACKING::reco_tpc_maxtime_sample
            << " pre: " << TRACKING::reco_tpc_time_presample
            << " vdrift: " << G4TPC::tpc_drift_velocity_reco
            << std::endl;

  output_dir = "/sphenix/tg/tg01/hf/cdean/QM25_productions/"; //Top dir of where the output nTuples will be written
  trailer = "_" + nice_runnumber.str() + "_" + nice_segment.str() + "_" + nice_skip.str() + ".root";

  if (run_pipi_reco) create_hf_directories(pipi_reconstruction_name, pipi_output_dir, pipi_output_reco_file);
  if (run_Kpi_reco) create_hf_directories(Kpi_reconstruction_name, Kpi_output_dir, Kpi_output_reco_file);
  if (run_Kpipi_reco) create_hf_directories(Kpipi_reconstruction_name, Kpipi_output_dir, Kpipi_output_reco_file);
  if (run_KKpi_reco) create_hf_directories(KKpi_reconstruction_name, KKpi_output_dir, KKpi_output_reco_file);
  if (run_pKpi_reco) create_hf_directories(pKpi_reconstruction_name, pKpi_output_dir, pKpi_output_reco_file);

  //distortion calibration mode
  /*
   * set to true to enable residuals in the TPC with
   * TPC clusters not participating to the ACTS track fit
   */

  G4TRACKING::SC_CALIBMODE = false;
  TRACKING::pp_mode = true;

  //ACTSGEOM::tpotMisalignment = 100.;

  auto se = Fun4AllServer::instance();
  se->Verbosity(1);

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

  auto hitsinseed = new Fun4AllDstInputManager("SeedInputManager");
  hitsinseed->fileopen(inputSeedFile);
  se->registerInputManager(hitsinseed);

  if (get_trigger_info)
  {
    std::string gl1_file = "/sphenix/lustre01/sphnxpro/production/run2pp/physics/ana464_nocdbtag_v001/DST_STREAMING_EVENT_INTT0/run_"
                         + nice_rounded_down.str() + "_" + nice_rounded_up.str()
                         + "/dst/DST_STREAMING_EVENT_INTT0_run2pp_ana464_nocdbtag_v001-" + nice_runnumber.str() + "-" + nice_segment.str() + ".root";

    auto hitsintrig = new Fun4AllDstInputManager("TriggerInputManager");
    hitsintrig->fileopen(gl1_file);
    se->registerInputManager(hitsintrig);
  }

  if (get_dEdx_info || get_detector_info) //Required when building tracks from seeds
  {
    std::string clus_file = "/sphenix/lustre01/sphnxpro/production/run2pp/physics/ana466_2024p012_v001/DST_TRKR_CLUSTER/run_"
                          + nice_rounded_down.str() + "_" + nice_rounded_up.str()
                          + "/dst/DST_TRKR_CLUSTER_run2pp_ana466_2024p012_v001-" + nice_runnumber.str() + "-" + nice_segment.str() + ".root";

    auto hitsinclus = new Fun4AllDstInputManager("ClusterInputManager");
    hitsinclus->fileopen(clus_file);
    se->registerInputManager(hitsinclus);
  }

  // reject laser events if G4TPC::REJECT_LASER_EVENTS is true
  Reject_Laser_Events();

 /*
   * Track Matching between silicon and TPC
   */

  // The normal silicon association methods
  // Match the TPC track stubs from the CA seeder to silicon track stubs from PHSiliconTruthTrackSeeding
  auto silicon_match = new PHSiliconTpcTrackMatching;
  silicon_match->Verbosity(0);
  silicon_match->set_use_legacy_windowing(false);
  silicon_match->set_pp_mode(TRACKING::pp_mode);
  if(G4TPC::ENABLE_AVERAGE_CORRECTIONS)
    {
      // reset phi matching window to be centered on zero
      // it defaults to being centered on -0.1 radians for the case of static corrections only
      std::array<double,3> arrlo = {-0.15,0,0};
      std::array<double,3> arrhi = {0.15,0,0};
      silicon_match->window_dphi.set_QoverpT_range(arrlo, arrhi);
    }
    se->registerSubsystem(silicon_match);

  // Match TPC track stubs from CA seeder to clusters in the micromegas layers
  auto mm_match = new PHMicromegasTpcTrackMatching;
  mm_match->Verbosity(0);
  mm_match->set_pp_mode(TRACKING::pp_mode);
  mm_match->set_rphi_search_window_lyr1(3.);
  mm_match->set_rphi_search_window_lyr2(15.0);
  mm_match->set_z_search_window_lyr1(30.0);
  mm_match->set_z_search_window_lyr2(3.);

  mm_match->set_min_tpc_layer(38);             // layer in TPC to start projection fit
  mm_match->set_test_windows_printout(false);  // used for tuning search windows only
  se->registerSubsystem(mm_match);

  auto deltazcorr = new PHTpcDeltaZCorrection;
  deltazcorr->Verbosity(0);
  se->registerSubsystem(deltazcorr);

  // perform final track fit with ACTS
  auto actsFit = new PHActsTrkFitter;
  actsFit->Verbosity(0);
  actsFit->commissioning(G4TRACKING::use_alignment);
  // in calibration mode, fit only Silicons and Micromegas hits
  actsFit->fitSiliconMMs(G4TRACKING::SC_CALIBMODE);
  actsFit->setUseMicromegas(G4TRACKING::SC_USE_MICROMEGAS);
  actsFit->set_pp_mode(TRACKING::pp_mode);
  actsFit->set_use_clustermover(true);  // default is true for now
  actsFit->useActsEvaluator(false);
  actsFit->useOutlierFinder(false);
  actsFit->setFieldMap(G4MAGNET::magfield_tracking);
  se->registerSubsystem(actsFit);

  auto cleaner = new PHTrackCleaner();
  cleaner->Verbosity(0);
  cleaner->set_pp_mode(TRACKING::pp_mode);
  se->registerSubsystem(cleaner);

  auto finder = new PHSimpleVertexFinder;
  finder->Verbosity(0);
  
  //new cuts
  finder->setDcaCut(0.02);
  finder->setTrackPtCut(0.1);
  finder->setBeamLineCut(1);
  finder->setTrackQualityCut(100);
  finder->setNmvtxRequired(3);
  finder->setOutlierPairCut(0.05);
  
  se->registerSubsystem(finder);

  // Propagate track positions to the vertex position
  auto vtxProp = new PHActsVertexPropagator;
  vtxProp->Verbosity(0);
  vtxProp->fieldMap(G4MAGNET::magfield_tracking);
  se->registerSubsystem(vtxProp);

  if (run_pipi_reco || run_Kpi_reco || run_Kpipi_reco || run_KKpi_reco || run_pKpi_reco) init_kfp_dependencies();

  if (run_pipi_reco) reconstruct_pipi_mass();
  if (run_Kpi_reco) reconstruct_Kpi_mass();
  if (run_Kpipi_reco) reconstruct_Kpipi_mass();
  if (run_KKpi_reco) reconstruct_KKpi_mass();
  if (run_pKpi_reco) reconstruct_pKpi_mass();

  se->skip(nSkip);
  se->run(nEvents);
  se->End();
  se->PrintTimer();

  if (run_pipi_reco) end_kfparticle(pipi_output_reco_file, pipi_output_dir);
  if (run_Kpi_reco) end_kfparticle(Kpi_output_reco_file, Kpi_output_dir);
  if (run_Kpipi_reco) end_kfparticle(Kpipi_output_reco_file, Kpipi_output_dir);
  if (run_KKpi_reco) end_kfparticle(KKpi_output_reco_file, KKpi_output_dir);
  if (run_pKpi_reco) end_kfparticle(pKpi_output_reco_file, pKpi_output_dir);

  delete se;

  std::cout << "Finished" << std::endl;
  gSystem->Exit(0);
}
