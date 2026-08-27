/*
 * This macro shows a minimum working example of running track fitting over
 * the production cluster and track seed DSTs.. There are some analysis 
 * modules run at the end which package clusters, and clusters on tracks 
 * into trees for analysis.
 */

#include <fun4all/Fun4AllUtils.h>
#include <G4_ActsGeom.C>
#include <G4_Global.C>
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

#include <calotrigger/TriggerRunInfoReco.h>

#include <track_eec/triggerSelector.h>
#include <track_eec/trackEEC.h>

#include <cstdio>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)
R__LOAD_LIBRARY(libcdbobjects.so)
R__LOAD_LIBRARY(libTrackingDiagnostics.so)
R__LOAD_LIBRARY(libtrackingqa.so)
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libTrackEEC.so)
void Fun4All_trackEEC(
    const int nEvents = 10,
    const int runnumber = 79597,
    const int segment = 0,
    const std::string& seedfilelist = "/sphenix/lustre01/sphnxpro/production/run3pp/physics/ana538_2025p011_v001/DST_TRKR_SEED/run_00079500_00079600/DST_TRKR_SEED_run3pp_ana538_2025p011_v001-00079516-00009.root",
    const std::string& outfilename = "clusters_seeds",
    const bool convertSeeds = false)
{

  G4TRACKING::convert_seeds_to_svtxtracks = convertSeeds;
  std::cout << "Converting to seeds : " << G4TRACKING::convert_seeds_to_svtxtracks << std::endl;
  //std::pair<int, int>
  //    runseg = Fun4AllUtils::GetRunSegment(seedfilename);
  //int runnumber = runseg.first;
  //int segment = runseg.second;

  std::cout << " run: " << runnumber
            << " samples: " << TRACKING::reco_tpc_maxtime_sample
            << " pre: " << TRACKING::reco_tpc_time_presample
            << " vdrift: " << G4TPC::tpc_drift_velocity_reco
            << std::endl;

  // distortion calibration mode
  /*
   * set to true to enable residuals in the TPC with
   * TPC clusters not participating to the ACTS track fit
   */
  G4TRACKING::SC_CALIBMODE = false;
  Enable::MVTX_APPLYMISALIGNMENT = true;
  ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;
  TRACKING::streaming_mode = true;
  
  TString outfile = outfilename + "_" + runnumber + "-" + segment + "-" + (segment+9);

  std::string theOutfile = outfile.Data();

  auto *se = Fun4AllServer::instance();
  se->Verbosity(2);

  auto *rc = recoConsts::instance();
  rc->set_IntFlag("RUNNUMBER", runnumber);
  //rc->set_IntFlag("RUNSEGMENT", segment);
  
  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", "newcdbtag");
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");

  Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
  ingeo->AddFile(geofile);
  se->registerInputManager(ingeo);

  TpcReadoutInit( runnumber );
  // these lines show how to override the drift velocity and time offset values set in TpcReadoutInit
  // G4TPC::tpc_drift_velocity_reco = 0.0073844; // cm/ns
  // TpcClusterZCrossingCorrection::_vdrift = G4TPC::tpc_drift_velocity_reco;
  // G4TPC::tpc_tzero_reco = -5*50;  // ns
  G4TPC::REJECT_LASER_EVENTS=true;
  G4TPC::ENABLE_MODULE_EDGE_CORRECTIONS = true;

  // to turn on the default static corrections, enable the two lines below
  G4TPC::ENABLE_STATIC_CORRECTIONS = true;

  //to turn on the average corrections, enable the three lines below
  //note: these are designed to be used only if static corrections are also applied
  G4TPC::ENABLE_AVERAGE_CORRECTIONS = true;
  G4TPC::USE_PHI_AS_RAD_AVERAGE_CORRECTIONS = false;
   // to use a custom file instead of the database file:
  G4TPC::average_correction_filename = CDBInterface::instance()->getUrl("TPC_LAMINATION_FIT_CORRECTION");

  G4MAGNET::magfield_rescale = 1;
  TrackingInit();

  auto *hitsinseed = new Fun4AllDstInputManager("SeedInputManager");
  hitsinseed->AddListFile(seedfilelist);
  se->registerInputManager(hitsinseed);

  Reject_Laser_Events();

  TriggerRunInfoReco *trigReco = new TriggerRunInfoReco("TriggerRunInfoReco");
  trigReco->UseEmulator(false);
  se->registerSubsystem(trigReco);

  triggerSelector *trigSel = new triggerSelector("triggerSelector");
  trigSel->setTriggerName("Jet 10 GeV");
  se->registerSubsystem(trigSel);
  
  Tracking_Reco_TrackMatching_run2pp("TRKR_CLUSTER_SEED");
  
  
  /*
   * Either converts seeds to tracks with a straight line/helix fit
   * or run the full Acts track kalman filter fit
   */
  if (G4TRACKING::convert_seeds_to_svtxtracks)
  {
    auto *converter = new TrackSeedTrackMapConverter;
    // Default set to full SvtxTrackSeeds. Can be set to
    // SiliconTrackSeedContainer or TpcTrackSeedContainer
    converter->setTrackSeedName("SvtxTrackSeedContainer");
    converter->setFieldMap(G4MAGNET::magfield_tracking);
    converter->setClusterMapName("TRKR_CLUSTER_SEED");
    converter->Verbosity(0);
    se->registerSubsystem(converter);
  }
  else
  {
    Tracking_Reco_TrackFit_run2pp(theOutfile,"TRKR_CLUSTER_SEED");
  }

  //vertexing and propagation to vertex
  Tracking_Reco_Vertex_run2pp("TRKR_CLUSTER_SEED");

  TString residoutfile = "/sphenix/tg/tg01/jets/bkimelman/track_EEC/trackDSTs/July15_2026_cuts/" + theOutfile + "_trackEECs.root";
  std::string residstring(residoutfile.Data());

  trackEEC *EEC = new trackEEC("trackEEC");
  EEC->SetOutfileName(residstring);
  se->registerSubsystem(EEC);


  se->run(nEvents);
  se->End();
  se->PrintTimer();

  std::cout << "CDB Files used:" << std::endl;
  CDBInterface::instance()->Print();
  
  delete se;
  std::cout << "Finished" << std::endl;
  gSystem->Exit(0);
}
