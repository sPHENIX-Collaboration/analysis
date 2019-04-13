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
class TH2F;
class TTree;
class TChain;
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

  // ShowerCalib Analysis
  int InitAna();

  int MakeAna();

  int FinishAna();

  void set_runID(std::string runID)
  {
    _mRunID = runID;
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

  //! output root file name
  std::string _filename;

  //! simple event counter
  unsigned long _ievent;

  //! run infomation. To be copied to output TTree T
  Eval_Run _eval_run;

  //! hcal infromation. To be copied to output TTree T
  HCAL_Tower _tower;

  // TowerCalib Analysis
  TFile *mFile_OutPut;
  TChain *mChainInPut;
  unsigned long _mStartEvent;
  unsigned long _mStopEvent;
  int _mInPut_flag;
  std::string _mList;
  std::string _mRunID;

  Eval_Run *_mInfo;
  HCAL_Tower *_mTower;

  // TH2F *h_mAsymmEnergy_mixed_sim_wo_cut; // sim
  TH2F *h_mAsymmEnergy_pion_sim_wo_cut;

  // TH2F *h_mAsymmEnergy_mixed_sim;
  TH2F *h_mAsymmEnergy_pion_sim;

  TH2F *h_mAsymmAdc_mixed; // production
  TH2F *h_mAsymmAdc_electron;
  TH2F *h_mAsymmAdc_pion;

  TH2F *h_mAsymmEnergy_mixed_wo_cut;
  TH2F *h_mAsymmEnergy_electron_wo_cut;
  TH2F *h_mAsymmEnergy_pion_wo_cut;

  TH2F *h_mAsymmEnergy_mixed; // MIP study
  TH2F *h_mAsymmEnergy_electron;
  TH2F *h_mAsymmEnergy_pion;

  // balancing
  TH2F *h_mAsymmEnergy_mixed_balancing;
  TH2F *h_mAsymmEnergy_electron_balancing;
  TH2F *h_mAsymmEnergy_pion_balancing;

  // leveling correction
  TH2F *h_mAsymmEnergy_mixed_leveling;
  TH2F *h_mAsymmEnergy_electron_leveling;
  TH2F *h_mAsymmEnergy_pion_leveling;

  // shower calib
  TH2F *h_mAsymmEnergy_mixed_ShowerCalib;
  TH2F *h_mAsymmEnergy_electron_ShowerCalib;
  TH2F *h_mAsymmEnergy_pion_ShowerCalib;

  // Outer HCal only study
  TH2F *h_mAsymmEnergy_mixed_MIP;
  TH1F *h_mEnergyOut_electron; // hadron MIP through inner HCal
  TH1F *h_mEnergyOut_pion;
  TH1F *h_mEnergyOut_electron_ShowerCalib;
  TH1F *h_mEnergyOut_pion_ShowerCalib;

  int getChannelNumber(int column, int row);
  int setTowerCalibParas();

  // correction factors
  const double samplefrac_in = 0.09267; 
  const double samplefrac_out = 0.02862;

  // inner HCAL MIP energy extracted from muon
  const double MIP_mean  = 0.654927;
  const double MIP_width = 0.151484;

  // const double samplefrac_in = 0.0631283;  // from Songkyo
  // const double samplefrac_out = 0.0338021;

  double towercalib_lg_in[16];
  double towercalib_lg_out[16];
  double towercalib_hg_out[16];

  // const double showercalib = 3.03185; // extracted with 12 GeV Test Beam Data
  const double showercalib = 2.92243; // extracted with 8 GeV Test Beam Data
  const double showercalib_ohcal = 3.37511; // extracted with 8 GeV Test Beam Data

  float find_range();
  int find_energy();

  // used by RAW
};

#endif  // __Proto4ShowerCalib_H__
