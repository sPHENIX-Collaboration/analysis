// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALOTREEGEN_H
#define CALOTREEGEN_H

#include <fun4all/SubsysReco.h>
#include <calobase/TowerInfoDefs.h>

#include <TNtuple.h>

#include <string>
#include <vector>

class PHCompositeNode;
class Fun4AllHistoManager;
class TFile;
class RawCluster;
class TowerInfoContainer;
class TH1F;
class TH2F;
class TowerInfo;

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
    Int_t Init(PHCompositeNode *topNode) override;

    /** Called for first event when run number is known.
        Typically this is where you may want to fetch data from
        database, because you know the run number. A place
        to book histograms which have to know the run number.
    */
    Int_t InitRun(PHCompositeNode *topNode) override;

    /** Called for each event.
        This is where you do the real work.
    */
    Int_t process_event(PHCompositeNode *topNode) override;

    /// Clean up internals after each event.
    Int_t ResetEvent(PHCompositeNode *topNode) override;

    /// Called at the end of each run.
    Int_t EndRun(const Int_t runnumber) override;

    /// Called at the end of all processing.
    Int_t End(PHCompositeNode *topNode) override;

    /// Reset
    Int_t Reset(PHCompositeNode * /*topNode*/) override;

    void Print(const std::string &what = "ALL") const override;

    void set_clusterE_min(Float_t clusE_min = 1) {
      this->clusE_min = clusE_min;
    }

    void set_cluster_chi_max(Float_t clus_chi_max = 10) {
      this->clus_chi_max = clus_chi_max;
    }

  private:

    TNtuple *T;

    TFile *out;
    TFile *out2;
    //Fun4AllHistoManager *hm = nullptr;
    std::string Outfile = "test.root";
    std::string Outfile2 = "ntp.root";

    UInt_t getCaloTowerPhiBin(const UInt_t key);
    UInt_t getCaloTowerEtaBin(const UInt_t key);
    Float_t getMaxTowerE(RawCluster *cluster, TowerInfoContainer *towerContainer);
    std::vector<Float_t> returnClusterTowE(RawCluster *cluster, TowerInfoContainer *towerContainer);
    std::vector<Int_t> returnClusterTowPhi(RawCluster *cluster, TowerInfoContainer *towerContainer);
    std::vector<Int_t> returnClusterTowEta(RawCluster *cluster, TowerInfoContainer *towerContainer);

    TowerInfo* getTower(RawTowerDefs::keytype key, TowerInfoContainer *towerContainer);
    TowerInfo* getMaxTower(RawCluster *cluster, TowerInfoContainer *towerContainer);
    Short_t    getMaxTowerTime(RawCluster *cluster, TowerInfoContainer *towerContainer);

    Int_t iEvent;

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
    TH1F* hClusterTime;
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

    UInt_t  bins_time = 32;
    Float_t low_time  = 0;
    Float_t high_time = 32;

    UInt_t  bins_n = 3000;
    Float_t low_n  = 0;
    Float_t high_n = 3000;

    UInt_t  bins_totalmbd = 200;
    Float_t low_totalmbd  = 0;
    Float_t high_totalmbd = 2000;

    UInt_t  bins_totalcaloE = 1500;
    Float_t low_totalcaloE  = 0;
    Float_t high_totalcaloE = 1500;

    // Define PI0 Cuts
    Float_t clusE_min    = 1;
    Float_t clus_chi_max = 10;
};
#endif
