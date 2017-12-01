#ifndef __JETPERFORMANCE_H__
#define __JETPERFORMANCE_H__


#include <fun4all/SubsysReco.h>
#include <vector>

#include <TTree.h>
#include <TMath.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>

/* static const double ptbins[] = {0.0, 2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0, */
/* 				20.0, 22.0, 24.0, 26.0, 28.0, 30.0, 32.0, 34.0, 36.0, */
/* 				38.0, 40.0, 42.0, 44.0, 46.0, 48.0, 50.0, 52.0, 54.0, */
/* 				56.0, 58.0, 60.0, 62.0, 64.0, 66.0, 68.0, 70.0, 72.0, */
/* 				74.0, 76.0, 78.0, 80.0, 82.0, 84.0, 86.0, 88.0, 90.0, */
/* 				92.0, 94.0, 96.0, 98.0, 100.0}; */
/* static const int ptbins_int[] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, */
/* 				 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, */
/* 				 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, */
/* 				 80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100}; */
static const double ptbins[] = {10.0, 15.0, 25.0, 35.0, 60.0};
static const int ptbins_int[] = {10, 15, 25, 35, 60};
static const int nbins_pt = sizeof(ptbins)/sizeof(double) -1;

/* static const double etabins[] = {-1., -0.6, -0.3, 0.3, 0.6, 1.}; */
static const double etabins[] = {-0.6, 0.6};
static const int nbins_eta = sizeof(etabins)/sizeof(double) -1;  


const float pi2 = -1.5707963;
const float threepi2 = 4.71238898;
const float pi = 3.1415926;

class PHCompositeNode;
class Jet;
namespace HepMC
{
  class GenEvent;
  class GenParticle;
}

class PHCompositeNode;
class RawClusterContainer;
class RawCluster;
class SvtxTrackMap;
class JetMap;
class JetEvalStack;
class JetRecoEval;
class JetPerformance: public SubsysReco
{

 public:
  
  JetPerformance(const std::string &name="jetperformance_analysis.root");
  double isoconeradius,mincluspt;
  int Init(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);


 private:

  //! tag jet flavor by parton matching, like PRL 113, 132301 (2014)
  int parton_tagging(Jet * jet, HepMC::GenEvent*, const double match_radius); 
  float parton_frag(Jet * jet, HepMC::GenEvent*, const double match_radius);
  void clearallvectors();
  
  void Set_Tree_Branches();
  std::string outfilename;

  int nevents;
  float weight;
  int signal_processID;
  int mpi;
  float event_scale;
  float alpha_QCD;
  float alpha_QED;
  TH1F *histo; 
  TFile *file;
  TTree *tree;  

  TH1F * hRecoJetpT_R02;
  TH1F * hRecoJeteta_R02;
  TH1F * hRecoJetphi_R02;
  TH1F * hReco_NoHcalIn_JetpT_R02;
  TH1F * hReco_NoHcalIn_Jeteta_R02;
  TH1F * hReco_NoHcalIn_Jetphi_R02;
  TH1F * hReco_NoHcalIn_GenJetdelR_R02;
  TH1F * hGenJetpT_R02;
  TH1F * hGenJeteta_R02;
  TH1F * hGenJetphi_R02;
  TH1F * hRecoGenJetdelR_R02;
  TH1F * hJER_R02[nbins_pt][nbins_eta];
  TH1F * hJER_allpT_allEta_R02;  
  TH1F * hJER_noHcalIn_R02[nbins_pt][nbins_eta];
  TH1F * hJER_noHcalIn_allpT_allEta_R02;  

  TTree *truth_g4particles;
  TTree *truthjettree_R02;
  TTree *recojettree_R02;
  TTree *reco_noHcalIn_jettree_R02;

  TTree *truthjettree_R03;
  TTree *recojettree_R03;
  TTree *reco_noHcalIn_jettree_R03;
  TH1F * hRecoJetpT_R03;
  TH1F * hRecoJeteta_R03;
  TH1F * hRecoJetphi_R03;
  TH1F * hReco_NoHcalIn_JetpT_R03;
  TH1F * hReco_NoHcalIn_Jeteta_R03;
  TH1F * hReco_NoHcalIn_Jetphi_R03;
  TH1F * hReco_NoHcalIn_GenJetdelR_R03;
  TH1F * hGenJetpT_R03;
  TH1F * hGenJeteta_R03;
  TH1F * hGenJetphi_R03;
  TH1F * hRecoGenJetdelR_R03;
  TH1F * hJER_R03[nbins_pt][nbins_eta];
  TH1F * hJER_allpT_allEta_R03;  
  TH1F * hJER_noHcalIn_R03[nbins_pt][nbins_eta];
  TH1F * hJER_noHcalIn_allpT_allEta_R03;  
  
  TTree *truthjettree_R04;
  TTree *recojettree_R04;
  TTree *reco_noHcalIn_jettree_R04;
  TH1F * hRecoJetpT_R04;
  TH1F * hRecoJeteta_R04;
  TH1F * hRecoJetphi_R04;
  TH1F * hReco_NoHcalIn_JetpT_R04;
  TH1F * hReco_NoHcalIn_Jeteta_R04;
  TH1F * hReco_NoHcalIn_Jetphi_R04;
  TH1F * hReco_NoHcalIn_GenJetdelR_R04;
  TH1F * hGenJetpT_R04;
  TH1F * hGenJeteta_R04;
  TH1F * hGenJetphi_R04;
  TH1F * hRecoGenJetdelR_R04;
  TH1F * hJER_R04[nbins_pt][nbins_eta];
  TH1F * hJER_allpT_allEta_R04;  
  TH1F * hJER_noHcalIn_R04[nbins_pt][nbins_eta];
  TH1F * hJER_noHcalIn_allpT_allEta_R04;  
  

  
  //all truth particles
  std::vector<float> truthpx;
  std::vector<float> truthpy;
  std::vector<float> truthpz;
  std::vector<float> truthp;
  std::vector<float> truthphi;
  std::vector<float> trutheta;
  std::vector<float> truthpt;
  std::vector<float> truthenergy;
  std::vector<int> truthpid;  

  //all truth jets
  std::vector<float> truthjetpt_R02;
  std::vector<float> truthjetpx_R02;
  std::vector<float> truthjetpy_R02;
  std::vector<float> truthjetpz_R02;
  std::vector<float> truthjetphi_R02;
  std::vector<float> truthjeteta_R02;
  std::vector<float> truthjetmass_R02;
  std::vector<float> truthjetp_R02;
  std::vector<float> truthjetenergy_R02;

  //all reco jets
  std::vector<float> recojetpt_R02;
  std::vector<float> recojetpx_R02;
  std::vector<float> recojetpy_R02;
  std::vector<float> recojetpz_R02;
  std::vector<float> recojetphi_R02;
  std::vector<float> recojeteta_R02;
  std::vector<float> recojetmass_R02;
  std::vector<float> recojetp_R02;
  std::vector<float> recojetenergy_R02;
  std::vector<float> recojetid_R02;
  std::vector<float> truthjetid_R02;
  std::vector<float> truthjetpt_reco_R02;
  std::vector<float> truthjetpx_reco_R02;
  std::vector<float> truthjetpy_reco_R02;
  std::vector<float> truthjetpz_reco_R02;
  std::vector<float> truthjetphi_reco_R02;
  std::vector<float> truthjeteta_reco_R02;
  std::vector<float> truthjetmass_reco_R02;
  std::vector<float> truthjetp_reco_R02;
  std::vector<float> truthjetenergy_reco_R02;
  std::vector<float> recogenpTratio_R02;
  std::vector<float> delRrecotruth_R02;
  std::vector<int> parton_flavor_R02;
  std::vector<float> parton_frag_zt_R02;
  std::vector<int> recojetsubid_R02;

  //all reco_noHcalIn_ jets
  std::vector<float> reco_noHcalIn_jetpt_R02;
  std::vector<float> reco_noHcalIn_jetpx_R02;
  std::vector<float> reco_noHcalIn_jetpy_R02;
  std::vector<float> reco_noHcalIn_jetpz_R02;
  std::vector<float> reco_noHcalIn_jetphi_R02;
  std::vector<float> reco_noHcalIn_jeteta_R02;
  std::vector<float> reco_noHcalIn_jetmass_R02;
  std::vector<float> reco_noHcalIn_jetp_R02;
  std::vector<float> reco_noHcalIn_jetenergy_R02;
  std::vector<float> reco_noHcalIn_jetid_R02;
  std::vector<float> reco_noHcalIn_truthjetid_R02;
  std::vector<float> truthjetpt_reco_noHcalIn_R02;
  std::vector<float> truthjetpx_reco_noHcalIn_R02;
  std::vector<float> truthjetpy_reco_noHcalIn_R02;
  std::vector<float> truthjetpz_reco_noHcalIn_R02;
  std::vector<float> truthjetphi_reco_noHcalIn_R02;
  std::vector<float> truthjeteta_reco_noHcalIn_R02;
  std::vector<float> truthjetmass_reco_noHcalIn_R02;
  std::vector<float> truthjetp_reco_noHcalIn_R02;
  std::vector<float> truthjetenergy_reco_noHcalIn_R02;
  std::vector<float> reco_noHcalIn_genpTratio_R02;
  std::vector<float> delRreco_noHcalIn_truth_R02;
  std::vector<int> reco_noHcalIn_parton_flavor_R02;
  std::vector<float> reco_noHcalIn_parton_frag_zt_R02;
  std::vector<int> reco_noHcalIn_jetsubid_R02;
  

  //all truth jets
  std::vector<float> truthjetpt_R03;
  std::vector<float> truthjetpx_R03;
  std::vector<float> truthjetpy_R03;
  std::vector<float> truthjetpz_R03;
  std::vector<float> truthjetphi_R03;
  std::vector<float> truthjeteta_R03;
  std::vector<float> truthjetmass_R03;
  std::vector<float> truthjetp_R03;
  std::vector<float> truthjetenergy_R03;

  //all reco jets
  std::vector<float> recojetpt_R03;
  std::vector<float> recojetpx_R03;
  std::vector<float> recojetpy_R03;
  std::vector<float> recojetpz_R03;
  std::vector<float> recojetphi_R03;
  std::vector<float> recojeteta_R03;
  std::vector<float> recojetmass_R03;
  std::vector<float> recojetp_R03;
  std::vector<float> recojetenergy_R03;
  std::vector<float> recojetid_R03;
  std::vector<float> truthjetid_R03;
  std::vector<float> truthjetpt_reco_R03;
  std::vector<float> truthjetpx_reco_R03;
  std::vector<float> truthjetpy_reco_R03;
  std::vector<float> truthjetpz_reco_R03;
  std::vector<float> truthjetphi_reco_R03;
  std::vector<float> truthjeteta_reco_R03;
  std::vector<float> truthjetmass_reco_R03;
  std::vector<float> truthjetp_reco_R03;
  std::vector<float> truthjetenergy_reco_R03;
  std::vector<float> recogenpTratio_R03;
  std::vector<float> delRrecotruth_R03;
  std::vector<int> parton_flavor_R03;
  std::vector<float> parton_frag_zt_R03;
  std::vector<int> recojetsubid_R03;

  //all reco_noHcalIn_ jets
  std::vector<float> reco_noHcalIn_jetpt_R03;
  std::vector<float> reco_noHcalIn_jetpx_R03;
  std::vector<float> reco_noHcalIn_jetpy_R03;
  std::vector<float> reco_noHcalIn_jetpz_R03;
  std::vector<float> reco_noHcalIn_jetphi_R03;
  std::vector<float> reco_noHcalIn_jeteta_R03;
  std::vector<float> reco_noHcalIn_jetmass_R03;
  std::vector<float> reco_noHcalIn_jetp_R03;
  std::vector<float> reco_noHcalIn_jetenergy_R03;
  std::vector<float> reco_noHcalIn_jetid_R03;
  std::vector<float> reco_noHcalIn_truthjetid_R03;
  std::vector<float> truthjetpt_reco_noHcalIn_R03;
  std::vector<float> truthjetpx_reco_noHcalIn_R03;
  std::vector<float> truthjetpy_reco_noHcalIn_R03;
  std::vector<float> truthjetpz_reco_noHcalIn_R03;
  std::vector<float> truthjetphi_reco_noHcalIn_R03;
  std::vector<float> truthjeteta_reco_noHcalIn_R03;
  std::vector<float> truthjetmass_reco_noHcalIn_R03;
  std::vector<float> truthjetp_reco_noHcalIn_R03;
  std::vector<float> truthjetenergy_reco_noHcalIn_R03;
  std::vector<float> reco_noHcalIn_genpTratio_R03;
  std::vector<float> delRreco_noHcalIn_truth_R03;
  std::vector<int> reco_noHcalIn_parton_flavor_R03;
  std::vector<float> reco_noHcalIn_parton_frag_zt_R03;
  std::vector<int> reco_noHcalIn_jetsubid_R03;


  //all truth jets
  std::vector<float> truthjetpt_R04;
  std::vector<float> truthjetpx_R04;
  std::vector<float> truthjetpy_R04;
  std::vector<float> truthjetpz_R04;
  std::vector<float> truthjetphi_R04;
  std::vector<float> truthjeteta_R04;
  std::vector<float> truthjetmass_R04;
  std::vector<float> truthjetp_R04;
  std::vector<float> truthjetenergy_R04;

  //all reco jets
  std::vector<float> recojetpt_R04;
  std::vector<float> recojetpx_R04;
  std::vector<float> recojetpy_R04;
  std::vector<float> recojetpz_R04;
  std::vector<float> recojetphi_R04;
  std::vector<float> recojeteta_R04;
  std::vector<float> recojetmass_R04;
  std::vector<float> recojetp_R04;
  std::vector<float> recojetenergy_R04;
  std::vector<float> recojetid_R04;
  std::vector<float> truthjetid_R04;
  std::vector<float> truthjetpt_reco_R04;
  std::vector<float> truthjetpx_reco_R04;
  std::vector<float> truthjetpy_reco_R04;
  std::vector<float> truthjetpz_reco_R04;
  std::vector<float> truthjetphi_reco_R04;
  std::vector<float> truthjeteta_reco_R04;
  std::vector<float> truthjetmass_reco_R04;
  std::vector<float> truthjetp_reco_R04;
  std::vector<float> truthjetenergy_reco_R04;
  std::vector<float> recogenpTratio_R04;
  std::vector<float> delRrecotruth_R04;
  std::vector<int> parton_flavor_R04;
  std::vector<float> parton_frag_zt_R04;
  std::vector<int> recojetsubid_R04;

  //all reco_noHcalIn_ jets
  std::vector<float> reco_noHcalIn_jetpt_R04;
  std::vector<float> reco_noHcalIn_jetpx_R04;
  std::vector<float> reco_noHcalIn_jetpy_R04;
  std::vector<float> reco_noHcalIn_jetpz_R04;
  std::vector<float> reco_noHcalIn_jetphi_R04;
  std::vector<float> reco_noHcalIn_jeteta_R04;
  std::vector<float> reco_noHcalIn_jetmass_R04;
  std::vector<float> reco_noHcalIn_jetp_R04;
  std::vector<float> reco_noHcalIn_jetenergy_R04;
  std::vector<float> reco_noHcalIn_jetid_R04;
  std::vector<float> reco_noHcalIn_truthjetid_R04;
  std::vector<float> truthjetpt_reco_noHcalIn_R04;
  std::vector<float> truthjetpx_reco_noHcalIn_R04;
  std::vector<float> truthjetpy_reco_noHcalIn_R04;
  std::vector<float> truthjetpz_reco_noHcalIn_R04;
  std::vector<float> truthjetphi_reco_noHcalIn_R04;
  std::vector<float> truthjeteta_reco_noHcalIn_R04;
  std::vector<float> truthjetmass_reco_noHcalIn_R04;
  std::vector<float> truthjetp_reco_noHcalIn_R04;
  std::vector<float> truthjetenergy_reco_noHcalIn_R04;
  std::vector<float> reco_noHcalIn_genpTratio_R04;
  std::vector<float> delRreco_noHcalIn_truth_R04;
  std::vector<int> reco_noHcalIn_parton_flavor_R04;
  std::vector<float> reco_noHcalIn_parton_frag_zt_R04;
  std::vector<int> reco_noHcalIn_jetsubid_R04;
  
  
  
};

#endif // __JETPERFORMANCE_H__

