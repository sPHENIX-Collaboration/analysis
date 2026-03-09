// -- c++ includes --
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

// -- root includes --
#include <TFile.h>
#include <TChain.h>

#include <calobase/TowerInfoDefs.h>

using std::cerr;
using std::cout;
using std::endl;
using std::max;
using std::min;
using std::ofstream;
using std::pair;
using std::string;
using std::stringstream;
using std::to_string;
using std::vector;

namespace fs = std::filesystem;

namespace myAnalysis
{
    void analyze();
    Int_t readFiles(const string &input);
    vector<string> files;
}  // namespace myAnalysis

Int_t myAnalysis::readFiles(const string &input)
{
  // Create an input stream
  std::ifstream file(input);

  // Check if the file was successfully opened
  if (!file.is_open())
  {
    cerr << "Failed to open file list: " << input << endl;
    return 1;
  }

  cout << "Reading Files" << endl;
  cout << "======================================" << endl;

  string line;
  while (std::getline(file, line))
  {
    cout << "Reading File: " << line << endl;
    files.push_back(line);
  }

  // Close the file
  file.close();

  return 0;
}

void myAnalysis::analyze()
{
    Int_t badFiles = 0;
    for(UInt_t i = 0; i < files.size(); ++i) {
        string file = files[i];
        cout << "Processing: " << fs::path(file).filename() << ", " << i*100./static_cast<Double_t>(files.size()) << " %" << endl;
        TChain* chain = new TChain("Multiple");
        chain->Add(file.c_str());

        Int_t id;
        Float_t time;

        chain->SetBranchAddress("IID", &id);
        chain->SetBranchAddress("Ftime", &time);

        Int_t ctr = 0;

        for(UInt_t j = 0; j < chain->GetEntries(); ++j) {
            chain->GetEntry(j);
            if(std::isfinite(time)) ++ctr;
            // cout << "id: " << id << ", time: " << time << endl;
        }

        if(ctr != chain->GetEntries()) ++badFiles;

        delete chain;
    }
    cout << "Bad Files: " << badFiles << endl;
}

void printStats(const string &input)
{
  cout << "#############################" << endl;
  cout << "Run Parameters" << endl;
  cout << "input: " << input << endl;
  cout << "#############################" << endl;

  myAnalysis::readFiles(input);
  myAnalysis::analyze();
}

#ifndef __CINT__
Int_t main(Int_t argc, const char* const argv[])
{
  if (argc < 2 || argc > 2)
  {
    cout << "usage: ./genStatus input" << endl;
    cout << "input: input list" << endl;
    return 1;
  }

  printStats(argv[1]);

  cout << "======================================" << endl;
  cout << "done" << endl;
  return 0;
}
#endif
