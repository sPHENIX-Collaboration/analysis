#ifndef MACRO_FUN4ALLSINGLETRACKAN_C
#define MACRO_FUN4ALLSINGLETRACKAN_C

#include <GlobalVariables.C>

#include <G4_ActsGeom.C>
#include <G4_Global.C>
#include <G4_Magnet.C>
#include <QA.C>
#include <Trkr_Clustering.C>
#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>
#include <Trkr_TpcReadoutInit.C>
#include <G4Setup_sPHENIX.C>

#include <tpccalib/PHTpcResiduals.h>

#include <trackingqa/SiliconSeedsQA.h>
#include <trackingqa/TpcSeedsQA.h>
#include <trackingqa/TpcSiliconQA.h>

#include <trackingdiagnostics/TrackResiduals.h>
#include <trackingdiagnostics/TrkrNtuplizer.h>

#include <kfparticle_sphenix/KFParticle_sPHENIX.h>

#include <ffamodules/CDBInterface.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>

#include <uspin/SpinDBNode.h>

#include <phool/recoConsts.h>

#include <bcolumicount/StreamingBcoReco.h>
#include <bcolumicount/BcoLumiReco.h>
#include <ffamodules/SyncReco.h>

// here you need your package name (set in configure.ac)
#include <singletrackan/SingleTrackAN.h>
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libsingletrackan.so)


void Fun4All_SingleTrackAN(const int nevnt = 10, 
  const char *inputfile = "/sphenix/lustre01/sphnxpro/production/run3pp/physics/ana556_2025p013_v002/DST_TRKR_SEED/run_00079500_00079600/DST_TRKR_SEED_run3pp_ana556_2025p013_v002-00079507-00000.root",
  const std::string& outputdir = "/sphenix/tg/tg01/coldqcd/dloomis/singletrackan/", 
  const char *inputfile_bco = "../streamingbcodsts/DST_STREAMING_BCOINFO-00079507.root",
  const std::string& outfilename = "clusters_seeds"
)
{


  Fun4AllServer *se = Fun4AllServer::instance();

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTin");
  in->fileopen(inputfile);
  se->registerInputManager(in);

  Fun4AllInputManager *in_bco = new Fun4AllDstInputManager("DSTin_bco");
  in_bco->fileopen(inputfile_bco);
  se->registerInputManager(in_bco);


  G4TRACKING::convert_seeds_to_svtxtracks = false;
  std::cout << "Converting to seeds : " << G4TRACKING::convert_seeds_to_svtxtracks << std::endl;
  std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(inputfile);
  int runnumber = runseg.first;
  int segment = runseg.second;

  G4TRACKING::SC_CALIBMODE = false;
  Enable::MVTX_APPLYMISALIGNMENT = true;
  ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;
  TRACKING::streaming_mode = true;

  TString outfile = outputdir + runnumber + "-" + segment + ".root";

  std::string theOutfile = outfile.Data();


  auto *rc = recoConsts::instance();
  rc->set_IntFlag("RUNNUMBER", runnumber);
  rc->set_IntFlag("RUNSEGMENT", segment);

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", "newcdbtag");
  rc->set_uint64Flag("TIMESTAMP", runnumber);


  // if you want to rebuild the geometry before reconstructing data
  bool rebuild_tracking_geometry = true;
  if(rebuild_tracking_geometry)
    {
      // To make simulation geometry before running reconstruction
      
      Enable::MVTX = true;
      Enable::INTT = true;
      Enable::TPC = true;
      Enable::MICROMEGAS = true;
      
      G4TPC::tpc_survey_position = true; // to use TPC survey position, set to true
      G4Init();
      G4Setup();
    }

  TpcReadoutInit(runnumber);
 // these lines show how to override the drift velocity and time offset values set in TpcReadoutInit
  // G4TPC::tpc_drift_velocity_reco = 0.0073844; // cm/ns
  // TpcClusterZCrossingCorrection::_vdrift = G4TPC::tpc_drift_velocity_reco;
  // G4TPC::tpc_tzero_reco = -5*50;  // ns
  std::cout << " run: " << runnumber
            << " samples: " << TRACKING::reco_tpc_maxtime_sample
            << " pre: " << TRACKING::reco_tpc_time_presample
            << " vdrift: " << G4TPC::tpc_drift_velocity_reco
            << std::endl;

  if(!rebuild_tracking_geometry)
    {
      std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");
      Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
      ingeo->AddFile(geofile);
      se->registerInputManager(ingeo);
    }
  
  G4TPC::REJECT_LASER_EVENTS=true;
  G4TPC::ENABLE_MODULE_EDGE_CORRECTIONS = true;

  // to turn on the default static corrections, enable the two lines below
  G4TPC::ENABLE_STATIC_CORRECTIONS = true;

  //to turn on the average corrections, enable the three lines below
  //note: these are designed to be used only if static corrections are also applied
  G4TPC::ENABLE_AVERAGE_CORRECTIONS = true;

  G4MAGNET::magfield_rescale = 1;
  TrackingInit();

  // reject laser events if G4TPC::REJECT_LASER_EVENTS is true
  Reject_Laser_Events();

  
  //*********************** Go from clusters to seeds ******************** //

  // auto *hitsinclus = new Fun4AllDstInputManager("ClusterInputManager");
  // hitsinclus->fileopen(inputclusterRawHitFile);
  // se->registerInputManager(hitsinclus);

  // Tracking_Reco_TrackSeed_run2pp();

  // ************************************************* //

  // Go from seeds to tracks

  // Track Matching
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
    converter->Verbosity(0);
    se->registerSubsystem(converter);
  }
  else
  {
    Tracking_Reco_TrackFit_run2pp(theOutfile,"TRKR_CLUSTER_SEED");
  }

  //vertexing and propagation to vertex
  Tracking_Reco_Vertex_run2pp("TRKR_CLUSTER_SEED");

  // ****************** register spin info *****************
  SpinDBNode *spin = new SpinDBNode();
  se->registerSubsystem(spin);

  StreamingBcoReco *streaming_bco = new StreamingBcoReco();
  //streaming_bco->Verbosity(2);
  se->registerSubsystem(streaming_bco);

  
  SingleTrackAN *singleTrackAN = new SingleTrackAN("SingleTrackAN");
  singleTrackAN->set_output_filename(theOutfile);
  //  SingleTrackAN->Verbosity(0);
  se->registerSubsystem(singleTrackAN);


  se->run(nevnt);
  se->End();
  delete se;
  gSystem->Exit(0);
}

#endif
