int Fun4All_G4_Calos(const int nEvents = 10, const char * outfile = NULL)
{

  gSystem->Load("libfun4all");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4testbench.so");

  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  // PHG4ParticleGenerator generates particle
  // distributions in eta/phi/mom range
  PHG4ParticleGenerator *gen = new PHG4ParticleGenerator("PGENERATOR");
  int uniqueseed = TRandom3(0).GetSeed();
  recoConsts *rc = recoConsts::instance();
  rc->set_IntFlag("RANDOMSEED", 12345);
  //  gen->set_seed(uniqueseed);
  //gen->set_name("gamma");
  gen->set_name("proton");
  gen->set_vtx(0, 0, 0);
  gen->set_eta_range(-0.05, +0.05);
  gen->set_mom_range(0.4, 10.0);
  gen->set_phi_range(0., 90. / 180.*TMath::Pi()); // 0-90 deg
  //  se->registerSubsystem(gen);
  PHG4ParticleGun *gun = new PHG4ParticleGun();
  //  gun->set_name("anti_proton");
  gun->set_name("anti_sigma+");
  gun->set_vtx(0, 0, 0);
  gun->set_mom(10, 0, 0.01);
  // gun->AddParticle("geantino",1.7776,-0.4335,0.);
  // gun->AddParticle("geantino",1.7709,-0.4598,0.);
  // gun->AddParticle("geantino",2.5621,0.60964,0.);
  // gun->AddParticle("geantino",1.8121,0.253,0.);
  se->registerSubsystem(gun);


  PHG4Reco* g4Reco = new PHG4Reco();
  g4Reco->SetWorldMaterial("G4_Galactic");
//  g4Reco->set_field(1.5); // 1.5 T solenoidal field 

  double calorad[3] = {90.,116.,178.};
  PHG4CylinderSubsystem *cyl;
  // here is our silicon:
  for (int ilayer = 0; ilayer < 3; ilayer++)
    {
      cyl = new PHG4CylinderSubsystem("CALO", ilayer);
      cyl->set_double_param("radius",calorad[ilayer]);
      cyl->set_string_param("material","G4_Galactic");
      cyl->set_double_param("thickness",0.001);
      cyl->SetActive();
      cyl->SuperDetector("CALO"); // combine all layers into one node
      g4Reco->registerSubsystem( cyl );
    }
  PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
  g4Reco->registerSubsystem(truth);

  se->registerSubsystem( g4Reco );
  if (0)
    {
      // save a comprehensive  evaluation file
      PHG4DSTReader* ana = new PHG4DSTReader(
          string(outfile) + string("_DSTReader.root"));
      ana->set_save_particle(true);
      ana->set_load_all_particle(false);
      ana->set_load_active_particle(true);
      ana->set_save_vertex(true);
      if (nEvents > 0 && nEvents < 2)
        {
          ana->Verbosity(2);
        }
      ana->AddNode("box_0");
      se->registerSubsystem(ana);
    }

  if (outfile)
    {
      Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT",outfile);
      se->registerOutputManager(out);
    }
  Fun4AllInputManager *in = new Fun4AllDummyInputManager( "JADE");
  se->registerInputManager( in );


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

}

PHG4ParticleGenerator *get_gen(const char *name="PGENERATOR")
{
  Fun4AllServer *se = Fun4AllServer::instance();
  PHG4ParticleGenerator *pgun = (PHG4ParticleGenerator *) se->getSubsysReco(name);
  return pgun;
}
