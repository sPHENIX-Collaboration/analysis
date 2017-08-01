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
/* class SvtxTrackMap; */
class JetMap;
class JetEvalStack;
class JetRecoEval;

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
  /* TTree *tracktree; */
  TTree *truthjettree;
  TTree *recojettree;
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

  /* //track info */
  /* float tr_px,tr_py,tr_pz; */
  /* float tr_p; */
  /* float tr_pt; */
  /* float tr_phi; */
  /* float tr_eta; */
  /* int charge; */
  /* float chisq; */
  /* unsigned int ndf; */
  /* float dca; */
  /* float tr_x,tr_y,tr_z; */
  /* float truthtrackpx,truthtrackpy,truthtrackpz; */
  /* float truthtrackp; */
  /* float truthtracke; */
  /* float truthtrackpt; */
  /* float truthtrackphi; */
  /* float truthtracketa; */
  /* int truthtrackpid; */
  /* bool truth_is_primary; */

  //all truth jets
  float truthjetpt;
  float truthjetpx,truthjetpy,truthjetpz;
  float truthjetphi;
  float truthjeteta;
  float truthjetmass;
  float truthjetp;
  float truthjetenergy;

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


  //all truth particles
  float truthpx,truthpy,truthpz;
  float truthp;
  float truthphi;
  float trutheta;
  float truthpt;
  float truthenergy;
  int truthpid;


  const float pi = 3.1415926;

};

#endif // __JEWELTEST_H__
