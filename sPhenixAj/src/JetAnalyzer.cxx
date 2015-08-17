/** @file JetAnalyzer.cxx
    @author Kauder:Kolja
    @version Revision 0.1
    @brief Light FastJet wrapper for Heavy Ion analysis
    program file and the functions in that file.
    @details
    @date Mar 04, 2015
*/

#include "JetAnalyzer.hh"

// ------------
// Constructors
// ------------

// ------------------------------------------------------------------------
// Standard ctor
JetAnalyzer::JetAnalyzer( std::vector<fastjet::PseudoJet>& InOrigParticles, fastjet::JetDefinition& JetDef,
			  fastjet::AreaDefinition& AreaDef, fastjet::Selector selector_bkgd )
  : fastjet::ClusterSequenceArea ( InOrigParticles, JetDef, AreaDef ), OrigParticles ( InOrigParticles ),
    selector_bkgd (selector_bkgd)
{
  
  CanDoBackground=true;
	 
  // this should not be necessary :-/
  bkgd_subtractor=0;
  jet_def_bkgd = 0;
  bkgd_estimator = 0;
  bkgd_subtractor = 0;
  area_def_bkgd = new fastjet::AreaDefinition( AreaDef );
  // DEBUG
  // area_def_bkgd = new fastjet::AreaDefinition( fastjet::VoronoiAreaSpec( 1.0 ) );

  // // std::cout << "Area Spec " << area_spec.description() << std::endl;
  // std::cout << "Area Def given: " << std::endl << AreaDef.description() << std::endl;
  // std::cout << "Area Def Bkgd:  " << std::endl << area_def_bkgd->description() << std::endl;
  // std::cout << "This:  " << std::endl << this->area_def().description() << std::endl;
      
}
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
// Standard ctor for use as ClusterSequence
JetAnalyzer::JetAnalyzer( std::vector<fastjet::PseudoJet>& InOrigParticles, fastjet::JetDefinition& JetDef )
  : fastjet::ClusterSequenceArea ( InOrigParticles, JetDef, 0 ), OrigParticles ( InOrigParticles )
{
  CanDoBackground=false;

  // this should not be necessary :-/
  bkgd_subtractor=0;
  area_def_bkgd=0;
  jet_def_bkgd = 0;
  bkgd_estimator = 0;
  bkgd_subtractor = 0;

}
// ------------------------------------------------------------------------



// ------------
// Methods
// ------------

// ------------------------------------------------------------------------
// Background functionality
fastjet::Subtractor* JetAnalyzer::GetBackgroundSubtractor(){
  if ( !CanDoBackground ) {
    // throw std::string("Should not be called unless we can actually do background subtraction.");
    return 0;
  }

  // // Area
  // // ----
  // // Construct from jet definition and radius
  // double ymin, ymax;
  // selector_bkgd.get_rapidity_extent (ymin, ymax);
  // assert ( !std::isinf(ymin) && !std::isinf(ymax)   && "Can't handle unrestricted area." );
  // assert ( ymin == -ymax                            && "Can't construct ghosted area for asymmetric rapidity cuts.");

  // double ghost_maxrap = ymax + 2.0*jet_def().R();  
  // area_def_bkgd = new fastjet::AreaDefinition (fastjet::active_area_explicit_ghosts, 
  // 					       fastjet::GhostedAreaSpec( ghost_maxrap ));

  // Background jet definition
  // -------------------------
  jet_def_bkgd = new fastjet::JetDefinition (fastjet::kt_algorithm, jet_def().R());
  
  // Estimator and subtractor
  // ------------------------
  // std::cout << area_def_bkgd->description() << std::endl;
  // std::cout << selector_bkgd.description() << std::endl; 
  // std::cout << "selector.has_finite_area() = "<< selector_bkgd.has_finite_area() << std::endl; 
  bkgd_estimator  = new fastjet::JetMedianBackgroundEstimator(selector_bkgd, *jet_def_bkgd, *area_def_bkgd);
  bkgd_subtractor = new fastjet::Subtractor(bkgd_estimator);

  // since FastJet 3.1.0, rho_m is supported natively in background
  // estimation (both JetMedianBackgroundEstimator and
  // GridMedianBackgroundEstimator).
  //
  // For backward-compatibility reasons its use is by default switched off
  // (as is the enforcement of m>0 for the subtracted jets). The
  // following 2 lines of code switch these on. They are strongly
  // recommended and should become the default in future versions of
  // FastJet.
#if FASTJET_VERSION_NUMBER >= 30100 
  bkgd_subtractor->set_use_rho_m(true);
  bkgd_subtractor->set_safe_mass(true);
#endif

  bkgd_estimator->set_particles( OrigParticles );
  
  // std::cout << OrigParticles.size() <<  std::endl;
  // std::cout << "    rho     = " << bkgd_estimator->rho()   << std::endl;
  // std::cout << "    sigma   = " << bkgd_estimator->sigma() << std::endl;
  
  return bkgd_subtractor;
}
// ------------------------------------------------------------------------
// ----------------
// Analysis methods
// ----------------
// Dijet finding as a Selector
void SelectorDijetWorker::terminator(std::vector<const fastjet::PseudoJet *> & jets) const{
  // For each jet that does not pass the cuts, this routine sets the 
  // pointer to 0. 

  // Save these ones from being zeroed
  int i0 = -1;
  int i1 = -1;
  
  // For dPhi<0 or too few jets, we don't even need to try
  if ( dPhi>=0 && jets.size() >= 2 ) {
    // need to allow unsorted input
    // So find the two largest pT jets. Sigh.

    // Pair negative pT^2 with the index
    // That takes care of pT sign and of stl::sorting order
    std::vector< std::pair<int,double> > IndexPt;
    for (unsigned int i=0; i<jets.size(); ++i ){
      // Note that jets.at(i) might be NULL
      IndexPt.push_back( std::pair<int,double>( i, jets.at(i) ? -jets.at(i)->perp2() : 0.0) );
    }
    
    // and sort
    std::sort(IndexPt.begin(), IndexPt.end(), sort_IntDoubleByDouble());

    // We only accept top two jets
    // Now Index 0 and 1 should point to the largest ones
    // Tentatively set them to be keepers
    i0 = IndexPt.at(0).first;
    i1 = IndexPt.at(1).first;

    // clarified for readability
    double phi0 = jets.at(i0)->phi();
    double phi1 = jets.at(i1)->phi();

    if ( ! (fabs ( JetAnalyzer::phimod2pi( phi0 - phi1 - JetAnalyzer::pi) ) < dPhi) ) {
      // Nope, don't save them after all
      i0=i1=-1;
    }
  } else {
    // std::cout << "  -------------------- dPhi<0 or Not enough jets" << std::endl;
  }

  // Zero out everything else
  for ( int i=0; i< int( jets.size() ) ; ++i ){
    if ( i==i0 ) continue;
    if ( i==i1 ) continue;
    jets.at(i)= NULL;
  }
  
  return;
}
fastjet::Selector SelectorDijets( const double dPhi ){
  return fastjet::Selector(new SelectorDijetWorker( dPhi ) );
}

// ------------------------------------------------------------------------

// -------
// Helpers
// -------
double JetAnalyzer::phimod2pi ( double phi ){
  while ( phi <-pi ) phi += 2.0*pi;
  while ( phi > pi ) phi -= 2.0*pi;
  return phi;
}

// ------------------------------------------------------------------------

// -------------------
// Getters and Setters
// -------------------

// ------------------------------------------------------------------------


// --------------
// Other funtions
// --------------
// ------------------------------------------------------------------------
bool IsMatched ( const std::vector<fastjet::PseudoJet>& jetset1, const std::vector<fastjet::PseudoJet>& jetset2, const double Rmax )
{
  // could also return lists of matches or so
  // As it is, I'm using a primitive 1-to-1 matching
  // Enforcing 1 to 1 to avoid stupid pathologies
  // For dijet A_J, the following is already overkill, but I want to
  // lay the groundwork for more complex tasks
  if ( jetset1.size() != jetset2.size() ) return false;

  std::vector<fastjet::PseudoJet> j2copy = jetset2; // cause we're going to destroy it
  for ( int j1 = 0; j1 < int( jetset1.size() ) ; ++j1 ){
    bool j1matched=false;
    for ( std::vector<fastjet::PseudoJet>::iterator pj2=j2copy.begin() ; pj2!=j2copy.end(); ++pj2 ){
      if ( jetset1.at(j1).delta_R( *pj2 ) < Rmax ){
	j1matched=true;

	// std::cout << jetset1.at(j1).delta_R( *pj2 ) << std::endl; // DEBUG
	// could pop it into a result list, like so:
	// result.push_back (*pj2);

	// remove from jetset2
	j2copy.erase( pj2 );

	// done with this one
	break;
      } // if (R<max)      
    } // for jet2
    // not matched? bad
    if ( !j1matched ) return false;
  } // for jet1

  // Now we should have cleanly removed all jets from set 2;
  if ( j2copy.size() >0 ) return false;

  return true;  
}
// ------------------------------------------------------------------------
bool IsMatched ( const std::vector<fastjet::PseudoJet>& jetset1, const fastjet::PseudoJet& reference, const double Rmax )
{  
  for ( int j1 = 0; j1 < int (jetset1.size()) ; ++ j1 ){
    if ( jetset1.at(j1).delta_R( reference ) < Rmax ){
      return true;
    } // if (R<max)      
  } // for jet1

  return false;  
}

// ------------------------------------------------------------------------
bool IsMatched ( const fastjet::PseudoJet& jet1, const fastjet::PseudoJet& jet2, const double Rmax )
{  
  return jet1.delta_R( jet2 ) < Rmax;
}

// ------------------------------------------------------------------------
TLorentzVector MakeTLorentzVector ( const fastjet::PseudoJet& pj ){
  return TLorentzVector( pj.px(), pj.py(), pj.pz(), pj.E() );
  // Below could be slightly faster but circumvents encapsulation.
  // four_mom() is a valarray<double> which is NOT guaranteed to be contiguous
  // return TLorentzVector( (Double_t*) &pj.four_mom()[0] );
}
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
fastjet::PseudoJet MakePseudoJet ( const TLorentzVector* const lv ){
  return fastjet::PseudoJet ( *lv );
}


// ------------------------------------------------------------------------
// the function that allows to write simply
// Selector sel = SelectorChargeRange( cmin, cmax );
fastjet::Selector SelectorChargeRange( const int cmin, const int cmax){
  return fastjet::Selector(new SelectorChargeWorker(cmin, cmax));
};
// ------------------------------------------------------------------------
// Helper to get an enum from a string    
fastjet::JetAlgorithm AlgoFromString( std::string s){
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);  

  if ( s.substr(0,2) == "kt" )       return fastjet::kt_algorithm;
  if ( s.substr(0,6) == "antikt" )   return fastjet::antikt_algorithm;
  if ( s.substr(0,6) == "cambri" )   return fastjet::cambridge_algorithm;
  
  return fastjet::undefined_jet_algorithm;

}
// ------------------------------------------------------------------------
const double JetAnalyzer::pi = 3.141592653589793238462643383279502884;
