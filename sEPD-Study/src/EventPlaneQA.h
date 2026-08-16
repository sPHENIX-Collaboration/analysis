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

 private:

  struct EventData
  {
    // sEPD
    double psi2_raw_S{0};
    double psi2_raw_N{0};
    double psi2_raw_NS{0};

    double psi2_S{0};
    double psi2_N{0};
    double psi2_NS{0};
  };

  EventData m_data;
};
