#ifndef __Proto4ShowerCalib_H__
#define __Proto4ShowerCalib_H__

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
class SvtxEvalStack;
class PHG4Particle;
class RawTowerGeom;
class RawTowerContainer;
class SvtxTrack;

/// \class Proto4ShowerCalib to help you get started
class Proto4ShowerCalib : public SubsysReco
{
 public:
  //! constructor
  Proto4ShowerCalib(const std::string &filename = "Proto4ShowerCalib.root");

  //! destructor
  virtual ~Proto4ShowerCalib();

  //! Standard function called at initialization
  int Init(PHCompositeNode *topNode);

  //! Standard function called when a new run is processed
  int InitRun(PHCompositeNode *topNode);

  //! Standard function called at each event
  int process_event(PHCompositeNode *topNode);

  //! Standard function called at the end of processing. Save your stuff here.
  int End(PHCompositeNode *topNode);

  //! Is processing simulation files?
  void
  is_sim(bool b)
  {
    _is_sim = b;
  }

  class Eval_Run : public TObject
  {
   public:
    Eval_Run()
    {
      reset();
    }
    virtual ~Eval_Run()
    {
    }

    void
    reset()
    {
      run = -31454;
      event = -31454;
      beam_mom = -0;
      hodo_h = -31454;
      hodo_v = -31454;
      C2_sum = -31454;
      C1 = -31454;

      valid_hodo_v = false;
      valid_hodo_h = false;
      trigger_veto_pass = false;
      good_e = false;
      good_anti_e = false;

      beam_2CH_mm = -31454;
      beam_2CV_mm = -31454;

      truth_y = -31454;
      truth_z = -31454;

      sum_E_CEMC = -31454;
      sum_E_HCAL_OUT = -31454;
      sum_E_HCAL_IN = -31454;
    }

    int run;
    int event;

    //! beam momentum with beam charge
    float beam_mom;

    //! hodoscope index
    int hodo_h;
    int hodo_v;

    //! Cherenkov sums
    float C2_sum;
    float C1;

    //! has valid hodoscope?
    bool valid_hodo_v;
    bool valid_hodo_h;

    //! has valid veto counter?
    bool trigger_veto_pass;

    //! Good electrons?
    bool good_e;

    //! Good hadron and muons?
    bool good_anti_e;

    //! 2C motion table positions
    float beam_2CH_mm;
    float beam_2CV_mm;

    //! Turth beam position. Simulation only.
    float truth_y;
    float truth_z;

    //! Sum energy of all towers
    double sum_E_CEMC;
    double sum_E_HCAL_OUT;
    double sum_E_HCAL_IN;

    ClassDef(Eval_Run, 10)
  };

  class HCAL_Shower : public TObject
  {
   public:
    HCAL_Shower()
    {
     reset();
    }

    virtual ~HCAL_Shower(){}

    void reset()
    {
     for(int itwr=0; itwr<16; itwr++)
     {
      hcalin_lg_twr_raw[itwr] = 0.;
      hcalout_lg_twr_raw[itwr] = 0.;
      hcalout_hg_twr_raw[itwr] = 0.;
      hcalin_lg_twr_calib[itwr] = 0.;
      hcalout_lg_twr_calib[itwr] = 0.;
      hcalout_hg_twr_calib[itwr] = 0.;
     }

     hcalin_lg_e_raw = 0.;
     hcalout_lg_e_raw = 0.;
     hcalout_hg_e_raw = 0.;
     hcal_total_raw = -999.;
     hcal_asym_raw = -999.;

     hcalin_lg_e_calib = 0.;
     hcalout_lg_e_calib = 0.;
     hcalout_hg_e_calib = 0.;
     hcal_total_calib = -999.;
     hcal_asym_calib = -999.;
    }

   float hcalin_lg_e_raw;
   float hcalin_lg_twr_raw[16];
   float hcalout_lg_e_raw;
   float hcalout_lg_twr_raw[16];
   float hcalout_hg_e_raw;
   float hcalout_hg_twr_raw[16];
   float hcal_total_raw;
   float hcal_asym_raw;

   float hcalin_lg_e_calib;
   float hcalin_lg_twr_calib[16];
   float hcalout_lg_e_calib;
   float hcalout_lg_twr_calib[16];
   float hcalout_hg_e_calib;
   float hcalout_hg_twr_calib[16];
   float hcal_total_calib;
   float hcal_asym_calib;

   ClassDef(HCAL_Shower, 10)
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

  //! output root file name
  std::string _filename;

  //! simple event counter
  unsigned long _ievent;

  //! run infomation. To be copied to output TTree T
  Eval_Run _eval_run;

  //! hcal infromation. To be copied to output TTree T
  HCAL_Shower _shower;
};

#endif  // __Proto4ShowerCalib_H__
