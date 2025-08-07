#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

//#include <inttread/InttSaveTree.h>

#include <phool/recoConsts.h>

#include <Trkr_Clustering.C>
#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>
#include <G4Setup_sPHENIX.C>

#include <inttzvertexfindertrapezoidal/InttZVertexFinderTrapezoidal.h>

// #include <intt/InttCombinedRawDataDecoder.h>
// #include <intt/InttMapping.h>

//#include <inttread/InttRawData.h>
//#include <inttread/InttSaveTree.h>

// #include "functions.hh"
// #include "constant_values.hh"

// #include <dndetaintt/dNdEtaINTT.h>

// #include <intt/InttClusterizer.h>

// #include <mbd/MbdReco.h>

// #include <evtidreporter/EvtIDReporter.h>

// R__LOAD_LIBRARY(libfun4all.so)
// R__LOAD_LIBRARY(libfun4allraw.so)
// R__LOAD_LIBRARY(libffarawmodules.so)


/////////////////////////////////////////////////////////////////
// note : for clustering in Z axis
// #include <GlobalVariables.C>

// #include <G4_ActsGeom.C>
// #include <G4_TrkrVariables.C>
// #include <intt/InttClusterizer.h>
/////////////////////////////////////////////////////////////////

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libInttZVertexFinderTrapezoidal.so)


int F4AINTTzTrapezoidal()
{
    int process_id = 0;
    int run_num = 54280;
    int nevents = 5000;
    string inputDST_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/From_official_INTTRAWHIT_DST_ColdHotChannelMask/completed";
    string inputDST_filename  = "DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_00000.root";
    string outputDST_directory = "/sphenix/user/ChengWei/sPH_analysis_repo/INTTvtxZTrapezoidal/macro";
    string outputDST_filename  = "DSTvtxZTrapezoidal.root";
    
    pair<double, double> vertexXYIncm = {-0.0215087, 0.223197};
    bool IsFieldOn = false;
    bool IsDCACutApplied = true;
    std::pair<std::pair<double,double>,std::pair<double,double>> DeltaPhiCutInDegree = {{-0.5, 0.5},{-1000.,1000.}}; // note : in degree
    std::pair<std::pair<double,double>,std::pair<double,double>> DCAcutIncm = {{-0.1,0.1},{-1000.,1000.}}; // note : in cm
    int ClusAdcCut = 35;
    int ClusPhiSizeCut = 5;
    bool PrintRecoDetails = true;
 

    TStopwatch* watch = new TStopwatch();
    watch->Start();
    
    ////////////////////////////////////////////////////////////////////////
    string run_type = "physics";
    int total_run_len = 5;
    string run_num_str = to_string( run_num );
    run_num_str.insert(0, total_run_len - run_num_str.size(), '0');



    Fun4AllServer* se = Fun4AllServer::instance();
    // se->Verbosity(10000000);

    // just if we set some flags somewhere in this macro
    recoConsts *rc = recoConsts::instance();
    
    Enable::CDB = true;
    rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
    // 64 bit timestamp
    rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);
    rc->set_IntFlag("RUNNUMBER", run_num );


    Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_track");
    in->Verbosity(2);
    in->AddFile( inputDST_directory + "/" + inputDST_filename);
    // in->AddListFile( list_file_intt ); 
    // in->AddListFile( list_file_calo );
    in->Print("ALL");
    se->registerInputManager(in);
    // in->Print("ALL");  

    //Enable::BBC = true;
    Enable::MVTX = true;
    Enable::INTT = true;
    Enable::TPC = true;
    Enable::MICROMEGAS = true;
    G4Init();
    G4Setup();

    TrackingInit();

    // note : INTT vertex Z trapezoidal
    InttZVertexFinderTrapezoidal * inttz = new InttZVertexFinderTrapezoidal(
        "inttz",
        vertexXYIncm,
        IsFieldOn,
        IsDCACutApplied,
        DeltaPhiCutInDegree,
        DCAcutIncm,
        ClusAdcCut,
        ClusPhiSizeCut,
        PrintRecoDetails
    );
    se->registerSubsystem(inttz);

    Fun4AllOutputManager* out = new Fun4AllDstOutputManager("DST", outputDST_directory + "/" + outputDST_filename);
    se->registerOutputManager(out);

    se->run(nevents);
    se->End();

    return 0;
}
