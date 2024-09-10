#ifndef CALOVALID_CALOVALID_H
#define CALOVALID_CALOVALID_H

#include <fun4all/SubsysReco.h>

#include <string>

// Forward declarations
class TNtuple;
class PHCompositeNode;
class TH2F;
class Fun4AllHistoManager;
class TrashInfo;
class FindOutlier : public SubsysReco
{
 public:
  //! constructor
  FindOutlier(const std::string& name = "FindOutlier", const std::string& outfile = "testQA.root"); //int nevents = 100);

  //! destructor
  ~FindOutlier() override;

  //! full initialization
  int Init(PHCompositeNode*) override;

  //! event processing method
  int process_event(PHCompositeNode*) override;

  //! end of run method
  int End(PHCompositeNode*) override;

 private:

  Fun4AllHistoManager *hm{nullptr};
  TNtuple *tn{nullptr};
  TrashInfo *trashinfo;
  TH2F* h_emcal{nullptr};
  TH2F* h_hcalout{nullptr};
  TH2F* h_hcalout_time{nullptr};
  int _eventcounter{0};
  int _eventcount{0};
  int _range{1};
  std::string m_outfilename;
  float max_cut = 15.;
  bool useWave{false};
};

#endif
