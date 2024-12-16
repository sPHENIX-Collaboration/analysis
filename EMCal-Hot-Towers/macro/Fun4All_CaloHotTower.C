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
#include <fun4all/Fun4AllUtils.h>

#include <caloreco/CaloTowerStatus.h>

#include <calotrigger/TriggerRunInfoReco.h>

#include <phool/recoConsts.h>

#include <calohottower/CaloHotTower.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::pair;

R__LOAD_LIBRARY(libCaloHotTower.so)

void Fun4All_CaloHotTower(const string  &inputFile,
                          const string  &hotTowerFile,
                          const string  &outputFile = "test.root",
                          UInt_t nEvents = 0) {

  cout << "#############################" << endl;
  cout << "Run Parameters" << endl;
  cout << "inputFile: "    << inputFile << endl;
  cout << "hotTowerFile: " << hotTowerFile << endl;
  cout << "outputFile: "   << outputFile << endl;
  cout << "Events: "       << nEvents << endl;
  cout << "#############################" << endl;

  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc    = recoConsts::instance();

  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");

  pair<Int_t, Int_t> runseg = Fun4AllUtils::GetRunSegment(inputFile);
  Int_t runnumber = runseg.first;
  rc->set_uint64Flag("TIMESTAMP", runnumber);

  // //! Quiet mode. Only output critical messages. Intended for batch production mode.
  // VERBOSITY_QUIET = 0,

  // //! Output some useful messages during manual command line running
  // VERBOSITY_SOME = 1,

  // //! Output more messages
  // VERBOSITY_MORE = 2,

  // //! Output even more messages
  // VERBOSITY_EVEN_MORE = 3,

  // //! Output a lot of messages
  // VERBOSITY_A_LOT = 4,

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  // in->AddListFile(inputFile.c_str());
  in->AddFile(inputFile.c_str());
  se->registerInputManager(in);

  TriggerRunInfoReco* triggerruninforeco = new TriggerRunInfoReco();
  se->registerSubsystem(triggerruninforeco);

  // need to set the isBadChi2 flag for the towers
  CaloTowerStatus *statusEMC = new CaloTowerStatus("CEMCSTATUS");
  statusEMC->set_detector_type(CaloTowerDefs::CEMC);
  statusEMC->set_time_cut(1);
  se->registerSubsystem(statusEMC);

  CaloHotTower *calo = new CaloHotTower();
  calo->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  calo->setOutputFile(outputFile);
  calo->setHotTowerIndexFile(hotTowerFile);

  se->registerSubsystem(calo);

  se->run(nEvents);
  se->End();
  se->PrintTimer();
  cout << "All done!" << endl;

  gSystem->Exit(0);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 3 || argc > 5){
        cout << "usage: ./bin/Fun4All_CaloHotTower inputFile hotTowerFile [outputFile] [events]" << endl;
        cout << "inputFile: Location of fileList containing dst." << endl;
        cout << "hotTowerFile: Location of input hot tower list file." << endl;
        cout << "outputFile: name of output file. Default: diphoton.root" << endl;
        cout << "events: Number of events to analyze. Default: all" << endl;
        return 1;
    }

    string outputFile = "test.root";
    UInt_t events     = 0;

    if(argc >= 4) {
        outputFile = argv[3];
    }
    if(argc >= 5) {
        events = atoi(argv[4]);
    }

    Fun4All_CaloHotTower(argv[1], argv[2], outputFile, events);

    cout << "done" << endl;
    return 0;
}
# endif
