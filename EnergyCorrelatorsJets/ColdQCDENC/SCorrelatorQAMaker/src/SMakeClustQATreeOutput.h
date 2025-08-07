// ----------------------------------------------------------------------------
// 'SMakeClustQATreeOutput.h'
// Derek Anderson
// 03.10.2024
//
// SCorrelatorQAMaker plugin to produce the QA tree
// for calorimeter clusters.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORQAMAKER_SMAKECLUSTQATREEOUTPUT_H
#define SCORRELATORQAMAKER_SMAKECLUSTQATREEOUTPUT_H

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // SMakeClustQATreeOutput definition ----------------------------------------

  struct SMakeClustQATreeOutput {

    // event level info
    Types::RecoInfo recInfo;
    Types::GenInfo  genInfo;

    // calo info
    vector<Types::ClustInfo> emCalInfo;
    vector<Types::ClustInfo> ihCalInfo;
    vector<Types::ClustInfo> ohCalInfo;

    void Reset() {
      recInfo.Reset();
      genInfo.Reset();
      emCalInfo.clear();
      ihCalInfo.clear();
      ohCalInfo.clear();
      return;
    }

  };  // end SMakeClustQATreeOutput

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
