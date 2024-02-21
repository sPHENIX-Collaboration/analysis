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

// simulation centrality module
#include <g4centrality/PHG4CentralityReco.h>

// #include <phool/PHRandomSeed.h>
// #include <phool/recoConsts.h>

#include <calotreegen/caloTreeGen.h>
#include <calotrigger/MinimumBiasClassifier.h>

using std::cout;
using std::endl;
using std::string;

R__LOAD_LIBRARY(libcaloTreeGen.so)

void Fun4All_CaloTreeGen(const string  &inputFile,
                         const string  &qaFile       = "qa.root",
                         const string  &diphotonFile = "diphoton.root",
                         const Bool_t  doPi0Ana      = true,
                         const Float_t vtx_z_max     = 10, /*cm*/
                         const Bool_t  isSim         = false,
                         const string  &dstGlobal    = "",
                         const string  &g4Hits       = "",
                         const Float_t clusE_min     = 0.5 /*GeV*/,
                         const Float_t clusChi_max   = 4,
                         const UInt_t  nEvents       = 0) {
  Fun4AllServer *se = Fun4AllServer::instance();
  // recoConsts *rc = recoConsts::instance();

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

  if(isSim) {
    PHG4CentralityReco *cent = new PHG4CentralityReco();
    cent->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
    cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
    se->registerSubsystem(cent);

    Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTglobal");
    in2->AddFile(dstGlobal.c_str());
    se->registerInputManager(in2);

    Fun4AllInputManager *in3 = new Fun4AllDstInputManager("G4Hits");
    in3->AddFile(g4Hits.c_str());
    se->registerInputManager(in3);
  }

  MinimumBiasClassifier *minimumbiasclassifier = new MinimumBiasClassifier();
  // minimumbiasclassifier->Verbosity(Fun4AllBase::VERBOSITY_SOME);
  // se->registerSubsystem(minimumbiasclassifier);

  caloTreeGen *calo = new caloTreeGen("caloTreeGen");
  calo->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  calo->set_output_QA(qaFile);
  calo->set_output_diphoton(diphotonFile);
  calo->set_simulation(isSim);
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
    if(argc < 2 || argc > 12){
        cout << "usage: ./bin/Fun4All_CaloTreeGen inputFile qaFile diphotonFile doPi0Ana vtx_z_max isSim DSTglobal g4Hits clusE_min clusChi_max events" << endl;
        cout << "inputFile: Location of fileList containing dst." << endl;
        cout << "qaFile: name of output file." << endl;
        cout << "diphotonFile: name of output file." << endl;
        cout << "doPi0Ana: Enable pi0 analysis (takes longer). Default: true" << endl;
        cout << "vtx_z_max: Maximum z-vertex [cm]. Default: 10" << endl;
        cout << "isSim: Analysis simulation?. Default: false" << endl;
        cout << "DSTglobal: DSTglobal. " << endl;
        cout << "g4Hits: g4Hits. " << endl;
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
    Bool_t isSim        = false;
    string dstGlobal    = "";
    string g4Hits       = "";
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
        isSim = atoi(argv[6]);
    }
    if(argc >= 8) {
        dstGlobal = argv[7];
    }
    if(argc >= 9) {
        g4Hits = argv[8];
    }
    if(argc >= 10) {
        clusE_min = atof(argv[9]);
    }
    if(argc >= 11) {
        clusChi_max = atof(argv[10]);
    }
    if(argc >= 12) {
        events = atoi(argv[11]);
    }

    Fun4All_CaloTreeGen(inputFile, qaFile, diphotonFile, doPi0Ana, vtx_z_max, isSim, dstGlobal, g4Hits, clusE_min, clusChi_max, events);

    cout << "done" << endl;
    return 0;
}
# endif
