#ifndef FINDREJECTS_H
#define FINDREJECTS_H

#include <fun4all/SubsysReco.h>

#include <string>

// Forward declarations
class TriggerAnalyzer;
class TNtuple;
class PHCompositeNode;
class TH2F;
class Fun4AllHistoManager;
class TrashInfo;
class FindRejects : public SubsysReco
{
 public:
  //! constructor
  FindRejects(const std::string& name = "FindRejects", const std::string& outfile = "testQA.root"); //int nevents = 100);

  //! destructor
  ~FindRejects() override;

  //! full initialization
  int Init(PHCompositeNode*) override;

  //! event processing method
  int process_event(PHCompositeNode*) override;

  //! end of run method
  int End(PHCompositeNode*) override;

 private:

  Fun4AllHistoManager *hm{nullptr};
  TNtuple *tn{nullptr};
  TH2F* h_emcal{nullptr};
  TH2F* h_hcalout{nullptr};
  TH2F* h_hcalout_time{nullptr};

  int _eventcounter{0};
  int _eventcount{0};
  int _range{1};
  std::string m_outfilename;
  float m_maxcut = 15.;
  TriggerAnalyzer *m_triggeranalyzer{nullptr};
};

#endif
