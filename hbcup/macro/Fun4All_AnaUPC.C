#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>

#include <TSystem.h>

#include <hbcup/AnaUPC.h>

R__LOAD_LIBRARY(libhbcup.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4dst.so)

//! Simple macro to analyze simDSTs or real DSTs for UPC analysis
void Fun4All_AnaUPC(const int nevnt = 0,
    const std::string& inputfile = "G4sPHENIX.root",
    std::string outputfile = "")
{
  Fun4AllServer *se = Fun4AllServer::instance();

  if ( outputfile.size() == 0 )
  {
    outputfile = inputfile + "_anaupc.root";
  }

  AnaUPC *anaupc = new AnaUPC("anaupc", outputfile);
  //anaupc->Verbosity(10);
  anaupc->analyzeTracks(true);
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

