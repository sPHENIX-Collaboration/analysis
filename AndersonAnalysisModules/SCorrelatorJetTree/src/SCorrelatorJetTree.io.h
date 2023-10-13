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



void SCorrelatorJetTree::SetTrackQualityRange(const pair<double, double> qualRange) {

  m_trkQualRange[0] = qualRange.first;
  m_trkQualRange[1] = qualRange.second;
  return;

}  // end 'SetTrackQualityRange(pair<double, double>)'



void SCorrelatorJetTree::SetTrackNMvtxRange(const pair<double, double> nMvtxRange) {

  m_trkNMvtxRange[0] = nMvtxRange.first;
  m_trkNMvtxRange[1] = nMvtxRange.second;
  return;

}  // end 'SetTrackNMvtxRange(pair<double, double>)'



void SCorrelatorJetTree::SetTrackNInttRange(const pair<double, double> nInttRange) {

  m_trkNInttRange[0] = nInttRange.first;
  m_trkNInttRange[1] = nInttRange.second;
  return;

}  // end 'SetTrackNInttRange(pair<double, double>)'



void SCorrelatorJetTree::SetTrackNTpcRange(const pair<double, double> nTpcRange) {

  m_trkNTpcRange[0] = nTpcRange.first;
  m_trkNTpcRange[1] = nTpcRange.second;
  return;

}  // end SetTrackNTpcRange(pair<double, double>)'



void SCorrelatorJetTree::SetTrackDcaRangeXY(const pair<double, double> dcaRangeXY) {

  m_trkDcaRangeXY[0] = dcaRangeXY.first;
  m_trkDcaRangeXY[1] = dcaRangeXY.second;
  return;

}  // end 'SetTrackDcaRangeXY(pair<double, double>)'



void SCorrelatorJetTree::SetTrackDcaRangeZ(const pair<double, double> dcaRangeZ) {

  m_trkDcaRangeZ[0] = dcaRangeZ.first;
  m_trkDcaRangeZ[1] = dcaRangeZ.second;
  return;

}  // end 'SetTrackDcaRangeZ(pair<double, double>)'



void SCorrelatorJetTree::SetTrackDeltaPtRange(const pair<double, double> deltaPtRange) {

  m_trkDeltaPtRange[0] = deltaPtRange.first;
  m_trkDeltaPtRange[1] = deltaPtRange.second;
  return;

}  // end 'SetTrackDeltaPtRange(pair<double, double>)'



void SCorrelatorJetTree::SetTrackDcaSigmaParameters(const bool doDcaSigmaCut, const pair<double, double> nSigma, const vector<double> paramDcaXY, const vector<double> paramDcaZ) {

  m_doDcaSigmaCut = doDcaSigmaCut;
  m_nSigCutXY     = nSigma.first;
  m_nSigCutZ      = nSigma.second;
  for (uint8_t iParam = 0; iParam < CONST::NParam; iParam++) {

    // try to set dca xy values
    try {
      m_parSigDcaXY[iParam] = paramDcaXY.at(iParam);
    } catch (std::out_of_range &out) {
      cerr << "SCorrelatorJetTree::SetTrackDcaSigmaParameters: WARNING!\n"
           << "  Tried to pass a vector of wrong size for sigma dca xy fit parameters!\n"
           << "  Size of vector was " << paramDcaXY.size() << " but should've been " << CONST::NParam << "..."
           << endl;
      assert(paramDcaXY.size() == CONST::NParam);
    }

    // try to set dca z values
    try {
      m_parSigDcaZ[iParam] = paramDcaZ.at(iParam);
    } catch (std::out_of_range &out) {
      cerr << "SCorrelatorJetTree::SetTrackDcaSigmaParameters: WARNING!\n"
           << "  Tried to pass a vector of wrong size for sigma dca z fit parameters!\n"
           << "  Size of vector was " << paramDcaZ.size() << " but should've been " << CONST::NParam << "..."
           << endl;
      assert(paramDcaZ.size() == CONST::NParam);
    }
  }  // end parameter loop
  return;

}  // end 'SetTrackDcaSigmaParameters(bool, pair<double, double>, vector<double>, vector<double>)'



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
