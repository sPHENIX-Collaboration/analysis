// ----------------------------------------------------------------------------
// 'RecoInfo.cc'
// Derek Anderson
// 03.06.2024
//
// Utility class to hold event-level reconstructed
// information.
// ----------------------------------------------------------------------------

#define SCORRELATORUTILITIES_RECOINFO_CC

// class definition
#include "RecoInfo.h"

// make comon namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {


  // internal methods ---------------------------------------------------------

  void Types::RecoInfo::Minimize() {

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



  void Types::RecoInfo::Maximize() {

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



  // public methods -----------------------------------------------------------

  void Types::RecoInfo::Reset() {

    Maximize();
    return;

  }  // end 'Reset()'



  void Types::RecoInfo::SetInfo(PHCompositeNode* topNode) {

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



  // static methods -----------------------------------------------------------

  vector<string> Types::RecoInfo::GetListOfMembers() {

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



  // ctor/dtor ----------------------------------------------------------------

  Types::RecoInfo::RecoInfo() {

    /* nothing to do */

  }  // end ctor()



  Types::RecoInfo::~RecoInfo() {

    /* nothing to do */

  }  // end dtor()



  Types::RecoInfo::RecoInfo(const Const::Init init) {

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



  Types::RecoInfo::RecoInfo(PHCompositeNode* topNode) {

    SetInfo(topNode);

  }  // end ctor(PHCompositeNode*)

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
