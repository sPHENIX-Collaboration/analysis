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

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

 private:

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

    TH1* hCentrality{nullptr};
    TH2* h2CentralityTotalCaloE{nullptr};
    TH1* hZVertex{nullptr};
  };

  AnalysisHists m_hists;

  // Cuts
  struct Cuts
  {
    double m_zvtx_max{10}; /*cm*/
    double m_cent_max{80};
  };

  Cuts m_cuts;

  // Event Info
  struct EventData
  {
    int event_id{0};
    double centrality{9999};
  };

  EventData m_data;

  int m_event{0};

  static constexpr int PROGRESS_PRINT_INTERVAL = 20;

  int process_event_check(PHCompositeNode *topNode);
  int process_centrality(PHCompositeNode *topNode);
  int process_calo(PHCompositeNode *topNode) const;
};

#endif  // CALOCHECK_H
