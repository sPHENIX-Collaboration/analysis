#ifndef MACRO_FUN4ALL_G4_SPHENIX_HEPMC_C
#define MACRO_FUN4ALL_G4_SPHENIX_HEPMC_C

#include <GlobalVariables.C>

#include "G4Setup_sPHENIX.C"

#include <G4_Global.C>
#include <G4_Input.C>
#include <G4_Mbd.C>

#include "./G4_TrkrSimulation.C"
#include <Trkr_Clustering.C>
#include <Trkr_LaserClustering.C>
#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <trackreco/PHTruthVertexing.h>

#include <vertexcompare/VertexCompare.h>

#include <Rtypes.h>
#include <TSystem.h>

#include <iostream>
#include <string>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libg4eval.so)
R__LOAD_LIBRARY(libVertexCompare.so)

/**
 * Full sPHENIX simulation and standard reconstruction from a HepMC2 file.
 *
 * Chain:
 *   HepMC2
 *     -> PHHepMCGenEventMap
 *     -> HepMCNodeReader
 *     -> Geant4 detector simulation
 *     -> standard detector digitization
 *     -> standard clustering
 *     -> standard tracking reconstruction
 *     -> global vertex reconstruction
 *     -> VertexCompare analysis ntuple
 *     -> unfiltered DST
 *
 * Example:
 *
 * root -b -q \
 * 'Fun4All_G4_sPHENIX_HepMC.C(
 *    10,
 *    "oo200_hepmc2.hepmc",
 *    "DST_OO200_Angantyr.root",
 *    "VertexCompare_OO200_Angantyr.root",
 *    0,
 *    0,
 *    false)'
 */
int Fun4All_G4_sPHENIX_HepMC(                                                                                                                                       //
    const int nEvents = 5,                                                                                                                                          //
    const std::string &inputFile = "/sphenix/tg/tg01/hf/hjheng/ppg-dNdEta-OOpp/generator/pythia_angantyr/Gaussian/hepmc/oo200gev_angantyr_Gaussian_00000000.hepmc", //
    const std::string &dstOutputFile = "DST_OO200_Angantyr_test.root",                                                                                              //
    const std::string &analysisOutputFile = "VertexCompare_OO200_Angantyr_test.root",                                                                               //
    const int process = 0,                                                                                                                                          //
    const int skip = 0,                                                                                                                                             //
    const bool inputIsList = false                                                                                                                                  //
)
{
    Fun4AllServer *se = Fun4AllServer::instance();
    se->Verbosity(0);

    PHRandomSeed::Verbosity(1);
    CDBInterface::instance()->Verbosity(1);

    recoConsts *rc = recoConsts::instance();

    Enable::CDB = true;
    rc->set_StringFlag("CDB_GLOBALTAG", CDB::global_tag);
    rc->set_uint64Flag("TIMESTAMP", CDB::timestamp);
    rc->set_IntFlag("RUNNUMBER", 1);
    rc->set_IntFlag("RANDOMSEED", process + 12345);

    // --------------------------------------------------------------------------
    // HepMC input
    // --------------------------------------------------------------------------
    Input::VERBOSITY = 0;

    Input::HEPMC = true;
    Input::READHITS = false;
    Input::EMBED = false;
    Input::PILEUPRATE = 0.0;

    // Use the central sPHENIX O+O beam and collision-vertex configuration
    Input::BEAM_CONFIGURATION = Input::OO_COLLISION;

    INPUTHEPMC::filename.clear();
    INPUTHEPMC::listfile.clear();

    if (inputIsList)
    {
        INPUTHEPMC::listfile = inputFile;
    }
    else
    {
        INPUTHEPMC::filename = inputFile;
    }

    // Do not apply generator-specific afterburners to the Angantyr record
    INPUTHEPMC::HIJINGFLIP = false;
    INPUTHEPMC::FLOW = false;
    INPUTHEPMC::FERMIMOTION = false;
    INPUTHEPMC::REACTIONPLANERAND = false;
    INPUTHEPMC::HEPMC_STRANGENESS_FRACTION = -1.0;

    // Creates Fun4AllHepMCInputManager
    InputInit();

    // Apply the standard O+O beam crossing and collision-vertex distribution
    Input::ApplysPHENIXBeamParameter(INPUTMANAGER::HepMCInputManager);

    // Registers HepMCNodeReader, which copies particles from HepMC to Geant4
    InputRegister();

    // Standard event and run headers for generator input
    SyncReco *sync = new SyncReco();
    se->registerSubsystem(sync);

    HeadReco *head = new HeadReco();
    se->registerSubsystem(head);

    FlagHandler *flag = new FlagHandler();
    se->registerSubsystem(flag);

    // --------------------------------------------------------------------------
    // Detector configuration
    // --------------------------------------------------------------------------
    Enable::MBD = true;
    Enable::MBDRECO = true;
    Enable::MBDFAKE = false;

    Enable::PIPE = true;
    Enable::PIPE_ABSORBER = true;

    Enable::MVTX = true;
    Enable::INTT = true;
    Enable::TPC = true;
    Enable::MICROMEGAS = true;

    Enable::BEAMLINE = true;

    // Standard detector hit reconstruction.
    Enable::MVTX_CELL = true;
    Enable::INTT_CELL = true;
    Enable::TPC_CELL = true;
    Enable::MICROMEGAS_CELL = true;

    // Standard clustering.
    Enable::MVTX_CLUSTER = true;
    Enable::INTT_CLUSTER = true;
    Enable::TPC_CLUSTER = true;
    Enable::MICROMEGAS_CLUSTER = true;

    // Standard full tracking and global reconstruction.
    Enable::TRACKING_TRACK = true;
    Enable::GLOBAL_RECO = true;

    // This sample contains one in-time O+O collision per HepMC event.
    TRACKING::streaming_mode = false;

    // --------------------------------------------------------------------------
    // Geant4 detector simulation
    // --------------------------------------------------------------------------
    G4Init();
    G4Setup();

    // --------------------------------------------------------------------------
    // Standard detector reconstruction
    // --------------------------------------------------------------------------
    if (Enable::MBD && Enable::MBDRECO)
    {
        Mbd_Reco();
    }

    if (Enable::MVTX_CELL)
    {
        Mvtx_Cells();
    }

    if (Enable::INTT_CELL)
    {
        Intt_Cells();
    }

    if (Enable::TPC_CELL)
    {
        TPC_Cells();
    }

    if (Enable::MICROMEGAS_CELL)
    {
        Micromegas_Cells();
    }

    if (Enable::TRACKING_TRACK)
    {
        TrackingInit();
    }

    if (Enable::MVTX_CLUSTER)
    {
        Mvtx_Clustering();
    }

    if (Enable::INTT_CLUSTER)
    {
        Intt_Clustering();
    }

    if (Enable::TPC_CLUSTER)
    {
        if (G4TPC::ENABLE_DIRECT_LASER_HITS || G4TPC::ENABLE_CENTRAL_MEMBRANE_HITS)
        {
            TPC_LaserClustering();
        }
        else
        {
            TPC_Clustering();
        }
    }

    if (Enable::MICROMEGAS_CLUSTER)
    {
        Micromegas_Clustering();
    }

    if (Enable::TRACKING_TRACK)
    {
        // Central sPHENIX tracking wrapper:
        // silicon seeding, TPC seeding, matching, track fitting, cleaning,
        // track vertexing, and track propagation.
        Tracking_Reco();
    }

    if (Enable::GLOBAL_RECO)
    {
        Global_Reco();
    }

    // Truth vertex map used by the simulation analysis.
    PHTruthVertexing *truthVertexing = new PHTruthVertexing();
    truthVertexing->associate_tracks(false);
    se->registerSubsystem(truthVertexing);

    // --------------------------------------------------------------------------
    // User analysis module
    // --------------------------------------------------------------------------
    VertexCompareVerbosity::fillSilconSeed = 0;
    VertexCompareVerbosity::fillCluster = 0;
    VertexCompareVerbosity::fillTruthParticle = 0;

    VertexCompare *compare = new VertexCompare();
    compare->IsSimulation();
    compare->doTrackOutput();
    compare->doTpcSeedOutput();
    compare->setOutputName(analysisOutputFile.c_str());
    se->registerSubsystem(compare);

    // --------------------------------------------------------------------------
    // Input manager and DST output
    // --------------------------------------------------------------------------
    InputManagers();

    Fun4AllDstOutputManager *dstOutput = new Fun4AllDstOutputManager("DSTOUT", dstOutputFile);
    se->registerOutputManager(dstOutput);

    // --------------------------------------------------------------------------
    // Event processing
    // --------------------------------------------------------------------------
    if (nEvents < 0)
    {
        return 0;
    }

    se->skip(skip);
    se->run(nEvents);

    CDBInterface::instance()->Print();

    se->End();
    se->PrintTimer();

    std::cout << "Fun4All_G4_sPHENIX_HepMC completed." << std::endl;
    std::cout << "DST:      " << dstOutputFile << std::endl;
    std::cout << "Analysis: " << analysisOutputFile << std::endl;

    delete se;

    gSystem->Exit(0);
    return 0;
}

#endif