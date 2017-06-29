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



void DataAna2(){
 gStyle->SetOptFit(0);
   gStyle->SetOptStat(0);
	
TFile *fin = new TFile("Interpolated.root");

const int NBins = 10;
int bins = 5;
double step = 1;

const int All = 7;
double inteval = 12.5;
double xinitial = -188;
//double x[All];
double yinitial = -77;
double x;
char HistoName[512];
//Set Tower Vertical Lines//

double x1 = 205;
double x2 = 228;
double x3 = 253;
double x4 = 277;
double x5 = 301;
char outname[512];
/*
for(int i = 0; i < All; i ++)
{

	



}

*/

double Xmin = -300;
double Xmax = -170;

double Ymax = -60;
double Ymin = -245.0;

int binlow;

int binhigh;

double Center;

double xlow;
double xhigh;

double ylow;
double yhigh;
int ybinlow;
int ybinhigh;
double y;

int ycent;

double mean;

int YBins;

int N;

int Ybinmax;

int Ybinmin;

char Title[512];

if(0){

for(int j = 0; j < All; j++)
{
TCanvas *c22 = new TCanvas("c22", "c22",0,0,800,600);

c22->cd();

	TFile *fin = new TFile("Interpolated2.root");
	x = xinitial - inteval * j;


//cout << "x = " << x << endl;
//cout << "j  =  "  << j << endl;

Center = Inter->GetXaxis()->FindBin(x);



//double x[NBins];
//int xbin[NBins];

cout << "OK 0" << endl;

 xlow = x - bins * step;
 xhigh = x + bins * step;

 binlow = Inter->GetXaxis()->FindBin(xlow);

 binhigh = Inter->GetXaxis()->FindBin(xhigh);

 YBins = (Ymax - Ymin)/step;

cout << "binlow =  " << binlow << endl;
cout << "binhigh =  " << binhigh << endl;


TH1D *h1 = new TH1D("h1","",YBins,Ymin,Ymax);


Inter->ProjectionY("h1",binlow,binhigh);

//cout << "mean = " << Inter->GetMean() << endl;



//h1->Draw();


 Ybinmin = h1->GetXaxis()->FindBin(Ymin);


 Ybinmax = h1->GetXaxis()->FindBin(Ymax);
cout << " OK 1" << endl;

 N = (Ybinmax - Ybinmin)/NBins;
/*
for(int i = 0; i < N; i++)
{

	y = Ymin + (i+0.5) * NBins;

cout << "y = " <<  y << endl;
ylow = Ymin + i * NBins;

yhigh = Ymin + (i+1)*NBins;

ybinlow = h1->GetXaxis()->FindBin(ylow);
ybinhigh = h1->GetXaxis()->FindBin(yhigh);

ycent = h2->GetXaxis()->FindBin(y);


mean = h1->Integral(ybinlow,ybinhigh)/NBins/(ybinhigh - ybinlow);

cout << "Difference  = " << ybinhigh - ybinlow << endl;
cout << "y Bin = " << ycent << endl;
cout << "Average is = " << mean << endl;

h2->SetBinContent(ycent,mean);

}
*/


h1->Scale(0.1);
//h1->Draw();
//h2 = h1;
//h2->Draw();

sprintf(HistoName,"Result%d.root",j);

  TFile *fout = new TFile(HistoName,"RECREATE");
  h1->GetXaxis()->SetTitle("y (mm)");
  h1->GetYaxis()->SetTitle("Average Energy (GeV)");
int k = 7- (j+1)/2;
int l = 6- (j+1)/2;
int m = 7 - (j+2)/2;


if(j == 1 || j == 3 || j == 5) sprintf(Title,"10 X Bin Average Energy vs Vertical Position Tower Between %d and %d",k,l);

if(j == 0 || j == 2 || j == 4 || j == 6 ) sprintf(Title,"10 X Bin Average Energy vs Vertical Position Tower %d",m);

  h1->SetTitle(Title);

  h1->Write();



//return;

h1->Draw("same");

sprintf(outname,"Result/Energy vs 10-Bin Average Y %d.png",j);



c22->Update();

c22->SaveAs(outname);

//cout << " OK 2" << endl;
}

}
int HAll = All + 4;

for(int j = 0; j < HAll; j++)
{
TCanvas *c22 = new TCanvas("c22", "c22",0,0,800,600);

c22->cd();

	TFile *fin = new TFile("Interpolated2.root");
	y = yinitial - inteval * j;


//cout << "x = " << x << endl;
//cout << "j  =  "  << j << endl;

Center = Inter->GetXaxis()->FindBin(x);



//double x[NBins];
//int xbin[NBins];

//cout << "OK 0" << endl;

 ylow = y - bins * step;
 yhigh = y + bins * step;

ybinlow = Inter->GetYaxis()->FindBin(ylow);

 ybinhigh = Inter->GetYaxis()->FindBin(yhigh);



 YBins = (Ymax - Ymin)/step;

cout << "ybinlow =  " <<ybinlow << endl;
cout << "ybinhigh =  " << ybinhigh << endl;


TH1D *h1 = new TH1D("h1","",YBins,Ymin,Ymax);


Inter->ProjectionX("h1",ybinlow,ybinhigh);

//cout << "mean = " << Inter->GetMean() << endl;



//h1->Draw();


 Ybinmin = h1->GetXaxis()->FindBin(Ymin);


 Ybinmax = h1->GetXaxis()->FindBin(Ymax);
cout << " OK 1" << endl;

 N = (Ybinmax - Ybinmin)/NBins;
/*
for(int i = 0; i < N; i++)
{

	y = Ymin + (i+0.5) * NBins;

cout << "y = " <<  y << endl;
ylow = Ymin + i * NBins;

yhigh = Ymin + (i+1)*NBins;

ybinlow = h1->GetXaxis()->FindBin(ylow);
ybinhigh = h1->GetXaxis()->FindBin(yhigh);

ycent = h2->GetXaxis()->FindBin(y);


mean = h1->Integral(ybinlow,ybinhigh)/NBins/(ybinhigh - ybinlow);

cout << "Difference  = " << ybinhigh - ybinlow << endl;
cout << "y Bin = " << ycent << endl;
cout << "Average is = " << mean << endl;

h2->SetBinContent(ycent,mean);

}
*/


h1->Scale(0.1);
//h1->Draw();
//h2 = h1;
//h2->Draw();

sprintf(HistoName,"Result%d.root",j);

  TFile *fout = new TFile(HistoName,"RECREATE");
  h1->GetXaxis()->SetTitle("x (mm)");
  h1->GetYaxis()->SetTitle("Average Energy (GeV)");
int k = 7- (j+1)/2;
int l = 6- (j+1)/2;
int m = 7 - (j+2)/2;


if(j == 1 || j == 3 || j == 5 || j == 7 || j == 9) sprintf(Title,"10 X Bin Average Energy vs Horizontal Position Tower Between %d and %d",k,l);

if(j == 0 || j == 2 || j == 4 || j == 6 || j == 8 || j == 10 ) sprintf(Title,"10 X Bin Average Energy vs Horizontal Position Tower %d",m);

  h1->SetTitle(Title);

  h1->Write();



//return;

h1->Draw("same");

sprintf(outname,"Result2/Energy vs 10-Bin Average X %d.png",j);



c22->Update();

c22->SaveAs(outname);

cout << " OK 2" << endl;
}



}
