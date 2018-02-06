int run_macro(
	      std::string infile = "G4sPHENIX.root",
	      std::string outfile = "test.root"
              )
{

  // --- need to check whether some of these are redundant, see e.g. the Makefile...
  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");
  gSystem->Load("libcalo_io.so");
  gSystem->Load("libcalo_util.so");
  //gSystem->Load("libg4eval.so");
  //gSystem->Load("libcalotrigger.so");
  //gSystem->Load("libjetbackground.so");

  gSystem->Load("libTreeMaker.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity( 0 );
  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();

  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
  //hitsin->fileopen("");
  //hitsin->fileopen("$SPHENIX/blank/macros/macros/g4simulations/output/outputDijetTrig_35.root");
  hitsin->fileopen( infile );
  se->registerInputManager(hitsin);

  JetReco *truthjetreco = new JetReco();
  TruthJetInput *tji = new TruthJetInput(Jet::PARTICLE);
  tji->add_embedding_flag( 2 );
  truthjetreco->add_input(tji);
  truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT,0.2),"AntiKt_Truth_r02");
  truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT,0.3),"AntiKt_Truth_r03");
  truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT,0.4),"AntiKt_Truth_r04");
  truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT,0.5),"AntiKt_Truth_r05");
  truthjetreco->set_algo_node("ANTIKT");
  truthjetreco->set_input_node("TRUTH");
  truthjetreco->Verbosity(0);
  se->registerSubsystem(truthjetreco);

  // HI part

  RetowerCEMC *rcemc = new RetowerCEMC();
  rcemc->Verbosity( 0 );
  se->registerSubsystem( rcemc );

  JetReco *towerjetreco = new JetReco();
  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWER_RETOWER));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWER));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWER));
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT,0.2),"AntiKt_Tower_HIRecoSeedsRaw_r02");
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(0);
  se->registerSubsystem(towerjetreco);

  DetermineTowerBackground *dtb = new DetermineTowerBackground();
  dtb->SetBackgroundOutputName("TowerBackground_Sub1");
  dtb->SetSeedType( 0 );
  dtb->Verbosity( 0 );
  se->registerSubsystem( dtb );

  CopyAndSubtractJets *casj = new CopyAndSubtractJets();
  casj->Verbosity( 0 );
  se->registerSubsystem( casj );

  DetermineTowerBackground *dtb2 = new DetermineTowerBackground();
  dtb2->SetBackgroundOutputName("TowerBackground_Sub2");
  dtb2->SetSeedType( 1 );
  dtb2->Verbosity( 0 );
  se->registerSubsystem( dtb2 );

  SubtractTowers *st = new SubtractTowers();
  st->Verbosity( 0 );
  se->registerSubsystem( st );

  JetReco *towerjetreco = new JetReco();
  // --- added by Ron (is this right?)
  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWER));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWER));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWER));
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT,0.2),"AntiKt_Tower_r02");
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT,0.3),"AntiKt_Tower_r03");
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT,0.4),"AntiKt_Tower_r04");
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT,0.5),"AntiKt_Tower_r05");
  // ---
  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWER_SUB1));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWER_SUB1));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWER_SUB1));
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT,0.2,0),"AntiKt_Tower_r02_Sub1");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT,0.3,0),"AntiKt_Tower_r03_Sub1");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT,0.4,1),"AntiKt_Tower_r04_Sub1");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT,0.5,1),"AntiKt_Tower_r05_Sub1");
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity( 0 );
  se->registerSubsystem(towerjetreco);

  TreeMaker *tt = new TreeMaker( outfile );
  tt->Verbosity(2);
  se->registerSubsystem( tt );

  se->run( 1 );

  se->End();
  std::cout << "All done" << std::endl;
  delete se;

  gSystem->Exit(0);
}
