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
    TTree*  tree;

    struct Cut {
        Float_t e;      // min cluster energy
        Float_t e_asym;   // max cluster pair energy asymmetry: |E1-E2|/(E1+E2), where E1 and E2 is cluster ecore
        Float_t deltaR; // min cluster pair deltaR
        Float_t chi;    // max cluster chi2
    };

    vector<Cut> cuts;

    Int_t init(const string &i_input, const string &i_cuts, const string &fitStats, Long64_t start = 0, Long64_t end = 0);
    Int_t readFiles(const string &i_input, Long64_t start = 0, Long64_t end = 0);
    Int_t readCuts(const string &i_cuts);
    Int_t readFitStats(const string &fitStats);
    void init_hists();

    void process_event(Long64_t start = 0, Long64_t end = 0);
    void finalize(const string &i_output = "test.root");

    map<string,pair<Float_t, Float_t>> centrality = {{"40-60",  make_pair(215, 497.222)},
                                                     {"20-40",  make_pair(497.222, 955.741)}};

    map<string,pair<Float_t, Float_t>> diphoton_pt = {{"2-2.5", make_pair(2,   2.5)},
                                                      {"2.5-3", make_pair(2.5, 3)},
                                                      {"3-3.5", make_pair(3,   3.5)},
                                                      {"3.5-4", make_pair(3.5, 4)},
                                                      {"4-4.5", make_pair(4,   4.5)},
                                                      {"4.5-5", make_pair(4.5, 5)}};

    vector<string> cent_key = {"40-60", "20-40"};
    vector<string> pt_key   = {"2-2.5", "2.5-3", "3-3.5", "3.5-4", "4-4.5", "4.5-5"};

    TH1F* pt_dum_vec   = new TH1F("pt_dum_vec","",6,2,5);
    TH1F* cent_dum_vec = new TH1F("cent_dum_vec","", 2, new Double_t[3] {215, 497.222, 955.741});

    vector<Float_t> pi0_ctr(cent_key.size()*pt_key.size());

    vector<UInt_t> evt_ctr(cent_key.size());

    vector<Float_t> QQ(cent_key.size());

    // contains the aggregrate sum of dot product of pi0 q vector and MBD Q vector
    vector<Float_t> qQ(cent_key.size()*pt_key.size());

    // keep track of low and high pi0 mass values to filter on for the computation of the v2
    vector<pair<Float_t,Float_t>> pi0_mass_range(cent_key.size()*pt_key.size());

    map<pair<string,string>, vector<TH1F*>> hPi0Mass; // hPi0Mass[make_pair(cent,pt)][i], for accessing diphoton invariant mass hist of i-th cut of cent,pt

    map<pair<string,string>, TH2F*> h2DeltaRVsMass;
    map<pair<string,string>, TH2F*> h2AsymVsMass;
    map<string, TH1F*>              hDiphotonPt;

    Int_t   bins_pi0_mass = 48;
    Float_t hpi0_mass_min = 0;
    Float_t hpi0_mass_max = 0.6;

    Int_t   bins_pt = 500;
    Float_t hpt_min = 0;
    Float_t hpt_max = 20;

    Int_t   bins_deltaR   = 100;
    Float_t hdeltaR_min   = 0;
    Float_t hdeltaR_max   = 4;

    Int_t   bins_asym = 100;
    Float_t hasym_min = 0;
    Float_t hasym_max = 1;

    Bool_t use_mass_range = false;
}

Int_t myAnalysis::init(const string &i_input, const string &i_cuts, const string& fitStats, Long64_t start, Long64_t end) {
    T = new TChain("T");
    T->Add(i_input.c_str());

    tree = new TTree("flow","flow");
    tree->Branch("pi0_ctr", &pi0_ctr);
    tree->Branch("evt_ctr", &evt_ctr);
    tree->Branch("QQ", &QQ);
    tree->Branch("qQ", &qQ);
    Int_t ret;
    // ret = readFiles(i_input, start, end);
    // if(ret != 0) return ret;

    Int_t ret = readCuts(i_cuts);
    if(ret != 0) return ret;

    init_hists();

    if(!fitStats.empty()) ret = readFitStats(fitStats);

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

        if(entries > end) break;
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
        cerr << "Failed to open cuts file: " << fitStats << endl;
        return 1;
    }

    int col1Idx = 15; // Replace with the index of the first column you want
    int col2Idx = 16; // Replace with the index of the second column you want

    std::string line;
    Int_t idx = 0;
    while (std::getline(file, line)) {

        // skip header line
        if(!idx++) continue;

        std::stringstream ss(line);
        std::string token;

        int colIdx = 0;
        while (std::getline(ss, token, ',')) { // Assuming comma-separated values
            if (colIdx == col1Idx) {
                // load pi0_mass low val
                pi0_mass_range[idx-2].first = stof(token);
            } else if (colIdx == col2Idx) {
                // load pi0_mass high val
                pi0_mass_range[idx-2].second = stof(token);
            }
            colIdx++;
        }
    }

    use_mass_range = true;

    cout << endl;
    for(Int_t i = 0; i < cent_key.size(); ++i) {
        cout << "cent: " << cent_key[i] << endl;

        for(Int_t j = 0; j < pt_key.size(); ++j) {
            Int_t idx = i*pt_key.size()+j;

            cout << "pt: " << pt_key[j]
                 << ", low mass val: "  << pi0_mass_range[idx].first
                 << ", high mass val: " << pi0_mass_range[idx].second << endl;
        }
        cout << endl;
    }

    return 0;
}


void myAnalysis::init_hists() {

    stringstream s;
    stringstream t;
    // create a pi0 mass hist for each cut
    // create QA plots for each centrality/pt bin
    for(auto cent : centrality) {

        string suffix_title =  "Centrality: " + cent.first + "%";
        hDiphotonPt[cent.first] = new TH1F(("hDiphotonPt_"+cent.first).c_str(), ("Diphoton p_{T}, " + suffix_title +"; p_{T} [GeV]; Counts").c_str(), bins_pt, hpt_min, hpt_max);

        for(auto pt : diphoton_pt) {

            pair<string,string> key = make_pair(cent.first,pt.first);
            string suffix = "_"+cent.first+"_"+pt.first;
            suffix_title = "Centrality: " + cent.first + "%, Diphoton p_{T}: " + pt.first + " GeV";


            h2DeltaRVsMass[key] = new TH2F(("h2DeltaRVsMass"+suffix).c_str(),
                                            ("#Delta R vs Diphoton Invariant Mass, " + suffix_title +"; Mass [GeV]; #Delta R").c_str(),
                                            bins_pi0_mass, hpi0_mass_min, hpi0_mass_max,
                                            bins_deltaR, hdeltaR_min, hdeltaR_max);

            h2AsymVsMass[key] = new TH2F(("h2AsymVsMass"+suffix).c_str(),
                                            ("Cluster Energy Asymmetry vs Diphoton Invariant Mass, " + suffix_title +"; Mass [GeV]; Energy Asymmetry").c_str(),
                                            bins_pi0_mass, hpi0_mass_min, hpi0_mass_max,
                                            bins_asym, hasym_min, hasym_max);

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

void myAnalysis::process_event(Long64_t start, Long64_t end) {
    cout << "======================================" << endl;
    cout << "Begin Process Event" << endl;
    cout << "======================================" << endl;

    Int_t   run;
    Int_t   event;
    Float_t totalMBD;
    Float_t Q_N_x;
    Float_t Q_N_y;
    Float_t Q_P_x;
    Float_t Q_P_y;
    vector<Float_t>* pi0_mass   = 0;
    vector<Float_t>* pi0_pt     = 0;
    vector<Float_t>* pi0_phi    = 0;
    vector<Float_t>* pi0_eta    = 0;
    vector<Float_t>* pi0_asym   = 0;
    vector<Float_t>* pi0_deltaR = 0;
    vector<Float_t>* ecore_min  = 0;
    vector<Float_t>* chi2_max   = 0;

    T->SetBranchAddress("run",       &run);
    T->SetBranchAddress("event",     &event);
    T->SetBranchAddress("totalMBD",  &totalMBD);
    T->SetBranchAddress("Q_N_x",     &Q_N_x);
    T->SetBranchAddress("Q_N_y",     &Q_N_y);
    T->SetBranchAddress("Q_P_x",     &Q_P_x);
    T->SetBranchAddress("Q_P_y",     &Q_P_y);
    T->SetBranchAddress("pi0_mass",  &pi0_mass);
    T->SetBranchAddress("pi0_pt",    &pi0_pt);
    T->SetBranchAddress("pi0_phi",   &pi0_phi);
    T->SetBranchAddress("pi0_eta",   &pi0_eta);
    T->SetBranchAddress("asym",      &pi0_asym);
    T->SetBranchAddress("deltaR",    &pi0_deltaR);
    T->SetBranchAddress("ecore_min", &ecore_min);
    T->SetBranchAddress("chi2_max",  &chi2_max);

    end = (end) ? min(end, T->GetEntries()-1) : T->GetEntries()-1;

    UInt_t max_npi0 = 0;

    cout << "Events to process: " << end-start << endl;
    // loop over each event
    for (Long64_t i = start; i <= end; ++i) {
        if(i%100 == 0) cout << "Progress: " << (i-start)*100./(end-start) << "%" << endl;
        // Load the data for the given tree entry
        T->GetEntry(i);

        Int_t cent_idx = cent_dum_vec->FindBin(totalMBD)-1;

        // check if centrality is found in one of the specified bins
        if(cent_idx < 0 || cent_idx >= 2) continue;

        ++evt_ctr[cent_idx];

        QQ[cent_idx] += Q_N_x*Q_P_x + Q_N_y*Q_P_y;

        UInt_t n = pi0_mass->size();

        max_npi0 = max(max_npi0, n);

        // loop over all diphoton candidates
        for(UInt_t j = 0; j < n; ++j) {
            Float_t pi0_pt_val = pi0_pt->at(j);
            Int_t pt_idx = pt_dum_vec->FindBin(pi0_pt_val)-1;

            hDiphotonPt[cent_key[cent_idx]]->Fill(pi0_pt_val);
            // check if pt is found in one of the specified bins
            if(pt_idx < 0 || pt_idx >= 6) continue;

            pair<string,string> key = make_pair(cent_key[cent_idx], pt_key[pt_idx]);

            Float_t pi0_mass_val  = pi0_mass->at(j);
            Float_t pi0_phi_val   = pi0_phi->at(j);
            Float_t pi0_eta_val   = pi0_eta->at(j);
            Float_t asym_val      = pi0_asym->at(j);
            Float_t deltaR_val    = pi0_deltaR->at(j);
            Float_t ecore_min_val = ecore_min->at(j);
            Float_t chi2_max_val  = chi2_max->at(j);

            Float_t Q_x = (pi0_eta_val < 0) ? Q_P_x : Q_N_x;
            Float_t Q_y = (pi0_eta_val < 0) ? Q_P_y : Q_N_y;

            Float_t q_x = cos(2*pi0_phi_val);
            Float_t q_y = sin(2*pi0_phi_val);

            Float_t qQ_val = q_x*Q_x + q_y*Q_y;

            h2AsymVsMass[key]->Fill(pi0_mass_val, asym_val);
            h2DeltaRVsMass[key]->Fill(pi0_mass_val, deltaR_val);

            Int_t idx = cent_idx*pt_key.size()+pt_idx;

            for(Int_t k = 0; k < cuts.size(); ++k) {
                if(ecore_min_val >= cuts[k].e      && asym_val     < cuts[k].e_asym &&
                   deltaR_val    >= cuts[k].deltaR && chi2_max_val < cuts[k].chi) {
                    hPi0Mass[key][k]->Fill(pi0_mass_val);

                    // add condition to check if mass is in range
                    // do this for only one of the cuts for which we have signal bound information
                    if(k == 0 && use_mass_range && pi0_mass_val >= pi0_mass_range[idx].first && pi0_mass_val < pi0_mass_range[idx].second) {
                        ++pi0_ctr[idx];
                        qQ[idx] += qQ_val;
                    }
                }
            }
        }
    }

    tree->Fill();

    cout << endl;
    for(Int_t i = 0; i < cent_key.size(); ++i) {
        QQ[i] /= evt_ctr[i];
        cout << "Cent: "     << cent_key[i]
             << ", Events: " << evt_ctr[i]
             << ", avg QQ: " << QQ[i] << endl;

        for(Int_t j = 0; j < pt_key.size(); ++j) {
            Int_t key = i*pt_key.size()+j;
            qQ[key] = (pi0_ctr[key]) ? qQ[key]/pi0_ctr[key] : 0;

            cout << "pT: "          << pt_key[j]
                 << ", asym: "      << cuts[0].e_asym
                 << ", ECore_min: " << cuts[0].e
                 << ", pi0s: "      << pi0_ctr[key]
                 << ", avg qQ: "    << qQ[key] << endl;
        }
        cout << endl;
    }

    cout << "Max Pi0s per event: " << max_npi0 << endl;
    cout << "Finish Process Event" << endl;
}


void myAnalysis::finalize(const string &i_output) {
    TFile output(i_output.c_str(),"recreate");
    output.mkdir("results");
    output.mkdir("QA/hDiphotonPt");
    output.mkdir("QA/h2DeltaRVsMass");
    output.mkdir("QA/h2AsymVsMass");

    for(auto cent : centrality) {
        output.cd("QA/hDiphotonPt");
        hDiphotonPt[cent.first]->Write();

        for(auto pt : diphoton_pt) {
            pair<string,string> key = make_pair(cent.first, pt.first);

            output.cd("QA/h2DeltaRVsMass");
            h2DeltaRVsMass[key]    ->Write();

            output.cd("QA/h2AsymVsMass");
            h2AsymVsMass[key]      ->Write();

            output.mkdir(("results/"+cent.first+"/"+pt.first).c_str());
            output.cd(("results/"+cent.first+"/"+pt.first).c_str());

            for(Int_t i = 0; i < cuts.size(); ++i) {
                hPi0Mass[key][i]->Write();
            }
        }
    }

    output.cd();
    tree->Write();

    output.Close();
}

void pi0Analysis(const string &i_input,
                 const string &i_cuts,
                 const string &fitStats = "",
                 const string &i_output = "test.root",
                 Long64_t      start    = 0,
                 Long64_t      end      = 0) {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "  << i_input << endl;
    cout << "Cuts: "       << i_cuts << endl;
    cout << "fitStats: "   << fitStats << endl;
    cout << "outputFile: " << i_output << endl;
    cout << "start: "      << start << endl;
    cout << "end: "        << end << endl;
    cout << "#############################" << endl;

    Int_t ret = myAnalysis::init(i_input, i_cuts, fitStats, start, end);
    if(ret != 0) return;

    myAnalysis::process_event(start, end);
    myAnalysis::finalize(i_output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 3 || argc > 7){
        cout << "usage: ./pi0Ana inputFile cuts [fitStats] [outputFile] [start] [end] " << endl;
        cout << "inputFile: containing list of root file paths" << endl;
        cout << "cuts: csv file containing cuts" << endl;
        cout << "fitStats: csv file containing fit stats" << endl;
        cout << "outputFile: location of output file. Default: test.root." << endl;
        cout << "start: start event number. Default: 0." << endl;
        cout << "end: end event number. Default: 0. (to run over all entries)." << endl;
        return 1;
    }

    string fitStats   = "";
    string outputFile = "test.root";
    Long64_t start    = 0;
    Long64_t end      = 0;

    if(argc >= 4) {
        fitStats = argv[3];
    }
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

    pi0Analysis(argv[1], argv[2], fitStats, outputFile, start, end);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
