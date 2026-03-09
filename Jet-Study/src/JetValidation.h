// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETVALIDATION_H
#define JETVALIDATION_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

#include <TH2.h>

class PHCompositeNode;
class TTree;
class TFile;

class JetValidation : public SubsysReco
{
 public:
  JetValidation();

  ~JetValidation() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  Int_t Init(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  Int_t process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  Int_t ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of all processing.
  Int_t End(PHCompositeNode *topNode) override;

  void set_outputTreeFileName(const std::string &m_outputTreeFileName) {
    this->m_outputTreeFileName = m_outputTreeFileName;
  }

  void set_outputQAFileName(const std::string &m_outputQAFileName) {
    this->m_outputQAFileName = m_outputQAFileName;
  }

  void set_saveTree(Bool_t m_saveTree) {
    this->m_saveTree = m_saveTree;
  }

  void set_bkg_tower_energy(Float_t m_bkg_tower_energy) {
    this->m_bkg_tower_energy = m_bkg_tower_energy;
  }

  void set_bkg_tower_neighbor_energy(Float_t m_bkg_tower_neighbor_energy) {
    this->m_bkg_tower_neighbor_energy = m_bkg_tower_neighbor_energy;
  }

  void set_bkg_towers(UInt_t m_bkg_towers) {
    this->m_bkg_towers = m_bkg_towers;
  }

  Bool_t isBackgroundEvent(std::vector<Float_t> &towerEnergy);

 private:
  std::string m_recoJetName_r02;
  std::string m_recoJetName_r04;
  std::string m_recoJetName_r06;
  TFile* m_outputTreeFile;
  TFile* m_outputQAFile;
  std::string m_outputTreeFileName;
  std::string m_outputQAFileName;
  std::string m_emcTowerNodeBase;
  std::string m_emcTowerNode;
  std::string m_ihcalTowerNode;
  std::string m_ohcalTowerNode;
  std::string m_emcTowerNodeSub;
  std::string m_ihcalTowerNodeSub;
  std::string m_ohcalTowerNodeSub;

  enum class Trigger {
      MBD_NS_1 = 10,
      JET_6    = 20,
      JET_8    = 21,
      JET_10   = 22,
      JET_12   = 23
  };

  enum class EventStatus {
      ALL                 = 0,
      ZVTX                = 1,
      ZVTX60              = 2,
      ZVTX50              = 3,
      ZVTX30              = 4,
      ZVTX20              = 5,
      ZVTX10              = 6,
      ALL_MBDNS1          = 7,
      ALL_MBDNS1_JET8     = 8,
      ALL_MBDNS1_JET10    = 9,
      ALL_MBDNS1_JET12    = 10,
      ZVTX60_MBDNS1       = 11,
      ZVTX60_MBDNS1_JET8  = 12,
      ZVTX60_MBDNS1_JET10 = 13,
      ZVTX60_MBDNS1_JET12 = 14,
      ZVTX60_JET8         = 15,
      ZVTX60_JET10        = 16,
      ZVTX60_JET12        = 17
  };

  enum class JetEvent_Status {
      ALL_MBDNS1          = 0,
      ALL_MBDNS1_JET8     = 1,
      ALL_MBDNS1_JET10    = 2,
      ALL_MBDNS1_JET12    = 3,
      ZVTX60_MBDNS1       = 4,
      ZVTX60_MBDNS1_JET8  = 5,
      ZVTX60_MBDNS1_JET10 = 6,
      ZVTX60_MBDNS1_JET12 = 7,
      ZVTX60              = 8,
      ZVTX60_JET8         = 9,
      ZVTX60_JET10        = 10,
      ZVTX60_JET12        = 11
  };

  std::vector<std::string> JetEvent_Status_vec = {
      "ALL_MBDNS1",
      "ALL_MBDNS1_JET8",
      "ALL_MBDNS1_JET10",
      "ALL_MBDNS1_JET12",
      "ZVTX60_MBDNS1",
      "ZVTX60_MBDNS1_JET8",
      "ZVTX60_MBDNS1_JET10",
      "ZVTX60_MBDNS1_JET12",
      "ZVTX60",
      "ZVTX60_JET8",
      "ZVTX60_JET10",
      "ZVTX60_JET12"
  };

  Bool_t  m_saveTree;
  Float_t m_zvtx_max[5] = {60,50,30,20,10}; /*cm*/
  Float_t m_lowPtThreshold;
  Float_t m_highPtThreshold;
  Float_t m_subLeadPtThreshold;
  UInt_t  m_highPtJetCtr;
  UInt_t m_bins_phi;
  UInt_t m_bins_eta;
  Float_t m_eta_low;
  Float_t m_eta_high;

  TH1F* hEvents;
  TH1F* hEventsBkg;
  TH1F* hZVtx;

  // Jet Histograms
  // pt
  std::vector<TH1F*> hJetPt_r02;
  std::vector<TH1F*> hJetPt_r04;
  std::vector<TH1F*> hJetPt_r06;

  std::vector<TH1F*> hJetPt_r02_bkg;
  std::vector<TH1F*> hJetPt_r04_bkg;
  std::vector<TH1F*> hJetPt_r06_bkg;

  std::vector<TH1F*> hJetPt_r02_nobkg;
  std::vector<TH1F*> hJetPt_r04_nobkg;
  std::vector<TH1F*> hJetPt_r06_nobkg;

  // deltaPhi
  std::vector<TH1F*> hJetDeltaPhi_r02;
  std::vector<TH1F*> hJetDeltaPhi_r04;
  std::vector<TH1F*> hJetDeltaPhi_r06;

  std::vector<TH1F*> hJetDeltaPhi_r02_bkg;
  std::vector<TH1F*> hJetDeltaPhi_r04_bkg;
  std::vector<TH1F*> hJetDeltaPhi_r06_bkg;

  std::vector<TH1F*> hJetDeltaPhi_r02_nobkg;
  std::vector<TH1F*> hJetDeltaPhi_r04_nobkg;
  std::vector<TH1F*> hJetDeltaPhi_r06_nobkg;

  // eta_phi
  std::vector<TH2F*> h2JetEtaPhi_r02;
  std::vector<TH2F*> h2JetEtaPhi_r04;
  std::vector<TH2F*> h2JetEtaPhi_r06;

  std::vector<TH2F*> h2JetEtaPhi_r02_bkg;
  std::vector<TH2F*> h2JetEtaPhi_r04_bkg;
  std::vector<TH2F*> h2JetEtaPhi_r06_bkg;

  std::vector<TH2F*> h2JetEtaPhi_r02_nobkg;
  std::vector<TH2F*> h2JetEtaPhi_r04_nobkg;
  std::vector<TH2F*> h2JetEtaPhi_r06_nobkg;

  std::vector<TH2F*> h2TowerEnergy;
  std::vector<TH2F*> h2TowerEnergySub;

  UInt_t  m_bins_pt;
  Float_t m_pt_low;
  Float_t m_pt_high;

  UInt_t m_bins_events;

  UInt_t m_bins_zvtx;
  Float_t m_zvtx_low;
  Float_t m_zvtx_high;

  Float_t m_bkg_tower_energy;
  Float_t m_bkg_tower_neighbor_energy;
  UInt_t  m_bkg_towers;

  //! Output Tree variables
  TTree* m_T;

  //! eventwise quantities
  Int_t m_run;
  Int_t m_globalEvent;
  Int_t m_event;
  Int_t m_nJets_r02;
  Int_t m_nJets_r04;
  Int_t m_nJets_r06;
  Float_t m_zvtx;

  //! tower info
  std::vector<Bool_t>  m_towersCEMCBase_isGood;
  std::vector<Float_t> m_towersCEMCBase_energy;
  std::vector<Float_t> m_towersCEMCBase_time;
  std::vector<Bool_t>  m_towersCEMC_isGood;
  std::vector<Bool_t>  m_towersIHCal_isGood;
  std::vector<Bool_t>  m_towersOHCal_isGood;
  std::vector<Float_t> m_towersCEMC_energy;
  std::vector<Float_t> m_towersIHCal_energy;
  std::vector<Float_t> m_towersOHCal_energy;
  std::vector<Float_t> m_towersCEMCSub_energy;
  std::vector<Float_t> m_towersIHCalSub_energy;
  std::vector<Float_t> m_towersOHCalSub_energy;

  //! trigger info
  std::vector<bool> m_scaledVector;

  //! reconstructed jets R = 0.2
  Int_t m_nJet_r02;
  std::vector<Int_t> m_id_r02;
  std::vector<Int_t> m_nComponent_r02;
  std::vector<Float_t> m_eta_r02;
  std::vector<Float_t> m_phi_r02;
  std::vector<Float_t> m_e_r02;
  std::vector<Float_t> m_pt_r02;

  //! reconstructed jets R = 0.4
  Int_t m_nJet_r04;
  std::vector<Int_t> m_id_r04;
  std::vector<Int_t> m_nComponent_r04;
  std::vector<Float_t> m_eta_r04;
  std::vector<Float_t> m_phi_r04;
  std::vector<Float_t> m_e_r04;
  std::vector<Float_t> m_pt_r04;

  //! reconstructed jets R = 0.6
  Int_t m_nJet_r06;
  std::vector<Int_t> m_id_r06;
  std::vector<Int_t> m_nComponent_r06;
  std::vector<Float_t> m_eta_r06;
  std::vector<Float_t> m_phi_r06;
  std::vector<Float_t> m_e_r06;
  std::vector<Float_t> m_pt_r06;
};

#endif  // JETVALIDATION_H
