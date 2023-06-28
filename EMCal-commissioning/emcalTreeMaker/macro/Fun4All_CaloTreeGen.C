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
// #include <phool/recoConsts.h>

#include <calotreegen/caloTreeGen.h>

#include "caloreco/RawClusterPositionCorrection.h"

using std::cout;
using std::endl;
using std::string;

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcaloTreeGen.so)
R__LOAD_LIBRARY(libcalo_reco.so)

void Fun4All_CaloTreeGen(const int nEvents = 0, const string &listFile = "fileList_withGeo_timingCut_Template.list", const string &outputFile = "commissioning.root")
{
  Fun4AllServer *se = Fun4AllServer::instance();
  // recoConsts *rc = recoConsts::instance();

  RawClusterPositionCorrection *clusterCorrection = new RawClusterPositionCorrection("CEMC");
  clusterCorrection->set_UseTowerInfo(1); // to use towerinfo objects rather than old RawTower

  //! Quiet mode. Only output critical messages. Intended for batch production mode.
  // VERBOSITY_QUIET = 0,

  //! Output some useful messages during manual command line running
  // VERBOSITY_SOME = 1,

  //! Output more messages
  // VERBOSITY_MORE = 2,

  //! Output even more messages
  // VERBOSITY_EVEN_MORE = 3,

  //! Output a lot of messages
  // VERBOSITY_A_LOT = 4,
  clusterCorrection->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(clusterCorrection);

  caloTreeGen *calo = new caloTreeGen(outputFile.c_str());
  calo -> setClusters(1);
  calo -> setFineClusters(0);
  calo -> Verbosity(Fun4AllBase::VERBOSITY_SOME);
  se->registerSubsystem(calo);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  in->AddListFile(listFile.c_str());
  se->registerInputManager(in);

  // Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", outputFile);
  // se->registerOutputManager(out);

  se->run(nEvents);
  se->End();
  se->PrintTimer();
  std::cout << "All done!" << std::endl;

  gSystem->Exit(0);
}


# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 1 || argc > 5){
        cout << "usage: ./bin/Fun4All_CaloTreeGen events listFile outputFile" << endl;
        cout << "events: Number of events to analyze." << endl;
        cout << "listFile: Location of fileList containing dst." << endl;
        cout << "outputFile: name of output file." << endl;
        return 1;
    }

    UInt_t events = 0;
    string listFile;
    string outputFile = "commissioning.root";

    if(argc >= 2) {
        events = atoi(argv[1]);
    }
    if(argc >= 3) {
        listFile = argv[2];
    }
    if(argc >= 4) {
        outputFile = argv[3];
    }

    Fun4All_CaloTreeGen(events, listFile, outputFile);

    cout << "done" << endl;
    return 0;
}
# endif
