#include <iostream>
#include <string>
#include <regex>

void generator_dNdEtaNpart2(std::string filepath = "/sphenix/tg/tg01/hf/hjheng/ppg02/dst/Sim_HIJING_MDC2_ana472_20250307/")
{
    std::string plotdir = "./generator_dNdEtaNpart2";
    system(Form("mkdir -p %s", plotdir.c_str()));

    std::regex pattern(R"(/Sim_([^/_]+))"); // Match "Sim_" followed by the generator name
    std::smatch match;

    if (!std::regex_search(filepath, match, pattern))
    {
        std::cerr << "Generator name not found in the file path" << std::endl;
        return;
    }

    std::string generator = match[1].str();

    std::cout << "Generator: " << generator << std::endl;

    // Set a timer to measure the time it takes to run the analysis
    TStopwatch timer;
    timer.Start();

    ROOT::EnableImplicitMT();
    std::vector<std::string> filelist;
    for (int i = 0; i < 5000; i++)
    {
        std::string fname = filepath + "ntuple_" + std::string(TString::Format("%05d", i).Data()) + ".root";

        // Check if file exists and is valid
        TFile *f = TFile::Open(fname.c_str());
        if (!f || f->IsZombie() || f->GetSize() <= 0)
        {
            std::cout << "Skipping invalid or empty file: " << fname << std::endl;
            if (f)
                f->Close();
            continue;
        }

        // Check if file contains the required tree
        if (!f->GetListOfKeys()->Contains("EventTree"))
        {
            std::cout << "Skipping file without EventTree: " << fname << std::endl;
            f->Close();
            continue;
        }

        f->Close();
        std::cout << "Adding file: " << fname << std::endl;
        filelist.push_back(fname);
    }
    ROOT::RDataFrame df("EventTree", filelist);

    // first, filter the events with is_min_bias==1
    auto df_minbias = df.Filter("is_min_bias==1");
    // get number of minimum bias events
    int Nminbias = df_minbias.Count().GetValue();

    // Add a column to calculate the average number of PrimaryG4P_Eta with PrimaryG4P_isChargeHadron==1 within |eta|<=0.3
    auto df_truthdNdEta = df_minbias
                              .Define("truthdNdEta_etaleq0p3",
                                      [](const std::vector<float> &PrimaryG4P_Eta, const std::vector<bool> &PrimaryG4P_isChargeHadron)
                                      {
                                          double N = 0;
                                          for (size_t i = 0; i < PrimaryG4P_Eta.size(); i++)
                                          {
                                              if (PrimaryG4P_isChargeHadron[i] == true && fabs(PrimaryG4P_Eta[i]) <= 0.3)
                                              {
                                                  N++;
                                              }
                                          }

                                          return N / 0.6;
                                      },
                                      {"PrimaryG4P_Eta", "PrimaryG4P_isChargeHadron"})
                              .Define("truthdNdEta_etaleq0p3overNpart2", [](double N, int Npart) { return static_cast<double>(static_cast<double>(N) / (0.5 * static_cast<double>(Npart))); }, {"truthdNdEta_etaleq0p3", "npart"});

    // 2D histogram for truth (dN/dEta)/(0.5*npart) vs npart/2
    auto h_truthdNdEta = df_truthdNdEta.Histo2D<int, double>({"h_truthdNdEta", "h_truthdNdEta", 40, 0, 400, 30, 0, 6}, "npart", "truthdNdEta_etaleq0p3overNpart2");

    TCanvas *c = new TCanvas("c", "c", 800, 700);
    gPad->SetRightMargin(0.22);
    gPad->SetTopMargin(0.07);
    gPad->SetLeftMargin(0.17);
    c->cd();
    h_truthdNdEta->GetXaxis()->SetTitle("N_{part}");
    h_truthdNdEta->GetYaxis()->SetTitle("(dN_{ch}/d#eta)/(0.5N_{part})|_{|#eta|#leq0.3}");
    h_truthdNdEta->GetZaxis()->SetTitle("Entries");
    h_truthdNdEta->GetZaxis()->SetTitleOffset(1.7);
    h_truthdNdEta->Draw("colz");
    // profile
    TProfile *p_truthdNdEta = h_truthdNdEta->ProfileX();
    p_truthdNdEta->SetMarkerSize(0);
    p_truthdNdEta->SetLineColor(kRed);
    p_truthdNdEta->DrawCopy("hist l same");
    // draw the text for generator
    TLatex *t = new TLatex();
    t->SetTextSize(0.04);
    t->SetTextAlign(kHAlignRight + kVAlignCenter);
    t->DrawLatexNDC(0.75, 0.2, generator.c_str());
    c->SaveAs(Form("%s/h_truthdNdEta_%s.png", plotdir.c_str(), generator.c_str()));
    c->SaveAs(Form("%s/h_truthdNdEta_%s.pdf", plotdir.c_str(), generator.c_str()));

    // Set up the TGraph for the profile
    TGraph *g_truthdNdEta = new TGraph(p_truthdNdEta->GetNbinsX());
    for (int i = 0; i < p_truthdNdEta->GetNbinsX(); i++)
    {
        g_truthdNdEta->SetPoint(i, p_truthdNdEta->GetBinCenter(i + 1), p_truthdNdEta->GetBinContent(i + 1));
    }
    g_truthdNdEta->SetName("g_truthdNdEta");
    g_truthdNdEta->SetTitle("g_truthdNdEta");

    // Save to file
    TFile *fout = new TFile(Form("%s/truthdNdEta_%s.root", plotdir.c_str(), generator.c_str()), "RECREATE");
    h_truthdNdEta->Write();
    p_truthdNdEta->Write();
    g_truthdNdEta->Write();
    fout->Close();
}