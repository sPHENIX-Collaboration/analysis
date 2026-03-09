/// ---------------------------------------------------------------------------
/*! \file   REvtInfo.cc
 *  \author Derek Anderson
 *  \date   03.06.2024
 *
 *  Utility class to hold event-level reconstructed
 *  information.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_REVTINFO_CC

// class definition
#include "REvtInfo.h"

// make comon namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // private methods ==========================================================

  // --------------------------------------------------------------------------
  //! Set data members to absolute minima
  // --------------------------------------------------------------------------
  void Types::REvtInfo::Minimize() {

    nTrks     = -1 * numeric_limits<int>::max();
    pSumTrks  = -1. * numeric_limits<double>::max();
    eSumEMCal = -1. * numeric_limits<double>::max();
    eSumIHCal = -1. * numeric_limits<double>::max();
    eSumOHCal = -1. * numeric_limits<double>::max();
    vx        = -1. * numeric_limits<double>::max();
    vy        = -1. * numeric_limits<double>::max();
    vz        = -1. * numeric_limits<double>::max();
    vr        = -1. * numeric_limits<double>::max();
    return;

  }  // end 'Minimize()'



  // --------------------------------------------------------------------------
  //! Set data members to absolute maxima
  // --------------------------------------------------------------------------
  void Types::REvtInfo::Maximize() {

    nTrks     = numeric_limits<int>::max();
    pSumTrks  = numeric_limits<double>::max();
    eSumEMCal = numeric_limits<double>::max();
    eSumIHCal = numeric_limits<double>::max();
    eSumOHCal = numeric_limits<double>::max();
    vx        = numeric_limits<double>::max();
    vy        = numeric_limits<double>::max();
    vz        = numeric_limits<double>::max();
    vr        = numeric_limits<double>::max();
    return;

  }  // end 'Maximize()'



  // public methods ===========================================================

  // --------------------------------------------------------------------------
  //! Reset object by maximizing data members
  // --------------------------------------------------------------------------
  void Types::REvtInfo::Reset() {

    Maximize();
    return;

  }  // end 'Reset()'



  // --------------------------------------------------------------------------
  //! Pull relevant information from a F4A node
  // --------------------------------------------------------------------------
  void Types::REvtInfo::SetInfo(PHCompositeNode* topNode) {

    // get sums
    nTrks     = Tools::GetNumTrks(topNode);
    pSumTrks  = Tools::GetSumTrkMomentum(topNode);
    eSumEMCal = Tools::GetSumCaloEne(topNode, "CLUSTER_CEMC");
    eSumIHCal = Tools::GetSumCaloEne(topNode, "CLUSTER_HCALIN");
    eSumOHCal = Tools::GetSumCaloEne(topNode, "CLUSTER_HCALOUT");

    // get vertex
    ROOT::Math::XYZVector vtx = Interfaces::GetRecoVtx(topNode);
    vx = vtx.x();
    vy = vtx.y();
    vz = vtx.z();
    vr = hypot(vx, vy);
    return;

  }  // end 'SetInfo(PHCompositeNode*)'



  // static methods ===========================================================

  // --------------------------------------------------------------------------
  //! Get list of data fields
  // --------------------------------------------------------------------------
  vector<string> Types::REvtInfo::GetListOfMembers() {

    vector<string> members = {
      "nTrks",
      "pSumTrks",
      "eSumEMCal",
      "eSumIHCal",
      "eSumOHCal",
      "vx",
      "vy",
      "vz",
      "vr"
    };
    return members;

  }  // end 'GetListOfMembers()'



  // ctor/dtor ================================================================

  // --------------------------------------------------------------------------
  //! Default class constructor
  // --------------------------------------------------------------------------
  Types::REvtInfo::REvtInfo() {

    /* nothing to do */

  }  // end ctor()



  // --------------------------------------------------------------------------
  //! Default class destructor
  // --------------------------------------------------------------------------
  Types::REvtInfo::~REvtInfo() {

    /* nothing to do */

  }  // end dtor()



  // --------------------------------------------------------------------------
  //! Constructor accepting initialization option (minimize or maximize)
  // --------------------------------------------------------------------------
  Types::REvtInfo::REvtInfo(const Const::Init init) {

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

  }  // end ctor(Const::init)



  // --------------------------------------------------------------------------
  //! Constructor accepting a F4A node
  // --------------------------------------------------------------------------
  Types::REvtInfo::REvtInfo(PHCompositeNode* topNode) {

    SetInfo(topNode);

  }  // end ctor(PHCompositeNode*)

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
