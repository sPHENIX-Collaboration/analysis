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
    std::vector<double> corrected_v2_0_20, corrected_v2_20_40, corrected_v2_40_60;
    std::vector<double> corrected_v2_0_20_Errors, corrected_v2_20_40_Errors, corrected_v2_40_60_Errors;
    
    std::vector<double> stat_uncertainties_40_60;
    std::vector<double> stat_uncertainties_20_40;
    std::vector<double> stat_uncertainties_0_20;
    
    std::vector<double> unWeighted_stat_uncertainties_40_60;
    std::vector<double> unWeighted_stat_uncertainties_20_40;
    std::vector<double> unWeighted_stat_uncertainties_0_20;
    
    // New vectors to store the three systematic uncertainties
    std::vector<double> quad_sum_EMCal_syst_0_20, quad_sum_EMCal_syst_20_40, quad_sum_EMCal_syst_40_60;
    std::vector<double> signal_window_syst_0_20, signal_window_syst_20_40, signal_window_syst_40_60;
    std::vector<double> background_window_syst_0_20, background_window_syst_20_40, background_window_syst_40_60;
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
        
        double stat_uncertainty = std::stod(rowData.at(5));
        double unWeighted_stat_uncertainty = std::stod(rowData.at(6));
        double quad_sum_EMCal_syst = std::stod(rowData.at(2)); // Quad_Sum_EMCal_Syst
        double signal_window_syst = std::stod(rowData.at(3));  // SignalWindow_Syst
        double background_window_syst = std::stod(rowData.at(4)); // BackgroundWindow_Syst
        
    
        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            data.stat_uncertainties_40_60.push_back(stat_uncertainty);
            data.unWeighted_stat_uncertainties_40_60.push_back(unWeighted_stat_uncertainty);
            data.quad_sum_EMCal_syst_40_60.push_back(quad_sum_EMCal_syst);
            data.signal_window_syst_40_60.push_back(signal_window_syst);
            data.background_window_syst_40_60.push_back(background_window_syst);
            
            
        } else if (index >= 6 && index <= 11) {
            data.stat_uncertainties_20_40.push_back(stat_uncertainty);
            data.unWeighted_stat_uncertainties_20_40.push_back(unWeighted_stat_uncertainty);
            data.quad_sum_EMCal_syst_20_40.push_back(quad_sum_EMCal_syst);
            data.signal_window_syst_20_40.push_back(signal_window_syst);
            data.background_window_syst_20_40.push_back(background_window_syst);

        } else if (index >= 12 && index <= 17) {
            data.stat_uncertainties_0_20.push_back(stat_uncertainty);
            data.unWeighted_stat_uncertainties_0_20.push_back(unWeighted_stat_uncertainty);
            data.quad_sum_EMCal_syst_0_20.push_back(quad_sum_EMCal_syst);
            data.signal_window_syst_0_20.push_back(signal_window_syst);
            data.background_window_syst_0_20.push_back(background_window_syst);
        }
    }
    file.close();

}
void PrintStatisticalUncertainties(const Data& data, const std::vector<double>& ptCenters) {
    // Print the header of the table
    std::cout << std::left << std::setw(15) << "pT (GeV/c)"
              << std::setw(20) << "Centrality"
              << std::setw(20) << "v2 Value"
              << std::setw(30) << "Scaled EMCal Error"
              << std::setw(30) << "Scaled Signal Error"
              << std::setw(30) << "Scaled Background Error"
              << std::setw(30) << "Final Error via Quad" << std::endl;
    std::cout << std::string(175, '-') << std::endl;

    // Helper lambda to print uncertainties for a given centrality
    auto print_uncertainties = [&](const std::vector<double>& v2_values,
                                   const std::vector<double>& emcal_errors,
                                   const std::vector<double>& signal_errors,
                                   const std::vector<double>& background_errors,
                                   const std::vector<double>& stat_errors,
                                   const std::string& centrality_label) {
        for (size_t i = 0; i < ptCenters.size(); ++i) {
            std::cout << std::left << std::setw(15) << ptCenters[i]
                      << std::setw(20) << centrality_label
                      << std::setw(20) << std::fixed << std::setprecision(4) << v2_values[i]
                      << std::setw(30) << std::fixed << std::setprecision(4) << emcal_errors[i] * v2_values[i]
                      << std::setw(30) << std::fixed << std::setprecision(4) << signal_errors[i] * v2_values[i]
                      << std::setw(30) << std::fixed << std::setprecision(4) << background_errors[i] * v2_values[i]
                      << std::setw(30) << std::fixed << std::setprecision(4) << stat_errors[i] << std::endl;
        }
    };

    // Print uncertainties for 0-20% centrality
    print_uncertainties(data.corrected_v2_0_20, data.quad_sum_EMCal_syst_0_20, data.signal_window_syst_0_20, data.background_window_syst_0_20, data.stat_uncertainties_0_20, "0-20%");
    
    // Print uncertainties for 20-40% centrality
    print_uncertainties(data.corrected_v2_20_40, data.quad_sum_EMCal_syst_20_40, data.signal_window_syst_20_40, data.background_window_syst_20_40, data.stat_uncertainties_20_40, "20-40%");
    
    // Print uncertainties for 40-60% centrality
    print_uncertainties(data.corrected_v2_40_60, data.quad_sum_EMCal_syst_40_60, data.signal_window_syst_40_60, data.background_window_syst_40_60, data.stat_uncertainties_40_60, "40-60%");
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
        
        double v2_corrected = std::stod(rowData.at(1));
        double v2_error_corrected = std::stod(rowData.at(2));
    
        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            data.corrected_v2_40_60.push_back(v2_corrected);
            data.corrected_v2_40_60_Errors.push_back(v2_error_corrected);
            
        } else if (index >= 6 && index <= 11) {
            data.corrected_v2_20_40.push_back(v2_corrected);
            data.corrected_v2_20_40_Errors.push_back(v2_error_corrected);

        } else if (index >= 12 && index <= 17) {
            data.corrected_v2_0_20.push_back(v2_corrected);
            data.corrected_v2_0_20_Errors.push_back(v2_error_corrected);
        }
    }
    file.close();
}

void DrawZeroLine(TCanvas* canvas) {
    if (!canvas) return;
    canvas->Update();
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
                                    const std::vector<double>& statErrors,
                                    const std::vector<double>& sysErrors) {

    std::vector<double> sysx(ptCenters.size(), 0.1);
    auto* graph = new TGraphErrors(ptCenters.size(), &ptCenters[0], &values[0], &sysx[0], &sysErrors[0]);
    
    graph->SetFillColorAlpha(kRed, 0.35);
    
    return graph;
}

// Function to create and return a TGraphErrors pointer
TGraphErrors* CreateGraph(const std::vector<double>& ptCenters, const std::vector<double>& values, const std::vector<double>& errors) {
    return new TGraphErrors(ptCenters.size(), &ptCenters[0], &values[0], nullptr, &errors[0]);
}
void PrintValues(const std::vector<double>& ptCenters,
                 const std::vector<double>& values,
                 const std::vector<double>& statErrors,
                 const std::vector<double>& sysErrors,
                 const std::string& centrality) {
    std::cout << "Centrality: " << centrality << "\n";
    std::cout << "---------------------------------------\n";
    std::cout << "pT (GeV) | v2 | Statistical Error | Systematic Error\n";
    std::cout << "---------------------------------------\n";
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        std::cout << std::fixed << std::setprecision(2) << ptCenters[i] << " GeV | ";
        std::cout << std::fixed << std::setprecision(4) << values[i] << " | ";
        std::cout << std::fixed << std::setprecision(4) << statErrors[i] << " | ";
        std::cout << std::fixed << std::setprecision(4) << sysErrors[i] << "\n";
    }
    std::cout << "---------------------------------------\n\n";
}

void plotting(const Data& data1) {
    std::vector<double> ptCenters = {2.25, 2.75, 3.25, 3.75, 4.25, 4.75}; // Mid-points of pT ranges
    std::string BasePlotOutputPath = "/Users/patsfan753/Desktop";
    
    PrintValues(ptCenters, data1.corrected_v2_0_20, data1.corrected_v2_0_20_Errors, data1.stat_uncertainties_0_20, "0-20%");
    PrintValues(ptCenters, data1.corrected_v2_20_40, data1.corrected_v2_20_40_Errors, data1.stat_uncertainties_20_40, "20-40%");
    PrintValues(ptCenters, data1.corrected_v2_40_60, data1.corrected_v2_40_60_Errors, data1.stat_uncertainties_40_60, "40-60%");
    
    TGraphErrors* corrected_v2_0_20_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_0_20, data1.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_20_40, data1.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_40_60, data1.corrected_v2_40_60_Errors);
    

    TGraphErrors* sys_v2_0_20_graph_1 = CreateSystematicGraph(ptCenters, data1.corrected_v2_0_20, data1.corrected_v2_0_20_Errors, data1.stat_uncertainties_0_20);
    TGraphErrors* sys_v2_20_40_graph_1 = CreateSystematicGraph(ptCenters, data1.corrected_v2_20_40, data1.corrected_v2_20_40_Errors, data1.stat_uncertainties_20_40);
    TGraphErrors* sys_v2_40_60_graph_1 = CreateSystematicGraph(ptCenters, data1.corrected_v2_40_60, data1.corrected_v2_40_60_Errors, data1.stat_uncertainties_40_60);
    
    TGraphErrors* EMCal_syst_0_20_graph = CreateSystematicGraph(ptCenters, data1.corrected_v2_0_20, data1.corrected_v2_0_20_Errors, data1.quad_sum_EMCal_syst_0_20);
    TGraphErrors* EMCal_syst_20_40_graph = CreateSystematicGraph(ptCenters, data1.corrected_v2_20_40, data1.corrected_v2_20_40_Errors, data1.quad_sum_EMCal_syst_20_40);
    TGraphErrors* EMCal_syst_40_60_graph = CreateSystematicGraph(ptCenters, data1.corrected_v2_40_60, data1.corrected_v2_40_60_Errors, data1.quad_sum_EMCal_syst_40_60);
    
    TGraphErrors* signal_window_syst_0_20_graph = CreateSystematicGraph(ptCenters, data1.corrected_v2_0_20, data1.corrected_v2_0_20_Errors, data1.signal_window_syst_0_20);
    TGraphErrors* signal_window_syst_20_40_graph = CreateSystematicGraph(ptCenters, data1.corrected_v2_20_40, data1.corrected_v2_20_40_Errors, data1.signal_window_syst_20_40);
    TGraphErrors* signal_window_syst_40_60_graph = CreateSystematicGraph(ptCenters, data1.corrected_v2_40_60, data1.corrected_v2_40_60_Errors, data1.signal_window_syst_40_60);
    
    TGraphErrors* background_window_syst_0_20_graph = CreateSystematicGraph(ptCenters, data1.corrected_v2_0_20, data1.corrected_v2_0_20_Errors, data1.background_window_syst_0_20);
    TGraphErrors* background_window_syst_20_40_graph = CreateSystematicGraph(ptCenters, data1.corrected_v2_20_40, data1.corrected_v2_20_40_Errors, data1.background_window_syst_20_40);
    TGraphErrors* background_window_syst_40_60_graph = CreateSystematicGraph(ptCenters, data1.corrected_v2_40_60, data1.corrected_v2_40_60_Errors, data1.background_window_syst_40_60);


    
    double minYaxis_correctedComparisons = -0.2;
    double maxYaxis_correctedComparisons = 0.45;
    double TLatexSize = 0.025;
    // Define the offsets for jitter in Overlays
    double offset = 0.05;
    
    int markerColor_1 = kBlack;
    int markerStyle_1 = 20;
    double markerSize_1 = 1.0;
    
    
    corrected_v2_0_20_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_0_20_graph_1->SetLineColor(markerColor_1);
    corrected_v2_0_20_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_0_20_graph_1->SetMarkerStyle(markerStyle_1);
    
    
    
    corrected_v2_20_40_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_20_40_graph_1->SetLineColor(markerColor_1);
    corrected_v2_20_40_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_20_40_graph_1->SetMarkerStyle(markerStyle_1);
    
    
    corrected_v2_40_60_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_40_60_graph_1->SetLineColor(markerColor_1);
    corrected_v2_40_60_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_40_60_graph_1->SetMarkerStyle(markerStyle_1);
    
    
//    sys_v2_0_20_graph_1->SetFillColor(kGray+1); // Gray color
//    sys_v2_0_20_graph_1->SetFillStyle(3001); // Semi-transparent fill
//    
//    sys_v2_20_40_graph_1->SetFillColor(kGray+1); // Gray color
//    sys_v2_20_40_graph_1->SetFillStyle(3001); // Semi-transparent fill
//
//    sys_v2_40_60_graph_1->SetFillColor(kGray+1); // Gray color
//    sys_v2_40_60_graph_1->SetFillStyle(3001); // Semi-transparent fill
//    
//    
//    
//    
//
//    TCanvas *c_Overlay_1 = new TCanvas("c_Overlay_1", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
//    sys_v2_0_20_graph_1->Draw("A2"); // Draw systematic errors first as a shaded area
//    corrected_v2_0_20_graph_1->Draw("P SAME"); // Then draw the statistical errors on top
//    
//    sys_v2_0_20_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 0-20% Centrality");
//    sys_v2_0_20_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
//    sys_v2_0_20_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
//    sys_v2_0_20_graph_1->SetMinimum(-0.35); // Set the minimum y value
//    sys_v2_0_20_graph_1->SetMaximum(0.45); // Set the maximum y value
//
//    DrawZeroLine(c_Overlay_1);
//    TLegend *leg1 = new TLegend(0.14,.19,0.34,.39);
//    leg1->SetFillStyle(0);
//    leg1->AddEntry("","#it{#bf{sPHENIX}} Internal","");
//    leg1->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
//    leg1->AddEntry("","0-20% Centrality","");
//    leg1->Draw("same");
//    
//    TLegend *leg_uncertainty_0_20 = new TLegend(0.67,.19,0.87,.34);
//    leg_uncertainty_0_20->SetTextSize(0.029);
//    leg_uncertainty_0_20->AddEntry(corrected_v2_0_20_graph_1, "Statistical Uncertainty", "pe"); // "lep" for line and marker
//    leg_uncertainty_0_20->AddEntry(sys_v2_0_20_graph_1, "Systematic Uncertainty", "pf"); // "f" for a filled object
//    leg_uncertainty_0_20->Draw("same");
//    
//    
//    c_Overlay_1->SaveAs((BasePlotOutputPath + "/Final_v2_withSyst_0_20.png").c_str());
//    
//    
//    
//
//    TCanvas *c_Overlay_2 = new TCanvas("c_Overlay_2", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 20-40% Centrality", 800, 600);
//    sys_v2_20_40_graph_1->Draw("A2");
//    sys_v2_20_40_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 20-40% Centrality");
//    sys_v2_20_40_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
//    sys_v2_20_40_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
//    sys_v2_20_40_graph_1->SetMinimum(-0.35); // Set the minimum y value
//    sys_v2_20_40_graph_1->SetMaximum(0.45); // Set the maximum y value
//    
//    corrected_v2_20_40_graph_1->Draw("P SAME");
//    
//    DrawZeroLine(c_Overlay_2);
//    TLegend *leg2 = new TLegend(0.14,.19,0.34,.39);
//    leg2->SetFillStyle(0);
//    leg2->AddEntry("","#it{#bf{sPHENIX}} Internal","");
//    leg2->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
//    leg2->AddEntry("","20-40% Centrality","");
//    leg2->Draw("same");
//    
//    TLegend *leg_uncertainty_20_40 = new TLegend(0.67,.19,0.87,.34);
//    leg_uncertainty_20_40->SetTextSize(0.029);
//    leg_uncertainty_20_40->AddEntry(corrected_v2_0_20_graph_1, "Statistical Uncertainty", "pe"); // "lep" for line and marker
//    leg_uncertainty_20_40->AddEntry(sys_v2_0_20_graph_1, "Systematic Uncertainty", "pf"); // "f" for a filled object
//    leg_uncertainty_20_40->Draw("same");
//    
//    
//    c_Overlay_2->SaveAs((BasePlotOutputPath + "/Final_v2_withSyst_20_40.png").c_str());
//    
//    
//    
//    
//
//    TCanvas *c_Overlay_3 = new TCanvas("c_Overlay_3", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
//    sys_v2_40_60_graph_1->Draw("A2");
//    sys_v2_40_60_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality");
//    sys_v2_40_60_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
//    sys_v2_40_60_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
//    sys_v2_40_60_graph_1->SetMinimum(-0.35); // Set the minimum y value
//    sys_v2_40_60_graph_1->SetMaximum(0.45); // Set the maximum y value
//    
//    corrected_v2_40_60_graph_1->Draw("P SAME");
//    
//    DrawZeroLine(c_Overlay_3);
//    TLegend *leg3 = new TLegend(0.14,.19,0.34,.39);
//    leg3->SetFillStyle(0);
//    leg3->AddEntry("","#it{#bf{sPHENIX}} Internal","");
//    leg3->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
//    leg3->AddEntry("","40-60% Centrality","");
//    leg3->Draw("same");
//    
//    TLegend *leg_uncertainty_40_60 = new TLegend(0.67,.19,0.87,.34);
//    leg_uncertainty_40_60->SetTextSize(0.029);
//    leg_uncertainty_40_60->AddEntry(corrected_v2_0_20_graph_1, "Statistical Uncertainty", "pe"); // "lep" for line and marker
//    leg_uncertainty_40_60->AddEntry(sys_v2_0_20_graph_1, "Systematic Uncertainty", "pf"); // "f" for a filled object
//    leg_uncertainty_40_60->Draw("same");
//
//    c_Overlay_3->SaveAs((BasePlotOutputPath + "/Final_v2_withSyst_40_60.png").c_str());

    
    
    TCanvas *c_Overlay_0_20_Systematics = new TCanvas("c_Overlay_0_20_Systematics", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);

    // Create TGraph for systematic uncertainties as points
    TGraph* EMCal_syst_0_20_points = new TGraph(ptCenters.size());
    TGraph* signal_window_syst_0_20_points = new TGraph(ptCenters.size());
    TGraph* background_window_syst_0_20_points = new TGraph(ptCenters.size());
    TGraph* sys_v2_0_20_graph_points = new TGraph(ptCenters.size());
    
    for (int i = 0; i < ptCenters.size(); ++i) {
        double v2_value = data1.corrected_v2_0_20[i];
        // Set the point at the y-value plus the scaled systematic error
        EMCal_syst_0_20_points->SetPoint(i, ptCenters[i] - .075, data1.quad_sum_EMCal_syst_0_20[i] * v2_value);
        signal_window_syst_0_20_points->SetPoint(i, ptCenters[i] - .025, data1.signal_window_syst_0_20[i] * v2_value);
        background_window_syst_0_20_points->SetPoint(i, ptCenters[i] + .025, data1.background_window_syst_0_20[i] * v2_value);
        sys_v2_0_20_graph_points->SetPoint(i, ptCenters[i] + .075, data1.stat_uncertainties_0_20[i]);
        
//        
//        EMCal_syst_0_20_points->SetPoint(i, ptCenters[i] - .075, data1.quad_sum_EMCal_syst_0_20[i]);
//        signal_window_syst_0_20_points->SetPoint(i, ptCenters[i] - .025, data1.signal_window_syst_0_20[i]);
//        background_window_syst_0_20_points->SetPoint(i, ptCenters[i] + .025, data1.background_window_syst_0_20[i]);
//        unWeighted_sys_v2_0_20_graph_points->SetPoint(i, ptCenters[i] + .075, data1.unWeighted_stat_uncertainties_0_20[i]);
        
    }

    // Customize the markers
    EMCal_syst_0_20_points->SetMarkerStyle(21);
    EMCal_syst_0_20_points->SetMarkerColor(kMagenta+1);
    signal_window_syst_0_20_points->SetMarkerStyle(21);
    signal_window_syst_0_20_points->SetMarkerColor(kAzure+10);
    background_window_syst_0_20_points->SetMarkerStyle(21);
    background_window_syst_0_20_points->SetMarkerColor(kYellow+2);
    sys_v2_0_20_graph_points->SetMarkerStyle(21);
    sys_v2_0_20_graph_points->SetMarkerColor(kGray+1);

    sys_v2_0_20_graph_points->Draw("AP"); // Draw the final systematic uncertainties
    EMCal_syst_0_20_points->Draw("P SAME");
    signal_window_syst_0_20_points->Draw("P SAME");
    background_window_syst_0_20_points->Draw("P SAME");
    

    // Set the ranges for the x-axis and y-axis
    sys_v2_0_20_graph_points->GetXaxis()->SetLimits(2.0, 5.0); // pT range
    sys_v2_0_20_graph_points->GetYaxis()->SetRangeUser(0, 0.35); // Systematic uncertainty range

    // Titles
    sys_v2_0_20_graph_points->SetTitle("#pi^{0} #it{v}_{2} Systematic Uncertainties; #it{p}_{T} [GeV/c]; Systematic Uncertainty");

    TLegend *legend_Overlay_0_20 = new TLegend(0.2, 0.55, 0.4, 0.75);
    legend_Overlay_0_20->SetHeader("Absolute Systematic Uncertainties", "R");
    legend_Overlay_0_20->SetTextSize(0.028);
    legend_Overlay_0_20->AddEntry(EMCal_syst_0_20_points, "EMCal Variations", "p");
    legend_Overlay_0_20->AddEntry(signal_window_syst_0_20_points, "Signal Window Variation", "p");
    legend_Overlay_0_20->AddEntry(background_window_syst_0_20_points, "Background Window Variation", "p");
    legend_Overlay_0_20->AddEntry(sys_v2_0_20_graph_points, "Final Systematic Uncertainty", "p");
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
    c_Overlay_0_20_Systematics->SaveAs((BasePlotOutputPath + "/AbsoluteSystUncertainty_Overlay_0_20.png").c_str());
    
    
    
    
    
    // Assuming ptCenters and data1 are defined and filled as in your current code
    TCanvas *c_Overlay_20_40_Systematics = new TCanvas("c_Overlay_20_40_Systematics", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);

    // Create TGraph for systematic uncertainties as points
    TGraph* EMCal_syst_20_40_points = new TGraph(ptCenters.size());
    TGraph* signal_window_syst_20_40_points = new TGraph(ptCenters.size());
    TGraph* background_window_syst_20_40_points = new TGraph(ptCenters.size());
    TGraph* sys_v2_20_40_graph_points = new TGraph(ptCenters.size());
    
    for (int i = 0; i < ptCenters.size(); ++i) {
        double v2_value = data1.corrected_v2_20_40[i];
        // Set the point at the y-value plus the scaled systematic error
        EMCal_syst_20_40_points->SetPoint(i, ptCenters[i] - .075, data1.quad_sum_EMCal_syst_20_40[i] * v2_value);
        signal_window_syst_20_40_points->SetPoint(i, ptCenters[i] - .025, data1.signal_window_syst_20_40[i] * v2_value);
        background_window_syst_20_40_points->SetPoint(i, ptCenters[i] + .025, data1.background_window_syst_20_40[i] * v2_value);
        sys_v2_20_40_graph_points->SetPoint(i, ptCenters[i] + .075, data1.stat_uncertainties_20_40[i]);
        
        
//        EMCal_syst_20_40_points->SetPoint(i, ptCenters[i] - .075, data1.quad_sum_EMCal_syst_20_40[i]);
//        signal_window_syst_20_40_points->SetPoint(i, ptCenters[i] - .025, data1.signal_window_syst_20_40[i]);
//        background_window_syst_20_40_points->SetPoint(i, ptCenters[i] + .025, data1.background_window_syst_20_40[i]);
//        unWeighted_sys_v2_20_40_graph_points->SetPoint(i, ptCenters[i] + .075, data1.unWeighted_stat_uncertainties_20_40[i]);
    }

    // Customize the markers
    EMCal_syst_20_40_points->SetMarkerStyle(21);
    EMCal_syst_20_40_points->SetMarkerColor(kMagenta+1);
    signal_window_syst_20_40_points->SetMarkerStyle(21);
    signal_window_syst_20_40_points->SetMarkerColor(kAzure+10);
    background_window_syst_20_40_points->SetMarkerStyle(21);
    background_window_syst_20_40_points->SetMarkerColor(kYellow+2);
    sys_v2_20_40_graph_points->SetMarkerStyle(21);
    sys_v2_20_40_graph_points->SetMarkerColor(kGray+1);

    sys_v2_20_40_graph_points->Draw("AP"); // Draw the final systematic uncertainties
    EMCal_syst_20_40_points->Draw("P SAME");
    signal_window_syst_20_40_points->Draw("P SAME");
    background_window_syst_20_40_points->Draw("P SAME");
    

    // Set the ranges for the x-axis and y-axis
    sys_v2_20_40_graph_points->GetXaxis()->SetLimits(2.0, 5.0); // pT range
    sys_v2_20_40_graph_points->GetYaxis()->SetRangeUser(0, 0.12); // Systematic uncertainty range

    // Titles
    sys_v2_20_40_graph_points->SetTitle("#pi^{0} #it{v}_{2} Systematic Uncertainties; #it{p}_{T} [GeV/c]; Systematic Uncertainty");

    TLegend *legend_Overlay_20_40 = new TLegend(0.2, 0.55, 0.4, 0.75);
    legend_Overlay_20_40->SetHeader("Absolute Systematic Uncertainties", "R");
    legend_Overlay_20_40->SetTextSize(0.03);
    legend_Overlay_20_40->AddEntry(EMCal_syst_20_40_points, "EMCal Variations", "p");
    legend_Overlay_20_40->AddEntry(signal_window_syst_20_40_points, "Signal Window Variation", "p");
    legend_Overlay_20_40->AddEntry(background_window_syst_20_40_points, "Background Window Variation", "p");
    legend_Overlay_20_40->AddEntry(sys_v2_20_40_graph_points, "Final Systematic Uncertainty", "p");
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
    c_Overlay_20_40_Systematics->SaveAs((BasePlotOutputPath + "/AbsoluteSystUncertainty_Overlay_20_40.png").c_str());
    
    
    
    
    
    // Assuming ptCenters and data1 are defined and filled as in your current code
    TCanvas *c_Overlay_40_60_Systematics = new TCanvas("c_Overlay_40_60_Systematics", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);

    // Create TGraph for systematic uncertainties as points
    TGraph* EMCal_syst_40_60_points = new TGraph(ptCenters.size());
    TGraph* signal_window_syst_40_60_points = new TGraph(ptCenters.size());
    TGraph* background_window_syst_40_60_points = new TGraph(ptCenters.size());
    TGraph* sys_v2_40_60_graph_points = new TGraph(ptCenters.size());
    
    for (int i = 0; i < ptCenters.size(); ++i) {
        double v2_value = data1.corrected_v2_40_60[i];
//        // Set the point at the y-value plus the scaled systematic error
        EMCal_syst_40_60_points->SetPoint(i, ptCenters[i] - .075, data1.quad_sum_EMCal_syst_40_60[i] * v2_value);
        signal_window_syst_40_60_points->SetPoint(i, ptCenters[i] - .025, data1.signal_window_syst_40_60[i] * v2_value);
        background_window_syst_40_60_points->SetPoint(i, ptCenters[i] + .025, data1.background_window_syst_40_60[i] * v2_value);
        sys_v2_40_60_graph_points->SetPoint(i, ptCenters[i] + .075, data1.stat_uncertainties_40_60[i]);
        
        
//        EMCal_syst_40_60_points->SetPoint(i, ptCenters[i] - .075, data1.quad_sum_EMCal_syst_40_60[i]);
//        signal_window_syst_40_60_points->SetPoint(i, ptCenters[i] - .025, data1.signal_window_syst_40_60[i]);
//        background_window_syst_40_60_points->SetPoint(i, ptCenters[i] + .025, data1.background_window_syst_40_60[i]);
//        unWeighted_sys_v2_40_60_graph_points->SetPoint(i, ptCenters[i] + .075, data1.unWeighted_stat_uncertainties_40_60[i]);
    }

    // Customize the markers
    EMCal_syst_40_60_points->SetMarkerStyle(21);
    EMCal_syst_40_60_points->SetMarkerColor(kMagenta+1);
    signal_window_syst_40_60_points->SetMarkerStyle(21);
    signal_window_syst_40_60_points->SetMarkerColor(kAzure+10);
    background_window_syst_40_60_points->SetMarkerStyle(21);
    background_window_syst_40_60_points->SetMarkerColor(kYellow+2);
    sys_v2_40_60_graph_points->SetMarkerStyle(21);
    sys_v2_40_60_graph_points->SetMarkerColor(kGray+1);

    sys_v2_40_60_graph_points->Draw("AP"); // Draw the final systematic uncertainties
    EMCal_syst_40_60_points->Draw("P SAME");
    signal_window_syst_40_60_points->Draw("P SAME");
    background_window_syst_40_60_points->Draw("P SAME");
    

    // Set the ranges for the x-axis and y-axis
    sys_v2_40_60_graph_points->GetXaxis()->SetLimits(2.0, 5.0); // pT range
    sys_v2_40_60_graph_points->GetYaxis()->SetRangeUser(0, 0.16); // Systematic uncertainty range

    // Titles
    sys_v2_40_60_graph_points->SetTitle("#pi^{0} #it{v}_{2} Systematic Uncertainties; #it{p}_{T} [GeV/c]; Systematic Uncertainty");

    TLegend *legend_Overlay_40_60 = new TLegend(0.2, 0.55, 0.4, 0.75);
    legend_Overlay_40_60->SetHeader("Absolute Systematic Uncertainties", "R");
    legend_Overlay_40_60->SetTextSize(0.028);
    legend_Overlay_40_60->AddEntry(EMCal_syst_40_60_points, "EMCal Variations", "p");
    legend_Overlay_40_60->AddEntry(signal_window_syst_40_60_points, "Signal Window Variation", "p");
    legend_Overlay_40_60->AddEntry(background_window_syst_40_60_points, "Background Window Variation", "p");
    legend_Overlay_40_60->AddEntry(sys_v2_40_60_graph_points, "Final Systematic Uncertainty", "p");
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
    c_Overlay_40_60_Systematics->SaveAs((BasePlotOutputPath + "/AbsoluteSystUncertainty_Overlay_40_60.png").c_str());
}

void v2_Results_Plotter() {
    gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();
    
    std::string filePath1 = "/Users/patsfan753/Desktop/Default_Final_v2_energyCutsFinal.csv";

    Data data1;

    // Read the datasets
    Read_DataSet(filePath1, data1);
    
    std::string statUncertaintyFilePath = "/Users/patsfan753/Desktop/Systematics_Analysis/StatUncertaintyTable.csv";
    
    ReadStatUncertainties(statUncertaintyFilePath, data1);

    std::vector<double> ptCenters = {2.25, 2.75, 3.25, 3.75, 4.25, 4.75}; // Mid-points of pT ranges
    PrintStatisticalUncertainties(data1, ptCenters);
    
    plotting(data1);
}

