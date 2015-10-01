#ifndef __CALOEVALUATOR_H__
#define __CALOEVALUATOR_H__

#include <fun4all/SubsysReco.h>
#include <phool/PHCompositeNode.h>

#include <TNtuple.h>
#include <TFile.h>

#include "EMCalTrk.h"

#include <string>

class PHCompositeNode;
class PHG4HitContainer;
class Fun4AllHistoManager;
class TH1F;
class TTree;

/// \class EMCalAna
class EMCalAna : public SubsysReco
{

public:

  EMCalAna( const std::string &filename = "cemc_ana.root", enu_flags flags = kDefaultFlag);
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


  enum enu_flags
  {
    kProcessSF = 1<<1,
    kProcessTower = 1<<2,
    kProcessTrk = 1<<3,

    kDefaultFlag = kProcessSF | kProcessTower
  };

  enu_flags
  get_flags() const
  {
    return _flags;
  }

  void
  set_flags(enu_flags flags)
  {
    _flags = flags;
  }

  void
  set_flag(enu_flags flag)
  {
    _flags |= flag;
  }

  bool
  flag(enu_flags flag)
  {
    return _flags & flag;
  }

  void
  reset_flag(enu_flags flag)
  {
    _flags &= not flag;
  }


private:

  Fun4AllHistoManager * get_HistoManager();

  int
  Init_SF(PHCompositeNode *topNode);
  int
  process_event_SF(PHCompositeNode *topNode);

  int
  Init_Tower(PHCompositeNode *topNode);
  int
  process_event_Tower(PHCompositeNode *topNode);

  int
  Init_Trk(PHCompositeNode *topNode);
  int
  process_event_Trk(PHCompositeNode *topNode);

  TTree * _T_EMCalTrk;
  EMCalTrk _trk;

  std::string _filename;

  enu_flags _flags;
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
