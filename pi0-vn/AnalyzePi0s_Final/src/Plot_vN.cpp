#include <TGraph.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
void ReadPHENIXData(std::string filePath,
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

        vN = std::stod(rowData.at(2)); // Assuming vN is at the third column
        error_pos = std::stod(rowData.at(rowData.size() - 2)); // stat. +
        error_neg = std::stod(rowData.back()); // stat. -

        // Assigning to the correct centrality vector based on row count
        if (rowCounter < 6) { // 20-30%
            vN_20_30.push_back(vN);
            vN_20_30_Errors.push_back(error_pos);
            vN_20_30_Errors_Negative.push_back(fabs(error_neg));
        } else if (rowCounter < 12) { // 30-40%
            vN_30_40.push_back(vN);
            vN_30_40_Errors.push_back(error_pos);
            vN_30_40_Errors_Negative.push_back(fabs(error_neg));
        } else if (rowCounter < 18) { // 40-50%
            vN_40_50.push_back(vN);
            vN_40_50_Errors.push_back(error_pos);
            vN_40_50_Errors_Negative.push_back(fabs(error_neg));
        } else { // 50-60%
            vN_50_60.push_back(vN);
            vN_50_60_Errors.push_back(error_pos);
            vN_50_60_Errors_Negative.push_back(fabs(error_neg));
        }

        rowCounter++;
    }
    file.close();
}


void Plot_vN() {
    std::string filePath = "/Users/patsfan753/Desktop/TestUpdate1.csv";

    std::ifstream file(filePath);
    std::string line;
    std::vector<double> ptCenters = {2.25, 2.75, 3.25, 3.75, 4.25, 4.75}; // Mid-points of pT ranges
    std::vector<double> vN_20_40, vN_40_60; // vN values for different centralities
    std::vector<double> vN_20_40_Errors, vN_40_60_Errors; // Errors for vN values
    double vN, vN_error;

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
        vN = std::stod(rowData.at(rowData.size() - 2)); // Second last column is vN
        vN_error = std::stod(rowData.back()); // Last column is vN error

        // Mapping index to centrality and pT
        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            // 40-60% centrality
            vN_40_60.push_back(vN);
            vN_40_60_Errors.push_back(vN_error);
        } else if (index >= 6 && index <= 11) {
            // 20-40% centrality
            vN_20_40.push_back(vN);
            vN_20_40_Errors.push_back(vN_error);
        }
    }

    // Close the file as we have finished reading the data
    file.close();

    // Define marker style
    int markerStyle = 21;

    // Create and draw graph for 20-40% centrality
    TCanvas *c1 = new TCanvas("c1", "vN vs pT 20-40%", 800, 600);
    TGraphErrors *graph_20_40 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &vN_20_40[0], 0, &vN_20_40_Errors[0]);
    graph_20_40->SetMarkerColor(kBlue);
    graph_20_40->SetMarkerStyle(markerStyle);
    graph_20_40->SetMarkerSize(1.1);
    graph_20_40->SetTitle("#it{#pi^{0}} #it{v}_{2} vs #it{p}_{T} 20-40% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    graph_20_40->Draw("AP");
    graph_20_40->GetXaxis()->SetLimits(2.0, 5.0);
    graph_20_40->GetXaxis()->SetNdivisions(010);
    graph_20_40->SetMinimum(0.0); // Set the minimum y value
    graph_20_40->SetMaximum(0.5); // Set the maximum y value
    c1->SetTicks();
    
    //Output cut information
    TLatex latex2040;
    latex2040.SetTextSize(0.03);
    latex2040.SetTextAlign(12);
    latex2040.SetNDC();

    latex2040.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex2040.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex2040.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex2040.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    latex2040.DrawLatex(0.13, 0.7, "#bf{497.222 #leq MBD Charge < 955.741}");
    
    c1->Modified();
    c1->Update();
    c1->SaveAs("/Users/patsfan753/Desktop/vN_vs_pT_20_40.png");

    
    
    
    // Create and draw graph for 40-60% centrality
    TCanvas *c2 = new TCanvas("c2", "vN vs pT 40-60%", 800, 600);
    TGraphErrors *graph_40_60 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &vN_40_60[0], 0, &vN_40_60_Errors[0]);
    graph_40_60->SetMarkerColor(kBlue);
    graph_40_60->SetMarkerStyle(markerStyle);
    graph_40_60->SetMarkerSize(1.1);
    graph_40_60->SetTitle("#it{v}_{2} vs #it{p}_{T} 40-60% Centrality; #it{p}_{T} [GeV]; #it{v}_{2}");
    graph_40_60->Draw("AP");
    graph_40_60->GetXaxis()->SetLimits(2.0, 5.0);
    graph_40_60->GetXaxis()->SetNdivisions(010);
    graph_40_60->SetMinimum(0.0); // Set the minimum y value
    graph_40_60->SetMaximum(0.25); // Set the maximum y value
    graph_40_60->SetMinimum(0.0); // Set the minimum y value
    graph_40_60->SetMaximum(0.5); // Set the maximum y value

    c2->SetTicks();
    
    //Output cut information
    TLatex latex4060;
    latex4060.SetTextSize(0.03);
    latex4060.SetTextAlign(12);
    latex4060.SetNDC();

    latex2040.DrawLatex(0.13, 0.86, "Cuts (Inclusive):");
    latex2040.DrawLatex(0.13, 0.82, "#bf{Asymmetry < 0.5}");
    latex2040.DrawLatex(0.13, 0.78, "#bf{#chi^{2} < 4.0}");
    latex2040.DrawLatex(0.13, 0.74, "#bf{Cluster E #geq 0.5 GeV}");
    latex4060.DrawLatex(0.13, 0.7, "#bf{215.000 #leq MBD Charge < 497.222}");
    
    c2->Modified();
    c2->Update();
    c2->SaveAs("/Users/patsfan753/Desktop/vN_vs_pT_40_60.png");

    
    std::string phenixFilePath = "/Users/patsfan753/Desktop/Cleaned_PHENIX.csv";
    std::vector<double> vN_20_30, vN_30_40, vN_40_50, vN_50_60;
    std::vector<double> vN_20_30_Errors, vN_30_40_Errors, vN_40_50_Errors, vN_50_60_Errors;
    std::vector<double> vN_20_30_Errors_Negative, vN_30_40_Errors_Negative, vN_40_50_Errors_Negative, vN_50_60_Errors_Negative;

    // Read in the data and errors
    ReadPHENIXData(phenixFilePath, vN_20_30, vN_20_30_Errors, vN_20_30_Errors_Negative,
                   vN_30_40, vN_30_40_Errors, vN_30_40_Errors_Negative,
                   vN_40_50, vN_40_50_Errors, vN_40_50_Errors_Negative,
                   vN_50_60, vN_50_60_Errors, vN_50_60_Errors_Negative);

    
    // Define the offsets for jitter
    double offset = 0.06;
    
    // Print out the values and errors for the overlay plot 20-40%
    std::cout << "Overlay Plot 20-40%:\n";
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        std::cout << "pT: " << ptCenters[i] << ", 20-40% vN: " << vN_20_40[i] << " +/- " << vN_20_40_Errors[i];
        std::cout << ", 20-30% vN: " << vN_20_30[i] << " +/- " << vN_20_30_Errors[i];
        std::cout << ", 30-40% vN: " << vN_30_40[i] << " +/- " << vN_30_40_Errors[i] << std::endl;
    }

    // Overlay vN_20_30 and vN_30_40 on the same plot as vN_20_40
    TCanvas *c3 = new TCanvas("c3", "Overlay 20-40%", 800, 600);
    graph_20_40->Draw("AP");  // This will be the base graph
    TGraphErrors *graph_20_30 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &vN_20_30[0], 0, &vN_20_30_Errors[0]);
    TGraphErrors *graph_30_40 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &vN_30_40[0], 0, &vN_30_40_Errors[0]);
    
    // Apply jitter to x values for graph_20_30 and graph_30_40
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_20_30->SetPoint(i, ptCenters[i] - offset, vN_20_30[i]);
        graph_30_40->SetPoint(i, ptCenters[i] + offset, vN_30_40[i]);
    }

    graph_20_30->SetMarkerColor(kRed);
    graph_20_30->SetMarkerStyle(20);
    graph_30_40->SetMarkerColor(kGreen+3);
    graph_30_40->SetMarkerStyle(20);

    graph_20_30->Draw("P SAME");
    graph_30_40->Draw("P SAME");

    // Create a legend for the first overlay plot
    TLegend *legend1 = new TLegend(0.5, 0.7, 0.9, 0.9);
    legend1->AddEntry(graph_20_40, "20-40%, #bf{sPHENIX} data", "p");
    legend1->AddEntry(graph_20_30, "20-30%, #bf{PHENIX} data", "p");
    legend1->AddEntry(graph_30_40, "30-40%, #bf{PHENIX} data", "p");
    legend1->Draw();

    c3->Modified();
    c3->Update();
    c3->SaveAs("/Users/patsfan753/Desktop/vN_vs_pT_20_40_Overlay.png");
    
    // Print out the values and errors for the overlay plot 40-60%
    std::cout << "\nOverlay Plot 40-60%:\n";
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        std::cout << "pT: " << ptCenters[i] << ", 40-60% vN: " << vN_40_60[i] << " +/- " << vN_40_60_Errors[i];
        std::cout << ", 40-50% vN: " << vN_40_50[i] << " +/- " << vN_40_50_Errors[i];
        std::cout << ", 50-60% vN: " << vN_50_60[i] << " +/- " << vN_50_60_Errors[i] << std::endl;
    }
    
    // Define the marker styles and colors for clarity
    int markerStyle_40_60 = 21; // Square for 40-60%
    int markerStyle_40_50 = 20; // Circle for 40-50%
    int markerStyle_50_60 = 20; // Circle for 50-60%

    // Define colors for each centrality range
    int color_40_60 = kBlue;    // Red for 40-60%
    int color_40_50 = kRed;   // Blue for 40-50%
    int color_50_60 = kGreen+3;// Dark Green for 50-60%

    // Overlay vN_40_50 and vN_50_60 on the same plot as vN_40_60
    TCanvas *c4 = new TCanvas("c4", "Overlay 40-60%", 800, 600);
    graph_40_60->Draw("AP");  // This will be the base graph
    TGraphErrors *graph_40_50 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &vN_40_50[0], 0, &vN_40_50_Errors[0]);
    TGraphErrors *graph_50_60 = new TGraphErrors(ptCenters.size(), &ptCenters[0], &vN_50_60[0], 0, &vN_50_60_Errors[0]);
    
    // Apply jitter to x values for graph_40_50 and graph_50_60
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_40_50->SetPoint(i, ptCenters[i] - offset, vN_40_50[i]);
        graph_50_60->SetPoint(i, ptCenters[i] + offset, vN_50_60[i]);
    }
    // Adjust the color and style for the 40-60% plot
    graph_40_60->SetMarkerColor(color_40_60);
    graph_40_60->SetMarkerStyle(markerStyle_40_60); // Star marker style for 40-60%

    // Set the styles and colors for the additional graphs
    graph_40_50->SetMarkerColor(color_40_50);
    graph_40_50->SetMarkerStyle(markerStyle_40_50);
    graph_50_60->SetMarkerColor(color_50_60);
    graph_50_60->SetMarkerStyle(markerStyle_50_60);

    // Draw the additional graphs with the new styles and colors
    graph_40_50->Draw("P SAME");
    graph_50_60->Draw("P SAME");


    // Update the legend for the c4 canvas to include new marker styles
    TLegend *legend2 = new TLegend(0.5, 0.7, 0.9, 0.9);
    legend2->AddEntry(graph_40_60, "40-60%, #bf{sPHENIX} data", "p");
    legend2->AddEntry(graph_40_50, "40-50%, #bf{PHENIX} data", "p");
    legend2->AddEntry(graph_50_60, "50-60%, #bf{PHENIX} data", "p");
    legend2->Draw();

    c4->Modified();
    c4->Update();
    c4->SaveAs("/Users/patsfan753/Desktop/vN_vs_pT_40_60_Overlay.png");

    
    // Clean up
    delete c1;
    delete c2;
    delete graph_20_40;
    delete graph_40_60;

}
