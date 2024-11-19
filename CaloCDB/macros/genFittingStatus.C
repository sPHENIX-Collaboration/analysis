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
  void makeDir(const string &output);
  Int_t readHists(const string &input);

  pair<string, string> run_dataset;

  TProfile2D *h_CaloFittingQA_cemc_etaphi_ZScrosscalib = nullptr;
  TProfile2D *h_CaloFittingQA_ihcal_etaphi_ZScrosscalib = nullptr;
  TProfile2D *h_CaloFittingQA_ohcal_etaphi_ZScrosscalib = nullptr;

  UInt_t cemc_bins_eta = 96;
  UInt_t cemc_bins_phi = 256;
  UInt_t hcal_bins_eta = 24;
  UInt_t hcal_bins_phi = 64;
}  // namespace myAnalysis

void myAnalysis::makeDir(const string &output)
{
  if (std::filesystem::exists(output))
  {
    std::cout << "Directory '" << output << "' already exists." << std::endl;
  }
  else
  {
    try
    {
      std::filesystem::create_directory(output);
      std::cout << "Directory '" << output << "' created successfully." << std::endl;
    }
    catch (const std::filesystem::filesystem_error &e)
    {
      // Handle other potential errors
      std::cerr << "Error creating directory: " << e.what() << std::endl;
    }
  }
}

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

  delete h_CaloFittingQA_cemc_etaphi_ZScrosscalib;
  delete h_CaloFittingQA_ihcal_etaphi_ZScrosscalib;
  delete h_CaloFittingQA_ohcal_etaphi_ZScrosscalib;

  h_CaloFittingQA_cemc_etaphi_ZScrosscalib = new TProfile2D("cemc_etaphi_ZScrosscalib","", cemc_bins_eta, 0, cemc_bins_eta, cemc_bins_phi, 0, cemc_bins_phi);
  h_CaloFittingQA_ihcal_etaphi_ZScrosscalib = new TProfile2D("ihcal_etaphi_ZScrosscalib","", hcal_bins_eta, 0, hcal_bins_eta, hcal_bins_phi, 0, hcal_bins_phi);
  h_CaloFittingQA_ohcal_etaphi_ZScrosscalib = new TProfile2D("ohcal_etaphi_ZScrosscalib","", hcal_bins_eta, 0, hcal_bins_eta, hcal_bins_phi, 0, hcal_bins_phi);

  string line;
  while (std::getline(file, line))
  {
    cout << "Reading File: " << line << endl;
    auto tf = TFile::Open(line.c_str());

    auto h = (TProfile2D*) tf->Get("h_CaloFittingQA_cemc_etaphi_ZScrosscalib");

    if (h) h_CaloFittingQA_cemc_etaphi_ZScrosscalib->Add(h);

    h = (TProfile2D*) tf->Get("h_CaloFittingQA_ihcal_etaphi_ZScrosscalib");

    if (h) h_CaloFittingQA_ihcal_etaphi_ZScrosscalib->Add(h);

    h = (TProfile2D*) tf->Get("h_CaloFittingQA_ohcal_etaphi_ZScrosscalib");

    if (h) h_CaloFittingQA_ohcal_etaphi_ZScrosscalib->Add(h);

    tf->Close();
  }

  // Close the file
  file.close();

  return 0;
}

void myAnalysis::histToCaloCDBTree(string outputfile, string fieldName, Int_t icalo, TProfile2D *hist)
{
  Int_t neta, nphi;

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

  Float_t mean = 0;
  Int_t count = 0;

  for (Int_t ie = 0; ie < neta; ie++)
  {
    for (Int_t ip = 0; ip < nphi; ip++)
    {
      UInt_t key;
      if (icalo == 0) key = TowerInfoDefs::encode_emcal(ie, ip);
      if (icalo == 1) key = TowerInfoDefs::encode_hcal(ie, ip);
      Float_t val = hist->GetBinContent(ie + 1, ip + 1);
      if (val < 1.2 && val > 0.0) { // clean up of cross calib factors for towers with bit flip behavior 
        cdbttree->SetFloatValue(key, fieldName, val);
      } else {
        cdbttree->SetFloatValue(key, fieldName, 1.0);
      }
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

  makeDir(output);
  makeDir(t.str());

  string detector = "CEMC";
  // ZScrosscalib
  string payloadName = t.str() + "/" + detector + "_ZSCrossCalib" + "_" + dataset + "_" + run + ".root";
  if (h_CaloFittingQA_cemc_etaphi_ZScrosscalib) histToCaloCDBTree(payloadName, "ratio", 0, h_CaloFittingQA_cemc_etaphi_ZScrosscalib);

  detector = "HCALIN";
  // ZScrosscalib
  payloadName = t.str() + "/" + detector + "_ZSCrossCalib" + "_" + dataset + "_" + run + ".root";
  if (h_CaloFittingQA_ihcal_etaphi_ZScrosscalib) histToCaloCDBTree(payloadName, "ratio", 1, h_CaloFittingQA_ihcal_etaphi_ZScrosscalib);

  detector = "HCALOUT";
  // ZScrosscalib
  payloadName = t.str() + "/" + detector + "_ZSCrossCalib" + "_" + dataset + "_" + run + ".root";
  if (h_CaloFittingQA_ohcal_etaphi_ZScrosscalib) histToCaloCDBTree(payloadName, "ratio", 1, h_CaloFittingQA_ohcal_etaphi_ZScrosscalib);
}

void genFittingStatus(const string &input, const string &output = "output")
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
Int_t main(Int_t argc, char *argv[])
{
  if (argc < 2 || argc > 3)
  {
    cout << "usage: ./genFittingStatus input [output]" << endl;
    cout << "input: input list" << endl;
    cout << "output: output directory" << endl;
    return 1;
  }

  string output = "output";

  if (argc >= 3)
  {
    output = argv[2];
  }

  genFittingStatus(argv[1], output);

  cout << "======================================" << endl;
  cout << "done" << endl;
  return 0;
}
#endif
