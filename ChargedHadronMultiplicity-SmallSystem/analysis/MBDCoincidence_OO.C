#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "TCanvas.h"
#include "TGaxis.h"
#include "TH1.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TLine.h"
#include "TString.h"
#include "TSystem.h"

// TGaxis::SetMaxDigits(3);

struct RunRate
{
    int runNumber = 0;
    double medianRate = 0.0;
    double standardDeviationRate = 0.0;
    double averageRate = 0.0;
    double runLengthSeconds = 0.0;
};

static std::vector<std::string> splitString(const std::string &text, const char delimiter)
{
    std::vector<std::string> fields;
    std::stringstream stream(text);
    std::string field;
    while (std::getline(stream, field, delimiter))
    {
        fields.push_back(field);
    }
    return fields;
}

static std::vector<RunRate> queryRunRates(const int startRun, const int endRun)
{
    const std::string sql = Form(R"(
SELECT
    run_number,
    median_value,
    COALESCE(std_value, 0) AS std_value,
    avg_value,
    end_time - start_time AS run_seconds
FROM run_timeseries_db_summary
WHERE
    run_number BETWEEN %d AND %d
    AND run_number NOT BETWEEN 82481 AND 82485
    AND time_series_name = 'sphenix_cad_sisScaler_Hz'
    AND labels->>'short_name' = U&'MBD N\00B7S'
    AND (end_time - start_time) > 300
    AND avg_value > 10000
    AND median_value > 25000
    AND median_value > 0
ORDER BY run_number;
)",
                                 startRun, endRun);

    const std::string command = "psql -h sphnxdaqdbreplica.sdcc.bnl.gov daq -At -F '|' -c \"" + sql + "\"";
    const std::string output = gSystem->GetFromPipe(command.c_str()).Data();

    std::vector<RunRate> runRates;
    std::stringstream rows(output);
    std::string row;
    while (std::getline(rows, row))
    {
        if (row.empty())
            continue;

        const std::vector<std::string> fields = splitString(row, '|');
        if (fields.size() != 5)
        {
            std::cerr << "Skipping malformed psql row: " << row << std::endl;
            continue;
        }

        RunRate runRate;
        runRate.runNumber = std::stoi(fields[0]);
        runRate.medianRate = std::stod(fields[1]) / 1000;            // convert Hz to kHz
        runRate.standardDeviationRate = std::stod(fields[2]) / 1000; // convert Hz to kHz
        runRate.averageRate = std::stod(fields[3]) / 1000;           // convert Hz to kHz
        runRate.runLengthSeconds = std::stod(fields[4]);
        runRates.push_back(runRate);
    }

    return runRates;
}

static double averageMedianRate(const std::vector<RunRate> &runRates)
{
    if (runRates.empty())
        return 0.0;

    const double sum = std::accumulate(runRates.begin(), runRates.end(), 0.0, [](const double value, const RunRate &runRate) { return value + runRate.medianRate; });
    return sum / runRates.size();
}

static std::vector<std::string> runNumberLines(const std::vector<RunRate> &runRates, const size_t runsPerLine)
{
    std::vector<std::string> lines;
    for (size_t firstRun = 0; firstRun < runRates.size(); firstRun += runsPerLine)
    {
        std::stringstream line;
        const size_t lastRun = std::min(firstRun + runsPerLine, runRates.size());
        for (size_t irun = firstRun; irun < lastRun; ++irun)
        {
            if (irun > firstRun)
                line << ", ";
            line << Form("%d (%.1f #pm %.1f)", runRates[irun].runNumber, runRates[irun].medianRate, runRates[irun].standardDeviationRate);
        }
        lines.push_back(line.str());
    }
    return lines;
}

static void drawRateMarker(const double rate, const double ymax, const int color, const int lineStyle, const std::string &label)
{
    TLine *line = new TLine(rate, 0.0, rate, ymax);
    line->SetLineColor(color);
    line->SetLineStyle(lineStyle);
    line->SetLineWidth(3);
    line->Draw("same");

    TLatex *text = new TLatex(rate + 3, ymax, Form("%s %.6g kHz", label.c_str(), rate));
    text->SetTextAngle(90);
    text->SetTextAlign(kHAlignRight + kVAlignTop);
    text->SetTextColor(color);
    text->SetTextSize(0.027);
    text->Draw("same");
}

static void drawSummaryPlot(const std::string &plotdir, const std::vector<RunRate> &lowerEndRuns, const std::vector<RunRate> &higherEndRuns, const size_t totalRuns)
{
    const std::vector<std::string> lowerLines = runNumberLines(lowerEndRuns, 4);
    const std::vector<std::string> higherLines = runNumberLines(higherEndRuns, 4);

    const double topY = 0.93;
    const double bottomY = 0.06;
    const size_t layoutRows = lowerLines.size() + higherLines.size() + 7;
    const double lineHeight = std::min(0.04, (topY - bottomY) / std::max<size_t>(1, layoutRows));
    const double bodyTextSize = std::min(0.029, 0.72 * lineHeight);
    const double titleTextSize = std::min(0.038, 1.2 * bodyTextSize);

    TCanvas *canvas = new TCanvas("c_MBDCoincidence_OO_summary", "c_MBDCoincidence_OO_summary", 1100, 800);
    canvas->cd();

    TLatex text;
    text.SetNDC();
    text.SetTextFont(42);

    double y = topY;
    text.SetTextSize(titleTextSize);
    text.DrawLatex(0.06, y, "MBD coincidence selected-run summary");

    text.SetTextSize(bodyTextSize);
    y -= 1.4 * lineHeight;
    text.DrawLatex(0.06, y, Form("Total selected runs: %zu", totalRuns));

    y -= 1.4 * lineHeight;
    text.DrawLatex(0.06, y, Form("Lower 25%% runs (%zu), median #pm std. [kHz]:", lowerEndRuns.size()));
    for (const std::string &line : lowerLines)
    {
        y -= lineHeight;
        text.DrawLatex(0.08, y, line.c_str());
    }

    y -= 1.4 * lineHeight;
    text.DrawLatex(0.06, y, Form("Higher 25%% runs (%zu), median #pm std. [kHz]:", higherEndRuns.size()));
    for (const std::string &line : higherLines)
    {
        y -= lineHeight;
        text.DrawLatex(0.08, y, line.c_str());
    }

    canvas->SaveAs(Form("%s/MBDCoincidence_OO_summary.png", plotdir.c_str()));
    canvas->SaveAs(Form("%s/MBDCoincidence_OO_summary.pdf", plotdir.c_str()));
    delete canvas;
}

const int startRun = 82451;
const int endRun = 82703;
const std::vector<std::string> plotinfo = {Form("O+O, 0.75 mrad, run length > 5 min")};

void MBDCoincidence_OO()
{
    std::string plotdir = "./figure/figure-MBDCoincidence_OO";
    gSystem->mkdir(plotdir.c_str(), true);

    std::vector<RunRate> runRates = queryRunRates(startRun, endRun);
    if (runRates.empty())
    {
        std::cerr << "No selected MBD coincidence rates found." << std::endl;
        return;
    }

    std::vector<RunRate> sortedRunRates = runRates;
    std::sort(sortedRunRates.begin(), sortedRunRates.end(), [](const RunRate &lhs, const RunRate &rhs) { return lhs.medianRate < rhs.medianRate; });

    const size_t groupSize = std::max<size_t>(1, sortedRunRates.size() / 4);
    const std::vector<RunRate> lowerEndRuns(sortedRunRates.begin(), sortedRunRates.begin() + groupSize);
    const std::vector<RunRate> higherEndRuns(sortedRunRates.end() - groupSize, sortedRunRates.end());
    const double lowerAverageRate = averageMedianRate(lowerEndRuns);
    const double higherAverageRate = averageMedianRate(higherEndRuns);
    const double allRunAverageRate = averageMedianRate(runRates);
    const size_t medianIndex = sortedRunRates.size() / 2;
    const double allRunMedianRate = (sortedRunRates.size() % 2 == 0) ? 0.5 * (sortedRunRates[medianIndex - 1].medianRate + sortedRunRates[medianIndex].medianRate) : sortedRunRates[medianIndex].medianRate;

    std::cout << "Run range: " << startRun << "-" << endRun << std::endl;
    std::cout << "Selected runs: " << runRates.size() << std::endl;
    std::cout << "Lower-end group size: " << lowerEndRuns.size() << std::endl;
    std::cout << "Higher-end group size: " << higherEndRuns.size() << std::endl;
    std::cout << "Lower-end average median MBD coincidence rate: " << lowerAverageRate << " kHz" << std::endl;
    std::cout << "Higher-end average median MBD coincidence rate: " << higherAverageRate << " kHz" << std::endl;
    std::cout << "All-run average median MBD coincidence rate: " << allRunAverageRate << " kHz" << std::endl;
    std::cout << "All-run median MBD coincidence rate: " << allRunMedianRate << " kHz" << std::endl;

    const auto minmaxRate = std::minmax_element(runRates.begin(), runRates.end(), [](const RunRate &lhs, const RunRate &rhs) { return lhs.medianRate < rhs.medianRate; });
    const double minRate = minmaxRate.first->medianRate;
    const double maxRate = minmaxRate.second->medianRate;
    const double ratePadding = 0.05 * (maxRate - minRate);

    TH1F *hMedianRate = new TH1F("hMedianRate", "", 30, std::max(0.0, minRate - ratePadding), maxRate + ratePadding);
    hMedianRate->SetStats(0);
    hMedianRate->SetLineWidth(2);
    hMedianRate->SetLineColor(kBlack);
    hMedianRate->GetXaxis()->SetTitle("Median MBD coincidence rate [kHz]");
    hMedianRate->GetYaxis()->SetTitle("Entries");
    hMedianRate->GetYaxis()->SetTitleOffset(1.25);

    for (const RunRate &runRate : runRates)
    {
        hMedianRate->Fill(runRate.medianRate);
    }

    const double hMax = hMedianRate->GetMaximum();
    const double yAxisMax = 1.35 * hMax;
    hMedianRate->GetYaxis()->SetRangeUser(0.0, yAxisMax);

    TCanvas *canvas = new TCanvas("c_MBDCoincidence_OO", "c_MBDCoincidence_OO", 800, 700);
    canvas->cd();
    gPad->SetLeftMargin(0.14);
    gPad->SetRightMargin(0.05);
    gPad->SetTopMargin(0.08);

    hMedianRate->Draw("hist");

    drawRateMarker(lowerAverageRate, hMax * 1.15, kAzure + 1, 7, "Lower 25% avg:");
    drawRateMarker(higherAverageRate, hMax * 1.15, kRed - 7, 7, "Higher 25% avg:");
    drawRateMarker(allRunAverageRate, hMax * 1.15, kBlack, 2, "All-run avg:");
    drawRateMarker(allRunMedianRate, hMax * 1.15, kGreen + 2, 7, "All-run median:");

    TLegend *legend = new TLegend(gPad->GetLeftMargin() + 0.03, 1 - gPad->GetTopMargin() - 0.04 - 0.04 * plotinfo.size(), gPad->GetLeftMargin() + 0.2, 1 - gPad->GetTopMargin() - 0.04);
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->SetTextSize(0.035);
    for (size_t i = 0; i < plotinfo.size(); ++i)
    {
        legend->AddEntry((TObject *)0, plotinfo[i].c_str(), "");
    }
    legend->Draw();

    canvas->RedrawAxis();

    canvas->SaveAs(Form("%s/MBDCoincidence_OO.png", plotdir.c_str()));
    canvas->SaveAs(Form("%s/MBDCoincidence_OO.pdf", plotdir.c_str()));
    drawSummaryPlot(plotdir, lowerEndRuns, higherEndRuns, runRates.size());

    delete canvas;
    delete hMedianRate;
}
