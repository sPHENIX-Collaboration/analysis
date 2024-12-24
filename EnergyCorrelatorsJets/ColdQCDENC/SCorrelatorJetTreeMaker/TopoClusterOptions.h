/// ---------------------------------------------------------------------------
/*! \file   TopoClusterOptions.h
 *  \author Derek Anderson
 *  \date   04.12.2024
 *
 * Options for the RawClusterBuilderTopo module
 */
/// ---------------------------------------------------------------------------

#ifndef TOPOCLUSTEROPTIONS_H
#define TOPOCLUSTEROPTIONS_H

// c++ utilities
#include <string>
#include <vector>
#include <utility>
// calo libraries
#include <caloreco/RawClusterBuilderTopo.h>

// make common namespaces implicit
using namespace std;



namespace TopoClusterOptions {

  // possible calo combinations ===============================================

  enum Combo {ECalOnly, BothCalos};



  // TopoClusterConfig definition =============================================

  // --------------------------------------------------------------------------
  //! Topo-Cluster Configuration
  // --------------------------------------------------------------------------
  /*! A lightweight POD struct consolidating
   *  options for the RawClusterBuilderTopo
   *  module.
   */ 
  struct TopoClusterConfig {

    // i/o options
    string moduleName;
    string nodeName;

    // calo options
    bool enableECal;
    bool enableHCal;

    // general options
    int    verbosity;
    bool   doSplit;
    bool   allowCorners;
    double rShower;

    // growth options
    vector<double> noiseLevels;
    vector<double> sigValues;
    vector<double> localMinEne;

  };  // end TopoClusterConfig



  // options ==================================================================

  // calo options
  const pair<bool, bool> enableECal = {true,  true};
  const pair<bool, bool> enableHCal = {false, true};

  // general options
  const bool   doSplit      = true;
  const bool   allowCorners = true;
  const double showerR      = 0.025;

  // growth options
  const vector<double> noiseLevels  = {0.0025, 0.006, 0.03};
  const vector<double> significance = {4.0,    2.0,   0.0};
  const vector<double> localMinE    = {1.0,    2.0,   0.5};



  // set up configuration =====================================================

  // --------------------------------------------------------------------------
  //! Generate RawClusterBuilderTopo configuration
  // --------------------------------------------------------------------------
  TopoClusterConfig GetConfig(const int combo, const int verbosity, const string module, const string node) {

    TopoClusterConfig cfg {
      .moduleName   = module,
      .nodeName     = node,
      .enableECal   = (combo == Combo::ECalOnly) ? enableECal.first : enableECal.second,
      .enableHCal   = (combo == Combo::ECalOnly) ? enableHCal.first : enableHCal.second,
      .verbosity    = verbosity,
      .doSplit      = doSplit,
      .allowCorners = allowCorners,
      .rShower      = showerR,
      .noiseLevels  = noiseLevels,
      .sigValues    = significance,
      .localMinEne  = localMinE
    };
    return cfg;

  }  // end 'GetConfig(int)'



  // initialize topocluster builder ===========================================

  // --------------------------------------------------------------------------
  //! Instantiate RawClusterBuilderTopo module with given configuration
  // --------------------------------------------------------------------------
  RawClusterBuilderTopo* CreateBuilder(TopoClusterConfig& config) {

    RawClusterBuilderTopo* builder = new RawClusterBuilderTopo(config.moduleName.data());
    builder -> Verbosity(config.verbosity);
    builder -> set_nodename(config.nodeName.data());
    builder -> set_enable_EMCal(config.enableECal);
    builder -> set_enable_HCal(config.enableHCal);
    builder -> set_do_split(config.doSplit);
    builder -> allow_corner_neighbor(config.allowCorners);
    builder -> set_R_shower(config.rShower);
    builder -> set_noise(
      config.noiseLevels.at(0),
      config.noiseLevels.at(1),
      config.noiseLevels.at(2)
    );
    builder -> set_significance(
      config.sigValues.at(0),
      config.sigValues.at(1),
      config.sigValues.at(2)
    );
    builder -> set_minE_local_max(
      config.localMinEne.at(0),
      config.localMinEne.at(1),
      config.localMinEne.at(2)
    );
    return builder;

  }  // end 'CreateBuilder(TopoClusterConfig&)'

}  // end TopoClusterOptions namespace

#endif

// end ------------------------------------------------------------------------
