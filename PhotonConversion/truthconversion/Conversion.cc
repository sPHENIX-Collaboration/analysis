#include "Conversion.h"
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <assert.h>

int Conversion::setRecoTracks(SvtxTrackEval* trackeval){	
  this->trackeval=trackeval;
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
  }
  if (reco2)
  {
    r++;
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
  }
  if (reco2)
  {
    r++;
  }
  setRecoPhoton();
  return r;
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


int Conversion::get_cluster_id(){
  if(!reco1){
    assert(trackeval);
    reco1=trackeval->best_track_from(e1);
    if(!reco1){
      cout<<"bad reco"<<endl;
      e1->identify();
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

int Conversion::trackDLayer(SvtxClusterMap* svtxClusterMap,SvtxHitMap* hitMap){
  if (recoCount()==2){
    SvtxCluster *c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
    SvtxCluster *c2 = svtxClusterMap->get(*(reco2->begin_clusters()));
    SvtxHit *h1 = hitMap->get(*(c1->begin_hits()));
    SvtxHit *h2 = hitMap->get(*(c2->begin_hits()));
    int l1 = h1->get_layer();
    int l2 = h2->get_layer();
    return abs(l1-l2);
  }
  else return -1;
}
int Conversion::firstLayer(SvtxClusterMap* svtxClusterMap){
  if (recoCount()==2){
    SvtxCluster *c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
    SvtxCluster *c2 = svtxClusterMap->get(*(reco2->begin_clusters()));
    if(c1->get_layer()>c2->get_layer()){
      return c2->get_layer();
    }
    else return c1->get_layer();
  }
  else return -1;
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

float Conversion::setRecoVtx(SvtxVertex *recovtx,SvtxClusterMap* svtxClusterMap){
    recoVtx=recovtx;
    SvtxCluster *c1 = svtxClusterMap->get(*(reco1->begin_clusters()));
    SvtxCluster *c2 = svtxClusterMap->get(*(reco2->begin_clusters()));
    float r1 = sqrt(abs(c1->get_x()-recovtx->get_x())+abs(c1->get_y()-recovtx->get_y())+abs(c1->get_z()-recovtx->get_z()));
    float r2 = sqrt(abs(c2->get_x()-recovtx->get_x())+abs(c2->get_y()-recovtx->get_y())+abs(c2->get_z()-recovtx->get_z()));
    if (r1>r2)
    {
      return r1;
    }
    else return r2;
}
