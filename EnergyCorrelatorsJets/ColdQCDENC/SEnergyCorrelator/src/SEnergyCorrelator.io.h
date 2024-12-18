/// ---------------------------------------------------------------------------
/*! \file    SEnergyCorrelator.io.h
 *  \authors Derek Anderson, Alex Clarke
 *  \date    01.20.2023
 *
 *  A module to run ENC calculations in the sPHENIX
 *  software stack for the Cold QCD EEC analysis.
 */
/// ---------------------------------------------------------------------------

#pragma once

// make common namespaces implicit
using namespace std;
using namespace findNode;



namespace SColdQcdCorrelatorAnalysis {

  // i/o methods ==============================================================

  // --------------------------------------------------------------------------
  //! Open input files
  // --------------------------------------------------------------------------
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



  // --------------------------------------------------------------------------
  //! Open output files
  // --------------------------------------------------------------------------
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



  // --------------------------------------------------------------------------
  //! Save output
  // --------------------------------------------------------------------------
  void SEnergyCorrelator::SaveOutput() {

    // print debug statement
    if (m_config.isDebugOn) PrintDebug(9);

    // save local output histograms
    //   - FIXME move to a dedicated histogram collection
    m_outFile -> cd();
    if (m_config.doLocalCalc) {
      for (size_t iPtBin = 0; iPtBin < m_config.ptJetBins.size(); iPtBin++) {

        // save package histograms
        if (m_config.doPackageCalc) {
          m_outPackageHistVarDrAxis[iPtBin]   -> Write();
          m_outPackageHistErrDrAxis[iPtBin]   -> Write();
          m_outPackageHistVarLnDrAxis[iPtBin] -> Write();
          m_outPackageHistErrLnDrAxis[iPtBin] -> Write();
        }

        // save manual histograms
        if (m_config.doManualCalc) {
          m_outManualHistErrDrAxis[iPtBin]->Write();
          if (m_config.doThreePoint) {
            m_outProjE3C[iPtBin]->Write();
            for(size_t jRLBin = 0; jRLBin < m_config.rlBins.size(); jRLBin++){
              m_outE3C[iPtBin][jRLBin]->Write();
            }  // end rl bin loop
          }  // end if three point
        }  // end if manual
      } // end pt bin loop
    }  // end if local

     // save global output histograms
    //   - FIXME move to a dedicated histogram collection
    if (m_config.doGlobalCalc) {
      for (size_t iHtBin = 0; iHtBin < m_config.htEvtBins.size(); iHtBin++) {
        m_outGlobalHistDPhiAxis.at(iHtBin)  -> Write();
        m_outGlobalHistCosDFAxis.at(iHtBin) -> Write();
      }  // end ht bin loop
    }  // end if global

    // announce saving
    PrintMessage(10);
    return;

  }  // end 'SaveOutput()'



  // --------------------------------------------------------------------------
  //! Close output files
  // --------------------------------------------------------------------------
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
