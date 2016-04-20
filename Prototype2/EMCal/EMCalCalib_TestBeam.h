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
class EMCalCalib_TestBeam : public SubsysReco
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

  EMCalCalib_TestBeam(const std::string &filename = "cemc_ana.root", enu_flags flags =
      kDefaultFlag);
  virtual
  ~EMCalCalib_TestBeam();

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

 

private:

  Fun4AllHistoManager *
  get_HistoManager();

 
  int
  Init_Tower(PHCompositeNode *topNode);
  int
  process_event_Tower(PHCompositeNode *topNode);


 
  enum enu_calo
  {
    kCEMC, kHCALIN, kHCALOUT
  };

  SvtxEvalStack * _eval_stack;
  TTree * _T_EMCalTrk;


 
  std::string _filename;

  uint32_t _flags;
  unsigned long _ievent;


};

#endif // __CALOEVALUATOR_H__
