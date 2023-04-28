// c++ includes --
#include <string>
#include <iostream>

// Fun4All includes --
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

// my includes --
#include "ledtowerbuilder/LEDTowerBuilder.h"

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libledtowerbuilder.so)

using std::cout;
using std::endl;
using std::string;

void Fun4All_LEDTowerBuilder(const int events = 10, const string &fListname = "files/fileList.list", const string &outfile = "data/LEDTowerBuilder.root") {
  // gSystem->Load("libg4dst");
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  LEDTowerBuilder *ca = new LEDTowerBuilder(outfile.c_str());
  ca->set_detector_type(LEDTowerBuilder::CEMC);
  se->registerSubsystem(ca);

  //CaloAna *caloana = new CaloAna("CALOANA",outfile);
  //caloana->Detector("HCALOUT");
  //se->registerSubsystem(caloana);

  Fun4AllInputManager *in = new Fun4AllPrdfInputManager("in");
  in->AddListFile(fListname.c_str());
  se->registerInputManager(in);

  se->run(events);
  se->End();
  se->PrintTimer();
  std::cout << "All done!" << std::endl;
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 1 || argc > 4){
        cout << "usage: ./bin/Fun4All_LEDTowerBuilder events fListname outputFile" << endl;
        cout << "events: Number of events to analyze. Default = 10." << endl;
        cout << "inputFile: Location of fileList containing prdfs. Default = files/fileList.list." << endl;
        cout << "outputFile: output root file. Default = data/LEDTowerBuilder.root." << endl;
        return 1;
    }

    UInt_t events = 10;
    string input  = "files/fileList.list";
    string output = "data/LEDTowerBuilder.root";

    if(argc >= 2) {
        events = atoi(argv[1]);
    }
    if(argc >= 3) {
        input = argv[2];
    }
    if(argc >= 4) {
        output = argv[3];
    }

    Fun4All_LEDTowerBuilder(events, input, output);

    cout << "done" << endl;
    return 0;
}
# endif
