// -- my includes --
#include "FitUtils.h"

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

    void init(const string &i_input);
    void process_event(UInt_t events, UInt_t start = 0, const string &i_output = "test.root");
    // void finalize(const string &i_output);

    TH1F* hResponse_fit;
    TH2F* h2Response;

    UInt_t  bins_response = 200;
    Float_t low_response  = 0;
    Float_t high_response = 2;

    UInt_t  bins_eta = 768;
    Float_t low_eta  = -0.5;
    Float_t high_eta = 95.5;

    UInt_t  bins_phi = 64;
    Float_t low_phi  = -0.5;
    Float_t high_phi = 7.5;
}

void myAnalysis::init(const string &i_input) {
    cout << "----------------------" << endl;
    cout << "starting init" << endl;
    input = TFile::Open(i_input.c_str());

    hResponse_fit = new TH1F("hResponse_fit", "Response; E_{Core}/E_{Truth}; Counts", bins_response, low_response, high_response);
    h2Response    = new TH2F("h2Response", "Cluster Response; towerid #eta; towerid #phi", bins_eta, low_eta, high_eta, bins_phi, low_phi, high_phi);

    cout << "finished init" << endl;
}

void myAnalysis::process_event(UInt_t events, UInt_t start, const string &i_output) {
    cout << "======================" << endl;
    cout << "starting process event" << endl;

    TFile output(i_output.c_str(),"recreate");

    output.mkdir("response");
    output.mkdir("response/0.6-0.9");
    output.mkdir("response/0.9-1.05");
    output.mkdir("response/1.05-2");

    for (UInt_t i = start; i < start+events; ++i) {
        if((i-start) % 80 == 0) cout << "Progress: " << (i-start)*100./events << " %" << endl;
        Int_t x = i / bins_phi;
        Int_t y = i % bins_phi;

        string name = "response/hResponse_"+to_string(x)+"_"+to_string(y);

        auto h = (TH1F*)input->Get(name.c_str());

        if(!h) continue;

        UInt_t ndf;
        Double_t mu;
        Double_t mu_error;
        Double_t chi2;
        Double_t xmin = 0; // 0.5
        Double_t xmax = 2; // 1.3

        Int_t fitStatus = FitUtils::single_fit(h, ndf, mu, mu_error, chi2, xmin, xmax);

        hResponse_fit->Fill(mu);
        h2Response->SetBinContent(x+1,y+1, mu);

        if(mu < 0.9) output.cd("response/0.6-0.9");
        else if(mu < 1.05) output.cd("response/0.9-1.05");
        else output.cd("response/1.05-2");

        h->Write();

        // cout << "x: " << x << ", y: " << y << ", fit mean: " << mu << endl;
    }

    output.cd();

    hResponse_fit->Write();
    h2Response->Write();

    // Close root file
    input->Close();
    output.Close();

    cout << "finished process event" << endl;
    cout << "======================" << endl;
}

void PDC_fits(const string &i_input,
              UInt_t        events,
              UInt_t        start  = 0,
              const string &i_output = "test.root") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: " << i_input << endl;
    cout << "outputFile: " << i_output << endl;
    cout << "start: " << start << endl;
    cout << "events: " << events << endl;
    cout << "#############################" << endl;

    myAnalysis::init(i_input);

    myAnalysis::process_event(events, start, i_output);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 3 || argc > 5){
        cout << "usage: ./bin/PDC-fits inputFile events [start] [outputFile]" << endl;
        cout << "inputFile: train sample root file. Required." << endl;
        cout << "outputFile: location of output file. Default: test.root." << endl;
        cout << "start: Event offset. Default: 0 (start from the first event)." << endl;
        cout << "events: Number of events to analyze. Default: 0 (to run over all entries)." << endl;
        return 1;
    }

    UInt_t start      = 0;
    string outputFile = "test.root";

    if(argc >= 4) {
        start = atoi(argv[3]);
    }
    if(argc >= 5) {
        outputFile = argv[4];
    }

    PDC_fits(argv[1], atoi(argv[2]), start, outputFile);

    cout << "done" << endl;
    return 0;
}
# endif
