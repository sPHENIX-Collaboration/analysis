using namespace std;

void FastSim_Reco_EIC(int verbosity = 0) {

  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4hough.so");

  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();

  PHG4TrackFastSim* kalman = new PHG4TrackFastSim("PHG4TrackFastSim");
  kalman->Verbosity(0);

  kalman->set_use_vertex_in_fitting(true);
  kalman->set_vertex_xy_resolution(50E-4);
  kalman->set_vertex_z_resolution(50E-4);

  kalman->set_detector_type(PHG4TrackFastSim::Vertical_Plane); // Vertical_Plane, Cylinder
  kalman->set_phi_resolution(50E-4);
  kalman->set_r_resolution(1.);

  kalman->set_pat_rec_hit_finding_eff(1.);
  kalman->set_pat_rec_noise_prob(0.);

  std::string phg4hits_names[] = {"G4HIT_SVTX", "G4HIT_SILICON_TRACKER", "G4HIT_MAPS", "G4HIT_FGEM_0", "G4HIT_FGEM_1", "G4HIT_FGEM_2", "G4HIT_FGEM_3", "G4HIT_FGEM_4", "G4HIT_EGEM_0", "G4HIT_EGEM_1", "G4HIT_EGEM_2"};
  kalman->set_phg4hits_names(phg4hits_names, 11);
  kalman->set_sub_top_node_name("SVTX");
  kalman->set_trackmap_out_name("SvtxTrackMap_FastSim");

  // Saved track states (projections)
  //std::string state_names[] = {"CEMC", "HCALIN", "HCALOUT", "FEMC", "FHCAL", "EEMC"};
  std::string state_names[] = {"CEMC", "FEMC", "FHCAL", "EEMC"};
  kalman->set_state_names(state_names, 4);

  kalman->set_fit_alg_name("KalmanFitterRefTrack");//
  kalman->set_primary_assumption_pid(-211); // -211 = pi-
  kalman->set_do_evt_display(false);

  se->registerSubsystem(kalman);

}

