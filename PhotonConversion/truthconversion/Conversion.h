/**
* @Conversion.h
* @author Francesco Vassalli <Francesco.Vassalli@colorado.edu>
* @version 1.0
*
* @brief Simple class to hold conversion information
*/
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
  Conversion(){verbosity=0;}
  Conversion(SvtxTrackEval* trackeval,int verbosity=0){
    this->trackeval=trackeval;
    this->verbosity=verbosity
  }
  Conversion(PHG4VtxPoint* vtx,int verbosity=0){
    this->vtx=vtx;
    this->verbosity=verbosity
  }
  Conversion(PHG4VtxPoint* vtx,SvtxTrackEval *trackeval,int verbosity=0){
    this->trackeval=trackeval;
    this->vtx=vtx;
    this->verbosity=verbosity
  }
  ~Conversion(){
    //dont delete the points as you are not the owner and did not make your own copies
  }
  /** sets the daughters of the conversion
  * use this to set the electron and positron
  * initializes both points but does not determine charge*/
  inline void setElectron(PHG4Particle* e){
    if (e1)
    {
      if (e2&&verbosity>0)
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
  /**Sets the negative charge to {@link e1}
  * @return false otherwise
  * will print warnings if something is wrong with the Conversion
  **/
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
  inline void setVtx(PHG4VtxPoint* vtx){
    this->vtx=vtx;
  }
  inline PHG4VtxPoint* getVtx()const{
    return vtx;
  }
  ///true if both electrons and photon are set
  inline bool isComplete()const{
    if (e1&&e2&&photon)
    {
      return true;
    }
    else{
      return false;
    }
  }
  ///check if both electrons are initialized
  inline bool hasPair()const{
    if (e1&&e2)
    {
      return true;
    }
    else{
      return false;
    }
  }
  ///sets the photon
  inline void setParent(PHG4Particle* parent){
    photon=parent;
  }
  /** match the truth particles to reco tracks
  * if the trackeval was not previously set it needs to be set now
  * @return the number of matched tracks [0,2]*/
  int setRecoTracks(SvtxTrackEval* trackeval);
  int setRecoTracks();
  /// @return the number of matched reco tracks
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
  ///calls {@link setElectron()} then returns {@link e1}
  inline PHG4Particle* getElectron(){
    setElectron();
    return e1;
  }
  ///calls {@link setElectron()} if true returns {@link e2} else returns {@link e1}
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
  /**finds the cluster associated with {@link reco1} 
  * if the trackeval was not previously set it needs to be set now
  * if {@link reco1} not set will attempt to set
  * @return -1 if no cluster is found*/
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
  int verbosity;
};
#endif //CONVERSION_H__
