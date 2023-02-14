// ----------------------------------------------------------------------------
// 'SEnergyCorrelator.ana.h'
// Derek Anderson
// 02.14.2023
//
// A module to implement Peter Komiske's
// EEC library in the sPHENIX software
// stack.
// ----------------------------------------------------------------------------

#pragma once

using namespace std;
using namespace fastjet;



void SEnergyCorrelator::ExtractHistsFromCorr() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(25);

  /* TODO extracting goes here */

  if (m_inStandaloneMode) PrintMessage(14);
  return;

}  // end 'ExtractHistsFromCorr()'



bool SEnergyCorrelator::ApplyJetCuts(const double ptJet, const double etaJet) {

  // print debug statement
  if (m_inDebugMode && (m_verbosity > 7)) PrintDebug(26);

  const bool isInPtRange  = ((ptJet >= m_ptJetRange[0])  && (ptJet < m_ptJetRange[1]));
  const bool isInEtaRange = ((etaJet > m_etaJetRange[0]) && (etaJet < m_etaJetRange[1]));
  const bool isGoodJet    = (isInPtRange && isInEtaRange);
  return isGoodJet;

}  // end 'ApplyJetCuts(double, double)'



bool SEnergyCorrelator::ApplyCstCuts(const double momCst, const double drCst) {

  // print debug statement
  if (m_inDebugMode && (m_verbosity > 7)) PrintDebug(27);

  const bool isInMomRange = ((momCst >= m_momCstRange[0]) && (momCst < m_momCstRange[1]));
  const bool isInDrRange  = ((drCst >= m_drCstRange[0])   && (drCst < m_drCstRange[1]));
  const bool isGoodCst    = (isInMomRange && isInDrRange);
  return isGoodCst;

}  // end 'ApplyCstCuts(double, double)'



uint32_t SEnergyCorrelator::GetJetPtBin(const double ptJet) {

  // print debug statement
  if (m_inDebugMode && (m_verbosity > 7)) PrintDebug(28);

  bool     isInPtBin(false);
  uint32_t iJetPtBin(0);
  for (size_t iJetBin = 0; iJetBin < m_nBinsJetPt; iJetBin++) {
    isInPtBin = ((ptJet >= m_ptJetBins[iJetBin].first) && (ptJet < m_ptJetBins[iJetBin].second));
    if (isInPtBin) {
      iJetPtBin = iJetBin;
      break; 
    }
  }
  return iJetPtBin;

}  // end 'GetJetPtBin(double)'

// end ------------------------------------------------------------------------
