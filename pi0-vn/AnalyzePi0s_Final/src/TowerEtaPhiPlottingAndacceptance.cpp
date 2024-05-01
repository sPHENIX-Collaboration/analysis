#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <fstream>
#include <iomanip>

// Define a class to handle Statistics
class EMCalStatistics {
public:
    // Public member function to process multiple runs
    void ProcessRuns(const std::vector<std::string>& runNumbers);
    
    // Public member function to write normalization statistics to a file
    void WriteNormalizationToFile(const std::string& runNumber, int nEvents, double AcceptedBins);

private:
    // Member function to process a single run
    void ProcessSingleRun(const std::string& runNumber);
    
    // Count accepted Clusters based on a threshold
    int CountAcceptedClusters(TH2F* histogram, double threshold);
    
    void DrawAndSaveCanvas(TH2F* h2D, TH1F* h1D, const std::string& runNumber, int nEvents, double mean, double sd, double AcceptedBins);

    
    // Member function to print various statistics to the terminal
    void PrintStatisticsToTerminal(const std::string& runNumber, int nEvents, TH1F* h1D);
    
};

// Process all specified runs in the given list
void EMCalStatistics::ProcessRuns(const std::vector<std::string>& runNumbers) {
    for (const auto& run : runNumbers) {
        ProcessSingleRun(run);
    }
}

// Display the calculated statistics in the terminal
void EMCalStatistics::PrintStatisticsToTerminal(const std::string& runNumber, int nEvents, TH1F* h1D) {
    // Calculate mean, standard deviation, and other statistics (ACCEPTED BIN CALCULATION)
    double mean = h1D->GetMean();
    double sd = h1D->GetStdDev();
    //double cutOff = std::max(0.01, mean - sd);  // Calculate the cut-off
    //double BinValue = h1D->FindBin(cutOff);  // Use the cut-off instead of mean-2*sd
    double BinValue = h1D->FindBin(mean - 3*sd);  // Use the cut-off instead of mean-2*sd
    double AcceptedBins = h1D->Integral(BinValue, 120);
    
    // Printing in terminal with colors and set precision for mean and sd to 5 decimal places
    std::cout << "\033[1;32mRun Number: \033[0m" << runNumber << std::endl;
    std::cout << "\033[1;32mnEvents: \033[0m" << nEvents << std::endl;
    std::cout << "Mean - SD: " << std::fixed << std::setprecision(10) << mean - sd << std::endl;
    std::cout << "Mean - 2*SD: " << std::fixed << std::setprecision(10) << mean - 2*sd << std::endl;
    std::cout << "Mean - 3*SD: " << std::fixed << std::setprecision(10) << mean - 3*sd << std::endl;
    std::cout << "Bin Value: " << std::fixed << std::setprecision(10) << BinValue << std::endl;
    std::cout << "Accepted Bins: " << std::fixed << std::setprecision(10) << AcceptedBins << std::endl;
    
    // Write the calculated statistics to a file
    WriteNormalizationToFile(runNumber, nEvents, AcceptedBins);

    // Now also write the same information to a separate text file in an organized format
    static int runCounter = 0; // Static counter to keep track of the number of runs processed
    std::ofstream statsFile;
    std::string statsFilePath = "/Users/patsfan753/Desktop/p015/QA/Central/ClusterEnergyStatisticsOutput.txt";
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
        
        // Increment the counter for each run processed
        runCounter++;

        // If the counter is a multiple of 7, add an extra newline for spacing
        if(runCounter % 7 == 0) {
            statsFile << "\n\n\n"; // This will insert a blank line for visual separation
        }

        statsFile.close(); // Close the file
    } else {
        std::cerr << "Unable to open file for writing.\n";
    }

    std::cout << "----------------------------------------" << std::endl;
}
// Write the calculated statistics to a file
void EMCalStatistics::WriteNormalizationToFile(const std::string& runNumber, int nEvents, double AcceptedBins) {
    // File handling
    std::ofstream outFile;
    std::string outputFilePath = "/Users/patsfan753/Desktop/p015/QA/Central/NormalizeNO.txt";
    
    static bool firstRun = true; // Static flag to track first run
    if (firstRun) {
        outFile.open(outputFilePath, std::ios::trunc); // truncate only for the first run
        firstRun = false;  // reset flag
    } else {
        outFile.open(outputFilePath, std::ios::app); // append mode for subsequent runs
    }
    
    // Write to file if it's open
    if(outFile.is_open()) {
        outFile << runNumber << "\t" << nEvents << "\t" << AcceptedBins << std::endl;
        outFile.close();
    } else {
        std::cerr << "Failed to open output file for writing.\n";
    }
}

// Main logic to process a single run
void EMCalStatistics::ProcessSingleRun(const std::string& runNumber) {
    // Define base and file paths
    const std::string baseDir = "/Users/patsfan753/Desktop/p015/QA/Central/";
    const std::string filePath = "/Users/patsfan753/Desktop/p015/QA/RootFiles/" + runNumber + "/qa-" + runNumber + ".root";
    
    // Open the ROOT file
    TFile* file = TFile::Open(filePath.c_str());

    // File validation
    if (!file || file->IsZombie()) {
        std::cerr << "Failed to open ROOT file for run " << runNumber << '\n';
        return;
    }

    // Fetch histograms from the ROOT file
    TH2F* h2TowEtaPhiWeighted = static_cast<TH2F*>(file->Get("h2ClusterEtaPhiWeighted"));
    TH1F* hNClusters = static_cast<TH1F*>(file->Get("hNClusters"));

    // Validate histograms
    if (!h2TowEtaPhiWeighted || !hNClusters) {
        std::cerr << "Failed to get histogram for run " << runNumber << '\n';
        file->Close();
        return;
    }

    // Compute normalization factor
    int nEvents = hNClusters->GetEntries();
    h2TowEtaPhiWeighted->Scale(1.0 / nEvents);

    // Update the title to include run number for the 2D histogram
    std::string title2D = "p013, No PDC, w/ z vtx, Cluster Energy Distribution (2D) for Run " + runNumber;
    h2TowEtaPhiWeighted->SetTitle(title2D.c_str());

    // Update the title to include run number for the 1D histogram
    std::string title1D = "Cluster Energy Distribution (1D) for Run " + runNumber;
    
    
    TH1F* h1D_TowEtaPhiWeighted = new TH1F("h1D_TowEtaPhiWeighted", title1D.c_str(), 120, -.02, .1);
    

    // Loop over all eta bins in the 2D histogram
    for (int eta = 1; eta <= h2TowEtaPhiWeighted->GetNbinsX(); ++eta) {

        // Loop over all phi bins in the 2D histogram
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

    DrawAndSaveCanvas(h2TowEtaPhiWeighted, h1D_TowEtaPhiWeighted, runNumber, nEvents, mean, sd, AcceptedBins);
    
    // Call to print the stats table
    PrintStatisticsToTerminal(runNumber,nEvents,h1D_TowEtaPhiWeighted);
    // Close the ROOT file
    file->Close();
}

void EMCalStatistics::DrawAndSaveCanvas(TH2F* h2D, TH1F* h1D, const std::string& runNumber, int nEvents, double mean, double sd, double AcceptedBins) {
    // Canvas for 2D histogram
    TCanvas c2D(("canvas_2D_" + runNumber).c_str(), ("canvas_2D_" + runNumber).c_str(), 900, 600);
    h2D->Draw("COLZ1");
    gPad->SetRightMargin(0.22);

    h2D->SetStats(kFALSE);
    h2D->GetZaxis()->SetRangeUser(0, 0.01);  // Fixed Z-axis range for Cluster energy
//    h2D->GetZaxis()->SetRangeUser(0, 0.03);  // Fixed Z-axis range
    //h2D->GetZaxis()->SetTickLength(0.002); // tick length of 0.02
    h2D->GetXaxis()->SetTitle("Cluster Id #eta");
    h2D->GetYaxis()->SetTitle("Cluster Id #phi");
    h2D->GetZaxis()->SetTitle("Cluster Energy/Event [GeV]"); // Set the Z-axis title
    h2D->GetZaxis()->SetTitleOffset(1.3);
    // Create TLatex object to draw the run number on the 2D histogram
    TLatex latex;
    latex.SetNDC(); // Set coordinates to NDC to keep the text in the same place on the canvas regardless of the canvas size
    latex.SetTextSize(0.045); // Set text size. Adjust as needed.
    latex.SetTextAlign(23); // Center the text horizontally and vertically
    latex.DrawLatex(0.28, 0.99, ("Run Number: " + runNumber).c_str()); // Position the text at the top center of the canvas

    
    
    c2D.SaveAs(("/Users/patsfan753/Desktop/p015/QA/Central/ClusterEnergyDistributions/h2ClusterEtaPhiWeighted_2D_" + runNumber + ".png").c_str());

    // Canvas for 1D histogram
    TCanvas c1D(("canvas_1D_" + runNumber).c_str(), ("canvas_1D_" + runNumber).c_str(), 900, 800);
    
    // Set Y-axis to logarithmic scale
    c1D.SetLogy(1); // 1 to enable log scale, 0 to disable

    h1D->GetXaxis()->SetTitle("Cluster Energy/Event [GeV]");
    h1D->GetYaxis()->SetTitle("Counts");
    h1D->Draw();
    // Legend setup
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
    c1D.SaveAs(("/Users/patsfan753/Desktop/p015/QA/Central/ClusterEnergyDistributions/hClusterEtaPhiWeighted_1D_" + runNumber + ".png").c_str());
}

void TowerEtaPhiPlottingAndacceptance() {
    gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();
    EMCalStatistics plotter;



    plotter.ProcessRuns({
        "23536",  "23539", "23543", "23547", "23550",  "23552", "23557", "23558",
        "23561", "23562", "23563", "23566", "23568",  "23572",  "23574", "23575", "23582",
        "23590", "23592", "23594", "23604", "23605", "23617", "23618", "23619"
    });


//    "23020", "23671", "23672", "23676", "23681", "23682", "23687", "23690", "23693", "23694",
//    "23695", "23696", "23697", "23699", "23702", "23714", "23726", "23727", "23728", "23735",
//    "23737", "23738", "23739", "23740", "23743", "23745", "23746"
}
