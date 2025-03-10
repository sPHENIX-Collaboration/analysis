// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>
#include <stdexcept> // For exception handling

// -- root includes --
#include <TH2F.h>
#include <TH3F.h>
#include <TF1.h>
#include <TFile.h>
#include <TProfile.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TColor.h>

// -- sPHENIX Style
#include <sPhenixStyle.C>

// TowerInfo
#include <calobase/TowerInfoDefs.h>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::stringstream;
using std::min;
using std::max;
using std::ofstream;
using std::pair;
using std::make_pair;
using std::unordered_map;
namespace fs = std::filesystem;

namespace myAnalysis {
    void analyze(const string &output, const string &outputRoot, const string &input_calib);
    Int_t readCSV(const string &filename);
    Int_t readCSV_EMCal(const string& filename);
    Int_t readMaps(const string &filename, unordered_map<Int_t,Int_t> &map);
    pair<Int_t,Int_t> getDetectorCoordinates(Int_t serial, Int_t ib, Int_t ib_channel, Bool_t verbose = false);
    void setEMCalDim(TH1* hist);
    void writeCSV(const string &filename);

    // Define the structure for your data
    struct MyData {
        Int_t serial;
        Int_t sector;
        Int_t ib;
        Int_t ib_channel;
        Float_t bias; // V
        Int_t offset; // mV
        Int_t iphi;
        Int_t ieta;
    };

    struct MyDatav2 {
        Int_t channel;
        Int_t iphi;
        Int_t ieta;
        Float_t block_density; // g/cm^3
        Float_t cosmic_MPV;
        Float_t light_transmission;
        Float_t scintillation_ratio;
        string fiberType;
    };

    vector<MyData> data;
    vector<MyDatav2> datav2;
    unordered_map<Int_t,Int_t> serial_to_sector;
    unordered_map<Int_t,Int_t> ib_channel_to_ADC_channel;
    unordered_map<string, Int_t> m_fiberTypeMap = {
                                                 {"SG47",1},
                                                 {"K",2},
                                                 {"SG",3},
                                                 {"SG-B",4},
                                                 {"P-SG",5},
                                                 {"I-K",6},
                                                 {"PSG+IK+K",7}
    };

    size_t m_sample_print = 10;
    Int_t m_nsector = 64;
    Int_t m_nchannel_per_sector = 384;
    Int_t m_nchannel_per_ib = 64;
    Int_t m_nphi = 256;
    Int_t m_neta = 96;
    Int_t m_nib = 384;
    Int_t m_nib_per_sector = 6;

    Float_t min_bias = 9999;
    Float_t max_bias = -9999;

    Int_t min_offset = 9999;
    Int_t max_offset = -9999;

    Float_t min_block_density = 9999;
    Float_t max_block_density = -9999;

    Float_t min_cosmic_MPV = 9999;
    Float_t max_cosmic_MPV = -9999;

    Float_t min_light_transmission = 9999;
    Float_t max_light_transmission = -9999;

    Float_t min_scintillation_ratio = 9999;
    Float_t max_scintillation_ratio = -9999;

    Float_t min_calib = 9999;
    Float_t max_calib = -9999;

    Float_t min_vbDivDensity = 9999;
    Float_t max_vbDivDensity = -9999;

    Float_t min_vbDivCosmic = 9999;
    Float_t max_vbDivCosmic = -9999;

    Int_t m_bins_bias   = 300;
    Float_t m_bias_low  = 67;
    Float_t m_bias_high = 70;

    Int_t m_bins_offset   = 300;
    Float_t m_offset_low  = -2e3;
    Float_t m_offset_high = 1e3;

    Int_t m_bins_block_density = 250;
    Float_t m_block_density_low  = 8;
    Float_t m_block_density_high = 10.5;

    Int_t m_bins_cosmic_MPV = 220;
    Float_t m_cosmic_MPV_low  = -800;
    Float_t m_cosmic_MPV_high = 1400;

    Int_t m_bins_light_transmission = 140;
    Float_t m_light_transmission_low  = 80;
    Float_t m_light_transmission_high = 150;

    Int_t m_bins_scintillation_ratio = 120;
    Float_t m_scintillation_ratio_low  = 0;
    Float_t m_scintillation_ratio_high = 6;

    Int_t m_bins_calib = 100;
    Float_t m_calib_low  = 0;
    Float_t m_calib_high = 5;

    Int_t m_bins_vbDivDensity = 320;
    Float_t m_vbDivDensity_low  = -220;
    Float_t m_vbDivDensity_high = 100;

    Int_t m_bins_vbDivCosmic = 240;
    Float_t m_vbDivCosmic_low  = -8;
    Float_t m_vbDivCosmic_high = 16;

    unordered_map<string,TH1*> m_hists;
}

void myAnalysis::writeCSV(const string &filename) {
    ofstream file(filename);

    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << " for writing." << endl;
        return; // Exit if file cannot be opened
    }

    file << "sector_serial,sector,ib,ib_channel,iphi,ieta,bias,offset" << endl;

    for(const auto &p : data) {
       file << p.serial << ","
            << p.sector << ","
            << p.ib << ","
            << p.ib_channel << ","
            << p.iphi << ","
            << p.ieta << ","
            << p.bias << ","
            << p.offset << endl;
    }

    file.close();
    cout << "Data written to " << filename << " successfully." << endl;
}

pair<Int_t, Int_t> myAnalysis::getDetectorCoordinates(Int_t serial, Int_t ib, Int_t ib_channel, Bool_t verbose) {

    Int_t sector = serial_to_sector[serial];
    Int_t ADC_channel = ib_channel_to_ADC_channel[ib_channel];

    Int_t ADC_channel_offset = (sector >= 32) ? m_nchannel_per_sector*2*(sector-m_nsector/2)+m_nchannel_per_ib*ib
                                              : m_nchannel_per_sector*2*sector+m_nchannel_per_ib*ib+m_nchannel_per_sector;

    ADC_channel += ADC_channel_offset;

    UInt_t key = TowerInfoDefs::encode_emcal(ADC_channel);

    Int_t iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    Int_t ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

   if(verbose) {
    cout << "##############" << endl;
    cout << "serial: " << serial << ", ib: " << ib << ", ib_channel: " << ib_channel << endl;
    cout << "sector: " << sector << ", ib: " << ib << ", channel: " << ADC_channel << endl;
    cout << "iphi: " << iphi << ", ieta: " << ieta << endl;
   }

   return make_pair(iphi, ieta);
}

Int_t myAnalysis::readMaps(const string &filename, unordered_map<Int_t,Int_t> &map) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    string line;

    // Skip the header line if it exists (optional)
    if (std::getline(file, line)) {
        // You might want to process the header here if needed.
        // For example, you could check if the header is as expected.
    }

    while (std::getline(file, line)) {
        stringstream ss(line);
        string cell;
        Int_t key;
        Int_t value;

        // Extract the data from each cell, handling potential errors
        try {
            // key
            if (std::getline(ss, cell, ',')) {
                 try {
                    key = std::stoi(cell);
                } catch (const std::invalid_argument& e) {
                    throw std::runtime_error("Invalid integer in column1: " + cell);
                } catch (const std::out_of_range& e) {
                    throw std::runtime_error("Integer out of range in column1: " + cell);
                }
            } else {
                 throw std::runtime_error("Error parsing column1");
            }

            // value
            if (std::getline(ss, cell, ',')) {
                 try {
                    value = std::stoi(cell);
                } catch (const std::invalid_argument& e) {
                    throw std::runtime_error("Invalid integer in column2: " + cell);
                } catch (const std::out_of_range& e) {
                    throw std::runtime_error("Integer out of range in column2: " + cell);
                }
            } else {
                 throw std::runtime_error("Error parsing column2");
            }

            map[key] = value;

        } catch (const std::runtime_error& e) {
            cout << "Error processing line: " << line << ".  Error: " << e.what() << std::endl;
            return 1;
            // You could choose to continue processing or exit here.  For now, continuing.
        }
    }

    cout << "################" << endl;
    cout << "Rows Read: " << map.size() << endl;
    cout << "Sample Rows" << endl;
    for (Int_t i = 0; i < min(map.size(), m_sample_print); ++i) {
        cout << map[i] << endl;
    }

    file.close();
    return 0;
}

Int_t myAnalysis::readCSV_EMCal(const string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    string line;
    // Skip the header line if it exists (optional)
    if (std::getline(file, line)) {
        // You might want to process the header here if needed.
        // For example, you could check if the header is as expected.
    }

    while (std::getline(file, line)) {
        stringstream ss(line);
        string cell;
        MyDatav2 row;

        // Extract the data from each cell, handling potential errors
        try {
            // channel
            if (std::getline(ss, cell, ',')) {
                 try {
                    row.channel = std::stoi(cell);

                    UInt_t key = TowerInfoDefs::encode_emcal(row.channel);
                    row.iphi   = TowerInfoDefs::getCaloTowerPhiBin(key);
                    row.ieta   = TowerInfoDefs::getCaloTowerEtaBin(key);
                } catch (const std::invalid_argument& e) {
                    throw std::runtime_error("Invalid integer in column1: " + cell);
                } catch (const std::out_of_range& e) {
                    throw std::runtime_error("Integer out of range in column1: " + cell);
                }
            } else {
                 throw std::runtime_error("Error parsing channel");
            }

            // block density
            if (std::getline(ss, cell, ',')) {
                 try {
                    row.block_density = std::stof(cell);
                    min_block_density = min(min_block_density, row.block_density);
                    max_block_density = max(max_block_density, row.block_density);
                } catch (const std::invalid_argument& e) {
                    throw std::runtime_error("Invalid float in column2: " + cell);
                } catch (const std::out_of_range& e) {
                    throw std::runtime_error("Float out of range in column2: " + cell);
                }
            } else {
                 throw std::runtime_error("Error parsing block density");
            }

            // cosmic MPV
            if (std::getline(ss, cell, ',')) {
                 try {
                    row.cosmic_MPV = std::stof(cell);
                    min_cosmic_MPV = min(min_cosmic_MPV, row.cosmic_MPV);
                    max_cosmic_MPV = max(max_cosmic_MPV, row.cosmic_MPV);
                } catch (const std::invalid_argument& e) {
                    throw std::runtime_error("Invalid float in column3: " + cell);
                } catch (const std::out_of_range& e) {
                    throw std::runtime_error("Float out of range in column3: " + cell);
                }
            } else {
                 throw std::runtime_error("Error parsing cosmic MPV");
            }

            // light transmission
            if (std::getline(ss, cell, ',')) {
                try {
                    row.light_transmission = std::stof(cell);
                    min_light_transmission = min(min_light_transmission, row.light_transmission);
                    max_light_transmission = max(max_light_transmission, row.light_transmission);
                } catch (const std::invalid_argument& e) {
                    throw std::runtime_error("Invalid float in column3: " + cell);
                } catch (const std::out_of_range& e) {
                    throw std::runtime_error("Float out of range in column3: " + cell);
                }

            } else {
                throw std::runtime_error("Error parsing light transmission");
            }

            // scintillation ratio
            if (std::getline(ss, cell, ',')) {
                 try {
                    row.scintillation_ratio = std::stof(cell);
                    min_scintillation_ratio = min(min_scintillation_ratio, row.scintillation_ratio);
                    max_scintillation_ratio = max(max_scintillation_ratio, row.scintillation_ratio);
                } catch (const std::invalid_argument& e) {
                    throw std::runtime_error("Invalid float in column4: " + cell);
                } catch (const std::out_of_range& e) {
                    throw std::runtime_error("Float out of range in column4: " + cell);
                }
            } else {
                 throw std::runtime_error("Error parsing scintillation ratio");
            }

            // fiber type
            if (std::getline(ss, cell, ',')) {
                row.fiberType = cell;
                if(!m_fiberTypeMap.contains(row.fiberType)) {
                    throw std::runtime_error("Error! Unknown Fiber Type: " + row.fiberType);
                }
            }
            else {
                throw std::runtime_error("Error parsing fiber type");
            }

            datav2.push_back(row);
        } catch (const std::runtime_error& e) {
            cout << "Error processing line: " << line << ".  Error: " << e.what() << std::endl;
            return 1;
            // You could choose to continue processing or exit here.  For now, continuing.
        }
    }

    cout << "################" << endl;
    cout << "Rows Read: " << data.size() << endl;
    cout << "Sample Rows" << endl;
    cout << "channel,iphi,ieta,block_density,cosmic_MPV,light_transmission,scintillation_ratio,fiberType" << endl;
    for (Int_t i = 0; i < min(datav2.size(), m_sample_print); ++i) {
        cout << datav2[i].channel << ","
             << datav2[i].iphi << ","
             << datav2[i].ieta << ","
             << datav2[i].block_density << ","
             << datav2[i].cosmic_MPV << ","
             << datav2[i].light_transmission << ","
             << datav2[i].scintillation_ratio << ","
             << datav2[i].fiberType << endl;
    }
    cout << "################" << endl;
    cout << "Fiber Types" << endl;
    for (const auto& [key, value] : m_fiberTypeMap) {
        std::cout << "Fiber Type: " << key << ", Key: " << value << std::endl;
    }
    cout << "################" << endl;
    cout << "Block Density Min: " << min_block_density << ", Max: " << max_block_density << endl;
    cout << "Cosmic MPV Min: " << min_cosmic_MPV << ", Max: " << max_cosmic_MPV << endl;
    cout << "Light Transmission Min: " << min_light_transmission << ", Max: " << max_light_transmission << endl;
    cout << "Scintillation Ratio Min: " << min_scintillation_ratio << ", Max: " << max_scintillation_ratio << endl;

    file.close();
    return 0;
}

Int_t myAnalysis::readCSV(const string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    string line;
    // Skip the header line if it exists (optional)
    if (std::getline(file, line)) {
        // You might want to process the header here if needed.
        // For example, you could check if the header is as expected.
    }

    while (std::getline(file, line)) {
        stringstream ss(line);
        string cell;
        MyData row;

        // Extract the data from each cell, handling potential errors
        try {
            // serial
            if (std::getline(ss, cell, ',')) {
                 try {
                    row.serial = std::stoi(cell);
                    row.sector = serial_to_sector[row.serial];
                } catch (const std::invalid_argument& e) {
                    throw std::runtime_error("Invalid integer in column1: " + cell);
                } catch (const std::out_of_range& e) {
                    throw std::runtime_error("Integer out of range in column1: " + cell);
                }
            } else {
                 throw std::runtime_error("Error parsing column1");
            }

            // ib
            if (std::getline(ss, cell, ',')) {
                 try {
                    row.ib = std::stoi(cell);
                } catch (const std::invalid_argument& e) {
                    throw std::runtime_error("Invalid integer in column2: " + cell);
                } catch (const std::out_of_range& e) {
                    throw std::runtime_error("Integer out of range in column2: " + cell);
                }
            } else {
                 throw std::runtime_error("Error parsing column2");
            }

            // ib channel
            if (std::getline(ss, cell, ',')) {
                 try {
                    row.ib_channel = std::stoi(cell);
                } catch (const std::invalid_argument& e) {
                    throw std::runtime_error("Invalid integer in column3: " + cell);
                } catch (const std::out_of_range& e) {
                    throw std::runtime_error("Integer out of range in column3: " + cell);
                }
            } else {
                 throw std::runtime_error("Error parsing column3");
            }

            pair<Int_t,Int_t> iphi_ieta = getDetectorCoordinates(row.serial, row.ib, row.ib_channel);

            row.iphi = iphi_ieta.first;
            row.ieta = iphi_ieta.second;

            // bias
            if (std::getline(ss, cell, ',')) {
                try {
                    row.bias = std::stof(cell);
                    min_bias = min(min_bias, row.bias);
                    max_bias = max(max_bias, row.bias);
                } catch (const std::invalid_argument& e) {
                    throw std::runtime_error("Invalid float in column3: " + cell);
                } catch (const std::out_of_range& e) {
                    throw std::runtime_error("Float out of range in column3: " + cell);
                }

            } else {
                throw std::runtime_error("Error parsing column3");
            }

            // offset
            if (std::getline(ss, cell, ',')) {
                 try {
                    row.offset = std::stoi(cell);
                    min_offset = min(min_offset, row.offset);
                    max_offset = max(max_offset, row.offset);
                } catch (const std::invalid_argument& e) {
                    throw std::runtime_error("Invalid integer in column4: " + cell);
                } catch (const std::out_of_range& e) {
                    throw std::runtime_error("Integer out of range in column4: " + cell);
                }
            } else {
                 throw std::runtime_error("Error parsing column4");
            }

            data.push_back(row);

        } catch (const std::runtime_error& e) {
            cout << "Error processing line: " << line << ".  Error: " << e.what() << std::endl;
            return 1;
            // You could choose to continue processing or exit here.  For now, continuing.
        }
    }

    cout << "################" << endl;
    cout << "Rows Read: " << data.size() << endl;
    cout << "Sample Rows" << endl;
    cout << "sector,ib,ib_channel,iphi,ieta,bias,offset" << endl;
    for (Int_t i = 0; i < min(data.size(), m_sample_print); ++i) {
        cout << data[i].sector << ","
             << data[i].ib << ","
             << data[i].ib_channel << ","
             << data[i].iphi << ","
             << data[i].ieta << ","
             << data[i].bias << ","
             << data[i].offset << endl;
    }
    cout << "################" << endl;
    cout << "Bias Min: " << min_bias << ", Max: " << max_bias << endl;
    cout << "Offset Min: " << min_offset << ", Max: " << max_offset << endl;

    file.close();
    return 0;
}

void myAnalysis::setEMCalDim(TH1* hist) {
    hist->GetXaxis()->SetLimits(0,256);
    hist->GetXaxis()->SetNdivisions(32, false);
    hist->GetXaxis()->SetLabelSize(0.04);
    hist->GetXaxis()->SetTickSize(0.01);
    hist->GetYaxis()->SetTickSize(0.01);
    hist->GetYaxis()->SetLabelSize(0.04);
    hist->GetYaxis()->SetLimits(0,96);
    hist->GetYaxis()->SetNdivisions(12, false);
    hist->GetYaxis()->SetTitleOffset(0.5);
    hist->GetXaxis()->SetTitleOffset(1);
}

void myAnalysis::analyze(const string &output, const string &outputRoot, const string &input_calib) {
    string outputDir = fs::absolute(output).parent_path().string();
    fs::create_directories(outputDir);

    // open calib file
    // Open the ROOT file
    TFile* file = TFile::Open(input_calib.c_str());

    if (!file || file->IsZombie()) {
        cout << "Error: Could not open ROOT file." << endl;
        return; // Indicate an error
    }

    m_hists["h2_mevOverADC"] = (TH2F*)file->Get("h2_mevOverADC");

    if (!m_hists["h2_mevOverADC"]) {
        cout << "Error: Histogram not found in ROOT file." << endl;
        file->Close();
        return; // Indicate an error
    }

    // hists
    m_hists["h2Bias"] = new TH2F("h2Bias","Bias [V]; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);
    m_hists["h2Offset"] = new TH2F("h2Offset","Offset [mV]; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);
    m_hists["h2BlockDensity"] = new TH2F("h2BlockDensity","Block Density [g/cm^{3}]; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);
    m_hists["h2CosmicMPV"] = new TH2F("h2CosmicMPV","Cosmic MPV; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);
    m_hists["h2Light"] = new TH2F("h2Light","Light Transmission [%]; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);
    m_hists["h2ScintRatio"] = new TH2F("h2ScintRatio","Scintillation Ratio; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);
    m_hists["h2Calib"] = new TH2F("h2Calib","EMCal Calibration [MeV/ADC]; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);
    m_hists["hBias"] = new TH1F("hBias","Tower; Bias [V]; Counts", m_bins_bias, m_bias_low, m_bias_high);
    m_hists["hOffset"] = new TH1F("hOffset","Tower; Offset [mV]; Counts", m_bins_offset, m_offset_low, m_offset_high);
    m_hists["hBlockDensity"] = new TH1F("hBlockDensity","Tower; Block Density [g/cm^{3}]; Counts", m_bins_block_density, m_block_density_low, m_block_density_high);
    m_hists["hCosmicMPV"] = new TH1F("hCosmicMPV","Tower; Cosmic MPV; Counts", m_bins_cosmic_MPV, m_cosmic_MPV_low, m_cosmic_MPV_high);
    m_hists["hLight"] = new TH1F("hLight","Tower; Light Transmission [%]; Counts", m_bins_light_transmission, m_light_transmission_low, m_light_transmission_high);
    m_hists["hScintRatio"] = new TH1F("hScintRatio","Tower; Scintillation Ratio; Counts", m_bins_scintillation_ratio, m_scintillation_ratio_low, m_scintillation_ratio_high);
    m_hists["h3CalibOffsetBlockDensity"] = new TH3F("h3CalibOffsetBlockDensity","EMCal; Block Density [g/cm^{3}]; Offset [mV]; EMCal Calibration [MeV/ADC]", m_bins_block_density, m_block_density_low, m_block_density_high, m_bins_offset, m_offset_low, m_offset_high, m_bins_calib, m_calib_low, m_calib_high);
    m_hists["h3CalibOffsetCosmicMPV"] = new TH3F("h3CalibOffsetCosmicMPV","EMCal; Cosmic MPV; Offset [mV]; EMCal Calibration [MeV/ADC]", m_bins_cosmic_MPV, m_cosmic_MPV_low, m_cosmic_MPV_high, m_bins_offset, m_offset_low, m_offset_high, m_bins_calib, m_calib_low, m_calib_high);
    m_hists["h2VbDivDensityCalib"] = new TH2F("h2VbDivDensityCalib","EMCal; EMCal Calibration [MeV/ADC]; Offset/Block Density [mV*cm^{3}/g]", m_bins_calib, m_calib_low, m_calib_high, m_bins_vbDivDensity, m_vbDivDensity_low, m_vbDivDensity_high);
    m_hists["h2VbDivCosmicCalib"] = new TH2F("h2VbDivCosmicCalib","EMCal; EMCal Calibration [MeV/ADC]; Offset/Cosmic MPV [mV]", m_bins_calib, m_calib_low, m_calib_high, m_bins_vbDivCosmic, m_vbDivCosmic_low, m_vbDivCosmic_high);
    m_hists["h2FiberType"] = new TH2F("h2FiberType","Fiber Type; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);

    // dummy hists for labeling
    m_hists["h2DummySector"] = new TH2F("h2DummySector","", m_nsector/2, 0, m_nsector/2, 2, 0, 2);
    m_hists["h2DummyIB"] = new TH2F("h2DummyIB","", m_nsector/2, 0, m_nsector/2, m_nib*2/m_nsector, 0, m_nib*2/m_nsector);

    for(auto p : data) {
        m_hists["h2Bias"]->SetBinContent(p.iphi+1, p.ieta+1, p.bias);
        m_hists["hBias"]->Fill(p.bias);
        m_hists["h2Offset"]->SetBinContent(p.iphi+1, p.ieta+1, p.offset);
        m_hists["hOffset"]->Fill(p.offset);
    }

    for(auto p : datav2) {
        m_hists["h2BlockDensity"]->SetBinContent(p.iphi+1, p.ieta+1, p.block_density);
        m_hists["h2CosmicMPV"]->SetBinContent(p.iphi+1, p.ieta+1, p.cosmic_MPV);
        m_hists["h2Light"]->SetBinContent(p.iphi+1, p.ieta+1, p.light_transmission);
        m_hists["h2ScintRatio"]->SetBinContent(p.iphi+1, p.ieta+1, p.scintillation_ratio);
        m_hists["h2FiberType"]->SetBinContent(p.iphi+1, p.ieta+1, m_fiberTypeMap[p.fiberType]);

        m_hists["hBlockDensity"]->Fill(p.block_density);
        m_hists["hCosmicMPV"]->Fill(p.cosmic_MPV);
        m_hists["hLight"]->Fill(p.light_transmission);
        m_hists["hScintRatio"]->Fill(p.scintillation_ratio);
    }

    for(UInt_t i = 1; i <= m_nphi; ++i) {
        for(UInt_t j = 1; j <= m_neta; ++j) {
            Float_t calib = m_hists["h2_mevOverADC"]->GetBinContent(j,i);
            m_hists["h2Calib"]->SetBinContent(i,j,calib);
            min_calib = min(min_calib, calib);
            max_calib = max(max_calib, calib);

            Float_t bd = m_hists["h2BlockDensity"]->GetBinContent(i,j);
            Float_t offset = m_hists["h2Offset"]->GetBinContent(i,j);
            Float_t cosmic = m_hists["h2CosmicMPV"]->GetBinContent(i,j);

            Float_t vbDivDensity = (bd) ? offset/bd : 0;
            min_vbDivDensity = min(min_vbDivDensity, vbDivDensity);
            max_vbDivDensity = max(max_vbDivDensity, vbDivDensity);

            Float_t vbDivCosmic = (cosmic) ? offset/cosmic : 0;
            min_vbDivCosmic = min(min_vbDivCosmic, vbDivCosmic);
            max_vbDivCosmic = max(max_vbDivCosmic, vbDivCosmic);

            if(calib) ((TH2*)m_hists["h2VbDivDensityCalib"])->Fill(calib,vbDivDensity);
            if(cosmic && calib) ((TH2*)m_hists["h2VbDivCosmicCalib"])->Fill(calib,vbDivCosmic);

            if(calib) ((TH3*)m_hists["h3CalibOffsetBlockDensity"])->Fill(bd,offset,calib);
            if(cosmic && calib) ((TH3*)m_hists["h3CalibOffsetCosmicMPV"])->Fill(cosmic,offset,calib);
        }
    }

    cout << "Calib Min: " << min_calib << ", Max: " << max_calib << endl;
    cout << "vbDivDensity Min: " << min_vbDivDensity << ", Max: " << max_vbDivDensity << endl;
    cout << "vbDivCosmic Min: " << min_vbDivCosmic << ", Max: " << max_vbDivCosmic << endl;

    // save plots to root file
    TFile tf(outputRoot.c_str(),"recreate");
    tf.cd();

    m_hists["h2BlockDensity"]->Write();
    m_hists["h2CosmicMPV"]->Write();
    m_hists["h2Light"]->Write();
    m_hists["h2ScintRatio"]->Write();
    m_hists["h2Calib"]->Write();
    m_hists["h3CalibOffsetBlockDensity"]->Write();
    m_hists["h3CalibOffsetCosmicMPV"]->Write();
    m_hists["h2VbDivDensityCalib"]->Write();
    m_hists["h2VbDivCosmicCalib"]->Write();
    m_hists["h2FiberType"]->Write();

    m_hists["hBlockDensity"]->Write();
    m_hists["hCosmicMPV"]->Write();
    m_hists["hLight"]->Write();
    m_hists["hScintRatio"]->Write();

    tf.Close();

    for(UInt_t i = 0; i < m_nsector; ++i) {
        Int_t x = i % (m_nsector / 2) + 1;
        Int_t y = (i < (m_nsector / 2)) ? 2 : 1;
        m_hists["h2DummySector"]->SetBinContent(x,y,i);
    }

    for(UInt_t i = 0; i < m_nib; ++i) {
        Int_t val = i % m_nib_per_sector;
        Int_t x = (i / m_nib_per_sector) % (m_nsector / 2) + 1;
        Int_t y = (i < (m_nib / 2)) ? m_nib_per_sector - val : m_nib_per_sector + val + 1;
        m_hists["h2DummyIB"]->SetBinContent(x,y,val);
    }

    setEMCalDim(m_hists["h2Bias"]);
    setEMCalDim(m_hists["h2Offset"]);
    setEMCalDim(m_hists["h2BlockDensity"]);
    setEMCalDim(m_hists["h2CosmicMPV"]);
    setEMCalDim(m_hists["h2Light"]);
    setEMCalDim(m_hists["h2ScintRatio"]);
    setEMCalDim(m_hists["h2Calib"]);
    setEMCalDim(m_hists["h2FiberType"]);

    setEMCalDim(m_hists["h2DummySector"]);
    setEMCalDim(m_hists["h2DummyIB"]);

    m_hists["h2DummySector"]->SetMarkerSize(1.8);

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06);
    c1->SetRightMargin(.12);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    gStyle->SetOptTitle(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleFontSize(0.08);
    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.08);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(0);

    gPad->SetGrid();

    c1->Print((output + "[").c_str(), "pdf portrait");

    m_hists["h2Bias"]->SetMinimum((Int_t)min_bias);

    m_hists["h2Bias"]->Draw("COLZ");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2Bias"]->GetName()) + ".png").c_str());

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2Bias"]->GetName()) + "-labeled.png").c_str());

    // ---------------------------

    m_hists["h2Offset"]->SetMinimum(-2000);

    m_hists["h2Offset"]->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2Offset"]->GetName()) + ".png").c_str());

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2Offset"]->GetName()) + "-labeled.png").c_str());

    // ---------------------------

    m_hists["h2BlockDensity"]->SetMinimum(8.4);
    m_hists["h2BlockDensity"]->SetMaximum(10.2);

    m_hists["h2BlockDensity"]->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2BlockDensity"]->GetName()) + ".png").c_str());

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2BlockDensity"]->GetName()) + "-labeled.png").c_str());

    // ---------------------------------

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06);
    c1->SetRightMargin(.02);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    // https://github.com/mpetroff/accessible-color-cycles?tab=readme-ov-file#final-results
    vector<string> myColors = {"#ffffff", "#5790fc", "#f89c20", "#e42536", "#33d433", "#ff00b3", "#7a21dd"};

    Int_t palette[7];
    for(UInt_t i = 0; i < myColors.size(); ++i) {
       palette[i] = TColor::GetColor(myColors[i].c_str());
    }

    gStyle->SetPalette(myColors.size(),palette);

    m_hists["h2FiberType"]->Draw("COL");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2FiberType"]->GetName()) + ".png").c_str());

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2FiberType"]->GetName()) + "-labeled.png").c_str());

    // ---------------------------------

    gStyle->SetPalette(kBird);
    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06);
    c1->SetRightMargin(.12);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    m_hists["h2CosmicMPV"]->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2CosmicMPV"]->GetName()) + ".png").c_str());

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2CosmicMPV"]->GetName()) + "-labeled.png").c_str());

    m_hists["h2CosmicMPV"]->SetMinimum(0);
    m_hists["h2CosmicMPV"]->SetMaximum(800);

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2CosmicMPV"]->GetName()) + "-labeled-zoom.png").c_str());

    // ---------------------------------

    m_hists["h2Light"]->SetMinimum(90);
    m_hists["h2Light"]->SetMaximum(110);

    m_hists["h2Light"]->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2Light"]->GetName()) + ".png").c_str());

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2Light"]->GetName()) + "-labeled.png").c_str());

    // ---------------------------------

    m_hists["h2ScintRatio"]->SetMaximum(3);

    m_hists["h2ScintRatio"]->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2ScintRatio"]->GetName()) + ".png").c_str());

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2ScintRatio"]->GetName()) + "-labeled.png").c_str());

    // ---------------------------------

    m_hists["h2Calib"]->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2Calib"]->GetName()) + ".png").c_str());

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2Calib"]->GetName()) + "-labeled.png").c_str());

    m_hists["h2Calib"]->SetMinimum(0.5);
    m_hists["h2Calib"]->SetMaximum(3);

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2Calib"]->GetName()) + "-labeled-zoom.png").c_str());

    // ---------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.16);
    c1->SetRightMargin(.05);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    m_hists["hBias"]->Rebin(10);
    m_hists["hBias"]->Draw();
    m_hists["hBias"]->GetXaxis()->SetTitleOffset(1);
    m_hists["hBias"]->GetYaxis()->SetRangeUser(0,4e3);
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["hBias"]->GetName()) + ".png").c_str());

    m_hists["hOffset"]->Rebin(10);
    m_hists["hOffset"]->Draw();
    m_hists["hOffset"]->GetYaxis()->SetRangeUser(0,4e3);
    m_hists["hOffset"]->GetXaxis()->SetTitleOffset(1);
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["hOffset"]->GetName()) + ".png").c_str());

    gPad->SetLogy();

    m_hists["hBlockDensity"]->Rebin(5);
    m_hists["hBlockDensity"]->Draw();
    m_hists["hBlockDensity"]->GetXaxis()->SetTitleOffset(1);
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["hBlockDensity"]->GetName()) + ".png").c_str());

    m_hists["hCosmicMPV"]->Draw();
    m_hists["hCosmicMPV"]->GetXaxis()->SetTitleOffset(1);
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["hCosmicMPV"]->GetName()) + ".png").c_str());

    m_hists["hLight"]->Draw();
    m_hists["hLight"]->GetXaxis()->SetTitleOffset(1);
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["hLight"]->GetName()) + ".png").c_str());

    m_hists["hScintRatio"]->Draw();
    m_hists["hScintRatio"]->GetXaxis()->SetTitleOffset(1);
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["hScintRatio"]->GetName()) + ".png").c_str());

    m_hists["hCalib"] = ((TH3*)m_hists["h3CalibOffsetBlockDensity"])->Project3D("z");
    m_hists["hCalib"]->SetTitle("EMCal");
    m_hists["hCalib"]->GetYaxis()->SetTitle("Counts");
    m_hists["hCalib"]->Draw();
    m_hists["hCalib"]->GetXaxis()->SetTitleOffset(1);
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["hCalib"]->GetName()) + ".png").c_str());

    // ---------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.16);
    c1->SetRightMargin(.12);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    gPad->SetLogy(0);
    gPad->SetGrid(0,0);

    m_hists["h2CalibVsBlockDensity"] = ((TH3*)m_hists["h3CalibOffsetBlockDensity"])->Project3D("zx");

    m_hists["h2CalibVsBlockDensity"]->SetTitle("EMCal");
    m_hists["h2CalibVsBlockDensity"]->Rebin(5);
    m_hists["h2CalibVsBlockDensity"]->Draw("COLZ1");
    m_hists["h2CalibVsBlockDensity"]->GetXaxis()->SetTitleOffset(1);

    TH1* h2CalibVsBlockDensity_px = ((TH2*)m_hists["h2CalibVsBlockDensity"])->ProfileX();
    h2CalibVsBlockDensity_px->SetLineColor(kRed);
    h2CalibVsBlockDensity_px->SetMarkerColor(kRed);
    h2CalibVsBlockDensity_px->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2CalibVsBlockDensity"]->GetName()) + ".png").c_str());

    // ---------------------------------

    m_hists["h2CalibVsCosmicMPV"] = ((TH3*)m_hists["h3CalibOffsetCosmicMPV"])->Project3D("zx");

    m_hists["h2CalibVsCosmicMPV"]->SetTitle("EMCal");
    // m_hists["h2CalibVsCosmicMPV"]->Rebin(5);
    m_hists["h2CalibVsCosmicMPV"]->Draw("COLZ1");
    m_hists["h2CalibVsCosmicMPV"]->GetXaxis()->SetTitleOffset(1);

    TH1* h2CalibVsCosmicMPV_px = ((TH2*)m_hists["h2CalibVsCosmicMPV"])->ProfileX();
    h2CalibVsCosmicMPV_px->SetLineColor(kRed);
    h2CalibVsCosmicMPV_px->SetMarkerColor(kRed);
    h2CalibVsCosmicMPV_px->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2CalibVsCosmicMPV"]->GetName()) + ".png").c_str());


    m_hists["h2CalibVsCosmicMPV"]->GetXaxis()->SetRangeUser(0,800);
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2CalibVsCosmicMPV"]->GetName()) + "-zoom.png").c_str());

    // ---------------------------------

    m_hists["h2CalibVsOffset"] = ((TH3*)m_hists["h3CalibOffsetBlockDensity"])->Project3D("zy");

    m_hists["h2CalibVsOffset"]->SetTitle("EMCal");
    m_hists["h2CalibVsOffset"]->Rebin(10);
    m_hists["h2CalibVsOffset"]->Draw("COLZ1");
    m_hists["h2CalibVsOffset"]->GetXaxis()->SetTitleOffset(1);

    TH1* h2CalibVsOffset_px = ((TH2*)m_hists["h2CalibVsOffset"])->ProfileX();
    h2CalibVsOffset_px->SetLineColor(kRed);
    h2CalibVsOffset_px->SetMarkerColor(kRed);
    h2CalibVsOffset_px->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2CalibVsOffset"]->GetName()) + ".png").c_str());

    // ---------------------------------

    m_hists["h2BlockDensityVsOffset"] = ((TH3*)m_hists["h3CalibOffsetBlockDensity"])->Project3D("xy");

    m_hists["h2BlockDensityVsOffset"]->SetTitle("EMCal");
    m_hists["h2BlockDensityVsOffset"]->Rebin(10);
    ((TH2*)m_hists["h2BlockDensityVsOffset"])->RebinY(5);
    m_hists["h2BlockDensityVsOffset"]->Draw("COLZ1");
    m_hists["h2BlockDensityVsOffset"]->GetXaxis()->SetTitleOffset(1);

    TH1* h2BlockDensityVsOffset_px = ((TH2*)m_hists["h2BlockDensityVsOffset"])->ProfileX();
    h2BlockDensityVsOffset_px->SetLineColor(kRed);
    h2BlockDensityVsOffset_px->SetMarkerColor(kRed);
    // h2BlockDensityVsOffset_px->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2BlockDensityVsOffset"]->GetName()) + ".png").c_str());

    // ---------------------------------

    m_hists["h2CosmicMPVOffset"] = ((TH3*)m_hists["h3CalibOffsetCosmicMPV"])->Project3D("xy");

    m_hists["h2CosmicMPVOffset"]->SetTitle("EMCal");
    m_hists["h2CosmicMPVOffset"]->Rebin(10);
    // ((TH2*)m_hists["h2CosmicMPVOffset"])->RebinY(5);
    m_hists["h2CosmicMPVOffset"]->Draw("COLZ1");
    m_hists["h2CosmicMPVOffset"]->GetXaxis()->SetTitleOffset(1);

    TH1* h2CosmicMPVOffset_px = ((TH2*)m_hists["h2CosmicMPVOffset"])->ProfileX();
    h2CosmicMPVOffset_px->SetLineColor(kRed);
    h2CosmicMPVOffset_px->SetMarkerColor(kRed);
    // h2CosmicMPVOffset_px->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2CosmicMPVOffset"]->GetName()) + ".png").c_str());

    m_hists["h2CosmicMPVOffset"]->GetYaxis()->SetRangeUser(0,800);
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2CosmicMPVOffset"]->GetName()) + "-zoom.png").c_str());

    // ---------------------------------

    ((TH2*)m_hists["h2VbDivDensityCalib"])->RebinY(5);
    m_hists["h2VbDivDensityCalib"]->Draw("COLZ1");
    m_hists["h2VbDivDensityCalib"]->GetXaxis()->SetTitleOffset(1);

    TH1* h2VbDivDensityCalib_px = ((TH2*)m_hists["h2VbDivDensityCalib"])->ProfileX();
    h2VbDivDensityCalib_px->SetLineColor(kRed);
    h2VbDivDensityCalib_px->SetMarkerColor(kRed);
    h2VbDivDensityCalib_px->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2VbDivDensityCalib"]->GetName()) + ".png").c_str());

    // ---------------------------------

    ((TH2*)m_hists["h2VbDivCosmicCalib"])->RebinY(5);
    m_hists["h2VbDivCosmicCalib"]->Draw("COLZ1");
    m_hists["h2VbDivCosmicCalib"]->GetXaxis()->SetTitleOffset(1);

    TH1* h2VbDivCosmicCalib_px = ((TH2*)m_hists["h2VbDivCosmicCalib"])->ProfileX();
    h2VbDivCosmicCalib_px->SetLineColor(kRed);
    h2VbDivCosmicCalib_px->SetMarkerColor(kRed);
    h2VbDivCosmicCalib_px->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2VbDivCosmicCalib"]->GetName()) + ".png").c_str());

    m_hists["h2VbDivCosmicCalib"]->GetYaxis()->SetRangeUser(-8,4);

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists["h2VbDivCosmicCalib"]->GetName()) + "-zoom.png").c_str());

    // ---------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.16);
    c1->SetRightMargin(.05);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    Int_t biny_start = m_hists["h2VbDivDensityCalib"]->GetXaxis()->FindBin(1.5);
    Int_t biny_end   = m_hists["h2VbDivDensityCalib"]->GetXaxis()->FindBin(2);
    TH1* h2VbDivDensityCalibA_py = ((TH2*)m_hists["h2VbDivDensityCalib"])->ProjectionY("h2VbDivDensityCalib_py", biny_start, biny_end-1);
    TH1* h2VbDivDensityCalibB_py = ((TH2*)m_hists["h2VbDivDensityCalib"])->ProjectionY("h2VbDivDensityCalib_0-1.5_py", 1, biny_start-1);

    h2VbDivDensityCalibA_py->Rebin(2);
    h2VbDivDensityCalibB_py->Rebin(2);

    h2VbDivDensityCalibB_py->GetYaxis()->SetTitle("Counts");
    h2VbDivDensityCalibB_py->GetYaxis()->SetRangeUser(0,3000);
    h2VbDivDensityCalibB_py->GetXaxis()->SetTitleOffset(1);
    h2VbDivDensityCalibB_py->SetLineColor(kRed);

    h2VbDivDensityCalibB_py->Draw();
    h2VbDivDensityCalibA_py->Draw("same");

    Float_t meanA = (Int_t)(h2VbDivDensityCalibA_py->GetMean()*100)/100.;
    Float_t meanB = (Int_t)(h2VbDivDensityCalibB_py->GetMean()*100)/100.;

    stringstream legA, legB;

    legB << "0 < Calibration < 1.5, #mu = " << meanB << " [mV*cm^{3}/g]";
    legA << "1.5 #leq Calibration < 2, #mu = " << meanA << " [mV*cm^{3}/g]";

    auto leg = new TLegend(0.2,.75,0.65,.85);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(h2VbDivDensityCalibA_py,legA.str().c_str(),"f");
    leg->AddEntry(h2VbDivDensityCalibB_py,legB.str().c_str(),"f");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2VbDivDensityCalibA_py->GetName()) + ".png").c_str());

    // ---------------------------------

    biny_start = m_hists["h2VbDivCosmicCalib"]->GetXaxis()->FindBin(1.5);
    biny_end   = m_hists["h2VbDivCosmicCalib"]->GetXaxis()->FindBin(2);
    TH1* h2VbDivCosmicCalibA_py = ((TH2*)m_hists["h2VbDivCosmicCalib"])->ProjectionY("h2VbDivCosmicCalib_py", biny_start, biny_end-1);
    TH1* h2VbDivCosmicCalibB_py = ((TH2*)m_hists["h2VbDivCosmicCalib"])->ProjectionY("h2VbDivCosmicCalib_0-1.5_py", 1, biny_start-1);

    h2VbDivCosmicCalibB_py->GetYaxis()->SetTitle("Counts");
    h2VbDivCosmicCalibB_py->GetYaxis()->SetRangeUser(0,3000);
    h2VbDivCosmicCalibB_py->GetXaxis()->SetTitleOffset(1);
    h2VbDivCosmicCalibB_py->SetLineColor(kRed);

    h2VbDivCosmicCalibB_py->Draw();
    h2VbDivCosmicCalibA_py->Draw("same");

    meanA = (Int_t)(h2VbDivCosmicCalibA_py->GetMean()*100)/100.;
    meanB = (Int_t)(h2VbDivCosmicCalibB_py->GetMean()*100)/100.;

    legA.str("");
    legB.str("");

    legB << "0 < Calibration < 1.5, #mu = " << meanB << " [mV]";
    legA << "1.5 #leq Calibration < 2, #mu = " << meanA << " [mV]";

    leg = new TLegend(0.17,.7,0.31,.8);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(h2VbDivCosmicCalibA_py,legA.str().c_str(),"f");
    leg->AddEntry(h2VbDivCosmicCalibB_py,legB.str().c_str(),"f");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(h2VbDivCosmicCalibA_py->GetName()) + ".png").c_str());

    c1->Print((output + "]").c_str(), "pdf portrait");

    file->Close();
}

void read_Bias(const string &input,
               const string &input_sector,
               const string &input_channel,
               const string &input_blockInfo,
               const string &input_calib,
               const string &output="plots.pdf",
               const string &outputCSV="vop.csv",
               const string &outputRoot="test.root") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "input sector map: "  << input_sector << endl;
    cout << "input channel map: "  << input_channel << endl;
    cout << "input block info: "  << input_blockInfo << endl;
    cout << "input calib: "  << input_calib << endl;
    cout << "output: " << output << endl;
    cout << "output CSV: " << outputCSV << endl;
    cout << "output root file: " << outputRoot << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    string outputCSVDir = fs::absolute(outputCSV).parent_path().string();
    string outputRootDir = fs::absolute(outputRoot).parent_path().string();

    fs::create_directories(outputCSVDir);
    fs::create_directories(outputRootDir);

    if(myAnalysis::readMaps(input_sector, myAnalysis::serial_to_sector) ||
       myAnalysis::readMaps(input_channel, myAnalysis::ib_channel_to_ADC_channel) ||
       myAnalysis::readCSV(input) ||
       myAnalysis::readCSV_EMCal(input_blockInfo)) return;

    myAnalysis::analyze(output, outputRoot, input_calib);
    myAnalysis::writeCSV(outputCSV);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 6 || argc > 9){
        cout << "usage: ./read-Bias input input_sector input_channel input_blockInfo input_calib [output] [outputCSV] [outputRoot]" << endl;
        cout << "input: input csv file" << endl;
        cout << "input_sector: input sector map" << endl;
        cout << "input_channel: input channel map" << endl;
        cout << "input_blockInfo: input EMCal block info" << endl;
        cout << "input_calib: input EMCal block info" << endl;
        cout << "output: output pdf file" << endl;
        cout << "output CSV: output csv file" << endl;
        cout << "output root: output root file" << endl;
        return 1;
    }

    string output    = "plots.pdf";
    string outputCSV = "vop.csv";
    string outputRoot = "test.root";

    if(argc >= 7) {
        output = argv[6];
    }
    if(argc >= 8) {
        outputCSV = argv[7];
    }
    if(argc >= 9) {
        outputRoot = argv[8];
    }

    read_Bias(argv[1], argv[2], argv[3], argv[4], argv[5], output, outputCSV, outputRoot);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
