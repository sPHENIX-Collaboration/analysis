#ifndef CALOWAVEFORMSIM_H__
#define CALOWAVEFORMSIM_H__

#include <fun4all/SubsysReco.h>
#include <TTree.h>
#include <TRandom3.h>
#include <caloreco/CaloWaveformProcessing.h>
#include <g4detectors/PHG4CylinderGeomContainer.h>
#include <g4detectors/PHG4FullProjSpacalCellReco.h>
// Forward declarations
class Fun4AllHistoManager;
class PHCompositeNode;
class TFile;
class TNtuple;
class TTree;

class CaloWaveFormSim : public SubsysReco
{
 public:
  //! constructor
  CaloWaveFormSim(const std::string &name = "CaloWaveFormSim", const std::string &fname = "MyNtuple.root");

  //! destructor
  virtual ~CaloWaveFormSim();

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
  static double template_function(double *x, double *par);

  void Detector(const std::string &name) { detector = name; }

 protected:
  std::string detector;
  std::string outfilename;
  Fun4AllHistoManager *hm;
  TFile *outfile;
  /* TNtuple *g4hitntuple; */
  std::vector<float> m_t0;
  std::vector<float> m_edep;
  std::vector<float> m_nhits;
  std::vector<float> m_z0;
  std::vector<float> m_x0;
  std::vector<float> m_y0;
  std::vector<float> m_eta;
  std::vector<float> m_phi;
  std::vector<float> m_primid;
  std::vector<float> m_primpt;
  std::vector<float> m_primeta;
  std::vector<float> m_primtrkid;
  std::vector<float> m_g4primtrkid; 
  std::vector<float> m_g4primpt;
  std::vector<float> m_etabin;
  std::vector<float> m_phibin;

  std::vector<float> m_geoetabin;
  std::vector<float> m_geophibin;

  int m_waveform[24576][16];
  /* int m_sub_waveform[24576][500][16]; */
  int m_ndep[24576];
  int m_tedep[24576];
  float m_extractedadc[24576];
  float m_toweradc[24576];
  

  TTree *g4hitntuple;
  TNtuple *g4cellntuple;
  TNtuple *towerntuple;
  TNtuple *clusterntuple;

  CaloWaveformProcessing* WaveformProcessing;

  PHG4FullProjSpacalCellReco::LightCollectionModel light_collection_model;
  TTree* noise;
  float noise_val[31];
  TRandom3* rnd;

};

#endif
