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

class EventValidation : public SubsysReco
{
 public:
  EventValidation();

  ~EventValidation() override;

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

  void set_zvtx_max(Float_t m_zvtx_max) {
    this->m_zvtx_max = m_zvtx_max;
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

  void set_use_zvtx(Bool_t m_use_zvtx) {
    this->m_use_zvtx = m_use_zvtx;
  }

  void set_saveTree(Bool_t m_saveTree) {
    this->m_saveTree = m_saveTree;
  }

  Bool_t isBackgroundEvent(std::vector<Float_t> &towerEnergy);

 private:
  TFile* m_outputTreeFile;
  TFile* m_outputQAFile;
  std::string m_outputTreeFileName;
  std::string m_outputQAFileName;
  std::string m_emcTowerNodeBase;
  std::string m_emcTowerNode;
  std::string m_ihcalTowerNode;
  std::string m_ohcalTowerNode;

  enum class Trigger {
      MBD_NS_1 = 10,
      JET_6    = 20,
      JET_8    = 21,
      JET_10   = 22,
      JET_12   = 23
  };

  enum class EventStatus {
      trigger           = 0,
      trigger_bkg       = 1,
      trigger_bkgCEMC   = 2,
      trigger_mbdNS     = 3,
      trigger_mbdNS_bkg = 4
  };

  Bool_t  m_saveTree;
  UInt_t  m_saveHistMax;
  Bool_t  m_use_zvtx;
  Float_t m_zvtx_max;
  Float_t m_zvtx_max2;
  Float_t m_zvtx_max3;

  TH1F* hEvents;
  TH1F* hEvents_Jet6;
  TH1F* hEvents_Jet8;
  TH1F* hEvents_Jet10;
  TH1F* hEvents_Jet12;

  TH1F* hEvents_Jet6_A;
  TH1F* hEvents_Jet8_A;
  TH1F* hEvents_Jet10_A;
  TH1F* hEvents_Jet12_A;

  TH1F* hEvents_Jet6_bkg_A;
  TH1F* hEvents_Jet8_bkg_A;
  TH1F* hEvents_Jet10_bkg_A;
  TH1F* hEvents_Jet12_bkg_A;

  TH1F* hZVtx;

  std::vector<TH2F*> h2TowerEnergy;
  std::vector<TH2F*> h2TowerEnergyCEMC;

  UInt_t m_bins_events;
  UInt_t m_bins_events_A;
  UInt_t m_bins_events_jets;
  UInt_t m_bins_phi;
  UInt_t m_bins_eta;

  Float_t m_events_A_high;
  UInt_t m_bins_zvtx;
  Float_t m_zvtx_low;
  Float_t m_zvtx_high;

  // bkg
  Float_t m_bkg_tower_energy;
  Float_t m_bkg_tower_neighbor_energy;
  UInt_t  m_bkg_towers;
  std::vector<Bool_t> m_hasBkg_vec;
  std::vector<Bool_t> m_hasBkgCEMC_vec;

  //! Output Tree variables
  TTree* m_T;

  //! eventwise quantities
  Int_t m_run;
  Int_t m_globalEvent;
  Int_t m_event;
  Int_t m_eventZVtx;
  Int_t m_eventZVtx30;
  Float_t m_zvtx;
  Bool_t m_hasBkg;
  Bool_t m_hasBkgCEMC;

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

  //! trigger info
  std::vector<Bool_t> m_triggerVector;
};

#endif  // JETVALIDATION_H
