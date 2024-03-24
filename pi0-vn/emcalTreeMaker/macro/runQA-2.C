// -- c++ includes --
#include <iostream>
#include <iomanip>
#include <fstream>

// -- root includes --
#include <TH2F.h>
#include <TFile.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TLatex.h>

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
using std::setprecision;
using std::fixed;

namespace myAnalysis {
    vector<pair<string,TFile*>> inputs; // run, file
    vector<pair<string,TFile*>> inputs2; // run, file

    Int_t init(const string &i_input, const string &i_input2);
    Int_t readFiles(const string &i_input, vector<pair<string,TFile*>> &inputs);

    void process_event();
    void finalize();

    vector<string> cent_key = {"40-60", "20-40", "0-20"};
    vector<string> pt_key   = {"2-2.5", "2.5-3", "3-3.5", "3.5-4", "4-4.5", "4.5-5"};

    TH1F* pt_dum_vec   = new TH1F("pt_dum_vec","",6,2,5);
    TH1F* cent_dum_vec = new TH1F("cent_dum_vec","", 3, 0, 0.6);
}

Int_t myAnalysis::init(const string &i_input, const string &i_input2) {
    Int_t ret = readFiles(i_input, inputs);
    if(ret) return ret;

    return readFiles(i_input2, inputs2);
}

Int_t myAnalysis::readFiles(const string &i_input, vector<pair<string,TFile*>> &inputs) {
    // Create an input stream
    std::ifstream file(i_input);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open root file list: " << i_input << endl;
        return 1;
    }

    cout << "Reading in Files" << endl;
    cout << "======================================" << endl;
    string line;
    while (std::getline(file, line)) {
        // Find the position of the last '.'
        Int_t dot_pos = line.find_last_of('.');
        string run    = line.substr(line.rfind('-')+1, dot_pos-line.rfind('-')-1);
        // Process each line here
        inputs.push_back(make_pair(run, TFile::Open(line.c_str())));
    }

    // Close the file
    file.close();

    return 0;
}

void myAnalysis::process_event() {
    TCanvas* c3 = new TCanvas();

    // gStyle->SetPadLeftMargin(0.05);
    // gStyle->SetPadRightMargin(0.1);
    c3->SetCanvasSize(3000, 1500);
    c3->Divide(7,4,0.005,0.005);

    string output3 = "Z-vtx.pdf";

    TCanvas* c4 = new TCanvas();

    c4->SetCanvasSize(3000, 1500);
    c4->Divide(7,4,0.005,0.005);

    string output4 = "Z-vtx-ratio.pdf";

    // loop over each file
    TLatex l1;
    l1.SetTextSize(0.1);

    for(UInt_t i = 0; i < inputs.size(); ++i) {
        // Tests
        cout << "Run1: " << inputs[i].first << ", Run2: " << inputs2[i].first << endl;

        auto h1 = (TH1F*)((inputs[i].second->Get("hVtxZv2"))->Clone("hVtxZv2_1"));
        auto h2 = (TH1F*)((inputs2[i].second->Get("hVtxZv2"))->Clone("hVtxZv2_2"));
        auto h3 = (TH1F*)((inputs2[i].second->Get("hVtxZv2"))->Clone("hVtxZv2_3"));

        Int_t low = h1->FindBin(-10);
        Int_t high = h1->FindBin(10)-1;

        Int_t events1 = h1->Integral(low, high);
        Int_t events2 = h2->Integral(low, high);

        Float_t ratio = (events1-events2)*1./events1;

        // Tests
        // cout << "h1: " << h1->GetName() << ", h2: " << h2->GetName() << endl;

        c3->cd(i+1);
        gPad->SetRightMargin(0.03);
        gPad->SetLeftMargin(0.27);
        gPad->SetBottomMargin(0.15);
        // gPad->SetBottomMargin(2);

        // gPad->SetLeftMargin(1);
        // gPad->SetRightMargin(1);
        // pad1->cd(i);
        // gPad->SetTickx();
        // gPad->SetTicky();

        h1->SetTitle("");
        // h->SetTitle(("Cluster E_{Core}, Run: " + input.first).c_str());
        // gPad->GetYaxis()->SetTitleOffset(1);
        h1->SetStats(0);

        h1->GetYaxis()->SetTitle("Counts");

        h1->GetXaxis()->SetRangeUser(-20,20);
        h1->GetYaxis()->SetRangeUser(0,h1->GetMaximum()+400);

        // h1->SetLineWidth(1);

        h1->GetYaxis()->SetLabelSize(0.08);
        h1->GetYaxis()->SetTitleSize(0.08);
        h1->GetXaxis()->SetLabelSize(0.07);
        h1->GetXaxis()->SetTitleSize(0.08);

        h2->SetLineColor(kRed);
        // h2->SetLineWidth(1);

        // Test
        // cout << "h1 Max: " << h1->GetMaximum() << endl;

        h1->Draw("HIST");
        h2->Draw("HIST same");

        // l1->SetTextFont(42);
        l1.DrawLatexNDC(0.4,0.8,("Run: " + inputs[i].first).c_str());

        auto legend = new TLegend(0.7,0.55,0.97,0.75);
        // legend->SetHeader("The Legend Title","C"); // option "C" allows to center the header
        legend->AddEntry(h1,"ZDC > 0", "PE");
        legend->AddEntry(h2,"ZDC > 40", "PE");
        legend->SetTextSize(0.05);
        legend->Draw("same");

        c4->cd(i+1);
        gPad->SetRightMargin(0.03);
        gPad->SetLeftMargin(0.24);
        gPad->SetBottomMargin(0.16);
        gPad->SetTopMargin(0.04);

        h3->Divide(h1);

        h3->SetTitle("");
        h3->SetStats(0);

        h3->GetYaxis()->SetTitle("Ratio");

        h3->GetYaxis()->SetRangeUser(0.8,1.1);
        h3->GetXaxis()->SetRangeUser(-20,20);

        h3->GetYaxis()->SetLabelSize(0.08);
        h3->GetYaxis()->SetTitleSize(0.08);
        h3->GetXaxis()->SetLabelSize(0.07);
        h3->GetXaxis()->SetTitleSize(0.08);

        h3->GetYaxis()->SetLabelOffset(0.02);

        h3->Draw();

        l1.DrawLatexNDC(0.4,0.85,("Run: " + inputs[i].first).c_str());

        stringstream t;
        t.str("");
        t << "#scale[0.7]{Missing fraction: " << fixed << setprecision(3) << ratio << "}" << endl;
        l1.DrawLatexNDC(0.3,0.75,t.str().c_str());

        auto f1 = new TF1("f1","1",-20,20);

        f1->SetLineColor(kRed);
        f1->SetLineWidth(1);
        f1->SetLineStyle(2);
        f1->Draw("same");

    }

    c3->Print(output3.c_str(), "pdf portrait");
    c3->Print("Z-vtx.png");

    c4->Print(output4.c_str(), "pdf portrait");
    c4->Print("Z-vtx-ratio.png");
}

void myAnalysis::finalize() {}

void runQA_2(const string &i_input, const string &i_input2) {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "inputFile: "  << i_input << endl;
    cout << "inputFile2: "  << i_input2 << endl;
    cout << "#############################" << endl;

    Int_t ret = myAnalysis::init(i_input, i_input2);
    if(ret != 0) return;

    myAnalysis::process_event();
    myAnalysis::finalize();
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 3 || argc > 3){
        cout << "usage: ./runQA-2 inputFile inputFile2" << endl;
        cout << "inputFile: containing list of root file paths" << endl;
        return 1;
    }

    runQA_2(argv[1], argv[2]);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
