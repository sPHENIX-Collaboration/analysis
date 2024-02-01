#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include <GlobalVariables.C>

#include <G4Setup_sPHENIX.C>
#include <G4_Centrality.C>
#include <G4_Input.C>
#include <Trkr_Clustering.C>
#include <Trkr_RecoInit.C>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <g4centrality/PHG4CentralityReco.h>
#include <trackreco/PHTruthVertexing.h>

#include <dndetaintt/dNdEtaINTT.h>

#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libdNdEtaINTT.so)
R__LOAD_LIBRARY(libcentrality_io.so)
R__LOAD_LIBRARY(libg4centrality.so)

int Fun4All_G4_sPHENIX(const bool rundata = true, const string generator = "HIJING", const int nEvents = -1, const string &outputFile = "dataNtuple.root", const int process = 0)
{
    int skip;
    if (rundata)
        skip = nEvents * process;
    else
        skip = 0;
    Fun4AllServer *se = Fun4AllServer::instance();
    se->Verbosity(1);

    recoConsts *rc = recoConsts::instance();

    Input::VERBOSITY = INT_MAX;
    Input::READHITS = true;

    string infile;
    if (rundata)
    {
        infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/data/run_00020869/ana.382/beam_intt_combined-dst-00020869-0000.root";
    }
    else
    {
        if (generator == "HIJING")
            infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.398/HIJING/fullSim/magOff/detectorAligned/dstSet_00000/dNdeta-sim-HIJING-000-" + std::string(TString::Format("%05d", process).Data()) + ".root";
        else if (generator == "EPOS")
            infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.399/EPOS/fullSim/magOff/detectorAligned/dstSet_00000/dNdeta-sim-EPOS-000-" + std::string(TString::Format("%05d", process).Data()) + ".root";
        else if (generator == "AMPT")
            infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/new/AMPT/fullSim/magOff/detectorAligned/dstSet_00000/dNdeta-sim-AMPT-000-" + std::string(TString::Format("%05d", process).Data()) + ".root";
        else
        {
            std::cout << "Generator " << generator << " is not [HIJING, EPOS, AMPT]. Exit" << std::endl;
            return 0;
        }
    }

    INPUTREADHITS::filename[0] = infile;
    // const vector<string> &filelist = {"/sphenix/user/hjheng/sPHENIXdNdEta/macros/list/dNdEta_INTT/dst_INTTdNdEta_data.list"};

    // for (unsigned int i = 0; i < filelist.size(); ++i)
    // {
    //     INPUTREADHITS::listfile[i] = filelist[i];
    // }

    // register all input generators with Fun4All
    // InputRegister();

    Enable::MBD = true;
    Enable::PIPE = true;
    Enable::MVTX = true;
    Enable::INTT = true;
    Enable::TPC = true;
    Enable::MICROMEGAS = true;

    //===============
    // conditions DB flags
    //===============
    Enable::CDB = true;
    Enable::VERBOSITY = 2;
    // global tag
    rc->set_StringFlag("CDB_GLOBALTAG", CDB::global_tag);
    // 64 bit timestamp
    rc->set_uint64Flag("TIMESTAMP", CDB::timestamp);

    G4Init();
    G4Setup();

    // TrkrHit reconstructions
    // Mvtx_Cells();
    // Load ActsGeometry object
    TrackingInit();
    // Reco clustering
    // Mvtx_Clustering();
    if (rundata)
        Intt_Clustering();

    //-----------------
    // Centrality Determination
    //-----------------
    // Centrality();
    PHG4CentralityReco *cent = new PHG4CentralityReco();
    cent->Verbosity(0);
    cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
    se->registerSubsystem(cent);

    auto vtxing = new PHTruthVertexing;
    vtxing->associate_tracks(false); // This is set to false because we do not run tracking
    se->registerSubsystem(vtxing);

    dNdEtaINTT *myAnalyzer = new dNdEtaINTT("dNdEtaAnalyzer", outputFile, rundata);
    myAnalyzer->GetTruthPV(true);
    myAnalyzer->GetRecoCluster(true);
    myAnalyzer->GetCentrality(true);
    myAnalyzer->GetTrkrHit(true);
    se->registerSubsystem(myAnalyzer);

    //--------------
    // Set up Input Managers
    //--------------
    InputManagers();

    //-----------------
    // Event processing
    //-----------------
    se->skip(skip);
    se->run(nEvents);

    //-----
    // Exit
    //-----
    se->End();
    std::cout << "All done" << std::endl;
    delete se;

    gSystem->Exit(0);
    return 0;
}
#endif
