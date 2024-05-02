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
    if (argc != 5)
    {
        std::cout << "Usage: ./plotRecoCluster [isdata] [EvtVtx_map_filename] [infilename (ntuple)] [outfilename (histogram)]" << std::endl;
        return 0;
    }

    for (int i = 0; i < argc; i++)
    {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    }

    bool IsData = (TString(argv[1]).Atoi() == 1) ? true : false;
    TString EvtVtx_map_filename = TString(argv[2]);
    TString infilename = TString(argv[3]);
    TString outfilename = TString(argv[4]);

    TString idxstr = (IsData) ? "INTT_BCO" : "event";

    // std::map<int, vector<float>> EvtVtx_map = EvtVtx_map_tklcluster(EvtVtx_map_filename.Data());
    std::map<int, vector<float>> EvtVtxMap_event = EvtVtx_map_event(EvtVtx_map_filename.Data()); // use with simulation
    std::map<uint64_t, vector<float>> EvtVtxMap_inttbco = EvtVtx_map_inttbco(EvtVtx_map_filename.Data()); // use with data

    // TH1F *hM_ClusX_all = new TH1F("hM_ClusX_all", "hM_ClusX_all", 200, -10, 10);
    // TH1F *hM_ClusX_layer1 = new TH1F("hM_ClusX_layer1", "hM_ClusX_layer1", 200, -10, 10);
    // TH1F *hM_ClusX_layer2 = new TH1F("hM_ClusX_layer2", "hM_ClusX_layer2", 200, -10, 10);
    // TH1F *hM_ClusY_all = new TH1F("hM_ClusY_all", "hM_ClusY_all", 200, -10, 10);
    // TH1F *hM_ClusY_layer1 = new TH1F("hM_ClusY_layer1", "hM_ClusY_layer1", 200, -10, 10);
    // TH1F *hM_ClusY_layer2 = new TH1F("hM_ClusY_layer2", "hM_ClusY_layer2", 200, -10, 10);
    TH1F *hM_ClusZ_all = new TH1F("hM_ClusZ_all", "hM_ClusZ_all", 200, -25, 25);
    TH1F *hM_ClusZ_layer1 = new TH1F("hM_ClusZ_layer1", "hM_ClusZ_layer1", 200, -25, 25);
    TH1F *hM_ClusZ_layer2 = new TH1F("hM_ClusZ_layer2", "hM_ClusZ_layer2", 200, -25, 25);
    // TH1F *hM_ClusR_all = new TH1F("hM_ClusR_all", "hM_ClusR_all", 120, 4, 7);
    // TH1F *hM_ClusR_layer1 = new TH1F("hM_ClusR_layer1", "hM_ClusR_layer1", 120, 4, 7);
    // TH1F *hM_ClusR_layer2 = new TH1F("hM_ClusR_layer2", "hM_ClusR_layer2", 120, 4, 7);
    // TH1F *hM_ClusEtaOri_all = new TH1F("hM_ClusEtaOri_all", "hM_ClusEtaOri_all", 160, -4, 4);
    // TH1F *hM_ClusEtaOri_layer1 = new TH1F("hM_ClusEtaOri_layer1", "hM_ClusEtaOri_layer1", 160, -4, 4);
    // TH1F *hM_ClusEtaOri_layer2 = new TH1F("hM_ClusEtaOri_layer2", "hM_ClusEtaOri_layer2", 160, -4, 4);
    TH1F *hM_ClusEtaPV_all = new TH1F("hM_ClusEtaPV_all", "hM_ClusEtaPV_all", 160, -4, 4);
    TH1F *hM_ClusEtaPV_layer1 = new TH1F("hM_ClusEtaPV_layer1", "hM_ClusEtaPV_layer1", 160, -4, 4);
    TH1F *hM_ClusEtaPV_layer2 = new TH1F("hM_ClusEtaPV_layer2", "hM_ClusEtaPV_layer2", 160, -4, 4);
    // TH1F *hM_ClusPhiOri_all = new TH1F("hM_ClusPhiOri_all", "hM_ClusPhiOri_all", 140, -3.5, 3.5);
    // TH1F *hM_ClusPhiOri_layer1 = new TH1F("hM_ClusPhiOri_layer1", "hM_ClusPhiOri_layer1", 140, -3.5, 3.5);
    // TH1F *hM_ClusPhiOri_layer2 = new TH1F("hM_ClusPhiOri_layer2", "hM_ClusPhiOri_layer2", 140, -3.5, 3.5);
    TH1F *hM_ClusPhiPV_all = new TH1F("hM_ClusPhiPV_all", "hM_ClusPhiPV_all", 140, -3.5, 3.5);
    TH1F *hM_ClusPhiPV_layer1 = new TH1F("hM_ClusPhiPV_layer1", "hM_ClusPhiPV_layer1", 140, -3.5, 3.5);
    TH1F *hM_ClusPhiPV_layer2 = new TH1F("hM_ClusPhiPV_layer2", "hM_ClusPhiPV_layer2", 140, -3.5, 3.5);
    TH1F *hM_ClusPhiPV_ClusPhiSize43or46 = new TH1F("hM_ClusPhiPV_ClusPhiSize43or46", "hM_ClusPhiPV_ClusPhiSize43or46", 330, -3.3, 3.3);
    // Very fine binning for ladder overlap study
    // INTT pitch size in phi = 78 um (Reference: https://indico.bnl.gov/event/15547/contributions/62868/attachments/41171/68968/2022_sPHENIX_School_RN.pdf)
    // INTT outer radius 102.62 mm
    // Azimuthal angle per pitch = (78*1E-6) / (102.62*1E-3) ~= 7.601E-4 rad ~= 0.0435 degree 
    // Number of bins = (3.2*2) / (7.601E-4) = 8421
    TH1F *hM_ClusPhiPV_all_fine = new TH1F("hM_ClusPhiPV_all_fine", "hM_ClusPhiPV_all_fine", 8421, -3.2, 3.2);
    TH1F *hM_ClusPhiPV_layer1_fine = new TH1F("hM_ClusPhiPV_layer1_fine", "hM_ClusPhiPV_layer1_fine", 8421, -3.2, 3.2);
    TH1F *hM_ClusPhiPV_layer2_fine = new TH1F("hM_ClusPhiPV_layer2_fine", "hM_ClusPhiPV_layer2_fine", 8421, -3.2, 3.2);
    //
    TH1F *hM_ClusADC_all = new TH1F("hM_ClusADC_all", "hM_ClusADC_all", 1800, 0, 18000);
    TH1F *hM_ClusADC_layer1 = new TH1F("hM_ClusADC_layer1", "hM_ClusADC_layer1", 1800, 0, 18000);
    TH1F *hM_ClusADC_layer2 = new TH1F("hM_ClusADC_layer2", "hM_ClusADC_layer2", 1800, 0, 18000);
    // TH1F *hM_ClusZSize_all = new TH1F("hM_ClusZSize_all", "hM_ClusZSize_all", 20, 0, 20);
    // TH1F *hM_ClusZSize_layer1 = new TH1F("hM_ClusZSize_layer1", "hM_ClusZSize_layer1", 20, 0, 20);
    // TH1F *hM_ClusZSize_layer2 = new TH1F("hM_ClusZSize_layer2", "hM_ClusZSize_layer2", 20, 0, 20);
    TH1F *hM_ClusPhiSize_all = new TH1F("hM_ClusPhiSize_all", "hM_ClusPhiSize_all", 100, 0, 100);
    TH1F *hM_ClusPhiSize_layer1 = new TH1F("hM_ClusPhiSize_layer1", "hM_ClusPhiSize_layer1", 100, 0, 100);
    TH1F *hM_ClusPhiSize_layer2 = new TH1F("hM_ClusPhiSize_layer2", "hM_ClusPhiSize_layer2", 100, 0, 100);
    TH2F *hM_ClusX_ClusY_all = new TH2F("hM_ClusX_ClusY_all", "hM_ClusX_ClusY_all", 260, -13, 13, 260, -13, 13);
    TH2F *hM_ClusX_ClusY_all_weiphisize = new TH2F("hM_ClusX_ClusY_all_weiphisize", "hM_ClusX_ClusY_all_weiphisize", 260, -13, 13, 260, -13, 13);
    TH2F *hM_ClusX_ClusY_all_weiclusadc = new TH2F("hM_ClusX_ClusY_all_weiclusadc", "hM_ClusX_ClusY_all_weiclusadc", 260, -13, 13, 260, -13, 13);
    TH2F *hM_ClusX_ClusY_ClusPhiSize43or46 = new TH2F("hM_ClusX_ClusY_ClusPhiSize43or46", "hM_ClusX_ClusY_ClusPhiSize43or46", 260, -13, 13, 260, -13, 13);
    TH2F *hM_ClusX_ClusY_ClusPhiSize43 = new TH2F("hM_ClusX_ClusY_ClusPhiSize43", "hM_ClusX_ClusY_ClusPhiSize43", 260, -13, 13, 260, -13, 13);
    TH2F *hM_ClusX_ClusY_ClusPhiSize46 = new TH2F("hM_ClusX_ClusY_ClusPhiSize46", "hM_ClusX_ClusY_ClusPhiSize46", 260, -13, 13, 260, -13, 13);

    TH2F *hM_ClusZ_ClusPhiPV_all = new TH2F("hM_ClusZ_ClusPhiPV_all", "hM_ClusZ_ClusPhiPV_all", 1000, -25, 25, 350, -3.5, 3.5);
    TH2F *hM_ClusZ_ClusPhiPV_layer1 = new TH2F("hM_ClusZ_ClusPhiPV_layer1", "hM_ClusZ_ClusPhiPV_layer1", 1000, -25, 25, 350, -3.5, 3.5);
    TH2F *hM_ClusZ_ClusPhiPV_layer2 = new TH2F("hM_ClusZ_ClusPhiPV_layer2", "hM_ClusZ_ClusPhiPV_layer2", 1000, -25, 25, 350, -3.5, 3.5);
    TH2F *hM_ClusZ_ClusPhiPV_all_coarse = new TH2F("hM_ClusZ_ClusPhiPV_all_coarse", "hM_ClusZ_ClusPhiPV_all_coarse", 100, -25, 25, 350, -3.5, 3.5);
    TH2F *hM_ClusZ_ClusPhiPV_all_coarse_weiphisize = new TH2F("hM_ClusZ_ClusPhiPV_all_coarse_weiphisize", "hM_ClusZ_ClusPhiPV_all_coarse_weiphisize", 100, -25, 25, 350, -3.5, 3.5);
    TH2F *hM_ClusZ_ClusPhiPV_all_coarse_weiclusadc = new TH2F("hM_ClusZ_ClusPhiPV_all_coarse_weiclusadc", "hM_ClusZ_ClusPhiPV_all_coarse_weiclusadc", 100, -25, 25, 350, -3.5, 3.5);
    // TH2F *hM_ClusEta_ClusZSize_all = new TH2F("hM_ClusEta_ClusZSize_all", "hM_ClusEta_ClusZSize_all", 160, -4, 4, 20, 0, 20);
    // TH2F *hM_ClusEta_ClusZSize_layer1 = new TH2F("hM_ClusEta_ClusZSize_layer1", "hM_ClusEta_ClusZSize_layer1", 160, -4, 4, 20, 0, 20);
    // TH2F *hM_ClusEta_ClusZSize_layer2 = new TH2F("hM_ClusEta_ClusZSize_layer2", "hM_ClusEta_ClusZSize_layer2", 160, -4, 4, 20, 0, 20);
    TH2F *hM_ClusPhiPV_ClusPhiSize_all = new TH2F("hM_ClusPhiPV_ClusPhiSize_all", "hM_ClusPhiPV_ClusPhiSize_all", 140, -3.5, 3.5, 100, 0, 100);
    TH2F *hM_ClusPhiPV_ClusPhiSize_layer1 = new TH2F("hM_ClusPhiPV_ClusPhiSize_layer1", "hM_ClusPhiPV_ClusPhiSize_layer1", 140, -3.5, 3.5, 100, 0, 100);
    TH2F *hM_ClusPhiPV_ClusPhiSize_layer2 = new TH2F("hM_ClusPhiPV_ClusPhiSize_layer2", "hM_ClusPhiPV_ClusPhiSize_layer2", 140, -3.5, 3.5, 100, 0, 100);
    TH2F *hM_ClusPhiPV_ClusADC_all = new TH2F("hM_ClusPhiPV_ClusADC_all", "hM_ClusPhiPV_ClusADC_all", 140, -3.5, 3.5, 180, 0, 18000);
    TH2F *hM_ClusPhiPV_ClusADC_layer1 = new TH2F("hM_ClusPhiPV_ClusADC_layer1", "hM_ClusPhiPV_ClusADC_layer1", 140, -3.5, 3.5, 180, 0, 18000);
    TH2F *hM_ClusPhiPV_ClusADC_layer2 = new TH2F("hM_ClusPhiPV_ClusADC_layer2", "hM_ClusPhiPV_ClusADC_layer2", 140, -3.5, 3.5, 180, 0, 18000);
    // TH2F *hM_ClusZSize_ClusPhiSize_all = new TH2F("hM_ClusZSize_ClusPhiSize_all", "hM_ClusZSize_ClusPhiSize_all", 20, 0, 20, 100, 0, 100);
    // TH2F *hM_ClusZSize_ClusPhiSize_layer1 = new TH2F("hM_ClusZSize_ClusPhiSize_layer1", "hM_ClusZSize_ClusPhiSize_layer1", 20, 0, 20, 100, 0, 100);
    // TH2F *hM_ClusZSize_ClusPhiSize_layer2 = new TH2F("hM_ClusZSize_ClusPhiSize_layer2", "hM_ClusZSize_ClusPhiSize_layer2", 20, 0, 20, 100, 0, 100);
    TH2F *hM_ClusEtaPV_ClusADC_all = new TH2F("hM_ClusEtaPV_ClusADC_all", "hM_ClusEtaPV_ClusADC_all", 160, -4, 4, 180, 0, 18000);
    TH2F *hM_ClusEtaPV_ClusADC_layer1 = new TH2F("hM_ClusEtaPV_ClusADC_layer1", "hM_ClusEtaPV_ClusADC_layer1", 160, -4, 4, 180, 0, 18000);
    TH2F *hM_ClusEtaPV_ClusADC_layer2 = new TH2F("hM_ClusEtaPV_ClusADC_layer2", "hM_ClusEtaPV_ClusADC_layer2", 160, -4, 4, 180, 0, 18000);
    TH2F *hM_ClusEtaPV_ClusADC_all_zoomin = new TH2F("hM_ClusEtaPV_ClusADC_all_zoomin", "hM_ClusEtaPV_ClusADC_all_zoomin", 120, -3, 3, 50, 0, 500);
    TH2F *hM_ClusEtaPV_ClusADC_layer1_zoomin = new TH2F("hM_ClusEtaPV_ClusADC_layer1_zoomin", "hM_ClusEtaPV_ClusADC_layer1_zoomin", 120, -3, 3, 50, 0, 500);
    TH2F *hM_ClusEtaPV_ClusADC_layer2_zoomin = new TH2F("hM_ClusEtaPV_ClusADC_layer2_zoomin", "hM_ClusEtaPV_ClusADC_layer2_zoomin", 120, -3, 3, 50, 0, 500);
    TH2F *hM_ClusEtaPV_ClusPhiSize_all = new TH2F("hM_ClusEtaPV_ClusPhiSize_all", "hM_ClusEtaPV_ClusPhiSize_all", 160, -4, 4, 100, 0, 100);
    TH2F *hM_ClusEtaPV_ClusPhiSize_layer1 = new TH2F("hM_ClusEtaPV_ClusPhiSize_layer1", "hM_ClusEtaPV_ClusPhiSize_layer1", 160, -4, 4, 100, 0, 100);
    TH2F *hM_ClusEtaPV_ClusPhiSize_layer2 = new TH2F("hM_ClusEtaPV_ClusPhiSize_layer2", "hM_ClusEtaPV_ClusPhiSize_layer2", 160, -4, 4, 100, 0, 100);
    TH2F *hM_ClusEtaPV_ClusPhiSize_all_zoomin = new TH2F("hM_ClusEtaPV_ClusPhiSize_all_zoomin", "hM_ClusEtaPV_ClusPhiSize_all_zoomin", 120, -3, 3, 20, 0, 20);
    TH2F *hM_ClusEtaPV_ClusPhiSize_layer1_zoomin = new TH2F("hM_ClusEtaPV_ClusPhiSize_layer1_zoomin", "hM_ClusEtaPV_ClusPhiSize_layer1_zoomin", 120, -3, 3, 20, 0, 20);
    TH2F *hM_ClusEtaPV_ClusPhiSize_layer2_zoomin = new TH2F("hM_ClusEtaPV_ClusPhiSize_layer2_zoomin", "hM_ClusEtaPV_ClusPhiSize_layer2_zoomin", 120, -3, 3, 20, 0, 20);
    TH2F *hM_ClusPhiSize_ClusADC_all = new TH2F("hM_ClusPhiSize_ClusADC_all", "hM_ClusPhiSize_ClusADC_all", 100, 0, 100, 180, 0, 18000);
    TH2F *hM_ClusPhiSize_ClusADC_layer1 = new TH2F("hM_ClusPhiSize_ClusADC_layer1", "hM_ClusPhiSize_ClusADC_layer1", 100, 0, 100, 180, 0, 18000);
    TH2F *hM_ClusPhiSize_ClusADC_layer2 = new TH2F("hM_ClusPhiSize_ClusADC_layer2", "hM_ClusPhiSize_ClusADC_layer2", 100, 0, 100, 180, 0, 18000);
    TH2F *hM_ClusPhiSize_ClusADC_all_zoomin = new TH2F("hM_ClusPhiSize_ClusADC_all_zoomin", "hM_ClusPhiSize_ClusADC_all_zoomin", 20, 0, 20, 50, 0, 500);
    TH2F *hM_ClusPhiSize_ClusADC_layer1_zoomin = new TH2F("hM_ClusPhiSize_ClusADC_layer1_zoomin", "hM_ClusPhiSize_ClusADC_layer1_zoomin", 20, 0, 20, 50, 0, 500);
    TH2F *hM_ClusPhiSize_ClusADC_layer2_zoomin = new TH2F("hM_ClusPhiSize_ClusADC_layer2_zoomin", "hM_ClusPhiSize_ClusADC_layer2_zoomin", 20, 0, 20, 50, 0, 500);

    TFile *f = new TFile(infilename, "READ");
    TTree *t = (TTree *)f->Get("EventTree");
    t->BuildIndex(idxstr); // Reference: https://root-forum.cern.ch/t/sort-ttree-entries/13138
    TTreeIndex *index = (TTreeIndex *)t->GetTreeIndex();
    int event;
    uint64_t INTT_BCO;
    vector<int> *ClusLayer = 0;
    vector<float> *ClusX = 0, *ClusY = 0, *ClusZ = 0, *ClusR = 0, *ClusPhi = 0, *ClusEta = 0, *ClusPhiSize = 0, *ClusZSize = 0;
    vector<unsigned int> *ClusAdc = 0;
    t->SetBranchAddress("event", &event);
    t->SetBranchAddress("INTT_BCO", &INTT_BCO);
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
    for (int ev = 0; ev < t->GetEntriesFast(); ev++)
    {
        Long64_t local = t->LoadTree(index->GetIndex()[ev]);
        t->GetEntry(local);

        vector<float> PV = (IsData) ? EvtVtxMap_inttbco[INTT_BCO] : EvtVtxMap_event[event];

        if (PV.size() != 3)
        {
            cout << "[ERROR] No PV found for event " << event << endl;
            exit(1);
        }

        for (size_t i = 0; i < ClusLayer->size(); i++)
        {
            if (ClusLayer->at(i) < 3 || ClusLayer->at(i) > 6)
            {
                cout << "[ERROR] Unknown layer: " << ClusLayer->at(i) << endl; // Should not happen
                exit(1);
            }

            int layer = (ClusLayer->at(i) == 3 || ClusLayer->at(i) == 4) ? 0 : 1;

            Hit *hit = new Hit(ClusX->at(i), ClusY->at(i), ClusZ->at(i), PV[0], PV[1], PV[2], layer);

            // hM_ClusX_all->Fill(ClusX->at(i));
            // hM_ClusY_all->Fill(ClusY->at(i));
            hM_ClusZ_all->Fill(ClusZ->at(i));
            // hM_ClusR_all->Fill(ClusR->at(i));
            // hM_ClusEtaOri_all->Fill(ClusEta->at(i));
            hM_ClusEtaPV_all->Fill(hit->Eta());
            // hM_ClusPhiOri_all->Fill(ClusPhi->at(i));
            hM_ClusPhiPV_all->Fill(hit->Phi());
            hM_ClusPhiPV_all_fine->Fill(hit->Phi());
            hM_ClusADC_all->Fill(ClusAdc->at(i));
            hM_ClusPhiSize_all->Fill(ClusPhiSize->at(i));
            hM_ClusX_ClusY_all->Fill(ClusX->at(i), ClusY->at(i));
            if (ClusPhiSize->at(i) == 43 || ClusPhiSize->at(i) == 46)
            {
                hM_ClusPhiPV_ClusPhiSize43or46->Fill(hit->Phi());
                hM_ClusX_ClusY_ClusPhiSize43or46->Fill(ClusX->at(i), ClusY->at(i));

                if (ClusPhiSize->at(i) == 43)
                {
                    hM_ClusX_ClusY_ClusPhiSize43->Fill(ClusX->at(i), ClusY->at(i));
                }
                
                if (ClusPhiSize->at(i) == 46)
                {
                    hM_ClusX_ClusY_ClusPhiSize46->Fill(ClusX->at(i), ClusY->at(i));
                }
            }

            hM_ClusX_ClusY_all_weiphisize->Fill(ClusX->at(i), ClusY->at(i), ClusPhiSize->at(i));
            hM_ClusX_ClusY_all_weiclusadc->Fill(ClusX->at(i), ClusY->at(i), ClusAdc->at(i));
            hM_ClusZ_ClusPhiPV_all_coarse->Fill(ClusZ->at(i), hit->Phi());
            hM_ClusZ_ClusPhiPV_all_coarse_weiphisize->Fill(ClusZ->at(i), hit->Phi(), ClusPhiSize->at(i));
            hM_ClusZ_ClusPhiPV_all_coarse_weiclusadc->Fill(ClusZ->at(i), hit->Phi(), ClusAdc->at(i));
            hM_ClusZ_ClusPhiPV_all->Fill(ClusZ->at(i), hit->Phi());
            hM_ClusPhiPV_ClusPhiSize_all->Fill(hit->Phi(), ClusPhiSize->at(i));
            hM_ClusEtaPV_ClusADC_all->Fill(hit->Eta(), ClusAdc->at(i));
            hM_ClusEtaPV_ClusADC_all_zoomin->Fill(hit->Eta(), ClusAdc->at(i));
            hM_ClusEtaPV_ClusPhiSize_all->Fill(hit->Eta(), ClusPhiSize->at(i));
            hM_ClusEtaPV_ClusPhiSize_all_zoomin->Fill(hit->Eta(), ClusPhiSize->at(i));
            hM_ClusPhiPV_ClusADC_all->Fill(hit->Phi(), ClusAdc->at(i));
            hM_ClusPhiSize_ClusADC_all->Fill(ClusPhiSize->at(i), ClusAdc->at(i));
            hM_ClusPhiSize_ClusADC_all_zoomin->Fill(ClusPhiSize->at(i), ClusAdc->at(i));
            // hM_ClusZSize_all->Fill(ClusZSize->at(i));
            // hM_ClusZSize_ClusPhiSize_all->Fill(ClusZSize->at(i), ClusPhiSize->at(i));
            // hM_ClusZSize_ClusADC_all->Fill(ClusZSize->at(i), ClusAdc->at(i));
            if (layer == 0)
            {

                // hM_ClusX_layer1->Fill(ClusX->at(i));
                // hM_ClusY_layer1->Fill(ClusY->at(i));
                hM_ClusZ_layer1->Fill(ClusZ->at(i));
                // hM_ClusR_layer1->Fill(ClusR->at(i));
                // hM_ClusEtaOri_layer1->Fill(ClusEta->at(i));
                hM_ClusEtaPV_layer1->Fill(hit->Eta());
                // hM_ClusPhiOri_layer1->Fill(ClusPhi->at(i));
                hM_ClusPhiPV_layer1->Fill(hit->Phi());
                hM_ClusPhiPV_layer1_fine->Fill(hit->Phi());
                hM_ClusADC_layer1->Fill(ClusAdc->at(i));
                hM_ClusPhiSize_layer1->Fill(ClusPhiSize->at(i));
                hM_ClusZ_ClusPhiPV_layer1->Fill(ClusZ->at(i), hit->Phi());
                hM_ClusPhiPV_ClusPhiSize_layer1->Fill(hit->Phi(), ClusPhiSize->at(i));
                hM_ClusEtaPV_ClusADC_layer1->Fill(hit->Eta(), ClusAdc->at(i));
                hM_ClusEtaPV_ClusADC_layer1_zoomin->Fill(hit->Eta(), ClusAdc->at(i));
                hM_ClusEtaPV_ClusPhiSize_layer1->Fill(hit->Eta(), ClusPhiSize->at(i));
                hM_ClusEtaPV_ClusPhiSize_layer1_zoomin->Fill(hit->Eta(), ClusPhiSize->at(i));
                hM_ClusPhiPV_ClusADC_layer1->Fill(hit->Phi(), ClusAdc->at(i));
                hM_ClusPhiSize_ClusADC_layer1->Fill(ClusPhiSize->at(i), ClusAdc->at(i));
                hM_ClusPhiSize_ClusADC_layer1_zoomin->Fill(ClusPhiSize->at(i), ClusAdc->at(i));

                // hM_ClusZSize_layer1->Fill(ClusZSize->at(i));
                // hM_ClusEta_ClusZSize_layer1->Fill(hit->Eta(), ClusZSize->at(i));
                // hM_ClusZSize_ClusPhiSize_layer1->Fill(ClusZSize->at(i), ClusPhiSize->at(i));
                // hM_ClusZSize_ClusADC_layer1->Fill(ClusZSize->at(i), ClusAdc->at(i));
            }
            else
            {
                // hM_ClusX_layer2->Fill(ClusX->at(i));
                // hM_ClusY_layer2->Fill(ClusY->at(i));
                hM_ClusZ_layer2->Fill(ClusZ->at(i));
                // hM_ClusR_layer2->Fill(ClusR->at(i));
                // hM_ClusEtaOri_layer2->Fill(ClusEta->at(i));
                hM_ClusEtaPV_layer2->Fill(hit->Eta());
                // hM_ClusPhiOri_layer2->Fill(ClusPhi->at(i));
                hM_ClusPhiPV_layer2->Fill(hit->Phi());
                hM_ClusPhiPV_layer2_fine->Fill(hit->Phi());
                hM_ClusADC_layer2->Fill(ClusAdc->at(i));
                hM_ClusPhiSize_layer2->Fill(ClusPhiSize->at(i));
                hM_ClusZ_ClusPhiPV_layer2->Fill(ClusZ->at(i), hit->Phi());
                hM_ClusPhiPV_ClusPhiSize_layer2->Fill(hit->Phi(), ClusPhiSize->at(i));
                hM_ClusEtaPV_ClusADC_layer2->Fill(hit->Eta(), ClusAdc->at(i));
                hM_ClusEtaPV_ClusADC_layer2_zoomin->Fill(hit->Eta(), ClusAdc->at(i));
                hM_ClusEtaPV_ClusPhiSize_layer2->Fill(hit->Eta(), ClusPhiSize->at(i));
                hM_ClusEtaPV_ClusPhiSize_layer2_zoomin->Fill(hit->Eta(), ClusPhiSize->at(i));
                hM_ClusPhiPV_ClusADC_layer2->Fill(hit->Phi(), ClusAdc->at(i));
                hM_ClusPhiSize_ClusADC_layer2->Fill(ClusPhiSize->at(i), ClusAdc->at(i));
                hM_ClusPhiSize_ClusADC_layer2_zoomin->Fill(ClusPhiSize->at(i), ClusAdc->at(i));

                // hM_ClusZSize_layer2->Fill(ClusZSize->at(i));
                // hM_ClusEta_ClusZSize_layer2->Fill(ClusEta->at(i), ClusZSize->at(i));
                // hM_ClusZSize_ClusPhiSize_layer2->Fill(ClusZSize->at(i), ClusPhiSize->at(i));
                // hM_ClusZSize_ClusADC_layer2->Fill(ClusZSize->at(i), ClusAdc->at(i));
            }
        }
    }

    f->Close();

    TFile *fout = new TFile(outfilename, "RECREATE");
    fout->cd();
    // hM_ClusX_all->Write();
    // hM_ClusX_layer1->Write();
    // hM_ClusX_layer2->Write();
    // hM_ClusY_all->Write();
    // hM_ClusY_layer1->Write();
    // hM_ClusY_layer2->Write();
    hM_ClusZ_all->Write();
    hM_ClusZ_layer1->Write();
    hM_ClusZ_layer2->Write();
    // hM_ClusR_all->Write();
    // hM_ClusR_layer1->Write();
    // hM_ClusR_layer2->Write();
    hM_ClusEtaPV_all->Write();
    hM_ClusEtaPV_layer1->Write();
    hM_ClusEtaPV_layer2->Write();
    // hM_ClusEtaOri_all->Write();
    // hM_ClusEtaOri_layer1->Write();
    // hM_ClusEtaOri_layer2->Write();
    // hM_ClusPhiOri_all->Write();
    // hM_ClusPhiOri_layer1->Write();
    // hM_ClusPhiOri_layer2->Write();
    hM_ClusPhiPV_all->Write();
    hM_ClusPhiPV_layer1->Write();
    hM_ClusPhiPV_layer2->Write();
    hM_ClusPhiPV_ClusPhiSize43or46->Write();
    hM_ClusPhiPV_all_fine->Write();
    hM_ClusPhiPV_layer1_fine->Write();
    hM_ClusPhiPV_layer2_fine->Write();
    hM_ClusADC_all->Write();
    hM_ClusADC_layer1->Write();
    hM_ClusADC_layer2->Write();
    // hM_ClusZSize_all->Write();
    // hM_ClusZSize_layer1->Write();
    // hM_ClusZSize_layer2->Write();
    hM_ClusPhiSize_all->Write();
    hM_ClusPhiSize_layer1->Write();
    hM_ClusPhiSize_layer2->Write();
    hM_ClusX_ClusY_all->Write();
    hM_ClusX_ClusY_ClusPhiSize43or46->Write();
    hM_ClusX_ClusY_ClusPhiSize43->Write();
    hM_ClusX_ClusY_ClusPhiSize46->Write();
    hM_ClusX_ClusY_all_weiphisize->Write();
    hM_ClusX_ClusY_all_weiclusadc->Write();
    hM_ClusZ_ClusPhiPV_all_coarse->Write();
    hM_ClusZ_ClusPhiPV_all_coarse_weiphisize->Write();
    hM_ClusZ_ClusPhiPV_all_coarse_weiclusadc->Write();
    hM_ClusZ_ClusPhiPV_all->Write();
    hM_ClusZ_ClusPhiPV_layer1->Write();
    hM_ClusZ_ClusPhiPV_layer2->Write();
    // hM_ClusEta_ClusZSize_all->Write();
    // hM_ClusEta_ClusZSize_layer1->Write();
    // hM_ClusEta_ClusZSize_layer2->Write();
    hM_ClusPhiPV_ClusPhiSize_all->Write();
    hM_ClusPhiPV_ClusPhiSize_layer1->Write();
    hM_ClusPhiPV_ClusPhiSize_layer2->Write();
    hM_ClusEtaPV_ClusADC_all->Write();
    hM_ClusEtaPV_ClusADC_layer1->Write();
    hM_ClusEtaPV_ClusADC_layer2->Write();
    hM_ClusEtaPV_ClusADC_all_zoomin->Write();
    hM_ClusEtaPV_ClusADC_layer1_zoomin->Write();
    hM_ClusEtaPV_ClusADC_layer2_zoomin->Write();
    hM_ClusEtaPV_ClusPhiSize_all->Write();
    hM_ClusEtaPV_ClusPhiSize_layer1->Write();
    hM_ClusEtaPV_ClusPhiSize_layer2->Write();
    hM_ClusEtaPV_ClusPhiSize_all_zoomin->Write();
    hM_ClusEtaPV_ClusPhiSize_layer1_zoomin->Write();
    hM_ClusEtaPV_ClusPhiSize_layer2_zoomin->Write();
    hM_ClusPhiPV_ClusADC_all->Write();
    hM_ClusPhiPV_ClusADC_layer1->Write();
    hM_ClusPhiPV_ClusADC_layer2->Write();
    // hM_ClusZSize_ClusPhiSize_all->Write();
    // hM_ClusZSize_ClusPhiSize_layer1->Write();
    // hM_ClusZSize_ClusPhiSize_layer2->Write();
    // hM_ClusZSize_ClusADC_all->Write();
    // hM_ClusZSize_ClusADC_layer1->Write();
    // hM_ClusZSize_ClusADC_layer2->Write();
    hM_ClusPhiSize_ClusADC_all->Write();
    hM_ClusPhiSize_ClusADC_layer1->Write();
    hM_ClusPhiSize_ClusADC_layer2->Write();
    hM_ClusPhiSize_ClusADC_all_zoomin->Write();
    hM_ClusPhiSize_ClusADC_layer1_zoomin->Write();
    hM_ClusPhiSize_ClusADC_layer2_zoomin->Write();
    fout->Close();
}
