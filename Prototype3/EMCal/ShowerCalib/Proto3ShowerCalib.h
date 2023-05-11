#ifndef __Proto3ShowerCalib_H__
#define __Proto3ShowerCalib_H__

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

/// \class Proto3ShowerCalib
class Proto3ShowerCalib : public SubsysReco
{

public:

  Proto3ShowerCalib(const std::string &filename = "cemc_ana.root");
  virtual
  ~Proto3ShowerCalib();

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

  void
  is_sim(bool b) {_is_sim = b;}

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

      sum_energy_T = -31454;
      EoP = -31454;
      truth_y = -31454;
      truth_z = -31454;

      valid_hodo_v = false;
      valid_hodo_h = false;
      trigger_veto_pass = false;
      good_temp = false;
      good_e = false;
      good_data = false;
    }

    int run;
    int event;

    float beam_mom;

    int hodo_h;
    int hodo_v;

    float C2_sum;

    //! sum tower energy after temp correction
    float sum_energy_T;
    //! sum tower E over P after temp correction
    float EoP;

    bool valid_hodo_v;
    bool valid_hodo_h;
    bool trigger_veto_pass;
    bool good_temp;
    bool good_e;
    bool good_data;

    float truth_y;
    float truth_z;

    float beam_2CH_mm;
    float beam_2CV_mm;

  ClassDef(Eval_Run,4)
  };

  class Eval_Cluster : public TObject
  {
  public:
    Eval_Cluster()
    {
      reset();
    }

    virtual
    ~Eval_Cluster()
    {
    }

    void
    reset()
    {
      max_col = -1;
      max_row = -1;
      average_col = 0;
      average_row = 0;

      sum_E = 0;
    }

    void
    reweight_clus_pol()
    {
      if (sum_E > 0)
        {
          average_col /= sum_E;
          average_row /= sum_E;
        }
      else
        {
          average_col = -31454;
          average_row = -31454;

        }
    }

    int max_col;
    int max_row;

    float average_col;
    float average_row;
    float sum_E;

  ClassDef(Eval_Cluster,2)
  };

private:

  // calorimeter size
  enum
  {
    n_size = 8
  };

  bool _is_sim;

  Fun4AllHistoManager *
  get_HistoManager();

  std::pair<int, int>
  find_max(RawTowerContainer* towers, int cluster_size);

  std::string _filename;

  unsigned long _ievent;

  fstream fdata;

  Eval_Run _eval_run;

  Eval_Cluster _eval_3x3_raw;
  Eval_Cluster _eval_5x5_raw;

  Eval_Cluster _eval_3x3_prod;
  Eval_Cluster _eval_5x5_prod;

  Eval_Cluster _eval_3x3_temp;
  Eval_Cluster _eval_5x5_temp;

  Eval_Cluster _eval_3x3_recalib;
  Eval_Cluster _eval_5x5_recalib;

  std::map<std::pair<int, int>, double> _recalib_const;
};

#endif // __Proto3ShowerCalib_H__
