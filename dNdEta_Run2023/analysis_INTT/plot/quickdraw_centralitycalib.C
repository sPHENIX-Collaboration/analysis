#include <filesystem>

float ytitleoffset = 1.6;

void quickdraw_centralitycalib()
{
    std::string plotdir = "./quickcheck_MBDcentrality";
    system(Form("mkdir -p %s", plotdir.c_str()));
    
    std::string ntpdir = "/sphenix/tg/tg01/hf/hjheng/ppg02/dst/Data_Run54280_20250114_ProdA2024";

    int ntotalfile = 326;
    // int ntotalfile = 5; // for testing
    int nfilesperturn = 5;
    TCanvas *c = new TCanvas("c", "c", 1400, 1200);
    c->Divide(2, 2);
    c->Print(Form("%s/centralitycalib_Run54280_ProdA2024_InRunDependence.pdf[", plotdir.c_str()));
    for (int ifile = 0; ifile < int(ntotalfile / nfilesperturn); ifile++)
    {
        std::unique_ptr<TH1F> hM_MBDcentrality_wocut(new TH1F("hM_MBDcentrality_wocut", "hM_MBDcentrality_wocut", 101, -0.5, 100.5));
        std::unique_ptr<TH1F> hM_MBDcentrality_isMinBias(new TH1F("hM_MBDcentrality_isMinBias", "hM_MBDcentrality_isMinBias", 101, -0.5, 100.5));
        std::unique_ptr<TH1F> hM_MBDcentrality_Trigbit10(new TH1F("hM_MBDcentrality_Trigbit10", "hM_MBDcentrality_Trigbit10", 101, -0.5, 100.5));
        std::unique_ptr<TH1F> hM_MBDcentrality_Trigbit12(new TH1F("hM_MBDcentrality_Trigbit12", "hM_MBDcentrality_Trigbit12", 101, -0.5, 100.5));
        std::unique_ptr<TH1F> hM_MBDcentrality_Trigbit13(new TH1F("hM_MBDcentrality_Trigbit13", "hM_MBDcentrality_Trigbit13", 101, -0.5, 100.5));

        std::unique_ptr<TH1F> hM_MBDchargesum_wocut(new TH1F("hM_MBDchargesum_wocut", "hM_MBDchargesum_wocut", 125, 0, 2500));
        std::unique_ptr<TH1F> hM_MBDchargesum_isMinBias(new TH1F("hM_MBDchargesum_isMinBias", "hM_MBDchargesum_isMinBias", 125, 0, 2500));
        std::unique_ptr<TH1F> hM_MBDchargesum_Trigbit10(new TH1F("hM_MBDchargesum_Trigbit10", "hM_MBDchargesum_Trigbit10", 125, 0, 2500));
        std::unique_ptr<TH1F> hM_MBDchargesum_Trigbit12(new TH1F("hM_MBDchargesum_Trigbit12", "hM_MBDchargesum_Trigbit12", 125, 0, 2500));
        std::unique_ptr<TH1F> hM_MBDchargesum_Trigbit13(new TH1F("hM_MBDchargesum_Trigbit13", "hM_MBDchargesum_Trigbit13", 125, 0, 2500));

        std::unique_ptr<TH1F> hM_MBDchargeassym_wocut(new TH1F("hM_MBDchargeassym_wocut", "hM_MBDchargeassym_wocut", 100, -1, 1));
        std::unique_ptr<TH1F> hM_MBDchargeassym_isMinBias(new TH1F("hM_MBDchargeassym_isMinBias", "hM_MBDchargeassym_isMinBias", 100, -1, 1));
        std::unique_ptr<TH1F> hM_MBDchargeassym_Trigbit10(new TH1F("hM_MBDchargeassym_Trigbit10", "hM_MBDchargeassym_Trigbit10", 100, -1, 1));
        std::unique_ptr<TH1F> hM_MBDchargeassym_Trigbit12(new TH1F("hM_MBDchargeassym_Trigbit12", "hM_MBDchargeassym_Trigbit12", 100, -1, 1));
        std::unique_ptr<TH1F> hM_MBDchargeassym_Trigbit13(new TH1F("hM_MBDchargeassym_Trigbit13", "hM_MBDchargeassym_Trigbit13", 100, -1, 1));

        std::unique_ptr<TH1F> hM_MBDZvtx_wocut(new TH1F("hM_MBDZvtx_wocut", "hM_MBDZvtx_wocut", 140, -70, 70));
        std::unique_ptr<TH1F> hM_MBDZvtx_isMinBias(new TH1F("hM_MBDZvtx_isMinBias", "hM_MBDZvtx_isMinBias", 140, -70, 70));
        std::unique_ptr<TH1F> hM_MBDZvtx_Trigbit10(new TH1F("hM_MBDZvtx_Trigbit10", "hM_MBDZvtx_Trigbit10", 140, -70, 70));
        std::unique_ptr<TH1F> hM_MBDZvtx_Trigbit12(new TH1F("hM_MBDZvtx_Trigbit12", "hM_MBDZvtx_Trigbit12", 140, -70, 70));
        std::unique_ptr<TH1F> hM_MBDZvtx_Trigbit13(new TH1F("hM_MBDZvtx_Trigbit13", "hM_MBDZvtx_Trigbit13", 140, -70, 70));

        vector<uint64_t> vec_gl1bco;
        vec_gl1bco.clear();

        for (int i = ifile*nfilesperturn; i < (ifile+1)*nfilesperturn; i++)
        {
            std::string formatidx = std::string(TString::Format("%05d", i).Data());
            std::string filename = Form("%s/ntuple_%s.root", ntpdir.c_str(), formatidx.c_str());
            // check if the file exists
            std::filesystem::path entry(filename);
            if (!std::filesystem::exists(entry))
                continue;

            std::cout << "Processing " << filename << std::endl;

            TFile *f = new TFile(filename.c_str(), "READ");
            TTree *t = (TTree *)f->Get("EventTree");
            uint64_t GL1Packet_BCO;
            std::vector<int> *firedTriggers = 0;
            bool is_min_bias;
            float MBD_z_vtx, MBD_centrality, MBD_charge_sum, mbd_charge_asymm;
            t->SetBranchAddress("GL1Packet_BCO", &GL1Packet_BCO);
            t->SetBranchAddress("firedTriggers", &firedTriggers);
            t->SetBranchAddress("is_min_bias", &is_min_bias);
            t->SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);
            t->SetBranchAddress("MBD_centrality", &MBD_centrality);
            t->SetBranchAddress("MBD_charge_sum", &MBD_charge_sum);
            t->SetBranchAddress("MBD_charge_asymm", &mbd_charge_asymm);
            for (int ev = 0; ev < t->GetEntriesFast(); ev++)
            {
                t->GetEntry(ev);
                vec_gl1bco.push_back(GL1Packet_BCO);
                MBD_centrality = MBD_centrality * 100;
                hM_MBDcentrality_wocut->Fill(MBD_centrality);
                hM_MBDchargesum_wocut->Fill(MBD_charge_sum);
                hM_MBDchargeassym_wocut->Fill(mbd_charge_asymm);
                hM_MBDZvtx_wocut->Fill(MBD_z_vtx);
                if (is_min_bias)
                {
                    hM_MBDcentrality_isMinBias->Fill(MBD_centrality);
                    hM_MBDchargesum_isMinBias->Fill(MBD_charge_sum);
                    hM_MBDchargeassym_isMinBias->Fill(mbd_charge_asymm);
                    hM_MBDZvtx_isMinBias->Fill(MBD_z_vtx);
                }
                if (is_min_bias && std::find(firedTriggers->begin(), firedTriggers->end(), 10) != firedTriggers->end())
                {
                    hM_MBDcentrality_Trigbit10->Fill(MBD_centrality);
                    hM_MBDchargesum_Trigbit10->Fill(MBD_charge_sum);
                    hM_MBDchargeassym_Trigbit10->Fill(mbd_charge_asymm);
                    hM_MBDZvtx_Trigbit10->Fill(MBD_z_vtx);
                }
                if (is_min_bias && std::find(firedTriggers->begin(), firedTriggers->end(), 12) != firedTriggers->end())
                {
                    hM_MBDcentrality_Trigbit12->Fill(MBD_centrality);
                    hM_MBDchargesum_Trigbit12->Fill(MBD_charge_sum);
                    hM_MBDchargeassym_Trigbit12->Fill(mbd_charge_asymm);
                    hM_MBDZvtx_Trigbit12->Fill(MBD_z_vtx);
                }
                if (is_min_bias && std::find(firedTriggers->begin(), firedTriggers->end(), 13) != firedTriggers->end())
                {
                    hM_MBDcentrality_Trigbit13->Fill(MBD_centrality);
                    hM_MBDchargesum_Trigbit13->Fill(MBD_charge_sum);
                    hM_MBDchargeassym_Trigbit13->Fill(mbd_charge_asymm);
                    hM_MBDZvtx_Trigbit13->Fill(MBD_z_vtx);
                }
            }
            f->Close();
        }

        c->cd(1);
        gPad->SetTopMargin(0.25);
        hM_MBDcentrality_wocut->GetXaxis()->SetTitle("Centrality percentile");
        hM_MBDcentrality_wocut->GetYaxis()->SetTitle("Counts");
        hM_MBDcentrality_wocut->GetYaxis()->SetTitleOffset(ytitleoffset);
        hM_MBDcentrality_wocut->GetYaxis()->SetRangeUser(0, 1.3 * hM_MBDcentrality_wocut->GetMaximum());
        hM_MBDcentrality_wocut->SetLineColor(kBlack);
        hM_MBDcentrality_wocut->SetLineWidth(2);
        hM_MBDcentrality_isMinBias->SetLineColor(kTBriBlue);
        hM_MBDcentrality_isMinBias->SetLineWidth(2);
        hM_MBDcentrality_Trigbit10->SetLineColor(kTBriGreen);
        hM_MBDcentrality_Trigbit10->SetLineWidth(2);
        hM_MBDcentrality_Trigbit12->SetLineColor(kTBriYellow);
        hM_MBDcentrality_Trigbit12->SetLineWidth(2);
        hM_MBDcentrality_Trigbit13->SetLineColor(kTBriRed);
        hM_MBDcentrality_Trigbit13->SetLineWidth(2);
        hM_MBDcentrality_wocut->Draw("hist");
        hM_MBDcentrality_isMinBias->Draw("hist same");
        hM_MBDcentrality_Trigbit10->Draw("hist same");
        hM_MBDcentrality_Trigbit12->Draw("hist same");
        hM_MBDcentrality_Trigbit13->Draw("hist same");
        hM_MBDcentrality_wocut->Draw("hist same");
        c->RedrawAxis();
        TLegend *l = new TLegend(gPad->GetLeftMargin(), 1 - gPad->GetTopMargin() + 0.02, 1 - gPad->GetRightMargin() - 0.45, 0.98);
        l->SetNColumns(1);
        l->AddEntry(hM_MBDcentrality_wocut.get(), "Without cut", "l");
        l->AddEntry(hM_MBDcentrality_isMinBias.get(), "Is min bias", "l");
        l->AddEntry(hM_MBDcentrality_Trigbit10.get(), "Is MB & Trigger bit 10 (MBD N&S#geq2)", "l");
        l->AddEntry(hM_MBDcentrality_Trigbit12.get(), "Is MB & Trigger bit 12 (MBD N&S#geq2, |vtx|<10cm)", "l");
        l->AddEntry(hM_MBDcentrality_Trigbit13.get(), "Is MB & Trigger bit 13 (MBD N&S#geq2, |vtx|<30cm)", "l");
        l->SetTextSize(0.035);
        l->SetBorderSize(0);
        l->SetFillStyle(0);
        l->Draw();
        TLatex *t = new TLatex();
        t->SetTextAlign(23);
        t->SetTextSize(0.035);
        t->DrawLatexNDC(0.5, 1 - gPad->GetTopMargin() - 0.04, Form("GL1 BCO [%lu,%lu]", vec_gl1bco.front(), vec_gl1bco.back()));

        c->cd(2);
        gPad->SetTopMargin(0.05);
        gPad->SetLogy();
        hM_MBDchargesum_wocut->GetXaxis()->SetTitle("MBD Charge Sum");
        hM_MBDchargesum_wocut->GetYaxis()->SetTitle("Counts");
        hM_MBDchargesum_wocut->GetYaxis()->SetTitleOffset(ytitleoffset);
        hM_MBDchargesum_wocut->GetYaxis()->SetRangeUser(0.5, 10 * hM_MBDchargesum_wocut->GetMaximum());
        hM_MBDchargesum_wocut->SetLineColor(kBlack);
        hM_MBDchargesum_wocut->SetLineWidth(2);
        hM_MBDchargesum_isMinBias->SetLineColor(kTBriBlue);
        hM_MBDchargesum_isMinBias->SetLineWidth(2);
        hM_MBDchargesum_Trigbit10->SetLineColor(kTBriGreen);
        hM_MBDchargesum_Trigbit10->SetLineWidth(2);
        hM_MBDchargesum_Trigbit12->SetLineColor(kTBriYellow);
        hM_MBDchargesum_Trigbit12->SetLineWidth(2);
        hM_MBDchargesum_Trigbit13->SetLineColor(kTBriRed);
        hM_MBDchargesum_Trigbit13->SetLineWidth(2);
        hM_MBDchargesum_wocut->Draw("hist");
        hM_MBDchargesum_isMinBias->Draw("hist same");
        hM_MBDchargesum_Trigbit10->Draw("hist same");
        hM_MBDchargesum_Trigbit12->Draw("hist same");
        hM_MBDchargesum_Trigbit13->Draw("hist same");
        hM_MBDchargesum_wocut->Draw("hist same");
        c->RedrawAxis();

        c->cd(3);
        gPad->SetTopMargin(0.05);
        hM_MBDchargeassym_wocut->GetXaxis()->SetTitle("MBD Charge Asymmetry");
        hM_MBDchargeassym_wocut->GetYaxis()->SetTitle("Counts");
        hM_MBDchargeassym_wocut->GetYaxis()->SetTitleOffset(ytitleoffset);
        hM_MBDchargeassym_wocut->GetYaxis()->SetRangeUser(0, 1.3 * hM_MBDchargeassym_wocut->GetMaximum());
        hM_MBDchargeassym_wocut->SetLineColor(kBlack);
        hM_MBDchargeassym_wocut->SetLineWidth(2);
        hM_MBDchargeassym_isMinBias->SetLineColor(kTBriBlue);
        hM_MBDchargeassym_isMinBias->SetLineWidth(2);
        hM_MBDchargeassym_Trigbit10->SetLineColor(kTBriGreen);
        hM_MBDchargeassym_Trigbit10->SetLineWidth(2);
        hM_MBDchargeassym_Trigbit12->SetLineColor(kTBriYellow);
        hM_MBDchargeassym_Trigbit12->SetLineWidth(2);
        hM_MBDchargeassym_Trigbit13->SetLineColor(kTBriRed);
        hM_MBDchargeassym_Trigbit13->SetLineWidth(2);
        hM_MBDchargeassym_wocut->Draw("hist");
        hM_MBDchargeassym_isMinBias->Draw("hist same");
        hM_MBDchargeassym_Trigbit10->Draw("hist same");
        hM_MBDchargeassym_Trigbit12->Draw("hist same");
        hM_MBDchargeassym_Trigbit13->Draw("hist same");
        hM_MBDchargeassym_wocut->Draw("hist same");
        c->RedrawAxis();

        c->cd(4);
        gPad->SetTopMargin(0.05);
        hM_MBDZvtx_wocut->GetXaxis()->SetTitle("MBD Z Vertex");
        hM_MBDZvtx_wocut->GetYaxis()->SetTitle("Counts");
        hM_MBDZvtx_wocut->GetYaxis()->SetTitleOffset(ytitleoffset);
        hM_MBDZvtx_wocut->GetYaxis()->SetRangeUser(0, 1.3 * hM_MBDZvtx_wocut->GetMaximum());
        hM_MBDZvtx_wocut->SetLineColor(kBlack);
        hM_MBDZvtx_wocut->SetLineWidth(2);
        hM_MBDZvtx_isMinBias->SetLineColor(kTBriBlue);
        hM_MBDZvtx_isMinBias->SetLineWidth(2);
        hM_MBDZvtx_Trigbit10->SetLineColor(kTBriGreen);
        hM_MBDZvtx_Trigbit10->SetLineWidth(2);
        hM_MBDZvtx_Trigbit12->SetLineColor(kTBriYellow);
        hM_MBDZvtx_Trigbit12->SetLineWidth(2);
        hM_MBDZvtx_Trigbit13->SetLineColor(kTBriRed);
        hM_MBDZvtx_Trigbit13->SetLineWidth(2);
        hM_MBDZvtx_wocut->Draw("hist");
        hM_MBDZvtx_isMinBias->Draw("hist same");
        hM_MBDZvtx_Trigbit10->Draw("hist same");
        hM_MBDZvtx_Trigbit12->Draw("hist same");
        hM_MBDZvtx_Trigbit13->Draw("hist same");
        hM_MBDZvtx_wocut->Draw("hist same");
        c->RedrawAxis();

        // c->SaveAs(Form("centralitycalib_Run54280_ProdA2024_ntuple%dto%d.pdf", initialidx, initialidx+nfilestorun-1));
        c->Print(Form("%s/centralitycalib_Run54280_ProdA2024_InRunDependence.pdf", plotdir.c_str()));
    }
    c->Print(Form("%s/centralitycalib_Run54280_ProdA2024_InRunDependence.pdf]", plotdir.c_str()));
}   