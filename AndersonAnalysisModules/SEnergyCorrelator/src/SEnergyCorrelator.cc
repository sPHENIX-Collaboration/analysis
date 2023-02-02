// 'SEnergyCorrelator.cc'
// Derek Anderson
// 01.20.2023
//
// A module to implement Peter Komiske's
// EEC library in the sPHENIX software
// stack.

#define SENERGYCORRELATOR_CC

// user includes
#include "SEnergyCorrelator.h"
#include "SEnergyCorrelator.io.h"
#include "SEnergyCorrelator.sys.h"

using namespace std;
using namespace findNode;



// ctor/dtor ------------------------------------------------------------------

SEnergyCorrelator::SEnergyCorrelator(const string &name, const bool isComplex, const bool doDebug) : SubsysReco(name) {

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

  // print debug statement
  m_inDebugMode = doDebug;
  if (m_inDebugMode) {
    PrintMessage(1);
  }

}  // end ctor(string, bool, bool)



SEnergyCorrelator::~SEnergyCorrelator() {

  // print debug statement
  if (m_inDebugMode) {
    PrintMessage(14);
  }
  delete m_inFile;
  delete m_outFile;

}  // end dtor



// F4A methods ----------------------------------------------------------------

int SEnergyCorrelator::Init(PHCompositeNode *topNode) {

  // print debug statement
  if (m_inDebugMode) {
    PrintMessage(2);
  }

  // make sure complex mode is on & open input
  if (m_inStandaloneMode) {
    PrintError(0);
    assert(m_inComplexMode);
  } else {
    GrabInputNode();
  }

  // initialize input, output, & correlators
  InitializeTree();
  InitializeHists();
  InitializeCorrs();
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'Init(PHCompositeNode*)'



int SEnergyCorrelator::process_event(PHCompositeNode *topNode) {

  // print debug statement
  if (m_inDebugMode) {
    PrintMessage(7);
  }

  // make sure complex mode is on
  if (m_inStandaloneMode) {
    PrintError(3);
    assert(m_inComplexMode);
  }

  /* TODO analysis goes here */
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'process_event(PHCompositeNode*)'



int SEnergyCorrelator::End(PHCompositeNode *topNode) {

  // print debug statement
  if (m_inDebugMode) {
    PrintMessage(8);
  }

  // make sure complex mode is on & save output
  if (m_inStandaloneMode) {
    PrintError(4);
    assert(m_inComplexMode);
  } else {
    SaveOutput();
  }
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'End(PHCompositeNode*)'



// standalone-only methods ----------------------------------------------------

void SEnergyCorrelator::Init() {

  // print debug statement
  if (m_inDebugMode) {
    PrintMessage(10);
  }

  // make sure standalone mode is on & open input
  if (m_inComplexMode) {
    PrintError(5);
    assert(m_inStandaloneMode);
  } else {
    OpenInputFile();
  }

  // initialize input, output, & correlators
  InitializeTree();
  InitializeHists();
  InitializeCorrs();
  return;

}  // end 'Init()'



void SEnergyCorrelator::Analyze() {

  // print debug statement
  if (m_inDebugMode) {
    PrintMessage(12);
  }

  // make sure standalone mode is on
  if (m_inComplexMode) {
    PrintError(8);
    assert(m_inStandaloneMode);
  }

  /* TODO analysis goes here */
  return;

}  // end 'Analyze()'



void SEnergyCorrelator::End() {

  // print debug statement
  if (m_inDebugMode) {
    PrintMessage(13);
  }

  // make sure standalone mode is on & save output
  if (m_inComplexMode) {
    PrintError(9);
    assert(m_inStandaloneMode);
  } else {
    SaveOutput();
  }
  return;

}  // end 'End()'

// end ------------------------------------------------------------------------
