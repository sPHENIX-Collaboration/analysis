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
class TH1;

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
  void set_timing_cut_width(const int &t) { _range = t;}
  void set_vertex_cut(const float &v) { _vz = v;}

 protected:
  std::string detector;
  std::string outfilename;
  int Getpeaktime(TH1 *h);
  Fun4AllHistoManager *hm = nullptr;
  TFile *outfile = nullptr;
  TH2F* h_emcal_mbd_correlation = nullptr;
  TH2F* h_ohcal_mbd_correlation = nullptr;
  TH2F* h_ihcal_mbd_correlation = nullptr;
  TH2F* h_emcal_hcal_correlation = nullptr;
  TH2F* h_emcal_zdc_correlation = nullptr;
  
  TH1F* h_InvMass = nullptr;

  TH2F* h_cemc_etaphi = nullptr;
  TH2F* h_hcalin_etaphi = nullptr;
  TH2F* h_hcalout_etaphi = nullptr;
  TH1* h_totalzdc_e;

  TH1* hzdctime;
  TH1* hmbdtime;
  TH1* hemcaltime;
  TH1* hihcaltime;
  TH1* hohcaltime;

  TH1* hzdctime_cut;
  TH1* hmbdtime_cut;
  TH1* hemcaltime_cut;
  TH1* hihcaltime_cut;
  TH1* hohcaltime_cut;

  TH1* hvtx_z_raw;
  TH1* hvtx_z_cut;

  TH1* hzdcSouthraw;
  TH1* hzdcNorthraw;
  TH1* hzdcSouthcalib;
  TH1* hzdcNorthcalib;

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
  int _eventcounter;
  int _range = 1;
  float _vz = 0.;
  bool m_vtxCut = 0;
};

#endif
