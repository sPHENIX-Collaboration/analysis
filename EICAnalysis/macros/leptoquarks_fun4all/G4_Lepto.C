int G4_Lepto(string filename)
{
  cout << "G4 Lepto Loaded!!!" << endl;

  cout << "**************************************" << endl;
  Fun4AllServer *se = Fun4AllServer::instance();
  LeptoquarksReco *mcana1 = new LeptoquarksReco(filename+"_r05.root");
  LeptoquarksReco *mcana2 = new LeptoquarksReco(filename+"_r06.root");
  LeptoquarksReco *mcana3 = new LeptoquarksReco(filename+"_r07.root");
  LeptoquarksReco *mcana4 = new LeptoquarksReco(filename+"_r08.root");

  
  mcana1->set_reco_jet_collection("AntiKt_Tower_r05");
  mcana2->set_reco_jet_collection("AntiKt_Tower_r06");
  mcana3->set_reco_jet_collection("AntiKt_Tower_r07");
  mcana4->set_reco_jet_collection("AntiKt_Tower_r08");
  
  se->registerSubsystem( mcana1 );
  se->registerSubsystem( mcana2 );
  se->registerSubsystem( mcana3 );
  se->registerSubsystem( mcana4 );
  cout << "**************************************" << endl;

  return 0;
}
