
#include "SvtxSimPerformanceCheckReco.h"

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

SvtxSimPerformanceCheckReco::SvtxSimPerformanceCheckReco(const string &name)
  : SubsysReco(name),
    _event(0),
    _nlayers(7),
    _inner_layer_mask(0x7),
    _truept_dptoverpt(NULL),
    _truept_dca(NULL),
    _truept_particles_leaving7Hits(NULL),
    _truept_particles_recoWithExactHits(NULL),
    _truept_particles_recoWithin1Hit(NULL),
    _truept_particles_recoWithin2Hits(NULL),
    _truept_particles_recoWithExactInnerHits(NULL),
    _truept_particles_recoWithin1InnerHit(NULL),
    _truept_particles_recoWithin2InnerHits(NULL),
    _truept_particles_recoWithin3Percent(NULL),
    _truept_particles_recoWithin4Percent(NULL),
    _truept_particles_recoWithin5Percent(NULL),
    _recopt_tracks_all(NULL),
    _recopt_tracks_recoWithExactHits(NULL),
    _recopt_tracks_recoWithin1Hit(NULL),
    _recopt_tracks_recoWithin2Hits(NULL),
    _recopt_tracks_recoWithExactInnerHits(NULL),
    _recopt_tracks_recoWithin1InnerHit(NULL),
    _recopt_tracks_recoWithin2InnerHits(NULL),
    _recopt_tracks_recoWithin3Percent(NULL),
    _recopt_tracks_recoWithin4Percent(NULL),
    _recopt_tracks_recoWithin5Percent(NULL),
    _recopt_quality(NULL),
    _dx_vertex(NULL),
    _dy_vertex(NULL),
    _dz_vertex(NULL),
    _truept_quality_particles_recoWithin4Percent(NULL),
    _recopt_quality_tracks_all(NULL),
    _recopt_quality_tracks_recoWithin4Percent(NULL) { 
}

int SvtxSimPerformanceCheckReco::Init(PHCompositeNode *topNode) {

  // register histograms
  Fun4AllServer *se = Fun4AllServer::instance();

  _truept_dptoverpt = new TH2D("truept_dptoverpt",
			       "truept_dptoverpt",
			       40,0.0,40.0,
			       200,-0.5,0.5);
  
  _truept_dca = new TH2D("truept_dca",
			 "truept_dca",
			 20,0.0,10.0,
			 200,-0.1,0.1);

  _truept_particles_leaving7Hits = new TH1D("truept_particles_leaving7Hits",
					    "truept_particles_leaving7Hits",
					    20,0.0,10.0);

  _truept_particles_recoWithExactHits = new TH1D("truept_particles_recoWithExactHits",
						 "truept_particles_recoWithExactHits",
						 20,0.0,10.0);

  _truept_particles_recoWithin1Hit = new TH1D("truept_particles_recoWithin1Hit",
					      "truept_particles_recoWithin1Hit",
					      20,0.0,10.0);

  _truept_particles_recoWithin2Hits = new TH1D("truept_particles_recoWithin2Hits",
					       "truept_particles_recoWithin2Hits",
					       20,0.0,10.0);

  _truept_particles_recoWithExactInnerHits = new TH1D("truept_particles_recoWithExactInnerHits",
						      "truept_particles_recoWithExactInnerHits",
						 20,0.0,10.0);

  _truept_particles_recoWithin1InnerHit = new TH1D("truept_particles_recoWithin1InnerHit",
						   "truept_particles_recoWithin1InnerHit",
						   20,0.0,10.0);
  
  _truept_particles_recoWithin2InnerHits = new TH1D("truept_particles_recoWithin2InnerHits",
						    "truept_particles_recoWithin2InnerHits",
						    20,0.0,10.0);
  
  
  _truept_particles_recoWithin3Percent = new TH1D("truept_particles_recoWithin3Percent",
						  "truept_particles_recoWithin3Percent",
						  20,0.0,10.0);

  _truept_particles_recoWithin4Percent = new TH1D("truept_particles_recoWithin4Percent",
						  "truept_particles_recoWithin4Percent",
						  20,0.0,10.0);

  _truept_particles_recoWithin5Percent = new TH1D("truept_particles_recoWithin5Percent",
						  "truept_particles_recoWithin5Percent",
						  20,0.0,10.0);

  _recopt_tracks_all = new TH1D("recopt_tracks_all",
				"recopt_tracks_all",
				20,0.0,10.0);

  _recopt_tracks_recoWithExactHits = new TH1D("recopt_tracks_recoWithExactHits",
					      "recopt_tracks_recoWithExactHits",
					      20,0.0,10.0);

  _recopt_tracks_recoWithin1Hit = new TH1D("recopt_tracks_recoWithin1Hit",
					   "recopt_tracks_recoWithin1Hit",
					   20,0.0,10.0);
  
  _recopt_tracks_recoWithin2Hits = new TH1D("recopt_tracks_recoWithin2Hits",
					    "recopt_tracks_recoWithin2Hits",
					    20,0.0,10.0);

  _recopt_tracks_recoWithExactInnerHits = new TH1D("recopt_tracks_recoWithExactInnerHits",
					      "recopt_tracks_recoWithExactInnerHits",
					      20,0.0,10.0);

  _recopt_tracks_recoWithin1InnerHit = new TH1D("recopt_tracks_recoWithin1InnerHit",
					   "recopt_tracks_recoWithin1InnerHit",
					   20,0.0,10.0);
  
  _recopt_tracks_recoWithin2InnerHits = new TH1D("recopt_tracks_recoWithin2InnerHits",
					    "recopt_tracks_recoWithin2InnerHits",
					    20,0.0,10.0);
  
  _recopt_tracks_recoWithin3Percent = new TH1D("recopt_tracks_recoWithin3Percent",
					    "recopt_tracks_recoWithin3Percent",
					    20,0.0,10.0);

  _recopt_tracks_recoWithin4Percent = new TH1D("recopt_tracks_recoWithin4Percent",
					       "recopt_tracks_recoWithin4Percent",
					       20,0.0,10.0);
  
  _recopt_tracks_recoWithin5Percent = new TH1D("recopt_tracks_recoWithin5Percent",
					       "recopt_tracks_recoWithin5Percent",
					       20,0.0,10.0);

  _recopt_quality = new TH2D("recopt_quality",
			     "recopt_quality",
			     20,0.0,10.0,
			     100,0.0,5.0);

  _dx_vertex = new TH1D("dx_vertex",
			"dx_vertex",
			200,-0.01,0.01);

  _dy_vertex = new TH1D("dy_vertex",
			"dy_vertex",
			200,-0.01,0.01);

  _dz_vertex = new TH1D("dz_vertex",
			"dz_vertex",
			200,-0.01,0.01);
  
  _truept_quality_particles_recoWithin4Percent = new TH2D("truept_quality_particles_recoWithin4Percent",
							  "truept_quality_particles_recoWithin4Percent",
							  20,0.0,10.0,
							  100,0.0,5.0);

  _recopt_quality_tracks_all = new TH2D("recopt_quality_tracks_all",
					"recopt_quality_tracks_all",
					20,0.0,10.0,
					100,0.0,5.0);

  _recopt_quality_tracks_recoWithin4Percent = new TH2D("recopt_quality_tracks_recoWithin4Percent",
						       "recopt_quality_tracks_recoWithin4Percent",
						       20,0.0,10.0,
						       100,0.0,5.0);
  
  
  se->registerHisto(_truept_dptoverpt);                    

  se->registerHisto(_truept_dca);                          
 
  se->registerHisto(_truept_particles_leaving7Hits);       

  se->registerHisto(_truept_particles_recoWithExactHits);  
  se->registerHisto(_truept_particles_recoWithin1Hit);
  se->registerHisto(_truept_particles_recoWithin2Hits);

  se->registerHisto(_truept_particles_recoWithExactInnerHits);  
  se->registerHisto(_truept_particles_recoWithin1InnerHit);
  se->registerHisto(_truept_particles_recoWithin2InnerHits);
  
  se->registerHisto(_truept_particles_recoWithin3Percent); 
  se->registerHisto(_truept_particles_recoWithin4Percent);
  se->registerHisto(_truept_particles_recoWithin5Percent);

  se->registerHisto(_recopt_tracks_all);                   
 
  se->registerHisto(_recopt_tracks_recoWithExactHits);     
  se->registerHisto(_recopt_tracks_recoWithin1Hit);
  se->registerHisto(_recopt_tracks_recoWithin2Hits);

  se->registerHisto(_recopt_tracks_recoWithExactInnerHits);     
  se->registerHisto(_recopt_tracks_recoWithin1InnerHit);
  se->registerHisto(_recopt_tracks_recoWithin2InnerHits);
  
  se->registerHisto(_recopt_tracks_recoWithin3Percent);    
  se->registerHisto(_recopt_tracks_recoWithin4Percent);
  se->registerHisto(_recopt_tracks_recoWithin5Percent);

  se->registerHisto(_recopt_quality);                     
 
  se->registerHisto(_dx_vertex);                          
  se->registerHisto(_dy_vertex);
  se->registerHisto(_dz_vertex);

  se->registerHisto(_truept_quality_particles_recoWithin4Percent);
  se->registerHisto(_recopt_quality_tracks_all);
  se->registerHisto(_recopt_quality_tracks_recoWithin4Percent);
  
  return 0;
}

int SvtxSimPerformanceCheckReco::process_event(PHCompositeNode *topNode) {

  ++_event;
  
  // need things off of the DST...
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
  if (!truthinfo) {
    cerr << PHWHERE << " ERROR: Can't find G4TruthInfo" << endl;
    exit(-1);
  }

  SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
  if (!trackmap) {
    cerr << PHWHERE << " ERROR: Can't find SvtxTrackMap" << endl;
    exit(-1);
  }

  SvtxVertexMap* vertexmap = findNode::getClass<SvtxVertexMap>(topNode,"SvtxVertexMap");
  if (!vertexmap) {
    cerr << PHWHERE << " ERROR: Can't find SvtxVertexMap" << endl;
    exit(-1);
  }

  // create SVTX eval stack
  SvtxEvalStack svtxevalstack(topNode);

  SvtxVertexEval*   vertexeval = svtxevalstack.get_vertex_eval();
  SvtxTrackEval*     trackeval = svtxevalstack.get_track_eval();
  SvtxTruthEval*     trutheval = svtxevalstack.get_truth_eval();
  
  // loop over all truth particles
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
  for (PHG4TruthInfoContainer::ConstIterator iter = range.first; 
       iter != range.second; 
       ++iter) {
    PHG4Particle* g4particle = iter->second;
    if (trutheval->get_embed(g4particle) <= 0) continue;
    
    std::set<PHG4Hit*> g4hits = trutheval->all_truth_hits(g4particle);     
    float ng4hits = g4hits.size();  

    float truept = sqrt(pow(g4particle->get_px(),2)+pow(g4particle->get_py(),2));
    
    // examine truth particles that leave 7 detector hits
    if (ng4hits == _nlayers) {
      _truept_particles_leaving7Hits->Fill(truept);
    
      SvtxTrack* track = trackeval->best_track_from(g4particle);
    
      if (!track) {continue;}
      
      unsigned int nfromtruth = trackeval->get_nclusters_contribution(track,g4particle);
      float recopt = track->get_pt();

      unsigned int ndiff = abs((int)nfromtruth-(int)_nlayers);
      if (ndiff <= 2) {
	_truept_particles_recoWithin2Hits->Fill(truept);
      }
      if (ndiff <= 1) {
	_truept_particles_recoWithin1Hit->Fill(truept);
      }
      if (ndiff == 0) {
	_truept_particles_recoWithExactHits->Fill(truept);
      } 

      unsigned int layersfromtruth = trackeval->get_nclusters_contribution_by_layer(track,g4particle);
      unsigned int innerhits = (layersfromtruth & _inner_layer_mask);
      unsigned int ninnerhitsfromtruth = 0;
      unsigned int ninnerlayers = 0;
      for (unsigned int i = 0; i < 32; ++i) {
	ninnerhitsfromtruth += (0x1 & (innerhits >> i));
	ninnerlayers += (0x1 & (_inner_layer_mask >> i));
      }
      
      ndiff = abs((int)ninnerhitsfromtruth-(int)ninnerlayers);
      if (ndiff <= 2) {
	_truept_particles_recoWithin2InnerHits->Fill(truept);
      }
      if (ndiff <= 1) {
	_truept_particles_recoWithin1InnerHit->Fill(truept);
      }
      if (ndiff == 0) {
	_truept_particles_recoWithExactInnerHits->Fill(truept);
      } 
      
      float diff = fabs(recopt-truept)/truept;
      if (diff < 0.05) {
	_truept_particles_recoWithin5Percent->Fill(truept);
      }
      if (diff < 0.04) {
	_truept_particles_recoWithin4Percent->Fill(truept);
	_truept_quality_particles_recoWithin4Percent->Fill(truept,track->get_quality());
      }
      if (diff < 0.03) {
	_truept_particles_recoWithin3Percent->Fill(truept);
      }      
    }    
  }

  // loop over all reco particles
  for (SvtxTrackMap::Iter iter = trackmap->begin();
       iter != trackmap->end();
       ++iter) {
    
    SvtxTrack*    track      = iter->second;
    float recopt = track->get_pt();
        
    PHG4Particle* g4particle = trackeval->max_truth_particle_by_nclusters(track);
    float truept = sqrt(pow(g4particle->get_px(),2)+pow(g4particle->get_py(),2));

    if (trutheval->get_embed(g4particle) > 0) {
      // embedded results (quality or preformance measures)
      _truept_dptoverpt->Fill(truept,(recopt-truept)/truept);
      _truept_dca->Fill(truept,track->get_dca2d());

      _recopt_quality->Fill(recopt,track->get_quality());      
    } else {
      // non-embedded results (purity measures)
      _recopt_tracks_all->Fill(recopt);
      _recopt_quality_tracks_all->Fill(recopt,track->get_quality());

      unsigned int nfromtruth = trackeval->get_nclusters_contribution(track,g4particle);

      unsigned int ndiff = abs((int)nfromtruth-(int)_nlayers);
      if (ndiff <= 2) {
	_recopt_tracks_recoWithin2Hits->Fill(recopt);
      }
      if (ndiff <= 1) {
	_recopt_tracks_recoWithin1Hit->Fill(recopt);
      }
      if (ndiff == 0) {
	_recopt_tracks_recoWithExactHits->Fill(recopt);
      }

      unsigned int layersfromtruth = trackeval->get_nclusters_contribution_by_layer(track,g4particle);
      unsigned int innerhits = (layersfromtruth & _inner_layer_mask);
      unsigned int ninnerhitsfromtruth = 0;
      unsigned int ninnerlayers = 0;
      for (unsigned int i = 0; i < 32; ++i) {
	ninnerhitsfromtruth += (0x1 & (innerhits >> i));
	ninnerlayers += (0x1 & (_inner_layer_mask >> i));
      }
      
      ndiff = abs((int)ninnerhitsfromtruth-(int)ninnerlayers);
      if (ndiff <= 2) {
	_recopt_tracks_recoWithin2InnerHits->Fill(recopt);
      }
      if (ndiff <= 1) {
	_recopt_tracks_recoWithin1InnerHit->Fill(recopt);
      }
      if (ndiff == 0) {
	_recopt_tracks_recoWithExactInnerHits->Fill(recopt);
      } 
     
      float diff = fabs(recopt-truept)/truept;
      if (diff < 0.05) {
	_recopt_tracks_recoWithin5Percent->Fill(recopt);
      }
      if (diff < 0.04) {
	_recopt_tracks_recoWithin4Percent->Fill(recopt);
	_recopt_quality_tracks_recoWithin4Percent->Fill(recopt,track->get_quality());
      }
      if (diff < 0.03) {
	_recopt_tracks_recoWithin3Percent->Fill(recopt);
      }            
    }
  }

  // get leading vertex
  SvtxVertex* maxvertex = NULL;
  unsigned int maxtracks = 0;  
  for (SvtxVertexMap::Iter iter = vertexmap->begin();
       iter != vertexmap->end();
       ++iter) {
    SvtxVertex* vertex = iter->second;

    if (vertex->size_tracks() > maxtracks) {
      maxvertex = vertex;
      maxtracks = vertex->size_tracks();
    }    
  }
  
  if (maxvertex) {
    PHG4VtxPoint* point = vertexeval->max_truth_point_by_ntracks(maxvertex);

    if (point) {
      _dx_vertex->Fill(maxvertex->get_x() - point->get_x());
      _dy_vertex->Fill(maxvertex->get_y() - point->get_y());
      _dz_vertex->Fill(maxvertex->get_z() - point->get_z());
    }
  }

  return 0;
}

int SvtxSimPerformanceCheckReco::End(PHCompositeNode *topNode) {
 return 0;
}
