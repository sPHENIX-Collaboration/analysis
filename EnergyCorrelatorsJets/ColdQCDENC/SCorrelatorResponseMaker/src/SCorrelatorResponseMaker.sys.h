/// ---------------------------------------------------------------------------
/*! \file   SCorrelatorResponseMaker.sys.h
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



// system methods =============================================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Open a specified file for reading
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::OpenFile(const string& fileName, TFile*& file) {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 2)) {
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
  //! Initialize input/output trees
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::InitializeTrees() {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 1)) {
      PrintDebug(11);
    }

    // check for trees
    if (!m_inRecoTree) {
      PrintError(2);
      assert(m_inRecoTree);
    }
    if (!m_inTrueTree) {
      PrintError(1);
      assert(m_inTrueTree);
    }
    m_fRecoCurrent = -1;
    m_fTrueCurrent = -1;
    m_inRecoTree   -> SetMakeClass(1);
    m_inTrueTree   -> SetMakeClass(1);

    // set input trees
    if (m_config.isLegacyIO) {
      m_recoLegacy.SetTreeAddresses(m_inRecoTree);
      m_trueLegacy.SetTreeAddresses(m_inTrueTree);
    } else {
      m_recoInput.SetTreeAddresses(m_inRecoTree);
      m_trueInput.SetTreeAddresses(m_inTrueTree);
    }

    // initialize response tree
    m_matchTree = new TTree("ResponseTree", "A tree of matched truth-reco. events");
    if (m_config.isLegacyIO) {
      m_outLegacy.SetTreeAddresses(m_matchTree);
    } else {
      m_output.SetTreeAddresses(m_matchTree);
    }

    // announce tree setting
    PrintMessage(5);
    return;

  }  // end 'InitializeTree()'



  // --------------------------------------------------------------------------
  //! Fill output tree
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::FillTree() {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 1)) {
      PrintDebug(14);
    }

    // if making legacy output, translate accordingly
    if (m_config.isLegacyIO) {
      m_outLegacy.GetOutput(m_output);
    }

    // fill output tree
    m_matchTree -> Fill();
    return;

  }  // end 'FillTree()'



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
    PrintMessage(4);
    return;

  }  // end 'SaveOutput()'



  // --------------------------------------------------------------------------
  //! Reset output variables
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::ResetOutVariables() {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 1)) {
      PrintDebug(16);
    }

    m_output.Reset();
    m_outLegacy.Reset();
    return;

  }  // end 'ResetOutVariables()'



  // --------------------------------------------------------------------------
  //! Reset bookkeeping variables
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::ResetBookkeepers() {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 2)) {
      PrintDebug(17);
    }

    m_mapTrueToRecoCst.clear();
    return;

  }  // end 'ResetBookkeepers()'



  // --------------------------------------------------------------------------
  //! Print standard operation messages
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::PrintMessage(
    const uint32_t code,
    const uint64_t iEvt,
    const pair<uint64_t, uint64_t> nEvts
  ) {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 3)) {
      PrintDebug(12);
    }

    switch (code) {
      case 0:
        cout << "\n  Beginning correlator response making..." << endl;
        break;
      case 1:
        cout << "    Initializing response maker:\n"
             << "      input truth file = " << m_config.inTrueFileName << "\n"
             << "      input reco file  = " << m_config.inRecoFileName
             << endl;
        break;
      case 2:
        cout << "    Beginning analysis." << endl;
        break;
      case 3:
        cout << "  Correlator folding complete!\n" << endl;
        break;
      case 4:
        cout << "      Saved output." << endl;
        break;
      case 5:
        cout << "    Initialized input trees:\n"
             << "      input truth tree = " << m_config.inTrueTreeName << "\n"
             << "      input reco tree  = " << m_config.inRecoTreeName
             << endl;
        break;
      case 6:
        cout << "    Beginning event loop: " << nEvts.first << " truth events and " << nEvts.second << " reconstructed events to process." << endl;
        break;
      case 7:
        if (iEvt == nEvts.first) {
          cout << "      Processing event " << iEvt << "/" << nEvts.first << "..." << endl;
        } else {
          cout << "      Processing event " << iEvt << "/" << nEvts.first << "...\r" << flush;
        }
        break;
      case 8:
        cout << "    Finished event loop." << endl;
        break;
      default:
        PrintError(code);
        break;
    }
    return;

  }  // end 'PrintMessage(uint32_t, uint64_t, pair<uint64_t, uint64_t>)'



  // --------------------------------------------------------------------------
  //! Print debugging messages
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::PrintDebug(const uint32_t code) {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 3)) {
      cout << "SCorrelatorResponseMaker::PrintDebug(uint32_t) printing a debugging statement..." << endl;
    }

    switch (code) {
      case 0:
        cout << "SCorrelatorResponseMaker::SCorrelatorResponseMaker() constructing response maker..." << endl;
        break;
      case 1:
        cout << "SCorrelatorResponseMaker::~SCorrelatorResponseMaker() destructing response maker..." << endl;
        break;
      case 2:
        cout << "SCorrelatorResponseMaker::Init() initializing..." << endl;
        break;
      case 3:
        cout << "SCorrelatorResponseMaker::Analyze() running analysis..." << endl;
        break;
      case 4:
        cout << "SCorrelatorResponseMaker::End() ending..." << endl;
        break;
      case 5:
        cout << "SCorrelatorResponseMaker::GrabInputNodes() grabbing input nodes..." << endl;
        break;
      case 6:
        cout << "SCorrelatorResponseMaker::OpenInputFiles() opening input files..." << endl;
        break;
      case 7:
        cout << "SCorrelatorResponseMaker::OpenOutputFile() opening output file..." << endl;
        break;
      case 8:
        cout << "SCorrelatorResponseMaker::OpenFile() opening a single file..." << endl;
        break;
      case 9:
        cout << "SCorrelatorResponseMaker::SaveOutput() saving output..." << endl;
        break;
      case 10:
        cout << "SCorrelatorResponseMaker::DoMatching() matching jets..." << endl;
        break;
      case 11:
        cout << "SCorrelatorResponseMaker::InitializeTrees() initializing trees..." << endl;
        break;
      case 12:
        cout << "SCorrelatorResponseMaker::PrintMessage(uint32_t) printing a message..." << endl;
        break;
      case 13:
        cout << "SCorrelatorResponseMaker::PrintError(uint32_t) printing an error..." << endl;
        break;
      case 14:
        cout << "SCorrelatorResponseMaker::FillTree() filling output tree..." << endl;
        break; 
      case 15:
        cout << "SCorrelatorResponseMaker::IsBetterMatch(double, double) checking if match is best so far..." << endl;
        break;
      case 16:
        cout << "SCorrelatorResponseMaker::ResetOutVariables() resetting output variables..." << endl;
        break;
      case 17:
        cout << "SCorrelatorResponseMaker::ResetBookkeepers() resetting bookkeeping variables..." << endl;
        break;
      default:
        PrintError(code);
        break;
    }
    return;

  }  // end 'PrintDebug(uint32_t)'



  // --------------------------------------------------------------------------
  //! Print error messages
  // --------------------------------------------------------------------------
  void SCorrelatorResponseMaker::PrintError(const uint32_t code, const uint64_t iEvt) {

    // print debug statement
    if (m_config.inDebugMode && (m_config.verbosity > 3)) {
      PrintDebug(13);
    }

    switch (code) {
      case 1:
        cerr << "PANIC: couldn't open input file! Aborting!\n" << endl;
        break;
      case 2:
        cerr << "PANIC: no input truth tree! Aborting!\n" << endl;
        break;
      case 3:
        cerr << "PANIC: no input reco tree! Aborting!\n" << endl;
        break;
      case 4:
        cerr << "WARNING: issue with entry " << iEvt << " in truth tree! Aborting event loop!" << endl;
        break;
      case 5:
        cerr << "WARNING: issue with entry " << iEvt << " in reco tree! Aborting event loop!" << endl;
        break;
      default:
        cerr << "WARNING: unknown status code!\n"
             << "         code = " << code
             << endl;
        break;
    }
    return;

  }  // end 'PrintError(unint32_t, uint64_t)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
