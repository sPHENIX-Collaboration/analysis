#ifndef CONVERTEDPHOTONRECONSTRUCTOR_H__
#define CONVERTEDPHOTONRECONSTRUCTOR_H__

//===============================================
/// \file CONVERTEDPHOTONRECONSTRUCTOR.h
/// \brief Reconstructes electron positron tracks into photons
/// \author Francesco Vassalli
//===============================================


#include <fun4all/SubsysReco.h>
#include <trackbase_historic/SvtxTrack_v1.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TTree.h>
#include <TFile.h>
#include <iostream>
#include <string>
#include <vector>
class PHCompositeNode;
class SvtxEvalStack;
class SvtxClusterMap;

class ReconstructedConvertedPhoton
{
  public:
    ReconstructedConvertedPhoton(int event, const TLorentzVector& reco,const TVector3& recoVert,const TLorentzVector& truth, const TVector3& truthVert): event(event){
      recovec    =reco;
      truthvec   = truth;
      truthVertex= truthVert;
      recoVertex = recoVert;
      positron=nullptr;
      electron=nullptr;
    }
    ReconstructedConvertedPhoton(int event, const TLorentzVector& reco,const TVector3& recoVert,const TLorentzVector& truth, const TVector3& truthVert,SvtxTrack* ptrack,SvtxTrack* etrack,SvtxClusterMap* clustermap)
      : event(event){
        positron = dynamic_cast<SvtxTrack_v1*>(ptrack->Clone());
        electron = dynamic_cast<SvtxTrack_v1*>(etrack->Clone());
        if(!positron||!electron){
          std::cout<<"Null tracks in photon contructor dropping tracks"<<std::endl;
          *this = ReconstructedConvertedPhoton(event,reco,recoVert,truth,truthVert); 
        }
        else{
          recovec    =reco;
          truthvec   = truth;
          truthVertex= truthVert;
          recoVertex = recoVert;
          subtracted = false;
         // removeTracks(clustermap);
        }
      }

    ~ReconstructedConvertedPhoton(){
      if(positron) delete positron;
      if(positron) delete electron;
    }

    void setPositron(SvtxTrack_v1* track){positron=static_cast<SvtxTrack_v1*>(track->Clone());}
    void setElectron(SvtxTrack_v1* track){electron=static_cast<SvtxTrack_v1*>(track->Clone());}
    inline SvtxTrack_v1* get_positron() const{return positron;}
    inline SvtxTrack_v1* get_electron() const{return electron;}

    inline friend std::ostream& operator<<(std::ostream& os, ReconstructedConvertedPhoton const & tc) {
      return os <<"Converted Photon: \n \t pvec:" << tc.recovec.Pt()
        <<"\n \t truth pvec:"<<tc.truthvec.Pt()<<'\n';
    }
  private:
    int event;
    bool subtracted;
    TLorentzVector recovec;
    TLorentzVector truthvec;
    TVector3 truthVertex;
    TVector3 recoVertex;
    //maybe an std::pair would be better 
    SvtxTrack_v1* positron;
    SvtxTrack_v1* electron;
};

class ConvertedPhotonReconstructor : public SubsysReco {

  public:

    ConvertedPhotonReconstructor(const std::string &name);
    ~ConvertedPhotonReconstructor();	
    int Init(PHCompositeNode *topNode);
    int InitRun(PHCompositeNode *topNode);
    int process_event(PHCompositeNode *topNode);
    int End(PHCompositeNode *topNode);
    //std::vector<ReconstructedConvertedPhoton> getPhotons() const {return reconstructedConvertedPhotons;}//i removed this as a class member but I may re add it.  It is declared in the reconstruct method

  protected:
    const float kEmass = 0.000511;
    const unsigned int n_intt_layer = 4;  
    const unsigned int n_maps_layer = 3;  // must be 0-3, setting it to zero removes MVTX completely, n < 3 gives the first n layers
  private:
    int event;
		bool b_goodCharge=true;
		bool b_hasSilicone=false;
    std::string hash;
    std::string name;
    TFile *_file;
    TTree *_tree;
    std::vector<ReconstructedConvertedPhoton*> recoveredPhotonVec;
    TLorentzVector *b_recovec1;
    TLorentzVector *b_truthvec1;
    TLorentzVector *b_recovec2;
    TLorentzVector *b_truthvec2;
    TVector3       *b_truthVertex;
    TVector3       *b_recoVertex;

    ReconstructedConvertedPhoton* reconstruct(PHCompositeNode *topNode);
    inline float pToE(TVector3 v, float mass){
      return quadrature((float) quadrature(v.x(),v.y()),(float) quadrature((float)v.z(),mass));
    }
    void process_recoTracks(PHCompositeNode *topNode);
    template<class T>
      T quadrature(T d1, T d2){
        return TMath::Sqrt((double)d1*d1+d2*d2);
      }
};

#endif // __CONVERTEDPHOTONRECONSTRUCTOR_H__
