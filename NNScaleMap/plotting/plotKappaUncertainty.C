#include <TCanvas.h>
#include <TColor.h>
#include <TGaxis.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TObjArray.h>
#include <TPad.h>
#include <TPaletteAxis.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TTree.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "plotUtils.h"

// Per-grid-point uncertainty of the kappa map from the syst/ campaign (training/syst/combine.py output):
//   <campaign>/kappa_lookup_unc.csv -> <outdir>/kappa_vs_pt_unc.{png,pdf}        kappa vs pT with stat and total bands
//                                      <outdir>/kappa_unc_breakdown_vs_pt.{png,pdf} RMS over (eta, phi) of each source vs pT
//                                      <outdir>/kappa_unc_total_eta_phi.{png,pdf}   (eta, phi) maps of the total uncertainty
// usage (fresh shell with ROOT):  root -l -b -q 'plotKappaUncertainty.C("<campaign>")'   (default outdir <campaign>/figures)

namespace
{
struct SourceStyle
{
    std::string name; // combine.py source: column sigma_<name>
    std::string label;
    int color;
    int lineStyle;
};

struct ChargeStyle
{
    int q;
    std::string label;
    int color;
};

// pixel (font 43) text sizes shared by every pad
const int kTitleSize = 28;
const int kLabelSize = 24;
const int kTextSize = 24;

// Canvas of ncol x nrow pads whose frames all have the same pixel size. Only the outer pads carry the
// left/right/bottom/top margins (in pixels) for the shared axis titles, labels, legends and palette.
TCanvas *makePadGrid(const std::string &name, const int ncol, const int nrow, const int frameW, const int frameH, const int left, const int right, const int bottom,
    const int top, std::vector<TPad *> &pads)
{
    const int width = left + ncol * frameW + right;
    const int height = top + nrow * frameH + bottom;
    TCanvas *canvas = new TCanvas(name.c_str(), name.c_str(), width, height);
    canvas->SetCanvasSize(width, height);
    for (int row = 0; row < nrow; ++row)
    {
        for (int col = 0; col < ncol; ++col)
        {
            const int x0 = col == 0 ? 0 : left + col * frameW;
            const int x1 = left + (col + 1) * frameW + (col == ncol - 1 ? right : 0);
            const int y0 = bottom + (nrow - 1 - row) * frameH - (row == nrow - 1 ? bottom : 0);
            const int y1 = bottom + (nrow - row) * frameH + (row == 0 ? top : 0);
            canvas->cd();
            TPad *pad = new TPad(Form("%s_pad%d_%d", name.c_str(), row, col), "", double(x0) / width, double(y0) / height, double(x1) / width, double(y1) / height);
            pad->SetLeftMargin(col == 0 ? double(left) / (x1 - x0) : 0.0);
            pad->SetRightMargin(col == ncol - 1 ? double(right) / (x1 - x0) : 0.0);
            pad->SetBottomMargin(row == nrow - 1 ? double(bottom) / (y1 - y0) : 0.0);
            pad->SetTopMargin(row == 0 ? double(top) / (y1 - y0) : 0.0);
            pad->SetTicks(1, 1);
            pad->SetFillStyle(4000);
            pad->Draw();
            pads.push_back(pad);
        }
    }
    return canvas;
}

// Axis fonts in pixels so that every pad of a multi-pad canvas has the same absolute sizes;
// hidden shared axes keep zero label size and no title.
void styleFrame(TH1 *frame, const bool showX, const bool showY, const double titleOffsetX, const double titleOffsetY)
{
    frame->SetStats(false);
    for (TAxis *axis : {frame->GetXaxis(), frame->GetYaxis(), frame->GetZaxis()})
    {
        axis->SetTitleFont(43);
        axis->SetLabelFont(43);
        axis->SetTitleSize(kTitleSize);
        axis->SetLabelSize(kLabelSize);
    }
    frame->GetXaxis()->SetTitleOffset(titleOffsetX);
    frame->GetYaxis()->SetTitleOffset(titleOffsetY);
    if (!showX)
    {
        frame->GetXaxis()->SetLabelSize(0);
        frame->GetXaxis()->SetTitle("");
    }
    if (!showY)
    {
        frame->GetYaxis()->SetLabelSize(0);
        frame->GetYaxis()->SetTitle("");
    }
}

// nearest grid value to a requested slice position
int nearestIndex(const std::vector<double> &grid, const double value)
{
    int best = 0;
    for (int i = 1; i < static_cast<int>(grid.size()); ++i)
    {
        if (std::abs(grid[i] - value) < std::abs(grid[best] - value))
            best = i;
    }
    return best;
}
} // namespace

void plotKappaUncertainty(std::string campaign = "/sphenix/tg/tg01/hf/hjheng/HF-analysis/NNScaleMap/syst/fill79514to79516", std::string outdir = "")
{
    // one ordered list: legend order, colors and line styles of the uncertainty sources
    const std::vector<SourceStyle> sources = {
        {"stat", "Statistical (bootstrap)", TColor::GetColor("#4477AA"), 1},
        {"seed", "Network initialisation (seed)", TColor::GetColor("#EE6677"), 1},
        {"feature", "Momentum feature", TColor::GetColor("#228833"), 1},
        {"binning", "Loss-cell p_{T} binning", TColor::GetColor("#CCBB44"), 1},
        {"lam_split", "#Lambda/#bar{#Lambda} split weight", TColor::GetColor("#66CCEE"), 7},
        {"lam_alpha", "K^{0}_{S}#kern[0.3]{#LT#alpha#GT} weight", TColor::GetColor("#AA3377"), 7},
        {"lam_prior", "(#varepsilon,#kern[0.3]{#delta}) prior weight", TColor::GetColor("#BBBBBB"), 7},
        {"window", "Mass-fit window", TColor::GetColor("#EE7733"), 7},
        {"bkg", "Background model", TColor::GetColor("#009988"), 7},
    };
    const std::vector<ChargeStyle> charges = {
        {+1, "q = +1", TColor::GetColor("#CC3311")},
        {-1, "q = -1", TColor::GetColor("#0077BB")},
    };
    const std::vector<double> etaSlices = {-1.0, 0.0, 1.0}; // kappa-vs-pT columns
    const double phiSlice = 0.0;                            // kappa vs pT is shown at one grid phi, so the bands are exact per-point uncertainties
    const std::vector<double> ptSlices = {0.3, 1.0, 3.0};  // GeV, (eta, phi) map columns (nearest grid pT)
    const std::string sphenixLabel = "#it{#bf{sPHENIX}} Internal";

    if (outdir.empty())
        outdir = campaign + "/figures";
    gSystem->mkdir(outdir.c_str(), true);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    TGaxis::SetMaxDigits(3);

    // ---- read the table: '#' comment lines are skipped and the first other line is the header
    const std::string csv = campaign + "/kappa_lookup_unc.csv";
    TTree table("kappa_unc", "kappa_lookup_unc.csv");
    if (table.ReadFile(csv.c_str(), "", ',') <= 0)
    {
        std::cerr << "Could not read " << csv << std::endl;
        return;
    }
    std::map<std::string, std::vector<double>> column;
    {
        std::map<std::string, float> value;
        TObjArray *branches = table.GetListOfBranches();
        for (int i = 0; i < branches->GetEntries(); ++i)
        {
            const std::string name = branches->At(i)->GetName();
            table.SetBranchAddress(name.c_str(), &value[name]);
        }
        for (Long64_t entry = 0; entry < table.GetEntries(); ++entry)
        {
            table.GetEntry(entry);
            for (const auto &item : value)
                column[item.first].push_back(item.second);
        }
    }
    for (const std::string name : {"q", "pT", "eta", "phi", "kappa", "sigma_stat", "sigma_syst"})
    {
        if (!column.count(name))
        {
            std::cerr << csv << ": missing column " << name << std::endl;
            return;
        }
    }
    std::vector<SourceStyle> present;
    for (const SourceStyle &source : sources)
    {
        if (column.count("sigma_" + source.name))
            present.push_back(source);
    }
    for (const auto &item : column)
    {
        const std::string &name = item.first;
        const bool known = std::any_of(sources.begin(), sources.end(), [&](const SourceStyle &s) { return name == "sigma_" + s.name; });
        if (name.rfind("sigma_", 0) == 0 && name != "sigma_stat" && name != "sigma_syst" && !known)
            std::cerr << "Warning: " << name << " has no entry in the source list and is not drawn" << std::endl;
    }
    const std::size_t nRows = column["q"].size();
    std::vector<double> sigmaTotal(nRows);
    for (std::size_t i = 0; i < nRows; ++i)
        sigmaTotal[i] = std::hypot(column["sigma_stat"][i], column["sigma_syst"][i]);

    // grid axes and the row of every (q, pT, eta, phi) point
    std::set<double> ptSet(column["pT"].begin(), column["pT"].end());
    std::set<double> etaSet(column["eta"].begin(), column["eta"].end());
    std::set<double> phiSet(column["phi"].begin(), column["phi"].end());
    const std::vector<double> ptGrid(ptSet.begin(), ptSet.end());
    const std::vector<double> etaGrid(etaSet.begin(), etaSet.end());
    const std::vector<double> phiGrid(phiSet.begin(), phiSet.end());
    const int nPt = ptGrid.size();
    const int nEta = etaGrid.size();
    const int nPhi = phiGrid.size();
    std::vector<int> rowOf(2 * nPt * nEta * nPhi, -1);
    auto gridIndex = [&](const int iq, const int ipt, const int ieta, const int iphi) { return ((iq * nPt + ipt) * nEta + ieta) * nPhi + iphi; };
    for (std::size_t i = 0; i < nRows; ++i)
    {
        const int iq = column["q"][i] > 0 ? 0 : 1;
        const int ipt = std::lower_bound(ptGrid.begin(), ptGrid.end(), column["pT"][i]) - ptGrid.begin();
        const int ieta = std::lower_bound(etaGrid.begin(), etaGrid.end(), column["eta"][i]) - etaGrid.begin();
        const int iphi = std::lower_bound(phiGrid.begin(), phiGrid.end(), column["phi"][i]) - phiGrid.begin();
        rowOf[gridIndex(iq, ipt, ieta, iphi)] = i;
    }
    if (std::count(rowOf.begin(), rowOf.end(), -1) > 0)
    {
        std::cerr << csv << ": the rows do not fill a complete (q, pT, eta, phi) grid" << std::endl;
        return;
    }
    // the phi grid closes on itself (phi = +pi repeats -pi); the RMS counts that point once
    const bool phiClosed = std::abs(phiGrid.back() - phiGrid.front() - 2 * M_PI) < 1.0e-3;
    const int nPhiIndependent = phiClosed ? nPhi - 1 : nPhi;
    // pT axes of the multi-pad figures start at the lowest grid pT and end above the highest, so the last pT
    // label stays inside its pad; the first label of every pad but the leftmost is dropped
    const double ptAxisMax = 1.1 * ptGrid.back();
    std::cout << "Read " << nRows << " grid points: " << nPt << " pT x " << nEta << " eta x " << nPhi << " phi x 2 charges, " << present.size() << " sources" << std::endl;

    // ---- 1. kappa vs pT with stat and stat (+) syst bands, both charges, at fixed eta and phi
    {
        const int iphi = nearestIndex(phiGrid, phiSlice);
        const int ncol = etaSlices.size();
        std::vector<TPad *> pads;
        TCanvas *canvas = makePadGrid("c_kappa_vs_pt", ncol, 1, 440, 440, 120, 25, 90, 110, pads);

        // per column and charge: total band, stat band, central value
        std::vector<std::vector<TGraphErrors *>> totalBand(ncol), statBand(ncol), central(ncol);
        double ymin = 1.0;
        double ymax = 1.0;
        for (int col = 0; col < ncol; ++col)
        {
            const int ieta = nearestIndex(etaGrid, etaSlices[col]);
            for (std::size_t c = 0; c < charges.size(); ++c)
            {
                TGraphErrors *total = new TGraphErrors(nPt);
                TGraphErrors *stat = new TGraphErrors(nPt);
                TGraphErrors *line = new TGraphErrors(nPt);
                for (int ipt = 0; ipt < nPt; ++ipt)
                {
                    const int row = rowOf[gridIndex(c, ipt, ieta, iphi)];
                    const double kappa = column["kappa"][row];
                    total->SetPoint(ipt, ptGrid[ipt], kappa);
                    total->SetPointError(ipt, 0.0, sigmaTotal[row]);
                    stat->SetPoint(ipt, ptGrid[ipt], kappa);
                    stat->SetPointError(ipt, 0.0, column["sigma_stat"][row]);
                    line->SetPoint(ipt, ptGrid[ipt], kappa);
                    ymin = std::min(ymin, kappa - sigmaTotal[row]);
                    ymax = std::max(ymax, kappa + sigmaTotal[row]);
                }
                total->SetFillColorAlpha(charges[c].color, 0.25);
                stat->SetFillColor(TColor::GetColorDark(charges[c].color)); // opaque, drawn on top of the line
                line->SetLineColor(charges[c].color);
                line->SetLineWidth(2);
                totalBand[col].push_back(total);
                statBand[col].push_back(stat);
                central[col].push_back(line);
            }
        }
        const double margin = 0.08 * (ymax - ymin);

        for (int col = 0; col < ncol; ++col)
        {
            pads[col]->cd();
            pads[col]->SetLogx();
            TH1 *frame = pads[col]->DrawFrame(ptGrid.front(), ymin - margin, ptAxisMax, ymax + margin, ";p_{T} [GeV];#kappa = p_{T}^{corr}/p_{T}");
            styleFrame(frame, true, col == 0, 1.3, 2.0);
            frame->GetXaxis()->SetMoreLogLabels();
            frame->GetXaxis()->SetNoExponent();
            if (col > 0) // the first pT label would sit on the edge shared with the previous pad
                frame->GetXaxis()->ChangeLabel(1, -1, 0);
            TLine *unity = new TLine(ptGrid.front(), 1.0, ptAxisMax, 1.0);
            unity->SetLineStyle(2);
            unity->SetLineColor(kGray + 2);
            unity->Draw();
            // total band, central value, then the narrow stat band on top so it stays visible
            for (std::size_t c = 0; c < charges.size(); ++c)
                totalBand[col][c]->Draw("3 same");
            for (std::size_t c = 0; c < charges.size(); ++c)
            {
                central[col][c]->Draw("L same");
                statBand[col][c]->Draw("3 same");
            }
            pads[col]->RedrawAxis();
        }

        canvas->cd();
        TLatex text;
        text.SetNDC();
        text.SetTextFont(43);
        text.SetTextSize(kTextSize);
        const double width = canvas->GetWw();
        const double height = canvas->GetWh();
        text.DrawLatex(120 / width, 1 - 35 / height, sphenixLabel.c_str());
        text.SetTextAlign(21);
        for (int col = 0; col < ncol; ++col)
            text.DrawLatex((120 + (col + 0.5) * 440) / width, 1 - 95 / height,
                Form("#eta = %.2f, #phi = %.2f", etaGrid[nearestIndex(etaGrid, etaSlices[col])], phiGrid[iphi]));

        // legend in the top margin, never on the data
        // one row per charge: central value, stat band, stat (+) syst band, in the colours drawn
        TLegend *legend = new TLegend(500 / width, 1 - 70 / height, 1 - 25 / width, 1 - 5 / height);
        legend->SetNColumns(3);
        legend->SetBorderSize(0);
        legend->SetFillStyle(0);
        legend->SetTextFont(43);
        legend->SetTextSize(kTextSize);
        for (std::size_t c = 0; c < charges.size(); ++c)
        {
            legend->AddEntry(central[0][c], charges[c].label.c_str(), "l");
            legend->AddEntry(statBand[0][c], "Stat.", "f");
            legend->AddEntry(totalBand[0][c], "Stat.#oplussyst.", "f");
        }
        legend->Draw();

        canvas->SaveAs((outdir + "/kappa_vs_pt_unc.png").c_str());
        canvas->SaveAs((outdir + "/kappa_vs_pt_unc.pdf").c_str());
    }

    // ---- 2. uncertainty breakdown vs pT: RMS over the (eta, phi) grid at each pT, per charge
    {
        const int ncol = charges.size();
        std::vector<TPad *> pads;
        TCanvas *canvas = makePadGrid("c_kappa_unc_breakdown", ncol, 1, 520, 460, 130, 400, 90, 70, pads);

        // curves: total, stat (+) syst sum of the non-stat sources, then every source in list order
        std::vector<std::string> curveLabel = {"Total (stat.#oplus syst.)", "Syst. (quadrature sum)"};
        std::vector<int> curveColor = {kBlack, kBlack};
        std::vector<int> curveStyle = {1, 2};
        std::vector<int> curveWidth = {4, 3};
        std::vector<const std::vector<double> *> curveSigma = {&sigmaTotal, &column["sigma_syst"]};
        for (const SourceStyle &source : present)
        {
            curveLabel.push_back(source.label);
            curveColor.push_back(source.color);
            curveStyle.push_back(source.lineStyle);
            curveWidth.push_back(3);
            curveSigma.push_back(&column["sigma_" + source.name]);
        }

        std::vector<std::vector<TGraph *>> curves(ncol);
        double ymin = 1.0e30;
        double ymax = 0.0;
        for (int iq = 0; iq < ncol; ++iq)
        {
            for (std::size_t k = 0; k < curveSigma.size(); ++k)
            {
                TGraph *graph = new TGraph(nPt);
                for (int ipt = 0; ipt < nPt; ++ipt)
                {
                    double sum2 = 0.0;
                    for (int ieta = 0; ieta < nEta; ++ieta)
                    {
                        for (int iphi = 0; iphi < nPhiIndependent; ++iphi)
                            sum2 += std::pow((*curveSigma[k])[rowOf[gridIndex(iq, ipt, ieta, iphi)]], 2);
                    }
                    const double rms = std::sqrt(sum2 / (nEta * nPhiIndependent));
                    graph->SetPoint(ipt, ptGrid[ipt], rms);
                    if (rms > 0)
                        ymin = std::min(ymin, rms);
                    ymax = std::max(ymax, rms);
                }
                graph->SetLineColor(curveColor[k]);
                graph->SetLineStyle(curveStyle[k]);
                graph->SetLineWidth(curveWidth[k]);
                curves[iq].push_back(graph);
            }
        }

        for (int iq = 0; iq < ncol; ++iq)
        {
            pads[iq]->cd();
            pads[iq]->SetLogx();
            pads[iq]->SetLogy();
            TH1 *frame = pads[iq]->DrawFrame(ptGrid.front(), ymin / 2, ptAxisMax, ymax * 4, ";p_{T} [GeV];RMS(#sigma_{#kappa})");
            styleFrame(frame, true, iq == 0, 1.3, 2.1);
            frame->GetXaxis()->SetMoreLogLabels();
            frame->GetXaxis()->SetNoExponent();
            if (iq > 0) // the first pT label would sit on the edge shared with the previous pad
                frame->GetXaxis()->ChangeLabel(1, -1, 0);
            for (std::size_t k = curves[iq].size(); k-- > 0;)
                curves[iq][k]->Draw("L same");
            pads[iq]->RedrawAxis();
        }

        canvas->cd();
        const double width = canvas->GetWw();
        const double height = canvas->GetWh();
        TLatex text;
        text.SetNDC();
        text.SetTextFont(43);
        text.SetTextSize(kTextSize);
        text.SetTextAlign(21);
        for (int iq = 0; iq < ncol; ++iq)
            text.DrawLatex((130 + (iq + 0.5) * 520) / width, 1 - 50 / height, charges[iq].label.c_str());
        text.SetTextAlign(11);
        const double xRight = (130 + ncol * 520 + 25) / width;
        text.DrawLatex(xRight, 1 - 50 / height, sphenixLabel.c_str());

        // legend in the right margin, never on the data; the aggregation is stated under it
        TLegend *legend = new TLegend(xRight, 1 - (75 + 32 * curveLabel.size()) / height, 1 - 10 / width, 1 - 70 / height);
        legend->SetBorderSize(0);
        legend->SetFillStyle(0);
        legend->SetTextFont(43);
        legend->SetTextSize(kTextSize);
        for (std::size_t k = 0; k < curveLabel.size(); ++k)
            legend->AddEntry(curves[0][k], curveLabel[k].c_str(), "l");
        legend->Draw();
        const double yNote = 1 - (75 + 32 * curveLabel.size() + 40) / height;
        text.SetTextSize(kTextSize - 2);
        text.DrawLatex(xRight, yNote, "RMS over the lookup grid,");
        text.DrawLatex(xRight, yNote - 30 / height, Form("#eta: %d points, |#eta|#leq %.1f", nEta, etaGrid.back()));
        text.DrawLatex(xRight, yNote - 60 / height, Form("#phi: %d points in [#minus#pi,#kern[0.3]{#pi})", nPhiIndependent));

        canvas->SaveAs((outdir + "/kappa_unc_breakdown_vs_pt.png").c_str());
        canvas->SaveAs((outdir + "/kappa_unc_breakdown_vs_pt.pdf").c_str());
    }

    // ---- 3. (eta, phi) maps of the total uncertainty at a few pT, both charges, one common z scale
    {
        const int ncol = ptSlices.size();
        const int nrow = charges.size();
        const double etaHalfBin = 0.5 * (etaGrid[1] - etaGrid[0]);
        const double phiHalfBin = 0.5 * (phiGrid[1] - phiGrid[0]);
        std::vector<TPad *> pads;
        TCanvas *canvas = makePadGrid("c_kappa_unc_maps", ncol, nrow, 360, 360, 210, 200, 90, 100, pads);

        std::vector<TH2D *> maps;
        double zmax = 0.0;
        for (int iq = 0; iq < nrow; ++iq)
        {
            for (int col = 0; col < ncol; ++col)
            {
                const int ipt = nearestIndex(ptGrid, ptSlices[col]);
                TH2D *map = new TH2D(Form("unc_q%d_pt%d", iq, col), ";#eta;#phi [rad];#sigma_{#kappa}^{total} [10^{-3}]", nEta, etaGrid.front() - etaHalfBin,
                    etaGrid.back() + etaHalfBin, nPhi, phiGrid.front() - phiHalfBin, phiGrid.back() + phiHalfBin);
                map->SetDirectory(nullptr);
                for (int ieta = 0; ieta < nEta; ++ieta)
                {
                    for (int iphi = 0; iphi < nPhi; ++iphi)
                    {
                        const double value = 1.0e3 * sigmaTotal[rowOf[gridIndex(iq, ipt, ieta, iphi)]];
                        map->SetBinContent(ieta + 1, iphi + 1, value);
                        zmax = std::max(zmax, value);
                    }
                }
                maps.push_back(map);
            }
        }

        gStyle->SetPalette(kViridis);
        for (int iq = 0; iq < nrow; ++iq)
        {
            for (int col = 0; col < ncol; ++col)
            {
                TPad *pad = pads[iq * ncol + col];
                TH2D *map = maps[iq * ncol + col];
                pad->cd();
                map->SetMinimum(0.0);
                map->SetMaximum(zmax);
                map->SetContour(255);
                styleFrame(map, iq == nrow - 1, col == 0, 1.3, 1.6);
                map->GetZaxis()->CenterTitle();
                map->GetZaxis()->SetTitleOffset(2.2);
                map->GetXaxis()->SetNdivisions(506);
                map->Draw("COL");
                pad->RedrawAxis();
            }
        }

        canvas->cd();
        const double width = canvas->GetWw();
        const double height = canvas->GetWh();
        const double frameTop = 1 - 100 / height;
        const double frameBottom = 90 / height;
        // one palette for all pads, in the right margin
        TPaletteAxis *palette = new TPaletteAxis((210 + ncol * 360 + 20) / width, frameBottom, (210 + ncol * 360 + 50) / width, frameTop, maps.front());
        palette->SetLabelFont(43);
        palette->SetLabelSize(kLabelSize);
        palette->Draw();

        TLatex text;
        text.SetNDC();
        text.SetTextFont(43);
        text.SetTextSize(kTextSize);
        text.DrawLatex(210 / width, 1 - 35 / height, sphenixLabel.c_str());
        text.SetTextAlign(31);
        text.DrawLatex((210 + ncol * 360) / width, 1 - 35 / height, "Total = stat.#oplus syst.");
        text.SetTextAlign(21);
        for (int col = 0; col < ncol; ++col)
            text.DrawLatex((210 + (col + 0.5) * 360) / width, 1 - 80 / height, Form("p_{T} = %.2f GeV", ptGrid[nearestIndex(ptGrid, ptSlices[col])]));
        text.SetTextSize(kTitleSize);
        text.SetTextAngle(90);
        for (int iq = 0; iq < nrow; ++iq)
            text.DrawLatex(45 / width, (90 + (nrow - iq - 0.5) * 360) / height, charges[iq].label.c_str());

        canvas->SaveAs((outdir + "/kappa_unc_total_eta_phi.png").c_str());
        canvas->SaveAs((outdir + "/kappa_unc_total_eta_phi.pdf").c_str());
    }

    std::cout << "Saved kappa-uncertainty figures to " << outdir << std::endl;
}
