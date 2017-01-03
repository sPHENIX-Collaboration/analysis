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
#include "TString.h"

#include <iostream>

using namespace std;

TrackingPerformanceCheck::TrackingPerformanceCheck(const string &name)
  : SubsysReco(name) {
}

int TrackingPerformanceCheck::Init(PHCompositeNode *topNode) {
  // register histograms
  Fun4AllServer *se = Fun4AllServer::instance();

  const int nptbinsPONE = 52;
  Int_t nptbins = nptbinsPONE - 1;
  Double_t ptbins[nptbinsPONE];
  ptbins[0] = 0.2;
  for(int i=1; i!=9; ++i)
    ptbins[i] = 0.2 + i*0.2;
  for(int i=0; i!=11; ++i)
    ptbins[9+i] = 2.0 + i*0.5;
  for(int i=0; i!=10; ++i)
    ptbins[20+i] = 8.0 + i*1.0;
  for(int i=0; i!=12; ++i)
    ptbins[30+i] = 18.0 + i*2.0;
  for(int i=0; i!=10; ++i)
    ptbins[42+i] = 45.0 + i*5.0;

  for(int i=0; i!=nptbinsPONE; ++i) std::cout << i << " " << ptbins[i] << std::endl;

  fHNEvents = new TH1F("Events","Events",1,-0.5,0.5);
  se->registerHisto(fHNEvents);

  TString sTname[4] = {"AllG4Particles","Primaries","Embedded","Reconstructables"};
  for(int i=0; i!=4; ++i) {
    fHTN[i] = new TH1F(Form("Truths%d_N",i),Form("Number of %s",sTname[i].Data()),50,-0.5,49.5);
    fHTPt[i] = new TH1F(Form("Truths%d_Pt",i),Form("%s Pt",sTname[i].Data()),nptbins,ptbins);
    fHTPhi[i] = new TH1F(Form("Truths%d_Phi",i),Form("%s Phi",sTname[i].Data()),100,0,TMath::TwoPi());
    fHTEta[i] = new TH1F(Form("Truths%d_Eta",i),Form("%s Eta",sTname[i].Data()),100,-10,+10);
    se->registerHisto(fHTN[i]);
    se->registerHisto(fHTPt[i]);
    se->registerHisto(fHTPhi[i]);
    se->registerHisto(fHTEta[i]);
    if(i<2) continue;
    fHTNHits[i] = new TH1F(Form("Truths%d_Hits",i),Form("Number of TPCHits in %s",sTname[i].Data()),61,-0.5,60.5);
    se->registerHisto(fHTNHits[i]);
  }

  TString sRname[4] = {"AllTracks","M2G4Particle","M2Embedded","M2Reconstructable"};
  for(int i=0; i!=4; ++i) {
    fHRN[i] = new TH1F(Form("Tracks%d_N",i),Form("Number of %s",sRname[i].Data()),100,-0.5,1999.5);
    fHRPt[i] = new TH1F(Form("Tracks%d_Pt",i),Form("%s Pt",sRname[i].Data()),nptbins,ptbins);
    fHRPhi[i] = new TH1F(Form("Tracks%d_Phi",i),Form("%s Phi",sRname[i].Data()),100,0,TMath::TwoPi());
    fHREta[i] = new TH1F(Form("Tracks%d_Eta",i),Form("%s Eta",sRname[i].Data()),100,-1.5,+1.5);
    fHRDca2D[i] = new TH2F( Form("Tracks%d_Dca2D",i),Form("%s Dca2D;PT;DCA2D",sRname[i].Data()),nptbins,ptbins,200,-1.0,1.0);
    se->registerHisto(fHRN[i]);
    se->registerHisto(fHRPt[i]);
    se->registerHisto(fHRPhi[i]);
    se->registerHisto(fHREta[i]);
    se->registerHisto(fHRDca2D[i]);
    if(i<1) continue;
    fHRNClustersContribution[i] = new TH2F(Form("Tracks%d_NClusters",i), Form("%s Number of Clusters Matched",sRname[i].Data()), nptbins,ptbins,70,-0.5,69.5);
    se->registerHisto(fHRNClustersContribution[i]);
    if(i<3) continue;
    fHRPtResolution[i] = new TH2F(Form("Tracks%d_ResPt",i), Form("%s PtResolution;PT;REL DIFF",sRname[i].Data()), nptbins,ptbins,200,-1.5,+1.5);
    fHRPhiResolution[i] = new TH2F(Form("Tracks%d_ResPhi",i), Form("%s PhiResolution;PT;REL DIFF",sRname[i].Data()), nptbins,ptbins,200,-0.1,+0.1);
    fHREtaResolution[i] = new TH2F(Form("Tracks%d_ResEta",i), Form("%s EtaResolution;PT;REL DIFF",sRname[i].Data()), nptbins,ptbins,200,-0.1,+0.1);
    se->registerHisto(fHRPtResolution[i]);
    se->registerHisto(fHRPhiResolution[i]);
    se->registerHisto(fHREtaResolution[i]);
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

  // We need a method to distinguish between embedded and reconstructables. The current flag for embeded is an int with zero and one, so naturally this can be extended to hold values higher than two. However the current list cannot be modified, because the only accessor returns const_iterator, so we either change that in the base class (PH4TruthInfoContainer) or we clone the list (what we currently do here). Since this list only concerns embedded particles, it is going to be small
  std::pair< std::map<int,int>::const_iterator, std::map<int,int>::const_iterator > rangeE = truthinfo->GetEmbeddedTrkIds();
  for(std::map<int,int>::const_iterator iterE=rangeE.first; iterE!=rangeE.second; ++iterE) {
    int tid = (*iterE).first;
    int tem = (*iterE).second;
    fEmbedded.insert( std::make_pair(tid,tem) );
  }
  std::cout << "TrackingPerformanceCheck ==> Embedded: " << fEmbedded.size() << std::endl;

  SvtxEvalStack svtxevalstack(topNode);
  SvtxTruthEval *trutheval = svtxevalstack.get_truth_eval();  
  //PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
  PHG4TruthInfoContainer::Range range = truthinfo->GetParticleRange();
  int nA=0;
  int nB=0;
  int nC=0;
  int nD=0;
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
    fHTPt[0]->Fill(pt);
    fHTPhi[0]->Fill(phi);
    fHTEta[0]->Fill(eta);
    if( iter->first < 0 ) continue;
    nB++; // primaries
    fHTPt[1]->Fill(pt);
    fHTPhi[1]->Fill(phi);
    fHTEta[1]->Fill(eta);
    int id = g4particle->get_track_id();
    std::map<int,int>::iterator embeddedflag = fEmbedded.find(id);
    if(embeddedflag==fEmbedded.end()) continue; // not found
    if((*embeddedflag).second==0) continue; // not embedded
    //====> EMBEDDED
    nC++; // embedded
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
    fHTPt[2]->Fill(pt);
    fHTPhi[2]->Fill(phi);
    fHTEta[2]->Fill(eta);
    fHTNHits[2]->Fill( nhits );
    //====> RECONSTRUCTABLES
    if(nhits<40) continue; // threshold at 2/3 of nlayers
    (*embeddedflag).second = 2;
    nD++; // reconstructable
    fHTPt[3]->Fill(pt);
    fHTPhi[3]->Fill(phi);
    fHTEta[3]->Fill(eta);
    fHTNHits[3]->Fill( nhits );
  }
  fHTN[0]->Fill(nA);
  fHTN[1]->Fill(nB);
  fHTN[2]->Fill(nC);
  fHTN[3]->Fill(nD);

  // loop over all reco particles
  nA = 0;
  nB = 0;
  nC = 0;
  nD = 0;
  SvtxTrackEval *trackeval = svtxevalstack.get_track_eval();
  for(SvtxTrackMap::Iter iter = trackmap->begin();
      iter != trackmap->end(); ++iter) {
    //ALL TRACKS
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
    PHG4Particle* g4particle = trackeval->max_truth_particle_by_nclusters(track); // get the most probable (max nclus) truth
    if(!g4particle) continue;
    SvtxTrack *best = trackeval->best_track_from(g4particle); // get the best track (max nclus) for this truth
    if(!best) continue;
    if(best!=track) continue; // skippping low quality duplicates
    //MATCHED
    nB++;
    float px = g4particle->get_px();
    float py = g4particle->get_py();
    float pz = g4particle->get_pz();
    float pt = TMath::Sqrt( px*px + py*py );
    float p = TMath::Sqrt( px*px + py*py + pz*pz );
    float phi = TMath::Pi()+TMath::ATan2(-py,-px);
    float eta = 1.e30;
    if(p != TMath::Abs(pz)) eta = 0.5*TMath::Log((p+pz)/(p-pz));
    int nclusterscontrib = int( trackeval->get_nclusters_contribution(track,g4particle) ); // computes nclus
    fHRPt[1]->Fill(rpt);
    fHRPhi[1]->Fill(rphi);
    fHREta[1]->Fill(reta);
    fHRDca2D[1]->Fill(rpt,rdca2d);
    fHRNClustersContribution[1]->Fill(pt, nclusterscontrib);
    // check if it was embedded
    int id = g4particle->get_track_id();
    std::map<int,int>::iterator embeddedflag = fEmbedded.find(id);
    if(embeddedflag==fEmbedded.end()) continue; // not found
    if((*embeddedflag).second==0) continue; // not embedded
    //EMBEDDED
    nC++;
    fHRPt[2]->Fill(rpt);
    fHRPhi[2]->Fill(rphi);
    fHREta[2]->Fill(reta);
    fHRDca2D[2]->Fill(rpt,rdca2d);
    fHRNClustersContribution[2]->Fill(pt, nclusterscontrib );
    // check if it was reconstructable
    if((*embeddedflag).second==1) continue; // not reconstructable
    //RECONSTRUCTABLE
    nD++;
    float ptreldiff = (rpt-pt)/pt;
    float phireldiff = (rphi-phi)/phi;
    float etareldiff = (reta-eta)/eta;
    fHRPt[3]->Fill(rpt);
    fHRPhi[3]->Fill(rphi);
    fHREta[3]->Fill(reta);
    fHRDca2D[3]->Fill(rpt,rdca2d);
    fHRPtResolution[3]->Fill(pt,ptreldiff);
    fHRPhiResolution[3]->Fill(pt,phireldiff);
    fHREtaResolution[3]->Fill(pt,etareldiff);
    fHRNClustersContribution[3]->Fill(pt, nclusterscontrib );

  }
  fHRN[0]->Fill( nA );
  fHRN[1]->Fill( nB );
  fHRN[2]->Fill( nC );
  fHRN[3]->Fill( nD );

  // get leading vertex
  //SvtxVertexEval *vertexeval = svtxevalstack.get_vertex_eval();
  //SvtxVertex* maxvertex = NULL;
  //unsigned int maxtracks = 0;  
  nA=0;
  for (SvtxVertexMap::Iter iter = vertexmap->begin();
       iter != vertexmap->end();
       ++iter) {
    nA++;
    //SvtxVertex* vertex = iter->second;
    //if (vertex->size_tracks() > maxtracks) {
    //  maxvertex = vertex;
    //  maxtracks = vertex->size_tracks();
    //}
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
