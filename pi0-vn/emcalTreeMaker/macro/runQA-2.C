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

        h3->GetYaxis()->SetRangeUser(0,1.1);
        h3->GetXaxis()->SetRangeUser(-20,20);

        // h3->SetLineColor(kBlack);

        h3->GetYaxis()->SetLabelSize(0.08);
        h3->GetYaxis()->SetTitleSize(0.08);
        h3->GetXaxis()->SetLabelSize(0.07);
        h3->GetXaxis()->SetTitleSize(0.08);

        h3->GetYaxis()->SetLabelOffset(0.02);

        h3->Draw();

        l1.DrawLatexNDC(0.4,0.7,("Run: " + inputs[i].first).c_str());

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

/*
void myAnalysis::process_event() {

    TCanvas* c1 = new TCanvas();

    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1500, 1000);
    c1->SetLeftMargin(.09);
    c1->SetRightMargin(.13);

    TCanvas* c2 = new TCanvas();

    c2->SetCanvasSize(2000, 1000);
    c2->Divide(7,4,0.00005,0.0005);

    c2->SetTickx();
    c2->SetTicky();

    TCanvas* c3 = new TCanvas();

    // gStyle->SetPadLeftMargin(0.05);
    // gStyle->SetPadRightMargin(0.1);
    c3->SetCanvasSize(3000, 1500);
    c3->Divide(7,4,0.005,0.005);

    // auto pad1 = new TPad("pad1", "pad1", 0.01, 0.01, 1, 1);
    // pad1->Divide(7,4,0.01,0.01);
    // pad1->Draw();

    // c3->SetLeftMargin(0.05);
    // c3->SetRightMargin(.02);

    string output1 = "test.pdf";
    string output2 = "ClusterECore-2D.pdf";
    string output3 = "ClusterECore-1D.pdf";

    c1->Print((output1 + "[").c_str(), "pdf portrait");

    UInt_t ctr[2] = {0};
    Int_t i = 1;
    // loop over each file
    TLatex l1;
    l1.SetTextSize(0.1);

    for(auto input : inputs) {
        cout << "Run: " << input.first << endl;
        auto h = (TH1F*)(input.second->Get("hVtxZ"));
        Int_t low = h->FindBin(-10);
        Int_t high = h->FindBin(10)-1;

        Int_t events = h->Integral(low, high);

        h = (TH1F*)(input.second->Get("hVtxZv2"));
        Int_t events_mb = h->Integral(low, high);

        cout << "Events |z| < 10: " << events << ", Events: |z| < 10 and MB: " << events_mb << endl;

        ctr[0] += events;
        ctr[1] += events_mb;

        c1->cd();

        auto h2 = (TH2F*)(input.second->Get("h2ClusterEtaPhiWeighted")->Clone());

        h2->SetTitle(("Cluster E_{Core}, Run: " + input.first).c_str());
        h2->SetStats(0);
        h2->Draw("COLZ1");

        c1->Print(output1.c_str(), "pdf portrait");

        h = (TH1F*)h2->ProjectionX()->Clone();
        h->SetTitle(("Cluster E_{Core}, Run: " + input.first).c_str());
        h->Draw("HIST");

        c1->Print(output1.c_str(), "pdf portrait");
        // c1->Print((outDir + "/qa-PhotonGE.png").c_str());

        c2->cd(i);
        h2->SetTitle("");
        // h2->SetTitleSize(0.1);
        h2->GetYaxis()->SetLabelSize(0.08);
        h2->GetYaxis()->SetTitleSize(0.08);
        h2->GetYaxis()->SetTitleOffset(0.7);
        h2->GetYaxis()->SetLabelOffset(0.01);
        h2->GetXaxis()->SetLabelSize(0.08);
        h2->GetXaxis()->SetTitleSize(0.08);
        h2->GetZaxis()->SetLabelSize(0.04);
        // h2->GetZaxis()->SetTitleSize(0.08);
        h2->Draw("COLZ1");

        gPad->SetRightMargin(0.15);
        gPad->SetLeftMargin(0.15);
        gPad->SetBottomMargin(0.15);
        gPad->SetTopMargin(0.1);

        l1.DrawLatexNDC(0.3,0.93,("Run: " + input.first).c_str());

        c3->cd(i);
        gPad->SetRightMargin(0.01);
        gPad->SetLeftMargin(0.25);
        gPad->SetBottomMargin(0.15);
        // gPad->SetBottomMargin(2);

        // gPad->SetLeftMargin(1);
        // gPad->SetRightMargin(1);
        // pad1->cd(i);
        // gPad->SetTickx();
        // gPad->SetTicky();

        h->SetTitle("");
        // h->SetTitle(("Cluster E_{Core}, Run: " + input.first).c_str());
        // gPad->GetYaxis()->SetTitleOffset(1);
        h->SetStats(0);
        h->GetYaxis()->SetTitle("Cluster E_{Core} [GeV]");
        h->GetYaxis()->SetLabelSize(0.08);
        h->GetYaxis()->SetTitleSize(0.08);
        h->GetXaxis()->SetLabelSize(0.08);
        h->GetXaxis()->SetTitleSize(0.08);

        h->Draw("HIST");

        // l1->SetTextFont(42);
        l1.DrawLatexNDC(0.4,0.8,("Run: " + input.first).c_str());

        cout << endl;
        ++i;
    }

    c1->Print((output1 + "]").c_str(), "pdf portrait");

    c2->Print(output2.c_str(), "pdf portrait");
    c2->Print("ClusterECore-2D.png");
    c3->Print(output3.c_str(), "pdf portrait");
    c3->Print("ClusterECore-1D.png");

    cout << "Total" << endl;
    cout << "Events |z| < 10: " << ctr[0] << ", Events: |z| < 10 and MB: " << ctr[1] << endl;
}
*/

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
