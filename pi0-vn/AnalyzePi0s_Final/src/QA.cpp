#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <TH1F.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TRandom.h>
#include <TColor.h>
#include <TROOT.h>
#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define BLACK       "\033[30m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"

const std::string baseDir = "/Users/patsfan753/Desktop/p015/QA/";
const std::string QA_ROOT_FILES_PATH = baseDir + "RootFiles/";
std::vector<std::string> runNumbers_MB =    {
    "23020", "23671", "23672", "23676", "23681", "23682", "23687", "23690", "23693", "23694",
    "23695", "23696", "23697", "23699", "23702", "23714", "23726", "23727", "23728", "23735",
    "23737", "23738", "23739", "23740", "23743", "23745", "23746"
};
std::vector<std::string> runNumbers_Central =    {
    "23536",  "23539", "23543", "23547", "23550",  "23552", "23557", "23558",
    "23561", "23562", "23563", "23566", "23568",  "23572",  "23574", "23575", "23582",
    "23590", "23592", "23594", "23604", "23605", "23617", "23618", "23619"
};
// Define a class to handle Statistics
class EMCalStatistics {
public:
    // Public member function to process multiple runs
    void ProcessRuns(const std::vector<std::string>& runNumbers, const std::string& category);
    
    // Public member function to write normalization statistics to a file
    void WriteNormalizationToFile(const std::string& runNumber, int nEvents, double AcceptedBins, const std::string& category);

private:
    // Member function to process a single run
    void ProcessSingleRun(const std::string& runNumber, const std::string& category);
    
    void DrawAndSaveCanvas(TH2F* h2D, TH1F* h1D, const std::string& runNumber, int nEvents, double mean, double sd, double AcceptedBins, const std::string& category);

    // Member function to print various statistics to the terminal
    void PrintStatisticsToTerminal(const std::string& runNumber, int nEvents, TH1F* h1D, const std::string& category);
    
};
// Process all specified runs in the given list
void EMCalStatistics::ProcessRuns(const std::vector<std::string>& runNumbers, const std::string& category) {
    for (const auto& run : runNumbers) {
        ProcessSingleRun(run, category);
    }
}
// Display the calculated statistics in the terminal
void EMCalStatistics::PrintStatisticsToTerminal(const std::string& runNumber, int nEvents, TH1F* h1D, const std::string& category) {
    // Calculate mean, standard deviation, and other statistics (ACCEPTED BIN CALCULATION)
    double mean = h1D->GetMean();
    double sd = h1D->GetStdDev();
    double BinValue = h1D->FindBin(mean - 3*sd);
    double AcceptedBins = h1D->Integral(BinValue, 120);
    
    std::cout << "\033[1;32mRun Number: \033[0m" << runNumber << ", Category: " << category << std::endl;
    std::cout << "\033[1;32mnEvents: \033[0m" << nEvents << std::endl;
    std::cout << "Mean - SD: " << std::fixed << std::setprecision(10) << mean - sd << std::endl;
    std::cout << "Mean - 2*SD: " << std::fixed << std::setprecision(10) << mean - 2*sd << std::endl;
    std::cout << "Mean - 3*SD: " << std::fixed << std::setprecision(10) << mean - 3*sd << std::endl;
    std::cout << "Bin Value: " << std::fixed << std::setprecision(10) << BinValue << std::endl;
    std::cout << "Accepted Bins: " << std::fixed << std::setprecision(10) << AcceptedBins << std::endl;
    
    // Write the calculated statistics to a file
    WriteNormalizationToFile(runNumber, nEvents, AcceptedBins, category);

    // Now also write the same information to a separate text file
    static int runCounter = 0;
    std::ofstream statsFile;
    std::string specificDir = (category == "MB") ? "MB/" : "Central/";
    std::string statsFilePath = baseDir + specificDir + "TowerEnergyStatisticsOutput.txt";
    statsFile.open(statsFilePath, std::ios::app); // Open in append mode
    if(statsFile.is_open()) {
        statsFile << "Run Number: " << runNumber << "\n"
                  << "nEvents: " << nEvents << "\n"
                  << "Mean - SD: " << std::fixed << std::setprecision(10) << mean - sd << "\n"
                  << "Mean - 2*SD: " << std::fixed << std::setprecision(10) << mean - 2*sd << "\n"
                  << "Mean - 3*SD: " << std::fixed << std::setprecision(10) << mean - 3*sd << "\n"
                  << "Bin Value: " << std::fixed << std::setprecision(10) << BinValue << "\n"
                  << "Accepted Bins: " << std::fixed << std::setprecision(10) << AcceptedBins << "\n"
                  << "----------------------------------------\n";
        runCounter++;
        // If the counter is a multiple of 7, add an extra newline for spacing
        if(runCounter % 7 == 0) {
            statsFile << "\n\n\n";
        }

        statsFile.close();
    } else {
        std::cerr << "Unable to open file for writing.\n";
    }

    std::cout << "----------------------------------------" << std::endl;
}
// Write the calculated statistics to a file
void EMCalStatistics::WriteNormalizationToFile(const std::string& runNumber, int nEvents, double AcceptedBins, const std::string& category) {
    // File handling
    std::ofstream outFile;
    std::string specificDir = (category == "MB") ? "MB/" : "Central/";
    std::string outputFilePath = baseDir + specificDir + "Normalize.txt";
    
    static bool firstRun = true; // Static flag to track first run
    if (firstRun) {
        outFile.open(outputFilePath, std::ios::trunc); // truncate only for the first run
        firstRun = false;  // reset flag
    } else {
        outFile.open(outputFilePath, std::ios::app); // append mode for subsequent runs
    }
    if(outFile.is_open()) {
        outFile << runNumber << "\t" << nEvents << "\t" << AcceptedBins << std::endl;
        outFile.close();
    } else {
        std::cerr << "Failed to open output file for writing.\n";
    }
}
// Main logic to process a single run
void EMCalStatistics::ProcessSingleRun(const std::string& runNumber, const std::string& category) {
    const std::string filePath = baseDir + "RootFiles/" + runNumber + "/qa-" + runNumber + ".root";
    TFile* file = TFile::Open(filePath.c_str());
    if (!file || file->IsZombie()) {
        std::cerr << "Failed to open ROOT file for run " << runNumber << '\n';
        return;
    }
    TH2F* h2TowEtaPhiWeighted = static_cast<TH2F*>(file->Get("h2TowEtaPhiWeighted"));
    TH1F* hNClusters = static_cast<TH1F*>(file->Get("hNClusters"));
    if (!h2TowEtaPhiWeighted || !hNClusters) {
        std::cerr << "Failed to get histogram for run " << runNumber << '\n';
        file->Close();
        return;
    }
    int nEvents = hNClusters->GetEntries();
    h2TowEtaPhiWeighted->Scale(1.0 / nEvents);
    std::string title2D = "p013, No PDC, w/ z vtx, Tower Energy Distribution (2D) for Run " + runNumber;
    h2TowEtaPhiWeighted->SetTitle(title2D.c_str());
    std::string title1D = "Tower Energy Distribution (1D) for Run " + runNumber;
    TH1F* h1D_TowEtaPhiWeighted = new TH1F("h1D_TowEtaPhiWeighted", title1D.c_str(), 120, -.02, .1);
    // Loop over all bins in the 2D histogram
    for (int eta = 1; eta <= h2TowEtaPhiWeighted->GetNbinsX(); ++eta) {
        for (int phi = 1; phi <= h2TowEtaPhiWeighted->GetNbinsY(); ++phi) {
            // Retrieve the content (energy) at the (eta, phi) bin of the 2D histogram
            double content = h2TowEtaPhiWeighted->GetBinContent(eta, phi);
            // If the content (energy) is greater than 0, fill the 1D histogram with that value
            if (content > 0) h1D_TowEtaPhiWeighted->Fill(content);
        }
    }
    // Extract mean and AcceptedBins from h1D (assuming you calculate it here similarly to PrintStatisticsToTerminal)
    double mean = h1D_TowEtaPhiWeighted->GetMean();
    double sd = h1D_TowEtaPhiWeighted->GetStdDev();
    double BinValue = h1D_TowEtaPhiWeighted->FindBin(mean - 3*sd);
    double AcceptedBins = h1D_TowEtaPhiWeighted->Integral(BinValue, 120);

    DrawAndSaveCanvas(h2TowEtaPhiWeighted, h1D_TowEtaPhiWeighted, runNumber, nEvents, mean, sd, AcceptedBins, category);
    PrintStatisticsToTerminal(runNumber,nEvents,h1D_TowEtaPhiWeighted, category);
    file->Close();
}

void EMCalStatistics::DrawAndSaveCanvas(TH2F* h2D, TH1F* h1D, const std::string& runNumber, int nEvents, double mean, double sd, double AcceptedBins, const std::string& category) {
    std::string specificDir = (category == "MB") ? "MB/" : "Central/";
    // Canvas for 2D histogram
    TCanvas c2D(("canvas_2D_" + runNumber).c_str(), ("canvas_2D_" + runNumber).c_str(), 900, 600);
    h2D->Draw("COLZ1");
    gPad->SetRightMargin(0.22);
    h2D->SetStats(kFALSE);
    h2D->GetZaxis()->SetRangeUser(0, 0.03);  // Fixed Z-axis range for Tower energy
    h2D->GetXaxis()->SetTitle("Tower Id #eta");
    h2D->GetYaxis()->SetTitle("Tower Id #phi");
    h2D->GetZaxis()->SetTitle("Tower Energy/Event [GeV]");
    h2D->GetZaxis()->SetTitleOffset(1.3);
    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.045);
    latex.SetTextAlign(23); // Center the text horizontally and vertically
    latex.DrawLatex(0.28, 0.99, ("Run Number: " + runNumber).c_str()); // Position the text at the top center of the canvas
    c2D.SaveAs((baseDir + specificDir + "TowerEnergyDistributions/h2TowerEtaPhiWeighted_2D_" + runNumber + ".png").c_str());

    // Canvas for 1D histogram
    TCanvas c1D(("canvas_1D_" + runNumber).c_str(), ("canvas_1D_" + runNumber).c_str(), 900, 800);
    c1D.SetLogy(1);
    h1D->GetXaxis()->SetTitle("Tower Energy/Event [GeV]");
    h1D->GetYaxis()->SetTitle("Counts");
    h1D->Draw();
    TLegend *leg = new TLegend(0.6, 0.67, 0.7, 0.92);
    leg->SetFillStyle(0);
    leg->SetTextSize(.035);
    leg->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    std::string runEntry = "Run: " + runNumber;
    leg->AddEntry("", runEntry.c_str(), "");
    std::string meanEntry = "Mean: " + std::to_string(mean);
    leg->AddEntry("", meanEntry.c_str(), "");
    std::string sdEntry = "#sigma: " + std::to_string(sd);
    leg->AddEntry("", sdEntry.c_str(), "");
    int AcceptedBinsInt = static_cast<int>(AcceptedBins); // Cast double to integer
    std::string acceptedEntry = "Accepted Bins: " + std::to_string(AcceptedBinsInt);
    leg->AddEntry("", acceptedEntry.c_str(), "");
    leg->Draw("same");
    c1D.SaveAs((baseDir + specificDir + "TowerEnergyDistributions/hTowerEtaPhiWeighted_1D_" + runNumber + ".png").c_str());
}
// Data structure to hold key information about each run
struct RunData {
    int nEvents;    // Total number of events in this run
    int acceptance; // Event acceptance criteria
    int color;      // Color code for visualization
};

// Class definition for generating overlay plots
class OverlayPlotter {
public:
    // Constructor initializes the RunData and base directory
    OverlayPlotter(const std::unordered_map<int, RunData>& runData, const std::string& baseDir, const std::string& category) :
        runData_(runData), baseDir_(baseDir) {}
    
    // Returns an absolute path where the individual histograms will be saved.
    std::string GetOutputPath_SingleHists(const std::string& histName, const std::string& category) {
        if (histName == "hClusterChi") {
            return baseDir + category + "/InvidualRunByRunOutput/chi2/";
        }
        if (histName == "hClusterECore") {
            return baseDir + category + "/InvidualRunByRunOutput/ECore/";
        }
        if (histName == "hTotalCaloE") {
            return baseDir + category + "/InvidualRunByRunOutput/TotalCaloE/";
        }
        if (histName == "hTotalMBD") {
            return baseDir + category + "/InvidualRunByRunOutput/MBD/";
        }
        return "";
    }
    // Hash map to store distance of each run's histogram from the average histogram
    std::unordered_map<std::string, double> runDistanceFromAverage_;
    
    // Method to draw the average histogram on the same canvas
    void DrawAverageHistogram(TH1F* accumulatedHist, TLegend* leg, int nRuns) {
        if (nRuns == 0 || !accumulatedHist) return;

        // Normalize the accumulated histogram by the number of runs
        for (int bin = 1; bin <= accumulatedHist->GetNbinsX(); ++bin) {
            double content = accumulatedHist->GetBinContent(bin) / nRuns;
            accumulatedHist->SetBinContent(bin, content);
        }
        accumulatedHist->SetLineColor(kRed+2);
        accumulatedHist->SetLineStyle(1);
        accumulatedHist->SetLineWidth(4);
        accumulatedHist->Draw("HIST SAME");
    }
    // Method to calculate the Euclidean distance of the run's histogram from the average histogram
    // This will be used to analyze how each run deviates from the ~average behavior.
    void CalculateDistanceFromAverage(const std::string& run, TH1F* hist, TH1F* accumulatedHist_, int nRunsProcessed) {
        if (!accumulatedHist_) return;
        // Create a clone of the accumulated histogram and normalize it
        TH1F* avgHist = (TH1F*)accumulatedHist_->Clone("avgHist");
        for (int bin = 1; bin <= avgHist->GetNbinsX(); ++bin) {
            double content = avgHist->GetBinContent(bin) / nRunsProcessed;
            avgHist->SetBinContent(bin, content);
        }
        // Calculate the distance from the average histogram
        double distance = 0;
        for (int i = 1; i <= hist->GetNbinsX(); ++i) {
            double diff = hist->GetBinContent(i) - avgHist->GetBinContent(i);
            distance += diff * diff;
        }
        distance = sqrt(distance);
        runDistanceFromAverage_[run] = distance;
        delete avgHist;
    }
    // Method to convert color code to ANSI color string
    std::string ConvertToAnsiColor(int colorCode) {
        if (colorCode == kBlue) return "Blue";
        if (colorCode == kOrange+7) return "Modified Orange";
        if (colorCode == kBlack) return "Black";
        if (colorCode == kOrange+2) return "Brown";
        if (colorCode == kRed) return "Red";
        if (colorCode == kCyan+3) return "Modified Cyan";
        if (colorCode == kMagenta) return "Magenta";
        if (colorCode == kViolet+1) return "Modified Violet";
        if (colorCode == kMagenta+2) return "Light Magenta";
        if (colorCode == kAzure+4) return "Modified Azure";
        if (colorCode == kAzure+2) return "Light Azure";
        if (colorCode == kBlue+3) return "Light Blue";
        if (colorCode == kPink-3) return "Modified Pink";
        if (colorCode == kGray+1) return "Light Gray";
        return WHITE;
    }
    // Method to overlay histograms (AND PLOTS SINGLE HISTOGRAMS), given a histogram name, title, and axis labels
    void Overlay(const std::string& histName, const std::string& title, const std::string& xAxis, const std::string& yAxis, const std::string& category) {
        bool addAverageToLegend = false;
        TCanvas* c = new TCanvas();
        c->SetTitle(title.c_str());
        c->SetLogy();
        std::vector<std::pair<int, double>> runMaxBinTable;
        // Flags to identify if it's the first run of the loop
        bool firstRun = true;
        TH1F* firstHist = nullptr; // Reference to first histogram
        // Null pointer to hold sum of histograms for average calculations
        TH1F* accumulatedHist_ = nullptr;
        int nRunsProcessed = 0; // Counter for the number of processed runs
        int nRuns = runData_.size();
        TLegend *leg = new TLegend(0.72, 0.5, 0.92, 0.92);
        leg->SetNColumns(2);
        leg->SetTextSize(0.029);
        for (const auto& entry : runData_) {
            int run = entry.first;  // Extract run number
            RunData data = entry.second;  // Extract associated RunData struct
            std::string filePath = QA_ROOT_FILES_PATH + std::to_string(run) + "/qa-" + std::to_string(run) + ".root";
            TFile* file = TFile::Open(filePath.c_str());
            TH1F* hist = (TH1F*)file->Get(histName.c_str());
            hist->SetStats(0);
            if (!hist) continue;
            float normalize;
            if (histName == "hTotalMBD") {
                normalize = 1.0 / float(data.nEvents); //normalize by 1/nEvents for MBD Charge
            } else {
                normalize = 1.0 / (float(data.nEvents) * float(data.acceptance)); //otherwise by 1/nEvents*acceptance
            }
            hist->Scale(normalize);
            hist->SetLineColor(data.color);
            hist->SetStats(0);
            // If this is the first run, draw the histogram; otherwise overlay it
            if (firstRun) {
                hist->Draw("HIST");
                firstHist = hist;
                firstRun = false;
            } else {
                hist->Draw("HIST SAME");
            }
           hist->SetMaximum(hist->GetBinContent(hist->GetMaximumBin()) * 10);
           leg->AddEntry(hist, (std::to_string(run)).c_str(), "l");
           double maxBinContent = hist->GetBinContent(hist->GetMaximumBin());
           runMaxBinTable.emplace_back(run, maxBinContent);
           std::string outputPath = GetOutputPath_SingleHists(histName, category);
           if (!outputPath.empty()) {
                // Clone the histogram for individual saving
                TH1F* clonedHist = (TH1F*)hist->Clone();
                TCanvas* singleCanvas = new TCanvas();
                singleCanvas->SetLogy(1);
                clonedHist->SetStats(1);
                clonedHist->GetYaxis()->SetTitle("Normalized Counts");
                clonedHist->Draw("HIST");
                TLegend *leg2 = new TLegend(0.55,.74,0.75,.93);
                leg2->SetFillStyle(0);
                leg2->AddEntry("","#it{#bf{sPHENIX}} Internal","");
                leg2->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
                std::string runEntry = "Run: " + std::to_string(run);
                leg2->AddEntry("", runEntry.c_str(), "");
                leg2->Draw("same");
                std::string updatedTitle = title + " (Run: " + std::to_string(run) + ")";
                clonedHist->SetTitle(updatedTitle.c_str());
                singleCanvas->SaveAs((outputPath + "Run_" + std::to_string(run) + "_" + title + ".png").c_str());
                
                delete singleCanvas;
                delete clonedHist;
            }
            if (!accumulatedHist_) {
                accumulatedHist_ = (TH1F*)hist->Clone(("accumulated_" + histName).c_str());
            } else {
                accumulatedHist_->Add(hist);
            }
            // Increment counter for processed runs
            nRunsProcessed++;
            // Calculate how far the run is from the average
            CalculateDistanceFromAverage(std::to_string(run), hist, accumulatedHist_, nRunsProcessed);
        }
        // Sort based on max bin content in descending order
        std::sort(runMaxBinTable.begin(), runMaxBinTable.end(),
                  [](const auto& a, const auto& b) {
                      return b.second < a.second;
                  });
        std::cout << BOLD << CYAN << "Sorted Runs:" << RESET << std::endl;
        std::cout << BOLD << CYAN << "Histogram: " << histName << RESET << std::endl;
        std::cout << std::setw(12) << "Run"
                  << std::setw(20) << "Max Bin Content" << std::endl;
        std::cout << std::string(52, '-') << std::endl;
        // Table content
        for (const auto& entry : runMaxBinTable) {
            int run = entry.first;
            double maxBinContent = entry.second;
            RunData data = runData_.at(run);
            std::string colorString = ConvertToAnsiColor(data.color);
            std::cout << std::setw(12) << run
                      << std::setw(20) << maxBinContent << std::endl;
        }
        leg->SetBorderSize(0);
        leg->Draw();
        TLegend *leg1 = new TLegend(0.3,.8,0.5,0.9);
        leg1->SetFillStyle(0);
        leg1->AddEntry("","#it{#bf{sPHENIX}} Internal","");
        leg1->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
        leg1->Draw("same");
        if (firstHist) {
            firstHist->SetTitle(title.c_str());
            firstHist->GetXaxis()->SetTitle(xAxis.c_str());
            firstHist->GetYaxis()->SetTitle(yAxis.c_str());
        }
        DrawAverageHistogram(accumulatedHist_, leg, nRunsProcessed);
        if (addAverageToLegend) {
            leg->AddEntry(accumulatedHist_, "AVERAGE HISTOGRAM", "l");
        }
        std::string filePathOverlayedWithAve = baseDir + category + "/OverlayedWithAverage/" + title + ".png";
        c->SaveAs(filePathOverlayedWithAve.c_str());
        if (accumulatedHist_) {
            delete accumulatedHist_;
            accumulatedHist_ = nullptr;
        }
        
        std::string filePathOverlay = baseDir + category +  + "/Overlayed_RunByRunOutput/" + title + ".png";
        c->SaveAs(filePathOverlay.c_str());
        delete c;
    }

private:
    const std::unordered_map<int, RunData>& runData_;
    const std::string baseDir_;
};
struct HistogramData {
    std::string histogramName;
    std::vector<std::pair<int, double>> sortedRatios;  // Store run number and average ratio
};

std::map<std::string, HistogramData> globalHistogramData;

// Function to write to CSV
void writeDataToCSV(const std::string& filename) {
    std::ofstream csvFile(filename);
    csvFile << "Histogram,Run Number,Average Ratio\n";

    for (const auto& histData : globalHistogramData) {
        for (const auto& data : histData.second.sortedRatios) {
            csvFile << histData.first << "," << data.first << "," << data.second << "\n";
        }
    }

    csvFile.close();
}
void RatioPlot(const std::unordered_map<int, RunData>& runData_, const std::string& baseDir_, const std::string& histName, std::string& refRun, const std::string& category) {
    TCanvas* cRatio = new TCanvas();

    bool firstRatio = true;
    TH1F* refHist = nullptr;
    // Map to store the ratio data
    std::map<double, int, std::greater<>> sortedRatios;
    std::string completeTitle = histName + " Ratio (Reference Run: " + refRun + ")";
    cRatio->SetTitle(completeTitle.c_str());
    TLegend* ratioLeg;
    TLatex latex;
    latex.SetTextSize(0.05);
    latex.SetNDC(true);
    /*
     Allow varied legend coordinates depending on what would fit distribution being plotted (all are fine consistent for now, wasn't always the case)
     */
    if (histName == "hClusterChi") {
        ratioLeg = new TLegend(0.2, 0.6, 0.4, 0.92);
    } else if (histName == "hTotalMBD") {
        ratioLeg = new TLegend(0.2, 0.6, 0.4, 0.92);
    } else if (histName =="hTotalCaloE") {
        ratioLeg = new TLegend(0.2, 0.6, 0.4, 0.92);
    } else {
        ratioLeg = new TLegend(0.2, 0.6, 0.4, 0.92);
    }
    ratioLeg->SetNColumns(2);
    ratioLeg->SetBorderSize(0);
    ratioLeg->SetMargin(0.15);
    ratioLeg->SetTextSize(0.025);
    // Rebinning based on histogram name
    int rebinFactor = 1;  // Default rebin factor
    if (histName == "hClusterChi") {
        rebinFactor = 10;
    } else if (histName == "hTotalMBD") {
        rebinFactor = 10;
    } else if (histName == "hTotalCaloE") {
        rebinFactor = 50;
    } else if (histName == "hClusterECore") {
        rebinFactor = 1;
    }
    // Find maxHist if specified
    double maxIntegral = 0.0;
    if (refRun == "maxHist") {
        for (const auto& entry : runData_) {
            int run = entry.first;
            std::string filePath = QA_ROOT_FILES_PATH + std::to_string(run) + "/qa-" + std::to_string(run) + ".root";
            TFile* file = TFile::Open(filePath.c_str());
            TH1F* hist = (TH1F*)file->Get(histName.c_str());
            if (!hist) continue;
            double integral = hist->Integral();
            if (integral > maxIntegral) {
                maxIntegral = integral;
                refRun = std::to_string(run);
            }
        }
        completeTitle = histName + " Ratio (Reference Run: " + refRun + ")";
    }
    std::cout << "For histogram " << histName << ", the run with maximum integral is Run " << refRun << std::endl;
    std::string refFilePath = baseDir_ + refRun + "/qa-" + refRun + ".root";
    TFile* refFile = TFile::Open(refFilePath.c_str());
    refHist = (TH1F*)refFile->Get(histName.c_str());
    refHist->SetDirectory(0); // Important to keep histogram when file closes
    // Calculate normalization factor for ref histogram
    RunData refRunData = runData_.at(std::stoi(refRun));
    float normalize;
    if (histName == "hTotalMBD") {
        normalize = 1.0 / float(refRunData.nEvents);
    } else {
        normalize = 1.0 / (float(refRunData.nEvents) * float(refRunData.acceptance));
    }
    refHist->Scale(normalize);
    // Rebin the reference histogram
    refHist->Rebin(rebinFactor);
    // Add a header for the table before the loop starts
    std::cout << BOLD << CYAN << "Histogram: " << histName << RESET << std::endl;
    std::cout << BOLD << CYAN << "Sorted Average Ratios--" << RESET << std::endl;
    std::cout << BOLD << CYAN << "Run Number" << "\t" << "Average Ratio" << RESET << std::endl;
    // Iterate over each entry in the runData_ map
    for (const auto& entry : runData_) {
        // Extract the run number and the associated data
        int run = entry.first;
        RunData data = entry.second;

        std::string filePath = baseDir_ + std::to_string(run) + "/qa-" + std::to_string(run) + ".root";
        TFile* file = TFile::Open(filePath.c_str());

        TH1F* hist = (TH1F*)file->Get(histName.c_str());
        if (!hist) continue;

        float normalize_current;
        if (histName == "hTotalMBD") {
            normalize_current = 1.0 / float(refRunData.nEvents);
        } else {
            normalize_current = 1.0 / (float(refRunData.nEvents) * float(refRunData.acceptance));
        }
        // Clone the histogram, then normalize it
        TH1F* ratioHist = (TH1F*)hist->Clone();
        ratioHist->Scale(normalize_current);
        // Rebin the histogram based on the given factor
        ratioHist->Rebin(rebinFactor);
        // Divide by the already-normalized reference histogram
        ratioHist->Divide(refHist);
        for (int bin = 1; bin <= ratioHist->GetNbinsX(); ++bin) {
            ratioHist->SetBinError(bin, 0.0);
        }
        ratioHist->SetLineColor(data.color);
        ratioHist->SetMarkerColor(data.color);
        
        ratioHist->SetTitle(completeTitle.c_str());
        ratioHist->SetStats(0);
        
        /*
         Change Minimum and Maximum x/y values for outputted ratio histograms
         */
        double lowerChiBound, upperChiBound;
        double lowerMBDbound, upperMBDbound;
        double lowerTimingBound, upperTimingBound;
        double lowerTotalCaloBound, upperTotalCaloBound;
        double lowerECoreBound, upperECoreBound;
        if (histName == "hClusterChi") {
            lowerChiBound = 0;
            upperChiBound = 50;
            ratioHist->GetXaxis()->SetRangeUser(lowerChiBound, upperChiBound);
            ratioHist->GetYaxis()->SetRangeUser(0, 2.0);
        } else if (histName == "hTotalMBD") {
            lowerMBDbound = 0;
            upperMBDbound = 1800;
            ratioHist->GetXaxis()->SetRangeUser(lowerMBDbound, upperMBDbound);
            ratioHist->GetYaxis()->SetRangeUser(0, 2.0);
        } else if (histName == "hTotalCaloE") {
            lowerTotalCaloBound = 400.0;
            upperTotalCaloBound = 1050.0;
            ratioHist->GetXaxis()->SetRangeUser(lowerTotalCaloBound, upperTotalCaloBound);
            ratioHist->GetYaxis()->SetRangeUser(0, 2.0);

        } else if (histName == "hClusterECore") {
            lowerECoreBound = 0.0;
            upperECoreBound = 6.0;
            ratioHist->GetXaxis()->SetRangeUser(lowerECoreBound, upperECoreBound);
            ratioHist->GetYaxis()->SetRangeUser(0, 2.0);
        }
        ratioHist->SetMarkerStyle(20);
        ratioHist->SetMarkerSize(0.8);
        ratioHist->SetLineWidth(3);
        if (std::to_string(run) != refRun) { // make sure run number is not the reference run
            if (firstRatio) {
                ratioHist->Draw("P0");
                firstRatio = false;
            } else {
                ratioHist->Draw("P0 SAME");
            }
            // Initialize variables for calculating the average ratio
            double sum = 0.0;  // Sum of the bin contents
            int count = 0;     // Number of non-zero bins considered
            int binStart = 0;
            int binEnd = ratioHist->GetNbinsX(); // Default: all bins
            // Customize the bin range based on the specific histogram type.
            if (histName == "hClusterChi") {
                binStart = ratioHist->FindBin(lowerChiBound);
                binEnd = ratioHist->FindBin(upperChiBound);
            } else if (histName == "hTotalMBD") {
                binStart = ratioHist->FindBin(lowerMBDbound);
                binEnd = ratioHist->FindBin(upperMBDbound);
            } else if (histName == "hTotalCaloE") {
                binStart = ratioHist->FindBin(lowerTotalCaloBound);
                binEnd = ratioHist->FindBin(upperTotalCaloBound);
            } else if (histName == "hClusterECore") {
                binStart = ratioHist->FindBin(lowerECoreBound);
                binEnd = ratioHist->FindBin(upperECoreBound);
            }
            for (int i = binStart; i <= binEnd; ++i) {
                // Get the content of the current bin
                double binContent = ratioHist->GetBinContent(i);
                // Only consider non-zero bins for the average
                if (binContent != 0) {
                    sum += binContent;
                    ++count;
                }
            }
            //calculation of average for nonzero bins
            double average = (count > 0) ? sum / count : 0;
            if (std::to_string(run) != refRun) {
                // Add the average ratio to the sorted map with average as the key and run as the value
                sortedRatios[average] = run;
            }
        }
        if (std::to_string(run) != refRun) {
            ratioLeg->AddEntry(ratioHist, (std::to_string(run)).c_str(), "p");
        }
    }
    // Print sorted ratios and corresponding run numbers after the loop
    HistogramData& hData = globalHistogramData[histName];
    hData.histogramName = histName;
    for (const auto& entry : sortedRatios) {
        std::cout << "Run: " << entry.second << "\tAverage Ratio: " << entry.first << std::endl;
        hData.sortedRatios.emplace_back(entry.second, entry.first);
    }
    ratioLeg->Draw();
    latex.DrawLatex(0.6, 0.85, ("Reference: Run " + refRun).c_str());

    std::string filePathOverlayRatio = baseDir + category + "/RatioPlotOutput/" + histName + "_Ratio.png";
    cRatio->SaveAs(filePathOverlayRatio.c_str());
    
    delete cRatio;
}
void ProcessQA(const std::string& category, const std::unordered_map<int, int>& colorMap) {
    gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();
    std::string normalizeFilePath = baseDir + category + "/Normalize.txt";
    std::unordered_map<int, RunData> runData_;
    std::ifstream infile(normalizeFilePath);
    int run, nEvents, acceptance;
    while (infile >> run >> nEvents >> acceptance) {
        RunData data = {nEvents, acceptance, colorMap.at(run)};  // Use color map to set colors dynamically
        runData_[run] = data;
    }
    infile.close();
    OverlayPlotter overlayPlotter(runData_, QA_ROOT_FILES_PATH, category);
    overlayPlotter.Overlay("hClusterChi", "Cluster_Chi2_Distribution", "Cluster #chi^{2}", "Normalized Counts", category);
    overlayPlotter.Overlay("hTotalMBD", "MBD_Charge_Distribution", "MBD Charge", "Normalized Counts", category);
    overlayPlotter.Overlay("hTotalCaloE", "Total_Calorimeter_Energy_Distribution", "Cluster Energy [GeV]", "Normalized Counts", category);
    overlayPlotter.Overlay("hClusterECore", "Cluster_ECore_Distribution", "Cluster ECore [GeV]", "Normalized Counts", category);

    std::string refRun = "maxHist";    //INPUT ANY RUN NUMBER, or input 'maxHist' see above for how is calculated
    RatioPlot(runData_, QA_ROOT_FILES_PATH, "hClusterChi", refRun, category);
    RatioPlot(runData_, QA_ROOT_FILES_PATH, "hTotalMBD", refRun, category);
    RatioPlot(runData_, QA_ROOT_FILES_PATH, "hTotalCaloE", refRun, category);
    RatioPlot(runData_, QA_ROOT_FILES_PATH, "hClusterECore", refRun, category);
    writeDataToCSV(("/Users/patsfan753/Desktop/p015/QA/" + category + "/sorted_ave_ratios.csv").c_str());
}
void GenerateAdditionalQA() {
    gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();
    
    double totalEntriesE_MB = 0.0, totalEntriesE_Central = 0.0;
    double totalEntriesEv2_MB = 0.0, totalEntriesEv2_Central = 0.0;
    
    
    // Merge the two vectors for a unified loop
    std::vector<std::string> allRuns;
    allRuns.insert(allRuns.end(), runNumbers_MB.begin(), runNumbers_MB.end());
    allRuns.insert(allRuns.end(), runNumbers_Central.begin(), runNumbers_Central.end());

    for (const auto& run : allRuns) {
        std::string category = "MB";
        
        // Determine which category the run belongs to
        if (std::find(runNumbers_Central.begin(), runNumbers_Central.end(), run) != runNumbers_Central.end()) {
            category = "Central";
        }
        
        std::string filePath = baseDir + "RootFiles/" + run + "/qa-" + run + ".root";
        TFile *file = TFile::Open(filePath.c_str());
        if (!file || file->IsZombie()) continue;
        
        // Get the first histogram
        TH2F *hist = (TH2F *)file->Get("h2TotalMBDCaloE");
        if (!hist) continue;
        
        std::cout << "\033[1m\033[31m" << "Run " << run << ", h2TotalMBDCaloE entries: " << hist->GetEntries() << "\033[0m" << std::endl;
        
        // Get the second histogram
        TH2F *hist_v2 = (TH2F *)file->Get("h2TotalMBDCaloEv2");
        if (!hist_v2) continue;
        
        if (hist && hist_v2) {
            double entriesE = hist->GetEntries();
            double entriesEv2 = hist_v2->GetEntries();
            
            if (category == "MB") {
                totalEntriesE_MB += entriesE;
                totalEntriesEv2_MB += entriesEv2;
            } else {
                totalEntriesE_Central += entriesE;
                totalEntriesEv2_Central += entriesEv2;
            }
        }
        // Calculate the ratio of the number of entries
        double ratio = std::max(hist_v2->GetEntries(), 1.0) / hist->GetEntries(); // Avoid division by zero
    
        /*
         Plotting Total MBD, Centrality Correlations
         */
        TH2F *hist_MBD_Centrality = (TH2F *)file->Get("h2TotalMBDCentrality");
        if (!hist_MBD_Centrality) continue;

        // Common setup for canvases
        TCanvas *c1 = new TCanvas("c1", "CaloE vs MBD", 900, 600);
        TCanvas *c2 = new TCanvas("c2", "CaloE vs MBD v2", 900, 600);
        TCanvas *c_MBD_centrality = new TCanvas("c_MBD_centrality", "MBD, Centrality Correlations", 900, 600);
        
        
        // Plotting for h2TotalMBDCaloE
        c1->cd();
        c1->SetRightMargin(0.15);
        gStyle->SetOptStat(0);
        hist->GetXaxis()->SetTitle("Total EMCal Energy [Arb]");
        hist->GetYaxis()->SetTitle("Total MBD Charge [Arb]");
        hist->GetZaxis()->SetTitle("Counts"); // Set the Z-axis title
        hist->SetTitle(Form("p013, Central Triggered CaloE vs MBD (Run: %s)", run.c_str()));
        hist->Draw("COLZ");
        hist->GetZaxis()->SetRangeUser(1, 100000);
        c1->SetLogz();
        TLegend *leg = new TLegend(0.41,.8,0.61,.9);
        leg->SetFillStyle(0);
        leg->AddEntry("","#it{#bf{sPHENIX}} Internal","");
        leg->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
        leg->Draw("SAME");
        TLatex latex;
        latex.SetNDC();
        latex.SetTextSize(0.05);
        std::string runEntry = "Run: " + run;
        latex.DrawLatex(0.56, 0.34, Form("Run: %s", run.c_str()));
        latex.DrawLatex(0.56, 0.29, Form("Entries: %.0f", hist->GetEntries()));
        std::string outFilePath = baseDir + category + "/EMcal_MBD_correlations/CaloE_MBD_Correlation_" + run + ".png";
        c1->SaveAs(outFilePath.c_str(), "PNG");

        
        
        // Plotting for h2TotalMBDCaloEv2
        c2->cd();
        c2->SetRightMargin(0.15); // Increase the left margin if necessary
        hist_v2->Draw("COLZ");
        hist_v2->GetXaxis()->SetTitle("Total EMCal Energy [GeV]");
        hist->GetYaxis()->SetTitle("Total MBD Charge");
        hist_v2->SetTitle(Form("p013, no PDC, w/ z vtx, CaloE vs MBD negative output (Run: %s)", run.c_str()));
        hist_v2->GetZaxis()->SetRangeUser(.1, 100);
        hist_v2->GetYaxis()->SetRangeUser(0, 5000);
        hist_v2->GetZaxis()->SetTitle("Counts"); // Set the Z-axis title
        TLegend *leg2 = new TLegend(0.44,.81,0.64,.91);
        leg2->SetFillStyle(0);
        leg2->AddEntry("","#it{#bf{sPHENIX}} Internal","");
        leg2->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
        leg2->Draw("SAME");
        TLatex latex2;
        latex2.SetNDC();
        latex2.SetTextSize(0.05);
        latex2.DrawLatex(0.2, 0.78, Form("Run: %s", run.c_str()));
        latex2.DrawLatex(0.2, 0.73, Form("Entries: %.0f", hist_v2->GetEntries()));
        latex2.DrawLatex(0.2, 0.68, Form("Ratio: %.5f", ratio));
        c2->SetLogz();
        std::string outFilePath_v2 = baseDir + category + "/EMcal_MBD_correlations/CaloE_MBD_Correlation_v2_" + run + ".png";
        c2->SaveAs(outFilePath_v2.c_str(), "PNG");
        
        
        
        c_MBD_centrality->cd();
        c_MBD_centrality->SetRightMargin(0.15);
        gStyle->SetOptStat(0);
        hist_MBD_Centrality->GetZaxis()->SetTitle("Counts"); // Set the Z-axis title
        hist_MBD_Centrality->SetTitle(Form("p014, MBD Charge, Centrality Correlations (Run: %s)", run.c_str()));
        hist_MBD_Centrality->Draw("COLZ");
        hist_MBD_Centrality->GetZaxis()->SetRangeUser(1, 100000);
        c_MBD_centrality->SetLogz();
        TLegend *leg_MBD_Centrality = new TLegend(0.13,.18,0.33,0.28);
        leg_MBD_Centrality->SetFillStyle(0);
        leg_MBD_Centrality->AddEntry("","#it{#bf{sPHENIX}} Internal","");
        leg_MBD_Centrality->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
        leg_MBD_Centrality->Draw("SAME");
        TLatex latex_MBD_centrality;
        latex_MBD_centrality.SetNDC();
        latex_MBD_centrality.SetTextSize(0.05);
        latex_MBD_centrality.DrawLatex(0.66, 0.89, Form("Run: %s", run.c_str()));
        std::string outFilePath_MBD_centrality = baseDir + category + "/MBD_Centrality/MBD_Centrality_Correlations" + run + ".png";
        c_MBD_centrality->SaveAs(outFilePath_MBD_centrality.c_str(), "PNG");
        
        
        
        // Handling hCentrality histogram
        TH1F *hCentrality = (TH1F *)file->Get("hCentrality");
        if (hCentrality) {
            TCanvas *c_1Dcentrality = new TCanvas("c_1Dcentrality", "Centrality Distribution", 800, 600);
            hCentrality->Draw(); // Draw the histogram
            hCentrality->SetTitle(Form("Centrality Distribution (Run: %s)", run.c_str()));
            hCentrality->GetXaxis()->SetTitle("Centrality");
            hCentrality->GetYaxis()->SetTitle("Counts");
            TLegend *leg_Centrality = new TLegend(.18,.3,.38 ,.45);
            leg_Centrality->SetFillStyle(0);
            leg_Centrality->AddEntry("","#it{#bf{sPHENIX}} Internal","");
            leg_Centrality->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
            leg_Centrality->Draw("SAME");
            TLatex latexCentrality;
            latexCentrality.SetNDC();
            latexCentrality.SetTextSize(0.05);
            latexCentrality.DrawLatex(0.23, 0.255, Form("p013, Run: %s", run.c_str()));
            latexCentrality.DrawLatex(0.23, 0.19, Form("Entries: %.0f", hCentrality->GetEntries()));
            
            std::string centralityOutFilePath = baseDir + category + "/1D_Centrality/Centrality_Distribution_" + run + ".png";
            c_1Dcentrality->SaveAs(centralityOutFilePath.c_str(), "PNG"); // Save the canvas to a file
            delete c_1Dcentrality; // Cleanup
        }

        //Plotting z-vertex Distributions
        TH1F *hVtxZv = (TH1F *)file->Get("hVtxZv2");
        if (hVtxZv) {
            TCanvas *c_zvtx = new TCanvas("c_zvtx", "Centrality Distribution", 800, 600);
            hVtxZv->Draw(); // Draw the histogram
            hVtxZv->SetTitle(Form("Z-vertex Distribution (Run: %s)", run.c_str())); // Set title with run number
            hVtxZv->GetXaxis()->SetTitle("z [cm]");
            hVtxZv->GetYaxis()->SetTitle("Counts");
            hVtxZv->GetXaxis()->SetRangeUser(-20, 20);
            
            TLegend *leg_zvtx = new TLegend(0.53,0.8,0.7,.9);
            leg_zvtx->SetFillStyle(0);
            leg_zvtx->AddEntry("","#it{#bf{sPHENIX}} Internal","");
            leg_zvtx->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
            leg_zvtx->Draw("SAME");
            TLatex latexZvtx;
            latexZvtx.SetNDC();
            latexZvtx.SetTextSize(0.05);
            latexZvtx.DrawLatex(0.68, 0.7, Form("Run: %s", run.c_str()));
            latexZvtx.DrawLatex(0.68, 0.65, Form("Entries: %.0f", hVtxZv->GetEntries()));
            latexZvtx.DrawLatex(0.68, 0.6, Form("Mean: %.2f", hVtxZv->GetMean())); // Include the mean value
             
            
            std::string zVtx_OutFilePath = baseDir + category + "/Zvertex/Zvtx_Distribution_" + run + ".png";
            c_zvtx->SaveAs(zVtx_OutFilePath.c_str(), "PNG"); // Save the canvas to a file
            delete c_zvtx; // Cleanup
        }
        
        TH2F* h2ClusterEtaPhiWeighted = static_cast<TH2F*>(file->Get("h2ClusterEtaPhiWeighted"));
        TH1F* hNClusters = static_cast<TH1F*>(file->Get("hNClusters"));
        // Validate histograms
        if (!h2ClusterEtaPhiWeighted || !hNClusters) {
            std::cerr << "Failed to get histogram for run " << run << '\n';
            file->Close();
            return;
        }
        // Compute normalization factor
        int nEvents = hNClusters->GetEntries();
        h2ClusterEtaPhiWeighted->Scale(1.0 / nEvents);
        if (h2ClusterEtaPhiWeighted) {
            TCanvas *c2D = new TCanvas("c2D", "Eta-Phi Cluster Energy Distributions", 800, 600);
            h2ClusterEtaPhiWeighted->Draw("COLZ1");
            gPad->SetRightMargin(0.22);
            h2ClusterEtaPhiWeighted->SetStats(kFALSE);
            h2ClusterEtaPhiWeighted->GetZaxis()->SetRangeUser(0, 0.01);  // Fixed Z-axis range for Cluster energy
            h2ClusterEtaPhiWeighted->GetXaxis()->SetTitle("Cluster Id #eta");
            h2ClusterEtaPhiWeighted->GetYaxis()->SetTitle("Cluster Id #phi");
            h2ClusterEtaPhiWeighted->GetZaxis()->SetTitle("Cluster Energy/Event [GeV]");
            h2ClusterEtaPhiWeighted->GetZaxis()->SetTitleOffset(1.3);
            TLatex latex;
            latex.SetNDC();
            latex.SetTextSize(0.045);
            latex.SetTextAlign(23);
            latex.DrawLatex(0.28, 0.99, ("Run Number: " + run).c_str()); // Position the text at the top center of the canvas
            std::string zVtx_OutFilePath = baseDir + category + "/ClusterEnergyDistributions/h2ClusterEtaPhiWeighted_2D_" + run + ".png";
            c2D->SaveAs(zVtx_OutFilePath.c_str(), "PNG");
            delete c2D;
        }
        
        delete c1;
        delete c2;
        file->Close();
        delete file;
    }

    // Output accumulated information for MB and Central
    std::cout << "Accumulated Information for MB Runs:\n";
    std::cout << "Total entries in h2TotalMBDCaloE: " << totalEntriesE_MB << std::endl;
    std::cout << "Total entries in h2TotalMBDCaloEv2: " << totalEntriesEv2_MB << std::endl;
    std::cout << "Total ratio of entries (h2TotalMBDCaloEv2/h2TotalMBDCaloE): "
              << (totalEntriesE_MB > 0 ? totalEntriesEv2_MB / totalEntriesE_MB : 0) << std::endl;

    std::cout << "\nAccumulated Information for Central Runs:\n";
    std::cout << "Total entries in h2TotalMBDCaloE: " << totalEntriesE_Central << std::endl;
    std::cout << "Total entries in h2TotalMBDCaloEv2: " << totalEntriesEv2_Central << std::endl;
    std::cout << "Total ratio of entries (h2TotalMBDCaloEv2/h2TotalMBDCaloE): "
              << (totalEntriesE_Central > 0 ? totalEntriesEv2_Central / totalEntriesE_Central : 0) << std::endl;

    // Print the tables
    std::cout << "\nRun Numbers for MB (Total: " << runNumbers_MB.size() << "):\n";
    for (const auto& run : runNumbers_MB) {
        std::cout << run << " ";
    }
    std::cout << "\n\nRun Numbers for Central (Total: " << runNumbers_Central.size() << "):\n";
    for (const auto& run : runNumbers_Central) {
        std::cout << run << " ";
    }
    std::cout << std::endl;
}
void QA() { //main function
    gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();
    
    EMCalStatistics plotter;

    plotter.ProcessRuns(runNumbers_MB, "MB");
    plotter.ProcessRuns(runNumbers_Central, "Central");
    
    std::unordered_map<int, int> mbColors = {
        {23020, kBlue},
        {23671, kOrange+7},
        {23672, kBlack},
        {23676, kOrange+2},
        {23681, kRed},
        {23682, kCyan+3},
        {23687, kMagenta},
        {23690, kViolet+1},
        {23693, kMagenta+2},
        {23694, kAzure+4},
        {23695, kAzure+2},
        {23696, kBlue+3},
        {23697, kPink-3},
        {23699, kGray+1},
        {23702, kViolet+8},
        {23714, kSpring+5},
        {23726, kAzure+7},
        {23727, kGreen+3},
        {23728, kPink+6},
        {23735, kOrange-7},
        {23737, kCyan-5},
        {23738, kYellow+3},
        {23739, kMagenta-7},
        {23740, kGreen+1},
        {23743, kBlue-7},
        {23745, kRed+1},
        {23746, kViolet-5}
    };

    std::unordered_map<int, int> centralColors = {
        {23536, kBlue},
        {23539, kOrange+7},
        {23543, kBlack},
        {23547, kOrange+2},
        {23550, kRed},
        {23552, kCyan+3},
        {23557, kMagenta},
        {23558, kViolet+1},
        {23561, kMagenta+2},
        {23562, kAzure+4},
        {23563, kAzure+2},
        {23566, kGreen-2},
        {23568, kBlue+3},
        {23572, kPink-3},
        {23574, kGray+1},
        {23575, kViolet+8},
        {23582, kSpring+5},
        {23590, kAzure+7},
        {23592, kGreen+3},
        {23594, kPink+6},
        {23604, kOrange-7},
        {23605, kCyan-5},
        {23617, kYellow+3},
        {23618, kMagenta-7},
        {23619, kGreen+1}
    };
    
    // Process MB runs
    ProcessQA("MB", mbColors);

    // Process Central runs
    ProcessQA("Central", centralColors);
    
    GenerateAdditionalQA();
}

