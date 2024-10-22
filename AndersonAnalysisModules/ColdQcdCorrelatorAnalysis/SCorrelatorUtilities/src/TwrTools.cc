/// ---------------------------------------------------------------------------
/*! \file   TwrTools.cc
 *  \author Derek Anderson
 *  \date   08.04.2024
 *
 *  Collection of frequent tower-related methods utilized in
 *  the sPHENIX Cold QCD Energy-Energy Correlator analysis.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_TWRTOOLS_CC

// namespace definition
#include "TwrTools.h"

// make common namespaces implicit
using namespace std;



// tower methods ==============================================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Get tower status
  // --------------------------------------------------------------------------
  /*! NOTE: only applies to TowerInfo objects.
   */
  int Tools::GetTowerStatus(const TowerInfo* tower) {

    int status = Const::TowerStatus::Unknown;
    if (tower -> get_isHot()) {
      status = Const::TowerStatus::Hot;
    } else if (tower -> get_isBadTime()) {
      status = Const::TowerStatus::BadTime;
    } else if (tower -> get_isBadChi2()) {
      status = Const::TowerStatus::BadChi;
    } else if (tower -> get_isNotInstr()) {
      status = Const::TowerStatus::NotInstr;
    } else if (tower -> get_isNoCalib()) {
      status = Const::TowerStatus::NoCalib;
    } else {
      status = Const::TowerStatus::Good;
    }
    return status;

  }  // end 'GetTowerStatus(TowerInfo*)' 



  // --------------------------------------------------------------------------
  //! Get Calorimeter ID from a RawTower
  // --------------------------------------------------------------------------
  int Tools::GetCaloIDFromRawTower(RawTower* tower) {

    auto rawKey = tower -> get_id();
    return RawTowerDefs::decode_caloid(rawKey);

  }  // end 'GetCaloIDFromRawTower(RawTower*)'



  // --------------------------------------------------------------------------
  //! Get raw tower key based on tower indices
  // --------------------------------------------------------------------------
  int Tools::GetRawTowerKey(const int idGeo, const tuple<int, int, int> indices) {

    const int key = (int) RawTowerDefs::encode_towerid(
      (RawTowerDefs::CalorimeterId) idGeo,
      get<1>(indices),
      get<2>(indices)
    );
    return key;

  }  // end 'GetRawTowerKey(int, tuple<int, int, int>)'



  // --------------------------------------------------------------------------
  //! Get key and eta-phi indices for a TowerInfo object
  // --------------------------------------------------------------------------
  tuple<int, int, int> Tools::GetTowerIndices(
    const int channel,
    const int subsys,
    PHCompositeNode* topNode
  ) {

    // grab container & key
    TowerInfoContainer* towers = Interfaces::GetTowerInfoStore(
      topNode,
      Const::MapIndexOntoTowerInfo()[ subsys ]
    );
    const uint32_t key = towers -> encode_key(channel);

    // grab indices
    const int iEta = towers -> getTowerEtaBin(key);
    const int iPhi = towers -> getTowerPhiBin(key);
    return make_tuple((int) key, iEta, iPhi);

  }  // end 'GetTowerIndices(int, int, PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Get tower coordinates in (x, y, z)
  // --------------------------------------------------------------------------
  ROOT::Math::XYZVector Tools::GetTowerPositionXYZ(const int rawKey, const int subsys, PHCompositeNode* topNode) {

    // get corresponding geometry
    RawTowerGeom* geometry = Interfaces::GetTowerGeometry(topNode, subsys, rawKey);

    // grab (x, y, z) coordinates
    ROOT::Math::XYZVector position(
      geometry -> get_center_x(),
      geometry -> get_center_y(),
      geometry -> get_center_z()
    );
    return position;

  }  // end 'GetTowerPositionXYZ(int, int, PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Get tower coordinates in (r, eta, phi)
  // --------------------------------------------------------------------------
  ROOT::Math::RhoEtaPhiVector Tools::GetTowerPositionRhoEtaPhi(
    const int rawKey,
    const int subsys,
    const float zVtx,
    PHCompositeNode* topNode
  ) {

    // get corresponding geometry
    RawTowerGeom* geometry = Interfaces::GetTowerGeometry(topNode, subsys, rawKey);

    // calculate variables
    const float zShift = (geometry -> get_center_z()) - zVtx;
    const float radius = geometry -> get_center_radius();
    const float eta    = asinh(zShift / radius);
    const float phi    = atan2(geometry -> get_center_y(), geometry -> get_center_x());

    // grab (rho, eta, phi) coordinates
    ROOT::Math::RhoEtaPhiVector position(
      radius,
      eta,
      phi
    );
    return position;

  }  // end 'GetTowerPositionRhoEtaPhi(int, int, float, PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Get 4-momentum for a tower
  // --------------------------------------------------------------------------
  ROOT::Math::PxPyPzEVector Tools::GetTowerMomentum(const double energy, const ROOT::Math::RhoEtaPhiVector pos) {

    // grab eta, phi, and pt
    const double hTwr  = pos.Eta();
    const double fTwr  = pos.Phi();
    const double ptTwr = energy / cosh(hTwr);

    // fill in 4-vector
    ROOT::Math::PxPyPzEVector mom(
      ptTwr * cos(fTwr),
      ptTwr * sin(fTwr),
      ptTwr * sinh(hTwr),
      energy
    );
    return mom;

  }  // end 'GetTowerMomentum(double, ROOT::Math::XYZVector, ROOT::Math::XYZVector)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
