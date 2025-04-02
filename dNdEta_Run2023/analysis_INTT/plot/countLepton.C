float TickSize = 0.03;
float AxisTitleSize = 0.05;
float AxisLabelSize = 0.045;
float LeftMargin = 0.15;
float RightMargin = 0.05;
float TopMargin = 0.08;
float BottomMargin = 0.13;
float textscale = 2.6;

void countLepton()
{
    // directory to save the output plots; if the directory does not exist, create it using
    std::string plotdir = "./countLepton";
    system(Form("mkdir -p %s", plotdir.c_str()));

    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("minitree", "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/test.root");

    // define 2 new columns, one for the number of leptons and one for the number of total charged particles
    // -> the former counts the number of elements in the GenHadron_PID vector with a PID of 11, -11, 13, or -13
    // -> the latter counts the total number of elements in the GenHadron_PID vector
    auto df_with_lepton_count = df.Define("Nleptons",
                                          [](const std::vector<int> &GenHadron_PID, const std::vector<float> &GenHadron_eta)
                                          {
                                              int Nleptons = 0;
                                              for (size_t i = 0; i < GenHadron_PID.size(); i++)
                                              {
                                                  if ((abs(GenHadron_PID[i]) == 11 || abs(GenHadron_PID[i]) == 13) && fabs(GenHadron_eta[i]) < 1.5)
                                                  {
                                                      Nleptons++;
                                                  }
                                              }
                                              return Nleptons;
                                          },
                                          {"GenHadron_PID", "GenHadron_eta"})
                                    .Define("Ncharged", [](const std::vector<int> &GenHadron_PID) { return static_cast<int>(GenHadron_PID.size()); }, {"GenHadron_PID"})
                                    .Define("NStrange",
                                            [](const std::vector<int> &PrimaryG4P_PID, const std::vector<float> &PrimaryG4P_eta)
                                            {
                                                int NStrange = 0;
                                                for (size_t i = 0; i < PrimaryG4P_PID.size(); i++)
                                                {
                                                    if ((PrimaryG4P_PID[i] == 310 || abs(PrimaryG4P_PID[i]) == 3122) && fabs(PrimaryG4P_eta[i]) < 1.5)
                                                    {
                                                        NStrange++;
                                                    }
                                                }
                                                return NStrange;
                                            },
                                            {"PrimaryG4P_PID", "PrimaryG4P_eta"});

    // get the vector of Nleptons and Ncharged
    auto v_NLeptons = df_with_lepton_count.Take<int>("Nleptons");
    auto v_NCharged = df_with_lepton_count.Take<int>("Ncharged");
    auto v_NStrange = df_with_lepton_count.Take<int>("NStrange");
    // create 3 histograms, one for the number of leptons, one for the number of charged particles, and one for the ratio of the two. All these three are as a function of the event index
    auto h_NLeptons = new TH1F("h_NLeptons", "h_NLeptons;Event index;Nleptons", v_NLeptons->size(), 0, v_NLeptons->size());
    auto h_NStrange = new TH1F("h_NStrange", "h_NStrange;Event index;Nstrange", v_NLeptons->size(), 0, v_NLeptons->size());
    auto h_NCharged = new TH1F("h_NCharged", "h_NCharged;Event index;Ncharged", v_NLeptons->size(), 0, v_NLeptons->size());
    auto h_NLeptons_ratio = new TH1F("h_NLeptons_ratio", "h_NLeptons_ratio;Event index;Nleptons/Ncharged", v_NLeptons->size(), 0, v_NLeptons->size());
    auto h_NStrange_ratio = new TH1F("h_NStrange_ratio", "h_NStrange_ratio;Event index;Nstrange/Ncharged", v_NLeptons->size(), 0, v_NLeptons->size());
    float avg_lepton = 0, avg_strange = 0;
    for (size_t i = 0; i < v_NLeptons->size(); i++)
    {
        h_NLeptons->SetBinContent(i + 1, (*v_NLeptons)[i]);
        h_NStrange->SetBinContent(i + 1, (*v_NStrange)[i]);
        h_NCharged->SetBinContent(i + 1, (*v_NCharged)[i]);
        h_NStrange_ratio->SetBinContent(i + 1, (float)(*v_NStrange)[i] / (*v_NCharged)[i] * 100.);
        h_NLeptons_ratio->SetBinContent(i + 1, (float)(*v_NLeptons)[i] / (*v_NCharged)[i] * 100.);
        avg_lepton += (float)(*v_NLeptons)[i] / (*v_NCharged)[i] * 100.;
        avg_strange += (float)(*v_NStrange)[i] / (*v_NCharged)[i] * 100.;
    }
    avg_lepton /= v_NLeptons->size();
    avg_strange /= v_NStrange->size();

    TCanvas *c = new TCanvas("c", "c", 800, 700);
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1);
    pad1->SetBottomMargin(0);
    pad1->SetTopMargin(0.2);
    pad1->SetRightMargin(RightMargin);
    pad1->Draw();
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.3);
    pad2->SetRightMargin(RightMargin);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.33);
    pad2->Draw();
    pad1->cd();
    pad1->SetLogy(1);
    h_NCharged->SetLineColor(kBlack);
    h_NCharged->SetLineWidth(2);
    h_NCharged->GetYaxis()->SetTitle("Counts");
    h_NCharged->GetYaxis()->SetTitleOffset(1.4);
    h_NCharged->GetYaxis()->SetRangeUser(h_NLeptons->GetMinimum(0) * 0.3, h_NCharged->GetMaximum() * 1.2);
    h_NCharged->GetYaxis()->SetTitleSize(AxisTitleSize);
    h_NCharged->GetYaxis()->SetLabelSize(AxisLabelSize);
    h_NCharged->GetXaxis()->SetTitleSize(0);
    h_NCharged->GetXaxis()->SetLabelSize(0);
    h_NCharged->Draw();
    h_NStrange->SetLineColor(38);
    h_NStrange->SetLineWidth(2);
    h_NStrange->Draw("same");
    h_NLeptons->SetLineColor(46);
    h_NLeptons->SetLineWidth(2);
    h_NLeptons->Draw("same");
    TLegend *leg = new TLegend(pad1->GetLeftMargin(),           //
                               1 - pad1->GetTopMargin() + 0.01, //
                               pad1->GetLeftMargin() + 0.25,    //
                               0.98);
                               
    leg->AddEntry(h_NCharged, "Number of charged particles", "l");
    leg->AddEntry(h_NStrange, "Number of strange particle (K_{s}^{0}, #Lambda), |#eta|<1.5", "l");
    leg->AddEntry(h_NLeptons, "Number of leptons (e, #mu), |#eta|<1.5", "l");
    leg->SetBorderSize(0);
    leg->Draw();
    pad2->cd();
    // pad2->SetLogy(1);
    float ratiomax = h_NStrange_ratio->GetMaximum();
    h_NStrange_ratio->SetLineColor(38);
    h_NStrange_ratio->SetLineWidth(2);
    h_NStrange_ratio->GetYaxis()->SetTitle("Fraction (%)");
    h_NStrange_ratio->GetYaxis()->SetTitleOffset(0.5);
    h_NStrange_ratio->GetYaxis()->SetRangeUser(0, ratiomax * 1.05);
    h_NStrange_ratio->GetYaxis()->SetNdivisions(505);
    h_NStrange_ratio->GetYaxis()->SetTitleSize(AxisTitleSize * textscale);
    h_NStrange_ratio->GetYaxis()->SetLabelSize(AxisLabelSize * textscale);
    h_NStrange_ratio->GetXaxis()->SetTitleSize(AxisTitleSize * textscale);
    h_NStrange_ratio->GetXaxis()->SetLabelSize(AxisLabelSize * textscale);
    h_NStrange_ratio->GetXaxis()->SetTitle("Event index");
    h_NStrange_ratio->GetXaxis()->SetTitleOffset(1.2);
    h_NStrange_ratio->Draw("l");
    h_NLeptons_ratio->SetLineColor(46);
    h_NLeptons_ratio->SetLineWidth(2);
    h_NLeptons_ratio->Draw("l same");
    TLine *l_lepton = new TLine(0, avg_lepton, v_NLeptons->size(), avg_lepton);
    l_lepton->SetLineColor(46);
    l_lepton->SetLineStyle(2);
    l_lepton->Draw("same");
    TLine *l_strange = new TLine(0, avg_strange, v_NLeptons->size(), avg_strange);
    l_strange->SetLineColor(38);
    l_strange->SetLineStyle(2);
    l_strange->Draw("same");
    TLatex *l_avg = new TLatex();
    l_avg->SetTextAlign(kHAlignLeft + kVAlignTop);
    l_avg->SetTextSize(AxisLabelSize * textscale);
    l_avg->DrawLatex(1, ratiomax * 1.05, Form("Avg. lepton fraction=%.3g%%; Avg. strange fraction=%.3g%%", avg_lepton, avg_strange));
    c->SaveAs(Form("%s/countLepton.png", plotdir.c_str()));
    c->SaveAs(Form("%s/countLepton.pdf", plotdir.c_str()));
}