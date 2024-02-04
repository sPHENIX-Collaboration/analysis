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
        Float_t e;      // min cluster energy
        Float_t e_asym;   // max cluster pair energy asymmetry: |E1-E2|/(E1+E2), where E1 and E2 is cluster ecore
        Float_t deltaR; // min cluster pair deltaR
        Float_t chi;    // max cluster chi2
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
    map<string, TH1F*>              hDiphotonPt;
    map<string, TH1F*>              hQQ;
    map<pair<string,string>, TH1F*> hqQ;
    map<pair<string,string>, TH1F*> hqQ_bg;
    map<pair<string,string>, TH1F*> hqQ_bg_left;
    map<pair<string,string>, TH1F*> hNPi0;
    map<pair<string,string>, TH2F*> h2Pi0EtaPhi;

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

    Bool_t do_vn_calc = true;

    // First Order Correction
    Float_t Q_S_x_avg[3] = {0};
    Float_t Q_S_y_avg[3] = {0};
    Float_t Q_N_x_avg[3] = {0};
    Float_t Q_N_y_avg[3] = {0};

    // Second Order Correction
    Float_t X_S[3][2][2] = {0}; // [cent][row][col], off diagonal entries are the same
    Float_t X_N[3][2][2] = {0}; // [cent][row][col], off diagonal entries are the same
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
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        string cell;
        char comma;

        Cut cut;

        if (lineStream >> cut.e      >> comma
                       >> cut.e_asym >> comma
                       >> cut.deltaR >> comma
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
    for(auto cut : cuts) {
        cout << left << "e: "        << setw(8) << cut.e
                     << ", e_asym: " << setw(8) << cut.e_asym
                     << ", deltaR: " << setw(8) << cut.deltaR
                     << ", chi: "    << setw(8) << cut.chi << endl;
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

    // 5: mu
    // 7: sigma
    Int_t col[2] = {5,7};

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

        if (!(lineStream >> Q_S_x_avg[i] >> comma
                         >> Q_S_y_avg[i] >> comma
                         >> Q_N_x_avg[i] >> comma
                         >> Q_N_y_avg[i] >> comma
                         >> X_S[i][0][0] >> comma
                         >> X_S[i][0][1] >> comma
                         >> X_S[i][1][1] >> comma
                         >> X_N[i][0][0] >> comma
                         >> X_N[i][0][1] >> comma
                         >> X_N[i][1][1])) {

            cerr << "Failed to parse line: " << line << endl;
            return 1;
        }
        ++i;
    }

    // Close the file
    file.close();

    cout << "Q Vector Corr Factors" << endl;
    for(Int_t j = 0; j < i; ++j) {
        cout << "Cent: " << cent_key[j] << endl;
        cout << left << "Q_S_x_avg: "   << setw(8) << Q_S_x_avg[j]
                     << ", Q_S_y_avg: " << setw(8) << Q_S_y_avg[j] << endl;

        cout << left << "Q_N_x_avg: "   << setw(8) << Q_N_x_avg[j]
                     << ", Q_N_y_avg: " << setw(8) << Q_N_y_avg[j] << endl;

        cout << left << "X_S_00: "   << setw(8) << X_S[j][0][0]
                     << ", X_S_01: " << setw(8) << X_S[j][0][1]
                     << ", X_S_11: " << setw(8) << X_S[j][1][1] << endl;

        cout << left << "X_N_00: "   << setw(8) << X_N[j][0][0]
                     << ", X_N_01: " << setw(8) << X_N[j][0][1]
                     << ", X_N_11: " << setw(8) << X_N[j][1][1] << endl;

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

        hQQ[cent_key[i]] = new TH1F(("hQQ_"+cent_key[i]).c_str(), ("QQ, " + suffix_title +"; QQ; Counts").c_str(), bins_Q, Q_min, Q_max);

        for(Int_t j = 0; j < pt_key.size(); ++j) {

            Int_t idx = i*pt_key.size()+j;

            pair<string,string> key = make_pair(cent_key[i],pt_key[j]);
            string suffix = "_"+cent_key[i]+"_"+pt_key[j];
            suffix_title = "Centrality: " + cent_key[i] + "%, Diphoton p_{T}: " + pt_key[j] + " GeV";

            hqQ[key] = new TH1F(("hqQ_"+to_string(idx)).c_str(), ("qQ, " + suffix_title + "; qQ; Counts").c_str(), bins_Q, Q_min, Q_max);
            hqQ_bg[key] = new TH1F(("hqQ_bg_"+to_string(idx)).c_str(), ("qQ, " + suffix_title + "; qQ; Counts").c_str(), bins_Q, Q_min, Q_max);
            hqQ_bg_left[key] = new TH1F(("hqQ_bg_left_"+to_string(idx)).c_str(), ("qQ, " + suffix_title + "; qQ; Counts").c_str(), bins_Q, Q_min, Q_max);

            h2Pi0EtaPhi[key] = new TH2F(("h2Pi0EtaPhi_"+to_string(idx)).c_str(), ("#pi_{0}, " + suffix_title + "; #eta; #phi").c_str(), bins_eta, eta_min, eta_max, bins_phi, phi_min, phi_max);

            h2DeltaRVsMass[key] = new TH2F(("h2DeltaRVsMass_"+to_string(idx)).c_str(),
                                            ("#Delta R vs Diphoton Invariant Mass, " + suffix_title +"; Mass [GeV]; #Delta R").c_str(),
                                            bins_pi0_mass, hpi0_mass_min, hpi0_mass_max,
                                            bins_deltaR, hdeltaR_min, hdeltaR_max);

            h2AsymVsMass[key] = new TH2F(("h2AsymVsMass_"+to_string(idx)).c_str(),
                                            ("Cluster Energy Asymmetry vs Diphoton Invariant Mass, " + suffix_title +"; Mass [GeV]; Energy Asymmetry").c_str(),
                                            bins_pi0_mass, hpi0_mass_min, hpi0_mass_max,
                                            bins_asym, hasym_min, hasym_max);

            hNPi0[key] = new TH1F(("hNPi0_"+to_string(idx)).c_str(), ("#pi_{0} Counts, " + suffix_title +"; # of #pi_{0} per event; Counts").c_str(), bins_npi0, npi0_min, npi0_max);

            vector<TH1F*> h;
            for (auto cut : cuts) {
                s.str("");
                t.str("");
                s << "hPi0Mass" << suffix << "_" << cut.e << "_" << cut.e_asym << "_" << cut.deltaR << "_" << cut.chi;
                t << "Diphoton: E #geq " << cut.e << ", Asym < " << cut.e_asym << ", #Delta R #geq " << cut.deltaR
                  << ", #chi^{2} < " << cut.chi << "; Invariant Mass [GeV]; Counts";
                h.push_back(new TH1F(s.str().c_str(), t.str().c_str(), bins_pi0_mass, hpi0_mass_min, hpi0_mass_max));
            }
            hPi0Mass[key] = h;
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
    T->SetBranchStatus("ecore_min",  true);
    T->SetBranchStatus("chi2_max",   true);

    if(do_vn_calc) {
        T->SetBranchStatus("Q_S_x",   true);
        T->SetBranchStatus("Q_S_y",   true);
        T->SetBranchStatus("Q_N_x",   true);
        T->SetBranchStatus("Q_N_y",   true);

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
    vector<Float_t>* ecore_min  = 0;
    vector<Float_t>* chi2_max   = 0;

    Float_t Q_S_x;
    Float_t Q_S_y;
    Float_t Q_N_x;
    Float_t Q_N_y;
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
    T->SetBranchAddress("ecore_min",  &ecore_min);
    T->SetBranchAddress("chi2_max",   &chi2_max);

    if(do_vn_calc) {
        T->SetBranchAddress("Q_S_x",   &Q_S_x);
        T->SetBranchAddress("Q_S_y",   &Q_S_y);
        T->SetBranchAddress("Q_N_x",   &Q_N_x);
        T->SetBranchAddress("Q_N_y",   &Q_N_y);

        T->SetBranchAddress("pi0_phi", &pi0_phi);
        T->SetBranchAddress("pi0_eta", &pi0_eta);
    }

    end = (end) ? min(end, T->GetEntries()-1) : T->GetEntries()-1;

    UInt_t evt_ctr[cent_key.size()] = {0};

    Float_t QQ_min    = 9999;
    Float_t qQ_min    = 9999;
    Float_t qQ_bg_min = 9999;
    Float_t eta_min   = 9999;
    UInt_t  npi0_max  = 0;
    Float_t QQ_max    = 0;
    Float_t qQ_max    = 0;
    Float_t qQ_bg_max = 0;
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

        ++evt_ctr[cent_idx];

        if(do_vn_calc) {
            // Apply first and second order correction to the Q vectors
            //============================================
            // compute first order correction
            Float_t Q_S_x_temp = Q_S_x - Q_S_x_avg[cent_idx];
            Float_t Q_S_y_temp = Q_S_y - Q_S_y_avg[cent_idx];
            Float_t Q_N_x_temp = Q_N_x - Q_N_x_avg[cent_idx];
            Float_t Q_N_y_temp = Q_N_y - Q_N_y_avg[cent_idx];

            // compute second order correction
            Q_S_x = X_S[cent_idx][0][0]*Q_S_x_temp+X_S[cent_idx][0][1]*Q_S_y_temp;
            Q_S_y = X_S[cent_idx][0][1]*Q_S_x_temp+X_S[cent_idx][1][1]*Q_S_y_temp;
            Q_N_x = X_N[cent_idx][0][0]*Q_N_x_temp+X_N[cent_idx][0][1]*Q_N_y_temp;
            Q_N_y = X_N[cent_idx][0][1]*Q_N_x_temp+X_N[cent_idx][1][1]*Q_N_y_temp;
            //============================================

            Float_t QQ = Q_S_x*Q_N_x + Q_S_y*Q_N_y;
            QQ_min = min(QQ_min, QQ);
            QQ_max = max(QQ_max, QQ);

            hQQ[cent_key[cent_idx]]->Fill(QQ);
        }

        UInt_t pi0_ctr[cent_key.size()*pt_key.size()] = {0};
        UInt_t bg_ctr[cent_key.size()*pt_key.size()]  = {0};
        UInt_t bg_left_ctr[cent_key.size()*pt_key.size()]  = {0};
        Float_t qQ[cent_key.size()*pt_key.size()]     = {0};
        Float_t qQ_bg[cent_key.size()*pt_key.size()]  = {0};
        Float_t qQ_bg_left[cent_key.size()*pt_key.size()]  = {0};
        // loop over all diphoton candidates
        for(UInt_t j = 0; j < pi0_mass->size(); ++j) {
            Float_t pi0_pt_val = pi0_pt->at(j);
            Int_t pt_idx = pt_dum_vec->FindBin(pi0_pt_val)-1;

            hDiphotonPt[cent_key[cent_idx]]->Fill(pi0_pt_val);
            // check if pt is found in one of the specified bins
            if(pt_idx < 0 || pt_idx >= 6) continue;

            pair<string,string> key = make_pair(cent_key[cent_idx], pt_key[pt_idx]);

            Float_t pi0_mass_val  = pi0_mass->at(j);
            Float_t asym_val      = pi0_asym->at(j);
            Float_t deltaR_val    = pi0_deltaR->at(j);
            Float_t ecore_min_val = ecore_min->at(j);
            Float_t chi2_max_val  = chi2_max->at(j);

            Float_t pi0_phi_val = 0;
            Float_t pi0_eta_val = 0;

            Float_t Q_x = 0;
            Float_t Q_y = 0;

            Double_t q_x    = 0;
            Double_t q_y    = 0;
            Double_t qQ_val = 0;

            if(do_vn_calc) {
                pi0_phi_val   = pi0_phi->at(j);
                pi0_eta_val   = pi0_eta->at(j);

                Q_x = (pi0_eta_val < 0) ? Q_N_x : Q_S_x;
                Q_y = (pi0_eta_val < 0) ? Q_N_y : Q_S_y;

                q_x = cos(2*pi0_phi_val);
                q_y = sin(2*pi0_phi_val);
                qQ_val = q_x*Q_x + q_y*Q_y;
            }

            h2AsymVsMass[key]->Fill(pi0_mass_val, asym_val);
            h2DeltaRVsMass[key]->Fill(pi0_mass_val, deltaR_val);

            Int_t idx = cent_idx*pt_key.size()+pt_idx;

            // compute mu+-2*sd of the pi0 mass to select diphotons as pi0 candidates
            Float_t pi0_mass_low  = pi0_mass_mu_sigma[idx].first-2*pi0_mass_mu_sigma[idx].second;
            Float_t pi0_mass_high = pi0_mass_mu_sigma[idx].first+2*pi0_mass_mu_sigma[idx].second;

            // compute mu+3*sd of the pi0 mass to select background diphotons
            Float_t bg_min = pi0_mass_mu_sigma[idx].first+3*pi0_mass_mu_sigma[idx].second;
            Float_t bg_max = 0.5; // setting max at 0.5 GeV to avoid the eta

            for(Int_t k = 0; k < cuts.size(); ++k) {
                if(ecore_min_val >= cuts[k].e      && asym_val     < cuts[k].e_asym &&
                   deltaR_val    >= cuts[k].deltaR && chi2_max_val < cuts[k].chi) {
                    hPi0Mass[key][k]->Fill(pi0_mass_val);

                    // fill in qQ for the background to the left of the pi0 peak
                    if(k == 0 && do_vn_calc && pi0_mass_val < bg_min) {
                        ++bg_left_ctr[idx];
                        qQ_bg_left[idx] += qQ_val;
                    }

                    // fill in qQ for the background
                    if(k == 0 && do_vn_calc && pi0_mass_val >= bg_min && pi0_mass_val < bg_max) {
                        ++bg_ctr[idx];
                        qQ_bg[idx] += qQ_val;
                    }
                    // fill in the qQ for the signal+background region
                    // do this for only one of the cuts for which we have signal bound information
                    if(k == 0 && do_vn_calc && pi0_mass_val >= pi0_mass_low && pi0_mass_val < pi0_mass_high) {
                        ++pi0_ctr[idx];
                        qQ[idx] += qQ_val;
                        h2Pi0EtaPhi[key]->Fill(pi0_eta_val, pi0_phi_val);
                        eta_min = min(eta_min, pi0_eta_val);
                        eta_max = max(eta_max, pi0_eta_val);
                    }
                }
            }
        }

        // loop over each centrality and pT bin
        for(UInt_t j = 0; j < cent_key.size(); ++j) {
            for(UInt_t k = 0; k < pt_key.size(); ++k) {
                Int_t idx = j*pt_key.size()+k;
                pair<string,string> key = make_pair(cent_key[j], pt_key[k]);

                if(do_vn_calc) {
                    npi0_max = max(npi0_max, pi0_ctr[idx]);
                    if(pi0_ctr[idx]) hNPi0[key]->Fill(pi0_ctr[idx]);

                    // compute qQ for the background to the left of the pi0 peak
                    qQ_bg_left[idx] = (bg_left_ctr[idx]) ? qQ_bg_left[idx]/bg_left_ctr[idx] : 0;
                    qQ_bg_min = min(qQ_bg_min, qQ_bg_left[idx]);
                    qQ_bg_max = max(qQ_bg_max, qQ_bg_left[idx]);

                    if(qQ_bg_left[idx]) hqQ_bg_left[key]->Fill(qQ_bg_left[idx]);

                    // compute qQ for the background
                    qQ_bg[idx] = (bg_ctr[idx]) ? qQ_bg[idx]/bg_ctr[idx] : 0;
                    qQ_bg_min = min(qQ_bg_min, qQ_bg[idx]);
                    qQ_bg_max = max(qQ_bg_max, qQ_bg[idx]);

                    if(qQ_bg[idx]) hqQ_bg[key]->Fill(qQ_bg[idx]);

                    // compute qQ for the pi0 candiates
                    qQ[idx] = (pi0_ctr[idx]) ? qQ[idx]/pi0_ctr[idx] : 0;
                    qQ_min = min(qQ_min, qQ[idx]);
                    qQ_max = max(qQ_max, qQ[idx]);

                    if(qQ[idx]) hqQ[key]->Fill(qQ[idx]);
                }
            }
        }
    }

    cout << endl;
    for(Int_t i = 0; i < cent_key.size(); ++i) {
        cout << "Cent: "     << cent_key[i]
             << ", Events: " << evt_ctr[i] << endl;
    }

    cout << "QQ min: " << QQ_min << ", QQ max: " << QQ_max << endl;
    cout << "qQ min: " << qQ_min << ", qQ max: " << qQ_max << endl;
    cout << "qQ bg min: " << qQ_bg_min << ", qQ bg max: " << qQ_bg_max << endl;
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

    if(do_vn_calc) {
        output.mkdir("vn/QQ");
        output.mkdir("vn/qQ");
        output.mkdir("vn/qQ_bg");
        output.mkdir("vn/qQ_bg_left");

        output.mkdir("QA/h2Pi0EtaPhi");
        output.mkdir("QA/hNPi0");
    }

    for(auto cent : cent_key) {
        output.cd("QA/hDiphotonPt");
        hDiphotonPt[cent]->Write();

        if(do_vn_calc) {
            output.cd("vn/QQ");
            hQQ[cent]->Write();
        }

        for(auto pt : pt_key) {
            pair<string,string> key = make_pair(cent, pt);

            output.cd("QA/h2DeltaRVsMass");
            h2DeltaRVsMass[key]    ->Write();

            output.cd("QA/h2AsymVsMass");
            h2AsymVsMass[key]      ->Write();

            if(do_vn_calc) {
                output.cd("QA/hNPi0");
                hNPi0[key]->Write();

                output.cd("vn/qQ");
                hqQ[key]->Write();

                output.cd("vn/qQ_bg");
                hqQ_bg[key]->Write();

                output.cd("vn/qQ_bg_left");
                hqQ_bg_left[key]->Write();

                output.cd("QA/h2Pi0EtaPhi");
                h2Pi0EtaPhi[key]->Write();
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
                 const string &fitStats = "",
                 const string &QVecCorr  = "",
                 const string &i_output = "test.root",
                 Float_t       z        = 10, /*cm*/
                 Long64_t      start    = 0,
                 Long64_t      end      = 0) {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "  << i_input << endl;
    cout << "Cuts: "       << i_cuts << endl;
    cout << "fitStats: "   << fitStats << endl;
    cout << "QVecCorr: "   << QVecCorr << endl;
    cout << "outputFile: " << i_output << endl;
    cout << "z: "          << z << endl;
    cout << "start: "      << start << endl;
    cout << "end: "        << end << endl;
    cout << "#############################" << endl;

    Int_t ret = myAnalysis::init(i_input, i_cuts, fitStats, QVecCorr, start, end);
    if(ret != 0) return;

    myAnalysis::process_event(z, start, end);
    myAnalysis::finalize(i_output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 3 || argc > 9){
        cout << "usage: ./pi0Ana inputFile cuts [fitStats] [QVecCorr] [outputFile] [z] [start] [end] " << endl;
        cout << "inputFile: containing list of root file paths" << endl;
        cout << "cuts: csv file containing cuts" << endl;
        cout << "fitStats: csv file containing fit stats" << endl;
        cout << "QVecCorr: csv file containing Q vector corrections" << endl;
        cout << "z: z-vertex cut. Default: 10 cm. Range: 0 to 30 cm." << endl;
        cout << "outputFile: location of output file. Default: test.root." << endl;
        cout << "start: start event number. Default: 0." << endl;
        cout << "end: end event number. Default: 0. (to run over all entries)." << endl;
        return 1;
    }

    string fitStats   = "";
    string QVecCorr   = "";
    string outputFile = "test.root";
    Float_t z         = 10;
    Long64_t start    = 0;
    Long64_t end      = 0;

    if(argc >= 4) {
        fitStats = argv[3];
    }
    if(argc >= 5) {
        QVecCorr = argv[4];
    }
    if(argc >= 6) {
        outputFile = argv[5];
    }
    if(argc >= 7) {
        z = atof(argv[6]);
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

    pi0Analysis(argv[1], argv[2], fitStats, QVecCorr, outputFile, z, start, end);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
