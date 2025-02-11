#include <cstdlib>

float TickSize = 0.03;
float AxisTitleSize = 0.05;
float AxisLabelSize = 0.045;
float LeftMargin = 0.15;
float RightMargin = 0.05;
float TopMargin = 0.08;
float BottomMargin = 0.13;
float textscale = 2.6;

void recotkl_matchEff()
{
    // directory to save the output plots; if the directory does not exist, create it using
    std::string plotdir = "recotkl_matchEff";
    system(Form("mkdir -p %s", plotdir.c_str()));

    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("minitree", "/sphenix/tg/tg01/hf/hjheng/ppg02/minitree/TrackletMinitree_Sim_HIJING_ananew_20250130/dRcut999p0_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0/minitree_00*.root");

    auto h_recotklraw_dR = df.Histo1D({"h_recotklraw_dR", "h_recotklraw_dR:Reco-tracklet #DeltaR:Entries", 100, 0, 1}, "recotklraw_dR");
    // define a new column for the tracklets with the corresponding value in recotkl_isMatchedGChadron is true
    auto df_with_matched = df.Define("recotkl_matched",
                                     [](const std::vector<float> &recotklraw_dR, const std::vector<bool> &recotkl_isMatchedGChadron)
                                     {
                                         std::vector<float> recotkl_matched;
                                         for (size_t i = 0; i < recotklraw_dR.size(); i++)
                                         {
                                             if (recotkl_isMatchedGChadron[i])
                                             {
                                                 recotkl_matched.push_back(recotklraw_dR[i]);
                                             }
                                         }
                                         return recotkl_matched;
                                     },
                                     {"recotklraw_dR", "recotkl_isMatchedGChadron"});

    auto h_recotklraw_dR_matched = df_with_matched.Histo1D({"h_recotklraw_dR_matched", "h_recotklraw_dR_matched:Tracklet #DeltaR:Entries", 100, 0, 1}, "recotkl_matched");

    // draw the histograms on a canvas; split the canvas into two pads, the top pad to draw the two histograms and the bottom pad to draw the ratio plot
    TCanvas *c = new TCanvas("c", "c", 800, 700);
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
    h_recotklraw_dR->GetXaxis()->SetTitleSize(0);
    h_recotklraw_dR->GetXaxis()->SetLabelSize(0);
    h_recotklraw_dR->GetYaxis()->SetRangeUser(h_recotklraw_dR_matched->GetMinimum(0) * 0.8, h_recotklraw_dR->GetMaximum() * 5);
    h_recotklraw_dR->GetYaxis()->SetTitleOffset(1.4);
    h_recotklraw_dR->GetYaxis()->SetTitle("Counts");
    h_recotklraw_dR->GetYaxis()->SetTitleSize(AxisTitleSize);
    h_recotklraw_dR->GetYaxis()->SetLabelSize(AxisLabelSize);
    h_recotklraw_dR->Draw("hist");
    h_recotklraw_dR_matched->SetLineColor(kRed);
    h_recotklraw_dR_matched->Draw("hist same");
    TLegend *leg = new TLegend(0.6, 0.72, 0.85, 0.85);
    leg->AddEntry(h_recotklraw_dR.GetPtr(), "All tracklets", "l");
    leg->AddEntry(h_recotklraw_dR_matched.GetPtr(), "Matched tracklets", "l");
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.055);
    leg->Draw();
    pad2->cd();
    pad2->SetLogy(1);
    TH1F *ratio = (TH1F *)h_recotklraw_dR_matched->Clone("ratio");
    ratio->Divide(h_recotklraw_dR.GetPtr());
    cout << ratio->GetMinimum(0) << endl;
    ratio->SetLineColor(kBlack);
    ratio->SetLineWidth(2);
    ratio->GetYaxis()->SetTitle("Ratio");
    ratio->GetYaxis()->SetRangeUser(ratio->GetMinimum(0) * 0.8, 2);
    // ratio->GetYaxis()->SetNdivisions(502);
    ratio->GetYaxis()->SetTitleSize(AxisTitleSize * textscale);
    ratio->GetYaxis()->SetTitleOffset(0.5);
    ratio->GetYaxis()->SetLabelSize(AxisLabelSize * textscale);
    ratio->GetXaxis()->SetTitle("Reco-tracklet #DeltaR");
    ratio->GetXaxis()->SetTitleSize(AxisTitleSize * textscale);
    ratio->GetXaxis()->SetTitleOffset(1.1);
    ratio->GetXaxis()->SetLabelSize(AxisLabelSize * textscale);
    ratio->Draw();
    // Draw a horizontal line at y = 1
    TLine *line = new TLine(ratio->GetXaxis()->GetXmin(), 1, ratio->GetXaxis()->GetXmax(), 1);
    line->SetLineStyle(2);
    line->Draw();
    c->SaveAs(Form("%s/recotkl_matchEff.pdf", plotdir.c_str()));
    c->SaveAs(Form("%s/recotkl_matchEff.png", plotdir.c_str()));
}