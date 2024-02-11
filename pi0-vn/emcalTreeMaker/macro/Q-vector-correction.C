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
        Float_t Q_S_x;
        Float_t Q_S_y;
        Float_t Q_N_x;
        Float_t Q_N_y;
        Float_t cent;
        Float_t z;
    };

    vector<Event> events;

    Int_t init(const string &i_input, Long64_t start = 0, Long64_t end = 0);
    Int_t readFiles(const string &i_input, Long64_t start = 0, Long64_t end = 0);
    void init_hists();

    void process_event(Float_t z_max = 10, Long64_t start = 0, Long64_t end = 0);
    void finalize(const string &i_output = "test.root", const string &i_output_csv = "test.csv");

    vector<string> cent_key = {"40-60", "20-40", "0-20"};

    TH1F* cent_dum_vec = new TH1F("cent_dum_vec","", 3, 0, 0.6);

    TH1F* hPsi_S[3][3]; // [cent][order of correction]
    TH1F* hPsi_N[3][3]; // [cent][order of correction]

    UInt_t bins_psi  = 100;
    Float_t low_psi  = -M_PI;
    Float_t high_psi = M_PI;

    // First Order Correction
    Float_t Q_S_x_avg[3] = {0};
    Float_t Q_S_y_avg[3] = {0};
    Float_t Q_N_x_avg[3] = {0};
    Float_t Q_N_y_avg[3] = {0};

    // Second Order Correction
    Float_t X_S[3][2][2] = {0}; // [cent][row][col]
    Float_t X_N[3][2][2] = {0}; // [cent][row][col]
}

void myAnalysis::init_hists() {
    for (Int_t i = 0; i < cent_key.size(); ++i) {
        for (Int_t j = 0; j < 3; ++j) {
            string name = "hPsi_S_"+cent_key[i]+"_"+to_string(j);
            string title = to_string(j) + "-th Order Corrected 2#Psi_{2}^{S}, Centrality: " + cent_key[i] + "; 2#Psi_{2}^{S}; Counts";
            hPsi_S[i][j] = new TH1F((name).c_str(), title.c_str(), bins_psi, low_psi, high_psi);

            name = "hPsi_N_"+cent_key[i]+"_"+to_string(j);
            title = to_string(j) + "-th Order Corrected 2#Psi_{2}^{N}, Centrality: " + cent_key[i] + "; 2#Psi_{2}^{N}; Counts";
            hPsi_N[i][j] = new TH1F((name).c_str(), title.c_str(), bins_psi, low_psi, high_psi);
       }
    }
}

Int_t myAnalysis::init(const string &i_input, Long64_t start, Long64_t end) {
    Int_t ret = readFiles(i_input, start, end);
    if(ret != 0) return ret;

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
    T->SetBranchStatus("Q_S_x",    true);
    T->SetBranchStatus("Q_S_y",    true);
    T->SetBranchStatus("Q_N_x",    true);
    T->SetBranchStatus("Q_N_y",    true);
    T->SetBranchStatus("vtx_z",    true);
    // T->SetBranchStatus("totalMBD", true);
    T->SetBranchStatus("centrality", true);

    Float_t Q_S_x;
    Float_t Q_S_y;
    Float_t Q_N_x;
    Float_t Q_N_y;
    Float_t totalMBD;
    Float_t cent;
    Float_t z;

    // event counter
    UInt_t evt_ctr[3] = {0};

    // First Order Correction
    Float_t Q_S_x_corr[3] = {0};
    Float_t Q_S_y_corr[3] = {0};
    Float_t Q_N_x_corr[3] = {0};
    Float_t Q_N_y_corr[3] = {0};

    // Second Order Correction
    Float_t Q2_S_x_avg[3]  = {0};
    Float_t Q2_S_y_avg[3]  = {0};
    Float_t Q2_S_xy_avg[3] = {0};
    Float_t Q2_N_x_avg[3]  = {0};
    Float_t Q2_N_y_avg[3]  = {0};
    Float_t Q2_N_xy_avg[3] = {0};

    Float_t Q_S_x_corr2[3] = {0};
    Float_t Q_S_y_corr2[3] = {0};
    Float_t Q_N_x_corr2[3] = {0};
    Float_t Q_N_y_corr2[3] = {0};

    Float_t D_S[3] = {0};
    Float_t D_N[3] = {0};
    Float_t N_S[3] = {0};
    Float_t N_N[3] = {0};

    Float_t psi_S[3][3] = {0}; // [cent][order of correction]
    Float_t psi_N[3][3] = {0}; // [cent][order of correction]

    T->SetBranchAddress("Q_S_x", &Q_S_x);
    T->SetBranchAddress("Q_S_y", &Q_S_y);
    T->SetBranchAddress("Q_N_x", &Q_N_x);
    T->SetBranchAddress("Q_N_y", &Q_N_y);
    // T->SetBranchAddress("totalMBD",   &totalMBD);
    T->SetBranchAddress("centrality", &cent);
    T->SetBranchAddress("vtx_z", &z);

    end = (end) ? min(end, T->GetEntries()-1) : T->GetEntries()-1;

    cout << "Loading Events" << endl;
    for (Long64_t i = start; i <= end; ++i) {
        if(i%100000 == 0) cout << "Progress: " << (i-start)*100./(end-start) << "%" << endl;
        // Load the data for the given tree entry
        T->GetEntry(i);

        Event event;

        event.Q_S_x = Q_S_x;
        event.Q_S_y = Q_S_y;
        event.Q_N_x = Q_N_x;
        event.Q_N_y = Q_N_y;
        event.cent  = cent;
        event.z     = z;

        events.push_back(event);
    }
    cout << "Finished Loading Events" << endl;

    cout << "Events to process: " << end-start+1 << endl;
    // loop over each event
    // first order correction
    for (Long64_t i = start; i <= end; ++i) {
        if(i%100000 == 0) cout << "Progress: " << (i-start)*100./(end-start) << "%" << endl;
        // Load the data for the given tree entry
        // T->GetEntry(i);

        // ensure the z vertex is within range
        if(abs(events[i].z) >= z_max) continue;

        // Int_t j = cent_dum_vec->FindBin(totalMBD)-1;
        Int_t j = cent_dum_vec->FindBin(events[i].cent)-1;

        // check if centrality is found in one of the specified bins
        if(j < 0 || j >= 3) continue;

        // need to reverse this index since we want to match cent_key
        j = cent_key.size() - j - 1;

        ++evt_ctr[j];

        Q_S_x_avg[j] += events[i].Q_S_x;
        Q_S_y_avg[j] += events[i].Q_S_y;
        Q_N_x_avg[j] += events[i].Q_N_x;
        Q_N_y_avg[j] += events[i].Q_N_y;
    }

    cout << endl;
    cout << "First Order Correction" << endl;
    // normalize to compute the averages
    for (Int_t i = 0; i < cent_key.size(); ++i) {
        Q_S_x_avg[i] /= evt_ctr[i];
        Q_S_y_avg[i] /= evt_ctr[i];
        Q_N_x_avg[i] /= evt_ctr[i];
        Q_N_y_avg[i] /= evt_ctr[i];

        cout << "Cent: " << cent_key[i] << ", Events: " << evt_ctr[i] << endl
             << "Q_S_x_avg: "   << Q_S_x_avg[i]
             << ", Q_S_y_avg: " << Q_S_y_avg[i]
             << ", Q_N_x_avg: " << Q_N_x_avg[i]
             << ", Q_N_y_avg: " << Q_N_y_avg[i] << endl;

        cout << endl;
    }

    // loop over each event
    // apply first order correction
    // compute second order correction
    for (Long64_t i = start; i <= end; ++i) {
        if(i%100000 == 0) cout << "Progress: " << (i-start)*100./(end-start) << "%" << endl;
        // Load the data for the given tree entry
        // T->GetEntry(i);

        // ensure the z vertex is within range
        if(abs(events[i].z) >= z_max) continue;

        // Int_t j = cent_dum_vec->FindBin(totalMBD)-1;
        Int_t j = cent_dum_vec->FindBin(events[i].cent)-1;

        // check if centrality is found in one of the specified bins
        if(j < 0 || j >= 3) continue;

        // need to reverse this index since we want to match cent_key
        j = cent_key.size() - j - 1;

        // compute the first order correction
        Q_S_x_corr[j] = events[i].Q_S_x - Q_S_x_avg[j];
        Q_S_y_corr[j] = events[i].Q_S_y - Q_S_y_avg[j];
        Q_N_x_corr[j] = events[i].Q_N_x - Q_N_x_avg[j];
        Q_N_y_corr[j] = events[i].Q_N_y - Q_N_y_avg[j];

        // compute averages required for second order correction
        Q2_S_x_avg[j]  += Q_S_x_corr[j]*Q_S_x_corr[j];
        Q2_S_y_avg[j]  += Q_S_y_corr[j]*Q_S_y_corr[j];
        Q2_S_xy_avg[j] += Q_S_x_corr[j]*Q_S_y_corr[j];
        Q2_N_x_avg[j]  += Q_N_x_corr[j]*Q_N_x_corr[j];
        Q2_N_y_avg[j]  += Q_N_y_corr[j]*Q_N_y_corr[j];
        Q2_N_xy_avg[j] += Q_N_x_corr[j]*Q_N_y_corr[j];
    }

    cout << endl;
    cout << "Second Order Correction" << endl;
    // normalize to compute the averages
    for (Int_t i = 0; i < cent_key.size(); ++i) {
        Q2_S_x_avg[i]  /= evt_ctr[i];
        Q2_S_y_avg[i]  /= evt_ctr[i];
        Q2_S_xy_avg[i] /= evt_ctr[i];
        Q2_N_x_avg[i]  /= evt_ctr[i];
        Q2_N_y_avg[i]  /= evt_ctr[i];
        Q2_N_xy_avg[i] /= evt_ctr[i];

        D_S[i] = sqrt(Q2_S_x_avg[i]*Q2_S_y_avg[i] - Q2_S_xy_avg[i]*Q2_S_xy_avg[i]);
        D_N[i] = sqrt(Q2_N_x_avg[i]*Q2_N_y_avg[i] - Q2_N_xy_avg[i]*Q2_N_xy_avg[i]);

        N_S[i] = D_S[i]*(Q2_S_x_avg[i]+Q2_S_y_avg[i]+2*D_S[i]);
        N_N[i] = D_N[i]*(Q2_N_x_avg[i]+Q2_N_y_avg[i]+2*D_N[i]);

        // Compute matrix elements
        X_S[i][0][0] = 1/sqrt(N_S[i])*(Q2_S_y_avg[i]+D_S[i]);
        X_S[i][0][1] = -1/sqrt(N_S[i])*(Q2_S_xy_avg[i]);
        X_S[i][1][0] = X_S[i][0][1];
        X_S[i][1][1] = 1/sqrt(N_S[i])*(Q2_S_x_avg[i]+D_S[i]);

        X_N[i][0][0] = 1/sqrt(N_N[i])*(Q2_N_y_avg[i]+D_N[i]);
        X_N[i][0][1] = -1/sqrt(N_N[i])*(Q2_N_xy_avg[i]);
        X_N[i][1][0] = X_N[i][0][1];
        X_N[i][1][1] = 1/sqrt(N_N[i])*(Q2_N_x_avg[i]+D_N[i]);

        cout << "Cent: "          << cent_key[i] << ", Events: " << evt_ctr[i] << endl
             << "Q2_S_x_avg: "    << Q2_S_x_avg[i]
             << ", Q2_S_y_avg: "  << Q2_S_y_avg[i]
             << ", Q2_S_xy_avg: " << Q2_S_xy_avg[i] << endl
             << "Q2_N_x_avg: "    << Q2_N_x_avg[i]
             << ", Q2_N_y_avg: "  << Q2_N_y_avg[i]
             << ", Q2_N_xy_avg: " << Q2_N_xy_avg[i] << endl
             << "D_S: " << D_S[i] << ", D_N: " << D_N[i] << endl
             << "N_S: " << N_S[i] << ", N_N: " << N_N[i] << endl
             << "X_S_00: "   << X_S[i][0][0]
             << ", X_S_01: " << X_S[i][0][1]
             << ", X_S_11: " << X_S[i][1][1] << endl
             << "X_N_00: "   << X_N[i][0][0]
             << ", X_N_01: " << X_N[i][0][1]
             << ", X_N_11: " << X_N[i][1][1] << endl;
        cout << endl;
    }

    // loop over each event
    // apply second order correction
    for (Long64_t i = start; i <= end; ++i) {
        if(i%100000 == 0) cout << "Progress: " << (i-start)*100./(end-start) << "%" << endl;
        // Load the data for the given tree entry
        // T->GetEntry(i);

        // ensure the z vertex is within range
        if(abs(events[i].z) >= z_max) continue;

        // Int_t j = cent_dum_vec->FindBin(totalMBD)-1;
        Int_t j = cent_dum_vec->FindBin(events[i].cent)-1;

        // check if centrality is found in one of the specified bins
        if(j < 0 || j >= 3) continue;

        // need to reverse this index since we want to match cent_key
        j = cent_key.size() - j - 1;

        // compute first order correction
        Q_S_x_corr[j] = events[i].Q_S_x - Q_S_x_avg[j];
        Q_S_y_corr[j] = events[i].Q_S_y - Q_S_y_avg[j];
        Q_N_x_corr[j] = events[i].Q_N_x - Q_N_x_avg[j];
        Q_N_y_corr[j] = events[i].Q_N_y - Q_N_y_avg[j];

        // compute second order correction
        Q_S_x_corr2[j] = X_S[j][0][0]*Q_S_x_corr[j]+X_S[j][0][1]*Q_S_y_corr[j];
        Q_S_y_corr2[j] = X_S[j][1][0]*Q_S_x_corr[j]+X_S[j][1][1]*Q_S_y_corr[j];
        Q_N_x_corr2[j] = X_N[j][0][0]*Q_N_x_corr[j]+X_N[j][0][1]*Q_N_y_corr[j];
        Q_N_y_corr2[j] = X_N[j][1][0]*Q_N_x_corr[j]+X_N[j][1][1]*Q_N_y_corr[j];

        // compute Psi
        // no correction
        Float_t psi = atan2(events[i].Q_S_y, events[i].Q_S_x);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi_S[j][0]->Fill(psi);

        psi = atan2(events[i].Q_N_y, events[i].Q_N_x);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi_N[j][0]->Fill(psi);

        // order 1
        psi = atan2(Q_S_y_corr[j], Q_S_x_corr[j]);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi_S[j][1]->Fill(psi);

        psi = atan2(Q_N_y_corr[j], Q_N_x_corr[j]);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi_N[j][1]->Fill(psi);

        // order 2
        psi = atan2(Q_S_y_corr2[j], Q_S_x_corr2[j]);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi_S[j][2]->Fill(psi);

        psi = atan2(Q_N_y_corr2[j], Q_N_x_corr2[j]);
        // psi = (psi < 0) ? (2*M_PI+psi)/2 : psi/2;
        hPsi_N[j][2]->Fill(psi);
    }

    cout << "Finish Process Event" << endl;
}


void myAnalysis::finalize(const string &i_output, const string &i_output_csv) {
    TFile output(i_output.c_str(),"recreate");

    for (Int_t i = 0; i < cent_key.size(); ++i) {
        output.mkdir((cent_key[i]).c_str());
        output.cd((cent_key[i]).c_str());

        for (Int_t j = 0; j < 3; ++j) {
            hPsi_S[i][j]->Write();
            hPsi_N[i][j]->Write();
       }
    }
    output.Close();

    std::ofstream file(i_output_csv.c_str());

    // Write header
    file << "Q_S_x_avg, Q_S_y_avg, Q_N_x_avg, Q_N_y_avg, X_S_00, X_S_01, X_S_11, X_N_00, X_N_01, X_N_11" << endl;

    for (Int_t i = 0; i < cent_key.size(); ++i) {
        file << Q_S_x_avg[i] << ","
             << Q_S_y_avg[i] << ","
             << Q_N_x_avg[i] << ","
             << Q_N_y_avg[i] << ","
             << X_S[i][0][0] << ","
             << X_S[i][0][1] << ","
             << X_S[i][1][1] << ","
             << X_N[i][0][0] << ","
             << X_N[i][0][1] << ","
             << X_N[i][1][1] << endl;
    }

    file.close();
}

void Q_vector_correction(const string &i_input,
                         Float_t z                  = 10,
                         const string &i_output     = "test.root",
                         const string &i_output_csv = "test.csv",
                         Long64_t      start        = 0,
                         Long64_t      end          = 0) {

    cout << "#############################"     << endl;
    cout << "Run Parameters"                    << endl;
    cout << "inputFile: "       << i_input      << endl;
    cout << "z: "               << z << endl;
    cout << "outputFile: "      << i_output     << endl;
    cout << "output CSV File: " << i_output_csv << endl;
    cout << "start: "           << start        << endl;
    cout << "end: "             << end          << endl;
    cout << "#############################"     << endl;

    Int_t ret = myAnalysis::init(i_input, start, end);
    if(ret != 0) return;

    myAnalysis::process_event(z, start, end);
    myAnalysis::finalize(i_output, i_output_csv);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 7){
        cout << "usage: ./Q-vec-corr inputFile [z] [outputFile] [output_csv] [start] [end] " << endl;
        cout << "inputFile: containing list of root file paths" << endl;
        cout << "z: z-vertex cut. Default: 10 cm. Range: 0 to 30 cm." << endl;
        cout << "outputFile: location of output file. Default: test.root." << endl;
        cout << "start: start event number. Default: 0." << endl;
        cout << "end: end event number. Default: 0. (to run over all entries)." << endl;
        return 1;
    }

    Float_t z         = 10;
    string outputFile = "test.root";
    string output_csv = "test.csv";
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

    Q_vector_correction(argv[1], z, outputFile, output_csv, start, end);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
