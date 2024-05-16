// ----------------------------------------------------------------------------
// 'SMakeTrackQATupleConfig.h'
// Derek Anderson
// 03.10.2024
//
// SCorrelatorQAMaker plugin to produce QA tuples
// for tracks.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORQAMAKER_SMAKETRACKQATUPLECONFIG_H
#define SCORRELATORQAMAKER_SMAKETRACKQATUPLECONFIG_H

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // SMakeTrackQATupleConfig definition ----------------------------------------

  struct SMakeTrackQATupleConfig {

    bool isEmbed;
    bool doDcaSigCut;
    bool requireSiSeed;
    bool useOnlyPrimVtx;

    // track acceptance
    pair<Types::TrkInfo, Types::TrkInfo> trkAccept;

    // for pt-dependent sigma cut
    pair<float, float> nSigCut;
    pair<float, float> ptFitMax;
    pair<TF1*,  TF1*>  fSigDca;

  };  // end SMakeTrackQATupleConfig

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end -----------------------------------------------------------------------
