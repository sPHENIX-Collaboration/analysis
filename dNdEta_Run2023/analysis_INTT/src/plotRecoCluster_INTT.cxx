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

#include "GenHadron.h"
#include "Tracklet.h"
#include "Vertex.h"

int main(int argc, char *argv[])
{
    bool rundata = true;
    TString infilename = "/sphenix/user/hjheng/TrackletAna/data/INTT/INTTRecoClusters_test.root";
    TString outfilename = Form("/sphenix/user/hjheng/TrackletAna/analysis/plot/hists/INTTData/Hists_RecoClusters_INTT.root");
    // TString EvtVtx_map_filename = "/sphenix/user/hjheng/TrackletAna/minitree/AuAu_ana325private_NoPileup_RecoVtx_Optimization/TrackletAna_RecoClusters_RecoVtx_TklCluster_dPhiCutbin3_dZCutbin10.root";

    // std::map<int, float> EvtVtx_map = EvtVtx_map_tklcluster(EvtVtx_map_filename.Data());

    TH1F *hM_ClusX_all = new TH1F("hM_ClusX_all", "hM_ClusX_all", 200, -10, 10);
    TH1F *hM_ClusX_layer1 = new TH1F("hM_ClusX_layer1", "hM_ClusX_layer1", 200, -10, 10);
    TH1F *hM_ClusX_layer2 = new TH1F("hM_ClusX_layer2", "hM_ClusX_layer2", 200, -10, 10);
    TH1F *hM_ClusY_all = new TH1F("hM_ClusY_all", "hM_ClusY_all", 200, -10, 10);
    TH1F *hM_ClusY_layer1 = new TH1F("hM_ClusY_layer1", "hM_ClusY_layer1", 200, -10, 10);
    TH1F *hM_ClusY_layer2 = new TH1F("hM_ClusY_layer2", "hM_ClusY_layer2", 200, -10, 10);
    TH1F *hM_ClusZ_all = new TH1F("hM_ClusZ_all", "hM_ClusZ_all", 200, -25, 25);
    TH1F *hM_ClusZ_layer1 = new TH1F("hM_ClusZ_layer1", "hM_ClusZ_layer1", 200, -25, 25);
    TH1F *hM_ClusZ_layer2 = new TH1F("hM_ClusZ_layer2", "hM_ClusZ_layer2", 200, -25, 25);
    TH1F *hM_ClusR_all = new TH1F("hM_ClusR_all", "hM_ClusR_all", 120, 4, 7);
    TH1F *hM_ClusR_layer1 = new TH1F("hM_ClusR_layer1", "hM_ClusR_layer1", 120, 4, 7);
    TH1F *hM_ClusR_layer2 = new TH1F("hM_ClusR_layer2", "hM_ClusR_layer2", 120, 4, 7);
    // TH1F *hM_ClusEtaPV_all = new TH1F("hM_ClusEtaPV_all", "hM_ClusEtaPV_all", 160, -4, 4);
    // TH1F *hM_ClusEtaPV_layer1 = new TH1F("hM_ClusEtaPV_layer1", "hM_ClusEtaPV_layer1", 160, -4, 4);
    // TH1F *hM_ClusEtaPV_layer2 = new TH1F("hM_ClusEtaPV_layer2", "hM_ClusEtaPV_layer2", 160, -4, 4);
    TH1F *hM_ClusEtaOri_all = new TH1F("hM_ClusEtaOri_all", "hM_ClusEtaOri_all", 120, -3, 3);
    TH1F *hM_ClusEtaOri_layer1 = new TH1F("hM_ClusEtaOri_layer1", "hM_ClusEtaOri_layer1", 120, -3, 3);
    TH1F *hM_ClusEtaOri_layer2 = new TH1F("hM_ClusEtaOri_layer2", "hM_ClusEtaOri_layer2", 120, -3, 3);
    TH1F *hM_ClusPhi_all = new TH1F("hM_ClusPhi_all", "hM_ClusPhi_all", 140, -3.5, 3.5);
    TH1F *hM_ClusPhi_layer1 = new TH1F("hM_ClusPhi_layer1", "hM_ClusPhi_layer1", 140, -3.5, 3.5);
    TH1F *hM_ClusPhi_layer2 = new TH1F("hM_ClusPhi_layer2", "hM_ClusPhi_layer2", 140, -3.5, 3.5);
    TH1F *hM_ClusADC_all = new TH1F("hM_ClusADC_all", "hM_ClusADC_all", 100, 0, 100);
    TH1F *hM_ClusADC_layer1 = new TH1F("hM_ClusADC_layer1", "hM_ClusADC_layer1", 100, 0, 100);
    TH1F *hM_ClusADC_layer2 = new TH1F("hM_ClusADC_layer2", "hM_ClusADC_layer2", 100, 0, 100);
    TH1F *hM_ClusZSize_all = new TH1F("hM_ClusZSize_all", "hM_ClusZSize_all", 20, 0, 20);
    TH1F *hM_ClusZSize_layer1 = new TH1F("hM_ClusZSize_layer1", "hM_ClusZSize_layer1", 20, 0, 20);
    TH1F *hM_ClusZSize_layer2 = new TH1F("hM_ClusZSize_layer2", "hM_ClusZSize_layer2", 20, 0, 20);
    TH1F *hM_ClusPhiSize_all = new TH1F("hM_ClusPhiSize_all", "hM_ClusPhiSize_all", 40, 0, 40);
    TH1F *hM_ClusPhiSize_layer1 = new TH1F("hM_ClusPhiSize_layer1", "hM_ClusPhiSize_layer1", 40, 0, 40);
    TH1F *hM_ClusPhiSize_layer2 = new TH1F("hM_ClusPhiSize_layer2", "hM_ClusPhiSize_layer2", 40, 0, 40);
    // TH1F *hM_ClusPairDr_layer1 = new TH1F("hM_ClusPairDr_layer1", "hM_ClusPairDr_layer1", 50, 0, 0.1);
    // TH1F *hM_ClusPairDr_layer2 = new TH1F("hM_ClusPairDr_layer2", "hM_ClusPairDr_layer2", 50, 0, 0.1);
    TH2F *hM_ClusX_ClusY_all = new TH2F("hM_ClusX_ClusY_all", "hM_ClusX_ClusY_all", 260, -13, 13, 260, -13, 13);
    TH2F *hM_ClusZ_ClusPhi_all = new TH2F("hM_ClusZ_ClusPhi_all", "hM_ClusZ_ClusPhi_all", 1000, -25, 25, 350, -3.5, 3.5);
    TH2F *hM_ClusZ_ClusPhi_layer1 = new TH2F("hM_ClusZ_ClusPhi_layer1", "hM_ClusZ_ClusPhi_layer1", 1000, -25, 25, 350, -3.5, 3.5);
    TH2F *hM_ClusZ_ClusPhi_layer2 = new TH2F("hM_ClusZ_ClusPhi_layer2", "hM_ClusZ_ClusPhi_layer2", 1000, -25, 25, 350, -3.5, 3.5);
    TH2F *hM_ClusEta_ClusZSize_all = new TH2F("hM_ClusEta_ClusZSize_all", "hM_ClusEta_ClusZSize_all", 160, -4, 4, 20, 0, 20);
    TH2F *hM_ClusEta_ClusZSize_layer1 = new TH2F("hM_ClusEta_ClusZSize_layer1", "hM_ClusEta_ClusZSize_layer1", 160, -4, 4, 20, 0, 20);
    TH2F *hM_ClusEta_ClusZSize_layer2 = new TH2F("hM_ClusEta_ClusZSize_layer2", "hM_ClusEta_ClusZSize_layer2", 160, -4, 4, 20, 0, 20);
    TH2F *hM_ClusPhi_ClusPhiSize_all = new TH2F("hM_ClusPhi_ClusPhiSize_all", "hM_ClusPhi_ClusPhiSize_all", 140, -3.5, 3.5, 20, 0, 20);
    TH2F *hM_ClusPhi_ClusPhiSize_layer1 = new TH2F("hM_ClusPhi_ClusPhiSize_layer1", "hM_ClusPhi_ClusPhiSize_layer1", 140, -3.5, 3.5, 20, 0, 20);
    TH2F *hM_ClusPhi_ClusPhiSize_layer2 = new TH2F("hM_ClusPhi_ClusPhiSize_layer2", "hM_ClusPhi_ClusPhiSize_layer2", 140, -3.5, 3.5, 20, 0, 20);
    TH2F *hM_ClusZSize_ClusPhiSize_all = new TH2F("hM_ClusZSize_ClusPhiSize_all", "hM_ClusZSize_ClusPhiSize_all", 20, 0, 20, 20, 0, 20);
    TH2F *hM_ClusZSize_ClusPhiSize_layer1 = new TH2F("hM_ClusZSize_ClusPhiSize_layer1", "hM_ClusZSize_ClusPhiSize_layer1", 20, 0, 20, 20, 0, 20);
    TH2F *hM_ClusZSize_ClusPhiSize_layer2 = new TH2F("hM_ClusZSize_ClusPhiSize_layer2", "hM_ClusZSize_ClusPhiSize_layer2", 20, 0, 20, 20, 0, 20);
    TH2F *hM_ClusEta_ClusADC_all = new TH2F("hM_ClusEta_ClusADC_all", "hM_ClusEta_ClusADC_all", 160, -4, 4, 50, 0, 50);
    TH2F *hM_ClusEta_ClusADC_layer1 = new TH2F("hM_ClusEta_ClusADC_layer1", "hM_ClusEta_ClusADC_layer1", 160, -4, 4, 50, 0, 50);
    TH2F *hM_ClusEta_ClusADC_layer2 = new TH2F("hM_ClusEta_ClusADC_layer2", "hM_ClusEta_ClusADC_layer2", 160, -4, 4, 50, 0, 50);
    TH2F *hM_ClusPhiSize_ClusADC_all = new TH2F("hM_ClusPhiSize_ClusADC_all", "hM_ClusPhiSize_ClusADC_all", 40, 0, 40, 100, 0, 100);
    TH2F *hM_ClusPhiSize_ClusADC_layer1 = new TH2F("hM_ClusPhiSize_ClusADC_layer1", "hM_ClusPhiSize_ClusADC_layer1", 40, 0, 40, 100, 0, 100);
    TH2F *hM_ClusPhiSize_ClusADC_layer2 = new TH2F("hM_ClusPhiSize_ClusADC_layer2", "hM_ClusPhiSize_ClusADC_layer2", 40, 0, 40, 100, 0, 100);

    TFile *f = new TFile(infilename, "READ");
    TTree *t = (TTree *)f->Get("EventTree");
    t->BuildIndex("event"); // Reference: https://root-forum.cern.ch/t/sort-ttree-entries/13138
    TTreeIndex *index = (TTreeIndex *)t->GetTreeIndex();
    int event, NTruthVtx;
    float TruthPV_trig_z, TruthPV_mostNpart_z;
    vector<int> *ClusLayer = 0;
    vector<float> *ClusX = 0, *ClusY = 0, *ClusZ = 0, *ClusR = 0, *ClusPhi = 0, *ClusEta = 0, *ClusPhiSize = 0, *ClusZSize = 0;
    vector<unsigned int> *ClusAdc = 0;
    t->SetBranchAddress("event", &event);
    t->SetBranchAddress("NTruthVtx", &NTruthVtx);
    t->SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z);
    t->SetBranchAddress("TruthPV_mostNpart_z", &TruthPV_mostNpart_z);
    t->SetBranchAddress("ClusLayer", &ClusLayer);
    t->SetBranchAddress("ClusX", &ClusX);
    t->SetBranchAddress("ClusY", &ClusY);
    t->SetBranchAddress("ClusZ", &ClusZ);
    t->SetBranchAddress("ClusR", &ClusR);
    t->SetBranchAddress("ClusPhi", &ClusPhi);
    t->SetBranchAddress("ClusEta", &ClusEta);
    t->SetBranchAddress("ClusAdc", &ClusAdc);
    t->SetBranchAddress("ClusPhiSize", &ClusPhiSize);
    t->SetBranchAddress("ClusZSize", &ClusZSize);
    for (int i = 0; i < t->GetEntriesFast(); i++)
    {
        Long64_t local = t->LoadTree(index->GetIndex()[i]);
        t->GetEntry(local);

        // float PV_z = EvtVtx_map[event];
        float PV_z = -20.; // FIX!

        // vector<Hit *> l1hits, l2hits, l3hits;
        // l1hits.clear();
        // l2hits.clear();
        // l3hits.clear();

        for (size_t i = 0; i < ClusLayer->size(); i++)
        {   
            // float x0 = ClusX->at(i);
            // float y0 = ClusY->at(i);
            // float z0 = ClusZ->at(i);
            // vector<float> cpos = {x0, y0, z0};
            // UpdatePos_GapTwoHalves(cpos, gap_north, cent_shift, gap_upper);
            // Hit *hit = new Hit(cpos[0], cpos[1], cpos[2], 0., 0., PV_z, ClusLayer->at(i));

            hM_ClusX_all->Fill(ClusX->at(i));
            hM_ClusY_all->Fill(ClusY->at(i));
            hM_ClusZ_all->Fill(ClusZ->at(i));
            hM_ClusR_all->Fill(ClusR->at(i));
            // hM_ClusEtaPV_all->Fill(ClusEta->at(i));
            hM_ClusEtaOri_all->Fill(ClusEta->at(i));
            hM_ClusPhi_all->Fill(ClusPhi->at(i));
            hM_ClusADC_all->Fill(ClusAdc->at(i));
            // hM_ClusZSize_all->Fill(ClusZSize->at(i));
            hM_ClusPhiSize_all->Fill(ClusPhiSize->at(i));
            hM_ClusX_ClusY_all->Fill(ClusX->at(i), ClusY->at(i));
            hM_ClusZ_ClusPhi_all->Fill(ClusZ->at(i), ClusPhi->at(i));
            // hM_ClusEta_ClusZSize_all->Fill(ClusEta->at(i), ClusZSize->at(i));
            hM_ClusPhi_ClusPhiSize_all->Fill(ClusPhi->at(i), ClusPhiSize->at(i));
            // hM_ClusZSize_ClusPhiSize_all->Fill(ClusZSize->at(i), ClusPhiSize->at(i));
            hM_ClusEta_ClusADC_all->Fill(ClusEta->at(i), ClusAdc->at(i));
            // hM_ClusZSize_ClusADC_all->Fill(ClusZSize->at(i), ClusAdc->at(i));
            hM_ClusPhiSize_ClusADC_all->Fill(ClusPhiSize->at(i), ClusAdc->at(i));
            if (ClusLayer->at(i) == 3 || ClusLayer->at(i) == 4)
            {
                // l1hits.push_back(hit);
                hM_ClusX_layer1->Fill(ClusX->at(i));
                hM_ClusY_layer1->Fill(ClusY->at(i));
                hM_ClusZ_layer1->Fill(ClusZ->at(i));
                hM_ClusR_layer1->Fill(ClusR->at(i));
                // hM_ClusEtaPV_layer1->Fill(ClusEta->at(i));
                hM_ClusEtaOri_layer1->Fill(ClusEta->at(i));
                hM_ClusPhi_layer1->Fill(ClusPhi->at(i));
                hM_ClusADC_layer1->Fill(ClusAdc->at(i));
                // hM_ClusZSize_layer1->Fill(ClusZSize->at(i));
                hM_ClusPhiSize_layer1->Fill(ClusPhiSize->at(i));
                hM_ClusZ_ClusPhi_layer1->Fill(ClusZ->at(i), ClusPhi->at(i));
                // hM_ClusEta_ClusZSize_layer1->Fill(ClusEta->at(i), ClusZSize->at(i));
                hM_ClusPhi_ClusPhiSize_layer1->Fill(ClusPhi->at(i), ClusPhiSize->at(i));
                // hM_ClusZSize_ClusPhiSize_layer1->Fill(ClusZSize->at(i), ClusPhiSize->at(i));
                hM_ClusEta_ClusADC_layer1->Fill(ClusEta->at(i), ClusAdc->at(i));
                // hM_ClusZSize_ClusADC_layer1->Fill(ClusZSize->at(i), ClusAdc->at(i));
                hM_ClusPhiSize_ClusADC_layer1->Fill(ClusPhiSize->at(i), ClusAdc->at(i));
            }
            else if (ClusLayer->at(i) == 5 || ClusLayer->at(i) == 6)
            {
                // l2hits.push_back(hit);
                hM_ClusX_layer2->Fill(ClusX->at(i));
                hM_ClusY_layer2->Fill(ClusY->at(i));
                hM_ClusZ_layer2->Fill(ClusZ->at(i));
                hM_ClusR_layer2->Fill(ClusR->at(i));
                // hM_ClusEtaPV_layer2->Fill(ClusEta->at(i));
                hM_ClusEtaOri_layer2->Fill(ClusEta->at(i));
                hM_ClusPhi_layer2->Fill(ClusPhi->at(i));
                hM_ClusADC_layer2->Fill(ClusAdc->at(i));
                // hM_ClusZSize_layer2->Fill(ClusZSize->at(i));
                hM_ClusPhiSize_layer2->Fill(ClusPhiSize->at(i));
                hM_ClusZ_ClusPhi_layer2->Fill(ClusZ->at(i), ClusPhi->at(i));
                // hM_ClusEta_ClusZSize_layer2->Fill(ClusEta->at(i), ClusZSize->at(i));
                hM_ClusPhi_ClusPhiSize_layer2->Fill(ClusPhi->at(i), ClusPhiSize->at(i));
                // hM_ClusZSize_ClusPhiSize_layer2->Fill(ClusZSize->at(i), ClusPhiSize->at(i));
                hM_ClusEta_ClusADC_layer2->Fill(ClusEta->at(i), ClusAdc->at(i));
                // hM_ClusZSize_ClusADC_layer2->Fill(ClusZSize->at(i), ClusAdc->at(i));
                hM_ClusPhiSize_ClusADC_layer2->Fill(ClusPhiSize->at(i), ClusAdc->at(i));
            }
            else
            {
                cout << "[WARNING] ClusLayer = " << ClusLayer->at(i) << ", which is not INTT. Check!" << endl;
                continue;
            }
        }
    }

    TFile *fout = new TFile(outfilename, "RECREATE");
    fout->cd();
    hM_ClusX_all->Write();
    hM_ClusX_layer1->Write();
    hM_ClusX_layer2->Write();
    hM_ClusY_all->Write();
    hM_ClusY_layer1->Write();
    hM_ClusY_layer2->Write();
    hM_ClusZ_all->Write();
    hM_ClusZ_layer1->Write();
    hM_ClusZ_layer2->Write();
    hM_ClusR_all->Write();
    hM_ClusR_layer1->Write();
    hM_ClusR_layer2->Write();
    // hM_ClusEtaPV_all->Write();
    // hM_ClusEtaPV_layer1->Write();
    // hM_ClusEtaPV_layer2->Write();
    hM_ClusEtaOri_all->Write();
    hM_ClusEtaOri_layer1->Write();
    hM_ClusEtaOri_layer2->Write();
    hM_ClusPhi_all->Write();
    hM_ClusPhi_layer1->Write();
    hM_ClusPhi_layer2->Write();
    hM_ClusADC_all->Write();
    hM_ClusADC_layer1->Write();
    hM_ClusADC_layer2->Write();
    // hM_ClusZSize_all->Write();
    // hM_ClusZSize_layer1->Write();
    // hM_ClusZSize_layer2->Write();
    hM_ClusPhiSize_all->Write();
    hM_ClusPhiSize_layer1->Write();
    hM_ClusPhiSize_layer2->Write();
    // hM_ClusPairDr_layer1->Write();
    // hM_ClusPairDr_layer2- >Write();
    hM_ClusX_ClusY_all->Write();
    hM_ClusZ_ClusPhi_all->Write();
    hM_ClusZ_ClusPhi_layer1->Write();
    hM_ClusZ_ClusPhi_layer2->Write();
    // hM_ClusEta_ClusZSize_all->Write();
    // hM_ClusEta_ClusZSize_layer1->Write();
    // hM_ClusEta_ClusZSize_layer2->Write();
    hM_ClusPhi_ClusPhiSize_all->Write();
    hM_ClusPhi_ClusPhiSize_layer1->Write();
    hM_ClusPhi_ClusPhiSize_layer2->Write();
    hM_ClusEta_ClusADC_all->Write();
    hM_ClusEta_ClusADC_layer1->Write();
    hM_ClusEta_ClusADC_layer2->Write();
    // hM_ClusZSize_ClusPhiSize_all->Write();
    // hM_ClusZSize_ClusPhiSize_layer1->Write();
    // hM_ClusZSize_ClusPhiSize_layer2->Write();
    // hM_ClusZSize_ClusADC_all->Write();
    // hM_ClusZSize_ClusADC_layer1->Write();
    // hM_ClusZSize_ClusADC_layer2->Write();
    hM_ClusPhiSize_ClusADC_all->Write();
    hM_ClusPhiSize_ClusADC_layer1->Write();
    hM_ClusPhiSize_ClusADC_layer2->Write();
    fout->Close();

    system(Form("cd ./plot/; python plotRecoCluster.py -f %s -i %d", outfilename.Data(), rundata));
}
