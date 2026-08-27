#include <fun4all/Fun4AllUtils.h>
#include <G4_ActsGeom.C>
#include <G4_Global.C>
#include <G4_Magnet.C>
#include <G4_Mbd.C>
#include <GlobalVariables.C>
#include <Trkr_Clustering.C>
#include <Trkr_LaserClustering.C>
#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>
#include <Trkr_TpcReadoutInit.C>
#include <QA.C>
#include <Calo_Calib.C>

#include <ffamodules/CDBInterface.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/recoConsts.h>

#include <mvtxrawhitqa/MvtxRawHitQA.h>
#include <inttrawhitqa/InttRawHitQA.h>
#include <tpcqa/TpcRawHitQA.h>
#include <trackingqa/InttClusterQA.h>
#include <trackingqa/MicromegasClusterQA.h>
#include <trackingqa/MvtxClusterQA.h>
#include <trackingqa/TpcClusterQA.h>
#include <trackingqa/SiliconSeedsQA.h>
#include <trackingqa/TpcSeedsQA.h>
#include <trackingqa/TrackFittingQA.h>
#include <trackingqa/TpcSiliconQA.h>
#include <trackingqa/VertexQA.h>
#include <trackreco/PHActsTrackProjection.h>

#include <caloreco/CaloGeomMapping.h>
#include <caloreco/RawClusterBuilderTemplate.h>

#include <kfparticle_sphenix/KFParticle_sPHENIX.h>

#include <stdio.h>

#include "HF_selections.C"

using namespace HeavyFlavorReco;

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)
R__LOAD_LIBRARY(libcdbobjects.so)

bool IsListFile = false;
bool IsDstFile = false;
bool DoUnpacking = false;
bool DoClustering = false;
bool DoSeeding = false;
bool DoFitting = false;

void CheckDstType(const std::string inputDST)
{
    if (inputDST.find("DST_STREAMING_EVENT") != std::string::npos)
    {
        DoUnpacking = true;
        DoClustering = true;
        DoSeeding = true;
        DoFitting = true;
    }
    else if (inputDST.find("DST_TRKR_CLUSTER") != std::string::npos)
    {
        DoUnpacking = false;
        DoClustering = false;
        DoSeeding = true;
        DoFitting = true;
    }
    else if (inputDST.find("DST_TRKR_SEED") != std::string::npos)
    {
        DoUnpacking = false;
        DoClustering = false;
        DoSeeding = false;
        DoFitting = true;
    }
    else if (inputDST.find("DST_TRKR_TRACKS") != std::string::npos)
    {
        DoUnpacking = false;
        DoClustering = false;
        DoSeeding = false;
        DoFitting = false;
    }
    return;
}

std::string GetRunInfo(const std::string &filename)
{
    const std::vector<std::string> runspecies_patterns = {"run2pp", "run3pp", "run3auau"};

    boost::char_separator<char> sep("_");
    boost::tokenizer<boost::char_separator<char>> tok(filename, sep);

    for (const auto &t : tok)
    {
        for (const auto &pattern : runspecies_patterns)
        {
            if (t == pattern)
            {
                return t;
            }
        }
    }

    std::cout << "Cannot extract run info from filename!" << std::endl;
    return "";
}

void Fun4All_HF(const int nEvents = 500,                                                                                                                                                                                            //
                const std::string inputDST = "/sphenix/lustre01/sphnxpro/production/run2pp/physics/ana506_2024p023_v001/DST_TRKR_TRACKS/run_00053800_00053900/dst/DST_TRKR_TRACKS_run2pp_ana506_2024p023_v001-00053877-00000.root", //
                const std::string outDir = "./",                                                                                                                                                                                    //
                const int nSkip = 0,                                                                                                                                                                                                //
                const bool convertSeeds = false)                                                                                                                                                                                    //
{
    auto se = Fun4AllServer::instance();
    se->Verbosity(1);
    // se->VerbosityDownscale(500);
    auto rc = recoConsts::instance();

    if (inputDST.find(".list") != std::string::npos)
    {
        IsListFile = true;
        std::cout << "Input is list file" << std::endl;
    }
    if (inputDST.find(".root") != std::string::npos)
    {
        IsDstFile = true;
        std::cout << "Input is dst file" << std::endl;
    }
    if (!IsListFile && !IsDstFile)
    {
        std::cout << "Check your input! Exit" << std::endl;
        gSystem->Exit(0);
    }

    int runnumber = std::numeric_limits<int>::quiet_NaN();
    int segment = std::numeric_limits<int>::quiet_NaN();
    std::string runspecies = "";

    if (IsListFile)
    {
        auto *hitsin = new Fun4AllDstInputManager("InputManager");
        hitsin->AddListFile(inputDST);
        se->registerInputManager(hitsin);
       
        std::ifstream ifs(inputDST);
        std::string filepath;
        int i = 0;
        while (std::getline(ifs, filepath))
        {
            std::cout << "Adding DST with filepath: " << filepath << std::endl;
            if (i == 0)
            {
                std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(filepath);
                runnumber = runseg.first;
                segment = runseg.second;
                runspecies = GetRunInfo(filepath);
                CheckDstType(filepath);
                break;
            }
            /*
            if (!DoSeeding && (get_dEdx_info || get_detector_info))
            {
              std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(filepath);
              int segmentDST = runseg.second;
              std::stringstream nice_segment;
              nice_segment << std::setw(5) << std::setfill('0') << to_string(segmentDST);
              
              std::stringstream nice_runnumber;
              nice_runnumber << std::setw(8) << std::setfill('0') << to_string(runnumber);

              int rounded_up = 100 * (std::ceil((float)(runnumber+0.1) / 100));
              std::stringstream nice_rounded_up;
              nice_rounded_up << std::setw(8) << std::setfill('0') << to_string(rounded_up);

              int rounded_down = 100 * (std::floor((float)(runnumber+0.1) / 100));
              std::stringstream nice_rounded_down;
              nice_rounded_down << std::setw(8) << std::setfill('0') << to_string(rounded_down);
              //unsigned begin = inputDST.find("ana");
              //unsigned end = inputDST.find("-");
              std::string clus_anacdbver = "ana532_2025p009_v001"; //inputDST.substr(begin, end - begin);
              std::string clus_file = "";
              if (runspecies == "run3pp" || runspecies == "run3auau")
              {
                if (runnumber == 79516)
                {
                  clus_file = "/sphenix/lustre01/sphnxpro/production/" + runspecies + "/physics/ana538_2025p011_v001/DST_TRKR_CLUSTER/run_" + nice_rounded_down.str() + "_" + nice_rounded_up.str() + "/DST_TRKR_CLUSTER_" + runspecies + "_" + clus_anacdbver + "-" + nice_runnumber.str() + "-" + nice_segment.str() + ".root";
                }
                else
                {
                  clus_file = "/sphenix/lustre01/sphnxpro/production/" + runspecies + "/physics/" + clus_anacdbver + "/DST_TRKR_CLUSTER/run_" + nice_rounded_down.str() + "_" + nice_rounded_up.str() + "/DST_TRKR_CLUSTER_" + runspecies + "_" + clus_anacdbver + "-" + nice_runnumber.str() + "-" + nice_segment.str() + ".root";
                }
              }
              if (runspecies == "run2pp")
              {
                clus_file = "/sphenix/lustre01/sphnxpro/production/" + runspecies + "/physics/" + clus_anacdbver + "/DST_TRKR_CLUSTER/run_" + nice_rounded_down.str() + "_" + nice_rounded_up.str() + "/dst/DST_TRKR_CLUSTER_" + runspecies + "_" + clus_anacdbver + "-" + nice_runnumber.str() + "-" + nice_segment.str() + ".root";
              }
              std::cout << "Input cluster DST: " << clus_file << std::endl;

              std::string inputnameCluster = "InputManagerCluster" + std::to_string(i);
              auto hitsinclus = new Fun4AllDstInputManager(inputnameCluster);
              hitsinclus->fileopen(clus_file);
              se->registerInputManager(hitsinclus); 
            }
            */
            i++;
        }
    }

    if (IsDstFile)
    {
        std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(inputDST);
        runnumber = runseg.first;
        segment = runseg.second;
        runspecies = GetRunInfo(inputDST);

        CheckDstType(inputDST);

        auto *hitsin = new Fun4AllDstInputManager("InputManager");
        hitsin->fileopen(inputDST);
        se->registerInputManager(hitsin);
    }

    rc->set_IntFlag("RUNNUMBER", runnumber);
    rc->set_IntFlag("RUNSEGMENT", segment);
    rc->set_uint64Flag("TIMESTAMP", runnumber);

    Enable::CDB = true;
    rc->set_StringFlag("CDB_GLOBALTAG", "newcdbtag");
    rc->set_uint64Flag("TIMESTAMP", runnumber);

    std::stringstream nice_runnumber;
    nice_runnumber << std::setw(8) << std::setfill('0') << to_string(runnumber);

    int rounded_up = 100 * (std::ceil((float)(runnumber+0.1) / 100));
    std::stringstream nice_rounded_up;
    nice_rounded_up << std::setw(8) << std::setfill('0') << to_string(rounded_up);

    int rounded_down = 100 * (std::floor((float)(runnumber+0.1) / 100));
    std::stringstream nice_rounded_down;
    nice_rounded_down << std::setw(8) << std::setfill('0') << to_string(rounded_down);

    std::stringstream nice_segment;
    nice_segment << std::setw(5) << std::setfill('0') << to_string(segment);

    std::stringstream nice_skip;
    nice_skip << std::setw(5) << std::setfill('0') << to_string(nSkip);

    if (!DoSeeding && (get_dEdx_info || get_detector_info) && IsDstFile)
    {
        //unsigned begin = inputDST.find("ana");
        //unsigned end = inputDST.find("-");
        std::string clus_anacdbver = "ana532_2025p009_v001"; //inputDST.substr(begin, end - begin);
        std::string clus_file = "";
        if (runspecies == "run3pp" || runspecies == "run3auau")
        {
            clus_file = "/sphenix/lustre01/sphnxpro/production/" + runspecies + "/physics/" + clus_anacdbver + "/DST_TRKR_CLUSTER/run_" + nice_rounded_down.str() + "_" + nice_rounded_up.str() + "/DST_TRKR_CLUSTER_" + runspecies + "_" + clus_anacdbver + "-" + nice_runnumber.str() + "-" + nice_segment.str() + ".root";
        }
        if (runspecies == "run2pp")
        {
            clus_file = "/sphenix/lustre01/sphnxpro/production/" + runspecies + "/physics/" + clus_anacdbver + "/DST_TRKR_CLUSTER/run_" + nice_rounded_down.str() + "_" + nice_rounded_up.str() + "/dst/DST_TRKR_CLUSTER_" + runspecies + "_" + clus_anacdbver + "-" + nice_runnumber.str() + "-" + nice_segment.str() + ".root";
        }
        std::cout << "Input cluster DST: " << clus_file << std::endl;

        auto hitsinclus = new Fun4AllDstInputManager("ClusterInputManager");
        hitsinclus->fileopen(clus_file);
        se->registerInputManager(hitsinclus);
    }

    if (get_calo_info)
    {
        int ratio_nCalo_over_nTrkr = 10; // number of events per calo DST divided by number of events per tracking DST
        std::stringstream nice_segment_calo;
        nice_segment_calo << std::setw(5) << std::setfill('0') << to_string(segment / ratio_nCalo_over_nTrkr);

        std::string calo_anacdbver = "ana509_2024p022_v001";
        std::string calo_file = "";
        calo_file = "/sphenix/lustre01/sphnxpro/production2/" + runspecies + "/physics/calofitting/" + calo_anacdbver + "/run_" + nice_rounded_down.str() + "_" + nice_rounded_up.str() + "/DST_CALOFITTING_" + runspecies + "_" + calo_anacdbver + "-" + nice_runnumber.str() + "-" + nice_segment_calo.str() + ".root";
        std::cout << "Input calo DST: " << calo_file << std::endl;

        auto hitsincalo = new Fun4AllDstInputManager("CaloInputManager");
        hitsincalo->fileopen(calo_file);
        se->registerInputManager(hitsincalo);
    }

    std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");

    Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
    ingeo->AddFile(geofile);
    se->registerInputManager(ingeo);

    // set flags
    TRACKING::pp_mode = true;

    Enable::MVTX_APPLYMISALIGNMENT = true;
    ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;

    // distortion calibration mode
    /*
     * set to true to enable residuals in the TPC with
     * TPC clusters not participating to the ACTS track fit
     */
    G4TRACKING::SC_CALIBMODE = false;

    TpcReadoutInit(runnumber);
    // these lines show how to override the drift velocity and time offset values set in TpcReadoutInit
    // G4TPC::tpc_drift_velocity_reco = 0.0073844; // cm/ns
    // TpcClusterZCrossingCorrection::_vdrift = G4TPC::tpc_drift_velocity_reco;
    // G4TPC::tpc_tzero_reco = -5*50;  // ns
    std::cout << " run: " << runnumber << " samples: " << TRACKING::reco_tpc_maxtime_sample << " pre: " << TRACKING::reco_tpc_time_presample << " vdrift: " << G4TPC::tpc_drift_velocity_reco << std::endl;

    G4TPC::REJECT_LASER_EVENTS = true;
    G4TPC::ENABLE_MODULE_EDGE_CORRECTIONS = true;
    // Flag for running the tpc hit unpacker with zero suppression on
    TRACKING::tpc_zero_supp = true;

    // to turn on the default static corrections, enable the two lines below
    G4TPC::ENABLE_STATIC_CORRECTIONS = true;
    G4TPC::USE_PHI_AS_RAD_STATIC_CORRECTIONS = false;

    // to turn on the average corrections, enable the three lines below
    // note: these are designed to be used only if static corrections are also applied
    G4TPC::ENABLE_AVERAGE_CORRECTIONS = true;
    G4TPC::USE_PHI_AS_RAD_AVERAGE_CORRECTIONS = false;
    // to use a custom file instead of the database file:
    G4TPC::average_correction_filename = CDBInterface::instance()->getUrl("TPC_LAMINATION_FIT_CORRECTION");

    G4MAGNET::magfield_rescale = 1;
    TrackingInit();

    output_dir = "./"; // Top dir of where the output nTuples will be written
    trailer = "_" + nice_runnumber.str() + "_" + nice_segment.str() + "_" + nice_skip.str() + ".root";

    if (DoUnpacking)
    {
        for (int felix = 0; felix < 6; felix++)
        {
            Mvtx_HitUnpacking(std::to_string(felix));
        }
        for (int server = 0; server < 8; server++)
        {
            Intt_HitUnpacking(std::to_string(server));
        }
        std::ostringstream ebdcname;
        if (runspecies == "run3pp" || runspecies == "run3auau")
        {
            for (int ebdc = 0; ebdc < 24; ebdc++)
            {
                for (int endpoint = 0; endpoint < 2; endpoint++)
                {
                    ebdcname.str("");
                    if (ebdc < 10)
                    {
                        ebdcname << "0";
                    }
                    ebdcname << ebdc << "_" << endpoint;
                    Tpc_HitUnpacking(ebdcname.str());
                }
            }
        }
        if (runspecies == "run2pp")
        {
            for (int ebdc = 0; ebdc < 24; ebdc++)
            {
                ebdcname.str("");
                if (ebdc < 10)
                {
                    ebdcname << "0";
                }
                ebdcname << ebdc;
                Tpc_HitUnpacking(ebdcname.str());
            }
        }

        Micromegas_HitUnpacking();

        //se->registerSubsystem(new MvtxRawHitQA);
        //se->registerSubsystem(new InttRawHitQA);
        //se->registerSubsystem(new TpcRawHitQA);
    }

    if (DoClustering)
    {
        Mvtx_Clustering();

        Intt_Clustering();

        Tpc_LaserEventIdentifying();

        TPC_Clustering_run2pp();

        Micromegas_Clustering();

        Reject_Laser_Events();

        //se->registerSubsystem(new MvtxClusterQA);
        //se->registerSubsystem(new InttClusterQA);
        //se->registerSubsystem(new TpcClusterQA);
        //se->registerSubsystem(new MicromegasClusterQA);
    }

    if (DoSeeding)
    {
        Tracking_Reco_TrackSeed_run2pp();

        auto converter = new TrackSeedTrackMapConverter("SiliconSeedConverter");
        // Default set to full SvtxTrackSeeds. Can be set to
        // SiliconTrackSeedContainer or TpcTrackSeedContainer
        converter->setTrackSeedName("SiliconTrackSeedContainer");
        converter->setTrackMapName("SiliconSvtxTrackMap");
        converter->setFieldMap(G4MAGNET::magfield_tracking);
        converter->Verbosity(0);
        se->registerSubsystem(converter);

        auto finder = new PHSimpleVertexFinder("SiliconVertexFinder");
        finder->Verbosity(0);
        finder->setDcaCut(0.1);
        finder->setTrackPtCut(0.1);
        finder->setBeamLineCut(1);
        finder->setTrackQualityCut(1000000000);
        finder->setNmvtxRequired(3);
        finder->setOutlierPairCut(0.1);
        finder->setTrackMapName("SiliconSvtxTrackMap");
        finder->setVertexMapName("SiliconSvtxVertexMap");
        se->registerSubsystem(finder);

        auto siliconqa = new SiliconSeedsQA;
        siliconqa->setTrackMapName("SiliconSvtxTrackMap");
        siliconqa->setVertexMapName("SiliconSvtxVertexMap");
        se->registerSubsystem(siliconqa);

        auto convertertpc = new TrackSeedTrackMapConverter("TpcSeedConverter");
        // Default set to full SvtxTrackSeeds. Can be set to
        // SiliconTrackSeedContainer or TpcTrackSeedContainer
        convertertpc->setTrackSeedName("TpcTrackSeedContainer");
        convertertpc->setTrackMapName("TpcSvtxTrackMap");
        convertertpc->setFieldMap(G4MAGNET::magfield_tracking);
        convertertpc->Verbosity(0);
        se->registerSubsystem(convertertpc);

        auto findertpc = new PHSimpleVertexFinder("TpcSimpleVertexFinder");
        findertpc->Verbosity(0);
        findertpc->setDcaCut(0.5);
        findertpc->setTrackPtCut(0.2);
        findertpc->setBeamLineCut(1);
        findertpc->setTrackQualityCut(1000000000);
        // findertpc->setNmvtxRequired(3);
        findertpc->setRequireMVTX(false);
        findertpc->setOutlierPairCut(0.1);
        findertpc->setTrackMapName("TpcSvtxTrackMap");
        findertpc->setVertexMapName("TpcSvtxVertexMap");
        se->registerSubsystem(findertpc);

        //auto tpcqa = new TpcSeedsQA;
        //tpcqa->setTrackMapName("TpcSvtxTrackMap");
        //tpcqa->setVertexMapName("TpcSvtxVertexMap");
        //tpcqa->setSegment(rc->get_IntFlag("RUNSEGMENT"));
        //se->registerSubsystem(tpcqa);
    }

    if (DoFitting)
    {
        Tracking_Reco_TrackMatching_run2pp("TRKR_CLUSTER_SEED");

        G4TRACKING::convert_seeds_to_svtxtracks = convertSeeds;
        std::cout << "Converting to seeds : " << G4TRACKING::convert_seeds_to_svtxtracks << std::endl;
        /*
         * Either converts seeds to tracks with a straight line/helix fit
         * or run the full Acts track kalman filter fit
         */
        if (G4TRACKING::convert_seeds_to_svtxtracks)
        {
            auto *converter = new TrackSeedTrackMapConverter;
            // Default set to full SvtxTrackSeeds. Can be set to
            // SiliconTrackSeedContainer or TpcTrackSeedContainer
            converter->setTrackSeedName("SvtxTrackSeedContainer");
            converter->setFieldMap(G4MAGNET::magfield_tracking);
            converter->Verbosity(0);
            se->registerSubsystem(converter);
        }
        else
        {
            Tracking_Reco_TrackFit_run2pp("outfile.root", "TRKR_CLUSTER_SEED");
            //Tracking_Reco_TrackFit_run2pp();
        }

        // vertexing and propagation to vertex
        //Tracking_Reco_Vertex_run2pp();
        Tracking_Reco_Vertex_run2pp("TRKR_CLUSTER_SEED");

        //se->registerSubsystem(new TpcSiliconQA);
        //se->registerSubsystem(new TrackFittingQA);
        //se->registerSubsystem(new VertexQA);
    }

    if (get_calo_info)
    {
        std::cout << "Begin my calo reco" << std::endl;

        Process_Calo_Calib();

        Global_Reco();

        bool doEMcalRadiusCorr = true;
        auto projection = new PHActsTrackProjection("CaloProjection");
        if (doEMcalRadiusCorr)
        {
            projection->setLayerRadius(SvtxTrack::CEMC, cemc_proj_radius);
        }
        se->registerSubsystem(projection);

        CaloGeomMapping *cgm = new CaloGeomMapping();
        cgm->set_detector_name("CEMC");
        cgm->set_UseDetailedGeometry(true);
        se->registerSubsystem(cgm);

        //////////////////
        // Clusters
        std::cout << "Building clusters" << std::endl;
        RawClusterBuilderTemplate *ClusterBuilder = new RawClusterBuilderTemplate("EmcRawClusterBuilderTemplate");
        ClusterBuilder->Detector("CEMC");
        ClusterBuilder->set_UseDetailedGeometry(true);
        ClusterBuilder->set_threshold_energy(0.070); // for when using basic calibration
        std::string emc_prof = getenv("CALIBRATIONROOT");
        emc_prof += "/EmcProfile/CEMCprof_Thresh30MeV.root";
        // ClusterBuilder->set_UseAltZVertex(3); //0: GlobalVertexMap, 1: MbdVertexMap, 2: Nothing, 3: G4TruthInfo
        ClusterBuilder->LoadProfile(emc_prof);
        ClusterBuilder->set_UseTowerInfo(1); // to use towerinfo objects rather than old RawTower
        se->registerSubsystem(ClusterBuilder);
    }

    output_dir = outDir;

    if (run_pipi_reco)
        create_hf_directories(pipi_reconstruction_name, pipi_output_dir, pipi_output_reco_file);
    if (run_Kpi_reco)
        create_hf_directories(Kpi_reconstruction_name, Kpi_output_dir, Kpi_output_reco_file);
    if (run_KK_reco)
        create_hf_directories(KK_reconstruction_name, KK_output_dir, KK_output_reco_file);
    if (run_ppi_reco)
        create_hf_directories(ppi_reconstruction_name, ppi_output_dir, ppi_output_reco_file);
    if (run_ee_reco)
        create_hf_directories(ee_reconstruction_name, ee_output_dir, ee_output_reco_file);
    if (run_Lambdapi_reco)
        create_hf_directories(Lambdapi_reconstruction_name, Lambdapi_output_dir, Lambdapi_output_reco_file);
    if (run_LambdaK_reco)
        create_hf_directories(LambdaK_reconstruction_name, LambdaK_output_dir, LambdaK_output_reco_file);

    if (run_pipi_reco || run_Kpi_reco || run_KK_reco || run_ppi_reco || run_ee_reco || run_Lambdapi_reco || run_LambdaK_reco)
        init_kfp_dependencies();

    if (run_pipi_reco)
        reconstruct_pipi_mass();
    if (run_Kpi_reco)
        reconstruct_Kpi_mass();
    if (run_KK_reco)
        reconstruct_KK_mass();
    if (run_ppi_reco)
        reconstruct_ppi_mass();
    if (run_ee_reco)
        reconstruct_ee_mass();
    if (run_Lambdapi_reco)
        reconstruct_Lambdapi_mass();
    if (run_LambdaK_reco)
        reconstruct_LambdaK_mass();

    se->skip(nSkip);
    se->run(nEvents);
    se->End();
    se->PrintTimer();

    /*
    TString qaname = output_dir + "qaOut/HIST_" + nice_runnumber.str() + "_" + nice_segment.str() + "_" + nice_skip.str() + "_QA.root";
    std::string makeDirectory = "mkdir -p " + output_dir + "qaOut";
    if (run_pipi_reco)
    {
        qaname = pipi_output_dir + "qaOut/HIST_" + nice_runnumber.str() + "_" + nice_segment.str() + "_" + nice_skip.str() + "_QA.root";
        makeDirectory = "mkdir -p " + pipi_output_dir + "qaOut";
    }
    else if (run_Kpi_reco)
    {
        qaname = Kpi_output_dir + "qaOut/HIST_" + nice_runnumber.str() + "_" + nice_segment.str() + "_" + nice_skip.str() + "_QA.root";
        makeDirectory = "mkdir -p " + Kpi_output_dir + "qaOut";
    }
    else if (run_KK_reco)
    {
        qaname = KK_output_dir + "qaOut/HIST_" + nice_runnumber.str() + "_" + nice_segment.str() + "_" + nice_skip.str() + "_QA.root";
        makeDirectory = "mkdir -p " + KK_output_dir + "qaOut";
    }
    else if (run_ppi_reco)
    {
        qaname = ppi_output_dir + "qaOut/HIST_" + nice_runnumber.str() + "_" + nice_segment.str() + "_" + nice_skip.str() + "_QA.root";
        makeDirectory = "mkdir -p " + ppi_output_dir + "qaOut";
    }
    else if (run_ee_reco)
    {
        qaname = ee_output_dir + "qaOut/HIST_" + nice_runnumber.str() + "_" + nice_segment.str() + "_" + nice_skip.str() + "_QA.root";
        makeDirectory = "mkdir -p " + ee_output_dir + "qaOut";
    }
    else if (run_Lambdapi_reco)
    {
        qaname = Lambdapi_output_dir + "qaOut/HIST_" + nice_runnumber.str() + "_" + nice_segment.str() + "_" + nice_skip.str() + "_QA.root";
        makeDirectory = "mkdir -p " + Lambdapi_output_dir + "qaOut";
    }
    else if (run_LambdaK_reco)
    {
        qaname = ee_output_dir + "qaOut/HIST_" + nice_runnumber.str() + "_" + nice_segment.str() + "_" + nice_skip.str() + "_QA.root";
        makeDirectory = "mkdir -p " + LambdaK_output_dir + "qaOut";
    }
    std::cout << "Output QA file: " << qaname << std::endl;
    system(makeDirectory.c_str());
    std::string qaOutputFileName(qaname.Data());
    //QA_Output(qaOutputFileName);
    */

    if (run_pipi_reco)
        end_kfparticle(pipi_output_reco_file, pipi_output_dir);
    if (run_Kpi_reco)
        end_kfparticle(Kpi_output_reco_file, Kpi_output_dir);
    if (run_KK_reco)
        end_kfparticle(KK_output_reco_file, KK_output_dir);
    if (run_ppi_reco)
        end_kfparticle(ppi_output_reco_file, ppi_output_dir);
    if (run_ee_reco)
        end_kfparticle(ee_output_reco_file, ee_output_dir);
    if (run_Lambdapi_reco)
        end_kfparticle(Lambdapi_output_reco_file, Lambdapi_output_dir);
    if (run_LambdaK_reco)
        end_kfparticle(LambdaK_output_reco_file, LambdaK_output_dir);

    delete se;

    std::cout << "Finished" << std::endl;
    gSystem->Exit(0);
}
