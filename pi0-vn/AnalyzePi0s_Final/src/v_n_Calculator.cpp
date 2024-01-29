#include <TFile.h>
#include <TH1F.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

void v_n_Calculator() {
    // Define the histogram names for QQ
    // Open the ROOT file
    TFile *file = new TFile("/Users/patsfan753/Desktop/Test.root", "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file or file is not a valid ROOT file" << std::endl;
        return;
    }

    // Define the histogram names for QQ within the vn/QQ directory
    std::string hist_name_QQ[2] = {"vn/QQ/hQQ_40-60", "vn/QQ/hQQ_20-40"};

    // Define the histogram names for qQ within the vn/qQ directory
    std::string hist_name_qq[12];
    for (int i = 0; i < 12; ++i) {
        hist_name_qq[i] = "vn/qQ/hqQ_" + std::to_string(i);
    }

    // Function to draw and save histograms
    auto drawAndSaveHist = [&](const std::string& histPath, const std::string& savePath) {
        TH1F *hist = (TH1F*)file->Get(histPath.c_str());
        if (!hist) {
            std::cerr << "Error: Histogram " << histPath << " not found" << std::endl;
            return;
        }
        TCanvas *c = new TCanvas();
        c->SetLogy(); // Set logarithmic scale for y-axis
        hist->Draw();
        c->SaveAs(savePath.c_str());
        delete c;
    };

    // Process and save QQ histograms
    for (const auto& histPath : hist_name_QQ) {
        std::string savePath = "/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/QQ_qqHists/" + histPath.substr(6) + ".png";
        drawAndSaveHist(histPath, savePath);
    }

    // Process and save qQ histograms
    for (const auto& histPath : hist_name_qq) {
        std::string savePath = "/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/QQ_qqHists/" + histPath.substr(6) + ".png";
        drawAndSaveHist(histPath, savePath);
    }
    
    // Retrieve and calculate QQ means and their errors
    float QQ_mean[2], QQ_error[2];
    for (int i = 0; i < 2; ++i) {
        TH1F *h_QQ = (TH1F*)file->Get(hist_name_QQ[i].c_str());
        if (!h_QQ) {
            std::cerr << "Error: QQ histogram " << hist_name_QQ[i] << " not found" << std::endl;
            file->Close();
            delete file;
            return;
        }
        QQ_mean[i] = h_QQ->GetMean();
        float stdDev = h_QQ->GetStdDev();
        int nEntries = h_QQ->GetEntries();
        QQ_error[i] = stdDev / sqrt(nEntries); // Standard deviation divided by the square root of the number of entries
    }
    
    
    // Retrieve and calculate qQ means and their errors
    float qQ_mean[12], qQ_error[12];
    for (int i = 0; i < 12; ++i) {
        TH1F *h_qQ = (TH1F*)file->Get(hist_name_qq[i].c_str());
        if (!h_qQ) {
            std::cerr << "Error: qQ histogram " << hist_name_qq[i] << " not found" << std::endl;
            file->Close();
            delete file;
            return;
        }
        qQ_mean[i] = h_qQ->GetMean();
        float stdDev = h_qQ->GetStdDev();
        int nEntries = h_qQ->GetEntries();
        qQ_error[i] = stdDev / sqrt(nEntries);
    }

    std::ifstream inFile("/Users/patsfan753/Desktop/PlotByPlotOutputJamies_1_27_24.csv");
    std::ofstream outFile("/Users/patsfan753/Desktop/TestUpdate2.csv");

    std::string line;
    int lineIndex = 0;

    while (getline(inFile, line)) {
        if (lineIndex == 0) { // Header line
            // Add headers for the new columns
            outFile << line << ",v2,v2_error" << std::endl;
        } else {
            std::stringstream ss(line);
            std::string token;
            getline(ss, token, ',');
            int idx = std::stoi(token); // Get the index from the CSV row

            // Determine QQ index based on CSV index
            int QQ_index = idx / 6; // 0 for indices 0-5, 1 for indices 6-11
            float v2_value = qQ_mean[idx] / sqrt(QQ_mean[QQ_index]);
            // Calculate the error on v2 using propagation of errors
            float v2_error = abs(v2_value) * sqrt(pow(qQ_error[idx] / qQ_mean[idx], 2) + 0.25 * pow(QQ_error[QQ_index] / QQ_mean[QQ_index], 2));

            // Print details with error
            std::cout << "v2 calculated with QQ = '" << hist_name_QQ[QQ_index]
                      << "' and qQ = '" << hist_name_qq[idx]
                      << "' and outputted v2 = '" << v2_value
                      << "' with error = '" << v2_error
                      << "' to index " << idx << " in the CSV" << std::endl;

            // Append v2 value and error to CSV
            outFile << line << "," << v2_value << "," << v2_error << std::endl;
        }
        ++lineIndex;
    }

    inFile.close();
    outFile.close();

    // Clean up
    delete file;
}
