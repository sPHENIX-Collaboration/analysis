/// ---------------------------------------------------------------------------
/*! \file   JetTreeMakerOptions.h 
 *  \author Derek Anderson
 *  \date   04.11.2024
 *
 *  Options for the SCorrelatorJetTreeMaker module
 */
/// ---------------------------------------------------------------------------

#ifndef JETTREEMAKEROPTIONS_H
#define JETTREEMAKEROPTIONS_H

// c++ utilities
#include <limits>
#include <string>
#include <vector>
#include <utility>
// root libraries
#include <TF1.h>
// analysis utilties
#include <scorrelatorutilities/Types.h>
#include <scorrelatorutilities/Constants.h>
#include <scorrelatorutilities/Interfaces.h>
// module configuration
#include <scorrelatorjettreemaker/SCorrelatorJetTreeMakerConfig.h>

// make common namespaces implicit
using namespace std;
using namespace SColdQcdCorrelatorAnalysis;



namespace JetTreeMakerOptions {

  // acceptance cuts ==========================================================

  // event acceptances
  const pair<float, float> vzEvtRange = {-10., 10.};
  const pair<float, float> vrEvtRange = {0.0, 0.418};

  // track acceptances
  const pair<int,   int>   nMvtxTrkRange = {2, numeric_limits<int>::max()};
  const pair<int,   int>   nInttTrkRange = {1, numeric_limits<int>::max()};
  const pair<int,   int>   nTpcTrkRange  = {24, numeric_limits<int>::max()};
  const pair<float, float> ptTrkRange    = {0.2, 100.};
  const pair<float, float> etaTrkRange   = {-1.1, 1.1};
  const pair<float, float> qualTrkRange  = {0., 10.};
  const pair<float, float> dcaTrkRangeXY = {-5., 5.};
  const pair<float, float> dcaTrkRangeZ  = {-5., 5.};
  const pair<float, float> ptErrTrkRange = {0., 0.5};

  // for pt dependent dca cuts
  const pair<float, float> dcaPtFitMax    = {15., 15.};
  const pair<float, float> nDcaSigmaTrk   = {3., 3.};
  const vector<float>      dcaSigmaParsXY = {-0.0095, 0.091, -0.029};
  const vector<float>      dcaSigmaParsZ  = {1.73, 26.1, -9.45};

  // particle flow acceptances
  const pair<float, float> ptFlowRange  = {0.2, 100.};
  const pair<float, float> etaFlowRange = {-1.1, 1.1};

  // calo tower acceptances
  const pair<float, float> eETwrRange   = {0.2, 100.};
  const pair<float, float> etaETwrRange = {-1.1, 1.1};
  const pair<float, float> eHTwrRange   = {0.2, 100.};
  const pair<float, float> etaHTwrRange = {-1.1, 1.1};

  // calo cluster acceptances
  const pair<float, float> eEClustRange   = {0.3, 100.};
  const pair<float, float> etaEClustRange = {-1.1, 1.1};
  const pair<float, float> eHClustRange   = {0.3, 100.};
  const pair<float, float> etaHClustRange = {-1.1, 1.1};

  // particle acceptances
  const pair<float, float> ptParRange  = {0., 100.};
  const pair<float, float> etaParRange = {-1.1, 1.1};



  // bundle acceptances into pairs ============================================

  // --------------------------------------------------------------------------
  //! Bundle track acceptance cuts into a pair
  // --------------------------------------------------------------------------
  pair<Types::TrkInfo, Types::TrkInfo> GetTrkAccept(const bool doSigmaCut = false) {

    // create maximal range
    pair<Types::TrkInfo, Types::TrkInfo> trkAccept = {
      Types::TrkInfo(Const::Init::Minimize),
      Types::TrkInfo(Const::Init::Maximize)
    };

    // set specific bounds
    trkAccept.first.SetNMvtxLayer( nMvtxTrkRange.first );
    trkAccept.first.SetNInttLayer( nInttTrkRange.first );
    trkAccept.first.SetNTpcLayer( nTpcTrkRange.first );
    trkAccept.first.SetPT( ptTrkRange.first );
    trkAccept.first.SetEta( etaTrkRange.first );
    trkAccept.first.SetQuality( qualTrkRange.first );
    trkAccept.first.SetPtErr( ptErrTrkRange.first );
    trkAccept.second.SetNMvtxLayer( nMvtxTrkRange.second );
    trkAccept.second.SetNInttLayer( nInttTrkRange.second );
    trkAccept.second.SetNTpcLayer( nTpcTrkRange.second );
    trkAccept.second.SetPT( ptTrkRange.second );
    trkAccept.second.SetEta( etaTrkRange.second );
    trkAccept.second.SetQuality( qualTrkRange.second );
    trkAccept.second.SetPtErr( ptErrTrkRange.second );

    // set dca bounds if not doing pt-dependent cut
    if (!doSigmaCut) {
      trkAccept.first.SetDcaXY( dcaTrkRangeXY.first );
      trkAccept.first.SetDcaZ( dcaTrkRangeZ.first );
      trkAccept.second.SetDcaXY( dcaTrkRangeXY.second );
      trkAccept.second.SetDcaZ( dcaTrkRangeZ.second );
    }
    return trkAccept;

  }  // end 'GetTrkAccept(bool)'



  // --------------------------------------------------------------------------
  //! Bundle particle flow acceptance cuts into a pair
  // --------------------------------------------------------------------------
  pair<Types::FlowInfo, Types::FlowInfo> GetFlowAccept() {

    // create maximal range
    pair<Types::FlowInfo, Types::FlowInfo> flowAccept = {
      Types::FlowInfo(Const::Init::Minimize),
      Types::FlowInfo(Const::Init::Maximize)
    };

    // set specific bounds
    flowAccept.first.SetPT( ptFlowRange.first );
    flowAccept.first.SetEta( etaFlowRange.first );
    flowAccept.second.SetPT( ptFlowRange.second );
    flowAccept.second.SetEta( etaFlowRange.second );
    return flowAccept;

  }  // end 'GetFlowAccept()'



  // --------------------------------------------------------------------------
  //! Bundle calo tower acceptance cuts into a pair
  // --------------------------------------------------------------------------
  pair<Types::TwrInfo, Types::TwrInfo> GetTowerAccept(
    pair<double, double> eRange,
    pair<double, double> etaRange
  ) {

    // create maximal range
    pair<Types::TwrInfo, Types::TwrInfo> twrAccept = {
      Types::TwrInfo(Const::Init::Minimize),
      Types::TwrInfo(Const::Init::Maximize)
    };

    // set specific bounds
    twrAccept.first.SetEne( eRange.first );
    twrAccept.first.SetEta( etaRange.first );
    twrAccept.second.SetEne( eRange.second );
    twrAccept.second.SetEta( etaRange.second );
    return twrAccept;

  }  // end 'GetTowerAccept(pair<double, double>, pair<double, double>)'



  // --------------------------------------------------------------------------
  //! Bundle calo cluster acceptance cuts into a pair
  // --------------------------------------------------------------------------
  pair<Types::ClustInfo, Types::ClustInfo> GetClustAccept(
    pair<double, double> eRange,
    pair<double, double> etaRange
  ) {

    // create maximal range
    pair<Types::ClustInfo, Types::ClustInfo> clustAccept = {
      Types::ClustInfo(Const::Init::Minimize),
      Types::ClustInfo(Const::Init::Maximize)
    };

    // set specific bounds
    clustAccept.first.SetEne( eRange.first );
    clustAccept.first.SetEta( etaRange.first );
    clustAccept.second.SetEne( eRange.second );
    clustAccept.second.SetEta( etaRange.second );
    return clustAccept;

  }  // end 'GetClustAccept(pair<double, double>, pair<double, double>)'



  // --------------------------------------------------------------------------
  //! Bundle MC particle acceptance cuts into a pair
  // --------------------------------------------------------------------------
  pair<Types::ParInfo, Types::ParInfo> GetParAccept() {

    // create maximal range
    pair<Types::ParInfo, Types::ParInfo> parAccept = {
      Types::ParInfo(Const::Init::Minimize),
      Types::ParInfo(Const::Init::Maximize)
    };

    // set specific bounds
    parAccept.first.SetPT( ptParRange.first );
    parAccept.first.SetEta( etaParRange.first );
    parAccept.second.SetPT( ptParRange.second );
    parAccept.second.SetEta( etaParRange.second );
    return parAccept;

  }  // end 'GetParAccept()'



  // make sigma-dca fit funtions ==============================================

  // --------------------------------------------------------------------------
  //! Construct DCA sigma functions
  // --------------------------------------------------------------------------
  pair<TF1*, TF1*> GetSigmaDcaFunctions() {

    TF1* fSigmaDcaXY = Interfaces::GetSigmaDcaTF1( "fSigmaDcaXY", dcaSigmaParsXY, make_pair(ptTrkRange.first, dcaPtFitMax.first) );
    TF1* fSigmaDcaZ  = Interfaces::GetSigmaDcaTF1( "fSigmaDcaZ",  dcaSigmaParsZ,  make_pair(ptTrkRange.first, dcaPtFitMax.second) );
    return make_pair(fSigmaDcaXY, fSigmaDcaZ);

  }  // end 'GetSigmaDcaFunctions()'



  // set up configuration =====================================================

  // --------------------------------------------------------------------------
  //! Generate configuration for running jet reconstruction in module
  // --------------------------------------------------------------------------
  SCorrelatorJetTreeMakerConfig GetConfig(
    const int verbosity,
    const string outFile,
    const bool doDcaSigmaCut = false
  ) {

    SCorrelatorJetTreeMakerConfig cfg {
      .verbosity       = verbosity,
      .readJetNodes    = false,
      .isDebugOn       = true,
      .isSimulation    = true,
      .isEmbed         = false,
      .moduleName      = "SCorrelatorJetTreeMaker",
      .inRecoNodeName  = "",
      .inTrueNodeName  = "",
      .outFileName     = outFile,
      .recoJetTreeName = "RecoJetTree",
      .trueJetTreeName = "TrueJetTree",
      .rJet            = 0.4,
      .jetAlgo         = "antikt",
      .jetRecomb       = "pt",
      .jetArea         = "active",
      .jetType         = Const::JetType::Tower,
      .doVtxCut        = false,
      .doDcaSigmaCut   = doDcaSigmaCut,
      .requireSiSeeds  = true,
      .useOnlyPrimVtx  = true,
      .subEvtOpt       = Const::SubEvtOpt::Everything,
      .vrAccept        = vrEvtRange,
      .vzAccept        = vzEvtRange,
      .trkAccept       = GetTrkAccept(doDcaSigmaCut),
      .flowAccept      = GetFlowAccept(),
      .eTwrAccept      = GetTowerAccept(eETwrRange, etaETwrRange),
      .hTwrAccept      = GetTowerAccept(eHTwrRange, etaHTwrRange),
      .eClustAccept    = GetClustAccept(eEClustRange, etaEClustRange),
      .hClustAccept    = GetClustAccept(eHClustRange, etaHClustRange),
      .parAccept       = GetParAccept(),
      .nSigCut         = nDcaSigmaTrk,
      .ptFitMax        = dcaPtFitMax,
      .fSigDca         = GetSigmaDcaFunctions()
    };
    return cfg;

  }  // end 'GetConfig(int, string, bool)'



  // --------------------------------------------------------------------------
  //! Generate configuration for reading in a jet node
  // --------------------------------------------------------------------------
  SCorrelatorJetTreeMakerConfig GetConfig(
    const int verbosity,
    const float reso,
    const string inRecoNode,
    const string inTrueNode,
    const string outFile
  ) {

    SCorrelatorJetTreeMakerConfig cfg {
      .verbosity       = verbosity,
      .readJetNodes    = true,
      .isDebugOn       = true,
      .isSimulation    = true,
      .isEmbed         = false,
      .moduleName      = "SCorrelatorJetTreeMaker",
      .inRecoNodeName  = inRecoNode,
      .inTrueNodeName  = inTrueNode,
      .outFileName     = outFile,
      .recoJetTreeName = "RecoJetTree",
      .trueJetTreeName = "TrueJetTree",
      .rJet            = reso,
      .jetAlgo         = "",
      .jetRecomb       = "",
      .jetArea         = "",
      .jetType         = Const::JetType::Tower,
      .doVtxCut        = false,
      .doDcaSigmaCut   = false,
      .requireSiSeeds  = false,
      .useOnlyPrimVtx  = true,
      .subEvtOpt       = Const::SubEvtOpt::Everything,
      .vrAccept        = vrEvtRange,
      .vzAccept        = vzEvtRange,
      .trkAccept       = GetTrkAccept(false),
      .flowAccept      = GetFlowAccept(),
      .eTwrAccept      = GetTowerAccept(eETwrRange, etaETwrRange),
      .hTwrAccept      = GetTowerAccept(eHTwrRange, etaHTwrRange),
      .eClustAccept    = GetClustAccept(eEClustRange, etaEClustRange),
      .hClustAccept    = GetClustAccept(eHClustRange, etaHClustRange),
      .parAccept       = GetParAccept(),
      .nSigCut         = nDcaSigmaTrk,
      .ptFitMax        = dcaPtFitMax,
      .fSigDca         = GetSigmaDcaFunctions()
    };
    return cfg;

  }  // end 'GetConfig(int, float, string, string, string)'

}  // end JetTreeMakerOptions namespace

#endif

// end ------------------------------------------------------------------------
