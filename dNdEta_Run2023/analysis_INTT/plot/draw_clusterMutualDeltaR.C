void draw_clusterMutualDeltaR()
{
std::string plotdir = "./clusterMutualDeltaR";
    system(Form("mkdir -p %s", plotdir.c_str()));

    // data histogram
    TFile *fdata = new TFile("/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Data_Run54280_20250117_ProdA2024/Cluster/hists_merged.root", "READ");
    TH1F *hdata = (TH1F *)fdata->Get("hM_mutualdRcluster_all");
    hdata->SetDirectory(0);
    fdata->Close();

    TFile *fsim = new TFile("/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/plot/hists/Sim_HIJING_ananew_20250131/Cluster/hists_merged.root", "READ");
    TH1F *hsim = (TH1F *)fsim->Get("hM_mutualdRcluster_all");
    hsim->SetDirectory(0);
    fsim->Close();

    // normalize the histograms
    hdata->Scale(1.0 / hdata->Integral());
    hsim->Scale(1.0 / hsim->Integral());
    // rebin 
    // hdata->Rebin(2);
    // hsim->Rebin(2);

    // draw the histograms
    TCanvas *c = new TCanvas("c", "c", 800, 700);
    c->cd();
    hsim->SetLineColor(kRed);
    hsim->SetLineWidth(2);
    hsim->GetXaxis()->SetTitle("#DeltaR(cluster_{i}, cluster_{j})");
    hsim->GetXaxis()->SetNdivisions(505);
    hsim->GetYaxis()->SetTitle("Normalized entries");
    hsim->GetYaxis()->SetTitleOffset(1.7);
    hsim->GetYaxis()->SetRangeUser(0, max(hdata->GetMaximum(), hsim->GetMaximum()) * 1.2);
    hsim->Draw("hist");
    hdata->SetLineColor(kBlack);
    hdata->SetLineWidth(2);
    hdata->SetMarkerStyle(20);
    hdata->SetMarkerSize(0.8);
    hdata->SetMarkerColor(kBlack);
    hdata->Draw("E1 same");
    TLegend *leg = new TLegend(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.1, gPad->GetLeftMargin() + 0.5, 1 - gPad->GetTopMargin() - 0.05);
    leg->SetNColumns(2);
    leg->AddEntry(hdata, "Data", "lep");
    leg->AddEntry(hsim, "Simulation", "l");
    leg->SetTextSize(0.04);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->Draw();
    c->SaveAs(Form("%s/clusterMutualDeltaR.pdf", plotdir.c_str()));
    c->SaveAs(Form("%s/clusterMutualDeltaR.png", plotdir.c_str()));
}