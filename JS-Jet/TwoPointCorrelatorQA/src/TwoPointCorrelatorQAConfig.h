// ----------------------------------------------------------------------------
// 'TwoPointCorrelatorQA.h'
// Derek Anderson
// 03.25.2024
//
// A Fun4All QA module to produce an EEC spectra
// for jets constructed from an arbitrary source.
// ----------------------------------------------------------------------------

#ifndef TWOPOINTCORRELATORQACONFIG_H
#define TWOPOINTCORRELATORQACONFIG_H



// TwoPointCorrelatorQAConfig definition --------------------------------------

struct TwoPointCorrelatorQAConfig {

  bool doDebug = false;

  // vector members
  std::vector<std::pair<float, float>> ptJetBins;


};

#endif

// end ------------------------------------------------------------------------

