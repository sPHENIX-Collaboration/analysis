/// ---------------------------------------------------------------------------
/*! \file   JetInfo.cc
 *  \author Derek Anderson
 *  \date   03.04.2024
 *
 *  Utility class to hold information from jets.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_JETINFO_CC

// class definition
#include "JetInfo.h"

// make comon namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // private methods ==========================================================

  // --------------------------------------------------------------------------
  //! Set data members to absolute minima
  // --------------------------------------------------------------------------
  void Types::JetInfo::Minimize() {

    jetID = numeric_limits<uint32_t>::min();
    nCsts = numeric_limits<uint64_t>::min();
    ene   = -1. * numeric_limits<double>::max();
    px    = -1. * numeric_limits<double>::max();
    py    = -1. * numeric_limits<double>::max();
    pz    = -1. * numeric_limits<double>::max();
    pt    = -1. * numeric_limits<double>::max();
    eta   = -1. * numeric_limits<double>::max();
    phi   = -1. * numeric_limits<double>::max();
    area  = -1. * numeric_limits<double>::max();
    return;

  }  // end 'Minimize()'



  // --------------------------------------------------------------------------
  //! Set data members to absolute maxima
  // --------------------------------------------------------------------------
  void Types::JetInfo::Maximize() {

    jetID = numeric_limits<uint32_t>::max();
    nCsts = numeric_limits<uint64_t>::max();
    ene   = numeric_limits<double>::max();
    px    = numeric_limits<double>::max();
    py    = numeric_limits<double>::max();
    pz    = numeric_limits<double>::max();
    pt    = numeric_limits<double>::max();
    eta   = numeric_limits<double>::max();
    phi   = numeric_limits<double>::max();
    area  = numeric_limits<double>::max();
    return;

  }  // end 'Maximize()'



  // public methods ===========================================================

  // --------------------------------------------------------------------------
  //! Reset object by maximizing data members
  // --------------------------------------------------------------------------
  void Types::JetInfo::Reset() {

    Maximize();
    return;

  }  // end 'Reset()'



  // --------------------------------------------------------------------------
  //! Pull relevant information from a fastjet PseudoJet
  // --------------------------------------------------------------------------
  void Types::JetInfo::SetInfo(fastjet::PseudoJet& pseudojet) {

    nCsts = pseudojet.constituents().size();
    ene   = pseudojet.E();
    px    = pseudojet.px();
    py    = pseudojet.py();
    pz    = pseudojet.pz();
    pt    = pseudojet.perp();
    eta   = pseudojet.pseudorapidity();
    phi   = pseudojet.phi_std();
    return;

  }  // end 'SetInfo(PseudoJet&)'



  // --------------------------------------------------------------------------
  //! Pull relevant information from a F4A Jet
  // --------------------------------------------------------------------------
  void Types::JetInfo::SetInfo(Jet& jet) {

    nCsts = jet.size_comp();
    ene   = jet.get_e();
    px    = jet.get_px();
    py    = jet.get_py();
    pz    = jet.get_pz();
    pt    = jet.get_pt();
    eta   = jet.get_eta();
    phi   = jet.get_phi();
    return;

  }  // end 'SetInfo(Jet& jet)'



  // --------------------------------------------------------------------------
  //! Check if object is within provided bounds (explicit minimum, maximum)
  // --------------------------------------------------------------------------
  bool Types::JetInfo::IsInAcceptance(const JetInfo& minimum, const JetInfo& maximum) const {

    return ((*this >= minimum) && (*this <= maximum));

  }  // end 'IsInAcceptance(JetInfo&, JetInfo&)'



  // --------------------------------------------------------------------------
  //! Check if object is within provided bounds (set by pair)
  // --------------------------------------------------------------------------
  bool Types::JetInfo::IsInAcceptance(const pair<JetInfo, JetInfo>& range) const {

    return ((*this >= range.first) && (*this <= range.second));

  }  // end 'IsInAcceptance(pair<JetInfo, JetInfo>&)'



  // static methods ===========================================================

  // --------------------------------------------------------------------------
  //! Get list of data fields
  // --------------------------------------------------------------------------
  vector<string> Types::JetInfo::GetListOfMembers() {

    vector<string> members = {
      "jetID",
      "nCsts",
      "ene",
      "px",
      "py",
      "pz",
      "pt",
      "eta",
      "phi",
      "area"
    };
    return members;

  }  // end 'GetListOfMembers()'



  // overloaded operators =====================================================

  // --------------------------------------------------------------------------
  //! Overloaded less-than comparison
  // --------------------------------------------------------------------------
  bool Types::operator <(const JetInfo& lhs, const JetInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isLessThan = (
      (lhs.nCsts < rhs.nCsts) &&
      (lhs.ene   < rhs.ene)   &&
      (lhs.px    < rhs.px)    &&
      (lhs.py    < rhs.py)    &&
      (lhs.pz    < rhs.pz)    &&
      (lhs.pt    < rhs.pt)    &&
      (lhs.eta   < rhs.eta)   &&
      (lhs.phi   < rhs.phi)   &&
      (lhs.area  < rhs.area)
    );
    return isLessThan;
  
  }  // end 'operator <(JetInfo&, JetInfo&)'



  // --------------------------------------------------------------------------
  //! Overloaded greater-than comparison
  // --------------------------------------------------------------------------
  bool Types::operator >(const JetInfo& lhs, const JetInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isGreaterThan = (
      (lhs.nCsts > rhs.nCsts) &&
      (lhs.ene   > rhs.ene)   &&
      (lhs.px    > rhs.px)    &&
      (lhs.py    > rhs.py)    &&
      (lhs.pz    > rhs.pz)    &&
      (lhs.pt    > rhs.pt)    &&
      (lhs.eta   > rhs.eta)   &&
      (lhs.phi   > rhs.phi)   &&
      (lhs.area  > rhs.area)
    );
    return isGreaterThan;
  
  }  // end 'operator >(JetInfo&, JetInfo&)'



  // --------------------------------------------------------------------------
  //! Overloaded less-than-or-equal-to comparison
  // --------------------------------------------------------------------------
  bool Types::operator <=(const JetInfo& lhs, const JetInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isLessThanOrEqualTo = (
      (lhs.nCsts <= rhs.nCsts) &&
      (lhs.ene   <= rhs.ene)   &&
      (lhs.px    <= rhs.px)    &&
      (lhs.py    <= rhs.py)    &&
      (lhs.pz    <= rhs.pz)    &&
      (lhs.pt    <= rhs.pt)    &&
      (lhs.eta   <= rhs.eta)   &&
      (lhs.phi   <= rhs.phi)   &&
      (lhs.area  <= rhs.area)
    );
    return isLessThanOrEqualTo;
  
  }  // end 'operator <=(JetInfo&, JetInfo&)'



  // --------------------------------------------------------------------------
  //! Overloaded greater-than-or-equal-to comparison
  // --------------------------------------------------------------------------
  bool Types::operator >=(const JetInfo& lhs, const JetInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isGreaterThanOrEqualTo = (
      (lhs.nCsts >= rhs.nCsts) &&
      (lhs.ene   >= rhs.ene)   &&
      (lhs.px    >= rhs.px)    &&
      (lhs.py    >= rhs.py)    &&
      (lhs.pz    >= rhs.pz)    &&
      (lhs.pt    >= rhs.pt)    &&
      (lhs.eta   >= rhs.eta)   &&
      (lhs.phi   >= rhs.phi)   &&
      (lhs.area  >= rhs.area)
    );
    return isGreaterThanOrEqualTo;
  
  }  // end 'operator >=(JetInfo&, JetInfo&)'



  // ctor/dtor ================================================================

  // --------------------------------------------------------------------------
  //! Default class constructor
  // --------------------------------------------------------------------------
  Types::JetInfo::JetInfo() {

    /* nothing to do */

  }  // end ctor()



  // --------------------------------------------------------------------------
  //! Default class destructor
  // --------------------------------------------------------------------------
  Types::JetInfo::~JetInfo() {

    /* nothing to do */

  }  // end dtor()



  // --------------------------------------------------------------------------
  //! Constructor accepting initialization option (minimize or maximize)
  // --------------------------------------------------------------------------
  Types::JetInfo::JetInfo(const Const::Init init) {

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
  //! Constructor accepting a fastjet PseudoJet
  // --------------------------------------------------------------------------
  Types::JetInfo::JetInfo(fastjet::PseudoJet& pseudojet) {

    SetInfo(pseudojet);

  }  // ctor(PseudoJet&)'



  // --------------------------------------------------------------------------
  //! Constructor accepting a F4A Jet
  // --------------------------------------------------------------------------
  Types::JetInfo::JetInfo(Jet& jet) {

    SetInfo(jet);

  }  // ctor(Jet&)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
