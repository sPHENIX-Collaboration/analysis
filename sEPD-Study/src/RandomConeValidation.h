#pragma once

#include <fun4all/SubsysReco.h>

#include "RandomConeMaker.h"

#include <string>
#include <vector>

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

  void set_do_detailed(bool b = true) { m_do_detailed = b; }
  bool get_do_detailed() const { return m_do_detailed; }

 private:
  struct ConeData
  {
    double eta{0};          // NOLINT(misc-non-private-member-variables-in-classes)
    double phi{0};          // NOLINT(misc-non-private-member-variables-in-classes)
    double pt{0};           // NOLINT(misc-non-private-member-variables-in-classes)
    double energy{0};       // NOLINT(misc-non-private-member-variables-in-classes)
    double pt_sub1{0};      // NOLINT(misc-non-private-member-variables-in-classes)
    double energy_sub1{0};  // NOLINT(misc-non-private-member-variables-in-classes)

    // Detailed tower info - Unsubtracted
    std::vector<int> emcal_tower_index;     // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> emcal_tower_energy;  // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> emcal_tower_pt;      // NOLINT(misc-non-private-member-variables-in-classes)

    std::vector<int> ihcal_tower_index;     // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> ihcal_tower_energy;  // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> ihcal_tower_pt;      // NOLINT(misc-non-private-member-variables-in-classes)

    std::vector<int> ohcal_tower_index;     // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> ohcal_tower_energy;  // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> ohcal_tower_pt;      // NOLINT(misc-non-private-member-variables-in-classes)

    // Detailed tower info - Subtracted (sub1)
    std::vector<double> emcal_sub1_tower_energy;  // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> emcal_sub1_tower_pt;      // NOLINT(misc-non-private-member-variables-in-classes)

    std::vector<double> ihcal_sub1_tower_energy;  // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> ihcal_sub1_tower_pt;      // NOLINT(misc-non-private-member-variables-in-classes)

    std::vector<double> ohcal_sub1_tower_energy;  // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> ohcal_sub1_tower_pt;      // NOLINT(misc-non-private-member-variables-in-classes)

    void clear()
    {
      eta = 0;
      phi = 0;
      pt = 0;
      energy = 0;
      pt_sub1 = 0;
      energy_sub1 = 0;

      emcal_tower_index.clear();
      emcal_tower_energy.clear();
      emcal_tower_pt.clear();

      ihcal_tower_index.clear();
      ihcal_tower_energy.clear();
      ihcal_tower_pt.clear();

      ohcal_tower_index.clear();
      ohcal_tower_energy.clear();
      ohcal_tower_pt.clear();

      emcal_sub1_tower_energy.clear();
      emcal_sub1_tower_pt.clear();

      ihcal_sub1_tower_energy.clear();
      ihcal_sub1_tower_pt.clear();

      ohcal_sub1_tower_energy.clear();
      ohcal_sub1_tower_pt.clear();
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

  bool m_do_detailed{false};
};
