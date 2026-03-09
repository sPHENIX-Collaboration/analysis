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

#include <jetvalidation/JetValidation.h>

#include <ffamodules/CDBInterface.h>

#include <globalvertex/GlobalVertexReco.h>

#include <mbd/MbdReco.h>

using std::cout;
using std::endl;
using std::string;
using std::pair;
using std::istringstream;

#include "Calo_Calib.C"
#include "HIJetReco.C"

R__LOAD_LIBRARY(libJetValidation.so)

void Fun4All_JetVal(const string &inputFile,
                    const string &outputTreeFile = "tree.root",
                    const string &outputQAFile   = "qa.root",
                    UInt_t nEvents = 0)
{
  cout << "#############################" << endl;
  cout << "Run Parameters" << endl;
  cout << "inputFile: "    << inputFile << endl;
  cout << "output tree: "  << outputTreeFile << endl;
  cout << "output QA: "    << outputQAFile << endl;
  cout << "Events: "       << nEvents << endl;
  cout << "#############################" << endl;

  Fun4AllServer *se = Fun4AllServer::instance();

  recoConsts *rc = recoConsts::instance();

  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");

  pair<Int_t, Int_t> runseg = Fun4AllUtils::GetRunSegment(inputFile);
  Int_t runnumber = runseg.first;
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

  HIJetReco();

  JetValidation *myJetVal = new JetValidation();
  myJetVal->set_outputTreeFileName(outputTreeFile);
  myJetVal->set_outputQAFileName(outputQAFile);
  myJetVal->set_bkg_tower_neighbor_energy(0.3); /*GeV*/
  myJetVal->set_saveTree(false);
  se->registerSubsystem(myJetVal);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  in->AddFile(inputFile.c_str());
  se->registerInputManager(in);

  se->run(nEvents);
  se->End();
  // se->PrintTimer();

  gSystem->Exit(0);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 2 || argc > 5){
        cout << "usage: ./bin/Fun4All_JetVal inputFile [outputTreeFile] [outputQAFile] [events]" << endl;
        cout << "inputFile: Location of fileList containing dst." << endl;
        cout << "outputTreeFile: name of output Tree file. Default: tree.root" << endl;
        cout << "outputQAFile: name of output QA file. Default: qa.root" << endl;
        cout << "events: Number of events to analyze. Default: all" << endl;
        return 1;
    }

    string outputTreeFile = "tree.root";
    string outputQAFile   = "qa.root";
    UInt_t events         = 0;

    if(argc >= 3) {
        outputTreeFile = argv[2];
    }
    if(argc >= 4) {
        outputQAFile = argv[3];
    }
    if(argc >= 5) {
        events = atoi(argv[4]);
    }

    Fun4All_JetVal(argv[1], outputTreeFile, outputQAFile, events);

    cout << "done" << endl;
    return 0;
}
# endif
