#ifndef CALOCHECK_H
#define CALOCHECK_H

#include <fun4all/SubsysReco.h>

// ====================================================================
// ROOT Includes
// ====================================================================
#include <TH1.h>
#include <TH2.h>
#include <TProfile2D.h>

// -- c++
#include <string>
#include <cstdint>
#include <vector>

class PHCompositeNode;

class CaloQA : public SubsysReco
{
 public:
  explicit CaloQA(const std::string &name = "CaloQA");

  int Init(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  int ResetEvent(PHCompositeNode *topNode) override;

  int End(PHCompositeNode *topNode) override;

  void set_do_hist(bool do_hists)
  {
    m_do_hists = do_hists;
  }

  void set_do_detailed(bool b = true) { m_do_detailed = b; }
  void set_do_iter(bool b = true) { m_do_iter = b; }
  void set_do_mult(bool b = true) { m_do_mult = b; }
  void set_do_retower(bool b = true) { m_do_retower = b; }
  void set_do_cemc_retower(bool b = true) { m_do_retower = b; }
  void set_do_tree(bool b = true) { m_do_tree = b; }

  bool get_do_detailed() const { return m_do_detailed; }
  bool get_do_iter() const { return m_do_iter; }
  bool get_do_mult() const { return m_do_mult; }
  bool get_do_retower() const { return m_do_retower; }
  bool get_do_cemc_retower() const { return m_do_retower; }
  bool get_do_tree() const { return m_do_tree; }

 private:

  int process_centrality(PHCompositeNode *topNode);
  int process_calo(PHCompositeNode *topNode);

  // Hists
  struct AnalysisHists
  {
    TProfile2D* h2EMCal{nullptr};
    TProfile2D* h2EMCalRetowered{nullptr};
    TProfile2D* h2IHCal{nullptr};
    TProfile2D* h2OHCal{nullptr};

    TH2* h2EMCalCent{nullptr};
    TH2* h2EMCalRetoweredCent{nullptr};
    TH2* h2IHCalCent{nullptr};
    TH2* h2OHCalCent{nullptr};

    TH2* h2EMCalZSCent{nullptr};
    TH2* h2IHCalZSCent{nullptr};
    TH2* h2OHCalZSCent{nullptr};

    TH2* h2EMCalNoZSCent{nullptr};
    TH2* h2IHCalNoZSCent{nullptr};
    TH2* h2OHCalNoZSCent{nullptr};

    TH2* h2CentralityTotalCaloE{nullptr};
  };

  AnalysisHists m_hists;

  struct EventData
  {
    double emcal_energy{0};  // NOLINT(misc-non-private-member-variables-in-classes)
    double ihcal_energy{0};  // NOLINT(misc-non-private-member-variables-in-classes)
    double ohcal_energy{0};  // NOLINT(misc-non-private-member-variables-in-classes)

    double centrality{0};    // NOLINT(misc-non-private-member-variables-in-classes)

    // Detailed Unsubtracted
    std::vector<int> emcal_base_tower_index;        // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> emcal_base_tower_energy;   // NOLINT(misc-non-private-member-variables-in-classes)

    std::vector<int> emcal_retower_tower_index;     // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> emcal_retower_tower_energy;// NOLINT(misc-non-private-member-variables-in-classes)

    std::vector<int> ihcal_tower_index;             // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> ihcal_tower_energy;        // NOLINT(misc-non-private-member-variables-in-classes)

    std::vector<int> ohcal_tower_index;             // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> ohcal_tower_energy;        // NOLINT(misc-non-private-member-variables-in-classes)

    // Detailed Iterative Subtracted
    std::vector<int> iter_emcal_tower_index;        // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> iter_emcal_tower_energy;   // NOLINT(misc-non-private-member-variables-in-classes)

    std::vector<int> iter_ihcal_tower_index;        // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> iter_ihcal_tower_energy;   // NOLINT(misc-non-private-member-variables-in-classes)

    std::vector<int> iter_ohcal_tower_index;        // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> iter_ohcal_tower_energy;   // NOLINT(misc-non-private-member-variables-in-classes)

    // Detailed Multiplicity Subtracted
    std::vector<int> mult_emcal_tower_index;        // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> mult_emcal_tower_energy;   // NOLINT(misc-non-private-member-variables-in-classes)

    std::vector<int> mult_ihcal_tower_index;        // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> mult_ihcal_tower_energy;   // NOLINT(misc-non-private-member-variables-in-classes)

    std::vector<int> mult_ohcal_tower_index;        // NOLINT(misc-non-private-member-variables-in-classes)
    std::vector<double> mult_ohcal_tower_energy;   // NOLINT(misc-non-private-member-variables-in-classes)

    void clear()
    {
      emcal_energy = 0;
      ihcal_energy = 0;
      ohcal_energy = 0;
      centrality = 0;

      emcal_base_tower_index.clear();
      emcal_base_tower_energy.clear();
      emcal_retower_tower_index.clear();
      emcal_retower_tower_energy.clear();
      ihcal_tower_index.clear();
      ihcal_tower_energy.clear();
      ohcal_tower_index.clear();
      ohcal_tower_energy.clear();

      iter_emcal_tower_index.clear();
      iter_emcal_tower_energy.clear();
      iter_ihcal_tower_index.clear();
      iter_ihcal_tower_energy.clear();
      iter_ohcal_tower_index.clear();
      iter_ohcal_tower_energy.clear();

      mult_emcal_tower_index.clear();
      mult_emcal_tower_energy.clear();
      mult_ihcal_tower_index.clear();
      mult_ihcal_tower_energy.clear();
      mult_ohcal_tower_index.clear();
      mult_ohcal_tower_energy.clear();
    }
  };

  EventData m_data;

  bool m_do_hists{true};
  bool m_do_detailed{false};
  bool m_do_iter{false};
  bool m_do_mult{false};
  bool m_do_retower{true};
  bool m_do_tree{true};
};

#endif  // CALOCHECK_H
