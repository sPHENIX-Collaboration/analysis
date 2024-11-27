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
    void analyze(const string& i_input, const string &output, const string &log);

    UInt_t  m_ntowers = 24576;
    Float_t m_threshold;
    Float_t m_thresholdNoise;
    string m_detector = "CEMC";
    // string m_detector = "HCAL";
}

void myAnalysis::analyze(const string& i_input, const string &output, const string &log) {
    TFile input(i_input.c_str());

    auto hBadTowersHot  = (TH1F*)input.Get("hBadTowersHot");

    ofstream f(output);
    f << "Hot Tower Index,Reference Tower Index" << endl;

    ofstream f_l(log);
    f_l << "Hot Phi,Hot Eta,Runs Hot,Ref Phi,Ref Eta,Hot Ref Runs,Phi Distance" << endl;

    for(UInt_t i = 1; i <= m_ntowers; ++i) {
        UInt_t towerIndex = i-1;
        UInt_t key    = (m_detector == "CEMC") ? TowerInfoDefs::encode_emcal(towerIndex) : TowerInfoDefs::encode_hcal(towerIndex);
        UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
        UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

        UInt_t towerIndexRef = 0;
        UInt_t nHotRuns = hBadTowersHot->GetBinContent(i);

        if(nHotRuns >= m_threshold) {
            UInt_t j = 1;
            UInt_t towerIndexRef = 0;
            UInt_t nRuns = 0;
            while(1) {
                UInt_t towerIndexRight = (m_detector == "CEMC") ? TowerInfoDefs::decode_emcal(TowerInfoDefs::encode_emcal(etabin, phibin+j))
                                                                : TowerInfoDefs::decode_hcal(TowerInfoDefs::encode_hcal(etabin, phibin+j));
                UInt_t towerIndexLeft  = (m_detector == "CEMC") ? TowerInfoDefs::decode_emcal(TowerInfoDefs::encode_emcal(etabin, phibin-j))
                                                                : TowerInfoDefs::decode_hcal(TowerInfoDefs::encode_hcal(etabin, phibin-j));
                UInt_t nRunsRight = hBadTowersHot->GetBinContent(towerIndexRight+1);
                UInt_t nRunsLeft  = hBadTowersHot->GetBinContent(towerIndexLeft+1);

                nRuns         = nRunsRight;
                towerIndexRef = towerIndexRight;

                if(nRunsLeft < nRunsRight) {
                    nRuns         = nRunsLeft;
                    towerIndexRef = towerIndexLeft;
                }

                if(nRuns < m_thresholdNoise) break;

                ++j;
            }
            UInt_t keyRef    = (m_detector == "CEMC") ? TowerInfoDefs::encode_emcal(towerIndexRef)
                                                      : TowerInfoDefs::encode_hcal(towerIndexRef);
            UInt_t etabinRef = TowerInfoDefs::getCaloTowerEtaBin(keyRef);
            UInt_t phibinRef = TowerInfoDefs::getCaloTowerPhiBin(keyRef);

            cout << "Hot: " << phibin << ", " << etabin << ", Runs: " << nHotRuns
                 << ", Ref: " << phibinRef << ", " << etabinRef
                 << ", j: " << j
                 << ", Runs: " << nRuns << endl;

            f_l << phibin << ","
                << etabin << ","
                << nHotRuns << ","
                << phibinRef << ","
                << etabinRef << ","
                << nRuns << ","
                << j << endl;

            f << towerIndex << "," << towerIndexRef << endl;
        }
    }

    f.close();
    f_l.close();
}

void genHotTowerList(const string &input, Int_t threshold = 400, Int_t thresholdNoise = 20, const string &output="hot.list", const string &log="log.csv") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "output: " << output << endl;
    cout << "log: " << log << endl;
    cout << "threshold: " << threshold << endl;
    cout << "Noise threshold: " << thresholdNoise << endl;
    cout << "#############################" << endl;

    myAnalysis::m_threshold      = threshold;
    myAnalysis::m_thresholdNoise = thresholdNoise;

    if(myAnalysis::m_detector == "HCAL") {
        myAnalysis::m_ntowers = 1536;
    }

    myAnalysis::analyze(input, output, log);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 6){
        cout << "usage: ./genHotTowerList input [threshold] [thresholdNoise] [output] [log]" << endl;
        cout << "input: input root file" << endl;
        cout << "threshold: minimum number of runs to be considered frequently hot. Default: 400" << endl;
        cout << "thresholdNoise: maximum number of runs to be considered not hot frequently. Default: 20" << endl;
        cout << "output: output hot tower list. Default: hot.list" << endl;
        cout << "log: processing log. Default: log.txt" << endl;
        return 1;
    }

    Int_t threshold      = 400;
    Int_t thresholdNoise = 20;
    string output        = "hot.list";
    string log           = "log.csv";

    if(argc >= 3) {
        threshold = atoi(argv[2]);
    }
    if(argc >= 4) {
        thresholdNoise = atoi(argv[3]);
    }
    if(argc >= 5) {
        output = argv[4];
    }
    if(argc >= 5) {
        log = argv[5];
    }

    genHotTowerList(argv[1], threshold, thresholdNoise, output, log);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
