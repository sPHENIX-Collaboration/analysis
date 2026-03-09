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

// jet background cut includes --
#include <jetbackgroundcut/jetBackgroundCut.h>

#include <phool/recoConsts.h>

// My Analysis includes --
#include <jetvalidation/JetValidationv2.h>
#include <jetvalidation/EventCheck.h>

using std::cout;
using std::endl;
using std::istringstream;
using std::pair;
using std::string;

#include "Calo_Calib.C"

R__LOAD_LIBRARY(libJetValidation.so)

void Fun4All_JetValv2(const string &input_JET,
                      const string &input_JETCALO,
                      const string &outputFile = "test.root",
                      UInt_t nEvents = 0)
{
  cout << "#############################" << endl;
  cout << "Run Parameters" << endl;
  cout << "input file: " << input_JET << endl;
  cout << "input file: " << input_JETCALO << endl;
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
  myEventCheck->set_zvtx_max(30); /*cm*/
  myEventCheck->set_trigger(17); /*Jet 8 GeV + MBD NS >= 1*/
  se->registerSubsystem(myEventCheck);

  Process_Calo_Calib();

  jetBackgroundCut *jocl = new jetBackgroundCut("AntiKt_unsubtracted_r04","JOCL", 0, false);
  se->registerSubsystem(jocl);

  JetValidationv2 *myJetVal = new JetValidationv2();
  myJetVal->set_outputFile(outputFile);
  se->registerSubsystem(myJetVal);

  se->run(nEvents);
  se->End();
  se->PrintTimer();
  cout << "All done!" << endl;

  gSystem->Exit(0);
  std::quick_exit(0);
}

#ifndef __CINT__
int main(int argc, char *argv[])
{
  if (argc < 3 || argc > 5)
  {
    cout << "usage: ./bin/Fun4All_JetValv2 input_JET input_JETCALO [outputFile] [events]" << endl;
    cout << "input_JET: Location of fileList containing dst JET." << endl;
    cout << "input_JETCALO: Location of fileList containing dst JETCALO." << endl;
    cout << "outputFile: name of output QA file. Default: test.root" << endl;
    cout << "events: Number of events to analyze. Default: all" << endl;
    return 1;
  }

  string outputFile = "test.root";
  UInt_t events = 0;

  if (argc >= 4)
  {
    outputFile = argv[3];
  }
  if (argc >= 5)
  {
    events = atoi(argv[4]);
  }

  Fun4All_JetValv2(argv[1], argv[2], outputFile, events);

  cout << "done" << endl;
  return 0;
}
#endif
