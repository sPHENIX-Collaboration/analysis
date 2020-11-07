
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TChain.h"
#include "TRandom3.h"
#include "TGraphAsymmErrors.h"
#include "TGraphErrors.h"

#include <fstream>
#include <iostream>
//#include <iomanip>
//#include <vector>

using namespace std;

int ccbb(double eideff, string ofname);

TH1D* hcharm_pt;
TH1D* hbottom_pt;
TH1D* hhf_pt;
TH1D* hcharm_pt_nosupp;
TH1D* hbottom_pt_nosupp;
TH1D* hhf_pt_nosupp;

double norm_charm(double* x, double* par) {
  double pt = x[0];
  double norm = par[0];
  int bin = hcharm_pt->GetXaxis()->FindBin(pt);
  double value = hcharm_pt->GetBinContent(bin);
  return norm*value;
}
double norm_charm_nosupp(double* x, double* par) {
  double pt = x[0];
  double norm = par[0];
  int bin = hcharm_pt_nosupp->GetXaxis()->FindBin(pt);
  double value = hcharm_pt_nosupp->GetBinContent(bin);
  return norm*value;
}

double norm_bottom(double* x, double* par) {
  double pt = x[0];
  double norm = par[0];
  int bin = hbottom_pt->GetXaxis()->FindBin(pt);
  double value = hbottom_pt->GetBinContent(bin);
  return norm*value;
}
double norm_bottom_nosupp(double* x, double* par) {
  double pt = x[0];
  double norm = par[0];
  int bin = hbottom_pt_nosupp->GetXaxis()->FindBin(pt);
  double value = hbottom_pt_nosupp->GetBinContent(bin);
  return norm*value;
}

double norm_hf(double* x, double* par) {
  double pt = x[0];
  double norm = par[0];
  int bin = hhf_pt->GetXaxis()->FindBin(pt);
  double value = hhf_pt->GetBinContent(bin);
  return norm*value;
}
double norm_hf_nosupp(double* x, double* par) {
  double pt = x[0];
  double norm = par[0];
  int bin = hhf_pt_nosupp->GetXaxis()->FindBin(pt);
  double value = hhf_pt_nosupp->GetBinContent(bin);
  return norm*value;
}



//====================================================================================

int main(int argc, char* argv[]) 
{
  double eideff = 0.9;
  string ofname="ccbb.root";
  if(argc==1) cout << argv[0] << " is running with standard parameters..." << endl;
  if(argc==2) { eideff = atoi(argv[1]); }
  if(argc==3) { eideff = atoi(argv[1]); ofname=argv[2]; }
  cout << "eID efficiency = " << eideff << endl;
  cout << "output file name: " << ofname << endl;
    return ccbb(eideff, ofname);
}

//------------------------------------------------------------------------------------

int ccbb(double eideff, string ofname) {

// 0 = ppg182, min. bias AuAu
// 1 = FONLL, p+p
// 2 = my own function
// 3 = Kazuya, 0-10% central AuAu
int which_botfrac = 3;

// 0 = Min.Bias from ppg182
// 1 = 0-10% central from Kazuya
// 2 = no suppression
// 3 = my own suppression
int which_supp = 2;

double ptcut = 2.0;

double NeventsAuAu = 10.0e+09;

TRandom* rnd = new TRandom3();
rnd->SetSeed(0);

const int nbins = 15;
double binlim[nbins+1];
for(int i=0; i<=nbins; i++) {binlim[i]=double(i);}

double startmass=0.;
double stopmass=20.;
int nchanmass = 400;
double startpt=0.;
double stoppt=10.;
int nchanpt = 100;
double bsizept = (stoppt-startpt)/float(nchanpt);

char hname[999];

double erpt[99], erpt2[99]; for(int i=0; i<99; i++) {erpt[i]=0.075; erpt2[i]=0.1;}
std::string tmp0,tmp1,tmp2,tmp3,tmp4,tmp5;

// last histogram is integrated over pT
TH1D* hcharm[nbins+1];
TH1D* hcharm_ckin3_4[nbins+1];
TH1D* hbottom[nbins+1];
TH1D* hdy[nbins+1];
for(int i=0; i<nbins+1; i++) { sprintf(hname,"hcharm_%d",i);  hcharm[i]  = new TH1D(hname,"",nchanmass,startmass,stopmass); hcharm[i]->Sumw2(); }
for(int i=0; i<nbins+1; i++) { sprintf(hname,"hcharm_ckin3_3_%d",i);  hcharm_ckin3_4[i]  = new TH1D(hname,"",nchanmass,startmass,stopmass); hcharm_ckin3_4[i]->Sumw2(); }
for(int i=0; i<nbins+1; i++) { sprintf(hname,"hbottom_%d",i); hbottom[i] = new TH1D(hname,"",nchanmass,startmass,stopmass); hbottom[i]->Sumw2(); }
for(int i=0; i<nbins+1; i++) { sprintf(hname,"hdy_%d",i);     hdy[i]     = new TH1D(hname,"",nchanmass,startmass,stopmass); hdy[i]->Sumw2();}
hcharm_pt  = new TH1D("hcharm_pt", "",nchanpt,startpt,stoppt);
hbottom_pt = new TH1D("hbottom_pt","",nchanpt,startpt,stoppt);
hcharm_pt->Sumw2();
hbottom_pt->Sumw2();
hcharm_pt_nosupp  = new TH1D("hcharm_pt_nosupp", "",nchanpt,startpt,stoppt);
hbottom_pt_nosupp = new TH1D("hbottom_pt_nosupp","",nchanpt,startpt,stoppt);
hcharm_pt_nosupp->Sumw2();
hbottom_pt_nosupp->Sumw2();

float mass,pt,eta,pt1,pt2,eta1,eta2;
float pt_sngl,eta_sngl;

float fitstart=2.0;
float fitstop=8.0;
TF1* fnorm_charm  = new TF1("fnorm_charm", norm_charm, fitstart,fitstop,1);
TF1* fnorm_bottom = new TF1("fnorm_bottom",norm_bottom,fitstart,fitstop,1);
TF1* fnorm_hf     = new TF1("fnorm_hf"   , norm_hf,    fitstart,fitstop,1);
TF1* fnorm_charm_nosupp  = new TF1("fnorm_charm_nosupp", norm_charm_nosupp, fitstart,fitstop,1);
TF1* fnorm_bottom_nosupp = new TF1("fnorm_bottom_nosupp",norm_bottom_nosupp,fitstart,fitstop,1);
TF1* fnorm_hf_nosupp     = new TF1("fnorm_hf_nosupp"   , norm_hf_nosupp,    fitstart,fitstop,1);

/*
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
TFile *fFONLL=new TFile("FONLL_ratio.root");
  TGraph *gYieldFONLL_c = (TGraph*)fFONLL->Get("gRatio_c")->Clone();
  TGraph *gYieldFONLL_b = (TGraph*)fFONLL->Get("gRatio_b")->Clone();
  int Ndata=gYieldFONLL_c->GetN();
  double *Xc=gYieldFONLL_c->GetX();
  double *Yc=gYieldFONLL_c->GetY();
  double *Xb=gYieldFONLL_b->GetX();
  double *Yb=gYieldFONLL_b->GetY();
  double XR[999], YR[999], ZR[999];
  for(int i=0; i<Ndata; i++) {XR[i]=Xc[i]; YR[i]=Yb[i]/(Yb[i]+Yc[i]); ZR[i]=Yc[i]/(Yb[i]+Yc[i]);}
  TGraph *gFracFONLL_b  = new TGraph(Ndata,XR,YR);
  TGraph *gFracFONLL_c  = new TGraph(Ndata,XR,ZR);
fFONLL->Close();
*/
//
// bottom/(charm+bottom) and charm/(charm+bottom) ratios in p+p from FONLL  
// from Darren (ppg182)
//
ifstream fin_fonll;
double pt_fonll[99],fonll[99],fonllup[99],fonll_erup[99],fonll_erdn[99],dtmp0,dtmp1;
fin_fonll.open("FONLL_ratio.txt");
if(!fin_fonll) {cerr << " ERROR: Cannot open input text file. " << endl;}
for(int i=0; i<19; i++) {
  fin_fonll >> pt_fonll[i] >> tmp0 >> fonll[i] >> tmp1 >> dtmp0 >> tmp2 >> dtmp1 >> tmp3;
  fonllup[i] = dtmp0;
  fonll_erup[i] = dtmp0 - fonll[i];
  fonll_erdn[i] = fonll[i] - dtmp1;
  cout << pt_fonll[i] << " " << fonll[i] << " " << fonll_erup[i] << " " << fonll_erdn[i] << endl;
}
fin_fonll.close();
  TGraphAsymmErrors *gfonll  = new TGraphAsymmErrors(19,pt_fonll,fonll,0,0,fonll_erdn,fonll_erup);
  TGraph *gfonllup  = new TGraph(19,pt_fonll,fonllup);


//----------------------------------------------------------------------
// my bottom/(charm+bottom) fraction just for testing
//----------------------------------------------------------------------
TF1* mybotfrac = new TF1("mybotfrac","(0.896512*pow(x+(1.050669),5.792579)/(2630.566406+pow(x+(1.050669),5.792579)))",0.,10.); // my FONLL-like function 
//TF1* mybotfrac = new TF1("mybotfrac","0.98*pow(x-1.0,3)/(2+pow(x-1.0,3))",1.,10.); // myfunction close to Kazuya's upper limit
double myx[999];
double myy[999];
double pyy[999];
int myn = 150;
for(int i=0; i<myn; i++) { myx[i] = i*0.1; myy[i] = mybotfrac->Eval(myx[i]); }
//for(int i=0; i<myn; i++) { myx[i] = i*0.1; myy[i] = gfonll->Eval(myx[i])*mybotfrac->Eval(myx[i]); }
TGraph* gr_mybotfrac = new TGraph(myn,myx,myy);

TF1* pythia_botfrac = new TF1("pythia_botfrac","(0.898794*pow(x+(1.035767),5.749620)/(2453.650635+pow(x+(1.035767),5.749620)))*1.0",0.,10.); // PYTHIA
for(int i=0; i<myn; i++) { pyy[i] = pythia_botfrac->Eval(myx[i]); }
TGraph* gr_pythia_botfrac = new TGraph(myn,myx,pyy);

//--------------------------------------------------------------------------------------
// bottom/(charm+bottom) ratio in Min. Bias Au+Au from ppg182:  
// arXiv:1509.04662 
// A. Adare et al. (PHENIX Collaboration) Phys. Rev. C 93, 034904 (2016)
//--------------------------------------------------------------------------------------
double ratcb[99],ratcberup[99],ratcberdn[99],ratcbpt[99];
for(int i=0; i<15; i++) { ratcbpt[i] = 1.1+0.2*i; }
ratcbpt[15] = 4.25;
ratcbpt[16] = 4.75;
ratcbpt[17] = 5.5;
ratcbpt[18] = 6.5;
ratcbpt[19] = 7.5;
ratcbpt[20] = 8.5;
ratcb[0] = 3.99e-02;
ratcb[1] = 6.15e-02;
ratcb[2] = 9.41e-02;
ratcb[3] = 1.40e-01;
ratcb[4] = 2.01e-01;
ratcb[5] = 2.73e-01;
ratcb[6] = 3.49e-01;
ratcb[7] = 4.22e-01;
ratcb[8] = 4.84e-01;
ratcb[9] = 5.32e-01;
ratcb[10] = 5.64e-01;
ratcb[11] = 5.81e-01;
ratcb[12] = 5.89e-01;
ratcb[13] = 5.87e-01;
ratcb[14] = 5.79e-01;
ratcb[15] = 5.57e-01;
ratcb[16] = 5.13e-01;
ratcb[17] = 4.79e-01;
ratcb[18] = 4.71e-01;
ratcb[19] = 4.61e-01;
ratcb[20] = 4.36e-01;
  TGraph *gFrac_b = new TGraph(21,ratcbpt,ratcb);

double fracc[99];
for(int i=0; i<21; i++) { fracc[i] = 1.0-ratcb[i]; }
  TGraph *gFrac_c = new TGraph(21,ratcbpt,fracc);


//-------------------------------------------------------------------------------
// bottom fraction from Kazuya Nagashima's talk at QM2017
// PHENIX preliminary for 0-10% central Au+Au 
// http://www.phenix.bnl.gov/phenix/WWW/p/draft/nagasy/QM2017/slide/PHENIX_HF_KazuyaNagashima.pdf
//-------------------------------------------------------------------------------
  TGraphAsymmErrors* bfrac_cent1;
  TFile *fk = new TFile("kazuya/bfrac_cent1.root");
    bfrac_cent1 = (TGraphAsymmErrors*)fk->Get("bfrac_cent1");
  fk->Close();

//-----------------------------------------------------------------------------------------
//  R_AA of HF electrons according to Kazuya Nagashima; 0-10% central; 2004+2014 data
//  Different from ppg077 
//  Probably because it's combined 2004 and 2014 results (ppg077 = 2004 only)
//-----------------------------------------------------------------------------------------
  ifstream kfile;
  kfile.open("kazuya/csv/ppg077_HFRAA_cent1.csv");
  TGraphErrors *tRaadata = new TGraphErrors();
  TGraphErrors *tRaadata_sys = new TGraphErrors();
  double var1, var2, var3, var4, var5, var6;
  for(int i=0;i<28;i++) {
    kfile >> var1 >> var2 >> var3 >> var4 >> var5 >> var6;
    tRaadata->SetPoint(i,var1,var2);
    tRaadata->SetPointError(i,0.,var3);
    tRaadata_sys->SetPoint(i,var1,var2);
    tRaadata_sys->SetPointError(i,0.1,var5);
  }
  kfile.close();

//--------------------------------------------------------------------------------------
// Charm and Bottom R_AA in 0-10% central Au+Au from Kazuya Nagashima's QM2017 presentation  
//--------------------------------------------------------------------------------------
  TGraphAsymmErrors* tcharm;
  TGraphAsymmErrors* tbottom;
  TFile *fk2 = new TFile("kazuya/Raa_cent1.root");
    tcharm = (TGraphAsymmErrors*)fk2->Get("tcharm");
    tbottom = (TGraphAsymmErrors*)fk2->Get("tbottom");
  fk2->Close();

  TGraph* mytcharm = new TGraph();
  TGraph* mytbottom = new TGraph();
  mytcharm->SetPoint(0,0.0,1.0);
  mytcharm->SetPoint(1,1.5,1.0);
  mytbottom->SetPoint(0,0.0,1.0);
  mytbottom->SetPoint(1,1.5,1.0);
  double *tmpx = tcharm->GetX();
  double *tmpy = tcharm->GetY();
  for(int i=2; i<8; i++) { mytcharm->SetPoint(i,tmpx[i-2],tmpy[i-2]); }
  mytcharm->SetPoint(8,15.,tmpy[5]);
    tmpx = tbottom->GetX();
    tmpy = tbottom->GetY();
    for(int i=2; i<8; i++) { mytbottom->SetPoint(i,tmpx[i-2],tmpy[i-2]); }
    mytbottom->SetPoint(2,tmpx[0],1.0);
    mytbottom->SetPoint(8,15.,tmpy[5]);


//--------------------------------------------------------------------------------------
// Charm and Bottom R_AA in Min. Bias Au+Au from ppg182:  
// arXiv:1509.04662 
// A. Adare et al. (PHENIX Collaboration) Phys. Rev. C 93, 034904 (2016)
//--------------------------------------------------------------------------------------

double pt182[9],raa182charm[9],raa182bottom[9],raa182charm_erdn[9],raa182charm_erup[9],raa182bottom_erdn[9],raa182bottom_erup[9];
double pt182orig[9],raa182charmorig[9],raa182bottomorig[9];
pt182[0]=0.0;
pt182[1]=1.5;
raa182charm[0] = 1.0;
raa182charm[1] = 1.0;
raa182bottom[0] = 1.0;
raa182bottom[1] = 1.0;

ifstream fin182;
fin182.open("ppg182.txt");
if(!fin182) {cerr << " ERROR: Cannot open input text file. " << endl;}
fin182 >> tmp0 >> tmp1 >> tmp2 >> tmp3;
fin182 >> tmp0 >> tmp1 >> tmp2 >> tmp3;
for(int i=2; i<8; i++) {
  fin182 >> pt182[i] >> raa182charm[i] >> raa182charm_erup[i-2] >> raa182charm_erdn[i-2];
  pt182orig[i-2]=pt182[i];
  raa182charmorig[i-2] = raa182charm[i]; 
}
fin182 >> tmp0 >> tmp1 >> tmp2 >> tmp3;
fin182 >> tmp0 >> tmp1 >> tmp2 >> tmp3;
for(int i=2; i<8; i++) {
  fin182 >> pt182[i] >> raa182bottom[i] >> raa182bottom_erup[i-2] >> raa182bottom_erdn[i-2];
  raa182bottomorig[i-2] = raa182bottom[i]; 
}

pt182[8]=15.;
raa182charm[8] = raa182charm[7];
raa182bottom[2] = 1.0;  // measured value is 1.15
raa182bottom[8] = raa182bottom[7];
  TGraph *gRAA_charm  = new TGraph(9,pt182,raa182charm);
  TGraph *gRAA_bottom = new TGraph(9,pt182,raa182bottom);
  TGraphAsymmErrors *gRAASysErr_charm  = new TGraphAsymmErrors(6,pt182orig,raa182charmorig, 0,0,raa182charm_erdn, raa182charm_erup);
  TGraphAsymmErrors *gRAASysErr_bottom = new TGraphAsymmErrors(6,pt182orig,raa182bottomorig,0,0,raa182bottom_erdn,raa182bottom_erup);


// My own RAA from comparison with data
  //TF1* fmyraac =  new TF1("fmyraac","0.303395+18.995424/(66.573738+pow(x,7.237049))",1.0,9.0);  // fit above 3 GeV
  //TF1* fmyraab =  new TF1("fmyraab","0.347859+64550.024555/(140970.430941+pow(x,9.780939))",1.0,9.0); // fit above 3 Gev
  TF1* fmyraac =  new TF1("fmyraac","0.327075+20.482705/(66.589763+pow(x,7.237551))",1.0,9.0);  // fit above 2 GeV
  TF1* fmyraab =  new TF1("fmyraab","0.370667+68699.897905/(140801.861045+pow(x,9.779860))",1.0,9.0); // fit above 2 Gev
    double mysuppc[999],mysuppb[999],myxx[999];
    int mycount=0;
    for(int i=0; i<myn; i++) { if(myx[i]>1.0) { myxx[mycount] = myx[i]; mysuppc[mycount] = fmyraac->Eval(myx[i]); mycount++; } }
    TGraph* gr_mysupp_charm = new TGraph(mycount,myxx,mysuppc);
    mycount=0;
    for(int i=0; i<myn; i++) { if(myx[i]>1.0) { mysuppb[mycount] = fmyraab->Eval(myx[i]); mycount++; } }
    TGraph* gr_mysupp_bottom = new TGraph(mycount,myxx,mysuppb);


// Choose which suppression to use
TGraph* gCharmSuppression;
TGraph* gBottomSuppression;
if(which_supp==0) {
   gCharmSuppression =  new TGraph(*gRAA_charm);
   gBottomSuppression = new TGraph(*gRAA_bottom);
} else if(which_supp==1) {
   gCharmSuppression =  new TGraph(*mytcharm);
   gBottomSuppression = new TGraph(*mytbottom);
} else if(which_supp==2) {
   double x1[4] = {0.,5.,10.,15.};
   double y1[4] = {1.,1.,1., 1.};
   gCharmSuppression =  new TGraph(4,x1,y1);
   gBottomSuppression = new TGraph(4,x1,y1);
} else if(which_supp==3) {
   gCharmSuppression =  new TGraph(*gr_mysupp_charm);
   gBottomSuppression = new TGraph(*gr_mysupp_bottom);
} else {
  cerr << "ERROR: Wrong suppression factors !!!" << endl;
}

// Choose which Bottom Fraction to use
TGraph* gBottomFraction;
if(which_botfrac==0) {        // ppg182, min. bias AuAu
 gBottomFraction = new TGraph(*gFrac_b);
} else if(which_botfrac==1) { // FONLL p+p
 //gBottomFraction = new TGraph(*gFracFONLL_b);
 gBottomFraction = new TGraph(*gfonll);
} else if(which_botfrac==2) { // my own function
 gBottomFraction = new TGraph(*gr_mybotfrac);
} else if(which_botfrac==3) { // Kazuya, 0-10% central AuAu
 gBottomFraction = new TGraph(*bfrac_cent1);
}



//-----------------------------------------------------------------------------------------
//  R_AA of HF electrons from ppg077; 0-10% central Au+Au; 2004 data
//-----------------------------------------------------------------------------------------

double ppg077_pt[99],ppg077_raa[99],ppg077_staterup[99],ppg077_staterdn[99],ppg077_syserup[99],ppg077_syserdn[99];

ifstream finppg077;
finppg077.open("ppg077_0-10.txt");
if(!finppg077) {cerr << " ERROR: Cannot open input text file. " << endl;}
finppg077 >> tmp0 >> tmp1 >> tmp2 >> tmp3 >> tmp4 >> tmp5;
finppg077 >> tmp0 >> tmp1;
finppg077 >> tmp0 >> tmp1 >> tmp2 >> tmp3 >> tmp4 >> tmp5;
for(unsigned int i=0; i<28; i++) { finppg077 >> ppg077_pt[i] >> ppg077_raa[i] >> ppg077_staterup[i] >> ppg077_staterdn[i] >> ppg077_syserup[i] >> ppg077_syserdn[i]; }

TGraphAsymmErrors* grRAA_ppg077 = new TGraphAsymmErrors(28,ppg077_pt,ppg077_raa,0,0,ppg077_staterdn,ppg077_staterup);
TGraphAsymmErrors* grRAA_ppg077SysErr = new TGraphAsymmErrors(28,ppg077_pt,ppg077_raa,erpt2,erpt2,ppg077_syserdn,ppg077_syserup);


//------------------------------------------------------------------------------------------
// Invariant yield of HF electrons in Au+Au from ppg066, units are (GeV/c)^-2
// Multiply it by pT, multiply by 2pi. 
// This should be dN/dpT per event per unit of rapidity.
// Then multiply by 2 since we are looking at +-1 unit of rapidity.
// First two errors are statistical (up/down) third and fourth are systematic (up/down)
// 0-10% central
// ppg066 data are in form: 1/2pipT * dN/dpT/dy units are (GeV/c)^{-2}
// these are heavy flavor electrons (c+b)
//------------------------------------------------------------------------------------------
double xau1[99],exau1[99],yau1[99],eyau1up[99],eyau1dn[99],eyau1upsys[99],eyau1dnsys[99];
double ycau1[99],eycau1up[99],eycau1dn[99],eycau1upsys[99],eycau1dnsys[99];
double ybau1[99],eybau1up[99],eybau1dn[99],eybau1upsys[99],eybau1dnsys[99];
double xau2[99],exau2[99],yau2[99],eyau2up[99],eyau2dn[99],eyau2upsys[99],eyau2dnsys[99];
double ycau2[99],eycau2up[99],eycau2dn[99],eycau2upsys[99],eycau2dnsys[99];
double ybau2[99],eybau2up[99],eybau2dn[99],eybau2upsys[99],eybau2dnsys[99];

ifstream finau1;
finau1.open("ppg066_0-10.txt");
if(!finau1) {cerr << " ERROR: Cannot open input text file. " << endl;}
for(int i=0; i<28; i++) {
finau1 >> xau1[i] >> yau1[i] >> eyau1up[i] >> eyau1dn[i] >> eyau1upsys[i] >> eyau1dnsys[i];
yau1[i]       = yau1[i]*       xau1[i]*2.*3.141592654*2. * NeventsAuAu;  // multiply by pT, by 2pi and by 2 units of rapidity to get dN/dpT per event
eyau1up[i]    = eyau1up[i]*    xau1[i]*2.*3.141592654*2. * NeventsAuAu;
eyau1dn[i]    = eyau1dn[i]*    xau1[i]*2.*3.141592654*2. * NeventsAuAu;
eyau1upsys[i] = eyau1upsys[i]* xau1[i]*2.*3.141592654*2. * NeventsAuAu;
eyau1dnsys[i] = eyau1dnsys[i]* xau1[i]*2.*3.141592654*2. * NeventsAuAu;

double mybottom = gBottomFraction->Eval(xau1[i]);
double mycharm  = 1.-mybottom;
ycau1[i]       = yau1[i]*       mycharm;  // multiply by c/(c+b) ratio to get charm yield 
eycau1up[i]    = eyau1up[i]*    mycharm;
eycau1dn[i]    = eyau1dn[i]*    mycharm;
eycau1upsys[i] = eyau1upsys[i]* mycharm;
eycau1dnsys[i] = eyau1dnsys[i]* mycharm;
ybau1[i]       = yau1[i]*       mybottom;  // multiply by b/(c+b) ratio to get bottom yield
eybau1up[i]    = eyau1up[i]*    mybottom;
eybau1dn[i]    = eyau1dn[i]*    mybottom;
eybau1upsys[i] = eyau1upsys[i]* mybottom;
eybau1dnsys[i] = eyau1dnsys[i]* mybottom;

}
finau1.close();

  TGraphAsymmErrors *gHFAuAu      = new TGraphAsymmErrors(28,xau1,yau1,0,0,eyau1dn,eyau1up);
  TGraphAsymmErrors *gCharmAuAu   = new TGraphAsymmErrors(28,xau1,ycau1,0,0,eycau1dn,eycau1up);
  TGraphAsymmErrors *gBottomAuAu  = new TGraphAsymmErrors(28,xau1,ybau1,0,0,eybau1dn,eybau1up);
  TGraphAsymmErrors *gHFAuAuSysErr      = new TGraphAsymmErrors(28,xau1,yau1, erpt,erpt,eyau1dnsys,eyau1upsys);
  TGraphAsymmErrors *gCharmAuAuSysErr   = new TGraphAsymmErrors(28,xau1,ycau1,erpt,erpt,eycau1dnsys,eycau1upsys);
  TGraphAsymmErrors *gBottomAuAuSysErr  = new TGraphAsymmErrors(28,xau1,ybau1,erpt,erpt,eybau1dnsys,eybau1upsys);


//-------------------------------------------------------------------------------------------
// Invariant cross-section of HF electrons from ppg066 (run5 data) in p+p
// Convert to invariant yield by dividing by 42mb (p+p total cross-section)
//-------------------------------------------------------------------------------------------
  TGraphErrors *gsys_all4;
  TGraphErrors *gxs_all4;
  TFile *fpp = new TFile("/phenix/WWW/p/info/ppg/066/material/RUN5pp_best_fit.root");
    gxs_all4  = (TGraphErrors*)fpp->Get("gxs_all4");
    gsys_all4 = (TGraphErrors*)fpp->Get("gsys_all4");
  fpp->Close();
  int Ndatapp=gsys_all4->GetN();
  double *X1 = gxs_all4->GetX();
  double *Y1 = gxs_all4->GetY();
  double XX[99], YY[99], Estat[99], Esys[99];
  double YYC[99], YYB[99], ECstat[99], EBstat[99], ECsys[99], EBsys[99];
  for(int i=0; i<Ndatapp; i++) {
    XX[i] = X1[i]; 
    YY[i] = Y1[i]/42.e-03 * XX[i]*2.*3.141592654*2. * NeventsAuAu; // multiply by pT, by 2pi and by 2 units of rapidity to get dN/dpT per event
    Estat[i] = gxs_all4->GetErrorY(i)/42.e-03  * XX[i]*2.*3.141592654*2. * NeventsAuAu;
    Esys[i]  = gsys_all4->GetErrorY(i)/42.e-03 * XX[i]*2.*3.141592654*2. * NeventsAuAu;
    double mybottom = pythia_botfrac->Eval(XX[i]);   // PYTHIA
    double mycharm  = 1.-mybottom;
    //double mybottom = gfonllup->Eval(XX[i]); // use FONLL upper limit prediction to separate charm and bottom
    //double mycharm  = 1.-mybottom;
    //double mybottom = gfonll->Eval(XX[i]);  // use FONLL prediction to separate charm and bottom
    //double mycharm  = 1.-mybottom;
    YYC[i]    = YY[i]    * mycharm;
    ECstat[i] = Estat[i] * mycharm;
    ECsys[i]  = Esys[i]  * mycharm;
    YYB[i]    = YY[i]    * mybottom;
    EBstat[i] = Estat[i] * mybottom;
    EBsys[i]  = Esys[i]  * mybottom;
  }
  TGraphErrors *ppgraph_stat  = new TGraphErrors(Ndatapp,XX,YY,0,Estat);
  TGraphErrors *ppgraph_stat_charm  = new TGraphErrors(Ndatapp,XX,YYC,0,ECstat);
  TGraphErrors *ppgraph_stat_bottom  = new TGraphErrors(Ndatapp,XX,YYB,0,EBstat);
  TGraphErrors *ppgraph_sys  = new TGraphErrors(Ndatapp,XX,YY,0,Esys);







//------------------------------------------------------------------------------------------
// CCbar from PYTHIA
//------------------------------------------------------------------------------------------

TChain* chain_ccbar = new TChain("ntp2");

// each of these files has 1.63386e+09 generated event
// cross-section = 0.187161 mb
// 10B 0-10% central Au+Au events before suppression corerespond to 9550B p+p events.
// each file corresponds to 1.63386e+09 * 40/0.187161 = 349.19e+09 Min.Bias p+p events
// need 9550/349.19 = 27.35 files. 
// we can use those 27 files if we believe PYTHIA cross-section
// PYTHIA ccbar cross-section scale up by 3.31994 +- 0.0403619 (fit above 2GeV); (was 2.09),
// so we need 9080 runs (was 5716 runs)
// all files below are for 5716 runs
// 27 files (2700 runs)
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_10.root"); // ckin3_3 is really not ckin3
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_11.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_12.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_13.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_14.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_15.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_16.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_17.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_18.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_19.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_20.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_21.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_22.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_23.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_24.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_25.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_26.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_27.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_28.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_29.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_30.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_31.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_32.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_34.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_35.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_36.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_37.root");

// 17 more files
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_4.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_5.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_6.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_7.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_8.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_38.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_41.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_42.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_43.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_44.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_48.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_49.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_51.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_53.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_54.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_55.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_56.root");

// six files below have 560 runs together (5.6 files)
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_9.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_39.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_46.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_47.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_50.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_52.root");

// two files below have together 388 runs (3.88 files)
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_1.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_3/ccbar_ckin3_3_2.root");

// ten files below are equivalent to 6 files above (six files = 3.6 files above)
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar/ccbar_0.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar/ccbar_1.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar/ccbar_2.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar/ccbar_3.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar/ccbar_4.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar/ccbar_5.root");
/*
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar/ccbar_6.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar/ccbar_7.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar/ccbar_8.root");
chain_ccbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar/ccbar_9.root");
*/

// Single electron normalization.
// PYTHIA cross-section = 0.18713mb
// measured cs = 544 +/- 39(stat) +/- 142(syst) +/- 200(model) mkb; from ppg085, arXiv:0802.0050, PLB 670 iss. 4-5 p.313 (2009)
// Files 0 through 9 have 1.67471e+07 generated p+p events each 
// Files 10 through 14 have 6.6809e+07 generated events each
// All files correspond to 40/0.18713 * (10*1.67471e+07 + 5*6.6809e+07) = 107.20B Min.Bias p+p events
//   10B 0-10% central Au+Au events (Ncoll = 955) correspond to 9550B p+p events (before suppression)
//   Normalization factor = 9550/107.20 = 89.1
double charm_scale = 89.1;
TChain* chain_ccbar_sngl = new TChain("ntp1");
chain_ccbar_sngl->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_norm/ccbar_norm_0.root");
chain_ccbar_sngl->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_norm/ccbar_norm_1.root");
chain_ccbar_sngl->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_norm/ccbar_norm_2.root");
chain_ccbar_sngl->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_norm/ccbar_norm_3.root");
chain_ccbar_sngl->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_norm/ccbar_norm_4.root");
chain_ccbar_sngl->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_norm/ccbar_norm_5.root");
chain_ccbar_sngl->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_norm/ccbar_norm_6.root");
chain_ccbar_sngl->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_norm/ccbar_norm_7.root");
chain_ccbar_sngl->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_norm/ccbar_norm_8.root");
chain_ccbar_sngl->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_norm/ccbar_norm_9.root");
chain_ccbar_sngl->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_norm/ccbar_norm_10.root");
chain_ccbar_sngl->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_norm/ccbar_norm_11.root");
chain_ccbar_sngl->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_norm/ccbar_norm_12.root");
chain_ccbar_sngl->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_norm/ccbar_norm_13.root");
chain_ccbar_sngl->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_norm/ccbar_norm_14.root");

cout << "Number of entries in CCbar chain: " << chain_ccbar->GetEntries() << endl;
cout << "Number of entries in CCbar SINGLES chain: " << chain_ccbar_sngl->GetEntries() << endl;

chain_ccbar->SetBranchAddress("mass",&mass);
chain_ccbar->SetBranchAddress("pt",  &pt);
chain_ccbar->SetBranchAddress("eta", &eta);
chain_ccbar->SetBranchAddress("pt1", &pt1);
chain_ccbar->SetBranchAddress("pt2", &pt2);
chain_ccbar->SetBranchAddress("eta1",&eta1);
chain_ccbar->SetBranchAddress("eta2",&eta2);

chain_ccbar_sngl->SetBranchAddress("pt",  &pt_sngl);
chain_ccbar_sngl->SetBranchAddress("eta", &eta_sngl);

// Singles for normalization
for(int j=0; j<chain_ccbar_sngl->GetEntries(); j++){
  chain_ccbar_sngl->GetEvent(j);
  if(j%100000==0) cout << "entry # " << j << " " << endl;
  if(fabs(eta_sngl)<1.0) { hcharm_pt_nosupp->Fill(pt_sngl); }
  double random = rnd->Uniform(0.,1.);
  double myraa = gCharmSuppression->Eval(pt_sngl);
  if(random<=myraa) {  if(fabs(eta_sngl)<1.0) { hcharm_pt->Fill(pt_sngl); } }
}
cout << "Number of entries in charm pT histogram = " << hcharm_pt->GetEntries() << endl;

hcharm_pt->Scale(1.0/bsizept); // convert to dN/dpT
hcharm_pt->Scale(charm_scale); // scale up to 10B most central AuAu according to cross-section
  // Normalize PYTHIA to measured charm
  fnorm_charm->SetParameter(0,1.);
  gCharmAuAu->Fit(fnorm_charm,"QRN0","",fitstart,fitstop);
  double norm_charm = fnorm_charm->GetParameter(0);
  cout << "charm normalization = " << norm_charm << endl;

hcharm_pt_nosupp->Scale(1.0/bsizept); // convert to dN/dpT
hcharm_pt_nosupp->Scale(charm_scale); // scale up to 10B most central AuAu according to cross-section
  fnorm_charm_nosupp->SetParameter(0,1.);
  ppgraph_stat_charm->Fit(fnorm_charm_nosupp,"QRN0","",fitstart,fitstop);
  double norm_charm_nosupp = fnorm_charm_nosupp->GetParameter(0);
  cout << "charm normalization (no suppression) = " << norm_charm_nosupp << endl;


// invariant mass distributions for CCbar
for(int j=0; j<chain_ccbar->GetEntries()*eideff*eideff; j++){
  chain_ccbar->GetEvent(j);
  if(j%20000==0) cout << "entry # " << j << endl;
  if(pt1>ptcut && pt2>ptcut && fabs(eta1)<1.0 && fabs(eta2)<1.0) {
    double random1 = rnd->Uniform(0.,1.);
    double myraa1 = gCharmSuppression->Eval(pt1);
    double random2 = rnd->Uniform(0.,1.);
    double myraa2 = gCharmSuppression->Eval(pt2);
    if(random1<=myraa1 && random2<myraa2) {
      int mybin = -1;
      for(int i=0; i<nbins; i++) { if(pt<binlim[i+1]) { mybin = i; break; } }
      if(mybin>=0 && mybin<nbins) { (hcharm[mybin])->Fill(mass); }
      (hcharm[nbins])->Fill(mass); // all pT
    }
  }
}

//------------------------------------------------------------------------------------------
// CCbar with ckin(3) = 4 from PYTHIA
//------------------------------------------------------------------------------------------

TChain* chain_ccbar_ckin3_4 = new TChain("ntp2");
// Each "run" has 4.98345e+06 generated events
// Cross-section 0.825489e-03 mb
// ccbar_ckin3_4/ccbar cross-section normalization factor = 241478.0e+09 / 9428.13e+09 = 25.61
// Fit gives scaling factor = 21.6616   +/-  0.539134
// // corerection for acceptance = 1.1824 (we need 1.1824 times more ckin3=4 data because of lower acceptance)
// According to PYTHIA cross-section we need 9550/24147.8 * 1.1824 * 100 = 46.76 runs 
// to get an equivalint of 10B 0-10% central Au+Au events.
// Additional cross-section scaling (from measured HF cross-section) = 3.31994 +- 0.0403619 (was 2.09) 
// This, we need 155.2 runs (was 97.7 runs).
// 
// The file below contains 49 runs, it corresponds to 10B 0-10% AuAu events (before suppression) if we use PYTHIA cross-section.
//chain_ccbar_ckin3_4->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_4/ccbar_ckin3_4_9550B_ppevents.root");
// The file below has 100 runs 
chain_ccbar_ckin3_4->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_4/ccbar_ckin3_4_0.root");
// the file below has 55 runs
chain_ccbar_ckin3_4->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_ccbar_ckin3_4/ccbar_ckin3_4_88.root");


cout << "Number of entries in CCbar with ckin(3) = 4 chain: " << chain_ccbar_ckin3_4->GetEntries() << endl;

chain_ccbar_ckin3_4->SetBranchAddress("mass",&mass);
chain_ccbar_ckin3_4->SetBranchAddress("pt",  &pt);
chain_ccbar_ckin3_4->SetBranchAddress("eta", &eta);
chain_ccbar_ckin3_4->SetBranchAddress("pt1", &pt1);
chain_ccbar_ckin3_4->SetBranchAddress("pt2", &pt2);
chain_ccbar_ckin3_4->SetBranchAddress("eta1",&eta1);
chain_ccbar_ckin3_4->SetBranchAddress("eta2",&eta2);

// invariant mass distributions for CCbar with ckin(3) = 4
for(int j=0; j<chain_ccbar_ckin3_4->GetEntries()*eideff*eideff; j++){
  chain_ccbar_ckin3_4->GetEvent(j);
  if(j%20000==0) cout << "entry # " << j << endl;
  if(pt1>ptcut && pt2>ptcut && fabs(eta1)<1.0 && fabs(eta2)<1.0) {
    double random1 = rnd->Uniform(0.,1.);
    double myraa1 = gCharmSuppression->Eval(pt1);
    double random2 = rnd->Uniform(0.,1.);
    double myraa2 = gCharmSuppression->Eval(pt2);
    if(random1<=myraa1 && random2<myraa2) {
      int mybin = -1;
      for(int i=0; i<nbins; i++) { if(pt<binlim[i+1]) { mybin = i; break; } }
      if(mybin>=0 && mybin<nbins) { (hcharm_ckin3_4[mybin])->Fill(mass); }
      (hcharm_ckin3_4[nbins])->Fill(mass); // all pT
    }
  }
}

//------------------------------------------------------------------------------------------
// BBbar from PYTHIA
//------------------------------------------------------------------------------------------

TChain* chain_bbbar = new TChain("ntp2");
// PYTHIA cross-section = 0.000734778 mb
// One "run" has 1.189e+06 generated events = 64.734e+09 MinBias p+p events.
// So we need 147.5 runs if we believe pythia cross-section
// But bbbar cross-section in PYTHIA seems to be 3.22271 +- 0.0388839 (was 2.66) times smaller than one measured by PHENIX
// So we need 475.5 runs
// The file below has 148 runs and can be used if we believe PYTHIA cross-section
chain_bbbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_bbbar/bbbar_9550B_ppevents.root"); 
// The file below has 114 runs 
//chain_bbbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_bbbar/bbbar_7350B_ppevents.root"); 
//
// Files 5, 6, 7 contain 100 runs, file 8 contains 93 runs, file 88 contains 28 runs.
// One run has 1.19143e+06 generated events
// all files have 476 runs
chain_bbbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_bbbar/bbbar_5.root"); 
chain_bbbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_bbbar/bbbar_6.root"); 
chain_bbbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_bbbar/bbbar_7.root"); 
chain_bbbar->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_bbbar/bbbar_88.root"); 

// Single bottom normalization.
// PYTHIA cross-section = 0.00073482mb
// FONLL cs = 1.87 +0.99 -0.67 mkb
// measured cs = 3.2 +1.2-1.1(stat) +1.4-1.3(sys) [A.Adare et al., Phys.Rev.Lett., 103.082002; arXiv:0903.4851]
// measured dSigma/dY at midrapidity = 0.92 +0.34-0.31(stat) +0.39-0.36(sys) mkb [same]
// measured from dielectroins cs = 3.9 +2.5(stat) +3-2(sys) mkb [A. Adare et al., Phys. Lett. B670, 313 (2009)]
// File 0 has 6.17663e+06 generated p+p events
// Corresponds to 40/0.00073482*6.17663e+06 = 336.23B minbias p+p events
//   10B 0-10% central Au+Au events (Ncoll = 955) correspond to 9550B p+p events (before suppression)
//   Bottom normalization factor = 9550/336.23 = 28.4
double bottom_scale = 28.4;
TChain* chain_bbbar_sngl = new TChain("ntp1");
chain_bbbar_sngl->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_bbbar_norm/bbbar_norm_0.root"); 

cout << "Number of entries in BBbar chain: " << chain_bbbar->GetEntries() << endl;
cout << "Number of entries in BBbar SINGLES chain: " << chain_bbbar_sngl->GetEntries() << endl;

chain_bbbar->SetBranchAddress("mass",&mass);
chain_bbbar->SetBranchAddress("pt",  &pt);
chain_bbbar->SetBranchAddress("eta", &eta);
chain_bbbar->SetBranchAddress("pt1", &pt1);
chain_bbbar->SetBranchAddress("pt2", &pt2);
chain_bbbar->SetBranchAddress("eta1",&eta1);
chain_bbbar->SetBranchAddress("eta2",&eta2);

chain_bbbar_sngl->SetBranchAddress("pt",  &pt_sngl);
chain_bbbar_sngl->SetBranchAddress("eta", &eta_sngl);

// Singles for normalization
for(int j=0; j<chain_bbbar_sngl->GetEntries(); j++){
  chain_bbbar_sngl->GetEvent(j);
  if(j%100000==0) cout << "entry # " << j << " " << endl;
  if(fabs(eta_sngl)<1.0) { hbottom_pt_nosupp->Fill(pt_sngl); }
  double random = rnd->Uniform(0.,1.);
  double myraa = gBottomSuppression->Eval(pt_sngl);
  if(random<=myraa) {  if(fabs(eta_sngl)<1.0) { hbottom_pt->Fill(pt_sngl); } }
}
cout << "Number of entries in bottom pT histogram = " << hbottom_pt->GetEntries() << endl;

hbottom_pt->Scale(1.0/bsizept);  // convert to dN/dpT
hbottom_pt->Scale(bottom_scale); // scale up to 10B most central AuAu according to cross-section
  // normalize PYTHIA to measured bottom
  fnorm_bottom->SetParameter(0,1.);
  gBottomAuAu->Fit(fnorm_bottom,"QRN0","",fitstart,fitstop);
  double norm_bottom = fnorm_bottom->GetParameter(0);
  cout << "bottom normalization = " << norm_bottom << endl;

hbottom_pt_nosupp->Scale(1.0/bsizept);  // convert to dN/dpT
hbottom_pt_nosupp->Scale(bottom_scale); // scale up to 10B most central AuAu according to cross-section
  fnorm_bottom_nosupp->SetParameter(0,1.);
  ppgraph_stat_bottom->Fit(fnorm_bottom_nosupp,"QRN0","",fitstart,fitstop);
  double norm_bottom_nosupp = fnorm_bottom_nosupp->GetParameter(0);
  cout << "bottom normalization (no suppression) = " << norm_bottom_nosupp << endl;
 
  // normalize PYTHIA to measured Heavy Flavor
  hhf_pt = (TH1D*)hbottom_pt->Clone("hhf_pt");
  hhf_pt->SetName("hhf_pt");
  hhf_pt->Add(hcharm_pt);
    fnorm_hf->SetParameter(0,1.);
    gHFAuAu->Fit(fnorm_hf,"QRN0","",fitstart,fitstop);
    double norm_hf = fnorm_hf->GetParameter(0);
    cout << "HF normalization = " << norm_hf << endl;

  // HF no suppression
  hhf_pt_nosupp = (TH1D*)hbottom_pt_nosupp->Clone("hhf_pt_nosupp");
  hhf_pt_nosupp->SetName("hhf_pt_nosupp");
  hhf_pt_nosupp->Add(hcharm_pt_nosupp);
    fnorm_hf_nosupp->SetParameter(0,1.);
    ppgraph_stat->Fit(fnorm_hf_nosupp,"QRN0","",fitstart,fitstop);
    double norm_hf_nosupp = fnorm_hf_nosupp->GetParameter(0);
    cout << "HF normalization (no suppression) = " << norm_hf_nosupp << endl;

// invariant mass distributions for BBbar
for(int j=0; j<chain_bbbar->GetEntries()*eideff*eideff; j++){
  chain_bbbar->GetEvent(j);
  if(j%20000==0) cout << "entry # " << j << endl;
  if(pt1>ptcut && pt2>ptcut && fabs(eta1)<1.0 && fabs(eta2)<1.0) {
    double random1 = rnd->Uniform(0.,1.);
    double myraa1 = gBottomSuppression->Eval(pt1);
    double random2 = rnd->Uniform(0.,1.);
    double myraa2 = gBottomSuppression->Eval(pt2);
    if(random1<=myraa1 && random2<=myraa2) {
      int mybin = -1;
      for(int i=0; i<nbins; i++) { if(pt<binlim[i+1]) { mybin = i; break; } }
      if(mybin>=0 && mybin<nbins) { (hbottom[mybin])->Fill(mass); }
      (hbottom[nbins])->Fill(mass);
    }
  }
}


//-----------------------------------------------------------------------
//  DY from PYTHIA
//----------------------------------------------------------------------
TChain* chain_dy = new TChain("ntp2");
// PYTHIA cross-section = 0.000118202 mb
// so, one generated event corresponds to 40/0.000118202 = 338409.5 MinBias p+p events
// one run has 0.465274e+06 generated events
// so, 10B 0-10% central AuAu events (9550B p+p min.bias events) correspond to 60.65 runs; 
// the file below has 61 runs.
// do we need to scale up like charm and bottom?
chain_dy->Add("/phenix/hhj/lebedev/sphenix/phpythia6/ntp_dy/dy_9550B_ppevents.root"); 

cout << "Number of entries in DY chain: " << chain_dy->GetEntries() << endl;

chain_dy->SetBranchAddress("mass",&mass);
chain_dy->SetBranchAddress("pt",  &pt);
chain_dy->SetBranchAddress("eta", &eta);
chain_dy->SetBranchAddress("pt1", &pt1);
chain_dy->SetBranchAddress("pt2", &pt2);
chain_dy->SetBranchAddress("eta1",&eta1);
chain_dy->SetBranchAddress("eta2",&eta2);

// Invariant mass distributions for DY
for(int j=0; j<chain_dy->GetEntries()*eideff*eideff; j++){
  chain_dy->GetEvent(j);
  if(j%20000==0) cout << "entry # " << j << endl;
  if(pt1>ptcut && pt2>ptcut && fabs(eta1)<1.0 && fabs(eta2)<1.0) { 
    int mybin = -1;
    for(int i=0; i<nbins; i++) { if(pt<binlim[i+1]) { mybin = i; break; } }
    if(mybin>=0 && mybin<nbins) { (hdy[mybin])->Fill(mass); }
    (hdy[nbins])->Fill(mass);
  }
}



//----------------------------------------------------------------------
//  Write out
//----------------------------------------------------------------------

TFile* fout = new TFile(ofname.c_str(),"RECREATE");

  TH1D* hhcharm[nbins];
  TH1D* hhcharm_ckin3_4[nbins];
  TH1D* hhbottom[nbins];
  TH1D* hhdy[nbins];
  for(int i=0; i<nbins+1; i++) {
    sprintf(hname,"hhbottom_%d",i); hhbottom[i]  = (TH1D*)(hbottom[i])->Clone(hname);
    sprintf(hname,"hhcharm_%d",i);  hhcharm[i]   = (TH1D*)(hcharm[i])->Clone(hname);
    sprintf(hname,"hhcharm_ckin3_4_%d",i);  hhcharm_ckin3_4[i]   = (TH1D*)(hcharm_ckin3_4[i])->Clone(hname);
    sprintf(hname,"hhdy_%d",i);     hhdy[i]      = (TH1D*)(hdy[i])->Clone(hname);
  }

  TH1D* hhbottom_pt  = (TH1D*)hbottom_pt->Clone("hhbottom_pt");
  TH1D* hhcharm_pt  = (TH1D*)hcharm_pt->Clone("hhcharm_pt");
  TH1D* hhhf_pt = (TH1D*)hhbottom_pt->Clone("hhhf_pt");
  hhhf_pt->Add(hhcharm_pt);
  hhcharm_pt->SetLineColor(kRed+2);
  hhbottom_pt->SetLineColor(kBlue+1);
  hhhf_pt->SetLineColor(kBlack);

  TH1D* hhbottom_pt_nosupp  = (TH1D*)hbottom_pt_nosupp->Clone("hhbottom_pt_nosupp");
  TH1D* hhcharm_pt_nosupp  = (TH1D*)hcharm_pt_nosupp->Clone("hhcharm_pt_nosupp");
  TH1D* hhhf_pt_nosupp = (TH1D*)hhbottom_pt_nosupp->Clone("hhhf_pt_nosupp");
  hhhf_pt_nosupp->Add(hhcharm_pt_nosupp);
  hhcharm_pt_nosupp->SetLineColor(kRed+2);
  hhbottom_pt_nosupp->SetLineColor(kBlue+1);
  hhhf_pt_nosupp->SetLineColor(kBlack);


  //TGraph *ggFracFONLL_b  = new TGraph(Ndata,XR,YR); ggFracFONLL_b->SetName("ggFracFONLL_b");
  TGraph *ggFrac_c = new TGraph(21,ratcbpt,fracc); ggFrac_c->SetName("ggFrac_c");
  TGraph *ggFrac_b = new TGraph(21,ratcbpt,ratcb); ggFrac_b->SetName("ggFrac_b");
  TGraphAsymmErrors *ggCharmAuAu   = new TGraphAsymmErrors(28,xau1,ycau1,0,0,eycau1dn,eycau1up); ggCharmAuAu->SetName("ggCharmAuAu");
  TGraphAsymmErrors *ggBottomAuAu  = new TGraphAsymmErrors(28,xau1,ybau1,0,0,eybau1dn,eybau1up); ggBottomAuAu->SetName("ggBottomAuAu");
  TGraphAsymmErrors *ggHFAuAu  = new TGraphAsymmErrors(28,xau1,yau1,0,0,eyau1dn,eyau1up); ggHFAuAu->SetName("ggHFAuAu");
  TGraphAsymmErrors *ggCharmAuAuSysErr   = new TGraphAsymmErrors(28,xau1,ycau1,erpt,erpt,eycau1dnsys,eycau1upsys); ggCharmAuAuSysErr->SetName("ggCharmAuAuSysErr");
  TGraphAsymmErrors *ggBottomAuAuSysErr  = new TGraphAsymmErrors(28,xau1,ybau1,erpt,erpt,eybau1dnsys,eybau1upsys); ggBottomAuAuSysErr->SetName("ggBottomAuAuSysErr");
  TGraphAsymmErrors *ggHFAuAuSysErr      = new TGraphAsymmErrors(28,xau1,yau1, erpt,erpt,eyau1dnsys,eyau1upsys); ggHFAuAuSysErr->SetName("ggHFAuAuSysErr");
  TGraphAsymmErrors* ggrRAA_ppg077 = new TGraphAsymmErrors(28,ppg077_pt,ppg077_raa,0,0,ppg077_staterdn,ppg077_staterup); ggrRAA_ppg077->SetName("ggrRAA_ppg077");
  TGraphAsymmErrors* ggrRAA_ppg077SysErr = new TGraphAsymmErrors(28,ppg077_pt,ppg077_raa,erpt2,erpt2,ppg077_syserdn,ppg077_syserup); ggrRAA_ppg077SysErr->SetName("ggrRAA_ppg077SysErr");

  TGraph *ggRAA_charm  = new TGraph(9,pt182,raa182charm); ggRAA_charm->SetName("ggRAA_charm");
  TGraph *ggRAA_bottom = new TGraph(9,pt182,raa182bottom); ggRAA_bottom->SetName("ggRAA_bottom");
  TGraphAsymmErrors *ggRAASysErr_charm  = new TGraphAsymmErrors(6,pt182orig,raa182charmorig, 0,0,raa182charm_erdn, raa182charm_erup); ggRAASysErr_charm->SetName("ggRAASysErr_charm");
  TGraphAsymmErrors *ggRAASysErr_bottom = new TGraphAsymmErrors(6,pt182orig,raa182bottomorig,0,0,raa182bottom_erdn,raa182bottom_erup); ggRAASysErr_bottom->SetName("ggRAASysErr_bottom");

  ggFrac_b->SetLineColor(kMagenta);
  ggFrac_b->SetLineWidth(2);
//  ggFracFONLL_b->SetLineColor(kBlack);
//  ggFracFONLL_b->SetLineWidth(2);

  ggHFAuAu->SetMarkerStyle(20);
  ggHFAuAu->SetMarkerColor(kBlack);
  ggBottomAuAu->SetMarkerStyle(20);
  ggBottomAuAu->SetMarkerColor(kBlue);
  ggBottomAuAu->SetLineColor(kBlue);
  ggCharmAuAu->SetMarkerStyle(20);
  ggCharmAuAu->SetMarkerColor(kRed);
  ggCharmAuAu->SetLineColor(kRed);

  ggHFAuAuSysErr->SetFillColor(0);
  ggHFAuAuSysErr->SetMarkerStyle(6);
  ggHFAuAuSysErr->SetMarkerSize(0.5);
  ggHFAuAuSysErr->SetMarkerColor(kBlack);
  ggHFAuAuSysErr->SetLineColor(kBlack);
  ggHFAuAuSysErr->SetLineWidth(2);
  ggHFAuAuSysErr->SetFillColor(kGray+1);

  ggrRAA_ppg077SysErr->SetFillColor(0);
  ggrRAA_ppg077SysErr->SetMarkerStyle(6);
  ggrRAA_ppg077SysErr->SetMarkerSize(0.5);
  ggrRAA_ppg077SysErr->SetMarkerColor(kBlack);
  ggrRAA_ppg077SysErr->SetLineColor(kBlack);
  ggrRAA_ppg077SysErr->SetLineWidth(2);
  ggrRAA_ppg077SysErr->SetFillColor(kGray+1);

  ggrRAA_ppg077->SetLineWidth(2);
  ggrRAA_ppg077->SetMarkerStyle(20);
  ggrRAA_ppg077->SetMarkerSize(1.0);
  ggrRAA_ppg077->SetMarkerColor(kBlack);

  ggRAASysErr_charm->SetFillColor(kGreen+2);
  ggRAASysErr_charm->SetFillStyle(3001);
  ggRAA_charm->SetLineWidth(3);
  ggRAA_charm->SetLineColor(kGreen+2);
  ggRAASysErr_bottom->SetFillColor(kBlue);
  ggRAASysErr_bottom->SetFillStyle(3002);
  ggRAA_bottom->SetLineWidth(3);
  ggRAA_bottom->SetLineColor(kBlue);

//  ggFracFONLL_b->Write();
  ggFrac_c->Write();
  ggFrac_b->Write();
  ggRAA_charm->Write();
  ggRAA_bottom->Write();
  ggRAASysErr_charm->Write();
  ggRAASysErr_bottom->Write();
  ggCharmAuAu->Write();
  ggBottomAuAu->Write();
  ggHFAuAu->Write();
  ggCharmAuAuSysErr->Write();
  ggBottomAuAuSysErr->Write();
  ggHFAuAuSysErr->Write();
  ggrRAA_ppg077SysErr->Write();
  ggrRAA_ppg077->Write();

  TF1 ffnorm_bottom = TF1(*fnorm_bottom);
  TF1 ffnorm_charm = TF1(*fnorm_charm);
  TF1 ffnorm_hf = TF1(*fnorm_hf);
    ffnorm_bottom.SetName("ffnorm_bottom");
    ffnorm_charm.SetName("ffnorm_charm");
    ffnorm_hf.SetName("ffnorm_hf");
    ffnorm_bottom.Write();
    ffnorm_charm.Write();
    ffnorm_hf.Write();
  TF1 ffnorm_bottom_nosupp = TF1(*fnorm_bottom_nosupp);
  TF1 ffnorm_charm_nosupp = TF1(*fnorm_charm_nosupp);
  TF1 ffnorm_hf_nosupp = TF1(*fnorm_hf_nosupp);
    ffnorm_bottom_nosupp.SetName("ffnorm_bottom_nosupp");
    ffnorm_charm_nosupp.SetName("ffnorm_charm_nosupp");
    ffnorm_hf_nosupp.SetName("ffnorm_hf_nosupp");
    ffnorm_bottom_nosupp.Write();
    ffnorm_charm_nosupp.Write();
    ffnorm_hf_nosupp.Write();


  TF1 mymybotfrac = TF1(*mybotfrac);
  mymybotfrac.SetName("mymybotfrac");
  mymybotfrac.SetLineColor(kGreen+2);
  mymybotfrac.Write();

  TF1 ppythia_botfrac = TF1(*pythia_botfrac);
  ppythia_botfrac.SetName("ppythia_botfrac");
  ppythia_botfrac.SetLineColor(kGreen+2);
  ppythia_botfrac.Write();


  TGraphAsymmErrors* ttcharm = new TGraphAsymmErrors(*tcharm);
  TGraphAsymmErrors* ttbottom = new TGraphAsymmErrors(*tbottom);
    ttcharm->SetFillColor(kGreen+2);
    ttcharm->SetFillStyle(3001);
    ttcharm->SetLineWidth(1);
    ttcharm->SetName("ttcharm");
    ttcharm->Write();
    ttcharm->SetLineColor(kGreen+2);
    ttbottom->SetFillColor(kBlue);
    ttbottom->SetFillStyle(3002);
    ttbottom->SetLineWidth(1);
    ttbottom->SetLineColor(kBlue);
    ttbottom->SetName("ttbottom");
    ttbottom->Write();

  TGraphAsymmErrors *ggfonll  = new TGraphAsymmErrors(*gfonll);
    ggfonll->SetLineColor(kBlack);
    ggfonll->SetFillColor(kGray);
    ggfonll->SetLineWidth(3.0);
    ggfonll->SetFillStyle(3001);
    ggfonll->SetName("ggfonll");
    ggfonll->Write();
  TGraph *ggfonllup  = new TGraph(*gfonllup);
    ggfonllup->SetLineColor(kBlue+2);
    ggfonllup->SetLineWidth(2.0);
    ggfonllup->SetName("ggfonllup");
    ggfonllup->Write();

  TGraphAsymmErrors* bbfrac_cent1 = new TGraphAsymmErrors(*bfrac_cent1);
    bbfrac_cent1->SetLineColor(kOrange+8);
    bbfrac_cent1->SetFillColor(kOrange+8);
    bbfrac_cent1->SetLineWidth(3.0);
    bbfrac_cent1->SetFillStyle(3002);
    bbfrac_cent1->SetName("bbfrac_cent1");
    bbfrac_cent1->Write();

  TGraphErrors *ttRaadata = new TGraphErrors(*tRaadata);
  TGraphErrors *ttRaadata_sys = new TGraphErrors(*tRaadata_sys);
    ttRaadata_sys->SetMarkerStyle(21);
    ttRaadata_sys->SetMarkerSize(1.0);
    ttRaadata_sys->SetMarkerColor(kGray+2);
    ttRaadata_sys->SetLineColor(kGray+2);
    ttRaadata_sys->SetFillStyle(0);
    ttRaadata_sys->SetName("ttRaadata_sys");
    ttRaadata_sys->Write();
    ttRaadata_sys->SetFillStyle(0);
    ttRaadata->SetMarkerStyle(21);
    ttRaadata->SetMarkerSize(1.0);
    ttRaadata->SetMarkerColor(kGray+2);
    ttRaadata->SetName("ttRaadata");
    ttRaadata->Write();

  TGraph* mymytcharm = new TGraph(*mytcharm);
    mymytcharm->SetLineWidth(3);
    mymytcharm->SetLineColor(kGreen+2);
    mymytcharm->SetName("mymytcharm");
    mymytcharm->Write();
  TGraph* mymytbottom = new TGraph(*mytbottom);
    mymytbottom->SetLineWidth(3);
    mymytbottom->SetLineColor(kBlue);
    mymytbottom->SetName("mymytbottom");
    mymytbottom->Write();

  TGraph* grgr_mysupp_charm = new TGraph(*gr_mysupp_charm);
    grgr_mysupp_charm->SetLineWidth(3);
    grgr_mysupp_charm->SetLineColor(kGreen+2);
    grgr_mysupp_charm->SetName("grgr_mysupp_charm");
    grgr_mysupp_charm->Write();
  TGraph* grgr_mysupp_bottom = new TGraph(*gr_mysupp_bottom);
    grgr_mysupp_bottom->SetLineWidth(3);
    grgr_mysupp_bottom->SetLineColor(kGreen+2);
    grgr_mysupp_bottom->SetName("grgr_mysupp_bottom");
    grgr_mysupp_bottom->Write();

  TGraphErrors *pppgraph_stat  = new TGraphErrors(Ndatapp,XX,YY,0,Estat);
  TGraphErrors *pppgraph_stat_charm  = new TGraphErrors(Ndatapp,XX,YYC,0,ECstat);
  TGraphErrors *pppgraph_stat_bottom  = new TGraphErrors(Ndatapp,XX,YYB,0,EBstat);
  TGraphErrors *pppgraph_sys  = new TGraphErrors(Ndatapp,XX,YY,0,Esys);
  pppgraph_stat->SetMarkerStyle(24);
  pppgraph_stat_charm->SetMarkerStyle(24);
  pppgraph_stat_charm->SetMarkerColor(kRed);
  pppgraph_stat_charm->SetLineColor(kRed);
  pppgraph_stat_bottom->SetMarkerStyle(24);
  pppgraph_stat_bottom->SetMarkerColor(kBlue);
  pppgraph_stat_bottom->SetLineColor(kBlue);
  pppgraph_stat->SetName("pppgraph_stat");
  pppgraph_stat->Write();
  pppgraph_stat_charm->SetName("pppgraph_stat_charm");
  pppgraph_stat_charm->Write();
  pppgraph_stat_bottom->SetName("pppgraph_stat_bottom");
  pppgraph_stat_bottom->Write();
  pppgraph_sys->SetName("pppgraph_sys");
  pppgraph_sys->Write();

TGraph* ggr_mybotfrac = new TGraph(*gr_mybotfrac);
ggr_mybotfrac->SetLineWidth(2);
ggr_mybotfrac->SetLineColor(kGreen+2);
ggr_mybotfrac->SetName("ggr_mybotfrac");
ggr_mybotfrac->Write();

TGraph* ggr_pythia_botfrac = new TGraph(*gr_pythia_botfrac);
ggr_pythia_botfrac->SetLineWidth(2);
ggr_pythia_botfrac->SetLineColor(kGreen+2);
ggr_pythia_botfrac->SetName("ggr_pythia_botfrac");
ggr_pythia_botfrac->Write();


fout->Write();
fout->Close();


 return 0;
}

