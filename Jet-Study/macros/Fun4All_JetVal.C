// c++ includes --
#include <string>
#include <iostream>

// root includes --
#include <TSystem.h>
#include <TROOT.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>

#include <phool/recoConsts.h>

#include <g4centrality/PHG4CentralityReco.h>

#include <jetvalidation/JetValidation.h>

using std::cout;
using std::endl;
using std::string;
using std::pair;
using std::istringstream;

#include "HIJetReco.C"
#include <Calo_Calib.C>

R__LOAD_LIBRARY(libJetValidation.so)

void Fun4All_JetVal(const string &inputFile,
                    const string &outputTreeFile = "tree.root",
                    const string &outputQAFile   = "qa.root",
                    UInt_t nEvents = 0,
                    Int_t interestEvent = 0,
                    Int_t neighbors = 0)
{
  cout << "#############################" << endl;
  cout << "Run Parameters" << endl;
  cout << "inputFile: "    << inputFile << endl;
  cout << "output tree: "  << outputTreeFile << endl;
  cout << "output QA: "    << outputQAFile << endl;
  cout << "Events: "       << nEvents << endl;
  cout << "Interest Event: " << interestEvent << endl;
  cout << "Neighbors: "       << neighbors << endl;
  cout << "#############################" << endl;

  Fun4AllServer *se = Fun4AllServer::instance();

  recoConsts *rc = recoConsts::instance();

  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");

  pair<Int_t, Int_t> runseg = Fun4AllUtils::GetRunSegment(inputFile);
  Int_t runnumber = runseg.first;
  rc->set_uint64Flag("TIMESTAMP", runnumber);

  Process_Calo_Calib();

  HIJetReco();

  JetValidation *myJetVal = new JetValidation();
  myJetVal->set_outputTreeFileName(outputTreeFile);
  myJetVal->set_outputQAFileName(outputQAFile);
  if(interestEvent) {
    myJetVal->set_interestEvent(interestEvent);
    myJetVal->set_neighbors(neighbors);
  }
  se->registerSubsystem(myJetVal);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  in->AddFile(inputFile.c_str());
  se->registerInputManager(in);

  se->run(nEvents);
  se->End();

  gSystem->Exit(0);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 2 || argc > 7){
        cout << "usage: ./bin/Fun4All_JetVal inputFile [outputTreeFile] [outputQAFile] [events] [interestEvent] [neighbors]" << endl;
        cout << "inputFile: Location of fileList containing dst." << endl;
        cout << "outputTreeFile: name of output Tree file. Default: tree.root" << endl;
        cout << "outputQAFile: name of output QA file. Default: qa.root" << endl;
        cout << "events: Number of events to analyze. Default: all" << endl;
        cout << "interestEvent: specific event to filter. Default: 0" << endl;
        cout << "neighbors: number of events to save before or after the interest event. Default: 0" << endl;
        return 1;
    }

    string outputTreeFile = "tree.root";
    string outputQAFile   = "qa.root";
    UInt_t events         = 0;
    Int_t interestEvent   = 0;
    Int_t neighbors       = 0;

    if(argc >= 3) {
        outputTreeFile = argv[2];
    }
    if(argc >= 4) {
        outputQAFile = argv[3];
    }
    if(argc >= 5) {
        events = atoi(argv[4]);
    }
    if(argc >= 6) {
        interestEvent = atoi(argv[5]);
    }
    if(argc >= 7) {
        neighbors = atoi(argv[6]);
    }

    Fun4All_JetVal(argv[1], outputTreeFile, outputQAFile, events, interestEvent, neighbors);

    cout << "done" << endl;
    return 0;
}
# endif
