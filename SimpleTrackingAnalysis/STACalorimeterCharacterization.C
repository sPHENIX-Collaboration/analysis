#include "STACalorimeterCharacterization.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
//#include <g4hough/PHG4HoughTransform.h> // CAUSES ERRORS DUE TO VertexFinder.h FAILING TO FIND <Eigen/LU>


#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>

#include <g4hough/SvtxVertexMap.h>
#include <g4hough/SvtxVertex.h>
#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxTrack.h>

#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxTrackEval.h>
#include <g4eval/SvtxVertexEval.h>
#include <g4eval/SvtxTruthEval.h>

// --- common to all calorimeters
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <g4eval/CaloEvalStack.h>
#include <g4eval/CaloRawClusterEval.h>
#include <g4eval/CaloRawTowerEval.h>

#include <TH1D.h>
#include <TH2D.h>
#include <TProfile2D.h>

#include <iostream>
//#include <cassert>

using namespace std;



STACalorimeterCharacterization::STACalorimeterCharacterization(const string &name) : SubsysReco(name)
{
  cout << "Class constructor called " << endl;
  nevents = 0;
  nerrors = 0;
  nwarnings = 0;
  nlayers = 7;
  verbosity = 0;
  magneticfield = 1.4; // default is 1.5 but Mike's tracking stuff is using 1.4 for now...
  //docalocuts = false;
}



int STACalorimeterCharacterization::Init(PHCompositeNode *topNode)
{

  // --------------------------------------------------------------------------------
  // --- This is the class initialization method
  // --- Get a pointer to the Fun4All server to register histograms
  // --- Histograms are declared in class header file
  // --- Histograms are instantiated here and then registered with the Fun4All server
  // --------------------------------------------------------------------------------


  // --- get instance of
  Fun4AllServer *se = Fun4AllServer::instance();






  // --- some basic calorimeter performance histograms

  _energy_ratio_emc = new TH2D("energy_ratio_emc", "", 300,0.0,30.0, 100,0.0,2.0);
  _energy_ratio_hci = new TH2D("energy_ratio_hci", "", 300,0.0,30.0, 100,0.0,2.0);
  _energy_ratio_hco = new TH2D("energy_ratio_hco", "", 300,0.0,30.0, 100,0.0,2.0);
  _energy_ratio_hct = new TH2D("energy_ratio_hct", "", 300,0.0,30.0, 100,0.0,2.0);
  _energy_ratio_tot_dumb = new TH2D("energy_ratio_tot_dumb", "", 300,0.0,30.0, 100,0.0,2.0);
  _energy_ratio_tot_smart = new TH2D("energy_ratio_tot_smart", "", 300,0.0,30.0, 100,0.0,2.0);
  se->registerHisto(_energy_ratio_emc);
  se->registerHisto(_energy_ratio_hci);
  se->registerHisto(_energy_ratio_hco);
  se->registerHisto(_energy_ratio_hct);
  se->registerHisto(_energy_ratio_tot_dumb);
  se->registerHisto(_energy_ratio_tot_smart);

  _energy_ratio_elb_emc = new TH2D("energy_ratio_elb_emc", "", 300,0.0,30.0, 100,0.0,2.0);
  _energy_ratio_elb_hci = new TH2D("energy_ratio_elb_hci", "", 300,0.0,30.0, 100,0.0,2.0);
  _energy_ratio_elb_hco = new TH2D("energy_ratio_elb_hco", "", 300,0.0,30.0, 100,0.0,2.0);
  _energy_ratio_elb_hct = new TH2D("energy_ratio_elb_hct", "", 300,0.0,30.0, 100,0.0,2.0);
  _energy_ratio_elb_tot_dumb = new TH2D("energy_ratio_elb_tot_dumb", "", 300,0.0,30.0, 100,0.0,2.0);
  _energy_ratio_elb_tot_smart = new TH2D("energy_ratio_elb_tot_smart", "", 300,0.0,30.0, 100,0.0,2.0);
  se->registerHisto(_energy_ratio_elb_emc);
  se->registerHisto(_energy_ratio_elb_hci);
  se->registerHisto(_energy_ratio_elb_hco);
  se->registerHisto(_energy_ratio_elb_hct);
  se->registerHisto(_energy_ratio_elb_tot_dumb);
  se->registerHisto(_energy_ratio_elb_tot_smart);

  _energy_ratio_eub_emc = new TH2D("energy_ratio_eub_emc", "", 300,0.0,30.0, 100,0.0,2.0);
  _energy_ratio_eub_hci = new TH2D("energy_ratio_eub_hci", "", 300,0.0,30.0, 100,0.0,2.0);
  _energy_ratio_eub_hco = new TH2D("energy_ratio_eub_hco", "", 300,0.0,30.0, 100,0.0,2.0);
  _energy_ratio_eub_hct = new TH2D("energy_ratio_eub_hct", "", 300,0.0,30.0, 100,0.0,2.0);
  _energy_ratio_eub_tot_dumb = new TH2D("energy_ratio_eub_tot_dumb", "", 300,0.0,30.0, 100,0.0,2.0);
  _energy_ratio_eub_tot_smart = new TH2D("energy_ratio_eub_tot_smart", "", 300,0.0,30.0, 100,0.0,2.0);
  se->registerHisto(_energy_ratio_eub_emc);
  se->registerHisto(_energy_ratio_eub_hci);
  se->registerHisto(_energy_ratio_eub_hco);
  se->registerHisto(_energy_ratio_eub_hct);
  se->registerHisto(_energy_ratio_eub_tot_dumb);
  se->registerHisto(_energy_ratio_eub_tot_smart);

  _energy_dphi_emc = new TH2D("energy_dphi_emc", "", 300,0.0,30.0, 100,-1.0,1.0);
  _energy_dphi_hci = new TH2D("energy_dphi_hci", "", 300,0.0,30.0, 100,-1.0,1.0);
  _energy_dphi_hco = new TH2D("energy_dphi_hco", "", 300,0.0,30.0, 100,-1.0,1.0);
  se->registerHisto(_energy_dphi_emc);
  se->registerHisto(_energy_dphi_hci);
  se->registerHisto(_energy_dphi_hco);

  _energy_deta_emc = new TH2D("energy_deta_emc", "", 300,0.0,30.0, 100,-1.0,1.0);
  _energy_deta_hci = new TH2D("energy_deta_hci", "", 300,0.0,30.0, 100,-1.0,1.0);
  _energy_deta_hco = new TH2D("energy_deta_hco", "", 300,0.0,30.0, 100,-1.0,1.0);
  se->registerHisto(_energy_deta_emc);
  se->registerHisto(_energy_deta_hci);
  se->registerHisto(_energy_deta_hco);



  _towers_3x3_emc = new TProfile2D("towers_3x3_emc", "", 3,-1.5,1.5, 3,-1.5,1.5, 0.0,9998.0,"");
  _towers_5x5_emc = new TProfile2D("towers_5x5_emc", "", 5,-2.5,2.5, 5,-2.5,2.5, 0.0,9998.0,"");
  _towers_7x7_emc = new TProfile2D("towers_7x7_emc", "", 7,-3.5,3.5, 7,-3.5,3.5, 0.0,9998.0,"");
  _towers_9x9_emc = new TProfile2D("towers_9x9_emc", "", 9,-4.5,4.5, 9,-4.5,4.5, 0.0,9998.0,"");
  se->registerHisto(_towers_3x3_emc);
  se->registerHisto(_towers_5x5_emc);
  se->registerHisto(_towers_7x7_emc);
  se->registerHisto(_towers_9x9_emc);

  for ( int i = 0; i < 10; ++i )
    {
      _towersum_energy_emc[i] = new TH2D(Form("towersum_energy_emc_%d",i), "", 300,0.0,30.0, 100,0.0,2.0);
      _tower_energy_emc[i] = new TH2D(Form("tower_energy_emc_%d",i), "", 300,0.0,30.0, 100,0.0,2.0);
      se->registerHisto(_towersum_energy_emc[i]);
      se->registerHisto(_tower_energy_emc[i]);
    }

  _towers_3x3_hci = new TProfile2D("towers_3x3_hci", "", 3,-1.5,1.5, 3,-1.5,1.5, 0.0,9998.0,"");
  _towers_5x5_hci = new TProfile2D("towers_5x5_hci", "", 5,-2.5,2.5, 5,-2.5,2.5, 0.0,9998.0,"");
  _towers_7x7_hci = new TProfile2D("towers_7x7_hci", "", 7,-3.5,3.5, 7,-3.5,3.5, 0.0,9998.0,"");
  _towers_9x9_hci = new TProfile2D("towers_9x9_hci", "", 9,-4.5,4.5, 9,-4.5,4.5, 0.0,9998.0,"");
  se->registerHisto(_towers_3x3_hci);
  se->registerHisto(_towers_5x5_hci);
  se->registerHisto(_towers_7x7_hci);
  se->registerHisto(_towers_9x9_hci);

  for ( int i = 0; i < 10; ++i )
    {
      _towersum_energy_hci[i] = new TH2D(Form("towersum_energy_hci_%d",i), "", 300,0.0,30.0, 100,0.0,2.0);
      _tower_energy_hci[i] = new TH2D(Form("tower_energy_hci_%d",i), "", 300,0.0,30.0, 100,0.0,2.0);
      se->registerHisto(_towersum_energy_hci[i]);
      se->registerHisto(_tower_energy_hci[i]);
    }

  _towers_3x3_hco = new TProfile2D("towers_3x3_hco", "", 3,-1.5,1.5, 3,-1.5,1.5, 0.0,9998.0,"");
  _towers_5x5_hco = new TProfile2D("towers_5x5_hco", "", 5,-2.5,2.5, 5,-2.5,2.5, 0.0,9998.0,"");
  _towers_7x7_hco = new TProfile2D("towers_7x7_hco", "", 7,-3.5,3.5, 7,-3.5,3.5, 0.0,9998.0,"");
  _towers_9x9_hco = new TProfile2D("towers_9x9_hco", "", 9,-4.5,4.5, 9,-4.5,4.5, 0.0,9998.0,"");

  for ( int i = 0; i < 10; ++i )
    {
      _towersum_energy_hco[i] = new TH2D(Form("towersum_energy_hco_%d",i), "", 300,0.0,30.0, 100,0.0,2.0);
      _tower_energy_hco[i] = new TH2D(Form("tower_energy_hco_%d",i), "", 300,0.0,30.0, 100,0.0,2.0);
      se->registerHisto(_towersum_energy_hco[i]);
      se->registerHisto(_tower_energy_hco[i]);
    }
  se->registerHisto(_towers_3x3_hco);
  se->registerHisto(_towers_5x5_hco);
  se->registerHisto(_towers_7x7_hco);
  se->registerHisto(_towers_9x9_hco);



  return 0;

}



int STACalorimeterCharacterization::process_event(PHCompositeNode *topNode)
{

  // --- This is the class process_event method
  // --- This is where the bulk of the analysis is done
  // --- Here we get the various data nodes we need to do the analysis
  // --- Then we use variables (accessed through class methods) to perform calculations

  if ( verbosity > -1 )
    {
      cout << endl;
      cout << "------------------------------------------------------------------------------------" << endl;
      cout << "Now processing event number " << nevents << endl; // would be good to add verbosity switch
    }

  ++nevents; // You may as youtself, why ++nevents (pre-increment) rather
  // than nevents++ (post-increment)?  The short answer is performance.
  // For simple types it probably doesn't matter, but it can really help
  // for complex types (like the iterators below).


  // --- Truth level information
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
  if ( !truthinfo )
    {
      cerr << PHWHERE << " ERROR: Can't find G4TruthInfo" << endl;
      exit(-1);
    }

  // --- SvtxTrackMap
  SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
  if ( !trackmap )
    {
      cerr << PHWHERE << " ERROR: Can't find SvtxTrackMap" << endl;
      exit(-1);
    }

  // --- SvtxVertexMap
  SvtxVertexMap* vertexmap = findNode::getClass<SvtxVertexMap>(topNode,"SvtxVertexMap");
  if ( !vertexmap )
    {
      cerr << PHWHERE << " ERROR: Can't find SvtxVertexMap" << endl;
      exit(-1);
    }

  // --- Raw cluster classes
  bool clusters_available = true;
  RawClusterContainer *emc_clustercontainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  RawClusterContainer *hci_clustercontainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALIN");
  RawClusterContainer *hco_clustercontainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALOUT");
  if ( !emc_clustercontainer || !hci_clustercontainer || !hco_clustercontainer )
    {
      if ( verbosity > -1 )
	{
	  cerr << PHWHERE << " WARNING: Can't find cluster nodes" << endl;
	  cerr << PHWHERE << "  emc_clustercontainer " << emc_clustercontainer << endl;
	  cerr << PHWHERE << "  hci_clustercontainer " << hci_clustercontainer << endl;
	  cerr << PHWHERE << "  hco_clustercontainer " << hco_clustercontainer << endl;
	}
      clusters_available = false;
      ++nwarnings;
    }

  // --- Tower geometry
  RawTowerGeomContainer *emc_towergeo = findNode::getClass<RawTowerGeomContainer>(topNode,"TOWERGEOM_CEMC");
  RawTowerGeomContainer *hci_towergeo = findNode::getClass<RawTowerGeomContainer>(topNode,"TOWERGEOM_HCALIN");
  RawTowerGeomContainer *hco_towergeo = findNode::getClass<RawTowerGeomContainer>(topNode,"TOWERGEOM_HCALOUT");
  if ( !emc_towergeo || !hci_towergeo || !hco_towergeo )
    {
      if ( verbosity > -1 )
	{
	  cerr << PHWHERE << " WARNING: Can't find tower geometry nodes" << endl;
	  cerr << PHWHERE << "  emc_towergeo " << emc_towergeo << endl;
	  cerr << PHWHERE << "  hci_towergeo " << hci_towergeo << endl;
	  cerr << PHWHERE << "  hco_towergeo " << hco_towergeo << endl;
	}
      ++nwarnings;
    }

  // --- Tower container
  RawTowerContainer *emc_towercontainer = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_CEMC");
  RawTowerContainer *hci_towercontainer = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_HCALIN");
  RawTowerContainer *hco_towercontainer = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_HCALOUT");
  if ( !emc_towercontainer || !hci_towercontainer || !hco_towercontainer )
    {
      if ( verbosity > -1 )
	{
	  cerr << PHWHERE << " WARNING: Can't find tower container nodes" << endl;
	  cerr << PHWHERE << "  emc_towercontainer " << emc_towercontainer << endl;
	  cerr << PHWHERE << "  hci_towercontainer " << hci_towercontainer << endl;
	  cerr << PHWHERE << "  hco_towercontainer " << hco_towercontainer << endl;
	}
      ++nwarnings;
    }




  // --- step 1: loop over all possible towers and make a map of energy, tower address
  // --- step 2: loop over map in reverse order and fill a vector of tower addresses
  // --- step 3: use vector of tower addresses (which are ordered by energy highest to lowest) as desired
  // --- note: the vector is not really needed, as one can just get anything that's needed from the map
  // ---       itself, but personally I like vectors

  //int nphi = emc_towergeo->get_phibins();
  //int neta = emc_towergeo->get_etabins();



  vector<RawCluster*> emc_clusters = get_ordered_clusters(emc_clustercontainer);
  vector<RawCluster*> hci_clusters = get_ordered_clusters(hci_clustercontainer);
  vector<RawCluster*> hco_clusters = get_ordered_clusters(hco_clustercontainer);



  // --- Create SVTX eval stack
  SvtxEvalStack svtxevalstack(topNode);
  // --- Get evaluator objects from the eval stack
  //SvtxVertexEval*   vertexeval = svtxevalstack.get_vertex_eval();
  SvtxTrackEval*     trackeval = svtxevalstack.get_track_eval();
  SvtxTruthEval*     trutheval = svtxevalstack.get_truth_eval();

  // --- Create calorimter eval stacks
  CaloEvalStack emc_caloevalstack(topNode,"CLUSTER_CEMC");
  CaloEvalStack hci_caloevalstack(topNode,"CLUSTER_HCALIN");
  CaloEvalStack hco_caloevalstack(topNode,"CLUSTER_HCALOUT");
  // --- get the evaluator objects
  CaloRawClusterEval *emc_rawclustereval = emc_caloevalstack.get_rawcluster_eval();
  CaloRawClusterEval *hci_rawclustereval = hci_caloevalstack.get_rawcluster_eval();
  CaloRawClusterEval *hco_rawclustereval = hco_caloevalstack.get_rawcluster_eval();
  emc_rawclustereval->set_verbosity(0); // temp while resolving issues
  hci_rawclustereval->set_verbosity(0); // temp while resolving issues
  hco_rawclustereval->set_verbosity(0); // temp while resolving issues

  if ( verbosity > 0 || ( !clusters_available && verbosity > -1 ) )
    {
      cout << "Eval stack memory addresses..." << endl;
      cout << &svtxevalstack << endl;
      cout << &emc_caloevalstack << endl;
      cout << &hci_caloevalstack << endl;
      cout << &hco_caloevalstack << endl;
      cout << "Evaluator addresses..." << endl;
      cout << trackeval << endl;
      cout << emc_rawclustereval << endl;
      cout << hci_rawclustereval << endl;
      cout << hco_rawclustereval << endl;
    }

  if ( verbosity > 0 ) cout << "Now going to loop over truth partcles..." << endl; // need verbosity switch

  // --- Loop over all truth particles
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
  for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter )
    {
      PHG4Particle* g4particle = iter->second; // You may ask yourself, why second?
      // In C++ the iterator is a map, which has two members
      // first is the key (analogous the index of an arry),
      // second is the value (analogous to the value stored for the array index)
      int particleID = g4particle->get_pid();

      if ( trutheval->get_embed(g4particle) <= 0 && fabs(particleID) == 11 && verbosity > 0 )
        {
          cout << "NON EMBEDDED ELECTRON!!!  WHEE!!! " << particleID << " " << iter->first << endl;
        }

      if ( trutheval->get_embed(g4particle) <= 0 ) continue; // only look at embedded particles // no good for hits files
      bool iselectron = fabs(particleID) == 11;
      bool ispion = fabs(particleID) == 211;
      if ( verbosity > 0 ) cout << "embedded particle ID is " << particleID << " ispion " << ispion << " iselectron " << iselectron << " " << iter->first << endl;

      set<PHG4Hit*> g4hits = trutheval->all_truth_hits(g4particle);
      //float ng4hits = g4hits.size();

      float truept = sqrt(pow(g4particle->get_px(),2)+pow(g4particle->get_py(),2));
      float true_energy = g4particle->get_e();

      // --- Get the reconsructed SvtxTrack based on the best candidate from the truth info
      SvtxTrack* track = trackeval->best_track_from(g4particle);
      if (!track) continue;
      float recopt = track->get_pt();
      //float recop = track->get_p();

      if ( verbosity > 0 )
	{
	  cout << "truept is " << truept << endl;
	  cout << "recopt is " << recopt << endl;
	  cout << "true energy is " << true_energy << endl;
	}

      // ----------------------------------------------------------------------
      // ----------------------------------------------------------------------
      // ----------------------------------------------------------------------

      // --- Get the clusters from the best candidate from the truth info using the eval
      RawCluster* emc_bestcluster = NULL;
      RawCluster* hci_bestcluster = NULL;
      RawCluster* hco_bestcluster = NULL;
      if ( emc_rawclustereval ) emc_bestcluster = emc_rawclustereval->best_cluster_from(g4particle);
      if ( hci_rawclustereval ) hci_bestcluster = hci_rawclustereval->best_cluster_from(g4particle);
      if ( hco_rawclustereval ) hco_bestcluster = hco_rawclustereval->best_cluster_from(g4particle);

      if ( verbosity > 5 )
	{
	  cout << "Cluster addresses from best cluster " << endl;
	  cout << emc_bestcluster << endl;
	  cout << hci_bestcluster << endl;
	  cout << hco_bestcluster << endl;
	}

      // --- If that didn't work, take the largest cluster in the event
      // --- This is terrible for more than one particle, so I need to
      // --- develop my own track-cluster association...
      if ( !emc_bestcluster && emc_clusters.size() ) emc_bestcluster = emc_clusters[0];
      if ( !hci_bestcluster && hci_clusters.size() ) hci_bestcluster = hci_clusters[0];
      if ( !hco_bestcluster && hco_clusters.size() ) hco_bestcluster = hco_clusters[0];

      if ( verbosity > 5 )
	{
	  cout << "Cluster addresses from my (very bad) association " << endl;
	  cout << emc_bestcluster << endl;
	  cout << hci_bestcluster << endl;
	  cout << hco_bestcluster << endl;
	}

      // --- Get a vector of the towers from the cluster
      vector<RawTower*> emc_towers_from_bestcluster = get_ordered_towers(emc_bestcluster,emc_towercontainer);
      vector<RawTower*> hci_towers_from_bestcluster = get_ordered_towers(hci_bestcluster,hci_towercontainer);
      vector<RawTower*> hco_towers_from_bestcluster = get_ordered_towers(hco_bestcluster,hco_towercontainer);

      // --- Inspect the towers (fills a bunch of histograms, prints to screen if verbose)
      inspect_ordered_towers(emc_towers_from_bestcluster,true_energy,SvtxTrack::CEMC);
      inspect_ordered_towers(hci_towers_from_bestcluster,true_energy,SvtxTrack::HCALIN);
      inspect_ordered_towers(hco_towers_from_bestcluster,true_energy,SvtxTrack::HCALOUT);

      // ----------------------------------------------------------------------
      // ----------------------------------------------------------------------
      // ----------------------------------------------------------------------



      // --- energy variables from the best candidate clusters
      float emc_energy_best = -9999;
      float hci_energy_best = -9999;
      float hco_energy_best = -9999;

      // --- energy variables directly from the track object
      float emc_energy_track = -9999;
      float hci_energy_track = -9999;
      float hco_energy_track = -9999;

      if ( verbosity > 2 ) cout << "Now attempting to get the energies..." << endl;

      // --- get the energy values directly from the best candidate IF the cluster container exists
      if ( emc_bestcluster ) emc_energy_best = emc_bestcluster->get_energy();
      if ( hci_bestcluster ) hci_energy_best = hci_bestcluster->get_energy();
      if ( hco_bestcluster ) hco_energy_best = hco_bestcluster->get_energy();

      if ( verbosity > 5 )
	{
	  cout << "emc_energy_best is " << emc_energy_best << endl;
	  cout << "hci_energy_best is " << hci_energy_best << endl;
	  cout << "hco_energy_best is " << hco_energy_best << endl;
	}

      // --- get the energy values directly from the track IF the cluster container exists
      if ( emc_clustercontainer ) emc_energy_track = track->get_cal_cluster_e(SvtxTrack::CEMC);
      if ( hci_clustercontainer ) hci_energy_track = track->get_cal_cluster_e(SvtxTrack::HCALIN);
      if ( hco_clustercontainer ) hco_energy_track = track->get_cal_cluster_e(SvtxTrack::HCALOUT);

      if ( verbosity > 5 )
	{
	  cout << "emc_energy_track is " << emc_energy_track << endl;
	  cout << "hci_energy_track is " << hci_energy_track << endl;
	  cout << "hco_energy_track is " << hco_energy_track << endl;
	}

      // -------------------------------------------------------------------------------------
      // --- IMPORTANT NOTE: according to Jin, the clusterizing will gather all towers in the
      // ---                 calorimeter, so we need to use the 3x3 instead

      if ( emc_clustercontainer ) emc_energy_track = track->get_cal_energy_3x3(SvtxTrack::CEMC);
      if ( hci_clustercontainer ) hci_energy_track = track->get_cal_energy_3x3(SvtxTrack::HCALIN);
      if ( hco_clustercontainer ) hco_energy_track = track->get_cal_energy_3x3(SvtxTrack::HCALOUT);

      if ( verbosity > 0 )
	{
	  cout << "emc_energy_track is " << emc_energy_track << endl;
	  cout << "hci_energy_track is " << hci_energy_track << endl;
	  cout << "hco_energy_track is " << hco_energy_track << endl;
	}

      // -------------------------------------------------------------------------------------
      // --- IMPORTANT NOTE: according to Jin, dphi and deta will not work correctly in HIJING

      float emc_dphi_track = -9999;
      float hci_dphi_track = -9999;
      float hco_dphi_track = -9999;
      if ( emc_clustercontainer ) emc_dphi_track = track->get_cal_dphi(SvtxTrack::CEMC);
      if ( hci_clustercontainer ) hci_dphi_track = track->get_cal_dphi(SvtxTrack::HCALIN);
      if ( hco_clustercontainer ) hco_dphi_track = track->get_cal_dphi(SvtxTrack::HCALOUT);

      float emc_deta_track = -9999;
      float hci_deta_track = -9999;
      float hco_deta_track = -9999;
      if ( emc_clustercontainer ) emc_deta_track = track->get_cal_deta(SvtxTrack::CEMC);
      if ( hci_clustercontainer ) hci_deta_track = track->get_cal_deta(SvtxTrack::HCALIN);
      if ( hco_clustercontainer ) hco_deta_track = track->get_cal_deta(SvtxTrack::HCALOUT);

      _energy_dphi_emc->Fill(true_energy,emc_dphi_track);
      _energy_dphi_hci->Fill(true_energy,hci_dphi_track);
      _energy_dphi_hco->Fill(true_energy,hco_dphi_track);

      _energy_deta_emc->Fill(true_energy,emc_deta_track);
      _energy_deta_hci->Fill(true_energy,hci_deta_track);
      _energy_deta_hco->Fill(true_energy,hco_deta_track);

      float assoc_dphi = 0.1; // adjust as needed, consider class set method
      float assoc_deta = 0.1; // adjust as needed, consider class set method
      bool good_emc_assoc = fabs(emc_dphi_track) < assoc_dphi && fabs(emc_deta_track) < assoc_deta;
      bool good_hci_assoc = fabs(hci_dphi_track) < assoc_dphi && fabs(hci_deta_track) < assoc_deta;
      bool good_hco_assoc = fabs(hco_dphi_track) < assoc_dphi && fabs(hco_deta_track) < assoc_deta;

      // ------------------------------------------------------------------------------------------

      // --- check the variables
      if ( verbosity > 3 )
	{
	  cout << "emc_energy_best is " << emc_energy_best << endl;
	  cout << "hci_energy_best is " << hci_energy_best << endl;
	  cout << "hco_energy_best is " << hco_energy_best << endl;
	  cout << "emc_energy_track is " << emc_energy_track << endl;
	  cout << "hci_energy_track is " << hci_energy_track << endl;
	  cout << "hco_energy_track is " << hco_energy_track << endl;
	  cout << "emc_dphi_track is " << emc_dphi_track << endl;
	  cout << "hci_dphi_track is " << hci_dphi_track << endl;
	  cout << "hco_dphi_track is " << hco_dphi_track << endl;
	  cout << "emc_deta_track is " << emc_deta_track << endl;
	  cout << "hci_deta_track is " << hci_deta_track << endl;
	  cout << "hco_deta_track is " << hco_deta_track << endl;
	} // check on verbosity

      float hct_energy_track = 0;
      if ( hci_energy_track >= 0 ) hct_energy_track += hci_energy_track;
      if ( hco_energy_track >= 0 ) hct_energy_track += hco_energy_track;

      float total_energy_dumb = 0;
      if ( emc_energy_track >= 0 ) total_energy_dumb += emc_energy_track;
      if ( hci_energy_track >= 0 ) total_energy_dumb += hci_energy_track;
      if ( hco_energy_track >= 0 ) total_energy_dumb += hco_energy_track;

      float total_energy_smart = 0;
      if ( good_emc_assoc ) total_energy_smart += emc_energy_track;
      if ( good_hci_assoc ) total_energy_smart += hci_energy_track;
      if ( good_hco_assoc ) total_energy_smart += hco_energy_track;



      float emc_eratio = emc_energy_track/true_energy;
      float hci_eratio = hci_energy_track/true_energy;
      float hco_eratio = hco_energy_track/true_energy;
      float hct_eratio = hct_energy_track/true_energy;
      float tot_dumb_eratio = total_energy_dumb/true_energy;
      float tot_smart_eratio = total_energy_smart/true_energy;

      _energy_ratio_emc->Fill(true_energy,emc_eratio);
      _energy_ratio_hci->Fill(true_energy,hci_eratio);
      _energy_ratio_hco->Fill(true_energy,hco_eratio);
      _energy_ratio_hct->Fill(true_energy,hct_eratio);
      _energy_ratio_tot_dumb->Fill(true_energy,tot_dumb_eratio);
      _energy_ratio_tot_smart->Fill(true_energy,tot_smart_eratio);

      if ( emc_eratio < 0.1 )
	{
	  _energy_ratio_elb_emc->Fill(true_energy,emc_eratio);
	  _energy_ratio_elb_hci->Fill(true_energy,hci_eratio);
	  _energy_ratio_elb_hco->Fill(true_energy,hco_eratio);
	  _energy_ratio_elb_hct->Fill(true_energy,hct_eratio);
	  _energy_ratio_elb_tot_dumb->Fill(true_energy,tot_dumb_eratio);
	  _energy_ratio_elb_tot_smart->Fill(true_energy,tot_smart_eratio);
	}

      if ( emc_eratio > 0.1 )
	{
	  _energy_ratio_eub_emc->Fill(true_energy,emc_eratio);
	  _energy_ratio_eub_hci->Fill(true_energy,hci_eratio);
	  _energy_ratio_eub_hco->Fill(true_energy,hco_eratio);
	  _energy_ratio_eub_hct->Fill(true_energy,hct_eratio);
	  _energy_ratio_eub_tot_dumb->Fill(true_energy,tot_dumb_eratio);
	  _energy_ratio_eub_tot_smart->Fill(true_energy,tot_smart_eratio);
	}

      // ----------------------------------------------------------------------
      // ----------------------------------------------------------------------
      // ----------------------------------------------------------------------

      //cout << "starting the main part of the truth analysis" << endl;

    } // end of loop over truth particles



  return Fun4AllReturnCodes::EVENT_OK;

}



int STACalorimeterCharacterization::End(PHCompositeNode *topNode)
{
  cout << "End called, " << nevents << " events processed with " << nerrors << " errors and " << nwarnings << " warnings." << endl;
  return 0;
}



// --- not sure if it's possible but it'd be nice to do some better function polymorphism here
vector<RawCluster*> STACalorimeterCharacterization::get_ordered_clusters(const RawClusterContainer* clusters)
{

  if ( clusters == NULL ) return vector<RawCluster*>();

  // getClusters has two options, const and non const, so I use const here
  auto range = clusters->getClusters();

  map<double,RawCluster*> cluster_map;
  for ( auto it = range.first; it != range.second; ++it )
    {
      RawCluster* cluster = it->second;
      double energy = cluster->get_energy();
      cluster_map.insert(make_pair(energy,cluster));
    }

  vector<RawCluster*> cluster_list;
  for ( auto rit = cluster_map.rbegin(); rit != cluster_map.rend(); ++rit )
    {
      RawCluster* cluster = rit->second;
      cluster_list.push_back(cluster);
    }

  return cluster_list;

}



// --- not sure if it's possible but it'd be nice to do some better function polymorphism here
vector<RawTower*> STACalorimeterCharacterization::get_ordered_towers(const RawTowerContainer* towers)
{

  if ( towers == NULL ) return vector<RawTower*>();

  // getTowers has two options, const and non const, so I use const here
  auto range = towers->getTowers();

  map<double,RawTower*> tower_map;
  for ( auto it = range.first; it != range.second; ++it )
    {
      RawTower* tower = it->second;
      double energy = tower->get_energy();
      tower_map.insert(make_pair(energy,tower));
    }

  vector<RawTower*> tower_list;
  for ( auto rit = tower_map.rbegin(); rit != tower_map.rend(); ++rit )
    {
      RawTower* tower = rit->second;
      tower_list.push_back(tower);
    }

  return tower_list;

}



// --- not sure if it's possible but it'd be nice to do some better function polymorphism here
vector<RawTower*> STACalorimeterCharacterization::get_ordered_towers(RawCluster* cluster, RawTowerContainer* towers)
{

  if ( cluster == NULL || towers == NULL ) return vector<RawTower*>();

  // note that RawCluster* cannot be const, get_towers is not declared as const function in class header
  auto range = cluster->get_towers();

  multimap<double,RawTower*> tower_map;
  for ( auto it = range.first; it != range.second; ++it )
    {
      // note that RawTowerContainer cannot be const, getTower is not declared as a const function in class header
      RawTower* tower = towers->getTower(it->first);
      double energy = tower->get_energy();
      tower_map.insert(make_pair(energy,tower));
    }

  vector<RawTower*> tower_list;
  for ( auto rit = tower_map.rbegin(); rit != tower_map.rend(); ++rit )
    {
      RawTower* tower = rit->second;
      tower_list.push_back(tower);
    }

  return tower_list;

}



void STACalorimeterCharacterization::inspect_ordered_towers(const vector<RawTower*>& towers)
{
  inspect_ordered_towers(towers,0,0);
}



void STACalorimeterCharacterization::inspect_ordered_towers(const vector<RawTower*>& towers, int calo_layer)
{
  inspect_ordered_towers(towers,0,calo_layer);
}



void STACalorimeterCharacterization::inspect_ordered_towers(const vector<RawTower*>& towers, double true_energy, int calo_layer)
{

  if ( towers.size() == 0 ) return;

  int nphi = 9999;

  if ( calo_layer == SvtxTrack::CEMC ) nphi = 256;
  if ( calo_layer == SvtxTrack::HCALIN ) nphi = 64;
  if ( calo_layer == SvtxTrack::HCALOUT ) nphi = 64;

  RawTower* ctower = towers[0]; // central tower

  for ( unsigned int i = 0; i < towers.size(); ++i )
    {
      RawTower* itower = towers[i];
      double ienergy = itower->get_energy();

      // if ( verbosity > 5 ) cout << "energy is " << ienergy << " tower address is " << itower << endl;

      // get the coordinates
      int etabin_center = ctower->get_bineta();
      int phibin_center = ctower->get_binphi();
      int etabin = itower->get_bineta();
      int phibin = itower->get_binphi();
      // recenter around central tower
      etabin -= etabin_center;
      phibin -= phibin_center;
      // boundary and periodicity corrections...
      if ( phibin > nphi/2 ) phibin -= nphi;
      if ( phibin < -nphi/2 ) phibin += nphi;
      // if ( verbosity > 6 ) cout << "eta phi coordinates relative to central tower " << etabin << " " << phibin << endl;

      // fill some 2d coordinate space histograms
      if ( calo_layer == SvtxTrack::CEMC)
	{
	  _towers_3x3_emc->Fill(etabin,phibin,ienergy/true_energy);
	  _towers_5x5_emc->Fill(etabin,phibin,ienergy/true_energy);
	  _towers_7x7_emc->Fill(etabin,phibin,ienergy/true_energy);
	  _towers_9x9_emc->Fill(etabin,phibin,ienergy/true_energy);
	}
      if ( calo_layer == SvtxTrack::HCALIN)
	{
	  _towers_3x3_hci->Fill(etabin,phibin,ienergy/true_energy);
	  _towers_5x5_hci->Fill(etabin,phibin,ienergy/true_energy);
	  _towers_7x7_hci->Fill(etabin,phibin,ienergy/true_energy);
	  _towers_9x9_hci->Fill(etabin,phibin,ienergy/true_energy);
	}
      if ( calo_layer == SvtxTrack::HCALOUT)
	{
	  _towers_3x3_hco->Fill(etabin,phibin,ienergy/true_energy);
	  _towers_5x5_hco->Fill(etabin,phibin,ienergy/true_energy);
	  _towers_7x7_hco->Fill(etabin,phibin,ienergy/true_energy);
	  _towers_9x9_hco->Fill(etabin,phibin,ienergy/true_energy);
	}

    } // loop over towers

  // characterize the tower energy with the truth energy
  double energy_sumtower[10] = {0};
  double energy_singletower = 0;
  unsigned int nloop = 10;
  if ( towers.size() < nloop ) nloop = towers.size();
  for ( unsigned int i = 0; i < nloop; ++i )
    {
      // get the single and summed tower energies
      energy_singletower = towers[i]->get_energy();
      energy_sumtower[i] = energy_singletower;
      for ( unsigned int j = i; j > 0; --j ) energy_sumtower[j] += energy_sumtower[j-1];

      // if ( verbosity > 5 )
      //   {
      //     cout << "inspecting tower energies" << endl;
      //     cout << "single tower energy is " << energy_singletower << endl;
      //     cout << "sum tower energy is " << energy_sumtower[i] << endl;
      //   }

      // fill histograms
      if ( calo_layer == SvtxTrack::CEMC )
	{
	  _tower_energy_emc[i]->Fill(true_energy,energy_singletower/true_energy);
	  _towersum_energy_emc[i]->Fill(true_energy,energy_sumtower[i]/true_energy);
	}
      if ( calo_layer == SvtxTrack::HCALIN )
	{
	  _tower_energy_hci[i]->Fill(true_energy,energy_singletower/true_energy);
	  _towersum_energy_hci[i]->Fill(true_energy,energy_sumtower[i]/true_energy);
	}
      if ( calo_layer == SvtxTrack::HCALOUT )
	{
	  _tower_energy_hco[i]->Fill(true_energy,energy_singletower/true_energy);
	  _towersum_energy_hco[i]->Fill(true_energy,energy_sumtower[i]/true_energy);
	}
    } // fixed size loop

} // inspect_ordered_towers
