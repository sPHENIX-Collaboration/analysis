// c++ includes --
#include <string>
#include <vector>
#include <iostream>

// -- root includes --
#include <TH2F.h>
#include <TFile.h>
#include <TChain.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::min;
using std::max;
using std::to_string;

namespace myAnalysis {

    void init(const string& input, Bool_t filesDir=false);
    void analyze(UInt_t nevents=0);
    void finalize(const string &outputFile="test.root");

    TChain* data;

    // QA
    TH1F* hNClusters;
    TH1F* hClusNtow;
    TH1F* hclusterEta;
    TH1F* hclusterPhi;
    TH1F* hclusterEFull;
    TH1F* hclusterEFull_2;
    TH1F* hclusterECore;
    TH1F* hclusterPt;
    TH1F* hclusterChisq;
    TH1F* hclusterChisq_2;
    TH1F* hclusterChisq_3;

    // 2D correlations
    TH2F* h2clusterPhiVsEta;
    TH2F* h2clusterPhiVsEta_2;
    TH2F* h2clusterPhiVsEta_3;
    TH2F* h2clusterChisqVsEFull;
    TH2F* h2clusterChisqVsEFull_2;

    UInt_t  eta_bins = 46;
    Float_t eta_min  = -1.15;
    Float_t eta_max  = 1.15;

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

    UInt_t  clusNtow_bins = 100;
    Float_t clusNtow_min  = 0;
    Float_t clusNtow_max  = 100;

    UInt_t  nClus_bins = 300;
    Float_t nClus_min  = 0;
    Float_t nClus_max  = 3e3;

    vector<Float_t>* clusterPhi    = 0;
    vector<Float_t>* clusterEta    = 0;
    vector<Float_t>* clusterEFull  = 0;
    vector<Float_t>* clusterECore  = 0;
    vector<Float_t>* clusterPt     = 0;
    vector<Float_t>* clusterChisq  = 0;
    vector<Float_t>* clusterNtow   = 0;
}

void myAnalysis::init(const string& input, Bool_t filesDir) {
    cout << "Start Init" << endl;
    data = new TChain("T");
    if(filesDir) {
        Int_t nFiles = data->Add((input+"/*.root").c_str());
    }
    else {
        data->Add((input).c_str());
    }

    data->SetBranchAddress("clusterPhi",&clusterPhi);
    data->SetBranchAddress("clusterEta",&clusterEta);
    data->SetBranchAddress("clusterEFull",&clusterEFull);
    data->SetBranchAddress("clusterECore",&clusterECore);
    data->SetBranchAddress("clustrPt",&clusterPt);
    data->SetBranchAddress("clusterChi",&clusterChisq);
    data->SetBranchAddress("clusterNtow",&clusterNtow);

    // QA
    hNClusters              = new TH1F("hNClusters", "# of clusters per event; # of clusters per event; Counts", nClus_bins, nClus_min, nClus_max);
    hClusNtow               = new TH1F("hClusNtow", "# of towers per cluster; # of towers per cluster; Counts", clusNtow_bins, clusNtow_min, clusNtow_max);
    hclusterEta             = new TH1F("hclusterEta", "Cluster #eta; #eta; Counts", eta_bins, eta_min, eta_max);
    hclusterPhi             = new TH1F("hclusterPhi", "Cluster #phi; #phi; Counts", phi_bins, phi_min, phi_max);

    hclusterEFull           = new TH1F("hclusterEFull", "Cluster Full Energy; ADC; Counts", e_bins, e_min, e_max);
    hclusterEFull_2         = new TH1F("hclusterEFull_2", "Cluster Energy (#chi^{2} < 10); ADC; Counts", e_bins, e_min, e_max);

    hclusterECore           = new TH1F("hclusterECore", "Cluster Core Energy; ADC; Counts", e_bins, e_min, e_max);
    hclusterPt              = new TH1F("hclusterPt", "Cluster p_{T}; ADC; Counts", pt_bins, pt_min, pt_max);

    hclusterChisq           = new TH1F("hclusterChisq", "Cluster #chi^{2}; #chi^{2}; Counts", chi2_bins, chi2_min, chi2_max);
    hclusterChisq_2         = new TH1F("hclusterChisq_2", "Cluster #chi^{2}; #chi^{2}; Counts", 100, 0, 100);
    hclusterChisq_3         = new TH1F("hclusterChisq_3", "Cluster #chi^{2} (500 #leq ADC < 16000); #chi^{2}; Counts", 100, 0, 100);

    h2clusterPhiVsEta       = new TH2F("h2clusterPhiVsEta", "Cluster; #eta; #phi", eta_bins, eta_min, eta_max, phi_bins, phi_min, phi_max);
    h2clusterPhiVsEta_2     = new TH2F("h2clusterPhiVsEta_2", "Cluster (500 #leq ADC < 16000 and #chi^{2} < 10); #eta; #phi", eta_bins, eta_min, eta_max, phi_bins, phi_min, phi_max);
    h2clusterPhiVsEta_3     = new TH2F("h2clusterPhiVsEta_3", "Cluster Energy; #eta; #phi", eta_bins, eta_min, eta_max, phi_bins, phi_min, phi_max);

    h2clusterChisqVsEFull   = new TH2F("h2clusterChisqVsEFull", "Cluster #chi^{2} vs E; Energy; #chi^{2}", e_bins, e_min, e_max, chi2_bins, chi2_min, chi2_max);
    h2clusterChisqVsEFull_2 = new TH2F("h2clusterChisqVsEFull_2", "Cluster #chi^{2} vs E; Energy; #chi^{2}", e_bins, e_min, e_max, 100, 0, 100);

    cout << "Finish Init" << endl;
}

void myAnalysis::analyze(UInt_t nevents) {
    cout << "Start Analyze" << endl;

    // if nevents is 0 then use all events otherwise use the set number of events
    nevents = (nevents) ? nevents : data->GetEntries();

    Float_t nClusters_max    = 0;
    Float_t clusterEta_min   = 0;
    Float_t clusterEta_max   = 0;
    Float_t clusterPhi_min   = 0;
    Float_t clusterPhi_max   = 0;
    Float_t clusterEFull_min = 0;
    Float_t clusterEFull_max = 0;
    Float_t clusterECore_min = 0;
    Float_t clusterECore_max = 0;
    Float_t clusterPt_min    = 0;
    Float_t clusterPt_max    = 0;
    Float_t clusterChisq_min = 0;
    Float_t clusterChisq_max = 0;
    Float_t clusterNtow_min  = 0;
    Float_t clusterNtow_max  = 0;
    UInt_t ctr[3] = {0};
    cout << "Starting Event Loop" << endl;
    cout << "Total Events: " << nevents << endl;
    for(UInt_t i = 0; i < nevents; ++i) {

        if(i%500 == 0) cout << "Progress: " << i*100./nevents << " %" << endl;

        data->GetEntry(i);

        Float_t nclusters = clusterPhi->size();

        hNClusters->Fill(nclusters);

        nClusters_max = max(nClusters_max, nclusters);

        for (UInt_t j = 0; j < nclusters; ++j) {
            Float_t clusterEta_val   = clusterEta->at(j);
            Float_t clusterPhi_val   = clusterPhi->at(j);
            Float_t clusterEFull_val = clusterEFull->at(j);
            Float_t clusterECore_val = clusterECore->at(j);
            Float_t clusterPt_val    = clusterPt->at(j);
            Float_t clusterChisq_val = clusterChisq->at(j);
            Float_t clusterNtow_val  = clusterNtow->at(j);

            clusterEta_min   = min(clusterEta_min, clusterEta_val);
            clusterEta_max   = max(clusterEta_max, clusterEta_val);
            clusterPhi_min   = min(clusterPhi_min, clusterPhi_val);
            clusterPhi_max   = max(clusterPhi_max, clusterPhi_val);
            clusterEFull_min = min(clusterEFull_min, clusterEFull_val);
            clusterEFull_max = max(clusterEFull_max, clusterEFull_val);
            clusterECore_min = min(clusterECore_min, clusterECore_val);
            clusterECore_max = max(clusterECore_max, clusterECore_val);
            clusterPt_min    = min(clusterPt_min, clusterPt_val);
            clusterPt_max    = max(clusterPt_max, clusterPt_val);
            clusterChisq_min = min(clusterChisq_min, clusterChisq_val);
            clusterChisq_max = max(clusterChisq_max, clusterChisq_val);
            clusterNtow_min = min(clusterNtow_min, clusterNtow_val);
            clusterNtow_max = max(clusterNtow_max, clusterNtow_val);

            hClusNtow->Fill(clusterNtow_val);
            hclusterEta->Fill(clusterEta_val);
            hclusterPhi->Fill(clusterPhi_val);

            h2clusterPhiVsEta->Fill(clusterEta_val, clusterPhi_val);
            h2clusterPhiVsEta_3->Fill(clusterEta_val, clusterPhi_val, clusterEFull_val);

            hclusterEFull->Fill(clusterEFull_val);
            hclusterECore->Fill(clusterECore_val);
            hclusterPt->Fill(clusterPt_val);
            hclusterChisq->Fill(clusterChisq_val);
            hclusterChisq_2->Fill(clusterChisq_val);

            h2clusterChisqVsEFull->Fill(clusterEFull_val, clusterChisq_val);
            h2clusterChisqVsEFull_2->Fill(clusterEFull_val, clusterChisq_val);

            if(clusterChisq_val < 10) {
                hclusterEFull_2->Fill(clusterEFull_val);
                ++ctr[1];
            }

            if(clusterEFull_val >= 500 && clusterEFull_val < 16000) {
                hclusterChisq_3->Fill(clusterChisq_val);
            }

            if(clusterChisq_val < 10 && clusterEFull_val >= 500 && clusterEFull_val < 16000) {
                h2clusterPhiVsEta_2->Fill(clusterEta_val, clusterPhi_val);
                ++ctr[2];
            }

            // keep track of number of clusters
            ++ctr[0];
        }
    }

    cout << "events processed: " << nevents << endl;
    cout << "total clusters: " << ctr[0] << endl;
    cout << "nClusters_max (by Event): " << nClusters_max << endl;
    cout << "clusters with chi2 < 10: " << ctr[1] << ", " << ctr[1]*100./ctr[0] << " %" << endl;
    cout << "clusters with chi2 < 10 and 500 <= ADC < 16000: " << ctr[2] << ", " << ctr[2]*100./ctr[0] << " %" << endl;
    cout << "clusterEta_min: " << clusterEta_min << " clusterEta_max: " << clusterEta_max << endl;
    cout << "clusterPhi_min: " << clusterPhi_min << " clusterPhi_max: " << clusterPhi_max << endl;
    cout << "clusterEFull_min: " << clusterEFull_min << " clusterEFull_max: " << clusterEFull_max << endl;
    cout << "clusterECore_min: " << clusterECore_min << " clusterECore_max: " << clusterECore_max << endl;
    cout << "clusterPt_min: " << clusterPt_min << " clusterPt_max: " << clusterPt_max << endl;
    cout << "clusterChisq_min: " << clusterChisq_min << " clusterChisq_max: " << clusterChisq_max << endl;
    cout << "clusterNtow_min: " << clusterNtow_min << " clusterNtow_max: " << clusterNtow_max << endl;
    cout << "Finish Analyze" << endl;
}

void myAnalysis::finalize(const string& outputFile) {
    TFile output(outputFile.c_str(),"recreate");
    output.cd();

    hNClusters->Write();
    hClusNtow->Write();
    hclusterEta->Write();
    hclusterPhi->Write();
    h2clusterPhiVsEta->Write();
    hclusterEFull->Write();
    hclusterECore->Write();
    hclusterPt->Write();
    hclusterChisq->Write();
    h2clusterChisqVsEFull->Write();
    h2clusterChisqVsEFull_2->Write();

    h2clusterPhiVsEta_2->Write();
    h2clusterPhiVsEta_3->Write();
    hclusterEFull_2->Write();
    hclusterChisq_2->Write();
    hclusterChisq_3->Write();

    output.Close();
}

void QA(const string &input,
        const string &outputFile="test.root",
        UInt_t nevents=0,
        Bool_t filesDir=false) {

    cout << "Input: " << input << endl;
    cout << "Output:" << outputFile << endl;
    cout << "nevents: " << nevents << endl;
    cout << "Use filesDir: " << filesDir << endl;

    myAnalysis::init(input, filesDir);
    myAnalysis::analyze(nevents);
    myAnalysis::finalize(outputFile);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 2 || argc > 5){
        cout << "usage: ./bin/QA input outputFile events filesDir" << endl;
        return 1;
    }

    string input;
    string output = "test.root";
    UInt_t events = 0;
    Bool_t filesDir = false;

    if(argc >= 2) {
        input = argv[1];
    }
    if(argc >= 3) {
        output = argv[2];
    }
    if(argc >= 4) {
        events = atoi(argv[3]);
    }
    if(argc >= 5) {
        filesDir = atoi(argv[4]);
    }

    QA(input, output, events, filesDir);

    cout << "done" << endl;
    return 0;
}
# endif
