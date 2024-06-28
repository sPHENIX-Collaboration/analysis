// ----------------------------------------------------------------------------
// 'SCheckCstPairsConfig.h'
// Derek Anderson
// 03.10.2024
//
// SCorrelatorQAMaker plugin to iterate through all pairs of constituents in
// an event and fill tuples/histograms comparing them.
//
// This is similar to the `SCheckTrackPairs` plugin, which specifically looks
// at pairs of tracks off the node tree. This plugin compares constituents
// of any type off the correlator jet tree.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORQAMAKER_SCHECKCSTPAIRSCONFIG_H
#define SCORRELATORQAMAKER_SCHECKCSTPAIRSCONFIG_H

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // SCheckCstPairsConfig definition ------------------------------------------

  struct SCheckCstPairsConfig {

    // i/o options
    string inFileName     {""};
    string inChainName    {""};
    bool   isInChainTruth {true};

    // jet and cst acceptances
    ptJetMin  {0.2};
    etaJetMax {0.7};
    drCstMax  {10.};
    eCstMin   {0.2};
    eCstMax   {100.};

  };  // end SCheckCstPairsConfig

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
