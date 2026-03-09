// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <regex>
#include <unordered_map>

// -- root includes --
#include <TH1F.h>
#include <TGraph.h>
#include <TFile.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TCanvas.h>

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
using std::unordered_map;

namespace myAnalysis
{
  Int_t read(const string &inputFile);
  Int_t readZDCNS(const string &inputFile);
  void init();
  void write(const string &outputFile);
  void plot(const string &output);

  string extractNumberFromLastToken(const string &filename);

  struct RunInfo {
    string run;
    UInt_t evt_Jet6;
    UInt_t evt_Jet8;
    UInt_t evt_Jet10;
    UInt_t evt_Jet12;
    UInt_t evt_Jet6_mbdNS;
    UInt_t evt_Jet8_mbdNS;
    UInt_t evt_Jet10_mbdNS;
    UInt_t evt_Jet12_mbdNS;
    UInt_t evt_Jet6_bkg;
    UInt_t evt_Jet8_bkg;
    UInt_t evt_Jet10_bkg;
    UInt_t evt_Jet12_bkg;
    UInt_t evt_Jet6_mbdNS_bkg;
    UInt_t evt_Jet8_mbdNS_bkg;
    UInt_t evt_Jet10_mbdNS_bkg;
    UInt_t evt_Jet12_mbdNS_bkg;
    UInt_t evt_Jet6_bkgCEMC;
    UInt_t evt_Jet8_bkgCEMC;
    UInt_t evt_Jet10_bkgCEMC;
    UInt_t evt_Jet12_bkgCEMC;

    vector<TH1*> hEvents_JetX;
    vector<TH1*> hEvents_JetX_bkg;
  };

  enum class EventStatus {
    trigger = 0,
    trigger_bkg = 1,
    trigger_bkgCEMC = 2,
    trigger_mbdNS = 3,
    trigger_mbdNS_bkg = 4
  };

  vector<string> triggerIdx = {"Jet6", "Jet8", "Jet10", "Jet12"};

  vector<RunInfo> runs;

  unordered_map<string,Float_t> zdc_NS_vec;

  TGraph* hJet6_bkg;
  TGraph* hJet8_bkg;
  TGraph* hJet10_bkg;
  TGraph* hJet12_bkg;

  TGraph* hJet6_mbdNS_bkg;
  TGraph* hJet8_mbdNS_bkg;
  TGraph* hJet10_mbdNS_bkg;
  TGraph* hJet12_mbdNS_bkg;
}

string myAnalysis::extractNumberFromLastToken(const string &filename) {
    size_t lastSlashPos = filename.rfind('/');
    string lastToken = filename.substr(lastSlashPos + 1);

    std::regex numberRegex(R"(\d+)");
    std::smatch match;

    if (std::regex_search(lastToken, match, numberRegex)) {
        return match[0];
    }

    return "";
}

void myAnalysis::init() {
  cout << "init" << endl;
  hJet6_bkg  = new TGraph();
  hJet8_bkg  = new TGraph();
  hJet10_bkg = new TGraph();
  hJet12_bkg = new TGraph();

  hJet6_mbdNS_bkg  = new TGraph();
  hJet8_mbdNS_bkg  = new TGraph();
  hJet10_mbdNS_bkg = new TGraph();
  hJet12_mbdNS_bkg = new TGraph();
}

Int_t myAnalysis::readZDCNS(const string &inputFile) {
  // Create an input stream
  std::ifstream file(inputFile);

  // Check if the file was successfully opened
  if (!file.is_open()) {
        cerr << "Failed to open cuts file: " << inputFile << endl;
        return 1;
  }

  string line;
  // skip header
  std::getline(file, line);
  while (std::getline(file, line))
  {
        std::istringstream lineStream(line);
        string cell;
        char comma;

        UInt_t run;
        Float_t zdc_NS;

        if (lineStream >> run >> comma >> zdc_NS)
        {
          zdc_NS_vec[to_string(run)] = zdc_NS;
        }
        else
        {
          cerr << "Failed to parse line: " << line << endl;
          return 1;
        }
  }

  // Close the file
  file.close();

  return 0;
}

Int_t myAnalysis::read(const string &inputFile) {
  std::ifstream file(inputFile);

  // Check if the file was successfully opened
  if (!file.is_open()) {
      cerr << "Failed to open file: " << inputFile << endl;
      return 1;
  }

  string line;

  // sets a global switch disabling the referencing
  // allows histograms to be used even after file is closed
  TH1::AddDirectory(kFALSE);
  // loop over each run
  while (std::getline(file, line)) {
      std::istringstream lineStream(line);

      string rootFile;
      RunInfo info;

      if (lineStream >> rootFile) {
          TFile f(rootFile.c_str());

          auto hEvents_Jet6  = ((TH1F*)f.Get("hEvents_Jet6"));
          auto hEvents_Jet8  = ((TH1F*)f.Get("hEvents_Jet8"));
          auto hEvents_Jet10 = ((TH1F*)f.Get("hEvents_Jet10"));
          auto hEvents_Jet12 = ((TH1F*)f.Get("hEvents_Jet12"));

          UInt_t evt_Jet6  = hEvents_Jet6->GetBinContent((Int_t)EventStatus::trigger+1);
          UInt_t evt_Jet8  = hEvents_Jet8->GetBinContent((Int_t)EventStatus::trigger+1);
          UInt_t evt_Jet10 = hEvents_Jet10->GetBinContent((Int_t)EventStatus::trigger+1);
          UInt_t evt_Jet12 = hEvents_Jet12->GetBinContent((Int_t)EventStatus::trigger+1);

          UInt_t evt_Jet6_mbdNS  = hEvents_Jet6->GetBinContent((Int_t)EventStatus::trigger_mbdNS+1);
          UInt_t evt_Jet8_mbdNS  = hEvents_Jet8->GetBinContent((Int_t)EventStatus::trigger_mbdNS+1);
          UInt_t evt_Jet10_mbdNS = hEvents_Jet10->GetBinContent((Int_t)EventStatus::trigger_mbdNS+1);
          UInt_t evt_Jet12_mbdNS = hEvents_Jet12->GetBinContent((Int_t)EventStatus::trigger_mbdNS+1);

          UInt_t evt_Jet6_bkg  = hEvents_Jet6->GetBinContent((Int_t)EventStatus::trigger_bkg+1);
          UInt_t evt_Jet8_bkg  = hEvents_Jet8->GetBinContent((Int_t)EventStatus::trigger_bkg+1);
          UInt_t evt_Jet10_bkg = hEvents_Jet10->GetBinContent((Int_t)EventStatus::trigger_bkg+1);
          UInt_t evt_Jet12_bkg = hEvents_Jet12->GetBinContent((Int_t)EventStatus::trigger_bkg+1);

          UInt_t evt_Jet6_mbdNS_bkg  = hEvents_Jet6->GetBinContent((Int_t)EventStatus::trigger_mbdNS_bkg+1);
          UInt_t evt_Jet8_mbdNS_bkg  = hEvents_Jet8->GetBinContent((Int_t)EventStatus::trigger_mbdNS_bkg+1);
          UInt_t evt_Jet10_mbdNS_bkg = hEvents_Jet10->GetBinContent((Int_t)EventStatus::trigger_mbdNS_bkg+1);
          UInt_t evt_Jet12_mbdNS_bkg = hEvents_Jet12->GetBinContent((Int_t)EventStatus::trigger_mbdNS_bkg+1);

          UInt_t evt_Jet6_bkgCEMC  = hEvents_Jet6->GetBinContent((Int_t)EventStatus::trigger_bkgCEMC+1);
          UInt_t evt_Jet8_bkgCEMC  = hEvents_Jet8->GetBinContent((Int_t)EventStatus::trigger_bkgCEMC+1);
          UInt_t evt_Jet10_bkgCEMC = hEvents_Jet10->GetBinContent((Int_t)EventStatus::trigger_bkgCEMC+1);
          UInt_t evt_Jet12_bkgCEMC = hEvents_Jet12->GetBinContent((Int_t)EventStatus::trigger_bkgCEMC+1);

          info.run = extractNumberFromLastToken(rootFile);

          info.evt_Jet6 = evt_Jet6;
          info.evt_Jet8 = evt_Jet8;
          info.evt_Jet10 = evt_Jet10;
          info.evt_Jet12 = evt_Jet12;

          info.evt_Jet6_mbdNS = evt_Jet6_mbdNS;
          info.evt_Jet8_mbdNS = evt_Jet8_mbdNS;
          info.evt_Jet10_mbdNS = evt_Jet10_mbdNS;
          info.evt_Jet12_mbdNS = evt_Jet12_mbdNS;

          info.evt_Jet6_bkg = evt_Jet6_bkg;
          info.evt_Jet8_bkg = evt_Jet8_bkg;
          info.evt_Jet10_bkg = evt_Jet10_bkg;
          info.evt_Jet12_bkg = evt_Jet12_bkg;

          info.evt_Jet6_mbdNS_bkg = evt_Jet6_mbdNS_bkg;
          info.evt_Jet8_mbdNS_bkg = evt_Jet8_mbdNS_bkg;
          info.evt_Jet10_mbdNS_bkg = evt_Jet10_mbdNS_bkg;
          info.evt_Jet12_mbdNS_bkg = evt_Jet12_mbdNS_bkg;

          info.evt_Jet6_bkgCEMC = evt_Jet6_bkgCEMC;
          info.evt_Jet8_bkgCEMC = evt_Jet8_bkgCEMC;
          info.evt_Jet10_bkgCEMC = evt_Jet10_bkgCEMC;
          info.evt_Jet12_bkgCEMC = evt_Jet12_bkgCEMC;

          vector<TH1*> hEvents_JetX;
          vector<TH1*> hEvents_JetX_bkg;
          for(auto s : triggerIdx) {
            string name = "time/hEvents_"+s+"_A";
            hEvents_JetX.push_back((TH1F*)f.Get(name.c_str()));

            name = "time/hEvents_"+s+"_bkg_A";
            hEvents_JetX_bkg.push_back((TH1F*)f.Get(name.c_str()));
          }

          info.hEvents_JetX     = hEvents_JetX;
          info.hEvents_JetX_bkg = hEvents_JetX_bkg;

          runs.push_back(info);

          cout << "File: " << rootFile << ", Run: " << info.run << endl;

          f.Close();
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

void myAnalysis::write(const string &outputFile) {
   ofstream output(outputFile);
   output << "run,Events_Jet6,Events_Jet6_mbdNS,Events_Jet6_bkg,Events_Jet6_mbdNS_bkg,Events_Jet6_bkgCEMC,Jet6_bkg_fraction,Jet6_mbdNS_bkg_fraction,Jet6_bkgCEMC_fraction"
             << ",Events_Jet8,Events_Jet8_mbdNS,Events_Jet8_bkg,Events_Jet8_mbdNS_bkg,Events_Jet8_bkgCEMC,Jet8_bkg_fraction,Jet8_mbdNS_bkg_fraction,Jet8_bkgCEMC_fraction"
             << ",Events_Jet10,Events_Jet10_mbdNS,Events_Jet10_bkg,Events_Jet10_mbdNS_bkg,Events_Jet10_bkgCEMC,Jet10_bkg_fraction,Jet10_mbdNS_bkg_fraction,Jet10_bkgCEMC_fraction"
             << ",Events_Jet12,Events_Jet12_mbdNS,Events_Jet12_bkg,Events_Jet12_mbdNS_bkg,Events_Jet12_bkgCEMC,Jet12_bkg_fraction,Jet12_mbdNS_bkg_fraction,Jet12_bkgCEMC_fraction" << endl;

   stringstream s;
   Float_t max_frac = 0;
   for (auto info : runs) {
      // ensure that the run has a corresponding zdc rate recorded
      if (!zdc_NS_vec.count(info.run)) continue;

      s.str("");
      Float_t fraction_Jet6_bkg = (info.evt_Jet6) ? (Int_t) (info.evt_Jet6_bkg * 1e4 / info.evt_Jet6) / 100. : 0;
      Float_t fraction_Jet8_bkg = (info.evt_Jet8) ? (Int_t) (info.evt_Jet8_bkg * 1e4 / info.evt_Jet8) / 100. : 0;
      Float_t fraction_Jet10_bkg = (info.evt_Jet10) ? (Int_t) (info.evt_Jet10_bkg * 1e4 / info.evt_Jet10) / 100. : 0;
      Float_t fraction_Jet12_bkg = (info.evt_Jet12) ? (Int_t) (info.evt_Jet12_bkg * 1e4 / info.evt_Jet12) / 100. : 0;

      Float_t fraction_Jet6_mbdNS_bkg = (info.evt_Jet6_mbdNS) ? (Int_t) (info.evt_Jet6_mbdNS_bkg * 1e4 / info.evt_Jet6_mbdNS) / 100. : 0;
      Float_t fraction_Jet8_mbdNS_bkg = (info.evt_Jet8_mbdNS) ? (Int_t) (info.evt_Jet8_mbdNS_bkg * 1e4 / info.evt_Jet8_mbdNS) / 100. : 0;
      Float_t fraction_Jet10_mbdNS_bkg = (info.evt_Jet10_mbdNS) ? (Int_t) (info.evt_Jet10_mbdNS_bkg * 1e4 / info.evt_Jet10_mbdNS) / 100. : 0;
      Float_t fraction_Jet12_mbdNS_bkg = (info.evt_Jet12_mbdNS) ? (Int_t) (info.evt_Jet12_mbdNS_bkg * 1e4 / info.evt_Jet12_mbdNS) / 100. : 0;

      Float_t fraction_bkgCEMC_Jet6 = (info.evt_Jet6) ? (Int_t) (info.evt_Jet6_bkgCEMC * 1e4 / info.evt_Jet6) / 100. : 0;
      Float_t fraction_bkgCEMC_Jet8 = (info.evt_Jet8) ? (Int_t) (info.evt_Jet8_bkgCEMC * 1e4 / info.evt_Jet8) / 100. : 0;
      Float_t fraction_bkgCEMC_Jet10 = (info.evt_Jet10) ? (Int_t) (info.evt_Jet10_bkgCEMC * 1e4 / info.evt_Jet10) / 100. : 0;
      Float_t fraction_bkgCEMC_Jet12 = (info.evt_Jet12) ? (Int_t) (info.evt_Jet12_bkgCEMC * 1e4 / info.evt_Jet12) / 100. : 0;

      max_frac = max(max_frac, fraction_Jet6_bkg);
      max_frac = max(max_frac, fraction_Jet8_bkg);
      max_frac = max(max_frac, fraction_Jet10_bkg);
      max_frac = max(max_frac, fraction_Jet12_bkg);

      max_frac = max(max_frac, fraction_Jet6_mbdNS_bkg);
      max_frac = max(max_frac, fraction_Jet8_mbdNS_bkg);
      max_frac = max(max_frac, fraction_Jet10_mbdNS_bkg);
      max_frac = max(max_frac, fraction_Jet12_mbdNS_bkg);

      hJet6_bkg->AddPoint(zdc_NS_vec[info.run],fraction_Jet6_bkg);
      hJet8_bkg->AddPoint(zdc_NS_vec[info.run], fraction_Jet8_bkg);
      hJet10_bkg->AddPoint(zdc_NS_vec[info.run], fraction_Jet10_bkg);
      hJet12_bkg->AddPoint(zdc_NS_vec[info.run], fraction_Jet12_bkg);

      hJet6_mbdNS_bkg->AddPoint(zdc_NS_vec[info.run], fraction_Jet6_mbdNS_bkg);
      hJet8_mbdNS_bkg->AddPoint(zdc_NS_vec[info.run], fraction_Jet8_mbdNS_bkg);
      hJet10_mbdNS_bkg->AddPoint(zdc_NS_vec[info.run], fraction_Jet10_mbdNS_bkg);
      hJet12_mbdNS_bkg->AddPoint(zdc_NS_vec[info.run], fraction_Jet12_mbdNS_bkg);

      // print outlier
      if(zdc_NS_vec[info.run] < 3) cout << "Run: " << info.run << ", ZDC NS: " << zdc_NS_vec[info.run] << ", frac: " << fraction_Jet6_bkg << endl;

      if(zdc_NS_vec[info.run] >= 3 && fraction_Jet12_bkg > 15) cout << "Run: " << info.run << ", ZDC NS: " << zdc_NS_vec[info.run] << ", frac: " << fraction_Jet12_bkg << endl;

      s << info.run << "," << info.evt_Jet6 << "," << info.evt_Jet6_mbdNS << "," << info.evt_Jet6_bkg << "," << info.evt_Jet6_mbdNS_bkg << "," << info.evt_Jet6_bkgCEMC << "," << fraction_Jet6_bkg << "," << fraction_Jet6_mbdNS_bkg << "," << fraction_bkgCEMC_Jet6
        << "," << info.evt_Jet8 << "," << info.evt_Jet8_mbdNS << "," << info.evt_Jet8_bkg << "," << info.evt_Jet8_mbdNS_bkg << "," << info.evt_Jet8_bkgCEMC << "," << fraction_Jet8_bkg << "," << fraction_Jet8_mbdNS_bkg << "," << fraction_bkgCEMC_Jet8
        << "," << info.evt_Jet10 << "," << info.evt_Jet10_mbdNS << "," << info.evt_Jet10_bkg << "," << info.evt_Jet10_mbdNS_bkg << "," << info.evt_Jet10_bkgCEMC << "," << fraction_Jet10_bkg << "," << fraction_Jet10_mbdNS_bkg << "," << fraction_bkgCEMC_Jet10
        << "," << info.evt_Jet12 << "," << info.evt_Jet12_mbdNS << "," << info.evt_Jet12_bkg << "," << info.evt_Jet12_mbdNS_bkg << "," << info.evt_Jet12_bkgCEMC << "," << fraction_Jet12_bkg << "," << fraction_Jet12_mbdNS_bkg << "," << fraction_bkgCEMC_Jet12 << endl;

      output << s.str();
   }

   cout << "max frac: " << max_frac << endl;

   output.close();
}

void myAnalysis::plot(const string &output) {
    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.08);
    c1->SetRightMargin(.02);
    c1->SetBottomMargin(.1);

    gPad->SetGrid(0,1);

    gStyle->SetTitleXOffset(0.9);
    gStyle->SetTitleYOffset(0.7);

    Float_t leg_x_low  = 0.15;
    Float_t leg_x_high = 0.25;
    Float_t leg_y_low  = 0.8;
    Float_t leg_y_high = 0.9;

    c1->Print((output + "[").c_str(), "pdf portrait");

    hJet6_mbdNS_bkg->SetMarkerColor(kRed);
    hJet8_mbdNS_bkg->SetMarkerColor(kRed);
    hJet10_mbdNS_bkg->SetMarkerColor(kRed);
    hJet12_mbdNS_bkg->SetMarkerColor(kRed);

    // gPad->SetLogy();
    hJet6_bkg->GetYaxis()->SetRangeUser(0,40);
    hJet6_bkg->GetXaxis()->SetLimits(3,9);
    hJet6_bkg->SetTitle("; #LT ZDC NS #GT [kHz]; Fraction of Background Events [%]");
    hJet6_bkg->Draw("AP");
    hJet6_mbdNS_bkg->Draw("same P");

    auto leg = new TLegend(leg_x_low, leg_y_low, leg_x_high, leg_y_high);

    leg->SetTextSize(0.05);
    leg->SetFillStyle(0);
    leg->AddEntry(hJet6_bkg, "Jet 6 GeV", "p");
    leg->AddEntry(hJet6_mbdNS_bkg, "Jet 6 GeV && MBD NS #geq 1", "p");
    leg->Draw("same");

    c1->Print("Jet6-bkg-vs-zdc-v1.png");
    c1->Print((output).c_str(), "pdf portrait");

    hJet6_bkg->GetYaxis()->SetRangeUser(0,12);

    c1->Print("Jet6-bkg-vs-zdc-v2.png");
    c1->Print((output).c_str(), "pdf portrait");

    hJet8_bkg->GetYaxis()->SetRangeUser(0,40);
    hJet8_bkg->GetXaxis()->SetLimits(3,9);
    hJet8_bkg->SetTitle("; #LT ZDC NS #GT [kHz]; Fraction of Background Events [%]");
    hJet8_bkg->Draw("AP");
    hJet8_mbdNS_bkg->Draw("same P");

    leg = new TLegend(leg_x_low, leg_y_low, leg_x_high, leg_y_high);

    leg->SetTextSize(0.05);
    leg->SetFillStyle(0);
    leg->AddEntry(hJet8_bkg, "Jet 8 GeV", "p");
    leg->AddEntry(hJet8_mbdNS_bkg, "Jet 8 GeV && MBD NS #geq 1", "p");
    leg->Draw("same");

    c1->Print("Jet8-bkg-vs-zdc-v1.png");
    c1->Print((output).c_str(), "pdf portrait");

    hJet8_bkg->GetYaxis()->SetRangeUser(0,25);
    c1->Print("Jet8-bkg-vs-zdc-v2.png");
    c1->Print((output).c_str(), "pdf portrait");

    hJet10_bkg->GetYaxis()->SetRangeUser(0,40);
    hJet10_bkg->GetXaxis()->SetLimits(3,9);
    hJet10_bkg->SetTitle("; #LT ZDC NS #GT [kHz]; Fraction of Background Events [%]");
    hJet10_bkg->Draw("AP");
    hJet10_mbdNS_bkg->Draw("same P");

    leg = new TLegend(leg_x_low, leg_y_low, leg_x_high, leg_y_high);

    leg->SetTextSize(0.05);
    leg->SetFillStyle(0);
    leg->AddEntry(hJet10_bkg, "Jet 10 GeV", "p");
    leg->AddEntry(hJet10_mbdNS_bkg, "Jet 10 GeV && MBD NS #geq 1", "p");
    leg->Draw("same");

    c1->Print("Jet10-bkg-vs-zdc-v1.png");
    c1->Print((output).c_str(), "pdf portrait");

    hJet12_bkg->GetYaxis()->SetRangeUser(0,40);
    hJet12_bkg->GetXaxis()->SetLimits(3,9);
    hJet12_bkg->SetTitle("; #LT ZDC NS #GT [kHz]; Fraction of Background Events [%]");
    hJet12_bkg->Draw("AP");
    hJet12_mbdNS_bkg->Draw("same P");

    leg = new TLegend(leg_x_low, leg_y_low, leg_x_high, leg_y_high);

    leg->SetTextSize(0.05);
    leg->SetFillStyle(0);
    leg->AddEntry(hJet12_bkg, "Jet 12 GeV", "p");
    leg->AddEntry(hJet12_mbdNS_bkg, "Jet 12 GeV && MBD NS #geq 1", "p");
    leg->Draw("same");

    c1->Print("Jet12-bkg-vs-zdc-v1.png");
    c1->Print((output).c_str(), "pdf portrait");

    c1->Print((output + "]").c_str(), "pdf portrait");
}

void event_stats(const string &inputFile, const string &inputZDCNSFile, const string &outputFile="stats.csv", const string &outputPlotFile="stats.pdf") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << inputFile << endl;
    cout << "input ZDC: " << inputZDCNSFile << endl;
    cout << "output: " << outputFile << endl;
    cout << "output plots: " << outputPlotFile << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    if(myAnalysis::read(inputFile)) return;
    if(myAnalysis::readZDCNS(inputZDCNSFile)) return;
    myAnalysis::init();
    myAnalysis::write(outputFile);
    myAnalysis::plot(outputPlotFile);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 3 || argc > 5){
        cout << "usage: ./event-stats inputFile inputZDCNSFile [outputFile] [outputPlotFile]" << endl;
        cout << "inputFile: input list file" << endl;
        cout << "inputZDCNSFile: input csv file" << endl;
        cout << "outputFile: output csv file" << endl;
        cout << "outputPlotFile: output plot file" << endl;
        return 1;
    }

    string outputFile     = "stats.csv";
    string outputPlotFile = "stats.pdf";

    if(argc >= 4) {
        outputFile = argv[2];
    }
    if(argc >= 5) {
        outputPlotFile = argv[3];
    }

    event_stats(argv[1], argv[2], outputFile, outputPlotFile);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
