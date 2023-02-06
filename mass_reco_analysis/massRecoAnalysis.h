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


  void setPtCut(double ptcut) {invariant_pt_cut = ptcut;}
  void setTrackQualityCut(double cut) {_qual_cut = cut;}
  void setPairDCACut(double cut){pair_dca_cut = cut;}
  void setTrackDCACut(double cut){track_dca_cut = cut;}
  void setRequireMVTX(bool set) {_require_mvtx = set;}
  void setDecayMass(Float_t decayMassSet){decaymass = decayMassSet;}  //(muons decaymass = 0.1057) (pions = 0.13957) (electron = 0.000511)
  void set_output_number(int proc){process=proc;}

 private:  

  void fillNtp(SvtxTrack *track1, SvtxTrack *track2, Acts::Vector3 dcavals1, Acts::Vector3 dcavals2, 
	       Acts::Vector3 pca_rel1, Acts::Vector3 pca_rel2, double pair_dca, double invariantMass, double invariantPt, float rapidity, float pseudorapidity);
  void fillHistogram(SvtxTrack *track1, SvtxTrack *track2, TH1D *massreco, double& invariantMass, double& invariantPt, float& rapidity, float& pseudorapidity);
  void findPcaTwoTracks(SvtxTrack *track1, SvtxTrack *track2, Acts::Vector3& pca1, Acts::Vector3& pca2, double& dca);
  int getNodes(PHCompositeNode *topNode);
  Acts::Vector3 calculateDca(SvtxTrack *track, Acts::Vector3 momentum, Acts::Vector3 position);

  bool massRecoAnalysis::projectTrackToCylinder(SvtxTrack* track, double Radius, Eigen::Vector3d& pos, Eigen::Vector3d& mom);
  BoundTrackParamResult massRecoAnalysis::propagateTrack(const Acts::BoundTrackParameters& params, const SurfacePtr& targetSurf);
  Acts::BoundTrackParameters massRecoAnalysis::makeTrackParams(SvtxTrack* track);



  TNtuple *ntp_reco_info;
  SvtxTrackMap *m_svtxTrackMap = nullptr; 
  SvtxVertexMap *m_vertexMap   = nullptr;
  
  int process                  = 0;
  Float_t decaymass            = 0.13957;  // pion decay mass
  bool _require_mvtx           = true;
  double _qual_cut             = 5.0;
  double pair_dca_cut          = 0.05; // kshort relative cut 500 microns
  double track_dca_cut         = 0.01;
  double invariant_pt_cut                = 0.1;
  TFile *fout;
  TH1D* recomass;
};

#endif  // MASSRECOANALYSIS_H 
