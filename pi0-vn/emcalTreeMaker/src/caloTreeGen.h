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

  Float_t min_clusterECore;
  Float_t min_clusterEta;
  Float_t min_clusterPhi;
  Float_t min_clusterPt;
  Float_t min_clusterChi;

  Float_t max_clusterECore;
  Float_t max_clusterEta;
  Float_t max_clusterPhi;
  Float_t max_clusterPt;
  Float_t max_clusterChi;
  Float_t max_totalCaloE;
  Float_t max_totalmbd;
  UInt_t  max_NClusters;

  TH1F* hClusterECore;
  TH1F* hClusterPt;
  TH1F* hClusterChi;
  TH1F* hNClusters;
  TH1F* hTotalMBD;
  TH1F* hTotalCaloE;
  TH2F* h2ClusterEtaPhi;
  TH2F* h2ClusterEtaPhiWeighted;
  TH2F* h2TowEtaPhiWeighted;

};
#endif
