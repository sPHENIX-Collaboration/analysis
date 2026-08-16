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
    double emcal_energy{0};
    double ihcal_energy{0};
    double ohcal_energy{0};

    double centrality{0};
  };

  EventData m_data;

  bool m_do_hists{true};
};

#endif  // CALOCHECK_H
