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

void Fun4All_LEDTowerBuilder(const int events, const string &input, const string &outDir, const string &outName) {
  // gSystem->Load("libg4dst");
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  LEDTowerBuilder *ca = new LEDTowerBuilder((outDir + "/" + outName + "-CEMC.root").c_str());
  ca->set_detector_type(LEDTowerBuilder::CEMC);
  se->registerSubsystem(ca);

  LEDTowerBuilder *ca_hcal_in = new LEDTowerBuilder((outDir + "/" + outName + "-HCALIN.root").c_str());
  ca_hcal_in->set_detector_type(LEDTowerBuilder::HCALIN);
  se->registerSubsystem(ca_hcal_in);

  LEDTowerBuilder *ca_hcal_out = new LEDTowerBuilder((outDir + "/" + outName + "-HCALOUT.root").c_str());
  ca_hcal_out->set_detector_type(LEDTowerBuilder::HCALOUT);
  se->registerSubsystem(ca_hcal_out);

  //CaloAna *caloana = new CaloAna("CALOANA",outfile);
  //caloana->Detector("HCALOUT");
  //se->registerSubsystem(caloana);

  Fun4AllInputManager *in = new Fun4AllPrdfInputManager("in");
  in->AddListFile(input.c_str());
  se->registerInputManager(in);

  se->run(events);
  se->End();
  se->PrintTimer();
  std::cout << "All done!" << std::endl;
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 1 || argc > 5){
        cout << "usage: ./bin/Fun4All_LEDTowerBuilder events input outDir outName" << endl;
        cout << "events: Number of events to analyze." << endl;
        cout << "inputFile: Location of fileList containing prdfs." << endl;
        cout << "outputFile: output root file." << endl;
        return 1;
    }

    UInt_t events;
    string input;
    string outDir;
    string outName;

    if(argc >= 2) {
        events = atoi(argv[1]);
    }
    if(argc >= 3) {
        input = argv[2];
    }
    if(argc >= 4) {
        outDir = argv[3];
    }
    if(argc >= 5) {
        outName = argv[4];
    }

    Fun4All_LEDTowerBuilder(events, input, outDir, outName);

    cout << "done" << endl;
    return 0;
}
# endif
