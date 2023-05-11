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

  TTree *test1;
  TNtuple *NtupleAll;
  Float_t Etotal_t;
  Float_t Emax_t;
  Float_t E3by3_t;
  Float_t E5by5_t;
  Float_t C1_t;
  // Float_t C2_t;
   Float_t C2_inner_t;
   Float_t C2_outer_t;
   Float_t C2_inner_new_t;
   Float_t C2_outer_new_t;
  //  Float_t C2_inner_beforerun2210_t;
  // Float_t C2_outer_beforerun2210_t;
  Float_t Veto1_t;
  Float_t Veto2_t;
  Float_t Veto3_t;
  Float_t Veto4_t;
  Float_t Horz_HODO_R0_t;
  Float_t Horz_HODO_R1_t;
  Float_t Horz_HODO_R2_t;
  Float_t Horz_HODO_R3_t;
  Float_t Horz_HODO_R4_t;
  Float_t Horz_HODO_R5_t;
  Float_t Horz_HODO_R6_t;
  Float_t Horz_HODO_R7_t;
  Float_t Vert_HODO_R0_t;
  Float_t Vert_HODO_R1_t;
  Float_t Vert_HODO_R2_t;
  Float_t Vert_HODO_R3_t;
  Float_t Vert_HODO_R4_t;
  Float_t Vert_HODO_R5_t;
  Float_t Vert_HODO_R6_t;
  Float_t Vert_HODO_R7_t;
  Float_t TowerID_t;
  Float_t Tower_column_t;
  Float_t Tower_row_t;
  Float_t TowerE_column_0_t;
  Float_t TowerE_column_1_t;
  Float_t TowerE_column_2_t;
  Float_t TowerE_column_3_t;
  Float_t TowerE_column_4_t;
  Float_t TowerE_column_5_t;
  Float_t TowerE_column_6_t;
  Float_t TowerE_column_7_t;
  Float_t TowerE_row_0_t;
  Float_t TowerE_row_1_t;
  Float_t TowerE_row_2_t;
  Float_t TowerE_row_3_t;
  Float_t TowerE_row_4_t;
  Float_t TowerE_row_5_t;
  Float_t TowerE_row_6_t;
  Float_t TowerE_row_7_t;
  Float_t HorzTowerID_t;
  Float_t VertTowerID_t;
  Float_t SaveHoriz_TowerID0_t;
  Float_t SaveHoriz_TowerID1_t;
  Float_t SaveHoriz_TowerID2_t;
  Float_t SaveHoriz_TowerID3_t;
  Float_t SaveHoriz_TowerID4_t;
  Float_t SaveHoriz_TowerID5_t;
  Float_t SaveHoriz_TowerID6_t;
  Float_t SaveHoriz_TowerID7_t;

  Float_t SaveVert_TowerID0_t;
  Float_t SaveVert_TowerID1_t;
  Float_t SaveVert_TowerID2_t;
  Float_t SaveVert_TowerID3_t;
  Float_t SaveVert_TowerID4_t;
  Float_t SaveVert_TowerID5_t;
  Float_t SaveVert_TowerID6_t;
  Float_t SaveVert_TowerID7_t;
  

  EMCalCalib_TestBeam(const std::string &filename = "cemc_ana.root");
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
