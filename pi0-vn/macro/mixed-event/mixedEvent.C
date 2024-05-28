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
        Float_t e_asym; // max cluster pair energy asymmetry: |E1-E2|/(E1+E2), where E1 and E2 is cluster ecore
        Float_t deltaR; // min cluster pair deltaR
        Float_t chi;    // max cluster chi2
    };

    struct Event {
        Float_t cent;
        Float_t z;
        vector<Float_t> pi0_mass;
        vector<Float_t> pi0_pt;
        vector<Float_t> pi0_asym;
        vector<Float_t> pi0_deltaR;
        vector<Float_t> ecore_min;
        vector<Float_t> chi2_max;

        Float_t Q_S_x;
        Float_t Q_S_y;
        Float_t Q_N_x;
        Float_t Q_N_y;

        vector<Float_t> pi0_phi;
        vector<Float_t> pi0_eta;
    };

    vector<Cut> cuts;
    vector<Event> events;

    Int_t init(const string &i_input, const string &i_cuts, const string &QVecCorr, Long64_t start = 0, Long64_t end = 0);
    Int_t readFiles(const string &i_input, Long64_t start = 0, Long64_t end = 0);
    Int_t readCuts(const string &i_cuts);
    Int_t readQVectorCorrection(const string &i_input);
    void init_hists();

    void process_event(Long64_t start = 0, Long64_t end = 0);
    void finalize(const string &i_output = "test.root");

    vector<string> cent_key = {"40-60", "20-40", "0-20"};
    vector<string> pt_key   = {"2-2.5", "2.5-3", "3-3.5", "3.5-4", "4-4.5", "4.5-5"};

    TH1F* pt_dum_vec   = new TH1F("pt_dum_vec","",6,2,5);
    TH1F* cent_dum_vec = new TH1F("cent_dum_vec","", 3, 0, 0.6);

    TH1F* hQQ;
    TH1F* hqQ_mix;
    TH1F* hqQ;
    TH1F* hPi0Mass;
    TH2F* h2ZVsCent;

    Float_t z_min = 0; // cm
    Float_t z_max = 1; // cm
    Float_t cent_min = 0.1;
    Float_t cent_max = 0.15;
    Float_t pt_min = 2; // GeV
    Float_t pt_max = 2.5; // GeV

    Float_t pi0_mass_low  = 0.1;
    Float_t pi0_mass_high = 0.25;

    Int_t   bins_pi0_mass = 56;
    Float_t hpi0_mass_min = 0;
    Float_t hpi0_mass_max = 0.7;

    Int_t   bins_cent = 100;
    Float_t hcent_min = 0;
    Float_t hcent_max = 1;

    Int_t   bins_z = 40;
    Float_t hz_min = -10;
    Float_t hz_max = 10;

    Int_t   bins_Q = 200;
    Float_t Q_min  = -1;
    Float_t Q_max  = 1;

    // First Order Correction
    Float_t Q_S_x_avg[3] = {0};
    Float_t Q_S_y_avg[3] = {0};
    Float_t Q_N_x_avg[3] = {0};
    Float_t Q_N_y_avg[3] = {0};

    // Second Order Correction
    Float_t X_S[3][2][2] = {0}; // [cent][row][col], off diagonal entries are the same
    Float_t X_N[3][2][2] = {0}; // [cent][row][col], off diagonal entries are the same

    UInt_t empty_segments = 0;
    UInt_t total_segments = 0;
}

Int_t myAnalysis::init(const string &i_input, const string &i_cuts, const string& QVecCorr, Long64_t start, Long64_t end) {
    // T = new TChain("T");
    // T->Add(i_input.c_str());

    Int_t ret;
    ret = readFiles(i_input, start, end);
    if(ret != 0) return ret;

    ret = readCuts(i_cuts);
    if(ret != 0) return ret;

    init_hists();

    ret = readQVectorCorrection(QVecCorr);

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

        // keep track of the number of empty files
        if(!(T->GetEntries()-entries)) ++empty_segments;

        ++total_segments;

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

    stringstream t;
    t << "QQ: " << z_min << " cm #leq z < " << z_max << " cm, "
                << cent_min << " #leq cent < " << cent_max
                << "; QQ; Counts" << endl;

    hQQ = new TH1F("hQQ", t.str().c_str(), bins_Q, Q_min, Q_max);

    t.str("");
    t << "qQ: " << z_min << " cm #leq z < " << z_max << " cm, "
                << cent_min << " #leq cent < " << cent_max << ", "
                << pt_min << " GeV #leq pt < " << pt_max
                << " GeV; qQ; Counts" << endl;

    hqQ_mix = new TH1F("hqQ_mix", t.str().c_str(), bins_Q, Q_min, Q_max);
    hqQ = new TH1F("hqQ", t.str().c_str(), bins_Q, Q_min, Q_max);

    t.str("");
    t << "Diphoton: " << z_min << " cm #leq z < " << z_max << " cm, "
                << cent_min << " #leq cent < " << cent_max << ", "
                << pt_min << " GeV #leq p_{T} < " << pt_max
                << " GeV; Invariant Mass [GeV]; Counts" << endl;
    hPi0Mass = new TH1F("hPi0Mass", t.str().c_str(), bins_pi0_mass, hpi0_mass_min, hpi0_mass_max);

    h2ZVsCent = new TH2F("h2ZVsCent", "Z Vertex vs Centrality; Centrality; z [cm]", bins_cent, hcent_min, hcent_max, bins_z, hz_min, hz_max);
}

void myAnalysis::process_event(Long64_t start, Long64_t end) {
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

    T->SetBranchStatus("Q_S_x", true);
    T->SetBranchStatus("Q_S_y", true);
    T->SetBranchStatus("Q_N_x", true);
    T->SetBranchStatus("Q_N_y", true);

    T->SetBranchStatus("pi0_phi", true);
    T->SetBranchStatus("pi0_eta", true);

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

    T->SetBranchAddress("Q_S_x", &Q_S_x);
    T->SetBranchAddress("Q_S_y", &Q_S_y);
    T->SetBranchAddress("Q_N_x", &Q_N_x);
    T->SetBranchAddress("Q_N_y", &Q_N_y);

    T->SetBranchAddress("pi0_phi", &pi0_phi);
    T->SetBranchAddress("pi0_eta", &pi0_eta);

    end = (end) ? min(end, T->GetEntries()-1) : T->GetEntries()-1;

    Float_t QQ_min        = 9999;
    Float_t qQ_min        = 9999;
    Float_t QQ_max        = 0;
    Float_t qQ_max        = 0;
    UInt_t  npi0_max      = 0;
    UInt_t  mix_event_ctr = 0;
    UInt_t  event_ctr     = 0;

    cout << "Events to process: " << end-start+1 << endl;

    cout << "Loading Events" << endl;
    for (Long64_t i = start; i <= end; ++i) {
        if(i%100 == 0) cout << "Progress: " << (i-start)*100./(end-start) << "%" << endl;
        // Load the data for the given tree entry
        T->GetEntry(i);

        if(z >= hz_min && z < hz_max) h2ZVsCent->Fill(cent, z);

        // ensure the z vertex is within range
        // ensure centrality is within range
        if(!(z >= z_min && z < z_max && cent >= cent_min && cent < cent_max)) continue;

        // Int_t cent_idx = cent_dum_vec->FindBin(totalMBD)-1;
        Int_t cent_idx = cent_dum_vec->FindBin(cent)-1;

        // check if centrality is found in one of the specified bins
        if(!(cent_idx >= 0 && cent_idx < 3)) continue;

        Event event;
        event.pi0_mass.assign(pi0_mass->begin(), pi0_mass->end());
        event.pi0_pt.assign(pi0_pt->begin(), pi0_pt->end());
        event.pi0_asym.assign(pi0_asym->begin(), pi0_asym->end());
        event.pi0_deltaR.assign(pi0_deltaR->begin(), pi0_deltaR->end());
        event.pi0_phi.assign(pi0_phi->begin(), pi0_phi->end());
        event.pi0_eta.assign(pi0_eta->begin(), pi0_eta->end());
        event.ecore_min.assign(ecore_min->begin(), ecore_min->end());
        event.chi2_max.assign(chi2_max->begin(), chi2_max->end());

        event.cent  = cent;
        event.z     = z;
        event.Q_S_x = Q_S_x;
        event.Q_S_y = Q_S_y;
        event.Q_N_x = Q_N_x;
        event.Q_N_y = Q_N_y;

        events.push_back(event);

        // loop over all diphoton candidates
        for(UInt_t j = 0; j < pi0_mass->size(); ++j) {
            Float_t pi0_pt_val = pi0_pt->at(j);

            // check if pt within the specified range
            if(!(pi0_pt_val >= pt_min && pi0_pt_val < pt_max)) continue;

            Float_t ecore_min_val = ecore_min->at(j);
            Float_t asym_val      = pi0_asym->at(j);
            Float_t deltaR_val    = pi0_deltaR->at(j);
            Float_t chi2_max_val  = chi2_max->at(j);
            Float_t pi0_mass_val  = pi0_mass->at(j);

            if(!(ecore_min_val >= cuts[0].e      && asym_val     < cuts[0].e_asym &&
                 deltaR_val    >= cuts[0].deltaR && chi2_max_val < cuts[0].chi)) continue;

            hPi0Mass->Fill(pi0_mass_val);
        }
    }
    cout << "Finished Loading Events" << endl;

    cout << endl;

    cout << "Begin Event Mixing" << endl;
    // loop over each event
    for (Long64_t i = 0; i < events.size(); ++i) {
        if(i%10 == 0) cout << "Progress: " << i*100./events.size() << "%" << endl;

        Int_t cent_idx = cent_dum_vec->FindBin(events[i].cent)-1;

        // need to reverse this index since we want to match cent_key
        cent_idx = cent_key.size() - cent_idx - 1;

        // Apply first and second order correction to the Q vectors
        //============================================
        // compute first order correction
        Float_t Q_S_x_temp = events[i].Q_S_x - Q_S_x_avg[cent_idx];
        Float_t Q_S_y_temp = events[i].Q_S_y - Q_S_y_avg[cent_idx];
        Float_t Q_N_x_temp = events[i].Q_N_x - Q_N_x_avg[cent_idx];
        Float_t Q_N_y_temp = events[i].Q_N_y - Q_N_y_avg[cent_idx];

        // compute second order correction
        Q_S_x_temp = X_S[cent_idx][0][0]*Q_S_x_temp+X_S[cent_idx][0][1]*Q_S_y_temp;
        Q_S_y_temp = X_S[cent_idx][0][1]*Q_S_x_temp+X_S[cent_idx][1][1]*Q_S_y_temp;
        Q_N_x_temp = X_N[cent_idx][0][0]*Q_N_x_temp+X_N[cent_idx][0][1]*Q_N_y_temp;
        Q_N_y_temp = X_N[cent_idx][0][1]*Q_N_x_temp+X_N[cent_idx][1][1]*Q_N_y_temp;
        //============================================

        Float_t QQ = Q_S_x_temp*Q_N_x_temp + Q_S_y_temp*Q_N_y_temp;
        QQ_min = min(QQ_min, QQ);
        QQ_max = max(QQ_max, QQ);

        hQQ->Fill(QQ);

        // loop over remaning events for mixed event correlations
        for (Long64_t j = 0; j < events.size(); ++j) {

            Int_t cent_idx2 = cent_dum_vec->FindBin(events[j].cent)-1;

            // need to reverse this index since we want to match cent_key
            cent_idx2 = cent_key.size() - cent_idx2 - 1;

            // check if we are in same centrality bin
            if(cent_idx2 != cent_idx) continue;

            UInt_t pi0_mix_ctr = 0;
            UInt_t pi0_ctr     = 0;
            Float_t qQ_mix     = 0;
            Float_t qQ         = 0;

            // loop over all diphoton candidates
            for(UInt_t k = 0; k < events[j].pi0_mass.size(); ++k) {

                Float_t pi0_pt_val = events[j].pi0_pt.at(k);

                // check if pt within the specified range
                if(pi0_pt_val < pt_min || pi0_pt_val >= pt_max) continue;

                Float_t ecore_min_val = events[j].ecore_min.at(k);
                Float_t asym_val      = events[j].pi0_asym.at(k);
                Float_t deltaR_val    = events[j].pi0_deltaR.at(k);
                Float_t chi2_max_val  = events[j].chi2_max.at(k);
                Float_t pi0_mass_val  = events[j].pi0_mass.at(k);

                Int_t pt_idx = pt_dum_vec->FindBin(pi0_pt_val)-1;
                Int_t idx = cent_idx*pt_key.size()+pt_idx;

                // check that the diphoton passes the cuts and is within the mass region
                if(!(ecore_min_val >= cuts[0].e      && asym_val     < cuts[0].e_asym &&
                   deltaR_val      >= cuts[0].deltaR && chi2_max_val < cuts[0].chi &&
                   pi0_mass_val    >= pi0_mass_low   && pi0_mass_val < pi0_mass_high)) continue;

                Float_t pi0_phi_val = events[j].pi0_phi.at(k);
                Float_t pi0_eta_val = events[j].pi0_eta.at(k);

                Float_t Q_x = (pi0_eta_val < 0) ? Q_N_x_temp : Q_S_x_temp;
                Float_t Q_y = (pi0_eta_val < 0) ? Q_N_y_temp : Q_S_y_temp;

                Float_t q_x = cos(2*pi0_phi_val);
                Float_t q_y = sin(2*pi0_phi_val);
                Float_t qQ_val = q_x*Q_x + q_y*Q_y;

                // same event correlations
                if(i == j) {
                    qQ += qQ_val;
                    ++pi0_ctr;
                }
                // mixed event correlations
                else {
                    qQ_mix += qQ_val;
                    ++pi0_mix_ctr;
                }
            }

            // compute qQ_mix for the pi0 candiates
            qQ_mix = (pi0_mix_ctr) ? qQ_mix/pi0_mix_ctr : 0;
            qQ_min = min(qQ_min, qQ_mix);
            qQ_max = max(qQ_max, qQ_mix);

            npi0_max = max(npi0_max, pi0_mix_ctr);

            if(qQ_mix) {
                hqQ_mix->Fill(qQ_mix);
                ++mix_event_ctr;
            }

            // compute qQ for the pi0 candiates
            qQ = (pi0_ctr) ? qQ/pi0_ctr : 0;
            qQ_min = min(qQ_min, qQ);
            qQ_max = max(qQ_max, qQ);

            npi0_max = max(npi0_max, pi0_ctr);

            if(qQ) {
                hqQ->Fill(qQ);
                ++event_ctr;
            }
        }
    }

    cout << endl;
    cout << "=============================================="<< endl;
    cout << "Total files: " << total_segments << endl;
    cout << "Empty files: " << empty_segments << ", " << empty_segments*100./total_segments << " %"<< endl;
    cout << "Max Pi0s per event: " << npi0_max << endl;
    cout << "Events: " << event_ctr << endl;
    cout << "mixed event pair: " << mix_event_ctr << endl;
    cout << "QQ min: " << QQ_min << ", QQ max: " << QQ_max << endl;
    cout << "qQ min: " << qQ_min << ", qQ max: " << qQ_max << endl;
    // print v2
    cout << "hQQ Mean: " << hQQ->GetMean() << endl;
    cout << "hqQ_mix Mean: " << hqQ_mix->GetMean() << endl;
    cout << "hqQ Mean: " << hqQ->GetMean() << endl;
    cout << "v2 mix: " << hqQ_mix->GetMean()/sqrt(hQQ->GetMean()) << endl;
    cout << "v2: " << hqQ->GetMean()/sqrt(hQQ->GetMean()) << endl;
    cout << "Finish Process Event" << endl;
    cout << "=============================================="<< endl;
}

void myAnalysis::finalize(const string &i_output) {
    TFile output(i_output.c_str(),"recreate");
    output.cd();

    hQQ->Write();
    hqQ_mix->Write();
    hqQ->Write();
    hPi0Mass->Write();
    h2ZVsCent->Write();

    output.Close();
}

void mixedEvent(const string &i_input,
                 const string &i_cuts,
                 const string &QVecCorr  = "",
                 const string &i_output = "test.root",
                 Long64_t      start    = 0,
                 Long64_t      end      = 0) {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "  << i_input << endl;
    cout << "Cuts: "       << i_cuts << endl;
    cout << "QVecCorr: "   << QVecCorr << endl;
    cout << "outputFile: " << i_output << endl;
    cout << "start: "      << start << endl;
    cout << "end: "        << end << endl;
    cout << "#############################" << endl;

    Int_t ret = myAnalysis::init(i_input, i_cuts, QVecCorr, start, end);
    if(ret != 0) return;

    myAnalysis::process_event(start, end);
    myAnalysis::finalize(i_output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 4 || argc > 8){
        cout << "usage: ./mixedEvent inputFile cuts QVecCorr [outputFile] [start] [end] " << endl;
        cout << "inputFile: containing list of root file paths" << endl;
        cout << "cuts: csv file containing cuts" << endl;
        cout << "fitStats: csv file containing fit stats" << endl;
        cout << "QVecCorr: csv file containing Q vector corrections" << endl;
        cout << "outputFile: location of output file. Default: test.root." << endl;
        cout << "start: start event number. Default: 0." << endl;
        cout << "end: end event number. Default: 0. (to run over all entries)." << endl;
        return 1;
    }

    string outputFile = "test.root";
    Long64_t start    = 0;
    Long64_t end      = 0;

    if(argc >= 5) {
        outputFile = argv[4];
    }
    if(argc >= 6) {
        start = atol(argv[5]);
    }
    if(argc >= 7) {
        end = atol(argv[6]);
    }

    // ensure that 0 <= start <= end
    if(start > end || start < 0) {
        cerr << "Error: Invalid start, end provided! " << endl;
        return 1;
    }

    mixedEvent(argv[1], argv[2], argv[3], outputFile, start, end);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
