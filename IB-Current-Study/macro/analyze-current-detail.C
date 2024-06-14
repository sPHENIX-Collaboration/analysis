// -- c++ includes --
#include <iostream>
#include <iomanip>
#include <fstream>

// -- root includes --
#include <TH2F.h>
#include <TFile.h>
#include <TMath.h>
#include <TCanvas.h>

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

    struct Data {
        string  readtime;
        Int_t   sector;
        Int_t   ib;
        Float_t imeas;
    };

    vector<Data> dataset;

    void analyze(const string &i_input, const string &i_output, const string& outputPDF);

    UInt_t  bins_eta = 12;
    Float_t eta_low  = -6;
    Float_t eta_high = 6;

    UInt_t  bins_phi = 32;
    Float_t phi_low  = 0;
    Float_t phi_high = 32;

    UInt_t  bins_current = 100;
    Float_t current_low  = 1e3;
    Float_t current_high = 3.5e3;
}

void myAnalysis::analyze(const string &i_input, const string &i_output, const string &outputPDF) {
    TFile input(i_input.c_str());

    auto h2 = (TH2F*)input.Get("h2Current_0");

    vector<TH1F*> hCurrent;
    vector<string> names;

    for(UInt_t ieta = 1; ieta <= bins_eta; ++ieta) {

        string name  = "hCurrent_"+to_string(ieta-1);
        string title = string(h2->GetTitle()) + ", IB #eta Index: " + to_string((Int_t)(eta_low+ieta-1)) + "; Current [#mu A]; Counts";

        auto h1 = new TH1F(name.c_str(), title.c_str(), bins_current, current_low, current_high);

        for(UInt_t iphi = 1; iphi <= bins_phi; ++iphi) h1->Fill(h2->GetBinContent(iphi,ieta));

        hCurrent.push_back(h1);
        names.push_back(name);
    }

    TFile output(i_output.c_str(), "recreate");

    output.cd();

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1200, 1000);
    c1->SetLeftMargin(.08);
    c1->SetRightMargin(.05);

    c1->Print((outputPDF + "[").c_str(), "pdf portrait");

    cout << "Samples: " << hCurrent.size() << endl;
    for(UInt_t i = 0; i < hCurrent.size(); ++i) {

        hCurrent[i]->Write();

        c1->cd();

        hCurrent[i]->Draw();
        gPad->Update();

        auto st = (TPaveStats*)hCurrent[i]->GetListOfFunctions()->FindObject("stats");
        st->SetX1NDC(0.8);
        st->SetX2NDC(0.95);
        st->SetY1NDC(0.7);
        st->SetY2NDC(0.9);

        c1->Print((outputPDF).c_str(), "pdf portrait");
        c1->Print((names[i] + ".png").c_str());
    }

    c1->Print((outputPDF + "]").c_str(), "pdf portrait");

    output.Close();
    input.Close();
}

void analyze_current_detail(const string &input,
                     const string &output = "test-detail.root",
                     const string &outputPDF = "test-detail.pdf") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "    << input << endl;
    cout << "outputFile: "   << output << endl;
    cout << "outputPDF: "    << outputPDF << endl;
    cout << "#############################" << endl;

    myAnalysis::analyze(input, output, outputPDF);
}
