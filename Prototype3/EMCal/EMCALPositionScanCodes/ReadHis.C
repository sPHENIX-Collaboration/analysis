#include <cmath>
#include <TFile.h>
#include <TString.h>
#include <TLine.h>
#include <TTree.h>
#include <TLatex.h>
#include <TGraphErrors.h>
#include <cassert>
#include "SaveCanvas.C"
#include "SetOKStyle.C"
#include <iostream>
#include <fstream>
#include "TROOT.h"
#include "TH1.h"
#include "TTree.h"
using namespace std;



using std::cout;
using std::endl;
#endif




void ReadHis()
{

  gSystem->Load("libg4eval.so");
  gSystem->Load("libqa_modules.so");
  gSystem->Load("libPrototype3.so");
 gStyle->SetOptFit(0);
   gStyle->SetOptStat(0);

   double step = 1;

int Ini = 3543;
int Final = 3579;


double Xmin = 170;
double Xmax = 310.0;

double Ymin = 60;
double Ymax = 245.0;


/*

double Xmin = 150;
double Xmax = 310.0;

double Ymin = 20;
double Ymax = 200.0;
*/


double Energy;
double Energy_err;
double Width;
double Width_err;
double Center;
double Center_err;

double x;
double y;
double xhigh;
double yhigh;
int binxhigh;
int binyhigh;

int binx;
int biny;
int XBins = (Xmax - Xmin)/step+1;
int YBins = (Ymax - Ymin)/step+1;
int index = 0;

int indexmax = XBins * YBins;
char Filename[512];
char inputfile[512];

sprintf(inputfile,"Hisfiles130/HisAll.root");

//sprintf(inputfile,"His2.root");


TFile *fin = new TFile(inputfile);
TH2D *EnPo= new TH2D("EnPo","",XBins,Xmin,Xmax,YBins,Ymin,Ymax);


cout << "XBins = " << XBins << endl;
cout << "YBins = " << YBins << endl;

char hisname[512];

for(int i = 0; i < XBins; i++)
{

x = Xmin + step*i;

xhigh = Xmin + step*(i+1);


for(int j = 0; j < YBins; j++)
{

//	TCanvas *c1 = new TCanvas("c1", "c1",0,0,800,600);
	y = Ymin + step*j;
yhigh = Ymin + step*(j+1);


binxhigh = Energyhis->GetZaxis()->FindBin(xhigh);
binyhigh = Energyhis->GetYaxis()->FindBin(yhigh);



binx = Energyhis->GetZaxis()->FindBin(x);
biny = Energyhis->GetYaxis()->FindBin(y);

//binxhigh = binx +1;
//binyhigh = biny +1;


TH1D *h5= new TH1D("h5","",50,6,10);

 Energyhis->ProjectionX("h5",biny,binyhigh,binx,binxhigh);

// h5->GetXaxis()->SetRange(6,10);
 //h5->Rebin(2);

 //return;
//
/*
 h5->Fit("gaus");

c1->Update();
sprintf(hisname,"pngfiles/Plot%d-%d.png",i,j);

  c1->SaveAs(hisname);


Energy = gaus->GetParameter(0);
Energy_err = gaus->GetParError(0);
Center = gaus->GetParameter(1);
Center_err = gaus->GetParError(1);
Width = gaus->GetParameter(2);
Width_err = gaus->GetParError(2);
*/

 Center = h5->GetMean();

cout << "Mean =  "  << Center << endl;

EnPo->SetBinContent(binx,biny,Center);


index = index + 1;

}

}

TCanvas *c22 = new TCanvas("c22", "c22",0,0,800,600);

c22->cd();

EnPo->Draw();
	
	c22->Update();

  c22->SaveAs("EnPo.png");


sprintf(Filename,"EnergyPosition.root");

//sprintf(Filename,"EnergyPosition2.root");
  TFile *fout = new TFile(Filename,"RECREATE");
EnPo->Write();


}
