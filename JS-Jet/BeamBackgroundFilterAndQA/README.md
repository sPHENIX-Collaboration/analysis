# Beam Background Filter and QA

### Authors:
Hanpu Jiang, Derek Anderson, Noah Applegate

### Description:
Development repo for a F4A module to filter out events with significant beam
background (e.g. the so-called "streak events") and produce a few relevant 
QA histograms. The module is designed in such a way that additional filters
can be added with relatively minimal overhead.

There are currently two filters:

  - **`The Null Filter,`** which does nothing, but provides a template
    for other other filters; and
  - **`The Streak Sideband Filter,`** which checks for cases in the
    OHCal where you have a continuous row of towers along eta above
    some threshold in energy.

Note that others are expected to be added in the future, and that these
filters can be used in other modules. For example, the streak sideband
filter could be deployed in an analysis module like so:

```
// in your .h file
#include <beambackgroundfilterandqa/StreakSidebandFilter.h>

class MyModule : SubsysReco {
  //... other stuff ...//

  public:
    void InitFilter(const StreakSidebandFilter::Config& cfg);

  private:
    StreakSidebandFilter m_filter;

}

// in your .cc file

//... other stuff ...//

void MyModule::InitFilter(const StreakSidebandFilter::Config& cfg) {
  m_filter = StreakSidebandFilter(cfg, "MyFilter");
  m_filter.BuildHistograms();
}

void MyModule::process_event(PHCompositeNode* topNode) {
  foundBkgd = m_filter.ApplyFilter(topNode);
  if (foundBkgd) {
    //... do stuff ...//
  }
}
```

The user has the option to either throw away or keep events in which
a filter has identified beam background. In either case, the results
of each filter (and the overall result) are stored as integer flags in
`recoConsts` with the value of 0 for NO beam background and 1 FOR
beam background. The module `TestPHFlags` illustrates how to
retrieve these flags in downstream modules.


Lastly, the overall code structure is:

  - **`BaseBeamBackgroundFilter.h:`** A base class for all filters to
    be applied, consolidates common functionality across filters. New
    filters must inherit from this.
  - **`{Null,StreakSideband}Filter.{cc,h}`:** The actual filters to
    be applied.
  - **`BeamBackgroundFilterAndQA.{cc,h}`:** The actual F4A module
    which organizes and runs all of the specified filters.
  - **`BeamBackgroundFilterAndQADefs.h`:** A namespace to collect
    a variety of useful methods used throughout the module and its
    componenets.


