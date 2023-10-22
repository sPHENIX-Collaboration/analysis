// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALOTREEGEN_H
#define CALOTREEGEN_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>
#include <TNtuple.h>

class TNtuple;
class PHCompositeNode;
class Fun4AllHistoManager;
class TFile;
class RawCluster;
class TowerInfoContainer;
class TH1F;
class TH2F;

class caloTreeGen : public SubsysReco
{
 public:

  caloTreeGen(const std::string &name = "caloTreeGen", const std::string &name2 = "ntp");

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

  TNtuple *T;

  TFile *out;
  TFile *out2;
  //Fun4AllHistoManager *hm = nullptr;
  std::string Outfile = "test.root";
  std::string Outfile2 = "ntp.root";

  unsigned int getCaloTowerPhiBin(const unsigned int key);
  unsigned int getCaloTowerEtaBin(const unsigned int key);
  float getMaxTowerE(RawCluster *cluster, TowerInfoContainer *towerContainer);
  std::vector<float> returnClusterTowE(RawCluster *cluster, TowerInfoContainer *towerContainer);
  std::vector<int> returnClusterTowPhi(RawCluster *cluster, TowerInfoContainer *towerContainer);
  std::vector<int> returnClusterTowEta(RawCluster *cluster, TowerInfoContainer *towerContainer);

  int iEvent;

  Double_t min_towE;
  Float_t min_clusterECore;
  Float_t min_clusterEta;
  Float_t min_clusterPhi;
  Float_t min_clusterPt;
  Float_t min_clusterChi;

  Double_t max_towE;
  Float_t max_clusterECore;
  Float_t max_clusterEta;
  Float_t max_clusterPhi;
  Float_t max_clusterPt;
  Float_t max_clusterChi;
  Float_t max_totalCaloE;
  Float_t max_totalmbd;
  UInt_t  max_NClusters;

  TH1F* hTowE;
  TH1F* hClusterECore;
  TH1F* hClusterPt;
  TH1F* hClusterChi;
  TH1F* hNClusters;
  TH1F* hTotalMBD;
  TH1F* hTotalCaloE;
  TH2F* h2ClusterEtaPhi;
  TH2F* h2ClusterEtaPhiWeighted;
  TH2F* h2TowEtaPhiWeighted;
  TH2F* h2TotalMBDCaloE;

  UInt_t bins_towE = 400;
  UInt_t  bins_e   = 200;
  Float_t low_e    = 0;
  Float_t high_e   = 50;

  UInt_t  bins_pt = 200;
  Float_t low_pt  = 0;
  Float_t high_pt = 50;

  UInt_t  bins_eta = 96;
  Float_t low_eta  = -1.152;
  Float_t high_eta = 1.152;

  UInt_t  bins_phi = 256;
  Float_t low_phi  = -M_PI;
  Float_t high_phi = M_PI;

  UInt_t  bins_chi = 400;
  Float_t low_chi  = 0;
  Float_t high_chi = 100;

  UInt_t  bins_n = 3000;
  Float_t low_n  = 0;
  Float_t high_n = 3000;

  UInt_t  bins_totalmbd = 3500;
  Float_t low_totalmbd  = 0;
  Float_t high_totalmbd = 350000;

  UInt_t  bins_totalcaloE = 1500;
  Float_t low_totalcaloE  = 0;
  Float_t high_totalcaloE = 1500;

  // Define PI0 Cuts
  Float_t clusE_min    = 1;
  Float_t clus_chi_max = 10;
};
#endif
