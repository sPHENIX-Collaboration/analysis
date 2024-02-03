#include <TFile.h>
#include <TH1F.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

bool plotEtaPhiHists = false; // Set this to true or false, IF TRUE, it will output eta phi hists for each pT and centrality bin
/*
 Only set below bool to true if already calculated vN and outputted for one set of cuts to a CSV
 */
bool Plot_vN_bool = true; // Set this to true or false for vN plotting--IF TRUE, then vN will NOT BE re appended/calculated in the CSV being used

void ReadPHENIXData(std::string filePath,
                    std::vector<double>& vN_0_10,
                    std::vector<double>& vN_0_10_Errors,
                    std::vector<double>& vN_0_10_Errors_Negative,
                    std::vector<double>& vN_10_20,
                    std::vector<double>& vN_10_20_Errors,
                    std::vector<double>& vN_10_20_Errors_Negative,
                    std::vector<double>& vN_20_30,
                    std::vector<double>& vN_20_30_Errors,
                    std::vector<double>& vN_20_30_Errors_Negative,
                    std::vector<double>& vN_30_40,
                    std::vector<double>& vN_30_40_Errors,
                    std::vector<double>& vN_30_40_Errors_Negative,
                    std::vector<double>& vN_40_50,
                    std::vector<double>& vN_40_50_Errors,
                    std::vector<double>& vN_40_50_Errors_Negative,
                    std::vector<double>& vN_50_60,
                    std::vector<double>& vN_50_60_Errors,
                    std::vector<double>& vN_50_60_Errors_Negative) {
    std::ifstream file(filePath);
    std::string line;
    double vN, error_pos, error_neg;
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

        vN = std::stod(rowData.at(2));
        error_pos = std::stod(rowData.at(rowData.size() - 2)); // stat. +
        error_neg = std::stod(rowData.back()); // stat. -
        
        // Assigning to the correct centrality vector based on row count
        if (rowCounter < 6) { // 0-10%
            vN_0_10.push_back(vN);
            vN_0_10_Errors.push_back(error_pos);
            vN_0_10_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for vN_0_10:\033[0m " << vN << " \u00B1 " << error_pos << std::endl;
        } else if (rowCounter < 12) { // 10-20%
            vN_10_20.push_back(vN);
            vN_10_20_Errors.push_back(error_pos);
            vN_10_20_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for vN_10_20:\033[0m " << vN << " \u00B1 " << error_pos << std::endl;
        } else if (rowCounter < 18) { // 20-30%
            vN_20_30.push_back(vN);
            vN_20_30_Errors.push_back(error_pos);
            vN_20_30_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for vN_20_30:\033[0m " << vN << " \u00B1 " << error_pos << std::endl;
        } else if (rowCounter < 24) { // 30-40%
            vN_30_40.push_back(vN);
            vN_30_40_Errors.push_back(error_pos);
            vN_30_40_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for vN_30_40:\033[0m " << vN << " \u00B1 " << error_pos << std::endl;
        } else if (rowCounter < 30) { // 40-50%
            vN_40_50.push_back(vN);
            vN_40_50_Errors.push_back(error_pos);
            vN_40_50_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for vN_40_50:\033[0m " << vN << " \u00B1 " << error_pos << std::endl;
        } else { // 50-60%
            vN_50_60.push_back(vN);
            vN_50_60_Errors.push_back(error_pos);
            vN_50_60_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for vN_50_60:\033[0m " << vN << " \u00B1 " << error_pos << std::endl;
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
void Plot_vN() {
    std::string filePath = "/Users/patsfan753/Desktop/UpdatedCSV_E05_A05_Chi4_1_31_24.csv";

    std::ifstream file(filePath);
    std::string line;
    std::vector<double> ptCenters = {2.25, 2.75, 3.25, 3.75, 4.25, 4.75}; // Mid-points of pT ranges
    std::vector<double> vN_0_20, vN_20_40, vN_40_60, bg_vN_0_20, bg_vN_20_40, bg_vN_40_60, corrected_vN_0_20, corrected_vN_20_40, corrected_vN_40_60; // vN values for different centralities
    std::vector<double> vN_0_20_Errors, vN_20_40_Errors, vN_40_60_Errors, bg_vN_0_20_Errors, bg_vN_20_40_Errors, bg_vN_40_60_Errors, corrected_vN_0_20_Errors, corrected_vN_20_40_Errors, corrected_vN_40_60_Errors; // Errors for vN values
    double vN, vN_error, bg_vN, bg_vN_error, vN_corrected, vN_error_corrected;

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

        // Converting string to double for vN and its error
        vN = std::stod(rowData.at(rowData.size() - 8)); // 8th last column is vN
        vN_error = std::stod(rowData.at(rowData.size() - 7)); // 7th last column is vN
        // Converting string to double for vN and its error
        bg_vN = std::stod(rowData.at(rowData.size() - 6)); // 6th last column is vN
        bg_vN_error = std::stod(rowData.at(rowData.size() - 5)); // 5th last column is vN error

        vN_corrected = std::stod(rowData.at(rowData.size() - 2)); // Second last column is vN corrected
        vN_error_corrected = std::stod(rowData.back()); // Last column is vN corrected error

        // Mapping index to centrality and pT
        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            // 40-60% centrality
            vN_40_60.push_back(vN);
            vN_40_60_Errors.push_back(vN_error);
            
            bg_vN_40_60.push_back(bg_vN);
            bg_vN_40_60_Errors.push_back(bg_vN_error);
            
            corrected_vN_40_60.push_back(vN_corrected);
            corrected_vN_40_60_Errors.push_back(vN_error_corrected);
            
        } else if (index >= 6 && index <= 11) {
            // 20-40% centrality
            vN_20_40.push_back(vN);
            vN_20_40_Errors.push_back(vN_error);

            bg_vN_20_40.push_back(bg_vN);
            bg_vN_20_40_Errors.push_back(bg_vN_error);
            
            corrected_vN_20_40.push_back(vN_corrected);
            corrected_vN_20_40_Errors.push_back(vN_error_corrected);
            
        } else if (index >= 12 && index <= 17) {
            // 0-20% centrality
            vN_0_20.push_back(vN);
            vN_0_20_Errors.push_back(vN_error);
            
            bg_vN_0_20.push_back(bg_vN);
            bg_vN_0_20_Errors.push_back(bg_vN_error);
            
            corrected_vN_0_20.push_back(vN_corrected);
            corrected_vN_0_20_Errors.push_back(vN_error_corrected);
        }
    }

    // Close the file as we have finished reading the data
    file.close();
    
    // Create and draw graph for 40-60% centrality
    TCanvas *c1 = new TCanvas("c1", "vN vs pT 0-20%", 800, 600);
    TGraphErrors *graph_0_20 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &vN_0_20[0], 0, &vN_0_20_Errors[0]);
    graph_0_20->SetMarkerColor(kBlue);
    graph_0_20->SetMarkerStyle(21);
    graph_0_20->SetMarkerSize(1.1);
    graph_0_20->SetTitle("Diphoton #it{v}_{2} vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    graph_0_20->Draw("AP");
    graph_0_20->GetXaxis()->SetLimits(2.0, 5.0);
    graph_0_20->GetXaxis()->SetNdivisions(010);
    graph_0_20->SetMinimum(0.0); // Set the minimum y value
    graph_0_20->SetMaximum(0.25); // Set the maximum y value
    graph_0_20->SetMinimum(-0.2); // Set the minimum y value
    graph_0_20->SetMaximum(0.5); // Set the maximum y value

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

    
    c1->Modified();
    c1->Update();
    c1->SaveAs("/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/v2/0_20/vN_vs_pT_0_20.png");


    // Create and draw graph for 20-40% centrality
    TCanvas *c2 = new TCanvas("c2", "vN vs pT 20-40%", 800, 600);
    TGraphErrors *graph_20_40 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &vN_20_40[0], 0, &vN_20_40_Errors[0]);
    graph_20_40->SetMarkerColor(kBlue);
    graph_20_40->SetMarkerStyle(21);
    graph_20_40->SetMarkerSize(1.1);
    graph_20_40->SetTitle("Diphoton #it{v}_{2} vs #it{p}_{T} 20-40% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    graph_20_40->Draw("AP");
    graph_20_40->GetXaxis()->SetLimits(2.0, 5.0);
    graph_20_40->GetXaxis()->SetNdivisions(010);
    graph_20_40->SetMinimum(0.0); // Set the minimum y value
    graph_20_40->SetMaximum(0.5); // Set the maximum y value
    c2->SetTicks();
    
    //Output cut information
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
    c2->SaveAs("/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/v2/20_40/vN_vs_pT_20_40.png");

    
    
    
    // Create and draw graph for 40-60% centrality
    TCanvas *c3 = new TCanvas("c3", "vN vs pT 40-60%", 800, 600);
    TGraphErrors *graph_40_60 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &vN_40_60[0], 0, &vN_40_60_Errors[0]);
    graph_40_60->SetMarkerColor(kBlue);
    graph_40_60->SetMarkerStyle(21);
    graph_40_60->SetMarkerSize(1.1);
    graph_40_60->SetTitle("Diphoton #it{v}_{2} vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    graph_40_60->Draw("AP");
    graph_40_60->GetXaxis()->SetLimits(2.0, 5.0);
    graph_40_60->GetXaxis()->SetNdivisions(010);
    graph_40_60->SetMinimum(0.0); // Set the minimum y value
    graph_40_60->SetMaximum(0.25); // Set the maximum y value
    graph_40_60->SetMinimum(-0.2); // Set the minimum y value
    graph_40_60->SetMaximum(0.7); // Set the maximum y value

    c3->SetTicks();
    
    //Output cut information
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
    c3->SaveAs("/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/v2/40_60/vN_vs_pT_40_60.png");
    

    // Create and draw graph for 40-60% centrality
    TCanvas *c4 = new TCanvas("c4", "Background vN vs pT 0-20%", 800, 600);
    TGraphErrors *bg_graph_0_20 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &bg_vN_0_20[0], 0, &bg_vN_0_20_Errors[0]);
    bg_graph_0_20->SetMarkerColor(kBlue);
    bg_graph_0_20->SetMarkerStyle(21);
    bg_graph_0_20->SetMarkerSize(1.1);
    bg_graph_0_20->SetTitle("Background #it{v}_{2} vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    bg_graph_0_20->Draw("AP");
    bg_graph_0_20->GetXaxis()->SetLimits(2.0, 5.0);
    bg_graph_0_20->GetXaxis()->SetNdivisions(010);
    bg_graph_0_20->SetMinimum(0.0); // Set the minimum y value
    bg_graph_0_20->SetMaximum(0.25); // Set the maximum y value
    bg_graph_0_20->SetMinimum(-0.2); // Set the minimum y value
    bg_graph_0_20->SetMaximum(0.5); // Set the maximum y value

    c4->SetTicks();
    
    //Output cut information
    TLatex latex020_bg;
    latex020_bg.SetTextSize(0.03);
    latex020_bg.SetTextAlign(12);
    latex020_bg.SetNDC();

    latex020_bg.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex020_bg.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex020_bg.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex020_bg.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");

    
    c4->Modified();
    c4->Update();
    c4->SaveAs("/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/v2/0_20/bg_vN_vs_pT_0_20.png");


    // Create and draw graph for 20-40% centrality
    TCanvas *c5 = new TCanvas("c5", "Background vN vs pT 20-40%", 800, 600);
    TGraphErrors *bg_graph_20_40 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &bg_vN_20_40[0], 0, &bg_vN_20_40_Errors[0]);
    bg_graph_20_40->SetMarkerColor(kBlue);
    bg_graph_20_40->SetMarkerStyle(21);
    bg_graph_20_40->SetMarkerSize(1.1);
    bg_graph_20_40->SetTitle("Background #it{v}_{2} vs #it{p}_{T} 20-40% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    bg_graph_20_40->Draw("AP");
    bg_graph_20_40->GetXaxis()->SetLimits(2.0, 5.0);
    bg_graph_20_40->GetXaxis()->SetNdivisions(010);
    bg_graph_20_40->SetMinimum(0.0); // Set the minimum y value
    bg_graph_20_40->SetMaximum(0.5); // Set the maximum y value
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
    c5->SaveAs("/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/v2/20_40/bg_vN_vs_pT_20_40.png");

    
    
    
    // Create and draw graph for 40-60% centrality
    TCanvas *c6 = new TCanvas("c3", "Background vN vs pT 40-60%", 800, 600);
    TGraphErrors *graph_40_60_bg = new TGraphErrors(ptCenters.size(), &ptCenters[0], &bg_vN_40_60[0], 0, &bg_vN_40_60_Errors[0]);
    graph_40_60_bg->SetMarkerColor(kBlue);
    graph_40_60_bg->SetMarkerStyle(21);
    graph_40_60_bg->SetMarkerSize(1.1);
    graph_40_60_bg->SetTitle("Background #it{v}_{2} vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    graph_40_60_bg->Draw("AP");
    graph_40_60_bg->GetXaxis()->SetLimits(2.0, 5.0);
    graph_40_60_bg->GetXaxis()->SetNdivisions(010);
    graph_40_60_bg->SetMinimum(0.0); // Set the minimum y value
    graph_40_60_bg->SetMaximum(0.25); // Set the maximum y value
    graph_40_60_bg->SetMinimum(-0.2); // Set the minimum y value
    graph_40_60_bg->SetMaximum(0.7); // Set the maximum y value

    c6->SetTicks();
    
    //Output cut information
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
    c6->SaveAs("/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/v2/40_60/bg_vN_vs_pT_40_60.png");
    
    
    // Create and draw graph for 40-60% centrality
    TCanvas *c7 = new TCanvas("c4", "Corrected vN vs pT 0-20%", 800, 600);
    TGraphErrors *corrected_graph_0_20 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &corrected_vN_0_20[0], 0, &corrected_vN_0_20_Errors[0]);
    corrected_graph_0_20->SetMarkerColor(kBlue);
    corrected_graph_0_20->SetMarkerStyle(21);
    corrected_graph_0_20->SetMarkerSize(1.1);
    corrected_graph_0_20->SetTitle("Corrected #it{v}_{2} vs #it{p}_{T} 0-20% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    corrected_graph_0_20->Draw("AP");
    corrected_graph_0_20->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_graph_0_20->GetXaxis()->SetNdivisions(010);
    corrected_graph_0_20->SetMinimum(-0.4); // Set the minimum y value
    corrected_graph_0_20->SetMaximum(2.5); // Set the maximum y value

    c7->SetTicks();
    
    //Output cut information
    TLatex latex020_correct;
    latex020_correct.SetTextSize(0.03);
    latex020_correct.SetTextAlign(12);
    latex020_correct.SetNDC();

    latex020_correct.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex020_correct.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex020_correct.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex020_correct.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");

    
    c7->Modified();
    c7->Update();
    c7->SaveAs("/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/v2/0_20/vN_vs_pT_0_20_corrected.png");


    // Create and draw graph for 20-40% centrality
    TCanvas *c8 = new TCanvas("c8", "Corrected vN vs pT 20-40%", 800, 600);
    TGraphErrors *corrected_graph_20_40 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &corrected_vN_20_40[0], 0, &corrected_vN_20_40_Errors[0]);
    corrected_graph_20_40->SetMarkerColor(kBlue);
    corrected_graph_20_40->SetMarkerStyle(21);
    corrected_graph_20_40->SetMarkerSize(1.1);
    corrected_graph_20_40->SetTitle("Background #it{v}_{2} vs #it{p}_{T} 20-40% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    corrected_graph_20_40->Draw("AP");
    corrected_graph_20_40->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_graph_20_40->GetXaxis()->SetNdivisions(010);
    corrected_graph_20_40->SetMinimum(-0.8); // Set the minimum y value
    corrected_graph_20_40->SetMaximum(1.0); // Set the maximum y value
    c8->SetTicks();
    
    //Output cut information
    TLatex latex2040_corr;
    latex2040_corr.SetTextSize(0.03);
    latex2040_corr.SetTextAlign(12);
    latex2040_corr.SetNDC();

    latex2040_corr.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex2040_corr.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex2040_corr.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex2040_corr.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");

    
    c8->Modified();
    c8->Update();
    c8->SaveAs("/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/v2/20_40/vN_vs_pT_20_40_corrected.png");

    
    
    
    // Create and draw graph for 40-60% centrality
    TCanvas *c9 = new TCanvas("c9", "Corrected vN vs pT 40-60%", 800, 600);
    TGraphErrors *graph_40_60_corrected = new TGraphErrors(ptCenters.size(), &ptCenters[0], &corrected_vN_40_60[0], 0, &corrected_vN_40_60_Errors[0]);
    graph_40_60_corrected->SetMarkerColor(kBlue);
    graph_40_60_corrected->SetMarkerStyle(21);
    graph_40_60_corrected->SetMarkerSize(1.1);
    graph_40_60_corrected->SetTitle("Corrected #it{v}_{2} vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    graph_40_60_corrected->Draw("AP");
    graph_40_60_corrected->GetXaxis()->SetLimits(2.0, 5.0);
    graph_40_60_corrected->GetXaxis()->SetNdivisions(010);
    graph_40_60_corrected->SetMinimum(0.0); // Set the minimum y value
    graph_40_60_corrected->SetMaximum(0.25); // Set the maximum y value
    graph_40_60_corrected->SetMinimum(-0.2); // Set the minimum y value
    graph_40_60_corrected->SetMaximum(0.7); // Set the maximum y value

    c9->SetTicks();
    
    //Output cut information
    TLatex latex4060_corr;
    latex4060_corr.SetTextSize(0.03);
    latex4060_corr.SetTextAlign(12);
    latex4060_corr.SetNDC();

    latex4060_corr.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex4060_corr.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex4060_corr.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex4060_corr.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");

    
    c9->Modified();
    c9->Update();
    c9->SaveAs("/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/v2/40_60/vN_vs_pT_40_60_corrected.png");
    
    
    
    
    std::string phenixFilePath = "/Users/patsfan753/Desktop/FinalCleanedPhenix.csv";
    std::vector<double> vN_0_10, vN_10_20, vN_20_30, vN_30_40, vN_40_50, vN_50_60;
    std::vector<double> vN_0_10_Errors, vN_10_20_Errors, vN_20_30_Errors, vN_30_40_Errors, vN_40_50_Errors, vN_50_60_Errors;
    std::vector<double> vN_0_10_Errors_Negative, vN_10_20_Errors_Negative, vN_20_30_Errors_Negative, vN_30_40_Errors_Negative, vN_40_50_Errors_Negative, vN_50_60_Errors_Negative;

    // Read in the data and errors
    ReadPHENIXData(phenixFilePath, vN_0_10, vN_0_10_Errors, vN_0_10_Errors_Negative,
                   vN_10_20, vN_10_20_Errors, vN_10_20_Errors_Negative,
                   vN_20_30, vN_20_30_Errors, vN_20_30_Errors_Negative,
                   vN_30_40, vN_30_40_Errors, vN_30_40_Errors_Negative,
                   vN_40_50, vN_40_50_Errors, vN_40_50_Errors_Negative,
                   vN_50_60, vN_50_60_Errors, vN_50_60_Errors_Negative);

    
    // Define the offsets for jitter
    double offset = 0.06;
    
    // Define the marker styles and colors for clarity
    int markerStyle_sPHENIX = 21; // Square for 40-60%
    int markerStyle_PHENIX = 20; // Circle for 40-50%

    // Define colors for each centrality range
    int color_sPHENIX = kBlue;    // Red for 40-60%
    int color_PHENIX_low = kRed;   // Blue for 40-50%
    int color_PHENIX_high= kGreen+3;// Dark Green for 50-60%

    // Overlay vN_40_50 and vN_50_60 on the same plot as vN_40_60
    TCanvas *c10 = new TCanvas("c10", "Overlay 0-20%", 800, 600);
    graph_0_20->Draw("AP");  // This will be the base graph
    TGraphErrors *graph_0_10 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &vN_0_10[0], 0, &vN_10_20_Errors[0]);
    TGraphErrors *graph_10_20 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &vN_0_10[0], 0, &vN_10_20_Errors[0]);
    
    // Apply jitter to x values for graph_40_50 and graph_50_60
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_0_10->SetPoint(i, ptCenters[i] - offset, vN_0_10[i]);
        graph_10_20->SetPoint(i, ptCenters[i] + offset, vN_10_20[i]);
    }
    // Adjust the color and style for the 40-60% plot
    graph_0_20->SetMarkerColor(color_sPHENIX);
    graph_0_20->SetMarkerStyle(markerStyle_sPHENIX);

    // Set the styles and colors for the additional graphs
    graph_0_10->SetMarkerColor(color_PHENIX_low);
    graph_0_10->SetMarkerStyle(markerStyle_PHENIX);
    graph_10_20->SetMarkerColor(color_PHENIX_high);
    graph_10_20->SetMarkerStyle(markerStyle_PHENIX);

    // Draw the additional graphs with the new styles and colors
    graph_0_10->Draw("P SAME");
    graph_10_20->Draw("P SAME");

    printOverlayData(ptCenters,
                     vN_0_10, vN_0_10_Errors,
                     vN_10_20, vN_10_20_Errors,
                     vN_0_20, vN_0_20_Errors);

    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend1 = new TLegend(0.5, 0.7, 0.9, 0.9);
    legend1->AddEntry(graph_0_20, "0-20%, #bf{sPHENIX}", "p");
    legend1->AddEntry(graph_0_10, "0-10%, #bf{PHENIX} 2010", "p");
    legend1->AddEntry(graph_10_20, "10-20%, #bf{PHENIX} 2010", "p");
    legend1->Draw();

    c10->Modified();
    c10->Update();
    c10->SaveAs("/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/v2/0_20/vN_vs_pT_0_20_Overlay.png");

    
    
    
    

    // Overlay vN_20_30 and vN_30_40 on the same plot as vN_20_40
    TCanvas *c11 = new TCanvas("c11", "Overlay 20-40%", 800, 600);
    graph_20_40->Draw("AP");  // This will be the base graph
    TGraphErrors *graph_20_30 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &vN_20_30[0], 0, &vN_20_30_Errors[0]);
    TGraphErrors *graph_30_40 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &vN_30_40[0], 0, &vN_30_40_Errors[0]);
    
    // Apply jitter to x values for graph_20_30 and graph_30_40
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_20_30->SetPoint(i, ptCenters[i] - offset, vN_20_30[i]);
        graph_30_40->SetPoint(i, ptCenters[i] + offset, vN_30_40[i]);
    }
    
    // Adjust the color and style for the 40-60% plot
    graph_20_40->SetMarkerColor(color_sPHENIX);
    graph_20_40->SetMarkerStyle(markerStyle_sPHENIX);

    graph_20_30->SetMarkerColor(color_PHENIX_low);
    graph_20_30->SetMarkerStyle(markerStyle_PHENIX);
    graph_30_40->SetMarkerColor(color_PHENIX_high);
    graph_30_40->SetMarkerStyle(markerStyle_PHENIX);

    graph_20_30->Draw("P SAME");
    graph_30_40->Draw("P SAME");
    
    printOverlayData(ptCenters,
                     vN_20_30, vN_20_30_Errors,
                     vN_30_40, vN_30_40_Errors,
                     vN_20_40, vN_20_40_Errors);

    // Create a legend for the first overlay plot
    TLegend *legend2 = new TLegend(0.5, 0.7, 0.9, 0.9);
    legend2->AddEntry(graph_20_40, "20-40%, #bf{sPHENIX}", "p");
    legend2->AddEntry(graph_20_30, "20-30%, #bf{PHENIX} 2010", "p");
    legend2->AddEntry(graph_30_40, "30-40%, #bf{PHENIX} 2010", "p");
    legend2->Draw();

    c11->Modified();
    c11->Update();
    c11->SaveAs("/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/v2/20_40/vN_vs_pT_20_40_Overlay.png");
    
    
    
    

    // Overlay vN_40_50 and vN_50_60 on the same plot as vN_40_60
    TCanvas *c12 = new TCanvas("c12", "Overlay 40-60%", 800, 600);
    graph_40_60->Draw("AP");  // This will be the base graph
    TGraphErrors *graph_40_50 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &vN_40_50[0], 0, &vN_40_50_Errors[0]);
    TGraphErrors *graph_50_60 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &vN_50_60[0], 0, &vN_50_60_Errors[0]);
    
    // Apply jitter to x values for graph_40_50 and graph_50_60
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_40_50->SetPoint(i, ptCenters[i] - offset, vN_40_50[i]);
        graph_50_60->SetPoint(i, ptCenters[i] + offset, vN_50_60[i]);
    }
    // Adjust the color and style for the 40-60% plot
    graph_40_60->SetMarkerColor(color_sPHENIX);
    graph_40_60->SetMarkerStyle(markerStyle_sPHENIX); // Star marker style for 40-60%

    // Set the styles and colors for the additional graphs
    graph_40_50->SetMarkerColor(color_PHENIX_low);
    graph_40_50->SetMarkerStyle(markerStyle_PHENIX);
    graph_50_60->SetMarkerColor(color_PHENIX_high);
    graph_50_60->SetMarkerStyle(markerStyle_PHENIX);

    // Draw the additional graphs with the new styles and colors
    graph_40_50->Draw("P SAME");
    graph_50_60->Draw("P SAME");

    printOverlayData(ptCenters,
                     vN_40_50, vN_40_50_Errors,
                     vN_50_60, vN_50_60_Errors,
                     vN_40_60, vN_40_60_Errors);

    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend3 = new TLegend(0.5, 0.7, 0.9, 0.9);
    legend3->AddEntry(graph_40_60, "40-60%, #bf{sPHENIX}", "p");
    legend3->AddEntry(graph_40_50, "40-50%, #bf{PHENIX} 2010", "p");
    legend3->AddEntry(graph_50_60, "50-60%, #bf{PHENIX} 2010", "p");
    legend3->Draw();

    c12->Modified();
    c12->Update();
    c12->SaveAs("/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/v2/40_60/vN_vs_pT_40_60_Overlay.png");
    

    
    
    
    // Overlay vN_40_50 and vN_50_60 on the same plot as vN_40_60
    TCanvas *c13 = new TCanvas("c13", "Corrected Overlay 0-20%", 800, 600);
    corrected_graph_0_20->Draw("AP");  // This will be the base graph
    
    // Apply jitter to x values for graph_40_50 and graph_50_60
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_0_10->SetPoint(i, ptCenters[i] - offset, vN_0_10[i]);
        graph_10_20->SetPoint(i, ptCenters[i] + offset, vN_10_20[i]);
    }
    // Adjust the color and style for the 40-60% plot
    corrected_graph_0_20->SetMarkerColor(color_sPHENIX);
    corrected_graph_0_20->SetMarkerStyle(markerStyle_sPHENIX);

    // Set the styles and colors for the additional graphs
    graph_0_10->SetMarkerColor(color_PHENIX_low);
    graph_0_10->SetMarkerStyle(markerStyle_PHENIX);
    graph_10_20->SetMarkerColor(color_PHENIX_high);
    graph_10_20->SetMarkerStyle(markerStyle_PHENIX);

    // Draw the additional graphs with the new styles and colors
    graph_0_10->Draw("P SAME");
    graph_10_20->Draw("P SAME");

    printOverlayData(ptCenters,
                     vN_0_10, vN_0_10_Errors,
                     vN_10_20, vN_10_20_Errors,
                     corrected_vN_0_20, corrected_vN_0_20_Errors);

    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend4 = new TLegend(0.5, 0.7, 0.9, 0.9);
    legend4->AddEntry(graph_0_20, "0-20%, #bf{sPHENIX}", "p");
    legend4->AddEntry(graph_0_10, "0-10%, #bf{PHENIX} 2010", "p");
    legend4->AddEntry(graph_10_20, "10-20%, #bf{PHENIX} 2010", "p");
    legend4->Draw();

    c13->Modified();
    c13->Update();
    c13->SaveAs("/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/v2/0_20/vN_vs_pT_0_20_Overlay_corrected.png");

    
    
    
    

    // Overlay vN_20_30 and vN_30_40 on the same plot as vN_20_40
    TCanvas *c14 = new TCanvas("c14", "Corrected Overlay 20-40%", 800, 600);
    corrected_graph_20_40->Draw("AP");  // This will be the base graph

    
    // Apply jitter to x values for graph_20_30 and graph_30_40
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_20_30->SetPoint(i, ptCenters[i] - offset, vN_20_30[i]);
        graph_30_40->SetPoint(i, ptCenters[i] + offset, vN_30_40[i]);
    }
    
    // Adjust the color and style for the 40-60% plot
    corrected_graph_20_40->SetMarkerColor(color_sPHENIX);
    corrected_graph_20_40->SetMarkerStyle(markerStyle_sPHENIX);

    graph_20_30->SetMarkerColor(color_PHENIX_low);
    graph_20_30->SetMarkerStyle(markerStyle_PHENIX);
    graph_30_40->SetMarkerColor(color_PHENIX_high);
    graph_30_40->SetMarkerStyle(markerStyle_PHENIX);

    graph_20_30->Draw("P SAME");
    graph_30_40->Draw("P SAME");
    
    printOverlayData(ptCenters,
                     vN_20_30, vN_20_30_Errors,
                     vN_30_40, vN_30_40_Errors,
                     corrected_vN_20_40, corrected_vN_20_40_Errors);

    // Create a legend for the first overlay plot
    TLegend *legend5 = new TLegend(0.5, 0.7, 0.9, 0.9);
    legend5->AddEntry(graph_20_40, "20-40%, #bf{sPHENIX}", "p");
    legend5->AddEntry(graph_20_30, "20-30%, #bf{PHENIX} 2010", "p");
    legend5->AddEntry(graph_30_40, "30-40%, #bf{PHENIX} 2010", "p");
    legend5->Draw();

    c14->Modified();
    c14->Update();
    c14->SaveAs("/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/v2/20_40/vN_vs_pT_20_40_Overlay_corrected.png");
    
    
    
    

    // Overlay vN_40_50 and vN_50_60 on the same plot as vN_40_60
    TCanvas *c15 = new TCanvas("c15", "Overlay 40-60%", 800, 600);
    graph_40_60_corrected->Draw("AP");  // This will be the base graph

    
    // Apply jitter to x values for graph_40_50 and graph_50_60
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_40_50->SetPoint(i, ptCenters[i] - offset, vN_40_50[i]);
        graph_50_60->SetPoint(i, ptCenters[i] + offset, vN_50_60[i]);
    }
    // Adjust the color and style for the 40-60% plot
    graph_40_60_corrected->SetMarkerColor(color_sPHENIX);
    graph_40_60_corrected->SetMarkerStyle(markerStyle_sPHENIX); // Star marker style for 40-60%

    // Set the styles and colors for the additional graphs
    graph_40_50->SetMarkerColor(color_PHENIX_low);
    graph_40_50->SetMarkerStyle(markerStyle_PHENIX);
    graph_50_60->SetMarkerColor(color_PHENIX_high);
    graph_50_60->SetMarkerStyle(markerStyle_PHENIX);

    // Draw the additional graphs with the new styles and colors
    graph_40_50->Draw("P SAME");
    graph_50_60->Draw("P SAME");

    printOverlayData(ptCenters,
                     vN_40_50, vN_40_50_Errors,
                     vN_50_60, vN_50_60_Errors,
                     corrected_vN_40_60, corrected_vN_40_60_Errors);

    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend6 = new TLegend(0.5, 0.7, 0.9, 0.9);
    legend6->AddEntry(graph_40_60, "40-60%, #bf{sPHENIX}", "p");
    legend6->AddEntry(graph_40_50, "40-50%, #bf{PHENIX} 2010", "p");
    legend6->AddEntry(graph_50_60, "50-60%, #bf{PHENIX} 2010", "p");
    legend6->Draw();

    c15->Modified();
    c15->Update();
    c15->SaveAs("/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/plotOutput/v2/40_60/vN_vs_pT_40_60_Overlay_corrected.png");

}


void OutputEtaPhiHists() {
    TFile *file = new TFile("/Users/patsfan753/Desktop/test.root", "READ");
    if (!file || file->IsZombie()) {
        std::cout << "Error opening file" << std::endl;
        return;
    }

    TDirectory *qaDir = file->GetDirectory("QA");
    if (!qaDir) {
        std::cout << "QA directory not found" << std::endl;
        return;
    }

    TDirectory *etaPhiDir = qaDir->GetDirectory("h2Pi0EtaPhi");
    if (!etaPhiDir) {
        std::cout << "h2Pi0EtaPhi directory not found" << std::endl;
        return;
    }

    etaPhiDir->cd();
    TIter next(etaPhiDir->GetListOfKeys());
    TKey *key;

    while ((key = (TKey*)next())) {
        TClass *cl = gROOT->GetClass(key->GetClassName());
        if (!cl->InheritsFrom("TH2")) continue;

        TH2 *h = (TH2*)key->ReadObj();
        h->SetStats(kFALSE);

        // Find the maximum value in the histogram
        double maxBinContent = h->GetMaximum();

        // Set the Z-axis range based on the maximum content
        h->GetZaxis()->SetRangeUser(0, maxBinContent);

        TString savePath = "/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/etaPhi_Pi0_Plots_vN_output/";
        savePath += h->GetName();
        savePath += ".png";

        // Output histogram name and number of events
        std::cout << "Plotting: " << h->GetName() << ", Number of events: " << h->GetEntries() << std::endl;

        TCanvas *c = new TCanvas("c", "Canvas", 800, 600);
        h->Draw("COLZ");
        c->SaveAs(savePath);
        delete c;
    }

    file->Close();
    delete file;
}
void v_n_Calculator() {
    // Check if plotEtaPhiHists is true and call OutputEtaPhiHists
    if (plotEtaPhiHists) {
        OutputEtaPhiHists();
    }
    // Define the histogram names for QQ
    // Open the ROOT file
    TFile *file = new TFile("/Users/patsfan753/Desktop/test.root", "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file or file is not a valid ROOT file" << std::endl;
        return;
    }

    // Define the histogram names for QQ within the vn/QQ directory
    std::string hist_name_QQ[3] = {"vn/QQ/hQQ_40-60", "vn/QQ/hQQ_20-40", "vn/QQ/hQQ_0-20"};

    // Define the histogram names for qQ within the vn/qQ directory
    std::string hist_name_qq[18];
    for (int i = 0; i < 18; ++i) {
        hist_name_qq[i] = "vn/qQ/hqQ_" + std::to_string(i);
    }
    
    std::string hist_name_qq_bg[18];
    for (int i = 0; i < 18; ++i) {
        hist_name_qq_bg[i] = "vn/qQ_bg/hqQ_bg_" + std::to_string(i);
    }

    std::string hist_name_qq_bg_left[18];
    for (int i = 0; i < 18; ++i) {
        hist_name_qq_bg_left[i] = "vn/qQ_bg_left/hqQ_bg_left_" + std::to_string(i);
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
    float QQ_mean[3], QQ_error[3];
    for (int i = 0; i < 3; ++i) {
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
        
        //std::cout << "QQ Index " << i << " (" << hist_name_QQ[i] << "): Mean = " << QQ_mean[i] << ", Error = " << QQ_error[i] << std::endl;
    }
    
    
    // Retrieve and calculate qQ means and their errors
    float qQ_mean[18], qQ_error[18];
    for (int i = 0; i < 18; ++i) {
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
        
        //std::cout << "qQ Index " << i << " (" << hist_name_qq[i] << "): Mean = " << qQ_mean[i] << ", Error = " << qQ_error[i] << std::endl;
    }
    
    // Retrieve and calculate background qQ means and their errors
    float qQ_bg_mean[18], qQ_bg_error[18];
    for (int i = 0; i < 18; ++i) {
        TH1F *h_qQ_bg = (TH1F*)file->Get(hist_name_qq_bg[i].c_str());
        if (!h_qQ_bg) {
            std::cerr << "Error: Background qQ histogram " << hist_name_qq_bg[i] << " not found" << std::endl;
            continue; // Skip to the next iteration instead of returning to process all histograms
        }
        qQ_bg_mean[i] = h_qQ_bg->GetMean();
        float stdDev = h_qQ_bg->GetStdDev();
        int nEntries = h_qQ_bg->GetEntries();
        qQ_bg_error[i] = stdDev / sqrt(nEntries);
    }

    // Retrieve and calculate left background qQ means and their errors
    float qQ_bg_left_mean[18], qQ_bg_left_error[18];
    for (int i = 0; i < 18; ++i) {
        TH1F *h_qQ_bg_left = (TH1F*)file->Get(hist_name_qq_bg_left[i].c_str());
        if (!h_qQ_bg_left) {
            std::cerr << "Error: Left Background qQ histogram " << hist_name_qq_bg_left[i] << " not found" << std::endl;
            continue; // Use continue to skip this iteration, ensuring all histograms are processed
        }
        qQ_bg_left_mean[i] = h_qQ_bg_left->GetMean();
        float stdDev = h_qQ_bg_left->GetStdDev();
        int nEntries = h_qQ_bg_left->GetEntries();
        qQ_bg_left_error[i] = stdDev / sqrt(nEntries);
    }
    
    // User-defined cut values
    float userEnergy = 0.5;
    float userAsymm = 0.5;
    float userChi2 = 4;
    float userDeltaR = 0;

    if (!Plot_vN_bool) {
        std::ifstream inFile("/Users/patsfan753/Desktop/Desktop/AnalyzePi0s_Final/dataOutput/PlotByPlotOutput_2_1_24.csv");
        std::ofstream outFile("/Users/patsfan753/Desktop/UpdatedCSV_E05_A05_Chi4_1_31_24.csv");

        std::string line;
        int lineIndex = 0;

        while (getline(inFile, line)) {
            if (lineIndex == 0) {
                // Header line
                outFile << line << ",v2,v2_error,bg_v2,bg_v2_err,left_bg_v2,left_bg_v2_err,v2_corrected,v2_corrected_error" << std::endl;
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

                    float v2_value = qQ_mean[idx] / sqrt(QQ_mean[QQ_index]);
                    float v2_error = abs(v2_value) * sqrt(pow(qQ_error[idx] / qQ_mean[idx], 2) + 0.25 * pow(QQ_error[QQ_index] / QQ_mean[QQ_index], 2));

                    float bg_v2_value = qQ_bg_mean[idx] / sqrt(QQ_mean[QQ_index]);
                    float bg_v2_error = abs(bg_v2_value) * sqrt(pow(qQ_bg_error[idx] / qQ_bg_mean[idx], 2) + 0.25 * pow(QQ_error[QQ_index] / QQ_mean[QQ_index], 2));

                    // Calculate left background v2 and v2_error
                    float left_bg_v2_value = qQ_bg_left_mean[idx] / sqrt(QQ_mean[QQ_index]);
                    float left_bg_v2_error = abs(left_bg_v2_value) * sqrt(pow(qQ_bg_left_error[idx] / qQ_bg_left_mean[idx], 2) + 0.25 * pow(QQ_error[QQ_index] / QQ_mean[QQ_index], 2));

                    float v2_corrected = v2_value * (1 + (1 / SB)) - ((1 / SB) * bg_v2_value);
                    
                    float v2_corrected_error = sqrt(
                        pow((1 + (1 / SB)) * v2_error, 2) + // Error contribution from v2_signal
                        pow((-1 / SB) * bg_v2_error, 2) +   // Error contribution from v2_bg (note the negative sign is squared, having no effect)
                        pow((-(v2_value - bg_v2_value) / (SB * SB)) * SBerror, 2) // Error contribution from SB
                    );
                    
                    // Color and style codes for console output
                    const string RED = "\033[31;1m";
                    const string GREEN = "\033[32;1m";
                    const string YELLOW = "\033[33;1m";
                    const string BLUE = "\033[34;1m";
                    const string MAGENTA = "\033[35;1m";
                    const string CYAN = "\033[36;1m";
                    const string RESET = "\033[0m";

                    // Enhanced printout with color coding and bolding
                    cout << CYAN << "Analysis for Index: " << idx << RESET << endl;
                    cout << GREEN << "Signal-to-Background (S/B): " << RESET << SB << ", Error: " << SBerror << endl;
                    cout << YELLOW << "Signal v2: " << RESET << "Value: " << v2_value << ", Error: " << v2_error << endl;
                    cout << RED << "Background v2: " << RESET << "Value: " << bg_v2_value << ", Error: " << bg_v2_error << endl;
                    cout << BLUE << "Left Background v2: " << RESET << "Value: " << left_bg_v2_value << ", Error: " << left_bg_v2_error << endl;
                    cout << MAGENTA << "Corrected v2: " << RESET << "Value: " << v2_corrected << ", Error: " << v2_corrected_error << endl;

                    // Append calculated values and errors to CSV
                    outFile << line << "," << v2_value << "," << v2_error << "," << bg_v2_value << "," << bg_v2_error
                            << "," << left_bg_v2_value << "," << left_bg_v2_error << "," << v2_corrected << "," << v2_corrected_error << endl;

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

