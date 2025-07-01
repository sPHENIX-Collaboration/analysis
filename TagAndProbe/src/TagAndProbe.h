#ifndef TAGANDPROBE_TAGANDPROBE_H
#define TAGANDPROBE_TAGANDPROBE_H

// sPHENIX stuff
#include <fun4all/SubsysReco.h>

#include <algorithm>  // for max
#include <memory>     // for allocator_traits<>::valu...
#include <string>
#include <utility>  // for pair
#include <vector>   // for vector

#include <trackbase/ActsTrackingGeometry.h>
#include <globalvertex/SvtxVertexMap.h>

//#include <KFParticle.h>
#include <trackbase_historic/SvtxTrack.h>

#include <trackreco/ActsPropagator.h>
#include <Eigen/Dense>

#include <Acts/Definitions/Algebra.hpp>

#include <Acts/EventData/TrackParameters.hpp>
#include <Acts/Surfaces/CylinderSurface.hpp>
#include <Acts/Utilities/Result.hpp>

class PHCompositeNode;
class TFile;
class TTree;

class TagAndProbe : public SubsysReco
{
 public:
  TagAndProbe();

  explicit TagAndProbe(const std::string &name, const float run, const float seg);

  ~TagAndProbe() override = default;

  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  int End(PHCompositeNode *topNode) override;
  
  void initializeBranches();

  void clearValues();

  void set_nTPC_cut (int nClus_tag, int nClus_passprobe)
  {
    m_nTPC_tag = nClus_tag; 
    m_nTPC_passprobe = nClus_passprobe; 
  }
  
  void set_nINTT_cut (int nClus_tag, int nClus_passprobe)
  {
    m_nINTT_tag = nClus_tag; 
    m_nINTT_passprobe = nClus_passprobe; 
  }
  
  void set_nMVTX_cut (int nClus_tag, int nClus_passprobe)
  {
    m_nMVTX_tag = nClus_tag; 
    m_nMVTX_passprobe = nClus_passprobe; 
  }

  void set_nTPOT_cut (int nClus_tag, int nClus_passprobe)
  {
    m_nTPOT_tag = nClus_tag; 
    m_nTPOT_passprobe = nClus_passprobe; 
  }
  
  void set_quality_cut (float quality_tag, float quality_passprobe)
  {
    m_quality_tag = quality_tag; 
    m_quality_passprobe = quality_passprobe; 
  } 
  
  void set_include_PV_info (bool include = false)
  {
    m_include_pv_info = include; 
  } 

  void saveOutput(bool save = true) { m_save_output = save; }

  void setOutputName(const std::string &name) { m_outfile_name = name; }
 
 private:
  void findPcaTwoTracks(const Acts::Vector3& pos1, const Acts::Vector3& pos2, Acts::Vector3 mom1, Acts::Vector3 mom2, Acts::Vector3& pca1, Acts::Vector3& pca2, double& dca);

  bool projectTrackToPoint(SvtxTrack* track, Eigen::Vector3d PCA, Eigen::Vector3d& pos, Eigen::Vector3d& mom);  
  
  //void getField();

  //KFParticle makeParticle(SvtxTrack* track_kfp); 

  bool m_save_output;
  std::string m_outfile_name;
  TFile *m_outfile;
  
  float m_run;
  float m_segment;
  float m_event;

  float m_trackID_1, m_trackID_2;
  float m_crossing_1, m_crossing_2;
  float m_px_1, m_px_2;  
  float m_py_1, m_py_2;  
  float m_pz_1, m_pz_2;  
  float m_pt_1, m_pt_2;  
  float m_px_proj_1, m_px_proj_2;  
  float m_py_proj_1, m_py_proj_2;  
  float m_pz_proj_1, m_pz_proj_2;  
  //float m_px_proj_kfp_1, m_px_proj_kfp_2;  
  //float m_py_proj_kfp_1, m_py_proj_kfp_2;  
  //float m_pz_proj_kfp_1, m_pz_proj_kfp_2;  
  float m_x_proj_1, m_x_proj_2;  
  float m_y_proj_1, m_y_proj_2;  
  float m_z_proj_1, m_z_proj_2;  
  float m_eta_1, m_eta_2;  
  float m_phi_1, m_phi_2;  
  float m_charge_1, m_charge_2;  
  float m_quality_1, m_quality_2;
  float m_chisq_1, m_chisq_2;
  float m_ndf_1, m_ndf_2;
  float m_nhits_1, m_nhits_2;
  float m_nlayers_1, m_nlayers_2;
  float m_nmaps_1, m_nmaps_2;
  float m_nintt_1, m_nintt_2;
  float m_ntpc_1, m_ntpc_2;
  float m_nmms_1, m_nmms_2;
  float m_mapsstates_1, m_mapsstates_2;
  float m_inttstates_1, m_inttstates_2;
  float m_tpcstates_1, m_tpcstates_2;
  float m_mmsstates_1, m_mmsstates_2;
  float m_pca_x_1, m_pca_x_2;  
  float m_pca_y_1, m_pca_y_2;  
  float m_pca_z_1, m_pca_z_2;  
  float m_tagpass_1, m_tagpass_2;  
  float m_probepass_1, m_probepass_2;  

  float m_invM;
  //float m_invM_kfp;
  float m_dca;
  float m_vx;
  float m_vy;
  float m_vz;
   
  float m_nTPC_tag = 0;
  float m_nTPC_passprobe = 0; 
  float m_nMVTX_tag = 0;
  float m_nMVTX_passprobe = 0; 
  float m_nINTT_tag = 0;
  float m_nINTT_passprobe = 0; 
  float m_nTPOT_tag = 0;
  float m_nTPOT_passprobe = 0; 
  float m_quality_tag = FLT_MAX;
  float m_quality_passprobe = FLT_MAX; 

  bool m_include_pv_info;
  
  ActsGeometry* m_tGeometry = nullptr;
  SvtxVertexMap* m_vertexMap = nullptr;

  unsigned int _nlayers_maps = 3;
  unsigned int _nlayers_intt = 4;
  unsigned int _nlayers_tpc = 48;
  unsigned int _nlayers_mms = 2;

  float _pionMass = 0.13957; //GeV/c^2
 
  TTree *m_cutInfoTree {nullptr};
  TTree *m_TAPTree {nullptr};

  std::string m_magField = "FIELDMAP_TRACKING";
  std::string m_trk_map_node_name = "SvtxTrackMap";
  std::string m_vtx_map_node_name = "SvtxVertexMap";
};

#endif  // TAGANDPROBE_TAGANDPROBE_H
