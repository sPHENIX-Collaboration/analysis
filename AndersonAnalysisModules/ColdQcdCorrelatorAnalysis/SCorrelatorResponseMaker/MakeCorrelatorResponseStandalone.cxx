/// ---------------------------------------------------------------------------
/*! \file   MakeCorrelatorResponseStandalone.cxx
 *  \author Derek Anderson
 *  \date   04.30.2023
 *
 *  Use this to run the SCorrelatorResponseMaker
 *  class in standalone mode.
 */
/// ---------------------------------------------------------------------------

#ifndef MAKECORRELATORRESPONSESTANDALONE_CXX
#define MAKECORRELATORRESPONSESTANDALONE_CXX

// standard c includes
#include <string>
#include <cstdlib>
#include <utility>
// module definition
#include <scorrelatorresponsemaker/SCorrelatorResponseMaker.h>
// macro options
#include "EnergyCorrelatorOptions.h"

// load libraries
R__LOAD_LIBRARY(libscorrelatorutilities.so)
R__LOAD_LIBRARY(libscorrelatorresponsemaker.so)

// make common namespaces implicit
using namespace std;



// macro body -----------------------------------------------------------------

void MakeCorrelatorResponseStandalone(const bool doBatch = false, const int verbosity = 0) {

  // get module configuration
  SCorrealtorResponseMakerConfig cfg = CorrelatorResponseMakerOptions::GetConfig(doBatch, verbosity);

  // construct correlator response tree
  SCorrelatorResponseMaker* maker = new SCorrelatorResponseMaker(cfg);
  maker -> Init();
  maker -> Analyze();
  maker -> End();
  return;

}

#endif

// end ------------------------------------------------------------------------
