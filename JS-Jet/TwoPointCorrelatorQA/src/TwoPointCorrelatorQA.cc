// ----------------------------------------------------------------------------
// 'TwoPointCorrelatorQA.cc'
// Derek Anderson
// 03.25.2024
//
// A Fun4All QA module to produce an EEC spectra
// for jets constructed from an arbitrary source.
// ----------------------------------------------------------------------------

#define TWOPOINTCORRELATORQA_CC

// f4a libraries
#include <fun4all/Fun4AllReturnCodes.h>
// phool libraries
#include <phool/PHCompositeNode.h>

// module definition
#include "TwoPointCorrelatorQA.h"



// ctor/dtor ------------------------------------------------------------------

TwoPointCorrelatorQA::TwoPointCorrelatorQA(const std::string &name) : SubsysReco(name) {

  // print debug message
  if (m_config.doDebug && (Verbosity() > 0)) {
    std::cout << "TwoPointCorrelatorQA::TwoPointCorrelatorQA(const std::string &name) Calling ctor" << std::endl;
  }

  /* nothing to do */

}  // end ctor



TwoPointCorrelatorQA::~TwoPointCorrelatorQA() {

  // print debug message
  if (m_config.doDebug && (Verbosity() > 0)) {
    std::cout << "TwoPointCorrelatorQA::~TwoPointCorrelatorQA() Calling dtor" << std::endl;
  }

  /* nothing to do */

}  // end dtor



// fun4all methods ------------------------------------------------------------

int TwoPointCorrelatorQA::Init(PHCompositeNode* topNode) {

  if (m_config.doDebug) {
    std::cout << "TwoPointCorrelatorQA::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  }

  /* TODO fill in */

  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'Init(PHCompositeNode*)'



int TwoPointCorrelatorQA::process_event(PHCompositeNode* topNode) {

  if (m_config.doDebug) {
    std::cout << "TwoPointCorrelatorQA::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  }

  /* TODO fill in */

  return Fun4AllReturnCodes::EVENT_OK;


}  // end 'process_event(PHCompositeNode*)'



int TwoPointCorrelatorQA::End(PHCompositeNode *topNode) {

  if (m_config.doDebug) {
    std::cout << "TwoPointCorrelatorQA::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  }

  /* TODO fill in */

  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'End(PHCompositeNode*)'



// private methods ------------------------------------------------------------

void TwoPointCorrelatorQA::BuildHistograms() {

  // print debug message
  if (m_config.doDebug && (Verbosity() > 0)) {
    std::cout << "TwoPointCorrelatorQA::BuildHistograms() Building qa histograms" << std::endl;
  }

  /* TODO fill in */
  return;

}  // end 'BuildHistograms()'



void TwoPointCorrelatorQA::GrabNodes(PHCompositeNode* topNode) {

  // print debug message
  if (m_config.doDebug && (Verbosity() > 0)) {
    std::cout << "TwoPointCorrelatorQA::GrabNodes(PHCompositeNode*) Grabbing input nodes" << std::endl;
  }

  /* TODO fill in */
  return;

}  // end 'GrabNodes(PHCompositeNode*)'

// end ------------------------------------------------------------------------
