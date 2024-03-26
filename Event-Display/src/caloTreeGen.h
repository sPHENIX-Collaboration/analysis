// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALOTREEGEN_H
#define CALOTREEGEN_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>
#include <TTree.h>
#include <fstream>

class TTree;
class PHCompositeNode;
class Fun4AllHistoManager;
class TFile;
class RawCluster;
class TowerInfoContainer;

class caloTreeGen : public SubsysReco
{
 public:

  caloTreeGen(const std::string &name="caloTreeGen",
              const std::string &eventFile="test.json",
              const int eventNumber = 0,
              const int runid = 0,
              const float tow_cemc_min = 0,
              const float tow_hcalin_min = 0,
              const float tow_hcalout_min = 0);

  ~caloTreeGen() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
  */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
  */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
  */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

 private:

  TTree *T;
  
  std::vector<float> m_emcTowPhi;
  std::vector<float> m_emcTowEta;
  std::vector<float> m_emcTowE;
  std::vector<float> m_emcTiming;
  std::vector<float> m_emciEta;
  std::vector<float> m_emciPhi;

  std::vector<float> m_ohciTowPhi;
  std::vector<float> m_ohciTowEta;
  std::vector<float> m_ohcTowE;
  
  std::vector<float> m_ihciTowPhi;
  std::vector<float> m_ihciTowEta;
  std::vector<float> m_ihcTowE;
  
  TFile *out;
  //Fun4AllHistoManager *hm = nullptr;
  std::string Outfile = "commissioning.root";
  std::string eventFile = "test.json";

  unsigned int getCaloTowerPhiBin(const unsigned int key);
  unsigned int getCaloTowerEtaBin(const unsigned int key);
  float getMaxTowerE(RawCluster *cluster, TowerInfoContainer *towerContainer);
  std::vector<float> returnClusterTowE(RawCluster *cluster, TowerInfoContainer *towerContainer);
  std::vector<int> returnClusterTowPhi(RawCluster *cluster, TowerInfoContainer *towerContainer);
  std::vector<int> returnClusterTowEta(RawCluster *cluster, TowerInfoContainer *towerContainer);

  float totalCaloE;
  int iEvent;
  int eventNumber;
  int runid;
  float tow_cemc_min;
  float tow_hcalin_min;
  float tow_hcalout_min;

  std::ofstream eventOutput;

  const float eta_map[24] = {-1.05417,  -0.9625, -0.870833, -0.779167, -0.6875, -0.595833, -0.504167, -0.4125, -0.320833, -0.229167, -0.1375, -0.0458333,
                              0.0458333, 0.1375,  0.229167,  0.320833,  0.4125,  0.504167,  0.595833,  0.6875,  0.779167,  0.870833,  0.9625,  1.05417};

  const float phi_map[64] = {0.0490874, 0.147262, 0.245437, 0.343612, 0.441786, 0.539961, 0.638136, 0.736311,
                             0.834486,  0.93266,  1.03084,  1.12901,  1.22718,  1.32536,  1.42353,  1.52171,
                             1.61988,   1.71806,  1.81623,  1.91441,  2.01258,  2.11076,  2.20893,  2.30711,
                             2.40528,   2.50346,  2.60163,  2.69981,  2.79798,  2.89616,  2.99433,  3.09251,
                             3.19068,   3.28885,  3.38703,  3.4852,   3.58338,  3.68155,  3.77973,  3.8779,
                             3.97608,   4.07425,  4.17243,  4.2706,   4.36878,  4.46695,  4.56513,  4.6633,
                             4.76148,   4.85965,  4.95783,  5.056,    5.15418,  5.25235,  5.35052,  5.4487,
                             5.54687,   5.64505,  5.74322,  5.8414,   5.93957,  6.03775,  6.13592,  6.2341};
};

#endif 
