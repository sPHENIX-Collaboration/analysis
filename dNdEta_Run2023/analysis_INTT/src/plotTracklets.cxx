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
float edges[NBins + 1] = {1.00000000e-04, 1.18571250e-04, 1.40591414e-04, 1.66700998e-04, 1.97659458e-04, 2.34367291e-04, 2.77892228e-04, 3.29500290e-04, 3.90692614e-04, 4.63249118e-04, 5.49280272e-04, 6.51288487e-04, 7.72240903e-04,
                          9.15655696e-04, 1.08570441e-03, 1.28733329e-03, 1.52640718e-03, 1.80988009e-03, 2.14599745e-03, 2.54453601e-03, 3.01708817e-03, 3.57739917e-03, 4.24176693e-03, 5.02951609e-03, 5.96356012e-03, 7.07106781e-03,
                          8.38425353e-03, 9.94131425e-03, 1.17875406e-02, 1.39766343e-02, 1.65722701e-02, 1.96499479e-02, 2.32991889e-02, 2.76261397e-02, 3.27566592e-02, 3.88399805e-02, 4.60530506e-02, 5.46056779e-02, 6.47466352e-02,
                          7.67708949e-02, 9.10282102e-02, 1.07933287e-01, 1.27977848e-01, 1.51744935e-01, 1.79925867e-01, 2.13340350e-01, 2.52960321e-01, 2.99938216e-01, 3.55640493e-01, 4.21687380e-01, 5.00000000e-01};

void makehist(TString infname, TString outfname)
{
    TFile *fout = new TFile(outfname, "RECREATE");

    TH1F *hM_clusphi = new TH1F("hM_clusphi", "hM_clusphi", 140, -3.5, 3.5);
    TH1F *hM_cluseta = new TH1F("hM_cluseta", "hM_cluseta", 160, -4, 4);
    TH1F *hM_clusphisize = new TH1F("hM_clusphisize", "hM_clusphisize", 80, 0, 80);
    TH2F *hM_clusphi_clusphisize = new TH2F("hM_clusphi_clusphisize", "hM_clusphi_clusphisize", 140, -3.5, 3.5, 80, 0, 80);
    TH2F *hM_cluseta_clusphisize = new TH2F("hM_cluseta_clusphisize", "hM_cluseta_clusphisize", 160, -4, 4, 80, 0, 80);

    TH1F *hM_dEta_proto = new TH1F("hM_dEta_proto", "hM_dEta_proto", 200, -5, 5);
    TH1F *hM_dPhi_proto = new TH1F("hM_dPhi_proto", "hM_dPhi_proto", 100, -0.5, 0.5);
    TH1F *hM_dPhi_proto_altrange = new TH1F("hM_dPhi_proto_altrange", "hM_dPhi_proto_altrange", 100, -0.05, 0.05);
    TH1F *hM_dR_proto = new TH1F("hM_dR_proto", "hM_dR_proto", 100, 0, 0.5);
    TH1F *hM_dR_proto_altrange = new TH1F("hM_dR_proto_altrange", "hM_dR_proto_altrange", 50, 0, 0.05);
    TH1F *hM_dR_proto_LogX = new TH1F("hM_dR_proto_LogX", "hM_dR_proto_LogX", NBins, edges);
    TH1F *hM_Eta_proto = new TH1F("hM_Eta_proto", "hM_Eta_proto", 160, -4, 4);
    TH1F *hM_Phi_proto = new TH1F("hM_Phi_proto", "hM_Phi_proto", 140, -3.5, 3.5);
    TH2F *hM_Eta_vtxZ_proto_incl = new TH2F("hM_Eta_vtxZ_proto_incl", "hM_Eta_vtxZ_proto_incl", 280, -3.5, 3.5, 300, -50, 10);

    TH1F *hM_dEta_reco = new TH1F("hM_dEta_reco", "hM_dEta_reco", 200, -3, 3);
    TH1F *hM_dEta_reco_altrange = new TH1F("hM_dEta_reco_altrange", "hM_dEta_reco_altrange", 100, -0.5, 0.5);
    TH1F *hM_dEta_reco_altrange_Nclusle4000 = new TH1F("hM_dEta_reco_altrange_Nclusle4000", "hM_dEta_reco_altrange_Nclusle4000", 100, -0.5, 0.5);
    TH1F *hM_dEta_reco_altrange_Nclusgt4000 = new TH1F("hM_dEta_reco_altrange_Nclusgt4000", "hM_dEta_reco_altrange_Nclusgt4000", 100, -0.5, 0.5);
    TH1F *hM_dPhi_reco = new TH1F("hM_dPhi_reco", "hM_dPhi_reco", 200, -0.5, 0.5);
    TH1F *hM_dPhi_reco_altrange = new TH1F("hM_dPhi_reco_altrange", "hM_dPhi_reco_altrange", 200, -0.05, 0.05);
    TH1F *hM_dPhi_reco_altrange_Nclusle4000 = new TH1F("hM_dPhi_reco_altrange_Nclusle4000", "hM_dPhi_reco_altrange_Nclusle4000", 200, -0.05, 0.05);
    TH1F *hM_dPhi_reco_altrange_Nclusgt4000 = new TH1F("hM_dPhi_reco_altrange_Nclusgt4000", "hM_dPhi_reco_altrange_Nclusgt4000", 200, -0.05, 0.05);
    TH1F *hM_dR_reco = new TH1F("hM_dR_reco", "hM_dR_reco", 100, 0, 0.5);
    TH1F *hM_dR_reco_Nclusle4000 = new TH1F("hM_dR_reco_Nclusle4000", "hM_dR_reco_Nclusle4000", 100, 0, 0.5);
    TH1F *hM_dR_reco_Nclusgt4000 = new TH1F("hM_dR_reco_Nclusgt4000", "hM_dR_reco_Nclusgt4000", 100, 0, 0.5);
    TH1F *hM_dR_reco_altrange = new TH1F("hM_dR_reco_altrange", "hM_dR_reco_altrange", 50, 0, 0.05);
    TH1F *hM_dR_reco_LogX = new TH1F("hM_dR_reco_LogX", "hM_dR_reco_LogX", NBins, edges);
    TH1F *hM_Eta_reco = new TH1F("hM_Eta_reco", "hM_Eta_reco", 160, -4, 4);
    TH1F *hM_Eta_reco_Nclusle4000 = new TH1F("hM_Eta_reco_Nclusle4000", "hM_Eta_reco_Nclusle4000", 160, -4, 4);
    TH1F *hM_Eta_reco_Nclusgt4000 = new TH1F("hM_Eta_reco_Nclusgt4000", "hM_Eta_reco_Nclusgt4000", 160, -4, 4);
    TH1F *hM_Phi_reco = new TH1F("hM_Phi_reco", "hM_Phi_reco", 140, -3.5, 3.5);
    TH1F *hM_Phi_reco_Nclusle4000 = new TH1F("hM_Phi_reco_Nclusle4000", "hM_Phi_reco_Nclusle4000", 140, -3.5, 3.5);
    TH1F *hM_Phi_reco_Nclusgt4000 = new TH1F("hM_Phi_reco_Nclusgt4000", "hM_Phi_reco_Nclusgt4000", 140, -3.5, 3.5);
    TH2F *hM_Eta_vtxZ_reco_incl = new TH2F("hM_Eta_vtxZ_reco_incl", "hM_Eta_vtxZ_reco_incl", 280, -3.5, 3.5, 300, -50, 10);
    TH2F *hM_Eta_vtxZ_reco_Nclusle4000 = new TH2F("hM_Eta_vtxZ_reco_Nclusle4000", "hM_Eta_vtxZ_reco_Nclusle4000", 280, -3.5, 3.5, 300, -50, 10);
    TH2F *hM_Eta_vtxZ_reco_Nclusgt4000 = new TH2F("hM_Eta_vtxZ_reco_Nclusgt4000", "hM_Eta_vtxZ_reco_Nclusgt4000", 280, -3.5, 3.5, 300, -50, 10);

    TH1F *hM_NClusLayer1 = new TH1F("hM_NClusLayer1", "hM_NClusLayer1", 100, 0, 5000);
    TH1F *hM_NPrototkl = new TH1F("hM_NPrototkl", "hM_NPrototkl", 100, 0, 10000);
    TH1F *hM_NRecotkl_Raw = new TH1F("hM_NRecotkl_Raw", "hM_NRecotkl_Raw", 100, 0, 5000);

    TH1F *hM_RecoPVz = new TH1F("hM_RecoPVz", "hM_RecoPVz", 300, -50, 10);
    TH1F *hM_RecoPVz_Nclusle4000 = new TH1F("hM_RecoPVz_Nclusle4000", "hM_RecoPVz_Nclusle4000", 300, -50, 10);
    TH1F *hM_RecoPVz_Nclusgt4000 = new TH1F("hM_RecoPVz_Nclusgt4000", "hM_RecoPVz_Nclusgt4000", 300, -50, 10);

    TFile *f = new TFile(infname, "READ");
    TTree *t = (TTree *)f->Get("minitree");
    t->BuildIndex("event"); // Reference: https://root-forum.cern.ch/t/sort-ttree-entries/13138
    TTreeIndex *index = (TTreeIndex *)t->GetTreeIndex();
    int event, NClusLayer1, NPrototkl, NRecotkl_Raw, NRecotkl_GenMatched, NGenHadron;
    float PV_z, TruthPV_z;
    vector<float> *clusphi = 0, *cluseta = 0, *clusphisize = 0;
    vector<float> *prototkl_eta = 0, *prototkl_phi = 0, *prototkl_deta = 0, *prototkl_dphi = 0, *prototkl_dR = 0;
    vector<float> *recotklraw_eta = 0, *recotklraw_phi = 0, *recotklraw_deta = 0, *recotklraw_dphi = 0, *recotklraw_dR = 0;
    t->SetBranchAddress("event", &event);
    t->SetBranchAddress("NClusLayer1", &NClusLayer1);
    t->SetBranchAddress("NPrototkl", &NPrototkl);
    t->SetBranchAddress("NRecotkl_Raw", &NRecotkl_Raw);
    t->SetBranchAddress("PV_z", &PV_z);
    t->SetBranchAddress("TruthPV_z", &TruthPV_z);
    t->SetBranchAddress("clusphi", &clusphi);
    t->SetBranchAddress("cluseta", &cluseta);
    t->SetBranchAddress("clusphisize", &clusphisize);
    t->SetBranchAddress("prototkl_eta", &prototkl_eta);
    t->SetBranchAddress("prototkl_phi", &prototkl_phi);
    t->SetBranchAddress("prototkl_deta", &prototkl_deta);
    t->SetBranchAddress("prototkl_dphi", &prototkl_dphi);
    t->SetBranchAddress("prototkl_dR", &prototkl_dR);
    t->SetBranchAddress("recotklraw_eta", &recotklraw_eta);
    t->SetBranchAddress("recotklraw_phi", &recotklraw_phi);
    t->SetBranchAddress("recotklraw_deta", &recotklraw_deta);
    t->SetBranchAddress("recotklraw_dphi", &recotklraw_dphi);
    t->SetBranchAddress("recotklraw_dR", &recotklraw_dR);

    for (int ev = 0; ev < t->GetEntriesFast(); ev++)
    {
        Long64_t local = t->LoadTree(index->GetIndex()[ev]);
        t->GetEntry(local);

        if (PV_z < -99.)
            continue;

        cout << "Event=" << event << "; NClusLayer1=" << NClusLayer1 << "; NPrototkl=" << NPrototkl << "; NRecotkl_Raw=" << NRecotkl_Raw << endl;

        hM_NClusLayer1->Fill(NClusLayer1);
        hM_NPrototkl->Fill(NPrototkl);
        hM_NRecotkl_Raw->Fill(NRecotkl_Raw);

        hM_RecoPVz->Fill(PV_z);
        if (NClusLayer1 <= 4000)
            hM_RecoPVz_Nclusle4000->Fill(PV_z);
        else
            hM_RecoPVz_Nclusgt4000->Fill(PV_z);

        for (size_t j = 0; j < clusphisize->size(); j++)
        {
            hM_clusphi->Fill(clusphi->at(j));
            hM_cluseta->Fill(cluseta->at(j));
            hM_clusphisize->Fill(clusphisize->at(j));
            hM_clusphi_clusphisize->Fill(clusphi->at(j), clusphisize->at(j));
            hM_cluseta_clusphisize->Fill(cluseta->at(j), clusphisize->at(j));
        }

        for (size_t j = 0; j < prototkl_eta->size(); j++)
        {
            hM_dEta_proto->Fill(prototkl_deta->at(j));
            hM_dPhi_proto->Fill(prototkl_dphi->at(j));
            hM_dPhi_proto_altrange->Fill(prototkl_dphi->at(j));
            hM_dR_proto->Fill(prototkl_dR->at(j));
            hM_dR_proto_altrange->Fill(prototkl_dR->at(j));
            hM_dR_proto_LogX->Fill(prototkl_dR->at(j));
            hM_Eta_proto->Fill(prototkl_eta->at(j));
            hM_Phi_proto->Fill(prototkl_phi->at(j));

            hM_Eta_vtxZ_proto_incl->Fill(prototkl_eta->at(j), PV_z);
        }

        for (size_t j = 0; j < recotklraw_eta->size(); j++)
        {
            hM_dEta_reco->Fill(recotklraw_deta->at(j));
            hM_dEta_reco_altrange->Fill(recotklraw_deta->at(j));
            hM_dPhi_reco->Fill(recotklraw_dphi->at(j));
            hM_dPhi_reco_altrange->Fill(recotklraw_dphi->at(j));
            hM_dR_reco->Fill(recotklraw_dR->at(j));
            hM_dR_reco_altrange->Fill(recotklraw_dR->at(j));
            hM_dR_reco_LogX->Fill(recotklraw_dR->at(j));
            hM_Eta_reco->Fill(recotklraw_eta->at(j));
            hM_Phi_reco->Fill(recotklraw_phi->at(j));
            hM_Eta_vtxZ_reco_incl->Fill(recotklraw_eta->at(j), PV_z);

            if (NClusLayer1 <= 4000)
            {
                hM_dEta_reco_altrange_Nclusle4000->Fill(recotklraw_deta->at(j));
                hM_dPhi_reco_altrange_Nclusle4000->Fill(recotklraw_dphi->at(j));
                hM_dR_reco_Nclusle4000->Fill(recotklraw_dR->at(j));
                hM_Eta_reco_Nclusle4000->Fill(recotklraw_eta->at(j));
                hM_Phi_reco_Nclusle4000->Fill(recotklraw_phi->at(j));
                hM_Eta_vtxZ_reco_Nclusle4000->Fill(recotklraw_eta->at(j), PV_z);
            }
            else
            {
                hM_dEta_reco_altrange_Nclusgt4000->Fill(recotklraw_deta->at(j));
                hM_dPhi_reco_altrange_Nclusgt4000->Fill(recotklraw_dphi->at(j));
                hM_dR_reco_Nclusgt4000->Fill(recotklraw_dR->at(j));
                hM_Eta_reco_Nclusgt4000->Fill(recotklraw_eta->at(j));
                hM_Phi_reco_Nclusgt4000->Fill(recotklraw_phi->at(j));
                hM_Eta_vtxZ_reco_Nclusgt4000->Fill(recotklraw_eta->at(j), PV_z);
            }
        }
    }

    f->Close();

    fout->cd();
    hM_NClusLayer1->Write();
    hM_NPrototkl->Write();
    hM_NRecotkl_Raw->Write();
    hM_RecoPVz->Write();
    hM_RecoPVz_Nclusle4000->Write();
    hM_RecoPVz_Nclusgt4000->Write();
    hM_clusphi->Write();
    hM_cluseta->Write();
    hM_clusphisize->Write();
    hM_clusphi_clusphisize->Write();
    hM_cluseta_clusphisize->Write();
    hM_dEta_proto->Write();
    hM_dEta_reco->Write();
    hM_dEta_proto->Write();
    hM_dPhi_proto->Write();
    hM_dPhi_proto_altrange->Write();
    hM_dR_proto->Write();
    hM_dR_proto_altrange->Write();
    hM_dR_proto_LogX->Write();
    hM_Eta_proto->Write();
    hM_Phi_proto->Write();
    hM_dEta_reco->Write();
    hM_dEta_reco_altrange->Write();
    hM_dEta_reco_altrange_Nclusle4000->Write();
    hM_dEta_reco_altrange_Nclusgt4000->Write();
    hM_dPhi_reco->Write();
    hM_dPhi_reco_altrange->Write();
    hM_dPhi_reco_altrange_Nclusle4000->Write();
    hM_dPhi_reco_altrange_Nclusgt4000->Write();
    hM_dR_reco->Write();
    hM_dR_reco_Nclusle4000->Write();
    hM_dR_reco_Nclusgt4000->Write();
    hM_dR_reco_altrange->Write();
    hM_dR_reco_LogX->Write();
    hM_Eta_reco->Write();
    hM_Eta_reco_Nclusle4000->Write();
    hM_Eta_reco_Nclusgt4000->Write();
    hM_Phi_reco->Write();
    hM_Phi_reco_Nclusle4000->Write();
    hM_Phi_reco_Nclusgt4000->Write();
    hM_Eta_vtxZ_proto_incl->Write();
    hM_Eta_vtxZ_reco_incl->Write();
    hM_Eta_vtxZ_reco_Nclusle4000->Write();
    hM_Eta_vtxZ_reco_Nclusgt4000->Write();

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
