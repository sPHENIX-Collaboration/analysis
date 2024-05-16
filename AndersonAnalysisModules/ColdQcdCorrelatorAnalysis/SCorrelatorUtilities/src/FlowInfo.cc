// ----------------------------------------------------------------------------
// 'FlowInfo.cc'
// Derek Anderson
// 03.03.2024
//
// Utility class to hold information from
// particle flow elements.
// ----------------------------------------------------------------------------

#define SCORRELATORUTILITIES_FLOWINFO_CC

// class definition
#include "FlowInfo.h"

// make comon namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // internal methods ---------------------------------------------------------

  void Types::FlowInfo::Minimize() {

    id   = -1 * numeric_limits<int>::max();
    type = -1 * numeric_limits<int>::max();
    mass = -1. * numeric_limits<double>::max();
    eta  = -1. * numeric_limits<double>::max();
    phi  = -1. * numeric_limits<double>::max();
    ene  = -1. * numeric_limits<double>::max();
    px   = -1. * numeric_limits<double>::max();
    py   = -1. * numeric_limits<double>::max();
    pz   = -1. * numeric_limits<double>::max();
    pt   = -1. * numeric_limits<double>::max();
    return;

  }  // end 'Minimize()'



  void Types::FlowInfo::Maximize() {

    id   = numeric_limits<int>::max();
    type = numeric_limits<int>::max();
    mass = numeric_limits<double>::max();
    eta  = numeric_limits<double>::max();
    phi  = numeric_limits<double>::max();
    ene  = numeric_limits<double>::max();
    px   = numeric_limits<double>::max();
    py   = numeric_limits<double>::max();
    pz   = numeric_limits<double>::max();
    pt   = numeric_limits<double>::max();
    return;

  }  // end 'Maximize()'



  // public methods -----------------------------------------------------------

  void Types::FlowInfo::Reset() {

    Maximize();
    return;

  }  // end 'Reset()'



  void Types::FlowInfo::SetInfo(const ParticleFlowElement* flow) {

    id   = flow -> get_id();
    type = flow -> get_type();
    mass = flow -> get_mass();
    eta  = flow -> get_eta();
    phi  = flow -> get_phi();
    ene  = flow -> get_e();
    px   = flow -> get_px();
    py   = flow -> get_py();
    pz   = flow -> get_pz();
    pt   = flow -> get_pt();
    return;
 
 }  // end 'SetInfo(ParticleFlowElement*)'



  bool Types::FlowInfo::IsInAcceptance(const FlowInfo& minimum, const FlowInfo& maximum) const {

    return ((*this >= minimum) && (*this <= maximum));

  }  // end 'IsInAcceptance(FlowInfo&, FlowInfo&)'



  bool Types::FlowInfo::IsInAcceptance(const pair<FlowInfo, FlowInfo>& range) const {

    return ((*this >= range.first) && (*this <= range.second));

  }  // end 'IsInAcceptance(pair<FlowInfo, FlowInfo>&)'



  // static methods -----------------------------------------------------------

  vector<string> Types::FlowInfo::GetListOfMembers() {

    vector<string> members = {
      "id",
      "type",
      "mass",
      "eta",
      "phi",
      "ene",
      "px",
      "py",
      "pz",
      "pt"
    };
    return members;

  }  // end 'GetListOfMembers()'



  // overloaded operators -----------------------------------------------------

  bool Types::operator <(const FlowInfo& lhs, const FlowInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isLessThan = (
      (lhs.mass < rhs.mass) &&
      (lhs.ene  < rhs.ene)  &&
      (lhs.eta  < rhs.eta)  &&
      (lhs.phi  < rhs.phi)  &&
      (lhs.px   < rhs.px)   &&
      (lhs.py   < rhs.py)   &&
      (lhs.pz   < rhs.pz)   &&
      (lhs.pt   < rhs.pt)
    );
    return isLessThan;

  }  // end 'operator <(FlowInfo&, FlowInfo&)'



  bool Types::operator >(const FlowInfo& lhs, const FlowInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isGreaterThan = (
      (lhs.mass > rhs.mass) &&
      (lhs.ene  > rhs.ene)  &&
      (lhs.eta  > rhs.eta)  &&
      (lhs.phi  > rhs.phi)  &&
      (lhs.px   > rhs.px)   &&
      (lhs.py   > rhs.py)   &&
      (lhs.pz   > rhs.pz)   &&
      (lhs.pt   > rhs.pt)
    );
    return isGreaterThan;

  }  // end 'operator >(FlowInfo&, FlowInfo&)'



  bool Types::operator <=(const FlowInfo& lhs, const FlowInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isLessThanOrEqualTo = (
      (lhs.mass < rhs.mass) &&
      (lhs.ene  < rhs.ene)  &&
      (lhs.eta  < rhs.eta)  &&
      (lhs.phi  < rhs.phi)  &&
      (lhs.px   < rhs.px)   &&
      (lhs.py   < rhs.py)   &&
      (lhs.pz   < rhs.pz)   &&
      (lhs.pt   < rhs.pt)
    );
    return isLessThanOrEqualTo;

  }  // end 'operator <=(FlowInfo&, FlowInfo&)'



  bool Types::operator >=(const FlowInfo& lhs, const FlowInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isGreaterThanOrEqualTo = (
      (lhs.mass > rhs.mass) &&
      (lhs.ene  > rhs.ene)  &&
      (lhs.eta  > rhs.eta)  &&
      (lhs.phi  > rhs.phi)  &&
      (lhs.px   > rhs.px)   &&
      (lhs.py   > rhs.py)   &&
      (lhs.pz   > rhs.pz)   &&
      (lhs.pt   > rhs.pt)
    );
    return isGreaterThanOrEqualTo;

  }  // end 'operator >=(FlowInfo&, FlowInfo&)'



  // ctor/dtor ----------------------------------------------------------------

  Types::FlowInfo::FlowInfo() {

    /* nothing to do */

  }  // end ctor()



  Types::FlowInfo::~FlowInfo() {

    /* nothing to do */

  }  // end dtor()



  Types::FlowInfo::FlowInfo(const Const::Init init) {

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



  Types::FlowInfo::FlowInfo(const ParticleFlowElement* flow) {

    SetInfo(flow);

  }  // end ctor(ParticleFlowElement*)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
