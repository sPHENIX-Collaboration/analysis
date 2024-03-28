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

namespace myAnalysis {
    TChain* T;

    struct Cut {
        Float_t e1;         // min cluster energy
        Float_t e2;         // min cluster 2 energy
        Float_t asym;       // max cluster pair energy asymmetry: |E1-E2|/(E1+E2), where E1 and E2 is cluster ecore
        Float_t deltaR_min; // min cluster pair deltaR
        Float_t deltaR_max; // max cluster pair deltaR
        Float_t chi;        // max cluster chi2
    };

    vector<Cut> cuts;

    Int_t init(const string &i_input, const string &i_cuts, const string &fitStats, const string &QVecCorr, Long64_t start = 0, Long64_t end = 0);
    Int_t readFiles(const string &i_input, Long64_t start = 0, Long64_t end = 0);
    Int_t readCuts(const string &i_cuts);
    Int_t readFitStats(const string &fitStats);
    Int_t readQVectorCorrection(const string &i_input);
    void init_hists();

    void process_event(Float_t z_max = 10, Long64_t start = 0, Long64_t end = 0);
    void finalize(const string &i_output = "test.root");

    vector<string> cent_key = {"40-60", "20-40", "0-20"};
    vector<string> pt_key   = {"2-2.5", "2.5-3", "3-3.5", "3.5-4", "4-4.5", "4.5-5"};

    TH1F* pt_dum_vec   = new TH1F("pt_dum_vec","",6,2,5);
    TH1F* cent_dum_vec = new TH1F("cent_dum_vec","", 3, 0, 0.6);

    // keep track of low and high pi0 mass values to filter on for the computation of the v2
    vector<pair<Float_t,Float_t>> pi0_mass_mu_sigma(cent_key.size()*pt_key.size()); // (mu, sigma)

    map<pair<string,string>, vector<TH1F*>> hPi0Mass; // hPi0Mass[make_pair(cent,pt)][i], for accessing diphoton invariant mass hist of i-th cut of cent,pt

    map<pair<string,string>, TH2F*> h2DeltaRVsMass;
    map<pair<string,string>, TH2F*> h2AsymVsMass;
    map<pair<string,string>, TH2F*> h2DeltaRVsAsym;
    map<string, TH1F*>              hDiphotonPt;
    // v2
    vector<map<string, TH1F*>>              hQQ2;
    vector<map<pair<string,string>, TH1F*>> hqQ2;
    vector<map<pair<string,string>, TH1F*>> hqQ2_bg;
    vector<map<pair<string,string>, TH1F*>> hqQ2_bg_3;
    vector<map<pair<string,string>, TH1F*>> hqQ2_bg_4;
    vector<map<pair<string,string>, TH1F*>> hqQ2_bg_left;
    // v3
    vector<map<string, TH1F*>>              hQQ3;
    vector<map<pair<string,string>, TH1F*>> hqQ3;
    vector<map<pair<string,string>, TH1F*>> hqQ3_bg;
    vector<map<pair<string,string>, TH1F*>> hqQ3_bg_left;

    map<pair<string,string>, TH1F*> hNPi0;
    map<pair<string,string>, TH2F*> h2Pi0EtaPhi;
    map<pair<string,string>, TH2F*> h2Pi0EtaPhiv2;

    Int_t   bins_pi0_mass = 56;
    Float_t hpi0_mass_min = 0;
    Float_t hpi0_mass_max = 0.7;

    Int_t   bins_pt = 500;
    Float_t hpt_min = 0;
    Float_t hpt_max = 20;

    Int_t   bins_deltaR = 100;
    Float_t hdeltaR_min = 0;
    Float_t hdeltaR_max = 4;

    Int_t   bins_asym = 100;
    Float_t hasym_min = 0;
    Float_t hasym_max = 1;

    Int_t   bins_Q = 200;
    Float_t Q_min  = -1;
    Float_t Q_max  = 1;

    Int_t   bins_phi = 128; // Bin width on the order of blocks
    Float_t phi_min  = -M_PI;
    Float_t phi_max  = M_PI;

    Int_t   bins_eta = 48; // Bin width on the order of blocks
    Float_t eta_min  = -1.152;
    Float_t eta_max  = 1.152;

    Int_t   bins_npi0 = 60;
    Float_t npi0_min  = 0;
    Float_t npi0_max  = 60;

    Bool_t do_vn_calc = false;
    Int_t cut_num = 0;
    Int_t subsamples  = 1;

    // First Order Correction
    // v2
    Float_t Q2_S_x_avg[3] = {0};
    Float_t Q2_S_y_avg[3] = {0};
    Float_t Q2_N_x_avg[3] = {0};
    Float_t Q2_N_y_avg[3] = {0};
    // v3
    Float_t Q3_S_x_avg[3] = {0};
    Float_t Q3_S_y_avg[3] = {0};
    Float_t Q3_N_x_avg[3] = {0};
    Float_t Q3_N_y_avg[3] = {0};

    // Second Order Correction
    // v2
    Float_t X2_S[3][2][2] = {0}; // [cent][row][col], off diagonal entries are the same
    Float_t X2_N[3][2][2] = {0}; // [cent][row][col], off diagonal entries are the same
    // v3
    Float_t X3_S[3][2][2] = {0}; // [cent][row][col], off diagonal entries are the same
    Float_t X3_N[3][2][2] = {0}; // [cent][row][col], off diagonal entries are the same
}

Int_t myAnalysis::init(const string &i_input, const string &i_cuts, const string& fitStats, const string& QVecCorr, Long64_t start, Long64_t end) {
    T = new TChain("T");
    T->Add(i_input.c_str());

    Int_t ret;
    // ret = readFiles(i_input, start, end);
    // if(ret != 0) return ret;

    ret = readCuts(i_cuts);
    if(ret != 0) return ret;

    init_hists();

    if(do_vn_calc) {
        ret = readFitStats(fitStats);
        if(ret) return ret;
        ret = readQVectorCorrection(QVecCorr);
    }

    return ret;
}

Int_t myAnalysis::readFiles(const string &i_input, Long64_t start, Long64_t end) {
    T = new TChain("T");

    // Create an input stream
    std::ifstream file(i_input);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open root file list: " << i_input << endl;
        return 1;
    }

    cout << "Reading in TNtuples" << endl;
    cout << "======================================" << endl;
    string line;
    ULong_t entries = 0;
    while (std::getline(file, line)) {
        // Process each line here
        T->Add(line.c_str());

        cout << line << ", entries: " << T->GetEntries()-entries << endl;
        entries = T->GetEntries();

        if(end && entries > end) break;
    }
    cout << "======================================" << endl;
    cout << "Total Entries: " << T->GetEntries() << endl;
    cout << "======================================" << endl;

    // Close the file
    file.close();

    return 0;
}

Int_t myAnalysis::readCuts(const string &i_cuts) {
    // Create an input stream
    std::ifstream file(i_cuts);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open cuts file: " << i_cuts << endl;
        return 1;
    }

    string line;

    // skip header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        string cell;
        char comma;

        Cut cut;

        if (lineStream >> cut.e1         >> comma
                       >> cut.e2         >> comma
                       >> cut.asym       >> comma
                       >> cut.deltaR_min >> comma
                       >> cut.deltaR_max >> comma
                       >> cut.chi) {
            cuts.push_back(cut);
        }
        else {
            cerr << "Failed to parse line: " << line << endl;
            return 1;
        }
    }

    // Close the file
    file.close();

    cout << "Cuts" << endl;
    UInt_t i = 0;
    for(auto cut : cuts) {
        if(i == cut_num) cout << "################################################################" << endl;

        cout << left << "e1: "           << setw(8) << cut.e1
                     << ", e2: "         << setw(8) << cut.e2
                     << ", asym: "       << setw(8) << cut.asym
                     << ", deltaR_min: " << setw(8) << cut.deltaR_min
                     << ", deltaR_max: " << setw(8) << cut.deltaR_max
                     << ", chi: "        << setw(8) << cut.chi << endl;

        if(i == cut_num) cout << "################################################################" << endl;
        ++i;
    }

    return 0;
}

Int_t myAnalysis::readFitStats(const string &fitStats) {
    // Create an input stream
    std::ifstream file(fitStats);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open fit-stats file: " << fitStats << endl;
        return 1;
    }

    // 7: mu
    // 9: sigma
    Int_t col[2] = {7,9};

    std::string line;
    Int_t idx = 0;

    // skip header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;

        Int_t colIdx = 0;
        while (std::getline(ss, token, ',')) { // Assuming comma-separated values
            if (colIdx == col[0]) {
                pi0_mass_mu_sigma[idx].first = stof(token);
            }
            else if (colIdx == col[1]) {
                pi0_mass_mu_sigma[idx].second = stof(token);
            }
            colIdx++;
        }
        ++idx;
    }

    cout << endl;
    for(Int_t i = 0; i < cent_key.size(); ++i) {
        cout << "cent: " << cent_key[i] << endl;

        for(Int_t j = 0; j < pt_key.size(); ++j) {
            Int_t idx = i*pt_key.size()+j;

            cout << "pt: " << pt_key[j]
                 << ", mu: "            << pi0_mass_mu_sigma[idx].first
                 << ", sigma: "         << pi0_mass_mu_sigma[idx].second
                 << ", mu-2*sigma: "    << pi0_mass_mu_sigma[idx].first-2*pi0_mass_mu_sigma[idx].second
                 << ", mu+2*sigma: "    << pi0_mass_mu_sigma[idx].first+2*pi0_mass_mu_sigma[idx].second
                 << ", mu+3*sigma: "    << pi0_mass_mu_sigma[idx].first+3*pi0_mass_mu_sigma[idx].second
                 << endl;
        }
        cout << endl;
    }

    return 0;
}

Int_t myAnalysis::readQVectorCorrection(const string &i_input) {
    // Create an input stream
    std::ifstream file(i_input);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open Q-vec-corr file: " << i_input << endl;
        return 1;
    }

    string line;

    // get line and discard header
    std::getline(file, line);

    Int_t i = 0;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        string cell;
        char comma;

        if (!(lineStream >> Q2_S_x_avg[i] >> comma
                         >> Q2_S_y_avg[i] >> comma
                         >> Q2_N_x_avg[i] >> comma
                         >> Q2_N_y_avg[i] >> comma
                         >> X2_S[i][0][0] >> comma
                         >> X2_S[i][0][1] >> comma
                         >> X2_S[i][1][1] >> comma
                         >> X2_N[i][0][0] >> comma
                         >> X2_N[i][0][1] >> comma
                         >> X2_N[i][1][1] >> comma
                         >> Q3_S_x_avg[i] >> comma
                         >> Q3_S_y_avg[i] >> comma
                         >> Q3_N_x_avg[i] >> comma
                         >> Q3_N_y_avg[i] >> comma
                         >> X3_S[i][0][0] >> comma
                         >> X3_S[i][0][1] >> comma
                         >> X3_S[i][1][1] >> comma
                         >> X3_N[i][0][0] >> comma
                         >> X3_N[i][0][1] >> comma
                         >> X3_N[i][1][1])) {

            cerr << "Failed to parse line: " << line << endl;
            return 1;
        }
        ++i;
    }

    // Close the file
    file.close();

    cout << "Q2 Vector Corr Factors" << endl;
    for(Int_t j = 0; j < i; ++j) {
        cout << "Cent: " << cent_key[j] << endl;
        cout << left << "Q2_S_x_avg: "   << setw(8) << Q2_S_x_avg[j]
                     << ", Q2_S_y_avg: " << setw(8) << Q2_S_y_avg[j] << endl;

        cout << left << "Q2_N_x_avg: "   << setw(8) << Q2_N_x_avg[j]
                     << ", Q2_N_y_avg: " << setw(8) << Q2_N_y_avg[j] << endl;

        cout << left << "X2_S_00: "   << setw(8) << X2_S[j][0][0]
                     << ", X2_S_01: " << setw(8) << X2_S[j][0][1]
                     << ", X2_S_11: " << setw(8) << X2_S[j][1][1] << endl;

        cout << left << "X2_N_00: "   << setw(8) << X2_N[j][0][0]
                     << ", X2_N_01: " << setw(8) << X2_N[j][0][1]
                     << ", X2_N_11: " << setw(8) << X2_N[j][1][1] << endl;

        cout << endl;

        cout << left << "Q3_S_x_avg: "   << setw(8) << Q3_S_x_avg[j]
                     << ", Q3_S_y_avg: " << setw(8) << Q3_S_y_avg[j] << endl;

        cout << left << "Q3_N_x_avg: "   << setw(8) << Q3_N_x_avg[j]
                     << ", Q3_N_y_avg: " << setw(8) << Q3_N_y_avg[j] << endl;

        cout << left << "X3_S_00: "   << setw(8) << X3_S[j][0][0]
                     << ", X3_S_01: " << setw(8) << X3_S[j][0][1]
                     << ", X3_S_11: " << setw(8) << X3_S[j][1][1] << endl;

        cout << left << "X3_N_00: "   << setw(8) << X3_N[j][0][0]
                     << ", X3_N_01: " << setw(8) << X3_N[j][0][1]
                     << ", X3_N_11: " << setw(8) << X3_N[j][1][1] << endl;

        cout << endl;
    }

    return 0;
}

void myAnalysis::init_hists() {

    stringstream s;
    stringstream t;
    // create a pi0 mass hist for each cut
    // create QA plots for each centrality/pt bin
    for(Int_t i = 0; i < cent_key.size(); ++i) {

        string suffix_title =  "Centrality: " + cent_key[i] + "%";
        hDiphotonPt[cent_key[i]] = new TH1F(("hDiphotonPt_"+cent_key[i]).c_str(), ("Diphoton p_{T}, " + suffix_title +"; p_{T} [GeV]; Counts").c_str(), bins_pt, hpt_min, hpt_max);

        for(Int_t j = 0; j < pt_key.size(); ++j) {

            Int_t idx = i*pt_key.size()+j;

            pair<string,string> key = make_pair(cent_key[i],pt_key[j]);
            string suffix = "_"+cent_key[i]+"_"+pt_key[j];
            suffix_title = "Centrality: " + cent_key[i] + "%, Diphoton p_{T}: " + pt_key[j] + " GeV";

            h2Pi0EtaPhi[key] = new TH2F(("h2Pi0EtaPhi_"+to_string(idx)).c_str(), ("#pi_{0}, " + suffix_title + "; #eta; #phi").c_str(), bins_eta, eta_min, eta_max, bins_phi, phi_min, phi_max);
            h2Pi0EtaPhiv2[key] = new TH2F(("h2Pi0EtaPhiv2_"+to_string(idx)).c_str(), ("#pi_{0}, " + suffix_title + "; #eta; #phi").c_str(), bins_eta, eta_min, eta_max, bins_phi, phi_min, phi_max);

            h2DeltaRVsMass[key] = new TH2F(("h2DeltaRVsMass_"+to_string(idx)).c_str(),
                                            ("#Delta R vs Diphoton Invariant Mass, " + suffix_title +"; Mass [GeV]; #Delta R").c_str(),
                                            bins_pi0_mass, hpi0_mass_min, hpi0_mass_max,
                                            bins_deltaR, hdeltaR_min, hdeltaR_max);

            h2AsymVsMass[key] = new TH2F(("h2AsymVsMass_"+to_string(idx)).c_str(),
                                            ("Cluster Energy Asymmetry vs Diphoton Invariant Mass, " + suffix_title +"; Mass [GeV]; Energy Asymmetry").c_str(),
                                            bins_pi0_mass, hpi0_mass_min, hpi0_mass_max,
                                            bins_asym, hasym_min, hasym_max);

            h2DeltaRVsAsym[key] = new TH2F(("h2DeltaRVsAsym_"+to_string(idx)).c_str(),
                                            ("#Delta R vs Cluster Energy Asymmetry, " + suffix_title +"; Energy Asymmetry; #Delta R").c_str(),
                                            bins_asym, hasym_min, hasym_max,
                                            bins_deltaR, hdeltaR_min, hdeltaR_max);

            hNPi0[key] = new TH1F(("hNPi0_"+to_string(idx)).c_str(), ("#pi_{0} Counts, " + suffix_title +"; # of #pi_{0} per event; Counts").c_str(), bins_npi0, npi0_min, npi0_max);

            vector<TH1F*> h;
            for (auto cut : cuts) {
                s.str("");
                t.str("");
                s << "hPi0Mass" << suffix << "_" << cut.e1 << "_" << cut.e2 << "_" << cut.asym << "_" << cut.deltaR_min << "_" << cut.deltaR_max << "_" << cut.chi;
                t << "Diphoton: E_{1} #geq " << cut.e1 << ", E_{2} #geq " << cut.e2 << ", Asym < " << cut.asym
                  << ", " << cut.deltaR_min << " #leq #Delta R < " << cut.deltaR_max
                  << ", #chi^{2} < " << cut.chi << "; Invariant Mass [GeV]; Counts";
                auto hist = new TH1F(s.str().c_str(), t.str().c_str(), bins_pi0_mass, hpi0_mass_min, hpi0_mass_max);
                hist->Sumw2();
                h.push_back(hist);
            }
            hPi0Mass[key] = h;
        }
    }

    if(do_vn_calc) {
        for(Int_t k = 0; k < subsamples; ++k) {
            map<string, TH1F*> hQQ2_dummy;
            map<pair<string,string>, TH1F*> hqQ2_dummy;
            map<pair<string,string>, TH1F*> hqQ2_bg_dummy;
            map<pair<string,string>, TH1F*> hqQ2_bg_3_dummy;
            map<pair<string,string>, TH1F*> hqQ2_bg_4_dummy;
            map<pair<string,string>, TH1F*> hqQ2_bg_left_dummy;

            map<string, TH1F*> hQQ3_dummy;
            map<pair<string,string>, TH1F*> hqQ3_dummy;
            map<pair<string,string>, TH1F*> hqQ3_bg_dummy;
            map<pair<string,string>, TH1F*> hqQ3_bg_left_dummy;

            for(Int_t i = 0; i < cent_key.size(); ++i) {

                string suffix_title =  "Centrality: " + cent_key[i] + "%";
                // v2
                hQQ2_dummy[cent_key[i]] = new TH1F(("hQQ2_"+to_string(k)+"_"+cent_key[i]).c_str(), ("QQ2, " + suffix_title +"; QQ2; Counts").c_str(), bins_Q, Q_min, Q_max);

                // v3
                hQQ3_dummy[cent_key[i]] = new TH1F(("hQQ3_"+to_string(k)+"_"+cent_key[i]).c_str(), ("QQ3, " + suffix_title +"; QQ3; Counts").c_str(), bins_Q, Q_min, Q_max);

                for(Int_t j = 0; j < pt_key.size(); ++j) {
                    Int_t idx = i*pt_key.size()+j;

                    pair<string,string> key = make_pair(cent_key[i],pt_key[j]);
                    string suffix = "_"+cent_key[i]+"_"+pt_key[j];
                    suffix_title = "Centrality: " + cent_key[i] + "%, Diphoton p_{T}: " + pt_key[j] + " GeV";

                    hqQ2_dummy[key]         = new TH1F(("hqQ2_"+to_string(k)+"_"+to_string(idx)).c_str(), ("qQ2, " + suffix_title + "; qQ2; Counts").c_str(), bins_Q, Q_min, Q_max);
                    hqQ2_bg_dummy[key]      = new TH1F(("hqQ2_bg_"+to_string(k)+"_"+to_string(idx)).c_str(), ("qQ2, " + suffix_title + "; qQ2; Counts").c_str(), bins_Q, Q_min, Q_max);
                    hqQ2_bg_3_dummy[key]    = new TH1F(("hqQ2_bg_3_"+to_string(k)+"_"+to_string(idx)).c_str(), ("qQ2, " + suffix_title + "; qQ2; Counts").c_str(), bins_Q, Q_min, Q_max);
                    hqQ2_bg_4_dummy[key]    = new TH1F(("hqQ2_bg_4_"+to_string(k)+"_"+to_string(idx)).c_str(), ("qQ2, " + suffix_title + "; qQ2; Counts").c_str(), bins_Q, Q_min, Q_max);
                    hqQ2_bg_left_dummy[key] = new TH1F(("hqQ2_bg_left_"+to_string(k)+"_"+to_string(idx)).c_str(), ("qQ2, " + suffix_title + "; qQ2; Counts").c_str(), bins_Q, Q_min, Q_max);

                    hqQ3_dummy[key]         = new TH1F(("hqQ3_"+to_string(k)+"_"+to_string(idx)).c_str(), ("qQ3, " + suffix_title + "; qQ3; Counts").c_str(), bins_Q, Q_min, Q_max);
                    hqQ3_bg_dummy[key]      = new TH1F(("hqQ3_bg_"+to_string(k)+"_"+to_string(idx)).c_str(), ("qQ3, " + suffix_title + "; qQ3; Counts").c_str(), bins_Q, Q_min, Q_max);
                    hqQ3_bg_left_dummy[key] = new TH1F(("hqQ3_bg_left_"+to_string(k)+"_"+to_string(idx)).c_str(), ("qQ3, " + suffix_title + "; qQ3; Counts").c_str(), bins_Q, Q_min, Q_max);
                }
            }

            hQQ2.push_back(hQQ2_dummy);
            hqQ2.push_back(hqQ2_dummy);
            hqQ2_bg.push_back(hqQ2_bg_dummy);
            hqQ2_bg_3.push_back(hqQ2_bg_3_dummy);
            hqQ2_bg_4.push_back(hqQ2_bg_4_dummy);
            hqQ2_bg_left.push_back(hqQ2_bg_left_dummy);

            hQQ3.push_back(hQQ3_dummy);
            hqQ3.push_back(hqQ3_dummy);
            hqQ3_bg.push_back(hqQ3_bg_dummy);
            hqQ3_bg_left.push_back(hqQ3_bg_left_dummy);
        }
    }
}

void myAnalysis::process_event(Float_t z_max, Long64_t start, Long64_t end) {
    cout << "======================================" << endl;
    cout << "Begin Process Event" << endl;
    cout << "======================================" << endl;

    // Disable everything...
    T->SetBranchStatus("*", false);
    // ...but the branch we need
    // T->SetBranchStatus("run",       true);
    // T->SetBranchStatus("event",     true);
    // T->SetBranchStatus("totalMBD",  true);
    T->SetBranchStatus("centrality", true);
    T->SetBranchStatus("vtx_z",      true);
    T->SetBranchStatus("pi0_mass",   true);
    T->SetBranchStatus("pi0_pt",     true);
    T->SetBranchStatus("asym",       true);
    T->SetBranchStatus("deltaR",     true);
    T->SetBranchStatus("ecore1",     true);
    T->SetBranchStatus("ecore2",     true);
    T->SetBranchStatus("chi2_max",   true);

    if(do_vn_calc) {
        T->SetBranchStatus("Q2_S_x",  true);
        T->SetBranchStatus("Q2_S_y",  true);
        T->SetBranchStatus("Q2_N_x",  true);
        T->SetBranchStatus("Q2_N_y",  true);

        T->SetBranchStatus("Q3_S_x",  true);
        T->SetBranchStatus("Q3_S_y",  true);
        T->SetBranchStatus("Q3_N_x",  true);
        T->SetBranchStatus("Q3_N_y",  true);

        T->SetBranchStatus("pi0_phi", true);
        T->SetBranchStatus("pi0_eta", true);
    }

    // Int_t   run;
    // Int_t   event;
    Float_t totalMBD;
    Float_t cent;
    Float_t z;
    vector<Float_t>* pi0_mass   = 0;
    vector<Float_t>* pi0_pt     = 0;
    vector<Float_t>* pi0_asym   = 0;
    vector<Float_t>* pi0_deltaR = 0;
    vector<Float_t>* ecore1     = 0;
    vector<Float_t>* ecore2     = 0;
    vector<Float_t>* chi2_max   = 0;
    vector<Bool_t>*  isFarNorth = 0;

    Float_t Q2_S_x;
    Float_t Q2_S_y;
    Float_t Q2_N_x;
    Float_t Q2_N_y;
    Float_t Q3_S_x;
    Float_t Q3_S_y;
    Float_t Q3_N_x;
    Float_t Q3_N_y;
    vector<Float_t>* pi0_phi    = 0;
    vector<Float_t>* pi0_eta    = 0;

    // T->SetBranchAddress("run",       &run);
    // T->SetBranchAddress("event",     &event);
    // T->SetBranchAddress("totalMBD",   &totalMBD);
    T->SetBranchAddress("centrality", &cent);
    T->SetBranchAddress("vtx_z",      &z);
    T->SetBranchAddress("pi0_mass",   &pi0_mass);
    T->SetBranchAddress("pi0_pt",     &pi0_pt);
    T->SetBranchAddress("asym",       &pi0_asym);
    T->SetBranchAddress("deltaR",     &pi0_deltaR);
    T->SetBranchAddress("ecore1",     &ecore1);
    T->SetBranchAddress("ecore2",     &ecore2);
    T->SetBranchAddress("chi2_max",   &chi2_max);
    T->SetBranchAddress("isFarNorth", &isFarNorth);

    if(do_vn_calc) {
        T->SetBranchAddress("Q2_S_x", &Q2_S_x);
        T->SetBranchAddress("Q2_S_y", &Q2_S_y);
        T->SetBranchAddress("Q2_N_x", &Q2_N_x);
        T->SetBranchAddress("Q2_N_y", &Q2_N_y);

        T->SetBranchAddress("Q3_S_x", &Q3_S_x);
        T->SetBranchAddress("Q3_S_y", &Q3_S_y);
        T->SetBranchAddress("Q3_N_x", &Q3_N_x);
        T->SetBranchAddress("Q3_N_y", &Q3_N_y);

        T->SetBranchAddress("pi0_phi", &pi0_phi);
        T->SetBranchAddress("pi0_eta", &pi0_eta);
    }

    end = (end) ? min(end, T->GetEntries()-1) : T->GetEntries()-1;

    UInt_t evt_ctr[cent_key.size()] = {0};

    Float_t QQ2_min    = 9999;
    Float_t qQ2_min    = 9999;
    Float_t qQ2_bg_min = 9999;
    Float_t QQ3_min    = 9999;
    Float_t qQ3_min    = 9999;
    Float_t qQ3_bg_min = 9999;
    Float_t eta_min   = 9999;
    UInt_t  npi0_max  = 0;
    Float_t QQ2_max    = 0;
    Float_t qQ2_max    = 0;
    Float_t qQ2_bg_max = 0;
    Float_t QQ3_max    = 0;
    Float_t qQ3_max    = 0;
    Float_t qQ3_bg_max = 0;
    Float_t eta_max   = 0;

    cout << "Events to process: " << end-start+1 << endl;

    // loop over each event
    for (Long64_t i = start; i <= end; ++i) {
        if(i%100 == 0) cout << "Progress: " << (i-start)*100./(end-start) << "%" << endl;
        // Load the data for the given tree entry
        T->GetEntry(i);

        // ensure the z vertex is within range
        if(abs(z) >= z_max) continue;

        // Int_t cent_idx = cent_dum_vec->FindBin(totalMBD)-1;
        Int_t cent_idx = cent_dum_vec->FindBin(cent)-1;

        // check if centrality is found in one of the specified bins
        if(cent_idx < 0 || cent_idx >= 3) continue;

        // need to reverse this index since we want to match cent_key
        cent_idx = cent_key.size() - cent_idx - 1;

        // assign each event to a sample
        UInt_t k = evt_ctr[cent_idx]%subsamples;

        ++evt_ctr[cent_idx];

        if(do_vn_calc) {
            // v2
            // Apply first and second order correction to the Q vectors
            //============================================
            // compute first order correction
            Float_t Q2_S_x_temp = Q2_S_x - Q2_S_x_avg[cent_idx];
            Float_t Q2_S_y_temp = Q2_S_y - Q2_S_y_avg[cent_idx];
            Float_t Q2_N_x_temp = Q2_N_x - Q2_N_x_avg[cent_idx];
            Float_t Q2_N_y_temp = Q2_N_y - Q2_N_y_avg[cent_idx];

            // compute second order correction
            Q2_S_x = X2_S[cent_idx][0][0]*Q2_S_x_temp+X2_S[cent_idx][0][1]*Q2_S_y_temp;
            Q2_S_y = X2_S[cent_idx][0][1]*Q2_S_x_temp+X2_S[cent_idx][1][1]*Q2_S_y_temp;
            Q2_N_x = X2_N[cent_idx][0][0]*Q2_N_x_temp+X2_N[cent_idx][0][1]*Q2_N_y_temp;
            Q2_N_y = X2_N[cent_idx][0][1]*Q2_N_x_temp+X2_N[cent_idx][1][1]*Q2_N_y_temp;
            //============================================

            Float_t QQ2 = Q2_S_x*Q2_N_x + Q2_S_y*Q2_N_y;
            QQ2_min = min(QQ2_min, QQ2);
            QQ2_max = max(QQ2_max, QQ2);

            hQQ2[k][cent_key[cent_idx]]->Fill(QQ2);

            // v3
            // Apply first and second order correction to the Q vectors
            //============================================
            // compute first order correction
            Float_t Q3_S_x_temp = Q3_S_x - Q3_S_x_avg[cent_idx];
            Float_t Q3_S_y_temp = Q3_S_y - Q3_S_y_avg[cent_idx];
            Float_t Q3_N_x_temp = Q3_N_x - Q3_N_x_avg[cent_idx];
            Float_t Q3_N_y_temp = Q3_N_y - Q3_N_y_avg[cent_idx];

            // compute second order correction
            Q3_S_x = X3_S[cent_idx][0][0]*Q3_S_x_temp+X3_S[cent_idx][0][1]*Q3_S_y_temp;
            Q3_S_y = X3_S[cent_idx][0][1]*Q3_S_x_temp+X3_S[cent_idx][1][1]*Q3_S_y_temp;
            Q3_N_x = X3_N[cent_idx][0][0]*Q3_N_x_temp+X3_N[cent_idx][0][1]*Q3_N_y_temp;
            Q3_N_y = X3_N[cent_idx][0][1]*Q3_N_x_temp+X3_N[cent_idx][1][1]*Q3_N_y_temp;
            //============================================

            Float_t QQ3 = Q3_S_x*Q3_N_x + Q3_S_y*Q3_N_y;
            QQ3_min = min(QQ3_min, QQ3);
            QQ3_max = max(QQ3_max, QQ3);

            hQQ3[k][cent_key[cent_idx]]->Fill(QQ3);
        }

        UInt_t pi0_ctr[cent_key.size()*pt_key.size()]       = {0};
        UInt_t bg_ctr[cent_key.size()*pt_key.size()]        = {0};
        UInt_t bg_3_ctr[cent_key.size()*pt_key.size()]      = {0};
        UInt_t bg_4_ctr[cent_key.size()*pt_key.size()]      = {0};
        UInt_t bg_left_ctr[cent_key.size()*pt_key.size()]   = {0};
        Float_t qQ2[cent_key.size()*pt_key.size()]          = {0};
        Float_t qQ2_bg[cent_key.size()*pt_key.size()]       = {0};
        Float_t qQ2_bg_3[cent_key.size()*pt_key.size()]     = {0};
        Float_t qQ2_bg_4[cent_key.size()*pt_key.size()]     = {0};
        Float_t qQ2_bg_left[cent_key.size()*pt_key.size()]  = {0};
        Float_t qQ3[cent_key.size()*pt_key.size()]          = {0};
        Float_t qQ3_bg[cent_key.size()*pt_key.size()]       = {0};
        Float_t qQ3_bg_left[cent_key.size()*pt_key.size()]  = {0};

        // loop over all diphoton candidates
        for(UInt_t j = 0; j < pi0_mass->size(); ++j) {
            Float_t pi0_pt_val = pi0_pt->at(j);
            Int_t pt_idx = pt_dum_vec->FindBin(pi0_pt_val)-1;

            hDiphotonPt[cent_key[cent_idx]]->Fill(pi0_pt_val);
            // check if pt is found in one of the specified bins
            if(pt_idx < 0 || pt_idx >= 6) continue;

            pair<string,string> key = make_pair(cent_key[cent_idx], pt_key[pt_idx]);

            Float_t pi0_mass_val   = pi0_mass->at(j);
            Float_t asym_val       = pi0_asym->at(j);
            Float_t deltaR_val     = pi0_deltaR->at(j);
            Float_t ecore1_val     = min(ecore1->at(j), ecore2->at(j));
            Float_t ecore2_val     = max(ecore1->at(j), ecore2->at(j));
            Float_t chi2_max_val   = chi2_max->at(j);
            Bool_t  isFarNorth_val = isFarNorth->at(j);

            Float_t pi0_phi_val = 0;
            Float_t pi0_eta_val = 0;

            Float_t Q2_x = 0;
            Float_t Q2_y = 0;

            Float_t Q3_x = 0;
            Float_t Q3_y = 0;

            Double_t q2_x    = 0;
            Double_t q2_y    = 0;
            Double_t qQ2_val = 0;

            Double_t q3_x    = 0;
            Double_t q3_y    = 0;
            Double_t qQ3_val = 0;

            if(do_vn_calc) {
                pi0_phi_val   = pi0_phi->at(j);
                pi0_eta_val   = pi0_eta->at(j);

                Q2_x = (pi0_eta_val < 0) ? Q2_N_x : Q2_S_x;
                Q2_y = (pi0_eta_val < 0) ? Q2_N_y : Q2_S_y;

                q2_x = cos(2*pi0_phi_val);
                q2_y = sin(2*pi0_phi_val);
                qQ2_val = q2_x*Q2_x + q2_y*Q2_y;

                Q3_x = (pi0_eta_val < 0) ? Q3_N_x : Q3_S_x;
                Q3_y = (pi0_eta_val < 0) ? Q3_N_y : Q3_S_y;

                q3_x = cos(3*pi0_phi_val);
                q3_y = sin(3*pi0_phi_val);
                qQ3_val = q3_x*Q3_x + q3_y*Q3_y;
            }

            h2AsymVsMass[key]->Fill(pi0_mass_val, asym_val);
            h2DeltaRVsMass[key]->Fill(pi0_mass_val, deltaR_val);
            h2DeltaRVsAsym[key]->Fill(asym_val, deltaR_val);

            Int_t idx = cent_idx*pt_key.size()+pt_idx;

            // compute mu+-2*sd of the pi0 mass to select diphotons as pi0 candidates
            Float_t pi0_mass_low  = pi0_mass_mu_sigma[idx].first-2*pi0_mass_mu_sigma[idx].second;
            Float_t pi0_mass_high = pi0_mass_mu_sigma[idx].first+2*pi0_mass_mu_sigma[idx].second;

            // compute mu+3*sd of the pi0 mass to select background diphotons
            Float_t bg_min = pi0_mass_mu_sigma[idx].first+3*pi0_mass_mu_sigma[idx].second;
            Float_t bg_3_max = 0.3;
            Float_t bg_4_max = 0.4;
            Float_t bg_max   = 0.5; // setting max at 0.5 GeV to avoid the eta

            for(Int_t c = 0; c < cuts.size(); ++c) {
                if(ecore1_val >= cuts[c].e1         && ecore2_val >= cuts[c].e2        && asym_val     < cuts[c].asym &&
                   deltaR_val >= cuts[c].deltaR_min && deltaR_val < cuts[c].deltaR_max && chi2_max_val < cuts[c].chi) {
                    hPi0Mass[key][c]->Fill(pi0_mass_val);

                    // fill in qQ for the background to the left of the pi0 peak
                    if(c == cut_num && do_vn_calc && pi0_mass_val < bg_min) {
                        ++bg_left_ctr[idx];
                        qQ2_bg_left[idx] += qQ2_val;
                        qQ3_bg_left[idx] += qQ3_val;
                    }

                    // fill in qQ for the background
                    if(c == cut_num && do_vn_calc && pi0_mass_val >= bg_min && pi0_mass_val < bg_max) {
                        ++bg_ctr[idx];
                        qQ2_bg[idx] += qQ2_val;
                        qQ3_bg[idx] += qQ3_val;

                        if(pi0_mass_val < bg_3_max) {
                            ++bg_3_ctr[idx];
                            qQ2_bg_3[idx] += qQ2_val;
                        }
                        if(pi0_mass_val < bg_4_max) {
                            ++bg_4_ctr[idx];
                            qQ2_bg_4[idx] += qQ2_val;
                        }
                    }
                    // fill in the qQ for the signal+background region
                    // do this for only one of the cuts for which we have signal bound information
                    if(c == cut_num && do_vn_calc && pi0_mass_val >= pi0_mass_low && pi0_mass_val < pi0_mass_high) {
                        ++pi0_ctr[idx];
                        qQ2[idx] += qQ2_val;
                        qQ3[idx] += qQ3_val;
                        h2Pi0EtaPhi[key]->Fill(pi0_eta_val, pi0_phi_val);
                        if(!isFarNorth_val) h2Pi0EtaPhiv2[key]->Fill(pi0_eta_val, pi0_phi_val);
                        eta_min = min(eta_min, pi0_eta_val);
                        eta_max = max(eta_max, pi0_eta_val);
                    }
                }
            }
        }

        if (do_vn_calc) {
          // loop over each centrality and pT bin
          for (UInt_t j = 0; j < cent_key.size(); ++j) {
            for (UInt_t p = 0; p < pt_key.size(); ++p) {
              Int_t idx = j * pt_key.size() + p;
              pair<string, string> key = make_pair(cent_key[j], pt_key[p]);

              npi0_max = max(npi0_max, pi0_ctr[idx]);
              if (pi0_ctr[idx]) hNPi0[key]->Fill(pi0_ctr[idx]);

              // v2
              // compute qQ for the background to the left of the pi0 peak
              qQ2_bg_left[idx] = (bg_left_ctr[idx]) ? qQ2_bg_left[idx] / bg_left_ctr[idx] : 0;
              qQ2_bg_min = min(qQ2_bg_min, qQ2_bg_left[idx]);
              qQ2_bg_max = max(qQ2_bg_max, qQ2_bg_left[idx]);

              if (qQ2_bg_left[idx]) hqQ2_bg_left[k][key]->Fill(qQ2_bg_left[idx], bg_left_ctr[idx]);

              // compute qQ for the background
              qQ2_bg[idx] = (bg_ctr[idx]) ? qQ2_bg[idx] / bg_ctr[idx] : 0;
              qQ2_bg_min = min(qQ2_bg_min, qQ2_bg[idx]);
              qQ2_bg_max = max(qQ2_bg_max, qQ2_bg[idx]);

              if (qQ2_bg[idx]) hqQ2_bg[k][key]->Fill(qQ2_bg[idx], bg_ctr[idx]);

              // compute qQ for the background
              qQ2_bg_3[idx] = (bg_3_ctr[idx]) ? qQ2_bg_3[idx] / bg_3_ctr[idx] : 0;
              qQ2_bg_min = min(qQ2_bg_min, qQ2_bg_3[idx]);
              qQ2_bg_max = max(qQ2_bg_max, qQ2_bg_3[idx]);

              if (qQ2_bg_3[idx]) hqQ2_bg_3[k][key]->Fill(qQ2_bg_3[idx], bg_3_ctr[idx]);

              // compute qQ for the background
              qQ2_bg_4[idx] = (bg_4_ctr[idx]) ? qQ2_bg_4[idx] / bg_4_ctr[idx] : 0;
              qQ2_bg_min = min(qQ2_bg_min, qQ2_bg_4[idx]);
              qQ2_bg_max = max(qQ2_bg_max, qQ2_bg_4[idx]);

              if (qQ2_bg_4[idx]) hqQ2_bg_4[k][key]->Fill(qQ2_bg_4[idx], bg_4_ctr[idx]);

              // compute qQ for the pi0 candiates
              qQ2[idx] = (pi0_ctr[idx]) ? qQ2[idx] / pi0_ctr[idx] : 0;
              qQ2_min = min(qQ2_min, qQ2[idx]);
              qQ2_max = max(qQ2_max, qQ2[idx]);

              if (qQ2[idx]) hqQ2[k][key]->Fill(qQ2[idx], pi0_ctr[idx]);

              // v3
              // compute qQ for the background to the left of the pi0 peak
              qQ3_bg_left[idx] = (bg_left_ctr[idx]) ? qQ3_bg_left[idx] / bg_left_ctr[idx] : 0;
              qQ3_bg_min = min(qQ3_bg_min, qQ3_bg_left[idx]);
              qQ3_bg_max = max(qQ3_bg_max, qQ3_bg_left[idx]);

              if (qQ3_bg_left[idx]) hqQ3_bg_left[k][key]->Fill(qQ3_bg_left[idx], bg_left_ctr[idx]);

              // compute qQ for the background
              qQ3_bg[idx] = (bg_ctr[idx]) ? qQ3_bg[idx] / bg_ctr[idx] : 0;
              qQ3_bg_min = min(qQ3_bg_min, qQ3_bg[idx]);
              qQ3_bg_max = max(qQ3_bg_max, qQ3_bg[idx]);

              if (qQ3_bg[idx]) hqQ3_bg[k][key]->Fill(qQ3_bg[idx], bg_ctr[idx]);

              // compute qQ for the pi0 candiates
              qQ3[idx] = (pi0_ctr[idx]) ? qQ3[idx] / pi0_ctr[idx] : 0;
              qQ3_min = min(qQ3_min, qQ3[idx]);
              qQ3_max = max(qQ3_max, qQ3[idx]);

              if (qQ3[idx]) hqQ3[k][key]->Fill(qQ3[idx], pi0_ctr[idx]);
            }
          }
        }
    }

    cout << endl;
    UInt_t events = 0;
    for(Int_t i = 0; i < cent_key.size(); ++i) {
        cout << "Cent: "     << cent_key[i]
             << ", Events: " << evt_ctr[i] << endl;
        events += evt_ctr[i];
    }

    cout << endl;
    cout << "Events Processed: " << events << ", " << events*100./(end-start) << " %" << endl;
    cout << endl;
    cout << "QQ2 min: " << QQ2_min << ", QQ2 max: " << QQ2_max << endl;
    cout << "qQ2 min: " << qQ2_min << ", qQ2 max: " << qQ2_max << endl;
    cout << "qQ2 bg min: " << qQ2_bg_min << ", qQ2 bg max: " << qQ2_bg_max << endl;
    cout << endl;
    cout << "QQ3 min: " << QQ3_min << ", QQ3 max: " << QQ3_max << endl;
    cout << "qQ3 min: " << qQ3_min << ", qQ3 max: " << qQ3_max << endl;
    cout << "qQ3 bg min: " << qQ3_bg_min << ", qQ3 bg max: " << qQ3_bg_max << endl;
    cout << endl;
    cout << "pi0 eta min: " << eta_min << ", pi0 eta max: " << eta_max << endl;
    cout << "Max Pi0s per event: " << npi0_max << endl;
    cout << "Finish Process Event" << endl;
}


void myAnalysis::finalize(const string &i_output) {
    TFile output(i_output.c_str(),"recreate");
    output.mkdir("results");
    output.mkdir("QA/hDiphotonPt");
    output.mkdir("QA/h2DeltaRVsMass");
    output.mkdir("QA/h2AsymVsMass");
    output.mkdir("QA/h2DeltaRVsAsym");

    if(do_vn_calc) {

        for(Int_t k = 0; k < subsamples; ++k) {
            string prefix = "vn/"+to_string(k);

            output.mkdir((prefix+"/QQ2").c_str());
            output.mkdir((prefix+"/qQ2").c_str());
            output.mkdir((prefix+"/qQ2_bg").c_str());
            output.mkdir((prefix+"/qQ2_bg_3").c_str());
            output.mkdir((prefix+"/qQ2_bg_4").c_str());
            output.mkdir((prefix+"/qQ2_bg_left").c_str());

            output.mkdir((prefix+"/QQ3").c_str());
            output.mkdir((prefix+"/qQ3").c_str());
            output.mkdir((prefix+"/qQ3_bg").c_str());
            output.mkdir((prefix+"/qQ3_bg_left").c_str());
        }

        output.mkdir("QA/h2Pi0EtaPhi");
        output.mkdir("QA/h2Pi0EtaPhiv2");
        output.mkdir("QA/hNPi0");
    }

    for(auto cent : cent_key) {
        output.cd("QA/hDiphotonPt");
        hDiphotonPt[cent]->Write();

        if(do_vn_calc) {
            for(Int_t k = 0; k < subsamples; ++k) {
                string prefix = "vn/"+to_string(k);
                output.cd((prefix+"/QQ2").c_str());
                hQQ2[k][cent]->Write();

                output.cd((prefix+"/QQ3").c_str());
                hQQ3[k][cent]->Write();
            }
        }

        for(auto pt : pt_key) {
            pair<string,string> key = make_pair(cent, pt);

            output.cd("QA/h2DeltaRVsMass");
            h2DeltaRVsMass[key]->Write();

            output.cd("QA/h2AsymVsMass");
            h2AsymVsMass[key]->Write();

            output.cd("QA/h2DeltaRVsAsym");
            h2DeltaRVsAsym[key]->Write();

            if(do_vn_calc) {
                for(Int_t k = 0; k < subsamples; ++k) {
                    string prefix = "vn/"+to_string(k);

                    output.cd((prefix+"/qQ2").c_str());
                    hqQ2[k][key]->Write();

                    output.cd((prefix+"/qQ2_bg").c_str());
                    hqQ2_bg[k][key]->Write();

                    output.cd((prefix+"/qQ2_bg_3").c_str());
                    hqQ2_bg_3[k][key]->Write();

                    output.cd((prefix+"/qQ2_bg_4").c_str());
                    hqQ2_bg_4[k][key]->Write();

                    output.cd((prefix+"/qQ2_bg_left").c_str());
                    hqQ2_bg_left[k][key]->Write();

                    output.cd((prefix+"/qQ3").c_str());
                    hqQ3[k][key]->Write();

                    output.cd((prefix+"/qQ3_bg").c_str());
                    hqQ3_bg[k][key]->Write();

                    output.cd((prefix+"/qQ3_bg_left").c_str());
                    hqQ3_bg_left[k][key]->Write();
                }
                output.cd("QA/hNPi0");
                hNPi0[key]->Write();

                output.cd("QA/h2Pi0EtaPhi");
                h2Pi0EtaPhi[key]->Write();

                output.cd("QA/h2Pi0EtaPhiv2");
                h2Pi0EtaPhiv2[key]->Write();
            }

            output.mkdir(("results/"+cent+"/"+pt).c_str());
            output.cd(("results/"+cent+"/"+pt).c_str());

            for(Int_t i = 0; i < cuts.size(); ++i) {
                hPi0Mass[key][i]->Write();
            }
        }
    }

    output.Close();
}

void pi0Analysis(const string &i_input,
                 const string &i_cuts,
                 Float_t       z           = 10, /*cm*/
                 const string &i_output    = "test.root",
                 Bool_t        do_vn_calc  = false,
                 const string &fitStats    = "",
                 const string &QVecCorr    = "",
                 Int_t         subsamples  = 1,
                 Int_t         cut_num     = 0,
                 Long64_t      start       = 0,
                 Long64_t      end         = 0) {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "   << i_input << endl;
    cout << "Cuts: "        << i_cuts << endl;
    cout << "z: "           << z << endl;
    cout << "outputFile: "  << i_output << endl;
    cout << "do_vn_calc: "  << do_vn_calc << endl;
    cout << "fitStats: "    << fitStats << endl;
    cout << "QVecCorr: "    << QVecCorr << endl;
    cout << "Subsamples: "  << subsamples << endl;
    cout << "cut_num: " << cut_num << endl;
    cout << "start: "       << start << endl;
    cout << "end: "         << end << endl;
    cout << "#############################" << endl;

    myAnalysis::do_vn_calc = do_vn_calc;
    myAnalysis::cut_num = cut_num;
    myAnalysis::subsamples = subsamples;

    Int_t ret = myAnalysis::init(i_input, i_cuts, fitStats, QVecCorr, start, end);
    if(ret != 0) return;

    myAnalysis::process_event(z, start, end);
    myAnalysis::finalize(i_output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 3 || argc > 12){
        cout << "usage: ./pi0Ana inputFile cuts [z] [outputFile] [do_vn_calc] [fitStats] [QVecCorr] [subsamples] [cut_num] [start] [end] " << endl;
        cout << "inputFile: containing list of root file paths" << endl;
        cout << "cuts: csv file containing cuts" << endl;
        cout << "z: z-vertex cut. Default: 10 cm. Range: 0 to 30 cm." << endl;
        cout << "outputFile: location of output file. Default: test.root." << endl;
        cout << "do_vn_calc: Do vn calculations. Default: False" << endl;
        cout << "fitStats: csv file containing fit stats" << endl;
        cout << "QVecCorr: csv file containing Q vector corrections" << endl;
        cout << "subsamples: number of subsamples for the vn analysis. Default: 1." << endl;
        cout << "cut_num: the specific diphoton cut to use for the vn analysis. Default: 0." << endl;
        cout << "start: start event number. Default: 0." << endl;
        cout << "end: end event number. Default: 0. (to run over all entries)." << endl;
        return 1;
    }

    Float_t  z           = 10;
    string   outputFile  = "test.root";
    Bool_t   do_vn_calc  = false;
    string   fitStats    = "";
    string   QVecCorr    = "";
    Int_t    subsamples  = 1;
    Int_t    cut_num     = 0;
    Long64_t start       = 0;
    Long64_t end         = 0;

    if(argc >= 4) {
        z = atof(argv[3]);
    }
    if(argc >= 5) {
        outputFile = argv[4];
    }
    if(argc >= 6) {
        do_vn_calc = atoi(argv[5]);
    }
    if(argc >= 7) {
        fitStats = argv[6];
    }
    if(argc >= 8) {
        QVecCorr = argv[7];
    }
    if(argc >= 9) {
        subsamples = atoi(argv[8]);
    }
    if(argc >= 10) {
        cut_num = atoi(argv[9]);
    }
    if(argc >= 11) {
        start = atol(argv[10]);
    }
    if(argc >= 12) {
        end = atol(argv[11]);
    }

    // ensure that 0 <= start <= end
    if(start > end || start < 0) {
        cerr << "Error: Invalid start, end provided! " << endl;
        return 1;
    }

    pi0Analysis(argv[1], argv[2], z, outputFile, do_vn_calc, fitStats, QVecCorr, subsamples, cut_num, start, end);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
