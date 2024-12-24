// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

// -- root includes --
#include <TH2F.h>
#include <TF1.h>
#include <TFile.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TCanvas.h>

// -- Tower Info
#include <calobase/TowerInfoDefs.h>

// -- sPHENIX Style
#include "sPhenixStyle.C"

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
using std::pair;

R__LOAD_LIBRARY(libcalo_io.so)

namespace myAnalysis {
    void plots(const string& i_input, const string &hotMap, const string &output);
    Int_t readHotTowerIndexFile(const string &hotList);

    vector<pair<UInt_t,UInt_t>> hotTowerIndex;
}

Int_t myAnalysis::readHotTowerIndexFile(const string &hotList) {

  cout << "Reading Hot and Ref tower indices" << endl;

  std::ifstream file(hotList);

  // Check if the file was successfully opened
  if (!file.is_open()) {
      cerr << "Failed to open file: " << hotList << endl;
      return 1;
  }

  string line;

  // skip header
  std::getline(file, line);

  // loop over each run
  while (std::getline(file, line)) {
      std::istringstream lineStream(line);

      UInt_t towerHotIndex;
      UInt_t towerRefIndex;
      Char_t comma;

      if (lineStream >> towerHotIndex >> comma >> towerRefIndex) {
          hotTowerIndex.push_back(std::make_pair(towerHotIndex, towerRefIndex));
          cout << "Hot: " << towerHotIndex << ", Ref: " << towerRefIndex << endl;
      }
      else {
          cerr << "Failed to parse line: " << line << endl;
          return 1;
      }
  }

  // Close the file
  file.close();

  return 0;
}

void myAnalysis::plots(const string& i_input, const string &hotMap, const string &output) {
    TFile input(i_input.c_str());
    TFile inputHot(hotMap.c_str());

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1500, 1000);
    c1->SetLeftMargin(.13);
    c1->SetRightMargin(.05);

    c1->Print((output + "[").c_str(), "pdf portrait");

    c1->cd();

    TLatex l1;
    TLatex l2;
    // l1.SetTextSize(0.05);

    auto hHotMap = (TH1F*)inputHot.Get("hBadTowersHot");

    stringstream s;

    UInt_t i = 0;
    // initialize histograms
    for(auto idx : hotTowerIndex) {

        UInt_t key    = TowerInfoDefs::encode_emcal(idx.first);
        UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
        UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

        string name = "Hot/HotTower_"+to_string(phibin)+"_"+to_string(etabin);
        string lHot = "Hot (Status = 2): (" + to_string(phibin) + "," + to_string(etabin) + ")";
        auto hHot   = (TH1F*)input.Get(name.c_str());
        Int_t overFlowHot = hHot->GetBinContent(hHot->GetNbinsX()+1);
        if(hHot->Integral()) hHot->Scale(1./hHot->Integral());

        name            = "HotComplement/HotTowerComplement_"+to_string(phibin)+"_"+to_string(etabin);
        string lHotComp = "Hot (Status #neq 2): (" + to_string(phibin) + "," + to_string(etabin) + ")";
        auto hHotComp   = (TH1F*)input.Get(name.c_str());
        Int_t overFlowHotComp = hHotComp->GetBinContent(hHotComp->GetNbinsX()+1);
        if(hHotComp->Integral()) hHotComp->Scale(1./hHotComp->Integral());

        UInt_t nRuns = hHotMap->GetBinContent(idx.first+1);

        key    = TowerInfoDefs::encode_emcal(idx.second);
        etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
        phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

        name        = "Ref/RefTower_"+to_string(phibin)+"_"+to_string(etabin)+"_"+to_string(i);
        string lRef = "Ref (Status = 0): (" + to_string(phibin) + "," + to_string(etabin) + ")";
        auto hRef   = (TH1F*)input.Get(name.c_str());
        Int_t overFlowRef = hRef->GetBinContent(hRef->GetNbinsX()+1);
        if(hRef->Integral()) hRef->Scale(1./hRef->Integral());

        gPad->SetLogy();
        hHot->SetLineColor(kRed);
        hHot->GetYaxis()->SetTitle("Normalized Counts");
        hHot->GetYaxis()->SetTitleOffset(1.2);
        hHot->Draw("HIST");
        hHotComp->SetLineColor(kGreen+2);
        hHotComp->Draw("HIST same");
        hRef->SetLineColor(kBlue);
        hRef->Draw("HIST same");

        name = "#splitline{#color[2]{Overflow: " + to_string(overFlowHot) + "}}{#splitline{" +
          "#color[3]{Overflow: " + to_string(overFlowHotComp) + "}}{" +
          "#color[4]{Overflow: " + to_string(overFlowRef) + "}}}";

        l1.DrawLatexNDC(0.57,0.7, name.c_str());

        s.str("");
        s << "#color[2]{Runs: " << nRuns << ", (" << (Int_t)(nRuns*10000./305)/100. << " %)}";

        l2.DrawLatexNDC(0.2,0.85, s.str().c_str());

        TLegend *leg = new TLegend(0.52,.75,0.72,.92);
        leg->SetFillStyle(0);
        leg->AddEntry(hHot,lHot.c_str(),"f");
        leg->AddEntry(hHotComp,lHotComp.c_str(),"f");
        leg->AddEntry(hRef,lRef.c_str(),"f");
        leg->Draw("same");

        c1->Print((string(hHot->GetName()) + ".png").c_str());
        c1->Print((output).c_str(), "pdf portrait");

        gPad->SetLogy(0);
        hHot->Rebin(10);
        hHotComp->Rebin(10);
        hRef->Rebin(10);
        auto h = (TH1F*)hHot->Clone("h");
        hHot->Divide(hRef);
        hHot->GetYaxis()->SetTitle("Ratio");
        hHot->SetLineColor(kBlue);
        hHot->Draw("HIST");

        h->Divide(hHotComp);
        h->SetLineColor(kGreen+2);
        h->Draw("HIST same");

        Float_t high = max(hHot->GetMaximum(), h->GetMaximum());
        hHot->GetYaxis()->SetRangeUser(0, high);

        TLine* line = new TLine(0, 1, 1e4, 1);
        line->SetLineColor(kBlack);
        line->SetLineStyle(9);
        // line->SetLineWidth(1);
        line->Draw("same");

        leg = new TLegend(0.6,.75,0.8,.92);
        leg->SetFillStyle(0);
        leg->AddEntry(hHot,"Hot (Status = 2) / Ref (Status = 0)","l");
        leg->AddEntry(h,"Hot (Status = 2) / Hot (Status #neq 2)","l");
        leg->SetTextSize(0.03);
        leg->Draw("same");

        c1->Print((string(hHot->GetName()) + "-ratio.png").c_str());
        c1->Print((output).c_str(), "pdf portrait");

        ++i;
    }

    c1->Print((output + "]").c_str(), "pdf portrait");

    inputHot.Close();
    input.Close();
}

void display_hot(const string &input, const string &hotList, const string &hotMap, const string &output="plots.pdf") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "hotList: "  << hotList << endl;
    cout << "hotMap: "  << hotMap << endl;
    cout << "output: " << output << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    Int_t ret = myAnalysis::readHotTowerIndexFile(hotList);
    if(ret) return;

    myAnalysis::plots(input, hotMap, output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 4 || argc > 5){
        cout << "usage: ./plots-hot input hotList hotMap [output]" << endl;
        cout << "input: input root file" << endl;
        cout << "hotList: list of hot and reference towers" << endl;
        cout << "output: output pdf file" << endl;
        return 1;
    }

    string output  = "plots.pdf";

    if(argc >= 5) {
        output = argv[4];
    }

    display_hot(argv[1], argv[2], argv[3], output);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
