// c++ includes --
#include <iostream>
#include <strings.h>
#include <vector>
#include <sstream>

// -- root includes --
#include <TH2D.h>
#include <TF1.h>
#include <TFile.h>
#include <TMath.h>
#include <TNtuple.h>

using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::min;
using std::max;
using std::stringstream;

namespace myAnalysis {
    TFile* input;
    TNtuple* ntp;

    void init(const string &i_input);
    void init_hists();
    void process_event(UInt_t events = 0);
    void finalize(const string &i_output);

    TH1F* hClusECore;
    TH1F* hPhotonGE;
    TH1F* hPhotonPt;
    TH1F* hResponse;

    TH2F* hClusECoreVsPhotonGE;
    TH2F* hResponseVsPhotonGE; // ECore/GE vs GE
    TH2F* h2Clusters;
    TH2F* h2ClustersEtaPhi;

    vector<vector<TH1F*>> hResponses;

    UInt_t  bins_e = 500;
    Float_t low_e  = 0;
    Float_t high_e = 5;

    UInt_t  bins_ge = 300;
    Float_t low_ge  = 0;
    Float_t high_ge = 3;

    UInt_t  bins_clus_eta = 96;
    Float_t low_clus_eta  = -1.152;
    Float_t high_clus_eta = 1.152;

    UInt_t  bins_clus_phi = 64;
    Float_t low_clus_phi  = -0.1;
    Float_t high_clus_phi = 0.1;

    UInt_t  bins_response = 200;
    Float_t low_response  = 0;
    Float_t high_response = 2;

    UInt_t  bins_gpt = 22;
    Float_t low_gpt  = 3;
    Float_t high_gpt = 25;

    UInt_t  bins_eta = 768;
    Float_t low_eta  = -0.5;
    Float_t high_eta = 95.5;

    UInt_t  bins_phi = 64;
    Float_t low_phi  = -0.5;
    Float_t high_phi = 7.5;
}

void myAnalysis::init_hists() {
    cout << "----------------------" << endl;
    cout << "starting init hists" << endl;

    hClusECore      = new TH1F("hClusECore", "Cluster E_{Core}; Energy [GeV]; Counts", bins_e, low_e, high_e);
    hPhotonGE       = new TH1F("hPhotonGE", "Photon Energy; Energy [GeV]; Counts", bins_ge, low_ge, high_ge);
    hPhotonPt       = new TH1F("hPhotonPt", "Photon p_{T}; p_{T} [GeV]; Counts", bins_ge, low_ge, high_ge);
    hResponse       = new TH1F("hResponse", "Response; E_{Core}/E_{Truth}; Counts", bins_response, low_response, high_response);

    hClusECoreVsPhotonGE      = new TH2F("hClusECoreVsPhotonGE", "Cluster E_{Core} vs Photon Energy; Photon Energy [GeV]; Cluster E_{Core} [GeV]", bins_ge, low_ge, high_ge, bins_e, low_e, high_e);
    hResponseVsPhotonGE       = new TH2F("hResponseVsPhotonGE", "Response vs Photon Energy; Photon Energy [GeV]; E_{Core}/E_{Truth}", bins_ge, low_ge, high_ge, bins_response, low_response, high_response);
    h2Clusters = new TH2F("h2Clusters", "Clusters; towerid #eta; towerid #phi", bins_eta, low_eta, high_eta, bins_phi, low_phi, high_phi);
    h2ClustersEtaPhi = new TH2F("h2ClustersEtaPhi", "Clusters; #eta; #phi", bins_clus_eta, low_clus_eta, high_clus_eta, bins_clus_phi, low_clus_phi, high_clus_phi);

    stringstream title;
    Float_t eta_bin = (high_eta-low_eta)/bins_eta;
    Float_t phi_bin = (high_phi-low_phi)/bins_phi;

    for(UInt_t i = 0; i < bins_eta; ++i) {
        vector<TH1F*> hDummy;
        for(UInt_t j = 0; j < bins_phi; ++j) {
            title.str("");
            title << "Response: " << low_eta+eta_bin*i << " #leq towerid #eta < " << low_eta+eta_bin*(i+1)
                  << ", " << low_phi+phi_bin*j << " #leq towerid #phi < " << low_phi+phi_bin*(j+1)
                  << "; Response [E_{Core}/E_{Truth}]; Counts";

            string name = "hResponse_"+to_string(i)+"_"+to_string(j);
            auto h = new TH1F(name.c_str(), title.str().c_str(), bins_response, low_response, high_response);
            h->Sumw2();
            hDummy.push_back(h);
        }
        hResponses.push_back(hDummy);
    }

    cout << "finished init hists" << endl;
}

void myAnalysis::init(const string &i_input) {
    cout << "----------------------" << endl;
    cout << "starting init" << endl;
    input = TFile::Open(i_input.c_str());
    ntp   = (TNtuple*)input->Get("ntp_cluster");

    // Disable everything...
    ntp->SetBranchStatus("*", false);
    // ...but the branch we need
    ntp->SetBranchStatus("ecore", true);
    ntp->SetBranchStatus("ge", true);
    ntp->SetBranchStatus("gpt", true);
    ntp->SetBranchStatus("eta_detector", true);
    ntp->SetBranchStatus("phi", true);
    ntp->SetBranchStatus("avgeta", true);
    ntp->SetBranchStatus("avgphi", true);

    init_hists();
    cout << "finished init" << endl;
}

void myAnalysis::process_event(UInt_t events) {
    cout << "----------------------" << endl;
    cout << "starting process event" << endl;
    events = (events) ? events : ntp->GetEntries();
    cout << "events: " << events << endl;

    Float_t ecore; // cluster ecore
    Float_t ge; // truth cluster energy
    Float_t gpt; // truth cluster pt
    Float_t eta; // cluster eta in reference to a vertex of (0,0,0)
    Float_t phi; // cluster phi
    Float_t avgeta;
    Float_t avgphi;

    ntp->SetBranchAddress("ecore",        &ecore);
    ntp->SetBranchAddress("ge",           &ge);
    ntp->SetBranchAddress("gpt",          &gpt);
    ntp->SetBranchAddress("eta_detector", &eta);
    ntp->SetBranchAddress("phi",          &phi);
    ntp->SetBranchAddress("avgeta",       &avgeta);
    ntp->SetBranchAddress("avgphi",       &avgphi);

    Float_t ecore_min    = 9999;
    Float_t ge_min       = 9999;
    Float_t gpt_min      = 9999;
    Float_t eta_min      = 9999;
    Float_t phi_min      = 9999;
    Float_t avgeta_min   = 9999;
    Float_t avgphi_min   = 9999;
    Float_t response_min = 9999;

    Float_t ecore_max    = 0;
    Float_t ge_max       = 0;
    Float_t gpt_max      = 0;
    Float_t eta_max      = 0;
    Float_t phi_max      = 0;
    Float_t avgeta_max   = 0;
    Float_t avgphi_max   = 0;
    Float_t response_max = 0;

    UInt_t ctr = 0;

    for (UInt_t i = 0; i < events; ++i) {

        ntp->GetEntry(i);

        // track progress of the program
        if(i%100000 == 0) cout << "Progress: " << i*100./events << " %" << endl;

        if(ecore < 0.5) continue;

        Float_t response       = ecore/ge;

        ecore_min    = min(ecore_min, ecore);
        eta_min      = min(eta_min, eta);
        phi_min      = min(phi_min, phi);
        avgeta_min   = min(avgeta_min, avgeta);
        avgphi_min   = min(avgphi_min, avgphi);
        ge_min       = min(ge_min, ge);
        gpt_min      = min(gpt_min, gpt);
        response_min = min(response_min, response);

        ecore_max    = max(ecore_max, ecore);
        eta_max      = max(eta_max, eta);
        phi_max      = max(phi_max, phi);
        avgeta_max   = max(avgeta_max, avgeta);
        avgphi_max   = max(avgphi_max, avgphi);
        ge_max       = max(ge_max, ge);
        gpt_max      = max(gpt_max, gpt);
        response_max = max(response_max, response);

        hPhotonPt->Fill(gpt);
        hClusECore->Fill(ecore);
        hPhotonGE->Fill(ge);
        hResponse->Fill(response);

        h2Clusters->Fill(avgeta, avgphi);
        h2ClustersEtaPhi->Fill(eta, phi);
        hResponseVsPhotonGE->Fill(ge, response);
        hClusECoreVsPhotonGE->Fill(ge, ecore);

        Int_t binx = h2Clusters->GetXaxis()->FindBin(avgeta)-1;
        Int_t biny = h2Clusters->GetYaxis()->FindBin(avgphi)-1;
        if(binx >= 0 && binx < bins_eta && biny >= 0 && biny < bins_phi) {
            hResponses[binx][biny]->Fill(response);
            ++ctr;
        }
    }

    cout << "ecore min: " << ecore_min << ", ecore max: " << ecore_max << endl;
    cout << "ge min: " << ge_min << ", ge max: " << ge_max << endl;
    cout << "gpt min: " << gpt_min << ", gpt max: " << gpt_max << endl;
    cout << "eta min: " << eta_min << ", eta max: " << eta_max << endl;
    cout << "phi min: " << phi_min << ", phi max: " << phi_max << endl;
    cout << "avgeta min: " << avgeta_min << ", avgeta max: " << avgeta_max << endl;
    cout << "avgphi min: " << avgphi_min << ", avgphi max: " << avgphi_max << endl;
    cout << "response min: " << response_min << ", response max: " << response_max << endl;
    cout << "Accepted clusters: " << ctr << ", " << ctr*100./events << " %"<< endl;
    cout << "finished process event" << endl;
}

void myAnalysis::finalize(const string &i_output) {
    cout << "----------------------" << endl;
    cout << "starting finalize" << endl;
    TFile output(i_output.c_str(),"recreate");
    output.cd();

    hClusECore->Write();
    hPhotonGE->Write();
    hPhotonPt->Write();

    h2Clusters->Write();
    h2ClustersEtaPhi->Write();

    hClusECoreVsPhotonGE->Write();

    hResponse->Write();
    hResponseVsPhotonGE->Write();

    output.mkdir("response");

    for(UInt_t i = 0; i < bins_eta; ++i) {
        for(UInt_t j = 0; j < bins_phi; ++j) {
            output.cd("response");
            if(hResponses[i][j]->GetEntries()) hResponses[i][j]->Write();
        }
    }

    // Close root file
    input->Close();
    output.Close();
    cout << "finished finalize" << endl;
}

void PDC_train(const string &i_input,
               const string &i_output = "test.root",
               UInt_t        events   = 0) {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: " << i_input << endl;
    cout << "outputFile: " << i_output << endl;
    cout << "#############################" << endl;

    myAnalysis::init(i_input);

    myAnalysis::process_event(events);

    myAnalysis::finalize(i_output);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 2 || argc > 4){
        cout << "usage: ./bin/PDC-Analysis inputFile [outputFile] [ntp_cluster_entries]" << endl;
        cout << "inputFile: train sample root file. Required." << endl;
        cout << "outputFile: location of output file. Default: test.root." << endl;
        cout << "ntp_cluster_entries: Number of events to analyze. Default: 0 (to run over all entries)." << endl;
        return 1;
    }

    string outputFile          = "test.root";
    UInt_t ntp_cluster_entries = 0;

    if(argc >= 3) {
        outputFile = argv[2];
    }
    if(argc >= 4) {
        ntp_cluster_entries = atoi(argv[3]);
    }

    PDC_train(argv[1], outputFile, ntp_cluster_entries);

    cout << "done" << endl;
    return 0;
}
# endif
