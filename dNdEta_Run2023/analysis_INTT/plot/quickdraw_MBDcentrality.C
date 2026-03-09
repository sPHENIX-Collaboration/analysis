float ytitleoffset = 1.6;

void quickdraw_MBDcentrality()
{
    std::string plotdir = "./quickcheck_MBDcentrality";
    system(Form("mkdir -p %s", plotdir.c_str()));

    // Set a timer to measure the time it takes to run the analysis
    TStopwatch timer;
    timer.Start();

    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("EventTree", "/sphenix/tg/tg01/hf/hjheng/ppg02/dst/Data_Run54280_20250210_ProdA2024/ntuple_0*.root");

    // auto count_all = df.Count();
    // std::cout << "Number of entries in the dataframe: " << *count_all << std::endl;

    // auto df_redefine = df.Redefine("MBD_centrality", "MBD_centrality * 100");
    auto df_isMB = df.Filter([](bool is_min_bias) { return is_min_bias; }, {"is_min_bias"});
    auto df_isMB_Trigbit10 = df.Filter([](bool is_min_bias, const std::vector<int> &firedTriggers) { return is_min_bias && std::find(firedTriggers.begin(), firedTriggers.end(), 10) != firedTriggers.end(); }, {"is_min_bias", "firedTriggers"}).Define("NOuterClus", "static_cast<int>(NClus - NClus_Layer1)");
    auto df_isMB_Trigbit12 = df.Filter([](bool is_min_bias, const std::vector<int> &firedTriggers) { return is_min_bias && std::find(firedTriggers.begin(), firedTriggers.end(), 12) != firedTriggers.end(); }, {"is_min_bias", "firedTriggers"});
    auto df_isMB_Trigbit13 = df.Filter([](bool is_min_bias, const std::vector<int> &firedTriggers) { return is_min_bias && std::find(firedTriggers.begin(), firedTriggers.end(), 13) != firedTriggers.end(); }, {"is_min_bias", "firedTriggers"});

    // auto count_isMB_Trigbit10 = df_isMB_Trigbit10.Count();
    // std::cout << "Number of entries in the dataframe with is_min_bias and Trigger bit 10: " << *count_isMB_Trigbit10 << std::endl;

    // histogram for MBD centrality without cut
    auto hM_MBDcentrality_wocut = df.Histo1D({"hM_MBDcentrality_wocut", "hM_MBDcentrality_wocut", 100, -0.5, 99.5}, "MBD_centrality");
    auto hM_MBDcentrality_isMinBias = df_isMB.Histo1D({"hM_MBDcentrality_isMinBias", "hM_MBDcentrality_isMinBias", 100, -0.5, 99.5}, "MBD_centrality");
    auto hM_MBDcentrality_Trigbit10 = df_isMB_Trigbit10.Histo1D({"hM_MBDcentrality_Trigbit10", "hM_MBDcentrality_Trigbit10", 100, -0.5, 99.5}, "MBD_centrality");
    auto hM_MBDcentrality_Trigbit12 = df_isMB_Trigbit12.Histo1D({"hM_MBDcentrality_Trigbit12", "hM_MBDcentrality_Trigbit12", 100, -0.5, 99.5}, "MBD_centrality");
    auto hM_MBDcentrality_Trigbit13 = df_isMB_Trigbit13.Histo1D({"hM_MBDcentrality_Trigbit13", "hM_MBDcentrality_Trigbit13", 100, -0.5, 99.5}, "MBD_centrality");
    auto hM_NInnerClus_MBDChargeSum_Trig10_IsMB = df_isMB_Trigbit10.Histo2D({"hM_NInnerClus_MBDChargeSum_Trig10_IsMB", "hM_NInnerClus_MBDChargeSum_Trig10_IsMB", 250, 0, 5000, 150, 0, 3000}, "NClus_Layer1", "MBD_charge_sum");
    auto hM_NOuterClus_MBDChargeSum_Trig10_IsMB = df_isMB_Trigbit10.Histo2D({"hM_NOuterClus_MBDChargeSum_Trig10_IsMB", "hM_NOuterClus_MBDChargeSum_Trig10_IsMB", 250, 0, 5000, 150, 0, 3000}, "NOuterClus", "MBD_charge_sum");
    auto hM_MBDcentrality_MBDZvtx_Trigbit10 = df_isMB_Trigbit10.Histo2D({"hM_MBDcentrality_MBDZvtx_Trigbit10", "hM_MBDcentrality_MBDZvtx_Trigbit10;MBD Z vertex [cm];Centrality [%]", 120, -30, 30, 101, -0.5, 100.5}, "MBD_z_vtx", "MBD_centrality");

    // Draw the histograms
    TCanvas *c = new TCanvas("canvas", "canvas", 800, 600);
    c->cd();
    gPad->SetTopMargin(0.25);
    hM_MBDcentrality_wocut->GetXaxis()->SetTitle("Centrality percentile");
    hM_MBDcentrality_wocut->GetYaxis()->SetTitle("Counts");
    hM_MBDcentrality_wocut->GetYaxis()->SetTitleOffset(ytitleoffset);
    hM_MBDcentrality_wocut->GetYaxis()->SetRangeUser(0, 1.1 * hM_MBDcentrality_wocut->GetMaximum());
    hM_MBDcentrality_wocut->SetLineColor(kBlack);
    hM_MBDcentrality_wocut->SetLineWidth(2);
    hM_MBDcentrality_isMinBias->SetLineColor(kTBriBlue);
    hM_MBDcentrality_isMinBias->SetLineWidth(2);
    hM_MBDcentrality_Trigbit10->SetLineColor(kTBriGreen);
    hM_MBDcentrality_Trigbit10->SetLineWidth(2);
    hM_MBDcentrality_Trigbit10->SetLineStyle(2);
    hM_MBDcentrality_Trigbit12->SetLineColor(kTBriYellow);
    hM_MBDcentrality_Trigbit12->SetLineWidth(2);
    hM_MBDcentrality_Trigbit13->SetLineColor(kTBriRed);
    hM_MBDcentrality_Trigbit13->SetLineWidth(2);
    hM_MBDcentrality_wocut->Draw("hist");
    hM_MBDcentrality_isMinBias->Draw("hist same");
    hM_MBDcentrality_Trigbit10->Draw("hist same");
    // hM_MBDcentrality_Trigbit12->Draw("hist same");
    // hM_MBDcentrality_Trigbit13->Draw("hist same");
    hM_MBDcentrality_wocut->Draw("hist same");
    c->RedrawAxis();
    TLegend *l = new TLegend(gPad->GetLeftMargin(), 1 - gPad->GetTopMargin() + 0.05, 1 - gPad->GetRightMargin() - 0.45, 0.98);
    l->SetNColumns(1);
    l->AddEntry(hM_MBDcentrality_wocut.GetPtr(), "Without cut", "l");
    l->AddEntry(hM_MBDcentrality_isMinBias.GetPtr(), "Is Min. Bias", "l");
    l->AddEntry(hM_MBDcentrality_Trigbit10.GetPtr(), "Is MB & Trigger bit 10 (MBD N&S#geq2)", "l");
    // l->AddEntry(hM_MBDcentrality_Trigbit12.GetPtr(), "Is MB & Trigger bit 12 (MBD N&S#geq2, |vtx|<10cm)", "l");
    // l->AddEntry(hM_MBDcentrality_Trigbit13.GetPtr(), "Is MB & Trigger bit 13 (MBD N&S#geq2, |vtx|<30cm)", "l");
    l->SetTextSize(0.035);
    l->SetBorderSize(0);
    l->SetFillStyle(0);
    l->Draw();
    c->SaveAs(Form("%s/MBDcentrality_selections.pdf", plotdir.c_str()));
    c->SaveAs(Form("%s/MBDcentrality_selections.png", plotdir.c_str()));

    c->Clear();
    c->cd();
    gPad->SetTopMargin(0.06);
    gPad->SetRightMargin(0.15);
    hM_MBDcentrality_MBDZvtx_Trigbit10->GetXaxis()->SetTitle("MBD Z vertex [cm]");
    hM_MBDcentrality_MBDZvtx_Trigbit10->GetYaxis()->SetTitle("Centrality [%]");
    // hM_MBDcentrality_MBDZvtx_Trigbit10->GetYaxis()->SetTitleOffset(ytitleoffset);
    hM_MBDcentrality_MBDZvtx_Trigbit10->Draw("colz");
    c->SaveAs(Form("%s/MBDcentrality_vs_MBDZvtx_Trigbit10.pdf", plotdir.c_str()));
    c->SaveAs(Form("%s/MBDcentrality_vs_MBDZvtx_Trigbit10.png", plotdir.c_str()));


    // TCanvas *c2 = new TCanvas("canvas2", "canvas2", 800, 700);
    // c2->cd();
    // c2->SetLogz();
    // gPad->SetRightMargin(0.15);
    // gPad->SetTopMargin(0.07);
    // hM_NInnerClus_MBDChargeSum_Trig10_IsMB->GetXaxis()->SetTitle("Number of inner clusters");
    // hM_NInnerClus_MBDChargeSum_Trig10_IsMB->GetYaxis()->SetTitle("MBD Charge Sum");
    // hM_NInnerClus_MBDChargeSum_Trig10_IsMB->GetYaxis()->SetTitleOffset(ytitleoffset);
    // // only maximal 3 digits for the axis labels
    // TGaxis::SetMaxDigits(3);
    // hM_NInnerClus_MBDChargeSum_Trig10_IsMB->Draw("colz");
    // // Text for selection
    // TLatex *t = new TLatex();
    // t->SetTextSize(0.035);
    // t->DrawLatexNDC(0.2, 0.85, "Is MinBias & Trigger bit 10 (MBD N&S#geq2)");
    // c2->SaveAs(Form("%s/NInnerClus_vs_MBDChargeSum.pdf", plotdir.c_str()));
    // c2->SaveAs(Form("%s/NInnerClus_vs_MBDChargeSum.png", plotdir.c_str()));

    // c2->Clear();
    // c2->cd();
    // c2->SetLogz();
    // gPad->SetRightMargin(0.15);
    // gPad->SetTopMargin(0.07);
    // hM_NOuterClus_MBDChargeSum_Trig10_IsMB->GetXaxis()->SetTitle("Number of outer clusters");
    // hM_NOuterClus_MBDChargeSum_Trig10_IsMB->GetYaxis()->SetTitle("MBD Charge Sum");
    // hM_NOuterClus_MBDChargeSum_Trig10_IsMB->GetYaxis()->SetTitleOffset(ytitleoffset);
    // // only maximal 3 digits for the axis labels
    // TGaxis::SetMaxDigits(3);
    // hM_NOuterClus_MBDChargeSum_Trig10_IsMB->Draw("colz");
    // t->DrawLatexNDC(0.2, 0.85, "Is MinBias & Trigger bit 10 (MBD N&S#geq2)");
    // c2->SaveAs(Form("%sNOuterClus_vs_MBDChargeSum.pdf", plotdir.c_str()));
    // c2->SaveAs(Form("%sNOuterClus_vs_MBDChargeSum.png", plotdir.c_str()));

    timer.Stop();
    timer.Print();
}