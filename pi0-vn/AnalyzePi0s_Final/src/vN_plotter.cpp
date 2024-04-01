#include <TFile.h>
#include <TH1F.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

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
/*
 For seperating a CSV file to be overlayed
 */
struct Data {
    std::vector<double> corrected_v2_0_20, corrected_v2_20_40, corrected_v2_40_60;
    std::vector<double> corrected_v2_0_20_Errors, corrected_v2_20_40_Errors, corrected_v2_40_60_Errors;
    
    std::vector<double> corrected_v2_0_20_type3, corrected_v2_20_40_type3, corrected_v2_40_60_type3;
    std::vector<double> corrected_v2_0_20_Errors_type3, corrected_v2_20_40_Errors_type3, corrected_v2_40_60_Errors_type3;
    
    std::vector<double> corrected_v2_0_20_type4, corrected_v2_20_40_type4, corrected_v2_40_60_type4;
    std::vector<double> corrected_v2_0_20_Errors_type4, corrected_v2_20_40_Errors_type4, corrected_v2_40_60_Errors_type4;
    
    std::vector<double> signal_v2_0_20, signal_v2_20_40, signal_v2_40_60;
    std::vector<double> signal_v2_0_20_Errors, signal_v2_20_40_Errors, signal_v2_40_60_Errors;
    
    std::vector<double> bg_v2_0_20, bg_v2_20_40, bg_v2_40_60;
    std::vector<double> bg_v2_0_20_Errors, bg_v2_20_40_Errors, bg_v2_40_60_Errors;
    
    std::vector<double> bg_v2_0_20_type3, bg_v2_20_40_type3, bg_v2_40_60_type3;
    std::vector<double> bg_v2_0_20_Errors_type3, bg_v2_20_40_Errors_type3, bg_v2_40_60_Errors_type3;
    
    std::vector<double> bg_v2_0_20_type4, bg_v2_20_40_type4, bg_v2_40_60_type4;
    std::vector<double> bg_v2_0_20_Errors_type4, bg_v2_20_40_Errors_type4, bg_v2_40_60_Errors_type4;
    
    std::vector<double> corrected_v3_0_20, corrected_v3_20_40, corrected_v3_40_60;
    std::vector<double> corrected_v3_0_20_Errors, corrected_v3_20_40_Errors, corrected_v3_40_60_Errors;
};
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
        
        double v2_corrected_type3 = std::stod(rowData.at(3));
        double v2_error_corrected_type3 = std::stod(rowData.at(4));
        
        double v2_corrected_type4 = std::stod(rowData.at(5));
        double v2_error_corrected_type4 = std::stod(rowData.at(6));

        double v3_corrected = std::stod(rowData.at(7));
        double v3_error_corrected = std::stod(rowData.at(8));
        
        double v2_signal = std::stod(rowData.at(9));
        double v2_error_signal = std::stod(rowData.at(10));
        
        double v2_bg = std::stod(rowData.at(11));
        double v2_error_bg = std::stod(rowData.at(12));
        
        double v2_bg_type3 = std::stod(rowData.at(13));
        double v2_error_bg_type3 = std::stod(rowData.at(14));
        
        double v2_bg_type4 = std::stod(rowData.at(15));
        double v2_error_bg_type4 = std::stod(rowData.at(16));
        
        
        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            data.corrected_v2_40_60.push_back(v2_corrected);
            data.corrected_v2_40_60_Errors.push_back(v2_error_corrected);
            
            data.corrected_v2_40_60_type3.push_back(v2_corrected_type3);
            data.corrected_v2_40_60_Errors_type3.push_back(v2_error_corrected_type3);
            
            data.corrected_v2_40_60_type4.push_back(v2_corrected_type4);
            data.corrected_v2_40_60_Errors_type4.push_back(v2_error_corrected_type4);
            
            data.signal_v2_40_60.push_back(v2_signal);
            data.signal_v2_40_60_Errors.push_back(v2_error_signal);
            
            data.bg_v2_40_60.push_back(v2_bg);
            data.bg_v2_40_60_Errors.push_back(v2_error_bg);
            
            data.bg_v2_40_60_type3.push_back(v2_bg_type3);
            data.bg_v2_40_60_Errors_type3.push_back(v2_error_bg_type3);
            
            data.bg_v2_40_60_type4.push_back(v2_bg_type4);
            data.bg_v2_40_60_Errors_type4.push_back(v2_error_bg_type4);
            
            data.corrected_v3_40_60.push_back(v3_corrected);
            data.corrected_v3_40_60_Errors.push_back(v3_error_corrected);
            
        } else if (index >= 6 && index <= 11) {
            data.corrected_v2_20_40.push_back(v2_corrected);
            data.corrected_v2_20_40_Errors.push_back(v2_error_corrected);
            
            data.corrected_v2_20_40_type3.push_back(v2_corrected_type3);
            data.corrected_v2_20_40_Errors_type3.push_back(v2_error_corrected_type3);
            
            data.corrected_v2_20_40_type4.push_back(v2_corrected_type4);
            data.corrected_v2_20_40_Errors_type4.push_back(v2_error_corrected_type4);
            
            data.signal_v2_20_40.push_back(v2_signal);
            data.signal_v2_20_40_Errors.push_back(v2_error_signal);
            
            data.bg_v2_20_40.push_back(v2_bg);
            data.bg_v2_20_40_Errors.push_back(v2_error_bg);
            
            data.bg_v2_20_40_type3.push_back(v2_bg_type3);
            data.bg_v2_20_40_Errors_type3.push_back(v2_error_bg_type3);
            
            data.bg_v2_20_40_type4.push_back(v2_bg_type4);
            data.bg_v2_20_40_Errors_type4.push_back(v2_error_bg_type4);
            
            data.corrected_v3_20_40.push_back(v3_corrected);
            data.corrected_v3_20_40_Errors.push_back(v3_error_corrected);

        } else if (index >= 12 && index <= 17) {
            data.corrected_v2_0_20.push_back(v2_corrected);
            data.corrected_v2_0_20_Errors.push_back(v2_error_corrected);
            
            data.corrected_v2_0_20_type3.push_back(v2_corrected_type3);
            data.corrected_v2_0_20_Errors_type3.push_back(v2_error_corrected_type3);
            
            data.corrected_v2_0_20_type4.push_back(v2_corrected_type4);
            data.corrected_v2_0_20_Errors_type4.push_back(v2_error_corrected_type4);
            
            data.signal_v2_0_20.push_back(v2_signal);
            data.signal_v2_0_20_Errors.push_back(v2_error_signal);
            
            data.bg_v2_0_20.push_back(v2_bg);
            data.bg_v2_0_20_Errors.push_back(v2_error_bg);
            
            data.bg_v2_0_20_type3.push_back(v2_bg_type3);
            data.bg_v2_0_20_Errors_type3.push_back(v2_error_bg_type3);
            
            data.bg_v2_0_20_type4.push_back(v2_bg_type4);
            data.bg_v2_0_20_Errors_type4.push_back(v2_error_bg_type4);
            
            data.corrected_v3_0_20.push_back(v3_corrected);
            data.corrected_v3_0_20_Errors.push_back(v3_error_corrected);
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
// Function to create and return a TGraphErrors pointer
TGraphErrors* CreateGraph(const std::vector<double>& ptCenters, const std::vector<double>& values, const std::vector<double>& errors) {
    return new TGraphErrors(ptCenters.size(), &ptCenters[0], &values[0], nullptr, &errors[0]);
}
void plotting(const Data& data1, Data& data2, Data& data3, Data& data4, Data& data5, Data& data6) {
    std::string phenixFilePath = "/Users/patsfan753/Desktop/vN_AnalysisFinal/data/PHENIX_Data_Overlayed/FinalCleanedPhenix.csv";
    std::vector<double> ptCenters = {2.25, 2.75, 3.25, 3.75, 4.25, 4.75}; // Mid-points of pT ranges
    std::string BasePlotOutputPath = "/Users/patsfan753/Desktop";
    
    //vectors for PHENIX data
    std::vector<double> v2_0_10, v2_0_10_Errors, v2_0_10_Errors_Negative, v2_10_20, v2_10_20_Errors, v2_10_20_Errors_Negative, v2_20_30, v2_20_30_Errors, v2_20_30_Errors_Negative, v2_30_40, v2_30_40_Errors, v2_30_40_Errors_Negative, v2_40_50, v2_40_50_Errors, v2_40_50_Errors_Negative, v2_50_60, v2_50_60_Errors, v2_50_60_Errors_Negative;
    /*
     PHENIX data graph pointers
     */
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
    TGraphErrors* graph_0_10 = CreateGraph(ptCenters, v2_0_10, v2_0_10_Errors);
    graph_0_10->SetMarkerColor(color_PHENIX_low);
    graph_0_10->SetLineColor(color_PHENIX_low);
    graph_0_10->SetMarkerStyle(markerStyle_PHENIX);
    graph_0_10->SetMarkerSize(1.5);
    
    TGraphErrors* graph_10_20 = CreateGraph(ptCenters, v2_10_20, v2_10_20_Errors);
    graph_10_20->SetMarkerColor(color_PHENIX_high);
    graph_10_20->SetLineColor(color_PHENIX_high);
    graph_10_20->SetMarkerStyle(markerStyle_PHENIX);
    graph_10_20->SetMarkerSize(1.5);
    
    TGraphErrors* graph_20_30 = CreateGraph(ptCenters, v2_20_30, v2_20_30_Errors);
    graph_20_30->SetMarkerColor(color_PHENIX_low);
    graph_20_30->SetLineColor(color_PHENIX_low);
    graph_20_30->SetMarkerStyle(markerStyle_PHENIX);
    graph_20_30->SetMarkerSize(1.5);
    
    TGraphErrors* graph_30_40 = CreateGraph(ptCenters, v2_30_40, v2_30_40_Errors);
    graph_30_40->SetMarkerColor(color_PHENIX_high);
    graph_30_40->SetLineColor(color_PHENIX_high);
    graph_30_40->SetMarkerStyle(markerStyle_PHENIX);
    graph_30_40->SetMarkerSize(1.5);
    
    TGraphErrors* graph_40_50 = CreateGraph(ptCenters, v2_40_50, v2_40_50_Errors);
    graph_40_50->SetMarkerColor(color_PHENIX_low);
    graph_40_50->SetLineColor(color_PHENIX_low);
    graph_40_50->SetMarkerStyle(markerStyle_PHENIX);
    graph_40_50->SetMarkerSize(1.5);
    
    TGraphErrors* graph_50_60 = CreateGraph(ptCenters, v2_50_60, v2_50_60_Errors);
    graph_50_60->SetMarkerColor(color_PHENIX_high);
    graph_50_60->SetLineColor(color_PHENIX_high);
    graph_50_60->SetMarkerStyle(markerStyle_PHENIX);
    graph_50_60->SetMarkerSize(1.5);
    
    
    /*
     dataN Setting Data and Graphs
     */
    const auto& corrected_v2_0_20_1 = data1.corrected_v2_0_20;
    const auto& corrected_v2_20_40_1 = data1.corrected_v2_20_40;
    const auto& corrected_v2_40_60_1 = data1.corrected_v2_40_60;
    const auto& corrected_v2_0_20_Errors_1 = data1.corrected_v2_0_20_Errors;
    const auto& corrected_v2_20_40_Errors_1 = data1.corrected_v2_20_40_Errors;
    const auto& corrected_v2_40_60_Errors_1 = data1.corrected_v2_40_60_Errors;
    
    const auto& corrected_v2_0_20_2 = data2.corrected_v2_0_20;
    const auto& corrected_v2_20_40_2 = data2.corrected_v2_20_40;
    const auto& corrected_v2_40_60_2 = data2.corrected_v2_40_60;
    const auto& corrected_v2_0_20_Errors_2 = data2.corrected_v2_0_20_Errors;
    const auto& corrected_v2_20_40_Errors_2 = data2.corrected_v2_20_40_Errors;
    const auto& corrected_v2_40_60_Errors_2 = data2.corrected_v2_40_60_Errors;
    
    const auto& corrected_v2_0_20_3 = data3.corrected_v2_0_20;
    const auto& corrected_v2_20_40_3 = data3.corrected_v2_20_40;
    const auto& corrected_v2_40_60_3 = data3.corrected_v2_40_60;
    const auto& corrected_v2_0_20_Errors_3 = data3.corrected_v2_0_20_Errors;
    const auto& corrected_v2_20_40_Errors_3 = data3.corrected_v2_20_40_Errors;
    const auto& corrected_v2_40_60_Errors_3 = data3.corrected_v2_40_60_Errors;
    
    const auto& corrected_v2_0_20_4 = data4.corrected_v2_0_20;
    const auto& corrected_v2_20_40_4 = data4.corrected_v2_20_40;
    const auto& corrected_v2_40_60_4 = data4.corrected_v2_40_60;
    const auto& corrected_v2_0_20_Errors_4 = data4.corrected_v2_0_20_Errors;
    const auto& corrected_v2_20_40_Errors_4 = data4.corrected_v2_20_40_Errors;
    const auto& corrected_v2_40_60_Errors_4 = data4.corrected_v2_40_60_Errors;
    
    const auto& corrected_v2_0_20_5 = data5.corrected_v2_0_20;
    const auto& corrected_v2_20_40_5 = data5.corrected_v2_20_40;
    const auto& corrected_v2_40_60_5 = data5.corrected_v2_40_60;
    const auto& corrected_v2_0_20_Errors_5 = data5.corrected_v2_0_20_Errors;
    const auto& corrected_v2_20_40_Errors_5 = data5.corrected_v2_20_40_Errors;
    const auto& corrected_v2_40_60_Errors_5 = data5.corrected_v2_40_60_Errors;
    
    const auto& corrected_v2_0_20_6 = data6.corrected_v2_0_20;
    const auto& corrected_v2_20_40_6 = data6.corrected_v2_20_40;
    const auto& corrected_v2_40_60_6 = data6.corrected_v2_40_60;
    const auto& corrected_v2_0_20_Errors_6 = data6.corrected_v2_0_20_Errors;
    const auto& corrected_v2_20_40_Errors_6 = data6.corrected_v2_20_40_Errors;
    const auto& corrected_v2_40_60_Errors_6 = data6.corrected_v2_40_60_Errors;
    
    
    const auto& corrected_v2_0_20_1_type3 = data1.corrected_v2_0_20_type3;
    const auto& corrected_v2_20_40_1_type3 = data1.corrected_v2_20_40_type3;
    const auto& corrected_v2_40_60_1_type3 = data1.corrected_v2_40_60_type3;
    const auto& corrected_v2_0_20_Errors_1_type3 = data1.corrected_v2_0_20_Errors_type3;
    const auto& corrected_v2_20_40_Errors_1_type3 = data1.corrected_v2_20_40_Errors_type3;
    const auto& corrected_v2_40_60_Errors_1_type3 = data1.corrected_v2_40_60_Errors_type3;
    
    const auto& corrected_v2_0_20_2_type3 = data2.corrected_v2_0_20_type3;
    const auto& corrected_v2_20_40_2_type3 = data2.corrected_v2_20_40_type3;
    const auto& corrected_v2_40_60_2_type3 = data2.corrected_v2_40_60_type3;
    const auto& corrected_v2_0_20_Errors_2_type3 = data2.corrected_v2_0_20_Errors_type3;
    const auto& corrected_v2_20_40_Errors_2_type3 = data2.corrected_v2_20_40_Errors_type3;
    const auto& corrected_v2_40_60_Errors_2_type3 = data2.corrected_v2_40_60_Errors_type3;
    
    const auto& corrected_v2_0_20_3_type3 = data3.corrected_v2_0_20_type3;
    const auto& corrected_v2_20_40_3_type3 = data3.corrected_v2_20_40_type3;
    const auto& corrected_v2_40_60_3_type3 = data3.corrected_v2_40_60_type3;
    const auto& corrected_v2_0_20_Errors_3_type3 = data3.corrected_v2_0_20_Errors_type3;
    const auto& corrected_v2_20_40_Errors_3_type3 = data3.corrected_v2_20_40_Errors_type3;
    const auto& corrected_v2_40_60_Errors_3_type3 = data3.corrected_v2_40_60_Errors_type3;
    
    const auto& corrected_v2_0_20_4_type3 = data4.corrected_v2_0_20_type3;
    const auto& corrected_v2_20_40_4_type3 = data4.corrected_v2_20_40_type3;
    const auto& corrected_v2_40_60_4_type3 = data4.corrected_v2_40_60_type3;
    const auto& corrected_v2_0_20_Errors_4_type3 = data4.corrected_v2_0_20_Errors_type3;
    const auto& corrected_v2_20_40_Errors_4_type3 = data4.corrected_v2_20_40_Errors_type3;
    const auto& corrected_v2_40_60_Errors_4_type3 = data4.corrected_v2_40_60_Errors_type3;
    
    const auto& corrected_v2_0_20_5_type3 = data5.corrected_v2_0_20_type3;
    const auto& corrected_v2_20_40_5_type3 = data5.corrected_v2_20_40_type3;
    const auto& corrected_v2_40_60_5_type3 = data5.corrected_v2_40_60_type3;
    const auto& corrected_v2_0_20_Errors_5_type3 = data5.corrected_v2_0_20_Errors_type3;
    const auto& corrected_v2_20_40_Errors_5_type3 = data5.corrected_v2_20_40_Errors_type3;
    const auto& corrected_v2_40_60_Errors_5_type3 = data5.corrected_v2_40_60_Errors_type3;
    
    const auto& corrected_v2_0_20_6_type3 = data6.corrected_v2_0_20_type3;
    const auto& corrected_v2_20_40_6_type3 = data6.corrected_v2_20_40_type3;
    const auto& corrected_v2_40_60_6_type3 = data6.corrected_v2_40_60_type3;
    const auto& corrected_v2_0_20_Errors_6_type3 = data6.corrected_v2_0_20_Errors_type3;
    const auto& corrected_v2_20_40_Errors_6_type3 = data6.corrected_v2_20_40_Errors_type3;
    const auto& corrected_v2_40_60_Errors_6_type3 = data6.corrected_v2_40_60_Errors_type3;
    
    const auto& corrected_v2_0_20_1_type4 = data1.corrected_v2_0_20_type4;
    const auto& corrected_v2_20_40_1_type4 = data1.corrected_v2_20_40_type4;
    const auto& corrected_v2_40_60_1_type4 = data1.corrected_v2_40_60_type4;
    const auto& corrected_v2_0_20_Errors_1_type4 = data1.corrected_v2_0_20_Errors_type4;
    const auto& corrected_v2_20_40_Errors_1_type4 = data1.corrected_v2_20_40_Errors_type4;
    const auto& corrected_v2_40_60_Errors_1_type4 = data1.corrected_v2_40_60_Errors_type4;
    
    const auto& corrected_v2_0_20_2_type4 = data2.corrected_v2_0_20_type4;
    const auto& corrected_v2_20_40_2_type4 = data2.corrected_v2_20_40_type4;
    const auto& corrected_v2_40_60_2_type4 = data2.corrected_v2_40_60_type4;
    const auto& corrected_v2_0_20_Errors_2_type4 = data2.corrected_v2_0_20_Errors_type4;
    const auto& corrected_v2_20_40_Errors_2_type4 = data2.corrected_v2_20_40_Errors_type4;
    const auto& corrected_v2_40_60_Errors_2_type4 = data2.corrected_v2_40_60_Errors_type4;
    
    const auto& corrected_v2_0_20_3_type4 = data3.corrected_v2_0_20_type4;
    const auto& corrected_v2_20_40_3_type4 = data3.corrected_v2_20_40_type4;
    const auto& corrected_v2_40_60_3_type4 = data3.corrected_v2_40_60_type4;
    const auto& corrected_v2_0_20_Errors_3_type4 = data3.corrected_v2_0_20_Errors_type4;
    const auto& corrected_v2_20_40_Errors_3_type4 = data3.corrected_v2_20_40_Errors_type4;
    const auto& corrected_v2_40_60_Errors_3_type4 = data3.corrected_v2_40_60_Errors_type4;
    
    const auto& corrected_v2_0_20_4_type4 = data4.corrected_v2_0_20_type4;
    const auto& corrected_v2_20_40_4_type4 = data4.corrected_v2_20_40_type4;
    const auto& corrected_v2_40_60_4_type4 = data4.corrected_v2_40_60_type4;
    const auto& corrected_v2_0_20_Errors_4_type4 = data4.corrected_v2_0_20_Errors_type4;
    const auto& corrected_v2_20_40_Errors_4_type4 = data4.corrected_v2_20_40_Errors_type4;
    const auto& corrected_v2_40_60_Errors_4_type4 = data4.corrected_v2_40_60_Errors_type4;
    
    const auto& corrected_v2_0_20_5_type4 = data5.corrected_v2_0_20_type4;
    const auto& corrected_v2_20_40_5_type4 = data5.corrected_v2_20_40_type4;
    const auto& corrected_v2_40_60_5_type4 = data5.corrected_v2_40_60_type4;
    const auto& corrected_v2_0_20_Errors_5_type4 = data5.corrected_v2_0_20_Errors_type4;
    const auto& corrected_v2_20_40_Errors_5_type4 = data5.corrected_v2_20_40_Errors_type4;
    const auto& corrected_v2_40_60_Errors_5_type4 = data5.corrected_v2_40_60_Errors_type4;
    
    const auto& corrected_v2_0_20_6_type4 = data6.corrected_v2_0_20_type4;
    const auto& corrected_v2_20_40_6_type4 = data6.corrected_v2_20_40_type4;
    const auto& corrected_v2_40_60_6_type4 = data6.corrected_v2_40_60_type4;
    const auto& corrected_v2_0_20_Errors_6_type4 = data6.corrected_v2_0_20_Errors_type4;
    const auto& corrected_v2_20_40_Errors_6_type4 = data6.corrected_v2_20_40_Errors_type4;
    const auto& corrected_v2_40_60_Errors_6_type4 = data6.corrected_v2_40_60_Errors_type4;
    
    const auto& signal_v2_0_20_1 = data1.signal_v2_0_20;
    const auto& signal_v2_20_40_1 = data1.signal_v2_20_40;
    const auto& signal_v2_40_60_1 = data1.signal_v2_40_60;
    const auto& signal_v2_0_20_Errors_1 = data1.signal_v2_0_20_Errors;
    const auto& signal_v2_20_40_Errors_1 = data1.signal_v2_20_40_Errors;
    const auto& signal_v2_40_60_Errors_1 = data1.signal_v2_40_60_Errors;
    const auto& signal_v2_0_20_2 = data2.signal_v2_0_20;
    const auto& signal_v2_20_40_2 = data2.signal_v2_20_40;
    const auto& signal_v2_40_60_2 = data2.signal_v2_40_60;
    const auto& signal_v2_0_20_Errors_2 = data2.signal_v2_0_20_Errors;
    const auto& signal_v2_20_40_Errors_2 = data2.signal_v2_20_40_Errors;
    const auto& signal_v2_40_60_Errors_2 = data2.signal_v2_40_60_Errors;
    const auto& signal_v2_0_20_3 = data3.signal_v2_0_20;
    const auto& signal_v2_20_40_3 = data3.signal_v2_20_40;
    const auto& signal_v2_40_60_3 = data3.signal_v2_40_60;
    const auto& signal_v2_0_20_Errors_3 = data3.signal_v2_0_20_Errors;
    const auto& signal_v2_20_40_Errors_3 = data3.signal_v2_20_40_Errors;
    const auto& signal_v2_40_60_Errors_3 = data3.signal_v2_40_60_Errors;
    const auto& signal_v2_0_20_4 = data4.signal_v2_0_20;
    const auto& signal_v2_20_40_4 = data4.signal_v2_20_40;
    const auto& signal_v2_40_60_4 = data4.signal_v2_40_60;
    const auto& signal_v2_0_20_Errors_4 = data4.signal_v2_0_20_Errors;
    const auto& signal_v2_20_40_Errors_4 = data4.signal_v2_20_40_Errors;
    const auto& signal_v2_40_60_Errors_4 = data4.signal_v2_40_60_Errors;
    const auto& signal_v2_0_20_5 = data5.signal_v2_0_20;
    const auto& signal_v2_20_40_5 = data5.signal_v2_20_40;
    const auto& signal_v2_40_60_5 = data5.signal_v2_40_60;
    const auto& signal_v2_0_20_Errors_5 = data5.signal_v2_0_20_Errors;
    const auto& signal_v2_20_40_Errors_5 = data5.signal_v2_20_40_Errors;
    const auto& signal_v2_40_60_Errors_5 = data5.signal_v2_40_60_Errors;
    const auto& signal_v2_0_20_6 = data6.signal_v2_0_20;
    const auto& signal_v2_20_40_6 = data6.signal_v2_20_40;
    const auto& signal_v2_40_60_6 = data6.signal_v2_40_60;
    const auto& signal_v2_0_20_Errors_6 = data6.signal_v2_0_20_Errors;
    const auto& signal_v2_20_40_Errors_6 = data6.signal_v2_20_40_Errors;
    const auto& signal_v2_40_60_Errors_6 = data6.signal_v2_40_60_Errors;

    const auto& bg_v2_0_20_1 = data1.bg_v2_0_20;
    const auto& bg_v2_20_40_1 = data1.bg_v2_20_40;
    const auto& bg_v2_40_60_1 = data1.bg_v2_40_60;
    const auto& bg_v2_0_20_Errors_1 = data1.bg_v2_0_20_Errors;
    const auto& bg_v2_20_40_Errors_1 = data1.bg_v2_20_40_Errors;
    const auto& bg_v2_40_60_Errors_1 = data1.bg_v2_40_60_Errors;
    const auto& bg_v2_0_20_2 = data2.bg_v2_0_20;
    const auto& bg_v2_20_40_2 = data2.bg_v2_20_40;
    const auto& bg_v2_40_60_2 = data2.bg_v2_40_60;
    const auto& bg_v2_0_20_Errors_2 = data2.bg_v2_0_20_Errors;
    const auto& bg_v2_20_40_Errors_2 = data2.bg_v2_20_40_Errors;
    const auto& bg_v2_40_60_Errors_2 = data2.bg_v2_40_60_Errors;
    const auto& bg_v2_0_20_3 = data3.bg_v2_0_20;
    const auto& bg_v2_20_40_3 = data3.bg_v2_20_40;
    const auto& bg_v2_40_60_3 = data3.bg_v2_40_60;
    const auto& bg_v2_0_20_Errors_3 = data3.bg_v2_0_20_Errors;
    const auto& bg_v2_20_40_Errors_3 = data3.bg_v2_20_40_Errors;
    const auto& bg_v2_40_60_Errors_3 = data3.bg_v2_40_60_Errors;
    const auto& bg_v2_0_20_4 = data4.bg_v2_0_20;
    const auto& bg_v2_20_40_4 = data4.bg_v2_20_40;
    const auto& bg_v2_40_60_4 = data4.bg_v2_40_60;
    const auto& bg_v2_0_20_Errors_4 = data4.bg_v2_0_20_Errors;
    const auto& bg_v2_20_40_Errors_4 = data4.bg_v2_20_40_Errors;
    const auto& bg_v2_40_60_Errors_4 = data4.bg_v2_40_60_Errors;
    const auto& bg_v2_0_20_5 = data5.bg_v2_0_20;
    const auto& bg_v2_20_40_5 = data5.bg_v2_20_40;
    const auto& bg_v2_40_60_5 = data5.bg_v2_40_60;
    const auto& bg_v2_0_20_Errors_5 = data5.bg_v2_0_20_Errors;
    const auto& bg_v2_20_40_Errors_5 = data5.bg_v2_20_40_Errors;
    const auto& bg_v2_40_60_Errors_5 = data5.bg_v2_40_60_Errors;
    const auto& bg_v2_0_20_6 = data6.bg_v2_0_20;
    const auto& bg_v2_20_40_6 = data6.bg_v2_20_40;
    const auto& bg_v2_40_60_6 = data6.bg_v2_40_60;
    const auto& bg_v2_0_20_Errors_6 = data6.bg_v2_0_20_Errors;
    const auto& bg_v2_20_40_Errors_6 = data6.bg_v2_20_40_Errors;
    const auto& bg_v2_40_60_Errors_6 = data6.bg_v2_40_60_Errors;

    const auto& bg_v2_0_20_1_type3 = data1.bg_v2_0_20_type3;
    const auto& bg_v2_20_40_1_type3 = data1.bg_v2_20_40_type3;
    const auto& bg_v2_40_60_1_type3 = data1.bg_v2_40_60_type3;
    const auto& bg_v2_0_20_Errors_1_type3 = data1.bg_v2_0_20_Errors_type3;
    const auto& bg_v2_20_40_Errors_1_type3 = data1.bg_v2_20_40_Errors_type3;
    const auto& bg_v2_40_60_Errors_1_type3 = data1.bg_v2_40_60_Errors_type3;
    const auto& bg_v2_0_20_2_type3 = data2.bg_v2_0_20_type3;
    const auto& bg_v2_20_40_2_type3 = data2.bg_v2_20_40_type3;
    const auto& bg_v2_40_60_2_type3 = data2.bg_v2_40_60_type3;
    const auto& bg_v2_0_20_Errors_2_type3 = data2.bg_v2_0_20_Errors_type3;
    const auto& bg_v2_20_40_Errors_2_type3 = data2.bg_v2_20_40_Errors_type3;
    const auto& bg_v2_40_60_Errors_2_type3 = data2.bg_v2_40_60_Errors_type3;
    const auto& bg_v2_0_20_3_type3 = data3.bg_v2_0_20_type3;
    const auto& bg_v2_20_40_3_type3 = data3.bg_v2_20_40_type3;
    const auto& bg_v2_40_60_3_type3 = data3.bg_v2_40_60_type3;
    const auto& bg_v2_0_20_Errors_3_type3 = data3.bg_v2_0_20_Errors_type3;
    const auto& bg_v2_20_40_Errors_3_type3 = data3.bg_v2_20_40_Errors_type3;
    const auto& bg_v2_40_60_Errors_3_type3 = data3.bg_v2_40_60_Errors_type3;
    const auto& bg_v2_0_20_4_type3 = data4.bg_v2_0_20_type3;
    const auto& bg_v2_20_40_4_type3 = data4.bg_v2_20_40_type3;
    const auto& bg_v2_40_60_4_type3 = data4.bg_v2_40_60_type3;
    const auto& bg_v2_0_20_Errors_4_type3 = data4.bg_v2_0_20_Errors_type3;
    const auto& bg_v2_20_40_Errors_4_type3 = data4.bg_v2_20_40_Errors_type3;
    const auto& bg_v2_40_60_Errors_4_type3 = data4.bg_v2_40_60_Errors_type3;
    const auto& bg_v2_0_20_5_type3 = data5.bg_v2_0_20_type3;
    const auto& bg_v2_20_40_5_type3 = data5.bg_v2_20_40_type3;
    const auto& bg_v2_40_60_5_type3 = data5.bg_v2_40_60_type3;
    const auto& bg_v2_0_20_Errors_5_type3 = data5.bg_v2_0_20_Errors_type3;
    const auto& bg_v2_20_40_Errors_5_type3 = data5.bg_v2_20_40_Errors_type3;
    const auto& bg_v2_40_60_Errors_5_type3 = data5.bg_v2_40_60_Errors_type3;
    const auto& bg_v2_0_20_6_type3 = data6.bg_v2_0_20_type3;
    const auto& bg_v2_20_40_6_type3 = data6.bg_v2_20_40_type3;
    const auto& bg_v2_40_60_6_type3 = data6.bg_v2_40_60_type3;
    const auto& bg_v2_0_20_Errors_6_type3 = data6.bg_v2_0_20_Errors_type3;
    const auto& bg_v2_20_40_Errors_6_type3 = data6.bg_v2_20_40_Errors_type3;
    const auto& bg_v2_40_60_Errors_6_type3 = data6.bg_v2_40_60_Errors_type3;
    
    const auto& bg_v2_0_20_1_type4 = data1.bg_v2_0_20_type4;
    const auto& bg_v2_20_40_1_type4 = data1.bg_v2_20_40_type4;
    const auto& bg_v2_40_60_1_type4 = data1.bg_v2_40_60_type4;
    const auto& bg_v2_0_20_Errors_1_type4 = data1.bg_v2_0_20_Errors_type4;
    const auto& bg_v2_20_40_Errors_1_type4 = data1.bg_v2_20_40_Errors_type4;
    const auto& bg_v2_40_60_Errors_1_type4 = data1.bg_v2_40_60_Errors_type4;
    const auto& bg_v2_0_20_2_type4 = data2.bg_v2_0_20_type4;
    const auto& bg_v2_20_40_2_type4 = data2.bg_v2_20_40_type4;
    const auto& bg_v2_40_60_2_type4 = data2.bg_v2_40_60_type4;
    const auto& bg_v2_0_20_Errors_2_type4 = data2.bg_v2_0_20_Errors_type4;
    const auto& bg_v2_20_40_Errors_2_type4 = data2.bg_v2_20_40_Errors_type4;
    const auto& bg_v2_40_60_Errors_2_type4 = data2.bg_v2_40_60_Errors_type4;
    const auto& bg_v2_0_20_3_type4 = data3.bg_v2_0_20_type4;
    const auto& bg_v2_20_40_3_type4 = data3.bg_v2_20_40_type4;
    const auto& bg_v2_40_60_3_type4 = data3.bg_v2_40_60_type4;
    const auto& bg_v2_0_20_Errors_3_type4 = data3.bg_v2_0_20_Errors_type4;
    const auto& bg_v2_20_40_Errors_3_type4 = data3.bg_v2_20_40_Errors_type4;
    const auto& bg_v2_40_60_Errors_3_type4 = data3.bg_v2_40_60_Errors_type4;
    const auto& bg_v2_0_20_4_type4 = data4.bg_v2_0_20_type4;
    const auto& bg_v2_20_40_4_type4 = data4.bg_v2_20_40_type4;
    const auto& bg_v2_40_60_4_type4 = data4.bg_v2_40_60_type4;
    const auto& bg_v2_0_20_Errors_4_type4 = data4.bg_v2_0_20_Errors_type4;
    const auto& bg_v2_20_40_Errors_4_type4 = data4.bg_v2_20_40_Errors_type4;
    const auto& bg_v2_40_60_Errors_4_type4 = data4.bg_v2_40_60_Errors_type4;
    const auto& bg_v2_0_20_5_type4 = data5.bg_v2_0_20_type4;
    const auto& bg_v2_20_40_5_type4 = data5.bg_v2_20_40_type4;
    const auto& bg_v2_40_60_5_type4 = data5.bg_v2_40_60_type4;
    const auto& bg_v2_0_20_Errors_5_type4 = data5.bg_v2_0_20_Errors_type4;
    const auto& bg_v2_20_40_Errors_5_type4 = data5.bg_v2_20_40_Errors_type4;
    const auto& bg_v2_40_60_Errors_5_type4 = data5.bg_v2_40_60_Errors_type4;
    const auto& bg_v2_0_20_6_type4 = data6.bg_v2_0_20_type4;
    const auto& bg_v2_20_40_6_type4 = data6.bg_v2_20_40_type4;
    const auto& bg_v2_40_60_6_type4 = data6.bg_v2_40_60_type4;
    const auto& bg_v2_0_20_Errors_6_type4 = data6.bg_v2_0_20_Errors_type4;
    const auto& bg_v2_20_40_Errors_6_type4 = data6.bg_v2_20_40_Errors_type4;
    const auto& bg_v2_40_60_Errors_6_type4 = data6.bg_v2_40_60_Errors_type4;

    PrintVectorContents(data1.corrected_v2_0_20, data1.corrected_v2_0_20_Errors, "Corrected v2 0-20% Data1");
    PrintVectorContents(data2.corrected_v2_0_20, data2.corrected_v2_0_20_Errors, "Corrected v2 0-20% data2");
    PrintVectorContents(data3.corrected_v2_0_20, data3.corrected_v2_0_20_Errors, "Corrected v2 0-20% data3");
    PrintVectorContents(data4.corrected_v2_0_20, data4.corrected_v2_0_20_Errors, "Corrected v2 0-20% data4");
    PrintVectorContents(data5.corrected_v2_0_20, data5.corrected_v2_0_20_Errors, "Corrected v2 0-20% data5");
    PrintVectorContents(data6.corrected_v2_0_20, data6.corrected_v2_0_20_Errors, "Corrected v2 0-20% data6");
    PrintVectorContents(data1.corrected_v2_0_20_type3, data1.corrected_v2_0_20_Errors_type3, "Corrected v2 0-20% Data1 type3");
    PrintVectorContents(data2.corrected_v2_0_20_type3, data2.corrected_v2_0_20_Errors_type3, "Corrected v2 0-20% data2 type3");
    PrintVectorContents(data3.corrected_v2_0_20_type3, data3.corrected_v2_0_20_Errors_type3, "Corrected v2 0-20% data3 type3");
    PrintVectorContents(data4.corrected_v2_0_20_type3, data4.corrected_v2_0_20_Errors_type3, "Corrected v2 0-20% data4 type3");
    PrintVectorContents(data5.corrected_v2_0_20_type3, data5.corrected_v2_0_20_Errors_type3, "Corrected v2 0-20% data5 type3");
    PrintVectorContents(data6.corrected_v2_0_20_type3, data6.corrected_v2_0_20_Errors_type3, "Corrected v2 0-20% data6 type3");
    PrintVectorContents(data1.corrected_v2_0_20_type4, data1.corrected_v2_0_20_Errors_type4, "Corrected v2 0-20% Data1 type4");
    PrintVectorContents(data2.corrected_v2_0_20_type4, data2.corrected_v2_0_20_Errors_type4, "Corrected v2 0-20% data2 type4");
    PrintVectorContents(data3.corrected_v2_0_20_type4, data3.corrected_v2_0_20_Errors_type4, "Corrected v2 0-20% data3 type4");
    PrintVectorContents(data4.corrected_v2_0_20_type4, data4.corrected_v2_0_20_Errors_type4, "Corrected v2 0-20% data4 type4");
    PrintVectorContents(data5.corrected_v2_0_20_type4, data5.corrected_v2_0_20_Errors_type4, "Corrected v2 0-20% data5 type4");
    PrintVectorContents(data6.corrected_v2_0_20_type4, data6.corrected_v2_0_20_Errors_type4, "Corrected v2 0-20% data6 type4");
    PrintVectorContents(data1.signal_v2_0_20, data1.signal_v2_0_20_Errors, "signal v2 0-20% Data1");
    PrintVectorContents(data2.signal_v2_0_20, data2.signal_v2_0_20_Errors, "signal v2 0-20% data2");
    PrintVectorContents(data3.signal_v2_0_20, data3.signal_v2_0_20_Errors, "signal v2 0-20% data3");
    PrintVectorContents(data4.signal_v2_0_20, data4.signal_v2_0_20_Errors, "signal v2 0-20% data4");
    PrintVectorContents(data5.signal_v2_0_20, data5.signal_v2_0_20_Errors, "signal v2 0-20% data5");
    PrintVectorContents(data6.signal_v2_0_20, data6.signal_v2_0_20_Errors, "signal v2 0-20% data6");
    PrintVectorContents(data1.bg_v2_0_20, data1.bg_v2_0_20_Errors, "bg v2 0-20% Data1");
    PrintVectorContents(data2.bg_v2_0_20, data2.bg_v2_0_20_Errors, "bg v2 0-20% data2");
    PrintVectorContents(data3.bg_v2_0_20, data3.bg_v2_0_20_Errors, "bg v2 0-20% data3");
    PrintVectorContents(data4.bg_v2_0_20, data4.bg_v2_0_20_Errors, "bg v2 0-20% data4");
    PrintVectorContents(data5.bg_v2_0_20, data5.bg_v2_0_20_Errors, "bg v2 0-20% data5");
    PrintVectorContents(data6.bg_v2_0_20, data6.bg_v2_0_20_Errors, "bg v2 0-20% data6");
    PrintVectorContents(data1.bg_v2_0_20_type3, data1.bg_v2_0_20_Errors_type3, "bg v2 0-20% Data1 type3");
    PrintVectorContents(data2.bg_v2_0_20_type3, data2.bg_v2_0_20_Errors_type3, "bg v2 0-20% data2 type3");
    PrintVectorContents(data3.bg_v2_0_20_type3, data3.bg_v2_0_20_Errors_type3, "bg v2 0-20% data3 type3");
    PrintVectorContents(data4.bg_v2_0_20_type3, data4.bg_v2_0_20_Errors_type3, "bg v2 0-20% data4 type3");
    PrintVectorContents(data5.bg_v2_0_20_type3, data5.bg_v2_0_20_Errors_type3, "bg v2 0-20% data5 type3");
    PrintVectorContents(data6.bg_v2_0_20_type3, data6.bg_v2_0_20_Errors_type3, "bg v2 0-20% data6 type3");
    PrintVectorContents(data1.bg_v2_0_20_type4, data1.bg_v2_0_20_Errors_type4, "bg v2 0-20% Data1 type4");
    PrintVectorContents(data2.bg_v2_0_20_type4, data2.bg_v2_0_20_Errors_type4, "bg v2 0-20% data2 type4");
    PrintVectorContents(data3.bg_v2_0_20_type4, data3.bg_v2_0_20_Errors_type4, "bg v2 0-20% data3 type4");
    PrintVectorContents(data4.bg_v2_0_20_type4, data4.bg_v2_0_20_Errors_type4, "bg v2 0-20% data4 type4");
    PrintVectorContents(data5.bg_v2_0_20_type4, data5.bg_v2_0_20_Errors_type4, "bg v2 0-20% data5 type4");
    PrintVectorContents(data6.bg_v2_0_20_type4, data6.bg_v2_0_20_Errors_type4, "bg v2 0-20% data6 type4");
    
    

    PrintVectorContents(data1.corrected_v2_20_40, data1.corrected_v2_20_40_Errors, "Corrected v2 20-40% Data1");
    PrintVectorContents(data2.corrected_v2_20_40, data2.corrected_v2_20_40_Errors, "Corrected v2 20-40% data2");
    PrintVectorContents(data3.corrected_v2_20_40, data3.corrected_v2_20_40_Errors, "Corrected v2 20-40% data3");
    PrintVectorContents(data4.corrected_v2_20_40, data4.corrected_v2_20_40_Errors, "Corrected v2 20-40% data4");
    PrintVectorContents(data5.corrected_v2_20_40, data5.corrected_v2_20_40_Errors, "Corrected v2 20-40% data5");
    PrintVectorContents(data6.corrected_v2_20_40, data6.corrected_v2_20_40_Errors, "Corrected v2 20-40% data6");
    PrintVectorContents(data1.corrected_v2_20_40_type3, data1.corrected_v2_20_40_Errors_type3, "Corrected v2 20-40% Data1 type3");
    PrintVectorContents(data2.corrected_v2_20_40_type3, data2.corrected_v2_20_40_Errors_type3, "Corrected v2 20-40% data2 type3");
    PrintVectorContents(data3.corrected_v2_20_40_type3, data3.corrected_v2_20_40_Errors_type3, "Corrected v2 20-40% data3 type3");
    PrintVectorContents(data4.corrected_v2_20_40_type3, data4.corrected_v2_20_40_Errors_type3, "Corrected v2 20-40% data4 type3");
    PrintVectorContents(data5.corrected_v2_20_40_type3, data5.corrected_v2_20_40_Errors_type3, "Corrected v2 20-40% data5 type3");
    PrintVectorContents(data6.corrected_v2_20_40_type3, data6.corrected_v2_20_40_Errors_type3, "Corrected v2 20-40% data6 type3");
    PrintVectorContents(data1.corrected_v2_20_40_type4, data1.corrected_v2_20_40_Errors_type4, "Corrected v2 20-40% Data1 type4");
    PrintVectorContents(data2.corrected_v2_20_40_type4, data2.corrected_v2_20_40_Errors_type4, "Corrected v2 20-40% data2 type4");
    PrintVectorContents(data3.corrected_v2_20_40_type4, data3.corrected_v2_20_40_Errors_type4, "Corrected v2 20-40% data3 type4");
    PrintVectorContents(data4.corrected_v2_20_40_type4, data4.corrected_v2_20_40_Errors_type4, "Corrected v2 20-40% data4 type4");
    PrintVectorContents(data5.corrected_v2_20_40_type4, data5.corrected_v2_20_40_Errors_type4, "Corrected v2 20-40% data5 type4");
    PrintVectorContents(data6.corrected_v2_20_40_type4, data6.corrected_v2_20_40_Errors_type4, "Corrected v2 20-40% data6 type4");
    PrintVectorContents(data1.signal_v2_20_40, data1.signal_v2_20_40_Errors, "signal v2 20-40% Data1");
    PrintVectorContents(data2.signal_v2_20_40, data2.signal_v2_20_40_Errors, "signal v2 20-40% data2");
    PrintVectorContents(data3.signal_v2_20_40, data3.signal_v2_20_40_Errors, "signal v2 20-40% data3");
    PrintVectorContents(data4.signal_v2_20_40, data4.signal_v2_20_40_Errors, "signal v2 20-40% data4");
    PrintVectorContents(data5.signal_v2_20_40, data5.signal_v2_20_40_Errors, "signal v2 20-40% data5");
    PrintVectorContents(data6.signal_v2_20_40, data6.signal_v2_20_40_Errors, "signal v2 20-40% data6");
    PrintVectorContents(data1.bg_v2_20_40, data1.bg_v2_20_40_Errors, "bg v2 20-40% Data1");
    PrintVectorContents(data2.bg_v2_20_40, data2.bg_v2_20_40_Errors, "bg v2 20-40% data2");
    PrintVectorContents(data3.bg_v2_20_40, data3.bg_v2_20_40_Errors, "bg v2 20-40% data3");
    PrintVectorContents(data4.bg_v2_20_40, data4.bg_v2_20_40_Errors, "bg v2 20-40% data4");
    PrintVectorContents(data5.bg_v2_20_40, data5.bg_v2_20_40_Errors, "bg v2 20-40% data5");
    PrintVectorContents(data6.bg_v2_20_40, data6.bg_v2_20_40_Errors, "bg v2 20-40% data6");
    PrintVectorContents(data1.bg_v2_20_40_type3, data1.bg_v2_20_40_Errors_type3, "bg v2 20-40% Data1 type3");
    PrintVectorContents(data2.bg_v2_20_40_type3, data2.bg_v2_20_40_Errors_type3, "bg v2 20-40% data2 type3");
    PrintVectorContents(data3.bg_v2_20_40_type3, data3.bg_v2_20_40_Errors_type3, "bg v2 20-40% data3 type3");
    PrintVectorContents(data4.bg_v2_20_40_type3, data4.bg_v2_20_40_Errors_type3, "bg v2 20-40% data4 type3");
    PrintVectorContents(data5.bg_v2_20_40_type3, data5.bg_v2_20_40_Errors_type3, "bg v2 20-40% data5 type3");
    PrintVectorContents(data6.bg_v2_20_40_type3, data6.bg_v2_20_40_Errors_type3, "bg v2 20-40% data6 type3");
    PrintVectorContents(data1.bg_v2_20_40_type4, data1.bg_v2_20_40_Errors_type4, "bg v2 20-40% Data1 type4");
    PrintVectorContents(data2.bg_v2_20_40_type4, data2.bg_v2_20_40_Errors_type4, "bg v2 20-40% data2 type4");
    PrintVectorContents(data3.bg_v2_20_40_type4, data3.bg_v2_20_40_Errors_type4, "bg v2 20-40% data3 type4");
    PrintVectorContents(data4.bg_v2_20_40_type4, data4.bg_v2_20_40_Errors_type4, "bg v2 20-40% data4 type4");
    PrintVectorContents(data5.bg_v2_20_40_type4, data5.bg_v2_20_40_Errors_type4, "bg v2 20-40% data5 type4");
    PrintVectorContents(data6.bg_v2_20_40_type4, data6.bg_v2_20_40_Errors_type4, "bg v2 20-40% data6 type4");
    
    PrintVectorContents(data1.corrected_v2_40_60, data1.corrected_v2_40_60_Errors, "Corrected v2 40-60% Data1");
    PrintVectorContents(data2.corrected_v2_40_60, data2.corrected_v2_40_60_Errors, "Corrected v2 40-60% data2");
    PrintVectorContents(data3.corrected_v2_40_60, data3.corrected_v2_40_60_Errors, "Corrected v2 40-60% data3");
    PrintVectorContents(data4.corrected_v2_40_60, data4.corrected_v2_40_60_Errors, "Corrected v2 40-60% data4");
    PrintVectorContents(data5.corrected_v2_40_60, data5.corrected_v2_40_60_Errors, "Corrected v2 40-60% data5");
    PrintVectorContents(data6.corrected_v2_40_60, data6.corrected_v2_40_60_Errors, "Corrected v2 40-60% data6");
    PrintVectorContents(data1.corrected_v2_40_60_type3, data1.corrected_v2_40_60_Errors_type3, "Corrected v2 40-60% Data1 type3");
    PrintVectorContents(data2.corrected_v2_40_60_type3, data2.corrected_v2_40_60_Errors_type3, "Corrected v2 40-60% data2 type3");
    PrintVectorContents(data3.corrected_v2_40_60_type3, data3.corrected_v2_40_60_Errors_type3, "Corrected v2 40-60% data3 type3");
    PrintVectorContents(data4.corrected_v2_40_60_type3, data4.corrected_v2_40_60_Errors_type3, "Corrected v2 40-60% data4 type3");
    PrintVectorContents(data5.corrected_v2_40_60_type3, data5.corrected_v2_40_60_Errors_type3, "Corrected v2 40-60% data5 type3");
    PrintVectorContents(data6.corrected_v2_40_60_type3, data6.corrected_v2_40_60_Errors_type3, "Corrected v2 40-60% data6 type3");
    PrintVectorContents(data1.corrected_v2_40_60_type4, data1.corrected_v2_40_60_Errors_type4, "Corrected v2 40-60% Data1 type4");
    PrintVectorContents(data2.corrected_v2_40_60_type4, data2.corrected_v2_40_60_Errors_type4, "Corrected v2 40-60% data2 type4");
    PrintVectorContents(data3.corrected_v2_40_60_type4, data3.corrected_v2_40_60_Errors_type4, "Corrected v2 40-60% data3 type4");
    PrintVectorContents(data4.corrected_v2_40_60_type4, data4.corrected_v2_40_60_Errors_type4, "Corrected v2 40-60% data4 type4");
    PrintVectorContents(data5.corrected_v2_40_60_type4, data5.corrected_v2_40_60_Errors_type4, "Corrected v2 40-60% data5 type4");
    PrintVectorContents(data6.corrected_v2_40_60_type4, data6.corrected_v2_40_60_Errors_type4, "Corrected v2 40-60% data6 type4");
    PrintVectorContents(data1.signal_v2_40_60, data1.signal_v2_40_60_Errors, "signal v2 40-60% Data1");
    PrintVectorContents(data2.signal_v2_40_60, data2.signal_v2_40_60_Errors, "signal v2 40-60% data2");
    PrintVectorContents(data3.signal_v2_40_60, data3.signal_v2_40_60_Errors, "signal v2 40-60% data3");
    PrintVectorContents(data4.signal_v2_40_60, data4.signal_v2_40_60_Errors, "signal v2 40-60% data4");
    PrintVectorContents(data5.signal_v2_40_60, data5.signal_v2_40_60_Errors, "signal v2 40-60% data5");
    PrintVectorContents(data6.signal_v2_40_60, data6.signal_v2_40_60_Errors, "signal v2 40-60% data6");
    PrintVectorContents(data1.bg_v2_40_60, data1.bg_v2_40_60_Errors, "bg v2 40-60% Data1");
    PrintVectorContents(data2.bg_v2_40_60, data2.bg_v2_40_60_Errors, "bg v2 40-60% data2");
    PrintVectorContents(data3.bg_v2_40_60, data3.bg_v2_40_60_Errors, "bg v2 40-60% data3");
    PrintVectorContents(data4.bg_v2_40_60, data4.bg_v2_40_60_Errors, "bg v2 40-60% data4");
    PrintVectorContents(data5.bg_v2_40_60, data5.bg_v2_40_60_Errors, "bg v2 40-60% data5");
    PrintVectorContents(data6.bg_v2_40_60, data6.bg_v2_40_60_Errors, "bg v2 40-60% data6");
    PrintVectorContents(data1.bg_v2_40_60_type3, data1.bg_v2_40_60_Errors_type3, "bg v2 40-60% Data1 type3");
    PrintVectorContents(data2.bg_v2_40_60_type3, data2.bg_v2_40_60_Errors_type3, "bg v2 40-60% data2 type3");
    PrintVectorContents(data3.bg_v2_40_60_type3, data3.bg_v2_40_60_Errors_type3, "bg v2 40-60% data3 type3");
    PrintVectorContents(data4.bg_v2_40_60_type3, data4.bg_v2_40_60_Errors_type3, "bg v2 40-60% data4 type3");
    PrintVectorContents(data5.bg_v2_40_60_type3, data5.bg_v2_40_60_Errors_type3, "bg v2 40-60% data5 type3");
    PrintVectorContents(data6.bg_v2_40_60_type3, data6.bg_v2_40_60_Errors_type3, "bg v2 40-60% data6 type3");
    PrintVectorContents(data1.bg_v2_40_60_type4, data1.bg_v2_40_60_Errors_type4, "bg v2 40-60% Data1 type4");
    PrintVectorContents(data2.bg_v2_40_60_type4, data2.bg_v2_40_60_Errors_type4, "bg v2 40-60% data2 type4");
    PrintVectorContents(data3.bg_v2_40_60_type4, data3.bg_v2_40_60_Errors_type4, "bg v2 40-60% data3 type4");
    PrintVectorContents(data4.bg_v2_40_60_type4, data4.bg_v2_40_60_Errors_type4, "bg v2 40-60% data4 type4");
    PrintVectorContents(data5.bg_v2_40_60_type4, data5.bg_v2_40_60_Errors_type4, "bg v2 40-60% data5 type4");
    PrintVectorContents(data6.bg_v2_40_60_type4, data6.bg_v2_40_60_Errors_type4, "bg v2 40-60% data6 type4");



                    
    
    TGraphErrors* corrected_v2_0_20_graph_1 = CreateGraph(ptCenters, corrected_v2_0_20_1, corrected_v2_0_20_Errors_1);
    TGraphErrors* corrected_v2_20_40_graph_1 = CreateGraph(ptCenters, corrected_v2_20_40_1, corrected_v2_20_40_Errors_1);
    TGraphErrors* corrected_v2_40_60_graph_1 = CreateGraph(ptCenters, corrected_v2_40_60_1, corrected_v2_40_60_Errors_1);
    
    TGraphErrors* corrected_v2_0_20_graph_2 = CreateGraph(ptCenters, corrected_v2_0_20_2, corrected_v2_0_20_Errors_2);
    TGraphErrors* corrected_v2_20_40_graph_2 = CreateGraph(ptCenters, corrected_v2_20_40_2, corrected_v2_20_40_Errors_2);
    TGraphErrors* corrected_v2_40_60_graph_2 = CreateGraph(ptCenters, corrected_v2_40_60_2, corrected_v2_40_60_Errors_2);

    TGraphErrors* corrected_v2_0_20_graph_3 = CreateGraph(ptCenters, corrected_v2_0_20_3, corrected_v2_0_20_Errors_3);
    TGraphErrors* corrected_v2_20_40_graph_3 = CreateGraph(ptCenters, corrected_v2_20_40_3, corrected_v2_20_40_Errors_3);
    TGraphErrors* corrected_v2_40_60_graph_3 = CreateGraph(ptCenters, corrected_v2_40_60_3, corrected_v2_40_60_Errors_3);
    
    TGraphErrors* corrected_v2_0_20_graph_4 = CreateGraph(ptCenters, corrected_v2_0_20_4, corrected_v2_0_20_Errors_4);
    TGraphErrors* corrected_v2_20_40_graph_4 = CreateGraph(ptCenters, corrected_v2_20_40_4, corrected_v2_20_40_Errors_4);
    TGraphErrors* corrected_v2_40_60_graph_4 = CreateGraph(ptCenters, corrected_v2_40_60_4, corrected_v2_40_60_Errors_4);

    TGraphErrors* corrected_v2_0_20_graph_5 = CreateGraph(ptCenters, corrected_v2_0_20_5, corrected_v2_0_20_Errors_5);
    TGraphErrors* corrected_v2_20_40_graph_5 = CreateGraph(ptCenters, corrected_v2_20_40_5, corrected_v2_20_40_Errors_5);
    TGraphErrors* corrected_v2_40_60_graph_5 = CreateGraph(ptCenters, corrected_v2_40_60_5, corrected_v2_40_60_Errors_5);

    TGraphErrors* corrected_v2_0_20_graph_6 = CreateGraph(ptCenters, corrected_v2_0_20_6, corrected_v2_0_20_Errors_6);
    TGraphErrors* corrected_v2_20_40_graph_6 = CreateGraph(ptCenters, corrected_v2_20_40_6, corrected_v2_20_40_Errors_6);
    TGraphErrors* corrected_v2_40_60_graph_6 = CreateGraph(ptCenters, corrected_v2_40_60_6, corrected_v2_40_60_Errors_6);
    

    TGraphErrors* corrected_v2_0_20_graph_1_type3 = CreateGraph(ptCenters, corrected_v2_0_20_1_type3, corrected_v2_0_20_Errors_1_type3);
    TGraphErrors* corrected_v2_20_40_graph_1_type3 = CreateGraph(ptCenters, corrected_v2_20_40_1_type3, corrected_v2_20_40_Errors_1_type3);
    TGraphErrors* corrected_v2_40_60_graph_1_type3 = CreateGraph(ptCenters, corrected_v2_40_60_1_type3, corrected_v2_40_60_Errors_1_type3);
    
    TGraphErrors* corrected_v2_0_20_graph_2_type3 = CreateGraph(ptCenters, corrected_v2_0_20_2_type3, corrected_v2_0_20_Errors_2_type3);
    TGraphErrors* corrected_v2_20_40_graph_2_type3 = CreateGraph(ptCenters, corrected_v2_20_40_2_type3, corrected_v2_20_40_Errors_2_type3);
    TGraphErrors* corrected_v2_40_60_graph_2_type3 = CreateGraph(ptCenters, corrected_v2_40_60_2_type3, corrected_v2_40_60_Errors_2_type3);

    TGraphErrors* corrected_v2_0_20_graph_3_type3 = CreateGraph(ptCenters, corrected_v2_0_20_3_type3, corrected_v2_0_20_Errors_3_type3);
    TGraphErrors* corrected_v2_20_40_graph_3_type3 = CreateGraph(ptCenters, corrected_v2_20_40_3_type3, corrected_v2_20_40_Errors_3_type3);
    TGraphErrors* corrected_v2_40_60_graph_3_type3 = CreateGraph(ptCenters, corrected_v2_40_60_3_type3, corrected_v2_40_60_Errors_3_type3);
    
    TGraphErrors* corrected_v2_0_20_graph_4_type3 = CreateGraph(ptCenters, corrected_v2_0_20_4_type3, corrected_v2_0_20_Errors_4_type3);
    TGraphErrors* corrected_v2_20_40_graph_4_type3 = CreateGraph(ptCenters, corrected_v2_20_40_4_type3, corrected_v2_20_40_Errors_4_type3);
    TGraphErrors* corrected_v2_40_60_graph_4_type3 = CreateGraph(ptCenters, corrected_v2_40_60_4_type3, corrected_v2_40_60_Errors_4_type3);

    TGraphErrors* corrected_v2_0_20_graph_5_type3 = CreateGraph(ptCenters, corrected_v2_0_20_5_type3, corrected_v2_0_20_Errors_5_type3);
    TGraphErrors* corrected_v2_20_40_graph_5_type3 = CreateGraph(ptCenters, corrected_v2_20_40_5_type3, corrected_v2_20_40_Errors_5_type3);
    TGraphErrors* corrected_v2_40_60_graph_5_type3 = CreateGraph(ptCenters, corrected_v2_40_60_5_type3, corrected_v2_40_60_Errors_5_type3);

    TGraphErrors* corrected_v2_0_20_graph_6_type3 = CreateGraph(ptCenters, corrected_v2_0_20_6_type3, corrected_v2_0_20_Errors_6_type3);
    TGraphErrors* corrected_v2_20_40_graph_6_type3 = CreateGraph(ptCenters, corrected_v2_20_40_6_type3, corrected_v2_20_40_Errors_6_type3);
    TGraphErrors* corrected_v2_40_60_graph_6_type3 = CreateGraph(ptCenters, corrected_v2_40_60_6_type3, corrected_v2_40_60_Errors_6_type3);
    
    
    
    TGraphErrors* corrected_v2_0_20_graph_1_type4 = CreateGraph(ptCenters, corrected_v2_0_20_1_type4, corrected_v2_0_20_Errors_1_type4);
    TGraphErrors* corrected_v2_20_40_graph_1_type4 = CreateGraph(ptCenters, corrected_v2_20_40_1_type4, corrected_v2_20_40_Errors_1_type4);
    TGraphErrors* corrected_v2_40_60_graph_1_type4 = CreateGraph(ptCenters, corrected_v2_40_60_1_type4, corrected_v2_40_60_Errors_1_type4);
    
    TGraphErrors* corrected_v2_0_20_graph_2_type4 = CreateGraph(ptCenters, corrected_v2_0_20_2_type4, corrected_v2_0_20_Errors_2_type4);
    TGraphErrors* corrected_v2_20_40_graph_2_type4 = CreateGraph(ptCenters, corrected_v2_20_40_2_type4, corrected_v2_20_40_Errors_2_type4);
    TGraphErrors* corrected_v2_40_60_graph_2_type4 = CreateGraph(ptCenters, corrected_v2_40_60_2_type4, corrected_v2_40_60_Errors_2_type4);

    TGraphErrors* corrected_v2_0_20_graph_3_type4 = CreateGraph(ptCenters, corrected_v2_0_20_3_type4, corrected_v2_0_20_Errors_3_type4);
    TGraphErrors* corrected_v2_20_40_graph_3_type4 = CreateGraph(ptCenters, corrected_v2_20_40_3_type4, corrected_v2_20_40_Errors_3_type4);
    TGraphErrors* corrected_v2_40_60_graph_3_type4 = CreateGraph(ptCenters, corrected_v2_40_60_3_type4, corrected_v2_40_60_Errors_3_type4);
    
    TGraphErrors* corrected_v2_0_20_graph_4_type4 = CreateGraph(ptCenters, corrected_v2_0_20_4_type4, corrected_v2_0_20_Errors_4_type4);
    TGraphErrors* corrected_v2_20_40_graph_4_type4 = CreateGraph(ptCenters, corrected_v2_20_40_4_type4, corrected_v2_20_40_Errors_4_type4);
    TGraphErrors* corrected_v2_40_60_graph_4_type4 = CreateGraph(ptCenters, corrected_v2_40_60_4_type4, corrected_v2_40_60_Errors_4_type4);

    TGraphErrors* corrected_v2_0_20_graph_5_type4 = CreateGraph(ptCenters, corrected_v2_0_20_5_type4, corrected_v2_0_20_Errors_5_type4);
    TGraphErrors* corrected_v2_20_40_graph_5_type4 = CreateGraph(ptCenters, corrected_v2_20_40_5_type4, corrected_v2_20_40_Errors_5_type4);
    TGraphErrors* corrected_v2_40_60_graph_5_type4 = CreateGraph(ptCenters, corrected_v2_40_60_5_type4, corrected_v2_40_60_Errors_5_type4);

    TGraphErrors* corrected_v2_0_20_graph_6_type4 = CreateGraph(ptCenters, corrected_v2_0_20_6_type4, corrected_v2_0_20_Errors_6_type4);
    TGraphErrors* corrected_v2_20_40_graph_6_type4 = CreateGraph(ptCenters, corrected_v2_20_40_6_type4, corrected_v2_20_40_Errors_6_type4);
    TGraphErrors* corrected_v2_40_60_graph_6_type4 = CreateGraph(ptCenters, corrected_v2_40_60_6_type4, corrected_v2_40_60_Errors_6_type4);
    
    TGraphErrors* signal_v2_0_20_graph_1 = CreateGraph(ptCenters, signal_v2_0_20_1, signal_v2_0_20_Errors_1);
    TGraphErrors* signal_v2_20_40_graph_1 = CreateGraph(ptCenters, signal_v2_20_40_1, signal_v2_20_40_Errors_1);
    TGraphErrors* signal_v2_40_60_graph_1 = CreateGraph(ptCenters, signal_v2_40_60_1, signal_v2_40_60_Errors_1);
    
    TGraphErrors* signal_v2_0_20_graph_2 = CreateGraph(ptCenters, signal_v2_0_20_2, signal_v2_0_20_Errors_2);
    TGraphErrors* signal_v2_20_40_graph_2 = CreateGraph(ptCenters, signal_v2_20_40_2, signal_v2_20_40_Errors_2);
    TGraphErrors* signal_v2_40_60_graph_2 = CreateGraph(ptCenters, signal_v2_40_60_2, signal_v2_40_60_Errors_2);
    
    TGraphErrors* signal_v2_0_20_graph_3 = CreateGraph(ptCenters, signal_v2_0_20_3, signal_v2_0_20_Errors_3);
    TGraphErrors* signal_v2_20_40_graph_3 = CreateGraph(ptCenters, signal_v2_20_40_3, signal_v2_20_40_Errors_3);
    TGraphErrors* signal_v2_40_60_graph_3 = CreateGraph(ptCenters, signal_v2_40_60_3, signal_v2_40_60_Errors_3);
    
    TGraphErrors* signal_v2_0_20_graph_4 = CreateGraph(ptCenters, signal_v2_0_20_4, signal_v2_0_20_Errors_4);
    TGraphErrors* signal_v2_20_40_graph_4 = CreateGraph(ptCenters, signal_v2_20_40_4, signal_v2_20_40_Errors_4);
    TGraphErrors* signal_v2_40_60_graph_4 = CreateGraph(ptCenters, signal_v2_40_60_4, signal_v2_40_60_Errors_4);
    
    TGraphErrors* signal_v2_0_20_graph_5 = CreateGraph(ptCenters, signal_v2_0_20_5, signal_v2_0_20_Errors_5);
    TGraphErrors* signal_v2_20_40_graph_5 = CreateGraph(ptCenters, signal_v2_20_40_5, signal_v2_20_40_Errors_5);
    TGraphErrors* signal_v2_40_60_graph_5 = CreateGraph(ptCenters, signal_v2_40_60_5, signal_v2_40_60_Errors_5);
    
    TGraphErrors* signal_v2_0_20_graph_6 = CreateGraph(ptCenters, signal_v2_0_20_6, signal_v2_0_20_Errors_6);
    TGraphErrors* signal_v2_20_40_graph_6 = CreateGraph(ptCenters, signal_v2_20_40_6, signal_v2_20_40_Errors_6);
    TGraphErrors* signal_v2_40_60_graph_6 = CreateGraph(ptCenters, signal_v2_40_60_6, signal_v2_40_60_Errors_6);

    
    TGraphErrors* bg_v2_0_20_graph_1 = CreateGraph(ptCenters, bg_v2_0_20_1, bg_v2_0_20_Errors_1);
    TGraphErrors* bg_v2_20_40_graph_1 = CreateGraph(ptCenters, bg_v2_20_40_1, bg_v2_20_40_Errors_1);
    TGraphErrors* bg_v2_40_60_graph_1 = CreateGraph(ptCenters, bg_v2_40_60_1, bg_v2_40_60_Errors_1);
    
    TGraphErrors* bg_v2_0_20_graph_2 = CreateGraph(ptCenters, bg_v2_0_20_2, bg_v2_0_20_Errors_2);
    TGraphErrors* bg_v2_20_40_graph_2 = CreateGraph(ptCenters, bg_v2_20_40_2, bg_v2_20_40_Errors_2);
    TGraphErrors* bg_v2_40_60_graph_2 = CreateGraph(ptCenters, bg_v2_40_60_2, bg_v2_40_60_Errors_2);
    
    TGraphErrors* bg_v2_0_20_graph_3 = CreateGraph(ptCenters, bg_v2_0_20_3, bg_v2_0_20_Errors_3);
    TGraphErrors* bg_v2_20_40_graph_3 = CreateGraph(ptCenters, bg_v2_20_40_3, bg_v2_20_40_Errors_3);
    TGraphErrors* bg_v2_40_60_graph_3 = CreateGraph(ptCenters, bg_v2_40_60_3, bg_v2_40_60_Errors_3);
    
    TGraphErrors* bg_v2_0_20_graph_4 = CreateGraph(ptCenters, bg_v2_0_20_4, bg_v2_0_20_Errors_4);
    TGraphErrors* bg_v2_20_40_graph_4 = CreateGraph(ptCenters, bg_v2_20_40_4, bg_v2_20_40_Errors_4);
    TGraphErrors* bg_v2_40_60_graph_4 = CreateGraph(ptCenters, bg_v2_40_60_4, bg_v2_40_60_Errors_4);
    
    TGraphErrors* bg_v2_0_20_graph_5 = CreateGraph(ptCenters, bg_v2_0_20_5, bg_v2_0_20_Errors_5);
    TGraphErrors* bg_v2_20_40_graph_5 = CreateGraph(ptCenters, bg_v2_20_40_5, bg_v2_20_40_Errors_5);
    TGraphErrors* bg_v2_40_60_graph_5 = CreateGraph(ptCenters, bg_v2_40_60_5, bg_v2_40_60_Errors_5);
    
    TGraphErrors* bg_v2_0_20_graph_6 = CreateGraph(ptCenters, bg_v2_0_20_6, bg_v2_0_20_Errors_6);
    TGraphErrors* bg_v2_20_40_graph_6 = CreateGraph(ptCenters, bg_v2_20_40_6, bg_v2_20_40_Errors_6);
    TGraphErrors* bg_v2_40_60_graph_6 = CreateGraph(ptCenters, bg_v2_40_60_6, bg_v2_40_60_Errors_6);
    
    
    TGraphErrors* bg_v2_0_20_graph_1_type3 = CreateGraph(ptCenters, bg_v2_0_20_1_type3, bg_v2_0_20_Errors_1_type3);
    TGraphErrors* bg_v2_20_40_graph_1_type3 = CreateGraph(ptCenters, bg_v2_20_40_1_type3, bg_v2_20_40_Errors_1_type3);
    TGraphErrors* bg_v2_40_60_graph_1_type3 = CreateGraph(ptCenters, bg_v2_40_60_1_type3, bg_v2_40_60_Errors_1_type3);
    
    TGraphErrors* bg_v2_0_20_graph_2_type3 = CreateGraph(ptCenters, bg_v2_0_20_2_type3, bg_v2_0_20_Errors_2_type3);
    TGraphErrors* bg_v2_20_40_graph_2_type3 = CreateGraph(ptCenters, bg_v2_20_40_2_type3, bg_v2_20_40_Errors_2_type3);
    TGraphErrors* bg_v2_40_60_graph_2_type3 = CreateGraph(ptCenters, bg_v2_40_60_2_type3, bg_v2_40_60_Errors_2_type3);

    TGraphErrors* bg_v2_0_20_graph_3_type3 = CreateGraph(ptCenters, bg_v2_0_20_3_type3, bg_v2_0_20_Errors_3_type3);
    TGraphErrors* bg_v2_20_40_graph_3_type3 = CreateGraph(ptCenters, bg_v2_20_40_3_type3, bg_v2_20_40_Errors_3_type3);
    TGraphErrors* bg_v2_40_60_graph_3_type3 = CreateGraph(ptCenters, bg_v2_40_60_3_type3, bg_v2_40_60_Errors_3_type3);
    
    TGraphErrors* bg_v2_0_20_graph_4_type3 = CreateGraph(ptCenters, bg_v2_0_20_4_type3, bg_v2_0_20_Errors_4_type3);
    TGraphErrors* bg_v2_20_40_graph_4_type3 = CreateGraph(ptCenters, bg_v2_20_40_4_type3, bg_v2_20_40_Errors_4_type3);
    TGraphErrors* bg_v2_40_60_graph_4_type3 = CreateGraph(ptCenters, bg_v2_40_60_4_type3, bg_v2_40_60_Errors_4_type3);

    TGraphErrors* bg_v2_0_20_graph_5_type3 = CreateGraph(ptCenters, bg_v2_0_20_5_type3, bg_v2_0_20_Errors_5_type3);
    TGraphErrors* bg_v2_20_40_graph_5_type3 = CreateGraph(ptCenters, bg_v2_20_40_5_type3, bg_v2_20_40_Errors_5_type3);
    TGraphErrors* bg_v2_40_60_graph_5_type3 = CreateGraph(ptCenters, bg_v2_40_60_5_type3, bg_v2_40_60_Errors_5_type3);

    TGraphErrors* bg_v2_0_20_graph_6_type3 = CreateGraph(ptCenters, bg_v2_0_20_6_type3, bg_v2_0_20_Errors_6_type3);
    TGraphErrors* bg_v2_20_40_graph_6_type3 = CreateGraph(ptCenters, bg_v2_20_40_6_type3, bg_v2_20_40_Errors_6_type3);
    TGraphErrors* bg_v2_40_60_graph_6_type3 = CreateGraph(ptCenters, bg_v2_40_60_6_type3, bg_v2_40_60_Errors_6_type3);

    TGraphErrors* bg_v2_0_20_graph_1_type4 = CreateGraph(ptCenters, bg_v2_0_20_1_type4, bg_v2_0_20_Errors_1_type4);
    TGraphErrors* bg_v2_20_40_graph_1_type4 = CreateGraph(ptCenters, bg_v2_20_40_1_type4, bg_v2_20_40_Errors_1_type4);
    TGraphErrors* bg_v2_40_60_graph_1_type4 = CreateGraph(ptCenters, bg_v2_40_60_1_type4, bg_v2_40_60_Errors_1_type4);
    
    TGraphErrors* bg_v2_0_20_graph_2_type4 = CreateGraph(ptCenters, bg_v2_0_20_2_type4, bg_v2_0_20_Errors_2_type4);
    TGraphErrors* bg_v2_20_40_graph_2_type4 = CreateGraph(ptCenters, bg_v2_20_40_2_type4, bg_v2_20_40_Errors_2_type4);
    TGraphErrors* bg_v2_40_60_graph_2_type4 = CreateGraph(ptCenters, bg_v2_40_60_2_type4, bg_v2_40_60_Errors_2_type4);

    TGraphErrors* bg_v2_0_20_graph_3_type4 = CreateGraph(ptCenters, bg_v2_0_20_3_type4, bg_v2_0_20_Errors_3_type4);
    TGraphErrors* bg_v2_20_40_graph_3_type4 = CreateGraph(ptCenters, bg_v2_20_40_3_type4, bg_v2_20_40_Errors_3_type4);
    TGraphErrors* bg_v2_40_60_graph_3_type4 = CreateGraph(ptCenters, bg_v2_40_60_3_type4, bg_v2_40_60_Errors_3_type4);
    
    TGraphErrors* bg_v2_0_20_graph_4_type4 = CreateGraph(ptCenters, bg_v2_0_20_4_type4, bg_v2_0_20_Errors_4_type4);
    TGraphErrors* bg_v2_20_40_graph_4_type4 = CreateGraph(ptCenters, bg_v2_20_40_4_type4, bg_v2_20_40_Errors_4_type4);
    TGraphErrors* bg_v2_40_60_graph_4_type4 = CreateGraph(ptCenters, bg_v2_40_60_4_type4, bg_v2_40_60_Errors_4_type4);

    TGraphErrors* bg_v2_0_20_graph_5_type4 = CreateGraph(ptCenters, bg_v2_0_20_5_type4, bg_v2_0_20_Errors_5_type4);
    TGraphErrors* bg_v2_20_40_graph_5_type4 = CreateGraph(ptCenters, bg_v2_20_40_5_type4, bg_v2_20_40_Errors_5_type4);
    TGraphErrors* bg_v2_40_60_graph_5_type4 = CreateGraph(ptCenters, bg_v2_40_60_5_type4, bg_v2_40_60_Errors_5_type4);

    TGraphErrors* bg_v2_0_20_graph_6_type4 = CreateGraph(ptCenters, bg_v2_0_20_6_type4, bg_v2_0_20_Errors_6_type4);
    TGraphErrors* bg_v2_20_40_graph_6_type4 = CreateGraph(ptCenters, bg_v2_20_40_6_type4, bg_v2_20_40_Errors_6_type4);
    TGraphErrors* bg_v2_40_60_graph_6_type4 = CreateGraph(ptCenters, bg_v2_40_60_6_type4, bg_v2_40_60_Errors_6_type4);
    
    
    
    double minYaxis_correctedComparisons = -2.0;
    double maxYaxis_correctedComparisons = 3.0;
    double TLatexSize = 0.025;
    // Define the offsets for jitter in Overlays
    double offset = 0.05;
    
    int markerColor_1 = kRed;
    int markerStyle_1 = 20;
    double markerSize_1 = 1.0;
    
    int markerColor_2 = kGreen + 3;
    int markerStyle_2 = 21;
    double markerSize_2 = 1.0;
    
    int markerColor_3 = kBlue;
    int markerStyle_3 = 22;
    double markerSize_3 = 1.2;
    
    int markerColor_4 = kPink + 10;
    int markerStyle_4 = 33;
    double markerSize_4 = 1.2;
    
    int markerColor_5 = kBlack;
    int markerStyle_5 = 4;
    double markerSize_5 = 1.0;
    
    int markerColor_6 = kViolet + 9;
    int markerStyle_6 = 25;
    double markerSize_6 = 1.0;
    
    corrected_v2_0_20_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_0_20_graph_1->SetLineColor(markerColor_1);
    corrected_v2_0_20_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_0_20_graph_1->SetMarkerStyle(markerStyle_1);
    
    corrected_v2_0_20_graph_2->SetMarkerColor(markerColor_2);
    corrected_v2_0_20_graph_2->SetLineColor(markerColor_2);
    corrected_v2_0_20_graph_2->SetMarkerSize(markerSize_2);
    corrected_v2_0_20_graph_2->SetMarkerStyle(markerStyle_2);

    corrected_v2_0_20_graph_3->SetMarkerColor(markerColor_3);
    corrected_v2_0_20_graph_3->SetLineColor(markerColor_3);
    corrected_v2_0_20_graph_3->SetMarkerSize(markerSize_3);
    corrected_v2_0_20_graph_3->SetMarkerStyle(markerStyle_3);
    
    corrected_v2_0_20_graph_4->SetMarkerColor(markerColor_4);
    corrected_v2_0_20_graph_4->SetLineColor(markerColor_4);
    corrected_v2_0_20_graph_4->SetMarkerSize(markerSize_4);
    corrected_v2_0_20_graph_4->SetMarkerStyle(markerStyle_4);

    corrected_v2_0_20_graph_5->SetMarkerColor(markerColor_5);
    corrected_v2_0_20_graph_5->SetLineColor(markerColor_5);
    corrected_v2_0_20_graph_5->SetMarkerSize(markerSize_5);
    corrected_v2_0_20_graph_5->SetMarkerStyle(markerStyle_5);
    
    corrected_v2_0_20_graph_6->SetMarkerColor(markerColor_6);
    corrected_v2_0_20_graph_6->SetLineColor(markerColor_6);
    corrected_v2_0_20_graph_6->SetMarkerSize(markerSize_6);
    corrected_v2_0_20_graph_6->SetMarkerStyle(markerStyle_6);
    
    
    corrected_v2_0_20_graph_1_type4->SetMarkerColor(markerColor_1);
    corrected_v2_0_20_graph_1_type4->SetLineColor(markerColor_1);
    corrected_v2_0_20_graph_1_type4->SetMarkerSize(markerSize_1);
    corrected_v2_0_20_graph_1_type4->SetMarkerStyle(markerStyle_1);
    
    corrected_v2_0_20_graph_2_type4->SetMarkerColor(markerColor_2);
    corrected_v2_0_20_graph_2_type4->SetLineColor(markerColor_2);
    corrected_v2_0_20_graph_2_type4->SetMarkerSize(markerSize_2);
    corrected_v2_0_20_graph_2_type4->SetMarkerStyle(markerStyle_2);

    corrected_v2_0_20_graph_3_type4->SetMarkerColor(markerColor_3);
    corrected_v2_0_20_graph_3_type4->SetLineColor(markerColor_3);
    corrected_v2_0_20_graph_3_type4->SetMarkerSize(markerSize_3);
    corrected_v2_0_20_graph_3_type4->SetMarkerStyle(markerStyle_3);
    
    corrected_v2_0_20_graph_4_type4->SetMarkerColor(markerColor_4);
    corrected_v2_0_20_graph_4_type4->SetLineColor(markerColor_4);
    corrected_v2_0_20_graph_4_type4->SetMarkerSize(markerSize_4);
    corrected_v2_0_20_graph_4_type4->SetMarkerStyle(markerStyle_4);

    corrected_v2_0_20_graph_5_type4->SetMarkerColor(markerColor_5);
    corrected_v2_0_20_graph_5_type4->SetLineColor(markerColor_5);
    corrected_v2_0_20_graph_5_type4->SetMarkerSize(markerSize_5);
    corrected_v2_0_20_graph_5_type4->SetMarkerStyle(markerStyle_5);
    
    corrected_v2_0_20_graph_6_type4->SetMarkerColor(markerColor_6);
    corrected_v2_0_20_graph_6_type4->SetLineColor(markerColor_6);
    corrected_v2_0_20_graph_6_type4->SetMarkerSize(markerSize_6);
    corrected_v2_0_20_graph_6_type4->SetMarkerStyle(markerStyle_6);
    
    
    corrected_v2_20_40_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_20_40_graph_1->SetLineColor(markerColor_1);
    corrected_v2_20_40_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_20_40_graph_1->SetMarkerStyle(markerStyle_1);
    
    corrected_v2_20_40_graph_2->SetMarkerColor(markerColor_2);
    corrected_v2_20_40_graph_2->SetLineColor(markerColor_2);
    corrected_v2_20_40_graph_2->SetMarkerSize(markerSize_2);
    corrected_v2_20_40_graph_2->SetMarkerStyle(markerStyle_2);

    corrected_v2_20_40_graph_3->SetMarkerColor(markerColor_3);
    corrected_v2_20_40_graph_3->SetLineColor(markerColor_3);
    corrected_v2_20_40_graph_3->SetMarkerSize(markerSize_3);
    corrected_v2_20_40_graph_3->SetMarkerStyle(markerStyle_3);
    
    corrected_v2_20_40_graph_4->SetMarkerColor(markerColor_4);
    corrected_v2_20_40_graph_4->SetLineColor(markerColor_4);
    corrected_v2_20_40_graph_4->SetMarkerSize(markerSize_4);
    corrected_v2_20_40_graph_4->SetMarkerStyle(markerStyle_4);

    corrected_v2_20_40_graph_5->SetMarkerColor(markerColor_5);
    corrected_v2_20_40_graph_5->SetLineColor(markerColor_5);
    corrected_v2_20_40_graph_5->SetMarkerSize(markerSize_5);
    corrected_v2_20_40_graph_5->SetMarkerStyle(markerStyle_5);
    
    corrected_v2_20_40_graph_6->SetMarkerColor(markerColor_6);
    corrected_v2_20_40_graph_6->SetLineColor(markerColor_6);
    corrected_v2_20_40_graph_6->SetMarkerSize(markerSize_6);
    corrected_v2_20_40_graph_6->SetMarkerStyle(markerStyle_6);
    
    
    corrected_v2_40_60_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_40_60_graph_1->SetLineColor(markerColor_1);
    corrected_v2_40_60_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_40_60_graph_1->SetMarkerStyle(markerStyle_1);
    
    corrected_v2_40_60_graph_2->SetMarkerColor(markerColor_2);
    corrected_v2_40_60_graph_2->SetLineColor(markerColor_2);
    corrected_v2_40_60_graph_2->SetMarkerSize(markerSize_2);
    corrected_v2_40_60_graph_2->SetMarkerStyle(markerStyle_2);

    corrected_v2_40_60_graph_3->SetMarkerColor(markerColor_3);
    corrected_v2_40_60_graph_3->SetLineColor(markerColor_3);
    corrected_v2_40_60_graph_3->SetMarkerSize(markerSize_3);
    corrected_v2_40_60_graph_3->SetMarkerStyle(markerStyle_3);
    
    corrected_v2_40_60_graph_4->SetMarkerColor(markerColor_4);
    corrected_v2_40_60_graph_4->SetLineColor(markerColor_4);
    corrected_v2_40_60_graph_4->SetMarkerSize(markerSize_4);
    corrected_v2_40_60_graph_4->SetMarkerStyle(markerStyle_4);

    corrected_v2_40_60_graph_5->SetMarkerColor(markerColor_5);
    corrected_v2_40_60_graph_5->SetLineColor(markerColor_5);
    corrected_v2_40_60_graph_5->SetMarkerSize(markerSize_5);
    corrected_v2_40_60_graph_5->SetMarkerStyle(markerStyle_5);
    
    corrected_v2_40_60_graph_6->SetMarkerColor(markerColor_6);
    corrected_v2_40_60_graph_6->SetLineColor(markerColor_6);
    corrected_v2_40_60_graph_6->SetMarkerSize(markerSize_6);
    corrected_v2_40_60_graph_6->SetMarkerStyle(markerStyle_6);
    
    
    /*
     0-20 percent v2 corrected
     */
    TCanvas *c_Overlay_0_20_v2_corrected = new TCanvas("c_Overlay_1", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    corrected_v2_0_20_graph_4_type4->Draw("AP");  // This will be the base graph
    corrected_v2_0_20_graph_4_type4->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 0-20% Centrality, Min Clus Energy Variations, Sideband = (#mu + 3#sigma, 0.4) GeV");
    corrected_v2_0_20_graph_4_type4->GetXaxis()->SetTitle("p_{T}");
    corrected_v2_0_20_graph_4_type4->GetYaxis()->SetTitle("v_{2}");

    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_0_20_graph_1_type4->SetPoint(i, ptCenters[i] - 3.0 * offset, corrected_v2_0_20_1_type4[i]);
        corrected_v2_0_20_graph_2_type4->SetPoint(i, ptCenters[i] - 2.0 * offset, corrected_v2_0_20_2_type4[i]);
        corrected_v2_0_20_graph_3_type4->SetPoint(i, ptCenters[i] - 1.0 * offset, corrected_v2_0_20_3_type4[i]);
        corrected_v2_0_20_graph_5_type4->SetPoint(i, ptCenters[i] + 1.0 * offset, corrected_v2_0_20_5_type4[i]);
        corrected_v2_0_20_graph_6_type4->SetPoint(i, ptCenters[i] + 2.0 * offset, corrected_v2_0_20_6_type4[i]);
    }
    corrected_v2_0_20_graph_1_type4 -> Draw("P SAME");
    corrected_v2_0_20_graph_2_type4 -> Draw("P SAME");
    corrected_v2_0_20_graph_3_type4 -> Draw("P SAME");
    corrected_v2_0_20_graph_5_type4 -> Draw("P SAME");
    corrected_v2_0_20_graph_6_type4 -> Draw("P SAME");

    
    corrected_v2_0_20_graph_4_type4->SetMinimum(minYaxis_correctedComparisons); // Set the minimum y value
    corrected_v2_0_20_graph_4_type4->SetMaximum(maxYaxis_correctedComparisons); // Set the maximum y value
    
    TLegend *legend_Overlay_0_20_v2_corrected = new TLegend(0.11, 0.69, 0.81, 0.89);
    legend_Overlay_0_20_v2_corrected->SetNColumns(2);

    legend_Overlay_0_20_v2_corrected->SetBorderSize(0);
    legend_Overlay_0_20_v2_corrected->SetTextSize(TLatexSize);

    legend_Overlay_0_20_v2_corrected->AddEntry(corrected_v2_0_20_graph_1_type4, "0-20%, E_{A} #geq 1.0 GeV, E_{B} #geq 1.0 GeV", "pe");
    legend_Overlay_0_20_v2_corrected->AddEntry(corrected_v2_0_20_graph_2_type4, "0-20%, E_{A} #geq 1.0 GeV, E_{B} #geq 2.25 GeV", "pe");
    legend_Overlay_0_20_v2_corrected->AddEntry(corrected_v2_0_20_graph_3_type4, "0-20%, E_{A} #geq 1.0 GeV, E_{B} #geq 2.75 GeV", "pe");
    legend_Overlay_0_20_v2_corrected->AddEntry(corrected_v2_0_20_graph_4_type4, "0-20%, E_{A} #geq 1.25 GeV, E_{B} #geq 2.5 GeV", "pe");
    legend_Overlay_0_20_v2_corrected->AddEntry(corrected_v2_0_20_graph_5_type4, "0-20%, E_{A} #geq 1.75 GeV, E_{B} #geq 2.0 GeV", "pe");
    legend_Overlay_0_20_v2_corrected->AddEntry(corrected_v2_0_20_graph_6_type4, "0-20%, E_{A} #geq 1.75 GeV, E_{B} #geq 2.75 GeV", "pe");

    legend_Overlay_0_20_v2_corrected->Draw();
    DrawZeroLine(c_Overlay_0_20_v2_corrected);
    c_Overlay_0_20_v2_corrected->Modified();
    c_Overlay_0_20_v2_corrected->Update();
    c_Overlay_0_20_v2_corrected->SaveAs((BasePlotOutputPath + "/Overlay_0_20_v2.png").c_str());
    
    
    /*
     20-40 percent v2 corrected
     */
    TCanvas *c_Overlay_20_40_v2_corrected = new TCanvas("c_Overlay_2", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 20-40% Centrality", 800, 600);
    corrected_v2_20_40_graph_4->Draw("AP");  // This will be the base graph
    corrected_v2_20_40_graph_4->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 20-40% Centrality, Min Clus Energy Variations, Sideband = (#mu + 3#sigma, 0.5) GeV");
    corrected_v2_20_40_graph_4->GetXaxis()->SetTitle("p_{T}");
    corrected_v2_20_40_graph_4->GetYaxis()->SetTitle("v_{2}");

    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_20_40_graph_1->SetPoint(i, ptCenters[i] - 3.0 * offset, corrected_v2_20_40_1[i]);
        corrected_v2_20_40_graph_2->SetPoint(i, ptCenters[i] - 2.0 * offset, corrected_v2_20_40_2[i]);
        corrected_v2_20_40_graph_3->SetPoint(i, ptCenters[i] - 1.0 * offset, corrected_v2_20_40_3[i]);
        corrected_v2_20_40_graph_5->SetPoint(i, ptCenters[i] + 1.0 * offset, corrected_v2_20_40_5[i]);
        corrected_v2_20_40_graph_6->SetPoint(i, ptCenters[i] + 2.0 * offset, corrected_v2_20_40_6[i]);
    }
    corrected_v2_20_40_graph_1 -> Draw("P SAME");
    corrected_v2_20_40_graph_2 -> Draw("P SAME");
    corrected_v2_20_40_graph_3 -> Draw("P SAME");
    corrected_v2_20_40_graph_5 -> Draw("P SAME");
    corrected_v2_20_40_graph_6 -> Draw("P SAME");

    
    corrected_v2_20_40_graph_4->SetMinimum(minYaxis_correctedComparisons); // Set the minimum y value
    corrected_v2_20_40_graph_4->SetMaximum(maxYaxis_correctedComparisons); // Set the maximum y value
    
    TLegend *legend_Overlay_20_40_v2_corrected = new TLegend(0.11, 0.69, 0.81, 0.89);
    legend_Overlay_20_40_v2_corrected->SetNColumns(2);

    legend_Overlay_20_40_v2_corrected->SetBorderSize(0);
    legend_Overlay_20_40_v2_corrected->SetTextSize(TLatexSize);

    legend_Overlay_20_40_v2_corrected->AddEntry(corrected_v2_20_40_graph_1, "20-40%, E_{A} #geq 1.0 GeV, E_{B} #geq 1.0 GeV", "pe");
    legend_Overlay_20_40_v2_corrected->AddEntry(corrected_v2_20_40_graph_2, "20-40%, E_{A} #geq 1.0 GeV, E_{B} #geq 2.25 GeV", "pe");
    legend_Overlay_20_40_v2_corrected->AddEntry(corrected_v2_20_40_graph_3, "20-40%, E_{A} #geq 1.0 GeV, E_{B} #geq 2.75 GeV", "pe");
    legend_Overlay_20_40_v2_corrected->AddEntry(corrected_v2_20_40_graph_4, "20-40%, E_{A} #geq 1.25 GeV, E_{B} #geq 2.5 GeV", "pe");
    legend_Overlay_20_40_v2_corrected->AddEntry(corrected_v2_20_40_graph_5, "20-40%, E_{A} #geq 1.75 GeV, E_{B} #geq 2.0 GeV", "pe");
    legend_Overlay_20_40_v2_corrected->AddEntry(corrected_v2_20_40_graph_6, "20-40%, E_{A} #geq 1.75 GeV, E_{B} #geq 2.75 GeV", "pe");

    legend_Overlay_20_40_v2_corrected->Draw();
    DrawZeroLine(c_Overlay_20_40_v2_corrected);
    c_Overlay_20_40_v2_corrected->Modified();
    c_Overlay_20_40_v2_corrected->Update();
    c_Overlay_20_40_v2_corrected->SaveAs((BasePlotOutputPath + "/Overlay_20_40_v2.png").c_str());
    
    

    
    /*
     40-60 percent v2 corrected
     */
    TCanvas *c_Overlay_40_60_v2_corrected = new TCanvas("c_Overlay_3", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    corrected_v2_40_60_graph_4->Draw("AP");  // This will be the base graph
    corrected_v2_40_60_graph_4->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Min Clus Energy Variations, Sideband = (#mu + 3#sigma, 0.5) GeV");
    corrected_v2_40_60_graph_4->GetXaxis()->SetTitle("p_{T}");
    corrected_v2_40_60_graph_4->GetYaxis()->SetTitle("v_{2}");

    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_40_60_graph_1->SetPoint(i, ptCenters[i] - 3.0 * offset, corrected_v2_40_60_1[i]);
        corrected_v2_40_60_graph_2->SetPoint(i, ptCenters[i] - 2.0 * offset, corrected_v2_40_60_2[i]);
        corrected_v2_40_60_graph_3->SetPoint(i, ptCenters[i] - 1.0 * offset, corrected_v2_40_60_3[i]);
        corrected_v2_40_60_graph_5->SetPoint(i, ptCenters[i] + 1.0 * offset, corrected_v2_40_60_5[i]);
        corrected_v2_40_60_graph_6->SetPoint(i, ptCenters[i] + 2.0 * offset, corrected_v2_40_60_6[i]);
    }
    corrected_v2_40_60_graph_1 -> Draw("P SAME");
    corrected_v2_40_60_graph_2 -> Draw("P SAME");
    corrected_v2_40_60_graph_3 -> Draw("P SAME");
    corrected_v2_40_60_graph_5 -> Draw("P SAME");
    corrected_v2_40_60_graph_6 -> Draw("P SAME");

    
    corrected_v2_40_60_graph_4->SetMinimum(minYaxis_correctedComparisons); // Set the minimum y value
    corrected_v2_40_60_graph_4->SetMaximum(maxYaxis_correctedComparisons); // Set the maximum y value
    
    TLegend *legend_Overlay_40_60_v2_corrected = new TLegend(0.11, 0.69, 0.81, 0.89);
    legend_Overlay_40_60_v2_corrected->SetNColumns(2);

    legend_Overlay_40_60_v2_corrected->SetBorderSize(0);
    legend_Overlay_40_60_v2_corrected->SetTextSize(TLatexSize);

    legend_Overlay_40_60_v2_corrected->AddEntry(corrected_v2_40_60_graph_1, "40-60%, E_{A} #geq 1.0 GeV, E_{B} #geq 1.0 GeV", "pe");
    legend_Overlay_40_60_v2_corrected->AddEntry(corrected_v2_40_60_graph_2, "40-60%, E_{A} #geq 1.0 GeV, E_{B} #geq 2.25 GeV", "pe");
    legend_Overlay_40_60_v2_corrected->AddEntry(corrected_v2_40_60_graph_3, "40-60%, E_{A} #geq 1.0 GeV, E_{B} #geq 2.75 GeV", "pe");
    legend_Overlay_40_60_v2_corrected->AddEntry(corrected_v2_40_60_graph_4, "40-60%, E_{A} #geq 1.25 GeV, E_{B} #geq 2.5 GeV", "pe");
    legend_Overlay_40_60_v2_corrected->AddEntry(corrected_v2_40_60_graph_5, "40-60%, E_{A} #geq 1.75 GeV, E_{B} #geq 2.0 GeV", "pe");
    legend_Overlay_40_60_v2_corrected->AddEntry(corrected_v2_40_60_graph_6, "40-60%, E_{A} #geq 1.75 GeV, E_{B} #geq 2.75 GeV", "pe");

    legend_Overlay_40_60_v2_corrected->Draw();
    DrawZeroLine(c_Overlay_40_60_v2_corrected);
    c_Overlay_40_60_v2_corrected->Modified();
    c_Overlay_40_60_v2_corrected->Update();
    c_Overlay_40_60_v2_corrected->SaveAs((BasePlotOutputPath + "/Overlay_40_60_v2.png").c_str());
    
    
    
    TCanvas *c_Overlay_4 = new TCanvas("c_Overlay_4", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    corrected_v2_0_20_graph_1->Draw("AP");  // This will be the base graph
    corrected_v2_0_20_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 0-20% Centrality, Min Clus Energy Cut of 1.0 GeV, Sideband = (#mu + 3#sigma, 0.5) GeV GeV");
    corrected_v2_0_20_graph_1->GetXaxis()->SetTitle("p_{T}");
    corrected_v2_0_20_graph_1->GetYaxis()->SetTitle("v_{2}");
    corrected_v2_0_20_graph_1->SetMinimum(-0.2); // Set the minimum y value
    corrected_v2_0_20_graph_1->SetMaximum(0.45); // Set the maximum y value
    DrawZeroLine(c_Overlay_0_20_v2_corrected);
    c_Overlay_4->Modified();
    c_Overlay_4->Update();
    c_Overlay_4->SaveAs((BasePlotOutputPath + "/Overlay_0_20_v2_singleCut.png").c_str());
    
    TCanvas *c_Overlay_5 = new TCanvas("c_Overlay_5", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 20-40% Centrality", 800, 600);
    corrected_v2_20_40_graph_1->Draw("AP");  // This will be the base graph
    corrected_v2_20_40_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 20-40% Centrality, Min Clus Energy Cut of 1.0 GeV, Sideband = (#mu + 3#sigma, 0.5) GeV GeV");
    corrected_v2_20_40_graph_1->GetXaxis()->SetTitle("p_{T}");
    corrected_v2_20_40_graph_1->GetYaxis()->SetTitle("v_{2}");
    corrected_v2_20_40_graph_1->SetMinimum(-0.2); // Set the minimum y value
    corrected_v2_20_40_graph_1->SetMaximum(0.45); // Set the maximum y value
    DrawZeroLine(c_Overlay_20_40_v2_corrected);
    c_Overlay_5->Modified();
    c_Overlay_5->Update();
    c_Overlay_5->SaveAs((BasePlotOutputPath + "/Overlay_20_40_v2_singleCut.png").c_str());
    
    TCanvas *c_Overlay_6 = new TCanvas("c_Overlay_6", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    corrected_v2_40_60_graph_1->Draw("AP");  // This will be the base graph
    corrected_v2_40_60_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Min Clus Energy Cut of 1.0 GeV, Sideband = (#mu + 3#sigma, 0.5) GeV GeV");
    corrected_v2_40_60_graph_1->GetXaxis()->SetTitle("p_{T}");
    corrected_v2_40_60_graph_1->GetYaxis()->SetTitle("v_{2}");
    corrected_v2_40_60_graph_1->SetMinimum(-0.2); // Set the minimum y value
    corrected_v2_40_60_graph_1->SetMaximum(0.45); // Set the maximum y value
    DrawZeroLine(c_Overlay_40_60_v2_corrected);
    c_Overlay_6->Modified();
    c_Overlay_6->Update();
    c_Overlay_6->SaveAs((BasePlotOutputPath + "/Overlay_40_60_v2_singleCut.png").c_str());
    
    
    corrected_v2_0_20_graph_1_type3->SetMarkerColor(kBlue);
    corrected_v2_0_20_graph_1_type3->SetLineColor(kBlue);
    corrected_v2_0_20_graph_1_type3->SetMarkerSize(markerSize_1);
    corrected_v2_0_20_graph_1_type3->SetMarkerStyle(20);
    
    corrected_v2_0_20_graph_1_type4->SetMarkerColor(kGreen + 3);
    corrected_v2_0_20_graph_1_type4->SetLineColor(kGreen + 3);
    corrected_v2_0_20_graph_1_type4->SetMarkerSize(markerSize_1);
    corrected_v2_0_20_graph_1_type4->SetMarkerStyle(22);
    
    /*
     0-20 percent v2 corrected
     */
    TCanvas *c_Overlay_1_sideBandVariation = new TCanvas("c_Overlay_1_sideBandVariation", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    corrected_v2_0_20_graph_1_type4->Draw("AP");  // This will be the base graph
    corrected_v2_0_20_graph_1_type4->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 0-20% Centrality, Min Clus Energy 1.0 GeV, Sideband Variations");
    corrected_v2_0_20_graph_1_type4->GetXaxis()->SetTitle("p_{T}");
    corrected_v2_0_20_graph_1_type4->GetYaxis()->SetTitle("v_{2}");

    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_0_20_graph_1_type3->SetPoint(i, ptCenters[i] - 2.0 * offset, corrected_v2_0_20_1_type3[i]);
        corrected_v2_0_20_graph_1->SetPoint(i, ptCenters[i] + 2.0 * offset, corrected_v2_0_20_1[i]);
    }
    corrected_v2_0_20_graph_1_type3 -> Draw("P SAME");
    corrected_v2_0_20_graph_1 -> Draw("P SAME");
    
    corrected_v2_0_20_graph_1_type4->SetMinimum(-0.2); // Set the minimum y value
    corrected_v2_0_20_graph_1_type4->SetMaximum(0.75); // Set the maximum y value
    
    TLegend *legend_Overlay_0_20_v2_corrected_sideBandVariation = new TLegend(0.11, 0.69, 0.41, 0.89);
    legend_Overlay_0_20_v2_corrected_sideBandVariation->SetBorderSize(0);
    legend_Overlay_0_20_v2_corrected_sideBandVariation->SetTextSize(TLatexSize);

    legend_Overlay_0_20_v2_corrected_sideBandVariation->AddEntry(corrected_v2_0_20_graph_1_type3, "0-20%, Sideband = (#mu + 3#sigma, 0.3) GeV", "pe");
    legend_Overlay_0_20_v2_corrected_sideBandVariation->AddEntry(corrected_v2_0_20_graph_1_type4, "0-20%, Sideband = (#mu + 3#sigma, 0.4) GeV", "pe");
    legend_Overlay_0_20_v2_corrected_sideBandVariation->AddEntry(corrected_v2_0_20_graph_1, "0-20%, Sideband = (#mu + 3#sigma, 0.5) GeV", "pe");

    legend_Overlay_0_20_v2_corrected_sideBandVariation->Draw();
    DrawZeroLine(c_Overlay_1_sideBandVariation);
    c_Overlay_1_sideBandVariation->Modified();
    c_Overlay_1_sideBandVariation->Update();
    c_Overlay_1_sideBandVariation->SaveAs((BasePlotOutputPath + "/Overlay_0_20_v2_sideBandVariation.png").c_str());
    
    
    corrected_v2_20_40_graph_1_type3->SetMarkerColor(kBlue);
    corrected_v2_20_40_graph_1_type3->SetLineColor(kBlue);
    corrected_v2_20_40_graph_1_type3->SetMarkerSize(markerSize_1);
    corrected_v2_20_40_graph_1_type3->SetMarkerStyle(20);
    
    corrected_v2_20_40_graph_1_type4->SetMarkerColor(kGreen + 3);
    corrected_v2_20_40_graph_1_type4->SetLineColor(kGreen + 3);
    corrected_v2_20_40_graph_1_type4->SetMarkerSize(markerSize_1);
    corrected_v2_20_40_graph_1_type4->SetMarkerStyle(22);
    
    TCanvas *c_Overlay_2_sideBandVariation = new TCanvas("c_Overlay_2_sideBandVariation", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 20-40% Centrality", 800, 600);
    corrected_v2_20_40_graph_1_type4->Draw("AP");  // This will be the base graph
    corrected_v2_20_40_graph_1_type4->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 20-40% Centrality, Min Clus Energy 1.0 GeV, Sideband Variations");
    corrected_v2_20_40_graph_1_type4->GetXaxis()->SetTitle("p_{T}");
    corrected_v2_20_40_graph_1_type4->GetYaxis()->SetTitle("v_{2}");

    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_20_40_graph_1_type3->SetPoint(i, ptCenters[i] - 2.0 * offset, corrected_v2_20_40_1_type3[i]);
        corrected_v2_20_40_graph_1->SetPoint(i, ptCenters[i] + 2.0 * offset, corrected_v2_20_40_1[i]);
    }
    corrected_v2_20_40_graph_1_type3 -> Draw("P SAME");
    corrected_v2_20_40_graph_1 -> Draw("P SAME");
    
    corrected_v2_20_40_graph_1_type4->SetMinimum(-0.2); // Set the minimum y value
    corrected_v2_20_40_graph_1_type4->SetMaximum(0.75); // Set the maximum y value
    
    TLegend *legend_Overlay_20_40_v2_corrected_sideBandVariation = new TLegend(0.11, 0.69, 0.41, 0.89);
    legend_Overlay_20_40_v2_corrected_sideBandVariation->SetBorderSize(0);
    legend_Overlay_20_40_v2_corrected_sideBandVariation->SetTextSize(TLatexSize);

    legend_Overlay_20_40_v2_corrected_sideBandVariation->AddEntry(corrected_v2_20_40_graph_1_type3, "20-40%, Sideband = (#mu + 3#sigma, 0.3) GeV", "pe");
    legend_Overlay_20_40_v2_corrected_sideBandVariation->AddEntry(corrected_v2_20_40_graph_1_type4, "20-40%, Sideband = (#mu + 3#sigma, 0.4) GeV", "pe");
    legend_Overlay_20_40_v2_corrected_sideBandVariation->AddEntry(corrected_v2_20_40_graph_1, "20-40%, Sideband = (#mu + 3#sigma, 0.5) GeV", "pe");

    legend_Overlay_20_40_v2_corrected_sideBandVariation->Draw();
    DrawZeroLine(c_Overlay_2_sideBandVariation);
    c_Overlay_2_sideBandVariation->Modified();
    c_Overlay_2_sideBandVariation->Update();
    c_Overlay_2_sideBandVariation->SaveAs((BasePlotOutputPath + "/Overlay_20_40_v2_sideBandVariation.png").c_str());
    
    
    corrected_v2_40_60_graph_1_type3->SetMarkerColor(kBlue);
    corrected_v2_40_60_graph_1_type3->SetLineColor(kBlue);
    corrected_v2_40_60_graph_1_type3->SetMarkerSize(markerSize_1);
    corrected_v2_40_60_graph_1_type3->SetMarkerStyle(20);
    
    corrected_v2_40_60_graph_1_type4->SetMarkerColor(kGreen + 3);
    corrected_v2_40_60_graph_1_type4->SetLineColor(kGreen + 3);
    corrected_v2_40_60_graph_1_type4->SetMarkerSize(markerSize_1);
    corrected_v2_40_60_graph_1_type4->SetMarkerStyle(22);
    
    TCanvas *c_Overlay_3_sideBandVariation = new TCanvas("c_Overlay_3_sideBandVariation", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    corrected_v2_40_60_graph_1_type4->Draw("AP");  // This will be the base graph
    corrected_v2_40_60_graph_1_type4->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Min Clus Energy 1.0 GeV, Sideband Variations");
    corrected_v2_40_60_graph_1_type4->GetXaxis()->SetTitle("p_{T}");
    corrected_v2_40_60_graph_1_type4->GetYaxis()->SetTitle("v_{2}");

    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_40_60_graph_1_type3->SetPoint(i, ptCenters[i] - 2.0 * offset, corrected_v2_40_60_1_type3[i]);
        corrected_v2_40_60_graph_1->SetPoint(i, ptCenters[i] + 2.0 * offset, corrected_v2_40_60_1[i]);
    }
    corrected_v2_40_60_graph_1_type3 -> Draw("P SAME");
    corrected_v2_40_60_graph_1 -> Draw("P SAME");
    
    corrected_v2_40_60_graph_1_type4->SetMinimum(-0.2); // Set the minimum y value
    corrected_v2_40_60_graph_1_type4->SetMaximum(0.75); // Set the maximum y value
    
    TLegend *legend_Overlay_40_60_v2_corrected_sideBandVariation = new TLegend(0.11, 0.69, 0.41, 0.89);
    legend_Overlay_40_60_v2_corrected_sideBandVariation->SetBorderSize(0);
    legend_Overlay_40_60_v2_corrected_sideBandVariation->SetTextSize(TLatexSize);

    legend_Overlay_40_60_v2_corrected_sideBandVariation->AddEntry(corrected_v2_40_60_graph_1_type3, "40-60%, Sideband = (#mu + 3#sigma, 0.3) GeV", "pe");
    legend_Overlay_40_60_v2_corrected_sideBandVariation->AddEntry(corrected_v2_40_60_graph_1_type4, "40-60%, Sideband = (#mu + 3#sigma, 0.4) GeV", "pe");
    legend_Overlay_40_60_v2_corrected_sideBandVariation->AddEntry(corrected_v2_40_60_graph_1, "40-60%, Sideband = (#mu + 3#sigma, 0.5) GeV", "pe");

    legend_Overlay_40_60_v2_corrected_sideBandVariation->Draw();
    DrawZeroLine(c_Overlay_3_sideBandVariation);
    c_Overlay_3_sideBandVariation->Modified();
    c_Overlay_3_sideBandVariation->Update();
    c_Overlay_3_sideBandVariation->SaveAs((BasePlotOutputPath + "/Overlay_40_60_v2_sideBandVariation.png").c_str());
    
    
    
}
void vN_plotter() {
    std::string baseDataPath = "/Users/patsfan753/Desktop/Analysis_3_29/vN_Output/";
    std::string filePath1 = baseDataPath + "1-1/vn-1-1.csv";
    std::string filePath2 = baseDataPath + "1-2_25/vn-1-2.25.csv";
    std::string filePath3 = baseDataPath + "1-2_75/vn-1-2.75.csv";
    std::string filePath4 = baseDataPath + "1_25-2_5/vn-1.25-2.5.csv";
    std::string filePath5 = baseDataPath + "1_75-2/vn-1.75-2.csv";
    std::string filePath6 = baseDataPath + "1_75-2_75/vn-1.75-2.75.csv";

    Data data1, data2, data3, data4, data5, data6;

    // Read the datasets
    Read_DataSet(filePath1, data1);
    Read_DataSet(filePath2, data2);
    Read_DataSet(filePath3, data3);
    Read_DataSet(filePath4, data4);
    Read_DataSet(filePath5, data5);
    Read_DataSet(filePath6, data6);

    plotting(data1, data2, data3, data4, data5, data6);
}



