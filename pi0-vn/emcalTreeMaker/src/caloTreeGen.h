// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALOTREEGEN_H
#define CALOTREEGEN_H

#include <fun4all/SubsysReco.h>
#include <calobase/TowerInfoDefs.h>

#include <TTree.h>

#include <string>
#include <vector>

class PHCompositeNode;
class Fun4AllHistoManager;
class TFile;
class RawCluster;
class TowerInfoContainer;
class TH1F;
class TH2F;
class TH3F;
class TowerInfo;

class caloTreeGen : public SubsysReco
{
  public:

    caloTreeGen(const std::string &name = "caloTreeGen");

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

    void set_vtx_z_max(Float_t vtx_z_max = 10) {
      this->vtx_z_max = vtx_z_max;
    }

    void set_output_QA(const std::string &Outfile) {
      this->Outfile = Outfile;
    }

    void set_output_diphoton(const std::string &Outfile2) {
      this->Outfile2 = Outfile2;
    }

    void set_do_pi0_ana(Bool_t do_pi0_ana = false) {
      this->do_pi0_ana = do_pi0_ana;
    }

    void set_simulation(Bool_t isSim = false) {
      this->isSim = isSim;

      if(isSim) {
        bins_totalmbd = 400;
        high_totalmbd = 2.5e5;

        bins_totalmbdv2 = bins_totalmbd;
        high_totalmbdv2 = high_totalmbd;

        bins_vtx_z = 120;
        low_vtx_z  = -30;
        high_vtx_z = 30;

        bins_totalcaloE = 2000;
        low_totalcaloE  = 0;
        high_totalcaloE = 2e3;

        bins_towE  = 160;
        low_towE   = 0;
        high_towE  = 20;

        bins_e = 80;
        low_e  = 0;
        high_e = 20;
      }
    }

    void set_clusterNode(const std::string &clusterNode) {
      this->clusterNode = clusterNode;
    }

    void set_towerNode(const std::string &towerNode) {
      this->towerNode = towerNode;
    }

  private:

    TTree *T;

    TFile *out;
    TFile *out2;
    //Fun4AllHistoManager *hm = nullptr;
    std::string Outfile     = "qa.root";
    std::string Outfile2    = "diphoton.root";
    std::string clusterNode = "CLUSTERINFO_CEMC";
    std::string towerNode   = "TOWERINFO_CALIB_CEMC";

    UInt_t getCaloTowerPhiBin(const UInt_t key);
    UInt_t getCaloTowerEtaBin(const UInt_t key);
    Float_t getMaxTowerE(RawCluster *cluster, TowerInfoContainer *towerContainer);
    std::vector<Float_t> returnClusterTowE(RawCluster *cluster);
    std::vector<Int_t> returnClusterTowPhi(RawCluster *cluster);
    std::vector<Int_t> returnClusterTowEta(RawCluster *cluster);

    TowerInfo* getTower(RawTowerDefs::keytype key, TowerInfoContainer *towerContainer);
    TowerInfo* getMaxTower(RawCluster *cluster, TowerInfoContainer *towerContainer);
    Float_t    getMaxTowerTime(RawCluster *cluster, TowerInfoContainer *towerContainer);

    Bool_t hasTowerFar(RawCluster *cluster, Bool_t isSim = false);

    Int_t gpi0_ctr = 0;
    Int_t iEvent = 0;
    Int_t iEventGood = 0;
    Float_t avg_goodTowers = 0;
    Bool_t isSim = false;

    Double_t min_towE        = 9999;
    Float_t min_clusterECore = 9999;
    Float_t min_clusterEta   = 9999;
    Float_t min_clusterPhi   = 9999;
    Float_t min_clusterPt    = 9999;
    Float_t min_clusterChi   = 9999;
    Float_t min_totalCaloE   = 9999;
    Int_t   min_goodTowers   = 9999;
    Float_t min_vtx_z        = 9999;
    Float_t min_cent         = 9999;
    Float_t min_b            = 9999;

    Double_t max_towE         = 0;
    Float_t  max_clusterECore = 0;
    Float_t  max_clusterEta   = 0;
    Float_t  max_clusterPhi   = 0;
    Float_t  max_clusterPt    = 0;
    Float_t  max_clusterChi   = 0;
    Float_t  max_totalCaloE   = 0;
    Float_t  max_totalmbd     = 0;
    Float_t  max_totalmbd2    = 0;
    UInt_t   max_NClusters    = 0;
    Int_t    max_goodTowers   = 0;
    Float_t  max_vtx_z        = 0;
    Float_t  max_cent         = 0;
    Float_t  max_b            = 0;

    TH1F* hTowE;
    TH1F* hClusterECore;
    TH1F* hClusterPt;
    TH1F* hClusterChi;
    // TH1F* hClusterTime;
    TH1F* hNClusters;
    TH1F* hTotalMBD;
    TH1F* hCentrality;
    TH1F* hImpactPar;
    TH1F* hTotalCaloE;
    TH1F* hVtxZ;
    TH1F* hVtxZv2;
    TH1F* hBadPMTs;

    TH2F* h2ClusterEtaPhi;
    TH2F* h2ClusterEtaPhiWeighted;
    TH2F* h2TowEtaPhiWeighted;
    TH2F* h2TotalMBDCaloE;
    TH2F* h2TotalMBDCentrality;
    TH2F* h2ImpactParCentrality;
    TH2F* h2TotalMBDCaloEv2;

    TH3F* h3ImpactParPtEta;

    UInt_t bins_towE  = 1200;
    Float_t low_towE  = -50;
    Float_t high_towE = 100;

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

    UInt_t  bins_nPMTs = 129;
    Float_t low_nPMTs  = 0;
    Float_t high_nPMTs = 129;

    // UInt_t  bins_time = 32;
    // Float_t low_time  = 0;
    // Float_t high_time = 32;

    UInt_t  bins_n = 3000;
    Float_t low_n  = 0;
    Float_t high_n = 3000;

    UInt_t  bins_totalmbd = 300;
    Float_t low_totalmbd  = 0;
    Float_t high_totalmbd = 3000;

    UInt_t  bins_cent = 100;
    Float_t low_cent  = -0.005;
    Float_t high_cent = 0.995;

    UInt_t  bins_totalcaloE = 2500;
    Float_t low_totalcaloE  = -500;
    Float_t high_totalcaloE = 2000;

    UInt_t  bins_totalmbdv2 = 100;
    Float_t low_totalmbdv2  = 0;
    Float_t high_totalmbdv2 = 3e4;

    UInt_t  bins_totalcaloEv2 = 100;
    Float_t low_totalcaloEv2  = -200;
    Float_t high_totalcaloEv2 = 0;

    UInt_t  bins_vtx_z = 400;
    Float_t low_vtx_z  = -100;
    Float_t high_vtx_z = 100;

    UInt_t  bins_b = 320;
    Float_t low_b  = 0;
    Float_t high_b = 16;

    UInt_t  bins_gpt = 60;
    Float_t low_gpt  = 2;
    Float_t high_gpt = 5;

    // Define PI0 Cuts
    Float_t clusE_min    = 1;
    Float_t clus_chi_max = 10;

    Bool_t do_pi0_ana = false;

    // Define z-vertex cut
    Float_t vtx_z_max;

    // branches of T
    Int_t   run      = 0;
    Int_t   event    = 0;
    Float_t vtx_z    = 0;
    Float_t totalMBD = 0;
    Float_t cent     = 0;
    Float_t b        = 0; /*Impact Parameter from MC*/
    Float_t Q2_S_x   = 0;
    Float_t Q2_S_y   = 0;
    Float_t Q2_N_x   = 0;
    Float_t Q2_N_y   = 0;
    Float_t Q3_S_x   = 0;
    Float_t Q3_S_y   = 0;
    Float_t Q3_N_x   = 0;
    Float_t Q3_N_y   = 0;
    std::vector<Float_t> pi0_phi_vec;
    std::vector<Float_t> pi0_eta_vec;
    std::vector<Float_t> pi0_pt_vec;
    std::vector<Float_t> pi0_mass_vec;
    std::vector<Float_t> asym_vec;
    std::vector<Float_t> deltaR_vec;
    std::vector<Float_t> ecore1_vec;
    std::vector<Float_t> ecore2_vec;
    std::vector<Float_t> chi2_max_vec;
    std::vector<Bool_t>  isFarNorth_vec; // true if either cluster in the diphoton has a tower that is in the last IB board, ieta: 88-95
};
#endif
