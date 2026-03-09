/// ---------------------------------------------------------------------------
/*! \file   MakeCorrelatorResponseTree.cxx
 *  \author Derek Anderson
 *  \date   04.30.2023
 *
 *  Use this to run the SCorrelatorResponseMaker
 *  class in standalone mode.
 */
/// ---------------------------------------------------------------------------

#ifndef MAKECORRELATORRESPONSETREE_CXX
#define MAKECORRELATORRESPONSETREE_CXX

// standard c includes
#include <string>
#include <cstdlib>
#include <utility>
// module definition
#include <scorrelatorresponsemaker/SCorrelatorResponseMaker.h>
// macro options
#include "CorrelatorResponseMakerOptions.h"

// load libraries
R__LOAD_LIBRARY(libscorrelatorutilities.so)
R__LOAD_LIBRARY(libscorrelatorresponsemaker.so)

// make common namespaces implicit
using namespace std;



// macro body -----------------------------------------------------------------

void MakeCorrelatorResponseTree(const bool doBatch = false, const int verbosity = 0) {

  // get module configuration
  SCorrelatorResponseMakerConfig cfg = CorrelatorResponseMakerOptions::GetConfig(doBatch, verbosity);

  // construct correlator response tree
  SCorrelatorResponseMaker* maker = new SCorrelatorResponseMaker(cfg);
  maker -> Init();
  maker -> Analyze();
  maker -> End();
  return;

}

#endif

// end ------------------------------------------------------------------------
