// c++ includes --
#include <iostream>
#include <string>

// root includes --
#include <TROOT.h>
#include <TSystem.h>

// fun4all includes --
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/SubsysReco.h>

#include <phool/recoConsts.h>

// My Analysis includes --
#include <jetvalidation/EventValidation.h>
#include <jetvalidation/EventCheck.h>

using std::cout;
using std::endl;
using std::istringstream;
using std::pair;
using std::string;

#include "Calo_Calib-EventVal.C"

R__LOAD_LIBRARY(libJetValidation.so)

void Fun4All_EventVal(const string &input_JET,
                      const string &input_JETCALO,
                      const string &eventList,
                      const string &outputFile = "test.root",
                      UInt_t nEvents = 0)
{
  cout << "#############################" << endl;
  cout << "Run Parameters" << endl;
  cout << "input file: " << input_JET << endl;
  cout << "input file: " << input_JETCALO << endl;
  cout << "Event List: " << eventList << endl;
  cout << "output file: " << outputFile << endl;
  cout << "Events: " << nEvents << endl;
  cout << "#############################" << endl;

  Fun4AllServer *se = Fun4AllServer::instance();

  recoConsts *rc = recoConsts::instance();

  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");

  Int_t runnumber = (input_JET.find(".root") != string::npos) ? Fun4AllUtils::GetRunSegment(input_JET).first
                                                              : stoi(input_JET.substr(input_JET.rfind("-")+1,input_JET.rfind(".")-input_JET.rfind("-")-1));
  rc->set_uint64Flag("TIMESTAMP", runnumber);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_JET");
  if(input_JET.find(".root") != string::npos) in->AddFile(input_JET.c_str());
  else                                        in->AddListFile(input_JET.c_str());
  se->registerInputManager(in);

  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DST_JETCALO");
  if(input_JETCALO.find(".root") != string::npos) in2->AddFile(input_JETCALO.c_str());
  else                                            in2->AddListFile(input_JETCALO.c_str());
  se->registerInputManager(in2);

  EventCheck *myEventCheck = new EventCheck();
  myEventCheck->set_eventList(eventList);
  se->registerSubsystem(myEventCheck);

  Process_Calo_Calib();

  EventValidation *myEventVal = new EventValidation();
  myEventVal->set_outputFile(outputFile);
  se->registerSubsystem(myEventVal);

  se->run();
  se->End();
  se->PrintTimer();
  cout << "All done!" << endl;

  gSystem->Exit(0);
  std::quick_exit(0);
}

#ifndef __CINT__
int main(int argc, char *argv[])
{
  if (argc < 4 || argc > 6)
  {
    cout << "usage: ./bin/Fun4All_EventVal input_JET input_JETCALO eventList [outputFile] [events]" << endl;
    cout << "input_JET: Location of fileList containing dst JET." << endl;
    cout << "input_JETCALO: Location of fileList containing dst JETCALO." << endl;
    cout << "eventList: List of events of interest to save." << endl;
    cout << "outputFile: name of output QA file. Default: test.root" << endl;
    cout << "events: Number of events to analyze. Default: all" << endl;
    return 1;
  }

  string outputFile = "test.root";
  UInt_t events = 0;

  if (argc >= 5)
  {
    outputFile = argv[4];
  }
  if (argc >= 6)
  {
    events = atoi(argv[5]);
  }

  Fun4All_EventVal(argv[1], argv[2], argv[3], outputFile, events);

  cout << "done" << endl;
  return 0;
}
#endif
