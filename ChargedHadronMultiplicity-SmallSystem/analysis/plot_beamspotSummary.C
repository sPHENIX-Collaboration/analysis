#include <algorithm>
#include <cmath>
#include <cctype>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "TCanvas.h"
#include "TColor.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TList.h"
#include "TSystem.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"

struct BeamspotRunSummary
{
    int runNumber = 0;
    double beamspotX = 0.0;
    double beamspotY = 0.0;
    double beamSpreadX = 0.0;
    double beamSpreadY = 0.0;
    double mbdZMeanTrg12 = -9999.0;
    double mbdZSigmaTrg12 = -9999.0;
    double mbdZMeanTrg12or14 = -9999.0;
    double mbdZSigmaTrg12or14 = -9999.0;
    double mbdZMeanTrg14 = -9999.0;
    double mbdZSigmaTrg14 = -9999.0;
    double mbdZMeanInclusive = -9999.0;
    double mbdZSigmaInclusive = -9999.0;
    double beamspotXError = 0.0;
    double beamspotYError = 0.0;
    double beamSpreadXError = 0.0;
    double beamSpreadYError = 0.0;
    double mbdZMeanTrg12Error = 0.0;
    double mbdZSigmaTrg12Error = 0.0;
    double mbdZMeanTrg12or14Error = 0.0;
    double mbdZSigmaTrg12or14Error = 0.0;
    double mbdZMeanTrg14Error = 0.0;
    double mbdZSigmaTrg14Error = 0.0;
    double mbdZMeanInclusiveError = 0.0;
    double mbdZSigmaInclusiveError = 0.0;
};

struct TriggerBitRunSummary
{
    int runNumber = 0;
    std::vector<std::pair<double, double>> triggerBitCounts;
};

struct GraphStyleConfig
{
    TGraphErrors *graph = nullptr;
    std::string label;
    int color = kBlack;
    int markerStyle = kFullCircle;
};

static bool isRunDirectoryName(const std::string &name)
{
    if (name.empty())
    {
        return false;
    }

    for (const char character : name)
    {
        if (!std::isdigit(static_cast<unsigned char>(character)))
        {
            return false;
        }
    }
    return true;
}

static double getQuantile(const std::vector<double> &sortedValues, const double quantile)
{
    if (sortedValues.empty())
    {
        return 0.0;
    }

    const double position = quantile * (sortedValues.size() - 1);
    const size_t lowerIndex = static_cast<size_t>(std::floor(position));
    const size_t upperIndex = static_cast<size_t>(std::ceil(position));
    const double fraction = position - lowerIndex;
    return sortedValues[lowerIndex] * (1.0 - fraction) + sortedValues[upperIndex] * fraction;
}

static void drawBeamspotGraph(TGraphErrors *graph, const std::string &ytitle, const std::pair<double, double> &yrange, const std::string &filename, const std::vector<std::string> &addinfo = {})
{
    if (!graph || graph->GetN() == 0)
    {
        std::cout << "No valid points for " << filename << std::endl;
        return;
    }

    double xmin = 0.0;
    double xmax = 0.0;
    std::vector<double> yValues;
    yValues.reserve(graph->GetN());
    for (int point = 0; point < graph->GetN(); ++point)
    {
        double x = 0.0;
        double y = 0.0;
        graph->GetPoint(point, x, y);
        yValues.push_back(y);

        if (point == 0)
        {
            xmin = xmax = x;
        }
        else
        {
            xmin = std::min(xmin, x);
            xmax = std::max(xmax, x);
        }
    }

    std::sort(yValues.begin(), yValues.end());
    const double median = getQuantile(yValues, 0.5);
    const double lower68 = getQuantile(yValues, 0.16);
    const double upper68 = getQuantile(yValues, 0.84);

    double xAxisMin = 50.0 * std::floor(xmin / 50.0);
    double xAxisMax = 50.0 * std::ceil(xmax / 50.0);
    if (xAxisMin == xAxisMax)
    {
        xAxisMax = xAxisMin + 50.0;
    }
    const int nXAxisDivisions = (xAxisMax - xAxisMin) / 100.0;

    TCanvas *canvas = new TCanvas("c_beamspotSummary", "c_beamspotSummary", 900, 800);
    canvas->cd();
    gPad->SetLeftMargin(0.17);
    gPad->SetRightMargin(0.07);
    gPad->SetTopMargin(0.07);
    gPad->SetGrid(1, 1);

    TH1F *frame = new TH1F("frame_beamspotSummary", "", 100, xAxisMin, xAxisMax);
    frame->SetStats(0);
    frame->GetXaxis()->SetTitle("Run number");
    frame->GetXaxis()->SetNdivisions(nXAxisDivisions, false);
    frame->GetXaxis()->SetLabelOffset(0.02);
    frame->GetYaxis()->SetTitle(ytitle.c_str());
    frame->GetYaxis()->SetTitleOffset(1.7);
    frame->GetYaxis()->SetRangeUser(yrange.first, yrange.second);
    frame->Draw("axis");

    graph->SetLineColor(kBlack);
    graph->SetLineWidth(2);
    graph->SetMarkerColor(kBlack);
    graph->SetMarkerStyle(kFullCircle);
    graph->SetMarkerSize(1.1);
    graph->Draw("PZ same");

    TLegend *legend = new TLegend(gPad->GetLeftMargin() + 0.02,                              //
                                  1 - gPad->GetTopMargin() - 0.04 - 0.045 * (addinfo.size() + 2), //
                                  gPad->GetLeftMargin() + 0.2,                              //
                                  1 - gPad->GetTopMargin() - 0.04                            //
    );
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->SetTextSize(0.04);
    for (const std::string &info : addinfo)
    {
        legend->AddEntry((TObject *)0, info.c_str(), "");
    }
    legend->AddEntry((TObject *)0, Form("Median = %.3g cm", median), "");
    legend->AddEntry((TObject *)0, Form("68%% range = [%.3g, %.3g] cm", lower68, upper68), "");
    legend->Draw();

    canvas->SaveAs((filename + ".png").c_str());
    canvas->SaveAs((filename + ".pdf").c_str());

    delete legend;
    delete frame;
    delete canvas;
}

static void drawMbdZCompilation(const std::vector<GraphStyleConfig> &graphConfigs, const std::string &ytitle, const std::pair<double, double> &yrange, const std::string &filename)
{
    bool hasPoints = false;
    double xmin = 0.0;
    double xmax = 0.0;
    for (const GraphStyleConfig &config : graphConfigs)
    {
        if (!config.graph || config.graph->GetN() == 0)
        {
            continue;
        }

        for (int point = 0; point < config.graph->GetN(); ++point)
        {
            double x = 0.0;
            double y = 0.0;
            config.graph->GetPoint(point, x, y);
            if (!hasPoints)
            {
                xmin = xmax = x;
                hasPoints = true;
            }
            else
            {
                xmin = std::min(xmin, x);
                xmax = std::max(xmax, x);
            }
        }
    }

    if (!hasPoints)
    {
        std::cout << "No valid points for " << filename << std::endl;
        return;
    }

    double xAxisMin = 50.0 * std::floor(xmin / 50.0);
    double xAxisMax = 50.0 * std::ceil(xmax / 50.0);
    if (xAxisMin == xAxisMax)
    {
        xAxisMax = xAxisMin + 50.0;
    }
    const int nXAxisDivisions = (xAxisMax - xAxisMin) / 100.0;

    TCanvas *canvas = new TCanvas("c_mbdZCompilation", "c_mbdZCompilation", 900, 800);
    canvas->cd();
    gPad->SetLeftMargin(0.17);
    gPad->SetRightMargin(0.07);
    gPad->SetTopMargin(0.07);
    gPad->SetGrid(1, 1);

    TH1F *frame = new TH1F("frame_mbdZCompilation", "", 100, xAxisMin, xAxisMax);
    frame->SetStats(0);
    frame->GetXaxis()->SetTitle("Run number");
    frame->GetXaxis()->SetNdivisions(nXAxisDivisions, false);
    frame->GetXaxis()->SetLabelOffset(0.02);
    frame->GetYaxis()->SetTitle(ytitle.c_str());
    frame->GetYaxis()->SetTitleOffset(1.7);
    frame->GetYaxis()->SetRangeUser(yrange.first, yrange.second);
    frame->Draw("axis");

    TLegend *legend = new TLegend(gPad->GetLeftMargin() + 0.03,
                                  1 - gPad->GetTopMargin() - 0.04 - 0.05 * graphConfigs.size(),
                                  gPad->GetLeftMargin() + 0.2,
                                  1 - gPad->GetTopMargin() - 0.04);
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->SetTextSize(0.032);

    for (const GraphStyleConfig &config : graphConfigs)
    {
        if (!config.graph || config.graph->GetN() == 0)
        {
            continue;
        }

        config.graph->SetLineColor(config.color);
        config.graph->SetLineWidth(2);
        config.graph->SetMarkerColor(config.color);
        config.graph->SetMarkerStyle(config.markerStyle);
        config.graph->SetMarkerSize(1.2);
        config.graph->Draw("PZ same");

        std::vector<double> yValues;
        yValues.reserve(config.graph->GetN());
        for (int point = 0; point < config.graph->GetN(); ++point)
        {
            double x = 0.0;
            double y = 0.0;
            config.graph->GetPoint(point, x, y);
            yValues.push_back(y);
        }
        std::sort(yValues.begin(), yValues.end());
        const double median = getQuantile(yValues, 0.5);
        const std::string legendLabel = Form("%s: median %.3g [cm]", config.label.c_str(), median);
        legend->AddEntry(config.graph, legendLabel.c_str(), "p");
    }
    legend->Draw();

    canvas->SaveAs((filename + ".png").c_str());
    canvas->SaveAs((filename + ".pdf").c_str());

    delete legend;
    delete frame;
    delete canvas;
}

static void drawTriggerBitSummary(const std::vector<TriggerBitRunSummary> &triggerSummaries, const std::string &filename)
{
    if (triggerSummaries.empty())
    {
        std::cout << "No trigger-bit summaries for " << filename << std::endl;
        return;
    }

    std::vector<TriggerBitRunSummary> sortedTriggerSummaries = triggerSummaries;
    std::sort(sortedTriggerSummaries.begin(), sortedTriggerSummaries.end(), [](const TriggerBitRunSummary &left, const TriggerBitRunSummary &right) { return left.runNumber < right.runNumber; });

    TH2F *hist = new TH2F("hM_triggerbits_vs_Run", "", sortedTriggerSummaries.size(), 0.5, sortedTriggerSummaries.size() + 0.5, 41, -0.5, 40.5);
    hist->SetStats(0);
    for (size_t irun = 0; irun < sortedTriggerSummaries.size(); ++irun)
    {
        const TriggerBitRunSummary &summary = sortedTriggerSummaries[irun];
        hist->GetXaxis()->SetBinLabel(irun + 1, std::to_string(summary.runNumber).c_str());
        for (const auto &triggerBitCount : summary.triggerBitCounts)
        {
            hist->Fill(irun + 1, triggerBitCount.first, triggerBitCount.second);
        }
    }

    TCanvas *canvas = new TCanvas("c_triggerbitsSummary", "c_triggerbitsSummary", 1100, 600);
    canvas->cd();
    gPad->SetLeftMargin(0.14);
    gPad->SetRightMargin(0.17);
    gPad->SetTopMargin(0.07);
    gPad->SetBottomMargin(0.16);

    hist->GetXaxis()->SetTitle("Run number");
    // hist->GetXaxis()->SetLabelOffset(0.02);
    hist->GetXaxis()->SetLabelSize(0.025);
    hist->GetYaxis()->SetTitle("Fired trigger bit");
    hist->GetYaxis()->SetTitleOffset(1.3);
    hist->GetZaxis()->SetTitle("Counts");
    hist->GetZaxis()->SetTitleOffset(1.25);
    hist->GetZaxis()->SetRangeUser(0, std::ceil(hist->GetMaximum()/5000)*5000);// set to nearest multiple of 5000
    hist->Draw("COLZ");

    canvas->SaveAs((filename + ".png").c_str());
    canvas->SaveAs((filename + ".pdf").c_str());

    delete hist;
    delete canvas;
}

// list of runs to exclude
std::vector<int> excludedRuns = {
    82391,
    82522,
    82628
};

void plot_beamspotSummary()
{
    const std::string plotdir = "./figure/figure-SvtxMbdVertex";

    TSystemDirectory baseDirectory("baseDirectory", plotdir.c_str());
    TList *directoryEntries = baseDirectory.GetListOfFiles();
    if (!directoryEntries)
    {
        std::cout << "Failed to list directory: " << plotdir << std::endl;
        return;
    }

    int missingFileCount = 0;
    int failedFileCount = 0;
    int missingHistogramCount = 0;
    std::vector<BeamspotRunSummary> runSummaries;
    std::vector<TriggerBitRunSummary> triggerBitRunSummaries;
    TIter nextEntry(directoryEntries);
    while (TSystemFile *entry = static_cast<TSystemFile *>(nextEntry()))
    {
        const std::string runDirectoryName = entry->GetName();
        if (!entry->IsDirectory() || !isRunDirectoryName(runDirectoryName))
        {
            continue;
        }

        const int runNumber = std::stoi(runDirectoryName);
        if (std::find(excludedRuns.begin(), excludedRuns.end(), runNumber) != excludedRuns.end())
        {
            std::cout << "Skipping excluded run: " << runDirectoryName << std::endl;
            continue;
        }

        const std::string rootFileName = plotdir + "/" + runDirectoryName + "/beamspot_2DJointMedian.root";
        if (gSystem->AccessPathName(rootFileName.c_str()))
        {
            ++missingFileCount;
            continue;
        }

        TFile *rootFile = TFile::Open(rootFileName.c_str(), "READ");
        if (!rootFile || rootFile->IsZombie())
        {
            ++failedFileCount;
            delete rootFile;
            continue;
        }

        TH1 *summaryHist = static_cast<TH1 *>(rootFile->Get("hM_beamspotSummary"));
        if (!summaryHist)
        {
            ++missingHistogramCount;
            rootFile->Close();
            delete rootFile;
            continue;
        }

        BeamspotRunSummary summary;
        summary.runNumber = runNumber;
        summary.beamspotX = summaryHist->GetBinContent(2);
        summary.beamspotY = summaryHist->GetBinContent(3);
        summary.beamSpreadX = summaryHist->GetBinContent(4);
        summary.beamSpreadY = summaryHist->GetBinContent(5);
        summary.beamspotXError = summaryHist->GetBinError(2);
        summary.beamspotYError = summaryHist->GetBinError(3);
        summary.beamSpreadXError = summaryHist->GetBinError(4);
        summary.beamSpreadYError = summaryHist->GetBinError(5);
        if (summaryHist->GetNbinsX() >= 11)
        {
            summary.mbdZMeanTrg12 = summaryHist->GetBinContent(6);
            summary.mbdZSigmaTrg12 = summaryHist->GetBinContent(7);
            summary.mbdZMeanTrg12or14 = summaryHist->GetBinContent(8);
            summary.mbdZSigmaTrg12or14 = summaryHist->GetBinContent(9);
            summary.mbdZMeanTrg14 = summaryHist->GetBinContent(10);
            summary.mbdZSigmaTrg14 = summaryHist->GetBinContent(11);
            summary.mbdZMeanTrg12Error = summaryHist->GetBinError(6);
            summary.mbdZSigmaTrg12Error = summaryHist->GetBinError(7);
            summary.mbdZMeanTrg12or14Error = summaryHist->GetBinError(8);
            summary.mbdZSigmaTrg12or14Error = summaryHist->GetBinError(9);
            summary.mbdZMeanTrg14Error = summaryHist->GetBinError(10);
            summary.mbdZSigmaTrg14Error = summaryHist->GetBinError(11);
        }
        if (summaryHist->GetNbinsX() >= 13)
        {
            summary.mbdZMeanInclusive = summaryHist->GetBinContent(12);
            summary.mbdZSigmaInclusive = summaryHist->GetBinContent(13);
            summary.mbdZMeanInclusiveError = summaryHist->GetBinError(12);
            summary.mbdZSigmaInclusiveError = summaryHist->GetBinError(13);
        }
        runSummaries.push_back(summary);

        TH1 *triggerBitsHist = static_cast<TH1 *>(rootFile->Get("hM_triggerbits"));
        if (triggerBitsHist)
        {
            TriggerBitRunSummary triggerSummary;
            triggerSummary.runNumber = runNumber;
            triggerSummary.triggerBitCounts.reserve(triggerBitsHist->GetNbinsX());
            for (int ibin = 1; ibin <= triggerBitsHist->GetNbinsX(); ++ibin)
            {
                triggerSummary.triggerBitCounts.push_back(std::make_pair(triggerBitsHist->GetBinCenter(ibin), triggerBitsHist->GetBinContent(ibin)));
            }
            triggerBitRunSummaries.push_back(triggerSummary);
        }

        rootFile->Close();
        delete rootFile;
    }

    if (missingFileCount > 0)
    {
        std::cout << "Skipped " << missingFileCount << " run directories without beamspot_2DJointMedian.root" << std::endl;
    }
    if (failedFileCount > 0)
    {
        std::cout << "Skipped " << failedFileCount << " unreadable ROOT files" << std::endl;
    }
    if (missingHistogramCount > 0)
    {
        std::cout << "Skipped " << missingHistogramCount << " ROOT files without hM_beamspotSummary" << std::endl;
    }

    if (runSummaries.empty())
    {
        std::cout << "No run summaries found under: " << plotdir << std::endl;
        return;
    }

    std::sort(runSummaries.begin(), runSummaries.end(), [](const BeamspotRunSummary &left, const BeamspotRunSummary &right) { return left.runNumber < right.runNumber; });

    TGraphErrors *beamspotXGraph = new TGraphErrors();
    TGraphErrors *beamspotYGraph = new TGraphErrors();
    TGraphErrors *beamSpreadXGraph = new TGraphErrors();
    TGraphErrors *beamSpreadYGraph = new TGraphErrors();
    TGraphErrors *mbdZMeanTrg12Graph = new TGraphErrors();
    TGraphErrors *mbdZSigmaTrg12Graph = new TGraphErrors();
    TGraphErrors *mbdZMeanTrg12or14Graph = new TGraphErrors();
    TGraphErrors *mbdZSigmaTrg12or14Graph = new TGraphErrors();
    TGraphErrors *mbdZMeanTrg14Graph = new TGraphErrors();
    TGraphErrors *mbdZSigmaTrg14Graph = new TGraphErrors();
    TGraphErrors *mbdZMeanInclusiveGraph = new TGraphErrors();
    TGraphErrors *mbdZSigmaInclusiveGraph = new TGraphErrors();
    beamspotXGraph->SetName("g_beamspotX_vs_Run");
    beamspotYGraph->SetName("g_beamspotY_vs_Run");
    beamSpreadXGraph->SetName("g_beamSpreadX_vs_Run");
    beamSpreadYGraph->SetName("g_beamSpreadY_vs_Run");
    mbdZMeanTrg12Graph->SetName("g_mbdZMeanTrg12_vs_Run");
    mbdZSigmaTrg12Graph->SetName("g_mbdZSigmaTrg12_vs_Run");
    mbdZMeanTrg12or14Graph->SetName("g_mbdZMeanTrg12or14_vs_Run");
    mbdZSigmaTrg12or14Graph->SetName("g_mbdZSigmaTrg12or14_vs_Run");
    mbdZMeanTrg14Graph->SetName("g_mbdZMeanTrg14_vs_Run");
    mbdZSigmaTrg14Graph->SetName("g_mbdZSigmaTrg14_vs_Run");
    mbdZMeanInclusiveGraph->SetName("g_mbdZMeanInclusive_vs_Run");
    mbdZSigmaInclusiveGraph->SetName("g_mbdZSigmaInclusive_vs_Run");

    for (const BeamspotRunSummary &summary : runSummaries)
    {
        if (std::isfinite(summary.beamspotX) && summary.beamspotX > -9000.0)
        {
            const int point = beamspotXGraph->GetN();
            beamspotXGraph->SetPoint(point, summary.runNumber, summary.beamspotX);
            beamspotXGraph->SetPointError(point, 0.0, summary.beamspotXError);
        }

        if (std::isfinite(summary.beamspotY) && summary.beamspotY > -9000.0)
        {
            const int point = beamspotYGraph->GetN();
            beamspotYGraph->SetPoint(point, summary.runNumber, summary.beamspotY);
            beamspotYGraph->SetPointError(point, 0.0, summary.beamspotYError);
        }

        if (std::isfinite(summary.beamSpreadX) && summary.beamSpreadX > -9000.0)
        {
            const int point = beamSpreadXGraph->GetN();
            beamSpreadXGraph->SetPoint(point, summary.runNumber, summary.beamSpreadX);
            beamSpreadXGraph->SetPointError(point, 0.0, summary.beamSpreadXError);
        }

        if (std::isfinite(summary.beamSpreadY) && summary.beamSpreadY > -9000.0)
        {
            const int point = beamSpreadYGraph->GetN();
            beamSpreadYGraph->SetPoint(point, summary.runNumber, summary.beamSpreadY);
            beamSpreadYGraph->SetPointError(point, 0.0, summary.beamSpreadYError);
        }

        if (std::isfinite(summary.mbdZMeanTrg12) && summary.mbdZMeanTrg12 > -9000.0)
        {
            const int point = mbdZMeanTrg12Graph->GetN();
            mbdZMeanTrg12Graph->SetPoint(point, summary.runNumber, summary.mbdZMeanTrg12);
            mbdZMeanTrg12Graph->SetPointError(point, 0.0, summary.mbdZMeanTrg12Error);
        }
        if (std::isfinite(summary.mbdZSigmaTrg12) && summary.mbdZSigmaTrg12 > 0.0)
        {
            const int point = mbdZSigmaTrg12Graph->GetN();
            mbdZSigmaTrg12Graph->SetPoint(point, summary.runNumber, summary.mbdZSigmaTrg12);
            mbdZSigmaTrg12Graph->SetPointError(point, 0.0, summary.mbdZSigmaTrg12Error);
        }
        if (std::isfinite(summary.mbdZMeanTrg12or14) && summary.mbdZMeanTrg12or14 > -9000.0)
        {
            const int point = mbdZMeanTrg12or14Graph->GetN();
            mbdZMeanTrg12or14Graph->SetPoint(point, summary.runNumber, summary.mbdZMeanTrg12or14);
            mbdZMeanTrg12or14Graph->SetPointError(point, 0.0, summary.mbdZMeanTrg12or14Error);
        }
        if (std::isfinite(summary.mbdZSigmaTrg12or14) && summary.mbdZSigmaTrg12or14 > 0.0)
        {
            const int point = mbdZSigmaTrg12or14Graph->GetN();
            mbdZSigmaTrg12or14Graph->SetPoint(point, summary.runNumber, summary.mbdZSigmaTrg12or14);
            mbdZSigmaTrg12or14Graph->SetPointError(point, 0.0, summary.mbdZSigmaTrg12or14Error);
        }
        if (std::isfinite(summary.mbdZMeanTrg14) && summary.mbdZMeanTrg14 > -9000.0)
        {
            const int point = mbdZMeanTrg14Graph->GetN();
            mbdZMeanTrg14Graph->SetPoint(point, summary.runNumber, summary.mbdZMeanTrg14);
            mbdZMeanTrg14Graph->SetPointError(point, 0.0, summary.mbdZMeanTrg14Error);
        }
        if (std::isfinite(summary.mbdZSigmaTrg14) && summary.mbdZSigmaTrg14 > 0.0)
        {
            const int point = mbdZSigmaTrg14Graph->GetN();
            mbdZSigmaTrg14Graph->SetPoint(point, summary.runNumber, summary.mbdZSigmaTrg14);
            mbdZSigmaTrg14Graph->SetPointError(point, 0.0, summary.mbdZSigmaTrg14Error);
        }
        if (std::isfinite(summary.mbdZMeanInclusive) && summary.mbdZMeanInclusive > -9000.0)
        {
            const int point = mbdZMeanInclusiveGraph->GetN();
            mbdZMeanInclusiveGraph->SetPoint(point, summary.runNumber, summary.mbdZMeanInclusive);
            mbdZMeanInclusiveGraph->SetPointError(point, 0.0, summary.mbdZMeanInclusiveError);
        }
        if (std::isfinite(summary.mbdZSigmaInclusive) && summary.mbdZSigmaInclusive > 0.0)
        {
            const int point = mbdZSigmaInclusiveGraph->GetN();
            mbdZSigmaInclusiveGraph->SetPoint(point, summary.runNumber, summary.mbdZSigmaInclusive);
            mbdZSigmaInclusiveGraph->SetPointError(point, 0.0, summary.mbdZSigmaInclusiveError);
        }
    }

    drawBeamspotGraph(beamspotXGraph, "Beamspot X [cm]", std::make_pair(-0.5, 0.5), plotdir + "/beamspotX_vs_Run");
    drawBeamspotGraph(beamspotYGraph, "Beamspot Y [cm]", std::make_pair(-0.5, 0.5), plotdir + "/beamspotY_vs_Run");
    drawBeamspotGraph(beamSpreadXGraph, "Beam spread X [cm]", std::make_pair(0.0, 0.02), plotdir + "/beamSpreadX_vs_Run");
    drawBeamspotGraph(beamSpreadYGraph, "Beam spread Y [cm]", std::make_pair(0.0, 0.02), plotdir + "/beamSpreadY_vs_Run");
    drawBeamspotGraph(mbdZMeanTrg12Graph, "MBD z-vertex mean #mu_{Gaussian fit} [cm]", std::make_pair(-10.0, 10.0), plotdir + "/mbdZMean_trg12_vs_Run", {"Trigger bit 12"});
    drawBeamspotGraph(mbdZSigmaTrg12Graph, "MBD z-vertex width #sigma_{Gaussian fit} [cm]", std::make_pair(0.0, 15.0), plotdir + "/mbdZSigma_trg12_vs_Run", {"Trigger bit 12"});
    drawBeamspotGraph(mbdZMeanTrg12or14Graph, "MBD z-vertex mean #mu_{Gaussian fit} [cm]", std::make_pair(-10.0, 10.0), plotdir + "/mbdZMean_trg12or14_vs_Run", {"Trigger bit 12 or 14"});
    drawBeamspotGraph(mbdZSigmaTrg12or14Graph, "MBD z-vertex width #sigma_{Gaussian fit} [cm]", std::make_pair(0.0, 15.0), plotdir + "/mbdZSigma_trg12or14_vs_Run", {"Trigger bit 12 or 14"});
    drawBeamspotGraph(mbdZMeanTrg14Graph, "MBD z-vertex mean #mu_{Gaussian fit} [cm]", std::make_pair(-15.0, 15.0), plotdir + "/mbdZMean_trg14_vs_Run", {"Trigger bit 14"});
    drawBeamspotGraph(mbdZSigmaTrg14Graph, "MBD z-vertex width #sigma_{Gaussian fit} [cm]", std::make_pair(0.0, 25.0), plotdir + "/mbdZSigma_trg14_vs_Run", {"Trigger bit 14"});
    drawBeamspotGraph(mbdZMeanInclusiveGraph, "MBD z-vertex mean #mu_{Gaussian fit} [cm]", std::make_pair(-15.0, 15.0), plotdir + "/mbdZMean_inclusive_vs_Run", {"All trigger bits included"});
    drawBeamspotGraph(mbdZSigmaInclusiveGraph, "MBD z-vertex width #sigma_{Gaussian fit} [cm]", std::make_pair(0.0, 25.0), plotdir + "/mbdZSigma_inclusive_vs_Run", {"All trigger bits included"});
    const int inclusiveColor = TColor::GetColor("#808080");
    const int trigger12Color = TColor::GetColor("#73A5CA");
    const int trigger14Color = TColor::GetColor("#468432");
    const int trigger12or14Color = TColor::GetColor("#FF9E01");

    drawMbdZCompilation({{mbdZMeanInclusiveGraph, "All trigger bits included", inclusiveColor, 20},
                         {mbdZMeanTrg12Graph, "Trigger bit 12", trigger12Color, 21},
                         {mbdZMeanTrg14Graph, "Trigger bit 14", trigger14Color, 34},
                         {mbdZMeanTrg12or14Graph, "Trigger bit 12 or 14", trigger12or14Color, 47}},
                        "MBD z-vertex mean #mu_{Gaussian fit} [cm]", std::make_pair(-15.0, 15.0), plotdir + "/mbdZMean_triggerComparison_vs_Run");
    drawMbdZCompilation({{mbdZSigmaInclusiveGraph, "All trigger bits included", inclusiveColor, 20},
                         {mbdZSigmaTrg12Graph, "Trigger bit 12", trigger12Color, 21},
                         {mbdZSigmaTrg14Graph, "Trigger bit 14", trigger14Color, 34},
                         {mbdZSigmaTrg12or14Graph, "Trigger bit 12 or 14", trigger12or14Color, 47}},
                        "MBD z-vertex width #sigma_{Gaussian fit} [cm]", std::make_pair(0.0, 25.0), plotdir + "/mbdZSigma_triggerComparison_vs_Run");

    drawTriggerBitSummary(triggerBitRunSummaries, plotdir + "/triggerBits_vs_Run");

    delete beamspotXGraph;
    delete beamspotYGraph;
    delete beamSpreadXGraph;
    delete beamSpreadYGraph;
    delete mbdZMeanTrg12Graph;
    delete mbdZSigmaTrg12Graph;
    delete mbdZMeanTrg12or14Graph;
    delete mbdZSigmaTrg12or14Graph;
    delete mbdZMeanTrg14Graph;
    delete mbdZSigmaTrg14Graph;
    delete mbdZMeanInclusiveGraph;
    delete mbdZSigmaInclusiveGraph;
}
