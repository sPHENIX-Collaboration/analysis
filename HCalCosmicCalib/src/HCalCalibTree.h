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
  HCalCalibTree(const std::string &name = "HCalCalibTree", const std::string &fname = "HCalCalibtuple.root", const std::string &prefix = "TOWERSV2_");
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
  float tower_threshold, vert_threshold, veto_threshold;

  const float tower_threshold_data = 500;
  const float vert_threshold_data = 1500;
  const float veto_threshold_data = 350;

  const float tower_threshold_sim = 0.001767; //283 ADC/MeV
  const float vert_threshold_sim = 0.0053;
  const float veto_threshold_sim = 0.001237;

  TH1F* h_channel_hist[n_etabin][n_phibin] = {{nullptr}};
  TH2F* h_waveformchi2 = nullptr;
  TH1F* h_check = nullptr;
  TH1F* h_eventnumber_record;

  std::string outfilename;

  int event;
  int goodevent;
  int goodevent_check;

  float m_peak[n_etabin][n_phibin];
  float m_chi2[n_etabin][n_phibin];

  TFile *outfile;
};

#endif // HCALCALIBTREE_H
