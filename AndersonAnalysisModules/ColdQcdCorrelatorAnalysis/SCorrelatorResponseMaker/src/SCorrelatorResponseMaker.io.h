/// ---------------------------------------------------------------------------
/*! \file   SCorrelatorResponseMaker.io.h
 *  \author Derek Anderson
 *  \date   04.30.2023
 *
 * A module to match truth to reconstructed
 * jets/particles to derive corrections for
 * an n-point energy correlation strength
 * function.
 */
/// ---------------------------------------------------------------------------

#pragma once

using namespace std;



// i/o methods ================================================================

namespace SColdQcdCorrelatorAnalysis {

  // -------------------------------------------------------------------------
  //! Grab input nodes
  // -------------------------------------------------------------------------
  void SCorrelatorResponseMaker::GrabInputNodes() {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 1)) {
      PrintDebug(5);
    }

    /* TODO method goes here */
    return;

  }  // end 'GrabInputNodes()'



  // --------------------------------------------------------------------------
  //! Open input files
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::OpenInputFiles() {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 1)) {
      PrintDebug(6);
    }

    // open files
    const bool isTrueTreeNotLoaded = (!m_inTrueTree);
    const bool isRecoTreeNotLoaded = (!m_inRecoTree);
    if (isTrueTreeNotLoaded) OpenFile(m_config.inTrueFileName, m_inTrueFile);
    if (isRecoTreeNotLoaded) OpenFile(m_config.inRecoFileName, m_inRecoFile);

    // try to grab trees
    try {
      m_inTrueFile -> GetObject(m_config.inTrueTreeName.data(), m_inTrueTree);
      m_inRecoFile -> GetObject(m_config.inRecoTreeName.data(), m_inRecoTree);
    } catch (...) {
      PrintError(1);
      assert(m_inTrueFile && m_inRecoFile);
    }

    // check if trees exist
    if (!m_inTrueTree) {
      PrintError(2);
      assert(m_inTrueTree);
    }
    if (!m_inRecoTree) {
      PrintError(3);
      assert(m_inRecoTree);
    }
    return;

  }  // end 'OpenInputFiles()'



  // --------------------------------------------------------------------------
  //! Open output file
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::OpenOutputFile() {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 1)) {
      PrintDebug(7);
    }

    // open file
    m_outFile = new TFile(m_config.outFileName.data(), "recreate");
    if (!m_outFile) {
      PrintError(11);
      assert(m_outFile);
    }
    return;

  }  // end 'OpenOutputFile()'


  // --------------------------------------------------------------------------
  //! Open a specified file for reading
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::OpenFile(const string& fileName, TFile*& file) {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbsoity > 2)) {
      PrintDebug(8);
    }

    file = (TFile*) gROOT -> GetListOfFiles() -> FindObject(fileName.data());
    if (!file || !(file -> IsOpen())) {
      file = new TFile(fileName.data(), "read");
      if (!file) {
        PrintError(6);
        assert(file);
      }
    }  // end if (file dne or isn't open)
    return;

  }  // end 'OpenFile(string&, TFile*&)'



  // --------------------------------------------------------------------------
  //! Save output
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::SaveOutput() {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 1)) {
      PrintDebug(9);
    }

    m_outFile   -> cd();
    m_matchTree -> Write();

    // announce saving
    if (m_config.isStandalone) PrintMessage(4);
    return;

  }  // end 'SaveOutput()'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
