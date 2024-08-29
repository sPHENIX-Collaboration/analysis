// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

// -- root includes --
#include <TH1F.h>
#include <TFile.h>

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

namespace myAnalysis {
    Int_t read(const string &inputFile);
    void write(const string &outputFile);

    struct RunInfo {
        string run;
        UInt_t evt_Jet6;
        UInt_t evt_Jet8;
        UInt_t evt_Jet10;
        UInt_t evt_Jet12;
        UInt_t evt_Jet6_bkg;
        UInt_t evt_Jet8_bkg;
        UInt_t evt_Jet10_bkg;
        UInt_t evt_Jet12_bkg;
        UInt_t evt_Jet6_CEMC_bkg;
        UInt_t evt_Jet8_CEMC_bkg;
        UInt_t evt_Jet10_CEMC_bkg;
        UInt_t evt_Jet12_CEMC_bkg;
    };

    vector<RunInfo> runs;
}

Int_t myAnalysis::read(const string &inputFile) {
  std::ifstream file(inputFile);

  // Check if the file was successfully opened
  if (!file.is_open()) {
      cerr << "Failed to open file: " << inputFile << endl;
      return 1;
  }

  string line;

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

          UInt_t evt_Jet6  = hEvents_Jet6->GetBinContent(1);
          UInt_t evt_Jet8  = hEvents_Jet8->GetBinContent(1);
          UInt_t evt_Jet10 = hEvents_Jet10->GetBinContent(1);
          UInt_t evt_Jet12 = hEvents_Jet12->GetBinContent(1);

          UInt_t evt_Jet6_bkg  = hEvents_Jet6->GetBinContent(2);
          UInt_t evt_Jet8_bkg  = hEvents_Jet8->GetBinContent(2);
          UInt_t evt_Jet10_bkg = hEvents_Jet10->GetBinContent(2);
          UInt_t evt_Jet12_bkg = hEvents_Jet12->GetBinContent(2);

          UInt_t evt_Jet6_CEMC_bkg  = hEvents_Jet6->GetBinContent(3);
          UInt_t evt_Jet8_CEMC_bkg  = hEvents_Jet8->GetBinContent(3);
          UInt_t evt_Jet10_CEMC_bkg = hEvents_Jet10->GetBinContent(3);
          UInt_t evt_Jet12_CEMC_bkg = hEvents_Jet12->GetBinContent(3);

          stringstream ss(rootFile);
          string token;

          UInt_t field = 0;
          string run;

          while (std::getline(ss, token, '/')) {
              if(field == 9) {
                  run = token;
                  break;
              }
              ++field;
          }

          info.run = run;
          info.evt_Jet6 = evt_Jet6;
          info.evt_Jet8 = evt_Jet8;
          info.evt_Jet10 = evt_Jet10;
          info.evt_Jet12 = evt_Jet12;

          info.evt_Jet6_bkg = evt_Jet6_bkg;
          info.evt_Jet8_bkg = evt_Jet8_bkg;
          info.evt_Jet10_bkg = evt_Jet10_bkg;
          info.evt_Jet12_bkg = evt_Jet12_bkg;

          info.evt_Jet6_CEMC_bkg = evt_Jet6_CEMC_bkg;
          info.evt_Jet8_CEMC_bkg = evt_Jet8_CEMC_bkg;
          info.evt_Jet10_CEMC_bkg = evt_Jet10_CEMC_bkg;
          info.evt_Jet12_CEMC_bkg = evt_Jet12_CEMC_bkg;

          runs.push_back(info);

          cout << "File: " << rootFile << ", Run: " << run << endl;

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
   output << "run,Events_Jet6,Events_Jet6_bkg,Events_Jet6_CEMC_bkg,Jet6_bkg_fraction,Jet6_CEMC_bkg_fraction"
             << ",Events_Jet8,Events_Jet8_bkg,Events_Jet8_CEMC_bkg,Jet8_bkg_fraction,Jet8_CEMC_bkg_fraction"
             << ",Events_Jet10,Events_Jet10_bkg,Events_Jet10_CEMC_bkg,Jet10_bkg_fraction,Jet10_CEMC_bkg_fraction"
             << ",Events_Jet12,Events_Jet12_bkg,Events_Jet12_CEMC_bkg,Jet12_bkg_fraction,Jet12_CEMC_bkg_fraction" << endl;

   stringstream s;
   for(auto info : runs) {
       s.str("");
       Float_t fraction_Jet6  = (info.evt_Jet6)  ? (Int_t)(info.evt_Jet6_bkg*1e4/info.evt_Jet6)/100.   : 0;
       Float_t fraction_Jet8  = (info.evt_Jet8)  ? (Int_t)(info.evt_Jet8_bkg*1e4/info.evt_Jet8)/100.   : 0;
       Float_t fraction_Jet10 = (info.evt_Jet10) ? (Int_t)(info.evt_Jet10_bkg*1e4/info.evt_Jet10)/100. : 0;
       Float_t fraction_Jet12 = (info.evt_Jet12) ? (Int_t)(info.evt_Jet12_bkg*1e4/info.evt_Jet12)/100. : 0;

       Float_t fraction_CEMC_Jet6  = (info.evt_Jet6)  ? (Int_t)(info.evt_Jet6_CEMC_bkg*1e4/info.evt_Jet6)/100.   : 0;
       Float_t fraction_CEMC_Jet8  = (info.evt_Jet8)  ? (Int_t)(info.evt_Jet8_CEMC_bkg*1e4/info.evt_Jet8)/100.   : 0;
       Float_t fraction_CEMC_Jet10 = (info.evt_Jet10) ? (Int_t)(info.evt_Jet10_CEMC_bkg*1e4/info.evt_Jet10)/100. : 0;
       Float_t fraction_CEMC_Jet12 = (info.evt_Jet12) ? (Int_t)(info.evt_Jet12_CEMC_bkg*1e4/info.evt_Jet12)/100. : 0;

       s << info.run << "," << info.evt_Jet6  << "," << info.evt_Jet6_bkg  << "," << info.evt_Jet6_CEMC_bkg  << "," << fraction_Jet6 << "," << fraction_CEMC_Jet6
                     << "," << info.evt_Jet8  << "," << info.evt_Jet8_bkg  << "," << info.evt_Jet8_CEMC_bkg  << "," << fraction_Jet8 << "," << fraction_CEMC_Jet8
                     << "," << info.evt_Jet10  << "," << info.evt_Jet10_bkg  << "," << info.evt_Jet10_CEMC_bkg  << "," << fraction_Jet10 << "," << fraction_CEMC_Jet10
                     << "," << info.evt_Jet12  << "," << info.evt_Jet12_bkg  << "," << info.evt_Jet12_CEMC_bkg  << "," << fraction_Jet12 << "," << fraction_CEMC_Jet12 << endl;

       output << s.str();
   }

   output.close();
}

void event_stats(const string &inputFile, const string &outputFile="stats.csv") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << inputFile << endl;
    cout << "output: " << outputFile << endl;
    cout << "#############################" << endl;

    if(myAnalysis::read(inputFile)) return;
    myAnalysis::write(outputFile);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 3){
        cout << "usage: ./event-stats inputFile [outputFile]" << endl;
        cout << "inputFile: input root file" << endl;
        cout << "outputFile: output csv file" << endl;
        return 1;
    }

    string outputFile  = "stats.csv";

    if(argc >= 3) {
        outputFile = argv[2];
    }

    event_stats(argv[1], outputFile);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
