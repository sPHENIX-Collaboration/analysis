// ----------------------------------------------------------------------------
// 'SEnergyCorrelator.io.h'
// Derek Anderson
// 01.27.2023
//
// A module to implement Peter Komiske's
// EEC library in the sPHENIX software
// stack.
// ----------------------------------------------------------------------------

#pragma once

using namespace std;



// i/o methods ----------------------------------------------------------------

void SEnergyCorrelator::SetInputTree(const string &iTreeName, const bool isTruthTree) {

  // print debug statemet
  if (m_inDebugMode) PrintDebug(18);
  
  m_inTreeName       = iTreeName;
  m_isInputTreeTruth = isTruthTree; 
  return;

}  // end 'SetInputTree(string&, bool)'



void SEnergyCorrelator::SetJetParameters(const vector<pair<double, double>> &pTjetBins, const double minEta, const double maxEta) {

  // print debug statement
  if (m_inDebugMode) PrintDebug(20);

  m_etaJetRange[0] = minEta;
  m_etaJetRange[1] = maxEta;
  m_nBinsJetPt     = pTjetBins.size();
  for (uint32_t iPtBin = 0; iPtBin < m_nBinsJetPt; iPtBin++) {
    const double               minPt = pTjetBins.at(iPtBin).first;
    const double               maxPt = pTjetBins.at(iPtBin).second;
    const pair<double, double> ptBin = {minPt, maxPt};
    m_ptJetBins.push_back(ptBin);
  }
  m_ptJetRange[0] = m_ptJetBins[0].first;
  m_ptJetRange[1] = m_ptJetBins[m_nBinsJetPt - 1].second;

  // announce jet parameters
  if (m_inStandaloneMode) PrintMessage(6);
  return;

}  // end 'SetJetParameters(vector<pair<double, double>>&, double, double)'



void SEnergyCorrelator::SetConstituentParameters(const double minMom, const double maxMom, const double minDr, const double maxDr) {

  // print debug statement
  if (m_inDebugMode) PrintDebug(24);

  m_momCstRange[0] = minMom;
  m_momCstRange[1] = maxMom;
  m_drCstRange[0]  = minDr;
  m_drCstRange[1]  = maxDr;

  // announce cst parameters
  if (m_inStandaloneMode) PrintMessage(12);
  return;

}  // end 'SetConstituentParameters(double, double, double, double)'



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

  m_outFile -> cd();
  for (size_t iPtBin = 0; iPtBin < m_nBinsJetPt; iPtBin++) {
    m_outHistDrAxis[iPtBin]   -> Write();
    m_outHistLnDrAxis[iPtBin] -> Write();
  }

  // announce saving
  if (m_inStandaloneMode) PrintMessage(10);
  return;

}  // end 'SaveOutput()'



void SEnergyCorrelator::CloseInputFile() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(29);

  m_inFile -> cd();
  m_inFile -> Close();
  return;

}  // end 'CloseInputFile()'



void SEnergyCorrelator::CloseOutputFile() {

  // print debug statement
  if (m_inDebugMode) PrintDebug(30);

  // close file
  m_outFile -> cd();
  m_outFile -> Close();
  return;

}  // end 'CloseOutputFile()'

// end ------------------------------------------------------------------------
