int Nevt = 100;

void valid()
{
    // Read in two ntuple files into Rdataframe
    // ROOT::EnableImplicitMT(); 
    // ROOT::RDataFrame df_ori("EventTree", "/sphenix/tg/tg01/hf/hjheng/ppg02/dst/Sim_HIJING_ana457_20250110/ntuple_00000.root");
    ROOT::RDataFrame df_ori("EventTree", "/sphenix/tg/tg01/hf/hjheng/ppg02/dst/Sim_HIJING_MDC2_ana467_20250225/ntuple_00000.root");
    ROOT::RDataFrame df_add40perc("EventTree", "/sphenix/tg/tg01/hf/hjheng/ppg02/dst/Sim_HIJING_addStrange40perc_ana457_20250113/ntuple_00000.root");
    // ROOT::RDataFrame df_add40perc("EventTree", "/sphenix/tg/tg01/hf/hjheng/ppg02/dst/Sim_HIJING_strangeness_MDC2_ana467_20250226/ntuple_00000.root");
    ROOT::RDataFrame df_add100perc("EventTree", "/sphenix/tg/tg01/hf/hjheng/ppg02/dst/Sim_HIJING_addStrange100perc_ana457_20250114/ntuple_00000.root");

    std::vector<int> list_strangePID = {310, 3122, -3122}; // list of PID for strange particles to check
    // From the HepMCFSPrtl_PID column, count the number of strange particles (PID is in list_strangePID) and create a new column for an integer, Nstrange_HepMC, for the count
    // And from the PrimaryG4P_PID column, count the number of strange particles and add a new column, Nstrange_PHG4, for the count
    auto df_ori_Nstrange = df_ori
                               .Define("Nstrange_HepMC",
                                       [list_strangePID](const std::vector<int> &HepMCFSPrtl_PID)
                                       {
                                           // Count occurrences of PIDs in list_strangePID within HepMCFSPrtl_PID
                                           return static_cast<int>(std::count_if(HepMCFSPrtl_PID.begin(), HepMCFSPrtl_PID.end(), [&list_strangePID](int pid) { return std::find(list_strangePID.begin(), list_strangePID.end(), pid) != list_strangePID.end(); }));
                                       },
                                       {"HepMCFSPrtl_PID"})
                               .Define("Nstrange_PHG4",
                                       [list_strangePID](const std::vector<int> &PrimaryG4P_PID)
                                       {
                                           // Count occurrences of PIDs in list_strangePID within PrimaryG4P_PID
                                           return static_cast<int>(std::count_if(PrimaryG4P_PID.begin(), PrimaryG4P_PID.end(), [&list_strangePID](int pid) { return std::find(list_strangePID.begin(), list_strangePID.end(), pid) != list_strangePID.end(); }));
                                       },
                                       {"PrimaryG4P_PID"});

    auto df_add40perc_Nstrange = df_add40perc
                                     .Define("Nstrange_HepMC",
                                             [list_strangePID](const std::vector<int> &HepMCFSPrtl_PID)
                                             {
                                                 // Count occurrences of PIDs in list_strangePID within HepMCFSPrtl_PID
                                                 return static_cast<int>(std::count_if(HepMCFSPrtl_PID.begin(), HepMCFSPrtl_PID.end(), [&list_strangePID](int pid) { return std::find(list_strangePID.begin(), list_strangePID.end(), pid) != list_strangePID.end(); }));
                                             },
                                             {"HepMCFSPrtl_PID"})
                                     .Define("Nstrange_PHG4",
                                             [list_strangePID](const std::vector<int> &PrimaryG4P_PID)
                                             {
                                                 // Count occurrences of PIDs in list_strangePID within PrimaryG4P_PID
                                                 return static_cast<int>(std::count_if(PrimaryG4P_PID.begin(), PrimaryG4P_PID.end(), [&list_strangePID](int pid) { return std::find(list_strangePID.begin(), list_strangePID.end(), pid) != list_strangePID.end(); }));
                                             },
                                             {"PrimaryG4P_PID"});

    auto df_add100perc_Nstrange = df_add100perc
                                      .Define("Nstrange_HepMC",
                                              [list_strangePID](const std::vector<int> &HepMCFSPrtl_PID)
                                              {
                                                  // Count occurrences of PIDs in list_strangePID within HepMCFSPrtl_PID
                                                  return static_cast<int>(std::count_if(HepMCFSPrtl_PID.begin(), HepMCFSPrtl_PID.end(), [&list_strangePID](int pid) { return std::find(list_strangePID.begin(), list_strangePID.end(), pid) != list_strangePID.end(); }));
                                              },
                                              {"HepMCFSPrtl_PID"})
                                      .Define("Nstrange_PHG4",
                                              [list_strangePID](const std::vector<int> &PrimaryG4P_PID)
                                              {
                                                  // Count occurrences of PIDs in list_strangePID within PrimaryG4P_PID
                                                  return static_cast<int>(std::count_if(PrimaryG4P_PID.begin(), PrimaryG4P_PID.end(), [&list_strangePID](int pid) { return std::find(list_strangePID.begin(), list_strangePID.end(), pid) != list_strangePID.end(); }));
                                              },
                                              {"PrimaryG4P_PID"});

    // Histograms of the PrimaryG4P_Pt and PrimaryG4P_Eta of the 40% and 100% additional strangeness samples
    auto h_ori_PHG4_Pt = df_ori.Histo1D({"h_ori_PHG4_Pt", ";PHG4Particle p_{T} [GeV]; Normalized entries", 100, 0, 10}, "PrimaryG4P_Pt");
    auto h_ori_PHG4_Eta = df_ori.Histo1D({"h_ori_PHG4_Eta", ";PHG4Particle #eta; Normalized entries", 40, -1, 1}, "PrimaryG4P_Eta");
    auto h_add40perc_PHG4_Pt = df_add40perc.Histo1D({"h_add40perc_PHG4_Pt", ";PHG4Particle p_{T} [GeV]; Normalized entries", 100, 0, 10}, "PrimaryG4P_Pt");
    auto h_add40perc_PHG4_Eta = df_add40perc.Histo1D({"h_add40perc_PHG4_Eta", ";PHG4Particle #eta; Normalized entries", 40, -1, 1}, "PrimaryG4P_Eta");
    auto h_add100perc_PHG4_Pt = df_add100perc.Histo1D({"h_add100perc_PHG4_Pt", ";PHG4Particle p_{T} [GeV]; Normalized entries", 100, 0, 10}, "PrimaryG4P_Pt");
    auto h_add100perc_PHG4_Eta = df_add100perc.Histo1D({"h_add100perc_PHG4_Eta", ";PHG4Particle #eta; Normalized entries", 40, -1, 1}, "PrimaryG4P_Eta");

    // Make histogram/graphs for the number of strange particles in HepMC and PHG4 as a function of event number
    // First, get the Nstrange_HepMC/Nstrange_PHG4 columns as vectors
    auto Nstrange_HepMC_ori = df_ori_Nstrange.Take<int>("Nstrange_HepMC");
    auto Nstrange_PHG4_ori = df_ori_Nstrange.Take<int>("Nstrange_PHG4");
    auto Nstrange_HepMC_add40perc = df_add40perc_Nstrange.Take<int>("Nstrange_HepMC");
    auto Nstrange_PHG4_add40perc = df_add40perc_Nstrange.Take<int>("Nstrange_PHG4");
    auto Nstrange_HepMC_add100perc = df_add100perc_Nstrange.Take<int>("Nstrange_HepMC");
    auto Nstrange_PHG4_add100perc = df_add100perc_Nstrange.Take<int>("Nstrange_PHG4");

    TH1F *h_Nstrange_HepMC_ori = new TH1F("h_Nstrange_HepMC_ori", ";Event index;N_{strange (K_{s}^{0},#Lambda)}", Nevt+1, -0.5, Nevt+0.5);
    TH1F *h_Nstrange_PHG4_ori = new TH1F("h_Nstrange_PHG4_ori", ";Event index;N_{strange (K_{s}^{0},#Lambda)}", Nevt+1, -0.5, Nevt+0.5);
    TH1F *h_Nstrange_HepMC_add40perc = new TH1F("h_Nstrange_HepMC_add40perc", ";Event index;N_{strange (K_{s}^{0},#Lambda)}", Nevt+1, -0.5, Nevt+0.5);
    TH1F *h_Nstrange_PHG4_add40perc = new TH1F("h_Nstrange_PHG4_add40perc", ";Event index;N_{strange (K_{s}^{0},#Lambda)}", Nevt+1, -0.5, Nevt+0.5);
    TH1F *h_Nstrange_HepMC_add100perc = new TH1F("h_Nstrange_HepMC_add100perc", ";Event index;N_{strange (K_{s}^{0},#Lambda)}", Nevt+1, -0.5, Nevt+0.5);
    TH1F *h_Nstrange_PHG4_add100perc = new TH1F("h_Nstrange_PHG4_add100perc", ";Event index;N_{strange (K_{s}^{0},#Lambda)}", Nevt+1, -0.5, Nevt+0.5);

    for (int i = 0; i < Nevt; i++)
    {
        cout << "Event " << i << ": Nstrange HepMC (ori,40%,100%): " << (*Nstrange_HepMC_ori)[i] << ", " << (*Nstrange_HepMC_add40perc)[i] << ", " << (*Nstrange_HepMC_add100perc)[i] << "; Nstrange PHG4 (ori,40%,100%): " << (*Nstrange_PHG4_ori)[i] << ", " << (*Nstrange_PHG4_add40perc)[i] << ", " << (*Nstrange_PHG4_add100perc)[i] << endl;
        h_Nstrange_HepMC_ori->SetBinContent(i + 1, (*Nstrange_HepMC_ori)[i]);
        h_Nstrange_PHG4_ori->SetBinContent(i + 1, (*Nstrange_PHG4_ori)[i]);
        h_Nstrange_HepMC_add40perc->SetBinContent(i + 1, (*Nstrange_HepMC_add40perc)[i]);
        h_Nstrange_PHG4_add40perc->SetBinContent(i + 1, (*Nstrange_PHG4_add40perc)[i]);
        h_Nstrange_HepMC_add100perc->SetBinContent(i + 1, (*Nstrange_HepMC_add100perc)[i]);
        h_Nstrange_PHG4_add100perc->SetBinContent(i + 1, (*Nstrange_PHG4_add100perc)[i]);
    }

    TH1F *h_NstrangeRatio_PHG4_add40perc = (TH1F*) h_Nstrange_PHG4_add40perc->Clone("h_NstrangeRatio_PHG4_add40perc");
    h_NstrangeRatio_PHG4_add40perc->Divide(h_Nstrange_HepMC_ori);
    TH1F *h_NstrangeRatio_PHG4_add100perc = (TH1F*) h_Nstrange_PHG4_add100perc->Clone("h_NstrangeRatio_PHG4_add100perc");
    h_NstrangeRatio_PHG4_add100perc->Divide(h_Nstrange_HepMC_ori);

    // get the maximum value among all histograms
    double maxval = std::max({h_Nstrange_HepMC_ori->GetMaximum(), h_Nstrange_PHG4_ori->GetMaximum(), h_Nstrange_HepMC_add40perc->GetMaximum(), h_Nstrange_PHG4_add40perc->GetMaximum(), h_Nstrange_HepMC_add100perc->GetMaximum(), h_Nstrange_PHG4_add100perc->GetMaximum()});

    TCanvas *c1 = new TCanvas("c1", "c1", 800, 700);
    gPad->SetTopMargin(0.08);
    c1->cd();
    h_Nstrange_HepMC_ori->SetLineColor(kBlack);
    h_Nstrange_PHG4_ori->SetLineColor(4);
    h_Nstrange_HepMC_add40perc->SetLineColor(7);
    h_Nstrange_PHG4_add40perc->SetLineColor(3);
    h_Nstrange_HepMC_add100perc->SetLineColor(5);
    h_Nstrange_PHG4_add100perc->SetLineColor(2);
    h_Nstrange_HepMC_ori->GetYaxis()->SetRangeUser(0, maxval * 1.8);
    h_Nstrange_HepMC_ori->GetYaxis()->SetTitleOffset(1.6);
    h_Nstrange_HepMC_ori->Draw("L");
    h_Nstrange_PHG4_ori->Draw("L same");
    h_Nstrange_HepMC_add40perc->Draw("L same");
    h_Nstrange_PHG4_add40perc->Draw("L same");
    h_Nstrange_HepMC_add100perc->Draw("L same");
    h_Nstrange_PHG4_add100perc->Draw("L same");
    TLegend *leg = new TLegend(0.2, 0.67, 0.4, 0.88);
    leg->AddEntry(h_Nstrange_HepMC_ori, "HepMC Particle - original", "l");
    leg->AddEntry(h_Nstrange_PHG4_ori, "PHG4Particle - original", "l");
    leg->AddEntry(h_Nstrange_HepMC_add40perc, "HepMC Particle - add 40% strange particles", "l");
    leg->AddEntry(h_Nstrange_PHG4_add40perc, "PHG4Particle - add 40% strange particles", "l");
    leg->AddEntry(h_Nstrange_HepMC_add100perc, "HepMC Particle - add 100% strange particles", "l");
    leg->AddEntry(h_Nstrange_PHG4_add100perc, "PHG4Particle - add 100% strange particles", "l");
    leg->SetTextSize(0.035);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->Draw();
    c1->SaveAs("validation_NstrangeEvt.png");
    c1->SaveAs("validation_NstrangeEvt.pdf");

    h_ori_PHG4_Pt->Scale(1.0 / h_ori_PHG4_Pt->Integral(-1, -1));
    h_add40perc_PHG4_Pt->Scale(1.0 / h_add40perc_PHG4_Pt->Integral(-1, -1));
    h_add100perc_PHG4_Pt->Scale(1.0 / h_add100perc_PHG4_Pt->Integral(-1, -1));
    float maxval_PHG4_Pt = std::max({h_ori_PHG4_Pt->GetMaximum(), h_add40perc_PHG4_Pt->GetMaximum(), h_add100perc_PHG4_Pt->GetMaximum()});
    float minval_PHG4_Pt = std::min({h_ori_PHG4_Pt->GetMinimum(0), h_add40perc_PHG4_Pt->GetMinimum(0), h_add100perc_PHG4_Pt->GetMinimum(0)});
    c1->Clear();
    c1->cd();
    c1->SetLogy(1);
    h_ori_PHG4_Pt->SetLineColor(kBlack);
    h_add40perc_PHG4_Pt->SetLineColor(4);
    h_add100perc_PHG4_Pt->SetLineColor(2);
    h_ori_PHG4_Pt->GetYaxis()->SetRangeUser(minval_PHG4_Pt*0.7, maxval_PHG4_Pt*1.5);
    h_ori_PHG4_Pt->Draw("hist");
    h_add40perc_PHG4_Pt->Draw("hist same");
    h_add100perc_PHG4_Pt->Draw("hist same");
    TLegend *leg1_pt = new TLegend(0.45, 0.73, 0.8, 0.88);
    leg1_pt->AddEntry(h_ori_PHG4_Pt.GetPtr(), "Original", "l");
    leg1_pt->AddEntry(h_add40perc_PHG4_Pt.GetPtr(), "Add 40% strange particles", "l");
    leg1_pt->AddEntry(h_add100perc_PHG4_Pt.GetPtr(), "Add 100% strange particles", "l");
    leg1_pt->SetTextSize(0.04);
    leg1_pt->SetBorderSize(0);
    leg1_pt->SetFillStyle(0);
    leg1_pt->Draw();
    c1->SaveAs("validation_PHG4_Pt.png");
    c1->SaveAs("validation_PHG4_Pt.pdf");

    h_ori_PHG4_Eta->Scale(1.0 / h_ori_PHG4_Eta->Integral());
    h_add40perc_PHG4_Eta->Scale(1.0 / h_add40perc_PHG4_Eta->Integral());
    h_add100perc_PHG4_Eta->Scale(1.0 / h_add100perc_PHG4_Eta->Integral());
    float maxval_PHG4_Eta = std::max({h_ori_PHG4_Eta->GetMaximum(), h_add40perc_PHG4_Eta->GetMaximum(), h_add100perc_PHG4_Eta->GetMaximum()});
    c1->Clear();
    c1->cd();
    c1->SetLogy(0);
    h_ori_PHG4_Eta->SetLineColor(kBlack);
    h_add40perc_PHG4_Eta->SetLineColor(4);
    h_add100perc_PHG4_Eta->SetLineColor(2);
    h_ori_PHG4_Eta->GetYaxis()->SetRangeUser(0, maxval_PHG4_Eta*1.5);
    h_ori_PHG4_Eta->GetYaxis()->SetTitleOffset(1.6);
    h_ori_PHG4_Eta->Draw("hist");
    h_add40perc_PHG4_Eta->Draw("ep same");
    h_add100perc_PHG4_Eta->Draw("hist same");
    TLegend *leg1_eta = new TLegend(0.45, 0.73, 0.8, 0.88);
    leg1_eta->AddEntry(h_ori_PHG4_Eta.GetPtr(), "Original", "l");
    leg1_eta->AddEntry(h_add40perc_PHG4_Eta.GetPtr(), "Add 40% strange particles", "l");
    leg1_eta->AddEntry(h_add100perc_PHG4_Eta.GetPtr(), "Add 100% strange particles", "l");
    leg1_eta->SetTextSize(0.04);
    leg1_eta->SetBorderSize(0);
    leg1_eta->SetFillStyle(0);
    leg1_eta->Draw();
    c1->SaveAs("validation_PHG4_Eta.png");
    c1->SaveAs("validation_PHG4_Eta.pdf");

    c1->Clear();
    c1->cd();
    h_NstrangeRatio_PHG4_add40perc->SetLineColor(3);
    h_NstrangeRatio_PHG4_add100perc->SetLineColor(2);
    h_NstrangeRatio_PHG4_add40perc->GetYaxis()->SetRangeUser(0, 5);
    h_NstrangeRatio_PHG4_add40perc->Draw("L");
    h_NstrangeRatio_PHG4_add100perc->Draw("L same");
    TLegend *leg2 = new TLegend(0.2, 0.7, 0.4, 0.9);
    leg2->AddEntry(h_NstrangeRatio_PHG4_add40perc, "PHG4Particle - add 40% strange particles", "l");
    leg2->AddEntry(h_NstrangeRatio_PHG4_add100perc, "PHG4Particle - add 100% strange particles", "l");
    leg2->SetTextSize(0.035);
    leg2->SetBorderSize(0);
    leg2->SetFillStyle(0);
    leg2->Draw();
    c1->SaveAs("validation_NstrangeRatioEvt.png");
    c1->SaveAs("validation_NstrangeRatioEvt.pdf");
}
