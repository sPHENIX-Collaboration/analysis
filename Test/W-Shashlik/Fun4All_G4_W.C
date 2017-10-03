int
Fun4All_G4_W(const int nEvents = 100, const double momentum = 5, const char *outfile = "test.root")
{

  gSystem->Load("libfun4all");
  gSystem->Load("libg4detectors");
  gSystem->Load("libg4testbench");
  gSystem->Load("libg4eval");
  gSystem->Load("libg4histos");

  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

//  gSystem->Load("libPHPythia8.so");
//
//  PHPythia8* pythia8 = new PHPythia8();
//  // see coresoftware/generators/PHPythia8 for example config
//  pythia8->set_config_file("./phpythia8_510.cfg");
//  se->registerSubsystem(pythia8);
//
//  HepMCNodeReader *hr = new HepMCNodeReader();
//  se->registerSubsystem(hr);

//  // particle gun
//  PHG4ParticleGun *gun = new PHG4ParticleGun("PGUN");
//  //  gun->set_name("anti_proton");
//  gun->set_name("mu-");
//  //  gun->set_name("proton");
//  gun->set_vtx(-0,0,0);
//  gun->set_mom(10,0,0.);
//  se->registerSubsystem(gun);

  // Fun4All G4 module
  PHG4Reco* g4Reco = new PHG4Reco();
  // no magnetic field
  g4Reco->set_field(0);
  g4Reco->set_field_rescale(1);
  // size of the world - every detector has to fit in here
  g4Reco->SetWorldSizeX(500);
  g4Reco->SetWorldSizeY(500);
  g4Reco->SetWorldSizeZ(500);
  // shape of our world - it is a box
  g4Reco->SetWorldShape("G4BOX");
  // this is what our world is filled with
  g4Reco->SetWorldMaterial("G4_AIR");
  // Geant4 Physics list to use
  g4Reco->SetPhysicsList("QGSP_BERT");

  ///////////////////////////////////////////
  // Event generator
  //////////////////////////////////////////
  // toss low multiplicity dummy events
  PHG4SimpleEventGenerator *gen = new PHG4SimpleEventGenerator();
  gen->add_particles("e-", 1); // mu+,e+,proton,pi+,Upsilon
  // gen->add_particles("e+",5); // mu-,e-,anti_proton,pi-

  gen->set_vertex_distribution_function(PHG4SimpleEventGenerator::Uniform,
      PHG4SimpleEventGenerator::Uniform, PHG4SimpleEventGenerator::Uniform);
  gen->set_vertex_distribution_mean(0.0, 0.0, 0.0);
  gen->set_vertex_distribution_width(0.0, 0.0, 5.0);
  gen->set_vertex_size_function(PHG4SimpleEventGenerator::Uniform);
  gen->set_vertex_size_parameters(0.0, 0.0);
  gen->set_eta_range(-0.1, 0.1);
  gen->set_phi_range(-1.0 * TMath::Pi(), 1.0 * TMath::Pi());
  gen->set_p_range(momentum, momentum);
  gen->Embed(1);
  gen->Verbosity(0);
  se->registerSubsystem(gen);

  ///////////////////////////////////////////
  // W-shashlyk
  //////////////////////////////////////////
  // radiation length/layer = 0.12/0.3504 + 0.03/1.436 + 0.15/41.31 = 0.37
  // 20 Radiation length = 54 layers
  // Total length = 16 cm
  double scintiwidth = 0.15;
  double tungstenwidth = 0.12;
  double copperwidth = 0.015;
  double no_overlapp = 0.0001;
  double radius = 95;
  int Max_cemc_layer = 54;
  int absorberactive = 1;
  int overlapcheck = 1;

  PHG4CylinderSubsystem *cemc;

  for (int ilayer = 0; ilayer <= Max_cemc_layer; ilayer++)
    {

      cemc = new PHG4CylinderSubsystem("ABSORBER_CEMC", 3 * ilayer + 0);
      cemc->set_double_param("radius", radius);
      cemc->set_string_param("material", "G4_Cu");
      cemc->set_double_param("thickness", copperwidth);
      if (absorberactive)
        cemc->SetActive();
      cemc->OverlapCheck(overlapcheck);
      g4Reco->registerSubsystem(cemc);
      cemc->SuperDetector("ABSORBER_CEMC");

      radius += copperwidth;
      radius += no_overlapp;

      cemc = new PHG4CylinderSubsystem("ABSORBER_CEMC", 3 * ilayer + 1);
      cemc->set_double_param("radius", radius);
      cemc->set_string_param("material", "G4_W");
      cemc->set_double_param("thickness", tungstenwidth);
      if (absorberactive)
        cemc->SetActive();
      cemc->OverlapCheck(overlapcheck);
      g4Reco->registerSubsystem(cemc);
      cemc->SuperDetector("ABSORBER_CEMC");

      radius += tungstenwidth;
      radius += no_overlapp;

      cemc = new PHG4CylinderSubsystem("ABSORBER_CEMC", 3 * ilayer + 2);
      cemc->set_double_param("radius", radius);
      cemc->set_string_param("material", "G4_Cu");
      cemc->set_double_param("thickness", copperwidth);
      if (absorberactive)
        cemc->SetActive();
      cemc->OverlapCheck(overlapcheck);
      g4Reco->registerSubsystem(cemc);
      cemc->SuperDetector("ABSORBER_CEMC");

      radius += copperwidth;
      radius += no_overlapp;

      cemc = new PHG4CylinderSubsystem("CEMC", 3 * ilayer + 0);
      cemc->set_double_param("radius", radius);
      cemc->set_string_param("material", "G4_POLYSTYRENE");
      cemc->set_double_param("thickness", scintiwidth);
      if (absorberactive)
        cemc->SetActive();
      cemc->OverlapCheck(overlapcheck);
      g4Reco->registerSubsystem(cemc);
      cemc->SuperDetector("CEMC");

      radius += scintiwidth;
      radius += no_overlapp;
    }

  //----------------------------------------
  // BLACKHOLE

  // swallow all particles coming out of the backend of sPHENIX
  PHG4CylinderSubsystem *blackhole = new PHG4CylinderSubsystem("BH", 1);
  blackhole->set_double_param("radius", radius + 10); // add 10 cm

  blackhole->set_int_param("lengthviarapidity", 0);
  blackhole->set_double_param("length", g4Reco->GetWorldSizeZ() - no_overlapp); // make it cover the world in length
  blackhole->BlackHole();
  blackhole->set_double_param("thickness", 0.1); // it needs some thickness
  blackhole->SetActive(); // always see what leaks out
  blackhole->OverlapCheck(overlapcheck);
  g4Reco->registerSubsystem(blackhole);

  //----------------------------------------
  // FORWARD BLACKHOLEs
  // +Z
  blackhole = new PHG4CylinderSubsystem("BH_FORWARD_PLUS", 1);
  blackhole->SuperDetector("BH_FORWARD_PLUS");
  blackhole->set_double_param("radius", 0); // add 10 cm
  blackhole->set_int_param("lengthviarapidity", 0);
  blackhole->set_double_param("length", 0.1); // make it cover the world in length
  blackhole->set_double_param("place_z",
      g4Reco->GetWorldSizeZ() / 2. - 0.1 - no_overlapp);
  blackhole->BlackHole();
  blackhole->set_double_param("thickness", radius - no_overlapp); // it needs some thickness
  blackhole->SetActive(); // always see what leaks out
  blackhole->OverlapCheck(overlapcheck);
  g4Reco->registerSubsystem(blackhole);

  blackhole = new PHG4CylinderSubsystem("BH_FORWARD_NEG", 1);
  blackhole->SuperDetector("BH_FORWARD_NEG");
  blackhole->set_double_param("radius", 0); // add 10 cm
  blackhole->set_int_param("lengthviarapidity", 0);
  blackhole->set_double_param("length", 0.1); // make it cover the world in length
  blackhole->set_double_param("place_z",
      -g4Reco->GetWorldSizeZ() / 2. + 0.1 + no_overlapp);
  blackhole->BlackHole();
  blackhole->set_double_param("thickness", radius - no_overlapp); // it needs some thickness
  blackhole->SetActive(); // always see what leaks out
  blackhole->OverlapCheck(overlapcheck);
  g4Reco->registerSubsystem(blackhole);

  //----------------------------------------
  // PHG4TruthSubsystem

  PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
  g4Reco->registerSubsystem(truth);

  se->registerSubsystem(g4Reco);

  ///////////////////////////////////////////
  // Output
  ///////////////////////////////////////////

//  if (outfile)
//    {
//      Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT",outfile);
//      se->registerOutputManager(out);
//
//    }
  if (outfile)
    {
      // save a comprehensive  evaluation file
      PHG4DSTReader* ana = new PHG4DSTReader(
          string(outfile) + string("_DSTReader.root"));
      ana->set_save_particle(true);
      ana->set_load_all_particle(false);
      ana->set_load_active_particle(false);
      ana->set_save_vertex(true);
      if (nEvents > 0 && nEvents < 2)
        {
          ana->Verbosity(2);
        }
      ana->AddNode("CEMC");
      se->registerSubsystem(ana);

      gSystem->Load("libqa_modules");

      QAG4SimulationCalorimeter * qa = new QAG4SimulationCalorimeter("CEMC",
          QAG4SimulationCalorimeter::kProcessG4Hit);
      se->registerSubsystem(qa);

    }

  // input - we need a dummy to drive the event loop
  Fun4AllInputManager *in = new Fun4AllDummyInputManager("JADE");
  se->registerInputManager(in);

  //-----------------
  // Event processing
  //-----------------
  if (nEvents < 0)
    {
      return;
    }
  // if we run the particle generator and use 0 it'll run forever
  if (nEvents == 0)
    {
      cout
          << "using 0 for number of events is a bad idea when using particle generators"
          << endl;
      cout << "it will run forever, so I just return without running anything"
          << endl;
      return;
    }

  se->run(nEvents);

//  if (!readhits)
//    {
//      // Geometry export:
//      PHG4Reco *g4 = (PHG4Reco *) se->getSubsysReco("PHG4RECO");
//      g4->Dump_GDML("sPHENIX.gdml");
//    }
  //-----
  // Exit
  //-----
  if (outfile)
  {
    gSystem->Load("libqa_modules");
    QAHistManagerDef::saveQARootFile(string(outfile) + "_qa.root");

  }

  se->End();
  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);

  return;

}

void
G4Cmd(const char * cmd)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  PHG4Reco *g4 = (PHG4Reco *) se->getSubsysReco("PHG4RECO");
  g4->ApplyCommand(cmd);
}

PHG4ParticleGun *
get_gun(const char *name = "PGUN")
{
  Fun4AllServer *se = Fun4AllServer::instance();
  PHG4ParticleGun *pgun = (PHG4ParticleGun *) se->getSubsysReco(name);
  return pgun;
}

