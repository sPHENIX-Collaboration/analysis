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

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <g4eval/CaloEvaluator.h>
#include <caloreco/RawClusterPositionCorrection.h>

using std::cout;
using std::endl;
using std::string;

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libg4eval.so)

void Fun4All_CaloTreeGen(const string &inputFile,
                         const string &inputG4Hits,
                         const string &output = "test.root",
                         const int nEvents = 0) {
  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc = recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2023");
  rc->set_uint64Flag("TIMESTAMP", 0);

  RawClusterPositionCorrection *clusterCorrection = new RawClusterPositionCorrection("CEMC");
  // clusterCorrection->set_UseTowerInfo(1);
  se->registerSubsystem(clusterCorrection);

  CaloEvaluator *eval = new CaloEvaluator("CEMCEVALUATOR", "CEMC", output);
  eval->set_do_gpoint_eval(false);
  eval->set_do_gshower_eval(false);
  eval->set_do_tower_eval(false);
  // set minimum cluster energy
  eval->set_reco_tracing_energy_threshold(0.5);

  // eval->Verbosity(verbosity);
  se->registerSubsystem(eval);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  // in->AddListFile(inputFile.c_str());
  in->AddFile(inputFile.c_str());
  se->registerInputManager(in);

  Fun4AllInputManager *g4Hits = new Fun4AllDstInputManager("G4Hits");
  // g4Hits->AddListFile(inputG4Hits.c_str());
  g4Hits->AddFile(inputG4Hits.c_str());
  se->registerInputManager(g4Hits);

  se->run(nEvents);
  se->End();
  se->PrintTimer();
  cout << "All done!" << endl;

  gSystem->Exit(0);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 3 || argc > 5){
        cout << "usage: ./bin/Fun4All_CaloTreeGen inputFile [output] [events]" << endl;
        cout << "inputFile: Location of fileList containing dst." << endl;
        cout << "output: name of output file." << endl;
        cout << "events: Number of events to analyze." << endl;
        return 1;
    }

    string inputFile;
    string inputG4Hits;
    string output = "test.root";
    UInt_t events = 0;

    if(argc >= 2) {
        inputFile = argv[1];
    }
    if(argc >= 3) {
        inputG4Hits = argv[2];
    }
    if(argc >= 4) {
        output = argv[3];
    }
    if(argc >= 5) {
        events = atoi(argv[4]);
    }

    Fun4All_CaloTreeGen(inputFile, inputG4Hits, output, events);

    cout << "done" << endl;
    return 0;
}
# endif
