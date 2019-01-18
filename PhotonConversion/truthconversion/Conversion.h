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
		///sets the photon
		inline void setParent(PHG4Particle* parent){photon=parent;}

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
    ///@return {@link e1}
    inline PHG4Particle* getElectron()const{return e1;}
		///calls {@link setElectron()} if true returns {@link e2} else returns {@link e1}
		inline PHG4Particle* getPositron(){
			if(setElectron()){
				return e2;
			}
			else{
				return e1;
			}
		}
    ///@return {@link e2}
    inline PHG4Particle* getPositron()const{return e2;}
		inline PHG4Particle* getPhoton()const{return photon;}
		inline int getEmbed() const {return embedID;}

		inline void setEmbed(int embedID) {this->embedID=embedID;}
		inline void setSourceId(int source){sourceId=source;}
		inline int getSourceId()const{return sourceId;}
		inline SvtxVertex* getRecoVtx()const{return recoVtx;}
		/** Finds the delta eta of the reco tracks.
		 * @return -1 if reco tracks are not set */
		inline float trackDEta()const{
			if (recoCount()==2)
			{
				return fabs(reco1->get_eta()-reco2->get_eta());
			}
			else return -1.;
		}
    /** Finds the delta phi of the reco tracks.
     * @return -1 if reco tracks are not set */
		inline float trackDPhi()const{
			if (recoCount()==2)
			{
				return fabs(reco1->get_phi()-reco2->get_phi());
			}
			else return -1.;
		}
    ///@return the minimun reco track pT
		inline float minTrackpT(){
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
		inline std::pair<SvtxTrack*,SvtxTrack*> getRecoTracks()const{
			return std::pair<SvtxTrack*,SvtxTrack*>(reco1,reco2);
		}
		/** set the reco maps used for {@link trackDEta}, {@link trackDLayer},{@link hasSilicon}*/
		inline void setRecoMaps(SvtxClusterMap* cmap,SvtxHitMap* hmap){
			_svtxClusterMap=cmap;
			_hitMap=hmap;
		}
		/**Finds the cluster associated with {@link reco1} 
		 * if the trackeval was not previously set it needs to be set now
		 * if {@link reco1} not set will attempt to set
		 * @return -1 if no cluster is found*/
    int get_cluster_id();
    /**Finds the cluster associated with {@link reco1} 
     * @return -1 if no cluster is found*/
		int get_cluster_id()const;
    /**Finds the cluster associated with {@link reco1} 
     * if {@link reco1} not set will attempt to set
     * @return -1 if no cluster is found*/
    int get_cluster_id(SvtxTrackEval *trackeval);
		/**@return the cluster ID for both tracks if both are set. 
		 *If one is set pair.first will be the id.
		 *the unset tracks get -1 as the cluster id*/
		std::pair<int,int> get_cluster_ids();
		/** match the truth particles to reco tracks
		 * if the trackeval was not previously set it needs to be set now
		 * @return the number of matched tracks [0,2]*/
		int setRecoTracks(SvtxTrackEval* trackeval);
		int setRecoTracks();
		/**Sets the negative charge to {@link e1}
		 * @return false otherwise
		 * will print warnings if something is wrong with the Conversion
		 **/
		bool setElectron();
		/** Return the difference in layers of the first hits of the reco track 
		 * @return -1 if reco tracks are not set*/
		int trackDLayer(SvtxClusterMap* cmap,SvtxHitMap* hitMap);
		///@return the first layer the associated reco clusters hit
		int firstLayer(SvtxClusterMap* cmap);
		///@return true if there are any silicon hits for the conversion
		bool hasSilicon(SvtxClusterMap* cmap);
		/** distance between two closest points on the reco tracks 
		 * @return -1 if tracks are not set*/
		double approachDistance()const;

		double dist(PHG4VtxPoint* vtx, SvtxClusterMap* cmap);
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
