// Illustrates how to find peaks in histograms.
// This script generates a random number of gaussian peaks
// on top of a linear background.
// The position of the peaks is found via TSpectrum and injected
// as initial values of parameters to make a global fit.
// The background is computed and drawn on top of the original histogram.
//
// To execute this example, do
//  root > .x peaks.C  (generate 10 peaks by default)
//  root > .x peaks.C++ (use the compiler)
//  root > .x peaks.C++(30) (generates 30 peaks)
//
// To execute only the first part of the script (without fitting)
// specify a negative value for the number of peaks, eg
//  root > .x peaks.C(-20)
//
//Author: Rene Brun

#include "TCanvas.h"
#include "TMath.h"
#include "TH1.h"
#include "TF1.h"
#include "TRandom.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"

Int_t npeaks = 30;
Double_t fpeaks(Double_t *x, Double_t *par) {
   Double_t result = par[0] + par[1]*x[0];
   for (Int_t p=0;p<npeaks;p++) {
      Double_t norm  = par[3*p+2];
      Double_t mean  = par[3*p+3];
      Double_t sigma = par[3*p+4];
      result += norm*TMath::Gaus(x[0],mean,sigma);
   }
   return result;
}
void peaks(Int_t np=10) {
   npeaks = TMath::Abs(np);
   TH1F *h = new TH1F("h","test",500,0,1000);
   //generate n peaks at random
   Double_t par[3000];
   par[0] = 0.8;
   par[1] = -0.6/1000;
   Int_t p;
   for (p=0;p<npeaks;p++) {
      par[3*p+2] = 1;
      par[3*p+3] = 10+gRandom->Rndm()*980;
      par[3*p+4] = 3+2*gRandom->Rndm();
   }
   TF1 *f = new TF1("f",fpeaks,0,1000,2+3*npeaks);
   f->SetNpx(1000);
   f->SetParameters(par);
   TCanvas *c1 = new TCanvas("c1","c1",10,10,1000,900);
   c1->Divide(1,2);
   c1->cd(1);
   h->FillRandom("f",200000);
   h->Draw();
   TH1F *h2 = (TH1F*)h->Clone("h2");
   //Use TSpectrum to find the peak candidates
   TSpectrum *s = new TSpectrum(2*npeaks);
   Int_t nfound = s->Search(h,2,"",0.10);
   printf("Found %d candidate peaks to fit\n",nfound);
   //Estimate background using TSpectrum::Background
   TH1 *hb = s->Background(h,20,"same");
   if (hb) c1->Update();
   if (np <0) return;

   //estimate linear background using a fitting method
   c1->cd(2);
   TF1 *fline = new TF1("fline","pol1",0,1000);
   h->Fit("fline","qn");
   //Loop on all found peaks. Eliminate peaks at the background level
   par[0] = fline->GetParameter(0);
   par[1] = fline->GetParameter(1);
   npeaks = 0;
   Double_t *xpeaks = s->GetPositionX();
   for (p=0;p<nfound;p++) {
      Double_t xp = xpeaks[p];
      Int_t bin = h->GetXaxis()->FindBin(xp);
      Double_t yp = h->GetBinContent(bin);
      if (yp-TMath::Sqrt(yp) < fline->Eval(xp)) continue;
      par[3*npeaks+2] = yp;
      par[3*npeaks+3] = xp;
      par[3*npeaks+4] = 3;
      npeaks++;
   }
   printf("Found %d useful peaks to fit\n",npeaks);
   printf("Now fitting: Be patient\n");
   TF1 *fit = new TF1("fit",fpeaks,0,1000,2+3*npeaks);
   //we may have more than the default 25 parameters
   TVirtualFitter::Fitter(h2,10+3*npeaks);
   fit->SetParameters(par);
   fit->SetNpx(1000);
   h2->Fit("fit");
}





// ;; This buffer is for notes you don't want to save, and for Lisp evaluation.
// ;; If you want to create a file, visit that file with C-x C-f,
// ;; then enter the text in that file's own buffer.

// **************************************************************************
//  assume background is linear what is most likely not true ....
//  par[0]   background constant 
//     [1]   background slope   
//  assume the signal is the sequence of equally spaced gaussian peaks
//  par[2]   peak to peak separation 
//     [3]   normalization constant for the first peak (pedestal)
//     [4]   location of the first peak
//     [5]   rms of the first peak - at this stage we will allow it to vary peak-to-peak
//     then 2 parameters per peak (normalization and RMS)
Double_t sipmpeaks(Double_t *x, Double_t *par) {
   Double_t result  = par[0] + par[1]*x[0];
   //  add pedestal peak first
   result          += par[3]*TMath::Gaus(x[0],par[4],par[5]); 
   for (Int_t p=0; p<npeaks; p++) {
      Double_t norm  = par[6  + p*2];
      Double_t mean  = par[4] + par[2]*(p+1);
      Double_t sigma = par[8  + p*2];
      result += norm*TMath::Gaus(x[0],mean,sigma);
   }
   return result;
}
/// **************************************************************************
// do single cell calibration using low end data stored in rv_# and fm_# channel histograms 
// the assumption - we have beautiful picture with at least 20 peaks !!!!!!
Int_t spmcalib(TString hName)
{
  TH1F * data = (TH1F*)(gSystem->FindObject(hName));
  if(!data){
    cout<<"spmCalib  Data Histogramm "<<hName<<"  not found"<<endl;
    exit();
  }
  Int_t      maxPeaks(20);        //  maximum number of peaks we have chance to resolve
  Double_t   range(150.);         //  range of amplitudes for peak search
  Double_t   Double_t norm0(1.),  slope0(-1./range); 
  Double_t   peak_to_peak(10.);
  Double_t   pLoc(0.),            pRMS(1.5);
  Double_t   par[6+maxPeaks*2];   //  backgrownd, pedestal and nPeaks of gaussian peaks with constant peak-to-peak separation
  
  par[0] = norm0;
  par[1] = slope0;
  par[2] = peak_to_peak;       //  peak-to-peak separation
  par[3] = norm0;              //  normalization for pedestal peak
  par[4] = pLoc;               //  pedestal mean
  par[5] = pRMS;               //  pedestal rms
  for (int p=0; p< maxPeaks; p++) {
      par[6 + p*3] = norm0;                       //  norm of the peak p
      par[7 + p*3] = par[4] + par[2]*(p+1);       //  mean of the peak p
      par[8 + p*3] = 2.;                          //  rms  of the peak p
   }
  TString scId   = "sc_"; scId += hName; 
  TF1  * scF     = (TF1*) (gROOT->FindObject(scId));               if(scF) delete scF;
  scF            = new TF1(scId,&sipmpeaks,0,range,6+maxPeaks*2); 
  TH1F * dataCl  = (TH1F*)(data->Clone("dataCl")); 
  dataCl        -> SetAxisRange(0., range);
  Int_t  binsToUse  = dataCl->GetXaxis()->FindBin(range);
  TCanvas * peaks = new TCanvas("peaks","peaks",10,10,1000,900);
  peaks         -> Divide(1,2);
  peaks         -> cd(1);
  dataCl        -> Draw();
  TH1F * dataCl2 = (TH1F*)data->Clone("dataCl2");

  //  Use TSpectrum to find the peak candidates
  TSpectrum *s = new TSpectrum(maxPeaks+1);
  Int_t nfound = s->Search(dataCl, 2, "", 0.10);
  printf("TSpectrum:  Found %d candidate peaks to fit\n",nfound);

  //Estimate background using TSpectrum::Background
  TH1 *hb = s->Background(h,20,"same");
  if (hb)   peaks->Update();
  //  if (np <0) return;
  //  function to estimate linear background
  peaks           -> cd(2);
  TF1 *fline       = new TF1("fline","pol1",0,range);
  dataCl          -> Fit("fline","qn");
  //Loop on all found peaks. Eliminate peaks at the background level
  par[0]           = fline->GetParameter(0);
  par[1]           = fline->GetParameter(1);
  Int_t     npeaks = 0;
  Double_t *xpeaks = s->GetPositionX();
  Double_t spacing = 0.;
  for (p=0; p<nfound; p++) {
    Double_t xp    = xpeaks[p];
    Int_t    bin   = dataCl->GetXaxis()->FindBin(xp);
    Double_t yp    = dataCl->GetBinContent(bin);
    cout<<"Testing Peak "<<p<<"  xp "<<xp<<"  bin "<<bin<<"  yP "<<yp<<"  BCKG "<<fline->Eval(xp)<<endl;
    if (yp-TMath::Sqrt(yp) < fline->Eval(xp)) continue;
    if(npeaks) spacing += xp - par[4 + (npeaks-1)+3];
    par[3 + npeaks*3] = yp;
    par[4 + npeaks*3] = xp;
    par[5 + npeaks*3] = 3.;
    npeaks++;
  }
  printf("Found %d useful peaks to fit\n",npeaks);
  printf("Now fitting: Be patient\n");
   TF1 *fit = new TF1("fit",&sipmpeaks,0,1000,3+3*npeaks);  // first peak is the pedestal
  //we may have more than the default 25 parameters
  TVirtualFitter::Fitter(h2,10+3*npeaks);
  fit->SetParameters(par);
  fit->SetNpx(1000);
  h2->Fit("fit");
}





/// **************************************************************************
// do single cell calibration using low end data stored in rv_# and fm_# channel histograms 
// the assumption - we have beautiful picture with at least 20 peaks !!!!!!
void sccalib(nPeaks = 20)
{
  Double_t   rcpar[2+3+2*nPeaks];   //  backgrownd and nPeaks of uncorrelated gaussian peaks  
  Double_t   fcpar[2+3+2*nPeaks];   //  backgrownd and nPeaks of uncorrelated gaussian peaks  

  int nx_c = 2;             
  int ny_c = NCH;
  TString cSC = "Single Cell Calibration (black - raw, red - fit) Run "; cSC += runId.Data();
  scCalibDisplay = new TCanvas("scCalibDisplay",cSC,200*nx_c,100*ny_c);
  scCalibDisplay->Divide(nx_c, ny_c);
  gStyle->SetOptStat(1110);
  gStyle->SetOptFit(111);
  Int_t  binsToUse(200);     Double_t slope0(-1./(Double_t))binsToUse);  Double_t norm0(1.); 
  Double_t peak_to_peak(10.);
  for (int ich = 0; ich<NCH; ich++){
    TH1F *hrv = (TH1F*)rpeak[ich];
    hrv       -> SetLineColor(1);
    hrv       -> SetAxisRange(0., binsToUse);
    TH1F *fmv = (TH1F*)fm[ich];
    fmv       -> SetLineColor(2); 
    fmv       -> SetAxisRange(0., binsToUse);

    //  estimate exponential backgrownd we go to 500 ADC counts  
    rcpar[0] = norm0;
    rcpar[1] = slope0;
    rcpar[2] = peak_to_peak;       //  peak-to-peak separation
    rcpar[3] = norm0;              //  normalization for pedestal peak
    rcpar[4] = 2.;                 //  pedestal mean
    rcpar[5] = 1.5;                //  pedestal rms
    //  we fit peak-to-peak not the individual independent peak positions
    for (int p=1; p<= nPeaks; p++) {
      rcpar[2*p+4] = norm0;    //  norm of the peak p
      rcpar[2*p+5] = 2.;       //  rms of the peak p
   }
    fcpar[0] = 1.;
    fcpar[1] = slope0;
    fcpar[2] = peak_to_peak;       //  peak-to-peak separation
    fcpar[3] = norm0;              //  normalization for pedestal peak
    fcpar[4] = 2.;                 //  pedestal mean
    fcpar[5] = 1.5;                //  pedestal rms
    //  we fit peak-to-peak not the individual independent peak positions
    for (int p=1; p<= nPeaks; p++) {
      fcpar[2*p+4] = norm0;    //  norm of the peak p
      fcpar[2*p+5] = 2.;    //  rms of the peak p
   }

    TString rvscId = "rvsc_"; rvscId += ich; 
    TString fmscId = "fmsc_"; fmscId += ich;
    rvsc[ich] = (TF1*) (gROOT->FindObject(rvscId));           if(rvsc[ich]) delete rvsc[ich];
    fmsc[ich] = (TF1*) (gROOT->FindObject(fmscId));           if(fmsc[ich]) delete fmsc[ich];
    rvsc[ich] = new TF1(rvscId,&scpeaks,0,500,5+2*nPeaks);    fmsc[ich] = new TF1(fmscId,&scpeaks,0,500,5+2*nPeaks);
    rvsc[ich] -> SetNpx(binsToUse);                           fmsc[ich] -> SetNpx(binsToUse);
    rvsc[ich] -> SetParameters(rcpar);                        fmsc[ich] -> SetParameters(fcpar);   
    rvsc[ich] -> SetLineColor(1);                             fmsc[ich] -> SetLineColor(2);
    TH1F * rh = (TH1F*)(hrv->Clone("rh"));                    TH1F * fh = (TH1F*)(fmv->Clone("fh"));

    //  function to estimate linear background
    TF1 *fline = new TF1("fline","pol1",0,binsToUse);
    //  Use TSpectrum to find the peak candidates
    TSpectrum *s = new TSpectrum(2*nPeaks);
    scCalibDisplay -> cd(ich*2+1);                            hrv -> Draw(); 
    Int_t      rfound = s->Search(hrv,1,"new");                    
    printf("Found %d candidate peaks in Raw hist ",rfound);  
    scCalibDisplay -> Update();
    hrv -> Fit("fline","qn");
    rcpar[0] = fline->GetParameter(0);
    rcpar[1] = fline->GetParameter(1);
    //Loop on all found peaks. Eliminate peaks at the background level
    nrpeaks = 0;
    Float_t *xrpeaks = s->GetPositionX();
    for (p=0;p<nfound;p++) {
      Float_t xp  = xpeaks[p];
      Int_t   bin = hrv->GetXaxis()->FindBin(xp);
      Float_t yp  = h->GetBinContent(bin);
      if (yp-TMath::Sqrt(yp) < fline->Eval(xp)) continue;
      par[2*npeaks+2] = yp;
      par[3*npeaks+3] = xp;
      par[3*npeaks+4] = 3;
      nrpeaks++;
    }




    scCalibDisplay -> cd(ich*2+2);                            fmv -> Draw(); 
    int_t ffound = s->Search(fmv,1,"new"); 
    printf("Found %d candidate peaks in Fit hist ",ffound);
    scCalibDisplay -> Update();
    fmv -> Fit("fline","qn");
    fcpar[0] = fline->GetParameter(0);
    fcpar[1] = fline->GetParameter(1);

    //Loop on all found peaks. Eliminate peaks at the background level
   npeaks = 0;
   Float_t *xpeaks = s->GetPositionX();
   for (p=0;p<nfound;p++) {
      Float_t xp = xpeaks[p];
      Int_t bin = h->GetXaxis()->FindBin(xp);
      Float_t yp = h->GetBinContent(bin);
      if (yp-TMath::Sqrt(yp) < fline->Eval(xp)) continue;
      par[3*npeaks+2] = yp;
      par[3*npeaks+3] = xp;
      par[3*npeaks+4] = 3;
      npeaks++;
   }








    //    rvsc[ich] -> SetParNames ("#mu","g","#sigma","A");         fmsc[ich] -> SetParNames ("#mu","g","#sigma","A");
    scCalibDisplay   -> cd(ich+1); hrv->SetAxisRange(0., 200); hrv ->Draw();  fmv->Draw("same");
    hrv       -> Fit(rvsc[ich],"rnQ");                         fmv -> Fit(fmsc[ich],"rnq"); 
    rvsc[ich] -> Draw("same");                                 fmsc[ich] -> Draw("same");
  }
  scCalibDisplay -> Update();
}
