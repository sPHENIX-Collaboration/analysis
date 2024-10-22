/// ---------------------------------------------------------------------------
/*! \file   TrkInfo.cc
 *  \author Derek Anderson
 *  \date   03.05.2024
 *
 *  Utility class to hold information from tracks.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_TRKINFO_CC

// class definition
#include "TrkInfo.h"

// make comon namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // private methods ==========================================================

  // --------------------------------------------------------------------------
  //! Set data members to absolute minima
  // --------------------------------------------------------------------------
  void Types::TrkInfo::Minimize() {

    id         = -1 * numeric_limits<int>::max();
    vtxID      = -1 * numeric_limits<int>::max();
    nMvtxLayer = -1 * numeric_limits<int>::max();
    nInttLayer = -1 * numeric_limits<int>::max();
    nTpcLayer  = -1 * numeric_limits<int>::max();
    nMvtxClust = -1 * numeric_limits<int>::max();
    nInttClust = -1 * numeric_limits<int>::max();
    nTpcClust  = -1 * numeric_limits<int>::max();
    eta        = -1. * numeric_limits<double>::max();
    phi        = -1. * numeric_limits<double>::max();
    px         = -1. * numeric_limits<double>::max();
    py         = -1. * numeric_limits<double>::max();
    pz         = -1. * numeric_limits<double>::max();
    pt         = -1. * numeric_limits<double>::max();
    ene        = -1. * numeric_limits<double>::max();
    dcaXY      = -1. * numeric_limits<double>::max();
    dcaZ       = -1. * numeric_limits<double>::max();
    ptErr      = -1. * numeric_limits<double>::max();
    quality    = -1. * numeric_limits<double>::max();
    vx         = -1. * numeric_limits<double>::max();
    vy         = -1. * numeric_limits<double>::max();
    vz         = -1. * numeric_limits<double>::max();
    return;

  }  // end 'Minimize()'



  // --------------------------------------------------------------------------
  //! Set data members to absolute maxima
  // --------------------------------------------------------------------------
  void Types::TrkInfo::Maximize() {

    id         = numeric_limits<int>::max();
    vtxID      = numeric_limits<int>::max();
    nMvtxLayer = numeric_limits<int>::max();
    nInttLayer = numeric_limits<int>::max();
    nTpcLayer  = numeric_limits<int>::max();
    nMvtxClust = numeric_limits<int>::max();
    nInttClust = numeric_limits<int>::max();
    nTpcClust  = numeric_limits<int>::max();
    eta        = numeric_limits<double>::max();
    phi        = numeric_limits<double>::max();
    px         = numeric_limits<double>::max();
    py         = numeric_limits<double>::max();
    pz         = numeric_limits<double>::max();
    pt         = numeric_limits<double>::max();
    ene        = numeric_limits<double>::max();
    dcaXY      = numeric_limits<double>::max();
    dcaZ       = numeric_limits<double>::max();
    ptErr      = numeric_limits<double>::max();
    quality    = numeric_limits<double>::max();
    vx         = numeric_limits<double>::max();
    vy         = numeric_limits<double>::max();
    vz         = numeric_limits<double>::max();
    return;

  }  // end 'Maximize()'


  // public methods ===========================================================

  // --------------------------------------------------------------------------
  //! Reset object by maximizing data members
  // --------------------------------------------------------------------------
  void Types::TrkInfo::Reset() {

    Maximize();
    return;

  }  // end 'Reset()'



  // --------------------------------------------------------------------------
  //! Pull relevant information from a F4A SvtxTrack
  // --------------------------------------------------------------------------
  void Types::TrkInfo::SetInfo(SvtxTrack* track, PHCompositeNode* topNode) {

    // do relevant calculations
    const ROOT::Math::XYZVector trkVtx     = Tools::GetTrackVertex(track, topNode);
    const pair<double, double>  trkDcaPair = Tools::GetTrackDcaPair(track, topNode);

    // set track info
    id         = track -> get_id();
    vtxID      = track -> get_vertex_id();
    quality    = track -> get_quality();
    eta        = track -> get_eta();
    phi        = track -> get_phi();
    px         = track -> get_px();
    py         = track -> get_py();
    pz         = track -> get_pz();
    pt         = track -> get_pt();
    ene        = sqrt((px * px) + (py * py) + (pz * pz) + (Const::MassPion() * Const::MassPion()));
    vx         = trkVtx.x();
    vy         = trkVtx.y();
    vz         = trkVtx.z();
    dcaXY      = trkDcaPair.first;
    dcaZ       = trkDcaPair.second;
    nMvtxLayer = Tools::GetNumLayer(track, Const::Subsys::Mvtx);
    nInttLayer = Tools::GetNumLayer(track, Const::Subsys::Intt);
    nTpcLayer  = Tools::GetNumLayer(track, Const::Subsys::Tpc);
    nMvtxClust = Tools::GetNumClust(track, Const::Subsys::Mvtx);
    nInttClust = Tools::GetNumClust(track, Const::Subsys::Intt);
    nTpcClust  = Tools::GetNumClust(track, Const::Subsys::Tpc);
    ptErr      = Tools::GetTrackDeltaPt(track);
    return;

  }  // end 'SetInfo(SvtxTrack*, PHCompositeNode*)'



  // --------------------------------------------------------------------------
  //! Check if object is within provided bounds (explicit minimum, maximum)
  // --------------------------------------------------------------------------
  bool Types::TrkInfo::IsInAcceptance(const TrkInfo& minimum, const TrkInfo& maximum) const {

    return ((*this >= minimum) && (*this <= maximum));

  }  // end 'IsInAcceptance(TrkInfo&, TrkInfo&)'



  // --------------------------------------------------------------------------
  //! Check if object is within provided bounds (set by pair)
  // --------------------------------------------------------------------------
  bool Types::TrkInfo::IsInAcceptance(const pair<TrkInfo, TrkInfo>& range) const {

    return ((*this >= range.first) && (*this <= range.second));

  }  // end 'IsInAcceptance(pair<TrkInfo, TrkInfo>&)'



  // --------------------------------------------------------------------------
  //! Check if object is within pt-dependent DCA cut
  // --------------------------------------------------------------------------
  bool Types::TrkInfo::IsInSigmaDcaCut(
    const pair<float, float> nSigCut,
    const pair<float, float> ptFitMax,
    const pair<TF1*, TF1*> fSigmaDca
  ) const {

    // if above max pt used to fit dca width, use value of fit at max pt
    const double ptEvalXY = (pt > ptFitMax.first)  ? ptFitMax.first  : pt;
    const double ptEvalZ  = (pt > ptFitMax.second) ? ptFitMax.second : pt;

    // check if dca is in cut
    const bool isInDcaRangeXY  = (abs(dcaXY) < (nSigCut.first  * (fSigmaDca.first  -> Eval(ptEvalXY))));
    const bool isInDcaRangeZ   = (abs(dcaZ)  < (nSigCut.second * (fSigmaDca.second -> Eval(ptEvalZ))));
    const bool isInSigmaDcaCut = (isInDcaRangeXY && isInDcaRangeZ);
    return isInSigmaDcaCut;

  }  // end 'IsInSigmaDcaCut(pair<float, float>, pair<float, float>, pair<TF1*, TF1*>)'



  // --------------------------------------------------------------------------
  //! Check if object is from the primary vertex
  // --------------------------------------------------------------------------
  bool Types::TrkInfo::IsFromPrimaryVtx(PHCompositeNode* topNode) {

    GlobalVertex* primVtx   = Interfaces::GetGlobalVertex(topNode);
    const int     primVtxID = primVtx -> get_id();
    return (vtxID == primVtxID);

  }  // end 'IsFromPrimaryVtx(PHCompositeNode*)'



  // static methods ===========================================================

  // --------------------------------------------------------------------------
  //! Get list of data fields
  // --------------------------------------------------------------------------
  vector<string> Types::TrkInfo::GetListOfMembers() {

    vector<string> members = {
      "id",
      "vtxID",
      "nMvtxLayer",
      "nInttLayer",
      "nTpcLayer",
      "nMvtxClust",
      "nInttClust",
      "nTpcClust",
      "eta",
      "phi",
      "px",
      "py",
      "pz",
      "pt",
      "ene",
      "dcaXY",
      "dcaZ",
      "ptErr",
      "quality",
      "vx",
      "vy",
      "vz"
    };
    return members;

  }  // end 'GetListOfMembers()'



  // overloaded operators =====================================================

  // --------------------------------------------------------------------------
  //! Overloaded less-than comparison
  // --------------------------------------------------------------------------
  bool Types::operator <(const TrkInfo& lhs, const TrkInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isLessThan = (
      (lhs.nMvtxLayer < rhs.nMvtxLayer) &&
      (lhs.nInttLayer < rhs.nInttLayer) &&
      (lhs.nTpcLayer  < rhs.nTpcLayer)  &&
      (lhs.nMvtxClust < rhs.nMvtxClust) &&
      (lhs.nInttClust < rhs.nInttClust) &&
      (lhs.nTpcClust  < rhs.nTpcClust)  &&
      (lhs.eta        < rhs.eta)        &&
      (lhs.phi        < rhs.phi)        &&
      (lhs.px         < rhs.px)         &&
      (lhs.py         < rhs.py)         &&
      (lhs.pz         < rhs.pz)         &&
      (lhs.pt         < rhs.pt)         &&
      (lhs.ene        < rhs.ene)        &&
      (lhs.dcaXY      < rhs.dcaXY)      &&
      (lhs.dcaZ       < rhs.dcaZ)       &&
      (lhs.ptErr      < rhs.ptErr)      &&
      (lhs.quality    < rhs.quality)    &&
      (lhs.vx         < rhs.vx)         &&
      (lhs.vy         < rhs.vy)         &&
      (lhs.vz         < rhs.vz)
    );
    return isLessThan;

  }  // end 'operator <(TrkInfo&, TrkInfo&)'



  // --------------------------------------------------------------------------
  //! Overloaded greater-than comparison
  // --------------------------------------------------------------------------
  bool Types::operator >(const TrkInfo& lhs, const TrkInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isGreaterThan = (
      (lhs.nMvtxLayer > rhs.nMvtxLayer) &&
      (lhs.nInttLayer > rhs.nInttLayer) &&
      (lhs.nTpcLayer  > rhs.nTpcLayer)  &&
      (lhs.nMvtxClust > rhs.nMvtxClust) &&
      (lhs.nInttClust > rhs.nInttClust) &&
      (lhs.nTpcClust  > rhs.nTpcClust)  &&
      (lhs.eta        > rhs.eta)        &&
      (lhs.phi        > rhs.phi)        &&
      (lhs.px         > rhs.px)         &&
      (lhs.py         > rhs.py)         &&
      (lhs.pz         > rhs.pz)         &&
      (lhs.pt         > rhs.pt)         &&
      (lhs.ene        > rhs.ene)        &&
      (lhs.dcaXY      > rhs.dcaXY)      &&
      (lhs.dcaZ       > rhs.dcaZ)       &&
      (lhs.ptErr      > rhs.ptErr)      &&
      (lhs.quality    > rhs.quality)    &&
      (lhs.vx         > rhs.vx)         &&
      (lhs.vy         > rhs.vy)         &&
      (lhs.vz         > rhs.vz)
    );
    return isGreaterThan;

  }  // end 'operator >(TrkInfo&, TrkInfo&)'



  // --------------------------------------------------------------------------
  //! Overloaded less-than-or-equal-to comparison
  // --------------------------------------------------------------------------
  bool Types::operator <=(const TrkInfo& lhs, const TrkInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isLessThanOrEqualTo = (
      (lhs.nMvtxLayer <= rhs.nMvtxLayer) &&
      (lhs.nInttLayer <= rhs.nInttLayer) &&
      (lhs.nTpcLayer  <= rhs.nTpcLayer)  &&
      (lhs.nMvtxClust <= rhs.nMvtxClust) &&
      (lhs.nInttClust <= rhs.nInttClust) &&
      (lhs.nTpcClust  <= rhs.nTpcClust)  &&
      (lhs.eta        <= rhs.eta)        &&
      (lhs.phi        <= rhs.phi)        &&
      (lhs.px         <= rhs.px)         &&
      (lhs.py         <= rhs.py)         &&
      (lhs.pz         <= rhs.pz)         &&
      (lhs.pt         <= rhs.pt)         &&
      (lhs.ene        <= rhs.ene)        &&
      (lhs.dcaXY      <= rhs.dcaXY)      &&
      (lhs.dcaZ       <= rhs.dcaZ)       &&
      (lhs.ptErr      <= rhs.ptErr)      &&
      (lhs.quality    <= rhs.quality)    &&
      (lhs.vx         <= rhs.vx)         &&
      (lhs.vy         <= rhs.vy)         &&
      (lhs.vz         <= rhs.vz)
    );
    return isLessThanOrEqualTo;

  }  // end 'operator <(TrkInfo&, TrkInfo&)'



  // --------------------------------------------------------------------------
  //! Overloaded greater-than-or-equal-to comparison
  // --------------------------------------------------------------------------
  bool Types::operator >=(const TrkInfo& lhs, const TrkInfo& rhs) {

    // note that some quantities aren't relevant for this comparison
    const bool isGreaterThanOrEqualTo = (
      (lhs.nMvtxLayer >= rhs.nMvtxLayer) &&
      (lhs.nInttLayer >= rhs.nInttLayer) &&
      (lhs.nTpcLayer  >= rhs.nTpcLayer)  &&
      (lhs.nMvtxClust >= rhs.nMvtxClust) &&
      (lhs.nInttClust >= rhs.nInttClust) &&
      (lhs.nTpcClust  >= rhs.nTpcClust)  &&
      (lhs.eta        >= rhs.eta)        &&
      (lhs.phi        >= rhs.phi)        &&
      (lhs.px         >= rhs.px)         &&
      (lhs.py         >= rhs.py)         &&
      (lhs.pz         >= rhs.pz)         &&
      (lhs.pt         >= rhs.pt)         &&
      (lhs.ene        >= rhs.ene)        &&
      (lhs.dcaXY      >= rhs.dcaXY)      &&
      (lhs.dcaZ       >= rhs.dcaZ)       &&
      (lhs.ptErr      >= rhs.ptErr)      &&
      (lhs.quality    >= rhs.quality)    &&
      (lhs.vx         >= rhs.vx)         &&
      (lhs.vy         >= rhs.vy)         &&
      (lhs.vz         >= rhs.vz)
    );
    return isGreaterThanOrEqualTo;

  }  // end 'operator >(TrkInfo&, TrkInfo&)'



  // ctor/dtor ================================================================

  // --------------------------------------------------------------------------
  //! Default class constructor
  // --------------------------------------------------------------------------
  Types::TrkInfo::TrkInfo() {

    /* nothing to do */

  }  // end ctor()



  // --------------------------------------------------------------------------
  //! Default class destructor
  // --------------------------------------------------------------------------
  Types::TrkInfo::~TrkInfo() {

    /* nothing to do */

  }  // end dtor()



  // --------------------------------------------------------------------------
  //! Constructor accepting initialization option (minimize or maximize)
  // --------------------------------------------------------------------------
  Types::TrkInfo::TrkInfo(const Const::Init init) {

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
  //! Constructor accepting a F4A SvtxTrack
  // --------------------------------------------------------------------------
  Types::TrkInfo::TrkInfo(SvtxTrack* track, PHCompositeNode* topNode) {

    SetInfo(track, topNode);

  }  // end ctor(SvtxTrack*, PHCompositeNode*)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------
