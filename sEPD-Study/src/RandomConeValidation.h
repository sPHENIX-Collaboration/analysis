#pragma once

#include <fun4all/SubsysReco.h>

#include "RandomConeMaker.h"

#include <string>

class PHCompositeNode;

class RandomConeValidation : public SubsysReco
{
 public:
  explicit RandomConeValidation(const std::string &name = "RandomConeValidation");

  int Init(PHCompositeNode *topNode) override;
  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

 private:
  struct ConeData
  {
    double eta{0};          // NOLINT(misc-non-private-member-variables-in-classes)
    double phi{0};          // NOLINT(misc-non-private-member-variables-in-classes)
    double pt{0};           // NOLINT(misc-non-private-member-variables-in-classes)
    double energy{0};       // NOLINT(misc-non-private-member-variables-in-classes)
    double pt_sub1{0};      // NOLINT(misc-non-private-member-variables-in-classes)
    double energy_sub1{0};  // NOLINT(misc-non-private-member-variables-in-classes)

    void clear()
    {
      eta = 0;
      phi = 0;
      pt = 0;
      energy = 0;
      pt_sub1 = 0;
      energy_sub1 = 0;
    }
  };

  struct EventData
  {
    ConeData r02;
    ConeData r03;
  };

  EventData m_data;

  RandomConeMaker m_maker_r02;
  RandomConeMaker m_maker_r03;
};
