#include <cstdlib>

float TickSize = 0.03;
float AxisTitleSize = 0.05;
float AxisLabelSize = 0.045;
float LeftMargin = 0.15;
float RightMargin = 0.05;
float TopMargin = 0.08;
float BottomMargin = 0.13;
float textscale = 2.6;

void recotkl_DCA()
{
    // directory to save the output plots; if the directory does not exist, create it using
    std::string plotdir = "recotkl_DCA";
    system(Form("mkdir -p %s", plotdir.c_str()));

    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("minitree", "/sphenix/tg/tg01/hf/hjheng/ppg02/minitree/TrackletMinitree_Sim_HIJING_ananew_20250130/dRcut0p5_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0/minitree_00*.root");

    // histogram of reco-tracklet DCA, is is_min_bias
    auto h_recotklraw_DCA3d = df.Filter("is_min_bias&&abs(PV_z)<=10").Histo1D({"h_recotklraw_DCA3d", "h_recotklraw_DCA3d:Tracklet DCA (w.r.t event vertex) [cm]:Entries", 200, 0, 10}, "recotklraw_dca3dvtx");
    // define 2 new columns for the tracklets dca with the corresponding value in recotkl_isMatchedGChadron is true and is false
    auto df_with_matched = df.Define("recotkldca_matched",
                                     [](const std::vector<float> &recotklraw_dca3dvtx, const std::vector<bool> &recotkl_isMatchedGChadron)
                                     {
                                         std::vector<float> recotkl_matched;
                                         for (size_t i = 0; i < recotklraw_dca3dvtx.size(); i++)
                                         {
                                             if (recotkl_isMatchedGChadron[i])
                                             {
                                                 recotkl_matched.push_back(recotklraw_dca3dvtx[i]);
                                             }
                                         }
                                         return recotkl_matched;
                                     },
                                     {"recotklraw_dca3dvtx", "recotkl_isMatchedGChadron"})
                               .Define("recotkldca_unmatched",
                                       [](const std::vector<float> &recotklraw_dca3dvtx, const std::vector<bool> &recotkl_isMatchedGChadron)
                                       {
                                           std::vector<float> recotkl_unmatched;
                                           for (size_t i = 0; i < recotklraw_dca3dvtx.size(); i++)
                                           {
                                               if (!recotkl_isMatchedGChadron[i])
                                               {
                                                   recotkl_unmatched.push_back(recotklraw_dca3dvtx[i]);
                                               }
                                           }
                                           return recotkl_unmatched;
                                       },
                                       {"recotklraw_dca3dvtx", "recotkl_isMatchedGChadron"});

    auto h_recotklraw_DCA3d_matched = df_with_matched.Filter("is_min_bias&&abs(PV_z)<=10").Histo1D({"h_recotklraw_DCA3d_matched", "h_recotklraw_DCA3d_matched:Tracklet DCA (w.r.t event vertex) [cm]:Entries", 200, 0, 10}, "recotkldca_matched");
    auto h_recotklraw_DCA3d_unmatched = df_with_matched.Filter("is_min_bias&&abs(PV_z)<=10").Histo1D({"h_recotklraw_DCA3d_unmatched", "h_recotklraw_DCA3d_unmatched:Tracklet DCA (w.r.t event vertex) [cm]:Entries", 200, 0, 10}, "recotkldca_unmatched");

    // draw the histograms on a canvas; split the canvas into two pads, the top pad to draw the two histograms and the bottom pad to draw the ratio plot
    TCanvas *c = new TCanvas("c", "c", 800, 700);
    gPad->SetTopMargin(TopMargin);
    c->cd();
    THStack *hs = new THStack("hs", ";Tracklet DCA (w.r.t the event vertex) [cm];Entries");
    hs->Draw("hist");
    gPad->Modified();
    gPad->Update();
    h_recotklraw_DCA3d_matched->SetLineColor(kRed);
    h_recotklraw_DCA3d_matched->SetLineWidth(0);
    h_recotklraw_DCA3d_matched->SetFillColorAlpha(kRed, 0.3);
    hs->Add(h_recotklraw_DCA3d_matched.GetPtr());
    h_recotklraw_DCA3d_unmatched->SetLineColor(kBlue);
    h_recotklraw_DCA3d_unmatched->SetLineWidth(0);
    h_recotklraw_DCA3d_unmatched->SetFillColorAlpha(kBlue, 0.3);
    hs->Add(h_recotklraw_DCA3d_unmatched.GetPtr());
    hs->GetXaxis()->SetTitle("Tracklet DCA (w.r.t the event vertex) [cm]");
    hs->SetMaximum(h_recotklraw_DCA3d->GetMaximum()*1.3);
    hs->GetYaxis()->SetTitle("Entries");
    hs->GetYaxis()->SetTitleOffset(1.6);
    gPad->Modified();
    gPad->Update();
    h_recotklraw_DCA3d->SetLineColor(kBlack);
    h_recotklraw_DCA3d->SetLineWidth(2);
    h_recotklraw_DCA3d->Draw("hist same");
    TLegend *leg = new TLegend(gPad->GetLeftMargin() + 0.02, 1 - gPad->GetTopMargin() - 0.15, gPad->GetLeftMargin() + 0.3, 1 - gPad->GetTopMargin() - 0.04);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.035);
    leg->AddEntry(h_recotklraw_DCA3d.GetPtr(), "All tracklets", "l");
    leg->AddEntry(h_recotklraw_DCA3d_matched.GetPtr(), "Tracklets matched to primary charged hadrons", "f");
    leg->AddEntry(h_recotklraw_DCA3d_unmatched.GetPtr(), "Tracklets not matched to primary charged hadrons", "f");
    leg->Draw();
    c->SaveAs(Form("%s/recotkl_DCA3d.pdf", plotdir.c_str()));
    c->SaveAs(Form("%s/recotkl_DCA3d.png", plotdir.c_str()));
}