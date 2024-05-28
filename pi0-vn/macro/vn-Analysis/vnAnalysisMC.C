// -- c++ includes --
#include <iostream>
#include <iomanip>
#include <fstream>

// -- root includes --
#include <TH2F.h>
#include <TFile.h>
#include <TChain.h>
#include <TMath.h>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::map;
using std::pair;
using std::make_pair;
using std::min;
using std::max;
using std::left;
using std::setw;
using std::stringstream;
using std::sin;
using std::cos;
using std::setprecision;
using std::ofstream;

namespace myAnalysis {
    TFile*  input;

    void init(const string &i_input);

    void process_event(Int_t samples = 30, const string &outputCSV = "vn.csv", const string &outputCSV2 = "vn-info.csv");
    void finalize(const string &outputFile = "vn.root");

    vector<string> cent_key;
    vector<string> cent_key1 = {"40-60", "20-40", "0-20"};
    vector<string> cent_key2 = {"50-60", "40-50", "30-40","20-30","10-20","0-10"};

    vector<string> pt_key;
    vector<string> pt_key1   = {"2-2.5", "2.5-3", "3-3.5", "3.5-4", "4-4.5", "4.5-5"};
    vector<string> pt_key2   = {"2-5"};


    vector<TH1F*> hv2;

    Int_t   bins_v2 = 60;
    Float_t v2_min  = 0;
    Float_t v2_max  = 0.6;
}

void myAnalysis::init(const string &i_input) {
    input = TFile::Open(i_input.c_str());

    for(UInt_t i = 0; i < cent_key.size(); ++i) {
        for(UInt_t j = 0; j < pt_key.size(); ++j) {
            Int_t idx = i*pt_key.size()+j;
            string suffix_title = "Centrality: " + cent_key[i] + "%, Truth #pi^{0} p_{T}: " + pt_key[j] + " GeV";
            string title = "v_{2} " + suffix_title + "; v_{2}; Counts";
            string name = "v2_"+to_string(idx);

            hv2.push_back(new TH1F(name.c_str(), title.c_str(), bins_v2, v2_min, v2_max));
        }
    }
}

void myAnalysis::process_event(Int_t samples, const string &outputCSV, const string &outputCSV2) {

    // corrected and measured v2
    vector<Float_t> sum_w_v2(cent_key.size()*pt_key.size());
    vector<Float_t> sum_w2_v2(cent_key.size()*pt_key.size());

    vector<vector<Float_t>> v2_vec(samples, vector<Float_t>(cent_key.size()*pt_key.size())); // corrected v2

    vector<vector<Float_t>> w_v2(samples, vector<Float_t>(cent_key.size()*pt_key.size())); // corrected and measured v2

    Float_t v2_min = 9999;
    Float_t v2_max = 0;

    // compute v2 for the pi0 candidates for each subsample
    for(UInt_t k = 0; k < samples; ++k) {
        cout << "Sample: " << k << endl;
        for(UInt_t i = 0; i < cent_key.size(); ++i) {
            string path = "vn/"+to_string(k)+"/QQ2/hQQ2_"+to_string(k)+"_"+cent_key[i];
            auto hQQ2 = (TH1F*)input->Get(path.c_str());

            Float_t QQ2 = hQQ2->GetMean();

            if(QQ2 <= 0) cout << "ERROR Sample: " << k << ", centrality: " << cent_key[i] << ", QQ2: " << QQ2 << endl;

            // cout << "Centrality: " << cent_key[i] << ", QQ Events: " << hQQ2->GetEntries() << endl;

            for(UInt_t j = 0; j < pt_key.size(); ++j) {
                Int_t idx = i*pt_key.size()+j;

                if(QQ2 > 0) {

                    // measured v2
                    path = "vn/"+to_string(k)+"/qQ2/hqQ2_"+to_string(k)+"_"+to_string(idx);
                    auto hqQ2 = (TH1F*)input->Get(path.c_str());

                    Float_t qQ2    = hqQ2->GetMean();
                    Float_t nPi0s  = hqQ2->GetSumOfWeights();

                    sum_w_v2[idx]  += nPi0s;
                    sum_w2_v2[idx] += nPi0s*nPi0s;
                    w_v2[k][idx]    = nPi0s;

                    Float_t v2 = qQ2/sqrt(QQ2);

                    hv2[idx]->Fill(v2, nPi0s);

                    v2_vec[k][idx] = v2;

                    v2_min = min(v2_min, v2);
                    v2_max = max(v2_max, v2);
                }
            }
        }
        cout << endl;
    }

    cout << "v2_min: " << v2_min << ", v2_max: " << v2_max << endl;
    cout << endl;

    // compute average of v2 from the above loop
    ofstream output(outputCSV.c_str());
    ofstream output2(outputCSV2.c_str());

    // write header
    output << "Index,v2,v2_err" << endl;
    output2 << "Index,Sample,v2,weight" << endl;

    stringstream s;
    for(UInt_t i = 0; i < cent_key.size(); ++i) {
        for(UInt_t j = 0; j < pt_key.size(); ++j) {
            Int_t idx = i*pt_key.size()+j;

            Float_t v2        = hv2[idx]->GetMean();
            Float_t Keff_v2   = (sum_w2_v2[idx]) ? sum_w_v2[idx]*sum_w_v2[idx]/sum_w2_v2[idx] : 0;

            Float_t v2_err        = -9999;

            if(Keff_v2 > 1) {
                Float_t sum_v2_err        = 0;

                for(UInt_t k = 0; k < samples; ++k) {
                    sum_v2_err        += w_v2[k][idx] * pow(v2_vec[k][idx]-v2, 2);

                    // write detailed log of v2 per sample to output CSV
                    s.str("");
                    s << idx << "," << k << "," << v2_vec[k][idx] << "," << w_v2[k][idx] << endl;
                    output2 << s.str();
                }

                Float_t v2_err2        = (sum_v2_err/sum_w_v2[idx]) * Keff_v2/(Keff_v2-1);
                v2_err        = sqrt(v2_err2/Keff_v2);
            }
            else cout << "Idx: " << idx << ", Keff_v2: " << Keff_v2 << endl;

            s.str("");

            s << idx << "," << v2 << "," << v2_err << endl;

            output << s.str();
        }
    }
    output.close();
    output2.close();
}

void myAnalysis::finalize(const string &outputFile) {

    TFile output(outputFile.c_str(),"recreate");

    for(UInt_t i = 0; i < cent_key.size(); ++i) {
        for(UInt_t j = 0; j < pt_key.size(); ++j) {
            Int_t idx = i*pt_key.size()+j;
            hv2[idx]->Write();
        }
    }

    output.Close();
    input->Close();
}

void vnAnalysisMC(const string &i_input,
                const string &outputCSV  = "vn.csv",
                const string &outputCSV2  = "vn-info.csv",
                const string &outputFile = "vn.root",
                      Int_t   anaType    = 1,
                      Int_t   samples    = 30) {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "   << i_input << endl;
    cout << "outputCSV: "   << outputCSV << endl;
    cout << "outputCSV2: "  << outputCSV2 << endl;
    cout << "outputFile: "  << outputFile << endl;
    cout << "anaType: "     << anaType << endl;
    cout << "Samples: "     << samples << endl;
    cout << "#############################" << endl;

    myAnalysis::cent_key = (anaType == 0) ? myAnalysis::cent_key1 : myAnalysis::cent_key2;
    myAnalysis::pt_key   = (anaType == 0) ? myAnalysis::pt_key1   : myAnalysis::pt_key2;

    myAnalysis::init(i_input);

    myAnalysis::process_event(samples, outputCSV, outputCSV2);
    myAnalysis::finalize(outputFile);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 7){
        cout << "usage: ./vnAnaMC inputFile [outputCSV] [outputCSV2] [outputFile] [anaType] [samples]" << endl;
        cout << "inputFile: containing list of root file paths" << endl;
        cout << "outputCSV: location of output CSV. Default: vn.csv." << endl;
        cout << "outputCSV2: location of output CSV2. Default: vn-info.csv." << endl;
        cout << "outputFile: location of output file. Default: vn.root." << endl;
        cout << "anaType: analysis type. Default: 1." << endl;
        cout << "samples: number of samples for the vn analysis. Default: 30." << endl;
        return 1;
    }

    string outputCSV  = "vn.csv";
    string outputCSV2 = "vn-info.csv";
    string outputFile = "vn.root";
    Int_t  anaType    = 1;
    Int_t  samples    = 30;

    if(argc >= 3) {
        outputCSV = argv[2];
    }
    if(argc >= 4) {
        outputCSV2 = argv[3];
    }
    if(argc >= 5) {
        outputFile = argv[4];
    }
    if(argc >= 6) {
        anaType = atoi(argv[5]);
    }
    if(argc >= 7) {
        samples = atoi(argv[6]);
    }

    vnAnalysisMC(argv[1], outputCSV, outputCSV2, outputFile, anaType, samples);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
