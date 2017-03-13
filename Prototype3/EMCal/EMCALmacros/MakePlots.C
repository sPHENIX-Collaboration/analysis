#include <cmath>
#include <TFile.h>
#include <TString.h>
#include <TLine.h>
#include <TTree.h>
#include <TLatex.h>
#include <TGraphErrors.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include "TROOT.h"
#include "TH1.h"
#include "TTree.h"
using namespace std;



using std::cout;
using std::endl;
#endif


void MakePlots()
{

TFile *fin = new TFile("ACX203.root");

TCanvas *c1 = new TCanvas("c1", "c1",0,0,800,600);

c1->cd();

h7->Draw("colz");

c1->SaveAs("Average Column vs Horizontal.png");

TCanvas *c2 = new TCanvas("c2", "c1",0,0,800,600);

c2->cd();

h8->Draw("colz");

c2->SaveAs("Average Row vs Vertical.png");

















}
