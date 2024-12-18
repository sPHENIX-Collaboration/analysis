/// ---------------------------------------------------------------------------
/*! \file   ClustTools.cc
 *  \author Derek Anderson
 *  \date   03.29.2024
 *
 *  Collection of frequent cluster-related methods utilized
 *  in the sPHENIX Cold QCD Energy-Energy Correlator analysis.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_CLUSTTOOLS_CC

// namespace definition
#include "ClustTools.h"

// make common namespaces implicit
using namespace std;



// cluster methods ============================================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Get 4-momentum for a cluster
  // --------------------------------------------------------------------------
  ROOT::Math::PxPyPzEVector Tools::GetClustMomentum(const double energy, const ROOT::Math::XYZVector pos, const ROOT::Math::XYZVector vtx) {

    // get displacement
    ROOT::Math::XYZVector displace = GetDisplacement(pos, vtx);

    // get magnitdue of 3-momentum
    const double magnitude = sqrt((energy * energy) - (Const::MassPion() * Const::MassPion()));

    // now get components and return
    ROOT::Math::XYZVector momentum = displace.Unit() * magnitude;
    return ROOT::Math::PxPyPzEVector(momentum.X(), momentum.Y(), momentum.Z(), energy);

  }  // end 'GetClustMomentum(double, ROOT::Math::XYZVector)'

}  // end SColdQcdCorrelatorAnalysis namespace


// end ------------------------------------------------------------------------
