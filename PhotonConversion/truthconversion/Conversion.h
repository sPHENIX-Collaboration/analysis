#ifndef CONVERSION_H__
#define CONVERSION_H__

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <trackbase_historic/SvtxTrack.h>
#include <g4eval/SvtxTrackEval.h>

class SvtxTrackEval;

class Conversion
{
public:
  Conversion(){}
  Conversion(SvtxTrackEval* trackeval){
    this->trackeval=trackeval;
  }
  Conversion(PHG4VtxPoint* vtx){
    this->vtx=vtx;
  }
  Conversion(PHG4VtxPoint* vtx,SvtxTrackEval *trackeval){
    this->trackeval=trackeval;
    this->vtx=vtx;
  }
  ~Conversion(){
    //dont delete the points as you are not the owner and did not make your own copies
  }
  inline void setElectron(PHG4Particle* e){
    if (e1)
    {
      if (e2)
      {
        std::cout<<"WARNING: oversetting conversion electrons"<<std::endl;
      }
      else{
        e2=e;
      }
    }
    else{
      e1=e;
    }
  }
  inline void setTrackEval(SvtxTrackEval *trackeval){
    this->trackeval=trackeval;
  }
  inline bool setElectron(){
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
        std::cout<<"WARNING: bad charges in conversion"<<std::endl;
        return false;
      }
    }
    else{
      if (e1->get_pid()<0)
      {
        std::cout<<"WARNING: only 1 positron in conversion"<<std::endl;
        return false;
      }
    }
    return true;
  }
  inline void setVtx(PHG4VtxPoint* vtx){
    this->vtx=vtx;
  }
  inline PHG4VtxPoint* getVtx()const{
    return vtx;
  }
  inline bool isComplete()const{
    if (e1&&e2&&photon)
    {
      return true;
    }
    else{
      return false;
    }
  }
  inline bool hasPair()const{
    if (e1&&e2)
    {
      return true;
    }
    else{
      return false;
    }
  }
  inline void setParent(PHG4Particle* parent){
    photon=parent;
  }
  int setRecoTracks(SvtxTrackEval* trackeval);
  int setRecoTracks();

  inline int recoCount(){
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
  inline PHG4Particle* getElectron(){
    setElectron();
    return e1;
  }
  inline PHG4Particle* getPositron(){
    if(setElectron()){
      return e2;
    }
    else{
      return e1;
    }
  }
  inline PHG4Particle* getPhoton(){
    return photon;
  }
  inline int getEmbed() const {return embedID;}

  inline void setEmbed(int embedID) {this->embedID=embedID;}

  int get_cluster_id();
  int get_cluster_id(SvtxTrackEval *trackeval);
private:
  PHG4Particle* e1=NULL;
  PHG4Particle* e2=NULL;
  PHG4Particle* photon=NULL;
  PHG4VtxPoint* vtx=NULL;
  SvtxTrack* reco1=NULL;
  SvtxTrack* reco2=NULL;
  SvtxTrackEval* trackeval=NULL;

  int embedID=0;
  /*inline bool inAcceptance(){

  }*/
};
#endif //CONVERSION_H__
