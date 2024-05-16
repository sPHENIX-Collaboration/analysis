// ----------------------------------------------------------------------------
// 'SEnergyCorrelator.cc'
// Derek Anderson
// 01.20.2023
//
// A module to implement Peter Komiske's EEC library
// in the sPHENIX software stack for the Cold QCD
// Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#define SENERGYCORRELATOR_CC

// analysis definition
#include "SEnergyCorrelator.h"
#include "SEnergyCorrelator.io.h"
#include "SEnergyCorrelator.sys.h"
#include "SEnergyCorrelator.ana.h"
#include "SEnergyCorrelatorConfig.h"

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {

  // ctor/dtor ----------------------------------------------------------------

  SEnergyCorrelator::SEnergyCorrelator(SEnergyCorrelatorConfig& config) : SubsysReco(config.moduleName) {

    // set config & initialize internal variables
    m_config = config;
    InitializeMembers();

    // announce start of calculation
    if (m_config.isStandalone) PrintMessage(0);

  }  // end ctor(SEnergyCorrelatorConfig&)



  SEnergyCorrelator::~SEnergyCorrelator() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(14);

    // delete pointers to files
    if (!m_inChain) {
      delete m_outFile;
    }

    // delete pointers to correlators/histograms
    for (size_t iPtBin = 0; iPtBin < m_config.ptJetBins.size(); iPtBin++) {
      delete m_eecLongSide.at(iPtBin);
      delete m_outPackageHistVarDrAxis.at(iPtBin);
      delete m_outPackageHistErrDrAxis.at(iPtBin);
      delete m_outPackageHistVarLnDrAxis.at(iPtBin);
      delete m_outPackageHistErrLnDrAxis.at(iPtBin);
      delete m_outManualHistErrDrAxis.at(iPtBin);
    }
    m_eecLongSide.clear();
    m_outPackageHistVarDrAxis.clear();
    m_outPackageHistErrDrAxis.clear();
    m_outPackageHistVarLnDrAxis.clear();
    m_outPackageHistErrLnDrAxis.clear();
    m_outManualHistErrDrAxis.clear();

  }  // end dtor



  // F4A methods --------------------------------------------------------------

  int SEnergyCorrelator::Init(PHCompositeNode*) {

    /* TODO init will go here */
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'Init(PHCompositeNode*)'




  int SEnergyCorrelator::process_event(PHCompositeNode*) {

    /* TODO event processing will go here */
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'process_event(PHCompositeNode*)'



  int SEnergyCorrelator::End(PHCompositeNode*) {

    /* TODO end will go here */
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'End(PHCompositeNode*)'



  // standalone-only methods --------------------------------------------------

  void SEnergyCorrelator::Init() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(10);

    // make sure standalone mode is on & open files
    if (m_config.isStandalone) {
      OpenInputFiles();
    } else {
      PrintError(5);
      assert(m_config.isStandalone);
    }
    OpenOutputFile();

    // announce files
    PrintMessage(1);

    // initialize input, output, & correlators
    InitializeTree();
    InitializeCorrs();
    InitializeHists();
    return;

  }  // end 'StandaloneInit()'



  void SEnergyCorrelator::Analyze() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(12);

    // make sure standalone mode is on
    if (!m_config.isStandalone) {
      PrintError(8);
      assert(m_config.isStandalone);
    }

    // announce start of event loop
    const uint64_t nEvts = m_inChain -> GetEntriesFast();
    PrintMessage(7, nEvts);

    // event loop
    uint64_t nBytes = 0;
    for (uint64_t iEvt = 0; iEvt < nEvts; iEvt++) {

      const uint64_t entry = Interfaces::LoadTree(m_inChain, iEvt, m_fCurrent);
      if (entry < 0) break;

      const uint64_t bytes = Interfaces::GetEntry(m_inChain, iEvt);
      if (bytes < 0) {
        break;
      } else {
        nBytes += bytes;
        PrintMessage(8, nEvts, iEvt);
      }

      // if using legacy input, fill container
      if (m_config.isLegacyInput) {
        m_legacy.SetCorrelatorInput(m_input, m_config.isInTreeTruth, m_config.isEmbed);
      }
 
      // run calculations
      DoLocalCalculation();

    }  // end event loop

    // announce end of event loop
    PrintMessage(13);

    // translate correlators into root hists
    ExtractHistsFromCorr();
    PrintMessage(9);
    return;

  }  // end 'StandaloneAnalyze()'



  void SEnergyCorrelator::End() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(13);

    // make sure standalone mode is on & save output
    if (m_config.isStandalone) {
      SaveOutput();
    } else {
      PrintError(9);
      assert(m_config.isStandalone);
    }

    // close files and announce end
    if (!m_config.isStandalone) {
      PrintError(15);
      assert(m_config.isStandalone);
    }
    CloseOutputFile();
    PrintMessage(11);
    return;

  }  // end 'StandaloneEnd()'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
