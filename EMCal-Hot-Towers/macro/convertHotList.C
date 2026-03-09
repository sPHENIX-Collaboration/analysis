// c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <unordered_set>

// root includes --
#include <TSystem.h>
#include <TROOT.h>
#include <TH2F.h>
#include <TFile.h>
#include <TMath.h>
#include <TDatime.h>
#include <TGraph.h>

#include <calobase/TowerInfoDefs.h>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::stringstream;
using std::ofstream;
using std::vector;
using std::pair;
using std::min;
using std::max;
using std::unordered_set;

R__LOAD_LIBRARY(libcalo_io.so)

namespace myAnalysis {
    Int_t process(const string &input, const string &output);

    string m_detector = "CEMC";
}

Int_t myAnalysis::process(const string &input, const string &output) {
    std::ifstream file(input);
    ofstream f(output);

    f << "Hot Tower Index,Reference Tower Index" << endl;

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open file: " << input << endl;
        return 1;
    }

    string line;

    // skip the header
    std::getline(file, line);

    // loop over each run
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);

        UInt_t phibin;
        UInt_t etabin;
        char comma;

        if (lineStream >> phibin >> comma >> etabin) {
            UInt_t towerIndex = (m_detector == "CEMC") ? TowerInfoDefs::decode_emcal(TowerInfoDefs::encode_emcal(etabin, phibin)) :
                                                         TowerInfoDefs::decode_hcal(TowerInfoDefs::encode_hcal(etabin, phibin));
            cout << "iphi: " << phibin << ", ieta: " << etabin << ", towerIndex: " << towerIndex << endl;

            f << towerIndex << "," << towerIndex << endl;
        }
        else {
            cerr << "Failed to parse line: " << line << endl;
            return 1;
        }
    }

    // Close the file
    file.close();
    f.close();

    return 0;
}

void convertHotList(const string &input,
                    const string &output="hot.list") {

    cout << "#############################" << endl;
    cout << "Input Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "output: " << output << endl;

    cout << "Detector: " << myAnalysis::m_detector << endl;

    Int_t ret = myAnalysis::process(input, output);
    if(ret) return;

    cout << "======================================" << endl;
    cout << "done" << endl;
    std::quick_exit(0);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 3) {
        cout << "usage: ./hotAna inputFile hotList [outputFile]" << endl;
        cout << "inputFile: containing list of hot towers" << endl;
        cout << "outputFile: hot tower list by tower index." << endl;
        return 1;
    }

    string outputFile = "hotIndex.list";

    if (argc >= 3) {
        outputFile = argv[2];
    }

    convertHotList(argv[1], outputFile);

    return 0;
}
# endif
