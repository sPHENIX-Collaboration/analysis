#pragma once

#include <fun4all/SubsysReco.h>

#include <map>
#include <string>
#include <vector>

class PHCompositeNode;
class TH1;
class TH2;
class TriggerAnalyzer;

class CentralityQA : public SubsysReco
{
 public:
  explicit CentralityQA(const std::string &name = "CentralityQA");

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

 private:
  int process_event_check(PHCompositeNode *topNode);
  int process_centrality(PHCompositeNode *topNode);

  // Configuration
  unsigned int m_bins_cent{100};
  double m_cent_low{-0.5};
  double m_cent_high{99.5};
        
  double m_cent_max_threshold_ana{60};

  double m_zvtx_max{10};     // cm
  double m_zvtx_max_v2{50};  // cm

  // Event Variables
  double m_cent{0};
  double m_zvtx{0};
  int m_globalEvent{0};
  std::map<std::string, int> m_ctr;

  TriggerAnalyzer* triggeranalyzer{nullptr};

  enum class EventType : std::uint8_t
  {
    ALL,
    ZVTX,
    ZVTX50,
    ZVTX10,
    ZVTX10_MB,
    ZVTX10_MB_CENT,
    TRIG10 = 1,
    TRIG12 = 2,
    TRIG14 = 3
  };

  std::vector<std::string> m_triggernames = {"MBD N&S >= 2",
                                             "MBD N&S >= 2, vtx < 10 cm",
                                             "MBD N&S >= 2, vtx < 150 cm"};

  // Event Selection Flags
  bool m_pass_MB{false};
  bool m_pass_Zvtx{false};

  // Histograms
  TH1 *hEvent{nullptr};
  TH1 *hEventTrigger{nullptr};
  TH1 *hZVertex{nullptr};
  TH1 *hCentrality{nullptr};
  TH1 *hCentralityZ50{nullptr};
  TH1 *hCentralityZOuter{nullptr};
  TH2 *h2ZVertexCentrality{nullptr};

  // Trigger
  std::vector<TH1*> hZVertexTrig;
  std::vector<TH1*> hCentralityTrig;
  std::vector<TH1*> hCentralityZ50Trig;
  std::vector<TH1*> hCentralityZOuterTrig;
  std::vector<TH2*> h2ZVertexCentralityTrig;
};
