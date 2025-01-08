// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETVALIDATIONv2_H
#define JETVALIDATIONv2_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

#include <TH2.h>
#include <TH3.h>

#include <calotrigger/TriggerAnalyzer.h>

class PHCompositeNode;
class TFile;

class JetValidationv2 : public SubsysReco
{
 public:
  JetValidationv2();

  ~JetValidationv2() override;

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

  void set_outputFile(const std::string &m_outputFile) {
    this->m_outputFile = m_outputFile;
  }

 private:
  std::string m_recoJetName_r04;

  std::string m_emcTowerNodeBase;
  std::string m_emcTowerNode;
  std::string m_ihcalTowerNode;
  std::string m_ohcalTowerNode;

  std::string m_outputFile;

  UInt_t m_triggerBits;
  Float_t m_zvtx_max;

  UInt_t  m_bins_phi;
  Float_t m_phi_low;
  Float_t m_phi_high;

  UInt_t  m_bins_eta;
  Float_t m_eta_low;
  Float_t m_eta_high;

  UInt_t  m_bins_pt;
  Float_t m_pt_low;
  Float_t m_pt_high;
  Float_t m_pt_background;

  UInt_t  m_bins_zvtx;
  Float_t m_zvtx_low;
  Float_t m_zvtx_high;

  UInt_t  m_event;
  Float_t m_R;

  enum m_status {ALL, ZVTX60, ZVTX60_BKG, ZVTX60_BKG_EMCAL};
  std::vector<std::string> m_eventStatus = {"All", "|z| < 60 cm", "|z| < 60 cm and Bkg", "|z| < 60 cm, Bkg, and EMCal"};

  TriggerAnalyzer* m_triggeranalyzer = nullptr;

  TH1* hEvents = nullptr;
  TH1* hTriggers = nullptr;
  TH1* hTriggersBkg = nullptr;
  TH1* hzvtxAll = nullptr;
  TH2* hTriggerZvtxBkg = nullptr;
  std::vector<TH1*> hzvtx;
  std::vector<TH3*> hjetPhiEtaPt;
  std::vector<TH2*> hCEMCBase;
  std::vector<TH2*> hCEMC;
  std::vector<TH2*> hIHCal;
  std::vector<TH2*> hOHCal;
};

#endif  // JETVALIDATIONv2_H
