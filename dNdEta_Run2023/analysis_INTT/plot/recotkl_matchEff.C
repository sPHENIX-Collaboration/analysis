#include <cstdlib>

float TickSize = 0.03;
float AxisTitleSize = 0.05;
float AxisLabelSize = 0.045;
float LeftMargin = 0.15;
float RightMargin = 0.05;
float TopMargin = 0.08;
float BottomMargin = 0.13;
float textscale = 2.6;

std::vector<std::string> v_color{"#f2777a", "#6699cc", "#99cc99"};

void recotkl_matchEff()
{
    // directory to save the output plots; if the directory does not exist, create it using
    std::string plotdir = "recotkl_matchEff";
    system(Form("mkdir -p %s", plotdir.c_str()));

    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("minitree", "/sphenix/tg/tg01/hf/hjheng/ppg02/minitree/TrackletMinitree_Sim_SIMPLE_ana466_20250214/dRcut999p0_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0/minitree_00*.root");

    auto h_recotklraw_dR = df.Histo1D({"h_recotklraw_dR", "h_recotklraw_dR:Reco-tracklet #DeltaR:Entries", 100, 0, 1}, "recotklraw_dR");
    auto h_recotklraw_dPhi = df.Histo1D({"h_recotklraw_dPhi", "h_recotklraw_dPhi:Reco-tracklet #Delta#phi:Entries", 100, 0, 1}, "recotklraw_dphi");
    auto h_recotklraw_dEta = df.Histo1D({"h_recotklraw_dEta", "h_recotklraw_dEta:Reco-tracklet #Delta#eta:Entries", 100, 0, 1}, "recotklraw_deta");
    auto h_recotklraw_dR_zoomin = df.Histo1D({"h_recotklraw_dR_zoomin", "h_recotklraw_dR_zoomin:Reco-tracklet #DeltaR:Entries", 100, 0, 0.2}, "recotklraw_dR");
    auto h_recotklraw_dPhi_zoomin = df.Histo1D({"h_recotklraw_dPhi_zoomin", "h_recotklraw_dPhi_zoomin:Reco-tracklet #Delta#phi:Entries", 100, 0, 0.1}, "recotklraw_dphi");
    auto h_recotklraw_dEta_zoomin = df.Histo1D({"h_recotklraw_dEta_zoomin", "h_recotklraw_dEta_zoomin:Reco-tracklet #Delta#eta:Entries", 100, 0, 0.2}, "recotklraw_deta");
    auto h_genhadron_pt = df.Histo1D({"h_genhadron_pt", "h_genhadron_pt:Gen-hadron p_{T}:Entries", 100, 0, 10}, "GenHadron_Pt");
    auto h_genhadron_eta = df.Histo1D({"h_genhadron_eta", "h_genhadron_eta:Gen-hadron #eta:Entries", 105, -1.05, 1.05}, "GenHadron_eta");
    auto h_genhadron_phi = df.Histo1D({"h_genhadron_phi", "h_genhadron_phi:Gen-hadron #phi:Entries", 128, -3.2, 3.2}, "GenHadron_phi");
    // define a new column for the tracklets with the corresponding value in recotkl_isMatchedGChadron is true
    auto df_with_matched = df.Define("recotkl_dR_matched_posTrackID",
                                     [](const std::vector<float> &recotklraw_dR, const std::vector<int> &recotkl_clus1_matchedtrackID, const std::vector<int> &recotkl_clus2_matchedtrackID)
                                     {
                                         std::vector<float> recotkl_dR_matched;
                                         for (size_t i = 0; i < recotklraw_dR.size(); i++)
                                         {
                                             if (recotkl_clus1_matchedtrackID[i] == recotkl_clus2_matchedtrackID[i] && recotkl_clus1_matchedtrackID[i] > 0)
                                             {
                                                 recotkl_dR_matched.push_back(recotklraw_dR[i]);
                                             }
                                         }
                                         return recotkl_dR_matched;
                                     },
                                     {"recotklraw_dR", "recotkl_clus1_matchedtrackID", "recotkl_clus2_matchedtrackID"})
                               .Define("recotkl_dR_matched_negTrackID",
                                       [](const std::vector<float> &recotklraw_dR, const std::vector<int> &recotkl_clus1_matchedtrackID, const std::vector<int> &recotkl_clus2_matchedtrackID)
                                       {
                                           std::vector<float> recotkl_dR_matched;
                                           for (size_t i = 0; i < recotklraw_dR.size(); i++)
                                           {
                                               if (recotkl_clus1_matchedtrackID[i] == recotkl_clus2_matchedtrackID[i] && recotkl_clus1_matchedtrackID[i] < 0)
                                               {
                                                   recotkl_dR_matched.push_back(recotklraw_dR[i]);
                                               }
                                           }
                                           return recotkl_dR_matched;
                                       },
                                       {"recotklraw_dR", "recotkl_clus1_matchedtrackID", "recotkl_clus2_matchedtrackID"})
                               .Define("recotkl_dR_notmatched",
                                       [](const std::vector<float> &recotklraw_dR, const std::vector<int> &recotkl_clus1_matchedtrackID, const std::vector<int> &recotkl_clus2_matchedtrackID)
                                       {
                                           std::vector<float> recotkl_dR_notmatched;
                                           for (size_t i = 0; i < recotklraw_dR.size(); i++)
                                           {
                                               if (recotkl_clus1_matchedtrackID[i] != recotkl_clus2_matchedtrackID[i])
                                               {
                                                   recotkl_dR_notmatched.push_back(recotklraw_dR[i]);
                                               }
                                           }
                                           return recotkl_dR_notmatched;
                                       },
                                       {"recotklraw_dR", "recotkl_clus1_matchedtrackID", "recotkl_clus2_matchedtrackID"})
                               .Define("recotkl_dPhi_matched_posTrackID",
                                       [](const std::vector<float> &recotklraw_dphi, const std::vector<int> &recotkl_clus1_matchedtrackID, const std::vector<int> &recotkl_clus2_matchedtrackID)
                                       {
                                           std::vector<float> recotkl_dPhi_matched;
                                           for (size_t i = 0; i < recotklraw_dphi.size(); i++)
                                           {
                                               if (recotkl_clus1_matchedtrackID[i] == recotkl_clus2_matchedtrackID[i] && recotkl_clus1_matchedtrackID[i] > 0)
                                               {
                                                   recotkl_dPhi_matched.push_back(recotklraw_dphi[i]);
                                               }
                                           }
                                           return recotkl_dPhi_matched;
                                       },
                                       {"recotklraw_dphi", "recotkl_clus1_matchedtrackID", "recotkl_clus2_matchedtrackID"})
                               .Define("recotkl_dPhi_matched_negTrackID",
                                       [](const std::vector<float> &recotklraw_dphi, const std::vector<int> &recotkl_clus1_matchedtrackID, const std::vector<int> &recotkl_clus2_matchedtrackID)
                                       {
                                           std::vector<float> recotkl_dPhi_matched;
                                           for (size_t i = 0; i < recotklraw_dphi.size(); i++)
                                           {
                                               if (recotkl_clus1_matchedtrackID[i] == recotkl_clus2_matchedtrackID[i] && recotkl_clus1_matchedtrackID[i] < 0)
                                               {
                                                   recotkl_dPhi_matched.push_back(recotklraw_dphi[i]);
                                               }
                                           }
                                           return recotkl_dPhi_matched;
                                       },
                                       {"recotklraw_dphi", "recotkl_clus1_matchedtrackID", "recotkl_clus2_matchedtrackID"})
                               .Define("recotkl_dPhi_notmatched",
                                       [](const std::vector<float> &recotklraw_dphi, const std::vector<int> &recotkl_clus1_matchedtrackID, const std::vector<int> &recotkl_clus2_matchedtrackID)
                                       {
                                           std::vector<float> recotkl_dPhi_notmatched;
                                           for (size_t i = 0; i < recotklraw_dphi.size(); i++)
                                           {
                                               if (recotkl_clus1_matchedtrackID[i] != recotkl_clus2_matchedtrackID[i])
                                               {
                                                   recotkl_dPhi_notmatched.push_back(recotklraw_dphi[i]);
                                               }
                                           }
                                           return recotkl_dPhi_notmatched;
                                       },
                                       {"recotklraw_dphi", "recotkl_clus1_matchedtrackID", "recotkl_clus2_matchedtrackID"})
                               .Define("recotkl_dEta_matched_posTrackID",
                                       [](const std::vector<float> &recotklraw_deta, const std::vector<int> &recotkl_clus1_matchedtrackID, const std::vector<int> &recotkl_clus2_matchedtrackID)
                                       {
                                           std::vector<float> recotkl_dEta_matched;
                                           for (size_t i = 0; i < recotklraw_deta.size(); i++)
                                           {
                                               if (recotkl_clus1_matchedtrackID[i] == recotkl_clus2_matchedtrackID[i] && recotkl_clus1_matchedtrackID[i] > 0)
                                               {
                                                   recotkl_dEta_matched.push_back(recotklraw_deta[i]);
                                               }
                                           }
                                           return recotkl_dEta_matched;
                                       },
                                       {"recotklraw_deta", "recotkl_clus1_matchedtrackID", "recotkl_clus2_matchedtrackID"})
                               .Define("recotkl_dEta_matched_negTrackID",
                                       [](const std::vector<float> &recotklraw_deta, const std::vector<int> &recotkl_clus1_matchedtrackID, const std::vector<int> &recotkl_clus2_matchedtrackID)
                                       {
                                           std::vector<float> recotkl_dEta_matched;
                                           for (size_t i = 0; i < recotklraw_deta.size(); i++)
                                           {
                                               if (recotkl_clus1_matchedtrackID[i] == recotkl_clus2_matchedtrackID[i] && recotkl_clus1_matchedtrackID[i] < 0)
                                               {
                                                   recotkl_dEta_matched.push_back(recotklraw_deta[i]);
                                               }
                                           }
                                           return recotkl_dEta_matched;
                                       },
                                       {"recotklraw_deta", "recotkl_clus1_matchedtrackID", "recotkl_clus2_matchedtrackID"})
                               .Define("recotkl_dEta_notmatched",
                                       [](const std::vector<float> &recotklraw_deta, const std::vector<int> &recotkl_clus1_matchedtrackID, const std::vector<int> &recotkl_clus2_matchedtrackID)
                                       {
                                           std::vector<float> recotkl_dEta_notmatched;
                                           for (size_t i = 0; i < recotklraw_deta.size(); i++)
                                           {
                                               if (recotkl_clus1_matchedtrackID[i] != recotkl_clus2_matchedtrackID[i])
                                               {
                                                   recotkl_dEta_notmatched.push_back(recotklraw_deta[i]);
                                               }
                                           }
                                           return recotkl_dEta_notmatched;
                                       },
                                       {"recotklraw_deta", "recotkl_clus1_matchedtrackID", "recotkl_clus2_matchedtrackID"})
                               .Define("genhadron_pt_matched",
                                       [](const std::vector<float> &GenHadron_Pt, const std::vector<bool> &GenHadron_IsRecotkl)
                                       {
                                           std::vector<float> genhadron_pt_matched;
                                           for (size_t i = 0; i < GenHadron_Pt.size(); i++)
                                           {
                                               if (GenHadron_IsRecotkl[i])
                                               {
                                                   genhadron_pt_matched.push_back(GenHadron_Pt[i]);
                                               }
                                           }
                                           return genhadron_pt_matched;
                                       },
                                       {"GenHadron_Pt", "GenHadron_IsRecotkl"})
                               .Define("genhadron_eta_matched",
                                       [](const std::vector<float> &GenHadron_eta, const std::vector<bool> &GenHadron_IsRecotkl)
                                       {
                                           std::vector<float> genhadron_eta_matched;
                                           for (size_t i = 0; i < GenHadron_eta.size(); i++)
                                           {
                                               if (GenHadron_IsRecotkl[i])
                                               {
                                                   genhadron_eta_matched.push_back(GenHadron_eta[i]);
                                               }
                                           }
                                           return genhadron_eta_matched;
                                       },
                                       {"GenHadron_eta", "GenHadron_IsRecotkl"})
                               .Define("genhadron_phi_matched",
                                       [](const std::vector<float> &GenHadron_phi, const std::vector<bool> &GenHadron_IsRecotkl)
                                       {
                                           std::vector<float> genhadron_phi_matched;
                                           for (size_t i = 0; i < GenHadron_phi.size(); i++)
                                           {
                                               if (GenHadron_IsRecotkl[i])
                                               {
                                                   genhadron_phi_matched.push_back(GenHadron_phi[i]);
                                               }
                                           }
                                           return genhadron_phi_matched;
                                       },
                                       {"GenHadron_phi", "GenHadron_IsRecotkl"});

    auto h_recotklraw_dR_matched_posTrackID = df_with_matched.Histo1D({"h_recotklraw_dR_matched_posTrackID", "h_recotklraw_dR_matched_posTrackID:Tracklet #DeltaR:Entries", 100, 0, 1}, "recotkl_dR_matched_posTrackID");
    auto h_recotklraw_dPhi_matched_posTrackID = df_with_matched.Histo1D({"h_recotklraw_dPhi_matched_posTrackID", "h_recotklraw_dPhi_matched_posTrackID:Tracklet #Delta#phi:Entries", 100, 0, 1}, "recotkl_dPhi_matched_posTrackID");
    auto h_recotklraw_dEta_matched_posTrackID = df_with_matched.Histo1D({"h_recotklraw_dEta_matched_posTrackID", "h_recotklraw_dEta_matched_posTrackID:Tracklet #Delta#eta:Entries", 100, 0, 1}, "recotkl_dEta_matched_posTrackID");
    auto h_recotklraw_dR_matched_negTrackID = df_with_matched.Histo1D({"h_recotklraw_dR_matched_negTrackID", "h_recotklraw_dR_matched_negTrackID:Tracklet #DeltaR:Entries", 100, 0, 1}, "recotkl_dR_matched_negTrackID");
    auto h_recotklraw_dPhi_matched_negTrackID = df_with_matched.Histo1D({"h_recotklraw_dPhi_matched_negTrackID", "h_recotklraw_dPhi_matched_negTrackID:Tracklet #Delta#phi:Entries", 100, 0, 1}, "recotkl_dPhi_matched_negTrackID");
    auto h_recotklraw_dEta_matched_negTrackID = df_with_matched.Histo1D({"h_recotklraw_dEta_matched_negTrackID", "h_recotklraw_dEta_matched_negTrackID:Tracklet #Delta#eta:Entries", 100, 0, 1}, "recotkl_dEta_matched_negTrackID");
    auto h_recotklraw_dR_notmatched = df_with_matched.Histo1D({"h_recotklraw_dR_notmatched", "h_recotklraw_dR_notmatched:Tracklet #DeltaR:Entries", 100, 0, 1}, "recotkl_dR_notmatched");
    auto h_recotklraw_dPhi_notmatched = df_with_matched.Histo1D({"h_recotklraw_dPhi_notmatched", "h_recotklraw_dPhi_notmatched:Tracklet #Delta#phi:Entries", 100, 0, 1}, "recotkl_dPhi_notmatched");
    auto h_recotklraw_dEta_notmatched = df_with_matched.Histo1D({"h_recotklraw_dEta_notmatched", "h_recotklraw_dEta_notmatched:Tracklet #Delta#eta:Entries", 100, 0, 1}, "recotkl_dEta_notmatched");
    auto h_recotklraw_dR_zoomin_matched_posTrackID = df_with_matched.Histo1D({"h_recotklraw_dR_zoomin_matched_posTrackID", "h_recotklraw_dR_zoomin_matched_posTrackID:Tracklet #DeltaR:Entries", 100, 0, 0.2}, "recotkl_dR_matched_posTrackID");
    auto h_recotklraw_dPhi_zoomin_matched_posTrackID = df_with_matched.Histo1D({"h_recotklraw_dPhi_zoomin_matched_posTrackID", "h_recotklraw_dPhi_zoomin_matched_posTrackID:Tracklet #Delta#phi:Entries", 100, 0, 0.1}, "recotkl_dPhi_matched_posTrackID");
    auto h_recotklraw_dEta_zoomin_matched_posTrackID = df_with_matched.Histo1D({"h_recotklraw_dEta_zoomin_matched_posTrackID", "h_recotklraw_dEta_zoomin_matched_posTrackID:Tracklet #Delta#eta:Entries", 100, 0, 0.2}, "recotkl_dEta_matched_posTrackID");
    auto h_recotklraw_dR_zoomin_matched_negTrackID = df_with_matched.Histo1D({"h_recotklraw_dR_zoomin_matched_negTrackID", "h_recotklraw_dR_zoomin_matched_negTrackID:Tracklet #DeltaR:Entries", 100, 0, 0.2}, "recotkl_dR_matched_negTrackID");
    auto h_recotklraw_dPhi_zoomin_matched_negTrackID = df_with_matched.Histo1D({"h_recotklraw_dPhi_zoomin_matched_negTrackID", "h_recotklraw_dPhi_zoomin_matched_negTrackID:Tracklet #Delta#phi:Entries", 100, 0, 0.1}, "recotkl_dPhi_matched_negTrackID");
    auto h_recotklraw_dEta_zoomin_matched_negTrackID = df_with_matched.Histo1D({"h_recotklraw_dEta_zoomin_matched_negTrackID", "h_recotklraw_dEta_zoomin_matched_negTrackID:Tracklet #Delta#eta:Entries", 100, 0, 0.2}, "recotkl_dEta_matched_negTrackID");
    auto h_recotklraw_dR_zoomin_notmatched = df_with_matched.Histo1D({"h_recotklraw_dR_zoomin_notmatched", "h_recotklraw_dR_zoomin_notmatched:Tracklet #DeltaR:Entries", 100, 0, 0.2}, "recotkl_dR_notmatched");
    auto h_recotklraw_dPhi_zoomin_notmatched = df_with_matched.Histo1D({"h_recotklraw_dPhi_zoomin_notmatched", "h_recotklraw_dPhi_zoomin_notmatched:Tracklet #Delta#phi:Entries", 100, 0, 0.1}, "recotkl_dPhi_notmatched");
    auto h_recotklraw_dEta_zoomin_notmatched = df_with_matched.Histo1D({"h_recotklraw_dEta_zoomin_notmatched", "h_recotklraw_dEta_zoomin_notmatched:Tracklet #Delta#eta:Entries", 100, 0, 0.2}, "recotkl_dEta_notmatched");

    auto h_genhadron_pt_matched = df_with_matched.Histo1D({"h_genhadron_pt_matched", "h_genhadron_pt_matched:Gen-hadron p_{T}:Entries", 100, 0, 10}, "genhadron_pt_matched");
    auto h_genhadron_eta_matched = df_with_matched.Histo1D({"h_genhadron_eta_matched", "h_genhadron_eta_matched:Gen-hadron #eta:Entries", 105, -1.05, 1.05}, "genhadron_eta_matched");
    auto h_genhadron_phi_matched = df_with_matched.Histo1D({"h_genhadron_phi_matched", "h_genhadron_phi_matched:Gen-hadron #phi:Entries", 128, -3.2, 3.2}, "genhadron_phi_matched");

    // draw the histograms on a canvas; split the canvas into two pads, the top pad to draw the two histograms and the bottom pad to draw the ratio plot
    auto drawComparison = [&](TH1 *h_all, const std::vector<TH1 *> &vec_h_stack, bool logy, float ymaxscale, const std::string &xTitle, const std::vector<std::string> &v_leg, const std::string &plotName)
    {
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
        pad1->SetLogy(logy);
        float ymin = 1E10;
        THStack *hs = new THStack("hs", "");
        for (size_t i = 0; i < vec_h_stack.size(); ++i)
        {
            vec_h_stack[i]->SetLineColor(TColor::GetColor(v_color[i].c_str()));
            vec_h_stack[i]->SetFillColorAlpha(TColor::GetColor(v_color[i].c_str()), 0.5);
            hs->Add(vec_h_stack[i]);

            if (vec_h_stack[i]->GetMinimum(0) < ymin)
            {
                ymin = vec_h_stack[i]->GetMinimum(0);
            }
        }
        hs->Draw();
        gPad->Modified();
        gPad->Update();
        hs->GetXaxis()->SetTitleSize(0);
        hs->GetXaxis()->SetLabelSize(0);
        // hs->GetXaxis()->SetTitle(xTitle.c_str());
        hs->SetMaximum(h_all->GetMaximum() * ymaxscale);
        hs->SetMinimum(ymin * 0.9);
        hs->GetYaxis()->SetTitle("Counts");
        hs->GetYaxis()->SetTitleOffset(1.4);
        hs->GetYaxis()->SetTitleSize(AxisTitleSize);
        hs->GetYaxis()->SetLabelSize(AxisLabelSize);
        gPad->Modified();
        gPad->Update();
        h_all->Draw("hist same");

        TLegend *leg = new TLegend(1 - pad1->GetRightMargin() - 0.57,                          //
                                   1 - pad1->GetTopMargin() - 0.06 * (vec_h_stack.size() + 1), //
                                   1 - pad1->GetRightMargin() - 0.2,                           //
                                   1 - pad1->GetTopMargin() - 0.05                             //
        );
        leg->AddEntry(h_all, "All tracklets", "l");
        for (size_t i = 0; i < vec_h_stack.size(); ++i)
        {
            leg->AddEntry(vec_h_stack[i], v_leg[i].c_str(), "lf");
        }
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->SetTextSize(0.045);
        leg->Draw();

        pad2->cd();
        pad2->SetLogy(logy);
        float ratio_min = 999.;
        THStack *hs_ratio = new THStack("hs_ratio", "");
        for (size_t i = 0; i < vec_h_stack.size(); ++i)
        {
            TH1 *h_ratio = (TH1 *)vec_h_stack[i]->Clone(Form("h_ratio_%zu", i));
            h_ratio->Divide(h_all);
            h_ratio->SetFillColorAlpha(TColor::GetColor(v_color[i].c_str()), 0.5);
            h_ratio->SetLineColor(TColor::GetColor(v_color[i].c_str()));
            h_ratio->SetLineWidth(2);
            hs_ratio->Add(h_ratio);

            if (h_ratio->GetMinimum(0) < ratio_min)
            {
                ratio_min = h_ratio->GetMinimum(0);
            }
        }
        hs_ratio->Draw();
        // reset the style of the ratio plot
        gPad->Modified();
        gPad->Update();
        hs_ratio->GetYaxis()->SetTitle("Ratio");
        hs_ratio->SetMaximum(1.1);
        hs_ratio->SetMinimum((logy) ? ratio_min * 0.9 : 0);
        hs_ratio->GetYaxis()->SetTitleSize(AxisTitleSize * textscale);
        hs_ratio->GetYaxis()->SetTitleOffset(0.5);
        hs_ratio->GetYaxis()->SetLabelSize(AxisLabelSize * textscale);
        hs_ratio->GetXaxis()->SetTitle(xTitle.c_str());
        hs_ratio->GetXaxis()->SetTitleSize(AxisTitleSize * textscale);
        hs_ratio->GetXaxis()->SetTitleOffset(1.1);
        hs_ratio->GetXaxis()->SetLabelSize(AxisLabelSize * textscale);
        hs_ratio->Draw();
        TLine *line = new TLine(hs_ratio->GetXaxis()->GetXmin(), 1, hs_ratio->GetXaxis()->GetXmax(), 1);
        line->SetLineStyle(2);
        line->Draw();

        c->SaveAs(Form("%s/%s.pdf", plotdir.c_str(), plotName.c_str()));
        c->SaveAs(Form("%s/%s.png", plotdir.c_str(), plotName.c_str()));
    };

    std::vector<std::string> hleg{"Matched to G4P with positive track ID", "Matched to G4P with negative track ID", "Not matched"};
    drawComparison(h_recotklraw_dR.GetPtr(), {h_recotklraw_dR_matched_posTrackID.GetPtr(), h_recotklraw_dR_matched_negTrackID.GetPtr(), h_recotklraw_dR_notmatched.GetPtr()}, true, 5, "Reco-tracklet #DeltaR", hleg, "recotkl_matchEff_dR");
    drawComparison(h_recotklraw_dPhi.GetPtr(), {h_recotklraw_dPhi_matched_posTrackID.GetPtr(), h_recotklraw_dPhi_matched_negTrackID.GetPtr(), h_recotklraw_dPhi_notmatched.GetPtr()}, true, 5, "Reco-tracklet #Delta#phi", hleg, "recotkl_matchEff_dPhi");
    drawComparison(h_recotklraw_dEta.GetPtr(), {h_recotklraw_dEta_matched_posTrackID.GetPtr(), h_recotklraw_dEta_matched_negTrackID.GetPtr(), h_recotklraw_dEta_notmatched.GetPtr()}, true, 5, "Reco-tracklet #Delta#eta", hleg, "recotkl_matchEff_dEta");
    drawComparison(h_recotklraw_dR_zoomin.GetPtr(), {h_recotklraw_dR_zoomin_matched_posTrackID.GetPtr(), h_recotklraw_dR_zoomin_matched_negTrackID.GetPtr(), h_recotklraw_dR_zoomin_notmatched.GetPtr()}, true, 50, "Reco-tracklet #DeltaR", hleg, "recotkl_matchEff_dR_zoomin");
    drawComparison(h_recotklraw_dPhi_zoomin.GetPtr(), {h_recotklraw_dPhi_zoomin_matched_posTrackID.GetPtr(), h_recotklraw_dPhi_zoomin_matched_negTrackID.GetPtr(), h_recotklraw_dPhi_zoomin_notmatched.GetPtr()}, true, 5, "Reco-tracklet #Delta#phi", hleg, "recotkl_matchEff_dPhi_zoomin");
    drawComparison(h_recotklraw_dEta_zoomin.GetPtr(), {h_recotklraw_dEta_zoomin_matched_posTrackID.GetPtr(), h_recotklraw_dEta_zoomin_matched_negTrackID.GetPtr(), h_recotklraw_dEta_zoomin_notmatched.GetPtr()}, true, 50, "Reco-tracklet #Delta#eta", hleg, "recotkl_matchEff_dEta_zoomin");
    drawComparison(h_genhadron_pt.GetPtr(), {h_genhadron_pt_matched.GetPtr()}, false, 1.4, "Gen-hadron p_{T}", {"Matched to reco-tracklets"}, "recotkl_matchEff_genhadron_pt");
    drawComparison(h_genhadron_eta.GetPtr(), {h_genhadron_eta_matched.GetPtr()}, false, 1.4, "Gen-hadron #eta", {"Matched to reco-tracklets"}, "recotkl_matchEff_genhadron_eta");
    drawComparison(h_genhadron_phi.GetPtr(), {h_genhadron_phi_matched.GetPtr()}, false, 1.4, "Gen-hadron #phi", {"Matched to reco-tracklets"}, "recotkl_matchEff_genhadron_phi");
}