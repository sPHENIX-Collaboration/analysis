#include "CheckDeltaR.h"
#include <iostream>
using namespace std;

CheckDeltaR::CheckDeltaR(string run_type, string input_folder_directory, string input_file_name, string input_tree_name, string out_folder_directory)
:
run_type(run_type),
input_folder_directory(input_folder_directory),
input_file_name(input_file_name),
input_tree_name(input_tree_name),
out_folder_directory(out_folder_directory)
{   
    track_pair.clear();
    track_pair_deltaR.clear();
    true_track_used_tag.clear();
    reco_track_used_tag.clear();

    plot_text = (run_type == "MC") ? "Simulation" : "Work-in-progress";

    cout << "CheckDeltaR::CheckDeltaR()" << endl;
    file_in = TFile::Open(Form("%s/%s", input_folder_directory.c_str(), input_file_name.c_str()));
    tree_in = (TTree *)file_in->Get(input_tree_name.c_str());
    readeta = new ReadEta_v3(tree_in);

    cout<<"In CheckDeltaR::CheckDeltaR(), tree entry : "<<tree_in -> GetEntries()<<endl;

    system(Form("mkdir %s", out_folder_directory.c_str()));

    Init();
}

void CheckDeltaR::Init()
{
    c1 = new TCanvas("","",950,800);
    c1 -> cd();

    ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    DeltaPhi_All_1D = new TH1F("","DeltaPhi_All_1D",  100, -5, 5);
    DeltaPhi_All_1D -> GetXaxis() -> SetTitle("#Delta#phi [degree]");
    DeltaPhi_All_1D -> GetYaxis() -> SetTitle("Entry");
    DeltaPhi_All_1D -> GetXaxis() -> SetNdivisions(505);

    DeltaEta_All_1D = new TH1F("","DeltaEta_All_1D", 100, -5, 5);
    DeltaEta_All_1D -> GetXaxis() -> SetTitle("#Delta#eta");
    DeltaEta_All_1D -> GetYaxis() -> SetTitle("Entry");
    DeltaEta_All_1D -> GetXaxis() -> SetNdivisions(505);

    DeltaPhiDeltaEta_All_2D = new TH2F("","DeltaPhiDeltaEta_All_2D",  100, -5, 5, 100, -5, 5);
    DeltaPhiDeltaEta_All_2D -> GetXaxis() -> SetTitle("#Delta#phi [degree]");
    DeltaPhiDeltaEta_All_2D -> GetYaxis() -> SetTitle("#Delta#eta");
    DeltaPhiDeltaEta_All_2D -> GetXaxis() -> SetNdivisions(505);


    DeltaPhi_link_1D = new TH1F("","DeltaPhi_link_1D",  100, -5, 5);
    DeltaPhi_link_1D -> GetXaxis() -> SetTitle("#Delta#phi [degree]");
    DeltaPhi_link_1D -> GetYaxis() -> SetTitle("Entry");
    DeltaPhi_link_1D -> GetXaxis() -> SetNdivisions(505);

    DeltaEta_link_1D = new TH1F("","DeltaEta_link_1D", 100, -0.15, 0.15);
    DeltaEta_link_1D -> GetXaxis() -> SetTitle("#Delta#eta");
    DeltaEta_link_1D -> GetYaxis() -> SetTitle("Entry");
    DeltaEta_link_1D -> GetXaxis() -> SetNdivisions(505);

    DeltaPhiDeltaEta_link_2D = new TH2F("","DeltaPhiDeltaEta_link_2D",  100, -5, 5, 100, -5, 5);
    DeltaPhiDeltaEta_link_2D -> GetXaxis() -> SetTitle("#Delta#phi [degree]");
    DeltaPhiDeltaEta_link_2D -> GetYaxis() -> SetTitle("#Delta#eta");
    DeltaPhiDeltaEta_link_2D -> GetXaxis() -> SetNdivisions(505);

    DeltaR_best_1D = new TH1F("","DeltaR_best_1D", 100, -10, 10);
    DeltaR_best_1D -> GetXaxis() -> SetTitle("#DeltaR, with weighted #Delta#eta");
    DeltaR_best_1D -> GetYaxis() -> SetTitle("Entry");
    DeltaR_best_1D -> GetXaxis() -> SetNdivisions(505);

    DeltaPhiDeltaEta_best_2D = new TH2F("","DeltaPhiDeltaEta_best_2D",  100, -5, 5, 100, -5, 5);
    DeltaPhiDeltaEta_best_2D -> GetXaxis() -> SetTitle("#Delta#phi [degree]");
    DeltaPhiDeltaEta_best_2D -> GetYaxis() -> SetTitle("Weighted #Delta#eta");
    DeltaPhiDeltaEta_best_2D -> GetXaxis() -> SetNdivisions(505);

    DeltaEtaTrueEta_best_2D = new TH2F("","DeltaEtaTrueEta_best_2D",  100, -3, 3, 100, -0.15, 0.15);
    DeltaEtaTrueEta_best_2D -> GetXaxis() -> SetTitle("True track #eta");
    DeltaEtaTrueEta_best_2D -> GetYaxis() -> SetTitle("#Delta#eta [reco - true]");
    DeltaEtaTrueEta_best_2D -> GetXaxis() -> SetNdivisions(505);

    DeltaEtaRecoZ_best_2D = new TH2F("","DeltaEtaRecoZ_best_2D",  100, -400, 100, 100, -0.15, 0.15);
    DeltaEtaRecoZ_best_2D -> GetXaxis() -> SetTitle("Reco zvtx [mm]");
    DeltaEtaRecoZ_best_2D -> GetYaxis() -> SetTitle("#Delta#eta [reco - true]");
    DeltaEtaRecoZ_best_2D -> GetXaxis() -> SetNdivisions(505);

    DeltaEtaTrueZ_best_2D = new TH2F("","DeltaEtaTrueZ_best_2D",  100, -400, 100, 100, -0.15, 0.15);
    DeltaEtaTrueZ_best_2D -> GetXaxis() -> SetTitle("True zvtx [mm]");
    DeltaEtaTrueZ_best_2D -> GetYaxis() -> SetTitle("#Delta#eta [reco - true]");
    DeltaEtaTrueZ_best_2D -> GetXaxis() -> SetNdivisions(505);

}

void CheckDeltaR::Processing(int run_Nevent)
{
    cout << "CheckDeltaR::Processing()" << endl;

    double best_delta_phi;
    double best_delta_eta;
    double best_delta_R;

    int final_run_Nevent = (run_Nevent == -1 || run_Nevent > tree_in -> GetEntries()) ? tree_in -> GetEntries() : run_Nevent;

    for (int i = 0; i < final_run_Nevent/*tree_in -> GetEntries()*/; i++)
    {
        readeta -> LoadTree(i);
        readeta -> GetEntry(i);
        if (i % 1 == 0) cout << "Processing entry " << i << endl;

        if (readeta -> NTrueTrack > 500 || readeta -> NTrueTrack < 100) {continue;}
        if (readeta -> True_zvtx > -180 || readeta -> True_zvtx < -220) {continue;}

        for (int true_i = 0; true_i < readeta -> NTrueTrack; true_i++)
        {
            for (int reco_i = 0; reco_i < readeta -> N2Clu_track; reco_i++)
            {
                double delta_phi = readeta -> RecoTrack_phi_d -> at(reco_i) - readeta -> TrueTrack_phi_d -> at(true_i);
                double delta_eta = readeta -> RecoTrack_eta_d -> at(reco_i) - readeta -> TrueTrack_eta_d -> at(true_i);
                DeltaPhi_All_1D -> Fill(delta_phi);
                DeltaEta_All_1D -> Fill(delta_eta);
                DeltaPhiDeltaEta_All_2D -> Fill(delta_phi, delta_eta);

                if (reco_i == 0)
                {
                    best_delta_phi = delta_phi;
                    best_delta_eta = delta_eta;
                }
                else
                {
                    if (abs(delta_phi) < abs(best_delta_phi))
                    {
                        best_delta_phi = delta_phi;
                    }
                    if (abs(delta_eta) < abs(best_delta_eta))
                    {
                        best_delta_eta = delta_eta;
                    }
                
                }

                track_pair.push_back({double(true_i), double(reco_i), delta_phi, delta_eta, readeta -> TrueTrack_eta_d -> at(true_i)});
                track_pair_deltaR.push_back(sqrt(pow(delta_phi,2) + pow(delta_eta * eta_weight,2)));
            }

            DeltaPhi_link_1D -> Fill(best_delta_phi);
            DeltaEta_link_1D -> Fill(best_delta_eta);
        }

        long long vec_size = track_pair_deltaR.size();
        long long ind[track_pair_deltaR.size()];
        TMath::Sort(vec_size, &track_pair_deltaR[0], ind, false);

        for (int index = 0; index < track_pair_deltaR.size(); index++)
        {
            int better_index = ind[index];
            if (true_track_used_tag[int(track_pair[better_index][0])] != 0) {continue;}
            if (reco_track_used_tag[int(track_pair[better_index][1])] != 0) {continue;}

            DeltaR_best_1D -> Fill(track_pair_deltaR[better_index]);
            DeltaPhiDeltaEta_best_2D -> Fill(track_pair[better_index][2], track_pair[better_index][3]*eta_weight);

            if (track_pair_deltaR[better_index] < 2)
            {
                DeltaEtaTrueEta_best_2D -> Fill(track_pair[better_index][4], track_pair[better_index][3]);
                DeltaEtaRecoZ_best_2D -> Fill(readeta -> Evt_zvtx, track_pair[better_index][3]);
                DeltaEtaTrueZ_best_2D -> Fill(readeta -> True_zvtx, track_pair[better_index][3]);
            }

            true_track_used_tag[int(track_pair[better_index][0])] += 1;
            reco_track_used_tag[int(track_pair[better_index][1])] += 1;
        }

        ClearEvt();
    } // note : event loop

    return;
}

void CheckDeltaR::ClearEvt()
{
    track_pair.clear();
    track_pair_deltaR.clear();
    true_track_used_tag.clear();
    reco_track_used_tag.clear();
}

void CheckDeltaR::PrintPlots()
{
    c1 -> cd();
    DeltaPhi_All_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DeltaPhi_All_1D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    DeltaEta_All_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DeltaEta_All_1D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    DeltaPhiDeltaEta_All_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DeltaPhiDeltaEta_All_2D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    DeltaPhi_link_1D -> Draw("hist");
    cout<<"DeltaPhi_link_1D, stddev : "<<DeltaPhi_link_1D -> GetStdDev()<<endl;
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DeltaPhi_link_1D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    DeltaEta_link_1D -> Draw("hist");
    cout<<"DeltaEta_link_1D, stddev : "<<DeltaEta_link_1D -> GetStdDev()<<endl;
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DeltaEta_link_1D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    DeltaR_best_1D -> Draw("hist");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DeltaR_best_1D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    DeltaPhiDeltaEta_best_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DeltaPhiDeltaEta_best_2D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    DeltaEtaTrueEta_best_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DeltaEtaTrueEta_best_2D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    DeltaEtaRecoZ_best_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DeltaEtaRecoZ_best_2D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    DeltaEtaTrueZ_best_2D -> Draw("colz0");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
    c1 -> Print(Form("%s/DeltaEtaTrueZ_best_2D.pdf",out_folder_directory.c_str()));
    c1 -> Clear();

    return;
}