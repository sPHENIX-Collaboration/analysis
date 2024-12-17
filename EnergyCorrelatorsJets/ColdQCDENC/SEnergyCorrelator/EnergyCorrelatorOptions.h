/// ---------------------------------------------------------------------------
/*! \file    EnergyCorrelatorOptions.h
 *  \authors Derek Anderson, Alex Clarke
 *  \date    02.08.2024
 *
 *  Options for the SEnergyCorrelator module.
 */
/// ---------------------------------------------------------------------------

#ifndef ENERGYCORRELATOROPTIONS_H
#define ENERGYCORRELATOROPTIONS_H 

// c++ utilities
#include <string>
#include <utility>
#include <optional>
// analysis utilities
#include <scorrelatorutilities/Types.h>
#include <scorrelatorutilities/Constants.h>
// module configuration
#include <senergycorrelator/SEnergyCorrelatorConfig.h>

// make common namespacs implicit
using namespace std;
using namespace SColdQcdCorrelatorAnalysis;



namespace EnergyCorrelatorOptions {

  // calculation options ======================================================

  // general correlator parameters
  const vector<uint32_t>     nPoints    = {2};
  const pair<double, double> binRangeDr = {1e-5, 1.};
  const uint64_t             nBinsDr    = 75;

  // basic options
  const int  verbosity = 0;
  const int  subEvtOpt = Const::SubEvtOpt::Everything;
  const bool isEmbed   = false;
  const bool doCstCuts = true;
  const bool doDebug   = false;
  const bool doBatch   = false;

  // calculation options
  const int  momOption     = 0;
  const int  normOption    = 0;
  const bool doLocalCalc   = false;
  const bool doGlobalCalc  = true;
  const bool doManualCalc  = false;
  const bool doPackageCalc = false;
  const bool doThreePoint  = false;

  // smearing options
  const bool   doJetSmear      = false;
  const bool   doCstSmear      = false;
  const bool   doCstPtSmear    = false;
  const bool   doCstThetaSmear = false;
  const bool   doCstPhiSmear   = false;
  const float  ptJetSmear      = 0.5;
  const float  ptCstSmear      = 0.1;
  const float  thCstSmear      = 0.01;

  // efficiency options
  const bool   doCstEff    = false;
  const float  effValue    = 0.9;
  const float  effSmooth   = 4.0;
  const string effFunction = "[0]*(1.0-TMath::Exp(-1.0*[1]*x))";

  // event ht bins
  const vector<pair<double, double>> htEvtBins = {
    {0., 200.}
  };

  // jet pT bins
  const vector<pair<double, double>> ptJetBins = {
    {0.,  5.},
    {5.,  10.},
    {10., 15.},
    {15., 20.},
    {20., 30.},
    {30., 50.},
    {50., 100.}
  };

  // rl bins for for manual E3C calc
  const vector<pair<double, double>> rlBins = {
    {0.0,     0.22664},
    {0.22664, 0.26666},
    {0.26666, 1.0}
  };

  // acceptance cuts ==========================================================

  // jet acceptance
  const pair<float, float> eneJetRange = {0.2,  100.};
  const pair<float, float> etaJetRange = {-0.7, 0.7};

  // cst acceptance
  const pair<float, float> drCstRange  = {0.,  100.};
  const pair<float, float> eneCstRange = {0.1, 100.};



  // create constituent efficiency function ===================================

  TF1* MakeEffFunction() {

    TF1* eff = new TF1(
      "fCstEfficiency",
      effFunction.data(),
      eneCstRange.first,
      eneCstRange.second
    );
    eff -> SetParameter(0, effValue);
    eff -> SetParameter(1, effSmooth);
    return eff;

  }  // end 'MakeEffFunction()'

 
 
  // bundle acceptances into pairs ============================================

  // --------------------------------------------------------------------------
  //! Bundle jet acceptance cuts into a pair
  // --------------------------------------------------------------------------
  pair<Types::JetInfo, Types::JetInfo> GetJetAccept() {

    // create maximal range
    pair<Types::JetInfo, Types::JetInfo> jetAccept = {
      Types::JetInfo(Const::Init::Minimize),
      Types::JetInfo(Const::Init::Maximize)
    };

    // set specific bounds
    jetAccept.first.SetPT( ptJetBins.front().first );
    jetAccept.first.SetEne( eneJetRange.first );
    jetAccept.first.SetEta( etaJetRange.first );
    jetAccept.second.SetPT( ptJetBins.back().second );
    jetAccept.second.SetEne( eneJetRange.second );
    jetAccept.second.SetEta( etaJetRange.second );
    return jetAccept;

  }  // end 'GetJetAccept()'



  // --------------------------------------------------------------------------
  //! Bundle constituent acceptance cuts into a pair
  // --------------------------------------------------------------------------
  pair<Types::CstInfo, Types::CstInfo> GetCstAccept() {

   // create maxmimal range
    pair<Types::CstInfo, Types::CstInfo> cstAccept = {
      Types::CstInfo(Const::Init::Minimize),
      Types::CstInfo(Const::Init::Maximize)
    };

    // set specific bounds
    cstAccept.first.SetDR( drCstRange.first );
    cstAccept.first.SetEne( eneCstRange.first );
    cstAccept.second.SetDR( drCstRange.second );
    cstAccept.second.SetEne( eneCstRange.second );
    return cstAccept;

  }  // end 'GetCstAccept()'



  // set up configurations ====================================================

  // --------------------------------------------------------------------------
  //! Generate configuration for reconstructed jets
  // --------------------------------------------------------------------------
  SEnergyCorrelatorConfig GetRecoConfig(
    const vector<string> cfg_inFiles,
    const string cfg_outFile,
    const bool cfg_doBatch = doBatch,
    const int cfg_verbosity = verbosity
  ) {

    SEnergyCorrelatorConfig cfg_reco {
      .verbosity     = cfg_verbosity,
      .isDebugOn     = doDebug,
      .isBatchOn     = cfg_doBatch,
      .isEmbed       = isEmbed,
      .isInTreeTruth = false,
      .moduleName    = "SRecoEnergyCorrelator",
      .inTreeName    = "RecoJetTree",
      .outFileName   = cfg_outFile,
      .applyCstCuts  = doCstCuts,
      .subEvtOpt     = subEvtOpt,
      .inFileNames   = cfg_inFiles,
      .nPoints       = nPoints,
      .nBinsDr       = nBinsDr,
      .drBinRange    = binRangeDr,
      .ptJetBins     = ptJetBins,
      .htEvtBins     = htEvtBins,
      .doLocalCalc   = doLocalCalc,
      .doGlobalCalc  = doGlobalCalc,
      .doManualCalc  = doManualCalc,
      .doPackageCalc = doPackageCalc,
      .doThreePoint  = doThreePoint,
      .rlBins        = rlBins,
      .momOption     = momOption,
      .normOption    = normOption,
      .jetAccept     = GetJetAccept(),
      .cstAccept     = GetCstAccept()
    };
    return cfg_reco;

  }  // end 'GetRecoConfig(vector<string>, string, int)'



  // --------------------------------------------------------------------------
  //! Generate configuration for truth jets
  // --------------------------------------------------------------------------
  SEnergyCorrelatorConfig GetTruthConfig(
    const vector<string> cfg_inFiles,
    const string cfg_outFile,
    const bool cfg_doBatch = doBatch,
    const int cfg_verbosity = verbosity
  ) {

    SEnergyCorrelatorConfig cfg_true {
      .verbosity       = cfg_verbosity,
      .isDebugOn       = doDebug,
      .isBatchOn       = cfg_doBatch,
      .isEmbed         = isEmbed,
      .isInTreeTruth   = true,
      .moduleName      = "TrueEnergyCorrelator",
      .inTreeName      = "TruthJetTree",
      .outFileName     = cfg_outFile,
      .applyCstCuts    = doCstCuts,
      .subEvtOpt       = subEvtOpt,
      .inFileNames     = cfg_inFiles,
      .nPoints         = nPoints,
      .nBinsDr         = nBinsDr,
      .drBinRange      = binRangeDr,
      .ptJetBins       = ptJetBins,
      .htEvtBins       = htEvtBins,
      .doLocalCalc     = doLocalCalc,
      .doGlobalCalc    = doGlobalCalc,
      .doManualCalc    = doManualCalc,
      .doPackageCalc   = doPackageCalc,
      .doThreePoint    = doThreePoint,
      .rlBins          = rlBins,
      .momOption       = momOption,
      .normOption      = normOption,
      .jetAccept       = GetJetAccept(),
      .cstAccept       = GetCstAccept(),
      .doJetSmear      = doJetSmear,
      .doCstSmear      = doCstSmear,
      .doCstPtSmear    = doCstPtSmear,
      .doCstThetaSmear = doCstThetaSmear,
      .doCstPhiSmear   = doCstPhiSmear,
      .doCstEff        = doCstEff,
      .ptJetSmear      = ptJetSmear,
      .ptCstSmear      = ptCstSmear,
      .thCstSmear      = thCstSmear,
      .funcEff         = MakeEffFunction()
    };
    return cfg_true;

  }  // end 'GetConfig(vector<string>, string, int)'

}  // end EnergyCorrelatorOptions namespace

#endif

// end -----------------------------------------------------------------
