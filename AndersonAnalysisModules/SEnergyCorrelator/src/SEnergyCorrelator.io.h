// 'SEnergyCorrelator.io.h'
// Derek Anderson
// 01.27.2023
//
// A module to implement Peter Komiske's
// EEC library in the sPHENIX software
// stack.

#pragma once

using namespace std;



void SEnergyCorrelator::SetInputTree(const string &iTreeName, const bool isTruthTree) {

  // print debug statemet
  if (m_inDebugMode) PrintDebug(18);
  
  m_inTreeName       = iTreeName;
  m_isInputTreeTruth = isTruthTree; 
  return;

}  // end 'SetInputTree(string&, bool)'



void SEnergyCorrelator::SetCorrelatorParameters(const uint32_t nPointCorr, const uint64_t nBinsDr, const double minDr, const double maxDr) {

  // print debug statement
  if (m_inDebugMode) PrintDebug(19);

  m_nPointCorr    = nPointCorr;
  m_nBinsDr       = nBinsDr;
  m_drBinRange[0] = minDr;
  m_drBinRange[1] = maxDr;

  // announce correlator parameters
  if (m_inStandaloneMode) PrintMessage(5);
  return;

}  // end 'SetCorrelatorParameters(uint32_t, uint64_t, double, double)'



void SEnergyCorrelator::SetPtJetBins(const vector<pair<double, double>> &pTjetBins) {

  // print debug statement
  if (m_inDebugMode) PrintDebug(20);

  m_nBinsJetPt = pTjetBins.size();
  for (uint32_t iJetBin = 0; iJetBin < m_nBinsJetPt; iJetBin++) {
    const double               minPt = pTjetBins.at(iJetBin).first;
    const double               maxPt = pTjetBins.at(iJetBin).second;
    const pair<double, double> ptBin = {minPt, maxPt};
    m_ptJetBins.push_back(ptBin);
  }

  // announce pTjet bins
  if (m_inStandaloneMode) PrintMessage(6);
  return;

}  // end 'SetPtJetBins(vector<pair<double, double>>&)'



void SEnergyCorrelator::GrabInputNode() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(3);

  /* TODO method goes here */
  return;

}  // end 'GrabInputNode()'



void SEnergyCorrelator::OpenInputFile() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(11);

  // open file
  const bool isTreeNotLoaded = (m_inTree == 0);
  if (isTreeNotLoaded) {

    // check list of files & open if needed
    m_inFile = (TFile*) gROOT -> GetListOfFiles() -> FindObject(m_inFileName.data());
    if (!m_inFile || !(m_inFile -> IsOpen())) {
      m_inFile = new TFile(m_inFileName.data(), "read");
      if (!m_inFile) {
        PrintError(6);
        assert(m_inFile);
      }
    }
  }

  // grab tree
  m_inFile -> GetObject(m_inTreeName.data(), m_inTree);
  if (!m_inTree) {
    PrintError(7);
    assert(m_inTree);
  }
  return;

}  // end 'OpenInputFile()'



void SEnergyCorrelator::OpenOutputFile() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(15);

  // open file
  m_outFile = new TFile(m_outFileName.data(), "recreate");
  if (!m_outFile) {
    PrintError(11);
    assert(m_outFile);
  }
  return;

}  // end 'OpenOutputFile()'



void SEnergyCorrelator::SaveOutput() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(9);

  /* TODO saving goes here */

  // announce saving
  if (m_inStandaloneMode) PrintMessage(10);
  return;

}  // end 'SaveOutput()'

// end ------------------------------------------------------------------------
