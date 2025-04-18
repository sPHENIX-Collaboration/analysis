float TickSize = 0.03;
float AxisTitleSize = 0.05;
float AxisLabelSize = 0.04;
float LeftMargin = 0.15;
float RightMargin = 0.08;
float TopMargin = 0.08;
float BottomMargin = 0.13;
float textscale = 2.7;

void InnerOuterClus_quickana()
{
    TH1F *hM_cent_IsMB_all = new TH1F("hM_cent_IsMB_all", "hM_cent_IsMB_all", 10, 0, 10);
    TH1F *hM_cent_IsMB_OuterInnerBranch = new TH1F("hM_cent_IsMB_OuterInnerBranch", "hM_cent_IsMB_OuterInnerBranch", 10, 0, 10);
    TH1F *hM_ratio = new TH1F("hM_ratio", "hM_ratio", 10, 0, 10);

    TChain *chain = new TChain("EventTree");
    chain->Add("./ntuple_00*.root");
    
    for (int i = 0; i < 10; i++)
    {
        TString cut_base = Form("is_min_bias==1&&(MBD_centrality>%f&&MBD_centrality<=%f)", i * 0.1, (i + 1) * 0.1);
        // TString cut_OuterInnerBranch = Form("is_min_bias==1&&(MBD_centrality>%f&&MBD_centrality<=%f)&&((NClus-NClus_Layer1)/NClus_Layer1)>=1.1", i * 0.1, (i + 1) * 0.1);
        TString cut_OuterInnerBranch = Form("is_min_bias==1&&(MBD_centrality>%f&&MBD_centrality<=%f)&&(NClus_Layer1<(NClus-NClus_Layer1)-5*TMath::Sqrt(NClus-NClus_Layer1))", i * 0.1, (i + 1) * 0.1); // inner cluster is smaller than outer cluster by 5 sigma
        std::cout << chain->GetEntries(cut_base.Data()) << " " << chain->GetEntries(cut_OuterInnerBranch.Data()) << " " << static_cast<double>(chain->GetEntries(cut_OuterInnerBranch.Data())) / static_cast<double>(chain->GetEntries(cut_base.Data())) * 100. << std::endl;

        hM_cent_IsMB_all->SetBinContent(i + 1, static_cast<double>(chain->GetEntries(cut_base.Data())));
        hM_cent_IsMB_OuterInnerBranch->SetBinContent(i + 1, static_cast<double>(chain->GetEntries(cut_OuterInnerBranch.Data())));
        hM_ratio->SetBinContent(i + 1, static_cast<double>(chain->GetEntries(cut_OuterInnerBranch.Data())) / static_cast<double>(chain->GetEntries(cut_base.Data())) * 100.);
    }

    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1);
    pad1->SetBottomMargin(0);
    pad1->SetTopMargin(TopMargin);
    pad1->SetRightMargin(RightMargin);
    pad1->Draw();
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.3);
    pad2->SetRightMargin(RightMargin);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.31);
    pad2->Draw();
    pad1->cd();
    pad1->SetLogy(1);
    hM_cent_IsMB_all->GetXaxis()->SetTitleSize(0);
    hM_cent_IsMB_all->GetXaxis()->SetLabelSize(0);
    hM_cent_IsMB_all->GetYaxis()->SetRangeUser(hM_cent_IsMB_OuterInnerBranch->GetMinimum(0)*0.5, hM_cent_IsMB_all->GetMaximum()*1000);
    hM_cent_IsMB_all->GetYaxis()->SetTitle("Counts");
    hM_cent_IsMB_all->GetYaxis()->SetTitleSize(AxisTitleSize);
    hM_cent_IsMB_all->GetYaxis()->SetLabelSize(AxisTitleSize);
    hM_cent_IsMB_all->SetMarkerSize(1.6);
    hM_cent_IsMB_all->Draw("histtext");
    hM_cent_IsMB_OuterInnerBranch->SetLineColor(kTVibRed);
    hM_cent_IsMB_OuterInnerBranch->SetMarkerSize(1.6);
    hM_cent_IsMB_OuterInnerBranch->Draw("histtextsame");
    TLegend *leg = new TLegend(0.55, 0.75, 0.85, 0.85);
    leg->AddEntry(hM_cent_IsMB_all, "Is Minimum Bias (MB)", "l");
    leg->AddEntry(hM_cent_IsMB_OuterInnerBranch, "Is MB && N_{Inner}<5#times#sigma(N_{outer})", "l");
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->Draw();
    pad2->cd();
    pad2->SetLogy(1);
    for (int i = 0; i < 10; i++)
    {
        hM_ratio->GetXaxis()->SetBinLabel(i + 1, Form("%d-%d%%", i * 10, (i + 1) * 10));
    }
    hM_ratio->SetLineColor(kBlack);
    hM_ratio->SetLineWidth(2);
    hM_ratio->GetYaxis()->SetTitle("Ratio (%)");
    hM_ratio->GetYaxis()->SetNdivisions(505);
    hM_ratio->GetYaxis()->SetTitleSize(AxisTitleSize*textscale);
    hM_ratio->GetYaxis()->SetTitleOffset(0.5);
    hM_ratio->GetYaxis()->SetLabelSize(AxisTitleSize*textscale);
    hM_ratio->GetYaxis()->SetRangeUser(hM_ratio->GetMinimum(0)*0.5, hM_ratio->GetMaximum()*100);
    hM_ratio->GetXaxis()->SetTitle("Centrality interval");
    hM_ratio->GetXaxis()->SetTitleSize(AxisTitleSize*textscale);
    hM_ratio->GetXaxis()->SetTitleOffset(1.1);
    hM_ratio->GetXaxis()->SetLabelSize(AxisTitleSize*textscale);
    gStyle->SetPaintTextFormat(".2e");
    hM_ratio->SetMarkerSize(1.5*textscale);
    hM_ratio->Draw("histtext");
    c1->SaveAs("InnerOuterClus_quickana.pdf");
}