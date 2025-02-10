// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef EVENTCHECK_H
#define EVENTCHECK_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

#include <calotrigger/TriggerAnalyzer.h>

// Jet Utils
#include "JetUtils.h"

using std::string;
using std::vector;
using std::pair;

class PHCompositeNode;

class EventCheck : public SubsysReco
{
 public:
  EventCheck();

  ~EventCheck() override;

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

  void set_zvtx_max(float m_zvtx_max) {
    this->m_zvtx_max = m_zvtx_max;
  }

  void set_trigger(int m_triggerBit) {
    this->m_triggerBit = m_triggerBit;
  }

  void set_eventList(const string &m_eventList) {
    this->m_eventList = m_eventList;
    m_doSpecificEvents = !JetUtils::readEventList(m_eventList, m_eventList_vec, 0, true);
  }

 private:

  int m_triggerBit;
  float m_zvtx_max;
  bool m_doSpecificEvents;
  string m_eventList;

  vector<pair<int, int>> m_eventList_vec;

  TriggerAnalyzer* m_triggeranalyzer = nullptr;
};

#endif  // EVENTCHECK_H
