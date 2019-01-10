#include "Conversion.h"
#include <assert.h>

int Conversion::setRecoTracks(SvtxTrackEval* trackeval){	
  this->trackeval=trackeval;
  if (e1)
  {
  	reco1=trackeval->best_track_from(e1); // have not checked that these are in range 
  }
  if (e2)
  {
  	reco2=trackeval->best_track_from(e1);
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
    reco2=trackeval->best_track_from(e1);
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
  return r;
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

bool Conversion::hasSilicon(){
  switch(recoCount){
          case 2:
            SvtxCluster *c1 = _svtxClusterMap->get(*(reco1->begin_clusters()));
            SvtxCluster *c2 = _svtxClusterMap->get(*(reco2->begin_clusters()));
            return c1->get_layer()<=_kNSiliconLayer||c2->get_layer()<=_kNSiliconLayer;
            break;
          case 1:
            SvtxCluster *c1;
            if (reco1)
            {
              c1 = _svtxClusterMap->get(*(reco1->begin_clusters()));
            }
            else{
              c1 = _svtxClusterMap->get(*(reco2->begin_clusters()));
            }
            return c1->get_layer()<=_kNSiliconLayer;
            break;
          default:
            return false;
            break;
        }
}

int Conversion::trackDLayer(){
  if (recoCount()==2){
          SvtxCluster *c1 = _svtxClusterMap->get(*(reco1->begin_clusters()));
          SvtxCluster *c2 = _svtxClusterMap->get(*(reco2->begin_clusters()));
          SvtxHit *h1 = _hitMap->get(*(c1->begin_hits()));
          SvtxHit *h2 = _hitMap->get(*(c2->begin_hits()));
          return abs(h1->get_layer()-h2->get_layer());
        }
        else return -1;
        
        //check that the first hits are close enough
        if (c1->get_layer()>_kNSiliconLayer&&c2->get_layer()>_kNSiliconLayer)
        {
          if (abs(h1->get_layer()-h2->get_layer())>_kFirstHitStrict)
          {
            return false;
          }
        }
        else{
          if (abs(h1->get_layer()-h2->get_layer())>_kFirstHit)
          {
            return false;
          }
        }
        //check the approach distance
        return true;
}