// -- c++ includes --
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

// -- root includes --
#include <TFile.h>
#include <TProfile2D.h>
#include <TLatex.h>

#include <calobase/TowerInfoDefs.h>
#include <cdbobjects/CDBTTree.h>

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

namespace myAnalysis
{

  void histToCaloCDBTree(string outputfile, string fieldName, Int_t icalo, TProfile2D *hist);
  void analyze(const string &output);

  // utils
  pair<string, string> getRunDataset(const string &input);
  Int_t readHists(const string &input);

  pair<string, string> run_dataset;

  TProfile2D *h_CaloValid_cemc_etaphi_badChi2 = nullptr;
  TProfile2D *h_CaloValid_ihcal_etaphi_badChi2 = nullptr;
  TProfile2D *h_CaloValid_ohcal_etaphi_badChi2 = nullptr;

  TProfile2D *h_CaloValid_cemc_etaphi_time_raw = nullptr;
  TProfile2D *h_CaloValid_ihcal_etaphi_time_raw = nullptr;
  TProfile2D *h_CaloValid_ohcal_etaphi_time_raw = nullptr;

  Int_t cemc_bins_eta = 96;
  Int_t cemc_bins_phi = 256;
  Int_t hcal_bins_eta = 24;
  Int_t hcal_bins_phi = 64;
}  // namespace myAnalysis

pair<string, string> myAnalysis::getRunDataset(const string &input)
{
  string basename = std::filesystem::path(input).filename();
  string run = basename.substr(0, basename.find("_"));
  string dataset = basename.substr(basename.find("_") + 1, basename.size() - basename.find("_") - 6);
  return make_pair(run, dataset);
}

Int_t myAnalysis::readHists(const string &input)
{
  // Create an input stream
  std::ifstream file(input);

  // Check if the file was successfully opened
  if (!file.is_open())
  {
    cerr << "Failed to open file list: " << input << endl;
    return 1;
  }

  run_dataset = getRunDataset(input);

  cout << "Reading Hists" << endl;
  cout << "======================================" << endl;

  delete h_CaloValid_cemc_etaphi_badChi2;
  delete h_CaloValid_ihcal_etaphi_badChi2;
  delete h_CaloValid_ohcal_etaphi_badChi2;

  delete h_CaloValid_cemc_etaphi_time_raw;
  delete h_CaloValid_ihcal_etaphi_time_raw;
  delete h_CaloValid_ohcal_etaphi_time_raw;

  h_CaloValid_cemc_etaphi_badChi2 = new TProfile2D("cemc_etaphi_badChi2","", cemc_bins_eta, 0, cemc_bins_eta, cemc_bins_phi, 0, cemc_bins_phi);
  h_CaloValid_ihcal_etaphi_badChi2 = new TProfile2D("ihcal_etaphi_badChi2","", hcal_bins_eta, 0, hcal_bins_eta, hcal_bins_phi, 0, hcal_bins_phi);
  h_CaloValid_ohcal_etaphi_badChi2 = new TProfile2D("ohcal_etaphi_badChi2","", hcal_bins_eta, 0, hcal_bins_eta, hcal_bins_phi, 0, hcal_bins_phi);

  h_CaloValid_cemc_etaphi_time_raw = new TProfile2D("cemc_etaphi_time_raw","", cemc_bins_eta, 0, cemc_bins_eta, cemc_bins_phi, 0, cemc_bins_phi);
  h_CaloValid_ihcal_etaphi_time_raw = new TProfile2D("ihcal_etaphi_time_raw","", hcal_bins_eta, 0, hcal_bins_eta, hcal_bins_phi, 0, hcal_bins_phi);
  h_CaloValid_ohcal_etaphi_time_raw = new TProfile2D("ohcal_etaphi_time_raw","", hcal_bins_eta, 0, hcal_bins_eta, hcal_bins_phi, 0, hcal_bins_phi);

  string line;
  while (std::getline(file, line))
  {
    cout << "Reading File: " << line << endl;
    auto tf = TFile::Open(line.c_str());
    auto h = static_cast<TProfile2D*>(tf->Get("h_CaloValid_cemc_etaphi_badChi2"));

    if (h) h_CaloValid_cemc_etaphi_badChi2->Add(h);

    h = static_cast<TProfile2D*>(tf->Get("h_CaloValid_ihcal_etaphi_badChi2"));

    if (h) h_CaloValid_ihcal_etaphi_badChi2->Add(h);

    h = static_cast<TProfile2D*>(tf->Get("h_CaloValid_ohcal_etaphi_badChi2"));

    if (h) h_CaloValid_ohcal_etaphi_badChi2->Add(h);

    h = static_cast<TProfile2D*>(tf->Get("h_CaloValid_cemc_etaphi_time_raw"));

    if (h) h_CaloValid_cemc_etaphi_time_raw->Add(h);

    h = static_cast<TProfile2D*>(tf->Get("h_CaloValid_ihcal_etaphi_time_raw"));

    if (h) h_CaloValid_ihcal_etaphi_time_raw->Add(h);

    h = static_cast<TProfile2D*>(tf->Get("h_CaloValid_ohcal_etaphi_time_raw"));

    if (h) h_CaloValid_ohcal_etaphi_time_raw->Add(h);

    tf->Close();
  }

  // Close the file
  file.close();

  return 0;
}

void myAnalysis::histToCaloCDBTree(string outputfile, string fieldName, Int_t icalo, TProfile2D *hist)
{
  UInt_t neta, nphi;

  if (icalo != 0 && icalo != 1) return;

  if (icalo == 0)
  {
    neta = 96;
    nphi = 256;
  }
  if (icalo == 1)
  {
    neta = 24;
    nphi = 64;
  }

  CDBTTree *cdbttree = new CDBTTree(outputfile);

  Double_t mean = 0;
  Int_t count = 0;

  for (UInt_t ie = 0; ie < neta; ie++)
  {
    for (UInt_t ip = 0; ip < nphi; ip++)
    {
      UInt_t key;
      if (icalo == 0) key = TowerInfoDefs::encode_emcal(ie, ip);
      if (icalo == 1) key = TowerInfoDefs::encode_hcal(ie, ip);
      Float_t val = static_cast<Float_t>(hist->GetBinContent(static_cast<Int_t>(ie) + 1, static_cast<Int_t>(ip) + 1));
      cdbttree->SetFloatValue(static_cast<Int_t>(key), fieldName, val);
      mean += val;
      count++;
    }
  }

  cout << "Writing " << outputfile.c_str() << "   with mean=" << mean / count << endl;
  cdbttree->Commit();
  cdbttree->WriteCDBTTree();
  // cdbttree->Print();
  delete cdbttree;
}

void myAnalysis::analyze(const string &output)
{
  stringstream t;

  t.str("");

  string run = run_dataset.first;
  string dataset = run_dataset.second;

  cout << "Processing: Run: " << run << ", Dataset: " << dataset << endl;

  t << output << "/" << run << "_" << dataset;

  std::filesystem::create_directories(t.str());

  string detector = "CEMC";
  // fracBadChi2
  string payloadName = t.str() + "/" + detector + "_hotTowers_fracBadChi2" + "_" + dataset + "_" + run + ".root";
  if (h_CaloValid_cemc_etaphi_badChi2) histToCaloCDBTree(payloadName, "fraction", 0, h_CaloValid_cemc_etaphi_badChi2);
  // time
  payloadName = t.str() + "/" + detector + "_meanTime" + "_" + dataset + "_" + run + ".root";
  if (h_CaloValid_cemc_etaphi_time_raw) histToCaloCDBTree(payloadName, "time", 0, h_CaloValid_cemc_etaphi_time_raw);

  detector = "HCALIN";
  // fracBadChi2
  payloadName = t.str() + "/" + detector + "_hotTowers_fracBadChi2" + "_" + dataset + "_" + run + ".root";
  if (h_CaloValid_ihcal_etaphi_badChi2) histToCaloCDBTree(payloadName, "fraction", 1, h_CaloValid_ihcal_etaphi_badChi2);
  // time
  payloadName = t.str() + "/" + detector + "_meanTime" + "_" + dataset + "_" + run + ".root";
  if (h_CaloValid_ihcal_etaphi_time_raw) histToCaloCDBTree(payloadName, "time", 1, h_CaloValid_ihcal_etaphi_time_raw);

  detector = "HCALOUT";
  // fracBadChi2
  payloadName = t.str() + "/" + detector + "_hotTowers_fracBadChi2" + "_" + dataset + "_" + run + ".root";
  if (h_CaloValid_ohcal_etaphi_badChi2) histToCaloCDBTree(payloadName, "fraction", 1, h_CaloValid_ohcal_etaphi_badChi2);
  // time
  payloadName = t.str() + "/" + detector + "_meanTime" + "_" + dataset + "_" + run + ".root";
  if (h_CaloValid_ohcal_etaphi_time_raw) histToCaloCDBTree(payloadName, "time", 1, h_CaloValid_ohcal_etaphi_time_raw);
}

void genStatus(const string &input, const string &output = "output")
{
  cout << "#############################" << endl;
  cout << "Run Parameters" << endl;
  cout << "input: " << input << endl;
  cout << "output: " << output << endl;
  cout << "#############################" << endl;

  // merges individal qa into one per run
  myAnalysis::readHists(input);
  myAnalysis::analyze(output);
}

#ifndef __CINT__
Int_t main(Int_t argc, const char* const argv[])
{
  if (argc < 2 || argc > 3)
  {
    cout << "usage: ./genStatus input [output]" << endl;
    cout << "input: input list" << endl;
    cout << "output: output directory" << endl;
    return 1;
  }

  string output = "output";

  if (argc >= 3)
  {
    output = argv[2];
  }

  genStatus(argv[1], output);

  cout << "======================================" << endl;
  cout << "done" << endl;
  return 0;
}
#endif
