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
    Bool_t savePlots = true;
    UInt_t triggerBits = 42;

    enum m_event_type {_60_100, _100_200, _200_500, ABOVE_500};
    vector<string> labels = {"60 GeV to 100 GeV", "100 GeV to 200 GeV", "200 GeV to 500 GeV", "Above 500 GeV"};
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

    string tag = output.substr(0,output.rfind("."));

    ofstream event_log(tag+"-events.csv");
    event_log << "Run,Event,Triggers,zvtx,pt1,pt2,frcem,frcoh,maxJetET,dPhi,isDijet" << endl;

    vector<string> suffix_vec;

    suffix_vec.push_back("-pt-60-100.pdf");
    suffix_vec.push_back("-pt-100-200.pdf");
    suffix_vec.push_back("-pt-200-500.pdf");
    suffix_vec.push_back("-pt-above-500.pdf");

    c1->Print((tag + suffix_vec[m_event_type::_60_100]   + "[").c_str(), "pdf portrait");
    c1->Print((tag + suffix_vec[m_event_type::_100_200]  + "[").c_str(), "pdf portrait");
    c1->Print((tag + suffix_vec[m_event_type::_200_500]  + "[").c_str(), "pdf portrait");
    c1->Print((tag + suffix_vec[m_event_type::ABOVE_500] + "[").c_str(), "pdf portrait");

    c1->cd();

    UInt_t ctr[5] = {0};

    TH1::AddDirectory(kFALSE);

    // File Loop
    for(UInt_t i = 0; i < files.size(); ++i) {
        if(i%20 == 0) cout << "Progress: " << i << ", " << (i+1)*100./files.size() << " %" << endl;

        string file = files[i];

        TFile* tfile = TFile::Open(file.c_str());

        TList* keysCEMCBase = ((TDirectory*)tfile->Get("CEMCBase"))->GetListOfKeys();
        TList* keysCEMC     = ((TDirectory*)tfile->Get("CEMC"))->GetListOfKeys();
        TList* keysIHCal    = ((TDirectory*)tfile->Get("IHCal"))->GetListOfKeys();
        TList* keysOHCal    = ((TDirectory*)tfile->Get("OHCal"))->GetListOfKeys();

        // Event Loop
        for(UInt_t j = 0; j < keysCEMC->GetSize(); ++j) {
            string name  = keysCEMC->At(j)->GetName();
            vector<string> tokens = JetUtils::split(name,'_');

            string run         = tokens[1];
            string event       = tokens[2];
            vector<string> triggerIndex = JetUtils::split(tokens[3],'-');
            Int_t zvtx         = stoi(tokens[4]);
            Int_t leadJetPt    = stoi(tokens[5]);
            Int_t subLeadJetPt = stoi(tokens[6]);
            Float_t frcem      = stof(tokens[7]);
            Float_t frcoh      = stof(tokens[8]);
            Int_t maxJetET     = stoi(tokens[9]);
            Float_t dPhi       = stof(tokens[10]);
            Bool_t isDijet     = stoi(tokens[11]);

            cout << "Run: " << run << ", Event: " << event
                 << ", trigger: " << tokens[3]
                 << ", Z: " << zvtx << " cm"
                 << ", pt: " << leadJetPt << " GeV" << endl;

            event_log << run << "," << event << "," << tokens[3] << "," << zvtx << "," << leadJetPt << "," << subLeadJetPt
                      << "," << frcem << "," << frcoh << "," << maxJetET << "," << dPhi << "," << isDijet << endl;

            auto hCEMC = (TH2*)tfile->Get(("CEMC/"+name).c_str());

            // hCEMC->GetXaxis()->SetLimits(0,64);
            // hCEMC->GetXaxis()->SetNdivisions(21, false);
            // hCEMC->GetXaxis()->SetLabelSize(0.04);
            // hCEMC->GetXaxis()->SetTickSize(0.01);
            // hCEMC->GetYaxis()->SetTickSize(0.01);
            // hCEMC->GetYaxis()->SetLabelSize(0.04);
            // hCEMC->GetYaxis()->SetLimits(0,24);
            // hCEMC->GetYaxis()->SetNdivisions(11, false);
            hCEMC->GetYaxis()->SetTitleOffset(0.5);

            name  = keysCEMCBase->At(j)->GetName();
            auto hCEMCBase = (TH2*)tfile->Get(("CEMCBase/"+name).c_str());
            hCEMCBase->GetYaxis()->SetTitleOffset(0.5);

            name  = keysIHCal->At(j)->GetName();
            auto hIHCal = (TH2*)tfile->Get(("IHCal/"+name).c_str());
            if(hIHCal->GetMaximum() >= zMin) {
                hIHCal->SetMinimum(zMin);

                hIHCal->GetYaxis()->SetTitleOffset(0.5);
            }

            name  = keysOHCal->At(j)->GetName();
            auto hOHCal = (TH2*)tfile->Get(("OHCal/"+name).c_str());
            if(hOHCal->GetMaximum() >= zMin) {
                hOHCal->SetMinimum(zMin);

                hOHCal->GetYaxis()->SetTitleOffset(0.5);
            }

            m_event_type pt_key = m_event_type::_60_100;

            if(leadJetPt >= 60 && leadJetPt < 100) {
                ++ctr[1];
            }
            else if(leadJetPt >= 100 && leadJetPt < 200) {
                pt_key = m_event_type::_100_200;
                ++ctr[2];
            }
            else if(leadJetPt >= 200 && leadJetPt < 500) {
                pt_key = m_event_type::_200_500;
                ++ctr[3];
            }
            else if(leadJetPt >= 500) {
                pt_key = m_event_type::ABOVE_500;
                ++ctr[4];
            }

            string suffix = suffix_vec[pt_key];

            if(savePlots) {
                hCEMCBase->Draw("COLZ1");
                c1->Print((tag+suffix).c_str(), "pdf portrait");

                hCEMCBase->SetMinimum(zMin);
                c1->Print((tag+suffix).c_str(), "pdf portrait");

                hCEMC->Draw("COLZ1");
                c1->Print((tag+suffix).c_str(), "pdf portrait");

                hCEMC->SetMinimum(zMin);
                c1->Print((tag+suffix).c_str(), "pdf portrait");

                if(hIHCal->GetMaximum() >= zMin) {
                    hIHCal->Draw("COLZ1");
                    c1->Print((tag+suffix).c_str(), "pdf portrait");
                }

                if(hOHCal->GetMaximum() >= zMin) {
                    hOHCal->Draw("COLZ1");
                    c1->Print((tag+suffix).c_str(), "pdf portrait");
                }
            }
            ++ctr[0];
        }
        tfile->Close();
    }

    event_log.close();

    // gPad->SetGrid();
    gPad->SetLogy();

    c1->Print((tag + suffix_vec[m_event_type::_60_100]   + "]").c_str(), "pdf portrait");
    c1->Print((tag + suffix_vec[m_event_type::_100_200]  + "]").c_str(), "pdf portrait");
    c1->Print((tag + suffix_vec[m_event_type::_200_500]  + "]").c_str(), "pdf portrait");
    c1->Print((tag + suffix_vec[m_event_type::ABOVE_500] + "]").c_str(), "pdf portrait");

    // cleaning step, remove extra files
    for(UInt_t j = 0; j < labels.size(); ++j) {
        string suffix = suffix_vec[j];
        if(ctr[j+1] == 0 || !savePlots) {
            std::remove((tag+suffix).c_str());
        }
    }

    cout << "----------------------------" << endl;
    cout << "Total Events: " << ctr[0] << endl;
    for(UInt_t i = 0; i < labels.size(); ++i) {
        cout << labels[i] << ": " << ctr[i+1] << " Events" << endl;
    }
}

void display_events(const string &input,
                    const string &output="plots.pdf",
                    Bool_t savePlots=true,
                    const string &outputFileList="") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "output: " << output << endl;
    cout << "Save Plots: " << savePlots << endl;
    cout << "outputFileList: " << outputFileList << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    myAnalysis::savePlots = savePlots;

    myAnalysis::readFile(input, outputFileList);

    myAnalysis::plots(output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 5){
        cout << "usage: ./display-events input [output] [savePlots] [outputFileList]" << endl;
        cout << "input: input list root file" << endl;
        cout << "output: output pdf file" << endl;
        cout << "savePlots: save event plots" << endl;
        cout << "outputFileList: List of files that have data." << endl;
        return 1;
    }

    string output  = "plots.pdf";
    Bool_t savePlots = true;
    string outputFileList  = "";

    if(argc >= 3) {
        output = argv[2];
    }
    if(argc >= 4) {
        savePlots = atoi(argv[3]);
    }
    if(argc >= 5) {
        outputFileList = argv[4];
    }

    display_events(argv[1], output, savePlots, outputFileList);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
