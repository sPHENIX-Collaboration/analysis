/** @file PHAjMaker.h
    @author Kolja Kauder
    @version Revision 0.2
    @brief Class for A<SUB>J</SUB> analysis
    @details Uses JetAnalyzer objects to perform A<SUB>J</SUB> analysis.
    @date Aug 03, 2015
*/

#ifndef PHAJMAKER_H__
#define PHAJMAKER_H__

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "PJTranslator.h"

#ifndef __CINT__
#include "JetAnalyzer.hh"
#include "AjParameters.hh"
#endif // __CINT__

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TString.h"
#include "TChain.h"

class PHCompositeNode;
class RawClusterContainer;
class SvtxTrackMap;
class TF1;

#include <assert.h>
#include <iostream>
#include <cmath>

/* namespace fastjet { */
/*   class PseudoJet; */
/*   class JetDefinition; */
/*   class SISConePlugin;     */
/*   class ClusterSequence; */
/* } */


class PHAJMaker: public SubsysReco
{
  // ------------------------------------------------------
  // ------------------- PUBLIC  --------------------------
  // ------------------------------------------------------
 public:

  /** Standard constructor. Set up analysis parameters.
      \param name: Name
      \param R: jet resolution parameter (radius)
      \param jet_ptmin: minimum jet p<SUB>T</SUB>
      \param jet_ptmax: maximum jet p<SUB>T</SUB>
      \param LeadPtMin: leading jet minimum p<SUB>T</SUB>
      \param SubLeadPtMin: subleading jet minimum p<SUB>T</SUB>
      \param max_track_rap: constituent rapidity cut
      \param PtConsLo: constituent minimum p<SUB>T</SUB>
      \param PtConsHi: constituent maximum p<SUB>T</SUB>
      \param dPhiCut: opening angle for dijet requirement. Accept only  |&phi;1 - &phi;2 - &pi;| < dPhiCut.
   */
  PHAJMaker( const std::string &name = "PHAJMaker",
	     double R = 0.4,
	     double jet_ptmin = 10.0, double jet_ptmax = 100.0,
	     double LeadPtMin = 20.0, double SubLeadPtMin = 10, 
	     double max_track_rap = 1.0, double PtConsLo=0.2, double PtConsHi=2.0,
	     double dPhiCut = 0.4
	     );

  /** Initializer, called ONCE */
  int Init(PHCompositeNode *);

  /** Main analysis routine, called for EVERY EVENT */
  int process_event(PHCompositeNode *);

  /** This little helper is true if there's at least one 10 GeV jet
   **/
  bool Has10Gev;

  // Getters and Setters
  // -------------------
  /// Get jet radius
  inline double GetR ( )                   { return R; };
  /// Set jet radius
  inline void   SetR ( const double newv ) { R=newv;   };

  /// Get minimum jet p<SUB>T</SUB>
  inline double GetJet_ptmin ( )                   { return jet_ptmin; };
  /// Set minimum jet p<SUB>T</SUB>
  inline void   SetJet_ptmin ( const double newv ) { jet_ptmin=newv;   };

  /// Get maximum jet p<SUB>T</SUB>
  inline double GetJet_ptmax ( )                   { return jet_ptmax; };
  /// Set maximum jet p<SUB>T</SUB>
  inline void   SetJet_ptmax ( const double newv ) { jet_ptmax=newv;   };

  /// Get jet rapidity acceptance
  inline double GetMax_rap ( )                   { return max_rap; };
  /// Set jet rapidity acceptance
  inline void   SetMax_rap ( const double newv ) { max_rap=newv;   };

  /// Get ghosted area rapidity cut, should be >= max_rap + 2*R
  inline double GetGhost_maxrap ( )                   { return ghost_maxrap; };
  /// Set ghosted area rapidity cut, should be >= max_rap + 2*R
  inline void   SetGhost_maxrap ( const double newv ) { ghost_maxrap=newv;   };

   /// Get dijet opening angle
  inline double GetDPhiCut ( )                   { return dPhiCut; };
  /// Set dijet opening angle
  inline void   SetDPhiCut ( const double newv ) { dPhiCut=newv;   };  

  /** Standard dtor (empty) */
  virtual ~PHAJMaker();

  /** Dijet asymmetry A<SUB>J</SUB> = &Delta;p<SUB>T</SUB> / &Sigma;p<SUB>T</SUB> */

#ifndef __CINT__
  inline double CalcAj ( std::vector<fastjet::PseudoJet>& jets ){
    if ( jets.size()!=2 ){
      throw ( -1 );
      return -1e10;
    }
    return fabs (( jets.at(0).pt()-jets.at(1).pt() ) / ( jets.at(0).pt()+jets.at(1).pt() ));    
  }

  // Objects will be handed by _reference_! Obviates need for setter
  /// Handle to jet definition
  inline fastjet::JetDefinition& GetJet_def () { return jet_def; }
  /// Handle to selector for low  p<SUB>T</SUB> constituents
  inline fastjet::Selector& GetLoConsSelector () { return slo; }
  /// Handle to selector for high  p<SUB>T</SUB> constituents
  inline fastjet::Selector& GetHiConsSelector () { return shi; }
  
  /// Handle to selector for jet candidates
  inline fastjet::Selector& GetJetSelector () { return sjet; }

  /// Handle to ghosted area specification
  inline fastjet::GhostedAreaSpec& GetArea_spec () { return area_spec; }
  /// Handle to jet area definition
  inline fastjet::AreaDefinition& GetArea_def () { return area_def; }


  /// Handle to JetAnalyzer for high pT constituents
  inline JetAnalyzer* GetJAhi() {return pJAhi; };
  /// Handle to JetAnalyzer for low pT constituents
  inline JetAnalyzer* GetJAlo() {return pJAlo; };

  /// Handle to unaltered clustering result with high pT constituents
  inline std::vector<fastjet::PseudoJet> GetJAhiResult() {return JAhiResult; };
  /// Handle to unaltered clustering result with low pT constituents
  inline std::vector<fastjet::PseudoJet> GetJAloResult() {return JAloResult; };

  /// Handle to high pT constituents
  inline std::vector<fastjet::PseudoJet> GetLoConstituents() {return pLo; };
  /// Handle to low pT constituents
  inline std::vector<fastjet::PseudoJet> GetHiConstituents() {return pHi; };

  
  /// Handle to Dijet result with high pT constituents
  inline std::vector<fastjet::PseudoJet> GetDiJetsHi() {return DiJetsHi; };
  /// Handle to Dijet result with low pT constituents
  inline std::vector<fastjet::PseudoJet> GetDiJetsLo() {return DiJetsLo; };


#endif // __CINT__

  //void save_jets_to_nodetree();


  
  // ------------------------------------------------------
  // ------------------- PRIVATE --------------------------
  // ------------------------------------------------------
  private:			 

  Fun4AllHistoManager* MyHistos;
  TH2D* UnmatchedAJ_hi;
  TH2D* AJ_hi;
  TH2D* AJ_lo;

  double R;              ///< Resolution parameter ("jet radius")
  double jet_ptmin;      ///< minimum jet p<SUB>T</SUB>
  double jet_ptmax;      ///< maximum jet p<SUB>T</SUB>
  double LeadPtMin;      ///< leading jet minimum p<SUB>T</SUB>
  double SubLeadPtMin;   ///< subleading jet minimum p<SUB>T</SUB>
  double max_rap;        ///< jet rapidity acceptance
  double ghost_maxrap;   ///< for ghosted area, should be >= max_rap + 2*R

  double max_track_rap;  ///< constituent rapidity cut
  double PtConsLo;       ///< constituent minimum p<SUB>T</SUB>
  double PtConsHi;       ///< constituent maximum p<SUB>T</SUB>

  double dPhiCut;        ///< opening angle for dijet requirement. Accept only  |&phi;1 - &phi;2 - &pi;| < &Delta;&phi;.

#ifndef __CINT__
  fastjet::JetDefinition jet_def;       ///< jet definition
  fastjet::JetDefinition other_jet_def; ///< jet definition with a different radius

  fastjet::Selector select_track_rap;   ///< constituent rapidity selector
  fastjet::Selector select_lopt;        ///< constituent p<SUB>T</SUB> selector
  fastjet::Selector select_hipt;        ///< constituent p<SUB>T</SUB> selector

  fastjet::Selector slo;                ///< compound selector for low  p<SUB>T</SUB> constituents
  fastjet::Selector shi;                ///< compound selector for high p<SUB>T</SUB> constituents

// Relevant jet candidates
  fastjet::Selector select_jet_rap;        ///< jet rapidity selector
  fastjet::Selector select_jet_pt_min;     ///< jet p<SUB>T</SUB> selector
  fastjet::Selector select_jet_pt_max;     ///< jet p<SUB>T</SUB> selector
  fastjet::Selector sjet;                  ///< compound jet selector

  fastjet::GhostedAreaSpec area_spec;      ///< ghosted area specification
  fastjet::AreaDefinition area_def;        ///< jet area definition

  JetAnalyzer* pJAhi;                      ///< JetAnalyzer object for high pT
  JetAnalyzer* pJAlo;                      ///< JetAnalyzer object for low pT
  JetAnalyzer* pOtherJAlo;                 ///< JetAnalyzer object for low pT with different R

  std::vector<fastjet::PseudoJet> pHi;     ///< High pT constituents
  std::vector<fastjet::PseudoJet> pLo;     ///< Low pT constituents

  std::vector<fastjet::PseudoJet> JAhiResult;  ///< Unaltered clustering result with high pT constituents
  std::vector<fastjet::PseudoJet> JAloResult;  ///< Unaltered clustering result with low pT constituents
  std::vector<fastjet::PseudoJet> OtherJAloResult;  ///< Unaltered clustering result with low pT constituents, different R
  
  std::vector<fastjet::PseudoJet> DiJetsHi;    ///< Dijet result with high pT constituents
  std::vector<fastjet::PseudoJet> DiJetsLo;    ///< Dijet result with low pT constituents
  std::vector<fastjet::PseudoJet> OtherDiJetsLo; ///< Dijet result with low pT constituents and different R

#endif // __CINT__


};

#endif
