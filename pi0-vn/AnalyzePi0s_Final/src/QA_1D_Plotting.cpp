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
// ANSI escape codes for text formatting
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

// Global constants
constexpr char ORGANIZED_QA_PATH[] = "/Users/patsfan753/Desktop/Desktop/Organized_QA/"; //for output of Plots
const std::string QA_ROOT_FILES_PATH = "/Users/patsfan753/Desktop/p015/QA/RootFiles/";  //current QA root Files location
const std::string NORMALIZE_FILE_PATH = "/Users/patsfan753/Desktop/p015/QA/Central/Normalize.txt";


const std::string percentileFileListPath = ORGANIZED_QA_PATH + std::string("AllgoodRuns_MBD_Percentiles.txt");


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
    OverlayPlotter(const std::unordered_map<int, RunData>& runData, const std::string& baseDir) :
        runData_(runData), baseDir_(baseDir) {}
    
    // Method to get output path based on histogram name
    // Returns an absolute path where the individual histograms will be saved.
    std::string GetOutputPath_SingleHists(const std::string& histName) {
        std::string basePath = ORGANIZED_QA_PATH; // Use the global constant

        if (histName == "hClusterChi") {
            return basePath + "IndividualPlotOutput/chi2/";
        }
        if (histName == "hClusterECore") {
            return basePath + "IndividualPlotOutput/ECore/";
        }
        if (histName == "hTotalCaloE") {
            return basePath + "IndividualPlotOutput/TotalCaloE/";
        }
        if (histName == "hTotalMBD") {
            return basePath + "IndividualPlotOutput/MBD/";
        }
//        if (histName == "hClusterTime") {
//            return basePath + "IndividualPlotOutput/TimingQA/";
//        }

        return "";
    }
    
    // Hash map to store distance of each run's histogram from the average histogram
    std::unordered_map<std::string, double> runDistanceFromAverage_;
    
    // Method to draw the average histogram on the same canvas
    // This is an overlay histogram representing the average of all runs
    void DrawAverageHistogram(TH1F* accumulatedHist, TLegend* leg, int nRuns) {
        if (nRuns == 0 || !accumulatedHist) return;

        // Normalize the accumulated histogram by the number of runs
        for (int bin = 1; bin <= accumulatedHist->GetNbinsX(); ++bin) {
            double content = accumulatedHist->GetBinContent(bin) / nRuns;
            accumulatedHist->SetBinContent(bin, content);
        }
        
        // Set line attributes for average histogram
        accumulatedHist->SetLineColor(kRed+2);
        accumulatedHist->SetLineStyle(1);
        accumulatedHist->SetLineWidth(4);
        accumulatedHist->Draw("HIST SAME");
        //leg->AddEntry(accumulatedHist, "AVERAGE HISTOGRAM", "l");
    }
    
    
    // Method to calculate the Euclidean distance of the run's histogram from the average histogram
    // This will be used to analyze how each run deviates from the average behavior.
    void CalculateDistanceFromAverage(const std::string& run, TH1F* hist, TH1F* accumulatedHist_, int nRunsProcessed) {
        if (!accumulatedHist_) return;
        
        // Create a clone of the accumulated histogram and normalize it
        TH1F* avgHist = (TH1F*)accumulatedHist_->Clone("avgHist");
        for (int bin = 1; bin <= avgHist->GetNbinsX(); ++bin) {
            double content = avgHist->GetBinContent(bin) / nRunsProcessed;
            avgHist->SetBinContent(bin, content);
        }
        
        // Calculate the Euclidean distance from the average histogram
        double distance = 0;
        for (int i = 1; i <= hist->GetNbinsX(); ++i) {
            double diff = hist->GetBinContent(i) - avgHist->GetBinContent(i);
            distance += diff * diff;
        }
        
        // Store the calculated distance
        distance = sqrt(distance);
        runDistanceFromAverage_[run] = distance;
        
        // Clean up the cloned histogram
        delete avgHist;
    }
    // Method to convert color code to ANSI color string
    std::string ConvertToAnsiColor(int colorCode) {
        if (colorCode == kBlue) return "Blue";
        if (colorCode == kOrange+7) return "Modified Orange"; // Or some ANSI code
        if (colorCode == kBlack) return "Black";
        if (colorCode == kOrange+2) return "Brown"; // Or some ANSI code
        if (colorCode == kRed) return "Red";
        if (colorCode == kCyan+3) return "Modified Cyan"; // Or some ANSI code
        if (colorCode == kMagenta) return "Magenta";
        if (colorCode == kViolet+1) return "Modified Violet"; // Or some ANSI code
        if (colorCode == kMagenta+2) return "Light Magenta"; // Or some ANSI code
        if (colorCode == kAzure+4) return "Modified Azure"; // Or some ANSI code
        if (colorCode == kAzure+2) return "Light Azure"; // Or some ANSI code
        if (colorCode == kBlue+3) return "Light Blue"; // Or some ANSI code
        if (colorCode == kPink-3) return "Modified Pink"; // Or some ANSI code
        if (colorCode == kGray+1) return "Light Gray"; // Or some ANSI code
        return WHITE;
    }
    // Method to overlay histograms (AND PLOTS SINGLE HISTOGRAMS), given a histogram name, title, and axis labels
    void Overlay(const std::string& histName, const std::string& title, const std::string& xAxis, const std::string& yAxis) {
        bool addAverageToLegend = false;
        // Create a new canvas for plotting
        TCanvas* c = new TCanvas();
        c->SetTitle(title.c_str());
        c->SetLogy();  // Setting the y-axis to logarithmic scale
        
        // Vector to hold (run number, max bin content)
        std::vector<std::pair<int, double>> runMaxBinTable;
        
        // Flags to identify if it's the first run of the loop
        bool firstRun = true;
        TH1F* firstHist = nullptr; // Reference to first histogram
        
        // Null pointer to hold sum of histograms for average calculations
        TH1F* accumulatedHist_ = nullptr;  // Initialize to nullptr
        int nRunsProcessed = 0; // Counter for the number of processed runs
        
        int nRuns = runData_.size();
        
        // Initialize legend settings
        TLegend *leg = new TLegend(0.72, 0.5, 0.92, 0.92);
        leg->SetNColumns(2);
        leg->SetTextSize(0.029);
        
        // Log information header
        std::cout << BOLD << GREEN
                  << std::setw(10) << std::left << "Run"
                  << std::setw(20) << std::left << "Distance from Avg"
                  << std::setw(10) << std::left << "Color"
                  << std::setw(20) << std::left << "N Events"
                  << std::setw(20) << std::left << "Acceptance"
                  << RESET << std::endl;

        std::cout << BOLD << YELLOW << std::string(80, '-') << RESET << std::endl;
        
        // Loop through all the run data
        for (const auto& entry : runData_) {
            int run = entry.first;  // Extract run number
            RunData data = entry.second;  // Extract associated RunData struct
            
            // Construct file path to corresponding 'qa.root' file for the current run
            std::string filePath = QA_ROOT_FILES_PATH + std::to_string(run) + "/qa-" + std::to_string(run) + ".root";
            // Open the ROOT file
            TFile* file = TFile::Open(filePath.c_str());
            // Retrieve the specific histogram based on histName
            TH1F* hist = (TH1F*)file->Get(histName.c_str());
            hist->SetStats(0);  // Remove the statistics box for overlayed plots
            // Continue to next iteration if histogram not found
            if (!hist) continue;
            
            // Calculate normalization factor for the histogram
            float normalize;
            if (histName == "hTotalMBD") {
                normalize = 1.0 / float(data.nEvents);
            } else {
                normalize = 1.0 / (float(data.nEvents) * float(data.acceptance));
            }

            // Apply normalization
            hist->Scale(normalize);
            
            
            // Set the line color for the histogram
            hist->SetLineColor(data.color);  // Removed the fallback to `currentColor`
            hist->SetStats(0);  // Remove the statistics box

            
            // If this is the first run, draw the histogram; otherwise overlay it
            if (firstRun) {
                hist->Draw("HIST");  // First plot
                firstHist = hist;    // Store reference to first histogram
                firstRun = false;    // Mark first run as processed
            } else {
                hist->Draw("HIST SAME");  // Overlay plot
            }
           hist->SetMaximum(hist->GetBinContent(hist->GetMaximumBin()) * 10);
//            
            
            
            /*
             CENTRALITY BINNING CODE
             */
            // Open the file stream for percentiles outside the loop over runs
            std::ofstream percentileFile(percentileFileListPath, std::ios::app);
            

            if (histName == "hTotalMBD") {
                // Create the Cumulative Distribution Function (CDF) histogram
                // We're initializing a histogram identical in structure to the original histogram,
                // but we'll populate it with the CDF data.
                // "CDF_" + std::to_string(run): creates a unique name for the CDF histogram based on the run number
                // hist->GetNbinsX(), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax(): preserve the bin settings of the original histogram
                TH1F* hMBDChargeCDF = new TH1F(("CDF_" + std::to_string(run)).c_str(), ("CDF for run " + std::to_string(run)).c_str(), hist->GetNbinsX(), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());

                // Loop over each bin in the original histogram to fill the CDF histogram
                // Integral(1, i) gets the integral of the histogram from the first bin to the i-th bin.
                // We store this integral value in the corresponding bin of the CDF histogram.
                for (int i = 1; i <= hist->FindBin(2000); ++i) {
                    float integral = hist->Integral(1, i);
                    hMBDChargeCDF->SetBinContent(i, integral);
                }

                // Normalize the CDF histogram
                // The maximum value of the CDF should be 1, so we scale the histogram by 1 over its maximum value.
                hMBDChargeCDF->Scale(1.0 / hMBDChargeCDF->GetMaximum());

                std::cout << "\033[1;35mRun " << run << "\033[0m" << std::endl;
                std::cout << "\033[1;36mCentrality\tMBD Value\033[0m" << std::endl;
                percentileFile << "Run " << run << std::endl;
                percentileFile << "Centrality\tMBD Value" << std::endl;

                // Defining the percentiles we are interested in
                std::vector<float> percentiles = {0.2, 0.4, 0.6, 0.8};

                // Loop over each percentile
                for (float percentile : percentiles) {
                    int bin = hMBDChargeCDF->FindFirstBinAbove(percentile);
                    float mbdValue = hMBDChargeCDF->GetBinCenter(bin);
                    
                    // Convert percentile to centrality
                    float centrality = 1 - percentile;
                    
                    // Write the centrality and corresponding MBD value to the text file and console.
                    percentileFile << centrality << "\t" << mbdValue << std::endl;
                    std::cout << "\033[1;32m" << centrality << "\t\033[1;34m" << mbdValue << "\033[0m" << std::endl;
                }
                // Write an empty line to separate the information of different runs in the text file
                percentileFile << std::endl;

                // Cleanup
                delete hMBDChargeCDF;
            }

            // Close the file after looping over all runs
            percentileFile.close();


            
            
            
            
            // Add current histogram to the legend
            leg->AddEntry(hist, (std::to_string(run)).c_str(), "l");
            // Get maximum bin content
            double maxBinContent = hist->GetBinContent(hist->GetMaximumBin());
            // Add run and max bin content to the vector
            runMaxBinTable.emplace_back(run, maxBinContent);

            // Output individual histograms to separate files
            std::string outputPath = GetOutputPath_SingleHists(histName);
            if (!outputPath.empty()) {
                // Clone the histogram for individual saving
                TH1F* clonedHist = (TH1F*)hist->Clone(); // Clone the histogram
                // Create a new canvas for this specific histogram
                TCanvas* singleCanvas = new TCanvas();
                
                
                singleCanvas->SetLogy(1);  // Set the y-axis to logarithmic scale
                clonedHist->SetStats(1);   // Show the statistics box only for single histograms
                // Draw and save histogram
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
                
                // Clean up
                delete singleCanvas;
                delete clonedHist; // Delete the cloned histogram
            }
            
            // If first run, initialize accumulatedHist_; otherwise add current histogram to it
            if (!accumulatedHist_) {
                accumulatedHist_ = (TH1F*)hist->Clone(("accumulated_" + histName).c_str());
            } else {
                accumulatedHist_->Add(hist);
            }
            // Increment counter for processed runs
            nRunsProcessed++;
            // Calculate how far the run is from the average
            CalculateDistanceFromAverage(std::to_string(run), hist, accumulatedHist_, nRunsProcessed);
            
            // Enhanced row output
            std::string color = data.color < 400 ? RED : (data.color < 800 ? YELLOW : GREEN);
            std::cout << BOLD << color
                      << std::setw(10) << std::left << run
                      << std::setw(20) << std::left << runDistanceFromAverage_[std::to_string(run)]
                      << std::setw(10) << std::left << data.color
                      << std::setw(20) << std::left << data.nEvents
                      << std::setw(20) << std::left << data.acceptance
                      << RESET << std::endl;
        }
        // Sort based on max bin content in descending order
        std::sort(runMaxBinTable.begin(), runMaxBinTable.end(),
                  [](const auto& a, const auto& b) {
                      return b.second < a.second;
                  });
        // Output the sorted table with ANSI color codes
        std::cout << BOLD << CYAN << "Sorted Runs:" << RESET << std::endl;
        // Add a header for the table before the loop starts
        std::cout << BOLD << CYAN << "Histogram: " << histName << RESET << std::endl;

        // Table header
        std::cout << std::setw(12) << "Run"
                  << std::setw(20) << "Max Bin Content" << std::endl;
        std::cout << std::string(52, '-') << std::endl;

        // Table content
        for (const auto& entry : runMaxBinTable) {
            int run = entry.first;
            double maxBinContent = entry.second;
            RunData data = runData_.at(run);
            std::string colorString = ConvertToAnsiColor(data.color);  // This function should now return just the name of the color
            std::cout << std::setw(12) << run
                      << std::setw(20) << maxBinContent << std::endl;
        }
        // Draw legend
        leg->SetBorderSize(0);
        leg->Draw();

        TLegend *leg1 = new TLegend(0.3,.8,0.5,0.9);
        leg1->SetFillStyle(0);
        leg1->AddEntry("","#it{#bf{sPHENIX}} Internal","");
        leg1->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
        leg1->Draw("same");
        
        // Set x-axis and y-axis titles if at least one histogram was processed
        if (firstHist) {
            firstHist->SetTitle(title.c_str());
            firstHist->GetXaxis()->SetTitle(xAxis.c_str());
            firstHist->GetYaxis()->SetTitle(yAxis.c_str());
        }
        
        // Draw average histogram based on all the processed runs
        DrawAverageHistogram(accumulatedHist_, leg, nRunsProcessed);
        
        // Conditional check before adding the average histogram to the legend
        if (addAverageToLegend) {
            leg->AddEntry(accumulatedHist_, "AVERAGE HISTOGRAM", "l");
        }
        // Save the canvas as a PNG file
        std::string filePathOverlayedWithAve = std::string(ORGANIZED_QA_PATH) + "OverlayedWithAverage/" + title + ".png";
        c->SaveAs(filePathOverlayedWithAve.c_str());
        // Cleanup: Delete allocated objects
        if (accumulatedHist_) {
            delete accumulatedHist_;
            accumulatedHist_ = nullptr;
        }
        
        std::string filePathOverlay = std::string(ORGANIZED_QA_PATH) + "OverlayedOutput/" + title + ".png";
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
void RatioPlot(const std::unordered_map<int, RunData>& runData_, const std::string& baseDir_, const std::string& histName, std::string& refRun) {
    TCanvas* cRatio = new TCanvas();

    bool firstRatio = true;
    TH1F* refHist = nullptr;
    // Map to store the ratio data
    std::map<double, int, std::greater<>> sortedRatios;
    // Combine the title here
    std::string completeTitle = histName + " Ratio (Reference Run: " + refRun + ")";
    cRatio->SetTitle(completeTitle.c_str());
    
    // Initialize legend for ratio plots
    TLegend* ratioLeg;
    // Initialization of TLatex for displaying max histogram information
    TLatex latex;
    latex.SetTextSize(0.05);
    latex.SetNDC(true);
    if (histName == "hClusterChi") {
        // TLegend(x-axis start, y-axis start, x-axis end, y-axis end)
        ratioLeg = new TLegend(0.2, 0.6, 0.4, 0.92);
        ratioLeg->SetNColumns(4);
    } else if (histName == "hTotalMBD") {
        ratioLeg = new TLegend(0.2, 0.6, 0.4, 0.92);
        ratioLeg->SetNColumns(4);
    } else if (histName =="hTotalCaloE") {
        ratioLeg = new TLegend(0.2, 0.6, 0.4, 0.92);
        ratioLeg->SetNColumns(4);
    } else {
        ratioLeg = new TLegend(0.2, 0.6, 0.4, 0.92);
        ratioLeg->SetNColumns(2);
    }

    ratioLeg->SetNColumns(2);
    ratioLeg->SetBorderSize(0);
    ratioLeg->SetMargin(0.15);
    ratioLeg->SetTextSize(0.025);
    
    
    // Rebinning based on histogram name
    int rebinFactor = 1;  // Default rebin factor
    if (histName == "hClusterChi") {
        rebinFactor = 10;  // Set your desired rebin factor
    } else if (histName == "hTotalMBD") {
        rebinFactor = 10;  // Set your desired rebin factor
    } else if (histName == "hTotalCaloE") {
        rebinFactor = 50;  // Set your desired rebin factor
    } else if (histName == "hClusterECore") {
        rebinFactor = 1;  // Set your desired rebin factor
    }
    
    // Find maxHist if specified
    double maxIntegral = 0.0;
    if (refRun == "maxHist") {
        for (const auto& entry : runData_) {
            int run = entry.first;
            std::string filePath = baseDir_ + std::to_string(run) + "/qa-" + std::to_string(run) + ".root";
            TFile* file = TFile::Open(filePath.c_str());
            TH1F* hist = (TH1F*)file->Get(histName.c_str());
            if (!hist) continue;
            double integral = hist->Integral();
            if (integral > maxIntegral) {
                maxIntegral = integral;
                refRun = std::to_string(run);
            }
        }
        // Update completeTitle with the actual run number for maxHist
        completeTitle = histName + " Ratio (Reference Run: " + refRun + ")";
    }

    std::cout << "For histogram " << histName << ", the run with maximum integral is Run " << refRun << std::endl;
    std::string refFilePath = baseDir_ + refRun + "/qa-" + refRun + ".root";
    TFile* refFile = TFile::Open(refFilePath.c_str());
    refHist = (TH1F*)refFile->Get(histName.c_str());
    refHist->SetDirectory(0); // Important to keep histogram when file closes
    
    // Calculate normalization factor for the histogram
    //fetch RunData structure corresponding to reference run from unordered map runData_
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
        // Construct the filepath to the 'qa.root'
        std::string filePath = baseDir_ + std::to_string(run) + "/qa-" + std::to_string(run) + ".root";
        // Open the ROOT file
        TFile* file = TFile::Open(filePath.c_str());

        // Get the specified histogram from the file
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
        
        // Set line and marker color for the ratio histogram
        ratioHist->SetLineColor(data.color);
        ratioHist->SetMarkerColor(data.color);
        
        ratioHist->SetTitle(completeTitle.c_str());  // Set the combined title here
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
            // This allows for focusing on specific x-value ranges for different histograms.
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

    std::string filePathOverlayRatio = std::string(ORGANIZED_QA_PATH) + "OverlayedRatioOutput/" + histName + "_Ratio.png";
    cRatio->SaveAs(filePathOverlayRatio.c_str());
    
    delete cRatio;
}
// Main function for 1D Quality Assurance (QA) plotting.
void QA_1D_Plotting() {
    gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();
    // Initialize an unordered_map to hold run data.
    std::unordered_map<int, RunData> runData_;
    // Read from Normalize.txt to populate runData_ with run number, number of events, and acceptance.
    std::ifstream infile(NORMALIZE_FILE_PATH);
    
    int run, nEvents, acceptance;
    
    // Loop to read from Normalize.txt
    // Each line in the file contains a run number, the number of events for that run, and the acceptance value.
    // Reads each line from the file and populates the 'runData_' map with the read values.
    // The key for the map is the run number ('run'), and the value is a RunData struct containing nEvents, acceptance, and a placeholder for color (initialized to 0).
    while (infile >> run >> nEvents >> acceptance) {
        RunData data = {nEvents, acceptance, 0};
        runData_[run] = data;
    }
    
    // Hardcode colors
//    runData_[23020].color = kBlue;
//    runData_[23671].color = kOrange+7;
//    runData_[23672].color = kBlack;
//    runData_[23676].color = kOrange+2;
//    runData_[23681].color = kRed;
//    runData_[23682].color = kCyan+3;
//    runData_[23687].color = kMagenta;
//    runData_[23690].color = kViolet+1;
//    runData_[23693].color = kMagenta+2;
//    runData_[23694].color = kAzure+4;
//    runData_[23695].color = kAzure+2;
//    runData_[23696].color = kBlue+3;
//    runData_[23697].color = kPink-3;
//    runData_[23699].color = kGray+1;
//    runData_[23702].color = kViolet+8;
//    runData_[23714].color = kSpring+5;
//    runData_[23726].color = kAzure+7;
//    runData_[23727].color = kGreen+3;
//    runData_[23728].color = kPink+6;
//    runData_[23735].color = kOrange-7;
//    runData_[23737].color = kCyan-5;
//    runData_[23738].color = kYellow+3;
//    runData_[23739].color = kMagenta-7;
//    runData_[23740].color = kGreen+1;
//    runData_[23743].color = kBlue-7;
//    runData_[23745].color = kRed+1;
//    runData_[23746].color = kViolet-5;
    
    runData_[23536].color = kBlue;
    runData_[23539].color = kOrange+7;
    runData_[23543].color = kBlack;
    runData_[23547].color = kOrange+2;
    runData_[23550].color = kRed;
    runData_[23552].color = kCyan+3;
    runData_[23557].color = kMagenta;
    runData_[23558].color = kViolet+1;
    runData_[23561].color = kMagenta+2;
    runData_[23562].color = kAzure+4;
    runData_[23563].color = kAzure+2;
    runData_[23566].color = kGreen-2;
    runData_[23568].color = kBlue+3;
    runData_[23572].color = kPink-3;
    runData_[23574].color = kGray+1;
    runData_[23575].color = kViolet+8;
    runData_[23582].color = kSpring+5;
    runData_[23590].color = kAzure+7;
    runData_[23592].color = kGreen+3;
    runData_[23594].color = kPink+6;
    runData_[23604].color = kOrange-7;
    runData_[23605].color = kCyan-5;
    runData_[23617].color = kYellow+3;
    runData_[23618].color = kMagenta-7;
    runData_[23619].color = kGreen+1;


    
    //INPUT DESIRED DIRECTORY THAT includes folders labelled by run number with the specified qa.root within all of them
    OverlayPlotter overlayPlotter(runData_, QA_ROOT_FILES_PATH);

    overlayPlotter.Overlay("hClusterChi", "Cluster_Chi2_Distribution", "Cluster #chi^{2}", "Normalized Counts");
    overlayPlotter.Overlay("hTotalMBD", "MBD_Charge_Distribution", "MBD Charge", "Normalized Counts");
    overlayPlotter.Overlay("hTotalCaloE", "Total_Calorimeter_Energy_Distribution", "Cluster Energy [GeV]", "Normalized Counts");
    overlayPlotter.Overlay("hClusterECore", "Cluster_ECore_Distribution", "Cluster ECore [GeV]", "Normalized Counts");
    //overlayPlotter.Overlay("hClusterTime", "Cluster_Timing_Distribution", "Timing Peak Location", "Counts");
    
    
    // Call the new RatioPlot function
    //INPUT ANY RUN NUMBER, or input 'maxHist' see above for how is calculated
    std::string refRun = "maxHist";  // Set your reference run number here
    
    
    RatioPlot(runData_, QA_ROOT_FILES_PATH, "hClusterChi", refRun);
    RatioPlot(runData_, QA_ROOT_FILES_PATH, "hTotalMBD", refRun);
    RatioPlot(runData_, QA_ROOT_FILES_PATH, "hTotalCaloE", refRun);
    RatioPlot(runData_, QA_ROOT_FILES_PATH, "hClusterECore", refRun);
    
    writeDataToCSV("/Users/patsfan753/Desktop/p015/QA/Central/sorted_ave_ratios.csv");
    
    // For calculating and storing average MBD values
    std::unordered_map<double, std::vector<int>> avgMBDData;

    // Read MBD values from text file
    std::ifstream mbdFile(percentileFileListPath);
    std::string line;
    double centrality;
    int mbdValue;
    std::string runHeader;

    while (std::getline(mbdFile, line)) {
        std::istringstream iss(line);
        if (line.find("Run") != std::string::npos || line.find("Centrality") != std::string::npos) {
            // Skip "Run" and "Centrality" headers
            continue;
        }
        iss >> centrality >> mbdValue;
        avgMBDData[centrality].push_back(mbdValue);
    }

    // Calculate averages
    std::cout << "\033[1;31mAverage MBD Values for Each Centrality:\033[0m" << std::endl;
    for (const auto& entry : avgMBDData) {
        double total = 0.0;
        for (const int& val : entry.second) {
            total += val;
        }
        double avg = total / entry.second.size();
        std::cout << "Centrality: " << entry.first << " Avg MBD: " << avg << std::endl;
    }
}
