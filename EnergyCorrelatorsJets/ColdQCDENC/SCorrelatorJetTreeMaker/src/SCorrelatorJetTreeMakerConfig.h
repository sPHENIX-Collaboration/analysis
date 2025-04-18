/// ---------------------------------------------------------------------------
/*! \file   SCorrelatorJetTreeMakerConfig.h
 *  \author Derek Anderson
 *  \date   03.22.2024
 *
 *  A module to produce a tree of jets for the sPHENIX
 *  Cold QCD Energy-Energy Correlator analysis. Initially
 *  derived from code by Antonio Silva.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORJETTREEMAKERCONFIG_H
#define SCORRELATORJETTREEMAKERCONFIG_H

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! User options for module
  // --------------------------------------------------------------------------
  struct SCorrelatorJetTreeMakerConfig {

    // system options
    int    verbosity       {0};
    bool   readJetNodes    {false};
    bool   isDebugOn       {false};
    bool   isSimulation    {true};
    bool   isEmbed         {false};
    string moduleName      {""};
    string inRecoNodeName  {""};
    string inTrueNodeName  {""};
    string outFileName     {""};
    string recoJetTreeName {""};
    string trueJetTreeName {""};

    // jet options
    float          rJet      {0.4};
    string         jetAlgo   {"antikt"};
    string         jetRecomb {"pt"};
    string         jetArea   {"active"};
    Const::JetType jetType   {Const::JetType::Charged};

    // cut options
    bool             doVtxCut       {false};
    bool             doDcaSigmaCut  {false};
    bool             requireSiSeeds {true};
    bool             useOnlyPrimVtx {true};
    Const::SubEvtOpt subEvtOpt      {Const::SubEvtOpt::Everything};

    // vertex cuts
    pair<float, float> vrAccept;
    pair<float, float> vzAccept;

    // constituent cuts
    pair<Types::TrkInfo,   Types::TrkInfo>   trkAccept;
    pair<Types::FlowInfo,  Types::FlowInfo>  flowAccept;
    pair<Types::TwrInfo,   Types::TwrInfo>   eTwrAccept;
    pair<Types::TwrInfo,   Types::TwrInfo>   hTwrAccept;
    pair<Types::ClustInfo, Types::ClustInfo> eClustAccept;
    pair<Types::ClustInfo, Types::ClustInfo> hClustAccept;
    pair<Types::ParInfo,   Types::ParInfo>   parAccept;

    // for pt-dependent dca cuts
    pair<float, float> nSigCut;
    pair<float, float> ptFitMax;
    pair<TF1*,  TF1*>  fSigDca;

  };  // end SCorrelatorJetTreeMakerConfig

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
