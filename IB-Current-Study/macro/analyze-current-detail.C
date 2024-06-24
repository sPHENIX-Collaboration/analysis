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

    // there are 6 interface boards for north and south (total 12 along eta)
    UInt_t  bins_eta = 12;
    Float_t eta_low  = -6;
    Float_t eta_high = 6;

    // there are 32 sectors in phi
    UInt_t  bins_phi = 32;
    Float_t phi_low  = 0;
    Float_t phi_high = 2*M_PI;

    UInt_t  bins_current = 100;
    Float_t current_low  = 1e3;
    Float_t current_high = 3.5e3;

    // tune the number of samples to process
    Int_t process_samples = 60;
}

void myAnalysis::analyze(const string &i_input, const string &i_output, const string &outputPDF) {
    TFile input(i_input.c_str());


    vector<TH1F*> hCurrentNorth;
    vector<TH1F*> hCurrentSouth;

    for(UInt_t i = 0; i < process_samples; ++i) {

        string name  = "h2Current_"+to_string(i);
        string name1 = "hNorth_"+to_string(i);
        string name2 = "hSouth_"+to_string(i);

        auto h2 = (TH2F*)input.Get(name.c_str());

        string title = string(h2->GetTitle()) + ", North IB: 4; #phi; Current [#mu A]";

        auto hNorth = new TH1F(name1.c_str(), title.c_str(), bins_phi, phi_low, phi_high);

        title = string(h2->GetTitle()) + ", South IB: 4; #phi; Current [#mu A]";

        auto hSouth = new TH1F(name2.c_str(), title.c_str(), bins_phi, phi_low, phi_high);

        for(UInt_t iphi = 1; iphi <= bins_phi; ++iphi) {
            Float_t val = h2->GetBinContent(iphi,bins_eta-1);
            if(val) hNorth->SetBinContent(iphi, val);

            val = h2->GetBinContent(iphi,2);
            if(val) hSouth->SetBinContent(iphi, val);
        }

        hCurrentNorth.push_back(hNorth);
        hCurrentSouth.push_back(hSouth);
    }

    TFile output(i_output.c_str(), "recreate");

    output.cd();

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1200, 1000);
    c1->SetLeftMargin(.13);
    c1->SetRightMargin(.05);

    c1->cd();

    hCurrentNorth[0]->SetStats(0);
    hCurrentNorth[0]->SetLineColor(kRed);
    // hCurrentNorth[0]->SetLineWidth(1);
    hCurrentNorth[0]->GetYaxis()->SetRangeUser(0, 3.5e3);
    hCurrentNorth[0]->SetTitle("Thu Jun 13 2024 09:16:56 to 10:00:49");
    hCurrentNorth[0]->Draw();

    hCurrentSouth[0]->SetLineColor(kBlue);
    // hCurrentSouth[0]->SetLineWidth(1);
    hCurrentSouth[0]->Draw("same");

    TLegend *leg = new TLegend(0.7,0.7,0.95,0.9);
    leg->AddEntry(hCurrentNorth[0], "North IB 4");
    leg->AddEntry(hCurrentSouth[0], "South IB 4");
    // leg->SetBorderSize(0);
    leg->Draw("same");

    for(UInt_t i = 0; i < hCurrentNorth.size(); ++i) {
        if(i == 14 || (i >= 22 && i <= 25)) continue;

        hCurrentNorth[i]->Write();
        hCurrentSouth[i]->Write();

        hCurrentNorth[i]->SetLineColor(kRed);
        // hCurrentNorth[i]->SetLineWidth(1);
        hCurrentNorth[i]->Draw("same");

        hCurrentSouth[i]->SetLineColor(kBlue);
        // hCurrentSouth[i]->SetLineWidth(1);
        hCurrentSouth[i]->Draw("same");
    }

    c1->Print("hNorthSouth.png");

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
