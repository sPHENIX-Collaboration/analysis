//----------------------------------------------------------
// Module for flow jet reconstruction in sPHENIX
// J. Orjuela-Koop
// May 5 2015
//----------------------------------------------------------

#include"PHAJMaker.h"
#include"PseudoJetPlus.h"

#include<calobase/RawCluster.h>
#include<calobase/RawClusterContainer.h>

#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxTrack.h>

#include<phool/PHCompositeNode.h>
#include<phool/PHNodeIterator.h>
#include<phool/PHNodeReset.h>
#include<fun4all/getClass.h>

#include <fastjet/JetDefinition.hh>
#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/SISConePlugin.hh>

#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TLorentzVector.h>
#include <TNtuple.h>

#include <cmath>

using namespace std;

typedef std::map<int,TLorentzVector*> tlvmap;

/*
 * Constructor
 */

// Standard ctor
PHAJMaker::PHAJMaker( const std::string &name,
		      double R,
		      double jet_ptmin, double jet_ptmax,
		      double LeadPtMin, double SubLeadPtMin, 
		      double max_track_rap, double PtConsLo, double PtConsHi,
		      double dPhiCut
		      )
  :   SubsysReco(name),
      R(R),
      jet_ptmin(jet_ptmin), jet_ptmax(jet_ptmax),
      LeadPtMin(LeadPtMin), SubLeadPtMin(SubLeadPtMin),
      max_track_rap (max_track_rap), PtConsLo (PtConsLo), PtConsHi (PtConsHi),
      dPhiCut (dPhiCut),
      pJAhi (0), pJAlo(0), pOtherJAlo(0)
{
  // derived rapidity cuts
  // ---------------------
  max_rap      = max_track_rap-R;
  ghost_maxrap = max_rap + 2.0 * R;
  
  // Constituent selectors
  // ---------------------
  select_track_rap = fastjet::SelectorAbsRapMax(max_track_rap);
  select_lopt      = fastjet::SelectorPtMin( PtConsLo );
  select_hipt      = fastjet::SelectorPtMin( PtConsHi );
  
  // Provide but turn off charge selector
  // fastjet::Selector select_track_charge= SelectorChargeRange( -3, 3);
  fastjet::Selector select_track_charge= fastjet::SelectorIdentity();
  slo     = select_track_rap * select_lopt * select_track_charge;
  shi     = select_track_rap * select_hipt * select_track_charge;
  
  // Jet candidate selectors
  // -----------------------
  select_jet_rap     = fastjet::SelectorAbsRapMax(max_rap);
  select_jet_pt_min  = fastjet::SelectorPtMin( jet_ptmin );
  select_jet_pt_max  = fastjet::SelectorPtMax( jet_ptmax );
  sjet = select_jet_rap && select_jet_pt_min && select_jet_pt_max;
  
  // Choose a jet and area definition
  // --------------------------------
  jet_def = fastjet::JetDefinition(fastjet::antikt_algorithm, R);

  // create an area definition for the clustering
  //----------------------------------------------------------
  // ghosts should go up to the acceptance of the detector or
  // (with infinite acceptance) at least 2R beyond the region
  // where you plan to investigate jets.
  area_spec = fastjet::GhostedAreaSpec( ghost_maxrap, AjParameters::ghost_repeat, AjParameters::ghost_area );
  // // DEBUG
  // // area_spec.set_grid_scatter(0);
  // // area_spec.set_pt_scatter(0);
  area_def = fastjet::AreaDefinition(fastjet::active_area_explicit_ghosts, area_spec);

  // DEBUG
  // area_spec = fastjet::VoronoiAreaSpec( 0.9 );
  // area_def = fastjet::AreaDefinition( fastjet::VoronoiAreaSpec( 0.9 ) );
  
  std::cout << " ################################################### " << std::endl;
  std::cout << "Leading jet above " << LeadPtMin << std::endl;
  std::cout << "Sub-Leading jet above " << SubLeadPtMin << std::endl;
  std::cout << "Clustered with " << jet_def.description() << std::endl;
  std::cout << "Area Spec " << area_spec.description() << std::endl;
  std::cout << "Area Def  " << area_def.description() << std::endl;
  std::cout << " ################################################### " << std::endl;

  // std::cout << slo.applies_jet_by_jet() << std::endl;
  // std::cout << shi.applies_jet_by_jet() << std::endl;
  // std::cout << sjet.applies_jet_by_jet() << std::endl;  
  // std::cout << " ################################################### " << std::endl;

}

/*
 * Empty destructor
 */
PHAJMaker::~PHAJMaker()
{
}

/*
 * Initialize module
 */
int PHAJMaker::Init(PHCompositeNode* topNode)
{
  cout << "------PHAJMaker::Init(PHCompositeNode*)------" << endl;

  // Histo Manager
  // -------------
  MyHistos = new Fun4AllHistoManager("MyHistos");
  MyHistos->setOutfileName( "AjHistos.root");
  Fun4AllServer *se = Fun4AllServer::instance();
  se->registerHistoManager(MyHistos);

  // Histos
  // ------
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  TH3::SetDefaultSumw2(true);
    
  UnmatchedAJ_hi = new TH2D( "UnmatchedAJ_hi","Unmatched A_{J} for hard constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );
  AJ_hi = new TH2D( "AJ_hi","A_{J} for hard constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );
  AJ_lo = new TH2D( "AJ_lo","A_{J} for soft constituent jets;A_{J};Refmult;fraction", 40, -0.3, 0.9, 800, -0.5, 799.5 );
  
  MyHistos->registerHisto(UnmatchedAJ_hi);
  MyHistos->registerHisto(AJ_hi);
  MyHistos->registerHisto(AJ_lo);

  return 0;
}

/*
 * Process event
 */
int PHAJMaker::process_event(PHCompositeNode* topNode)
{
  cout << "------PHAJMaker::process_event(PHCompositeNode*)------" << endl;

  //-------------------------------------------------
  // Get Information from Node Tree
  //-------------------------------------------------
  PJContainer* particlesCont = findNode::getClass<PJContainer> (topNode,"PJ");
  std::vector<fastjet::PseudoJet>& particles = particlesCont->data;

  // Classifier, such as Centrality. 
  double EventClassifier=0;

  // We want to hold onto the jetanalyzer objects, so they're created dynamically
  // Need to delete them by hand
  if (pJAhi){    delete pJAhi;    pJAhi=0;  }
  if (pJAlo){    delete pJAlo;    pJAlo=0;  }
  if ( pOtherJAlo ){    delete pOtherJAlo; pOtherJAlo=0;  }


  DiJetsHi.clear();
  DiJetsLo.clear();
  OtherDiJetsLo.clear();
  
  pLo.clear();
  pHi.clear();

  Has10Gev=false;

  // Select particles to perform analysis on
  // ---------------------------------------
  pLo = slo( particles );
  pHi = shi( particles );

  // Background selector
  // -------------------
  // It is unclear to me why, but it leads to segfaults if only a once-initialized member :-/
  fastjet::Selector selector_bkgd = fastjet::SelectorAbsRapMax( max_rap ) * (!fastjet::SelectorNHardest(2));
  // selector_bkgd=fastjet::SelectorAbsRapMax( max_rap );

  // find high constituent pT jets
  // -----------------------------
  pJAhi = new JetAnalyzer( pHi, jet_def ); // NO background subtraction
  JetAnalyzer& JAhi = *pJAhi;
  // std::cout << 0 << std::endl;
  JAhiResult = fastjet::sorted_by_pt( sjet ( JAhi.inclusive_jets() ) );
  // std::cout << 1 << std::endl;
  

  // DEBUG
  // -----  
  cout << " -------------------------------------------------------------------- " << endl;  
  cout << "particles.size() = " << particles.size() << endl;  
  cout << "pLo.size() = " << pLo.size() << endl;  
  cout << "pHi.size() = " << pHi.size() << endl;  
  cout << "JAhiResult.size() = " << JAhiResult.size() << endl;  
  for ( unsigned int i=0; i<JAhiResult.size() ; ++i ){
    cout << "JAhiResult.at(" << i<< ").pT() = " << JAhiResult.at(i).pt() << endl;
  }
  cout << " -------------------------------------------------------------------- " << endl;  


  if ( JAhiResult.size() < 1 )                 {     return 0; }
  if ( JAhiResult.at(0).pt() > 10 )            { Has10Gev=true; }

  if ( JAhiResult.size() < 2 )                 {     return 0; }
  if ( JAhiResult.at(0).pt() < LeadPtMin )     {     return 0; }
  if ( JAhiResult.at(1).pt() < SubLeadPtMin )  {     return 0; }  
  
  // back to back? Answer this question with a selector
  // ---------------------------------------------------
  DiJetsHi = SelectorDijets( dPhiCut ) ( JAhiResult );
  if ( DiJetsHi.size() == 0 ) {
    // std::cout << " NO dijet found" << std::endl;
    return 0;
  }
  assert ( DiJetsHi.size() == 2 && "SelectorDijets returned impossible number of Dijets." );  
  
  // // FOR EMBEDDING: At least one matched to the reference jet?
  // // ---------------------------------------------------------
  // if ( ToMatch ){
  //   if ( !IsMatched( DiJetsHi, *ToMatch, R ) ) return 0;
  // }

  // Calculate Aj and fill histos -- for unmatched high constituent pT di-jets
  // -------------------------------------------------------------------------
  UnmatchedAJ_hi->Fill ( CalcAj( DiJetsHi ), EventClassifier );

  // find corresponding jets with soft constituents
  // ----------------------------------------------
  pJAlo = new JetAnalyzer( pLo, jet_def, area_def, selector_bkgd ); // WITH background subtraction
  JetAnalyzer& JAlo = *pJAlo;
  fastjet::Subtractor* BackgroundSubtractor =  JAlo.GetBackgroundSubtractor();
  JAloResult = fastjet::sorted_by_pt( (*BackgroundSubtractor)( JAlo.inclusive_jets() ) );

  // Using selectors mostly because I can :)
  fastjet::Selector SelectClose = fastjet::SelectorCircle( R );

  // Leading:
  SelectClose.set_reference( DiJetsHi.at(0) );  
  // Pulling apart this one-liner,   DiJetsLo.push_back ( ((SelectClose && fastjet::SelectorNHardest(1)) ( JAloResult )).at(0));
  // More readable and we can catch abnormalities
  std::vector<fastjet::PseudoJet> MatchedToLead = sorted_by_pt(SelectClose( JAloResult ));
  if ( MatchedToLead.size() == 0 ) {
    std::cerr << "PROBLEM: SelectorClose returned no match to leading jet." << std::endl;
    // return 2;
    return 0;
  }
  DiJetsLo.push_back ( MatchedToLead.at(0) );

  SelectClose.set_reference( DiJetsHi.at(1) );
  std::vector<fastjet::PseudoJet> MatchedToSubLead = sorted_by_pt(SelectClose( JAloResult ));
  if ( MatchedToSubLead.size() == 0 ) {
    std::cerr << "PROBLEM: SelectorClose returned no match to sub-leading jet." << std::endl;
    //     return 2;
    return 0;
  }
  DiJetsLo.push_back ( MatchedToSubLead.at(0) );

  if ( fabs(DiJetsLo.at(0).eta())>max_rap ) std:: cerr << "Uh-oh... Lead jet eta = " << DiJetsLo.at(0).eta() << std::endl;
  if ( fabs(DiJetsLo.at(1).eta())>max_rap ) std:: cerr << "Uh-oh... SubLead jet eta = " << DiJetsLo.at(1).eta() << std::endl;
  
  // And we're done! Calculate A_J
  // -----------------------------
  AJ_hi->Fill( CalcAj( DiJetsHi ), EventClassifier );
  AJ_lo->Fill( CalcAj( DiJetsLo ), EventClassifier );

  // DEBUG
  // -----  
  cout << " -------------------------------------------------------------------- " << endl;  
  cout << "DiJetsHi.at(0).pT() = " << DiJetsHi.at(0).pt() << endl;
  cout << "DiJetsHi.at(1).pT() = " << DiJetsHi.at(1).pt() << endl;
  cout << "AJ, hi: " << CalcAj( DiJetsHi ) << endl;
  cout << "DiJetsLo.at(0).pT() = " << DiJetsLo.at(0).pt() << endl;
  cout << "DiJetsLo.at(1).pT() = " << DiJetsLo.at(1).pt() << endl;
  cout << "AJ, lo: " << CalcAj( DiJetsLo ) << endl;
  cout << " -------------------------------------------------------------------- " << endl;  



  return 0;
}

/*
 * Write jets to node tree
 */
/*
todo - we are working on a jet object
void PHAJMaker::save_jets_to_nodetree()
{
  flow_jet_container = new PHPyJetContainerV2();
}
*/
