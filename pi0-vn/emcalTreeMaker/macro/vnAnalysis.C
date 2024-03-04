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

    Bool_t init(const string &i_input, const string &fitStats);

    void process_event(Int_t samples = 20, const string &outputCSV = "vn.csv");
    void finalize(const string &outputFile = "vn.root");

    Int_t readFitStats(const string &fitStats);

    vector<string> cent_key = {"40-60", "20-40", "0-20"};
    vector<string> pt_key   = {"2-2.5", "2.5-3", "3-3.5", "3.5-4", "4-4.5", "4.5-5"};

    vector<Float_t> SB;

    vector<TH1F*> hv2;
    vector<TH1F*> hv3;

    Int_t   bins_v2 = 280;
    Float_t v2_min  = -7;
    Float_t v2_max  = 7;

    Int_t   bins_v3 = 600;
    Float_t v3_min  = -15;
    Float_t v3_max  = 15;
}

Bool_t myAnalysis::init(const string &i_input, const string &fitStats) {
    input = TFile::Open(i_input.c_str());

    for(UInt_t i = 0; i < cent_key.size(); ++i) {
        for(UInt_t j = 0; j < pt_key.size(); ++j) {
            Int_t idx = i*pt_key.size()+j;
            string suffix_title = "Centrality: " + cent_key[i] + "%, Diphoton p_{T}: " + pt_key[j] + " GeV";
            string title = "v_{2} " + suffix_title + "; v_{2}; Counts";
            string name = "v2_"+to_string(idx);

            hv2.push_back(new TH1F(name.c_str(), title.c_str(), bins_v2, v2_min, v2_max));

            title = "v_{3} " + suffix_title + "; v_{3}; Counts";
            name = "v3_"+to_string(idx);

            hv3.push_back(new TH1F(name.c_str(), title.c_str(), bins_v3, v3_min, v3_max));
        }
    }

    return readFitStats(fitStats);
}

Int_t myAnalysis::readFitStats(const string &fitStats) {
    // Create an input stream
    std::ifstream file(fitStats);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open fit-stats file: " << fitStats << endl;
        return 1;
    }

    // 9: S/B
    Int_t col = 9;

    std::string line;

    // skip header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;

        Int_t colIdx = 0;
        while (std::getline(ss, token, ',')) { // Assuming comma-separated values
            if (colIdx == col) {
                SB.push_back(stof(token));
            }
            colIdx++;
        }
    }

    cout << endl;
    for(Int_t i = 0; i < cent_key.size(); ++i) {
        cout << "cent: " << cent_key[i] << endl;
        for(Int_t j = 0; j < pt_key.size(); ++j) {
            Int_t idx = i*pt_key.size()+j;
            cout << "pt: " << pt_key[j] << ", S/B: " << SB[idx] << endl;
        }
        cout << endl;
    }

    return 0;
}

void myAnalysis::process_event(Int_t samples, const string &outputCSV) {

    vector<Float_t> sum_w(cent_key.size()*pt_key.size());
    vector<Float_t> sum_w2(cent_key.size()*pt_key.size());

    vector<vector<Float_t>> v2_vec(samples, vector<Float_t>(cent_key.size()*pt_key.size()));
    vector<vector<Float_t>> v3_vec(samples, vector<Float_t>(cent_key.size()*pt_key.size()));
    vector<vector<Float_t>>  w(samples, vector<Float_t>(cent_key.size()*pt_key.size()));

    Float_t v2_min  = 9999;
    Float_t v3_min  = 9999;

    Float_t v2_max  = 0;
    Float_t v3_max  = 0;

    // compute v2 and v3 for the pi0 candidates for each subsample
    for(UInt_t k = 0; k < samples; ++k) {
        cout << "Sample: " << k << endl;
        for(UInt_t i = 0; i < cent_key.size(); ++i) {
            string path = "vn/"+to_string(k)+"/QQ2/hQQ2_"+to_string(k)+"_"+cent_key[i];
            auto hQQ2 = (TH1F*)input->Get(path.c_str());

            path = "vn/"+to_string(k)+"/QQ3/hQQ3_"+to_string(k)+"_"+cent_key[i];
            auto hQQ3 = (TH1F*)input->Get(path.c_str());

            Float_t QQ2 = hQQ2->GetMean();
            Float_t QQ3 = hQQ3->GetMean();

            if(QQ2 <= 0) cout << "ERROR Sample: " << k << ", centrality: " << cent_key[i] << ", QQ2: " << QQ2 << endl;
            if(QQ3 <= 0) cout << "ERROR Sample: " << k << ", centrality: " << cent_key[i] << ", QQ3: " << QQ3 << endl;

            cout << "Centrality: " << cent_key[i] << ", QQ Events: " << hQQ2->GetEntries() << endl;

            for(UInt_t j = 0; j < pt_key.size(); ++j) {
                Int_t idx = i*pt_key.size()+j;

                path = "vn/"+to_string(k)+"/qQ2/hqQ2_"+to_string(k)+"_"+to_string(idx);
                auto hqQ2 = (TH1F*)input->Get(path.c_str());

                Float_t qQ2    = hqQ2->GetMean();
                Float_t nPi0s  = hqQ2->GetSumOfWeights();

                sum_w[idx]  += nPi0s;
                sum_w2[idx] += nPi0s*nPi0s;
                w[k][idx]    = nPi0s;

                cout << "pT: " << pt_key[j] << ", qQ Events: " << hqQ2->GetEntries() << ", " << hqQ2->GetEntries()*100./hQQ2->GetEntries() << " %" << endl;

                if(QQ2 > 0) {
                    path = "vn/"+to_string(k)+"/qQ2_bg/hqQ2_bg_"+to_string(k)+"_"+to_string(idx);
                    auto hqQ2_bg = (TH1F*)input->Get(path.c_str());
                    Float_t qQ2_bg = hqQ2_bg->GetMean();

                    Float_t v2_diphoton = qQ2/sqrt(QQ2);
                    Float_t v2_bg       = qQ2_bg/sqrt(QQ2);

                    Float_t v2 = v2_diphoton + (v2_diphoton - v2_bg)/SB[idx];
                    hv2[idx]->Fill(v2, nPi0s);
                    v2_vec[k][idx] = v2;

                    v2_min = min(v2_min, v2);
                    v2_max = max(v2_max, v2);
                }

                if(QQ3 > 0) {
                    path = "vn/"+to_string(k)+"/qQ3/hqQ3_"+to_string(k)+"_"+to_string(idx);
                    auto hqQ3 = (TH1F*)input->Get(path.c_str());

                    path = "vn/"+to_string(k)+"/qQ3_bg/hqQ3_bg_"+to_string(k)+"_"+to_string(idx);
                    auto hqQ3_bg = (TH1F*)input->Get(path.c_str());

                    Float_t qQ3    = hqQ3->GetMean();
                    Float_t qQ3_bg = hqQ3_bg->GetMean();
                    Float_t nPi0s  = hqQ3->GetSumOfWeights();

                    Float_t v3_diphoton = qQ3/sqrt(QQ3);
                    Float_t v3_bg       = qQ3_bg/sqrt(QQ3);

                    Float_t v3 = v3_diphoton + (v3_diphoton - v3_bg)/SB[idx];
                    hv3[idx]->Fill(v3, nPi0s);
                    v3_vec[k][idx] = v3;

                    v3_min = min(v3_min, v3);
                    v3_max = max(v3_max, v3);
                }
            }
        }
        cout << endl;
    }

    cout << endl;
    cout << "v2_min: " << v2_min << ", v2_max: " << v2_max << endl;
    cout << "v3_min: " << v3_min << ", v3_max: " << v3_max << endl;
    cout << endl;

    // compute average of v2 and v3 from the above loop
    ofstream output(outputCSV.c_str());

    // write header
    output << "Index,v2,v2_err,v3,v3_err" << endl;

    stringstream s;
    for(UInt_t i = 0; i < cent_key.size(); ++i) {
        for(UInt_t j = 0; j < pt_key.size(); ++j) {
            Int_t idx = i*pt_key.size()+j;

            if(!sum_w2[idx]) {
                cout << "ERROR Cent: " << cent_key[i] << ", pT: " << pt_key[j] << ", No Pi0s" << endl;
                continue;
            }

            Float_t Keff = sum_w[idx]*sum_w[idx]/sum_w2[idx];

            if(Keff <= 1) {
                cout << "ERROR Cent: " << cent_key[i] << ", pT: " << pt_key[j] << ", Keff: " << Keff << endl;
                continue;
            }

            Float_t v2 = hv2[idx]->GetMean();
            Float_t v3 = hv3[idx]->GetMean();

            Float_t sum_v2_err = 0;
            Float_t sum_v3_err = 0;

            for(UInt_t k = 0; k < samples; ++k) {
                sum_v2_err += w[k][idx] * pow(v2_vec[k][idx]-v2, 2);
                sum_v3_err += w[k][idx] * pow(v3_vec[k][idx]-v3, 2);
            }

            Float_t v2_err2 = (sum_v2_err/sum_w[idx]) * Keff/(Keff-1);
            Float_t v2_err  = sqrt(v2_err2/Keff);

            Float_t v3_err2 = (sum_v3_err/sum_w[idx]) * Keff/(Keff-1);
            Float_t v3_err  = sqrt(v3_err2/Keff);

            s.str("");

            s << idx << "," << v2 << "," << v2_err << "," << v3 << "," << v3_err << endl;

            output << s.str();
        }
    }
    output.close();
}

void myAnalysis::finalize(const string &outputFile) {

    TFile output(outputFile.c_str(),"recreate");

    output.mkdir("v2");
    output.mkdir("v3");

    for(UInt_t i = 0; i < cent_key.size(); ++i) {
        for(UInt_t j = 0; j < pt_key.size(); ++j) {
            Int_t idx = i*pt_key.size()+j;

            output.cd("v2");
            hv2[idx]->Write();

            output.cd("v3");
            hv3[idx]->Write();
        }
    }

    output.Close();
    input->Close();
}

void vnAnalysis(const string &i_input,
                const string &fitStats,
                Int_t         samples  = 20,
                const string &outputCSV = "vn.csv",
                const string &outputFile = "vn.root") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "   << i_input << endl;
    cout << "fitStats: "    << fitStats << endl;
    cout << "Samples: "     << samples << endl;
    cout << "outputCSV: "   << outputCSV << endl;
    cout << "outputFile: "  << outputFile << endl;
    cout << "#############################" << endl;

    if(myAnalysis::init(i_input, fitStats)) return;

    myAnalysis::process_event(samples, outputCSV);
    myAnalysis::finalize(outputFile);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 3 || argc > 6){
        cout << "usage: ./vnAna inputFile fitStats [samples] [outputCSV] [outputFile]" << endl;
        cout << "inputFile: containing list of root file paths" << endl;
        cout << "fitStats: csv file containing fit stats" << endl;
        cout << "samples: number of samples for the vn analysis" << endl;
        cout << "outputCSV: location of output CSV. Default: vn.csv." << endl;
        cout << "outputFile: location of output file. Default: vn.root." << endl;
        return 1;
    }

    Int_t  samples     = 20;
    string outputCSV   = "vn.csv";
    string outputFile  = "vn.root";

    if(argc >= 4) {
        samples = atoi(argv[3]);
    }
    if(argc >= 5) {
        outputCSV = argv[4];
    }
    if(argc >= 6) {
        outputFile = argv[5];
    }

    vnAnalysis(argv[1], argv[2], samples, outputCSV, outputFile);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
