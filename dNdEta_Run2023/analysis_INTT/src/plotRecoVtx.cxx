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
    if (argc != 4)
    {
        std::cout << "Usage: ./plotRecoVtx [isdata] [infilename (ntuple)] [outfilename (histogram)]" << std::endl;
        return 0;
    }

    for (int i = 0; i < argc; i++)
    {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    }

    bool isdata = atoi(argv[1]);
    TString infilename = TString(argv[2]);
    TString outfilename = TString(argv[3]);

    TH2F *hM_INTTVtxZ_MBDVtxZ_Inclusive = new TH2F("hM_INTTVtxZ_MBDVtxZ_Inclusive", "hM_INTTVtxZ_MBDVtxZ_Inclusive", 160, -40, 40, 160, -40, 40);
    TH1F *hM_INTTVtxZ_Inclusive = new TH1F("hM_INTTVtxZ_Inclusive", "hM_INTTVtxZ_Inclusive", 160, -40, 40);
    TH1F *hM_INTTVtxZ_MBDAsymLe1 = new TH1F("hM_INTTVtxZ_MBDAsymLe1", "hM_INTTVtxZ_MBDAsymLe1", 160, -40, 40);
    TH1F *hM_INTTVtxZ_MBDAsymLe1_VtxZm10to10 = new TH1F("hM_INTTVtxZ_MBDAsymLe1_VtxZm10to10", "hM_INTTVtxZ_MBDAsymLe1_VtxZm10to10", 100, -10, 10);
    TH1F *hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm10to10 = new TH1F("hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm10to10", "hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm10to10", 100, -10, 10);
    // for reweighting
    TH1F *hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm30to30_coarse = new TH1F("hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm30to30_coarse", "hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm30to30_coarse", 60, -30, 30);
    TH1F *hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm10to10_coarse = new TH1F("hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm10to10_coarse", "hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm10to10_coarse", 20, -10, 10);
    TH2F *hM_INTTVtxZ_MBDAsymm_Centrality0to70_Inclusive = new TH2F("hM_INTTVtxZ_MBDAsymm_Centrality0to70_Inclusive", "hM_INTTVtxZ_MBDAsymm_Centrality0to70_Inclusive", 160, -40, 40, 200, -1, 1);
    TH2F *hM_INTTVtxZ_MBDAsymm_Centrality0to70_MBDAsymLe1_VtxZm10to10 = new TH2F("hM_INTTVtxZ_MBDAsymm_Centrality0to70_MBDAsymLe1_VtxZm10to10", "hM_INTTVtxZ_MBDAsymm_Centrality0to70_MBDAsymLe1_VtxZm10to10", 200, -10, 10, 200, -1, 1);
    TH2F *hM_INTTVtxZ_MBDVtxZ_Centrality0to70_MBDAsymLe1 = new TH2F("hM_INTTVtxZ_MBDVtxZ_Centrality0to70_MBDAsymLe1", "hM_INTTVtxZ_MBDVtxZ_Centrality0to70_MBDAsymLe1", 160, -40, 40, 160, -40, 40);
    vector<float> centrality_cut = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    vector<TH1F *> hM_INTTVtxZ_Centrality;
    vector<TH1F *> hM_INTTVtxZ_Centrality_MBDAsymLe1;
    vector<TH2F *> hM_INTTVtxZ_MBDVtxZ_Centrality;
    vector<TH1F *> hM_INTTVtxZ_Centrality_MBDAsymLe1_VtxZm10to10;
    vector<TH2F *> hM_INTTVtxZ_MBDAsymm_Centrality_MBDAsymLe1_VtxZm10to10;
    for (int i = 0; i < centrality_cut.size() - 1; i++)
    {
        hM_INTTVtxZ_Centrality.push_back(new TH1F(Form("hM_INTTVtxZ_Centrality_%dto%d", (int)centrality_cut[i], (int)centrality_cut[i + 1]), Form("hM_INTTVtxZ_Centrality_%dto%d", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 160, -40, 40));
        hM_INTTVtxZ_Centrality_MBDAsymLe1.push_back(new TH1F(Form("hM_INTTVtxZ_Centrality_%dto%d_MBDAsymLe1", (int)centrality_cut[i], (int)centrality_cut[i + 1]), Form("hM_INTTVtxZ_Centrality_%dto%d_MBDAsymLe1", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 160, -40, 40));
        hM_INTTVtxZ_MBDVtxZ_Centrality.push_back(new TH2F(Form("hM_INTTVtxZ_MBDVtxZ_Centrality_%dto%d", (int)centrality_cut[i], (int)centrality_cut[i + 1]), Form("hM_INTTVtxZ_MBDVtxZ_Centrality_%dto%d", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 160, -40, 40, 160, -40, 40));
        hM_INTTVtxZ_Centrality_MBDAsymLe1_VtxZm10to10.push_back(new TH1F(Form("hM_INTTVtxZ_Centrality_%dto%d_MBDAsymLe1_VtxZm10to10", (int)centrality_cut[i], (int)centrality_cut[i + 1]), Form("hM_INTTVtxZ_Centrality_%dto%d_MBDAsymLe1_VtxZm10to10", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 100, -10, 10));
        hM_INTTVtxZ_MBDAsymm_Centrality_MBDAsymLe1_VtxZm10to10.push_back(new TH2F(Form("hM_INTTVtxZ_MBDAsymm_Centrality_%dto%d_MBDAsymLe1_VtxZm10to10", (int)centrality_cut[i], (int)centrality_cut[i + 1]), Form("hM_INTTVtxZ_MBDAsymm_Centrality_%dto%d_MBDAsymLe1_VtxZm10to10", (int)centrality_cut[i], (int)centrality_cut[i + 1]), 100, -10, 10, 200, -1, 1));
    }

    TFile *f = new TFile(infilename, "READ");
    TTree *t = (TTree *)f->Get("minitree");
    int event;
    vector<int> *firedTriggers = 0;
    bool is_min_bias;
    int NClusLayer1, NTruthVtx;
    float TruthPV_x, TruthPV_y, TruthPV_z, Centrality_bimp, Centrality_impactparam, MBD_centrality, mbd_charge_sum, mbd_south_charge_sum, mbd_north_charge_sum, mbd_charge_asymm, mbd_z_vtx, PV_x, PV_y, PV_z;
    t->SetBranchAddress("event", &event);
    if (isdata)
    {
        t->SetBranchAddress("firedTriggers", &firedTriggers);
    }
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
    t->SetBranchAddress("mbd_south_charge_sum", &mbd_south_charge_sum);
    t->SetBranchAddress("mbd_north_charge_sum", &mbd_north_charge_sum);
    t->SetBranchAddress("mbd_charge_asymm", &mbd_charge_asymm);
    t->SetBranchAddress("mbd_z_vtx", &mbd_z_vtx);
    t->SetBranchAddress("PV_x", &PV_x);
    t->SetBranchAddress("PV_y", &PV_y);
    t->SetBranchAddress("PV_z", &PV_z);

    for (int ev = 0; ev < t->GetEntriesFast(); ev++)
    {
        t->GetEntry(ev);

        // set up selection criteria
        bool goodMbdAsymm = (fabs(mbd_charge_asymm) <= 1.0);
        bool validMbdVtx = (mbd_z_vtx == mbd_z_vtx);
        bool MbdNScharnge = (mbd_north_charge_sum > 0 || mbd_south_charge_sum > 0);
        bool MbdZvtxRange_wide = (mbd_z_vtx >= -30 && mbd_z_vtx <= 30);
        bool MbdZvtxRange_narrow = (mbd_z_vtx >= -10 && mbd_z_vtx <= 10);
        bool firedTrig10 = (isdata) ? (find(firedTriggers->begin(), firedTriggers->end(), 10) != firedTriggers->end()) : true; // MBD N&S >= 2
        bool firedTrig12 = (isdata) ? (find(firedTriggers->begin(), firedTriggers->end(), 12) != firedTriggers->end()) : true; // MBD N&S >= 2, vtx < 10 cm
        bool firedTrig13 = (isdata) ? (find(firedTriggers->begin(), firedTriggers->end(), 13) != firedTriggers->end()) : true; // MBD N&S >= 2, vtx < 30 cm
        bool goodCentrality = (MBD_centrality >= 0 && MBD_centrality <= 70);
        // bool goodCentrality = true;
        bool InttZvtxRange_wide = (PV_z >= -30 && PV_z <= 30);
        bool InttZvtxRange_narrow = (PV_z >= -10 && PV_z <= 10);

        // Histograms - Inclusive, wide z vtx range
        if (is_min_bias && validMbdVtx && MbdNScharnge && firedTrig10 && MbdZvtxRange_wide)
        {
            hM_INTTVtxZ_MBDVtxZ_Inclusive->Fill(PV_z, mbd_z_vtx);
            hM_INTTVtxZ_Inclusive->Fill(PV_z);

            if (goodCentrality)
            {
                hM_INTTVtxZ_MBDAsymm_Centrality0to70_Inclusive->Fill(PV_z, mbd_charge_asymm);
            }
        }

        if (is_min_bias && goodMbdAsymm)
        {
            hM_INTTVtxZ_MBDAsymLe1->Fill(PV_z);

            if (InttZvtxRange_narrow)
            {
                hM_INTTVtxZ_MBDAsymLe1_VtxZm10to10->Fill(PV_z);
                if (goodCentrality)
                {
                    hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm10to10->Fill(PV_z);
                    hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm10to10_coarse->Fill(PV_z); // for reweighting
                    hM_INTTVtxZ_MBDAsymm_Centrality0to70_MBDAsymLe1_VtxZm10to10->Fill(PV_z, mbd_charge_asymm);
                }
            }

            if (InttZvtxRange_wide)
            {
                if (goodCentrality)
                {
                    hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm30to30_coarse->Fill(PV_z); // for reweighting
                }
            }

            if (goodCentrality)
            {
                hM_INTTVtxZ_MBDVtxZ_Centrality0to70_MBDAsymLe1->Fill(PV_z, mbd_z_vtx);
            }
        }

        for (int i = 0; i < centrality_cut.size() - 1; i++)
        {
            if (is_min_bias && MBD_centrality >= centrality_cut[i] && MBD_centrality < centrality_cut[i + 1])
            {
                hM_INTTVtxZ_Centrality[i]->Fill(PV_z);
                hM_INTTVtxZ_MBDVtxZ_Centrality[i]->Fill(PV_z, mbd_z_vtx);

                if (goodMbdAsymm)
                {
                    hM_INTTVtxZ_Centrality_MBDAsymLe1[i]->Fill(PV_z);
                    if (InttZvtxRange_narrow)
                    {
                        hM_INTTVtxZ_Centrality_MBDAsymLe1_VtxZm10to10[i]->Fill(PV_z);
                        hM_INTTVtxZ_MBDAsymm_Centrality_MBDAsymLe1_VtxZm10to10[i]->Fill(PV_z, mbd_charge_asymm);
                    }
                }
            }
        }
    }

    TFile *fout = new TFile(outfilename, "RECREATE");
    fout->cd();
    hM_INTTVtxZ_MBDVtxZ_Inclusive->Write();
    hM_INTTVtxZ_Inclusive->Write();
    hM_INTTVtxZ_MBDAsymLe1->Write();
    hM_INTTVtxZ_MBDAsymLe1_VtxZm10to10->Write();
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm10to10->Write();
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm30to30_coarse->Write(); // for reweighting
    hM_INTTVtxZ_Centrality0to70_MBDAsymLe1_VtxZm10to10_coarse->Write(); // for reweighting
    hM_INTTVtxZ_MBDAsymm_Centrality0to70_MBDAsymLe1_VtxZm10to10->Write();
    hM_INTTVtxZ_MBDAsymm_Centrality0to70_Inclusive->Write();
    hM_INTTVtxZ_MBDVtxZ_Centrality0to70_MBDAsymLe1->Write();
    for (int i = 0; i < centrality_cut.size() - 1; i++)
    {
        hM_INTTVtxZ_Centrality[i]->Write();
        hM_INTTVtxZ_MBDVtxZ_Centrality[i]->Write();
        hM_INTTVtxZ_Centrality_MBDAsymLe1[i]->Write();
        hM_INTTVtxZ_Centrality_MBDAsymLe1_VtxZm10to10[i]->Write();
        hM_INTTVtxZ_MBDAsymm_Centrality_MBDAsymLe1_VtxZm10to10[i]->Write();
    }
    fout->Close();
}