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


void FillEnergy()
{
  gSystem->Load("libg4eval.so");
  gSystem->Load("libqa_modules.so");
  gSystem->Load("libPrototype3.so");
 gStyle->SetOptFit(0);
   gStyle->SetOptStat(0);

double inteval = 1;

double Xmin = 170;
double Xmax = 310.0;

double Ymin = 60;
double Ymax = 245.0;

int XBins =(Xmax - Xmin)/inteval;
int YBins = (Ymax - Ymin)/inteval;



int Ini = 3543;
int Final = 3579;

 int step = 1;

int num = (Final - Ini)/step;

cout << "num = " << num << endl;

const int N = 8;

const int NTotal = N * N;

double x;
	double y;
char Filename[512];
/*

double Horizontal[N];
double Vertical[N];
double Energy[N];

*/
char inputfile[512];

int j;

double xmax = 0;
double ymax = 0;

double Energy;
double Energy_err;
double Width;
double Width_err;
double Center;
double Center_err;

int index;


//

for(int i =0; i < num; i ++)
{
index = 0;

 gStyle->SetOptFit(0);
   gStyle->SetOptStat(0);

TCanvas *c1 = new TCanvas("c1", "c1",0,0,800,600);
c1->cd();


cout << " i = " << i << endl; 



j = i * step + Ini;

cout << " j = " << j << endl; 

if(j != 3416 && j != 3418 && j != 3420 && j != 3421 && j != 3426)
{

//sprintf(inputfile,"/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2017/Production_0128_WithHCalCalib/beam_0000%d-0000_DSTReader.root",j);

sprintf(inputfile,"/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2017/Production_0130_WithEMCalCalib/beam_0000%d-0000_DSTReader.root",j);


cout << "Infile name = " << inputfile << endl;

ifstream ifile(inputfile);



if(ifile){

TFile *fin = new TFile(inputfile);



TTree *t = (TTree *)fin->Get("T");

  t->SetAlias("C2_Inner_e", "1*abs(TOWER_RAW_C2[2].energy)");
  t->SetAlias("C2_Outer_e", "1*abs(TOWER_RAW_C2[3].energy)");
t->SetAlias("Average_column", "Sum$(TOWER_CALIB_CEMC.get_column() * TOWER_CALIB_CEMC.get_energy())/Sum$(TOWER_CALIB_CEMC.get_energy())");
  t->SetAlias("Average_HODO_HORIZONTAL", "Sum$(TOWER_CALIB_HODO_HORIZONTAL.towerid * (abs(TOWER_CALIB_HODO_HORIZONTAL.energy)>30) * abs(TOWER_CALIB_HODO_HORIZONTAL.energy))/Sum$((abs(TOWER_CALIB_HODO_HORIZONTAL.energy)>30) * abs(TOWER_CALIB_HODO_HORIZONTAL.energy))");
  t->SetAlias("Valid_HODO_HORIZONTAL", "Sum$(abs(TOWER_CALIB_HODO_HORIZONTAL.energy)>30) > 0");
  t->SetAlias("No_Triger_VETO", "Sum$(abs(TOWER_RAW_TRIGGER_VETO.energy)>15)==0");
 t->SetAlias("Valid_HODO_VERTICAL", "Sum$(abs(TOWER_CALIB_HODO_VERTICAL.energy)>30) > 0");
  t->SetAlias("C2_Sum_e", "C2_Inner_e + C2_Outer_e");
  t->SetAlias("Average_HODO_VERTICAL","Sum$(TOWER_CALIB_HODO_VERTICAL.towerid * (abs(TOWER_CALIB_HODO_VERTICAL.energy)>30) * abs(TOWER_CALIB_HODO_VERTICAL.energy))/Sum$((abs(TOWER_CALIB_HODO_VERTICAL.energy)>30) * abs(TOWER_CALIB_HODO_VERTICAL.energy))");
//return;
  t->SetAlias("Energy_Sum_CEMC", "1*Sum$(TOWER_CALIB_CEMC.get_energy())");

//Cut on Events//

// TCut event_sel = "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && No_Triger_VETO && C2_Sum_e > 200 && fmod(Entry$,10)==0";
TCut event_sel = "Valid_HODO_HORIZONTAL && Valid_HODO_VERTICAL && No_Triger_VETO && C2_Sum_e > 200";

  t->Draw(">>EventList", event_sel);
 
  TEventList * elist = gDirectory->GetObjectChecked("EventList", "TEventList");
 
//cout << "Cut is " << event_sel.data() << endl;

  cout << elist->GetN() << " / " << T->GetEntriesFast() << " events selected" << endl;
 
  t->SetEventList(elist);


TH1D *h5= new TH1D("h5","",200, -1, 10);


/*
t->Draw("beam_2CH_mm >> h1");
t->Draw("beam_2CV_mm >> h2");


t->Draw("Average_column >> h5");

t->Draw("Average_HODO_HORIZONTAL>>h3");

t->Draw("Average_HODO_VERTICAL>>h4");


*/
sprintf(Filename,"Hisfiles130/His%d.root",i); 

TFile *fout = new TFile(Filename,"RECREATE");

  t->SetAlias("XPos", "beam_2CH_mm - 5* int( Average_HODO_HORIZONTAL + 0.5)");
  t->SetAlias("YPos", "beam_2CV_mm + 5* int(Average_HODO_VERTICAL + 0.5)");




//cout <<"Start the scan..."<<endl;
//t->Scan("XPos:YPos:Energy_Sum_CEMC");
TH3F *Energyhis = new TH3F("Energyhis","",200,0,20,YBins,Ymin,Ymax,XBins,Xmin,Xmax);

// t->Draw("XPos:YPos:Energy_Sum_CEMC>>Energyhis(200,0,20,200,69.7,191.0,200,212,302.0)");

 t->Draw("XPos:YPos:Energy_Sum_CEMC>>Energyhis");


Energyhis->Write();
//fout->Print();

/*

new TCanvas();
t->Draw("XPos>>hXPos");

new TCanvas();
t->Draw("YPos>>hYPos");

new TCanvas();
t->Draw("Energy_Sum_CEMC>>hEnergy_CEMC");

 fout->Write();
	fout->Print();
*/
//t->Project("Energyhis","XPos:YPos:Energy_Sum_CEMC");

//TF1 *f1 = new TF1("f1","[0]*TMath::Exp(-(x - [1] )*(x - [1] )/(2 * [2] * [2] ))",7,12);


//t->Draw("beam_2CV_mm >> h2");

//t->Draw("TOWER_CALIB_CEMC.energy >> h3");

//return;
//
//
/*
cout <<"n_eve = "<<n_eve<<", hist = "<<Energyhis->GetSum()<<endl;
gDirectory->Print();

TCanvas *c22 = new TCanvas("c22", "c22",0,0,800,600);

c22->cd();

Energyhis->Draw();
	
	c22->Update();

  c22->SaveAs("Energy.png");


*/


/*
  Energyhis->Fit("gaus");

c1->Update();

  c1->SaveAs("AVHis.png");


Energy = gaus->GetParameter(0);
Energy_err = gaus->GetParError(0);
Center = gaus->GetParameter(1);
Center_err = gaus->GetParError(1);
Width = gaus->GetParameter(2);
Width_err = gaus->GetParError(2);

*/
//cout << "Energy =  "  << Energy << endl;
/*
for(int j = 0; j < N; j++)
{	

for(int k = 0; k < N; k++)
{

x = h1->GetMean()-0.5*j;
y = h2->GetMean()-0.5*k;


//E[i] = h3->GetMean();

cout << "x = " <<  x << endl;

cout << "y = " <<  y << endl;


Energyhis->Fill(x,y,Energy);

index = index + 1;

cout << "index = " << index << endl; 

}

}
*/


}

if (!ifile) { 

	i = i + 1;

cout << "File  " << i << "does not exist" << endl; 

}


}

}

/*
TCanvas *c11 = new TCanvas("c11", "c11",0,0,800,600);
c11->cd();
   gStyle->SetEndErrorSize(0.01);
   c11->SetFillColor(10);
   c11->SetBorderMode(0);
   c11->SetBorderSize(2);
   c11->SetFrameFillColor(0);
   c11->SetFrameBorderMode(0);

   c11->SetGridx();
   c11->SetGridy();

   c11->SetLeftMargin(0.13);
   c11->SetBottomMargin(0.13);
   c11->SetTopMargin(0.02);
   c11->SetRightMargin(0.06);

   double x1 = 0;
   double x2 = xmax*1.2;
   double y1 = 0;
   double y2 = ymax*1.2;

   TH1D *d0 = new TH1D("d0","",100,x1,x2);
   d0->SetMinimum(y1);
   d0->SetMaximum(y2);
   d0->GetXaxis()->SetNdivisions(208);
   d0->GetXaxis()->SetTitle("x");
   d0->GetXaxis()->SetTitleOffset(0.9);
   d0->GetXaxis()->SetTitleSize(0.06);
   d0->GetXaxis()->SetLabelOffset(0.01);
   d0->GetXaxis()->SetLabelSize(0.045);
   d0->GetXaxis()->SetLabelFont(42);
   d0->GetXaxis()->SetTitleFont(42);
   d0->GetYaxis()->SetNdivisions(205);
   d0->GetYaxis()->SetTitle("Energy");
   d0->GetYaxis()->SetTitleOffset(1.0);
   d0->GetYaxis()->SetTitleSize(0.06);
   d0->GetYaxis()->SetLabelOffset(0.005);
   d0->GetYaxis()->SetLabelSize(0.045);
   d0->GetYaxis()->SetLabelFont(42);
   d0->GetYaxis()->SetTitleFont(42);
   d0->SetLineWidth(2);
   d0->Draw();

   TLine *l1 = new TLine(x1,y1,x2,y1);
   l1->SetLineWidth(3);
   l1->Draw("same");
   TLine *l2 = new TLine(x1,y2,x2,y2);
   l2->SetLineWidth(3);
   l2->Draw("same");
   TLine *l3 = new TLine(x1,y1,x1,y2);
   l3->SetLineWidth(3);
   l3->Draw("same");
   TLine *l4 = new TLine(x2,y1,x2,y2);
   l4->SetLineWidth(3);
   l4->Draw("same");

TGraph *gr = new TGraphErrors(N, x, y);




    gr->SetMarkerSize(1.1);
   gr->SetMarkerStyle(20);
   gr->SetLineWidth(2);
    gr->SetName("Energy vs x");

	gr->Draw("p");
	
	c11->Update();

  c11->SaveAs("AVCH.png");
*/



}
