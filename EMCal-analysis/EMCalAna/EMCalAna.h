#ifndef __CALOEVALUATOR_H__
#define __CALOEVALUATOR_H__

#include <fun4all/SubsysReco.h>
#include <phool/PHCompositeNode.h>

#include <TNtuple.h>
#include <TFile.h>

#include <string>

class PHCompositeNode;
class PHG4HitContainer;
class TH1F;

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

  int
  Init(PHCompositeNode *topNode);
  int
  InitRun(PHCompositeNode *topNode);
  int
  process_event(PHCompositeNode *topNode);
  int
  End(PHCompositeNode *topNode);

private:

  std::string _caloname;

  unsigned long _ievent;

  PHG4HitContainer* _hcalout_hit_container;
  PHG4HitContainer* _hcalin_hit_container;
  PHG4HitContainer* _cemc_hit_container;
  PHG4HitContainer* _hcalout_abs_hit_container;
  PHG4HitContainer* _hcalin_abs_hit_container;
  PHG4HitContainer* _cemc_abs_hit_container;
  PHG4HitContainer* _magnet_hit_container;
  PHG4HitContainer* _bh_hit_container;
  PHG4HitContainer* _bh_plus_hit_container;
  PHG4HitContainer* _bh_minus_hit_container;
  PHG4HitContainer* _cemc_electronics_hit_container;
  PHG4HitContainer* _hcalin_spt_hit_container;
  PHG4HitContainer* _svtx_hit_container;
  PHG4HitContainer* _svtx_support_hit_container;
};

#endif // __CALOEVALUATOR_H__
