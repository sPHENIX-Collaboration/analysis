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
using namespace std;



using std::cout;
using std::endl;
#endif


void EnergyGlance()
{
 gStyle->SetOptFit(0);
   gStyle->SetOptStat(0);

const int N = 8;

double x[N] ={-288, -288, -277, -260, -230, -205, -258, -300};
double y[N] = {-105, -177, -117, -165, -165, -138, -138, -188};

int XBinlow;
int XBinhigh;

int YBinlow;
int YBinhigh;

char hisname[512];

char Title[512];

double step = 5;

TFile *fin = new TFile("His3.root");

 TCanvas *c12 = new TCanvas("c12", "c12",0,0,800,600);
 c12->cd();


TH1D *h1 = new TH1D("h1","",50,4,12);

for(int i = 0; i < N ; i++)
{
cout << i << endl;



XBinlow = Energyhis->GetXaxis()->FindBin(x[i]-step);
XBinhigh = Energyhis->GetXaxis()->FindBin(x[i]+step);

YBinlow = Energyhis->GetXaxis()->FindBin(y[i]-step);
YBinhigh = Energyhis->GetXaxis()->FindBin(y[i]+step);

cout << "XBin = " << XBinlow << endl;

Energyhis->ProjectionZ("h1",XBinlow,XBinhigh,YBinlow,YBinhigh);




sprintf(Title,"Energy Spetrum at Location X = %d Y = %d",x[i],y[i]);

h1->SetTitle(Title);
h1->GetXaxis()->SetTitle("Energy (GeV)");
h1->GetYaxis()->SetTitle("Counts");

//return;

h1->Draw();



c12->Update();

sprintf(hisname,"Result/Spetrum%d.png",i);

c12->SaveAs(hisname);


}

}
