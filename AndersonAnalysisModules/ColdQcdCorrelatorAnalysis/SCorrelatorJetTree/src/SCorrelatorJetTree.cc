// ----------------------------------------------------------------------------
// 'SCorrelatorJetTree.cc'
// Derek Anderson
// 12.04.2022
//
// A module to produce a tree of jets for the sPHENIX
// Cold QCD Energy-Energy Correlator analysis.
//
// Derived from code by Antonio Silva (thanks!!)
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



namespace SColdQcdCorrelatorAnalysis {

  // ctor/dtor ----------------------------------------------------------------

  SCorrelatorJetTree::SCorrelatorJetTree(const string& name, const string& outFile, const bool isMC, const bool isEmbed, const bool debug) : SubsysReco(name) {

    // print debug statement
    m_isMC    = isMC;
    m_isEmbed = isEmbed;
    m_doDebug = debug;
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::SCorrelatorJetTree(string, string, bool, bool, bool) Calling ctor" << endl;
    }
    m_outFileName = outFile;
    InitVariables();

  }  // end ctor(string&, string&, bool, bool, bool)



  SCorrelatorJetTree::~SCorrelatorJetTree() {

    // print debug statement
    if (m_doDebug) {
      cout << "SCorrelatorJetTree::~SCorrelatorJetTree() Calling dtor" << endl;
    }

    // clean up dangling pointers
    if (m_histMan) {
      delete m_histMan;
      m_histMan = NULL;
    }
    if (m_evalStack) {
      delete m_evalStack;
      m_evalStack = NULL;
      m_trackEval = NULL;
    }
    if (m_trueJetDef) {
      delete m_trueJetDef;
      m_trueJetDef = NULL;
    }
    if (m_recoJetDef) {
      delete m_recoJetDef;
      m_recoJetDef = NULL;
    }
    if (m_trueClust) {
      delete m_trueClust;
      m_trueClust = NULL;
    }
    if (m_recoClust) {
      delete m_recoClust;
      m_recoClust = NULL;
    }

  }  // end dtor



  // F4A methods --------------------------------------------------------------

  int SCorrelatorJetTree::Init(PHCompositeNode* topNode) {

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

    // initialize QA histograms/tuples, output trees, and functions
    InitHists();
    InitTuples();
    InitTrees();
    InitFuncs();
    return Fun4AllReturnCodes::EVENT_OK;

  }  // end 'Init(PHcompositeNode*)'



  int SCorrelatorJetTree::process_event(PHCompositeNode* topNode) {

    // print debug statement
    if (m_doDebug || (Verbosity() > 1)) {
      cout << "SCorrelatorJetTree::process_event(PHCompositeNode*) Processing Event..." << endl;
    }

    // reset event-wise variables & members
    ResetVariables();

    // initialize evaluator & determine subevts to grab for event
    if (m_isMC) {
      InitEvals(topNode);
      DetermineEvtsToGrab(topNode);
    }

    // get event-wise variables
    GetEventVariables(topNode);
    if (m_isMC) {
      GetPartonInfo(topNode);
    }

    // check if reconstructed vertex is in in acceptance
    bool isGoodEvt = true;
    if (m_doVtxCut) {
      isGoodEvt = IsGoodEvent(m_recoVtx);
    }

    // set event status
    int eventStatus = Fun4AllReturnCodes::EVENT_OK;
    if (m_doVtxCut && !isGoodEvt) {
      eventStatus = Fun4AllReturnCodes::DISCARDEVENT;
    } else {
      eventStatus = Fun4AllReturnCodes::EVENT_OK;
    }

    // if event is good, continue processing
    if (isGoodEvt) {

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
    }
    return eventStatus;

  }  // end 'process_event(PHCompositeNode*)'



  int SCorrelatorJetTree::End(PHCompositeNode* topNode) {

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

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
