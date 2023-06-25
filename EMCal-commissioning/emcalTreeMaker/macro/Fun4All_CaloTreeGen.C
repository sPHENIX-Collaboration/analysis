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

#include "caloreco/RawClusterPositionCorrection.h"

using std::cout;
using std::endl;
using std::string;

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcaloTreeGen.so)
R__LOAD_LIBRARY(libcalo_reco.so)

void Fun4All_CaloTreeGen(const int nEvents = 0, const string &listFile = "fileList_withGeo_timingCut_Template.list", const string &inName = "commissioning.root")
{
  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc = recoConsts::instance();

  RawClusterPositionCorrection *clusterCorrection = new RawClusterPositionCorrection("CEMC");
  clusterCorrection->set_UseTowerInfo(1); // to use towerinfo objects rather than old RawTower
  // clusterCorrection->Verbosity(1);
  se->registerSubsystem(clusterCorrection);

  caloTreeGen *calo = new caloTreeGen(inName.c_str());
  calo -> setClusters(1);
  calo -> setFineClusters(1);
  se->registerSubsystem(calo);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  in->AddListFile(listFile.c_str());
  

  se->registerInputManager(in);

  se->run(nEvents);
  se->End();
  se->PrintTimer();
  std::cout << "All done!" << std::endl;

  gSystem->Exit(0);
}


# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 1 || argc > 5){
        cout << "usage: ./bin/Fun4All_CaloTreeGen events listFile inName" << endl;
        cout << "events: Number of events to analyze." << endl;
        cout << "listFile: Location of fileList containing dst." << endl;
        cout << "inName: name of output file." << endl;
        return 1;
    }

    UInt_t events = 0;
    string listFile;
    string inName = "commissioning.root";

    if(argc >= 2) {
        events = atoi(argv[1]);
    }
    if(argc >= 3) {
        listFile = argv[2];
    }
    if(argc >= 4) {
        inName = argv[3];
    }

    Fun4All_CaloTreeGen(events, listFile, inName);

    cout << "done" << endl;
    return 0;
}
# endif
