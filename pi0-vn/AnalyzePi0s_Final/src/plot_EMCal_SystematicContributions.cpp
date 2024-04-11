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
    std::vector<double> RelativeUncertainty_SYST1CEMC_40_60;
    std::vector<double> AbsoluteUncertainty_SYST1CEMC_40_60;
    std::vector<double> RelativeUncertainty_SYST1CEMC_20_40;
    std::vector<double> AbsoluteUncertainty_SYST1CEMC_20_40;
    std::vector<double> RelativeUncertainty_SYST1CEMC_0_20;
    std::vector<double> AbsoluteUncertainty_SYST1CEMC_0_20;
    
    std::vector<double> RelativeUncertainty_SYST2CEMC_40_60;
    std::vector<double> AbsoluteUncertainty_SYST2CEMC_40_60;
    std::vector<double> RelativeUncertainty_SYST2CEMC_20_40;
    std::vector<double> AbsoluteUncertainty_SYST2CEMC_20_40;
    std::vector<double> RelativeUncertainty_SYST2CEMC_0_20;
    std::vector<double> AbsoluteUncertainty_SYST2CEMC_0_20;

    std::vector<double> RelativeUncertainty_SYST3DCEMC_40_60;
    std::vector<double> AbsoluteUncertainty_SYST3DCEMC_40_60;
    std::vector<double> RelativeUncertainty_SYST3DCEMC_20_40;
    std::vector<double> AbsoluteUncertainty_SYST3DCEMC_20_40;
    std::vector<double> RelativeUncertainty_SYST3DCEMC_0_20;
    std::vector<double> AbsoluteUncertainty_SYST3DCEMC_0_20;
    
    std::vector<double> RelativeUncertainty_SYST3UCEMC_40_60;
    std::vector<double> AbsoluteUncertainty_SYST3UCEMC_40_60;
    std::vector<double> RelativeUncertainty_SYST3UCEMC_20_40;
    std::vector<double> AbsoluteUncertainty_SYST3UCEMC_20_40;
    std::vector<double> RelativeUncertainty_SYST3UCEMC_0_20;
    std::vector<double> AbsoluteUncertainty_SYST3UCEMC_0_20;
    
    std::vector<double> RelativeUncertainty_SYST4CEMC_40_60;
    std::vector<double> AbsoluteUncertainty_SYST4CEMC_40_60;
    std::vector<double> RelativeUncertainty_SYST4CEMC_20_40;
    std::vector<double> AbsoluteUncertainty_SYST4CEMC_20_40;
    std::vector<double> RelativeUncertainty_SYST4CEMC_0_20;
    std::vector<double> AbsoluteUncertainty_SYST4CEMC_0_20;
    
    
    std::vector<double> Relative_QuadSum_40_60;
    std::vector<double> Absolute_QuadSum_40_60;
    std::vector<double> Relative_QuadSum_20_40;
    std::vector<double> Absolute_QuadSum_20_40;
    std::vector<double> Relative_QuadSum_0_20;
    std::vector<double> Absolute_QuadSum_0_20;
    
};

void ReadStatUncertainties(const std::string& filePath, Data& data) {
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
        
        double relativeUncertainty_SYST1CEMC = std::stod(rowData.at(2));
        double absoluteUncertainty_SYST1CEMC = std::stod(rowData.at(3));
        
        double relativeUncertainty_SYST2CEMC = std::stod(rowData.at(4));
        double absoluteUncertainty_SYST2CEMC = std::stod(rowData.at(5));
        
        double relativeUncertainty_SYST3DCEMC = std::stod(rowData.at(6));
        double absoluteUncertainty_SYST3DCEMC = std::stod(rowData.at(7));
        
        double relativeUncertainty_SYST3UCEMC = std::stod(rowData.at(8));
        double absoluteUncertainty_SYST3UCEMC = std::stod(rowData.at(9));
        
        double relativeUncertainty_SYST4CEMC = std::stod(rowData.at(10));
        double absoluteUncertainty_SYST4CEMC = std::stod(rowData.at(11));
        
        double relative_QuadSum = std::stod(rowData.at(12));
        double Absolute_QuadSum = std::stod(rowData.at(13));

        
        
        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            data.RelativeUncertainty_SYST1CEMC_40_60.push_back(relativeUncertainty_SYST1CEMC);
            data.AbsoluteUncertainty_SYST1CEMC_40_60.push_back(absoluteUncertainty_SYST1CEMC);
            
            data.RelativeUncertainty_SYST2CEMC_40_60.push_back(relativeUncertainty_SYST2CEMC);
            data.AbsoluteUncertainty_SYST2CEMC_40_60.push_back(absoluteUncertainty_SYST2CEMC);
            
            data.RelativeUncertainty_SYST3DCEMC_40_60.push_back(relativeUncertainty_SYST3DCEMC);
            data.AbsoluteUncertainty_SYST3DCEMC_40_60.push_back(absoluteUncertainty_SYST3DCEMC);
            
            data.RelativeUncertainty_SYST3UCEMC_40_60.push_back(relativeUncertainty_SYST3UCEMC);
            data.AbsoluteUncertainty_SYST3UCEMC_40_60.push_back(absoluteUncertainty_SYST3UCEMC);
            
            data.RelativeUncertainty_SYST4CEMC_40_60.push_back(relativeUncertainty_SYST4CEMC);
            data.AbsoluteUncertainty_SYST4CEMC_40_60.push_back(absoluteUncertainty_SYST4CEMC);
            
            data.Relative_QuadSum_40_60.push_back(relative_QuadSum);
            data.Absolute_QuadSum_40_60.push_back(Absolute_QuadSum);
            
            
        } else if (index >= 6 && index <= 11) {
            data.RelativeUncertainty_SYST1CEMC_20_40.push_back(relativeUncertainty_SYST1CEMC);
            data.AbsoluteUncertainty_SYST1CEMC_20_40.push_back(absoluteUncertainty_SYST1CEMC);
            
            data.RelativeUncertainty_SYST2CEMC_20_40.push_back(relativeUncertainty_SYST2CEMC);
            data.AbsoluteUncertainty_SYST2CEMC_20_40.push_back(absoluteUncertainty_SYST2CEMC);
            
            data.RelativeUncertainty_SYST3DCEMC_20_40.push_back(relativeUncertainty_SYST3DCEMC);
            data.AbsoluteUncertainty_SYST3DCEMC_20_40.push_back(absoluteUncertainty_SYST3DCEMC);
            
            data.RelativeUncertainty_SYST3UCEMC_20_40.push_back(relativeUncertainty_SYST3UCEMC);
            data.AbsoluteUncertainty_SYST3UCEMC_20_40.push_back(absoluteUncertainty_SYST3UCEMC);
            
            data.RelativeUncertainty_SYST4CEMC_20_40.push_back(relativeUncertainty_SYST4CEMC);
            data.AbsoluteUncertainty_SYST4CEMC_20_40.push_back(absoluteUncertainty_SYST4CEMC);
            
            data.Relative_QuadSum_20_40.push_back(relative_QuadSum);
            data.Absolute_QuadSum_20_40.push_back(Absolute_QuadSum);
            
        } else if (index >= 12 && index <= 17) {
            data.RelativeUncertainty_SYST1CEMC_0_20.push_back(relativeUncertainty_SYST1CEMC);
            data.AbsoluteUncertainty_SYST1CEMC_0_20.push_back(absoluteUncertainty_SYST1CEMC);
            
            data.RelativeUncertainty_SYST2CEMC_0_20.push_back(relativeUncertainty_SYST2CEMC);
            data.AbsoluteUncertainty_SYST2CEMC_0_20.push_back(absoluteUncertainty_SYST2CEMC);
            
            data.RelativeUncertainty_SYST3DCEMC_0_20.push_back(relativeUncertainty_SYST3DCEMC);
            data.AbsoluteUncertainty_SYST3DCEMC_0_20.push_back(absoluteUncertainty_SYST3DCEMC);
            
            data.RelativeUncertainty_SYST3UCEMC_0_20.push_back(relativeUncertainty_SYST3UCEMC);
            data.AbsoluteUncertainty_SYST3UCEMC_0_20.push_back(absoluteUncertainty_SYST3UCEMC);
            
            data.RelativeUncertainty_SYST4CEMC_0_20.push_back(relativeUncertainty_SYST4CEMC);
            data.AbsoluteUncertainty_SYST4CEMC_0_20.push_back(absoluteUncertainty_SYST4CEMC);
            
            data.Relative_QuadSum_0_20.push_back(relative_QuadSum);
            data.Absolute_QuadSum_0_20.push_back(Absolute_QuadSum);
            
        }
    }
    file.close();
    
}


void DrawZeroLine(TCanvas* canvas) {
    if (!canvas) return;
    canvas->Update(); // Make sure the canvas is up-to-date to get correct axis limits
    double x_min = canvas->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max = canvas->GetUxmax(); // Get the maximum x-value from the canvas
    // Create and draw a dashed line at y = 0
    TLine* zeroLine = new TLine(x_min, 0, x_max, 0);
    zeroLine->SetLineStyle(2);
    zeroLine->Draw("SAME");
    canvas->Modified();
    canvas->Update();
}

// Function to create and return a TGraphAsymmErrors pointer
TGraphErrors* CreateSystematicGraph(const std::vector<double>& ptCenters,
                                    const std::vector<double>& values,
                                    const std::vector<double>& statErrors, // Not used here
                                    const std::vector<double>& sysErrors) {
    // Create an array of zero values for the X error since we want vertical boxes only
    std::vector<double> sysx(ptCenters.size(), 0.1); // Replace 0.1 with actual systematic error width if needed
    auto* graph = new TGraphErrors(ptCenters.size(), &ptCenters[0], &values[0], &sysx[0], &sysErrors[0]);
    
    // Set the fill style and color for systematic errors (shaded area)
    graph->SetFillColorAlpha(kRed, 0.35); // Adjust color and transparency as needed
    
    return graph;
}

// Function to create and return a TGraphErrors pointer
TGraphErrors* CreateGraph(const std::vector<double>& ptCenters, const std::vector<double>& values, const std::vector<double>& errors) {
    return new TGraphErrors(ptCenters.size(), &ptCenters[0], &values[0], nullptr, &errors[0]);
}


void plotting(const Data& data1) {
    std::vector<double> ptCenters = {2.25, 2.75, 3.25, 3.75, 4.25, 4.75}; // Mid-points of pT ranges
    std::string BasePlotOutputPath = "/Users/patsfan753/Desktop";
    
    
    TCanvas *c_Overlay_0_20_Systematics = new TCanvas("c_Overlay_0_20_Systematics", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);

    // Create TGraph for systematic uncertainties as points
    TGraph* RelativeUncertainty_SYST1CEMC_0_20_graph = new TGraph(ptCenters.size());
    TGraph* AbsoluteUncertainty_SYST1CEMC_0_20_graph = new TGraph(ptCenters.size());

    // Create TGraph for systematic uncertainties as points
    TGraph* RelativeUncertainty_SYST2CEMC_0_20_graph = new TGraph(ptCenters.size());
    TGraph* AbsoluteUncertainty_SYST2CEMC_0_20_graph = new TGraph(ptCenters.size());
    
    // Create TGraph for systematic uncertainties as points
    TGraph* RelativeUncertainty_SYST3DCEMC_0_20_graph = new TGraph(ptCenters.size());
    TGraph* AbsoluteUncertainty_SYST3DCEMC_0_20_graph = new TGraph(ptCenters.size());
    
    // Create TGraph for systematic uncertainties as points
    TGraph* RelativeUncertainty_SYST3UCEMC_0_20_graph = new TGraph(ptCenters.size());
    TGraph* AbsoluteUncertainty_SYST3UCEMC_0_20_graph = new TGraph(ptCenters.size());
    
    // Create TGraph for systematic uncertainties as points
    TGraph* RelativeUncertainty_SYST4CEMC_0_20_graph = new TGraph(ptCenters.size());
    TGraph* AbsoluteUncertainty_SYST4CEMC_0_20_graph = new TGraph(ptCenters.size());
    
    // Create TGraph for systematic uncertainties as points
    TGraph* Relative_QuadSum_0_20_graph = new TGraph(ptCenters.size());
    TGraph* Absolute_QuadSum_0_20_graph = new TGraph(ptCenters.size());
    
    
    for (int i = 0; i < ptCenters.size(); ++i) {
        // Set the point at the y-value plus the scaled systematic error
        AbsoluteUncertainty_SYST1CEMC_0_20_graph->SetPoint(i, ptCenters[i] - .1, data1.AbsoluteUncertainty_SYST1CEMC_0_20[i]);
        AbsoluteUncertainty_SYST2CEMC_0_20_graph->SetPoint(i, ptCenters[i] - .075, data1.AbsoluteUncertainty_SYST2CEMC_0_20[i]);
        AbsoluteUncertainty_SYST3DCEMC_0_20_graph->SetPoint(i, ptCenters[i] - .025, data1.AbsoluteUncertainty_SYST3DCEMC_0_20[i]);
        AbsoluteUncertainty_SYST3UCEMC_0_20_graph->SetPoint(i, ptCenters[i] +.025, data1.AbsoluteUncertainty_SYST3UCEMC_0_20[i]);
        AbsoluteUncertainty_SYST4CEMC_0_20_graph->SetPoint(i, ptCenters[i] + .055, data1.AbsoluteUncertainty_SYST4CEMC_0_20[i]);
        Absolute_QuadSum_0_20_graph->SetPoint(i, ptCenters[i] + .12, data1.Absolute_QuadSum_0_20[i]);
        
    }

    int markerStyle = 21;

    // Using a distinct and color-blind friendly palette
    AbsoluteUncertainty_SYST1CEMC_0_20_graph->SetMarkerStyle(markerStyle);
    AbsoluteUncertainty_SYST1CEMC_0_20_graph->SetMarkerColor(kRed);
    
    AbsoluteUncertainty_SYST2CEMC_0_20_graph->SetMarkerStyle(markerStyle);
    AbsoluteUncertainty_SYST2CEMC_0_20_graph->SetMarkerColor(kBlue);
    
    AbsoluteUncertainty_SYST3DCEMC_0_20_graph->SetMarkerStyle(markerStyle);
    AbsoluteUncertainty_SYST3DCEMC_0_20_graph->SetMarkerColor(kGreen+1);
    
    AbsoluteUncertainty_SYST3UCEMC_0_20_graph->SetMarkerStyle(markerStyle);
    AbsoluteUncertainty_SYST3UCEMC_0_20_graph->SetMarkerColor(kOrange+7);
    
    AbsoluteUncertainty_SYST4CEMC_0_20_graph->SetMarkerStyle(markerStyle);
    AbsoluteUncertainty_SYST4CEMC_0_20_graph->SetMarkerColor(kViolet); 

    Absolute_QuadSum_0_20_graph->SetMarkerStyle(markerStyle);
    Absolute_QuadSum_0_20_graph->SetMarkerColor(kGray+1);


    Absolute_QuadSum_0_20_graph->Draw("AP"); // Draw the final systematic uncertainties
    AbsoluteUncertainty_SYST1CEMC_0_20_graph->Draw("P SAME");
    AbsoluteUncertainty_SYST2CEMC_0_20_graph->Draw("P SAME");
    AbsoluteUncertainty_SYST3DCEMC_0_20_graph->Draw("P SAME");
    AbsoluteUncertainty_SYST3UCEMC_0_20_graph->Draw("P SAME");
    AbsoluteUncertainty_SYST4CEMC_0_20_graph->Draw("P SAME");
    

    // Set the ranges for the x-axis and y-axis
    Absolute_QuadSum_0_20_graph->GetXaxis()->SetLimits(2.0, 5.0);
    Absolute_QuadSum_0_20_graph->GetYaxis()->SetRangeUser(0, 0.26);

    // Titles
    Absolute_QuadSum_0_20_graph->SetTitle("#pi^{0} #it{v}_{2} Systematic Uncertainties; #it{p}_{T} [GeV/c]; Systematic Uncertainty");

    TLegend *legend_Overlay_0_20 = new TLegend(0.2, 0.53, 0.4, 0.75);
    legend_Overlay_0_20->SetHeader("Absolute Systematic Uncertainties", "R");
    legend_Overlay_0_20->SetTextSize(0.028);
    legend_Overlay_0_20->AddEntry(AbsoluteUncertainty_SYST1CEMC_0_20_graph, "SYST1CEMC", "p");
    legend_Overlay_0_20->AddEntry(AbsoluteUncertainty_SYST2CEMC_0_20_graph, "SYST2CEMC", "p");
    legend_Overlay_0_20->AddEntry(AbsoluteUncertainty_SYST3DCEMC_0_20_graph, "SYST3DCEMC", "p");
    legend_Overlay_0_20->AddEntry(AbsoluteUncertainty_SYST3UCEMC_0_20_graph, "SYST3UCEMC", "p");
    legend_Overlay_0_20->AddEntry(AbsoluteUncertainty_SYST4CEMC_0_20_graph, "SYST4CEMC", "p");
    legend_Overlay_0_20->AddEntry(Absolute_QuadSum_0_20_graph, "Quadrature Sum", "p");
    legend_Overlay_0_20->Draw();

    
    DrawZeroLine(c_Overlay_0_20_Systematics);
    
    TLegend *leg0_20_overlay = new TLegend(0.14,.77,0.34,.92);
    leg0_20_overlay->SetFillStyle(0);
    leg0_20_overlay->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg0_20_overlay->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg0_20_overlay->AddEntry("","0-20% Centrality","");
    leg0_20_overlay->Draw("same");
    
    c_Overlay_0_20_Systematics->Modified();
    c_Overlay_0_20_Systematics->Update();
    c_Overlay_0_20_Systematics->SaveAs((BasePlotOutputPath + "/EMCal_SystContributor_Overlay_0_20.png").c_str());
    
    
    
    
    TCanvas *c_Overlay_20_40_Systematics = new TCanvas("c_Overlay_20_40_Systematics", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);

    // Create TGraph for systematic uncertainties as points
    TGraph* RelativeUncertainty_SYST1CEMC_20_40_graph = new TGraph(ptCenters.size());
    TGraph* AbsoluteUncertainty_SYST1CEMC_20_40_graph = new TGraph(ptCenters.size());

    // Create TGraph for systematic uncertainties as points
    TGraph* RelativeUncertainty_SYST2CEMC_20_40_graph = new TGraph(ptCenters.size());
    TGraph* AbsoluteUncertainty_SYST2CEMC_20_40_graph = new TGraph(ptCenters.size());
    
    // Create TGraph for systematic uncertainties as points
    TGraph* RelativeUncertainty_SYST3DCEMC_20_40_graph = new TGraph(ptCenters.size());
    TGraph* AbsoluteUncertainty_SYST3DCEMC_20_40_graph = new TGraph(ptCenters.size());
    
    // Create TGraph for systematic uncertainties as points
    TGraph* RelativeUncertainty_SYST3UCEMC_20_40_graph = new TGraph(ptCenters.size());
    TGraph* AbsoluteUncertainty_SYST3UCEMC_20_40_graph = new TGraph(ptCenters.size());
    
    // Create TGraph for systematic uncertainties as points
    TGraph* RelativeUncertainty_SYST4CEMC_20_40_graph = new TGraph(ptCenters.size());
    TGraph* AbsoluteUncertainty_SYST4CEMC_20_40_graph = new TGraph(ptCenters.size());
    
    // Create TGraph for systematic uncertainties as points
    TGraph* Relative_QuadSum_20_40_graph = new TGraph(ptCenters.size());
    TGraph* Absolute_QuadSum_20_40_graph = new TGraph(ptCenters.size());
    
    
    for (int i = 0; i < ptCenters.size(); ++i) {
        // Set the point at the y-value plus the scaled systematic error
        AbsoluteUncertainty_SYST1CEMC_20_40_graph->SetPoint(i, ptCenters[i] - .1, data1.AbsoluteUncertainty_SYST1CEMC_20_40[i]);
        AbsoluteUncertainty_SYST2CEMC_20_40_graph->SetPoint(i, ptCenters[i] - .075, data1.AbsoluteUncertainty_SYST2CEMC_20_40[i]);
        AbsoluteUncertainty_SYST3DCEMC_20_40_graph->SetPoint(i, ptCenters[i] - .025, data1.AbsoluteUncertainty_SYST3DCEMC_20_40[i]);
        AbsoluteUncertainty_SYST3UCEMC_20_40_graph->SetPoint(i, ptCenters[i] +.025, data1.AbsoluteUncertainty_SYST3UCEMC_20_40[i]);
        AbsoluteUncertainty_SYST4CEMC_20_40_graph->SetPoint(i, ptCenters[i] + .055, data1.AbsoluteUncertainty_SYST4CEMC_20_40[i]);
        Absolute_QuadSum_20_40_graph->SetPoint(i, ptCenters[i] + .12, data1.Absolute_QuadSum_20_40[i]);
        
    }

    // Using a distinct and color-blind friendly palette
    AbsoluteUncertainty_SYST1CEMC_20_40_graph->SetMarkerStyle(markerStyle);
    AbsoluteUncertainty_SYST1CEMC_20_40_graph->SetMarkerColor(kRed);
    
    AbsoluteUncertainty_SYST2CEMC_20_40_graph->SetMarkerStyle(markerStyle);
    AbsoluteUncertainty_SYST2CEMC_20_40_graph->SetMarkerColor(kBlue);
    
    AbsoluteUncertainty_SYST3DCEMC_20_40_graph->SetMarkerStyle(markerStyle);
    AbsoluteUncertainty_SYST3DCEMC_20_40_graph->SetMarkerColor(kGreen+1);
    
    AbsoluteUncertainty_SYST3UCEMC_20_40_graph->SetMarkerStyle(markerStyle);
    AbsoluteUncertainty_SYST3UCEMC_20_40_graph->SetMarkerColor(kOrange+7);
    
    AbsoluteUncertainty_SYST4CEMC_20_40_graph->SetMarkerStyle(markerStyle);
    AbsoluteUncertainty_SYST4CEMC_20_40_graph->SetMarkerColor(kViolet);

    Absolute_QuadSum_20_40_graph->SetMarkerStyle(markerStyle);
    Absolute_QuadSum_20_40_graph->SetMarkerColor(kGray+1);


    Absolute_QuadSum_20_40_graph->Draw("AP"); // Draw the final systematic uncertainties
    AbsoluteUncertainty_SYST1CEMC_20_40_graph->Draw("P SAME");
    AbsoluteUncertainty_SYST2CEMC_20_40_graph->Draw("P SAME");
    AbsoluteUncertainty_SYST3DCEMC_20_40_graph->Draw("P SAME");
    AbsoluteUncertainty_SYST3UCEMC_20_40_graph->Draw("P SAME");
    AbsoluteUncertainty_SYST4CEMC_20_40_graph->Draw("P SAME");
    

    // Set the ranges for the x-axis and y-axis
    Absolute_QuadSum_20_40_graph->GetXaxis()->SetLimits(2.0, 5.0);
    Absolute_QuadSum_20_40_graph->GetYaxis()->SetRangeUser(0, 0.1);

    // Titles
    Absolute_QuadSum_20_40_graph->SetTitle("#pi^{0} #it{v}_{2} Systematic Uncertainties; #it{p}_{T} [GeV/c]; Systematic Uncertainty");

    TLegend *legend_Overlay_20_40 = new TLegend(0.2, 0.53, 0.4, 0.75);
    legend_Overlay_20_40->SetHeader("Absolute Systematic Uncertainties", "R");
    legend_Overlay_20_40->SetTextSize(0.028);
    legend_Overlay_20_40->AddEntry(AbsoluteUncertainty_SYST1CEMC_20_40_graph, "SYST1CEMC", "p");
    legend_Overlay_20_40->AddEntry(AbsoluteUncertainty_SYST2CEMC_20_40_graph, "SYST2CEMC", "p");
    legend_Overlay_20_40->AddEntry(AbsoluteUncertainty_SYST3DCEMC_20_40_graph, "SYST3DCEMC", "p");
    legend_Overlay_20_40->AddEntry(AbsoluteUncertainty_SYST3UCEMC_20_40_graph, "SYST3UCEMC", "p");
    legend_Overlay_20_40->AddEntry(AbsoluteUncertainty_SYST4CEMC_20_40_graph, "SYST4CEMC", "p");
    legend_Overlay_20_40->AddEntry(Absolute_QuadSum_20_40_graph, "Quadrature Sum", "p");
    legend_Overlay_20_40->Draw();

    
    DrawZeroLine(c_Overlay_20_40_Systematics);
    
    TLegend *leg20_40_overlay = new TLegend(0.14,.77,0.34,.92);
    leg20_40_overlay->SetFillStyle(0);
    leg20_40_overlay->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg20_40_overlay->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg20_40_overlay->AddEntry("","20-40% Centrality","");
    leg20_40_overlay->Draw("same");
    
    c_Overlay_20_40_Systematics->Modified();
    c_Overlay_20_40_Systematics->Update();
    c_Overlay_20_40_Systematics->SaveAs((BasePlotOutputPath + "/EMCal_SystContributor_Overlay_20_40.png").c_str());
    
    
    
    
    TCanvas *c_Overlay_40_60_Systematics = new TCanvas("c_Overlay_40_60_Systematics", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);

    // Create TGraph for systematic uncertainties as points
    TGraph* RelativeUncertainty_SYST1CEMC_40_60_graph = new TGraph(ptCenters.size());
    TGraph* AbsoluteUncertainty_SYST1CEMC_40_60_graph = new TGraph(ptCenters.size());

    // Create TGraph for systematic uncertainties as points
    TGraph* RelativeUncertainty_SYST2CEMC_40_60_graph = new TGraph(ptCenters.size());
    TGraph* AbsoluteUncertainty_SYST2CEMC_40_60_graph = new TGraph(ptCenters.size());
    
    // Create TGraph for systematic uncertainties as points
    TGraph* RelativeUncertainty_SYST3DCEMC_40_60_graph = new TGraph(ptCenters.size());
    TGraph* AbsoluteUncertainty_SYST3DCEMC_40_60_graph = new TGraph(ptCenters.size());
    
    // Create TGraph for systematic uncertainties as points
    TGraph* RelativeUncertainty_SYST3UCEMC_40_60_graph = new TGraph(ptCenters.size());
    TGraph* AbsoluteUncertainty_SYST3UCEMC_40_60_graph = new TGraph(ptCenters.size());
    
    // Create TGraph for systematic uncertainties as points
    TGraph* RelativeUncertainty_SYST4CEMC_40_60_graph = new TGraph(ptCenters.size());
    TGraph* AbsoluteUncertainty_SYST4CEMC_40_60_graph = new TGraph(ptCenters.size());
    
    // Create TGraph for systematic uncertainties as points
    TGraph* Relative_QuadSum_40_60_graph = new TGraph(ptCenters.size());
    TGraph* Absolute_QuadSum_40_60_graph = new TGraph(ptCenters.size());
    
    
    for (int i = 0; i < ptCenters.size(); ++i) {
        // Set the point at the y-value plus the scaled systematic error
        AbsoluteUncertainty_SYST1CEMC_40_60_graph->SetPoint(i, ptCenters[i] - .1, data1.AbsoluteUncertainty_SYST1CEMC_40_60[i]);
        AbsoluteUncertainty_SYST2CEMC_40_60_graph->SetPoint(i, ptCenters[i] - .075, data1.AbsoluteUncertainty_SYST2CEMC_40_60[i]);
        AbsoluteUncertainty_SYST3DCEMC_40_60_graph->SetPoint(i, ptCenters[i] - .025, data1.AbsoluteUncertainty_SYST3DCEMC_40_60[i]);
        AbsoluteUncertainty_SYST3UCEMC_40_60_graph->SetPoint(i, ptCenters[i] +.025, data1.AbsoluteUncertainty_SYST3UCEMC_40_60[i]);
        AbsoluteUncertainty_SYST4CEMC_40_60_graph->SetPoint(i, ptCenters[i] + .055, data1.AbsoluteUncertainty_SYST4CEMC_40_60[i]);
        Absolute_QuadSum_40_60_graph->SetPoint(i, ptCenters[i] + .12, data1.Absolute_QuadSum_40_60[i]);
        
    }

    // Using a distinct and color-blind friendly palette
    AbsoluteUncertainty_SYST1CEMC_40_60_graph->SetMarkerStyle(markerStyle);
    AbsoluteUncertainty_SYST1CEMC_40_60_graph->SetMarkerColor(kRed);
    
    AbsoluteUncertainty_SYST2CEMC_40_60_graph->SetMarkerStyle(markerStyle);
    AbsoluteUncertainty_SYST2CEMC_40_60_graph->SetMarkerColor(kBlue);
    
    AbsoluteUncertainty_SYST3DCEMC_40_60_graph->SetMarkerStyle(markerStyle);
    AbsoluteUncertainty_SYST3DCEMC_40_60_graph->SetMarkerColor(kGreen+1);
    
    AbsoluteUncertainty_SYST3UCEMC_40_60_graph->SetMarkerStyle(markerStyle);
    AbsoluteUncertainty_SYST3UCEMC_40_60_graph->SetMarkerColor(kOrange+7);
    
    AbsoluteUncertainty_SYST4CEMC_40_60_graph->SetMarkerStyle(markerStyle);
    AbsoluteUncertainty_SYST4CEMC_40_60_graph->SetMarkerColor(kViolet);

    Absolute_QuadSum_40_60_graph->SetMarkerStyle(markerStyle);
    Absolute_QuadSum_40_60_graph->SetMarkerColor(kGray+1);


    Absolute_QuadSum_40_60_graph->Draw("AP"); // Draw the final systematic uncertainties
    AbsoluteUncertainty_SYST1CEMC_40_60_graph->Draw("P SAME");
    AbsoluteUncertainty_SYST2CEMC_40_60_graph->Draw("P SAME");
    AbsoluteUncertainty_SYST3DCEMC_40_60_graph->Draw("P SAME");
    AbsoluteUncertainty_SYST3UCEMC_40_60_graph->Draw("P SAME");
    AbsoluteUncertainty_SYST4CEMC_40_60_graph->Draw("P SAME");
    

    // Set the ranges for the x-axis and y-axis
    Absolute_QuadSum_40_60_graph->GetXaxis()->SetLimits(2.0, 5.0);
    Absolute_QuadSum_40_60_graph->GetYaxis()->SetRangeUser(0, 0.12);

    // Titles
    Absolute_QuadSum_40_60_graph->SetTitle("#pi^{0} #it{v}_{2} Systematic Uncertainties; #it{p}_{T} [GeV/c]; Systematic Uncertainty");

    TLegend *legend_Overlay_40_60 = new TLegend(0.2, 0.53, 0.4, 0.75);
    legend_Overlay_40_60->SetHeader("Absolute Systematic Uncertainties", "R");
    legend_Overlay_40_60->SetTextSize(0.028);
    legend_Overlay_40_60->AddEntry(AbsoluteUncertainty_SYST1CEMC_40_60_graph, "SYST1CEMC", "p");
    legend_Overlay_40_60->AddEntry(AbsoluteUncertainty_SYST2CEMC_40_60_graph, "SYST2CEMC", "p");
    legend_Overlay_40_60->AddEntry(AbsoluteUncertainty_SYST3DCEMC_40_60_graph, "SYST3DCEMC", "p");
    legend_Overlay_40_60->AddEntry(AbsoluteUncertainty_SYST3UCEMC_40_60_graph, "SYST3UCEMC", "p");
    legend_Overlay_40_60->AddEntry(AbsoluteUncertainty_SYST4CEMC_40_60_graph, "SYST4CEMC", "p");
    legend_Overlay_40_60->AddEntry(Absolute_QuadSum_40_60_graph, "Quadrature Sum", "p");
    legend_Overlay_40_60->Draw();

    
    DrawZeroLine(c_Overlay_40_60_Systematics);
    
    TLegend *leg40_60_overlay = new TLegend(0.14,.77,0.34,.92);
    leg40_60_overlay->SetFillStyle(0);
    leg40_60_overlay->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg40_60_overlay->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg40_60_overlay->AddEntry("","40-60% Centrality","");
    leg40_60_overlay->Draw("same");
    
    c_Overlay_40_60_Systematics->Modified();
    c_Overlay_40_60_Systematics->Update();
    c_Overlay_40_60_Systematics->SaveAs((BasePlotOutputPath + "/EMCal_SystContributor_Overlay_40_60.png").c_str());
}

void plot_EMCal_SystematicContributions() {
    gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();

    Data data1;
    
    std::string statUncertaintyFilePath = "/Users/patsfan753/Desktop/Systematics_Analysis/StatUncertaintyTable_EMCalVariationsOnly.csv";
    
    ReadStatUncertainties(statUncertaintyFilePath, data1);

    plotting(data1);
}


