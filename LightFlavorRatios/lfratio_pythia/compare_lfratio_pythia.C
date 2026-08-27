#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TSystem.h"

#include <algorithm>
#include <iostream>

void compare_lfratio_pythia()
{
    const char *output_file_pattern = "/sphenix/tg/tg01/hf/hjheng/HF-analysis/simulation/Pythia_ppMinBias/lfratio/lfratio_hist_%d.root";
    const int n_files = 5000;

    const int n_regions = 3;
    const char *region_labels[n_regions] = {"|y| #leq 0.5", "0.5 < |y| #leq 1.0", "|y| > 1.0"};
    const char *kshort_names[n_regions] = {"h_prompt_kshort_pt_y_le_0p5", "h_prompt_kshort_pt_y_0p5_1p0", "h_prompt_kshort_pt_y_gt_1p0"};
    const char *lambda_names[n_regions] = {"h_prompt_lambda_pt_y_le_0p5", "h_prompt_lambda_pt_y_0p5_1p0", "h_prompt_lambda_pt_y_gt_1p0"};

    TH1D *sum_kshort[n_regions] = {nullptr, nullptr, nullptr};
    TH1D *sum_lambda[n_regions] = {nullptr, nullptr, nullptr};

    int n_files_used = 0;
    for (int ifile = 0; ifile < n_files; ++ifile)
    {
        const TString file_name = Form(output_file_pattern, ifile);
        if (gSystem->AccessPathName(file_name))
            continue;

        TFile *input = TFile::Open(file_name, "READ");
        if (!input || input->IsZombie())
        {
            delete input;
            continue;
        }

        for (int iregion = 0; iregion < n_regions; ++iregion)
        {
            TH1D *kshort = static_cast<TH1D *>(input->Get(kshort_names[iregion]));
            TH1D *lambda = static_cast<TH1D *>(input->Get(lambda_names[iregion]));
            if (!kshort || !lambda)
                continue;

            if (!sum_kshort[iregion])
            {
                sum_kshort[iregion] = static_cast<TH1D *>(kshort->Clone(Form("sum_%s", kshort_names[iregion])));
                sum_lambda[iregion] = static_cast<TH1D *>(lambda->Clone(Form("sum_%s", lambda_names[iregion])));
                sum_kshort[iregion]->SetDirectory(nullptr);
                sum_lambda[iregion]->SetDirectory(nullptr);
                sum_kshort[iregion]->Sumw2();
                sum_lambda[iregion]->Sumw2();
            }
            else
            {
                sum_kshort[iregion]->Add(kshort);
                sum_lambda[iregion]->Add(lambda);
            }
        }

        ++n_files_used;
        input->Close();
        delete input;
    }

    TH1D *ratio[n_regions] = {nullptr, nullptr, nullptr};
    const int colors[n_regions] = {kBlack, kRed + 1, kBlue + 1};
    double ymax = 0.0;

    for (int iregion = 0; iregion < n_regions; ++iregion)
    {
        if (!sum_kshort[iregion] || !sum_lambda[iregion])
        {
            std::cerr << "No summed histogram for " << region_labels[iregion] << std::endl;
            return;
        }

        ratio[iregion] = static_cast<TH1D *>(sum_lambda[iregion]->Clone(Form("h_prompt_lfratio_yregion%d", iregion)));
        ratio[iregion]->SetDirectory(nullptr);
        ratio[iregion]->SetTitle(";p_{T} [GeV];(#Lambda+#bar{#Lambda})/(2K^{0}_{S})");
        ratio[iregion]->Divide(sum_lambda[iregion], sum_kshort[iregion], 1.0, 2.0);
        ratio[iregion]->SetLineColor(colors[iregion]);
        ratio[iregion]->SetLineWidth(2);
        ymax = std::max(ymax, ratio[iregion]->GetMaximum());
    }

    TGraph *ratio_graph[n_regions] = {nullptr, nullptr, nullptr};
    for (int iregion = 0; iregion < n_regions; ++iregion)
    {
        ratio_graph[iregion] = new TGraph();
        ratio_graph[iregion]->SetName(Form("g_prompt_lfratio_yregion%d", iregion));
        ratio_graph[iregion]->SetLineColor(colors[iregion]);
        ratio_graph[iregion]->SetLineWidth(2);

        for (int ibin = 1; ibin <= ratio[iregion]->GetNbinsX(); ++ibin)
        {
            if (sum_kshort[iregion]->GetBinContent(ibin) <= 0.0)
                continue;

            const int point = ratio_graph[iregion]->GetN();
            ratio_graph[iregion]->SetPoint(point, ratio[iregion]->GetBinCenter(ibin), ratio[iregion]->GetBinContent(ibin));
        }
    }

    gStyle->SetOptStat(0);

    TCanvas *canvas = new TCanvas("c_prompt_lfratio_y_comparison", "Prompt ratio by rapidity", 800, 700);
    canvas->SetLeftMargin(0.15);
    // canvas->SetBottomMargin(0.12);

    ratio[0]->SetMinimum(0.0);
    // ratio[0]->SetMaximum(ymax > 0.0 ? 1.25 * ymax : 1.0);
    ratio[0]->SetMaximum(1.0);
    ratio[0]->Draw("AXIS");
    for (int iregion = 0; iregion < n_regions; ++iregion)
        ratio_graph[iregion]->Draw("L SAME");

    TLegend *legend = new TLegend(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.25, gPad->GetLeftMargin() + 0.3, 1 - gPad->GetTopMargin() - 0.05);
    legend->SetHeader("Pythia 8 (Detroit tune, pp #sqrt{s} = 200 GeV)");
    legend->SetTextSize(0.04);
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    for (int iregion = 0; iregion < n_regions; ++iregion)
        legend->AddEntry(ratio_graph[iregion], region_labels[iregion], "l");
    legend->Draw();

    canvas->SaveAs("compare_lfratio_pythia.pdf");
    canvas->SaveAs("compare_lfratio_pythia.png");

    std::cout << "Read " << n_files_used << " files" << std::endl;
}
