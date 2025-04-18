#ifndef __FORWARD_PI0S_H__
#define __FORWARD_PI0S_H__

#include <fun4all/SubsysReco.h>
#include <vector>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TTree.h>

class PHCompositeNode;
class RawClusterContainer;
class RawCluster;
class SvtxTrackMap;
class JetMap;
class JetEvalStack;
class JetRecoEval;
class SvtxTrackEval;
class PHG4TruthInfoContainer;
class PHHepMCGenEvent;
class Forward_pi0s : public SubsysReco
{
 public:
  Forward_pi0s(const std::string &name = "pi0_eval.root");
  double isoconeradius, mincluspt;
  float jet_cone_size;
  float _etalow, _etahi;
  int _useforwardarm;
  int use_isocone;

  int Init(PHCompositeNode *);
  int process_event(PHCompositeNode *);
  int End(PHCompositeNode *);

  void set_cluspt_mincut(double pt) { mincluspt = pt; };
  void SetFirstEventNum(int eventnum) { nevents = eventnum; };
  void set_eta_lowhigh(float etalow, float etahi)
  {
    _etalow = etalow;
    _etahi = etahi;
  };
  void useforwardarm(int yesorno) { _useforwardarm = yesorno;};

 private:
  void Set_Tree_Branches();
  void initialize_things();

  TFile *file;
  TTree *tree;
  TTree *cluster_tree;
  TTree *truth_g4particles;
  TTree *inhcal_tree;
  TTree *fcluster_tree;

  std::string outfilename;

  int nevents;
  TH1F *histo;

  //hcalcluster info
  float hcal_energy;
  float hcal_eta;
  float hcal_phi;
  float hcal_pt;
  float hcal_px;
  float hcal_py;
  float hcal_pz;
  float hcal_theta;
  float hcal_x, hcal_y, hcal_z, hcal_t;

  //cluster info
  float clus_energy;
  float clus_eta;
  float clus_phi;
  float clus_pt;
  float clus_px;
  float clus_py;
  float clus_pz;
  float clus_energy2;
  float clus_eta2;
  float clus_phi2;
  float clus_pt2;
  float clus_px2;
  float clus_py2;
  float clus_pz2;
  float clus_theta2;
  float cent_invmass;
  float clus_theta;
  float clus_x, clus_y, clus_z, clus_t;

  float fclusenergy2;
  float fclus_eta2;
  float fclus_phi2;
  float fclus_theta2;
  float fclus_pt2;
  float fclus_px2;
  float fclus_py2;
  float fclus_pz2;
  float invmass;

  //all truth particles
  float truthpx, truthpy, truthpz;
  float truthp;
  float truthphi;
  float trutheta;
  float truthpt;
  float truthenergy;
  int truthpid;
  int numparticlesinevent;
  int process_id;

  //clust truth variables
  float clustruthpx;
  float clustruthpy;
  float clustruthpz;
  float clustruthenergy;
  float clustruthpt;
  float clustruthphi;
  float clustrutheta;
  int clustruthpid;



  float hclustruthpx;
  float hclustruthpy;
  float hclustruthpz;
  float hclustruthenergy;
  float hclustruthpt;
  float hclustruthphi;
  float hclustrutheta;
  int hclustruthpid;

  //forward clusters
  float fclusenergy;
  float fclus_eta;
  float fclus_phi;
  float fclus_theta;
  float fclus_pt;
  int fclustruthpid;
  float fclustruthpx;
  float fclustruthpy;
  float fclustruthpz;
  float fclustruthenergy;
  float fclustruthpt;
  float fclustruthphi;
  float fclustrutheta;
  float fclus_px;
  float fclus_py;
  float fclus_pz;

  float tpi0e;
  float tpi0px;
  float tpi0py;
  float tpi0pz;
  float tpi0pid;
  float tpi0pt;
  float tpi0phi;
  float tpi0eta;

  float tphote1;
  float tphotpx1;
  float tphotpy1;
  float tphotpz1;
  int tphotpid1;
  int tphotparentid1;
  float tphotpt1;
  float tphotphi1;
  float tphoteta1;

  float tphote2;
  float tphotpx2;
  float tphotpy2;
  float tphotpz2;
  int tphotpid2;
  int tphotparentid2;
  float tphotpt2;
  float tphotphi2;
  float tphoteta2;

  const float pi2 = -1.5707963;
  const float threepi2 = 4.71238898;
  const float pi = 3.1415926;
};

#endif  // __FORWARD_PI0S_H__
