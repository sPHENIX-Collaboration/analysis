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

R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libledtowerbuilder.so)

using std::cout;
using std::endl;
using std::string;

void Fun4All_LEDTowerBuilder(const string &fListname, const string &outfile = "LEDTowerBuilder.root", const Int_t events = 0, const Int_t skip = 0) {
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  LEDTowerBuilder *ca = new LEDTowerBuilder();
  ca->set_output(outfile);
  ca->set_skip(skip);
  se->registerSubsystem(ca);

  Fun4AllInputManager *in = new Fun4AllPrdfInputManager("in");
  in->AddFile(fListname.c_str());
  se->registerInputManager(in);

  se->run(events+skip);
  se->End();
  se->PrintTimer();
  std::cout << "All done!" << std::endl;
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
    if(argc < 2 || argc > 5){
        cout << "usage: ./bin/Fun4All_LEDTowerBuilder fListname outputFile events skip" << endl;
        cout << "inputFile: Location of input prdf." << endl;
        cout << "outputFile: output root file. Default = LEDTowerBuilder.root." << endl;
        cout << "events: Number of events to analyze. Default = 0 [all]." << endl;
        cout << "skip: Number of events to skip. Default = 0." << endl;
        return 1;
    }

    string output = "LEDTowerBuilder.root";
    Int_t events = 0;
    Int_t skip = 0;

    if(argc >= 3) {
        output = argv[2];
    }
    if(argc >= 4) {
        events = atoi(argv[3]);
    }
    if(argc >= 5) {
        skip = atoi(argv[4]);
    }

    Fun4All_LEDTowerBuilder(argv[1], output, events, skip);

    cout << "done" << endl;
    return 0;
}
# endif
