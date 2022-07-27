/*!
 *  \file		BJetModule.h
 *  \brief		Gether information from Truth Jet and Tracks, DST -> NTuple
 *  \details	Gether information from Truth Jet and Tracks, DST -> NTuple
 *  \author		Dennis V. Perepelitsa
 */

#ifndef __BJETMODULE_H__
#define __BJETMODULE_H__

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <vector>

#include "TMath.h"

#include "TFile.h"
#include "TTree.h"

class PHCompositeNode;
class SvtxVertexMap;
class SvtxTrack;
//class JetEvalStack;
//class SvtxEvalStack;

class BJetModule : public SubsysReco
{
 public:
  BJetModule(const std::string& name = "BJetModule", const std::string& out = "HFtag.root");

  int Init(PHCompositeNode*);
  int reset_tree_vars();
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);

  const std::string& get_trackmap_name() const
  {
    return _trackmap_name;
  }

  void set_trackmap_name(const std::string& trackmapName)
  {
    _trackmap_name = trackmapName;
  }

  const std::string& get_vertexmap_name() const
  {
    return _vertexmap_name;
  }
  void set_truthjetmap_name(const std::string& name)
  { _truthjetmap_name = name; }
  void set_recojetmap_name(const std::string& name)
  { _recojetmap_name = name; }
  void set_vertexmap_name(const std::string& vertexmapName)
  {
    _vertexmap_name = vertexmapName;
  }

 private:
  float dR(float eta1, float eta2, float phi1, float phi2)
  {
    float deta = eta1 - eta2;
    float dphi = phi1 - phi2;
    if (dphi > +3.14159)
      dphi -= 2 * 3.14159;
    if (dphi < -3.14159)
      dphi += 2 * 3.14159;

    return sqrt(pow(deta, 2) + pow(dphi, 2));
  }
 
  void calc3DDCA(SvtxTrack* track, SvtxVertexMap* vertexmap,
		 float& dca3d_xy, float& dca3d_xy_error,
		 float& dca3d_z, float& dca3d_z_error);
 
  void setBranches();
  std::string _foutname;
  
  std::string _truthjetmap_name;
  std::string _recojetmap_name;

  std::string _trackmap_name;
  std::string _vertexmap_name;

  int _ievent;

  TFile* _f;
  TTree* _tree;

  int _b_event;

  int _b_truth_vertex_n;
  float _b_truth_vertex_x[10];
  float _b_truth_vertex_y[10];
  float _b_truth_vertex_z[10];

  int _b_truthjet_n;
  int _b_truthjet_parton_flavor[10];
  int _b_truthjet_hadron_flavor[10];

  float _b_truthjet_pt[10];
  float _b_truthjet_eta[10];
  float _b_truthjet_phi[10];

  int _b_recojet_valid[10];
  float _b_recojet_pt[10];
  float _b_recojet_eta[10];
  float _b_recojet_phi[10];

  int _b_particle_n;
  float _b_particle_pt[1000];
  float _b_particle_eta[1000];
  float _b_particle_phi[1000];
  int _b_particle_pid[1000];
  unsigned int _b_particle_embed[1000];

  float _b_particle_vertex_x[1000];
  float _b_particle_vertex_y[1000];
  float _b_particle_vertex_z[1000];
  float _b_particle_dca_xy[1000];
  float _b_particle_dca_z[1000];

  int _b_track_n;
  float _b_track_pt[1000];
  float _b_track_eta[1000];
  float _b_track_phi[1000];

  float _b_track_dca2d[1000];
  float _b_track_dca2d_error[1000];

  float _b_track_dca3d_xy[1000];
  float _b_track_dca3d_xy_error[1000];

  float _b_track_dca3d_z[1000];
  float _b_track_dca3d_z_error[1000];

  float _b_track_dca2d_calc[1000];
  float _b_track_dca2d_calc_truth[1000];
  float _b_track_dca3d_calc[1000];
  float _b_track_dca3d_calc_truth[1000];

  float _b_track_pca_phi[1000];
  float _b_track_pca_x[1000];
  float _b_track_pca_y[1000];
  float _b_track_pca_z[1000];

  float _b_track_quality[1000];
  float _b_track_chisq[1000];
  int _b_track_ndf[1000];

  int _b_track_nmaps[1000];

  unsigned int _b_track_nclusters[1000];
  unsigned int _b_track_nclusters_by_layer[1000];
  unsigned int _b_track_best_nclusters[1000];
  unsigned int _b_track_best_nclusters_by_layer[1000];

  unsigned int _b_track_best_embed[1000];
  bool _b_track_best_primary[1000];
  int _b_track_best_pid[1000];
  float _b_track_best_pt[1000];

  int _b_track_best_in[1000];
  int _b_track_best_out[1000];
  int _b_track_best_parent_pid[1000];

  float _b_track_best_vertex_x[1000];
  float _b_track_best_vertex_y[1000];
  float _b_track_best_vertex_z[1000];
  float _b_track_best_dca_xy[1000];
  float _b_track_best_dca_z[1000];

  //! The embedding ID for the HepMC subevent to be analyzed.
  //! positive ID is the embedded event of interest, e.g. jetty event from pythia
  //! negative IDs are backgrounds, .e.g out of time pile up collisions
  //! Usually, ID = 0 means the primary Au+Au collision background
  int _embedding_id;
};

#endif  // __BJETMODULE_H__
