// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

// -- sPHENIX
#include <eventplaneinfo/Eventplaneinfo.h>
#include <fun4all/SubsysReco.h>

// -- c++
#include <cstdint> // Required for std::uint8_t, std::uint16_t, etc.
#include <map>
#include <memory>
#include <string>
#include <vector>

// -- ROOT
#include <TH1.h>
#include <TMath.h>

class PHCompositeNode;

class sEPDValidation : public SubsysReco
{
 public:
  explicit sEPDValidation(const std::string &name = "sEPDValidation");

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void set_filename(const std::string &file)
  {
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

  unsigned int m_bins_cent;
  double m_cent_low;
  double m_cent_high;

  unsigned int m_bins_sepd_charge;
  double m_sepd_charge_low;
  double m_sepd_charge_high;

  unsigned int m_bins_sepd_Q;
  double m_sepd_Q_low;
  double m_sepd_Q_high;

  unsigned int m_bins_sepd_total_charge;
  double m_sepd_total_charge_low;
  double m_sepd_total_charge_high;

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
  double m_cent;

  // Cuts
  double m_zvtx_max;
  double m_sepd_charge_threshold;

  // Logging Info
  double m_cent_min;
  double m_cent_max;
  double m_sepd_charge_min;
  double m_sepd_charge_max;
  double m_sepd_Q_min;
  double m_sepd_Q_max;
  double m_sepd_total_charge_south_min;
  double m_sepd_total_charge_south_max;
  double m_sepd_total_charge_north_min;
  double m_sepd_total_charge_north_max;
  double m_sepd_phi_min;
  double m_sepd_phi_max;
  double m_psi_min;
  double m_psi_max;

  std::map<std::string, std::unique_ptr<TH1>> m_hists;
  std::map<std::string, int> m_ctr;
};
