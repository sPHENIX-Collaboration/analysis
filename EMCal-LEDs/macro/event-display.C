#include <calobase/TowerInfoDefs.h>

// c++ includes --
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

// -- root includes --
#include <TH2F.h>
#include <TFile.h>
#include <TTree.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::min;
using std::max;
using std::to_string;
using std::ofstream;

R__LOAD_LIBRARY(libcalo_io.so)

namespace myAnalysis {

    void init(const string& inputFile = "data/LEDTowerBuilder.root");
    void analyze(const string& outputFile = "event-display/test.json");
    void finalize();

    TFile* input;
    TTree* led_tree;

    UInt_t eta_bins = 96;
    UInt_t phi_bins = 256;
}

void myAnalysis::init(const string& inputFile) {
    input = TFile::Open(inputFile.c_str());
    led_tree = (TTree*)input->Get("W");
}

void myAnalysis::analyze(const string& outputFile) {
    // vector<Float_t>* time              = 0;
    vector<Float_t>* ADC = 0;
    // vector<Float_t>* ped               = 0;
    vector<Int_t>* chan  = 0;
    // vector<vector<Float_t>>* waveforms = 0; // 2D: channel x time sample

    // led_tree->SetBranchAddress("time",&time);
    led_tree->SetBranchAddress("adc",&ADC);
    // led_tree->SetBranchAddress("ped",&ped);
    led_tree->SetBranchAddress("chan",&chan);
    // led_tree->SetBranchAddress("waveforms",&waveforms);
    led_tree->GetEntry(0);
    UInt_t nchannels = ADC->size();

    ofstream output(outputFile.c_str());

    output << "{\n \
    \"EVENT\": {\n \
      \"runid\": 1,\n \
      \"evtid\": 1,\n \
      \"time\": 0,\n \
      \"type\": \"Collision\",\n \
      \"s_nn\": 0,\n \
      \"B\": 3.0,\n \
      \"pv\": [0,0,0]\n \
    },\n \
    \"META\": {\n \
      \"HITS\": {\n \
        \"INNERTRACKER\": {\n \
          \"type\": \"3D\",\n \
          \"options\": {\n \
            \"size\": 5,\n \
            \"color\": 16777215\n \
          }\n \
        },\n \
        \"CEMC\": {\n \
          \"type\": \"PROJECTIVE\",\n \
            \"options\": {\n \
              \"rmin\": 90,\n \
              \"rmax\": 116.1,\n \
              \"deta\": 0.025,\n \
              \"dphi\": 0.025,\n \
              \"color\": 16766464,\n \
              \"transparent\": 0.6,\n \
              \"scaleminmax\": false\n \
            }\n \
        },\n \
        \"HCALIN\": {\n \
          \"type\": \"PROJECTIVE\",\n \
            \"options\": {\n \
              \"rmin\": 117.27,\n \
              \"rmax\": 134.42,\n \
              \"deta\": 0.025,\n \
              \"dphi\": 0.025,\n \
              \"color\": 4290445312,\n \
              \"transparent\": 0.6,\n \
              \"scaleminmax\": false\n \
            }\n \
        },\n \
        \"HCALOUT\": {\n \
          \"type\": \"PROJECTIVE\",\n \
            \"options\": {\n \
              \"rmin\": 183.3,\n \
              \"rmax\": 274.317,\n \
              \"deta\": 0.025,\n \
              \"dphi\": 0.025,\n \
              \"color\": 24773,\n \
              \"transparent\": 0.6,\n \
              \"scaleminmax\": true\n \
            }\n \
        }\n \
      }\n \
    },\n \
\"HITS\": {\n";

    for(UInt_t j = 0; j < nchannels; ++j) {
        // Float_t time_val = time->at(j);
        Float_t ADC_val  = ADC->at(j);
        Float_t e        = 0.002*ADC_val;
        // Float_t ped_val  = ped->at(j);
        Int_t channel    = chan->at(j);
        if(channel >= 24576) {
            cout << "invalid channel number: " << channel << endl;
            continue;
        }

        UInt_t key    = TowerInfoDefs::encode_emcal(channel);
        UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
        UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

        // mapping: [0,95] -> [-1.1, 1.1]
        Float_t eta = 2.2/95*etabin-1.1;
        // mapping: [0,127] -> [0, pi] and [128, 255] -> [-pi, 0]
        Float_t phi = (phibin <= 127) ? M_PI/127*phibin : M_PI/127*(phibin*1.-255);

        if(j == 0) output << "\"CEMC\": [{ \"eta\": " << eta << ", \"phi\": " << phi << " , \"e\": " << e << "}\n";

        output << ",{ \"eta\": " << eta << ", \"phi\": " << phi << " , \"e\": " << e << "}\n";
        // cout << "etabin: " << etabin << ", phibin: " << phibin << ", ADC: " << ADC_val << endl;
        // cout << ",{ \"eta\": " << eta << ", \"phi\": " << phi << " , \"e\": " << ADC_val << "}" << endl;
    }

    output << "],\n \
\"HCALIN\": [],\n \
\"HCALOUT\": []\n \
},\n \
\"TRACKS\": {\n \
\"INNERTRACKER\": []\n \
}}";

    output.close();
}

void myAnalysis::finalize() {
    // Close root file
    input->Close();
}

void event_display(const string &inputFile="data/LEDTowerBuilder.root",
                   const string &outputFile="event-display/test.json") {

    myAnalysis::init(inputFile);
    myAnalysis::analyze(outputFile);
    myAnalysis::finalize();
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 1 || argc > 3){
        cout << "usage: ./bin/event-display inputFile outputFile" << endl;
        cout << "inputFile: Location of LEDTowerBuilder.root. Default = data/LEDTowerBuilder.root." << endl;
        cout << "outputFile: output json file. Default = event-display/test.json." << endl;
        return 1;
    }

    string input  = "data/LEDTowerBuilder.root";
    string output = "event-display/test.json";

    if(argc >= 2) {
        input = argv[1];
    }
    if(argc >= 3) {
        output = argv[2];
    }
    event_display(input, output);

    cout << "done" << endl;
    return 0;
}
# endif
