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

  _recopt_tracks_all = new TH1D("recopt_tracks_all", "recopt_tracks_all", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_all);

  _recopt_tracks_recoWithExactHits = new TH1D("recopt_tracks_recoWithExactHits", "recopt_tracks_recoWithExactHits", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_recoWithExactHits);

  _recopt_tracks_recoWithin1Hit = new TH1D("recopt_tracks_recoWithin1Hit", "recopt_tracks_recoWithin1Hit", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_recoWithin1Hit);

  _recopt_tracks_recoWithin2Hits = new TH1D("recopt_tracks_recoWithin2Hits", "recopt_tracks_recoWithin2Hits", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_recoWithin2Hits);

  _recopt_tracks_recoWithin3Percent = new TH1D("recopt_tracks_recoWithin3Percent", "recopt_tracks_recoWithin3Percent", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_recoWithin3Percent);

  _recopt_tracks_recoWithin4Percent = new TH1D("recopt_tracks_recoWithin4Percent", "recopt_tracks_recoWithin4Percent", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_recoWithin4Percent);

  _recopt_tracks_recoWithin5Percent = new TH1D("recopt_tracks_recoWithin5Percent", "recopt_tracks_recoWithin5Percent", 20,0.0,10.0);
  se->registerHisto(_recopt_tracks_recoWithin5Percent);

  _recopt_quality = new TH2D("recopt_quality", "recopt_quality", 20,0.0,10.0, 100,0.0,5.0);
  se->registerHisto(_recopt_quality);



  // --- vertex residual histograms

  _dx_vertex = new TH1D("dx_vertex", "dx_vertex", 200,-0.03,0.03);
  se->registerHisto(_dx_vertex);

  _dy_vertex = new TH1D("dy_vertex", "dy_vertex", 200,-0.03,0.03);
  se->registerHisto(_dy_vertex);

  _dz_vertex = new TH1D("dz_vertex", "dz_vertex", 200,-0.03,0.03);
  se->registerHisto(_dz_vertex);



  // --- additional histograms

  _truept_quality_particles_recoWithin4Percent = new TH2D("truept_quality_particles_recoWithin4Percent", "truept_quality_particles_recoWithin4Percent", 20,0.0,10.0, 100,0.0,5.0);
  se->registerHisto(_truept_quality_particles_recoWithin4Percent);

  _recopt_quality_tracks_all = new TH2D("recopt_quality_tracks_all", "recopt_quality_tracks_all", 20,0.0,10.0, 100,0.0,5.0);
  se->registerHisto(_recopt_quality_tracks_all);

  _recopt_quality_tracks_recoWithin4Percent = new TH2D("recopt_quality_tracks_recoWithin4Percent", "recopt_quality_tracks_recoWithin4Percent", 20,0.0,10.0, 100,0.0,5.0);
  se->registerHisto(_recopt_quality_tracks_recoWithin4Percent);



  return 0;

}



int SimpleTrackingAnalysis::process_event(PHCompositeNode *topNode)
{

  // --- This is the class process_event method
  // --- This is where the bulk of the analysis is done
  // --- Here we get the various data nodes we need to do the analysis
  // --- Then we use variables (accessed through class methods) to perform calculations

  //cout << "Now processing event number " << nevents << endl; // would be good to add verbosity switch

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

  // --- Create SVTX eval stack
  SvtxEvalStack svtxevalstack(topNode);
  // --- Get evaluator objects from the eval stack
  SvtxVertexEval*   vertexeval = svtxevalstack.get_vertex_eval();
  SvtxTrackEval*     trackeval = svtxevalstack.get_track_eval();
  SvtxTruthEval*     trutheval = svtxevalstack.get_truth_eval();

  // --- Loop over all truth particles
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
  for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter )
    {
      PHG4Particle* g4particle = iter->second; // You may ask yourself, why second?
      // In C++ the iterator is a map, which has two members
      // first is the key (analogous the index of an arry),
      // second is the value (analogous to the value stored for the array index)
      if ( trutheval->get_embed(g4particle ) == 0) continue;

      set<PHG4Hit*> g4hits = trutheval->all_truth_hits(g4particle);
      float ng4hits = g4hits.size();

      float truept = sqrt(pow(g4particle->get_px(),2)+pow(g4particle->get_py(),2));

      // examine truth particles that leave 7 detector hits
      if ( ng4hits == nlayers )
	{
	  _truept_particles_leaving7Hits->Fill(truept);

	  SvtxTrack* track = trackeval->best_track_from(g4particle);
	  if (!track) continue;

	  unsigned int nfromtruth = trackeval->get_nclusters_contribution(track,g4particle);
	  float recopt = track->get_pt();

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
