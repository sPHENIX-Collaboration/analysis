// ----------------------------------------------------------------------------
// 'SCorrelatorQAMaker.cc'
// Derek Anderson
// 10.04.2023
//
// A module to produce TNtuples and histograms for QA
// plots relevant to the sPHENIX Cold QCD Energy-Energy
// Correlator analysis.
// ----------------------------------------------------------------------------

#define SCORRELATORQAMAKER_CC

// module definition
#include "SCorrelatorQAMaker.h"

using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // ctor/dtor ----------------------------------------------------------------

  SCorrelatorQAMaker::~SCorrelatorQAMaker() {

    delete m_checkTrackPairs;
    delete m_makeTrackQATuple;
    delete m_makeClustQATree;
    delete m_readLambdaJetTree;

  }  // end dtor



  // global setters -----------------------------------------------------------

  void SCorrelatorQAMaker::SetGlobalDebug(const bool debug) {

    if (m_checkTrackPairs)   m_checkTrackPairs   -> SetDebug(debug);
    if (m_makeTrackQATuple)  m_makeTrackQATuple  -> SetDebug(debug);
    if (m_makeClustQATree)   m_makeClustQATree   -> SetDebug(debug);
    if (m_readLambdaJetTree) m_readLambdaJetTree -> SetDebug(debug);
    return;

  }  // end 'SetGlobalDebug(bool)'



  void SCorrelatorQAMaker::SetGlobalOutFile(const string sOutFile) {

    if (m_checkTrackPairs)   m_checkTrackPairs   -> SetOutFile(sOutFile);
    if (m_makeTrackQATuple)  m_makeTrackQATuple  -> SetOutFile(sOutFile);
    if (m_makeClustQATree)   m_makeClustQATree   -> SetOutFile(sOutFile);
    if (m_readLambdaJetTree) m_readLambdaJetTree -> SetOutFile(sOutFile);
    return;

  }  // end 'SetGlobalOutFile(string)'



  void SCorrelatorQAMaker::SetGlobalVerbosity(const int verbosity) {

    if (m_checkTrackPairs)   m_checkTrackPairs   -> SetVerbosity(verbosity);
    if (m_makeTrackQATuple)  m_makeTrackQATuple  -> SetVerbosity(verbosity);
    if (m_makeClustQATree)   m_makeClustQATree   -> SetVerbosity(verbosity);
    if (m_readLambdaJetTree) m_readLambdaJetTree -> SetVerbosity(verbosity);
    return;

  }  // end 'SetGlobalVerbosity(int)'



  // plugin initializers ------------------------------------------------------

  // specialization for SCheckTrackPairs
  template <> void SCorrelatorQAMaker::InitPlugin(const SCheckTrackPairsConfig& config, optional<string> name) {

    // throw error if no name provided
    if (!name.has_value()) {
      assert(name.has_value());
    }

    m_checkTrackPairs = new SCheckTrackPairs(name.value());
    m_checkTrackPairs -> SetConfig(config);
    return;

  }  // end 'InitPlugin(SCheckTrackPairs&, optional<string>)'



  // specialization for SMakeTrackQATuple
  template <> void SCorrelatorQAMaker::InitPlugin(const SMakeTrackQATupleConfig& config, optional<string> name) {

    // throw error if no name provided
    if (!name.has_value()) {
      assert(name.has_value());
    }

    m_makeTrackQATuple = new SMakeTrackQATuple(name.value());
    m_makeTrackQATuple -> SetConfig(config);
    return;

  }  // end 'InitPlugin(SMakeTrackQATuple&, optional<string>)'



  // specialization for SMakeClustQATree
  template <> void SCorrelatorQAMaker::InitPlugin(const SMakeClustQATreeConfig& config, optional<string> name) {

    // throw error if no name provided
    if (!name.has_value()) {
      assert(name.has_value());
    }

    m_makeClustQATree = new SMakeClustQATree(name.value());
    m_makeClustQATree -> SetConfig(config);
    return;

  }  // end 'InitPlugin(SMakeClustQATreeConfig&, optional<string>)'



  // specialization for SReadLambdaJetTree
  template <> void SCorrelatorQAMaker::InitPlugin(const SReadLambdaJetTreeConfig& config, optional<string> name) {

    m_readLambdaJetTree = new SReadLambdaJetTree();
    m_readLambdaJetTree -> SetConfig(config);
    return;

  }  // end 'InitPlugin(SReadLambdaJetTreeConfig&, optional<string>)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
