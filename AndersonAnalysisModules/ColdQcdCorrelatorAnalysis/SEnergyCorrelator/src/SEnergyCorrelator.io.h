// ----------------------------------------------------------------------------
// 'SEnergyCorrelator.io.h'
// Derek Anderson
// 01.27.2023
//
// A module to implement Peter Komiske's EEC library
// in the sPHENIX software stack for the Cold QCD
// Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#pragma once

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {

  // i/o methods --------------------------------------------------------------

  void SEnergyCorrelator::GrabInputNode() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(3);

    /* TODO method goes here */
    return;

  }  // end 'GrabInputNode()'



  void SEnergyCorrelator::OpenInputFiles() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(11);

    // insantiate input chain
    m_inChain = new TChain(m_config.inTreeName.data());
    if (!m_inChain) {
      PrintError(6);
      assert(m_inChain);
    }

    // loop over provided input files
    bool     isFileGood = true;
    uint64_t bytes      = 0;
    for (const string& inFileName : m_config.inFileNames) {
      bytes      = m_inChain -> Add(inFileName.data(), 0);
      isFileGood = (bytes > 0);
      if (!isFileGood) {
        PrintError(7, 0, 0, inFileName);
        assert(isFileGood);
      }
    }  // end input file loop
    return;

  }  // end 'OpenInputFile()'



  void SEnergyCorrelator::OpenOutputFile() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(15);

    // open file
    m_outFile = new TFile(m_config.outFileName.data(), "recreate");
    if (!m_outFile) {
      PrintError(11);
      assert(m_outFile);
    }
    return;

  }  // end 'OpenOutputFile()'



  void SEnergyCorrelator::SaveOutput() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(9);

    m_outFile -> cd();
    for (size_t iPtBin = 0; iPtBin < m_config.ptJetBins.size(); iPtBin++) {
      m_outPackageHistVarDrAxis[iPtBin]   -> Write();
      m_outPackageHistErrDrAxis[iPtBin]   -> Write();
      m_outPackageHistVarLnDrAxis[iPtBin] -> Write();
      m_outPackageHistErrLnDrAxis[iPtBin] -> Write();
      if(m_config.doManualCalc){
	m_outManualHistErrDrAxis[iPtBin]->Write();
      }
    }

    // announce saving
    if (m_config.isStandalone) PrintMessage(10);
    return;

  }  // end 'SaveOutput()'



  void SEnergyCorrelator::CloseOutputFile() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(30);

    // close file
    m_outFile -> cd();
    m_outFile -> Close();
    return;

  }  // end 'CloseOutputFile()'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
