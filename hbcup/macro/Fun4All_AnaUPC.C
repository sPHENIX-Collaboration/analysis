#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>

#include <TSystem.h>

#include <hbcup/AnaUPC.h>

R__LOAD_LIBRARY(libhbcup.so)
R__LOAD_LIBRARY(libfun4all.so)

//! Simple macro to readback previous generated simulation DSTs to analyze with `AnaUPC`
void Fun4All_AnaUPC(const int nevnt = 0, const std::string & inputfile = "G4sPHENIX.root")
{
  gSystem->Load("libg4dst");
  Fun4AllServer *se = Fun4AllServer::instance();

  AnaUPC *anaupc = new AnaUPC("anaupc", inputfile + "_anaupc.root");
  anaupc->Verbosity(0);
  anaupc->analyzeTracks(true);
  //anaupc->analyzeClusters(true);
  anaupc->analyzeTruth(false);
  se->registerSubsystem(anaupc);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTin");

  if ( inputfile.substr(inputfile.size()-5,inputfile.size()-1) == ".root" )
  {
    in->fileopen(inputfile);
  }
  if ( inputfile.substr(inputfile.size()-5,inputfile.size()-1) == ".list" )
  {
    in->AddListFile(inputfile);
  }
  se->registerInputManager(in);

  se->run(nevnt);
  se->End();
  delete se;

  cout <<"All done. Exiting..."<<endl;

  gSystem->Exit(0);
}
