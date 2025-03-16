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
#include <TFile.h>

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
    void analyze(const string &outputDir);
    Int_t readCSV(const string &filename);
    Int_t readMaps(const string &filename, unordered_map<Int_t,Int_t> &map, unordered_map<Int_t,Int_t> &rev_map);
    pair<Int_t,Int_t> getDetectorCoordinates(Int_t serial, Int_t ib, Int_t ib_channel, Bool_t verbose = false);
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

    // custom key for hashmap
    struct MyKey {
        Int_t serial;
        Int_t ib;
        Int_t channel;

        // Constructor
        MyKey(Int_t key_serial, Int_t key_ib, Int_t key_channel) : serial(key_serial), ib(key_ib), channel(key_channel) {}

        // Overload operator== for comparison
        bool operator==(const MyKey& other) const {
            return serial == other.serial && ib == other.ib && channel == other.channel;
        }
    };

    // Custom hash function (required for unordered_map)
    struct MyKeyHash {
        std::size_t operator()(const MyKey& k) const {
            // Combine hashes of individual members
            return std::hash<Int_t>()(k.serial) ^
                (std::hash<Int_t>()(k.ib) << 1) ^
                (std::hash<Int_t>()(k.channel) >> 1);
        }
    };

    // [serial sector, ib, ib_channel] -> [offset]
    unordered_map<MyKey, Int_t, MyKeyHash> myMap;

    vector<MyData> data;
    unordered_map<Int_t,Int_t> serial_to_sector;
    unordered_map<Int_t,Int_t> sector_to_serial;
    unordered_map<Int_t,Int_t> ib_channel_to_ADC_channel;
    unordered_map<Int_t,Int_t> ADC_channel_to_ib_channel;

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

    Int_t m_variations = 26;
    Int_t m_startBiasOffset = 500; /*mV*/
    Int_t m_offset_step = 100; /*mV*/

    // IB to vary the offsets
    // sector, ib
    vector<pair<Int_t,Int_t>> m_IB_vary = {make_pair(48,5),
                                           make_pair(49,5),
                                           make_pair(51,5)};
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

Int_t myAnalysis::readMaps(const string &filename, unordered_map<Int_t,Int_t> &map, unordered_map<Int_t,Int_t> &rev_map) {
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
            rev_map[value] = key;

        } catch (const std::runtime_error& e) {
            cout << "Error processing line: " << line << ".  Error: " << e.what() << std::endl;
            return 1;
            // You could choose to continue processing or exit here.  For now, continuing.
        }
    }

    cout << "################" << endl;
    cout << "Rows Read: " << map.size() << endl;
    cout << "Sample Rows" << endl;
    Int_t i = 0;
    for (const auto &p : map) {
        cout << "key: " << p.first
             << ", value: " << p.second
             << ", map: " << map[p.first]
             << ", rev: " << rev_map[p.second] << endl;
        if(++i == min(map.size(), m_sample_print)) break;
    }

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

            myMap[MyKey(row.serial,row.ib,row.ib_channel)] = row.offset;

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

void myAnalysis::analyze(const string &outputDir) {

    string out = outputDir + "/original";
    fs::create_directories(out);

    stringstream filename;

    // first write the original set of offsets
    // <= is used to include the serial sector 64
    for(UInt_t i = 1; i <= m_nsector; ++i) {
        for(UInt_t j = 0; j < m_nib_per_sector; ++j) {
            filename.str("");
            filename << out << "/sector" << i << "-" << j << ".dat";
            ofstream file(filename.str());
            if (!file.is_open()) {
                cout << "Error: Could not open file " << filename.str() << " for writing." << endl;
                return; // Exit if file cannot be opened
            }

            for(UInt_t k = 0; k < m_nchannel_per_ib; ++k) {
                file << myMap[MyKey(i,j,k)] << endl;
            }

            file.close();
        }
    }

    // write the different variations of bias offsets to file
    for(UInt_t v = 0; v < m_variations; ++v) {
        out = outputDir + "/var-" + to_string(v);
        fs::create_directories(out);

        cout << "Variation: " << v << ", output: " << fs::absolute(out).filename().string() << endl;

        Int_t offset = m_startBiasOffset - m_offset_step * v;

        // <= is used to include the serial sector 64
        for(UInt_t i = 1; i <= m_nsector; ++i) {
            for(UInt_t j = 0; j < m_nib_per_sector; ++j) {
                Bool_t isIB = false;
                for (const auto& [sector, ib] : m_IB_vary) {
                    if(i == sector_to_serial[sector] && ib == j) {
                      isIB = true;
                      // cout << "Found: sector: " << sector << ", serial: " << i << ", ib: " << ib << endl;
                    }
                }

                filename.str("");
                filename << out << "/sector" << i << "-" << j << ".dat";

                ofstream file(filename.str());
                if (!file.is_open()) {
                    cout << "Error: Could not open file " << filename.str() << " for writing." << endl;
                    return; // Exit if file cannot be opened
                }

                for(UInt_t k = 0; k < m_nchannel_per_ib; ++k) {
                    if(isIB) {
                        file << offset << endl;
                    }
                    else {
                        file << myMap[MyKey(i,j,k)]  << endl;
                    }
                }

                file.close();
            }
        }
    }
}

void write_Bias(const string &input,
                const string &input_sector,
                const string &input_channel,
                const string &outputDir=".",
                      Int_t   variations      = 26,
                      Int_t   startBiasOffset = 500, /*mV*/
                      Int_t   offset_step     = 100 /*mV*/) {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "input sector map: "  << input_sector << endl;
    cout << "input channel map: "  << input_channel << endl;
    cout << "outputDir: " << outputDir << endl;
    cout << "variations: " << variations << endl;
    cout << "start bias offset: " << startBiasOffset << endl;
    cout << "offset step: " << offset_step << endl;
    cout << "#############################" << endl;

    myAnalysis::m_variations = variations;
    myAnalysis::m_startBiasOffset = startBiasOffset;
    myAnalysis::m_offset_step = offset_step;

    fs::create_directories(outputDir);

    if(myAnalysis::readMaps(input_sector, myAnalysis::serial_to_sector, myAnalysis::sector_to_serial) ||
       myAnalysis::readMaps(input_channel, myAnalysis::ib_channel_to_ADC_channel, myAnalysis::ADC_channel_to_ib_channel) ||
       myAnalysis::readCSV(input)) return;

    myAnalysis::analyze(outputDir);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 4 || argc > 8){
        cout << "usage: ./write-Bias input input_sector input_channel [outputDir] [variations] [startBiasOffset] [offset_step]" << endl;
        cout << "input: input csv file" << endl;
        cout << "input_sector: input sector map" << endl;
        cout << "input_channel: input channel map" << endl;
        cout << "outputDir: output directory" << endl;
        cout << "variations: number of variations to run" << endl;
        cout << "startBiasOffset: start of the bias offset value" << endl;
        cout << "offset_step: increments of the bias offset variations" << endl;
        return 1;
    }

    string outputDir = ".";
    Int_t  variations      = 26;
    Int_t  startBiasOffset = 500; /*mV*/
    Int_t  offset_step     = 100; /*mV*/

    if(argc >= 5) {
        outputDir = argv[4];
    }
    if(argc >= 6) {
        variations = atoi(argv[5]);
    }
    if(argc >= 7) {
        startBiasOffset = atoi(argv[6]);
    }
    if(argc >= 8) {
        offset_step = atoi(argv[7]);
    }

    write_Bias(argv[1], argv[2], argv[3], outputDir, variations, startBiasOffset, offset_step);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
