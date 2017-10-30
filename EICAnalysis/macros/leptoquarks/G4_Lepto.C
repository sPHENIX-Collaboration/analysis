int G4_Lepto(string filename)
{
  cout << "G4 Lepto Loaded!!!" << endl;

  cout << "**************************************" << endl;
  Fun4AllServer *se = Fun4AllServer::instance();
  LeptoquarksReco *mcana2 = new LeptoquarksReco(filename);

  //mcana2->set_beam_energies(10,250);
  se->registerSubsystem( mcana2 );
  cout << "**************************************" << endl;

  return 0;
}
