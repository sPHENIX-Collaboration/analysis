#ifndef CENTRALITY_VALIDATION_H
#define CENTRALITY_VALIDATION_H

#include <fun4all/SubsysReco.h>

#include <limits>

// Forward declarations
class CentralityInfo;
class MinimumBiasInfo;
class Fun4AllHistoManager;
class PHCompositeNode;
class TH1;
class TEfficiency;

class CentralityValid : public SubsysReco
{
 public:
  //! constructor
  explicit CentralityValid(const std::string &name = "CentralityValid", const std::string &hist_name = "QA_CentralityReco.root");

  //! destructor
  virtual ~CentralityValid();

  //! full initialization
  int Init(PHCompositeNode *);
  int InitRun(PHCompositeNode *);

  int process_event(PHCompositeNode *);
  
  //! end of run method
  int End(PHCompositeNode *);

 protected:

  Fun4AllHistoManager *hm = nullptr;

  CentralityInfo *_central = nullptr;
  MinimumBiasInfo *_minimumbiasinfo = nullptr;

  std::string _hist_filename;

  TH1 *_h_centrality_bin = nullptr;
 
  TH1 *_h_centrality_bin_mb = nullptr;

  TEfficiency *_he_min_bias = nullptr;
  
};

#endif
