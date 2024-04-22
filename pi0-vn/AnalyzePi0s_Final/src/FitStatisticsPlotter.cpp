#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

#include <TFile.h>
#include <TH1F.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

struct Data {
    std::vector<double> sb_0_20, sb_20_40, sb_40_60;
    std::vector<double> sb_0_20_Errors, sb_20_40_Errors, sb_40_60_Errors;
    
    std::vector<double> gaussMean_0_20, gaussMean_20_40, gaussMean_40_60;
    std::vector<double> gaussMean_0_20_Errors, gaussMean_20_40_Errors, gaussMean_40_60_Errors;
    
    std::vector<double> gaussSigma_0_20, gaussSigma_20_40, gaussSigma_40_60;
    std::vector<double> gaussSigma_0_20_Errors, gaussSigma_20_40_Errors, gaussSigma_40_60_Errors;
    
    std::vector<double> gaussMeanDividedBySigma_0_20, gaussMeanDividedBySigma_20_40, gaussMeanDividedBySigma_40_60;
    std::vector<double> gaussMeanDividedBySigma_0_20_Errors, gaussMeanDividedBySigma_20_40_Errors, gaussMeanDividedBySigma_40_60_Errors;
    
};

double CalculateError(double mean, double sigma, double meanError, double sigmaError) {
    // Assuming mean and sigma are uncorrelated (no covariance term)
    double value = mean / sigma;
    double fractionalErrorMean = meanError / mean;
    double fractionalErrorSigma = sigmaError / sigma;
    double error = std::abs(value) * std::sqrt(fractionalErrorMean * fractionalErrorMean + fractionalErrorSigma * fractionalErrorSigma);
    return error;
}

void Read_DataSet(const std::string& filePath, Data& data) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }
    std::string line;
    std::getline(file, line); // Skip the header line

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::vector<std::string> rowData;
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            rowData.push_back(cell);
        }
        
        double sb = std::stod(rowData.at(17));
        double sbError = std::stod(rowData.at(18));
        
        double gaussMean = std::stod(rowData.at(7));
        double gaussMeanError = std::stod(rowData.at(8));

        double gaussSigma = std::stod(rowData.at(9));
        double gaussSigmaError = std::stod(rowData.at(10));
        
        double value = gaussSigma / gaussMean;
        double error = CalculateError(gaussMean, gaussSigma, gaussMeanError, gaussSigmaError);
        
        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            data.sb_40_60.push_back(sb);
            data.sb_40_60_Errors.push_back(sbError);
            
            data.gaussMean_40_60.push_back(gaussMean);
            data.gaussMean_40_60_Errors.push_back(gaussMeanError);
            
            data.gaussSigma_40_60.push_back(gaussSigma);
            data.gaussSigma_40_60_Errors.push_back(gaussSigmaError);
            
            data.gaussMeanDividedBySigma_40_60.push_back(value);
            data.gaussMeanDividedBySigma_40_60_Errors.push_back(error);
            
            
        } else if (index >= 6 && index <= 11) {
            data.sb_20_40.push_back(sb);
            data.sb_20_40_Errors.push_back(sbError);
            
            data.gaussMean_20_40.push_back(gaussMean);
            data.gaussMean_20_40_Errors.push_back(gaussMeanError);
            
            data.gaussSigma_20_40.push_back(gaussSigma);
            data.gaussSigma_20_40_Errors.push_back(gaussSigmaError);
            
            data.gaussMeanDividedBySigma_20_40.push_back(value);
            data.gaussMeanDividedBySigma_20_40_Errors.push_back(error);
            
        } else if (index >= 12 && index <= 17) {
            data.sb_0_20.push_back(sb);
            data.sb_0_20_Errors.push_back(sbError);
            
            data.gaussMean_0_20.push_back(gaussMean);
            data.gaussMean_0_20_Errors.push_back(gaussMeanError);
            
            data.gaussSigma_0_20.push_back(gaussSigma);
            data.gaussSigma_0_20_Errors.push_back(gaussSigmaError);
            
            data.gaussMeanDividedBySigma_0_20.push_back(value);
            data.gaussMeanDividedBySigma_0_20_Errors.push_back(error);
            
        }
    }
    file.close();
}
void PrintVectorContents(const std::vector<double>& vec, const std::vector<double>& vecErrors, const std::string& name) {
    std::cout << "\033[1m\033[31m" // Red and bold text
              << "Contents of " << name << " and its Errors:" << "\033[0m" << std::endl; // Reset formatting at the end
    std::cout << std::left << std::setw(20) << "Value"
              << std::left << std::setw(20) << "Error" << std::endl;
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << std::left << std::setw(20) << vec[i]
                  << std::left << std::setw(20) << vecErrors[i] << std::endl;
    }
    std::cout << std::endl;
}
// Function to create and return a TGraphErrors pointer
TGraphErrors* CreateGraph(const std::vector<double>& ptCenters, const std::vector<double>& values, const std::vector<double>& errors) {
    return new TGraphErrors(ptCenters.size(), &ptCenters[0], &values[0], nullptr, &errors[0]);
}
void plotting(const Data& data1, Data& data2) {
    std::vector<double> ptCenters = {2.25, 2.75, 3.25, 3.75, 4.25, 4.75}; // Mid-points of pT ranges
    std::string BasePlotOutputPath = "/Users/patsfan753/Desktop";
    
    const auto& sb_0_20 = data1.sb_0_20;
    const auto& sb_20_40 = data1.sb_20_40;
    const auto& sb_40_60 = data1.sb_40_60;
    const auto& sb_0_20_Errors = data1.sb_0_20_Errors;
    const auto& sb_20_40_Errors = data1.sb_20_40_Errors;
    const auto& sb_40_60_Errors = data1.sb_40_60_Errors;
    
    
    const auto& sb_0_20_p014 = data2.sb_0_20;
    const auto& sb_20_40_p014 = data2.sb_20_40;
    const auto& sb_40_60_p014 = data2.sb_40_60;
    const auto& sb_0_20_Errors_p014 = data2.sb_0_20_Errors;
    const auto& sb_20_40_Errors_p014 = data2.sb_20_40_Errors;
    const auto& sb_40_60_Errors_p014 = data2.sb_40_60_Errors;
    
    
    const auto& gaussMean_0_20 = data1.gaussMean_0_20;
    const auto& gaussMean_20_40 = data1.gaussMean_20_40;
    const auto& gaussMean_40_60 = data1.gaussMean_40_60;
    const auto& gaussMean_0_20_Errors = data1.gaussMean_0_20_Errors;
    const auto& gaussMean_20_40_Errors = data1.gaussMean_20_40_Errors;
    const auto& gaussMean_40_60_Errors = data1.gaussMean_40_60_Errors;
    
    
    const auto& gaussSigma_0_20 = data1.gaussSigma_0_20;
    const auto& gaussSigma_20_40 = data1.gaussSigma_20_40;
    const auto& gaussSigma_40_60 = data1.gaussSigma_40_60;
    const auto& gaussSigma_0_20_Errors = data1.gaussSigma_0_20_Errors;
    const auto& gaussSigma_20_40_Errors = data1.gaussSigma_20_40_Errors;
    const auto& gaussSigma_40_60_Errors = data1.gaussSigma_40_60_Errors;
    
    const auto& gaussMeanDividedBySigma_0_20 = data1.gaussMeanDividedBySigma_0_20;
    const auto& gaussMeanDividedBySigma_20_40 = data1.gaussMeanDividedBySigma_20_40;
    const auto& gaussMeanDividedBySigma_40_60 = data1.gaussMeanDividedBySigma_40_60;
    const auto& gaussMeanDividedBySigma_0_20_Errors = data1.gaussMeanDividedBySigma_0_20_Errors;
    const auto& gaussMeanDividedBySigma_20_40_Errors = data1.gaussMeanDividedBySigma_20_40_Errors;
    const auto& gaussMeanDividedBySigma_40_60_Errors = data1.gaussMeanDividedBySigma_40_60_Errors;
    
    
    
    PrintVectorContents(data1.sb_0_20, data1.sb_0_20_Errors, "SB 0-20% Data1");
    PrintVectorContents(data1.sb_20_40, data1.sb_20_40_Errors, "SB 20-40% Data1");
    PrintVectorContents(data1.sb_40_60, data1.sb_40_60_Errors, "SB 40-60% Data1");

    PrintVectorContents(data1.gaussMean_0_20, data1.gaussMean_0_20_Errors, "Gauss Mean 0-20% Data1");
    PrintVectorContents(data1.gaussMean_20_40, data1.gaussMean_20_40_Errors, "Gauss Mean 20-40% Data1");
    PrintVectorContents(data1.gaussMean_40_60, data1.gaussMean_40_60_Errors, "Gauss mEAN 40-60% Data1");
    
    PrintVectorContents(data1.gaussSigma_0_20, data1.gaussSigma_0_20_Errors, "gaussSigma 0-20% Data1");
    PrintVectorContents(data1.gaussSigma_20_40, data1.gaussSigma_20_40_Errors, "gaussSigma 20-40% Data1");
    PrintVectorContents(data1.gaussSigma_40_60, data1.gaussSigma_40_60_Errors, "gaussSigma 40-60% Data1");
    
    PrintVectorContents(data1.gaussMeanDividedBySigma_0_20, data1.gaussMeanDividedBySigma_0_20_Errors, "gaussMeanDividedBySigma 0-20% Data1");
    PrintVectorContents(data1.gaussMeanDividedBySigma_20_40, data1.gaussMeanDividedBySigma_20_40_Errors, "gaussMeanDividedBySigma 20-40% Data1");
    PrintVectorContents(data1.gaussMeanDividedBySigma_40_60, data1.gaussMeanDividedBySigma_40_60_Errors, "gaussMeanDividedBySigma 40-60% Data1");
    
    
    
    TGraphErrors* sb_0_20_graph = CreateGraph(ptCenters, sb_0_20, sb_0_20_Errors);
    TGraphErrors* sb_20_40_graph = CreateGraph(ptCenters, sb_20_40, sb_20_40_Errors);
    TGraphErrors* sb_40_60_graph = CreateGraph(ptCenters, sb_40_60, sb_40_60_Errors);
    
    TGraphErrors* sb_0_20_graph_p014 = CreateGraph(ptCenters, sb_0_20_p014, sb_0_20_Errors_p014);
    TGraphErrors* sb_20_40_graph_p014 = CreateGraph(ptCenters, sb_20_40_p014, sb_20_40_Errors_p014);
    TGraphErrors* sb_40_60_graph_p014 = CreateGraph(ptCenters, sb_40_60_p014, sb_40_60_Errors_p014);
    
    
    TGraphErrors* gaussMean_0_20_graph = CreateGraph(ptCenters, gaussMean_0_20, gaussMean_0_20_Errors);
    TGraphErrors* gaussMean_20_40_graph = CreateGraph(ptCenters, gaussMean_20_40, gaussMean_20_40_Errors);
    TGraphErrors* gaussMean_40_60_graph = CreateGraph(ptCenters, gaussMean_40_60, gaussMean_40_60_Errors);
    
    TGraphErrors* gaussSigma_0_20_graph = CreateGraph(ptCenters, gaussSigma_0_20, gaussSigma_0_20_Errors);
    TGraphErrors* gaussSigma_20_40_graph = CreateGraph(ptCenters, gaussSigma_20_40, gaussSigma_20_40_Errors);
    TGraphErrors* gaussSigma_40_60_graph = CreateGraph(ptCenters, gaussSigma_40_60, gaussSigma_40_60_Errors);
    
    TGraphErrors* gaussMeanDividedBySigma_0_20_graph = CreateGraph(ptCenters, gaussMeanDividedBySigma_0_20, gaussMeanDividedBySigma_0_20_Errors);
    TGraphErrors* gaussMeanDividedBySigma_20_40_graph = CreateGraph(ptCenters, gaussMeanDividedBySigma_20_40, gaussMeanDividedBySigma_20_40_Errors);
    TGraphErrors* gaussMeanDividedBySigma_40_60_graph = CreateGraph(ptCenters, gaussMeanDividedBySigma_40_60, gaussMeanDividedBySigma_40_60_Errors);
    
    
    
    
    double minYaxis_correctedComparisons = 0;
    double maxYaxis_correctedComparisons = 5;
    double TLatexSize = 0.025;
    // Define the offsets for jitter in Overlays
    double offset = 0.05;
    
    int markerColor = kBlack;
    int markerColor2 = kBlue;
    int markerStyle = 20;
    double markerSize = 1.0;
    
    
    sb_0_20_graph->SetMarkerColor(markerColor);
    sb_0_20_graph->SetLineColor(markerColor);
    sb_0_20_graph->SetMarkerSize(markerSize);
    sb_0_20_graph->SetMarkerStyle(markerStyle);
    
    sb_20_40_graph->SetMarkerColor(markerColor);
    sb_20_40_graph->SetLineColor(markerColor);
    sb_20_40_graph->SetMarkerSize(markerSize);
    sb_20_40_graph->SetMarkerStyle(markerStyle);
    
    sb_40_60_graph->SetMarkerColor(markerColor);
    sb_40_60_graph->SetLineColor(markerColor);
    sb_40_60_graph->SetMarkerSize(markerSize);
    sb_40_60_graph->SetMarkerStyle(markerStyle);
    
    
    sb_0_20_graph_p014->SetMarkerColor(markerColor2);
    sb_0_20_graph_p014->SetLineColor(markerColor2);
    sb_0_20_graph_p014->SetMarkerSize(markerSize);
    sb_0_20_graph_p014->SetMarkerStyle(markerStyle);
    
    sb_20_40_graph_p014->SetMarkerColor(markerColor2);
    sb_20_40_graph_p014->SetLineColor(markerColor2);
    sb_20_40_graph_p014->SetMarkerSize(markerSize);
    sb_20_40_graph_p014->SetMarkerStyle(markerStyle);
    
    sb_40_60_graph_p014->SetMarkerColor(markerColor2);
    sb_40_60_graph_p014->SetLineColor(markerColor2);
    sb_40_60_graph_p014->SetMarkerSize(markerSize);
    sb_40_60_graph_p014->SetMarkerStyle(markerStyle);
    
    
    TCanvas *c_SBratio_0_20 = new TCanvas("c_SBratio_0_20", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        sb_0_20_graph->SetPoint(i, ptCenters[i] - .05, sb_0_20[i]);
        sb_0_20_graph_p014->SetPoint(i, ptCenters[i] + .05, sb_0_20_p014[i]);
    }
    sb_0_20_graph->Draw("AP");  // This will be the base graph
    sb_0_20_graph_p014->Draw("P SAME");  // This will be the base graph
    sb_0_20_graph->GetXaxis()->SetLimits(2.0, 5.0);
    
    sb_0_20_graph->SetTitle("S/B vs #it{p}_{T} 0-20% Centrality");
    sb_0_20_graph->GetXaxis()->SetTitle("p_{T} [GeV]");
    sb_0_20_graph->GetYaxis()->SetTitle("S/B");
    sb_0_20_graph->SetMinimum(0); // Set the minimum y value
    sb_0_20_graph->SetMaximum(1); // Set the maximum y value
    
    TLegend *legend_Overlay_0_20_p013_p014 = new TLegend(0.2, 0.4, 0.4, 0.6);
    legend_Overlay_0_20_p013_p014->SetBorderSize(0);
    legend_Overlay_0_20_p013_p014->SetTextSize(0.032);
    legend_Overlay_0_20_p013_p014->SetTextSize(TLatexSize);
    legend_Overlay_0_20_p013_p014->AddEntry(sb_0_20_graph, "p013 S/B", "pe");
    legend_Overlay_0_20_p013_p014->AddEntry(sb_0_20_graph_p014, "p014 S/B", "pe");
    legend_Overlay_0_20_p013_p014->Draw();
    
    TLegend *leg_SB_0_20 = new TLegend(0.14,.72,0.34,.92);
    leg_SB_0_20->SetFillStyle(0);
    leg_SB_0_20->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg_SB_0_20->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg_SB_0_20->AddEntry("","0-20% Centrality","");
    leg_SB_0_20->Draw("same");
    c_SBratio_0_20->SaveAs((BasePlotOutputPath + "/Overlay_0_20_SB.png").c_str());
    
    
    TCanvas *c_SBratio_20_40 = new TCanvas("c_SBratio_20_40", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        sb_20_40_graph->SetPoint(i, ptCenters[i] - .05, sb_20_40[i]);
        sb_20_40_graph_p014->SetPoint(i, ptCenters[i] + .05, sb_20_40_p014[i]);
    }
    sb_20_40_graph->Draw("AP");  // This will be the base graph
    sb_20_40_graph_p014->Draw("P SAME");  // This will be the base graph
    sb_20_40_graph->GetXaxis()->SetLimits(2.0, 5.0);
    sb_20_40_graph->SetTitle("S/B vs #it{p}_{T} 0-20% Centrality");
    sb_20_40_graph->GetXaxis()->SetTitle("p_{T} [GeV]");
    sb_20_40_graph->GetYaxis()->SetTitle("S/B");
    sb_20_40_graph->SetMinimum(0); // Set the minimum y value
    sb_20_40_graph->SetMaximum(2.2); // Set the maximum y value
    
    TLegend *legend_Overlay_20_40_p013_p014 = new TLegend(0.2, 0.4, 0.4, 0.6);
    legend_Overlay_20_40_p013_p014->SetBorderSize(0);
    legend_Overlay_20_40_p013_p014->SetTextSize(0.032);
    legend_Overlay_20_40_p013_p014->SetTextSize(TLatexSize);
    legend_Overlay_20_40_p013_p014->AddEntry(sb_20_40_graph, "p013 S/B", "pe");
    legend_Overlay_20_40_p013_p014->AddEntry(sb_20_40_graph_p014, "p014 S/B", "pe");
    legend_Overlay_20_40_p013_p014->Draw();
    
    TLegend *leg_SB_20_40 = new TLegend(0.14,.72,0.34,.92);
    leg_SB_20_40->SetFillStyle(0);
    leg_SB_20_40->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg_SB_20_40->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg_SB_20_40->AddEntry("","0-20% Centrality","");
    leg_SB_20_40->Draw("same");
    c_SBratio_20_40->SaveAs((BasePlotOutputPath + "/Overlay_20_40_SB.png").c_str());
    
    TCanvas *c_SBratio_40_60 = new TCanvas("c_SBratio_40_60", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        sb_40_60_graph->SetPoint(i, ptCenters[i] - .05, sb_40_60[i]);
        sb_40_60_graph_p014->SetPoint(i, ptCenters[i] + .05, sb_40_60_p014[i]);
    }
    sb_40_60_graph->Draw("AP");  // This will be the base graph
    sb_40_60_graph_p014->Draw("P SAME");  // This will be the base graph
    sb_40_60_graph->GetXaxis()->SetLimits(2.0, 5.0);
    sb_40_60_graph->SetTitle("S/B vs #it{p}_{T} 0-20% Centrality");
    sb_40_60_graph->GetXaxis()->SetTitle("p_{T} [GeV]");
    sb_40_60_graph->GetYaxis()->SetTitle("S/B");
    sb_40_60_graph->SetMinimum(0); // Set the minimum y value
    sb_40_60_graph->SetMaximum(6.5); // Set the maximum y value
    
    TLegend *legend_Overlay_40_60_p013_p014 = new TLegend(0.2, 0.4, 0.4, 0.6);
    legend_Overlay_40_60_p013_p014->SetBorderSize(0);
    legend_Overlay_40_60_p013_p014->SetTextSize(0.032);
    legend_Overlay_40_60_p013_p014->SetTextSize(TLatexSize);
    legend_Overlay_40_60_p013_p014->AddEntry(sb_40_60_graph, "p013 S/B", "pe");
    legend_Overlay_40_60_p013_p014->AddEntry(sb_40_60_graph_p014, "p014 S/B", "pe");
    legend_Overlay_40_60_p013_p014->Draw();
    
    TLegend *leg_SB_40_60 = new TLegend(0.14,.72,0.34,.92);
    leg_SB_40_60->SetFillStyle(0);
    leg_SB_40_60->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg_SB_40_60->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg_SB_40_60->AddEntry("","0-20% Centrality","");
    leg_SB_40_60->Draw("same");
    c_SBratio_40_60->SaveAs((BasePlotOutputPath + "/Overlay_40_60_SB.png").c_str());
    
    
    
    TCanvas *c_gaussMean_0_20 = new TCanvas("c_gaussMean_0_20", "#pi^{0} Gaussian Mean vs #it{p}_{T} 0-20% Centrality", 800, 600);
    gaussMean_0_20_graph->Draw("AP");  // This will be the base graph
    gaussMean_0_20_graph->SetTitle("Gaussian Mean vs #it{p}_{T} 0-20% Centrality");
    gaussMean_0_20_graph->GetXaxis()->SetTitle("p_{T} [GeV]");
    gaussMean_0_20_graph->GetYaxis()->SetTitle("#mu_{Gauss} [GeV]");
    gaussMean_0_20_graph->SetMinimum(0.14); // Set the minimum y value
    gaussMean_0_20_graph->SetMaximum(0.21); // Set the maximum y value
    TLegend *leg_gaussMean_0_20 = new TLegend(0.16,.2,0.34,.45);
    leg_gaussMean_0_20->SetFillStyle(0);
    leg_gaussMean_0_20->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg_gaussMean_0_20->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg_gaussMean_0_20->AddEntry("","0-20% Centrality","");
    leg_gaussMean_0_20->Draw("same");
//    TLine *line_0_20 = new TLine(gaussMean_0_20_graph->GetXaxis()->GetXmin(), 0.135, gaussMean_0_20_graph->GetXaxis()->GetXmax(), 0.135);
//    line_0_20->SetLineStyle(2); // Set the line style to dashed
//    line_0_20->SetLineWidth(2); // Increase line width for better visibility
//    line_0_20->Draw("same");
//    TLine *dummyLine = new TLine();
//    dummyLine->SetLineStyle(2);
//    dummyLine->SetLineWidth(3); // Set a thicker line width for visibility in the legend
//
//    TLegend *leg_Line_0_20 = new TLegend(0.6, .8, 0.75, .9);
//    leg_Line_0_20->SetFillStyle(0);
//    leg_Line_0_20->SetTextSize(0.033);
//    leg_Line_0_20->SetTextAlign(12);
//    leg_Line_0_20->AddEntry(dummyLine, "Theoretical #pi^{0} Mass (PDG)", "l"); // Use dummy line for legend
//    leg_Line_0_20->Draw("same");
    c_gaussMean_0_20->SaveAs((BasePlotOutputPath + "/Overlay_0_20_gaussMean.png").c_str());
    
    TCanvas *c_gaussMean_20_40 = new TCanvas("c_gaussMean_20_40", "#pi^{0} Gaussian Mean vs #it{p}_{T} 0-20% Centrality", 800, 600);
    gaussMean_20_40_graph->Draw("AP");  // This will be the base graph
    gaussMean_20_40_graph->SetTitle("Gaussian Mean vs #it{p}_{T} 0-20% Centrality");
    gaussMean_20_40_graph->GetXaxis()->SetTitle("p_{T} [GeV]");
    gaussMean_20_40_graph->GetYaxis()->SetTitle("#mu_{Gauss} [GeV]");
    gaussMean_20_40_graph->SetMinimum(0.14); // Set the minimum y value
    gaussMean_20_40_graph->SetMaximum(0.21); // Set the maximum y value
    TLegend *leg_gaussMean_20_40 = new TLegend(0.16,.7,0.34,.9);
    leg_gaussMean_20_40->SetFillStyle(0);
    leg_gaussMean_20_40->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg_gaussMean_20_40->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg_gaussMean_20_40->AddEntry("","20-40% Centrality","");
    leg_gaussMean_20_40->Draw("same");
//    TLine *line_20_40 = new TLine(gaussMean_20_40_graph->GetXaxis()->GetXmin(), 0.135, gaussMean_20_40_graph->GetXaxis()->GetXmax(), 0.135);
//    line_20_40->SetLineStyle(2); // Set the line style to dashed
//    line_20_40->SetLineWidth(2); // Increase line width for better visibility
//    line_20_40->Draw("same");
//    TLegend *leg_Line_20_40 = new TLegend(0.6, .8, 0.75, .9);
//    leg_Line_20_40->SetFillStyle(0);
//    leg_Line_20_40->SetTextSize(0.033);
//    leg_Line_20_40->SetTextAlign(12);
//    leg_Line_20_40->AddEntry(dummyLine, "Theoretical #pi^{0} Mass (PDG)", "l"); // Use dummy line for legend
//    leg_Line_20_40->Draw("same");
    c_gaussMean_20_40->SaveAs((BasePlotOutputPath + "/Overlay_20_40_gaussMean.png").c_str());
    
    
    TCanvas *c_gaussMean_40_60 = new TCanvas("c_gaussMean_40_60", "#pi^{0} Gaussian Mean vs #it{p}_{T} 0-20% Centrality", 800, 600);
    gaussMean_40_60_graph->Draw("AP");  // This will be the base graph
    gaussMean_40_60_graph->SetTitle("Gaussian Mean vs #it{p}_{T} 0-20% Centrality");
    gaussMean_40_60_graph->GetXaxis()->SetTitle("p_{T} [GeV]");
    gaussMean_40_60_graph->GetYaxis()->SetTitle("#mu_{Gauss} [GeV]");
    gaussMean_40_60_graph->SetMinimum(0.14); // Set the minimum y value
    gaussMean_40_60_graph->SetMaximum(0.21); // Set the maximum y value
    TLegend *leg_gaussMean_40_60 = new TLegend(0.16,.7,0.34,.9);
    leg_gaussMean_40_60->SetFillStyle(0);
    leg_gaussMean_40_60->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg_gaussMean_40_60->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg_gaussMean_40_60->AddEntry("","40-60% Centrality","");
    leg_gaussMean_40_60->Draw("same");
//    TLine *line_40_60 = new TLine(gaussMean_40_60_graph->GetXaxis()->GetXmin(), 0.135, gaussMean_40_60_graph->GetXaxis()->GetXmax(), 0.135);
//    line_40_60->SetLineStyle(2); // Set the line style to dashed
//    line_40_60->SetLineWidth(2); // Increase line width for better visibility
//    line_40_60->Draw("same");
//    TLegend *leg_Line_40_60 = new TLegend(0.6, .8, 0.75, .9);
//    leg_Line_40_60->SetFillStyle(0);
//    leg_Line_40_60->SetTextSize(0.033);
//    leg_Line_40_60->SetTextAlign(12);
//    leg_Line_40_60->AddEntry(dummyLine, "Theoretical #pi^{0} Mass (PDG)", "l"); // Use dummy line for legend
//    leg_Line_40_60->Draw("same");
    c_gaussMean_40_60->SaveAs((BasePlotOutputPath + "/Overlay_40_60_gaussMean.png").c_str());
    
    
    
    
    gaussSigma_0_20_graph->SetMarkerColor(markerColor);
    gaussSigma_0_20_graph->SetLineColor(markerColor);
    gaussSigma_0_20_graph->SetMarkerSize(markerSize);
    gaussSigma_0_20_graph->SetMarkerStyle(markerStyle);
    
    gaussSigma_20_40_graph->SetMarkerColor(markerColor);
    gaussSigma_20_40_graph->SetLineColor(markerColor);
    gaussSigma_20_40_graph->SetMarkerSize(markerSize);
    gaussSigma_20_40_graph->SetMarkerStyle(markerStyle);
    
    gaussSigma_40_60_graph->SetMarkerColor(markerColor);
    gaussSigma_40_60_graph->SetLineColor(markerColor);
    gaussSigma_40_60_graph->SetMarkerSize(markerSize);
    gaussSigma_40_60_graph->SetMarkerStyle(markerStyle);
    
    
    TCanvas *c_gaussSigma_0_20 = new TCanvas("c_gaussSigma_0_20", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    gaussSigma_0_20_graph->Draw("AP");  // This will be the base graph
    gaussSigma_0_20_graph->SetTitle("S/B vs #it{p}_{T} 0-20% Centrality");
    gaussSigma_0_20_graph->GetXaxis()->SetTitle("p_{T} [GeV]");
    gaussSigma_0_20_graph->GetYaxis()->SetTitle("#sigma_{Gauss} [GeV]");
    gaussSigma_0_20_graph->SetMinimum(0.01); // Set the minimum y value
    gaussSigma_0_20_graph->SetMaximum(0.04); // Set the maximum y value
    TLegend *leg_gaussSigma_0_20 = new TLegend(0.14,.72,0.34,.92);
    leg_gaussSigma_0_20->SetFillStyle(0);
    leg_gaussSigma_0_20->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg_gaussSigma_0_20->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg_gaussSigma_0_20->AddEntry("","0-20% Centrality","");
    leg_gaussSigma_0_20->Draw("same");
    c_gaussSigma_0_20->SaveAs((BasePlotOutputPath + "/Overlay_0_20_gaussSigma.png").c_str());
    
    
    TCanvas *c_gaussSigma_20_40 = new TCanvas("c_gaussSigma_20_40", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    gaussSigma_20_40_graph->Draw("AP");  // This will be the base graph
    gaussSigma_20_40_graph->SetTitle("S/B vs #it{p}_{T} 0-20% Centrality");
    gaussSigma_20_40_graph->GetXaxis()->SetTitle("p_{T} [GeV]");
    gaussSigma_20_40_graph->GetYaxis()->SetTitle("#sigma_{Gauss} [GeV]");
    gaussSigma_20_40_graph->SetMinimum(0.01); // Set the minimum y value
    gaussSigma_20_40_graph->SetMaximum(0.04); // Set the maximum y value
    TLegend *leg_gaussSigma_20_40 = new TLegend(0.14,.72,0.34,.92);
    leg_gaussSigma_20_40->SetFillStyle(0);
    leg_gaussSigma_20_40->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg_gaussSigma_20_40->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg_gaussSigma_20_40->AddEntry("","20-40% Centrality","");
    leg_gaussSigma_20_40->Draw("same");
    c_gaussSigma_20_40->SaveAs((BasePlotOutputPath + "/Overlay_20_40_gaussSigma.png").c_str());
    
    
    TCanvas *c_gaussSigma_40_60 = new TCanvas("c_gaussSigma_40_60", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    gaussSigma_40_60_graph->Draw("AP");  // This will be the base graph
    gaussSigma_40_60_graph->SetTitle("S/B vs #it{p}_{T} 0-20% Centrality");
    gaussSigma_40_60_graph->GetXaxis()->SetTitle("p_{T} [GeV]");
    gaussSigma_40_60_graph->GetYaxis()->SetTitle("#sigma_{Gauss} [GeV]");
    gaussSigma_40_60_graph->SetMinimum(0.01); // Set the minimum y value
    gaussSigma_40_60_graph->SetMaximum(0.04); // Set the maximum y value
    TLegend *leg_gaussSigma_40_60 = new TLegend(0.14,.72,0.34,.92);
    leg_gaussSigma_40_60->SetFillStyle(0);
    leg_gaussSigma_40_60->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg_gaussSigma_40_60->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg_gaussSigma_40_60->AddEntry("","40-60% Centrality","");
    leg_gaussSigma_40_60->Draw("same");
    c_gaussSigma_40_60->SaveAs((BasePlotOutputPath + "/Overlay_40_60_gaussSigma.png").c_str());
    
    
    
    gaussMeanDividedBySigma_0_20_graph->SetMarkerColor(markerColor);
    gaussMeanDividedBySigma_0_20_graph->SetLineColor(markerColor);
    gaussMeanDividedBySigma_0_20_graph->SetMarkerSize(markerSize);
    gaussMeanDividedBySigma_0_20_graph->SetMarkerStyle(markerStyle);
    // Loop over all points in the graph and set the errors to zero
    for (int i = 0; i < gaussMeanDividedBySigma_0_20_graph->GetN(); ++i) {
        gaussMeanDividedBySigma_0_20_graph->SetPointError(i, 0, 0); // Set both x and y errors to 0
    }
    
    gaussMeanDividedBySigma_20_40_graph->SetMarkerColor(markerColor);
    gaussMeanDividedBySigma_20_40_graph->SetLineColor(markerColor);
    gaussMeanDividedBySigma_20_40_graph->SetMarkerSize(markerSize);
    gaussMeanDividedBySigma_20_40_graph->SetMarkerStyle(markerStyle);
    // Repeat the process for the other centrality graphs
    for (int i = 0; i < gaussMeanDividedBySigma_20_40_graph->GetN(); ++i) {
        gaussMeanDividedBySigma_20_40_graph->SetPointError(i, 0, 0); // Set errors to 0
    }
    
    gaussMeanDividedBySigma_40_60_graph->SetMarkerColor(markerColor);
    gaussMeanDividedBySigma_40_60_graph->SetLineColor(markerColor);
    gaussMeanDividedBySigma_40_60_graph->SetMarkerSize(markerSize);
    gaussMeanDividedBySigma_40_60_graph->SetMarkerStyle(markerStyle);
    for (int i = 0; i < gaussMeanDividedBySigma_40_60_graph->GetN(); ++i) {
        gaussMeanDividedBySigma_40_60_graph->SetPointError(i, 0, 0); // Set errors to 0
    }

    
    
    TCanvas *c_gaussMeanDividedBySigma_0_20 = new TCanvas("c_gaussMeanDividedBySigma_0_20", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    gaussMeanDividedBySigma_0_20_graph->Draw("AP");  // This will be the base graph
    gaussMeanDividedBySigma_0_20_graph->SetTitle("S/B vs #it{p}_{T} 0-20% Centrality");
    gaussMeanDividedBySigma_0_20_graph->GetXaxis()->SetTitle("p_{T} [GeV]");
    gaussMeanDividedBySigma_0_20_graph->GetYaxis()->SetTitle("Resolution");
    gaussMeanDividedBySigma_0_20_graph->SetMinimum(0); // Set the minimum y value
    gaussMeanDividedBySigma_0_20_graph->SetMaximum(0.3); // Set the maximum y value
    TLegend *leg_gaussMeanDividedBySigma_0_20 = new TLegend(0.14,.72,0.34,.92);
    leg_gaussMeanDividedBySigma_0_20->SetFillStyle(0);
    leg_gaussMeanDividedBySigma_0_20->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg_gaussMeanDividedBySigma_0_20->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg_gaussMeanDividedBySigma_0_20->AddEntry("","0-20% Centrality","");
    leg_gaussMeanDividedBySigma_0_20->Draw("same");
    c_gaussMeanDividedBySigma_0_20->SaveAs((BasePlotOutputPath + "/Overlay_0_20_gaussMeanDividedBySigma.png").c_str());
    
    
    TCanvas *c_gaussMeanDividedBySigma_20_40 = new TCanvas("c_gaussMeanDividedBySigma_20_40", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    gaussMeanDividedBySigma_20_40_graph->Draw("AP");  // This will be the base graph
    gaussMeanDividedBySigma_20_40_graph->SetTitle("S/B vs #it{p}_{T} 0-20% Centrality");
    gaussMeanDividedBySigma_20_40_graph->GetXaxis()->SetTitle("p_{T} [GeV]");
    gaussMeanDividedBySigma_20_40_graph->GetYaxis()->SetTitle("Resolution");
    gaussMeanDividedBySigma_20_40_graph->SetMinimum(0); // Set the minimum y value
    gaussMeanDividedBySigma_20_40_graph->SetMaximum(0.3); // Set the maximum y value
    TLegend *leg_gaussMeanDividedBySigma_20_40 = new TLegend(0.14,.72,0.34,.92);
    leg_gaussMeanDividedBySigma_20_40->SetFillStyle(0);
    leg_gaussMeanDividedBySigma_20_40->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg_gaussMeanDividedBySigma_20_40->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg_gaussMeanDividedBySigma_20_40->AddEntry("","20-40% Centrality","");
    leg_gaussMeanDividedBySigma_20_40->Draw("same");
    c_gaussMeanDividedBySigma_20_40->SaveAs((BasePlotOutputPath + "/Overlay_20_40_gaussMeanDividedBySigma.png").c_str());
    
    
    TCanvas *c_gaussMeanDividedBySigma_40_60 = new TCanvas("c_gaussMeanDividedBySigma_40_60", "#pi^{0} S/B vs #it{p}_{T} 0-20% Centrality", 800, 600);
    gaussMeanDividedBySigma_40_60_graph->Draw("AP");  // This will be the base graph
    gaussMeanDividedBySigma_40_60_graph->SetTitle("S/B vs #it{p}_{T} 0-20% Centrality");
    gaussMeanDividedBySigma_40_60_graph->GetXaxis()->SetTitle("p_{T} [GeV]");
    gaussMeanDividedBySigma_40_60_graph->GetYaxis()->SetTitle("Resolution");
    gaussMeanDividedBySigma_40_60_graph->SetMinimum(0); // Set the minimum y value
    gaussMeanDividedBySigma_40_60_graph->SetMaximum(0.3); // Set the maximum y value
    TLegend *leg_gaussMeanDividedBySigma_40_60 = new TLegend(0.14,.72,0.34,.92);
    leg_gaussMeanDividedBySigma_40_60->SetFillStyle(0);
    leg_gaussMeanDividedBySigma_40_60->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg_gaussMeanDividedBySigma_40_60->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg_gaussMeanDividedBySigma_40_60->AddEntry("","40-60% Centrality","");
    leg_gaussMeanDividedBySigma_40_60->Draw("same");
    c_gaussMeanDividedBySigma_40_60->SaveAs((BasePlotOutputPath + "/Overlay_40_60_gaussMeanDividedBySigma.png").c_str());

    
}
void FitStatisticsPlotter() {
    gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();
    std::string filePath_p013 = "/Users/patsfan753/Desktop/PlotByPlotOutput_p013.csv";
    std::string filePath_p014 = "/Users/patsfan753/Desktop/p015/InvMassOutput/CSV/PlotByPlot_p015_Index7Varied.csv";
    
    Data data1, data2;
    Read_DataSet(filePath_p013, data1);
    Read_DataSet(filePath_p014, data2);

    plotting(data1, data2);
}
