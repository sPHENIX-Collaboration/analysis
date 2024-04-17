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
struct PHENIX_data {
    std::vector<double> v2_0_20_PHENIX;
    std::vector<double> v2_0_20_Errors_PHENIX;
    std::vector<double> v2_20_40_PHENIX;
    std::vector<double> v2_20_40_Errors_PHENIX;
    std::vector<double> v2_40_60_PHENIX;
    std::vector<double> v2_40_60_Errors_PHENIX;
};

void ReadPHENIXData(std::string filePath,
                    std::vector<double>& v2_0_10,
                    std::vector<double>& v2_0_10_Errors,
                    std::vector<double>& v2_0_10_Errors_Negative,
                    std::vector<double>& v2_10_20,
                    std::vector<double>& v2_10_20_Errors,
                    std::vector<double>& v2_10_20_Errors_Negative,
                    std::vector<double>& v2_20_30,
                    std::vector<double>& v2_20_30_Errors,
                    std::vector<double>& v2_20_30_Errors_Negative,
                    std::vector<double>& v2_30_40,
                    std::vector<double>& v2_30_40_Errors,
                    std::vector<double>& v2_30_40_Errors_Negative,
                    std::vector<double>& v2_40_50,
                    std::vector<double>& v2_40_50_Errors,
                    std::vector<double>& v2_40_50_Errors_Negative,
                    std::vector<double>& v2_50_60,
                    std::vector<double>& v2_50_60_Errors,
                    std::vector<double>& v2_50_60_Errors_Negative) {
    std::ifstream file(filePath);
    std::string line;
    double v2, error_pos, error_neg;
    int rowCounter = 0;
    // Skip the header line
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> rowData;
        while (std::getline(ss, cell, ',')) {
            rowData.push_back(cell);
        }
        v2 = std::stod(rowData.at(2));
        error_pos = std::stod(rowData.at(rowData.size() - 2)); // stat. +
        error_neg = std::stod(rowData.back()); // stat. -
        // Assigning to the correct centrality vector based on row count
        if (rowCounter < 6) { // 0-10%
            v2_0_10.push_back(v2);
            v2_0_10_Errors.push_back(error_pos);
            v2_0_10_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for v2_0_10:\033[0m " << v2 << " \u00B1 " << error_pos << std::endl;
        } else if (rowCounter < 12) { // 10-20%
            v2_10_20.push_back(v2);
            v2_10_20_Errors.push_back(error_pos);
            v2_10_20_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for v2_10_20:\033[0m " << v2 << " \u00B1 " << error_pos << std::endl;
        } else if (rowCounter < 18) { // 20-30%
            v2_20_30.push_back(v2);
            v2_20_30_Errors.push_back(error_pos);
            v2_20_30_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for v2_20_30:\033[0m " << v2 << " \u00B1 " << error_pos << std::endl;
        } else if (rowCounter < 24) { // 30-40%
            v2_30_40.push_back(v2);
            v2_30_40_Errors.push_back(error_pos);
            v2_30_40_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for v2_30_40:\033[0m " << v2 << " \u00B1 " << error_pos << std::endl;
        } else if (rowCounter < 30) { // 40-50%
            v2_40_50.push_back(v2);
            v2_40_50_Errors.push_back(error_pos);
            v2_40_50_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for v2_40_50:\033[0m " << v2 << " \u00B1 " << error_pos << std::endl;
        } else { // 50-60%
            v2_50_60.push_back(v2);
            v2_50_60_Errors.push_back(error_pos);
            v2_50_60_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for v2_50_60:\033[0m " << v2 << " \u00B1 " << error_pos << std::endl;
        }
        rowCounter++;
    }
    file.close();
}
void CombineCentralityData(const std::vector<double>& v2_0_10, const std::vector<double>& v2_0_10_Errors,
                           const std::vector<double>& v2_10_20, const std::vector<double>& v2_10_20_Errors,
                           std::vector<double>& v2_0_20_PHENIX, std::vector<double>& v2_0_20_Errors_PHENIX,
                           const std::vector<double>& v2_20_30, const std::vector<double>& v2_20_30_Errors,
                           const std::vector<double>& v2_30_40, const std::vector<double>& v2_30_40_Errors,
                           std::vector<double>& v2_20_40_PHENIX, std::vector<double>& v2_20_40_Errors_PHENIX,
                           const std::vector<double>& v2_40_50, const std::vector<double>& v2_40_50_Errors,
                           const std::vector<double>& v2_50_60, const std::vector<double>& v2_50_60_Errors,
                           std::vector<double>& v2_40_60_PHENIX, std::vector<double>& v2_40_60_Errors_PHENIX) {
    for (size_t i = 0; i < v2_0_10.size(); ++i) {
        // Compute average v2
        double avg_v2 = (v2_0_10[i] + v2_10_20[i]) / 2.0;
        v2_0_20_PHENIX.push_back(avg_v2);

        // Propagate error assuming statistical independence
        double combined_error = sqrt(pow(v2_0_10_Errors[i], 2) + pow(v2_10_20_Errors[i], 2)) / 2.0;
        v2_0_20_Errors_PHENIX.push_back(combined_error);
    }
    
    for (size_t i = 0; i < v2_20_30.size(); ++i) {
        double avg_v2_20_40 = (v2_20_30[i] + v2_30_40[i]) / 2.0;
        v2_20_40_PHENIX.push_back(avg_v2_20_40);
        double combined_error_20_40 = sqrt(pow(v2_20_30_Errors[i], 2) + pow(v2_30_40_Errors[i], 2)) / 2.0;
        v2_20_40_Errors_PHENIX.push_back(combined_error_20_40);
    }
    
    for (size_t i = 0; i < v2_40_50.size(); ++i) {
        double avg_v2_40_60 = (v2_40_50[i] + v2_50_60[i]) / 2.0;
        v2_40_60_PHENIX.push_back(avg_v2_40_60);
        double combined_error_40_60 = sqrt(pow(v2_40_50_Errors[i], 2) + pow(v2_50_60_Errors[i], 2)) / 2.0;
        v2_40_60_Errors_PHENIX.push_back(combined_error_40_60);
    }
}

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
    


    
    double minYaxis_correctedComparisons = -0.2;
    double maxYaxis_correctedComparisons = 0.45;
    double TLatexSize = 0.025;

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
    
    TGraphErrors* sys_v2_0_20_graph_1 = CreateSystematicGraph(ptCenters, data1.corrected_v2_0_20, data1.corrected_v2_0_20_Errors, data1.stat_uncertainties_0_20);
    TGraphErrors* sys_v2_20_40_graph_1 = CreateSystematicGraph(ptCenters, data1.corrected_v2_20_40, data1.corrected_v2_20_40_Errors, data1.stat_uncertainties_20_40);
    TGraphErrors* sys_v2_40_60_graph_1 = CreateSystematicGraph(ptCenters, data1.corrected_v2_40_60, data1.corrected_v2_40_60_Errors, data1.stat_uncertainties_40_60);
    
    
    sys_v2_0_20_graph_1->SetFillColor(kGray+1); // Gray color
    sys_v2_0_20_graph_1->SetFillStyle(3001); // Semi-transparent fill
    
    sys_v2_20_40_graph_1->SetFillColor(kGray+1); // Gray color
    sys_v2_20_40_graph_1->SetFillStyle(3001); // Semi-transparent fill

    sys_v2_40_60_graph_1->SetFillColor(kGray+1); // Gray color
    sys_v2_40_60_graph_1->SetFillStyle(3001); // Semi-transparent fill
    
    
    
    
/*
 FINAL RESULTS WITH SYSTEMATIC UNCERTAINTY
 */
    TCanvas *c_Overlay_1_finalResults = new TCanvas("c_Overlay_1_finalResults", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    sys_v2_0_20_graph_1->Draw("A2"); // Draw systematic errors first as a shaded area
    corrected_v2_0_20_graph_1->Draw("P SAME"); // Then draw the statistical errors on top
    
    sys_v2_0_20_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 0-20% Centrality");
    sys_v2_0_20_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    sys_v2_0_20_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    sys_v2_0_20_graph_1->SetMinimum(-0.35); // Set the minimum y value
    sys_v2_0_20_graph_1->SetMaximum(0.45); // Set the maximum y value

    DrawZeroLine(c_Overlay_1_finalResults);
    TLegend *leg0_20_FinalResults = new TLegend(0.14,.19,0.34,.39);
    leg0_20_FinalResults->SetFillStyle(0);
    leg0_20_FinalResults->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg0_20_FinalResults->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg0_20_FinalResults->AddEntry("","0-20% Centrality","");
    leg0_20_FinalResults->Draw("same");
    
    TLegend *leg_uncertainty_0_20 = new TLegend(0.67,.19,0.87,.34);
    leg_uncertainty_0_20->SetTextSize(0.029);
    leg_uncertainty_0_20->AddEntry(corrected_v2_0_20_graph_1, "Statistical Uncertainty", "pe"); // "lep" for line and marker
    leg_uncertainty_0_20->AddEntry(sys_v2_0_20_graph_1, "Systematic Uncertainty", "pf"); // "f" for a filled object
    leg_uncertainty_0_20->Draw("same");
    
    
    c_Overlay_1_finalResults->SaveAs((BasePlotOutputPath + "/Final_v2_withSyst_0_20.png").c_str());
    
    
    

    TCanvas *c_Overlay_2_finalResults = new TCanvas("c_Overlay_2_finalResults", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 20-40% Centrality", 800, 600);
    sys_v2_20_40_graph_1->Draw("A2");
    sys_v2_20_40_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 20-40% Centrality");
    sys_v2_20_40_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    sys_v2_20_40_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    sys_v2_20_40_graph_1->SetMinimum(-0.35); // Set the minimum y value
    sys_v2_20_40_graph_1->SetMaximum(0.45); // Set the maximum y value
    
    corrected_v2_20_40_graph_1->Draw("P SAME");
    
    DrawZeroLine(c_Overlay_2_finalResults);
    TLegend *leg20_40_FinalResults = new TLegend(0.14,.19,0.34,.39);
    leg20_40_FinalResults->SetFillStyle(0);
    leg20_40_FinalResults->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg20_40_FinalResults->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg20_40_FinalResults->AddEntry("","20-40% Centrality","");
    leg20_40_FinalResults->Draw("same");
    
    TLegend *leg_uncertainty_20_40 = new TLegend(0.67,.19,0.87,.34);
    leg_uncertainty_20_40->SetTextSize(0.029);
    leg_uncertainty_20_40->AddEntry(corrected_v2_0_20_graph_1, "Statistical Uncertainty", "pe"); // "lep" for line and marker
    leg_uncertainty_20_40->AddEntry(sys_v2_0_20_graph_1, "Systematic Uncertainty", "pf"); // "f" for a filled object
    leg_uncertainty_20_40->Draw("same");
    
    
    c_Overlay_2_finalResults->SaveAs((BasePlotOutputPath + "/Final_v2_withSyst_20_40.png").c_str());
    
    
    
    

    TCanvas *c_Overlay_3_finalResults = new TCanvas("c_Overlay_3_finalResults", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    sys_v2_40_60_graph_1->Draw("A2");
    sys_v2_40_60_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality");
    sys_v2_40_60_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    sys_v2_40_60_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    sys_v2_40_60_graph_1->SetMinimum(-0.35); // Set the minimum y value
    sys_v2_40_60_graph_1->SetMaximum(0.45); // Set the maximum y value
    
    corrected_v2_40_60_graph_1->Draw("P SAME");
    
    DrawZeroLine(c_Overlay_3_finalResults);
    TLegend *leg40_60_FinalResults = new TLegend(0.14,.19,0.34,.39);
    leg40_60_FinalResults->SetFillStyle(0);
    leg40_60_FinalResults->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg40_60_FinalResults->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg40_60_FinalResults->AddEntry("","40-60% Centrality","");
    leg40_60_FinalResults->Draw("same");
    
    TLegend *leg_uncertainty_40_60 = new TLegend(0.67,.19,0.87,.34);
    leg_uncertainty_40_60->SetTextSize(0.029);
    leg_uncertainty_40_60->AddEntry(corrected_v2_0_20_graph_1, "Statistical Uncertainty", "pe"); // "lep" for line and marker
    leg_uncertainty_40_60->AddEntry(sys_v2_0_20_graph_1, "Systematic Uncertainty", "pf"); // "f" for a filled object
    leg_uncertainty_40_60->Draw("same");

    c_Overlay_3_finalResults->SaveAs((BasePlotOutputPath + "/Final_v2_withSyst_40_60.png").c_str());
    
    

    
    
    /*
     OVERLAYING WITH PHENIX DATA FINAL RESULTS
     */
    
    std::string phenixFilePath = "/Users/patsfan753/Desktop/vN_AnalysisFinal/data/PHENIX_Data_Overlayed/FinalCleanedPhenix.csv";
    //vectors for PHENIX data
    std::vector<double> v2_0_10, v2_0_10_Errors, v2_0_10_Errors_Negative, v2_10_20, v2_10_20_Errors, v2_10_20_Errors_Negative, v2_20_30, v2_20_30_Errors, v2_20_30_Errors_Negative, v2_30_40, v2_30_40_Errors, v2_30_40_Errors_Negative, v2_40_50, v2_40_50_Errors, v2_40_50_Errors_Negative, v2_50_60, v2_50_60_Errors, v2_50_60_Errors_Negative;
    
    // Read in the data and errors
    ReadPHENIXData(phenixFilePath, v2_0_10, v2_0_10_Errors, v2_0_10_Errors_Negative,
                   v2_10_20, v2_10_20_Errors, v2_10_20_Errors_Negative,
                   v2_20_30, v2_20_30_Errors, v2_20_30_Errors_Negative,
                   v2_30_40, v2_30_40_Errors, v2_30_40_Errors_Negative,
                   v2_40_50, v2_40_50_Errors, v2_40_50_Errors_Negative,
                   v2_50_60, v2_50_60_Errors, v2_50_60_Errors_Negative);
    
    int markerStyle_PHENIX = 22;
    int color_PHENIX_low = kBlack;
    int color_PHENIX_high= kGreen+3;
    
    // Combine data for 0-20%
    PHENIX_data combinedData;
    CombineCentralityData(v2_0_10, v2_0_10_Errors, v2_10_20, v2_10_20_Errors,
                          combinedData.v2_0_20_PHENIX, combinedData.v2_0_20_Errors_PHENIX, 
                          v2_20_30, v2_20_30_Errors, v2_30_40, v2_30_40_Errors,
                          combinedData.v2_20_40_PHENIX, combinedData.v2_20_40_Errors_PHENIX,
                          v2_40_50, v2_40_50_Errors, v2_50_60, v2_50_60_Errors,
                          combinedData.v2_40_60_PHENIX, combinedData.v2_40_60_Errors_PHENIX);

    // Create graph for combined data
    TGraphErrors* graph_0_20_PHENIXdataAveraged = CreateGraph(ptCenters, combinedData.v2_0_20_PHENIX, combinedData.v2_0_20_Errors_PHENIX);
    graph_0_20_PHENIXdataAveraged->SetMarkerColor(kBlue);
    graph_0_20_PHENIXdataAveraged->SetLineColor(kBlue);
    graph_0_20_PHENIXdataAveraged->SetMarkerStyle(20);
    graph_0_20_PHENIXdataAveraged->SetMarkerSize(1.0);

    TGraphErrors* graph_20_40_PHENIXdataAveraged = CreateGraph(ptCenters, combinedData.v2_20_40_PHENIX, combinedData.v2_20_40_Errors_PHENIX);
    graph_20_40_PHENIXdataAveraged->SetMarkerColor(kBlue);
    graph_20_40_PHENIXdataAveraged->SetLineColor(kBlue);
    graph_20_40_PHENIXdataAveraged->SetMarkerStyle(20);
    graph_20_40_PHENIXdataAveraged->SetMarkerSize(1.0);

    TGraphErrors* graph_40_60_PHENIXdataAveraged = CreateGraph(ptCenters, combinedData.v2_40_60_PHENIX, combinedData.v2_40_60_Errors_PHENIX);
    graph_40_60_PHENIXdataAveraged->SetMarkerColor(kBlue);
    graph_40_60_PHENIXdataAveraged->SetLineColor(kBlue);
    graph_40_60_PHENIXdataAveraged->SetMarkerStyle(20);
    graph_40_60_PHENIXdataAveraged->SetMarkerSize(1.0);
    
    
    
    TCanvas *c_Overlay_ResultsWithPHENIX_0_20 = new TCanvas("c_Overlay_ResultsWithPHENIX_0_20", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        sys_v2_0_20_graph_1->SetPoint(i, ptCenters[i] - .1, data1.corrected_v2_0_20[i]);
        corrected_v2_0_20_graph_1->SetPoint(i, ptCenters[i] - .1, data1.corrected_v2_0_20[i]);
    }
    sys_v2_0_20_graph_1->Draw("A2"); // Draw systematic errors first as a shaded area
    corrected_v2_0_20_graph_1->Draw("P SAME"); // Then draw the statistical errors on top
    
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_0_20_PHENIXdataAveraged->SetPoint(i, ptCenters[i] + .1, combinedData.v2_0_20_PHENIX[i]);
    }
    graph_0_20_PHENIXdataAveraged->Draw("P SAME");
    
    sys_v2_0_20_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 0-20% Centrality");
    sys_v2_0_20_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    sys_v2_0_20_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    sys_v2_0_20_graph_1->SetMinimum(-0.35); // Set the minimum y value
    sys_v2_0_20_graph_1->SetMaximum(0.45); // Set the maximum y value

    DrawZeroLine(c_Overlay_ResultsWithPHENIX_0_20);
    TLegend *leg1_PHENIXoverlay = new TLegend(0.14,.19,0.34,.39);
    leg1_PHENIXoverlay->SetFillStyle(0);
    leg1_PHENIXoverlay->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg1_PHENIXoverlay->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg1_PHENIXoverlay->AddEntry("","0-20% Centrality","");
    leg1_PHENIXoverlay->Draw("same");
    
    TLegend *leg_uncertainty_0_20_forPHENIXoverlay = new TLegend(0.67,.19,0.87,.4);
    leg_uncertainty_0_20_forPHENIXoverlay->SetTextSize(0.029);
    leg_uncertainty_0_20_forPHENIXoverlay->AddEntry(graph_0_20_PHENIXdataAveraged, "PHENIX, 2010 Data", "pe");
    leg_uncertainty_0_20_forPHENIXoverlay->AddEntry(corrected_v2_0_20_graph_1, "Statistical Uncertainty", "pe"); // "lep" for line and marker
    leg_uncertainty_0_20_forPHENIXoverlay->AddEntry(sys_v2_0_20_graph_1, "Systematic Uncertainty", "pf"); // "f" for a filled object
    leg_uncertainty_0_20_forPHENIXoverlay->Draw("same");
    
    
    c_Overlay_ResultsWithPHENIX_0_20->SaveAs((BasePlotOutputPath + "/Final_v2_withSyst_0_20_OverlayedWithPHENIX.png").c_str());
    
    
    

    TCanvas *c_Overlay_ResultsWithPHENIX_20_40 = new TCanvas("c_Overlay_ResultsWithPHENIX_20_40", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        sys_v2_20_40_graph_1->SetPoint(i, ptCenters[i] - .1, data1.corrected_v2_20_40[i]);
        corrected_v2_20_40_graph_1->SetPoint(i, ptCenters[i] - .1, data1.corrected_v2_20_40[i]);
    }
    sys_v2_20_40_graph_1->Draw("A2"); // Draw systematic errors first as a shaded area
    corrected_v2_20_40_graph_1->Draw("P SAME"); // Then draw the statistical errors on top
    
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_20_40_PHENIXdataAveraged->SetPoint(i, ptCenters[i] + .1, combinedData.v2_20_40_PHENIX[i]);
    }
    graph_20_40_PHENIXdataAveraged->Draw("P SAME");
    
    sys_v2_20_40_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 0-20% Centrality");
    sys_v2_20_40_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    sys_v2_20_40_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    sys_v2_20_40_graph_1->SetMinimum(-0.35); // Set the minimum y value
    sys_v2_20_40_graph_1->SetMaximum(0.45); // Set the maximum y value

    DrawZeroLine(c_Overlay_ResultsWithPHENIX_20_40);
    TLegend *leg2_PHENIXoverlay = new TLegend(0.14,.19,0.34,.39);
    leg2_PHENIXoverlay->SetFillStyle(0);
    leg2_PHENIXoverlay->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg2_PHENIXoverlay->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg2_PHENIXoverlay->AddEntry("","20-40% Centrality","");
    leg2_PHENIXoverlay->Draw("same");
    
    TLegend *leg_uncertainty_20_40_forPHENIXoverlay = new TLegend(0.67,.19,0.87,.4);
    leg_uncertainty_20_40_forPHENIXoverlay->SetTextSize(0.029);
    leg_uncertainty_20_40_forPHENIXoverlay->AddEntry(graph_20_40_PHENIXdataAveraged, "PHENIX, 2010 Data", "pe");
    leg_uncertainty_20_40_forPHENIXoverlay->AddEntry(corrected_v2_20_40_graph_1, "Statistical Uncertainty", "pe"); // "lep" for line and marker
    leg_uncertainty_20_40_forPHENIXoverlay->AddEntry(sys_v2_20_40_graph_1, "Systematic Uncertainty", "pf"); // "f" for a filled object
    leg_uncertainty_20_40_forPHENIXoverlay->Draw("same");
    
    
    c_Overlay_ResultsWithPHENIX_20_40->SaveAs((BasePlotOutputPath + "/Final_v2_withSyst_20_40_OverlayedWithPHENIX.png").c_str());
    
    
    TCanvas *c_Overlay_ResultsWithPHENIX_40_60 = new TCanvas("c_Overlay_ResultsWithPHENIX_40_60", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        sys_v2_40_60_graph_1->SetPoint(i, ptCenters[i] - .1, data1.corrected_v2_40_60[i]);
        corrected_v2_40_60_graph_1->SetPoint(i, ptCenters[i] - .1, data1.corrected_v2_40_60[i]);
    }
    sys_v2_40_60_graph_1->Draw("A2"); // Draw systematic errors first as a shaded area
    corrected_v2_40_60_graph_1->Draw("P SAME"); // Then draw the statistical errors on top
    
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_40_60_PHENIXdataAveraged->SetPoint(i, ptCenters[i] + .1, combinedData.v2_40_60_PHENIX[i]);
    }
    graph_40_60_PHENIXdataAveraged->Draw("P SAME");
    
    sys_v2_40_60_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 0-20% Centrality");
    sys_v2_40_60_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    sys_v2_40_60_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    sys_v2_40_60_graph_1->SetMinimum(-0.35); // Set the minimum y value
    sys_v2_40_60_graph_1->SetMaximum(0.45); // Set the maximum y value

    DrawZeroLine(c_Overlay_ResultsWithPHENIX_40_60);
    TLegend *leg3_PHENIXoverlay = new TLegend(0.14,.19,0.34,.39);
    leg3_PHENIXoverlay->SetFillStyle(0);
    leg3_PHENIXoverlay->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg3_PHENIXoverlay->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg3_PHENIXoverlay->AddEntry("","40-60% Centrality","");
    leg3_PHENIXoverlay->Draw("same");
    
    TLegend *leg_uncertainty_40_60_forPHENIXoverlay = new TLegend(0.67,.19,0.87,.4);
    leg_uncertainty_40_60_forPHENIXoverlay->SetTextSize(0.029);
    leg_uncertainty_40_60_forPHENIXoverlay->AddEntry(graph_40_60_PHENIXdataAveraged, "PHENIX, 2010 Data", "pe");
    leg_uncertainty_40_60_forPHENIXoverlay->AddEntry(corrected_v2_40_60_graph_1, "Statistical Uncertainty", "pe"); // "lep" for line and marker
    leg_uncertainty_40_60_forPHENIXoverlay->AddEntry(sys_v2_40_60_graph_1, "Systematic Uncertainty", "pf"); // "f" for a filled object
    leg_uncertainty_40_60_forPHENIXoverlay->Draw("same");
    
    
    c_Overlay_ResultsWithPHENIX_40_60->SaveAs((BasePlotOutputPath + "/Final_v2_withSyst_40_60_OverlayedWithPHENIX.png").c_str());
    
    
    
    
    
    
    

    
    
    double barWidth = 0.05; // Width of each bar
    int colors[] = {kRed, kBlue, kGreen+1, kGray+1}; // Colors for the bars
    const double EPSILON = 1e-6; //used to hid a pT bin if want to zoom in and large uncertainty in a specific bin
    
    std::vector<TH1F*> histograms_0_20_RelativeUncertainty; // Vector to hold histogram pointers for legend
    
    TCanvas *c_Overlay_0_20_Systematics_RelativeUncertainty = new TCanvas("c_Overlay_0_20_Systematics_RelativeUncertainty", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    TH1F *frame_0_20_RelativeUncertainty = c_Overlay_0_20_Systematics_RelativeUncertainty->DrawFrame(2.0, 0, 5.0, 1.1);
    
    frame_0_20_RelativeUncertainty->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_0_20_RelativeUncertainty->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        /*
         UNCOMMENT LINE BELOW TO HIDE LARGE UNCERTAINTY IF WANT TO ZOOM IN
         */
//        if (std::abs(ptCenters[i] - 3.75) < EPSILON) {
//            continue;  // Skip the rest of the loop for this pT center
//        }
        double x = ptCenters[i] - (2.5 * barWidth); // Starting x position for the bars
        
        std::vector<double> values_RelativeUncertainty_0_20 = {
            data1.quad_sum_EMCal_syst_0_20[i],
            data1.signal_window_syst_0_20[i],
            data1.background_window_syst_0_20[i],
            data1.unWeighted_stat_uncertainties_0_20[i]
        };
        
        
        for (size_t j = 0; j < values_RelativeUncertainty_0_20.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_0_20_RelativeUncertainty = new TH1F(Form("h_0_20_RelativeUncertainty_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_0_20_RelativeUncertainty->SetBinContent(1, values_RelativeUncertainty_0_20[j]);
            h_0_20_RelativeUncertainty->SetFillColor(colors[j]);
            h_0_20_RelativeUncertainty->Draw("SAME");
            histograms_0_20_RelativeUncertainty.push_back(h_0_20_RelativeUncertainty); // Save the histogram pointer for the legend
        }
        
    }

    TLegend *legend_0_20_bars_RelativeUncertainty = new TLegend(0.58, 0.54, 0.78, 0.74);
    legend_0_20_bars_RelativeUncertainty->SetTextSize(0.027);
    legend_0_20_bars_RelativeUncertainty->SetHeader("Relative Systematic Uncertainties", "L");
    // Add histograms to legend using the pointers stored in the vector
    legend_0_20_bars_RelativeUncertainty->AddEntry(histograms_0_20_RelativeUncertainty[0], "EMCal Variations", "f");
    legend_0_20_bars_RelativeUncertainty->AddEntry(histograms_0_20_RelativeUncertainty[1], "Signal Window Variation", "f");
    legend_0_20_bars_RelativeUncertainty->AddEntry(histograms_0_20_RelativeUncertainty[2], "Background Window Variation", "f");
    legend_0_20_bars_RelativeUncertainty->AddEntry(histograms_0_20_RelativeUncertainty[3], "Final Systematic Uncertainty", "f");
    legend_0_20_bars_RelativeUncertainty->Draw();
    
    
    TLegend *leg0_20_overlay_RelativeUncertainty = new TLegend(0.52, 0.76, 0.72, 0.92);
    leg0_20_overlay_RelativeUncertainty->SetFillStyle(0);
    leg0_20_overlay_RelativeUncertainty->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg0_20_overlay_RelativeUncertainty->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg0_20_overlay_RelativeUncertainty->AddEntry("", "0-20% Centrality", "");
    leg0_20_overlay_RelativeUncertainty->Draw("same");
    c_Overlay_0_20_Systematics_RelativeUncertainty->Modified();
    c_Overlay_0_20_Systematics_RelativeUncertainty->Update();
    
    
    c_Overlay_0_20_Systematics_RelativeUncertainty->SaveAs((BasePlotOutputPath + "/RelativeSystUncertainty_Overlay_0_20.png").c_str());
    
    
    
    
    
    std::vector<TH1F*> histograms_20_40_RelativeUncertainty; // Vector to hold histogram pointers for legend
    
    TCanvas *c_Overlay_20_40_Systematics_RelativeUncertainty = new TCanvas("c_Overlay_20_40_Systematics_RelativeUncertainty", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    TH1F *frame_20_40_RelativeUncertainty = c_Overlay_20_40_Systematics_RelativeUncertainty->DrawFrame(2.0, 0, 5.0, 1.1);
    
    frame_20_40_RelativeUncertainty->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_20_40_RelativeUncertainty->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        /*
         UNCOMMENT LINE BELOW TO HIDE LARGE UNCERTAINTY IF WANT TO ZOOM IN
         */
//        if (std::abs(ptCenters[i] - 3.75) < EPSILON) {
//            continue;  // Skip the rest of the loop for this pT center
//        }
        double x = ptCenters[i] - (2.5 * barWidth); // Starting x position for the bars
        
        std::vector<double> values_RelativeUncertainty_20_40 = {
            data1.quad_sum_EMCal_syst_20_40[i],
            data1.signal_window_syst_20_40[i],
            data1.background_window_syst_20_40[i],
            data1.unWeighted_stat_uncertainties_20_40[i]
        };
        
        
        for (size_t j = 0; j < values_RelativeUncertainty_20_40.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_20_40_RelativeUncertainty = new TH1F(Form("h_20_40_RelativeUncertainty_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_20_40_RelativeUncertainty->SetBinContent(1, values_RelativeUncertainty_20_40[j]);
            h_20_40_RelativeUncertainty->SetFillColor(colors[j]);
            h_20_40_RelativeUncertainty->Draw("SAME");
            histograms_20_40_RelativeUncertainty.push_back(h_20_40_RelativeUncertainty); // Save the histogram pointer for the legend
        }
        
    }

    TLegend *legend_20_40_bars_RelativeUncertainty = new TLegend(0.58, 0.54, 0.78, 0.74);
    legend_20_40_bars_RelativeUncertainty->SetTextSize(0.027);
    legend_20_40_bars_RelativeUncertainty->SetHeader("Relative Systematic Uncertainties", "L");
    // Add histograms to legend using the pointers stored in the vector
    legend_20_40_bars_RelativeUncertainty->AddEntry(histograms_20_40_RelativeUncertainty[0], "EMCal Variations", "f");
    legend_20_40_bars_RelativeUncertainty->AddEntry(histograms_20_40_RelativeUncertainty[1], "Signal Window Variation", "f");
    legend_20_40_bars_RelativeUncertainty->AddEntry(histograms_20_40_RelativeUncertainty[2], "Background Window Variation", "f");
    legend_20_40_bars_RelativeUncertainty->AddEntry(histograms_20_40_RelativeUncertainty[3], "Final Systematic Uncertainty", "f");
    legend_20_40_bars_RelativeUncertainty->Draw();
    
    
    TLegend *leg20_40_overlay_RelativeUncertainty = new TLegend(0.52, 0.76, 0.72, 0.92);
    leg20_40_overlay_RelativeUncertainty->SetFillStyle(0);
    leg20_40_overlay_RelativeUncertainty->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg20_40_overlay_RelativeUncertainty->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg20_40_overlay_RelativeUncertainty->AddEntry("", "20-40% Centrality", "");
    leg20_40_overlay_RelativeUncertainty->Draw("same");
    c_Overlay_20_40_Systematics_RelativeUncertainty->Modified();
    c_Overlay_20_40_Systematics_RelativeUncertainty->Update();
    
    
    c_Overlay_20_40_Systematics_RelativeUncertainty->SaveAs((BasePlotOutputPath + "/RelativeSystUncertainty_Overlay_20_40.png").c_str());
    
    
    
    
    std::vector<TH1F*> histograms_40_60_RelativeUncertainty; // Vector to hold histogram pointers for legend
    
    TCanvas *c_Overlay_40_60_Systematics_RelativeUncertainty = new TCanvas("c_Overlay_40_60_Systematics_RelativeUncertainty", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    TH1F *frame_40_60_RelativeUncertainty = c_Overlay_40_60_Systematics_RelativeUncertainty->DrawFrame(2.0, 0, 5.0, 1.1);
    
    frame_40_60_RelativeUncertainty->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_40_60_RelativeUncertainty->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        /*
         UNCOMMENT LINE BELOW TO HIDE LARGE UNCERTAINTY IF WANT TO ZOOM IN
         */
//        if (std::abs(ptCenters[i] - 3.75) < EPSILON) {
//            continue;  // Skip the rest of the loop for this pT center
//        }
        double x = ptCenters[i] - (2.5 * barWidth); // Starting x position for the bars
        
        std::vector<double> values_RelativeUncertainty_40_60 = {
            data1.quad_sum_EMCal_syst_40_60[i],
            data1.signal_window_syst_40_60[i],
            data1.background_window_syst_40_60[i],
            data1.unWeighted_stat_uncertainties_40_60[i]
        };
        
        
        for (size_t j = 0; j < values_RelativeUncertainty_40_60.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_40_60_RelativeUncertainty = new TH1F(Form("h_40_60_RelativeUncertainty_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_40_60_RelativeUncertainty->SetBinContent(1, values_RelativeUncertainty_40_60[j]);
            h_40_60_RelativeUncertainty->SetFillColor(colors[j]);
            h_40_60_RelativeUncertainty->Draw("SAME");
            histograms_40_60_RelativeUncertainty.push_back(h_40_60_RelativeUncertainty); // Save the histogram pointer for the legend
        }
        
    }

    TLegend *legend_40_60_bars_RelativeUncertainty = new TLegend(0.58, 0.54, 0.78, 0.74);
    legend_40_60_bars_RelativeUncertainty->SetTextSize(0.027);
    legend_40_60_bars_RelativeUncertainty->SetHeader("Relative Systematic Uncertainties", "L");
    // Add histograms to legend using the pointers stored in the vector
    legend_40_60_bars_RelativeUncertainty->AddEntry(histograms_40_60_RelativeUncertainty[0], "EMCal Variations", "f");
    legend_40_60_bars_RelativeUncertainty->AddEntry(histograms_40_60_RelativeUncertainty[1], "Signal Window Variation", "f");
    legend_40_60_bars_RelativeUncertainty->AddEntry(histograms_40_60_RelativeUncertainty[2], "Background Window Variation", "f");
    legend_40_60_bars_RelativeUncertainty->AddEntry(histograms_40_60_RelativeUncertainty[3], "Final Systematic Uncertainty", "f");
    legend_40_60_bars_RelativeUncertainty->Draw();
    
    
    TLegend *leg40_60_overlay_RelativeUncertainty = new TLegend(0.52, 0.76, 0.72, 0.92);
    leg40_60_overlay_RelativeUncertainty->SetFillStyle(0);
    leg40_60_overlay_RelativeUncertainty->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg40_60_overlay_RelativeUncertainty->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg40_60_overlay_RelativeUncertainty->AddEntry("", "40-60% Centrality", "");
    leg40_60_overlay_RelativeUncertainty->Draw("same");
    c_Overlay_40_60_Systematics_RelativeUncertainty->Modified();
    c_Overlay_40_60_Systematics_RelativeUncertainty->Update();
    
    
    c_Overlay_40_60_Systematics_RelativeUncertainty->SaveAs((BasePlotOutputPath + "/RelativeSystUncertainty_Overlay_40_60.png").c_str());
    
    
    
    /*
     ABSOLUTE SYSTEMATIC UNCERTAINTIES PLOTTING
     */
    
    std::vector<TH1F*> histograms_0_20_AbsoluteUncertainty; // Vector to hold histogram pointers for legend
    
    TCanvas *c_Overlay_0_20_AbsoluteUncertainty = new TCanvas("c_Overlay_0_20_AbsoluteUncertainty", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    TH1F *frame_0_20_AbsoluteUncertainty = c_Overlay_0_20_AbsoluteUncertainty->DrawFrame(2.0, 0, 5.0, 1.1);
    
    frame_0_20_AbsoluteUncertainty->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_0_20_AbsoluteUncertainty->GetXaxis()->SetTitle("p_{T} [GeV]");

    for (size_t i = 0; i < ptCenters.size(); ++i) {
        double x = ptCenters[i] - (2.5 * barWidth); // Starting x position for the bars
        double v2_value = data1.corrected_v2_0_20[i];
        
        std::vector<double> values_AbsoluteSyst = {
            data1.quad_sum_EMCal_syst_0_20[i] * v2_value,
            data1.signal_window_syst_0_20[i] * v2_value,
            data1.background_window_syst_0_20[i] * v2_value,
            data1.stat_uncertainties_0_20[i]
        };
        
        for (size_t j = 0; j < values_AbsoluteSyst.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_0_20_AbsoluteSyst = new TH1F(Form("h_0_20_AbsoluteSyst_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_0_20_AbsoluteSyst->SetBinContent(1, values_AbsoluteSyst[j]);
            h_0_20_AbsoluteSyst->SetFillColor(colors[j]);
            h_0_20_AbsoluteSyst->Draw("SAME");
            histograms_0_20_AbsoluteUncertainty.push_back(h_0_20_AbsoluteSyst); // Save the histogram pointer for the legend
        }
        
    }

    TLegend *legend_0_20_bars_AbsoluteSyst = new TLegend(0.58, 0.54, 0.78, 0.74);
    legend_0_20_bars_AbsoluteSyst->SetTextSize(0.027);
    legend_0_20_bars_AbsoluteSyst->SetHeader("Absolute Systematic Uncertainties", "L");
    // Add histograms to legend using the pointers stored in the vector
    legend_0_20_bars_AbsoluteSyst->AddEntry(histograms_0_20_AbsoluteUncertainty[0], "EMCal Variations", "f");
    legend_0_20_bars_AbsoluteSyst->AddEntry(histograms_0_20_AbsoluteUncertainty[1], "Signal Window Variation", "f");
    legend_0_20_bars_AbsoluteSyst->AddEntry(histograms_0_20_AbsoluteUncertainty[2], "Background Window Variation", "f");
    legend_0_20_bars_AbsoluteSyst->AddEntry(histograms_0_20_AbsoluteUncertainty[3], "Final Systematic Uncertainty", "f");
    legend_0_20_bars_AbsoluteSyst->Draw();
    
    
    TLegend *leg0_20_overlay_AbsoluteSyst = new TLegend(0.52, 0.76, 0.72, 0.92);
    leg0_20_overlay_AbsoluteSyst->SetFillStyle(0);
    leg0_20_overlay_AbsoluteSyst->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg0_20_overlay_AbsoluteSyst->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg0_20_overlay_AbsoluteSyst->AddEntry("", "0-20% Centrality", "");
    leg0_20_overlay_AbsoluteSyst->Draw("same");
    c_Overlay_0_20_AbsoluteUncertainty->Modified();
    c_Overlay_0_20_AbsoluteUncertainty->Update();
    
    
    c_Overlay_0_20_AbsoluteUncertainty->SaveAs((BasePlotOutputPath + "/AbsoluteSystUncertainty_Overlay_0_20_v2.png").c_str());
    
    
    
    
    
    
    
    std::vector<TH1F*> histograms_20_40_AbsoluteUncertainty; // Vector to hold histogram pointers for legend
    
    TCanvas *c_Overlay_20_40_AbsoluteUncertainty = new TCanvas("c_Overlay_20_40_AbsoluteUncertainty", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    TH1F *frame_20_40_AbsoluteUncertainty = c_Overlay_20_40_AbsoluteUncertainty->DrawFrame(2.0, 0, 5.0, 1.1);
    
    frame_20_40_AbsoluteUncertainty->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_20_40_AbsoluteUncertainty->GetXaxis()->SetTitle("p_{T} [GeV]");

    for (size_t i = 0; i < ptCenters.size(); ++i) {
        double x = ptCenters[i] - (2.5 * barWidth); // Starting x position for the bars
        double v2_value = data1.corrected_v2_20_40[i];
        
        std::vector<double> values_AbsoluteSyst = {
            data1.quad_sum_EMCal_syst_20_40[i] * v2_value,
            data1.signal_window_syst_20_40[i] * v2_value,
            data1.background_window_syst_20_40[i] * v2_value,
            data1.stat_uncertainties_20_40[i]
        };
        
        for (size_t j = 0; j < values_AbsoluteSyst.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_20_40_AbsoluteSyst = new TH1F(Form("h_20_40_AbsoluteSyst_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_20_40_AbsoluteSyst->SetBinContent(1, values_AbsoluteSyst[j]);
            h_20_40_AbsoluteSyst->SetFillColor(colors[j]);
            h_20_40_AbsoluteSyst->Draw("SAME");
            histograms_20_40_AbsoluteUncertainty.push_back(h_20_40_AbsoluteSyst); // Save the histogram pointer for the legend
        }
        
    }

    TLegend *legend_20_40_bars_AbsoluteSyst = new TLegend(0.58, 0.54, 0.78, 0.74);
    legend_20_40_bars_AbsoluteSyst->SetTextSize(0.027);
    legend_20_40_bars_AbsoluteSyst->SetHeader("Absolute Systematic Uncertainties", "L");
    // Add histograms to legend using the pointers stored in the vector
    legend_20_40_bars_AbsoluteSyst->AddEntry(histograms_20_40_AbsoluteUncertainty[0], "EMCal Variations", "f");
    legend_20_40_bars_AbsoluteSyst->AddEntry(histograms_20_40_AbsoluteUncertainty[1], "Signal Window Variation", "f");
    legend_20_40_bars_AbsoluteSyst->AddEntry(histograms_20_40_AbsoluteUncertainty[2], "Background Window Variation", "f");
    legend_20_40_bars_AbsoluteSyst->AddEntry(histograms_20_40_AbsoluteUncertainty[3], "Final Systematic Uncertainty", "f");
    legend_20_40_bars_AbsoluteSyst->Draw();
    
    
    TLegend *leg20_40_overlay_AbsoluteSyst = new TLegend(0.52, 0.76, 0.72, 0.92);
    leg20_40_overlay_AbsoluteSyst->SetFillStyle(0);
    leg20_40_overlay_AbsoluteSyst->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg20_40_overlay_AbsoluteSyst->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg20_40_overlay_AbsoluteSyst->AddEntry("", "20-40% Centrality", "");
    leg20_40_overlay_AbsoluteSyst->Draw("same");
    c_Overlay_20_40_AbsoluteUncertainty->Modified();
    c_Overlay_20_40_AbsoluteUncertainty->Update();
    
    
    c_Overlay_20_40_AbsoluteUncertainty->SaveAs((BasePlotOutputPath + "/AbsoluteSystUncertainty_Overlay_20_40_v2.png").c_str());
    
    
    
    
    
    
    std::vector<TH1F*> histograms_40_60_AbsoluteUncertainty; // Vector to hold histogram pointers for legend
    
    TCanvas *c_Overlay_40_60_AbsoluteUncertainty = new TCanvas("c_Overlay_40_60_AbsoluteUncertainty", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    TH1F *frame_40_60_AbsoluteUncertainty = c_Overlay_40_60_AbsoluteUncertainty->DrawFrame(2.0, 0, 5.0, 1.1);
    
    frame_40_60_AbsoluteUncertainty->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_40_60_AbsoluteUncertainty->GetXaxis()->SetTitle("p_{T} [GeV]");

    for (size_t i = 0; i < ptCenters.size(); ++i) {
        double x = ptCenters[i] - (2.5 * barWidth); // Starting x position for the bars
        double v2_value = data1.corrected_v2_40_60[i];
        
        std::vector<double> values_AbsoluteSyst = {
            data1.quad_sum_EMCal_syst_40_60[i] * v2_value,
            data1.signal_window_syst_40_60[i] * v2_value,
            data1.background_window_syst_40_60[i] * v2_value,
            data1.stat_uncertainties_40_60[i]
        };
        
        for (size_t j = 0; j < values_AbsoluteSyst.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_40_60_AbsoluteSyst = new TH1F(Form("h_40_60_AbsoluteSyst_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_40_60_AbsoluteSyst->SetBinContent(1, values_AbsoluteSyst[j]);
            h_40_60_AbsoluteSyst->SetFillColor(colors[j]);
            h_40_60_AbsoluteSyst->Draw("SAME");
            histograms_40_60_AbsoluteUncertainty.push_back(h_40_60_AbsoluteSyst); // Save the histogram pointer for the legend
        }
        
    }

    TLegend *legend_40_60_bars_AbsoluteSyst = new TLegend(0.58, 0.54, 0.78, 0.74);
    legend_40_60_bars_AbsoluteSyst->SetTextSize(0.027);
    legend_40_60_bars_AbsoluteSyst->SetHeader("Absolute Systematic Uncertainties", "L");
    // Add histograms to legend using the pointers stored in the vector
    legend_40_60_bars_AbsoluteSyst->AddEntry(histograms_40_60_AbsoluteUncertainty[0], "EMCal Variations", "f");
    legend_40_60_bars_AbsoluteSyst->AddEntry(histograms_40_60_AbsoluteUncertainty[1], "Signal Window Variation", "f");
    legend_40_60_bars_AbsoluteSyst->AddEntry(histograms_40_60_AbsoluteUncertainty[2], "Background Window Variation", "f");
    legend_40_60_bars_AbsoluteSyst->AddEntry(histograms_40_60_AbsoluteUncertainty[3], "Final Systematic Uncertainty", "f");
    legend_40_60_bars_AbsoluteSyst->Draw();
    
    
    TLegend *leg40_60_overlay_AbsoluteSyst = new TLegend(0.52, 0.76, 0.72, 0.92);
    leg40_60_overlay_AbsoluteSyst->SetFillStyle(0);
    leg40_60_overlay_AbsoluteSyst->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg40_60_overlay_AbsoluteSyst->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg40_60_overlay_AbsoluteSyst->AddEntry("", "40-60% Centrality", "");
    leg40_60_overlay_AbsoluteSyst->Draw("same");
    c_Overlay_40_60_AbsoluteUncertainty->Modified();
    c_Overlay_40_60_AbsoluteUncertainty->Update();
    
    
    c_Overlay_40_60_AbsoluteUncertainty->SaveAs((BasePlotOutputPath + "/AbsoluteSystUncertainty_Overlay_40_60_v2.png").c_str());

    
    
}

void v2_Results_Plotter() {
    gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();
    
    std::string filePath1 = "/Users/patsfan753/Desktop/Default_Final_v2_p013.csv";

    Data data1;

    // Read the datasets
    Read_DataSet(filePath1, data1);
    
    std::string statUncertaintyFilePath = "/Users/patsfan753/Desktop/Systematics_Analysis/StatUncertaintyTable.csv";
    
    ReadStatUncertainties(statUncertaintyFilePath, data1);

    std::vector<double> ptCenters = {2.25, 2.75, 3.25, 3.75, 4.25, 4.75}; // Mid-points of pT ranges
    PrintStatisticalUncertainties(data1, ptCenters);
    
    plotting(data1);
}

