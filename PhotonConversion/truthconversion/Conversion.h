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
#include <trackbase_historic/SvtxHitMap.h>
#include <trackbase_historic/SvtxHit.h>
#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxCluster.h>
#include <trackbase_historic/SvtxVertex.h>
#include <g4eval/SvtxTrackEval.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <utility>

class SvtxTrackEval;
class PHCompositeNode;

class Conversion
{
public:
  Conversion(){verbosity=0;}
  Conversion(SvtxTrackEval* trackeval,int verbosity=0){
    this->trackeval=trackeval;
    this->verbosity=verbosity;
  }
  Conversion(PHG4VtxPoint* vtx,int verbosity=0){
    this->vtx=vtx;
    this->verbosity=verbosity;
  }
  Conversion(PHG4VtxPoint* vtx,SvtxTrackEval *trackeval,int verbosity=0){
    this->trackeval=trackeval;
    this->vtx=vtx;
    this->verbosity=verbosity;
  }
  //dtor at bottom of public methods
  
  /** sets the daughters of the conversion
  * use this to set the electron and positron
  * initializes both points but does not determine charge*/
  inline void setElectron(PHG4Particle* e){
    if (e1)
    {
      if (e2&&verbosity>0)
      {
        std::cout<<"WARNING in Conversion oversetting conversion electrons"<<std::endl;
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
  inline int recoCount()const{
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
  inline void setSourceId(int source){sourceId=source;}
  inline int getSourceId(){return sourceId;}
  /**Finds the cluster associated with {@link reco1} 
  * if the trackeval was not previously set it needs to be set now
  * if {@link reco1} not set will attempt to set
  * @return -1 if no cluster is found*/
  int get_cluster_id();
  int get_cluster_id(SvtxTrackEval *trackeval);

  inline std::pair<SvtxTrack*,SvtxTrack*> getRecoTracks(){
    return std::pair<SvtxTrack*,SvtxTrack*>(reco1,reco2);
  }

  /** Finds the delta eta of the reco tracks.
  * @return -1 if reco tracks are not set */
  inline float trackDEta(){
      if (recoCount()==2)
      {
        return fabs(reco1->get_eta()-reco2->get_eta());
      }
      else return -1.;
  }
  /** set the reco maps used for {@link trackDEta}, {@link trackDLayer},{@link hasSilicon}*/
  inline void setRecoMaps(SvtxClusterMap* cmap,SvtxHitMap* hmap){
    _svtxClusterMap=cmap;
    _hitMap=hmap;
  }
      /** Return the difference in layers of the first hits of the reco track 
      * @return -1 if reco tracks are not set*/
      int trackDLayer(SvtxClusterMap* cmap,SvtxHitMap* hitMap);
      ///@return the first layer the associated reco clusters hit
      int firstLayer(SvtxClusterMap* cmap);
      ///@return true if there are any silicon hits for the conversion
      bool hasSilicon(SvtxClusterMap* cmap);
      /** distance between two closest points on the reco tracks 
      * @return -1 if tracks are not set*/
  inline double approachDistance()const{
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

  inline SvtxVertex* getRecoVtx(){
    return recoVtx;
  }
  float dist(PHG4VtxPoint* vtx, SvtxClusterMap* cmap);
  float setRecoVtx(SvtxVertex* recovtx,SvtxClusterMap* cmap);
  TLorentzVector* setRecoPhoton();

  ~Conversion(){
    if(recoVertex) delete recoVertex;
    if(recoPhoton) delete recoPhoton;
    //dont delete the points as you are not the owner and did not make your own copies
  }

private:
  PHG4Particle* e1=NULL;
  PHG4Particle* e2=NULL;
  PHG4Particle* photon=NULL;
  PHG4VtxPoint* vtx=NULL;
  SvtxVertex* recoVtx=NULL;
  SvtxTrack* reco1=NULL;
  SvtxTrack* reco2=NULL;
  SvtxTrackEval* trackeval=NULL;
  SvtxClusterMap* _svtxClusterMap=NULL;                                                                              
  SvtxHitMap *_hitMap=NULL;
  SvtxVertex *recoVertex=NULL;
  TLorentzVector *recoPhoton=NULL;
  static const int _kNSiliconLayer =7; ///<hardcoded 
  int embedID=0;
  int verbosity;
  int sourceId;
  float _kElectronRestM=.5109989461;

};
#endif //CONVERSION_H__
