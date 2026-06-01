#ifndef MACRO_FUN4ALLG4SPHENIX_C
#define MACRO_FUN4ALLG4SPHENIX_C

#include <GlobalVariables.C>

#include "G4Setup_sPHENIX.C"
#include "RecoScanConfig.h"

#include <DisplayOn.C>
#include <G4_Mbd.C>
#include <G4_CaloTrigger.C>
#include <G4_Centrality.C>
#include <G4_DSTReader.C>
#include <G4_Global.C>
#include <G4_HIJetReco.C>
#include <G4_Input.C>
#include <G4_Jets.C>
#include <G4_KFParticle.C>
#include <G4_ParticleFlow.C>
#include <G4_Production.C>
#include <G4_TopoClusterReco.C>

#include <Trkr_RecoInit.C>
#include <Trkr_Clustering.C>
#include <Trkr_LaserClustering.C>
#include "./Trkr_Reco.C" // for testing purpose
#include <Trkr_Eval.C>
#include <Trkr_QA.C>

#include <Trkr_Diagnostics.C>
#include <G4_User.C>
#include <QA.C>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <calotrigger/MinimumBiasClassifier.h>
#include <centrality/CentralityReco.h>
#include <g4centrality/PHG4CentralityReco.h>
#include <trackreco/PHTruthVertexing.h>

#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloTowerCalib.h>
#include <globalvertex/GlobalVertexReco.h>
#include <mbd/MbdReco.h>
#include <zdcinfo/ZdcReco.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <Rtypes.h> // resolves R__LOAD_LIBRARY for clang-tidy
#include <TROOT.h>

#include <trackreco/PHCASiliconSeeding.h> // CA silicon seeding

#include <g4eval/SvtxTruthRecoTableEval.h>

#include <vertexcompare/VertexCompare.h>
#include <limits>
#include <sstream>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)

R__LOAD_LIBRARY(libmbd.so)
R__LOAD_LIBRARY(libglobalvertex.so)
R__LOAD_LIBRARY(libVertexCompare.so)
// R__LOAD_LIBRARY(libfun4allraw.so)
// R__LOAD_LIBRARY(libffarawmodules.so)
R__LOAD_LIBRARY(libcentrality_io.so)
R__LOAD_LIBRARY(libcentrality.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libcalotrigger.so)
// R__LOAD_LIBRARY(libcentralityvalid.so)
R__LOAD_LIBRARY(libzdcinfo.so)

R__LOAD_LIBRARY(libg4eval.so)

// For HepMC Hijing
// try inputFile = /sphenix/sim/sim01/sphnxpro/sHijing_HepMC/sHijing_0-12fm.dat

int Fun4All_G4_sPHENIX(const int nEvents = 1,                                             //
                       const string generator = "HIJING",                                 //
                       const std::string &seedingalgo = "ACTS",                           //
                       const std::string &outputFile = "test.root",                       //
                       const int process = 0,                                             //
                       const std::string &scanName = "",                                  //
                       const double scanValue = std::numeric_limits<double>::quiet_NaN(), //
                       const std::string &scanTag = ""                                    //
)
{
    bool UseActsSiliconSeeding = true;
    if (seedingalgo == "CA")
    {
        UseActsSiliconSeeding = false;
    }
    else if (seedingalgo == "ACTS")
    {
        UseActsSiliconSeeding = true;
    }
    else
    {
        std::cout << "Seeding algorithm " << seedingalgo << " not currently supported, exiting now and please check input." << std::endl;
        return -1;
    }
    const std::string productionTag = "OO_0_15fm";
    const std::string species = "OO";
    const int runNumber = 1;
    std::string compareOutputFile = outputFile;

    gRecoScanConfig = RecoScanConfig{};
    if (!scanName.empty() && !applyScanOverride(gRecoScanConfig, scanName, scanValue))
    {
        std::cout << "Scan variable " << scanName << " not currently supported or has an invalid value, exiting now and please check input." << std::endl;
        return -1;
    }

    std::string effectiveTag = scanTag;
    if (effectiveTag.empty() && !scanName.empty())
    {
        effectiveTag = RecoScan::buildTagToken(scanName, scanValue);
    }

    std::string defaultSuffix = buildRecoScanDefaultSuffix(scanName, gRecoScanConfig);

    if (!defaultSuffix.empty())
    {
        if (!effectiveTag.empty())
        {
            effectiveTag += "_" + defaultSuffix;
        }
        else
        {
            effectiveTag = defaultSuffix;
        }
    }

    if (!effectiveTag.empty())
    {
        const auto slashPos = compareOutputFile.find_last_of("/");
        if (slashPos == std::string::npos)
        {
            compareOutputFile = effectiveTag + "/" + compareOutputFile;
        }
        else
        {
            compareOutputFile = compareOutputFile.substr(0, slashPos + 1) + effectiveTag + compareOutputFile.substr(slashPos);
        }
    }

    Fun4AllServer *se = Fun4AllServer::instance();
    se->Verbosity(0);

    //===============
    // Input options
    //===============
    // verbosity setting (applies to all input managers)
    Input::VERBOSITY = 0;
    // First enable the input generators
    // Either:
    // read previously generated g4-hits files, in this case it opens a DST and skips
    // the simulations step completely. The G4Setup macro is only loaded to get information
    // about the number of layers used for the cell reco code
    Input::READHITS = true;
    bool fromG4Hits = false;
    if (generator == "HIJING")
    {
        // G4 hits files (Pass1)
        if (fromG4Hits)
        {
            // Run35, OO 0-15fm, no pileup
            INPUTREADHITS::filename[0] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/OO_0_15fm/g4hits/run0035/G4Hits_sHijing_OO_0_15fm-0000000035-" + std::string(Form("%06d", process)) + ".root";
            // INPUTREADHITS::filename[0] = "/sphenix/user/adeebsaed/macros/detectors/sPHENIX/outputfiles_fun4all/G4sPHENIX-0000000001-" + std::string(Form("%06d", process)) + ".root";
        }
        else
        {
            // TRKR G4HIT (Pass2)
            // Run34, OO 0-15fm, pileup, 220kHz
            INPUTREADHITS::filename[0] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/OO_0_15fm/pileup/run0034/220kHz/DST_TRKR_G4HIT_sHijing_OO_0_15fm_220kHz_bkg_0_15fm-0000000034-" + std::string(Form("%06d", process)) + ".root";

            // TRUTH_G4HIT (Pass2)
            // Run34, OO 0-15fm, pileup, 220kHz
            INPUTREADHITS::filename[1] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/OO_0_15fm/pileup/run0034/220kHz/DST_TRUTH_G4HIT_sHijing_OO_0_15fm_220kHz_bkg_0_15fm-0000000034-" + std::string(Form("%06d", process)) + ".root";

            // MBD/sEPD (Pass3, MBD/EPD)
            // Run34, OO 0-15fm, pileup, 220kHz
            INPUTREADHITS::filename[2] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/OO_0_15fm/mbdepd/run0034/220khz/DST_MBD_EPD_sHijing_OO_0_15fm_220kHz_bkg_0_15fm-0000000034-" + std::string(Form("%06d", process)) + ".root";

            // TRUTH (Pass3 Track)
            // Run34, OO 0-15fm, pileup, 220kHz
            INPUTREADHITS::filename[3] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/OO_0_15fm/trkrhit/run0034/220khz/DST_TRUTH_sHijing_OO_0_15fm_220kHz_bkg_0_15fm-0000000034-" + std::string(Form("%06d", process)) + ".root";

            // TrkrHits (Pass3 Track)
            // Run34, OO 0-15fm, pileup, 220kHz
            INPUTREADHITS::filename[4] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/OO_0_15fm/trkrhit/run0034/220khz/DST_TRKR_HIT_sHijing_OO_0_15fm_220kHz_bkg_0_15fm-0000000034-" + std::string(Form("%06d", process)) + ".root";

            // TrkrClusters (Pass4 Job0)
            // Run34, OO 0-15fm, pileup, 220kHz
            INPUTREADHITS::filename[5] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/OO_0_15fm/trkrcluster/run0034/220khz/DST_TRKR_CLUSTER_sHijing_OO_0_15fm_220kHz_bkg_0_15fm-0000000034-" + std::string(Form("%06d", process)) + ".root";

            // TRACKSEEDS (Pass4 JobA)
            // Run34, OO 0-15fm, pileup, 220kHz
            // INPUTREADHITS::filename[6] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/OO_0_15fm/nopileup/trackseeds/run0035/DST_TRACKSEEDS_sHijing_OO_0_15fm-0000000035-" + std::string(Form("%06d", process)) + ".root";

            // GLOBAL (Pass5 Global) --> disable, because we want to build vertices from silicon instead of using the ones in DSTs which are built from the full tracks
            // Run34, OO 0-15fm, pileup, 220kHz
            // INPUTREADHITS::filename[7] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/OO_0_15fm/global/run0034/220khz/DST_GLOBAL_sHijing_OO_0_15fm_220kHz_bkg_0_15fm-0000000034-" + std::string(Form("%06d", process)) + ".root";

            // TRUTH RECO (Pass5 Truth Track) --> disable, build the reco-truth matching map/container from SvtxTrackEval
            // Run34, OO 0-15fm, pileup, 220kHz
            // INPUTREADHITS::filename[8] = "/sphenix/lustre01/sphnxpro/mdc2/shijing_hepmc/OO_0_15fm/truthreco/run0034/220khz/DST_TRUTH_RECO_sHijing_OO_0_15fm_220kHz_bkg_0_15fm-0000000034-" + std::string(Form("%06d", process)) + ".root";
        }
    }
    else
    {
        std::cout << "Generator " << generator << " not currently supported, exiting now and please check input." << std::endl;
        return -1;
    }

    //======================
    // What to run
    //======================

    // QA, main switch
    // Enable::QA = true;
    // Global options (enabled for all enables subsystems - if implemented)
    //  Enable::ABSORBER = true;
    //  Enable::OVERLAPCHECK = true;
    //  Enable::VERBOSITY = 1;

    Enable::MBD = true;
    // Enable::MBD_SUPPORT = true; // save hist in MBD/BBC support structure
    Enable::MBDRECO = Enable::MBD && true;
    // Enable::MBDFAKE = true; // Smeared vtx and t0, use if you don't want real MBD/BBC in simulation

    Enable::PIPE = true;
    Enable::PIPE_ABSORBER = true;

    // central tracking
    Enable::MVTX = true;
    Enable::INTT = true;
    Enable::TPC = true;
    Enable::MICROMEGAS = true;
    Enable::BEAMLINE = true;

    //===============
    // conditions DB flags
    //===============
    recoConsts *rc = recoConsts::instance();
    Enable::CDB = true;
    // global tag
    rc->set_StringFlag("CDB_GLOBALTAG", CDB::global_tag);
    rc->set_uint64Flag("TIMESTAMP", CDB::timestamp);
    rc->set_IntFlag("RUNNUMBER", runNumber);

    // Initialize the selected subsystems
    G4Init();
    if (!fromG4Hits) G4Setup(); // disable this if run from g4hits

    Mbd_Reco();
    if (fromG4Hits)
    {
        Mvtx_Cells();
        Intt_Cells();
    }

    TrackingInit();

    if (fromG4Hits)
    {
        Mvtx_Clustering();
        Intt_Clustering();
    }

    // Silicon Seeding
    bool doSiliconSeeding = true;
    Enable::TRACKING_VERBOSITY = 10;
    if (doSiliconSeeding)
    {
        if (UseActsSiliconSeeding)
        {
            Tracking_Reco_SiliconSeed_run2pp(); // default Acts silicon seeding, iterative
        }
        else // CA silicon seeding, one pass
        {
            auto silicon_Seeding = new PHCASiliconSeeding();
            silicon_Seeding->Verbosity(0);
            // silicon_Seeding->SetTrackMapName("SiliconTrackSeedContainer_CA");
            silicon_Seeding->SetMVTXStrobeIDRange(-1, 1);
            silicon_Seeding->SetLayerRange(0, 6);         // MVTX + INTT
            silicon_Seeding->SetSearchWindow(1.5, 0.2);   // SetSearchWindow(float eta_allowance, float phi_width) // default: float neighbor_phi_width = .02; float eta_allowance = 1.1
            silicon_Seeding->SetAlgoUseBestTriplet(true); // default: true
            // silicon_Seeding->SetRequireINTTConsistency(bool req); // default: true
            silicon_Seeding->SetMinMVTXClusters(2);
            silicon_Seeding->SetMinINTTClusters(1);
            silicon_Seeding->SetMinClustersPerTrack(3); //
            se->registerSubsystem(silicon_Seeding);

            auto merger = new PHSiliconSeedMerger;
            merger->Verbosity(0);
            // merger->trackMapName("SiliconTrackSeedContainer_CA");
            se->registerSubsystem(merger);
        }

        auto converter = new TrackSeedTrackMapConverter;
        // Default set to full SvtxTrackSeeds. Can be set to SiliconTrackSeedContainer or TpcTrackSeedContainer
        converter->setTrackSeedName("SiliconTrackSeedContainer");
        converter->setFieldMap(G4MAGNET::magfield_tracking);
        converter->Verbosity(0);
        se->registerSubsystem(converter);

        // PHSimpleVertexFinder to find primary vertex using silicon-seeded tracks
        auto finder = new PHSimpleVertexFinder;
        finder->Verbosity(0);
        // finder->setTrackMapName("SvtxTrackMap_Acts");
        applyRecoScanConfig(finder, gRecoScanConfig);
        se->registerSubsystem(finder);
    }

    // if (Enable::GLOBAL_RECO) // This does GlobalVertexReco (G4_Global.C)
    bool doGlobalReco = true;
    if (doGlobalReco) // run global reco regardless of the flag setting since we want to compare the vertexing performance with and without silicon seeding, and global reco is needed for that comparison
    {
        Global_Reco();

        /*
        auto get_species_enum = [](const string &species_str) -> MinimumBiasInfo::SPECIES
        {
            if (species_str == "pp")
                return MinimumBiasInfo::SPECIES::PP;
            else if (species_str == "OO")
                return MinimumBiasInfo::SPECIES::OO;
            else if (species_str == "AuAu")
                return MinimumBiasInfo::SPECIES::AUAU;
            else
            {
                std::cerr << "Unknown species string: " << species_str << ", defaulting to PP" << std::endl;
                return MinimumBiasInfo::SPECIES::PP;
            }
        };

        MinimumBiasClassifier *mb = new MinimumBiasClassifier();
        mb->Verbosity(0);
        mb->setSpecies(get_species_enum(species));
        mb->setIsSim(true);
        mb->setOverwriteScale("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/scales/cdb_centrality_scale_1.root");
        mb->setOverwriteVtx("/sphenix/user/dlis/Projects/centrality/cdb/calibrations/vertexscales/cdb_centrality_vertex_scale_1.root");
        se->registerSubsystem(mb);
        */
    }

    // truth vertexing
    {
        auto vtxing = new PHTruthVertexing;
        vtxing->associate_tracks(false);
        se->registerSubsystem(vtxing);
    }

    // Truth evaluator for reco-truth matching
    {
        auto truthRecoEval = new SvtxTruthRecoTableEval;
        truthRecoEval->Verbosity(0);
        se->registerSubsystem(truthRecoEval);
    }

    // ntuplizer
    VertexCompareVerbosity::fillSilconSeed = 0;
    VertexCompareVerbosity::fillCluster = 0;
    VertexCompareVerbosity::fillTruthParticle = 0;
    bool doTruthMatching = true;
    auto compare = new VertexCompare();
    compare->IsSimulation();
    compare->setOutputName(compareOutputFile.c_str());
    if (doTruthMatching) compare->doTruthMatching();
    se->registerSubsystem(compare);

    //--------------
    // Set up Input Managers
    //--------------

    InputManagers();

    // if we use a negative number of events we go back to the command line here
    if (nEvents < 0)
    {
        return 0;
    }

    // se->skip(skip);
    se->run(nEvents);
    //  se->PrintTimer();

    //-----
    // Exit
    //-----

    CDBInterface::instance()->Print(); // print used DB files
    se->End();
    std::cout << "All done" << std::endl;
    delete se;

    gSystem->Exit(0);
    return 0;
}
#endif