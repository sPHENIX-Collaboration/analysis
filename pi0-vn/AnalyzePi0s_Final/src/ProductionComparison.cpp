#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TAxis.h>

struct DataEntry {
    double pT;
    double gaussSigma;
    double gaussSigmaError; // Add field for GaussSigmaError
};

std::vector<DataEntry> readCSV(const std::string& filePath) {
    std::vector<DataEntry> entries;
    std::ifstream file(filePath);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open file at path: " << filePath << std::endl;
        return entries; // Early return if file can't be opened
    }

    std::cout << "Reading file: " << filePath << std::endl;

    // Skip the header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;

        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        // Calculate pT range midpoint
        double pT = 2.0 + 0.5 * (std::stoi(row[0]) % 6) + 0.25;
        double gaussSigma = std::stod(row[5]);
        double gaussSigmaError = std::stod(row[6]);
        entries.push_back({pT, gaussSigma, gaussSigmaError});
    }

    if(entries.empty()) {
        std::cout << "No entries were added from the file. Please check the file content." << std::endl;
    }

    return entries;
}

void plotData(const std::vector<DataEntry>& data1, const std::vector<DataEntry>& data2, const std::string& title, double jitter = 0.03) {
    if (data1.empty() && data2.empty()) {
        std::cout << "No data to plot for " << title << std::endl;
        return;
    }
    std::cout << "Plotting for: " << title << std::endl;

    auto graph1 = new TGraphErrors(data1.size());
    auto graph2 = new TGraphErrors(data2.size());

    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();
    
    
    // Apply jitter to data1
    std::cout << "Dataset: E 0.5" << std::endl;
    for (size_t i = 0; i < data1.size(); ++i) {
        graph1->SetPoint(i, data1[i].pT - jitter, data1[i].gaussSigma); // Apply negative jitter to data1
        graph1->SetPointError(i, 0, data1[i].gaussSigmaError);
        minY = std::min(minY, data1[i].gaussSigma - data1[i].gaussSigmaError);
        maxY = std::max(maxY, data1[i].gaussSigma + data1[i].gaussSigmaError);
        std::cout << "pT: " << data1[i].pT << ", Value: " << data1[i].gaussSigma << ", Error: " << data1[i].gaussSigmaError << std::endl;
    }
    graph1->SetMarkerStyle(21);
    graph1->SetMarkerSize(1.3);
    graph1->SetMarkerColor(kBlue);

    // Apply jitter to data2
    std::cout << "Dataset: E 1.0" << std::endl;
    for (size_t i = 0; i < data2.size(); ++i) {
        graph2->SetPoint(i, data2[i].pT + jitter, data2[i].gaussSigma); // Apply positive jitter to data2
        graph2->SetPointError(i, 0, data2[i].gaussSigmaError);
        minY = std::min(minY, data2[i].gaussSigma - data2[i].gaussSigmaError);
        maxY = std::max(maxY, data2[i].gaussSigma + data2[i].gaussSigmaError);
        std::cout << "pT: " << data2[i].pT << ", Value: " << data2[i].gaussSigma << ", Error: " << data2[i].gaussSigmaError << std::endl;
    }
    graph2->SetMarkerStyle(22);
    graph2->SetMarkerSize(1.3);
    graph2->SetMarkerColor(kRed);

    auto canvas = new TCanvas(title.c_str(), title.c_str(), 800, 600);
    graph1->SetTitle("Gaussian Mean Comparison 0 - 20% Centrality");
    graph1->GetXaxis()->SetTitle("pT [GeV]");
    graph1->GetYaxis()->SetTitle("S/B");
    // Adjust Y-axis range with a buffer
    double rangeBuffer = 0.6 * (maxY - minY); // 10% buffer
    graph1->GetYaxis()->SetRangeUser(minY - rangeBuffer, maxY + rangeBuffer);
    graph1->Draw("AP");
    graph2->Draw("P SAME");

    // Adding legend
    auto legend = new TLegend(0.6, 0.7, 0.9, 0.9);
    legend->AddEntry(graph1, "#bf{Cuts}: E #geq 0.5, Asym < 0.5, #chi^{2} < 4", "pe");
    legend->AddEntry(graph2, "#bf{Cuts}: E #geq 0.5, Asym < 0.5, #chi^{2} < 4", "pe");
    legend->Draw();
    
//    double xMin = graph1->GetXaxis()->GetXmin();
//    double xMax = graph1->GetXaxis()->GetXmax();
//    TLine *line = new TLine(xMin, 0.135, xMax, 0.135); // Create line from xMin to xMax at y = 0.135
//    line->SetLineColor(kGreen + 4); // Set line color (optional)
//    line->SetLineStyle(2); // Set line style (optional, e.g., dashed)
//    line->Draw(); // Draw the line on the same canvas

    // Update the path as needed
    std::string savePath = "/Users/patsfan753/Desktop/" + title + ".png"; // Adjust path as necessary
    canvas->SaveAs(savePath.c_str());
    std::cout << "Saved plot to " << savePath << std::endl;
}

void ProductionComparison() {
    // Update basePath to your actual path
    const std::string basePath = "/Users/patsfan753/Desktop/vN_AnalysisFinal/data/PlotByPlotOutput_FromFits/p011/No_pdc_withZvertex";
    const std::string csvPath1 = basePath + "/PlotByPlotOutput_p011_WithZvertex_noPDC_EnergyCut_point5_Asym_point5.csv";
    const std::string csvPath2 = basePath + "/PlotByPlotOutput_p011_WithZvertex_noPDC_EnergyCut_1_Asym_point5.csv";

    auto data1 = readCSV(csvPath1);
    auto data2 = readCSV(csvPath2);

    // Titles for centrality ranges
    std::vector<std::string> centralityRanges = {"40-60%", "20-40%", "0-20%"};

    for (int centrality = 0; centrality < centralityRanges.size(); ++centrality) {
        std::vector<DataEntry> dataCentrality1;
        std::vector<DataEntry> dataCentrality2;
        int startIndex = centrality * 6;
        int endIndex = startIndex + 6;

        // Extract data for the current centrality
        for (int i = startIndex; i < endIndex; ++i) {
            if (i < data1.size()) dataCentrality1.push_back(data1[i]);
            if (i < data2.size()) dataCentrality2.push_back(data2[i]);
        }

        std::string title = "Centrality " + centralityRanges[centrality] + " Gaussian Sigma Comparison";
        plotData(dataCentrality1, dataCentrality2, title);
    }
}
