#include <GlobalVariables.C>

#include <DisplayOn.C>
#include <G4Setup_sPHENIX.C>
#include <G4_Global.C>
#include <G4_ActsGeom.C>

#include <Trkr_RecoInit.C>
#include <Trkr_Clustering.C>
#include <Trkr_LaserClustering.C>

#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>

#include <fun4all/Fun4AllServer.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <g4tpc/PHG4TpcCentralMembrane.h>

#include <tpccentralmembraneplotter/TpcCentralMembranePlotter.h>

void Fun4All_DiffuseLaserSimulations()
{
  auto se = Fun4AllServer::instance();
  auto rc = recoConsts::instance();

  
  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);
  rc->set_IntFlag("RUNNUMBER", 1);

  SyncReco *sync = new SyncReco();
  se->registerSubsystem(sync);

  HeadReco *head = new HeadReco();
  se->registerSubsystem(head);

  // Build active TPC geometry and create G4HIT_TPC.
  
  Enable::MVTX = true;
  Enable::INTT = true;
  Enable::TPC = true;
  Enable::MICROMEGAS = true;

  Enable::MVTX_CELL = false;
  Enable::INTT_CELL = false;
  Enable::TPC_CELL = true;
  Enable::MICROMEGAS_CELL = false;

  Enable::TPC_ENDCAP = false;


  G4TPC::ENABLE_CENTRAL_MEMBRANE_HITS = true;
  G4TPC::ENABLE_CENTRAL_MEMBRANE_CLUSTERING = true;

  // Generate central membrane diffuse laser G4 hits.
  G4TPC::ENABLE_CENTRAL_MEMBRANE_HITS = true;

  // Enable diffuse laser clustering.
  G4TPC::ENABLE_CENTRAL_MEMBRANE_CLUSTERING = true;
  G4TPC::LaserClusteringSequential = false;
  G4TPC::laserClusterFitting = false;
  G4TPC::laser_adc_threshold = 100.0;


  TRACKING::streaming_mode = true; // need this to allow for long enough readout window to read laser pulse


  // Configure and register Geant4 detector geometry.
  G4Init();
  G4Setup();

  auto centralMembrane = new PHG4TpcCentralMembrane;
  centralMembrane->setCentralMembraneDelay(4200); // delay 4.2 microseconds to put laser flash in t bin to match data
  centralMembrane->setCentralMembraneEventModulo(1);
  centralMembrane->set_int_param("electrons_per_stripe", 100);
  se->registerSubsystem(centralMembrane);

  auto* padplane = new PHG4TpcPadPlaneReadout;
  padplane->Verbosity(0);
  padplane->set_int_param("ntpc_phibins_inner", G4TPC::tpc_layer_rphi_count_inner);
  //  padplane->SetDriftVelocity(drift_vel);

  auto* edrift = new PHG4TpcElectronDrift;
  edrift->Detector("TPC");
  edrift->Verbosity(0);

  // if( G4TPC::ENABLE_STATIC_DISTORTIONS && G4TPC::static_distortion_filename.empty() )
  // {
  //   G4TPC::static_distortion_filename = CDBInterface::instance()->getUrl("TPC_STATIC_DISTORTION");
  // }

  // if( G4TPC::ENABLE_TIME_ORDERED_DISTORTIONS && G4TPC::time_ordered_distortion_filename.empty() )
  // {
  //   G4TPC::time_ordered_distortion_filename = CDBInterface::instance()->getUrl("TPC_TIMEORDERED_DISTORTION");
  // }

  // if (G4TPC::ENABLE_STATIC_DISTORTIONS || G4TPC::ENABLE_TIME_ORDERED_DISTORTIONS)
  // {
  //   auto* distortionMap = new PHG4TpcDistortion;

  //   distortionMap->set_read_phi_as_radians(G4TPC::DISTORTIONS_USE_PHI_AS_RADIANS);

  //   distortionMap->set_do_static_distortions(G4TPC::ENABLE_STATIC_DISTORTIONS);
  //   distortionMap->set_static_distortion_filename(G4TPC::static_distortion_filename);

  //   distortionMap->set_do_time_ordered_distortions(G4TPC::ENABLE_TIME_ORDERED_DISTORTIONS);
  //   distortionMap->set_time_ordered_distortion_filename(G4TPC::time_ordered_distortion_filename);

  //   distortionMap->set_do_ReachesReadout(G4TPC::ENABLE_REACHES_READOUT);
  //   distortionMap->Init();
  //   edrift->setTpcDistortion(distortionMap);
  // }

  // fudge factors to get drphi 150 microns (in mid and outer Tpc) and dz 500 microns cluster resolution
  // They represent effects not due to ideal gas properties and ideal readout plane behavior
  // defaults are 0.085 and 0.105, they can be changed here to get a different resolution
  edrift->set_double_param("added_smear_trans", 0.085);
  edrift->set_double_param("added_smear_long", 0.105);
  edrift->set_double_param("added_smear_trans", G4TPC::tpc_added_smear_trans);
  edrift->set_double_param("added_smear_long", G4TPC::tpc_added_smear_long);
  edrift->set_double_param("diffusion_long", G4TPC::ArCF4Isobutane_diffusion_long);
  edrift->set_double_param("diffusion_trans", G4TPC::ArCF4Isobutane_diffusion_trans);
  edrift->set_double_param("Ne_frac", G4TPC::ArCF4Isobutane_Ne_frac);
  edrift->set_double_param("Ar_frac", G4TPC::ArCF4Isobutane_Ar_frac);
  edrift->set_double_param("CF4_frac", G4TPC::ArCF4Isobutane_CF4_frac);
  edrift->set_double_param("N2_frac", G4TPC::ArCF4Isobutane_N2_frac);
  edrift->set_double_param("isobutane_frac", G4TPC::ArCF4Isobutane_isobutane_frac);


  edrift->registerPadPlane(padplane);
  se->registerSubsystem(edrift);

  // Tpc digitizer
  //=========
  PHG4TpcDigitizer* digitpc = new PHG4TpcDigitizer();
  digitpc->SetTpcMinLayer(G4MVTX::n_maps_layer + G4INTT::n_intt_layer);
  double ENC = 670.0;  // standard
  digitpc->SetENC(ENC);
  double ADC_threshold = 4.0 * ENC;
  digitpc->SetADCThreshold(ADC_threshold);  // 4 * ENC seems OK
  digitpc->Verbosity(0);
  std::cout << " Tpc digitizer: Setting ENC to " << ENC << " ADC threshold to " << ADC_threshold
            << " maps+Intt layers set to " << G4MVTX::n_maps_layer + G4INTT::n_intt_layer << std::endl;
  digitpc->set_skip_noise_flag(false);
  se->registerSubsystem(digitpc);

  

  // Acts geometry must be registered before LaserEventIdentifier.
  ACTSGEOM::ActsGeomInit();

  // Identify diffuse laser events from digitized TPC hits.
  G4TPC::laser_event_debug_filename = "LaserEventIdentifier_debug.root";
  Enable::TPC_VERBOSITY = 2; 
  TRACKING::reco_tpc_maxtime_sample = 425;
  Tpc_LaserEventIdentifying();

  // Cluster diffuse laser hits into LASER_CLUSTER.
  // TPC_LaserClustering();
  LaserClusterizer *laserClusterizer = new LaserClusterizer;
  laserClusterizer->Verbosity(1);
  laserClusterizer->set_max_time_samples(TRACKING::reco_tpc_maxtime_sample);
  laserClusterizer->set_adc_threshold(0);
  laserClusterizer->set_do_sequential(G4TPC::LaserClusteringSequential);
  laserClusterizer->set_do_fitting(G4TPC::laserClusterFitting);
  // laserClusterizer->set_is_simulation(true);
  se->registerSubsystem(laserClusterizer);

  TString out_DST = Form("DST_LASER_CLUSTER_SIMULATIONS.root");
  std::string theOutDST = out_DST.Data();

  Fun4AllOutputManager *out = new Fun4AllDstOutputManager("out",theOutDST);
  out->AddNode("Sync");
  out->AddNode("EventHeader");
  out->AddNode("GL1RAWHIT");
  out->AddNode("LaserEventInfo");
  out->AddNode("LASER_CLUSTER");
  se->registerOutputManager(out);

  auto centralmembraneplotter = new TpcCentralMembranePlotter;
  centralmembraneplotter->Verbosity(0);
  centralmembraneplotter->setOutputfile("centralmembraneclusters.root");
  se->registerSubsystem(centralmembraneplotter);

  se->run(1);
  se->End();

  gSystem->Exit(0);
}