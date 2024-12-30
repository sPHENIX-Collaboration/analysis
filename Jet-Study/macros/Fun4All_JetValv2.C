// c++ includes --
#include <iostream>
#include <string>

// root includes --
#include <TROOT.h>
#include <TSystem.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/SubsysReco.h>

#include <calotrigger/TriggerRunInfoReco.h>

#include <phool/recoConsts.h>

#include <jetvalidation/JetValidationv2.h>

using std::cout;
using std::endl;
using std::istringstream;
using std::pair;
using std::string;

#include "Calo_Calib.C"

R__LOAD_LIBRARY(libJetValidation.so)

void Fun4All_JetValv2(const string &input_JET,
                      // const string &input_JETCALO = "",
                      const string &outputFile = "test.root",
                      UInt_t nEvents = 0)
{
  cout << "#############################" << endl;
  cout << "Run Parameters" << endl;
  cout << "input file: " << input_JET << endl;
  // cout << "input file: " << input_JETCALO << endl;
  cout << "output file: " << outputFile << endl;
  cout << "Events: " << nEvents << endl;
  cout << "#############################" << endl;

  Fun4AllServer *se = Fun4AllServer::instance();

  recoConsts *rc = recoConsts::instance();

  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");

  // pair<Int_t, Int_t> runseg = Fun4AllUtils::GetRunSegment(input_JET);
  // Int_t runnumber = runseg.first;
  Int_t runnumber = stoi(input_JET.substr(input_JET.rfind("-")+1,input_JET.rfind(".")-input_JET.rfind("-")-1));
  rc->set_uint64Flag("TIMESTAMP", runnumber);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_JET");
  // in->AddFile(input_JET.c_str());
  in->AddListFile(input_JET.c_str());
  se->registerInputManager(in);

  // Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DST_JETCALO");
  // // in2->AddFile(input_JETCALO.c_str());
  // in2->AddListFile(input_JETCALO.c_str());
  // se->registerInputManager(in2);

  TriggerRunInfoReco *triggerruninforeco = new TriggerRunInfoReco();
  se->registerSubsystem(triggerruninforeco);

  // Process_Calo_Calib();

  JetValidationv2 *myJetVal = new JetValidationv2();
  myJetVal->set_outputFile(outputFile);
  se->registerSubsystem(myJetVal);

  se->run(nEvents);
  se->End();
  se->PrintTimer();
  cout << "All done!" << endl;

  gSystem->Exit(0);
}

#ifndef __CINT__
int main(int argc, char *argv[])
{
  if (argc < 2 || argc > 4)
  {
    cout << "usage: ./bin/Fun4All_JetValv2 input_JET [outputFile] [events]" << endl;
    cout << "input_JET: Location of fileList containing dst JET." << endl;
    cout << "outputFile: name of output QA file. Default: test.root" << endl;
    cout << "events: Number of events to analyze. Default: all" << endl;
    return 1;
  }

  string outputFile = "test.root";
  UInt_t events = 0;

  if (argc >= 3)
  {
    outputFile = argv[2];
  }
  if (argc >= 4)
  {
    events = atoi(argv[3]);
  }

  Fun4All_JetValv2(argv[1], outputFile, events);

  cout << "done" << endl;
  return 0;
}
#endif
