// ----------------------------------------------------------------------------
// 'SCorrelatorJetTree.cc'
// Derek Anderson
// 12.04.2022
//
// Class to construct a tree of
// jets from a specified set of
// events.
//
// Standard F4A methods are
// collected here.
//
// Derived from code by Antonio
// Silva (thanks!!)
// ----------------------------------------------------------------------------

#define SCORRELATORJETTREE_CC

// user includes
#include "SCorrelatorJetTree.h"
#include "SCorrelatorJetTree.io.h"
#include "SCorrelatorJetTree.jets.h"
#include "SCorrelatorJetTree.system.h"
#include "SCorrelatorJetTree.constituents.h"

using namespace std;
using namespace fastjet;
using namespace findNode;



// ctor/dtor ------------------------------------------------------------------

SCorrelatorJetTree::SCorrelatorJetTree(const string &name, const string &outfile, const bool isMC, const bool debug) : SubsysReco(name) {

  // print debug statement
  m_ismc    = isMC;
  m_doDebug = debug;
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::SCorrelatorJetTree(string, string, bool, bool) Calling ctor" << endl;
  }
  m_outfilename = outfile;
  initializeVariables();

}  // end ctor(string, string, bool, bool)



SCorrelatorJetTree::~SCorrelatorJetTree() {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::~SCorrelatorJetTree() Calling dtor" << endl;
  }
  delete m_hm;
  delete m_outFile;
  delete m_recTree;
  delete m_truTree;

}  // end dtor



// F4A methods ----------------------------------------------------------------

int SCorrelatorJetTree::Init(PHCompositeNode *topNode) {

  // print debug statement
  if (m_doDebug || (Verbosity() > 1)) {
    cout << "SCorrelatorJetTree::Init(PHCompositeNode*) Initializing..." << endl;
  }

  // intitialize output file
  m_outFile = new TFile(m_outfilename.c_str(), "RECREATE");
  if (!m_outFile) {
    cerr << "PANIC: couldn't open SCorrelatorJetTree output file!" << endl;
  }

  // create node for jet-tree
  if (m_save_dst) {
    createJetNode(topNode);
  }

  // initialize QA histograms and output trees
  initializeHists();
  initializeTrees();
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'Init(PHcompositeNode*)'



int SCorrelatorJetTree::process_event(PHCompositeNode *topNode) {

  // print debug statement
  if (m_doDebug || (Verbosity() > 1)) {
    cout << "SCorrelatorJetTree::process_event(PHCompositeNode*) Processing Event..." << endl;
  }

  // find jets
  if (m_ismc) {
    findMcJets(topNode);
  }
  findJets(topNode);
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'process_event(PHCompositeNode*)'



int SCorrelatorJetTree::End(PHCompositeNode *topNode) {

  // print debug statements
  if (m_doDebug || (Verbosity() > 1)) {
    cout << "SCorrelatorJetTree::End(PHCompositeNode*) This is the End..." << endl;
  }

  // save output and close
  saveOutput();
  m_outFile -> cd();
  m_outFile -> Close();
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'End(PHcompositeNode*)'

// end ------------------------------------------------------------------------
