#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "TCanvas.h"
#include "TFile.h"
#include "TF1.h"
#include "TGaxis.h"
#include "TGraphAsymmErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TTree.h"

TGaxis::SetMaxDigits(3);

static double median1D(std::vector<double> v)
{
    if (v.empty())
        return 0.0;
    std::sort(v.begin(), v.end());
    const size_t n = v.size();
    if (n % 2 == 1)
        return v[n / 2];
    return 0.5 * (v[n / 2 - 1] + v[n / 2]);
}

static double histogramMedian(TH1 *hist)
{
    if (!hist || hist->GetEntries() == 0)
        return 0.0;

    double median = 0.0;
    double probability = 0.5;
    hist->GetQuantiles(1, &median, &probability);
    return median;
}

std::vector<double> trackMultiplicityBinEdges = {0, 25, 50, 100, 150, 200, 250, 300, 400, 600, 1000};

static TH1F *makeVertexSummaryHist(const std::string &name, const std::string &title, const std::vector<std::string> &binLabels)
{
    TH1F *hist = new TH1F(name.c_str(), title.c_str(), binLabels.size(), 0.5, binLabels.size() + 0.5);
    hist->SetStats(0);
    for (size_t ibin = 0; ibin < binLabels.size(); ++ibin)
    {
        hist->GetXaxis()->SetBinLabel(ibin + 1, binLabels[ibin].c_str());
    }
    return hist;
}

static TGraphAsymmErrors *makeVertexSummaryVsTrackGraph(TH1 *summaryHist, const std::vector<bool> &validFits, const std::string &name)
{
    TGraphAsymmErrors *graph = new TGraphAsymmErrors();
    graph->SetName(name.c_str());
    for (int ibin = 1; ibin <= summaryHist->GetNbinsX(); ++ibin)
    {
        const size_t index = ibin - 1;
        if (index + 1 >= trackMultiplicityBinEdges.size() || index >= validFits.size() || !validFits[index])
            continue;

        const double x = 0.5 * (trackMultiplicityBinEdges[index] + trackMultiplicityBinEdges[index + 1]);
        const double y = summaryHist->GetBinContent(ibin);
        const int point = graph->GetN();
        graph->SetPoint(point, x, y);
        graph->SetPointError(point, x - trackMultiplicityBinEdges[index], trackMultiplicityBinEdges[index + 1] - x, summaryHist->GetBinError(ibin), summaryHist->GetBinError(ibin));
    }
    return graph;
}

static void drawVertexSummaryVsTrackBin(TH1 *summaryHist,                        //
                                        const std::vector<bool> &validFits,      //
                                        const std::string &graphName,            //
                                        const std::string &ytitle,               //
                                        const std::pair<float, float> &yrange,   //
                                        const std::vector<std::string> &addinfo, //
                                        const std::string &filename              //
)
{
    if (!summaryHist)
        return;

    TGaxis::SetMaxDigits(5);

    TCanvas *c = new TCanvas("c_vertexWidth", "c_vertexWidth", 900, 800);
    c->cd();
    gPad->SetLeftMargin(0.2);
    gPad->SetRightMargin(0.05);
    gPad->SetTopMargin(0.07);

    TGraphAsymmErrors *graph = makeVertexSummaryVsTrackGraph(summaryHist, validFits, graphName);
    TH1F *frame = new TH1F(Form("frame_%s", graphName.c_str()), "", 100, trackMultiplicityBinEdges.front(), trackMultiplicityBinEdges.back());
    frame->SetStats(0);
    frame->GetXaxis()->SetTitle("N_{silicon seeds} associated to vertex");
    frame->GetYaxis()->SetTitle(ytitle.c_str());
    frame->GetYaxis()->SetTitleOffset(2.0);
    frame->GetYaxis()->SetRangeUser(yrange.first, yrange.second);
    frame->Draw("axis");

    graph->SetLineColor(kBlack);
    graph->SetLineWidth(2);
    graph->SetMarkerColor(kBlack);
    graph->SetMarkerStyle(kFullCircle);
    graph->SetMarkerSize(1.1);
    graph->Draw("PZ same");

    TLegend *leg = new TLegend(gPad->GetLeftMargin() + 0.03, 1 - gPad->GetTopMargin() - 0.03 - 0.04 * (addinfo.size() + 1), gPad->GetLeftMargin() + 0.2, 1 - gPad->GetTopMargin() - 0.03);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.035);
    for (size_t i = 0; i < addinfo.size(); ++i)
    {
        leg->AddEntry((TObject *)0, addinfo[i].c_str(), "");
    }
    leg->Draw();

    c->SaveAs((filename + ".png").c_str());
    c->SaveAs((filename + ".pdf").c_str());
    delete frame;
    delete graph;
    delete c;

    TGaxis::SetMaxDigits(3);
}

static void draw2Dhistogram(TH2 *hist,                               //
                            const std::string &xtitle,               //
                            const std::string &ytitle,               //
                            const std::vector<std::string> &addinfo, //
                            const std::string &filename              //
)
{
    if (!hist)
        return;

    TCanvas *c = new TCanvas("c_2d", "c_2d", 900, 800);
    c->cd();
    gPad->SetLeftMargin(0.14);
    gPad->SetRightMargin(0.15);
    gPad->SetTopMargin(0.08);

    hist->SetTitle("");
    hist->GetXaxis()->SetTitle(xtitle.c_str());
    hist->GetYaxis()->SetTitle(ytitle.c_str());
    hist->GetYaxis()->SetTitleOffset(1.4);
    hist->Draw("COLZ");

    TLegend *leg = new TLegend(gPad->GetLeftMargin() + 0.03, 1 - gPad->GetTopMargin() - 0.03 - 0.04 * addinfo.size(), gPad->GetLeftMargin() + 0.24, 1 - gPad->GetTopMargin() - 0.03);
    for (size_t i = 0; i < addinfo.size(); ++i)
    {
        leg->AddEntry((TObject *)0, addinfo[i].c_str(), "");
    }
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.035);
    leg->Draw();

    c->SaveAs((filename + ".png").c_str());
    c->SaveAs((filename + ".pdf").c_str());
    delete c;
}

static void draw1Dhistogram(TH1 *hist,                                 //
                            const bool logy,                           //
                            const std::string &xtitle,                 //
                            const std::string &ytitle,                 //
                            const std::vector<std::string> &addinfo,   //
                            const std::string &drawoption = "hist e1", //
                            const std::string &filename = "plot1d")
{
    if (!hist)
        return;

    TCanvas *c = new TCanvas("c_1d", "c_1d", 900, 800);
    c->SetLogy(logy);
    c->cd();
    gPad->SetLeftMargin(0.14);
    gPad->SetRightMargin(0.05);
    gPad->SetTopMargin(0.08);

    hist->SetTitle("");
    hist->GetXaxis()->SetTitle(xtitle.c_str());
    hist->GetYaxis()->SetTitle(ytitle.c_str());
    hist->GetYaxis()->SetTitleOffset(1.4);
    hist->GetYaxis()->SetRangeUser((logy ? hist->GetMinimum(0) * 0.5 : 0.0), hist->GetMaximum() * (logy ? 10.0 : 1.4));
    hist->SetLineWidth(2);
    hist->SetLineColor(kBlack);
    hist->SetMarkerStyle(kFullCircle);
    hist->SetMarkerSize(1.0);
    hist->SetMarkerColor(kBlack);
    hist->Draw(drawoption.c_str());

    TLegend *leg = new TLegend(gPad->GetLeftMargin() + 0.03,                              //
                               1 - gPad->GetTopMargin() - 0.03 - 0.04 * (addinfo.size()), //
                               gPad->GetLeftMargin() + 0.2,                               //
                               1 - gPad->GetTopMargin() - 0.03                            //
    );
    for (size_t i = 0; i < addinfo.size(); ++i)
    {
        leg->AddEntry((TObject *)0, addinfo[i].c_str(), "");
    }
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.035);
    leg->Draw();

    c->SaveAs((filename + ".png").c_str());
    c->SaveAs((filename + ".pdf").c_str());
    delete c;
}

struct fitConfig
{
    std::string fitFunction = "[0] + [1]*exp(-0.5*((x-[2])/[3])^2)";                                                      // or "[0] + [1]*exp(-0.5*((x-[2])/[3])^2)" for Gaussian + constant background
    std::vector<double> initialParams = {100.0, 1000.0, 0.0, 0.1};                                                        // initial parameter guesses for the fit function
    std::vector<std::pair<int, std::string>> paramNames = {{0, "Constant"}, {1, "Amplitude"}, {2, "Mean"}, {3, "Sigma"}}; // names for the fit parameters (for legend)
    std::map<std::string, double> postFitParams;                                                                          // to store post-fit parameter values for later use
    std::map<std::string, double> postFitParamErrors;                                                                     // to store post-fit parameter errors for later use
    int fitStatus = -1;
    std::pair<double, double> fitRange = {-0.5, 0.5}; // fit range for the x-axis
};

static void draw1Dhistogram_GaussianFit(TH1 *hist,                                 //
                                        const bool logy,                           //
                                        fitConfig &config,                         //
                                        const std::string &xtitle,                 //
                                        const std::string &ytitle,                 //
                                        const std::vector<std::string> &addinfo,   //
                                        const std::string &drawoption = "hist e1", //
                                        const std::string &filename = "plot1d")
{
    if (!hist)
        return;

    TCanvas *c = new TCanvas("c_1d", "c_1d", 900, 800);
    c->SetLogy(logy);
    c->cd();
    gPad->SetLeftMargin(0.14);
    gPad->SetRightMargin(0.05);
    gPad->SetTopMargin(0.08);

    hist->SetTitle("");
    hist->GetXaxis()->SetTitle(xtitle.c_str());
    hist->GetYaxis()->SetTitle(ytitle.c_str());
    hist->GetYaxis()->SetTitleOffset(1.4);
    hist->GetYaxis()->SetRangeUser((logy ? hist->GetMinimum(0) * 0.5 : 0.0), hist->GetMaximum() * (logy ? 10.0 : 1.4));
    hist->SetLineWidth(2);
    hist->SetLineColor(kBlack);
    hist->SetMarkerStyle(kFullCircle);
    hist->SetMarkerSize(1.0);
    hist->SetMarkerColor(kBlack);
    hist->Draw(drawoption.c_str());

    TF1 *gausFit = new TF1("gausFit", config.fitFunction.data(), config.fitRange.first, config.fitRange.second);
    for (size_t i = 0; i < config.initialParams.size(); ++i)
    {
        gausFit->SetParameter(i, config.initialParams[i]);
        gausFit->SetParName(i, config.paramNames[i].second.c_str());
        if (config.paramNames[i].second == "Mean")
        {
            gausFit->SetParLimits(i, config.fitRange.first, config.fitRange.second); // limit mean to fit range
        }
        else if (config.paramNames[i].second == "Sigma")
        {
            gausFit->SetParLimits(i, 0.007, config.initialParams[i] * 10.0); // limit sigma to reasonable positive values
        }
        else
        {
            gausFit->SetParLimits(i, 0.0, config.initialParams[i] * 10.0); // set limits to help convergence, can be adjusted as needed
        }
    }
    config.fitStatus = hist->Fit(gausFit, "RQM");
    gausFit->SetLineColor(kRed);
    gausFit->SetLineWidth(2);
    gausFit->Draw("same");

    std::cout << "Fit parameters:\n";
    for (const auto &param : config.paramNames)
    {
        const std::string &name = param.second;
        config.postFitParams[name] = gausFit->GetParameter(name.c_str());
        config.postFitParamErrors[name] = gausFit->GetParError(param.first);
        std::cout << "  " << name << ": " << config.postFitParams[name] << " +/- " << config.postFitParamErrors[name] << "\n";
    }

    TLegend *leg = new TLegend(gPad->GetLeftMargin() + 0.05,                                  //
                               1 - gPad->GetTopMargin() - 0.03 - 0.04 * (addinfo.size() + 2), //
                               gPad->GetLeftMargin() + 0.2,                                   //
                               1 - gPad->GetTopMargin() - 0.03                                //
    );
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.035);
    leg->AddEntry(hist, "Data", "pel");
    leg->AddEntry(gausFit, Form("Gaussian Fit: #mu = %.3g [cm], #sigma = %.3g [cm]", gausFit->GetParameter("Mean"), gausFit->GetParameter("Sigma")), "l");
    for (size_t i = 0; i < addinfo.size(); ++i)
    {
        leg->AddEntry((TObject *)0, addinfo[i].c_str(), "");
    }
    leg->Draw();

    c->SaveAs((filename + ".png").c_str());
    c->SaveAs((filename + ".pdf").c_str());
    delete c;
}

void beamspot_2DJointMedian(int runNumber = 82391)
{
    std::string plotdir = "./figure/figure-SvtxMbdVertex/" + std::to_string(runNumber) + "/";
    system(("mkdir -p " + plotdir).c_str());
    
    std::string filename = "/sphenix/tg/tg01/hf/hjheng/ppg-dNdEta-OOpp/data/" + std::to_string(runNumber) + "/Ntuples/CombineNtuple_Acts_0.root"; //! TODO: make it argument for condor job

    std::vector<std::pair<double, double>> pts;
    std::vector<double> vtxZ;
    TH2 *hM_vertexXY = new TH2F("hM_vertexXY", "Vertex XY positions;vertexX [cm];vertexY [cm]", 200, -0.5, 0.5, 200, -0.5, 0.5);
    TH1 *hM_vertexX = new TH1F("hM_vertexX", "Vertex X positions;vertexX [cm];Counts", 200, -0.5, 0.5);
    TH1 *hM_vertexY = new TH1F("hM_vertexY", "Vertex Y positions;vertexY [cm];Counts", 200, -0.5, 0.5);
    TH1 *hM_vertexZ = new TH1F("hM_vertexZ", "Vertex Z positions;vertexZ [cm];Counts", 60, -30, 30);
    TH1 *hM_vertexZ_TrgCrossing = new TH1F("hM_vertexZ_trgCrossing", "Vertex Z positions for different trigger crossings;vertexZ [cm];Counts", 60, -30, 30);
    TH1 *hM_vertexZ_nonTrgCrossing = new TH1F("hM_vertexZ_nonTrgCrossing", "Vertex Z positions for non-trigger crossings;vertexZ [cm];Counts", 60, -30, 30);
    TH1 *hM_MBDVertexZ = new TH1F("hM_MBDVertexZ", "MBD Vertex Z positioon;MBD vertexZ [cm];Counts", 60, -120, 120);
    TH1 *hM_MBDVertexZ_trg14 = new TH1F("hM_MBDVertexZ_trg14", "MBD Vertex Z positioon for trigger 14;MBD vertexZ [cm];Counts", 60, -120, 120);
    TH1 *hM_MBDVertexZ_trg12 = new TH1F("hM_MBDVertexZ_trg12", "MBD Vertex Z positioon for trigger 12;MBD vertexZ [cm];Counts", 120, -60, 60);
    TH1 *hM_MBDVertexZ_trg12or14 = new TH1F("hM_MBDVertexZ_trg12or14", "MBD Vertex Z positioon for trigger 12 or 14;MBD vertexZ [cm];Counts", 120, -60, 60);


    TH1 *hM_triggerbits = new TH1F("hM_triggerbits", "Fired trigger bits;Trigger bit;Counts", 41, -0.5, 40.5);

    std::vector<std::string> trackBinLabels;
    trackBinLabels.reserve(trackMultiplicityBinEdges.size() - 1);
    for (size_t ibin = 0; ibin + 1 < trackMultiplicityBinEdges.size(); ++ibin)
    {
        if (ibin + 2 == trackMultiplicityBinEdges.size())
        {
            trackBinLabels.push_back(Form(">%g", trackMultiplicityBinEdges[ibin]));
        }
        else if (ibin + 3 == trackMultiplicityBinEdges.size())
        {
            trackBinLabels.push_back(Form("[%g,%g]", trackMultiplicityBinEdges[ibin], trackMultiplicityBinEdges[ibin + 1]));
        }
        else
        {
            trackBinLabels.push_back(Form("[%g,%g)", trackMultiplicityBinEdges[ibin], trackMultiplicityBinEdges[ibin + 1]));
        }
    }

    std::vector<TH1 *> hM_vertexXByNTracks(trackBinLabels.size(), nullptr);
    std::vector<TH1 *> hM_vertexYByNTracks(trackBinLabels.size(), nullptr);
    std::vector<TH2 *> hM_vertexXYByNTracks(trackBinLabels.size(), nullptr);
    std::vector<std::vector<double>> vertexXByNTracks(trackBinLabels.size());
    std::vector<std::vector<double>> vertexYByNTracks(trackBinLabels.size());
    std::vector<bool> validWidthX(trackBinLabels.size(), false);
    std::vector<bool> validWidthY(trackBinLabels.size(), false);
    TH1F *hM_vertexWidthX_vs_NTracks = makeVertexSummaryHist("hM_vertexWidthX_vs_NTracks", "Vertex X fit width vs N tracks/seeds;N tracks/seeds associated to vertex;#sigma_{X} [cm]", trackBinLabels);
    TH1F *hM_vertexWidthY_vs_NTracks = makeVertexSummaryHist("hM_vertexWidthY_vs_NTracks", "Vertex Y fit width vs N tracks/seeds;N tracks/seeds associated to vertex;#sigma_{Y} [cm]", trackBinLabels);
    TH1F *hM_vertexMeanX_vs_NTracks = makeVertexSummaryHist("hM_vertexMeanX_vs_NTracks", "Vertex X fit mean vs N tracks/seeds;N tracks/seeds associated to vertex;#mu_{X} [cm]", trackBinLabels);
    TH1F *hM_vertexMeanY_vs_NTracks = makeVertexSummaryHist("hM_vertexMeanY_vs_NTracks", "Vertex Y fit mean vs N tracks/seeds;N tracks/seeds associated to vertex;#mu_{Y} [cm]", trackBinLabels);
    TH1F *hM_beamspotSummary = new TH1F("hM_beamspotSummary", "Beamspot summary;;value", 13, 0.5, 13.5);
    hM_beamspotSummary->SetStats(0);
    hM_beamspotSummary->GetXaxis()->SetBinLabel(1, "Run number");
    hM_beamspotSummary->GetXaxis()->SetBinLabel(2, "Beamspot X [cm]");
    hM_beamspotSummary->GetXaxis()->SetBinLabel(3, "Beamspot Y [cm]");
    hM_beamspotSummary->GetXaxis()->SetBinLabel(4, "Beam spread X [cm]");
    hM_beamspotSummary->GetXaxis()->SetBinLabel(5, "Beam spread Y [cm]");
    hM_beamspotSummary->GetXaxis()->SetBinLabel(6, "MBD Z mean trg12 [cm]");
    hM_beamspotSummary->GetXaxis()->SetBinLabel(7, "MBD Z sigma trg12 [cm]");
    hM_beamspotSummary->GetXaxis()->SetBinLabel(8, "MBD Z mean trg12or14 [cm]");
    hM_beamspotSummary->GetXaxis()->SetBinLabel(9, "MBD Z sigma trg12or14 [cm]");
    hM_beamspotSummary->GetXaxis()->SetBinLabel(10, "MBD Z mean trg14 [cm]");
    hM_beamspotSummary->GetXaxis()->SetBinLabel(11, "MBD Z sigma trg14 [cm]");
    hM_beamspotSummary->GetXaxis()->SetBinLabel(12, "MBD Z mean inclusive [cm]");
    hM_beamspotSummary->GetXaxis()->SetBinLabel(13, "MBD Z sigma inclusive [cm]");
    for (size_t ibin = 0; ibin < trackBinLabels.size(); ++ibin)
    {
        hM_vertexXByNTracks[ibin] = new TH1F(Form("hM_vertexX_NTracks_bin%zu", ibin), Form("Vertex X for NTracks %s;vertexX [cm];Counts", trackBinLabels[ibin].c_str()), 200, -0.5, 0.5);
        hM_vertexYByNTracks[ibin] = new TH1F(Form("hM_vertexY_NTracks_bin%zu", ibin), Form("Vertex Y for NTracks %s;vertexY [cm];Counts", trackBinLabels[ibin].c_str()), 200, -0.5, 0.5);
        hM_vertexXYByNTracks[ibin] = new TH2F(Form("hM_vertexXY_NTracks_bin%zu", ibin), Form("Vertex XY for NTracks %s;vertexX [cm];vertexY [cm]", trackBinLabels[ibin].c_str()), 200, -0.5, 0.5, 200, -0.5, 0.5);
    }

    TFile *file = TFile::Open(filename.c_str(), "READ");
    if (!file || file->IsZombie())
    {
        std::cout << "Failed to open file: " << filename << std::endl;
        return;
    }

    TTree *tree = static_cast<TTree *>(file->Get("VTX"));
    if (!tree)
    {
        std::cout << "Failed to get TTree VTX from: " << filename << std::endl;
        return;
    }

    tree->SetBranchStatus("*", 0);
    const std::vector<std::string> enabledBranches = {
        "counter", "mbdVertex", "gl1bco", "bcotr", "firedTriggers", "trackerVertexX", "trackerVertexY", "trackerVertexZ", "trackerVertexCrossing", "trackerVertexNTracks",
    };
    for (const auto &branchName : enabledBranches)
    {
        tree->SetBranchStatus(branchName.c_str(), 1);
    }

    int counter;
    std::vector<float> *mbdVertex = 0;
    uint64_t gl1bco, bcotr;
    std::vector<int> *firedTriggers = 0;
    std::vector<float> *trackerVertexX = 0;
    std::vector<float> *trackerVertexY = 0;
    std::vector<float> *trackerVertexZ = 0;
    std::vector<short> *trackerVertexCrossing = 0;
    std::vector<int> *trackerVertexNTracks = 0;
    tree->SetBranchAddress("counter", &counter);
    tree->SetBranchAddress("mbdVertex", &mbdVertex);
    tree->SetBranchAddress("gl1bco", &gl1bco);
    tree->SetBranchAddress("bcotr", &bcotr);
    tree->SetBranchAddress("firedTriggers", &firedTriggers);
    tree->SetBranchAddress("trackerVertexX", &trackerVertexX);
    tree->SetBranchAddress("trackerVertexY", &trackerVertexY);
    tree->SetBranchAddress("trackerVertexZ", &trackerVertexZ);
    tree->SetBranchAddress("trackerVertexCrossing", &trackerVertexCrossing);
    tree->SetBranchAddress("trackerVertexNTracks", &trackerVertexNTracks);

    const Long64_t nentries = tree->GetEntries();
    for (Long64_t entry = 0; entry < nentries; ++entry)
    {
        tree->GetEntry(entry);

        size_t n = std::min(trackerVertexX->size(), trackerVertexY->size());
        n = std::min(n, trackerVertexZ->size());
        n = std::min(n, trackerVertexCrossing->size());
        n = std::min(n, trackerVertexNTracks->size());

        for (size_t i = 0; i < n; ++i)
        {
            // if (trackerVertexCrossing->at(i) != 0)
            // {
            //     continue;
            // }

            pts.emplace_back(trackerVertexX->at(i), trackerVertexY->at(i));
            vtxZ.push_back(trackerVertexZ->at(i));
            hM_vertexXY->Fill(trackerVertexX->at(i), trackerVertexY->at(i));
            hM_vertexX->Fill(trackerVertexX->at(i));
            hM_vertexY->Fill(trackerVertexY->at(i));
            hM_vertexZ->Fill(trackerVertexZ->at(i));

            const int ntracks = trackerVertexNTracks->at(i);
            int trackBin = -1;
            for (size_t ibin = 0; ibin + 2 < trackMultiplicityBinEdges.size(); ++ibin)
            {
                const bool isLastFiniteBin = (ibin + 3 == trackMultiplicityBinEdges.size());
                const bool isInFiniteBin = ntracks >= trackMultiplicityBinEdges[ibin] && (isLastFiniteBin ? ntracks <= trackMultiplicityBinEdges[ibin + 1] : ntracks < trackMultiplicityBinEdges[ibin + 1]);
                if (isInFiniteBin)
                {
                    trackBin = ibin;
                    break;
                }
            }
            if (trackBin < 0 && ntracks > trackMultiplicityBinEdges[trackMultiplicityBinEdges.size() - 2])
            {
                trackBin = trackMultiplicityBinEdges.size() - 2;
            }
            if (trackBin >= 0)
            {
                hM_vertexXByNTracks[trackBin]->Fill(trackerVertexX->at(i));
                hM_vertexYByNTracks[trackBin]->Fill(trackerVertexY->at(i));
                hM_vertexXYByNTracks[trackBin]->Fill(trackerVertexX->at(i), trackerVertexY->at(i));
                vertexXByNTracks[trackBin].push_back(trackerVertexX->at(i));
                vertexYByNTracks[trackBin].push_back(trackerVertexY->at(i));
            }
            if (trackerVertexCrossing->at(i) == 0)
            {
                hM_vertexZ_TrgCrossing->Fill(trackerVertexZ->at(i));
            }
            else
            {
                hM_vertexZ_nonTrgCrossing->Fill(trackerVertexZ->at(i));
            }
        }

        if (mbdVertex->size() > 0)
        {
            const float mbdZ = mbdVertex->at(0);
            const bool hasTrg12 = std::find(firedTriggers->begin(), firedTriggers->end(), 12) != firedTriggers->end();
            const bool hasTrg14 = std::find(firedTriggers->begin(), firedTriggers->end(), 14) != firedTriggers->end();

            hM_MBDVertexZ->Fill(mbdZ);
            if (hasTrg12)
                hM_MBDVertexZ_trg12->Fill(mbdZ);
            if (hasTrg14)
                hM_MBDVertexZ_trg14->Fill(mbdZ);
            if (hasTrg12 || hasTrg14)
                hM_MBDVertexZ_trg12or14->Fill(mbdZ);
        }

        // fill fired trigger bits
        for (const auto &trg : *firedTriggers)
        {
            hM_triggerbits->Fill(trg);
        }
    }

    if (pts.empty())
    {
        std::cout << "No trackerVertex points found with trackerVertexCrossing==0." << std::endl;
        return;
    }

    std::vector<double> xs, ys;
    xs.reserve(pts.size());
    ys.reserve(pts.size());
    double mx = 0, my = 0;
    for (const auto &p : pts)
    {
        xs.push_back(p.first);
        ys.push_back(p.second);
        mx += p.first;
        my += p.second;
    }
    mx /= pts.size();
    my /= pts.size();

    const double coordMedX = median1D(xs);
    const double coordMedY = median1D(ys);

    std::cout << "Mean:                          (" << mx << ", " << my << ")\n";
    std::cout << "Coordinate-wise median:        (" << coordMedX << ", " << coordMedY << ")\n";

    // calculate the mean, median, and effective sigmal, standard deviation for Z vertex
    double zMean = std::accumulate(vtxZ.begin(), vtxZ.end(), 0.0) / vtxZ.size();
    double zCoordMed = median1D(vtxZ);
    double zStdDev = std::sqrt(std::accumulate(vtxZ.begin(), vtxZ.end(), 0.0, [zMean](double acc, double z) { return acc + (z - zMean) * (z - zMean); }) / vtxZ.size());
    std::cout << "Z Vertex - Mean: " << zMean << ", Coordinate-wise Median: " << zCoordMed << ", Std Dev: " << zStdDev << std::endl;

    draw1Dhistogram(hM_vertexX,                           //
                    false,                                //
                    "Vertex X [cm]",                      //
                    "Counts",                             //
                    {"Run " + std::to_string(runNumber)}, //
                    "hist e1",                            //
                    plotdir + "/vertexX_allCrossing"      //
    );
    draw1Dhistogram(hM_vertexY,                           //
                    false,                                //
                    "Vertex Y [cm]",                      //
                    "Counts",                             //
                    {"Run " + std::to_string(runNumber)}, //
                    "hist e1",                            //
                    plotdir + "/vertexY_allCrossing"      //
    );
    draw1Dhistogram(hM_vertexZ_TrgCrossing,          //
                    false,                           //
                    "Vertex Z [cm]",                 //
                    "Counts",                        //
                    {"Trigger crossings"},           //
                    "hist e1",                       //
                    plotdir + "/vertexZ_trgCrossing" //
    );
    draw1Dhistogram(hM_vertexZ_nonTrgCrossing,          //
                    false,                              //
                    "Vertex Z [cm]",                    //
                    "Counts",                           //
                    {"Non-trigger crossings"},          //
                    "hist e1",                          //
                    plotdir + "/vertexZ_nonTrgCrossing" //
    );
    draw1Dhistogram(hM_triggerbits,                       //
                    false,                                //
                    "Fired trigger bit",                  //
                    "Counts",                             //
                    {"Run " + std::to_string(runNumber)}, //
                    "hist e1",                            //
                    plotdir + "/firedTriggerBits"         //
    );

    fitConfig fitCfg_X;
    fitCfg_X.fitFunction = "[0] + [1]*exp(-0.5*((x-[2])/(sqrt(2)*[3]))^2)";
    fitCfg_X.initialParams = {1000.0, 9000.0, coordMedX, 0.012};                          // initial parameter guesses for the fit function
    fitCfg_X.paramNames = {{0, "Constant"}, {1, "Amplitude"}, {2, "Mean"}, {3, "Sigma"}}; // names for the fit parameters (for legend)
    fitCfg_X.fitRange = std::make_pair(coordMedX - 0.06, coordMedX + 0.06);               // fit range for the x-axis

    fitConfig fitCfg_Y;
    fitCfg_Y.fitFunction = "[0] + [1]*exp(-0.5*((x-[2])/(sqrt(2)*[3]))^2)";
    fitCfg_Y.initialParams = {1000.0, 9000.0, coordMedY, 0.012};                          // initial parameter guesses for the fit function
    fitCfg_Y.paramNames = {{0, "Constant"}, {1, "Amplitude"}, {2, "Mean"}, {3, "Sigma"}}; // names for the fit parameters (for legend)
    fitCfg_Y.fitRange = std::make_pair(coordMedY - 0.06, coordMedY + 0.06);               // fit range for the x-axis

    fitConfig fitCfg_Z;
    fitCfg_Z.fitFunction = "[0]*exp(-0.5*((x-[1])/(sqrt(2)*[2]))^2)";
    fitCfg_Z.initialParams = {9747.28, zCoordMed, 12.0};                 // initial parameter guesses for the fit function
    fitCfg_Z.paramNames = {{0, "Amplitude"}, {1, "Mean"}, {2, "Sigma"}}; // names for the fit parameters (for legend)
    fitCfg_Z.fitRange = std::make_pair(-15, 15);                         // fit range for the x-axis

    auto makeMbdZFitConfig = [](TH1 *hist, float fitRangeHalfWidth = 15.0) -> fitConfig {
        fitConfig config;
        const double median = histogramMedian(hist);
        config.fitFunction = "[0]*exp(-0.5*((x-[1])/(sqrt(2)*[2]))^2)";
        config.initialParams = {std::max(1.0, hist->GetMaximum()), median, 6.0};
        config.paramNames = {{0, "Amplitude"}, {1, "Mean"}, {2, "Sigma"}};
        config.fitRange = std::make_pair(median - fitRangeHalfWidth, median + fitRangeHalfWidth);
        return config;
    };

    fitConfig fitCfg_MBDZ = makeMbdZFitConfig(hM_MBDVertexZ);
    fitConfig fitCfg_MBDZ_trg12 = makeMbdZFitConfig(hM_MBDVertexZ_trg12);
    fitConfig fitCfg_MBDZ_trg12or14 = makeMbdZFitConfig(hM_MBDVertexZ_trg12or14);
    fitConfig fitCfg_MBDZ_trg14 = makeMbdZFitConfig(hM_MBDVertexZ_trg14, 30.0);

    draw1Dhistogram_GaussianFit(hM_vertexX,                                               //
                                false,                                                    //
                                fitCfg_X,                                                 //
                                "Vertex X [cm]",                                          //
                                "Counts",                                                 //
                                {"Run " + std::to_string(runNumber) + ". All crossings"}, //
                                "hist e1",                                                //
                                plotdir + "/vertexX_allCrossing_GaussFit"                 //
    );

    draw1Dhistogram_GaussianFit(hM_vertexY,                                               //
                                false,                                                    //
                                fitCfg_Y,                                                 //
                                "Vertex Y [cm]",                                          //
                                "Counts",                                                 //
                                {"Run " + std::to_string(runNumber) + ". All crossings"}, //
                                "hist e1",                                                //
                                plotdir + "/vertexY_allCrossing_GaussFit"                 //
    );

    draw1Dhistogram_GaussianFit(hM_vertexZ,                                               //
                                false,                                                    //
                                fitCfg_Z,                                                 //
                                "Silicon vertex Z [cm]",                                  //
                                "Counts",                                                 //
                                {"Run " + std::to_string(runNumber) + ". All crossings"}, //
                                "hist e1",                                                //
                                plotdir + "/vertexZ_allCrossing"                          //
    );

    draw1Dhistogram_GaussianFit(hM_MBDVertexZ,                                                        //
                                false,                                                                //
                                fitCfg_MBDZ,                                                          //
                                "MBD vertex Z [cm]",                                                  //
                                "Counts",                                                             //
                                {"Run " + std::to_string(runNumber) + ". All trigger bits included"}, //
                                "hist e1",                                                            //
                                plotdir + "/MbdVertexZ_GaussFit"                                      //
    );

    draw1Dhistogram_GaussianFit(hM_MBDVertexZ_trg14,                                                        //
                                false,                                                                //
                                fitCfg_MBDZ_trg14,                                                    //
                                "MBD vertex Z [cm]",                                                  //
                                "Counts",                                                             //
                                {"Run " + std::to_string(runNumber) + ". Trigger bit 14"},            //
                                "hist e1",                                                            //
                                plotdir + "/MbdVertexZ_trg14_GaussFit"                                //
    );

    draw1Dhistogram_GaussianFit(hM_MBDVertexZ_trg12,                                                        //
                                false,                                                                //
                                fitCfg_MBDZ_trg12,                                                    //
                                "MBD vertex Z [cm]",                                                  //
                                "Counts",                                                             //
                                {"Run " + std::to_string(runNumber) + ". Trigger bit 12"},            //
                                "hist e1",                                                            //
                                plotdir + "/MbdVertexZ_trg12_GaussFit"                                //
    );

    draw1Dhistogram_GaussianFit(hM_MBDVertexZ_trg12or14,                                                        //
                                false,                                                                //
                                fitCfg_MBDZ_trg12or14,                                                //
                                "MBD vertex Z [cm]",                                                  //
                                "Counts",                                                             //
                                {"Run " + std::to_string(runNumber) + ". Trigger bit 12 or 14"},      //
                                "hist e1",                                                            //
                                plotdir + "/MbdVertexZ_trg12or14_GaussFit"                            //
    );

    for (size_t ibin = 0; ibin < trackBinLabels.size(); ++ibin)
    {
        if (vertexXByNTracks[ibin].size() < 20 || vertexYByNTracks[ibin].size() < 20)
        {
            std::cout << "Skip NTracks " << trackBinLabels[ibin] << " width fits: only " << vertexXByNTracks[ibin].size() << " entries\n";
            continue;
        }

        draw2Dhistogram(hM_vertexXYByNTracks[ibin], "Vertex X [cm]", "Vertex Y [cm]", {"Run " + std::to_string(runNumber) + ". All crossings", "N_{Silicon seeds} " + trackBinLabels[ibin]}, Form("%s/vertexXY_NTracks_bin%zu", plotdir.c_str(), ibin));

        fitConfig fitCfg_binX = fitCfg_X;
        fitCfg_binX.initialParams = {1000.0, 9000.0, median1D(vertexXByNTracks[ibin]), 0.012};
        fitCfg_binX.fitRange = std::make_pair(fitCfg_binX.initialParams[2] - 0.06, fitCfg_binX.initialParams[2] + 0.06);
        draw1Dhistogram_GaussianFit(hM_vertexXByNTracks[ibin], false, fitCfg_binX, "Vertex X [cm]", "Counts", {"Run " + std::to_string(runNumber) + ". All crossings", "N_{Silicon seeds} " + trackBinLabels[ibin]}, "hist e1", Form("%s/vertexX_NTracks_bin%zu_GaussFit", plotdir.c_str(), ibin));
        validWidthX[ibin] = std::isfinite(fitCfg_binX.postFitParams["Sigma"]) && fitCfg_binX.postFitParams["Sigma"] > 0.0;
        if (validWidthX[ibin])
        {
            hM_vertexWidthX_vs_NTracks->SetBinContent(ibin + 1, fitCfg_binX.postFitParams["Sigma"]);
            hM_vertexWidthX_vs_NTracks->SetBinError(ibin + 1, fitCfg_binX.postFitParamErrors["Sigma"]);
            hM_vertexMeanX_vs_NTracks->SetBinContent(ibin + 1, fitCfg_binX.postFitParams["Mean"]);
            hM_vertexMeanX_vs_NTracks->SetBinError(ibin + 1, fitCfg_binX.postFitParamErrors["Mean"]);
        }

        fitConfig fitCfg_binY = fitCfg_Y;
        fitCfg_binY.initialParams = {1000.0, 9000.0, median1D(vertexYByNTracks[ibin]), 0.012};
        fitCfg_binY.fitRange = std::make_pair(fitCfg_binY.initialParams[2] - 0.06, fitCfg_binY.initialParams[2] + 0.06);
        draw1Dhistogram_GaussianFit(hM_vertexYByNTracks[ibin], false, fitCfg_binY, "Vertex Y [cm]", "Counts", {"Run " + std::to_string(runNumber) + ". All crossings", "N_{Silicon seeds} " + trackBinLabels[ibin]}, "hist e1", Form("%s/vertexY_NTracks_bin%zu_GaussFit", plotdir.c_str(), ibin));
        validWidthY[ibin] = std::isfinite(fitCfg_binY.postFitParams["Sigma"]) && fitCfg_binY.postFitParams["Sigma"] > 0.0;
        if (validWidthY[ibin])
        {
            hM_vertexWidthY_vs_NTracks->SetBinContent(ibin + 1, fitCfg_binY.postFitParams["Sigma"]);
            hM_vertexWidthY_vs_NTracks->SetBinError(ibin + 1, fitCfg_binY.postFitParamErrors["Sigma"]);
            hM_vertexMeanY_vs_NTracks->SetBinContent(ibin + 1, fitCfg_binY.postFitParams["Mean"]);
            hM_vertexMeanY_vs_NTracks->SetBinError(ibin + 1, fitCfg_binY.postFitParamErrors["Mean"]);
        }
    }

    double finalBeamspotX = -9999.0;
    double finalBeamspotY = -9999.0;
    double finalBeamSpreadX = -9999.0;
    double finalBeamSpreadY = -9999.0;
    double finalBeamspotXError = 0.0;
    double finalBeamspotYError = 0.0;
    double finalBeamSpreadXError = 0.0;
    double finalBeamSpreadYError = 0.0;

    double meanXSum = 0.0;
    double meanYSum = 0.0;
    double meanXErrorSquaredSum = 0.0;
    double meanYErrorSquaredSum = 0.0;
    int nMeanX = 0;
    int nMeanY = 0;
    for (size_t ibin = 1; ibin < trackBinLabels.size(); ++ibin)
    {
        if (validWidthX[ibin])
        {
            meanXSum += hM_vertexMeanX_vs_NTracks->GetBinContent(ibin + 1);
            meanXErrorSquaredSum += std::pow(hM_vertexMeanX_vs_NTracks->GetBinError(ibin + 1), 2);
            ++nMeanX;
        }
        if (validWidthY[ibin])
        {
            meanYSum += hM_vertexMeanY_vs_NTracks->GetBinContent(ibin + 1);
            meanYErrorSquaredSum += std::pow(hM_vertexMeanY_vs_NTracks->GetBinError(ibin + 1), 2);
            ++nMeanY;
        }
    }
    if (nMeanX > 0)
    {
        finalBeamspotX = meanXSum / nMeanX;
        finalBeamspotXError = std::sqrt(meanXErrorSquaredSum) / nMeanX;
    }
    if (nMeanY > 0)
    {
        finalBeamspotY = meanYSum / nMeanY;
        finalBeamspotYError = std::sqrt(meanYErrorSquaredSum) / nMeanY;
    }

    const size_t highestMultiplicityBin = trackBinLabels.size() - 1;
    if (validWidthX[highestMultiplicityBin])
    {
        finalBeamSpreadX = hM_vertexWidthX_vs_NTracks->GetBinContent(highestMultiplicityBin + 1);
        finalBeamSpreadXError = hM_vertexWidthX_vs_NTracks->GetBinError(highestMultiplicityBin + 1);
    }
    if (validWidthY[highestMultiplicityBin])
    {
        finalBeamSpreadY = hM_vertexWidthY_vs_NTracks->GetBinContent(highestMultiplicityBin + 1);
        finalBeamSpreadYError = hM_vertexWidthY_vs_NTracks->GetBinError(highestMultiplicityBin + 1);
    }

    hM_beamspotSummary->SetBinContent(1, runNumber);
    hM_beamspotSummary->SetBinContent(2, finalBeamspotX);
    hM_beamspotSummary->SetBinContent(3, finalBeamspotY);
    hM_beamspotSummary->SetBinContent(4, finalBeamSpreadX);
    hM_beamspotSummary->SetBinContent(5, finalBeamSpreadY);
    hM_beamspotSummary->SetBinContent(6, fitCfg_MBDZ_trg12.postFitParams["Mean"]);
    hM_beamspotSummary->SetBinContent(7, fitCfg_MBDZ_trg12.postFitParams["Sigma"]);
    hM_beamspotSummary->SetBinContent(8, fitCfg_MBDZ_trg12or14.postFitParams["Mean"]);
    hM_beamspotSummary->SetBinContent(9, fitCfg_MBDZ_trg12or14.postFitParams["Sigma"]);
    hM_beamspotSummary->SetBinContent(10, fitCfg_MBDZ_trg14.postFitParams["Mean"]);
    hM_beamspotSummary->SetBinContent(11, fitCfg_MBDZ_trg14.postFitParams["Sigma"]);
    hM_beamspotSummary->SetBinContent(12, fitCfg_MBDZ.postFitParams["Mean"]);
    hM_beamspotSummary->SetBinContent(13, fitCfg_MBDZ.postFitParams["Sigma"]);
    hM_beamspotSummary->SetBinError(1, 0.0);
    hM_beamspotSummary->SetBinError(2, finalBeamspotXError);
    hM_beamspotSummary->SetBinError(3, finalBeamspotYError);
    hM_beamspotSummary->SetBinError(4, finalBeamSpreadXError);
    hM_beamspotSummary->SetBinError(5, finalBeamSpreadYError);
    hM_beamspotSummary->SetBinError(6, fitCfg_MBDZ_trg12.postFitParamErrors["Mean"]);
    hM_beamspotSummary->SetBinError(7, fitCfg_MBDZ_trg12.postFitParamErrors["Sigma"]);
    hM_beamspotSummary->SetBinError(8, fitCfg_MBDZ_trg12or14.postFitParamErrors["Mean"]);
    hM_beamspotSummary->SetBinError(9, fitCfg_MBDZ_trg12or14.postFitParamErrors["Sigma"]);
    hM_beamspotSummary->SetBinError(10, fitCfg_MBDZ_trg14.postFitParamErrors["Mean"]);
    hM_beamspotSummary->SetBinError(11, fitCfg_MBDZ_trg14.postFitParamErrors["Sigma"]);
    hM_beamspotSummary->SetBinError(12, fitCfg_MBDZ.postFitParamErrors["Mean"]);
    hM_beamspotSummary->SetBinError(13, fitCfg_MBDZ.postFitParamErrors["Sigma"]);

    std::cout << "Final beamspot X from fitted means excluding lowest multiplicity: " << finalBeamspotX << " cm\n";
    std::cout << "Final beamspot Y from fitted means excluding lowest multiplicity: " << finalBeamspotY << " cm\n";
    std::cout << "Final beam spread X from highest multiplicity bin " << trackBinLabels[highestMultiplicityBin] << ": " << finalBeamSpreadX << " cm\n";
    std::cout << "Final beam spread Y from highest multiplicity bin " << trackBinLabels[highestMultiplicityBin] << ": " << finalBeamSpreadY << " cm\n";

    drawVertexSummaryVsTrackBin(hM_vertexWidthX_vs_NTracks, validWidthX, "g_vertexWidthX_vs_NTracks", "Gaussian-fit width #sigma_{X} [cm]", std::make_pair(0.0, 0.025), {"Run " + std::to_string(runNumber) + ". All crossings"}, plotdir + "/vertexWidthX_vs_NTracks");
    drawVertexSummaryVsTrackBin(hM_vertexWidthY_vs_NTracks, validWidthY, "g_vertexWidthY_vs_NTracks", "Gaussian-fit width #sigma_{Y} [cm]", std::make_pair(0.0, 0.025), {"Run " + std::to_string(runNumber) + ". All crossings"}, plotdir + "/vertexWidthY_vs_NTracks");
    drawVertexSummaryVsTrackBin(hM_vertexMeanX_vs_NTracks, validWidthX, "g_vertexMeanX_vs_NTracks", "Gaussian-fit mean #mu_{X} [cm]", std::make_pair(-0.05, -0.02), {"Run " + std::to_string(runNumber) + ". All crossings"}, plotdir + "/vertexMeanX_vs_NTracks");
    drawVertexSummaryVsTrackBin(hM_vertexMeanY_vs_NTracks, validWidthY, "g_vertexMeanY_vs_NTracks", "Gaussian-fit mean #mu_{Y} [cm]", std::make_pair(0.13, 0.17), {"Run " + std::to_string(runNumber) + ". All crossings"}, plotdir + "/vertexMeanY_vs_NTracks");

    TCanvas *c = new TCanvas("c", "c", 800, 700);
    gPad->SetRightMargin(0.15);
    gPad->SetTopMargin(0.07);
    c->cd();
    hM_vertexXY->GetXaxis()->SetTitle("Vertex X [cm]");
    hM_vertexXY->GetYaxis()->SetTitle("Vertex Y [cm]");
    hM_vertexXY->Draw("COLZ");

    c->Update();

    TLegend *leg = new TLegend(gPad->GetLeftMargin() + 0.02,   //
                               gPad->GetBottomMargin() + 0.03, //
                               gPad->GetLeftMargin() + 0.2,    //
                               gPad->GetBottomMargin() + 0.13  //
    );
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.03);
    leg->SetTextAlign(kHAlignLeft + kVAlignBottom);
    leg->AddEntry((TObject *)0, Form("Gaussian-fit #sigma_{X}: %.3g cm", fitCfg_X.postFitParams["Sigma"]), "");
    leg->AddEntry((TObject *)0, Form("Gaussian-fit #sigma_{Y}: %.3g cm", fitCfg_Y.postFitParams["Sigma"]), "");
    leg->Draw("SAME");

    c->SaveAs(Form("%s/beamspot_2DJointMedian.png", plotdir.c_str()));
    c->SaveAs(Form("%s/beamspot_2DJointMedian.pdf", plotdir.c_str()));

    // output tree
    TFile *outFile = TFile::Open(Form("%s/beamspot_2DJointMedian.root", plotdir.c_str()), "RECREATE"); //! TODO: make it argument for condor job
    TTree *outTree = new TTree("vertex", "Vertex Tree with MBD Z vertex and silicon beamspot and per-crossing vertex");
    uint64_t out_gl1bco, out_bcotr;
    float beamspotX = finalBeamspotX;
    float beamspotY = finalBeamspotY;
    float MbdVertexZ;
    bool trigger12; // true if trigger 12 fired (MBD N&S >=2 & |vtx| < 10 cm)
    float SiliconVertexZ_crossing0;
    std::vector<float> SiliconVertexZ_nonzeroCrossing;
    outTree->Branch("counter", &counter);
    outTree->Branch("gl1bco", &out_gl1bco);
    outTree->Branch("bcotr", &out_bcotr);
    outTree->Branch("beamspotX", &beamspotX);
    outTree->Branch("beamspotY", &beamspotY);
    outTree->Branch("MbdVertexZ", &MbdVertexZ);
    outTree->Branch("trigger12", &trigger12);
    outTree->Branch("SiliconVertexZ_crossing0", &SiliconVertexZ_crossing0);
    outTree->Branch("SiliconVertexZ_nonzeroCrossing", &SiliconVertexZ_nonzeroCrossing);
    for (Long64_t entry = 0; entry < tree->GetEntries(); ++entry)
    {
        tree->GetEntry(entry);

        out_gl1bco = gl1bco;
        out_bcotr = bcotr;

        // check if trigger 12 is fired
        trigger12 = false;
        auto it = std::find(firedTriggers->begin(), firedTriggers->end(), 12);
        if (it != firedTriggers->end())
        {
            trigger12 = true;
        }

        // MBD vertex Z
        MbdVertexZ = (mbdVertex->empty()) ? -9999.0 : mbdVertex->at(0); // all the mbdVertex are the same per trigger frame

        // Silicon vertex Z
        SiliconVertexZ_crossing0 = -9999.0;
        SiliconVertexZ_nonzeroCrossing.clear();

        for (size_t i = 0; i < trackerVertexCrossing->size(); ++i)
        {
            if (trackerVertexCrossing->at(i) == 0)
            {
                SiliconVertexZ_crossing0 = trackerVertexZ->at(i); // assuming Z is vertex Z
            }
            else
            {
                SiliconVertexZ_nonzeroCrossing.push_back(trackerVertexZ->at(i)); // assuming Z is vertex Z
            }
        }

        outTree->Fill();
    }
    outFile->cd();
    hM_vertexXY->Write();
    hM_vertexX->Write();
    hM_vertexY->Write();
    hM_vertexZ->Write();
    hM_vertexZ_TrgCrossing->Write();
    hM_vertexZ_nonTrgCrossing->Write();
    hM_MBDVertexZ->Write();
    hM_MBDVertexZ_trg14->Write();
    hM_MBDVertexZ_trg12->Write();
    hM_MBDVertexZ_trg12or14->Write();
    hM_triggerbits->Write();
    hM_vertexWidthX_vs_NTracks->Write();
    hM_vertexWidthY_vs_NTracks->Write();
    hM_vertexMeanX_vs_NTracks->Write();
    hM_vertexMeanY_vs_NTracks->Write();
    hM_beamspotSummary->Write();
    TGraphAsymmErrors *g_vertexWidthX_vs_NTracks = makeVertexSummaryVsTrackGraph(hM_vertexWidthX_vs_NTracks, validWidthX, "g_vertexWidthX_vs_NTracks");
    TGraphAsymmErrors *g_vertexWidthY_vs_NTracks = makeVertexSummaryVsTrackGraph(hM_vertexWidthY_vs_NTracks, validWidthY, "g_vertexWidthY_vs_NTracks");
    TGraphAsymmErrors *g_vertexMeanX_vs_NTracks = makeVertexSummaryVsTrackGraph(hM_vertexMeanX_vs_NTracks, validWidthX, "g_vertexMeanX_vs_NTracks");
    TGraphAsymmErrors *g_vertexMeanY_vs_NTracks = makeVertexSummaryVsTrackGraph(hM_vertexMeanY_vs_NTracks, validWidthY, "g_vertexMeanY_vs_NTracks");
    g_vertexWidthX_vs_NTracks->Write();
    g_vertexWidthY_vs_NTracks->Write();
    g_vertexMeanX_vs_NTracks->Write();
    g_vertexMeanY_vs_NTracks->Write();
    delete g_vertexWidthX_vs_NTracks;
    delete g_vertexWidthY_vs_NTracks;
    delete g_vertexMeanX_vs_NTracks;
    delete g_vertexMeanY_vs_NTracks;
    for (size_t ibin = 0; ibin < trackBinLabels.size(); ++ibin)
    {
        hM_vertexXByNTracks[ibin]->Write();
        hM_vertexYByNTracks[ibin]->Write();
        hM_vertexXYByNTracks[ibin]->Write();
    }
    outTree->Write();
    outFile->Close();

    // clean up
    delete c;
    delete hM_vertexXY;
    delete hM_vertexX;
    delete hM_vertexY;
    delete hM_vertexWidthX_vs_NTracks;
    delete hM_vertexWidthY_vs_NTracks;
    delete hM_vertexMeanX_vs_NTracks;
    delete hM_vertexMeanY_vs_NTracks;
    delete hM_beamspotSummary;
    for (size_t ibin = 0; ibin < trackBinLabels.size(); ++ibin)
    {
        delete hM_vertexXByNTracks[ibin];
        delete hM_vertexYByNTracks[ibin];
        delete hM_vertexXYByNTracks[ibin];
    }
    file->Close();
}
