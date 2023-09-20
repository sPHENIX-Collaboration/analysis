#ifndef CALOANA_H__
#define CALOANA_H__

#include <fun4all/SubsysReco.h>
#include <vector>
// Forward declarations
class Fun4AllHistoManager;
class PHCompositeNode;
class TFile;
class TNtuple;
class TTree;
class TH2F;
class TH1F;

class CaloAna : public SubsysReco
{
 public:
  //! constructor
  CaloAna(const std::string &name = "CaloAna", const std::string &fname = "MyNtuple.root");

  //! destructor
  virtual ~CaloAna();

  //! full initialization
  int Init(PHCompositeNode *);

  //! event processing method
  int process_event(PHCompositeNode *);

  //! end of run method
  int End(PHCompositeNode *);

  int process_g4hits(PHCompositeNode *);
  int process_g4cells(PHCompositeNode *);
  int process_towers(PHCompositeNode *);
  int process_clusters(PHCompositeNode *);

  void Detector(const std::string &name) { detector = name; }

 protected:
  std::string detector;
  std::string outfilename;
  Fun4AllHistoManager *hm = nullptr;
  TFile *outfile = nullptr;


  TH2F* h_emcal_mbd_correlation = nullptr;
  TH2F* h_ohcal_mbd_correlation = nullptr;
  TH2F* h_ihcal_mbd_correlation = nullptr;
  TH2F* h_emcal_hcal_correlation = nullptr;
  TH1F* h_InvMass = nullptr;

  TH2F* h_cemc_etaphi = nullptr;
  TH2F* h_hcalin_etaphi = nullptr;
  TH2F* h_hcalout_etaphi = nullptr;



  TNtuple *g4hitntuple = nullptr;
  TNtuple *g4cellntuple = nullptr;
  TTree *towerntuple = nullptr;
  TNtuple *clusterntuple = nullptr;
  std::vector<float> m_energy;
  std::vector<int> m_etabin;
  std::vector<int> m_phibin;
  std::vector<int> m_time;

  std::vector<float> m_hcalin_energy;
  std::vector<int> m_hcalin_etabin;
  std::vector<int> m_hcalin_phibin;
  std::vector<int> m_hcalin_time;

  std::vector<float> m_hcalout_energy;
  std::vector<int> m_hcalout_etabin;
  std::vector<int> m_hcalout_phibin;
  std::vector<int> m_hcalout_time;


  std::vector<float> m_zdc_energy;
  std::vector<int> m_zdc_index;
  std::vector<int> m_zdc_side;


  std::vector<float> m_bbc_energy;
  std::vector<int> m_bbc_type;
  std::vector<int> m_bbc_side;

};

#endif
