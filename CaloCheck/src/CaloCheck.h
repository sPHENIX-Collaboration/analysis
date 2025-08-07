// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALOCHECK_H
#define CALOCHECK_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <set>
#include <cstddef>
#include <memory>
#include <map>

// -- root
#include <TH1.h>

class PHCompositeNode;

class CaloCheck : public SubsysReco
{
 public:
  CaloCheck(const std::string &name = "CaloCheck");

  ~CaloCheck() override;

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

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  void set_output(const std::string &output)
  {
    m_output = output;
  }

  void set_event_list(const std::string &event_list)
  {
    m_event_list = event_list;
  }

 private:

  int read_event_list(const std::string &event_list);
  int process_event_check(PHCompositeNode *topNode);
  int process_calo(PHCompositeNode *topNode);

  std::string m_event_list = "";
  std::string m_output = "test.root";
  int m_event = 0;
  std::size_t m_processed_event = 0;
  std::string m_emcTowerNode = "TOWERS_CEMC";
  int m_globalEvent = -1;
  int m_run         = -1;

  std::set<int> m_events;
  std::map<std::string, std::unique_ptr<TH1>> m_hists;
};

#endif  // CALOCHECK_H
