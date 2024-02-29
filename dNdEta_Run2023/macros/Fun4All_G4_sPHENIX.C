#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <GlobalVariables.C>

#include <G4Setup_sPHENIX.C>
#include <G4_Centrality.C>
#include <G4_Input.C>
#include <Trkr_Clustering.C>
#include <Trkr_RecoInit.C>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <calotrigger/MinimumBiasClassifier.h>
#include <centrality/CentralityReco.h>
#include <g4centrality/PHG4CentralityReco.h>
#include <trackreco/PHTruthVertexing.h>

#include <dndetaintt/dNdEtaINTT.h>

#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libdNdEtaINTT.so)
R__LOAD_LIBRARY(libcentrality_io.so)
R__LOAD_LIBRARY(libcentrality.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libcalotrigger.so)

int Fun4All_G4_sPHENIX(                           //
    const bool rundata = true,                    //
    const int runnumber = 20869,                  //
    const string generator = "HIJING",            //
    const int nEvents = 1,                        //
    const string &outputFile = "testNtuple.root", //
    const int process = 0                         //
)
{
    int skip;
    if (rundata)
        skip = nEvents * process;
    else
        skip = 0;

    Fun4AllServer *se = Fun4AllServer::instance();
    se->Verbosity(1);

    recoConsts *rc = recoConsts::instance();
    rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2023");
    // 64 bit timestamp
    rc->set_uint64Flag("TIMESTAMP", runnumber);

    Input::VERBOSITY = 1E9;
    Input::READHITS = true;

    string infile;
    if (rundata)
    {
        // hardcoded for now, fix in the future
        // infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/data/run_00020869/ana.382/beam_intt_combined-dst-00020869-0000.root";
        infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/data/run_" + std::string(TString::Format("%08d", runnumber).Data()) + "/ana.382/beam_intt_combined-dst-" +
                 std::string(TString::Format("%08d", runnumber).Data()) + "-0000.root";
    }
    else
    {
        if (generator == "HIJING")
            infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.398/HIJING/fullSim/magOff/detectorAligned/dstSet_00000/dNdeta-sim-HIJING-000-" +
                     std::string(TString::Format("%05d", process).Data()) + ".root";
        // infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.376/HIJING/fullSim/magOff/detectorAligned/dstSet_00000/dNdeta-sim-HIJING-000-" +
        //          std::string(TString::Format("%04d", process).Data()) + ".root";
        // infile = "/direct/sphenix+tg+tg01/bulk/dNdeta_INTT_run2023/dNdeta_sPHENIX_simulations/macro/dstSet_00000/HIJING_sim_no_beam_angle-000-00000.root";
        else if (generator == "EPOS")
            infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.399/EPOS/fullSim/magOff/detectorAligned/dstSet_00000/dNdeta-sim-EPOS-000-" +
                     std::string(TString::Format("%05d", process).Data()) + ".root";
        else if (generator == "AMPT")
            infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/new/AMPT/fullSim/magOff/detectorAligned/dstSet_00000/dNdeta-sim-AMPT-000-" +
                     std::string(TString::Format("%05d", process).Data()) + ".root";
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

    // Enable subsystems
    Enable::MBD = true;
    Enable::PIPE = true;
    Enable::MVTX = true;
    Enable::INTT = true;
    Enable::TPC = true;
    Enable::MICROMEGAS = true;

    // Set up GEANT
    G4Init();
    G4Setup();

    // Load ActsGeometry object
    TrackingInit();

    // Reco clustering
    if (rundata)
        Intt_Clustering();

    PHG4CentralityReco *cent = new PHG4CentralityReco();
    cent->Verbosity(0);
    cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
    se->registerSubsystem(cent);

    // CentralityReco *cr = new CentralityReco();
    // cr->Verbosity(1);
    // se->registerSubsystem(cr);

    // MinimumBiasClassifier *mb = new MinimumBiasClassifier();
    // mb->Verbosity(0);
    // se->registerSubsystem(mb);

    auto vtxing = new PHTruthVertexing;
    vtxing->associate_tracks(false); // This is set to false because we do not run tracking
    se->registerSubsystem(vtxing);

    dNdEtaINTT *myAnalyzer = new dNdEtaINTT("dNdEtaAnalyzer", outputFile, rundata);
    myAnalyzer->GetHEPMC(true);
    myAnalyzer->GetTruthCluster(true);
    myAnalyzer->GetRecoCluster(true);
    myAnalyzer->GetCentrality(true);
    myAnalyzer->GetInttRawHit(true);
    myAnalyzer->GetTrkrHit(true);
    myAnalyzer->GetPHG4(true);
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
