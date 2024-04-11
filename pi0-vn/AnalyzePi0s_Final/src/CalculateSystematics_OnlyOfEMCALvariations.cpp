#include <TFile.h>
#include <TH1F.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

struct Data {
    std::vector<double> corrected_v2_0_20, corrected_v2_20_40, corrected_v2_40_60;
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
        } else if (index >= 6 && index <= 11) {
            data.corrected_v2_20_40.push_back(v2_corrected);
        } else if (index >= 12 && index <= 17) {
            data.corrected_v2_0_20.push_back(v2_corrected);
        }
    }
    file.close();
}

void WriteComparisonToCSV(const std::vector<Data>& emCalDataSets, const Data& referenceData, const std::string& outputPath) {
    std::ofstream outFile(outputPath);
    
    // Updated header to include new columns
    outFile << "Index,Reference_v2";
    std::vector<std::string> dataSetLabels = {"SYST1CEMC", "SYST2CEMC", "SYST3DCEMC", "SYST3UCEMC", "SYST4CEMC"};
    for (const auto& label : dataSetLabels) {
        outFile << ",RelativeUncertainty_" << label << ",AbsoluteUncertainty_" << label;
    }
    // New columns for quadrature sum
    outFile << ",QuadratureSumRelativeUncertainties,QuadratureSumAbsoluteUncertainties\n";

    for (size_t index = 0; index < 18; ++index) {
        double sumSquaredRelative = 0.0;
        double sumSquaredAbsolute = 0.0;

        const auto& referenceVector = index < 6 ? referenceData.corrected_v2_40_60 :
                                    index < 12 ? referenceData.corrected_v2_20_40 :
                                    referenceData.corrected_v2_0_20;
        size_t binIndex = index % 6;
        if (binIndex < referenceVector.size()) {
            double reference_v2 = referenceVector[binIndex];
            outFile << index << "," << reference_v2;

            for (size_t dataSetIndex = 1; dataSetIndex < emCalDataSets.size(); ++dataSetIndex) {
                const auto& currentDataSet = emCalDataSets[dataSetIndex];
                const auto& v2_vector = index < 6 ? currentDataSet.corrected_v2_40_60 :
                                        index < 12 ? currentDataSet.corrected_v2_20_40 :
                                        currentDataSet.corrected_v2_0_20;

                if (binIndex < v2_vector.size()) {
                    double v2 = v2_vector[binIndex];
                    double relativeUncertainty = std::abs(v2 - reference_v2) / reference_v2;
                    double absoluteUncertainty = std::abs(v2 - reference_v2);

                    // Squaring the uncertainties for quadrature sum
                    sumSquaredRelative += pow(relativeUncertainty, 2);
                    sumSquaredAbsolute += pow(absoluteUncertainty, 2);

                    outFile << "," << relativeUncertainty << "," << absoluteUncertainty;
                } else {
                    outFile << ",,"; // In case there's no data for this dataset at this bin index
                }
            }

            // Writing the quadrature sums to the CSV
            outFile << "," << sqrt(sumSquaredRelative) << "," << sqrt(sumSquaredAbsolute);
            outFile << "\n";
        }
    }
    outFile.close();
}


void CalculateSystematics_OnlyOfEMCALvariations() {
    std::string filePath1_ReferenceDataSet = "/Users/patsfan753/Desktop/Default_Final_v2_energyCutsFinal.csv";

    // EMCal Systematics datasets
    std::string baseDataPath_EmCal_Systematics = "/Users/patsfan753/Desktop/Systematics_Analysis/EMCal_systematics/";
    std::string filePathEMCal_Syst_SYST1CEMC = baseDataPath_EmCal_Systematics + "vn-SYST1CEMC.csv";
    std::string filePathEMCal_Syst_SYST2CEMC = baseDataPath_EmCal_Systematics + "vn-SYST2CEMC.csv";
    std::string filePathEMCal_Syst_SYST3DCEMC = baseDataPath_EmCal_Systematics + "vn-SYST3DCEMC.csv";
    std::string filePathEMCal_Syst_SYST3UCEMC = baseDataPath_EmCal_Systematics + "vn-SYST3UCEMC.csv";
    std::string filePathEMCal_Syst_SYST4CEMC = baseDataPath_EmCal_Systematics + "vn-SYST4CEMC.csv";

    Data data1_Reference, data_EMCal_Syst_1CEMC, data_EMCal_Syst_2CEMC, data_EMCal_Syst_3DCEMC, data_EMCal_Syst_3UCEMC, data_EMCal_Syst_4CEMC;

    // Read the datasets
    Read_DataSet(filePath1_ReferenceDataSet, data1_Reference);
    Read_DataSet(filePathEMCal_Syst_SYST1CEMC, data_EMCal_Syst_1CEMC);
    Read_DataSet(filePathEMCal_Syst_SYST2CEMC, data_EMCal_Syst_2CEMC);
    Read_DataSet(filePathEMCal_Syst_SYST3DCEMC, data_EMCal_Syst_3DCEMC);
    Read_DataSet(filePathEMCal_Syst_SYST3UCEMC, data_EMCal_Syst_3UCEMC);
    Read_DataSet(filePathEMCal_Syst_SYST4CEMC, data_EMCal_Syst_4CEMC);

    std::vector<Data> emCalDataSets = {data1_Reference, data_EMCal_Syst_1CEMC, data_EMCal_Syst_2CEMC, data_EMCal_Syst_3DCEMC, data_EMCal_Syst_3UCEMC, data_EMCal_Syst_4CEMC};

    std::string csvOutputPath = "/Users/patsfan753/Desktop/Systematics_Analysis/StatUncertaintyTable_EMCalVariationsOnly.csv";

    // Call the modified WriteComparisonToCSV function
    WriteComparisonToCSV(emCalDataSets, data1_Reference, csvOutputPath);
}
