#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <filesystem>
#include <string>

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

#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloTowerCalib.h>
#include <calotrigger/MinimumBiasClassifier.h>
#include <centrality/CentralityReco.h>
#include <globalvertex/GlobalVertexReco.h>
#include <mbd/MbdReco.h>
#include <zdcinfo/ZdcReco.h>

#include <dndetaintt/dNdEtaINTT.h>

#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawmodules.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libdNdEtaINTT.so)
R__LOAD_LIBRARY(libcentrality_io.so)
R__LOAD_LIBRARY(libcentrality.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libmbd.so)
R__LOAD_LIBRARY(libglobalvertex.so)
// R__LOAD_LIBRARY(libcentralityvalid.so)
R__LOAD_LIBRARY(libzdcinfo.so)

int Fun4All_G4_sPHENIX(                           //
    const bool rundata = false,                   //
    const bool getINTTData = true,                //
    const int runnumber = 54280,                  //
    const string productionTag = "ProdA_2024",    //
    const string generator = "HIJING",            // only relevant for simulation
    const int nEvents = 10,                       //
    const string &outputFile = "testNtuple.root", //
    const int process = 0                         //
)
{
    // bool getINTTData = true;
    bool getCentralityData = (rundata) ? !getINTTData : true;
    if (rundata && getINTTData && getCentralityData)
    {
        std::cout << "We currently can't get INTT and Centrality info from the same file for real data, exiting!" << std::endl;
        exit(1);
    }

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

    Input::VERBOSITY = 10;
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
                infile = "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/production/ProdDST-HotDead-BCO-ADC-Survey/intt-" + std::string(TString::Format("%08d", runnumber).Data()) + ".root";
            }
            else
            {
                fclose(file);
            }

            INPUTREADHITS::filename[0] = infile;
        }
        else if (getCentralityData) // Generate MBD data list with `CreateDstList.pl --run 20869 --build ana403 --cdb 2023p011 DST_CALO_run1auau`
        {
            INPUTREADHITS::listfile[0] = "dst_calo_run1auau-000" + std::to_string(runnumber) + "-ana410_2023p014.list";
            // INPUTREADHITS::listfile[0] = "dst_calo-000" + std::to_string(runnumber) + ".list";
        }
    }
    else
    {
        if (generator == "HIJING")
        {
            // infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.457/HIJING/fullSim/magOff/detectorAligned/dstSet_00000/dNdeta-sim-HIJING-000-" + std::string(TString::Format("%05d", process).Data()) + ".root"; // 500K events
            // infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.457/HIJING/fullSim/magOff/detectorAligned/dstSet_00001/dNdeta-sim-HIJING-000-" + std::string(TString::Format("%05d", process).Data()) + ".root"; // 500K events, adding 40% of strange particles
            // infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.457/HIJING/fullSim/magOff/detectorAligned/dstSet_00002/dNdeta-sim-HIJING-000-" + std::string(TString::Format("%05d", process).Data()) + ".root"; // 500K events, enable z-clustering

            // Official production - nominal
            // INPUTREADHITS::filename[0] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/nopileup/trkrhit/run0026/DST_TRUTH_sHijing_0_20fm-0000000026-" + std::string(TString::Format("%06d", process).Data()) + ".root"; // Official production, MDC2
            // INPUTREADHITS::filename[1] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/nopileup/trkrhit/run0026/DST_TRKR_HIT_sHijing_0_20fm-0000000026-" + std::string(TString::Format("%06d", process).Data()) + ".root"; // Official production, MDC2
            // INPUTREADHITS::filename[2] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/nopileup/trkrcluster/run0026/DST_TRKR_CLUSTER_sHijing_0_20fm-0000000026-" + std::string(TString::Format("%06d", process).Data()) + ".root"; // Official production, MDC2
            // INPUTREADHITS::filename[3] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/nopileup/mbdepd/run0026/DST_MBD_EPD_sHijing_0_20fm-0000000026-" + std::string(TString::Format("%06d", process).Data()) + ".root"; // Official production, MDC2

            // Official production - enhanced strangeness
            INPUTREADHITS::filename[0] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/nopileup/trkrhit/run0027/DST_TRUTH_sHijing_0_20fm-0000000027-" + std::string(TString::Format("%06d", process).Data()) + ".root";            // Official production, MDC2
            INPUTREADHITS::filename[1] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/nopileup/trkrhit/run0027/DST_TRKR_HIT_sHijing_0_20fm-0000000027-" + std::string(TString::Format("%06d", process).Data()) + ".root";         // Official production, MDC2
            INPUTREADHITS::filename[2] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/nopileup/trkrcluster/run0027/DST_TRKR_CLUSTER_sHijing_0_20fm-0000000027-" + std::string(TString::Format("%06d", process).Data()) + ".root"; // Official production, MDC2
            INPUTREADHITS::filename[3] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/fm_0_20/nopileup/mbdepd/run0027/DST_MBD_EPD_sHijing_0_20fm-0000000027-" + std::string(TString::Format("%06d", process).Data()) + ".root";           // Official production, MDC2
        }
        else if (generator == "EPOS")
        {
            // infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.419/EPOS/fullSim/magOff/detectorMisaligned/dstSet_00000/dNdeta-sim-EPOS-000-" + std::string(TString::Format("%05d", process).Data()) + ".root";

            INPUTREADHITS::filename[0] = "/sphenix/lustre01/sphnxpro/mdc2/epos/nopileup/trkrhit/run0026/DST_TRUTH_epos_0_153fm-0000000026-" + std::string(TString::Format("%06d", process).Data()) + ".root";            // Official production, MDC2
            INPUTREADHITS::filename[1] = "/sphenix/lustre01/sphnxpro/mdc2/epos/nopileup/trkrhit/run0026/DST_TRKR_HIT_epos_0_153fm-0000000026-" + std::string(TString::Format("%06d", process).Data()) + ".root";         // Official production, MDC2
            INPUTREADHITS::filename[2] = "/sphenix/lustre01/sphnxpro/mdc2/epos/nopileup/trkrcluster/run0026/DST_TRKR_CLUSTER_epos_0_153fm-0000000026-" + std::string(TString::Format("%06d", process).Data()) + ".root"; // Official production, MDC2
            INPUTREADHITS::filename[3] = "/sphenix/lustre01/sphnxpro/mdc2/epos/nopileup/mbdepd/run0026/DST_MBD_EPD_epos_0_153fm-0000000026-" + std::string(TString::Format("%06d", process).Data()) + ".root";           // Official production, MDC2
        }
        else if (generator == "AMPT")
        {
            // infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.419/AMPT/fullSim/magOff/detectorMisaligned/dstSet_00000/dNdeta-sim-AMPT-000-" + std::string(TString::Format("%05d", process).Data()) + ".root";

            INPUTREADHITS::filename[0] = "/sphenix/lustre01/sphnxpro/mdc2/ampt/nopileup/trkrhit/run0026/DST_TRUTH_ampt_0_20fm-0000000026-" + std::string(TString::Format("%06d", process).Data()) + ".root";            // Official production, MDC2
            INPUTREADHITS::filename[1] = "/sphenix/lustre01/sphnxpro/mdc2/ampt/nopileup/trkrhit/run0026/DST_TRKR_HIT_ampt_0_20fm-0000000026-" + std::string(TString::Format("%06d", process).Data()) + ".root";         // Official production, MDC2
            INPUTREADHITS::filename[2] = "/sphenix/lustre01/sphnxpro/mdc2/ampt/nopileup/trkrcluster/run0026/DST_TRKR_CLUSTER_ampt_0_20fm-0000000026-" + std::string(TString::Format("%06d", process).Data()) + ".root"; // Official production, MDC2
            INPUTREADHITS::filename[3] = "/sphenix/lustre01/sphnxpro/mdc2/ampt/nopileup/mbdepd/run0026/DST_MBD_EPD_ampt_0_20fm-0000000026-" + std::string(TString::Format("%06d", process).Data()) + ".root";           // Official production, MDC2
        }
        else if (generator == "SIMPLE")
        {
            infile = "/sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.474/SIMPLE/e-/fullSim/magOff/detectorAligned/dstSet_00001/dNdeta-sim-SIMPLE-e--000-" + std::string(TString::Format("%05d", process).Data()) + ".root";
            INPUTREADHITS::filename[0] = infile;
        }
        else
        {
            std::cout << "Generator " << generator << " is not [HIJING, EPOS, AMPT, SIMPLE]. Exit" << std::endl;
            return 0;
        }
    }

    // register all input generators with Fun4All
    // InputRegister();

    Enable::MBD = getINTTData;
    Enable::MBDRECO = getCentralityData || !rundata;
    Enable::PIPE = getINTTData;
    Enable::MVTX = getINTTData;
    Enable::INTT = getINTTData;
    Enable::INTT_VERBOSITY = 0;
    Enable::TPC = getINTTData;
    Enable::MICROMEGAS = getINTTData;

    //===============
    // conditions DB flags
    //===============
    recoConsts *rc = recoConsts::instance();
    Enable::CDB = true;
    Enable::VERBOSITY = 0;
    rc->set_StringFlag("CDB_GLOBALTAG", productionTag);
    rc->set_uint64Flag("TIMESTAMP", runnumber);
    rc->set_IntFlag("RUNNUMBER", runnumber);

    if (getINTTData)
    {
        G4Init();
        G4Setup();

        // Load ActsGeometry object
        TrackingInit();
        // Reco clustering
        // if (rundata)
        // Intt_Clustering();
    }

    if (getCentralityData) // when running on real data for MBD ntuples or when running on simulation
    {
        // if (!rundata)
        //     Mbd_Reco();

        MbdReco *mbdreco = new MbdReco();
        mbdreco->Verbosity(0);
        se->registerSubsystem(mbdreco);

        GlobalVertexReco *gblvertex = new GlobalVertexReco();
        gblvertex->Verbosity(INT_MAX - 1);
        se->registerSubsystem(gblvertex);

        // CaloTowerDefs::BuilderType buildertype = CaloTowerDefs::kPRDFTowerv4;
        // CaloTowerBuilder *caZDC = new CaloTowerBuilder("ZDCBUILDER");
        // caZDC->set_detector_type(CaloTowerDefs::ZDC);
        // caZDC->set_builder_type(buildertype);
        // caZDC->set_processing_type(CaloWaveformProcessing::FAST);
        // caZDC->set_nsamples(16);
        // caZDC->set_offlineflag();
        // se->registerSubsystem(caZDC);

        // std::cout << "Calibrating ZDC" << std::endl;
        // CaloTowerCalib *calibZDC = new CaloTowerCalib("ZDC");
        // calibZDC->set_detector_type(CaloTowerDefs::ZDC);
        // se->registerSubsystem(calibZDC); // conditions DB flags

        // ZDC Reconstruction--Calib Info
        // ZdcReco *zdcreco = new ZdcReco();
        // zdcreco->set_zdc1_cut(0.0);
        // zdcreco->set_zdc2_cut(0.0);
        // se->registerSubsystem(zdcreco);

        MinimumBiasClassifier *mb = new MinimumBiasClassifier();
        mb->Verbosity(INT_MAX - 1);
        mb->setIsSim(!rundata);
        mb->setOverwriteScale("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/scales/cdb_centrality_scale_1.root");
        mb->setOverwriteVtx("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/vertexscales/cdb_centrality_vertex_scale_1.root");
        se->registerSubsystem(mb);

        CentralityReco *cr = new CentralityReco();
        cr->Verbosity(INT_MAX - 1);
        if (generator == "HIJING")
        {
            cr->setOverwriteScale("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/scales/cdb_centrality_scale_1.root");
            cr->setOverwriteVtx("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/vertexscales/cdb_centrality_vertex_scale_1.root");
            cr->setOverwriteDivs("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/divs/cdb_centrality_1.root");
        }
        else if (generator == "EPOS")
        {
            cr->setOverwriteScale("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/divs/cdb_centrality_scale_epos_magoff.root");
            cr->setOverwriteVtx("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/vertexscales/cdb_centrality_vertex_scale_epos_magoff.root");
            cr->setOverwriteDivs("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/divs/cdb_centrality_epos_magoff.root");
        }
        else if (generator == "AMPT")
        {
            cr->setOverwriteScale("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/divs/cdb_centrality_scale_ampt_magoff.root");
            cr->setOverwriteVtx("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/vertexscales/cdb_centrality_vertex_scale_ampt_magoff.root");
            cr->setOverwriteDivs("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/divs/cdb_centrality_ampt_magoff.root");
        }
        else
        {
            std::cout << "Generator " << generator << " is not [HIJING, EPOS, AMPT]. Use HIJING as default" << std::endl;
            cr->setOverwriteScale("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/scales/cdb_centrality_scale_1.root");
            cr->setOverwriteVtx("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/vertexscales/cdb_centrality_vertex_scale_1.root");
            cr->setOverwriteDivs("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/divs/cdb_centrality_1.root");
        }
        se->registerSubsystem(cr);
    }

    if (!rundata)
    {
        auto vtxing = new PHTruthVertexing;
        vtxing->associate_tracks(false);
        se->registerSubsystem(vtxing);
    }

    if (rundata && getCentralityData)
    {
        MinimumBiasClassifier *mb = new MinimumBiasClassifier();
        mb->Verbosity(0);
        se->registerSubsystem(mb);
    }

    dNdEtaINTT *myAnalyzer = new dNdEtaINTT("dNdEtaAnalyzer", outputFile, rundata);
    myAnalyzer->GetINTTdata(getINTTData || !rundata);
    myAnalyzer->GetRecoCluster(getINTTData || !rundata);
    myAnalyzer->GetTruthCluster(getINTTData || !rundata);
    myAnalyzer->GetInttRawHit(false);
    myAnalyzer->GetTrkrHit(getINTTData || !rundata); // disable for the official production, no TRKR_HITSET node
    myAnalyzer->GetCentrality(getCentralityData);
    // bool getPMTinfo = getCentralityData && false;
    myAnalyzer->GetPMTInfo(getCentralityData);
    myAnalyzer->GetPHG4(!rundata);
    myAnalyzer->GetAllPHG4(!rundata);
    if (!rundata)
    {
        if (generator == "SIMPLE")
            myAnalyzer->GetHEPMC(false);
        else
            myAnalyzer->GetHEPMC(true);
    }
    myAnalyzer->GetTrigger(rundata);

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
