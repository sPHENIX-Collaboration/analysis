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

// user includes
#include "SEnergyCorrelator.h"
#include "SEnergyCorrelator.io.h"
#include "SEnergyCorrelator.sys.h"
#include "SEnergyCorrelator.ana.h"

using namespace std;
using namespace fastjet;



namespace SColdQcdCorrelatorAnalysis {

  // ctor/dtor ----------------------------------------------------------------

  SEnergyCorrelator::SEnergyCorrelator(const string &name, const bool isComplex, const bool doDebug, const bool inBatch) : SubsysReco(name) {

    // initialize internal variables
    InitializeMembers();

    // set standalone/complex mode
    if (isComplex) {
      m_inComplexMode    = true;
      m_inStandaloneMode = false; 
    } else {
      m_inComplexMode    = false;
      m_inStandaloneMode = true;
    }

    // set verbosity in complex mode
    if (m_inComplexMode) {
      m_verbosity = Verbosity();
    }

    // set debug/batch mode & print debug statement
    m_inDebugMode = doDebug;
    m_inBatchMode = inBatch;
    if (m_inDebugMode) PrintDebug(1);

    // set module name & announce start of calculation
    m_moduleName = name;
    if (m_inStandaloneMode) PrintMessage(0);

  }  // end ctor(string, bool, bool)



  SEnergyCorrelator::~SEnergyCorrelator() {

    // print debug statement
    if (m_inDebugMode) PrintDebug(14);

    // delete pointers to files
    if (!m_inChain) {
      delete m_outFile;
    }
    m_inFileNames.clear();

    // delete pointers to correlators/histograms
    for (size_t iPtBin = 0; iPtBin < m_nBinsJetPt; iPtBin++) {
      delete m_eecLongSide.at(iPtBin);
      delete m_outHistVarDrAxis.at(iPtBin);
      delete m_outHistErrDrAxis.at(iPtBin);
      delete m_outHistVarLnDrAxis.at(iPtBin);
      delete m_outHistErrLnDrAxis.at(iPtBin);
    }
    m_ptJetBins.clear();
    m_eecLongSide.clear();
    m_outHistVarDrAxis.clear();
    m_outHistErrDrAxis.clear();
    m_outHistVarLnDrAxis.clear();
    m_outHistErrLnDrAxis.clear();

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
    if (m_inDebugMode) PrintDebug(10);

    // make sure standalone mode is on & open files
    if (m_inComplexMode) {
      PrintError(5);
      assert(m_inStandaloneMode);
    } else {
      OpenInputFiles();
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
    if (m_inDebugMode) PrintDebug(12);

    // make sure standalone mode is on
    if (m_inComplexMode) {
      PrintError(8);
      assert(m_inStandaloneMode);
    }

    // loop over events and calculate correlators
    DoCorrelatorCalculation();

    // translate correlators into root hists
    ExtractHistsFromCorr();
    PrintMessage(9);
    return;

  }  // end 'StandaloneAnalyze()'



  void SEnergyCorrelator::End() {

    // print debug statement
    if (m_inDebugMode) PrintDebug(13);

    // make sure standalone mode is on & save output
    if (m_inComplexMode) {
      PrintError(9);
      assert(m_inStandaloneMode);
    } else {
      SaveOutput();
    }

    // close files and announce end
    if (m_inComplexMode) {
      PrintError(15);
      assert(m_inStandaloneMode);
    }
    CloseOutputFile();
    PrintMessage(11);
    return;

  }  // end 'StandaloneEnd()'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
