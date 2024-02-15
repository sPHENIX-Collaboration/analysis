// ----------------------------------------------------------------------------
// 'RunMatcherComparator.C'
// Derek Anderson
// 01.30.2024
//
// root macro to run the STrackMatcherComparator module.
// ----------------------------------------------------------------------------

// analysis specific utilities
#include "MatcherComparatorOptions.h"
#include "/sphenix/user/danderson/install/include/strackmatchercomparator/STrackMatcherComparator.h"
#include "/sphenix/user/danderson/install/include/strackmatchercomparator/STrackMatcherComparatorConfig.h"
#include "/sphenix/user/danderson/install/include/strackmatchercomparator/STrackMatcherComparatorHistDef.h"

// load libraries
R__LOAD_LIBRARY(/sphenix/user/danderson/install/lib/libstrackmatchercomparator.so)



// macro body -----------------------------------------------------------------

void RunMatcherComparator() {

  // get options
  STrackMatcherComparatorConfig  config = MatcherComparatorOptions::GetConfig();
  STrackMatcherComparatorHistDef histos = MatcherComparatorOptions::GetHistDef();

  // run module
  STrackMatcherComparator* comparator = new STrackMatcherComparator(config);
  comparator -> SetHistDef(histos);
  comparator -> Init();
  comparator -> Analyze();
  comparator -> End();
  return;

}

// end ------------------------------------------------------------------------
