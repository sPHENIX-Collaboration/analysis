// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

// -- root includes --
#include <TH1F.h>
#include <TFile.h>

// -- Tower Info
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

R__LOAD_LIBRARY(libcalo_io.so)

namespace myAnalysis {
    void analyze(const string& i_input, const string &output);

    UInt_t  ntowers   = 24576;
    Float_t threshold;
    Float_t thresholdNoise;
}

void myAnalysis::analyze(const string& i_input, const string &output) {
    TFile input(i_input.c_str());

    auto hBadTowersHot  = (TH1F*)input.Get("hBadTowersHot");

    ofstream f(output);
    f << "Hot Tower Index,Reference Tower Index" << endl;

    for(UInt_t i = 1; i <= ntowers; ++i) {
        UInt_t towerIndex = i-1;
        UInt_t key    = TowerInfoDefs::encode_emcal(towerIndex);
        UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
        UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

        UInt_t towerIndexRef = 0;
        UInt_t nHotRuns = hBadTowersHot->GetBinContent(i);

        if(nHotRuns >= threshold) {
            UInt_t j = 1;
            UInt_t towerIndexRef = 0;
            UInt_t nRuns = 0;
            while(1) {
                UInt_t towerIndexRight = TowerInfoDefs::decode_emcal(TowerInfoDefs::encode_emcal(etabin, phibin+j));
                UInt_t towerIndexLeft  = TowerInfoDefs::decode_emcal(TowerInfoDefs::encode_emcal(etabin, phibin-j));
                UInt_t nRunsRight = hBadTowersHot->GetBinContent(towerIndexRight+1);
                UInt_t nRunsLeft  = hBadTowersHot->GetBinContent(towerIndexLeft+1);

                nRuns         = nRunsRight;
                towerIndexRef = towerIndexRight;

                if(nRunsLeft < nRunsRight) {
                    nRuns         = nRunsLeft;
                    towerIndexRef = towerIndexLeft;
                }

                if(nRuns < thresholdNoise) break;

                ++j;
            }
            UInt_t keyRef    = TowerInfoDefs::encode_emcal(towerIndexRef);
            UInt_t etabinRef = TowerInfoDefs::getCaloTowerEtaBin(keyRef);
            UInt_t phibinRef = TowerInfoDefs::getCaloTowerPhiBin(keyRef);

            cout << "Hot: " << phibin << ", " << etabin << ", Runs: " << nHotRuns
                 << ", Ref: " << phibinRef << ", " << etabinRef
                 << ", j: " << j
                 << ", Runs: " << nRuns << endl;

            f << towerIndex << "," << towerIndexRef << endl;
        }
    }

    f.close();
}

void genHotTowerList(const string &input, Int_t threshold = 100, Int_t thresholdNoise = 20, const string &output="hot.list") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "output: " << output << endl;
    cout << "threshold: " << threshold << endl;
    cout << "Noise threshold: " << thresholdNoise << endl;
    cout << "#############################" << endl;

    myAnalysis::threshold      = threshold;
    myAnalysis::thresholdNoise = thresholdNoise;
    myAnalysis::analyze(input, output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 5){
        cout << "usage: ./genHotTowerList input [threshold] [thresholdNoise] [output]" << endl;
        cout << "input: input root file" << endl;
        cout << "threshold: minimum number of runs to be considered frequently hot. Default: 100" << endl;
        cout << "thresholdNoise: maximum number of runs to be considered not hot frequently. Default: 20" << endl;
        cout << "output: output hot tower list. Default: hot.list" << endl;
        return 1;
    }

    string output        = "hot.list";
    Int_t threshold      = 100;
    Int_t thresholdNoise = 20;

    if(argc >= 3) {
        threshold = atoi(argv[2]);
    }
    if(argc >= 4) {
        thresholdNoise = atoi(argv[3]);
    }
    if(argc >= 5) {
        output = argv[2];
    }

    genHotTowerList(argv[1], threshold, thresholdNoise, output);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
