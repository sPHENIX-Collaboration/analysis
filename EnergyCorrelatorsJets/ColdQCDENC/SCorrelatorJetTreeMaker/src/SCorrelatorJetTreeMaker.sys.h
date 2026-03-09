/// ---------------------------------------------------------------------------
/*! \file   SCorrelatorJetTreeMaker.sys.h
 *  \author Derek Anderson
 *  \date   01.18.2023
 *
 *  A module to produce a tree of jets for the sPHENIX
 *  Cold QCD Energy-Energy Correlator analysis. Initially
 *  derived from code by Antonio Silva.
 */
/// ---------------------------------------------------------------------------

#pragma once

using namespace std;
using namespace findNode;



// system methods =============================================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Open output file
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::OpenOutFile() {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 1)) {
      cout << "SCorrelatorJetTreeMaker::OpenOutFile() Opening output file..." << endl;
    }

    m_outFile = new TFile(m_config.outFileName.c_str(), "RECREATE");
    if (!m_outFile) {
      cerr << "PANIC: couldn't open SCorrelatorJetTreeMaker output file!" << endl;
    }
    return;

  }  // end 'InitOutFile()'



  // --------------------------------------------------------------------------
  //! Initialize output trees
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::InitTrees() {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 1)) {
      cout << "SCorrelatorJetTreeMaker::InitTrees() Initializing output trees..." << endl;
    }

    // initialize reco tree
    m_recoTree = new TTree("RecoJetTree", "A tree of reconstructed jets");
    m_recoInterface.SetTreeAddresses(m_recoTree);

    // initialize truth tree
    if (m_config.isSimulation) {
      m_trueTree = new TTree("TruthJetTree", "A tree of truth jets");
      m_trueInterface.SetTreeAddresses(m_trueTree);
    }
    return;

  }  // end 'InitTrees()'



  // --------------------------------------------------------------------------
  //! Initialize FastJet
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::InitFastJet() {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 1)) {
      cout << "SCorrelatorJetTreeMaker::InitFastjet() Initializing fastjet..." << endl;
    }

    // set reco jet definition
    m_recoJetDef = make_unique<JetDefinition>(
      Const::MapStringOntoFJAlgo()[ m_config.jetAlgo ],
      m_config.rJet,
      Const::MapStringOntoFJRecomb()[ m_config.jetRecomb ],
      fastjet::Best
    );

    // set truth jet definition if needed
    if (m_config.isSimulation) {
      m_trueJetDef = make_unique<JetDefinition>(
        Const::MapStringOntoFJAlgo()[ m_config.jetAlgo ],
        m_config.rJet,
        Const::MapStringOntoFJRecomb()[ m_config.jetRecomb ],
        fastjet::Best
      );
    }
    return;

  }  // end 'InitFastJet()'



  // --------------------------------------------------------------------------
  //! Initialize track evaluators
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::InitEvals(PHCompositeNode* topNode) {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 1)) {
      cout << "SCorrelatorJetTreeMaker::InitEvals(PHCompositeNode*) Initializing evaluators..." << endl;
    }

    m_evalStack = new SvtxEvalStack(topNode);
    if (!m_evalStack) {
      cerr << "SCorrelatorJetTreeMaker::InitEvals(PHCompositeNode*) PANIC: couldn't grab SvtxEvalStack! Aborting!" << endl;
      assert(m_evalStack);
    } else {
      m_evalStack -> next_event(topNode);
    }

    m_trackEval = m_evalStack -> get_track_eval();
    if (!m_trackEval) {
      cerr << "SCorrelatorJetTreeMaker::InitEvals(PHCompositeNode*) PANIC: couldn't grab track evaluator! Aborting!" << endl;
      assert(m_trackEval);
    }
    return;

  }  // end 'InitEvals(PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Fill output trees
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::FillTrees() {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 1)) {
      cout << "SCorrelatorJetTreeMaker::FillTrueTree() Filling jet trees..." << endl;
    }

    // fill tree addresses from output structs
    m_recoInterface.GetTreeMakerOutput(m_recoOutput);
    if (m_config.isSimulation) {
      m_trueInterface.GetTreeMakerOutput(m_trueOutput);
    }

    // fill output trees
    m_recoTree -> Fill();
    if (m_config.isSimulation) {
      m_trueTree -> Fill();
    }
    return;

  }  // end'FillTrees()'



  // --------------------------------------------------------------------------
  //! Save trees to output file
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::SaveOutput() {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 1)) {
      cout << "SCorrelatorJetTreeMaker::SaveOutput() Saving output trees..." << endl;
    }

    // save output trees
    m_outFile  -> cd();
    m_recoTree -> Write();
    if (m_config.isSimulation) {
      m_trueTree -> Write();
    }
    return;

  }  // end 'SaveOutput()'



  // --------------------------------------------------------------------------
  //! Close output file
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::CloseOutFile() {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 1)) {
      cout << "SCorrelatorJetTreeMaker::SaveOutput() Closing output file..." << endl;
    }

    m_outFile -> cd();
    m_outFile -> Close();
    return;

  }  // end 'CloseOutFile()'



  // --------------------------------------------------------------------------
  //! Reset book-keeping variables
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::ResetSysVariables() {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 2)) {
      cout << "SCorrelatorJetTreeMaker::ResetSysVariables() Resetting system variables..." << endl;
    }

    m_vecEvtsToGrab.clear();
    m_mapCstToEmbedID.clear();
    return;

  }  // end 'ResetSysVariables()'



  // --------------------------------------------------------------------------
  //! Reset addresses for output trees
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::ResetOutVariables() {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 2)) {
      cout << "SCorrelatorJetTreeMaker::ResetOutVariables() Resetting output variables..." << endl;
    }

    m_recoOutput.Reset();
    m_trueOutput.Reset();
    m_recoInterface.Reset();
    m_trueInterface.Reset();
    return;

  }  // end 'ResetOutVariables()'



  // --------------------------------------------------------------------------
  //! Reset jet-specific book-keeping variables
  // --------------------------------------------------------------------------
  void SCorrelatorJetTreeMaker::ResetJetVariables() {

    // print debug statement
    if (m_config.isDebugOn && (m_config.verbosity > 2)) {
      cout << "SCorrelatorJetTreeMaker::ResetJetVariables() Resetting jet variables..." << endl;
    }

    m_recoJets.clear();
    m_trueJets.clear();
    m_recoJetInput.clear();
    m_trueJetInput.clear();
    return;

  }  // end 'ResetJetVariables()'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
