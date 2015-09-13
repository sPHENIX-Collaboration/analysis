#ifndef __CALOEVALUATOR_H__
#define __CALOEVALUATOR_H__

#include <fun4all/SubsysReco.h>
#include <phool/PHCompositeNode.h>

#include <TNtuple.h>
#include <TFile.h>

#include <string>

/// \class EMCalAna
class EMCalAna : public SubsysReco
{

public:

  EMCalAna(const std::string &name = "EMCalAna", const std::string &caloname =
      "CEMC", const std::string &filename = "cemc_ana.root");
  virtual
  ~EMCalAna()
  {
  }
  ;

  int
  Init(PHCompositeNode *topNode);
  int
  process_event(PHCompositeNode *topNode);
  int
  End(PHCompositeNode *topNode);

private:

  std::string _caloname;

  unsigned long _ievent;

};

#endif // __CALOEVALUATOR_H__
