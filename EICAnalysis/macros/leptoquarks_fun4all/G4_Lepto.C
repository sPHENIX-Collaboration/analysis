int G4_Lepto(string filename)
{
  cout << "G4 Lepto Loaded!!!" << endl;

  cout << "**************************************" << endl;
  Fun4AllServer *se = Fun4AllServer::instance();

  Leptoquarks *truthana = new Leptoquarks(filename+"_TruthEvent.root");
  //  se->registerSubsystem( truthana );

  LeptoquarksReco *mcana_r05 = new LeptoquarksReco(filename+"_r05.root");
  LeptoquarksReco *mcana_r10 = new LeptoquarksReco(filename+"_r10.root");

  LeptoquarksReco *mcana_r05truth = new LeptoquarksReco(filename+"_r05truth.root");
  LeptoquarksReco *mcana_r10truth = new LeptoquarksReco(filename+"_r10truth.root");

  mcana_r05->set_reco_jet_collection("AntiKt_Tower_r05");
  mcana_r05->set_save_towers( true );
  mcana_r05->set_save_tracks( true );

  mcana_r10->set_reco_jet_collection("AntiKt_Tower_r10");
  mcana_r10->set_save_towers( true );
  mcana_r10->set_save_tracks( true );

  mcana_r05truth->set_reco_jet_collection("AntiKt_Truth_r05");
  mcana_r05truth->set_save_towers( true );
  mcana_r05truth->set_save_tracks( true );

  mcana_r10truth->set_reco_jet_collection("AntiKt_Truth_r10");
  mcana_r10truth->set_save_towers( true );
  mcana_r10truth->set_save_tracks( true );

  se->registerSubsystem( mcana_r05 );
//  se->registerSubsystem( mcana_r10 );
//  se->registerSubsystem( mcana_r05truth );
//  se->registerSubsystem( mcana_r10truth );
  cout << "**************************************" << endl;

  return 0;
}
