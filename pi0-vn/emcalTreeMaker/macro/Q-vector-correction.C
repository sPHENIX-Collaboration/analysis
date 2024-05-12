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

    struct Event {
        Float_t Q2_S_x;
        Float_t Q2_S_y;
        Float_t Q2_N_x;
        Float_t Q2_N_y;
        Float_t Q3_S_x;
        Float_t Q3_S_y;
        Float_t Q3_N_x;
        Float_t Q3_N_y;
        Float_t cent;
        Float_t b;
        Float_t z;
    };

    vector<Event> events;

    Int_t init(const string &i_input, Long64_t start = 0, Long64_t end = 0);
    Int_t readFiles(const string &i_input, Long64_t start = 0, Long64_t end = 0);
    void init_hists();

    void process_event(Float_t z_max = 10, Long64_t start = 0, Long64_t end = 0);
    void finalize(const string &i_output = "test.root", const string &i_output_csv = "test.csv");

    Int_t anaType;
    Bool_t isSim;

    vector<string> cent_key;
    vector<string> cent_key1 = {"40-60", "20-40", "0-20"};
    vector<string> cent_key2 = {"50-60", "40-50", "30-40","20-30","10-20","0-10"};

    // Impact parameter bin edges taken from: https://wiki.sphenix.bnl.gov/index.php/MDC2_2022
    vector<string>  b_key1 = {"9.71-11.84", "6.81-9.71", "0-6.81"}; /*fm*/
    vector<string>  b_key2 = {"10.81-11.84","9.71-10.81","8.40-9.71","6.81-8.40","4.88-6.81","0-4.88"}; /*fm*/

    vector<Float_t> b_bin;
    vector<Float_t> b_bin1 = {0, 6.81, 9.71, 11.84};
    vector<Float_t> b_bin2 = {0, 4.88, 6.81, 8.4, 9.71, 10.81, 11.84};

    // TH1F* cent_dum_vec = new TH1F("cent_dum_vec","", 3, 0, 0.6);
    TH1F* cent_dum_vec;

    vector<vector<TH1F*>> hPsi2_S; // [cent][order of correction]
    vector<vector<TH1F*>> hPsi2_N; // [cent][order of correction]
    vector<vector<TH1F*>> hPsi3_S; // [cent][order of correction]
    vector<vector<TH1F*>> hPsi3_N; // [cent][order of correction]

    UInt_t bins_psi  = 100;
    Float_t low_psi  = -M_PI;
    Float_t high_psi = M_PI;

    // First Order Correction
    vector<Float_t> Q2_S_x_avg;
    vector<Float_t> Q2_S_y_avg;
    vector<Float_t> Q2_N_x_avg;
    vector<Float_t> Q2_N_y_avg;

    vector<Float_t> Q3_S_x_avg;
    vector<Float_t> Q3_S_y_avg;
    vector<Float_t> Q3_N_x_avg;
    vector<Float_t> Q3_N_y_avg;

    // Second Order Correction
    vector<vector<vector<Float_t>>> X2_S; // [cent][row][col]
    vector<vector<vector<Float_t>>> X2_N; // [cent][row][col]

    vector<vector<vector<Float_t>>> X3_S; // [cent][row][col]
    vector<vector<vector<Float_t>>> X3_N; // [cent][row][col]
}

void myAnalysis::init_hists() {
    for (Int_t i = 0; i < cent_key.size(); ++i) {
        for (Int_t j = 0; j < 3; ++j) {
            string name = "hPsi2_S_"+cent_key[i]+"_"+to_string(j);
            string title = to_string(j) + "-th Order Corrected 2#Psi_{2}^{S}, Centrality: " + cent_key[i] + "; 2#Psi_{2}^{S}; Counts";
            hPsi2_S[i][j] = new TH1F((name).c_str(), title.c_str(), bins_psi, low_psi, high_psi);

            name = "hPsi2_N_"+cent_key[i]+"_"+to_string(j);
            title = to_string(j) + "-th Order Corrected 2#Psi_{2}^{N}, Centrality: " + cent_key[i] + "; 2#Psi_{2}^{N}; Counts";
            hPsi2_N[i][j] = new TH1F((name).c_str(), title.c_str(), bins_psi, low_psi, high_psi);

            name = "hPsi3_S_"+cent_key[i]+"_"+to_string(j);
            title = to_string(j) + "-th Order Corrected 3#Psi_{3}^{S}, Centrality: " + cent_key[i] + "; 3#Psi_{3}^{S}; Counts";
            hPsi3_S[i][j] = new TH1F((name).c_str(), title.c_str(), bins_psi, low_psi, high_psi);

            name = "hPsi3_N_"+cent_key[i]+"_"+to_string(j);
            title = to_string(j) + "-th Order Corrected 3#Psi_{3}^{N}, Centrality: " + cent_key[i] + "; 3#Psi_{3}^{N}; Counts";
            hPsi3_N[i][j] = new TH1F((name).c_str(), title.c_str(), bins_psi, low_psi, high_psi);
       }
    }
}

Int_t myAnalysis::init(const string &i_input, Long64_t start, Long64_t end) {
    Int_t ret = readFiles(i_input, start, end);
    if(ret != 0) return ret;

    if(isSim) {
        cent_key = (anaType == 0) ? b_key1 : b_key2;
        b_bin    = (anaType == 0) ? b_bin1 : b_bin2;
    }
    else {
        cent_key = (anaType == 0) ? cent_key1 : cent_key2;
    }

    cent_dum_vec = (isSim) ? new TH1F("cent_dum_vec","", cent_key.size(), b_bin.data())
                           : new TH1F("cent_dum_vec","", cent_key.size(), 0, 0.6);

    X2_S.resize(cent_key.size(), vector<vector<Float_t>>(2, vector<Float_t>(2)));

    hPsi2_S.resize(cent_key.size(), vector<TH1F*>(3)); // [cent][order of correction]
    hPsi2_N.resize(cent_key.size(), vector<TH1F*>(3)); // [cent][order of correction]
    hPsi3_S.resize(cent_key.size(), vector<TH1F*>(3)); // [cent][order of correction]
    hPsi3_N.resize(cent_key.size(), vector<TH1F*>(3)); // [cent][order of correction]

    // First Order Correction
    Q2_S_x_avg.resize(cent_key.size());
    Q2_S_y_avg.resize(cent_key.size());
    Q2_N_x_avg.resize(cent_key.size());
    Q2_N_y_avg.resize(cent_key.size());

    Q3_S_x_avg.resize(cent_key.size());
    Q3_S_y_avg.resize(cent_key.size());
    Q3_N_x_avg.resize(cent_key.size());
    Q3_N_y_avg.resize(cent_key.size());

    // Second Order Correction
    X2_S.resize(cent_key.size(), vector<vector<Float_t>>(2, vector<Float_t>(2))); // [cent][row][col]
    X2_N.resize(cent_key.size(), vector<vector<Float_t>>(2, vector<Float_t>(2))); // [cent][row][col]

    X3_S.resize(cent_key.size(), vector<vector<Float_t>>(2, vector<Float_t>(2))); // [cent][row][col]
    X3_N.resize(cent_key.size(), vector<vector<Float_t>>(2, vector<Float_t>(2))); // [cent][row][col]

    init_hists();

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

    cout << "Reading in TTree" << endl;
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

void myAnalysis::process_event(Float_t z_max, Long64_t start, Long64_t end) {
    cout << "======================================" << endl;
    cout << "Begin Process Event" << endl;
    cout << "======================================" << endl;

    // Disable everything...
    T->SetBranchStatus("*", false);
    // ...but the branch we need
    T->SetBranchStatus("Q2_S_x",    true);
    T->SetBranchStatus("Q2_S_y",    true);
    T->SetBranchStatus("Q2_N_x",    true);
    T->SetBranchStatus("Q2_N_y",    true);
    T->SetBranchStatus("Q3_S_x",    true);
    T->SetBranchStatus("Q3_S_y",    true);
    T->SetBranchStatus("Q3_N_x",    true);
    T->SetBranchStatus("Q3_N_y",    true);
    T->SetBranchStatus("vtx_z",    true);
    // T->SetBranchStatus("totalMBD", true);
    T->SetBranchStatus("centrality", true);

    if(isSim) {
        T->SetBranchStatus("b", true);
    }

    Float_t Q2_S_x;
    Float_t Q2_S_y;
    Float_t Q2_N_x;
    Float_t Q2_N_y;
    Float_t Q3_S_x;
    Float_t Q3_S_y;
    Float_t Q3_N_x;
    Float_t Q3_N_y;
    Float_t totalMBD;
    Float_t cent;
    Float_t b;
    Float_t z;

    // event counter
    vector<UInt_t> evt_ctr(cent_key.size());

    // First Order Correction
    vector<Float_t> Q2_S_x_corr(cent_key.size());
    vector<Float_t> Q2_S_y_corr(cent_key.size());
    vector<Float_t> Q2_N_x_corr(cent_key.size());
    vector<Float_t> Q2_N_y_corr(cent_key.size());

    vector<Float_t> Q3_S_x_corr(cent_key.size());
    vector<Float_t> Q3_S_y_corr(cent_key.size());
    vector<Float_t> Q3_N_x_corr(cent_key.size());
    vector<Float_t> Q3_N_y_corr(cent_key.size());

    // Second Order Correction
    vector<Float_t> Q2_S_xx_avg(cent_key.size());
    vector<Float_t> Q2_S_yy_avg(cent_key.size());
    vector<Float_t> Q2_S_xy_avg(cent_key.size());
    vector<Float_t> Q2_N_xx_avg(cent_key.size());
    vector<Float_t> Q2_N_yy_avg(cent_key.size());
    vector<Float_t> Q2_N_xy_avg(cent_key.size());

    vector<Float_t> Q2_S_x_corr2(cent_key.size());
    vector<Float_t> Q2_S_y_corr2(cent_key.size());
    vector<Float_t> Q2_N_x_corr2(cent_key.size());
    vector<Float_t> Q2_N_y_corr2(cent_key.size());

    vector<Float_t> D2_S(cent_key.size());
    vector<Float_t> D2_N(cent_key.size());
    vector<Float_t> N2_S(cent_key.size());
    vector<Float_t> N2_N(cent_key.size());

    vector<Float_t> Q3_S_xx_avg(cent_key.size());
    vector<Float_t> Q3_S_yy_avg(cent_key.size());
    vector<Float_t> Q3_S_xy_avg(cent_key.size());
    vector<Float_t> Q3_N_xx_avg(cent_key.size());
    vector<Float_t> Q3_N_yy_avg(cent_key.size());
    vector<Float_t> Q3_N_xy_avg(cent_key.size());

    vector<Float_t> Q3_S_x_corr2(cent_key.size());
    vector<Float_t> Q3_S_y_corr2(cent_key.size());
    vector<Float_t> Q3_N_x_corr2(cent_key.size());
    vector<Float_t> Q3_N_y_corr2(cent_key.size());

    vector<Float_t> D3_S(cent_key.size());
    vector<Float_t> D3_N(cent_key.size());
    vector<Float_t> N3_S(cent_key.size());
    vector<Float_t> N3_N(cent_key.size());

    T->SetBranchAddress("Q2_S_x", &Q2_S_x);
    T->SetBranchAddress("Q2_S_y", &Q2_S_y);
    T->SetBranchAddress("Q2_N_x", &Q2_N_x);
    T->SetBranchAddress("Q2_N_y", &Q2_N_y);
    T->SetBranchAddress("Q3_S_x", &Q3_S_x);
    T->SetBranchAddress("Q3_S_y", &Q3_S_y);
    T->SetBranchAddress("Q3_N_x", &Q3_N_x);
    T->SetBranchAddress("Q3_N_y", &Q3_N_y);
    // T->SetBranchAddress("totalMBD",   &totalMBD);
    T->SetBranchAddress("centrality", &cent);
    T->SetBranchAddress("vtx_z", &z);

    if(isSim) {
        T->SetBranchAddress("b", &b);
    }

    end = (end) ? min(end, T->GetEntries()-1) : T->GetEntries()-1;

    cout << "Loading Events" << endl;
    for (Long64_t i = start; i <= end; ++i) {
        if(i%100000 == 0) cout << "Progress: " << (i-start)*100./(end-start) << "%" << endl;
        // Load the data for the given tree entry
        T->GetEntry(i);

        // ensure the z vertex is within range
        if(abs(z) >= z_max) continue;

        // Int_t j = cent_dum_vec->FindBin(totalMBD)-1;
        Int_t j = (isSim) ? cent_dum_vec->FindBin(b)-1 : cent_dum_vec->FindBin(cent)-1;

        // check if centrality is found in one of the specified bins
        if(j < 0 || j >= cent_key.size()) continue;

        Event event;

        event.Q2_S_x = Q2_S_x;
        event.Q2_S_y = Q2_S_y;
        event.Q2_N_x = Q2_N_x;
        event.Q2_N_y = Q2_N_y;
        event.Q3_S_x = Q3_S_x;
        event.Q3_S_y = Q3_S_y;
        event.Q3_N_x = Q3_N_x;
        event.Q3_N_y = Q3_N_y;
        event.cent  = cent;
        event.b     = b;
        event.z     = z;

        events.push_back(event);
    }
    cout << "Finished Loading Events" << endl;

    cout << "Events to process: " << end-start+1 << endl;
    // loop over each event
    // first order correction
    for (Long64_t i = 0; i < events.size(); ++i) {
        if(i%100000 == 0) cout << "Progress: " << (i-start)*100./(end-start) << "%" << endl;

        // check if centrality is found in one of the specified bins
        Int_t j = (isSim) ? cent_dum_vec->FindBin(events[i].b)-1
                          : cent_dum_vec->FindBin(events[i].cent)-1;

        // need to reverse this index since we want to match cent_key
        j = cent_key.size() - j - 1;

        ++evt_ctr[j];

        Q2_S_x_avg[j] += events[i].Q2_S_x;
        Q2_S_y_avg[j] += events[i].Q2_S_y;
        Q2_N_x_avg[j] += events[i].Q2_N_x;
        Q2_N_y_avg[j] += events[i].Q2_N_y;

        Q3_S_x_avg[j] += events[i].Q3_S_x;
        Q3_S_y_avg[j] += events[i].Q3_S_y;
        Q3_N_x_avg[j] += events[i].Q3_N_x;
        Q3_N_y_avg[j] += events[i].Q3_N_y;
    }

    cout << endl;
    cout << "First Order Correction" << endl;
    // normalize to compute the averages
    for (Int_t i = 0; i < cent_key.size(); ++i) {
        Q2_S_x_avg[i] /= evt_ctr[i];
        Q2_S_y_avg[i] /= evt_ctr[i];
        Q2_N_x_avg[i] /= evt_ctr[i];
        Q2_N_y_avg[i] /= evt_ctr[i];

        cout << "Cent: " << cent_key[i] << ", Events: " << evt_ctr[i] << endl
             << "Q2_S_x_avg: "   << Q2_S_x_avg[i]
             << ", Q2_S_y_avg: " << Q2_S_y_avg[i]
             << ", Q2_N_x_avg: " << Q2_N_x_avg[i]
             << ", Q2_N_y_avg: " << Q2_N_y_avg[i] << endl;

        cout << endl;

        Q3_S_x_avg[i] /= evt_ctr[i];
        Q3_S_y_avg[i] /= evt_ctr[i];
        Q3_N_x_avg[i] /= evt_ctr[i];
        Q3_N_y_avg[i] /= evt_ctr[i];

        cout << "Q3_S_x_avg: "   << Q3_S_x_avg[i]
             << ", Q3_S_y_avg: " << Q3_S_y_avg[i]
             << ", Q3_N_x_avg: " << Q3_N_x_avg[i]
             << ", Q3_N_y_avg: " << Q3_N_y_avg[i] << endl;
    }

    // loop over each event
    // apply first order correction
    // compute second order correction
    for (Long64_t i = 0; i < events.size(); ++i) {
        if(i%100000 == 0) cout << "Progress: " << (i-start)*100./(end-start) << "%" << endl;

        // check if centrality is found in one of the specified bins
        Int_t j = (isSim) ? cent_dum_vec->FindBin(events[i].b)-1
                          : cent_dum_vec->FindBin(events[i].cent)-1;

        // need to reverse this index since we want to match cent_key
        j = cent_key.size() - j - 1;

        // compute the first order correction
        Q2_S_x_corr[j] = events[i].Q2_S_x - Q2_S_x_avg[j];
        Q2_S_y_corr[j] = events[i].Q2_S_y - Q2_S_y_avg[j];
        Q2_N_x_corr[j] = events[i].Q2_N_x - Q2_N_x_avg[j];
        Q2_N_y_corr[j] = events[i].Q2_N_y - Q2_N_y_avg[j];

        Q3_S_x_corr[j] = events[i].Q3_S_x - Q3_S_x_avg[j];
        Q3_S_y_corr[j] = events[i].Q3_S_y - Q3_S_y_avg[j];
        Q3_N_x_corr[j] = events[i].Q3_N_x - Q3_N_x_avg[j];
        Q3_N_y_corr[j] = events[i].Q3_N_y - Q3_N_y_avg[j];

        // compute averages required for second order correction
        Q2_S_xx_avg[j]  += Q2_S_x_corr[j]*Q2_S_x_corr[j];
        Q2_S_yy_avg[j]  += Q2_S_y_corr[j]*Q2_S_y_corr[j];
        Q2_S_xy_avg[j] += Q2_S_x_corr[j]*Q2_S_y_corr[j];
        Q2_N_xx_avg[j]  += Q2_N_x_corr[j]*Q2_N_x_corr[j];
        Q2_N_yy_avg[j]  += Q2_N_y_corr[j]*Q2_N_y_corr[j];
        Q2_N_xy_avg[j] += Q2_N_x_corr[j]*Q2_N_y_corr[j];

        Q3_S_xx_avg[j]  += Q3_S_x_corr[j]*Q3_S_x_corr[j];
        Q3_S_yy_avg[j]  += Q3_S_y_corr[j]*Q3_S_y_corr[j];
        Q3_S_xy_avg[j] += Q3_S_x_corr[j]*Q3_S_y_corr[j];
        Q3_N_xx_avg[j]  += Q3_N_x_corr[j]*Q3_N_x_corr[j];
        Q3_N_yy_avg[j]  += Q3_N_y_corr[j]*Q3_N_y_corr[j];
        Q3_N_xy_avg[j] += Q3_N_x_corr[j]*Q3_N_y_corr[j];
    }

    cout << endl;
    cout << "Second Order Correction" << endl;
    // normalize to compute the averages
    for (Int_t i = 0; i < cent_key.size(); ++i) {
        Q2_S_xx_avg[i] /= evt_ctr[i];
        Q2_S_yy_avg[i] /= evt_ctr[i];
        Q2_S_xy_avg[i] /= evt_ctr[i];
        Q2_N_xx_avg[i] /= evt_ctr[i];
        Q2_N_yy_avg[i] /= evt_ctr[i];
        Q2_N_xy_avg[i] /= evt_ctr[i];

        D2_S[i] = sqrt(Q2_S_xx_avg[i]*Q2_S_yy_avg[i] - Q2_S_xy_avg[i]*Q2_S_xy_avg[i]);
        D2_N[i] = sqrt(Q2_N_xx_avg[i]*Q2_N_yy_avg[i] - Q2_N_xy_avg[i]*Q2_N_xy_avg[i]);

        N2_S[i] = D2_S[i]*(Q2_S_xx_avg[i]+Q2_S_yy_avg[i]+2*D2_S[i]);
        N2_N[i] = D2_N[i]*(Q2_N_xx_avg[i]+Q2_N_yy_avg[i]+2*D2_N[i]);

        // Compute matrix elements
        X2_S[i][0][0] = 1/sqrt(N2_S[i])*(Q2_S_yy_avg[i]+D2_S[i]);
        X2_S[i][0][1] = -1/sqrt(N2_S[i])*(Q2_S_xy_avg[i]);
        X2_S[i][1][0] = X2_S[i][0][1];
        X2_S[i][1][1] = 1/sqrt(N2_S[i])*(Q2_S_xx_avg[i]+D2_S[i]);

        X2_N[i][0][0] = 1/sqrt(N2_N[i])*(Q2_N_yy_avg[i]+D2_N[i]);
        X2_N[i][0][1] = -1/sqrt(N2_N[i])*(Q2_N_xy_avg[i]);
        X2_N[i][1][0] = X2_N[i][0][1];
        X2_N[i][1][1] = 1/sqrt(N2_N[i])*(Q2_N_xx_avg[i]+D2_N[i]);

        cout << "Cent: "          << cent_key[i] << ", Events: " << evt_ctr[i] << endl
             << "Q2_S_xx_avg: "    << Q2_S_xx_avg[i]
             << ", Q2_S_yy_avg: "  << Q2_S_yy_avg[i]
             << ", Q2_S_xy_avg: " << Q2_S_xy_avg[i] << endl
             << "Q2_N_xx_avg: "    << Q2_N_xx_avg[i]
             << ", Q2_N_yy_avg: "  << Q2_N_yy_avg[i]
             << ", Q2_N_xy_avg: " << Q2_N_xy_avg[i] << endl
             << "D2_S: " << D2_S[i] << ", D2_N: " << D2_N[i] << endl
             << "N2_S: " << N2_S[i] << ", N2_N: " << N2_N[i] << endl
             << "X2_S_00: "   << X2_S[i][0][0]
             << ", X2_S_01: " << X2_S[i][0][1]
             << ", X2_S_11: " << X2_S[i][1][1] << endl
             << "X2_N_00: "   << X2_N[i][0][0]
             << ", X2_N_01: " << X2_N[i][0][1]
             << ", X2_N_11: " << X2_N[i][1][1] << endl;
        cout << endl;

        Q3_S_xx_avg[i] /= evt_ctr[i];
        Q3_S_yy_avg[i] /= evt_ctr[i];
        Q3_S_xy_avg[i] /= evt_ctr[i];
        Q3_N_xx_avg[i] /= evt_ctr[i];
        Q3_N_yy_avg[i] /= evt_ctr[i];
        Q3_N_xy_avg[i] /= evt_ctr[i];

        D3_S[i] = sqrt(Q3_S_xx_avg[i]*Q3_S_yy_avg[i] - Q3_S_xy_avg[i]*Q3_S_xy_avg[i]);
        D3_N[i] = sqrt(Q3_N_xx_avg[i]*Q3_N_yy_avg[i] - Q3_N_xy_avg[i]*Q3_N_xy_avg[i]);

        N3_S[i] = D3_S[i]*(Q3_S_xx_avg[i]+Q3_S_yy_avg[i]+2*D3_S[i]);
        N3_N[i] = D3_N[i]*(Q3_N_xx_avg[i]+Q3_N_yy_avg[i]+2*D3_N[i]);

        // Compute matrix elements
        X3_S[i][0][0] = 1/sqrt(N3_S[i])*(Q3_S_yy_avg[i]+D3_S[i]);
        X3_S[i][0][1] = -1/sqrt(N3_S[i])*(Q3_S_xy_avg[i]);
        X3_S[i][1][0] = X3_S[i][0][1];
        X3_S[i][1][1] = 1/sqrt(N3_S[i])*(Q3_S_xx_avg[i]+D3_S[i]);

        X3_N[i][0][0] = 1/sqrt(N3_N[i])*(Q3_N_yy_avg[i]+D3_N[i]);
        X3_N[i][0][1] = -1/sqrt(N3_N[i])*(Q3_N_xy_avg[i]);
        X3_N[i][1][0] = X3_N[i][0][1];
        X3_N[i][1][1] = 1/sqrt(N3_N[i])*(Q3_N_xx_avg[i]+D3_N[i]);

        cout << "Q3_S_xx_avg: "    << Q3_S_xx_avg[i]
             << ", Q3_S_yy_avg: "  << Q3_S_yy_avg[i]
             << ", Q3_S_xy_avg: " << Q3_S_xy_avg[i] << endl
             << "Q3_N_xx_avg: "    << Q3_N_xx_avg[i]
             << ", Q3_N_yy_avg: "  << Q3_N_yy_avg[i]
             << ", Q3_N_xy_avg: " << Q3_N_xy_avg[i] << endl
             << "D3_S: " << D3_S[i] << ", D3_N: " << D3_N[i] << endl
             << "N3_S: " << N3_S[i] << ", N3_N: " << N3_N[i] << endl
             << "X3_S_00: "   << X3_S[i][0][0]
             << ", X3_S_01: " << X3_S[i][0][1]
             << ", X3_S_11: " << X3_S[i][1][1] << endl
             << "X3_N_00: "   << X3_N[i][0][0]
             << ", X3_N_01: " << X3_N[i][0][1]
             << ", X3_N_11: " << X3_N[i][1][1] << endl;
        cout << endl;
    }

    // loop over each event
    // apply second order correction
    for (Long64_t i = 0; i < events.size(); ++i) {
        if(i%100000 == 0) cout << "Progress: " << (i-start)*100./(end-start) << "%" << endl;

        // check if centrality is found in one of the specified bins
        Int_t j = (isSim) ? cent_dum_vec->FindBin(events[i].b)-1
                          : cent_dum_vec->FindBin(events[i].cent)-1;

        // need to reverse this index since we want to match cent_key
        j = cent_key.size() - j - 1;

        // compute first order correction
        Q2_S_x_corr[j] = events[i].Q2_S_x - Q2_S_x_avg[j];
        Q2_S_y_corr[j] = events[i].Q2_S_y - Q2_S_y_avg[j];
        Q2_N_x_corr[j] = events[i].Q2_N_x - Q2_N_x_avg[j];
        Q2_N_y_corr[j] = events[i].Q2_N_y - Q2_N_y_avg[j];

        // compute second order correction
        Q2_S_x_corr2[j] = X2_S[j][0][0]*Q2_S_x_corr[j]+X2_S[j][0][1]*Q2_S_y_corr[j];
        Q2_S_y_corr2[j] = X2_S[j][1][0]*Q2_S_x_corr[j]+X2_S[j][1][1]*Q2_S_y_corr[j];
        Q2_N_x_corr2[j] = X2_N[j][0][0]*Q2_N_x_corr[j]+X2_N[j][0][1]*Q2_N_y_corr[j];
        Q2_N_y_corr2[j] = X2_N[j][1][0]*Q2_N_x_corr[j]+X2_N[j][1][1]*Q2_N_y_corr[j];

        // compute Psi
        // no correction
        Float_t psi = atan2(events[i].Q2_S_y, events[i].Q2_S_x);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi2_S[j][0]->Fill(psi);

        psi = atan2(events[i].Q2_N_y, events[i].Q2_N_x);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi2_N[j][0]->Fill(psi);

        // order 1
        psi = atan2(Q2_S_y_corr[j], Q2_S_x_corr[j]);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi2_S[j][1]->Fill(psi);

        psi = atan2(Q2_N_y_corr[j], Q2_N_x_corr[j]);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi2_N[j][1]->Fill(psi);

        // order 2
        psi = atan2(Q2_S_y_corr2[j], Q2_S_x_corr2[j]);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi2_S[j][2]->Fill(psi);

        psi = atan2(Q2_N_y_corr2[j], Q2_N_x_corr2[j]);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi2_N[j][2]->Fill(psi);

        // compute first order correction
        Q3_S_x_corr[j] = events[i].Q3_S_x - Q3_S_x_avg[j];
        Q3_S_y_corr[j] = events[i].Q3_S_y - Q3_S_y_avg[j];
        Q3_N_x_corr[j] = events[i].Q3_N_x - Q3_N_x_avg[j];
        Q3_N_y_corr[j] = events[i].Q3_N_y - Q3_N_y_avg[j];

        // compute second order correction
        Q3_S_x_corr2[j] = X3_S[j][0][0]*Q3_S_x_corr[j]+X3_S[j][0][1]*Q3_S_y_corr[j];
        Q3_S_y_corr2[j] = X3_S[j][1][0]*Q3_S_x_corr[j]+X3_S[j][1][1]*Q3_S_y_corr[j];
        Q3_N_x_corr2[j] = X3_N[j][0][0]*Q3_N_x_corr[j]+X3_N[j][0][1]*Q3_N_y_corr[j];
        Q3_N_y_corr2[j] = X3_N[j][1][0]*Q3_N_x_corr[j]+X3_N[j][1][1]*Q3_N_y_corr[j];

        // compute Psi
        // no correction
        psi = atan2(events[i].Q3_S_y, events[i].Q3_S_x);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi3_S[j][0]->Fill(psi);

        psi = atan2(events[i].Q3_N_y, events[i].Q3_N_x);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi3_N[j][0]->Fill(psi);

        // order 1
        psi = atan2(Q3_S_y_corr[j], Q3_S_x_corr[j]);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi3_S[j][1]->Fill(psi);

        psi = atan2(Q3_N_y_corr[j], Q3_N_x_corr[j]);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi3_N[j][1]->Fill(psi);

        // order 2
        psi = atan2(Q3_S_y_corr2[j], Q3_S_x_corr2[j]);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi3_S[j][2]->Fill(psi);

        psi = atan2(Q3_N_y_corr2[j], Q3_N_x_corr2[j]);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi3_N[j][2]->Fill(psi);
    }

    cout << "Finish Process Event" << endl;
}


void myAnalysis::finalize(const string &i_output, const string &i_output_csv) {
    TFile output(i_output.c_str(),"recreate");

    for (Int_t i = 0; i < cent_key.size(); ++i) {
        output.mkdir((cent_key[i]).c_str());
        output.cd((cent_key[i]).c_str());

        for (Int_t j = 0; j < 3; ++j) {
            hPsi2_S[i][j]->Write();
            hPsi2_N[i][j]->Write();
            hPsi3_S[i][j]->Write();
            hPsi3_N[i][j]->Write();
       }
    }
    output.Close();

    std::ofstream file(i_output_csv.c_str());

    // Write header
    file << "Q2_S_x_avg, Q2_S_y_avg, Q2_N_x_avg, Q2_N_y_avg, X2_S_00, X2_S_01, X2_S_11, X2_N_00, X2_N_01, X2_N_11,"
         << "Q3_S_x_avg, Q3_S_y_avg, Q3_N_x_avg, Q3_N_y_avg, X3_S_00, X3_S_01, X3_S_11, X3_N_00, X3_N_01, X3_N_11" << endl;

    for (Int_t i = 0; i < cent_key.size(); ++i) {
        file << Q2_S_x_avg[i] << ","
             << Q2_S_y_avg[i] << ","
             << Q2_N_x_avg[i] << ","
             << Q2_N_y_avg[i] << ","
             << X2_S[i][0][0] << ","
             << X2_S[i][0][1] << ","
             << X2_S[i][1][1] << ","
             << X2_N[i][0][0] << ","
             << X2_N[i][0][1] << ","
             << X2_N[i][1][1] << ","

             << Q3_S_x_avg[i] << ","
             << Q3_S_y_avg[i] << ","
             << Q3_N_x_avg[i] << ","
             << Q3_N_y_avg[i] << ","
             << X3_S[i][0][0] << ","
             << X3_S[i][0][1] << ","
             << X3_S[i][1][1] << ","
             << X3_N[i][0][0] << ","
             << X3_N[i][0][1] << ","
             << X3_N[i][1][1] << endl;
    }

    file.close();
}

void Q_vector_correction(const string &i_input,
                         Float_t z                  = 10,
                         const string &i_output     = "test.root",
                         const string &i_output_csv = "test.csv",
                         Bool_t        isSim        = false,
                         Int_t         anaType      = 0,
                         Long64_t      start        = 0,
                         Long64_t      end          = 0) {

    cout << "#############################"     << endl;
    cout << "Run Parameters"                    << endl;
    cout << "inputFile: "       << i_input      << endl;
    cout << "z: "               << z << endl;
    cout << "outputFile: "      << i_output     << endl;
    cout << "output CSV File: " << i_output_csv << endl;
    cout << "isSim: "           << isSim        << endl;
    cout << "anaType: "         << anaType      << endl;
    cout << "start: "           << start        << endl;
    cout << "end: "             << end          << endl;
    cout << "#############################"     << endl;

    myAnalysis::anaType = anaType;
    myAnalysis::isSim   = isSim;

    Int_t ret = myAnalysis::init(i_input, start, end);
    if(ret != 0) return;

    myAnalysis::process_event(z, start, end);
    myAnalysis::finalize(i_output, i_output_csv);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 9){
        cout << "usage: ./Q-vec-corr inputFile [z] [outputFile] [output_csv] [isSim] [anaType] [start] [end] " << endl;
        cout << "inputFile: containing list of root file paths" << endl;
        cout << "z: z-vertex cut. Default: 10 cm. Range: 0 to 30 cm." << endl;
        cout << "outputFile: location of output file. Default: test.root." << endl;
        cout << "outputCSV: location of output csv. Default: test.csv." << endl;
        cout << "isSim: Simulation. Default: False." << endl;
        cout << "anaType: analysis type. Default: 0." << endl;
        cout << "start: start event number. Default: 0." << endl;
        cout << "end: end event number. Default: 0. (to run over all entries)." << endl;
        return 1;
    }

    Float_t z         = 10;
    string outputFile = "test.root";
    string output_csv = "test.csv";
    Bool_t   isSim    = false;
    Int_t    anaType  = 0;
    Long64_t start    = 0;
    Long64_t end      = 0;

    if(argc >= 3) {
        z = atof(argv[2]);
    }
    if(argc >= 4) {
        outputFile = argv[3];
    }
    if(argc >= 5) {
        output_csv = argv[4];
    }
    if(argc >= 6) {
        isSim = atoi(argv[5]);
    }
    if(argc >= 7) {
        anaType = atoi(argv[6]);
    }
    if(argc >= 8) {
        start = atol(argv[7]);
    }
    if(argc >= 9) {
        end = atol(argv[8]);
    }

    // ensure that 0 <= start <= end
    if(start > end || start < 0) {
        cerr << "Error: Invalid start, end provided! " << endl;
        return 1;
    }

    Q_vector_correction(argv[1], z, outputFile, output_csv, isSim, anaType, start, end);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
