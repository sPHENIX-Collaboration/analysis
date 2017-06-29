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




void ReadHis2()
{

int Method = 0;


  gSystem->Load("libg4eval.so");
  gSystem->Load("libqa_modules.so");
  gSystem->Load("libPrototype3.so");
 gStyle->SetOptFit(0);
   gStyle->SetOptStat(0);

   double step = 1;

// Fitting: Method = 1; Mean: Method = 0//
   
   
   int Ini = 3543;
int Final = 3579;


double Xmin = 170;
double Xmax = 310.0;

double Ymin = 60;
double Ymax = 245.0;


/*
int Xmin = 170;
int Xmax = 340.0;

int Ymin = 60;
int Ymax = 245.0;
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
int XBins = (Xmax - Xmin)/step;
int YBins = (Ymax - Ymin)/step;

//int XBins = 200;
//int YBins = 200;
char Name[512];

int index = 0;
double average;
int indexmax = XBins * YBins;
char Filename[512];
char inputfile[512];

//sprintf(inputfile,"Hisfiles130/HisAll.root");

sprintf(inputfile,"His2.root");
//sprintf(inputfile,"His3.root");

TFile *fin = new TFile(inputfile);
TH2D *EnPo= new TH2D("EnPo","",XBins,Xmin,Xmax,YBins,Ymin,Ymax);

TF1 *f1 = new TF1("f1","gaus",6,11);
cout << "XBins = " << XBins << endl;
cout << "YBins = " << YBins << endl;

char hisname[512];

for(int i = 0; i < XBins; i++)
{

x = Xmin + step*i;

xhigh = Xmin + step*(i+1);


for(int j = 0; j < YBins; j++)
{

TCanvas *c1 = new TCanvas("c1", "c1",0,0,800,600);
	y = Ymin + step*j;
yhigh = Ymin + step*(j+1);

//etTitle("Total Energy vs Position by Fitting");


binx = Energyhis->GetXaxis()->FindBin(x+0.0001);
biny = Energyhis->GetYaxis()->FindBin(y+0.0001);

//binxhigh = binx +1;
//binyhigh = biny +1;



binxhigh = Energyhis->GetXaxis()->FindBin(xhigh-0.0001);
binyhigh = Energyhis->GetYaxis()->FindBin(yhigh-0.0001);


TH1D *h5= new TH1D("h5","",25,4,10);




h5 = Energyhis->ProjectionZ("h5",binx,binxhigh,biny,binyhigh);

//return;



//return;
//

//Fitting Method //
if(Method ==1){

//h5->Rebin(4);


average = h5->GetMean();

if(average > 1) 
{

 h5->Fit(f1,"R");

c1->Update();



Energy = f1->GetParameter(0);
Energy_err = f1->GetParError(0);
Center = f1->GetParameter(1);
Center_err = f1->GetParError(1);
Width = f1->GetParameter(2);
Width_err = f1->GetParError(2);


// Center = h5->GetMean();

// cout << "x = " <<  x << endl;

//cout << "y = " <<  y << endl;

if(Center > 11 || Center < 4){

	cout << "Old Center = " << Center << endl;


Center = h5->GetMean();

cout << "New Center = " << Center << endl;
}



if(5.5 < Center < 6.5){
h5->Draw();

 h5->Fit(f1,"R");

sprintf(Name,"EMCAL -10 Degree Energy Distribution in x = %d mm and y = %d mm ",x,y)
h5->GetXaxis()->SetTitle("Energy (GeV)");
h5->GetYaxis()->SetTitle("Counts");
h5->SetTitle(Name);
c1->Update();


sprintf(hisname,"pngfiles/Plot%d-%d.png",x,y);

  c1->SaveAs(hisname);
}

}


if(average < 1) Center = 0; 


}



//Direct Mean Method//

if(Method == 0) Center = h5->GetMean();


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


//sprintf(Filename,"EnergyPosition.root");

sprintf(Filename,"EnergyPosition2.root");
//sprintf(Filename,"EnergyPosition3.root");

  TFile *fout = new TFile(Filename,"RECREATE");
EnPo->Write();


}
