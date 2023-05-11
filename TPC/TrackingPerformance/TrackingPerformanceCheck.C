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
#include <g4hough/SvtxClusterMap.h>
#include <g4hough/SvtxCluster.h>

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

TrackingPerformanceCheck::TrackingPerformanceCheck(const string &name) :
  SubsysReco(name),
  fLayerTPCBegins(7),
  fReconstructable_TPCHits(30), // 40/60 or 30/40
  fFair_ClustersContribution(20), // 25/67 or 20/47
  fGTrack_Chi2NDF(2.0),
  fGTrack_TPCClusters(20) // 25/60 or 20/40
{
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

  TString sTname[5] = {"AllG4Particles","Primaries","Embedded","Reconstructables","RecoMatched"};
  for(int i=0; i!=5; ++i) {
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
    if(i<4) continue;
    fHTChi2[i] = new TH2F(Form("Truths%d_Chi2NDF",i),Form("%s Chi2NDF;PT;CHI2NDF",sTname[i].Data()),nptbins,ptbins,100,0,5);
    fHTDca2D[i] = new TH2F( Form("Truths%d_Dca2D",i),Form("%s Dca2D;PT;DCA2D",sTname[i].Data()),nptbins,ptbins,200,-0.003,+0.003);
    fHTNClustersContribution[i] = new TH2F(Form("Truths%d_NClusters",i), Form("%s Number of Clusters Matched",sTname[i].Data()), nptbins,ptbins,70,-0.5,69.5);
    fHTPtResolution[i] = new TH2F(Form("Truths%d_ResPt",i), Form("%s PtResolution;PT;REL DIFF",sTname[i].Data()), nptbins,ptbins,400,-1.1,+1.1);
    fHTPtResolution2[i] = new TH2F(Form("Truths%d_Res2Pt",i), Form("%s PtResolution2;PT;DIFF",sTname[i].Data()), nptbins,ptbins,400,-0.2,+0.2);
    fHTPhiResolution[i] = new TH2F(Form("Truths%d_ResPhi",i), Form("%s PhiResolution;PT;REL DIFF",sTname[i].Data()), nptbins,ptbins,200,-0.1,+0.1);
    fHTEtaResolution[i] = new TH2F(Form("Truths%d_ResEta",i), Form("%s EtaResolution;PT;REL DIFF",sTname[i].Data()), nptbins,ptbins,200,-0.1,+0.1);
    se->registerHisto(fHTChi2[i]);
    se->registerHisto(fHTDca2D[i]);
    se->registerHisto(fHTNClustersContribution[i]);
    se->registerHisto(fHTPtResolution[i]);
    se->registerHisto(fHTPtResolution2[i]);
    se->registerHisto(fHTPhiResolution[i]);
    se->registerHisto(fHTEtaResolution[i]);
  }

  TString sRname[4] = {"AllTracks","GoodTracks","GTracksM2R","GTracksExcess"};
  for(int i=0; i!=4; ++i) {
    fHRN[i] = new TH1F(Form("Tracks%d_N",i),Form("Number of %s",sRname[i].Data()),100,-0.5,1999.5);
    fHRPt[i] = new TH1F(Form("Tracks%d_Pt",i),Form("%s Pt",sRname[i].Data()),nptbins,ptbins);
    fHRPhi[i] = new TH1F(Form("Tracks%d_Phi",i),Form("%s Phi",sRname[i].Data()),100,0,TMath::TwoPi());
    fHREta[i] = new TH1F(Form("Tracks%d_Eta",i),Form("%s Eta",sRname[i].Data()),100,-1.5,+1.5);
    fHRTPCClus[i] = new TH2F(Form("Tracks%d_TPCClus",i),Form("%s TPCCLus;PT;TPCClusters",sRname[i].Data()),nptbins,ptbins,70,-0.5,69.5);
    fHRChi2[i] = new TH2F(Form("Tracks%d_Chi2NDF",i),Form("%s Chi2NDF;PT;CHI2NDF",sRname[i].Data()),nptbins,ptbins,100,0,5);
    fHRDca2D[i] = new TH2F( Form("Tracks%d_Dca2D",i),Form("%s Dca2D;PT;DCA2D",sRname[i].Data()),nptbins,ptbins,200,-0.003,+0.003);
    se->registerHisto(fHRN[i]);
    se->registerHisto(fHRPt[i]);
    se->registerHisto(fHRPhi[i]);
    se->registerHisto(fHREta[i]);
    se->registerHisto(fHRTPCClus[i]);
    se->registerHisto(fHRChi2[i]);
    se->registerHisto(fHRDca2D[i]);
    if(i<2) continue;
    fHRPtResolution[i] = new TH2F(Form("Tracks%d_ResPt",i), Form("%s PtResolution;PT;REL DIFF",sRname[i].Data()), nptbins,ptbins,400,-1.5,+1.5);
    fHRPtResolution2[i] = new TH2F(Form("Tracks%d_Res2Pt",i), Form("%s PtResolution2;PT;DIFF",sRname[i].Data()), nptbins,ptbins,400,-0.2,+0.2);
    fHRPhiResolution[i] = new TH2F(Form("Tracks%d_ResPhi",i), Form("%s PhiResolution;PT;REL DIFF",sRname[i].Data()), nptbins,ptbins,200,-0.1,+0.1);
    fHREtaResolution[i] = new TH2F(Form("Tracks%d_ResEta",i), Form("%s EtaResolution;PT;REL DIFF",sRname[i].Data()), nptbins,ptbins,200,-0.1,+0.1);
    fHRNClustersContribution[i] = new TH2F(Form("Tracks%d_NClustersContri",i), Form("%s Number of Clusters Contribution",sRname[i].Data()), nptbins,ptbins,70,-0.5,69.5);
    se->registerHisto(fHRPtResolution[i]);
    se->registerHisto(fHRPtResolution2[i]);
    se->registerHisto(fHRPhiResolution[i]);
    se->registerHisto(fHREtaResolution[i]);
    se->registerHisto(fHRNClustersContribution[i]);
  }

  fHNVertexes = new TH1F("Vertexes_N","Number of Vertexes",10,-0.5,9.5);
  se->registerHisto(fHNVertexes);
  return 0;
}

int TrackingPerformanceCheck::process_event(PHCompositeNode *topNode) {
  //std::cout << "TrackingPerformanceCheck [ENTER]" << std::endl;
  fHNEvents->Fill(0);
  fEmbedded.clear();
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
  SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
  SvtxClusterMap *clustermap = findNode::getClass<SvtxClusterMap>(topNode,"SvtxClusterMap");
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
    if (tem > 0) //embeded signal
    fEmbedded.insert( std::make_pair(tid,tem) );
  }
  //std::cout << "TrackingPerformanceCheck ==> Embedded: " << fEmbedded.size() << std::endl;

  SvtxEvalStack svtxevalstack(topNode);
  SvtxTruthEval *trutheval = svtxevalstack.get_truth_eval();  
  SvtxTrackEval *trackeval = svtxevalstack.get_track_eval();
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
    //need a method to count how many layers where crossed by truth. Solution is to loop over g4hits and flag layers as hits belong to them.
    std::set<PHG4Hit*> g4hits = trutheval->all_truth_hits(g4particle);     
    bool hit[70];
    for(int i=0; i!=70; ++i) hit[i] = false;
    for(std::set<PHG4Hit*>::iterator iter = g4hits.begin();
	iter != g4hits.end();
	++iter) {
      PHG4Hit *candidate = *iter;
      unsigned int lyr = candidate->get_layer();
      hit[lyr] = true;
    }
    int nhits = 0;
    for(int i=fLayerTPCBegins; i!=70; ++i) if(hit[i]) nhits++; // counts only tpc hits
    fHTPt[2]->Fill(pt);
    fHTPhi[2]->Fill(phi);
    fHTEta[2]->Fill(eta);
    fHTNHits[2]->Fill( nhits );
    //====> RECONSTRUCTABLES
    if(nhits<fReconstructable_TPCHits) continue;
    (*embeddedflag).second = 2;
    nD++; // reconstructable
    fHTPt[3]->Fill(pt);
    fHTPhi[3]->Fill(phi);
    fHTEta[3]->Fill(eta);
    fHTNHits[3]->Fill( nhits );
    SvtxTrack *best = trackeval->best_track_from(g4particle); // get the best track (max nclus) for this truth
    if(!best) continue;
    //====> MATCHED TO TRACK
    float rpx = best->get_px();
    float rpy = best->get_py();
    float rpz = best->get_pz();
    float rpt = TMath::Sqrt( rpx*rpx + rpy*rpy );
    float rp = TMath::Sqrt( rpx*rpx + rpy*rpy + rpz*rpz );
    float rphi = TMath::Pi()+TMath::ATan2(-rpy,-rpx);
    float reta = 1.e30;
    if(rp != TMath::Abs(rpz)) reta = 0.5*TMath::Log((rp+rpz)/(rp-rpz));
    int nclusterscontrib = int( trackeval->get_nclusters_contribution(best,g4particle) ); // computes nclus
    if(nclusterscontrib<fFair_ClustersContribution) continue;
    float rdca2d = best->get_dca2d();
    int ndf = int(best->get_ndf());
    float chi2ndf = -1;
    if(ndf!=0) chi2ndf = best->get_chisq() / ndf;
    float ptreldiff = (rpt-pt)/pt;
    float ptdiff = rpt-pt;
    float phireldiff = (rphi-phi)/phi;
    float etareldiff = (reta-eta)/eta;
    fHTPt[4]->Fill(pt);
    fHTPhi[4]->Fill(phi);
    fHTEta[4]->Fill(eta);
    fHTNHits[4]->Fill( nhits );
    fHTChi2[4]->Fill(chi2ndf,ptreldiff);
    fHTDca2D[4]->Fill(pt,rdca2d);
    fHTPtResolution[4]->Fill(pt,ptreldiff);
    fHTPtResolution2[4]->Fill(pt,ptdiff);
    fHTPhiResolution[4]->Fill(pt,phireldiff);
    fHTEtaResolution[4]->Fill(pt,etareldiff);
    fHTNClustersContribution[4]->Fill(pt, nclusterscontrib );
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
  //std::cout << "TRACKS!" << std::endl;
  for(SvtxTrackMap::Iter iter = trackmap->begin();
      iter != trackmap->end(); ++iter) {
    //===> ALL TRACKS
    nA++;
    SvtxTrack *track = iter->second;
    if (!track) continue;
    // counting tpcclusters
    int tpcclus=0;
    for (SvtxTrack::ConstClusterIter iter = track->begin_clusters();
	 iter != track->end_clusters(); ++iter) {
      unsigned int cluster_id = *iter;
      SvtxCluster* cluster = clustermap->get(cluster_id);
      if(!cluster) continue;
      int lyr = cluster->get_layer();
      if(lyr>=fLayerTPCBegins) tpcclus++;
    }
    float rpx = track->get_px();
    float rpy = track->get_py();
    float rpz = track->get_pz();
    float rpt = TMath::Sqrt( rpx*rpx + rpy*rpy );
    float rp = TMath::Sqrt( rpx*rpx + rpy*rpy + rpz*rpz );
    float rphi = TMath::Pi()+TMath::ATan2(-rpy,-rpx);
    float reta = 1.e30;
    if(rp != TMath::Abs(rpz)) reta = 0.5*TMath::Log((rp+rpz)/(rp-rpz));
    float rdca2d = track->get_dca2d();
    int ndf = int(track->get_ndf());
    float chi2ndf = -1;
    if(ndf!=0) chi2ndf = track->get_chisq() / ndf;
    fHRPt[0]->Fill(rpt);
    fHRPhi[0]->Fill(rphi);
    fHREta[0]->Fill(reta);
    fHRTPCClus[0]->Fill(rpt,tpcclus);
    fHRChi2[0]->Fill(rpt,chi2ndf);
    fHRDca2D[0]->Fill(rpt,rdca2d);
    if(chi2ndf>fGTrack_Chi2NDF) continue;
    if(tpcclus<fGTrack_TPCClusters) continue;
    //===> SELECTED
    nB++;
    fHRPt[1]->Fill(rpt);
    fHRPhi[1]->Fill(rphi);
    fHREta[1]->Fill(reta);
    fHRTPCClus[1]->Fill(rpt,tpcclus);
    fHRChi2[1]->Fill(rpt,chi2ndf);
    fHRDca2D[1]->Fill(rpt,rdca2d);
    // check if matches to a MCTruth
    PHG4Particle* g4particle = trackeval->max_truth_particle_by_nclusters(track); // get the most probable (max nclus) truth
    if(!g4particle) continue;
    //SvtxTrack *best = trackeval->best_track_from(g4particle); // get the best track (max nclus) for this truth
    //if(!best) continue;
    //std::cout << track->get_id() << " ==> " << g4particle->get_track_id() << " ==> " << best->get_id() << std::endl;
    //if(best!=track) continue; // skippping low quality duplicates
    // check if it was embedded
    int id = g4particle->get_track_id();
    std::map<int,int>::iterator embeddedflag = fEmbedded.find(id);
    if(embeddedflag==fEmbedded.end()) continue; // not found
    //std::cout << " " << track->get_id() << " ==> " << g4particle->get_track_id() << "(" << (*embeddedflag).second << ")" << " ==> " << best->get_id() << std::endl;
    if((*embeddedflag).second==0) continue; // not embedded
    // check if it was reconstructable
    if((*embeddedflag).second==1) continue; // not reconstructable
    if((*embeddedflag).second>1) (*embeddedflag).second++; // increase counter
    //std::cout << " " << track->get_id() << " ==> " << g4particle->get_track_id() << "(" << (*embeddedflag).second << ")" << " ==> " << best->get_id() << std::endl;
    //===> MATCHED TO EMBEDDED RECONSTRUCTABLE
    nC++;
    float px = g4particle->get_px();
    float py = g4particle->get_py();
    float pz = g4particle->get_pz();
    float pt = TMath::Sqrt( px*px + py*py );
    float p = TMath::Sqrt( px*px + py*py + pz*pz );
    float phi = TMath::Pi()+TMath::ATan2(-py,-px);
    float eta = 1.e30;
    if(p != TMath::Abs(pz)) eta = 0.5*TMath::Log((p+pz)/(p-pz));
    int nclusterscontrib = int( trackeval->get_nclusters_contribution(track,g4particle) ); // computes nclus
    float ptreldiff = (rpt-pt)/pt;
    float ptdiff = rpt-pt;
    float phireldiff = (rphi-phi)/phi;
    float etareldiff = (reta-eta)/eta;
    fHRPt[2]->Fill(rpt);
    fHRPhi[2]->Fill(rphi);
    fHREta[2]->Fill(reta);
    fHRTPCClus[2]->Fill(rpt,tpcclus);
    fHRChi2[2]->Fill(rpt,chi2ndf);
    fHRDca2D[2]->Fill(rpt,rdca2d);
    fHRPtResolution[2]->Fill(pt,ptreldiff);
    fHRPtResolution2[2]->Fill(pt,ptdiff);
    fHRPhiResolution[2]->Fill(pt,phireldiff);
    fHREtaResolution[2]->Fill(pt,etareldiff);
    fHRNClustersContribution[2]->Fill(pt, nclusterscontrib);
    if((*embeddedflag).second<4) continue; // 2->3 is okay, higher is excess
    //===> FAKES (counters are the only ones meaningful)
    nD++;
    fHRPt[3]->Fill(rpt);
    fHRPhi[3]->Fill(rphi);
    fHREta[3]->Fill(reta);
    fHRTPCClus[2]->Fill(rpt,tpcclus);
    fHRChi2[3]->Fill(rpt,chi2ndf);
    fHRDca2D[3]->Fill(rpt,rdca2d);
    fHRPt[3]->Fill(rpt);
    fHRPhi[3]->Fill(rphi);
    fHREta[3]->Fill(reta);
    fHRChi2[3]->Fill(rpt,chi2ndf);
    fHRDca2D[3]->Fill(rpt,rdca2d);
    fHRPtResolution[3]->Fill(pt,ptreldiff);
    fHRPtResolution2[3]->Fill(pt,ptdiff);
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
  //std::cout << "TrackingPerformanceCheck [EXIT]" << std::endl;
  return 0;
}

int TrackingPerformanceCheck::End(PHCompositeNode *topNode) {
 return 0;
}
