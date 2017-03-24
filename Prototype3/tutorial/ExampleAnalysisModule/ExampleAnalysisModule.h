#ifndef __ExampleAnalysisModule_H__
#define __ExampleAnalysisModule_H__

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

/// \class ExampleAnalysisModule to help you get started
class ExampleAnalysisModule : public SubsysReco
{

public:

  //! constructor
  ExampleAnalysisModule(const std::string &filename = "ExampleAnalysis.root");

  //! destructor
  virtual
  ~ExampleAnalysisModule();

  //! Standard function called at initialization
  int
  Init(PHCompositeNode *topNode);

  //! Standard function called when a new run is processed
  int
  InitRun(PHCompositeNode *topNode);

  //! Standard function called at each event
  int
  process_event(PHCompositeNode *topNode);

  //! Standard function called at the end of processing. Save your stuff here.
  int
  End(PHCompositeNode *topNode);

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

  ClassDef(Eval_Run,10)
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

    //!max hit col
    int max_col;
    //!max hit row
    int max_row;

    //!energy weighted col
    float average_col;
    //!energy weighted row
    float average_row;

    //!sum cluster energy
    float sum_E;

  ClassDef(Eval_Cluster,10)
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
  find_max(RawTowerContainer* towers, int cluster_size);

  //! output root file name
  std::string _filename;

  //! simple event counter
  unsigned long _ievent;

  //! run infomation. To be copied to output TTree T
  Eval_Run _eval_run;

  //! clusters of max 5x5 EMCal cluster. To be copied to output TTree T
  Eval_Cluster _eval_5x5_CEMC;

};

#endif // __ExampleAnalysisModule_H__
