/// ---------------------------------------------------------------------------
/*! \file   RetowerOptions.h
 *  \author Derek Anderson
 *  \date   08.14.2024
 *
 * Options for the RetowerCEMC module
 */
/// ---------------------------------------------------------------------------

#ifndef RETOWEROPTIONS_H
#define RETOWEROPTIONS_H

// jet libraries
#include <jetbackground/RetowerCEMC.h>

// make common namespaces implicit
using namespace std;



namespace RetowerOptions {

  // RetowerConfig definition =================================================

  // --------------------------------------------------------------------------
  //! Retower Configuration
  // --------------------------------------------------------------------------
  /*! A lightweight POD struct consolidating
   *  options for the RetowerCEMC module.
   */
  struct RetowerConfig {

    // i/o options
    bool   useTowerInfo;
    string nodePrefix;

    // general options
    float fracCut;

  };  // end RetowerConfig


  // options ==================================================================

  // general options
  const float fracCut = 0.5;



  // set up configuration =====================================================

  // --------------------------------------------------------------------------
  //! Generate RetowerCEMC configuration
  // --------------------------------------------------------------------------
  RetowerConfig GetConfig(const bool isTowerInfo = true, const string prefix = "TOWERINFO_CALIB") {

    RetowerConfig cfg {
      .useTowerInfo = isTowerInfo,
      .nodePrefix   = prefix,
      .fracCut      = fracCut
    };
    return cfg;

  }  // end 'GetConfig(bool, string)'



  // initialize emcal retowerer ===============================================

  // --------------------------------------------------------------------------
  //! Instantiate RetowerCEMC module with given configuration
  // --------------------------------------------------------------------------
  RetowerCEMC* CreateRetowerer(RetowerConfig& config) {

    RetowerCEMC* retower = new RetowerCEMC();
    retower -> set_towerinfo(config.useTowerInfo);
    retower -> set_frac_cut(config.fracCut);
    retower -> set_towerNodePrefix(config.nodePrefix);
    return retower;

  }  // end 'CreateRetowerer(RetowerConfig&)'

}  // end RetowerOptions namespace 

#endif

// end ------------------------------------------------------------------------
