
void Jet_Reco(int verbosity = 0) {

  gSystem->Load("libg4jets.so");
  
  Fun4AllServer *se = Fun4AllServer::instance();

  // truth particle level jets
  JetReco *truthjetreco = new JetReco();
  truthjetreco->add_input(new TruthJetInput(Jet::PARTICLE));
  truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT,0.2),"AntiKt_Truth_r02");
  truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT,0.3),"AntiKt_Truth_r03");
  truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT,0.4),"AntiKt_Truth_r04");
  truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT,0.5),"AntiKt_Truth_r05");
  truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT,0.6),"AntiKt_Truth_r06");
  truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT,0.7),"AntiKt_Truth_r07");
  truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT,0.8),"AntiKt_Truth_r08");
  truthjetreco->set_algo_node("ANTIKT");
  truthjetreco->set_input_node("TRUTH");
  truthjetreco->Verbosity(verbosity);
  se->registerSubsystem(truthjetreco);


  return; 
}

void Jet_Eval(std::string outfilename = "g4jets_eval.root",
	      int verbosity = 0) {

  gSystem->Load("libg4eval.so");
  
  Fun4AllServer *se = Fun4AllServer::instance();

  JetEvaluator* eval = new JetEvaluator("JETEVALUATOR",
   					"AntiKt_Tower_r03",
   					"AntiKt_Truth_r03",
   					outfilename);
  eval->Verbosity(verbosity);
  se->registerSubsystem(eval);
  
  return;
}
