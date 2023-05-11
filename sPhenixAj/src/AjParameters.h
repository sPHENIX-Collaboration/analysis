/** @file AjParameters.hh
    @author Kolja Kauder
    @brief Common parameters
    @details Used to quickly include the same parameters into different macros.
    @date Jul 29, 2015   
 */

#ifndef AJPARAMETERS_HH
#define AJPARAMETERS_HH

namespace AjParameters{

  const double jet_ptmin = 10.0;     ///< Min jet pT
  const double jet_ptmax = 1000.0;   ///< Max jet pT
  
  const double LeadPtMin=20.0;      ///< Min leading jet pT 
  const double SubLeadPtMin=10.0;   ///< Min sub-leading jet pT 
  const double dPhiCut = 0.4;       ///< Dijet acceptance angle,  |&phi;1 - &phi;2 -&pi;| < dPhiCut
  
  const double max_track_rap = 1.0; ///< Constituent &eta; acceptance
  const double PtConsHi=2.0;        ///< High constituent pT cut (low cut is specified in wrapper)


  // Event quality
  const double VzCut=10;            ///< Vertex z 
  
  // Track quality
  const double DcaCut=1.0;               ///< track dca
  const int NMinFit=20;                  ///< minimum number of fit points for tracks
  const double FitOverMaxPointsCut=0.52; ///< NFit / NFitPossible
}

#endif // AJPARAMETERS_HH
