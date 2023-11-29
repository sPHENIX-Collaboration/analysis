// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef HCALCALIBTREE_H
#define HCALCALIBTREE_H

#include <fun4all/SubsysReco.h>
#include <vector>
#include <array>
#include "TH2F.h"

// Forward declarations
class Fun4AllHistoManager;
class PHCompositeNode;
class TFile;
class TH1F;

class HCalCalibTree : public SubsysReco
{
public:
  //! constructor
  HCalCalibTree(const std::string &name = "HCalCalibTree", const std::string &fname = "HCalCalibtuple.root");
  //! destructor
  virtual ~HCalCalibTree();

  //! Processing
  int Init(PHCompositeNode *) override;
  int process_event(PHCompositeNode *) override;
  int process_towers(PHCompositeNode *);
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *) override;

  void Detector(const std::string &name) { detector = name; }
  void TowerPrefix(const std::string &name) { prefix = name; }

protected:
  std::string detector;
  std::string prefix;
  
  Fun4AllHistoManager *hm = nullptr;

  // HCal geometry
  static const int n_etabin = 24;
  static const int n_phibin = 64;

  // Cut threshold
  static const int tower_threshold = 500;
  static const int vert_threshold = 1500;
  static const int veto_threshold = 350;

  TH1F* h_channel_hist[n_etabin][n_phibin] = {{nullptr}};
  TH2F* h_waveformchi2 = nullptr;

  std::string outfilename;

  int event;

  float m_peak[n_etabin][n_phibin];
  float m_chi2[n_etabin][n_phibin];

  TFile *outfile;
};

#endif // HCALCALIBTREE_H
