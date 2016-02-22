#include "SimpleTrackingAnalysis.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>

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
#include <g4cemc/RawTowerGeomContainer.h>
#include <g4cemc/RawTowerContainer.h>
#include <g4cemc/RawTower.h>
#include <g4cemc/RawCluster.h>
#include <g4cemc/RawClusterContainer.h>
#include <g4eval/CaloEvalStack.h>
#include <g4eval/CaloRawClusterEval.h>
#include <g4eval/CaloRawTowerEval.h>

#include <TH1D.h>
#include <TH2D.h>

#include <iostream>
#include <cassert>

using namespace std;



SimpleTrackingAnalysis::SimpleTrackingAnalysis(const string &name) : SubsysReco(name)
{
  cout << "Class constructor called " << endl;
  nevents = 0;
  nlayers = 7;
  verbosity = 0;
  magneticfield = 1.4; // default is 1.5 but Mike's tracking stuff is using 1.4 for now...
  //docalocuts = false;
}



int SimpleTrackingAnalysis::Init(PHCompositeNode *topNode)
{

  // --------------------------------------------------------------------------------
  // --- This is the class initialization method
  // --- Get a pointer to the Fun4All server to register histograms
  // --- Histograms are declared in class header file
  // --- Histograms are instantiated here and then registered with the Fun4All server
  // --------------------------------------------------------------------------------


  // --- get instance of
  Fun4AllServer *se = Fun4AllServer::instance();



  // --- histograms over true pt, used for finding efficiencies

  _truept_dptoverpt = new TH2D("truept_dptoverpt", "", 40,0.0,40.0, 200,-0.5,0.5);
  se->registerHisto(_truept_dptoverpt);

  _truept_dca = new TH2D("truept_dca", "", 20,0.0,10.0, 200,-0.1,0.1);
  se->registerHisto(_truept_dca);

  _truept_particles_leaving7Hits = new TH1D("truept_particles_leaving7Hits", "", 20,0.0,10.0);
  se->registerHisto(_truept_particles_leaving7Hits);

  _truept_particles_recoWithExactHits = new TH1D("truept_particles_recoWithExactHits", "", 20,0.0,10.0);
  se->registerHisto(_truept_particles_recoWithExactHits);

  _truept_particles_recoWithin1Hit = new TH1D("truept_particles_recoWithin1Hit", "", 20,0.0,10.0);
  se->registerHisto(_truept_particles_recoWithin1Hit);

  _truept_particles_recoWithin2Hits = new TH1D("truept_particles_recoWithin2Hits", "", 20,0.0,10.0);
  se->registerHisto(_truept_particles_recoWithin2Hits);

  _truept_particles_recoWithin3Percent = new TH1D("truept_particles_recoWithin3Percent", "", 20,0.0,10.0);
  se->registerHisto(_truept_particles_recoWithin3Percent);

  _truept_particles_recoWithin4Percent = new TH1D("truept_particles_recoWithin4Percent", "", 20,0.0,10.0);
  se->registerHisto(_truept_particles_recoWithin4Percent);

  _truept_particles_recoWithin5Percent = new TH1D("truept_particles_recoWithin5Percent", "", 20,0.0,10.0);
  se->registerHisto(_truept_particles_recoWithin5Percent);



  // --- (mostly) the same set of histograms over reconstructed pt, used for studying purity

  _recopt_tracks_all = new TH1D("recopt_tracks_all", "", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_all);

  _recopt_tracks_recoWithExactHits = new TH1D("recopt_tracks_recoWithExactHits", "", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_recoWithExactHits);

  _recopt_tracks_recoWithin1Hit = new TH1D("recopt_tracks_recoWithin1Hit", "", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_recoWithin1Hit);

  _recopt_tracks_recoWithin2Hits = new TH1D("recopt_tracks_recoWithin2Hits", "", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_recoWithin2Hits);

  _recopt_tracks_recoWithin3Percent = new TH1D("recopt_tracks_recoWithin3Percent", "", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_recoWithin3Percent);

  _recopt_tracks_recoWithin4Percent = new TH1D("recopt_tracks_recoWithin4Percent", "", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_recoWithin4Percent);

  _recopt_tracks_recoWithin5Percent = new TH1D("recopt_tracks_recoWithin5Percent", "", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_recoWithin5Percent);



  // --- (mostly) the same set of histograms over reconstructed pt, used for studying purity, with calorimeter cuts

  _recopt_tracks_withcalocuts_all = new TH1D("recopt_tracks_withcalocuts_all", "", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_withcalocuts_all);

  _recopt_tracks_withcalocuts_recoWithExactHits = new TH1D("recopt_tracks_withcalocuts_recoWithExactHits", "", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_withcalocuts_recoWithExactHits);

  _recopt_tracks_withcalocuts_recoWithin1Hit = new TH1D("recopt_tracks_withcalocuts_recoWithin1Hit", "", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_withcalocuts_recoWithin1Hit);

  _recopt_tracks_withcalocuts_recoWithin2Hits = new TH1D("recopt_tracks_withcalocuts_recoWithin2Hits", "", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_withcalocuts_recoWithin2Hits);

  _recopt_tracks_withcalocuts_recoWithin3Percent = new TH1D("recopt_tracks_withcalocuts_recoWithin3Percent", "", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_withcalocuts_recoWithin3Percent);

  _recopt_tracks_withcalocuts_recoWithin4Percent = new TH1D("recopt_tracks_withcalocuts_recoWithin4Percent", "", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_withcalocuts_recoWithin4Percent);

  _recopt_tracks_withcalocuts_recoWithin5Percent = new TH1D("recopt_tracks_withcalocuts_recoWithin5Percent", "", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_withcalocuts_recoWithin5Percent);



  // --- vertex residual histograms

  _dx_vertex = new TH1D("dx_vertex", "dx_vertex", 200,-0.03,0.03);
  se->registerHisto(_dx_vertex);

  _dy_vertex = new TH1D("dy_vertex", "dy_vertex", 200,-0.03,0.03);
  se->registerHisto(_dy_vertex);

  _dz_vertex = new TH1D("dz_vertex", "dz_vertex", 200,-0.03,0.03);
  se->registerHisto(_dz_vertex);



  // --- additional tracking histograms for studying quality

  _recopt_quality = new TH2D("recopt_quality", "", 20,0.0,10.0, 100,0.0,5.0);
  se->registerHisto(_recopt_quality);

  _truept_quality_particles_recoWithin4Percent = new TH2D("truept_quality_particles_recoWithin4Percent", "", 20,0.0,10.0, 100,0.0,5.0);
  se->registerHisto(_truept_quality_particles_recoWithin4Percent);

  _recopt_quality_tracks_all = new TH2D("recopt_quality_tracks_all", "", 20,0.0,10.0, 100,0.0,5.0);
  se->registerHisto(_recopt_quality_tracks_all);

  _recopt_quality_tracks_recoWithin4Percent = new TH2D("recopt_quality_tracks_recoWithin4Percent", "", 20,0.0,10.0, 100,0.0,5.0);
  se->registerHisto(_recopt_quality_tracks_recoWithin4Percent);



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

  return 0;

}



int SimpleTrackingAnalysis::process_event(PHCompositeNode *topNode)
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
    }

  // --- Tower geometry
  RawTowerGeomContainer *emc_towergeo = findNode::getClass<RawTowerGeomContainer>(topNode,"TOWERGEOM_CEMC");
  RawTowerGeomContainer *hci_towergeo = findNode::getClass<RawTowerGeomContainer>(topNode,"TOWERGEOM_HCALIN");
  RawTowerGeomContainer *hco_towergeo = findNode::getClass<RawTowerGeomContainer>(topNode,"TOWERGEOM_HCALOUT");
  if ( !emc_towergeo || !hci_towergeo || !hco_towergeo )
    {
      if ( verbosity > -1 )
	{
	  cerr << PHWHERE << " WARNING: Can't find cluster nodes" << endl;
	  cerr << PHWHERE << "  emc_towergeo " << emc_towergeo << endl;
	  cerr << PHWHERE << "  hci_towergeo " << hci_towergeo << endl;
	  cerr << PHWHERE << "  hco_towergeo " << hco_towergeo << endl;
	}
    }

  // --- Tower container
  RawTowerContainer *emc_towercontainer = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_CEMC");
  RawTowerContainer *hci_towercontainer = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_HCALIN");
  RawTowerContainer *hco_towercontainer = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_HCALOUT");
  if ( !emc_towercontainer || !hci_towercontainer || !hco_towercontainer )
    {
      if ( verbosity > -1 )
	{
	  cerr << PHWHERE << " WARNING: Can't find cluster nodes" << endl;
	  cerr << PHWHERE << "  emc_towercontainer " << emc_towercontainer << endl;
	  cerr << PHWHERE << "  hci_towercontainer " << hci_towercontainer << endl;
	  cerr << PHWHERE << "  hco_towercontainer " << hco_towercontainer << endl;
	}
    }




  // --- step 1: loop over all possible towers and make a map of energy, tower address
  // --- step 2: loop over map in reverse order and fill a vector of tower addresses
  // --- step 3: use vector of tower addresses (which are ordered by energy highest to lowest) as desired
  // --- note: the vector is not really needed, as one can just get anything that's needed from the map
  // ---       itself, but personally I like vectors

  int nphi = emc_towergeo->get_phibins();
  //int neta = emc_towergeo->get_etabins();

  map<double,RawTower*> emc_towers_map;
  RawTowerContainer::Range emc_towerrange = emc_towercontainer->getTowers();
  for ( RawTowerContainer::Iterator it = emc_towerrange.first; it != emc_towerrange.second; ++it )
    {
      RawTower* tower = emc_towercontainer->getTower(it->first);
      if ( tower )
	{
	  double energy = tower->get_energy();
	  emc_towers_map.insert(make_pair(energy,tower));
	} // check on tower
    } // loop over tower range

  vector<RawTower*> emc_towers;
  for ( map<double,RawTower*>::reverse_iterator rit = emc_towers_map.rbegin(); rit != emc_towers_map.rend(); ++rit )
    {
      if ( verbosity > 7 ) cout << "energy is " << rit->first << " tower address is " << rit->second << endl;
      emc_towers.push_back(rit->second);
    }

  if ( verbosity > 1 ) cout << "number of clusters is " << emc_clustercontainer->size() << endl;
  RawClusterContainer::ConstRange emc_clusterrange = emc_clustercontainer->getClusters();
  for ( RawClusterContainer::ConstIterator it = emc_clusterrange.first; it != emc_clusterrange.second; ++it )
    {
      RawCluster *cluster = emc_clustercontainer->getCluster(it->first);
      int ntowers = cluster->getNTowers();
      double energy = cluster->get_energy();
      if ( ntowers > 2 )
	{
	  if ( verbosity > 2 ) cout << "cluster energy is " << energy << " and number of towers is " << ntowers << endl;
	  RawCluster::TowerConstRange emc_towerrange = cluster->get_towers();
	  for ( RawCluster::TowerConstIterator it_tower = emc_towerrange.first; it_tower != emc_towerrange.second; ++it_tower )
	    {
	      RawTower* tower = emc_towercontainer->getTower(it_tower->first);
	      if ( tower )
		{
		  double energy_tower = tower->get_energy();
		  if ( verbosity > 2 ) cout << "tower energy is " << energy_tower << " at address " << tower << endl;
		} // check on tower
	    } // loop over tower range
	} // check on number of towers
    } // loop over clusters

  double emc_energysum = 0;
  for ( unsigned int i = 0; i < emc_towers.size(); ++i )
    {
      // --- note: central stuff should be moved outside of loop for performance
      RawTower* ctower = emc_towers[0];
      RawTower* itower = emc_towers[i];
      double cenergy = ctower->get_energy();
      double ienergy = itower->get_energy();
      if ( ienergy < 0.05*cenergy ) break; // just to see a few
      if ( verbosity > 1 ) cout << "energy is " << ienergy << " tower address is " << itower << endl;
      // --- more stuff
      emc_energysum += ienergy;
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
      if ( verbosity > 2 ) cout << "eta phi coordinates relative to central tower " << etabin << " " << phibin << endl;
      if ( verbosity > 2 ) cout << "energy sum is " << emc_energysum << endl;
    }



  // --- Create SVTX eval stack
  SvtxEvalStack svtxevalstack(topNode);
  // --- Get evaluator objects from the eval stack
  SvtxVertexEval*   vertexeval = svtxevalstack.get_vertex_eval();
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
      // if ( trutheval->get_embed(g4particle) == 0 ) continue; // only look at embedded particles (for now) // no good for hits files

      set<PHG4Hit*> g4hits = trutheval->all_truth_hits(g4particle);
      float ng4hits = g4hits.size();

      float truept = sqrt(pow(g4particle->get_px(),2)+pow(g4particle->get_py(),2));
      float true_energy = g4particle->get_e();

      // --- Get the reconsructed SvtxTrack based on the best candidate from the truth info
      SvtxTrack* track = trackeval->best_track_from(g4particle);
      if (!track) continue;
      float recopt = track->get_pt();

      // ----------------------------------------------------------------------
      // ----------------------------------------------------------------------
      // ----------------------------------------------------------------------

      // for ( int i = 0; i < emc_ntowers; ++i )
      // 	{

      // 	}

      // ----------------------------------------------------------------------
      // ----------------------------------------------------------------------
      // ----------------------------------------------------------------------





      if ( verbosity > 0 )
	{
	  cout << "truept is " << truept << endl;
	  cout << "recopt is " << recopt << endl;
	  cout << "true energy is " << true_energy << endl;
	}

      // ---------------------
      // --- calorimeter stuff
      // ---------------------

      // --- Get the clusters from the best candidate from the truth info
      RawCluster* emc_rawcluster = NULL;
      RawCluster* hci_rawcluster = NULL;
      RawCluster* hco_rawcluster = NULL;
      if ( emc_rawclustereval ) emc_rawcluster = emc_rawclustereval->best_cluster_from(g4particle);
      if ( hci_rawclustereval ) hci_rawcluster = hci_rawclustereval->best_cluster_from(g4particle);
      if ( hco_rawclustereval ) hco_rawcluster = hco_rawclustereval->best_cluster_from(g4particle);

      if ( verbosity > 1 )
	{
	  cout << "RawCluster memory addresses for best from truth..." << endl;
	  cout << emc_rawcluster << endl;
	  cout << hci_rawcluster << endl;
	  cout << hco_rawcluster << endl;
	}

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
      if ( emc_rawcluster ) emc_energy_track = emc_rawcluster->get_energy();
      if ( hci_rawcluster ) hci_energy_track = hci_rawcluster->get_energy();
      if ( hco_rawcluster ) hco_energy_track = hco_rawcluster->get_energy();

      // --- get the energy values directly from the track IF the cluster container exists
      if ( emc_clustercontainer ) emc_energy_track = track->get_cal_cluster_e(SvtxTrack::CEMC);
      if ( hci_clustercontainer ) hci_energy_track = track->get_cal_cluster_e(SvtxTrack::HCALIN);
      if ( hco_clustercontainer ) hco_energy_track = track->get_cal_cluster_e(SvtxTrack::HCALOUT);

      if ( verbosity > 0 )
	{
	  cout << "emc_energy_track is " << emc_energy_track << endl;
	  cout << "hci_energy_track is " << hci_energy_track << endl;
	  cout << "hco_energy_track is " << hco_energy_track << endl;
	}

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



      // examine truth particles that leave all (7 or 8 depending on design) detector hits
      if ( ng4hits == nlayers )
	{
	  _truept_particles_leaving7Hits->Fill(truept);

	  unsigned int nfromtruth = trackeval->get_nclusters_contribution(track,g4particle);

	  unsigned int ndiff = abs((int)nfromtruth-(int)nlayers);
	  if ( ndiff <= 2 ) _truept_particles_recoWithin2Hits->Fill(truept);
	  if ( ndiff <= 1 ) _truept_particles_recoWithin1Hit->Fill(truept);
	  if ( ndiff == 0 ) _truept_particles_recoWithExactHits->Fill(truept);

	  float diff = fabs(recopt-truept)/truept;
	  if ( diff < 0.05 ) _truept_particles_recoWithin5Percent->Fill(truept);
	  if ( diff < 0.04 )
	    {
	      _truept_particles_recoWithin4Percent->Fill(truept);
	      _truept_quality_particles_recoWithin4Percent->Fill(truept,track->get_quality());
	    }
	  if ( diff < 0.03 ) _truept_particles_recoWithin3Percent->Fill(truept);

	} // end of requirement of ng4hits == nlayers

    } // end of loop over truth particles



  // loop over all reco particles
  for ( SvtxTrackMap::Iter iter = trackmap->begin(); iter != trackmap->end(); ++iter )
    {

      // --- Get the StxTrack object (from the iterator)
      SvtxTrack* track = iter->second;
      float recopt = track->get_pt();

      // --- Get the truth particle from the evaluator
      PHG4Particle* g4particle = trackeval->max_truth_particle_by_nclusters(track);
      float truept = sqrt(pow(g4particle->get_px(),2)+pow(g4particle->get_py(),2));

      // ---------------------
      // --- calorimeter stuff
      // ---------------------

      // --- energy variables directly from the track object
      float emc_energy_track = -9999;
      //float hci_energy_track = -9999;
      //float hco_energy_track = -9999;

      // --- get the energy values directly from the track
      if ( emc_clustercontainer ) emc_energy_track = track->get_cal_energy_3x3(SvtxTrack::CEMC);
      //if ( hci_clustercontainer ) hci_energy_track = track->get_cal_energy_3x3(SvtxTrack::HCALIN);
      //if ( hco_clustercontainer ) hco_energy_track = track->get_cal_energy_3x3(SvtxTrack::HCALOUT);

      // ---

      if (trutheval->get_embed(g4particle) != 0)
	{
	  // embedded results (quality or performance measures)
	  _truept_dptoverpt->Fill(truept,(recopt-truept)/truept);
	  _truept_dca->Fill(truept,track->get_dca2d());
	  _recopt_quality->Fill(recopt,track->get_quality());
	} // end if (embedded results)
      else
	{
	  // non-embedded results (purity measures)
	  _recopt_tracks_all->Fill(recopt);
	  _recopt_quality_tracks_all->Fill(recopt,track->get_quality());

	  unsigned int nfromtruth = trackeval->get_nclusters_contribution(track,g4particle);

	  unsigned int ndiff = abs((int)nfromtruth-(int)nlayers);
	  if ( ndiff <= 2 ) _recopt_tracks_recoWithin2Hits->Fill(recopt);
	  if ( ndiff <= 1 ) _recopt_tracks_recoWithin1Hit->Fill(recopt);
	  if ( ndiff == 0 ) _recopt_tracks_recoWithExactHits->Fill(recopt);

	  float diff = fabs(recopt-truept)/truept;
	  if ( diff < 0.05 ) _recopt_tracks_recoWithin5Percent->Fill(recopt);
	  if ( diff < 0.04 )
	    {
	      _recopt_tracks_recoWithin4Percent->Fill(recopt);
	      _recopt_quality_tracks_recoWithin4Percent->Fill(recopt,track->get_quality());
	    }
	  if ( diff < 0.03 ) _recopt_tracks_recoWithin3Percent->Fill(recopt);


	  // --------------------------------------
	  // --- same but now with calorimeter cuts
	  // --------------------------------------

	  // this needs careful study and consideration, just getting started for now...
	  bool goodcalo = (emc_energy_track > 0.2 * recopt);

	  if ( goodcalo )
	    {
	      _recopt_tracks_withcalocuts_all->Fill(recopt);

	      unsigned int nfromtruth = trackeval->get_nclusters_contribution(track,g4particle);

	      unsigned int ndiff = abs((int)nfromtruth-(int)nlayers);
	      if ( ndiff <= 2 ) _recopt_tracks_withcalocuts_recoWithin2Hits->Fill(recopt);
	      if ( ndiff <= 1 ) _recopt_tracks_withcalocuts_recoWithin1Hit->Fill(recopt);
	      if ( ndiff == 0 ) _recopt_tracks_withcalocuts_recoWithExactHits->Fill(recopt);

	      float diff = fabs(recopt-truept)/truept;
	      if ( diff < 0.05 ) _recopt_tracks_withcalocuts_recoWithin5Percent->Fill(recopt);
	      if ( diff < 0.04 ) _recopt_tracks_withcalocuts_recoWithin4Percent->Fill(recopt);
	      if ( diff < 0.03 ) _recopt_tracks_withcalocuts_recoWithin3Percent->Fill(recopt);

	    } // check on good calo

	  // --- done with reco tracks

	} // else (non-embedded results)

    } // loop over reco tracks



  // --- Get the leading vertex
  SvtxVertex* maxvertex = NULL;
  unsigned int maxtracks = 0;
  for ( SvtxVertexMap::Iter iter = vertexmap->begin(); iter != vertexmap->end(); ++iter )
    {
      SvtxVertex* vertex = iter->second;
      if ( vertex->size_tracks() > maxtracks )
	{
	  maxvertex = vertex;
	  maxtracks = vertex->size_tracks();
	}
    }
  if ( !maxvertex )
    {
      cerr << PHWHERE << " ERROR: cannot get reconstructed vertex" << endl;
      return -1;
    }

  // --- Get the coordinates for the vertex from the evaluator
  PHG4VtxPoint* point = vertexeval->max_truth_point_by_ntracks(maxvertex);
  if ( !point )
    {
      cerr << PHWHERE << " ERROR: cannot get truth vertex" << endl;
      return -1;
    }
  _dx_vertex->Fill(maxvertex->get_x() - point->get_x());
  _dy_vertex->Fill(maxvertex->get_y() - point->get_y());
  _dz_vertex->Fill(maxvertex->get_z() - point->get_z());



  return 0;

}



int SimpleTrackingAnalysis::End(PHCompositeNode *topNode)
{
  cout << "End called. " << nevents << " events processed." << endl;
  return 0;
}
