// ----------------------------------------------------------------------------
// 'SCorrelatorJetTree.evt.h'
// Derek Anderson
// 03.28.2023
//
// Class to construct a tree of
// jets from a specified set of
// events.
//
// Derived from code by Antonio
// Silva (thanks!!)
// ----------------------------------------------------------------------------

#pragma once

using namespace std;
using namespace findNode;



// event methods --------------------------------------------------------------

void SCorrelatorJetTree::FindPartons(PHCompositeNode *topNode) {

  /* TODO will find partons here */
  return;

}  // end 'FindPartons(PHCompositeNode*)'



void SCorrelatorJetTree::GetEventVariables(PHCompositeNode *topNode) {

  m_numTrks    = GetNumTrks(topNode);
  m_sumECalEne = GetSumECalEne(topNode);
  m_sumHCalEne = GetSumHCalEne(topNode);
  if (m_isMC) {
    m_numChrgPars = GetNumChrgPars(topNode);
    m_sumParEne   = GetSumParEne(topNode);
  }
  return;

}  // end 'GetEventVariables(PHCompositeNode*)'



long SCorrelatorJetTree::GetNumTrks(PHCompositeNode *topNode) {

  /* TODO grab no. of tracks here */
  return 78;

}  // end 'GetNumTrks(PHCompositeNode*)'



long SCorrelatorJetTree::GetNumChrgPars(PHCompositeNode *topNode) {

  /* TODO grab no. of charged particles here */
  return 78;

}  // end 'GetNumChrgPars(PHCompositeNode*)'



double SCorrelatorJetTree::GetSumECalEne(PHCompositeNode *topNode) {

  /* TODO grab ecal sum here */
  return 36.;

}  // end 'GetSumECalEne(PHCompositeNode*)'



double SCorrelatorJetTree::GetSumHCalEne(PHCompositeNode *topNode) {

  /* TODO grab hcal sum here */
  return 36.;

}  // end 'GetSumHCalEne(PHCompositeNode*)'



double SCorrelatorJetTree::GetSumParEne(PHCompositeNode *topNode) {

  /* TODO grab particle sum here */
  return 36.;

}  // end 'GetSumParEne(PHCompositeNode*)'

// end ------------------------------------------------------------------------
