#include <iomanip>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

 
// #include <TROOT.h>
// #include <TFile.h>
// #include <TTree.h>
// #include <TGMsgBox.h>
// #include <TCanvas.h>
// #include <TGraph.h>
// #include <TString.h>
// #include <TText.h>
// #include <TApplication.h>
// #include <TStyle.h>
// #include <TSystem.h>
// #include <TGClient.h>
// #include <TGWindow.h>
// #include <TH1.h>
// #include <TH1F.h>
// #include <TH1D.h>
// #include <TProfile.h>
// #include <TF1.h>
// #include <TH2.h>
// #include <TH3.h>
// #include <TMath.h>
// #include <TSpectrum.h>
// #include "TVirtualFitter.h"
// #include <TSystemDirectory.h>
// #include <TSystemFile.h>

//#ifndef __CINT__

#include <fileEventiterator.h>
#include <Event.h>
#include <EventTypes.h>
#include <PHTimeStamp.h>
#include <packetConstants.h>
#include <packet.h>
#include <packet_hbd_fpgashort.h>

//#endif


#include "hcalUtil.h"


hcalUtil    * hcalUtil    :: single = 0;
hLabHelper  * hLabHelper  :: single = 0;
hcalHelper  * hcalHelper  :: single = 0;
hcalTree    * hcalTree    :: single = 0;
tileHelper  * tileHelper  :: single = 0;
tileTree    * tileTree    :: single = 0;

using namespace std;
using namespace TMath;

TCanvas * fitFunc;

TCanvas * scCalibDisplay;


//   ************************************************************************************


//#endif /* _CINT_ */

//  cosmic runs of interest
static const int   cosmicruns = 12;
int   cosmicRN[cosmicruns]    = {212, 215, 216, 218, 223, 337, 338, 339, 405, 406, 229, 240}; 

//  LED calibration and PEDESTALS
static const int   pedruns    = 1;
int   pedcalibRN[pedruns]     = {646};
static const int   ledruns    = 1;
int   ledcalibRN[ledruns]     = {647};

//  -----------------------------------------------------------------------------------------------

//  Unipolar pulse (0.3164*pow(x,4)*exp(-x*1.5) - Unity integral, peak ~1/3 of integral)
Double_t par0[]       = {     1.,  (Double_t)NSAMPLES/2, 3., 2.,   PEDESTAL,   0. };
Double_t par0Max[]    = {  -100.,  (Double_t)NSAMPLES,   4., 5.,   50000.,     0.3};
//Double_t par0Min[]    = {    1., RISETIME-1,            3., 1.,   1950.,    -0.2};
Double_t par0Min[]    = {-50000.,  0.,                   2., 0.,  -50000.,    -0.3};


TString             runId;

//   The next few functions are to help processing data from HCal Lab recorded via HBD electronics

// **************************************************************************

Double_t ps( Double_t *chadc, Double_t *par)
{
  Double_t x = chadc[0];

  Double_t mu = par[0];
  Double_t g = par[1];
  Double_t sigma = par[2];
  Double_t norm = par[3];

  Double_t sum = 0.0;
  for (Double_t n = 1.0; n < 100.0; n++ ) {
    sum += TMath::Poisson(n,mu)*TMath::Gaus(x,n*g,sigma,kTRUE);
  }

  return norm*sum;
}
// **************************************************************************
Int_t  calibPeaks(0), calibMode (0); Double_t spCalib(10.);
// **************************************************************************
//  mode   0  - peaks uncorrelated (unconstrained)
//  mode   1  - peaks equidistant
//  assume background is linear what is most likely not true ....
//  par[0]   background constant 
//     [1]   background slope 
//     [2]   background quadratic term  
//  assume the signal is the sequence of equally spaced gaussian peaks
//  par[3]   peak to peak separation 
//     [4]   normalization constant for the first peak (pedestal)
//     [5]   location of the first (pedestal) peak
//     [6]   rms of the first peak - at this stage we will allow it to vary peak-to-peak
//     then 3 parameters per signal peak (normalization, mean and RMS)
Double_t sipmPeaks(Double_t *x, Double_t *par) {
   Double_t result  = par[0] + par[1]*x[0] + par[2]*x[0]*x[0];
   //  add pedestal peak first
   result          += par[4]*TMath::Gaus(x[0],par[5],par[6]); 
   for (Int_t p=1; p<calibPeaks; p++) {
      Double_t norm  = par[4 + p*3];
      Double_t mean  = par[5 + p*3];
      if(calibMode) {
	mean  = (Int_t)((mean+spCalib/2)/spCalib)*par[3];
      } 
      Double_t sigma = par[6 + p*3];
      result += norm*TMath::Gaus(x[0],mean,sigma);
   }
   return result;
   ;}


// **************************************************************************
// do single cell calibration using low end data stored in rv_# and fm_# channel histograms 
// the assumption - we have beautiful picture with at least 20 peaks !!!!!!
//  mode   0  - peaks uncorrelated (unconstrained)
//  mode   1  - peaks equidistant
Int_t sipmCalib(TString & hName, Int_t mode){
  TH1F * data = (TH1F*)(gROOT->FindObject(hName));
  if(!data){
    cout<<"spmCalib  Data Histogramm "<<hName<<"  not found"<<endl;
    return 0;
  }
  cout<<hName<<"  calibration mode "<<mode<<endl;
  calibMode = mode;
  Int_t      maxPeaks(20);        //  maximum number of peaks we have chance to resolve
  Double_t   range(150.);         //  range of amplitudes for peak search
  //  second degree polinomial background
  Double_t   bckgr0(100.), bckgr1(-1./range),  bckgr2(0.); 
  Double_t   pNorm(1000.), pLoc(0.),           pRMS(1.);
  Double_t   par[4+maxPeaks*3];   //  backgrownd, pedestal and nPeaks of gaussian peaks with constant peak-to-peak separation
  //  initialization
  par[0] = bckgr0;
  par[1] = bckgr1;
  par[2] = bckgr2;
  par[3] = spCalib;            //  peak-to-peak separation
  par[4] = pNorm;              //  normalization for pedestal peak
  par[5] = pLoc;               //  pedestal mean
  par[6] = pRMS;               //  pedestal rms
  //  all other peaks are initially positioned equidistantly
  for (int p=1; p<= maxPeaks; p++) {
      par[4 + p*3] = pNorm;                       //  norm of the peak p
      par[5 + p*3] = par[3]*p;                    //  mean of the peak p
      par[6 + p*3] = pRMS;                        //  rms  of the peak p
  }
  TH1F * dataCl  = (TH1F*)(data->Clone("dataCl")); 
  dataCl        -> SetAxisRange(0., range);
  TH1F * dataCl2 = (TH1F*)data->Clone("dataCl2");
  dataCl2       -> SetAxisRange(0., range);
  TString cTitle  = "Peaks from "; cTitle += hName;
  TCanvas * peaks = new TCanvas("peaks",cTitle,10,10,1000,900);
  peaks         -> Divide(1,2);
  peaks         -> cd(1);
  dataCl        -> Draw();

  //  Use TSpectrum to find the peak candidates
  TSpectrum *s = new TSpectrum(maxPeaks);
  s->SetResolution(2.);
  Int_t nfound = s->Search(dataCl, 1.5, "nobackground    ", 0.10);
  printf("TSpectrum:  Found %d candidate peaks to fit\n",nfound);

  //Estimate background using TSpectrum::Background
  TH1 *hb = s->Background(dataCl, 20, "same");
  if (hb)   peaks->Update();
  //  if only one or no peaks  found (only pedestals) - we are done
  if(nfound<=1) {
    cout<<"sipmCalib  One or No peaks were found - EXITING"<<endl;
    calibPeaks = nfound;
    
  }  else {
    peaks           -> cd(2);

    //  function to estimate linear background
    //  TF1 *fline       = new TF1("fline","pol1",0.,range);
    //  function to estimate quadratic background
    TF1 *fline       = new TF1("fline","pol2",0.,range);

    dataCl2          -> Fit("fline","qn");
    //Loop on all found peaks. Eliminate peaks at the background level
    par[0]           = fline->GetParameter(0);
    par[1]           = fline->GetParameter(1);
    par[2]           = fline->GetParameter(2);
    //  cout<<"Line par "<<par[0]<<"   "<<par[1]<<"  "<<par[2]<<endl;
    Float_t * xpeaks  = s->GetPositionX();
    // peak ordering
    Int_t pindex[nfound];
    TMath::Sort(nfound, xpeaks, pindex, kFALSE);
    //  peaks now  ordered, let's now find the peak to peak separation
    Double_t sep[nfound-1];
    //    Int_t    sepind[nfound-1];
    //  pedestal peak in my definition == what is left of a sygnal after pedestal subtraction - it is definitely shifted 
    for (int ip = 1; ip<nfound; ip++){
      sep[ip-1] = xpeaks[pindex[ip]];
      if(ip>1)  sep[ip-1] -= xpeaks[pindex[ip-1]];
      cout<<"Separation  "<<ip<<" val "<<sep[ip-1]<<endl;
    } 
    //    TMath::Sort(nfound-1, sep, sepind, kFALSE);
    //  zero approximation for peak0-to-peak separation
    Int_t cont(0); Double_t avsep(0.);
    for (int ip = 0; ip < nfound-1; ip++){
      if(ip==0) {
	//	avsep = sep[sepind[ip]];
	avsep = sep[ip];
	cont ++;
      } else {
	//	if(abs((avsep- sep[sepind[ip]])/(avsep+ sep[sepind[ip]]))<0.2) {
	if(abs((avsep- sep[ip])/(avsep+ sep[ip]))<0.2) {
	  avsep = (avsep*cont+sep[ip])/(cont+1);
	  cont++;
	}
      }
      cout<<"ip  "<<ip<<" cont "<<cont<<"  avsep "<<avsep<<" var "<<(ip!=0? (avsep- sep[ip])/(avsep+ sep[ip])  : 0.)<<endl;
    }
    cout<<"Zero approximation to sSingle Pixel Calib = "<<avsep<<"   Contributors "<<cont<<endl; 
    spCalib    = avsep;
    par[3]     = avsep;
    calibPeaks = 0;
    for (int p=0; p<nfound; p++) {
      Double_t xp    = xpeaks[pindex[p]];
      Int_t    bin   = dataCl2->GetXaxis()->FindBin(xp);
      Double_t yp    = dataCl2->GetBinContent(bin);
      cout<<"Testing Peak (LFit) "<<p<<" index "<<pindex[p]<<"  xp "<<xp<<"  bin "<<bin<<"  yP "<<yp<<"  BCKG "<<fline->Eval(xp)<<endl;
      //  extract data for this bin from background shape
      Double_t bckgr = hb->GetBinContent(bin);
      cout<<"Testing Peak (TSp ) "<<p<<"  xp "<<xp<<"  bin "<<bin<<"  Bcgrd "<<bckgr<<" Signal "<<yp-bckgr<<endl;
      if (yp-TMath::Sqrt(yp) < bckgr) continue;
      par[4 + calibPeaks*3] = yp;
      par[5 + calibPeaks*3] = xp;
      par[6 + calibPeaks*3] = (p==0?  pRMS  : 2.);
      calibPeaks++;
    }
  }
  printf("Found %d useful peaks to fit\n",calibPeaks);
  printf("Now fitting: Be patient\n");
  TF1 *fit = new TF1("fit",&sipmPeaks,0,range,4+calibPeaks*3);  // first peak is the pedestal
  //we may have more than the default 25 parameters
  TVirtualFitter::Fitter(dataCl2,10+calibPeaks*3);
  fit->SetParameters(par);
  //  if mode!=0 we fix all location parameters to what came from TSpectrum
  if(mode) for(int ip=0; ip<calibPeaks; ip++) {
      fit->FixParameter(5+ip*3, par[5+ip*3]);
      fit->SetParLimits(3, 0.5*par[3], 1.5*par[3]);
    }  else fit->FixParameter(3, par[3]);
  //  in all cases we set max RMS equal to pps/2.
  for(int ip=0; ip<calibPeaks; ip++) fit->SetParLimits(6+ip*3, 0.5, par[3]/2.);
  
  fit->SetNpx(1000);
  dataCl2->Fit("fit");
  fit->GetParameters(par);

  char p5[10], p6[10], p3[10];
  sprintf(p5, "%.2f ", par[5]);
  sprintf(p6, "%.2f ", par[6]);
  sprintf(p3, "%.2f ", par[3]);

  TString ped = "Pedestal at     "; ped += p5; ped += "  RMS = "; ped += p6;
  TString cal = "Pixel Calibtion "; cal += p3; cal += " ADC counts per pixel";
  TText label;
  Double_t  ymax = data-> GetMaximum();
  label.DrawText(30, ymax*0.8,ped);
  label.DrawText(30, ymax*0.6,cal);
  return nfound;
}

// **************************************************************************
Double_t AsymToX(Double_t ax){
  return 12.5*(1 + ax/0.19); 
}


// **************************************************************************

Double_t signalShape(Double_t *x, Double_t * par){
  Double_t signal(0.), pedestal(0.);
  //  cout<<"signalShape x="<<x[0]<<" (0)="<<par[0]<<" (1)="<<par[1]<<" (2)="<<par[2]<<" (3)="<<par[3]<<" (4)="<<par[4]<<" (5)="<<par[5]<<endl;
  //  if(x[0]<par[1])  return PEDESTAL;
  pedestal = par[4]+x[0]*par[5];
  if(x[0]<par[1])   return pedestal;
  //  signal contribution
  //  cout<<"(0) "<<pow((x[0]-par[1]),par[2])<<endl;
  //  cout<<"(1) "<<exp(-(x[0]-par[1])*par[3])<<endl;
  // Double_t a0 = par[0]; 
  // Double_t a1 = pow((x[0]-par[1]),par[2]);
  // Double_t a2 = exp(-(x[0]-par[1])*par[3]);
  // Double_t a  = a0*a1*a2;
  // //  cout<<a0<<"   "<<a1<<"  "<<a2<<"  "<<a<<endl;
  // signal   = a;
  // return a;
  signal   = par[0]*pow((x[0]-par[1]),par[2])*exp(-(x[0]-par[1])*par[3]);
  //  cout<<"  X  "<<x[0]<<"  signal "<<signal<<"  pedestal  "<<pedestal<<endl;
  return   signal+pedestal;
}

// **************************************************************************

// Double_t shape0(Double_t *x, Double_t * par){
//   return par[0]*pow(x[0],4.)*(exp(-x[0]*16./fN));    //  fN is set to (float)NSAMPLES
// }

// **************************************************************************

Double_t erfunc(Double_t *x, Double_t * par){
  //  Double_t erfcont =  par[3]*(TMath::Erf((x[0]-par[0])/par[1])+1.);  
  //  Double_t expcont =  TMath::Exp(par[2]*x[0]);
  return par[3]*(TMath::Erf((x[0]-par[0])/par[1])+1.)*TMath::Exp(par[2]*x[0]);
}
// **************************************************************************

Double_t erf_g(Double_t *x, Double_t * par){
  float  g = par[0]/(sqrt(6.2830)*par[2])*TMath::Exp(-(x[0]-par[1])*(x[0]-par[1])/(2.*par[2]*par[2]));
  return g+par[6]*(TMath::Erf((x[0]-par[3])/par[4])+1.)*TMath::Exp(par[5]*x[0]);
}
// **************************************************************************

Double_t powerFun(Double_t *x, Double_t * par){
  //  return par[0]*pow((x[0]),par[1]*pow((par[2]-x[0]),par[3]);
 return par[0]*pow(x[0],par[1])*pow((1-x[0]),par[2]);
 //*pow((0.5-x[0]),par[2]);
}

// **************************************************************************

Double_t power_g(Double_t *x, Double_t * par){
  float  g = par[0]/(sqrt(6.2830)*par[2])*TMath::Exp(-(x[0]-par[1])*(x[0]-par[1])/(2.*par[2]*par[2]));
  float  p = par[3]*pow(x[0],par[4])*pow((1-x[0]),par[5]);
  //*pow((0.5-x[0]),par[5]);
  return g+p;
}



//   ====================================================================================
//   ************************************************************************************
//   ====================================================================================


hcalUtil::hcalUtil(){
  char * s = getenv("BASE_DIR");
  TString basedir = (s==0||strlen(s)==0)? "/home/kistenev/workarea/" : s;
}

//----------------------------------------------------------------------------
// void hcalUtil::setBaseDir(char * bDir){
//   basedir.Resize(0);
//   basedir += bDir;
// }

//----------------------------------------------------------------------------

float hcalUtil::xPeak(TF1* f, float x1, float x2){
  //  searches for peak position in the TF1 function - looks into derivatives
  float y1 = f->Derivative(x1, 0, 0.002);
  float y2 = f->Derivative(x2, 0, 0.002);
  if(y1*y2>0) {
    return (f->Eval(x1)>f->Eval(x2))? x1 : x2;
  }
  float xx1 = x1;
  float xx2 = x2;
  float dx = fabs(xx2-xx1);
  float x = 0.;
  while(y1*y2<0&&dx>0.001){
    x  = (xx1+xx2)/2.;
    float y = f->Derivative(x, 0, 0.01);
    if(y1*y>0)  xx1 = x; else xx2 = x;
    dx = fabs(xx2-xx1);
  }
  return x;
}

//----------------------------------------------------------------------------

float hcalUtil::width(TF1* f, float x1, float x2, float peak){
  //  find width of the function (erfunc only) !!!
  float yhm = f->Eval(peak)/2;

  float xl = x1;
  float xr = peak;

  float xm = (xr+xl)/2.;
  float ym = f->Eval(xm);
  //  cout<<yhm<<" xl "<< xl<<" xr "<<xr<<" xm "<<xm<<" ym "<<ym<<endl;
  while(abs(ym-yhm)>0.01*yhm && xr-xl>0.001){
    if(ym>yhm) xr = xm; else xl = xm;
    xm = (xr+xl)/2.;
    ym = f->Eval(xm);
    //cout<<"left xl "<<xl<<" xr "<<xr<<" xm "<<xm<<" ym "<<ym<<endl;  
  }
  bool lok = (abs(ym-yhm)>0.1*yhm)? false : true;
  float xml = xm;
  xl = peak;
  xr = x2;
  xm = (xr+xl)/2.;
  ym = f->Eval(xm);
  while(abs(ym-yhm)>0.01*yhm&&xr-xl>0.001){
    if(ym>yhm) xl = xm; else xr = xm;
    xm = (xr+xl)/2.;
    ym = f->Eval(xm);
    //cout<<"right xl "<<xl<<" xr "<<xr<<" xm "<<xm<<" ym "<<ym<<endl;  
  }
  float xmr = xm;
  bool rok = (abs(ym-yhm)>0.1*yhm)? false : true;
  float w   = 0;
  if(lok){
    if(rok) w = xmr-xml; else  w = 2*(peak-xml);
  }  else if (rok){
    w = 2*(xmr-peak);
  } else w = x2-x1;
  return w/2.35;
}

//----------------------------------------------------------------------------

bool hcalUtil::shapeFit(TH1D * proj,  Double_t * pE, float & maxE, Double_t * pG, bool fine, bool MIP){
  TF1 * fE;
  TF1 * fG;
  int    bMax = proj->GetMaximumBin();
  float  pMax = proj->GetBinCenter(bMax);
  bool   erfOK = true;
  TAxis *x;
  x = proj->GetXaxis();
  double xmin = x->GetXmin();
  double xmax = x->GetXmax();
  pE[0] = pMax;
  pE[1] = proj->GetRMS();
  pE[2] = -1./pE[1];
  pE[3] = proj->GetBinContent(bMax)/TMath::Exp(pE[2]*pE[0]);
  fE    = (TF1*)(gROOT->GetListOfFunctions()->FindObject("fE"));
  if(fE) delete fE;
  fE = new TF1("fE",&erfunc,xmin,xmax,4);
  fE->SetParameters(pE);
  proj->Fit("fE","NQL0");
  fE->GetParameters(pE);
  maxE = xPeak(fE,xmin,xmax);
  //  cout<<"Position of Maximum: "<<maxE<<" "<<xmin<<" "<<xmax<<endl;
  if(maxE<=xmin||maxE>=xmax||pE[0]<=xmin/*||pE[0]>=xmax*/) erfOK = false;
  float scale = (fine? 1. : 1.5);
  //    float scale = (fine? 1. : 1);
  pG[0] = proj->GetBinContent(bMax);
  if(erfOK){
    pG[1] =  maxE;
    pG[2] =  width(fE,xmin,xmax,maxE);
  } else {
    pG[1] = proj->GetMean();
    pG[2] = proj->GetRMS();
  }
  fG = (TF1*)(gROOT->FindObject("fG"));
  if(fG) delete fG;
  if(MIP) {
    fG = new TF1("fG","gaus",TMath::Max(xmin,pG[1]-0.3*scale),TMath::Min(xmax,pG[1]+0.075*scale));
    fG->SetParameters(pG);
    fG->SetParLimits(1,TMath::Max(xmin,pG[1]-0.1*scale),TMath::Min(xmax,pG[1]+0.05*scale));
  } else {
    fG = new TF1("fG","gaus",TMath::Max(xmin,pG[1]-1.5*scale),TMath::Min(xmax,pG[1]+1.5*scale));
    fG->SetParameters(pG);
    fG->SetParLimits(1,TMath::Max(xmin,pG[1]-2.*scale),TMath::Min(xmax,pG[1]+2.*scale));
  }
  proj->Fit("fG","NLQR0");
  fG->GetParameters(pG);
  return ((!erfOK||pG[1]<xmin+0.1||pG[1]>xmax-0.05)? false : true);
}

//----------------------------------------------------------------------------

bool hcalUtil::emcFit(TH1D * proj,  bool fine, bool sing, Double_t * pE, float & maxE, Double_t * pG, bool MIP){
  TF1 * fE;
  TF1 * fG;
  int    bMax = proj->GetMaximumBin();
  float  pMax = proj->GetBinCenter(bMax);
  bool   erfOK = true;
  TAxis *x;
  x = proj->GetXaxis();
  double xmin = x->GetXmin();
  double xmax = x->GetXmax();
  pE[0] = pMax;
  pE[1] = proj->GetRMS();
  pE[2] = -1./pE[1];
  pE[3] = proj->GetBinContent(bMax)/TMath::Exp(pE[2]*pE[0]);
  fE    = (TF1*)(gROOT->GetListOfFunctions()->FindObject("fE"));
  if(fE) delete fE;
  fE = new TF1("fE",&erfunc,xmin,xmax,4);
  fE->SetParameters(pE);
  proj->Fit("fE","NQL0");
  fE->GetParameters(pE);
  maxE = xPeak(fE,xmin,xmax);
  //  cout<<"Position of Maximum: "<<maxE<<" "<<xmin<<" "<<xmax<<endl;
  if(maxE<=xmin||maxE>=xmax||pE[0]<=xmin/*||pE[0]>=xmax*/) erfOK = false;
  if(!sing){
    float scale = (fine? 1. : 1.5);
    //    float scale = (fine? 1. : 1);
    pG[0] = proj->GetBinContent(bMax);
    if(erfOK){
      pG[1] =  maxE;
      pG[2] =  width(fE,xmin,xmax,maxE);
    } else {
      pG[1] = proj->GetMean();
      pG[2] = proj->GetRMS();
    }
    fG = (TF1*)(gROOT->FindObject("fG"));
    if(fG) delete fG;
    if(MIP) {
      fG = new TF1("fG","gaus",TMath::Max(xmin,pG[1]-0.3*scale),TMath::Min(xmax,pG[1]+0.075*scale));
      fG->SetParameters(pG);
      fG->SetParLimits(1,TMath::Max(xmin,pG[1]-0.1*scale),TMath::Min(xmax,pG[1]+0.05*scale));
    } else {
      fG = new TF1("fG","gaus",TMath::Max(xmin,pG[1]-1.5*scale),TMath::Min(xmax,pG[1]+1.5*scale));
      fG->SetParameters(pG);
      fG->SetParLimits(1,TMath::Max(xmin,pG[1]-2.*scale),TMath::Min(xmax,pG[1]+2.*scale));
    }
    proj->Fit("fG","NLQR0");
    fG->GetParameters(pG);
  }
  return ((!erfOK||pG[1]<xmin+0.1||pG[1]>xmax-0.05)? false : true);
}

//----------------------------------------------------------------------------

bool hcalUtil::emcFit(TH1D * proj,  bool sing, Double_t * pE, float & maxE, Double_t * pG){
  TF1 * fE;
  TF1 * fG;
  int    bMax = proj->GetMaximumBin();
  float  pMax = proj->GetBinCenter(bMax);
  bool   erfOK = true;
  TAxis *x;
  x = proj->GetXaxis();
  double xmin = x->GetXmin();
  double xmax = x->GetXmax();
  pE[0] = pMax;
  pE[1] = proj->GetRMS();
  pE[2] = -1./pE[1];
  pE[3] = proj->GetBinContent(bMax)/TMath::Exp(pE[2]*pE[0]);
  fE    = (TF1*)(gROOT->GetListOfFunctions()->FindObject("fE"));
  if(fE) delete fE;
  fE = new TF1("fE",&erfunc,xmin,xmax,4);
  fE->SetParameters(pE);
  proj->Fit("fE","QNL0");
  fE->GetParameters(pE);
  maxE = xPeak(fE,xmin,xmax);
  //  cout<<"Position of Maximum: "<<maxE<<" "<<xmin<<" "<<xmax<<endl;
  if(maxE<=xmin||maxE>=xmax||pE[0]<=xmin/*||pE[0]>=xmax*/) erfOK = false;
  if(!sing){
    pG[0] = proj->GetBinContent(bMax);
    if(erfOK){
      pG[1] =  maxE;
      pG[2] = width(fE,xmin,xmax,maxE) ;
    } else {
      pG[1] = proj->GetMean();
      pG[2] = proj->GetRMS();
    }
    fG = (TF1*)(gROOT->FindObject("fG"));
    if(fG) delete fG;
    fG = new TF1("fG","gaus",TMath::Max(xmin,pG[1]-pG[2]),TMath::Min(xmax,pG[1]+pG[2]));
    fG->SetParameters(pG);
    fG->SetParLimits(1,TMath::Max(xmin,pG[1]-pG[2]),TMath::Min(xmax,pG[1]+pG[2]));
    proj->Fit("fG","QNLR0");
    fG->GetParameters(pG);
  }
  return ((!erfOK||pG[1]<xmin+0.1||pG[1]>xmax-0.05)? false : true);
}
//----------------------------------------------------------------------------

bool hcalUtil::emcFit(TH1D * proj,  bool sing, Double_t * pE, float & maxE, Double_t * pG, float minx, float maxx){
  TF1 * fE;
  TF1 * fG;
  int    bMax = proj->GetMaximumBin();
  float  pMax = proj->GetBinCenter(bMax);
  bool   erfOK = true;
  TAxis *x(NULL);
  double xmin(0.);
  double xmax = x->GetXmax();
  xmin = TMath::Max((double)minx,xmin);
  xmax = TMath::Min((double)maxx,xmax);
  pE[0] = pMax;
  pE[1] = proj->GetRMS();
  pE[2] = -1./pE[1];
  pE[3] = proj->GetBinContent(bMax)/TMath::Exp(pE[2]*pE[0]);
  fE    = (TF1*)(gROOT->GetListOfFunctions()->FindObject("fE"));
  if(fE) delete fE;
  fE = new TF1("fE",&erfunc,xmin,xmax,4);
  fE->SetParameters(pE);
  proj->Fit("fE","QNL0");
  fE->GetParameters(pE);
  maxE = xPeak(fE,xmin,xmax);
  //  cout<<"Position of Maximum: "<<maxE<<" "<<xmin<<" "<<xmax<<endl;
  if(maxE<=xmin||maxE>=xmax||pE[0]<=xmin/*||pE[0]>=xmax*/) erfOK = false;
  if(!sing){
    pG[0] = proj->GetBinContent(bMax);
    if(erfOK){
      pG[1] =  maxE;
      pG[2] = width(fE,xmin,xmax,maxE) ;
      //      cout<<pG[0]<<" "<<pG[1]<<" "<<pG[2]<<endl;
    } else {
      pG[1] = proj->GetMean();
      pG[2] = proj->GetRMS();
    }
    fG = (TF1*)(gROOT->FindObject("fG"));
    if(fG) delete fG;
    fG = new TF1("fG","gaus",TMath::Max(xmin,pG[1]-pG[2]),TMath::Min(xmax,pG[1]+pG[2]));
    fG->SetParameters(pG);
    fG->SetParLimits(1,TMath::Max(xmin,pG[1]-pG[2]),TMath::Min(xmax,pG[1]+pG[2]));
    proj->Fit("fG","QNLR0");
    fG->GetParameters(pG);
  }
  return ((!erfOK||pG[1]<xmin+0.1||pG[1]>xmax-0.05)? false : true);
}

// **************************************************************************

bool hcalUtil::twrMipFit(TH1D * pr,  Double_t * pE, float & maxE, Double_t * pG){
  TF1 * fE;
  TF1 * fG;
  int    bMax = pr->GetMaximumBin();
  TAxis *x;
  x = pr->GetXaxis();
  TH1D * p=(TH1D*)(pr->Clone());
  float xmean = p->GetMean();
  xmean = 0.3;
  int b1 = x->FindBin(xmean-0.07);
  int b2 = x->FindBin(xmean+0.07);
  for (int bin = b1; bin<=b2; bin++){
    p->SetBinError(bin,p->GetBinContent(bin));
  }
  double xmin = x->GetXmin();
  if(xmin<=0.) xmin = 0.05;
  //  double xmin = 0.12;
  double xmax = x->GetXmax();
  pE[0] = pr->Integral()*x->GetBinWidth(0)/(log(xmax)-log(xmin));;
  pE[1] = 1.;
  pE[2] = 1.;
  cout<<"Initial par. values"<<pE[0]<<" "<<pE[1]<<" "<< pE[2]<<endl;
  if(pE[0]<=0.) return false;

  fE    = (TF1*)(gROOT->GetListOfFunctions()->FindObject("fE"));
  if(fE) delete fE;
  //  cout<<(int)erfunc<<endl;
  fE = new TF1("fE",&powerFun,xmin,xmax,3);
  fE->SetParameters(pE);
  //  p->Fit("fE","NLR0");
  p->Fit("fE","NR");
  //  p->Draw();
  //  fE->Draw("same");
  fE->GetParameters(pE);
  pG[0] = pr->GetBinContent(bMax)*sqrt(6.28)*0.03;
  pG[1] = 0.3;
  pG[2] = 0.03;
  pG[3] = pE[0];
  pG[4] = pE[1];
  pG[5] = pE[2];
  fG = (TF1*)(gROOT->FindObject("fG"));
  if(fG) delete fG;
  fG = new TF1("fG",&power_g,xmin,xmax,6);
  fG->SetParameters(pG);
  fG->SetParLimits(3,pE[0],pE[0]);
  fG->SetParLimits(4,pE[1],pE[1]);
  fG->SetParLimits(5,pE[2],pE[2]);
//    fG->FixParameter(3,pE[0]);
//    fG->FixParameter(4,pE[1]);
//    fG->FixParameter(5,pE[2]);
  //pr->Fit("fG","NLR0");
  pr->Fit("fG","NRB");
  //  pr->Draw("same");
  //  fG->Draw("same");
  fG->GetParameters(pG);
  maxE = xPeak(fG,pG[1]-0.1,pG[1]+0.1);
  cout<<maxE<<endl;
  //  delete p;
  return ((pG[1]<xmin+0.05||pG[1]>xmax-0.05)? false : true);
}


// **************************************************************************

bool hcalUtil::twrTimeFit(TH1D * pr,  Double_t * pE, float & maxE, Double_t * pG){
  TF1 * fE;
  TF1 * fG;
  int    bMax = pr->GetMaximumBin();
  TAxis *x;
  x = pr->GetXaxis();
  TH1D * p=(TH1D*)(pr->Clone());
  float xmean = p->GetMean();
  xmean = 0.3;
  int b1 = x->FindBin(xmean-0.07);
  int b2 = x->FindBin(xmean+0.07);
  for (int bin = b1; bin<=b2; bin++){
    p->SetBinError(bin,p->GetBinContent(bin));
  }
  double xmin = x->GetXmin();
  ///////  if(xmin<=0.) xmin = 0.05;
  //  double xmin = 0.12;
  double xmax = x->GetXmax();
  pE[0] = pr->Integral()*x->GetBinWidth(0)/(log(xmax)-log(xmin));;
  pE[1] = 1.;
  pE[2] = 1.;
  fE    = (TF1*)(gROOT->GetListOfFunctions()->FindObject("fE"));
  if(fE) delete fE;
  //  cout<<(int)erfunc<<endl;
  fE = new TF1("fE",&powerFun,xmin,xmax,3);
  fE->SetParameters(pE);
  //  p->Fit("fE","NLR0");
  p->Fit("fE","NR");
  //  p->Draw();
  //  fE->Draw("same");
  fE->GetParameters(pE);
  pG[0] = pr->GetBinContent(bMax)*sqrt(6.28)*0.03;
  pG[1] = 0.3;
  pG[2] = 0.03;
  pG[3] = pE[0];
  pG[4] = pE[1];
  pG[5] = pE[2];
  fG = (TF1*)(gROOT->FindObject("fG"));
  if(fG) delete fG;
  fG = new TF1("fG",&power_g,xmin,xmax,6);
  fG->SetParameters(pG);
  fG->SetParLimits(3,pE[0],pE[0]);
  fG->SetParLimits(4,pE[1],pE[1]);
  fG->SetParLimits(5,pE[2],pE[2]);
//    fG->FixParameter(3,pE[0]);
//    fG->FixParameter(4,pE[1]);
//    fG->FixParameter(5,pE[2]);
  //pr->Fit("fG","NLR0");
  pr->Fit("fG","NRB");
  //  pr->Draw("same");
  //  fG->Draw("same");
  fG->GetParameters(pG);
  maxE = xPeak(fG,pG[1]-0.1,pG[1]+0.1);
  cout<<maxE<<endl;
  //  delete p;
  return ((pG[1]<xmin+0.05||pG[1]>xmax-0.05)? false : true);
}

//   ====================================================================================
//   ************************************************************************************
//   ====================================================================================

#include "hLabHelper.C"

#include "hcalHelper.C"

#include "hcalTree.C"

#include "tileHelper.C"

#include "tileTree.C"

