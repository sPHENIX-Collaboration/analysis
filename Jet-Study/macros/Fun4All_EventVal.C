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
#include <fun4all/Fun4AllUtils.h>

#include <phool/recoConsts.h>

#include <jetbackground/RetowerCEMC.h>

#include <jetvalidation/EventValidation.h>

using std::cout;
using std::endl;
using std::string;
using std::pair;
using std::istringstream;

#include <Calo_Calib.C>

R__LOAD_LIBRARY(libJetValidation.so)

void Fun4All_EventVal(const string &inputFile,
                    const string &outputTreeFile = "tree.root",
                    const string &outputQAFile   = "qa.root",
                    UInt_t nEvents = 0)
{
  cout << "#############################" << endl;
  cout << "Run Parameters" << endl;
  cout << "inputFile: "    << inputFile << endl;
  cout << "output tree: "  << outputTreeFile << endl;
  cout << "output QA: "    << outputQAFile << endl;
  cout << "Events: "       << nEvents << endl;
  cout << "#############################" << endl;

  Fun4AllServer *se = Fun4AllServer::instance();

  recoConsts *rc = recoConsts::instance();

  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");

  pair<Int_t, Int_t> runseg = Fun4AllUtils::GetRunSegment(inputFile);
  Int_t runnumber = runseg.first;
  rc->set_uint64Flag("TIMESTAMP", runnumber);

  // Process_Calo_Calib();

  RetowerCEMC *rcemc = new RetowerCEMC();
  rcemc->Verbosity(0);
  rcemc->set_towerinfo(true);
  rcemc->set_frac_cut(0.5); //fraction of retower that must be masked to mask the full retower
  rcemc->set_towerNodePrefix("TOWERINFO_CALIB");
  se->registerSubsystem(rcemc);

  EventValidation *myEventVal = new EventValidation();
  myEventVal->set_outputTreeFileName(outputTreeFile);
  myEventVal->set_outputQAFileName(outputQAFile);
  myEventVal->set_bkg_tower_neighbor_energy(0.3);
  myEventVal->set_use_zvtx(false);
  myEventVal->set_saveTree(false);
  se->registerSubsystem(myEventVal);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  in->AddFile(inputFile.c_str());
  se->registerInputManager(in);

  se->run(nEvents);
  se->End();

  gSystem->Exit(0);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 2 || argc > 5){
        cout << "usage: ./bin/Fun4All_EventVal inputFile [outputTreeFile] [outputQAFile] [events]" << endl;
        cout << "inputFile: Location of fileList containing dst." << endl;
        cout << "outputTreeFile: name of output Tree file. Default: tree.root" << endl;
        cout << "outputQAFile: name of output QA file. Default: qa.root" << endl;
        cout << "events: Number of events to analyze. Default: all" << endl;
        return 1;
    }

    string outputTreeFile = "tree.root";
    string outputQAFile   = "qa.root";
    UInt_t events         = 0;

    if(argc >= 3) {
        outputTreeFile = argv[2];
    }
    if(argc >= 4) {
        outputQAFile = argv[3];
    }
    if(argc >= 5) {
        events = atoi(argv[4]);
    }

    Fun4All_EventVal(argv[1], outputTreeFile, outputQAFile, events);

    cout << "done" << endl;
    return 0;
}
# endif
