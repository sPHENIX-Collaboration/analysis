// ----------------------------------------------------------------------------
// 'STrackMatcherComparatorHistContent.h'
// Derek Anderson
// 02.01.2024
//
// Struct to hold histogram info for 'STrackMatcherComparator' module.
// ----------------------------------------------------------------------------

#ifndef STRACKMATCHERCOMPARATORHISTCONTENT_H
#define STRACKMATCHERCOMPARATORHISTCONTENT_H

// c++ utilities
#include <limits>

// make common namespaces implicit
using namespace std;



// STrackMatcherComparatorHistContent definition ----------------------------------

struct STrackMatcherComparatorHistContent {

  double nTot    = numeric_limits<double>::min();
  double nIntt   = numeric_limits<double>::min();
  double nMvtx   = numeric_limits<double>::min();
  double nTpc    = numeric_limits<double>::min();
  double rTot    = numeric_limits<double>::min();
  double rIntt   = numeric_limits<double>::min();
  double rMvtx   = numeric_limits<double>::min();
  double rTpc    = numeric_limits<double>::min();
  double phi     = numeric_limits<double>::min();
  double eta     = numeric_limits<double>::min();
  double pt      = numeric_limits<double>::min();
  double ptFrac  = numeric_limits<double>::min();
  double quality = numeric_limits<double>::min();
  double ptErr   = numeric_limits<double>::min();
  double etaErr  = numeric_limits<double>::min();
  double phiErr  = numeric_limits<double>::min();
  double ptRes   = numeric_limits<double>::min();
  double etaRes  = numeric_limits<double>::min();
  double phiRes  = numeric_limits<double>::min();

};

#endif

// end ------------------------------------------------------------------------

