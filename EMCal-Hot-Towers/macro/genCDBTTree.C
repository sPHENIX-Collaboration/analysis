// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>

// -- root includes --
#include <TH2F.h>
#include <TFile.h>

// -- CDB TTree
#include <cdbobjects/CDBTTree.h>

// Tower stuff
#include <calobase/TowerInfoDefs.h>

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
    void analyze(const string& inputFile, const string &output);

    UInt_t ntowers = 24576;
    Float_t threshold; // nRuns above which tower is frequently bad
    string fieldName = "status";
}

void myAnalysis::analyze(const string& inputFile, const string &output) {
  TFile input(inputFile.c_str());

  auto hRunStatus = (TH1F *) input.Get("hRunStatus");

  Float_t norm = hRunStatus->GetBinContent(1);
  Float_t norm_chi2 = hRunStatus->GetBinContent(2);

  auto hBadTowersDead = (TH1F *) input.Get("hBadTowersDead");
  auto hBadTowersHot = (TH1F *) input.Get("hBadTowersHot");
  auto hBadTowersCold = (TH1F *) input.Get("hBadTowersCold");
  auto hBadTowersHotChi2 = (TH1F *) input.Get("hBadTowersHotChi2");

  auto cdbttree = new CDBTTree(output);

  if (norm != 0)
  {
    hBadTowersDead->Scale(1. / norm);
    hBadTowersHot->Scale(1. / norm);
    hBadTowersCold->Scale(1. / norm);
  }

  if (norm_chi2 != 0)
  {
    hBadTowersHotChi2->Scale(1. / norm_chi2);
  }

  UInt_t ctr[5] = {0};
  for (UInt_t i = 0; i < ntowers; ++i)
  {
    UInt_t key = TowerInfoDefs::encode_emcal(i);
    Int_t val = 0;
    if (hBadTowersDead->GetBinContent(i+1) >= threshold)
    {
      val = 1;
      ++ctr[1];
    }
    else if (hBadTowersHot->GetBinContent(i+1) >= threshold)
    {
      val = 2;
      ++ctr[2];
    }
    else if (hBadTowersCold->GetBinContent(i+1) >= threshold)
    {
      val = 3;
      ++ctr[3];
    }
    else if (hBadTowersHotChi2->GetBinContent(i+1) >= threshold)
    {
      val = 4;
      ++ctr[4];
    }
    else {
        ++ctr[0];
    }

    cdbttree->SetIntValue(key, fieldName, val);
  }

  cdbttree->Commit();
  cdbttree->WriteCDBTTree();

  delete cdbttree;

  cout << "=======" << endl;
  cout << "Stats" << endl;
  cout << "Good: "     << ctr[0] << ", " << ctr[0]*100./ntowers << " %" << endl;
  cout << "Dead: "     << ctr[1] << ", " << ctr[1]*100./ntowers << " %" << endl;
  cout << "Hot: "      << ctr[2] << ", " << ctr[2]*100./ntowers << " %" << endl;
  cout << "Cold: "     << ctr[3] << ", " << ctr[3]*100./ntowers << " %" << endl;
  cout << "Bad Chi2: " << ctr[4] << ", " << ctr[4]*100./ntowers << " %" << endl;

  input.Close();
}

void genCDBTTree(const string &input, const Float_t threshold = 0.5, const string &output="test.root") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "threshold: "  << threshold << endl;
    cout << "output: " << output << endl;
    cout << "#############################" << endl;

    myAnalysis::threshold = threshold;

    myAnalysis::analyze(input, output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 4){
        cout << "usage: ./genCDBTTree.C input [threshold] [output]" << endl;
        cout << "input: input root file" << endl;
        cout << "threshold: frequently bad threshold. Default: 0.5" << endl;
        cout << "output: output file. Default: test.root " << endl;
        return 1;
    }

    Float_t threshold = 0.5;
    string output = "test.root";

    if(argc >= 3) {
        threshold = atof(argv[2]);
    }
    if(argc >= 4) {
        output = argv[3];
    }

    genCDBTTree(argv[1], threshold, output);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
