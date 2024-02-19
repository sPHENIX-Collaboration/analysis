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
void Plot_vN() {
    std::string filePath = "/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/I_O_Accumulated_PlotByPlotOutput/UpdatedCSV_AccumulatedDists_p009.csv";

    std::ifstream file(filePath);
    std::string line;
    std::vector<double> ptCenters = {2.25, 2.75, 3.25, 3.75, 4.25, 4.75}; // Mid-points of pT ranges
    std::vector<double> v2_0_20, v2_20_40, v2_40_60, bg_v2_0_20, bg_v2_20_40, bg_v2_40_60, bg_v2_left_0_20, bg_v2_left_20_40, bg_v2_left_40_60, corrected_v2_0_20, corrected_v2_20_40, corrected_v2_40_60;
    std::vector<double> v2_0_20_Errors, v2_20_40_Errors, v2_40_60_Errors, bg_v2_0_20_Errors, bg_v2_20_40_Errors, bg_v2_40_60_Errors, bg_v2_left_0_20_Errors, bg_v2_left_20_40_Errors, bg_v2_left_40_60_Errors, corrected_v2_0_20_Errors, corrected_v2_20_40_Errors, corrected_v2_40_60_Errors;
    double v2, v2_error, bg_v2, bg_v2_error, bg_v2_left, bg_v2_error_left, v2_corrected, v2_error_corrected;
    
    
    std::vector<double> v3_0_20, v3_20_40, v3_40_60, bg_v3_0_20, bg_v3_20_40, bg_v3_40_60, bg_v3_left_0_20, bg_v3_left_20_40, bg_v3_left_40_60, corrected_v3_0_20, corrected_v3_20_40, corrected_v3_40_60; // v3 values for different centralities
    std::vector<double> v3_0_20_Errors, v3_20_40_Errors, v3_40_60_Errors, bg_v3_0_20_Errors, bg_v3_20_40_Errors, bg_v3_40_60_Errors, bg_v3_left_0_20_Errors, bg_v3_left_20_40_Errors, bg_v3_left_40_60_Errors, corrected_v3_0_20_Errors, corrected_v3_20_40_Errors, corrected_v3_40_60_Errors; // Errors for v3 values
    double v3, v3_error, bg_v3, bg_v3_error, bg_v3_left, bg_v3_error_left, v3_corrected, v3_error_corrected;

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

        // Converting string to double for v2 and its error
        v2 = std::stod(rowData.at(rowData.size() - 16));
        v2_error = std::stod(rowData.at(rowData.size() - 15));
        // Converting string to double for v2 and its error
        bg_v2 = std::stod(rowData.at(rowData.size() - 14));
        bg_v2_error = std::stod(rowData.at(rowData.size() - 13));
        // Converting string to double for v2 and its error
        bg_v2_left = std::stod(rowData.at(rowData.size() - 12));
        bg_v2_error_left = std::stod(rowData.at(rowData.size() - 11));
        
        v2_corrected = std::stod(rowData.at(rowData.size() - 10));
        v2_error_corrected = std::stod(rowData.at(rowData.size() - 9));
        
        
        
        
        // Converting string to double for v2 and its error
        v3 = std::stod(rowData.at(rowData.size() - 8)); // 8th last column is v3
        v3_error = std::stod(rowData.at(rowData.size() - 7)); // 7th last column is v3
        // Converting string to double for v2 and its error
        bg_v3 = std::stod(rowData.at(rowData.size() - 6)); // 6th last column is v3
        bg_v3_error = std::stod(rowData.at(rowData.size() - 5)); // 5th last column is v3 error
        bg_v3_left = std::stod(rowData.at(rowData.size() - 4)); // 6th last column is v3
        bg_v3_error_left = std::stod(rowData.at(rowData.size() - 3)); // 5th last column is v3 error
        v3_corrected = std::stod(rowData.at(rowData.size() - 2)); // Second last column is v3 corrected
        v3_error_corrected = std::stod(rowData.back()); // Last column is v2 corrected error

        // Mapping index to centrality and pT
        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            // 40-60% centrality
            v2_40_60.push_back(v2);
            v2_40_60_Errors.push_back(v2_error);
            
            bg_v2_40_60.push_back(bg_v2);
            bg_v2_40_60_Errors.push_back(bg_v2_error);
            
            bg_v2_left_40_60.push_back(bg_v2_left);
            bg_v2_left_40_60_Errors.push_back(bg_v2_error_left);
            
            corrected_v2_40_60.push_back(v2_corrected);
            corrected_v2_40_60_Errors.push_back(v2_error_corrected);
            
            // 40-60% centrality
            v3_40_60.push_back(v3);
            v3_40_60_Errors.push_back(v3_error);
            
            bg_v3_40_60.push_back(bg_v3);
            bg_v3_40_60_Errors.push_back(bg_v3_error);
            
            bg_v3_left_40_60.push_back(bg_v3_left);
            bg_v3_left_40_60_Errors.push_back(bg_v3_error_left);
            
            corrected_v3_40_60.push_back(v3_corrected);
            corrected_v3_40_60_Errors.push_back(v3_error_corrected);
            
            
        } else if (index >= 6 && index <= 11) {
            // 20-40% centrality
            v2_20_40.push_back(v2);
            v2_20_40_Errors.push_back(v2_error);

            bg_v2_20_40.push_back(bg_v2);
            bg_v2_20_40_Errors.push_back(bg_v2_error);
            
            bg_v2_left_20_40.push_back(bg_v2_left);
            bg_v2_left_20_40_Errors.push_back(bg_v2_error_left);
            
            corrected_v2_20_40.push_back(v2_corrected);
            corrected_v2_20_40_Errors.push_back(v2_error_corrected);
            
            // 20-40% centrality
            v3_20_40.push_back(v3);
            v3_20_40_Errors.push_back(v3_error);

            bg_v3_20_40.push_back(bg_v3);
            bg_v3_20_40_Errors.push_back(bg_v3_error);
            
            bg_v3_left_20_40.push_back(bg_v3_left);
            bg_v3_left_20_40_Errors.push_back(bg_v3_error_left);
            
            corrected_v3_20_40.push_back(v3_corrected);
            corrected_v3_20_40_Errors.push_back(v3_error_corrected);
            
            
        } else if (index >= 12 && index <= 17) {
            // 0-20% centrality
            v2_0_20.push_back(v2);
            v2_0_20_Errors.push_back(v2_error);
            
            bg_v2_0_20.push_back(bg_v2);
            bg_v2_0_20_Errors.push_back(bg_v2_error);
            
            bg_v2_left_0_20.push_back(bg_v2_left);
            bg_v2_left_0_20_Errors.push_back(bg_v2_error_left);
            PrintVectorContents(bg_v2_left_0_20, bg_v2_left_0_20_Errors, "bg_v2_left_0_20");
            
            corrected_v2_0_20.push_back(v2_corrected);
            corrected_v2_0_20_Errors.push_back(v2_error_corrected);
            
            // 0-20% centrality
            v3_0_20.push_back(v3);
            v3_0_20_Errors.push_back(v3_error);
            
            bg_v3_0_20.push_back(bg_v3);
            bg_v3_0_20_Errors.push_back(bg_v3_error);
            
            bg_v3_left_0_20.push_back(bg_v3_left);
            bg_v3_left_0_20_Errors.push_back(bg_v3_error_left);
            
            corrected_v3_0_20.push_back(v3_corrected);
            corrected_v3_0_20_Errors.push_back(v3_error_corrected);
        }
    }

    // Close the file as we have finished reading the data
    file.close();
    
    /*
     Plot Measured v2 for 0-20% centrality
     */
    TCanvas *c1 = new TCanvas("c1", "v2 vs pT 0-20%", 800, 600);
    TGraphErrors *graph_0_20_v2 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v2_0_20[0], 0, &v2_0_20_Errors[0]);
    graph_0_20_v2->SetMarkerColor(kBlue);
    graph_0_20_v2->SetMarkerStyle(21);
    graph_0_20_v2->SetMarkerSize(1.1);
    graph_0_20_v2->SetTitle("Diphoton #it{v}_{2} vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    graph_0_20_v2->Draw("AP");
    graph_0_20_v2->GetXaxis()->SetLimits(2.0, 5.0);
    graph_0_20_v2->GetXaxis()->SetNdivisions(010);
    graph_0_20_v2->SetMinimum(0.0); // Set the minimum y value
    graph_0_20_v2->SetMaximum(0.25); // Set the maximum y value

    c1->SetTicks();
    //Output cut information
    TLatex latex020;
    latex020.SetTextSize(0.03);
    latex020.SetTextAlign(12);
    latex020.SetNDC();
    latex020.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex020.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex020.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex020.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    latex020.DrawLatex(0.13, 0.7, "#bf{Mass Range of Calculation: (#mu - 2*#sigma, #mu + 2*#sigma) GeV}");
    c1->Modified();
    c1->Update();
    c1->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/0_20/v2_measured_vs_pT_0_20.png");

    
    
    /*
     Plot Measured v2 for 20-40% centrality
     */
    TCanvas *c2 = new TCanvas("c2", "v2 vs pT 20-40%", 800, 600);
    TGraphErrors *graph_20_40_v2 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v2_20_40[0], 0, &v2_20_40_Errors[0]);
    graph_20_40_v2->SetMarkerColor(kBlue);
    graph_20_40_v2->SetMarkerStyle(21);
    graph_20_40_v2->SetMarkerSize(1.1);
    graph_20_40_v2->SetTitle("Diphoton #it{v}_{2} vs #it{p}_{T} 20-40% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    graph_20_40_v2->Draw("AP");
    graph_20_40_v2->GetXaxis()->SetLimits(2.0, 5.0);
    graph_20_40_v2->GetXaxis()->SetNdivisions(010);
    graph_20_40_v2->SetMinimum(0.0); // Set the minimum y value
    graph_20_40_v2->SetMaximum(0.5); // Set the maximum y value
    c2->SetTicks();
    TLatex latex2040;
    latex2040.SetTextSize(0.03);
    latex2040.SetTextAlign(12);
    latex2040.SetNDC();
    latex2040.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex2040.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex2040.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex2040.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    c2->Modified();
    c2->Update();
    c2->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/20_40/v2_measured_vs_pT_20_40.png");

    
    
    /*
     Plot Measured v2 for 0-20% centrality
     */
    TCanvas *c3 = new TCanvas("c3", "v2 vs pT 40-60%", 800, 600);
    TGraphErrors *graph_40_60_v2 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v2_40_60[0], 0, &v2_40_60_Errors[0]);
    graph_40_60_v2->SetMarkerColor(kBlue);
    graph_40_60_v2->SetMarkerStyle(21);
    graph_40_60_v2->SetMarkerSize(1.1);
    graph_40_60_v2->SetTitle("Diphoton #it{v}_{2} vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    graph_40_60_v2->Draw("AP");
    graph_40_60_v2->GetXaxis()->SetLimits(2.0, 5.0);
    graph_40_60_v2->GetXaxis()->SetNdivisions(010);
    graph_40_60_v2->SetMinimum(0.0); // Set the minimum y value
    graph_40_60_v2->SetMaximum(0.25); // Set the maximum y value
    graph_40_60_v2->SetMinimum(-0.2); // Set the minimum y value
    graph_40_60_v2->SetMaximum(0.7); // Set the maximum y value
    c3->SetTicks();
    TLatex latex4060;
    latex4060.SetTextSize(0.03);
    latex4060.SetTextAlign(12);
    latex4060.SetNDC();
    latex2040.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex2040.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex2040.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex2040.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    c3->Modified();
    c3->Update();
    c3->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/40_60/v2_measured_vs_pT_40_60.png");
    
    
    
    
    /*
     Plot RIGHT OF BACKGROUND v2 for 0 - 20 percent centrality
     */
    TCanvas *c4 = new TCanvas("c4", "Background v2 vs pT 0-20%", 800, 600);
    TGraphErrors *bg_graph_0_20_v2 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &bg_v2_0_20[0], 0, &bg_v2_0_20_Errors[0]);
    bg_graph_0_20_v2->SetMarkerColor(kBlue);
    bg_graph_0_20_v2->SetMarkerStyle(21);
    bg_graph_0_20_v2->SetMarkerSize(1.1);
    bg_graph_0_20_v2->SetTitle("Background #it{v}_{2} vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    bg_graph_0_20_v2->Draw("AP");
    
    bg_graph_0_20_v2->GetXaxis()->SetLimits(2.0, 5.0);
    bg_graph_0_20_v2->GetXaxis()->SetNdivisions(010);
    bg_graph_0_20_v2->SetMinimum(0); // Set the minimum y value
    bg_graph_0_20_v2->SetMaximum(0.3); // Set the maximum y value
    c4->SetTicks();
    TLatex latex020_bg;
    latex020_bg.SetTextSize(0.03);
    latex020_bg.SetTextAlign(12);
    latex020_bg.SetNDC();
    latex020_bg.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex020_bg.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex020_bg.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex020_bg.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    latex020_bg.DrawLatex(0.13, 0.7, "#bf{Mass Range of Calculation: (#mu + 3*#sigma, 0.5) GeV}");
    
    c4->Update();
    // Draw a dashed line at y = 0
    double x_min_bg_0_20_v2 = c4->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_bg_0_20_v2 = c4->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_0_20_bg_v2 = new TLine(x_min_bg_0_20_v2, 0, x_max_bg_0_20_v2, 0);
    zeroLine_0_20_bg_v2->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_0_20_bg_v2->Draw("SAME"); // Draw the line on the same canvas as your plot

    
    
    c4->Modified();
    c4->Update();
    c4->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/0_20/bg_v2_vs_pT_0_20.png");

    
    
    /*
     OVERLAY BACKGROUND v2 TO LEFT AND RIGHT OF SIGNAL REGION for 0-20%
     */
    TCanvas *c_0_20_Left_Right_bg_Overlay_v2 = new TCanvas("c_0_20_Left_Right_bg_Overlay_v2", "Sideband #v_2 Overlay, 0-20% Centrality", 800, 600);
    TGraphErrors *graph_0_20_bg_left_v2 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &bg_v2_left_0_20[0], 0, &bg_v2_left_0_20_Errors[0]);
    
    graph_0_20_bg_left_v2->SetMarkerStyle(20); // Circle
    graph_0_20_bg_left_v2->SetMarkerColor(kRed); // Changed to red for visibility
    graph_0_20_bg_left_v2->SetLineColor(kRed);
    // Draw the right background graph as the base graph
    bg_graph_0_20_v2->Draw("AP");
    
    // Apply jitter to x values for graph_40_50 and graph_50_60
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_0_20_bg_left_v2->SetPoint(i, ptCenters[i] - .06, bg_v2_left_0_20[i]);
    }
    
    bg_graph_0_20_v2->SetTitle("Sideband #it{v}_{2} to Left and Right of Signal Region vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    bg_graph_0_20_v2->SetMinimum(0.0); // Set the minimum y value
    bg_graph_0_20_v2->SetMaximum(0.25); // Set the maximum y value
    graph_0_20_bg_left_v2->Draw("P SAME");

    // Create a legend to distinguish between the two graphs
    TLegend *legend_leftRightbg_overlay_0_20_v2 = new TLegend(0.11, 0.11, 0.31, 0.31);
    legend_leftRightbg_overlay_0_20_v2->SetBorderSize(0);
    legend_leftRightbg_overlay_0_20_v2->SetTextSize(0.028);
    legend_leftRightbg_overlay_0_20_v2->AddEntry(bg_graph_0_20_v2, "0-20%, v_{2}^{bg} right of signal region in range (#mu + 3*#sigma, 0.5) GeV", "p");
    legend_leftRightbg_overlay_0_20_v2->AddEntry(graph_0_20_bg_left_v2, "0-20%, v_{2}^{bg} left of signal region in range (0, #mu - 2*#sigma) GeV", "p");
    legend_leftRightbg_overlay_0_20_v2->Draw();
    
    c_0_20_Left_Right_bg_Overlay_v2->Modified();
    c_0_20_Left_Right_bg_Overlay_v2->Update();
    c_0_20_Left_Right_bg_Overlay_v2->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/0_20/bg_v2_Left_Right_Overlay_0_20.png");

    
    
    
    /*
     Plot RIGHT OF BACKGROUND v2 for 20 - 40 percent centrality
     */
    TCanvas *c5 = new TCanvas("c5", "Background v2 vs pT 20-40%", 800, 600);
    TGraphErrors *bg_graph_20_40_v2 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &bg_v2_20_40[0], 0, &bg_v2_20_40_Errors[0]);
    bg_graph_20_40_v2->SetMarkerColor(kBlue);
    bg_graph_20_40_v2->SetMarkerStyle(21);
    bg_graph_20_40_v2->SetMarkerSize(1.1);
    bg_graph_20_40_v2->SetTitle("Background #it{v}_{2} vs #it{p}_{T} 20-40% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    bg_graph_20_40_v2->Draw("AP");
    bg_graph_20_40_v2->GetXaxis()->SetLimits(2.0, 5.0);
    bg_graph_20_40_v2->GetXaxis()->SetNdivisions(010);
    bg_graph_20_40_v2->SetMinimum(0.0); // Set the minimum y value
    bg_graph_20_40_v2->SetMaximum(0.5); // Set the maximum y value
    c5->SetTicks();
    
    //Output cut information
    TLatex latex2040_bg;
    latex2040_bg.SetTextSize(0.03);
    latex2040_bg.SetTextAlign(12);
    latex2040_bg.SetNDC();

    latex2040_bg.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex2040_bg.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex2040_bg.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex2040_bg.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");

    
    c5->Modified();
    c5->Update();
    c5->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/20_40/bg_v2_vs_pT_20_40.png");

    
    /*
     OVERLAY BACKGROUND v2 TO LEFT AND RIGHT OF SIGNAL REGION for 20-40%
     */
    TCanvas *c_20_40_Left_Right_bg_Overlay_v2 = new TCanvas("c_20_40_Left_Right_bg_Overlay_v2", "Sideband #v_2 Overlay, 20-40% Centrality", 800, 600);
    TGraphErrors *graph_20_40_bg_left_v2 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &bg_v2_left_20_40[0], 0, &bg_v2_left_20_40_Errors[0]);
    
    graph_20_40_bg_left_v2->SetMarkerStyle(20); // Circle
    graph_20_40_bg_left_v2->SetMarkerColor(kRed); // Changed to red for visibility
    graph_20_40_bg_left_v2->SetLineColor(kRed);
    // Draw the right background graph as the base graph
    bg_graph_20_40_v2->Draw("AP");
    
    // Apply jitter to x values for graph_40_50 and graph_50_60
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_20_40_bg_left_v2->SetPoint(i, ptCenters[i] - .06, bg_v2_left_20_40[i]);
    }
    
    bg_graph_20_40_v2->SetTitle("Sideband #it{v}_{2} to Left and Right of Signal Region vs #it{p}_{T} 20-40% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    bg_graph_20_40_v2->SetMinimum(0); // Set the minimum y value
    bg_graph_20_40_v2->SetMaximum(0.55); // Set the maximum y value
    graph_20_40_bg_left_v2->Draw("P SAME");

    // Create a legend to distinguish between the two graphs
    TLegend *legend_leftRightbg_overlay_20_40_v2 = new TLegend(0.12, 0.65, 0.32, 0.85);
    legend_leftRightbg_overlay_20_40_v2->SetBorderSize(0);
    legend_leftRightbg_overlay_20_40_v2->SetTextSize(0.024);
    legend_leftRightbg_overlay_20_40_v2->AddEntry(bg_graph_20_40_v2, "20-40%, v_{2}^{bg} right of signal region in range (#mu + 3*#sigma, 0.5) GeV", "p");
    legend_leftRightbg_overlay_20_40_v2->AddEntry(graph_20_40_bg_left_v2, "20-40%, v_{2}^{bg} left of signal region in range (0, #mu - 2*#sigma) GeV", "p");
    legend_leftRightbg_overlay_20_40_v2->Draw();

    c_20_40_Left_Right_bg_Overlay_v2->Modified();
    c_20_40_Left_Right_bg_Overlay_v2->Update();
    c_20_40_Left_Right_bg_Overlay_v2->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/20_40/bg_v2_Left_Right_Overlay_20_40.png");

    
    
    
    
    /*
     Plot RIGHT OF BACKGROUND v2 for 40 - 60 percent centrality
     */
    TCanvas *c6 = new TCanvas("c3", "Background v2 vs pT 40-60%", 800, 600);
    TGraphErrors *bg_graph_40_60_v2 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &bg_v2_40_60[0], 0, &bg_v2_40_60_Errors[0]);
    bg_graph_40_60_v2->SetMarkerColor(kBlue);
    bg_graph_40_60_v2->SetMarkerStyle(21);
    bg_graph_40_60_v2->SetMarkerSize(1.1);
    bg_graph_40_60_v2->SetTitle("Background #it{v}_{2} vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    bg_graph_40_60_v2->Draw("AP");
    bg_graph_40_60_v2->GetXaxis()->SetLimits(2.0, 5.0);
    bg_graph_40_60_v2->GetXaxis()->SetNdivisions(010);
    bg_graph_40_60_v2->SetMinimum(0.0); // Set the minimum y value
    bg_graph_40_60_v2->SetMaximum(0.25); // Set the maximum y value
    bg_graph_40_60_v2->SetMinimum(-0.2); // Set the minimum y value
    bg_graph_40_60_v2->SetMaximum(0.7); // Set the maximum y value
    c6->SetTicks();
    TLatex latex4060_bg;
    latex4060_bg.SetTextSize(0.03);
    latex4060_bg.SetTextAlign(12);
    latex4060_bg.SetNDC();
    latex4060_bg.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex4060_bg.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex4060_bg.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex4060_bg.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    c6->Modified();
    c6->Update();
    c6->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/40_60/bg_v2_vs_pT_40_60.png");
    
    
    /*
     OVERLAY BACKGROUND v2 TO LEFT AND RIGHT OF SIGNAL REGION for 40-60%
     */
    TCanvas *c_40_60_Left_Right_bg_Overlay_v2 = new TCanvas("c_40_60_Left_Right_bg_Overlay_v2", "Sideband #v_2 Overlay, 40-60% Centrality", 800, 600);
    TGraphErrors *graph_40_60_bg_left_v2 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &bg_v2_left_40_60[0], 0, &bg_v2_left_40_60_Errors[0]);
    
    graph_40_60_bg_left_v2->SetMarkerStyle(20); // Circle
    graph_40_60_bg_left_v2->SetMarkerColor(kRed); // Changed to red for visibility
    graph_40_60_bg_left_v2->SetLineColor(kRed);
    // Draw the right background graph as the base graph
    bg_graph_40_60_v2->Draw("AP");
    
    // Apply jitter to x values for graph_40_50 and graph_50_60
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_40_60_bg_left_v2->SetPoint(i, ptCenters[i] - .06, bg_v2_left_40_60[i]);
    }
    
    bg_graph_40_60_v2->SetTitle("Sideband #it{v}_{2} to Left and Right of Signal Region vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    bg_graph_40_60_v2->SetMinimum(-0.2); // Set the minimum y value
    bg_graph_40_60_v2->SetMaximum(.75); // Set the maximum y value
    graph_40_60_bg_left_v2->Draw("P SAME");

    // Create a legend to distinguish between the two graphs
    TLegend *legend_leftRightbg_overlay_40_60_v2 = new TLegend(0.11, 0.69, 0.31, 0.89);
    legend_leftRightbg_overlay_40_60_v2->SetBorderSize(0);
    legend_leftRightbg_overlay_40_60_v2->SetTextSize(0.03);
    legend_leftRightbg_overlay_40_60_v2->AddEntry(bg_graph_40_60_v2, "40-60%, v_{2}^{bg} right of signal region in range (#mu + 3*#sigma, 0.5) GeV", "p");
    legend_leftRightbg_overlay_40_60_v2->AddEntry(graph_40_60_bg_left_v2, "40-60%, v_{2}^{bg} left of signal region in range (0, #mu - 2*#sigma) GeV", "p");
    legend_leftRightbg_overlay_40_60_v2->Draw();

    c_40_60_Left_Right_bg_Overlay_v2->Modified();
    c_40_60_Left_Right_bg_Overlay_v2->Update();
    c_40_60_Left_Right_bg_Overlay_v2->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/40_60/bg_v2_Left_Right_Overlay_40_60.png");

    
    
    
    
    /*
     Plot Corrected v2 By itself for 0-20 centrality
     */
    TCanvas *c7 = new TCanvas("c7", "Corrected v2 vs pT 0-20%", 800, 600);
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
    c7->SetTicks();
    TLatex latex020_correct;
    latex020_correct.SetTextSize(0.03);
    latex020_correct.SetTextAlign(12);
    latex020_correct.SetNDC();
    latex020_correct.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex020_correct.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex020_correct.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex020_correct.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    
    c7->Update();
    // Draw a dashed line at y = 0
    double x_min_0_20_corr_v2 = c7->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_0_20_corr_v2 = c7->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_0_20_corr_v2 = new TLine(x_min_0_20_corr_v2, 0, x_max_0_20_corr_v2, 0);
    zeroLine_0_20_corr_v2->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_0_20_corr_v2->Draw("SAME"); // Draw the line on the same canvas as your plot

    
    c7->Modified();
    c7->Update();
    c7->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/0_20/Corrected_v2_vs_pT_0_20_correctedPointsOnly.png");

    
    /*
     Plot Corrected v2 By itself for 20-40 centrality
     */

    TCanvas *c8 = new TCanvas("c8", "Corrected v2 vs pT 20-40%", 800, 600);
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
    c8->SetTicks();
    TLatex latex2040_corr;
    latex2040_corr.SetTextSize(0.03);
    latex2040_corr.SetTextAlign(12);
    latex2040_corr.SetNDC();
    latex2040_corr.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex2040_corr.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex2040_corr.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex2040_corr.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    
    c8->Update();
    // Draw a dashed line at y = 0
    double x_min_20_40_corr_v2 = c8->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_20_40_corr_v2 = c8->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_20_40_corr_v2 = new TLine(x_min_20_40_corr_v2, 0, x_max_20_40_corr_v2, 0);
    zeroLine_20_40_corr_v2->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_20_40_corr_v2->Draw("SAME"); // Draw the line on the same canvas as your plot

    c8->Modified();
    c8->Update();
    c8->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/20_40/Corrected_v2_vs_pT_20_40_correctedPointsOnly.png.png");

    /*
     Plot Corrected v2 By itself for 40-60 centrality
     */
    TCanvas *c9 = new TCanvas("c9", "Corrected v2 vs pT 40-60%", 800, 600);
    TGraphErrors *corrected_graph_40_60_v2 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &corrected_v2_40_60[0], 0, &corrected_v2_40_60_Errors[0]);
    corrected_graph_40_60_v2->SetMarkerColor(kBlue);
    corrected_graph_40_60_v2->SetMarkerStyle(21);
    corrected_graph_40_60_v2->SetMarkerSize(1.1);
    corrected_graph_40_60_v2->SetTitle("Corrected #it{v}_{2} vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    corrected_graph_40_60_v2->Draw("AP");
    corrected_graph_40_60_v2->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_graph_40_60_v2->GetXaxis()->SetNdivisions(010);
    corrected_graph_40_60_v2->SetMinimum(0.0); // Set the minimum y value
    corrected_graph_40_60_v2->SetMaximum(0.25); // Set the maximum y value
    corrected_graph_40_60_v2->SetMinimum(-0.2); // Set the minimum y value
    corrected_graph_40_60_v2->SetMaximum(0.7); // Set the maximum y value
    c9->SetTicks();
    TLatex latex4060_corr;
    latex4060_corr.SetTextSize(0.03);
    latex4060_corr.SetTextAlign(12);
    latex4060_corr.SetNDC();
    latex4060_corr.DrawLatex(0.3, 0.86, "Cuts (Inclusive):");
    latex4060_corr.DrawLatex(0.3, 0.82, "#bf{Asymmetry < 0.5}");
    latex4060_corr.DrawLatex(0.3, 0.78, "#bf{#chi^{2} < 4.0}");
    latex4060_corr.DrawLatex(0.3, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    
    c9->Update();
    // Draw a dashed line at y = 0
    double x_min_40_60_corr_v2 = c9->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_40_60_corr_v2 = c9->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_40_60_corr_v2 = new TLine(x_min_40_60_corr_v2, 0, x_max_40_60_corr_v2, 0);
    zeroLine_40_60_corr_v2->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_40_60_corr_v2->Draw("SAME"); // Draw the line on the same canvas as your plot
    
    c9->Modified();
    c9->Update();
    c9->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/40_60/Corrected_v2_vs_pT_40_60_correctedPointsOnly.png");
    
    
    
    // Define the offsets for jitter
    double offset = 0.06;
    

    
    /*
     Read in Data from PHENIX CSV to plot overlay v2 with PHENIX 0-10 and 10-20
     */
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

    
    // Define the marker styles and colors for clarity
    int markerStyle_sPHENIX = 21; // Square for 40-60%
    int markerStyle_PHENIX = 20; // Circle for 40-50%

    // Define colors for each centrality range
    int color_sPHENIX = kBlue;    // Red for 40-60%
    int color_PHENIX_low = kRed;   // Blue for 40-50%
    int color_PHENIX_high= kGreen+3;// Dark Green for 50-60%

    /*
     Overlay measured v2 data with PHENIX pi0 data--0 to 20 percent centrality
     */
    TCanvas *c10 = new TCanvas("c10", "Overlay 0-20%", 800, 600);
    graph_0_20_v2->Draw("AP");  // This will be the base graph
    TGraphErrors *graph_0_10 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v2_0_10[0], 0, &v2_10_20_Errors[0]);
    TGraphErrors *graph_10_20 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v2_0_10[0], 0, &v2_10_20_Errors[0]);
    
    // Apply jitter to x values for graph_40_50 and graph_50_60
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_0_10->SetPoint(i, ptCenters[i] - offset, v2_0_10[i]);
        graph_10_20->SetPoint(i, ptCenters[i] + offset, v2_10_20[i]);
    }
    graph_0_20_v2->SetMarkerColor(color_sPHENIX);
    graph_0_20_v2->SetMarkerStyle(markerStyle_sPHENIX);
    graph_0_20_v2->SetMinimum(0); // Set the minimum y value
    graph_0_20_v2->SetMaximum(0.26); // Set the maximum y value
    
    graph_0_10->SetMarkerColor(color_PHENIX_low);
    graph_0_10->SetMarkerStyle(markerStyle_PHENIX);
    graph_10_20->SetMarkerColor(color_PHENIX_high);
    graph_10_20->SetMarkerStyle(markerStyle_PHENIX);

    graph_0_10->Draw("P SAME");
    graph_10_20->Draw("P SAME");

    printOverlayData(ptCenters,
                     v2_0_10, v2_0_10_Errors,
                     v2_10_20, v2_10_20_Errors,
                     v2_0_20, v2_0_20_Errors);

    TLegend *legend1 = new TLegend(0.11, 0.69, 0.31, 0.89);
    legend1->SetBorderSize(0);
    legend1->SetTextSize(0.028);
    legend1->AddEntry(graph_0_20_v2, "0-20%, #bf{sPHENIX}", "p");
    legend1->AddEntry(graph_0_10, "0-10%, #bf{PHENIX} 2010", "p");
    legend1->AddEntry(graph_10_20, "10-20%, #bf{PHENIX} 2010", "p");
    legend1->Draw();

    c10->Modified();
    c10->Update();
    c10->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/0_20/v2_vs_pT_0_20_Overlay.png");

    
    
    
    /*
     Overlay 20-40 measured v2 data with PHENIX 20-30 and 30-40
     */
    TCanvas *c11 = new TCanvas("c11", "Overlay 20-40%", 800, 600);
    graph_20_40_v2->Draw("AP");
    TGraphErrors *graph_20_30 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v2_20_30[0], 0, &v2_20_30_Errors[0]);
    TGraphErrors *graph_30_40 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v2_30_40[0], 0, &v2_30_40_Errors[0]);
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_20_30->SetPoint(i, ptCenters[i] - offset, v2_20_30[i]);
        graph_30_40->SetPoint(i, ptCenters[i] + offset, v2_30_40[i]);
    }
    graph_20_40_v2->SetMarkerColor(color_sPHENIX);
    graph_20_40_v2->SetLineColor(color_sPHENIX);
    graph_20_40_v2->SetMarkerStyle(markerStyle_sPHENIX);
    graph_20_40_v2->SetMinimum(0); // Set the minimum y value
    graph_20_40_v2->SetMaximum(0.45); // Set the maximum y value
    
    graph_20_30->SetMarkerColor(color_PHENIX_low);
    graph_20_30->SetMarkerStyle(markerStyle_PHENIX);
    graph_30_40->SetMarkerColor(color_PHENIX_high);
    graph_30_40->SetMarkerStyle(markerStyle_PHENIX);
    
    graph_20_30->Draw("P SAME");
    graph_30_40->Draw("P SAME");
    printOverlayData(ptCenters,
                     v2_20_30, v2_20_30_Errors,
                     v2_30_40, v2_30_40_Errors,
                     v2_20_40, v2_20_40_Errors);

    TLegend *legend2 = new TLegend(0.11, 0.69, 0.31, 0.89);
    legend2->SetBorderSize(0);
    legend2->SetTextSize(0.028);
    legend2->AddEntry(graph_20_40_v2, "20-40%, #bf{sPHENIX}", "p");
    legend2->AddEntry(graph_20_30, "20-30%, #bf{PHENIX} 2010", "p");
    legend2->AddEntry(graph_30_40, "30-40%, #bf{PHENIX} 2010", "p");
    legend2->Draw();

    c11->Modified();
    c11->Update();
    c11->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/20_40/v2_vs_pT_20_40_Overlay.png");
    
    
    
    

    /*
     Overlay 40-60 measured v2 data with PHENIX 40-50 and 50-60
     */
    TCanvas *c12 = new TCanvas("c12", "Overlay 40-60%", 800, 600);
    graph_40_60_v2->Draw("AP");  // This will be the base graph
    TGraphErrors *graph_40_50 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v2_40_50[0], 0, &v2_40_50_Errors[0]);
    TGraphErrors *graph_50_60 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v2_50_60[0], 0, &v2_50_60_Errors[0]);
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_40_50->SetPoint(i, ptCenters[i] - offset, v2_40_50[i]);
        graph_50_60->SetPoint(i, ptCenters[i] + offset, v2_50_60[i]);
    }
    graph_40_60_v2->SetMarkerColor(color_sPHENIX);
    graph_40_60_v2->SetLineColor(color_sPHENIX);
    graph_40_60_v2->SetMarkerStyle(markerStyle_sPHENIX);
    graph_40_50->SetMarkerColor(color_PHENIX_low);
    graph_40_50->SetMarkerStyle(markerStyle_PHENIX);
    graph_50_60->SetMarkerColor(color_PHENIX_high);
    graph_50_60->SetMarkerStyle(markerStyle_PHENIX);
    graph_40_60_v2->SetMinimum(0); // Set the minimum y value
    graph_40_60_v2->SetMaximum(0.6); // Set the maximum y value
    graph_40_50->Draw("P SAME");
    graph_50_60->Draw("P SAME");
    printOverlayData(ptCenters,
                     v2_40_50, v2_40_50_Errors,
                     v2_50_60, v2_50_60_Errors,
                     v2_40_60, v2_40_60_Errors);
    TLegend *legend3 = new TLegend(0.11, 0.69, 0.31, 0.89);
    legend3->SetBorderSize(0);
    legend3->SetTextSize(0.028);
    legend3->AddEntry(graph_40_60_v2, "40-60%, #bf{sPHENIX}", "p");
    legend3->AddEntry(graph_40_50, "40-50%, #bf{PHENIX} 2010", "p");
    legend3->AddEntry(graph_50_60, "50-60%, #bf{PHENIX} 2010", "p");
    legend3->Draw();
    c12->Modified();
    c12->Update();
    c12->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/40_60/v2_vs_pT_40_60_Overlay.png");
    

    
    
    
    /*
     Overlay Just the Corrected v2 data for 0-20 percent centrality with the PHENIX DATA
     */
    TCanvas *c13 = new TCanvas("c13", "Corrected Overlay 0-20%", 800, 600);
    corrected_graph_0_20_v2->Draw("AP");  // This will be the base graph

    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_0_10->SetPoint(i, ptCenters[i] - offset, v2_0_10[i]);
        graph_10_20->SetPoint(i, ptCenters[i] + offset, v2_10_20[i]);
    }

    corrected_graph_0_20_v2->SetMarkerColor(color_sPHENIX);
    corrected_graph_0_20_v2->SetMarkerStyle(markerStyle_sPHENIX);

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
    TLegend *legend4 = new TLegend(0.5, 0.7, 0.9, 0.9);
    legend4->AddEntry(graph_0_20_v2, "0-20%, #bf{sPHENIX}", "p");
    legend4->AddEntry(graph_0_10, "0-10%, #bf{PHENIX} 2010", "p");
    legend4->AddEntry(graph_10_20, "10-20%, #bf{PHENIX} 2010", "p");
    legend4->Draw();
    c13->Modified();
    c13->Update();
    c13->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/0_20/v2_vs_pT_0_20_Overlay_corrected.png");

    
    
    /*
     Overlay Just the Corrected v2 data for 20-40 percent centrality with the PHENIX DATA
     */
    TCanvas *c14 = new TCanvas("c14", "Corrected Overlay 20-40%", 800, 600);
    corrected_graph_20_40_v2->Draw("AP");  // This will be the base graph
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_20_30->SetPoint(i, ptCenters[i] - offset, v2_20_30[i]);
        graph_30_40->SetPoint(i, ptCenters[i] + offset, v2_30_40[i]);
    }
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
    TLegend *legend5 = new TLegend(0.5, 0.7, 0.9, 0.9);
    legend5->AddEntry(graph_20_40_v2, "20-40%, #bf{sPHENIX}", "p");
    legend5->AddEntry(graph_20_30, "20-30%, #bf{PHENIX} 2010", "p");
    legend5->AddEntry(graph_30_40, "30-40%, #bf{PHENIX} 2010", "p");
    legend5->Draw();

    c14->Modified();
    c14->Update();
    c14->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/20_40/v2_vs_pT_20_40_Overlay_corrected.png");
    
    
    
    

    /*
     Overlay Just the Corrected v2 data for 40-60 percent centrality with the PHENIX DATA
     */
    TCanvas *c15 = new TCanvas("c15", "Corrected Overlay 40-60%", 800, 600);
    corrected_graph_40_60_v2->Draw("AP");  // This will be the base graph

    
    // Apply jitter to x values for graph_40_50 and graph_50_60
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_40_50->SetPoint(i, ptCenters[i] - offset, v2_40_50[i]);
        graph_50_60->SetPoint(i, ptCenters[i] + offset, v2_50_60[i]);
    }
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

    TLegend *legend6 = new TLegend(0.5, 0.7, 0.9, 0.9);
    legend6->SetBorderSize(0);
    legend6->SetTextSize(0.024);
    legend6->AddEntry(graph_40_60_v2, "40-60%, #bf{sPHENIX}", "p");
    legend6->AddEntry(graph_40_50, "40-50%, #bf{PHENIX} 2010", "p");
    legend6->AddEntry(graph_50_60, "50-60%, #bf{PHENIX} 2010", "p");
    legend6->Draw();

    c15->Modified();
    c15->Update();
    c15->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/40_60/v2_vs_pT_40_60_Overlay_corrected.png");

    
    
    /*
     Overlay with PHENIX, measured v2, and corrected v2, for 0-20 percent centrality
     */
    TCanvas *c_v2_corr_unc_overlay_w_phenix0_20 = new TCanvas("c_v2_corr_unc_overlay_w_phenix0_20", "Overlay 0-20% Corr/UnCorr", 800, 600);
    corrected_graph_0_20_v2->Draw("AP");  // This will be the base graph
    double offsetNew = 0.03;
    // Jitter points for clarity
    for (int i = 0; i < ptCenters.size(); ++i) {
        double pt = ptCenters[i];
        graph_0_10->SetPoint(i, pt - offsetNew * 3.5, v2_0_10[i]); // Shift leftmost
        graph_0_20_v2->SetPoint(i, pt - offsetNew * 1.3, v2_0_20[i]); // Shift slightly left
        graph_10_20->SetPoint(i, pt + offsetNew * 3.5, v2_10_20[i]); // Shift slightly right
    }
    graph_0_20_v2->SetMarkerStyle(21); // square for uncorrected
    graph_0_20_v2->SetMarkerColor(kBlue);
    graph_0_20_v2->SetLineColor(kBlue);
    corrected_graph_0_20_v2->SetMarkerStyle(22); // triangle for corrected
    corrected_graph_0_20_v2->SetMarkerSize(1.3); // triangle for corrected
    corrected_graph_0_20_v2->SetMarkerColor(kBlack);
    corrected_graph_0_20_v2->SetLineColor(kBlack);
    corrected_graph_0_20_v2->SetMinimum(-0.8); // Set the minimum y value
    corrected_graph_0_20_v2->SetMaximum(1.2); // Set the maximum y value
    graph_0_10->SetMarkerStyle(20); // circle
    graph_0_10->SetMarkerColor(kRed);
    graph_0_10->SetLineColor(kBlue);
    graph_10_20->SetMarkerStyle(20); // circle
    graph_10_20->SetMarkerColor(kGreen+3);
    graph_10_20->SetLineColor(kGreen+3);

    graph_0_20_v2->Draw("P SAME");
    graph_0_10->Draw("P SAME");
    graph_10_20->Draw("P SAME");

    TLegend* legend0_20_corr_uncorr_overlay_v2 = new TLegend(0.11, 0.68, 0.31, 0.88);
    legend0_20_corr_uncorr_overlay_v2->SetBorderSize(0);
    legend0_20_corr_uncorr_overlay_v2->SetTextFont(42);
    legend0_20_corr_uncorr_overlay_v2->SetTextSize(0.03);
    legend0_20_corr_uncorr_overlay_v2->AddEntry(graph_0_20_v2, "#bf{v_{2}^{M}} (Uncorrected)", "pe");
    legend0_20_corr_uncorr_overlay_v2->AddEntry(corrected_graph_0_20_v2, "#bf{v_{2}^{#pi^{0}}} (Corrected)", "pe");
    legend0_20_corr_uncorr_overlay_v2->AddEntry(graph_0_10, "0-10% #bf{PHENIX} 2010", "pe");
    legend0_20_corr_uncorr_overlay_v2->AddEntry(graph_10_20, "10-20% #bf{PHENIX} 2010", "pe");
    legend0_20_corr_uncorr_overlay_v2->Draw();

    c_v2_corr_unc_overlay_w_phenix0_20->Update();
    // Draw a dashed line at y = 0
    double x_min_0_20_corr_uncorr_overal = c_v2_corr_unc_overlay_w_phenix0_20->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_0_20_corr_uncorr_overal = c_v2_corr_unc_overlay_w_phenix0_20->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_0_20_corr_uncorr_overal = new TLine(x_min_0_20_corr_uncorr_overal, 0, x_max_0_20_corr_uncorr_overal, 0);
    zeroLine_0_20_corr_uncorr_overal->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_0_20_corr_uncorr_overal->Draw("SAME"); // Draw the line on the same canvas as your plot

    
    c_v2_corr_unc_overlay_w_phenix0_20->Modified();
    c_v2_corr_unc_overlay_w_phenix0_20->Update();
    c_v2_corr_unc_overlay_w_phenix0_20->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/0_20/vN_vs_pT_0_20_corr_uncorr_overal.png");
    
    
    /*
     Overlay with PHENIX, measured v2, and corrected v2, for 20-40 percent centrality
     */
    TCanvas *c_v2_corr_unc_overlay_w_phenix20_40 = new TCanvas("c_v2_corr_unc_overlay_w_phenix20_40", "Overlay 20-40% Corr/UnCorr", 800, 600);
    corrected_graph_20_40_v2->Draw("AP");  // This will be the base graph

    for (int i = 0; i < ptCenters.size(); ++i) {
        double pt = ptCenters[i];
        graph_20_30->SetPoint(i, pt - offsetNew * 3.5, v2_20_30[i]); // Shift leftmost
        graph_20_40_v2->SetPoint(i, pt - offsetNew * 1.3, v2_20_40[i]); // Shift slightly left
        graph_30_40->SetPoint(i, pt + offsetNew * 3.5, v2_30_40[i]); // Shift slightly right
    }
    graph_20_40_v2->SetMarkerStyle(21);
    graph_20_40_v2->SetMarkerColor(kBlue);
    graph_20_40_v2->SetLineColor(kBlue);

    corrected_graph_20_40_v2->SetMarkerStyle(22); // triangle for corrected
    corrected_graph_20_40_v2->SetMarkerSize(1.3); // triangle for corrected
    corrected_graph_20_40_v2->SetMarkerColor(kBlack); // you choose
    corrected_graph_20_40_v2->SetLineColor(kBlack);

    graph_20_30->SetMarkerStyle(20); // circle
    graph_20_30->SetMarkerColor(kRed);
    graph_20_30->SetLineColor(kRed);

    graph_30_40->SetMarkerStyle(20); // circle
    graph_30_40->SetMarkerColor(kGreen+3);
    graph_30_40->SetLineColor(kGreen+3);

    // Draw the additional graphs with the new styles and colors
    graph_20_40_v2->Draw("P SAME");
    graph_20_30->Draw("P SAME");
    graph_30_40->Draw("P SAME");

    TLegend* legend20_40_corr_uncorr_overlay_v2 = new TLegend(0.11, 0.68, 0.31, 0.88);
    legend20_40_corr_uncorr_overlay_v2->SetBorderSize(0);
    legend20_40_corr_uncorr_overlay_v2->SetTextFont(42);
    legend20_40_corr_uncorr_overlay_v2->SetTextSize(0.03);
    
    legend20_40_corr_uncorr_overlay_v2->AddEntry(graph_20_40_v2, "#bf{v_{2}^{M}} (Uncorrected)", "p");
    legend20_40_corr_uncorr_overlay_v2->AddEntry(corrected_graph_20_40_v2, "#bf{v_{2}^{#pi^{0}}} (Corrected)", "p");
    legend20_40_corr_uncorr_overlay_v2->AddEntry(graph_20_30, "20-30% #bf{PHENIX} 2010", "p");
    legend20_40_corr_uncorr_overlay_v2->AddEntry(graph_30_40, "30-40% #bf{PHENIX} 2010", "p");
    legend20_40_corr_uncorr_overlay_v2->Draw();

    c_v2_corr_unc_overlay_w_phenix20_40->Update();
    // Draw a dashed line at y = 0
    double x_min_20_40_corr_uncorr_overal = c_v2_corr_unc_overlay_w_phenix20_40->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_20_40_corr_uncorr_overal = c_v2_corr_unc_overlay_w_phenix20_40->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_20_40_corr_uncorr_overal = new TLine(x_min_20_40_corr_uncorr_overal, 0, x_max_20_40_corr_uncorr_overal, 0);
    zeroLine_20_40_corr_uncorr_overal->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_20_40_corr_uncorr_overal->Draw("SAME"); // Draw the line on the same canvas as your plot
    
    c_v2_corr_unc_overlay_w_phenix20_40->Modified();
    c_v2_corr_unc_overlay_w_phenix20_40->Update();
    c_v2_corr_unc_overlay_w_phenix20_40->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/20_40/vN_vs_pT_20_40_corr_uncorr_overal.png");
    
    
    
    /*
     Overlay with PHENIX, measured v2, and corrected v2, for 40-60 percent centrality
     */
    TCanvas *c_v2_corr_unc_overlay_w_phenix40_60 = new TCanvas("c_v2_corr_unc_overlay_w_phenix40_60", "Overlay 40-60% Corr/UnCorr", 800, 600);
    corrected_graph_40_60_v2->Draw("AP");  // This will be the base graph

    for (int i = 0; i < ptCenters.size(); ++i) {
        double pt = ptCenters[i];
        graph_40_50->SetPoint(i, pt - offsetNew * 3.5, v2_40_50[i]);
        graph_40_60_v2->SetPoint(i, pt - offsetNew * 1.3, v2_40_60[i]);
        graph_50_60->SetPoint(i, pt + offsetNew * 3.5, v2_50_60[i]);
    }
    graph_40_60_v2->SetMarkerStyle(21); // square for uncorrected
    graph_40_60_v2->SetMarkerColor(kBlue);
    graph_40_60_v2->SetLineColor(kBlue);

    corrected_graph_40_60_v2->SetMarkerStyle(22); // triangle for corrected
    corrected_graph_40_60_v2->SetMarkerSize(1.3); // triangle for corrected
    corrected_graph_40_60_v2->SetMarkerColor(kBlack);
    corrected_graph_40_60_v2->SetLineColor(kBlack);
    
    corrected_graph_40_60_v2->SetMinimum(-0.25); // Set the minimum y value
    corrected_graph_40_60_v2->SetMaximum(0.75); // Set the maximum y value
    
    graph_40_50->SetMarkerStyle(20); // circle
    graph_40_50->SetMarkerColor(kRed);
    graph_40_50->SetLineColor(kRed);

    graph_50_60->SetMarkerStyle(20); // circle
    graph_50_60->SetMarkerColor(kGreen+3);
    graph_50_60->SetLineColor(kGreen+3);

    // Draw the additional graphs with the new styles and colors
    graph_40_60_v2->Draw("P SAME");
    graph_40_50->Draw("P SAME");
    graph_50_60->Draw("P SAME");

    // Legend
    TLegend* legend40_60_corr_uncorr_overlay_v2 = new TLegend(0.28, 0.69, 0.38, 0.89);
    legend40_60_corr_uncorr_overlay_v2->SetBorderSize(0);
    legend40_60_corr_uncorr_overlay_v2->SetTextFont(42);
    legend40_60_corr_uncorr_overlay_v2->SetTextSize(0.033);
    legend40_60_corr_uncorr_overlay_v2->AddEntry(graph_40_60_v2, "#bf{v_{2}^{M}} (Uncorrected)", "p");
    legend40_60_corr_uncorr_overlay_v2->AddEntry(corrected_graph_40_60_v2, "#bf{v_{2}^{#pi^{0}}} (Corrected)", "p");
    legend40_60_corr_uncorr_overlay_v2->AddEntry(graph_40_50, "40-50% #bf{PHENIX} 2010", "p");
    legend40_60_corr_uncorr_overlay_v2->AddEntry(graph_50_60, "50-60% #bf{PHENIX} 2010", "p");
    legend40_60_corr_uncorr_overlay_v2->Draw();
    
    c_v2_corr_unc_overlay_w_phenix40_60->Update();
    // Draw a dashed line at y = 0
    double x_min_40_60_corr_uncorr_overal = c_v2_corr_unc_overlay_w_phenix20_40->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_40_60_corr_uncorr_overal = c_v2_corr_unc_overlay_w_phenix20_40->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_40_60_corr_uncorr_overal = new TLine(x_min_20_40_corr_uncorr_overal, 0, x_max_20_40_corr_uncorr_overal, 0);
    zeroLine_40_60_corr_uncorr_overal->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_40_60_corr_uncorr_overal->Draw("SAME"); // Draw the line on the same canvas as your plot
    
    
    c_v2_corr_unc_overlay_w_phenix40_60->Modified();
    c_v2_corr_unc_overlay_w_phenix40_60->Update();
    c_v2_corr_unc_overlay_w_phenix40_60->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2/40_60/vN_vs_pT_40_60_corr_uncorr_overal.png");
    
    
    
    
    
    
    /*
     Plot Measured v3 Data for 0-20 centrality
     */
    TCanvas *c16 = new TCanvas("c16", "v3 vs pT 0-20%", 800, 600);
    TGraphErrors *graph_0_20_v3 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v3_0_20[0], 0, &v3_0_20_Errors[0]);
    graph_0_20_v3->SetMarkerColor(kBlue);
    graph_0_20_v3->SetMarkerStyle(21);
    graph_0_20_v3->SetMarkerSize(1.1);
    graph_0_20_v3->SetTitle("Diphoton #it{v}_{3} vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{3}");
    graph_0_20_v3->Draw("AP");
    graph_0_20_v3->GetXaxis()->SetLimits(2.0, 5.0);
    graph_0_20_v3->GetXaxis()->SetNdivisions(010);
    graph_0_20_v3->SetMinimum(-0.45); // Set the minimum y value
    graph_0_20_v3->SetMaximum(0.45); // Set the maximum y value
    c16->SetTicks();
    TLatex latex020_v3;
    latex020_v3.SetTextSize(0.03);
    latex020_v3.SetTextAlign(12);
    latex020_v3.SetNDC();
    latex020_v3.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex020_v3.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex020_v3.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex020_v3.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    c16->Modified();
    c16->Update();
    c16->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v3/0_20/measured_v3_vs_pT_0_20.png");

  
    
    /*
     Plot Measured v3 Data for 40-60 centrality -- Skipping 20-40 due to NaN values in CSV
     */
    TCanvas *c17 = new TCanvas("c17", "v3 vs pT 40-60%", 800, 600);
    TGraphErrors *graph_40_60_v3 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &v3_40_60[0], 0, &v3_40_60_Errors[0]);
    graph_40_60_v3->SetMarkerColor(kBlue);
    graph_40_60_v3->SetMarkerStyle(21);
    graph_40_60_v3->SetMarkerSize(1.1);
    graph_40_60_v3->SetTitle("Diphoton #it{v}_{3} vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    graph_40_60_v3->Draw("AP");
    graph_40_60_v3->GetXaxis()->SetLimits(2.0, 5.0);
    graph_40_60_v3->GetXaxis()->SetNdivisions(010);
    graph_40_60_v3->SetMinimum(-2); // Set the minimum y value
    graph_40_60_v3->SetMaximum(2); // Set the maximum y value
    c17->SetTicks();
    TLatex latex4060_v3;
    latex4060_v3.SetTextSize(0.03);
    latex4060_v3.SetTextAlign(12);
    latex4060_v3.SetNDC();
    latex4060_v3.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex4060_v3.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex4060_v3.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex4060_v3.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    
    c17->Modified();
    c17->Update();
    c17->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v3/40_60/measured_v3_vs_pT_40_60.png");
    

    
    /*
     Plot Background v3 Data for 0-20 percent centrality
     */
    TCanvas *c18 = new TCanvas("c18", "Background v3 vs pT 0-20%", 800, 600);
    TGraphErrors *bg_graph_0_20_v3 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &bg_v3_0_20[0], 0, &bg_v3_0_20_Errors[0]);
    bg_graph_0_20_v3->SetMarkerColor(kBlue);
    bg_graph_0_20_v3->SetMarkerStyle(21);
    bg_graph_0_20_v3->SetMarkerSize(1.1);
    bg_graph_0_20_v3->SetTitle("Background #it{v}_{3} vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{3}");
    bg_graph_0_20_v3->Draw("AP");
    bg_graph_0_20_v3->GetXaxis()->SetLimits(2.0, 5.0);
    bg_graph_0_20_v3->GetXaxis()->SetNdivisions(010);
    bg_graph_0_20_v3->SetMinimum(-0.2); // Set the minimum y value
    bg_graph_0_20_v3->SetMaximum(0.28); // Set the maximum y value
    c18->SetTicks();
    TLatex latex020_bg_v3;
    latex020_bg_v3.SetTextSize(0.03);
    latex020_bg_v3.SetTextAlign(12);
    latex020_bg_v3.SetNDC();
    latex020_bg_v3.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex020_bg_v3.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex020_bg_v3.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex020_bg_v3.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    c18->Modified();
    c18->Update();
    c18->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v3/0_20/bg_v3_vs_pT_0_20.png");
    

    /*
     Plot Background v3 Data for 40-60 percent centrality
     */
    TCanvas *c19 = new TCanvas("c19", "Background v3 vs pT 40-60%", 800, 600);
    TGraphErrors *bg_graph_40_60_v3 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &bg_v3_40_60[0], 0, &bg_v3_40_60_Errors[0]);
    bg_graph_40_60_v3->SetMarkerColor(kBlue);
    bg_graph_40_60_v3->SetMarkerStyle(21);
    bg_graph_40_60_v3->SetMarkerSize(1.1);
    bg_graph_40_60_v3->SetTitle("Background #it{v}_{3} vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    bg_graph_40_60_v3->Draw("AP");
    bg_graph_40_60_v3->GetXaxis()->SetLimits(2.0, 5.0);
    bg_graph_40_60_v3->GetXaxis()->SetNdivisions(010);
    bg_graph_40_60_v3->SetMinimum(-1.0); // Set the minimum y value
    bg_graph_40_60_v3->SetMaximum(0.7); // Set the maximum y value
    c19->SetTicks();
    //Output cut information
    TLatex latex4060_bg_v3;
    latex4060_bg_v3.SetTextSize(0.03);
    latex4060_bg_v3.SetTextAlign(12);
    latex4060_bg_v3.SetNDC();
    latex4060_bg_v3.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex4060_bg_v3.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex4060_bg_v3.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex4060_bg_v3.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    c19->Modified();
    c19->Update();
    c19->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v3/40_60/bg_v3_vs_pT_40_60.png");

    /*
     OVERLAY BACKGROUND v3 TO LEFT AND RIGHT OF SIGNAL REGION for 0-20%
     */
    TCanvas *c_0_20_Left_Right_bg_Overlay_v3 = new TCanvas("c_0_20_Left_Right_bg_Overlay_v3", "Sideband #v_3 Overlay, 0-20% Centrality", 800, 600);
    TGraphErrors *graph_0_20_bg_left_v3 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &bg_v3_left_0_20[0], 0, &bg_v3_left_0_20_Errors[0]);
    
    graph_0_20_bg_left_v3->SetMarkerStyle(20); // Circle
    graph_0_20_bg_left_v3->SetMarkerColor(kRed); // Changed to red for visibility
    graph_0_20_bg_left_v3->SetLineColor(kRed);
    // Draw the right background graph as the base graph
    bg_graph_0_20_v3->Draw("AP");
    
    // Apply jitter to x values for graph_40_50 and graph_50_60
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_0_20_bg_left_v3->SetPoint(i, ptCenters[i] - .06, bg_v3_left_0_20[i]);
    }
    
    bg_graph_0_20_v3->SetTitle("Sideband #it{v}_{3} to Left and Right of Signal Region vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    bg_graph_0_20_v3->SetMinimum(-0.25); // Set the minimum y value
    bg_graph_0_20_v3->SetMaximum(0.3); // Set the maximum y value
    graph_0_20_bg_left_v3->Draw("P SAME");

    // Create a legend to distinguish between the two graphs
    TLegend *legend_leftRightbg_overlay_0_20_v3 = new TLegend(0.12, 0.65, 0.32, 0.85);
    legend_leftRightbg_overlay_0_20_v3->SetBorderSize(0);
    legend_leftRightbg_overlay_0_20_v3->SetTextSize(0.03);
    legend_leftRightbg_overlay_0_20_v3->AddEntry(bg_graph_0_20_v3, "0-20%, v_{3}^{bg} right of signal region in range (#mu + 3*#sigma, 0.5) GeV", "p");
    legend_leftRightbg_overlay_0_20_v3->AddEntry(graph_0_20_bg_left_v3, "0-20%, v_{3}^{bg} left of signal region in range (0, #mu - 2*#sigma) GeV", "p");
    legend_leftRightbg_overlay_0_20_v3->Draw();

    c_0_20_Left_Right_bg_Overlay_v3->Modified();
    c_0_20_Left_Right_bg_Overlay_v3->Update();
    c_0_20_Left_Right_bg_Overlay_v3->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v3/0_20/bg_v3_Left_Right_Overlay_0_20.png");

    
    
    /*
     OVERLAY BACKGROUND v3 TO LEFT AND RIGHT OF SIGNAL REGION for 40-60%
     */
    TCanvas *c_40_60_Left_Right_bg_Overlay_v3 = new TCanvas("c_40_60_Left_Right_bg_Overlay_v3", "Sideband #v_3 Overlay, 40-60% Centrality", 800, 600);
    TGraphErrors *graph_40_60_bg_left_v3 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &bg_v3_left_40_60[0], 0, &bg_v3_left_40_60_Errors[0]);
    
    graph_40_60_bg_left_v3->SetMarkerStyle(20); // Circle
    graph_40_60_bg_left_v3->SetMarkerColor(kRed); // Changed to red for visibility
    graph_40_60_bg_left_v3->SetLineColor(kRed);
    // Draw the right background graph as the base graph
    bg_graph_40_60_v3->Draw("AP");
    
    // Apply jitter to x values for graph_40_50 and graph_50_60
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_40_60_bg_left_v3->SetPoint(i, ptCenters[i] - .06, bg_v3_left_40_60[i]);
    }
    
    bg_graph_40_60_v3->SetTitle("Sideband #it{v}_{3} to Left and Right of Signal Region vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    bg_graph_40_60_v3->SetMinimum(-2); // Set the minimum y value
    bg_graph_40_60_v3->SetMaximum(2); // Set the maximum y value
    graph_40_60_bg_left_v3->Draw("P SAME");

    // Create a legend to distinguish between the two graphs
    TLegend *legend_leftRightbg_overlay_40_60_v3 = new TLegend(0.12, 0.65, 0.32, 0.85);
    legend_leftRightbg_overlay_40_60_v3->SetBorderSize(0);
    legend_leftRightbg_overlay_40_60_v3->SetTextSize(0.024);
    legend_leftRightbg_overlay_40_60_v3->AddEntry(bg_graph_40_60_v3, "40-60%, v_{3}^{bg} right of signal region in range (#mu + 3*#sigma, 0.5) GeV", "p");
    legend_leftRightbg_overlay_40_60_v3->AddEntry(graph_40_60_bg_left_v3, "40-60%, v_{3}^{bg} left of signal region in range (0, #mu - 2*#sigma) GeV", "p");
    legend_leftRightbg_overlay_40_60_v3->Draw();

    c_40_60_Left_Right_bg_Overlay_v3->Modified();
    c_40_60_Left_Right_bg_Overlay_v3->Update();
    c_40_60_Left_Right_bg_Overlay_v3->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v3/40_60/bg_v3_Left_Right_Overlay_40_60.png");

    
    
    
    
    /*
     Plot corrected v3 data for 0-20 percent centrality
     */
    TCanvas *c20 = new TCanvas("c20", "Corrected v3 vs pT 0-20%", 800, 600);
    TGraphErrors *corrected_graph_0_20_v3 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &corrected_v3_0_20[0], 0, &corrected_v3_0_20_Errors[0]);
    corrected_graph_0_20_v3->SetMarkerColor(kBlue);
    corrected_graph_0_20_v3->SetMarkerStyle(21);
    corrected_graph_0_20_v3->SetMarkerSize(1.1);
    corrected_graph_0_20_v3->SetTitle("Corrected #it{v}_{3} vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    corrected_graph_0_20_v3->Draw("AP");
    corrected_graph_0_20_v3->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_graph_0_20_v3->GetXaxis()->SetNdivisions(010);
    corrected_graph_0_20_v3->SetMinimum(-0.4); // Set the minimum y value
    corrected_graph_0_20_v3->SetMaximum(2.5); // Set the maximum y value
    c20->SetTicks();
    //Output cut information
    TLatex latex020_correct_v3;
    latex020_correct_v3.SetTextSize(0.03);
    latex020_correct_v3.SetTextAlign(12);
    latex020_correct_v3.SetNDC();
    latex020_correct_v3.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex020_correct_v3.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex020_correct_v3.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex020_correct_v3.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    c20->Modified();
    c20->Update();
    c20->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v3/0_20/v3_vs_pT_0_20_corrected.png");

    
    /*
     Plot corrected v3 data for 40-60 percent centrality
     */
    TCanvas *c21 = new TCanvas("c21", "Corrected v3 vs pT 40-60%", 800, 600);
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
    corrected_graph_40_60_v3->SetMinimum(-0.2); // Set the minimum y value
    corrected_graph_40_60_v3->SetMaximum(0.7); // Set the maximum y value

    c21->SetTicks();
    
    //Output cut information
    TLatex latex4060_corr_v3;
    latex4060_corr_v3.SetTextSize(0.03);
    latex4060_corr_v3.SetTextAlign(12);
    latex4060_corr_v3.SetNDC();

    latex4060_corr_v3.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex4060_corr_v3.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex4060_corr_v3.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex4060_corr_v3.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");

    
    c21->Modified();
    c21->Update();
    c21->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v3/40_60/v3_vs_pT_40_60_corrected.png");
    
    
    /*
     Overlay Measured and Corrected v3 Output
     */
    TCanvas *c_0_20_measured_corrected_v3 = new TCanvas("c_0_20_measured_corrected_v3", "Measured and Corrected #v_3 Overlay, 0-20% Centrality", 800, 600);
    corrected_graph_0_20_v3->Draw("AP");  // This will be the base graph
    corrected_graph_0_20_v3->SetTitle("Measured #it{v}_{3} and #pi^{0} #it{v}_{3} vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{3}");
    corrected_graph_0_20_v3->SetMinimum(-2.0); // Set the minimum y value
    corrected_graph_0_20_v3->SetMaximum(2.0); // Set the maximum y value
    
    graph_0_20_v3->SetMarkerStyle(20); // circle
    graph_0_20_v3->SetMarkerColor(kBlue);
    graph_0_20_v3->SetLineColor(kBlue);
    
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_0_20_v3->SetPoint(i, ptCenters[i] - offset, v3_0_20[i]);
    }
    
    graph_0_20_v3 -> Draw("P SAME");

    corrected_graph_0_20_v3->SetMarkerColor(kRed);
    corrected_graph_0_20_v3->SetLineColor(kRed);
    corrected_graph_0_20_v3->SetMarkerStyle(21);

    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend_0_20_measured_corrected_v3 = new TLegend(0.1, 0.7, 0.3, 0.9);
    legend_0_20_measured_corrected_v3->AddEntry(corrected_graph_0_20_v3, "0-20%, v_{3}^{#pi^{0}}", "p");
    legend_0_20_measured_corrected_v3->AddEntry(graph_0_20_v3, "0-20%, v_{3}^{M}", "p");


    legend_0_20_measured_corrected_v3->Draw();

    c_0_20_measured_corrected_v3->Modified();
    c_0_20_measured_corrected_v3->Update();
    c_0_20_measured_corrected_v3->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v3/0_20/v3_measured_corrected_overal_0_20.png");
    
    
    /*
     Overlay Measured and Corrected v3 Output 40 - 60 percent centrality
     */
    TCanvas *c_40_60_measured_corrected_v3 = new TCanvas("c_40_60_measured_corrected_v3", "Measured and Corrected #v_3 Overlay, 0-20% Centrality", 800, 600);
    corrected_graph_40_60_v3->Draw("AP");  // This will be the base graph
    corrected_graph_40_60_v3->SetTitle("Measured #it{v}_{3} and #pi^{0} #it{v}_{3} vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{3}");
    corrected_graph_40_60_v3->SetMinimum(-2.0); // Set the minimum y value
    corrected_graph_40_60_v3->SetMaximum(2.0); // Set the maximum y value
    
    graph_40_60_v3->SetMarkerStyle(20); // circle
    graph_40_60_v3->SetMarkerColor(kRed);
    graph_40_60_v3->SetLineColor(kRed);
    
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_40_60_v3->SetPoint(i, ptCenters[i] - offset, v3_40_60[i]);
    }
    
    graph_40_60_v3 -> Draw("P SAME");

    corrected_graph_40_60_v3->SetMarkerColor(kBlue);
    corrected_graph_40_60_v3->SetLineColor(kBlue);
    corrected_graph_40_60_v3->SetMarkerStyle(21);

    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend_40_60_measured_corrected_v3 = new TLegend(0.1, 0.7, 0.3, 0.9);
    legend_40_60_measured_corrected_v3->AddEntry(graph_40_60_v3, "0-20%, v_{3}^{M}", "p");
    legend_40_60_measured_corrected_v3->AddEntry(corrected_graph_40_60_v3, "0-20%, v_{3}^{#pi^{0}}", "p");

    legend_40_60_measured_corrected_v3->Draw();

    c_40_60_measured_corrected_v3->Modified();
    c_40_60_measured_corrected_v3->Update();
    c_40_60_measured_corrected_v3->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v3/40_60/v3_measured_corrected_overal_40_60.png");
    
    

    
    
    
    
    
    
    
    
    
    
    /*
     Overlay v2 and v3 measured Output
     */
    TCanvas *c_0_20_v2_v3_measured = new TCanvas("c_0_20_v2_v3_measured", "Measured #v_2, #v_3 Overlay, 0-20% Centrality", 800, 600);
    graph_0_20_v2->Draw("AP");  // This will be the base graph
    graph_0_20_v2->SetTitle("Diphoton #it{v}_{2} and #it{v}_{3} Overlay vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{N}");
    graph_0_20_v2->SetMinimum(-0.4); // Set the minimum y value
    graph_0_20_v2->SetMaximum(0.4); // Set the maximum y value
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_0_20_v3->SetPoint(i, ptCenters[i] - offset, v3_0_20[i]);
    }
    
    graph_0_20_v3->SetMarkerStyle(20); // circle
    graph_0_20_v3->SetMarkerColor(kRed);
    graph_0_20_v3->SetLineColor(kRed);
    
    graph_0_20_v3 -> Draw("P SAME");
    

    graph_0_20_v2->SetMarkerColor(kBlue);
    graph_0_20_v2->SetMarkerStyle(21);

    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend_0_20_v2_v3_measured = new TLegend(0.11, 0.11, 0.31, 0.31);
    legend_0_20_v2_v3_measured->SetBorderSize(0);
    legend_0_20_v2_v3_measured->SetTextSize(0.03);
    legend_0_20_v2_v3_measured->AddEntry(graph_0_20_v2, "0-20%, v_{2}^{M}", "p");
    legend_0_20_v2_v3_measured->AddEntry(graph_0_20_v3, "0-20%, v_{3}^{M}", "p");

    legend_0_20_v2_v3_measured->Draw();
    
    TLatex latex_0_20_v2_v3_measured;
    latex_0_20_v2_v3_measured.SetTextSize(0.03);
    latex_0_20_v2_v3_measured.SetTextAlign(12);
    latex_0_20_v2_v3_measured.SetNDC();
    latex_0_20_v2_v3_measured.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex_0_20_v2_v3_measured.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex_0_20_v2_v3_measured.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex_0_20_v2_v3_measured.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    
    
    c_0_20_v2_v3_measured->Update();
    // Draw a dashed line at y = 0
    double x_min_v2_v3_measured_0_20 = c_0_20_v2_v3_measured->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_v2_v3_measured_0_20 = c_0_20_v2_v3_measured->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_v2_v3_measured_0_20 = new TLine(x_min_v2_v3_measured_0_20, 0, x_max_v2_v3_measured_0_20, 0);
    zeroLine_v2_v3_measured_0_20->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_v2_v3_measured_0_20->Draw("SAME"); // Draw the line on the same canvas as your plot

    

    c_0_20_v2_v3_measured->Modified();
    c_0_20_v2_v3_measured->Update();
    c_0_20_v2_v3_measured->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2_v3_Measured_Overlay_0_20.png");
    
    
    
    /*
     Overlay Measured v2 and v3 40-60 percent centrality
     */
    
    TCanvas *c_40_60_v2_v3_measured = new TCanvas("c_40_60_v2_v3_measured", "Measured #v_2, #v_3 Overlay, 40-60% Centrality", 800, 600);
    graph_40_60_v2->Draw("AP");  // This will be the base graph
    graph_40_60_v2->SetTitle("Diphoton #it{v}_{2} and #it{v}_{3} Overlay vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{N}");
    graph_40_60_v2->SetMinimum(-1.5); // Set the minimum y value
    graph_40_60_v2->SetMaximum(1.5); // Set the maximum y value
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_40_60_v3->SetPoint(i, ptCenters[i] - .08, v3_40_60[i]);
    }
    
    graph_40_60_v3->SetMarkerStyle(20); // circle
    graph_40_60_v3->SetMarkerColor(kRed);
    graph_40_60_v3->SetLineColor(kRed);
    
    graph_40_60_v3 -> Draw("P SAME");

    graph_40_60_v2->SetMarkerColor(kBlue);
    graph_40_60_v2->SetMarkerStyle(21);

    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend_40_60_v2_v3_measured = new TLegend(0.11, 0.11, 0.31, 0.31);
    legend_40_60_v2_v3_measured->SetBorderSize(0);
    legend_40_60_v2_v3_measured->SetTextSize(0.03);
    legend_40_60_v2_v3_measured->AddEntry(graph_40_60_v2, "0-20%, v_{2}^{M}", "p");
    legend_40_60_v2_v3_measured->AddEntry(graph_40_60_v3, "0-20%, v_{3}^{M}", "p");

    legend_40_60_v2_v3_measured->Draw();
    
    TLatex latex__40_60_v2_v3_measured;
    latex__40_60_v2_v3_measured.SetTextSize(0.03);
    latex__40_60_v2_v3_measured.SetTextAlign(12);
    latex__40_60_v2_v3_measured.SetNDC();
    latex__40_60_v2_v3_measured.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex__40_60_v2_v3_measured.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex__40_60_v2_v3_measured.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex__40_60_v2_v3_measured.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    
    c_40_60_v2_v3_measured->Update();
    // Draw a dashed line at y = 0
    double x_min_v2_v3_measured_40_60 = c_40_60_v2_v3_measured->GetUxmin(); // Get the minimum x-value from the canvas
    double x_max_v2_v3_measured_40_60 = c_40_60_v2_v3_measured->GetUxmax(); // Get the maximum x-value from the canvas
    TLine *zeroLine_v2_v3_measured_40_60 = new TLine(x_min_v2_v3_measured_40_60, 0, x_max_v2_v3_measured_40_60, 0);
    zeroLine_v2_v3_measured_40_60->SetLineStyle(2); // 2 corresponds to a dashed line
    zeroLine_v2_v3_measured_40_60->Draw("SAME"); // Draw the line on the same canvas as your plot

    c_40_60_v2_v3_measured->Modified();
    c_40_60_v2_v3_measured->Update();
    c_40_60_v2_v3_measured->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2_v3_Measured_Overlay_40_60.png");
    
    
    
    
    
    /*
     Overlay for v2 v3 Corrected Output
     */
    TCanvas *c_0_20_v2_v3_corrected = new TCanvas("c_0_20_v2_v3_corrected", "Corrected #v_2, #v_3 Overlay, Corrected, 0-20% Centrality", 800, 600);
    corrected_graph_0_20_v2->Draw("AP");  // This will be the base graph
    corrected_graph_0_20_v2->SetTitle("#pi^{0} #it{v}_{2} and #it{v}_{3} Overlay vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{N}");
    corrected_graph_0_20_v2->SetMinimum(-2.0); // Set the minimum y value
    corrected_graph_0_20_v2->SetMaximum(2.0); // Set the maximum y value
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_graph_0_20_v2->SetPoint(i, ptCenters[i] - offset, v2_0_20[i]);
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
    c_0_20_v2_v3_corrected->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2_v3_Corrected_Overlay_0_20.png");
    
    
    
    /*
     Make plot corrected v2 overlay with v3 for 40-60 centrality
     */
    TCanvas *c_40_60_v2_v3_corrected = new TCanvas("c_40_60_v2_v3_corrected", "Measured #v_2, #v_3 Overlay, Corrected, 40-60% Centrality", 800, 600);
    corrected_graph_40_60_v2->Draw("AP");  // This will be the base graph
    corrected_graph_40_60_v2->SetTitle("#pi^{0} #it{v}_{2} and #it{v}_{3} Overlay vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{N}");
    corrected_graph_40_60_v2->SetMinimum(-2.0); // Set the minimum y value
    corrected_graph_40_60_v2->SetMaximum(2.0); // Set the maximum y value
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_graph_40_60_v2->SetPoint(i, ptCenters[i] - offset, v2_40_60[i]);
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
    c_40_60_v2_v3_corrected->SaveAs("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/vN_Plot_Output/v2_v3_Corrected_Overlay_40_60.png");

    
    
}

void printCalculationDetails(const std::string& type, int index, const std::string& numeratorPath, const std::string& denominatorPath, float value, float error) {
    std::cout << std::left << std::setw(10) << type
              << "Index: " << std::setw(4) << index
              << " | Num: " << std::setw(30) << numeratorPath
              << " | Denom: " << std::setw(30) << denominatorPath
              << " | Value: " << std::setw(10) << value
              << " | Error: " << error << std::endl;
}


void vN_calculator_AccumulatedData() {
    // Open the ROOT file
    TFile *file = new TFile("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/testRootFiles_ByProduction/p009/test.root", "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file or file is not a valid ROOT file" << std::endl;
        return;
    }

    /*
     Histograms Related to v2, SET UP VECTORS FOR QQ2, qQ2, qQ2_bg, qQ2_bg_left
     */
    // Define the histogram names for QQ2 within the vn/QQ2 directory
    std::string hist_name_QQ2[3] = {"vn/QQ2/hQQ2_40-60", "vn/QQ2/hQQ2_20-40", "vn/QQ2/hQQ2_0-20"};

    // Define the histogram names for qQ within the vn/qQ directory
    std::string hist_name_qQ2[18];
    for (int i = 0; i < 18; ++i) {
        hist_name_qQ2[i] = "vn/qQ2/hqQ2_" + std::to_string(i);
    }
    
    std::string hist_name_qQ2_bg[18];
    for (int i = 0; i < 18; ++i) {
        hist_name_qQ2_bg[i] = "vn/qQ2_bg/hqQ2_bg_" + std::to_string(i);
    }

    std::string hist_name_qQ2_bg_left[18];
    for (int i = 0; i < 18; ++i) {
        hist_name_qQ2_bg_left[i] = "vn/qQ2_bg_left/hqQ2_bg_left_" + std::to_string(i);
    }
    
    
    /*
     Histograms Related to v3, SET UP VECTORS FOR QQ3, qQ3, qQ3_bg, qQ3_bg_left
     */
    
    std::string hist_name_QQ3[3] = {"vn/QQ3/hQQ3_40-60", "vn/QQ3/hQQ3_20-40", "vn/QQ3/hQQ3_0-20"};

    // Define the histogram names for qQ within the vn/qQ directory
    std::string hist_name_qQ3[18];
    for (int i = 0; i < 18; ++i) {
        hist_name_qQ3[i] = "vn/qQ3/hqQ3_" + std::to_string(i);
    }
    
    std::string hist_name_qQ3_bg[18];
    for (int i = 0; i < 18; ++i) {
        hist_name_qQ3_bg[i] = "vn/qQ3_bg/hqQ3_bg_" + std::to_string(i);
    }

    std::string hist_name_qQ3_bg_left[18];
    for (int i = 0; i < 18; ++i) {
        hist_name_qQ3_bg_left[i] = "vn/qQ3_bg_left/hqQ3_bg_left_" + std::to_string(i);
    }
    
    /*
     chunk of code below prints ALL HISTOGRAMS funneled through
     */
    
    auto drawAndSaveHist = [&](const std::string& histPath, const std::string& baseDir) {
        TH1F *hist = (TH1F*)file->Get(histPath.c_str());
        if (!hist) {
            std::cerr << "Error: Histogram " << histPath << " not found" << std::endl;
            return;
        }
        TCanvas *c = new TCanvas();
        c->SetLogy(); // Set logarithmic scale for y-axis

        // Determine the appropriate save directory based on the histogram's relation to v2 or v3
        std::string saveDir = baseDir;
        std::string histFileName = histPath.substr(histPath.find_last_of("/") + 1) + ".png"; // Extract histogram name and append .png

        std::string savePath = saveDir + "/" + histFileName;
        hist->Draw();
        c->SaveAs(savePath.c_str());
        delete c;
    };

    // Define the base directories for v2 and v3 histograms
    std::string baseDirV2 = "/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/qQ_and_QQ_histograms_p009/n_2";
    std::string baseDirV3 = "/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/qQ_and_QQ_histograms_p009/n_3";

    // Process and save QQ and qQ histograms for v2
    for (const auto& histPath : hist_name_QQ2) {
        drawAndSaveHist(histPath, baseDirV2);
    }
    for (const auto& histPath : hist_name_qQ2) {
        drawAndSaveHist(histPath, baseDirV2);
    }
    for (const auto& histPath : hist_name_qQ2_bg) {
        drawAndSaveHist(histPath, baseDirV2);
    }
    for (const auto& histPath : hist_name_qQ2_bg_left) {
        drawAndSaveHist(histPath, baseDirV2);
    }
    
    // Process and save QQ and qQ histograms for v3
    for (const auto& histPath : hist_name_QQ3) {
        drawAndSaveHist(histPath, baseDirV3);
    }
    for (const auto& histPath : hist_name_qQ3) {
        drawAndSaveHist(histPath, baseDirV3);
    }
    for (const auto& histPath : hist_name_qQ3_bg) {
        drawAndSaveHist(histPath, baseDirV3);
    }
    for (const auto& histPath : hist_name_qQ3_bg_left) {
        drawAndSaveHist(histPath, baseDirV3);
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
        std::ifstream inFile("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/I_O_Accumulated_PlotByPlotOutput/PlotByPlotOutput_2_15_24_cleaned.csv");
        std::ofstream outFile("/Users/patsfan753/Desktop/Desktop/v_N_Analysis_Final-2_15/I_O_Accumulated_PlotByPlotOutput/UpdatedCSV_AccumulatedDists_p009.csv");

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

                    /*
                     CALCULATIONS for v2 measured, left/right background, corrected, with error prop
                     */
                    // ANSI escape codes: "\033[1m" for bold, "\033[31m" for red, "\033[0m" resets to default
                    std::cout << "\033[1m\033[31m" // Start with bold and red
                              << "Type      Index |    Numerator Path                   |   Denominator Path                 | Value      | Error"
                              << "\033[0m" // Reset to default at the end
                              << std::endl;
                    std::cout << "\033[1m\033[31m" // Bold and red for the separator line as well
                              << std::string(120, '-')
                              << "\033[0m" // Reset to default
                              << std::endl;
                    
                    float v2_value = qQ2_mean[idx] / sqrt(QQ2_mean[QQ_index]);
                    float v2_error = abs(v2_value) * sqrt(pow(qQ2_error[idx] / qQ2_mean[idx], 2) + 0.25 * pow(QQ2_error[QQ_index] / QQ2_mean[QQ_index], 2));
                    printCalculationDetails("v2", idx, hist_name_qQ2[idx], hist_name_QQ2[QQ_index], v2_value, v2_error);
                    
                    

                    float bg_v2_value = qQ2_bg_mean[idx] / sqrt(QQ2_mean[QQ_index]);
                    float bg_v2_error = abs(bg_v2_value) * sqrt(pow(qQ2_bg_error[idx] / qQ2_bg_mean[idx], 2) + 0.25 * pow(QQ2_error[QQ_index] / QQ2_mean[QQ_index], 2));

                    // Calculate left background v2 and v2_error
                    float left_bg_v2_value = qQ2_bg_left_mean[idx] / sqrt(QQ2_mean[QQ_index]);
                    float left_bg_v2_error = abs(left_bg_v2_value) * sqrt(pow(qQ2_bg_left_error[idx] / qQ2_bg_left_mean[idx], 2) + 0.25 * pow(QQ2_error[QQ_index] / QQ2_mean[QQ_index], 2));

                    float v2_corrected = v2_value * (1 + (1 / SB)) - ((1 / SB) * bg_v2_value);
                    
                    float v2_corrected_error = sqrt(
                        pow((1 + (1 / SB)) * v2_error, 2) + // Error contribution from v2_signal
                        pow((-1 / SB) * bg_v2_error, 2) +   // Error contribution from v2_bg (note the negative sign is squared, having no effect)
                        pow((-(v2_value - bg_v2_value) / (SB * SB)) * SBerror, 2) // Error contribution from SB
                    );
                    
                    
                    /*
                     CALCULATIONS for v3 measured, left/right background, corrected, with error prop
                     */
                    
                    
                    float v3_value = qQ3_mean[idx] / sqrt(QQ3_mean[QQ_index]);
                    float v3_error = abs(v3_value) * sqrt(pow(qQ3_error[idx] / qQ3_mean[idx], 2) + 0.25 * pow(QQ3_error[QQ_index] / QQ3_mean[QQ_index], 2));
                    
                    printCalculationDetails("v3", idx, hist_name_qQ3[idx], hist_name_QQ3[QQ_index], v3_value, v3_error);

                    float bg_v3_value = qQ3_bg_mean[idx] / sqrt(QQ3_mean[QQ_index]);
                    float bg_v3_error = abs(bg_v3_value) * sqrt(pow(qQ3_bg_error[idx] / qQ3_bg_mean[idx], 2) + 0.25 * pow(QQ3_error[QQ_index] / QQ3_mean[QQ_index], 2));

                    // Calculate left background v3 and v3_error
                    float left_bg_v3_value = qQ3_bg_left_mean[idx] / sqrt(QQ3_mean[QQ_index]);
                    float left_bg_v3_error = abs(left_bg_v3_value) * sqrt(pow(qQ3_bg_left_error[idx] / qQ3_bg_left_mean[idx], 2) + 0.25 * pow(QQ3_error[QQ_index] / QQ3_mean[QQ_index], 2));

                    float v3_corrected = v3_value * (1 + (1 / SB)) - ((1 / SB) * bg_v3_value);
                    
                    float v3_corrected_error = sqrt(
                        pow((1 + (1 / SB)) * v3_error, 2) + // Error contribution from v3_signal
                        pow((-1 / SB) * bg_v3_error, 2) +   // Error contribution from v3_bg (note the negative sign is squared, having no effect)
                        pow((-(v3_value - bg_v3_value) / (SB * SB)) * SBerror, 2) // Error contribution from SB
                    );
                    

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

        // Clean up
        delete file;
    }
    
    // Check and execute Plot_vN
    if (Plot_vN_bool) {
        Plot_vN();
    }
}

