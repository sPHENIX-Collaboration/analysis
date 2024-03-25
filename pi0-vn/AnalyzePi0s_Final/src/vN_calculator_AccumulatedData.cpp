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
bool Plot_vN_bool = true; // Set this to true or false for vN plotting--IF TRUE, then vN will NOT BE re appended/calculated in the CSV being used
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
struct AdditionalData {
    std::vector<double> corrected_v2_0_20_Additional, corrected_v2_20_40_Additional, corrected_v2_40_60_Additional;
    std::vector<double> corrected_v2_0_20_Errors_Additional, corrected_v2_20_40_Errors_Additional, corrected_v2_40_60_Errors_Additional;
    
    std::vector<double> corrected_v3_0_20_Additional, corrected_v3_40_60_Additional;
    std::vector<double> corrected_v3_0_20_Errors_Additional, corrected_v3_40_60_Errors_Additional;
};
void Read_Additonal_DataSet(const std::string& filePath, AdditionalData& data) {
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
        double v2_corrected = std::stod(rowData.at(rowData.size() - 4));
        double v2_error_corrected = std::stod(rowData.at(rowData.size() - 3));
        double v3_corrected = std::stod(rowData.at(rowData.size() - 2));
        double v3_error_corrected = std::stod(rowData.back());
        
        /*
         BELOW CODE IS IF READING in PlotByPlotOutput format:
         */
//        double v2_corrected = std::stod(rowData.at(rowData.size() - 10));
//        double v2_error_corrected = std::stod(rowData.at(rowData.size() - 9));
//        double v3_corrected = std::stod(rowData.at(rowData.size() - 2));
//        double v3_error_corrected = std::stod(rowData.back());

        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            data.corrected_v2_40_60_Additional.push_back(v2_corrected);
            data.corrected_v2_40_60_Errors_Additional.push_back(v2_error_corrected);
            
            data.corrected_v3_40_60_Additional.push_back(v3_corrected);
            data.corrected_v3_40_60_Errors_Additional.push_back(v3_error_corrected);
            
        } else if (index >= 6 && index <= 11) {
            data.corrected_v2_20_40_Additional.push_back(v2_corrected);
            data.corrected_v2_20_40_Errors_Additional.push_back(v2_error_corrected);

        } else if (index >= 12 && index <= 17) {
            data.corrected_v2_0_20_Additional.push_back(v2_corrected);
            data.corrected_v2_0_20_Errors_Additional.push_back(v2_error_corrected);

            data.corrected_v3_0_20_Additional.push_back(v3_corrected);
            data.corrected_v3_0_20_Errors_Additional.push_back(v3_error_corrected);
        }
    }
    file.close();
}


struct AdditionalData_2 {
    std::vector<double> corrected_v2_0_20_Additional_2, corrected_v2_20_40_Additional_2, corrected_v2_40_60_Additional_2;
    std::vector<double> corrected_v2_0_20_Errors_Additional_2, corrected_v2_20_40_Errors_Additional_2, corrected_v2_40_60_Errors_Additional_2;
    
    std::vector<double> corrected_v3_0_20_Additional_2, corrected_v3_40_60_Additional_2;
    std::vector<double> corrected_v3_0_20_Errors_Additional_2, corrected_v3_40_60_Errors_Additional_2;
};
void Read_Additonal_DataSet_2(const std::string& filePath, AdditionalData_2& data_2) {
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
        double v2_corrected = std::stod(rowData.at(rowData.size() - 4));
        double v2_error_corrected = std::stod(rowData.at(rowData.size() - 3));
        double v3_corrected = std::stod(rowData.at(rowData.size() - 2));
        double v3_error_corrected = std::stod(rowData.back());
        
        /*
         BELOW CODE IS IF READING in PlotByPlotOutput format:
         */
//        double v2_corrected = std::stod(rowData.at(rowData.size() - 10));
//        double v2_error_corrected = std::stod(rowData.at(rowData.size() - 9));
//        double v3_corrected = std::stod(rowData.at(rowData.size() - 2));
//        double v3_error_corrected = std::stod(rowData.back());

        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            data_2.corrected_v2_40_60_Additional_2.push_back(v2_corrected);
            data_2.corrected_v2_40_60_Errors_Additional_2.push_back(v2_error_corrected);
            
            data_2.corrected_v3_40_60_Additional_2.push_back(v3_corrected);
            data_2.corrected_v3_40_60_Errors_Additional_2.push_back(v3_error_corrected);
            
        } else if (index >= 6 && index <= 11) {
            data_2.corrected_v2_20_40_Additional_2.push_back(v2_corrected);
            data_2.corrected_v2_20_40_Errors_Additional_2.push_back(v2_error_corrected);

        } else if (index >= 12 && index <= 17) {
            data_2.corrected_v2_0_20_Additional_2.push_back(v2_corrected);
            data_2.corrected_v2_0_20_Errors_Additional_2.push_back(v2_error_corrected);

            data_2.corrected_v3_0_20_Additional_2.push_back(v3_corrected);
            data_2.corrected_v3_0_20_Errors_Additional_2.push_back(v3_error_corrected);
        }
    }
    file.close();
}

struct AdditionalData_3 {
    std::vector<double> corrected_v2_0_20_Additional_3, corrected_v2_20_40_Additional_3, corrected_v2_40_60_Additional_3;
    std::vector<double> corrected_v2_0_20_Errors_Additional_3, corrected_v2_20_40_Errors_Additional_3, corrected_v2_40_60_Errors_Additional_3;
    
    std::vector<double> corrected_v3_0_20_Additional_3, corrected_v3_40_60_Additional_3;
    std::vector<double> corrected_v3_0_20_Errors_Additional_3, corrected_v3_40_60_Errors_Additional_3;
};

void Read_Additonal_DataSet_3(const std::string& filePath, AdditionalData_3& data_3) {
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
        double v2_corrected = std::stod(rowData.at(rowData.size() - 4));
        double v2_error_corrected = std::stod(rowData.at(rowData.size() - 3));
        double v3_corrected = std::stod(rowData.at(rowData.size() - 2));
        double v3_error_corrected = std::stod(rowData.back());
        
        /*
         BELOW CODE IS IF READING in PlotByPlotOutput format:
         */
//        double v2_corrected = std::stod(rowData.at(rowData.size() - 10));
//        double v2_error_corrected = std::stod(rowData.at(rowData.size() - 9));
//        double v3_corrected = std::stod(rowData.at(rowData.size() - 2));
//        double v3_error_corrected = std::stod(rowData.back());

        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            data_3.corrected_v2_40_60_Additional_3.push_back(v2_corrected);
            data_3.corrected_v2_40_60_Errors_Additional_3.push_back(v2_error_corrected);
            
            data_3.corrected_v3_40_60_Additional_3.push_back(v3_corrected);
            data_3.corrected_v3_40_60_Errors_Additional_3.push_back(v3_error_corrected);
            
        } else if (index >= 6 && index <= 11) {
            data_3.corrected_v2_20_40_Additional_3.push_back(v2_corrected);
            data_3.corrected_v2_20_40_Errors_Additional_3.push_back(v2_error_corrected);

        } else if (index >= 12 && index <= 17) {
            data_3.corrected_v2_0_20_Additional_3.push_back(v2_corrected);
            data_3.corrected_v2_0_20_Errors_Additional_3.push_back(v2_error_corrected);

            data_3.corrected_v3_0_20_Additional_3.push_back(v3_corrected);
            data_3.corrected_v3_0_20_Errors_Additional_3.push_back(v3_error_corrected);
        }
    }
    file.close();
}

struct AdditionalData_4 {
    std::vector<double> corrected_v2_0_20_Additional_4, corrected_v2_20_40_Additional_4, corrected_v2_40_60_Additional_4;
    std::vector<double> corrected_v2_0_20_Errors_Additional_4, corrected_v2_20_40_Errors_Additional_4, corrected_v2_40_60_Errors_Additional_4;
    
    std::vector<double> corrected_v3_0_20_Additional_4, corrected_v3_40_60_Additional_4;
    std::vector<double> corrected_v3_0_20_Errors_Additional_4, corrected_v3_40_60_Errors_Additional_4;
};

void Read_Additonal_DataSet_4(const std::string& filePath, AdditionalData_4& data_4) {
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
        double v2_corrected = std::stod(rowData.at(rowData.size() - 4));
        double v2_error_corrected = std::stod(rowData.at(rowData.size() - 3));
        double v3_corrected = std::stod(rowData.at(rowData.size() - 2));
        double v3_error_corrected = std::stod(rowData.back());
        
        /*
         BELOW CODE IS IF READING in PlotByPlotOutput format:
         */
//        double v2_corrected = std::stod(rowData.at(rowData.size() - 10));
//        double v2_error_corrected = std::stod(rowData.at(rowData.size() - 9));
//        double v3_corrected = std::stod(rowData.at(rowData.size() - 2));
//        double v3_error_corrected = std::stod(rowData.back());

        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            data_4.corrected_v2_40_60_Additional_4.push_back(v2_corrected);
            data_4.corrected_v2_40_60_Errors_Additional_4.push_back(v2_error_corrected);
            
            data_4.corrected_v3_40_60_Additional_4.push_back(v3_corrected);
            data_4.corrected_v3_40_60_Errors_Additional_4.push_back(v3_error_corrected);
            
        } else if (index >= 6 && index <= 11) {
            data_4.corrected_v2_20_40_Additional_4.push_back(v2_corrected);
            data_4.corrected_v2_20_40_Errors_Additional_4.push_back(v2_error_corrected);

        } else if (index >= 12 && index <= 17) {
            data_4.corrected_v2_0_20_Additional_4.push_back(v2_corrected);
            data_4.corrected_v2_0_20_Errors_Additional_4.push_back(v2_error_corrected);

            data_4.corrected_v3_0_20_Additional_4.push_back(v3_corrected);
            data_4.corrected_v3_0_20_Errors_Additional_4.push_back(v3_error_corrected);
        }
    }
    file.close();
}

struct AdditionalData_5 {
    std::vector<double> corrected_v2_0_20_Additional_5, corrected_v2_20_40_Additional_5, corrected_v2_40_60_Additional_5;
    std::vector<double> corrected_v2_0_20_Errors_Additional_5, corrected_v2_20_40_Errors_Additional_5, corrected_v2_40_60_Errors_Additional_5;
    
    std::vector<double> corrected_v3_0_20_Additional_5, corrected_v3_40_60_Additional_5;
    std::vector<double> corrected_v3_0_20_Errors_Additional_5, corrected_v3_40_60_Errors_Additional_5;
};

void Read_Additonal_DataSet_5(const std::string& filePath, AdditionalData_5& data_5) {
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
        double v2_corrected = std::stod(rowData.at(rowData.size() - 4));
        double v2_error_corrected = std::stod(rowData.at(rowData.size() - 3));
        double v3_corrected = std::stod(rowData.at(rowData.size() - 2));
        double v3_error_corrected = std::stod(rowData.back());
        
        /*
         BELOW CODE IS IF READING in PlotByPlotOutput format:
         */
//        double v2_corrected = std::stod(rowData.at(rowData.size() - 10));
//        double v2_error_corrected = std::stod(rowData.at(rowData.size() - 9));
//        double v3_corrected = std::stod(rowData.at(rowData.size() - 2));
//        double v3_error_corrected = std::stod(rowData.back());

        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            data_5.corrected_v2_40_60_Additional_5.push_back(v2_corrected);
            data_5.corrected_v2_40_60_Errors_Additional_5.push_back(v2_error_corrected);
            
            data_5.corrected_v3_40_60_Additional_5.push_back(v3_corrected);
            data_5.corrected_v3_40_60_Errors_Additional_5.push_back(v3_error_corrected);
            
        } else if (index >= 6 && index <= 11) {
            data_5.corrected_v2_20_40_Additional_5.push_back(v2_corrected);
            data_5.corrected_v2_20_40_Errors_Additional_5.push_back(v2_error_corrected);

        } else if (index >= 12 && index <= 17) {
            data_5.corrected_v2_0_20_Additional_5.push_back(v2_corrected);
            data_5.corrected_v2_0_20_Errors_Additional_5.push_back(v2_error_corrected);

            data_5.corrected_v3_0_20_Additional_5.push_back(v3_corrected);
            data_5.corrected_v3_0_20_Errors_Additional_5.push_back(v3_error_corrected);
        }
    }
    file.close();
}



struct AdditionalData_6 {
    std::vector<double> corrected_v2_0_20_Additional_6, corrected_v2_20_40_Additional_6, corrected_v2_40_60_Additional_6;
    std::vector<double> corrected_v2_0_20_Errors_Additional_6, corrected_v2_20_40_Errors_Additional_6, corrected_v2_40_60_Errors_Additional_6;
    
    std::vector<double> corrected_v3_0_20_Additional_6, corrected_v3_40_60_Additional_6;
    std::vector<double> corrected_v3_0_20_Errors_Additional_6, corrected_v3_40_60_Errors_Additional_6;
};

void Read_Additonal_DataSet_6(const std::string& filePath, AdditionalData_6& data_6) {
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
        double v2_corrected = std::stod(rowData.at(rowData.size() - 4));
        double v2_error_corrected = std::stod(rowData.at(rowData.size() - 3));
        double v3_corrected = std::stod(rowData.at(rowData.size() - 2));
        double v3_error_corrected = std::stod(rowData.back());
        
        /*
         BELOW CODE IS IF READING in PlotByPlotOutput format:
         */
//        double v2_corrected = std::stod(rowData.at(rowData.size() - 10));
//        double v2_error_corrected = std::stod(rowData.at(rowData.size() - 9));
//        double v3_corrected = std::stod(rowData.at(rowData.size() - 2));
//        double v3_error_corrected = std::stod(rowData.back());

        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            data_6.corrected_v2_40_60_Additional_6.push_back(v2_corrected);
            data_6.corrected_v2_40_60_Errors_Additional_6.push_back(v2_error_corrected);
            
            data_6.corrected_v3_40_60_Additional_6.push_back(v3_corrected);
            data_6.corrected_v3_40_60_Errors_Additional_6.push_back(v3_error_corrected);
            
        } else if (index >= 6 && index <= 11) {
            data_6.corrected_v2_20_40_Additional_6.push_back(v2_corrected);
            data_6.corrected_v2_20_40_Errors_Additional_6.push_back(v2_error_corrected);

        } else if (index >= 12 && index <= 17) {
            data_6.corrected_v2_0_20_Additional_6.push_back(v2_corrected);
            data_6.corrected_v2_0_20_Errors_Additional_6.push_back(v2_error_corrected);

            data_6.corrected_v3_0_20_Additional_6.push_back(v3_corrected);
            data_6.corrected_v3_0_20_Errors_Additional_6.push_back(v3_error_corrected);
        }
    }
    file.close();
}


struct AdditionalData_7 {
    std::vector<double> corrected_v2_0_20_Additional_7, corrected_v2_20_40_Additional_7, corrected_v2_40_60_Additional_7;
    std::vector<double> corrected_v2_0_20_Errors_Additional_7, corrected_v2_20_40_Errors_Additional_7, corrected_v2_40_60_Errors_Additional_7;
    
    std::vector<double> corrected_v3_0_20_Additional_7, corrected_v3_40_60_Additional_7;
    std::vector<double> corrected_v3_0_20_Errors_Additional_7, corrected_v3_40_60_Errors_Additional_7;
};

void Read_Additonal_DataSet_7(const std::string& filePath, AdditionalData_7& data_7) {
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
        double v2_corrected = std::stod(rowData.at(rowData.size() - 4));
        double v2_error_corrected = std::stod(rowData.at(rowData.size() - 3));
        double v3_corrected = std::stod(rowData.at(rowData.size() - 2));
        double v3_error_corrected = std::stod(rowData.back());
        
        /*
         BELOW CODE IS IF READING in PlotByPlotOutput format:
         */
//        double v2_corrected = std::stod(rowData.at(rowData.size() - 10));
//        double v2_error_corrected = std::stod(rowData.at(rowData.size() - 9));
//        double v3_corrected = std::stod(rowData.at(rowData.size() - 2));
//        double v3_error_corrected = std::stod(rowData.back());

        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            data_7.corrected_v2_40_60_Additional_7.push_back(v2_corrected);
            data_7.corrected_v2_40_60_Errors_Additional_7.push_back(v2_error_corrected);
            
            data_7.corrected_v3_40_60_Additional_7.push_back(v3_corrected);
            data_7.corrected_v3_40_60_Errors_Additional_7.push_back(v3_error_corrected);
            
        } else if (index >= 6 && index <= 11) {
            data_7.corrected_v2_20_40_Additional_7.push_back(v2_corrected);
            data_7.corrected_v2_20_40_Errors_Additional_7.push_back(v2_error_corrected);

        } else if (index >= 12 && index <= 17) {
            data_7.corrected_v2_0_20_Additional_7.push_back(v2_corrected);
            data_7.corrected_v2_0_20_Errors_Additional_7.push_back(v2_error_corrected);

            data_7.corrected_v3_0_20_Additional_7.push_back(v3_corrected);
            data_7.corrected_v3_0_20_Errors_Additional_7.push_back(v3_error_corrected);
        }
    }
    file.close();
}



// Call this function after vectors populated
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
void Plot_vN(const AdditionalData& data, const AdditionalData_2& data_2, const AdditionalData_3& data_3, const AdditionalData_4& data_4, const AdditionalData_5& data_5, const AdditionalData_6& data_6, const AdditionalData_7& data_7) {
    /*
     Path to non 'Additional' datasets, top data set in legend plotted
     */
    std::string filePath = "/Users/patsfan753/Desktop/combined_lowest_v2_errors.csv";
    
    /*
     Path to CSV data cleaned to be read in by method above
     */
    std::string phenixFilePath = "/Users/patsfan753/Desktop/vN_AnalysisFinal/data/PHENIX_Data_Overlayed/FinalCleanedPhenix.csv";
    
    std::vector<double> ptCenters = {2.25, 2.75, 3.25, 3.75, 4.25, 4.75}; // Mid-points of pT ranges
    
    std::vector<double> corrected_v2_0_20, corrected_v2_0_20_Errors, corrected_v2_20_40, corrected_v2_20_40_Errors, corrected_v2_40_60, corrected_v2_40_60_Errors, corrected_v3_0_20, corrected_v3_0_20_Errors, corrected_v3_20_40, corrected_v3_20_40_Errors, corrected_v3_40_60, corrected_v3_40_60_Errors;

    double v2_corrected, v2_error_corrected;
    double v3_corrected, v3_error_corrected;
    
    std::string BasePlotOutputPath = "/Users/patsfan753/Desktop/vN_AnalysisFinal/plotOutput/vN_Plot_Output";
    
    //vectors for PHENIX data
    std::vector<double> v2_0_10, v2_0_10_Errors, v2_0_10_Errors_Negative, v2_10_20, v2_10_20_Errors, v2_10_20_Errors_Negative, v2_20_30, v2_20_30_Errors, v2_20_30_Errors_Negative, v2_30_40, v2_30_40_Errors, v2_30_40_Errors_Negative, v2_40_50, v2_40_50_Errors, v2_40_50_Errors_Negative, v2_50_60, v2_50_60_Errors, v2_50_60_Errors_Negative;

    std::ifstream file(filePath);
    std::string line;
    // Skip the header line
    std::getline(file, line);
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
        
        /*
         BELOW CODE IS IF READING in PlotByPlotOutput format:
         */
//        double v2_corrected = std::stod(rowData.at(rowData.size() - 10));
//        double v2_error_corrected = std::stod(rowData.at(rowData.size() - 9));
//        double v3_corrected = std::stod(rowData.at(rowData.size() - 2));
//        double v3_error_corrected = std::stod(rowData.back());

        // Mapping index to centrality and pT
        int index = std::stoi(rowData[0]);
        if (index >= 0 && index <= 5) {
            // 40-60% centrality
            corrected_v2_40_60.push_back(v2_corrected);
            corrected_v2_40_60_Errors.push_back(v2_error_corrected);

            corrected_v3_40_60.push_back(v3_corrected);
            corrected_v3_40_60_Errors.push_back(v3_error_corrected);
        } else if (index >= 6 && index <= 11) {
            // 20-40% centrality
            corrected_v2_20_40.push_back(v2_corrected);
            corrected_v2_20_40_Errors.push_back(v2_error_corrected);

            corrected_v3_20_40.push_back(v3_corrected);
            corrected_v3_20_40_Errors.push_back(v3_error_corrected);
        } else if (index >= 12 && index <= 17) {
            // 0-20% centrality
            corrected_v2_0_20.push_back(v2_corrected);
            corrected_v2_0_20_Errors.push_back(v2_error_corrected);
            
            corrected_v3_0_20.push_back(v3_corrected);
            corrected_v3_0_20_Errors.push_back(v3_error_corrected);
        }
    }
    file.close();
    
    // Define the offsets for jitter in Overlays
    double offset = 0.03;
    
    // Define the marker styles and colors for clarity
    int markerStyle_sPHENIX = 20;
    
    //kOrange + 1 used in fit variations
    int color_sPHENIX = kRed;
    
    TGraphErrors* corrected_graph_0_20_v2 = CreateGraph(ptCenters, corrected_v2_0_20, corrected_v2_0_20_Errors);
    corrected_graph_0_20_v2->SetMarkerColor(color_sPHENIX);
    corrected_graph_0_20_v2->SetLineColor(color_sPHENIX);
    //corrected_graph_0_20_v2->SetMarkerSize(0.75);
    corrected_graph_0_20_v2->SetMarkerStyle(markerStyle_sPHENIX);
    
    TGraphErrors* corrected_graph_0_20_v3 = CreateGraph(ptCenters, corrected_v3_0_20, corrected_v3_0_20_Errors);
    corrected_graph_0_20_v3->SetMarkerColor(color_sPHENIX);
    corrected_graph_0_20_v3->SetLineColor(color_sPHENIX);
    corrected_graph_0_20_v3->SetMarkerStyle(markerStyle_sPHENIX);

    TGraphErrors* corrected_graph_20_40_v2 = CreateGraph(ptCenters, corrected_v2_20_40, corrected_v2_20_40_Errors);
    corrected_graph_20_40_v2->SetMarkerColor(color_sPHENIX);
    corrected_graph_20_40_v2->SetLineColor(color_sPHENIX);
    //corrected_graph_20_40_v2->SetMarkerSize(0.75);
    corrected_graph_20_40_v2->SetMarkerStyle(markerStyle_sPHENIX);
    
    TGraphErrors* corrected_graph_20_40_v3 = CreateGraph(ptCenters, corrected_v3_20_40, corrected_v3_20_40_Errors);
    corrected_graph_20_40_v3->SetMarkerColor(color_sPHENIX);
    corrected_graph_20_40_v3->SetLineColor(color_sPHENIX);
    corrected_graph_20_40_v3->SetMarkerStyle(markerStyle_sPHENIX);

    TGraphErrors* corrected_graph_40_60_v2 = CreateGraph(ptCenters, corrected_v2_40_60, corrected_v2_40_60_Errors);
    corrected_graph_40_60_v2->SetMarkerColor(color_sPHENIX);
    corrected_graph_40_60_v2->SetLineColor(color_sPHENIX);
    //corrected_graph_40_60_v2->SetMarkerSize(0.75);
    corrected_graph_40_60_v2->SetMarkerStyle(markerStyle_sPHENIX);
    
    TGraphErrors* corrected_graph_40_60_v3 = CreateGraph(ptCenters, corrected_v3_40_60, corrected_v3_40_60_Errors);
    corrected_graph_40_60_v3->SetMarkerColor(color_sPHENIX);
    corrected_graph_40_60_v3->SetLineColor(color_sPHENIX);
    corrected_graph_40_60_v3->SetMarkerSize(0.8);
    corrected_graph_40_60_v3->SetMarkerStyle(markerStyle_sPHENIX);

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
     Additional Data Read in for Comparison
     */
    const auto& corrected_v2_0_20_Additional = data.corrected_v2_0_20_Additional;
    const auto& corrected_v2_20_40_Additional = data.corrected_v2_20_40_Additional;
    const auto& corrected_v2_40_60_Additional = data.corrected_v2_40_60_Additional;
    const auto& corrected_v2_0_20_Errors_Additional = data.corrected_v2_0_20_Errors_Additional;
    const auto& corrected_v2_20_40_Errors_Additional = data.corrected_v2_20_40_Errors_Additional;
    const auto& corrected_v2_40_60_Errors_Additional = data.corrected_v2_40_60_Errors_Additional;
    const auto& corrected_v3_0_20_Additional = data.corrected_v3_0_20_Additional;
    const auto& corrected_v3_40_60_Additional = data.corrected_v3_40_60_Additional;
    const auto& corrected_v3_0_20_Errors_Additional = data.corrected_v3_0_20_Errors_Additional;
    const auto& corrected_v3_40_60_Errors_Additional = data.corrected_v3_40_60_Errors_Additional;
    
    int markerStyle_sPHENIX_AdditionalData = 20;
    //color_sPHENIX_AdditionalData = 6; used in fit variations
//    int color_sPHENIX_AdditionalData = kGreen+3;
    int color_sPHENIX_AdditionalData = kRed;
    
    
    TGraphErrors* corrected_v2_0_20_Additional_graph = CreateGraph(ptCenters, corrected_v2_0_20_Additional, corrected_v2_0_20_Errors_Additional);
    corrected_v2_0_20_Additional_graph->SetMarkerColor(color_sPHENIX_AdditionalData);
    corrected_v2_0_20_Additional_graph->SetLineColor(color_sPHENIX_AdditionalData);
    //corrected_v2_0_20_Additional_graph->SetMarkerSize(0.75);
    corrected_v2_0_20_Additional_graph->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData);
    
    TGraphErrors* corrected_v2_20_40_Additional_graph = CreateGraph(ptCenters, corrected_v2_20_40_Additional, corrected_v2_20_40_Errors_Additional);
    corrected_v2_20_40_Additional_graph->SetMarkerColor(color_sPHENIX_AdditionalData);
    corrected_v2_20_40_Additional_graph->SetLineColor(color_sPHENIX_AdditionalData);
    //corrected_v2_20_40_Additional_graph->SetMarkerSize(0.75);
    corrected_v2_20_40_Additional_graph->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData);
    
    TGraphErrors* corrected_v2_40_60_Additional_graph = CreateGraph(ptCenters, corrected_v2_40_60_Additional, corrected_v2_40_60_Errors_Additional);
    corrected_v2_40_60_Additional_graph->SetMarkerColor(color_sPHENIX_AdditionalData);
    corrected_v2_40_60_Additional_graph->SetLineColor(color_sPHENIX_AdditionalData);
    //corrected_v2_40_60_Additional_graph->SetMarkerSize(0.75);
    corrected_v2_40_60_Additional_graph->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData);
    
    
    
    
    
    /*
     Additional Data 2 Read in for Comparison
     */
    const auto& corrected_v2_0_20_Additional_2 = data_2.corrected_v2_0_20_Additional_2;
    const auto& corrected_v2_20_40_Additional_2 = data_2.corrected_v2_20_40_Additional_2;
    const auto& corrected_v2_40_60_Additional_2 = data_2.corrected_v2_40_60_Additional_2;
    const auto& corrected_v2_0_20_Errors_Additional_2 = data_2.corrected_v2_0_20_Errors_Additional_2;
    const auto& corrected_v2_20_40_Errors_Additional_2 = data_2.corrected_v2_20_40_Errors_Additional_2;
    const auto& corrected_v2_40_60_Errors_Additional_2 = data_2.corrected_v2_40_60_Errors_Additional_2;
    const auto& corrected_v3_0_20_Additional_2 = data_2.corrected_v3_0_20_Additional_2;
    const auto& corrected_v3_40_60_Additional_2 = data_2.corrected_v3_40_60_Additional_2;
    const auto& corrected_v3_0_20_Errors_Additional_2 = data_2.corrected_v3_0_20_Errors_Additional_2;
    const auto& corrected_v3_40_60_Errors_Additional_2 = data_2.corrected_v3_40_60_Errors_Additional_2;
    
    int markerStyle_sPHENIX_AdditionalData_2 = 22;
    int color_sPHENIX_AdditionalData_2 = kViolet - 3;
    
    TGraphErrors* corrected_v2_0_20_Additional_graph_2 = CreateGraph(ptCenters, corrected_v2_0_20_Additional_2, corrected_v2_0_20_Errors_Additional_2);
    corrected_v2_0_20_Additional_graph_2->SetMarkerColor(color_sPHENIX_AdditionalData_2);
    corrected_v2_0_20_Additional_graph_2->SetLineColor(color_sPHENIX_AdditionalData_2);
    //corrected_v2_0_20_Additional_graph_2->SetMarkerSize(0.75);
    corrected_v2_0_20_Additional_graph_2->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_2);
    
    TGraphErrors* corrected_v2_20_40_Additional_graph_2 = CreateGraph(ptCenters, corrected_v2_20_40_Additional_2, corrected_v2_20_40_Errors_Additional_2);
    corrected_v2_20_40_Additional_graph_2->SetMarkerColor(color_sPHENIX_AdditionalData_2);
    corrected_v2_20_40_Additional_graph_2->SetLineColor(color_sPHENIX_AdditionalData_2);
    //corrected_v2_20_40_Additional_graph_2->SetMarkerSize(0.75);
    corrected_v2_20_40_Additional_graph_2->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_2);
    
    TGraphErrors* corrected_v2_40_60_Additional_graph_2 = CreateGraph(ptCenters, corrected_v2_40_60_Additional_2, corrected_v2_40_60_Errors_Additional_2);
    corrected_v2_40_60_Additional_graph_2->SetMarkerColor(color_sPHENIX_AdditionalData_2);
    corrected_v2_40_60_Additional_graph_2->SetLineColor(color_sPHENIX_AdditionalData_2);
    //corrected_v2_40_60_Additional_graph_2->SetMarkerSize(0.75);
    corrected_v2_40_60_Additional_graph_2->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_2);
    
    
    
    
    
    /*
     Additional Data 3 Read in for Comparison
     */
    const auto& corrected_v2_0_20_Additional_3 = data_3.corrected_v2_0_20_Additional_3;
    const auto& corrected_v2_20_40_Additional_3 = data_3.corrected_v2_20_40_Additional_3;
    const auto& corrected_v2_40_60_Additional_3 = data_3.corrected_v2_40_60_Additional_3;
    const auto& corrected_v2_0_20_Errors_Additional_3 = data_3.corrected_v2_0_20_Errors_Additional_3;
    const auto& corrected_v2_20_40_Errors_Additional_3 = data_3.corrected_v2_20_40_Errors_Additional_3;
    const auto& corrected_v2_40_60_Errors_Additional_3 = data_3.corrected_v2_40_60_Errors_Additional_3;
    const auto& corrected_v3_0_20_Additional_3 = data_3.corrected_v3_0_20_Additional_3;
    const auto& corrected_v3_40_60_Additional_3 = data_3.corrected_v3_40_60_Additional_3;
    const auto& corrected_v3_0_20_Errors_Additional_3 = data_3.corrected_v3_0_20_Errors_Additional_3;
    const auto& corrected_v3_40_60_Errors_Additional_3 = data_3.corrected_v3_40_60_Errors_Additional_3;
    
    int markerStyle_sPHENIX_AdditionalData_3 = 23;
    int color_sPHENIX_AdditionalData_3 = kRed;
    
    TGraphErrors* corrected_v2_0_20_Additional_graph_3 = CreateGraph(ptCenters, corrected_v2_0_20_Additional_3, corrected_v2_0_20_Errors_Additional_3);
    corrected_v2_0_20_Additional_graph_3->SetMarkerColor(color_sPHENIX_AdditionalData_3);
    corrected_v2_0_20_Additional_graph_3->SetLineColor(color_sPHENIX_AdditionalData_3);
    corrected_v2_0_20_Additional_graph_3->SetMarkerSize(0.75);
    corrected_v2_0_20_Additional_graph_3->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_3);
    
    TGraphErrors* corrected_v2_20_40_Additional_graph_3 = CreateGraph(ptCenters, corrected_v2_20_40_Additional_3, corrected_v2_20_40_Errors_Additional_3);
    corrected_v2_20_40_Additional_graph_3->SetMarkerColor(color_sPHENIX_AdditionalData_3);
    corrected_v2_20_40_Additional_graph_3->SetLineColor(color_sPHENIX_AdditionalData_3);
    corrected_v2_20_40_Additional_graph_3->SetMarkerSize(0.75);
    corrected_v2_20_40_Additional_graph_3->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_3);
    
    TGraphErrors* corrected_v2_40_60_Additional_graph_3 = CreateGraph(ptCenters, corrected_v2_40_60_Additional_3, corrected_v2_40_60_Errors_Additional_3);
    corrected_v2_40_60_Additional_graph_3->SetMarkerColor(color_sPHENIX_AdditionalData_3);
    corrected_v2_40_60_Additional_graph_3->SetLineColor(color_sPHENIX_AdditionalData_3);
    corrected_v2_40_60_Additional_graph_3->SetMarkerSize(0.75);
    corrected_v2_40_60_Additional_graph_3->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_3);
    
    
    
    /*
     Additional Data 4 Read in for Comparison
     */
    const auto& corrected_v2_0_20_Additional_4 = data_4.corrected_v2_0_20_Additional_4;
    const auto& corrected_v2_20_40_Additional_4 = data_4.corrected_v2_20_40_Additional_4;
    const auto& corrected_v2_40_60_Additional_4 = data_4.corrected_v2_40_60_Additional_4;
    const auto& corrected_v2_0_20_Errors_Additional_4 = data_4.corrected_v2_0_20_Errors_Additional_4;
    const auto& corrected_v2_20_40_Errors_Additional_4 = data_4.corrected_v2_20_40_Errors_Additional_4;
    const auto& corrected_v2_40_60_Errors_Additional_4 = data_4.corrected_v2_40_60_Errors_Additional_4;
    const auto& corrected_v3_0_20_Additional_4 = data_4.corrected_v3_0_20_Additional_4;
    const auto& corrected_v3_40_60_Additional_4 = data_4.corrected_v3_40_60_Additional_4;
    const auto& corrected_v3_0_20_Errors_Additional_4 = data_4.corrected_v3_0_20_Errors_Additional_4;
    const auto& corrected_v3_40_60_Errors_Additional_4 = data_4.corrected_v3_40_60_Errors_Additional_4;
    
    int markerStyle_sPHENIX_AdditionalData_4 = 25;
    int color_sPHENIX_AdditionalData_4 = kOrange - 3;
    
    TGraphErrors* corrected_v2_0_20_Additional_graph_4 = CreateGraph(ptCenters, corrected_v2_0_20_Additional_4, corrected_v2_0_20_Errors_Additional_4);
    corrected_v2_0_20_Additional_graph_4->SetMarkerColor(color_sPHENIX_AdditionalData_4);
    corrected_v2_0_20_Additional_graph_4->SetLineColor(color_sPHENIX_AdditionalData_4);
    corrected_v2_0_20_Additional_graph_4->SetMarkerSize(0.75);
    corrected_v2_0_20_Additional_graph_4->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_4);
    
    TGraphErrors* corrected_v2_20_40_Additional_graph_4 = CreateGraph(ptCenters, corrected_v2_20_40_Additional_4, corrected_v2_20_40_Errors_Additional_4);
    corrected_v2_20_40_Additional_graph_4->SetMarkerColor(color_sPHENIX_AdditionalData_4);
    corrected_v2_20_40_Additional_graph_4->SetLineColor(color_sPHENIX_AdditionalData_4);
    corrected_v2_20_40_Additional_graph_4->SetMarkerSize(0.75);
    corrected_v2_20_40_Additional_graph_4->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_4);
    
    TGraphErrors* corrected_v2_40_60_Additional_graph_4 = CreateGraph(ptCenters, corrected_v2_40_60_Additional_4, corrected_v2_40_60_Errors_Additional_4);
    corrected_v2_40_60_Additional_graph_4->SetMarkerColor(color_sPHENIX_AdditionalData_4);
    corrected_v2_40_60_Additional_graph_4->SetLineColor(color_sPHENIX_AdditionalData_4);
    corrected_v2_40_60_Additional_graph_4->SetMarkerSize(0.75);
    corrected_v2_40_60_Additional_graph_4->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_4);
    
    
    
    const auto& corrected_v2_0_20_Additional_5 = data_5.corrected_v2_0_20_Additional_5;
    const auto& corrected_v2_20_40_Additional_5 = data_5.corrected_v2_20_40_Additional_5;
    const auto& corrected_v2_40_60_Additional_5 = data_5.corrected_v2_40_60_Additional_5;
    const auto& corrected_v2_0_20_Errors_Additional_5 = data_5.corrected_v2_0_20_Errors_Additional_5;
    const auto& corrected_v2_20_40_Errors_Additional_5 = data_5.corrected_v2_20_40_Errors_Additional_5;
    const auto& corrected_v2_40_60_Errors_Additional_5 = data_5.corrected_v2_40_60_Errors_Additional_5;
    const auto& corrected_v3_0_20_Additional_5 = data_5.corrected_v3_0_20_Additional_5;
    const auto& corrected_v3_40_60_Additional_5 = data_5.corrected_v3_40_60_Additional_5;
    const auto& corrected_v3_0_20_Errors_Additional_5 = data_5.corrected_v3_0_20_Errors_Additional_5;
    const auto& corrected_v3_40_60_Errors_Additional_5 = data_5.corrected_v3_40_60_Errors_Additional_5;
    
    int markerStyle_sPHENIX_AdditionalData_5 = 24;
    int color_sPHENIX_AdditionalData_5 = kSpring + 4;
    
    TGraphErrors* corrected_v2_0_20_Additional_graph_5 = CreateGraph(ptCenters, corrected_v2_0_20_Additional_5, corrected_v2_0_20_Errors_Additional_5);
    corrected_v2_0_20_Additional_graph_5->SetMarkerColor(color_sPHENIX_AdditionalData_5);
    corrected_v2_0_20_Additional_graph_5->SetLineColor(color_sPHENIX_AdditionalData_5);
    corrected_v2_0_20_Additional_graph_5->SetMarkerSize(0.75);
    corrected_v2_0_20_Additional_graph_5->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_5);
    
    TGraphErrors* corrected_v2_20_40_Additional_graph_5 = CreateGraph(ptCenters, corrected_v2_20_40_Additional_5, corrected_v2_20_40_Errors_Additional_5);
    corrected_v2_20_40_Additional_graph_5->SetMarkerColor(color_sPHENIX_AdditionalData_5);
    corrected_v2_20_40_Additional_graph_5->SetLineColor(color_sPHENIX_AdditionalData_5);
    corrected_v2_20_40_Additional_graph_5->SetMarkerSize(0.75);
    corrected_v2_20_40_Additional_graph_5->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_5);
    
    TGraphErrors* corrected_v2_40_60_Additional_graph_5 = CreateGraph(ptCenters, corrected_v2_40_60_Additional_5, corrected_v2_40_60_Errors_Additional_5);
    corrected_v2_40_60_Additional_graph_5->SetMarkerColor(color_sPHENIX_AdditionalData_5);
    corrected_v2_40_60_Additional_graph_5->SetLineColor(color_sPHENIX_AdditionalData_5);
    corrected_v2_40_60_Additional_graph_5->SetMarkerSize(0.75);
    corrected_v2_40_60_Additional_graph_5->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_5);
    
    
    
    
    
    
    const auto& corrected_v2_0_20_Additional_6 = data_6.corrected_v2_0_20_Additional_6;
    const auto& corrected_v2_20_40_Additional_6 = data_6.corrected_v2_20_40_Additional_6;
    const auto& corrected_v2_40_60_Additional_6 = data_6.corrected_v2_40_60_Additional_6;
    const auto& corrected_v2_0_20_Errors_Additional_6 = data_6.corrected_v2_0_20_Errors_Additional_6;
    const auto& corrected_v2_20_40_Errors_Additional_6 = data_6.corrected_v2_20_40_Errors_Additional_6;
    const auto& corrected_v2_40_60_Errors_Additional_6 = data_6.corrected_v2_40_60_Errors_Additional_6;
    const auto& corrected_v3_0_20_Additional_6 = data_6.corrected_v3_0_20_Additional_6;
    const auto& corrected_v3_40_60_Additional_6 = data_6.corrected_v3_40_60_Additional_6;
    const auto& corrected_v3_0_20_Errors_Additional_6 = data_6.corrected_v3_0_20_Errors_Additional_6;
    const auto& corrected_v3_40_60_Errors_Additional_6 = data_6.corrected_v3_40_60_Errors_Additional_6;
    
    int markerStyle_sPHENIX_AdditionalData_6 = 34;
    int color_sPHENIX_AdditionalData_6 = kPink + 7;
    
    TGraphErrors* corrected_v2_0_20_Additional_graph_6 = CreateGraph(ptCenters, corrected_v2_0_20_Additional_6, corrected_v2_0_20_Errors_Additional_6);
    corrected_v2_0_20_Additional_graph_6->SetMarkerColor(color_sPHENIX_AdditionalData_6);
    corrected_v2_0_20_Additional_graph_6->SetLineColor(color_sPHENIX_AdditionalData_6);
    corrected_v2_0_20_Additional_graph_6->SetMarkerSize(0.75);
    corrected_v2_0_20_Additional_graph_6->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_6);
    
    TGraphErrors* corrected_v2_20_40_Additional_graph_6 = CreateGraph(ptCenters, corrected_v2_20_40_Additional_6, corrected_v2_20_40_Errors_Additional_6);
    corrected_v2_20_40_Additional_graph_6->SetMarkerColor(color_sPHENIX_AdditionalData_6);
    corrected_v2_20_40_Additional_graph_6->SetLineColor(color_sPHENIX_AdditionalData_6);
    corrected_v2_20_40_Additional_graph_6->SetMarkerSize(0.75);
    corrected_v2_20_40_Additional_graph_6->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_6);
    
    TGraphErrors* corrected_v2_40_60_Additional_graph_6 = CreateGraph(ptCenters, corrected_v2_40_60_Additional_6, corrected_v2_40_60_Errors_Additional_6);
    corrected_v2_40_60_Additional_graph_6->SetMarkerColor(color_sPHENIX_AdditionalData_6);
    corrected_v2_40_60_Additional_graph_6->SetLineColor(color_sPHENIX_AdditionalData_6);
    corrected_v2_40_60_Additional_graph_6->SetMarkerSize(0.75);
    corrected_v2_40_60_Additional_graph_6->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_6);
    
    
    
    
    
    const auto& corrected_v2_0_20_Additional_7 = data_7.corrected_v2_0_20_Additional_7;
    const auto& corrected_v2_20_40_Additional_7 = data_7.corrected_v2_20_40_Additional_7;
    const auto& corrected_v2_40_60_Additional_7 = data_7.corrected_v2_40_60_Additional_7;
    const auto& corrected_v2_0_20_Errors_Additional_7 = data_7.corrected_v2_0_20_Errors_Additional_7;
    const auto& corrected_v2_20_40_Errors_Additional_7 = data_7.corrected_v2_20_40_Errors_Additional_7;
    const auto& corrected_v2_40_60_Errors_Additional_7 = data_7.corrected_v2_40_60_Errors_Additional_7;
    const auto& corrected_v3_0_20_Additional_7 = data_7.corrected_v3_0_20_Additional_7;
    const auto& corrected_v3_40_60_Additional_7 = data_7.corrected_v3_40_60_Additional_7;
    const auto& corrected_v3_0_20_Errors_Additional_7 = data_7.corrected_v3_0_20_Errors_Additional_7;
    const auto& corrected_v3_40_60_Errors_Additional_7 = data_7.corrected_v3_40_60_Errors_Additional_7;
    
    int markerStyle_sPHENIX_AdditionalData_7 = 47;
    int color_sPHENIX_AdditionalData_7 = kCyan + 1;
    
    TGraphErrors* corrected_v2_0_20_Additional_graph_7 = CreateGraph(ptCenters, corrected_v2_0_20_Additional_7, corrected_v2_0_20_Errors_Additional_7);
    corrected_v2_0_20_Additional_graph_7->SetMarkerColor(color_sPHENIX_AdditionalData_7);
    corrected_v2_0_20_Additional_graph_7->SetLineColor(color_sPHENIX_AdditionalData_7);
    corrected_v2_0_20_Additional_graph_7->SetMarkerSize(0.75);
    corrected_v2_0_20_Additional_graph_7->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_7);
    
    TGraphErrors* corrected_v2_20_40_Additional_graph_7 = CreateGraph(ptCenters, corrected_v2_20_40_Additional_7, corrected_v2_20_40_Errors_Additional_7);
    corrected_v2_20_40_Additional_graph_7->SetMarkerColor(color_sPHENIX_AdditionalData_7);
    corrected_v2_20_40_Additional_graph_7->SetLineColor(color_sPHENIX_AdditionalData_7);
    corrected_v2_20_40_Additional_graph_7->SetMarkerSize(0.75);
    corrected_v2_20_40_Additional_graph_7->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_7);
    
    TGraphErrors* corrected_v2_40_60_Additional_graph_7 = CreateGraph(ptCenters, corrected_v2_40_60_Additional_7, corrected_v2_40_60_Errors_Additional_7);
    corrected_v2_40_60_Additional_graph_7->SetMarkerColor(color_sPHENIX_AdditionalData_7);
    corrected_v2_40_60_Additional_graph_7->SetLineColor(color_sPHENIX_AdditionalData_7);
    corrected_v2_40_60_Additional_graph_7->SetMarkerSize(0.75);
    corrected_v2_40_60_Additional_graph_7->SetMarkerStyle(markerStyle_sPHENIX_AdditionalData_7);
    
    
    
    
    
    
    
    
    /*
     0-20 percent v2
     */
    TCanvas *c_p009_p010_Overlay_0_20_v2_corrected = new TCanvas("c_p009_p010_Overlay_0_20_v2_corrected", "#pi^{0} #it{v}_{2}, p011 Data, Without PDC, vertex = (0, 0, z), Energy Cut of 0.5 and 1 GeV  vs #it{p}_{T} 0-20% Centrality", 800, 600);
    corrected_graph_0_20_v2->Draw("AP");  // This will be the base graph
    corrected_graph_0_20_v2->SetTitle("#pi^{0} #it{v}_{2} vs #it{p}_{T} 0-20% Centrality");
    corrected_graph_0_20_v2->GetXaxis()->SetTitle("p_{T}");
    corrected_graph_0_20_v2->GetYaxis()->SetTitle("v_{2}");
    

    for (int i = 0; i < ptCenters.size(); ++i) {
        //corrected_graph_0_20_v2->SetPoint(i, ptCenters[i] - 2.0 * offset, corrected_v2_0_20[i]); //uncomment if use additional points
        
        //corrected_v2_0_20_Additional_graph->SetPoint(i, ptCenters[i] + 2.0 * offset, corrected_v2_0_20_Additional[i]);
        
        //corrected_v2_0_20_Additional_graph_2->SetPoint(i, ptCenters[i] + 2.0 * offset, corrected_v2_0_20_Additional_2[i]);
//        //corrected_v2_0_20_Additional_graph_3->SetPoint(i, ptCenters[i] - 0.0 * offset, corrected_v2_0_20_Additional_3[i]);
//        corrected_v2_0_20_Additional_graph_4->SetPoint(i, ptCenters[i] + 1.3 * offset, corrected_v2_0_20_Additional_4[i]);
//        corrected_v2_0_20_Additional_graph_5->SetPoint(i, ptCenters[i] + 2.8 * offset, corrected_v2_0_20_Additional_5[i]);
//        corrected_v2_0_20_Additional_graph_6->SetPoint(i, ptCenters[i] + 3.9 * offset, corrected_v2_0_20_Additional_6[i]);
//        corrected_v2_0_20_Additional_graph_7->SetPoint(i, ptCenters[i] + 5.1 * offset, corrected_v2_0_20_Additional_7[i]);
        
        
//        graph_0_10->SetPoint(i, ptCenters[i] - offset * 2.0, v2_0_10[i]);
//        graph_10_20->SetPoint(i, ptCenters[i] + offset, v2_10_20[i]);
        
    }
//    corrected_v2_0_20_Additional_graph -> Draw("P SAME");
    //corrected_v2_0_20_Additional_graph_2-> Draw("P SAME");
    
    corrected_graph_0_20_v2->SetMinimum(-.1); // Set the minimum y value
    corrected_graph_0_20_v2->SetMaximum(0.4); // Set the maximum y value
//
//    corrected_v2_0_20_Additional_graph_3-> Draw("P SAME");
//    corrected_v2_0_20_Additional_graph_4-> Draw("P SAME");
//    corrected_v2_0_20_Additional_graph_5-> Draw("P SAME");
//    corrected_v2_0_20_Additional_graph_6-> Draw("P SAME");
//    corrected_v2_0_20_Additional_graph_7-> Draw("P SAME");
    
    
//    graph_0_10->Draw("P SAME");
//    graph_10_20->Draw("P SAME");
//    
//    TLegend *legend_p009_p010_Overlay_0_20_v2_corrected = new TLegend(0.11, 0.64, 0.51, 0.89);
//    //legend_p009_p010_Overlay_0_20_v2_corrected->SetNColumns(2);
//
//    legend_p009_p010_Overlay_0_20_v2_corrected->SetBorderSize(0);
//    legend_p009_p010_Overlay_0_20_v2_corrected->SetTextSize(0.03);
////    legend_p009_p010_Overlay_0_20_v2_corrected->AddEntry(corrected_graph_0_20_v2, "0-20%, v_{2}^{#pi^{0}}, p013", "pe");
////    legend_p009_p010_Overlay_0_20_v2_corrected->AddEntry(corrected_v2_0_20_Additional_graph, "0-20%, v_{2}^{#pi^{0}}, p013", "pe");
////
////    legend_p009_p010_Overlay_0_20_v2_corrected->AddEntry(corrected_v2_0_20_Additional_graph_2, "0-20%, v_{2}^{#pi^{0}}, Type C", "pe");
////    legend_p009_p010_Overlay_0_20_v2_corrected->AddEntry(corrected_v2_0_20_Additional_graph_3, "0-20%, v_{2}^{#pi^{0}}, Type D", "pe");
////    legend_p009_p010_Overlay_0_20_v2_corrected->AddEntry(corrected_v2_0_20_Additional_graph_4, "0-20%, v_{2}^{#pi^{0}}, Type E", "pe");
////    legend_p009_p010_Overlay_0_20_v2_corrected->AddEntry(corrected_v2_0_20_Additional_graph_5, "0-20%, v_{2}^{#pi^{0}}, Type F", "pe");
////    legend_p009_p010_Overlay_0_20_v2_corrected->AddEntry(corrected_v2_0_20_Additional_graph_6, "0-20%, v_{2}^{#pi^{0}}, Type G", "pe");
////    legend_p009_p010_Overlay_0_20_v2_corrected->AddEntry(corrected_v2_0_20_Additional_graph_7, "0-20%, v_{2}^{#pi^{0}}, Type H", "pe");
//    
//    
//    
////
////    legend_p009_p010_Overlay_0_20_v2_corrected->AddEntry(graph_0_10, "0-10%, #bf{PHENIX} 2010", "pe");
////    legend_p009_p010_Overlay_0_20_v2_corrected->AddEntry(graph_10_20, "10-20%, #bf{PHENIX} 2010", "pe");
//    
//    legend_p009_p010_Overlay_0_20_v2_corrected->Draw();
    DrawZeroLine(c_p009_p010_Overlay_0_20_v2_corrected);
    c_p009_p010_Overlay_0_20_v2_corrected->Modified();
    c_p009_p010_Overlay_0_20_v2_corrected->Update();
    c_p009_p010_Overlay_0_20_v2_corrected->SaveAs((BasePlotOutputPath + "/p011_withAndWithoutZvtx_Corrected_Overlay_0_20.png").c_str());

    
    /*
     20-40 percent v2
     */
    TCanvas *c_p009_p010_Overlay_20_40_v2_corrected = new TCanvas("c_p009_p010_Overlay_20_40_v2_corrected", "#pi^{0} #it{v}_{2}, p011 Data, Without PDC, vertex = (0, 0, z), Energy Cut of 0.5 and 1 GeV  vs #it{p}_{T} 20-40% Centrality", 800, 600);
    corrected_graph_20_40_v2->Draw("AP");  // This will be the base graph
    corrected_graph_20_40_v2->SetTitle("#pi^{0} #it{v}_{2} vs #it{p}_{T} 20-40% Centrality");
    corrected_graph_20_40_v2->GetXaxis()->SetTitle("p_{T}");
    corrected_graph_20_40_v2->GetYaxis()->SetTitle("v_{2}");
    
    for (int i = 0; i < ptCenters.size(); ++i) {
//        corrected_graph_20_40_v2->SetPoint(i, ptCenters[i] - 4.3 * offset, corrected_v2_20_40[i]); //uncomment if use additional points
        
 //       corrected_v2_20_40_Additional_graph->SetPoint(i, ptCenters[i] + 2.0 * offset, corrected_v2_20_40_Additional[i]);
        
//        corrected_v2_20_40_Additional_graph_2->SetPoint(i, ptCenters[i] - 1.5 * offset, corrected_v2_20_40_Additional_2[i]);
//        //corrected_v2_20_40_Additional_graph_3->SetPoint(i, ptCenters[i] - 0.0 * offset, corrected_v2_20_40_Additional_3[i]);
//        corrected_v2_20_40_Additional_graph_4->SetPoint(i, ptCenters[i] + 1.3 * offset, corrected_v2_20_40_Additional_4[i]);
//        corrected_v2_20_40_Additional_graph_5->SetPoint(i, ptCenters[i] + 2.8 * offset, corrected_v2_20_40_Additional_5[i]);
//        corrected_v2_20_40_Additional_graph_6->SetPoint(i, ptCenters[i] + 3.9 * offset, corrected_v2_20_40_Additional_6[i]);
//        corrected_v2_20_40_Additional_graph_7->SetPoint(i, ptCenters[i] + 5.1 * offset, corrected_v2_20_40_Additional_7[i]);
        
        
//        graph_0_10->SetPoint(i, ptCenters[i] - offset * 2.0, v2_0_10[i]);
//        graph_10_20->SetPoint(i, ptCenters[i] + offset, v2_10_20[i]);
        
    }
//    corrected_v2_20_40_Additional_graph -> Draw("P SAME");
    corrected_graph_20_40_v2->SetMinimum(-.1); // Set the minimum y value
    corrected_graph_20_40_v2->SetMaximum(0.4); // Set the maximum y value
//
//    corrected_v2_20_40_Additional_graph_2-> Draw("P SAME");
//    corrected_v2_20_40_Additional_graph_3-> Draw("P SAME");
//    corrected_v2_20_40_Additional_graph_4-> Draw("P SAME");
//    corrected_v2_20_40_Additional_graph_5-> Draw("P SAME");
//    corrected_v2_20_40_Additional_graph_6-> Draw("P SAME");
//    corrected_v2_20_40_Additional_graph_7-> Draw("P SAME");
    
    
//    graph_0_10->Draw("P SAME");
//    graph_10_20->Draw("P SAME");
//
//    TLegend *legend_p009_p010_Overlay_20_40_v2_corrected = new TLegend(0.11, 0.64, 0.51, 0.89);
//    //legend_p009_p010_Overlay_20_40_v2_corrected->SetNColumns(2);
//
//    legend_p009_p010_Overlay_20_40_v2_corrected->SetBorderSize(0);
//    legend_p009_p010_Overlay_20_40_v2_corrected->SetTextSize(0.025);
////    legend_p009_p010_Overlay_20_40_v2_corrected->AddEntry(corrected_graph_20_40_v2, "20-40%, v_{2}^{#pi^{0}}, p011", "pe");
////    legend_p009_p010_Overlay_20_40_v2_corrected->AddEntry(corrected_v2_20_40_Additional_graph, "20-40%, v_{2}^{#pi^{0}}, p013", "pe");
//    
////    legend_p009_p010_Overlay_20_40_v2_corrected->AddEntry(corrected_v2_20_40_Additional_graph_2, "20-40%, v_{2}^{#pi^{0}}, Type C", "pe");
////    legend_p009_p010_Overlay_20_40_v2_corrected->AddEntry(corrected_v2_20_40_Additional_graph_3, "20-40%, v_{2}^{#pi^{0}}, Type D", "pe");
////    legend_p009_p010_Overlay_20_40_v2_corrected->AddEntry(corrected_v2_20_40_Additional_graph_4, "20-40%, v_{2}^{#pi^{0}}, Type E", "pe");
////    legend_p009_p010_Overlay_20_40_v2_corrected->AddEntry(corrected_v2_20_40_Additional_graph_5, "20-40%, v_{2}^{#pi^{0}}, Type F", "pe");
////    legend_p009_p010_Overlay_20_40_v2_corrected->AddEntry(corrected_v2_20_40_Additional_graph_6, "20-40%, v_{2}^{#pi^{0}}, Type G", "pe");
////    legend_p009_p010_Overlay_20_40_v2_corrected->AddEntry(corrected_v2_20_40_Additional_graph_7, "20-40%, v_{2}^{#pi^{0}}, Type H", "pe");
//    
//    
//    
////
////    legend_p009_p010_Overlay_0_20_v2_corrected->AddEntry(graph_0_10, "0-10%, #bf{PHENIX} 2010", "pe");
////    legend_p009_p010_Overlay_0_20_v2_corrected->AddEntry(graph_10_20, "10-20%, #bf{PHENIX} 2010", "pe");
//    
//    legend_p009_p010_Overlay_20_40_v2_corrected->Draw();
    DrawZeroLine(c_p009_p010_Overlay_20_40_v2_corrected);
    c_p009_p010_Overlay_20_40_v2_corrected->Modified();
    c_p009_p010_Overlay_20_40_v2_corrected->Update();
    c_p009_p010_Overlay_20_40_v2_corrected->SaveAs((BasePlotOutputPath + "/p011_withAndWithoutZvtx_Corrected_Overlay_20_40.png").c_str());

    
    
    
    
   TCanvas *c_p009_p010_Overlay_40_60_v2_corrected = new TCanvas("c_p009_p010_Overlay_40_60_v2_corrected", "#pi^{0} #it{v}_{2}, p011 Data, Without PDC, vertex = (0, 0, z), Energy Cut of 0.5 and 1 GeV  vs #it{p}_{T} 40-60% Centrality", 800, 600);
    corrected_graph_40_60_v2->Draw("AP");  // This will be the base graph
    corrected_graph_40_60_v2->SetTitle("#pi^{0} #it{v}_{2} vs #it{p}_{T} 40-60% Centrality");
    corrected_graph_40_60_v2->GetXaxis()->SetTitle("p_{T}");
    corrected_graph_40_60_v2->GetYaxis()->SetTitle("v_{2}");
    
    
   for (int i = 0; i < ptCenters.size(); ++i) {
//       corrected_graph_40_60_v2->SetPoint(i, ptCenters[i] - 4.3 * offset, corrected_v2_40_60[i]); //uncomment if use additional points
       
//       corrected_v2_40_60_Additional_graph->SetPoint(i, ptCenters[i] + 2.0 * offset, corrected_v2_40_60_Additional[i]);
       
//       corrected_v2_40_60_Additional_graph_2->SetPoint(i, ptCenters[i] - 1.5 * offset, corrected_v2_40_60_Additional_2[i]);
//       //corrected_v2_40_60_Additional_graph_3->SetPoint(i, ptCenters[i] - 0.0 * offset, corrected_v2_40_60_Additional_3[i]);
//       corrected_v2_40_60_Additional_graph_4->SetPoint(i, ptCenters[i] + 1.3 * offset, corrected_v2_40_60_Additional_4[i]);
//       corrected_v2_40_60_Additional_graph_5->SetPoint(i, ptCenters[i] + 2.8 * offset, corrected_v2_40_60_Additional_5[i]);
//       corrected_v2_40_60_Additional_graph_6->SetPoint(i, ptCenters[i] + 3.9 * offset, corrected_v2_40_60_Additional_6[i]);
//       corrected_v2_40_60_Additional_graph_7->SetPoint(i, ptCenters[i] + 5.1 * offset, corrected_v2_40_60_Additional_7[i]);
       
       
//        graph_0_10->SetPoint(i, ptCenters[i] - offset * 2.0, v2_0_10[i]);
//        graph_10_20->SetPoint(i, ptCenters[i] + offset, v2_10_20[i]);
       
   }
//   corrected_v2_40_60_Additional_graph -> Draw("P SAME");
   corrected_graph_40_60_v2->SetMinimum(-.1); // Set the minimum y value
   corrected_graph_40_60_v2->SetMaximum(0.4); // Set the maximum y value
   
//   corrected_v2_40_60_Additional_graph_2-> Draw("P SAME");
//    corrected_v2_40_60_Additional_graph_3-> Draw("P SAME");
//   corrected_v2_40_60_Additional_graph_4-> Draw("P SAME");
//   corrected_v2_40_60_Additional_graph_5-> Draw("P SAME");
//   corrected_v2_40_60_Additional_graph_6-> Draw("P SAME");
//   corrected_v2_40_60_Additional_graph_7-> Draw("P SAME");
   
   
//    graph_0_10->Draw("P SAME");
//    graph_10_20->Draw("P SAME");
//
//   TLegend *legend_p009_p010_Overlay_40_60_v2_corrected = new TLegend(0.11, 0.64, 0.51, 0.89);
//   //legend_p009_p010_Overlay_40_60_v2_corrected->SetNColumns(2);
//
//   legend_p009_p010_Overlay_40_60_v2_corrected->SetBorderSize(0);
//   legend_p009_p010_Overlay_40_60_v2_corrected->SetTextSize(0.025);
////   legend_p009_p010_Overlay_40_60_v2_corrected->AddEntry(corrected_graph_40_60_v2, "40-60%, v_{2}^{#pi^{0}}, p011", "pe");
////   legend_p009_p010_Overlay_40_60_v2_corrected->AddEntry(corrected_v2_40_60_Additional_graph, "40-60%, v_{2}^{#pi^{0}}, p013", "pe");
//   
////   legend_p009_p010_Overlay_40_60_v2_corrected->AddEntry(corrected_v2_40_60_Additional_graph_2, "40-60%, v_{2}^{#pi^{0}}, Type C", "pe");
////   legend_p009_p010_Overlay_40_60_v2_corrected->AddEntry(corrected_v2_40_60_Additional_graph_3, "40-60%, v_{2}^{#pi^{0}}, Type D", "pe");
////   legend_p009_p010_Overlay_40_60_v2_corrected->AddEntry(corrected_v2_40_60_Additional_graph_4, "40-60%, v_{2}^{#pi^{0}}, Type E", "pe");
////   legend_p009_p010_Overlay_40_60_v2_corrected->AddEntry(corrected_v2_40_60_Additional_graph_5, "40-60%, v_{2}^{#pi^{0}}, Type F", "pe");
////   legend_p009_p010_Overlay_40_60_v2_corrected->AddEntry(corrected_v2_40_60_Additional_graph_6, "40-60%, v_{2}^{#pi^{0}}, Type G", "pe");
////   legend_p009_p010_Overlay_40_60_v2_corrected->AddEntry(corrected_v2_40_60_Additional_graph_7, "40-60%, v_{2}^{#pi^{0}}, Type H", "pe");
//   
//   
//   
////
////    legend_p009_p010_Overlay_0_20_v2_corrected->AddEntry(graph_0_10, "0-10%, #bf{PHENIX} 2010", "pe");
////    legend_p009_p010_Overlay_0_20_v2_corrected->AddEntry(graph_10_20, "10-20%, #bf{PHENIX} 2010", "pe");
//   
//   legend_p009_p010_Overlay_40_60_v2_corrected->Draw();
   DrawZeroLine(c_p009_p010_Overlay_40_60_v2_corrected);
   c_p009_p010_Overlay_40_60_v2_corrected->Modified();
   c_p009_p010_Overlay_40_60_v2_corrected->Update();
   c_p009_p010_Overlay_40_60_v2_corrected->SaveAs((BasePlotOutputPath + "/p011_withAndWithoutZvtx_Corrected_Overlay_40_60.png").c_str());

    


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
    
    TLegend *legend_0_20_v2_v3_corrected = new TLegend(0.11, 0.11, 0.31, 0.31);
    legend_0_20_v2_v3_corrected->SetBorderSize(0);
    legend_0_20_v2_v3_corrected->SetTextSize(0.03);
    legend_0_20_v2_v3_corrected->AddEntry(corrected_graph_0_20_v2, "0-20%, v_{2}^{#pi^{0}}", "pe");
    legend_0_20_v2_v3_corrected->AddEntry(corrected_graph_0_20_v3, "0-20%, v_{3}^{#pi^{0}}", "pe");
    legend_0_20_v2_v3_corrected->Draw();
    DrawZeroLine(c_0_20_v2_v3_corrected);
    c_0_20_v2_v3_corrected->Modified();
    c_0_20_v2_v3_corrected->Update();
    c_0_20_v2_v3_corrected->SaveAs((BasePlotOutputPath + "/v2_v3_Corrected_Overlay_0_20.png").c_str());
    
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
    
    TLegend *legend_40_60_v2_v3_corrected = new TLegend(0.11, 0.11, 0.31, 0.31);
    legend_40_60_v2_v3_corrected->SetBorderSize(0);
    legend_40_60_v2_v3_corrected->SetTextSize(0.03);
    legend_40_60_v2_v3_corrected->AddEntry(corrected_graph_40_60_v2, "40-60%, v_{2}^{#pi^{0}}", "pe");
    legend_40_60_v2_v3_corrected->AddEntry(corrected_graph_40_60_v3, "40-60%, v_{3}^{#pi^{0}}", "pe");
    legend_40_60_v2_v3_corrected->Draw();
    DrawZeroLine(c_40_60_v2_v3_corrected);
    c_40_60_v2_v3_corrected->Modified();
    c_40_60_v2_v3_corrected->Update();
    c_40_60_v2_v3_corrected->SaveAs((BasePlotOutputPath + "/v2_v3_Corrected_Overlay_40_60.png").c_str());
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

void vN_calculator_AccumulatedData() {
    int subSampleSize = 30;//CHANGE TO SUB SAMPLES LOOPED THROUGH
    
    std::string baseProjectFolder = "/Users/patsfan753/Desktop/Fit_varsFina3_23/";
    /*
     Relevant Paths Set
     */
    //path to root file with QQ, qQ histograms to be analyzed
    std::string test_RootFilePath = (baseProjectFolder + "TypeH/test.root").c_str();
    
    
    //path to CSV file read in with S/B information for vn calculation
    /*
     Remember to change cut value being read in below depending on CSV choice below
     */
    std::string PlotByPlotOutput_OriginalPath = (baseProjectFolder + "TypeH/TypeHplotOutput.csv").c_str();
    
    //path for final CSV output and sample by sample calculation output
    std::string outputBasePathCSVs = (baseProjectFolder + "TypeH/SampleWise_vN_data/").c_str();
    
    const int num_v2 = 18;
    const int num_v3 = 18;
    
    // Initialize vectors to hold weighted sums and weights for each index
    std::vector<double> totalWeightedV2Corrected(num_v2, 0.0);
    std::vector<double> totalWeightedV3Corrected(num_v3, 0.0);
    std::vector<double> totalWeights_2(num_v2, 0.0);
    std::vector<double> totalWeights_3(num_v3, 0.0);
    
    // Additional variables for error propagation
    std::vector<double> sum_wk_xk_2(num_v2, 0.0);
    std::vector<double> sum_wk_xk_2_squared(num_v2, 0.0);
    std::vector<double> sum_wk_2_squared(num_v2, 0.0);

    std::vector<double> sum_wk_xk_3(num_v3, 0.0);
    std::vector<double> sum_wk_xk_3_squared(num_v3, 0.0);
    std::vector<double> sum_wk_3_squared(num_v3, 0.0);
    
    
    
    // Variables to track negative QQ2 and QQ3 means
    int negativeQQ2Count = 0;
    int negativeQQ3Count = 0;
    
    std::map<int, int> skippedQQ2Counts;
    std::map<int, int> skippedQQ3Counts;
    
    std::vector<std::pair<std::string, int>> skippedIndices; // To store sub samples and indices skipped due to negative values
    
    
    std::map<std::string, std::vector<int>> eventsPerCentralityQQ;
    std::map<std::string, std::vector<int>> eventsPerqQ;

    std::ofstream logFile("/Users/patsfan753/Desktop/Fit_varsFina3_23/TypeH/logFile_30_samples_histogram_entries_TypeH.txt");
    if (!logFile) {
        std::cerr << "Error: Unable to open log file." << std::endl;
        return; // Or handle the error appropriately
    }
    logFile << "Processing Data\n========================\n";

    
    std::vector<int> subsampleIndices;
    std::vector<float> v3CorrectedValues, wk3Values, QQ3MeanValues;
    std::vector<bool> isQQ3MeanNegative;

    // Loop over sub-samples --CHANGE SAMPLE SIZE
    for (int subSampleIndex = 0; subSampleIndex < subSampleSize; ++subSampleIndex) {
        std::cout << "-----------------------------------------\n";
        std::cout << "Processing Sub-Sample: " << subSampleIndex << std::endl;
        std::cout << "-----------------------------------------\n";
        
        
        // Print sub-sample header
        logFile << "\n-----------------------------------------\n";
        logFile << "Sub-Sample: " << subSampleIndex << "\n";
        logFile << "-----------------------------------------\n";
        
        
        TFile *file = new TFile(test_RootFilePath.c_str(), "READ");
        if (!file || file->IsZombie()) {
            std::cerr << "Error opening file or file is not a valid ROOT file" << std::endl;
            return;
        }
        
        /*
         Histograms Related to v2, SET UP VECTORS FOR QQ2, qQ2, qQ2_bg, qQ2_bg_left
         */
        // Define the histogram names for QQ2 within the vn/QQ2 directory
        std::string hist_name_QQ2[3] = {"vn/" + std::to_string(subSampleIndex) + "/QQ2/hQQ2_" + std::to_string(subSampleIndex) + "_40-60", "vn/" + std::to_string(subSampleIndex) + "/QQ2/hQQ2_" + std::to_string(subSampleIndex) + "_20-40", "vn/" + std::to_string(subSampleIndex) + "/QQ2/hQQ2_" + std::to_string(subSampleIndex) + "_0-20"};

        // Define the histogram names for qQ within the vn/qQ directory
        std::string hist_name_qQ2[18];
        for (int i = 0; i < 18; ++i) {
            hist_name_qQ2[i] = "vn/" + std::to_string(subSampleIndex) + "/qQ2/hqQ2_" + std::to_string(subSampleIndex) + "_" + std::to_string(i);
        }
        
        std::string hist_name_qQ2_QQ2[18];
        for (int i = 0; i < 18; ++i) {
            hist_name_qQ2[i] = "vn/" + std::to_string(subSampleIndex) + "/qQ2/hqQ2_" + std::to_string(subSampleIndex) + "_" + std::to_string(i);
        }
        
        std::string hist_name_qQ2_bg[18];
        for (int i = 0; i < 18; ++i) {
            hist_name_qQ2_bg[i] = "vn/" + std::to_string(subSampleIndex) + "/qQ2_bg/hqQ2_bg_" + std::to_string(subSampleIndex) + "_" + std::to_string(i);
        }

        std::string hist_name_qQ2_bg_left[18];
        for (int i = 0; i < 18; ++i) {
            hist_name_qQ2_bg_left[i] = "vn/" + std::to_string(subSampleIndex) + "/qQ2_bg_left/hqQ2_bg_left_" + std::to_string(subSampleIndex) + "_" + std::to_string(i);
        }
        
        
        /*
         Histograms Related to v3, SET UP VECTORS FOR QQ3, qQ3, qQ3_bg, qQ3_bg_left
         */
        std::string hist_name_QQ3[3] = {"vn/" + std::to_string(subSampleIndex) + "/QQ3/hQQ3_" + std::to_string(subSampleIndex) + "_40-60", "vn/" + std::to_string(subSampleIndex) + "/QQ3/hQQ3_" + std::to_string(subSampleIndex) + "_20-40", "vn/" + std::to_string(subSampleIndex) + "/QQ3/hQQ3_" + std::to_string(subSampleIndex) + "_0-20"};

        // Define the histogram names for qQ within the vn/qQ directory
        std::string hist_name_qQ3[18];
        for (int i = 0; i < 18; ++i) {
            hist_name_qQ3[i] = "vn/" + std::to_string(subSampleIndex) + "/qQ3/hqQ3_" + std::to_string(subSampleIndex) + "_" + std::to_string(i);
        }
        std::string hist_name_qQ3_bg[18];
        for (int i = 0; i < 18; ++i) {
            hist_name_qQ3_bg[i] = "vn/" + std::to_string(subSampleIndex) + "/qQ3_bg/hqQ3_bg_" + std::to_string(subSampleIndex) + "_" + std::to_string(i);
        }

        std::string hist_name_qQ3_bg_left[18];
        for (int i = 0; i < 18; ++i) {
            hist_name_qQ3_bg_left[i] = "vn/" + std::to_string(subSampleIndex) + "/qQ3_bg_left/hqQ3_bg_left_" + std::to_string(subSampleIndex) + "_" + std::to_string(i);
        }
        // Adjusted drawAndSaveHist to create sub-sample specific directories
        auto drawAndSaveHist = [&](const std::string& histPath, const std::string& baseDir) {
            TH1F *hist = (TH1F*)file->Get(histPath.c_str());
            if (!hist) {
                std::cerr << "Error: Histogram " << histPath << " not found" << std::endl;
                return;
            }
            TCanvas *c = new TCanvas();
            c->SetLogy(); // Set logarithmic scale for y-axis
            
            // Create a sub-sample specific directory within the base directory
            std::string subSampleDir = baseDir + "/subSample_" + std::to_string(subSampleIndex);
            gSystem->mkdir(subSampleDir.c_str(), kTRUE); // ROOT's gSystem->mkdir to create directories
            
            // Construct the histogram file name including the sub-sample index
            std::string histFileName = histPath.substr(histPath.find_last_of("/") + 1) + ".png";
            std::string savePath = subSampleDir + "/" + histFileName; // Full path for saving the histogram image
            
            hist->Draw();
            c->SaveAs(savePath.c_str());
            delete c;
        };
        
        /*
         If want to output all QQ, qQ histograms used, SHOULD USE THE CODE BELOW SETTING PATHS OF WHERE TO FUNNEL SUCH DATA
         */
        // Define the base directories for v2 and v3 histograms
        std::string baseDirV2 = (baseProjectFolder + "plotOutput/qQ_QQ_histograms_byProd/qQ_and_QQ_histograms_p011/30_samples_p011_withZvtx_NO_ZDC_E_1/n_2").c_str();
        std::string baseDirV3 = (baseProjectFolder + "plotOutput/qQ_QQ_histograms_byProd/qQ_and_QQ_histograms_p011/30_samples_p011_withZvtx_NO_ZDC_E_1/n_3").c_str();
        
        /*
         Comment out below if re run macro and already made hists
         */
        
//        // Process and save QQ and qQ histograms for v2
//        for (const auto& histPath : hist_name_QQ2) {
//            drawAndSaveHist(histPath, baseDirV2);
//        }
//        for (const auto& histPath : hist_name_qQ2) {
//            drawAndSaveHist(histPath, baseDirV2);
//        }
//        for (const auto& histPath : hist_name_qQ2_bg) {
//            drawAndSaveHist(histPath, baseDirV2);
//        }
////        for (const auto& histPath : hist_name_qQ2_bg_left) {
////            drawAndSaveHist(histPath, baseDirV2);
////        }
////        
//        // Process and save QQ and qQ histograms for v3
//        for (const auto& histPath : hist_name_QQ3) {
//            drawAndSaveHist(histPath, baseDirV3);
//        }
//        for (const auto& histPath : hist_name_qQ3) {
//            drawAndSaveHist(histPath, baseDirV3);
//        }
//        for (const auto& histPath : hist_name_qQ3_bg) {
//            drawAndSaveHist(histPath, baseDirV3);
//        }
////        for (const auto& histPath : hist_name_qQ3_bg_left) {
////            drawAndSaveHist(histPath, baseDirV3);
////        }
//        
        /*
         RETRIEVE MEANS AND ERROR FOR n = 2 histograms
         */
        // Retrieve and calculate QQ means and their errors
        float QQ2_mean[3], QQ2_error[3];
        int QQ2_entries[3] = {0}; // Store number of entries for each QQ2 histogram
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
            QQ2_entries[i] = h_QQ2->GetEntries();
            int nEntries = h_QQ2->GetEntries();
            QQ2_error[i] = stdDev / sqrt(nEntries); // Standard deviation divided by the square root of the number of entries
        }
        // Retrieve and calculate qQ means and their errors
        std::string currentCentrality = ""; // Track the current centrality to know when it changes
        float qQ2_mean[18], qQ2_error[18];
        double w_k_2[18];
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
            w_k_2[i] = h_qQ2->GetSumOfWeights();
            
            int QQ_index = i / 6; // Determine the centrality based on 'i'
            if (QQ2_mean[QQ_index] <= 0) {
                // If QQ2 mean is not positive, skip this iteration to not log negative or zero centrality data
                continue;
            }

            std::string centrality;
            switch (QQ_index) {
                case 0: centrality = "40-60"; break;
                case 1: centrality = "20-40"; break;
                case 2: centrality = "0-20"; break;
            }

            if (currentCentrality != centrality) {
                if (!currentCentrality.empty()) {
                    logFile << "\n"; // Add spacing between centrality groups
                }
                logFile << "Centrality: " << centrality << ", QQ Events: " << QQ2_entries[QQ_index] << "\n";
                currentCentrality = centrality;
            }

            // Now we only log data for h_qQ2 corresponding to positive h_QQ2 mean values
            double percent = 100.0 * h_qQ2->GetEntries() / QQ2_entries[QQ_index];
            std::string pT_range = std::to_string(2 + 0.5 * (i % 6)) + "-" + std::to_string(2.5 + 0.5 * (i % 6)) + " GeV";

            logFile << "pT: " << pT_range << ", qQ Events: " << nEntries << ", " << std::fixed << std::setprecision(2) << percent << " %" << "\n";
        }
        logFile << "\n=================================================\n\n";
        
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
        float qQ3_mean[18], qQ3_error[18];
        double w_k_3[18];
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
            w_k_3[i] = h_qQ3->GetSumOfWeights();
        }
        
        // After calculating w_k_2[i] and w_k_3[i] for each i
        for (int i = 0; i < 18; ++i) { // Assuming 18 is the loop count for both v2 and v3 histograms as indicated by num_v2 and num_v3
            TH1F *h_qQ2 = (TH1F*)file->Get(hist_name_qQ2[i].c_str());
            if (!h_qQ2) {
                std::cerr << "Error: qQ histogram " << hist_name_qQ2[i] << " not found" << std::endl;
                file->Close();
                delete file;
                return;
            }
            w_k_2[i] = h_qQ2->GetSumOfWeights();

            TH1F *h_qQ3 = (TH1F*)file->Get(hist_name_qQ3[i].c_str());
            if (!h_qQ3) {
                std::cerr << "Error: qQ histogram " << hist_name_qQ3[i] << " not found" << std::endl;
                file->Close();
                delete file;
                return;
            }
            w_k_3[i] = h_qQ3->GetSumOfWeights();
        }

        
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
        /*
        User-defined cut values, ensures only rows of CSV with cuts below correspond to SB used in calc
        Also specifies rows of data vN info is appended to
         */
        float userEnergy = 1.0; //switch to which ever cut
        float userAsymm = 0.5;
        float userChi2 = 4;
        float userDeltaR = 0;
        
        /*
         If boolean at top of macro is false, it means run calculation code not plotting, which is all the steps below
         */
        if (!Plot_vN_bool) {
            /*
             CSV data outputted from invariant mass fits
             */
            std::ifstream inFile(PlotByPlotOutput_OriginalPath.c_str());
            // Open the new CSV file for this run for writing
            std::string writeCSVFilePath = outputBasePathCSVs + "PlotByPlot-" + std::to_string(subSampleIndex) + "_updated.csv";
            std::ofstream outFile(writeCSVFilePath, std::ios::out);
            if (!outFile) {
                std::cerr << "Error opening output CSV file: " << writeCSVFilePath << std::endl;
                continue;
            }

            std::string line;
            int lineIndex = 0;
            while (getline(inFile, line)) {
                if (lineIndex == 0) {
                    // Header line
                    outFile << line << ",v2_measured,bg_v2,left_bg_v2,v2_corrected,v3_measured,bg_v3,left_bg_v3,v3_corrected" << std::endl;
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
                        
                        /*
                         The denominator quantity is by centrality, set QQ index to use proper histogram's mean and error for correct range of indices that correspond to each centrality
                         */
                        int QQ_index = 0;
                        if (idx >= 0 && idx <= 5) {
                            QQ_index = 0; // 40-60% centrality
                        } else if (idx >= 6 && idx <= 11) {
                            QQ_index = 1; // 20-40% centrality
                        } else if (idx >= 12 && idx <= 17) {
                            QQ_index = 2; // 0-20% centrality
                        }

                        float v2_value = qQ2_mean[idx] / sqrt(QQ2_mean[QQ_index]); //calculate measured v2 and its corresponding error

                        float bg_v2_value = qQ2_bg_mean[idx] / sqrt(QQ2_mean[QQ_index]); //bg v2 calculation and corresponding error

                        // Calculate left background v2 and left bg v2_error
                        float left_bg_v2_value = qQ2_bg_left_mean[idx] / sqrt(QQ2_mean[QQ_index]);

                        float v2_corrected = v2_value * (1 + (1 / SB)) - ((1 / SB) * bg_v2_value); //calculate pi0 v2 and its corresponding error
                        
                        /*
                         CALCULATIONS for v3 measured, left/right background, corrected, with error prop, done exactly the same as above but with n = 3 hists
                         */
                        float v3_value = qQ3_mean[idx] / sqrt(QQ3_mean[QQ_index]);
                        float bg_v3_value = qQ3_bg_mean[idx] / sqrt(QQ3_mean[QQ_index]);
                        float left_bg_v3_value = qQ3_bg_left_mean[idx] / sqrt(QQ3_mean[QQ_index]);
                        
                        float v3_corrected = v3_value * (1 + (1 / SB)) - ((1 / SB) * bg_v3_value);
                        
                        if (idx == 0) { // Only for the first index
                            subsampleIndices.push_back(subSampleIndex);
                            v3CorrectedValues.push_back(v3_corrected);
                            wk3Values.push_back(w_k_3[idx]);
                            QQ3MeanValues.push_back(QQ3_mean[QQ_index]);
                            isQQ3MeanNegative.push_back(QQ3_mean[QQ_index] < 0);
                        }
                        
                        // Check for negative QQ2 or QQ3 values
                        if (QQ2_mean[QQ_index] > 0) {
                            totalWeightedV2Corrected[idx] += v2_corrected * w_k_2[idx];
                            totalWeights_2[idx] += w_k_2[idx];
                            
                            sum_wk_xk_2[idx] += w_k_2[idx] * v2_corrected * v2_corrected; // Accumulate the weighted square of corrected v2
                            sum_wk_2_squared[idx] += w_k_2[idx] * w_k_2[idx]; // Accumulate the square of weights for v2
  
                        }
                        else {
                            std::cout << "\033[31mNegative QQ2_mean encountered: Sub Sample = " << subSampleIndex << ", Centrality Index = " << QQ_index << "\033[0m" << std::endl;
                            negativeQQ2Count++;
                            skippedQQ2Counts[QQ_index]++;
                            std::cout << std::endl;
                        }
                        // Only fill hV3Corrected if QQ3_mean[QQ_index] is not negative
                        if (QQ3_mean[QQ_index] > 0) {
                            totalWeightedV3Corrected[idx] += v3_corrected * w_k_3[idx];
                            totalWeights_3[idx] += w_k_3[idx];
                            
                            sum_wk_xk_3[idx] += w_k_3[idx] * v3_corrected * v3_corrected; // Accumulate the weighted square of corrected v3
                            
                            sum_wk_3_squared[idx] += w_k_3[idx] * w_k_3[idx]; // Accumulate the square of weights for v3
                            
                        }
                        else {
                            std::cout << "Negative QQ3_mean encountered: Sub Sample = " << subSampleIndex << ", Centrality Index = " << QQ_index << std::endl;
                            negativeQQ3Count++;
                            skippedQQ3Counts[QQ_index]++;
                            std::cout << std::endl; // And another extra line break here
                        }
                        std::stringstream ss;
                        /*
                         Construct appended data to new CSV
                         */
                        ss << line << ","
                           << v2_value << "," << bg_v2_value << "," << left_bg_v2_value << "," << v2_corrected << ","
                           << v3_value << "," << bg_v3_value << "," << left_bg_v3_value << "," << v3_corrected;
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
    }
    logFile.close();
    
    // Check and execute Plot_vN if boolean is set to true
    if (Plot_vN_bool) {
        AdditionalData data;
        /*
         Path to DATASET TO OVERALAY with whatever is plotted in Plot_vN method above
         */
        std::string Additional_Data_Path = "/Users/patsfan753/Desktop/combined_lowest_v2_errors.csv";
        Read_Additonal_DataSet(Additional_Data_Path, data);
        
        AdditionalData_2 data_2;
        /*
         Path to DATASET TO OVERALAY with whatever is plotted in Plot_vN method above
         */
        std::string Additional_Data_Path_2 = "/Users/patsfan753/Desktop/Fit_varsFina3_23/TypeH/vN_averaged_TypeH_MBplusCentral_noZDC_E_1.csv";
        Read_Additonal_DataSet_2(Additional_Data_Path_2, data_2);

        
        AdditionalData_3 data_3;
        /*
         Path to DATASET TO OVERALAY with whatever is plotted in Plot_vN method above
         */
        std::string Additional_Data_Path_3 = "/Users/patsfan753/Desktop/Fit_varsFina3_23/TypeD/vN_averaged_TypeD_MBplusCentral_noZDC_E_1.csv";
        Read_Additonal_DataSet_3(Additional_Data_Path_3, data_3);
        
        
        AdditionalData_4 data_4;
        /*
         Path to DATASET TO OVERALAY with whatever is plotted in Plot_vN method above
         */
        std::string Additional_Data_Path_4 = "/Users/patsfan753/Desktop/Fit_varsFina3_23/TypeE/vN_averaged_TypeE_MBplusCentral_noZDC_E_1.csv";
        Read_Additonal_DataSet_4(Additional_Data_Path_4, data_4);
        
        
        
        AdditionalData_5 data_5;
        /*
         Path to DATASET TO OVERALAY with whatever is plotted in Plot_vN method above
         */
        std::string Additional_Data_Path_5 = "/Users/patsfan753/Desktop/Fit_varsFina3_23/TypeF/vN_averaged_TypeF_MBplusCentral_noZDC_E_1.csv";
        Read_Additonal_DataSet_5(Additional_Data_Path_5, data_5);
        
        
        AdditionalData_6 data_6;
        /*
         Path to DATASET TO OVERALAY with whatever is plotted in Plot_vN method above
         */
        std::string Additional_Data_Path_6 = "/Users/patsfan753/Desktop/Fit_varsFina3_23/TypeG/vN_averaged_TypeG_MBplusCentral_noZDC_E_1.csv";
        Read_Additonal_DataSet_6(Additional_Data_Path_6, data_6);
        
        
        AdditionalData_7 data_7;
        /*
         Path to DATASET TO OVERALAY with whatever is plotted in Plot_vN method above
         */
        std::string Additional_Data_Path_7 = "/Users/patsfan753/Desktop/Fit_varsFina3_23/TypeH/vN_averaged_TypeH_MBplusCentral_noZDC_E_1.csv";
        Read_Additonal_DataSet_7(Additional_Data_Path_7, data_7);
        
        
        Plot_vN(data, data_2, data_3, data_4, data_5, data_6, data_7);
    }
    else {
        // After processing all sub-samples, calculate final corrected values
        std::vector<double> finalCorrectedV2(num_v2, 0.0);
        std::vector<double> finalCorrectedV3(num_v3, 0.0);

        std::vector<double> S2_v2(num_v2, 0.0);
        std::vector<double> S2_v3(num_v3, 0.0);

        std::vector<double> K_eff_2(num_v2, 0.0);
        std::vector<double> K_eff_3(num_v3, 0.0);

        std::vector<double> error_v2(num_v2, 0.0);
        std::vector<double> error_v3(num_v3, 0.0);

        // Calculate the final corrected values and their errors outside the sub-sample loop
        for (int idx = 0; idx < num_v2; ++idx) {
            if (totalWeights_2[idx] > 0) {
                finalCorrectedV2[idx] = totalWeightedV2Corrected[idx] / totalWeights_2[idx];

            }
        }
        
        for (int idx = 0; idx < num_v3; ++idx) {
            if (totalWeights_3[idx] > 0) {
                finalCorrectedV3[idx] = totalWeightedV3Corrected[idx] / totalWeights_3[idx];
            }
        }
        
        for (int idx = 0; idx < num_v2; ++idx) {
            if (totalWeights_2[idx] > 0) {
                // Calculate effective sample size for v2
                K_eff_2[idx] = (totalWeights_2[idx] * totalWeights_2[idx]) / sum_wk_2_squared[idx];
                

                S2_v2[idx] = ((sum_wk_xk_2[idx] - (2 * totalWeightedV2Corrected[idx]  * finalCorrectedV2[idx]) + (totalWeights_2[idx] * finalCorrectedV2[idx] * finalCorrectedV2[idx]))/ totalWeights_2[idx])* (K_eff_2[idx] / (K_eff_2[idx] - 1));

                // Calculate error for v2
                error_v2[idx] = sqrt(S2_v2[idx] / K_eff_2[idx]);
            }
        }

        for (int idx = 0; idx < num_v3; ++idx) {
            if (totalWeights_3[idx] > 0) {
                // Calculate effective sample size for v2
                K_eff_3[idx] = (totalWeights_3[idx] * totalWeights_3[idx]) / sum_wk_3_squared[idx];
                
                
                S2_v3[idx] = ((sum_wk_xk_3[idx] - (2 * totalWeightedV3Corrected[idx]  * finalCorrectedV3[idx]) + (totalWeights_3[idx] * finalCorrectedV3[idx] * finalCorrectedV3[idx]))/ totalWeights_3[idx])* (K_eff_3[idx] / (K_eff_3[idx] - 1));
                
                // Calculate error for v3
                error_v3[idx] = sqrt(S2_v3[idx] / K_eff_3[idx]);
            }
        }
            
        // Output to CSV
        std::ofstream outFile((baseProjectFolder + "TypeH/vN_averaged_TypeH_MBplusCentral_noZDC_E_1.csv").c_str());
        outFile << "Index,WeightedMeanV2Corrected,v2_error,WeightedMeanV3Corrected,v3_error\n";
        for (int idx = 0; idx < num_v2; ++idx) {
            outFile << idx << "," << finalCorrectedV2[idx] << "," << error_v2[idx] << "," << finalCorrectedV3[idx] << "," << error_v3[idx] << "\n";
        }
        outFile.close();


        
        std::cout << "\nSummary of Skipped Runs Due to Negative Means:\n";
        std::cout << "QQ2 Skipped Runs by Centrality:\n";
        PrintSkippedRunsSummary(skippedQQ2Counts);

        std::cout << "\nQQ3 Skipped Runs by Centrality:\n";
        PrintSkippedRunsSummary(skippedQQ3Counts);
        
        std::cout << std::left
                  << std::setw(15) << "SubSample"
                  << std::setw(15) << "v3_corrected"
                  << std::setw(15) << "w_k_3"
                  << std::setw(15) << "QQ3_mean"
                  << std::endl;
        std::cout << "-------------------------------------------------------------------" << std::endl;

        for (size_t i = 0; i < subsampleIndices.size(); ++i) {
            if (isQQ3MeanNegative[i]) {
                std::cout << "\033[31m"; // Start red text for negative QQ3_mean
            }
            std::cout << std::left
                      << std::setw(15) << subsampleIndices[i]
                      << std::setw(15) << v3CorrectedValues[i]
                      << std::setw(15) << wk3Values[i]
                      << std::setw(15) << QQ3MeanValues[i]
                      << "\033[0m" // Reset text color
                      << std::endl;
        }

    }
}
