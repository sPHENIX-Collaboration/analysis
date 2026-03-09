/// ---------------------------------------------------------------------------
/*! \file   SCorrelatorResponseMaker.cc
 *  \author Derek Anderson
 *  \date   04.30.2023
 *
 *  A module to match truth to reconstructed
 *  jets/particles to derive corrections for
 *  an n-point energy correlation strength
 *  function.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORRESPONSEMAKER_CC

// user includes
#include "SCorrelatorResponseMaker.h"
#include "SCorrelatorResponseMaker.sys.h"
#include "SCorrelatorResponseMaker.ana.h"

using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // ctor/dtor ================================================================

  // --------------------------------------------------------------------------
  //! Module ctor accepting config struct
  // --------------------------------------------------------------------------
  SCorrelatorResponseMaker::SCorrelatorResponseMaker(SCorrelatorResponseMakerConfig& config) :
    SubsysReco(config.moduleName)
  {

    m_config = config;
    if (m_config.inDebugMode && (m_config.verbosity > 1)) {
      PrintDebug(0);
    }

    // announce start of module
    PrintMessage(0);

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



  // public methods ===========================================================

  // --------------------------------------------------------------------------
  //! Module initialization
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::Init() {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 0)) {
      PrintDebug(2);
    }

    // open files
    OpenInputFiles();
    OpenOutputFile();

    // announce files
    PrintMessage(1);

    // initialize input and output
    InitializeTrees();
    return;

  }  // end 'Init()'



  // --------------------------------------------------------------------------
  //! Run analysis on input trees
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::Analyze() {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 0)) {
      PrintDebug(3);
    }

    // announce start of analysis
    PrintMessage(2);

    // do matching
    DoMatching();
    return;

  }  // end 'Analyze()'



  // --------------------------------------------------------------------------
  //! Module wind-down
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::End() {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 0)) {
      PrintDebug(4);
    }

    // save output
    SaveOutput();

    // announce end
    PrintMessage(3);
    return;

  }  // end 'End()'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
