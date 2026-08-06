#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TObject.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

TGaxis::SetMaxDigits(3);

namespace
{
struct SpeciesSpec
{
    std::string name;
    std::string label;
    std::string rawKey;
    std::string correctedKey;
    double pdgMass = 0.0;
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

TH1 *getHistogram(TFile *input, const std::string &key)
{
    TH1 *hist = dynamic_cast<TH1 *>(input->Get(key.c_str()));
    if (!hist)
    {
        std::cerr << "Missing TH1 histogram: " << key << std::endl;
        return nullptr;
    }

    TH1 *clone = static_cast<TH1 *>(hist->Clone(Form("%s_plot_clone", key.c_str())));
    if (!clone)
    {
        std::cerr << "Could not clone histogram: " << key << std::endl;
        return nullptr;
    }
    clone->SetDirectory(nullptr);
    return clone;
}

double histogramMax(const TH1 *a, const TH1 *b)
{
    double ymax = 0.0;
    for (const TH1 *hist : {a, b})
    {
        for (int bin = 1; bin <= hist->GetNbinsX(); ++bin)
            ymax = std::max(ymax, hist->GetBinContent(bin));
    }
    return ymax > 0.0 ? ymax : 1.0;
}

void styleHistogram(TH1 *hist, const int color, const int lineStyle)
{
    hist->SetStats(false);
    hist->SetLineColor(color);
    hist->SetMarkerColor(color);
    hist->SetLineStyle(lineStyle);
    hist->SetLineWidth(3);
    hist->GetXaxis()->SetTitle("Invariant mass [GeV]");
    hist->GetYaxis()->SetTitle("Candidates");
    // hist->GetXaxis()->SetTitleOffset(1.1);
    hist->GetYaxis()->SetTitleOffset(1.25);
}

void drawPdgLine(const double pdgMass, const double ymax)
{
    TLine *line = new TLine(pdgMass, 0.0, pdgMass, ymax);
    line->SetLineColor(kGray + 2);
    line->SetLineStyle(2);
    line->SetLineWidth(2);
    line->SetBit(kCanDelete);
    line->Draw();
}

void drawComparisonPad(TH1 *raw, TH1 *corrected, const SpeciesSpec &species, const bool drawLegend)
{
    styleHistogram(raw, kAzure + 2, 1);
    styleHistogram(corrected, kOrange + 7, 1);

    const double ymax = 1.25 * histogramMax(raw, corrected);
    raw->SetMaximum(ymax);
    corrected->SetMaximum(ymax);
    raw->SetTitle(Form("%s mass closure;Invariant mass [GeV];Candidates", species.label.c_str()));
    raw->GetXaxis()->SetNdivisions(505);
    raw->Draw("hist");
    corrected->Draw("hist same");
    drawPdgLine(species.pdgMass, ymax);

    // TLatex label;
    // label.SetNDC();
    // label.SetTextFont(42);
    // label.SetTextSize(0.040);
    // label.DrawLatex(0.16, 0.86, species.label.c_str());
    // label.SetTextSize(0.032);
    // label.DrawLatex(0.16, 0.805, Form("PDG mass = %.6f GeV", species.pdgMass));

    if (drawLegend)
    {
        TLegend *legend = new TLegend(0.54, 0.68, 0.88, 0.88);
        legend->SetHeader(Form("Resonance species: %s", species.label.c_str()), "C");
        legend->SetBorderSize(0);
        legend->SetFillStyle(0);
        legend->SetTextSize(0.04);
        legend->SetBit(kCanDelete);
        legend->AddEntry(raw, "Before correction", "l");
        legend->AddEntry(corrected, "After correction", "l");
        legend->AddEntry(static_cast<TObject *>(nullptr), "Dashed: PDG mass", "");
        legend->Draw();
    }
}
} // namespace

void massClosure(std::string inputfile = "./calib_out_pythiaInjection_20260805/stage1_mass_histograms.root")
{
    const std::string inputdir = directoryName(inputfile);
    const std::string inputname = baseName(inputfile);
    const std::string inputname_noext = stripExtension(inputname);
    const std::string outdir = inputdir + "/" + inputname_noext + "_closure";
    gSystem->mkdir(outdir.c_str(), true);

    gStyle->SetOptStat(0);
    gStyle->SetTitleFont(42, "XYZ");
    gStyle->SetLabelFont(42, "XYZ");
    gStyle->SetLegendFont(42);

    std::unique_ptr<TFile> input(TFile::Open(inputfile.c_str(), "READ"));
    if (!input || input->IsZombie())
    {
        std::cerr << "Could not open mass histogram file: " << inputfile << std::endl;
        return;
    }

    const std::vector<SpeciesSpec> speciesList = {
        {"kshort", "K^{0}_{S}", "kshort_mass_raw", "kshort_mass_corrected", 0.497611},
        {"lambda", "#Lambda", "lambda_mass_raw", "lambda_mass_corrected", 1.115683},
        {"anti_lambda", "#bar{#Lambda}", "anti_lambda_mass_raw", "anti_lambda_mass_corrected", 1.115683},
    };

    std::vector<TH1 *> rawHists;
    std::vector<TH1 *> correctedHists;
    std::vector<SpeciesSpec> foundSpecies;
    rawHists.reserve(speciesList.size());
    correctedHists.reserve(speciesList.size());
    foundSpecies.reserve(speciesList.size());

    for (const SpeciesSpec &species : speciesList)
    {
        TH1 *raw = getHistogram(input.get(), species.rawKey);
        TH1 *corrected = getHistogram(input.get(), species.correctedKey);
        if (!raw || !corrected)
        {
            delete raw;
            delete corrected;
            continue;
        }

        rawHists.push_back(raw);
        correctedHists.push_back(corrected);
        foundSpecies.push_back(species);

        TCanvas canvas(Form("c_%s_mass_closure", species.name.c_str()), Form("%s mass closure", species.label.c_str()), 800, 750);
        canvas.SetTicks(1, 1);
        canvas.SetLeftMargin(0.13);
        canvas.SetRightMargin(0.05);
        // canvas.SetBottomMargin(0.12);
        canvas.SetTopMargin(0.08);
        drawComparisonPad(raw, corrected, species, true);
        canvas.RedrawAxis();

        const std::string stem = outdir + "/" + safeFileStem(species.name + "_mass_comparison");
        canvas.SaveAs((stem + ".png").c_str());
        canvas.SaveAs((stem + ".pdf").c_str());
    }

    if (foundSpecies.empty())
    {
        std::cerr << "No complete raw/corrected mass histogram pairs found in " << inputfile << std::endl;
        return;
    }

    {
        TCanvas gridCanvas("c_mass_closure_all", "Mass closure comparisons", 1500, 520);
        gridCanvas.Divide(static_cast<int>(foundSpecies.size()), 1, 0.001, 0.001);
        for (std::size_t i = 0; i < foundSpecies.size(); ++i)
        {
            gridCanvas.cd(static_cast<int>(i) + 1);
            gPad->SetTicks(1, 1);
            gPad->SetLeftMargin(0.14);
            gPad->SetRightMargin(0.04);
            gPad->SetBottomMargin(0.13);
            gPad->SetTopMargin(0.10);
            gPad->RedrawAxis();
            drawComparisonPad(rawHists[i], correctedHists[i], foundSpecies[i], i == 0);
        }
        gridCanvas.SaveAs((outdir + "/all_mass_comparisons.png").c_str());
        gridCanvas.SaveAs((outdir + "/all_mass_comparisons.pdf").c_str());
    }

    std::cout << "Saved mass-closure comparison plots to " << outdir << std::endl;

    for (TH1 *hist : rawHists)
        delete hist;
    for (TH1 *hist : correctedHists)
        delete hist;
}
