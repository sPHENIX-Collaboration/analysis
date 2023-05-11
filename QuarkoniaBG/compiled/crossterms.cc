#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
//#include "TNtuple.h"
#include "TRandom3.h"
#include "TLorentzVector.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

using namespace std;

int crossterms(double, string, int);

double BFunction(double *x, double *p) {
  double b0 = p[0];
  double b1 = p[1];
  double b2 = p[2];
  double b3 = p[3];
  double bb0 = p[4];
  double bb1 = p[5];
  double bb2 = p[6];
  double bb3 = p[7];
  double eideff = p[8];
  double xx = x[0];
  double val=0.;
  if(xx<4.8) { val = 1.0e-10*eideff*b0*pow(xx,b3)*pow((1.+(xx/b1)*(xx/b1)),b2); } 
    else { val     = 1.0e-10*eideff*bb0*pow(xx,bb3)*pow((1.+(xx/bb1)*(xx/bb1)),bb2); }
  return val;
}
double CFunction(double *x, double *p) {
  double b0 = p[0];
  double b1 = p[1];
  double b2 = p[2];
  double b3 = p[3];
  double bb0 = p[4];
  double bb1 = p[5];
  double bb2 = p[6];
  double bb3 = p[7];
  double eideff = p[8];
  double xx = x[0];
  double val=0.;
  if(xx<2.5) { val = 1.0e-10*eideff*b0*pow(xx,b3)*pow((1.+(xx/b1)*(xx/b1)),b2); } 
    else { val     = 1.0e-10*eideff*bb0*pow(xx,bb3)*pow((1.+(xx/bb1)*(xx/bb1)),bb2); }
  return val;
}


// This program will generate invariant mass distribution from fake
// electrons (mis-identified charged hadrons) in 10B 0-10% central AuAu collisions

int main(int argc, char* argv[]) {
  double eideff = 0.7;
  string ofname="crossterms.root";
  int scalefactor  = 1;
  if(argc==1) cout << argv[0] << " is running with standard parameters..." << endl;
  //if(argc==2) {centr = atoi(argv[1]);}
  //if(argc==3) {centr = atoi(argv[1]); ofname=argv[2];}
  return crossterms(eideff, ofname, scalefactor);
}


int crossterms(double eideff, string ofname, int scalefactor) {

bool useoldhf=false;
TF1* fcharm;
TF1* fbottom;
double ptcut = 2.0;
double start = ptcut;
double stop = 20.0;

if(useoldhf) {
// OLD STUFF
// Functions below are dN/dPt per event for Au+Au events
// Charm/bottom functions are from ccbar.C and bbbar.C macros
  char ccharm[999];
  char cbottom[999];
  if(eideff==1.0) {
    sprintf(ccharm,"%f*0.030311*pow(x,2.071907)*pow((1.+(x/2.047743)*(x/2.047743)),-5.699277)",0.7);
    sprintf(cbottom,"%f*0.002977*pow(x,0.244158)*pow((1.+(x/3.453862)*(x/3.453862)),-5.024055)",0.7);
  } else {
    sprintf(ccharm,"%f*0.030311*pow(x,2.071907)*pow((1.+(x/2.047743)*(x/2.047743)),-5.699277)",eideff);
    sprintf(cbottom,"%f*0.002977*pow(x,0.244158)*pow((1.+(x/3.453862)*(x/3.453862)),-5.024055)",eideff);
  }
  fcharm =  new TF1("fcharm", ccharm, start,stop);
  fbottom = new TF1("fbottom",cbottom,start,stop);
}
// END OLD STUFF
else {

// Functions below are dN/dPt per event for Au+Au events
// Charm/bottom functions are from fitsingles.C macro
  double c0,c1,c2,c3,cc0,cc1,cc2,cc3;
  c0 = 4.27243e+09;
  c1 = 2.17776;
  c2 = -6.70255;
  c3 = -0.700129;
  cc0 = 553246;
  cc1 = 0.788563;
  cc2 = 14.975;
  cc3 = -36.3373;
  fcharm = new TF1("fcharm",CFunction,ptcut,stop,9);
  fcharm->SetParameters(c0,c1,c2,c3,cc0,cc1,cc2,cc3,eideff);

  double b0,b1,b2,b3,bb0,bb1,bb2,bb3;
  b0 = 5.98862e+07;
  b1 = 7.02853;
  b2 = -14.4721;
  b3 = -0.215377;
  bb0 = 5.47498e+09;
  bb1 = 1.50573;
  bb2 = 0.168816;
  bb3 = -6.8832;
  fbottom = new TF1("fbottom",BFunction,ptcut,stop,9);
  fbottom->SetParameters(b0,b1,b2,b3,bb0,bb1,bb2,bb3,eideff);
}

// fake electrons (hadron spectra corrected for rejection factor)
// from compare.C macro

string fake_str;
cout << "eID efficiency = " << eideff << endl;

if(eideff==0.9) {
// eid efficiency 90%
fake_str = "(941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*(0.157654+1.947663*pow(x,-2.581256))*(-0.003171+0.202062*pow(x,-2.413650)+0.000581*x)+((941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*sqrt(0.1396*0.1396+x*x)/sqrt(0.4937*0.4937+x*x)*0.290000)*(0.157654+1.947663*pow(x,-2.581256))*(-0.001784+0.277532*pow(x,-2.726740)+0.000220*x)+((941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*sqrt(0.1396*0.1396+x*x)/sqrt(0.9383*0.9383+x*x)*0.090000)*(1.013817*exp(-(x-2.413264)*(x-2.413264)/2./1.423838/1.423838)+0.157654)*(0.003047+0.717479*exp(x/-1.264460))";
}
else if(eideff==0.7) {
// eid efficiency 70%
fake_str = "(941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*(0.157654+1.947663*pow(x,-2.581256))*(-0.000471+0.107319*pow(x,-2.726070)+0.000152*x)+((941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*sqrt(0.1396*0.1396+x*x)/sqrt(0.4937*0.4937+x*x)*0.290000)*(0.157654+1.947663*pow(x,-2.581256))*(-0.001784+0.277532*pow(x,-2.726740)+0.000220*x)/2.+((941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*sqrt(0.1396*0.1396+x*x)/sqrt(0.9383*0.9383+x*x)*0.090000)*(1.013817*exp(-(x-2.413264)*(x-2.413264)/2./1.423838/1.423838)+0.157654)*(0.001085+0.522870*exp(x/-1.114070))";
}
else if(eideff==1.0) { // constant pion rejection factor = 90
cout << "Using constant pion rejection factor = 90." << endl;
fake_str  = "(941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*(0.157654+1.947663*pow(x,-2.581256))/90.";
}
else {cerr << "ERROR: eid efficiency must be 0.9, 0.7 or 1.0 (constant rejection factor = 90)" << endl;}

TF1* ffake =  new TF1("ffake",fake_str.c_str(),ptcut,stop);


double nc    = fcharm ->Integral(ptcut,stop);
double nb    = fbottom->Integral(ptcut,stop);
double nfake = ffake  ->Integral(ptcut,stop);
cout << "nfake = " << nfake << endl;
cout << "nc = " << nc << "     one event = " << 1./nc/nfake << " Au+Au events." << endl;
cout << "nb = " << nb << "     one event = " << 1./nb/nfake << " Au+Au events." << endl;
long int Ncharm = (long int) (10.0e+09*nc*nfake); 
long int Nbottom = (long int) (10.0e+09*nb*nfake); 
cout << " Number of events to generate for charm: " << Ncharm << endl;
cout << " Number of events to generate for bottom: " << Nbottom << endl;

//TRandom2* rnd = new TRandom2(52835623);
TRandom* rnd = new TRandom3();
rnd->SetSeed(0);
  
TFile* fout = new TFile(ofname.c_str(),"RECREATE");

const int nbins = 15;
double binlim[nbins+1];
for(int i=0; i<=nbins; i++) {binlim[i]=double(i);}

// last bin = integrated over pT
TH1D* hhmassc[nbins+1];
TH1D* hhmassb[nbins+1];
char hhname[999];
int nchan = 400;
for(int i=0; i<nbins+1; i++) {
  sprintf(hhname,"hhmassc_%d",i);
  hhmassc[i] = new TH1D(hhname,"",nchan,0.,20.);
  sprintf(hhname,"hhmassb_%d",i);
  hhmassb[i] = new TH1D(hhname,"",nchan,0.,20.);
  (hhmassc[i])->Sumw2();
  (hhmassb[i])->Sumw2();
}

double fscalefactor = double(scalefactor);


// Generate events for charm/fake pairs
cout << "Generating " << Ncharm*scalefactor << " charm/fake events..." << endl;


for(int i=0; i<Ncharm*scalefactor; i++) {

  if(i%500000==0) cout << "processing event # " << i << endl;

// fake electron
  double pt1 = ffake->GetRandom(); 
  double eta1 = rnd->Uniform(-1.0,1.0);
  double theta1 = 3.141592654/2. + (exp(2.*eta1)-1.)/(exp(2.*eta1)+1.);
  double phi1 = rnd->Uniform(-3.141592654,3.141592654);
  double px1 = pt1*cos(phi1);
  double py1 = pt1*sin(phi1);
  double pp1 = pt1/sin(theta1);
  double pz1 = pp1*cos(theta1);
  double ee1 = pp1;
  TLorentzVector vec1 = TLorentzVector(px1, py1, pz1, ee1);

// charm positron
  double pt2 = fcharm->GetRandom(); 
  double eta2 = rnd->Uniform(-1.0,1.0);
  double theta2 = 3.141592654/2. + (exp(2.*eta2)-1.)/(exp(2.*eta2)+1.);
  double phi2 = rnd->Uniform(-3.141592654,3.141592654);
  double px2 = pt2*cos(phi2);
  double py2 = pt2*sin(phi2);
  double pp2 = pt2/sin(theta2);
  double pz2 = pp2*cos(theta2);
  double ee2 = pp2;
  TLorentzVector vec2 = TLorentzVector(px2, py2, pz2, ee2);

  if(pt1<ptcut || pt2<ptcut) continue;

    TLorentzVector pair = vec1 + vec2;
    double invmass = pair.M();
    double ptpair = pair.Pt();

    (hhmassc[nbins])->Fill(invmass);
      int mybin = int(ptpair);
      if(mybin>=0 && mybin<nbins) { (hhmassc[mybin])->Fill(invmass); }

} // end loop over charm/fake events


// Generate events for bottom/fake pairs
cout << "Generating " << Nbottom*scalefactor << " bottom/fake events..." << endl;

for(int i=0; i<Nbottom*scalefactor; i++) {

  if(i%500000==0) cout << "processing event # " << i << endl;

// fake electron
  double pt1 = ffake->GetRandom();
  double eta1 = rnd->Uniform(-1.0,1.0);
  double theta1 = 3.141592654/2. + (exp(2.*eta1)-1.)/(exp(2.*eta1)+1.);
  double phi1 = rnd->Uniform(-3.141592654,3.141592654);
  double px1 = pt1*cos(phi1);
  double py1 = pt1*sin(phi1);
  double pp1 = pt1/sin(theta1);
  double pz1 = pp1*cos(theta1);
  double ee1 = pp1;
  TLorentzVector vec1 = TLorentzVector(px1, py1, pz1, ee1);

// bottom positron
  double pt2 = fbottom->GetRandom();
  double eta2 = rnd->Uniform(-1.0,1.0);
  double theta2 = 3.141592654/2. + (exp(2.*eta2)-1.)/(exp(2.*eta2)+1.);
  double phi2 = rnd->Uniform(-3.141592654,3.141592654);
  double px2 = pt2*cos(phi2);
  double py2 = pt2*sin(phi2);
  double pp2 = pt2/sin(theta2);
  double pz2 = pp2*cos(theta2);
  double ee2 = pp2;
  TLorentzVector vec2 = TLorentzVector(px2, py2, pz2, ee2);

  if(pt1<ptcut || pt2<ptcut) continue;

    TLorentzVector pair = vec1 + vec2;
    double invmass = pair.M();
    double ptpair = pair.Pt();

    (hhmassb[nbins])->Fill(invmass);
      int mybin = int(ptpair);
      if(mybin>=0 && mybin<nbins) { (hhmassb[mybin])->Fill(invmass); }

} // end loop over charm/fake events


  for(int i=0; i<nbins+1; i++) { (hhmassc[i])->Scale(1./fscalefactor); (hhmassb[i])->Scale(1./fscalefactor); }

  TH1D* hhfakehf[nbins+1];
  for(int i=0; i<nbins+1; i++) { 
    sprintf(hhname,"hhfakehf_%d",i);
    hhfakehf[i]  = (TH1D*)(hhmassc[i])->Clone(hhname);
    (hhfakehf[i])->Add(hhmassb[i]);
  }

  fout->Write();
  fout->Close();

  return 0;
}



