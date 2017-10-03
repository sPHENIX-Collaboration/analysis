#ifndef __Proto2ShowerCalib_H__
#define __Proto2ShowerCalib_H__

#include <fun4all/SubsysReco.h>
#include <TNtuple.h>
#include <TFile.h>
#include <string>
#include <stdint.h>
#include <fstream>

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

/// \class Proto2ShowerCalib
class Proto2ShowerCalib : public SubsysReco
{

public:

  Proto2ShowerCalib(const std::string &filename = "ana.root");
  virtual
  ~Proto2ShowerCalib();

  int
  Init(PHCompositeNode *topNode);
  int
  InitRun(PHCompositeNode *topNode);
  int
  process_event(PHCompositeNode *topNode);
  int
  End(PHCompositeNode *topNode);

  int
  LoadRecalibMap(const std::string & file);

  class Eval_Run : public TObject
  {
  public:
    Eval_Run()
    {
      reset();
    }
    virtual
    ~Eval_Run()
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
      EoP = -31454;
      valid_hodo_v = false;
      valid_hodo_h = false;
      trigger_veto_pass = false;
      good_temp = false;
      good_e = false;
      good_h = false;
    }

    int run;
    int event;
    float beam_mom;
    int hodo_h;
    int hodo_v;
    float C2_sum;
    float C1;
    float EoP;
    bool valid_hodo_v;
    bool valid_hodo_h;
    bool trigger_veto_pass;
    bool good_temp;
    bool good_e;
    bool good_h;   

  ClassDef(Eval_Run,2)
  };

  class HCAL_shower : public TObject
  {
   public:
    HCAL_shower()
    {
     reset();
    }

    virtual ~HCAL_shower(){}

    void reset()
    {
     emcal_e = 0.;
     hcalin_e = 0.;
     hcalout_e = 0.;
     sum_e = 0.;    
     
     emcal_e_recal = 0.;
     hcalin_e_recal = 0.;
     hcalout_e_recal = 0.;
     sum_e_recal = 0.;
     hcal_asym = -999;
    }


   float emcal_e;
   float hcalin_e;
   float hcalout_e;
   float sum_e;
  
   float emcal_e_recal;
   float hcalin_e_recal;
   float hcalout_e_recal;
   float hcal_asym;
   float sum_e_recal;
  };


  class Time_Samples : public TObject
  {
   public:
    Time_Samples(){ reset(); }
    virtual ~Time_Samples(){}

    void reset()
    {
     for(int itower=0; itower<16; itower++)
     {
      for(int isamp=0; isamp<24; isamp++)
      {
       hcalin_time_samples[itower][isamp] = -9999;
       hcalout_time_samples[itower][isamp] = -9999;
      }
     }

     for(int itower=0; itower<64; itower++)
     {
      for(int isamp=0; isamp<24; isamp++)
      {
       emcal_time_samples[itower][isamp] = -9999;
      }
     }
    }
   float hcalin_time_samples[16][24];
   float hcalout_time_samples[16][24];
   float emcal_time_samples[64][24];
  };

  void set_sim(const bool b)
  { _is_sim = b; }

  void fill_time_samples(const bool);

private:

  // calorimeter size
  enum
  {
    n_size_emcal = 8,
    n_size_hcalin = 4,
    n_size_hcalout = 4
  };

  Fun4AllHistoManager *
  get_HistoManager();

  std::string _filename;

  unsigned long _ievent;

  fstream fdata;

  Eval_Run _eval_run;

  HCAL_shower _shower;

  Time_Samples _time_samples;
 
  bool _is_sim;
  bool _fill_time_samples;
  std::map<std::pair<int, int>, double> _emcal_recalib_const;
  std::map<std::pair<int, int>, double> _hcalin_recalib_const;
  std::map<std::pair<int, int>, double> _hcalout_recalib_const;

};

#endif // __Proto2ShowerCalib_H__
