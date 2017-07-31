void MakeShowerSize(const char *inlist, const char *outfile, const int nevnt=0)
{
  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4dst.so");
  gSystem->Load("libantisigma");
  Fun4AllServer *se = Fun4AllServer::instance();
  ShowerSize *tt = new ShowerSize("JADEs Input",outfile);
  se->registerSubsystem(tt);
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTin");
  in->AddListFile(inlist);
  se->registerInputManager(in);
  // Fun4AllOutputManager *out = new Fun4AllDstOutputManager("DSTout",outfile);
  // out->AddNode(outnode);
  // se->registerOutputManager(out);

  se->run(nevnt);
  se->End();
  delete se;
}
