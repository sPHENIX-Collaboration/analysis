// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

// -- sPHENIX
#include <fun4all/SubsysReco.h>
#include <eventplaneinfo/Eventplaneinfo.h>

// -- c++
#include <string>
#include <vector>
#include <memory>
#include <map>

// -- ROOT
#include <TMath.h>
#include <TH1.h>

class PHCompositeNode;

class sEPDValidation : public SubsysReco
{
 public:

  explicit sEPDValidation(const std::string &name = "sEPDValidation");

  Int_t Init(PHCompositeNode *topNode) override;
  Int_t process_event(PHCompositeNode *topNode) override;
  Int_t End(PHCompositeNode *topNode) override;

  void set_filename(const std::string &file) {
      m_outfile_name = file;
  }

 private:

 Int_t process_event_check(PHCompositeNode *topNode);
 Int_t process_sEPD(PHCompositeNode *topNode);
 Int_t process_EventPlane(PHCompositeNode *topNode);
 Int_t process_EventPlane(Eventplaneinfo *_EPDS, Eventplaneinfo *_EPDN, Int_t order = 2);
 Int_t process_centrality(PHCompositeNode *topNode);

 Int_t m_event;

 std::string m_outfile_name;

 UInt_t m_bins_zvtx;
 Double_t m_zvtx_low;
 Double_t m_zvtx_high;

 UInt_t m_bins_centbin;
 Double_t m_centbin_low;
 Double_t m_centbin_high;

 UInt_t m_bins_sepd_q;
 Double_t m_sepd_q_low;
 Double_t m_sepd_q_high;

 UInt_t m_bins_psi;
 Double_t m_psi_low;
 Double_t m_psi_high;

 UInt_t m_bins_Q;
 Double_t m_Q_low;
 Double_t m_Q_high;

 enum m_event_type
 {
   ALL,
   ZVTX10,
   ZVTX10_MB
 };

 std::vector<std::string> m_eventType = {"All", "|z| < 10 cm", "|z| < 10 cm & MB"};

 // Event Vars
 Int_t m_zvtx;
 Int_t m_centbin;

 // Cuts
 Double_t m_zvtx_max;

 // Logging Info
 Int_t m_centbin_min;
 Int_t m_centbin_max;
 Double_t m_cent_min;
 Double_t m_cent_max;
 Double_t m_sepd_q_south_min;
 Double_t m_sepd_q_south_max;
 Double_t m_sepd_q_north_min;
 Double_t m_sepd_q_north_max;
 Double_t m_psi_min;
 Double_t m_psi_max;
 Double_t m_Q_min;
 Double_t m_Q_max;

 std::map<std::string, std::unique_ptr<TH1>> m_hists;
};
