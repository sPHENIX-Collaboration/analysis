// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

// -- root includes --
#include <TH2F.h>
#include <TF1.h>
#include <TKey.h>
#include <TFile.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TCanvas.h>

// -- sPHENIX Style
#include "sPhenixStyle.C"

// -- Utils
#include "jetvalidation/JetUtils.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::stringstream;
using std::min;
using std::max;
using std::ofstream;
using std::unique_ptr;
using std::unordered_map;

namespace myAnalysis {
    void plots(const string &output);
    Int_t readFile(const string &input, const string &output = "");
    void convert(TH2* p, TH2* h);

    vector<string> files;
    string dirname = "CEMC";
    Float_t zMin = 1;
    Float_t zMax = 100;
    UInt_t bins_phi = 64;
    Float_t phi_low = -M_PI;
    Float_t phi_high = M_PI;
    UInt_t bins_eta = 24;
    Float_t eta_low = -1.1;
    Float_t eta_high = 1.1;

    vector<string> m_triggers = {"None"
                               , "MBD N&S >= 1"
                               , "MBD N&S >= 1, vtx < 10 cm"
                               , "MBD N&S >= 1, vtx < 30 cm"
                               , "MBD N&S >= 1, vtx < 60 cm"
                               , "Jet 6 GeV + MBD NS >= 1"
                               , "Jet 8 GeV + MBD NS >= 1"
                               , "Jet 10 GeV + MBD NS >= 1"
                               , "Jet 12 GeV + MBD NS >= 1"
                               , "Jet 6 GeV, MBD N&S >= 1, vtx < 10 cm"
                               , "Jet 8 GeV, MBD N&S >= 1, vtx < 10 cm"
                               , "Jet 10 GeV, MBD N&S >= 1, vtx < 10 cm"
                               , "Jet 12 GeV, MBD N&S >= 1, vtx < 10 cm"
                              };
}

Int_t myAnalysis::readFile(const string &input, const string &output) {
    std::ifstream file(input);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open file: " << input << endl;
        return 1;
    }

    string line;
    ofstream outputFile;

    if(!output.empty()) outputFile.open(output);

    UInt_t ctr = 0;
    // loop over each run
    while (std::getline(file, line)) {
        ++ctr;
        TFile* tfile = TFile::Open(line.c_str());
        if (!tfile || tfile->IsZombie()) {
            std::cerr << "Error opening file: " << line << std::endl;
            continue;
        }

        TDirectory* dir = (TDirectory*)tfile->Get(dirname.c_str());
        if (!dir) {
            std::cerr << "Directory not found: " << dirname << std::endl;
            tfile->Close();
            continue;
        }

        if(dir->GetListOfKeys()->GetSize()) {
            files.push_back(line);
            cout << "Reading file: " << line << endl;
            if(!output.empty()) outputFile << line << endl;
        }

        tfile->Close();
    }

    if(!output.empty()) outputFile.close();

    cout << "Reading: " << files.size() << " files, " << files.size()*100./ctr << " %" << endl;

    // Close the file
    file.close();

    return 0;
}

void myAnalysis::plots(const string &output) {

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06);
    c1->SetRightMargin(.12);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    gStyle->SetOptTitle(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleFontSize(0.08);
    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.08);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleXOffset(1);
    gStyle->SetTitleYOffset(1);

    // gPad->SetGrid();

    string tag = output.substr(0,output.rfind("."));

    vector<string> suffix_70_100;
    vector<string> suffix_100_200;
    vector<string> suffix_200_500;
    vector<string> suffix_above_500;

    for(UInt_t i = 0; i < m_triggers.size(); ++i) {
        suffix_70_100.push_back("-trigger-"+to_string(i)+"-pt-70-100.pdf");
        suffix_100_200.push_back("-trigger-"+to_string(i)+"-pt-100-200.pdf");
        suffix_200_500.push_back("-trigger-"+to_string(i)+"-pt-200-500.pdf");
        suffix_above_500.push_back("-trigger-"+to_string(i)+"-pt-above-500.pdf");

        c1->Print((tag + suffix_70_100[i]    + "[").c_str(), "pdf portrait");
        c1->Print((tag + suffix_100_200[i]   + "[").c_str(), "pdf portrait");
        c1->Print((tag + suffix_200_500[i]   + "[").c_str(), "pdf portrait");
        c1->Print((tag + suffix_above_500[i] + "[").c_str(), "pdf portrait");
    }
    c1->cd();

    UInt_t ctr = 0;

    unordered_map<string,int> triggerPtCtr;

    for(UInt_t i = 0; i < files.size(); ++i) {
        if(i%20 == 0) cout << "Progress: " << i << ", " << (i+1)*100./files.size() << " %" << endl;

        string file = files[i];

        TFile* tfile = TFile::Open(file.c_str());

        TList* keysCEMCBase = ((TDirectory*)tfile->Get("CEMCBase"))->GetListOfKeys();
        TList* keysCEMC     = ((TDirectory*)tfile->Get("CEMC"))->GetListOfKeys();
        TList* keysIHCal    = ((TDirectory*)tfile->Get("IHCal"))->GetListOfKeys();
        TList* keysOHCal    = ((TDirectory*)tfile->Get("OHCal"))->GetListOfKeys();

        for(UInt_t j = 0; j < keysCEMC->GetSize(); ++j) {
            string name  = keysCEMCBase->At(j)->GetName();
            vector<string> tokens = JetUtils::split(name,'_');

            string run         = tokens[1];
            string event       = tokens[2];
            Int_t triggerIndex = stoi(tokens[3]);
            Int_t leadJetPt    = stoi(tokens[4]);

            cout << "Run: " << run << ", Event: " << event
                 << ", trigger: " << m_triggers[triggerIndex]
                 << ", pt: " << leadJetPt << " GeV" << endl;

            string suffix = suffix_70_100[triggerIndex];
            if(leadJetPt >= 100 && leadJetPt < 200) suffix = suffix_100_200[triggerIndex];
            if(leadJetPt >= 200 && leadJetPt < 500) suffix = suffix_200_500[triggerIndex];
            if(leadJetPt >= 500) suffix = suffix_above_500[triggerIndex];

            triggerPtCtr[suffix]++;

            auto hCEMCBase = (TH2*)tfile->Get(("CEMCBase/"+name).c_str());

            if(hCEMCBase->GetMinimum() > -1) hCEMCBase->SetMinimum(zMin);
            if(hCEMCBase->GetMaximum() < zMax) hCEMCBase->SetMaximum(zMax);

            hCEMCBase->Draw("COLZ1");

            // hCEMCBase->GetXaxis()->SetLimits(0,64);
            // hCEMCBase->GetXaxis()->SetNdivisions(21, false);
            // hCEMCBase->GetXaxis()->SetLabelSize(0.04);
            // hCEMCBase->GetXaxis()->SetTickSize(0.01);
            // hCEMCBase->GetYaxis()->SetTickSize(0.01);
            // hCEMCBase->GetYaxis()->SetLabelSize(0.04);
            // hCEMCBase->GetYaxis()->SetLimits(0,24);
            // hCEMCBase->GetYaxis()->SetNdivisions(11, false);
            hCEMCBase->GetYaxis()->SetTitleOffset(0.5);

            c1->Print((tag+suffix).c_str(), "pdf portrait");

            name  = keysCEMC->At(j)->GetName();
            auto hCEMC = (TH2*)tfile->Get(("CEMC/"+name).c_str());

            hCEMC->SetMinimum(zMin);
            if(hCEMC->GetMaximum() < zMax) hCEMC->SetMaximum(zMax);

            hCEMC->Draw("COLZ1");

            hCEMC->GetYaxis()->SetTitleOffset(0.5);

            c1->Print((tag+suffix).c_str(), "pdf portrait");

            name  = keysIHCal->At(j)->GetName();
            auto hIHCal = (TH2*)tfile->Get(("IHCal/"+name).c_str());
            if(hIHCal->GetMaximum() >= zMin) {
                hIHCal->SetMinimum(zMin);
                if(hIHCal->GetMaximum() < zMax) hIHCal->SetMaximum(zMax);

                hIHCal->Draw("COLZ1");

                hIHCal->GetYaxis()->SetTitleOffset(0.5);

                c1->Print((tag+suffix).c_str(), "pdf portrait");
            }

            name  = keysOHCal->At(j)->GetName();
            auto hOHCal = (TH2*)tfile->Get(("OHCal/"+name).c_str());
            if(hOHCal->GetMaximum() >= zMin) {
                hOHCal->SetMinimum(zMin);
                if(hOHCal->GetMaximum() < zMax) hOHCal->SetMaximum(zMax);

                hOHCal->Draw("COLZ1");

                hOHCal->GetYaxis()->SetTitleOffset(0.5);

                c1->Print((tag+suffix).c_str(), "pdf portrait");
            }

            ++ctr;
        }

        tfile->Close();
    }

    for(UInt_t i = 0; i < m_triggers.size(); ++i) {
        c1->Print((tag + suffix_70_100[i]    + "]").c_str(), "pdf portrait");
        c1->Print((tag + suffix_100_200[i]   + "]").c_str(), "pdf portrait");
        c1->Print((tag + suffix_200_500[i]   + "]").c_str(), "pdf portrait");
        c1->Print((tag + suffix_above_500[i] + "]").c_str(), "pdf portrait");
    }

    cout << "----------------------------" << endl;
    cout << "Trigger Pt Stats" << endl;
    // cleaning step, remove extra files
    for(UInt_t i = 0; i < m_triggers.size(); ++i) {
        if(triggerPtCtr.find(suffix_70_100[i])    == triggerPtCtr.end()) std::remove((tag+suffix_70_100[i]).c_str());
        else cout << "Trigger: " << m_triggers[i] << ", Jet Pt: 70 GeV to 100 GeV, Events: " << triggerPtCtr[suffix_70_100[i]] << endl;

        if(triggerPtCtr.find(suffix_100_200[i])   == triggerPtCtr.end()) std::remove((tag+suffix_100_200[i]).c_str());
        else cout << "Trigger: " << m_triggers[i] << ", Jet Pt: 100 GeV to 200 GeV, Events: " << triggerPtCtr[suffix_100_200[i]] << endl;

        if(triggerPtCtr.find(suffix_200_500[i])   == triggerPtCtr.end()) std::remove((tag+suffix_200_500[i]).c_str());
        else cout << "Trigger: " << m_triggers[i] << ", Jet Pt: 200 GeV to 500 GeV, Events: " << triggerPtCtr[suffix_200_500[i]] << endl;

        if(triggerPtCtr.find(suffix_above_500[i]) == triggerPtCtr.end()) std::remove((tag+suffix_above_500[i]).c_str());
        else cout << "Trigger: " << m_triggers[i] << ", Jet Pt: Above 500 GeV, Events: " << triggerPtCtr[suffix_above_500[i]] << endl;
    }
    cout << "----------------------------" << endl;

    cout << "Total Events: " << ctr << endl;
}

void display_events(const string &input, const string &output="plots.pdf", const string &outputFileList="") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "output: " << output << endl;
    cout << "outputFileList: " << outputFileList << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    myAnalysis::readFile(input, outputFileList);

    myAnalysis::plots(output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 4){
        cout << "usage: ./display-events input [output] [outputFileList]" << endl;
        cout << "input: input list root file" << endl;
        cout << "output: output pdf file" << endl;
        cout << "outputFileList: List of files that have data." << endl;
        return 1;
    }

    string output  = "plots.pdf";
    string outputFileList  = "";

    if(argc >= 3) {
        output = argv[2];
    }
    if(argc >= 4) {
        outputFileList = argv[3];
    }

    display_events(argv[1], output, outputFileList);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
