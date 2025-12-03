// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef EVENTSKIP_H
#define EVENTSKIP_H

#include <fun4all/SubsysReco.h>

// -- c++
#include <string>

class PHCompositeNode;

class EventSkip : public SubsysReco
{
 public:
  explicit EventSkip(const std::string &name = "EventSkip");

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

  void set_skip(int nSkip = 0)
  {
    m_nSkip = nSkip;
  }

  void set_event_id(int event_id)
  {
    m_event_id = event_id;
  }

 private:
  int m_event{0};
  int m_nSkip{0};
  int m_event_id{0};
};

#endif  // EVENTSKIP_H
