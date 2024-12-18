/// ---------------------------------------------------------------------------
/*! \file   TwrTools.h
 *  \author Derek Anderson
 *  \date   08.04.2024
 *
 *  Collection of frequent tower-related methods utilized in
 *  the sPHENIX Cold QCD Energy-Energy Correlator analysis.
 */
/// ---------------------------------------------------------------------------

#ifndef SCORRELATORUTILITIES_TWRTOOLS_H
#define SCORRELATORUTILITIES_TWRTOOLS_H

// c++ utilities
#include <cmath>
#include <utility>
// root libraries
#include <Math/Vector3D.h>
#include <Math/Vector4D.h>
// phool libraries
#include <phool/PHCompositeNode.h>
// calobase libraries
#include <calobase/TowerInfo.h>
#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/TowerInfoContainer.h>
// analysis utilities
#include "VtxTools.h"
#include "Constants.h"
#include "Interfaces.h"

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {
  namespace Tools {

    // tower methods ----------------------------------------------------------

    int                         GetTowerStatus(const TowerInfo* tower);
    int                         GetCaloIDFromRawTower(RawTower* tower);
    int                         GetRawTowerKey(const int idGeo, const tuple<int, int, int> indices);
    tuple<int, int, int>        GetTowerIndices(const int channel, const int subsys, PHCompositeNode* topNode);
    ROOT::Math::XYZVector       GetTowerPositionXYZ(const int rawKey, const int subsys, PHCompositeNode* topNode);
    ROOT::Math::RhoEtaPhiVector GetTowerPositionRhoEtaPhi(const int rawKey, const int subsys, const float zVtx, PHCompositeNode* topNode);
    ROOT::Math::PxPyPzEVector   GetTowerMomentum(const double energy, const ROOT::Math::RhoEtaPhiVector pos);

  }  // end Tools namespace
}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
