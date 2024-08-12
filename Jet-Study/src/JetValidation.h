// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef JETVALIDATION_H
#define JETVALIDATION_H

#include <fun4all/SubsysReco.h>
#include <jetbase/Jetv1.h>
#include <jetbase/Jetv2.h>

#include <string>
#include <vector>

#include <TH2.h>

class PHCompositeNode;
class TTree;

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

  void set_zvtx_max(Float_t m_zvtx_max) {
    this->m_zvtx_max = m_zvtx_max;
  }

 private:
  std::string m_recoJetName_r02;
  std::string m_recoJetName_r04;
  std::string m_recoJetName_r06;
  std::string m_outputTreeFileName;
  std::string m_outputQAFileName;

  Float_t m_zvtx_max;

  TH1F* hJetPt_r02;
  TH1F* hJetPt_r04;
  TH1F* hJetPt_r06;
  TH1F* hEvents;
  TH1F* hZVtx;

  UInt_t  m_bins_pt;
  Float_t m_pt_low;
  Float_t m_pt_high;

  UInt_t m_bins_events;
  Float_t m_events_low;
  Float_t m_events_high;

  UInt_t m_bins_zvtx;
  Float_t m_zvtx_low;
  Float_t m_zvtx_high;

  //! Output Tree variables
  TTree *m_T;

  //! eventwise quantities
  Int_t m_run;
  Int_t m_globalEvent;
  Int_t m_event;
  Int_t m_eventZVtx;
  Int_t m_eventZVtx30;
  Int_t m_nJets_r02;
  Int_t m_nJets_r04;
  Int_t m_nJets_r06;
  Float_t m_zvtx;

  //! trigger info
  std::vector<bool> m_triggerVector;

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
