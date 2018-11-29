#ifndef __Proto4TowerCalib_H__
#define __Proto4TowerCalib_H__

#include <TFile.h>
#include <TNtuple.h>
#include <fun4all/SubsysReco.h>
#include <stdint.h>
#include <fstream>
#include <string>

class PHCompositeNode;
class PHG4HitContainer;
class Fun4AllHistoManager;
class TH1F;
class TTree;
class TChain;
class SvtxEvalStack;
class PHG4Particle;
class RawTowerGeom;
class RawTowerContainer;
class SvtxTrack;

/// \class Proto4TowerCalib to help you get started
class Proto4TowerCalib : public SubsysReco
{
 public:
  //! constructor
  Proto4TowerCalib(const std::string &filename = "Proto4TowerCalib.root");

  //! destructor
  virtual ~Proto4TowerCalib();

  //! Standard function called at initialization
  int Init(PHCompositeNode *topNode);

  //! Standard function called when a new run is processed
  int InitRun(PHCompositeNode *topNode);

  //! Standard function called at each event
  int process_event(PHCompositeNode *topNode);

  //! Standard function called at the end of processing. Save your stuff here.
  int End(PHCompositeNode *topNode);

  //! Is processing simulation files?
  void is_sim(bool b)
  {
    _is_sim = b;
  }

  // TowerCalib Analysis
  int InitAna();

  int MakeAna();

  int FinishAna();

  void set_det(std::string detector)
  {
    _mDet = detector;
  }

  void set_colID(int colID)
  {
    _mCol = colID;
  }


  class HCAL_Tower : public TObject
  {
   public:
    HCAL_Tower()
    {
     reset();
    }

    virtual ~HCAL_Tower(){}

    void reset()
    {
     // HCALIN
     hcalin_e_sim = 0.;

     hcalin_lg_e_raw = 0.;
     hcalin_lg_e_calib = 0.;

     for(int itwr=0; itwr<16; itwr++)
     {
      hcalin_twr_sim[itwr] = 0.;
      hcalin_lg_twr_raw[itwr] = 0.;
      hcalin_lg_twr_calib[itwr] = 0.;
     }

     // HCALOUT
     hcalout_e_sim = 0.;
     
     hcalout_lg_e_raw = 0.;
     hcalout_lg_e_calib = 0.;

     hcalout_hg_e_raw = 0.;
     hcalout_hg_e_calib = 0.;

     for(int itwr=0; itwr<16; itwr++)
     {
      hcalout_twr_sim[itwr] = 0.;

      hcalout_lg_twr_raw[itwr] = 0.;
      hcalout_lg_twr_calib[itwr] = 0.;

      hcalout_hg_twr_raw[itwr] = 0.;
      hcalout_hg_twr_calib[itwr] = 0.;
     }

     // total energy and asymmetry
     hcal_total_sim = -999.;
     hcal_total_raw = -999.;
     hcal_total_calib = -999.;

     hcal_asym_sim = -999.;
     hcal_asym_raw = -999.;
     hcal_asym_calib = -999.;
    }


   // HCALIN
   float hcalin_e_sim;
   float hcalin_twr_sim[16];

   float hcalin_lg_e_raw;
   float hcalin_lg_twr_raw[16];

   float hcalin_lg_e_calib;
   float hcalin_lg_twr_calib[16];

   // HCALOUT
   float hcalout_e_sim;
   float hcalout_twr_sim[16];

   float hcalout_lg_e_raw;
   float hcalout_lg_twr_raw[16];

   float hcalout_lg_e_calib;
   float hcalout_lg_twr_calib[16];

   float hcalout_hg_e_raw;
   float hcalout_hg_twr_raw[16];

   float hcalout_hg_e_calib;
   float hcalout_hg_twr_calib[16];

   // total energy and asymmetry
   float hcal_total_sim;
   float hcal_total_raw;
   float hcal_total_calib;

   float hcal_asym_sim;
   float hcal_asym_raw;
   float hcal_asym_calib;

   ClassDef(HCAL_Tower, 10)
  };

 private:
  // calorimeter size
  enum
  {
    n_size = 8
  };

  //! is processing simulation files?
  bool _is_sim;

  //! get manager of histograms
  Fun4AllHistoManager *
  get_HistoManager();

  std::pair<int, int>
  find_max(RawTowerContainer *towers, int cluster_size);

  int getChannelNumber(int column, int row);

  //! output root file name
  std::string _filename;

  //! simple event counter
  unsigned long _ievent;

  //! hcal infromation. To be copied to output TTree T
  HCAL_Tower _tower;

  // TowerCalib Analysis
  TFile *mFile_OutPut;
  TChain *mChainInPut;
  unsigned long _mStartEvent;
  unsigned long _mStopEvent;
  int _mInPut_flag;
  std::string _mList;
  std::string _mDet;
  int _mCol;
  float _mPedestal;

  HCAL_Tower *_mTower;

  TH1F *h_mHCAL[16];
  float hcal_twr[16];
  bool _is_sig[16];

  bool is_sig(int colID);
  int fill_sig(int colID);
  int reset_pedestal();
};

#endif  // __Proto4TowerCalib_H__
