/** @file AjParameters.hh
    @author Kolja Kauder
    @brief Common parameters
    @details Used to quickly include the same parameters into different macros.
    @date Feb 10, 2015   
 */

#ifndef AJPARAMETERS_HH
#define AJPARAMETERS_HH

namespace AjParameters{

  /// Repetitions in the background. Anything other than 1 WILL NOT WORK because
  /// a) we're using explicit ghosts (though we don't have to)
  /// b) more importantly, the background subtractor contains fastjet::SelectorNHardest(2)
  ///    which doesn't work jet-by-jet and throws an error
  const int ghost_repeat = 1;
  const double ghost_area = 0.01;    ///< ghost area

  const double jet_ptmin = 10.0;     ///< Min jet pT
  const double jet_ptmax = 1000.0;   ///< Max jet pT
  
  const double LeadPtMin=20.0;      ///< Min leading jet pT 
  const double SubLeadPtMin=10.0;   ///< Min sub-leading jet pT 
  const double dPhiCut = 0.4;       ///< Dijet acceptance angle,  |&phi;1 - &phi;2 -&pi;| < dPhiCut
  
  const double max_track_rap = 1.0; ///< Constituent &eta; acceptance
  const double PtConsHi=2.0;        ///< High constituent pT cut (low cut is specified in wrapper)

  const double VzCut=30;            ///< Vertex z 
  // const double VzDiffCut=6;         ///< |Vz(TPC) - Vz(VPD)| <-- NOT WORKING in older data (no VPD)
  const double VzDiffCut=1000;      ///< |Vz(TPC) - Vz(VPD)|
  
  const double DcaCut=1.0;          ///< track dca
  const int NMinFit=20;             ///< minimum number of fit points for tracks
  const double FitOverMaxPointsCut=0.52; ///< NFit / NFitPossible

  // ************************************
  // Do NOT cut high tracks and towers!
  // Instead, reject the whole event when
  // of these is found
  // ************************************
  const double MaxEtCut=1000;       ///< tower ET cut
  const double MaxTrackPt=1000;     ///< track pT cut
  
  // EVENT rejection cuts
  const double MaxEventPtCut=30;       ///< track pT cut for event
  const double MaxEventEtCut=30;       ///< tower ET cut for event

  
  // // TO REPRODUCE JOERN:
  // const double VzCut=30;
  // const double AuAuRefMultCut=351; // not used
  // const double DcaCut=1.0;
  // const int NMinFit=20;
  // const double FitOverMaxPointsCut=0.52;
  // // NO VzDiffCut
  // // NO MaxTrackPt or MaxEtCut

  
}
#endif // AJPARAMETERS_HH
