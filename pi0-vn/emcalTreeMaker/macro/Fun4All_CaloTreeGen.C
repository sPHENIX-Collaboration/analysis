// c++ includes --
#include <string>
#include <iostream>

// root includes --
#include <TSystem.h>
#include <TROOT.h>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

// #include <phool/PHRandomSeed.h>
// #include <phool/recoConsts.h>

#include <calotreegen/caloTreeGen.h>

using std::cout;
using std::endl;
using std::string;

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcaloTreeGen.so)

void Fun4All_CaloTreeGen(const string &inputFile,
                         const string &qaFile = "qa.root",
                         const string &ntpFile = "ntp.root",
                         const int nEvents = 0) {
  Fun4AllServer *se = Fun4AllServer::instance();
  // recoConsts *rc = recoConsts::instance();

  caloTreeGen *calo = new caloTreeGen(qaFile.c_str(), ntpFile.c_str());
  calo->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  calo->set_clusterE_min(0.5);
  calo->set_cluster_chi_max(4);
  calo->set_do_pi0_ana(false);
  se->registerSubsystem(calo);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  // in->AddListFile(inputFile.c_str());
  in->AddFile(inputFile.c_str());
  se->registerInputManager(in);

  se->run(nEvents);
  se->End();
  se->PrintTimer();
  std::cout << "All done!" << std::endl;

  gSystem->Exit(0);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 2 || argc > 5){
        cout << "usage: ./bin/Fun4All_CaloTreeGen inputFile qaFile ntpFile events" << endl;
        cout << "events: Number of events to analyze." << endl;
        cout << "inputFile: Location of fileList containing dst." << endl;
        cout << "qaFile: name of output file." << endl;
        cout << "ntpFile: name of output file." << endl;
        return 1;
    }

    string inputFile;
    string qaFile = "qa.root";
    string ntpFile = "ntp.root";
    UInt_t events = 0;

    if(argc >= 2) {
        inputFile = argv[1];
    }
    if(argc >= 3) {
        qaFile = argv[2];
    }
    if(argc >= 4) {
        ntpFile = argv[3];
    }
    if(argc >= 5) {
        events = atoi(argv[4]);
    }

    Fun4All_CaloTreeGen(inputFile, qaFile, ntpFile, events);

    cout << "done" << endl;
    return 0;
}
# endif
