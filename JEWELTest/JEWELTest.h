#ifndef __JEWELTEST_H__
#define __JEWELTEST_H__


#include <fun4all/SubsysReco.h>
#include <vector>

#include <TTree.h>
#include <TMath.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>

class PHCompositeNode;
class RawClusterContainer;
class RawCluster;
class RawTower;
class RawTowerContainer;
class RawTowerGeomContainer;
class SvtxTrackMap; 
class JetMap;
class JetEvalStack;
class JetRecoEval;
// class PHHepMCGenEvent;

class JEWELTest: public SubsysReco
{

 public:
  
  JEWELTest(const std::string &name="jeweltest.root");
  int Init(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);
 private:
  

  void Set_Tree_Branches();
 
  
  TFile *file;
  TTree *tree;
  TTree *cluster_tree;
  TTree *truth_g4particles;
  /* TTree *tracktree;  */
  TTree *truthjettree;
  TTree *recojettree;
  TTree *scattcenter;
  TTree *finalstate;
  std::string outfilename;

  int nevents;
  TH1F *histo;

  //cluster info
  float clus_energy;
  float clus_eta;
  float clus_phi;
  float clus_pt;
  float clus_px;
  float clus_py;
  float clus_pz;
  float clus_theta;
  float clus_x,clus_y,clus_z,clus_t;

  //track info 
  /* float tr_px,tr_py,tr_pz;  */
  /* float tr_p;  */
  /* float tr_pt;  */
  /* float tr_phi;  */
  /* float tr_eta;  */
  /* int charge;  */
  /* float chisq;  */
  /* unsigned int ndf;  */
  /* float dca;  */
  /* float tr_x,tr_y,tr_z;  */
  /* float truthtrackpx,truthtrackpy,truthtrackpz;  */
  /* float truthtrackp;  */
  /* float truthtracke;  */
  /* float truthtrackpt;  */
  /* float truthtrackphi;  */
  /* float truthtracketa;  */
  /* int truthtrackpid;  */
  /* bool truth_is_primary;  */

  //all truth jets
  float truthjetpt;
  float truthjetpx,truthjetpy,truthjetpz;
  float truthjetphi;
  float truthjeteta;
  float truthjetmass;
  float truthjetp;
  float truthjetenergy;
  float truthjet_bkgsub_pt;
  float truthjet_bkgsub_px,truthjet_bkgsub_py,truthjet_bkgsub_pz;
  float truthjet_bkgsub_phi;
  float truthjet_bkgsub_eta;
  float truthjet_bkgsub_mass;
  float truthjet_bkgsub_p;
  float truthjet_bkgsub_energy;

  //all reco jets
  float recojetpt;
  float recojetpx,recojetpy,recojetpz;
  float recojetphi;
  float recojeteta;
  float recojetmass;
  float recojetp;
  float recojetenergy;
  float recojetid;
  float truthjetid;
  float recojet_bkgsub_pt;
  float recojet_bkgsub_px,recojet_bkgsub_py,recojet_bkgsub_pz;
  float recojet_bkgsub_phi;
  float recojet_bkgsub_eta;
  float recojet_bkgsub_mass;
  float recojet_bkgsub_p;
  float recojet_bkgsub_energy;

  //all truth particles
  float truthpx,truthpy,truthpz;
  float truthp;
  float truthphi;
  float trutheta;
  float truthpt;
  float truthenergy;
  int truthpid;

  //all scatteringcenter particles
  float scattcenterpx, scattcenterpy, scattcenterpz;
  float scattcenterp;
  float scattcenterphi;
  float scattcentereta;
  float scattcenterpt;
  float scattcenterenergy;

  //all finalstate particles
  float finalstatepx, finalstatepy, finalstatepz;
  float finalstatep;
  float finalstatephi;
  float finalstateeta;
  float finalstatept;
  float finalstateenergy;
  
  const float pi = 3.1415926;

};

#endif // __JEWELTEST_H__
