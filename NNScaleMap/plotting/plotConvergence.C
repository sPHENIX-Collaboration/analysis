#include <TAxis.h>
#include <TBox.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
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
#include <utility>
#include <vector>

#include "plotUtils.h"

// Training-procedure validation ("convergence") from training/convergence/compare.py:
//   <campaign>/convergence_vs_pt.csv, convergence_summary.csv ->
//     <outdir>/convergence_kappa_vs_pt.{png,pdf}   Adam float64 epoch doublings vs pT, with the reference seed spread
//     <outdir>/convergence_procedures.{png,pdf}    faster procedures vs the reference (top), float32 - float64 (bottom), vs pT
//     <outdir>/convergence_cost.{png,pdf}          agreement with the reference vs network passes per seed
// All kappa differences are RMS(Delta kappa)/sigma_stat; the reference is the seed mean of float64 Adam at the largest epoch count.
// usage (fresh shell with ROOT):  root -l -b -q 'plotConvergence.C("<campaign>")'   (default outdir <campaign>/figures)

namespace
{
struct MethodStyle
{
    std::string key; // compare.py method
    std::string label;
    int color;
    int marker;
};

// pixel (font 43) text sizes shared by every pad
const int kTitleSize = 28;
const int kLabelSize = 24;
const int kTextSize = 22;

// compare.py PT_CONSTRAINED: above it the map is an extrapolation
const double kPtConstrained = 2.0; // GeV

using CurveTable = std::map<std::string, std::vector<std::pair<double, double>>>; // comparison -> (pT, RMS)

// RMS(Delta kappa)/sigma_stat vs pT of one comparison of convergence_vs_pt.csv; nullptr when the campaign lacks it
TGraph *makeCurve(const CurveTable &table, const std::string &label, const int color, const int lineStyle, const int lineWidth)
{
    const auto it = table.find(label);
    if (it == table.end())
        return nullptr;
    TGraph *graph = new TGraph();
    for (const auto &point : it->second)
        graph->AddPoint(point.first, point.second);
    graph->SetLineColor(color);
    graph->SetLineStyle(lineStyle);
    graph->SetLineWidth(lineWidth);
    return graph;
}

// log-y range covering the graphs, with headroom for the in-pad labels
std::pair<double, double> logRange(const std::vector<TGraph *> &graphs)
{
    double ymin = 1.0e30;
    double ymax = 0.0;
    for (const TGraph *graph : graphs)
    {
        for (int i = 0; i < graph->GetN(); ++i)
        {
            if (graph->GetPointY(i) > 0)
                ymin = std::min(ymin, graph->GetPointY(i));
            const double errorHigh = graph->GetErrorYhigh(i); // -1 for a graph without errors
            ymax = std::max(ymax, graph->GetPointY(i) + std::max(errorHigh, 0.0));
        }
    }
    return {ymin / 2, ymax * 5};
}

// pixel fonts with identical absolute sizes in every pad; a hidden shared x axis has no labels or title
void styleFrame(TH1 *frame, const bool showX)
{
    for (TAxis *axis : {frame->GetXaxis(), frame->GetYaxis()})
    {
        axis->SetTitleFont(43);
        axis->SetLabelFont(43);
        axis->SetTitleSize(kTitleSize);
        axis->SetLabelSize(kLabelSize);
    }
    frame->GetXaxis()->SetTitleOffset(1.3);
    frame->GetYaxis()->SetTitleOffset(1.9);
    frame->GetXaxis()->SetMoreLogLabels();
    frame->GetXaxis()->SetNoExponent();
    frame->GetYaxis()->SetMoreLogLabels();
    frame->GetYaxis()->SetNoExponent();
    if (!showX)
    {
        frame->GetXaxis()->SetLabelSize(0);
        frame->GetXaxis()->SetTitle("");
    }
}

// pT pads: the extrapolation region (pT above kPtConstrained) shaded, and labelled when showLabel
void drawExtrapolation(const double ptMax, const double ymin, const double ymax, const bool showLabel)
{
    TBox *extrapolation = new TBox(kPtConstrained, ymin, ptMax, ymax);
    extrapolation->SetFillColorAlpha(kGray, 0.5);
    extrapolation->Draw();
    if (!showLabel)
        return;
    TLatex text;
    text.SetTextFont(43);
    text.SetTextSize(kTextSize - 2);
    text.SetTextColor(kGray + 2);
    text.SetTextAlign(13);
    // two lines 8 and 32 pixels below the frame top, whatever the pad height and log-y span
    const double framePixels = gPad->GetWh() * gPad->GetHNDC() * (1 - gPad->GetTopMargin() - gPad->GetBottomMargin());
    const double decadesPerPixel = std::log10(ymax / ymin) / framePixels;
    text.DrawLatex(kPtConstrained * 1.04, ymax / std::pow(10, 8 * decadesPerPixel), "Extrapolation");
    text.DrawLatex(kPtConstrained * 1.04, ymax / std::pow(10, 32 * decadesPerPixel), "region");
}

TLegend *makeLegend(const double x1, const double y1, const double x2, const double y2)
{
    TLegend *legend = new TLegend(x1, y1, x2, y2);
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->SetTextFont(43);
    legend->SetTextSize(kTextSize);
    return legend;
}
} // namespace

void plotConvergence(std::string campaign, std::string outdir = "")
{
    // one ordered list: legend order, colours and markers of the training procedures (open markers: float32)
    const std::vector<MethodStyle> methods = {
        {"adam64", "Adam, float64", TColor::GetColor("#0077BB"), 20},
        {"adam32", "Adam, float32", TColor::GetColor("#33BBEE"), 24},
        {"cosine64", "Adam + cosine decay, float64", TColor::GetColor("#EE7733"), 21},
        {"cosine32", "Adam + cosine decay, float32", TColor::GetColor("#CCBB44"), 25},
        {"lbfgs64", "Full-batch L-BFGS, float64", TColor::GetColor("#CC3311"), 22},
        {"lbfgs32", "Full-batch L-BFGS, float32", TColor::GetColor("#EE3377"), 26},
    };
    const std::vector<int> epochStyles = {1, 7, 2, 3};                                                  // line style by epoch rank within a method / by seed rank
    const std::vector<std::string> doublingColors = {"#88CCEE", "#44AA99", "#117733", "#332288", "#AA4499"}; // successive Adam doublings
    const std::string sphenixLabel = "#it{#bf{sPHENIX}} Internal";
    const std::string yTitle = "RMS(#Delta#kappa)/#sigma_{#kappa}^{stat}";
    const double ptMin = 0.1;
    const double ptMax = 5.5; // GeV, a little above the last grid point so its label stays inside the pad

    if (outdir.empty())
        outdir = campaign + "/figures";
    gSystem->mkdir(outdir.c_str(), true);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    // ---- read the two CSVs (string columns, so a plain ifstream parse)
    CurveTable curves;
    std::vector<std::map<std::string, std::string>> trials;
    for (const std::string name : {"convergence_vs_pt.csv", "convergence_summary.csv"})
    {
        std::ifstream in(campaign + "/" + name);
        if (!in)
        {
            std::cerr << "Could not open " << campaign << "/" << name << std::endl;
            return;
        }
        std::string line;
        std::getline(in, line);
        if (!line.empty() && line.back() == '\r') // the csv module writes \r\n line ends
            line.pop_back();
        std::vector<std::string> header;
        std::stringstream headerStream(line);
        for (std::string field; std::getline(headerStream, field, ',');)
            header.push_back(field);
        while (std::getline(in, line))
        {
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            std::map<std::string, std::string> row;
            std::stringstream lineStream(line);
            std::size_t i = 0;
            for (std::string field; std::getline(lineStream, field, ',') && i < header.size(); ++i)
                row[header[i]] = field;
            if (i != header.size())
                continue;
            if (name == "convergence_vs_pt.csv")
                curves[row["comparison"]].push_back({std::stod(row["pT"]), std::stod(row["rms_dkappa_over_sigma_stat"])});
            else
                trials.push_back(row);
        }
    }

    // epochs and seeds per method, and the reference (largest float64 Adam epoch count)
    std::map<std::string, std::vector<int>> epochsOf;
    std::vector<int> seeds;
    for (const auto &row : trials)
    {
        std::vector<int> &epochs = epochsOf[row.at("method")];
        if (std::find(epochs.begin(), epochs.end(), std::stoi(row.at("epochs"))) == epochs.end())
            epochs.push_back(std::stoi(row.at("epochs")));
        if (std::find(seeds.begin(), seeds.end(), std::stoi(row.at("seed"))) == seeds.end())
            seeds.push_back(std::stoi(row.at("seed")));
        if (std::none_of(methods.begin(), methods.end(), [&](const MethodStyle &m) { return m.key == row.at("method"); }))
            std::cerr << "Warning: method " << row.at("method") << " has no entry in the method list and is not drawn" << std::endl;
    }
    for (auto &item : epochsOf)
        std::sort(item.second.begin(), item.second.end());
    std::sort(seeds.begin(), seeds.end());
    if (epochsOf["adam64"].empty())
    {
        std::cerr << "No float64 Adam trial (the reference) in " << campaign << std::endl;
        return;
    }
    const int refEpochs = epochsOf["adam64"].back();
    const std::string refLabel = Form("Reference: Adam, float64, %d epochs, mean of 2 initializations", refEpochs);
    std::cout << "Read " << trials.size() << " trials and " << curves.size() << " comparisons; reference: float64 Adam " << refEpochs << " epochs" << std::endl;

    TGraph *refSpread = makeCurve(curves, Form("seed spread adam64 %d", refEpochs), kBlack, 7, 3);

    // single-pad canvases: frame and margins in pixels, legend in the right margin
    const int left = 130, frameW = 640, right = 560, top = 70, bottom = 95, frameH = 460;
    const int width = left + frameW + right;

    // ---- 1. Adam float64 doublings vs pT
    {
        const int height = top + frameH + bottom;
        TCanvas canvas("c_convergence_kappa_vs_pt", "", width, height);
        canvas.SetCanvasSize(width, height);
        canvas.SetMargin(double(left) / width, double(right) / width, double(bottom) / height, double(top) / height);
        canvas.SetLogx();
        canvas.SetLogy();
        canvas.SetTicks(1, 1);

        std::vector<TGraph *> drawn;
        std::vector<std::string> labels;
        const std::vector<int> &adam = epochsOf["adam64"];
        for (std::size_t i = 0; i + 1 < adam.size(); ++i)
        {
            TGraph *graph = makeCurve(curves, Form("adam64 %d -> %d", adam[i], adam[i + 1]), TColor::GetColor(doublingColors[i % doublingColors.size()].c_str()), 1, 3);
            if (!graph)
                continue;
            drawn.push_back(graph);
            labels.push_back(Form("%d#kern[0.2]{#rightarrow}#kern[0.2]{%d} epochs", adam[i], adam[i + 1]));
        }
        if (refSpread)
        {
            drawn.push_back(refSpread);
            labels.push_back(Form("Initialization spread (Adam, %d epochs)", refEpochs));
        }
        const std::pair<double, double> yrange = logRange(drawn);
        TH1 *frame = canvas.DrawFrame(ptMin, yrange.first, ptMax, yrange.second, (";p_{T} [GeV];" + yTitle).c_str());
        styleFrame(frame, true);
        drawExtrapolation(ptMax, yrange.first, yrange.second, true);
        for (TGraph *graph : drawn)
            graph->Draw("L same");
        canvas.RedrawAxis();

        TLatex text;
        text.SetNDC();
        text.SetTextFont(43);
        text.SetTextSize(kTextSize);
        text.DrawLatex(double(left) / width, 1 - 45.0 / height, sphenixLabel.c_str());
        const double xRight = double(left + frameW + 25) / width;
        text.DrawLatex(xRight, 1 - 45.0 / height, "Adam, float64: mean#kappa of 2 initializations");
        text.DrawLatex(xRight, 1 - 75.0 / height, "after an epoch doubling");
        TLegend *legend = makeLegend(xRight, 1 - (95.0 + 32 * labels.size()) / height, 1 - 10.0 / width, 1 - 90.0 / height);
        for (std::size_t i = 0; i < drawn.size(); ++i)
            legend->AddEntry(drawn[i], labels[i].c_str(), "l");
        legend->Draw();
        text.SetTextSize(kTextSize - 2);
        text.DrawLatex(xRight, double(bottom + 50) / height, "RMS over q,#kern[0.3]{#eta},#kern[0.3]{#phi} at each p_{T} value");
        text.DrawLatex(xRight, double(bottom + 20) / height, "Initialization spread: |#kappa_{1}#minus#kappa_{2}|/#sqrt{2}");

        canvas.SaveAs((outdir + "/convergence_kappa_vs_pt.png").c_str());
        canvas.SaveAs((outdir + "/convergence_kappa_vs_pt.pdf").c_str());
    }

    // ---- 2. faster procedures vs the reference (top) and float32 - float64 (bottom), shared pT axis
    {
        const int frameTop = 420, frameBottom = 300;
        const int height = top + frameTop + frameBottom + bottom;
        TCanvas canvas("c_convergence_procedures", "", width, height);
        canvas.SetCanvasSize(width, height);
        const double split = double(bottom + frameBottom) / height;
        TPad *pads[2] = {new TPad("padTop", "", 0.0, split, 1.0, 1.0), new TPad("padBottom", "", 0.0, 0.0, 1.0, split)};
        pads[0]->SetMargin(double(left) / width, double(right) / width, 0.0, double(top) / (top + frameTop));
        pads[1]->SetMargin(double(left) / width, double(right) / width, double(bottom) / (bottom + frameBottom), 0.0);

        // top: every non-reference procedure group except the float64 Adam doublings (figure 1); bottom: float32 twins
        std::vector<TGraph *> drawn[2];
        std::vector<std::string> labels[2];
        for (const MethodStyle &method : methods)
        {
            const std::vector<int> &epochs = epochsOf[method.key];
            for (std::size_t r = 0; r < epochs.size(); ++r)
            {
                if (method.key == "adam64")
                    continue;
                // float32 thinner than float64 and drawn after it, so a coinciding float64 twin stays visible
                const int lineWidth = method.key.substr(method.key.size() - 2) == "32" ? 2 : 4;
                TGraph *graph = makeCurve(curves, Form("%s %d vs reference", method.key.c_str(), epochs[r]), method.color, epochStyles[r % epochStyles.size()], lineWidth);
                if (graph)
                {
                    drawn[0].push_back(graph);
                    labels[0].push_back(Form("%s, %d epochs", method.label.c_str(), epochs[r]));
                }
                if (method.key.substr(method.key.size() - 2) != "32")
                    continue;
                // yardstick: seed-to-seed spread of the float64 twin at the same epochs, in its colour
                const std::string family = method.key.substr(0, method.key.size() - 2);
                const auto twin64 = std::find_if(methods.begin(), methods.end(), [&](const MethodStyle &m) { return m.key == family + "64"; });
                TGraph *spread = makeCurve(curves, Form("seed spread %s64 %d", family.c_str(), epochs[r]), twin64->color, 2, 4);
                if (spread)
                {
                    drawn[1].push_back(spread);
                    {
                        std::string family64 = twin64->label.substr(0, twin64->label.rfind(','));
                        const size_t decay = family64.find(" decay");   // "Adam + cosine decay" -> "Adam + cosine" so the entry fits the legend
                        if (decay != std::string::npos)
                            family64.erase(decay, 6);
                        labels[1].push_back(Form("Initialization spread (%s, %d ep.)", family64.c_str(), epochs[r]));
                    }
                }
                for (std::size_t s = 0; s < seeds.size(); ++s)
                {
                    TGraph *twin = makeCurve(curves, Form("float32 - float64 %s %d s%d", family.c_str(), epochs[r], seeds[s]), method.color, epochStyles[s % epochStyles.size()], 3);
                    if (!twin)
                        continue;
                    drawn[1].push_back(twin);
                    labels[1].push_back(Form("%s, %d epochs, initialization %d", method.label.substr(0, method.label.rfind(',')).c_str(), epochs[r], int(s) + 1));
                }
            }
        }
        if (refSpread)
        {
            drawn[0].push_back(refSpread);
            labels[0].push_back(Form("Initialization spread (Adam, %d epochs)", refEpochs));
        }
        const std::string padTitle[2] = {"Mean#kappa of 2 initializations vs reference", "float32#minusfloat64, same initialization and epochs"};
        for (int p = 0; p < 2; ++p)
        {
            canvas.cd();
            pads[p]->SetLogx();
            pads[p]->SetLogy();
            pads[p]->SetTicks(1, 1);
            pads[p]->SetFillStyle(4000);
            pads[p]->Draw();
            pads[p]->cd();
            const std::pair<double, double> yrange = logRange(drawn[p]);
            TH1 *frame = pads[p]->DrawFrame(ptMin, yrange.first, ptMax, yrange.second, (";p_{T} [GeV];" + yTitle).c_str());
            styleFrame(frame, p == 1);
            // decade labels only, so no label sits on the pad boundary; the bottom pad also drops its top-most
            // one, which would touch the lowest label of the top pad
            frame->GetYaxis()->SetMoreLogLabels(false);
            const int nDecadeLabels = int(std::floor(std::log10(yrange.second))) - int(std::ceil(std::log10(yrange.first))) + 1;
            if (p == 1 && nDecadeLabels > 0)
                frame->GetYaxis()->ChangeLabel(nDecadeLabels, -1, 0);
            drawExtrapolation(ptMax, yrange.first, yrange.second, p == 0); // the low bottom pad: shading only
            for (TGraph *graph : drawn[p])
                graph->Draw("L same");
            if (drawn[p].empty())
            {
                TLatex none;
                none.SetNDC();
                none.SetTextFont(43);
                none.SetTextSize(kTextSize);
                none.DrawLatex(pads[p]->GetLeftMargin() + 0.03, 0.5, "No comparison in this campaign");
            }
            pads[p]->RedrawAxis();

            // pad title and legend in the right margin, level with the pad
            canvas.cd();
            const double padTopY = p == 0 ? 1 - double(top) / height : split;
            const double xRight = double(left + frameW + 25) / width;
            TLatex text;
            text.SetNDC();
            text.SetTextFont(43);
            text.SetTextSize(kTextSize);
            text.DrawLatex(xRight, padTopY - 28.0 / height, padTitle[p].c_str());
            TLegend *legend = makeLegend(xRight, padTopY - (40.0 + 30 * labels[p].size()) / height, 1 - 10.0 / width, padTopY - 38.0 / height);
            legend->SetTextSize(kTextSize - 2);
            for (std::size_t i = 0; i < drawn[p].size(); ++i)
                legend->AddEntry(drawn[p][i], labels[p][i].c_str(), "l");
            legend->Draw();
        }
        canvas.cd();
        TLatex text;
        text.SetNDC();
        text.SetTextFont(43);
        text.SetTextSize(kTextSize);
        text.DrawLatex(double(left) / width, 1 - 45.0 / height, sphenixLabel.c_str());
        text.SetTextAlign(31);
        text.DrawLatex(1 - 10.0 / width, 1 - 45.0 / height, refLabel.c_str());

        canvas.SaveAs((outdir + "/convergence_procedures.png").c_str());
        canvas.SaveAs((outdir + "/convergence_procedures.pdf").c_str());
    }

    // ---- 3. accuracy vs cost: per (method, epochs) seed means, bars spanning the seeds; epochs of a method connected
    {
        const int height = top + frameH + bottom;
        TCanvas canvas("c_convergence_cost", "", width, height);
        canvas.SetCanvasSize(width, height);
        canvas.SetMargin(double(left) / width, double(right) / width, double(bottom) / height, double(top) / height);
        canvas.SetLogx();
        canvas.SetLogy();
        canvas.SetTicks(1, 1);

        std::vector<TGraph *> drawn;
        std::vector<std::string> labels;
        double xmin = 1.0e30, xmax = 0.0;
        for (const MethodStyle &method : methods)
        {
            TGraphAsymmErrors *graph = new TGraphAsymmErrors();
            for (const int epochs : epochsOf[method.key])
            {
                double nEval = 0.0, sum = 0.0, lo = 1.0e30, hi = 0.0;
                int n = 0;
                for (const auto &row : trials)
                {
                    if (row.at("method") != method.key || std::stoi(row.at("epochs")) != epochs)
                        continue;
                    const double rms = std::stod(row.at("rms_vs_ref_constrained"));
                    nEval += std::stod(row.at("n_eval"));
                    sum += rms;
                    lo = std::min(lo, rms);
                    hi = std::max(hi, rms);
                    ++n;
                }
                const int i = graph->GetN();
                // float32 points offset in x, so they stay visible next to a coinciding float64 twin
                const double xOffset = method.key.substr(method.key.size() - 2) == "32" ? 1.04 : 1.0;
                graph->SetPoint(i, xOffset * nEval / n, sum / n);
                graph->SetPointError(i, 0.0, 0.0, sum / n - lo, hi - sum / n);
                xmin = std::min(xmin, nEval / n);
                xmax = std::max(xmax, nEval / n);
            }
            if (graph->GetN() == 0)
                continue;
            graph->SetLineColor(method.color);
            graph->SetLineWidth(2);
            graph->SetMarkerColor(method.color);
            graph->SetMarkerStyle(method.marker);
            graph->SetMarkerSize(2.0);
            drawn.push_back(graph);
            labels.push_back(method.label);
        }
        // yardstick: reference seed-to-seed spread for pT < kPtConstrained (every pT value has the same number of
        // grid points, so the RMS over the grid is the quadratic mean of the per-pT values)
        double spreadSum2 = 0.0;
        int spreadN = 0;
        for (int i = 0; refSpread && i < refSpread->GetN(); ++i)
        {
            if (refSpread->GetPointX(i) >= kPtConstrained)
                continue;
            spreadSum2 += refSpread->GetPointY(i) * refSpread->GetPointY(i);
            ++spreadN;
        }
        std::pair<double, double> yrange = logRange(drawn);
        TLine *spread = nullptr;
        if (spreadN > 0)
        {
            const double value = std::sqrt(spreadSum2 / spreadN);
            yrange = {std::min(yrange.first, value / 2), std::max(yrange.second, value * 5)};
            spread = new TLine(xmin / 2, value, xmax * 2, value);
            spread->SetLineColor(kBlack);
            spread->SetLineStyle(7);
            spread->SetLineWidth(3);
        }
        TH1 *frame = canvas.DrawFrame(xmin / 2, yrange.first, xmax * 2, yrange.second,
            ";Network passes per seed (loss/gradient evaluations);RMS(#Delta#kappa)/#sigma_{#kappa}^{stat}, p_{T} < 2 GeV");
        styleFrame(frame, true);
        if (spread)
            spread->Draw();
        for (TGraph *graph : drawn)
            graph->Draw("PL same");
        canvas.RedrawAxis();

        TLatex text;
        text.SetNDC();
        text.SetTextFont(43);
        text.SetTextSize(kTextSize);
        text.DrawLatex(double(left) / width, 1 - 45.0 / height, sphenixLabel.c_str());
        const double xRight = double(left + frameW + 25) / width;
        text.DrawLatex(xRight, 1 - 45.0 / height, "Single seed vs reference");
        const std::size_t nEntries = spread ? labels.size() + 1 : labels.size();
        TLegend *legend = makeLegend(xRight, 1 - (95.0 + 34 * nEntries) / height, 1 - 10.0 / width, 1 - 90.0 / height);
        for (std::size_t i = 0; i < drawn.size(); ++i)
            legend->AddEntry(drawn[i], labels[i].c_str(), "pl");
        if (spread)
            legend->AddEntry(spread, Form("Seed-to-seed spread (Adam, %d epochs)", refEpochs), "l");
        legend->Draw();
        text.SetTextSize(kTextSize - 2);
        text.DrawLatex(xRight, double(bottom + 110) / height, "Points: seed mean; bars: seed range");
        text.DrawLatex(xRight, double(bottom + 80) / height, "float32 points offset in x for visibility");
        text.DrawLatex(xRight, double(bottom + 50) / height, "Line: increasing epochs of one procedure");
        text.DrawLatex(xRight, double(bottom + 20) / height, Form("Reference: Adam, float64, %d epochs", refEpochs));

        canvas.SaveAs((outdir + "/convergence_cost.png").c_str());
        canvas.SaveAs((outdir + "/convergence_cost.pdf").c_str());
    }

    std::cout << "Saved convergence figures to " << outdir << std::endl;
}
