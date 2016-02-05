#include "SimpleTrackingAnalysis.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>

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
#include <g4cemc/RawCluster.h>
#include <g4cemc/RawClusterContainer.h>

#include <TH1D.h>
#include <TH2D.h>

#include <iostream>

using namespace std;



SimpleTrackingAnalysis::SimpleTrackingAnalysis(const string &name) : SubsysReco(name)
{
  cout << "Class constructor called " << endl;
  nevents = 0;
  nlayers = 7;
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



  // --- histograms over true pt

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



  // --- (mostly) the same set of histograms over reconstructed pt

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

  _recopt_quality = new TH2D("recopt_quality", "", 20,0.0,10.0, 100,0.0,5.0);
  se->registerHisto(_recopt_quality);



  // --- vertex residual histograms

  _dx_vertex = new TH1D("dx_vertex", "dx_vertex", 200,-0.03,0.03);
  se->registerHisto(_dx_vertex);

  _dy_vertex = new TH1D("dy_vertex", "dy_vertex", 200,-0.03,0.03);
  se->registerHisto(_dy_vertex);

  _dz_vertex = new TH1D("dz_vertex", "dz_vertex", 200,-0.03,0.03);
  se->registerHisto(_dz_vertex);



  // --- additional tracking histograms

  _truept_quality_particles_recoWithin4Percent = new TH2D("truept_quality_particles_recoWithin4Percent", "", 20,0.0,10.0, 100,0.0,5.0);
  se->registerHisto(_truept_quality_particles_recoWithin4Percent);

  _recopt_quality_tracks_all = new TH2D("recopt_quality_tracks_all", "", 20,0.0,10.0, 100,0.0,5.0);
  se->registerHisto(_recopt_quality_tracks_all);

  _recopt_quality_tracks_recoWithin4Percent = new TH2D("recopt_quality_tracks_recoWithin4Percent", "", 20,0.0,10.0, 100,0.0,5.0);
  se->registerHisto(_recopt_quality_tracks_recoWithin4Percent);



  // --- some basic calorimeter performance histograms

  _energy_difference_emc = new TH2D("energy_difference_emc", "", 20,0.0,10.0, 100,0.0,1.0);
  _energy_difference_hci = new TH2D("energy_difference_hci", "", 20,0.0,10.0, 100,0.0,1.0);
  _energy_difference_hco = new TH2D("energy_difference_hco", "", 20,0.0,10.0, 100,0.0,1.0);
  se->registerHisto(_energy_difference_emc);
  se->registerHisto(_energy_difference_hci);
  se->registerHisto(_energy_difference_hco);

  _energy_ratio_emc = new TH2D("energy_ratio_emc", "", 20,0.0,10.0, 100,0.0,1.0);
  _energy_ratio_hci = new TH2D("energy_ratio_hci", "", 20,0.0,10.0, 100,0.0,1.0);
  _energy_ratio_hco = new TH2D("energy_ratio_hco", "", 20,0.0,10.0, 100,0.0,1.0);
  se->registerHisto(_energy_ratio_emc);
  se->registerHisto(_energy_ratio_hci);
  se->registerHisto(_energy_ratio_hco);

  return 0;

}



int SimpleTrackingAnalysis::process_event(PHCompositeNode *topNode)
{

  // --- This is the class process_event method
  // --- This is where the bulk of the analysis is done
  // --- Here we get the various data nodes we need to do the analysis
  // --- Then we use variables (accessed through class methods) to perform calculations

  cout << "Now processing event number " << nevents << endl; // would be good to add verbosity switch

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
  RawClusterContainer *emc_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  RawClusterContainer *hci_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALIN");
  RawClusterContainer *hco_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALOUT");
  if ( !emc_clusters || !hci_clusters || !hco_clusters )
    {
      cerr << PHWHERE << " WARNING: Can't find cluster nodes" << endl;
      cerr << PHWHERE << "  emc_clusters " << emc_clusters << endl;
      cerr << PHWHERE << "  hci_clusters " << hci_clusters << endl;
      cerr << PHWHERE << "  hco_clusters " << hco_clusters << endl;
      clusters_available = false;
    }



  // --- Create SVTX eval stack
  SvtxEvalStack svtxevalstack(topNode);
  // --- Get evaluator objects from the eval stack
  SvtxVertexEval*   vertexeval = svtxevalstack.get_vertex_eval();
  SvtxTrackEval*     trackeval = svtxevalstack.get_track_eval();
  SvtxTruthEval*     trutheval = svtxevalstack.get_truth_eval();



  cout << "Now going to loop over truth partcles..." << endl; // need verbosity switch

  // --- Loop over all truth particles
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
  for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter )
    {
      PHG4Particle* g4particle = iter->second; // You may ask yourself, why second?
      // In C++ the iterator is a map, which has two members
      // first is the key (analogous the index of an arry),
      // second is the value (analogous to the value stored for the array index)
      if ( trutheval->get_embed(g4particle) == 0 ) continue; // only look at embedded particles (for now)

      set<PHG4Hit*> g4hits = trutheval->all_truth_hits(g4particle);
      float ng4hits = g4hits.size();

      float truept = sqrt(pow(g4particle->get_px(),2)+pow(g4particle->get_py(),2));
      float true_energy = g4particle->get_e();

      // --- Get the reconsructed SvtxTrack based on the best candidate from the truth info
      SvtxTrack* track = trackeval->best_track_from(g4particle);
      if (!track) continue;
      float recopt = track->get_pt();

      cout << "truept is " << truept << endl;
      cout << "recopt is " << recopt << endl;



      // ---------------------
      // --- calorimeter stuff
      // ---------------------

      // --- emcal variables
      float emc_energy = -9999;
      float emc_eta = -9999;
      float emc_pt = -9999;

      // --- inner hcal variables
      float hci_energy = -9999;
      float hci_eta = -9999;
      float hci_pt = -9999;

      // --- outer hcal variables
      float hco_energy = -9999;
      float hco_eta = -9999;
      float hco_pt = -9999;

      // --- energy variables directly from the track object
      float emc_energy_track = -9999;
      float hci_energy_track = -9999;
      float hco_energy_track = -9999;

      if ( clusters_available )
	{
	  // --- get the energy values directly from the track
	  emc_energy_track = track->get_cal_cluster_e(SvtxTrack::CEMC);
	  hci_energy_track = track->get_cal_cluster_e(SvtxTrack::HCALIN);
	  hco_energy_track = track->get_cal_cluster_e(SvtxTrack::HCALOUT);

	  // --- get the cluster id number for the best match to the track for each calorimeter
	  int emcID = (int)track->get_cal_cluster_id(SvtxTrack::CEMC);
	  int hciID = (int)track->get_cal_cluster_id(SvtxTrack::HCALIN);
	  int hcoID = (int)track->get_cal_cluster_id(SvtxTrack::HCALOUT);

	  // --- get the single raw clusters for each calorimeter based on the id number
	  RawCluster* emc_cluster_matched = emc_clusters->getCluster(emcID);
	  RawCluster* hci_cluster_matched = hci_clusters->getCluster(hciID);
	  RawCluster* hco_cluster_matched = hco_clusters->getCluster(hcoID);

	  // --- emcal variables
	  emc_energy = emc_cluster_matched->get_energy();
	  emc_eta = emc_cluster_matched->get_eta();
	  emc_pt = emc_energy/cosh(emc_eta);

	  // --- inner hcal variables
	  hci_energy = hci_cluster_matched->get_energy();
	  hci_eta = hci_cluster_matched->get_eta();
	  hci_pt = hci_energy/cosh(hci_eta);

	  // --- outer hcal variables
	  hco_energy = hco_cluster_matched->get_energy();
	  hco_eta = hco_cluster_matched->get_eta();
	  hco_pt = hco_energy/cosh(hco_eta);

	  // --- check the variables
	  cout << "emc_pt is " << emc_pt << endl;
	  cout << "hci_pt is " << hci_pt << endl;
	  cout << "hco_pt is " << hco_pt << endl;
	  cout << "emc_energy is " << emc_energy << endl;
	  cout << "hci_energy is " << hci_energy << endl;
	  cout << "hco_energy is " << hco_energy << endl;
	  cout << "emc_energy_track is " << emc_energy_track << endl;
	  cout << "hci_energy_track is " << hci_energy_track << endl;
	  cout << "hco_energy_track is " << hco_energy_track << endl;
	}

      float emc_ediff = (true_energy - emc_energy)/true_energy;
      float hci_ediff = (true_energy - hci_energy)/true_energy;
      float hco_ediff = (true_energy - hco_energy)/true_energy;

      float emc_eratio = emc_energy/true_energy;
      float hci_eratio = hci_energy/true_energy;
      float hco_eratio = hco_energy/true_energy;

      _energy_difference_emc->Fill(true_energy,emc_ediff);
      _energy_difference_hci->Fill(true_energy,hci_ediff);
      _energy_difference_hco->Fill(true_energy,hco_ediff);

      _energy_ratio_emc->Fill(true_energy,emc_eratio);
      _energy_ratio_hci->Fill(true_energy,hci_eratio);
      _energy_ratio_hco->Fill(true_energy,hco_eratio);



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

  // --- Get the coordinates for the vertex from the evaluator
  PHG4VtxPoint* point = vertexeval->max_truth_point_by_ntracks(maxvertex);
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
