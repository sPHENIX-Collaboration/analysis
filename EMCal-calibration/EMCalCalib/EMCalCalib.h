#ifndef __CALOEVALUATOR_H__
#define __CALOEVALUATOR_H__

#include <fun4all/SubsysReco.h>
#include <phool/PHCompositeNode.h>
#include <g4hough/PHG4HoughTransform.h>

#include <TNtuple.h>
#include <TFile.h>

#include "MCPhoton.h"

#include <string>
#include <stdint.h>

class PHCompositeNode;
class PHG4HitContainer;
class Fun4AllHistoManager;
class TH1F;
class TTree;
class SvtxEvalStack;
class PHG4Particle;
class RawTowerGeom;
class RawTowerContainer;
class SvtxTrack;

/// \class EMCalCalib
class EMCalCalib : public SubsysReco
{

public:

  enum enu_flags
  {
    kProcessSF = 1 << 1,
    kProcessTower = 1 << 2,
    kProcessMCPhoton = 1 << 3,
    kProcessUpslisonTrig = 1 << 4,

    kDefaultFlag = kProcessSF | kProcessTower
  };

  EMCalCalib(const std::string &filename = "cemc_ana.root", enu_flags flags =
      kDefaultFlag);
  virtual
  ~EMCalCalib();

  int
  Init(PHCompositeNode *topNode);
  int
  InitRun(PHCompositeNode *topNode);
  int
  process_event(PHCompositeNode *topNode);
  int
  End(PHCompositeNode *topNode);

  uint32_t
  get_flags() const
  {
    return _flags;
  }

  void
  set_flags(enu_flags flags)
  {
    _flags = (uint32_t) flags;
  }

  void
  set_flag(enu_flags flag)
  {
    _flags |= (uint32_t) flag;
  }

  bool
  flag(enu_flags flag)
  {
    return _flags & flag;
  }

  void
  reset_flag(enu_flags flag)
  {
    _flags &= ~(uint32_t) flag;
  }

  float
  get_mag_field() const
  {
    return _magfield;
  }
  void
  set_mag_field(float magfield)
  {
    _magfield = magfield;
  }

private:

  Fun4AllHistoManager *
  get_HistoManager();

  int
  Init_SF(PHCompositeNode *topNode);
  int
  process_event_SF(PHCompositeNode *topNode);

  int
  Init_Tower(PHCompositeNode *topNode);
  int
  process_event_Tower(PHCompositeNode *topNode);

  int
  Init_MCPhoton(PHCompositeNode *topNode);
  int
  process_event_MCPhoton(PHCompositeNode *topNode);

  int
  Init_UpslisonTrig(PHCompositeNode *topNode);
  int
  process_event_UpslisonTrig(PHCompositeNode *topNode);

  //! project a photon to calorimeters and collect towers around it
  void
  eval_photon(PHG4Particle * primary_particle, MCPhoton & trk,
      PHCompositeNode *topNode);

  enum enu_calo
  {
    kCEMC, kHCALIN, kHCALOUT
  };

  void eval_photon_proj(//
      PHG4Particle * g4particle, //
      MCPhoton & trk,
      enu_calo calo_id,//
      const double gvz,
      PHCompositeNode *topNode//
      );

  SvtxEvalStack * _eval_stack;
  TTree * _T_EMCalTrk;
  MCPhoton * _mc_photon;

  double _magfield;
  PHG4HoughTransform _hough;

  std::string _filename;

  uint32_t _flags;
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
