#pragma once

#include <fun4all/SubsysReco.h>

#include <map>
#include <string>
#include <vector>
#include <memory>

class PHCompositeNode;
class TH1;
class TH2;
class TriggerAnalyzer;

class TriggerQA : public SubsysReco
{
 public:
  explicit TriggerQA(const std::string &name = "TriggerQA");
  ~TriggerQA() override;

  // -- Rule of Five: explicitly delete copy and move semantics --
  TriggerQA(const TriggerQA &) = delete;                 // Delete copy constructor
  TriggerQA &operator=(const TriggerQA &) = delete;      // Delete copy assignment
  TriggerQA(TriggerQA &&) = delete;                      // Delete move constructor
  TriggerQA &operator=(TriggerQA &&) = delete;           // Delete move assignment

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

 private:
  // Configuration
  double m_zvtx_max{10}; // cm

  // Event Variables
  int m_total_events{0};

  std::unique_ptr<TriggerAnalyzer> m_triggerAnalyzer;

  enum class EventType : std::uint8_t
  {
    ALL,
    ZVTX,
    ZVTX10,
    TRIG10,
    TRIG12,
    TRIG14,
    ZVTX10_TRIG10,
    ZVTX10_TRIG12,
    ZVTX10_TRIG14
  };

  const std::string m_trig_10 = "MBD N&S >= 2";
  const std::string m_trig_12 = "MBD N&S >= 2, vtx < 10 cm";
  const std::string m_trig_14 = "MBD N&S >= 2, vtx < 150 cm";

  // Histogram
  TH1 *hEvent{nullptr};
  TH1 *hZVertex{nullptr};
  TH1 *hZVertex_Trig10{nullptr};
  TH1 *hZVertex_Trig12{nullptr};
  TH1 *hZVertex_Trig14{nullptr};
  TH1 *hLuminosity{nullptr};
};
