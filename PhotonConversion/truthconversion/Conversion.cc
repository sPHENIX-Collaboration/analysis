#include "Conversion.h"
#include "SVReco.h"
#include "VtxRegressor.h"
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phgenfit/Track.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <trackbase_historic/SvtxCluster.h>
#include <trackbase_historic/SvtxHitMap.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertex_v1.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrClusterv1.h>
#include <TRandom3.h>
#include <assert.h>


const float Conversion::_kElectronRestM=.0005109989461;

Conversion::Conversion(SvtxTrackEval* trackeval,int verbosity){
  this->trackeval=trackeval;
  this->verbosity=verbosity;
  _refit_phgf_tracks.first=NULL;
  _refit_phgf_tracks.second=NULL;
  pairTruthReco1.second=0; 
  pairTruthReco2.second=0; 
  pairTruthReco1.first=0; 
  pairTruthReco2.first=0; 
}
Conversion::Conversion(PHG4VtxPoint* vtx,int verbosity){
  this->vtx=vtx;
  this->verbosity=verbosity;
  _refit_phgf_tracks.first=NULL;
  _refit_phgf_tracks.second=NULL;
  pairTruthReco1.second=0; 
  pairTruthReco2.second=0; 
  pairTruthReco1.first=0; 
  pairTruthReco2.first=0; 
}

Conversion::Conversion(PHG4VtxPoint* vtx,SvtxTrackEval *trackeval,int verbosity){
  this->trackeval=trackeval;
  this->vtx=vtx;
  this->verbosity=verbosity;
  _refit_phgf_tracks.first=NULL;
  _refit_phgf_tracks.second=NULL;
  pairTruthReco1.second=0; 
  pairTruthReco2.second=0; 
  pairTruthReco1.first=0; 
  pairTruthReco2.first=0; 
}

Conversion::~Conversion(){
  if(recoVertex) delete recoVertex;
  if(recoPhoton) delete recoPhoton;
  if(truthSvtxVtx) delete truthSvtxVtx;
  if (_refit_phgf_tracks.first) delete _refit_phgf_tracks.first;
  if (_refit_phgf_tracks.second) delete _refit_phgf_tracks.second;
  _refit_phgf_tracks.first=NULL;
  _refit_phgf_tracks.second=NULL;
  recoVertex=NULL;
  recoPhoton=NULL;
  truthSvtxVtx=NULL;
  //dont delete the points as you are not the owner and did not make your own copies
}

void Conversion::setElectron(PHG4Particle* e){
  if (e1)
  {
    if (e2&&verbosity>0)
    {
      std::cout<<"WARNING in Conversion oversetting conversion electrons"<<std::endl;
    }
    else{
      e2=e;
      pairTruthReco2.second=e->get_track_id();
    }
  }
  else{
    e1=e;
    pairTruthReco1.second=e->get_track_id();
  }
}


bool Conversion::setElectron(){
  if (hasPair())
  {
    if (e1->get_pid()<0)
    {
      PHG4Particle *temp = e1;
      e1=e2;
      e2=temp;
    }
    if (e1->get_pid()<0)
    {
      if (verbosity>0)
      {
        std::cout<<"Warning in Conversion bad charges in conversion"<<std::endl;
      }
      return false;
    }
  }
  else{
    if (!e1)
    {
      if (verbosity>0)
      {
        std::cout<<"Warning in Conversion no truth tracks set"<<std::endl;
      }
      return false;
    }
    else if (e1->get_pid()<0) return false;
  }
  return true;
}

PHG4Particle* Conversion::getElectron(){
  setElectron();
  return e1;
}

PHG4Particle* Conversion::getPositron(){
  if(setElectron()){
    return e2;
  }
  else{
    return e1;
  }
}

bool Conversion::setParent(PHG4Particle* parent){
  bool r =true;
  if(!photon) photon=parent;
  else{
    if(!(*photon==*parent)) cerr<<"Bad photon matching!"<<endl;
    r=false;
  }
  return r;
}

void Conversion::setPrimaryPhoton(PHG4Particle* parent,PHG4TruthInfoContainer* truthinfo){
  if(!setParent(parent)) cerr<<"Bad photon matching during primary photon set"<<endl;
  if(photon->get_track_id()==parent->get_primary_id()){
    primaryPhoton=photon;
  }
  else{
    primaryPhoton=truthinfo->GetParticle(parent->get_primary_id());
  }
}

void Conversion::setRecoTrack(int truthID, SvtxTrack* recoTrack){
  if(!recoTrack)return;
  setElectron();
  setRecoTracks();
  if (e1&&e1->get_track_id()==truthID&&!reco1)
  {
    reco1=recoTrack;
    pairTruthReco1.first=e1->get_track_id();
    pairTruthReco1.second=recoTrack->get_id();
  }
  else if (e2&&e2->get_track_id()==truthID&&!reco2)
  {
    reco2=recoTrack;
    pairTruthReco2.first=e2->get_track_id();
    pairTruthReco2.second=recoTrack->get_id();
  }
}

int Conversion::setRecoTracks(SvtxTrackEval* trackeval){	
  this->trackeval=trackeval;
  setElectron();
  if (e1)
  {
    reco1=trackeval->best_track_from(e1);  
  }
  if (e2)
  {
    reco2=trackeval->best_track_from(e2);
  }
  if(reco2==reco1){
    reco2=NULL;
  }
  int r=0;
  if (reco1)
  {
    r++;
    pairTruthReco1.first = e1->get_track_id();
    pairTruthReco1.second = reco1->get_id();
  }
  if (reco2)
  {
    r++;
    pairTruthReco2.first = e2->get_track_id();
    pairTruthReco2.second = reco2->get_id();
  }
  setRecoPhoton();
  return r;
}

int Conversion::setRecoTracks(){  
  assert(trackeval);
  if (e1)
  {
    reco1=trackeval->best_track_from(e1); // have not checked that these are in range 
  }
  if (e2)
  {
    reco2=trackeval->best_track_from(e2);
  }
  if(reco2==reco1){
    reco2=NULL;
  }
  int r=0;
  if (reco1)
  {
    r++;
    pairTruthReco1.first = e1->get_track_id();
    pairTruthReco1.second = reco1->get_id();
  }
  if (reco2)
  {
    r++;
    pairTruthReco2.first = e2->get_track_id();
    pairTruthReco2.second = reco2->get_id();
  }
  setRecoPhoton();
  return r;
}

SvtxTrack* Conversion::getRecoTrack(unsigned truthID) const{
  if (pairTruthReco1.second==truthID)
  {
    if(reco1&&reco1->get_id()==pairTruthReco1.first)
      return reco1;
    else if(reco2&&reco2->get_id()==pairTruthReco1.first)
      return reco2;
  }
  else if (pairTruthReco2.second==truthID)
  {
    if(reco1&&reco1->get_id()==pairTruthReco2.first)
      return reco1;
    else if(reco2&&reco2->get_id()==pairTruthReco2.first)
      return reco2;
  }
  return NULL;
}

TLorentzVector* Conversion::setRecoPhoton(){
  if (reco1&&reco2)
  {
    TLorentzVector tlv1(reco1->get_px(),reco1->get_py(),reco1->get_pz(),
        sqrt(_kElectronRestM*_kElectronRestM+reco1->get_p()*reco1->get_p()));
    TLorentzVector tlv2(reco2->get_px(),reco2->get_py(),reco2->get_pz(),
        sqrt(_kElectronRestM*_kElectronRestM+reco2->get_p()*reco2->get_p()));
    if (recoPhoton) delete recoPhoton;
    recoPhoton= new TLorentzVector(tlv1+tlv2);
  }
  return recoPhoton;
}

TLorentzVector* Conversion::getRecoPhoton(){
  return setRecoPhoton();
}

TLorentzVector* Conversion::getRecoPhoton(SvtxTrack* reco1, SvtxTrack* reco2){
  if(!(reco1&&reco2)) return NULL;
  TLorentzVector tlv1(reco1->get_px(),reco1->get_py(),reco1->get_pz(),
      sqrt(_kElectronRestM*_kElectronRestM+reco1->get_p()*reco1->get_p()));
  TLorentzVector tlv2(reco2->get_px(),reco2->get_py(),reco2->get_pz(),
      sqrt(_kElectronRestM*_kElectronRestM+reco2->get_p()*reco2->get_p()));
  return new TLorentzVector(tlv1+tlv2);
}

TLorentzVector* Conversion::getRefitRecoPhoton(){
  std::pair<TLorentzVector*,TLorentzVector*> refit_tlvs =getRefitRecoTlvs();
  if (refit_tlvs.first&&refit_tlvs.second)
  {
    if(recoPhoton) delete recoPhoton;
    recoPhoton= new TLorentzVector(*refit_tlvs.first + *refit_tlvs.second);
  }
  return recoPhoton;
}

std::pair<TLorentzVector*,TLorentzVector*> Conversion::getRecoTlvs(){
  std::pair<TLorentzVector*,TLorentzVector*> r;
  switch(recoCount()){
    case 2:
      r.first = new TLorentzVector();
      r.first->SetPxPyPzE (reco1->get_px(),reco1->get_py(),reco1->get_pz(),
          sqrt(_kElectronRestM*_kElectronRestM+reco1->get_p()*reco1->get_p()));
      r.second =   new TLorentzVector();
      r.second->SetPxPyPzE (reco2->get_px(),reco2->get_py(),reco2->get_pz(),
          sqrt(_kElectronRestM*_kElectronRestM+reco2->get_p()*reco2->get_p()));
      break;
    case 1:
      if(reco1){
        r.first = new TLorentzVector();
        r.first->SetPxPyPzE (reco1->get_px(),reco1->get_py(),reco1->get_pz(),
            sqrt(_kElectronRestM*_kElectronRestM+reco1->get_p()*reco1->get_p()));
        r.second = NULL;
      }
      else{
        r.first = NULL;
        r.second = new TLorentzVector ();
        r.second->SetPxPyPzE(reco2->get_px(),reco2->get_py(),reco2->get_pz(),
            sqrt(_kElectronRestM*_kElectronRestM+reco2->get_p()*reco2->get_p()));
      }
      break;
    default:
      r.first=NULL;
      r.second=NULL;
      break;
  }
  return r;
}

std::pair<TLorentzVector*,TLorentzVector*> Conversion::getRefitRecoTlvs(){
  std::pair<TLorentzVector*,TLorentzVector*> r;
  if (_refit_phgf_tracks.first&&_refit_phgf_tracks.second)
  {
    r.first = new TLorentzVector();
    r.first->SetVectM (_refit_phgf_tracks.first->get_mom(),_kElectronRestM);
    r.second =   new TLorentzVector();
    r.second->SetVectM (_refit_phgf_tracks.second->get_mom(),_kElectronRestM);
  }
  else{
    r.first=NULL;
    r.second=NULL;
  }  
  return r;
}

PHG4Particle* Conversion::getTruthPhoton(PHG4TruthInfoContainer* truthinfo){
  return photon;
}


int Conversion::get_cluster_id(){
  if(!reco1){
    assert(trackeval);
    reco1=trackeval->best_track_from(e1);
    if(!reco1){
      return -1;
    }
  }
  return reco1->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1));//id of the emcal
}

int Conversion::get_cluster_id()const{
  if(!reco1){
    return -1;
  }
  else return reco1->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1));//id of the emcal
}

int Conversion::get_cluster_id(SvtxTrackEval *trackeval){
  this->trackeval=trackeval;
  if (!reco1)
  {
    reco1=trackeval->best_track_from(e1);
    if(!reco1){
      cout<<"bad reco"<<endl;
      return -1;
    }
  }
  return reco1->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1));//id of the emcal
}

std::pair<int,int> Conversion::get_cluster_ids(){
  switch(recoCount()){
    case 2:
      return std::pair<int,int>(reco1->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)),reco2->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
      break;
    case 1:
      if (reco1)
      {
        return std::pair<int,int>(reco1->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)),-1);
      }
      else return std::pair<int,int>(reco2->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)),-1);
      break;
    default:
      return std::pair<int,int>(-1,-1);
      break;
  }
}



bool Conversion::hasSilicon(SvtxClusterMap* svtxClusterMap){
  switch(recoCount()){
    case 2:
      {
        SvtxCluster *c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
        SvtxCluster *c2 = svtxClusterMap->get(*(reco2->begin_clusters()));
        return c1->get_layer()<=_kNSiliconLayer||c2->get_layer()<=_kNSiliconLayer;
      }
      break;
    case 1:
      {
        SvtxCluster *c1;
        if (reco1)
        {
          c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
        }
        else{
          c1 = svtxClusterMap->get(*(reco2->begin_clusters()));
        }
        return c1->get_layer()<=_kNSiliconLayer;
      }
      break;
    default:
      return false;
      break;
  }
}

float Conversion::vtxTrackRZ(TVector3 vertpos){
  float d1=vtxTrackRZ(vertpos,reco1);
  float d2=vtxTrackRZ(vertpos,reco2);
  return d1>d2?d1:d2;
}
float Conversion::vtxTrackRZ(TVector3 vertpos,SvtxTrack* reco1,SvtxTrack* reco2){
  float d1=vtxTrackRZ(vertpos,reco1);
  float d2=vtxTrackRZ(vertpos,reco2);
  return d1>d2?d1:d2;
}

float Conversion::vtxTrackRPhi(TVector3 vertpos){
  float d1=vtxTrackRPhi(vertpos,reco1);
  float d2=vtxTrackRPhi(vertpos,reco2);
  return d1>d2?d1:d2;
}

float Conversion::vtxTrackRPhi(TVector3 vertpos,SvtxTrack* reco1,SvtxTrack* reco2){
  float d1=vtxTrackRPhi(vertpos,reco1);
  float d2=vtxTrackRPhi(vertpos,reco2);
  return d1>d2?d1:d2;
}
float Conversion::vtxTrackRZ(TVector3 recoVertPos,SvtxTrack *track){
  float dR = sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y())-sqrt(track->get_x()*track->get_x()+track->get_y()*track->get_y());
  float dZ = recoVertPos.z()-track->get_z();
  return sqrt(dR*dR+dZ*dZ);
}

float Conversion::vtxTrackRPhi(TVector3 recoVertPos,SvtxTrack *track){
  float vtxR=sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y());
  float trackR=sqrt(track->get_x()*track->get_x()+track->get_y()*track->get_y());
  return sqrt(vtxR*vtxR+trackR*trackR-2*vtxR*trackR*cos(recoVertPos.Phi()-track->get_phi()));
}

int Conversion::trackDLayer(SvtxClusterMap* svtxClusterMap,SvtxHitMap *hitmap){
  if (recoCount()==2){
    SvtxCluster *c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
    SvtxCluster *c2 = svtxClusterMap->get(*(reco2->begin_clusters()));
    SvtxHit *h1 = hitmap->get(*(c1->begin_hits()));
    SvtxHit *h2 = hitmap->get(*(c2->begin_hits()));
    int l1 = h1->get_layer();
    int l2 = h2->get_layer();
    return abs(l1-l2);
  }
  else return -1;
}

float Conversion::minDca(){
  if(reco1->get_dca()>reco2->get_dca()) return reco2->get_dca();
  else return reco1->get_dca();
}

int Conversion::trackDLayer(TrkrClusterContainer* clusterMap){
  if (recoCount()==2){
    TrkrCluster *c1 = clusterMap->findCluster(*(reco1->begin_cluster_keys()));
    TrkrCluster *c2 = clusterMap->findCluster(*(reco2->begin_cluster_keys()));
    unsigned l1 = TrkrDefs::getLayer(c1->getClusKey());
    unsigned l2 = TrkrDefs::getLayer(c2->getClusKey());
    return abs(l1-l2);
  }
  else return -1;
}
int Conversion::trackDLayer(TrkrClusterContainer* clusterMap,SvtxTrack* reco1, SvtxTrack* reco2){
  TrkrCluster *c1 = clusterMap->findCluster(*(reco1->begin_cluster_keys()));
  TrkrCluster *c2 = clusterMap->findCluster(*(reco2->begin_cluster_keys()));
  unsigned l1 = TrkrDefs::getLayer(c1->getClusKey());
  unsigned l2 = TrkrDefs::getLayer(c2->getClusKey());
  return abs(l1-l2);
}

int Conversion::firstLayer(SvtxClusterMap* svtxClusterMap,SvtxHitMap *hitmap){
  switch(recoCount()){
    case 2:
      {
        SvtxCluster *c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
        SvtxCluster *c2 = svtxClusterMap->get(*(reco2->begin_clusters()));
        SvtxHit *h1 = hitmap->get(*(c1->begin_hits()));
        SvtxHit *h2 = hitmap->get(*(c2->begin_hits()));
        if(h1->get_layer()>h2->get_layer()){
          return h2->get_layer();
        }
        else return h1->get_layer();
      }
    case 1:
      {
        if (reco1)return svtxClusterMap->get(*(reco1->begin_clusters()))->get_layer();
        else return svtxClusterMap->get(*(reco2->begin_clusters()))->get_layer();
      }
    default:
      return -1;
  }
}

int Conversion::firstLayer(TrkrClusterContainer* clusterMap){
  switch(recoCount()){
    case 2:
      {
        TrkrCluster *c1 = clusterMap->findCluster(*(reco1->begin_cluster_keys()));
        TrkrCluster *c2 = clusterMap->findCluster(*(reco2->begin_cluster_keys()));
        unsigned l1 = TrkrDefs::getLayer(c1->getClusKey());
        unsigned l2 = TrkrDefs::getLayer(c2->getClusKey());
        //maybe i need this? TrkrDefs::hitsetkey hitsetkey = TrkrDefs::getHitSetKeyFromClusKey(cluskey);
        if(l1>l2){
          return l1;
        }
        else return l2;
      }
    case 1:
      {
        if (reco1)return TrkrDefs::getLayer(*(reco1->begin_cluster_keys()));
        else return TrkrDefs::getLayer(*(reco2->begin_cluster_keys()));
      }
    default:
      return -1;
  }
}

double Conversion::dist(PHG4VtxPoint *recovtx,SvtxClusterMap* svtxClusterMap){
  SvtxCluster *c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
  SvtxCluster *c2 = svtxClusterMap->get(*(reco2->begin_clusters()));
  double r1 = sqrt(abs(c1->get_x()-recovtx->get_x())+abs(c1->get_y()-recovtx->get_y())+abs(c1->get_z()-recovtx->get_z()));
  double r2 = sqrt(abs(c2->get_x()-recovtx->get_x())+abs(c2->get_y()-recovtx->get_y())+abs(c2->get_z()-recovtx->get_z()));
  if (r1>r2)
  {
    return r1;
  }
  else return r2;
}

double Conversion::dist(TVector3 *recovtx,SvtxClusterMap* svtxClusterMap){
  SvtxCluster *c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
  SvtxCluster *c2 = svtxClusterMap->get(*(reco2->begin_clusters()));
  double r1 = sqrt(abs(c1->get_x()-recovtx->x())+abs(c1->get_y()-recovtx->y())+abs(c1->get_z()-recovtx->z()));
  double r2 = sqrt(abs(c2->get_x()-recovtx->x())+abs(c2->get_y()-recovtx->y())+abs(c2->get_z()-recovtx->z()));
  if (r1>r2)
  {
    return r1;
  }
  else return r2;
}

double Conversion::dist(TVector3 *recovtx,TrkrClusterContainer* clusterMap){
  TrkrCluster *c1 = clusterMap->findCluster(*(reco1->begin_cluster_keys()));
  TrkrCluster *c2 = clusterMap->findCluster(*(reco2->begin_cluster_keys()));
  double r1 = sqrt(abs(c1->getX()-recovtx->x())+abs(c1->getY()-recovtx->y())+abs(c1->getZ()-recovtx->z()));
  double r2 = sqrt(abs(c2->getX()-recovtx->x())+abs(c2->getY()-recovtx->y())+abs(c2->getZ()-recovtx->z()));
  if (r1>r2)
  {
    return r1;
  }
  else return r2;
}

void Conversion::printTruth(){
  cout<<"Conversion with truth info:\n";
  if (e1) e1->identify();
  if (e2) e2->identify();
  if (vtx) recoVertex->Print();
  if (recoPhoton) recoPhoton->Print();
}
void Conversion::printReco(){
  cout<<"Conversion with reco info:\n";
  if (reco1) reco1->identify();
  if (reco2) reco2->identify();
  if (vtx) vtx->identify();
  if (photon) photon->identify();
}

void Conversion::PrintPhotonRecoInfo(){
  if(!recoPhoton) cerr<<"No photon reconstructed"<<endl;
  else{
    cout<<"Truth Track: ";e1->identify();
    cout<<"Reco Track: ";getRecoTrack(e1->get_track_id())->identify(); 
    cout<<"Truth Track: ";e2->identify();
    cout<<"Reco Track: ";getRecoTrack(e2->get_track_id())->identify(); 
    cout<<"Truth Photon: ";photon->identify();
    cout<<"Reco Photon: ";recoPhoton->Print();
  }
}
void Conversion::PrintPhotonRecoInfo(TLorentzVector *tlv_photon,TLorentzVector *tlv_electron, TLorentzVector *tlv_positron,float mass){
  if(!recoPhoton) cerr<<"No photon reconstructed"<<endl;
  else{
    cout<<"Truth Track: ";tlv_electron->Print();
    cout<<"Reco Track: ";reco1->identify(); 
    cout<<"Truth Track: ";tlv_positron->Print();
    cout<<"Reco Track: ";reco2->identify(); 
    cout<<"Truth Photon: ";tlv_photon->Print();
    cout<<"Reco Photon with mass: "<<mass<<": ";recoPhoton->Print();
  }
}

/*This is deprecated
 * float Conversion::setRecoVtx(SvtxVertex *recovtx,SvtxClusterMap* svtxClusterMap){
 recoVertex=recovtx;
 SvtxCluster *c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
 SvtxCluster *c2 = svtxClusterMap->get(*(reco2->begin_clusters()));
 float r1 = sqrt(abs(c1->get_x()-recovtx->get_x())+abs(c1->get_y()-recovtx->get_y())+abs(c1->get_z()-recovtx->get_z()));
 float r2 = sqrt(abs(c2->get_x()-recovtx->get_x())+abs(c2->get_y()-recovtx->get_y())+abs(c2->get_z()-recovtx->get_z()));
 if (r1>r2)
 {
 return r1;
 }
 else return r2;
 }*/

double Conversion::approachDistance()const{
  if (recoCount()==2)
  {
    static const double eps = 0.000001;
    TVector3 u(reco1->get_px(),reco1->get_py(),reco1->get_pz());
    TVector3 v(reco2->get_px(),reco2->get_py(),reco2->get_pz());
    TVector3 w(reco1->get_x()-reco2->get_x(),reco1->get_x()-reco2->get_y(),reco1->get_x()-reco2->get_z());

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
    return w.Mag();   // return the closest distance 
  }
  else return -1; 
}

float Conversion::trackDEta()const{
  if (recoCount()==2)
  {
    return fabs(reco1->get_eta()-reco2->get_eta());
  }
  else return -1.;
}

double Conversion::approachDistance(SvtxTrack* reco1, SvtxTrack* reco2){
  static const double eps = 0.000001;
  TVector3 u(reco1->get_px(),reco1->get_py(),reco1->get_pz());
  TVector3 v(reco2->get_px(),reco2->get_py(),reco2->get_pz());
  TVector3 w(reco1->get_x()-reco2->get_x(),reco1->get_x()-reco2->get_y(),reco1->get_x()-reco2->get_z());

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
  return w.Mag();   // return the closest distance 
}

float Conversion::trackDEta(SvtxTrack* reco1, SvtxTrack* reco2){
  return fabs(reco1->get_eta()-reco2->get_eta());
}

float Conversion::minTrackpT(){
  switch(recoCount()){
    case 2:
      if (reco1->get_pt()<reco2->get_pt())
      {
        return reco1->get_pt();
      }
      else return reco2->get_pt();
      break;
    case 1:
      if (reco1)
      {
        return reco1->get_pt();
      }
      else return reco2->get_pt();
      break;
    default:
      return -1;
      break;
  }
}

std::pair<float,float> Conversion::getTrackpTs(){
  switch(recoCount()){
    case 2:
      return std::pair<float,float>(reco1->get_pt(),reco2->get_pt());
    case 1:
      if (reco1) return std::pair<float,float>(reco1->get_pt(),-1);
      else return std::pair<float,float>(-1,reco2->get_pt());
      break;
    default:
      return std::pair<float,float>(-1,-1);
      break;
  }
}

std::pair<float,float> Conversion::getTrackEtas(){
  switch(recoCount()){
    case 2:
      return std::pair<float,float>(reco1->get_eta(),reco2->get_eta());
    case 1:
      if (reco1) return std::pair<float,float>(reco1->get_eta(),-1);
      else return std::pair<float,float>(-1,reco2->get_eta());
      break;
    default:
      return std::pair<float,float>(-1,-1);
      break;
  }
}

std::pair<float,float> Conversion::getTrackPhis(){
  switch(recoCount()){
    case 2:
      return std::pair<float,float>(reco1->get_phi(),reco2->get_phi());
    case 1:
      if (reco1) return std::pair<float,float>(reco1->get_phi(),-1);
      else return std::pair<float,float>(-1,reco2->get_phi());
      break;
    default:
      return std::pair<float,float>(-1,-1);
      break;
  }
}
/*This is the ideal case but I do not have RaveVtx to SvtxVertex matching yet
 * genfit::GFRaveVertex* Conversion::getSecondaryVertex(SVReco* vertexer){
 if(recoCount()==2){
 if (recoVertex) delete recoVertex;
 recoVertex= vertexer->findSecondaryVertex(reco1,reco2);
 }
 return recoVertex;
 }*/
genfit::GFRaveVertex* Conversion::getSecondaryVertex(SVReco* vertexer){
  if(recoCount()==2){
    //this might seg fault
    if(recoVertex) delete recoVertex;
    recoVertex= vertexer->findSecondaryVertex(reco1,reco2);
  }
  return recoVertex;
}

genfit::GFRaveVertex* Conversion::correctSecondaryVertex(VtxRegressor* regressor){
  if(!recoVertex) {
    cerr<<"WARNING: no vertex to correct"<<endl;
    return NULL;
  }
  if (recoCount()!=2)
  {
    cerr<<"WARNING: no reco tracks to do vertex correction"<<endl;
    return NULL;
  }

  TVector3 nextPos = recoVertex->getPos();
  nextPos.SetMagThetaPhi(regressor->regress(reco1,reco2,recoVertex),nextPos.Theta(),nextPos.Phi());

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

genfit::GFRaveVertex* Conversion::correctSecondaryVertex(VtxRegressor* regressor,genfit::GFRaveVertex* recoVertex, SvtxTrack* reco1, SvtxTrack* reco2){
  if(!recoVertex) {
    cerr<<"WARNING: no vertex to correct"<<endl;
    return NULL;
  }
  if (!(reco1&&reco2))
  {
    cerr<<"WARNING: no reco tracks to do vertex correction"<<endl;
    return NULL;
  }

  TVector3 nextPos = recoVertex->getPos();
  nextPos.SetMagThetaPhi(regressor->regress(reco1,reco2,recoVertex),nextPos.Theta(),nextPos.Phi());

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

std::pair<PHGenFit::Track*,PHGenFit::Track*> Conversion::getPHGFTracks(SVReco* vertexer){
  std::pair<PHGenFit::Track*,PHGenFit::Track*> r;
  r.first = vertexer->getPHGFTrack(reco1);
  r.second = vertexer->getPHGFTrack(reco2);
  return r;
}

std::pair<PHGenFit::Track*,PHGenFit::Track*> Conversion::refitTracksTruthVtx(SVReco* vertexer,SvtxVertex* seedVtx){
  PHG4VtxPointToSvtxVertex(seedVtx);
  if(_refit_phgf_tracks.first) delete _refit_phgf_tracks.first;
  if(_refit_phgf_tracks.second) delete _refit_phgf_tracks.second;
  _refit_phgf_tracks.first=vertexer->refitTrack(truthSvtxVtx,reco1);
  _refit_phgf_tracks.second=vertexer->refitTrack(truthSvtxVtx,reco2);
  return _refit_phgf_tracks;
}

std::pair<PHGenFit::Track*,PHGenFit::Track*> Conversion::refitTracksTruthVtx(SVReco* vertexer){
  PHG4VtxPointToSvtxVertex();
  if(_refit_phgf_tracks.first) delete _refit_phgf_tracks.first;
  if(_refit_phgf_tracks.second) delete _refit_phgf_tracks.second;
  _refit_phgf_tracks.first=vertexer->refitTrack(truthSvtxVtx,reco1);
  _refit_phgf_tracks.second=vertexer->refitTrack(truthSvtxVtx,reco2);
  return _refit_phgf_tracks;
}
std::pair<PHGenFit::Track*,PHGenFit::Track*> Conversion::refitTracks(SVReco* vertexer){
  if (!recoVertex) getSecondaryVertex(vertexer);
  if(!recoVertex)
  {
    cerr<<"WARNING: No vertex to refit tracks"<<endl;
  }
  else{
    if(_refit_phgf_tracks.first) delete _refit_phgf_tracks.first;
    if(_refit_phgf_tracks.second) delete _refit_phgf_tracks.second;
    _refit_phgf_tracks.first=vertexer->refitTrack(recoVertex,reco1);
    _refit_phgf_tracks.second=vertexer->refitTrack(recoVertex,reco2);
  }
  return _refit_phgf_tracks;
}

std::pair<PHGenFit::Track*,PHGenFit::Track*> Conversion::refitTracks(SVReco* vertexer, SvtxVertex* recoVtx){
  if (!recoVtx)
  {
    cerr<<"WARNING: No vertex to refit tracks"<<endl;
  }
  else{
    if(_refit_phgf_tracks.first) delete _refit_phgf_tracks.first;
    if(_refit_phgf_tracks.second) delete _refit_phgf_tracks.second;
    _refit_phgf_tracks.first=vertexer->refitTrack(recoVtx,reco1);
    _refit_phgf_tracks.second=vertexer->refitTrack(recoVtx,reco2);
  }
  return _refit_phgf_tracks;
}

void Conversion::PHG4VtxPointToSvtxVertex(){
  truthSvtxVtx = new SvtxVertex_v1();
  truthSvtxVtx->set_x(vtx->get_x());
  truthSvtxVtx->set_y(vtx->get_y());
  truthSvtxVtx->set_z(vtx->get_z());
  truthSvtxVtx->set_t0(vtx->get_t());
  truthSvtxVtx->set_chisq(1.);
  truthSvtxVtx->set_ndof(1);

  TRandom3 rand(0);
  double ae = 0.0007;  //7 um
  double i = 0.003;//30 um
  double d = rand.Gaus(0, ae);
  double g = rand.Gaus(0, ae);
  double h = rand.Gaus(0, i);
  truthSvtxVtx->set_error(0,0,ae*ae);
  truthSvtxVtx->set_error(1,1,d*d+ae*ae);
  truthSvtxVtx->set_error(2,2,g*g+h*h+i*i);
  truthSvtxVtx->set_error(0,1,ae*d);
  truthSvtxVtx->set_error(1,0,ae*d);
  truthSvtxVtx->set_error(2,0,ae*g);
  truthSvtxVtx->set_error(0,2,ae*g);
  truthSvtxVtx->set_error(1,2,d*g+ae*h);
  truthSvtxVtx->set_error(2,1,d*g+ae*h);
  switch(recoCount()){
    case 2:
      truthSvtxVtx->insert_track(reco1->get_id());
      truthSvtxVtx->insert_track(reco2->get_id());
    case 1:
      if (reco1) return truthSvtxVtx->insert_track(reco1->get_id());
      else return truthSvtxVtx->insert_track(reco2->get_id());
      break;
    default:
      break;
  }
}

void Conversion::PHG4VtxPointToSvtxVertex(SvtxVertex* seedVtx){
  truthSvtxVtx = new SvtxVertex_v1();
  truthSvtxVtx->set_x(vtx->get_x());
  truthSvtxVtx->set_y(vtx->get_y());
  truthSvtxVtx->set_z(vtx->get_z());
  truthSvtxVtx->set_t0(vtx->get_t());
  truthSvtxVtx->set_chisq(1.);
  truthSvtxVtx->set_ndof(1);

  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      truthSvtxVtx->set_error(i,j,seedVtx->get_error(i,j));
    }
  }
  switch(recoCount()){
    case 2:
      truthSvtxVtx->insert_track(reco1->get_id());
      truthSvtxVtx->insert_track(reco2->get_id());
    case 1:
      if (reco1) return truthSvtxVtx->insert_track(reco1->get_id());
      else return truthSvtxVtx->insert_track(reco2->get_id());
      break;
    default:
      break;
  }
}

