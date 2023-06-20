// c++ includes --
#include <string>
#include <vector>
#include <iostream>

// -- root includes --
#include <TH2F.h>
#include <TFile.h>
#include <TTree.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::min;
using std::max;
using std::to_string;

namespace myAnalysis {

    void init(const string& inputFile);
    void analyze(UInt_t nevents=0);
    void finalize(const string &outputFile="test.root");

    TFile* input;
    TTree* data;

    // QA
    TH1F* hclusterEta;
    TH1F* hclusterPhi;
    TH1F* hclusterE;
    TH1F* hclusterE_2;
    TH1F* hclusterPt;
    TH1F* hclusterChisq;
    TH1F* hclusterChisq_2;
    TH1F* hclusterChisq_3;

    // 2D correlations
    TH2F* h2clusterPhiVsEta;
    TH2F* h2clusterPhiVsEta_2;
    TH2F* h2clusterPhiVsEta_3;
    TH2F* h2clusterChisqVsE;
    TH2F* h2clusterChisqVsE_2;
    TH2F* h2clusterE_CalibVsE;

    UInt_t  eta_bins = 48;
    Float_t eta_min  = -1.152;
    Float_t eta_max  = 1.152;

    UInt_t  phi_bins = 126;
    Float_t phi_min  = -3.15;
    Float_t phi_max  = 3.15;

    UInt_t  e_bins  = 500;
    Float_t e_min   = 0;
    Float_t e_max   = 5e4;

    UInt_t  pt_bins  = 500;
    Float_t pt_min   = 0;
    Float_t pt_max   = 5e4;

    UInt_t  chi2_bins  = 500;
    Float_t chi2_min   = 0;
    Float_t chi2_max   = 5e5;

    vector<Float_t>* clusterPhi     = 0;
    vector<Float_t>* clusterEta     = 0;
    vector<Float_t>* clusterE       = 0;
    vector<Float_t>* clusterPt      = 0;
    vector<Float_t>* clusterChisq   = 0;
}

void myAnalysis::init(const string& inputFile) {
    cout << "Start Init" << endl;
    input = TFile::Open(inputFile.c_str());
    data  = (TTree*)input->Get("T");

    data->SetBranchAddress("clusterPhi",&clusterPhi);
    data->SetBranchAddress("clusterEta",&clusterEta);
    data->SetBranchAddress("clusterE",&clusterE);
    data->SetBranchAddress("clustrPt",&clusterPt);
    data->SetBranchAddress("clusterChisq",&clusterChisq);

    // QA
    hclusterEta       = new TH1F("hclusterEta", "Cluster #eta; #eta; Counts", eta_bins, eta_min, eta_max);
    hclusterPhi       = new TH1F("hclusterPhi", "Cluster #phi; #phi; Counts", phi_bins, phi_min, phi_max);
    hclusterE         = new TH1F("hclusterE", "Cluster Energy; ADC; Counts", e_bins, e_min, e_max);
    hclusterPt        = new TH1F("hclusterPt", "Cluster p_{T}; ADC; Counts", pt_bins, pt_min, pt_max);
    hclusterChisq     = new TH1F("hclusterChisq", "Cluster #chi^2; #chi^2; Counts", chi2_bins, chi2_min, chi2_max);
    h2clusterPhiVsEta = new TH2F("h2clusterPhiVsEta", "Cluster; #eta; #phi", eta_bins, eta_min, eta_max, phi_bins, phi_min, phi_max);
    h2clusterChisqVsE = new TH2F("h2clusterChisqVsE", "Cluster #chi^2 vs E; Energy; #chi^2", e_bins, e_min, e_max, chi2_bins, chi2_min, chi2_max);
    h2clusterChisqVsE_2 = new TH2F("h2clusterChisqVsE_2", "Cluster #chi^2 vs E; Energy; #chi^2", e_bins, e_min, e_max, 100, 0, 100);

    // QA with cuts
    hclusterE_2         = new TH1F("hclusterE_2", "Cluster Energy (#chi^{2} < 10); ADC; Counts", e_bins, e_min, e_max);
    hclusterChisq_2     = new TH1F("hclusterChisq_2", "Cluster #chi^{2}; #chi^{2}; Counts", 100, 0, 100);
    hclusterChisq_3     = new TH1F("hclusterChisq_3", "Cluster #chi^{2} (500 #leq ADC < 16000); #chi^{2}; Counts", 100, 0, 100);
    h2clusterPhiVsEta_2 = new TH2F("h2clusterPhiVsEta_2", "Cluster (500 #leq ADC < 16000 and #chi^{2} < 10); #eta; #phi", eta_bins, eta_min, eta_max, phi_bins, phi_min, phi_max);
    h2clusterPhiVsEta_3 = new TH2F("h2clusterPhiVsEta_3", "Cluster (8500 #leq ADC < 9500 and #chi^{2} < 10); #eta; #phi", 96, eta_min, eta_max, 256, phi_min, phi_max);

    cout << "Finish Init" << endl;
}

void myAnalysis::analyze(UInt_t nevents) {
    cout << "Start Analyze" << endl;

    // if nevents is 0 then use all events otherwise use the set number of events
    nevents = (nevents) ? nevents : data->GetEntries();

    Float_t clusterEta_min     = 0;
    Float_t clusterEta_max     = 0;
    Float_t clusterPhi_min     = 0;
    Float_t clusterPhi_max     = 0;
    Float_t clusterE_min       = 0;
    Float_t clusterE_max       = 0;
    Float_t clusterPt_min      = 0;
    Float_t clusterPt_max      = 0;
    Float_t clusterChisq_min   = 0;
    Float_t clusterChisq_max   = 0;
    cout << "Starting Event Loop" << endl;
    for(UInt_t i = 0; i < nevents; ++i) {

        if(i%500 == 0) cout << "Progress: " << i*100./nevents << " %" << endl;

        data->GetEntry(i);

        UInt_t nclusters = clusterPhi->size();

        for (UInt_t j = 0; j < nclusters; ++j) {
            Float_t clusterEta_val     = clusterEta->at(j);
            Float_t clusterPhi_val     = clusterPhi->at(j);
            Float_t clusterE_val       = clusterE->at(j);
            Float_t clusterPt_val      = clusterPt->at(j);
            Float_t clusterChisq_val   = clusterChisq->at(j);

            clusterEta_min     = min(clusterEta_min, clusterEta_val);
            clusterEta_max     = max(clusterEta_max, clusterEta_val);
            clusterPhi_min     = min(clusterPhi_min, clusterPhi_val);
            clusterPhi_max     = max(clusterPhi_max, clusterPhi_val);
            clusterE_min       = min(clusterE_min, clusterE_val);
            clusterE_max       = max(clusterE_max, clusterE_val);
            clusterPt_min      = min(clusterPt_min, clusterPt_val);
            clusterPt_max      = max(clusterPt_max, clusterPt_val);
            clusterChisq_min   = min(clusterChisq_min, clusterChisq_val);
            clusterChisq_max   = max(clusterChisq_max, clusterChisq_val);

            hclusterEta->Fill(clusterEta_val);
            hclusterPhi->Fill(clusterPhi_val);

            h2clusterPhiVsEta->Fill(clusterEta_val, clusterPhi_val);

            hclusterE->Fill(clusterE_val);
            hclusterPt->Fill(clusterPt_val);
            hclusterChisq->Fill(clusterChisq_val);
            hclusterChisq_2->Fill(clusterChisq_val);

            h2clusterChisqVsE->Fill(clusterE_val, clusterChisq_val);
            h2clusterChisqVsE_2->Fill(clusterE_val, clusterChisq_val);

            if(clusterChisq_val < 10) {
                hclusterE_2->Fill(clusterE_val);
            }

            if(clusterE_val >= 500 && clusterE_val < 16000) {
                hclusterChisq_3->Fill(clusterChisq_val);
            }

            if(clusterChisq_val < 10 && clusterE_val >= 500 && clusterE_val < 16000) {
                h2clusterPhiVsEta_2->Fill(clusterEta_val, clusterPhi_val);
            }

            if(clusterChisq_val < 10 && clusterE_val >= 8.5e3 && clusterE_val < 9.5e3) {
                h2clusterPhiVsEta_3->Fill(clusterEta_val, clusterPhi_val);
            }
        }
    }

    cout << "events processed: " << nevents << endl;
    cout << "clusterEta_min: " << clusterEta_min << " clusterEta_max: " << clusterEta_max << endl;
    cout << "clusterPhi_min: " << clusterPhi_min << " clusterPhi_max: " << clusterPhi_max << endl;
    cout << "clusterE_min: " << clusterE_min << " clusterE_max: " << clusterE_max << endl;
    cout << "clusterPt_min: " << clusterPt_min << " clusterPt_max: " << clusterPt_max << endl;
    cout << "clusterChisq_min: " << clusterChisq_min << " clusterChisq_max: " << clusterChisq_max << endl;
    cout << "Finish Analyze" << endl;
}

void myAnalysis::finalize(const string& outputFile) {
    TFile output(outputFile.c_str(),"recreate");
    output.cd();

    hclusterEta->Write();
    hclusterPhi->Write();
    h2clusterPhiVsEta->Write();
    hclusterE->Write();
    hclusterPt->Write();
    hclusterChisq->Write();
    h2clusterChisqVsE->Write();
    h2clusterChisqVsE_2->Write();

    h2clusterPhiVsEta_2->Write();
    h2clusterPhiVsEta_3->Write();
    hclusterE_2->Write();
    hclusterChisq_2->Write();
    hclusterChisq_3->Write();

    input->Close();
    output.Close();
}

void QA(const string &inputFile,
        const string &outputFile="test.root",
        UInt_t nevents=0) {

    myAnalysis::init(inputFile);
    myAnalysis::analyze(nevents);
    myAnalysis::finalize(outputFile);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 2 || argc > 4){
        cout << "usage: ./bin/QA inputFile outputFile events" << endl;
        return 1;
    }

    string input;
    string output = "test.root";
    UInt_t events = 0;

    if(argc >= 2) {
        input = argv[1];
    }
    if(argc >= 3) {
        output = argv[2];
    }
    if(argc >= 4) {
        events = atoi(argv[3]);
    }

    QA(input, output, events);

    cout << "done" << endl;
    return 0;
}
# endif
