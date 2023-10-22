// -- c++ includes --
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// -- root includes --
#include <TH2F.h>
#include <TFile.h>
#include <TChain.h>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::min;
using std::max;
using std::left;
using std::setw;

namespace myAnalysis {
    TChain* T;

    struct Cut {
        Float_t pi0_pt_min;
        Float_t pi0_pt_max;
        Float_t e1;
        Float_t e2;
        Float_t e_asym;
        Float_t deltaR;
        Float_t chi1;
        Float_t chi2;
    };

    vector<Cut> cuts;

    Int_t init(const string &i_input, const string &i_cuts);
    Int_t readFiles(const string &i_input);
    Int_t readCuts(const string &i_cuts);
    void init_hists();

    void process_event(Long64_t events = 0);
    void finalize(const string &i_output = "test.root");

    TH2F* h2ClusE;
    TH2F* h2ClusEta;
    TH2F* h2ClusPhi;
    TH2F* h2ClusChi;
    TH1F* hDeltaR;
    TH1F* hEAsym;

    TH1F* hPi0Mass;
    TH1F* hPi0Pt;
    TH2F* h2Pi0EtaPhi;
    TH2F* h2Pi0MassPt;

    TH2F* h2Pi0MassDeltaR;
    TH2F* h2Pi0MassEAsym;

    TH2F* h2Pi0MassClusE1;
    TH2F* h2Pi0MassClusE2;
    TH2F* h2Pi0MassClusEta1;
    TH2F* h2Pi0MassClusEta2;
    TH2F* h2Pi0MassClusPhi1;
    TH2F* h2Pi0MassClusPhi2;
    TH2F* h2Pi0MassClusChi1;
    TH2F* h2Pi0MassClusChi2;

    Int_t   bins_clusE    = 500;
    Float_t hclusE_min   = 0;
    Float_t hclusE_max   = 20;

    Int_t   bins_clus_eta = 100;
    Float_t hclus_eta_min = -1.12;
    Float_t hclus_eta_max = 1.12;

    Int_t   bins_clus_phi = 100;
    Float_t hclus_phi_min = -3.15;
    Float_t hclus_phi_max = -3.15;

    Int_t   bins_clus_chi = 100;
    Float_t hclus_chi_min = 0;
    Float_t hclus_chi_max = 10;

    Int_t   bins_pi0_mass = 5000;
    Float_t hpi0_mass_min = 0;
    Float_t hpi0_mass_max = 10;

    Int_t   bins_pi0_eta  = 100;
    Float_t hpi0_eta_min  = -10;
    Float_t hpi0_eta_max  = 10;

    Int_t   bins_pi0_phi  = 100;
    Float_t hpi0_phi_min  = -3.15;
    Float_t hpi0_phi_max  = 3.15;

    Int_t   bins_pi0_pt   = 500;
    Float_t hpi0_pt_min   = 0;
    Float_t hpi0_pt_max   = 10;

    Int_t   bins_deltaR   = 100;
    Float_t hdeltaR_min   = 0;
    Float_t hdeltaR_max   = 10;

    Int_t   bins_e_asym   = 100;
    Float_t he_asym_min   = 0;
    Float_t he_asym_max   = 1;
}

Int_t myAnalysis::init(const string &i_input, const string &i_cuts) {
    Int_t ret = readFiles(i_input);
    if(ret != 0) return ret;

    ret = readCuts(i_cuts);
    if(ret != 0) return ret;

    init_hists();

    // Disable everything...
    T->SetBranchStatus("*", false);
    // ...but the branch we need
    T->SetBranchStatus("clus_E",    true);
    T->SetBranchStatus("clus_eta",  true);
    T->SetBranchStatus("clus_phi",  true);
    T->SetBranchStatus("clus_chi",  true);
    T->SetBranchStatus("clus_E2",   true);
    T->SetBranchStatus("clus_eta2", true);
    T->SetBranchStatus("clus_phi2", true);
    T->SetBranchStatus("clus_chi2", true);
    T->SetBranchStatus("pi0_mass",  true);
    T->SetBranchStatus("pi0_pt",    true);
    T->SetBranchStatus("pi0_eta",   true);
    T->SetBranchStatus("pi0_phi",   true);

    return 0;
}

Int_t myAnalysis::readFiles(const string &i_input) {
    T = new TChain("T");

    // Create an input stream
    std::ifstream file(i_input);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open the file." << endl;
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
        cerr << "Failed to open the file." << endl;
        return 1;
    }

    string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        string cell;
        char comma;

        Cut cut;

        if (lineStream >> cut.pi0_pt_min >> comma
                       >> cut.pi0_pt_max >> comma
                       >> cut.e1 >> comma
                       >> cut.e2 >> comma
                       >> cut.e_asym >> comma
                       >> cut.deltaR >> comma
                       >> cut.chi1 >> comma
                       >> cut.chi2) cuts.push_back(cut);
        else {
            cerr << "Failed to parse line: " << line << endl;
            return 1;
        }
    }

    // Close the file
    file.close();

    cout << "Cuts" << endl;
    for(auto cut : cuts) {
        cout << "pi0_pt_min: " << cut.pi0_pt_min
           << ", pi0_pt_max: " << cut.pi0_pt_max
           << ", e1: "         << cut.e1
           << ", e2: "         << cut.e2
           << ", e_asym: "     << cut.e_asym
           << ", deltaR: "     << cut.deltaR
           << ", chi1: "       << cut.chi1
           << ", chi2: "       << cut.chi2 << endl;
    }

    return 0;
}


void myAnalysis::init_hists() {
    h2ClusE   = new TH2F("h2ClusE", "Cluster Energy; Cluster 1 [GeV]; Cluster 2 [GeV]", bins_clusE, hclusE_min, hclusE_max, bins_clusE, hclusE_min, hclusE_max);
    h2ClusEta = new TH2F("h2ClusEta", "Cluster Eta; Cluster 1 #eta; Cluster 2 #eta", bins_clus_eta, hclus_eta_min, hclus_eta_max, bins_clus_eta, hclus_eta_min, hclus_eta_max);
    h2ClusPhi = new TH2F("h2ClusPhi", "Cluster Phi; Cluster 1 #phi; Cluster 2 #phi", bins_clus_phi, hclus_phi_min, hclus_phi_max, bins_clus_phi, hclus_phi_min, hclus_phi_max);
    h2ClusChi = new TH2F("h2ClusChi", "Cluster #chi^{2}; Cluster 1 #chi^{2}; Cluster 2 #chi^{2}", bins_clus_chi, hclus_chi_min, hclus_chi_max, bins_clus_chi, hclus_chi_min, hclus_chi_max);

    hDeltaR   = new TH1F("hDeltaR", "Cluster #Delta R; #Delta R; Counts", bins_deltaR, hdeltaR_min, hdeltaR_max);
    hEAsym    = new TH1F("hEAsym", "Cluster Energy Asymmetry; Energy Asymmetry; Counts", bins_e_asym, he_asym_min, he_asym_max);

    hPi0Mass    = new TH1F("hPi0Mass", "Invariant Mass; Mass [GeV]; Counts", bins_pi0_mass, hpi0_mass_min, hpi0_mass_max);
    hPi0Pt      = new TH1F("hPi0Pt", "Candidate p_{T}; p_{T} [GeV]; Counts", bins_pi0_pt, hpi0_pt_min, hpi0_pt_max);
    h2Pi0EtaPhi = new TH2F("hPi0EtaPhi", "Candidate #phi vs #eta; #eta; #phi", bins_pi0_eta, hpi0_eta_min, hpi0_eta_max, bins_pi0_phi, hpi0_phi_min, hpi0_phi_max);
    h2Pi0MassPt = new TH2F("hPi0MassPt", "Candidate p_{T} vs Mass; Mass [GeV]; p_{T} [GeV]", bins_pi0_mass, hpi0_mass_min, hpi0_mass_max, bins_pi0_pt, hpi0_pt_min, hpi0_pt_max);

    h2Pi0MassDeltaR = new TH2F("h2Pi0MassDeltaR", "Cluster #Delta R vs Invariant Mass; Mass [GeV]; #Delta R", bins_pi0_mass, hpi0_mass_min, hpi0_mass_max, bins_deltaR, hdeltaR_min, hdeltaR_max);
    h2Pi0MassEAsym  = new TH2F("h2Pi0MassEAsym", "Cluster Energy Asymmetry R vs Invariant Mass; Mass [GeV]; Energy Asymmetry", bins_pi0_mass, hpi0_mass_min, hpi0_mass_max, bins_e_asym, he_asym_min, he_asym_max);;

    h2Pi0MassClusE1   = new TH2F("h2Pi0MassClusE1", "Cluster 1 Energy vs Invariant Mass; Mass [GeV]; Energy [GeV]", bins_pi0_mass, hpi0_mass_min, hpi0_mass_max, bins_clusE, hclusE_min, hclusE_max);
    h2Pi0MassClusE2   = new TH2F("h2Pi0MassClusE2", "Cluster 2 Energy vs Invariant Mass; Mass [GeV]; Energy [GeV]", bins_pi0_mass, hpi0_mass_min, hpi0_mass_max, bins_clusE, hclusE_min, hclusE_max);

    h2Pi0MassClusEta1 = new TH2F("h2Pi0MassClusEta1", "Cluster 1 #eta vs Invariant Mass; Mass [GeV]; #eta", bins_pi0_mass, hpi0_mass_min, hpi0_mass_max, bins_clus_eta, hclus_eta_min, hclus_eta_max);
    h2Pi0MassClusEta2 = new TH2F("h2Pi0MassClusEta2", "Cluster 2 #eta vs Invariant Mass; Mass [GeV]; #eta", bins_pi0_mass, hpi0_mass_min, hpi0_mass_max, bins_clus_eta, hclus_eta_min, hclus_eta_max);

    h2Pi0MassClusPhi1 = new TH2F("h2Pi0MassClusPhi1", "Cluster 1 #phi vs Invariant Mass; Mass [GeV]; #phi", bins_pi0_mass, hpi0_mass_min, hpi0_mass_max, bins_clus_phi, hclus_phi_min, hclus_phi_max);
    h2Pi0MassClusPhi2 = new TH2F("h2Pi0MassClusPhi2", "Cluster 2 #phi vs Invariant Mass; Mass [GeV]; #phi", bins_pi0_mass, hpi0_mass_min, hpi0_mass_max, bins_clus_phi, hclus_phi_min, hclus_phi_max);

    h2Pi0MassClusChi1 = new TH2F("h2Pi0MassClusChi1", "Cluster 1 #chi^{2} vs Invariant Mass; Mass [GeV]; #chi^{2}", bins_pi0_mass, hpi0_mass_min, hpi0_mass_max, bins_clus_chi, hclus_chi_min, hclus_chi_max);
    h2Pi0MassClusChi2 = new TH2F("h2Pi0MassClusChi2", "Cluster 2 #chi^{2} vs Invariant Mass; Mass [GeV]; #chi^{2}", bins_pi0_mass, hpi0_mass_min, hpi0_mass_max, bins_clus_chi, hclus_chi_min, hclus_chi_max);
}

void myAnalysis::process_event(Long64_t events) {
    cout << "======================================" << endl;
    cout << "Begin Process Event" << endl;
    cout << "======================================" << endl;

    Float_t clus_E;
    Float_t clus_eta;
    Float_t clus_phi;
    Float_t clus_chi;
    Float_t clus_E2;
    Float_t clus_eta2;
    Float_t clus_phi2;
    Float_t clus_chi2;
    Float_t pi0_mass;
    Float_t pi0_pt;
    Float_t pi0_eta;
    Float_t pi0_phi;

    T->SetBranchAddress("clus_E",    &clus_E);
    T->SetBranchAddress("clus_eta",  &clus_eta);
    T->SetBranchAddress("clus_phi",  &clus_phi);
    T->SetBranchAddress("clus_chi",  &clus_chi);
    T->SetBranchAddress("clus_E2",   &clus_E2);
    T->SetBranchAddress("clus_eta2", &clus_eta2);
    T->SetBranchAddress("clus_phi2", &clus_phi2);
    T->SetBranchAddress("clus_chi2", &clus_chi2);
    T->SetBranchAddress("pi0_mass",  &pi0_mass);
    T->SetBranchAddress("pi0_pt",    &pi0_pt);
    T->SetBranchAddress("pi0_eta",   &pi0_eta);
    T->SetBranchAddress("pi0_phi",   &pi0_phi);

    Float_t clus_E_min   = 99999;
    Float_t clus_eta_min = 99999;
    Float_t clus_phi_min = 99999;
    Float_t clus_chi_min = 99999;
    Float_t pi0_mass_min = 99999;
    Float_t pi0_eta_min  = 99999;
    Float_t pi0_phi_min  = 99999;
    Float_t pi0_pt_min   = 99999;

    Float_t clus_E_max   = 0;
    Float_t clus_eta_max = 0;
    Float_t clus_phi_max = 0;
    Float_t clus_chi_max = 0;
    Float_t pi0_mass_max = 0;
    Float_t pi0_eta_max  = 0;
    Float_t pi0_phi_max  = 0;
    Float_t pi0_pt_max   = 0;

    Float_t deltaR_min   = 99999;
    Float_t e_asym_min   = 99999;

    Float_t deltaR_max   = 0;
    Float_t e_asym_max   = 0;

    events = min(events, T->GetEntries());

    // keep track of the ranges of values for each branch

    for (Int_t i = 0; i < events; ++i) {
        if(i%500000 == 0) cout << "Progress: " << i*100./events << "%" << endl;
        // Load the data for the given tree entry
        T->GetEntry(i);

        // if(pi0_pt < 2 || pi0_pt >= 3) continue;

        clus_E_min   = min(clus_E_min,   min(clus_E, clus_E2));
        clus_eta_min = min(clus_eta_min, min(clus_eta, clus_eta2));
        clus_phi_min = min(clus_phi_min, min(clus_phi, clus_phi2));
        clus_chi_min = min(clus_chi_min, min(clus_chi, clus_chi2));

        clus_E_max   = max(clus_E_max,   max(clus_E, clus_E2));
        clus_eta_max = max(clus_eta_max, max(clus_eta, clus_eta2));
        clus_phi_max = max(clus_phi_max, max(clus_phi, clus_phi2));
        clus_chi_max = max(clus_chi_max, max(clus_chi, clus_chi2));

        pi0_mass_min = min(pi0_mass_min, pi0_mass);
        pi0_eta_min  = min(pi0_eta_min,  pi0_eta);
        pi0_phi_min  = min(pi0_phi_min,  pi0_phi);
        pi0_pt_min   = min(pi0_pt_min,   pi0_pt);

        pi0_mass_max = max(pi0_mass_max, pi0_mass);
        pi0_eta_max  = max(pi0_eta_max,  pi0_eta);
        pi0_phi_max  = max(pi0_phi_max,  pi0_phi);
        pi0_pt_max   = max(pi0_pt_max,   pi0_pt);

        Float_t deltaR = sqrt(pow(clus_eta-clus_eta2,2)+pow(clus_phi-clus_phi2,2));
        Float_t e_asym = abs(clus_E-clus_E2)/(clus_E+clus_E2);

        deltaR_min = min(deltaR_min, deltaR);
        e_asym_min = min(e_asym_min, e_asym);

        deltaR_max = max(deltaR_max, deltaR);
        e_asym_max = max(e_asym_max, e_asym);

        h2ClusE->Fill(clus_E, clus_E2);
        h2ClusEta->Fill(clus_eta, clus_eta2);
        h2ClusPhi->Fill(clus_phi, clus_phi2);
        h2ClusChi->Fill(clus_chi, clus_chi2);
        hDeltaR->Fill(deltaR);
        hEAsym->Fill(e_asym);

        hPi0Mass->Fill(pi0_mass);
        hPi0Pt->Fill(pi0_pt);
        h2Pi0EtaPhi->Fill(pi0_eta, pi0_phi);
        h2Pi0MassPt->Fill(pi0_mass, pi0_pt);

        h2Pi0MassDeltaR->Fill(pi0_mass, deltaR);
        h2Pi0MassEAsym->Fill(pi0_mass, e_asym);

        h2Pi0MassClusE1->Fill(pi0_mass, clus_E);
        h2Pi0MassClusE2->Fill(pi0_mass, clus_E2);

        h2Pi0MassClusEta1->Fill(pi0_mass, clus_eta);
        h2Pi0MassClusEta2->Fill(pi0_mass, clus_eta2);

        h2Pi0MassClusPhi1->Fill(pi0_mass, clus_phi);
        h2Pi0MassClusPhi2->Fill(pi0_mass, clus_phi2);

        h2Pi0MassClusChi1->Fill(pi0_mass, clus_chi);
        h2Pi0MassClusChi2->Fill(pi0_mass, clus_chi2);
    }

    cout << "=====================================" << endl;
    cout << "Process Event Statistics" << endl;
    cout << "=====================================" << endl;
    cout << left << "clus_E_min:   " << setw(15) << clus_E_min   << ", clus_E_max:   " << clus_E_max   << endl;
    cout << left << "clus_eta_min: " << setw(15) << clus_eta_min << ", clus_eta_max: " << clus_eta_max << endl;
    cout << left << "clus_phi_min: " << setw(15) << clus_phi_min << ", clus_phi_max: " << clus_phi_max << endl;
    cout << left << "clus_chi_min: " << setw(15) << clus_chi_min << ", clus_chi_max: " << clus_chi_max << endl;
    cout << left << "pi0_mass_min: " << setw(15) << pi0_mass_min << ", pi0_mass_max: " << pi0_mass_max << endl;
    cout << left << "pi0_eta_min:  " << setw(15) << pi0_eta_min  << ", pi0_eta_max:  " << pi0_eta_max  << endl;
    cout << left << "pi0_phi_min:  " << setw(15) << pi0_phi_min  << ", pi0_phi_max:  " << pi0_phi_max  << endl;
    cout << left << "pi0_pt_min:   " << setw(15) << pi0_pt_min   << ", pi0_pt_max:   " << pi0_pt_max   << endl;
    cout << endl;
    cout << left << "deltaR_min:   " << setw(15) << deltaR_min   << ", deltaR_max:   " << deltaR_max   << endl;
    cout << left << "e_asym_min:   " << setw(15) << e_asym_min   << ", e_asym_max:   " << e_asym_max   << endl;
    cout << "=====================================" << endl;
    cout << "Finish Process Event" << endl;
}


void myAnalysis::finalize(const string &i_output) {
    TFile output(i_output.c_str(),"recreate");

    output.cd();

    h2ClusE->Write();
    h2ClusEta->Write();
    h2ClusPhi->Write();
    h2ClusChi->Write();

    hDeltaR->Write();
    hEAsym->Write();

    hPi0Mass->Write();
    hPi0Pt->Write();
    h2Pi0EtaPhi->Write();
    h2Pi0MassPt->Write();
    h2Pi0MassDeltaR->Write();
    h2Pi0MassEAsym->Write();

    h2Pi0MassClusE1->Write();
    h2Pi0MassClusE2->Write();
    h2Pi0MassClusEta1->Write();
    h2Pi0MassClusEta2->Write();
    h2Pi0MassClusPhi1->Write();
    h2Pi0MassClusPhi2->Write();
    h2Pi0MassClusChi1->Write();
    h2Pi0MassClusChi2->Write();

    output.Close();
}

void pi0Analysis(const string  &i_input,
                  const string &i_cuts,
                  Long64_t      events   = 0,
                  const string &i_output = "test.root") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "  << i_input << endl;
    cout << "Cuts: "       << i_cuts << endl;
    cout << "events: "     << events << endl;
    cout << "outputFile: " << i_output << endl;
    cout << "#############################" << endl;

    Int_t ret = myAnalysis::init(i_input, i_cuts);
    if(ret != 0) return;

    myAnalysis::process_event(events);
    myAnalysis::finalize(i_output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 3 || argc > 5){
        cout << "usage: ./pi0_analysis inputFile cuts [events] [outputFile]" << endl;
        cout << "inputFile: containing list of root file paths" << endl;
        cout << "cuts: csv file containing cuts" << endl;
        cout << "events: Number of events to analyze. Default: 0 (to run over all entries)." << endl;
        cout << "outputFile: location of output file. Default: test.root." << endl;
        return 1;
    }

    Long64_t events     = 0;
    string outputFile = "test.root";

    if(argc >= 4) {
        events = atoi(argv[3]);
    }
    if(argc >= 5) {
        outputFile = argv[4];
    }

    pi0Analysis(argv[1], argv[2], events, outputFile);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;

}
# endif
