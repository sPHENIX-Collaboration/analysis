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
#include <fun4all/Fun4AllUtils.h>

// simulation centrality module
#include <g4centrality/PHG4CentralityReco.h>

// #include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <calotreegen/caloTreeGen.h>
#include <calotrigger/MinimumBiasClassifier.h>

#include "Sys_Calo.C"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::pair;

R__LOAD_LIBRARY(libcaloTreeGen.so)

void Fun4All_CaloTreeGen(const string  &inputFile,
                         const string  &qaFile       = "qa.root",
                         const string  &diphotonFile = "diphoton.root",
                         const string  &systematic   = "none",
                         const Bool_t  doPi0Ana      = true,
                         const Float_t vtx_z_max     = 10, /*cm*/
                         const Bool_t  useZDCInfo    = false,
                         const Float_t zdc_cut       = 40, /*GeV*/
                         const Float_t clusE_min     = 1, /*GeV*/
                         const Float_t clusChi_max   = 4,
                         const Bool_t  isSim         = false,
                         const string  &dstGlobal    = "",
                         const string  &dstMBD       = "",
                         const string  &g4Hits       = "",
                         const UInt_t  nEvents       = 0) {

  cout << "#############################" << endl;
  cout << "Run Parameters"<< endl;
  cout << "inputFile: "              << inputFile << endl;
  cout << "qaFile: "                 << qaFile << endl;
  cout << "diphotonFile: "           << diphotonFile << endl;
  cout << "doPi0Ana: "               << doPi0Ana << endl;
  cout << "vtx_z_max: "              << vtx_z_max << " [cm]" << endl;
  cout << "useZDCInfo: "             << useZDCInfo << endl;
  cout << "zdc cut: "                << zdc_cut << " [GeV]" << endl;
  cout << "Minimum Cluster Energy: " << clusE_min << " [GeV]" << endl;
  cout << "Maximum Cluster Chi2: "   << clusChi_max << endl;
  cout << "isSim: "                  << isSim << endl;
  cout << "dstGlobal: "              << dstGlobal << endl;
  cout << "dstMBD: "                 << dstMBD << endl;
  cout << "g4Hits: "                 << g4Hits << endl;
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

    Fun4AllInputManager *in4 = new Fun4AllDstInputManager("DSTmbd");
    in4->AddFile(dstMBD.c_str());
    se->registerInputManager(in4);
  }

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  // in->AddListFile(inputFile.c_str());
  in->AddFile(inputFile.c_str());
  se->registerInputManager(in);

  MinimumBiasClassifier *minimumbiasclassifier = new MinimumBiasClassifier();
  minimumbiasclassifier->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  minimumbiasclassifier->set_useZDCInfo(useZDCInfo);
  minimumbiasclassifier->set_zdc_cut(zdc_cut);
  minimumbiasclassifier->set_simulation(isSim);
  se->registerSubsystem(minimumbiasclassifier);

  caloTreeGen *calo = new caloTreeGen("caloTreeGen");
  calo->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  calo->set_output_QA(qaFile);
  calo->set_output_diphoton(diphotonFile);
  calo->set_simulation(isSim);
  calo->set_clusterE_min(clusE_min);
  calo->set_cluster_chi_max(clusChi_max);
  calo->set_do_pi0_ana(doPi0Ana);
  calo->set_vtx_z_max(vtx_z_max);

  if(isSim) {
    calo->set_clusterNode("CLUSTER_CEMC");
  }

 /**
  * Systematic Node options:
    SYST1CEMC
    SYST2CEMC
    SYST3UCEMC
    SYST3DCEMC
    SYST4CEMC
  */
  if(systematic != "none") {

    string towerNode = "TOWERINFO_CALIB_"+systematic;
    string clusterNode = "CLUSTERINFO_CEMC_"+systematic;
    // Add Systematic Nodes
    Register_Tower_sys();

    // Clusters
    auto ClusterBuilder = new RawClusterBuilderTemplate(("EmcRawClusterBuilderTemplate_"+systematic).c_str());
    ClusterBuilder->Detector("CEMC");
    ClusterBuilder->set_threshold_energy(0.030);  // for when using basic calibration
    string emc_prof = getenv("CALIBRATIONROOT");
    emc_prof += "/EmcProfile/CEMCprof_Thresh30MeV.root";
    ClusterBuilder->LoadProfile(emc_prof);
    ClusterBuilder->set_UseTowerInfo(1);  // to use towerinfo objects rather than old RawTower
    ClusterBuilder->setInputTowerNodeName(towerNode.c_str());
    ClusterBuilder->setOutputClusterNodeName(clusterNode.c_str());
    se->registerSubsystem(ClusterBuilder);

    calo->set_towerNode(towerNode);
    calo->set_clusterNode(clusterNode);
  }

  se->registerSubsystem(calo);

  se->run(nEvents);
  se->End();
  se->PrintTimer();
  cout << "All done!" << endl;

  gSystem->Exit(0);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 2 || argc > 16){
        cout << "usage: ./bin/Fun4All_CaloTreeGen inputFile [qaFile] [diphotonFile] [systematic] [doPi0Ana] [vtx_z_max] [useZDCInfo] [zdc_cut] [clusE_min] [clusChi_max] [isSim] [DSTglobal] [DSTmbd] [g4Hits] [events]" << endl;
        cout << "inputFile: Location of fileList containing dst." << endl;
        cout << "qaFile: name of output file. Default: qa.root" << endl;
        cout << "diphotonFile: name of output file. Default: diphoton.root" << endl;
        cout << "systematic: name of systematic node. Default: none " << endl;
        cout << "doPi0Ana: Enable pi0 analysis (takes longer). Default: true" << endl;
        cout << "vtx_z_max: Maximum z-vertex [cm]. Default: 10" << endl;
        cout << "useZDCInfo: Use ZDC info for MB classifier. Default: false" << endl;
        cout << "zdc cut: Minimum ZDC energy. Default: 40 GeV" << endl;
        cout << "clusE_min: Minimum cluster energy. Default: 1 GeV" << endl;
        cout << "clusChi_max: Maximum cluster chi squared. Default: 4" << endl;
        cout << "isSim: Analysis simulation?. Default: false" << endl;
        cout << "DSTglobal: DSTglobal. " << endl;
        cout << "DSTmbd: DSTmbd. " << endl;
        cout << "g4Hits: g4Hits. " << endl;
        cout << "events: Number of events to analyze. Default: all" << endl;
        return 1;
    }

    string inputFile;
    string qaFile       = "qa.root";
    string diphotonFile = "diphoton.root";
    string systematic   = "none";
    Bool_t doPi0Ana     = true;
    Float_t vtx_z_max   = 10;
    Bool_t useZDCInfo   = false;
    Float_t zdc_cut     = 40;
    Float_t clusE_min   = 1;
    Float_t clusChi_max = 4;
    Bool_t isSim        = false;
    string dstGlobal    = "";
    string dstMBD       = "";
    string g4Hits       = "";
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
        systematic = argv[4];
    }
    if(argc >= 6) {
        doPi0Ana = atoi(argv[5]);
    }
    if(argc >= 7) {
        vtx_z_max = atof(argv[6]);
    }
    if(argc >= 8) {
        useZDCInfo = atoi(argv[7]);
    }
    if(argc >= 9) {
        zdc_cut = atof(argv[8]);
    }
    if(argc >= 10) {
        clusE_min = atof(argv[9]);
    }
    if(argc >= 11) {
        clusChi_max = atof(argv[10]);
    }
    if(argc >= 12) {
        isSim = atoi(argv[11]);
    }
    if(argc >= 13) {
        dstGlobal = argv[12];
    }
    if(argc >= 14) {
        dstMBD = argv[13];
    }
    if(argc >= 15) {
        g4Hits = argv[14];
    }
    if(argc >= 16) {
        events = atoi(argv[15]);
    }

    Fun4All_CaloTreeGen(inputFile, qaFile, diphotonFile, systematic, doPi0Ana, vtx_z_max, useZDCInfo, zdc_cut, clusE_min, clusChi_max, isSim, dstGlobal, dstMBD, g4Hits, events);

    cout << "done" << endl;
    return 0;
}
# endif
