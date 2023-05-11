
double no_overlapp = 0.0001; // added to radii to avoid overlapping volumes
bool overlapcheck = false; // set to true if you want to check for overlaps

void G4Init(bool do_magnet = true,
            bool do_pipe = true,
            bool do_ExtendedIR = true
            ) {

  // load detector/material macros and execute Init() function
  if (do_pipe)
    {
      gROOT->LoadMacro("G4_Pipe.C");
      PipeInit();
    }

  if (do_magnet)
    {
      gROOT->LoadMacro("G4_Magnet.C");
      MagnetInit();
    }

  if (do_ExtendedIR)
    {
      gROOT->LoadMacro("G4_IR_EIC.C");
      IRInit();
    }

}


int G4Setup(const int absorberactive = 0,
            const string &field ="1.5",
            const EDecayType decayType = TPythia6Decayer::kAll,
            const bool do_magnet = true,
            const bool do_pipe = true,
            const bool do_ExtendedIR = true,
            const float magfield_rescale = 1.0) {

  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4testbench.so");

  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();

  PHG4Reco* g4Reco = new PHG4Reco();
  g4Reco->set_rapidity_coverage(1.1); // according to drawings

  /* Set World Volume material to G4Galactic for beam line studies
   * (beam would be inside vacuum tubes) */
  g4Reco->SetWorldMaterial("G4_Galactic");

  if (decayType != TPythia6Decayer::kAll) {
    g4Reco->set_force_decay(decayType);
  }

  double fieldstrength;
  istringstream stringline(field);
  stringline >> fieldstrength;
  if (stringline.fail()) { // conversion to double fails -> we have a string

    if (field.find("sPHENIX.root") != string::npos) {
      g4Reco->set_field_map(field, 1);
    } else {
      g4Reco->set_field_map(field, 2);
    }
  } else {
    g4Reco->set_field(fieldstrength); // use const soleniodal field
  }
  g4Reco->set_field_rescale(magfield_rescale);

  double radius = 0.;

  //----------------------------------------
  // PIPE
  if (do_pipe) radius = Pipe(g4Reco, radius, absorberactive);

  //----------------------------------------
  // MAGNET

  if (do_magnet) radius = Magnet(g4Reco, radius, 0, absorberactive);

  //----------------------------------------
  // Extended IR
  if ( do_ExtendedIR )
    IRSetup(g4Reco);


  // discs to track very forward protons
  PHG4CylinderSubsystem *fwd_disc;
  for ( unsigned i = 0; i < 900; i++ )
    {
      fwd_disc = new PHG4CylinderSubsystem("FWDDISC", i);
      fwd_disc->set_int_param("lengthviarapidity",0);
      fwd_disc->set_double_param("length",1);
      fwd_disc->set_double_param("radius",0);
      fwd_disc->set_double_param("thickness",200.0);
      fwd_disc->set_double_param("place_z",450+i*10.0); // cm
      fwd_disc->set_string_param("material","G4_Galactic");
      fwd_disc->SetActive(true);
      fwd_disc->SuperDetector("FWDDISC");
      fwd_disc->OverlapCheck(overlapcheck);
      g4Reco->registerSubsystem(fwd_disc);
    }

  /* Add truth system */
  PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
  g4Reco->registerSubsystem(truth);

  /* add reco to server */
  se->registerSubsystem( g4Reco );
}
