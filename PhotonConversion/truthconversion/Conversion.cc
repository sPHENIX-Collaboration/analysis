#include "Conversion.h"
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <assert.h>


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
    else if (e1->get_pid()<0)
    {
      std::cout<<"Warning in Conversion only 1 positron in conversion"<<std::endl;
      return false;
    }
  }
  return true;
}

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
