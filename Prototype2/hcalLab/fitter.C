#include "TCanvas.h"
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
   npeaks = np;
   //  create histogram with x[0,1000] with randomly positioned np peaks each sigma of 4
   //  total number of parameters to fit are 2(linear backgrownd) +3*np
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
   //  now extracting those pareameters from MC generated histogram
   TH1F *h2 = (TH1F*)h->Clone("h2");
   //Use TSpectrum to find the peak candidates
   TSpectrum *s = new TSpectrum(2*npeaks);
   Int_t nfound = s->Search(h,1,"new");
   printf("Found %d candidate peaks to fitn",nfound);
   c1->Update();
   c1->cd(2);
 
   //estimate linear background
   TF1 *fline = new TF1("fline","pol1",0,1000);
   h->Fit("fline","qn");
   //Loop on all found peaks. Eliminate peaks at the background level
   par[0] = fline->GetParameter(0);
   par[1] = fline->GetParameter(1);
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

   printf("Found %d useful peaks to fitn",npeaks);
   printf("Now fitting: Be patientn");
   TF1 *fit = new TF1("fit",fpeaks,0,1000,2+3*npeaks);
   TVirtualFitter::Fitter(h2,10+3*npeaks); //we may have more than the default 25 parameters
   fit->SetParameters(par);
   fit->SetNpx(1000);
   h2->Fit("fit");             
}

