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

    void process_event(Int_t samples = 30, const string &outputCSV = "vn.csv", const string &outputCSV2 = "vn-info.csv");
    void finalize(const string &outputFile = "vn.root");

    Int_t readFitStats(const string &fitStats);

    vector<string> cent_key;
    vector<string> cent_key1 = {"40-60", "20-40", "0-20"};
    vector<string> cent_key2 = {"50-60", "40-50", "30-40","20-30","10-20","0-10"};

    vector<string> pt_key;
    vector<string> pt_key1   = {"2-2.5", "2.5-3", "3-3.5", "3.5-4", "4-4.5", "4.5-5"};
    vector<string> pt_key2   = {"2-5"};

    vector<Float_t> SB;

    vector<TH1F*> hv2;
    vector<TH1F*> hv2_type_4;
    vector<TH1F*> hv2_type_4sd;
    vector<TH1F*> hv2_m;
    vector<TH1F*> hv2_bg;
    vector<TH1F*> hv2_bg_4;
    vector<TH1F*> hv2_bg_4sd;
    vector<TH1F*> hv3;

    Int_t   bins_v2 = 250;
    Float_t v2_min  = -2;
    Float_t v2_max  = 1.5;

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

            auto h = new TH1F(name.c_str(), title.c_str(), bins_v2, v2_min, v2_max);
            h->Sumw2();
            hv2.push_back(h);

            title = "v_{2} " + suffix_title + "; v_{2}; Counts";
            name = "v2_type_4_"+to_string(idx);

            h = new TH1F(name.c_str(), title.c_str(), bins_v2, v2_min, v2_max);
            h->Sumw2();
            hv2_type_4.push_back(h);

            title = "v_{2} " + suffix_title + "; v_{2}; Counts";
            name = "v2_type_4sd_"+to_string(idx);

            h = new TH1F(name.c_str(), title.c_str(), bins_v2, v2_min, v2_max);
            h->Sumw2();
            hv2_type_4sd.push_back(h);

            title = "v_{2} Measured " + suffix_title + "; v_{2}; Counts";
            name  = "v2_m_"+to_string(idx);

            h = new TH1F(name.c_str(), title.c_str(), bins_v2, v2_min, v2_max);
            h->Sumw2();
            hv2_m.push_back(h);

            title = "v_{2} Background " + suffix_title + "; v_{2}; Counts";
            name  = "v2_bg_"+to_string(idx);

            h = new TH1F(name.c_str(), title.c_str(), bins_v2, v2_min, v2_max);
            h->Sumw2();
            hv2_bg.push_back(h);

            title = "v_{2} Background " + suffix_title + "; v_{2}; Counts";
            name  = "v2_bg_4_"+to_string(idx);

            h = new TH1F(name.c_str(), title.c_str(), bins_v2, v2_min, v2_max);
            h->Sumw2();
            hv2_bg_4.push_back(h);

            title = "v_{2} Background " + suffix_title + "; v_{2}; Counts";
            name  = "v2_bg_4sd_"+to_string(idx);

            h = new TH1F(name.c_str(), title.c_str(), bins_v2, v2_min, v2_max);
            h->Sumw2();
            hv2_bg_4sd.push_back(h);

            title = "v_{3} " + suffix_title + "; v_{3}; Counts";
            name = "v3_"+to_string(idx);

            h = new TH1F(name.c_str(), title.c_str(), bins_v3, v3_min, v3_max);
            h->Sumw2();
            hv3.push_back(h);
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

    // 3: S/B
    Int_t col = 3;

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

void myAnalysis::process_event(Int_t samples, const string &outputCSV, const string &outputCSV2) {

    // corrected and measured v2
    vector<Float_t> sum_w_v2(cent_key.size()*pt_key.size());
    vector<Float_t> sum_w2_v2(cent_key.size()*pt_key.size());

    // background v2
    vector<Float_t> sum_w_v2_bg(cent_key.size()*pt_key.size());
    vector<Float_t> sum_w2_v2_bg(cent_key.size()*pt_key.size());

    // background v2
    vector<Float_t> sum_w_v2_bg_4(cent_key.size()*pt_key.size());
    vector<Float_t> sum_w2_v2_bg_4(cent_key.size()*pt_key.size());

    vector<Float_t> sum_w_v2_bg_4sd(cent_key.size()*pt_key.size());
    vector<Float_t> sum_w2_v2_bg_4sd(cent_key.size()*pt_key.size());

    vector<Float_t> sum_w_v3(cent_key.size()*pt_key.size());
    vector<Float_t> sum_w2_v3(cent_key.size()*pt_key.size());

    vector<vector<Float_t>> v2_vec(samples, vector<Float_t>(cent_key.size()*pt_key.size())); // corrected v2
    vector<vector<Float_t>> v2_type_4_vec(samples, vector<Float_t>(cent_key.size()*pt_key.size())); // corrected v2
    vector<vector<Float_t>> v2_type_4sd_vec(samples, vector<Float_t>(cent_key.size()*pt_key.size())); // corrected v2
    vector<vector<Float_t>> v2_m_vec(samples, vector<Float_t>(cent_key.size()*pt_key.size())); // measured v2
    vector<vector<Float_t>> v2_bg_vec(samples, vector<Float_t>(cent_key.size()*pt_key.size())); // background v2
    vector<vector<Float_t>> v2_bg_4_vec(samples, vector<Float_t>(cent_key.size()*pt_key.size())); // background v2
    vector<vector<Float_t>> v2_bg_4sd_vec(samples, vector<Float_t>(cent_key.size()*pt_key.size())); // background v2

    vector<vector<Float_t>> w_v2(samples, vector<Float_t>(cent_key.size()*pt_key.size())); // corrected and measured v2
    vector<vector<Float_t>> w_v2_bg(samples, vector<Float_t>(cent_key.size()*pt_key.size())); // background v2
    vector<vector<Float_t>> w_v2_bg_4(samples, vector<Float_t>(cent_key.size()*pt_key.size())); // background v2
    vector<vector<Float_t>> w_v2_bg_4sd(samples, vector<Float_t>(cent_key.size()*pt_key.size())); // background v2

    vector<vector<Float_t>> v3_vec(samples, vector<Float_t>(cent_key.size()*pt_key.size()));
    vector<vector<Float_t>> w_v3(samples, vector<Float_t>(cent_key.size()*pt_key.size()));

    Float_t v2_min    = 9999;
    Float_t v2_m_min  = 9999;
    Float_t v2_bg_min = 9999;
    Float_t v3_min    = 9999;

    Float_t v2_max    = 0;
    Float_t v2_m_max  = 0;
    Float_t v2_bg_max = 0;
    Float_t v3_max    = 0;

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

                    // cout << "pT: " << pt_key[j] << ", qQ Events: " << hqQ2->GetEntries() << ", " << hqQ2->GetEntries()*100./hQQ2->GetEntries() << " %" << endl;

                    // background v2
                    path = "vn/"+to_string(k)+"/qQ2_bg_4/hqQ2_bg_4_"+to_string(k)+"_"+to_string(idx);
                    auto hqQ2_bg_4 = (TH1F*)input->Get(path.c_str());

                    Float_t qQ2_bg_4  = hqQ2_bg_4->GetMean();
                    Float_t nBGs_4    = hqQ2_bg_4->GetSumOfWeights();

                    sum_w_v2_bg_4[idx]  += nBGs_4;
                    sum_w2_v2_bg_4[idx] += nBGs_4*nBGs_4;
                    w_v2_bg_4[k][idx]    = nBGs_4;

                    // background v2
                    path = "vn/"+to_string(k)+"/qQ2_bg_4sd/hqQ2_bg_4sd_"+to_string(k)+"_"+to_string(idx);
                    auto hqQ2_bg_4sd = (TH1F*)input->Get(path.c_str());

                    Float_t qQ2_bg_4sd = hqQ2_bg_4sd->GetMean();
                    Float_t nBGs_4sd   = hqQ2_bg_4sd->GetSumOfWeights();

                    sum_w_v2_bg_4sd[idx]  += nBGs_4sd;
                    sum_w2_v2_bg_4sd[idx] += nBGs_4sd*nBGs_4sd;
                    w_v2_bg_4sd[k][idx]    = nBGs_4sd;

                    // background v2
                    path = "vn/"+to_string(k)+"/qQ2_bg/hqQ2_bg_"+to_string(k)+"_"+to_string(idx);
                    auto hqQ2_bg = (TH1F*)input->Get(path.c_str());

                    Float_t qQ2_bg  = hqQ2_bg->GetMean();
                    Float_t nBGs    = hqQ2_bg->GetSumOfWeights();

                    sum_w_v2_bg[idx]  += nBGs;
                    sum_w2_v2_bg[idx] += nBGs*nBGs;
                    w_v2_bg[k][idx]    = nBGs;

                    Float_t v2_bg       = qQ2_bg/sqrt(QQ2);
                    Float_t v2_bg_4     = qQ2_bg_4/sqrt(QQ2);
                    Float_t v2_bg_4sd   = qQ2_bg_4sd/sqrt(QQ2);

                    Float_t v2_diphoton = qQ2/sqrt(QQ2);

                    Float_t v2          = v2_diphoton + (v2_diphoton - v2_bg)/SB[idx];
                    Float_t v2_type_4   = v2_diphoton + (v2_diphoton - v2_bg_4)/SB[idx];
                    Float_t v2_type_4sd = v2_diphoton + (v2_diphoton - v2_bg_4sd)/SB[idx];

                    hv2[idx]->Fill(v2, nPi0s);
                    hv2_type_4[idx]->Fill(v2_type_4, nPi0s);
                    hv2_type_4sd[idx]->Fill(v2_type_4sd, nPi0s);
                    hv2_m[idx]->Fill(v2_diphoton, nPi0s);
                    hv2_bg[idx]->Fill(v2_bg, nBGs);
                    hv2_bg_4[idx]->Fill(v2_bg_4, nBGs_4);
                    hv2_bg_4sd[idx]->Fill(v2_bg_4sd, nBGs_4sd);

                    v2_vec[k][idx]          = v2;
                    v2_type_4_vec[k][idx]   = v2_type_4;
                    v2_type_4sd_vec[k][idx] = v2_type_4sd;
                    v2_m_vec[k][idx]        = v2_diphoton;
                    v2_bg_vec[k][idx]       = v2_bg;
                    v2_bg_4_vec[k][idx]     = v2_bg_4;
                    v2_bg_4sd_vec[k][idx]   = v2_bg_4sd;

                    v2_min = min(v2_min, v2);
                    v2_max = max(v2_max, v2);

                    v2_min = min(v2_min, v2_type_4);
                    v2_max = max(v2_max, v2_type_4);

                    v2_min = min(v2_min, v2_type_4sd);
                    v2_max = max(v2_max, v2_type_4sd);

                    v2_m_min = min(v2_m_min, v2_diphoton);
                    v2_m_max = max(v2_m_max, v2_diphoton);

                    v2_bg_min = min(v2_bg_min, v2_bg);
                    v2_bg_max = max(v2_bg_max, v2_bg);

                    v2_bg_min = min(v2_bg_min, v2_bg_4);
                    v2_bg_max = max(v2_bg_max, v2_bg_4);

                    v2_bg_min = min(v2_bg_min, v2_bg_4sd);
                    v2_bg_max = max(v2_bg_max, v2_bg_4sd);
                }

                if(QQ3 > 0) {
                    path = "vn/"+to_string(k)+"/qQ3/hqQ3_"+to_string(k)+"_"+to_string(idx);
                    auto hqQ3 = (TH1F*)input->Get(path.c_str());

                    path = "vn/"+to_string(k)+"/qQ3_bg/hqQ3_bg_"+to_string(k)+"_"+to_string(idx);
                    auto hqQ3_bg = (TH1F*)input->Get(path.c_str());

                    Float_t qQ3    = hqQ3->GetMean();
                    Float_t qQ3_bg = hqQ3_bg->GetMean();
                    Float_t nPi0s  = hqQ3->GetSumOfWeights();

                    sum_w_v3[idx]  += nPi0s;
                    sum_w2_v3[idx] += nPi0s*nPi0s;
                    w_v3[k][idx]    = nPi0s;

                    Float_t v3_diphoton = qQ3/sqrt(QQ3);
                    Float_t v3_bg       = qQ3_bg/sqrt(QQ3);

                    Float_t v3 = v3_diphoton + (v3_diphoton - v3_bg)/SB[idx];
                    hv3[idx]->Fill(v3, nPi0s);
                    v3_vec[k][idx] = v3;

                    v3_min = min(v3_min, v3);
                    v3_max = max(v3_max, v3);
                }
            }
            // cout << endl;
        }
        cout << endl;
    }

    cout << "v2_min: " << v2_min << ", v2_max: " << v2_max << endl;
    cout << "v2_m_min: " << v2_m_min << ", v2_m_max: " << v2_m_max << endl;
    cout << "v2_bg_min: " << v2_bg_min << ", v2_bg_max: " << v2_bg_max << endl;
    cout << "v3_min: " << v3_min << ", v3_max: " << v3_max << endl;
    cout << endl;

    // compute average of v2 and v3 from the above loop
    ofstream output(outputCSV.c_str());
    ofstream output2(outputCSV2.c_str());

    // write header
    output << "Index,v2,v2_err,v2_type_4,v2_type_4_err,v2_type_4sd,v2_type_4sd_err,v3,v3_err,v2_m,v2_m_err,v2_bg,v2_bg_err,v2_bg_4,v2_bg_4_err,v2_bg_4sd,v2_bg_4sd_err" << endl;
    output2 << "Index,Sample,v2,v2_m,v2_bg,weight" << endl;

    stringstream s;
    for(UInt_t i = 0; i < cent_key.size(); ++i) {
        for(UInt_t j = 0; j < pt_key.size(); ++j) {
            Int_t idx = i*pt_key.size()+j;

            Float_t v2          = hv2[idx]->GetMean();
            Float_t v2_type_4   = hv2_type_4[idx]->GetMean();
            Float_t v2_type_4sd = hv2_type_4sd[idx]->GetMean();
            Float_t v2_m        = hv2_m[idx]->GetMean();
            Float_t Keff_v2     = (sum_w2_v2[idx]) ? sum_w_v2[idx]*sum_w_v2[idx]/sum_w2_v2[idx] : 0;

            Float_t v2_bg     = hv2_bg[idx]->GetMean();
            Float_t v2_bg_4   = hv2_bg_4[idx]->GetMean();
            Float_t v2_bg_4sd = hv2_bg_4sd[idx]->GetMean();

            Float_t Keff_v2_bg     = (sum_w2_v2_bg[idx]) ? sum_w_v2_bg[idx]*sum_w_v2_bg[idx]/sum_w2_v2_bg[idx] : 0;
            Float_t Keff_v2_bg_4   = (sum_w2_v2_bg_4[idx]) ? sum_w_v2_bg_4[idx]*sum_w_v2_bg_4[idx]/sum_w2_v2_bg_4[idx] : 0;
            Float_t Keff_v2_bg_4sd = (sum_w2_v2_bg_4sd[idx]) ? sum_w_v2_bg_4sd[idx]*sum_w_v2_bg_4sd[idx]/sum_w2_v2_bg_4sd[idx] : 0;

            Float_t v2_err          = -9999;
            Float_t v2_type_4_err   = -9999;
            Float_t v2_type_4sd_err = -9999;
            Float_t v2_m_err        = -9999;
            Float_t v2_bg_err       = -9999;
            Float_t v2_bg_4_err     = -9999;
            Float_t v2_bg_4sd_err   = -9999;

            if(Keff_v2 > 1) {
                Float_t sum_v2_err          = 0;
                Float_t sum_v2_type_4_err   = 0;
                Float_t sum_v2_type_4sd_err = 0;
                Float_t sum_v2_m_err        = 0;

                for(UInt_t k = 0; k < samples; ++k) {
                    sum_v2_err          += w_v2[k][idx] * pow(v2_vec[k][idx]-v2, 2);
                    sum_v2_type_4_err   += w_v2[k][idx] * pow(v2_type_4sd_vec[k][idx]-v2_type_4sd, 2);
                    sum_v2_type_4sd_err += w_v2[k][idx] * pow(v2_type_4_vec[k][idx]-v2_type_4, 2);
                    sum_v2_m_err        += w_v2[k][idx] * pow(v2_m_vec[k][idx]-v2_m, 2);

                    // write detailed log of v2 per sample to output CSV
                    s.str("");
                    s << idx << "," << k << "," << v2_vec[k][idx] << "," << v2_m_vec[k][idx] << "," << v2_bg_vec[k][idx] << "," << w_v2[k][idx] << endl;
                    output2 << s.str();
                }

                Float_t v2_err2          = (sum_v2_err/sum_w_v2[idx]) * Keff_v2/(Keff_v2-1);
                Float_t v2_type_4_err2   = (sum_v2_type_4_err/sum_w_v2[idx]) * Keff_v2/(Keff_v2-1);
                Float_t v2_type_4sd_err2 = (sum_v2_type_4sd_err/sum_w_v2[idx]) * Keff_v2/(Keff_v2-1);
                Float_t v2_m_err2        = (sum_v2_m_err/sum_w_v2[idx]) * Keff_v2/(Keff_v2-1);
                v2_err          = sqrt(v2_err2/Keff_v2);
                v2_type_4_err   = sqrt(v2_type_4_err2/Keff_v2);
                v2_type_4sd_err = sqrt(v2_type_4sd_err2/Keff_v2);
                v2_m_err        = sqrt(v2_m_err2/Keff_v2);
            }
            else cout << "Idx: " << idx << ", Keff_v2: " << Keff_v2 << endl;

            if(Keff_v2_bg > 1) {
                Float_t sum_v2_bg_err = 0;

                for(UInt_t k = 0; k < samples; ++k) {
                    sum_v2_bg_err += w_v2_bg[k][idx] * pow(v2_bg_vec[k][idx]-v2_bg, 2);
                }

                Float_t v2_bg_err2 = (sum_v2_bg_err/sum_w_v2_bg[idx]) * Keff_v2_bg/(Keff_v2_bg-1);
                v2_bg_err = sqrt(v2_bg_err2/Keff_v2_bg);
            }

            if(Keff_v2_bg_4 > 1) {
                Float_t sum_v2_bg_4_err = 0;

                for(UInt_t k = 0; k < samples; ++k) {
                    sum_v2_bg_4_err += w_v2_bg_4[k][idx] * pow(v2_bg_4_vec[k][idx]-v2_bg_4, 2);
                }

                Float_t v2_bg_4_err2 = (sum_v2_bg_4_err/sum_w_v2_bg_4[idx]) * Keff_v2_bg_4/(Keff_v2_bg_4-1);
                v2_bg_4_err = sqrt(v2_bg_4_err2/Keff_v2_bg_4);
            }

            if(Keff_v2_bg_4sd > 1) {
                Float_t sum_v2_bg_4sd_err = 0;

                for(UInt_t k = 0; k < samples; ++k) {
                    sum_v2_bg_4sd_err += w_v2_bg_4sd[k][idx] * pow(v2_bg_4sd_vec[k][idx]-v2_bg_4sd, 2);
                }

                Float_t v2_bg_4sd_err2 = (sum_v2_bg_4sd_err/sum_w_v2_bg_4sd[idx]) * Keff_v2_bg_4sd/(Keff_v2_bg_4sd-1);
                v2_bg_4sd_err = sqrt(v2_bg_4sd_err2/Keff_v2_bg_4sd);
            }

            Float_t v3 = hv3[idx]->GetMean();
            Float_t Keff_v3 = (sum_w2_v3[idx]) ? sum_w_v3[idx]*sum_w_v3[idx]/sum_w2_v3[idx] : 0;
            Float_t v3_err  = -9999;

            if(Keff_v3 > 1) {
                Float_t sum_v3_err = 0;

                for(UInt_t k = 0; k < samples; ++k) {
                    sum_v3_err += w_v3[k][idx] * pow(v3_vec[k][idx]-v3, 2);
                }

                Float_t v3_err2 = (sum_v3_err/sum_w_v3[idx]) * Keff_v3/(Keff_v3-1);
                v3_err  = sqrt(v3_err2/Keff_v3);
            }
            else cout << "Idx: " << idx << ", Keff_v3: " << Keff_v3 << endl;

            s.str("");

            s << idx << "," << v2 << "," << v2_err << ","
                            << v2_type_4 << "," << v2_type_4_err << ","
                            << v2_type_4sd << "," << v2_type_4sd_err << ","
                            << v3 << "," << v3_err << ","
                            << v2_m << "," << v2_m_err << ","
                            << v2_bg << "," << v2_bg_err << ","
                            << v2_bg_4 << "," << v2_bg_4_err << ","
                            << v2_bg_4sd << "," << v2_bg_4sd_err << endl;

            output << s.str();
        }
    }
    output.close();
    output2.close();
}

void myAnalysis::finalize(const string &outputFile) {

    TFile output(outputFile.c_str(),"recreate");

    output.mkdir("v2");
    output.mkdir("v2/pi0");
    output.mkdir("v2/pi0_type_4");
    output.mkdir("v2/pi0_type_4sd");
    output.mkdir("v2/measured");
    output.mkdir("v2/background");
    output.mkdir("v2/background_4");
    output.mkdir("v3");

    for(UInt_t i = 0; i < cent_key.size(); ++i) {
        for(UInt_t j = 0; j < pt_key.size(); ++j) {
            Int_t idx = i*pt_key.size()+j;

            output.cd("v2/pi0");
            hv2[idx]->Write();

            output.cd("v2/pi0_type_4sd");
            hv2_type_4sd[idx]->Write();

            output.cd("v2/measured");
            hv2_m[idx]->Write();

            output.cd("v2/background");
            hv2_bg[idx]->Write();

            output.cd("v2/background_4");
            hv2_bg_4[idx]->Write();

            output.cd("v3");
            hv3[idx]->Write();
        }
    }

    output.Close();
    input->Close();
}

void vnAnalysis(const string &i_input,
                const string &fitStats,
                const string &outputCSV  = "vn.csv",
                const string &outputCSV2  = "vn-info.csv",
                const string &outputFile = "vn.root",
                      Int_t   anaType    = 1,
                      Int_t   samples    = 30) {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "   << i_input << endl;
    cout << "fitStats: "    << fitStats << endl;
    cout << "outputCSV: "   << outputCSV << endl;
    cout << "outputCSV2: "  << outputCSV2 << endl;
    cout << "outputFile: "  << outputFile << endl;
    cout << "anaType: "     << anaType << endl;
    cout << "Samples: "     << samples << endl;
    cout << "#############################" << endl;

    myAnalysis::cent_key = (anaType == 0) ? myAnalysis::cent_key1 : myAnalysis::cent_key2;
    myAnalysis::pt_key   = (anaType == 0) ? myAnalysis::pt_key1   : myAnalysis::pt_key2;

    if(myAnalysis::init(i_input, fitStats)) return;

    myAnalysis::process_event(samples, outputCSV, outputCSV2);
    myAnalysis::finalize(outputFile);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 3 || argc > 8){
        cout << "usage: ./vnAna inputFile fitStats [outputCSV] [outputCSV2] [outputFile] [anaType] [samples]" << endl;
        cout << "inputFile: containing list of root file paths" << endl;
        cout << "fitStats: csv file containing fit stats" << endl;
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

    if(argc >= 4) {
        outputCSV = argv[3];
    }
    if(argc >= 5) {
        outputCSV2 = argv[4];
    }
    if(argc >= 6) {
        outputFile = argv[5];
    }
    if(argc >= 7) {
        anaType = atoi(argv[6]);
    }
    if(argc >= 8) {
        samples = atoi(argv[7]);
    }

    vnAnalysis(argv[1], argv[2], outputCSV, outputCSV2, outputFile, anaType, samples);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
