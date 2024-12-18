// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef INCLUSIVEJET_H
#define INCLUSIVEJET_H

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

class InclusiveJet : public SubsysReco
{
 public:

  InclusiveJet(const std::string &recojetname = "AntiKt_Tower_r04",
		const std::string &truthjetname = "AntiKt_Truth_r04",
		const std::string &outputfilename = "myjetanalysis.root");

  ~InclusiveJet() override;

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
   doTruthParticles(int flag)
  {
    m_doTruth = flag;
  }
 void
   doSeeds(int flag)
 {
   m_doSeeds = flag;
 }
 void 
  doTowers(int flag)
  {
    m_doTowers = flag;
  }
  void
  doTopoclusters(int flag)
  {
    m_doTopoclusters = flag;
  }
  void
  doEmcalClusters(int flag)
  {
    m_doEmcalClusters = flag;
  }
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
  std::string m_recoJetName;
  std::string m_truthJetName;
  std::string m_outputFileName;
  std::pair<double, double> m_etaRange;
  std::pair<double, double> m_ptRange;
  int m_doTruthJets;
  int m_doTruth;
  int m_doTowers;
  int m_doEmcalClusters;
  int m_doTopoclusters;
  int m_doSeeds;

  //! Output Tree variables
  TTree *m_T;

  //! eventwise quantities
  int m_event;
  int m_nTruthJet;
  int m_nJet;
  float m_totalCalo;
  float m_zvtx;

  //!trigger info
  std::vector<int> m_triggerVector;

  //! reconstructed jets
  std::vector<int> m_nComponent;
  std::vector<float> m_eta;
  std::vector<float> m_phi;
  std::vector<float> m_e;
  std::vector<float> m_pt;

  //! truth jets
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

  int m_emcaln = 0;
  float m_emcale[24576] = {0}; 
  float m_emcalchi2[24576] = {0}; 
  float m_emcaleta[24576] = {0};
  float m_emcalphi[24576] = {0};
  int m_emcalieta[24576] = {0};
  int m_emcaliphi[24576] = {0};
  int m_emcalstatus[24576] = {0};
  float m_emcaltime[24576] = {0};
  int m_ihcaln = 0;
  float m_ihcale[1536] = {0};
  float m_ihcalchi2[1536] = {0};
  float m_ihcaleta[1536] = {0};
  float m_ihcalphi[1536] = {0};
  int m_ihcalieta[1536] = {0};
  int m_ihcaliphi[1536] = {0};
  int m_ihcalstatus[1536] = {0};
  float m_ihcaltime[1536] = {0};
  int m_ohcaln = 0;
  float m_ohcale[1536] = {0};
  float m_ohcalchi2[1536] = {0};
  float m_ohcaleta[1536] = {0};
  float m_ohcalphi[1536] = {0};
  int m_ohcalieta[1536] = {0};
  int m_ohcaliphi[1536] = {0};
  int m_ohcalstatus[1536] = {0};
  float m_ohcaltime[1536] = {0};

  int truthpar_n;
  float truthpar_pz[100000];
  float truthpar_pt[100000];
  float truthpar_e[100000];
  float truthpar_eta[100000];
  float truthpar_phi[100000];
  int truthpar_pid[100000];

  int m_emcal_clsmult;
  float m_emcal_cluster_e[10000];
  float m_emcal_cluster_eta[10000];
  float m_emcal_cluster_phi[10000];

  int m_clsmult;
  float m_cluster_e[10000];
  float m_cluster_eta[10000];
  float m_cluster_phi[10000];

};

#endif // INCLUSIVEJET_H
