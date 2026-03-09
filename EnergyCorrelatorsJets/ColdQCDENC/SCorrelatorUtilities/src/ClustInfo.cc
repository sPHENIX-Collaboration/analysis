/// ---------------------------------------------------------------------------
/*! \file   ClustInfo.cc
 *  \author Derek Anderson
 *  \date   03.01.2024
 *
 *  Utility class to hold information from
 *  calorimeter clusters.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_CLUSTINFO_CC

// class definition
#include "ClustInfo.h"

// make comon namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // private methods ==========================================================

  // --------------------------------------------------------------------------
  //! Set data members to absolute minima
  // --------------------------------------------------------------------------
  void Types::ClustInfo::Minimize() {

    system  = -1 * numeric_limits<int>::max();
    id      = -1 * numeric_limits<int>::max();
    nTwr    = -1 * numeric_limits<int>::max();
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
  void Types::ClustInfo::Maximize() {

    system  = numeric_limits<int>::max();
    id      = numeric_limits<int>::max();
    nTwr    = numeric_limits<int>::max();
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
  void Types::ClustInfo::Reset() {

    Maximize();
    return;

  }  // end 'Reset()'



  // --------------------------------------------------------------------------
  //! Pull relevant information from a F4A RawCluster
  // --------------------------------------------------------------------------
  void Types::ClustInfo::SetInfo(const RawCluster* clust, optional<ROOT::Math::XYZVector> vtx, optional<int> sys) {

    // if no vertex provided, use origin
    ROOT::Math::XYZVector vtxToUse(0., 0., 0.);
    if (vtx.has_value()) {
      vtxToUse = vtx.value();
    }

    // if subsystem ID provided, set data member
    if (sys.has_value()) {
      system = sys.value();
    }

    // grab position
    ROOT::Math::XYZVector position(
      clust -> get_position().x(),
      clust -> get_position().y(),
      clust -> get_position().z()
    );

    // grab momentum
    ROOT::Math::PxPyPzEVector momentum = Tools::GetClustMomentum(clust -> get_energy(), position, vtxToUse);

    // set remaining members
    id   = clust -> get_id();
    nTwr = clust -> getNTowers();
    ene  = clust -> get_energy();
    rho  = clust -> get_r();
    eta  = momentum.Eta();
    phi  = momentum.Phi();
    px   = momentum.Px();
    py   = momentum.Py();
    pz   = momentum.Pz();
    rx   = position.X();
    ry   = position.Y();
    rz   = position.Z();
    return;

  }  // end 'SetInfo(RawCluster*, optional<ROOT::Math::XYZVector>, optional<int>)'



  // --------------------------------------------------------------------------
  //! Check if object is within provided bounds (explicit minimum, maximum)
  // --------------------------------------------------------------------------
  bool Types::ClustInfo::IsInAcceptance(const ClustInfo& minimum, const ClustInfo& maximum) const {

    return ((*this >= minimum) && (*this <= maximum));

  }  // end 'IsInAcceptance(ClustInfo&, ClustInfo&)'



  // --------------------------------------------------------------------------
  //! Check if object is within provided bounds (set by pair)
  // --------------------------------------------------------------------------
  bool Types::ClustInfo::IsInAcceptance(const pair<ClustInfo, ClustInfo>& range) const {

    return ((*this >= range.first) && (*this <= range.second));

  }  // end 'IsInAcceptance(pair<ClustInfo, ClustInfo>&)'



  // static methods ===========================================================

  // --------------------------------------------------------------------------
  //! Get list of data fields
  // --------------------------------------------------------------------------
  vector<string> Types::ClustInfo::GetListOfMembers() {

    vector<string> members = {
      "system",
      "id",
      "nTwr",
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
  bool Types::operator <(const ClustInfo& lhs, const ClustInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isLessThan = (
      (lhs.nTwr < rhs.nTwr) &&
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

  }  // end 'operator <(ClustInfo&, ClustInfo&) const'



  // --------------------------------------------------------------------------
  //! Overloaded greater-than comparison
  // --------------------------------------------------------------------------
  bool Types::operator >(const ClustInfo& lhs, const ClustInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isGreaterThan = (
      (lhs.nTwr > rhs.nTwr) &&
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

  }  // end 'operator >(ClustInfo&, ClustInfo&)'



  // --------------------------------------------------------------------------
  //! Overloaded less-than-or-equal-to comparison
  // --------------------------------------------------------------------------
  bool Types::operator <=(const ClustInfo& lhs, const ClustInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isLessThanOrEqualTo = (
      (lhs.nTwr <= rhs.nTwr) &&
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

  }  // end 'operator <=(ClustInfo&, ClustInfo&) const'



  // --------------------------------------------------------------------------
  //! Overloaded greater-than-or-equal-to comparison
  // --------------------------------------------------------------------------
  bool Types::operator >=(const ClustInfo& lhs, const ClustInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isGreaterThanOrEqualTo = (
      (lhs.nTwr >= rhs.nTwr) &&
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

  }  // end 'operator >=(ClustInfo&, ClustInfo&)'



  // ctor/dtor ================================================================

  // --------------------------------------------------------------------------
  //! Default class constructor
  // --------------------------------------------------------------------------
  Types::ClustInfo::ClustInfo() {

     /* nothing to do */

  }  // end ctor()



  // --------------------------------------------------------------------------
  //! Default class destructor
  // --------------------------------------------------------------------------
  Types::ClustInfo::~ClustInfo() {

    /* nothing to do */

  }  // end dtor()



  // --------------------------------------------------------------------------
  //! Constructor accepting initialization option (minimize or maximize)
  // --------------------------------------------------------------------------
  Types::ClustInfo::ClustInfo(const Const::Init init) {

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
  //! Constructor accepting a F4A RawCluster and possibly a vertex
  // --------------------------------------------------------------------------
  Types::ClustInfo::ClustInfo(const RawCluster* clust, optional<ROOT::Math::XYZVector> vtx, optional<int> sys) {

    SetInfo(clust, vtx, sys);

  }  // end ctor(RawCluster*, optional<ROOT::Math::XYZVector>, optional<int>)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
