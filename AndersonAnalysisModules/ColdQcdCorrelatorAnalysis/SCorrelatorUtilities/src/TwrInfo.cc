/// ---------------------------------------------------------------------------
/*! \file   TwrInfo.cc
 *  \author Derek Anderson
 *  \date   08.04.2024
 *
 *  Utility class to hold information from
 *  calorimeter clusters.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_TWRINFO_CC

// class definition
#include "TwrInfo.h"

// make comon namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // private methods ==========================================================

  // --------------------------------------------------------------------------
  //! Set data members to absolute minima
  // --------------------------------------------------------------------------
  void Types::TwrInfo::Minimize() {

    system  = -1 * numeric_limits<int>::max();
    status  = -1 * numeric_limits<int>::max();
    channel = -1 * numeric_limits<int>::max();
    id      = -1 * numeric_limits<int>::max();
    ene     = -1. * numeric_limits<double>::max();
    rho     = -1. * numeric_limits<double>::max();
    eta     = -1. * numeric_limits<double>::max();
    phi     = -1. * numeric_limits<double>::max();
    px      = -1. * numeric_limits<double>::max();
    py      = -1. * numeric_limits<double>::max();
    pz      = -1. * numeric_limits<double>::max();
    rx      = -1. * numeric_limits<double>::max();
    ry      = -1. * numeric_limits<double>::max();
    rz      = -1. * numeric_limits<double>::max();
    return;

  }  // end 'Minimize()'



  // --------------------------------------------------------------------------
  //! Set data members to absolute maxima
  // --------------------------------------------------------------------------
  void Types::TwrInfo::Maximize() {

    system  = numeric_limits<int>::max();
    status  = numeric_limits<int>::max();
    channel = numeric_limits<int>::max();
    id      = numeric_limits<int>::max();
    ene     = numeric_limits<double>::max();
    rho     = numeric_limits<double>::max();
    eta     = numeric_limits<double>::max();
    phi     = numeric_limits<double>::max();
    px      = numeric_limits<double>::max();
    py      = numeric_limits<double>::max();
    pz      = numeric_limits<double>::max();
    rx      = numeric_limits<double>::max();
    ry      = numeric_limits<double>::max();
    rz      = numeric_limits<double>::max();
    return;

  }  // end 'Maximize()'



  // public methods ===========================================================

  // --------------------------------------------------------------------------
  //! Reset object by maximizing data members
  // --------------------------------------------------------------------------
  void Types::TwrInfo::Reset() {

    Maximize();
    return;

  }  // end 'Reset()'



  // --------------------------------------------------------------------------
  //! Pull relevant information from a F4A RawTower
  // --------------------------------------------------------------------------
  void Types::TwrInfo::SetInfo(
    const int sys,
    const RawTower* tower,
    PHCompositeNode* topNode,
    optional<ROOT::Math::XYZVector> vtx
  ) {

    // if no vertex provided, use origin
    ROOT::Math::XYZVector vtxToUse(0., 0., 0.);
    if (vtx.has_value()) {
      vtxToUse = vtx.value();
    }

    // grab raw key for geometry
    const int rawKey = tower -> get_key();

    // grab position in (x, y, z)
    ROOT::Math::XYZVector xyzPos = Tools::GetTowerPositionXYZ(
      rawKey,
      sys,
      topNode
    );

    // grab position in (rho, eta, phi)
    ROOT::Math::RhoEtaPhiVector rhfPos = Tools::GetTowerPositionRhoEtaPhi(
      rawKey,
      sys,
      vtxToUse.z(),
      topNode
    );

    // grab momentum
    ROOT::Math::PxPyPzEVector momentum = Tools::GetTowerMomentum(
      tower -> get_energy(),
      rhfPos
    );

    // set remaining members
    system  = sys;
    status  = Const::TowerStatus::NA;
    id      = rawKey;
    ene     = tower -> get_energy();
    rho     = rhfPos.Rho();
    eta     = rhfPos.Eta();
    phi     = rhfPos.Phi();
    px      = momentum.Px();
    py      = momentum.Py();
    pz      = momentum.Pz();
    rx      = xyzPos.x();
    ry      = xyzPos.y();
    rz      = xyzPos.z();
    return;

  }  // end 'SetInfo(int, RawTower*, PHCompositeNode*, optional<ROOT::Math::XYZVector>)'



  // --------------------------------------------------------------------------
  //! Pull relevant information from a F4A TowerInfo
  // --------------------------------------------------------------------------
  void Types::TwrInfo::SetInfo(
    const int sys,
    const int chan,
    TowerInfo* tower,
    PHCompositeNode* topNode, 
    optional<ROOT::Math::XYZVector> vtx
  ) {

    // if no vertex provided, use origin
    ROOT::Math::XYZVector vtxToUse(0., 0., 0.);
    if (vtx.has_value()) {
      vtxToUse = vtx.value();
    }

    // get raw tower key 
    const auto indices = Tools::GetTowerIndices(chan, sys, topNode);
    const int  rawKey  = Tools::GetRawTowerKey(Const::MapIndexOntoID()[ sys ], indices);

    // grab position in (x, y, z)
    ROOT::Math::XYZVector xyzPos = Tools::GetTowerPositionXYZ(
      rawKey,
      sys,
      topNode
    );

    // grab position in (rho, eta, phi)
    ROOT::Math::RhoEtaPhiVector rhfPos = Tools::GetTowerPositionRhoEtaPhi(
      rawKey,
      sys,
      vtxToUse.z(),
      topNode
    );

    // grab momentum
    ROOT::Math::PxPyPzEVector momentum = Tools::GetTowerMomentum(
      tower -> get_energy(),
      rhfPos
    );

    // set remaining members
    system  = sys;
    status  = Tools::GetTowerStatus(tower);
    channel = chan;
    id      = get<0>(indices);
    ene     = tower -> get_energy();
    rho     = rhfPos.Rho();
    eta     = rhfPos.Eta();
    phi     = rhfPos.Phi();
    px      = momentum.Px();
    py      = momentum.Py();
    pz      = momentum.Pz();
    rx      = xyzPos.x();
    ry      = xyzPos.y();
    rz      = xyzPos.z();
    return;

  }  // end 'SetInfo(int, int, TowerInfo*, PHCompositeNode*, optional<ROOT::Math::XYZVector>)'



  // --------------------------------------------------------------------------
  //! Check if object is within provided bounds (explicit minimum, maximum)
  // --------------------------------------------------------------------------
  bool Types::TwrInfo::IsInAcceptance(const TwrInfo& minimum, const TwrInfo& maximum) const {

    return ((*this >= minimum) && (*this <= maximum));

  }  // end 'IsInAcceptance(TwrInfo&, TwrInfo&)'



  // --------------------------------------------------------------------------
  //! Check if object is within provided bounds (set by pair)
  // --------------------------------------------------------------------------
  bool Types::TwrInfo::IsInAcceptance(const pair<TwrInfo, TwrInfo>& range) const {

    return ((*this >= range.first) && (*this <= range.second));

  }  // end 'IsInAcceptance(pair<TwrInfo, TwrInfo>&)'



  // --------------------------------------------------------------------------
  //! Check if tower is good
  // --------------------------------------------------------------------------
  bool Types::TwrInfo::IsGood() const {

    return (
      (status == Const::TowerStatus::Good) ||
      (status == Const::TowerStatus::NA)
    );

  }  // end 'IsGood()'



  // static methods ===========================================================

  // --------------------------------------------------------------------------
  //! Get list of data fields
  // --------------------------------------------------------------------------
  vector<string> Types::TwrInfo::GetListOfMembers() {

    vector<string> members = {
      "system",
      "status",
      "channel",
      "id",
      "ene",
      "rho",
      "eta",
      "phi",
      "px",
      "py",
      "pz",
      "rx",
      "ry",
      "rz"
    };
    return members;

  }  // end 'GetListOfMembers()'



  // overloaded operators =====================================================

  // --------------------------------------------------------------------------
  //! Overloaded less-than comparison
  // --------------------------------------------------------------------------
  bool Types::operator <(const TwrInfo& lhs, const TwrInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isLessThan = (
      (lhs.ene  < rhs.ene)  &&
      (lhs.rho  < rhs.rho)  &&
      (lhs.eta  < rhs.eta)  &&
      (lhs.phi  < rhs.phi)  &&
      (lhs.px   < rhs.px)   &&
      (lhs.py   < rhs.py)   &&
      (lhs.pz   < rhs.pz)   &&
      (lhs.rx   < rhs.rx)   &&
      (lhs.ry   < rhs.ry)   &&
      (lhs.rz   < rhs.rz)
    );
    return isLessThan;

  }  // end 'operator <(TwrInfo&, TwrInfo&) const'



  // --------------------------------------------------------------------------
  //! Overloaded greater-than comparison
  // --------------------------------------------------------------------------
  bool Types::operator >(const TwrInfo& lhs, const TwrInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isGreaterThan = (
      (lhs.ene  > rhs.ene)  &&
      (lhs.rho  > rhs.rho)  &&
      (lhs.eta  > rhs.eta)  &&
      (lhs.phi  > rhs.phi)  &&
      (lhs.px   > rhs.px)   &&
      (lhs.py   > rhs.py)   &&
      (lhs.pz   > rhs.pz)   &&
      (lhs.rx   > rhs.rx)   &&
      (lhs.ry   > rhs.ry)   &&
      (lhs.rz   > rhs.rz)
    );
    return isGreaterThan;

  }  // end 'operator >(TwrInfo&, TwrInfo&)'



  // --------------------------------------------------------------------------
  //! Overloaded less-than-or-equal-to comparison
  // --------------------------------------------------------------------------
  bool Types::operator <=(const TwrInfo& lhs, const TwrInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isLessThanOrEqualTo = (
      (lhs.ene  <= rhs.ene)  &&
      (lhs.rho  <= rhs.rho)  &&
      (lhs.eta  <= rhs.eta)  &&
      (lhs.phi  <= rhs.phi)  &&
      (lhs.px   <= rhs.px)   &&
      (lhs.py   <= rhs.py)   &&
      (lhs.pz   <= rhs.pz)   &&
      (lhs.rx   <= rhs.rx)   &&
      (lhs.ry   <= rhs.ry)   &&
      (lhs.rz   <= rhs.rz)
    );
    return isLessThanOrEqualTo;

  }  // end 'operator <=(TwrInfo&, TwrInfo&) const'



  // --------------------------------------------------------------------------
  //! Overloaded greater-than-or-equal-to comparison
  // --------------------------------------------------------------------------
  bool Types::operator >=(const TwrInfo& lhs, const TwrInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isGreaterThanOrEqualTo = (
      (lhs.ene  >= rhs.ene)  &&
      (lhs.rho  >= rhs.rho)  &&
      (lhs.eta  >= rhs.eta)  &&
      (lhs.phi  >= rhs.phi)  &&
      (lhs.px   >= rhs.px)   &&
      (lhs.py   >= rhs.py)   &&
      (lhs.pz   >= rhs.pz)   &&
      (lhs.rx   >= rhs.rx)   &&
      (lhs.ry   >= rhs.ry)   &&
      (lhs.rz   >= rhs.rz)
    );
    return isGreaterThanOrEqualTo;

  }  // end 'operator >=(TwrInfo&, TwrInfo&)'



  // ctor/dtor ================================================================

  // --------------------------------------------------------------------------
  //! Default class constructor
  // --------------------------------------------------------------------------
  Types::TwrInfo::TwrInfo() {

     /* nothing to do */

  }  // end ctor()



  // --------------------------------------------------------------------------
  //! Default class destructor
  // --------------------------------------------------------------------------
  Types::TwrInfo::~TwrInfo() {

    /* nothing to do */

  }  // end dtor()



  // --------------------------------------------------------------------------
  //! Constructor accepting initialization option (minimize or maximize)
  // --------------------------------------------------------------------------
  Types::TwrInfo::TwrInfo(const Const::Init init) {

    switch (init) {
      case Const::Init::Minimize:
        Minimize();
        break;
      case Const::Init::Maximize:
        Maximize();
        break;
      default:
        Maximize();
        break;
    }

  }  // end ctor(Const::Init)



  // --------------------------------------------------------------------------
  //! Constructor accepting a F4A RawTower
  // --------------------------------------------------------------------------
  Types::TwrInfo::TwrInfo(
    const int sys,
    const RawTower* tower,
    PHCompositeNode* topNode,
    optional<ROOT::Math::XYZVector> vtx
  ) {

    SetInfo(sys, tower, topNode, vtx);

  }  // end ctor(int, RawTower*, PHCompositeNode*, optional<ROOT::Math::XYZVector>)'



  // --------------------------------------------------------------------------
  //! Constructor accepting a F4A TowerInfo
  // --------------------------------------------------------------------------
  Types::TwrInfo::TwrInfo(
    const int sys,
    const int chan,
    TowerInfo* tower,
    PHCompositeNode* topNode,
    optional<ROOT::Math::XYZVector> vtx
  ) {

    SetInfo(sys, chan, tower, topNode, vtx);

  }  // end ctor(int, int, TowerInfo*, PHCompositeNode*, optional<ROOT::Math::XYZVector>)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
