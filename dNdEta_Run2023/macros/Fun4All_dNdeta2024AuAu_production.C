#include <filesystem>
#include <iostream>
#include <string>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>

#include <phool/recoConsts.h>

#include <G4Setup_sPHENIX.C>
#include <Trkr_Clustering.C>
#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>

#include <intt/InttCombinedRawDataDecoder.h>
#include <intt/InttMapping.h>

#include <calotrigger/MinimumBiasClassifier.h>
#include <calotrigger/TriggerRunInfoReco.h>
#include <centrality/CentralityReco.h>
#include <globalvertex/GlobalVertexReco.h>
// #include <centrality/CentralityValid.h>

#include <dndetaintt/dNdEtaINTT.h>

#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloTowerCalib.h>
#include <mbd/MbdReco.h>
#include <zdcinfo/ZdcReco.h>

#include <GlobalVariables.C>

#include <G4_ActsGeom.C>
#include <G4_TrkrVariables.C>
#include <intt/InttClusterizer.h>

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

void Intt_ZClustering()
{
    int verbosity = std::max(Enable::VERBOSITY, Enable::INTT_VERBOSITY);
    Fun4AllServer *se = Fun4AllServer::instance();

    InttClusterizer *inttclusterizer = new InttClusterizer("InttClusterizer", G4MVTX::n_maps_layer, G4MVTX::n_maps_layer + G4INTT::n_intt_layer - 1);
    inttclusterizer->Verbosity(verbosity);
    // no Z clustering for Intt type 1 layers (we DO want Z clustering for type 0 layers)
    // turning off phi clustering for type 0 layers is not necessary, there is only one strip
    // per sensor in phi
    for (int i = G4MVTX::n_maps_layer; i < G4MVTX::n_maps_layer + G4INTT::n_intt_layer; i++)
    {
        if (G4INTT::laddertype[i - G4MVTX::n_maps_layer] == PHG4InttDefs::SEGMENTATION_PHI)
        {
            inttclusterizer->set_z_clustering(i, true);
        }
    }
    se->registerSubsystem(inttclusterizer);
}

int Fun4All_dNdeta2024AuAu_production(                          //
    const int runnumber = 54280,                                //
    const string productionTag = "ProdA_2024",                  //
    const string calodstlist = "./calolists/calo_54280_0.list", //
    const string inttdstlist = "./inttlists/intt_54280_0.list", //
    const int nEvents = 10,                                     //
    const string &outputFile = "testNtuple.root",               //
    const int process = 0,                                      //
    const int skip = 0                                          //
)
{
    bool centralitycalib = true;
    bool minimumbiascalib = true;
    bool enable_inttzclustering = false;

    // local calibration files
    std::string hotchannel_file = "/sphenix/user/jaein213/macros/inttcalib/hotmap_cdb/hotmap_run_000" + std::to_string(runnumber) + ".root";
    std::string bcomap_file = "/sphenix/user/jaein213/macros/inttcalib/bcomap_cdb/bcomap_run_000" + std::to_string(runnumber) + ".root";
    std::string dac_file = "/sphenix/tg/tg01/commissioning/INTT/data/CDB_files/2024/dac_map/cdb_intt_dac_35_45_60_90_120_150_180_210_streaming.root";

    // guard
    if (!std::filesystem::exists(hotchannel_file))
    {
        std::cerr << "Error: Hot channel file does not exist: " << hotchannel_file << std::endl;
        return 1;
    }

    if (!std::filesystem::exists(bcomap_file))
    {
        std::cerr << "Error: BCO map file does not exist: " << bcomap_file << std::endl;
        return 1;
    }

    if (!std::filesystem::exists(dac_file))
    {
        std::cerr << "Error: DAC file does not exist: " << dac_file << std::endl;
        return 1;
    }

    Fun4AllServer *se = Fun4AllServer::instance();
    se->Verbosity(INT_MAX - 1);

    recoConsts *rc = recoConsts::instance();

    Enable::CDB = true;
    rc->set_StringFlag("CDB_GLOBALTAG", productionTag);
    rc->set_uint64Flag("TIMESTAMP", runnumber);
    rc->set_IntFlag("RUNNUMBER", runnumber); //! TODO: change the runnumber when the centrality calibration is available in CDB

    Fun4AllInputManager *caloinput = new Fun4AllDstInputManager("DST_calo");
    caloinput->Verbosity(2);
    caloinput->AddListFile(calodstlist);
    se->registerInputManager(caloinput);

    Fun4AllInputManager *inttinput = new Fun4AllDstInputManager("DST_track");
    inttinput->Verbosity(2);
    inttinput->AddListFile(inttdstlist);
    se->registerInputManager(inttinput);

    // Enable::MBD = true;
    // Enable::MBDRECO = false;
    // Enable::MBD_VERBOSITY = INT_MAX - 1;
    Enable::PIPE = true;
    Enable::MVTX = true;
    Enable::INTT = true;
    Enable::TPC = true;
    Enable::MICROMEGAS = true;
    Enable::ZDC = minimumbiascalib;
    Enable::ZDC_TOWER = minimumbiascalib;

    G4Init();
    G4Setup();

    // INTT unpacker - non-standard setup
    auto inttunpacker = new InttCombinedRawDataDecoder;
    inttunpacker->Verbosity(0);
    // inttunpacker->runInttStandalone(true);
    inttunpacker->runInttStandalone(false);
    inttunpacker->writeInttEventHeader(true);
    inttunpacker->set_triggeredMode(true);
    inttunpacker->LoadHotChannelMapLocal(hotchannel_file);
    inttunpacker->SetCalibBCO(bcomap_file, InttCombinedRawDataDecoder::FILE);
    inttunpacker->SetCalibDAC(dac_file, InttCombinedRawDataDecoder::FILE);
    se->registerSubsystem(inttunpacker);

    TrackingInit();

    if (enable_inttzclustering)
        Intt_ZClustering();
    else
        Intt_Clustering();

    if (minimumbiascalib)
    {
        MbdReco *mbdreco = new MbdReco();
        mbdreco->Verbosity(0);
        se->registerSubsystem(mbdreco);

        GlobalVertexReco *gblvertex = new GlobalVertexReco();
        gblvertex->Verbosity(0);
        se->registerSubsystem(gblvertex);

        CaloTowerDefs::BuilderType buildertype = CaloTowerDefs::kPRDFTowerv4;
        CaloTowerBuilder *caZDC = new CaloTowerBuilder("ZDCBUILDER");
        caZDC->set_detector_type(CaloTowerDefs::ZDC);
        caZDC->set_builder_type(buildertype);
        caZDC->set_processing_type(CaloWaveformProcessing::FAST);
        caZDC->set_nsamples(16);
        caZDC->set_offlineflag();
        se->registerSubsystem(caZDC);

        std::cout << "Calibrating ZDC" << std::endl;
        CaloTowerCalib *calibZDC = new CaloTowerCalib("ZDC");
        calibZDC->set_detector_type(CaloTowerDefs::ZDC);
        se->registerSubsystem(calibZDC); // conditions DB flags

        // ZDC Reconstruction--Calib Info
        ZdcReco *zdcreco = new ZdcReco();
        zdcreco->set_zdc1_cut(0.0);
        zdcreco->set_zdc2_cut(0.0);
        se->registerSubsystem(zdcreco);

        MinimumBiasClassifier *mb = new MinimumBiasClassifier();
        mb->Verbosity(0);
        se->registerSubsystem(mb);
    }

    if (centralitycalib)
    {
        CentralityReco *cr = new CentralityReco();
        cr->Verbosity(INT_MAX - 1);
        se->registerSubsystem(cr);
    }

    TriggerRunInfoReco *triggerruninforeco = new TriggerRunInfoReco();
    se->registerSubsystem(triggerruninforeco);

    dNdEtaINTT *myAnalyzer = new dNdEtaINTT("dNdEtaAnalyzer", outputFile, true);
    myAnalyzer->GetINTTdata(true);
    myAnalyzer->GetRecoCluster(true);
    myAnalyzer->GetInttRawHit(true);
    myAnalyzer->GetTrkrHit(true);
    myAnalyzer->GetCentrality(true);
    myAnalyzer->GetPMTInfo(true);
    myAnalyzer->GetPHG4(false);
    myAnalyzer->GetHEPMC(false);
    myAnalyzer->GetTrigger(true);
    se->registerSubsystem(myAnalyzer);

    //--------------
    // Set up Input Managers
    //--------------
    // InputManagers();

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