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
    // Calculate normalization factor for the histogram
    RunData refRunData = runData_.at(std::stoi(refRun));
    float normalize; // Declare normalize before the if-else block
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

        float normalize_current; // Declare normalize before the if-else block
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
            ratioHist->GetYaxis()->SetRangeUser(0, 2.0);  // Set your desired min and max
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

        // General aesthetics for the ratio histogram
        ratioHist->SetMarkerStyle(20);
        ratioHist->SetMarkerSize(0.8);
        ratioHist->SetLineWidth(3);  // Increase line width
        
        // If the run number is not the reference run, draw and analyze
        if (std::to_string(run) != refRun) {
            if (firstRatio) {
                ratioHist->Draw("P0");
                firstRatio = false;
            } else {
                ratioHist->Draw("P0 SAME");
            }
            // Initialize variables for calculating the average ratio
            double sum = 0.0;  // Sum of the bin contents
            int count = 0;     // Number of non-zero bins considered
            
            // Set the default bin range to include all bins in the histogram
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
            // Loop over the specified bin range to calculate the average ratio
            for (int i = binStart; i <= binEnd; ++i) {
                // Get the content of the current bin
                double binContent = ratioHist->GetBinContent(i);
                // Only consider non-zero bins for the average
                if (binContent != 0) {
                    sum += binContent;  // Add the bin content to the sum
                    ++count;            // Increment the count of non-zero bins
                }
            }
            //calculation of average for nonzero bins
            double average = (count > 0) ? sum / count : 0;
            
            //conditional check if current run is different from reference run
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

    // Draw legend for ratio plots
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


    //INPUT DESIRED DIRECTORY THAT includes folders labelled by run number with the specified qa.root within all of them
    OverlayPlotter overlayPlotter(runData_, QA_ROOT_FILES_PATH, category);

    overlayPlotter.Overlay("hClusterChi", "Cluster_Chi2_Distribution", "Cluster #chi^{2}", "Normalized Counts", category);
    overlayPlotter.Overlay("hTotalMBD", "MBD_Charge_Distribution", "MBD Charge", "Normalized Counts", category);
    overlayPlotter.Overlay("hTotalCaloE", "Total_Calorimeter_Energy_Distribution", "Cluster Energy [GeV]", "Normalized Counts", category);
    overlayPlotter.Overlay("hClusterECore", "Cluster_ECore_Distribution", "Cluster ECore [GeV]", "Normalized Counts", category);
    
    
    // Call the new RatioPlot function
    //INPUT ANY RUN NUMBER, or input 'maxHist' see above for how is calculated
    std::string refRun = "maxHist";  // Set your reference run number here
    
    RatioPlot(runData_, QA_ROOT_FILES_PATH, "hClusterChi", refRun, category);
    RatioPlot(runData_, QA_ROOT_FILES_PATH, "hTotalMBD", refRun, category);
    RatioPlot(runData_, QA_ROOT_FILES_PATH, "hTotalCaloE", refRun, category);
    RatioPlot(runData_, QA_ROOT_FILES_PATH, "hClusterECore", refRun, category);
    
    writeDataToCSV(("/Users/patsfan753/Desktop/p015/QA/" + category + "/sorted_ave_ratios.csv").c_str());
}
// Main function for 1D Quality Assurance (QA) plotting.
void QA_1D_Plotting() {
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
}
