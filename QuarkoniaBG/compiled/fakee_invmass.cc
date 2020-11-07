#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TMCParticle.h"
#include "TLorentzVector.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

using namespace std;

int fakee_invmass(double, string, long int);

// This program will generate invariant mass distribution from fake
// electrons (mis-identified charged hadrons) in central AuAu collisions
// and scales it up to 5B central events (50B min. bias)
// You can choose 0-10% (centr=0) or 10-20% (centr=1) central events
// You can choose three different pion rejection functions,
// or use constant (Pt-independent) pion rejection factor

int main(int argc, char* argv[]) {
  double eideff = 0.7;
  string ofname="fakee.root";
  long int stat=500000000;
  if(argc==1) cout << argv[0] << " is running with standard parameters..." << endl;
//  if(argc==2) {eideff = atoi(argv[1]);}
//  if(argc==3) {eideff = atoi(argv[1]); ofname=argv[2]; stat=atoi(argv[3]);}
  return fakee_invmass(eideff, ofname, stat);
}


int fakee_invmass(double eideff, string ofname, long int stat) {

int nstat = 20;

// This is dN/dpT for fake electrons from all sources (pions, kaons,protons, anti-protons)
//float start = 1.;
double start = 2.;
double stop = 20.0;
double ptcut=2.0;
string plusdistributionAuAu;
string minusdistributionAuAu;

if(eideff==0.9) {
cout << "eID efficiency = " << eideff << endl;
// eid efficiency 90%
plusdistributionAuAu  = "(941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*(0.157654+1.947663*pow(x,-2.581256))*(-0.003171+0.202062*pow(x,-2.413650)+0.000581*x)+((941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*sqrt(0.1396*0.1396+x*x)/sqrt(0.4937*0.4937+x*x)*0.290000)*(0.157654+1.947663*pow(x,-2.581256))*(-0.001784+0.277532*pow(x,-2.726740)+0.000220*x)+((941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*sqrt(0.1396*0.1396+x*x)/sqrt(0.9383*0.9383+x*x)*0.140000)*(1.013817*exp(-(x-2.413264)*(x-2.413264)/2./1.423838/1.423838)+0.157654)*(-0.001784+0.277532*pow(x,-2.726740)+0.000220*x)";
minusdistributionAuAu = "(941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*(0.157654+1.947663*pow(x,-2.581256))*(-0.003171+0.202062*pow(x,-2.413650)+0.000581*x)+((941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*sqrt(0.1396*0.1396+x*x)/sqrt(0.4937*0.4937+x*x)*0.290000)*(0.157654+1.947663*pow(x,-2.581256))*(-0.001784+0.277532*pow(x,-2.726740)+0.000220*x)+((941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*sqrt(0.1396*0.1396+x*x)/sqrt(0.9383*0.9383+x*x)*0.090000)*(1.013817*exp(-(x-2.413264)*(x-2.413264)/2./1.423838/1.423838)+0.157654)*(0.003047+0.717479*exp(x/-1.264460))";

} 
else if(eideff==0.7) {
cout << "eID efficiency = " << eideff << endl;
// eid efficnecy 70%
plusdistributionAuAu  = "(941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*(0.157654+1.947663*pow(x,-2.581256))*(-0.000471+0.107319*pow(x,-2.726070)+0.000152*x)+((941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*sqrt(0.1396*0.1396+x*x)/sqrt(0.4937*0.4937+x*x)*0.290000)*(0.157654+1.947663*pow(x,-2.581256))*(-0.001784+0.277532*pow(x,-2.726740)+0.000220*x)/2.+((941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*sqrt(0.1396*0.1396+x*x)/sqrt(0.9383*0.9383+x*x)*0.140000)*(1.013817*exp(-(x-2.413264)*(x-2.413264)/2./1.423838/1.423838)+0.157654)*(-0.000125+0.182736*pow(x,-3.662870)+0.000023*x)/2.";
minusdistributionAuAu = "(941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*(0.157654+1.947663*pow(x,-2.581256))*(-0.000471+0.107319*pow(x,-2.726070)+0.000152*x)+((941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*sqrt(0.1396*0.1396+x*x)/sqrt(0.4937*0.4937+x*x)*0.290000)*(0.157654+1.947663*pow(x,-2.581256))*(-0.001784+0.277532*pow(x,-2.726740)+0.000220*x)/2.+((941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*sqrt(0.1396*0.1396+x*x)/sqrt(0.9383*0.9383+x*x)*0.090000)*(1.013817*exp(-(x-2.413264)*(x-2.413264)/2./1.423838/1.423838)+0.157654)*(0.001085+0.522870*exp(x/-1.114070))";
}
else if(eideff==1.0) { // constant pion rejection factor = 90
cout << "Using constant pion rejection factor = 90." << endl;
plusdistributionAuAu  = "(941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*(0.157654+1.947663*pow(x,-2.581256))/90.";
minusdistributionAuAu  = "(941.256287*pow(x,-2.357330)*pow((1.+(x/1.455710)*(x/1.455710)),-3.022760)*x)*(0.157654+1.947663*pow(x,-2.581256))/90.";
}
else { cerr << "ERROR: eid efficiency must be 0.9, 0.7 or 1.0 (constant rejection factor = 90)" << endl; }

TF1* plusptdistrAuAu = new TF1("plusptdistrAuAu",plusdistributionAuAu.c_str(),start,stop);
double norm2plus = plusptdistrAuAu->Integral(start,stop);
cout << "Fake positron multiplicity above " << start << "GeV in Au+Au = " << norm2plus << endl;

TF1* minusptdistrAuAu = new TF1("minusptdistrAuAu",minusdistributionAuAu.c_str(),start,stop);
double norm2minus = minusptdistrAuAu->Integral(start,stop);
cout << "Fake electron multiplicity above " << start << "GeV in Au+Au = " << norm2minus << endl;



TRandom* rnd = new TRandom3();
rnd->SetSeed(0);
  
TFile* fout = new TFile(ofname.c_str(),"RECREATE");
int nchan = 400;
double s1=0.;
double s2=20.;
//TH1D* hmass  = new TH1D("hmass","", nchan,s1,s2);   
//TH1D* hmass0 = new TH1D("hmass0","",nchan,s1,s2);   
//TH1D* hmass1 = new TH1D("hmass1","",nchan,s1,s2);   
//TH1D* hmass2 = new TH1D("hmass2","",nchan,s1,s2);   
//TH1D* hmass3 = new TH1D("hmass3","",nchan,s1,s2);   
//TH1D* hmass_flatpt  = new TH1D("hmass_flatpt","", nchan,s1,s2);
//TH1D* hmass0_flatpt = new TH1D("hmass0_flatpt","",nchan,s1,s2);
//TH1D* hmass1_flatpt = new TH1D("hmass1_flatpt","",nchan,s1,s2);
//TH1D* hmass2_flatpt = new TH1D("hmass2_flatpt","",nchan,s1,s2);
//TH1D* hmass3_flatpt = new TH1D("hmass3_flatpt","",nchan,s1,s2);
TH1D* hmult1 = new TH1D("hmult1","",20,-0.5,19.5);
TH1D* hmult2 = new TH1D("hmult2","",20,-0.5,19.5);
TH1D* hmultpair = new TH1D("hmultpair","",20,-0.5,19.5);
//hmass->Sumw2();
//hmass0->Sumw2();
//hmass1->Sumw2();
//hmass2->Sumw2();
//hmass3->Sumw2();
//hmass_flatpt->Sumw2();
//hmass0_flatpt->Sumw2();
//hmass1_flatpt->Sumw2();
//hmass2_flatpt->Sumw2();
//hmass3_flatpt->Sumw2();

const int nbins = 15;
double binlim[nbins+1];
for(int i=0; i<=nbins; i++) {binlim[i]=double(i);}
//binlim[9] = 10.;
//binlim[10] = 15.;

TH1D* hhmass[nbins+1];
//TH1D* hhmass_flatpt[nbins+1];
char hhname[999];
for(int i=0; i<nbins+1; i++) {
  sprintf(hhname,"hhmass_%d",i);
  hhmass[i] = new TH1D(hhname,"",nchan,s1,s2);
//  sprintf(hhname,"hhmass_flatpt_%d",i);
//  hhmass_flatpt[i] = new TH1D(hhname,"",nchan,s1,s2);
  (hhmass[i])->Sumw2(); 
//  (hhmass_flatpt[i])->Sumw2(); 
}

// Generate events

for(int ii=0; ii<nstat; ii++) {
for(int i=0; i<stat; i++) {

  if(i%10000000==0) cout << "processing event # " << i << endl;

// first introduce 25% fluctuations in multiplicity
  float n1smeared = rnd->Gaus(norm2minus,norm2minus*0.25);
  float n2smeared = rnd->Gaus(norm2plus,norm2plus*0.25);
  if(n1smeared<=0. || n2smeared<=0.) continue;
//  float n1smeared = norm2minus;
//  float n2smeared = norm2plus;
// then calculate multiplicity for fake electrons and positrons
  int n1 = rnd->Poisson(n1smeared);
  int n2 = rnd->Poisson(n2smeared);
  hmult1->Fill(float(n1));
  hmult2->Fill(float(n2));
  hmultpair->Fill(float(n1*n2));
  if(n1<1 || n2<1) continue;

  // loop over pairs
  for(int i1=0; i1<n1; i1++) { 

/*
  double pt1 = rnd->Uniform(start,stop); // flat pT
  double eta1 = rnd->Uniform(-1.0,1.0);
  double theta1 = 3.141592654/2. + (exp(2.*eta1)-1.)/(exp(2.*eta1)+1.);
  double phi1 = rnd->Uniform(-3.141592654,3.141592654);
  double px1 = pt1*cos(phi1);
  double py1 = pt1*sin(phi1);
  double pp1 = pt1/sin(theta1);
  double pz1 = pp1*cos(theta1);
  double ee1 = pp1;
  TLorentzVector vec1 = TLorentzVector(px1, py1, pz1, ee1);
*/
    double pt11 = minusptdistrAuAu->GetRandom(); // correct pt used for cross-check of histogram normalization
    double eta11 = rnd->Uniform(-1.0,1.0);
    double theta11 = 3.141592654/2. + (exp(2.*eta11)-1.)/(exp(2.*eta11)+1.);
    double phi11 = rnd->Uniform(-3.141592654,3.141592654);
    double px11 = pt11*cos(phi11);
    double py11 = pt11*sin(phi11);
    double pp11 = pt11/sin(theta11);
    double pz11 = pp11*cos(theta11);
    double ee11 = pp11;
    TLorentzVector vec11 = TLorentzVector(px11, py11, pz11, ee11);

  for(int i2=0; i2<n2; i2++) {
/*
  double pt2 = rnd->Uniform(start,stop); // flat pT
  double eta2 = rnd->Uniform(-1.0,1.0);
  double theta2 = 3.141592654/2. + (exp(2.*eta2)-1.)/(exp(2.*eta2)+1.);
  double phi2 = rnd->Uniform(-3.141592654,3.141592654);
  double px2 = pt2*cos(phi2);
  double py2 = pt2*sin(phi2);
  double pp2 = pt2/sin(theta2);
  double pz2 = pp2*cos(theta2);
  double ee2 = pp2;
  TLorentzVector vec2 = TLorentzVector(px2, py2, pz2, ee2);
*/
    double pt22 = plusptdistrAuAu->GetRandom(); // correct pt used for cross-check of histogram normalization
    double eta22 = rnd->Uniform(-1.0,1.0);
    double theta22 = 3.141592654/2. + (exp(2.*eta22)-1.)/(exp(2.*eta22)+1.);
    double phi22 = rnd->Uniform(-3.141592654,3.141592654);
    double px22 = pt22*cos(phi22);
    double py22 = pt22*sin(phi22);
    double pp22 = pt22/sin(theta22);
    double pz22 = pp22*cos(theta22);
    double ee22 = pp22;
    TLorentzVector vec22 = TLorentzVector(px22, py22, pz22, ee22);

/*
    // flat pT
    if(pt1>ptcut && pt2>ptcut) {
      TLorentzVector upsilon = vec1 + vec2;  // flat pT
      double weight1 = minusptdistrAuAu->Eval(pt1)/norm2minus;  // weight normalized to 1.
      double weight2 =  plusptdistrAuAu->Eval(pt2)/norm2plus;
        (hhmass_flatpt[nbins])->Fill(upsilon.M(), weight1*weight2); 
        //if(upsilon.Pt()>0.0 && upsilon.Pt()<=2.0)  { hmass0_flatpt->Fill(upsilon.M(), weight1*weight2); }
        //if(upsilon.Pt()>2.0 && upsilon.Pt()<=4.0)  { hmass1_flatpt->Fill(upsilon.M(), weight1*weight2); }
        //if(upsilon.Pt()>4.0 && upsilon.Pt()<=6.0)  { hmass2_flatpt->Fill(upsilon.M(), weight1*weight2); }
        //if(upsilon.Pt()>6.0 && upsilon.Pt()<=10.0) { hmass3_flatpt->Fill(upsilon.M(), weight1*weight2); }
        int mybin = int(upsilon.Pt());
        //if(mybin==9) mybin = 8;
        //if(mybin>=11 && mybin<=14) mybin = 9;
        if(mybin>=0 && mybin<nbins) { (hhmass_flatpt[mybin])->Fill(upsilon.M(), weight1*weight2); }
    }
*/
//    // for proper normalization fill histograms with correct pt
    if(pt11>ptcut && pt22>ptcut) {
      TLorentzVector upsilon2 = vec11 + vec22;
        (hhmass[nbins])->Fill(upsilon2.M());
        //if(upsilon2.Pt()>0.0 && upsilon2.Pt()<=2.0)  { hmass0->Fill(upsilon2.M()); }
        //if(upsilon2.Pt()>2.0 && upsilon2.Pt()<=4.0)  { hmass1->Fill(upsilon2.M()); }
        //if(upsilon2.Pt()>4.0 && upsilon2.Pt()<=6.0)  { hmass2->Fill(upsilon2.M()); }
        //if(upsilon2.Pt()>6.0 && upsilon2.Pt()<=10.0) { hmass3->Fill(upsilon2.M()); }
        int mybin = int(upsilon2.Pt());
        //if(mybin==9) mybin = 8;
        //if(mybin>=11 && mybin<=14) mybin = 9;
        if(mybin>=0 && mybin<nbins) { (hhmass[mybin])->Fill(upsilon2.M()); }
    }

  }} // end loop over pairs

} // end loop over events
} // end second loop

/*
// normalize flat-pt histogram so that number of entries is the same as integral

for(int i=0; i<nbins+1; i++) {
  float nn = (hhmass[i])->Integral(1,nchan);
  float scerr = 1./sqrt(nn);
  float nn_flatpt = (hhmass_flatpt[i])->Integral(1,nchan);
  cout << "   Scale Factor = " << nn << " / " << nn_flatpt << " = " << nn/nn_flatpt << "      " << hhmass[i]->GetEntries() << endl;
  (hhmass_flatpt[i])->Scale(nn/nn_flatpt);
}

  float nn = hmass->Integral(1,nchan);   
  float scerr = 1./sqrt(nn);
  float nn_flatpt = hmass_flatpt->Integral(1,nchan);    
  cout << "Scale factor = " << nn << " / " << nn_flatpt << " = " << nn/nn_flatpt << "      " << hmass->GetEntries() << endl;
  hmass_flatpt->Scale(nn/nn_flatpt);

  nn = hmass0->Integral(1,nchan);
  float scerr0 = 1./sqrt(nn);
  nn_flatpt = hmass0_flatpt->Integral(1,nchan);
  cout << "Scale factor 0 = " << nn << " / " << nn_flatpt << " = " << nn/nn_flatpt << endl;
  hmass0_flatpt->Scale(nn/nn_flatpt);

  nn = hmass1->Integral(1,nchan);
  float scerr1 = 1./sqrt(nn);
  nn_flatpt = hmass1_flatpt->Integral(1,nchan);
  cout << "Scale factor 1 = " << nn << " / " << nn_flatpt << " = " << nn/nn_flatpt << endl;
  hmass1_flatpt->Scale(nn/nn_flatpt);

  nn = hmass2->Integral(1,nchan);
  float scerr2 = 1./sqrt(nn);
  nn_flatpt = hmass2_flatpt->Integral(1,nchan);
  cout << "Scale factor 2 = " << nn << " / " << nn_flatpt << " = " << nn/nn_flatpt << endl;
  hmass2_flatpt->Scale(nn/nn_flatpt);

  nn = hmass3->Integral(1,nchan);
  float scerr3 = 1./sqrt(nn);
  nn_flatpt = hmass3_flatpt->Integral(1,nchan);
  cout << "Scale factor 3 = " << nn << " / " << nn_flatpt << " = " << nn/nn_flatpt << endl;
  hmass3_flatpt->Scale(nn/nn_flatpt);


// scale to 10B events
  float evtscale = 10000000000./float(stat)/float(nstat);
  hmass_flatpt ->Scale(evtscale);
  hmass0_flatpt->Scale(evtscale);
  hmass1_flatpt->Scale(evtscale);
  hmass2_flatpt->Scale(evtscale);
  hmass3_flatpt->Scale(evtscale);
  for(int i=0; i<nbins; i++) { (hhmass_flatpt[i])->Scale(evtscale); }

// count background and errors
  double sumbg=0.; double sumbg0=0.; double sumbg1=0.; double sumbg2=0.; double sumbg3=0.;
  double ersumbg=0.; double ersumbg0=0.; double ersumbg1=0.; double ersumbg2=0.; double ersumbg3=0.;
  float u1start = 9.10;
  float u1stop  = 9.60;
  int fbin = hmass->FindBin(u1start + 0.001);
  int lbin = hmass->FindBin(u1stop  - 0.001);
  cout << "bin range: " << fbin << " - " << lbin << endl;
  cout << "inv. mass range: " << u1start << " - " << u1stop << endl;
  for(int i=fbin; i<=lbin; i++) { 
    sumbg  += hmass_flatpt->GetBinContent(i);
    sumbg0 += hmass0_flatpt->GetBinContent(i);
    sumbg1 += hmass1_flatpt->GetBinContent(i);
    sumbg2 += hmass2_flatpt->GetBinContent(i);
    sumbg3 += hmass3_flatpt->GetBinContent(i);
    ersumbg  += hmass_flatpt ->GetBinError(i)*hmass_flatpt ->GetBinError(i);
    ersumbg0 += hmass0_flatpt->GetBinError(i)*hmass0_flatpt->GetBinError(i);
    ersumbg1 += hmass1_flatpt->GetBinError(i)*hmass1_flatpt->GetBinError(i);
    ersumbg2 += hmass2_flatpt->GetBinError(i)*hmass2_flatpt->GetBinError(i);
    ersumbg3 += hmass3_flatpt->GetBinError(i)*hmass3_flatpt->GetBinError(i);
  }
  ersumbg  = sqrt(ersumbg);
  ersumbg0 = sqrt(ersumbg0);
  ersumbg1 = sqrt(ersumbg1);
  ersumbg2 = sqrt(ersumbg2);
  ersumbg3 = sqrt(ersumbg3);
  
// The main statistical uncertainty comes from histogram normalization
  cout << "In 10B 0-10% central events (100B min. bias):" << endl;
  cout << "  Background(all pT)  = " << sumbg << " +- " << ersumbg << "(stat) +- " << sumbg*scerr << "(scale) pairs." << endl;
  cout << "  Background(0-2) = " << sumbg0 << " +- " << ersumbg0 << "(stat) +- " << sumbg*scerr0 << "(scale) pairs." << endl;
  cout << "  Background(2-4) = " << sumbg1 << " +- " << ersumbg1 << "(stat) +- " << sumbg*scerr1 << "(scale) pairs." << endl;
  cout << "  Background(4-6) = " << sumbg2 << " +- " << ersumbg2 << "(stat) +- " << sumbg*scerr2 << "(scale) pairs." << endl;
  cout << "  Background(6-10) = " << sumbg3 << " +- " << ersumbg3 << "(stat) +- " << sumbg*scerr3 << "(scale) pairs." << endl;
*/

TH1D* hhfakefake[nbins];
for(int i=0; i<nbins+1; i++) {
  sprintf(hhname,"hhfakefake_%d",i);
  hhfakefake[i] = (TH1D*)((hhmass[i])->Clone(hhname));
}

/*
  TH1D* hfakefake  = (TH1D*)hmass_flatpt->Clone("hfakefake");
  TH1D* hfakefake0 = (TH1D*)hmass0_flatpt->Clone("hfakefake0");
  TH1D* hfakefake1 = (TH1D*)hmass1_flatpt->Clone("hfakefake1");
  TH1D* hfakefake2 = (TH1D*)hmass2_flatpt->Clone("hfakefake2");
  TH1D* hfakefake3 = (TH1D*)hmass3_flatpt->Clone("hfakefake3");

  //TF1* mymass = new TF1("mymass","[0]*pow(x,[3])*pow(1.+x*x/[1]/[1],[2])",2.0,15.0);
  TF1* mymass = new TF1("mymass","[0]*pow(1.+x*x/[1]/[1],[2])",2.0,15.0);
  mymass->SetParameter(0,5.0e+09);
  mymass->SetParameter(1,2.0);
  mymass->SetParameter(2,-10.0);
  mymass->SetParLimits(2,-50.0,0.0);
  //mymass->SetParameter(3,2.0);

  double mylim = 0.5;
  double x,a,b,erb;
  double fitstart = 7.0;
  double fitstop = 12.0;
  TH1F* hmynorm = (TH1F*)hmass_flatpt->Clone("hmynorm");
*/
/*
cout << "kuku2" << endl;
for(int j=0; j<nbins; j++) {
  cout << "fitting " << j << " " << (hhmass_flatpt[j])->GetEntries() << endl;
  (hhmass_flatpt[j])->Fit("mymass","qr","",fitstart,fitstop);
  cout << "  fit done." << endl;
  for(int i=1; i<=nchan; i++) {
    cout << "    bin " << i << endl;
    x = (hhmass_flatpt[j])->GetBinCenter(i);
    cout << "    x = " << x << endl;
    a = ((hhmass_flatpt[j])->GetFunction("mymass"))->Eval(x);
    cout << "    a = " << a << endl;
    //if(a>mylim) { b = int(rnd->Gaus(a,sqrt(a))+0.5); erb = sqrt(b); } else {b=0.00001; erb=0.;}
    b = int(rnd->Poisson(a)+0.5); if(b<0.){b=0.;}  erb = sqrt(b);
    cout << "    bin " << i << " " << b << " +- " << erb << endl;
    (hhfakefake[i])->SetBinContent(i,b);
    (hhfakefake[i])->SetBinError(i,erb);
    cout << "      all set." << endl;
  }
}
*/

//  hmass_flatpt->Fit("mymass","qr","",fitstart,fitstop);
/*
  for(int i=hmass_flatpt->FindBin(fitstart+0.001); i<=hmass_flatpt->FindBin(fitstop-0.001); i++) {
     double tmpa = hmass_flatpt->GetBinContent(i)/(hmass_flatpt->GetFunction("mymass")->Eval(hmass_flatpt->GetBinCenter(i)));
     hmynorm->SetBinContent(i,tmpa);
     //double tmpb = tmpa*(hmass_flatpt->GetBinError(i)/hmass_flatpt->GetBinContent(i));
     double tmpb = (hmass_flatpt->GetBinError(i)/hmass_flatpt->GetBinContent(i));
     hmynorm->SetBinError(i,tmpb);
     cout << "corr: " << tmpa << " +- " << tmpb << endl;
  }
  hmynorm->Fit("pol0","r","",fitstart,fitstop);
  double mycorr = hmynorm->GetFunction("pol0")->GetParameter(0);
  cout << "Correction factor = " << mycorr << endl;
*/
/*
  for(int i=1; i<=nchan; i++) {
    x = hmass_flatpt->GetBinCenter(i);
    a = (hmass_flatpt->GetFunction("mymass"))->Eval(x);
    //if(a>mylim) { b = int(rnd->Gaus(a,sqrt(a))+0.5); erb = sqrt(b); } else {b=0.00001; erb=0.;}
    b = int(rnd->Poisson(a)+0.5); if(b<0.){b=0.;}  erb = sqrt(b);
    hfakefake->SetBinContent(i,b);
    hfakefake->SetBinError(i,erb);
  }

  hmass0_flatpt->Fit("mymass","qr","",fitstart,fitstop);
  for(int i=1; i<=nchan; i++) {
    x = hmass0_flatpt->GetBinCenter(i);
    a = (hmass0_flatpt->GetFunction("mymass"))->Eval(x);
    //if(a>mylim) { b = int(rnd->Gaus(a,sqrt(a))+0.5); erb = sqrt(b); } else {b=0.; erb=0.;}
    b = int(rnd->Poisson(a)+0.5); if(b<0.){b=0.;}  erb = sqrt(b);
    hfakefake0->SetBinContent(i,b);
    hfakefake0->SetBinError(i,erb);
  }
  hmass1_flatpt->Fit("mymass","qr","",fitstart,fitstop);
  for(int i=1; i<=nchan; i++) {
    x = hmass1_flatpt->GetBinCenter(i);
    a = (hmass1_flatpt->GetFunction("mymass"))->Eval(x);
    //if(a>mylim) { b = int(rnd->Gaus(a,sqrt(a))+0.5); erb = sqrt(b); } else {b=0.; erb=0.;}
    b = int(rnd->Poisson(a)+0.5); if(b<0.){b=0.;}  erb = sqrt(b);
    hfakefake1->SetBinContent(i,b);
    hfakefake1->SetBinError(i,erb);
  }
  hmass2_flatpt->Fit("mymass","qr","",fitstart,fitstop);
  for(int i=1; i<=nchan; i++) {
    x = hmass2_flatpt->GetBinCenter(i);
    a = (hmass2_flatpt->GetFunction("mymass"))->Eval(x);
    //if(a>mylim) { b = int(rnd->Gaus(a,sqrt(a))+0.5); erb = sqrt(b); } else {b=0.; erb=0.;}
    b = int(rnd->Poisson(a)+0.5); if(b<0.){b=0.;}  erb = sqrt(b);
    hfakefake2->SetBinContent(i,b);
    hfakefake2->SetBinError(i,erb);
  }
  hmass3_flatpt->Fit("mymass","qr","",fitstart,fitstop);
  for(int i=1; i<=nchan; i++) {
    x = hmass3_flatpt->GetBinCenter(i);
    a = (hmass3_flatpt->GetFunction("mymass"))->Eval(x);
    //if(a>mylim) { b = int(rnd->Gaus(a,sqrt(a))+0.5); erb = sqrt(b); } else {b=0.; erb=0.;}
    b = int(rnd->Poisson(a)+0.5); if(b<0.){b=0.;}  erb = sqrt(b);
    hfakefake3->SetBinContent(i,b);
    hfakefake3->SetBinError(i,erb);
  }
*/


  fout->Write();
  fout->Close();

  return 0;
}



