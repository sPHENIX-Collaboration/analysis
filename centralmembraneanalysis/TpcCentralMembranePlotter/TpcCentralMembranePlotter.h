#ifndef TPCCALIB_TpcCentralMembranePlotter_H
#define TPCCALIB_TpcCentralMembranePlotter_H

#include <tpc/TpcDistortionCorrection.h>
#include <tpc/TpcDistortionCorrectionContainer.h>

#include <tpc/LaserClusterHelper.h>

#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrDefs.h>

#include <fun4all/SubsysReco.h>

class PHCompositeNode;

class LaserClusterContainer;
class EventHeader;
class LaserEventInfo;
class PHG4TpcGeom;
class PHG4TpcGeomContainer;
class ActsGeometry;
class TrkrHitTruthAssoc;
class TrkrHitSetContainer;

class TF1;
class TFile;
class TH1;
class TH2;
class TH3;
class TGraph;
class TNtuple;
class TTree;
class TVector3;

class TpcCentralMembranePlotter : public SubsysReco
{
 public:
  TpcCentralMembranePlotter(const std::string &name = "TpcCentralMembranePlotter");
  ~TpcCentralMembranePlotter() override = default;

  /// output file name for storing the space charge reconstruction matrices
  void setOutputfile(const std::string &outputfile);

  void set_event_sequence(int seq)
  {
    m_event_sequence = seq;
    m_event_index = 100 * seq;
  }

  void set_QAFileName(const std::string &QAFileName)
  {
    m_QAFileName = QAFileName;
  }

  void set_ppMode(bool mode){ ppMode = mode; }

  void set_phiHistInRad(bool rad){ m_phiHist_in_rad = rad; }

  void set_fieldOff(bool fieldOff){ m_fieldOff = fieldOff; }

  void set_nLayerCut(unsigned int cut) { m_nLayerCut = cut; }

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  int End(PHCompositeNode *topNode) override;

 private:
  EventHeader *eventHeader{nullptr};

  int GetNodes(PHCompositeNode *topNode);

  TpcDistortionCorrection m_distortionCorrection;

  LaserClusterContainer *m_correctedCMcluster_map{nullptr};

  TpcDistortionCorrectionContainer *m_dcc_in_module_edge{nullptr};
  TpcDistortionCorrectionContainer *m_dcc_in_static{nullptr};

  TpcDistortionCorrectionContainer *m_dcc_out{nullptr};

  PHG4TpcGeomContainer *m_geom_container {nullptr};
  ActsGeometry *m_tGeometry {nullptr};
  LaserEventInfo *m_laserEventInfo {nullptr};


  TrkrHitTruthAssoc *m_hitTruthAssoc {nullptr};
  TrkrHitSetContainer *m_hits {nullptr};

  std::string m_outputfile{"CMDistortionCorrections.root"};
  std::string m_QAFileName{""};

  // TH3 *m_clusters[2]{nullptr};

  // TH3 *m_hits[2]{nullptr};
  // TH3 *m_hits_hardware[2]{nullptr};

  TH2 *m_clusters_r_vs_phi[2]{nullptr};
  TH2 *m_clusters_z_vs_phi[2]{nullptr};
  TH2 *m_clusters_z_vs_r[2]{nullptr};

  TH2 *m_hits_r_vs_phi[2]{nullptr};
  TH2 *m_hits_z_vs_phi[2]{nullptr};
  TH2 *m_hits_z_vs_r[2]{nullptr};

  TH2 *m_hits_hardware_layervsphibin[2]{nullptr};
  TH2 *m_hits_hardware_timebinvsphibin[2]{nullptr};
  TH2 *m_hits_hardware_timebinvslayer[2]{nullptr};



  // TH2 *m_hPetal[2]{nullptr};
  // TH2 *m_hPetalRvsT[2]{nullptr};
  // TH2 *m_hPetalRvsZ[2]{nullptr};
  // TH2 *m_hPetalPhivsZ[2]{nullptr};
  // TH2 *m_hPetalHits[2]{nullptr};
  // TH2 *m_hPetalHitsLayervsPhiBin[2]{nullptr};
  // TH2 *m_hPetalHitsRvsT[2]{nullptr};


  TH2 *phiDistortionLamination[2]{nullptr};
  //TH2 *scaleFactorMap[2]{nullptr};

  unsigned int m_nLayerCut{1};
  
  bool m_useHeader{true};

  int m_event_index{0};
  int m_event_sequence{0};

  double m_nClusters{0};
  int m_nEvents{0};
  int m_runnumber{};

  bool ppMode{false};

  bool m_phiHist_in_rad{true};

  std::string m_stripePatternFile = "/sphenix/u/bkimelman/CMStripePattern.root";

  bool m_fieldOff{false};

  LaserClusterHelper m_laserClusterHelper;

};

#endif
