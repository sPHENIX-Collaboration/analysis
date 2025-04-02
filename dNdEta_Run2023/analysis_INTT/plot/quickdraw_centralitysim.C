#include "./plotutil.h"
float ytitleoffset = 1.6;

void quickdraw_centralitysim()
{
    std::string plotdir = "./quickcheck_centralitysim";
    system(Form("mkdir -p %s", plotdir.c_str()));

    // Set a timer to measure the time it takes to run the analysis
    TStopwatch timer;
    timer.Start();

    ROOT::EnableImplicitMT();
    std::vector<std::string> filelist;
    for (int i = 0; i < 5000; i++)
    {
        std::string fname = "/sphenix/tg/tg01/hf/hjheng/ppg02/dst/Sim_AMPT_MDC2_ana472_20250310/ntuple_" + std::string(TString::Format("%05d", i).Data()) + ".root";
        
        // Check if file exists and is valid
        TFile *f = TFile::Open(fname.c_str());
        if (!f || f->IsZombie() || f->GetSize() <= 0) {
            std::cout << "Skipping invalid or empty file: " << fname << std::endl;
            if (f) f->Close();
            continue;
        }
        
        // Check if file contains the required tree
        if (!f->GetListOfKeys()->Contains("EventTree")) {
            std::cout << "Skipping file without EventTree: " << fname << std::endl;
            f->Close();
            continue;
        }
        
        f->Close();
        std::cout << "Adding file: " << fname << std::endl;
        filelist.push_back(fname);
    }
    ROOT::RDataFrame df("EventTree", filelist);

    auto count_all = df.Count();
    std::cout << "Number of entries in the dataframe: " << *count_all << std::endl;

    // histogram for MBD centrality without cut
    auto hM_MBDcentrality_wocut = df.Histo1D({"hM_MBDcentrality_wocut", "hM_MBDcentrality_wocut", 102, -1.5, 100.5}, "MBD_centrality");
    auto hM_MBDcentrality_isMinBias = df.Filter("is_min_bias").Histo1D({"hM_MBDcentrality_isMinBias", "hM_MBDcentrality_isMinBias", 102, -1.5, 100.5}, "MBD_centrality");
    auto hM_Npart_isMinBias = df.Filter("is_min_bias").Histo1D({"hM_Npart_isMinBias", "hM_Npart_isMinBias", 401, -0.5, 400.5}, "npart");
    std::vector<float> centralitybins = {0, 3, 6, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100};
    // histogram for npart in all centrality intervals, for is_min_bias events
    std::vector<ROOT::RDF::RResultPtr<TH1D>> hM_Npart_centralities;
    for (size_t i = 0; i < centralitybins.size() - 1; i++)
    {
        auto hM = df.Filter(Form("is_min_bias && MBD_centrality >= %f && MBD_centrality < %f", centralitybins[i], centralitybins[i + 1])).Histo1D({Form("hM_Npart_centralities_%zu", i), Form("hM_Npart_centralities_%zu", i), 401, -0.5, 400.5}, "npart");
        hM_Npart_centralities.push_back(hM);
    }

    std::vector<float> meanNpart_centralities;
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
    hM_MBDcentrality_wocut->Draw("hist");
    hM_MBDcentrality_isMinBias->Draw("hist same");
    c->RedrawAxis();
    TLegend *l = new TLegend(gPad->GetLeftMargin(), 1 - gPad->GetTopMargin() + 0.02, 1 - gPad->GetRightMargin() - 0.45, 0.98);
    l->SetNColumns(1);
    l->AddEntry(hM_MBDcentrality_wocut.GetPtr(), "Without cut", "l");
    l->AddEntry(hM_MBDcentrality_isMinBias.GetPtr(), "Is min bias", "l");
    l->SetTextSize(0.035);
    l->SetBorderSize(0);
    l->SetFillStyle(0);
    l->Draw();
    c->SaveAs(Form("%s/MBDcentrality_selections.pdf", plotdir.c_str()));
    c->SaveAs(Form("%s/MBDcentrality_selections.png", plotdir.c_str()));

    c->Clear();
    gPad->SetTopMargin(0.35);
    gPad->Modified();
    gPad->Update();
    c->SetLogy();
    hM_Npart_isMinBias->GetXaxis()->SetTitle("Number of participants, N_{part}");
    hM_Npart_isMinBias->GetYaxis()->SetTitle("Counts");
    hM_Npart_isMinBias->GetYaxis()->SetTitleOffset(ytitleoffset);
    hM_Npart_isMinBias->SetLineColor(kBlack);
    hM_Npart_isMinBias->SetLineWidth(2);
    hM_Npart_isMinBias->Draw("hist");
    for (size_t i = 0; i < hM_Npart_centralities.size(); i++)
    {
        hM_Npart_centralities[i]->SetLineWidth(2);
        hM_Npart_centralities[i]->Draw("plc same");
    }
    hM_Npart_isMinBias->Draw("hist same");
    c->RedrawAxis();
    TLegend *leg = new TLegend(gPad->GetLeftMargin(), 1 - gPad->GetTopMargin() + 0.01, 1 - gPad->GetRightMargin(), 0.98);
    leg->SetNColumns(2);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.02);
    for (size_t i = 0; i < hM_Npart_centralities.size(); i++)
    {
        // get the mean of Npart from the histogram
        float meanNpart = hM_Npart_centralities[i]->GetMean();
        meanNpart_centralities.push_back(meanNpart);
        leg->AddEntry(hM_Npart_centralities[i].GetPtr(), Form("Centrality [%d-%d]%%, <N_{part}>=%.1f", (int)centralitybins[i], (int)centralitybins[i + 1], meanNpart), "l");
    }
    leg->Draw();
    c->SaveAs(Form("%s/Npart_centralities.pdf", plotdir.c_str()));
    c->SaveAs(Form("%s/Npart_centralities.png", plotdir.c_str()));

    // print out the centrality bins and the mean Npart in each bin in Latex table format
    std::cout << "Latex code for the table:\n";
    std::cout << "---------------------------------------------------------------------------\n";
    std::cout << "\\begin{table}[ht]\n";
    std::cout << "\\centering\n";
    std::cout << "\\begin{tabular}{c|c}\n";
    std::cout << "\\hline\n";
    std::cout << "Centrality interval [\\%] & $\\langle$\\npart$\\rangle$ \\\\\n";
    std::cout << "\\hline\n";

    for (size_t i = 0; i < centralitybins.size() - 1; ++i)
    {
        if (centralitybins[i] >= 70)
            continue;
        std::cout << std::fixed << std::setprecision(0) << centralitybins[i] << "-" << centralitybins[i + 1] << " & ";
        print_with_significant_digits(meanNpart_centralities[i]);
        std::cout << " \\\\\n";
    }

    std::cout << "\\hline\n";
    std::cout << "\\end{tabular}\n";
    std::cout << "\\caption{Centrality intervals and corresponding $\\langle$\\npart$\\rangle$ values.}\n";
    std::cout << "\\label{tab:centinterval_npart}\n";
    std::cout << "\\end{table}\n";
    std::cout << "---------------------------------------------------------------------------\n";

    // simple print out of the centrality bins and the mean Npart in each bin
    std::cout << "Centrality interval [%]: " << std::endl;
    for (size_t i = 0; i < centralitybins.size() - 1; ++i)
    {
        if (centralitybins[i] >= 70)
            continue;
        std::cout << std::fixed << std::setprecision(0) << centralitybins[i] << "-" << centralitybins[i + 1] << ", ";
    }
    std::cout << std::endl;
    std::cout << "Mean Npart: " << std::endl;
    for (size_t i = 0; i < meanNpart_centralities.size(); ++i)
    {
        if (centralitybins[i] >= 70)
            continue;
        print_with_significant_digits(meanNpart_centralities[i], 6);
        std::cout << ", ";
    }
    std::cout << std::endl;

    timer.Stop();
    timer.Print();
}