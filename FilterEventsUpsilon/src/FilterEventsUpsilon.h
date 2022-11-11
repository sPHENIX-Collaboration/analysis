
#ifndef __FILTEREVENTSUPSILON_H__
#define __FILTEREVENTSUPSILON_H__

#include <fun4all/SubsysReco.h>

class PairMaker: public SubsysReco {

public:

  FilterEventsUpsilon(const std::string &name = "FilterEventsUpsilon");
  virtual ~FilterEventsUpsilon() {}

  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

protected:

  std::string outnodename;
  int EventNumber;

};

#endif

