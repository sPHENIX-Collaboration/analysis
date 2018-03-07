
#include <iostream>

using namespace std;

int Fun4All_G4_LeadBrick(const int nEvents = 10, const char *outfile = NULL)
{
  gSystem->Load("libfun4all");
  gSystem->Load("libg4detectors");
  gSystem->Load("libg4testbench");
  gSystem->Load("libg4eval");

  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  // particle gun
  PHG4ParticleGun *gun = new PHG4ParticleGun("PGUN");
  //  gun->set_name("anti_proton");
  gun->set_name("proton");
  //  gun->set_name("mu-");
  //  gun->set_name("proton");
  gun->set_vtx(0, 0, -100);
  gun->set_mom(0, 0, 120);
  se->registerSubsystem(gun);

  // Fun4All G4 module
  PHG4Reco *g4Reco = new PHG4Reco();
  // no magnetic field
  g4Reco->set_field(0);
  // size of the world - every detector has to fit in here
  g4Reco->SetWorldSizeX(200);
  g4Reco->SetWorldSizeY(200);
  g4Reco->SetWorldSizeZ(200);
  // shape of our world - it is a box
  g4Reco->SetWorldShape("G4BOX");
  // this is what our world is filled with
  g4Reco->SetWorldMaterial("G4_AIR");
  // Geant4 Physics list to use
  g4Reco->SetPhysicsList("FTFP_BERT");

  // our block "detector", size is in cm
  double xsize = 200.;
  double ysize = 200.;
  double zsize = 400.;
  PHG4BlockSubsystem *box = new PHG4BlockSubsystem("LeadBrick");
  box->set_double_param("size_x", 20);
  box->set_double_param("size_y", 20);
  box->set_double_param("size_z", 10);
  box->set_double_param("place_z", 0);         // shift box so we do not create particles in its center and shift by 10 so we can see the track of the incoming particle
  box->set_string_param("material", "G4_Pb");  // material of box
  box->SetActive(0);                           // if it is an active volume - save G4Hits
  g4Reco->registerSubsystem(box);

  for (int stave = 0; stave < 4; ++stave)
  {
    box = new PHG4BlockSubsystem("MVTX", stave);
    box->SuperDetector("MVTX");
    box->set_double_param("size_x", 2);
    box->set_double_param("size_y", 1);
    box->set_double_param("size_z", 50e-4);              // 50us
    box->set_double_param("place_z", 10 * (stave + 1));  // shift box so we do not create particles in its center and shift by 10 so we can see the track of the incoming particle
    box->set_string_param("material", "G4_Si");          // material of box
    box->SetActive(1);                                   // it is an active volume - save G4Hits
    g4Reco->registerSubsystem(box);
  }

  PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
  g4Reco->registerSubsystem(truth);

  se->registerSubsystem(g4Reco);

  ///////////////////////////////////////////
  // Output
  ///////////////////////////////////////////

  // save a comprehensive  evaluation file
  PHG4DSTReader *ana = new PHG4DSTReader(
      string("LeadBrick_DSTReader.root"));
  ana->set_save_particle(true);
  ana->set_load_all_particle(false);
  ana->set_load_active_particle(true);
  ana->set_save_vertex(true);
  ana->AddNode("LeadBrick");
  ana->AddNode("MVTX");
  se->registerSubsystem(ana);

  // input - we need a dummy to drive the event loop
  Fun4AllInputManager *in = new Fun4AllDummyInputManager("JADE");
  se->registerInputManager(in);

  // a quick evaluator to inspect on hit/particle/tower level

  if (nEvents > 0)
  {
    se->run(nEvents);
    // finish job - close and save output files
    se->End();
    std::cout << "All done" << std::endl;

    // cleanup - delete the server and exit
    delete se;
    gSystem->Exit(0);
  }
  return;
}

PHG4ParticleGun *get_gun(const char *name = "PGUN")
{
  Fun4AllServer *se = Fun4AllServer::instance();
  PHG4ParticleGun *pgun = (PHG4ParticleGun *) se->getSubsysReco(name);
  return pgun;
}
