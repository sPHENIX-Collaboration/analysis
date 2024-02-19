#include <TFile.h>
#include <TH1F.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
/*
 Only set below bool to true if already calculated vN and outputted for one set of cuts to a CSV
 */
bool Plot_vN_bool = false; // Set this to true or false for vN plotting--IF TRUE, then vN will NOT BE re appended/calculated in the CSV being used

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
void printOverlayData(const std::vector<double>& ptCenters,
                      const std::vector<double>& data1, const std::vector<double>& error1,
                      const std::vector<double>& data2, const std::vector<double>& error2,
                      const std::vector<double>& data3, const std::vector<double>& error3) {
    std::cout << std::left << std::setw(10) << "pT" << std::setw(15) << "lowCen" << std::setw(15) << "lowCenErr"
              << std::setw(15) << "HighCen" << std::setw(15) << "HighCenErr"
              << std::setw(15) << "sPHEN" << std::setw(15) << "sPHENerr" << std::endl;
    std::cout << std::fixed << std::setprecision(5); // Apply fixed-point notation and set precision
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        std::cout << std::setw(10) << ptCenters[i]
                  << std::setw(15) << data1[i] << std::setw(15) << error1[i]
                  << std::setw(15) << data2[i] << std::setw(15) << error2[i]
                  << std::setw(15) << data3[i] << std::setw(15) << error3[i] << std::endl;
    }
}
// Call this function after you've populated your vectors
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
struct AccumulatedData {
    std::vector<double> corrected_v2_0_20_accumulated, corrected_v2_20_40_accumulated, corrected_v2_40_60_accumulated;
    std::vector<double> corrected_v2_0_20_Errors_accumulated, corrected_v2_20_40_Errors_accumulated, corrected_v2_40_60_Errors_accumulated;
    
    std::vector<double> corrected_v3_0_20_accumulated, corrected_v3_40_60_accumulated;
    std::vector<double> corrected_v3_0_20_Errors_accumulated, corrected_v3_40_60_Errors_accumulated;
};

void Read_Accumulated_CSV(const std::string& filePath, AccumulatedData& data) {
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
        
        double v2_corrected = std::stod(rowData.at(rowData.size() - 10));
        double v2_error_corrected = std::stod(rowData.at(rowData.size() - 9));
        double v3_corrected = std::stod(rowData.at(rowData.size() - 2));
        double v3_error_corrected = std::stod(rowData.back());

        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            data.corrected_v2_40_60_accumulated.push_back(v2_corrected);
            data.corrected_v2_40_60_Errors_accumulated.push_back(v2_error_corrected);

            
            data.corrected_v3_40_60_accumulated.push_back(v3_corrected);
            data.corrected_v3_40_60_Errors_accumulated.push_back(v3_error_corrected);

            
        } else if (index >= 6 && index <= 11) {
            data.corrected_v2_20_40_accumulated.push_back(v2_corrected);
            data.corrected_v2_20_40_Errors_accumulated.push_back(v2_error_corrected);

            

        } else if (index >= 12 && index <= 17) {
            data.corrected_v2_0_20_accumulated.push_back(v2_corrected);
            data.corrected_v2_0_20_Errors_accumulated.push_back(v2_error_corrected);

            
            data.corrected_v3_0_20_accumulated.push_back(v3_corrected);
            data.corrected_v3_0_20_Errors_accumulated.push_back(v3_error_corrected);
        }
    }
    file.close();
}



void Plot_vN(const AccumulatedData& data) {
    std::string filePath = "/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_RunByRun_Corrected_hists_data/mean_sigma_data.csv";
    std::string PlotOutputBasePath = "/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_RunByRun_Corrected_hists_data/vN_Final_RunByRunPlots/";

    std::ifstream file(filePath);
    std::string line;
    std::vector<double> ptCenters = {2.25, 2.75, 3.25, 3.75, 4.25, 4.75}; // Mid-points of pT ranges
    std::vector<double> corrected_v2_0_20, corrected_v2_20_40, corrected_v2_40_60;
    std::vector<double> corrected_v2_0_20_Errors, corrected_v2_20_40_Errors, corrected_v2_40_60_Errors;
    double v2_corrected, v2_error_corrected;
    
    
    std::vector<double> corrected_v3_0_20, corrected_v3_20_40, corrected_v3_40_60; // v3 values for different centralities
    std::vector<double> corrected_v3_0_20_Errors, corrected_v3_20_40_Errors, corrected_v3_40_60_Errors; // Errors for v3 values
    double v3_corrected, v3_error_corrected;

    // Skip the header line
    std::getline(file, line);

    // Reading the file
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> rowData;
        while (std::getline(ss, cell, ',')) {
            rowData.push_back(cell);
        }
        
        v2_corrected = std::stod(rowData.at(rowData.size() - 4));
        v2_error_corrected = std::stod(rowData.at(rowData.size() - 3));
        
        v3_corrected = std::stod(rowData.at(rowData.size() - 2)); // Second last column is v3 corrected
        v3_error_corrected = std::stod(rowData.back()); // Last column is v2 corrected error

        // Mapping index to centrality and pT
        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            corrected_v2_40_60.push_back(v2_corrected);
            corrected_v2_40_60_Errors.push_back(v2_error_corrected);

            corrected_v3_40_60.push_back(v3_corrected);
            corrected_v3_40_60_Errors.push_back(v3_error_corrected);

            
        } else if (index >= 6 && index <= 11) {
            corrected_v2_20_40.push_back(v2_corrected);
            corrected_v2_20_40_Errors.push_back(v2_error_corrected);

            corrected_v3_20_40.push_back(v3_corrected);
            corrected_v3_20_40_Errors.push_back(v3_error_corrected);

            
        } else if (index >= 12 && index <= 17) {
            corrected_v2_0_20.push_back(v2_corrected);
            corrected_v2_0_20_Errors.push_back(v2_error_corrected);

            corrected_v3_0_20.push_back(v3_corrected);
            corrected_v3_0_20_Errors.push_back(v3_error_corrected);

        }
        
    }

    // Close the file as we have finished reading the data
    file.close();
    PrintVectorContents(corrected_v2_40_60, corrected_v2_40_60_Errors, "corrected_v2_40_60");
    PrintVectorContents(corrected_v3_40_60, corrected_v3_40_60_Errors, "corrected_v3_40_60");
    PrintVectorContents(corrected_v2_20_40, corrected_v2_20_40_Errors, "corrected_v2_20_40");
    PrintVectorContents(corrected_v3_20_40, corrected_v3_20_40_Errors, "corrected_v3_20_40");
    PrintVectorContents(corrected_v2_0_20, corrected_v2_0_20_Errors, "corrected_v2_0_20");
    PrintVectorContents(corrected_v3_0_20, corrected_v3_0_20_Errors, "corrected_v3_0_20");
    /*
     Plot Corrected v2 By itself for 0-20 centrality
     */
    TCanvas *c_corrected_v2_0_20 = new TCanvas("c_corrected_v2_0_20", "Corrected v2 vs pT 0-20%", 800, 600);
    TGraphErrors *corrected_graph_0_20_v2 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &corrected_v2_0_20[0], 0, &corrected_v2_0_20_Errors[0]);
    corrected_graph_0_20_v2->SetMarkerColor(kBlue);
    corrected_graph_0_20_v2->SetMarkerStyle(21);
    corrected_graph_0_20_v2->SetMarkerSize(1.1);
    corrected_graph_0_20_v2->SetTitle("Corrected #it{v}_{2} vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    corrected_graph_0_20_v2->Draw("AP");
    corrected_graph_0_20_v2->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_graph_0_20_v2->GetXaxis()->SetNdivisions(010);
    corrected_graph_0_20_v2->SetMinimum(-1.35); // Set the minimum y value
    corrected_graph_0_20_v2->SetMaximum(2.5); // Set the maximum y value
    c_corrected_v2_0_20->SetTicks();
    TLatex latex020_correct;
    latex020_correct.SetTextSize(0.03);
    latex020_correct.SetTextAlign(12);
    latex020_correct.SetNDC();
    latex020_correct.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex020_correct.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex020_correct.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex020_correct.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    
    c_corrected_v2_0_20->Update();
    // Draw a dashed line at y = 0
    double x_min_0_20_corr_v2 = c_corrected_v2_0_20->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_0_20_corr_v2 = c_corrected_v2_0_20->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_0_20_corr_v2 = new TLine(x_min_0_20_corr_v2, 0, x_max_0_20_corr_v2, 0);
    zeroLine_0_20_corr_v2->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_0_20_corr_v2->Draw("SAME"); // Draw the line on the same canvas as your plot

    
    c_corrected_v2_0_20->Modified();
    c_corrected_v2_0_20->Update();
    c_corrected_v2_0_20->SaveAs((PlotOutputBasePath + "v2/0_20/Corrected_v2_vs_pT_0_20_correctedPointsOnly.png").c_str());


    
    /*
     Plot Corrected v2 By itself for 20-40 centrality
     */

    TCanvas *c_corrected_v2_20_40 = new TCanvas("c_corrected_v2_20_40", "Corrected v2 vs pT 20-40%", 800, 600);
    TGraphErrors *corrected_graph_20_40_v2 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &corrected_v2_20_40[0], 0, &corrected_v2_20_40_Errors[0]);
    corrected_graph_20_40_v2->SetMarkerColor(kBlue);
    corrected_graph_20_40_v2->SetMarkerStyle(21);
    corrected_graph_20_40_v2->SetMarkerSize(1.1);
    corrected_graph_20_40_v2->SetTitle("Corrected #it{v}_{2} vs #it{p}_{T} 20-40% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    corrected_graph_20_40_v2->Draw("AP");
    corrected_graph_20_40_v2->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_graph_20_40_v2->GetXaxis()->SetNdivisions(010);
    corrected_graph_20_40_v2->SetMinimum(-0.8); // Set the minimum y value
    corrected_graph_20_40_v2->SetMaximum(1.0); // Set the maximum y value
    c_corrected_v2_20_40->SetTicks();
    TLatex latex2040_corr;
    latex2040_corr.SetTextSize(0.03);
    latex2040_corr.SetTextAlign(12);
    latex2040_corr.SetNDC();
    latex2040_corr.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex2040_corr.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex2040_corr.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex2040_corr.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    
    c_corrected_v2_20_40->Update();
    // Draw a dashed line at y = 0
    double x_min_20_40_corr_v2 = c_corrected_v2_20_40->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_20_40_corr_v2 = c_corrected_v2_20_40->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_20_40_corr_v2 = new TLine(x_min_20_40_corr_v2, 0, x_max_20_40_corr_v2, 0);
    zeroLine_20_40_corr_v2->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_20_40_corr_v2->Draw("SAME"); // Draw the line on the same canvas as your plot

    c_corrected_v2_20_40->Modified();
    c_corrected_v2_20_40->Update();
    c_corrected_v2_20_40->SaveAs((PlotOutputBasePath + "v2/20_40/Corrected_v2_vs_pT_20_40_correctedPointsOnly.png").c_str());

    /*
     Plot Corrected v2 By itself for 40-60 centrality
     */
    TCanvas *c_corrected_v2_40_60 = new TCanvas("c_corrected_v2_40_60", "Corrected v2 vs pT 40-60%", 800, 600);
    TGraphErrors *corrected_graph_40_60_v2 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &corrected_v2_40_60[0], 0, &corrected_v2_40_60_Errors[0]);
    corrected_graph_40_60_v2->SetMarkerColor(kBlue);
    corrected_graph_40_60_v2->SetMarkerStyle(21);
    corrected_graph_40_60_v2->SetMarkerSize(1.1);
    corrected_graph_40_60_v2->SetTitle("Corrected #it{v}_{2} vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    corrected_graph_40_60_v2->Draw("AP");
    corrected_graph_40_60_v2->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_graph_40_60_v2->GetXaxis()->SetNdivisions(010);

    corrected_graph_40_60_v2->SetMinimum(-0.8); // Set the minimum y value
    corrected_graph_40_60_v2->SetMaximum(1.8); // Set the maximum y value
    c_corrected_v2_40_60->SetTicks();
    TLatex latex4060_corr;
    latex4060_corr.SetTextSize(0.03);
    latex4060_corr.SetTextAlign(12);
    latex4060_corr.SetNDC();
    latex4060_corr.DrawLatex(0.3, 0.86, "Cuts (Inclusive):");
    latex4060_corr.DrawLatex(0.3, 0.82, "#bf{Asymmetry < 0.5}");
    latex4060_corr.DrawLatex(0.3, 0.78, "#bf{#chi^{2} < 4.0}");
    latex4060_corr.DrawLatex(0.3, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    
    c_corrected_v2_40_60->Update();
    // Draw a dashed line at y = 0
    double x_min_40_60_corr_v2 = c_corrected_v2_40_60->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_40_60_corr_v2 = c_corrected_v2_40_60->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_40_60_corr_v2 = new TLine(x_min_40_60_corr_v2, 0, x_max_40_60_corr_v2, 0);
    zeroLine_40_60_corr_v2->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_40_60_corr_v2->Draw("SAME"); // Draw the line on the same canvas as your plot
    
    c_corrected_v2_40_60->Modified();
    c_corrected_v2_40_60->Update();
    c_corrected_v2_40_60->SaveAs((PlotOutputBasePath + "v2/40_60/Corrected_v2_vs_pT_40_60_corrected.png").c_str());
    
    
    
    // Define the offsets for jitter
    double offset = 0.06;
    std::string phenixFilePath = "/Users/patsfan753/Desktop/FinalCleanedPhenix.csv";
    std::vector<double> v2_0_10, v2_10_20, v2_20_30, v2_30_40, v2_40_50, v2_50_60;
    std::vector<double> v2_0_10_Errors, v2_10_20_Errors, v2_20_30_Errors, v2_30_40_Errors, v2_40_50_Errors, v2_50_60_Errors;
    std::vector<double> v2_0_10_Errors_Negative, v2_10_20_Errors_Negative, v2_20_30_Errors_Negative, v2_30_40_Errors_Negative, v2_40_50_Errors_Negative, v2_50_60_Errors_Negative;

    // Read in the data and errors
    ReadPHENIXData(phenixFilePath, v2_0_10, v2_0_10_Errors, v2_0_10_Errors_Negative,
                   v2_10_20, v2_10_20_Errors, v2_10_20_Errors_Negative,
                   v2_20_30, v2_20_30_Errors, v2_20_30_Errors_Negative,
                   v2_30_40, v2_30_40_Errors, v2_30_40_Errors_Negative,
                   v2_40_50, v2_40_50_Errors, v2_40_50_Errors_Negative,
                   v2_50_60, v2_50_60_Errors, v2_50_60_Errors_Negative);

    int markerStyle_sPHENIX = 21;
    int markerStyle_PHENIX = 20;

    int color_sPHENIX = kBlue;
    int color_PHENIX_low = kRed;
    int color_PHENIX_high= kGreen+3;
    /*
     Overlay Corrected v2 data for 0-20 percent centrality with the PHENIX DATA
     */
    TCanvas *c_corrected_v2_overlay0_20 = new TCanvas("c_corrected_v2_overlay0_20", "Corrected Overlay 0-20%", 800, 600);
    TGraphErrors *graph_0_10 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v2_0_10[0], 0, &v2_10_20_Errors[0]);
    TGraphErrors *graph_10_20 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v2_0_10[0], 0, &v2_10_20_Errors[0]);
    corrected_graph_0_20_v2->Draw("AP");  // This will be the base graph

    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_0_10->SetPoint(i, ptCenters[i] - offset, v2_0_10[i]);
        graph_10_20->SetPoint(i, ptCenters[i] + offset, v2_10_20[i]);
    }

    corrected_graph_0_20_v2->SetMarkerColor(color_sPHENIX);
    corrected_graph_0_20_v2->SetMarkerStyle(markerStyle_sPHENIX);
    corrected_graph_0_20_v2->SetMinimum(-0.8); // Set the minimum y value
    corrected_graph_0_20_v2->SetMaximum(1.0); // Set the maximum y value

    graph_0_10->SetMarkerColor(color_PHENIX_low);
    graph_0_10->SetMarkerStyle(markerStyle_PHENIX);
    graph_10_20->SetMarkerColor(color_PHENIX_high);
    graph_10_20->SetMarkerStyle(markerStyle_PHENIX);

    graph_0_10->Draw("P SAME");
    graph_10_20->Draw("P SAME");

    printOverlayData(ptCenters,
                     v2_0_10, v2_0_10_Errors,
                     v2_10_20, v2_10_20_Errors,
                     corrected_v2_0_20, corrected_v2_0_20_Errors);
    TLegend *legend4 = new TLegend(0.11, 0.69, 0.31, 0.89);
    legend4->SetBorderSize(0);
    legend4->SetTextSize(0.028);
    legend4->AddEntry(corrected_graph_0_20_v2, "0-20%, #bf{sPHENIX}", "p");
    legend4->AddEntry(graph_0_10, "0-10%, #bf{PHENIX} 2010", "p");
    legend4->AddEntry(graph_10_20, "10-20%, #bf{PHENIX} 2010", "p");
    legend4->Draw();
    
    c_corrected_v2_overlay0_20->Update();
    // Draw a dashed line at y = 0
    double x_min_corrected_v2_overlay0_20 = c_corrected_v2_overlay0_20->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_corrected_v2_overlay0_20 = c_corrected_v2_overlay0_20->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_corrected_v2_overlay0_20 = new TLine(x_min_corrected_v2_overlay0_20, 0, x_max_corrected_v2_overlay0_20, 0);
    zeroLine_corrected_v2_overlay0_20->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_corrected_v2_overlay0_20->Draw("SAME"); // Draw the line on the same canvas as your plot
    
    c_corrected_v2_overlay0_20->Modified();
    c_corrected_v2_overlay0_20->Update();
    c_corrected_v2_overlay0_20->SaveAs((PlotOutputBasePath + "v2/0_20/v2_vs_pT_0_20_Overlay_corrected.png").c_str());

    /*
     Overlay Just the Corrected v2 data for 20-40 percent centrality with the PHENIX DATA
     */
    TCanvas *c_corrected_v2_overlay20_40 = new TCanvas("c14", "Corrected Overlay 20-40%", 800, 600);
    TGraphErrors *graph_20_30 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v2_20_30[0], 0, &v2_20_30_Errors[0]);
    TGraphErrors *graph_30_40 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v2_30_40[0], 0, &v2_30_40_Errors[0]);
    corrected_graph_20_40_v2->Draw("AP");  // This will be the base graph
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_20_30->SetPoint(i, ptCenters[i] - offset, v2_20_30[i]);
        graph_30_40->SetPoint(i, ptCenters[i] + offset, v2_30_40[i]);
    }
    corrected_graph_20_40_v2->SetMinimum(-0.45); // Set the minimum y value
    corrected_graph_20_40_v2->SetMaximum(0.8); // Set the maximum y value
    corrected_graph_20_40_v2->SetMarkerColor(color_sPHENIX);
    corrected_graph_20_40_v2->SetMarkerStyle(markerStyle_sPHENIX);
    graph_20_30->SetMarkerColor(color_PHENIX_low);
    graph_20_30->SetMarkerStyle(markerStyle_PHENIX);
    graph_30_40->SetMarkerColor(color_PHENIX_high);
    graph_30_40->SetMarkerStyle(markerStyle_PHENIX);

    graph_20_30->Draw("P SAME");
    graph_30_40->Draw("P SAME");
    
    printOverlayData(ptCenters,
                     v2_20_30, v2_20_30_Errors,
                     v2_30_40, v2_30_40_Errors,
                     corrected_v2_20_40, corrected_v2_20_40_Errors);

    // Create a legend for the first overlay plot
    TLegend *legend5 = new TLegend(0.11, 0.69, 0.31, 0.89);
    legend5->SetBorderSize(0);
    legend5->SetTextSize(0.028);
    legend5->AddEntry(corrected_graph_20_40_v2, "20-40%, #bf{sPHENIX}", "p");
    legend5->AddEntry(graph_20_30, "20-30%, #bf{PHENIX} 2010", "p");
    legend5->AddEntry(graph_30_40, "30-40%, #bf{PHENIX} 2010", "p");
    legend5->Draw();
    
    c_corrected_v2_overlay20_40->Update();
    // Draw a dashed line at y = 0
    double x_min_corrected_v2_overlay20_40 = c_corrected_v2_overlay20_40->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_corrected_v2_overlay20_40 = c_corrected_v2_overlay20_40->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_corrected_v2_overlay20_40 = new TLine(x_min_corrected_v2_overlay20_40, 0, x_max_corrected_v2_overlay20_40, 0);
    zeroLine_corrected_v2_overlay20_40->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_corrected_v2_overlay20_40->Draw("SAME"); // Draw the line on the same canvas as your plot

    c_corrected_v2_overlay20_40->Modified();
    c_corrected_v2_overlay20_40->Update();
    c_corrected_v2_overlay20_40->SaveAs((PlotOutputBasePath + "v2/20_40/v2_vs_pT_20_40_Overlay_corrected.png").c_str());
    
    
    
    

    /*
     Overlay Just the Corrected v2 data for 40-60 percent centrality with the PHENIX DATA
     */
    TCanvas *c_corrected_v2_overlay40_60 = new TCanvas("c15", "Corrected Overlay 40-60%", 800, 600);
    TGraphErrors *graph_40_50 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v2_40_50[0], 0, &v2_40_50_Errors[0]);
    TGraphErrors *graph_50_60 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v2_50_60[0], 0, &v2_50_60_Errors[0]);
    corrected_graph_40_60_v2->Draw("AP");  // This will be the base graph

    
    // Apply jitter to x values for graph_40_50 and graph_50_60
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_40_50->SetPoint(i, ptCenters[i] - offset, v2_40_50[i]);
        graph_50_60->SetPoint(i, ptCenters[i] + offset, v2_50_60[i]);
    }
    corrected_graph_40_60_v2->SetMinimum(-0.8); // Set the minimum y value
    corrected_graph_40_60_v2->SetMaximum(1.8); // Set the maximum y value
    // Adjust the color and style for the 40-60% plot
    corrected_graph_40_60_v2->SetMarkerColor(color_sPHENIX);
    corrected_graph_40_60_v2->SetMarkerStyle(markerStyle_sPHENIX); // Star marker style for 40-60%

    // Set the styles and colors for the additional graphs
    graph_40_50->SetMarkerColor(color_PHENIX_low);
    graph_40_50->SetMarkerStyle(markerStyle_PHENIX);
    graph_50_60->SetMarkerColor(color_PHENIX_high);
    graph_50_60->SetMarkerStyle(markerStyle_PHENIX);

    // Draw the additional graphs with the new styles and colors
    graph_40_50->Draw("P SAME");
    graph_50_60->Draw("P SAME");

    printOverlayData(ptCenters,
                     v2_40_50, v2_40_50_Errors,
                     v2_50_60, v2_50_60_Errors,
                     corrected_v2_40_60, corrected_v2_40_60_Errors);

    TLegend *legend6 = new TLegend(0.11, 0.69, 0.31, 0.89);
    legend6->SetBorderSize(0);
    legend6->SetTextSize(0.028);
    legend6->AddEntry(corrected_graph_40_60_v2, "40-60%, #bf{sPHENIX}", "p");
    legend6->AddEntry(graph_40_50, "40-50%, #bf{PHENIX} 2010", "p");
    legend6->AddEntry(graph_50_60, "50-60%, #bf{PHENIX} 2010", "p");
    legend6->Draw();
    c_corrected_v2_overlay40_60->Update();
    // Draw a dashed line at y = 0
    double x_min_corrected_v2_overlay40_60 = c_corrected_v2_overlay40_60->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_corrected_v2_overlay40_60 = c_corrected_v2_overlay40_60->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_corrected_v2_overlay40_60 = new TLine(x_min_corrected_v2_overlay40_60, 0, x_max_corrected_v2_overlay40_60, 0);
    zeroLine_corrected_v2_overlay40_60->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_corrected_v2_overlay40_60->Draw("SAME"); // Draw the line on the same canvas as your plot
    c_corrected_v2_overlay40_60->Modified();
    c_corrected_v2_overlay40_60->Update();
    c_corrected_v2_overlay40_60->SaveAs((PlotOutputBasePath + "v2/40_60/v2_vs_pT_40_60_Overlay_corrected.png").c_str());

    
    /*
     Plot corrected v3 data for 0-20 percent centrality
     */
    TCanvas *c_corrected_v3_0_20 = new TCanvas("c_corrected_v3_0_20", "Corrected v3 vs pT 0-20%", 800, 600);
    TGraphErrors *corrected_graph_0_20_v3 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &corrected_v3_0_20[0], 0, &corrected_v3_0_20_Errors[0]);
    corrected_graph_0_20_v3->SetMarkerColor(kBlue);
    corrected_graph_0_20_v3->SetMarkerStyle(21);
    corrected_graph_0_20_v3->SetMarkerSize(1.1);
    corrected_graph_0_20_v3->SetTitle("Corrected #it{v}_{3} vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    corrected_graph_0_20_v3->Draw("AP");
    corrected_graph_0_20_v3->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_graph_0_20_v3->GetXaxis()->SetNdivisions(010);
    corrected_graph_0_20_v3->SetMinimum(-2); // Set the minimum y value
    corrected_graph_0_20_v3->SetMaximum(2); // Set the maximum y value
    c_corrected_v3_0_20->SetTicks();
    //Output cut information
    TLatex latex020_correct_v3;
    latex020_correct_v3.SetTextSize(0.03);
    latex020_correct_v3.SetTextAlign(12);
    latex020_correct_v3.SetNDC();
    latex020_correct_v3.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex020_correct_v3.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex020_correct_v3.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex020_correct_v3.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    c_corrected_v3_0_20->Update();
    // Draw a dashed line at y = 0
    double x_min_0_20_v3 = c_corrected_v3_0_20->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_0_20_v3 = c_corrected_v3_0_20->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_corrected_v3_0_20 = new TLine(x_min_0_20_v3, 0, x_max_0_20_v3, 0);
    zeroLine_corrected_v3_0_20->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_corrected_v3_0_20->Draw("SAME"); // Draw the line on the same canvas as your plot
    c_corrected_v3_0_20->Modified();
    c_corrected_v3_0_20->Update();
    c_corrected_v3_0_20->SaveAs((PlotOutputBasePath + "v3/0_20/v3_vs_pT_0_20_corrected.png").c_str());

    /*
     Plot corrected v3 data for 20-40 percent centrality
     */
    TCanvas *c_corrected_v3_20_40 = new TCanvas("c_corrected_v3_20_40", "Corrected v3 vs pT 20-40%", 800, 600);
    TGraphErrors *corrected_graph_20_40_v3 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &corrected_v3_20_40[0], 0, &corrected_v3_20_40_Errors[0]);
    corrected_graph_20_40_v3->SetMarkerColor(kBlue);
    corrected_graph_20_40_v3->SetMarkerStyle(21);
    corrected_graph_20_40_v3->SetMarkerSize(1.1);
    corrected_graph_20_40_v3->SetTitle("Corrected #it{v}_{3} vs #it{p}_{T} 20-40% Centrality; #it{p}_{T} [GeV]; #it{v}_{3}");
    corrected_graph_20_40_v3->Draw("AP");
    corrected_graph_20_40_v3->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_graph_20_40_v3->GetXaxis()->SetNdivisions(010);
    corrected_graph_20_40_v3->SetMinimum(-2); // Set the minimum y value
    corrected_graph_20_40_v3->SetMaximum(2); // Set the maximum y value
    c_corrected_v3_20_40->SetTicks();
    //Output cut information
    TLatex latex2040_correct_v3;
    latex2040_correct_v3.SetTextSize(0.03);
    latex2040_correct_v3.SetTextAlign(12);
    latex2040_correct_v3.SetNDC();
    latex2040_correct_v3.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex2040_correct_v3.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex2040_correct_v3.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex2040_correct_v3.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    
    c_corrected_v3_20_40->Update();
    // Draw a dashed line at y = 0
    double x_min_20_40_v3 = c_corrected_v3_20_40->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_20_40_v3 = c_corrected_v3_20_40->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_corrected_v3_20_40 = new TLine(x_min_20_40_v3, 0, x_max_20_40_v3, 0);
    zeroLine_corrected_v3_20_40->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_corrected_v3_20_40->Draw("SAME"); // Draw the line on the same canvas as your plot
    
    c_corrected_v3_20_40->Modified();
    c_corrected_v3_20_40->Update();
    c_corrected_v3_20_40->SaveAs((PlotOutputBasePath + "v3/20_40/v3_vs_pT_20_40_corrected.png").c_str());

    
    /*
     Plot corrected v3 data for 40-60 percent centrality
     */
    TCanvas *c_corrected_v3_40_60 = new TCanvas("c_corrected_v3_40_60", "Corrected v3 vs pT 40-60%", 800, 600);
    TGraphErrors *corrected_graph_40_60_v3 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &corrected_v3_40_60[0], 0, &corrected_v3_40_60_Errors[0]);
    corrected_graph_40_60_v3->SetMarkerColor(kBlue);
    corrected_graph_40_60_v3->SetMarkerStyle(21);
    corrected_graph_40_60_v3->SetMarkerSize(1.1);
    corrected_graph_40_60_v3->SetTitle("Corrected #it{v}_{3} vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    corrected_graph_40_60_v3->Draw("AP");
    corrected_graph_40_60_v3->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_graph_40_60_v3->GetXaxis()->SetNdivisions(010);
    corrected_graph_40_60_v3->SetMinimum(0.0); // Set the minimum y value
    corrected_graph_40_60_v3->SetMaximum(0.25); // Set the maximum y value
    corrected_graph_40_60_v3->SetMinimum(-2); // Set the minimum y value
    corrected_graph_40_60_v3->SetMaximum(2); // Set the maximum y value

    c_corrected_v3_40_60->SetTicks();
    //Output cut information
    TLatex latex4060_corr_v3;
    latex4060_corr_v3.SetTextSize(0.03);
    latex4060_corr_v3.SetTextAlign(12);
    latex4060_corr_v3.SetNDC();
    latex4060_corr_v3.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex4060_corr_v3.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex4060_corr_v3.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex4060_corr_v3.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    
    c_corrected_v3_40_60->Update();
    // Draw a dashed line at y = 0
    double x_min_40_60_v3 = c_corrected_v3_40_60->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_40_60_v3 = c_corrected_v3_40_60->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_corrected_v3_40_60 = new TLine(x_min_40_60_v3, 0, x_max_40_60_v3, 0);
    zeroLine_corrected_v3_40_60->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_corrected_v3_40_60->Draw("SAME"); // Draw the line on the same canvas as your plot
    
    c_corrected_v3_40_60->Modified();
    c_corrected_v3_40_60->Update();
    c_corrected_v3_40_60->SaveAs((PlotOutputBasePath + "v3/40_60/v3_vs_pT_40_60_corrected.png").c_str());
    
    
    /*
     Overlay for v2 v3 Corrected Output
     */
    TCanvas *c_0_20_v2_v3_corrected = new TCanvas("c_0_20_v2_v3_corrected", "Corrected #v_2, #v_3 Overlay, Corrected, 0-20% Centrality", 800, 600);
    corrected_graph_0_20_v2->Draw("AP");  // This will be the base graph
    corrected_graph_0_20_v2->SetTitle("#pi^{0} #it{v}_{2} and #it{v}_{3} Overlay vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{N}");
    corrected_graph_0_20_v2->SetMinimum(-2.0); // Set the minimum y value
    corrected_graph_0_20_v2->SetMaximum(2.0); // Set the maximum y value
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_graph_0_20_v2->SetPoint(i, ptCenters[i] - offset, corrected_v2_0_20[i]);
    }
    corrected_graph_0_20_v3->SetMarkerStyle(20); // circle
    corrected_graph_0_20_v3->SetMarkerColor(kRed);
    corrected_graph_0_20_v3->SetLineColor(kRed);
    corrected_graph_0_20_v3 -> Draw("P SAME");
    corrected_graph_0_20_v2->SetMarkerColor(kBlue);
    corrected_graph_0_20_v2->SetMarkerStyle(21);
    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend_0_20_v2_v3_corrected = new TLegend(0.11, 0.11, 0.31, 0.31);
    legend_0_20_v2_v3_corrected->SetBorderSize(0);
    legend_0_20_v2_v3_corrected->SetTextSize(0.03);
    legend_0_20_v2_v3_corrected->AddEntry(corrected_graph_0_20_v2, "0-20%, v_{2}^{#pi^{0}}", "p");
    legend_0_20_v2_v3_corrected->AddEntry(corrected_graph_0_20_v3, "0-20%, v_{3}^{#pi^{0}}", "p");
    legend_0_20_v2_v3_corrected->Draw();
    c_0_20_v2_v3_corrected->Update();
    // Draw a dashed line at y = 0
    double x_min_v2_v3_corrected_0_20 = c_0_20_v2_v3_corrected->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_v2_v3_corrected_0_20 = c_0_20_v2_v3_corrected->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_v2_v3_corrected_0_20 = new TLine(x_min_v2_v3_corrected_0_20, 0, x_max_v2_v3_corrected_0_20, 0);
    zeroLine_v2_v3_corrected_0_20->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_v2_v3_corrected_0_20->Draw("SAME"); // Draw the line on the same canvas as your plot
    c_0_20_v2_v3_corrected->Modified();
    c_0_20_v2_v3_corrected->Update();
    c_0_20_v2_v3_corrected->SaveAs((PlotOutputBasePath + "v2_v3_Corrected_Overlay_0_20.png").c_str());
    
    
    TCanvas *c_20_40_v2_v3_corrected = new TCanvas("c_20_40_v2_v3_corrected", "Corrected #v_2, #v_3 Overlay, Corrected, 20-40% Centrality", 800, 600);
    corrected_graph_20_40_v2->Draw("AP");  // This will be the base graph
    corrected_graph_20_40_v2->SetTitle("#pi^{0} #it{v}_{2} and #it{v}_{3} Overlay vs #it{p}_{T} 20-40% Centrality; #it{p}_{T} [GeV]; #it{v}_{N}");
    corrected_graph_20_40_v2->SetMinimum(-2.0); // Set the minimum y value
    corrected_graph_20_40_v2->SetMaximum(2.0); // Set the maximum y value
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_graph_20_40_v2->SetPoint(i, ptCenters[i] - offset, corrected_v2_20_40[i]);
    }
    
    corrected_graph_20_40_v3->SetMarkerStyle(20); // circle
    corrected_graph_20_40_v3->SetMarkerColor(kRed);
    corrected_graph_20_40_v3->SetLineColor(kRed);
    
    corrected_graph_20_40_v3 -> Draw("P SAME");

    corrected_graph_20_40_v2->SetMarkerColor(kBlue);
    corrected_graph_20_40_v2->SetMarkerStyle(21);

    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend_20_40_v2_v3_corrected = new TLegend(0.11, 0.11, 0.31, 0.31);
    legend_20_40_v2_v3_corrected->SetBorderSize(0);
    legend_20_40_v2_v3_corrected->SetTextSize(0.03);
    legend_20_40_v2_v3_corrected->AddEntry(corrected_graph_20_40_v2, "20-40%, v_{2}^{#pi^{0}}", "p");
    legend_20_40_v2_v3_corrected->AddEntry(corrected_graph_20_40_v3, "20-40%, v_{3}^{#pi^{0}}", "p");

    legend_20_40_v2_v3_corrected->Draw();
    
    c_20_40_v2_v3_corrected->Update();
    // Draw a dashed line at y = 0
    double x_min_v2_v3_corrected_20_40 = c_20_40_v2_v3_corrected->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_v2_v3_corrected_20_40 = c_20_40_v2_v3_corrected->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_v2_v3_corrected_20_40 = new TLine(x_min_v2_v3_corrected_20_40, 0, x_max_v2_v3_corrected_20_40, 0);
    zeroLine_v2_v3_corrected_20_40->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_v2_v3_corrected_20_40->Draw("SAME"); // Draw the line on the same canvas as your plot

    c_20_40_v2_v3_corrected->Modified();
    c_20_40_v2_v3_corrected->Update();
    c_20_40_v2_v3_corrected->SaveAs((PlotOutputBasePath + "v2_v3_Corrected_Overlay_20_40.png").c_str());
    
    
    /*
     Make plot corrected v2 overlay with v3 for 40-60 centrality
     */
    TCanvas *c_40_60_v2_v3_corrected = new TCanvas("c_40_60_v2_v3_corrected", "Measured #v_2, #v_3 Overlay, Corrected, 40-60% Centrality", 800, 600);
    corrected_graph_40_60_v2->Draw("AP");  // This will be the base graph
    corrected_graph_40_60_v2->SetTitle("#pi^{0} #it{v}_{2} and #it{v}_{3} Overlay vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{N}");
    corrected_graph_40_60_v2->SetMinimum(-2.0); // Set the minimum y value
    corrected_graph_40_60_v2->SetMaximum(2.0); // Set the maximum y value
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_graph_40_60_v2->SetPoint(i, ptCenters[i] - offset, corrected_v2_40_60[i]);
    }
    corrected_graph_40_60_v3->SetMarkerStyle(20); // circle
    corrected_graph_40_60_v3->SetMarkerColor(kRed);
    corrected_graph_40_60_v3->SetLineColor(kRed);
    corrected_graph_40_60_v3 -> Draw("P SAME");
    corrected_graph_40_60_v2->SetMarkerColor(kBlue);
    corrected_graph_40_60_v2->SetMarkerStyle(21);
    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend_40_60_v2_v3_corrected = new TLegend(0.11, 0.11, 0.31, 0.31);
    legend_40_60_v2_v3_corrected->SetBorderSize(0);
    legend_40_60_v2_v3_corrected->SetTextSize(0.03);
    legend_40_60_v2_v3_corrected->AddEntry(corrected_graph_40_60_v2, "40-60%, v_{2}^{#pi^{0}}", "p");
    legend_40_60_v2_v3_corrected->AddEntry(corrected_graph_40_60_v3, "40-60%, v_{3}^{#pi^{0}}", "p");
    legend_40_60_v2_v3_corrected->Draw();
    c_40_60_v2_v3_corrected->Update();
    // Draw a dashed line at y = 0
    double x_min_v2_v3_corrected_40_60 = c_40_60_v2_v3_corrected->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_v2_v3_corrected_40_60 = c_40_60_v2_v3_corrected->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_v2_v3_corrected_40_60 = new TLine(x_min_v2_v3_corrected_40_60, 0, x_max_v2_v3_corrected_40_60, 0);
    zeroLine_v2_v3_corrected_40_60->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_v2_v3_corrected_40_60->Draw("SAME"); // Draw the line on the same canvas as your plot
    c_40_60_v2_v3_corrected->Modified();
    c_40_60_v2_v3_corrected->Update();
    c_40_60_v2_v3_corrected->SaveAs((PlotOutputBasePath + "v2_v3_Corrected_Overlay_40_60.png").c_str());
    
    /*
     set constants to the accumulated variables set in the struct
     */
    
    const auto& corrected_v2_0_20_accumulated = data.corrected_v2_0_20_accumulated;
    const auto& corrected_v2_20_40_accumulated = data.corrected_v2_20_40_accumulated;
    const auto& corrected_v2_40_60_accumulated = data.corrected_v2_40_60_accumulated;
    const auto& corrected_v2_0_20_Errors_accumulated = data.corrected_v2_0_20_Errors_accumulated;
    const auto& corrected_v2_20_40_Errors_accumulated = data.corrected_v2_20_40_Errors_accumulated;
    const auto& corrected_v2_40_60_Errors_accumulated = data.corrected_v2_40_60_Errors_accumulated;

    const auto& corrected_v3_0_20_accumulated = data.corrected_v3_0_20_accumulated;
    const auto& corrected_v3_40_60_accumulated = data.corrected_v3_40_60_accumulated;
    const auto& corrected_v3_0_20_Errors_accumulated = data.corrected_v3_0_20_Errors_accumulated;
    const auto& corrected_v3_40_60_Errors_accumulated = data.corrected_v3_40_60_Errors_accumulated;
    
    PrintVectorContents(corrected_v2_40_60_accumulated, corrected_v2_40_60_Errors_accumulated, "corrected_v2_40_60_accumulated");
    PrintVectorContents(corrected_v3_40_60_accumulated, corrected_v3_40_60_Errors_accumulated, "corrected_v3_40_60_accumulated");
    PrintVectorContents(corrected_v2_20_40_accumulated, corrected_v2_20_40_Errors_accumulated, "corrected_v2_20_40_accumulated");
    PrintVectorContents(corrected_v2_0_20_accumulated, corrected_v2_0_20_Errors_accumulated, "corrected_v2_0_20_accumulated");
    PrintVectorContents(corrected_v3_0_20_accumulated, corrected_v3_0_20_Errors_accumulated, "corrected_v3_0_20_accumulated");
    
    
    
    /*
     Overlay Accum and Run By Run 0-20 percent v2
     */

    TCanvas *c_runByRun_Accumulated_Overlay_0_20_v2 = new TCanvas("c_runByRun_Accumulated_Overlay_0_20_v2", "#pi^{0} #it{v}_{2} Overlay, Accumulated and Run By Run Data vs #it{p}_{T} 0-20% Centrality", 800, 600);
    corrected_graph_0_20_v2->Draw("AP");  // This will be the base graph
    corrected_graph_0_20_v2->SetTitle("#pi^{0} #it{v}_{2} Overlay, Accumulated and Run By Run Data vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    corrected_graph_0_20_v2->SetMinimum(-2.0); // Set the minimum y value
    corrected_graph_0_20_v2->SetMaximum(2.0); // Set the maximum y value
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_graph_0_20_v2->SetPoint(i, ptCenters[i] - offset, corrected_v2_0_20[i]);
    }
    TGraphErrors *corrected_v2_0_20_accumulated_graph = new TGraphErrors(ptCenters.size(), &ptCenters[0], &corrected_v2_0_20_accumulated[0], 0, &corrected_v2_0_20_Errors_accumulated[0]);
    
    corrected_v2_0_20_accumulated_graph->SetMarkerStyle(20); // circle
    corrected_v2_0_20_accumulated_graph->SetMarkerColor(kRed);
    corrected_v2_0_20_accumulated_graph->SetLineColor(kRed);
    corrected_v2_0_20_accumulated_graph -> Draw("P SAME");
    corrected_graph_0_20_v2->SetMarkerColor(kBlue);
    corrected_graph_0_20_v2->SetMarkerStyle(21);
    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend_runByRun_Accumulated_Overlay_0_20_v2 = new TLegend(0.11, 0.11, 0.31, 0.31);
    legend_runByRun_Accumulated_Overlay_0_20_v2->SetBorderSize(0);
    legend_runByRun_Accumulated_Overlay_0_20_v2->SetTextSize(0.03);
    legend_runByRun_Accumulated_Overlay_0_20_v2->AddEntry(corrected_v2_0_20_accumulated_graph, "0-20%, v_{2}^{#pi^{0}, Accumulated}", "p");
    legend_runByRun_Accumulated_Overlay_0_20_v2->AddEntry(corrected_graph_0_20_v2, "0-20%, v_{2}^{#pi^{0}, Run-by-Run}", "p");
    legend_runByRun_Accumulated_Overlay_0_20_v2->Draw();
    c_runByRun_Accumulated_Overlay_0_20_v2->Update();
    // Draw a dashed line at y = 0
    double x_min_runByRun_Accumulated_Overlay_0_20_v2 = c_runByRun_Accumulated_Overlay_0_20_v2->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_runByRun_Accumulated_Overlay_0_20_v2 = c_runByRun_Accumulated_Overlay_0_20_v2->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_runByRun_Accumulated_Overlay_0_20_v2 = new TLine(x_min_runByRun_Accumulated_Overlay_0_20_v2, 0, x_max_runByRun_Accumulated_Overlay_0_20_v2, 0);
    zeroLine_runByRun_Accumulated_Overlay_0_20_v2->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_runByRun_Accumulated_Overlay_0_20_v2->Draw("SAME"); // Draw the line on the same canvas as your plot
    c_runByRun_Accumulated_Overlay_0_20_v2->Modified();
    c_runByRun_Accumulated_Overlay_0_20_v2->Update();
    c_runByRun_Accumulated_Overlay_0_20_v2->SaveAs((PlotOutputBasePath + "0_20_v2_RunByRun_Overlay_Accumulated.png").c_str());
    
    
    
    /*
     Overlay Accum and Run By Run 20-40 percent v2
     */
    
    TCanvas *c_runByRun_Accumulated_Overlay_20_40_v2 = new TCanvas("c_runByRun_Accumulated_Overlay_20_40_v2", "#pi^{0} #it{v}_{2} Overlay, Accumulated and Run By Run Data vs #it{p}_{T} 20-40% Centrality", 800, 600);
    corrected_graph_20_40_v2->Draw("AP");  // This will be the base graph
    corrected_graph_20_40_v2->SetTitle("#pi^{0} #it{v}_{2} Overlay, Accumulated and Run By Run Data vs #it{p}_{T} 20-40% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    corrected_graph_20_40_v2->SetMinimum(-2.0); // Set the minimum y value
    corrected_graph_20_40_v2->SetMaximum(2.0); // Set the maximum y value
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_graph_20_40_v2->SetPoint(i, ptCenters[i] - offset, corrected_v2_20_40[i]);
    }
    TGraphErrors *corrected_v2_20_40_accumulated_graph = new TGraphErrors(ptCenters.size(), &ptCenters[0], &corrected_v2_20_40_accumulated[0], 0, &corrected_v2_20_40_Errors_accumulated[0]);
    
    corrected_v2_20_40_accumulated_graph->SetMarkerStyle(20); // circle
    corrected_v2_20_40_accumulated_graph->SetMarkerColor(kRed);
    corrected_v2_20_40_accumulated_graph->SetLineColor(kRed);
    corrected_v2_20_40_accumulated_graph -> Draw("P SAME");
    corrected_graph_20_40_v2->SetMarkerColor(kBlue);
    corrected_graph_20_40_v2->SetMarkerStyle(21);
    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend_runByRun_Accumulated_Overlay_20_40_v2 = new TLegend(0.11, 0.11, 0.31, 0.31);
    legend_runByRun_Accumulated_Overlay_20_40_v2->SetBorderSize(0);
    legend_runByRun_Accumulated_Overlay_20_40_v2->SetTextSize(0.03);
    legend_runByRun_Accumulated_Overlay_20_40_v2->AddEntry(corrected_v2_20_40_accumulated_graph, "20-40%, v_{2}^{#pi^{0}, Accumulated}", "p");
    legend_runByRun_Accumulated_Overlay_20_40_v2->AddEntry(corrected_graph_20_40_v2, "20-40%, v_{2}^{#pi^{0}, Run-by-Run}", "p");
    legend_runByRun_Accumulated_Overlay_20_40_v2->Draw();
    c_runByRun_Accumulated_Overlay_20_40_v2->Update();
    // Draw a dashed line at y = 0
    double x_min_runByRun_Accumulated_Overlay_20_40_v2 = c_runByRun_Accumulated_Overlay_20_40_v2->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_runByRun_Accumulated_Overlay_20_40_v2 = c_runByRun_Accumulated_Overlay_20_40_v2->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_runByRun_Accumulated_Overlay_20_40_v2 = new TLine(x_min_runByRun_Accumulated_Overlay_20_40_v2, 0, x_max_runByRun_Accumulated_Overlay_20_40_v2, 0);
    zeroLine_runByRun_Accumulated_Overlay_20_40_v2->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_runByRun_Accumulated_Overlay_20_40_v2->Draw("SAME"); // Draw the line on the same canvas as your plot
    c_runByRun_Accumulated_Overlay_20_40_v2->Modified();
    c_runByRun_Accumulated_Overlay_20_40_v2->Update();
    c_runByRun_Accumulated_Overlay_20_40_v2->SaveAs((PlotOutputBasePath + "20_40_v2_RunByRun_Overlay_Accumulated.png").c_str());
    
    
    /*
     Overlay Accum and Run By Run 40-60 percent v2
     */
    TCanvas *c_runByRun_Accumulated_Overlay_40_60_v2 = new TCanvas("c_runByRun_Accumulated_Overlay_40_60_v2", "#pi^{0} #it{v}_{2} Overlay, Accumulated and Run By Run Data vs #it{p}_{T} 40-60% Centrality", 800, 600);
    corrected_graph_40_60_v2->Draw("AP");  // This will be the base graph
    corrected_graph_40_60_v2->SetTitle("#pi^{0} #it{v}_{2} Overlay, Accumulated and Run By Run Data vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    corrected_graph_40_60_v2->SetMinimum(-2.0); // Set the minimum y value
    corrected_graph_40_60_v2->SetMaximum(2.0); // Set the maximum y value
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_graph_40_60_v2->SetPoint(i, ptCenters[i] - offset, corrected_v2_40_60[i]);
    }
    TGraphErrors *corrected_v2_40_60_accumulated_graph = new TGraphErrors(ptCenters.size(), &ptCenters[0], &corrected_v2_40_60_accumulated[0], 0, &corrected_v2_40_60_Errors_accumulated[0]);
    
    corrected_v2_40_60_accumulated_graph->SetMarkerStyle(20); // circle
    corrected_v2_40_60_accumulated_graph->SetMarkerColor(kRed);
    corrected_v2_40_60_accumulated_graph->SetLineColor(kRed);
    corrected_v2_40_60_accumulated_graph -> Draw("P SAME");
    corrected_graph_40_60_v2->SetMarkerColor(kBlue);
    corrected_graph_40_60_v2->SetMarkerStyle(21);
    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend_runByRun_Accumulated_Overlay_40_60_v2 = new TLegend(0.11, 0.11, 0.31, 0.31);
    legend_runByRun_Accumulated_Overlay_40_60_v2->SetBorderSize(0);
    legend_runByRun_Accumulated_Overlay_40_60_v2->SetTextSize(0.03);
    legend_runByRun_Accumulated_Overlay_40_60_v2->AddEntry(corrected_v2_40_60_accumulated_graph, "40-60%, v_{2}^{#pi^{0}, Accumulated}", "p");
    legend_runByRun_Accumulated_Overlay_40_60_v2->AddEntry(corrected_graph_40_60_v2, "40-60%, v_{2}^{#pi^{0}, Run-by-Run}", "p");
    legend_runByRun_Accumulated_Overlay_40_60_v2->Draw();
    c_runByRun_Accumulated_Overlay_40_60_v2->Update();
    // Draw a dashed line at y = 0
    double x_min_runByRun_Accumulated_Overlay_40_60_v2 = c_runByRun_Accumulated_Overlay_40_60_v2->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_runByRun_Accumulated_Overlay_40_60_v2 = c_runByRun_Accumulated_Overlay_40_60_v2->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_runByRun_Accumulated_Overlay_40_60_v2 = new TLine(x_min_runByRun_Accumulated_Overlay_40_60_v2, 0, x_max_runByRun_Accumulated_Overlay_40_60_v2, 0);
    zeroLine_runByRun_Accumulated_Overlay_40_60_v2->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_runByRun_Accumulated_Overlay_40_60_v2->Draw("SAME"); // Draw the line on the same canvas as your plot
    c_runByRun_Accumulated_Overlay_40_60_v2->Modified();
    c_runByRun_Accumulated_Overlay_40_60_v2->Update();
    c_runByRun_Accumulated_Overlay_40_60_v2->SaveAs((PlotOutputBasePath + "40_60_v2_RunByRun_Overlay_Accumulated.png").c_str());
    
    
    
    
    /*
     Overlay Accum and Run By Run 0-20 percent v3
     */

    TCanvas *c_runByRun_Accumulated_Overlay_0_20_v3 = new TCanvas("c_runByRun_Accumulated_Overlay_0_20_v3", "#pi^{0} #it{v}_{3} Overlay, Accumulated and Run By Run Data vs #it{p}_{T} 0-20% Centrality", 800, 600);
    corrected_graph_0_20_v3->Draw("AP");  // This will be the base graph
    corrected_graph_0_20_v3->SetTitle("#pi^{0} #it{v}_{3} Overlay, Accumulated and Run By Run Data vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{3}");
    corrected_graph_0_20_v3->SetMinimum(-4.0); // Set the minimum y value
    corrected_graph_0_20_v3->SetMaximum(4.0); // Set the maximum y value
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_graph_0_20_v3->SetPoint(i, ptCenters[i] - offset, corrected_v3_0_20[i]);
    }
    TGraphErrors *corrected_v3_0_20_accumulated_graph = new TGraphErrors(ptCenters.size(), &ptCenters[0], &corrected_v3_0_20_accumulated[0], 0, &corrected_v3_0_20_Errors_accumulated[0]);
    
    corrected_v3_0_20_accumulated_graph->SetMarkerStyle(20); // circle
    corrected_v3_0_20_accumulated_graph->SetMarkerColor(kRed);
    corrected_v3_0_20_accumulated_graph->SetLineColor(kRed);
    corrected_v3_0_20_accumulated_graph -> Draw("P SAME");
    corrected_graph_0_20_v3->SetMarkerColor(kBlue);
    corrected_graph_0_20_v3->SetMarkerStyle(21);
    corrected_graph_0_20_v3->SetLineColor(kBlack);
    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend_runByRun_Accumulated_Overlay_0_20_v3 = new TLegend(0.11, 0.11, 0.31, 0.31);
    legend_runByRun_Accumulated_Overlay_0_20_v3->SetBorderSize(0);
    legend_runByRun_Accumulated_Overlay_0_20_v3->SetTextSize(0.03);
    legend_runByRun_Accumulated_Overlay_0_20_v3->AddEntry(corrected_v3_0_20_accumulated_graph, "0-20%, v_{3}^{#pi^{0}, Accumulated}", "p");
    legend_runByRun_Accumulated_Overlay_0_20_v3->AddEntry(corrected_graph_0_20_v3, "0-20%, v_{3}^{#pi^{0}, Run-by-Run}", "p");
    legend_runByRun_Accumulated_Overlay_0_20_v3->Draw();
    c_runByRun_Accumulated_Overlay_0_20_v3->Update();
    // Draw a dashed line at y = 0
    double x_min_runByRun_Accumulated_Overlay_0_20_v3 = c_runByRun_Accumulated_Overlay_0_20_v3->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_runByRun_Accumulated_Overlay_0_20_v3 = c_runByRun_Accumulated_Overlay_0_20_v3->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_runByRun_Accumulated_Overlay_0_20_v3 = new TLine(x_min_runByRun_Accumulated_Overlay_0_20_v3, 0, x_max_runByRun_Accumulated_Overlay_0_20_v3, 0);
    zeroLine_runByRun_Accumulated_Overlay_0_20_v3->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_runByRun_Accumulated_Overlay_0_20_v3->Draw("SAME"); // Draw the line on the same canvas as your plot
    c_runByRun_Accumulated_Overlay_0_20_v3->Modified();
    c_runByRun_Accumulated_Overlay_0_20_v3->Update();
    c_runByRun_Accumulated_Overlay_0_20_v3->SaveAs((PlotOutputBasePath + "0_20_v3_RunByRun_Overlay_Accumulated.png").c_str());
    
    
    
    //NA DATA FOR ACCUMULATED 20-40 V3
    
    /*
     Overlay Accum and Run By Run 40-60 percent v3
     */
    TCanvas *c_runByRun_Accumulated_Overlay_40_60_v3 = new TCanvas("c_runByRun_Accumulated_Overlay_40_60_v3", "#pi^{0} #it{v}_{2} Overlay, Accumulated and Run By Run Data vs #it{p}_{T} 40-60% Centrality", 800, 600);
    corrected_graph_40_60_v3->Draw("AP");  // This will be the base graph
    corrected_graph_40_60_v3->SetTitle("#pi^{0} #it{v}_{3} Overlay, Accumulated and Run By Run Data vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{3}");
    corrected_graph_40_60_v3->SetMinimum(-4.0); // Set the minimum y value
    corrected_graph_40_60_v3->SetMaximum(4.0); // Set the maximum y value
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_graph_40_60_v3->SetPoint(i, ptCenters[i] - offset, corrected_v3_40_60[i]);
    }
    TGraphErrors *corrected_v3_40_60_accumulated_graph = new TGraphErrors(ptCenters.size(), &ptCenters[0], &corrected_v3_40_60_accumulated[0], 0, &corrected_v3_40_60_Errors_accumulated[0]);
    
    corrected_v3_40_60_accumulated_graph->SetMarkerStyle(20); // circle
    corrected_v3_40_60_accumulated_graph->SetMarkerColor(kRed);
    corrected_v3_40_60_accumulated_graph->SetLineColor(kRed);
    corrected_v3_40_60_accumulated_graph -> Draw("P SAME");
    corrected_graph_40_60_v3->SetMarkerColor(kBlue);
    corrected_graph_40_60_v3->SetLineColor(kBlack);
    corrected_graph_40_60_v3->SetMarkerStyle(21);
    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend_runByRun_Accumulated_Overlay_40_60_v3 = new TLegend(0.11, 0.11, 0.31, 0.31);
    legend_runByRun_Accumulated_Overlay_40_60_v3->SetBorderSize(0);
    legend_runByRun_Accumulated_Overlay_40_60_v3->SetTextSize(0.03);
    legend_runByRun_Accumulated_Overlay_40_60_v3->AddEntry(corrected_v3_40_60_accumulated_graph, "40-60%, v_{3}^{#pi^{0}, Accumulated}", "p");
    legend_runByRun_Accumulated_Overlay_40_60_v3->AddEntry(corrected_graph_40_60_v3, "40-60%, v_{3}^{#pi^{0}, Run-by-Run}", "p");
    legend_runByRun_Accumulated_Overlay_40_60_v3->Draw();
    c_runByRun_Accumulated_Overlay_40_60_v3->Update();
    // Draw a dashed line at y = 0
    double x_min_runByRun_Accumulated_Overlay_40_60_v3 = c_runByRun_Accumulated_Overlay_40_60_v3->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_runByRun_Accumulated_Overlay_40_60_v3 = c_runByRun_Accumulated_Overlay_40_60_v3->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_runByRun_Accumulated_Overlay_40_60_v3 = new TLine(x_min_runByRun_Accumulated_Overlay_40_60_v3, 0, x_max_runByRun_Accumulated_Overlay_40_60_v3, 0);
    zeroLine_runByRun_Accumulated_Overlay_40_60_v3->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_runByRun_Accumulated_Overlay_40_60_v3->Draw("SAME"); // Draw the line on the same canvas as your plot
    c_runByRun_Accumulated_Overlay_40_60_v3->Modified();
    c_runByRun_Accumulated_Overlay_40_60_v3->Update();
    c_runByRun_Accumulated_Overlay_40_60_v3->SaveAs((PlotOutputBasePath + "40_60_v3_RunByRun_Overlay_Accumulated.png").c_str());
    
}




void SaveHistogramsAsImages() {
    const int numQQ2 = 18; // Assuming you have 18 histograms for v2
    const int numQQ3 = 18; // Assuming you have 18 histograms for v3

    // Directories for the PNG images
    std::string outputPathV2 = "/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_RunByRun_Corrected_hists_data/v2_Corrected_Histograms/";
    std::string outputPathV3 = "/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_RunByRun_Corrected_hists_data/v3_Corrected_Histograms/";

    // Open the v2 ROOT file
    TFile* fileV2 = TFile::Open("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/v2_corrected_runByRun.root", "READ");
    if (fileV2 && !fileV2->IsZombie()) {
        for (int i = 0; i < numQQ2; ++i) {
            TH1F* hist = (TH1F*)fileV2->Get(Form("v2_corrected_%d", i));
            if (hist) {
                TCanvas* c = new TCanvas();
                hist->Draw();
                c->SaveAs((outputPathV2 + Form("v2_corrected_%d.png", i)).c_str());
                delete c; // Clean up the canvas
            } else {
                std::cerr << "Histogram v2_corrected_" << i << " not found." << std::endl;
            }
        }
        fileV2->Close();
        delete fileV2; // Clean up the file object
    } else {
        std::cerr << "Failed to open v2 corrected ROOT file." << std::endl;
    }

    // Open the v3 ROOT file
    TFile* fileV3 = TFile::Open("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/v3_corrected_runByRun.root", "READ");
    if (fileV3 && !fileV3->IsZombie()) {
        for (int i = 0; i < numQQ3; ++i) {
            TH1F* hist = (TH1F*)fileV3->Get(Form("v3_corrected_%d", i));
            if (hist) {
                TCanvas* c = new TCanvas();
                hist->Draw();
                c->SaveAs((outputPathV3 + Form("v3_corrected_%d.png", i)).c_str());
                delete c; // Clean up the canvas
            } else {
                std::cerr << "Histogram v3_corrected_" << i << " not found." << std::endl;
            }
        }
        fileV3->Close();
        delete fileV3; // Clean up the file object
    } else {
        std::cerr << "Failed to open v3 corrected ROOT file." << std::endl;
    }
}
void ExtractHistogramRunByRunVn() {
    const int numIndices = 18; // Number of histograms per file
    TFile* fileV2 = TFile::Open("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/v2_corrected_runByRun.root", "READ");
    TFile* fileV3 = TFile::Open("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/v3_corrected_runByRun.root", "READ");

    // Change the output file extension to .csv and use commas as delimiters
    std::ofstream outFile("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_RunByRun_Corrected_hists_data/mean_sigma_data.csv");
    outFile << "Index,mean_pi0_v2,mean_pi0_v2_error,mean_pi0_v3,mean_pi0_v3_error\n";

    for (int i = 0; i < numIndices; ++i) {
        TH1F* hV2 = nullptr;
        TH1F* hV3 = nullptr;
        if (fileV2) hV2 = (TH1F*)fileV2->Get(Form("v2_corrected_%d", i));
        if (fileV3) hV3 = (TH1F*)fileV3->Get(Form("v3_corrected_%d", i));

        double meanV2 = 0, meanV3 = 0, errorV2 = 0, errorV3 = 0;

        if (hV2) {
            meanV2 = hV2->GetMean();
            errorV2 = hV2->GetStdDev() / sqrt(hV2->GetEntries());
        }

        if (hV3) {
            meanV3 = hV3->GetMean();
            errorV3 = hV3->GetStdDev() / sqrt(hV3->GetEntries());
        }

        // Use commas instead of tabs for CSV format
        outFile << i << "," << meanV2 << "," << errorV2 << "," << meanV3 << "," << errorV3 << "\n";
    }

    if (fileV2) {
        fileV2->Close();
        delete fileV2;
    }

    if (fileV3) {
        fileV3->Close();
        delete fileV3;
    }

    outFile.close();
}


// Function to translate centrality index to human-readable range
std::string CentralityDescription(int QQ_index) {
    switch (QQ_index) {
        case 0: return "40-60% Centrality";
        case 1: return "20-40% Centrality";
        case 2: return "0-20% Centrality";
        default: return "Unknown Centrality";
    }
}
void PrintSkippedRunsSummary(const std::map<int, int>& skippedCounts) {
    for (const auto& pair : skippedCounts) {
        std::cout << CentralityDescription(pair.first) << ": " << pair.second / 6 << " skipped runs\n";
    }
}
void SaveHistogram(TFile* file, const std::string& histName, const std::string& runDirPath, const std::string& runNumber) {
    TH1F* hist = (TH1F*)file->Get(histName.c_str());
    if (hist) {
        std::string newTitle = hist->GetTitle() + std::string(" - Run ") + runNumber;
        hist->SetTitle(newTitle.c_str());

        TCanvas canvas;
        canvas.SetLogy(1); // Set y-axis to logarithmic scale
        hist->Draw();

        std::string saveFileName = histName;
        std::replace(saveFileName.begin(), saveFileName.end(), '/', '_');
        std::string saveFilePath = runDirPath + "/" + saveFileName + ".png";
        canvas.SaveAs(saveFilePath.c_str());
        std::cout << "Saved " << saveFilePath << std::endl;
    } else {
        std::cerr << "Error: Histogram " << histName << " not found in run " << runNumber << std::endl;
    }
}
void vN_calculator_RunByRun_Final() {
    // Open the ROOT file
    std::vector<std::string> RunNumbers = {"23020", "23671", "23672", "23676", "23681", "23682", "23687", "23690", "23693", "23694", "23695", "23696", "23697", "23699", "23702", "23714", "23726", "23727", "23728", "23735", "23737", "23738", "23739", "23740", "23743", "23745", "23746"};
    std::string basePathRootFiles = "/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/run_by_run_vN_test_Root_Files_2_14_24/";
    

    std::string readCSVBasePath = "/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/PlotByPlotOutput_RunByRun_2_13_24/";
    std::string outputBasePathCSVs = "/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/PlotByPlotOutput_RunByRun_2_13_24/run_ByRun_EdittedCSVs/";
    
    const int numQQ2 = 18; // Number of histograms for v2
    const int numQQ3 = 18; // Number of histograms for v3
    
    // Initialize ROOT files for v2 and v3 corrected values
    TFile* fileV2Corrected = new TFile("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/v2_corrected_runByRun.root", "RECREATE");
    TFile* fileV3Corrected = new TFile("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/v3_corrected_runByRun.root", "RECREATE");

    
    // Initialize histograms for v2 and v3 corrected values
    TH1F* hV2Corrected[numQQ2];
    TH1F* hV3Corrected[numQQ3];
    
    // Create histograms
    for (int i = 0; i < numQQ2; ++i) {
        hV2Corrected[i] = new TH1F(Form("v2_corrected_%d", i), Form("Corrected v2 for Index %d", i), 100, -5.0, 5.0);
        hV2Corrected[i]->SetXTitle("v_{2}");
        hV2Corrected[i]->SetYTitle("Counts");
        hV3Corrected[i] = new TH1F(Form("v3_corrected_%d", i), Form("Corrected v3 for Index %d", i), 100, -5.0, 5.0); // Same for v3
        hV3Corrected[i]->SetXTitle("v_{3}");
        hV3Corrected[i]->SetYTitle("Counts");
    }
    
    // Variables to track negative QQ2 and QQ3 means
    int negativeQQ2Count = 0;
    int negativeQQ3Count = 0;
    
    std::map<int, int> skippedQQ2Counts;
    std::map<int, int> skippedQQ3Counts;
    
    
    std::vector<std::pair<std::string, int>> skippedIndices; // To store run numbers and indices skipped due to negative values


    for (const std::string& runNumber : RunNumbers) {
        std::cout << "\nProcessing Run Number: " << runNumber << std::endl;
        std::string rootFilePath = basePathRootFiles + runNumber + "/test-" + runNumber + ".root";
        std::cout << "Opening ROOT file: " << rootFilePath << std::endl;
        
        std::string readCSVFilePath = readCSVBasePath + "fit-stats-" + runNumber + ".csv";
        std::string writeCSVFilePath = outputBasePathCSVs + "Fit-stats-" + runNumber + "_updated.csv";
        std::string runDirPath = "/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_RunByRun_Corrected_hists_data/QQ_qQ_RunByRun_2_14_24/" + runNumber + "/";
        // List of histogram names to be processed
        std::vector<std::string> histogramNames = {
            "vn/QQ2/hQQ2_40-60", "vn/QQ2/hQQ2_20-40", "vn/QQ2/hQQ2_0-20", "vn/QQ3/hQQ3_40-60", "vn/QQ3/hQQ3_20-40", "vn/QQ3/hQQ3_0-20"
        };
        // Open the original CSV file for this run for reading
        std::ifstream inFile(readCSVFilePath);
        if (!inFile) {
            std::cerr << "Error opening input CSV file: " << readCSVFilePath << std::endl;
            continue;
        }

        // Open the new CSV file for this run for writing
        std::ofstream outFile(writeCSVFilePath, std::ios::out);
        if (!outFile) {
            std::cerr << "Error opening output CSV file: " << writeCSVFilePath << std::endl;
            continue;
        }

        // Assuming you're adding headers if the file is new or checking if it needs headers
        if (outFile.tellp() == 0) { // This check is not strictly necessary for newly created files
            outFile << "Header line with columns names, including new ones for v2 and v3 calculations" << std::endl;
        }

        // Open the ROOT file for this run
        TFile* file = TFile::Open(rootFilePath.c_str(), "READ");
        if (!file || file->IsZombie()) {
            std::cerr << "Error opening ROOT file: " << rootFilePath << std::endl;
            continue; // Skip to the next run number if this file cannot be opened
        }
        
        // Check if the CSV file is newly created or empty and, if so, write the header
        if (outFile.tellp() == 0) {
            outFile << "Header line with columns names, including new ones for v2 and v3 calculations" << std::endl;
        }
        
        /*
         Histograms Related to v2, SET UP VECTORS FOR QQ2, qQ2, qQ2_bg, qQ2_bg_left
         */
        // Define the histogram names for QQ2 within the vn/QQ2 directory
        std::string hist_name_QQ2[3] = {"vn/QQ2/hQQ2_40-60", "vn/QQ2/hQQ2_20-40", "vn/QQ2/hQQ2_0-20"};
        // Loop through all histogram arrays and save each histogram
        for (const auto& histName : hist_name_QQ2) {
            //SaveHistogram(file, histName, runDirPath, runNumber);
        }
        // Define the histogram names for qQ within the vn/qQ directory
        std::string hist_name_qQ2[18];
        for (int i = 0; i < 18; ++i) {
            hist_name_qQ2[i] = "vn/qQ2/hqQ2_" + std::to_string(i);
        }
        // Loop through all histogram arrays and save each histogram
        for (const auto& histName : hist_name_qQ2) {
//            SaveHistogram(file, histName, runDirPath, runNumber);
        }
        std::string hist_name_qQ2_bg[18];
        for (int i = 0; i < 18; ++i) {
            hist_name_qQ2_bg[i] = "vn/qQ2_bg/hqQ2_bg_" + std::to_string(i);
        }
        for (const auto& histName : hist_name_qQ2_bg) {
            //SaveHistogram(file, histName, runDirPath, runNumber);
        }

        std::string hist_name_qQ2_bg_left[18];
        for (int i = 0; i < 18; ++i) {
            hist_name_qQ2_bg_left[i] = "vn/qQ2_bg_left/hqQ2_bg_left_" + std::to_string(i);
        }
        for (const auto& histName : hist_name_qQ2_bg_left) {
            //SaveHistogram(file, histName, runDirPath, runNumber);
        }

        
        
        /*
         Histograms Related to v3, SET UP VECTORS FOR QQ3, qQ3, qQ3_bg, qQ3_bg_left
         */
        
        std::string hist_name_QQ3[3] = {"vn/QQ3/hQQ3_40-60", "vn/QQ3/hQQ3_20-40", "vn/QQ3/hQQ3_0-20"};
        for (const auto& histName : hist_name_QQ3) {
           // SaveHistogram(file, histName, runDirPath, runNumber);
        }

        // Define the histogram names for qQ within the vn/qQ directory
        std::string hist_name_qQ3[18];
        for (int i = 0; i < 18; ++i) {
            hist_name_qQ3[i] = "vn/qQ3/hqQ3_" + std::to_string(i);
        }
        for (const auto& histName : hist_name_qQ3) {
            //SaveHistogram(file, histName, runDirPath, runNumber);
        }
        
        std::string hist_name_qQ3_bg[18];
        for (int i = 0; i < 18; ++i) {
            hist_name_qQ3_bg[i] = "vn/qQ3_bg/hqQ3_bg_" + std::to_string(i);
        }
        for (const auto& histName : hist_name_qQ3_bg) {
           // SaveHistogram(file, histName, runDirPath, runNumber);
        }
        

        std::string hist_name_qQ3_bg_left[18];
        for (int i = 0; i < 18; ++i) {
            hist_name_qQ3_bg_left[i] = "vn/qQ3_bg_left/hqQ3_bg_left_" + std::to_string(i);
        }
        
        for (const auto& histName : hist_name_qQ3_bg_left) {
           // SaveHistogram(file, histName, runDirPath, runNumber);
        }
        
        
        /*
         RETRIEVE MEANS AND ERROR FOR n = 2 histograms
         */
        // Retrieve and calculate QQ means and their errors
        float QQ2_mean[3], QQ2_error[3];
        for (int i = 0; i < 3; ++i) {
            TH1F *h_QQ2 = (TH1F*)file->Get(hist_name_QQ2[i].c_str());
            if (!h_QQ2) {
                std::cerr << "Error: QQ2 histogram " << hist_name_QQ2[i] << " not found" << std::endl;
                file->Close();
                delete file;
                return;
            }
            QQ2_mean[i] = h_QQ2->GetMean();
            float stdDev = h_QQ2->GetStdDev();
            int nEntries = h_QQ2->GetEntries();
            QQ2_error[i] = stdDev / sqrt(nEntries); // Standard deviation divided by the square root of the number of entries
        }
        
        
        // Retrieve and calculate qQ means and their errors
        float qQ2_mean[18], qQ2_error[18];
        for (int i = 0; i < 18; ++i) {
            TH1F *h_qQ2 = (TH1F*)file->Get(hist_name_qQ2[i].c_str());
            if (!h_qQ2) {
                std::cerr << "Error: qQ histogram " << hist_name_qQ2[i] << " not found" << std::endl;
                file->Close();
                delete file;
                return;
            }
            qQ2_mean[i] = h_qQ2->GetMean();
            float stdDev = h_qQ2->GetStdDev();
            int nEntries = h_qQ2->GetEntries();
            qQ2_error[i] = stdDev / sqrt(nEntries);
        }
        
        // Retrieve and calculate background qQ means and their errors
        float qQ2_bg_mean[18], qQ2_bg_error[18];
        for (int i = 0; i < 18; ++i) {
            TH1F *h_qQ2_bg = (TH1F*)file->Get(hist_name_qQ2_bg[i].c_str());
            if (!h_qQ2_bg) {
                std::cerr << "Error: Background qQ histogram " << hist_name_qQ2_bg[i] << " not found" << std::endl;
                continue; // Skip to the next iteration instead of returning to process all histograms
            }
            qQ2_bg_mean[i] = h_qQ2_bg->GetMean();
            float stdDev = h_qQ2_bg->GetStdDev();
            int nEntries = h_qQ2_bg->GetEntries();
            qQ2_bg_error[i] = stdDev / sqrt(nEntries);
        }

        // Retrieve and calculate left background qQ means and their errors
        float qQ2_bg_left_mean[18], qQ2_bg_left_error[18];
        for (int i = 0; i < 18; ++i) {
            TH1F *h_qQ2_bg_left = (TH1F*)file->Get(hist_name_qQ2_bg_left[i].c_str());
            if (!h_qQ2_bg_left) {
                std::cerr << "Error: Left Background qQ histogram " << hist_name_qQ2_bg_left[i] << " not found" << std::endl;
                continue; // Use continue to skip this iteration, ensuring all histograms are processed
            }
            qQ2_bg_left_mean[i] = h_qQ2_bg_left->GetMean();
            float stdDev = h_qQ2_bg_left->GetStdDev();
            int nEntries = h_qQ2_bg_left->GetEntries();
            qQ2_bg_left_error[i] = stdDev / sqrt(nEntries);
        }
        
        
        
        
        /*
         Do the Same for N = 3 Histograms
         */
        // Retrieve and calculate QQ means and their errors
        float QQ3_mean[3], QQ3_error[3];
        for (int i = 0; i < 3; ++i) {
            TH1F *h_QQ3 = (TH1F*)file->Get(hist_name_QQ3[i].c_str());
            if (!h_QQ3) {
                std::cerr << "Error: QQ3 histogram " << hist_name_QQ3[i] << " not found" << std::endl;
                file->Close();
                delete file;
                return;
            }
            QQ3_mean[i] = h_QQ3->GetMean();
            float stdDev = h_QQ3->GetStdDev();
            int nEntries = h_QQ3->GetEntries();
            QQ3_error[i] = stdDev / sqrt(nEntries); // Standard deviation divided by the square root of the number of entries
        }
        
        
        // Retrieve and calculate qQ means and their errors
        float qQ3_mean[18], qQ3_error[18];
        for (int i = 0; i < 18; ++i) {
            TH1F *h_qQ3 = (TH1F*)file->Get(hist_name_qQ3[i].c_str());
            if (!h_qQ3) {
                std::cerr << "Error: qQ histogram " << hist_name_qQ3[i] << " not found" << std::endl;
                file->Close();
                delete file;
                return;
            }
            qQ3_mean[i] = h_qQ3->GetMean();
            float stdDev = h_qQ3->GetStdDev();
            int nEntries = h_qQ3->GetEntries();
            qQ3_error[i] = stdDev / sqrt(nEntries);
        }
        
        // Retrieve and calculate background qQ means and their errors
        float qQ3_bg_mean[18], qQ3_bg_error[18];
        for (int i = 0; i < 18; ++i) {
            TH1F *h_qQ3_bg = (TH1F*)file->Get(hist_name_qQ3_bg[i].c_str());
            if (!h_qQ3_bg) {
                std::cerr << "Error: Background qQ histogram " << hist_name_qQ3_bg[i] << " not found" << std::endl;
                continue; // Skip to the next iteration instead of returning to process all histograms
            }
            qQ3_bg_mean[i] = h_qQ3_bg->GetMean();
            float stdDev = h_qQ3_bg->GetStdDev();
            int nEntries = h_qQ3_bg->GetEntries();
            qQ3_bg_error[i] = stdDev / sqrt(nEntries);
        }

        // Retrieve and calculate left background qQ means and their errors
        float qQ3_bg_left_mean[18], qQ3_bg_left_error[18];
        for (int i = 0; i < 18; ++i) {
            TH1F *h_qQ3_bg_left = (TH1F*)file->Get(hist_name_qQ3_bg_left[i].c_str());
            if (!h_qQ3_bg_left) {
                std::cerr << "Error: Left Background qQ histogram " << hist_name_qQ3_bg_left[i] << " not found" << std::endl;
                continue; // Use continue to skip this iteration, ensuring all histograms are processed
            }
            qQ3_bg_left_mean[i] = h_qQ3_bg_left->GetMean();
            float stdDev = h_qQ3_bg_left->GetStdDev();
            int nEntries = h_qQ3_bg_left->GetEntries();
            qQ3_bg_left_error[i] = stdDev / sqrt(nEntries);
        }
        
        
        
        // User-defined cut values
        float userEnergy = 0.5;
        float userAsymm = 0.5;
        float userChi2 = 4;
        float userDeltaR = 0;

        if (!Plot_vN_bool) {
            std::string line;
            int lineIndex = 0;

            while (getline(inFile, line)) {
                if (lineIndex == 0) {
                    // Header line
                    outFile << line << ",v2_measured,v2_measured_error,bg_v2,bg_v2_err,left_bg_v2,left_bg_v2_err,v2_corrected,v2_corrected_error,v3_measured,v3_measured_error,bg_v3,bg_v3_err,left_bg_v3,left_bg_v3_err,v3_corrected,v3_corrected_error" << std::endl;
                } else {
                    std::stringstream ss(line);
                    std::string token;
                    std::vector<float> values;
                    while (getline(ss, token, ',')) {
                        values.push_back(std::stof(token));
                    }
                    

                    // Check if the row matches the user-defined cut values
                    if (values[1] == userEnergy && values[2] == userAsymm && values[3] == userChi2 && values[4] == userDeltaR) {
                        int idx = static_cast<int>(values[0]); // Index is the first value in the row
                        
                        float SB = values[9];
                        float SBerror = values[10]; // S/Berror is the 11th column

                        // Determine the QQ_index based on the idx value
                        int QQ_index = 0;
                        if (idx >= 0 && idx <= 5) {
                            QQ_index = 0; // 40-60% centrality
                        } else if (idx >= 6 && idx <= 11) {
                            QQ_index = 1; // 20-40% centrality
                        } else if (idx >= 12 && idx <= 17) {
                            QQ_index = 2; // 0-20% centrality
                        }
                        float v2_value = qQ2_mean[idx] / sqrt(QQ2_mean[QQ_index]);
                        float v2_error = abs(v2_value) * sqrt(pow(qQ2_error[idx] / qQ2_mean[idx], 2) + 0.25 * pow(QQ2_error[QQ_index] / QQ2_mean[QQ_index], 2));

                    
                        float bg_v2_value = qQ2_bg_mean[idx] / sqrt(QQ2_mean[QQ_index]);
                        float bg_v2_error = abs(bg_v2_value) * sqrt(pow(qQ2_bg_error[idx] / qQ2_bg_mean[idx], 2) + 0.25 * pow(QQ2_error[QQ_index] / QQ2_mean[QQ_index], 2));

                        // Calculate left background v2 and v2_error
                        float left_bg_v2_value = qQ2_bg_left_mean[idx] / sqrt(QQ2_mean[QQ_index]);
                        float left_bg_v2_error = abs(left_bg_v2_value) * sqrt(pow(qQ2_bg_left_error[idx] / qQ2_bg_left_mean[idx], 2) + 0.25 * pow(QQ2_error[QQ_index] / QQ2_mean[QQ_index], 2));

                        float v2_corrected = v2_value * (1 + (1 / SB)) - ((1 / SB) * bg_v2_value);
                        
                        float v2_corrected_error = sqrt(
                            (1 + 1 /(SB*SB)) * pow(v2_error, 2) + // Error contribution from v2_signal
                            pow((-1 / SB) * bg_v2_error, 2) +   // Error contribution from v2_bg (note the negative sign is squared, having no effect)
                            pow((-(v2_value - bg_v2_value) / (SB * SB)) * SBerror, 2) // Error contribution from SB
                        );
                        
                        
                        
                        float v3_value = qQ3_mean[idx] / sqrt(QQ3_mean[QQ_index]);
                        float v3_error = abs(v3_value) * sqrt(pow(qQ3_error[idx] / qQ3_mean[idx], 2) + 0.25 * pow(QQ3_error[QQ_index] / QQ3_mean[QQ_index], 2));


                        float bg_v3_value = qQ3_bg_mean[idx] / sqrt(QQ3_mean[QQ_index]);
                        float bg_v3_error = abs(bg_v3_value) * sqrt(pow(qQ3_bg_error[idx] / qQ3_bg_mean[idx], 2) + 0.25 * pow(QQ3_error[QQ_index] / QQ3_mean[QQ_index], 2));

                        // Calculate left background v3 and v3_error
                        float left_bg_v3_value = qQ3_bg_left_mean[idx] / sqrt(QQ3_mean[QQ_index]);
                        float left_bg_v3_error = abs(left_bg_v3_value) * sqrt(pow(qQ3_bg_left_error[idx] / qQ3_bg_left_mean[idx], 2) + 0.25 * pow(QQ3_error[QQ_index] / QQ3_mean[QQ_index], 2));

                        float v3_corrected = v3_value * (1 + (1 / SB)) - ((1 / SB) * bg_v3_value);
                        
                        float v3_corrected_error = sqrt(
                            (1 + 1 /(SB*SB)) * pow(v2_error, 2) + // Error contribution from v3_signal
                            pow((-1 / SB) * bg_v3_error, 2) +   // Error contribution from v3_bg (note the negative sign is squared, having no effect)
                            pow((-(v3_value - bg_v3_value) / (SB * SB)) * SBerror, 2) // Error contribution from SB
                        );
                        // Check for negative QQ2 or QQ3 values
                        if (QQ2_mean[QQ_index] >= 0) {
                            hV2Corrected[idx]->Fill(v2_corrected);
                        }
                        else {
                            std::cout << "\033[31mNegative QQ2_mean encountered: Run Number = " << runNumber << ", Centrality Index = " << QQ_index << "\033[0m" << std::endl;

                            negativeQQ2Count++;
                            
                            skippedQQ2Counts[QQ_index]++;
                            std::cout << std::endl; // Add an extra line break here
                        }
                        // Only fill hV3Corrected if QQ3_mean[QQ_index] is not negative
                        if (QQ3_mean[QQ_index] >= 0) {
                             hV3Corrected[idx]->Fill(v3_corrected);
                        }
                        else {
                            std::cout << "Negative QQ3_mean encountered: Run Number = " << runNumber << ", Centrality Index = " << QQ_index << std::endl;
                            negativeQQ3Count++;
                            skippedQQ3Counts[QQ_index]++;
                            std::cout << std::endl; // And another extra line break here
                        }


                        // Inside your function where the line is being constructed
                        std::stringstream ss;
                        ss << line << ","
                           << v2_value << "," << v2_error << ","
                           << bg_v2_value << "," << bg_v2_error << ","
                           << left_bg_v2_value << "," << left_bg_v2_error << ","
                           << v2_corrected << "," << v2_corrected_error << ","
                           << v3_value << "," << v3_error << ","
                           << bg_v3_value << "," << bg_v3_error << ","
                           << left_bg_v3_value << "," << left_bg_v3_error << ","
                           << v3_corrected << "," << v3_corrected_error;

                        // Output the constructed line to the outFile
                        outFile << ss.str() << std::endl;

                    }
                }
                ++lineIndex;
            }
            inFile.close();
            outFile.close();

            // Close the current ROOT file and CSV file
            file->Close();
            delete file;
        }
    }
    // Check and execute Plot_vN
    if (Plot_vN_bool) {
        AccumulatedData data;
        std::string Accumulated_Data_Path = "/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/I_O_Accumulated_PlotByPlotOutput/UpdatedCSV_AccumulatedDists_p009.csv";

        Read_Accumulated_CSV(Accumulated_Data_Path, data);
        Plot_vN(data);
    }
    else {
        fileV2Corrected->cd(); // Switch to the v2_corrected ROOT file directory
        for (int i = 0; i < numQQ2; ++i) {
            hV2Corrected[i]->Write(); // Write each histogram
        }

        fileV3Corrected->cd(); // Switch to the v3_corrected ROOT file directory
        for (int i = 0; i < numQQ3; ++i) {
            hV3Corrected[i]->Write(); // Write each histogram
        }

        // Close the ROOT files
        fileV2Corrected->Close();
        fileV3Corrected->Close();
        

        delete fileV2Corrected;
        delete fileV3Corrected;
        
        SaveHistogramsAsImages();
        
        ExtractHistogramRunByRunVn();
        
        std::cout << "\nSummary of Skipped Runs Due to Negative Means:\n";
        std::cout << "QQ2 Skipped Runs by Centrality:\n";
        PrintSkippedRunsSummary(skippedQQ2Counts);

        std::cout << "\nQQ3 Skipped Runs by Centrality:\n";
        PrintSkippedRunsSummary(skippedQQ3Counts);
    }
}
