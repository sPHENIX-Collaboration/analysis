#include <TNtuple.h>
#include <TF1.h>
#include <TLine.h>
#include "sPHENIXStyle/sPhenixStyle.C"

#include <iostream>
#include <fstream>

//----- MY STUFF -------------------------------------------------
double CBFunction(double *x, double *p)
{
  double norm = p[0];
  double alpha = p[1];  // tail start
  double n = p[2];      // tail shape 
  double mu = p[3];     // upsilon mass
  double sigma = p[4];  // upsilon width

  double A = pow(n/fabs(alpha),n)*TMath::Exp(-pow(fabs(alpha),2)/2.);
  double B = n/fabs(alpha) - fabs(alpha);
  double k = (x[0]-mu)/sigma;

  double val;
  if( k > -alpha )
    val = norm*TMath::Exp(-0.5*pow(k,2));
  else
    val = norm*A*pow(B-k,-n);

  if( TMath::IsNaN(val) )
    val = 0.0;

  return val;
}

double CBFunction_withBG(double *x, double *p)
{
  double norm = p[0];
  double alpha = p[1];  // tail start
  double n = p[2];      // tail shape 
  double mu = p[3];     // upsilon mass
  double sigma = p[4];  // upsilon width

  double A = pow(n/fabs(alpha),n)*TMath::Exp(-pow(fabs(alpha),2)/2.);
  double B = n/fabs(alpha) - fabs(alpha);
  double k = (x[0]-mu)/sigma;

  double val;
  if( k > -alpha )
    val = norm*TMath::Exp(-0.5*pow(k,2));
  else
    val = norm*A*pow(B-k,-n);

  double bgnorm1 = p[5];
  double bgslope1 = p[6];
  double bg = exp(bgnorm1+x[0]*bgslope1);

  val = val + bg;

  if( TMath::IsNaN(val) )
    val = 0.0;

  return val;
}
//---- ALICE STUFF ------------------------------------------------------------------

Double_t CrystallBall2( Double_t x, Double_t mean, Double_t sigma, Double_t alpha1, Double_t n1, Double_t alpha2, Double_t n2 )
{
  Double_t t = (x-mean)/sigma;
  if( t < -alpha1 )
  {
    Double_t a = TMath::Power( n1/alpha1, n1 )*TMath::Exp( -TMath::Power( alpha1, 2 )/2 );
    Double_t b = n1/alpha1 - alpha1;
    return a/TMath::Power( b - t, n1 );
  } else if( t > alpha2 ) {

    Double_t a = TMath::Power( n2/alpha2, n2 )*TMath::Exp( -TMath::Power( alpha2,2 )/2 );
    Double_t b = n2/alpha2 - alpha2;
    return a/TMath::Power( b + t, n2 );
  } else return TMath::Exp( -TMath::Power( t, 2 )/2 );
}

Double_t CrystallBall2Integral( Double_t sigma, Double_t alpha1, Double_t n1, Double_t alpha2, Double_t n2 )
{
// get corresponding integral
  alpha1 = fabs( alpha1 );
  alpha2 = fabs( alpha2 );
  return sigma*(
                n1/(alpha1*(n1-1))*TMath::Exp( -pow( alpha1, 2 )/2 ) +
                n2/(alpha2*(n2-1))*TMath::Exp( -pow( alpha2,2 )/2 ) +
    TMath::Sqrt( TMath::Pi()/2)*TMath::Erfc( -alpha1/TMath::Sqrt(2) ) -
    TMath::Sqrt( TMath::Pi()/2)*TMath::Erfc( alpha2/TMath::Sqrt(2) ) );
}

Double_t CrystallBall2( Double_t *x, Double_t *par )
{
  // get normalized Crystal ball
  Double_t result = CrystallBall2( x[0], par[1], par[2], par[3], par[4], par[5], par[6] );
  // get integral
  Double_t integral = CrystallBall2Integral( par[2], par[3], par[4], par[5], par[6] );
  // return scaled Crystalball so that par[0] corresponds to integral
  return par[0] * result/integral;
}

//======================================================================

void plotembed_vscent()
{
SetsPhenixStyle();
gStyle->SetOptStat(0);
gStyle->SetOptFit(1);

double RR = 15.;
const int nbins = 10;
double bins[nbins+1];
double AA = RR*RR/double(nbins);
bins[0] = 0.;
bins[nbins] = 16.;
for(int i=1; i<nbins; i++) { bins[i] = sqrt(i*AA); }
for(int i=0; i<=nbins; i++) { cout << "bin edge: " << i << " " << bins[i] << endl; }

TF1* fCB = new TF1("fCB",CBFunction,6.,12.,5);
TF1* fCBbg = new TF1("fCBbg",CBFunction_withBG,6.,12.,7);

float mass,type,pt,eta,rap,pt1,pt2,eta1,eta2,bimp;
float chisq1,chisq2,dca2d1,dca2d2,eop3x3_1,eop3x3_2;
float nmvtx1,nmvtx2,ntpc1,ntpc2;

char hname[99];
TH1D* hm[nbins];
TH1D* hmss[nbins];
TH1D* hmnbg[nbins];
TH1D* hbimp[nbins];
for(int i=0; i<nbins; i++) {
  sprintf(hname,"hm_%d",i);
  hm[i] = new TH1D(hname,hname,120,6.,12.);
  sprintf(hname,"hmss_%d",i);
  hmss[i] = new TH1D(hname,hname,120,6.,12.);
  sprintf(hname,"hmnbg_%d",i);
  hmnbg[i] = new TH1D(hname,hname,120,6.,12.);
  sprintf(hname,"hbimp_%d",i);
  hbimp[i] = new TH1D(hname,hname,1000,0.,20.);
  hm[i]->Sumw2();
  hmss[i]->Sumw2();
  hmnbg[i]->Sumw2();
}

TH1D* hmass = new TH1D("hmass","",120,6.,12.);
TH1D* hmass_ss = new TH1D("hmass_ss","",120,6.,12.);
TH1D* hmass_nobg = new TH1D("hmass_nobg","",120,6.,12.);
hmass->Sumw2();
hmass_ss->Sumw2();
hmass_nobg->Sumw2();

TLine* l1 = new TLine(7.,0.,11.,0.);
l1->SetLineStyle(2);

//TNtuple* ntp2;
TChain* cntp2 =  new TChain("ntp2");
TChain* cntp2c =  new TChain("ntp2");

//-------------------------------------------------------------------

// central ---------------------------------------

string infname_central;
ifstream ifs_central;
ifs_central.open("centrallist.txt");
if ( ifs_central.fail() ) { cout << "FAIL TO READ INPUT FILE 1" << endl; ifs_central.close(); return; }
while(!ifs_central.eof()) {
  ifs_central >> infname_central;
  cntp2c->AddFile(infname_central.c_str());
}
ifs_central.close();

cout << "number of CENTRAL entries = " << cntp2c->GetEntries() << endl;

cntp2c->SetBranchAddress("type",&type);
cntp2c->SetBranchAddress("mass",&mass);
cntp2c->SetBranchAddress("pt",&pt);
cntp2c->SetBranchAddress("eta",&eta);
cntp2c->SetBranchAddress("rap",&rap);
cntp2c->SetBranchAddress("pt1",&pt1);
cntp2c->SetBranchAddress("pt2",&pt2);
cntp2c->SetBranchAddress("eta1",&eta1);
cntp2c->SetBranchAddress("eta2",&eta2);
cntp2c->SetBranchAddress("chisq1",&chisq1);
cntp2c->SetBranchAddress("dca2d1",&dca2d1);
cntp2c->SetBranchAddress("eop3x3_1",&eop3x3_1);
cntp2c->SetBranchAddress("chisq2",&chisq2);
cntp2c->SetBranchAddress("dca2d2",&dca2d2);
cntp2c->SetBranchAddress("eop3x3_2",&eop3x3_2);
cntp2c->SetBranchAddress("nmvtx1",&nmvtx1);
cntp2c->SetBranchAddress("nmvtx2",&nmvtx2);
cntp2c->SetBranchAddress("ntpc1",&ntpc1);
cntp2c->SetBranchAddress("ntpc2",&ntpc2);

  for(int j=0; j<cntp2c->GetEntries(); j++) {
    cntp2c->GetEvent(j);
    if(chisq1>3 || chisq2>3) continue;
    if(nmvtx1<2 || nmvtx2<2) continue;
    if(ntpc1<26 || ntpc2<26) continue;
    if(eop3x3_1<0.7 || eop3x3_2<0.7) continue;
    //if(type==1) { hmass->Fill(mass); hmass_nobg->Fill(mass); }
    //if(type==2 || type==3) { hmass_ss->Fill(mass); }
      if(type==1) { hm[0]->Fill(mass); hmnbg[0]->Fill(mass); }
      if(type==2 || type==3) { hmss[0]->Fill(mass); }
  }
// end central -----------------------------------

// minbias ---------------------------------------
string infname;
ifstream ifs;
ifs.open("mblist.txt");
if ( ifs.fail() ) { cout << "FAIL TO READ INPUT FILE 1" << endl; ifs.close(); return; }
while(!ifs.eof()) {
  ifs >> infname;
  cntp2->AddFile(infname.c_str());
}
ifs.close();

cout << "number of MB entries = " << cntp2->GetEntries() << endl;

cntp2->SetBranchAddress("b",&bimp);
cntp2->SetBranchAddress("type",&type);
cntp2->SetBranchAddress("mass",&mass);
cntp2->SetBranchAddress("pt",&pt);
cntp2->SetBranchAddress("eta",&eta);
cntp2->SetBranchAddress("rap",&rap);
cntp2->SetBranchAddress("pt1",&pt1);
cntp2->SetBranchAddress("pt2",&pt2);
cntp2->SetBranchAddress("eta1",&eta1);
cntp2->SetBranchAddress("eta2",&eta2);
cntp2->SetBranchAddress("chisq1",&chisq1);
cntp2->SetBranchAddress("dca2d1",&dca2d1);
cntp2->SetBranchAddress("eop3x3_1",&eop3x3_1);
cntp2->SetBranchAddress("chisq2",&chisq2);
cntp2->SetBranchAddress("dca2d2",&dca2d2);
cntp2->SetBranchAddress("eop3x3_2",&eop3x3_2);
cntp2->SetBranchAddress("nmvtx1",&nmvtx1);
cntp2->SetBranchAddress("nmvtx2",&nmvtx2);
cntp2->SetBranchAddress("ntpc1",&ntpc1);
cntp2->SetBranchAddress("ntpc2",&ntpc2);

  for(int j=0; j<cntp2->GetEntries(); j++) {
    cntp2->GetEvent(j);
    if(chisq1>3 || chisq2>3) continue;
    if(nmvtx1<2 || nmvtx2<2) continue;
    if(ntpc1<26 || ntpc2<26) continue;
    if(eop3x3_1<0.7 || eop3x3_2<0.7) continue;
    if(type==1) { hmass->Fill(mass); hmass_nobg->Fill(mass); }
    if(type==2 || type==3) { hmass_ss->Fill(mass); }

    for(int j=0; j<nbins; j++) {
      if(bimp>bins[j] && bimp<bins[j+1]) {
        if(type==1) { hm[j]->Fill(mass); hmnbg[j]->Fill(mass); hbimp[j]->Fill(bimp); }
        if(type==2 || type==3) { hmss[j]->Fill(mass); }
      }
    }    

  }
// end minbias -------------------------------------------------------

for(int j=0; j<nbins; j++) { hmnbg[j]->Add(hmss[j],-1.0); }
hmass_nobg->Add(hmass_ss,-1.0);

double massres[nbins];
double massreserr[nbins];
double x[nbins];
for(int i=0; i<nbins; i++) { x[i] = hbimp[i]->GetMean(); cout << "mean b = " << x[i] << endl; }

//----------------------------------------------------------------

// ALICE Double Crystal Ball function
TF1 *f2 = new TF1("f2",CrystallBall2,7,11,7);
  f2->SetParameter(0, 2000. );
  f2->SetParameter(1, 9.46 );
  f2->SetParameter(2, 0.13 );
  f2->SetParameter(3, 1);
  f2->SetParameter(4, 3);
  f2->SetParameter(5, 1 );
  f2->SetParameter(6, 5 );
  f2->SetParNames("normalization", "mean", "sigma","alpha1","n1","alpha2","n2");
    f2->SetParLimits(1, 9.40, 9.55);
    f2->SetParLimits(2, 0.06, 0.20);
    f2->SetParLimits(3, 0.120, 10);
    f2->SetParLimits(4, 1.01, 10);
    f2->SetParLimits(5, 0.1, 10);
    f2->SetParLimits(6, 1.01, 10);


TCanvas* c3 = new TCanvas("c3","inv. mass allpT",20,20,600,500);

hmass->Draw();
hmass_ss->SetLineColor(kRed);
hmass_ss->Scale(1.2);
hmass_ss->Draw("same");

TCanvas* c33 = new TCanvas("c33","inv. mass nobg",20,20,600,500);
//c33->Divide(3,2);
/*
  fCB->SetParameter(0,1000.);
  fCB->SetParameter(1,1.0);
  fCB->SetParameter(2,1.0);
  fCB->SetParLimits(2,0.1,99.);
  fCB->SetParameter(3,9.0);
  fCB->SetParameter(4,0.100);
*/

//  c33->cd(1);
  hmass_nobg->SetAxisRange(7.,11.);
  hmass_nobg->Fit("f2","rlq","",7.5,10.0);

  for(int j=0; j<nbins; j++) { 
//    c33->cd(j+2);
  fCB->SetParameter(0,hmnbg[j]->GetMaximum());
    hmnbg[j]->SetTitle(";Invariant mass, GeV ; Counts");
    hmnbg[j]->Fit("f2","rlq","",7.5,10.0); 
    hmnbg[j]->SetAxisRange(7.,11.);
//    l1->Draw();
    cout << "mass resolution = " << j << " " << f2->GetParameter(2) << " +- " << f2->GetParError(2) << endl;
    massres[j] = 1000.*f2->GetParameter(2);
    massreserr[j] = 1000.*f2->GetParError(2);
  }
  hmass_nobg->SetTitle(";Invariant mass, GeV ; Counts");
  hmass_nobg->Draw();
  TLatex* lat1 = new TLatex(7.2,hmass_nobg->GetMaximum()*0.4,"Minimum Bias"); lat1->Draw();
  l1->Draw();


//  double fitndf = hmass_nobg->GetFunction("fCB")->GetNDF();
//  double fitchi2 = hmass_nobg->GetFunction("fCB")->GetChisquare();
//  cout << "chi2 = " << fitchi2/fitndf << endl;

double scale = 1.4;
TF1* fws = new TF1("fws","[0]/(1+exp((x-[1])/[2]))",0.,20.);
fws->SetParameter(0,8.68398e+01*scale);
fws->SetParameter(1,1.51845e+01);
fws->SetParameter(2,6.52512e-01);
TF1* fws_sum = new TF1("fws_sum","(x*[0])/(1+exp((x-[1])/[2]))",0.,20.);
fws_sum->SetParameter(0,8.68398e+01*scale);
fws_sum->SetParameter(1,1.51845e+01);
fws_sum->SetParameter(2,6.52512e-01);
double avg[nbins];
for(int j=0; j<nbins; j++) {
  avg[j] = fws_sum->Integral(bins[j],bins[j+1]);
//  cout << "expected = " << avg[j] << endl;
}
avg[0] = avg[0]*0.9;

double sumstart = 8.8;
double sumstop  = 9.8;
int fbin = hmass_nobg->FindBin(sumstart + 0.001);
int lbin = hmass_nobg->FindBin(sumstop  - 0.001);
cout << "counting bins from " << fbin << " to " << lbin << endl;
cout << "   in mass range from " << hmass_nobg->GetBinLowEdge(fbin) << " to " << hmass_nobg->GetBinLowEdge(lbin)+hmass_nobg->GetBinWidth(lbin) << endl;

  double sum =0.;
  double sumsum[nbins]; for(int i=0; i<nbins; i++) { sumsum[i]=0.; }
  double y[nbins],ey[nbins];
  for(int k=fbin; k<=lbin; k++) {
    sum += hmass_nobg->GetBinContent(k);
    for(int i=0; i<nbins; i++) { 
      sumsum[i] += hmnbg[i]->GetBinContent(k); 
      y[i] = sumsum[i]/avg[i];
      ey[i] = y[i]*0.05;
    }
  }
  cout << "Number of Upsilons = " << sum << endl;
  for(int i=0; i<nbins; i++) { cout << sumsum[i] << endl; }

  cout << "Number of Upsilons from fit = " << fCB->Integral(sumstart,sumstop)/hmass_nobg->GetBinWidth(lbin) << " " << fCB->Integral(5.0,11.0)/hmass_nobg->GetBinWidth(lbin) << endl;

TCanvas* c4 = new TCanvas("c4","mass resolution vs Bimp",20,20,600,500);
TH2D* hh = new TH2D("hh","",10,0.,18.,10,60.,140.);
hh->SetTitle(";Impact parameter, fm ; Mass resolution, MeV");
hh->Draw();

TGraphErrors* gr1 = new TGraphErrors(nbins,x,massres,0,massreserr);
gr1->SetMarkerStyle(20);
gr1->SetMarkerColor(kBlack);
gr1->SetMarkerSize(1.5);
gr1->SetLineColor(kBlack);
gr1->SetLineWidth(2);
gr1->Draw("p");

/*
TCanvas* c44 = new TCanvas("c44","mass resolution vs Bimp",120,120,600,500);
TH2D* hhh = new TH2D("hhh","",10,0.,18.,10,0.,1.5);
hhh->Draw();

TGraphErrors* gr2 = new TGraphErrors(nbins,x,y,0,ey);
gr2->SetMarkerStyle(20);
gr2->SetMarkerColor(kBlue);
gr2->SetMarkerSize(1.5);
gr2->SetLineColor(kBlue);
gr2->Draw("p");
*/

TCanvas* c5 = new TCanvas("c5","",60,60,600,500);
hmnbg[0]->Draw();
TLatex* lat2 = new TLatex(7.2,hmnbg[0]->GetMaximum()*0.4,"10% central Au+Au"); lat2->Draw();
l1->Draw();

}

