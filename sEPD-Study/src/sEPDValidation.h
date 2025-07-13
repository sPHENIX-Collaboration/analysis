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
#include <cstdint> // Required for std::uint8_t, std::uint16_t, etc.

// -- ROOT
#include <TMath.h>
#include <TH1.h>

class PHCompositeNode;

class sEPDValidation : public SubsysReco
{
 public:

  explicit sEPDValidation(const std::string &name = "sEPDValidation");

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void set_filename(const std::string &file) {
      m_outfile_name = file;
  }

 private:

 int process_event_check(PHCompositeNode *topNode);
 int process_sEPD(PHCompositeNode *topNode);
 int process_EventPlane(PHCompositeNode *topNode);
 int process_EventPlane(Eventplaneinfo *epd_S, Eventplaneinfo *epd_N, int order = 2);
 int process_centrality(PHCompositeNode *topNode);

 int m_event;

 std::string m_outfile_name;

 unsigned int m_bins_zvtx;
 double m_zvtx_low;
 double m_zvtx_high;

 unsigned int m_bins_centbin;
 double m_centbin_low;
 double m_centbin_high;

 unsigned int m_bins_sepd_q;
 double m_sepd_q_low;
 double m_sepd_q_high;

 unsigned int m_bins_psi;
 double m_psi_low;
 double m_psi_high;

 unsigned int m_bins_Q;
 double m_Q_low;
 double m_Q_high;

 enum class EventType : std::uint8_t
 {
   ALL,
   ZVTX10,
   ZVTX10_MB
 };

 std::vector<std::string> m_eventType = {"All", "|z| < 10 cm", "|z| < 10 cm & MB"};

 // Event Vars
 double m_zvtx;
 int m_centbin;

 // Cuts
 double m_zvtx_max;

 // Logging Info
 int m_centbin_min;
 int m_centbin_max;
 double m_cent_min;
 double m_cent_max;
 double m_sepd_q_south_min;
 double m_sepd_q_south_max;
 double m_sepd_q_north_min;
 double m_sepd_q_north_max;
 double m_psi_min;
 double m_psi_max;
 double m_Q_min;
 double m_Q_max;

 std::map<std::string, std::unique_ptr<TH1>> m_hists;
};
