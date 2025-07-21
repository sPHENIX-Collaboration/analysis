// ----------------------------------------------------------------------------
/*! \file    TriggerClusterTupleMaker.cc'
 *  \authors Derek Anderson
 *  \date    06.06.2024
 *
 *  A Fun4All module to dump trigger clusters
 *  into an NTuple
 */
// ----------------------------------------------------------------------------

#define TRIGGERCLUSTERTUPLEMAKER_CC

// module definition
#include "TriggerClusterTupleMaker.h"



// ctor/dtor ==================================================================

// ----------------------------------------------------------------------------
//! Module constructor
// ----------------------------------------------------------------------------
TriggerClusterTupleMaker::TriggerClusterTupleMaker(const std::string &name) : SubsysReco(name) {

  // print debug message
  if (m_config.debug && (Verbosity() > 0)) {
    std::cout << "TriggerClusterTupleMaker::TriggerClusterTupleMaker(const std::string &name) Calling ctor" << std::endl;
  }

  // make sure vector is empty
  m_vecOutVars.clear();

}  // end ctor



// ----------------------------------------------------------------------------
//! Module destructor
// ----------------------------------------------------------------------------
TriggerClusterTupleMaker::~TriggerClusterTupleMaker() {

  // print debug message
  if (m_config.debug && (Verbosity() > 0)) {
    std::cout << "TriggerClusterTupleMaker::~TriggerClusterTupleMaker() Calling dtor" << std::endl;
  }

  /* nothing to do */

}  // end dtor



// fun4all methods ============================================================

// ----------------------------------------------------------------------------
//! Initialize module
// ----------------------------------------------------------------------------
int TriggerClusterTupleMaker::Init(PHCompositeNode* topNode) {

  if (m_config.debug) {
    std::cout << "TriggerClusterTupleMaker::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  }

  // initialize output
  InitOutput(topNode);
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'Init(PHCompositeNode*)'



// ----------------------------------------------------------------------------
//! Grab inputs and build trigger clusters
// ----------------------------------------------------------------------------
int TriggerClusterTupleMaker::process_event(PHCompositeNode* topNode) {

  if (m_config.debug) {
    std::cout << "TriggerClusterTupleMaker::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  }

  // grab input nodes
  GrabInputNodes(topNode);

  /* TODO loop over trigger clusters here */

  // end event
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'process_event(PHCompositeNode*)'



// ----------------------------------------------------------------------------
//! Run final calculations
// ----------------------------------------------------------------------------
int TriggerClusterTupleMaker::End(PHCompositeNode *topNode) {

  if (m_config.debug) {
    std::cout << "TriggerClusterTupleMaker::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  }

  // save and exit
  SaveOutput();
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'End(PHCompositeNode*)'



// private methods ============================================================

// ----------------------------------------------------------------------------
//! Create output file and ntuple
// ----------------------------------------------------------------------------
void TriggerClusterTupleMaker::InitOutput() {

  // print debug message
  if (m_config.debug && (Verbosity() > 0)) {
    std::cout << "TriggerClusterTupleMaker::InitOutput(PHCompositeNode*) Creating output" << std::endl;
  }

  // open output file
  m_outFile = new TFile(m_config.outFile.data(), "recreate");
  if (!m_outFile) {
    std::cerr << PHWHERE << ": PANIC! Couldn't open output file!" << std::endl;
    assert(m_outFile);
  }

  // make list of variables
  const std::vector<std::string> vecLeaves = {
    "ntowers",
    "energy",
    "ecore",
    "phi",
    "rx",
    "ry",
    "rz",
    "z",
    "r"
  };

  // flatten list
  std::string argLeaves("");
  for (size_t iLeaf = 0; iLeaf < vecLeaves.size(); ++iLeaf) {
    argLeaves.append(vecLeaves[iLeaf]);
    if ((iLeaf + 1) != vecLeaves.size()) {
      argLeaves.append(":");
    }
  }

  // create tuple
  m_outTuple = new TNtuple(m_config.outTuple.data(), "Trigger Clusters", argLeaves.data());
  return;

}  // end 'InitOutput()'



// ----------------------------------------------------------------------------
//! Grab input nodes
// ----------------------------------------------------------------------------
void TriggerClusterTupleMaker::GrabInputNodes(PHCompositeNode* topNode) {

  // print debug message
  if (m_config.debug && (Verbosity() > 0)) {
    std::cout << "TriggerClusterTupleMaker::GrabTowerNodes(PHCompositeNode*) Grabbing input tower nodes" << std::endl;
  }

  // get emcal tower info node
  m_inTrgClusts = findNode::getClass<RawClusterContainer>(topNode, m_config.inNode);
  if (!m_inTrgClusts) {
    std::cerr << PHWHERE << ": PANIC! Couldn't grab node '" << m_config.inNode << "'!" << std::endl;
    assert(m_inTrgClusts); 
  }
  return;

}  // end 'GrabInputNodes(PHCompositeNode*)'



// ----------------------------------------------------------------------------
//! Set variables
// ----------------------------------------------------------------------------
void TriggerClusterTupleMaker::SetClusterVariables(RawClusterv1* cluster) {

  // print debug message
  if (m_config.debug && (Verbosity() > 0)) {
    std::cout << "TriggerClusterTupleMaker::GrabTowerNodes(PHCompositeNode*) Grabbing input tower nodes" << std::endl;
  }

  // get emcal tower info node
  m_inTrgClusts = findNode::getClass<RawClusterContainer>(topNode, m_config.inNode);
  if (!m_inTrgClusts) {
    std::cerr << PHWHERE << ": PANIC! Couldn't grab node '" << m_config.inNode << "'!" << std::endl;
    assert(m_inTrgClusts); 
  }
  return;

}  // end 'GrabInputNodes(PHCompositeNode*)'






// end ------------------------------------------------------------------------
