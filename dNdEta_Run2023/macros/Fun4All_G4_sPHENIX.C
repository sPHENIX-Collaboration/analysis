#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <string>
#include <filesystem>

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

#include <calotrigger/MinimumBiasClassifier.h>
#include <centrality/CentralityReco.h>

#include <dndetaintt/dNdEtaINTT.h>

#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libdNdEtaINTT.so)
R__LOAD_LIBRARY(libcentrality_io.so)
R__LOAD_LIBRARY(libcentrality.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libcentrality.so)
R__LOAD_LIBRARY(libcalotrigger.so)

int Fun4All_G4_sPHENIX(                           //
    const bool rundata = true,                    //
    const bool getINTTData = true,                //
    const int runnumber = 20869,                  //
    const string productionTag = "2023p011",      //
    const string generator = "HIJING",            // only relevant for simulation
    const int nEvents = 1,                        //
    const string &outputFile = "testNtuple.root", //
    const int process = 0                         //
)
{
    // bool getINTTData = true;
    bool getCentralityData = !getINTTData;
    if (rundata && getINTTData && getCentralityData)
    {
        std::cout << "We currently can't get INTT and Centrality info from the same file for real data, exiting!" << std::endl;
        exit(1);
    }

    // std::string productionTag = "2023p011"; //"ProdA_2023";

    int skip;
    if (rundata)
    {
        skip = nEvents * process;
    }
    else
    {
        skip = 0;
    }

    Fun4AllServer *se = Fun4AllServer::instance();
    se->Verbosity(1);

    Input::VERBOSITY = 0;
    Input::READHITS = true;

    string infile;
    if (rundata)
    {
        if (getINTTData)
        {
            // Check if the file exists. If not, just use the one that Cameron produced
            std::filesystem::path productiondir = std::filesystem::current_path().parent_path() / "production/";
            infile = productiondir.string() + "intt-" + std::string(TString::Format("%08d", runnumber).Data()) + ".root";
            FILE *file;
            file = fopen(infile.c_str(), "r");
            if (file == NULL)
            {
                std::cout << "File: " << infile << " does not exist. Use the pre-generated file" << std::endl;
                infile = "/gpfs/mnt/gpfs02/sphenix/user/cdean/software/macros/InttProduction/intt-" + std::string(TString::Format("%08d", runnumber).Data()) + ".root";
            }
            else
            {
                fclose(file);
            }

            INPUTREADHITS::filename[0] = infile;
        }
        else if (getCentralityData) // Generate MBD data list with `CreateDstList.pl --run 20869 --build ana403 --cdb 2023p011 DST_CALO`
        {
            INPUTREADHITS::listfile[0] = "dst_calo-000" + std::to_string(runnumber) + ".list";
        }
    }
    else
    {
        if (generator == "HIJING")
        {
            infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.398/HIJING/fullSim/magOff/detectorAligned/dstSet_00000/dNdeta-sim-HIJING-000-" +
                     std::string(TString::Format("%05d", process).Data()) + ".root";
            // infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.376/HIJING/fullSim/magOff/detectorAligned/dstSet_00000/dNdeta-sim-HIJING-000-" +
            //          std::string(TString::Format("%04d", process).Data()) + ".root";
            // infile = "/direct/sphenix+tg+tg01/bulk/dNdeta_INTT_run2023/dNdeta_sPHENIX_simulations/macro/dstSet_00000/HIJING_sim_no_beam_angle-000-00000.root";
            INPUTREADHITS::filename[0] = infile;
        }
        else if (generator == "EPOS")
        {
            infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.399/EPOS/fullSim/magOff/detectorAligned/dstSet_00000/dNdeta-sim-EPOS-000-" +
                     std::string(TString::Format("%05d", process).Data()) + ".root";

            INPUTREADHITS::filename[0] = infile;
        }

        else if (generator == "AMPT")
        {
            infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/new/AMPT/fullSim/magOff/detectorAligned/dstSet_00000/dNdeta-sim-AMPT-000-" +
                     std::string(TString::Format("%05d", process).Data()) + ".root";

            INPUTREADHITS::filename[0] = infile;
        }

        else
        {
            std::cout << "Generator " << generator << " is not [HIJING, EPOS, AMPT]. Exit" << std::endl;
            return 0;
        }
    }

    // register all input generators with Fun4All
    // InputRegister();

    Enable::MBD = getINTTData;
    Enable::PIPE = getINTTData;
    Enable::MVTX = getINTTData;
    Enable::INTT = getINTTData;
    Enable::TPC = getINTTData;
    Enable::MICROMEGAS = getINTTData;

    //===============
    // conditions DB flags
    //===============
    recoConsts *rc = recoConsts::instance();
    Enable::CDB = true;
    Enable::VERBOSITY = 0;
    if (!getCentralityData)
    {
        // global tag
        rc->set_StringFlag("CDB_GLOBALTAG", CDB::global_tag);
        // 64 bit timestamp
        rc->set_uint64Flag("TIMESTAMP", CDB::timestamp);
    }
    else
    {
        rc->set_StringFlag("CDB_GLOBALTAG", productionTag);
        // 64 bit timestamp
        rc->set_uint64Flag("TIMESTAMP", runnumber);
    }

    if (getINTTData)
    {
        G4Init();
        G4Setup();

        // Load ActsGeometry object
        TrackingInit();
        // Reco clustering
        if (rundata)
            Intt_Clustering();
    }

    //-----------------
    // Centrality Determination
    //-----------------
    // Centrality();
    if (!rundata)
    {
        PHG4CentralityReco *cent = new PHG4CentralityReco();
        cent->Verbosity(0);
        cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
        se->registerSubsystem(cent);

        auto vtxing = new PHTruthVertexing;
        vtxing->associate_tracks(false); // This is set to false because we do not run tracking
        se->registerSubsystem(vtxing);
    }

    if (getCentralityData)
    {
        CentralityReco *cr = new CentralityReco();
        cr->Verbosity(0);
        se->registerSubsystem(cr);

        MinimumBiasClassifier *mb = new MinimumBiasClassifier();
        mb->Verbosity(0);
        se->registerSubsystem(mb);
    }

    dNdEtaINTT *myAnalyzer = new dNdEtaINTT("dNdEtaAnalyzer", outputFile, rundata);
    myAnalyzer->GetHEPMC(true);
    myAnalyzer->GetRecoCluster(true);
    myAnalyzer->GetCentrality(true);
    myAnalyzer->GetInttRawHit(false);
    myAnalyzer->GetTrkrHit(false);
    myAnalyzer->GetINTTdata(getINTTData);
    myAnalyzer->GetCentrality(getCentralityData);
    bool getPMTinfo = getCentralityData && false;
    myAnalyzer->GetPMTInfo(getPMTinfo);
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
