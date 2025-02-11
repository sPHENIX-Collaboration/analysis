#include <numeric>

float TickSize = 0.03;
float AxisTitleSize = 0.05;
float AxisLabelSize = 0.04;
float LeftMargin = 0.15;
float RightMargin = 0.05;
float TopMargin = 0.08;
float BottomMargin = 0.13;
float textscale = 2.7;

void draw2Dhist(TH2 *hM, const char *xtitle, const char *ytitle, bool logz, vector<const char *> plotinfo, const char *plotname)
{
    float ytitleoffset = 1.6;

    TCanvas *c = new TCanvas("c", "c", 800, 700);
    gPad->SetRightMargin(0.15);
    gPad->SetTopMargin(0.07);
    c->cd();
    if (logz)
        c->SetLogz();
    hM->GetXaxis()->SetTitle(xtitle);
    hM->GetYaxis()->SetTitle(ytitle);
    hM->GetYaxis()->SetTitleOffset(ytitleoffset);
    // only maximal 3 digits for the axis labels
    TGaxis::SetMaxDigits(3);
    hM->Draw("colz");
    TLatex *t = new TLatex();
    t->SetTextSize(0.035);
    for (size_t i = 0; i < plotinfo.size(); ++i)
    {
        t->DrawLatexNDC(0.2, 0.85 - i * 0.04, plotinfo[i]);
    }
    c->SaveAs(Form("%s.pdf", plotname));
    c->SaveAs(Form("%s.png", plotname));
}

void EvtInttBCODiff(const char *filename = "/sphenix/tg/tg01/hf/hjheng/ppg02/dst/Data_Run54280_20241204_ProdA2024/ntuple_00000.root", //
                    const char *outfile = "./test.root",                                                                              //
                    uint64_t bcodiffcut = 61,                                                                                         //
                    bool debug = true,                                                                                                //
                    bool snapshot = false,                                                                                            //
                    bool diagnosticplot = true                                                                                        //
)
{
    TStopwatch timer;
    timer.Start();

    // ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("EventTree", filename);

    auto br_INTT_BCO = df.Take<uint64_t>("INTT_BCO");
    auto br_GL1Packet_BCO = df.Take<uint64_t>("GL1Packet_BCO");
    // difference w.r.t. the previous element
    std::vector<uint64_t> INTT_BCO_diff_prev(br_INTT_BCO->size());
    std::adjacent_difference(br_INTT_BCO->begin(), br_INTT_BCO->end(), INTT_BCO_diff_prev.begin());
    INTT_BCO_diff_prev[0] = 0;
    std::vector<uint64_t> INTT_BCO_diff_next(br_INTT_BCO->size());
    std::copy(INTT_BCO_diff_prev.begin() + 1, INTT_BCO_diff_prev.end(), INTT_BCO_diff_next.begin());
    INTT_BCO_diff_next.back() = 0;

    std::vector<uint64_t> GL1Packet_BCO_diff_prev(br_GL1Packet_BCO->size());
    std::adjacent_difference(br_GL1Packet_BCO->begin(), br_GL1Packet_BCO->end(), GL1Packet_BCO_diff_prev.begin());
    GL1Packet_BCO_diff_prev[0] = 0;
    std::vector<uint64_t> GL1Packet_BCO_diff_next(br_GL1Packet_BCO->size());
    std::copy(GL1Packet_BCO_diff_prev.begin() + 1, GL1Packet_BCO_diff_prev.end(), GL1Packet_BCO_diff_next.begin());
    GL1Packet_BCO_diff_next.back() = 0;

    if (debug)
    {
        for (size_t i = 0; i < 10; ++i)
        {
            std::cout << "INTT_BCO[" << i << "] = " << br_INTT_BCO->at(i) << ", INTT_BCO_diff_prev[" << i << "] = " << INTT_BCO_diff_prev[i] << ", INTT_BCO_diff_next[" << i << "] = " << INTT_BCO_diff_next[i] << std::endl;
        }
        for (size_t i = br_INTT_BCO->size() - 10; i < br_INTT_BCO->size(); ++i)
        {
            std::cout << "INTT_BCO[" << i << "] = " << br_INTT_BCO->at(i) << ", INTT_BCO_diff_prev[" << i << "] = " << INTT_BCO_diff_prev[i] << ", INTT_BCO_diff_next[" << i << "] = " << INTT_BCO_diff_next[i] << std::endl;
        }
    }

    auto df_with_diff = df.Define("INTT_BCO_diff_prev", [&INTT_BCO_diff_prev](ULong64_t i) { return INTT_BCO_diff_prev[i]; }, {"rdfentry_"})
                            .Define("INTT_BCO_diff_next", [&INTT_BCO_diff_next](ULong64_t i) { return INTT_BCO_diff_next[i]; }, {"rdfentry_"})
                            .Define("GL1Packet_BCO_diff_prev", [&GL1Packet_BCO_diff_prev](ULong64_t i) { return GL1Packet_BCO_diff_prev[i]; }, {"rdfentry_"})
                            .Define("GL1Packet_BCO_diff_next", [&GL1Packet_BCO_diff_next](ULong64_t i) { return GL1Packet_BCO_diff_next[i]; }, {"rdfentry_"});

    // auto df_with_diff_and_flag = df_with_diff.Define("InttBco_IsToBeRemoved", [bcodiffcut](uint64_t INTT_BCO_diff_prev, uint64_t INTT_BCO_diff_next) { return ((INTT_BCO_diff_prev < bcodiffcut && INTT_BCO_diff_prev > 0) || (INTT_BCO_diff_next < bcodiffcut && INTT_BCO_diff_next > 0)); }, {"INTT_BCO_diff_prev", "INTT_BCO_diff_next"});
    auto df_with_diff_and_flag = df_with_diff.Define("InttBco_IsToBeRemoved", [bcodiffcut](uint64_t INTT_BCO_diff_next) { return (INTT_BCO_diff_next <= bcodiffcut && INTT_BCO_diff_next > 0); }, {"INTT_BCO_diff_next"});

    // get all the column names
    auto all_columns = df_with_diff_and_flag.GetColumnNames();
    if (debug)
    {
        cout << "All columns: " << endl;
        for (auto &col : all_columns)
        {
            cout << col << ", ";
        }
        cout << endl;
    }

    // remove "firedTriggers_name" from the list of all columns
    all_columns.erase(std::remove(all_columns.begin(), all_columns.end(), "firedTriggers_name"), all_columns.end());
    if (snapshot)
        df_with_diff_and_flag.Snapshot("EventTree", outfile, all_columns);

    if (diagnosticplot)
    {
        std::string plotdir = "./quickcheck_InttBcoDiffEvt";
        system(Form("mkdir -p %s", plotdir.c_str()));

        float ytitleoffset = 1.6;

        auto df_isMB_Trigbit10 =
            df_with_diff_and_flag.Filter([](bool is_min_bias, const std::vector<int> &firedTriggers, bool InttBco_IsToBeRemoved) { return is_min_bias && std::find(firedTriggers.begin(), firedTriggers.end(), 10) != firedTriggers.end() && !InttBco_IsToBeRemoved; }, {"is_min_bias", "firedTriggers", "InttBco_IsToBeRemoved"}).Define("NOuterClus", "static_cast<int>(NClus - NClus_Layer1)");
        auto hM_NInnerClus_NOuterClus_Trig10_IsMB = df_isMB_Trigbit10.Histo2D({"hM_NInnerClus_NOuterClus_Trig10_IsMB", "hM_NInnerClus_NOuterClus_Trig10_IsMB", 250, 0, 5000, 250, 0, 5000}, "NClus_Layer1", "NOuterClus");
        auto hM_NInnerClus_MBDChargeSum_Trig10_IsMB = df_isMB_Trigbit10.Histo2D({"hM_NInnerClus_MBDChargeSum_Trig10_IsMB", "hM_NInnerClus_MBDChargeSum_Trig10_IsMB", 250, 0, 5000, 250, 0, 5000}, "NClus_Layer1", "MBD_charge_sum");
        auto hM_NOuterClus_MBDChargeSum_Trig10_IsMB = df_isMB_Trigbit10.Histo2D({"hM_NOuterClus_MBDChargeSum_Trig10_IsMB", "hM_NOuterClus_MBDChargeSum_Trig10_IsMB", 250, 0, 5000, 250, 0, 5000}, "NOuterClus", "MBD_charge_sum");

        auto df_isMB_Trigbit_woBcoDiffEvtRemoval = df_with_diff_and_flag.Filter([](bool is_min_bias, const std::vector<int> &firedTriggers) { return is_min_bias && std::find(firedTriggers.begin(), firedTriggers.end(), 10) != firedTriggers.end(); }, {"is_min_bias", "firedTriggers"}).Define("NOuterClus", "static_cast<int>(NClus - NClus_Layer1)");
        auto hM_NInnerClus_NOuterClus_Trig10_IsMB_woBcoDiffEvtRemoval = df_isMB_Trigbit_woBcoDiffEvtRemoval.Histo2D({"hM_NInnerClus_NOuterClus_Trig10_IsMB_woBcoDiffEvtRemoval", "hM_NInnerClus_NOuterClus_Trig10_IsMB_woBcoDiffEvtRemoval", 250, 0, 5000, 250, 0, 5000}, "NClus_Layer1", "NOuterClus");
        auto hM_NInnerClus_MBDChargeSum_Trig10_IsMB_woBcoDiffEvtRemoval = df_isMB_Trigbit_woBcoDiffEvtRemoval.Histo2D({"hM_NInnerClus_MBDChargeSum_Trig10_IsMB_woBcoDiffEvtRemoval", "hM_NInnerClus_MBDChargeSum_Trig10_IsMB_woBcoDiffEvtRemoval", 250, 0, 5000, 250, 0, 5000}, "NClus_Layer1", "MBD_charge_sum");
        auto hM_NOuterClus_MBDChargeSum_Trig10_IsMB_woBcoDiffEvtRemoval = df_isMB_Trigbit_woBcoDiffEvtRemoval.Histo2D({"hM_NOuterClus_MBDChargeSum_Trig10_IsMB_woBcoDiffEvtRemoval", "hM_NOuterClus_MBDChargeSum_Trig10_IsMB_woBcoDiffEvtRemoval", 250, 0, 5000, 250, 0, 5000}, "NOuterClus", "MBD_charge_sum");

        auto df_isMB_Trigbit10_removed =
            df_with_diff_and_flag.Filter([](bool is_min_bias, const std::vector<int> &firedTriggers, bool InttBco_IsToBeRemoved) { return is_min_bias && std::find(firedTriggers.begin(), firedTriggers.end(), 10) != firedTriggers.end() && InttBco_IsToBeRemoved; }, {"is_min_bias", "firedTriggers", "InttBco_IsToBeRemoved"}).Define("NOuterClus", "static_cast<int>(NClus - NClus_Layer1)");
        auto hM_NInnerClus_NOuterClus_Trig10_IsMB_removed = df_isMB_Trigbit10_removed.Histo2D({"hM_NInnerClus_NOuterClus_Trig10_IsMB_removed", "hM_NInnerClus_NOuterClus_Trig10_IsMB_removed", 250, 0, 5000, 250, 0, 5000}, "NClus_Layer1", "NOuterClus");
        auto hM_NInnerClus_MBDChargeSum_Trig10_IsMB_removed = df_isMB_Trigbit10_removed.Histo2D({"hM_NInnerClus_MBDChargeSum_Trig10_IsMB_removed", "hM_NInnerClus_MBDChargeSum_Trig10_IsMB_removed", 250, 0, 5000, 250, 0, 5000}, "NClus_Layer1", "MBD_charge_sum");
        auto hM_NOuterClus_MBDChargeSum_Trig10_IsMB_removed = df_isMB_Trigbit10_removed.Histo2D({"hM_NOuterClus_MBDChargeSum_Trig10_IsMB_removed", "hM_NOuterClus_MBDChargeSum_Trig10_IsMB_removed", 250, 0, 5000, 250, 0, 5000}, "NOuterClus", "MBD_charge_sum");

        draw2Dhist(hM_NInnerClus_NOuterClus_Trig10_IsMB.GetPtr(), "Number of inner clusters", "Number of outer clusters", true, std::vector<const char *>{"Is MinBias & Trigger bit 10 (MBD N&S#geq2)", "w. abnormal event removal"}, Form("%s/NInnerClus_vs_NOuterClus_Trig10_IsMB", plotdir.c_str()));
        draw2Dhist(hM_NInnerClus_MBDChargeSum_Trig10_IsMB.GetPtr(), "Number of inner clusters", "MBD Charge Sum", true, std::vector<const char *>{"Is MinBias & Trigger bit 10 (MBD N&S#geq2)", "w. abnormal event removal"}, Form("%s/NInnerClus_vs_MBDChargeSum_Trig10_IsMB", plotdir.c_str()));
        draw2Dhist(hM_NOuterClus_MBDChargeSum_Trig10_IsMB.GetPtr(), "Number of outer clusters", "MBD Charge Sum", true, std::vector<const char *>{"Is MinBias & Trigger bit 10 (MBD N&S#geq2)", "w. abnormal event removal"}, Form("%s/NOuterClus_vs_MBDChargeSum_Trig10_IsMB", plotdir.c_str()));
        draw2Dhist(hM_NInnerClus_NOuterClus_Trig10_IsMB_woBcoDiffEvtRemoval.GetPtr(), "Number of inner clusters", "Number of outer clusters", true, std::vector<const char *>{"Is MinBias & Trigger bit 10 (MBD N&S#geq2)", "w.o abnormal event removal"}, Form("%s/NInnerClus_vs_NOuterClus_Trig10_IsMB_woBcoDiffEvtRemoval", plotdir.c_str()));
        draw2Dhist(hM_NInnerClus_MBDChargeSum_Trig10_IsMB_woBcoDiffEvtRemoval.GetPtr(), "Number of inner clusters", "MBD Charge Sum", true, std::vector<const char *>{"Is MinBias & Trigger bit 10 (MBD N&S#geq2)", "w.o abnormal event removal"}, Form("%s/NInnerClus_vs_MBDChargeSum_Trig10_IsMB_woBcoDiffEvtRemoval", plotdir.c_str()));
        draw2Dhist(hM_NOuterClus_MBDChargeSum_Trig10_IsMB_woBcoDiffEvtRemoval.GetPtr(), "Number of outer clusters", "MBD Charge Sum", true, std::vector<const char *>{"Is MinBias & Trigger bit 10 (MBD N&S#geq2)", "w.o abnormal event removal"}, Form("%s/NOuterClus_vs_MBDChargeSum_Trig10_IsMB_woBcoDiffEvtRemoval", plotdir.c_str()));
        draw2Dhist(hM_NInnerClus_NOuterClus_Trig10_IsMB_removed.GetPtr(), "Number of inner clusters", "Number of outer clusters", true, std::vector<const char *>{"Is MinBias & Trigger bit 10 (MBD N&S#geq2)", "removed events"}, Form("%s/NInnerClus_vs_NOuterClus_Trig10_IsMB_removed", plotdir.c_str()));
        draw2Dhist(hM_NInnerClus_MBDChargeSum_Trig10_IsMB_removed.GetPtr(), "Number of inner clusters", "MBD Charge Sum", true, std::vector<const char *>{"Is MinBias & Trigger bit 10 (MBD N&S#geq2)", "removed events"}, Form("%s/NInnerClus_vs_MBDChargeSum_Trig10_IsMB_removed", plotdir.c_str()));
        draw2Dhist(hM_NOuterClus_MBDChargeSum_Trig10_IsMB_removed.GetPtr(), "Number of outer clusters", "MBD Charge Sum", true, std::vector<const char *>{"Is MinBias & Trigger bit 10 (MBD N&S#geq2)", "removed events"}, Form("%s/NOuterClus_vs_MBDChargeSum_Trig10_IsMB_removed", plotdir.c_str()));

        // check number of events remaining after the cut per centrality interval
        TH1F *hM_cent_IsMB_all = new TH1F("hM_cent_IsMB_all", "hM_cent_IsMB_all", 10, 0, 10);
        TH1F *hM_cent_IsMB_OuterInnerBranch = new TH1F("hM_cent_IsMB_OuterInnerBranch", "hM_cent_IsMB_OuterInnerBranch", 10, 0, 10);
        TH1F *hM_ratio = new TH1F("hM_ratio", "hM_ratio", 10, 0, 10);
        for (int i = 0; i < 10; i++)
        {
            float low = i * 10;
            float high = (i + 1) * 10;
            int Nevt_isMB_Trigbit10_woBcoDiffEvtRemoval = *df_isMB_Trigbit_woBcoDiffEvtRemoval.Filter([low, high](float MBD_centrality) { return MBD_centrality > low && MBD_centrality <= high; }, {"MBD_centrality"}).Count();
            int Nevt_isMB_Trigbit10_wBcoDiffEvtRemoval = *df_isMB_Trigbit10.Filter([low, high](float MBD_centrality) { return MBD_centrality > low && MBD_centrality <= high; }, {"MBD_centrality"}).Count();
            hM_cent_IsMB_all->SetBinContent(i + 1, Nevt_isMB_Trigbit10_woBcoDiffEvtRemoval);
            hM_cent_IsMB_OuterInnerBranch->SetBinContent(i + 1, Nevt_isMB_Trigbit10_wBcoDiffEvtRemoval);
            hM_ratio->SetBinContent(i + 1, static_cast<double>(Nevt_isMB_Trigbit10_wBcoDiffEvtRemoval) / static_cast<double>(Nevt_isMB_Trigbit10_woBcoDiffEvtRemoval) * 100.);
            hM_ratio->GetXaxis()->SetBinLabel(i + 1, Form("%d-%d%%", i * 10, (i + 1) * 10));
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
        pad1->SetLogy(0);
        hM_cent_IsMB_all->GetXaxis()->SetTitleSize(0);
        hM_cent_IsMB_all->GetXaxis()->SetLabelSize(0);
        // hM_cent_IsMB_all->GetYaxis()->SetRangeUser(hM_cent_IsMB_OuterInnerBranch->GetMinimum(0) * 0.8, hM_cent_IsMB_all->GetMaximum() * 2);
        hM_cent_IsMB_all->GetYaxis()->SetRangeUser(0, hM_cent_IsMB_all->GetMaximum() * 1.5);
        hM_cent_IsMB_all->GetYaxis()->SetMoreLogLabels(true);
        hM_cent_IsMB_all->GetYaxis()->SetTitle("Counts");
        hM_cent_IsMB_all->GetYaxis()->SetTitleSize(AxisTitleSize);
        hM_cent_IsMB_all->GetYaxis()->SetLabelSize(AxisTitleSize);
        hM_cent_IsMB_all->SetMarkerSize(1.6);
        hM_cent_IsMB_all->Draw("hist");
        hM_cent_IsMB_OuterInnerBranch->SetLineColor(kRed);
        hM_cent_IsMB_OuterInnerBranch->SetMarkerSize(1.6);
        hM_cent_IsMB_OuterInnerBranch->Draw("histsame");
        TLegend *leg = new TLegend(0.35, 0.72, 0.7, 0.85);
        leg->AddEntry(hM_cent_IsMB_all, "Is MB && Trigger bit 10", "l");
        leg->AddEntry(hM_cent_IsMB_OuterInnerBranch, "Is MB && Trigger bit 10 && BCO removal", "l");
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->SetTextSize(0.055);
        leg->Draw();
        pad2->cd();
        // pad2->SetLogy(1);
        hM_ratio->SetLineColor(kBlack);
        hM_ratio->SetLineWidth(2);
        hM_ratio->GetYaxis()->SetTitle("Fraction (%)");
        hM_ratio->GetYaxis()->SetNdivisions(502);
        hM_ratio->GetYaxis()->SetTitleSize(AxisTitleSize * textscale);
        hM_ratio->GetYaxis()->SetTitleOffset(0.5);
        hM_ratio->GetYaxis()->SetLabelSize(AxisTitleSize * textscale);
        hM_ratio->GetYaxis()->SetRangeUser(0, 199);
        hM_ratio->GetXaxis()->SetTitle("Centrality interval");
        hM_ratio->GetXaxis()->SetTitleSize(AxisTitleSize * textscale);
        hM_ratio->GetXaxis()->SetTitleOffset(1.1);
        hM_ratio->GetXaxis()->SetLabelSize(AxisTitleSize * textscale);
        gStyle->SetPaintTextFormat(".2f%%");
        hM_ratio->SetMarkerSize(1.5 * textscale);
        hM_ratio->Draw("histtext");
        c1->SaveAs(Form("%s/fracEvtRemoval_centrality.pdf", plotdir.c_str()));
        c1->SaveAs(Form("%s/fracEvtRemoval_centrality.png", plotdir.c_str()));
    }

    timer.Stop();
    timer.Print();
}