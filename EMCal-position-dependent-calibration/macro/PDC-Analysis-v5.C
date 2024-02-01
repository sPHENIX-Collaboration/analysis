//
// Code to process validation sample 0.5-25 GeV
// input: /sphenix/u/anarde/scratch/data/validation-original-0.3-25-GeV
// output: /sphenix/user/anarde/output/validation-original-plots-0.3-25-GeV-v2.root
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
    TH1F* hClusE;
    TH1F* hClusECalib;
    TH1F* hClusEta;
    TH1F* hPhotonGE;
    TH1F* hPhotonPt;
    TH1F* hPhotonEta;
    TH1F* hResponseECore;
    TH1F* hResponseECoreCalib;
    TH1F* hResponseE;
    TH1F* hResponseECalib;

    TH2F* hClusECoreCalibVsECore;
    TH2F* hClusECoreVsPhotonGE;
    TH2F* hClusECoreCalibVsPhotonGE;
    TH2F* hResponseECoreVsPhotonGE; // ECore/GE vs GE
    TH2F* hResponseECoreCalibVsPhotonGE; // ECore Calib/GE vs GE
    TH2F* hResponseECoreCalibVsResponse;
    TH2F* hResponseEVsPhotonGE; // E/GE vs GE
    TH2F* hResponseECalibVsPhotonGE; // E Calib/GE vs GE
    TH2F* hResponseECalibVsResponse;
    TH2F* hClusEtaVsPhotonEta;
    TH2F* hClusEtaVsGE;

    vector<vector<TH1F*>> hResponsesECore;
    vector<vector<TH1F*>> hResponsesECoreCalib;
    vector<vector<TH1F*>> hResponsesE;
    vector<vector<TH1F*>> hResponsesECalib;

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

    UInt_t  bins_ge_response = 49;
    Float_t low_ge_response  = 0.5;
    Float_t high_ge_response = 25;

    UInt_t  bins_eta = 22;
    Float_t low_eta  = -1.1;
    Float_t high_eta = 1.1;
    UInt_t eventsA        = 8532690;
    UInt_t eventsB        = 10233670;
    Float_t eventsBWeight = 20.*eventsA/(4*eventsB);
}

void myAnalysis::init_hists() {
    cout << "----------------------" << endl;
    cout << "starting init hists" << endl;

    hClusECore      = new TH1F("hClusECore", "Cluster E_{Core}; Energy [GeV]; Counts", bins_e, low_e, high_e);
    hClusECoreCalib = new TH1F("hClusECoreCalib", "Cluster E_{Core} Calib; Energy [GeV]; Counts", bins_e, low_e, high_e);
    hClusE          = new TH1F("hClusE", "Cluster E; Energy [GeV]; Counts", bins_e, low_e, high_e);
    hClusECalib     = new TH1F("hClusECalib", "Cluster E Calib; Energy [GeV]; Counts", bins_e, low_e, high_e);
    hClusEta        = new TH1F("hClusEta", "Cluster #eta; #eta; Counts", bins_eta, low_eta, high_eta);
    hPhotonGE       = new TH1F("hPhotonGE", "Photon Energy; Energy [GeV]; Counts", bins_ge, low_ge, high_ge);
    hPhotonPt       = new TH1F("hPhotonPt", "Photon p_{T}; p_{T} [GeV]; Counts", bins_ge, low_ge, high_ge);
    hPhotonEta      = new TH1F("hPhotonEta", "Photon #eta; #eta; Counts", bins_eta, low_eta, high_eta);
    hResponseECore       = new TH1F("hResponseECore", "Response; E_{Core}/E_{Truth}; Counts", bins_response, low_response, high_response);
    hResponseECoreCalib  = new TH1F("hResponseECoreCalib", "Response Calib; E_{Core}/E_{Truth}; Counts", bins_response, low_response, high_response);
    hResponseE       = new TH1F("hResponseE", "Response; E/E_{Truth}; Counts", bins_response, low_response, high_response);
    hResponseECalib  = new TH1F("hResponseECalib", "Response Calib; E/E_{Truth}; Counts", bins_response, low_response, high_response);

    hClusECoreCalibVsECore    = new TH2F("hClusECoreCalibVsECore", "Cluster E_{Core} Calib vs E_{Core}; E_{Core} [GeV]; E_{Core} Calib [GeV]", bins_e, low_e, high_e, bins_e, low_e, high_e);
    hClusECoreVsPhotonGE      = new TH2F("hClusECoreVsPhotonGE", "Cluster E_{Core} vs Photon Energy; Photon Energy [GeV]; Cluster E_{Core} [GeV]", bins_ge, low_ge, high_ge, bins_e, low_e, high_e);
    hClusECoreCalibVsPhotonGE = new TH2F("hClusECoreCalibVsPhotonGE", "Cluster E_{Core} Calib vs Photon Energy; Photon Energy [GeV]; Cluster E_{Core} [GeV]", bins_ge, low_ge, high_ge, bins_e, low_e, high_e);
    hResponseECoreVsPhotonGE       = new TH2F("hResponseECoreVsPhotonGE", "Response vs Photon Energy; Photon Energy [GeV]; E_{Core}/E_{Truth}", bins_ge, low_ge, high_ge, bins_response, low_response, high_response);
    hResponseECoreCalibVsPhotonGE  = new TH2F("hResponseECoreCalibVsPhotonGE", "Response Calib vs Photon Energy; Photon Energy [GeV]; E_{Core}/E_{Truth}", bins_ge, low_ge, high_ge, bins_response, low_response, high_response);
    hResponseECoreCalibVsResponse  = new TH2F("hResponseECoreCalibVsResponse", "Response Calib vs Response; E_{Core}/E_{Truth} ; E_{Core}/E_{Truth} Calib", bins_response, low_response, high_response, bins_response, low_response, high_response);
    hResponseEVsPhotonGE       = new TH2F("hResponseEVsPhotonGE", "Response vs Photon Energy; Photon Energy [GeV]; E/E_{Truth}", bins_ge, low_ge, high_ge, bins_response, low_response, high_response);
    hResponseECalibVsPhotonGE  = new TH2F("hResponseECalibVsPhotonGE", "Response Calib vs Photon Energy; Photon Energy [GeV]; E/E_{Truth}", bins_ge, low_ge, high_ge, bins_response, low_response, high_response);
    hResponseECalibVsResponse  = new TH2F("hResponseECalibVsResponse", "Response Calib vs Response; E/E_{Truth} ; E/E_{Truth} Calib", bins_response, low_response, high_response, bins_response, low_response, high_response);
    hClusEtaVsPhotonEta       = new TH2F("hClusEtaVsPhotonEta", "Cluster #eta vs Photon #eta_{Truth}; #eta_{Truth}; #eta", bins_eta, low_eta, high_eta, bins_eta, low_eta, high_eta);
    hClusEtaVsGE              = new TH2F("hClusEtaVsGE", "Cluster #eta vs E_{Truth}; E_{Truth} [GeV]; #eta", bins_ge_response, low_ge_response, high_ge_response, bins_eta, low_eta, high_eta);

    Float_t eta_bin   = (high_eta-low_eta)/bins_eta;
    Float_t ge_bin = (high_ge_response-low_ge_response)/bins_ge_response;
    cout << "eta_bin: " << eta_bin << ", ge_bin: " << ge_bin << endl;

    stringstream title;
    for(UInt_t i = 0; i < bins_ge_response; ++i) {
        vector<TH1F*> hDummy1;
        vector<TH1F*> hDummy2;
        vector<TH1F*> hDummy3;
        vector<TH1F*> hDummy4;
        for(UInt_t j = 0; j < bins_eta; ++j) {
            title.str("");
            title << "Response: " << low_ge_response+i*ge_bin << " #leq E_{Truth} < " << low_ge_response+(i+1)*ge_bin
                  << ", " << low_eta + j*eta_bin << " #leq #eta < " << low_eta + (j+1)*eta_bin
                  << "; E_{Core}/E_{Truth}; Counts";

            string name = "hResponseECore_"+to_string(i)+"_"+to_string(j);
            auto h = new TH1F(name.c_str(), title.str().c_str(), bins_response, low_response, high_response);
            h->Sumw2();
            hDummy1.push_back(h);

            name = "hResponseE_"+to_string(i)+"_"+to_string(j);
            h = new TH1F(name.c_str(), title.str().c_str(), bins_response, low_response, high_response);
            h->Sumw2();
            hDummy3.push_back(h);

            title.str("");
            title << "Response Calib: " << low_ge_response+i*ge_bin << " #leq E_{Truth} < " << low_ge_response+(i+1)*ge_bin
                  << ", " << low_eta + j*eta_bin << " #leq #eta < " << low_eta + (j+1)*eta_bin
                  << "; E_{Core}/E_{Truth}; Counts";

            name = "hResponseECoreCalib_"+to_string(i)+"_"+to_string(j);
            h = new TH1F(name.c_str(), title.str().c_str(), bins_response, low_response, high_response);
            h->Sumw2();
            hDummy2.push_back(h);

            name = "hResponseECalib_"+to_string(i)+"_"+to_string(j);
            h = new TH1F(name.c_str(), title.str().c_str(), bins_response, low_response, high_response);
            h->Sumw2();
            hDummy4.push_back(h);
        }
        hResponsesECore.push_back(hDummy1);
        hResponsesECoreCalib.push_back(hDummy2);
        hResponsesE.push_back(hDummy3);
        hResponsesECalib.push_back(hDummy4);
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
    ntp->SetBranchStatus("e", true);
    ntp->SetBranchStatus("e_calib", true);
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
    cout << "eventsA: " << eventsA << ", eventsB: " << eventsB << ", eventsBWeight: " << eventsBWeight << endl;

    Float_t ecore; // cluster ecore
    Float_t ecore_calib; // PDC calibrated cluster ecore
    Float_t e; // cluster e
    Float_t e_calib; // PDC calibrated cluster e
    Float_t ge; // truth cluster energy
    Float_t gpt; // truth cluster pt
    Float_t eta; // cluster eta in reference to a vertex of (0,0,0)
    Float_t geta; // truth cluster eta

    ntp->SetBranchAddress("ecore",        &ecore);
    ntp->SetBranchAddress("ecore_calib",  &ecore_calib);
    ntp->SetBranchAddress("e",            &e);
    ntp->SetBranchAddress("e_calib",      &e_calib);
    ntp->SetBranchAddress("ge",           &ge);
    ntp->SetBranchAddress("gpt",          &gpt);
    ntp->SetBranchAddress("eta_detector", &eta);
    ntp->SetBranchAddress("geta",         &geta);

    Float_t ecore_min                = 9999;
    Float_t ecore_calib_min          = 9999;
    Float_t e_min                    = 9999;
    Float_t e_calib_min              = 9999;
    Float_t ge_min                   = 9999;
    Float_t gpt_min                  = 9999;
    Float_t eta_min                  = 9999;
    Float_t geta_min                 = 9999;
    Float_t response_ecore_min       = 9999;
    Float_t response_ecore_calib_min = 9999;
    Float_t response_e_min           = 9999;
    Float_t response_e_calib_min     = 9999;

    Float_t ecore_max                = 0;
    Float_t ecore_calib_max          = 0;
    Float_t e_max                    = 0;
    Float_t e_calib_max              = 0;
    Float_t ge_max                   = 0;
    Float_t gpt_max                  = 0;
    Float_t eta_max                  = 0;
    Float_t geta_max                 = 0;
    Float_t response_ecore_max       = 0;
    Float_t response_ecore_calib_max = 0;
    Float_t response_e_max           = 0;
    Float_t response_e_calib_max     = 0;

    auto hDummyGE  = new TH1F("hDummyGE","",bins_ge_response, low_ge_response, high_ge_response);
    auto hDummyEta = new TH1F("hDummyEta","",bins_eta, low_eta, high_eta);

    UInt_t ctr = 0;

    for (UInt_t i = 0; i < events; ++i) {

        ntp->GetEntry(i);

        // track progress of the program
        if(i%100000 == 0) cout << "Progress: " << i*100./events << " %" << endl;

        Float_t response_ecore       = ecore/ge;
        Float_t response_e           = e/ge;
        Float_t response_ecore_calib = ecore_calib/ge;
        Float_t response_e_calib     = e_calib/ge;

        ecore_min                = min(ecore_min, ecore);
        ecore_calib_min          = min(ecore_calib_min, ecore_calib);
        e_min                    = min(e_min, e);
        e_calib_min              = min(e_calib_min, e_calib);
        eta_min                  = min(eta_min, eta);
        ge_min                   = min(ge_min, ge);
        gpt_min                  = min(gpt_min, gpt);
        geta_min                 = min(geta_min, geta);
        response_ecore_min       = min(response_ecore_min, response_ecore);
        response_ecore_calib_min = min(response_ecore_calib_min, response_ecore_calib);
        response_e_min           =  min(response_e_min, response_e);
        response_e_calib_min     =  min(response_e_calib_min, response_e_calib);

        ecore_max                = max(ecore_max, ecore);
        ecore_calib_max          = max(ecore_calib_max, ecore_calib);
        e_max                    = max(e_max, e);
        e_calib_max              = max(e_calib_max, e_calib);
        eta_max                  = max(eta_max, eta);
        ge_max                   = max(ge_max, ge);
        gpt_max                  = max(gpt_max, gpt);
        geta_max                 = max(geta_max, geta);
        response_ecore_max       = max(response_ecore_max, response_ecore);
        response_ecore_calib_max = max(response_ecore_calib_max, response_ecore_calib);
        response_e_max           = max(response_e_max, response_e);
        response_e_calib_max     = max(response_e_calib_max, response_e_calib);

        Float_t weight = 1;
        // use event weight for 5-25 GeV sample
        if(ge >= 5) weight = eventsBWeight;

        hPhotonPt      ->Fill(gpt, weight);

        hClusECore         ->Fill(ecore, weight);
        hClusECoreCalib    ->Fill(ecore_calib, weight);
        hClusE             ->Fill(e, weight);
        hClusECalib        ->Fill(e_calib, weight);
        hClusEta           ->Fill(eta, weight);
        hPhotonGE          ->Fill(ge, weight);
        hPhotonEta         ->Fill(geta, weight);
        hResponseECore     ->Fill(response_ecore, weight);
        hResponseECoreCalib->Fill(response_ecore_calib, weight);
        hResponseE         ->Fill(response_e, weight);
        hResponseECalib    ->Fill(response_e_calib, weight);

        hResponseECoreVsPhotonGE     ->Fill(ge, response_ecore, weight);
        hResponseECoreCalibVsPhotonGE->Fill(ge, response_ecore_calib, weight);
        hResponseECoreCalibVsResponse->Fill(response_ecore, response_ecore_calib, weight);
        hResponseEVsPhotonGE         ->Fill(ge, response_e, weight);
        hResponseECalibVsPhotonGE    ->Fill(ge, response_e_calib, weight);
        hResponseECalibVsResponse    ->Fill(response_e, response_e_calib, weight);
        hClusECoreCalibVsECore       ->Fill(ecore, ecore_calib, weight);
        hClusECoreVsPhotonGE         ->Fill(ge, ecore, weight);
        hClusECoreCalibVsPhotonGE    ->Fill(ge, ecore_calib, weight);
        hClusEtaVsPhotonEta          ->Fill(geta, eta, weight);
        hClusEtaVsGE                 ->Fill(ge, eta, weight);

        Int_t binx = hDummyGE->FindBin(ge)-1;
        Int_t biny = hDummyEta->FindBin(eta)-1;
        if(binx >= 0 && binx < bins_ge_response && biny >= 0 && biny < bins_eta) {
            hResponsesECore[binx][biny]->Fill(response_ecore, weight);
            hResponsesECoreCalib[binx][biny]->Fill(response_ecore_calib, weight);
            hResponsesE[binx][biny]->Fill(response_e, weight);
            hResponsesECalib[binx][biny]->Fill(response_e_calib, weight);
            ++ctr;
        }
        else if(ge >= 0.5 && abs(eta) < 1.1) cout << "eta: " << eta << ", ge: " << ge << endl;
    }

    cout << "ecore min: " << ecore_min << ", ecore max: " << ecore_max << endl;
    cout << "e min: " << e_min << ", e max: " << e_max << endl;
    cout << "ecore calib min: " << ecore_calib_min << ", ecore calib max: " << ecore_calib_max << endl;
    cout << "e calib min: " << e_calib_min << ", e calib max: " << e_calib_max << endl;
    cout << "ge min: " << ge_min << ", ge max: " << ge_max << endl;
    cout << "gpt min: " << gpt_min << ", gpt max: " << gpt_max << endl;
    cout << "eta min: " << eta_min << ", eta max: " << eta_max << endl;
    cout << "geta min: " << geta_min << ", geta max: " << geta_max << endl;
    cout << "response ecore min: " << response_ecore_min << ", response ecore max: " << response_ecore_max << endl;
    cout << "response ecore calib min: " << response_ecore_calib_min << ", response ecore calib max: " << response_ecore_calib_max << endl;
    cout << "response e min: " << response_e_min << ", response e max: " << response_e_max << endl;
    cout << "response e calib min: " << response_e_calib_min << ", response e calib max: " << response_e_calib_max << endl;
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
    hClusE         ->Write();
    hClusECalib    ->Write();
    hClusEta       ->Write();
    hPhotonGE      ->Write();
    hPhotonPt      ->Write();
    hPhotonEta     ->Write();

    hClusEtaVsPhotonEta      ->Write();
    hClusECoreCalibVsECore   ->Write();
    hClusECoreVsPhotonGE     ->Write();
    hClusECoreCalibVsPhotonGE->Write();
    hClusEtaVsGE     ->Write();

    hResponseECore      ->Write();
    hResponseECoreCalib ->Write();

    hResponseECoreVsPhotonGE     ->Write();
    hResponseECoreCalibVsPhotonGE->Write();
    hResponseECoreCalibVsResponse->Write();

    hResponseE      ->Write();
    hResponseECalib ->Write();

    hResponseEVsPhotonGE     ->Write();
    hResponseECalibVsPhotonGE->Write();
    hResponseECalibVsResponse->Write();

    output.mkdir("response_ecore");
    output.mkdir("response_ecore_calib");
    output.mkdir("response_e");
    output.mkdir("response_e_calib");

    for(UInt_t i = 0; i < bins_ge_response; ++i) {
        for(UInt_t j = 0; j < bins_eta; ++j) {
            output.cd("response_ecore");
            if(hResponsesECore[i][j]->Integral(1,bins_response)) hResponsesECore[i][j]->Write();

            output.cd("response_ecore_calib");
            if(hResponsesECoreCalib[i][j]->Integral(1,bins_response)) hResponsesECoreCalib[i][j]->Write();

            output.cd("response_e");
            if(hResponsesE[i][j]->Integral(1,bins_response)) hResponsesE[i][j]->Write();

            output.cd("response_e_calib");
            if(hResponsesECalib[i][j]->Integral(1,bins_response)) hResponsesECalib[i][j]->Write();
        }
    }

    // Close root file
    input->Close();
    output.Close();
    cout << "finished finalize" << endl;
}

void PDC_Analysis_v4(const string &i_input,
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

    PDC_Analysis_v4(argv[1], outputFile, ntp_cluster_entries);

    cout << "done" << endl;
    return 0;
}
# endif
