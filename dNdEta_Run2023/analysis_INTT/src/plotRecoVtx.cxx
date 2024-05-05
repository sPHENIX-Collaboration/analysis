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

#include "Utilities.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: ./plotRecoVtx [infilename (ntuple)] [outfilename (histogram)]" << std::endl;
        return 0;
    }

    for (int i = 0; i < argc; i++)
    {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    }

    TString infilename = TString(argv[1]);
    TString outfilename = TString(argv[2]);

    TH2F *hM_INTTVtxZ_MBDVtxZ_Inclusive = new TH2F("hM_INTTVtxZ_MBDVtxZ_Inclusive", "hM_INTTVtxZ_MBDVtxZ_Inclusive", 160, -60, 20, 160, -60, 20);
    TH1F *hM_INTTVtxZ_Inclusive = new TH1F("hM_INTTVtxZ_Inclusive", "hM_INTTVtxZ_Inclusive", 160, -60, 20);
    TH1F *hM_INTTVtxZ_MBDAsymLe0p75 = new TH1F("hM_INTTVtxZ_MBDAsymLe0p75", "hM_INTTVtxZ_MBDAsymLe0p75", 160, -60, 20);
    TH1F *hM_INTTVtxZ_MBDAsymLe0p75_VtxZm30tom10 = new TH1F("hM_INTTVtxZ_MBDAsymLe0p75_VtxZm30tom10", "hM_INTTVtxZ_MBDAsymLe0p75_VtxZm30tom10", 220, -31, -9);
    TH1F *hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10 = new TH1F("hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10", "hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10", 220, -31, -9);
    TH1F *hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse = new TH1F("hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse", "hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse", 40, -40, -0);
    TH2F *hM_INTTVtxZ_MBDAsymm_Centrality0to70_Inclusive = new TH2F("hM_INTTVtxZ_MBDAsymm_Centrality0to70_Inclusive", "hM_INTTVtxZ_MBDAsymm_Centrality0to70_Inclusive", 160, -60, 20, 200, -1, 1);
    TH2F *hM_INTTVtxZ_MBDAsymm_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10 = new TH2F("hM_INTTVtxZ_MBDAsymm_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10", "hM_INTTVtxZ_MBDAsymm_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10", 220, -31, -9, 200, -1, 1);
    vector<float> centrality_cut = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    vector<TH1F *> hM_INTTVtxZ_Centrality;
    vector<TH1F *> hM_INTTVtxZ_Centrality_MBDAsymLe0p75;
    vector<TH2F *> hM_INTTVtxZ_MBDVtxZ_Centrality;
    vector<TH1F *> hM_INTTVtxZ_Centrality_MBDAsymLe0p75_VtxZm30tom10;
    vector<TH2F *> hM_INTTVtxZ_MBDAsymm_Centrality_MBDAsymLe0p75_VtxZm30tom10;
    for (int i = 0; i < centrality_cut.size() - 1; i++)
    {
        hM_INTTVtxZ_Centrality.push_back(new TH1F(Form("hM_INTTVtxZ_Centrality_%dto%d", (int)centrality_cut[i], (int)centrality_cut[i + 1]), Form("hM_INTTVtxZ_Centrality_%dto%d", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 160, -60, 20));
        hM_INTTVtxZ_Centrality_MBDAsymLe0p75.push_back(new TH1F(Form("hM_INTTVtxZ_Centrality_%dto%d_MBDAsymLe0p75", (int)centrality_cut[i], (int)centrality_cut[i + 1]), Form("hM_INTTVtxZ_Centrality_%dto%d_MBDAsymLe0p75", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 160, -60, 20));
        hM_INTTVtxZ_MBDVtxZ_Centrality.push_back(new TH2F(Form("hM_INTTVtxZ_MBDVtxZ_Centrality_%dto%d", (int)centrality_cut[i], (int)centrality_cut[i + 1]), Form("hM_INTTVtxZ_MBDVtxZ_Centrality_%dto%d", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 160, -60, 20, 160, -60, 20));
        hM_INTTVtxZ_Centrality_MBDAsymLe0p75_VtxZm30tom10.push_back(new TH1F(Form("hM_INTTVtxZ_Centrality_%dto%d_MBDAsymLe0p75_VtxZm30tom10", (int)centrality_cut[i], (int)centrality_cut[i + 1]), Form("hM_INTTVtxZ_Centrality_%dto%d_MBDAsymLe0p75_VtxZm30tom10", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 220, -31, -9));
        hM_INTTVtxZ_MBDAsymm_Centrality_MBDAsymLe0p75_VtxZm30tom10.push_back(new TH2F(Form("hM_INTTVtxZ_MBDAsymm_Centrality_%dto%d_MBDAsymLe0p75_VtxZm30tom10", (int)centrality_cut[i], (int)centrality_cut[i + 1]), Form("hM_INTTVtxZ_MBDAsymm_Centrality_%dto%d_MBDAsymLe0p75_VtxZm30tom10", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 220, -31, -9, 200, -1, 1));
    }

    TFile *f = new TFile(infilename, "READ");
    TTree *t = (TTree *)f->Get("minitree");
    int event;
    bool is_min_bias;
    int NClusLayer1, NTruthVtx;
    float TruthPV_x, TruthPV_y, TruthPV_z, Centrality_bimp, Centrality_impactparam, MBD_centrality, mbd_charge_sum, mbd_charge_asymm, mbd_z_vtx, PV_x, PV_y, PV_z;
    t->SetBranchAddress("event", &event);
    t->SetBranchAddress("is_min_bias", &is_min_bias);
    t->SetBranchAddress("NClusLayer1", &NClusLayer1);
    t->SetBranchAddress("NTruthVtx", &NTruthVtx);
    t->SetBranchAddress("TruthPV_x", &TruthPV_x);
    t->SetBranchAddress("TruthPV_y", &TruthPV_y);
    t->SetBranchAddress("TruthPV_z", &TruthPV_z);
    t->SetBranchAddress("Centrality_bimp", &Centrality_bimp);
    t->SetBranchAddress("Centrality_impactparam", &Centrality_impactparam);
    t->SetBranchAddress("MBD_centrality", &MBD_centrality);
    t->SetBranchAddress("mbd_charge_sum", &mbd_charge_sum);
    t->SetBranchAddress("mbd_charge_asymm", &mbd_charge_asymm);
    t->SetBranchAddress("mbd_z_vtx", &mbd_z_vtx);
    t->SetBranchAddress("PV_x", &PV_x);
    t->SetBranchAddress("PV_y", &PV_y);
    t->SetBranchAddress("PV_z", &PV_z);

    for (int ev = 0; ev < t->GetEntriesFast(); ev++)
    {
        t->GetEntry(ev);
        
        hM_INTTVtxZ_MBDVtxZ_Inclusive->Fill(PV_z, mbd_z_vtx);
        hM_INTTVtxZ_Inclusive->Fill(PV_z);

        if (MBD_centrality >= 0 && MBD_centrality <= 0.7)
        {
            hM_INTTVtxZ_MBDAsymm_Centrality0to70_Inclusive->Fill(PV_z, mbd_charge_asymm);
        }

        if (fabs(mbd_charge_asymm) <= 0.75)
        {
            hM_INTTVtxZ_MBDAsymLe0p75->Fill(PV_z);
            if (PV_z >= -30 && PV_z <= -10)
            {
                hM_INTTVtxZ_MBDAsymLe0p75_VtxZm30tom10->Fill(PV_z);
                if (MBD_centrality >= 0 && MBD_centrality <= 0.7)
                {
                    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10->Fill(PV_z);
                    hM_INTTVtxZ_MBDAsymm_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10->Fill(PV_z, mbd_charge_asymm);
                }
            }

            // for reweighting
            if ((PV_z >= -40 && PV_z <= 0) && (MBD_centrality >= 0 && MBD_centrality <= 0.7) && fabs(mbd_charge_asymm) <= 0.75)
            {
                hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse->Fill(PV_z);
            }
        }

        for (int i = 0; i < centrality_cut.size() - 1; i++)
        {
            if (MBD_centrality >= centrality_cut[i]*0.01 && MBD_centrality < centrality_cut[i + 1]*0.01)
            {
                hM_INTTVtxZ_Centrality[i]->Fill(PV_z);
                hM_INTTVtxZ_MBDVtxZ_Centrality[i]->Fill(PV_z, mbd_z_vtx);

                if (fabs(mbd_charge_asymm) <= 0.75)
                {
                    hM_INTTVtxZ_Centrality_MBDAsymLe0p75[i]->Fill(PV_z);
                    if (PV_z >= -30 && PV_z <= -10)
                    {
                        hM_INTTVtxZ_Centrality_MBDAsymLe0p75_VtxZm30tom10[i]->Fill(PV_z);
                        hM_INTTVtxZ_MBDAsymm_Centrality_MBDAsymLe0p75_VtxZm30tom10[i]->Fill(PV_z, mbd_charge_asymm);
                    }
                }
            }
        }
    }

    TFile *fout = new TFile(outfilename, "RECREATE");
    fout->cd();
    hM_INTTVtxZ_MBDVtxZ_Inclusive->Write();
    hM_INTTVtxZ_Inclusive->Write();
    hM_INTTVtxZ_MBDAsymLe0p75->Write();
    hM_INTTVtxZ_MBDAsymLe0p75_VtxZm30tom10->Write();
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10->Write();
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe0p75_VtxZm40tom0_coarse->Write();
    hM_INTTVtxZ_MBDAsymm_Centrality0to70_MBDAsymLe0p75_VtxZm30tom10->Write();
    hM_INTTVtxZ_MBDAsymm_Centrality0to70_Inclusive->Write();
    for (int i = 0; i < centrality_cut.size() - 1; i++)
    {
        hM_INTTVtxZ_Centrality[i]->Write();
        hM_INTTVtxZ_MBDVtxZ_Centrality[i]->Write();
        hM_INTTVtxZ_Centrality_MBDAsymLe0p75[i]->Write();
        hM_INTTVtxZ_Centrality_MBDAsymLe0p75_VtxZm30tom10[i]->Write();
        hM_INTTVtxZ_MBDAsymm_Centrality_MBDAsymLe0p75_VtxZm30tom10[i]->Write();
    }
    fout->Close();
}