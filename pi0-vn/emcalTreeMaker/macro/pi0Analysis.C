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

namespace myAnalysis {
    TChain* T;

    struct Cut {
        Float_t e;      // min cluster energy
        Float_t e_asym;   // max cluster pair energy asymmetry: |E1-E2|/(E1+E2), where E1 and E2 is cluster ecore
        Float_t deltaR; // min cluster pair deltaR
        Float_t chi;    // max cluster chi2
    };

    vector<Cut> cuts;

    Int_t init(const string &i_input, const string &i_cuts, Long64_t start = 0, Long64_t end = 0);
    Int_t readFiles(const string &i_input, Long64_t start = 0, Long64_t end = 0);
    Int_t readCuts(const string &i_cuts);
    void init_hists();

    void process_event(Long64_t start = 0, Long64_t end = 0);
    void finalize(const string &i_output = "test.root");

    // map<string,pair<Float_t, Float_t>> centrality = {{"0-20",   make_pair(939.074, 2000)},
    //                                                  {"20-40",  make_pair(472.037, 939.074)},
    //                                                  {"40-60",  make_pair(187.222, 472.037)},
    //                                                  {"60-100", make_pair(0, 187.222)}};

    map<string,pair<Float_t, Float_t>> centrality = {{"20-40",  make_pair(472.037, 939.074)},
                                                     {"40-60",  make_pair(187.222, 472.037)}};

    map<string,pair<Float_t, Float_t>> diphoton_pt = {{"2-2.5", make_pair(2, 2.5)},
                                                      {"2.5-3", make_pair(2.5, 3)},
                                                      {"3-3.5", make_pair(3, 3.5)},
                                                      {"3.5-4", make_pair(3.5, 4)},
                                                      {"4-4.5", make_pair(4, 4.5)},
                                                      {"4.5-5", make_pair(4.5, 5)}};


    map<pair<string,string>, vector<TH1F*>> hPi0Mass; // hPi0Mass[make_pair(cent,pt)][i], for accessing diphoton invariant mass hist of i-th cut of cent,pt

    map<pair<string,string>, TH2F*> h2DeltaRVsMass;
    map<pair<string,string>, TH2F*> h2AsymVsMass;
    map<pair<string,string>, TH2F*> h2TimingAsymVsMass;
    map<pair<string,string>, TH2F*> h2ChiAsymVsMass;
    map<string, TH1F*>              hDiphotonPt;

    Int_t   bins_pi0_mass = 80;
    Float_t hpi0_mass_min = 0;
    Float_t hpi0_mass_max = 1;

    Int_t   bins_pt   = 500;
    Float_t hpt_min   = 0;
    Float_t hpt_max   = 20;

    Int_t   bins_deltaR   = 100;
    Float_t hdeltaR_min   = 0;
    Float_t hdeltaR_max   = 4;

    Int_t   bins_asym   = 100;
    Float_t hasym_min   = 0;
    Float_t hasym_max   = 1;

    Int_t   bins_time   = 32;
    Float_t htime_min   = 0;
    Float_t htime_max   = 32;
}

Int_t myAnalysis::init(const string &i_input, const string &i_cuts, Long64_t start, Long64_t end) {
    Int_t ret = readFiles(i_input, start, end);
    if(ret != 0) return ret;

    ret = readCuts(i_cuts);
    if(ret != 0) return ret;

    init_hists();

    // Disable everything...
    T->SetBranchStatus("*", false);
    // ...but the branch we need
    T->SetBranchStatus("totalMBD",  true);
    T->SetBranchStatus("clus_E",    true);
    T->SetBranchStatus("clus_eta",  true);
    T->SetBranchStatus("clus_phi",  true);
    T->SetBranchStatus("clus_chi",  true);
    T->SetBranchStatus("clus_time", true);
    T->SetBranchStatus("clus_E2",   true);
    T->SetBranchStatus("clus_eta2", true);
    T->SetBranchStatus("clus_phi2", true);
    T->SetBranchStatus("clus_chi2", true);
    T->SetBranchStatus("clus_time2",true);
    T->SetBranchStatus("pi0_mass",  true);
    T->SetBranchStatus("pi0_pt",    true);

    return 0;
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
    Int_t entries = 0;
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

            h2TimingAsymVsMass[key] = new TH2F(("h2TimingAsymVsMass"+suffix).c_str(),
                                            ("Cluster Timing Asymmetry vs Diphoton Invariant Mass, " + suffix_title +"; Mass [GeV]; Timing Asymmetry").c_str(),
                                            bins_pi0_mass, hpi0_mass_min, hpi0_mass_max,
                                            bins_asym, hasym_min, hasym_max);

            h2ChiAsymVsMass[key] = new TH2F(("h2ChiAsymVsMass"+suffix).c_str(),
                                            ("Cluster #chi^{2} Asymmetry vs Diphoton Invariant Mass, " + suffix_title +"; Mass [GeV]; #chi^{2} Asymmetry").c_str(),
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

    Float_t totalMBD;
    Float_t clus_E;
    Float_t clus_eta;
    Float_t clus_phi;
    Float_t clus_chi;
    Float_t clus_time;
    Float_t clus_E2;
    Float_t clus_eta2;
    Float_t clus_phi2;
    Float_t clus_chi2;
    Float_t clus_time2;
    Float_t pi0_mass;
    Float_t pi0_pt;

    T->SetBranchAddress("totalMBD",  &totalMBD);
    T->SetBranchAddress("clus_E",    &clus_E);
    T->SetBranchAddress("clus_eta",  &clus_eta);
    T->SetBranchAddress("clus_phi",  &clus_phi);
    T->SetBranchAddress("clus_chi",  &clus_chi);
    T->SetBranchAddress("clus_time", &clus_time);
    T->SetBranchAddress("clus_E2",   &clus_E2);
    T->SetBranchAddress("clus_eta2", &clus_eta2);
    T->SetBranchAddress("clus_phi2", &clus_phi2);
    T->SetBranchAddress("clus_chi2", &clus_chi2);
    T->SetBranchAddress("clus_time2",&clus_time2);
    T->SetBranchAddress("pi0_mass",  &pi0_mass);
    T->SetBranchAddress("pi0_pt",    &pi0_pt);

    end = (end) ? min(end, T->GetEntries()-1) : T->GetEntries()-1;

    UInt_t ctr = 0;
    cout << "Entries to process: " << end-start << endl;
    // loop over each diphoton candidate
    for (Long64_t i = start; i <= end; ++i) {
        if(i%500000 == 0) cout << "Progress: " << (i-start)*100./(end-start) << "%" << endl;
        // Load the data for the given tree entry
        T->GetEntry(i);

        Float_t deltaPhi = abs(clus_phi-clus_phi2);
        deltaPhi = (deltaPhi > M_PI) ? 2*M_PI - deltaPhi : deltaPhi; // ensure that deltaPhi is in [0,pi]

        Float_t deltaR = sqrt(pow(clus_eta-clus_eta2,2)+pow(deltaPhi,2));
        Float_t e_asym = abs(clus_E-clus_E2)/(clus_E+clus_E2);
        Float_t time_asym = abs(clus_time-clus_time2)/(clus_time+clus_time2);
        Float_t chi_asym = abs(clus_chi-clus_chi2)/(clus_chi+clus_chi2);

        // fill in the pi0 mass histogram for each cut
        // fill in QA plots for each centrality/pt bin
        bool flag = false;
        for(auto cent : centrality) {

            if(totalMBD >= cent.second.first && totalMBD < cent.second.second) {
               hDiphotonPt[cent.first]->Fill(pi0_pt);
            }

            for(auto pt : diphoton_pt) {

                if(totalMBD >= cent.second.first && totalMBD < cent.second.second &&
                   pi0_pt   >= pt.second.first   && pi0_pt   < pt.second.second) {

                    pair<string,string> key = make_pair(cent.first, pt.first);

                    h2DeltaRVsMass[key]    ->Fill(pi0_mass, deltaR);
                    h2AsymVsMass[key]      ->Fill(pi0_mass, e_asym);
                    h2TimingAsymVsMass[key]->Fill(pi0_mass, time_asym);
                    h2ChiAsymVsMass[key]   ->Fill(pi0_mass, chi_asym);

                    for(Int_t i = 0; i < cuts.size(); ++i) {

                        if(clus_E >= cuts[i].e     && clus_E2 >= cuts[i].e &&
                           clus_chi < cuts[i].chi  && clus_chi2 < cuts[i].chi &&
                           e_asym < cuts[i].e_asym && deltaR >= cuts[i].deltaR) {

                           hPi0Mass[key][i]->Fill(pi0_mass);
                        }
                    }

                    ++ctr;
                    flag = true;
                    break;
                }
            }
            if(flag) break;
        }
    }

    cout << "Accepted Entries: " << ctr << ", " << ctr*100./(end-start) << " %" << endl;
    cout << "Finish Process Event" << endl;
}


void myAnalysis::finalize(const string &i_output) {
    TFile output(i_output.c_str(),"recreate");
    output.mkdir("results");
    output.mkdir("QA/hDiphotonPt");
    output.mkdir("QA/h2DeltaRVsMass");
    output.mkdir("QA/h2AsymVsMass");
    output.mkdir("QA/h2TimingAsymVsMass");
    output.mkdir("QA/h2ChiAsymVsMass");

    for(auto cent : centrality) {
        output.cd("QA/hDiphotonPt");
        hDiphotonPt[cent.first]->Write();

        for(auto pt : diphoton_pt) {
            pair<string,string> key = make_pair(cent.first, pt.first);

            output.cd("QA/h2DeltaRVsMass");
            h2DeltaRVsMass[key]    ->Write();

            output.cd("QA/h2AsymVsMass");
            h2AsymVsMass[key]      ->Write();

            output.cd("QA/h2TimingAsymVsMass");
            h2TimingAsymVsMass[key]->Write();

            output.cd("QA/h2ChiAsymVsMass");
            h2ChiAsymVsMass[key]   ->Write();

            output.mkdir(("results/"+cent.first+"/"+pt.first).c_str());
            output.cd(("results/"+cent.first+"/"+pt.first).c_str());

            for(Int_t i = 0; i < cuts.size(); ++i) {
                hPi0Mass[key][i]->Write();
            }
        }
    }

    output.Close();
}

void pi0Analysis(const string &i_input,
                 const string &i_cuts,
                 Long64_t      start    = 0,
                 Long64_t      end      = 0,
                 const string &i_output = "test.root") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "  << i_input << endl;
    cout << "Cuts: "       << i_cuts << endl;
    cout << "start: "      << start << endl;
    cout << "end: "        << end << endl;
    cout << "outputFile: " << i_output << endl;
    cout << "#############################" << endl;

    Int_t ret = myAnalysis::init(i_input, i_cuts, start, end);
    if(ret != 0) return;

    myAnalysis::process_event(start,end);
    myAnalysis::finalize(i_output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 3 || argc > 6){
        cout << "usage: ./pi0Ana inputFile cuts [start] [end] [outputFile]" << endl;
        cout << "inputFile: containing list of root file paths" << endl;
        cout << "cuts: csv file containing cuts" << endl;
        cout << "start: start event number. Default: 0." << endl;
        cout << "end: end event number. Default: 0. (to run over all entries)." << endl;
        cout << "outputFile: location of output file. Default: test.root." << endl;
        return 1;
    }

    Long64_t start    = 0;
    Long64_t end      = 0;
    string outputFile = "test.root";

    if(argc >= 4) {
        start = atol(argv[3]);
    }
    if(argc >= 5) {
        end = atol(argv[4]);
    }
    if(argc >= 6) {
        outputFile = argv[5];
    }

    // ensure that 0 <= start <= end
    if(start > end || start < 0) {
        cerr << "Error: Invalid start, end provided! " << endl;
        return 1;
    }

    pi0Analysis(argv[1], argv[2], start, end, outputFile);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
