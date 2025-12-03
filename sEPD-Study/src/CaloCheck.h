// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALOCHECK_H
#define CALOCHECK_H

#include <fun4all/SubsysReco.h>

// ====================================================================
// ROOT Includes
// ====================================================================
#include <TH1.h>
#include <TH2.h>

// -- c++
#include <string>
#include <map>
#include <memory>
#include <cstdint>

class PHCompositeNode;

class CaloCheck : public SubsysReco
{
 public:
  explicit CaloCheck(const std::string &name = "CaloCheck");

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

  void set_event_id(int event_id)
  {
    m_event_id = event_id;
  }

 private:
  struct AnalysisHists
  {
    TH2* h2EMCalBase{nullptr};
    TH2* h2EMCal{nullptr};
    TH2* h2IHCal{nullptr};
    TH2* h2OHCal{nullptr};
    TH1* hEMCal{nullptr};
    TH1* hIHCal{nullptr};
    TH1* hOHCal{nullptr};
  };

  AnalysisHists m_hists;

  uint64_t m_run{0};
  int m_event_id{0};

  std::map<std::string, std::unique_ptr<TH1>> m_hists1D;
  std::map<std::string, std::unique_ptr<TH2>> m_hists2D;
};

#endif  // CALOCHECK_H
