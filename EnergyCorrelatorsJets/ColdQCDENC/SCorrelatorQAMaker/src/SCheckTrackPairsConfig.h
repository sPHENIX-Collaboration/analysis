// ----------------------------------------------------------------------------
// 'SCheckTrackPairsConfig.h'
// Derek Anderson
// 02.21.2024
//
// SCorrelatorQAMaker plugin to iterate through
// all pairs of tracks in an event and fill
// tuples/histograms comparing them.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORQAMAKER_SCHECKTRACKPAIRSCONFIG_H
#define SCORRELATORQAMAKER_SCHECKTRACKPAIRSCONFIG_H

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // SCheckTrackPairsConfig definition ----------------------------------------

  struct SCheckTrackPairsConfig {

    bool doDcaSigCut     {false};
    bool requireSiSeed   {true};
    bool useOnlyPrimVtx  {true};

    // track acceptance
    pair<Types::TrkInfo, Types::TrkInfo> trkAccept;

    // for pt-dependent sigma cut
    pair<float, float> nSigCut;
    pair<float, float> ptFitMax;
    pair<TF1*,  TF1*>  fSigDca;

  };  // end SCheckTrackPairsConfig

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
