std::vector<float> centbin = {0, 3, 6, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70};
int ncent = centbin.size() - 1;

int GetMbinNum(const std::string &fstr)
{
    if (fstr.find("Centrality0to3") != std::string::npos)
        return 0;
    if (fstr.find("Centrality3to6") != std::string::npos)
        return 1;
    if (fstr.find("Centrality6to10") != std::string::npos)
        return 2;
    if (fstr.find("Centrality10to15") != std::string::npos)
        return 3;
    if (fstr.find("Centrality15to20") != std::string::npos)
        return 4;
    if (fstr.find("Centrality20to25") != std::string::npos)
        return 5;
    if (fstr.find("Centrality25to30") != std::string::npos)
        return 6;
    if (fstr.find("Centrality30to35") != std::string::npos)
        return 7;
    if (fstr.find("Centrality35to40") != std::string::npos)
        return 8;
    if (fstr.find("Centrality40to45") != std::string::npos)
        return 9;
    if (fstr.find("Centrality45to50") != std::string::npos)
        return 10;
    if (fstr.find("Centrality50to55") != std::string::npos)
        return 11;
    if (fstr.find("Centrality55to60") != std::string::npos)
        return 12;
    if (fstr.find("Centrality60to65") != std::string::npos)
        return 13;
    if (fstr.find("Centrality65to70") != std::string::npos)
        return 14;
    if (fstr.find("Centrality0to70") != std::string::npos)
        return 70;

    return -1; // Default case if no match is found
}

TH1D* GetCWHist(int Mbin)
{
    TFile *f = new TFile(Form("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/FinalResult_10cm_Pol2BkgFit/completed/vtxZ_-10_10cm_MBin%d/Final_Mbin%d_00054280/Final_Mbin%d_00054280.root", Mbin, Mbin, Mbin), "READ");
    TH1D *h = (TH1D *)f->Get("h1D_dNdEta_truth");
    h->SetDirectory(0);
    f->Close();
    return h;
}

void quickdraw_genhadron()
{
    // directory to save the output plots; if the directory does not exist, create it using
    std::string plotdir = "quickdraw_genhadron";
    system(Form("mkdir -p %s", plotdir.c_str()));

    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("minitree", "/sphenix/tg/tg01/hf/hjheng/ppg02/minitree/TrackletMinitree_Sim_HIJING_MDC2_ana467_20250225/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0/minitree_set2.root");

    for (int i = 0; i < ncent; i++)
    {
        auto hM_genhadron = df.Define("GenHadron_eta_le4", "GenHadron_eta[abs(GenHadron_eta) < 4]").Filter(Form("is_min_bias&&(PV_z<=10&&PV_z>=-10)&&(MBD_centrality>%f && MBD_centrality<=%f)", centbin[i], centbin[i + 1])).Histo1D({"hM_genhadron", "hM_genhadron:Gen-hadron #eta:Entries", 27, -2.7, 2.7}, "GenHadron_eta_le4", "vtxzwei");
        auto h1WGevent = df.Filter(Form("is_min_bias&&(abs(PV_z)<=10.)&&(MBD_centrality>%f && MBD_centrality<=%f)", centbin[i], centbin[i + 1])).Histo1D({"h1WGevent", "h1WGevent:PV_z:Entries", 20, -10, 10}, "PV_z", "vtxzwei");
        float nWGevent = h1WGevent->Integral(0, h1WGevent->GetNbinsX() + 1);

        cout << "Number of entries in hM_genhadron: " << hM_genhadron->GetEntries() << "; number of entries in h1WGevent: " << h1WGevent->Integral(0, h1WGevent->GetNbinsX() + 1) << endl;

        hM_genhadron->Scale(1. / nWGevent, "width");

        std::string centstr = Form("Centrality%dto%d", (int)centbin[i], (int)centbin[i + 1]);
        TH1D *hM_CW = GetCWHist(GetMbinNum(centstr));

        // draw the histogram
        TCanvas *c = new TCanvas("c", "c", 800, 700);
        hM_genhadron->GetYaxis()->SetTitle("Entries");
        hM_genhadron->GetYaxis()->SetTitleOffset(1.4);
        hM_genhadron->GetYaxis()->SetRangeUser(hM_genhadron->GetMinimum(0) * 0.9, hM_genhadron->GetMaximum() * 1.2);
        hM_genhadron->SetMarkerSize(0);
        hM_genhadron->GetXaxis()->SetTitle("Gen-hadron #eta");
        hM_genhadron->Draw("histe");
        hM_CW->SetLineColor(kRed);
        hM_CW->SetMarkerSize(0);
        hM_CW->Draw("histe same");
        TLegend *l = new TLegend(1 - gPad->GetRightMargin() - 0.35, 1 - gPad->GetTopMargin() - 0.2, 1 - gPad->GetRightMargin() - 0.05, 1 - gPad->GetTopMargin() - 0.05);
        l->SetHeader(Form("Centrality %d-%d%%", (int)centbin[i], (int)centbin[i + 1]));
        l->SetTextAlign(kHAlignLeft + kVAlignTop);
        l->AddEntry(hM_genhadron.GetPtr(), "CMS approach", "l");
        l->AddEntry(hM_CW, "PHOBOS approach", "l");
        l->SetTextSize(0.035);
        l->SetBorderSize(0);
        l->SetFillStyle(0);
        l->Draw();
        c->SaveAs(Form("%s/hM_genhadron_centrality%dto%d.pdf", plotdir.c_str(), (int)centbin[i], (int)centbin[i + 1]));
        c->SaveAs(Form("%s/hM_genhadron_centrality%dto%d.png", plotdir.c_str(), (int)centbin[i], (int)centbin[i + 1]));
    }

    system(Form("cd %s; gs -dNOPAUSE -sDEVICE=pdfwrite -sOUTPUTFILE=hM_genhadron_crosscheck_merged.pdf -dBATCH hM_genhadron_centrality*.pdf", plotdir.c_str()));
}