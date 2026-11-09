#include <fun4all/Fun4AllUtils.h>
#include <G4_ActsGeom.C>
#include <G4_Magnet.C>
#include <GlobalVariables.C>
#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>
#include <Trkr_TpcReadoutInit.C>

#include <ffamodules/CDBInterface.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/recoConsts.h>

#include <cdbobjects/CDBTTree.h>

#include <uspin/SpinDBNode.h>

#include <calotrigger/TriggerRunInfoReco.h>

#include <kfparticle_sphenix/KFParticle_sPHENIX.h>
#include <kfpssa/KFP_singlespinasymmetries.h>

void Run_Fun4All_KFP_SSA(
    const int nEvents   = -1,
    const int input_runnumber = 79515,
    const int input_process   = 1234,
    // const std::string inputDST  = "DST_TRKR_TRACKS_run3pp_ana538_2025p011_v001-00079514-00000.root",
    const std::string inputDir  = "/sphenix/lustre01/sphnxpro/production/run3pp/physics/ana538_2025p011_v001/DST_TRKR_TRACKS/run_00079500_00079600/",
    const std::string outputDir = "/sphenix/tg/tg01/commissioning/INTT/work/schen4/K0S_reconstruction/TSSA_from_resonanse/",
    const int nSkip = 0) // 3539 files for 79514? 
{
    /* 
    should be only 1000 F4A events in a file -> 5000 strobes (?)
    nEvents = -1 to run all of them
    */

    std::string inputDST = Form("DST_TRKR_TRACKS_run3pp_ana538_2025p011_v001-%08d-%05d.root", input_runnumber, input_process);
    std::string inputTrackFile = inputDir + inputDST;


    std::string input_seed_file = Form("DST_TRKR_SEED_run3pp_ana538_2025p011_v001-%08d-%05d.root", input_runnumber, input_process);
    std::string input_seed_file_with_dir = "/sphenix/lustre01/sphnxpro/production/run3pp/physics/ana538_2025p011_v001/DST_TRKR_SEED/run_00079500_00079600/" + input_seed_file;
    std::string input_clus_file = Form("DST_TRKR_CLUSTER_run3pp_ana538_2025p011_v001-%08d-%05d.root", input_runnumber, input_process);
    std::string input_clus_file_with_dir = "/sphenix/lustre01/sphnxpro/production/run3pp/physics/ana538_2025p011_v001/DST_TRKR_CLUSTER/run_00079500_00079600/" + input_clus_file;


    std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(inputDST);
    int runnumber = runseg.first;
    std::stringstream nice_runnumber;
    nice_runnumber << std::setw(8) << std::setfill('0') << to_string(runnumber);

    int rounded_up = 100*(std::ceil((float) runnumber/100));
    std::stringstream nice_rounded_up;
    nice_rounded_up << std::setw(8) << std::setfill('0') << to_string(rounded_up);

    int rounded_down = 100*(std::floor((float) runnumber/100));
    std::stringstream nice_rounded_down;
    nice_rounded_down << std::setw(8) << std::setfill('0') << to_string(rounded_down);

    int segment = runseg.second;
    std::stringstream nice_segment;
    nice_segment << std::setw(5) << std::setfill('0') << to_string(segment);

    //--------------------//
    //  output file name  //
    //--------------------//
    std::string outputfilename = outputDir + Form("output_KFParticle_K0S2pipi_reco_%s_%s.root",nice_runnumber.str().c_str(),nice_segment.str().c_str());


    //--------------//
    //  F4A server  //
    //--------------//
    auto se = Fun4AllServer::instance();
    se->Verbosity(0);

    auto rc = recoConsts::instance();
    rc->set_IntFlag("RUNNUMBER", runnumber);

    // ********************* CDB tag **********************
    Enable::CDB = true;
    rc->set_StringFlag("CDB_GLOBALTAG", "newcdbtag");
    rc->set_uint64Flag("TIMESTAMP", runnumber);

    TpcReadoutInit(runnumber);

    // G4TPC::REJECT_LASER_EVENTS=true;
    // G4TPC::ENABLE_MODULE_EDGE_CORRECTIONS = true;

    // // to turn on the default static corrections, enable the two lines below
    // G4TPC::ENABLE_STATIC_CORRECTIONS = true;

    // //to turn on the average corrections, enable the three lines below
    // G4TPC::ENABLE_AVERAGE_CORRECTIONS = true;
    // G4TPC::USE_PHI_AS_RAD_AVERAGE_CORRECTIONS = false;
    // // to use a custom file instead of the database file:
    // G4TPC::average_correction_filename = CDBInterface::instance()->getUrl("TPC_LAMINATION_FIT_CORRECTION");


    Enable::MVTX_APPLYMISALIGNMENT = true;
    ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;
    
    //-----------------------------------------------------------------//
    //  Flags (PPG016 use pp_mode, but ana.new do not have this flag)  //
    //-----------------------------------------------------------------//
    // TRACKING::pp_mode = true; 
    TRACKING::streaming_mode = true;

    std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");
    Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
    ingeo->AddFile(geofile);
    se->registerInputManager(ingeo);

    // Initialize tracking
    // TrackingInit();

    // ********************* read in trackmaps **********************
    auto tracks = new Fun4AllDstInputManager("TrackInputManager");
    tracks->fileopen(inputTrackFile);
    se->registerInputManager(tracks);
    // **************************************************************

    // auto register_seed = new Fun4AllDstInputManager("SeedInputManager");
    // register_seed->fileopen(input_seed_file_with_dir);
    // se->registerInputManager(register_seed);

    // auto register_clus = new Fun4AllDstInputManager("ClusterInputManager");
    // register_clus->fileopen(input_clus_file_with_dir);
    // se->registerInputManager(register_clus);

    // // ********************* read in clustermaps for dedx **********************
    // auto clusters = new Fun4AllDstInputManager("ClusterInputManager");
    // std::string clus_file = "/sphenix/lustre01/sphnxpro/production/run3pp/physics/ana538_2025p011_v001/DST_TRKR_CLUSTER/run_" + nice_rounded_down.str() + "_" + nice_rounded_up.str()  + "/DST_TRKR_CLUSTER_run3pp_ana538_2025p011_v001-" + nice_runnumber.str() +  "-" + nice_segment.str() + ".root";
    // clusters->fileopen(clus_file);
    // se->registerInputManager(clusters);
    // // **************************************************************
    // // ********************* read in streaming input for INTT BCO **********************
    // std::string gl1_file = "/sphenix/lustre01/sphnxpro/production/run3pp/physics/ana532_nocdbtag_v001/DST_STREAMING_EVENT_intt0/run_" + nice_rounded_down.str() + "_" + nice_rounded_up.str()  + "/DST_STREAMING_EVENT_intt0_run3pp_ana532_nocdbtag_v001-" +  nice_runnumber.str() + "-" + nice_segment.str() + ".root";
    // auto streaming= new Fun4AllDstInputManager("StreamingInputManager");
    // streaming->fileopen(gl1_file);
    // se->registerInputManager(streaming);
    // // **************************************************************

    // // ******************** register TriggerRunInfo *****************
    // TriggerRunInfoReco *triggerruninforeco = new TriggerRunInfoReco();
    // se->registerSubsystem(triggerruninforeco);

    // ********************* register KFParticle *********************
    // KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX("Kpi_reco");
    KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX("pipi_reco");
    // KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX("rho2pipi_reco");
    kfparticle->Verbosity(0);
    
    // kfparticle->setDecayDescriptor("[D0 -> K^- pi^+]cc");
    kfparticle->setDecayDescriptor("K_S0 -> pi^+ pi^-");
    // kfparticle->setDecayDescriptor("rho0-> pi^+ pi^-");
    kfparticle->usePID(false);
    kfparticle->setPIDacceptFraction(0.6);  ///////////////////////// 0.4 originally
    kfparticle->dontUseGlobalVertex(true);  // originally, true
    kfparticle->requireTrackVertexBunchCrossingMatch(true);
    kfparticle->getAllPVInfo(true);
    kfparticle->allowZeroMassTracks();
    kfparticle->use2Dmatching(false);
    kfparticle->getTriggerInfo(true);  ////////////////////////////// ???? false in Light flavor group
    kfparticle->getDetectorInfo(true); ////////////////////////////// ???? false in Light flavor group
    kfparticle->saveDST(true);
    kfparticle->setContainerName("pipi_reco");
    // kfparticle->saveParticleContainer(true); ?????????????? true in Light flavor group 
    // kfparticle->setContainerName("rho2pipi_reco");
    kfparticle->magFieldFile("FIELDMAP_TRACKING");
    kfparticle->setOutputName(outputfilename);
    kfparticle->saveOutput(false);

    // ============== for Kshort -> pipi ==============
     //PV to SV cuts
    kfparticle->constrainToPrimaryVertex(true);  ///////////////////// no option in Light flavor group
    kfparticle->setFlightDistancechi2(-1.);
    kfparticle->setMinDIRA(0.99);  /////////////////////////////////// -1 originally
    kfparticle->setDecayLengthRange(0.05, FLT_MAX);
    kfparticle->setDecayTimeRange(-10000, FLT_MAX); ////////////////// unused category

    //Track parameters
    kfparticle->setMinimumTrackPT(0.0);
    kfparticle->setMaximumTrackchi2nDOF(300.);
    // kfparticle->setMinimumTrackIP_XY(0.05); /////////////////////// error: no member named 'setMinimumTrackIP_XY' in 'KFParticle_sPHENIX'
    // kfparticle->setMinimumTrackPV_DCA_XY()
    kfparticle->setMinMVTXhits(1);
    kfparticle->setMinINTThits(1);
    kfparticle->setMinTPChits(20);

    //Vertex parameters
    kfparticle->setMaximumVertexchi2nDOF(20);
    kfparticle->setMaximumDaughterDCA(0.5); //5 mm
    // kfparticle->setMaximumDaughterDCA_XY(100); //5 mm // after ana477  // unused category

    //Parent parameters
    kfparticle->setMotherPT(0);
    kfparticle->setMinimumMass(0.4); //Check mass ranges
    kfparticle->setMaximumMass(0.6);
    kfparticle->setMaximumMotherVertexVolume(0.1);

    // =============================== //
    se->registerSubsystem(kfparticle);

    // ****************** register spin info *****************
    SpinDBNode *spin = new SpinDBNode();
    se->registerSubsystem(spin);

    // ****************** Do TSSAs ***********************
    KFP_singlespinasymmetries *kfpssa = new KFP_singlespinasymmetries();
    kfpssa->setMotherParticleName("K_S0");
    kfpssa->setMotherParticlePDG(310); //D0 PDG: 421 //Ks PDG: 310 // omega(782): 223 //rho(770) neutral: 113
    kfpssa->setKFPContainerName("pipi_reco");
    // kfpssa->setKFPContainerName("rho2pipi_reco");
    kfpssa->setOutputName(outputfilename);
    se->registerSubsystem(kfpssa);

    se->run(nEvents);
    se->End();

    

}