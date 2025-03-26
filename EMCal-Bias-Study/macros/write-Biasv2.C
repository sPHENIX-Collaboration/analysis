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
#include <TH1.h>

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
    Int_t readHist(const string &filename, const string &histName);
    Int_t readMaps(const string &filename, unordered_map<Int_t,Int_t> &map, unordered_map<Int_t,Int_t> &rev_map);
    pair<Int_t,Int_t> getDetectorCoordinates(Int_t serial, Int_t ib, Int_t ib_channel, Bool_t verbose = false);

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

    Int_t m_min_offset = 9999;
    Int_t m_max_offset = -9999;

    TH1* m_hist;
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

Int_t myAnalysis::readHist(const string& filename, const string &histName) {

    TFile* tfile = TFile::Open(filename.c_str());
    if (!tfile || tfile->IsZombie()) {
        cout << "Error: Could not open ROOT file: " << filename << endl;
        return 1;
    }

    m_hist = (TH1*) tfile->Get(histName.c_str());
    if (!m_hist) {
      cout << "Error: Histogram '" << histName << "' not found in file: " << tfile->GetName() << endl;
      return 1;
    }

    tfile->Close();

    Int_t binsx = m_hist->GetNbinsX();
    Int_t binsy = m_hist->GetNbinsY();
    Int_t xlow  = m_hist->GetXaxis()->FindBin(0.);
    Int_t ylow  = m_hist->GetYaxis()->FindBin(0.);

    if(binsx != m_nphi) {
        cout << "Error: Histogram contains " << binsx << " x bins."  << endl;
        return 1;
    }
    if(binsy != m_neta) {
        cout << "Error: Histogram contains " << binsy << " y bins."  << endl;
        return 1;
    }
    if(xlow != 1) {
        cout << "Error: Histogram xlow is " << xlow << " != 1"  << endl;
        return 1;
    }
    if(ylow != 1) {
        cout << "Error: Histogram ylow is " << ylow << " != 1"  << endl;
        return 1;
    }

    return 0;
}

void myAnalysis::analyze(const string &outputDir) {

    fs::create_directories(outputDir);

    stringstream filename;

    // first write the original set of offsets
    // <= is used to include the serial sector 64
    for(UInt_t i = 1; i <= m_nsector; ++i) {
        for(UInt_t j = 0; j < m_nib_per_sector; ++j) {
            filename.str("");
            filename << outputDir << "/sector" << i << "-" << j << ".dat";
            ofstream file(filename.str());
            if (!file.is_open()) {
                cout << "Error: Could not open file " << filename.str() << " for writing." << endl;
                return; // Exit if file cannot be opened
            }

            for(UInt_t k = 0; k < m_nchannel_per_ib; ++k) {
                pair<Int_t, Int_t> phi_eta = getDetectorCoordinates(i, j, k);
                Int_t binx = phi_eta.first+1;
                Int_t biny = phi_eta.second+1;
                Int_t offset = m_hist->GetBinContent(binx,biny);
                file << offset << endl;
            }

            file.close();
        }
    }
}

void write_Biasv2(const string &input,
                const string &input_hist,
                const string &input_sector,
                const string &input_channel,
                const string &outputDir=".") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "input hist: "  << input_hist << endl;
    cout << "input sector map: "  << input_sector << endl;
    cout << "input channel map: "  << input_channel << endl;
    cout << "outputDir: " << outputDir << endl;
    cout << "#############################" << endl;

    fs::create_directories(outputDir);
    TH1::AddDirectory(kFALSE);

    if(myAnalysis::readMaps(input_sector, myAnalysis::serial_to_sector, myAnalysis::sector_to_serial) ||
       myAnalysis::readMaps(input_channel, myAnalysis::ib_channel_to_ADC_channel, myAnalysis::ADC_channel_to_ib_channel) ||
       myAnalysis::readHist(input,input_hist)) return;

    myAnalysis::analyze(outputDir);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 5 || argc > 6){
        cout << "usage: ./write-Biasv2 input input_hist input_sector input_channel [outputDir] [variations] [startBiasOffset] [offset_step] [varyAllIB]" << endl;
        cout << "input: input root file" << endl;
        cout << "input_hist: input 2D bias offset histogram" << endl;
        cout << "input_sector: input sector map" << endl;
        cout << "input_channel: input channel map" << endl;
        cout << "outputDir: output directory" << endl;
        return 1;
    }

    string outputDir = ".";

    if(argc >= 6) {
        outputDir = argv[5];
    }

    write_Biasv2(argv[1], argv[2], argv[3], argv[4], outputDir);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
