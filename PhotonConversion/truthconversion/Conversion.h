/**
 * @Conversion.h
 * @author Francesco Vassalli <Francesco.Vassalli@colorado.edu>
 * @version 1.0
 *
 * @brief Simple class to hold conversion information
 * This could be optimized by initializing the object with the necessary node pointers
 **/
#ifndef CONVERSION_H__
#define CONVERSION_H__

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <g4eval/SvtxTrackEval.h>
#include <GenFit/GFRaveVertex.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include <utility>

namespace PHGenFit {
  class Track;
} /* namespace PHGenFit */


class SvtxTrackEval;
class PHCompositeNode;
class PHG4TruthInfoContainer;
class TLorentzVector;
class SvtxHitMap;
class SVReco;
class VtxRegressor;

//TODO check function to see ifthey can be made const
class Conversion
{
  public:
    Conversion(){verbosity=0;}
    Conversion(SvtxTrackEval* trackeval,int verbosity=0);
    Conversion(PHG4VtxPoint* vtx,int verbosity=0);
    Conversion(PHG4VtxPoint* vtx,SvtxTrackEval *trackeval,int verbosity=0);
    ~Conversion();
    /** sets the daughters of the conversion
     * use this to set the electron and positron
     * initializes both points but does not determine charge**/
    void setElectron(PHG4Particle* e);
    inline void setTrackEval(SvtxTrackEval *trackeval){this->trackeval=trackeval;}
    inline void setVtx(PHG4VtxPoint* vtx){this->vtx=vtx;}
    inline PHG4VtxPoint* getVtx()const{return vtx;}
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
    ///sets the photon @return true if there are no errors
    bool setParent(PHG4Particle* parent);

    ///calls @{link setParent} {@link photon} is set with @param parent and the {@link primaryPhoton} is set with the primary particle associated with @param parent
    void setPrimaryPhoton(PHG4Particle* parent,PHG4TruthInfoContainer* truthinfo);

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
    PHG4Particle* getElectron();
    ///@return {@link e1}
    inline PHG4Particle* getElectron()const{return e1;}
    ///calls {@link setElectron()} if true returns {@link e2} else returns {@link e1}
    PHG4Particle* getPositron();
    ///@return {@link e2}
    inline PHG4Particle* getPositron()const{return e2;}
    inline PHG4Particle* getPhoton()const{return photon;}
    inline PHG4Particle* getPrimaryPhoton()const{return primaryPhoton;}
    inline int getEmbed() const {return embedID;}

    inline void setEmbed(int embedID) {this->embedID=embedID;}
    inline void setSourceId(int source){sourceId=source;}
    inline int getSourceId()const{return sourceId;}
    inline genfit::GFRaveVertex* getRecoVtx()const{return recoVertex;}
    /** Finds the delta eta of the reco tracks.
     * @return -1 if reco tracks are not set **/
    float trackDEta()const;
    static float trackDEta(SvtxTrack* reco1, SvtxTrack* reco2);
    /** Finds the delta phi of the reco tracks.
     * @return -1 if reco tracks are not set **/
    inline float trackDPhi()const{
      if (recoCount()==2)
      {
        return DeltaPhi(reco1->get_phi(),reco2->get_phi());
      }
      else return -1.;
    }
    inline static float trackDPhi(SvtxTrack* reco1, SvtxTrack* reco2){
      return DeltaPhi(reco1->get_phi(),reco2->get_phi());
    }
    ///@return the minimun reco track pT
    float minTrackpT();
    std::pair<float,float> getTrackpTs();
    std::pair<float,float> getTrackEtas();
    std::pair<float,float> getTrackPhis();
    inline std::pair<SvtxTrack*,SvtxTrack*> getRecoTracks()const{
      return std::pair<SvtxTrack*,SvtxTrack*>(reco1,reco2);
    }
    ///@return the reco track associated with the @param truthID
    SvtxTrack* getRecoTrack(unsigned truthID) const;
    /** set the reco maps used for {@link trackDEta}, {@link trackDLayer},{@link hasSilicon}
     *Note that all use of SvtxClusterMap is now deprecated**/
    inline void setClusterMap(SvtxClusterMap* cmap){
      _svtxClusterMap=cmap;
    }
    inline int setVerbosity(int v){
      verbosity=v;
      return verbosity;
    }
    /**Finds the cluster associated with {@link reco1} 
     * if the trackeval was not previously set it needs to be set now
     * if {@link reco1} not set will attempt to set
     * @return -1 if no cluster is found**/
    int get_cluster_id();
    /**Finds the cluster associated with {@link reco1} 
     * @return -1 if no cluster is found**/
    int get_cluster_id()const;
    /**Finds the cluster associated with {@link reco1} 
     * if {@link reco1} not set will attempt to set
     * @return -1 if no cluster is found**/
    int get_cluster_id(SvtxTrackEval *trackeval);
    /**@return the cluster ID for both tracks if both are set. 
     *If one is set pair.first will be the id.
     *the unset tracks get -1 as the cluster id**/
    std::pair<int,int> get_cluster_ids();
    /** match the truth particles to reco tracks
     * if the trackeval was not previously set it needs to be set now
     * @return the number of matched tracks [0,2]**/
    int setRecoTracks(SvtxTrackEval* trackeval);
    int setRecoTracks();
    /** call {@link setElectron()} then {@link setRecoTrakcs} then set the 
     * reco track for the truth particle that corresponds to @param truthID 
     * if it does not already have a reco track**/
    void setRecoTrack(int truthID, SvtxTrack* recoTrack);
    /**Sets the negative charge to {@link e1}
     * @return false otherwise
     * will print warnings if something is wrong with the Conversion
     **/
    bool setElectron();
    /** Return the difference in layers of the first hits of the reco track 
     * @return -1 if reco tracks are not set**/
    int trackDLayer(SvtxClusterMap* cmap,SvtxHitMap *hitmap);
    int trackDLayer(TrkrClusterContainer* );
    static int trackDLayer(TrkrClusterContainer*,SvtxTrack*,SvtxTrack* );
    ///@return the first layer the associated reco clusters hit
    int firstLayer(SvtxClusterMap* cmap,SvtxHitMap *hitmap);
    int firstLayer(TrkrClusterContainer* );
    ///@return true if there are any silicon hits for the conversion. Is deprecated.
    bool hasSilicon(SvtxClusterMap* );
    /** distance between two closest points on the reco tracks 
     * @return -1 if tracks are not set**/
    double approachDistance()const;
    static double approachDistance(SvtxTrack*,SvtxTrack*);
    ///@return distance in xyz space between the vertex and the closest track hit
    double dist(PHG4VtxPoint* vtx, SvtxClusterMap* cmap);
    double dist(TVector3* vtx, SvtxClusterMap* cmap);
    double dist(TVector3* vtx, TrkrClusterContainer* cmap);
    ///@return dca of the reco track with the lower dca
    float minDca();
    ///@return the RPhi distance between the reco vtx and further track
    float vtxTrackRPhi(TVector3 vertpos);
    static float vtxTrackRPhi(TVector3 vertpos,SvtxTrack*,SvtxTrack*);
    ///@return the RZ distance between the reco vtx and further track
    float vtxTrackRZ(TVector3 vertpos);
    static float vtxTrackRZ(TVector3 vertpos,SvtxTrack*,SvtxTrack*);

    //Deprecated 
    //float setRecoVtx(SvtxVertex* recovtx,SvtxClusterMap* cmap);
    TLorentzVector* setRecoPhoton();///< constructs a tlv for a photon by adding the tlvs for the reco tracks
    TLorentzVector* getRecoPhoton();///<@return the constructed tlv does not return ownserhip
    static TLorentzVector* getRecoPhoton(SvtxTrack* reco1, SvtxTrack* reco2);///<@return the addition of respective tlvs, @return ownership
    ///Uses the {@link _refit_phgf_tracks} to set {@link recoPhoton}. Will delete any existing {@link recoPhoton}. @return NULL if either {@link _refit_phgf_tracks} are NULL.
    TLorentzVector* getRefitRecoPhoton();
    PHG4Particle* getTruthPhoton(PHG4TruthInfoContainer*);///<@return NULL if not valid conversion else return photon
    ///Uses {@link recoVertex} and \param vertexer to make {@link _refit_phgf_tracks}. Sets {@link recoVertex} to \param recoVtx
    std::pair<PHGenFit::Track*,PHGenFit::Track*> refitTracks(SVReco* vertexer, SvtxVertex* recoVtx);
    ///Uses {@link recoVertex} and \param vertexer to make {@link _refit_phgf_tracks}. If {@link recoVertex} is not set call {@link getSecondaryVertex(SVReco* vertexer)}.
    std::pair<PHGenFit::Track*,PHGenFit::Track*> refitTracks(SVReco* vertexer);

    ///Uses the truth vertex and {@link SVReco} to improve the fit of {@link reco1} and {@link reco2}
    std::pair<PHGenFit::Track*,PHGenFit::Track*> refitTracksTruthVtx(SVReco* vertexer);
    ///Uses the truth vertex and {@link SVReco} to improve the fit of {@link reco1} and {@link reco2}. Uses @param seedVtx to set the uncertainies on the truth vertex.
    std::pair<PHGenFit::Track*,PHGenFit::Track*> refitTracksTruthVtx(SVReco* vertexer,SvtxVertex* seedVtx);
    //TODO Set {@link recoVertex} to the vertex reconstructed from {@link SVReco} using the reco tracks and @return {@link recoVertex}. @return NULL if {@link recoCount()}!=2. Delete any existing {@link recoVertex}
    ///@return the secondary vertex fit with {@link reco1} and {@link reco2}
    genfit::GFRaveVertex* getSecondaryVertex(SVReco* vertexer);
    ///get the PHGF version of {@link reco1} and {@link reco2}. If not possible @return NULL for that track.
    std::pair<PHGenFit::Track*,PHGenFit::Track*> getPHGFTracks(SVReco* vertexer);
    /*Convertes {@link reco1} and {@link reco2} into a pair of TLorentzVector* using the electron mass. 
     * references {@link recoCount()} to determine which NULL values to @return
     * Ownership is returned.**/
    std::pair<TLorentzVector*,TLorentzVector*> getRecoTlvs();
    /*Convertes {@link _refit_phgf_tracks} into a pair of TLorentzVector* using the electron mass. 
     * If either refit track is not defined @return will be a pair of NULL vectors
     * Ownership is returned.**/
    std::pair<TLorentzVector*,TLorentzVector*> getRefitRecoTlvs();
    /** @return the truth particle(s) that are missing reco tracks
     * fill NULL if the particles have a reco track 
     * in the case that only one is missing a track it will be the first value of the pair**/
    std::pair<PHG4Particle*,PHG4Particle*> getParticlesMissingTrack();
    ///Uses the specified TMVA method to update and @return {@link recoVertex} 
    genfit::GFRaveVertex* correctSecondaryVertex(VtxRegressor* regressor);
    static genfit::GFRaveVertex* correctSecondaryVertex(VtxRegressor* regressor,genfit::GFRaveVertex*, SvtxTrack* reco1, SvtxTrack* reco2);
    /**
     * Returns the equivalent angle in the range 0 to 2pi.
     **/
    inline static double InTwoPi (double phi) {
      while (phi < 0 || 2*TMath::Pi() <= phi) {
        if (phi < 0) phi += 2*TMath::Pi();
        else phi -= 2*TMath::Pi();
      }
      return phi;
    }

    /**
     * Returns the difference between two angles in 0 to pi.
     **/
    inline static double DeltaPhi (double phi1, double phi2) {
      phi1 = InTwoPi(phi1);
      phi2 = InTwoPi(phi2);
      double dphi = abs(phi1 - phi2);
      while (dphi > TMath::Pi()) dphi = abs (dphi - 2*TMath::Pi());
      return dphi;
    }

    ///print the truth info calls {@link e1->identify()}, {@link e2->identify()}, {@link vtx->identify()}, {@link photon->identify()}
    void printTruth();
    ///print the reco info calls {@link reco1->identify()}, {@link reco2->identify()}, {@link recoVertex->identify()}, {@link recoPhoton->print()}
    void printReco();
    ///print the info relating to photon reconstruction
    void PrintPhotonRecoInfo();
    void PrintPhotonRecoInfo(TLorentzVector *tlv_photon,TLorentzVector *tlv_electron, TLorentzVector *tlv_positron,float mass);
  private:
    PHG4Particle* e1=NULL;
    PHG4Particle* e2=NULL;
    PHG4Particle* photon=NULL;
    PHG4Particle* primaryPhoton=NULL;
    PHG4VtxPoint* vtx=NULL;
    SvtxVertex* truthSvtxVtx=NULL;
    SvtxTrack* reco1=NULL;
    SvtxTrack* reco2=NULL;
    SvtxTrackEval* trackeval=NULL;
    SvtxClusterMap* _svtxClusterMap=NULL;                                                                              
    genfit::GFRaveVertex *recoVertex=NULL;
    TLorentzVector *recoPhoton=NULL;
    std::pair<PHGenFit::Track*,PHGenFit::Track*> _refit_phgf_tracks;
    std::pair<unsigned,unsigned> pairTruthReco1;
    std::pair<unsigned,unsigned> pairTruthReco2;

    static const int _kNSiliconLayer =7; ///<hardcoded 
    int embedID=0;
    int verbosity;
    int sourceId;
    static const float _kElectronRestM;
    ///helper function 
    static float vtxTrackRZ(TVector3 recoVertPos,SvtxTrack *track);
    ///helper function 
    static float vtxTrackRPhi(TVector3 recoVertPos,SvtxTrack *track);
    /**Initializes {@link truthSvtxVtx} with x,y,x,t from the PHG4VtxPoint. 
     *Chisq and ndof are set to 1. 
     *The tracks are set with {@link reco1} and {@link reco2}. 
     *The errors are set to a randomish positive-definite matrix**/
    void PHG4VtxPointToSvtxVertex();
    /**Initializes {@link truthSvtxVtx} with x,y,x,t from the PHG4VtxPoint. 
     *Chisq and ndof are set to 1. 
     *The tracks are set with {@link reco1} and {@link reco2}. 
     *The errors are set to the errors of the seedVtx. The primary vtx is suggested.**/
    void PHG4VtxPointToSvtxVertex(SvtxVertex* seedVtx);

};
#endif //CONVERSION_H__
