#include "RecoConversionEval.h"
#include "SVReco.h"
#include "VtxRegressor.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phgenfit/Track.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertexMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxHitMap.h>
#include <trackbase_historic/SvtxHit.h>
#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxCluster.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrClusterv1.h>
#include <g4eval/SvtxEvalStack.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>


#include <TTree.h>
#include <TFile.h>
#include <TLorentzVector.h>

#include <iostream>
#include <cmath> 
#include <algorithm>
#include <sstream>

using namespace std;

RecoConversionEval::RecoConversionEval(const std::string &name,std::string tmvamethod,std::string tmvapath) :
  SubsysReco("RecoConversionEval"), _fname(name) 
{
  _regressor = new VtxRegressor(tmvamethod,tmvapath);
}

RecoConversionEval::~RecoConversionEval(){
  cout<<"deleting RCE"<<endl;
  if(_vertexer) delete _vertexer;
  if(_regressor) delete _regressor;
}

int RecoConversionEval::Init(PHCompositeNode *topNode) {
  return Fun4AllReturnCodes::EVENT_OK;
}

int RecoConversionEval::InitRun(PHCompositeNode *topNode) {
  _vertexer = new SVReco();
  //TODO turn this back into a subsystem and put it on the node tree
  _vertexer->InitRun(topNode);
  _file = new TFile( _fname.c_str(), "RECREATE");
  _treeSignal = new TTree("recoSignal","strong saharah bush");
  _treeSignal->SetAutoSave(300);
  _treeSignal->Branch("photon_m",   &_b_photon_m);
  _treeSignal->Branch("photon_pT",  &_b_photon_pT);
  _treeSignal->Branch("photon_eta", &_b_photon_eta);
  _treeSignal->Branch("photon_phi", &_b_photon_phi);
  _treeSignal->Branch("track1_pT", &_b_track1_pT);
  _treeSignal->Branch("track2_pT", &_b_track2_pT);
  _treeSignal->Branch("refit", &_b_refit);

  _treeBackground = new TTree("recoBackground","friendly fern");
  _treeBackground->SetAutoSave(300);
  _treeBackground->Branch("total",   &totalTracks);
  _treeBackground->Branch("tracksPEQ",  &passedpTEtaQ);
  _treeBackground->Branch("tracks_clus", &passedCluster);
  _treeBackground->Branch("pairs", &passedPair);
  _treeBackground->Branch("vtx", 	  &passedVtx);
  _treeBackground->Branch("photon", 	  &passedPhoton);

  return Fun4AllReturnCodes::EVENT_OK;
}

void RecoConversionEval::doNodePointers(PHCompositeNode *topNode){
  _allTracks = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
  _mainClusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  /*These are deprecated
   * _svtxClusterMap = findNode::getClass<SvtxClusterMap>(topNode,"SvtxClusterMap");
   _hitMap = findNode::getClass<SvtxHitMap>(topNode,"SvtxHitMap");*/
  //new version
  _clusterMap = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
  _vertexer->InitEvent(topNode);
  //to check if the id is correct
  _truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");

}

bool RecoConversionEval::hasNodePointers()const{
  return _allTracks &&_mainClusterContainer && _vertexer;
}

int RecoConversionEval::process_event(PHCompositeNode *topNode) {
  doNodePointers(topNode);
  /*the is not optimized but is just a nlogn process*/
  for ( SvtxTrackMap::Iter iter = _allTracks->begin(); iter != _allTracks->end(); ++iter) {
    //I want to now only check e tracks so check the clusters of the |charge|=1 tracks
    totalTracks++;
    if (abs(iter->second->get_charge())==1&&iter->second->get_pt()>_kTrackPtCut&&abs(iter->second->get_eta())<1.) //should i have the layer cut?
    {
      SvtxTrack* thisTrack = iter->second;
      passedpTEtaQ++;
      RawCluster* bestCluster= _mainClusterContainer->getCluster(thisTrack->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
      //TODO what if no cluster is found?
      if(bestCluster&&bestCluster->get_prob()>=_kEMProbCut){
        //loop over the following tracks
        passedCluster++;
        for (SvtxTrackMap::Iter jter = iter; jter != _allTracks->end(); ++jter)
        {
          //check that the next track is an opposite charge electron
          if (thisTrack->get_charge()*-1==jter->second->get_charge()&&jter->second->get_pt()>_kTrackPtCut&&abs(iter->second->get_eta())<1.)
          {
            RawCluster* nextCluster= _mainClusterContainer->getCluster(jter->second->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
            //what if no cluster is found?
            if(nextCluster&&nextCluster->get_prob()>=_kEMProbCut&&pairCuts(thisTrack,jter->second)){
              passedPair++;
              genfit::GFRaveVertex* vtxCan = _vertexer->findSecondaryVertex(thisTrack,jter->second);
              vtxCan=correctSecondaryVertex(vtxCan,thisTrack,jter->second);
              if (vtxCan&&vtxCuts(vtxCan))
              {
                passedVtx++;
                _b_refit=true;
                std::pair<PHGenFit::Track*,PHGenFit::Track*> refit_tracks = refitTracks(vtxCan,thisTrack,jter->second);
                //attempt to set the photon to the addition of the refit tracks may return NULL
                TLorentzVector* photon= reconstructPhoton(refit_tracks);
                //if photon is null attempt to set it to the addition of the original tracks may return NULL
                if (!photon)
                {
                  photon = reconstructPhoton(thisTrack,jter->second);
                  _b_refit=false;
                }
                //if the photon is reconstructed record its data 
                if(photonCuts(photon)){
                  _b_photon_m = photon->Dot(*photon);
                  _b_photon_pT = photon->Pt();
                  _b_photon_eta = photon->Eta();
                  _b_photon_phi = photon->Phi();
                  _b_track1_pT = thisTrack->get_pt();
                  _b_track2_pT = jter->second->get_pt();
                  passedPhoton++;
                  delete photon;
                }
                else{
                  _b_photon_m =   -999.;
                  _b_photon_pT =  -999.;
                  _b_photon_eta = -999.;
                  _b_photon_phi = -999.;
                  cout<<"photon not reconstructed"<<endl;
                }
                /*FIXME currently this is not a valid way to get the truthparticle because get_truth_track_id returns UINT_MAX
                  PHG4Particle* truthparticle = _truthinfo->GetParticle(jter->second->get_truth_track_id());
                  PHG4Particle* truthparticle2 = _truthinfo->GetParticle(thisTrack->get_truth_track_id());
                  PHG4Particle* parent;
                  if(truthparticle) {
                  parent = _truthinfo->GetParticle(truthparticle->get_parent_id());
                  if(TMath::Abs(truthparticle->get_pid())!=11||(parent&&parent->get_pid()!=22)||truthparticle->get_parent_id()!=truthparticle2->get_parent_id()){
                  _b_fake=true;
                  }
                  else if(parent&&parent->get_pid()==22){
                  TLorentzVector ptlv;
                  ptlv.SetPxPyPzE(parent->get_px(),parent->get_py(),parent->get_pz(),parent->get_e());
                  parent->identify();
                  PHG4Particle* grandparent = _truthinfo->GetParticle(parent->get_parent_id());
                  if(grandparent) grandparent->identify();
                  _b_tphoton_phi = ptlv.Phi();
                  _b_tphoton_eta = ptlv.Eta();
                  _b_tphoton_pT =  ptlv.Pt();
                  }
                  else{
                  _b_tphoton_phi = -999.;
                  _b_tphoton_eta = -999.;
                  _b_tphoton_pT =  -999.;
                  }
                  }//found truth particle
                  else{
                  cout<<"no truth particle"<<endl;
                  _b_tphoton_phi = -999.;
                  _b_tphoton_eta = -999.;
                  _b_tphoton_pT =  -999.;
                  }*/
                _treeSignal->Fill();
              }//vtx cuts
            }
          }
        }
      }
    }
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

genfit::GFRaveVertex* RecoConversionEval::correctSecondaryVertex(genfit::GFRaveVertex* recoVertex,SvtxTrack* reco1,SvtxTrack* reco2){
  if(!(recoVertex&&reco1&&reco2)) {
    return recoVertex;
  }
  TVector3 nextPos = recoVertex->getPos();
  nextPos.SetMagThetaPhi(_regressor->regress(reco1,reco2,recoVertex),nextPos.Theta(),nextPos.Phi());

  using namespace genfit;
  // GFRaveVertex* temp = recoVertex;
  std::vector<GFRaveTrackParameters*> tracks;
  for(unsigned i =0; i<recoVertex->getNTracks();i++){
    tracks.push_back(recoVertex->getParameters(i));
  }
  recoVertex = new GFRaveVertex(nextPos,recoVertex->getCov(),tracks,recoVertex->getNdf(),recoVertex->getChi2(),recoVertex->getId());
  //  delete temp; //this caused outside references to seg fault //TODO shared_ptr is better 
  return recoVertex;
}

TLorentzVector* RecoConversionEval::reconstructPhoton(std::pair<PHGenFit::Track*,PHGenFit::Track*> recos){
  if (recos.first&&recos.second)
  {
    cout<<"reconstructing photon from refit tracks"<<endl;
    TLorentzVector tlv1;
    tlv1.SetVectM(recos.first->get_mom(),_kElectronRestM);
    TLorentzVector tlv2;
    tlv2.SetVectM(recos.second->get_mom(),_kElectronRestM);
    return new TLorentzVector(tlv1+tlv2);
  }
  else return NULL;
}

TLorentzVector* RecoConversionEval::reconstructPhoton(SvtxTrack* reco1,SvtxTrack* reco2){
  if (reco1&&reco2)
  {
    cout<<"reconstructing photon from svtx tracks"<<endl;
    TLorentzVector tlv1(reco1->get_px(),reco1->get_py(),reco1->get_pz(),
        sqrt(_kElectronRestM*_kElectronRestM+reco1->get_p()*reco1->get_p()));
    TLorentzVector tlv2(reco2->get_px(),reco2->get_py(),reco2->get_pz(),
        sqrt(_kElectronRestM*_kElectronRestM+reco2->get_p()*reco2->get_p()));
    return new TLorentzVector(tlv1+tlv2);
  }
  else return NULL;
}

std::pair<PHGenFit::Track*,PHGenFit::Track*> RecoConversionEval::refitTracks(genfit::GFRaveVertex* vtx,SvtxTrack* reco1,SvtxTrack* reco2){
  std::pair<PHGenFit::Track*,PHGenFit::Track*> r;
  if(!vtx)
  {
    cerr<<"WARNING: No vertex to refit tracks"<<endl;
    r.first=NULL;
    r.second=NULL;

  }
  else{
    r.first=_vertexer->refitTrack(vtx,reco1);
    r.second=_vertexer->refitTrack(vtx,reco2);
  }
  return r;
}

bool RecoConversionEval::pairCuts(SvtxTrack* t1, SvtxTrack* t2)const{
  return detaCut(t1->get_eta(),t2->get_eta())&&hitCuts(t1,t2);//TODO add approach distance ?
}

bool RecoConversionEval::photonCuts(TLorentzVector* photon)const{
  return photon&&photon->Dot(*photon)>_kPhotonMmin&&photon->Dot(*photon)<_kPhotonMmax&&_kPhotonPTmin<photon->Pt();
}


bool RecoConversionEval::hitCuts(SvtxTrack* reco1, SvtxTrack* reco2)const {
  TrkrCluster *c1 = _clusterMap->findCluster(*(reco1->begin_cluster_keys()));
  TrkrCluster *c2 = _clusterMap->findCluster(*(reco2->begin_cluster_keys()));
  unsigned l1 = TrkrDefs::getLayer(c1->getClusKey());
  unsigned l2 = TrkrDefs::getLayer(c2->getClusKey());
  //check that the first hits are close enough
  return abs(l1-l2)<=_kDLayerCut;
}

/*bool RecoConversionEval::vtxCuts(genfit::GFRaveVertex* vtxCan, SvtxTrack* t1, SvtxTrack *t2){
//TODO program the cuts invariant mass, pT
return vtxRadiusCut(vtxCan->getPos());
// && vtxTrackRPhiCut(vtxCan->getPos(),t1)&&vtxTrackRPhiCut(vtxCan->getPos(),t2)&& 
//vtxTrackRZCut(vtxCan->getPos(),t1)&&vtxTrackRZCut(vtxCan->getPos(),t2)&&vtxCan->getChi2()>_kVtxChi2Cut;
}*/

bool RecoConversionEval::vtxCuts(genfit::GFRaveVertex* vtxCan){
  return vtxRadiusCut(vtxCan->getPos());
}

bool RecoConversionEval::vtxTrackRZCut(TVector3 recoVertPos, SvtxTrack* track){
  float dR = sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y())-sqrt(track->get_x()*track->get_x()+track->get_y()*track->get_y());
  float dZ = recoVertPos.z()-track->get_z();
  return sqrt(dR*dR+dZ*dZ)<_kVtxRZCut;
}

//bool RecoConversionEval::invariantMassCut()

bool RecoConversionEval::vtxTrackRPhiCut(TVector3 recoVertPos, SvtxTrack* track){
  float vtxR=sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y());
  float trackR=sqrt(track->get_x()*track->get_x()+track->get_y()*track->get_y());
  return sqrt(vtxR*vtxR+trackR*trackR-2*vtxR*trackR*cos(recoVertPos.Phi()-track->get_phi()))<_kVtxRPhiCut;
}

bool RecoConversionEval::vtxRadiusCut(TVector3 recoVertPos){
  return sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y()) > _kVtxRCut;
}

int RecoConversionEval::End(PHCompositeNode *topNode) {
  cout<<"Did RecoConversionEval with "<<totalTracks<<" total tracks\n\t";
  cout<<1-(float)passedpTEtaQ/totalTracks<<"+/-"<<sqrt((float)passedpTEtaQ)/totalTracks<<" of tracks were rejected by pTEtaQ\n\t";
  cout<<1-(float)passedCluster/passedpTEtaQ<<"+/-"<<sqrt((float)passedCluster)/passedpTEtaQ<<" of remaining tracks were rejected by cluster\n\t";
  cout<<1-(float)passedPair/passedCluster<<"+/-"<<sqrt((float)passedPair)/passedCluster<<" of pairs were rejected by pair cuts\n\t";
  cout<<1-(float)passedVtx/passedPair<<"+/-"<<sqrt((float)passedVtx)/passedPair<<" of vtx were rejected by vtx cuts\n\t";
  _treeBackground->Fill();
  if(_file){
    _file->Write();
    _file->Close();
  }
  cout<<"good end"<<endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

bool RecoConversionEval::approachDistance(SvtxTrack *t1,SvtxTrack* t2)const{
  static const double eps = 0.000001;
  TVector3 u(t1->get_px(),t1->get_py(),t1->get_pz());
  TVector3 v(t2->get_px(),t2->get_py(),t2->get_pz());
  TVector3 w(t1->get_x()-t2->get_x(),t1->get_x()-t2->get_y(),t1->get_x()-t2->get_z());

  double a = u.Dot(u);
  double b = u.Dot(v);
  double c = v.Dot(v);
  double d = u.Dot(w);
  double e = v.Dot(w);

  double D = a*c - b*b;
  double sc, tc;
  // compute the line parameters of the two closest points
  if (D < eps) {         // the lines are almost parallel
    sc = 0.0;
    tc = (b>c ? d/b : e/c);   // use the largest denominator
  }
  else {
    sc = (b*e - c*d) / D;
    tc = (a*e - b*d) / D;
  }
  // get the difference of the two closest points
  u*=sc;
  v*=tc;
  w+=u;
  w-=v;
  return w.Mag()<=_kApprochCut;   // return the closest distance 
}

