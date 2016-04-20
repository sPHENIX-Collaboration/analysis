#ifndef __CALOEVALUATOR_H__
#define __CALOEVALUATOR_H__

#include <fun4all/SubsysReco.h>
#include <phool/PHCompositeNode.h>
#include <g4hough/PHG4HoughTransform.h>

#include <TNtuple.h>
#include <TFile.h>
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
    kProcessTower = 1 << 1
  }
   

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

  // SvtxEvalStack * _eval_stack;
  TTree * _T_EMCalTrk;


 
  std::string _filename;

  unsigned long _ievent;


};

#endif // __CALOEVALUATOR_H__
