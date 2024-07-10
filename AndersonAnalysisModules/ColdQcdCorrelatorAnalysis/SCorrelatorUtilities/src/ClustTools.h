// ----------------------------------------------------------------------------
// 'ClustTools.h'
// Derek Anderson
// 03.29.2024
//
// Collection of frequent cluster-related methods utilized
// in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_CLUSTTOOLS_H
#define SCORRELATORUTILITIES_CLUSTTOOLS_H

// c++ utilities
#include <cmath>
// root libraries
#include <Math/Vector3D.h>
#include <Math/Vector4D.h>
// analysis utilities
#include "Constants.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Tools {

    // cluster methods --------------------------------------------------------

    ROOT::Math::XYZVector     GetDisplacement(const ROOT::Math::XYZVector pos, const ROOT::Math::XYZVector vtx);
    ROOT::Math::PxPyPzEVector GetClustMomentum(const double energy, const ROOT::Math::XYZVector pos, const ROOT::Math::XYZVector vtx);

  }  // end Tools namespace
}  // end SColdQcdCorrealtorAnalysis namespace

#endif

// end ------------------------------------------------------------------------

