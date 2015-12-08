
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
    _truept_dptoverpt(NULL),
    _truept_dca(NULL),
    _truept_particles_leaving7Hits(NULL),
    _truept_particles_recoWithExactHits(NULL),
    _truept_particles_recoWithin1Hit(NULL),
    _truept_particles_recoWithin2Hits(NULL),
    _truept_particles_recoWithin3Percent(NULL),
    _truept_particles_recoWithin4Percent(NULL),
    _truept_particles_recoWithin5Percent(NULL),
    _recopt_tracks_all(NULL),
    _recopt_tracks_recoWithExactHits(NULL),
    _recopt_tracks_recoWithin1Hit(NULL),
    _recopt_tracks_recoWithin2Hits(NULL),
    _recopt_tracks_recoWithin3Percent(NULL),
    _recopt_tracks_recoWithin4Percent(NULL),
    _recopt_tracks_recoWithin5Percent(NULL),
    _recopt_quality(NULL),
    _dx_vertex(NULL),
    _dy_vertex(NULL),
    _dz_vertex(NULL),
    _eff_vs_purity_pt_0_1(NULL),
    _eff_vs_purity_pt_1_2(NULL),
    _eff_vs_purity_pt_2_3(NULL),
    _eff_vs_purity_pt_3_4(NULL),
    _eff_vs_purity_pt_4_5(NULL),
    _eff_vs_purity_pt_5_6(NULL),
    _eff_vs_purity_pt_6_7(NULL) { 
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
			200,-0.03,0.03);

  _dy_vertex = new TH1D("dy_vertex",
			"dy_vertex",
			200,-0.03,0.03);

  _dz_vertex = new TH1D("dz_vertex",
			"dz_vertex",
			200,-0.03,0.03);
  
  _eff_vs_purity_pt_0_1 = new TH2D("eff_vs_purity_pt_0_1",
				   "eff_vs_purity_pt_0_1",
				   100,0.0,1.0,100,0.0,1.0);
  
  _eff_vs_purity_pt_1_2 = new TH2D("eff_vs_purity_pt_1_2",
				   "eff_vs_purity_pt_1_2",
				   100,0.0,1.0,100,0.0,1.0);

  _eff_vs_purity_pt_2_3 = new TH2D("eff_vs_purity_pt_2_3",
				   "eff_vs_purity_pt_2_3",
				   100,0.0,1.0,100,0.0,1.0);

  _eff_vs_purity_pt_3_4 = new TH2D("eff_vs_purity_pt_3_4",
				   "eff_vs_purity_pt_3_4",
				   100,0.0,1.0,100,0.0,1.0);

  _eff_vs_purity_pt_4_5 = new TH2D("eff_vs_purity_pt_4_5",
				   "eff_vs_purity_pt_4_5",
				   100,0.0,1.0,100,0.0,1.0);

  _eff_vs_purity_pt_5_6 = new TH2D("eff_vs_purity_pt_5_6",
				   "eff_vs_purity_pt_5_6",
				   100,0.0,1.0,100,0.0,1.0);

  _eff_vs_purity_pt_6_7 = new TH2D("eff_vs_purity_pt_6_7",
				   "eff_vs_purity_pt_6_7",
				   100,0.0,1.0,100,0.0,1.0);
  
  se->registerHisto(_truept_dptoverpt);                    

  se->registerHisto(_truept_dca);                          
 
  se->registerHisto(_truept_particles_leaving7Hits);       

  se->registerHisto(_truept_particles_recoWithExactHits);  
  se->registerHisto(_truept_particles_recoWithin1Hit);
  se->registerHisto(_truept_particles_recoWithin2Hits);

  se->registerHisto(_truept_particles_recoWithin3Percent); 
  se->registerHisto(_truept_particles_recoWithin4Percent);
  se->registerHisto(_truept_particles_recoWithin5Percent);

  se->registerHisto(_recopt_tracks_all);                   
 
  se->registerHisto(_recopt_tracks_recoWithExactHits);     
  se->registerHisto(_recopt_tracks_recoWithin1Hit);
  se->registerHisto(_recopt_tracks_recoWithin2Hits);

  se->registerHisto(_recopt_tracks_recoWithin3Percent);    
  se->registerHisto(_recopt_tracks_recoWithin4Percent);
  se->registerHisto(_recopt_tracks_recoWithin5Percent);

  se->registerHisto(_recopt_quality);                     
 
  se->registerHisto(_dx_vertex);                          
  se->registerHisto(_dy_vertex);
  se->registerHisto(_dz_vertex);

  se->registerHisto(_eff_vs_purity_pt_0_1);
  se->registerHisto(_eff_vs_purity_pt_1_2);
  se->registerHisto(_eff_vs_purity_pt_2_3);
  se->registerHisto(_eff_vs_purity_pt_3_4);
  se->registerHisto(_eff_vs_purity_pt_4_5);
  se->registerHisto(_eff_vs_purity_pt_5_6);
  se->registerHisto(_eff_vs_purity_pt_6_7);
  
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

  //SvtxVertexEval*   vertexeval = svtxevalstack.get_vertex_eval();
  SvtxTrackEval*     trackeval = svtxevalstack.get_track_eval();
  SvtxTruthEval*     trutheval = svtxevalstack.get_truth_eval();
  
  // loop over all truth particles
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
  for (PHG4TruthInfoContainer::ConstIterator iter = range.first; 
       iter != range.second; 
       ++iter) {
    PHG4Particle* g4particle = iter->second;
    if (trutheval->get_embed(g4particle) == 0) continue;
    
    std::set<PHG4Hit*> g4hits = trutheval->all_truth_hits(g4particle);     
    float ng4hits = g4hits.size();  

    float truept = sqrt(pow(g4particle->get_px(),2)+pow(g4particle->get_py(),2));
    
    // examine truth particles that leave 7 detector hits
    if (ng4hits == 7) {
      _truept_particles_leaving7Hits->Fill(truept);
    
      SvtxTrack* track = trackeval->best_track_from(g4particle);
      if (!track) continue;

      unsigned int nfromtruth = trackeval->get_nclusters_contribution(track,g4particle);
      float recopt = track->get_pt();

      unsigned int ndiff = abs((int)nfromtruth-7);
      if (ndiff <= 2) {
	_truept_particles_recoWithin2Hits->Fill(truept);
      }
      if (ndiff <= 1) {
	_truept_particles_recoWithin1Hit->Fill(truept);
      }
      if (ndiff == 0) {
	_truept_particles_recoWithExactHits->Fill(truept);
      } 
     
      float diff = fabs(recopt-truept)/truept;
      if (diff < 0.05) {
	_truept_particles_recoWithin5Percent->Fill(truept);
      }
      if (diff < 0.04) {
	_truept_particles_recoWithin4Percent->Fill(truept);
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

    if (trutheval->get_embed(g4particle) != 0) {
      // embedded results (quality or preformance measures)
      _truept_dptoverpt->Fill(truept,(recopt-truept)/truept);
      _truept_dca->Fill(truept,track->get_dca2d());

      _recopt_quality->Fill(recopt,track->get_quality());      
    } else {
      // non-embedded results (purity measures)
      _recopt_tracks_all->Fill(recopt);

      unsigned int nfromtruth = trackeval->get_nclusters_contribution(track,g4particle);

      unsigned int ndiff = abs((int)nfromtruth-7);
      if (ndiff <= 2) {
	_recopt_tracks_recoWithin2Hits->Fill(recopt);
      }
      if (ndiff <= 1) {
	_recopt_tracks_recoWithin1Hit->Fill(recopt);
      }
      if (ndiff == 0) {
	_recopt_tracks_recoWithExactHits->Fill(recopt);
      }
     
      float diff = fabs(recopt-truept)/truept;
      if (diff < 0.05) {
	_recopt_tracks_recoWithin5Percent->Fill(recopt);
      }
      if (diff < 0.04) {
	_recopt_tracks_recoWithin4Percent->Fill(recopt);
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

  //PHG4VtxPoint* point = vertexeval->max_truth_point_by_ntracks(maxvertex);

  _dx_vertex->Fill(maxvertex->get_x());// - point->get_x());
  _dy_vertex->Fill(maxvertex->get_y());// - point->get_y());
  _dz_vertex->Fill(maxvertex->get_z());// - point->get_z());

  TH2D *histo = NULL;

  // loop over pt window
  for (float ptmin = 0.0; ptmin < 5.1; ptmin += 1.0) {
    float ptmax = ptmin + 1.0;

    if      (ptmin == 0.0) histo = _eff_vs_purity_pt_0_1;
    else if (ptmin == 1.0) histo = _eff_vs_purity_pt_1_2;
    else if (ptmin == 2.0) histo = _eff_vs_purity_pt_2_3;
    else if (ptmin == 3.0) histo = _eff_vs_purity_pt_3_4;
    else if (ptmin == 4.0) histo = _eff_vs_purity_pt_4_5;
    else if (ptmin == 5.0) histo = _eff_vs_purity_pt_5_6;
    else if (ptmin == 6.0) histo = _eff_vs_purity_pt_6_7;
    else continue;

    // loop over chisq cut
    for (float chisqcut = 0.1; chisqcut < 10.1; chisqcut += 0.1) {

      unsigned int ntracks = 0;
      unsigned int puretracks = 0;
      
      // purity axis

      // answer: how often does a non-embedded track that passes the chisq cut trace to
      // a truth particle within 4% in pt
      
      for (SvtxTrackMap::Iter iter = trackmap->begin();
	   iter != trackmap->end();
	   ++iter) {
	
	SvtxTrack*    track      = iter->second;
	float recopt = track->get_pt();
	if (recopt < ptmin) continue;
	if (recopt > ptmax) continue;
	
	PHG4Particle* g4particle = trackeval->max_truth_particle_by_nclusters(track);
	if (trutheval->get_embed(g4particle) != 0) continue;

	float truept = sqrt(pow(g4particle->get_px(),2)+pow(g4particle->get_py(),2));
	
	++ntracks;

	if (track->get_quality() < chisqcut) {
	  if (fabs(recopt-truept)/truept < 0.04) {
	    ++puretracks;
	  }
	}
      }

      unsigned int nparticles = 0;
      unsigned int recoparticles = 0;
      
      // efficiency axis

      // answer: how often does an embedded truth particle leaving 7-hits get reconstructed
      // with a chisq better than the cut
      
      PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
      for (PHG4TruthInfoContainer::ConstIterator iter = range.first; 
	   iter != range.second; 
	   ++iter) {
	PHG4Particle* g4particle = iter->second;
	if (trutheval->get_embed(g4particle) == 0) continue;
	
	float truept = sqrt(pow(g4particle->get_px(),2)+pow(g4particle->get_py(),2));
	if (truept < ptmin) continue;
	if (truept > ptmax) continue;
	
	std::set<PHG4Hit*> g4hits = trutheval->all_truth_hits(g4particle);     
	float ng4hits = g4hits.size();  
		
	// examine truth particles that leave 7 detector hits
	if (ng4hits == 7) {
	  ++nparticles;
	  
	  SvtxTrack* track = trackeval->best_track_from(g4particle);
	  if (!track) continue;

	  if (track->get_quality() < chisqcut) {
	    ++recoparticles;
	  }
	} 
      }

      if ((nparticles != 0)&&(ntracks != 0)) {
	histo->Fill(recoparticles/nparticles,puretracks/ntracks);
	cout << ptmin << " " << chisqcut << " " << recoparticles/nparticles << " " << puretracks/ntracks << endl;
      }      
    }
  }
  
  return 0;
}

int SvtxSimPerformanceCheckReco::End(PHCompositeNode *topNode) {
 return 0;
}
