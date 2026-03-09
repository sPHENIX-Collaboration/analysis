// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETVALIDATIONTC_H
#define JETVALIDATIONTC_H

#include <fun4all/SubsysReco.h>
#include <jetbase/Jetv1.h>
#include <jetbase/Jetv2.h>

#include <string>
#include <vector>

#include <TFile.h>
#include <TH1F.h>
#include <TH2D.h>


class PHCompositeNode;
class TTree;
class Fun4AllHistoManager;

class JetValidationTC : public SubsysReco
{
 public:

  JetValidationTC(const std::string &recojetname = "AntiKt_Tower_r04",
		const std::string &truthjetname = "AntiKt_Truth_r04",
		const std::string &outputfilename = "myjetanalysis.root");

  ~JetValidationTC() override;

  void
    setEtaRange(double low, double high)
  {
    m_etaRange.first = low;
    m_etaRange.second = high;
  }
 void
   setPtRange(double low, double high)
 {
   m_ptRange.first = low;
   m_ptRange.second = high;
 }
 void
   doTruth(int flag)
 {
   m_doTruthJets = flag;
 }
 void
   doSeeds(int flag)
 {
   m_doSeeds = flag;
 }
 void
   doUnsub(int flag)
 {
   m_doUnsubJet = flag;
 }

 void doClusters(int flag) 
 {
  m_doClusters = flag;
 }

void setJetPtThreshold(float pt = 10.){
  m_pi0_jetThreshold = pt;
}

void setMinClusterE(float e) {
  m_minClusterE = e;
}

void setMinPhotonProb(float e) {
  m_minPhotonProb = e;
}

void setMinClusterDeltaR(float R = 0.08) {
  m_mindR = R;
}

void setHistoFileName(std::string name = "default_histos.root") {
  m_histoFileName = name;
}

void setMaxdR(float dR = 0.4) {
  m_maxdR = dR;
}

void setClusterType(std::string type = "TOPOCLUSTER_EMCAL"){
  m_clusterType = type;
}

void removeClustersInJets(int flag = 0){
  m_removeJetClusters = flag;
}

void findJetPions(Jet* jet);

void reconstruct_pi0s(PHCompositeNode *topNode);

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
   */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

 private:
  Fun4AllHistoManager *hm;
  TFile *outfile{nullptr};


  std::string m_recoJetName;
  std::string m_truthJetName;
  std::string m_outputFileName;
  std::string m_histoFileName;
  std::pair<double, double> m_etaRange;
  std::pair<double, double> m_ptRange;
  int m_doTruthJets;
  int m_doSeeds;
  int m_doUnsubJet;
  int m_doClusters;
  int m_removeJetClusters;


  //! Output Tree variables
  TTree *m_T;

  //! eventwise quantities
  int m_event;
  int m_nTruthJet;
  int m_nJet;
  float m_totalCalo;
  int m_centrality;
  float m_impactparam;
  float m_zvtx;
  int m_hasJetAboveThreshold;

  //! reconstructed jets
  std::vector<int> m_id;
  std::vector<int> m_nComponent;
  std::vector<float> m_eta;
  std::vector<float> m_phi;
  std::vector<float> m_e;
  std::vector<float> m_pt;
  std::vector<float> m_zg;
  std::vector<float> m_rg;
  std::vector<float> m_Et;

  //pi0 reco quantities
  float m_minClusterE;
  float m_minPhotonProb;
  float m_mindR;
  std::string m_clusterType;

  TH1F *h_pi0M;
  TH1F *h_npions;
  TH2F *h_eta_phi_clusters; //eta phi dist. of clusters used for pi0 reco
  TH1F *h_jetPionMult;
  TH1F *h_photonEnergy;


  int nclus;

  std::vector<float> cluster_energy;
  std::vector<float> cluster_eta;
  std::vector<float> cluster_phi;
  std::vector<float> cluster_prob;
  std::vector<float> cluster_chi2;

  int m_npi0;

  std::vector<float> pi0cand_pt;
  std::vector<float> pi0cand_eta;
  std::vector<float> pi0cand_phi;
  std::vector<float> pi0cand_mass;
  std::vector<float> pi0cand_energy;

  float m_maxdR; // the maximum distance between candidate pions and jet axes
  int m_nPionJets; //the number of jets containing at least one pi0
  std::vector<int> m_jetPionMult; //how many pions in each jet?
  std::vector<float> m_pionZ; //fraction of jet pT carried by pions
  std::vector<float> m_jetPionPt;
  std::vector<float> m_jetPionMass;
  std::vector<float> m_jetPionEta;
  std::vector<float> m_jetPionPhi;
  std::vector<float> m_jetPionEnergy;
  //std::vector<float> m_pionMassJetsRemoved; //pi0 invariant mass after candidate within jets are removed

  std::vector<float> m_constE; //jet constituent energy, either inclusive, or split between emcal and hcal TCs
  std::vector<float> m_hcalE; 
  std::vector<float> m_ecalE;

  std::vector<std::vector<float>> m_constEt; //jet constituent transverse energy, either inclusive, or split between emcal and hcal TCs
  std::vector<std::vector<float>> m_cluster_hcalEt; 
  std::vector<std::vector<float>> m_cluster_ecalEt;

  //Topo-Cluster variables
  std::vector<float> m_fe; //EMCal signal ratio
  std::vector<float> m_fh; //HCal signal ratio
  std::vector<float> m_fEt_emcal; //same ratios but with transverse energy
  std::vector<float> m_fEt_hcal;
  std::vector<float> m_Et_emcal; //Summed transverse energy for each calorimeter
  std::vector<float> m_Et_hcal;
  std::vector<int> m_emcal_cluster_mult;
  std::vector<int> m_hcal_cluster_mult;
  std::vector<int> m_cluster_mult;

  float m_pi0_jetThreshold;

  //towers from clusters
  std::vector<float> m_ClusterTowE;



  //! unsubtracted jets
  std::vector<float> m_unsub_pt;
  std::vector<float> m_sub_et;

  //! truth jets
  std::vector<int> m_truthID;
  std::vector<int> m_truthNComponent;
  std::vector<float> m_truthEta;
  std::vector<float> m_truthPhi;
  std::vector<float> m_truthE;
  std::vector<float> m_truthPt;
  std::vector<float> m_truthdR;

  //! seed jets
  std::vector<float> m_eta_rawseed;
  std::vector<float> m_phi_rawseed;
  std::vector<float> m_pt_rawseed;
  std::vector<float> m_e_rawseed;
  std::vector<int> m_rawseed_cut;
  std::vector<float> m_eta_subseed;
  std::vector<float> m_phi_subseed;
  std::vector<float> m_pt_subseed;
  std::vector<float> m_e_subseed;
  std::vector<int> m_subseed_cut;

  std::vector <int> m_triggers;
};

#endif // JETVALIDATIONTC_H
