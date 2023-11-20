#include <iostream>
#include <string>
#include <vector>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <fstream>  // Include for file operations
#include <iomanip>  // Include for setprecision

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
    
    // Count accepted towers based on a threshold
    int CountAcceptedTowers(TH2F* histogram, double threshold);
    
    // Member function to draw and save canvas for each run
    void DrawAndSaveCanvas(TH2F* h2D, TH1F* h1D, const std::string& runNumber, int nEvents);
    
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
    double BinValue = h1D->FindBin(mean-3*sd);
    double AcceptedBins = h1D->Integral(BinValue, 100);
    
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
    std::cout << "----------------------------------------" << std::endl;

}

// Write the calculated statistics to a file
void EMCalStatistics::WriteNormalizationToFile(const std::string& runNumber, int nEvents, double AcceptedBins) {
    // File handling
    std::ofstream outFile;
    std::string outputFilePath = "/Users/patsfan753/Desktop/Organized_QA/Normalize.txt";
    
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
    const std::string baseDir = "/Users/patsfan753/Desktop/QA_EMCal/UpdatedROOT_10_16/";
    const std::string filePath = baseDir + runNumber + "/qa.root";
    
    // Open the ROOT file
    TFile* file = TFile::Open(filePath.c_str());

    // File validation
    if (!file || file->IsZombie()) {
        std::cerr << "Failed to open ROOT file for run " << runNumber << '\n';
        return;
    }

    // Fetch histograms from the ROOT file
    TH2F* h2TowEtaPhiWeighted = static_cast<TH2F*>(file->Get("h2TowEtaPhiWeighted"));
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
    std::string title2D = "Tower Energy Distribution (2D) for Run " + runNumber;
    h2TowEtaPhiWeighted->SetTitle(title2D.c_str());

    // Update the title to include run number for the 1D histogram
    std::string title1D = "Tower Energy Distribution (1D) for Run " + runNumber;
    TH1F* h1D_TowEtaPhiWeighted = new TH1F("h1D_TowEtaPhiWeighted", title1D.c_str(), 100, 0, .1);
    

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

    
    // Call to draw and save the canvas
    DrawAndSaveCanvas(h2TowEtaPhiWeighted, h1D_TowEtaPhiWeighted, runNumber, nEvents);
    // Call to print the stats table
    PrintStatisticsToTerminal(runNumber,nEvents,h1D_TowEtaPhiWeighted);
    // Close the ROOT file
    file->Close();
}

void EMCalStatistics::DrawAndSaveCanvas(TH2F* h2D, TH1F* h1D, const std::string& runNumber, int nEvents) {
    // Canvas for 2D histogram
    TCanvas c2D(("canvas_2D_" + runNumber).c_str(), ("canvas_2D_" + runNumber).c_str(), 8000, 6000);  // Increased resolution
    h2D->Draw("COLZ");
    h2D->SetStats(kFALSE);
    h2D->GetZaxis()->SetRangeUser(0, 0.07);  // Fixed Z-axis range from 0 to 0.07
    //h2D->GetZaxis()->SetNdivisions(510, kFALSE); // 5 primary divisions, 10 secondary divisions
    h2D->GetZaxis()->SetTickLength(0.01); // tick length of 0.02
    h2D->GetXaxis()->SetTitle("Tower Id #eta");
    h2D->GetYaxis()->SetTitle("Tower Id #phi");
    h2D->SetTitleSize(0.04, "XYZ");
    h2D->SetTitleOffset(1.2, "X");
    h2D->SetTitleOffset(1.2, "Y");
    h2D->SetLabelSize(0.03, "XYZ");
    h2D->SetTitleFont(42, "XYZ");
    h2D->SetLabelFont(42, "XYZ");

    // Adjust palette position and size to make it clearly visible
    gPad->Update();
    TPaletteAxis* palette = (TPaletteAxis*)h2D->GetListOfFunctions()->FindObject("palette");
    if (palette) {
        palette->SetX1NDC(0.9);
        palette->SetX2NDC(0.92);
        palette->SetY1NDC(0.1);
        palette->SetY2NDC(0.9);
    }
    c2D.SaveAs(("/Users/patsfan753/Desktop/Organized_QA/h2TowerOutput_1017/h2TowEtaPhiWeighted_2D_" + runNumber + ".png").c_str());

    // Canvas for 1D histogram
    TCanvas c1D(("canvas_1D_" + runNumber).c_str(), ("canvas_1D_" + runNumber).c_str(), 800, 800);
    gPad->SetLogy();
    h1D->GetXaxis()->SetTitle("Tower Energy/Event [GeV]");
    h1D->GetYaxis()->SetTitle("Counts");
    h1D->Draw();
    c1D.SaveAs(("/Users/patsfan753/Desktop/Organized_QA/h2TowerOutput_1017/h2TowEtaPhiWeighted_1D_" + runNumber + ".png").c_str());
}

void TowerEtaPhiPlottingAndacceptance() {
    EMCalStatistics plotter;
    /*
     Can input more run numbers below
     
     */
    plotter.ProcessRuns({"21518", "21520", "21615","21598", "21599", "21796", "21813", "21889", "21891", "22949", "22950", "22951", "22979", "22982"});
}
