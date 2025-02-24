// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <stdexcept> // For exception handling

// -- root includes --
#include <TH2F.h>
#include <TF1.h>
#include <TFile.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TCanvas.h>

// -- sPHENIX Style
#include <sPhenixStyle.C>

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
namespace fs = std::filesystem;

namespace myAnalysis {
    void analyze(const string &output);
    Int_t readCSV(const string &filename);
    pair<Int_t,Int_t> getDetectorCoordinates(Int_t sector, Int_t ib, Int_t channel);

    // Define the structure for your data
    struct MyData {
        Int_t sector;
        Int_t ib;
        Int_t channel;
        Float_t bias; // V
        Int_t offset; // mV
        Int_t iphi;
        Int_t ieta;
    };

    vector<MyData> data;

    size_t m_sample_print = 10;
    Int_t m_nsector_per_2pi = 32;
    Int_t m_nchannel_per_ib = 64;
    Int_t m_nib_per_sector = 6;
    Int_t m_neta = 96;
    Int_t m_nchannel_per_block = 4;
    Int_t m_nblock_per_ib = 16;
}

pair<Int_t, Int_t> myAnalysis::getDetectorCoordinates(Int_t sector, Int_t ib, Int_t channel) {
   Int_t ib_phi_low = (sector % m_nsector_per_2pi) * sqrt(m_nchannel_per_ib);
   Int_t ib_eta_low = (sector >= m_nsector_per_2pi) ? (m_nib_per_sector - 1 - ib) * sqrt(m_nchannel_per_ib) :
                                                       ib * sqrt(m_nchannel_per_ib) + m_neta / 2;

   Int_t nblock = channel / m_nchannel_per_block;
   Int_t nblock_phi_low = nblock / sqrt(m_nblock_per_ib) * sqrt(m_nchannel_per_block) + ib_phi_low;
   Int_t nblock_eta_low = (sqrt(m_nblock_per_ib) - 1 - (nblock % (Int_t)sqrt(m_nblock_per_ib))) * sqrt(m_nchannel_per_block) + ib_eta_low;

   Int_t iphi = (channel % m_nchannel_per_block) % (Int_t)sqrt(m_nchannel_per_block) + nblock_phi_low;
   Int_t ieta = (channel % m_nchannel_per_block) / sqrt(m_nchannel_per_block) + nblock_eta_low;

   return make_pair(iphi, ieta);
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
            // sector
            if (std::getline(ss, cell, ',')) {
                 try {
                    // sector number in CSV starts at 1 but we need to start at zero
                    row.sector = std::stoi(cell)-1;
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

            // channel
            if (std::getline(ss, cell, ',')) {
                 try {
                    row.channel = std::stoi(cell);
                } catch (const std::invalid_argument& e) {
                    throw std::runtime_error("Invalid integer in column3: " + cell);
                } catch (const std::out_of_range& e) {
                    throw std::runtime_error("Integer out of range in column3: " + cell);
                }
            } else {
                 throw std::runtime_error("Error parsing column3");
            }

            pair<Int_t,Int_t> iphi_ieta = getDetectorCoordinates(row.sector, row.ib, row.channel);

            row.iphi = iphi_ieta.first;
            row.ieta = iphi_ieta.second;

            // bias
            if (std::getline(ss, cell, ',')) {
                try {
                    row.bias = std::stof(cell);
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
            std::cerr << "Error processing line: " << line << ".  Error: " << e.what() << std::endl;
            return 1;
            // You could choose to continue processing or exit here.  For now, continuing.
        }
    }

    cout << "################" << endl;
    cout << "Rows Read: " << data.size() << endl;
    cout << "Sample Rows" << endl;
    cout << "sector,ib,channel,iphi,ieta,bias,offset" << endl;
    for (Int_t i = 0; i < min(data.size(), m_sample_print); ++i) {
        cout << data[i].sector << ","
             << data[i].ib << ","
             << data[i].channel << ","
             << data[i].iphi << ","
             << data[i].ieta << ","
             << data[i].bias << ","
             << data[i].offset << endl;
    }
    cout << "################" << endl;

    file.close();
    return 0;
}

void myAnalysis::analyze(const string &output) {
    string outputDir = fs::absolute(output).parent_path().string();
    fs::create_directories(outputDir);
}

void read_Bias(const string &input, const string &output="plots.pdf") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "output: " << output << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    if(myAnalysis::readCSV(input)) return;

    myAnalysis::analyze(output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 3){
        cout << "usage: ./read-Bias input [output]" << endl;
        cout << "input: input csv file" << endl;
        cout << "output: output pdf file" << endl;
        return 1;
    }

    string output  = "plots.pdf";

    if(argc >= 3) {
        output = argv[2];
    }

    read_Bias(argv[1], output);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
