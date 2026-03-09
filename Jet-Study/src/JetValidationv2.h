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

  void set_zvtx_max(Float_t m_zvtx_max) {
    this->m_zvtx_max = m_zvtx_max;
  }

  void set_trigger(Int_t m_triggerBit) {
    this->m_triggerBit = m_triggerBit;
  }

  void set_pt_background(Float_t m_pt_background) {
    this->m_pt_background = m_pt_background;
  }

 private:
  std::string m_recoJetName_r04;

  std::string m_emcTowerBaseNode;
  std::string m_emcTowerNode;
  std::string m_ihcalTowerNode;
  std::string m_ohcalTowerNode;

  std::string m_outputFile;

  Int_t m_triggerBit;
  Int_t m_triggerBits;
  Float_t m_zvtx_max;

  UInt_t  m_bins_phi;
  UInt_t  m_bins_phi_cemc;
  Float_t m_phi_low;
  Float_t m_phi_high;

  UInt_t  m_bins_eta;
  UInt_t  m_bins_eta_cemc;
  Float_t m_eta_low;
  Float_t m_eta_high;

  UInt_t  m_bins_pt;
  Float_t m_pt_low;
  Float_t m_pt_high;
  Float_t m_pt_background;

  UInt_t  m_bins_zvtx;
  Float_t m_zvtx_low;
  Float_t m_zvtx_high;

  UInt_t m_bins_frac;
  Float_t m_frac_low;
  Float_t m_frac_high;

  UInt_t m_bins_ET;
  Float_t m_ET_low;
  Float_t m_ET_high;

  UInt_t m_bins_constituents;
  Float_t m_constituents_low;
  Float_t m_constituents_high;

  UInt_t m_bins_nJets;
  Float_t m_nJets_low;
  Float_t m_nJets_high;

  UInt_t  m_event;
  Float_t m_R;

  Int_t m_nJets_min;
  Int_t m_nJets_max;

  Int_t m_constituents_min;
  Int_t m_constituents_max;

  Int_t m_pt_min;
  Int_t m_pt_max;

  enum m_status {ALL, ZVTX30, ZVTX30_BKG
                            , ZVTX30_BKG_failsLoEmJetCut
                            , ZVTX30_BKG_failsHiEmJetCut
                            , ZVTX30_BKG_failsIhJetCut
                            , ZVTX30_BKG_failsNoneJetCut
                };

  std::vector<std::string> m_eventStatus = {"All", "|z| < 30 cm"
                                                 , "|z| < 30 cm and Bkg"
                                                 , "|z| < 30 cm and Bkg and failsLoEmJetCut"
                                                 , "|z| < 30 cm and Bkg and failsHiEmJetCut"
                                                 , "|z| < 30 cm and Bkg and failsIhJetCut"
                                                 , "|z| < 30 cm and Bkg and failsNoneJetCut"
                                           };

  TriggerAnalyzer* m_triggeranalyzer = nullptr;

  TH1* hEvents = nullptr;
  TH1* hzvtxAll = nullptr;
  TH3* hjetPhiEtaPt = nullptr;
  TH2* hjetConstituentsVsPt = nullptr;
  TH2* hNJetsVsLeadPt = nullptr;
  TH2* h2ETVsFracCEMC = nullptr;
  TH2* h2FracOHCalVsFracCEMC = nullptr;
  TH2* h2ETVsFracCEMC_miss = nullptr;
  TH2* h2FracOHCalVsFracCEMC_miss = nullptr;
  std::vector<TH2*> hCEMCBase;
  std::vector<TH2*> hCEMC;
  std::vector<TH2*> hIHCal;
  std::vector<TH2*> hOHCal;
};

#endif  // JETVALIDATIONv2_H
