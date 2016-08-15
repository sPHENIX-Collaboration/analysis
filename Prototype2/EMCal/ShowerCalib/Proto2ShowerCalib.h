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

  Proto2ShowerCalib(const std::string &filename = "cemc_ana.root");
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

  class Eval_Run: public TObject
  {
  public:
    virtual ~Eval_Run(){}

    int run;
    int event;

    float beam_mom;

    int hodo_h;
    int hodo_v;

    float C2_sum;

    bool valid_hodo_v;
    bool valid_hodo_h;
    bool trigger_veto_pass;
    bool good_temp;
    bool good_e;
    bool good_data;

  ClassDef(Eval_Run,1)
  };

  class Eval_Cluster: public TObject
  {
  public:
    virtual ~Eval_Cluster(){}
    int cluster_size;
    int max_col;
    int max_row;

    float average_col;
    float average_row;
    float sum_E;

  ClassDef(Eval_Cluster,1)
  };

private:

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
};

#endif // __Proto2ShowerCalib_H__
