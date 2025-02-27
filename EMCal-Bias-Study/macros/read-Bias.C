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
#include <TF1.h>
#include <TFile.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TCanvas.h>

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
    void analyze(const string &output);
    Int_t readCSV(const string &filename);
    Int_t readMaps(const string &filename, unordered_map<Int_t,Int_t> &map);
    pair<Int_t,Int_t> getDetectorCoordinates(Int_t serial, Int_t ib, Int_t ib_channel, Bool_t verbose = false);
    void setEMCalDim(TH2* hist);

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

    vector<MyData> data;
    unordered_map<Int_t,Int_t> serial_to_sector;
    unordered_map<Int_t,Int_t> ib_channel_to_ADC_channel;

    size_t m_sample_print = 10;
    Int_t m_nsector = 64;
    Int_t m_nchannel_per_sector = 384;
    Int_t m_nchannel_per_ib = 64;
    Int_t m_nphi = 256;
    Int_t m_neta = 96;

    Float_t min_bias = 9999;
    Float_t max_bias = -9999;

    Int_t min_offset = 9999;
    Int_t max_offset = -9999;

    unordered_map<string,TH2*> m_hists2D;
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
            std::cerr << "Error processing line: " << line << ".  Error: " << e.what() << std::endl;
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
            std::cerr << "Error processing line: " << line << ".  Error: " << e.what() << std::endl;
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

void myAnalysis::setEMCalDim(TH2* hist) {
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

void myAnalysis::analyze(const string &output) {
    string outputDir = fs::absolute(output).parent_path().string();
    fs::create_directories(outputDir);

    // hists
    m_hists2D["h2Bias"] = new TH2F("h2Bias","Bias [V]; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);
    m_hists2D["h2Offset"] = new TH2F("h2Offset","Offset [mV]; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);
    for(auto p : data) {
        m_hists2D["h2Bias"]->SetBinContent(p.iphi+1, p.ieta+1, p.bias);
        m_hists2D["h2Offset"]->SetBinContent(p.iphi+1, p.ieta+1, p.offset);
    }

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

    setEMCalDim(m_hists2D["h2Bias"]);
    m_hists2D["h2Bias"]->SetMinimum((Int_t)min_bias);

    m_hists2D["h2Bias"]->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists2D["h2Bias"]->GetName()) + ".png").c_str());

    setEMCalDim(m_hists2D["h2Offset"]);

    m_hists2D["h2Offset"]->Draw("COLZ1");
    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/" + string(m_hists2D["h2Offset"]->GetName()) + ".png").c_str());

    c1->Print((output + "]").c_str(), "pdf portrait");
}

void read_Bias(const string &input, const string &input_sector, const string &input_channel, const string &output="plots.pdf") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "input sector map: "  << input_sector << endl;
    cout << "input channel map: "  << input_channel << endl;
    cout << "output: " << output << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();


    if(myAnalysis::readMaps(input_sector, myAnalysis::serial_to_sector) ||
       myAnalysis::readMaps(input_channel, myAnalysis::ib_channel_to_ADC_channel) ||
       myAnalysis::readCSV(input)) return;

    myAnalysis::analyze(output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 4 || argc > 5){
        cout << "usage: ./read-Bias input input_sector input_channel [output]" << endl;
        cout << "input: input csv file" << endl;
        cout << "input_sector: input sector map" << endl;
        cout << "input_channel: input channel map" << endl;
        cout << "output: output pdf file" << endl;
        return 1;
    }

    string output  = "plots.pdf";

    if(argc >= 5) {
        output = argv[4];
    }

    read_Bias(argv[1], argv[2], argv[3], output);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
