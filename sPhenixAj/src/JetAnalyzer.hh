/** @file JetAnalyzer.hh
    @author Kauder:Kolja
    @version Revision 0.1
    @brief Light FastJet wrapper for Heavy Ion analysis
    program file and the functions in that file.
    @details Light FastJet wrapper for Heavy Ion analysis
    program file and the functions in that file.
    @date Mar 03, 2015
*/

/**
   @mainpage
   Light, unified wrapper for Fastjet 3.x to simplify common Heavy Ion tasks. 

   Design decisions:
   - KISS. Please do event selection and track cleanup in the calling macro. 
   Provide this class with PseudoJets and try to stick to the FastJet way and c++ stl functionality as much as possible.
   - Derived from fastjet::ClusterSequenceArea.
   - Typical heavy ion workflow: 
     - Fill with an event
     - Cluster
     - Do analysis. This may require minimal extra data (e.g. A_J) or more information (e.g. jet-h)
     - Tweak the jets (using selectors, transformers, filters, etc.), or the underlying data a bit and repeat.
   - FastJet Selectors are really meant to operate on the _result_ of a clustering, but can be used 
   on the original constituents as well.
   - Therefore, we will keep a version of the original event, and request a constituent selector to choose on which particles 
   to actually perform the analysis. A result selector can be used via default fastjet machinery.

   Style:
   - Doxygen comments should go in the header files.
   - Please name your variables properly. 
   - Comment a lot.
   - Please try to keep track of changes. Use version control.
   - Please keep indentation clean. I'm using some combination of [K&R and GNU style](http://en.wikipedia.org/wiki/Indent_style#Styles), 
   but anything clean works. If it gets messed up, open it in emacs, mark everything, and hit tab once.
*/


/*
  Author self-assessment statement: It gets the job done for A_J.
*/

#ifndef JETANALYZER_H
#define  JETANALYZER_H

// Includes and namespaces
//#include "FastJet3.h"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/ClusterSequencePassiveArea.hh"
#include "fastjet/ClusterSequenceActiveArea.hh"
#include "fastjet/ClusterSequenceActiveAreaExplicitGhosts.hh"
#include "fastjet/Selector.hh"

#include <TLorentzVector.h>
#include <TClonesArray.h>

#include "fastjet/tools/JetMedianBackgroundEstimator.hh"
#include "fastjet/tools/Subtractor.hh"
#include "fastjet/tools/Filter.hh"
#include "fastjet/FunctionOfPseudoJet.hh"

#include <iostream>
#include <string>
#include <sstream>

// For pairs and sorting in the dijet finding
#include <utility>
#include <algorithm>


/** The main class.
 */
class JetAnalyzer : public fastjet::ClusterSequenceArea {

private :  
  /** Keep a copy of the original constituents.
      In principle, they should be accessible via this->jets(),
      but there is extra space allocated in that vector and I'd rather not muck around with it.      
   */
  std::vector<fastjet::PseudoJet>& OrigParticles;

  /** Determined by whether we have an area definition */
  bool CanDoBackground;
  
  /**  For background subtraction.
       Pointer for now, so that it can be 0 if unused.
       Default specs and areas will be supplied.       
  */
  fastjet::JetMedianBackgroundEstimator* bkgd_estimator;
  /** Subtractor */
  fastjet::Subtractor* bkgd_subtractor;

  /** Background jet cut */
  fastjet::Selector selector_bkgd;
  /**  Background jet definiton */
  fastjet::JetDefinition* jet_def_bkgd;  
  /**  Background area definiton */
  fastjet::AreaDefinition* area_def_bkgd;

public :
  // ------------
  // Constructors
  // ------------
  /** Standard constructor. 
      Pass through to Fastjet and set up internal background estimator with default values.
      Note that initialization as ClusterSequenceArea( pHi, jet_def, 0 ) works fine and skips the 
      area computation. Instead, we provide a second ctor and use this information to 
      determine whether to provide background capability.
      \param InOrigParticles is the full set of constituent candidates. Passed by reference!
      \param JetDef is a fastjet::JetDefinition for the clustering. Passed by reference!
      \param AreaDef is a fastjet::AreaDefinition for the clustering
      \param selector_bkgd is a fastjet::Selector for background subtraction
   */
  JetAnalyzer ( std::vector<fastjet::PseudoJet>& InOrigParticles, fastjet::JetDefinition& JetDef, fastjet::AreaDefinition& AreaDef,
		fastjet::Selector selector_bkgd=fastjet::SelectorAbsRapMax( 0.6 ) * (!fastjet::SelectorNHardest(2)) );

  /** Use as ClusterSequence, without area computation and without BG options
      \param InOrigParticles is the full set of constituent candidates. handed by reference!
      \param JetDef is a fastjet::JetDefinition for the clustering
   */
  JetAnalyzer ( std::vector<fastjet::PseudoJet>& InOrigParticles, fastjet::JetDefinition& JetDef );
  
  /** Destructor. Take care of all the objects created with new.
   */
  ~JetAnalyzer (){
    if ( area_def_bkgd )  {
      delete area_def_bkgd;
      area_def_bkgd = 0 ;
    }
    if ( jet_def_bkgd )  {
      delete jet_def_bkgd;
      jet_def_bkgd = 0 ;
    }
    if ( bkgd_estimator ) {
      delete bkgd_estimator;
      bkgd_estimator = 0 ;
    }
    if ( bkgd_subtractor ) {
      delete bkgd_subtractor;
      bkgd_subtractor = 0 ;
    }
    
  };
  

  // ----------------
  // Analysis methods
  // ----------------

  // -------------
  // Other Methods
  // -------------
  /** Background functionality.
      Currently, the jet definition is hard-coded to fastjet::kt_algorithm, jet_def().R(),
      and the area definition is computed internally. Expand and modify as needed.
   */
  fastjet::Subtractor* GetBackgroundSubtractor();
  /**
     Handle to BackgroundEstimator()
   */
  fastjet::JetMedianBackgroundEstimator* GetBackgroundEstimator() { return bkgd_estimator; };
  // ---------
  // Operators 
  // ---------
  // May eventually be added.
  // bool operator==(JetAnalyzer& rhs);

  // -------
  // Helpers
  // -------
  /** The nth version of this important constant.
   */
  static const double pi;

  /** Returns an angle between -pi and pi
   */
  static double phimod2pi ( double phi );  
  
};

// NOT part of the class!
// =============================================================================
/** Dijet finding as a Selector. Fashioned after fastjet::SW_NHardest.
    Searches for and returns dijet pairs within |&phi;1 - &phi;2 - &pi;| < &Delta;&phi;.
    returns 0 if no pair is found.
    In the current implementation, only the top two jets are compared.

    NOTE: Could also use something like
    \code
    Selector sel = SelectorCircle( dPhi );
    sel.set_reference( -jet1 ); // Pseudocode
    vector<PseudoJet> jets_near_MinusJet1 = sel(jets);
    \endcode
 */
class SelectorDijetWorker : public fastjet::SelectorWorker{
public:
  /** Standard constructor
      \param dPhi: Opening angle, searching for |&phi;1 - &phi;2 - &pi;| < &Delta;&phi;
  */
  SelectorDijetWorker( double dPhi )
    : dPhi(dPhi){};
  
  /// the selector's description
  std::string description() const{
    std::ostringstream oss;
    oss << "Searches for and returns dijet pairs within |phi1 - phi2 - pi| < " << dPhi;    
    return oss.str() ; 
  };

  /// Returns false, we need a jet ensemble.
  bool applies_jet_by_jet() const { return false;};

  /// Returns false, we need a jet ensemble.
  bool pass(const fastjet::PseudoJet& pj) const{
    if (!applies_jet_by_jet())
      throw ( std::string("Cannot apply this selector worker to an individual jet") );
    return false;
  };

  /// The relevant method
  void terminator(std::vector<const fastjet::PseudoJet *> & jets) const;
 
private:
  const double dPhi;                   ///< Opening angle, searching for |&phi;1 - &phi;2 - &pi;| < &Delta;&phi;  
};

/** Helper for sorting pairs by second argument
 */
struct sort_IntDoubleByDouble {
  /// returns left.second < right.second
  bool operator()(const std::pair<int,double> &left, const std::pair<int,double> &right) {
    return left.second < right.second;
  }
};
/** Determines whether two vector sets are matched 1 to 1.
    Actual Dijet selector
    \param dPhi: Dijet acceptance angle &Delta;&phi;
 */
fastjet::Selector SelectorDijets( const double dPhi=0.4);



// =============================================================================
/** Determines whether two vector sets are matched 1 to 1.
    Could return something like lists of matches, but 
    for now it's using a primitive 1-to-1 matching.
    Enforcing 1-to-1 to avoid pathologies.
    For dijet A_J, the implementation is already overkill, but I want to
    lay the groundwork for more complex tasks
 */
bool IsMatched ( const std::vector<fastjet::PseudoJet>& jetset1, const std::vector<fastjet::PseudoJet>& jetset2, const double Rmax );

/** Check if one of the jets in jetset1 matches the reference. 
    TODO: Could use this in the vector-vector version
 */
bool IsMatched ( const std::vector<fastjet::PseudoJet>& jetset1, const fastjet::PseudoJet& reference, const double Rmax );

/** Check if jet and jet2 are matched 
    TODO: Could use this in the vector versions
 */
bool IsMatched ( const fastjet::PseudoJet& jet1, const fastjet::PseudoJet& jet2, const double Rmax );



// =============================================================================
/** The best way to interface vector<PseudoJet> with ROOT seems to be via 
    TClonesArray<TLorentzVector>, so we'll provide some ways to go back and forth between them.
*/
// ------------------------------------------------------------------------
TLorentzVector MakeTLorentzVector ( const fastjet::PseudoJet& pj );
// ------------------------------------------------------------------------

// // =============================================================================
// Experimental, not working.
// Should work if this class is _instantiated_ and then the instance is used.
// That's not really the usage scenario.
// class MakeTLorentzVector : public fastjet::FunctionOfPseudoJet<TLorentzVector>{
//   TLorentzVector result(const fastjet::PseudoJet &pj) const{
//     return TLorentzVector( pj.px(), pj.py(), pj.pz(), pj.E() );
//   };
//   std::string description() const {return "Translates Pseudojet into ROOT TLorentzVector";}
//   TLorentzVector operator()(const fastjet::PseudoJet & pj) const {return result(pj);};
// };


// =============================================================================
/** The best way to interface vector<PseudoJet> with ROOT seems to be via 
    TClonesArray<TLorentzVector>, so we'll provide some ways to go back and forth between them.
    Pretty redundant, PseudoJet is a smart class and does all the work.
*/
fastjet::PseudoJet MakePseudoJet ( const TLorentzVector* const lv );
// =============================================================================
// =============================================================================

/** Simple UserInfo. Derived from PseudoJet::UserInfoBase. 
    Currently just providing charge, please add as appropriate.
    Based on 09-user__info_8cc_source.cc
 */
class JetAnalysisUserInfo: public fastjet::PseudoJet::UserInfoBase {
public:
  /// Standard Constructor
  JetAnalysisUserInfo(int quarkcharge=-999) :  quarkcharge( quarkcharge )  {};
  
  /// Charge in units of e/3
  int GetQuarkCharge() const { return quarkcharge; };

private:
  const int quarkcharge;   ///< Charge in units of e/3
};

// =============================================================================
/** This shows how we can build a Selector that uses the user-defined
    information to select particles by charge.
    Charge is in units of e/3!
    
    To create a user-defined Selector, the first step is to
    create its associated "worker" class, i.e. to derive a class from
    SelectorWorker. Then (see below), we just write a function
    that creates a Selector with the appropriate worker class.
*/
class SelectorChargeWorker : public fastjet::SelectorWorker{
public:
  /** Standard constructor
      \param cmin: inclusive lower bound
      \param cmax: inclusive upper bound
  */
  SelectorChargeWorker( const int cmin, const int cmax)  : cmin(cmin), cmax(cmax) {};
  
  /// the selector's description
  std::string description() const{
    std::ostringstream oss;
    oss.str("");
    oss << cmin << " <= quark charge <= " << cmax;    
    return oss.str() ; 
  };
  
  /// keeps the ones that have cmin <= quarkcharge <= cmax
  bool pass(const fastjet::PseudoJet &p) const{
    const int & quarkcharge = p.user_info<JetAnalysisUserInfo>().GetQuarkCharge();
    return (quarkcharge >= cmin) &&  (quarkcharge<= cmax);
  };

private: 
  const int cmin; ///< inclusive lower bound
  const int cmax; ///< inclusive upper bound
  
};
// =============================================================================
/** the function that allows to write simply
    
    Selector sel = SelectorChargeRange( cmin, cmax );
*/
fastjet::Selector SelectorChargeRange( const int cmin=-999, const int cmax=999);

// =============================================================================
// =============================================================================
// =============================================================================
/** Helper to get an enum from a string
    we'll allow some more generous spellings and abbreviations
*/
fastjet::JetAlgorithm AlgoFromString( std::string s);


#endif // JETANALYZER_H
