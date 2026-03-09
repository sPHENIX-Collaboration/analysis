/// ---------------------------------------------------------------------------
/*! \file   SCorrelatorJetTreeMakerLinkDef.h
 *  \author Derek Anderson
 *  \date   12.21.2022
 *
 *  A module to produce a tree of jets for the sPHENIX
 *  Cold QCD Energy-Energy Correlator analysis. Initially
 *  derived from code by Antonio Silva.
 */
/// ---------------------------------------------------------------------------

// c++ utilities
#include <vector>
#include <utility>
// analysis utilities
#include <scorrelatorutilities/Types.h>
// analysis definitions
#include "SCorrelatorJetTreeMaker.h"
#include "SCorrelatorJetTreeMakerOutput.h"



#ifdef __CINT__

#pragma link C++ class SCorrelatorJetTreeMaker-!;
#pragma link C++ class SCorrelatorJetTreeMakerRecoOutput+;
#pragma link C++ class SCorrelatorJetTreeMakerTruthOutput+;

#endif

// end ------------------------------------------------------------------------
