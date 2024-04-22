#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

#include <TFile.h>
#include <TH1F.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

std::vector<double> ptCenters = {2.25, 2.75, 3.25, 3.75, 4.25, 4.75}; // Mid-points of pT ranges
std::string BasePlotOutputPath = "/Users/patsfan753/Desktop/p015";

std::string baseDataPath_EmCal_Systematics = BasePlotOutputPath + "/Systematics_Analysis-v2-Checks/EMCal_systematics/";
std::string baseDataPath_AsymmetryCutVariations = BasePlotOutputPath + "/Systematics_Analysis-v2-Checks/AsymmetryCutVariations/";
std::string baseDataPath_SampleSizeVariations = BasePlotOutputPath + "/Systematics_Analysis-v2-Checks/SampleSize_Variation/";
std::string baseDataPath_SignalWindowVariations = BasePlotOutputPath + "/Systematics_Analysis-v2-Checks/SignalWindow_Variations/";
std::string baseDataPath_BackgroundWindowVariations = BasePlotOutputPath + "/Systematics_Analysis-v2-Checks/BackgroundWindow_Variations/";



struct EMCal_Systematic_Data {
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


void ReadStatUncertainties_emCalScaleCSV(const std::string& filePath, EMCal_Systematic_Data& EMCal_Systematic_Data) {
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
            EMCal_Systematic_Data.RelativeUncertainty_SYST1CEMC_40_60.push_back(relativeUncertainty_SYST1CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST1CEMC_40_60.push_back(absoluteUncertainty_SYST1CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST2CEMC_40_60.push_back(relativeUncertainty_SYST2CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST2CEMC_40_60.push_back(absoluteUncertainty_SYST2CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3DCEMC_40_60.push_back(relativeUncertainty_SYST3DCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3DCEMC_40_60.push_back(absoluteUncertainty_SYST3DCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3UCEMC_40_60.push_back(relativeUncertainty_SYST3UCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3UCEMC_40_60.push_back(absoluteUncertainty_SYST3UCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST4CEMC_40_60.push_back(relativeUncertainty_SYST4CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST4CEMC_40_60.push_back(absoluteUncertainty_SYST4CEMC);
            
            EMCal_Systematic_Data.Relative_QuadSum_40_60.push_back(relative_QuadSum);
            EMCal_Systematic_Data.Absolute_QuadSum_40_60.push_back(Absolute_QuadSum);
            
            
        } else if (index >= 6 && index <= 11) {
            EMCal_Systematic_Data.RelativeUncertainty_SYST1CEMC_20_40.push_back(relativeUncertainty_SYST1CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST1CEMC_20_40.push_back(absoluteUncertainty_SYST1CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST2CEMC_20_40.push_back(relativeUncertainty_SYST2CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST2CEMC_20_40.push_back(absoluteUncertainty_SYST2CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3DCEMC_20_40.push_back(relativeUncertainty_SYST3DCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3DCEMC_20_40.push_back(absoluteUncertainty_SYST3DCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3UCEMC_20_40.push_back(relativeUncertainty_SYST3UCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3UCEMC_20_40.push_back(absoluteUncertainty_SYST3UCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST4CEMC_20_40.push_back(relativeUncertainty_SYST4CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST4CEMC_20_40.push_back(absoluteUncertainty_SYST4CEMC);
            
            EMCal_Systematic_Data.Relative_QuadSum_20_40.push_back(relative_QuadSum);
            EMCal_Systematic_Data.Absolute_QuadSum_20_40.push_back(Absolute_QuadSum);
            
        } else if (index >= 12 && index <= 17) {
            EMCal_Systematic_Data.RelativeUncertainty_SYST1CEMC_0_20.push_back(relativeUncertainty_SYST1CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST1CEMC_0_20.push_back(absoluteUncertainty_SYST1CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST2CEMC_0_20.push_back(relativeUncertainty_SYST2CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST2CEMC_0_20.push_back(absoluteUncertainty_SYST2CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3DCEMC_0_20.push_back(relativeUncertainty_SYST3DCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3DCEMC_0_20.push_back(absoluteUncertainty_SYST3DCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3UCEMC_0_20.push_back(relativeUncertainty_SYST3UCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3UCEMC_0_20.push_back(absoluteUncertainty_SYST3UCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST4CEMC_0_20.push_back(relativeUncertainty_SYST4CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST4CEMC_0_20.push_back(absoluteUncertainty_SYST4CEMC);
            
            EMCal_Systematic_Data.Relative_QuadSum_0_20.push_back(relative_QuadSum);
            EMCal_Systematic_Data.Absolute_QuadSum_0_20.push_back(Absolute_QuadSum);
            
        }
    }
    file.close();
    
}

struct Data {
    std::vector<double> corrected_v2_0_20, corrected_v2_20_40, corrected_v2_40_60;
    std::vector<double> corrected_v2_0_20_Errors, corrected_v2_20_40_Errors, corrected_v2_40_60_Errors;
    
    std::vector<double> corrected_v2_0_20_type4, corrected_v2_20_40_type4, corrected_v2_40_60_type4;
    std::vector<double> corrected_v2_0_20_Errors_type4, corrected_v2_20_40_Errors_type4, corrected_v2_40_60_Errors_type4;
    
    std::vector<double> signal_v2_0_20, signal_v2_20_40, signal_v2_40_60;
    std::vector<double> signal_v2_0_20_Errors, signal_v2_20_40_Errors, signal_v2_40_60_Errors;
    
    std::vector<double> bg_v2_0_20, bg_v2_20_40, bg_v2_40_60;
    std::vector<double> bg_v2_0_20_Errors, bg_v2_20_40_Errors, bg_v2_40_60_Errors;
    
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
        
        double v2_corrected_type4 = std::stod(rowData.at(5));
        double v2_error_corrected_type4 = std::stod(rowData.at(6));
        
        double v2_signal = std::stod(rowData.at(9));
        double v2_error_signal = std::stod(rowData.at(10));
        
        double v2_bg = std::stod(rowData.at(11));
        double v2_error_bg = std::stod(rowData.at(12));
        
    
        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            data.corrected_v2_40_60.push_back(v2_corrected);
            data.corrected_v2_40_60_Errors.push_back(v2_error_corrected);
            
            data.corrected_v2_40_60_type4.push_back(v2_corrected_type4);
            data.corrected_v2_40_60_Errors_type4.push_back(v2_error_corrected_type4);
            
            data.signal_v2_40_60.push_back(v2_signal);
            data.signal_v2_40_60_Errors.push_back(v2_error_signal);
            
            data.bg_v2_40_60.push_back(v2_bg);
            data.bg_v2_40_60_Errors.push_back(v2_error_bg);
            
        } else if (index >= 6 && index <= 11) {
            data.corrected_v2_20_40.push_back(v2_corrected);
            data.corrected_v2_20_40_Errors.push_back(v2_error_corrected);
            
            data.corrected_v2_20_40_type4.push_back(v2_corrected_type4);
            data.corrected_v2_20_40_Errors_type4.push_back(v2_error_corrected_type4);
            
            data.signal_v2_20_40.push_back(v2_signal);
            data.signal_v2_20_40_Errors.push_back(v2_error_signal);
            
            data.bg_v2_20_40.push_back(v2_bg);
            data.bg_v2_20_40_Errors.push_back(v2_error_bg);
            

        } else if (index >= 12 && index <= 17) {
            data.corrected_v2_0_20.push_back(v2_corrected);
            data.corrected_v2_0_20_Errors.push_back(v2_error_corrected);
            
            data.corrected_v2_0_20_type4.push_back(v2_corrected_type4);
            data.corrected_v2_0_20_Errors_type4.push_back(v2_error_corrected_type4);
            
            data.signal_v2_0_20.push_back(v2_signal);
            data.signal_v2_0_20_Errors.push_back(v2_error_signal);
            
            data.bg_v2_0_20.push_back(v2_bg);
            data.bg_v2_0_20_Errors.push_back(v2_error_bg);
            
        }
    }
    file.close();
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
std::vector<double> CalculatePHENIXsystematicError(const std::vector<double>& data, double sigma_RP, double v2_raw_syst) {
    std::vector<double> systematicErrors;
    for (const auto& value : data) {
        double sigma_RP_error = value * sigma_RP;  // Use the multiplier directly dependent on centrality
        double v2_raw_error = value * v2_raw_syst;  // Use the multiplier directly dependent on centrality
        double total_systematic_error = std::sqrt(sigma_RP_error * sigma_RP_error + v2_raw_error * v2_raw_error);
        systematicErrors.push_back(total_systematic_error);
    }
    return systematicErrors;
}

// Function to create and return a TGraphErrors pointer
TGraphErrors* CreateGraph(const std::vector<double>& ptCenters, const std::vector<double>& values, const std::vector<double>& errors) {
    return new TGraphErrors(ptCenters.size(), &ptCenters[0], &values[0], nullptr, &errors[0]);
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
void plotting(const Data& data1) {
    double minYaxis_correctedComparisons = -0.35;
    double maxYaxis_correctedComparisons = 0.5;
    double TLatexSize = 0.025;

    int markerColor_1 = kBlack;
    int markerStyle_1 = 20;
    double markerSize_1 = 1.0;
    
    TGraphErrors* corrected_v2_0_20_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_0_20, data1.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_20_40, data1.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_40_60, data1.corrected_v2_40_60_Errors);
    
    TGraphErrors* sys_v2_0_20_graph_1 = CreateSystematicGraph(ptCenters, data1.corrected_v2_0_20, data1.corrected_v2_0_20_Errors, data1.stat_uncertainties_0_20);
    TGraphErrors* sys_v2_20_40_graph_1 = CreateSystematicGraph(ptCenters, data1.corrected_v2_20_40, data1.corrected_v2_20_40_Errors, data1.stat_uncertainties_20_40);
    TGraphErrors* sys_v2_40_60_graph_1 = CreateSystematicGraph(ptCenters, data1.corrected_v2_40_60, data1.corrected_v2_40_60_Errors, data1.stat_uncertainties_40_60);
    
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
    sys_v2_0_20_graph_1->SetMinimum(minYaxis_correctedComparisons); // Set the minimum y value
    sys_v2_0_20_graph_1->SetMaximum(maxYaxis_correctedComparisons); // Set the maximum y value

    DrawZeroLine(c_Overlay_1_finalResults);
    TLegend *leg0_20_FinalResults = new TLegend(0.14,.19,0.34,.44);
    leg0_20_FinalResults->SetTextSize(0.04);
    leg0_20_FinalResults->SetFillStyle(0);
    leg0_20_FinalResults->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg0_20_FinalResults->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg0_20_FinalResults->AddEntry("","0-20% Centrality","");
    leg0_20_FinalResults->AddEntry("","Build: ana412, Production: 2023p015","");
    leg0_20_FinalResults->Draw("same");
    
    
    c_Overlay_1_finalResults->SaveAs((BasePlotOutputPath + "/Corrected_v2_0_20.png").c_str());
    
    
    

    TCanvas *c_Overlay_2_finalResults = new TCanvas("c_Overlay_2_finalResults", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 20-40% Centrality", 800, 600);
    sys_v2_20_40_graph_1->Draw("A2");
    sys_v2_20_40_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 20-40% Centrality");
    sys_v2_20_40_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    sys_v2_20_40_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    sys_v2_20_40_graph_1->SetMinimum(minYaxis_correctedComparisons); // Set the minimum y value
    sys_v2_20_40_graph_1->SetMaximum(maxYaxis_correctedComparisons); // Set the maximum y value
    
    corrected_v2_20_40_graph_1->Draw("P SAME");
    
    DrawZeroLine(c_Overlay_2_finalResults);
    TLegend *leg20_40_FinalResults = new TLegend(0.14,.19,0.34,.44);
    leg20_40_FinalResults->SetTextSize(0.04);
    leg20_40_FinalResults->SetFillStyle(0);
    leg20_40_FinalResults->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg20_40_FinalResults->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg20_40_FinalResults->AddEntry("","20-40% Centrality","");
    leg20_40_FinalResults->AddEntry("","Build: ana412, Production: 2023p015","");
    leg20_40_FinalResults->Draw("same");
    
    
    
    c_Overlay_2_finalResults->SaveAs((BasePlotOutputPath + "/Corrected_v2_20_40.png").c_str());
    
    
    
    

    TCanvas *c_Overlay_3_finalResults = new TCanvas("c_Overlay_3_finalResults", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    sys_v2_40_60_graph_1->Draw("A2");
    sys_v2_40_60_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality");
    sys_v2_40_60_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    sys_v2_40_60_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    sys_v2_40_60_graph_1->SetMinimum(minYaxis_correctedComparisons); // Set the minimum y value
    sys_v2_40_60_graph_1->SetMaximum(maxYaxis_correctedComparisons); // Set the maximum y value
    
    corrected_v2_40_60_graph_1->Draw("P SAME");
    
    DrawZeroLine(c_Overlay_3_finalResults);
    TLegend *leg40_60_FinalResults = new TLegend(0.14,.19,0.34,.44);
    leg40_60_FinalResults->SetFillStyle(0);
    leg40_60_FinalResults->SetTextSize(0.04);
    leg40_60_FinalResults->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg40_60_FinalResults->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg40_60_FinalResults->AddEntry("","40-60% Centrality","");
    leg40_60_FinalResults->AddEntry("","Build: ana412, Production: 2023p015","");
    leg40_60_FinalResults->Draw("same");
    

    c_Overlay_3_finalResults->SaveAs((BasePlotOutputPath + "/Corrected_v2_40_60.png").c_str());
    
    

    
    
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
    
    std::vector<double> sysErrors_0_20_PHENIX = CalculatePHENIXsystematicError(combinedData.v2_0_20_PHENIX, 0.1, 0.1);
    TGraphErrors* sys_graph_0_20_PHENIX = CreateSystematicGraph(ptCenters, combinedData.v2_0_20_PHENIX, combinedData.v2_0_20_Errors_PHENIX, sysErrors_0_20_PHENIX);
    sys_graph_0_20_PHENIX->SetFillColor(kBlue); // Gray color
    sys_graph_0_20_PHENIX->SetFillStyle(3001); // Semi-transparent fill

    
    
    TGraphErrors* graph_20_40_PHENIXdataAveraged = CreateGraph(ptCenters, combinedData.v2_20_40_PHENIX, combinedData.v2_20_40_Errors_PHENIX);
    graph_20_40_PHENIXdataAveraged->SetMarkerColor(kBlue);
    graph_20_40_PHENIXdataAveraged->SetLineColor(kBlue);
    graph_20_40_PHENIXdataAveraged->SetMarkerStyle(20);
    graph_20_40_PHENIXdataAveraged->SetMarkerSize(1.0);
    
    std::vector<double> sysErrors_20_40_PHENIX = CalculatePHENIXsystematicError(combinedData.v2_20_40_PHENIX, 0.05, 0.03);
    TGraphErrors* sys_graph_20_40_PHENIX = CreateSystematicGraph(ptCenters, combinedData.v2_20_40_PHENIX, combinedData.v2_20_40_Errors_PHENIX, sysErrors_20_40_PHENIX);
    sys_graph_20_40_PHENIX->SetFillColor(kBlue); // Gray color
    sys_graph_20_40_PHENIX->SetFillStyle(3001); // Semi-transparent fill

    TGraphErrors* graph_40_60_PHENIXdataAveraged = CreateGraph(ptCenters, combinedData.v2_40_60_PHENIX, combinedData.v2_40_60_Errors_PHENIX);
    graph_40_60_PHENIXdataAveraged->SetMarkerColor(kBlue);
    graph_40_60_PHENIXdataAveraged->SetLineColor(kBlue);
    graph_40_60_PHENIXdataAveraged->SetMarkerStyle(20);
    graph_40_60_PHENIXdataAveraged->SetMarkerSize(1.0);
    
    std::vector<double> sysErrors_40_60_PHENIX = CalculatePHENIXsystematicError(combinedData.v2_40_60_PHENIX, 0.1, 0.03);
    TGraphErrors* sys_graph_40_60_PHENIX = CreateSystematicGraph(ptCenters, combinedData.v2_40_60_PHENIX, combinedData.v2_40_60_Errors_PHENIX, sysErrors_40_60_PHENIX);
    sys_graph_40_60_PHENIX->SetFillColor(kBlue); // Gray color
    sys_graph_40_60_PHENIX->SetFillStyle(3001); // Semi-transparent fill
    
    TCanvas *c_Overlay_ResultsWithPHENIX_0_20 = new TCanvas("c_Overlay_ResultsWithPHENIX_0_20", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        sys_v2_0_20_graph_1->SetPoint(i, ptCenters[i] - .08, data1.corrected_v2_0_20[i]);
        corrected_v2_0_20_graph_1->SetPoint(i, ptCenters[i] - .08, data1.corrected_v2_0_20[i]);
    }
    sys_v2_0_20_graph_1->Draw("A2"); // Draw systematic errors first as a shaded area
    corrected_v2_0_20_graph_1->Draw("P SAME"); // Then draw the statistical errors on top
    sys_graph_0_20_PHENIX->Draw("2 SAME");  // Draw PHENIX systematic errors
    
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_0_20_PHENIXdataAveraged->SetPoint(i, ptCenters[i] + .08, combinedData.v2_0_20_PHENIX[i]);
        sys_graph_0_20_PHENIX->SetPoint(i, ptCenters[i] + .08, combinedData.v2_0_20_PHENIX[i]);
    }
    graph_0_20_PHENIXdataAveraged->Draw("P SAME");
    
    sys_v2_0_20_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 0-20% Centrality");
    sys_v2_0_20_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    sys_v2_0_20_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    sys_v2_0_20_graph_1->SetMinimum(minYaxis_correctedComparisons); // Set the minimum y value
    sys_v2_0_20_graph_1->SetMaximum(maxYaxis_correctedComparisons); // Set the maximum y value

    DrawZeroLine(c_Overlay_ResultsWithPHENIX_0_20);
    TLegend *leg1_PHENIXoverlay = new TLegend(0.14,.19,0.34,.39);
    leg1_PHENIXoverlay->SetFillStyle(0);
    leg1_PHENIXoverlay->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg1_PHENIXoverlay->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg1_PHENIXoverlay->AddEntry("","0-20% Centrality","");
    leg1_PHENIXoverlay->Draw("same");
    
    TLegend *leg_uncertainty_0_20_forPHENIXoverlay = new TLegend(0.67,.19,0.87,.4);
    leg_uncertainty_0_20_forPHENIXoverlay->SetTextSize(0.029);
    leg_uncertainty_0_20_forPHENIXoverlay->AddEntry(sys_graph_0_20_PHENIX, "PHENIX", "pef");
    leg_uncertainty_0_20_forPHENIXoverlay->AddEntry(sys_v2_0_20_graph_1, "sPHENIX", "pef"); // "f" for a filled object
    leg_uncertainty_0_20_forPHENIXoverlay->Draw("same");
    
    
    c_Overlay_ResultsWithPHENIX_0_20->SaveAs((BasePlotOutputPath + "/Final_v2_withSyst_0_20_OverlayedWithPHENIX.png").c_str());
    
    
    
    
    TCanvas *c_Overlay_ResultsWithPHENIX_20_40 = new TCanvas("c_Overlay_ResultsWithPHENIX_20_40", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        sys_v2_20_40_graph_1->SetPoint(i, ptCenters[i] - .08, data1.corrected_v2_20_40[i]);
        corrected_v2_20_40_graph_1->SetPoint(i, ptCenters[i] - .08, data1.corrected_v2_20_40[i]);
    }
    sys_v2_20_40_graph_1->Draw("A2"); // Draw systematic errors first as a shaded area
    corrected_v2_20_40_graph_1->Draw("P SAME"); // Then draw the statistical errors on top
    sys_graph_20_40_PHENIX->Draw("2 SAME");  // Draw PHENIX systematic errors
    
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_20_40_PHENIXdataAveraged->SetPoint(i, ptCenters[i] + .08, combinedData.v2_20_40_PHENIX[i]);
        sys_graph_20_40_PHENIX->SetPoint(i, ptCenters[i] + .08, combinedData.v2_20_40_PHENIX[i]);
    }
    graph_20_40_PHENIXdataAveraged->Draw("P SAME");
    
    sys_v2_20_40_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 0-20% Centrality");
    sys_v2_20_40_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    sys_v2_20_40_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    sys_v2_20_40_graph_1->SetMinimum(minYaxis_correctedComparisons); // Set the minimum y value
    sys_v2_20_40_graph_1->SetMaximum(maxYaxis_correctedComparisons); // Set the maximum y value

    DrawZeroLine(c_Overlay_ResultsWithPHENIX_20_40);
    TLegend *leg2_PHENIXoverlay = new TLegend(0.14,.19,0.34,.39);
    leg2_PHENIXoverlay->SetFillStyle(0);
    leg2_PHENIXoverlay->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg2_PHENIXoverlay->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg2_PHENIXoverlay->AddEntry("","0-20% Centrality","");
    leg2_PHENIXoverlay->Draw("same");
    
    TLegend *leg_uncertainty_20_40_forPHENIXoverlay = new TLegend(0.8,.19,0.92,.4);
    leg_uncertainty_20_40_forPHENIXoverlay->SetTextSize(0.029);
    leg_uncertainty_20_40_forPHENIXoverlay->AddEntry(sys_graph_20_40_PHENIX, "PHENIX", "pef");
    leg_uncertainty_20_40_forPHENIXoverlay->AddEntry(sys_v2_20_40_graph_1, "sPHENIX", "pef"); // "f" for a filled object
    leg_uncertainty_20_40_forPHENIXoverlay->Draw("same");
    
    
    c_Overlay_ResultsWithPHENIX_20_40->SaveAs((BasePlotOutputPath + "/Final_v2_withSyst_20_40_OverlayedWithPHENIX.png").c_str());
    
    
    
    
    TCanvas *c_Overlay_ResultsWithPHENIX_40_60 = new TCanvas("c_Overlay_ResultsWithPHENIX_40_60", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        sys_v2_40_60_graph_1->SetPoint(i, ptCenters[i] - .08, data1.corrected_v2_40_60[i]);
        corrected_v2_40_60_graph_1->SetPoint(i, ptCenters[i] - .08, data1.corrected_v2_40_60[i]);
    }
    sys_v2_40_60_graph_1->Draw("A2"); // Draw systematic errors first as a shaded area
    corrected_v2_40_60_graph_1->Draw("P SAME"); // Then draw the statistical errors on top
    sys_graph_40_60_PHENIX->Draw("2 SAME");  // Draw PHENIX systematic errors
    
    for (int i = 0; i < ptCenters.size(); ++i) {
        graph_40_60_PHENIXdataAveraged->SetPoint(i, ptCenters[i] + .08, combinedData.v2_40_60_PHENIX[i]);
        sys_graph_40_60_PHENIX->SetPoint(i, ptCenters[i] + .08, combinedData.v2_40_60_PHENIX[i]);
    }
    graph_40_60_PHENIXdataAveraged->Draw("P SAME");
    
    sys_v2_40_60_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 0-20% Centrality");
    sys_v2_40_60_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    sys_v2_40_60_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    sys_v2_40_60_graph_1->SetMinimum(minYaxis_correctedComparisons); // Set the minimum y value
    sys_v2_40_60_graph_1->SetMaximum(maxYaxis_correctedComparisons); // Set the maximum y value

    DrawZeroLine(c_Overlay_ResultsWithPHENIX_40_60);
    TLegend *leg3_PHENIXoverlay = new TLegend(0.14,.19,0.34,.39);
    leg3_PHENIXoverlay->SetFillStyle(0);
    leg3_PHENIXoverlay->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg3_PHENIXoverlay->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg3_PHENIXoverlay->AddEntry("","0-20% Centrality","");
    leg3_PHENIXoverlay->Draw("same");
    
    TLegend *leg_uncertainty_40_60_forPHENIXoverlay = new TLegend(0.67,.19,0.87,.4);
    leg_uncertainty_40_60_forPHENIXoverlay->SetTextSize(0.029);
    leg_uncertainty_40_60_forPHENIXoverlay->AddEntry(sys_graph_40_60_PHENIX, "PHENIX", "pef");
    leg_uncertainty_40_60_forPHENIXoverlay->AddEntry(sys_v2_40_60_graph_1, "sPHENIX", "pef"); // "f" for a filled object
    leg_uncertainty_40_60_forPHENIXoverlay->Draw("same");
    
    
    c_Overlay_ResultsWithPHENIX_40_60->SaveAs((BasePlotOutputPath + "/Final_v2_withSyst_40_60_OverlayedWithPHENIX.png").c_str());
}


double FindMaxValueFromPlottedData(const std::vector<double>& values) {
    double maxVal = *std::max_element(values.begin(), values.end());
    return maxVal;
}

void AdjustFrameYAxis(TCanvas* canvas, TH1F* frame, double maxYValue) {
    if (frame) {
        frame->GetYaxis()->SetRangeUser(0, maxYValue);
        canvas->Modified();
        canvas->Update();
    }
}

void plotting_Systematics(const Data& data1) {
    double barWidth = 0.05;
    int colors[] = {kRed, kBlue, kGreen+1, kGray+1};
    const double EPSILON = 1e-6;
    
    
    std::vector<TH1F*> histograms_0_20_RelativeUncertainty;
    std::vector<double> collect_Values_0_20_Relative;
    
    TCanvas *c_Overlay_0_20_Systematics_RelativeUncertainty = new TCanvas("c_Overlay_0_20_Systematics_RelativeUncertainty", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    TH1F *frame_0_20_RelativeUncertainty = c_Overlay_0_20_Systematics_RelativeUncertainty->DrawFrame(2.0, 0, 5.0, 0.7);
    frame_0_20_RelativeUncertainty->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_0_20_RelativeUncertainty->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        double x = ptCenters[i] - (2.5 * barWidth);
        
        std::vector<double> values_RelativeUncertainty_0_20 = {
            data1.quad_sum_EMCal_syst_0_20[i],
            data1.signal_window_syst_0_20[i],
            data1.background_window_syst_0_20[i],
            data1.unWeighted_stat_uncertainties_0_20[i]
        };
        collect_Values_0_20_Relative.insert(collect_Values_0_20_Relative.end(), values_RelativeUncertainty_0_20.begin(), values_RelativeUncertainty_0_20.end());
        
        for (size_t j = 0; j < values_RelativeUncertainty_0_20.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_0_20_RelativeUncertainty = new TH1F(Form("h_0_20_RelativeUncertainty_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_0_20_RelativeUncertainty->SetBinContent(1, values_RelativeUncertainty_0_20[j]);
            h_0_20_RelativeUncertainty->SetFillColor(colors[j]);
            h_0_20_RelativeUncertainty->Draw("SAME");
            histograms_0_20_RelativeUncertainty.push_back(h_0_20_RelativeUncertainty);
        }
        
    }
    double maxYValue_0_20_Relative = FindMaxValueFromPlottedData(collect_Values_0_20_Relative);
    AdjustFrameYAxis(c_Overlay_0_20_Systematics_RelativeUncertainty, frame_0_20_RelativeUncertainty, maxYValue_0_20_Relative * 1.05);
    
    TLegend *legend_0_20_bars_RelativeUncertainty = new TLegend(0.2, 0.72, 0.4, 0.92);
    legend_0_20_bars_RelativeUncertainty->SetTextSize(0.03);
    legend_0_20_bars_RelativeUncertainty->SetHeader("#bf{Relative Systematics, 0-20%}", "L");
    legend_0_20_bars_RelativeUncertainty->AddEntry(histograms_0_20_RelativeUncertainty[0], "EMCal Scale", "f");
    legend_0_20_bars_RelativeUncertainty->AddEntry(histograms_0_20_RelativeUncertainty[1], "Signal Window", "f");
    legend_0_20_bars_RelativeUncertainty->AddEntry(histograms_0_20_RelativeUncertainty[2], "Background Window", "f");
    legend_0_20_bars_RelativeUncertainty->AddEntry(histograms_0_20_RelativeUncertainty[3], "Final Systematic Uncertainty", "f");
    legend_0_20_bars_RelativeUncertainty->Draw();
    
    
    
    c_Overlay_0_20_Systematics_RelativeUncertainty->SaveAs((BasePlotOutputPath + "/Systematics_Analysis-v2-Checks/RelativeSystUncertainty_Overlay_0_20.png").c_str());
    
    
    
    
    
    std::vector<TH1F*> histograms_0_20_RelativeUncertainty_ZoomedIn;
    std::vector<double> collect_Values_0_20_Relative_ZoomedIn;
    
    TCanvas *c_Overlay_0_20_Systematics_RelativeUncertainty_ZoomedIn = new TCanvas("c_Overlay_0_20_Systematics_RelativeUncertainty_ZoomedIn", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    TH1F *frame_0_20_RelativeUncertainty_ZoomedIn = c_Overlay_0_20_Systematics_RelativeUncertainty_ZoomedIn->DrawFrame(2.0, 0, 5.0, 0.7);
    frame_0_20_RelativeUncertainty_ZoomedIn->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_0_20_RelativeUncertainty_ZoomedIn->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        
        if (std::abs(ptCenters[i] - 3.75) < EPSILON) {
            continue;
        }
        if (std::abs(ptCenters[i] - 4.25) < EPSILON) {
            continue;
        }
        double x = ptCenters[i] - (2.5 * barWidth);
        
        std::vector<double> values_RelativeUncertainty_0_20_ZoomedIn = {
            data1.quad_sum_EMCal_syst_0_20[i],
            data1.signal_window_syst_0_20[i],
            data1.background_window_syst_0_20[i],
            data1.unWeighted_stat_uncertainties_0_20[i]
        };
        collect_Values_0_20_Relative_ZoomedIn.insert(collect_Values_0_20_Relative_ZoomedIn.end(), values_RelativeUncertainty_0_20_ZoomedIn.begin(), values_RelativeUncertainty_0_20_ZoomedIn.end());
        
        for (size_t j = 0; j < values_RelativeUncertainty_0_20_ZoomedIn.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_0_20_RelativeUncertainty_ZoomedIn = new TH1F(Form("h_0_20_RelativeUncertainty_ZoomedIn_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_0_20_RelativeUncertainty_ZoomedIn->SetBinContent(1, values_RelativeUncertainty_0_20_ZoomedIn[j]);
            h_0_20_RelativeUncertainty_ZoomedIn->SetFillColor(colors[j]);
            h_0_20_RelativeUncertainty_ZoomedIn->Draw("SAME");
            histograms_0_20_RelativeUncertainty_ZoomedIn.push_back(h_0_20_RelativeUncertainty_ZoomedIn);
        }
        
    }
    double maxYValue_0_20_Relative_ZoomedIn = FindMaxValueFromPlottedData(collect_Values_0_20_Relative_ZoomedIn);
    AdjustFrameYAxis(c_Overlay_0_20_Systematics_RelativeUncertainty_ZoomedIn, frame_0_20_RelativeUncertainty_ZoomedIn, maxYValue_0_20_Relative_ZoomedIn * 1.05);
    
    TLegend *legend_0_20_bars_RelativeUncertainty_ZoomedIn = new TLegend(0.55, 0.72, 0.75, 0.92);
    legend_0_20_bars_RelativeUncertainty_ZoomedIn->SetTextSize(0.028);
    legend_0_20_bars_RelativeUncertainty_ZoomedIn->SetHeader("#bf{Relative Systematics, 0-20%}", "L");
    legend_0_20_bars_RelativeUncertainty_ZoomedIn->AddEntry(histograms_0_20_RelativeUncertainty_ZoomedIn[0], "EMCal Scale", "f");
    legend_0_20_bars_RelativeUncertainty_ZoomedIn->AddEntry(histograms_0_20_RelativeUncertainty_ZoomedIn[1], "Signal Window", "f");
    legend_0_20_bars_RelativeUncertainty_ZoomedIn->AddEntry(histograms_0_20_RelativeUncertainty_ZoomedIn[2], "Background Window", "f");
    legend_0_20_bars_RelativeUncertainty_ZoomedIn->AddEntry(histograms_0_20_RelativeUncertainty_ZoomedIn[3], "Final Systematic Uncertainty", "f");
    legend_0_20_bars_RelativeUncertainty_ZoomedIn->Draw();
    

    
    c_Overlay_0_20_Systematics_RelativeUncertainty_ZoomedIn->SaveAs((BasePlotOutputPath + "/Systematics_Analysis-v2-Checks/RelativeSystUncertainty_Overlay_0_20_Zoomed_In.png").c_str());
    
    
    
    
    std::vector<TH1F*> histograms_20_40_RelativeUncertainty;
    std::vector<double> collect_Values_20_40_Relative;
    
    TCanvas *c_Overlay_20_40_Systematics_RelativeUncertainty = new TCanvas("c_Overlay_20_40_Systematics_RelativeUncertainty", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 20-40% Centrality", 800, 600);
    TH1F *frame_20_40_RelativeUncertainty = c_Overlay_20_40_Systematics_RelativeUncertainty->DrawFrame(2.0, 0, 5.0, 0.7);
    frame_20_40_RelativeUncertainty->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_20_40_RelativeUncertainty->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        double x = ptCenters[i] - (2.5 * barWidth);
        
        std::vector<double> values_RelativeUncertainty_20_40 = {
            data1.quad_sum_EMCal_syst_20_40[i],
            data1.signal_window_syst_20_40[i],
            data1.background_window_syst_20_40[i],
            data1.unWeighted_stat_uncertainties_20_40[i]
        };
        collect_Values_20_40_Relative.insert(collect_Values_20_40_Relative.end(), values_RelativeUncertainty_20_40.begin(), values_RelativeUncertainty_20_40.end());
        
        for (size_t j = 0; j < values_RelativeUncertainty_20_40.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_20_40_RelativeUncertainty = new TH1F(Form("h_20_40_RelativeUncertainty_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_20_40_RelativeUncertainty->SetBinContent(1, values_RelativeUncertainty_20_40[j]);
            h_20_40_RelativeUncertainty->SetFillColor(colors[j]);
            h_20_40_RelativeUncertainty->Draw("SAME");
            histograms_20_40_RelativeUncertainty.push_back(h_20_40_RelativeUncertainty);
        }
        
    }
    double maxYValue_20_40_Relative = FindMaxValueFromPlottedData(collect_Values_20_40_Relative);
    AdjustFrameYAxis(c_Overlay_20_40_Systematics_RelativeUncertainty, frame_20_40_RelativeUncertainty, maxYValue_20_40_Relative * 1.05);
    
    TLegend *legend_20_40_bars_RelativeUncertainty = new TLegend(0.2, 0.72, 0.4, 0.92);
    legend_20_40_bars_RelativeUncertainty->SetTextSize(0.03);
    legend_20_40_bars_RelativeUncertainty->SetHeader("#bf{Relative Systematics, 20-40%}", "L");
    legend_20_40_bars_RelativeUncertainty->AddEntry(histograms_20_40_RelativeUncertainty[0], "EMCal Scale", "f");
    legend_20_40_bars_RelativeUncertainty->AddEntry(histograms_20_40_RelativeUncertainty[1], "Signal Window", "f");
    legend_20_40_bars_RelativeUncertainty->AddEntry(histograms_20_40_RelativeUncertainty[2], "Background Window", "f");
    legend_20_40_bars_RelativeUncertainty->AddEntry(histograms_20_40_RelativeUncertainty[3], "Final Systematic Uncertainty", "f");
    legend_20_40_bars_RelativeUncertainty->Draw();
    

    c_Overlay_20_40_Systematics_RelativeUncertainty->Modified();
    c_Overlay_20_40_Systematics_RelativeUncertainty->Update();
    
    
    c_Overlay_20_40_Systematics_RelativeUncertainty->SaveAs((BasePlotOutputPath + "/Systematics_Analysis-v2-Checks/RelativeSystUncertainty_Overlay_20_40.png").c_str());
    
    
    
    
    
    std::vector<TH1F*> histograms_20_40_RelativeUncertainty_ZoomedIn;
    std::vector<double> collect_Values_20_40_Relative_ZoomedIn;
    
    TCanvas *c_Overlay_20_40_Systematics_RelativeUncertainty_ZoomedIn = new TCanvas("c_Overlay_20_40_Systematics_RelativeUncertainty_ZoomedIn", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 20-40% Centrality", 800, 600);
    TH1F *frame_20_40_RelativeUncertainty_ZoomedIn = c_Overlay_20_40_Systematics_RelativeUncertainty_ZoomedIn->DrawFrame(2.0, 0, 5.0, 0.7);
    frame_20_40_RelativeUncertainty_ZoomedIn->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_20_40_RelativeUncertainty_ZoomedIn->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        
        if (std::abs(ptCenters[i] - 4.25) < EPSILON) {
            continue;
        }
        double x = ptCenters[i] - (2.5 * barWidth);
        
        std::vector<double> values_RelativeUncertainty_20_40_ZoomedIn = {
            data1.quad_sum_EMCal_syst_20_40[i],
            data1.signal_window_syst_20_40[i],
            data1.background_window_syst_20_40[i],
            data1.unWeighted_stat_uncertainties_20_40[i]
        };
        collect_Values_20_40_Relative_ZoomedIn.insert(collect_Values_20_40_Relative_ZoomedIn.end(), values_RelativeUncertainty_20_40_ZoomedIn.begin(), values_RelativeUncertainty_20_40_ZoomedIn.end());
        
        for (size_t j = 0; j < values_RelativeUncertainty_20_40_ZoomedIn.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_20_40_RelativeUncertainty_ZoomedIn = new TH1F(Form("h_20_40_RelativeUncertainty_ZoomedIn_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_20_40_RelativeUncertainty_ZoomedIn->SetBinContent(1, values_RelativeUncertainty_20_40_ZoomedIn[j]);
            h_20_40_RelativeUncertainty_ZoomedIn->SetFillColor(colors[j]);
            h_20_40_RelativeUncertainty_ZoomedIn->Draw("SAME");
            histograms_20_40_RelativeUncertainty_ZoomedIn.push_back(h_20_40_RelativeUncertainty_ZoomedIn);
        }
        
    }
    double maxYValue_20_40_Relative_ZoomedIn = FindMaxValueFromPlottedData(collect_Values_20_40_Relative_ZoomedIn);
    AdjustFrameYAxis(c_Overlay_20_40_Systematics_RelativeUncertainty_ZoomedIn, frame_20_40_RelativeUncertainty_ZoomedIn, maxYValue_20_40_Relative_ZoomedIn * 1.05);
    
    TLegend *legend_20_40_bars_RelativeUncertainty_ZoomedIn = new TLegend(0.65, 0.72, 0.8, 0.9);
    legend_20_40_bars_RelativeUncertainty_ZoomedIn->SetTextSize(0.027);
    legend_20_40_bars_RelativeUncertainty_ZoomedIn->SetHeader("#bf{Relative Systematics, 20-40%}", "L");
    legend_20_40_bars_RelativeUncertainty_ZoomedIn->AddEntry(histograms_20_40_RelativeUncertainty_ZoomedIn[0], "EMCal Scale", "f");
    legend_20_40_bars_RelativeUncertainty_ZoomedIn->AddEntry(histograms_20_40_RelativeUncertainty_ZoomedIn[1], "Signal Window", "f");
    legend_20_40_bars_RelativeUncertainty_ZoomedIn->AddEntry(histograms_20_40_RelativeUncertainty_ZoomedIn[2], "Background Window", "f");
    legend_20_40_bars_RelativeUncertainty_ZoomedIn->AddEntry(histograms_20_40_RelativeUncertainty_ZoomedIn[3], "Final Systematic Uncertainty", "f");
    legend_20_40_bars_RelativeUncertainty_ZoomedIn->Draw();

    c_Overlay_20_40_Systematics_RelativeUncertainty_ZoomedIn->Modified();
    c_Overlay_20_40_Systematics_RelativeUncertainty_ZoomedIn->Update();
    
    c_Overlay_20_40_Systematics_RelativeUncertainty_ZoomedIn->SaveAs((BasePlotOutputPath + "/Systematics_Analysis-v2-Checks/RelativeSystUncertainty_Overlay_20_40_Zoomed_In.png").c_str());
    
    
    
    
    std::vector<TH1F*> histograms_40_60_RelativeUncertainty;
    std::vector<double> collect_Values_40_60_Relative;
    
    TCanvas *c_Overlay_40_60_Systematics_RelativeUncertainty = new TCanvas("c_Overlay_40_60_Systematics_RelativeUncertainty", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    TH1F *frame_40_60_RelativeUncertainty = c_Overlay_40_60_Systematics_RelativeUncertainty->DrawFrame(2.0, 0, 5.0, 0.7);
    frame_40_60_RelativeUncertainty->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_40_60_RelativeUncertainty->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        double x = ptCenters[i] - (2.5 * barWidth);
        
        std::vector<double> values_RelativeUncertainty_40_60 = {
            data1.quad_sum_EMCal_syst_40_60[i],
            data1.signal_window_syst_40_60[i],
            data1.background_window_syst_40_60[i],
            data1.unWeighted_stat_uncertainties_40_60[i]
        };
        collect_Values_40_60_Relative.insert(collect_Values_40_60_Relative.end(), values_RelativeUncertainty_40_60.begin(), values_RelativeUncertainty_40_60.end());
        
        for (size_t j = 0; j < values_RelativeUncertainty_40_60.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_40_60_RelativeUncertainty = new TH1F(Form("h_40_60_RelativeUncertainty_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_40_60_RelativeUncertainty->SetBinContent(1, values_RelativeUncertainty_40_60[j]);
            h_40_60_RelativeUncertainty->SetFillColor(colors[j]);
            h_40_60_RelativeUncertainty->Draw("SAME");
            histograms_40_60_RelativeUncertainty.push_back(h_40_60_RelativeUncertainty);
        }
        
    }
    double maxYValue_40_60_Relative = FindMaxValueFromPlottedData(collect_Values_40_60_Relative);
    AdjustFrameYAxis(c_Overlay_40_60_Systematics_RelativeUncertainty, frame_40_60_RelativeUncertainty, maxYValue_40_60_Relative * 1.05);
    
    TLegend *legend_40_60_bars_RelativeUncertainty = new TLegend(0.2, 0.72, 0.4, 0.92);
    legend_40_60_bars_RelativeUncertainty->SetTextSize(0.03);
    legend_40_60_bars_RelativeUncertainty->SetHeader("#bf{Relative Systematics, 40-60%}", "L");
    legend_40_60_bars_RelativeUncertainty->AddEntry(histograms_40_60_RelativeUncertainty[0], "EMCal Scale", "f");
    legend_40_60_bars_RelativeUncertainty->AddEntry(histograms_40_60_RelativeUncertainty[1], "Signal Window", "f");
    legend_40_60_bars_RelativeUncertainty->AddEntry(histograms_40_60_RelativeUncertainty[2], "Background Window", "f");
    legend_40_60_bars_RelativeUncertainty->AddEntry(histograms_40_60_RelativeUncertainty[3], "Final Systematic Uncertainty", "f");
    legend_40_60_bars_RelativeUncertainty->Draw();
    

    c_Overlay_40_60_Systematics_RelativeUncertainty->Modified();
    c_Overlay_40_60_Systematics_RelativeUncertainty->Update();
    
    
    c_Overlay_40_60_Systematics_RelativeUncertainty->SaveAs((BasePlotOutputPath + "/Systematics_Analysis-v2-Checks/RelativeSystUncertainty_Overlay_40_60.png").c_str());
    
    
    
    
    
    std::vector<TH1F*> histograms_40_60_RelativeUncertainty_ZoomedIn;
    std::vector<double> collect_Values_40_60_Relative_ZoomedIn;
    
    TCanvas *c_Overlay_40_60_Systematics_RelativeUncertainty_ZoomedIn = new TCanvas("c_Overlay_40_60_Systematics_RelativeUncertainty_ZoomedIn", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    TH1F *frame_40_60_RelativeUncertainty_ZoomedIn = c_Overlay_40_60_Systematics_RelativeUncertainty_ZoomedIn->DrawFrame(2.0, 0, 5.0, 0.7);
    frame_40_60_RelativeUncertainty_ZoomedIn->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_40_60_RelativeUncertainty_ZoomedIn->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        
        if (std::abs(ptCenters[i] - 4.75) < EPSILON) {
            continue;
        }

        double x = ptCenters[i] - (2.5 * barWidth);
        
        std::vector<double> values_RelativeUncertainty_40_60_ZoomedIn = {
            data1.quad_sum_EMCal_syst_40_60[i],
            data1.signal_window_syst_40_60[i],
            data1.background_window_syst_40_60[i],
            data1.unWeighted_stat_uncertainties_40_60[i]
        };
        collect_Values_40_60_Relative_ZoomedIn.insert(collect_Values_40_60_Relative_ZoomedIn.end(), values_RelativeUncertainty_40_60_ZoomedIn.begin(), values_RelativeUncertainty_40_60_ZoomedIn.end());
        
        for (size_t j = 0; j < values_RelativeUncertainty_40_60_ZoomedIn.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_40_60_RelativeUncertainty_ZoomedIn = new TH1F(Form("h_40_60_RelativeUncertainty_ZoomedIn_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_40_60_RelativeUncertainty_ZoomedIn->SetBinContent(1, values_RelativeUncertainty_40_60_ZoomedIn[j]);
            h_40_60_RelativeUncertainty_ZoomedIn->SetFillColor(colors[j]);
            h_40_60_RelativeUncertainty_ZoomedIn->Draw("SAME");
            histograms_40_60_RelativeUncertainty_ZoomedIn.push_back(h_40_60_RelativeUncertainty_ZoomedIn);
        }
        
    }
    double maxYValue_40_60_Relative_ZoomedIn = FindMaxValueFromPlottedData(collect_Values_40_60_Relative_ZoomedIn);
    AdjustFrameYAxis(c_Overlay_40_60_Systematics_RelativeUncertainty_ZoomedIn, frame_40_60_RelativeUncertainty_ZoomedIn, maxYValue_40_60_Relative_ZoomedIn * 1.05);
    
    TLegend *legend_40_60_bars_RelativeUncertainty_ZoomedIn = new TLegend(0.52, 0.73, 0.65, 0.92);
    legend_40_60_bars_RelativeUncertainty_ZoomedIn->SetTextSize(0.025);
    legend_40_60_bars_RelativeUncertainty_ZoomedIn->SetHeader("#bf{Relative Systematics, 40-60%}", "L");
    legend_40_60_bars_RelativeUncertainty_ZoomedIn->AddEntry(histograms_40_60_RelativeUncertainty_ZoomedIn[0], "EMCal Scale", "f");
    legend_40_60_bars_RelativeUncertainty_ZoomedIn->AddEntry(histograms_40_60_RelativeUncertainty_ZoomedIn[1], "Signal Window", "f");
    legend_40_60_bars_RelativeUncertainty_ZoomedIn->AddEntry(histograms_40_60_RelativeUncertainty_ZoomedIn[2], "Background Window", "f");
    legend_40_60_bars_RelativeUncertainty_ZoomedIn->AddEntry(histograms_40_60_RelativeUncertainty_ZoomedIn[3], "Final Systematic Uncertainty", "f");
    legend_40_60_bars_RelativeUncertainty_ZoomedIn->Draw();

    c_Overlay_40_60_Systematics_RelativeUncertainty_ZoomedIn->Modified();
    c_Overlay_40_60_Systematics_RelativeUncertainty_ZoomedIn->Update();
    
    c_Overlay_40_60_Systematics_RelativeUncertainty_ZoomedIn->SaveAs((BasePlotOutputPath + "/Systematics_Analysis-v2-Checks/RelativeSystUncertainty_Overlay_40_60_Zoomed_In.png").c_str());
    
    
    
    
    std::vector<TH1F*> histograms_0_20_AbsoluteUncertainty;
    std::vector<double> collect_Values_0_20_Absolute;
    
    TCanvas *c_Overlay_0_20_Systematics_AbsoluteUncertainty = new TCanvas("c_Overlay_0_20_Systematics_AbsoluteUncertainty", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    TH1F *frame_0_20_AbsoluteUncertainty = c_Overlay_0_20_Systematics_AbsoluteUncertainty->DrawFrame(2.0, 0, 5.0, 0.7);
    frame_0_20_AbsoluteUncertainty->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_0_20_AbsoluteUncertainty->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        double x = ptCenters[i] - (2.5 * barWidth);
        double v2_value = std::abs(data1.corrected_v2_0_20[i]);
        
        std::vector<double> values_AbsoluteUncertainty_0_20 = {
            data1.quad_sum_EMCal_syst_0_20[i] * v2_value,
            data1.signal_window_syst_0_20[i] * v2_value,
            data1.background_window_syst_0_20[i] * v2_value,
            data1.stat_uncertainties_0_20[i]
        };
        collect_Values_0_20_Absolute.insert(collect_Values_0_20_Absolute.end(), values_AbsoluteUncertainty_0_20.begin(), values_AbsoluteUncertainty_0_20.end());
        
        for (size_t j = 0; j < values_AbsoluteUncertainty_0_20.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_0_20_AbsoluteUncertainty = new TH1F(Form("h_0_20_AbsoluteUncertainty_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_0_20_AbsoluteUncertainty->SetBinContent(1, values_AbsoluteUncertainty_0_20[j]);
            h_0_20_AbsoluteUncertainty->SetFillColor(colors[j]);
            h_0_20_AbsoluteUncertainty->Draw("SAME");
            histograms_0_20_AbsoluteUncertainty.push_back(h_0_20_AbsoluteUncertainty);
        }
        
    }
    double maxYValue_0_20_Absolute = FindMaxValueFromPlottedData(collect_Values_0_20_Absolute);
    AdjustFrameYAxis(c_Overlay_0_20_Systematics_AbsoluteUncertainty, frame_0_20_AbsoluteUncertainty, maxYValue_0_20_Absolute * 1.05);
    
    TLegend *legend_0_20_bars_AbsoluteUncertainty = new TLegend(0.2, 0.72, 0.4, 0.92);
    legend_0_20_bars_AbsoluteUncertainty->SetTextSize(0.029);
    legend_0_20_bars_AbsoluteUncertainty->SetHeader("#bf{Absolute Systematics, 0-20%}", "L");
    legend_0_20_bars_AbsoluteUncertainty->AddEntry(histograms_0_20_AbsoluteUncertainty[0], "EMCal Scale", "f");
    legend_0_20_bars_AbsoluteUncertainty->AddEntry(histograms_0_20_AbsoluteUncertainty[1], "Signal Window", "f");
    legend_0_20_bars_AbsoluteUncertainty->AddEntry(histograms_0_20_AbsoluteUncertainty[2], "Background Window", "f");
    legend_0_20_bars_AbsoluteUncertainty->AddEntry(histograms_0_20_AbsoluteUncertainty[3], "Final Systematic Uncertainty", "f");
    legend_0_20_bars_AbsoluteUncertainty->Draw();
    

    c_Overlay_0_20_Systematics_AbsoluteUncertainty->Modified();
    c_Overlay_0_20_Systematics_AbsoluteUncertainty->Update();
    
    
    c_Overlay_0_20_Systematics_AbsoluteUncertainty->SaveAs((BasePlotOutputPath + "/Systematics_Analysis-v2-Checks/AbsoluteSystUncertainty_Overlay_0_20.png").c_str());
    
    
    
    std::vector<TH1F*> histograms_20_40_AbsoluteUncertainty;
    std::vector<double> collect_Values_20_40_Absolute;
    
    TCanvas *c_Overlay_20_40_Systematics_AbsoluteUncertainty = new TCanvas("c_Overlay_20_40_Systematics_AbsoluteUncertainty", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 20-40% Centrality", 800, 600);
    TH1F *frame_20_40_AbsoluteUncertainty = c_Overlay_20_40_Systematics_AbsoluteUncertainty->DrawFrame(2.0, 0, 5.0, 0.7);
    frame_20_40_AbsoluteUncertainty->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_20_40_AbsoluteUncertainty->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        double x = ptCenters[i] - (2.5 * barWidth);
        double v2_value = std::abs(data1.corrected_v2_20_40[i]);
        
        std::vector<double> values_AbsoluteUncertainty_20_40 = {
            data1.quad_sum_EMCal_syst_20_40[i] * v2_value,
            data1.signal_window_syst_20_40[i] * v2_value,
            data1.background_window_syst_20_40[i] * v2_value,
            data1.stat_uncertainties_20_40[i]
        };
        collect_Values_20_40_Absolute.insert(collect_Values_20_40_Absolute.end(), values_AbsoluteUncertainty_20_40.begin(), values_AbsoluteUncertainty_20_40.end());
        
        for (size_t j = 0; j < values_AbsoluteUncertainty_20_40.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_20_40_AbsoluteUncertainty = new TH1F(Form("h_20_40_AbsoluteUncertainty_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_20_40_AbsoluteUncertainty->SetBinContent(1, values_AbsoluteUncertainty_20_40[j]);
            h_20_40_AbsoluteUncertainty->SetFillColor(colors[j]);
            h_20_40_AbsoluteUncertainty->Draw("SAME");
            histograms_20_40_AbsoluteUncertainty.push_back(h_20_40_AbsoluteUncertainty);
        }
        
    }
    double maxYValue_20_40_Absolute = FindMaxValueFromPlottedData(collect_Values_20_40_Absolute);
    AdjustFrameYAxis(c_Overlay_20_40_Systematics_AbsoluteUncertainty, frame_20_40_AbsoluteUncertainty, maxYValue_20_40_Absolute * 1.05);
    
    TLegend *legend_20_40_bars_AbsoluteUncertainty = new TLegend(0.2, 0.72, 0.4, 0.92);
    legend_20_40_bars_AbsoluteUncertainty->SetTextSize(0.03);
    legend_20_40_bars_AbsoluteUncertainty->SetHeader("#bf{Absolute Systematics, 20-40%}", "L");
    legend_20_40_bars_AbsoluteUncertainty->AddEntry(histograms_20_40_AbsoluteUncertainty[0], "EMCal Scale", "f");
    legend_20_40_bars_AbsoluteUncertainty->AddEntry(histograms_20_40_AbsoluteUncertainty[1], "Signal Window", "f");
    legend_20_40_bars_AbsoluteUncertainty->AddEntry(histograms_20_40_AbsoluteUncertainty[2], "Background Window", "f");
    legend_20_40_bars_AbsoluteUncertainty->AddEntry(histograms_20_40_AbsoluteUncertainty[3], "Final Systematic Uncertainty", "f");
    legend_20_40_bars_AbsoluteUncertainty->Draw();
   
    c_Overlay_20_40_Systematics_AbsoluteUncertainty->Modified();
    c_Overlay_20_40_Systematics_AbsoluteUncertainty->Update();
    
    
    c_Overlay_20_40_Systematics_AbsoluteUncertainty->SaveAs((BasePlotOutputPath + "/Systematics_Analysis-v2-Checks/AbsoluteSystUncertainty_Overlay_20_40.png").c_str());
    
    
    
    std::vector<TH1F*> histograms_40_60_AbsoluteUncertainty;
    std::vector<double> collect_Values_40_60_Absolute;
    
    TCanvas *c_Overlay_40_60_Systematics_AbsoluteUncertainty = new TCanvas("c_Overlay_40_60_Systematics_AbsoluteUncertainty", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    TH1F *frame_40_60_AbsoluteUncertainty = c_Overlay_40_60_Systematics_AbsoluteUncertainty->DrawFrame(2.0, 0, 5.0, 0.7);
    frame_40_60_AbsoluteUncertainty->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_40_60_AbsoluteUncertainty->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        double x = ptCenters[i] - (2.5 * barWidth);
        double v2_value = std::abs(data1.corrected_v2_40_60[i]);
        
        std::vector<double> values_AbsoluteUncertainty_40_60 = {
            data1.quad_sum_EMCal_syst_40_60[i] * v2_value,
            data1.signal_window_syst_40_60[i] * v2_value,
            data1.background_window_syst_40_60[i] * v2_value,
            data1.stat_uncertainties_40_60[i]
        };
        collect_Values_40_60_Absolute.insert(collect_Values_40_60_Absolute.end(), values_AbsoluteUncertainty_40_60.begin(), values_AbsoluteUncertainty_40_60.end());
        
        for (size_t j = 0; j < values_AbsoluteUncertainty_40_60.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_40_60_AbsoluteUncertainty = new TH1F(Form("h_40_60_AbsoluteUncertainty_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_40_60_AbsoluteUncertainty->SetBinContent(1, values_AbsoluteUncertainty_40_60[j]);
            h_40_60_AbsoluteUncertainty->SetFillColor(colors[j]);
            h_40_60_AbsoluteUncertainty->Draw("SAME");
            histograms_40_60_AbsoluteUncertainty.push_back(h_40_60_AbsoluteUncertainty);
        }
        
    }
    double maxYValue_40_60_Absolute = FindMaxValueFromPlottedData(collect_Values_40_60_Absolute);
    AdjustFrameYAxis(c_Overlay_40_60_Systematics_AbsoluteUncertainty, frame_40_60_AbsoluteUncertainty, maxYValue_40_60_Absolute * 1.05);
    
    TLegend *legend_40_60_bars_AbsoluteUncertainty = new TLegend(0.2, 0.72, 0.4, 0.92);
    legend_40_60_bars_AbsoluteUncertainty->SetTextSize(0.03);
    legend_40_60_bars_AbsoluteUncertainty->SetHeader("#bf{Absolute Systematics, 40-60%}", "L");
    legend_40_60_bars_AbsoluteUncertainty->AddEntry(histograms_40_60_AbsoluteUncertainty[0], "EMCal Scale", "f");
    legend_40_60_bars_AbsoluteUncertainty->AddEntry(histograms_40_60_AbsoluteUncertainty[1], "Signal Window", "f");
    legend_40_60_bars_AbsoluteUncertainty->AddEntry(histograms_40_60_AbsoluteUncertainty[2], "Background Window", "f");
    legend_40_60_bars_AbsoluteUncertainty->AddEntry(histograms_40_60_AbsoluteUncertainty[3], "Final Systematic Uncertainty", "f");
    legend_40_60_bars_AbsoluteUncertainty->Draw();
    
    c_Overlay_40_60_Systematics_AbsoluteUncertainty->Modified();
    c_Overlay_40_60_Systematics_AbsoluteUncertainty->Update();
    
    
    c_Overlay_40_60_Systematics_AbsoluteUncertainty->SaveAs((BasePlotOutputPath + "/Systematics_Analysis-v2-Checks/AbsoluteSystUncertainty_Overlay_40_60.png").c_str());
}



void DrawLegend_EMCal_Contributors(double x1, double y1, double x2, double y2, double textSize, const std::string& headerType, const std::vector<TH1F*>& histograms, const std::vector<std::string>& labels) {
    TLegend *legend = new TLegend(x1, y1, x2, y2);
    legend->SetTextSize(textSize);
    std::string header = "#bf{" + headerType + " Uncertainties}";
    legend->SetHeader(header.c_str(), "L");
    for (size_t i = 0; i < histograms.size(); ++i) {
        if (i < labels.size()) {
            legend->AddEntry(histograms[i], labels[i].c_str(), "f");
        }
    }
    legend->Draw();
}


void plotting_EMCal_Syst_Contributors(const EMCal_Systematic_Data& EMCal_Systematic_data1) {
    std::vector<double> ptCenters = {2.25, 2.75, 3.25, 3.75, 4.25, 4.75}; // Mid-points of pT ranges
    double barWidth = 0.05; // Width of each bar
    std::vector<std::string> labels = {"SYST1CEMC", "SYST2CEMC", "SYST3DCEMC", "SYST3UCEMC", "SYST4CEMC", "Quadrature Sum"};

    
    int colors[] = {kRed, kBlue, kGreen+1, kOrange+7, kViolet, kGray+1}; // Colors for the bars
    const double EPSILON = 1e-6;
    
    

    std::vector<TH1F*> histograms_0_20_Relative; // Vector to hold histogram pointers for legend
    std::vector<double> collect_Values_0_20_Relative;
    
    TCanvas *c_Overlay_0_20_Systematics_Relative = new TCanvas("c_Overlay_0_20_Systematics_Relative", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    TH1F *frame_0_20_Relative = c_Overlay_0_20_Systematics_Relative->DrawFrame(2.0, 0, 5.0, 0.62); // Adjust the range accordingly

    frame_0_20_Relative->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_0_20_Relative->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    // Loop over pT centers
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        double x = ptCenters[i] - (2.5 * barWidth); // Starting x position for the bars
        std::vector<double> values_Relative = {
            EMCal_Systematic_data1.RelativeUncertainty_SYST1CEMC_0_20[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST2CEMC_0_20[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST3DCEMC_0_20[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST3UCEMC_0_20[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST4CEMC_0_20[i],
            EMCal_Systematic_data1.Relative_QuadSum_0_20[i]
        };
        collect_Values_0_20_Relative.insert(collect_Values_0_20_Relative.end(), values_Relative.begin(), values_Relative.end());
        
        
        for (size_t j = 0; j < values_Relative.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_0_20_Relative = new TH1F(Form("h_0_20_Relative_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_0_20_Relative->SetBinContent(1, values_Relative[j]);
            h_0_20_Relative->SetFillColor(colors[j]);
            h_0_20_Relative->Draw("SAME");
            histograms_0_20_Relative.push_back(h_0_20_Relative); // Save the histogram pointer for the legend
        }
    }
    double maxYValue_0_20_Relative = FindMaxValueFromPlottedData(collect_Values_0_20_Relative);
    AdjustFrameYAxis(c_Overlay_0_20_Systematics_Relative, frame_0_20_Relative, maxYValue_0_20_Relative * 1.05);

    
    DrawLegend_EMCal_Contributors(0.2, 0.56, 0.4, .76, 0.03, "Relative", histograms_0_20_Relative, labels);


    TLegend *leg0_20_sPHENIXlabel_Relative = new TLegend(0.14, 0.76, 0.32, 0.92);
    leg0_20_sPHENIXlabel_Relative->SetTextSize(0.045);
    leg0_20_sPHENIXlabel_Relative->SetFillStyle(0);
    leg0_20_sPHENIXlabel_Relative->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg0_20_sPHENIXlabel_Relative->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg0_20_sPHENIXlabel_Relative->AddEntry("", "0-20% Centrality", "");
    leg0_20_sPHENIXlabel_Relative->Draw("same");
    c_Overlay_0_20_Systematics_Relative->Modified();
    c_Overlay_0_20_Systematics_Relative->Update();
    c_Overlay_0_20_Systematics_Relative->SaveAs((baseDataPath_EmCal_Systematics + "EMCal_SystContributor_Overlay_0_20_RelativeUncertainty.png").c_str());
    
    
    
    
    
    std::vector<TH1F*> histograms_0_20_Relative_Zoomed_In; // Vector to hold histogram pointers for legend
    std::vector<double> collect_Values_0_20_Relative_Zoomed_In;
    TCanvas *c_Overlay_0_20_Systematics_Relative_Zoomed_In = new TCanvas("c_Overlay_0_20_Systematics_Relative_Zoomed_In", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    TH1F *frame_0_20_Relative_Zoomed_In = c_Overlay_0_20_Systematics_Relative_Zoomed_In->DrawFrame(2.0, 0, 5.0, 0.62); // Adjust the range accordingly

    frame_0_20_Relative_Zoomed_In->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_0_20_Relative_Zoomed_In->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    // Loop over pT centers
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        // Skip this pT center if it's within EPSILON of 3.75
        if (std::abs(ptCenters[i] - 3.75) < EPSILON) {
            continue;  // Skip the rest of the loop for this pT center
        }
        if (std::abs(ptCenters[i] - 4.25) < EPSILON) {
            continue;  // Skip the rest of the loop for this pT center
        }
        double x = ptCenters[i] - (2.5 * barWidth); // Starting x position for the bars
        
        std::vector<double> values_Relative_Zoomed_In = {
            EMCal_Systematic_data1.RelativeUncertainty_SYST1CEMC_0_20[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST2CEMC_0_20[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST3DCEMC_0_20[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST3UCEMC_0_20[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST4CEMC_0_20[i],
            EMCal_Systematic_data1.Relative_QuadSum_0_20[i]
        };
        collect_Values_0_20_Relative_Zoomed_In.insert(collect_Values_0_20_Relative_Zoomed_In.end(), values_Relative_Zoomed_In.begin(), values_Relative_Zoomed_In.end());
        
        for (size_t j = 0; j < values_Relative_Zoomed_In.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_0_20_Relative_Zoomed_In = new TH1F(Form("h_0_20_Relative_Zoomed_In_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_0_20_Relative_Zoomed_In->SetBinContent(1, values_Relative_Zoomed_In[j]);
            h_0_20_Relative_Zoomed_In->SetFillColor(colors[j]);
            h_0_20_Relative_Zoomed_In->Draw("SAME");
            histograms_0_20_Relative_Zoomed_In.push_back(h_0_20_Relative_Zoomed_In); // Save the histogram pointer for the legend
        }
    }
    double maxYValue_0_20_Relative_Zoomed_In = FindMaxValueFromPlottedData(collect_Values_0_20_Relative_Zoomed_In);
    AdjustFrameYAxis(c_Overlay_0_20_Systematics_Relative_Zoomed_In, frame_0_20_Relative_Zoomed_In, maxYValue_0_20_Relative_Zoomed_In * 1.05);


    DrawLegend_EMCal_Contributors(0.56, 0.7, 0.72, 0.9, 0.03, "Relative", histograms_0_20_Relative_Zoomed_In, labels);


    TLegend *leg0_20_sPHENIXlabel_Relative_Zoomed_In = new TLegend(0.14, 0.76, 0.32, 0.92);
    leg0_20_sPHENIXlabel_Relative_Zoomed_In->SetTextSize(0.045);
    leg0_20_sPHENIXlabel_Relative_Zoomed_In->SetFillStyle(0);
    leg0_20_sPHENIXlabel_Relative_Zoomed_In->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg0_20_sPHENIXlabel_Relative_Zoomed_In->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg0_20_sPHENIXlabel_Relative_Zoomed_In->AddEntry("", "0-20% Centrality", "");
    leg0_20_sPHENIXlabel_Relative_Zoomed_In->Draw("same");
    
    c_Overlay_0_20_Systematics_Relative_Zoomed_In->Modified();
    c_Overlay_0_20_Systematics_Relative_Zoomed_In->Update();
    c_Overlay_0_20_Systematics_Relative_Zoomed_In->SaveAs((baseDataPath_EmCal_Systematics + "EMCal_SystContributor_Overlay_0_20_Relative_Zoomed_In_Uncertainty.png").c_str());
    
    
    
    
    
    
    
    std::vector<TH1F*> histograms_20_40_Relative; // Vector to hold histogram pointers for legend
    std::vector<double> collect_Values_20_40_Relative;
    
    TCanvas *c_Overlay_20_40_Systematics_Relative = new TCanvas("c_Overlay_20_40_Systematics_Relative", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 20-40% Centrality", 800, 600);
    TH1F *frame_20_40_Relative = c_Overlay_20_40_Systematics_Relative->DrawFrame(2.0, 0, 5.0, 0.62); // Adjust the range accordingly

    frame_20_40_Relative->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_20_40_Relative->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    // Loop over pT centers
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        double x = ptCenters[i] - (2.5 * barWidth); // Starting x position for the bars
        std::vector<double> values_Relative = {
            EMCal_Systematic_data1.RelativeUncertainty_SYST1CEMC_20_40[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST2CEMC_20_40[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST3DCEMC_20_40[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST3UCEMC_20_40[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST4CEMC_20_40[i],
            EMCal_Systematic_data1.Relative_QuadSum_20_40[i]
        };
        collect_Values_20_40_Relative.insert(collect_Values_20_40_Relative.end(), values_Relative.begin(), values_Relative.end());
        
        
        for (size_t j = 0; j < values_Relative.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_20_40_Relative = new TH1F(Form("h_20_40_Relative_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_20_40_Relative->SetBinContent(1, values_Relative[j]);
            h_20_40_Relative->SetFillColor(colors[j]);
            h_20_40_Relative->Draw("SAME");
            histograms_20_40_Relative.push_back(h_20_40_Relative); // Save the histogram pointer for the legend
        }
    }
    double maxYValue_20_40_Relative = FindMaxValueFromPlottedData(collect_Values_20_40_Relative);
    AdjustFrameYAxis(c_Overlay_20_40_Systematics_Relative, frame_20_40_Relative, maxYValue_20_40_Relative * 1.05);

    
    DrawLegend_EMCal_Contributors(0.2, 0.56, 0.4, .76, 0.03, "Relative", histograms_20_40_Relative, labels);


    TLegend *leg20_40_sPHENIXlabel_Relative = new TLegend(0.14, 0.76, 0.32, 0.92);
    leg20_40_sPHENIXlabel_Relative->SetTextSize(0.045);
    leg20_40_sPHENIXlabel_Relative->SetFillStyle(0);
    leg20_40_sPHENIXlabel_Relative->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg20_40_sPHENIXlabel_Relative->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg20_40_sPHENIXlabel_Relative->AddEntry("", "20-40% Centrality", "");
    leg20_40_sPHENIXlabel_Relative->Draw("same");
    c_Overlay_20_40_Systematics_Relative->Modified();
    c_Overlay_20_40_Systematics_Relative->Update();
    c_Overlay_20_40_Systematics_Relative->SaveAs((baseDataPath_EmCal_Systematics + "EMCal_SystContributor_Overlay_20_40_RelativeUncertainty.png").c_str());
    
    
    
    
    
    std::vector<TH1F*> histograms_20_40_Relative_Zoomed_In; // Vector to hold histogram pointers for legend
    std::vector<double> collect_Values_20_40_Relative_Zoomed_In;
    TCanvas *c_Overlay_20_40_Systematics_Relative_Zoomed_In = new TCanvas("c_Overlay_20_40_Systematics_Relative_Zoomed_In", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 20-40% Centrality", 800, 600);
    TH1F *frame_20_40_Relative_Zoomed_In = c_Overlay_20_40_Systematics_Relative_Zoomed_In->DrawFrame(2.0, 0, 5.0, 0.62); // Adjust the range accordingly

    frame_20_40_Relative_Zoomed_In->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_20_40_Relative_Zoomed_In->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    // Loop over pT centers
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        // Skip this pT center if it's within EPSILON of 3.75
        if (std::abs(ptCenters[i] - 4.25) < EPSILON) {
            continue;  // Skip the rest of the loop for this pT center
        }

        double x = ptCenters[i] - (2.5 * barWidth); // Starting x position for the bars
        
        std::vector<double> values_Relative_Zoomed_In = {
            EMCal_Systematic_data1.RelativeUncertainty_SYST1CEMC_20_40[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST2CEMC_20_40[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST3DCEMC_20_40[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST3UCEMC_20_40[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST4CEMC_20_40[i],
            EMCal_Systematic_data1.Relative_QuadSum_20_40[i]
        };
        collect_Values_20_40_Relative_Zoomed_In.insert(collect_Values_20_40_Relative_Zoomed_In.end(), values_Relative_Zoomed_In.begin(), values_Relative_Zoomed_In.end());
        
        for (size_t j = 0; j < values_Relative_Zoomed_In.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_20_40_Relative_Zoomed_In = new TH1F(Form("h_20_40_Relative_Zoomed_In_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_20_40_Relative_Zoomed_In->SetBinContent(1, values_Relative_Zoomed_In[j]);
            h_20_40_Relative_Zoomed_In->SetFillColor(colors[j]);
            h_20_40_Relative_Zoomed_In->Draw("SAME");
            histograms_20_40_Relative_Zoomed_In.push_back(h_20_40_Relative_Zoomed_In); // Save the histogram pointer for the legend
        }
    }
    double maxYValue_20_40_Relative_Zoomed_In = FindMaxValueFromPlottedData(collect_Values_20_40_Relative_Zoomed_In);
    AdjustFrameYAxis(c_Overlay_20_40_Systematics_Relative_Zoomed_In, frame_20_40_Relative_Zoomed_In, maxYValue_20_40_Relative_Zoomed_In * 1.05);


    DrawLegend_EMCal_Contributors(0.7, 0.7, 0.9, 0.9, 0.03, "Relative", histograms_20_40_Relative_Zoomed_In, labels);


    TLegend *leg20_40_sPHENIXlabel_Relative_Zoomed_In = new TLegend(0.14, 0.76, 0.32, 0.92);
    leg20_40_sPHENIXlabel_Relative_Zoomed_In->SetTextSize(0.045);
    leg20_40_sPHENIXlabel_Relative_Zoomed_In->SetFillStyle(0);
    leg20_40_sPHENIXlabel_Relative_Zoomed_In->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg20_40_sPHENIXlabel_Relative_Zoomed_In->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg20_40_sPHENIXlabel_Relative_Zoomed_In->AddEntry("", "20-40% Centrality", "");
    leg20_40_sPHENIXlabel_Relative_Zoomed_In->Draw("same");
    
    c_Overlay_20_40_Systematics_Relative_Zoomed_In->Modified();
    c_Overlay_20_40_Systematics_Relative_Zoomed_In->Update();
    c_Overlay_20_40_Systematics_Relative_Zoomed_In->SaveAs((baseDataPath_EmCal_Systematics + "EMCal_SystContributor_Overlay_20_40_Relative_Zoomed_In_Uncertainty.png").c_str());
    
    
    
    
    
    
    
    
    
    
    std::vector<TH1F*> histograms_40_60_Relative; // Vector to hold histogram pointers for legend
    std::vector<double> collect_Values_40_60_Relative;
    
    TCanvas *c_Overlay_40_60_Systematics_Relative = new TCanvas("c_Overlay_40_60_Systematics_Relative", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    TH1F *frame_40_60_Relative = c_Overlay_40_60_Systematics_Relative->DrawFrame(2.0, 0, 5.0, 0.62); // Adjust the range accordingly

    frame_40_60_Relative->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_40_60_Relative->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    // Loop over pT centers
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        double x = ptCenters[i] - (2.5 * barWidth); // Starting x position for the bars
        std::vector<double> values_Relative = {
            EMCal_Systematic_data1.RelativeUncertainty_SYST1CEMC_40_60[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST2CEMC_40_60[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST3DCEMC_40_60[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST3UCEMC_40_60[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST4CEMC_40_60[i],
            EMCal_Systematic_data1.Relative_QuadSum_40_60[i]
        };
        collect_Values_40_60_Relative.insert(collect_Values_40_60_Relative.end(), values_Relative.begin(), values_Relative.end());
        
        
        for (size_t j = 0; j < values_Relative.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_40_60_Relative = new TH1F(Form("h_40_60_Relative_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_40_60_Relative->SetBinContent(1, values_Relative[j]);
            h_40_60_Relative->SetFillColor(colors[j]);
            h_40_60_Relative->Draw("SAME");
            histograms_40_60_Relative.push_back(h_40_60_Relative); // Save the histogram pointer for the legend
        }
    }
    double maxYValue_40_60_Relative = FindMaxValueFromPlottedData(collect_Values_40_60_Relative);
    AdjustFrameYAxis(c_Overlay_40_60_Systematics_Relative, frame_40_60_Relative, maxYValue_40_60_Relative * 1.05);

    
    DrawLegend_EMCal_Contributors(0.2, 0.56, 0.4, .76, 0.03, "Relative", histograms_40_60_Relative, labels);


    TLegend *leg40_60_sPHENIXlabel_Relative = new TLegend(0.14, 0.76, 0.32, 0.92);
    leg40_60_sPHENIXlabel_Relative->SetTextSize(0.045);
    leg40_60_sPHENIXlabel_Relative->SetFillStyle(0);
    leg40_60_sPHENIXlabel_Relative->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg40_60_sPHENIXlabel_Relative->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg40_60_sPHENIXlabel_Relative->AddEntry("", "40-60% Centrality", "");
    leg40_60_sPHENIXlabel_Relative->Draw("same");
    c_Overlay_40_60_Systematics_Relative->Modified();
    c_Overlay_40_60_Systematics_Relative->Update();
    c_Overlay_40_60_Systematics_Relative->SaveAs((baseDataPath_EmCal_Systematics + "EMCal_SystContributor_Overlay_40_60_RelativeUncertainty.png").c_str());
    
    
    
    
    
    std::vector<TH1F*> histograms_40_60_Relative_Zoomed_In; // Vector to hold histogram pointers for legend
    std::vector<double> collect_Values_40_60_Relative_Zoomed_In;
    TCanvas *c_Overlay_40_60_Systematics_Relative_Zoomed_In = new TCanvas("c_Overlay_40_60_Systematics_Relative_Zoomed_In", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    TH1F *frame_40_60_Relative_Zoomed_In = c_Overlay_40_60_Systematics_Relative_Zoomed_In->DrawFrame(2.0, 0, 5.0, 0.62); // Adjust the range accordingly

    frame_40_60_Relative_Zoomed_In->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_40_60_Relative_Zoomed_In->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    // Loop over pT centers
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        // Skip this pT center if it's within EPSILON of 3.75
        if (std::abs(ptCenters[i] - 4.75) < EPSILON) {
            continue;  // Skip the rest of the loop for this pT center
        }
        double x = ptCenters[i] - (2.5 * barWidth); // Starting x position for the bars
        
        std::vector<double> values_Relative_Zoomed_In = {
            EMCal_Systematic_data1.RelativeUncertainty_SYST1CEMC_40_60[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST2CEMC_40_60[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST3DCEMC_40_60[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST3UCEMC_40_60[i],
            EMCal_Systematic_data1.RelativeUncertainty_SYST4CEMC_40_60[i],
            EMCal_Systematic_data1.Relative_QuadSum_40_60[i]
        };
        collect_Values_40_60_Relative_Zoomed_In.insert(collect_Values_40_60_Relative_Zoomed_In.end(), values_Relative_Zoomed_In.begin(), values_Relative_Zoomed_In.end());
        
        for (size_t j = 0; j < values_Relative_Zoomed_In.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_40_60_Relative_Zoomed_In = new TH1F(Form("h_40_60_Relative_Zoomed_In_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_40_60_Relative_Zoomed_In->SetBinContent(1, values_Relative_Zoomed_In[j]);
            h_40_60_Relative_Zoomed_In->SetFillColor(colors[j]);
            h_40_60_Relative_Zoomed_In->Draw("SAME");
            histograms_40_60_Relative_Zoomed_In.push_back(h_40_60_Relative_Zoomed_In); // Save the histogram pointer for the legend
        }
    }
    double maxYValue_40_60_Relative_Zoomed_In = FindMaxValueFromPlottedData(collect_Values_40_60_Relative_Zoomed_In);
    AdjustFrameYAxis(c_Overlay_40_60_Systematics_Relative_Zoomed_In, frame_40_60_Relative_Zoomed_In, maxYValue_40_60_Relative_Zoomed_In * 1.05);
    DrawLegend_EMCal_Contributors(0.56, 0.7, 0.72, 0.9, 0.03, "Relative", histograms_40_60_Relative_Zoomed_In, labels);

    TLegend *leg40_60_sPHENIXlabel_Relative_Zoomed_In = new TLegend(0.14, 0.76, 0.28, 0.92);
    leg40_60_sPHENIXlabel_Relative_Zoomed_In->SetTextSize(0.035);
    leg40_60_sPHENIXlabel_Relative_Zoomed_In->SetFillStyle(0);
    leg40_60_sPHENIXlabel_Relative_Zoomed_In->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg40_60_sPHENIXlabel_Relative_Zoomed_In->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg40_60_sPHENIXlabel_Relative_Zoomed_In->AddEntry("", "40-60% Centrality", "");
    leg40_60_sPHENIXlabel_Relative_Zoomed_In->Draw("same");
    
    c_Overlay_40_60_Systematics_Relative_Zoomed_In->Modified();
    c_Overlay_40_60_Systematics_Relative_Zoomed_In->Update();
    c_Overlay_40_60_Systematics_Relative_Zoomed_In->SaveAs((baseDataPath_EmCal_Systematics + "EMCal_SystContributor_Overlay_40_60_Relative_Zoomed_In_Uncertainty.png").c_str());
    
    
    
    std::vector<TH1F*> histograms_0_20_Absolute; // Vector to hold histogram pointers for legend
    std::vector<double> collect_Values_0_20_Absolute;
    
    TCanvas *c_Overlay_0_20_Systematics_Absolute = new TCanvas("c_Overlay_0_20_Systematics_Absolute", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    TH1F *frame_0_20_Absolute = c_Overlay_0_20_Systematics_Absolute->DrawFrame(2.0, 0, 5.0, 0.62); // Adjust the range accordingly

    frame_0_20_Absolute->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_0_20_Absolute->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    // Loop over pT centers
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        double x = ptCenters[i] - (2.5 * barWidth); // Starting x position for the bars
        std::vector<double> values_Absolute = {
            EMCal_Systematic_data1.AbsoluteUncertainty_SYST1CEMC_0_20[i],
            EMCal_Systematic_data1.AbsoluteUncertainty_SYST2CEMC_0_20[i],
            EMCal_Systematic_data1.AbsoluteUncertainty_SYST3DCEMC_0_20[i],
            EMCal_Systematic_data1.AbsoluteUncertainty_SYST3UCEMC_0_20[i],
            EMCal_Systematic_data1.AbsoluteUncertainty_SYST4CEMC_0_20[i],
            EMCal_Systematic_data1.Absolute_QuadSum_0_20[i]
        };
        collect_Values_0_20_Absolute.insert(collect_Values_0_20_Absolute.end(), values_Absolute.begin(), values_Absolute.end());
        
        
        for (size_t j = 0; j < values_Absolute.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_0_20_Absolute = new TH1F(Form("h_0_20_Absolute_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_0_20_Absolute->SetBinContent(1, values_Absolute[j]);
            h_0_20_Absolute->SetFillColor(colors[j]);
            h_0_20_Absolute->Draw("SAME");
            histograms_0_20_Absolute.push_back(h_0_20_Absolute); // Save the histogram pointer for the legend
        }
    }
    double maxYValue_0_20_Absolute = FindMaxValueFromPlottedData(collect_Values_0_20_Absolute);
    AdjustFrameYAxis(c_Overlay_0_20_Systematics_Absolute, frame_0_20_Absolute, maxYValue_0_20_Absolute * 1.05);

    
    DrawLegend_EMCal_Contributors(0.18, 0.56, 0.36, .76, 0.03, "Absolute", histograms_0_20_Absolute, labels);


    TLegend *leg0_20_sPHENIXlabel_Absolute = new TLegend(0.14, 0.76, 0.32, 0.92);
    leg0_20_sPHENIXlabel_Absolute->SetTextSize(0.045);
    leg0_20_sPHENIXlabel_Absolute->SetFillStyle(0);
    leg0_20_sPHENIXlabel_Absolute->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg0_20_sPHENIXlabel_Absolute->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg0_20_sPHENIXlabel_Absolute->AddEntry("", "0-20% Centrality", "");
    leg0_20_sPHENIXlabel_Absolute->Draw("same");
    c_Overlay_0_20_Systematics_Absolute->Modified();
    c_Overlay_0_20_Systematics_Absolute->Update();
    c_Overlay_0_20_Systematics_Absolute->SaveAs((baseDataPath_EmCal_Systematics + "EMCal_SystContributor_Overlay_0_20_AbsoluteUncertainty.png").c_str());
    
    
    
    std::vector<TH1F*> histograms_20_40_Absolute; // Vector to hold histogram pointers for legend
    std::vector<double> collect_Values_20_40_Absolute;
    
    TCanvas *c_Overlay_20_40_Systematics_Absolute = new TCanvas("c_Overlay_20_40_Systematics_Absolute", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 20-40% Centrality", 800, 600);
    TH1F *frame_20_40_Absolute = c_Overlay_20_40_Systematics_Absolute->DrawFrame(2.0, 0, 5.0, 0.62); // Adjust the range accordingly

    frame_20_40_Absolute->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_20_40_Absolute->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    // Loop over pT centers
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        double x = ptCenters[i] - (2.5 * barWidth); // Starting x position for the bars
        std::vector<double> values_Absolute = {
            EMCal_Systematic_data1.AbsoluteUncertainty_SYST1CEMC_20_40[i],
            EMCal_Systematic_data1.AbsoluteUncertainty_SYST2CEMC_20_40[i],
            EMCal_Systematic_data1.AbsoluteUncertainty_SYST3DCEMC_20_40[i],
            EMCal_Systematic_data1.AbsoluteUncertainty_SYST3UCEMC_20_40[i],
            EMCal_Systematic_data1.AbsoluteUncertainty_SYST4CEMC_20_40[i],
            EMCal_Systematic_data1.Absolute_QuadSum_20_40[i]
        };
        collect_Values_20_40_Absolute.insert(collect_Values_20_40_Absolute.end(), values_Absolute.begin(), values_Absolute.end());
        
        
        for (size_t j = 0; j < values_Absolute.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_20_40_Absolute = new TH1F(Form("h_20_40_Absolute_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_20_40_Absolute->SetBinContent(1, values_Absolute[j]);
            h_20_40_Absolute->SetFillColor(colors[j]);
            h_20_40_Absolute->Draw("SAME");
            histograms_20_40_Absolute.push_back(h_20_40_Absolute); // Save the histogram pointer for the legend
        }
    }
    double maxYValue_20_40_Absolute = FindMaxValueFromPlottedData(collect_Values_20_40_Absolute);
    AdjustFrameYAxis(c_Overlay_20_40_Systematics_Absolute, frame_20_40_Absolute, maxYValue_20_40_Absolute * 1.05);

    
    DrawLegend_EMCal_Contributors(0.18, 0.56, 0.36, .76, 0.028, "Absolute", histograms_20_40_Absolute, labels);


    TLegend *leg20_40_sPHENIXlabel_Absolute = new TLegend(0.14, 0.76, 0.32, 0.92);
    leg20_40_sPHENIXlabel_Absolute->SetTextSize(0.045);
    leg20_40_sPHENIXlabel_Absolute->SetFillStyle(0);
    leg20_40_sPHENIXlabel_Absolute->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg20_40_sPHENIXlabel_Absolute->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg20_40_sPHENIXlabel_Absolute->AddEntry("", "20-40% Centrality", "");
    leg20_40_sPHENIXlabel_Absolute->Draw("same");
    c_Overlay_20_40_Systematics_Absolute->Modified();
    c_Overlay_20_40_Systematics_Absolute->Update();
    c_Overlay_20_40_Systematics_Absolute->SaveAs((baseDataPath_EmCal_Systematics + "EMCal_SystContributor_Overlay_20_40_AbsoluteUncertainty.png").c_str());
    
    
    
    std::vector<TH1F*> histograms_40_60_Absolute; // Vector to hold histogram pointers for legend
    std::vector<double> collect_Values_40_60_Absolute;
    
    TCanvas *c_Overlay_40_60_Systematics_Absolute = new TCanvas("c_Overlay_40_60_Systematics_Absolute", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    TH1F *frame_40_60_Absolute = c_Overlay_40_60_Systematics_Absolute->DrawFrame(2.0, 0, 5.0, 0.62); // Adjust the range accordingly

    frame_40_60_Absolute->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_40_60_Absolute->GetXaxis()->SetTitle("p_{T} [GeV]");
    
    // Loop over pT centers
    for (size_t i = 0; i < ptCenters.size(); ++i) {
        double x = ptCenters[i] - (2.5 * barWidth); // Starting x position for the bars
        std::vector<double> values_Absolute = {
            EMCal_Systematic_data1.AbsoluteUncertainty_SYST1CEMC_40_60[i],
            EMCal_Systematic_data1.AbsoluteUncertainty_SYST2CEMC_40_60[i],
            EMCal_Systematic_data1.AbsoluteUncertainty_SYST3DCEMC_40_60[i],
            EMCal_Systematic_data1.AbsoluteUncertainty_SYST3UCEMC_40_60[i],
            EMCal_Systematic_data1.AbsoluteUncertainty_SYST4CEMC_40_60[i],
            EMCal_Systematic_data1.Absolute_QuadSum_40_60[i]
        };
        collect_Values_40_60_Absolute.insert(collect_Values_40_60_Absolute.end(), values_Absolute.begin(), values_Absolute.end());
        
        
        for (size_t j = 0; j < values_Absolute.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_40_60_Absolute = new TH1F(Form("h_40_60_Absolute_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_40_60_Absolute->SetBinContent(1, values_Absolute[j]);
            h_40_60_Absolute->SetFillColor(colors[j]);
            h_40_60_Absolute->Draw("SAME");
            histograms_40_60_Absolute.push_back(h_40_60_Absolute); // Save the histogram pointer for the legend
        }
    }
    double maxYValue_40_60_Absolute = FindMaxValueFromPlottedData(collect_Values_40_60_Absolute);
    AdjustFrameYAxis(c_Overlay_40_60_Systematics_Absolute, frame_40_60_Absolute, maxYValue_40_60_Absolute * 1.05);

    
    DrawLegend_EMCal_Contributors(0.2, 0.56, 0.4, .76, 0.03, "Absolute", histograms_40_60_Absolute, labels);


    TLegend *leg40_60_sPHENIXlabel_Absolute = new TLegend(0.14, 0.76, 0.32, 0.92);
    leg40_60_sPHENIXlabel_Absolute->SetTextSize(0.045);
    leg40_60_sPHENIXlabel_Absolute->SetFillStyle(0);
    leg40_60_sPHENIXlabel_Absolute->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg40_60_sPHENIXlabel_Absolute->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg40_60_sPHENIXlabel_Absolute->AddEntry("", "40-60% Centrality", "");
    leg40_60_sPHENIXlabel_Absolute->Draw("same");
    c_Overlay_40_60_Systematics_Absolute->Modified();
    c_Overlay_40_60_Systematics_Absolute->Update();
    c_Overlay_40_60_Systematics_Absolute->SaveAs((baseDataPath_EmCal_Systematics + "EMCal_SystContributor_Overlay_40_60_AbsoluteUncertainty.png").c_str());
}





double offset = 0.05;
void plot_EMCal_Scale_v2_overlays(const Data& data1, Data& data2, Data& data3, Data& data4, Data& data5, Data& data6) {
    TGraphErrors* corrected_v2_0_20_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_0_20, data1.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_20_40, data1.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_40_60, data1.corrected_v2_40_60_Errors);
    
    TGraphErrors* corrected_v2_0_20_graph_2 = CreateGraph(ptCenters, data2.corrected_v2_0_20, data2.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_2 = CreateGraph(ptCenters, data2.corrected_v2_20_40, data2.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_2 = CreateGraph(ptCenters, data2.corrected_v2_40_60, data2.corrected_v2_40_60_Errors);

    TGraphErrors* corrected_v2_0_20_graph_3 = CreateGraph(ptCenters, data3.corrected_v2_0_20, data3.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_3 = CreateGraph(ptCenters, data3.corrected_v2_20_40, data3.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_3 = CreateGraph(ptCenters, data3.corrected_v2_40_60, data3.corrected_v2_40_60_Errors);
    
    TGraphErrors* corrected_v2_0_20_graph_4 = CreateGraph(ptCenters, data4.corrected_v2_0_20, data4.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_4 = CreateGraph(ptCenters, data4.corrected_v2_20_40, data4.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_4 = CreateGraph(ptCenters, data4.corrected_v2_40_60, data4.corrected_v2_40_60_Errors);

    TGraphErrors* corrected_v2_0_20_graph_5 = CreateGraph(ptCenters, data5.corrected_v2_0_20, data5.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_5 = CreateGraph(ptCenters, data5.corrected_v2_20_40, data5.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_5 = CreateGraph(ptCenters, data5.corrected_v2_40_60, data5.corrected_v2_40_60_Errors);

    TGraphErrors* corrected_v2_0_20_graph_6 = CreateGraph(ptCenters, data6.corrected_v2_0_20, data6.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_6 = CreateGraph(ptCenters, data6.corrected_v2_20_40, data6.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_6 = CreateGraph(ptCenters, data6.corrected_v2_40_60, data6.corrected_v2_40_60_Errors);
    
    double minYaxis_correctedComparisons = -0.2;
    double maxYaxis_correctedComparisons = 0.45;
    
    int markerColor_1 = kBlack;
    int markerStyle_1 = 20;
    double markerSize_1 = 1.0;
    
    int markerColor_2 =  kBlue;
    int markerStyle_2 = 20;
    double markerSize_2 = 1.0;
    
    int markerColor_3 = kOrange+2;
    int markerStyle_3 = 20;
    double markerSize_3 = 1.2;
    
    int markerColor_4 = kGreen+3;
    int markerStyle_4 = 20;
    double markerSize_4 = 1.2;
    
    int markerColor_5 = kRed;
    int markerStyle_5 = 20;
    double markerSize_5 = 1.0;
    
    int markerColor_6 = kViolet - 2;
    int markerStyle_6 = 20;
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
     Plot Overlay Of EMCal Scale Variations
     */
    
    //0-20%
    TCanvas *c_Overlay_0_20_v2_corrected_EMCal_Scale = new TCanvas("c_Overlay_0_20_v2_corrected_EMCal_Scale", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    corrected_v2_0_20_graph_4->Draw("AP");
    corrected_v2_0_20_graph_4->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 0-20% Centrality, Min Clus Energy Variations, Sideband = (#mu + 3#sigma, 0.5) GeV");
    corrected_v2_0_20_graph_4->GetXaxis()->SetTitle("p_{T}");
    corrected_v2_0_20_graph_4->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_0_20_graph_1->SetPoint(i, ptCenters[i] - 3.0 * offset, data1.corrected_v2_0_20[i]);
        corrected_v2_0_20_graph_2->SetPoint(i, ptCenters[i] - 2.0 * offset, data2.corrected_v2_0_20[i]);
        corrected_v2_0_20_graph_3->SetPoint(i, ptCenters[i] - 1.0 * offset, data3.corrected_v2_0_20[i]);
        corrected_v2_0_20_graph_5->SetPoint(i, ptCenters[i] + 1.0 * offset, data5.corrected_v2_0_20[i]);
        corrected_v2_0_20_graph_6->SetPoint(i, ptCenters[i] + 2.0 * offset, data6.corrected_v2_0_20[i]);
    }
    corrected_v2_0_20_graph_1 -> Draw("P SAME");
    corrected_v2_0_20_graph_2 -> Draw("P SAME");
    corrected_v2_0_20_graph_3 -> Draw("P SAME");
    corrected_v2_0_20_graph_5 -> Draw("P SAME");
    corrected_v2_0_20_graph_6 -> Draw("P SAME");
    corrected_v2_0_20_graph_4->SetMinimum(minYaxis_correctedComparisons); // Set the minimum y value
    corrected_v2_0_20_graph_4->SetMaximum(maxYaxis_correctedComparisons); // Set the maximum y value
    TLegend *legend_Overlay_0_20_v2_corrected_EMCal_Scale = new TLegend(0.17, 0.18, 0.56, 0.33);
    legend_Overlay_0_20_v2_corrected_EMCal_Scale->SetNColumns(2);
    legend_Overlay_0_20_v2_corrected_EMCal_Scale->SetBorderSize(0);
    legend_Overlay_0_20_v2_corrected_EMCal_Scale->SetTextSize(.028);
    legend_Overlay_0_20_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_0_20_graph_1, "Default", "pe");
    legend_Overlay_0_20_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_0_20_graph_2, "SYST1CEMC", "pe");
    legend_Overlay_0_20_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_0_20_graph_3, "SYST2CEMC", "pe");
    legend_Overlay_0_20_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_0_20_graph_4, "SYST3DCEMC", "pe");
    legend_Overlay_0_20_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_0_20_graph_5, "SYST3UCEMC", "pe");
    legend_Overlay_0_20_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_0_20_graph_6, "SYST4CEMC", "pe");
    legend_Overlay_0_20_v2_corrected_EMCal_Scale->Draw();
    TLegend *leg_sPHENIX_0_20_v2_corrected_EMCal_Scale = new TLegend(0.14,.72,0.34,.92);
    leg_sPHENIX_0_20_v2_corrected_EMCal_Scale->SetFillStyle(0);
    leg_sPHENIX_0_20_v2_corrected_EMCal_Scale->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg_sPHENIX_0_20_v2_corrected_EMCal_Scale->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg_sPHENIX_0_20_v2_corrected_EMCal_Scale->AddEntry("","0-20% Centrality","");
    leg_sPHENIX_0_20_v2_corrected_EMCal_Scale->Draw("same");
    DrawZeroLine(c_Overlay_0_20_v2_corrected_EMCal_Scale);
    c_Overlay_0_20_v2_corrected_EMCal_Scale->Modified();
    c_Overlay_0_20_v2_corrected_EMCal_Scale->Update();
    c_Overlay_0_20_v2_corrected_EMCal_Scale->SaveAs((baseDataPath_EmCal_Systematics + "EMCalScaleOverlay_v2Corrected_0_20.png").c_str());
    
    
    //20-40
    TCanvas *c_Overlay_20_40_v2_corrected_EMCal_Scale = new TCanvas("c_Overlay_20_40_v2_corrected_EMCal_Scale", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    corrected_v2_20_40_graph_4->Draw("AP");
    corrected_v2_20_40_graph_4->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 0-20% Centrality, Min Clus Energy Variations, Sideband = (#mu + 3#sigma, 0.5) GeV");
    corrected_v2_20_40_graph_4->GetXaxis()->SetTitle("p_{T}");
    corrected_v2_20_40_graph_4->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_20_40_graph_1->SetPoint(i, ptCenters[i] - 3.0 * offset, data1.corrected_v2_20_40[i]);
        corrected_v2_20_40_graph_2->SetPoint(i, ptCenters[i] - 2.0 * offset, data2.corrected_v2_20_40[i]);
        corrected_v2_20_40_graph_3->SetPoint(i, ptCenters[i] - 1.0 * offset, data3.corrected_v2_20_40[i]);
        corrected_v2_20_40_graph_5->SetPoint(i, ptCenters[i] + 1.0 * offset, data5.corrected_v2_20_40[i]);
        corrected_v2_20_40_graph_6->SetPoint(i, ptCenters[i] + 2.0 * offset, data6.corrected_v2_20_40[i]);
    }
    corrected_v2_20_40_graph_1 -> Draw("P SAME");
    corrected_v2_20_40_graph_2 -> Draw("P SAME");
    corrected_v2_20_40_graph_3 -> Draw("P SAME");
    corrected_v2_20_40_graph_5 -> Draw("P SAME");
    corrected_v2_20_40_graph_6 -> Draw("P SAME");
    corrected_v2_20_40_graph_4->SetMinimum(minYaxis_correctedComparisons); // Set the minimum y value
    corrected_v2_20_40_graph_4->SetMaximum(maxYaxis_correctedComparisons); // Set the maximum y value
    TLegend *legend_Overlay_20_40_v2_corrected_EMCal_Scale = new TLegend(0.17, 0.18, 0.56, 0.33);
    legend_Overlay_20_40_v2_corrected_EMCal_Scale->SetNColumns(2);
    legend_Overlay_20_40_v2_corrected_EMCal_Scale->SetBorderSize(0);
    legend_Overlay_20_40_v2_corrected_EMCal_Scale->SetTextSize(.028);
    legend_Overlay_20_40_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_20_40_graph_1, "Default", "pe");
    legend_Overlay_20_40_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_20_40_graph_2, "SYST1CEMC", "pe");
    legend_Overlay_20_40_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_20_40_graph_3, "SYST2CEMC", "pe");
    legend_Overlay_20_40_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_20_40_graph_4, "SYST3DCEMC", "pe");
    legend_Overlay_20_40_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_20_40_graph_5, "SYST3UCEMC", "pe");
    legend_Overlay_20_40_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_20_40_graph_6, "SYST4CEMC", "pe");
    legend_Overlay_20_40_v2_corrected_EMCal_Scale->Draw();
    TLegend *leg_sPHENIX_20_40_v2_corrected_EMCal_Scale = new TLegend(0.14,.72,0.34,.92);
    leg_sPHENIX_20_40_v2_corrected_EMCal_Scale->SetFillStyle(0);
    leg_sPHENIX_20_40_v2_corrected_EMCal_Scale->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg_sPHENIX_20_40_v2_corrected_EMCal_Scale->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg_sPHENIX_20_40_v2_corrected_EMCal_Scale->AddEntry("","20-40% Centrality","");
    leg_sPHENIX_20_40_v2_corrected_EMCal_Scale->Draw("same");
    DrawZeroLine(c_Overlay_20_40_v2_corrected_EMCal_Scale);
    c_Overlay_20_40_v2_corrected_EMCal_Scale->Modified();
    c_Overlay_20_40_v2_corrected_EMCal_Scale->Update();
    c_Overlay_20_40_v2_corrected_EMCal_Scale->SaveAs((baseDataPath_EmCal_Systematics + "EMCalScaleOverlay_v2Corrected_20_40.png").c_str());
    
    
    //40-60
    TCanvas *c_Overlay_40_60_v2_corrected_EMCal_Scale = new TCanvas("c_Overlay_40_60_v2_corrected_EMCal_Scale", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    corrected_v2_40_60_graph_4->Draw("AP");
    corrected_v2_40_60_graph_4->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 0-20% Centrality, Min Clus Energy Variations, Sideband = (#mu + 3#sigma, 0.5) GeV");
    corrected_v2_40_60_graph_4->GetXaxis()->SetTitle("p_{T}");
    corrected_v2_40_60_graph_4->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_40_60_graph_1->SetPoint(i, ptCenters[i] - 3.0 * offset, data1.corrected_v2_40_60[i]);
        corrected_v2_40_60_graph_2->SetPoint(i, ptCenters[i] - 2.0 * offset, data2.corrected_v2_40_60[i]);
        corrected_v2_40_60_graph_3->SetPoint(i, ptCenters[i] - 1.0 * offset, data3.corrected_v2_40_60[i]);
        corrected_v2_40_60_graph_5->SetPoint(i, ptCenters[i] + 1.0 * offset, data5.corrected_v2_40_60[i]);
        corrected_v2_40_60_graph_6->SetPoint(i, ptCenters[i] + 2.0 * offset, data6.corrected_v2_40_60[i]);
    }
    corrected_v2_40_60_graph_1 -> Draw("P SAME");
    corrected_v2_40_60_graph_2 -> Draw("P SAME");
    corrected_v2_40_60_graph_3 -> Draw("P SAME");
    corrected_v2_40_60_graph_5 -> Draw("P SAME");
    corrected_v2_40_60_graph_6 -> Draw("P SAME");
    corrected_v2_40_60_graph_4->SetMinimum(minYaxis_correctedComparisons); // Set the minimum y value
    corrected_v2_40_60_graph_4->SetMaximum(maxYaxis_correctedComparisons); // Set the maximum y value
    TLegend *legend_Overlay_40_60_v2_corrected_EMCal_Scale = new TLegend(0.17, 0.18, 0.56, 0.33);
    legend_Overlay_40_60_v2_corrected_EMCal_Scale->SetNColumns(2);
    legend_Overlay_40_60_v2_corrected_EMCal_Scale->SetBorderSize(0);
    legend_Overlay_40_60_v2_corrected_EMCal_Scale->SetTextSize(.028);
    legend_Overlay_40_60_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_40_60_graph_1, "Default", "pe");
    legend_Overlay_40_60_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_40_60_graph_2, "SYST1CEMC", "pe");
    legend_Overlay_40_60_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_40_60_graph_3, "SYST2CEMC", "pe");
    legend_Overlay_40_60_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_40_60_graph_4, "SYST3DCEMC", "pe");
    legend_Overlay_40_60_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_40_60_graph_5, "SYST3UCEMC", "pe");
    legend_Overlay_40_60_v2_corrected_EMCal_Scale->AddEntry(corrected_v2_40_60_graph_6, "SYST4CEMC", "pe");
    legend_Overlay_40_60_v2_corrected_EMCal_Scale->Draw();
    TLegend *leg_sPHENIX_40_60_v2_corrected_EMCal_Scale = new TLegend(0.14,.72,0.34,.92);
    leg_sPHENIX_40_60_v2_corrected_EMCal_Scale->SetFillStyle(0);
    leg_sPHENIX_40_60_v2_corrected_EMCal_Scale->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg_sPHENIX_40_60_v2_corrected_EMCal_Scale->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg_sPHENIX_40_60_v2_corrected_EMCal_Scale->AddEntry("","40-60% Centrality","");
    leg_sPHENIX_40_60_v2_corrected_EMCal_Scale->Draw("same");
    DrawZeroLine(c_Overlay_40_60_v2_corrected_EMCal_Scale);
    c_Overlay_40_60_v2_corrected_EMCal_Scale->Modified();
    c_Overlay_40_60_v2_corrected_EMCal_Scale->Update();
    c_Overlay_40_60_v2_corrected_EMCal_Scale->SaveAs((baseDataPath_EmCal_Systematics + "EMCalScaleOverlay_v2Corrected_40_60.png").c_str());
}

void plot_SignalWindow_Variation(const Data& data1, Data& data2) {
    TGraphErrors* corrected_v2_0_20_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_0_20, data1.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_20_40, data1.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_40_60, data1.corrected_v2_40_60_Errors);
    
    TGraphErrors* corrected_v2_0_20_graph_2 = CreateGraph(ptCenters, data2.corrected_v2_0_20, data2.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_2 = CreateGraph(ptCenters, data2.corrected_v2_20_40, data2.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_2 = CreateGraph(ptCenters, data2.corrected_v2_40_60, data2.corrected_v2_40_60_Errors);

    
    double minYaxis_correctedComparisons = -0.2;
    double maxYaxis_correctedComparisons = 0.45;
    
    int markerColor_1 = kBlack;
    int markerStyle_1 = 20;
    double markerSize_1 = 1.0;
    
    int markerColor_2 =  kBlue;
    int markerStyle_2 = 20;
    double markerSize_2 = 1.0;
    
    
    
    corrected_v2_0_20_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_0_20_graph_1->SetLineColor(markerColor_1);
    corrected_v2_0_20_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_0_20_graph_1->SetMarkerStyle(markerStyle_1);
    
    corrected_v2_0_20_graph_2->SetMarkerColor(markerColor_2);
    corrected_v2_0_20_graph_2->SetLineColor(markerColor_2);
    corrected_v2_0_20_graph_2->SetMarkerSize(markerSize_2);
    corrected_v2_0_20_graph_2->SetMarkerStyle(markerStyle_2);
    
    
    corrected_v2_20_40_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_20_40_graph_1->SetLineColor(markerColor_1);
    corrected_v2_20_40_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_20_40_graph_1->SetMarkerStyle(markerStyle_1);
    
    corrected_v2_20_40_graph_2->SetMarkerColor(markerColor_2);
    corrected_v2_20_40_graph_2->SetLineColor(markerColor_2);
    corrected_v2_20_40_graph_2->SetMarkerSize(markerSize_2);
    corrected_v2_20_40_graph_2->SetMarkerStyle(markerStyle_2);

    corrected_v2_40_60_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_40_60_graph_1->SetLineColor(markerColor_1);
    corrected_v2_40_60_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_40_60_graph_1->SetMarkerStyle(markerStyle_1);
    
    corrected_v2_40_60_graph_2->SetMarkerColor(markerColor_2);
    corrected_v2_40_60_graph_2->SetLineColor(markerColor_2);
    corrected_v2_40_60_graph_2->SetMarkerSize(markerSize_2);
    corrected_v2_40_60_graph_2->SetMarkerStyle(markerStyle_2);
    
    
    /*
     Plot Signal Window Variations
     */
    //0-20%
    TCanvas *c_Overlay_0_20_signalWindowVariation = new TCanvas("c_Overlay_0_20_signalWindowVariation", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_0_20_graph_2->SetPoint(i, ptCenters[i] - .05, data2.corrected_v2_0_20[i]);
        corrected_v2_0_20_graph_1->SetPoint(i, ptCenters[i] + .05, data1.corrected_v2_0_20[i]);
    }
    corrected_v2_0_20_graph_1->Draw("AP");
    corrected_v2_0_20_graph_2->Draw("P SAME");
    corrected_v2_0_20_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_v2_0_20_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Signal Window Variations");
    corrected_v2_0_20_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    corrected_v2_0_20_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    corrected_v2_0_20_graph_1->SetMinimum(minYaxis_correctedComparisons);
    corrected_v2_0_20_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_0_20_v2_corrected_signalWindowVariation = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_0_20_v2_corrected_signalWindowVariation->SetBorderSize(0);
    legend_Overlay_0_20_v2_corrected_signalWindowVariation->SetTextSize(0.04);
    legend_Overlay_0_20_v2_corrected_signalWindowVariation->AddEntry(corrected_v2_0_20_graph_2, "Signal Window = #mu #pm 1.50#sigma", "pe");
    legend_Overlay_0_20_v2_corrected_signalWindowVariation->AddEntry(corrected_v2_0_20_graph_1, "Signal Window = #mu #pm 2.00#sigma (Default)", "pe");
    legend_Overlay_0_20_v2_corrected_signalWindowVariation->Draw();
    DrawZeroLine(c_Overlay_0_20_signalWindowVariation);
    TLegend *legSPHENIX_0_20_signalWindowVariation = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_0_20_signalWindowVariation->SetFillStyle(0);
    legSPHENIX_0_20_signalWindowVariation->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_0_20_signalWindowVariation->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_0_20_signalWindowVariation->AddEntry("","0-20% Centrality","");
    legSPHENIX_0_20_signalWindowVariation->Draw("same");
    c_Overlay_0_20_signalWindowVariation->Modified();
    c_Overlay_0_20_signalWindowVariation->Update();
    c_Overlay_0_20_signalWindowVariation->SaveAs((baseDataPath_SignalWindowVariations + "Overlay_0_20_v2_SignalWindowVariations.png").c_str());
    
    
    //20-40%
    TCanvas *c_Overlay_20_40_signalWindowVariation = new TCanvas("c_Overlay_20_40_signalWindowVariation", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_20_40_graph_2->SetPoint(i, ptCenters[i] - .05, data2.corrected_v2_20_40[i]);
        corrected_v2_20_40_graph_1->SetPoint(i, ptCenters[i] + .05, data1.corrected_v2_20_40[i]);
    }
    corrected_v2_20_40_graph_1->Draw("AP");
    corrected_v2_20_40_graph_2->Draw("P SAME");
    corrected_v2_20_40_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_v2_20_40_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Signal Window Variations");
    corrected_v2_20_40_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    corrected_v2_20_40_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    corrected_v2_20_40_graph_1->SetMinimum(minYaxis_correctedComparisons);
    corrected_v2_20_40_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_20_40_v2_corrected_signalWindowVariation = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_20_40_v2_corrected_signalWindowVariation->SetBorderSize(0);
    legend_Overlay_20_40_v2_corrected_signalWindowVariation->SetTextSize(0.04);
    legend_Overlay_20_40_v2_corrected_signalWindowVariation->AddEntry(corrected_v2_20_40_graph_2, "Signal Window = #mu #pm 1.50#sigma", "pe");
    legend_Overlay_20_40_v2_corrected_signalWindowVariation->AddEntry(corrected_v2_20_40_graph_1, "Signal Window = #mu #pm 2.00#sigma (Default)", "pe");
    legend_Overlay_20_40_v2_corrected_signalWindowVariation->Draw();
    DrawZeroLine(c_Overlay_20_40_signalWindowVariation);
    TLegend *legSPHENIX_20_40_signalWindowVariation = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_20_40_signalWindowVariation->SetFillStyle(0);
    legSPHENIX_20_40_signalWindowVariation->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_20_40_signalWindowVariation->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_20_40_signalWindowVariation->AddEntry("","20-40% Centrality","");
    legSPHENIX_20_40_signalWindowVariation->Draw("same");
    c_Overlay_20_40_signalWindowVariation->Modified();
    c_Overlay_20_40_signalWindowVariation->Update();
    c_Overlay_20_40_signalWindowVariation->SaveAs((baseDataPath_SignalWindowVariations + "Overlay_20_40_v2_SignalWindowVariations.png").c_str());
    
    
    //40-60%
    TCanvas *c_Overlay_40_60_signalWindowVariation = new TCanvas("c_Overlay_40_60_signalWindowVariation", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_40_60_graph_2->SetPoint(i, ptCenters[i] - .05, data2.corrected_v2_40_60[i]);
        corrected_v2_40_60_graph_1->SetPoint(i, ptCenters[i] + .05, data1.corrected_v2_40_60[i]);
    }
    corrected_v2_40_60_graph_1->Draw("AP");
    corrected_v2_40_60_graph_2->Draw("P SAME");
    corrected_v2_40_60_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_v2_40_60_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Signal Window Variations");
    corrected_v2_40_60_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    corrected_v2_40_60_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    corrected_v2_40_60_graph_1->SetMinimum(minYaxis_correctedComparisons);
    corrected_v2_40_60_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_40_60_v2_corrected_signalWindowVariation = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_40_60_v2_corrected_signalWindowVariation->SetBorderSize(0);
    legend_Overlay_40_60_v2_corrected_signalWindowVariation->SetTextSize(0.04);
    legend_Overlay_40_60_v2_corrected_signalWindowVariation->AddEntry(corrected_v2_40_60_graph_2, "Signal Window = #mu #pm 1.50#sigma", "pe");
    legend_Overlay_40_60_v2_corrected_signalWindowVariation->AddEntry(corrected_v2_40_60_graph_1, "Signal Window = #mu #pm 2.00#sigma (Default)", "pe");
    legend_Overlay_40_60_v2_corrected_signalWindowVariation->Draw();
    DrawZeroLine(c_Overlay_40_60_signalWindowVariation);
    TLegend *legSPHENIX_40_60_signalWindowVariation = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_40_60_signalWindowVariation->SetFillStyle(0);
    legSPHENIX_40_60_signalWindowVariation->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_40_60_signalWindowVariation->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_40_60_signalWindowVariation->AddEntry("","40-60% Centrality","");
    legSPHENIX_40_60_signalWindowVariation->Draw("same");
    c_Overlay_40_60_signalWindowVariation->Modified();
    c_Overlay_40_60_signalWindowVariation->Update();
    c_Overlay_40_60_signalWindowVariation->SaveAs((baseDataPath_SignalWindowVariations + "Overlay_40_60_v2_SignalWindowVariations.png").c_str());
}


void plot_SampleSize_Variations(const Data& data1, Data& data2) {
    TGraphErrors* corrected_v2_0_20_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_0_20, data1.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_20_40, data1.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_40_60, data1.corrected_v2_40_60_Errors);
    
    TGraphErrors* corrected_v2_0_20_graph_2 = CreateGraph(ptCenters, data2.corrected_v2_0_20, data2.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_2 = CreateGraph(ptCenters, data2.corrected_v2_20_40, data2.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_2 = CreateGraph(ptCenters, data2.corrected_v2_40_60, data2.corrected_v2_40_60_Errors);

    
    double minYaxis_correctedComparisons = -0.2;
    double maxYaxis_correctedComparisons = 0.45;
    
    int markerColor_1 = kBlack;
    int markerStyle_1 = 20;
    double markerSize_1 = 1.0;
    
    int markerColor_2 =  kBlue;
    int markerStyle_2 = 20;
    double markerSize_2 = 1.0;
    
    
    
    corrected_v2_0_20_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_0_20_graph_1->SetLineColor(markerColor_1);
    corrected_v2_0_20_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_0_20_graph_1->SetMarkerStyle(markerStyle_1);
    
    corrected_v2_0_20_graph_2->SetMarkerColor(markerColor_2);
    corrected_v2_0_20_graph_2->SetLineColor(markerColor_2);
    corrected_v2_0_20_graph_2->SetMarkerSize(markerSize_2);
    corrected_v2_0_20_graph_2->SetMarkerStyle(markerStyle_2);
    
    
    corrected_v2_20_40_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_20_40_graph_1->SetLineColor(markerColor_1);
    corrected_v2_20_40_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_20_40_graph_1->SetMarkerStyle(markerStyle_1);
    
    corrected_v2_20_40_graph_2->SetMarkerColor(markerColor_2);
    corrected_v2_20_40_graph_2->SetLineColor(markerColor_2);
    corrected_v2_20_40_graph_2->SetMarkerSize(markerSize_2);
    corrected_v2_20_40_graph_2->SetMarkerStyle(markerStyle_2);

    corrected_v2_40_60_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_40_60_graph_1->SetLineColor(markerColor_1);
    corrected_v2_40_60_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_40_60_graph_1->SetMarkerStyle(markerStyle_1);
    
    corrected_v2_40_60_graph_2->SetMarkerColor(markerColor_2);
    corrected_v2_40_60_graph_2->SetLineColor(markerColor_2);
    corrected_v2_40_60_graph_2->SetMarkerSize(markerSize_2);
    corrected_v2_40_60_graph_2->SetMarkerStyle(markerStyle_2);
    
    
    

    /*
     Plot Signal Window Variations
     */
    //0-20%
    TCanvas *c_Overlay_0_20_sampleSizeVariation = new TCanvas("c_Overlay_0_20_sampleSizeVariation", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_0_20_graph_2->SetPoint(i, ptCenters[i] - .05, data2.corrected_v2_0_20[i]);
        corrected_v2_0_20_graph_1->SetPoint(i, ptCenters[i] + .05, data1.corrected_v2_0_20[i]);
    }
    corrected_v2_0_20_graph_1->Draw("AP");
    corrected_v2_0_20_graph_2->Draw("P SAME");
    corrected_v2_0_20_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_v2_0_20_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Signal Window Variations");
    corrected_v2_0_20_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    corrected_v2_0_20_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    corrected_v2_0_20_graph_1->SetMinimum(minYaxis_correctedComparisons);
    corrected_v2_0_20_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_0_20_v2_corrected_sampleSizeVariation = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_0_20_v2_corrected_sampleSizeVariation->SetBorderSize(0);
    legend_Overlay_0_20_v2_corrected_sampleSizeVariation->SetTextSize(0.04);
    legend_Overlay_0_20_v2_corrected_sampleSizeVariation->AddEntry(corrected_v2_0_20_graph_2, "Signal Window = #mu #pm 1.50#sigma", "pe");
    legend_Overlay_0_20_v2_corrected_sampleSizeVariation->AddEntry(corrected_v2_0_20_graph_1, "Signal Window = #mu #pm 2.00#sigma (Default)", "pe");
    legend_Overlay_0_20_v2_corrected_sampleSizeVariation->Draw();
    DrawZeroLine(c_Overlay_0_20_sampleSizeVariation);
    TLegend *legSPHENIX_0_20_sampleSizeVariation = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_0_20_sampleSizeVariation->SetFillStyle(0);
    legSPHENIX_0_20_sampleSizeVariation->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_0_20_sampleSizeVariation->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_0_20_sampleSizeVariation->AddEntry("","0-20% Centrality","");
    legSPHENIX_0_20_sampleSizeVariation->Draw("same");
    c_Overlay_0_20_sampleSizeVariation->Modified();
    c_Overlay_0_20_sampleSizeVariation->Update();
    c_Overlay_0_20_sampleSizeVariation->SaveAs((baseDataPath_SampleSizeVariations + "Overlay_0_20_v2_sampleSizeVariations.png").c_str());
    
    
    //20-40%
    TCanvas *c_Overlay_20_40_sampleSizeVariation = new TCanvas("c_Overlay_20_40_sampleSizeVariation", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_20_40_graph_2->SetPoint(i, ptCenters[i] - .05, data2.corrected_v2_20_40[i]);
        corrected_v2_20_40_graph_1->SetPoint(i, ptCenters[i] + .05, data1.corrected_v2_20_40[i]);
    }
    corrected_v2_20_40_graph_1->Draw("AP");
    corrected_v2_20_40_graph_2->Draw("P SAME");
    corrected_v2_20_40_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_v2_20_40_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Signal Window Variations");
    corrected_v2_20_40_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    corrected_v2_20_40_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    corrected_v2_20_40_graph_1->SetMinimum(minYaxis_correctedComparisons);
    corrected_v2_20_40_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_20_40_v2_corrected_sampleSizeVariation = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_20_40_v2_corrected_sampleSizeVariation->SetBorderSize(0);
    legend_Overlay_20_40_v2_corrected_sampleSizeVariation->SetTextSize(0.04);
    legend_Overlay_20_40_v2_corrected_sampleSizeVariation->AddEntry(corrected_v2_20_40_graph_2, "Signal Window = #mu #pm 1.50#sigma", "pe");
    legend_Overlay_20_40_v2_corrected_sampleSizeVariation->AddEntry(corrected_v2_20_40_graph_1, "Signal Window = #mu #pm 2.00#sigma (Default)", "pe");
    legend_Overlay_20_40_v2_corrected_sampleSizeVariation->Draw();
    DrawZeroLine(c_Overlay_20_40_sampleSizeVariation);
    TLegend *legSPHENIX_20_40_sampleSizeVariation = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_20_40_sampleSizeVariation->SetFillStyle(0);
    legSPHENIX_20_40_sampleSizeVariation->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_20_40_sampleSizeVariation->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_20_40_sampleSizeVariation->AddEntry("","20-40% Centrality","");
    legSPHENIX_20_40_sampleSizeVariation->Draw("same");
    c_Overlay_20_40_sampleSizeVariation->Modified();
    c_Overlay_20_40_sampleSizeVariation->Update();
    c_Overlay_20_40_sampleSizeVariation->SaveAs((baseDataPath_SampleSizeVariations + "Overlay_20_40_v2_sampleSizeVariations.png").c_str());
    
    
    //40-60%
    TCanvas *c_Overlay_40_60_sampleSizeVariation = new TCanvas("c_Overlay_40_60_sampleSizeVariation", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_40_60_graph_2->SetPoint(i, ptCenters[i] - .05, data2.corrected_v2_40_60[i]);
        corrected_v2_40_60_graph_1->SetPoint(i, ptCenters[i] + .05, data1.corrected_v2_40_60[i]);
    }
    corrected_v2_40_60_graph_1->Draw("AP");
    corrected_v2_40_60_graph_2->Draw("P SAME");
    corrected_v2_40_60_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_v2_40_60_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Signal Window Variations");
    corrected_v2_40_60_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    corrected_v2_40_60_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    corrected_v2_40_60_graph_1->SetMinimum(minYaxis_correctedComparisons);
    corrected_v2_40_60_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_40_60_v2_corrected_sampleSizeVariation = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_40_60_v2_corrected_sampleSizeVariation->SetBorderSize(0);
    legend_Overlay_40_60_v2_corrected_sampleSizeVariation->SetTextSize(0.04);
    legend_Overlay_40_60_v2_corrected_sampleSizeVariation->AddEntry(corrected_v2_40_60_graph_2, "Signal Window = #mu #pm 1.50#sigma", "pe");
    legend_Overlay_40_60_v2_corrected_sampleSizeVariation->AddEntry(corrected_v2_40_60_graph_1, "Signal Window = #mu #pm 2.00#sigma (Default)", "pe");
    legend_Overlay_40_60_v2_corrected_sampleSizeVariation->Draw();
    DrawZeroLine(c_Overlay_40_60_sampleSizeVariation);
    TLegend *legSPHENIX_40_60_sampleSizeVariation = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_40_60_sampleSizeVariation->SetFillStyle(0);
    legSPHENIX_40_60_sampleSizeVariation->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_40_60_sampleSizeVariation->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_40_60_sampleSizeVariation->AddEntry("","40-60% Centrality","");
    legSPHENIX_40_60_sampleSizeVariation->Draw("same");
    c_Overlay_40_60_sampleSizeVariation->Modified();
    c_Overlay_40_60_sampleSizeVariation->Update();
    c_Overlay_40_60_sampleSizeVariation->SaveAs((baseDataPath_SampleSizeVariations + "Overlay_40_60_v2_sampleSizeVariations.png").c_str());
}



void plot_AsymmetryCut_Variations(const Data& data1, Data& data2, Data& data3){
    TGraphErrors* corrected_v2_0_20_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_0_20, data1.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_20_40, data1.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_40_60, data1.corrected_v2_40_60_Errors);
    
    TGraphErrors* corrected_v2_0_20_graph_2 = CreateGraph(ptCenters, data2.corrected_v2_0_20, data2.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_2 = CreateGraph(ptCenters, data2.corrected_v2_20_40, data2.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_2 = CreateGraph(ptCenters, data2.corrected_v2_40_60, data2.corrected_v2_40_60_Errors);

    TGraphErrors* corrected_v2_0_20_graph_3 = CreateGraph(ptCenters, data3.corrected_v2_0_20, data3.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_3 = CreateGraph(ptCenters, data3.corrected_v2_20_40, data3.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_3 = CreateGraph(ptCenters, data3.corrected_v2_40_60, data3.corrected_v2_40_60_Errors);
    
    
    double minYaxis_correctedComparisons = -0.2;
    double maxYaxis_correctedComparisons = 0.45;
    
    int markerColor_1 = kBlack;
    int markerStyle_1 = 20;
    double markerSize_1 = 1.0;
    
    int markerColor_2 =  kBlue;
    int markerStyle_2 = 20;
    double markerSize_2 = 1.0;
    
    int markerColor_3 = kOrange+2;
    int markerStyle_3 = 20;
    double markerSize_3 = 1.2;
    

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
    

    
    /*
     0-20 percent v2 corrected
     */
    TCanvas *c_Overlay_0_20_AsymmetryCutVariation = new TCanvas("c_Overlay_0_20_AsymmetryCutVariation", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    corrected_v2_0_20_graph_1->Draw("AP");
    corrected_v2_0_20_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_0_20_graph_2->SetPoint(i, ptCenters[i] - .075, data2.corrected_v2_0_20[i]);
        corrected_v2_0_20_graph_3->SetPoint(i, ptCenters[i] + .075, data3.corrected_v2_0_20[i]);
    }
    corrected_v2_0_20_graph_2->Draw("P SAME");
    corrected_v2_0_20_graph_3->Draw("P SAME");
    corrected_v2_0_20_graph_2->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_v2_0_20_graph_3->GetXaxis()->SetLimits(2.0, 5.0);

    corrected_v2_0_20_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 0-20% Centrality, Sample Size Variations");
    corrected_v2_0_20_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    corrected_v2_0_20_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    corrected_v2_0_20_graph_1->SetMinimum(minYaxis_correctedComparisons);
    corrected_v2_0_20_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_0_20_v2_corrected_AsymVariations = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_0_20_v2_corrected_AsymVariations->SetBorderSize(0);
    legend_Overlay_0_20_v2_corrected_AsymVariations->SetTextSize(0.032);
    legend_Overlay_0_20_v2_corrected_AsymVariations->AddEntry(corrected_v2_0_20_graph_2, "Asymmetry < 0.45", "pe");
    legend_Overlay_0_20_v2_corrected_AsymVariations->AddEntry(corrected_v2_0_20_graph_1, "Asymmetry < 0.5 (Default)", "pe");
    legend_Overlay_0_20_v2_corrected_AsymVariations->AddEntry(corrected_v2_0_20_graph_3, "Asymmetry < 0.55", "pe");
    legend_Overlay_0_20_v2_corrected_AsymVariations->Draw();
    DrawZeroLine(c_Overlay_0_20_AsymmetryCutVariation);
    
    TLegend *legSPHENIX_0_20_AsymVariations = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_0_20_AsymVariations->SetFillStyle(0);
    legSPHENIX_0_20_AsymVariations->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_0_20_AsymVariations->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_0_20_AsymVariations->AddEntry("","0-20% Centrality","");
    legSPHENIX_0_20_AsymVariations->Draw("same");
    c_Overlay_0_20_AsymmetryCutVariation->Modified();
    c_Overlay_0_20_AsymmetryCutVariation->Update();
    c_Overlay_0_20_AsymmetryCutVariation->SaveAs((baseDataPath_AsymmetryCutVariations + "Overlay_0_20_v2_AsymVariation.png").c_str());
    
    
    //20-40
    TCanvas *c_Overlay_20_40_AsymmetryCutVariation = new TCanvas("c_Overlay_20_40_AsymmetryCutVariation", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 20-40% Centrality", 800, 600);
    corrected_v2_20_40_graph_1->Draw("AP");
    corrected_v2_20_40_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_20_40_graph_2->SetPoint(i, ptCenters[i] - .075, data2.corrected_v2_20_40[i]);
        corrected_v2_20_40_graph_3->SetPoint(i, ptCenters[i] + .075, data3.corrected_v2_20_40[i]);
    }
    corrected_v2_20_40_graph_2->Draw("P SAME");
    corrected_v2_20_40_graph_3->Draw("P SAME");
    corrected_v2_20_40_graph_2->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_v2_20_40_graph_3->GetXaxis()->SetLimits(2.0, 5.0);

    corrected_v2_20_40_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 20-40% Centrality, Sample Size Variations");
    corrected_v2_20_40_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    corrected_v2_20_40_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    corrected_v2_20_40_graph_1->SetMinimum(minYaxis_correctedComparisons);
    corrected_v2_20_40_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_20_40_v2_corrected_AsymVariations = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_20_40_v2_corrected_AsymVariations->SetBorderSize(0);
    legend_Overlay_20_40_v2_corrected_AsymVariations->SetTextSize(0.032);
    legend_Overlay_20_40_v2_corrected_AsymVariations->AddEntry(corrected_v2_20_40_graph_2, "Asymmetry < 0.45", "pe");
    legend_Overlay_20_40_v2_corrected_AsymVariations->AddEntry(corrected_v2_20_40_graph_1, "Asymmetry < 0.5 (Default)", "pe");
    legend_Overlay_20_40_v2_corrected_AsymVariations->AddEntry(corrected_v2_20_40_graph_3, "Asymmetry < 0.55", "pe");
    legend_Overlay_20_40_v2_corrected_AsymVariations->Draw();
    DrawZeroLine(c_Overlay_20_40_AsymmetryCutVariation);
    
    TLegend *legSPHENIX_20_40_AsymVariations = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_20_40_AsymVariations->SetFillStyle(0);
    legSPHENIX_20_40_AsymVariations->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_20_40_AsymVariations->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_20_40_AsymVariations->AddEntry("","20-40% Centrality","");
    legSPHENIX_20_40_AsymVariations->Draw("same");
    c_Overlay_20_40_AsymmetryCutVariation->Modified();
    c_Overlay_20_40_AsymmetryCutVariation->Update();
    c_Overlay_20_40_AsymmetryCutVariation->SaveAs((baseDataPath_AsymmetryCutVariations + "Overlay_20_40_v2_AsymVariation.png").c_str());
    
    
    //40-60
    TCanvas *c_Overlay_40_60_AsymmetryCutVariation = new TCanvas("c_Overlay_40_60_AsymmetryCutVariation", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    corrected_v2_40_60_graph_1->Draw("AP");
    corrected_v2_40_60_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_40_60_graph_2->SetPoint(i, ptCenters[i] - .075, data2.corrected_v2_40_60[i]);
        corrected_v2_40_60_graph_3->SetPoint(i, ptCenters[i] + .075, data3.corrected_v2_40_60[i]);
    }
    corrected_v2_40_60_graph_2->Draw("P SAME");
    corrected_v2_40_60_graph_3->Draw("P SAME");
    corrected_v2_40_60_graph_2->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_v2_40_60_graph_3->GetXaxis()->SetLimits(2.0, 5.0);

    corrected_v2_40_60_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Sample Size Variations");
    corrected_v2_40_60_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    corrected_v2_40_60_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    corrected_v2_40_60_graph_1->SetMinimum(minYaxis_correctedComparisons);
    corrected_v2_40_60_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_40_60_v2_corrected_AsymVariations = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_40_60_v2_corrected_AsymVariations->SetBorderSize(0);
    legend_Overlay_40_60_v2_corrected_AsymVariations->SetTextSize(0.032);
    legend_Overlay_40_60_v2_corrected_AsymVariations->AddEntry(corrected_v2_40_60_graph_2, "Asymmetry < 0.45", "pe");
    legend_Overlay_40_60_v2_corrected_AsymVariations->AddEntry(corrected_v2_40_60_graph_1, "Asymmetry < 0.5 (Default)", "pe");
    legend_Overlay_40_60_v2_corrected_AsymVariations->AddEntry(corrected_v2_40_60_graph_3, "Asymmetry < 0.55", "pe");
    legend_Overlay_40_60_v2_corrected_AsymVariations->Draw();
    DrawZeroLine(c_Overlay_40_60_AsymmetryCutVariation);
    
    TLegend *legSPHENIX_40_60_AsymVariations = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_40_60_AsymVariations->SetFillStyle(0);
    legSPHENIX_40_60_AsymVariations->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_40_60_AsymVariations->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_40_60_AsymVariations->AddEntry("","40-60% Centrality","");
    legSPHENIX_40_60_AsymVariations->Draw("same");
    c_Overlay_40_60_AsymmetryCutVariation->Modified();
    c_Overlay_40_60_AsymmetryCutVariation->Update();
    c_Overlay_40_60_AsymmetryCutVariation->SaveAs((baseDataPath_AsymmetryCutVariations + "Overlay_40_60_v2_AsymVariation.png").c_str());
}

void plot_sigBgCorr_and_bgWindowVariations(const Data& data1){
    /*
     Reference Data Set
     */
    TGraphErrors* corrected_v2_0_20_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_0_20, data1.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_20_40, data1.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_40_60, data1.corrected_v2_40_60_Errors);
    
    /*
     For Varied Sideband Calculation with 0.4 Upper Bound -- Default is set at 0.5 GeV
     */
    
    TGraphErrors* corrected_v2_0_20_graph_1_type4 = CreateGraph(ptCenters, data1.corrected_v2_0_20_type4, data1.corrected_v2_0_20_Errors_type4);
    TGraphErrors* corrected_v2_20_40_graph_1_type4 = CreateGraph(ptCenters, data1.corrected_v2_20_40_type4, data1.corrected_v2_20_40_Errors_type4);
    TGraphErrors* corrected_v2_40_60_graph_1_type4 = CreateGraph(ptCenters, data1.corrected_v2_40_60_type4, data1.corrected_v2_40_60_Errors_type4);
    
    /*
     Measured v2
     */
    TGraphErrors* signal_v2_0_20_graph_1 = CreateGraph(ptCenters, data1.signal_v2_0_20, data1.signal_v2_0_20_Errors);
    TGraphErrors* signal_v2_20_40_graph_1 = CreateGraph(ptCenters, data1.signal_v2_20_40, data1.signal_v2_20_40_Errors);
    TGraphErrors* signal_v2_40_60_graph_1 = CreateGraph(ptCenters, data1.signal_v2_40_60, data1.signal_v2_40_60_Errors);
    
    /*
     Background v2
     */
    TGraphErrors* bg_v2_0_20_graph_1 = CreateGraph(ptCenters, data1.bg_v2_0_20, data1.bg_v2_0_20_Errors);
    TGraphErrors* bg_v2_20_40_graph_1 = CreateGraph(ptCenters, data1.bg_v2_20_40, data1.bg_v2_20_40_Errors);
    TGraphErrors* bg_v2_40_60_graph_1 = CreateGraph(ptCenters, data1.bg_v2_40_60, data1.bg_v2_40_60_Errors);
    

    
    
    double minYaxis_correctedComparisons = -0.2;
    double maxYaxis_correctedComparisons = 0.45;
    
    int markerColor_1 = kBlack;
    int markerStyle_1 = 20;
    double markerSize_1 = 1.0;
    
    int markerColor_2 =  kBlue;
    int markerStyle_2 = 20;
    double markerSize_2 = 1.0;
    
    int markerColor_3 = kOrange+2;
    int markerStyle_3 = 20;
    double markerSize_3 = 1.2;
    
    int markerColor_4 = kGreen+3;
    int markerStyle_4 = 20;
    double markerSize_4 = 1.2;
    
    int markerColor_5 = kRed;
    int markerStyle_5 = 20;
    double markerSize_5 = 1.0;
    
    int markerColor_6 = kViolet - 2;
    int markerStyle_6 = 20;
    double markerSize_6 = 1.0;
    
    
    corrected_v2_0_20_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_0_20_graph_1->SetLineColor(markerColor_1);
    corrected_v2_0_20_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_0_20_graph_1->SetMarkerStyle(markerStyle_1);
    
    corrected_v2_0_20_graph_1_type4->SetMarkerColor(markerColor_2);
    corrected_v2_0_20_graph_1_type4->SetLineColor(markerColor_2);
    corrected_v2_0_20_graph_1_type4->SetMarkerSize(markerSize_1);
    corrected_v2_0_20_graph_1_type4->SetMarkerStyle(markerStyle_1);
    
    bg_v2_0_20_graph_1->SetMarkerColor(markerColor_2);
    bg_v2_0_20_graph_1->SetLineColor(markerColor_2);
    bg_v2_0_20_graph_1->SetMarkerSize(markerSize_2);
    bg_v2_0_20_graph_1->SetMarkerStyle(markerStyle_2);
    
    signal_v2_0_20_graph_1->SetMarkerColor(markerColor_3);
    signal_v2_0_20_graph_1->SetLineColor(markerColor_3);
    signal_v2_0_20_graph_1->SetMarkerSize(markerSize_3);
    signal_v2_0_20_graph_1->SetMarkerStyle(markerStyle_3);
    
    
    corrected_v2_20_40_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_20_40_graph_1->SetLineColor(markerColor_1);
    corrected_v2_20_40_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_20_40_graph_1->SetMarkerStyle(markerStyle_1);
    
    corrected_v2_20_40_graph_1_type4->SetMarkerColor(markerColor_2);
    corrected_v2_20_40_graph_1_type4->SetLineColor(markerColor_2);
    corrected_v2_20_40_graph_1_type4->SetMarkerSize(markerSize_1);
    corrected_v2_20_40_graph_1_type4->SetMarkerStyle(markerStyle_1);
    
    bg_v2_20_40_graph_1->SetMarkerColor(markerColor_2);
    bg_v2_20_40_graph_1->SetLineColor(markerColor_2);
    bg_v2_20_40_graph_1->SetMarkerSize(markerSize_2);
    bg_v2_20_40_graph_1->SetMarkerStyle(markerStyle_2);
    
    signal_v2_20_40_graph_1->SetMarkerColor(markerColor_3);
    signal_v2_20_40_graph_1->SetLineColor(markerColor_3);
    signal_v2_20_40_graph_1->SetMarkerSize(markerSize_3);
    signal_v2_20_40_graph_1->SetMarkerStyle(markerStyle_3);
    
    
    
    corrected_v2_40_60_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_40_60_graph_1->SetLineColor(markerColor_1);
    corrected_v2_40_60_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_40_60_graph_1->SetMarkerStyle(markerStyle_1);
    
    corrected_v2_40_60_graph_1_type4->SetMarkerColor(markerColor_2);
    corrected_v2_40_60_graph_1_type4->SetLineColor(markerColor_2);
    corrected_v2_40_60_graph_1_type4->SetMarkerSize(markerSize_1);
    corrected_v2_40_60_graph_1_type4->SetMarkerStyle(markerStyle_1);
    

    bg_v2_40_60_graph_1->SetMarkerColor(markerColor_2);
    bg_v2_40_60_graph_1->SetLineColor(markerColor_2);
    bg_v2_40_60_graph_1->SetMarkerSize(markerSize_2);
    bg_v2_40_60_graph_1->SetMarkerStyle(markerStyle_2);
    
    signal_v2_40_60_graph_1->SetMarkerColor(markerColor_3);
    signal_v2_40_60_graph_1->SetLineColor(markerColor_3);
    signal_v2_40_60_graph_1->SetMarkerSize(markerSize_3);
    signal_v2_40_60_graph_1->SetMarkerStyle(markerStyle_3);
    
    
    /*
     Plot Background Window Variations
     */
    //0-20%
    TCanvas *c_Overlay_0_20_bgWindowVariation = new TCanvas("c_Overlay_0_20_bgWindowVariation", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_0_20_graph_1_type4->SetPoint(i, ptCenters[i] - .05, data1.corrected_v2_0_20_type4[i]);
        corrected_v2_0_20_graph_1->SetPoint(i, ptCenters[i] + .05, data1.corrected_v2_0_20[i]);
    }
    corrected_v2_0_20_graph_1->Draw("AP");
    corrected_v2_0_20_graph_1_type4->Draw("P SAME");
    corrected_v2_0_20_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_v2_0_20_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Signal Window Variations");
    corrected_v2_0_20_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    corrected_v2_0_20_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    corrected_v2_0_20_graph_1->SetMinimum(minYaxis_correctedComparisons);
    corrected_v2_0_20_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_0_20_v2_corrected_sideBandVariation = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_0_20_v2_corrected_sideBandVariation->SetBorderSize(0);
    legend_Overlay_0_20_v2_corrected_sideBandVariation->SetTextSize(0.032);
    legend_Overlay_0_20_v2_corrected_sideBandVariation->AddEntry(corrected_v2_0_20_graph_1_type4, "Sideband = [#mu + 3#sigma, 0.4] GeV", "pe");
    legend_Overlay_0_20_v2_corrected_sideBandVariation->AddEntry(corrected_v2_0_20_graph_1, "Sideband = [#mu + 3#sigma, 0.5] GeV (Default)", "pe");
    legend_Overlay_0_20_v2_corrected_sideBandVariation->Draw();
    DrawZeroLine(c_Overlay_0_20_bgWindowVariation);
    
    TLegend *legSPHENIX_0_20_sideBandVariation = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_0_20_sideBandVariation->SetTextSize(0.045);
    legSPHENIX_0_20_sideBandVariation->SetFillStyle(0);
    legSPHENIX_0_20_sideBandVariation->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_0_20_sideBandVariation->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_0_20_sideBandVariation->AddEntry("","0-20% Centrality","");
    legSPHENIX_0_20_sideBandVariation->Draw("same");
    c_Overlay_0_20_bgWindowVariation->Modified();
    c_Overlay_0_20_bgWindowVariation->Update();
    c_Overlay_0_20_bgWindowVariation->SaveAs((baseDataPath_BackgroundWindowVariations + "Overlay_0_20_v2_BackgroundWindowVariations.png").c_str());
    
    
    //20-40%
    TCanvas *c_Overlay_20_40_bgWindowVariation = new TCanvas("c_Overlay_20_40_bgWindowVariation", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_20_40_graph_1_type4->SetPoint(i, ptCenters[i] - .05, data1.corrected_v2_20_40_type4[i]);
        corrected_v2_20_40_graph_1->SetPoint(i, ptCenters[i] + .05, data1.corrected_v2_20_40[i]);
    }
    corrected_v2_20_40_graph_1->Draw("AP");
    corrected_v2_20_40_graph_1_type4->Draw("P SAME");
    corrected_v2_20_40_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_v2_20_40_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Signal Window Variations");
    corrected_v2_20_40_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    corrected_v2_20_40_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    corrected_v2_20_40_graph_1->SetMinimum(minYaxis_correctedComparisons);
    corrected_v2_20_40_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_20_40_v2_corrected_sideBandVariation = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_20_40_v2_corrected_sideBandVariation->SetBorderSize(0);
    legend_Overlay_20_40_v2_corrected_sideBandVariation->SetTextSize(0.032);
    legend_Overlay_20_40_v2_corrected_sideBandVariation->AddEntry(corrected_v2_20_40_graph_1_type4, "Sideband = [#mu + 3#sigma, 0.4] GeV", "pe");
    legend_Overlay_20_40_v2_corrected_sideBandVariation->AddEntry(corrected_v2_20_40_graph_1, "Sideband = [#mu + 3#sigma, 0.5] GeV (Default)", "pe");
    legend_Overlay_20_40_v2_corrected_sideBandVariation->Draw();
    DrawZeroLine(c_Overlay_20_40_bgWindowVariation);
    
    TLegend *legSPHENIX_20_40_sideBandVariation = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_20_40_sideBandVariation->SetTextSize(0.045);
    legSPHENIX_20_40_sideBandVariation->SetFillStyle(0);
    legSPHENIX_20_40_sideBandVariation->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_20_40_sideBandVariation->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_20_40_sideBandVariation->AddEntry("","20-40% Centrality","");
    legSPHENIX_20_40_sideBandVariation->Draw("same");
    c_Overlay_20_40_bgWindowVariation->Modified();
    c_Overlay_20_40_bgWindowVariation->Update();
    c_Overlay_20_40_bgWindowVariation->SaveAs((baseDataPath_BackgroundWindowVariations + "Overlay_20_40_v2_BackgroundWindowVariations.png").c_str());
    
    
    //40-60%
    TCanvas *c_Overlay_40_60_bgWindowVariation = new TCanvas("c_Overlay_40_60_bgWindowVariation", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_40_60_graph_1_type4->SetPoint(i, ptCenters[i] - .05, data1.corrected_v2_40_60_type4[i]);
        corrected_v2_40_60_graph_1->SetPoint(i, ptCenters[i] + .05, data1.corrected_v2_40_60[i]);
    }
    corrected_v2_40_60_graph_1->Draw("AP");
    corrected_v2_40_60_graph_1_type4->Draw("P SAME");
    corrected_v2_40_60_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_v2_40_60_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Signal Window Variations");
    corrected_v2_40_60_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    corrected_v2_40_60_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    corrected_v2_40_60_graph_1->SetMinimum(minYaxis_correctedComparisons);
    corrected_v2_40_60_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_40_60_v2_corrected_sideBandVariation = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_40_60_v2_corrected_sideBandVariation->SetBorderSize(0);
    legend_Overlay_40_60_v2_corrected_sideBandVariation->SetTextSize(0.032);
    legend_Overlay_40_60_v2_corrected_sideBandVariation->AddEntry(corrected_v2_40_60_graph_1_type4, "Sideband = [#mu + 3#sigma, 0.4] GeV", "pe");
    legend_Overlay_40_60_v2_corrected_sideBandVariation->AddEntry(corrected_v2_40_60_graph_1, "Sideband = [#mu + 3#sigma, 0.5] GeV (Default)", "pe");
    legend_Overlay_40_60_v2_corrected_sideBandVariation->Draw();
    DrawZeroLine(c_Overlay_40_60_bgWindowVariation);
    
    TLegend *legSPHENIX_40_60_sideBandVariation = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_40_60_sideBandVariation->SetTextSize(0.045);
    legSPHENIX_40_60_sideBandVariation->SetFillStyle(0);
    legSPHENIX_40_60_sideBandVariation->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_40_60_sideBandVariation->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_40_60_sideBandVariation->AddEntry("","40-60% Centrality","");
    legSPHENIX_40_60_sideBandVariation->Draw("same");
    c_Overlay_40_60_bgWindowVariation->Modified();
    c_Overlay_40_60_bgWindowVariation->Update();
    c_Overlay_40_60_bgWindowVariation->SaveAs((baseDataPath_BackgroundWindowVariations + "Overlay_40_60_v2_BackgroundWindowVariations.png").c_str());
    
    
    
    
    /*
     Plot Signal, Background Corrected v2 Overlay with Default Data
     */
    //0-20%
    TCanvas *c_Overlay_bg_sig_corrOverlay_0_20 = new TCanvas("c_Overlay_bg_sig_corrOverlay_0_20", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    bg_v2_0_20_graph_1->Draw("AP");
    bg_v2_0_20_graph_1->SetTitle("Measured, Background, Corrected #it{v}_{2} vs #it{p}_{T} 0-20% Centrality");
    bg_v2_0_20_graph_1->GetXaxis()->SetTitle("p_{T}");
    bg_v2_0_20_graph_1->GetYaxis()->SetTitle("v_{2}");
    for (int i = 0; i < ptCenters.size(); ++i) {
        signal_v2_0_20_graph_1->SetPoint(i, ptCenters[i] - 2.0 * offset, data1.signal_v2_0_20[i]);
        corrected_v2_0_20_graph_1->SetPoint(i, ptCenters[i] + 2.0 * offset, data1.corrected_v2_0_20[i]);
    }
    signal_v2_0_20_graph_1 -> Draw("P SAME");
    corrected_v2_0_20_graph_1 -> Draw("P SAME");
    bg_v2_0_20_graph_1->SetMinimum(minYaxis_correctedComparisons);
    bg_v2_0_20_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_0_20_bg_sig_corrOverlay = new TLegend(0.2, 0.19, 0.4, 0.39);
    legend_Overlay_0_20_bg_sig_corrOverlay->SetBorderSize(0);
    legend_Overlay_0_20_bg_sig_corrOverlay->SetTextSize(.03);
    legend_Overlay_0_20_bg_sig_corrOverlay->AddEntry(signal_v2_0_20_graph_1, "#it{v}_{2}^{measured}", "pe");
    legend_Overlay_0_20_bg_sig_corrOverlay->AddEntry(bg_v2_0_20_graph_1, "#it{v}_{2}^{background}", "pe");
    legend_Overlay_0_20_bg_sig_corrOverlay->AddEntry(corrected_v2_0_20_graph_1, "#it{v}_{2}^{#pi^{0}}", "pe");
    legend_Overlay_0_20_bg_sig_corrOverlay->Draw();
    TLegend *leg1_bg_sig_corr_overlay_0_20 = new TLegend(0.14,.72,0.34,.92);
    leg1_bg_sig_corr_overlay_0_20->SetFillStyle(0);
    leg1_bg_sig_corr_overlay_0_20->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg1_bg_sig_corr_overlay_0_20->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg1_bg_sig_corr_overlay_0_20->AddEntry("","0-20% Centrality","");
    leg1_bg_sig_corr_overlay_0_20->Draw("same");
    leg1_bg_sig_corr_overlay_0_20->SetFillStyle(0);
    DrawZeroLine(c_Overlay_bg_sig_corrOverlay_0_20);
    c_Overlay_bg_sig_corrOverlay_0_20->Modified();
    c_Overlay_bg_sig_corrOverlay_0_20->Update();
    c_Overlay_bg_sig_corrOverlay_0_20->SaveAs((BasePlotOutputPath + "/Overlay_0_20_v2_Signal_Back_Correct.png").c_str());
    
    //20-40
    TCanvas *c_Overlay_bg_sig_corrOverlay_20_40 = new TCanvas("c_Overlay_bg_sig_corrOverlay_20_40", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    bg_v2_20_40_graph_1->Draw("AP");
    bg_v2_20_40_graph_1->SetTitle("Measured, Background, Corrected #it{v}_{2} vs #it{p}_{T} 0-20% Centrality");
    bg_v2_20_40_graph_1->GetXaxis()->SetTitle("p_{T}");
    bg_v2_20_40_graph_1->GetYaxis()->SetTitle("v_{2}");
    for (int i = 0; i < ptCenters.size(); ++i) {
        signal_v2_20_40_graph_1->SetPoint(i, ptCenters[i] - 2.0 * offset, data1.signal_v2_20_40[i]);
        corrected_v2_20_40_graph_1->SetPoint(i, ptCenters[i] + 2.0 * offset, data1.corrected_v2_20_40[i]);
    }
    signal_v2_20_40_graph_1 -> Draw("P SAME");
    corrected_v2_20_40_graph_1 -> Draw("P SAME");
    bg_v2_20_40_graph_1->SetMinimum(minYaxis_correctedComparisons);
    bg_v2_20_40_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_20_40_bg_sig_corrOverlay = new TLegend(0.2, 0.19, 0.4, 0.39);
    legend_Overlay_20_40_bg_sig_corrOverlay->SetBorderSize(0);
    legend_Overlay_20_40_bg_sig_corrOverlay->SetTextSize(.03);
    legend_Overlay_20_40_bg_sig_corrOverlay->AddEntry(signal_v2_20_40_graph_1, "#it{v}_{2}^{measured}", "pe");
    legend_Overlay_20_40_bg_sig_corrOverlay->AddEntry(bg_v2_20_40_graph_1, "#it{v}_{2}^{background}", "pe");
    legend_Overlay_20_40_bg_sig_corrOverlay->AddEntry(corrected_v2_20_40_graph_1, "#it{v}_{2}^{#pi^{0}}", "pe");
    legend_Overlay_20_40_bg_sig_corrOverlay->Draw();
    TLegend *leg1_bg_sig_corr_overlay_20_40 = new TLegend(0.14,.72,0.34,.92);
    leg1_bg_sig_corr_overlay_20_40->SetFillStyle(0);
    leg1_bg_sig_corr_overlay_20_40->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg1_bg_sig_corr_overlay_20_40->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg1_bg_sig_corr_overlay_20_40->AddEntry("","20-40% Centrality","");
    leg1_bg_sig_corr_overlay_20_40->Draw("same");
    leg1_bg_sig_corr_overlay_20_40->SetFillStyle(0);
    DrawZeroLine(c_Overlay_bg_sig_corrOverlay_20_40);
    c_Overlay_bg_sig_corrOverlay_20_40->Modified();
    c_Overlay_bg_sig_corrOverlay_20_40->Update();
    c_Overlay_bg_sig_corrOverlay_20_40->SaveAs((BasePlotOutputPath + "/Overlay_20_40_v2_Signal_Back_Correct.png").c_str());
    
    
    //40-60
    TCanvas *c_Overlay_bg_sig_corrOverlay_40_60 = new TCanvas("c_Overlay_bg_sig_corrOverlay_40_60", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    bg_v2_40_60_graph_1->Draw("AP");
    bg_v2_40_60_graph_1->SetTitle("Measured, Background, Corrected #it{v}_{2} vs #it{p}_{T} 0-20% Centrality");
    bg_v2_40_60_graph_1->GetXaxis()->SetTitle("p_{T}");
    bg_v2_40_60_graph_1->GetYaxis()->SetTitle("v_{2}");
    for (int i = 0; i < ptCenters.size(); ++i) {
        signal_v2_40_60_graph_1->SetPoint(i, ptCenters[i] - 2.0 * offset, data1.signal_v2_40_60[i]);
        corrected_v2_40_60_graph_1->SetPoint(i, ptCenters[i] + 2.0 * offset, data1.corrected_v2_40_60[i]);
    }
    signal_v2_40_60_graph_1 -> Draw("P SAME");
    corrected_v2_40_60_graph_1 -> Draw("P SAME");
    bg_v2_40_60_graph_1->SetMinimum(minYaxis_correctedComparisons);
    bg_v2_40_60_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_40_60_bg_sig_corrOverlay = new TLegend(0.2, 0.19, 0.4, 0.39);
    legend_Overlay_40_60_bg_sig_corrOverlay->SetBorderSize(0);
    legend_Overlay_40_60_bg_sig_corrOverlay->SetTextSize(.03);
    legend_Overlay_40_60_bg_sig_corrOverlay->AddEntry(signal_v2_40_60_graph_1, "#it{v}_{2}^{measured}", "pe");
    legend_Overlay_40_60_bg_sig_corrOverlay->AddEntry(bg_v2_40_60_graph_1, "#it{v}_{2}^{background}", "pe");
    legend_Overlay_40_60_bg_sig_corrOverlay->AddEntry(corrected_v2_40_60_graph_1, "#it{v}_{2}^{#pi^{0}}", "pe");
    legend_Overlay_40_60_bg_sig_corrOverlay->Draw();
    TLegend *leg1_bg_sig_corr_overlay_40_60 = new TLegend(0.5,.18,0.6,.38);
    leg1_bg_sig_corr_overlay_40_60->SetFillStyle(0);
    leg1_bg_sig_corr_overlay_40_60->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg1_bg_sig_corr_overlay_40_60->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    leg1_bg_sig_corr_overlay_40_60->AddEntry("","40-60% Centrality","");
    leg1_bg_sig_corr_overlay_40_60->Draw("same");
    DrawZeroLine(c_Overlay_bg_sig_corrOverlay_40_60);
    c_Overlay_bg_sig_corrOverlay_40_60->Modified();
    c_Overlay_bg_sig_corrOverlay_40_60->Update();
    c_Overlay_bg_sig_corrOverlay_40_60->SaveAs((BasePlotOutputPath + "/Overlay_40_60_v2_Signal_Back_Correct.png").c_str());
}


void plotProduction_Comparisons(const Data& data1, Data& data2) {
    TGraphErrors* corrected_v2_0_20_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_0_20, data1.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_20_40, data1.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_1 = CreateGraph(ptCenters, data1.corrected_v2_40_60, data1.corrected_v2_40_60_Errors);
    
    TGraphErrors* corrected_v2_0_20_graph_2 = CreateGraph(ptCenters, data2.corrected_v2_0_20, data2.corrected_v2_0_20_Errors);
    TGraphErrors* corrected_v2_20_40_graph_2 = CreateGraph(ptCenters, data2.corrected_v2_20_40, data2.corrected_v2_20_40_Errors);
    TGraphErrors* corrected_v2_40_60_graph_2 = CreateGraph(ptCenters, data2.corrected_v2_40_60, data2.corrected_v2_40_60_Errors);
    /*
     Measured v2
     */
    TGraphErrors* signal_v2_0_20_graph_1 = CreateGraph(ptCenters, data1.signal_v2_0_20, data1.signal_v2_0_20_Errors);
    TGraphErrors* signal_v2_20_40_graph_1 = CreateGraph(ptCenters, data1.signal_v2_20_40, data1.signal_v2_20_40_Errors);
    TGraphErrors* signal_v2_40_60_graph_1 = CreateGraph(ptCenters, data1.signal_v2_40_60, data1.signal_v2_40_60_Errors);
    
    TGraphErrors* signal_v2_0_20_graph_2 = CreateGraph(ptCenters, data2.signal_v2_0_20, data2.signal_v2_0_20_Errors);
    TGraphErrors* signal_v2_20_40_graph_2 = CreateGraph(ptCenters, data2.signal_v2_20_40, data2.signal_v2_20_40_Errors);
    TGraphErrors* signal_v2_40_60_graph_2 = CreateGraph(ptCenters, data2.signal_v2_40_60, data2.signal_v2_40_60_Errors);
    
    /*
     Background v2
     */
    TGraphErrors* bg_v2_0_20_graph_1 = CreateGraph(ptCenters, data1.bg_v2_0_20, data1.bg_v2_0_20_Errors);
    TGraphErrors* bg_v2_20_40_graph_1 = CreateGraph(ptCenters, data1.bg_v2_20_40, data1.bg_v2_20_40_Errors);
    TGraphErrors* bg_v2_40_60_graph_1 = CreateGraph(ptCenters, data1.bg_v2_40_60, data1.bg_v2_40_60_Errors);
    
    TGraphErrors* bg_v2_0_20_graph_2 = CreateGraph(ptCenters, data2.bg_v2_0_20, data2.bg_v2_0_20_Errors);
    TGraphErrors* bg_v2_20_40_graph_2 = CreateGraph(ptCenters, data2.bg_v2_20_40, data2.bg_v2_20_40_Errors);
    TGraphErrors* bg_v2_40_60_graph_2 = CreateGraph(ptCenters, data2.bg_v2_40_60, data2.bg_v2_40_60_Errors);

    
    double minYaxis_correctedComparisons = -0.2;
    double maxYaxis_correctedComparisons = 0.45;
    
    int markerColor_1 = kBlack;
    int markerStyle_1 = 20;
    double markerSize_1 = 1.0;
    
    int markerColor_2 =  kBlue;
    int markerStyle_2 = 20;
    double markerSize_2 = 1.0;
    
    
    
    corrected_v2_0_20_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_0_20_graph_1->SetLineColor(markerColor_1);
    corrected_v2_0_20_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_0_20_graph_1->SetMarkerStyle(markerStyle_1);
    
    corrected_v2_0_20_graph_2->SetMarkerColor(markerColor_2);
    corrected_v2_0_20_graph_2->SetLineColor(markerColor_2);
    corrected_v2_0_20_graph_2->SetMarkerSize(markerSize_2);
    corrected_v2_0_20_graph_2->SetMarkerStyle(markerStyle_2);
    
    
    corrected_v2_20_40_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_20_40_graph_1->SetLineColor(markerColor_1);
    corrected_v2_20_40_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_20_40_graph_1->SetMarkerStyle(markerStyle_1);
    
    corrected_v2_20_40_graph_2->SetMarkerColor(markerColor_2);
    corrected_v2_20_40_graph_2->SetLineColor(markerColor_2);
    corrected_v2_20_40_graph_2->SetMarkerSize(markerSize_2);
    corrected_v2_20_40_graph_2->SetMarkerStyle(markerStyle_2);

    corrected_v2_40_60_graph_1->SetMarkerColor(markerColor_1);
    corrected_v2_40_60_graph_1->SetLineColor(markerColor_1);
    corrected_v2_40_60_graph_1->SetMarkerSize(markerSize_1);
    corrected_v2_40_60_graph_1->SetMarkerStyle(markerStyle_1);
    
    corrected_v2_40_60_graph_2->SetMarkerColor(markerColor_2);
    corrected_v2_40_60_graph_2->SetLineColor(markerColor_2);
    corrected_v2_40_60_graph_2->SetMarkerSize(markerSize_2);
    corrected_v2_40_60_graph_2->SetMarkerStyle(markerStyle_2);
    
    
    
    signal_v2_0_20_graph_1->SetMarkerColor(markerColor_1);
    signal_v2_0_20_graph_1->SetLineColor(markerColor_1);
    signal_v2_0_20_graph_1->SetMarkerSize(markerSize_1);
    signal_v2_0_20_graph_1->SetMarkerStyle(markerStyle_1);
    
    signal_v2_0_20_graph_2->SetMarkerColor(markerColor_2);
    signal_v2_0_20_graph_2->SetLineColor(markerColor_2);
    signal_v2_0_20_graph_2->SetMarkerSize(markerSize_2);
    signal_v2_0_20_graph_2->SetMarkerStyle(markerStyle_2);
    
    
    signal_v2_20_40_graph_1->SetMarkerColor(markerColor_1);
    signal_v2_20_40_graph_1->SetLineColor(markerColor_1);
    signal_v2_20_40_graph_1->SetMarkerSize(markerSize_1);
    signal_v2_20_40_graph_1->SetMarkerStyle(markerStyle_1);
    
    signal_v2_20_40_graph_2->SetMarkerColor(markerColor_2);
    signal_v2_20_40_graph_2->SetLineColor(markerColor_2);
    signal_v2_20_40_graph_2->SetMarkerSize(markerSize_2);
    signal_v2_20_40_graph_2->SetMarkerStyle(markerStyle_2);

    signal_v2_40_60_graph_1->SetMarkerColor(markerColor_1);
    signal_v2_40_60_graph_1->SetLineColor(markerColor_1);
    signal_v2_40_60_graph_1->SetMarkerSize(markerSize_1);
    signal_v2_40_60_graph_1->SetMarkerStyle(markerStyle_1);
    
    signal_v2_40_60_graph_2->SetMarkerColor(markerColor_2);
    signal_v2_40_60_graph_2->SetLineColor(markerColor_2);
    signal_v2_40_60_graph_2->SetMarkerSize(markerSize_2);
    signal_v2_40_60_graph_2->SetMarkerStyle(markerStyle_2);
    
    
    bg_v2_0_20_graph_1->SetMarkerColor(markerColor_1);
    bg_v2_0_20_graph_1->SetLineColor(markerColor_1);
    bg_v2_0_20_graph_1->SetMarkerSize(markerSize_1);
    bg_v2_0_20_graph_1->SetMarkerStyle(markerStyle_1);
    
    bg_v2_0_20_graph_2->SetMarkerColor(markerColor_2);
    bg_v2_0_20_graph_2->SetLineColor(markerColor_2);
    bg_v2_0_20_graph_2->SetMarkerSize(markerSize_2);
    bg_v2_0_20_graph_2->SetMarkerStyle(markerStyle_2);
    
    
    bg_v2_20_40_graph_1->SetMarkerColor(markerColor_1);
    bg_v2_20_40_graph_1->SetLineColor(markerColor_1);
    bg_v2_20_40_graph_1->SetMarkerSize(markerSize_1);
    bg_v2_20_40_graph_1->SetMarkerStyle(markerStyle_1);
    
    bg_v2_20_40_graph_2->SetMarkerColor(markerColor_2);
    bg_v2_20_40_graph_2->SetLineColor(markerColor_2);
    bg_v2_20_40_graph_2->SetMarkerSize(markerSize_2);
    bg_v2_20_40_graph_2->SetMarkerStyle(markerStyle_2);

    bg_v2_40_60_graph_1->SetMarkerColor(markerColor_1);
    bg_v2_40_60_graph_1->SetLineColor(markerColor_1);
    bg_v2_40_60_graph_1->SetMarkerSize(markerSize_1);
    bg_v2_40_60_graph_1->SetMarkerStyle(markerStyle_1);
    
    bg_v2_40_60_graph_2->SetMarkerColor(markerColor_2);
    bg_v2_40_60_graph_2->SetLineColor(markerColor_2);
    bg_v2_40_60_graph_2->SetMarkerSize(markerSize_2);
    bg_v2_40_60_graph_2->SetMarkerStyle(markerStyle_2);
    
    
    
    //0-20%
    TCanvas *c_Overlay_0_20_productionComparisons_corrected = new TCanvas("c_Overlay_0_20_productionComparisons_corrected", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_0_20_graph_2->SetPoint(i, ptCenters[i] - .05, data2.corrected_v2_0_20[i]);
        corrected_v2_0_20_graph_1->SetPoint(i, ptCenters[i] + .05, data1.corrected_v2_0_20[i]);
    }
    corrected_v2_0_20_graph_1->Draw("AP");
    corrected_v2_0_20_graph_2->Draw("P SAME");
    corrected_v2_0_20_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_v2_0_20_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Signal Window Variations");
    corrected_v2_0_20_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    corrected_v2_0_20_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    corrected_v2_0_20_graph_1->SetMinimum(minYaxis_correctedComparisons);
    corrected_v2_0_20_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_0_20_v2_corrected_productionComparisons = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_0_20_v2_corrected_productionComparisons->SetBorderSize(0);
    legend_Overlay_0_20_v2_corrected_productionComparisons->SetTextSize(0.04);
    legend_Overlay_0_20_v2_corrected_productionComparisons->AddEntry(corrected_v2_0_20_graph_2, "p013 v_{2}^{#pi^{0}}", "pe");
    legend_Overlay_0_20_v2_corrected_productionComparisons->AddEntry(corrected_v2_0_20_graph_1, "p015 v_{2}^{#pi^{0}}", "pe");
    legend_Overlay_0_20_v2_corrected_productionComparisons->Draw();
    DrawZeroLine(c_Overlay_0_20_productionComparisons_corrected);
    TLegend *legSPHENIX_0_20_productionComparisons_corrected = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_0_20_productionComparisons_corrected->SetFillStyle(0);
    legSPHENIX_0_20_productionComparisons_corrected->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_0_20_productionComparisons_corrected->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_0_20_productionComparisons_corrected->AddEntry("","0-20% Centrality","");
    legSPHENIX_0_20_productionComparisons_corrected->Draw("same");
    c_Overlay_0_20_productionComparisons_corrected->Modified();
    c_Overlay_0_20_productionComparisons_corrected->Update();
    c_Overlay_0_20_productionComparisons_corrected->SaveAs("/Users/patsfan753/Desktop/Overlay_0_20_v2_p013_p015_correctedV2Comparison.png");
    
    
    
    
    TCanvas *c_Overlay_20_40_productionComparisons_corrected = new TCanvas("c_Overlay_20_40_productionComparisons_corrected", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_20_40_graph_2->SetPoint(i, ptCenters[i] - .05, data2.corrected_v2_20_40[i]);
        corrected_v2_20_40_graph_1->SetPoint(i, ptCenters[i] + .05, data1.corrected_v2_20_40[i]);
    }
    corrected_v2_20_40_graph_1->Draw("AP");
    corrected_v2_20_40_graph_2->Draw("P SAME");
    corrected_v2_20_40_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_v2_20_40_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Signal Window Variations");
    corrected_v2_20_40_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    corrected_v2_20_40_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    corrected_v2_20_40_graph_1->SetMinimum(minYaxis_correctedComparisons);
    corrected_v2_20_40_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_20_40_v2_corrected_productionComparisons = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_20_40_v2_corrected_productionComparisons->SetBorderSize(0);
    legend_Overlay_20_40_v2_corrected_productionComparisons->SetTextSize(0.04);
    legend_Overlay_20_40_v2_corrected_productionComparisons->AddEntry(corrected_v2_20_40_graph_2, "p013 v_{2}^{#pi^{0}}", "pe");
    legend_Overlay_20_40_v2_corrected_productionComparisons->AddEntry(corrected_v2_20_40_graph_1, "p015 v_{2}^{#pi^{0}}", "pe");
    legend_Overlay_20_40_v2_corrected_productionComparisons->Draw();
    DrawZeroLine(c_Overlay_20_40_productionComparisons_corrected);
    TLegend *legSPHENIX_20_40_productionComparisons_corrected = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_20_40_productionComparisons_corrected->SetFillStyle(0);
    legSPHENIX_20_40_productionComparisons_corrected->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_20_40_productionComparisons_corrected->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_20_40_productionComparisons_corrected->AddEntry("","20-40% Centrality","");
    legSPHENIX_20_40_productionComparisons_corrected->Draw("same");
    c_Overlay_20_40_productionComparisons_corrected->Modified();
    c_Overlay_20_40_productionComparisons_corrected->Update();
    c_Overlay_20_40_productionComparisons_corrected->SaveAs("/Users/patsfan753/Desktop/Overlay_20_40_v2_p013_p015_correctedV2Comparison.png");
    
    
    
    
    TCanvas *c_Overlay_40_60_productionComparisons_corrected = new TCanvas("c_Overlay_40_60_productionComparisons_corrected", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        corrected_v2_40_60_graph_2->SetPoint(i, ptCenters[i] - .05, data2.corrected_v2_40_60[i]);
        corrected_v2_40_60_graph_1->SetPoint(i, ptCenters[i] + .05, data1.corrected_v2_40_60[i]);
    }
    corrected_v2_40_60_graph_1->Draw("AP");
    corrected_v2_40_60_graph_2->Draw("P SAME");
    corrected_v2_40_60_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    corrected_v2_40_60_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Signal Window Variations");
    corrected_v2_40_60_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    corrected_v2_40_60_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    corrected_v2_40_60_graph_1->SetMinimum(minYaxis_correctedComparisons);
    corrected_v2_40_60_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_40_60_v2_corrected_productionComparisons = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_40_60_v2_corrected_productionComparisons->SetBorderSize(0);
    legend_Overlay_40_60_v2_corrected_productionComparisons->SetTextSize(0.04);
    legend_Overlay_40_60_v2_corrected_productionComparisons->AddEntry(corrected_v2_40_60_graph_2, "p013 v_{2}^{#pi^{0}}", "pe");
    legend_Overlay_40_60_v2_corrected_productionComparisons->AddEntry(corrected_v2_40_60_graph_1, "p015 v_{2}^{#pi^{0}}", "pe");
    legend_Overlay_40_60_v2_corrected_productionComparisons->Draw();
    DrawZeroLine(c_Overlay_40_60_productionComparisons_corrected);
    TLegend *legSPHENIX_40_60_productionComparisons_corrected = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_40_60_productionComparisons_corrected->SetFillStyle(0);
    legSPHENIX_40_60_productionComparisons_corrected->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_40_60_productionComparisons_corrected->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_40_60_productionComparisons_corrected->AddEntry("","40-60% Centrality","");
    legSPHENIX_40_60_productionComparisons_corrected->Draw("same");
    c_Overlay_40_60_productionComparisons_corrected->Modified();
    c_Overlay_40_60_productionComparisons_corrected->Update();
    c_Overlay_40_60_productionComparisons_corrected->SaveAs("/Users/patsfan753/Desktop/Overlay_40_60_v2_p013_p015_correctedV2Comparison.png");
    
    
    TCanvas *c_Overlay_0_20_productionComparisons_signal = new TCanvas("c_Overlay_0_20_productionComparisons_signal", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        signal_v2_0_20_graph_2->SetPoint(i, ptCenters[i] - .05, data2.signal_v2_0_20[i]);
        signal_v2_0_20_graph_1->SetPoint(i, ptCenters[i] + .05, data1.signal_v2_0_20[i]);
    }
    signal_v2_0_20_graph_1->Draw("AP");
    signal_v2_0_20_graph_2->Draw("P SAME");
    signal_v2_0_20_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    signal_v2_0_20_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Signal Window Variations");
    signal_v2_0_20_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    signal_v2_0_20_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    signal_v2_0_20_graph_1->SetMinimum(minYaxis_correctedComparisons);
    signal_v2_0_20_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_0_20_v2_signal_productionComparisons = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_0_20_v2_signal_productionComparisons->SetBorderSize(0);
    legend_Overlay_0_20_v2_signal_productionComparisons->SetTextSize(0.04);
    legend_Overlay_0_20_v2_signal_productionComparisons->AddEntry(signal_v2_0_20_graph_2, "p013 v_{2}^{measured}", "pe");
    legend_Overlay_0_20_v2_signal_productionComparisons->AddEntry(signal_v2_0_20_graph_1, "p015 v_{2}^{measured}", "pe");
    legend_Overlay_0_20_v2_signal_productionComparisons->Draw();
    DrawZeroLine(c_Overlay_0_20_productionComparisons_signal);
    TLegend *legSPHENIX_0_20_productionComparisons_signal = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_0_20_productionComparisons_signal->SetFillStyle(0);
    legSPHENIX_0_20_productionComparisons_signal->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_0_20_productionComparisons_signal->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_0_20_productionComparisons_signal->AddEntry("","0-20% Centrality","");
    legSPHENIX_0_20_productionComparisons_signal->Draw("same");
    c_Overlay_0_20_productionComparisons_signal->Modified();
    c_Overlay_0_20_productionComparisons_signal->Update();
    c_Overlay_0_20_productionComparisons_signal->SaveAs("/Users/patsfan753/Desktop/Overlay_0_20_v2_p013_p015_signalV2Comparison.png");
    
    
    
    
    TCanvas *c_Overlay_20_40_productionComparisons_signal = new TCanvas("c_Overlay_20_40_productionComparisons_signal", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        signal_v2_20_40_graph_2->SetPoint(i, ptCenters[i] - .05, data2.signal_v2_20_40[i]);
        signal_v2_20_40_graph_1->SetPoint(i, ptCenters[i] + .05, data1.signal_v2_20_40[i]);
    }
    signal_v2_20_40_graph_1->Draw("AP");
    signal_v2_20_40_graph_2->Draw("P SAME");
    signal_v2_20_40_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    signal_v2_20_40_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Signal Window Variations");
    signal_v2_20_40_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    signal_v2_20_40_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    signal_v2_20_40_graph_1->SetMinimum(minYaxis_correctedComparisons);
    signal_v2_20_40_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_20_40_v2_signal_productionComparisons = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_20_40_v2_signal_productionComparisons->SetBorderSize(0);
    legend_Overlay_20_40_v2_signal_productionComparisons->SetTextSize(0.04);
    legend_Overlay_20_40_v2_signal_productionComparisons->AddEntry(signal_v2_20_40_graph_2, "p013 v_{2}^{measured}", "pe");
    legend_Overlay_20_40_v2_signal_productionComparisons->AddEntry(signal_v2_20_40_graph_1, "p015 v_{2}^{measured}", "pe");
    legend_Overlay_20_40_v2_signal_productionComparisons->Draw();
    DrawZeroLine(c_Overlay_20_40_productionComparisons_signal);
    TLegend *legSPHENIX_20_40_productionComparisons_signal = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_20_40_productionComparisons_signal->SetFillStyle(0);
    legSPHENIX_20_40_productionComparisons_signal->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_20_40_productionComparisons_signal->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_20_40_productionComparisons_signal->AddEntry("","20-40% Centrality","");
    legSPHENIX_20_40_productionComparisons_signal->Draw("same");
    c_Overlay_20_40_productionComparisons_signal->Modified();
    c_Overlay_20_40_productionComparisons_signal->Update();
    c_Overlay_20_40_productionComparisons_signal->SaveAs("/Users/patsfan753/Desktop/Overlay_20_40_v2_p013_p015_signalV2Comparison.png");
    
    
    
    
    TCanvas *c_Overlay_40_60_productionComparisons_signal = new TCanvas("c_Overlay_40_60_productionComparisons_signal", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        signal_v2_40_60_graph_2->SetPoint(i, ptCenters[i] - .05, data2.signal_v2_40_60[i]);
        signal_v2_40_60_graph_1->SetPoint(i, ptCenters[i] + .05, data1.signal_v2_40_60[i]);
    }
    signal_v2_40_60_graph_1->Draw("AP");
    signal_v2_40_60_graph_2->Draw("P SAME");
    signal_v2_40_60_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    signal_v2_40_60_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, Signal Window Variations");
    signal_v2_40_60_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    signal_v2_40_60_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    signal_v2_40_60_graph_1->SetMinimum(minYaxis_correctedComparisons);
    signal_v2_40_60_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_40_60_v2_signal_productionComparisons = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_40_60_v2_signal_productionComparisons->SetBorderSize(0);
    legend_Overlay_40_60_v2_signal_productionComparisons->SetTextSize(0.04);
    legend_Overlay_40_60_v2_signal_productionComparisons->AddEntry(signal_v2_40_60_graph_2, "p013 v_{2}^{measured}", "pe");
    legend_Overlay_40_60_v2_signal_productionComparisons->AddEntry(signal_v2_40_60_graph_1, "p015 v_{2}^{measured}", "pe");
    legend_Overlay_40_60_v2_signal_productionComparisons->Draw();
    DrawZeroLine(c_Overlay_40_60_productionComparisons_signal);
    TLegend *legSPHENIX_40_60_productionComparisons_signal = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_40_60_productionComparisons_signal->SetFillStyle(0);
    legSPHENIX_40_60_productionComparisons_signal->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_40_60_productionComparisons_signal->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_40_60_productionComparisons_signal->AddEntry("","40-60% Centrality","");
    legSPHENIX_40_60_productionComparisons_signal->Draw("same");
    c_Overlay_40_60_productionComparisons_signal->Modified();
    c_Overlay_40_60_productionComparisons_signal->Update();
    c_Overlay_40_60_productionComparisons_signal->SaveAs("/Users/patsfan753/Desktop/Overlay_40_60_v2_p013_p015_signalV2Comparison.png");
    
    
    
    TCanvas *c_Overlay_0_20_productionComparisons_bg = new TCanvas("c_Overlay_0_20_productionComparisons_bg", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        bg_v2_0_20_graph_2->SetPoint(i, ptCenters[i] - .05, data2.bg_v2_0_20[i]);
        bg_v2_0_20_graph_1->SetPoint(i, ptCenters[i] + .05, data1.bg_v2_0_20[i]);
    }
    bg_v2_0_20_graph_1->Draw("AP");
    bg_v2_0_20_graph_2->Draw("P SAME");
    bg_v2_0_20_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    bg_v2_0_20_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, bg Window Variations");
    bg_v2_0_20_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    bg_v2_0_20_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    bg_v2_0_20_graph_1->SetMinimum(minYaxis_correctedComparisons);
    bg_v2_0_20_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_0_20_v2_bg_productionComparisons = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_0_20_v2_bg_productionComparisons->SetBorderSize(0);
    legend_Overlay_0_20_v2_bg_productionComparisons->SetTextSize(0.04);
    legend_Overlay_0_20_v2_bg_productionComparisons->AddEntry(bg_v2_0_20_graph_2, "p013 v_{2}^{BG}", "pe");
    legend_Overlay_0_20_v2_bg_productionComparisons->AddEntry(bg_v2_0_20_graph_1, "p015 v_{2}^{BG}", "pe");
    legend_Overlay_0_20_v2_bg_productionComparisons->Draw();
    DrawZeroLine(c_Overlay_0_20_productionComparisons_bg);
    TLegend *legSPHENIX_0_20_productionComparisons_bg = new TLegend(0.14,.72,0.34,.92);
    legSPHENIX_0_20_productionComparisons_bg->SetFillStyle(0);
    legSPHENIX_0_20_productionComparisons_bg->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_0_20_productionComparisons_bg->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_0_20_productionComparisons_bg->AddEntry("","0-20% Centrality","");
    legSPHENIX_0_20_productionComparisons_bg->Draw("same");
    c_Overlay_0_20_productionComparisons_bg->Modified();
    c_Overlay_0_20_productionComparisons_bg->Update();
    c_Overlay_0_20_productionComparisons_bg->SaveAs("/Users/patsfan753/Desktop/Overlay_0_20_v2_p013_p015_bgV2Comparison.png");
    
    
    
    
    TCanvas *c_Overlay_20_40_productionComparisons_bg = new TCanvas("c_Overlay_20_40_productionComparisons_bg", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        bg_v2_20_40_graph_2->SetPoint(i, ptCenters[i] - .05, data2.bg_v2_20_40[i]);
        bg_v2_20_40_graph_1->SetPoint(i, ptCenters[i] + .05, data1.bg_v2_20_40[i]);
    }
    bg_v2_20_40_graph_1->Draw("AP");
    bg_v2_20_40_graph_2->Draw("P SAME");
    bg_v2_20_40_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    bg_v2_20_40_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, bg Window Variations");
    bg_v2_20_40_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    bg_v2_20_40_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    bg_v2_20_40_graph_1->SetMinimum(minYaxis_correctedComparisons);
    bg_v2_20_40_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_20_40_v2_bg_productionComparisons = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_20_40_v2_bg_productionComparisons->SetBorderSize(0);
    legend_Overlay_20_40_v2_bg_productionComparisons->SetTextSize(0.04);
    legend_Overlay_20_40_v2_bg_productionComparisons->AddEntry(bg_v2_20_40_graph_2, "p013 v_{2}^{BG}", "pe");
    legend_Overlay_20_40_v2_bg_productionComparisons->AddEntry(bg_v2_20_40_graph_1, "p015 v_{2}^{BG}", "pe");
    legend_Overlay_20_40_v2_bg_productionComparisons->Draw();
    DrawZeroLine(c_Overlay_20_40_productionComparisons_bg);
    TLegend *legSPHENIX_20_40_productionComparisons_bg = new TLegend(0.14,.42,0.34,.62);
    legSPHENIX_20_40_productionComparisons_bg->SetFillStyle(0);
    legSPHENIX_20_40_productionComparisons_bg->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_20_40_productionComparisons_bg->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_20_40_productionComparisons_bg->AddEntry("","20-40% Centrality","");
    legSPHENIX_20_40_productionComparisons_bg->Draw("same");
    c_Overlay_20_40_productionComparisons_bg->Modified();
    c_Overlay_20_40_productionComparisons_bg->Update();
    c_Overlay_20_40_productionComparisons_bg->SaveAs("/Users/patsfan753/Desktop/Overlay_20_40_v2_p013_p015_bgV2Comparison.png");
    
    
    
    
    TCanvas *c_Overlay_40_60_productionComparisons_bg = new TCanvas("c_Overlay_40_60_productionComparisons_bg", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality", 800, 600);
    for (int i = 0; i < ptCenters.size(); ++i) {
        bg_v2_40_60_graph_2->SetPoint(i, ptCenters[i] - .05, data2.bg_v2_40_60[i]);
        bg_v2_40_60_graph_1->SetPoint(i, ptCenters[i] + .05, data1.bg_v2_40_60[i]);
    }
    bg_v2_40_60_graph_1->Draw("AP");
    bg_v2_40_60_graph_2->Draw("P SAME");
    bg_v2_40_60_graph_1->GetXaxis()->SetLimits(2.0, 5.0);
    bg_v2_40_60_graph_1->SetTitle("#it{v}_{2}^{#pi^{0}} vs #it{p}_{T} 40-60% Centrality, bg Window Variations");
    bg_v2_40_60_graph_1->GetXaxis()->SetTitle("p_{T} [GeV]");
    bg_v2_40_60_graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    bg_v2_40_60_graph_1->SetMinimum(minYaxis_correctedComparisons);
    bg_v2_40_60_graph_1->SetMaximum(maxYaxis_correctedComparisons);
    TLegend *legend_Overlay_40_60_v2_bg_productionComparisons = new TLegend(0.2, 0.18, 0.4, 0.33);
    legend_Overlay_40_60_v2_bg_productionComparisons->SetBorderSize(0);
    legend_Overlay_40_60_v2_bg_productionComparisons->SetTextSize(0.04);
    legend_Overlay_40_60_v2_bg_productionComparisons->AddEntry(bg_v2_40_60_graph_2, "p013 v_{2}^{BG}", "pe");
    legend_Overlay_40_60_v2_bg_productionComparisons->AddEntry(bg_v2_40_60_graph_1, "p015 v_{2}^{BG}", "pe");
    legend_Overlay_40_60_v2_bg_productionComparisons->Draw();
    DrawZeroLine(c_Overlay_40_60_productionComparisons_bg);
    TLegend *legSPHENIX_40_60_productionComparisons_bg = new TLegend(0.14,.45,0.34,.65);
    legSPHENIX_40_60_productionComparisons_bg->SetFillStyle(0);
    legSPHENIX_40_60_productionComparisons_bg->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    legSPHENIX_40_60_productionComparisons_bg->AddEntry("","Au+Au #sqrt{s_{NN}} = 200 GeV","");
    legSPHENIX_40_60_productionComparisons_bg->AddEntry("","40-60% Centrality","");
    legSPHENIX_40_60_productionComparisons_bg->Draw("same");
    c_Overlay_40_60_productionComparisons_bg->Modified();
    c_Overlay_40_60_productionComparisons_bg->Update();
    c_Overlay_40_60_productionComparisons_bg->SaveAs("/Users/patsfan753/Desktop/Overlay_40_60_v2_p013_p015_bgV2Comparison.png");
    
}

struct PlotConfig {
    bool plotSignalWindowVariations;
    bool plotEMCalScaleVariations;
    bool plotAsymmetryCutVariations;
    bool plotSampleSizeVariations;
    bool plotSigBgCorrAnd_bgWindowVariations;
    bool plotProduction_Comparisons;
};


void v2_Results_Plotter() {
    gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();
    
    std::string ReferenceData = "/Users/patsfan753/Desktop/p015/vn-p015.csv";
    
    Data data1;
    
    // Read the datasets
    Read_DataSet(ReferenceData, data1);
    
    std::string statUncertaintyFilePath = BasePlotOutputPath + "/Systematics_Analysis-v2-Checks/StatUncertaintyTable_p015.csv";
    
    ReadStatUncertainties(statUncertaintyFilePath, data1);

    std::vector<double> ptCenters = {2.25, 2.75, 3.25, 3.75, 4.25, 4.75}; // Mid-points of pT ranges

    
    plotting(data1);
    plotting_Systematics(data1);
    
    EMCal_Systematic_Data EMCal_Systematic_data1;
    std::string statUncertaintyFilePath_EMCal_Contributors = baseDataPath_EmCal_Systematics + "StatUncertaintyTable_EMCalVariationsOnly.csv";
    ReadStatUncertainties_emCalScaleCSV(statUncertaintyFilePath_EMCal_Contributors, EMCal_Systematic_data1);
    plotting_EMCal_Syst_Contributors(EMCal_Systematic_data1);
    
    
    
    //PLOTTING FUNCTIONALITY for v2 overlays no systematics-- just variations
    std::string filePathSignal_Bound_Variation = baseDataPath_SignalWindowVariations + "vn-sigma-1.5.csv";
    
    
    std::string filePath_AsymmetryVariations_45 = baseDataPath_AsymmetryCutVariations + "vn-asym-0.45.csv";
    std::string filePath_AsymmetryVariations_55 = baseDataPath_AsymmetryCutVariations + "vn-asym-0.55.csv";
    
    std::string filePathSampleSizeVariation = baseDataPath_SampleSizeVariations + "vn-samples-25.csv";
    
    
    std::string filePathEMCal_Syst_SYST1CEMC = baseDataPath_EmCal_Systematics + "vn-SYST1CEMC.csv";
    std::string filePathEMCal_Syst_SYST2CEMC = baseDataPath_EmCal_Systematics + "vn-SYST2CEMC.csv";
    std::string filePathEMCal_Syst_SYST3DCEMC = baseDataPath_EmCal_Systematics + "vn-SYST3DCEMC.csv";
    std::string filePathEMCal_Syst_SYST3UCEMC = baseDataPath_EmCal_Systematics + "vn-SYST3UCEMC.csv";
    std::string filePathEMCal_Syst_SYST4CEMC = baseDataPath_EmCal_Systematics + "vn-SYST4CEMC.csv";
    
    std::string p013_filePath = "/Users/patsfan753/Desktop/Analysis_p013/Default_Final_v2_p013.csv";

    PlotConfig config;
    config.plotSignalWindowVariations = false;
    config.plotEMCalScaleVariations = false;
    config.plotAsymmetryCutVariations = false;
    config.plotSampleSizeVariations = false;
    config.plotProduction_Comparisons = false;
    
    //always will work if ref data set read in
    config.plotSigBgCorrAnd_bgWindowVariations = true;

    Data data2, data3, data4, data5, data6;

    if (config.plotSignalWindowVariations) {
        Read_DataSet(filePathSignal_Bound_Variation, data2);
        plot_SignalWindow_Variation(data1, data2);
    }
    if (config.plotEMCalScaleVariations) {
        Read_DataSet(filePathEMCal_Syst_SYST1CEMC, data2);
        Read_DataSet(filePathEMCal_Syst_SYST2CEMC, data3);
        Read_DataSet(filePathEMCal_Syst_SYST3DCEMC, data4);
        Read_DataSet(filePathEMCal_Syst_SYST3UCEMC, data5);
        Read_DataSet(filePathEMCal_Syst_SYST4CEMC, data6);
        plot_EMCal_Scale_v2_overlays(data1, data2, data3, data4, data5, data6);
    }
    if (config.plotAsymmetryCutVariations) {
        Read_DataSet(filePath_AsymmetryVariations_45, data2);
        Read_DataSet(filePath_AsymmetryVariations_55, data3);

        plot_AsymmetryCut_Variations(data1, data2, data3);
    }
    if (config.plotSampleSizeVariations) {
        Read_DataSet(filePathSampleSizeVariation, data2);
        plot_SampleSize_Variations(data1, data2);
    }
    if (config.plotSigBgCorrAnd_bgWindowVariations) {
        plot_sigBgCorr_and_bgWindowVariations(data1);
    }
    if (config.plotProduction_Comparisons) {
        Read_DataSet(p013_filePath, data2);
        plotProduction_Comparisons(data1, data2);
    }
}
