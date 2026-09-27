// Fun4All headers
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <GlobalVariables.C>

//#include <G4Setup_sPHENIX_Bbc.C>
#include <G4Setup_sPHENIX.C>
// #include <G4_Bbc.C>
// #include <G4_CaloTrigger.C>
// #include <G4_Centrality.C>
// #include <G4_DSTReader.C>
#include <G4_Global.C>
// #include <G4_HIJetReco.C>
#include <G4_Input.C>
// #include <G4_Jets.C>
// #include <G4_KFParticle.C>
// #include <G4_ParticleFlow.C>
// #include <G4_Production.C>
// #include <G4_TopoClusterReco.C>

#include <Trkr_RecoInit.C>
#include <Trkr_Clustering.C>
#include <Trkr_LaserClustering.C>
#include <Trkr_Reco.C>
#include <Trkr_Eval.C>
#include <G4_ActsGeom.C>
// #include <Trkr_QA.C>

// #include <Trkr_Diagnostics.C>
// #include <G4_User.C>
// #include <QA.C>
#include <trackreco/PHActsTrackProjection.h>
#include <phpythia8/PHPy8ParticleTrigger.h>
#include <phpythia8/PHPy8JetTrigger.h>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <iostream>
#include <typeinfo>

R__LOAD_LIBRARY(libfun4all.so)

// #include <physiTuto/tutorial.h>
#include <tutorial/tutorial.h>
#include <tutorial/TruthToSvtxTrack.h>
R__LOAD_LIBRARY( libtutorial.so )

#include <caloreco/CaloGeomMapping.h>
// #include <caloreco/CaloGeomMappingv2.h>
R__LOAD_LIBRARY(libcalo_reco.so)

void Fun4All_physiTuto(
	int nEvents = 10,
	const double particle_pT = 1.0,
    const double energy_range_up = 0.0,
    const double energy_range_down = 0.0,
    const string particle_species = "Electron",
    // const string &output_directory = "/sphenix/user/jzhang1/INTT-EMCAL/InttSeedingTrackDev/ParticleGen/output/",
    const string &output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/jingyu/EMCpositionModify/ParticleGen/output/",
    const string &output_filename = "results_test.root"
)
{
    std::cout << "CEMC_Clusters function is located at: " << (void*) CEMC_Clusters << std::endl;

    const int skip = 0;
	const bool is_pythia = false;
    const int Nparticle = 1;

    // const bool is_pythia = true;
    map<string, string> particle_map  = {
        {"MuonPlus", "mu+"},
        {"Muon", "mu-"},
        {"PionMinus", "pi-"},
        {"PionPlus", "pi+"},
        {"Electron", "e-"},
        {"Positron", "e+"}
        // {"Proton", "proton"},
        // {"Kaon", "kaon-"}
    };

    if (particle_map.find(particle_species) == particle_map.end())
    {
        cout << "Invalid particle species: " << particle_species << endl;
        return;
    }

    Fun4AllServer *se = Fun4AllServer::instance();
    //  se->Print("NODETREE"); // useless
    se->Verbosity(0);

    //Opt to print all random seed used for debugging reproducibility. Comment out to reduce stdout prints.
    //PHRandomSeed::Verbosity(1);

    // just if we set some flags somewhere in this macro
    recoConsts *rc = recoConsts::instance();
    // By default every random number generator uses
    // PHRandomSeed() which reads /dev/urandom to get its seed
    // if the RANDOMSEED flag is set its value is taken as seed
    // You can either set this to a random value using PHRandomSeed()
    // which will make all seeds identical (not sure what the point of
    // this would be:
    //  rc->set_IntFlag("RANDOMSEED",PHRandomSeed());
    // or set it to a fixed value so you can debug your code
    //  rc->set_IntFlag("RANDOMSEED", 12345);

    // INPUTREADHITS::filename[0] = inputFile;

    // Force to use Pythia8 or GUN
    Input::PYTHIA8 = is_pythia;
    Input::GUN = !( is_pythia );
    Input::SIMPLE = true;
    if( Input::PYTHIA8 == false )
    {
        Input::GUN_NUMBER = 1; // if you need 3 of them
        Input::GUN_VERBOSITY = 1;
    }

    // Initialize the selected Input/Event generation
    InputInit();

    ///////////////
    // Set generator specific options
    ////////////////
    // can only be set after InputInit() is called
  
    // pythia8
    if (Input::PYTHIA8)
    {
        // //! Nominal collision geometry is selected by Input::BEAM_CONFIGURATION
        // Input::ApplysPHENIXBeamParameter(INPUTGENERATOR::Pythia8);
        //! Nominal collision geometry is selected by Input::BEAM_CONFIGURATION

        PHPy8ParticleTrigger *p8_trigger = new PHPy8ParticleTrigger();
        p8_trigger->AddParticles(443); // J/psi
        p8_trigger->SetStableParticleOnly(false);

        INPUTGENERATOR::Pythia8->register_trigger(p8_trigger);
        INPUTGENERATOR::Pythia8->set_trigger_AND();

        /////////////////////// Part to Tune the Phytia beam parameters started //////////////////// 
        // Option 1) You can use some default configuration provided by sPHENIX
        Input::ApplysPHENIXBeamParameter(INPUTGENERATOR::Pythia8, Input::pp_COLLISION);
        //    Input::ApplysPHENIXBeamParameter(INPUTGENERATOR::Pythia8,Input::pp_ZEROANGLE);
        //    Input::ApplysPHENIXBeamParameter(INPUTGENERATOR::Pythia8,Input::AA_COLLISION);

        // Option 2) You can set the beam crossing or set the vertex_distribution_width at the INPUT generator level
        if (false)
        {
          Input::beam_crossing = 1.;
          double localbcross = Input::beam_crossing / 2. * 1e-3;
          localbcross = Input::beam_crossing / 2. * 1e-3;
          //  Xing angle is split among both beams, means set to 0.5 mRad
          INPUTGENERATOR::Pythia8->set_beam_direction_theta_phi(localbcross, 0, M_PI - localbcross, 0); // 1.5mrad x-ing of sPHENIX
          INPUTGENERATOR::Pythia8->set_vertex_distribution_width(
            120e-4,         // approximation from past PHENIX data
            120e-4,         // approximation from past PHENIX data
            16,             // measured in 2024 for 1.5mrad Xing angle
            20 / 29.9792);  // 20cm collision length / speed of light in cm/ns
        }
        /////////////////////// Part to Tune the Phytia beam parameters done ////////////////////
    }
    else // for GUN
    {
        // particle gun
        // if you run more than one of these Input::GUN_NUMBER > 1
        // add the settings for other with [1], next with [2]...
        // INPUTGENERATOR::Gun[0]->AddParticle("pi-", 0, 1, 0.1 );
        // INPUTGENERATOR::Gun[0]->set_vtx(0, 0, 0);

        INPUTGENERATOR::SimpleEventGenerator[0] -> add_particles(particle_map[particle_species.c_str()], Nparticle );
        // INPUTGENERATOR::SimpleEventGenerator[0] -> set_name("mu-");
        INPUTGENERATOR::SimpleEventGenerator[0] -> set_vtx(0, 0, 0);
        INPUTGENERATOR::SimpleEventGenerator[0] -> set_pt_range(particle_pT - 9.7, particle_pT + energy_range_up);
        // INPUTGENERATOR::SimpleEventGenerator[0] -> set_mom(120, 0, 0);
        INPUTGENERATOR::SimpleEventGenerator[0] -> set_eta_range(-1.1, 1.1); // +-1
        // INPUTGENERATOR::SimpleEventGenerator[0] -> set_eta_range(-0.01, 0.01);
        INPUTGENERATOR::SimpleEventGenerator[0] -> set_phi_range(-M_PI, M_PI);
    }

  InputRegister();

  // Flag Handler is always needed to read flags from input (if used)
  // and update our rc flags with them. At the end it saves all flags
  // again on the DST in the Flags node under the RUN node
  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);

  //======================
  // Write the DST
  //======================

  //Enable::DSTOUT = true;
  Enable::DSTOUT_COMPRESS = false;
  // DstOut::OutputDir = outdir;
  // DstOut::OutputFile = outputFile;

  //Option to convert DST to human command readable TTree for quick poke around the outputs
  //  Enable::DSTREADER = true;

  // turn the display on (default off)
  //Enable::DISPLAY = true;

  //======================
  // What to run
  //======================
  // QA, main switch
  Enable::QA					= false;

  Enable::PIPE					= true;
  Enable::PIPE_ABSORBER	= true;

  // central tracking
  Enable::MVTX					= true;
  Enable::MVTX_CELL			= Enable::MVTX && true;
  Enable::MVTX_CLUSTER	= Enable::MVTX_CELL && true;
  Enable::MVTX_QA				= Enable::MVTX_CLUSTER && Enable::QA && true;

  Enable::INTT					= true;
  //  Enable::INTT_ABSORBER			= true; // enables layerwise support structure readout
  //  Enable::INTT_SUPPORT			= true; // enable global support structure readout
  Enable::INTT_CELL				= Enable::INTT && true;
  Enable::INTT_CLUSTER		= Enable::INTT_CELL && true;
  Enable::INTT_QA				  = Enable::INTT_CLUSTER && Enable::QA && true;
  
  Enable::TPC					  = true;
  Enable::TPC_ABSORBER	= true;
  Enable::TPC_CELL			= Enable::TPC && true;
  Enable::TPC_CLUSTER		= Enable::TPC_CELL &&true;
  Enable::TPC_QA				= Enable::TPC_CLUSTER && Enable::QA && false;

  Enable::MICROMEGAS			  	= true;
  Enable::MICROMEGAS_CELL			= Enable::MICROMEGAS && true;
  Enable::MICROMEGAS_CLUSTER	= Enable::MICROMEGAS_CELL && true;
  Enable::MICROMEGAS_QA				= Enable::MICROMEGAS_CLUSTER && Enable::QA && false;

  Enable::TRACKING_TRACK	=  true;
  
  Enable::TRACKING_EVAL		= Enable::TRACKING_TRACK && false;
  Enable::TRACKING_QA			= Enable::TRACKING_TRACK && Enable::QA && true; // <-- this one


  //! forward flux return plug door. Out of acceptance and off by default.
  //Enable::PLUGDOOR				= true;
  Enable::PLUGDOOR_ABSORBER			= true;


  Enable::CEMC = true;
  Enable::CEMC_ABSORBER = true;
  Enable::CEMC_CELL = Enable::CEMC && true;
  Enable::CEMC_TOWER = Enable::CEMC_CELL && true;
  Enable::CEMC_CLUSTER = Enable::CEMC_TOWER && true;
  Enable::CEMC_EVAL = Enable::CEMC_CLUSTER && true;
  Enable::CEMC_QA = Enable::CEMC_CLUSTER && Enable::QA && false;

  Enable::HCALIN = true;
  Enable::HCALIN_ABSORBER = true;
  Enable::HCALIN_CELL = Enable::HCALIN && true;
  Enable::HCALIN_TOWER = Enable::HCALIN_CELL && true;
  Enable::HCALIN_CLUSTER = Enable::HCALIN_TOWER && true;
  Enable::HCALIN_EVAL = Enable::HCALIN_CLUSTER && true;
  Enable::HCALIN_QA = Enable::HCALIN_CLUSTER && Enable::QA && false;


  Enable::MAGNET				= true;
  Enable::MAGNET_ABSORBER			= true;


  Enable::HCALOUT = true;
  Enable::HCALOUT_ABSORBER = true;
  Enable::HCALOUT_CELL = Enable::HCALOUT && true;
  Enable::HCALOUT_TOWER = Enable::HCALOUT_CELL && true;
  Enable::HCALOUT_CLUSTER = Enable::HCALOUT_TOWER && true;
  Enable::HCALOUT_EVAL = Enable::HCALOUT_CLUSTER && true;
  Enable::HCALOUT_QA = Enable::HCALOUT_CLUSTER && Enable::QA && false;


  // new settings using Enable namespace in GlobalVariables.C
  Enable::BLACKHOLE				= true;
  //Enable::BLACKHOLE_SAVEHITS			= false; // turn off saving of bh hits
  //Enable::BLACKHOLE_FORWARD_SAVEHITS		= false; // disable forward/backward hits
  //BlackHoleGeometry::visible			= true;

  //////////////////////////////
  // conditions DB flags      //
  //////////////////////////////
  Enable::CDB					= true;
  // global tag
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);
  
    /////////////////
    // Magnet Settings
    /////////////////
    // G4MAGNET::magfield_rescale = 1.;  // for zero field   
    MagnetInit();

    // Initialize the selected subsystems
    G4Init();

    ///////////////////////
    // GEANT4 Detector description
    ///////////////////////
    // if (!Input::READHITS) G4Setup();
    G4Setup();

  ///////////////////////
  // Detector Division //
  ///////////////////////

  //  if ((Enable::MBD && Enable::MBDRECO) || Enable::MBDFAKE) Mbd_Reco();
  Mbd_Reco();
  if (Enable::MVTX_CELL) Mvtx_Cells();
  if (Enable::INTT_CELL) Intt_Cells();

  //////////////////////////////////
  // CEMC towering and clustering //
  //////////////////////////////////  
  if (Enable::CEMC_CELL) CEMC_Cells();
  if (Enable::CEMC_TOWER) CEMC_Towers();
  if (Enable::CEMC_CLUSTER) CEMC_Clusters();
    
  if (Enable::HCALIN_CELL) HCALInner_Cells();
  if (Enable::HCALOUT_CELL) HCALOuter_Cells();
  if (Enable::HCALIN_TOWER) HCALInner_Towers();
  if (Enable::HCALIN_CLUSTER) HCALInner_Clusters();
  if (Enable::HCALOUT_TOWER) HCALOuter_Towers();
  if (Enable::HCALOUT_CLUSTER) HCALOuter_Clusters();


    /////////////////
    // SVTX tracking
    ////////////////
    // Full tracker
    // if(Enable::TRACKING_TRACK) TrackingInit();
    // if (Enable::MVTX_CLUSTER) Mvtx_Clustering();
    // if (Enable::INTT_CLUSTER) Intt_Clustering();
    // if (Enable::TRACKING_TRACK) Tracking_Reco();

    // Silicon only
    ACTSGEOM::ActsGeomInit();
    // if(Enable::TRACKING_TRACK) TrackingInit();
    if (Enable::MVTX_CLUSTER) Mvtx_Clustering();
    if (Enable::INTT_CLUSTER) Intt_Clustering();
    // if (Enable::TRACKING_TRACK) Tracking_Reco();

    auto silicon_Seeding = new PHActsSiliconSeeding;
    // silicon_Seeding->set_track_map_name("SvtxTrackSeedContainer");
    silicon_Seeding->Verbosity(0);
    silicon_Seeding->setinttRPhiSearchWindow(0.2);
    silicon_Seeding->setinttZSearchWindow(1.0);
    silicon_Seeding->seedAnalysis(false);
    se->registerSubsystem(silicon_Seeding);

    auto merger = new PHSiliconSeedMerger;
    // merger->trackMapName("SvtxTrackSeedContainer");
    merger->Verbosity(0);
    se->registerSubsystem(merger);

    auto converter = new TrackSeedTrackMapConverter;
    // SiliconTrackSeedContainer or TpcTrackSeedContainer
    converter->setTrackSeedName("SiliconTrackSeedContainer");
    converter->setFieldMap(G4MAGNET::magfield_tracking);
    converter->Verbosity(10);
    se->registerSubsystem(converter);
    PHSimpleVertexFinder *finder = new PHSimpleVertexFinder;
    finder->Verbosity(0);
    finder->setDcaCut(0.1);
    finder->setTrackPtCut(0.);
    finder->setBeamLineCut(1);
    finder->setTrackQualityCut(20);
    finder->setNmvtxRequired(3);
    finder->setOutlierPairCut(0.1);
    se->registerSubsystem(finder);

    Global_Reco();
    build_truthreco_tables();

    //////////////////////
    // Simulation evaluation
    //////////////////////

    bool doEMcalRadiusCorr = true;
    auto projection = new PHActsTrackProjection("CaloProjection");
    float new_cemc_rad = 93.5; // Virgile recommendation according to DetailedCalorimeterGeometry
    if (doEMcalRadiusCorr)
    {
        projection->setLayerRadius(SvtxTrack::CEMC, new_cemc_rad);
    }
    se->registerSubsystem(projection);
    
    // Load the modified geometry
    // CaloGeomMappingv2 *cgm = new CaloGeomMappingv2();
    // cgm->set_detector_name("CEMC");
    // cgm->setTowerGeomNodeName("TOWERGEOM_CEMCv3");
    // se->registerSubsystem(cgm);
    CaloGeomMapping *cgm = new CaloGeomMapping();
    cgm->set_detector_name("CEMC");
    cgm->set_UseDetailedGeometry(true);
    se->registerSubsystem(cgm);

    // TruthToSvtxTrack *t2t = new TruthToSvtxTrack();
    // se->registerSubsystem(t2t);

    // auto truthProjection = new PHActsTrackProjection("TruthTrackProjection");
    // bool doEMcalRadiusCorr = true; // Set to true to apply the radius correction
    // float new_cemc_rad = 99;
    // if (doEMcalRadiusCorr)
    // {
    //     truthProjection->setLayerRadius(SvtxTrack::CEMC, new_cemc_rad);
    // }
    // se->registerSubsystem(truthProjection);

    // // tutorial info get
    tutorial* analysis_module = new tutorial( "name", output_directory, output_filename);
    // Read the same EMCal tower geometry node used by the standard CEMC clustering.
    // The analysis module itself also has a fallback to TOWERGEOM_CEMCv3.
    analysis_module->setTowerGeomNodeName("TOWERGEOM_CEMC");
    // string output_path = "tutorial_results_";
    // output_path += "pythia8_MC.root";
    // analysis_module->SetOutputPath(output_path);
    se->registerSubsystem(analysis_module);
  
    // auto siana = new SiliconSeedsAna("SiliconSeedsAna");
    // siana->setMC(true);
    // siana->setVtxSkip(true);
    // siana->setTopoCluster(useTopologicalCluster);
    // siana->setOutputFileName(outputName2);
    // int startnumber = nEvents * std::stoi(processID);
    // siana->setStartEventNumber(startnumber);
    // se->registerSubsystem(siana);

    // Flag Handler is always needed to read flags from input (if used)
    // and update our rc flags with them. At the end it saves all flags
    // again on the DST in the Flags node under the RUN node
    // FlagHandler *flag = new FlagHandler();
    // se->registerSubsystem(flag);
    se->skip(skip);
    se->run(nEvents);
    se->End();
    
    std::cout << "All done!" << std::endl;

    gSystem->Exit(0);
    return;
}


