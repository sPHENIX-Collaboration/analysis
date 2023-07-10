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
#include "SCorrelatorJetTree.evt.h"
#include "SCorrelatorJetTree.jet.h"
#include "SCorrelatorJetTree.cst.h"
#include "SCorrelatorJetTree.sys.h"

using namespace std;
using namespace fastjet;
using namespace findNode;



// ctor/dtor ------------------------------------------------------------------

SCorrelatorJetTree::SCorrelatorJetTree(const string &name, const string &outFile, const bool isMC, const bool debug) : SubsysReco(name) {

  // print debug statement
  m_isMC    = isMC;
  m_doDebug = debug;
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::SCorrelatorJetTree(string, string, bool, bool) Calling ctor" << endl;
  }
  m_outFileName = outFile;
  InitVariables();

}  // end ctor(string, string, bool, bool)



SCorrelatorJetTree::~SCorrelatorJetTree() {

  // print debug statement
  if (m_doDebug) {
    cout << "SCorrelatorJetTree::~SCorrelatorJetTree() Calling dtor" << endl;
  }
  delete m_histMan;
  delete m_evalStack;
  delete m_trackEval;
  delete m_outFile;
  delete m_recoTree;
  delete m_trueTree;
  delete m_matchTree;
  delete m_trueJetDef;
  delete m_recoJetDef;
  delete m_trueClust;
  delete m_recoClust;

}  // end dtor



// F4A methods ----------------------------------------------------------------

int SCorrelatorJetTree::Init(PHCompositeNode *topNode) {

  // print debug statement
  if (m_doDebug || (Verbosity() > 1)) {
    cout << "SCorrelatorJetTree::Init(PHCompositeNode*) Initializing..." << endl;
  }

  // intitialize output file
  m_outFile = new TFile(m_outFileName.c_str(), "RECREATE");
  if (!m_outFile) {
    cerr << "PANIC: couldn't open SCorrelatorJetTree output file!" << endl;
  }

  // create node for jet-tree
  if (m_saveDST) {
    CreateJetNode(topNode);
  }

  // initialize QA histograms, output trees, and evaluators (if needed)
  InitHists();
  InitTrees();
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'Init(PHcompositeNode*)'



int SCorrelatorJetTree::process_event(PHCompositeNode *topNode) {

  // print debug statement
  if (m_doDebug || (Verbosity() > 1)) {
    cout << "SCorrelatorJetTree::process_event(PHCompositeNode*) Processing Event..." << endl;
  }

  // initialize evaluator for event
  if (m_isMC) {
    InitEvals(topNode);
  }

  // reset for event and get event-wise variables
  ResetVariables();
  GetEventVariables(topNode);
  if (m_isMC) {
    GetPartonInfo(topNode);
  }

  // find jets
  FindRecoJets(topNode);
  if (m_isMC) {
    FindTrueJets(topNode);
  }

  // fill output trees
  FillRecoTree();
  if (m_isMC) {
    FillTrueTree();
  }
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'process_event(PHCompositeNode*)'



int SCorrelatorJetTree::End(PHCompositeNode *topNode) {

  // print debug statements
  if (m_doDebug || (Verbosity() > 1)) {
    cout << "SCorrelatorJetTree::End(PHCompositeNode*) This is the End..." << endl;
  }

  // save output and close
  SaveOutput();
  m_outFile -> cd();
  m_outFile -> Close();
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'End(PHcompositeNode*)'

// end ------------------------------------------------------------------------
