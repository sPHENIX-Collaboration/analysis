#include <TFile.h>
#include <TH1F.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

struct Data {
    std::vector<double> corrected_v2_0_20, corrected_v2_20_40, corrected_v2_40_60;
    std::vector<double> corrected_v2_0_20_BgWindow_toPoint4, corrected_v2_20_40_BgWindow_toPoint4, corrected_v2_40_60_BgWindow_toPoint4;
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
        
        double v2_corrected_BgWindow_toPoint4 = std::stod(rowData.at(5));

        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            data.corrected_v2_40_60.push_back(v2_corrected);
            
            data.corrected_v2_40_60_BgWindow_toPoint4.push_back(v2_corrected_BgWindow_toPoint4);

        } else if (index >= 6 && index <= 11) {
            data.corrected_v2_20_40.push_back(v2_corrected);
        
            data.corrected_v2_20_40_BgWindow_toPoint4.push_back(v2_corrected_BgWindow_toPoint4);

        } else if (index >= 12 && index <= 17) {
            data.corrected_v2_0_20.push_back(v2_corrected);
            
            data.corrected_v2_0_20_BgWindow_toPoint4.push_back(v2_corrected_BgWindow_toPoint4);
        }
    }
    file.close();
}

void WriteComparisonToCSV(const std::vector<Data>& emCalDataSets, const Data& signalWindowData, const Data& referenceData, const std::string& outputPath) {
    std::ofstream outFile(outputPath);
    outFile << "Index,Reference_v2,Quad_Sum_EMCal_Syst,SignalWindow_Syst,BackgroundWindow_Syst,WeightedQuadratureSum,unWeightedQuadratureSum\n";

    for (size_t index = 0; index < 18; ++index) {
        const auto& referenceVector = index < 6 ? referenceData.corrected_v2_40_60 :
                                    index < 12 ? referenceData.corrected_v2_20_40 :
                                    referenceData.corrected_v2_0_20;
                                    
        size_t binIndex = index % 6;
        double reference_v2 = referenceVector[binIndex];
        
        double quadratureSum = 0.0;
        
        // Calculate quadrature sum for EMCal systematics datasets
        for (size_t dataSetIndex = 1; dataSetIndex < emCalDataSets.size(); ++dataSetIndex) {
            const auto& v2_vector = index < 6 ? emCalDataSets[dataSetIndex].corrected_v2_40_60 :
                                  index < 12 ? emCalDataSets[dataSetIndex].corrected_v2_20_40 :
                                  emCalDataSets[dataSetIndex].corrected_v2_0_20;
            double v2 = v2_vector[binIndex];
            double relativeDifference = std::abs(v2 - reference_v2) / reference_v2;
            quadratureSum += std::pow(relativeDifference, 2);
        }
        double finalQuadratureSum = std::sqrt(quadratureSum);

        // Calculate relative difference for the signal window dataset
        double signalWindow_v2 = 0.0, signalWindowRelativeDifference = 0.0;
        if (signalWindowData.corrected_v2_0_20.size() > binIndex) { // Ensure there's data to compare
            const auto& signalWindowVector = index < 6 ? signalWindowData.corrected_v2_40_60 :
                                          index < 12 ? signalWindowData.corrected_v2_20_40 :
                                          signalWindowData.corrected_v2_0_20;
            signalWindow_v2 = signalWindowVector[binIndex];
            signalWindowRelativeDifference = std::abs(signalWindow_v2 - reference_v2) / reference_v2;
        }

        // Calculate relative difference for Type4 within the same dataset
        double type4_v2 = 0.0, type4RelativeDifference = 0.0;
        if (referenceData.corrected_v2_0_20_BgWindow_toPoint4.size() > binIndex) {
            const auto& type4Vector = index < 6 ? referenceData.corrected_v2_40_60_BgWindow_toPoint4 :
                                  index < 12 ? referenceData.corrected_v2_20_40_BgWindow_toPoint4 :
                                  referenceData.corrected_v2_0_20_BgWindow_toPoint4;
            type4_v2 = type4Vector[binIndex];
            type4RelativeDifference = std::abs(type4_v2 - reference_v2) / reference_v2;
        }

        // Calculate the weighted quadrature sum
        double weightedQuadratureSum = std::sqrt(
            std::pow(finalQuadratureSum * reference_v2, 2) +
            std::pow(signalWindowRelativeDifference * reference_v2, 2) +
            std::pow(type4RelativeDifference * reference_v2, 2)
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


void CalculateSystematics() {
    std::string filePath1_ReferenceDataSet = "/Users/patsfan753/Desktop/Default_Final_v2_energyCutsFinal.csv";

    // EMCal Systematics datasets
    std::string baseDataPath_EmCal_Systematics = "/Users/patsfan753/Desktop/Systematics_Analysis/EMCal_systematics/";
    std::string filePathEMCal_Syst_SYST1CEMC = baseDataPath_EmCal_Systematics + "vn-SYST1CEMC.csv";
    std::string filePathEMCal_Syst_SYST2CEMC = baseDataPath_EmCal_Systematics + "vn-SYST2CEMC.csv";
    std::string filePathEMCal_Syst_SYST3DCEMC = baseDataPath_EmCal_Systematics + "vn-SYST3DCEMC.csv";
    std::string filePathEMCal_Syst_SYST3UCEMC = baseDataPath_EmCal_Systematics + "vn-SYST3UCEMC.csv";
    std::string filePathEMCal_Syst_SYST4CEMC = baseDataPath_EmCal_Systematics + "vn-SYST4CEMC.csv";

    // Signal Window dataset
    std::string baseDataPath_SignalWindow_Systematics = "/Users/patsfan753/Desktop/Systematics_Analysis/SignalWindow_Variations/";
    std::string filePath_SignalWindow = baseDataPath_SignalWindow_Systematics + "vn-sigma-1.5.csv";

    Data data1_Reference, data_SignalWindow, data_EMCal_Syst_1CEMC, data_EMCal_Syst_2CEMC, data_EMCal_Syst_3DCEMC, data_EMCal_Syst_3UCEMC, data_EMCal_Syst_4CEMC;

    // Read the datasets
    Read_DataSet(filePath1_ReferenceDataSet, data1_Reference);
    Read_DataSet(filePathEMCal_Syst_SYST1CEMC, data_EMCal_Syst_1CEMC);
    Read_DataSet(filePathEMCal_Syst_SYST2CEMC, data_EMCal_Syst_2CEMC);
    Read_DataSet(filePathEMCal_Syst_SYST3DCEMC, data_EMCal_Syst_3DCEMC);
    Read_DataSet(filePathEMCal_Syst_SYST3UCEMC, data_EMCal_Syst_3UCEMC);
    Read_DataSet(filePathEMCal_Syst_SYST4CEMC, data_EMCal_Syst_4CEMC);
    Read_DataSet(filePath_SignalWindow, data_SignalWindow);

    std::vector<Data> emCalDataSets = {data1_Reference, data_EMCal_Syst_1CEMC, data_EMCal_Syst_2CEMC, data_EMCal_Syst_3DCEMC, data_EMCal_Syst_3UCEMC, data_EMCal_Syst_4CEMC};

    std::string csvOutputPath = "/Users/patsfan753/Desktop/Systematics_Analysis/StatUncertaintyTable.csv";

    // Call the modified WriteComparisonToCSV function
    WriteComparisonToCSV(emCalDataSets, data_SignalWindow, data1_Reference, csvOutputPath);
}

