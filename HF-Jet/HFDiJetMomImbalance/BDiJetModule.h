/*!
 *  \file		BDiJetModule.h
 *  \brief		Gether information from Truth Jet and Tracks, DST -> NTuple
 *  \details	Gether information from Truth Jet and Tracks, DST -> NTuple
 *  \author		Dennis V. Perepelitsa
 */

#ifndef __BDiJetModule_H__
#define __BDiJetModule_H__

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
// #include <GenFit/GFRaveVertex.h>
// #include <GenFit/Track.h>
#include <string>
#include <vector>

#include "TMath.h"


namespace PHGenFit {
  class Track;
} /* namespace PHGenFit */

namespace genfit {
  class GFRaveVertexFactory;
  class Track;
  class GFRaveVertex;
} /* namespace genfit */

namespace PHGenFit {
  class Fitter;
} /* namespace PHGenFit */

class SvtxTrack;
class SvtxTrackMap;
class SvtxVertexMap;
class SvtxVertex;
class PHCompositeNode;
class PHG4TruthInfoContainer;
class SvtxClusterMap;
class SvtxEvalStack;
class JetMap;

class TFile;
class TTree;

class BDiJetModule: public SubsysReco {

public:

	BDiJetModule(const std::string &name = "BDiJetModule",
               const std::string &ofName = "out.root");

  int Init(PHCompositeNode*);
	int InitRun(PHCompositeNode*);
	int process_event(PHCompositeNode*);
	int End(PHCompositeNode*);

private:

  //-- Functions
  void ResetVariables();             //! Reset TTree variables
  int GetNodes(PHCompositeNode *);   //! Get all nodes
  void InitTree();                   //! Initialize the TTree

  PHGenFit::Track* MakeGenFitTrack(PHCompositeNode *, 
                                   const SvtxTrack* intrack, 
                                   const SvtxVertex* vertex);

  //! Fill SvtxVertexMap from GFRaveVertexes and Tracks
  void FillVertexMap(
      const std::vector<genfit::GFRaveVertex*> & rave_vertices,
      const std::vector<genfit::Track*> & gf_tracks);

  int GetSVMass_mom(
      const genfit::GFRaveVertex* rave_vtx,
      float & vtx_mass,
      float & vtx_px,
      float & vtx_py,
      float & vtx_pz);


  //-- Nodes
  JetMap* _jetmap_truth;             //! Truth jet information  
  SvtxClusterMap* _clustermap;
  SvtxTrackMap* _trackmap;
  SvtxVertexMap* _vertexmap;

  //-- Flags
  // TODO: Make setters & getters
  bool _verbose;                     //! Verbose printing
  bool _write_tree;
  bool _ana_truth;                   //! Analyze truth info
  bool _ana_reco;                    //! Analyze reconstructed info
  bool _do_evt_display;              //! Event display for GenFit
  bool _use_ladder_geom;
  bool _cut_jet;
  bool _cut_Ncluster;

  //-- Output
  std::string _foutname;	
  TFile *_f;
	TTree *_tree;

  //-- Tree Variables
	int _b_event;                      //! Event counter

	int _truthjet_n;                   //! Number of truth jets
	int _truthjet_parton_flavor[10];   //! Truth jet parton flavor
	int _truthjet_hadron_flavor[10];   //! Truth jet hadron flavor
	float _truthjet_pt[10];            //! Truth jet pT
	float _truthjet_eta[10];           //! Truth jet eta
	float _truthjet_phi[10];           //! Truth jet phi

  float gf_prim_vtx[3];
  float gf_prim_vtx_err[3];
  int gf_prim_vtx_ntrk;
  float rv_prim_vtx[3];
  float rv_prim_vtx_err[3];
  int rv_prim_vtx_ntrk;

  int rv_sv_njets;
  int rv_sv_jet_id[10];
  int rv_sv_jet_prop[10][2];
  float rv_sv_jet_pT[10];
  float rv_sv_jet_phi[10];

  int rv_sv_pT00_nvtx[10];
  float rv_sv_pT00_vtx_x[10][30];
  float rv_sv_pT00_vtx_y[10][30];
  float rv_sv_pT00_vtx_z[10][30];
  float rv_sv_pT00_vtx_ex[10][30];
  float rv_sv_pT00_vtx_ey[10][30];
  float rv_sv_pT00_vtx_ez[10][30];
  int rv_sv_pT00_vtx_ntrk[10][30];
  int rv_sv_pT00_vtx_ntrk_good[10][30];
  float rv_sv_pT00_vtx_mass[10][30];
  float rv_sv_pT00_vtx_mass_corr[10][30];
  float rv_sv_pT00_vtx_pT[10][30];


  //-- internal variables
  int _ievent;

  int _primary_pid_guess;            //! Particle ID guess for GenFit

  double _cut_min_pT;                //! Cut on min track pT
  double _cut_chi2_ndf;              //! Cut on max track chi2
  double _cut_jet_pT;                //! Cut on min jet pT
  double _cut_jet_eta;               //! Cut on jet eta
  double _cut_jet_R;                 //! Cut on jet R

  std::string _track_fitting_alg_name;
  PHGenFit::Fitter* _fitter;

  //! https://rave.hepforge.org/trac/wiki/RaveMethods
  std::string _vertexing_method;
  genfit::GFRaveVertexFactory* _vertex_finder;

};

#endif // __BDiJetModule_H__
