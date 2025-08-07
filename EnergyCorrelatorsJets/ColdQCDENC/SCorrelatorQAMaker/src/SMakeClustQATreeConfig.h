// ----------------------------------------------------------------------------
// 'SMakeClustQATreeConfig.h'
// Derek Anderson
// 03.10.2024
//
// SCorrelatorQAMaker plugin to produce the QA tree
// for calorimeter clusters.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORQAMAKER_SMAKECLUSTQATREECONFIG_H
#define SCORRELATORQAMAKER_SMAKECLUSTQATREECONFIG_H

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // SMakeClustQATreeConfig definition ----------------------------------------

  struct SMakeClustQATreeConfig {

    bool isEmbed;

    // cluster acceptance
    pair<Types::ClustInfo, Types::ClustInfo> clustAccept;

  };  // end SMakeClustQATreeConfig

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
