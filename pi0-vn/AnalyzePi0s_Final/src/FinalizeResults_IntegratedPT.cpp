#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

#include <vector>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TLine.h>
#include <TLegend.h>
std::string BasePlotOutputPath = "/Users/patsfan753/Desktop/p015/IntegratedOverPt/";

std::string BaseCSVoutput = BasePlotOutputPath + "vN-CSV-files";
std::string SystematicsBasePlotOutput = BasePlotOutputPath + "Systematics/";
std::string baseDataPath_EmCal_Systematics = SystematicsBasePlotOutput + "emCalSystematics/";

std::vector<double> centralityCenters = {5, 15, 25, 35, 45, 55}; // Mid-points of centrality ranges for plots

struct Data {
    std::vector<double> corrected_v2_0_10, corrected_v2_0_10_Errors;
    std::vector<double> corrected_v2_10_20, corrected_v2_10_20_Errors;
    std::vector<double> corrected_v2_20_30, corrected_v2_20_30_Errors;
    std::vector<double> corrected_v2_30_40, corrected_v2_30_40_Errors;
    std::vector<double> corrected_v2_40_50, corrected_v2_40_50_Errors;
    std::vector<double> corrected_v2_50_60, corrected_v2_50_60_Errors;
    
    std::vector<double> corrected_v2_0_10_type4, corrected_v2_0_10_Errors_type4;
    std::vector<double> corrected_v2_10_20_type4, corrected_v2_10_20_Errors_type4;
    std::vector<double> corrected_v2_20_30_type4, corrected_v2_20_30_Errors_type4;
    std::vector<double> corrected_v2_30_40_type4, corrected_v2_30_40_Errors_type4;
    std::vector<double> corrected_v2_40_50_type4, corrected_v2_40_50_Errors_type4;
    std::vector<double> corrected_v2_50_60_type4, corrected_v2_50_60_Errors_type4;
    
    std::vector<double> total_syst_uncertainties_0_10, unWeighted_total_syst_uncertainties_0_10; //weighted and unweighted total systematic uncertainty calculated
    std::vector<double> total_syst_uncertainties_10_20, unWeighted_total_syst_uncertainties_10_20;
    std::vector<double> total_syst_uncertainties_20_30, unWeighted_total_syst_uncertainties_20_30;
    std::vector<double> total_syst_uncertainties_30_40, unWeighted_total_syst_uncertainties_30_40;
    std::vector<double> total_syst_uncertainties_40_50, unWeighted_total_syst_uncertainties_40_50;
    std::vector<double> total_syst_uncertainties_50_60, unWeighted_total_syst_uncertainties_50_60;
    
    std::vector<double> quad_sum_EMCal_syst_0_10, signal_window_syst_0_10, background_window_syst_0_10;
    std::vector<double> quad_sum_EMCal_syst_10_20, signal_window_syst_10_20, background_window_syst_10_20;
    std::vector<double> quad_sum_EMCal_syst_20_30, signal_window_syst_20_30, background_window_syst_20_30;
    std::vector<double> quad_sum_EMCal_syst_30_40, signal_window_syst_30_40, background_window_syst_30_40;
    std::vector<double> quad_sum_EMCal_syst_40_50, signal_window_syst_40_50, background_window_syst_40_50;
    std::vector<double> quad_sum_EMCal_syst_50_60, signal_window_syst_50_60, background_window_syst_50_60;
};

struct EMCal_Systematic_Data { //struct to hold relative and absolute systematic uncertainty contributions to EMCal Scale variations for each centrality
    std::vector<double> RelativeUncertainty_SYST1CEMC_0_10, AbsoluteUncertainty_SYST1CEMC_0_10;
    std::vector<double> RelativeUncertainty_SYST1CEMC_10_20, AbsoluteUncertainty_SYST1CEMC_10_20;
    std::vector<double> RelativeUncertainty_SYST1CEMC_20_30, AbsoluteUncertainty_SYST1CEMC_20_30;
    std::vector<double> RelativeUncertainty_SYST1CEMC_30_40, AbsoluteUncertainty_SYST1CEMC_30_40;
    std::vector<double> RelativeUncertainty_SYST1CEMC_40_50, AbsoluteUncertainty_SYST1CEMC_40_50;
    std::vector<double> RelativeUncertainty_SYST1CEMC_50_60, AbsoluteUncertainty_SYST1CEMC_50_60;
    
    std::vector<double> RelativeUncertainty_SYST2CEMC_0_10, AbsoluteUncertainty_SYST2CEMC_0_10;
    std::vector<double> RelativeUncertainty_SYST2CEMC_10_20, AbsoluteUncertainty_SYST2CEMC_10_20;
    std::vector<double> RelativeUncertainty_SYST2CEMC_20_30, AbsoluteUncertainty_SYST2CEMC_20_30;
    std::vector<double> RelativeUncertainty_SYST2CEMC_30_40, AbsoluteUncertainty_SYST2CEMC_30_40;
    std::vector<double> RelativeUncertainty_SYST2CEMC_40_50, AbsoluteUncertainty_SYST2CEMC_40_50;
    std::vector<double> RelativeUncertainty_SYST2CEMC_50_60, AbsoluteUncertainty_SYST2CEMC_50_60;
    
    std::vector<double> RelativeUncertainty_SYST3DCEMC_0_10, AbsoluteUncertainty_SYST3DCEMC_0_10;
    std::vector<double> RelativeUncertainty_SYST3DCEMC_10_20, AbsoluteUncertainty_SYST3DCEMC_10_20;
    std::vector<double> RelativeUncertainty_SYST3DCEMC_20_30, AbsoluteUncertainty_SYST3DCEMC_20_30;
    std::vector<double> RelativeUncertainty_SYST3DCEMC_30_40, AbsoluteUncertainty_SYST3DCEMC_30_40;
    std::vector<double> RelativeUncertainty_SYST3DCEMC_40_50, AbsoluteUncertainty_SYST3DCEMC_40_50;
    std::vector<double> RelativeUncertainty_SYST3DCEMC_50_60, AbsoluteUncertainty_SYST3DCEMC_50_60;
    
    std::vector<double> RelativeUncertainty_SYST3UCEMC_0_10, AbsoluteUncertainty_SYST3UCEMC_0_10;
    std::vector<double> RelativeUncertainty_SYST3UCEMC_10_20, AbsoluteUncertainty_SYST3UCEMC_10_20;
    std::vector<double> RelativeUncertainty_SYST3UCEMC_20_30, AbsoluteUncertainty_SYST3UCEMC_20_30;
    std::vector<double> RelativeUncertainty_SYST3UCEMC_30_40, AbsoluteUncertainty_SYST3UCEMC_30_40;
    std::vector<double> RelativeUncertainty_SYST3UCEMC_40_50, AbsoluteUncertainty_SYST3UCEMC_40_50;
    std::vector<double> RelativeUncertainty_SYST3UCEMC_50_60, AbsoluteUncertainty_SYST3UCEMC_50_60;
    
    std::vector<double> RelativeUncertainty_SYST4CEMC_0_10, AbsoluteUncertainty_SYST4CEMC_0_10;
    std::vector<double> RelativeUncertainty_SYST4CEMC_10_20, AbsoluteUncertainty_SYST4CEMC_10_20;
    std::vector<double> RelativeUncertainty_SYST4CEMC_20_30, AbsoluteUncertainty_SYST4CEMC_20_30;
    std::vector<double> RelativeUncertainty_SYST4CEMC_30_40, AbsoluteUncertainty_SYST4CEMC_30_40;
    std::vector<double> RelativeUncertainty_SYST4CEMC_40_50, AbsoluteUncertainty_SYST4CEMC_40_50;
    std::vector<double> RelativeUncertainty_SYST4CEMC_50_60, AbsoluteUncertainty_SYST4CEMC_50_60;

    std::vector<double> Relative_QuadSum_0_10, Absolute_QuadSum_0_10;
    std::vector<double> Relative_QuadSum_10_20, Absolute_QuadSum_10_20;
    std::vector<double> Relative_QuadSum_20_30, Absolute_QuadSum_20_30;
    std::vector<double> Relative_QuadSum_30_40, Absolute_QuadSum_30_40;
    std::vector<double> Relative_QuadSum_40_50, Absolute_QuadSum_40_50;
    std::vector<double> Relative_QuadSum_50_60, Absolute_QuadSum_50_60;
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
        
        double v2_corrected_type4 = std::stod(rowData.at(3));
        double v2_error_corrected_type4 = std::stod(rowData.at(4));

        
    
        int index = std::stoi(rowData[0]); //index of analysis bin, first column of CSV
        if (index == 0) { //index 0 50-60% Centrality
            data.corrected_v2_50_60.push_back(v2_corrected);
            data.corrected_v2_50_60_Errors.push_back(v2_error_corrected);
            
            data.corrected_v2_50_60_type4.push_back(v2_corrected_type4);
            data.corrected_v2_50_60_Errors_type4.push_back(v2_error_corrected_type4);

        } else if (index == 1) { //index 1 is 40-50
            data.corrected_v2_40_50.push_back(v2_corrected);
            data.corrected_v2_40_50_Errors.push_back(v2_error_corrected);
            
            data.corrected_v2_40_50_type4.push_back(v2_corrected_type4);
            data.corrected_v2_40_50_Errors_type4.push_back(v2_error_corrected_type4);

        } else if (index == 2) { //index 2 is 30-40
            data.corrected_v2_30_40.push_back(v2_corrected);
            data.corrected_v2_30_40_Errors.push_back(v2_error_corrected);
            
            data.corrected_v2_30_40_type4.push_back(v2_corrected_type4);
            data.corrected_v2_30_40_Errors_type4.push_back(v2_error_corrected_type4);
        } else if (index == 3) { //index 3 is 20-30
            data.corrected_v2_20_30.push_back(v2_corrected);
            data.corrected_v2_20_30_Errors.push_back(v2_error_corrected);

            data.corrected_v2_20_30_type4.push_back(v2_corrected_type4);
            data.corrected_v2_20_30_Errors_type4.push_back(v2_error_corrected_type4);
        } else if (index == 4) { //index 4 is 10-20
            data.corrected_v2_10_20.push_back(v2_corrected);
            data.corrected_v2_10_20_Errors.push_back(v2_error_corrected);
            
            data.corrected_v2_10_20_type4.push_back(v2_corrected_type4);
            data.corrected_v2_10_20_Errors_type4.push_back(v2_error_corrected_type4);

        } else if (index == 5) { //index 5 is 0-10
            data.corrected_v2_0_10.push_back(v2_corrected);
            data.corrected_v2_0_10_Errors.push_back(v2_error_corrected);
            
            data.corrected_v2_0_10_type4.push_back(v2_corrected_type4);
            data.corrected_v2_0_10_Errors_type4.push_back(v2_error_corrected_type4);
        }
    }
    file.close();
}

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
        // Assigning to the correct centrality vector based on row count in my specific cleaned PHENIX data CSV
        if (rowCounter < 6) { // 0-10%
            v2_0_10.push_back(v2);
            v2_0_10_Errors.push_back(error_pos);
            v2_0_10_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for PHENIX v2_0_10:\033[0m " << v2 << " \u00B1 " << error_pos << std::endl;
        } else if (rowCounter < 12) { // 10-20%
            v2_10_20.push_back(v2);
            v2_10_20_Errors.push_back(error_pos);
            v2_10_20_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for PHENIX v2_10_20:\033[0m " << v2 << " \u00B1 " << error_pos << std::endl;
        } else if (rowCounter < 18) { // 20-30%
            v2_20_30.push_back(v2);
            v2_20_30_Errors.push_back(error_pos);
            v2_20_30_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for PHENIX v2_20_30:\033[0m " << v2 << " \u00B1 " << error_pos << std::endl;
        } else if (rowCounter < 24) { // 30-40%
            v2_30_40.push_back(v2);
            v2_30_40_Errors.push_back(error_pos);
            v2_30_40_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for PHENIX v2_30_40:\033[0m " << v2 << " \u00B1 " << error_pos << std::endl;
        } else if (rowCounter < 30) { // 40-50%
            v2_40_50.push_back(v2);
            v2_40_50_Errors.push_back(error_pos);
            v2_40_50_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for PHENIX v2_40_50:\033[0m " << v2 << " \u00B1 " << error_pos << std::endl;
        } else { // 50-60%
            v2_50_60.push_back(v2);
            v2_50_60_Errors.push_back(error_pos);
            v2_50_60_Errors_Negative.push_back(fabs(error_neg));
            std::cout << "\033[1mData read in for PHENIX v2_50_60:\033[0m " << v2 << " \u00B1 " << error_pos << std::endl;
        }
        rowCounter++;
    }
    file.close();
}
//Writes CSV file for EMCal Scale variations, calculating each contribution to total weighted and unweighted quadrature sum that is funneled into total syst calc
void WriteComparisonTo_EMcalScale_CSV(const std::vector<Data>& emCalDataSets, const Data& referenceData, const std::string& outputPath) {
    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open the file at: " << outputPath << std::endl;
        return;
    }
    outFile << "Index,Reference_v2";
    std::vector<std::string> dataSetLabels = {"SYST1CEMC", "SYST2CEMC", "SYST3DCEMC", "SYST3UCEMC", "SYST4CEMC"}; //used for CSV header writing
    for (const auto& label : dataSetLabels) {
        outFile << ",RelativeUncertainty_" << label << ",AbsoluteUncertainty_" << label;
    }
    // New columns for quadrature sum
    outFile << ",QuadratureSumRelativeUncertainties,QuadratureSumAbsoluteUncertainties\n";

    // Output header for console
    std::cout << "\033[1mCalculating EMCal Systematics:\033[0m\n";
    std::cout << "\033[1m" << std::left << std::setw(15) << "DataSet Label"
              << std::setw(10) << "Index"
              << std::setw(15) << "Reference v2"
              << std::setw(22) << "Abs. Uncertainty" << "\033[0m" << std::endl;
    std::cout << std::string(62, '-') << "\n";
    
    for (size_t index = 0; index < 6; ++index) {
        double sumSquaredRelative = 0.0;
        double sumSquaredAbsolute = 0.0;

        const auto& referenceVector = index == 0 ? referenceData.corrected_v2_50_60 :
                                      index == 1 ? referenceData.corrected_v2_40_50 :
                                      index == 2 ? referenceData.corrected_v2_30_40 :
                                      index == 3 ? referenceData.corrected_v2_20_30 :
                                      index == 4 ? referenceData.corrected_v2_10_20 :
                                      referenceData.corrected_v2_0_10;
        size_t binIndex = index % 1;
        if (binIndex < referenceVector.size()) {
            double reference_v2 = referenceVector[binIndex];
            outFile << index << "," << reference_v2;
            // Starting from index 1 to skip the defaultData at index 0, which is used only as a reference for calculations
            for (size_t dataSetIndex = 1; dataSetIndex < emCalDataSets.size(); ++dataSetIndex) {
                //heres where we grab index 1 of emcaldatasets being SYST1CEMC
                const auto& currentDataSet = emCalDataSets[dataSetIndex];
                const auto& v2_vector = index == 0 ? currentDataSet.corrected_v2_50_60 :
                                        index == 1 ? currentDataSet.corrected_v2_40_50 :
                                        index == 2 ? currentDataSet.corrected_v2_30_40 :
                                        index == 3 ? currentDataSet.corrected_v2_20_30 :
                                        index == 4 ? currentDataSet.corrected_v2_10_20 :
                                        currentDataSet.corrected_v2_0_10;

                if (binIndex < v2_vector.size()) {
                    double v2 = v2_vector[binIndex];
                    double relativeUncertainty = std::abs((v2 - reference_v2) / reference_v2);
                    double absoluteUncertainty = std::abs(v2 - reference_v2);

                    // Squaring the uncertainties for quadrature sum
                    sumSquaredRelative += pow(relativeUncertainty, 2);
                    sumSquaredAbsolute += pow(absoluteUncertainty, 2);

                    outFile << "," << relativeUncertainty << "," << absoluteUncertainty;
                    
                    std::cout << std::left << "\033[1m" << std::setw(15) << dataSetLabels[dataSetIndex-1]
                              << std::setw(10) << index
                              << std::setw(15) << std::fixed << std::setprecision(6) << v2
                              << std::setw(22) << absoluteUncertainty << "\033[0m" << std::endl;
                    
                } else {
                    outFile << ",,"; // In case there's no data for this dataset at this bin index
                    std::cout << std::left << "\033[1m" << std::setw(15) << dataSetLabels[dataSetIndex]
                              << std::setw(10) << index
                              << std::setw(15) << "No Data"
                              << std::setw(22) << "N/A" << "\033[0m" << std::endl;
                }
            }

            // Writing the quadrature sums to the CSV
            outFile << "," << sqrt(sumSquaredRelative) << "," << sqrt(sumSquaredAbsolute);
            outFile << "\n";
        }
    }
    outFile.close();
}
/*
 Writes CSV file for total systematic uncertainty and respective contributors -- signal window, background window (read in as corrected_v2_type4), and EMCal Scale Variations
 */
void WriteComparisonToCSV(const std::vector<Data>& emCalDataSets, const Data& signalWindowData, const Data& referenceData, const std::string& outputPath) {
    std::ofstream outFile(outputPath);
    outFile << "Index,Reference_v2,Quad_Sum_EMCal_Syst,SignalWindow_Syst,BackgroundWindow_Syst,WeightedQuadratureSum,unWeightedQuadratureSum\n";
    
    
    // Header for terminal output with ANSI color codes for red font
    std::cout << "\033[1;31m"; // Red color start
    std::cout << std::left << std::setw(10) << "Index" << std::setw(20) << "Reference v2"
              << std::setw(20) << "Final Quad Sum" << "\n";
    std::cout << "\033[0m"; // Reset to default color

    bool skipNextDataSet = false; // Renamed for clarity
    for (size_t index = 0; index < 6; ++index) {
        const auto& referenceVector = index == 0 ? referenceData.corrected_v2_50_60 :
                                      index == 1 ? referenceData.corrected_v2_40_50 :
                                      index == 2 ? referenceData.corrected_v2_30_40 :
                                      index == 3 ? referenceData.corrected_v2_20_30 :
                                      index == 4 ? referenceData.corrected_v2_10_20 :
                                      referenceData.corrected_v2_0_10;
        size_t binIndex = index % 1;
        double reference_v2 = referenceVector[binIndex];
        double quadratureSum = 0.0;
        
        // Calculate quadrature sum for EMCal systematics datasets
        for (size_t dataSetIndex = 1; dataSetIndex < emCalDataSets.size(); ++dataSetIndex) {
            const auto& v2_vector = index == 0 ? emCalDataSets[dataSetIndex].corrected_v2_50_60 :
                                    index == 1 ? emCalDataSets[dataSetIndex].corrected_v2_40_50 :
                                    index == 2 ? emCalDataSets[dataSetIndex].corrected_v2_30_40 :
                                    index == 3 ? emCalDataSets[dataSetIndex].corrected_v2_20_30 :
                                    index == 4 ? emCalDataSets[dataSetIndex].corrected_v2_10_20 :
                                    emCalDataSets[dataSetIndex].corrected_v2_0_10;
            double v2 = v2_vector[binIndex];
            double relativeDifference = std::abs((v2 - reference_v2) / reference_v2);
            
            if (dataSetIndex == 3 || dataSetIndex == 4) { // Special handling for the two datasets
                const auto& other_v2_vector = index == 0 ? emCalDataSets[4].corrected_v2_50_60 :
                                              index == 1 ? emCalDataSets[4].corrected_v2_40_50 :
                                              index == 2 ? emCalDataSets[4].corrected_v2_30_40 :
                                              index == 3 ? emCalDataSets[4].corrected_v2_20_30 :
                                              index == 4 ? emCalDataSets[4].corrected_v2_10_20 :
                                              emCalDataSets[4].corrected_v2_0_10;
                double other_v2 = other_v2_vector[binIndex];
                double otherRelativeDifference = std::abs((other_v2 - reference_v2) / reference_v2);
                
                if (dataSetIndex == 3) {
                    relativeDifference = std::max(relativeDifference, otherRelativeDifference);
                    dataSetIndex++; // Skip the next dataset as it is already compared
                }
            }

            quadratureSum += std::pow(relativeDifference, 2);
        }

        double finalQuadratureSum = std::sqrt(quadratureSum);

        // Calculate relative difference for the signal window dataset
        double signalWindow_v2 = 0.0, signalWindowRelativeDifference = 0.0;
        if (signalWindowData.corrected_v2_0_10.size() > binIndex) { // Ensure there's data to compare
            const auto& signalWindowVector = index == 0 ? signalWindowData.corrected_v2_50_60 :
                                             index == 1 ? signalWindowData.corrected_v2_40_50 :
                                             index == 2 ? signalWindowData.corrected_v2_30_40 :
                                             index == 3 ? signalWindowData.corrected_v2_20_30 :
                                             index == 4 ? signalWindowData.corrected_v2_10_20 :
                                             signalWindowData.corrected_v2_0_10;
            signalWindow_v2 = signalWindowVector[binIndex];
            signalWindowRelativeDifference = std::abs((reference_v2 - signalWindow_v2) / reference_v2);
        }

        // Calculate relative difference for Type4 within the same dataset
        double type4_v2 = 0.0, type4RelativeDifference = 0.0;
        if (referenceData.corrected_v2_0_10_type4.size() > binIndex) {
            const auto& type4Vector = index == 0 ? referenceData.corrected_v2_50_60_type4 :
                                      index == 1 ? referenceData.corrected_v2_40_50_type4 :
                                      index == 2 ? referenceData.corrected_v2_30_40_type4 :
                                      index == 3 ? referenceData.corrected_v2_20_30_type4 :
                                      index == 4 ? referenceData.corrected_v2_10_20_type4 :
                                      referenceData.corrected_v2_0_10_type4;
            type4_v2 = type4Vector[binIndex];
            type4RelativeDifference = std::abs((reference_v2 - type4_v2) / reference_v2);
        }

        // Calculate the weighted quadrature sum
        double weightedQuadratureSum = std::abs(reference_v2)* std::sqrt(
            std::pow(finalQuadratureSum, 2) +
            std::pow(signalWindowRelativeDifference, 2) +
            std::pow(type4RelativeDifference, 2)
        );
        
        // Calculate the weighted quadrature sum
        double QuadratureSum_Unweighted = std::sqrt(
            std::pow(finalQuadratureSum, 2) +
            std::pow(signalWindowRelativeDifference, 2) +
            std::pow(type4RelativeDifference, 2)
        );


        outFile << index << "," << reference_v2 << "," << finalQuadratureSum << ","
                << signalWindowRelativeDifference << "," << type4RelativeDifference << "," << weightedQuadratureSum << "," << QuadratureSum_Unweighted << "\n";
    }
    
    outFile.close();
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
        
        double syst_uncertainty = std::stod(rowData.at(5));
        double unWeighted_syst_uncertainty = std::stod(rowData.at(6));
        double quad_sum_EMCal_syst = std::stod(rowData.at(2)); // Quad_Sum_EMCal_Syst
        double signal_window_syst = std::stod(rowData.at(3));  // SignalWindow_Syst
        double background_window_syst = std::stod(rowData.at(4)); // BackgroundWindow_Syst
        
    
        int index = std::stoi(rowData[0]);
        if (index == 0) {
            data.total_syst_uncertainties_50_60.push_back(syst_uncertainty);
            data.unWeighted_total_syst_uncertainties_50_60.push_back(unWeighted_syst_uncertainty);
            data.quad_sum_EMCal_syst_50_60.push_back(quad_sum_EMCal_syst);
            data.signal_window_syst_50_60.push_back(signal_window_syst);
            data.background_window_syst_50_60.push_back(background_window_syst);
            
        } else if (index == 1) {
            data.total_syst_uncertainties_40_50.push_back(syst_uncertainty);
            data.unWeighted_total_syst_uncertainties_40_50.push_back(unWeighted_syst_uncertainty);
            data.quad_sum_EMCal_syst_40_50.push_back(quad_sum_EMCal_syst);
            data.signal_window_syst_40_50.push_back(signal_window_syst);
            data.background_window_syst_40_50.push_back(background_window_syst);

        } else if (index == 2) {
            data.total_syst_uncertainties_30_40.push_back(syst_uncertainty);
            data.unWeighted_total_syst_uncertainties_30_40.push_back(unWeighted_syst_uncertainty);
            data.quad_sum_EMCal_syst_30_40.push_back(quad_sum_EMCal_syst);
            data.signal_window_syst_30_40.push_back(signal_window_syst);
            data.background_window_syst_30_40.push_back(background_window_syst);
            
        } else if (index == 3) {
            data.total_syst_uncertainties_20_30.push_back(syst_uncertainty);
            data.unWeighted_total_syst_uncertainties_20_30.push_back(unWeighted_syst_uncertainty);
            data.quad_sum_EMCal_syst_20_30.push_back(quad_sum_EMCal_syst);
            data.signal_window_syst_20_30.push_back(signal_window_syst);
            data.background_window_syst_20_30.push_back(background_window_syst);
            
        } else if (index == 4) {
            data.total_syst_uncertainties_10_20.push_back(syst_uncertainty);
            data.unWeighted_total_syst_uncertainties_10_20.push_back(unWeighted_syst_uncertainty);
            data.quad_sum_EMCal_syst_10_20.push_back(quad_sum_EMCal_syst);
            data.signal_window_syst_10_20.push_back(signal_window_syst);
            data.background_window_syst_10_20.push_back(background_window_syst);
            
        } else if (index == 5) {
            data.total_syst_uncertainties_0_10.push_back(syst_uncertainty);
            data.unWeighted_total_syst_uncertainties_0_10.push_back(unWeighted_syst_uncertainty);
            data.quad_sum_EMCal_syst_0_10.push_back(quad_sum_EMCal_syst);
            data.signal_window_syst_0_10.push_back(signal_window_syst);
            data.background_window_syst_0_10.push_back(background_window_syst);
        }
    }
    file.close();

}
//function that reads in systematic uncertainties from EMCal syst contributor CSV file generated from CalculateSystematics.cpp
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
        if (index == 0) {
            EMCal_Systematic_Data.RelativeUncertainty_SYST1CEMC_50_60.push_back(relativeUncertainty_SYST1CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST1CEMC_50_60.push_back(absoluteUncertainty_SYST1CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST2CEMC_50_60.push_back(relativeUncertainty_SYST2CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST2CEMC_50_60.push_back(absoluteUncertainty_SYST2CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3DCEMC_50_60.push_back(relativeUncertainty_SYST3DCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3DCEMC_50_60.push_back(absoluteUncertainty_SYST3DCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3UCEMC_50_60.push_back(relativeUncertainty_SYST3UCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3UCEMC_50_60.push_back(absoluteUncertainty_SYST3UCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST4CEMC_50_60.push_back(relativeUncertainty_SYST4CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST4CEMC_50_60.push_back(absoluteUncertainty_SYST4CEMC);
            
            EMCal_Systematic_Data.Relative_QuadSum_50_60.push_back(relative_QuadSum);
            EMCal_Systematic_Data.Absolute_QuadSum_50_60.push_back(Absolute_QuadSum);
            
            
        } else if (index == 1) {
            EMCal_Systematic_Data.RelativeUncertainty_SYST1CEMC_40_50.push_back(relativeUncertainty_SYST1CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST1CEMC_40_50.push_back(absoluteUncertainty_SYST1CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST2CEMC_40_50.push_back(relativeUncertainty_SYST2CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST2CEMC_40_50.push_back(absoluteUncertainty_SYST2CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3DCEMC_40_50.push_back(relativeUncertainty_SYST3DCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3DCEMC_40_50.push_back(absoluteUncertainty_SYST3DCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3UCEMC_40_50.push_back(relativeUncertainty_SYST3UCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3UCEMC_40_50.push_back(absoluteUncertainty_SYST3UCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST4CEMC_40_50.push_back(relativeUncertainty_SYST4CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST4CEMC_40_50.push_back(absoluteUncertainty_SYST4CEMC);
            
            EMCal_Systematic_Data.Relative_QuadSum_40_50.push_back(relative_QuadSum);
            EMCal_Systematic_Data.Absolute_QuadSum_40_50.push_back(Absolute_QuadSum);
            
        } else if (index == 2) {
            EMCal_Systematic_Data.RelativeUncertainty_SYST1CEMC_30_40.push_back(relativeUncertainty_SYST1CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST1CEMC_30_40.push_back(absoluteUncertainty_SYST1CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST2CEMC_30_40.push_back(relativeUncertainty_SYST2CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST2CEMC_30_40.push_back(absoluteUncertainty_SYST2CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3DCEMC_30_40.push_back(relativeUncertainty_SYST3DCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3DCEMC_30_40.push_back(absoluteUncertainty_SYST3DCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3UCEMC_30_40.push_back(relativeUncertainty_SYST3UCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3UCEMC_30_40.push_back(absoluteUncertainty_SYST3UCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST4CEMC_30_40.push_back(relativeUncertainty_SYST4CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST4CEMC_30_40.push_back(absoluteUncertainty_SYST4CEMC);
            
            EMCal_Systematic_Data.Relative_QuadSum_30_40.push_back(relative_QuadSum);
            EMCal_Systematic_Data.Absolute_QuadSum_30_40.push_back(Absolute_QuadSum);
            
        } else if (index == 3) {
            EMCal_Systematic_Data.RelativeUncertainty_SYST1CEMC_20_30.push_back(relativeUncertainty_SYST1CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST1CEMC_20_30.push_back(absoluteUncertainty_SYST1CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST2CEMC_20_30.push_back(relativeUncertainty_SYST2CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST2CEMC_20_30.push_back(absoluteUncertainty_SYST2CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3DCEMC_20_30.push_back(relativeUncertainty_SYST3DCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3DCEMC_20_30.push_back(absoluteUncertainty_SYST3DCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3UCEMC_20_30.push_back(relativeUncertainty_SYST3UCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3UCEMC_20_30.push_back(absoluteUncertainty_SYST3UCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST4CEMC_20_30.push_back(relativeUncertainty_SYST4CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST4CEMC_20_30.push_back(absoluteUncertainty_SYST4CEMC);
            
            EMCal_Systematic_Data.Relative_QuadSum_20_30.push_back(relative_QuadSum);
            EMCal_Systematic_Data.Absolute_QuadSum_20_30.push_back(Absolute_QuadSum);
            
        } else if (index == 4) {
            EMCal_Systematic_Data.RelativeUncertainty_SYST1CEMC_10_20.push_back(relativeUncertainty_SYST1CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST1CEMC_10_20.push_back(absoluteUncertainty_SYST1CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST2CEMC_10_20.push_back(relativeUncertainty_SYST2CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST2CEMC_10_20.push_back(absoluteUncertainty_SYST2CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3DCEMC_10_20.push_back(relativeUncertainty_SYST3DCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3DCEMC_10_20.push_back(absoluteUncertainty_SYST3DCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3UCEMC_10_20.push_back(relativeUncertainty_SYST3UCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3UCEMC_10_20.push_back(absoluteUncertainty_SYST3UCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST4CEMC_10_20.push_back(relativeUncertainty_SYST4CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST4CEMC_10_20.push_back(absoluteUncertainty_SYST4CEMC);
            
            EMCal_Systematic_Data.Relative_QuadSum_10_20.push_back(relative_QuadSum);
            EMCal_Systematic_Data.Absolute_QuadSum_10_20.push_back(Absolute_QuadSum);
            
        } else if (index == 5) {
            EMCal_Systematic_Data.RelativeUncertainty_SYST1CEMC_0_10.push_back(relativeUncertainty_SYST1CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST1CEMC_0_10.push_back(absoluteUncertainty_SYST1CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST2CEMC_0_10.push_back(relativeUncertainty_SYST2CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST2CEMC_0_10.push_back(absoluteUncertainty_SYST2CEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3DCEMC_0_10.push_back(relativeUncertainty_SYST3DCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3DCEMC_0_10.push_back(absoluteUncertainty_SYST3DCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST3UCEMC_0_10.push_back(relativeUncertainty_SYST3UCEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST3UCEMC_0_10.push_back(absoluteUncertainty_SYST3UCEMC);
            
            EMCal_Systematic_Data.RelativeUncertainty_SYST4CEMC_0_10.push_back(relativeUncertainty_SYST4CEMC);
            EMCal_Systematic_Data.AbsoluteUncertainty_SYST4CEMC_0_10.push_back(absoluteUncertainty_SYST4CEMC);
            
            EMCal_Systematic_Data.Relative_QuadSum_0_10.push_back(relative_QuadSum);
            EMCal_Systematic_Data.Absolute_QuadSum_0_10.push_back(Absolute_QuadSum);
            
        }
    }
    file.close();
    
}
TGraphErrors* CreateGraph(const std::vector<double>& centralityCenters, const std::vector<double>& values, const std::vector<double>& errors) {
    return new TGraphErrors(centralityCenters.size(), &centralityCenters[0], &values[0], nullptr, &errors[0]);
}

void setGraphProperties(TGraph* graph, int markerColor, int lineColor, float markerSize, int markerStyle) {
    graph->SetMarkerColor(markerColor);
    graph->SetLineColor(lineColor);
    graph->SetMarkerSize(markerSize);
    graph->SetMarkerStyle(markerStyle);
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

void CalculateAverages(const std::vector<double>& v2, const std::vector<double>& errors, double& mean, double& error, double sigma_RP, double v2_raw_syst, double& systematicError) {
    double sum = 0, sumErrors = 0;
    for (size_t i = 0; i < v2.size(); i++) {
        sum += v2[i];
        sumErrors += errors[i] * errors[i];  // Sum of squares of errors
    }
    mean = sum / v2.size();
    error = sqrt(sumErrors) / v2.size();  // Standard deviation of errors
    
    double sigma_RP_error = mean * sigma_RP;  // Use the multiplier directly dependent on centrality
    double v2_raw_error = mean * v2_raw_syst;  // Use the multiplier directly dependent on centrality
    systematicError = std::sqrt(sigma_RP_error * sigma_RP_error + v2_raw_error * v2_raw_error);
}
void plotting_Results(Data& data) {
    // File output path
    std::string phenixFilePath = "/Users/patsfan753/Desktop/FinalCleanedPhenix.csv";
    
    std::vector<double> v2_0_10, v2_0_10_Errors, v2_0_10_Errors_Negative, v2_10_20, v2_10_20_Errors, v2_10_20_Errors_Negative, v2_20_30, v2_20_30_Errors, v2_20_30_Errors_Negative, v2_30_40, v2_30_40_Errors, v2_30_40_Errors_Negative, v2_40_50, v2_40_50_Errors, v2_40_50_Errors_Negative, v2_50_60, v2_50_60_Errors, v2_50_60_Errors_Negative;
    
    // Read in PHENIX data and errors
    ReadPHENIXData(phenixFilePath, v2_0_10, v2_0_10_Errors, v2_0_10_Errors_Negative,
                   v2_10_20, v2_10_20_Errors, v2_10_20_Errors_Negative,
                   v2_20_30, v2_20_30_Errors, v2_20_30_Errors_Negative,
                   v2_30_40, v2_30_40_Errors, v2_30_40_Errors_Negative,
                   v2_40_50, v2_40_50_Errors, v2_40_50_Errors_Negative,
                   v2_50_60, v2_50_60_Errors, v2_50_60_Errors_Negative);
    
    
    
    // Prepare the graph
    TGraphErrors* graph = new TGraphErrors();
    TGraphAsymmErrors* systematicsGraph = new TGraphAsymmErrors();
    graph->SetTitle("#pi^{0} v_{2} vs Centrality;Centrality (%);v_{2}^{#pi^{0}}");
    int pointIndex = 0;

    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values;
    values.push_back(&data.corrected_v2_0_10);
    values.push_back(&data.corrected_v2_10_20);
    values.push_back(&data.corrected_v2_20_30);
    values.push_back(&data.corrected_v2_30_40);
    values.push_back(&data.corrected_v2_40_50);
    values.push_back(&data.corrected_v2_50_60);

    std::vector<std::vector<double>*> errors;
    errors.push_back(&data.corrected_v2_0_10_Errors);
    errors.push_back(&data.corrected_v2_10_20_Errors);
    errors.push_back(&data.corrected_v2_20_30_Errors);
    errors.push_back(&data.corrected_v2_30_40_Errors);
    errors.push_back(&data.corrected_v2_40_50_Errors);
    errors.push_back(&data.corrected_v2_50_60_Errors);
    
    std::vector<std::vector<double>*> systematicErrors_sPHENIX;
    systematicErrors_sPHENIX.push_back(&data.total_syst_uncertainties_0_10);
    systematicErrors_sPHENIX.push_back(&data.total_syst_uncertainties_10_20);
    systematicErrors_sPHENIX.push_back(&data.total_syst_uncertainties_20_30);
    systematicErrors_sPHENIX.push_back(&data.total_syst_uncertainties_30_40);
    systematicErrors_sPHENIX.push_back(&data.total_syst_uncertainties_40_50);
    systematicErrors_sPHENIX.push_back(&data.total_syst_uncertainties_50_60);

    // Loop over each centrality range
    for (size_t i = 0; i < centralityCenters.size(); ++i) {
        for (size_t j = 0; j < values[i]->size(); ++j) {
            graph->SetPoint(pointIndex, centralityCenters[i] - 1.2, (*values[i])[j]);
            graph->SetPointError(pointIndex, 0, (*errors[i])[j]);
            
            // Set asymmetric errors, if systematics are indeed symmetric, set them the same
            systematicsGraph->SetPoint(i, centralityCenters[i] - 1.2, (*values[i])[j]);
            systematicsGraph->SetPointError(i, .75, .75, (*systematicErrors_sPHENIX[i])[j], (*systematicErrors_sPHENIX[i])[j]);
            
            pointIndex++;
        }
    }

    // Graph properties
    graph->SetMarkerStyle(20);
    graph->SetMarkerSize(1.4);
    graph->SetMarkerColor(kBlue);
    graph->SetLineColor(kBlue);

    systematicsGraph->SetFillColorAlpha(kBlue, 0.35);
    systematicsGraph->SetFillColor(kBlue);
    systematicsGraph->SetFillStyle(3001);
    
    // New graph for PHENIX averages
    TGraphErrors* phenixGraph = new TGraphErrors();
    TGraphAsymmErrors* phenixGraph_systematics = new TGraphAsymmErrors();
    for (size_t i = 0; i < centralityCenters.size(); ++i) {
        double mean, error, systematicError;
        if (i == 0) CalculateAverages(v2_0_10, v2_0_10_Errors, mean, error, 0.1, 0.1, systematicError);
        if (i == 1) CalculateAverages(v2_10_20, v2_10_20_Errors, mean, error, 0.1, 0.1, systematicError);
        if (i == 2) CalculateAverages(v2_20_30, v2_20_30_Errors, mean, error, 0.05, 0.03, systematicError);
        if (i == 3) CalculateAverages(v2_30_40, v2_30_40_Errors, mean, error, 0.05, 0.03, systematicError);
        if (i == 4) CalculateAverages(v2_40_50, v2_40_50_Errors, mean, error, 0.1, 0.03, systematicError);
        if (i == 5) CalculateAverages(v2_50_60, v2_50_60_Errors, mean, error, 0.1, 0.03, systematicError);

        std::cout << "Centrality: " << centralityCenters[i] << "% - Average v2: " << mean << " ± " << error << " ± " << systematicError << std::endl;
        
        phenixGraph->SetPoint(i, centralityCenters[i] + 1.2, mean);
        phenixGraph->SetPointError(i, 0, error);
        
        // Set asymmetric errors, if systematics are indeed symmetric, set them the same
        phenixGraph_systematics->SetPoint(i, centralityCenters[i] + 1.2, mean);
        phenixGraph_systematics->SetPointError(i, .75, .75, systematicError, systematicError);
        
    }
    phenixGraph_systematics->SetFillColorAlpha(kRed, 0.35);
    phenixGraph_systematics->SetFillColor(kRed);
    phenixGraph_systematics->SetFillStyle(3001);
    
    phenixGraph->SetMarkerStyle(20);
    phenixGraph->SetMarkerSize(1.4);
    phenixGraph->SetMarkerColor(kRed);
    phenixGraph->SetLineColor(kRed);

    systematicsGraph->GetYaxis()->SetRangeUser(-0.15, 0.25);
    systematicsGraph->GetXaxis()->SetRangeUser(0, 60);
    graph->GetXaxis()->SetRangeUser(0, 60);
    phenixGraph->GetXaxis()->SetRangeUser(0, 60);

    systematicsGraph->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    systematicsGraph->GetXaxis()->SetTitle("Centrality [%]");
    // Create canvas
    TCanvas* canvas = new TCanvas("c1", "Centrality Plot", 800, 600);
    systematicsGraph->Draw("A2");
    graph->Draw("P SAME");
    phenixGraph_systematics->Draw("2 SAME");
    phenixGraph->Draw("P SAME");

    
    TLegend *leg = new TLegend(0.58, 0.2, 0.78, 0.4);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.042);
    leg->AddEntry("", "#it{#bf{sPHENIX}} Internal", "");
    leg->AddEntry("", "Au+Au #sqrt{s_{NN}} = 200 GeV", "");
    leg->AddEntry("", "p_{T} Range: [2, 5] GeV", "");
    leg->Draw("same");
    
    TLegend *leg2 = new TLegend(0.2, 0.8, 0.3, 0.9);
    leg2->SetFillStyle(0);
    leg2->SetTextSize(0.04);
    leg2->AddEntry(graph, "sPHENIX", "pe");
    leg2->AddEntry(phenixGraph, "PHENIX", "pe");
    leg2->Draw("same");
    
    canvas->Update();

    // Draw zero line
    double x_min = canvas->GetUxmin();
    double x_max = canvas->GetUxmax();
    TLine* zeroLine = new TLine(x_min, 0, x_max, 0);
    zeroLine->SetLineStyle(2);
    zeroLine->Draw("SAME");

    // Save the canvas
    canvas->SaveAs((BasePlotOutputPath + "/v2_overlayedWithPHENIX_integratedOverPT.png").c_str());
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
/*
 Legend for EMCal Scale Systematic Contributors
 */
void DrawLegend_EMCal_Contributors(double x1, double y1, double x2, double y2, double textSize, const std::string& headerType, const std::vector<TH1F*>& histograms, const std::vector<std::string>& labels) {
    TLegend *legend = new TLegend(x1, y1, x2, y2);
    legend->SetTextSize(textSize);
    std::string header = "#bf{" + headerType + " Uncertainties, " + "}";
    legend->SetHeader(header.c_str(), "L");
    // Add entries to the legend
    for (size_t i = 0; i < histograms.size(); ++i) {
        if (i < labels.size()) {
            legend->AddEntry(histograms[i], labels[i].c_str(), "f");
        }
    }
    legend->Draw();
}
void plotting_EMCal_Syst_Contributors(const EMCal_Systematic_Data& EMCal_Systematic_data1) {
    double barWidth = 1.2; // Width of each bar
    std::vector<std::string> labels = {"SYST1CEMC", "SYST2CEMC", "SYST3DCEMC", "SYST3UCEMC", "SYST4CEMC", "Quadrature Sum"};
    int colors[] = {kRed, kBlue, kGreen+1, kOrange+7, kViolet, kGray+1}; // Colors for the bars
    const double EPSILON = 1e-6;
    std::vector<double> SYST1CEMC_Relative = {
        EMCal_Systematic_data1.RelativeUncertainty_SYST1CEMC_0_10[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST1CEMC_10_20[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST1CEMC_20_30[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST1CEMC_30_40[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST1CEMC_40_50[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST1CEMC_50_60[0]
    };
    std::vector<double> SYST2CEMC_Relative = {
        EMCal_Systematic_data1.RelativeUncertainty_SYST2CEMC_0_10[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST2CEMC_10_20[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST2CEMC_20_30[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST2CEMC_30_40[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST2CEMC_40_50[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST2CEMC_50_60[0]
    };
    std::vector<double> SYST3DCEMC_Relative = {
        EMCal_Systematic_data1.RelativeUncertainty_SYST3DCEMC_0_10[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST3DCEMC_10_20[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST3DCEMC_20_30[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST3DCEMC_30_40[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST3DCEMC_40_50[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST3DCEMC_50_60[0]
    };
    std::vector<double> SYST3UCEMC_Relative = {
        EMCal_Systematic_data1.RelativeUncertainty_SYST3UCEMC_0_10[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST3UCEMC_10_20[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST3UCEMC_20_30[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST3UCEMC_30_40[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST3UCEMC_40_50[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST3UCEMC_50_60[0]
    };
    std::vector<double> SYST4CEMC_Relative = {
        EMCal_Systematic_data1.RelativeUncertainty_SYST4CEMC_0_10[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST4CEMC_10_20[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST4CEMC_20_30[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST4CEMC_30_40[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST4CEMC_40_50[0],
        EMCal_Systematic_data1.RelativeUncertainty_SYST4CEMC_50_60[0]
    };

    std::vector<double> Relative_QuadSum = {
        EMCal_Systematic_data1.Relative_QuadSum_0_10[0],
        EMCal_Systematic_data1.Relative_QuadSum_10_20[0],
        EMCal_Systematic_data1.Relative_QuadSum_20_30[0],
        EMCal_Systematic_data1.Relative_QuadSum_30_40[0],
        EMCal_Systematic_data1.Relative_QuadSum_40_50[0],
        EMCal_Systematic_data1.Relative_QuadSum_50_60[0]
    };
    
    /*
     EMCal systematics contributors bar chart Relative
     */
    std::vector<TH1F*> histograms_Relative; // Vector to hold dummy histogram pointers for legend
    std::vector<double> collect_Values_Relative; // Vector to collect all values for determining the maximum value for Y-axis scaling later.
    TCanvas *c_Overlay_Systematics_Relative = new TCanvas("c_Overlay_Systematics_Relative", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    // Define the frame of the plot with x-axis and y-axis limits (upper y is placeholder, is automated)
    TH1F *frame_Relative = c_Overlay_Systematics_Relative->DrawFrame(0, 0, 60, 0.62);
    frame_Relative->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_Relative->GetXaxis()->SetTitle("Centrality [%]");
    // Loop over the centrality centers to plot bars for each category of systematic uncertainty.
    for (size_t i = 0; i < centralityCenters.size(); ++i) {
        // Calculate the starting x position for the bars w/ adjusting for the bar width.
        double x = centralityCenters[i] - (3 * barWidth);
        // Retrieve systematic uncertainty values for each category
        std::vector<double> values_Relative = {
            SYST1CEMC_Relative[i],
            SYST2CEMC_Relative[i],
            SYST3DCEMC_Relative[i],
            SYST3UCEMC_Relative[i],
            SYST4CEMC_Relative[i],
            Relative_QuadSum[i]
        };
        /*
         Append all relative values to the global collector for max value computation later
         -- Done like this since in next graph, I skip a centrality bin that overwelms plot to zoom in, and want this taken into acct in max y calculation
         */
        collect_Values_Relative.insert(collect_Values_Relative.end(), values_Relative.begin(), values_Relative.end());
        for (size_t j = 0; j < values_Relative.size(); ++j) {
            double xj = x + j * barWidth; // Position each bar next to the previous one.
            TH1F *h_Relative = new TH1F(Form("h_Relative_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_Relative->SetBinContent(1, values_Relative[j]);
            h_Relative->SetFillColor(colors[j]);
            h_Relative->Draw("SAME");
            histograms_Relative.push_back(h_Relative);

        }
    }
    double maxYValue_Relative = FindMaxValueFromPlottedData(collect_Values_Relative);
    AdjustFrameYAxis(c_Overlay_Systematics_Relative, frame_Relative, maxYValue_Relative * 1.05);
    DrawLegend_EMCal_Contributors(0.6, 0.65, 0.8, .9, 0.035, "Relative", histograms_Relative, labels);
    c_Overlay_Systematics_Relative->Modified();
    c_Overlay_Systematics_Relative->Update();
    c_Overlay_Systematics_Relative->SaveAs((baseDataPath_EmCal_Systematics + "EMCal_SystContributor_Overlay_RelativeUncertainty.png").c_str());
    
    //EMCal systematics contributors bar chart Relative Zoomed in
    std::vector<TH1F*> histograms_Relative_Zoomed_In;
    std::vector<double> collect_Values_Relative_Zoomed_In;
    TCanvas *c_Overlay_Systematics_Relative_Zoomed_In = new TCanvas("c_Overlay_Systematics_Relative_Zoomed_In", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    TH1F *frame_Relative_Zoomed_In = c_Overlay_Systematics_Relative_Zoomed_In->DrawFrame(0, 0, 60, 0.62);
    frame_Relative_Zoomed_In->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_Relative_Zoomed_In->GetXaxis()->SetTitle("p_{T} [GeV]");
    for (size_t i = 0; i < centralityCenters.size(); ++i) {
        if (std::abs(centralityCenters[i] - 5) < EPSILON) {
            continue;  // Skip the rest of the loop for this pT center
        }
        double x = centralityCenters[i] - (2.5 * barWidth);
        std::vector<double> values_Relative_Zoomed_In = {
            SYST1CEMC_Relative[i],
            SYST2CEMC_Relative[i],
            SYST3DCEMC_Relative[i],
            SYST3UCEMC_Relative[i],
            SYST4CEMC_Relative[i],
            Relative_QuadSum[i]
        };
        
        collect_Values_Relative_Zoomed_In.insert(collect_Values_Relative_Zoomed_In.end(), values_Relative_Zoomed_In.begin(), values_Relative_Zoomed_In.end());
        for (size_t j = 0; j < values_Relative_Zoomed_In.size(); ++j) {
            double xj = x + j * barWidth;
            TH1F *h_Relative_Zoomed_In = new TH1F(Form("h_Relative_Zoomed_In_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_Relative_Zoomed_In->SetBinContent(1, values_Relative_Zoomed_In[j]);
            h_Relative_Zoomed_In->SetFillColor(colors[j]);
            h_Relative_Zoomed_In->Draw("SAME");
            histograms_Relative_Zoomed_In.push_back(h_Relative_Zoomed_In);
        }
    }
    double maxYValue_Relative_Zoomed_In = FindMaxValueFromPlottedData(collect_Values_Relative_Zoomed_In);
    AdjustFrameYAxis(c_Overlay_Systematics_Relative_Zoomed_In, frame_Relative_Zoomed_In, maxYValue_Relative_Zoomed_In * 1.05);
    DrawLegend_EMCal_Contributors(0.57, 0.65, 0.67, .9, 0.035, "Relative", histograms_Relative_Zoomed_In, labels);
    c_Overlay_Systematics_Relative_Zoomed_In->Modified();
    c_Overlay_Systematics_Relative_Zoomed_In->Update();
    c_Overlay_Systematics_Relative_Zoomed_In->SaveAs((baseDataPath_EmCal_Systematics + "EMCal_SystContributor_Overlay_Relative_Zoomed_In_Uncertainty.png").c_str());
    
    
    std::vector<double> SYST1CEMC_Absolute = {
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST1CEMC_0_10[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST1CEMC_10_20[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST1CEMC_20_30[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST1CEMC_30_40[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST1CEMC_40_50[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST1CEMC_50_60[0]
    };
    std::vector<double> SYST2CEMC_Absolute = {
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST2CEMC_0_10[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST2CEMC_10_20[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST2CEMC_20_30[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST2CEMC_30_40[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST2CEMC_40_50[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST2CEMC_50_60[0]
    };
    std::vector<double> SYST3DCEMC_Absolute = {
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST3DCEMC_0_10[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST3DCEMC_10_20[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST3DCEMC_20_30[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST3DCEMC_30_40[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST3DCEMC_40_50[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST3DCEMC_50_60[0]
    };
    std::vector<double> SYST3UCEMC_Absolute = {
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST3UCEMC_0_10[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST3UCEMC_10_20[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST3UCEMC_20_30[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST3UCEMC_30_40[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST3UCEMC_40_50[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST3UCEMC_50_60[0]
    };
    std::vector<double> SYST4CEMC_Absolute = {
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST4CEMC_0_10[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST4CEMC_10_20[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST4CEMC_20_30[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST4CEMC_30_40[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST4CEMC_40_50[0],
        EMCal_Systematic_data1.AbsoluteUncertainty_SYST4CEMC_50_60[0]
    };

    std::vector<double> Absolute_QuadSum = {
        EMCal_Systematic_data1.Absolute_QuadSum_0_10[0],
        EMCal_Systematic_data1.Absolute_QuadSum_10_20[0],
        EMCal_Systematic_data1.Absolute_QuadSum_20_30[0],
        EMCal_Systematic_data1.Absolute_QuadSum_30_40[0],
        EMCal_Systematic_data1.Absolute_QuadSum_40_50[0],
        EMCal_Systematic_data1.Absolute_QuadSum_50_60[0]
    };
    
    /*
     EMCal systematics contributors bar chart Absolute
     */
    std::vector<TH1F*> histograms_Absolute; // Vector to hold dummy histogram pointers for legend
    std::vector<double> collect_Values_Absolute; // Vector to collect all values for determining the maximum value for Y-axis scaling later.
    TCanvas *c_Overlay_Systematics_Absolute = new TCanvas("c_Overlay_Systematics_Absolute", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    // Define the frame of the plot with x-axis and y-axis limits (upper y is placeholder, is automated)
    TH1F *frame_Absolute = c_Overlay_Systematics_Absolute->DrawFrame(0, 0, 60, 0.62);
    frame_Absolute->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_Absolute->GetXaxis()->SetTitle("Centrality [%]");
    // Loop over the centrality centers to plot bars for each category of systematic uncertainty.
    for (size_t i = 0; i < centralityCenters.size(); ++i) {
        // Calculate the starting x position for the bars w/ adjusting for the bar width.
        double x = centralityCenters[i] - (3 * barWidth);
        // Retrieve systematic uncertainty values for each category
        std::vector<double> values_Absolute = {
            SYST1CEMC_Absolute[i],
            SYST2CEMC_Absolute[i],
            SYST3DCEMC_Absolute[i],
            SYST3UCEMC_Absolute[i],
            SYST4CEMC_Absolute[i],
            Absolute_QuadSum[i]
        };
        /*
         Append all Absolute values to the global collector for max value computation later
         -- Done like this since in next graph, I skip a centrality bin that overwelms plot to zoom in, and want this taken into acct in max y calculation
         */
        collect_Values_Absolute.insert(collect_Values_Absolute.end(), values_Absolute.begin(), values_Absolute.end());
        // Iterate over each systematic uncertainty category for the current pT value.
        for (size_t j = 0; j < values_Absolute.size(); ++j) {
            double xj = x + j * barWidth; // Position each bar next to the previous one.
            TH1F *h_Absolute = new TH1F(Form("h_Absolute_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_Absolute->SetBinContent(1, values_Absolute[j]);
            h_Absolute->SetFillColor(colors[j]);
            h_Absolute->Draw("SAME");
            histograms_Absolute.push_back(h_Absolute);
        }
    }
    double maxYValue_Absolute = FindMaxValueFromPlottedData(collect_Values_Absolute);
    AdjustFrameYAxis(c_Overlay_Systematics_Absolute, frame_Absolute, maxYValue_Absolute * 1.05);
    DrawLegend_EMCal_Contributors(0.58, 0.65, 0.78, .9, 0.035, "Absolute", histograms_Absolute, labels);
    c_Overlay_Systematics_Absolute->Modified();
    c_Overlay_Systematics_Absolute->Update();
    c_Overlay_Systematics_Absolute->SaveAs((baseDataPath_EmCal_Systematics + "EMCal_SystContributor_Overlay_AbsoluteUncertainty.png").c_str());
}
/*
 Legend generation for total systematic uncertainty contributors
 */
void Legend_Total_SystematicContributions(double x1, double y1, double x2, double y2, double textSize, const std::string& headerType, const std::vector<TH1F*>& histograms, const std::vector<std::string>& labels) {
    TLegend *legend = new TLegend(x1, y1, x2, y2);
    legend->SetTextSize(textSize);
    std::string header = "#bf{" + headerType + "}";
    legend->SetHeader(header.c_str(), "L");
    for (size_t i = 0; i < histograms.size(); ++i) {
        if (i < labels.size()) {
            legend->AddEntry(histograms[i], labels[i].c_str(), "f");
        }
    }
    legend->Draw();
}
//For plotting total systematic uncertainty contributions read in from StatUncertaintyTable_p015.csv
void plotting_Systematics(const Data& data1) {
    double barWidth = 1.2;
    int colors[] = {kRed, kBlue, kGreen+1, kGray+1};
    std::vector<std::string> labels_Total_SystematicContributors = {"EMCal Scale", "Signal Window", "Background Window", "Final Systematic Uncertainty"};
    const double EPSILON = 1e-6; //used to skip pT bins for a zoomed in plot in relative syst bar charts
    std::vector<double> relative_signalWindow = {
        data1.signal_window_syst_0_10[0],
        data1.signal_window_syst_10_20[0],
        data1.signal_window_syst_20_30[0],
        data1.signal_window_syst_30_40[0],
        data1.signal_window_syst_40_50[0],
        data1.signal_window_syst_50_60[0]
    };
    std::vector<double> relative_BackgroundWindow = {
        data1.background_window_syst_0_10[0],
        data1.background_window_syst_10_20[0],
        data1.background_window_syst_20_30[0],
        data1.background_window_syst_30_40[0],
        data1.background_window_syst_40_50[0],
        data1.background_window_syst_50_60[0]
    };
    std::vector<double> relative_EMCalContribution = {
        data1.quad_sum_EMCal_syst_0_10[0],
        data1.quad_sum_EMCal_syst_10_20[0],
        data1.quad_sum_EMCal_syst_20_30[0],
        data1.quad_sum_EMCal_syst_30_40[0],
        data1.quad_sum_EMCal_syst_40_50[0],
        data1.quad_sum_EMCal_syst_50_60[0]
    };
    std::vector<double> relative_totalSystematic = {
        data1.unWeighted_total_syst_uncertainties_0_10[0],
        data1.unWeighted_total_syst_uncertainties_10_20[0],
        data1.unWeighted_total_syst_uncertainties_20_30[0],
        data1.unWeighted_total_syst_uncertainties_30_40[0],
        data1.unWeighted_total_syst_uncertainties_40_50[0],
        data1.unWeighted_total_syst_uncertainties_50_60[0]
    };
    

    // Relative Uncertainty, include all data
    std::vector<TH1F*> histograms_RelativeUncertainty;
    std::vector<double> collect_Values_Relative; //used for y axis scaling
    TCanvas *c_Overlay_Systematics_RelativeUncertainty = new TCanvas("c_Overlay_Systematics_RelativeUncertainty", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    TH1F *frame_RelativeUncertainty = c_Overlay_Systematics_RelativeUncertainty->DrawFrame(0, 0, 60, 0.7);
    frame_RelativeUncertainty->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_RelativeUncertainty->GetXaxis()->SetTitle("Centrality [%]");

    for (size_t i = 0; i < centralityCenters.size(); ++i) {
        double x = centralityCenters[i] - (2.5 * barWidth);
        
        std::vector<double> values_RelativeUncertainty_0_20 = {
            relative_EMCalContribution[i],
            relative_signalWindow[i],
            relative_BackgroundWindow[i],
            relative_totalSystematic[i]
        };
        collect_Values_Relative.insert(collect_Values_Relative.end(), values_RelativeUncertainty_0_20.begin(), values_RelativeUncertainty_0_20.end());

        for (size_t j = 0; j < values_RelativeUncertainty_0_20.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_RelativeUncertainty = new TH1F(Form("h_RelativeUncertainty_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_RelativeUncertainty->SetBinContent(1, values_RelativeUncertainty_0_20[j]);
            h_RelativeUncertainty->SetFillColor(colors[j]);
            h_RelativeUncertainty->Draw("SAME");
            histograms_RelativeUncertainty.push_back(h_RelativeUncertainty);
        }
        
    }
    double maxYValue_Relative = FindMaxValueFromPlottedData(collect_Values_Relative);
    AdjustFrameYAxis(c_Overlay_Systematics_RelativeUncertainty, frame_RelativeUncertainty, maxYValue_Relative * 1.05);
    Legend_Total_SystematicContributions(0.6, 0.72, 0.8, 0.92, 0.03, "Relative Systematics", histograms_RelativeUncertainty, labels_Total_SystematicContributors);
    c_Overlay_Systematics_RelativeUncertainty->SaveAs((SystematicsBasePlotOutput + "RelativeSystUncertainty_Overlay.png").c_str());
    
    // same plot as above but zoomed in on pT bins that were hard to see
    std::vector<TH1F*> histograms_RelativeUncertainty_ZoomedIn;
    std::vector<double> collect_Values_Relative_ZoomedIn;
    TCanvas *c_Overlay_Systematics_RelativeUncertainty_ZoomedIn = new TCanvas("c_Overlay_Systematics_RelativeUncertainty_ZoomedIn", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    TH1F *frame_RelativeUncertainty_ZoomedIn = c_Overlay_Systematics_RelativeUncertainty_ZoomedIn->DrawFrame(0, 0, 60, 0.7);
    frame_RelativeUncertainty_ZoomedIn->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_RelativeUncertainty_ZoomedIn->GetXaxis()->SetTitle("Centrality [%]");
    for (size_t i = 0; i < centralityCenters.size(); ++i) {
        if (std::abs(centralityCenters[i] - 5) < EPSILON) {
            continue;
        }
        double x = centralityCenters[i] - (2.5 * barWidth);
        std::vector<double> values_RelativeUncertainty_ZoomedIn = {
            relative_EMCalContribution[i],
            relative_signalWindow[i],
            relative_BackgroundWindow[i],
            relative_totalSystematic[i]
        };
        collect_Values_Relative_ZoomedIn.insert(collect_Values_Relative_ZoomedIn.end(), values_RelativeUncertainty_ZoomedIn.begin(), values_RelativeUncertainty_ZoomedIn.end());
        for (size_t j = 0; j < values_RelativeUncertainty_ZoomedIn.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_RelativeUncertainty_ZoomedIn = new TH1F(Form("h_RelativeUncertainty_ZoomedIn_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_RelativeUncertainty_ZoomedIn->SetBinContent(1, values_RelativeUncertainty_ZoomedIn[j]);
            h_RelativeUncertainty_ZoomedIn->SetFillColor(colors[j]);
            h_RelativeUncertainty_ZoomedIn->Draw("SAME");
            histograms_RelativeUncertainty_ZoomedIn.push_back(h_RelativeUncertainty_ZoomedIn);
        }
        
    }
    double maxYValue_Relative_ZoomedIn = FindMaxValueFromPlottedData(collect_Values_Relative_ZoomedIn);
    AdjustFrameYAxis(c_Overlay_Systematics_RelativeUncertainty_ZoomedIn, frame_RelativeUncertainty_ZoomedIn, maxYValue_Relative_ZoomedIn * 1.05);
    Legend_Total_SystematicContributions(0.55, 0.72, 0.75, 0.92, 0.028, "Relative Systematics", histograms_RelativeUncertainty_ZoomedIn, labels_Total_SystematicContributors);
    c_Overlay_Systematics_RelativeUncertainty_ZoomedIn->SaveAs((SystematicsBasePlotOutput + "RelativeSystUncertainty_Overlay_Zoomed_In.png").c_str());

    
    
    std::vector<double> absolute_signalWindow = {
        data1.signal_window_syst_0_10[0] * std::abs(data1.corrected_v2_0_10[0]),
        data1.signal_window_syst_10_20[0] * std::abs(data1.corrected_v2_10_20[0]),
        data1.signal_window_syst_20_30[0] * std::abs(data1.corrected_v2_20_30[0]),
        data1.signal_window_syst_30_40[0] * std::abs(data1.corrected_v2_30_40[0]),
        data1.signal_window_syst_40_50[0] * std::abs(data1.corrected_v2_40_50[0]),
        data1.signal_window_syst_50_60[0] * std::abs(data1.corrected_v2_50_60[0])
    };
    std::vector<double> absolute_BackgroundWindow = {
        data1.background_window_syst_0_10[0] * std::abs(data1.corrected_v2_0_10[0]),
        data1.background_window_syst_10_20[0] * std::abs(data1.corrected_v2_10_20[0]),
        data1.background_window_syst_20_30[0] * std::abs(data1.corrected_v2_20_30[0]),
        data1.background_window_syst_30_40[0] * std::abs(data1.corrected_v2_30_40[0]),
        data1.background_window_syst_40_50[0] * std::abs(data1.corrected_v2_40_50[0]),
        data1.background_window_syst_50_60[0] * std::abs(data1.corrected_v2_50_60[0])
    };
    std::vector<double> absolute_EMCalContribution = {
        data1.quad_sum_EMCal_syst_0_10[0] * std::abs(data1.corrected_v2_0_10[0]),
        data1.quad_sum_EMCal_syst_10_20[0] * std::abs(data1.corrected_v2_10_20[0]),
        data1.quad_sum_EMCal_syst_20_30[0] * std::abs(data1.corrected_v2_20_30[0]),
        data1.quad_sum_EMCal_syst_30_40[0] * std::abs(data1.corrected_v2_30_40[0]),
        data1.quad_sum_EMCal_syst_40_50[0] * std::abs(data1.corrected_v2_40_50[0]),
        data1.quad_sum_EMCal_syst_50_60[0] * std::abs(data1.corrected_v2_50_60[0])
    };
    std::vector<double> absolute_totalSystematic = {
        data1.total_syst_uncertainties_0_10[0],
        data1.total_syst_uncertainties_10_20[0],
        data1.total_syst_uncertainties_20_30[0],
        data1.total_syst_uncertainties_30_40[0],
        data1.total_syst_uncertainties_40_50[0],
        data1.total_syst_uncertainties_50_60[0]
    };
    
    std::vector<TH1F*> histograms_absoluteUncertainty;
    std::vector<double> collect_Values_absolute; //used for y axis scaling
    TCanvas *c_Overlay_Systematics_absoluteUncertainty = new TCanvas("c_Overlay_Systematics_absoluteUncertainty", "#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality", 800, 600);
    TH1F *frame_absoluteUncertainty = c_Overlay_Systematics_absoluteUncertainty->DrawFrame(0, 0, 60, 0.7);
    frame_absoluteUncertainty->GetYaxis()->SetTitle("Systematic Uncertainty");
    frame_absoluteUncertainty->GetXaxis()->SetTitle("Centrality [%]");

    for (size_t i = 0; i < centralityCenters.size(); ++i) {
        double x = centralityCenters[i] - (2.5 * barWidth);
        
        std::vector<double> values_absoluteUncertainty_0_20 = {
            absolute_EMCalContribution[i],
            absolute_signalWindow[i],
            absolute_BackgroundWindow[i],
            absolute_totalSystematic[i]
        };
        collect_Values_absolute.insert(collect_Values_absolute.end(), values_absoluteUncertainty_0_20.begin(), values_absoluteUncertainty_0_20.end());

        for (size_t j = 0; j < values_absoluteUncertainty_0_20.size(); ++j) {
            double xj = x + j * barWidth; // Adjust x position for each bar
            TH1F *h_absoluteUncertainty = new TH1F(Form("h_absoluteUncertainty_%zu_%zu", i, j), "", 1, xj, xj + barWidth);
            h_absoluteUncertainty->SetBinContent(1, values_absoluteUncertainty_0_20[j]);
            h_absoluteUncertainty->SetFillColor(colors[j]);
            h_absoluteUncertainty->Draw("SAME");
            histograms_absoluteUncertainty.push_back(h_absoluteUncertainty);
        }
        
    }
    double maxYValue_absolute = FindMaxValueFromPlottedData(collect_Values_absolute);
    AdjustFrameYAxis(c_Overlay_Systematics_absoluteUncertainty, frame_absoluteUncertainty, maxYValue_absolute * 1.05);
    Legend_Total_SystematicContributions(0.52, 0.72, 0.72, 0.92, 0.03, "Absolute Uncertainties", histograms_absoluteUncertainty, labels_Total_SystematicContributors);
    c_Overlay_Systematics_absoluteUncertainty->SaveAs((SystematicsBasePlotOutput + "absoluteSystUncertainty_Overlay.png").c_str());
}
/*
 Plot overlay of EMCal scale variations v2 values
 */
void plot_EMCal_Scale_v2_overlays(Data& data1, Data& data2, Data& data3, Data& data4, Data& data5, Data& data6) {
    // Prepare the graph
    TGraphErrors* graph_1 = new TGraphErrors();
    int pointIndex = 0;

    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_1;
    values_1.push_back(&data1.corrected_v2_0_10);
    values_1.push_back(&data1.corrected_v2_10_20);
    values_1.push_back(&data1.corrected_v2_20_30);
    values_1.push_back(&data1.corrected_v2_30_40);
    values_1.push_back(&data1.corrected_v2_40_50);
    values_1.push_back(&data1.corrected_v2_50_60);

    std::vector<std::vector<double>*> errors_1;
    errors_1.push_back(&data1.corrected_v2_0_10_Errors);
    errors_1.push_back(&data1.corrected_v2_10_20_Errors);
    errors_1.push_back(&data1.corrected_v2_20_30_Errors);
    errors_1.push_back(&data1.corrected_v2_30_40_Errors);
    errors_1.push_back(&data1.corrected_v2_40_50_Errors);
    errors_1.push_back(&data1.corrected_v2_50_60_Errors);

    
    TGraphErrors* graph_2 = new TGraphErrors();
    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_2;
    values_2.push_back(&data2.corrected_v2_0_10);
    values_2.push_back(&data2.corrected_v2_10_20);
    values_2.push_back(&data2.corrected_v2_20_30);
    values_2.push_back(&data2.corrected_v2_30_40);
    values_2.push_back(&data2.corrected_v2_40_50);
    values_2.push_back(&data2.corrected_v2_50_60);

    std::vector<std::vector<double>*> errors_2;
    errors_2.push_back(&data2.corrected_v2_0_10_Errors);
    errors_2.push_back(&data2.corrected_v2_10_20_Errors);
    errors_2.push_back(&data2.corrected_v2_20_30_Errors);
    errors_2.push_back(&data2.corrected_v2_30_40_Errors);
    errors_2.push_back(&data2.corrected_v2_40_50_Errors);
    errors_2.push_back(&data2.corrected_v2_50_60_Errors);
    
    
    TGraphErrors* graph_3 = new TGraphErrors();
    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_3;
    values_3.push_back(&data3.corrected_v2_0_10);
    values_3.push_back(&data3.corrected_v2_10_20);
    values_3.push_back(&data3.corrected_v2_20_30);
    values_3.push_back(&data3.corrected_v2_30_40);
    values_3.push_back(&data3.corrected_v2_40_50);
    values_3.push_back(&data3.corrected_v2_50_60);

    std::vector<std::vector<double>*> errors_3;
    errors_3.push_back(&data3.corrected_v2_0_10_Errors);
    errors_3.push_back(&data3.corrected_v2_10_20_Errors);
    errors_3.push_back(&data3.corrected_v2_20_30_Errors);
    errors_3.push_back(&data3.corrected_v2_30_40_Errors);
    errors_3.push_back(&data3.corrected_v2_40_50_Errors);
    errors_3.push_back(&data3.corrected_v2_50_60_Errors);
    
    
    TGraphErrors* graph_4 = new TGraphErrors();
    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_4;
    values_4.push_back(&data4.corrected_v2_0_10);
    values_4.push_back(&data4.corrected_v2_10_20);
    values_4.push_back(&data4.corrected_v2_20_30);
    values_4.push_back(&data4.corrected_v2_30_40);
    values_4.push_back(&data4.corrected_v2_40_50);
    values_4.push_back(&data4.corrected_v2_50_60);

    std::vector<std::vector<double>*> errors_4;
    errors_4.push_back(&data4.corrected_v2_0_10_Errors);
    errors_4.push_back(&data4.corrected_v2_10_20_Errors);
    errors_4.push_back(&data4.corrected_v2_20_30_Errors);
    errors_4.push_back(&data4.corrected_v2_30_40_Errors);
    errors_4.push_back(&data4.corrected_v2_40_50_Errors);
    errors_4.push_back(&data4.corrected_v2_50_60_Errors);
    
    
    TGraphErrors* graph_5 = new TGraphErrors();
    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_5;
    values_5.push_back(&data5.corrected_v2_0_10);
    values_5.push_back(&data5.corrected_v2_10_20);
    values_5.push_back(&data5.corrected_v2_20_30);
    values_5.push_back(&data5.corrected_v2_30_40);
    values_5.push_back(&data5.corrected_v2_40_50);
    values_5.push_back(&data5.corrected_v2_50_60);

    std::vector<std::vector<double>*> errors_5;
    errors_5.push_back(&data5.corrected_v2_0_10_Errors);
    errors_5.push_back(&data5.corrected_v2_10_20_Errors);
    errors_5.push_back(&data5.corrected_v2_20_30_Errors);
    errors_5.push_back(&data5.corrected_v2_30_40_Errors);
    errors_5.push_back(&data5.corrected_v2_40_50_Errors);
    errors_5.push_back(&data5.corrected_v2_50_60_Errors);
    
    
    TGraphErrors* graph_6 = new TGraphErrors();
    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_6;
    values_6.push_back(&data6.corrected_v2_0_10);
    values_6.push_back(&data6.corrected_v2_10_20);
    values_6.push_back(&data6.corrected_v2_20_30);
    values_6.push_back(&data6.corrected_v2_30_40);
    values_6.push_back(&data6.corrected_v2_40_50);
    values_6.push_back(&data6.corrected_v2_50_60);

    std::vector<std::vector<double>*> errors_6;
    errors_6.push_back(&data6.corrected_v2_0_10_Errors);
    errors_6.push_back(&data6.corrected_v2_10_20_Errors);
    errors_6.push_back(&data6.corrected_v2_20_30_Errors);
    errors_6.push_back(&data6.corrected_v2_30_40_Errors);
    errors_6.push_back(&data6.corrected_v2_40_50_Errors);
    errors_6.push_back(&data6.corrected_v2_50_60_Errors);
    
    


    // Loop over each centrality range
    for (size_t i = 0; i < centralityCenters.size(); ++i) {
        for (size_t j = 0; j < values_1[i]->size(); ++j) {
            graph_1->SetPoint(pointIndex, centralityCenters[i] - 3.0, (*values_1[i])[j]);
            graph_1->SetPointError(pointIndex, 0, 0);
            
            graph_2->SetPoint(pointIndex, centralityCenters[i] - 2.0, (*values_2[i])[j]);
            graph_2->SetPointError(pointIndex, 0, 0);
            
            graph_3->SetPoint(pointIndex, centralityCenters[i] - 1.0, (*values_3[i])[j]);
            graph_3->SetPointError(pointIndex, 0, 0);
            
            graph_4->SetPoint(pointIndex, centralityCenters[i], (*values_4[i])[j]);
            graph_4->SetPointError(pointIndex, 0, 0);
            
            graph_5->SetPoint(pointIndex, centralityCenters[i] + 1.0, (*values_5[i])[j]);
            graph_5->SetPointError(pointIndex, 0, 0);
            
            graph_6->SetPoint(pointIndex, centralityCenters[i] + 2.0, (*values_6[i])[j]);
            graph_6->SetPointError(pointIndex, 0, 0);
            
            pointIndex++;
        }
    }

    // Graph properties
    graph_1->SetMarkerStyle(20);
    graph_1->SetMarkerColor(kBlack);
    graph_1->SetLineColor(kBlack);
    
    graph_2->SetMarkerStyle(20);
    graph_2->SetMarkerColor(kRed);
    graph_2->SetLineColor(kRed);
    
    graph_3->SetMarkerStyle(20);
    graph_3->SetMarkerColor(kBlue);
    graph_3->SetLineColor(kBlue);
    
    graph_4->SetMarkerStyle(20);
    graph_4->SetMarkerColor(kGreen+2);
    graph_4->SetLineColor(kGreen+2);
    
    graph_5->SetMarkerStyle(20);
    graph_5->SetMarkerColor(kOrange+7);
    graph_5->SetLineColor(kOrange+7);
    
    graph_6->SetMarkerStyle(20);
    graph_6->SetMarkerColor(kViolet-2);
    graph_6->SetLineColor(kViolet-2);

    graph_4->GetYaxis()->SetRangeUser(-0.15, 0.25);
    graph_4->GetXaxis()->SetRangeUser(0, 60);
    
    graph_4->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    graph_4->GetXaxis()->SetTitle("Centrality [%]");
    
    // Create canvas
    TCanvas* canvas = new TCanvas("c1", "Centrality Plot", 800, 600);
    graph_4->Draw("AP");
    graph_1->Draw("P SAME");
    graph_2->Draw("P SAME");
    graph_3->Draw("P SAME");
    graph_5->Draw("P SAME");
    graph_6->Draw("P SAME");
    
    TLegend *legend = new TLegend(0.5, 0.18, 0.9, 0.33);
    legend->SetNColumns(2);
    legend->SetBorderSize(0);
    legend->SetTextSize(0.028);
    legend->AddEntry(graph_1, "Default", "pe");
    legend->AddEntry(graph_2, "SYST1CEMC", "pe");
    legend->AddEntry(graph_3, "SYST2CEMC", "pe");
    legend->AddEntry(graph_4, "SYST3DCEMC", "pe");
    legend->AddEntry(graph_5, "SYST3UCEMC", "pe");
    legend->AddEntry(graph_6, "SYST4CEMC", "pe");
    legend->Draw("SAME");
    
    canvas->Update();

    // Draw zero line
    double x_min = canvas->GetUxmin();
    double x_max = canvas->GetUxmax();
    TLine* zeroLine = new TLine(x_min, 0, x_max, 0);
    zeroLine->SetLineStyle(2);
    zeroLine->Draw("SAME");

    // Save the canvas
    canvas->SaveAs((baseDataPath_EmCal_Systematics + "/v2_EMCalSystematicsOverlay_integratedOverPT.png").c_str());
}
void plot_SignalWindowVariation(Data& data1, Data& data2) {
    // Prepare the graph
    TGraphErrors* graph_1 = new TGraphErrors();
    int pointIndex = 0;

    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_1;
    values_1.push_back(&data1.corrected_v2_0_10);
    values_1.push_back(&data1.corrected_v2_10_20);
    values_1.push_back(&data1.corrected_v2_20_30);
    values_1.push_back(&data1.corrected_v2_30_40);
    values_1.push_back(&data1.corrected_v2_40_50);
    values_1.push_back(&data1.corrected_v2_50_60);

    std::vector<std::vector<double>*> errors_1;
    errors_1.push_back(&data1.corrected_v2_0_10_Errors);
    errors_1.push_back(&data1.corrected_v2_10_20_Errors);
    errors_1.push_back(&data1.corrected_v2_20_30_Errors);
    errors_1.push_back(&data1.corrected_v2_30_40_Errors);
    errors_1.push_back(&data1.corrected_v2_40_50_Errors);
    errors_1.push_back(&data1.corrected_v2_50_60_Errors);

    
    TGraphErrors* graph_2 = new TGraphErrors();
    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_2;
    values_2.push_back(&data2.corrected_v2_0_10);
    values_2.push_back(&data2.corrected_v2_10_20);
    values_2.push_back(&data2.corrected_v2_20_30);
    values_2.push_back(&data2.corrected_v2_30_40);
    values_2.push_back(&data2.corrected_v2_40_50);
    values_2.push_back(&data2.corrected_v2_50_60);

    std::vector<std::vector<double>*> errors_2;
    errors_2.push_back(&data2.corrected_v2_0_10_Errors);
    errors_2.push_back(&data2.corrected_v2_10_20_Errors);
    errors_2.push_back(&data2.corrected_v2_20_30_Errors);
    errors_2.push_back(&data2.corrected_v2_30_40_Errors);
    errors_2.push_back(&data2.corrected_v2_40_50_Errors);
    errors_2.push_back(&data2.corrected_v2_50_60_Errors);
    


    // Loop over each centrality range
    for (size_t i = 0; i < centralityCenters.size(); ++i) {
        for (size_t j = 0; j < values_1[i]->size(); ++j) {
            graph_1->SetPoint(pointIndex, centralityCenters[i] - 0.8, (*values_1[i])[j]);
            graph_1->SetPointError(pointIndex, 0, 0);
            
            graph_2->SetPoint(pointIndex, centralityCenters[i] + 0.8, (*values_2[i])[j]);
            graph_2->SetPointError(pointIndex, 0, 0);
            
            pointIndex++;
        }
    }

    // Graph properties
    graph_1->SetMarkerStyle(20);
    graph_1->SetMarkerColor(kBlack);
    graph_1->SetLineColor(kBlack);
    graph_1->GetYaxis()->SetRangeUser(-0.1, 0.22);
    
    graph_2->SetMarkerStyle(20);
    graph_2->SetMarkerColor(kBlue);
    graph_2->SetLineColor(kBlue);
    
    graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    graph_1->GetXaxis()->SetTitle("Centrality [%]");
    
    // Create canvas
    TCanvas* Signal = new TCanvas("Signal", "Centrality Plot", 800, 600);
    graph_1->Draw("AP");
    graph_2->Draw("P SAME");
    
    TLegend *legend = new TLegend(0.5, 0.18, 0.9, 0.33);
    legend->SetBorderSize(0);
    legend->SetTextSize(0.028);
    legend->AddEntry(graph_1, "Signal Window = #mu #pm 2.00#sigma (Default)", "pe");
    legend->AddEntry(graph_2, "Signal Window = #mu #pm 1.50#sigma", "pe");

    legend->Draw("SAME");
    
    Signal->Update();

    // Draw zero line
    double x_min = Signal->GetUxmin();
    double x_max = Signal->GetUxmax();
    TLine* zeroLine = new TLine(x_min, 0, x_max, 0);
    zeroLine->SetLineStyle(2);
    zeroLine->Draw("SAME");

    // Save the canvas
    Signal->SaveAs((SystematicsBasePlotOutput + "/v2_SignalWindowVariation_integratedOverPT.png").c_str());
}
void plot_BackgroundWindowVariation(Data& data1) {
    // Prepare the graph
    TGraphErrors* graph_1 = new TGraphErrors();
    int pointIndex = 0;

    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_1;
    values_1.push_back(&data1.corrected_v2_0_10);
    values_1.push_back(&data1.corrected_v2_10_20);
    values_1.push_back(&data1.corrected_v2_20_30);
    values_1.push_back(&data1.corrected_v2_30_40);
    values_1.push_back(&data1.corrected_v2_40_50);
    values_1.push_back(&data1.corrected_v2_50_60);

    std::vector<std::vector<double>*> errors_1;
    errors_1.push_back(&data1.corrected_v2_0_10_Errors);
    errors_1.push_back(&data1.corrected_v2_10_20_Errors);
    errors_1.push_back(&data1.corrected_v2_20_30_Errors);
    errors_1.push_back(&data1.corrected_v2_30_40_Errors);
    errors_1.push_back(&data1.corrected_v2_40_50_Errors);
    errors_1.push_back(&data1.corrected_v2_50_60_Errors);

    
    TGraphErrors* graph_2 = new TGraphErrors();
    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_2;
    values_2.push_back(&data1.corrected_v2_0_10_type4);
    values_2.push_back(&data1.corrected_v2_10_20_type4);
    values_2.push_back(&data1.corrected_v2_20_30_type4);
    values_2.push_back(&data1.corrected_v2_30_40_type4);
    values_2.push_back(&data1.corrected_v2_40_50_type4);
    values_2.push_back(&data1.corrected_v2_50_60_type4);

    std::vector<std::vector<double>*> errors_2;
    errors_2.push_back(&data1.corrected_v2_0_10_Errors_type4);
    errors_2.push_back(&data1.corrected_v2_10_20_Errors_type4);
    errors_2.push_back(&data1.corrected_v2_20_30_Errors_type4);
    errors_2.push_back(&data1.corrected_v2_30_40_Errors_type4);
    errors_2.push_back(&data1.corrected_v2_40_50_Errors_type4);
    errors_2.push_back(&data1.corrected_v2_50_60_Errors_type4);


    // Loop over each centrality range
    for (size_t i = 0; i < centralityCenters.size(); ++i) {
        for (size_t j = 0; j < values_1[i]->size(); ++j) {
            graph_1->SetPoint(pointIndex, centralityCenters[i] - 0.8, (*values_1[i])[j]);
            graph_1->SetPointError(pointIndex, 0, 0);
            
            graph_2->SetPoint(pointIndex, centralityCenters[i] + 0.8, (*values_2[i])[j]);
            graph_2->SetPointError(pointIndex, 0, 0);
            
            pointIndex++;
        }
    }

    // Graph properties
    graph_1->SetMarkerStyle(20);
    graph_1->SetMarkerColor(kBlack);
    graph_1->SetLineColor(kBlack);
    graph_1->GetYaxis()->SetRangeUser(-0.1, 0.22);
    
    graph_2->SetMarkerStyle(20);
    graph_2->SetMarkerColor(kGreen+2);
    graph_2->SetLineColor(kGreen+2);
    
    graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    graph_1->GetXaxis()->SetTitle("Centrality [%]");
    
    // Create canvas
    TCanvas* Background = new TCanvas("Background", "Centrality Plot", 800, 600);
    graph_1->Draw("AP");
    graph_2->Draw("P SAME");
    
    TLegend *legend = new TLegend(0.5, 0.18, 0.9, 0.33);
    legend->SetBorderSize(0);
    legend->SetTextSize(0.028);
    legend->AddEntry(graph_1, "Sideband = [#mu + 3#sigma, 0.5] GeV (Default)", "pe");
    legend->AddEntry(graph_2, "Sideband = [#mu + 3#sigma, 0.4] GeV", "pe");

    legend->Draw("SAME");
    
    Background->Update();

    // Draw zero line
    double x_min = Background->GetUxmin();
    double x_max = Background->GetUxmax();
    TLine* zeroLine = new TLine(x_min, 0, x_max, 0);
    zeroLine->SetLineStyle(2);
    zeroLine->Draw("SAME");

    // Save the canvas
    Background->SaveAs((SystematicsBasePlotOutput + "/v2_BackgroundWindowVariation_integratedOverPT.png").c_str());
}
void plotHijingOverlay(Data& data1, Data& data2, Data& data3) {
    // Prepare the graph
    TGraphErrors* graph_1 = new TGraphErrors();
    int pointIndex = 0;

    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_1;
    values_1.push_back(&data1.corrected_v2_0_10);
    values_1.push_back(&data1.corrected_v2_10_20);
    values_1.push_back(&data1.corrected_v2_20_30);
    values_1.push_back(&data1.corrected_v2_30_40);
    values_1.push_back(&data1.corrected_v2_40_50);
    values_1.push_back(&data1.corrected_v2_50_60);

    std::vector<std::vector<double>*> errors_1;
    errors_1.push_back(&data1.corrected_v2_0_10_Errors);
    errors_1.push_back(&data1.corrected_v2_10_20_Errors);
    errors_1.push_back(&data1.corrected_v2_20_30_Errors);
    errors_1.push_back(&data1.corrected_v2_30_40_Errors);
    errors_1.push_back(&data1.corrected_v2_40_50_Errors);
    errors_1.push_back(&data1.corrected_v2_50_60_Errors);

    
    TGraphErrors* graph_2 = new TGraphErrors();
    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_2;
    values_2.push_back(&data2.corrected_v2_0_10);
    values_2.push_back(&data2.corrected_v2_10_20);
    values_2.push_back(&data2.corrected_v2_20_30);
    values_2.push_back(&data2.corrected_v2_30_40);
    values_2.push_back(&data2.corrected_v2_40_50);
    values_2.push_back(&data2.corrected_v2_50_60);

    std::vector<std::vector<double>*> errors_2;
    errors_2.push_back(&data2.corrected_v2_0_10_Errors);
    errors_2.push_back(&data2.corrected_v2_10_20_Errors);
    errors_2.push_back(&data2.corrected_v2_20_30_Errors);
    errors_2.push_back(&data2.corrected_v2_30_40_Errors);
    errors_2.push_back(&data2.corrected_v2_40_50_Errors);
    errors_2.push_back(&data2.corrected_v2_50_60_Errors);
    
    TGraphErrors* graph_3 = new TGraphErrors();
    // Initialize values and errors vector lists
    std::vector<std::vector<double>*> values_3;
    values_3.push_back(&data3.corrected_v2_0_10);
    values_3.push_back(&data3.corrected_v2_10_20);
    values_3.push_back(&data3.corrected_v2_20_30);
    values_3.push_back(&data3.corrected_v2_30_40);
    values_3.push_back(&data3.corrected_v2_40_50);
    values_3.push_back(&data3.corrected_v2_50_60);

    std::vector<std::vector<double>*> errors_3;
    errors_3.push_back(&data3.corrected_v2_0_10_Errors);
    errors_3.push_back(&data3.corrected_v2_10_20_Errors);
    errors_3.push_back(&data3.corrected_v2_20_30_Errors);
    errors_3.push_back(&data3.corrected_v2_30_40_Errors);
    errors_3.push_back(&data3.corrected_v2_40_50_Errors);
    errors_3.push_back(&data3.corrected_v2_50_60_Errors);
    


    // Loop over each centrality range
    for (size_t i = 0; i < centralityCenters.size(); ++i) {
        for (size_t j = 0; j < values_1[i]->size(); ++j) {
            graph_1->SetPoint(pointIndex, centralityCenters[i] - 0.8, (*values_1[i])[j]);
            graph_1->SetPointError(pointIndex, 0, (*errors_1[i])[j]);
            
            graph_2->SetPoint(pointIndex, centralityCenters[i], (*values_2[i])[j]);
            graph_2->SetPointError(pointIndex, 0, (*errors_2[i])[j]);
            
            graph_3->SetPoint(pointIndex, centralityCenters[i] + 0.8, (*values_3[i])[j]);
            graph_3->SetPointError(pointIndex, 0, (*errors_3[i])[j]);
            
            pointIndex++;
        }
    }

    // Graph properties
    graph_1->SetMarkerStyle(20);
    graph_1->SetMarkerColor(kBlack);
    graph_1->SetLineColor(kBlack);
    graph_1->GetXaxis()->SetLimits(0, 60.0);

    
    graph_2->SetMarkerStyle(20);
    graph_2->SetMarkerColor(kRed);
    graph_2->SetLineColor(kRed);
    
    graph_3->SetMarkerStyle(20);
    graph_3->SetMarkerColor(kBlue);
    graph_3->SetLineColor(kBlue);
    
    graph_1->GetYaxis()->SetTitle("v_{2}^{#pi^{0}}");
    graph_1->GetXaxis()->SetTitle("Centrality [%]");
    
    // Create canvas
    TCanvas* Hijing = new TCanvas("Hijing", "Centrality Plot", 800, 600);
    graph_1->Draw("AP");
    graph_2->Draw("P SAME");
    graph_3->Draw("P SAME");
    
    // Set Y axis range
    graph_1->GetYaxis()->SetRangeUser(0, 0.5);

    TLegend *legend = new TLegend(0.7, 0.18, 0.9, 0.33);
    legend->SetBorderSize(0);
    legend->SetTextSize(0.028);
    legend->AddEntry(graph_1, "Default", "pe");
    legend->AddEntry(graph_2, "Observed", "pe");
    legend->AddEntry(graph_3, "Flow Afterburner", "pe");

    legend->Draw("SAME");
    
    Hijing->Update();

    // Draw zero line
    double x_min = Hijing->GetUxmin();
    double x_max = Hijing->GetUxmax();
    TLine* zeroLine = new TLine(x_min, 0, x_max, 0);
    zeroLine->SetLineStyle(2);
    zeroLine->Draw("SAME");

    // Save the canvas
    Hijing->SaveAs("/Users/patsfan753/Desktop/v2_HijingOverly.png");
}
void FinalizeResults_IntegratedPT() {
    gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();
    Data defaultData, data_SYST1CEMC, data_SYST2CEMC, data_SYST3DCEMC, data_SYST3UCEMC, data_SYST4CEMC, data_Signal_Bound_Variation;
    
    std::string ReferenceData = BaseCSVoutput + "/vn-anaType-1-p015.csv";
    std::string filePathEMCal_Syst_SYST1CEMC = BaseCSVoutput + "/vn-SYST1CEMC.csv";
    std::string filePathEMCal_Syst_SYST2CEMC = BaseCSVoutput + "/vn-SYST2CEMC.csv";
    std::string filePathEMCal_Syst_SYST3DCEMC = BaseCSVoutput + "/vn-SYST3DCEMC.csv";
    std::string filePathEMCal_Syst_SYST3UCEMC = BaseCSVoutput + "/vn-SYST3UCEMC.csv";
    std::string filePathEMCal_Syst_SYST4CEMC = BaseCSVoutput + "/vn-SYST4CEMC.csv";
    std::string filePathSignal_Bound_Variation = BaseCSVoutput + "/vn-sigma-1.5.csv";
    // Read the default data set
    Read_DataSet(ReferenceData, defaultData);
    // Read v2 values from each EMCal scale variation
    Read_DataSet(filePathEMCal_Syst_SYST1CEMC, data_SYST1CEMC);
    Read_DataSet(filePathEMCal_Syst_SYST2CEMC, data_SYST2CEMC);
    Read_DataSet(filePathEMCal_Syst_SYST3DCEMC, data_SYST3DCEMC);
    Read_DataSet(filePathEMCal_Syst_SYST3UCEMC, data_SYST3UCEMC);
    Read_DataSet(filePathEMCal_Syst_SYST4CEMC, data_SYST4CEMC);
    std::vector<Data> emCalDataSets = {defaultData, data_SYST1CEMC, data_SYST2CEMC, data_SYST3DCEMC, data_SYST3UCEMC, data_SYST4CEMC};
    Read_DataSet(filePathSignal_Bound_Variation, data_Signal_Bound_Variation);
    
    std::string statUncertaintyFilePath_EMCal_Contributors = baseDataPath_EmCal_Systematics + "StatUncertaintyTable_EMCalVariationsOnly.csv";
    //write and read path for CSV of total syst contributors and calculation
    std::string statUncertaintyFilePath = SystematicsBasePlotOutput + "StatUncertaintyTable_p015.csv";
    
    WriteComparisonTo_EMcalScale_CSV(emCalDataSets, defaultData, statUncertaintyFilePath_EMCal_Contributors); //calculate EMCal scale contributors and quadarture sum
    WriteComparisonToCSV(emCalDataSets, data_Signal_Bound_Variation, defaultData, statUncertaintyFilePath); //calculate total systematic uncertainty
    ReadStatUncertainties(statUncertaintyFilePath, defaultData); //read total systematic uncertainty CSV
    
    plotting_Results(defaultData); //plot final results and PHENIX overlay
    plotting_Systematics(defaultData); //plot total systematic contributions
    
    EMCal_Systematic_Data EMCal_Systematic_data1;
    ReadStatUncertainties_emCalScaleCSV(statUncertaintyFilePath_EMCal_Contributors, EMCal_Systematic_data1); //read EMCal Scale Syst Contributors
    plotting_EMCal_Syst_Contributors(EMCal_Systematic_data1); //plot the above
    
    plot_EMCal_Scale_v2_overlays(defaultData, data_SYST1CEMC, data_SYST2CEMC, data_SYST3DCEMC, data_SYST3UCEMC, data_SYST4CEMC);
    plot_SignalWindowVariation(defaultData, data_Signal_Bound_Variation);
    plot_BackgroundWindowVariation(defaultData);
    
    Data HijingData, flowAfterBurner;
    std::string HijingDataPath = "/Users/patsfan753/Desktop/p015/MonteCarlo/Integrated_OverPT/p015/vN-CSV-files/vn-anaType-1-hijing-0-20fm.csv";
    std::string FlowAfterBurnerPath = "/Users/patsfan753/Desktop/p015/MonteCarlo/Integrated_OverPT/p015/vN-CSV-files/FlowAfterBurner.csv";
    Read_DataSet(HijingDataPath, HijingData);
    Read_DataSet(FlowAfterBurnerPath, flowAfterBurner);
    plotHijingOverlay(defaultData, HijingData, flowAfterBurner);
}
