
#include "TrackingPerformanceCheck.h"

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

#include "TMath.h"
#include <TH1F.h>
#include <TH2F.h>
#include <TH1D.h>
#include <TH2D.h>

#include <iostream>

using namespace std;

TrackingPerformanceCheck::TrackingPerformanceCheck(const string &name)
  : SubsysReco(name) {
}

int TrackingPerformanceCheck::Init(PHCompositeNode *topNode) {
  // register histograms
  Fun4AllServer *se = Fun4AllServer::instance();

  const int nptbinsPONE = 30;
  Int_t nptbins = nptbinsPONE - 1;
  Double_t ptbins[nptbinsPONE];
  ptbins[0] = 0.2;
  for(int i=1; i!=9; ++i)
    ptbins[i] = 0.2 + i*0.2;
  for(int i=0; i!=11; ++i)
    ptbins[9+i] = 2.0 + i*0.5;
  for(int i=0; i!=10; ++i)
    ptbins[20+i] = 7.0 + i*1.0;
  for(int i=0; i!=nptbinsPONE; ++i) std::cout << i << " " << ptbins[i] << std::endl;

  fHNEvents = new TH1F("Events","Events",1,-0.5,0.5);
  se->registerHisto(fHNEvents);

  fHNTruths = new TH1F("Truths0_N","Number of Truths",100,-0.5,9999.5);
  se->registerHisto(fHNTruths);
  fHNEmbedded = new TH1F("Truths1_N","Number of Embedded",100,-0.5,99.5);
  se->registerHisto(fHNEmbedded);
  fHNReconstructables = new TH1F("Truths2_N","Number of Reconstructables",100,-0.5,99.5);
  se->registerHisto(fHNReconstructables);
  for(int i=0; i!=3; ++i) {
    fPt[i] = new TH1F(Form("Truths%d_Pt",i),Form("Truths%d_Pt",i),nptbins,ptbins);
    fPhi[i] = new TH1F(Form("Truths%d_Phi",i),Form("Truths%d_Phi",i),100,0,TMath::TwoPi());
    fEta[i] = new TH1F(Form("Truths%d_Eta",i),Form("Truths%d_Eta",i),100,-10,+10);
    se->registerHisto(fPt[i]);
    se->registerHisto(fPhi[i]);
    se->registerHisto(fEta[i]);
  }
  fHEmbeddedNHits[0] = new TH1F("Truths1_Hits","Number of TPCHits in Embedded",61,-0.5,60.5);
  se->registerHisto(fHEmbeddedNHits[0]);
  fHEmbeddedNHits[1] = new TH1F("Truths2_Hits","Number of TPCHits in Reconstructable",61,-0.5,60.5);
  se->registerHisto(fHEmbeddedNHits[1]);

  fHNTracks = new TH1F("Tracks0_N","Number of Tracks",100,-0.5,9999.5);
  se->registerHisto(fHNTracks);
  fHNTracksMatched = new TH1F("Tracks1_N","Number of Tracks Matched to Truth",100,-0.5,9999.5);
  se->registerHisto(fHNTracksMatched);
  fHNTracksEmbedded = new TH1F("Tracks2_N","Number of Tracks Matched to Embedded",100,-0.5,99.5);
  se->registerHisto(fHNTracksEmbedded);
  fHNTracksReconstructable = new TH1F("Tracks3_N","Number of Tracks Matched to Reconstructable",100,-0.5,99.5);
  se->registerHisto(fHNTracksReconstructable);
  for(int i=0; i!=4; ++i) {
    fHRPt[i] = new TH1F(Form("Tracks%d_Pt",i),Form("Tracks%d_Pt",i),nptbins,ptbins);
    fHRPhi[i] = new TH1F(Form("Tracks%d_Phi",i),Form("Tracks%d_Phi",i),100,0,TMath::TwoPi());
    fHREta[i] = new TH1F(Form("Tracks%d_Eta",i),Form("Tracks%d_Eta",i),100,-10,+10);
    fHRDca2D[i] = new TH2F( Form("Tracks%d_Dca2D",i),Form("Dca2D %d;PT;DCA2D",i),nptbins,ptbins,200,-1.0,1.0);
    se->registerHisto(fHRPt[i]);
    se->registerHisto(fHRPhi[i]);
    se->registerHisto(fHREta[i]);
    se->registerHisto(fHRDca2D[i]);
  }
  for(int i=0; i!=3; ++i) {
    fHNClustersContribution[i] = new TH1F(Form("Tracks%d_NClusters",i+1),
					  "Number of Clusters Belonging to Truth",100,-0.5,99.5);
    se->registerHisto(fHNClustersContribution[i]);
    fHPtResolution[i] = new TH2F(Form("Tracks%d_ResPt",i+1),
				 "PtResolution;PT;REL DIFF",
				 nptbins,ptbins,200,-5.0,5.0);
    se->registerHisto(fHPtResolution[i]);
    fHPhiResolution[i] = new TH2F(Form("Tracks%d_ResPhi",i+1),
				  "PhiResolution;PT;REL DIFF",
				  nptbins,ptbins,200,-0.1,+0.1);
    se->registerHisto(fHPhiResolution[i]);
    fHEtaResolution[i] = new TH2F(Form("Tracks%d_ResEta",i+1),
				  "EtaResolution;PT;REL DIFF",
				  nptbins,ptbins,200,-0.1,+0.1);
    se->registerHisto(fHEtaResolution[i]);
  }
  fHNVertexes = new TH1F("Vertexes_N","Number of Vertexes",10,-0.5,9.5);
  se->registerHisto(fHNVertexes);
  return 0;
}

int TrackingPerformanceCheck::process_event(PHCompositeNode *topNode) {
  std::cout << "TrackingPerformanceCheck [ENTER]" << std::endl;
  fHNEvents->Fill(0);
  fEmbedded.clear();
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
  SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
  SvtxVertexMap* vertexmap = findNode::getClass<SvtxVertexMap>(topNode,"SvtxVertexMap");
  if (!truthinfo) {
    cerr << PHWHERE << " ERROR: Can't find G4TruthInfo" << endl;
    exit(-1);
  }
  if (!trackmap) {
    cerr << PHWHERE << " ERROR: Can't find SvtxTrackMap" << endl;
    exit(-1);
  }
  if (!vertexmap) {
    cerr << PHWHERE << " ERROR: Can't find SvtxVertexMap" << endl;
    exit(-1);
  }

  SvtxEvalStack svtxevalstack(topNode);
  SvtxVertexEval *vertexeval = svtxevalstack.get_vertex_eval();
  SvtxTrackEval *trackeval = svtxevalstack.get_track_eval();
  SvtxTruthEval *trutheval = svtxevalstack.get_truth_eval();
  
  int nA=0;
  int nB=0;
  int nC=0;
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
  // We need a method to distinguish between embedded and reconstructables. The current flag for embeded is an int with zero and one, so naturally this can be extended to hold values higher than two. However the current list cannot be modified, because the only accessor returns const_iterator, so we either change that in the base class (PH4TruthInfoContainer) or we clone the list (what we currently do here). Since this list only concerns embedded particles, it is going to be small
  std::pair< std::map<int,int>::const_iterator, std::map<int,int>::const_iterator > rangeE = truthinfo->GetEmbeddedTrkIds();
  for(std::map<int,int>::const_iterator iterE=rangeE.first; iterE!=rangeE.second; ++iterE) {
    int tid = (*iterE).first;
    int tem = (*iterE).second;
    fEmbedded.insert( std::make_pair(tid,tem) );
  }
  std::cout << "TrackingPerformanceCheck ==> Embedded: " << fEmbedded.size() << std::endl;
  for(PHG4TruthInfoContainer::ConstIterator iter = range.first; 
      iter != range.second; 
      ++iter) {
    PHG4Particle* g4particle = iter->second;
    if(!g4particle) continue;
    nA++; // all
    float px = g4particle->get_px();
    float py = g4particle->get_py();
    float pz = g4particle->get_pz();
    float pt = TMath::Sqrt( px*px + py*py );
    float p = TMath::Sqrt( px*px + py*py + pz*pz );
    float phi = TMath::Pi()+TMath::ATan2(-py,-px);
    float eta = 1.e30;
    if(p != TMath::Abs(pz)) eta = 0.5*TMath::Log((p+pz)/(p-pz));
    fPt[0]->Fill(pt);
    fPhi[0]->Fill(phi);
    fEta[0]->Fill(eta);
    //if(trutheval->get_embed(g4particle)==0) continue;
    int id = g4particle->get_track_id();
    std::map<int,int>::iterator embeddedflag = fEmbedded.find(id);
    if(embeddedflag==fEmbedded.end()) continue; // not found
    if((*embeddedflag).second==0) continue; // not embedded
    //====> EMBEDDED
    nB++; // embedded
    // need a method to count how many layers where crossed by truth. Solution is to loop over g4hits and flag layers as hits belong to them.
    std::set<PHG4Hit*> g4hits = trutheval->all_truth_hits(g4particle);     
    bool hit[67];
    for(int i=0; i!=67; ++i) hit[i] = false;
    for(std::set<PHG4Hit*>::iterator iter = g4hits.begin();
	iter != g4hits.end();
	++iter) {
      PHG4Hit *candidate = *iter;
      unsigned int lyr = candidate->get_layer();
      hit[lyr] = true;
    }
    int nhits = 0;
    for(int i=7; i!=67; ++i) if(hit[i]) nhits++; // counts only tpc hits
    fPt[1]->Fill(pt);
    fPhi[1]->Fill(phi);
    fEta[1]->Fill(eta);
    fHEmbeddedNHits[0]->Fill( nhits );
    //====> RECONSTRUCTABLES
    if(nhits<40) continue; // threshold at 2/3 of nlayers
    (*embeddedflag).second = 2;
    nC++; // reconstructable
    fPt[2]->Fill(pt);
    fPhi[2]->Fill(phi);
    fEta[2]->Fill(eta);
    fHEmbeddedNHits[1]->Fill( nhits );
  }
  fHNTruths->Fill(nA);
  fHNEmbedded->Fill(nB);
  fHNReconstructables->Fill(nC);

  // loop over all reco particles
  nA = 0;
  nB = 0;
  nC = 0;
  int nD = 0;
  for(SvtxTrackMap::Iter iter = trackmap->begin();
      iter != trackmap->end(); ++iter) {
    nA++;
    SvtxTrack *track = iter->second;
    if (!track) continue;
    float rpx = track->get_px();
    float rpy = track->get_py();
    float rpz = track->get_pz();
    float rpt = TMath::Sqrt( rpx*rpx + rpy*rpy );
    float rp = TMath::Sqrt( rpx*rpx + rpy*rpy + rpz*rpz );
    float rphi = TMath::Pi()+TMath::ATan2(-rpy,-rpx);
    float reta = 1.e30;
    if(rp != TMath::Abs(rpz)) reta = 0.5*TMath::Log((rp+rpz)/(rp-rpz));
    float rdca2d = track->get_dca2d();
    fHRPt[0]->Fill(rpt);
    fHRPhi[0]->Fill(rphi);
    fHREta[0]->Fill(reta);
    fHRDca2D[0]->Fill(rpt,rdca2d);
    // check if matches to a MCTruth
    PHG4Particle* g4particle = trackeval->max_truth_particle_by_nclusters(track);
    if(!g4particle) continue;
    nB++;
    float px = g4particle->get_px();
    float py = g4particle->get_py();
    float pz = g4particle->get_pz();
    float pt = TMath::Sqrt( px*px + py*py );
    float p = TMath::Sqrt( px*px + py*py + pz*pz );
    float phi = TMath::Pi()+TMath::ATan2(-py,-px);
    float eta = 1.e30;
    if(p != TMath::Abs(pz)) eta = 0.5*TMath::Log((p+pz)/(p-pz));
    int nclusterscontrib = int( trackeval->get_nclusters_contribution(track,g4particle) );
    float ptreldiff = (rpt-pt)/pt;
    float phireldiff = (rphi-phi)/phi;
    float etareldiff = (reta-eta)/eta;
    fHNClustersContribution[0]->Fill( nclusterscontrib );
    fHPtResolution[0]->Fill(pt,ptreldiff);
    fHPhiResolution[0]->Fill(pt,phireldiff);
    fHEtaResolution[0]->Fill(pt,etareldiff);
    fHRPt[1]->Fill(rpt);
    fHRPhi[1]->Fill(rphi);
    fHREta[1]->Fill(reta);
    fHRDca2D[1]->Fill(rpt,rdca2d);
    // check if it was embedded
    int id = g4particle->get_track_id();
    std::map<int,int>::iterator embeddedflag = fEmbedded.find(id);
    if(embeddedflag==fEmbedded.end()) continue; // not found
    if((*embeddedflag).second==0) continue; // not embedded
    nC++;
    fHNClustersContribution[1]->Fill( nclusterscontrib );
    fHPtResolution[1]->Fill(pt,ptreldiff);
    fHPhiResolution[1]->Fill(pt,phireldiff);
    fHEtaResolution[1]->Fill(pt,etareldiff);
    fHRPt[2]->Fill(rpt);
    fHRPhi[2]->Fill(rphi);
    fHREta[2]->Fill(reta);
    fHRDca2D[2]->Fill(rpt,rdca2d);
    // check if it was reconstructable
    if((*embeddedflag).second==1) continue; // not reconstructable
    nD++;
    fHNClustersContribution[2]->Fill( nclusterscontrib );
    fHPtResolution[2]->Fill(pt,ptreldiff);
    fHPhiResolution[2]->Fill(pt,phireldiff);
    fHEtaResolution[2]->Fill(pt,etareldiff);
    fHRPt[3]->Fill(rpt);
    fHRPhi[3]->Fill(rphi);
    fHREta[3]->Fill(reta);
    fHRDca2D[3]->Fill(rpt,rdca2d);
  }
  fHNTracks->Fill( nA );
  fHNTracksMatched->Fill( nB );
  fHNTracksEmbedded->Fill( nC );
  fHNTracksReconstructable->Fill( nD );
 
  // get leading vertex
  SvtxVertex* maxvertex = NULL;
  unsigned int maxtracks = 0;  
  nA=0;
  for (SvtxVertexMap::Iter iter = vertexmap->begin();
       iter != vertexmap->end();
       ++iter) {
    nA++;
    SvtxVertex* vertex = iter->second;
    if (vertex->size_tracks() > maxtracks) {
      maxvertex = vertex;
      maxtracks = vertex->size_tracks();
    }
  }
  fHNVertexes->Fill(nA);
  
  /*
  if(maxvertex) {
    PHG4VtxPoint* point = vertexeval->max_truth_point_by_ntracks(maxvertex);
    if (point) {
      _dx_vertex->Fill(maxvertex->get_x() - point->get_x());
      _dy_vertex->Fill(maxvertex->get_y() - point->get_y());
      _dz_vertex->Fill(maxvertex->get_z() - point->get_z());
    }
  }
  */
  std::cout << "TrackingPerformanceCheck [EXIT]" << std::endl;
  return 0;
}

int TrackingPerformanceCheck::End(PHCompositeNode *topNode) {
 return 0;
}
