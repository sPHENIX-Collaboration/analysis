#include <TAxis.h>
#include <TBox.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TGraph.h>
#include <TH1.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TPad.h>
#include <TStyle.h>
#include <TSystem.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "plotUtils.h"

// Hyperparameter-scan summary from training/hpo/score.py:
//   <campaign>/hpo_summary.csv -> <outdir>/hpo_mass_term.{png,pdf}
// Top: mass term of every trial on the validation (odd) and training (even) halves, with the
// seed-to-seed spread of the nominal trials as a band. Bottom: validation - training.
// usage (fresh shell with ROOT):  root -l -b -q 'plotHPOSummary.C("<campaign>")'   (default outdir <campaign>/figures)

namespace
{
struct TrialStyle
{
    std::string name; // trial directory (training/hpo/trials.txt)
    std::string label;
};

struct HalfStyle
{
    std::string half; // score.py "half" column
    std::string label;
    int marker;
    int color;
};

// Band of mean +- standard deviation of the nominal (seed) trials across the whole x range
TBox *drawSeedBand(const std::vector<double> &values, const double xmax, const int color)
{
    double mean = 0.0;
    for (const double v : values)
        mean += v / values.size();
    double var = 0.0;
    for (const double v : values)
        var += (v - mean) * (v - mean) / (values.size() - 1);
    TBox *band = new TBox(0.0, mean - std::sqrt(var), xmax, mean + std::sqrt(var));
    band->SetFillColorAlpha(color, 0.35);
    band->SetLineWidth(0);
    band->Draw();
    TLine *line = new TLine(0.0, mean, xmax, mean);
    line->SetLineColor(color);
    line->SetLineStyle(2);
    line->SetLineWidth(2);
    line->Draw();
    return band;
}
} // namespace

void plotHPOSummary(std::string campaign = "/sphenix/tg/tg01/hf/hjheng/HF-analysis/NNScaleMap/hpo/fill79514to79516", std::string outdir = "")
{
    // one ordered list: x-axis order and labels of the trials (nominal: hidden 48, layers 3, lr 2e-3, 800 epochs, float32)
    const std::vector<TrialStyle> trials = {
        {"nominal_s17", "Nominal, seed 17"},
        {"nominal_s18", "Nominal, seed 18"},
        {"nominal_s19", "Nominal, seed 19"},
        {"nominal_s20", "Nominal, seed 20"},
        {"nominal_s21", "Nominal, seed 21"},
        {"hidden24", "Hidden width 24"},
        {"hidden96", "Hidden width 96"},
        {"layers2", "2 hidden layers"},
        {"layers4", "4 hidden layers"},
        {"lr1e-3", "lr = 1#times10^{-3}"},
        {"lr5e-3", "lr = 5#times10^{-3}"},
    };
    const std::vector<HalfStyle> halves = {
        {"odd", "Validation (odd half)", 20, TColor::GetColor("#0077BB")},
        {"even", "Training (even half)", 54, TColor::GetColor("#EE7733")}, // 54: open square with 2-pixel line (a 1-pixel line anti-aliases to grey in the PNG)
    };
    const int gapColor = kBlack;
    const int bandColor = kGray + 1;

    const int kTitleSize = 28;
    const int kLabelSize = 24;
    const int kTextSize = 24;

    if (outdir.empty())
        outdir = campaign + "/figures";
    gSystem->mkdir(outdir.c_str(), true);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    // ---- read hpo_summary.csv: mass term per (trial, half)
    const std::string csv = campaign + "/hpo_summary.csv";
    std::ifstream in(csv);
    if (!in)
    {
        std::cerr << "Could not open " << csv << std::endl;
        return;
    }
    std::map<std::string, std::map<std::string, double>> massTerm; // [trial][half]
    {
        std::string line;
        std::getline(in, line);
        if (!line.empty() && line.back() == '\r') // the csv module writes \r\n line ends
            line.pop_back();
        std::vector<std::string> header;
        std::stringstream headerStream(line);
        for (std::string field; std::getline(headerStream, field, ',');)
            header.push_back(field);
        const int iTrial = std::find(header.begin(), header.end(), "trial") - header.begin();
        const int iHalf = std::find(header.begin(), header.end(), "half") - header.begin();
        const int iMass = std::find(header.begin(), header.end(), "mass_term") - header.begin();
        if (std::max({iTrial, iHalf, iMass}) >= static_cast<int>(header.size()))
        {
            std::cerr << csv << ": needs the columns trial, half, mass_term" << std::endl;
            return;
        }
        while (std::getline(in, line))
        {
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            std::vector<std::string> fields;
            std::stringstream lineStream(line);
            for (std::string field; std::getline(lineStream, field, ',');)
                fields.push_back(field);
            if (fields.size() == header.size())
                massTerm[fields[iTrial]][fields[iHalf]] = std::stod(fields[iMass]);
        }
    }

    std::vector<TrialStyle> present;
    for (const TrialStyle &trial : trials)
    {
        if (massTerm.count(trial.name) && massTerm[trial.name].count("odd") && massTerm[trial.name].count("even"))
            present.push_back(trial);
    }
    for (const auto &item : massTerm)
    {
        if (std::none_of(trials.begin(), trials.end(), [&](const TrialStyle &t) { return t.name == item.first; }))
            std::cerr << "Warning: trial " << item.first << " has no entry in the trial list and is not drawn" << std::endl;
    }
    if (present.empty())
    {
        std::cerr << "No trial of the list found in " << csv << std::endl;
        return;
    }
    const int nTrials = present.size();
    std::cout << "Read " << nTrials << " scored trials from " << csv << std::endl;

    // per half and gap: values in x order; nominal-seed values for the bands
    std::vector<TGraph *> halfGraphs;
    std::vector<std::vector<double>> nominalHalf(halves.size());
    TGraph *gapGraph = new TGraph();
    std::vector<double> nominalGap;
    int lastNominal = -1;
    for (std::size_t h = 0; h < halves.size(); ++h)
    {
        TGraph *graph = new TGraph();
        graph->SetMarkerStyle(halves[h].marker);
        graph->SetMarkerColor(halves[h].color);
        graph->SetLineColor(halves[h].color);
        graph->SetMarkerSize(1.8);
        halfGraphs.push_back(graph);
    }
    for (int i = 0; i < nTrials; ++i)
    {
        const bool nominal = present[i].name.rfind("nominal", 0) == 0;
        if (nominal)
            lastNominal = i;
        for (std::size_t h = 0; h < halves.size(); ++h)
        {
            const double value = massTerm[present[i].name][halves[h].half];
            halfGraphs[h]->SetPoint(i, i + 0.5, value);
            if (nominal)
                nominalHalf[h].push_back(value);
        }
        // in units of 10^-3 (stated on the axis): ROOT's automatic axis exponent would sit in the
        // zero top margin of the bottom pad and be cut off
        const double gap = 1.0e3 * (massTerm[present[i].name]["odd"] - massTerm[present[i].name]["even"]);
        gapGraph->SetPoint(i, i + 0.5, gap);
        if (nominal)
            nominalGap.push_back(gap);
    }
    gapGraph->SetMarkerStyle(33);
    gapGraph->SetMarkerSize(2.4);
    gapGraph->SetMarkerColor(gapColor);

    // ---- canvas: top pad (mass term) over bottom pad (gap); identical frame widths, pixel fonts
    const int left = 130, right = 30, top = 120, bottom = 230, frameW = 90 * std::max(nTrials, 10), frameTop = 420, frameBottom = 220;
    const int width = left + frameW + right;
    const int height = top + frameTop + frameBottom + bottom;
    TCanvas canvas("c_hpo_mass_term", "HPO mass term", width, height);
    canvas.SetCanvasSize(width, height);
    const double split = double(bottom + frameBottom) / height;
    TPad *padTop = new TPad("padTop", "", 0.0, split, 1.0, 1.0);
    TPad *padBottom = new TPad("padBottom", "", 0.0, 0.0, 1.0, split);
    padTop->SetMargin(double(left) / width, double(right) / width, 0.0, double(top) / (top + frameTop));
    padBottom->SetMargin(double(left) / width, double(right) / width, double(bottom) / (bottom + frameBottom), 0.0);
    for (TPad *pad : {padTop, padBottom})
    {
        pad->SetTicks(1, 1);
        pad->SetFillStyle(4000);
        canvas.cd();
        pad->Draw();
    }

    // y ranges include the points and the seed bands
    double ymin = 1.0e30, ymax = -1.0e30, gmin = 0.0, gmax = 0.0;
    for (int i = 0; i < nTrials; ++i)
    {
        for (const HalfStyle &half : halves)
        {
            ymin = std::min(ymin, massTerm[present[i].name][half.half]);
            ymax = std::max(ymax, massTerm[present[i].name][half.half]);
        }
        gmin = std::min(gmin, gapGraph->GetPointY(i));
        gmax = std::max(gmax, gapGraph->GetPointY(i));
    }
    const double ypad = 0.15 * (ymax - ymin);
    const double gpad = 0.2 * (gmax - gmin);

    TH1D *frames[2];
    for (int p = 0; p < 2; ++p)
    {
        frames[p] = new TH1D(Form("frame%d", p), "", nTrials, 0.0, nTrials);
        frames[p]->SetDirectory(nullptr);
        for (int i = 0; i < nTrials; ++i)
            frames[p]->GetXaxis()->SetBinLabel(i + 1, present[i].label.c_str());
        for (TAxis *axis : {frames[p]->GetXaxis(), frames[p]->GetYaxis()})
        {
            axis->SetTitleFont(43);
            axis->SetLabelFont(43);
            axis->SetTitleSize(kTitleSize);
            axis->SetLabelSize(kLabelSize);
        }
        frames[p]->GetYaxis()->SetTitleOffset(2.0);
        frames[p]->GetYaxis()->SetNdivisions(505);
        frames[p]->GetYaxis()->SetNoExponent();
        frames[p]->GetXaxis()->SetTickLength(0.0);
    }
    frames[0]->GetXaxis()->SetLabelSize(0);
    frames[0]->GetYaxis()->SetTitle("Mass term #Sigma#LT(#Deltam/#sigma_{0})^{2}#GT");
    frames[0]->SetMinimum(ymin - ypad);
    frames[0]->SetMaximum(ymax + ypad);
    frames[1]->GetYaxis()->SetTitle("Validation - training [10^{-3}]");
    frames[1]->GetXaxis()->LabelsOption("v");
    frames[1]->GetXaxis()->SetLabelOffset(0.02);
    frames[1]->SetMinimum(gmin - gpad);
    frames[1]->SetMaximum(gmax + gpad);

    padTop->cd();
    frames[0]->Draw("AXIS");
    TBox *band = nullptr;
    if (nominalHalf[0].size() > 1)
        band = drawSeedBand(nominalHalf[0], nTrials, bandColor);
    for (TGraph *graph : halfGraphs)
        graph->Draw("P same");
    padTop->RedrawAxis();

    padBottom->cd();
    frames[1]->Draw("AXIS");
    if (nominalGap.size() > 1)
        drawSeedBand(nominalGap, nTrials, bandColor);
    TLine *zero = new TLine(0.0, 0.0, nTrials, 0.0);
    zero->SetLineStyle(3);
    zero->Draw();
    gapGraph->Draw("P same");
    padBottom->RedrawAxis();

    // nominal seeds | hyperparameter variations
    if (lastNominal >= 0 && lastNominal < nTrials - 1)
    {
        TPad *pads[2] = {padTop, padBottom};
        for (int p = 0; p < 2; ++p)
        {
            pads[p]->cd();
            TLine *separator = new TLine(lastNominal + 1, frames[p]->GetMinimum(), lastNominal + 1, frames[p]->GetMaximum());
            separator->SetLineStyle(3);
            separator->SetLineColor(kGray + 2);
            separator->Draw();
        }
    }

    canvas.cd();
    TLatex text;
    text.SetNDC();
    text.SetTextFont(43);
    text.SetTextSize(kTextSize);
    text.DrawLatex(double(left) / width, 1 - 40.0 / height, "#it{#bf{sPHENIX}} Internal");
    text.DrawLatex(double(left) / width, 1 - 75.0 / height, "Stage 1, K^{0}_{S}+#Lambda(#bar{#Lambda}) mass pulls");

    // legend in the top margin, never on the data
    TLegend *legend = new TLegend(0.55, 1 - 115.0 / height, 1 - double(right) / width, 1 - 5.0 / height);
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->SetTextFont(43);
    legend->SetTextSize(kTextSize - 2);
    for (std::size_t h = 0; h < halves.size(); ++h)
        legend->AddEntry(halfGraphs[h], halves[h].label.c_str(), "p");
    if (band)
        legend->AddEntry(band, Form("Nominal seeds: mean#pm s.d. (%zu)", nominalHalf[0].size()), "f");
    legend->Draw();

    canvas.SaveAs((outdir + "/hpo_mass_term.png").c_str());
    canvas.SaveAs((outdir + "/hpo_mass_term.pdf").c_str());
    std::cout << "Saved HPO summary to " << outdir << std::endl;
}
