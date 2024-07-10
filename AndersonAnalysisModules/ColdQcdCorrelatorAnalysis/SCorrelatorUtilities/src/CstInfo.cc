// ----------------------------------------------------------------------------
// 'CstInfo.cc'
// Derek Anderson
// 03.03.2024
//
// Utility class to hold information from
// jet constituents.
// ----------------------------------------------------------------------------

#define SCORRELATORUTILITIES_CSTINFO_CC

// class definition
#include "CstInfo.h"

// make comon namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // internal methods ---------------------------------------------------------

  void Types::CstInfo::Minimize() {

    type    = -1 * numeric_limits<int>::max();
    cstID   = -1 * numeric_limits<int>::max();
    jetID   = -1 * numeric_limits<int>::max();
    embedID = -1 * numeric_limits<int>::max();
    pid     = -1 * numeric_limits<int>::max();
    z       = -1. * numeric_limits<double>::max();
    dr      = -1. * numeric_limits<double>::max();
    jt      = -1. * numeric_limits<double>::max();
    ene     = -1. * numeric_limits<double>::max();
    px      = -1. * numeric_limits<double>::max();
    py      = -1. * numeric_limits<double>::max();
    pz      = -1. * numeric_limits<double>::max();
    pt      = -1. * numeric_limits<double>::max();
    eta     = -1. * numeric_limits<double>::max();
    phi     = -1. * numeric_limits<double>::max();
    return;

  }  // end 'Minimize()'



  void Types::CstInfo::Maximize() {

    type    = numeric_limits<int>::max();
    cstID   = numeric_limits<int>::max();
    jetID   = numeric_limits<int>::max();
    embedID = numeric_limits<int>::max();
    pid     = numeric_limits<int>::max();
    z       = numeric_limits<double>::max();
    dr      = numeric_limits<double>::max();
    jt      = numeric_limits<double>::max();
    ene     = numeric_limits<double>::max();
    px      = numeric_limits<double>::max();
    py      = numeric_limits<double>::max();
    pz      = numeric_limits<double>::max();
    pt      = numeric_limits<double>::max();
    eta     = numeric_limits<double>::max();
    phi     = numeric_limits<double>::max();
    return;

  }  // end 'Maximize()'



  // public methods -----------------------------------------------------------

  void Types::CstInfo::Reset() {

    Maximize();
    return;

  }  // end 'Reset()'



  void Types::CstInfo::SetInfo(fastjet::PseudoJet& pseudojet) {

    cstID = pseudojet.user_index();
    ene   = pseudojet.E();
    px    = pseudojet.px();
    py    = pseudojet.py();
    pz    = pseudojet.pz();
    pt    = pseudojet.perp();
    eta   = pseudojet.pseudorapidity();
    phi   = pseudojet.phi_std();
    return;

  }  // end 'SetInfo(fastjet::PseudoJet)'



  void Types::CstInfo::SetJetInfo(const Types::JetInfo& jet) {

    // grab 3-momenta
    ROOT::Math::XYZVector pJet(jet.GetPX(), jet.GetPY(), jet.GetPZ());
    ROOT::Math::XYZVector pCst(px, py, pz);
    ROOT::Math::XYZVector pCross = pCst.Cross(pJet);

    // get delta eta/phi
    const double dEta = eta - jet.GetEta();
    const double dPhi = phi - jet.GetPhi();

    // set values and exit
    z  = pCst.Dot(pJet) / pJet.Mag2();
    dr = sqrt((dEta * dEta) + (dPhi * dPhi));
    jt = sqrt( pCross.Mag2() ) / pJet.Mag2();
    return;

  }  // end 'SetJetInfo(Types::JetInfo&)'



  bool Types::CstInfo::IsInAcceptance(const CstInfo& minimum, const CstInfo& maximum) const {

    return ((*this >= minimum) && (*this <= maximum));

  }  // end 'IsInAcceptance(CstInfo&, CstInfo&)'



  bool Types::CstInfo::IsInAcceptance(const pair<CstInfo, CstInfo>& range) const {

    return ((*this >= range.first) && (*this <= range.second));

  }  // end 'IsInAcceptance(pair<CstInfo, CstInfo>&)'



  // static methods -----------------------------------------------------------

  vector<string> Types::CstInfo::GetListOfMembers() {

    vector<string> members = {
      "type",
      "cstID",
      "jetID",
      "embedID",
      "pid",
      "z",
      "dr",
      "jt",
      "ene",
      "px",
      "py",
      "pz",
      "pt",
      "eta",
      "phi"
    };
    return members;

  }  // end 'GetListOfMembers()'



  // overloaded operators -----------------------------------------------------

  bool Types::operator <(const CstInfo& lhs, const CstInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isLessThan = (
      (lhs.z   < rhs.z)   &&
      (lhs.dr  < rhs.dr)  &&
      (lhs.jt  < rhs.jt)  &&
      (lhs.ene < rhs.ene) &&
      (lhs.px  < rhs.px)  &&
      (lhs.py  < rhs.py)  &&
      (lhs.pz  < rhs.pz)  &&
      (lhs.pt  < rhs.pt)  &&
      (lhs.eta < rhs.eta) &&
      (lhs.phi < rhs.phi)
    );
    return isLessThan;
  
  }  // end 'operator <(CstInfo&, CstInfo&)'



  bool Types::operator >(const CstInfo& lhs, const CstInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isGreaterThan = (
      (lhs.z   > rhs.z)   &&
      (lhs.dr  > rhs.dr)  &&
      (lhs.jt  > lhs.jt)  &&
      (lhs.ene > rhs.ene) &&
      (lhs.px  > rhs.px)  &&
      (lhs.py  > rhs.py)  &&
      (lhs.pz  > rhs.pz)  &&
      (lhs.pt  > rhs.pt)  &&
      (lhs.eta > rhs.eta) &&
      (lhs.phi > rhs.phi)
    );
    return isGreaterThan;
  
  }  // end 'operator >(CstInfo&, CstInfo&)'



  bool Types::operator <=(const CstInfo& lhs, const CstInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isLessThanOrEqualTo = (
      (lhs.z   <= rhs.z)   &&
      (lhs.dr  <= rhs.dr)  &&
      (lhs.jt  <= rhs.jt)  &&
      (lhs.ene <= rhs.ene) &&
      (lhs.px  <= rhs.px)  &&
      (lhs.py  <= rhs.py)  &&
      (lhs.pz  <= rhs.pz)  &&
      (lhs.pt  <= rhs.pt)  &&
      (lhs.eta <= rhs.eta) &&
      (lhs.phi <= rhs.phi)
    );
    return isLessThanOrEqualTo;
  
  }  // end 'operator <=(CstInfo&, CstInfo&)'



  bool Types::operator >=(const CstInfo& lhs, const CstInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isGreaterThan = (
      (lhs.z   >= rhs.z)   &&
      (lhs.dr  >= rhs.dr)  &&
      (lhs.jt  >= rhs.jt)  &&
      (lhs.ene >= rhs.ene) &&
      (lhs.px  >= rhs.px)  &&
      (lhs.py  >= rhs.py)  &&
      (lhs.pz  >= rhs.pz)  &&
      (lhs.pt  >= rhs.pt)  &&
      (lhs.eta >= rhs.eta) &&
      (lhs.phi >= rhs.phi)
    );
    return isGreaterThan;
  
  }  // end 'operator >=(CstInfo&, CstInfo&)'



  // ctor/dtor ----------------------------------------------------------------

  Types::CstInfo::CstInfo() {

    /* nothing to do */

  }  // end ctor()



  Types::CstInfo::~CstInfo() {

    /* nothing to do */

  }  // end dtor()



  Types::CstInfo::CstInfo(const Const::Init init) {

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



  Types::CstInfo::CstInfo(fastjet::PseudoJet& pseudojet) {

    SetInfo(pseudojet);

  }  // end ctor(PseudoJet&)

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
