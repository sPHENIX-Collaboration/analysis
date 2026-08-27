#include <TAxis.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TFile.h>
#include <TH2.h>
#include <TIterator.h>
#include <TKey.h>
#include <TLatex.h>
#include <TObject.h>
#include <TPad.h>
#include <TROOT.h>
#include <TString.h>
#include <TStyle.h>
#include <TSystem.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

TGaxis::SetMaxDigits(3);

namespace
{
struct PlotInfo
{
    TH2 *hist = nullptr;
    std::string name;
    std::string title;
    std::string ztitle;
    int kind = 100;
    double pt = 0.0;
    bool hasPt = false;
};

struct ZRange
{
    double center = 0.0;
    double halfWidth = 0.0;
    bool initialized = false;
};

std::string directoryName(const std::string &path)
{
    const std::size_t slash = path.find_last_of('/');
    return slash == std::string::npos ? "." : path.substr(0, slash);
}

std::string baseName(const std::string &path)
{
    const std::size_t slash = path.find_last_of('/');
    return slash == std::string::npos ? path : path.substr(slash + 1);
}

std::string stripExtension(const std::string &name)
{
    const std::size_t dot = name.find_last_of('.');
    return dot == std::string::npos ? name : name.substr(0, dot);
}

std::string safeFileStem(std::string name)
{
    for (char &c : name)
    {
        if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-' || c == '.'))
            c = '_';
    }
    return name;
}

bool parsePtFromName(const std::string &name, double &pt)
{
    const std::size_t pos = name.rfind("_pt");
    if (pos == std::string::npos)
        return false;

    std::string token = name.substr(pos + 3);
    std::replace(token.begin(), token.end(), 'p', '.');
    try
    {
        pt = std::stod(token);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

PlotInfo makePlotInfo(TH2 *hist)
{
    PlotInfo info;
    info.hist = hist;
    info.name = hist->GetName();
    info.hasPt = parsePtFromName(info.name, info.pt);

    if (info.name.rfind("eps_", 0) == 0)
    {
        info.kind = 0;
        info.title = "Charge-even curvature-scale correction";
        info.ztitle = "Charge-even correction #epsilon";
    }
    else if (info.name.rfind("delta_", 0) == 0)
    {
        info.kind = 1;
        info.title = "Charge-odd curvature-scale correction";
        info.ztitle = "Charge-odd correction #delta";
    }
    else if (info.name.rfind("kappa_qplus_", 0) == 0)
    {
        info.kind = 2;
        info.title = "Final p_{T} scale correction, q = +1";
        info.ztitle = "Final scale correction #kappa(q=+1)";
    }
    else if (info.name.rfind("kappa_qminus_", 0) == 0)
    {
        info.kind = 3;
        info.title = "Final p_{T} scale correction, q = -1";
        info.ztitle = "Final scale correction #kappa(q=-1)";
    }
    else
    {
        info.title = info.name;
        info.ztitle = "scale correction";
    }

    return info;
}

std::string histogramTitle(const PlotInfo &info, const bool grid)
{
    std::string title = info.title;
    if (grid)
    {
        title = info.ztitle;
        if (info.name.rfind("kappa_qplus_", 0) == 0)
            title = "q = +1";
        else if (info.name.rfind("kappa_qminus_", 0) == 0)
            title = "q = -1";

        return Form("%s;#eta;#phi;%s", title.c_str(), info.ztitle.c_str());
    }

    if (info.hasPt)
        return Form("%s, p_{T} = %.2f GeV;#eta;#phi;%s", title.c_str(), info.pt, info.ztitle.c_str());
    return Form("%s;#eta;#phi;%s", title.c_str(), info.ztitle.c_str());
}

void styleHistogram(PlotInfo &info, const ZRange &range, const bool grid)
{
    TH2 *hist = info.hist;
    hist->SetTitle(histogramTitle(info, grid).c_str());
    hist->SetMinimum(range.center - range.halfWidth);
    hist->SetMaximum(range.center + range.halfWidth);
    // hist->SetContour(1000);

    hist->GetXaxis()->SetTitle("#eta");
    hist->GetYaxis()->SetTitle("#phi");
    hist->GetZaxis()->SetTitle(info.ztitle.c_str());

    // hist->GetXaxis()->CenterTitle();
    // hist->GetYaxis()->CenterTitle();
    // hist->GetZaxis()->CenterTitle();
    hist->GetXaxis()->SetTitleOffset(grid ? 1.2 : 1.1);
    hist->GetYaxis()->SetTitleOffset(grid ? 1.3 : 1.1);
    hist->GetZaxis()->SetTitleOffset(grid ? 1.4 : 1.8);

    // if (grid)
    // {
    //     hist->SetTitleSize(0.045, "XYZ");
    //     hist->SetLabelSize(0.035, "XYZ");
    // }
    // else
    // {
    //     hist->SetTitleSize(0.045, "XYZ");
    //     hist->SetLabelSize(0.04, "XYZ");
    // }
}

std::vector<ZRange> getZRanges(const std::vector<PlotInfo> &plots)
{
    const int nKinds = 4;
    std::vector<ZRange> ranges(nKinds);
    for (int kind = 0; kind < nKinds; ++kind)
        ranges[kind].center = kind < 2 ? 0.0 : 1.0;

    for (const PlotInfo &plot : plots)
    {
        if (plot.kind < 0 || plot.kind >= nKinds)
            continue;

        const TH2 *hist = plot.hist;
        ZRange &range = ranges[plot.kind];
        for (int ix = 1; ix <= hist->GetNbinsX(); ++ix)
        {
            for (int iy = 1; iy <= hist->GetNbinsY(); ++iy)
            {
                const double value = hist->GetBinContent(ix, iy);
                if (std::isfinite(value))
                {
                    range.halfWidth = std::max(range.halfWidth, std::abs(value - range.center));
                    range.initialized = true;
                }
            }
        }
    }

    for (ZRange &range : ranges)
    {
        if (range.halfWidth <= 0.0)
            range.halfWidth = 1.0e-12;
    }

    return ranges;
}

ZRange getRangeForPlot(const PlotInfo &plot, const std::vector<ZRange> &ranges)
{
    if (plot.kind >= 0 && plot.kind < static_cast<int>(ranges.size()) && ranges[plot.kind].initialized)
        return ranges[plot.kind];

    ZRange range;
    range.center = 0.0;
    range.halfWidth = 1.0e-12;
    for (int ix = 1; ix <= plot.hist->GetNbinsX(); ++ix)
    {
        for (int iy = 1; iy <= plot.hist->GetNbinsY(); ++iy)
        {
            const double value = plot.hist->GetBinContent(ix, iy);
            if (std::isfinite(value))
            {
                range.halfWidth = std::max(range.halfWidth, std::abs(value));
                range.initialized = true;
            }
        }
    }
    return range;
}

std::string mapKindName(const int kind)
{
    if (kind == 0)
        return "epsilon";
    if (kind == 1)
        return "delta";
    if (kind == 2)
        return "kappa q=+1";
    if (kind == 3)
        return "kappa q=-1";
    return Form("kind %d", kind);
}

void drawPadLabel(const PlotInfo &info, const bool grid)
{
    TLatex label;
    label.SetNDC();
    label.SetTextFont(42);
    label.SetTextSize(grid ? 0.045 : 0.035);

    std::string charge_label;
    if (info.name.rfind("kappa_qplus_", 0) == 0)
        charge_label = "q = +1";
    else if (info.name.rfind("kappa_qminus_", 0) == 0)
        charge_label = "q = -1";

    const double x = gPad->GetLeftMargin();
    double y = 1 - gPad->GetTopMargin() + 0.025;
    if (info.hasPt)
    {
        if (!charge_label.empty())
        {
            label.DrawLatex(x, y, Form("p_{T} = %.2f GeV, 1/p_{T} = %.2f GeV^{-1}, %s", info.pt, 1.0 / info.pt, charge_label.c_str()));
            y -= grid ? 0.055 : 0.045;
        }
        else
        {
            label.DrawLatex(x, y, Form("p_{T} = %.2f GeV, 1/p_{T} = %.2f GeV^{-1}", info.pt, 1.0 / info.pt));
            y -= grid ? 0.055 : 0.045;
        }
    }
}

std::pair<int, int> gridShape(const std::vector<PlotInfo> &plots)
{
    std::vector<double> pts;
    bool allHavePt = !plots.empty();
    for (const PlotInfo &plot : plots)
    {
        allHavePt = allHavePt && plot.hasPt;
        if (!plot.hasPt)
            continue;
        const bool seen = std::any_of(pts.begin(), pts.end(), [&](double value) { return std::abs(value - plot.pt) < 1.0e-6; });
        if (!seen)
            pts.push_back(plot.pt);
    }

    if (allHavePt && pts.size() > 1 && pts.size() <= 12 && (plots.size() + pts.size() - 1) / pts.size() <= 6)
    {
        const int ncols = static_cast<int>(pts.size());
        const int nrows = static_cast<int>((plots.size() + ncols - 1) / ncols);
        return {ncols, nrows};
    }

    const int ncols = static_cast<int>(std::ceil(std::sqrt(static_cast<double>(plots.size()))));
    const int nrows = static_cast<int>((plots.size() + ncols - 1) / ncols);
    return {ncols, nrows};
}
} // namespace

void makeScaleMapPlot( //
    std::string scalemapfile = "./calib_out_upgrade20260731/kappa_maps.root")
{
    const std::string scalemapdir = directoryName(scalemapfile);
    const std::string scalemapname = baseName(scalemapfile);
    const std::string scalemapname_noext = stripExtension(scalemapname);
    const std::string outdir = scalemapdir + "/" + scalemapname_noext + "_plots";
    system(("mkdir -p " + outdir).c_str());

    gStyle->SetPalette(kLightTemperature);

    std::unique_ptr<TFile> input(TFile::Open(scalemapfile.c_str(), "READ"));
    if (!input || input->IsZombie())
    {
        std::cerr << "Could not open scale map file: " << scalemapfile << std::endl;
        return;
    }

    std::vector<PlotInfo> plots;
    TIter next(input->GetListOfKeys());
    while (TKey *key = static_cast<TKey *>(next()))
    {
        std::unique_ptr<TObject> object(key->ReadObj());
        TH2 *hist = dynamic_cast<TH2 *>(object.get());
        if (!hist)
            continue;

        hist->SetDirectory(nullptr);
        TH2 *clone = static_cast<TH2 *>(hist->Clone(Form("%s_plot_clone", hist->GetName())));
        if (!clone)
            continue;
        clone->SetDirectory(nullptr);
        clone->SetName(hist->GetName());
        plots.push_back(makePlotInfo(clone));
    }

    if (plots.empty())
    {
        std::cerr << "No TH2 histograms found in scale map file: " << scalemapfile << std::endl;
        return;
    }

    std::sort(plots.begin(), plots.end(),
        [](const PlotInfo &a, const PlotInfo &b)
        {
            if (a.kind != b.kind)
                return a.kind < b.kind;
            if (a.hasPt != b.hasPt)
                return a.hasPt > b.hasPt;
            if (a.hasPt && std::abs(a.pt - b.pt) > 1.0e-6)
                return a.pt < b.pt;
            return a.name < b.name;
        });

    const std::vector<ZRange> ranges = getZRanges(plots);
    for (int kind = 0; kind < static_cast<int>(ranges.size()); ++kind)
    {
        if (!ranges[kind].initialized)
            continue;
        std::cout << "Using z-axis range [" << ranges[kind].center - ranges[kind].halfWidth << ", " << ranges[kind].center + ranges[kind].halfWidth << "] for " << mapKindName(kind) << "."
                  << std::endl;
    }

    for (PlotInfo &plot : plots)
    {
        styleHistogram(plot, getRangeForPlot(plot, ranges), false);

        TCanvas canvas(Form("c_%s", plot.name.c_str()), plot.title.c_str(), 800, 700);
        canvas.SetTicks(1, 1);
        canvas.SetLeftMargin(0.13);
        canvas.SetRightMargin(0.23);
        canvas.SetBottomMargin(0.13);
        canvas.SetTopMargin(0.08);
        // plot.hist->SetContour(1000);
        plot.hist->Draw("COLZ");
        // plot.hist->SetLineColor(kBlack);
        // plot.hist->SetLineWidth(1);
        // plot.hist->Draw("CONT3 same");
        drawPadLabel(plot, false);

        const std::string stem = outdir + "/" + safeFileStem(plot.name);
        canvas.SaveAs((stem + ".png").c_str());
        canvas.SaveAs((stem + ".pdf").c_str());
    }

    const std::pair<int, int> shape = gridShape(plots);
    const int ncols = shape.first;
    const int nrows = shape.second;
    TCanvas gridCanvas("c_all_scale_maps", "All scale maps", 360 * ncols, 330 * nrows);
    gridCanvas.Divide(ncols, nrows, 0.001, 0.001);

    for (std::size_t i = 0; i < plots.size(); ++i)
    {
        gridCanvas.cd(static_cast<int>(i) + 1);
        gPad->SetTicks(1, 1);
        gPad->SetLeftMargin(0.12);
        gPad->SetRightMargin(0.18);
        // gPad->SetBottomMargin(0.12);
        gPad->SetTopMargin(0.12);

        styleHistogram(plots[i], getRangeForPlot(plots[i], ranges), true);
        plots[i].hist->Draw("COLZ");
        drawPadLabel(plots[i], true);
    }

    gridCanvas.SaveAs((outdir + "/all_scale_maps_grid.png").c_str());
    gridCanvas.SaveAs((outdir + "/all_scale_maps_grid.pdf").c_str());

    std::cout << "Saved scale-map plots to " << outdir << std::endl;

    for (PlotInfo &plot : plots)
        delete plot.hist;
}
