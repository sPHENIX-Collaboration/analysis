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

    enum m_event_type {_70_100, _100_200, _200_500, ABOVE_500};
    vector<string> labels = {"70 GeV to 100 GeV", "100 GeV to 200 GeV", "200 GeV to 500 GeV", "Above 500 GeV"};
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

    c1->SetCanvasSize(2900, 2300);
    c1->SetLeftMargin(.1);
    c1->SetRightMargin(.03);
    c1->SetTopMargin(.05);
    c1->SetBottomMargin(.6);

    gStyle->SetOptTitle(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleFontSize(0.08);
    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.04);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleXOffset(1);
    gStyle->SetTitleYOffset(1);

    string tag = output.substr(0,output.rfind("."));

    ofstream event_log(tag+"-events.csv");
    event_log << "Run,Event,Triggers,pt1,pt2" << endl;

    vector<vector<string>> suffix_vec(4);

    for(UInt_t i = 0; i < JetUtils::m_triggers.size(); ++i) {
        suffix_vec[m_event_type::_70_100].push_back("-trigger-"+to_string(i)+"-pt-70-100.pdf");
        suffix_vec[m_event_type::_100_200].push_back("-trigger-"+to_string(i)+"-pt-100-200.pdf");
        suffix_vec[m_event_type::_200_500].push_back("-trigger-"+to_string(i)+"-pt-200-500.pdf");
        suffix_vec[m_event_type::ABOVE_500].push_back("-trigger-"+to_string(i)+"-pt-above-500.pdf");

        c1->Print((tag + suffix_vec[m_event_type::_70_100][i]   + "[").c_str(), "pdf portrait");
        c1->Print((tag + suffix_vec[m_event_type::_100_200][i]  + "[").c_str(), "pdf portrait");
        c1->Print((tag + suffix_vec[m_event_type::_200_500][i]  + "[").c_str(), "pdf portrait");
        c1->Print((tag + suffix_vec[m_event_type::ABOVE_500][i] + "[").c_str(), "pdf portrait");
    }
    c1->cd();

    UInt_t ctr[5] = {0};

    unordered_map<string,int> triggerPtCtr;

    TH1::AddDirectory(kFALSE);

    TH1* hTriggersBkg;

    vector<TH1*> hTriggersBkg_pt;

    hTriggersBkg_pt.push_back(new TH1F("hTriggersBkg_70_100_","",triggerBits,0,triggerBits));
    hTriggersBkg_pt.push_back(new TH1F("hTriggersBkg_100_200_","",triggerBits,0,triggerBits));
    hTriggersBkg_pt.push_back(new TH1F("hTriggersBkg_200_500_","",triggerBits,0,triggerBits));
    hTriggersBkg_pt.push_back(new TH1F("hTriggersBkg_above_100_","",triggerBits,0,triggerBits));

    // File Loop
    for(UInt_t i = 0; i < files.size(); ++i) {
        if(i%20 == 0) cout << "Progress: " << i << ", " << (i+1)*100./files.size() << " %" << endl;

        string file = files[i];

        TFile* tfile = TFile::Open(file.c_str());

        TList* keysCEMCBase = ((TDirectory*)tfile->Get("CEMCBase"))->GetListOfKeys();
        TList* keysCEMC     = ((TDirectory*)tfile->Get("CEMC"))->GetListOfKeys();
        TList* keysIHCal    = ((TDirectory*)tfile->Get("IHCal"))->GetListOfKeys();
        TList* keysOHCal    = ((TDirectory*)tfile->Get("OHCal"))->GetListOfKeys();

        auto hTriggersBkg_ = (TH1*)tfile->Get("trigger/hTriggersBkg");
        if(i == 0) {
            hTriggersBkg = (TH1*)hTriggersBkg_->Clone();
        }
        else {
            hTriggersBkg->Add(hTriggersBkg_);
        }

        // Event Loop
        for(UInt_t j = 0; j < keysCEMC->GetSize(); ++j) {
            string name  = keysCEMCBase->At(j)->GetName();
            vector<string> tokens = JetUtils::split(name,'_');

            string run         = tokens[1];
            string event       = tokens[2];
            vector<string> triggerIndex = JetUtils::split(tokens[3],'-');
            Int_t leadJetPt    = stoi(tokens[4]);
            Int_t subLeadJetPt = stoi(tokens[5]);

            cout << "Run: " << run << ", Event: " << event
                 << ", trigger: " << tokens[3]
                 << ", pt: " << leadJetPt << " GeV" << endl;

            event_log << run << "," << event << "," << tokens[3] << "," << leadJetPt << "," << subLeadJetPt << endl;

            auto hCEMCBase = (TH2*)tfile->Get(("CEMCBase/"+name).c_str());

            if(hCEMCBase->GetMinimum() > -1) hCEMCBase->SetMinimum(zMin);
            if(hCEMCBase->GetMaximum() < zMax) hCEMCBase->SetMaximum(zMax);

            // hCEMCBase->GetXaxis()->SetLimits(0,64);
            // hCEMCBase->GetXaxis()->SetNdivisions(21, false);
            // hCEMCBase->GetXaxis()->SetLabelSize(0.04);
            // hCEMCBase->GetXaxis()->SetTickSize(0.01);
            // hCEMCBase->GetYaxis()->SetTickSize(0.01);
            // hCEMCBase->GetYaxis()->SetLabelSize(0.04);
            // hCEMCBase->GetYaxis()->SetLimits(0,24);
            // hCEMCBase->GetYaxis()->SetNdivisions(11, false);
            hCEMCBase->GetYaxis()->SetTitleOffset(0.5);

            name  = keysCEMC->At(j)->GetName();
            auto hCEMC = (TH2*)tfile->Get(("CEMC/"+name).c_str());

            hCEMC->SetMinimum(zMin);
            if(hCEMC->GetMaximum() < zMax) hCEMC->SetMaximum(zMax);
            hCEMC->GetYaxis()->SetTitleOffset(0.5);

            name  = keysIHCal->At(j)->GetName();
            auto hIHCal = (TH2*)tfile->Get(("IHCal/"+name).c_str());
            if(hIHCal->GetMaximum() >= zMin) {
                hIHCal->SetMinimum(zMin);
                if(hIHCal->GetMaximum() < zMax) hIHCal->SetMaximum(zMax);

                hIHCal->GetYaxis()->SetTitleOffset(0.5);
            }

            name  = keysOHCal->At(j)->GetName();
            auto hOHCal = (TH2*)tfile->Get(("OHCal/"+name).c_str());
            if(hOHCal->GetMaximum() >= zMin) {
                hOHCal->SetMinimum(zMin);
                if(hOHCal->GetMaximum() < zMax) hOHCal->SetMaximum(zMax);

                hOHCal->GetYaxis()->SetTitleOffset(0.5);
            }

            m_event_type pt_key = m_event_type::_70_100;

            if(leadJetPt >= 70 && leadJetPt < 100) {
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

            // Trigger Loop
            for(auto idx : triggerIndex) {
                Int_t index = stoi(idx);
                string suffix = suffix_vec[pt_key][index];
                hTriggersBkg_pt[pt_key]->Fill(index);

                triggerPtCtr[suffix]++;

                if(savePlots) {

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

                    hCEMCBase->Draw("COLZ1");
                    c1->Print((tag+suffix).c_str(), "pdf portrait");

                    hCEMC->Draw("COLZ1");
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
            }
            ++ctr[0];
        }

        tfile->Close();
    }

    event_log.close();

    // gPad->SetGrid();
    gPad->SetLogy();

    vector<UInt_t> ctr2;
    for(UInt_t i = 1; i <= hTriggersBkg->GetNbinsX(); ++i) {
        if(hTriggersBkg->GetBinContent(i)) ctr2.push_back(i);
    }

    auto hTriggersBkg2 = new TH1F("hTriggersBkg2","Background Event (has jet p_{T} #geq 70 GeV in EMCal); ; Counts", ctr2.size(), 0, ctr2.size());
    auto hTriggersBkg_70_100    = new TH1F("hTriggersBkg_70_100","", ctr2.size() , 0, ctr2.size());
    auto hTriggersBkg_100_200   = new TH1F("hTriggersBkg_100_200","", ctr2.size(), 0, ctr2.size());
    auto hTriggersBkg_200_500   = new TH1F("hTriggersBkg_200_500","", ctr2.size(), 0, ctr2.size());
    auto hTriggersBkg_above_500 = new TH1F("hTriggersBkg_above_500","", ctr2.size(), 0, ctr2.size());

    for(UInt_t i = 0; i < ctr2.size(); ++i) {
        string triggerName = hTriggersBkg->GetXaxis()->GetBinLabel(ctr2[i]);

        UInt_t triggerEventsBkg = hTriggersBkg->GetBinContent(ctr2[i]);
        UInt_t triggerEventsBkg_70_100 = hTriggersBkg_pt[m_event_type::_70_100]->GetBinContent(ctr2[i]);
        UInt_t triggerEventsBkg_100_200 = hTriggersBkg_pt[m_event_type::_100_200]->GetBinContent(ctr2[i]);
        UInt_t triggerEventsBkg_200_500 = hTriggersBkg_pt[m_event_type::_200_500]->GetBinContent(ctr2[i]);
        UInt_t triggerEventsBkg_above_500 = hTriggersBkg_pt[m_event_type::ABOVE_500]->GetBinContent(ctr2[i]);

        hTriggersBkg2->SetBinContent(i+1, triggerEventsBkg);
        hTriggersBkg_70_100->SetBinContent(i+1, triggerEventsBkg_70_100);
        hTriggersBkg_100_200->SetBinContent(i+1, triggerEventsBkg_100_200);
        hTriggersBkg_200_500->SetBinContent(i+1, triggerEventsBkg_200_500);
        hTriggersBkg_above_500->SetBinContent(i+1, triggerEventsBkg_above_500);

        hTriggersBkg2->GetXaxis()->SetBinLabel(i+1, triggerName.c_str());
        hTriggersBkg_70_100->GetXaxis()->SetBinLabel(i+1, triggerName.c_str());
        hTriggersBkg_100_200->GetXaxis()->SetBinLabel(i+1, triggerName.c_str());
        hTriggersBkg_200_500->GetXaxis()->SetBinLabel(i+1, triggerName.c_str());
        hTriggersBkg_above_500->GetXaxis()->SetBinLabel(i+1, triggerName.c_str());
    }

    c1->Print((tag+"-triggers.pdf[").c_str(), "pdf portrait");

    hTriggersBkg2->GetXaxis()->SetLabelSize(0.048);
    hTriggersBkg2->GetYaxis()->SetRangeUser(5e-1,1e4);

    hTriggersBkg2->LabelsOption("v");
    hTriggersBkg2->Draw();

    c1->Print((tag+"-triggers.pdf").c_str(), "pdf portrait");
    c1->Print((tag+"-triggers-single.png").c_str());

    hTriggersBkg_70_100->SetLineColor(kBlue);
    hTriggersBkg_100_200->SetLineColor(kOrange+7);
    hTriggersBkg_200_500->SetLineColor(kGreen+3);
    hTriggersBkg_above_500->SetLineColor(kRed);

    hTriggersBkg_70_100->Draw("same");
    hTriggersBkg_100_200->Draw("same");
    hTriggersBkg_200_500->Draw("same");
    hTriggersBkg_above_500->Draw("same");

    TLegend *leg = new TLegend(0.74,.75,0.8,.92);
    leg->SetFillStyle(0);
    leg->AddEntry(hTriggersBkg2,"Jet p_{T} #geq 70 GeV","f");
    leg->AddEntry(hTriggersBkg_70_100,"70 GeV #leq Jet p_{T} < 100 GeV","f");
    leg->AddEntry(hTriggersBkg_100_200,"100 GeV #leq Jet p_{T} < 200 GeV","f");
    leg->AddEntry(hTriggersBkg_200_500,"200 GeV #leq Jet p_{T} < 500 GeV","f");
    leg->AddEntry(hTriggersBkg_above_500,"Jet p_{T} #geq 500 GeV","f");
    leg->SetTextSize(0.02);
    leg->Draw("same");

    c1->Print((tag+"-triggers.pdf").c_str(), "pdf portrait");
    c1->Print((tag+"-triggers-multiple.png").c_str());

    c1->Print((tag+"-triggers.pdf]").c_str(), "pdf portrait");


    for(UInt_t i = 0; i < JetUtils::m_triggers.size(); ++i) {
        c1->Print((tag + suffix_vec[m_event_type::_70_100][i]   + "]").c_str(), "pdf portrait");
        c1->Print((tag + suffix_vec[m_event_type::_100_200][i]  + "]").c_str(), "pdf portrait");
        c1->Print((tag + suffix_vec[m_event_type::_200_500][i]  + "]").c_str(), "pdf portrait");
        c1->Print((tag + suffix_vec[m_event_type::ABOVE_500][i] + "]").c_str(), "pdf portrait");
    }

    cout << "----------------------------" << endl;
    cout << "Trigger Pt Stats" << endl;

    // cleaning step, remove extra files
    for(UInt_t i = 0; i < JetUtils::m_triggers.size(); ++i) {
        for(UInt_t j = 0; j < suffix_vec.size(); ++j) {
            vector<string> suffix = suffix_vec[j];
            if(triggerPtCtr.find(suffix[i]) == triggerPtCtr.end() || !savePlots) {
                std::remove((tag+suffix[i]).c_str());
            }
            if(triggerPtCtr.find(suffix[i]) != triggerPtCtr.end()) {
                cout << "Trigger: " << JetUtils::m_triggers[i]
                     << ", Jet Pt: " << labels[j]
                     << ", Events: " << triggerPtCtr[suffix[i]] << endl;
            }
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
