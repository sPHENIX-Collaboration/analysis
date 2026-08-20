// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

#include <fun4all/SubsysReco.h>

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

class PHCompositeNode;

class EventPlaneQA : public SubsysReco
{
 public:
  explicit EventPlaneQA(const std::string &name = "EventPlaneQA");

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void set_do_ep(bool b = true) { m_do_ep = b; }
  void set_do_sepd(bool b = true) { m_do_sepd = b; }
  void set_sepd_threshold(double threshold) { m_sepd_channel_threshold = threshold; }

 private:

  int process_event_plane(PHCompositeNode *topNode);
  int process_sepd(PHCompositeNode *topNode);

  struct EventData
  {
    // sEPD - Event Plane
    double psi2_raw_S{0};
    double psi2_raw_N{0};
    double psi2_raw_NS{0};

    double psi2_S{0};
    double psi2_N{0};
    double psi2_NS{0};

    // sEPD - QA
    double sepd_charge_south{0};
    double sepd_charge_north{0};
  };

  EventData m_data;

  double m_sepd_channel_threshold{0.5};

  bool m_do_ep{true};
  bool m_do_sepd{true};
};
