// c++ includes --
#include <string>
#include <iostream>

// root includes --
#include <TSystem.h>
#include <TROOT.h>
#include <TF1.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllUtils.h>

#include <caloreco/CaloTowerStatus.h>

#include <phool/recoConsts.h>

#include <ffamodules/CDBInterface.h>

#include <globalvertex/GlobalVertexReco.h>

#include <mbd/MbdReco.h>

using std::cout;
using std::endl;
using std::string;
using std::pair;
using std::istringstream;

#include "Calo_Calib.C"

void Fun4All_CaloCDB(const string &inputFile, Int_t nEvents = 0)
{
  cout << "#############################" << endl;
  cout << "Run Parameters" << endl;
  cout << "inputFile: "    << inputFile << endl;
  cout << "Events: "       << nEvents << endl;
  cout << "#############################" << endl;

  Fun4AllServer *se = Fun4AllServer::instance();

  recoConsts *rc = recoConsts::instance();

  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");

  pair<ULong_t, Int_t> runseg = Fun4AllUtils::GetRunSegment(inputFile);
  ULong_t runnumber = runseg.first;
  rc->set_uint64Flag("TIMESTAMP", runnumber);

  // Geometry
  cout << "Adding Geometry file" << endl;
  Fun4AllInputManager* intrue2 = new Fun4AllRunNodeInputManager("DST_GEO");
  string geoLocation = CDBInterface::instance()->getUrl("calo_geo");
  intrue2->AddFile(geoLocation);
  se->registerInputManager(intrue2);

  // MBD/BBC Reconstruction
  MbdReco* mbdreco = new MbdReco();
  se->registerSubsystem(mbdreco);

  // Official vertex storage
  GlobalVertexReco* gvertex = new GlobalVertexReco();
  se->registerSubsystem(gvertex);

  Process_Calo_Calib();

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  in->AddFile(inputFile.c_str());
  se->registerInputManager(in);

  se->run(nEvents);
  se->End();
  // se->PrintTimer();

  gSystem->Exit(0);
}

# ifndef __CINT__
Int_t main(Int_t argc, const char* const argv[]) {
    if(argc < 2 || argc > 3){
        cout << "usage: ./bin/Fun4All_CaloCDB inputFile [events]" << endl;
        cout << "inputFile: Location of fileList containing dst." << endl;
        cout << "events: Number of events to analyze. Default: all" << endl;
        return 1;
    }

    Int_t events = 0;

    if(argc >= 3) {
        events = atoi(argv[2]);
    }

    Fun4All_CaloCDB(argv[1], events);

    cout << "done" << endl;
    return 0;
}
# endif
