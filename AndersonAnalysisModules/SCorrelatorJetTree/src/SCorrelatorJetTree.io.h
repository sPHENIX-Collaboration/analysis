// ----------------------------------------------------------------------------
// 'SCorrelatorJetTree.io.h'
// Derek Anderson
// 12.04.2022
//
// Class to construct a tree of
// jets from a specified set of
// events.
//
// Methods relevant to i/o
// operations are collected
// here.
//
// Derived from code by Antonio
// Silva (thanks!!)
// ----------------------------------------------------------------------------

#pragma once

using namespace std;
using namespace findNode;



// i/o methods ---------------------------------------------------------------- 

void SCorrelatorJetTree::SetParPtRange(const pair<double, double> ptRange) {

  m_parPtRange[0] = ptRange.first;
  m_parPtRange[1] = ptRange.second;
  return;

}  // end 'SetParEtaRange(pair<double, double>)'



void SCorrelatorJetTree::SetParEtaRange(const pair<double, double> etaRange) {

  m_parEtaRange[0] = etaRange.first;
  m_parEtaRange[1] = etaRange.second;
  return;

}  // end 'SetParEtaRange(pair<double, double>)'



void SCorrelatorJetTree::SetTrackPtRange(const pair<double, double> ptRange) {

  m_trkPtRange[0] = ptRange.first;
  m_trkPtRange[1] = ptRange.second;
  return;

}  // end 'SetTrackPtRange(pair<double, double>)'



void SCorrelatorJetTree::SetTrackEtaRange(const pair<double, double> etaRange) {

  m_trkEtaRange[0] = etaRange.first;
  m_trkEtaRange[1] = etaRange.second;
  return;

}  // end 'SetTrackEtaRange(pair<double, double>)'



void SCorrelatorJetTree::SetFlowPtRange(const pair<double, double> ptRange) {

  m_flowPtRange[0] = ptRange.first;
  m_flowPtRange[1] = ptRange.second;
  return;

}  // end 'SetFlowPtRange(pair<double, double>)'



void SCorrelatorJetTree::SetFlowEtaRange(const pair<double, double> etaRange) {

  m_flowEtaRange[0] = etaRange.first;
  m_flowEtaRange[1] = etaRange.second;
  return;

}  // end 'SetFlowEtaRange(pair<double, double>)'



void SCorrelatorJetTree::SetECalPtRange(const pair<double, double> ptRange) {

  m_ecalPtRange[0] = ptRange.first;
  m_ecalPtRange[1] = ptRange.second;
  return;

}  // end 'SetECalPtRange(pair<double, double>)'



void SCorrelatorJetTree::SetECalEtaRange(const pair<double, double> etaRange) {

  m_ecalEtaRange[0] = etaRange.first;
  m_ecalEtaRange[1] = etaRange.second;
  return;

}  // end 'SetECalEtaRange(pair<double, double>)'



void SCorrelatorJetTree::SetHCalPtRange(const pair<double, double> ptRange) {

  m_hcalPtRange[0] = ptRange.first;
  m_hcalPtRange[1] = ptRange.second;
  return;

}  // end 'SetHCalPtRange(pair<double, double>)'



void SCorrelatorJetTree::SetHCalEtaRange(const pair<double, double> etaRange) {

  m_hcalEtaRange[0] = etaRange.first;
  m_hcalEtaRange[1] = etaRange.second;
  return;

}  // end 'SetHCalEtaRange(pair<double, double>)'



void SCorrelatorJetTree::SetJetAlgo(const ALGO jetAlgo) {

  switch (jetAlgo) {
    case ALGO::ANTIKT:
      m_jetAlgo = fastjet::antikt_algorithm;
      break;
    case ALGO::KT:
      m_jetAlgo = fastjet::kt_algorithm;
      break;
    case ALGO::CAMBRIDGE:
      m_jetAlgo = fastjet::cambridge_algorithm;
      break;
    default:
      m_jetAlgo = fastjet::antikt_algorithm;
      break;
  }
  return;

}  // end 'SetJetAlgo(ALGO)'



void SCorrelatorJetTree::SetRecombScheme(const RECOMB recombScheme) {

  switch(recombScheme) {
    case RECOMB::E_SCHEME:
      m_recombScheme = fastjet::E_scheme;
      break;
    case RECOMB::PT_SCHEME:
      m_recombScheme = fastjet::pt_scheme;
      break;
    case RECOMB::PT2_SCHEME:
      m_recombScheme = fastjet::pt2_scheme;
      break;
    case RECOMB::ET_SCHEME:
      m_recombScheme = fastjet::Et_scheme;
      break;
    case RECOMB::ET2_SCHEME:
      m_recombScheme = fastjet::Et2_scheme;
      break;
    default:
      m_recombScheme = fastjet::E_scheme;
      break;
  }
  return;

}  // end 'setRecombScheme(RECOMB)'



void SCorrelatorJetTree::SetJetParameters(const double rJet, uint32_t jetType, const ALGO jetAlgo, const RECOMB recombScheme) {

  SetJetR(rJet);
  SetJetType(jetType);
  SetJetAlgo(jetAlgo);
  SetRecombScheme(recombScheme);
  return;

}  // end 'setJetParameters(double, unint32_t, ALGO, RECOMB)'

// end ------------------------------------------------------------------------
