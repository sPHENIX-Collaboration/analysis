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

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: ./plotTrkrHit [infilename (ntuple)] [outfilename (histogram)]" << std::endl;
        return 0;
    }

    for (int i = 0; i < argc; i++)
    {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    }

    TString infilename = TString(argv[1]);
    TString outfilename = TString(argv[2]);

    int z_offset[4] = {5, 0, 13, 21};
    int layer_offset[4] = {0, 12, 24, 40};
    int layerhitset_offset[4] = {0, 48, 96, 160};

    TH2F *hM_hitmap_RowColumnFlatten_unwei = new TH2F("hM_hitmap_RowColumnFlatten_unwei", "hM_hitmap_RowColumnFlatten_unwei", 4 * 32, 0, 4 * 32, 360 * 16, 0, 360 * 16);
    TH2F *hM_hitmap_RowColumnFlatten_weiADC = new TH2F("hM_hitmap_RowColumnFlatten_weiADC", "hM_hitmap_RowColumnFlatten_weiADC", 4 * 32, 0, 4 * 32, 360 * 16, 0, 360 * 16);

    TFile *f = new TFile(infilename, "READ");
    TTree *t = (TTree *)f->Get("EventTree");
    int NTrkrhits;
    std::vector<uint16_t> *TrkrHitRow = 0, *TrkrHitColumn = 0, *TrkrHitADC = 0;
    std::vector<uint8_t> *TrkrHitLadderZId = 0, *TrkrHitLadderPhiId = 0, *TrkrHitLayer = 0;
    t->SetBranchAddress("NTrkrhits", &NTrkrhits);
    t->SetBranchAddress("TrkrHitRow", &TrkrHitRow);
    t->SetBranchAddress("TrkrHitColumn", &TrkrHitColumn);
    t->SetBranchAddress("TrkrHitADC", &TrkrHitADC);
    t->SetBranchAddress("TrkrHitLadderZId", &TrkrHitLadderZId);
    t->SetBranchAddress("TrkrHitLadderPhiId", &TrkrHitLadderPhiId);
    t->SetBranchAddress("TrkrHitLayer", &TrkrHitLayer);

    for (int ev = 0; ev < t->GetEntriesFast(); ev++)
    {
        t->GetEntry(ev);

        for (int i = 0; i < NTrkrhits; i++)
        {
            int flat_x = TrkrHitColumn->at(i) + z_offset[TrkrHitLadderZId->at(i)] + 32 * (TrkrHitLayer->at(i) - 3);
            int flat_y = TrkrHitRow->at(i) + 360 * TrkrHitLadderPhiId->at(i);

            hM_hitmap_RowColumnFlatten_unwei->Fill(flat_x, flat_y);
            hM_hitmap_RowColumnFlatten_weiADC->Fill(flat_x, flat_y, TrkrHitADC->at(i) + 1);
        }
    }

    TFile *fout = new TFile(outfilename, "RECREATE");
    fout->cd();
    hM_hitmap_RowColumnFlatten_unwei->Write();
    hM_hitmap_RowColumnFlatten_weiADC->Write();
    fout->Close();
}