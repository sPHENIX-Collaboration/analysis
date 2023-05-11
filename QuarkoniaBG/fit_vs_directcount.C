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

  if( TMath::IsNaN(val) ) val = 0.0;

  return val;
}

double TripleCBFunction(double *x, double *p)
{
  double norm1 = p[0];  // amplitude of Y(1S) peak
  double alpha = p[1];  // tail start
  double n = p[2];      // tail shape 
  double mu1 = p[3];    // upsilon (1S) mass
  double sigma = p[4];  // upsilon width
  double norm2 = p[5];
  double norm3 = p[6];
  double mu2 = mu1*1.0595;
  double mu3 = mu1*1.0946;

  double A = pow(n/fabs(alpha),n)*TMath::Exp(-pow(fabs(alpha),2)/2.);
  double B = n/fabs(alpha) - fabs(alpha);
  double k1 = (x[0]-mu1)/sigma;
  double k2 = (x[0]-mu2)/sigma;
  double k3 = (x[0]-mu3)/sigma;

  double val,val1,val2,val3;

  if( k1 > -alpha ) { val1 = norm1*TMath::Exp(-0.5*pow(k1,2)); }
  else { val1 = norm1*A*pow(B-k1,-n); }
  if( k2 > -alpha ) { val2 = norm2*TMath::Exp(-0.5*pow(k2,2)); }
  else { val2 = norm2*A*pow(B-k2,-n); }
  if( k3 > -alpha ) { val3 = norm3*TMath::Exp(-0.5*pow(k3,2)); }
  else { val3 = norm3*A*pow(B-k3,-n); }

  val = val1 + val2 + val3;

  if( TMath::IsNaN(val) ) val = 0.0;

  return val;
}
double SandB_CBFunction(double *x, double *p)
{
  double norm1 = p[0];  // amplitude of Y(1S) peak
  double alpha = p[1];  // tail start
  double n = p[2];      // tail shape 
  double mu1 = p[3];     // upsilon (1S) mass
  double sigma = p[4];  // upsilon width
  double norm2 = p[5];
  double norm3 = p[6];
  double mu2 = mu1*1.0595;
  double mu3 = mu1*1.0946;

  double A = pow(n/fabs(alpha),n)*TMath::Exp(-pow(fabs(alpha),2)/2.);
  double B = n/fabs(alpha) - fabs(alpha);
  double k1 = (x[0]-mu1)/sigma;
  double k2 = (x[0]-mu2)/sigma;
  double k3 = (x[0]-mu3)/sigma;

  double val,val1,val2,val3;

  if( k1 > -alpha ) { val1 = norm1*TMath::Exp(-0.5*pow(k1,2)); }
  else { val1 = norm1*A*pow(B-k1,-n); }
  if( k2 > -alpha ) { val2 = norm2*TMath::Exp(-0.5*pow(k2,2)); }
  else { val2 = norm2*A*pow(B-k2,-n); }
  if( k3 > -alpha ) { val3 = norm3*TMath::Exp(-0.5*pow(k3,2)); }
  else { val3 = norm3*A*pow(B-k3,-n); }

  double bgnorm1 = p[7];
  double bgslope1 = p[8];

  double bg = exp(bgnorm1+x[0]*bgslope1);

  val = val1 + val2 + val3 + bg;
  if( TMath::IsNaN(val) ) val = 0.0;

  return val;
}

//=======================================================================

void fit_vs_directcount() {

const int nbins = 15;

// TRUE numbers
//Number of upsilons in AuAu plot = 11752 971 111
//Number of upsilons in pp plot = 9755 2537 1414
double Nups1[nbins+1],Nups2[nbins+1],Nups3[nbins+1];
Nups1[0]=1020.82;
Nups2[0]=84.3446;
Nups3[0]=9.64186;
Nups1[1]=2519.56;
Nups2[1]=208.177;
Nups3[1]=23.7977;
Nups1[2]=2870.95;
Nups2[2]=237.21;
Nups3[2]=27.1167;
Nups1[3]=2326.04;
Nups2[3]=192.187;
Nups3[3]=21.9699;
Nups1[4]=1500.87;
Nups2[4]=124.008;
Nups3[4]=14.176;
Nups1[5]=820.126;
Nups2[5]=67.7622;
Nups3[5]=7.74625;
Nups1[6]=396.538;
Nups2[6]=32.7637;
Nups3[6]=3.74538;
Nups1[7]=175.598;
Nups2[7]=14.5086;
Nups3[7]=1.65856;
Nups1[8]=73.2069;
Nups2[8]=6.04867;
Nups3[8]=0.691454;
Nups1[9]=29.3682;
Nups2[9]=2.42653;
Nups3[9]=0.277389;
Nups1[10]=11.5318;
Nups2[10]=0.952802;
Nups3[10]=0.10892;
Nups1[11]=4.49014;
Nups2[11]=0.370994;
Nups3[11]=0.0424103;
Nups1[12]=1.75068;
Nups2[12]=0.144648;
Nups3[12]=0.0165355;
Nups1[13]=0.688396;
Nups2[13]=0.0568782;
Nups3[13]=0.00650204;
Nups1[14]=0.274397;
Nups2[14]=0.0226719;
Nups3[14]=0.00259174;

double Nups1pp[nbins+1],Nups2pp[nbins+1],Nups3pp[nbins+1];
Nups1pp[0]=847.355;
Nups2pp[0]=220.373;
Nups3pp[0]=122.825;
Nups1pp[1]=2091.41;
Nups2pp[1]=543.917;
Nups3pp[1]=303.153;
Nups1pp[2]=2383.1;
Nups2pp[2]=619.776;
Nups3pp[2]=345.433;
Nups1pp[3]=1930.78;
Nups2pp[3]=502.14;
Nups3pp[3]=279.869;
Nups1pp[4]=1245.83;
Nups2pp[4]=324.005;
Nups3pp[4]=180.585;
Nups1pp[5]=680.763;
Nups2pp[5]=177.047;
Nups3pp[5]=98.6775;
Nups1pp[6]=329.155;
Nups2pp[6]=85.6039;
Nups3pp[6]=47.7115;
Nups1pp[7]=145.759;
Nups2pp[7]=37.9077;
Nups3pp[7]=21.1279;
Nups1pp[8]=60.767;
Nups2pp[8]=15.8038;
Nups3pp[8]=8.80825;
Nups1pp[9]=24.3777;
Nups2pp[9]=6.33996;
Nups3pp[9]=3.53358;
Nups1pp[10]=9.57218;
Nups2pp[10]=2.48945;
Nups3pp[10]=1.3875;
Nups1pp[11]=3.72714;
Nups2pp[11]=0.969323;
Nups3pp[11]=0.540253;
Nups1pp[12]=1.45319;
Nups2pp[12]=0.377933;
Nups3pp[12]=0.210641;
Nups1pp[13]=0.571418;
Nups2pp[13]=0.14861;
Nups3pp[13]=0.0828277;
Nups1pp[14]=0.227769;
Nups2pp[14]=0.0592364;
Nups3pp[14]=0.0330155;

// new suppression
double raapt[9],raa1s[9],raa2s[9],oldraa1s[9],oldraa2s[9];
raapt[0] = 1.5;
raapt[1] = 4.5;
raapt[2] = 7.5;
raapt[3] = 10.5;
raapt[4] = 13.5;
oldraa1s[0] = 0.535;
oldraa1s[1] = 0.535;
oldraa1s[2] = 0.535;
oldraa1s[3] = 0.535;
oldraa1s[4] = 0.535;
oldraa2s[0] = 0.170;
oldraa2s[1] = 0.170;
oldraa2s[2] = 0.170;
oldraa2s[3] = 0.170;
oldraa2s[4] = 0.170;
raa1s[0] = 0.4960;
raa1s[1] = 0.4960;
raa1s[2] = 0.4955;
raa1s[3] = 0.4968;
raa1s[4] = 0.4743;
raa2s[0] = 0.1710;
raa2s[1] = 0.1629;
raa2s[2] = 0.1326;
raa2s[3] = 0.1232;
raa2s[4] = 0.0928;
TGraph* grRAA1S = new TGraph(5,raapt,raa1s);
TGraph* grRAA2S = new TGraph(5,raapt,raa2s);
TGraph* groldRAA1S = new TGraph(5,raapt,oldraa1s);
TGraph* groldRAA2S = new TGraph(5,raapt,oldraa2s);
for(int i=0; i<nbins; i++) {
  Nups1[i] = Nups1[i] * grRAA1S->Eval(0.5+i*1.0)/groldRAA1S->Eval(0.5+i*1.0);
  Nups2[i] = Nups2[i] * grRAA2S->Eval(0.5+i*1.0)/groldRAA2S->Eval(0.5+i*1.0);
}
grRAA1S->SetLineColor(kBlue);
grRAA1S->SetLineStyle(3);
grRAA1S->SetLineWidth(3);
grRAA2S->SetLineColor(kMagenta);
grRAA2S->SetLineStyle(3);
grRAA2S->SetLineWidth(3);



//gROOT->LoadMacro("sPHENIXStyle/sPhenixStyle.C");
//SetsPhenixStyle();
gStyle->SetOptStat(0);
gStyle->SetOptFit(0);

char tmpname[999];
TLatex* tl[nbins+1];
char tlchar[999];

TH1D* hhfit[nbins+1];
TH1D* hhcorrbg_scaled[nbins+1];
TH1D* hhups1pp[nbins+1];
TH1D* hhups2pp[nbins+1];
TH1D* hhups3pp[nbins+1];
TH1D* hhupspp[nbins+1];
TH1D* hhups1[nbins+1];
TH1D* hhups2[nbins+1];
TH1D* hhups3[nbins+1];
TH1D* hhups[nbins+1];
TH1D* hhall_pp[nbins+1];
TH1D* hhcorrbg_pp[nbins+1];

TF1* fCBups1s = new TF1("fCBups1s",CBFunction,5.,14.,5);
TF1* fCBups2s = new TF1("fCBups2s",CBFunction,5.,14.,5);
TF1* fCBups1spp = new TF1("fCBups1spp",CBFunction,5.,14.,5);
TF1* fCBups2spp = new TF1("fCBups2spp",CBFunction,5.,14.,5);
TF1* fTCB = new TF1("fTCB",TripleCBFunction,5.,14.,7);
TF1* fSandB = new TF1("fSandB",SandB_CBFunction,5.,14.,9);
TF1* fSandBpp = new TF1("fSandBpp",SandB_CBFunction,5.,14.,9);
TF1* fSandBauau = new TF1("fSandBauau",SandB_CBFunction,5.,14.,9);

double tonypar1 = 0.98;    // Tony's 100 pion simulation April 2019
double tonypar2 = 0.93;    // Tony's 100 pion simulation April 2019
//double tonypar3 = 9.437;   // Tony's 100 pion simulation April 2019
double tonypar3 = 9.448;   // benchmark
double tonypar4 = 0.100;   // benchmark

double u1start = 9.25;
double u1stop  = 9.65;
double u2start = 9.80;
double u2stop  = 10.20;
double u3start = 10.20;
double u3stop  = 10.55;
double sum1[nbins+1]      = {0.};
double truesum1[nbins+1]  = {0.};
double truesum1pp[nbins+1]  = {0.};
double ersum1[nbins+1]    = {0.};
double sum2[nbins+1]      = {0.};
double truesum2[nbins+1]  = {0.};
double truesum2pp[nbins+1]  = {0.};
double ersum2[nbins+1]    = {0.};
double sum1pp[nbins+1]    = {0.};
double ersum1pp[nbins+1]  = {0.};
double sum2pp[nbins+1]    = {0.};
double ersum2pp[nbins+1]  = {0.};
double sumfit1[nbins+1]   = {0.};
double ersumfit1[nbins+1] = {0.};
double sumfit2[nbins+1]   = {0.};
double ersumfit2[nbins+1] = {0.};
double sumfit1pp[nbins+1]   = {0.};
double ersumfit1pp[nbins+1] = {0.};
double sumfit2pp[nbins+1]   = {0.};
double ersumfit2pp[nbins+1] = {0.};
double sum1ppbg[nbins+1]    = {0.};
double ersum1ppbg[nbins+1]  = {0.};
double sum2ppbg[nbins+1]    = {0.};
double ersum2ppbg[nbins+1]  = {0.};

double xx1[nbins+1]; for(int i=0; i<nbins+1; i++) {xx1[i] = 0.50 + double(i);}
double xx2[nbins+1]; for(int i=0; i<nbins+1; i++) {xx2[i] = 0.40 + double(i);}
double xx3[nbins+1]; for(int i=0; i<nbins+1; i++) {xx3[i] = 0.60 + double(i);}

//=====================================================================================================

// read histograms created by newplotbg.C

TFile* f=new TFile("ups_corrbg_24b_auau.root");
for(int i=0; i<nbins+1; i++) {

  sprintf(tmpname,"hhfit_%d",i);
  hhfit[i] = (TH1D*)f->Get(tmpname);
  hhfit[i]->SetDirectory(gROOT);
  sprintf(tmpname,"hhcorrbg_scaled_%d",i);
  hhcorrbg_scaled[i] = (TH1D*)f->Get(tmpname);
  hhcorrbg_scaled[i]->SetDirectory(gROOT);

  sprintf(tmpname,"hhups1pp_%d",i);
  hhups1pp[i] = (TH1D*)f->Get(tmpname);
  hhups1pp[i]->SetDirectory(gROOT);
  sprintf(tmpname,"hhups2pp_%d",i);
  hhups2pp[i] = (TH1D*)f->Get(tmpname);
  hhups2pp[i]->SetDirectory(gROOT);
  sprintf(tmpname,"hhups3pp_%d",i);
  hhups3pp[i] = (TH1D*)f->Get(tmpname);
  hhups3pp[i]->SetDirectory(gROOT);
  sprintf(tmpname,"hhupspp_%d",i);
  hhupspp[i] = (TH1D*)f->Get(tmpname);
  hhupspp[i]->SetDirectory(gROOT);

  sprintf(tmpname,"hhups1_%d",i);
  hhups1[i] = (TH1D*)f->Get(tmpname);
  hhups1[i]->SetDirectory(gROOT);
  sprintf(tmpname,"hhups2_%d",i);
  hhups2[i] = (TH1D*)f->Get(tmpname);
  hhups2[i]->SetDirectory(gROOT);
  sprintf(tmpname,"hhups3_%d",i);
  hhups3[i] = (TH1D*)f->Get(tmpname);
  hhups3[i]->SetDirectory(gROOT);
  sprintf(tmpname,"hhups_%d",i);
  hhups[i] = (TH1D*)f->Get(tmpname);
  hhups[i]->SetDirectory(gROOT);

  sprintf(tmpname,"hhall_pp_%d",i);
  hhall_pp[i] = (TH1D*)f->Get(tmpname);
  hhall_pp[i]->SetDirectory(gROOT);
  sprintf(tmpname,"hhcorrbg_pp_%d",i);
  hhcorrbg_pp[i] = (TH1D*)f->Get(tmpname);
  hhcorrbg_pp[i]->SetDirectory(gROOT);

}
f->Close();

  // fg and bg parameters from newplotbg.C
  double bgpar0[nbins+1],bgpar1[nbins+1];
  for(int i=0; i<nbins+1; i++) {
    bgpar0[i] = hhcorrbg_scaled[i]->GetFunction("expo")->GetParameter(0);
    bgpar1[i] = hhcorrbg_scaled[i]->GetFunction("expo")->GetParameter(1);
  }
  double fgpar0 =  hhups[nbins]->GetFunction("fTCBauau")->GetParameter(0);
  double fgpar5 =  hhups[nbins]->GetFunction("fTCBauau")->GetParameter(5);
  double fgpar6 =  hhups[nbins]->GetFunction("fTCBauau")->GetParameter(6);

//-------------------------------------------------------------------------------------

TCanvas* cups = new TCanvas("cups","Upsilons and correlated BG",150,150,700,700);

  // "true" distribution
  fSandBauau->SetParameter(0,fgpar0);
  fSandBauau->SetParameter(1,tonypar1);
  fSandBauau->SetParameter(2,tonypar2);
  fSandBauau->SetParameter(3,tonypar3);
  fSandBauau->SetParameter(4,tonypar4);
  fSandBauau->SetParameter(5,fgpar5);
  fSandBauau->SetParameter(6,fgpar6);
  fSandBauau->SetParameter(7,bgpar0[nbins]);
  fSandBauau->SetParameter(8,bgpar1[nbins]);
  fSandBauau->SetLineColor(kRed);
//  fSandBauau->Draw("same");

  // "true" Upsilon1S
  fCBups1s->SetParameter(0,fSandBauau->GetParameter(0));
  fCBups1s->SetParameter(1,fSandBauau->GetParameter(1));
  fCBups1s->SetParameter(2,fSandBauau->GetParameter(2));
  fCBups1s->SetParameter(3,fSandBauau->GetParameter(3));
  fCBups1s->SetParameter(4,fSandBauau->GetParameter(4));
  double true_ups1s_integral = fCBups1s->Integral(5.,14.);
  double true_ups1s_ampl = fSandBauau->GetParameter(0);
  double true_ups1s_amplerr = fSandBauau->GetParError(0);
  double binsize = hhfit[nbins]->GetBinWidth(1);
  cout << "TRUE Integral = " << true_ups1s_integral/binsize << " +- " << true_ups1s_integral*(true_ups1s_amplerr/true_ups1s_ampl)/binsize << "  ( " << true_ups1s_amplerr/true_ups1s_ampl*100. << "% )" << endl;

  // "true" corr bg
  TF1* fcorrbg[nbins+1];
  fcorrbg[nbins] = new TF1("fcorrbg_15","exp([0]+[1]*x)",7.,14.);
  fcorrbg[nbins]->SetParameters(bgpar0[nbins],bgpar1[nbins]);
  //fcorrbg[nbins]->SetLineStyle(2);
  fcorrbg[nbins]->SetLineWidth(1);
  fcorrbg[nbins]->SetLineColor(kRed);

//--- FIT all pT -------------------------------------------------------------------------

  fSandB->SetParameter(0,fgpar0);
  fSandB->FixParameter(1,tonypar1);
  fSandB->FixParameter(2,tonypar2);
  fSandB->FixParameter(3,tonypar3);
  fSandB->FixParameter(4,tonypar4);
  fSandB->SetParameter(5,fgpar5);
  fSandB->SetParameter(6,fgpar6);
  fSandB->SetParameter(7,bgpar0[nbins]);
  fSandB->SetParameter(8,bgpar1[nbins]);
  hhfit[nbins]->Fit(fSandB,"qrl","",8.,11.);

  hhfit[nbins]->GetXaxis()->SetTitleOffset(1.0);
  hhfit[nbins]->GetYaxis()->SetTitleOffset(1.6);
  hhfit[nbins]->SetAxisRange(8.0,11.0); 
  hhfit[nbins]->Draw();

  TF1* fcorrbg_fromfit[nbins+1];
  sprintf(tmpname,"fcorrbg_fromfit_%d",15);
  fcorrbg_fromfit[nbins] = new TF1(tmpname,"exp([0]+[1]*x)",7.,14.);
  fcorrbg_fromfit[nbins]->SetParameters(fSandB->GetParameter(7),fSandB->GetParameter(8));
  fcorrbg_fromfit[nbins]->SetLineStyle(2);
  fcorrbg_fromfit[nbins]->SetLineWidth(3);
  fcorrbg_fromfit[nbins]->Draw("same");
  fcorrbg[nbins]->Draw("same"); // true correlated bg

  // counting range
  TLine* line1 = new TLine(u1start,0.,u1start,hhfit[nbins]->GetMaximum()*1.1); line1->SetLineStyle(2); line1->Draw();
  TLine* line2 = new TLine(u1stop,0.,u1stop,hhfit[nbins]->GetMaximum()*1.1); line2->SetLineStyle(2); line2->Draw();

  // Upsilon1S from fit
  fCBups1s->SetParameter(0,fSandB->GetParameter(0));
  fCBups1s->SetParameter(1,fSandB->GetParameter(1));
  fCBups1s->SetParameter(2,fSandB->GetParameter(2));
  fCBups1s->SetParameter(3,fSandB->GetParameter(3));
  fCBups1s->SetParameter(4,fSandB->GetParameter(4));
  double ups1s_integral = fCBups1s->Integral(5.,14.);
  double ups1s_ampl = fSandB->GetParameter(0);
  double ups1s_amplerr = fSandB->GetParError(0);
  cout << "Integral = " << ups1s_integral/binsize << " +- " << ups1s_integral*(ups1s_amplerr/ups1s_ampl)/binsize << "  ( " << ups1s_amplerr/ups1s_ampl*100. << "% )" << endl;

//--- Direct Counting all pT ------------------------------------------------------

  int fbin1 = hhfit[nbins]->FindBin(u1start + 0.001);
  int lbin1 = hhfit[nbins]->FindBin(u1stop  - 0.001);
  int fbin2 = hhfit[nbins]->FindBin(u2start + 0.001);
  int lbin2 = hhfit[nbins]->FindBin(u2stop  - 0.001);
  int fbin3 = hhfit[nbins]->FindBin(u3start + 0.001);
  int lbin3 = hhfit[nbins]->FindBin(u3stop  - 0.001);

  for(int j=fbin1; j<=lbin1; j++) {
    //sum1[nbins]   += (hhfit[nbins]->GetBinContent(j) - fcorrbg_fromfit[nbins]->Eval(hhfit[nbins]->GetBinCenter(j)));
    sum1[nbins]   += (hhfit[nbins]->GetBinContent(j) - fcorrbg[nbins]->Eval(hhfit[nbins]->GetBinCenter(j)));
    ersum1[nbins]   += hhfit[nbins]->GetBinError(j)*hhfit[nbins]->GetBinError(j);
    truesum1[nbins]   += hhups1[nbins]->GetBinContent(j);
  }
  ersum1[nbins] = sqrt(ersum1[nbins]);
  cout << "Direct count = " << truesum1[nbins] << " +- " << ersum1[nbins] << "  ( " << ersum1[nbins]/truesum1[nbins]*100. << "% )" << endl;

//============================================================================================
//  VS pT
//============================================================================================

int npts=10;

TCanvas* dummy = new TCanvas("dummy","dummy",0,0,500,500);

  for(int i=0; i<npts; i++) {

    fSandB->SetParameter(0,hhfit[i]->GetMaximum()/9.);
      fSandB->SetParLimits(0,0.,99999.);
    fSandB->FixParameter(1,tonypar1);
    fSandB->FixParameter(2,tonypar2);
    fSandB->FixParameter(3,tonypar3);
    fSandB->FixParameter(4,tonypar4);
    fSandB->SetParameter(5,fgpar5/1.);
      fSandB->SetParLimits(5,0.5,9999.);
    fSandB->SetParameter(6,fgpar6/1.);
      fSandB->SetParLimits(6,0.,999.);
    fSandB->SetParameter(7,bgpar0[i]/9.);
      fSandB->SetParLimits(7,0.,99.);
    fSandB->FixParameter(8,bgpar1[i]);
    hhfit[i]->Fit(fSandB,"qrl","",8.,11.);

    fCBups1s->SetParameter(0,fSandB->GetParameter(0));
    fCBups1s->SetParameter(1,fSandB->GetParameter(1));
    fCBups1s->SetParameter(2,fSandB->GetParameter(2));
    fCBups1s->SetParameter(3,fSandB->GetParameter(3));
    fCBups1s->SetParameter(4,fSandB->GetParameter(4));
    double ups1s_integral = fCBups1s->Integral(5.,14.);
    double ups1s_ampl = fSandB->GetParameter(0);
    double ups1s_amplerr = fSandB->GetParError(0);
//    cout << i << "  Integral1 = " << ups1s_integral/binsize << " +- " << ups1s_integral*(ups1s_amplerr/ups1s_ampl)/binsize << "  ( " << ups1s_amplerr/ups1s_ampl*100. << "% )" << endl;
    sumfit1[i]   = ups1s_integral/binsize;
    ersumfit1[i] = ups1s_integral*(ups1s_amplerr/ups1s_ampl)/binsize;

    fCBups2s->SetParameter(0,fSandB->GetParameter(5));
    fCBups2s->SetParameter(1,fSandB->GetParameter(1));
    fCBups2s->SetParameter(2,fSandB->GetParameter(2));
    fCBups2s->SetParameter(3,fSandB->GetParameter(3));
    fCBups2s->SetParameter(4,fSandB->GetParameter(4));
    double ups2s_integral = fCBups2s->Integral(5.,14.);
    double ups2s_ampl = fSandB->GetParameter(5);
    double ups2s_amplerr = fSandB->GetParError(5);
    cout << i << "  Integral2 = " << ups2s_integral/binsize << " +- " << ups2s_integral*(ups2s_amplerr/ups2s_ampl)/binsize << "  ( " << ups2s_amplerr/ups2s_ampl*100. << "% )" << endl;
    sumfit2[i]   = ups2s_integral/binsize;
    ersumfit2[i] = ups2s_integral*(ups2s_amplerr/ups2s_ampl)/binsize;

    sprintf(tmpname,"fcorrbg_fromfit_%d",i);   // correlated bg from fit
    fcorrbg_fromfit[i] = new TF1(tmpname,"exp([0]+[1]*x)",7.,14.);
    fcorrbg_fromfit[i]->SetLineStyle(2);
    fcorrbg_fromfit[i]->SetParameters(fSandB->GetParameter(7),fSandB->GetParameter(8));

    sprintf(tmpname,"fcorrbg_%d",i);  // true correlated bg
    fcorrbg[i] = new TF1(tmpname,"exp([0]+[1]*x)",7.,14.);
    fcorrbg[i]->SetLineWidth(1);
    fcorrbg[i]->SetLineColor(kRed);
    fcorrbg[i]->SetParameters(bgpar0[i],bgpar1[i]);

//--- vs pT direct counting ----------------------------------------------------

    sum1[i]=0.;
    truesum1[i]=0.;
    ersum1[i]=0.;
    sum1pp[i]=0.;
    ersum1pp[i]=0.;
    for(int j=fbin1; j<=lbin1; j++) {
      //sum1[i]   += (hhfit[i]->GetBinContent(j) - fcorrbg_fromfit[i]->Eval(hhfit[i]->GetBinCenter(j)));
      sum1[i]   += (hhfit[i]->GetBinContent(j) - fcorrbg[i]->Eval(hhfit[i]->GetBinCenter(j)));
      ersum1[i]   += hhfit[i]->GetBinError(j)*hhfit[i]->GetBinError(j);
      sum1pp[i]   += hhupspp[i]->GetBinContent(j);
      ersum1pp[i]   += hhupspp[i]->GetBinError(j)*hhupspp[i]->GetBinError(j);
      truesum1[i]   += hhups1[i]->GetBinContent(j);
    }
    ersum1[i] = sqrt(ersum1[i]);
    ersum1pp[i] = sqrt(ersum1pp[i]);
    //cout << "   Direct count1 = " << truesum1[i] << " +- " << ersum1[i] << "  ( " << ersum1[i]/sum1[i]*100. << "% )" << endl;

    sum2[i]=0.;
    truesum2[i]=0.;
    ersum2[i]=0.;
    sum2pp[i]=0.;
    ersum2pp[i]=0.;
    for(int j=fbin2; j<=lbin2; j++) {
      //sum2[i]   += (hhfit[i]->GetBinContent(j) - fcorrbg_fromfit[i]->Eval(hhfit[i]->GetBinCenter(j)));
      sum2[i]   += (hhfit[i]->GetBinContent(j) - fcorrbg[i]->Eval(hhfit[i]->GetBinCenter(j)));
      ersum2[i]   += hhfit[i]->GetBinError(j)*hhfit[i]->GetBinError(j);
      sum2pp[i]   += hhupspp[i]->GetBinContent(j);
      ersum2pp[i]   += hhupspp[i]->GetBinError(j)*hhupspp[i]->GetBinError(j);
      truesum2[i]   += hhups2[i]->GetBinContent(j);
    }
    ersum2[i] = sqrt(ersum2[i]);
    ersum2pp[i] = sqrt(ersum2pp[i]);
    cout << "   Direct count2 = " << truesum2[i] << " +- " << ersum2[i] << "  ( " << ersum2[i]/sum2[i]*100. << "% )" << endl;


  } // end loop over pT bins

delete dummy;

TCanvas* cupsvspt_fit = new TCanvas("cupsvspt_fit","Upsilons vs. p_{T} after fit",100,100,1100,900);
cupsvspt_fit->Divide(3,3);
for(int i=0; i<9; i++) {
if(i==0)  {cupsvspt_fit->cd(0);}
if(i==1)  {cupsvspt_fit->cd(1);}
if(i==2)  {cupsvspt_fit->cd(2);}
if(i==3)  {cupsvspt_fit->cd(3);}
if(i==4)  {cupsvspt_fit->cd(4);}
if(i==5)  {cupsvspt_fit->cd(5);}
if(i==6)  {cupsvspt_fit->cd(6);}
if(i==7)  {cupsvspt_fit->cd(7);}
if(i==8)  {cupsvspt_fit->cd(8);}
  hhfit[i]->SetAxisRange(8.0,11.0); 
  hhfit[i]->Draw();
  fcorrbg_fromfit[i]->Draw("same");
  fcorrbg[i]->Draw("same");
  sprintf(tlchar,"%d-%d GeV",i,i+1);   tl[i] = new TLatex(8.2,hhfit[i]->GetMaximum()*0.95,tlchar); tl[i]->Draw();
}




//==========================================================================
//  p+p
//==========================================================================
cout << endl << "---------- p+p ---------------------------------------" << endl << endl;

double bgpar0pp[nbins+1],bgpar1pp[nbins+1];
for(int i=0; i<nbins+1; i++) {
  bgpar0pp[i] = hhcorrbg_pp[i]->GetFunction("expo")->GetParameter(0);
  bgpar1pp[i] = hhcorrbg_pp[i]->GetFunction("expo")->GetParameter(1);
}
TF1* fcorrbg_pp[nbins+1];
TF1* fcorrbg_pp_fromfit[nbins+1];

TCanvas* cpp = new TCanvas("cpp","p+p",100,100,700,700);

  double tmpmax = hhall_pp[nbins]->GetMaximum();

  fSandBpp->SetParameter(0,tmpmax/9.);
    fSandBpp->SetParLimits(0,0.,99999.);
  fSandBpp->FixParameter(1,tonypar1);
  fSandBpp->FixParameter(2,tonypar2);
  fSandBpp->FixParameter(3,tonypar3);
  fSandBpp->FixParameter(4,0.089); // p+p mass resolution
  fSandBpp->SetParameter(5,tmpmax/99.);
    fSandBpp->SetParLimits(5,0.5,9999.);
  fSandBpp->SetParameter(6,tmpmax/99.);
    fSandBpp->SetParLimits(6,0.,999.);
  fSandBpp->SetParameter(7,bgpar0pp[nbins]/9.);
    fSandBpp->SetParLimits(7,0.,99.);
  fSandBpp->SetParameter(8,bgpar1pp[nbins]);
    hhall_pp[nbins]->Fit(fSandBpp,"qrl","",7.,12.);
    hhall_pp[nbins]->SetAxisRange(8.,11.);
    hhall_pp[nbins]->Draw();
//    hhcorrbg_pp[nbins]->GetFunction("expo")->SetLineWidth(1);
//    hhcorrbg_pp[nbins]->GetFunction("expo")->SetLineColor(kRed);
//    hhcorrbg_pp[nbins]->GetFunction("expo")->Draw("same");  // "true" correlated bg
      sprintf(tmpname,"fcorrbg_pp_%d",nbins);  // "true" correlated bg copy
      fcorrbg_pp[nbins] = new TF1(tmpname,"exp([0]+[1]*x)",7.,14.);
      fcorrbg_pp[nbins]->SetLineColor(kRed);
      fcorrbg_pp[nbins]->SetLineWidth(1);
      fcorrbg_pp[nbins]->SetParameters(bgpar0[nbins], bgpar1pp[nbins]);
      fcorrbg_pp[nbins]->Draw("same");
  
    sprintf(tmpname,"fcorrbg_pp_fromfit_%d",nbins);  // correlated bg from fit
    fcorrbg_pp_fromfit[nbins] = new TF1(tmpname,"exp([0]+[1]*x)",7.,14.);
    fcorrbg_pp_fromfit[nbins]->SetLineStyle(2);
    fcorrbg_pp_fromfit[nbins]->SetParameters(hhall_pp[nbins]->GetFunction("fSandBpp")->GetParameter(7), hhall_pp[nbins]->GetFunction("fSandBpp")->GetParameter(8));
    fcorrbg_pp_fromfit[nbins]->Draw("same");


    fCBups1spp->SetParameter(0,fSandBpp->GetParameter(0));
    fCBups1spp->SetParameter(1,fSandBpp->GetParameter(1));
    fCBups1spp->SetParameter(2,fSandBpp->GetParameter(2));
    fCBups1spp->SetParameter(3,fSandBpp->GetParameter(3));
    fCBups1spp->SetParameter(4,fSandBpp->GetParameter(4));
    double ups1spp_integral = fCBups1spp->Integral(5.,14.);
    double ups1spp_ampl = fSandBpp->GetParameter(0);
    double ups1spp_amplerr = fSandBpp->GetParError(0);
    cout << "p+p Integral1 = " << ups1spp_integral/binsize << " +- " << ups1spp_integral*(ups1spp_amplerr/ups1spp_ampl)/binsize << "  ( " << ups1spp_amplerr/ups1spp_ampl*100. << "% )" << endl;
    fCBups2spp->SetParameter(0,fSandBpp->GetParameter(5));
    fCBups2spp->SetParameter(1,fSandBpp->GetParameter(1));
    fCBups2spp->SetParameter(2,fSandBpp->GetParameter(2));
    fCBups2spp->SetParameter(3,fSandBpp->GetParameter(3));
    fCBups2spp->SetParameter(4,fSandBpp->GetParameter(4));
    double ups2spp_integral = fCBups2spp->Integral(5.,14.);
    double ups2spp_ampl = fSandBpp->GetParameter(5);
    double ups2spp_amplerr = fSandBpp->GetParError(5);
    cout << "p+p Integral2 = " << ups2spp_integral/binsize << " +- " << ups2spp_integral*(ups2spp_amplerr/ups2spp_ampl)/binsize << "  ( " << ups2spp_amplerr/ups2spp_ampl*100. << "% )" << endl;

//------ p+p vs. pT -------------------------------------------------------------------------------

TCanvas* dummy2 = new TCanvas("dummy2","dummy2",0,0,500,500);

  for(int i=0; i<npts; i++) {

  double tmpmax = hhall_pp[i]->GetMaximum()/9.;

    fSandBpp->SetParameter(0,tmpmax);
      fSandBpp->SetParLimits(0,0.,99999.);
    fSandBpp->FixParameter(1,tonypar1);
    fSandBpp->FixParameter(2,tonypar2);
    fSandBpp->FixParameter(3,tonypar3);
    fSandBpp->FixParameter(4,0.089); // p+p mass resolution
    fSandBpp->SetParameter(5,tmpmax);
      fSandBpp->SetParLimits(5,0.0,9999.);
    fSandBpp->SetParameter(6,tmpmax);
      fSandBpp->SetParLimits(6,0.0,999.);
    double tmppar7 = bgpar0pp[i]; if(tmppar7<0.) {tmppar7=0.;}
    fSandBpp->SetParameter(7,tmppar7);
      fSandBpp->SetParLimits(7,0.,99.);
    fSandBpp->FixParameter(8,bgpar1pp[i]);
      hhall_pp[i]->Fit(fSandBpp,"qrl","",8.,11.);

      sprintf(tmpname,"fcorrbg_pp_%d",i);  // "true" correlated bg copy
      fcorrbg_pp[i] = new TF1(tmpname,"exp([0]+[1]*x)",7.,14.);
      fcorrbg_pp[i]->SetLineColor(kRed);
      fcorrbg_pp[i]->SetLineWidth(1);
      fcorrbg_pp[i]->SetParameters(bgpar0pp[i], bgpar1pp[i]);

      sprintf(tmpname,"fcorrbg_pp_fromfit_%d",i);  // correlated bg from fit
      fcorrbg_pp_fromfit[i] = new TF1(tmpname,"exp([0]+[1]*x)",7.,14.);
      fcorrbg_pp_fromfit[i]->SetLineStyle(2);
      fcorrbg_pp_fromfit[i]->SetParameters(hhall_pp[i]->GetFunction("fSandBpp")->GetParameter(7), hhall_pp[i]->GetFunction("fSandBpp")->GetParameter(8));

      fCBups1spp->SetParameter(0,fSandBpp->GetParameter(0));
      fCBups1spp->SetParameter(1,fSandBpp->GetParameter(1));
      fCBups1spp->SetParameter(2,fSandBpp->GetParameter(2));
      fCBups1spp->SetParameter(3,fSandBpp->GetParameter(3));
      fCBups1spp->SetParameter(4,fSandBpp->GetParameter(4));
      double ups1spp_integral = fCBups1spp->Integral(5.,14.);
      double ups1spp_ampl = fSandBpp->GetParameter(0);
      double ups1spp_amplerr = fSandBpp->GetParError(0);
      fCBups2spp->SetParameter(0,fSandBpp->GetParameter(5));
      fCBups2spp->SetParameter(1,fSandBpp->GetParameter(1));
      fCBups2spp->SetParameter(2,fSandBpp->GetParameter(2));
      fCBups2spp->SetParameter(3,fSandBpp->GetParameter(3));
      fCBups2spp->SetParameter(4,fSandBpp->GetParameter(4));
      double ups2spp_integral = fCBups2spp->Integral(5.,14.);
      double ups2spp_ampl = fSandBpp->GetParameter(5);
      double ups2spp_amplerr = fSandBpp->GetParError(5);
      cout << "p+p Integral1 vs pT = " << i << " " << ups1spp_integral/binsize << " +- " << ups1spp_integral*(ups1spp_amplerr/ups1spp_ampl)/binsize << "  ( " << ups1spp_amplerr/ups1spp_ampl*100. << "% )" << endl;


        sumfit1pp[i]   = ups1spp_integral/binsize;
        ersumfit1pp[i] = ups1spp_integral*(ups1spp_amplerr/ups1spp_ampl)/binsize;
        sumfit2pp[i]   = ups2spp_integral/binsize;
        ersumfit2pp[i] = ups2spp_integral*(ups2spp_amplerr/ups2spp_ampl)/binsize;

        truesum1pp[i]=0.;
        truesum2pp[i]=0.;
        sum1ppbg[i]=0.;
        ersum1ppbg[i]=0.;
        sum2ppbg[i]=0.;
        ersum2ppbg[i]=0.;
        for(int j=fbin1; j<=lbin1; j++) {
          //sum1ppbg[i]   += (hhall_pp[i]->GetBinContent(j) - fcorrbg_pp_fromfit[i]->Eval(hhall_pp[i]->GetBinCenter(j)));
          sum1ppbg[i]   += hhall_pp[i]->GetBinContent(j) - fcorrbg_pp[i]->Eval(hhall_pp[i]->GetBinCenter(j));
          ersum1ppbg[i] += hhall_pp[i]->GetBinError(j)*hhall_pp[i]->GetBinError(j);
          truesum1pp[i]   += hhups1pp[i]->GetBinContent(j);
        }
        ersum1ppbg[i] = sqrt(ersum1ppbg[i]);
          for(int j=fbin2; j<=lbin2; j++) {
          sum2ppbg[i]   += hhall_pp[i]->GetBinContent(j) - fcorrbg_pp[i]->Eval(hhall_pp[i]->GetBinCenter(j));
          ersum2ppbg[i] += hhall_pp[i]->GetBinError(j)*hhall_pp[i]->GetBinError(j);
          truesum2pp[i]   += hhups2pp[i]->GetBinContent(j);
        }
        ersum2ppbg[i] = sqrt(ersum2ppbg[i]);


  } // end loop over p+p pT bins

delete dummy;

TCanvas* cupsvspt_pp = new TCanvas("cupsvspt_pp","p+p Upsilons vs. p_{T}",100,100,1100,900);
cupsvspt_pp->Divide(3,3);
for(int i=0; i<9; i++) {
if(i==0)  {cupsvspt_pp->cd(0);}
if(i==1)  {cupsvspt_pp->cd(1);}
if(i==2)  {cupsvspt_pp->cd(2);}
if(i==3)  {cupsvspt_pp->cd(3);}
if(i==4)  {cupsvspt_pp->cd(4);}
if(i==5)  {cupsvspt_pp->cd(5);}
if(i==6)  {cupsvspt_pp->cd(6);}
if(i==7)  {cupsvspt_pp->cd(7);}
if(i==8)  {cupsvspt_pp->cd(8);}
  hhall_pp[i]->SetAxisRange(8.0,11.0);
  hhall_pp[i]->Draw();
  fcorrbg_pp[i]->Draw("same");
  fcorrbg_pp_fromfit[i]->Draw("same");
  sprintf(tlchar,"%d-%d GeV",i,i+1);   tl[i] = new TLatex(8.2,hhall_pp[i]->GetMaximum()*0.95,tlchar); tl[i]->Draw();
}

//
//=== compare errors from direct counting and fit
//

TCanvas* cercomp = new TCanvas("cercomp","error comparizon",50,50,700,700);
TH2F* hhcomp = new TH2F("hhcomp"," ",10,0.,10.,10,0.,1.0);
hhcomp->GetXaxis()->SetTitle("Transverse momentum [GeV/c]");
hhcomp->GetXaxis()->SetTitleOffset(1.0);
hhcomp->Draw();

double reler1[nbins+1],reler2[nbins+1];
double relerfit1[nbins+1],relerfit2[nbins+1];
double reler1pp[nbins+1],reler2pp[nbins+1];
double reler1ppbg[nbins+1],reler2ppbg[nbins+1];
double relerfit1pp[nbins+1],relerfit2pp[nbins+1];
for(int i=0; i<npts; i++) { 
  reler1[i] = ersum1[i]/truesum1[i];           reler2[i] = ersum2[i]/truesum2[i]; 
  relerfit1[i] = ersumfit1[i]/Nups1[i];        relerfit2[i] = ersumfit2[i]/Nups2[i]; 
  reler1pp[i] = ersum1pp[i]/truesum1pp[i];     reler2pp[i] = ersum2pp[i]/truesum2pp[i];     // direct count no bg
  reler1ppbg[i] = ersum1ppbg[i]/truesum1pp[i]; reler2ppbg[i] = ersum2ppbg[i]/truesum2pp[i]; // direct count with bg
  relerfit1pp[i] = ersumfit1pp[i]/Nups1pp[i];  relerfit2pp[i] = ersumfit2pp[i]/Nups2pp[i];  // fit
}

TGraphErrors* grcomp1 = new TGraphErrors(npts,xx1,reler1,0,0);
grcomp1->SetMarkerStyle(20);
grcomp1->SetMarkerColor(kBlack);
grcomp1->SetLineColor(kBlack);
grcomp1->SetLineWidth(2);
grcomp1->SetMarkerSize(1.5);
grcomp1->Draw("p");

TGraphErrors* grcompfit1 = new TGraphErrors(npts,xx1,relerfit1,0,0);
grcompfit1->SetMarkerStyle(24);
grcompfit1->SetMarkerColor(kBlack);
grcompfit1->SetLineColor(kBlack);
grcompfit1->SetLineWidth(2);
grcompfit1->SetMarkerSize(1.5);
grcompfit1->Draw("p");

TCanvas* cercomppp = new TCanvas("cercomppp","p+p error comparizon",50,50,700,700);
TH2F* hhcomppp = new TH2F("hhcomppp"," ",10,0.,10.,10,0.,1.0);
hhcomppp->GetXaxis()->SetTitle("Transverse momentum [GeV/c]");
hhcomppp->GetXaxis()->SetTitleOffset(1.0);
hhcomppp->Draw();

TGraphErrors* grcomp1pp = new TGraphErrors(npts,xx1,reler1pp,0,0);
grcomp1pp->SetMarkerStyle(20);
grcomp1pp->SetMarkerColor(kBlue);
grcomp1pp->SetLineColor(kBlue);
grcomp1pp->SetLineWidth(2);
grcomp1pp->SetMarkerSize(1.5);
grcomp1pp->Draw("p");

TGraphErrors* grcomp1ppbg = new TGraphErrors(npts,xx1,reler1ppbg,0,0);
grcomp1ppbg->SetMarkerStyle(21);
grcomp1ppbg->SetMarkerColor(kBlue);
grcomp1ppbg->SetLineColor(kBlue);
grcomp1ppbg->SetLineWidth(2);
grcomp1ppbg->SetMarkerSize(1.5);
grcomp1ppbg->Draw("p");

TGraphErrors* grcompfit1pp = new TGraphErrors(npts,xx1,relerfit1pp,0,0);
grcompfit1pp->SetMarkerStyle(24);
grcompfit1pp->SetMarkerColor(kBlue);
grcompfit1pp->SetLineColor(kBlue);
grcompfit1pp->SetLineWidth(2);
grcompfit1pp->SetMarkerSize(1.5);
grcompfit1pp->Draw("p");


//==========================================================================
//  RAA
//==========================================================================

double raa1[nbins+1],raa2[nbins+1],raa3[nbins+1],erraa1[nbins+1],erraa2[nbins+1],erraa3[nbins+1];
double raa1fit[nbins+1],raa2fit[nbins+1],raa3fit[nbins+1],erraa1fit[nbins+1],erraa2fit[nbins+1],erraa3fit[nbins+1];

for(int i=0; i<nbins; i++) { 

  raa1[i] = 0.535 * grRAA1S->Eval(0.5+i*1.0)/groldRAA1S->Eval(0.5+i*1.0); 
  raa2[i] = 0.170 * grRAA2S->Eval(0.5+i*1.0)/groldRAA2S->Eval(0.5+i*1.0); 
  raa3[i] = 0.035; 
  raa1fit[i] = 0.535 * grRAA1S->Eval(0.5+i*1.0)/groldRAA1S->Eval(0.5+i*1.0); 
  raa2fit[i] = 0.170 * grRAA2S->Eval(0.5+i*1.0)/groldRAA2S->Eval(0.5+i*1.0); 
  raa3fit[i] = 0.035; 

  erraa1[i] = raa1[i]*sqrt(pow(ersum1[i]/truesum1[i],2)+pow(ersum1pp[i]/truesum1pp[i],2));
  erraa2[i] = raa2[i]*sqrt(pow(ersum2[i]/truesum2[i],2)+pow(ersum2pp[i]/truesum2pp[i],2));
  erraa1fit[i] = raa1fit[i]*sqrt(pow(ersumfit1[i]/Nups1[i],2)+pow(ersumfit1pp[i]/Nups1pp[i],2));
  erraa2fit[i] = raa2fit[i]*sqrt(pow(ersumfit2[i]/Nups2[i],2)+pow(ersumfit2pp[i]/Nups2pp[i],2));

}

// plot icomparison direct counting vs. fit

TCanvas* craa = new TCanvas("craa","R_{AA}",120,120,600,600);
TH2F* hh2 = new TH2F("hh2"," ",10,0.,10.,10,0.,0.8);
hh2->GetXaxis()->SetTitle("Transverse momentum [GeV/c]");
hh2->GetXaxis()->SetTitleOffset(1.0);
hh2->GetXaxis()->SetTitleColor(1);
hh2->GetXaxis()->SetTitleSize(0.040);
hh2->GetXaxis()->SetLabelSize(0.040);
hh2->GetYaxis()->SetTitle("R_{AA}");
hh2->GetYaxis()->SetTitleOffset(1.3);
hh2->GetYaxis()->SetTitleSize(0.040);
hh2->GetYaxis()->SetLabelSize(0.040);
hh2->Draw();

TGraphErrors* gr1 = new TGraphErrors(npts,xx2,raa1,0,erraa1);
gr1->SetMarkerStyle(20);
gr1->SetMarkerColor(kBlack);
gr1->SetLineColor(kBlack);
gr1->SetLineWidth(2);
gr1->SetMarkerSize(1.5);
gr1->Draw("p");

TGraphErrors* gr1fit = new TGraphErrors(npts,xx3,raa1fit,0,erraa1fit);
gr1fit->SetMarkerStyle(24);
gr1fit->SetMarkerColor(kBlack);
gr1fit->SetLineColor(kBlack);
gr1fit->SetLineWidth(2);
gr1fit->SetMarkerSize(1.5);
gr1fit->Draw("p");

TGraphErrors* gr2 = new TGraphErrors(npts-2,xx2,raa2,0,erraa2);
gr2->SetMarkerStyle(20);
gr2->SetMarkerColor(kRed);
gr2->SetLineColor(kRed);
gr2->SetLineWidth(2);
gr2->SetMarkerSize(1.5);
gr2->Draw("p");

TGraphErrors* gr2fit = new TGraphErrors(npts-2,xx3,raa2fit,0,erraa2fit);
gr2fit->SetMarkerStyle(24);
gr2fit->SetMarkerColor(kRed);
gr2fit->SetLineColor(kRed);
gr2fit->SetLineWidth(2);
gr2fit->SetMarkerSize(1.5);
gr2fit->Draw("p");

delete dummy;

// plot fit

TCanvas* craafit = new TCanvas("craafit","R_{AA}",20,20,600,600);
TH2F* hh2f = new TH2F("hh2f"," ",10,0.,10.,10,0.,0.8);
hh2f->GetXaxis()->SetTitle("Transverse momentum [GeV/c]");
hh2f->GetXaxis()->SetTitleOffset(1.0);
hh2f->GetXaxis()->SetTitleColor(1);
hh2f->GetXaxis()->SetTitleSize(0.040);
hh2f->GetXaxis()->SetLabelSize(0.040);
hh2f->GetYaxis()->SetTitle("R_{AA}");
hh2f->GetYaxis()->SetTitleOffset(1.3);
hh2f->GetYaxis()->SetTitleSize(0.040);
hh2f->GetYaxis()->SetLabelSize(0.040);
hh2f->Draw();

TGraphErrors* gr1f = new TGraphErrors(npts,xx1,raa1fit,0,erraa1fit);
gr1f->SetMarkerStyle(20);
gr1f->SetMarkerColor(kBlack);
gr1f->SetLineColor(kBlack);
gr1f->SetLineWidth(2);
gr1f->SetMarkerSize(1.5);
gr1f->SetName("gr2c");
gr1f->Draw("p");

TGraphErrors* gr2f = new TGraphErrors(npts-2,xx1,raa2fit,0,erraa2fit);
gr2f->SetMarkerStyle(20);
gr2f->SetMarkerColor(kRed);
gr2f->SetLineColor(kRed);
gr2f->SetLineWidth(2);
gr2f->SetMarkerSize(1.5);
gr2f->SetName("gr2f");
gr2f->Draw("p");

TLegend *leg = new TLegend(0.2,0.77,0.5,0.92);
leg->SetFillColor(10);
leg->SetFillStyle(1001);
//TLegendEntry *entry=leg->AddEntry("gr1f","Y(1S)","p");
//entry=leg->AddEntry("gr2f","Y(2S)","p");
//entry=leg->AddEntry("","","");
leg->AddEntry("gr1f","Y(1S)","p");
leg->AddEntry("gr2f","Y(2S)","p");
leg->AddEntry("","","");
leg->Draw();
TLatex* l1 = new TLatex(3.5,0.73,"sPHENIX Simulation");
l1->Draw();

grRAA1S->Draw("l");
grRAA2S->Draw("l");

// plot direct count

TCanvas* craacount = new TCanvas("craacount","R_{AA}",20,20,600,600);
TH2F* hh2c = new TH2F("hh2c"," ",10,0.,10.,10,0.,0.8);
hh2c->GetXaxis()->SetTitle("Transverse momentum [GeV/c]");
hh2c->GetXaxis()->SetTitleOffset(1.0);
hh2c->GetXaxis()->SetTitleColor(1);
hh2c->GetXaxis()->SetTitleSize(0.040);
hh2c->GetXaxis()->SetLabelSize(0.040);
hh2c->GetYaxis()->SetTitle("R_{AA}");
hh2c->GetYaxis()->SetTitleOffset(1.3);
hh2c->GetYaxis()->SetTitleSize(0.040);
hh2c->GetYaxis()->SetLabelSize(0.040);
hh2c->Draw();

TGraphErrors* gr1c = new TGraphErrors(npts,xx1,raa1,0,erraa1);
gr1c->SetMarkerStyle(20);
gr1c->SetMarkerColor(kBlack);
gr1c->SetLineColor(kBlack);
gr1c->SetLineWidth(2);
gr1c->SetMarkerSize(1.5);
gr1c->SetName("gr1c");
gr1c->Draw("p");

TGraphErrors* gr2c = new TGraphErrors(npts-3,xx1,raa2,0,erraa2);
gr2c->SetMarkerStyle(20);
gr2c->SetMarkerColor(kRed);
gr2c->SetLineColor(kRed);
gr2c->SetLineWidth(2);
gr2c->SetMarkerSize(1.5);
gr2c->SetName("gr2c");
gr2c->Draw("p");

TLegend *legc = new TLegend(0.2,0.77,0.5,0.92);
legc->SetFillColor(10);
legc->SetFillStyle(1001);
legc->AddEntry("gr1c","Y(1S)","p");
legc->AddEntry("gr2c","Y(2S)","p");
legc->AddEntry("","","");
//TLegendEntry *entry2=legc->AddEntry("gr1c","Y(1S)","p");
//entry2=legc->AddEntry("gr2c","Y(2S)","p");
//entry2=legc->AddEntry("","","");
legc->Draw();
TLatex* l1c = new TLatex(3.5,0.73,"sPHENIX Simulation");
l1c->Draw();

grRAA1S->Draw("l");
grRAA2S->Draw("l");

}

