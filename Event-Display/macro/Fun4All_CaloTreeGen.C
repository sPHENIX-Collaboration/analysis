// c++ includes --
#include <string>
#include <iostream>

// root includes --
#include <TSystem.h>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <calotreegen/caloTreeGen.h>

using std::cout;
using std::endl;
using std::string;

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcaloTreeGen.so)

void Fun4All_CaloTreeGen(const string &file,
                         const int event = 0,
                         const int run = 0,
                         const float tow_cemc_min = 0,
                         const float tow_hcalin_min = 0,
                         const float tow_hcalout_min = 0,
                         const string& eventFile = "test.json",
                         const string &output = "commissioning.root") {

  Fun4AllServer *se = Fun4AllServer::instance();
  // recoConsts *rc = recoConsts::instance();

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  // in->AddListFile(file);
  in->AddFile(file);
  se->registerInputManager(in);

  caloTreeGen *calo = new caloTreeGen(output, eventFile, event, run, tow_cemc_min, tow_hcalin_min, tow_hcalout_min);
  se->registerSubsystem(calo);

  // se->skip(skip);
  se->run(event+1);
  se->End();
  se->PrintTimer();
  std::cout << "All done!" << std::endl;

  gSystem->Exit(0);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 2 || argc > 9){
        cout << "usage: ./bin/Fun4All_CaloTreeGen inputFile event run tow_cemc_min tow_hcalin_min tow_hcalout_min eventFile outputFile" << endl;
        cout << "inputFile: Location of fileList or file containing dst." << endl;
        cout << "event: Event number to analyze." << endl;
        cout << "run: Run number." << endl;
        cout << "tow_cemc_min: Minimum tower energy for CEMC." << endl;
        cout << "tow_hcalin_min: Minimum tower energy for HCALIN." << endl;
        cout << "tow_hcalout_min: Minimum tower energy for HCALOUT." << endl;
        cout << "eventFile: name of event File." << endl;
        cout << "outputFile: name of output file." << endl;
        return 1;
    }

    string inputFile;
    UInt_t event = 0;
    UInt_t run = 0;
    Float_t tow_cemc_min = 0;
    Float_t tow_hcalin_min = 0;
    Float_t tow_hcalout_min = 0;
    string eventFile = "test.json";
    string outputFile = "commissioning.root";

    if(argc >= 2) {
        inputFile = argv[1];
    }
    if(argc >= 3) {
        event = atoi(argv[2]);
    }
    if(argc >= 4) {
        run = atoi(argv[3]);
    }
    if(argc >= 5) {
        tow_cemc_min = atof(argv[4]);
    }
    if(argc >= 6) {
        tow_hcalin_min = atof(argv[5]);
    }
    if(argc >= 7) {
        tow_hcalout_min = atof(argv[6]);
    }
    if(argc >= 8) {
        eventFile = argv[7];
    }
    if(argc >= 9) {
        outputFile = argv[8];
    }

    Fun4All_CaloTreeGen(inputFile, event, run, tow_cemc_min, tow_hcalin_min, tow_hcalout_min, eventFile, outputFile);

    cout << "done" << endl;
    return 0;
}
# endif
