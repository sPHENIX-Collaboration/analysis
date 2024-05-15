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


    Int_t init(const string &i_input, const string &QVecCorr, Long64_t start = 0, Long64_t end = 0);
    Int_t readFiles(const string &i_input, Long64_t start = 0, Long64_t end = 0);
    Int_t readQVectorCorrection(const string &i_input);
    void init_hists();

    void process_event(Float_t z_max = 10, Long64_t start = 0, Long64_t end = 0);
    void finalize(const string &i_output = "test.root");

    Int_t anaType;

    vector<string> cent_key;
    vector<string> cent_key1 = {"40-60", "20-40", "0-20"};
    vector<string> cent_key2 = {"50-60", "40-50", "30-40","20-30","10-20","0-10"};

    vector<string> pt_key;
    vector<string> pt_key1 = {"2-2.5", "2.5-3", "3-3.5", "3.5-4", "4-4.5", "4.5-5"};
    vector<string> pt_key2 = {"2-5"};

    TH1F* pt_dum_vec;
    TH1F* cent_dum_vec;

    map<pair<string,string>, TH1F*> hPi0Mass; // hPi0Mass[make_pair(cent,pt)]

    map<string, TH1F*>              hPi0Pt;
    // v2
    vector<map<string, TH1F*>>              hQQ2;
    vector<map<pair<string,string>, TH1F*>> hqQ2;

    map<pair<string,string>, TH1F*> hNPi0;
    map<pair<string,string>, TH2F*> h2Pi0EtaPhi;

    Int_t   bins_pi0_mass = 56;
    Float_t hpi0_mass_min = 0;
    Float_t hpi0_mass_max = 0.7;

    Int_t   bins_pt = 500;
    Float_t hpt_min = 0;
    Float_t hpt_max = 20;

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

    Int_t subsamples  = 1;

    // First Order Correction
    // v2
    vector<Float_t> Q2_S_x_avg;
    vector<Float_t> Q2_S_y_avg;
    vector<Float_t> Q2_N_x_avg;
    vector<Float_t> Q2_N_y_avg;

    // Second Order Correction
    // v2
    vector<vector<vector<Float_t>>> X2_S; // [cent][row][col], off diagonal entries are the same
    vector<vector<vector<Float_t>>> X2_N; // [cent][row][col], off diagonal entries are the same
}

Int_t myAnalysis::init(const string &i_input, const string& QVecCorr, Long64_t start, Long64_t end) {
    T = new TChain("T");
    T->Add(i_input.c_str());

    cent_key = (anaType == 0) ? cent_key1 : cent_key2;
    pt_key   = (anaType == 0) ? pt_key1 : pt_key2;

    cent_dum_vec = new TH1F("cent_dum_vec","", cent_key.size(), 0, 0.6);

    pt_dum_vec   = new TH1F("pt_dum_vec","", pt_key.size(), 2, 5);

    // v2
    Q2_S_x_avg.resize(cent_key.size());
    Q2_S_y_avg.resize(cent_key.size());
    Q2_N_x_avg.resize(cent_key.size());
    Q2_N_y_avg.resize(cent_key.size());

    X2_S.resize(cent_key.size(), vector<vector<Float_t>>(2, vector<Float_t>(2)));
    X2_N.resize(cent_key.size(), vector<vector<Float_t>>(2, vector<Float_t>(2)));

    Int_t ret;
    // ret = readFiles(i_input, start, end);
    // if(ret != 0) return ret;

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

        Float_t temp;

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
                         >> temp >> comma
                         >> temp >> comma
                         >> temp >> comma
                         >> temp >> comma
                         >> temp >> comma
                         >> temp >> comma
                         >> temp >> comma
                         >> temp >> comma
                         >> temp >> comma
                         >> temp)) {

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
    }

    return 0;
}

void myAnalysis::init_hists() {

    stringstream s;
    stringstream t;
    // create a pi0 mass hist for each cut
    // create QA plots for each centrality/pt bin
    for(Int_t i = 0; i < cent_key.size(); ++i) {

        string suffix_title = "Centrality: " + cent_key[i] + "%";

        hPi0Pt[cent_key[i]] = new TH1F(("hPi0Pt_"+cent_key[i]).c_str(), ("Truth #pi^{0} p_{T}, " + suffix_title +"; p_{T} [GeV]; Counts").c_str(), bins_pt, hpt_min, hpt_max);

        for(Int_t j = 0; j < pt_key.size(); ++j) {

            Int_t idx = i*pt_key.size()+j;

            pair<string,string> key = make_pair(cent_key[i],pt_key[j]);
            string suffix = "_"+cent_key[i]+"_"+pt_key[j];
            suffix_title = "Centrality: " + cent_key[i] + "%, Truth #pi^{0} p_{T}: " + pt_key[j] + " GeV";

            h2Pi0EtaPhi[key] = new TH2F(("h2Pi0EtaPhi_"+to_string(idx)).c_str(), ("#pi_{0}, " + suffix_title + "; #eta; #phi").c_str(), bins_eta, eta_min, eta_max, bins_phi, phi_min, phi_max);

            hNPi0[key] = new TH1F(("hNPi0_"+to_string(idx)).c_str(), ("#pi_{0} Counts, " + suffix_title +"; # of #pi_{0} per event; Counts").c_str(), bins_npi0, npi0_min, npi0_max);

            s.str("");
            t.str("");
            s << "hPi0Mass" << suffix;
            t << "Truth #pi^{0}; Invariant Mass [GeV]; Counts";
            auto hist = new TH1F(s.str().c_str(), t.str().c_str(), bins_pi0_mass, hpi0_mass_min, hpi0_mass_max);
            hist->Sumw2();
            hPi0Mass[key] = hist;
        }
    }

    for(Int_t k = 0; k < subsamples; ++k) {
        map<string, TH1F*> hQQ2_dummy;
        map<pair<string,string>, TH1F*> hqQ2_dummy;

        for(Int_t i = 0; i < cent_key.size(); ++i) {

            string suffix_title = "Centrality: " + cent_key[i] + "%";
            // v2
            hQQ2_dummy[cent_key[i]] = new TH1F(("hQQ2_"+to_string(k)+"_"+cent_key[i]).c_str(), ("QQ2, " + suffix_title +"; QQ2; Counts").c_str(), bins_Q, Q_min, Q_max);

            for(Int_t j = 0; j < pt_key.size(); ++j) {
                Int_t idx = i*pt_key.size()+j;

                pair<string,string> key = make_pair(cent_key[i],pt_key[j]);
                string suffix = "_"+cent_key[i]+"_"+pt_key[j];
                suffix_title = "Centrality: " + cent_key[i] + "%, Truth #pi^{0} p_{T}: " + pt_key[j] + " GeV";

                hqQ2_dummy[key]         = new TH1F(("hqQ2_"+to_string(k)+"_"+to_string(idx)).c_str(), ("qQ2, " + suffix_title + "; qQ2; Counts").c_str(), bins_Q, Q_min, Q_max);
            }
        }

        hQQ2.push_back(hQQ2_dummy);
        hqQ2.push_back(hqQ2_dummy);
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
    T->SetBranchStatus("centrality", true);
    T->SetBranchStatus("vtx_z",      true);

    T->SetBranchStatus("Q2_S_x",  true);
    T->SetBranchStatus("Q2_S_y",  true);
    T->SetBranchStatus("Q2_N_x",  true);
    T->SetBranchStatus("Q2_N_y",  true);
    T->SetBranchStatus("geta",  true);
    T->SetBranchStatus("gphi",  true);
    T->SetBranchStatus("gpt",   true);
    T->SetBranchStatus("gmass", true);

    // Int_t   run;
    // Int_t   event;
    Float_t cent;
    Float_t z;
    vector<Float_t>* pi0_mass = 0;
    vector<Float_t>* pi0_pt   = 0;
    vector<Float_t>* pi0_phi  = 0;
    vector<Float_t>* pi0_eta  = 0;

    Float_t Q2_S_x;
    Float_t Q2_S_y;
    Float_t Q2_N_x;
    Float_t Q2_N_y;

    T->SetBranchAddress("centrality", &cent);
    T->SetBranchAddress("vtx_z",      &z);
    T->SetBranchAddress("gmass",      &pi0_mass);
    T->SetBranchAddress("gpt",        &pi0_pt);
    T->SetBranchAddress("gphi",       &pi0_phi);
    T->SetBranchAddress("geta",       &pi0_eta);

    T->SetBranchAddress("Q2_S_x", &Q2_S_x);
    T->SetBranchAddress("Q2_S_y", &Q2_S_y);
    T->SetBranchAddress("Q2_N_x", &Q2_N_x);
    T->SetBranchAddress("Q2_N_y", &Q2_N_y);

    end = (end) ? min(end, T->GetEntries()-1) : T->GetEntries()-1;

    UInt_t evt_ctr[cent_key.size()] = {0};

    Float_t QQ2_min    = 9999;
    Float_t qQ2_min    = 9999;
    Float_t eta_min    = 9999;

    UInt_t  npi0_max   = 0;
    Float_t QQ2_max    = 0;
    Float_t qQ2_max    = 0;
    Float_t eta_max    = 0;

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
        if(cent_idx < 0 || cent_idx >= cent_key.size()) continue;

        // need to reverse this index since we want to match cent_key
        cent_idx = cent_key.size() - cent_idx - 1;

        // assign each event to a sample
        UInt_t k = evt_ctr[cent_idx]%subsamples;

        ++evt_ctr[cent_idx];

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

        UInt_t  pi0_ctr[cent_key.size()*pt_key.size()]     = {0};
        Float_t qQ2[cent_key.size()*pt_key.size()]         = {0};

        // loop over all truth pi0 candidates
        for(UInt_t j = 0; j < pi0_mass->size(); ++j) {
            Float_t pi0_pt_val = pi0_pt->at(j);
            Int_t pt_idx = pt_dum_vec->FindBin(pi0_pt_val)-1;

            hPi0Pt[cent_key[cent_idx]]->Fill(pi0_pt_val);
            // check if pt is found in one of the specified bins
            if(pt_idx < 0 || pt_idx >= pt_key.size()) continue;

            pair<string,string> key = make_pair(cent_key[cent_idx], pt_key[pt_idx]);

            Float_t pi0_mass_val   = pi0_mass->at(j);

            Float_t pi0_phi_val = 0;
            Float_t pi0_eta_val = 0;

            Float_t Q2_x = 0;
            Float_t Q2_y = 0;

            Double_t q2_x    = 0;
            Double_t q2_y    = 0;
            Double_t qQ2_val = 0;

            pi0_phi_val   = pi0_phi->at(j);
            pi0_eta_val   = pi0_eta->at(j);

            Q2_x = (pi0_eta_val < 0) ? Q2_N_x : Q2_S_x;
            Q2_y = (pi0_eta_val < 0) ? Q2_N_y : Q2_S_y;

            q2_x = cos(2*pi0_phi_val);
            q2_y = sin(2*pi0_phi_val);
            qQ2_val = q2_x*Q2_x + q2_y*Q2_y;

            Int_t idx = cent_idx*pt_key.size()+pt_idx;

            hPi0Mass[key]->Fill(pi0_mass_val);

            ++pi0_ctr[idx];
            qQ2[idx] += qQ2_val;
            h2Pi0EtaPhi[key]->Fill(pi0_eta_val, pi0_phi_val);
            eta_min = min(eta_min, pi0_eta_val);
            eta_max = max(eta_max, pi0_eta_val);
        }

        // loop over each centrality and pT bin
        for (UInt_t j = 0; j < cent_key.size(); ++j) {
            for (UInt_t p = 0; p < pt_key.size(); ++p) {
                Int_t idx = j * pt_key.size() + p;
                pair<string, string> key = make_pair(cent_key[j], pt_key[p]);

                npi0_max = max(npi0_max, pi0_ctr[idx]);
                if (pi0_ctr[idx]) hNPi0[key]->Fill(pi0_ctr[idx]);

                // compute qQ for the pi0 candiates
                qQ2[idx] = (pi0_ctr[idx]) ? qQ2[idx] / pi0_ctr[idx] : 0;
                qQ2_min = min(qQ2_min, qQ2[idx]);
                qQ2_max = max(qQ2_max, qQ2[idx]);

                if (qQ2[idx]) hqQ2[k][key]->Fill(qQ2[idx], pi0_ctr[idx]);
            }
        }
    }

    cout << endl;
    UInt_t events = 0;
    for(Int_t i = 0; i < cent_key.size(); ++i) {
        cout << "Cent: " << cent_key[i] << ", Events: " << evt_ctr[i] << endl;

        events += evt_ctr[i];
    }

    cout << endl;
    cout << "Events Processed: " << events << ", " << events*100./(end-start) << " %" << endl;
    cout << endl;
    cout << "QQ2 min: " << QQ2_min << ", QQ2 max: " << QQ2_max << endl;
    cout << "qQ2 min: " << qQ2_min << ", qQ2 max: " << qQ2_max << endl;
    cout << endl;
    cout << "pi0 eta min: " << eta_min << ", pi0 eta max: " << eta_max << endl;
    cout << "Max Pi0s per event: " << npi0_max << endl;
    cout << "Finish Process Event" << endl;
}


void myAnalysis::finalize(const string &i_output) {
    TFile output(i_output.c_str(),"recreate");
    output.mkdir("results");
    output.mkdir("QA/hPi0Pt");

    for(Int_t k = 0; k < subsamples; ++k) {
        string prefix = "vn/"+to_string(k);

        output.mkdir((prefix+"/QQ2").c_str());
        output.mkdir((prefix+"/qQ2").c_str());
    }

    output.mkdir("QA/h2Pi0EtaPhi");
    output.mkdir("QA/hNPi0");

    for(auto cent : cent_key) {
        output.cd("QA/hPi0Pt");
        hPi0Pt[cent]->Write();

        for(Int_t k = 0; k < subsamples; ++k) {
            string prefix = "vn/"+to_string(k);
            output.cd((prefix+"/QQ2").c_str());
            hQQ2[k][cent]->Write();
        }

        for(auto pt : pt_key) {
            pair<string,string> key = make_pair(cent, pt);

            for(Int_t k = 0; k < subsamples; ++k) {
                string prefix = "vn/"+to_string(k);

                output.cd((prefix+"/qQ2").c_str());
                hqQ2[k][key]->Write();
            }

            output.cd("QA/hNPi0");
            hNPi0[key]->Write();

            output.cd("QA/h2Pi0EtaPhi");
            h2Pi0EtaPhi[key]->Write();

            output.mkdir(("results/"+cent+"/"+pt).c_str());
            output.cd(("results/"+cent+"/"+pt).c_str());

            hPi0Mass[key]->Write();
        }
    }

    output.Close();
}

void truthPi0Analysis(const string &i_input,
                      const string &QVecCorr,
                      Float_t       z           = 10, /*cm*/
                      const string &i_output    = "test.root",
                      Int_t         anaType     = 1,
                      Int_t         subsamples  = 1,
                      Long64_t      start       = 0,
                      Long64_t      end         = 0) {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "   << i_input << endl;
    cout << "QVecCorr: "    << QVecCorr << endl;
    cout << "z: "           << z << endl;
    cout << "outputFile: "  << i_output << endl;
    cout << "anaType: "     << anaType << endl;
    cout << "Subsamples: "  << subsamples << endl;
    cout << "start: "       << start << endl;
    cout << "end: "         << end << endl;
    cout << "#############################" << endl;

    myAnalysis::subsamples = subsamples;
    myAnalysis::anaType    = anaType;

    Int_t ret = myAnalysis::init(i_input, QVecCorr, start, end);
    if(ret != 0) return;

    myAnalysis::process_event(z, start, end);
    myAnalysis::finalize(i_output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 3 || argc > 9){
        cout << "usage: ./truthPi0Ana inputFile QVecCorr [z] [outputFile] [anaType] [subsamples] [start] [end] " << endl;
        cout << "inputFile: containing list of root file paths" << endl;
        cout << "QVecCorr: csv file containing Q vector corrections" << endl;
        cout << "z: z-vertex cut. Default: 10 cm. Range: 0 to 30 cm." << endl;
        cout << "outputFile: location of output file. Default: test.root." << endl;
        cout << "anaType: analysis type. Default: 0." << endl;
        cout << "subsamples: number of subsamples for the vn analysis. Default: 1." << endl;
        cout << "start: start event number. Default: 0." << endl;
        cout << "end: end event number. Default: 0. (to run over all entries)." << endl;
        return 1;
    }

    Float_t  z           = 10;
    string   outputFile  = "test.root";
    Int_t    anaType     = 1;
    Int_t    subsamples  = 1;
    Long64_t start       = 0;
    Long64_t end         = 0;

    if(argc >= 4) {
        z = atof(argv[3]);
    }
    if(argc >= 5) {
        outputFile = argv[4];
    }
    if(argc >= 6) {
        anaType = atoi(argv[5]);
    }
    if(argc >= 7) {
        subsamples = atoi(argv[6]);
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

    truthPi0Analysis(argv[1], argv[2], z, outputFile, anaType, subsamples, start, end);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
