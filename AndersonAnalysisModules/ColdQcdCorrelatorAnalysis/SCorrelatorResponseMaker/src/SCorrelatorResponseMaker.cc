/// ---------------------------------------------------------------------------
/*! \file   SCorrelatorResponseMaker.cc
 *  \author Derek Anderson
 *  \date   04.30.2023
 *
 * A module to match truth to reconstructed
 * jets/particles to derive corrections for
 * an n-point energy correlation strength
 * function.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORRESPONSEMAKER_CC

// user includes
#include "SCorrelatorResponseMaker.h"
#include "SCorrelatorResponseMaker.io.h"
#include "SCorrelatorResponseMaker.sys.h"
#include "SCorrelatorResponseMaker.ana.h"

using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // ctor/dtor ================================================================

  // --------------------------------------------------------------------------
  //! Module ctor accepting name and mode on/off flags
  // --------------------------------------------------------------------------
  SCorrelatorResponseMaker::SCorrelatorResponseMaker(
    const string &name,
    const bool debug,
    const bool standalone
  ) : SubsysReco(name) {

    // print relevant messages
    if (debug && (Verbosity() > 1)) {
      PrintDebug(0);
    }
    if (standalone) {
      PrintMessage(0);
    }

  }  // end ctor(string, bool, bool)



  // --------------------------------------------------------------------------
  //! Module ctor accepting config struct
  // --------------------------------------------------------------------------
  SCorrelatorResponseMaker::SCorrelatorResponseMaker(
    SCorrelatorResponseMakerConfig& config
  ) : SubsysReco(config.moduleName) {

    m_config = config;
    if (m_config.inDebugMode && (m_config.verbsoity > 1)) {
      PrintDebug(0);
    }

    // announce start of module
    if (m_config.isStandalone) PrintMessage(0);

  }  // end ctor(SCorrelatorResponseMakerConfig&)'



  // --------------------------------------------------------------------------
  //! Module dtor
  // --------------------------------------------------------------------------
  SCorrelatorResponseMaker::~SCorrelatorResponseMaker() {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 1)) {
      PrintDebug(1);
    }

    // delete pointers to files
    if (!m_inTrueTree || !m_inRecoTree) {
      if (!m_inTrueTree) {
        delete m_inTrueFile;
        m_inTrueFile = NULL;
      }
      if (!m_inRecoTree) {
        delete m_inRecoFile;
        m_inRecoFile = NULL;
      }
      delete m_outFile;
      m_outFile = NULL;
    }

  }  // end dtor



  // F4A methods ==============================================================

  // --------------------------------------------------------------------------
  //! F4A module initialization
  // --------------------------------------------------------------------------
  int SCorrelatorResponseMaker::Init(PHCompositeNode*) {

    /* TODO init will go here */
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'Init(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Process an event inside F4A
  // --------------------------------------------------------------------------
  int SCorrelatorResponseMaker::process_event(PHCompositeNode*) {

    /* TODO event processing will go here */
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'process_event(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! F4A module wind-down
  // --------------------------------------------------------------------------
  int SCorrelatorResponseMaker::End(PHCompositeNode*) {

    /* TODO end will go here */
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'End(PHCompositeNode*)'



  // standalone-only methods ==================================================

  // --------------------------------------------------------------------------
  //! Standalone module initialization
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::Init() {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 0)) {
      PrintDebug(2);
    }

    // make sure standalone mode is on & open files
    if (!m_config.isStandalone) {
      PrintError(0);
      assert(m_config.isStandalone);
    } else {
      OpenInputFiles();
    }
    OpenOutputFile();

    // announce files
    PrintMessage(1);

    // initialize input and output
    InitializeTrees();
    return;

  }  // end 'StandaloneInit()'



  // --------------------------------------------------------------------------
  //! Run analysis on input trees
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::Analyze() {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity(0)) {
      PrintDebug(3);
    }

    // make sure standalone mode is on
    if (!m_config.isStandalone) {
      PrintError(0);
      assert(m_config.isStandalone);
    }

    // announce start of analysis
    PrintMessage(2);

    DoMatching();
    return;

  }  // end 'StandaloneAnalyze()'



  // --------------------------------------------------------------------------
  //! Standalone module wind-down
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::End() {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity(0)) {
      PrintDebug(4);
    }

    // make sure standalone mode is on & save output
    if (!m_config.isStandalone) {
      PrintError(0);
      assert(m_config.isStandalone);
    } else {
      SaveOutput();
    }

    // announce end
    PrintMessage(3);
    return;

  }  // end 'StandaloneEnd()'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
