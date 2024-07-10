// ----------------------------------------------------------------------------
// 'CorrelatorQAMakerOptions.h'
// Derek Anderson
// 01.26.2024
//
// Options for the SCorrelatorQAMaker module(s).
// ----------------------------------------------------------------------------

#ifndef CORRELATORQAMAKEROPTIONS_H
#define CORRELATORQAMAKEROPTIONS_H

// c++ utilities
#include <string>
#include <utility>
// analysis utilities
#include <scorrelatorutilities/Types.h>
#include <scorrelatorutilities/Constants.h>
// plugin configurations
#include <scorrelatorqamaker/SMakeClustQATreeConfig.h>
#include <scorrelatorqamaker/SCheckTrackPairsConfig.h>
#include <scorrelatorqamaker/SMakeTrackQATupleConfig.h>
#include <scorrelatorqamaker/SReadLambdaJetTreeConfig.h>

// make common namespaces implicit
using namespace std;
using namespace SColdQcdCorrelatorAnalysis;



namespace CorrelatorQAMakerOptions {

  // track, cluster, etc. acceptances -----------------------------------------

  // track acceptance
  const pair<int, int>       nMvtxLayerTrkRange = {2, 100};
  const pair<int, int>       nInttLayerTrkRange = {1, 100};
  const pair<int, int>       nTpcLayerTrkRange  = {24, 100};
  const pair<double, double> ptTrkRange         = {0.2, 100.};
  const pair<double, double> etaTrkRange        = {-1.1, 1.1};
  const pair<double, double> dcaXYTrkRange      = {-5., 5.};
  const pair<double, double> dcaZTrkRange       = {-5., 5.};
  const pair<double, double> ptErrTrkRange      = {0., 0.5};
  const pair<double, double> qualityTrkRange    = {0., 10.};

  // cluster acceptance
  const pair<double, double> eneClustRange = {0.1, 100.};
  const pair<double, double> etaClustRange = {-1.1, 1.1};



  // set up acceptances -------------------------------------------------------

  // bundle track acceptance into a pair
  pair<Types::TrkInfo, Types::TrkInfo> GetTrackAccept() {

    // create maximal range
    pair<Types::TrkInfo, Types::TrkInfo> trkAccept = {
      Types::TrkInfo(Const::Init::Minimize),
      Types::TrkInfo(Const::Init::Maximize)
    };

    // set specific bounds
    trkAccept.first.SetNMvtxLayer( nMvtxLayerTrkRange.first );
    trkAccept.first.SetNInttLayer( nInttLayerTrkRange.first );
    trkAccept.first.SetNTpcLayer( nTpcLayerTrkRange.first );
    trkAccept.first.SetPT( ptTrkRange.first );
    trkAccept.first.SetEta( etaTrkRange.first );
    trkAccept.first.SetDcaXY( dcaXYTrkRange.first );
    trkAccept.first.SetDcaZ( dcaZTrkRange.first );
    trkAccept.first.SetPtErr( ptErrTrkRange.first );
    trkAccept.first.SetQuality( qualityTrkRange.first );
    trkAccept.second.SetNMvtxLayer( nMvtxLayerTrkRange.second );
    trkAccept.second.SetNInttLayer( nInttLayerTrkRange.second );
    trkAccept.second.SetNTpcLayer( nTpcLayerTrkRange.second );
    trkAccept.second.SetPT( ptTrkRange.second );
    trkAccept.second.SetEta( etaTrkRange.second );
    trkAccept.second.SetDcaXY( dcaXYTrkRange.second );
    trkAccept.second.SetDcaZ( dcaZTrkRange.second );
    trkAccept.second.SetPtErr( ptErrTrkRange.second );
    trkAccept.second.SetQuality( qualityTrkRange.second );
    return trkAccept;

  }  // end 'GetTrackAcceptance()'



  // bundle cluster acceptance into a pair
  pair<Types::ClustInfo, Types::ClustInfo> GetClustAccept() {

    // create maximal range
    pair<Types::ClustInfo, Types::ClustInfo> clustAccept = {
      Types::ClustInfo(Const::Init::Minimize),
      Types::ClustInfo(Const::Init::Maximize)
    };

    // set specific bounds
    clustAccept.first.SetEne( eneClustRange.first );
    clustAccept.first.SetEta( etaClustRange.first );
    clustAccept.second.SetEne( eneClustRange.second );
    clustAccept.second.SetEta( etaClustRange.second );
    return clustAccept;

  }  // end 'GetClustAccept()'



  // set up configurations ----------------------------------------------------

  // SCheckTrackPairs configuration
  SCheckTrackPairsConfig GetCheckTrackPairsConfig() {

    SCheckTrackPairsConfig config = {
      .doDcaSigCut    = false,
      .requireSiSeed  = true,
      .useOnlyPrimVtx = true,
      .trkAccept      = GetTrackAccept()
    };
    return config;

  }  // end 'GetCheckTrackPairsConfig()'



  // SMakeTrackQATuple configuration
  SMakeTrackQATupleConfig GetMakeTrackQATupleConfig() {

    SMakeTrackQATupleConfig config = {
      .isEmbed        = true,
      .doDcaSigCut    = false,
      .requireSiSeed  = true,
      .useOnlyPrimVtx = true,
      .trkAccept      = GetTrackAccept()
    };
    return config;

  }  // end 'GetMakeTrackQATupleConfig()'



  // SMakeClustQATree configuration
  SMakeClustQATreeConfig GetMakeClustQATreeConfig() {

    SMakeClustQATreeConfig config = {
      .isEmbed     = true,
      .clustAccept = GetClustAccept()
    };
    return config;

  }  // end 'GetMakeClustQATreeConfig()'



  // SReadLambdaJetTree configuration
  SReadLambdaJetTreeConfig GetReadLambdaJetTreeConfig(const string input) {

    SReadLambdaJetTreeConfig config = {
      .inFileName = input,
      .inTreeName = "LambdaJetTree",
      .ptJetMin   = 25.,
      .ptLamMin   = 0.2,
      .etaJetMax  = 0.7,
      .etaLamMax  = 1.1,
      .zLeadMin   = 0.5
    };
    return config;

  }  // end 'GetReadLambdaJetTreeConfig()'

}  // end CorrelatorQAMakerOptions namespace

#endif

// end ------------------------------------------------------------------------
