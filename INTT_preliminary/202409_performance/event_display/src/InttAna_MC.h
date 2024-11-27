#ifndef INTTANA_H__
#define INTTANA_H__

#include <fun4all/SubsysReco.h>
#include <string>

/// Class declarations for use in the analysis module
class PHCompositeNode;
class TFile;
class TH1;
class TH2;
class TNtuple;
class TTree;
class InttRawData;

/// Definition of this analysis module class
class InttAna : public SubsysReco
{
 public:
  /// Constructor
  InttAna(const std::string &f = "InttAna",
          const std::string &fname = "AnaTutorial_inttana.root");

  // Destructor
  virtual ~InttAna();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);
 
  /// SubsysReco initialize processing method
  int InitRun(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);

  //truth information
  void getHEPMCTruth(PHCompositeNode *topNode);
  void getPHG4Particle(PHCompositeNode *topNode);

  void setInttRawData(InttRawData *rawModule) { _rawModule = rawModule; }

  void readRawHit(PHCompositeNode *);

  void setBeamCenter(float x=0, float y=0) {xbeam_=x; ybeam_=y;}

 private:
  InttRawData* _rawModule;

  std::string fname_;
  TFile* anafile_;
  TH1*   h_dca2d_zero; 
  TH2*   h2_dca2d_zero; 
  TH2*   h2_dca2d_len; 
  TH1*   h_zvtx;
  TH1*   h_eta;
  TH1*   h_phi;
  TH1*   h_theta;
  TNtuple*  h_ntp_clus; 
  //TTree*  h_t_clus; 
  TNtuple*  h_ntp_cluspair; 

  float xbeam_;
  float ybeam_;

  double xvtx_sim;
  double yvtx_sim;
  double zvtx_sim;

  TH1* h_zvtxseed_;

  TTree *m_hepmctree;

    /// HEPMC Tree variables
  int m_evt=0;
  double m_xvtx;
  double m_yvtx;
  double m_zvtx;
  int m_partid1;
  int m_partid2;
  double m_x1;
  double m_x2;
  int m_mpi;
  int m_process_id;
  double m_truthenergy;
  double m_trutheta;
  double m_truththeta;
  double m_truthphi;
  int m_status;
  double m_truthpx;
  double m_truthpy;
  double m_truthpz;
  double m_truthpt;
  double m_truthp;
  int m_numparticlesinevent;
  int m_truthpid;
  double m_vertex;
};

#endif
