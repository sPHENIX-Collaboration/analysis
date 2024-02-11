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

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

// #include <phool/PHRandomSeed.h>
// #include <phool/recoConsts.h>

#include <calotreegen/caloTreeGen.h>

using std::cout;
using std::endl;
using std::string;

R__LOAD_LIBRARY(libcaloTreeGen.so)

void Fun4All_CaloTreeGen(const string  &inputFile,
                         const string  &qaFile       = "qa.root",
                         const string  &diphotonFile = "diphoton.root",
                         const Bool_t  doPi0Ana      = true,
                         const Float_t vtx_z_max     = 10, /*cm*/
                         const Float_t clusE_min     = 0.5 /*GeV*/,
                         const Float_t clusChi_max   = 4,
                         const UInt_t  nEvents       = 0) {
  Fun4AllServer *se = Fun4AllServer::instance();
  // recoConsts *rc = recoConsts::instance();

  caloTreeGen *calo = new caloTreeGen(qaFile.c_str(), diphotonFile.c_str());
  calo->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  calo->set_clusterE_min(clusE_min);
  calo->set_cluster_chi_max(clusChi_max);
  calo->set_do_pi0_ana(doPi0Ana);
  calo->set_vtx_z_max(vtx_z_max);
  se->registerSubsystem(calo);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  // in->AddListFile(inputFile.c_str());
  in->AddFile(inputFile.c_str());
  se->registerInputManager(in);

  se->run(nEvents);
  se->End();
  se->PrintTimer();
  std::cout << "All done!" << std::endl;

  gSystem->Exit(0);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 2 || argc > 9){
        cout << "usage: ./bin/Fun4All_CaloTreeGen inputFile qaFile diphotonFile doPi0Ana vtx_z_max clusE_min clusChi_max events" << endl;
        cout << "inputFile: Location of fileList containing dst." << endl;
        cout << "qaFile: name of output file." << endl;
        cout << "diphotonFile: name of output file." << endl;
        cout << "doPi0Ana: Enable pi0 analysis (takes longer). Default: true" << endl;
        cout << "vtx_z_max: Maximum z-vertex [cm]. Default: 30" << endl;
        cout << "clusE_min: Minimum cluster energy. Default: 0.5 GeV" << endl;
        cout << "clusChi_max: Maximum cluster chi squared. Default: 4" << endl;
        cout << "events: Number of events to analyze. Default: all" << endl;
        return 1;
    }

    string inputFile;
    string qaFile       = "qa.root";
    string diphotonFile = "diphoton.root";
    Bool_t doPi0Ana     = true;
    Float_t vtx_z_max   = 10;
    Float_t clusE_min   = 0.5;
    Float_t clusChi_max = 4;
    UInt_t events       = 0;

    if(argc >= 2) {
        inputFile = argv[1];
    }
    if(argc >= 3) {
        qaFile = argv[2];
    }
    if(argc >= 4) {
        diphotonFile = argv[3];
    }
    if(argc >= 5) {
        doPi0Ana = atoi(argv[4]);
    }
    if(argc >= 6) {
        vtx_z_max = atof(argv[5]);
    }
    if(argc >= 7) {
        clusE_min = atof(argv[6]);
    }
    if(argc >= 8) {
        clusChi_max = atof(argv[7]);
    }
    if(argc >= 9) {
        events = atoi(argv[8]);
    }

    Fun4All_CaloTreeGen(inputFile, qaFile, diphotonFile, doPi0Ana, vtx_z_max, clusE_min, clusChi_max, events);

    cout << "done" << endl;
    return 0;
}
# endif
