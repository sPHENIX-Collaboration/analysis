// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef ANA_PPG09_MOD_H
#define ANA_PPG09_MOD_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

#include <calotrigger/TriggerAnalyzer.h>

class PHCompositeNode;
class TH1D;

class Ana_PPG09_Mod : public SubsysReco
{
 public:

  Ana_PPG09_Mod(
     const std::string &recojetname = "Ana_PPG09_Mod",
     const std::string &outputfilename = "PlaceHolder.root");

  ~Ana_PPG09_Mod() override;

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;
  int Reset(PHCompositeNode * /*topNode*/) override;

 private:
 TriggerAnalyzer *triggeranalyzer{nullptr};
 std::string m_recoJetName;
 std::string m_outputFileName;

 double NComp_Cut = 1;
 double RPt_Cut = 1;
 double ZVtx_Cut = 80;

 TH3F *h_Eta_Phi_Pt_[8];
 TH3F *h_EMCal_Raw_Eta_Phi_E_[8];
 TH3F *h_iHCal_Raw_Eta_Phi_E_[8];
 TH3F *h_oHCal_Raw_Eta_Phi_E_[8];
 TH3F *h_EMCal_CS_Eta_Phi_E_[8];
 TH3F *h_iHCal_CS_Eta_Phi_E_[8];
 TH3F *h_oHCal_CS_Eta_Phi_E_[8];
 TH3F *h_EMCal_C_Eta_Phi_E_[8];
 TH3F *h_iHCal_C_Eta_Phi_E_[8];
 TH3F *h_oHCal_C_Eta_Phi_E_[8];
 TH3F *h_EMCal_Jet_Eta_Phi_E_[8];
 TH3F *h_iHCal_Jet_Eta_Phi_E_[8];
 TH3F *h_oHCal_Jet_Eta_Phi_E_[8];
 TH1D *h_ZVtx_[8];

 int m_event;
};

#endif // ANA_PPG09_MOD_H
