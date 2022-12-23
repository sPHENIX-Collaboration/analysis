#ifndef MASSRECOANALYSIS_H
#define MASSRECOANALYSIS_H

#include <fun4all/SubsysReco.h>
#include <trackbase/ActsTrackingGeometry.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertexMap.h>

#include <TFile.h>
#include <TH1D.h>
#include <TNtuple.h>


class PHCompositeNode;
class SvtxTrack;
class SvtxTrackMap;
class SvtxVertexMap;

class massRecoAnalysis : public SubsysReco
{
 public:
  massRecoAnalysis(const std::string &name = "massRecoAnalysis");
  virtual ~massRecoAnalysis(){}

  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int End(PHCompositeNode * /*topNode*/) override;

  void setTrackQualityCut(double cut) {_qual_cut = cut;}
  void setPairDCACut(double cut){pair_dca_cut = cut;}
  void setTrackDCACut(double cut){track_dca_cut = cut;}
  void setRequireMVTX(bool set) {_require_mvtx = set;}
  void setNmvtxRequired(unsigned int n) {_nmvtx_required = n;}
  void setDecayMass(Float_t decayMassSet){decaymass = decayMassSet;}  //(muons decaymass = 0.1057) (pions = 0.13957) (electron = 0.000511)
  void set_output_number(int proc){process=proc;}

 private:  

  void fillNtp(SvtxTrack *track1, SvtxTrack *track2, Acts::Vector3 dcavals1, Acts::Vector3 dcavals2, 
	       Acts::Vector3 pca_rel1, Acts::Vector3 pca_rel2, double pair_dca, double invariantMass);
  void fillHistogram(SvtxTrack *track1, SvtxTrack *track2, TH1D *massreco, double& invariantMass);
  void findPcaTwoTracks(SvtxTrack *track1, SvtxTrack *track2, Acts::Vector3& pca1, Acts::Vector3& pca2, double& dca);
  int getNodes(PHCompositeNode *topNode);
  Acts::Vector3 calculateDca(SvtxTrack *track, Acts::Vector3 momentum, Acts::Vector3 position);

  TNtuple *ntp_reco_info;
  SvtxTrackMap *m_svtxTrackMap = nullptr; 
  SvtxVertexMap *m_vertexMap   = nullptr;
  
  int process                  = 0;
  Float_t decaymass            = 0.13957;  // pion decay mass
  double _qual_cut             = 5.0;
  bool _require_mvtx           = true;
  unsigned int _nmvtx_required = 3; 
  double pair_dca_cut          = 0.05; // kshort relative cut 500 microns
  double track_dca_cut         = 0.01;
  TFile *fout;
  TH1D* recomass;
};

#endif  // MASSRECOANALYSIS_H 
