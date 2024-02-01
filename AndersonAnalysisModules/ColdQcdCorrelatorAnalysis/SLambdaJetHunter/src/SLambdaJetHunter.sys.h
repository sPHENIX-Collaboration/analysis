// ----------------------------------------------------------------------------
// 'SLambdaJetHunter.sys.h'
// Derek Anderson
// 01.25.2024
//
// A minimal analysis module to find lambda-tagged
// jets in pythia events.
// ----------------------------------------------------------------------------

#ifndef SLAMBDAJETHUNTER_SYS_H
#define SLAMBDAJETHUNTER_SYS_H

// c++ utilities
#include <cassert>

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // system methods -----------------------------------------------------------

  void SLambdaJetHunter::InitTree() {

    // print debug statement
    if (m_config.isDebugOn) {
      cout << "SLambdaJetHunter::InitTree() Initializing output tree" << endl;
    }

    // TODO add remaining members
    m_outTree = new TTree(m_config.outTreeName.data(), "A tree of lambda-tagged jets");
    m_outTree -> Branch("EvtInfo", "GenInfo", &m_genEvtInfo, 32000, 99);
    return;

  }  // end 'InitTree()'



  void SLambdaJetHunter::InitOutput() {

    // print debug statement
    if (m_config.isDebugOn) {
      cout << "SLambdaJetHunter::InitOuput() Initializing output file" << endl;
    }

    m_outFile = new TFile(m_config.outFileName.data(), "recreate");
    if (!m_outFile) {
      cerr << "PANIC: couldn't open SLambdaJetHunter output file!" << endl;
      assert(m_outFile);
    }
    return;

  }  // end 'InitOutput()'



  void SLambdaJetHunter::SaveAndCloseOutput() {

    // print debug statement
    if (m_config.isDebugOn) {
      cout << "SLambdaJetHunter::SaveAndCloseOuput() Saving to output file and closing" << endl;
    }

    m_outFile -> cd();
    m_outTree -> Write();
    m_outFile -> Close();
    return;

  }  // end 'SaveAndCloseOutput()'

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

