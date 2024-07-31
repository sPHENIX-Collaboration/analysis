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

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>

#include <phool/recoConsts.h>

#include <calotreegen/caloTreeGen.h>

#include "Sys_Calo.C"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::pair;

R__LOAD_LIBRARY(libcaloTreeGen.so)

void Fun4All_CaloTreeGen(const string  &inputFile,
                         const string  &qaFile       = "qa.root",
                         const string  &systematic   = "CEMC",
                         const Float_t vtx_z_max     = 10, /*cm*/
                         const UInt_t  nEvents       = 0) {

  cout << "#############################" << endl;
  cout << "Run Parameters"<< endl;
  cout << "inputFile: "              << inputFile << endl;
  cout << "qaFile: "                 << qaFile << endl;
  cout << "Systematic: "             << systematic << endl;
  cout << "vtx_z_max: "              << vtx_z_max << " [cm]" << endl;
  cout << "Events: "                 << nEvents << endl;
  cout << "#############################" << endl;

  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc = recoConsts::instance();

  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2023");

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

 /**
  * Systematic Node options:
    SYST1CEMC
    SYST2CEMC
    SYST3UCEMC
    SYST3DCEMC
    SYST4CEMC
  */
  string towerNode = "TOWERINFO_CALIB_"+systematic;
  if(systematic != "CEMC") {
    towerNode = "TOWERINFO_CALIB_"+systematic;
    // Add Systematic Nodes
    Register_Tower_sys();
  }

  caloTreeGen *calo = new caloTreeGen("caloTreeGen");
  calo->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  calo->set_output_QA(qaFile);
  calo->set_vtx_z_max(vtx_z_max);
  calo->set_towerNode(towerNode);
  se->registerSubsystem(calo);

  se->run(nEvents);
  se->End();
  se->PrintTimer();
  cout << "All done!" << endl;

  gSystem->Exit(0);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 2 || argc > 6){
        cout << "usage: ./bin/Fun4All_CaloTreeGen inputFile [qaFile] [systematic] [vtx_z_max] [events]" << endl;
        cout << "inputFile: Location of fileList containing dst." << endl;
        cout << "qaFile: name of output file. Default: qa.root" << endl;
        cout << "systematic: name of systematic node. Default: CEMC " << endl;
        cout << "vtx_z_max: Maximum z-vertex [cm]. Default: 10" << endl;
        cout << "events: Number of events to analyze. Default: all" << endl;
        return 1;
    }

    string qaFile       = "qa.root";
    string diphotonFile = "diphoton.root";
    string systematic   = "CEMC";
    Float_t vtx_z_max   = 10;
    UInt_t events       = 0;

    if(argc >= 3) {
        qaFile = argv[2];
    }
    if(argc >= 4) {
        systematic = argv[3];
    }
    if(argc >= 5) {
        vtx_z_max = atof(argv[4]);
    }
    if(argc >= 6) {
        events = atoi(argv[5]);
    }

    Fun4All_CaloTreeGen(argv[1], qaFile, systematic, vtx_z_max, events);

    cout << "done" << endl;
    return 0;
}
# endif
