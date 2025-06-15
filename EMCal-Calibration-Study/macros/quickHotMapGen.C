// -- c++ includes --
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <map>

// -- root includes --
#include <TFile.h>
// #include <TProfile2D.h>
// #include <TLatex.h>

// #include <calobase/TowerInfoDefs.h>
// #include <cdbobjects/CDBTTree.h>
#include <emcnoisytowerfinder/emcNoisyTowerFinder.h>

using std::cout;
using std::endl;
using std::string;
// using std::stringstream;
// using std::map;

// namespace fs = std::filesystem;

// namespace myAnalysis
// {

//   void analyze(const string &input, const string &output);

// }  // namespace myAnalysis

// void myAnalysis::analyze(const string &input, const string &output);
// {
// }

void quickHotMapGen(const string &input, const string &output = "test.root")
{
  cout << "#############################" << endl;
  cout << "Run Parameters" << endl;
  cout << "input: " << input << endl;
  cout << "output: " << output << endl;
  cout << "#############################" << endl;

  string s_input = input;
  string s_output = output;
  emcNoisyTowerFinder* calo = new emcNoisyTowerFinder();
  // calo->FindHot(input, output, "h_CaloValid_cemc_etaphi");
  calo->FindHot(s_input, s_output, "h_cemc_etaphi");
}

#ifndef __CINT__
Int_t main(Int_t argc, const char* const argv[])
{
  if (argc < 2 || argc > 3)
  {
    cout << "usage: ./quickHotMapGen input [output]" << endl;
    cout << "input: input list" << endl;
    cout << "output: output file" << endl;
    return 1;
  }

  string output = "test.root";

  if (argc >= 3)
  {
    output = argv[2];
  }

  quickHotMapGen(argv[1], output);

  cout << "======================================" << endl;
  cout << "done" << endl;
  return 0;
}
#endif
