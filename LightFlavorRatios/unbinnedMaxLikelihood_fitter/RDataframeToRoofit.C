#ifndef RDATAFRAMETOROOFIT_C
#define RDATAFRAMETOROOFIT_C

#include <algorithm>
#include <cmath>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#ifndef __CINT__
#include <RooGlobalFunc.h>
#endif

#include <RooAbsData.h>
#include <RooAbsPdf.h>
#include <RooAddPdf.h>
#include <RooArgList.h>
#include <RooArgSet.h>
#include <RooBinning.h>
#include <RooDataSet.h>
#include <RooFitResult.h>
#include <RooHist.h>
#include <RooMsgService.h>
#include <RooPlot.h>
#include <RooRealVar.h>
#include <RooAbsReal.h>

#include <RooAbsDataHelper.h>

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDFHelpers.hxx>

#include <TCanvas.h>
#include <TCut.h>
#include <TFile.h>
#include <TH1.h>
#include <TLegend.h>
#include <TLine.h>
#include <TLatex.h>
#include <TPad.h>
#include <TSystem.h>
#include <TTree.h>

#include "./fitutil.h"
#include "./plotutil.h"

using namespace RooFit;
using namespace std;

namespace
{
inline void mkdir_p(const std::string &path)
{
    if (path.empty())
        return;
    gSystem->mkdir(path.c_str(), kTRUE);
}

inline std::string dirname_of(const std::string &p)
{
    auto pos = p.find_last_of('/');
    if (pos == std::string::npos)
        return {};
    return p.substr(0, pos);
}

struct SeedGuess
{
    double mean = 0.0;
    double sigma = 0.01;
};

SeedGuess seed_from_dataset(RooAbsData &data, RooRealVar &mass, int nbins, double window_half_width)
{
    SeedGuess out;

    std::unique_ptr<TH1> h(data.createHistogram("h_seed", mass, RooFit::Binning(nbins, mass.getMin(), mass.getMax())));
    if (!h || h->GetEntries() <= 0)
    {
        out.mean = 0.5 * (mass.getMin() + mass.getMax());
        out.sigma = std::max(1e-4, 0.02 * (mass.getMax() - mass.getMin()));
        return out;
    }

    const int imax = h->GetMaximumBin();
    const double peak = h->GetXaxis()->GetBinCenter(imax);

    const double loX = std::max(mass.getMin(), peak - window_half_width);
    const double hiX = std::min(mass.getMax(), peak + window_half_width);

    int lo = std::max(1, h->GetXaxis()->FindBin(loX));
    int hi = std::min(h->GetNbinsX(), h->GetXaxis()->FindBin(hiX));

    double sw = 0.0, sx = 0.0, sx2 = 0.0;
    for (int i = lo; i <= hi; ++i)
    {
        const double x = h->GetXaxis()->GetBinCenter(i);
        const double w = h->GetBinContent(i);
        sw += w;
        sx += w * x;
        sx2 += w * x * x;
    }

    if (sw > 0.0)
    {
        out.mean = sx / sw;
        const double var = std::max(0.0, sx2 / sw - out.mean * out.mean);
        out.sigma = std::sqrt(std::max(var, 1e-12));
    }
    else
    {
        out.mean = peak;
        out.sigma = std::max(1e-4, window_half_width / 3.0);
    }

    const double fullW = mass.getMax() - mass.getMin();
    out.sigma = std::clamp(out.sigma, 1e-4, 0.25 * fullW);
    return out;
}

inline bool fit_is_good(const RooFitResult &fr) { return (fr.status() == 0) && (fr.covQual() >= 2); }

std::unique_ptr<RooFitResult> fit_pdf(RooAbsPdf &pdf, RooAbsData &data, bool extended, int strategy, int printLevel)
{
    RooCmdArg ext = extended ? Extended(true) : Extended(false);

    RooFitResult *r = pdf.fitTo(data, Save(true), ext, Minimizer("Minuit2", "migrad"), Strategy(strategy), Offset(true), PrintLevel(printLevel));
    return std::unique_ptr<RooFitResult>(r);
}

std::string define_sideband_ranges(RooRealVar &mass, double minMass, double maxMass, double mu, double sigma, double gapNSigma)
{
    const double sig = std::max(1e-9, std::abs(sigma));
    const double gap = std::max(1e-9, gapNSigma * sig);

    double lo_hi = std::clamp(mu - gap, minMass, maxMass);
    double hi_lo = std::clamp(mu + gap, minMass, maxMass);

    bool has_lo = (lo_hi > minMass + 1e-9);
    bool has_hi = (maxMass > hi_lo + 1e-9);

    std::string ranges;
    if (has_lo)
    {
        mass.setRange("sb_lo", minMass, lo_hi);
        ranges += "sb_lo";
    }
    if (has_hi)
    {
        mass.setRange("sb_hi", hi_lo, maxMass);
        if (!ranges.empty())
            ranges += ",";
        ranges += "sb_hi";
    }
    return ranges;
}

void configure_signal_params(const fitparam_config &fit_conf)
{
    if (fit_conf.sigmodel == "Gaussian")
    {
        FitParams::mean.setVal(fit_conf.mean);
        FitParams::mean.setRange(fit_conf.mean_low, fit_conf.mean_high);
        FitParams::sigma.setVal(fit_conf.sigma);
        FitParams::sigma.setRange(fit_conf.sigma_low, fit_conf.sigma_high);
    }
    else if (fit_conf.sigmodel == "Voigtian")
    {
        FitParams::mean.setVal(fit_conf.mean);
        FitParams::mean.setRange(fit_conf.mean_low, fit_conf.mean_high);
        FitParams::sigma.setVal(fit_conf.sigma);
        FitParams::sigma.setRange(fit_conf.sigma_low, fit_conf.sigma_high);
        FitParams::width.setVal(fit_conf.width);
        FitParams::width.setRange(fit_conf.width_low, fit_conf.width_high);
    }
    else if (fit_conf.sigmodel == "CrystalBall")
    {
        FitParams::mean.setVal(fit_conf.mean);
        FitParams::mean.setRange(fit_conf.mean_low, fit_conf.mean_high);
        FitParams::sigma.setVal(fit_conf.sigma);
        FitParams::sigma.setRange(fit_conf.sigma_low, fit_conf.sigma_high);
        FitParams::alpha1.setVal(fit_conf.alpha1);
        FitParams::alpha1.setRange(fit_conf.alpha1_low, fit_conf.alpha1_high);
        FitParams::n1.setVal(fit_conf.n1);
        FitParams::n1.setRange(fit_conf.n1_low, fit_conf.n1_high);
    }
    else if (fit_conf.sigmodel == "DoubleCrystalBall")
    {
        FitParams::mean.setVal(fit_conf.mean);
        FitParams::mean.setRange(fit_conf.mean_low, fit_conf.mean_high);
        FitParams::sigma.setVal(fit_conf.sigma);
        FitParams::sigma.setRange(fit_conf.sigma_low, fit_conf.sigma_high);
        FitParams::alpha1.setVal(fit_conf.alpha1);
        FitParams::alpha1.setRange(fit_conf.alpha1_low, fit_conf.alpha1_high);
        FitParams::n1.setVal(fit_conf.n1);
        FitParams::n1.setRange(fit_conf.n1_low, fit_conf.n1_high);
        FitParams::alpha2.setVal(fit_conf.alpha2);
        FitParams::alpha2.setRange(fit_conf.alpha2_low, fit_conf.alpha2_high);
        FitParams::n2.setVal(fit_conf.n2);
        FitParams::n2.setRange(fit_conf.n2_low, fit_conf.n2_high);
        FitParams::frac.setVal(fit_conf.frac);
        FitParams::frac.setRange(0.0, 1.0);
    }
    else
    {
        std::cerr << "[RDataframeToRoofit] ERROR: Unknown signal model type: " << fit_conf.sigmodel << std::endl;
    }
}

void configure_background_params(const fitparam_config &fit_conf)
{
    if (fit_conf.bkgmodel == "Argus")
    {
        FitParams::k.setVal(fit_conf.k);
        FitParams::k.setRange(fit_conf.k_low, fit_conf.k_high);
        FitParams::edp.setVal(fit_conf.edp);
        FitParams::edp.setRange(fit_conf.edp_low, fit_conf.edp_high);
        FitParams::ArgusShift = fit_conf.argusShift;
        FitParams::frac_argus.setVal(fit_conf.argusfrac);
        FitParams::frac_argus.setRange(0.0, 1.0);
        return;
    }

    const int order = FitParams::ParsePolyOrder(fit_conf.bkgmodel);
    if (order < 1)
    {
        std::cerr << "[RDataframeToRoofit] ERROR: Unknown background model type: " << fit_conf.bkgmodel << std::endl;
        return;
    }

    FitParams::p1.setVal(fit_conf.p1);
    FitParams::p1.setRange(fit_conf.p1_low, fit_conf.p1_high);
    if (order >= 2)
    {
        FitParams::p2.setVal(fit_conf.p2);
        FitParams::p2.setRange(fit_conf.p2_low, fit_conf.p2_high);
    }
    if (order >= 3)
    {
        FitParams::p3.setVal(fit_conf.p3);
        FitParams::p3.setRange(fit_conf.p3_low, fit_conf.p3_high);
    }
    if (order >= 4)
    {
        FitParams::p4.setVal(fit_conf.p4);
        FitParams::p4.setRange(fit_conf.p4_low, fit_conf.p4_high);
    }
    if (order >= 5)
    {
        FitParams::p5.setVal(fit_conf.p5);
        FitParams::p5.setRange(fit_conf.p5_low, fit_conf.p5_high);
    }
    if (order >= 6)
    {
        FitParams::p6.setVal(fit_conf.p6);
        FitParams::p6.setRange(fit_conf.p6_low, fit_conf.p6_high);
    }
    if (order >= 7)
    {
        FitParams::p7.setVal(fit_conf.p7);
        FitParams::p7.setRange(fit_conf.p7_low, fit_conf.p7_high);
    }
    if (order >= 8)
    {
        FitParams::p8.setVal(fit_conf.p8);
        FitParams::p8.setRange(fit_conf.p8_low, fit_conf.p8_high);
    }
    if (order >= 9)
    {
        FitParams::p9.setVal(fit_conf.p9);
        FitParams::p9.setRange(fit_conf.p9_low, fit_conf.p9_high);
    }
    if (order >= 10)
    {
        FitParams::p10.setVal(fit_conf.p10);
        FitParams::p10.setRange(fit_conf.p10_low, fit_conf.p10_high);
    }
}

} // namespace

double RDataframeToRoofit(const bool doSnapshot, std::string snapshotName, std::string inputfilename, TCut selections, fitparam_config &fit_conf, const std::string plotdir = "./figure", const std::vector<std::string> &cut_legend_entries = {})
{
    RooMsgService::instance().setGlobalKillBelow(RooFit::ERROR);

    mkdir_p(plotdir);
    if (doSnapshot)
        mkdir_p(dirname_of(snapshotName));

    auto start = std::chrono::high_resolution_clock::now();

    static bool mt_enabled = false;
    if (!mt_enabled)
    {
        ROOT::EnableImplicitMT();
        mt_enabled = true;
    }

    ROOT::RDataFrame df("DecayTree", inputfilename.c_str());
    auto filtered_df = df.Filter(selections.GetTitle());

    FitParams::branch = fit_conf.branch;
    FitParams::minMass = fit_conf.minMass;
    FitParams::maxMass = fit_conf.maxMass;

    FitParams::mass.SetName(FitParams::branch.c_str());
    FitParams::mass.SetTitle("mass");
    FitParams::mass.setRange(FitParams::minMass, FitParams::maxMass);

    auto dsAction = filtered_df.Book<float>(RooDataSetHelper("dataset", "dataset", RooArgSet(FitParams::mass)), {FitParams::branch.c_str()});

    std::vector<ROOT::RDF::RResultHandle> handles;
    handles.emplace_back(dsAction);

    if (doSnapshot)
    {
        auto snapAction = filtered_df.Snapshot("DecayTree", snapshotName.c_str());
        handles.emplace_back(snapAction);
    }

    ROOT::RDF::RunGraphs(handles);

    RooDataSet *dataset = dsAction.GetPtr();
    const int nEntries = dataset ? dataset->numEntries() : 0;
    std::cout << "Number of entries in the dataset: " << nEntries << std::endl;

    if (!dataset || nEntries <= 0)
    {
        std::cerr << "[RDataframeToRoofit] WARNING: Empty dataset after selections.\n";
        return 0.0;
    }

    FitParams::BeginFit();

    configure_signal_params(fit_conf);
    configure_background_params(fit_conf);

    RooAbsPdf *signal = signalModel(fit_conf.sigmodel, FitParams::mass);
    RooAbsPdf *background = backgroundModel(fit_conf.bkgmodel, FitParams::mass);
    if (!signal || !background)
    {
        std::cerr << "[RDataframeToRoofit] ERROR: Failed to build signal/background PDFs.\n";
        return 0.0;
    }

    const double nTot = static_cast<double>(nEntries);

    const double nSigStart = (fit_conf.nSig > 0.0) ? fit_conf.nSig : fit_conf.nSig_fracTol * nTot;
    FitParams::nSig.setVal(std::clamp(nSigStart, 0.0, nTot));
    if (fit_conf.nSig_low > 0.0)
    {
        FitParams::nSig.setRange(fit_conf.nSig_low, nTot);
    }

    const double nBkgStart = (fit_conf.nBkg > 0.0) ? fit_conf.nBkg : fit_conf.nBkg_fracTol * nTot;
    FitParams::nBkg.setVal(std::clamp(nBkgStart, 0.0, nTot));
    if (fit_conf.nBkg_low > 0.0)
    {
        FitParams::nBkg.setRange(fit_conf.nBkg_low, nTot);
    }

    RooAddPdf model("model", "model", RooArgList(*signal, *background), RooArgList(FitParams::nSig, FitParams::nBkg));

    const double fullW = FitParams::maxMass - FitParams::minMass;
    SeedGuess seed0 = {FitParams::mean.getVal(), FitParams::sigma.getVal()};
    if (!(seed0.sigma > 0.0) || seed0.sigma > 0.3 * fullW)
    {
        const double win = std::min(0.06, 0.25 * fullW);
        seed0 = seed_from_dataset(*dataset, FitParams::mass, std::max(40, fit_conf.nBins), win);
        FitParams::mean.setVal(seed0.mean);
        FitParams::sigma.setVal(seed0.sigma);
    }

    std::unique_ptr<RooFitResult> bkg_prefit;
    if (fit_conf.useStagedFit)
    {
        const std::string sbRanges = define_sideband_ranges(FitParams::mass, FitParams::minMass, FitParams::maxMass, seed0.mean, seed0.sigma, fit_conf.stagedGapNSigma);

        if (!sbRanges.empty())
        {
            std::unique_ptr<RooAbsData> sbData(dataset->reduce(CutRange(sbRanges.c_str())));
            if (sbData && sbData->numEntries() > 0)
            {
                bkg_prefit = fit_pdf(*background, *sbData, false, 1, -1);
                std::cout << "[RDataframeToRoofit] Sideband bkg prefit: status=" << bkg_prefit->status() << ", covQual=" << bkg_prefit->covQual() << "\n";
            }
            else
            {
                std::cout << "[RDataframeToRoofit] Sideband bkg prefit skipped (empty sideband data).\n";
            }
        }
        else
        {
            std::cout << "[RDataframeToRoofit] Sideband bkg prefit skipped (sidebands not usable).\n";
        }
    }

    std::unique_ptr<RooFitResult> fitres = fit_pdf(model, *dataset, true, 1, -1);

    if (!fit_is_good(*fitres))
    {
        std::cerr << "[RDataframeToRoofit] WARNING: Full fit not good (status=" << fitres->status() << ", covQual=" << fitres->covQual() << "). Reseed + retry.\n";

        const double win = std::min(0.06, 0.25 * fullW);
        SeedGuess seed = seed_from_dataset(*dataset, FitParams::mass, std::max(40, fit_conf.nBins), win);

        FitParams::mean.setVal(seed.mean);
        FitParams::mean.setRange(std::max(FitParams::minMass, seed.mean - 0.5 * win), std::min(FitParams::maxMass, seed.mean + 0.5 * win));

        FitParams::sigma.setVal(seed.sigma);
        FitParams::sigma.setRange(fit_conf.sigma_low, fit_conf.sigma_high);

        if (fit_conf.useStagedFit)
        {
            const std::string sbRanges = define_sideband_ranges(FitParams::mass, FitParams::minMass, FitParams::maxMass, seed.mean, seed.sigma, fit_conf.stagedGapNSigma);

            if (!sbRanges.empty())
            {
                std::unique_ptr<RooAbsData> sbData(dataset->reduce(CutRange(sbRanges.c_str())));
                if (sbData && sbData->numEntries() > 0)
                    bkg_prefit = fit_pdf(*background, *sbData, false, 2, -1);
            }
        }

        std::unique_ptr<RooFitResult> fitres2 = fit_pdf(model, *dataset, true, 2, -1);

        const bool good1 = fit_is_good(*fitres);
        const bool good2 = fit_is_good(*fitres2);

        if (good2 && !good1)
            fitres.swap(fitres2);
        else if (!good1 && !good2 && fitres2->minNll() < fitres->minNll())
            fitres.swap(fitres2);
    }

    fitres->Print();

    {
        TFile out(Form("%s/fitresults.root", plotdir.c_str()), "RECREATE");
        out.cd();
        fitres->Write("fitres");
        if (bkg_prefit)
            bkg_prefit->Write("bkg_prefit_sidebands");
    }

    const double mu = FitParams::mean.getVal();
    const double sg = FitParams::sigma.getVal();

    double lo = std::max(FitParams::minMass, mu - 3.0 * sg);
    double hi = std::min(FitParams::maxMass, mu + 3.0 * sg);
    if (!(hi > lo))
    {
        lo = FitParams::minMass;
        hi = FitParams::maxMass;
    }

    FitParams::mass.setRange("integralrange", lo, hi);

    std::unique_ptr<RooAbsReal> signalFrac(signal->createIntegral(FitParams::mass, NormSet(FitParams::mass), Range("integralrange")));
    std::unique_ptr<RooAbsReal> bkgFrac(background->createIntegral(FitParams::mass, NormSet(FitParams::mass), Range("integralrange")));

    const double S = signalFrac ? signalFrac->getVal() * FitParams::nSig.getVal() : 0.0;
    const double B = bkgFrac ? bkgFrac->getVal() * FitParams::nBkg.getVal() : 0.0;
    const double significance = (S + B > 0.0) ? (S / std::sqrt(S + B)) : 0.0;

    std::string xAxisTitle = fit_conf.decaystring + " candidate mass [GeV]";

    RooBinning bins(FitParams::minMass, FitParams::maxMass);
    bins.addUniform(fit_conf.nBins, FitParams::minMass, FitParams::maxMass);

    RooPlot *frame_display = FitParams::mass.frame(Title(""));
    RooPlot *frame_calc = FitParams::mass.frame(Title(""));

    dataset->plotOn(frame_calc, Binning(bins), DataError(RooAbsData::SumW2), Name("data_calc"));
    model.plotOn(frame_calc, LineColor(kBlack), Name("total_fit_calc"));
    const double chi2ndf = frame_calc->chiSquare("total_fit_calc", "data_calc");

    RooHist *pull = frame_calc->pullHist("data_calc", "total_fit_calc");
    RooPlot *frame_pull = FitParams::mass.frame(Title(""));
    frame_pull->addPlotable(pull, "PE1");

    dataset->plotOn(frame_display, Binning(bins), XErrorSize(0), DataError(RooAbsData::SumW2), Name("data_binned"));
    model.plotOn(frame_display, Components(*background), LineColor(kGray), DrawOption("F"), FillColor(kGray), Name("bkg"));
    model.plotOn(frame_display, Components(RooArgSet(*signal, *background)), LineColor(kAzure + 8), DrawOption("F"), FillColor(kAzure + 8), MoveToBack(), Name("sig_plus_bkg"));
    model.plotOn(frame_display, LineColor(kBlack), Name("total_fit"));
    dataset->plotOn(frame_display, DrawOption("PE1"), Binning(bins), XErrorSize(0), DataError(RooAbsData::SumW2), Name("data"));

    TCanvas *c = new TCanvas("massFitCanvas", "massFitCanvas", 800, 800);

    TPad mainPad("mainPad", "mainPad", 0., 0.3, 1., 1.);
    mainPad.SetTopMargin(TopMargin);
    mainPad.SetBottomMargin(0);
    mainPad.Draw();

    TPad pullPad("pullPad", "pullPad", 0., 0.0, 1., 0.3);
    pullPad.SetBottomMargin(0.5);
    pullPad.SetTopMargin(0);
    pullPad.Draw();

    mainPad.cd();
    frame_display->SetMarkerStyle(kCircle);
    frame_display->SetMarkerSize(0.02);
    frame_display->SetLineWidth(1);
    frame_display->GetXaxis()->SetTitleSize(0);
    frame_display->GetXaxis()->SetLabelSize(0);

    frame_display->GetYaxis()->SetTitleSize(AxisTitleSize * textscale_pad1);
    frame_display->GetYaxis()->SetLabelSize(AxisLabelSize * textscale_pad1);
    frame_display->GetYaxis()->SetTitleOffset(1.2);
    frame_display->GetYaxis()->SetTitleFont(42);
    frame_display->GetYaxis()->SetLabelFont(42);

    std::unique_ptr<TH1> hdataset(dataset->createHistogram("hdataset", FitParams::mass, Binning(fit_conf.nBins, FitParams::minMass, FitParams::maxMass)));
    frame_display->GetYaxis()->SetRangeUser(0.1, hdataset ? hdataset->GetMaximum() * 1.8 : 1.0);

    float binWidth = 1000.f * float(FitParams::maxMass - FitParams::minMass) / float(fit_conf.nBins);
    string yAxisTitle = "Candidates / (" + to_string_with_precision(binWidth, 1) + " MeV)";
    frame_display->GetYaxis()->SetTitle(yAxisTitle.c_str());
    frame_display->Draw();
    c->RedrawAxis();

    TLatex *datestamp = new TLatex();
    datestamp->SetTextSize(0.06);
    datestamp->SetTextAlign(kHAlignRight + kVAlignBottom);
    datestamp->SetNDC();
    datestamp->DrawLatex(1 - mainPad.GetRightMargin(), 1 - mainPad.GetTopMargin() + 0.01, getTime().c_str());

    TLegend *sphnxleg = new TLegend(mainPad.GetLeftMargin() + 0.03, 1 - mainPad.GetTopMargin() - 0.2, mainPad.GetLeftMargin() + 0.2, 1 - mainPad.GetTopMargin() - 0.05);
    sphnxleg->SetTextAlign(kHAlignLeft + kVAlignCenter);
    sphnxleg->SetTextSize(0.06);
    sphnxleg->SetFillStyle(0);
    sphnxleg->AddEntry("", Form("#it{#bf{sPHENIX}} %s", prelimtext.c_str()), "");
    sphnxleg->AddEntry("", "p+p #sqrt{s_{NN}}=200 GeV", "");
    sphnxleg->Draw();

    TLegend *leg = new TLegend(1 - mainPad.GetRightMargin() - 0.33, 1 - mainPad.GetTopMargin() - 0.33, 1 - mainPad.GetRightMargin() - 0.1, 1 - mainPad.GetTopMargin() - 0.06);
    leg->AddEntry(frame_display->findObject("data"), "Data", "PE2");
    leg->AddEntry(frame_display->findObject("total_fit"), "Fit", "L");
    leg->AddEntry(frame_display->findObject("sig_plus_bkg"), fit_conf.decaystring.c_str(), "f");
    leg->AddEntry(frame_display->findObject("bkg"), "Comb. Bkg.", "f");
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.055);
    leg->Draw();

    TLatex *text = new TLatex();
    text->SetTextSize(0.045);
    text->SetTextAlign(kHAlignLeft + kVAlignCenter);
    text->SetNDC();

    double y0 = 1 - mainPad.GetTopMargin() - 0.25;
    text->DrawLatex(mainPad.GetLeftMargin() + 0.07, y0, Form("S/#sqrt{S+B} = %.2f", significance));
    text->DrawLatex(mainPad.GetLeftMargin() + 0.07, y0 - 0.06, Form("#mu = %.0f #pm %.0f MeV", FitParams::mean.getVal() * 1000.0, FitParams::mean.getError() * 1000.0));
    text->DrawLatex(mainPad.GetLeftMargin() + 0.07, y0 - 0.115, Form("#sigma = %.2f #pm %.2f MeV", FitParams::sigma.getVal() * 1000.0, FitParams::sigma.getError() * 1000.0));
    text->DrawLatex(mainPad.GetLeftMargin() + 0.07, y0 - 0.170, Form("Yield = %.0f #pm %.0f", FitParams::nSig.getVal(), FitParams::nSig.getError()));
    // text->DrawLatex(mainPad.GetLeftMargin() + 0.07, y0 - 0.225, Form("#chi^{2}/ndf #approx %.2f", chi2ndf));

    if (!cut_legend_entries.empty())
    {
        const double cutTop = 1 - mainPad.GetTopMargin() - 0.35;
        const double cutBottom = cutTop - 0.045*cut_legend_entries.size();
        TLegend *cutleg = new TLegend(1 - mainPad.GetRightMargin() - 0.33, cutBottom, 1 - mainPad.GetRightMargin() - 0.1, cutTop);
        cutleg->SetFillColor(0);
        cutleg->SetFillStyle(0);
        cutleg->SetBorderSize(0);
        cutleg->SetTextSize(0.04);
        cutleg->SetTextAlign(kHAlignLeft + kVAlignCenter);
        for (const auto &entry : cut_legend_entries)
            cutleg->AddEntry("", entry.c_str(), "");
        cutleg->Draw();
    }

    // TLatex *qual = new TLatex();
    // qual->SetTextSize(0.04);
    // qual->SetTextAlign(kHAlignRight + kVAlignTop);
    // qual->SetNDC();
    // qual->DrawLatex(1 - mainPad.GetRightMargin(), 1 - mainPad.GetTopMargin() - 0.02, Form("status=%d, covQual=%d", fitres->status(), fitres->covQual()));

    pullPad.cd();
    frame_pull->SetMarkerStyle(kCircle);
    frame_pull->SetMarkerSize(0.02);
    frame_pull->SetTitle("");
    frame_pull->GetXaxis()->SetTitle(xAxisTitle.c_str());
    frame_pull->GetXaxis()->SetTitleOffset(1.3);
    frame_pull->GetXaxis()->SetTitleFont(42);
    frame_pull->GetXaxis()->SetTitleSize(AxisTitleSize * textscale_pad2);
    frame_pull->GetXaxis()->SetLabelFont(42);
    frame_pull->GetXaxis()->SetLabelSize(AxisLabelSize * textscale_pad2);

    frame_pull->GetYaxis()->SetTitle("Pull");
    frame_pull->GetYaxis()->SetTitleOffset(0.5);
    frame_pull->GetYaxis()->SetTitleFont(42);
    frame_pull->GetYaxis()->SetTitleSize(AxisTitleSize * textscale_pad2);
    frame_pull->GetYaxis()->SetLabelFont(42);
    frame_pull->GetYaxis()->SetLabelSize(AxisLabelSize * textscale_pad2);
    frame_pull->GetYaxis()->SetRangeUser(-6, 6);
    frame_pull->GetYaxis()->SetNdivisions(5);

    frame_pull->Draw();

    TLine *plusThreeLine = new TLine(FitParams::minMass, 3, FitParams::maxMass, 3);
    plusThreeLine->SetLineColor(1);
    plusThreeLine->SetLineStyle(2);
    plusThreeLine->SetLineWidth(2);
    plusThreeLine->Draw("same");
    TLine *zeroLine = new TLine(FitParams::minMass, 0, FitParams::maxMass, 0);
    zeroLine->SetLineColor(1);
    zeroLine->SetLineStyle(2);
    zeroLine->SetLineWidth(2);
    zeroLine->Draw("same");
    TLine *minusThreeLine = new TLine(FitParams::minMass, -3, FitParams::maxMass, -3);
    minusThreeLine->SetLineColor(1);
    minusThreeLine->SetLineStyle(2);
    minusThreeLine->SetLineWidth(2);
    minusThreeLine->Draw("same");

    const std::vector<std::string> exts = {".C", ".pdf", ".png"};
    for (const auto &ext : exts)
        c->SaveAs(Form("%s/RDataframeToRoofit_%s%s", plotdir.c_str(), FitParams::branch.c_str(), ext.c_str()));

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() / 60.0 << " minutes\n";

    return significance;
}

#endif // RDATAFRAMETOROOFIT_C
