#include <TCanvas.h>
#include <TCut.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TObjString.h>
#include <TRandom3.h>
#include <TTree.h>
#include <TTreeIndex.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// in python, do this: np.logspace(-4,np.log10(0.5),num=51)
const int NBins = 50;
float edges[NBins + 1] = {1.00000000e-04, 1.18571250e-04, 1.40591414e-04, 1.66700998e-04, 1.97659458e-04, 2.34367291e-04, 2.77892228e-04, 3.29500290e-04, 3.90692614e-04,
                          4.63249118e-04, 5.49280272e-04, 6.51288487e-04, 7.72240903e-04, 9.15655696e-04, 1.08570441e-03, 1.28733329e-03, 1.52640718e-03, 1.80988009e-03,
                          2.14599745e-03, 2.54453601e-03, 3.01708817e-03, 3.57739917e-03, 4.24176693e-03, 5.02951609e-03, 5.96356012e-03, 7.07106781e-03, 8.38425353e-03,
                          9.94131425e-03, 1.17875406e-02, 1.39766343e-02, 1.65722701e-02, 1.96499479e-02, 2.32991889e-02, 2.76261397e-02, 3.27566592e-02, 3.88399805e-02,
                          4.60530506e-02, 5.46056779e-02, 6.47466352e-02, 7.67708949e-02, 9.10282102e-02, 1.07933287e-01, 1.27977848e-01, 1.51744935e-01, 1.79925867e-01,
                          2.13340350e-01, 2.52960321e-01, 2.99938216e-01, 3.55640493e-01, 4.21687380e-01, 5.00000000e-01};

void makehist(TString infname, TString outfname)
{
    TFile *fout = new TFile(outfname, "RECREATE");

    // not zvtx weighted
    TH1F *hM_clusphi = new TH1F("hM_clusphi", "hM_clusphi", 140, -3.5, 3.5);
    TH1F *hM_cluseta = new TH1F("hM_cluseta", "hM_cluseta", 80, -4, 4);
    TH1F *hM_clusphisize = new TH1F("hM_clusphisize", "hM_clusphisize", 80, 0, 80);
    TH1F *hM_clusadc = new TH1F("hM_clusadc", "hM_clusadc", 200, 0, 20000);
    TH2F *hM_clusphi_clusphisize = new TH2F("hM_clusphi_clusphisize", "hM_clusphi_clusphisize", 140, -3.5, 3.5, 80, 0, 80);
    TH2F *hM_cluseta_clusphisize = new TH2F("hM_cluseta_clusphisize", "hM_cluseta_clusphisize", 160, -4, 4, 80, 0, 80);
    // zvtx weighted
    TH1F *hM_clusphi_zvtxwei = new TH1F("hM_clusphi_zvtxwei", "hM_clusphi_zvtxwei", 140, -3.5, 3.5);
    TH1F *hM_cluseta_zvtxwei = new TH1F("hM_cluseta_zvtxwei", "hM_cluseta_zvtxwei", 80, -4, 4);
    TH1F *hM_clusphisize_zvtxwei = new TH1F("hM_clusphisize_zvtxwei", "hM_clusphisize_zvtxwei", 80, 0, 80);
    TH2F *hM_clusphi_clusphisize_zvtxwei = new TH2F("hM_clusphi_clusphisize_zvtxwei", "hM_clusphi_clusphisize_zvtxwei", 140, -3.5, 3.5, 80, 0, 80);
    TH2F *hM_cluseta_clusphisize_zvtxwei = new TH2F("hM_cluseta_clusphisize_zvtxwei", "hM_cluseta_clusphisize_zvtxwei", 160, -4, 4, 80, 0, 80);

    // clusters of tracklets
    TH1F *hM_tklclus1phisize = new TH1F("hM_tklclus1phisize", "hM_tklclus1phisize", 80, 0, 80);
    TH1F *hM_tklclus2phisize = new TH1F("hM_tklclus2phisize", "hM_tklclus2phisize", 80, 0, 80);
    TH2F *hM_tklclus1phisize_tklclus2phisize = new TH2F("hM_tklclus1phisize_tklclus2phisize", "hM_tklclus1phisize_tklclus2phisize", 80, 0, 80, 80, 0, 80);
    TH2F *hM_tklclus1phi_tklclus1phisize = new TH2F("hM_tklclus1phi_tklclus1phisize", "hM_tklclus1phi_tklclus1phisize", 140, -3.5, 3.5, 80, 0, 80);
    TH2F *hM_tklclus2phi_tklclus2phisize = new TH2F("hM_tklclus2phi_tklclus2phisize", "hM_tklclus2phi_tklclus2phisize", 140, -3.5, 3.5, 80, 0, 80);
    TH2F *hM_tklclus1eta_tklclus1phisize = new TH2F("hM_tklclus1eta_tklclus1phisize", "hM_tklclus1eta_tklclus1phisize", 160, -4, 4, 80, 0, 80);
    TH2F *hM_tklclus2eta_tklclus2phisize = new TH2F("hM_tklclus2eta_tklclus2phisize", "hM_tklclus2eta_tklclus2phisize", 160, -4, 4, 80, 0, 80);

    TH1F *hM_dEta_reco = new TH1F("hM_dEta_reco", "hM_dEta_reco", 200, -3, 3);
    TH1F *hM_dEta_reco_altrange = new TH1F("hM_dEta_reco_altrange", "hM_dEta_reco_altrange", 100, -0.5, 0.5);
    TH1F *hM_dEta_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20 = new TH1F("hM_dEta_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20", "hM_dEta_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20", 100, -0.5, 0.5);
    TH1F *hM_dPhi_reco = new TH1F("hM_dPhi_reco", "hM_dPhi_reco", 200, -0.5, 0.5);
    TH1F *hM_dPhi_reco_altrange = new TH1F("hM_dPhi_reco_altrange", "hM_dPhi_reco_altrange", 200, -0.05, 0.05);
    TH1F *hM_dPhi_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20 = new TH1F("hM_dPhi_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20", "hM_dPhi_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20", 200, -0.05, 0.05);
    TH1F *hM_dR_reco = new TH1F("hM_dR_reco", "hM_dR_reco", 100, 0, 0.5);
    TH1F *hM_dR_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20 = new TH1F("hM_dR_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20", "hM_dR_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20", 100, 0, 0.5);
    TH1F *hM_dR_reco_altrange = new TH1F("hM_dR_reco_altrange", "hM_dR_reco_altrange", 50, 0, 0.05);
    TH1F *hM_dR_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20 = new TH1F("hM_dR_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20", "hM_dR_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20", 50, 0, 0.05);
    TH1F *hM_dR_reco_LogX = new TH1F("hM_dR_reco_LogX", "hM_dR_reco_LogX", NBins, edges);
    TH1F *hM_Eta_reco = new TH1F("hM_Eta_reco", "hM_Eta_reco", 80, -4, 4);
    TH1F *hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20 = new TH1F("hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20", "hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20", 80, -4, 4);
    TH1F *hM_Phi_reco = new TH1F("hM_Phi_reco", "hM_Phi_reco", 140, -3.5, 3.5);
    TH1F *hM_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20 = new TH1F("hM_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20", "hM_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20", 140, -3.5, 3.5);
    TH2F *hM_Eta_vtxZ_reco_incl = new TH2F("hM_Eta_vtxZ_reco_incl", "hM_Eta_vtxZ_reco_incl", 280, -3.5, 3.5, 310, -31, 31);
    TH2F *hM_Eta_vtxZ_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20 = new TH2F("hM_Eta_vtxZ_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20", "hM_Eta_vtxZ_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20", 350, -3.5, 3.5, 420, -21, 21);
    TH2F *hM_Eta_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20 = new TH2F("hM_Eta_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20", "hM_Eta_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20", 350, -3.5, 3.5, 350, -3.5, 3.5);

    vector<float> centrality_cut = {0, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    vector<TH1F *> hM_dEta_reco_Centrality;
    vector<TH1F *> hM_dPhi_reco_Centrality;
    vector<TH1F *> hM_dR_reco_Centrality;
    vector<TH2F *> hM_Eta_vtxZ_reco_Centrality;
    vector<TH1F *> hM_dEta_reco_Centrality_MBDAsymLe0p75;
    vector<TH1F *> hM_dPhi_reco_Centrality_MBDAsymLe0p75;
    vector<TH1F *> hM_dR_reco_Centrality_MBDAsymLe0p75;
    vector<TH2F *> hM_Eta_vtxZ_reco_Centrality_MBDAsymLe0p75;
    vector<TH1F *> hM_dEta_reco_Centrality_MBDAsymLe0p75_VtxZm20to20;
    vector<TH1F *> hM_dPhi_reco_Centrality_MBDAsymLe0p75_VtxZm20to20;
    vector<TH1F *> hM_dR_reco_Centrality_MBDAsymLe0p75_VtxZm20to20;
    vector<TH2F *> hM_Eta_vtxZ_reco_Centrality_MBDAsymLe0p75_VtxZm20to20;
    vector<TH2F *> hM_Eta_Phi_reco_Centrality_MBDAsymLe0p75_VtxZm20to20;
    vector<TH1F *> hM_Eta_reco_Centrality_MBDAsymLe0p75_VtxZm20to20;
    
    for (size_t i = 0; i < centrality_cut.size() - 1; i++)
    {
        hM_dEta_reco_Centrality.push_back(new TH1F(Form("hM_dEta_reco_Centrality_%dto%d", (int)centrality_cut[i], (int)centrality_cut[i + 1]),
                                                   Form("hM_dEta_reco_Centrality_%dto%d", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 100, -0.5, 0.5));
        hM_dPhi_reco_Centrality.push_back(new TH1F(Form("hM_dPhi_reco_Centrality_%dto%d", (int)centrality_cut[i], (int)centrality_cut[i + 1]),
                                                   Form("hM_dPhi_reco_Centrality_%dto%d", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 200, -0.05, 0.05));
        hM_dR_reco_Centrality.push_back(new TH1F(Form("hM_dR_reco_Centrality_%dto%d", (int)centrality_cut[i], (int)centrality_cut[i + 1]),
                                                 Form("hM_dR_reco_Centrality_%dto%d", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 100, 0, 0.5));
        hM_Eta_vtxZ_reco_Centrality.push_back(new TH2F(Form("hM_Eta_vtxZ_reco_Centrality_%dto%d", (int)centrality_cut[i], (int)centrality_cut[i + 1]),
                                                       Form("hM_Eta_vtxZ_reco_Centrality_%dto%d", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 280, -3.5, 3.5, 310, -31, 31));
        hM_dEta_reco_Centrality_MBDAsymLe0p75.push_back(new TH1F(Form("hM_dEta_reco_Centrality_%dto%d_MBDAsymLe0p75", (int)centrality_cut[i], (int)centrality_cut[i + 1]),
                                                                 Form("hM_dEta_reco_Centrality_%dto%d_MBDAsymLe0p75", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 100, -0.5, 0.5));
        hM_dPhi_reco_Centrality_MBDAsymLe0p75.push_back(new TH1F(Form("hM_dPhi_reco_Centrality_%dto%d_MBDAsymLe0p75", (int)centrality_cut[i], (int)centrality_cut[i + 1]),
                                                                 Form("hM_dPhi_reco_Centrality_%dto%d_MBDAsymLe0p75", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 200, -0.05, 0.05));
        hM_dR_reco_Centrality_MBDAsymLe0p75.push_back(new TH1F(Form("hM_dR_reco_Centrality_%dto%d_MBDAsymLe0p75", (int)centrality_cut[i], (int)centrality_cut[i + 1]),
                                                               Form("hM_dR_reco_Centrality_%dto%d_MBDAsymLe0p75", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 100, 0, 0.5));
        hM_Eta_vtxZ_reco_Centrality_MBDAsymLe0p75.push_back(new TH2F(Form("hM_Eta_vtxZ_reco_Centrality_%dto%d_MBDAsymLe0p75", (int)centrality_cut[i], (int)centrality_cut[i + 1]),
                                                                     Form("hM_Eta_vtxZ_reco_Centrality_%dto%d_MBDAsymLe0p75", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 280, -3.5, 3.5, 310,
                                                                     -31, 31));
        hM_dEta_reco_Centrality_MBDAsymLe0p75_VtxZm20to20.push_back(new TH1F(Form("hM_dEta_reco_Centrality_%dto%d_MBDAsymLe0p75_VtxZm20to20", (int)centrality_cut[i], (int)centrality_cut[i + 1]),
                                                                              Form("hM_dEta_reco_Centrality_%dto%d_MBDAsymLe0p75_VtxZm20to20", (int)centrality_cut[i], (int)centrality_cut[i + 1]),
                                                                              100, -0.5, 0.5));
        hM_dPhi_reco_Centrality_MBDAsymLe0p75_VtxZm20to20.push_back(new TH1F(Form("hM_dPhi_reco_Centrality_%dto%d_MBDAsymLe0p75_VtxZm20to20", (int)centrality_cut[i], (int)centrality_cut[i + 1]),
                                                                              Form("hM_dPhi_reco_Centrality_%dto%d_MBDAsymLe0p75_VtxZm20to20", (int)centrality_cut[i], (int)centrality_cut[i + 1]),
                                                                              200, -0.05, 0.05));
        hM_dR_reco_Centrality_MBDAsymLe0p75_VtxZm20to20.push_back(new TH1F(Form("hM_dR_reco_Centrality_%dto%d_MBDAsymLe0p75_VtxZm20to20", (int)centrality_cut[i], (int)centrality_cut[i + 1]),
                                                                            Form("hM_dR_reco_Centrality_%dto%d_MBDAsymLe0p75_VtxZm20to20", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 100, 0,
                                                                            0.5));
        hM_Eta_vtxZ_reco_Centrality_MBDAsymLe0p75_VtxZm20to20.push_back(
            new TH2F(Form("hM_Eta_vtxZ_reco_Centrality_%dto%d_MBDAsymLe0p75_VtxZm20to20", (int)centrality_cut[i], (int)centrality_cut[i + 1]),
                     Form("hM_Eta_vtxZ_reco_Centrality_%dto%d_MBDAsymLe0p75_VtxZm20to20", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 350, -3.5, 3.5, 420, -21, 21));

        hM_Eta_Phi_reco_Centrality_MBDAsymLe0p75_VtxZm20to20.push_back(
            new TH2F(Form("hM_Eta_Phi_reco_Centrality_%dto%d_MBDAsymLe0p75_VtxZm20to20", (int)centrality_cut[i], (int)centrality_cut[i + 1]),
                     Form("hM_Eta_Phi_reco_Centrality_%dto%d_MBDAsymLe0p75_VtxZm20to20", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 280, -3.5, 3.5, 350, -3.5, 3.5));
        hM_Eta_reco_Centrality_MBDAsymLe0p75_VtxZm20to20.push_back(
            new TH1F(Form("hM_Eta_reco_Centrality_%dto%d_MBDAsymLe0p75_VtxZm20to20", (int)centrality_cut[i], (int)centrality_cut[i + 1]),
                     Form("hM_Eta_reco_Centrality_%dto%d_MBDAsymLe0p75_VtxZm20to20", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 80, -4, 4));
    }

    // not zvtx weighted
    TH1F *hM_NClusLayer1 = new TH1F("hM_NClusLayer1", "hM_NClusLayer1", 100, 0, 5000);
    TH1F *hM_NClusLayer2 = new TH1F("hM_NClusLayer2", "hM_NClusLayer2", 100, 0, 5000);
    TH1F *hM_NTklclusLayer1 = new TH1F("hM_NTklclusLayer1", "hM_NTklclusLayer1", 100, 0, 5000);
    TH1F *hM_NPrototkl = new TH1F("hM_NPrototkl", "hM_NPrototkl", 200, 0, 20000);
    TH1F *hM_NRecotkl_Raw = new TH1F("hM_NRecotkl_Raw", "hM_NRecotkl_Raw", 100, 0, 5000);
    // zvtx weighted 
    TH1F *hM_NClusLayer1_zvtxwei = new TH1F("hM_NClusLayer1_zvtxwei", "hM_NClusLayer1_zvtxwei",  100, 0, 5000);
    TH1F *hM_NClusLayer2_zvtxwei = new TH1F("hM_NClusLayer2_zvtxwei", "hM_NClusLayer2_zvtxwei", 100, 0, 5000);
    TH1F *hM_NTklclusLayer1_zvtxwei = new TH1F("hM_NTklclusLayer1_zvtxwei", "hM_NTklclusLayer1_zvtxwei", 100, 0, 5000);
    TH1F *hM_NPrototkl_zvtxwei = new TH1F("hM_NPrototkl_zvtxwei", "hM_NPrototkl_zvtxwei", 200, 0, 20000);
    TH1F *hM_NRecotkl_Raw_zvtxwei = new TH1F("hM_NRecotkl_Raw_zvtxwei", "hM_NRecotkl_Raw_zvtxwei", 100, 0, 5000);

    TH1F *hM_RecoPVz = new TH1F("hM_RecoPVz", "hM_RecoPVz", 310, -31, 31);
    TH1F *hM_RecoPVz_MBDAsymLe0p75_VtxZm20to20 = new TH1F("hM_RecoPVz_MBDAsymLe0p75_VtxZm20to20", "hM_RecoPVz_MBDAsymLe0p75_VtxZm20to20", 420, -21, 21);
    TH1F *hM_MBDChargeAsymm_Le0p75 = new TH1F("hM_MBDChargeAsymm_Le0p75", "hM_MBDChargeAsymm_Le0p75", 100, -1, 1);
    TH1F *hM_MBDChargeAsymm_Le0p75_VtxZm20to20 = new TH1F("hM_MBDChargeAsymm_Le0p75_VtxZm20to20", "hM_MBDChargeAsymm_Le0p75_VtxZm20to20", 100, -1, 1);

    TFile *f = new TFile(infname, "READ");
    TTree *t = (TTree *)f->Get("minitree");
    t->BuildIndex("event"); // Reference: https://root-forum.cern.ch/t/sort-ttree-entries/13138
    TTreeIndex *index = (TTreeIndex *)t->GetTreeIndex();
    int event, NClusLayer1, NClusLayer2, NPrototkl, NRecotkl_Raw, NRecotkl_GenMatched, NGenHadron;
    float PV_z, TruthPV_z, vtxzwei;
    float MBD_centrality;
    float mbd_south_charge_sum, mbd_north_charge_sum, mbd_charge_sum, mbd_charge_asymm, mbd_z_vtx;
    bool is_min_bias, InttBco_IsToBeRemoved;
    bool firedTrig10_MBDSNgeq2, firedTrig12_vtxle10cm, firedTrig13_vtxle30cm, MbdNSge0, MbdZvtxle10cm, validMbdVtx;
    vector<int> *clusLayer = 0;
    vector<float> *clusPhi = 0, *clusEta = 0, *clusPhiSize = 0;
    vector<unsigned int> *clusADC = 0;
    vector<float> *tklclus1Phi = 0, *tklclus1Eta = 0, *tklclus1PhiSize = 0, *tklclus2Phi = 0, *tklclus2Eta = 0, *tklclus2PhiSize = 0;
    vector<unsigned int> *tklclus1ADC = 0, *tklclus2ADC = 0;
    // vector<float> *prototkl_eta = 0, *prototkl_phi = 0, *prototkl_deta = 0, *prototkl_dphi = 0, *prototkl_dR = 0;
    vector<float> *recotklraw_eta = 0, *recotklraw_phi = 0, *recotklraw_deta = 0, *recotklraw_dphi = 0, *recotklraw_dR = 0;
    t->SetBranchAddress("event", &event);
    t->SetBranchAddress("is_min_bias", &is_min_bias);
    if (t->GetListOfBranches()->FindObject("InttBco_IsToBeRemoved"))
    {
        t->SetBranchAddress("InttBco_IsToBeRemoved", &InttBco_IsToBeRemoved);
    }
    else
    {
        InttBco_IsToBeRemoved = false;
    }
    t->SetBranchAddress("firedTrig10_MBDSNgeq2", &firedTrig10_MBDSNgeq2);
    t->SetBranchAddress("firedTrig12_vtxle10cm", &firedTrig12_vtxle10cm);
    t->SetBranchAddress("firedTrig13_vtxle30cm", &firedTrig13_vtxle30cm);
    t->SetBranchAddress("MbdNSge0", &MbdNSge0);
    t->SetBranchAddress("MbdZvtxle10cm", &MbdZvtxle10cm);
    t->SetBranchAddress("validMbdVtx", &validMbdVtx);
    t->SetBranchAddress("MBD_centrality", &MBD_centrality);
    t->SetBranchAddress("MBD_z_vtx", &mbd_z_vtx);
    t->SetBranchAddress("MBD_south_charge_sum", &mbd_south_charge_sum);
    t->SetBranchAddress("MBD_north_charge_sum", &mbd_north_charge_sum);
    t->SetBranchAddress("MBD_charge_sum", &mbd_charge_sum);
    t->SetBranchAddress("MBD_charge_asymm", &mbd_charge_asymm);
    t->SetBranchAddress("NClusLayer1", &NClusLayer1);
    t->SetBranchAddress("NClusLayer2", &NClusLayer2);
    t->SetBranchAddress("NPrototkl", &NPrototkl);
    t->SetBranchAddress("NRecotkl_Raw", &NRecotkl_Raw);
    t->SetBranchAddress("PV_z", &PV_z);
    t->SetBranchAddress("vtxzwei", &vtxzwei);
    t->SetBranchAddress("TruthPV_z", &TruthPV_z);
    t->SetBranchAddress("clusLayer", &clusLayer);
    t->SetBranchAddress("clusPhi", &clusPhi);
    t->SetBranchAddress("clusEta", &clusEta);
    t->SetBranchAddress("clusPhiSize", &clusPhiSize);
    t->SetBranchAddress("clusADC", &clusADC);
    t->SetBranchAddress("tklclus1Phi", &tklclus1Phi);
    t->SetBranchAddress("tklclus1Eta", &tklclus1Eta);
    t->SetBranchAddress("tklclus1PhiSize", &tklclus1PhiSize);
    t->SetBranchAddress("tklclus2Phi", &tklclus2Phi);
    t->SetBranchAddress("tklclus2Eta", &tklclus2Eta);
    t->SetBranchAddress("tklclus2PhiSize", &tklclus2PhiSize);
    t->SetBranchAddress("tklclus1ADC", &tklclus1ADC);
    t->SetBranchAddress("tklclus2ADC", &tklclus2ADC);
    t->SetBranchAddress("recotklraw_eta", &recotklraw_eta);
    t->SetBranchAddress("recotklraw_phi", &recotklraw_phi);
    t->SetBranchAddress("recotklraw_deta", &recotklraw_deta);
    t->SetBranchAddress("recotklraw_dphi", &recotklraw_dphi);
    t->SetBranchAddress("recotklraw_dR", &recotklraw_dR);

    for (int ev = 0; ev < t->GetEntriesFast(); ev++)
    {
        Long64_t local = t->LoadTree(index->GetIndex()[ev]);
        t->GetEntry(local);

        cout << "Event=" << event << "; NClusLayer1=" << NClusLayer1 << "; NTklclusLayer1=" << tklclus1Phi->size() << "; NPrototkl=" << NPrototkl << "; NRecotkl_Raw=" << NRecotkl_Raw << endl;        

        if (InttBco_IsToBeRemoved)
            continue;
        
        bool InttZvtxle10cm = (PV_z >= -10. && PV_z <= 10.);
        bool evtsel = validMbdVtx && MbdNSge0 && is_min_bias && firedTrig10_MBDSNgeq2 && MbdZvtxle10cm && InttZvtxle10cm;
        if (!evtsel)
            continue; 

        hM_NClusLayer1->Fill(NClusLayer1);
        hM_NClusLayer2->Fill(NClusLayer2);
        hM_NTklclusLayer1->Fill(tklclus1Phi->size());
        hM_NPrototkl->Fill(NPrototkl);
        hM_NRecotkl_Raw->Fill(NRecotkl_Raw);

        // vtxzwei = 1.;
        hM_NClusLayer1_zvtxwei->Fill(NClusLayer1, vtxzwei);
        hM_NClusLayer2_zvtxwei->Fill(NClusLayer2, vtxzwei);
        hM_NTklclusLayer1_zvtxwei->Fill(tklclus1Phi->size(), vtxzwei);
        hM_NPrototkl_zvtxwei->Fill(NPrototkl, vtxzwei);
        hM_NRecotkl_Raw_zvtxwei->Fill(NRecotkl_Raw, vtxzwei);

        int NClusLayer1_clusADCgt35 = 0;

        hM_RecoPVz->Fill(PV_z, vtxzwei);

        cout << "Event " << event << " has " << clusPhi->size() << " clusters" << endl;
        for (size_t j = 0; j < clusPhiSize->size(); j++)
        {
            hM_clusphi->Fill(clusPhi->at(j));
            hM_cluseta->Fill(clusEta->at(j));
            hM_clusphisize->Fill(clusPhiSize->at(j));
            hM_clusadc->Fill(clusADC->at(j));
            hM_clusphi_clusphisize->Fill(clusPhi->at(j), clusPhiSize->at(j));
            hM_cluseta_clusphisize->Fill(clusEta->at(j), clusPhiSize->at(j));

            hM_clusphi_zvtxwei->Fill(clusPhi->at(j), vtxzwei);
            hM_cluseta_zvtxwei->Fill(clusEta->at(j), vtxzwei);
            hM_clusphisize_zvtxwei->Fill(clusPhiSize->at(j), vtxzwei);
            hM_clusphi_clusphisize_zvtxwei->Fill(clusPhi->at(j), clusPhiSize->at(j), vtxzwei);
            hM_cluseta_clusphisize_zvtxwei->Fill(clusEta->at(j), clusPhiSize->at(j), vtxzwei);
        }

        // clusters of tracklets
        for (size_t j = 0; j < tklclus1PhiSize->size(); j++)
        {
            hM_tklclus1phisize->Fill(tklclus1PhiSize->at(j), vtxzwei);
            hM_tklclus2phisize->Fill(tklclus2PhiSize->at(j), vtxzwei);
            hM_tklclus1phisize_tklclus2phisize->Fill(tklclus1PhiSize->at(j), tklclus2PhiSize->at(j), vtxzwei);
            hM_tklclus1phi_tklclus1phisize->Fill(tklclus1Phi->at(j), tklclus1PhiSize->at(j), vtxzwei);
            hM_tklclus2phi_tklclus2phisize->Fill(tklclus2Phi->at(j), tklclus2PhiSize->at(j), vtxzwei);
        }

        for (size_t j = 0; j < recotklraw_eta->size(); j++)
        {
            hM_dEta_reco->Fill(recotklraw_deta->at(j), vtxzwei);
            hM_dEta_reco_altrange->Fill(recotklraw_deta->at(j), vtxzwei);
            hM_dPhi_reco->Fill(recotklraw_dphi->at(j), vtxzwei);
            hM_dPhi_reco_altrange->Fill(recotklraw_dphi->at(j), vtxzwei);
            hM_dR_reco->Fill(recotklraw_dR->at(j), vtxzwei);
            hM_dR_reco_altrange->Fill(recotklraw_dR->at(j), vtxzwei);
            hM_dR_reco_LogX->Fill(recotklraw_dR->at(j), vtxzwei);
            hM_Eta_reco->Fill(recotklraw_eta->at(j), vtxzwei);
            hM_Phi_reco->Fill(recotklraw_phi->at(j), vtxzwei);
            hM_Eta_vtxZ_reco_incl->Fill(recotklraw_eta->at(j), PV_z, vtxzwei);
        }
        
        for (size_t i = 0; i < centrality_cut.size() - 1; i++)
        {
            if (MBD_centrality >= centrality_cut[i] && MBD_centrality < centrality_cut[i + 1])
            {
                for (size_t j = 0; j < recotklraw_eta->size(); j++)
                {
                    hM_dEta_reco_Centrality[i]->Fill(recotklraw_deta->at(j), vtxzwei);
                    hM_dPhi_reco_Centrality[i]->Fill(recotklraw_dphi->at(j), vtxzwei);
                    hM_dR_reco_Centrality[i]->Fill(recotklraw_dR->at(j), vtxzwei);
                    hM_Eta_vtxZ_reco_Centrality[i]->Fill(recotklraw_eta->at(j), PV_z, vtxzwei);
                }
            }
        }

        if (fabs(mbd_charge_asymm) <= 0.75)
        {
            hM_MBDChargeAsymm_Le0p75->Fill(mbd_charge_asymm, vtxzwei);

            for (size_t i = 0; i < centrality_cut.size() - 1; i++)
            {
                if (MBD_centrality >= centrality_cut[i] && MBD_centrality < centrality_cut[i + 1])
                {
                    for (size_t j = 0; j < recotklraw_eta->size(); j++)
                    {
                        hM_dEta_reco_Centrality_MBDAsymLe0p75[i]->Fill(recotklraw_deta->at(j), vtxzwei);
                        hM_dPhi_reco_Centrality_MBDAsymLe0p75[i]->Fill(recotklraw_dphi->at(j), vtxzwei);
                        hM_dR_reco_Centrality_MBDAsymLe0p75[i]->Fill(recotklraw_dR->at(j), vtxzwei);
                        hM_Eta_vtxZ_reco_Centrality_MBDAsymLe0p75[i]->Fill(recotklraw_eta->at(j), PV_z, vtxzwei);
                    }
                }
            }
        }

        if (fabs(mbd_charge_asymm) <= 0.75 && (PV_z >= -20 && PV_z <= 20))
        {
            hM_RecoPVz_MBDAsymLe0p75_VtxZm20to20->Fill(PV_z, vtxzwei);
            hM_MBDChargeAsymm_Le0p75_VtxZm20to20->Fill(mbd_charge_asymm, vtxzwei);

            if (MBD_centrality >= 0 && MBD_centrality <= 70)
            {
                for (size_t j = 0; j < recotklraw_eta->size(); j++)
                {
                    hM_dEta_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20->Fill(recotklraw_deta->at(j), vtxzwei);
                    hM_dPhi_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20->Fill(recotklraw_dphi->at(j), vtxzwei);
                    hM_dR_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20->Fill(recotklraw_dR->at(j), vtxzwei);
                    hM_dR_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20->Fill(recotklraw_dR->at(j), vtxzwei);
                    hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20->Fill(recotklraw_eta->at(j), vtxzwei);
                    hM_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20->Fill(recotklraw_phi->at(j), vtxzwei);
                    hM_Eta_vtxZ_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20->Fill(recotklraw_eta->at(j), PV_z, vtxzwei);
                    hM_Eta_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20->Fill(recotklraw_eta->at(j), recotklraw_phi->at(j), vtxzwei);
                }
            }

            for (size_t i = 0; i < centrality_cut.size() - 1; i++)
            {
                if (MBD_centrality >= centrality_cut[i] && MBD_centrality < centrality_cut[i + 1])
                {
                    for (size_t j = 0; j < recotklraw_eta->size(); j++)
                    {
                        hM_dEta_reco_Centrality_MBDAsymLe0p75_VtxZm20to20[i]->Fill(recotklraw_deta->at(j), vtxzwei);
                        hM_dPhi_reco_Centrality_MBDAsymLe0p75_VtxZm20to20[i]->Fill(recotklraw_dphi->at(j), vtxzwei);
                        hM_dR_reco_Centrality_MBDAsymLe0p75_VtxZm20to20[i]->Fill(recotklraw_dR->at(j), vtxzwei);
                        hM_Eta_vtxZ_reco_Centrality_MBDAsymLe0p75_VtxZm20to20[i]->Fill(recotklraw_eta->at(j), PV_z, vtxzwei);
                        hM_Eta_Phi_reco_Centrality_MBDAsymLe0p75_VtxZm20to20[i]->Fill(recotklraw_eta->at(j), recotklraw_phi->at(j), vtxzwei);
                        hM_Eta_reco_Centrality_MBDAsymLe0p75_VtxZm20to20[i]->Fill(recotklraw_eta->at(j), vtxzwei);
                    }
                }
            }
        }
    }

    f->Close();

    fout->cd();
    hM_NClusLayer1->Write();
    hM_NClusLayer2->Write();
    hM_NTklclusLayer1->Write();
    hM_NRecotkl_Raw->Write();
    hM_NClusLayer1_zvtxwei->Write();
    hM_NTklclusLayer1_zvtxwei->Write();
    hM_NRecotkl_Raw_zvtxwei->Write();
    hM_RecoPVz->Write();
    hM_RecoPVz_MBDAsymLe0p75_VtxZm20to20->Write();
    hM_MBDChargeAsymm_Le0p75->Write();
    hM_MBDChargeAsymm_Le0p75_VtxZm20to20->Write();
    hM_clusphi->Write();
    hM_cluseta->Write();
    hM_clusphisize->Write();
    hM_clusadc->Write();
    hM_clusphi_clusphisize->Write();
    hM_cluseta_clusphisize->Write();
    hM_clusphi_zvtxwei->Write();
    hM_cluseta_zvtxwei->Write();
    hM_clusphisize_zvtxwei->Write();
    hM_clusphi_clusphisize_zvtxwei->Write();
    hM_cluseta_clusphisize_zvtxwei->Write();
    hM_tklclus1phisize->Write();
    hM_tklclus2phisize->Write();
    hM_tklclus1phisize_tklclus2phisize->Write();
    hM_tklclus1phi_tklclus1phisize->Write();
    hM_tklclus2phi_tklclus2phisize->Write();
    hM_dEta_reco->Write();
    hM_dEta_reco->Write();
    hM_dEta_reco_altrange->Write();
    hM_dEta_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20->Write();
    hM_dPhi_reco->Write();
    hM_dPhi_reco_altrange->Write();
    hM_dPhi_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20->Write();
    hM_dR_reco->Write();
    hM_dR_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20->Write();
    hM_dR_reco_altrange->Write();
    hM_dR_reco_altrange_Centrality0to70_MBDAsymLe0p75_VtxZm20to20->Write();
    hM_dR_reco_LogX->Write();
    hM_Eta_reco->Write();
    hM_Eta_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20->Write();
    hM_Phi_reco->Write();
    hM_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20->Write();
    hM_Eta_vtxZ_reco_incl->Write();
    hM_Eta_vtxZ_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20->Write();
    hM_Eta_Phi_reco_Centrality0to70_MBDAsymLe0p75_VtxZm20to20->Write();
    for (size_t i = 0; i < centrality_cut.size() - 1; i++)
    {
        hM_dEta_reco_Centrality[i]->Write();
        hM_dPhi_reco_Centrality[i]->Write();
        hM_dR_reco_Centrality[i]->Write();
        hM_Eta_vtxZ_reco_Centrality[i]->Write();
        hM_dEta_reco_Centrality_MBDAsymLe0p75[i]->Write();
        hM_dPhi_reco_Centrality_MBDAsymLe0p75[i]->Write();
        hM_dR_reco_Centrality_MBDAsymLe0p75[i]->Write();
        hM_Eta_vtxZ_reco_Centrality_MBDAsymLe0p75[i]->Write();
        hM_dEta_reco_Centrality_MBDAsymLe0p75_VtxZm20to20[i]->Write();
        hM_dPhi_reco_Centrality_MBDAsymLe0p75_VtxZm20to20[i]->Write();
        hM_dR_reco_Centrality_MBDAsymLe0p75_VtxZm20to20[i]->Write();
        hM_Eta_vtxZ_reco_Centrality_MBDAsymLe0p75_VtxZm20to20[i]->Write();
        hM_Eta_Phi_reco_Centrality_MBDAsymLe0p75_VtxZm20to20[i]->Write();
        hM_Eta_reco_Centrality_MBDAsymLe0p75_VtxZm20to20[i]->Write();
    }

    fout->Close();
}

int main(int argc, char *argv[])
{
    TString infname;
    TString outfname;

    if (argc == 1)
    {
        infname = "/sphenix/user/hjheng/TrackletAna/minitree/INTT/TrackletMinitree_ana382_zvtx-20cm_dummyAlignParams/TrackletAna_minitree_Evt0to2000_dRcut0p5.root";
        outfname = "/sphenix/user/hjheng/TrackletAna/analysis_INTT/plot/hists/ana382_zvtx-20cm_dummyAlignParams/Hists_RecoTracklets.root";
    }
    else if (argc == 3)
    {
        infname = argv[1];
        outfname = argv[2];
    }
    else
    {
        std::cout << "Usage: ./plotTracklets [infile] [outfile]" << std::endl;
        return 0;
    }

    cout << "[Run Info] Input file = " << infname << endl << "           Output file = " << outfname << endl << "-----------" << endl;

    makehist(infname, outfname);

    return 0;
}
