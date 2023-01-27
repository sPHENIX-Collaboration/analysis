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
#include "SEnergyCorrelator.system.h"

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
    cout << "SEnergyCorrelator::SEnergyCorrelator(string, bool, bool) calling ctor" << endl;
  }

}  // end ctor(string, bool, bool)



SEnergyCorrelator::~SEnergyCorrelator() {

  // print debug statement
  if (m_inDebugMode) {
    cout << "SEnergyCorrelator::~SEnergyCorrelator() calling dtor" << endl;
  }
  delete m_inFile;
  delete m_outFile;

}  // end dtor



// F4A methods ----------------------------------------------------------------

int SEnergyCorrelator::Init(PHCompositeNode *topNode) {

  // print debug statement
  if (m_inDebugMode) {
    cout << "SEnergyCorrelator::Init(PHCompositeNode*) initializing" << endl;
  }

  InitializeHists();
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'Init(PHCompositeNode*)'



int SEnergyCorrelator::process_event(PHCompositeNode *topNode) {

  // print debug statement
  if (m_inDebugMode) {
    cout << "SEnergyCorrelator::process_event(PHCompositeNode*) processing event" << endl;
  }
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'process_event(PHCompositeNode*)'



int SEnergyCorrelator::End(PHCompositeNode *topNode) {

  // print debug statement
  if (m_inDebugMode) {
    cout << "SEnergyCorrelator::End(PHCompositeNode*) this is the end..." << endl;
  }
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'End(PHCompositeNode*)'



// standalone-only methods ----------------------------------------------------

int SEnergyCorrelator::Analyze() {

  // print debug statement
  if (m_inDebugMode) {
    cout << "SEnergyCorrelator::Analyze() analyzing input" << endl;
  }

  // make sure in standalone mode
  if (m_inComplexMode) {
    cerr << "SEnergyCorrelator::Analyze() PANIC: calling standalone method in complex mode!\n"
         << "                             Ending program execution!"
         << endl;
    assert(m_inStandaloneMode);
  }
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'Analyze()'

// end ------------------------------------------------------------------------
