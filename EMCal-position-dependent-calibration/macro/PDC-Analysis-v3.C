//
// Code to process validation sample 0.5-25 GeV
// input: /sphenix/u/anarde/scratch/data/validation-0.3-25-GeV.root
// output: /sphenix/user/anarde/output/validation-plots-0.3-25-GeV.root
//
// -- c++ includes --
#include <iostream>
#include <strings.h>
#include <vector>

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

namespace myAnalysis {
    TFile* input;
    TNtuple* ntp;

    void init(const string &i_input);
    void init_hists();
    void process_event(UInt_t events = 0);
    void process_fits();
    void finalize(const string &i_output);

    TH1F* hClusECore;
    TH1F* hClusECoreCalib;
    TH1F* hClusEta;
    TH1F* hPhotonGE;
    TH1F* hPhotonPt;
    TH1F* hPhotonEta;
    TH1F* hResponse;
    TH1F* hResponseCalib;

    TH2F* hClusECoreCalibVsECore;
    TH2F* hClusECoreVsPhotonGE;
    TH2F* hClusECoreCalibVsPhotonGE;
    TH2F* hResponseVsPhotonGE; // ECore/GE vs GE
    TH2F* hResponseCalibVsPhotonGE; // ECore Calib/GE vs GE
    TH2F* hResponseCalibVsResponse;
    TH2F* hClusEtaVsPhotonEta;
    TH2F* hClusEtaVsECoreCalib;

    vector<vector<TH1F*>> hResponses;
    vector<vector<TH1F*>> hResponsesCalib;

    UInt_t  bins_e = 1000;
    Float_t low_e  = 0;
    Float_t high_e = 100;

    UInt_t  bins_ge = 300;
    Float_t low_ge  = 0;
    Float_t high_ge = 30;

    UInt_t  bins_response = 100;
    Float_t low_response  = 0;
    Float_t high_response = 5;

    UInt_t  bins_gpt = 22;
    Float_t low_gpt  = 3;
    Float_t high_gpt = 25;

    UInt_t  bins_geta = 10;
    Float_t low_geta  = -1;
    Float_t high_geta  = 1;

    UInt_t  bins_ecore = 165;
    Float_t low_ecore  = 0.5;
    Float_t high_ecore = 83;

    UInt_t  bins_eta = 22;
    Float_t low_eta  = -1.1;
    Float_t high_eta = 1.1;
}

void myAnalysis::init_hists() {
    cout << "----------------------" << endl;
    cout << "starting init hists" << endl;

    hClusECore      = new TH1F("hClusECore", "Cluster E_{Core}; Energy [GeV]; Counts", bins_e, low_e, high_e);
    hClusECoreCalib = new TH1F("hClusECoreCalib", "Cluster E_{Core} Calib; Energy [GeV]; Counts", bins_e, low_e, high_e);
    hClusEta        = new TH1F("hClusEta", "Cluster #eta; #eta; Counts", bins_eta, low_eta, high_eta);
    hPhotonGE       = new TH1F("hPhotonGE", "Photon Energy; Energy [GeV]; Counts", bins_ge, low_ge, high_ge);
    hPhotonPt       = new TH1F("hPhotonPt", "Photon p_{T}; p_{T} [GeV]; Counts", bins_ge, low_ge, high_ge);
    hPhotonEta      = new TH1F("hPhotonEta", "Photon #eta; #eta; Counts", bins_eta, low_eta, high_eta);
    hResponse       = new TH1F("hResponse", "Response; E_{Core}/E_{Truth}; Counts", bins_response, low_response, high_response);
    hResponseCalib  = new TH1F("hResponseCalib", "Response Calib; E_{Core}/E_{Truth}; Counts", bins_response, low_response, high_response);

    hClusECoreCalibVsECore    = new TH2F("hClusECoreCalibVsECore", "Cluster E_{Core} Calib vs E_{Core}; E_{Core} [GeV]; E_{Core} Calib [GeV]", bins_e, low_e, high_e, bins_e, low_e, high_e);
    hClusECoreVsPhotonGE      = new TH2F("hClusECoreVsPhotonGE", "Cluster E_{Core} vs Photon Energy; Photon Energy [GeV]; Cluster E_{Core} [GeV]", bins_ge, low_ge, high_ge, bins_e, low_e, high_e);
    hClusECoreCalibVsPhotonGE = new TH2F("hClusECoreCalibVsPhotonGE", "Cluster E_{Core} Calib vs Photon Energy; Photon Energy [GeV]; Cluster E_{Core} [GeV]", bins_ge, low_ge, high_ge, bins_e, low_e, high_e);
    hResponseVsPhotonGE       = new TH2F("hResponseVsPhotonGE", "Response vs Photon Energy; Photon Energy [GeV]; E_{Core}/E_{Truth}", bins_ge, low_ge, high_ge, bins_response, low_response, high_response);
    hResponseCalibVsPhotonGE  = new TH2F("hResponseCalibVsPhotonGE", "Response Calib vs Photon Energy; Photon Energy [GeV]; E_{Core}/E_{Truth}", bins_ge, low_ge, high_ge, bins_response, low_response, high_response);
    hResponseCalibVsResponse  = new TH2F("hResponseCalibVsResponse", "Response Calib vs Response; E_{Core}/E_{Truth} ; E_{Core}/E_{Truth} Calib", bins_response, low_response, high_response, bins_response, low_response, high_response);
    hClusEtaVsPhotonEta       = new TH2F("hClusEtaVsPhotonEta", "Cluster #eta vs Photon #eta_{Truth}; #eta_{Truth}; #eta", bins_eta, low_eta, high_eta, bins_eta, low_eta, high_eta);
    hClusEtaVsECoreCalib      = new TH2F("hClusEtaVsECoreCalib", "Cluster #eta vs Cluster E_{Core} Calib; E_{Core} Calib [GeV]; #eta", bins_ecore, low_ecore, high_ecore, bins_eta, low_eta, high_eta);

    Float_t eta_bin   = (high_eta-low_eta)/bins_eta;
    Float_t ecore_bin = (high_ecore-low_ecore)/bins_ecore;
    cout << "eta_bin: " << eta_bin << ", ecore_bin: " << ecore_bin << endl;

    stringstream title;
    for(UInt_t i = 0; i < bins_ecore; ++i) {
        vector<TH1F*> hDummy1;
        vector<TH1F*> hDummy2;
        for(UInt_t j = 0; j < bins_eta; ++j) {
            title.str("");
            title << "Response: " << low_ecore+i*ecore_bin << " #leq E_{Core} Calib < " << low_ecore+(i+1)*ecore_bin
                  << ", " << low_eta + j*eta_bin << " #leq #eta < " << low_eta + (j+1)*eta_bin
                  << "; E_{Core}/E_{Truth}; Counts";

            string name = "hResponse_"+to_string(i)+"_"+to_string(j);
            auto h = new TH1F(name.c_str(), title.str().c_str(), bins_response, low_response, high_response);
            h->Sumw2();
            hDummy1.push_back(h);

            title.str("");
            title << "Response Calib: " << low_ecore+i*ecore_bin << " #leq E_{Core} Calib < " << low_ecore+(i+1)*ecore_bin
                  << ", " << low_eta + j*eta_bin << " #leq #eta < " << low_eta + (j+1)*eta_bin
                  << "; E_{Core}/E_{Truth}; Counts";

            name = "hResponseCalib_"+to_string(i)+"_"+to_string(j);
            h = new TH1F(name.c_str(), title.str().c_str(), bins_response, low_response, high_response);
            h->Sumw2();
            hDummy2.push_back(h);
        }
        hResponses.push_back(hDummy1);
        hResponsesCalib.push_back(hDummy2);
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
    ntp->SetBranchStatus("ecore_calib", true);
    ntp->SetBranchStatus("ge", true);
    ntp->SetBranchStatus("gpt", true);
    ntp->SetBranchStatus("eta_detector", true);
    ntp->SetBranchStatus("geta", true);

    init_hists();
    cout << "finished init" << endl;
}

void myAnalysis::process_event(UInt_t events) {
    cout << "----------------------" << endl;
    cout << "starting process event" << endl;
    events = (events) ? events : ntp->GetEntries();
    cout << "events: " << events << endl;

    Float_t ecore; // cluster ecore
    Float_t ecore_calib; // PDC calibrated cluster ecore
    Float_t ge; // truth cluster energy
    Float_t gpt; // truth cluster pt
    Float_t eta; // cluster eta in reference to a vertex of (0,0,0)
    Float_t geta; // truth cluster eta

    ntp->SetBranchAddress("ecore",        &ecore);
    ntp->SetBranchAddress("ecore_calib",  &ecore_calib);
    ntp->SetBranchAddress("ge",           &ge);
    ntp->SetBranchAddress("gpt",          &gpt);
    ntp->SetBranchAddress("eta_detector", &eta);
    ntp->SetBranchAddress("geta",         &geta);

    Float_t ecore_min          = 9999;
    Float_t ecore_calib_min    = 9999;
    Float_t ge_min             = 9999;
    Float_t gpt_min            = 9999;
    Float_t eta_min            = 9999;
    Float_t geta_min           = 9999;
    Float_t response_min       = 9999;
    Float_t response_calib_min = 9999;

    Float_t ecore_max          = 0;
    Float_t ecore_calib_max    = 0;
    Float_t ge_max             = 0;
    Float_t gpt_max            = 0;
    Float_t eta_max            = 0;
    Float_t geta_max           = 0;
    Float_t response_max       = 0;
    Float_t response_calib_max = 0;

    auto hDummyECore  = new TH1F("hDummyECore","",bins_ecore, low_ecore, high_ecore);
    auto hDummyEta = new TH1F("hDummyEta","",bins_eta, low_eta, high_eta);

    UInt_t ctr = 0;

    for (UInt_t i = 0; i < events; ++i) {

        ntp->GetEntry(i);

        // track progress of the program
        if(i%100000 == 0) cout << "Progress: " << i*100./events << " %" << endl;

        Float_t response       = ecore/ge;
        Float_t response_calib = ecore_calib/ge;

        ecore_min          = min(ecore_min, ecore);
        ecore_calib_min    = min(ecore_calib_min, ecore_calib);
        eta_min            = min(eta_min, eta);
        ge_min             = min(ge_min, ge);
        gpt_min            = min(gpt_min, gpt);
        geta_min           = min(geta_min, geta);
        response_min       = min(response_min, response);
        response_calib_min = min(response_calib_min, response_calib);

        ecore_max          = max(ecore_max, ecore);
        ecore_calib_max    = max(ecore_calib_max, ecore_calib);
        eta_max            = max(eta_max, eta);
        ge_max             = max(ge_max, ge);
        gpt_max            = max(gpt_max, gpt);
        geta_max           = max(geta_max, geta);
        response_max       = max(response_max, response);
        response_calib_max = max(response_calib_max, response_calib);

        hPhotonPt      ->Fill(gpt);

        hClusECore     ->Fill(ecore);
        hClusECoreCalib->Fill(ecore_calib);
        hClusEta       ->Fill(eta);
        hPhotonGE      ->Fill(ge);
        hPhotonEta     ->Fill(geta);
        hResponse      ->Fill(response);
        hResponseCalib ->Fill(response_calib);

        hResponseVsPhotonGE      ->Fill(ge, response);
        hResponseCalibVsPhotonGE ->Fill(ge, response_calib);
        hResponseCalibVsResponse ->Fill(response, response_calib);
        hClusECoreCalibVsECore   ->Fill(ecore, ecore_calib);
        hClusECoreVsPhotonGE     ->Fill(ge, ecore);
        hClusECoreCalibVsPhotonGE->Fill(ge, ecore_calib);
        hClusEtaVsPhotonEta      ->Fill(geta, eta);
        hClusEtaVsECoreCalib     ->Fill(ecore_calib, eta);

        Int_t binx = hDummyECore->FindBin(ecore_calib)-1;
        Int_t biny = hDummyEta->FindBin(eta)-1;
        if(binx >= 0 && binx < bins_ecore && biny >= 0 && biny < bins_eta) {
            // hResponses[binx][biny]->Fill(response);
            hResponsesCalib[binx][biny]->Fill(response_calib);
            ++ctr;
        }
        // else if(binx < 0 && binx >= bins_ecore) cout << "eta: " << eta << ", ecore_calib: " << ecore_calib << endl;
    }

    cout << "ecore min: " << ecore_min << ", ecore max: " << ecore_max << endl;
    cout << "ecore calib min: " << ecore_calib_min << ", ecore calib max: " << ecore_calib_max << endl;
    cout << "ge min: " << ge_min << ", ge max: " << ge_max << endl;
    cout << "gpt min: " << gpt_min << ", gpt max: " << gpt_max << endl;
    cout << "eta min: " << eta_min << ", eta max: " << eta_max << endl;
    cout << "geta min: " << geta_min << ", geta max: " << geta_max << endl;
    cout << "response min: " << response_min << ", response max: " << response_max << endl;
    cout << "response calib min: " << response_calib_min << ", response calib max: " << response_calib_max << endl;
    cout << "Accepted clusters: " << ctr << ", " << ctr*100./events << " %"<< endl;
    cout << "finished process event" << endl;
}

void myAnalysis::finalize(const string &i_output) {
    cout << "----------------------" << endl;
    cout << "starting finalize" << endl;
    TFile output(i_output.c_str(),"recreate");
    output.cd();

    hClusECore     ->Write();
    hClusECoreCalib->Write();
    hClusEta       ->Write();
    hPhotonGE      ->Write();
    hPhotonPt      ->Write();
    hPhotonEta     ->Write();

    hClusEtaVsPhotonEta      ->Write();
    hClusECoreCalibVsECore   ->Write();
    hClusECoreVsPhotonGE     ->Write();
    hClusECoreCalibVsPhotonGE->Write();
    hClusEtaVsECoreCalib     ->Write();

    hResponse      ->Write();
    hResponseCalib ->Write();

    hResponseVsPhotonGE     ->Write();
    hResponseCalibVsPhotonGE->Write();
    hResponseCalibVsResponse->Write();

    output.mkdir("response");
    output.mkdir("response_calib");

    for(UInt_t i = 0; i < bins_ecore; ++i) {
        for(UInt_t j = 0; j < bins_eta; ++j) {
            output.cd("response");
            if(hResponses[i][j]->Integral(1,bins_response)) hResponses[i][j]->Write();

            output.cd("response_calib");
            if(hResponsesCalib[i][j]->Integral(1,bins_response)) hResponsesCalib[i][j]->Write();
        }
    }

    // Close root file
    input->Close();
    output.Close();
    cout << "finished finalize" << endl;
}

void PDC_Analysis_v3(const string &i_input,
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

    PDC_Analysis_v3(argv[1], outputFile, ntp_cluster_entries);

    cout << "done" << endl;
    return 0;
}
# endif
