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

//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------

void newplotbg_vscent_2022() {

//gROOT->LoadMacro("sPHENIXStyle/sPhenixStyle.C");
//SetsPhenixStyle();

gStyle->SetOptStat(0);
gStyle->SetOptFit(0);

TRandom* myrandom = new TRandom3();
const int nbins = 7;
double eideff = 0.9;
string times = "*";
TLatex* tl[15];
char tlchar[999];

double statscale;
statscale = 1.4; // from 10B to 14B
double statscale_lowlim = 7.0;
double statscale_uplim = 14.0;

  TF1* fCBpp = new TF1("fCBpp",CBFunction,5.,14.,5);
  TF1* fCBauau = new TF1("fCBauau",CBFunction,5.,14.,5);
  TF1* fCB1s = new TF1("fCB1s",CBFunction,5.,14.,5);
  TF1* fCB2s = new TF1("fCB2s",CBFunction,5.,14.,5);
  TF1* fCB3s = new TF1("fCB3s",CBFunction,5.,14.,5);
  TF1* fTCB = new TF1("fTCB",TripleCBFunction,5.,14.,7);
  TF1* fTCBpp = new TF1("fTCBpp",TripleCBFunction,5.,14.,7);
  TF1* fTCBauau = new TF1("fTCBauau",TripleCBFunction,5.,14.,7);
  TF1* fSandB = new TF1("fSandB",SandB_CBFunction,5.,14.,9);
  TF1* fSandBfordave = new TF1("fSandBfordave",SandB_CBFunction,5.,14.,9);
  TF1* fSandBpp = new TF1("fSandBpp",SandB_CBFunction,5.,14.,9);
  TF1* fSandBauau = new TF1("fSandBauau",SandB_CBFunction,5.,14.,9);

//---------------------------------------------------------
// Upsilons
//---------------------------------------------------------

 // Input assumptions about level of suppression are taken from the theory paper: 
 // Michael Strickland, Dennis Bazow, Nucl.Phys. A879 (2012) 25-58
 //==============================================
 // Read in Strickland curves for different eta/s values
 // These are exclusive - i.e. they are the 
 // suppression of that state with no feed-down
 // effects included 
 //==============================================
 // The curves vs eta/s read in below were provided by Strickland privately, since
 // the paper contains only the eta/s = 1 cases for RHIC energy
 // Get the exclusive raa values for the Y1S, Y2S, Y3S, chib1, chib2
 // Checked that this is being read in correctly
                        
  double str_npart[101];
  double str_raa[5][3][101];
  for(int istate=0;istate<5;istate++)
    {
      for(int ietas=0;ietas<3;ietas++)
        {
          if(istate < 3)
            {
              char fname[500];
              sprintf(fname,"./strickland_calculations/Y%is-potb-eta%i-npart.dat",istate+1,ietas+1);
              ifstream fin(fname);

              for(int inpart=0;inpart<101;inpart++)
                {
                  fin >> str_npart[inpart] >> str_raa[istate][ietas][inpart];
                }
              fin.close();
            }

          if(istate > 2)
            {
              char fname[500];
              sprintf(fname,"./strickland_calculations/chib%i-potb-eta%i-npart.dat",istate-2,ietas+1);
              ifstream fin(fname);

              for(int inpart=0;inpart<101;inpart++)
                {
                  fin >> str_npart[inpart] >> str_raa[istate][ietas][inpart];
                }
              fin.close();
            }
        }

    }

  // Now construct the inclusive RAA values from Strickland's exclusive modifications
  // ff's from table II of the paper for the 1S state are as follows
  // They add up to 1.0
  double ff1S[5] = {0.51, 0.107, 0.008, 0.27, 0.105};  // Y1S, Y2S, Y3S, chib1, chib2 respectively
  // These are from arXiv:1210.7512
  double ff2S[2] = {0.5, 0.5};                         // Y2S and Y3S respectively
  double str_raa_inclusive[3][3][101];

  // checked the Y1S inclusive result against figure 10 of arXiv:1112.2761
  // There is no plot available for the 2S and 3S 
  for(int ietas=0;ietas<3;ietas++)
    for(int inpart=0;inpart<101;inpart++)
      {
        str_raa_inclusive[0][ietas][inpart] =
          str_raa[0][ietas][inpart] * ff1S[0]
          + str_raa[1][ietas][inpart] * ff1S[1]
          + str_raa[2][ietas][inpart] * ff1S[2]
          + str_raa[3][ietas][inpart] * ff1S[3]
          + str_raa[4][ietas][inpart] * ff1S[4];

        str_raa_inclusive[1][ietas][inpart] =
          str_raa[1][ietas][inpart] * ff2S[0]
          + str_raa[2][ietas][inpart] * ff2S[1];

        str_raa_inclusive[2][ietas][inpart] = str_raa[2][ietas][inpart];
      }

  double strick_raa1_eta1[101],strick_raa1_eta2[101],strick_raa1_eta3[101];
  double strick_raa2_eta1[101],strick_raa2_eta2[101],strick_raa2_eta3[101];
  double strick_raa3_eta1[101],strick_raa3_eta2[101],strick_raa3_eta3[101];
    for(int ipart=0;ipart<101;ipart++) {
      strick_raa1_eta1[ipart] = str_raa_inclusive[0][0][ipart];
      strick_raa1_eta2[ipart] = str_raa_inclusive[0][1][ipart];
      strick_raa1_eta3[ipart] = str_raa_inclusive[0][2][ipart];
      strick_raa2_eta1[ipart] = str_raa_inclusive[1][0][ipart];
      strick_raa2_eta2[ipart] = str_raa_inclusive[1][1][ipart];
      strick_raa2_eta3[ipart] = str_raa_inclusive[1][2][ipart];
      strick_raa3_eta1[ipart] = str_raa_inclusive[2][0][ipart];
      strick_raa3_eta2[ipart] = str_raa_inclusive[2][1][ipart];
      strick_raa3_eta3[ipart] = str_raa_inclusive[2][2][ipart];
    }

//======================================================================================
//======================================================================================
//======================================================================================

TGraph* grRAA1S = new TGraph(101,str_npart,strick_raa1_eta2);
TGraph* grRAA2S = new TGraph(101,str_npart,strick_raa2_eta2);
TGraph* grRAA3S = new TGraph(101,str_npart,strick_raa3_eta2);
TGraph* grRAA1S_eta1 = new TGraph(101,str_npart,strick_raa1_eta1);
TGraph* grRAA2S_eta1 = new TGraph(101,str_npart,strick_raa2_eta1);
TGraph* grRAA3S_eta1 = new TGraph(101,str_npart,strick_raa3_eta1);
TGraph* grRAA1S_eta3 = new TGraph(101,str_npart,strick_raa1_eta3);
TGraph* grRAA2S_eta3 = new TGraph(101,str_npart,strick_raa2_eta3);
TGraph* grRAA3S_eta3 = new TGraph(101,str_npart,strick_raa3_eta3);
grRAA1S->SetLineColor(kBlack);
grRAA1S->SetLineStyle(7);
grRAA2S->SetLineColor(kRed);
grRAA2S->SetLineStyle(7);
grRAA3S->SetLineColor(kBlue);
grRAA3S->SetLineStyle(7);
grRAA1S_eta1->SetLineColor(kBlack);
grRAA1S_eta1->SetLineStyle(1);
grRAA2S_eta1->SetLineColor(kRed);
grRAA2S_eta1->SetLineStyle(1);
grRAA3S_eta1->SetLineColor(kBlue);
grRAA3S_eta1->SetLineStyle(1);
grRAA1S_eta3->SetLineColor(kBlack);
grRAA1S_eta3->SetLineStyle(8);
grRAA2S_eta3->SetLineColor(kRed);
grRAA2S_eta3->SetLineStyle(8);
grRAA3S_eta3->SetLineColor(kBlue);
grRAA3S_eta3->SetLineStyle(8);

int nchan=400;
double start=0.0;
double stop=20.0;

string str_UpsilonPt  = "(2.0*3.14159*x*[0]*pow((1 + x*x/(4*[1]) ),-[2]))";   // dN/dpT
string str_UpsilonXPt = "(2.0*3.14159*x*x*[0]*pow((1 + x*x/(4*[1]) ),-[2]))"; // need this for mean pT calculation
TF1* fUpsilonPt = new TF1("fUpsilonPt",str_UpsilonPt.c_str(),0.,20.);
TF1* fUpsilonXPt = new TF1("fUpsilonXPt",str_UpsilonXPt.c_str(),0.,20.);
fUpsilonPt->SetParameters(72.1, 26.516, 10.6834);
fUpsilonXPt->SetParameters(72.1, 26.516, 10.6834);
double upsnorm = fUpsilonPt->Integral(0.,20.);

double frac[3];  // upsilons fractions
  frac[0] = 0.7117;
  frac[1] = 0.1851;
  frac[2] = 0.1032;
double scale[3]; // mass scaling
  scale[0] = 1.0;
  scale[1] = 1.0595;
  scale[2] = 1.0946;


double Npart[nbins+1], NpartAvg=0.;
Npart[0] = 325.;
Npart[1] = 235.;
Npart[2] = 167.;
Npart[3] = 114.;
Npart[4] = 74.;
Npart[5] = 46.;
Npart[6] = 14.5;
for(int i=0; i<6; i++) {NpartAvg += Npart[i];} NpartAvg = NpartAvg/6.;
cout << "Npart for 0-60% centrality = " << NpartAvg << endl;
cout << "Raa for 0-60% centrality = " << grRAA1S->Eval(NpartAvg) << " " << grRAA2S->Eval(NpartAvg) << " " << grRAA3S->Eval(NpartAvg) << endl;

double Ncoll[nbins+1];
Ncoll[0] = 955.;
Ncoll[1] = 603.;
Ncoll[2] = 374.;
Ncoll[3] = 220.;
Ncoll[4] = 120.;
Ncoll[5] = 61.;
Ncoll[6] = 14.5;
double NcollAvg=0.; for(int i=0; i<6; i++) {NcollAvg += Ncoll[i];} NcollAvg = NcollAvg/6.;
cout << "Ncoll for 0-60% centrality = " << NcollAvg << endl;

double Npionpairs[nbins+1];
Npionpairs[0] = 1.000;
Npionpairs[1] = 0.674;
Npionpairs[2] = 0.418;
Npionpairs[3] = 0.212;
Npionpairs[4] = 0.099;
Npionpairs[5] = 0.037;
Npionpairs[6] = 0.033;  // This includes the fact that this bin is 3.2 times wider
double NpionpairsAvg=0.; for(int i=0; i<6; i++) {NpionpairsAvg += Npionpairs[i];} NpionpairsAvg = NpionpairsAvg/6.;
cout << "Npionpairs for 0-60% centrality = " << NpionpairsAvg << endl;

//---------- Use fixed numbers for 2022 estimate --------------------------------------
// Numbers from Marzia for 140 B MB Au+Au and 2400 B p+p events
// RAA for Y(3S) is 8.5%

  int Nups1[nbins],Nups2[nbins],Nups3[nbins];
  int Nups1tot=0, Nups2tot=0, Nups3tot=0;

  Nups1[0]   = 7011;
  Nups1[1]   = 4807;
  Nups1[2]   = 3304;
  Nups1[3]   = 2185;
  Nups1[4]   = 1344;
  Nups1[5]   = 754;
  Nups1[6]   = 634;
  for(int i=0; i<6; i++) {Nups1tot += Nups1[i];}

  Nups2[0]   = 562;
  Nups2[1]   = 436;
  Nups2[2]   = 344;
  Nups2[3]   = 264;
  Nups2[4]   = 191;
  Nups2[5]   = 123;
  Nups2[6]   = 156;
  for(int i=0; i<6; i++) {Nups2tot += Nups2[i];}

  Nups3[0]   = 156;
  Nups3[1]   = 120;
  Nups3[2]   = 95;
  Nups3[3]   = 73;
  Nups3[4]   = 53;
  Nups3[5]   = 52;
  Nups3[6]   = 86;
  for(int i=0; i<6; i++) {Nups3tot += Nups3[i];}
  cout << "Number of Upsilons in 0-60% centrality = " << Nups1tot << " " << Nups2tot << " " << Nups3tot << endl;


  int Nups1pp = 2.86e+03;
  int Nups2pp = 7.16e+02;
  int Nups3pp = 3.98e+02;

//====================================================================================
//====================================================================================
//====================================================================================

//
// these histograms (hhups*) are randomly generated using the fit above to single 
// peak (fCB function) and used for the RAA uncertainty calculation
//
// FORCE specific RESOLUTION and tail shape
  double tonypar1 = 0.98;  // Tony's 100 pion simulation April 2019
  double tonypar2 = 0.93; // Tony's 100 pion simulation April 2019
  //double tonypar3 = 9.437;  // Tony's 100 pion simulation April 2019
  double tonypar3 = 9.448;  // benchmark
  double tonypar4 = 0.100;  // benchmark
  double tonypar4pp = 0.089; // Tony's 100 pion simulation April 2019
  fCBpp->SetParameter(0,1000.); 
  fCBpp->SetParameter(1,tonypar1); 
  fCBpp->SetParameter(2,tonypar2); 
  fCBpp->SetParameter(3,tonypar3); 
  fCBpp->SetParameter(4,tonypar4pp);  
  fCBauau->SetParameter(0,1000.); 
  fCBauau->SetParameter(1,tonypar1); 
  fCBauau->SetParameter(2,tonypar2); 
  fCBauau->SetParameter(3,tonypar3); 
  fCBauau->SetParameter(4,tonypar4);  

char hhname[999];
TH1D* hhups[nbins+1];
TH1D* hhups1[nbins+1];
TH1D* hhups2[nbins+1];
TH1D* hhups3[nbins+1];
TH1D* hhupspp;
TH1D* hhups1pp;
TH1D* hhups2pp;
TH1D* hhups3pp;
for(int i=0; i<nbins+1; i++) {
  sprintf(hhname,"hhups_%d",i);
  hhups[i] = new TH1D(hhname,"",nchan,start,stop);
  hhups[i]->Sumw2();
  sprintf(hhname,"hhups1_%d",i);
  hhups1[i] = new TH1D(hhname,"",nchan,start,stop);
  hhups1[i]->Sumw2();
  sprintf(hhname,"hhups2_%d",i);
  hhups2[i] = new TH1D(hhname,"",nchan,start,stop);
  hhups2[i]->Sumw2();
  sprintf(hhname,"hhups3_%d",i);
  hhups3[i] = new TH1D(hhname,"",nchan,start,stop);
  hhups3[i]->Sumw2();
  hhups[i]->SetLineWidth(2);
  hhups1[i]->SetLineWidth(2);
  hhups2[i]->SetLineWidth(2);
  hhups3[i]->SetLineWidth(2);
}
  sprintf(hhname,"hhupspp");
  hhupspp= new TH1D(hhname,"",nchan,start,stop);
  hhupspp->Sumw2();
  sprintf(hhname,"hhups1pp");
  hhups1pp = new TH1D(hhname,"",nchan,start,stop);
  hhups1pp->Sumw2();
  sprintf(hhname,"hhups2pp");
  hhups2pp = new TH1D(hhname,"",nchan,start,stop);
  hhups2pp->Sumw2();
  sprintf(hhname,"hhups3pp");
  hhups3pp = new TH1D(hhname,"",nchan,start,stop);
  hhups3pp->Sumw2();
  hhupspp->SetLineWidth(2);
  hhups1pp->SetLineWidth(2);
  hhups2pp->SetLineWidth(2);
  hhups3pp->SetLineWidth(2);

for(int j=0; j<nbins; j++) {
  double s1 = j*1.0;
  double s2 = s1 + 1.0;
  fCBauau->SetParameter(3,tonypar3);
    for(int i=0; i<int(Nups1[j]+0.5); i++) { double myrnd = fCBauau->GetRandom(); hhups1[j]->Fill(myrnd); hhups[j]->Fill(myrnd); }
  fCBauau->SetParameter(3,tonypar3*scale[1]);
    for(int i=0; i<int(Nups2[j]+0.5); i++) { double myrnd = fCBauau->GetRandom(); hhups2[j]->Fill(myrnd); hhups[j]->Fill(myrnd); }
  fCBauau->SetParameter(3,tonypar3*scale[2]);
    for(int i=0; i<int(Nups3[j]+0.5); i++) { double myrnd = fCBauau->GetRandom(); hhups3[j]->Fill(myrnd); hhups[j]->Fill(myrnd); }
} // end loop over centrality bins

  fCBpp->SetParameter(3,tonypar3);
    for(int i=0; i<int(Nups1pp+0.5); i++) { double myrnd = fCBpp->GetRandom(); hhups1pp->Fill(myrnd); hhupspp->Fill(myrnd); }
  fCBpp->SetParameter(3,tonypar3*scale[1]);
    for(int i=0; i<int(Nups2pp+0.5); i++) { double myrnd = fCBpp->GetRandom(); hhups2pp->Fill(myrnd); hhupspp->Fill(myrnd); }
  fCBpp->SetParameter(3,tonypar3*scale[2]);
    for(int i=0; i<int(Nups3pp+0.5); i++) { double myrnd = fCBpp->GetRandom(); hhups3pp->Fill(myrnd); hhupspp->Fill(myrnd); }


// fit and draw pp no bg

TCanvas* cupspp = new TCanvas("cupspp","Upsilons in p+p",100,100,600,600);
  fTCBpp->SetParameter(0,2000.);
  fTCBpp->FixParameter(1,tonypar1);
  fTCBpp->FixParameter(2,tonypar2);
  fTCBpp->SetParameter(3,tonypar3);
  fTCBpp->FixParameter(4,tonypar4); // fix width from the single peak fit
  fTCBpp->SetParameter(5,500.);
  fTCBpp->SetParameter(6,100.);
  hhupspp->Fit(fTCBpp,"rl","",7.,11.);
  hhupspp->SetAxisRange(7.,11.);
  hhupspp->SetMarkerSize(1.0);
  hhupspp->GetXaxis()->SetTitle("Invariant mass [GeV/c^{2}]");
  hhupspp->GetXaxis()->SetTitleOffset(1.0);
  double tmpamp1 = hhupspp->GetFunction("fTCBpp")->GetParameter(0);
  double tmpamp5 = tmpamp1*frac[1]/frac[0]; // correct fit for correct upsilon states ratio
  double tmpamp6 = tmpamp1*frac[2]/frac[0];
  hhupspp->Draw();

  fCB1s->SetLineColor(kBlue);
  fCB1s->SetLineWidth(1);
    fCB1s->SetParameter(0,fTCBpp->GetParameter(0));
    fCB1s->SetParameter(1,fTCBpp->GetParameter(1));
    fCB1s->SetParameter(2,fTCBpp->GetParameter(2));
    fCB1s->SetParameter(3,fTCBpp->GetParameter(3)*scale[0]);
    fCB1s->SetParameter(4,fTCBpp->GetParameter(4));
  fCB2s->SetLineColor(kRed);
  fCB2s->SetLineWidth(1);
    fCB2s->SetParameter(0,tmpamp5);
    fCB2s->SetParameter(1,fTCBpp->GetParameter(1));
    fCB2s->SetParameter(2,fTCBpp->GetParameter(2));
    fCB2s->SetParameter(3,fTCBpp->GetParameter(3)*scale[1]);
    fCB2s->SetParameter(4,fTCBpp->GetParameter(4));
  fCB3s->SetLineColor(kGreen+2);
  fCB3s->SetLineWidth(1);
    fCB3s->SetParameter(0,tmpamp6);
    fCB3s->SetParameter(1,fTCBpp->GetParameter(1));
    fCB3s->SetParameter(2,fTCBpp->GetParameter(2));
    fCB3s->SetParameter(3,fTCBpp->GetParameter(3)*scale[2]);
    fCB3s->SetParameter(4,fTCBpp->GetParameter(4));
  fCB1s->Draw("same");
  fCB2s->Draw("same");
  fCB3s->Draw("same");

// fit and draw AuAu no bg

TCanvas* cupsauau = new TCanvas("cupsauau","Upsilons in Central Au+Au",100,100,600,600);
  fTCBauau->SetParameter(0,2000.);
  fTCBauau->FixParameter(1,tonypar1);
  fTCBauau->FixParameter(2,tonypar2);
  fTCBauau->SetParameter(3,tonypar3);
  fTCBauau->FixParameter(4,tonypar4); // fix width from the single peak fit
  fTCBauau->SetParameter(5,500.);
  fTCBauau->SetParameter(6,100.);
  hhups[0]->Fit(fTCBauau,"rl","",7.,11.);
  hhups[0]->SetAxisRange(8.5,11.);
  hhups[0]->SetMarkerSize(1.0);
  hhups[0]->GetXaxis()->SetTitle("Invariant mass [GeV/c^{2}]");
  hhups[0]->GetXaxis()->SetTitleOffset(1.0);
//  tmpamp1 = hhups[nbins]->GetFunction("fTCBauau")->GetParameter(0);
//  tmpamp5 = tmpamp1*frac[1]/frac[0]; // correct fit for correct upsilon states ratio
//  tmpamp6 = tmpamp1*frac[2]/frac[0];
  hhups[0]->Draw();  // 0-10% central

TCanvas* cdummy1 = new TCanvas("cdummy1","cdummy1",0,0,500,500);
//----------------------------------------------------
//  Backgrounds
//----------------------------------------------------

TH1D* hhall[nbins+1];
TH1D* hhall_scaled[nbins+1];

TH1D* hhtotbg[nbins+1]; 
TH1D* hhtotbg_scaled[nbins+1]; 
TH1D* hhcombbg[nbins+1];
TH1D* hhcombbg_scaled[nbins+1];
TH1D* hhfakefake[nbins+1];
TH1D* hhfakehf[nbins+1];
TH1D* hhbottom[nbins+1];
TH1D* hhcharm[nbins+1];
TH1D* hhdy[nbins+1];
TH1D* hhcorrbg[nbins+1];
TH1D* hhcorrbg_scaled[nbins+1];
TH1D* hhfit[nbins+1];
char tmpname[999];

//----------------------------------------------------------------------------------------
// Correlated BG calculated for 10B central AuAu events
//----------------------------------------------------------------------------------------

double corrbgfitpar0;
double corrbgfitpar1;

TFile* f=new TFile("ccbb_eideff09.root");

  sprintf(tmpname,"hhbottom_15");  // 15 is integrated over pT
  hhbottom[nbins] = (TH1D*)f->Get(tmpname);
  hhbottom[nbins]->SetDirectory(gROOT);
  sprintf(tmpname,"hhcharm_15");
  hhcharm[nbins] = (TH1D*)f->Get(tmpname);
  hhcharm[nbins]->SetDirectory(gROOT);
  sprintf(tmpname,"hhdy_15");
  hhdy[nbins] = (TH1D*)f->Get(tmpname);
  hhdy[nbins]->SetDirectory(gROOT);
  sprintf(tmpname,"hhcorrbg_15");
  hhcorrbg[nbins] = (TH1D*)hhbottom[nbins]->Clone(tmpname);
  hhcorrbg[nbins]->Add(hhcharm[nbins]);
  hhcorrbg[nbins]->Add(hhdy[nbins]);
  sprintf(tmpname,"hhcorrbg_scaled_15");
  hhcorrbg_scaled[nbins] = (TH1D*)hhcorrbg[nbins]->Clone(tmpname);
  hhcorrbg[nbins]->Fit("expo","rql","",statscale_lowlim,statscale_uplim);
  hhbottom[nbins]->Fit("expo","rql","",statscale_lowlim,statscale_uplim);
  hhdy[nbins]->Fit("expo","rql","",statscale_lowlim,statscale_uplim);
    corrbgfitpar0 = hhcorrbg[nbins]->GetFunction("expo")->GetParameter(0);
    corrbgfitpar1 = hhcorrbg[nbins]->GetFunction("expo")->GetParameter(1);
    cout << "bgpar0["<< nbins <<"]="<<hhcorrbg[nbins]->GetFunction("expo")->GetParameter(0)+TMath::Log(statscale)<<";"<< endl; 
    cout << "bgpar1["<< nbins <<"]="<<hhcorrbg[nbins]->GetFunction("expo")->GetParameter(1)<<";"<< endl; 
    for(int k=1; k<=hhcorrbg[nbins]->GetNbinsX(); k++) {
      if(hhcorrbg[nbins]->GetBinLowEdge(k)<statscale_lowlim || (hhcorrbg[nbins]->GetBinLowEdge(k)+hhcorrbg[nbins]->GetBinWidth(k))>statscale_uplim) {
        hhcorrbg_scaled[nbins]->SetBinContent(k,0.); 
        hhcorrbg_scaled[nbins]->SetBinError(k,0.);
      }
      else {
        double tmp = statscale * hhcorrbg[nbins]->GetFunction("expo")->Eval(hhcorrbg[nbins]->GetBinCenter(k));
        double tmprnd = myrandom->Poisson(tmp); 
        if(tmprnd<0.) { tmprnd=0.; }
        hhcorrbg_scaled[nbins]->SetBinContent(k,tmprnd); 
        hhcorrbg_scaled[nbins]->SetBinError(k,sqrt(tmprnd));
      }
    }
  hhcorrbg_scaled[nbins]->Fit("expo","rql","",statscale_lowlim,statscale_uplim);
  hhcorrbg[nbins]->SetDirectory(gROOT);
  hhcorrbg_scaled[nbins]->SetDirectory(gROOT);

f->Close();

// Correlated background in AuAu vs centrality
cout << "kuku1" << endl;
for(int i=0; i<nbins; i++) {

  sprintf(tmpname,"hhcorrbg_%d",i);
  hhcorrbg_scaled[i] = (TH1D*)hhcorrbg_scaled[nbins]->Clone(tmpname);

    for(int k=1; k<=hhcorrbg_scaled[nbins]->GetNbinsX(); k++) {
      if(hhcorrbg_scaled[nbins]->GetBinLowEdge(k)<statscale_lowlim || (hhcorrbg_scaled[nbins]->GetBinLowEdge(k)+hhcorrbg_scaled[nbins]->GetBinWidth(k))>statscale_uplim) {
        hhcorrbg_scaled[i]->SetBinContent(k,0.);
        hhcorrbg_scaled[i]->SetBinError(k,0.);
      }
      else {
        double tmp = (Ncoll[i]/Ncoll[0]) * hhcorrbg_scaled[nbins]->GetFunction("expo")->Eval(hhcorrbg_scaled[nbins]->GetBinCenter(k));
        double tmprnd = myrandom->Poisson(tmp);
        if(tmprnd<0.) { tmprnd=0.; }
        hhcorrbg_scaled[i]->SetBinContent(k,tmprnd);
        hhcorrbg_scaled[i]->SetBinError(k,sqrt(tmprnd));
      }
    }
  hhcorrbg_scaled[i]->Fit("expo","rql","",statscale_lowlim,statscale_uplim);

} // end loop over centrality bins


delete cdummy1;

TCanvas* c111 = new TCanvas("c111","Au+Au Correlated Background vs. Centrality",200,200,1200,600);
c111->Divide(4,2);
for(int i=0; i<nbins; i++) {
  c111->cd(i+1);
  hhcorrbg_scaled[i]->SetAxisRange(8.5,11.0); hhcorrbg_scaled[i]->SetMarkerStyle(1); hhcorrbg_scaled[i]->Draw("pe");
  sprintf(tlchar,"%d-%d",10*i,10*(i+1));   tl[i] = new TLatex(9.0,hhcorrbg_scaled[i]->GetMaximum()*0.9,tlchar); tl[i]->Draw();
}


//-----------------------------------------------------------------------------------------
//  Correlated bg in p+p
//-----------------------------------------------------------------------------------------

TH1D* hhbottom_pp;
TH1D* hhdy_pp;
TH1D* hhcorrbg_pp;
TH1D* hhall_pp;

double ppcorr = (2400./14.)/962.; // 2400B pp and 140B MB AuAu
TF1* fbottom_nosup_corr = new TF1("fbottom_nosup_corr","[0]+[1]*x",5.,14.);
fbottom_nosup_corr->SetParameters(-2.13861, 0.683323);

  sprintf(tmpname,"hhbottom_pp");
  hhbottom_pp = (TH1D*)hhbottom[nbins]->Clone(tmpname);
  for(int k=1; k<=hhbottom_pp->GetNbinsX(); k++) {
    if(hhbottom_pp->GetBinLowEdge(k)<statscale_lowlim || (hhbottom_pp->GetBinLowEdge(k)+hhbottom_pp->GetBinWidth(k))>statscale_uplim) {
      hhbottom_pp->SetBinContent(k,0.);
      hhbottom_pp->SetBinError(k,0.);
    }
    else {
      double tmp = ppcorr * fbottom_nosup_corr->Eval(hhbottom[nbins]->GetBinCenter(k)) * hhbottom[nbins]->GetFunction("expo")->Eval(hhbottom[nbins]->GetBinCenter(k));
      double tmprnd = myrandom->Poisson(tmp);
      if(tmprnd<0.) { tmprnd=0.; }
      hhbottom_pp->SetBinContent(k,tmprnd);
      hhbottom_pp->SetBinError(k,sqrt(tmprnd));
    }
  }

  sprintf(tmpname,"hhdy_pp");
  hhdy_pp = (TH1D*)hhdy[nbins]->Clone(tmpname);
  for(int k=1; k<=hhdy_pp->GetNbinsX(); k++) {
    if(hhdy_pp->GetBinLowEdge(k)<statscale_lowlim || (hhdy_pp->GetBinLowEdge(k)+hhdy_pp->GetBinWidth(k))>statscale_uplim) {
      hhdy_pp->SetBinContent(k,0.);
      hhdy_pp->SetBinError(k,0.);
    }
    else {
      double tmp = ppcorr * hhdy[nbins]->GetFunction("expo")->Eval(hhdy[nbins]->GetBinCenter(k));
      double tmprnd = myrandom->Poisson(tmp);
      if(tmprnd<0.) { tmprnd=0.; }
      hhdy_pp->SetBinContent(k,tmprnd);
      hhdy_pp->SetBinError(k,sqrt(tmprnd));
    }
  }

  sprintf(tmpname,"hhcorrbg_pp");
  hhcorrbg_pp = (TH1D*)hhbottom_pp->Clone(tmpname);
  hhcorrbg_pp->Add(hhdy_pp);
    hhcorrbg_pp->SetMarkerColor(kBlack);
    hhcorrbg_pp->SetLineColor(kBlack);
    hhbottom_pp->SetLineColor(kBlue);
    hhdy_pp->SetLineColor(kGreen+2);
  sprintf(tmpname,"hhall_pp");
  hhall_pp = (TH1D*)hhcorrbg_pp->Clone(tmpname);
  hhall_pp->Add(hhupspp);
    hhall_pp->SetLineColor(kMagenta);
    hhall_pp->SetMarkerColor(kMagenta);


TCanvas* cbginpp = new TCanvas("cbginpp","corr bg in pp",10,10,700,700);

  hhcorrbg_pp->Fit("expo","rql","",statscale_lowlim,statscale_uplim);
  hhcorrbg_pp->GetFunction("expo")->SetLineColor(kBlack);
  hhbottom_pp->Fit("expo","rql","",statscale_lowlim,statscale_uplim);
  hhbottom_pp->GetFunction("expo")->SetLineColor(kBlue);
  hhdy_pp->Fit("expo","rql","",statscale_lowlim,statscale_uplim);
  hhdy_pp->GetFunction("expo")->SetLineColor(kGreen+2);

  hhall_pp->SetAxisRange(7.,12.);
  hhcorrbg_pp->Draw("pehist");
  hhbottom_pp->Draw("histsame");
  hhdy_pp->Draw("histsame");


TCanvas* cpp = new TCanvas("cpp","corr bg + sig in pp",100,100,700,700);
  hhall_pp->SetAxisRange(7.,12.);
  hhall_pp->Draw("pehist");
  hhcorrbg_pp->Draw("pesame");
  hhbottom_pp->Draw("same");
  hhdy_pp->Draw("same");

//-----------------------------------------------------------------------------------------
// Combinatorial BG calculated for 10B central AuAu events
//-----------------------------------------------------------------------------------------

TCanvas* cdummy = new TCanvas("cdummy","cdummy",0,0,500,500);

f = new TFile("fakee_eideff09.root");
  sprintf(tmpname,"hhfakefake_15"); // 15 is integrated over pT
  hhfakefake[nbins] = (TH1D*)f->Get(tmpname);
  hhfakefake[nbins]->SetDirectory(gROOT);
f->Close();

f = new TFile("crossterms_eideff09.root");
  sprintf(tmpname,"hhfakehf_15");
  hhfakehf[nbins] = (TH1D*)f->Get(tmpname);
  hhfakehf[nbins]->SetDirectory(gROOT);
f->Close();

TF1* fbg = new TF1("fbg","exp([0]+[1]*x)+exp([2]+[3]*x)",8.,11.);
fbg->SetParameters(10., -1.0, 4., -0.1);
fbg->SetParLimits(1.,-999.,0.);
fbg->SetParLimits(3.,-999.,0.);

  sprintf(tmpname,"hhcombbg_15");
  hhcombbg[nbins] = (TH1D*)hhfakefake[nbins]->Clone(tmpname);
  hhcombbg[nbins]->Add(hhfakehf[nbins]);
  sprintf(tmpname,"hhcombbg_scaled_15");
  hhcombbg_scaled[nbins] = (TH1D*)hhcombbg[nbins]->Clone(tmpname);
  fbg->SetParameters(10., -1.0, 4., -0.1); 
  hhcombbg[nbins]->Fit(fbg,"qrl","",statscale_lowlim,statscale_uplim);

    for(int k=1; k<=hhcombbg[nbins]->GetNbinsX(); k++) {
      if(hhcombbg[nbins]->GetBinLowEdge(k)<statscale_lowlim || (hhcombbg[nbins]->GetBinLowEdge(k)+hhcombbg[nbins]->GetBinWidth(k))>statscale_uplim) {
        hhcombbg_scaled[nbins]->SetBinContent(k,0.);
        hhcombbg_scaled[nbins]->SetBinError(k,0.);
      }
      else {
        double tmp = statscale * hhcombbg[nbins]->GetFunction("fbg")->Eval(hhcombbg[nbins]->GetBinCenter(k));
        double tmprnd = myrandom->Poisson(tmp);
        if(tmprnd<0.) { tmprnd=0.; }
        hhcombbg_scaled[nbins]->SetBinContent(k,tmprnd);
        hhcombbg_scaled[nbins]->SetBinError(k,sqrt(tmprnd));
      }
    }
  hhcombbg_scaled[nbins]->Fit(fbg,"qrl","",statscale_lowlim,statscale_uplim);

delete cdummy;

TCanvas* C1 = new TCanvas("C1","Combinatorial BG Central Au+Au",100,100,600,600);
C1->SetLogy();
  hhfakefake[nbins]->SetAxisRange(7.0,14.0);
  hhfakefake[nbins]->SetMinimum(0.1);
  hhfakefake[nbins]->SetMaximum(5000.);
  hhfakefake[nbins]->SetLineColor(kGreen+2);
  hhfakefake[nbins]->SetLineWidth(2);
  hhfakefake[nbins]->GetXaxis()->SetTitle("Transverse momentum [GeV/c]");
  hhfakefake[nbins]->GetXaxis()->SetTitleOffset(1.0);
  hhfakefake[nbins]->GetXaxis()->SetTitleColor(1);
  hhfakefake[nbins]->GetXaxis()->SetTitleSize(0.040);
  hhfakefake[nbins]->GetXaxis()->SetLabelSize(0.040);
  hhfakefake[nbins]->GetYaxis()->SetTitle("Combinatorial background");
  hhfakefake[nbins]->GetYaxis()->SetTitleOffset(1.3);
  hhfakefake[nbins]->GetYaxis()->SetTitleSize(0.040);
  hhfakefake[nbins]->GetYaxis()->SetLabelSize(0.040);
  hhfakefake[nbins]->Draw("e");

  hhfakehf[nbins]->SetLineColor(kOrange+4);
  hhfakehf[nbins]->SetLineWidth(2);
  hhfakehf[nbins]->Draw("esame");

  hhcombbg[nbins]->SetLineColor(kBlack);
  hhcombbg[nbins]->SetLineWidth(2);
  hhcombbg[nbins]->Draw("esame");

TCanvas* C1sc = new TCanvas("C1sc","SCALED Combinatorial BG Central Au+Au",100,100,600,600);
C1sc->SetLogy(); 
  hhcombbg_scaled[nbins]->SetAxisRange(7.,14.);
  hhcombbg_scaled[nbins]->Draw("esame");

// Scaled Combinatorial background vs centrality

for(int i=0; i<nbins; i++) {

  sprintf(tmpname,"hhcombbg_%d",i);
  hhcombbg_scaled[i] = (TH1D*)hhcombbg_scaled[nbins]->Clone(tmpname);

    for(int k=1; k<=hhcombbg_scaled[nbins]->GetNbinsX(); k++) {
      if(hhcombbg_scaled[nbins]->GetBinLowEdge(k)<statscale_lowlim || (hhcombbg_scaled[nbins]->GetBinLowEdge(k)+hhcombbg_scaled[nbins]->GetBinWidth(k))>statscale_uplim) {
        hhcombbg_scaled[i]->SetBinContent(k,0.);
        hhcombbg_scaled[i]->SetBinError(k,0.);
      }
      else {
        double tmp = Npionpairs[i] * hhcombbg_scaled[nbins]->GetFunction("fbg")->Eval(hhcombbg_scaled[nbins]->GetBinCenter(k));
        double tmprnd = myrandom->Poisson(tmp);
        if(tmprnd<0.) { tmprnd=0.; }
        hhcombbg_scaled[i]->SetBinContent(k,tmprnd);
        hhcombbg_scaled[i]->SetBinError(k,sqrt(tmprnd));
      }
    }
  hhcombbg_scaled[i]->Fit(fbg,"qrl","",statscale_lowlim,statscale_uplim);

} // end over centrality bins

TCanvas* c_comb_scaled = new TCanvas("c_comb_scaled","Combinatorial Background vs. Centrality",200,100,1200,600);
c_comb_scaled->Divide(4,2);
for(int i=0; i<nbins; i++) {
  c_comb_scaled->cd(i+1);
  hhcombbg_scaled[i]->SetAxisRange(8.5,11.0); hhcombbg_scaled[i]->SetMarkerStyle(1); hhcombbg_scaled[i]->Draw("pe");
  sprintf(tlchar,"%d-%d",10*i,10*(i+1));   tl[i] = new TLatex(9.0,hhcombbg_scaled[i]->GetMaximum()*0.9,tlchar); tl[i]->Draw();
}


//---------------------------------------------
//  Au+Au Signal + all BG
//---------------------------------------------

for(int i=0; i<nbins; i++) {
  sprintf(tmpname,"hhtotbg_scaled_%d",i);
  hhtotbg_scaled[i] = (TH1D*)hhcombbg_scaled[i]->Clone(tmpname);
  hhtotbg_scaled[i]->Add(hhcorrbg_scaled[i]);
}

for(int i=0; i<nbins; i++) {
  sprintf(tmpname,"hhall_scaled_%d",i);
  hhall_scaled[i] = (TH1D*)hhtotbg_scaled[i]->Clone(tmpname);
  hhall_scaled[i]->Add(hhups[i]);
}

TCanvas* c000 = new TCanvas("c000","Au+Au Signal + All Background vs. Centrality",200,200,1200,600);
c000->Divide(4,2);
for(int i=0; i<nbins; i++) {
  c000->cd(i+1);
  hhall_scaled[i]->SetAxisRange(8.5,11.0); hhall_scaled[i]->SetMarkerStyle(1); hhall_scaled[i]->Draw("pehist");
  sprintf(tlchar,"%d-%d",10*i,10*(i+1));   tl[i] = new TLatex(9.0,hhall_scaled[i]->GetMaximum()*0.9,tlchar); tl[i]->Draw();
}

/*
// Fit Signal + Correlated BG

// hhfit[] is signal + correlated bg
for(int i=0; i<nbins; i++) {
  sprintf(tmpname,"hhfit_%d",i);
  hhfit[i] = (TH1D*)hhall_scaled[i]->Clone(tmpname);
  for(int j=1; j<=hhall_scaled[i]->GetNbinsX(); j++) {
    hhfit[i]->SetBinContent(j,hhfit[i]->GetBinContent(j) - hhcombbg_scaled[i]->GetFunction("fbg")->Eval(hhcombbg_scaled[i]->GetBinCenter(j)));
    hhfit[i]->SetBinError(j,sqrt(pow(hhfit[i]->GetBinError(j),2)+hhcombbg_scaled[i]->GetFunction("fbg")->Eval(hhcombbg_scaled[i]->GetBinCenter(j))));
  }
}

//---------------------------------------------------------------------
// plot signal + correlated bg for all pT
//---------------------------------------------------------------------

  double tmppar0 = corrbgfitpar0+TMath::Log(statscale);
  double tmppar1 = corrbgfitpar1;
cout << "###### " << tmppar0 << " " << tmppar1 << endl;

double ppauauscale = fTCBauau->GetParameter(0)/fTCBpp->GetParameter(0)*0.9783;
fSandBpp->SetParameter(0,fTCBpp->GetParameter(0)*ppauauscale);
fSandBpp->SetParameter(1,fTCBpp->GetParameter(1));
fSandBpp->SetParameter(2,fTCBpp->GetParameter(2));
fSandBpp->SetParameter(3,fTCBpp->GetParameter(3));
fSandBpp->SetParameter(4,fTCBpp->GetParameter(4));
fSandBpp->SetParameter(5,fTCBpp->GetParameter(5)*ppauauscale);
fSandBpp->SetParameter(6,fTCBpp->GetParameter(6)*ppauauscale);
fSandBpp->SetParameter(7,tmppar0);
fSandBpp->SetParameter(8,tmppar1);
fSandBpp->SetLineColor(kBlue);
fSandBpp->SetLineStyle(2);

fSandBauau->SetParameter(0,fTCBauau->GetParameter(0));
fSandBauau->SetParameter(1,fTCBauau->GetParameter(1));
fSandBauau->SetParameter(2,fTCBauau->GetParameter(2));
fSandBauau->SetParameter(3,fTCBauau->GetParameter(3));
fSandBauau->SetParameter(4,fTCBauau->GetParameter(4));
fSandBauau->SetParameter(5,fTCBauau->GetParameter(5));
fSandBauau->SetParameter(6,fTCBauau->GetParameter(6));
fSandBauau->SetParameter(7,tmppar0);
fSandBauau->SetParameter(8,tmppar1);
fSandBauau->SetLineColor(kRed);

TCanvas* cfitall = new TCanvas("cfitall","Fit Central",270,270,600,600);
  hhfit[0]->SetAxisRange(7.0,14.);
  hhfit[0]->GetXaxis()->CenterTitle();
  hhfit[0]->GetXaxis()->SetTitle("Mass(e^{+}e^{-}) [GeV/c^2]");
  hhfit[0]->GetXaxis()->SetTitleOffset(1.1);
  hhfit[0]->GetXaxis()->SetLabelSize(0.045);
  hhfit[0]->GetYaxis()->CenterTitle();
  hhfit[0]->GetYaxis()->SetLabelSize(0.045);
  hhfit[0]->GetYaxis()->SetTitle("Events / (50 MeV/c^{2})");
  hhfit[0]->GetYaxis()->SetTitleOffset(1.5);
  hhfit[0]->Draw("pehist");
//  fSandBpp->Draw("same");
//  fSandBauau->Draw("same");
    //hhcorrbg_scaled[0]->SetLineColor(kRed);
    //hhcorrbg_scaled[0]->Scale(0.82);
    //hhcorrbg_scaled[0]->Scale(0.70);
    //hhcorrbg_scaled[0]->Draw("histsame");

  TF1* fmycorrbg = new TF1("fmycorrbg","exp([0]+[1]*x)",7.,14.);
  fmycorrbg->SetParameters(tmppar0,tmppar1);
  fmycorrbg->SetLineStyle(2);
  fmycorrbg->SetLineColor(kRed);
  fmycorrbg->Draw("same");

//double myheight = fTCBauau->GetParameter(0);
//TLatex* ld1 = new TLatex(10.1,myheight,"sPHENIX Simulation");
//ld1->SetTextSize(0.035);
//ld1->Draw();
//TLatex* ld2 = new TLatex(10.1,myheight-100.,"0-10% Au+Au #sqrt{s} = 200 GeV");
//ld2->SetTextSize(0.035);
//ld2->Draw();

TCanvas* cfitall2 = new TCanvas("cfitall2","FIT all pT",270,270,600,600);
TH1D* hhfit_tmp = (TH1D*)hhfit[0]->Clone("hhfit_tmp");
hhfit_tmp->SetAxisRange(8.0,11.);
hhfit_tmp->Draw("pehist");
//fSandBauau->Draw("same");
fmycorrbg->Draw("same");
*/

//----------------------------------------------------------------------

// plot signal + both backgrounds for central Au+Au

TCanvas* callpt = new TCanvas("callpt","Signal + All BG Central Au+Au",300,300,600,600);

  hhall_scaled[0]->GetXaxis()->SetTitle("Invariant mass GeV/c");
  hhall_scaled[0]->SetLineColor(kBlack);
  hhall_scaled[0]->SetMarkerColor(kBlack);
  hhall_scaled[0]->SetMarkerStyle(20);
  hhall_scaled[0]->SetAxisRange(8.0,10.8);
  hhall_scaled[0]->Draw("pehist");
    hhcombbg_scaled[0]->SetLineColor(kBlue);
    hhcombbg_scaled[0]->Draw("histsame");
      hhcorrbg_scaled[0]->SetLineColor(kRed);
      hhcorrbg_scaled[0]->Draw("histsame");



//----------------------------------------------------------------
//  Calculate RAA
//----------------------------------------------------------------

double u1start = 9.25;
double u1stop  = 9.65;
double u2start = 9.80;
double u2stop  = 10.20;
double u3start = 10.20;
double u3stop  = 10.55;
cout << "kuku2" << endl;
  double raa1[nbins+1],raa2[nbins+1],raa3[nbins+1],erraa1[nbins+1],erraa2[nbins+1],erraa3[nbins+1];
  for(int i=0; i<nbins; i++) { 
    //raa1[i] = raa1s[i];
    //raa2[i] = raa2s[i];
    //raa3[i] = raa3s[i];
    raa1[i] = grRAA1S->Eval(Npart[i]);
    raa2[i] = grRAA2S->Eval(Npart[i]);
    if(i<5) { raa3[i] = grRAA2S->Eval(Npart[i])/2.; }
      else { raa3[i] = (grRAA2S->Eval(Npart[i])+grRAA3S->Eval(Npart[i]))/2.; }
  }
  int fbin1 = hhall_scaled[0]->FindBin(u1start + 0.001);
  int lbin1 = hhall_scaled[0]->FindBin(u1stop  - 0.001);
  int fbin2 = hhall_scaled[0]->FindBin(u2start + 0.001);
  int lbin2 = hhall_scaled[0]->FindBin(u2stop  - 0.001);
  int fbin3 = hhall_scaled[0]->FindBin(u3start + 0.001);
  int lbin3 = hhall_scaled[0]->FindBin(u3stop  - 0.001);
  cout << "Y(1S) bin range: " << fbin1 << " - " << lbin1 << endl;
  cout << "Y(1S) inv. mass range: " << u1start << " - " << u1stop << endl;
  cout << "Y(2S) bin range: " << fbin2 << " - " << lbin2 << endl;
  cout << "Y(2S) inv. mass range: " << u2start << " - " << u2stop << endl;
  cout << "Y(3S) bin range: " << fbin3 << " - " << lbin3 << endl;
  cout << "Y(3S) inv. mass range: " << u3start << " - " << u3stop << endl;

  double sum1[99]   = {0.};
  double truesum1[99]   = {0.};
  double ersum1[99] = {0.};
  double sumpp1   = 0.;
  double ersumpp1 = 0.;
  double sum2[99]   = {0.};
  double truesum2[99]   = {0.};
  double ersum2[99] = {0.};
  double sumpp2   = 0.;
  double ersumpp2 = 0.;
  double sum3[99]   = {0.};
  double truesum3[99]   = {0.};
  double ersum3[99] = {0.};
  double sumpp3   = 0.;
  double ersumpp3 = 0.;

  double sumsum1[99]   = {0.};
  double sumsum2[99]   = {0.};
  double sumsum3[99]   = {0.};
  double sumsum1pp   = 0.;
  double sumsum2pp   = 0.;
  double sumsum3pp   = 0.;

  for(int j=fbin1; j<=lbin1; j++) {
    sumpp1   += hhups1pp->GetBinContent(j);
    ersumpp1 += hhupspp->GetBinError(j)*hhupspp->GetBinError(j);
  }
  for(int j=fbin2; j<=lbin2; j++) {
    sumpp2   += hhups2pp->GetBinContent(j);
    ersumpp2 += hhupspp->GetBinError(j)*hhupspp->GetBinError(j);
  }
  for(int j=fbin3; j<=lbin3; j++) {
    sumpp3   += hhups3pp->GetBinContent(j);
    ersumpp3 += hhupspp->GetBinError(j)*hhupspp->GetBinError(j);
  }

  for(int i=0; i<nbins; i++) {

    //double s1 = double(i); double s2 = double(i+1); if(i==nbins) {s1 = 0.;}

    for(int j=fbin1; j<=lbin1; j++) {
      sum1[i]   += (hhall_scaled[i]->GetBinContent(j) - hhcombbg_scaled[i]->GetFunction("fbg")->Eval(hhall_scaled[i]->GetBinCenter(j)) - hhcorrbg_scaled[i]->GetFunction("expo")->Eval(hhall_scaled[i]->GetBinCenter(j)));
      truesum1[i] += hhups1[i]->GetBinContent(j);
      ersum1[i] += hhall_scaled[i]->GetBinError(j)*hhall_scaled[i]->GetBinError(j);
    }
      sumsum1[i]     = truesum1[i];                   // direct count in mass range
      sumsum1pp   = sumpp1;       
      //sumsum1[i]     = hhups1[i]->GetEntries();       // total number of upsilons in pT bin (rounded up)
      //sumsum1pp   = hhups1pp->GetEntries();
      //sumsum1[i]     = Nups1*fUpsilonPt->Integral(s1,s2)/upsnorm;  // total number of upsilons in pT bin
      //sumsum1pp   = Nups1pp*fUpsilonPt->Integral(s1,s2)/upsnorm;

        if(sumsum1[i]>0. && sumsum1pp>0.) {
          erraa1[i] = raa1[i]*sqrt(ersum1[i]/sumsum1[i]/sumsum1[i] + ersumpp1/sumsum1pp/sumsum1pp);
        } else {raa1[i]=-1.0; erraa1[i] = 999.; }

    for(int j=fbin2; j<=lbin2; j++) {
      sum2[i]   += (hhall_scaled[i]->GetBinContent(j) - hhcombbg_scaled[i]->GetFunction("fbg")->Eval(hhall_scaled[i]->GetBinCenter(j)) - hhcorrbg_scaled[i]->GetFunction("expo")->Eval(hhall_scaled[i]->GetBinCenter(j)));
      truesum2[i] += hhups2[i]->GetBinContent(j);
      ersum2[i] += hhall_scaled[i]->GetBinError(j)*hhall_scaled[i]->GetBinError(j);
    }
      sumsum2[i]     = truesum2[i];                   // direct count in mass range
      sumsum2pp   = sumpp2;       
      //sumsum2[i]     = hhups2[i]->GetEntries();       // total number of upsilons in pT bin (rounded up)
      //sumsum2pp   = hhups2pp->GetEntries();
      //sumsum2[i]     = Nups2*fUpsilonPt->Integral(s1,s2)/upsnorm;  // total number of upsilons in pT bin
      //sumsum2pp   = Nups2pp*fUpsilonPt->Integral(s1,s2)/upsnorm;
      
        if(sumsum2[i]>0. && sumsum2pp>0.) {
          erraa2[i] = raa2[i]*sqrt(ersum2[i]/sumsum2[i]/sumsum2[i] + ersumpp2/sumsum2pp/sumsum2pp);
        } else {raa2[i]=-1.0; erraa2[i] = 999.; }

    for(int j=fbin3; j<=lbin3; j++) {
      sum3[i]   += (hhall_scaled[i]->GetBinContent(j) - hhcombbg_scaled[i]->GetFunction("fbg")->Eval(hhall_scaled[i]->GetBinCenter(j)) - hhcorrbg_scaled[i]->GetFunction("expo")->Eval(hhall_scaled[i]->GetBinCenter(j)));
      truesum3[i] += hhups3[i]->GetBinContent(j);
      ersum3[i] += hhall_scaled[i]->GetBinError(j)*hhall_scaled[i]->GetBinError(j);
    }
      sumsum3[i]     = truesum3[i];                   // direct count in mass range
      sumsum3pp   = sumpp3;       
      //sumsum3[i]     = hhups3[i]->GetEntries();       // total number of upsilons in pT bin (rounded up)
      //sumsum3pp   = hhups3pp->GetEntries();
      //sumsum3[i]     = Nups3*fUpsilonPt->Integral(s1,s2)/upsnorm;   // total number of upsilons in pT bin
      //sumsum3pp   = Nups3pp*fUpsilonPt->Integral(s1,s2)/upsnorm;
      
        if(truesum3[i]>0. && sumpp3>0.) {
          erraa3[i] = raa3[i]*sqrt(ersum3[i]/sumsum3[i]/sumsum3[i] + ersumpp3/sumsum3pp/sumsum3pp);
        } else {raa3[i]=-1.0; erraa3[i] = 999.; }
  
  } // end loop over centrality

  erraa3[3] = erraa3[3]*1.2;

  for(int i=0; i<nbins; i++) {
    cout << "Npart, Raa = " << Npart[i] << " " << raa1[i] << " " << raa2[i] << " " << raa3[i] << endl;
  }

cout << "====== Y(1S):" << endl;
  for(int i=0; i<nbins; i++) {
    cout << "   " << i << " " << sumsum1[i] << "(" << Nups1[i] << ")" << " +- " << sqrt(ersum1[i]) 
         << " \t\t pp: " << sumsum1pp << " +- " << sqrt(ersumpp1) << endl;
  }
cout << "====== Y(2S):" << endl;
  for(int i=0; i<nbins; i++) {
    cout << "   " << i << " " << sumsum2[i] << "(" << Nups2[i] << ")" << " +- " << sqrt(ersum2[i]) 
         << " \t\t pp: " << sumsum2pp << " +- " << sqrt(ersumpp2) << endl;
  }
cout << "====== Y(3S):" << endl;
  for(int i=0; i<nbins; i++) {
    cout << "   " << i << " " << sumsum3[i] << "(" << Nups3[i] << ")" << " +- " << sqrt(ersum3[i]) 
         << " \t\t pp: " << sumsum3pp << " +- " << sqrt(ersumpp3) << endl;
  }


/*
double raa2_rebin[9],raapt_rebin2[9],erraa2_rebin[9];
double raa3_rebin[9],raapt_rebin3[9],erraa3_rebin[9];
double sum2_rebin[9],ersum2_rebin[9],sum2pp_rebin[9],ersumpp2_rebin[9];
double sum3_rebin[9],ersum3_rebin[9],sum3pp_rebin[9],ersumpp3_rebin[9];

// Rebin Y(2S) by 2
raapt_rebin2[0] = 1.;
raapt_rebin2[1] = 3.;
raapt_rebin2[2] = 5.;
raapt_rebin2[3] = 7.;
raa2_rebin[0] = grRAA2S->Eval(raapt_rebin2[0]);
raa2_rebin[1] = grRAA2S->Eval(raapt_rebin2[1]);
raa2_rebin[2] = grRAA2S->Eval(raapt_rebin2[2]);
raa2_rebin[3] = grRAA2S->Eval(raapt_rebin2[3]);
sum2_rebin[0] = truesum2[0]+truesum2[1];
sum2_rebin[1] = truesum2[2]+truesum2[3];
sum2_rebin[2] = truesum2[4]+truesum2[5];
sum2_rebin[3] = truesum2[6]+truesum2[7]+truesum2[8]+truesum2[9];
ersum2_rebin[0] = ersum2[0]+ersum2[1];
ersum2_rebin[1] = ersum2[2]+ersum2[3];
ersum2_rebin[2] = ersum2[4]+ersum2[5];
ersum2_rebin[3] = ersum2[6]+ersum2[7]+ersum2[8]+ersum2[9];
sum2pp_rebin[0] = sumpp2[0]+sumpp2[1];
sum2pp_rebin[1] = sumpp2[2]+sumpp2[3];
sum2pp_rebin[2] = sumpp2[4]+sumpp2[5];
sum2pp_rebin[3] = sumpp2[6]+sumpp2[7]+sumpp2[8]+sumpp2[9];
ersumpp2_rebin[0] = ersumpp2[0]+ersumpp2[1];
ersumpp2_rebin[1] = ersumpp2[2]+ersumpp2[3];
ersumpp2_rebin[2] = ersumpp2[4]+ersumpp2[5];
ersumpp2_rebin[3] = ersumpp2[6]+ersumpp2[7]+ersumpp2[8]+ersumpp2[9];
  erraa2_rebin[0] = raa2[0]*sqrt(ersum2_rebin[0]/sum2_rebin[0]/sum2_rebin[0] + ersumpp2_rebin[0]/sum2pp_rebin[0]/sum2pp_rebin[0]);
  erraa2_rebin[1] = raa2[1]*sqrt(ersum2_rebin[1]/sum2_rebin[1]/sum2_rebin[1] + ersumpp2_rebin[1]/sum2pp_rebin[1]/sum2pp_rebin[1]);
  erraa2_rebin[2] = raa2[2]*sqrt(ersum2_rebin[2]/sum2_rebin[2]/sum2_rebin[2] + ersumpp2_rebin[2]/sum2pp_rebin[2]/sum2pp_rebin[2]);
  erraa2_rebin[3] = raa2[3]*sqrt(ersum2_rebin[3]/sum2_rebin[3]/sum2_rebin[3] + ersumpp2_rebin[3]/sum2pp_rebin[3]/sum2pp_rebin[3]);
// Rebin Y(3S) by 4
raapt_rebin3[0] = 2.;
raapt_rebin3[1] = 6.;
raa3_rebin[0] = grRAA3S->Eval(raapt_rebin3[0]);
raa3_rebin[1] = grRAA3S->Eval(raapt_rebin3[1]);
sum3_rebin[0] = truesum3[0]+truesum3[1]+truesum3[2]+truesum3[3];
sum3_rebin[1] = truesum3[4]+truesum3[5]+truesum3[6]+truesum3[7]+truesum3[8]+truesum3[9];
ersum3_rebin[0] = ersum3[0]+ersum3[1]+ersum3[2]+ersum3[3];
ersum3_rebin[1] = ersum3[4]+ersum3[5]+ersum3[6]+ersum3[7]+ersum3[8]+ersum3[9];
sum3pp_rebin[0] = sumpp3[0]+sumpp3[1]+sumpp3[3]+sumpp3[3];
sum3pp_rebin[1] = sumpp3[4]+sumpp3[5]+sumpp3[6]+sumpp3[7]+sumpp3[8]+sumpp3[9];
ersumpp3_rebin[0] = ersumpp3[0]+ersumpp3[1]+ersumpp3[2]+ersumpp3[3];
ersumpp3_rebin[1] = ersumpp3[4]+ersumpp3[5]+ersumpp3[6]+ersumpp3[7]+ersumpp3[8]+ersumpp3[9];
  erraa3_rebin[0] = raa3[0]*sqrt(ersum3_rebin[0]/sum3_rebin[0]/sum3_rebin[0] + ersumpp3_rebin[0]/sum3pp_rebin[0]/sum3pp_rebin[0]);
  erraa3_rebin[1] = raa3[1]*sqrt(ersum3_rebin[1]/sum3_rebin[1]/sum3_rebin[1] + ersumpp3_rebin[1]/sum3pp_rebin[1]/sum3pp_rebin[1]);
// Rebin Y(3S) by 8
*/
/*
raapt_rebin3[0] = 5.;
raa3_rebin[0] = grRAA3S->Eval(raapt_rebin3[0]);
sum3_rebin[0] = truesum3[0]+truesum3[1]+truesum3[2]+truesum3[3]+
                truesum3[4]+truesum3[5]+truesum3[6]+truesum3[7]+truesum3[8]+truesum3[9];
ersum3_rebin[0] = ersum3[0]+ersum3[1]+ersum3[2]+ersum3[3]+
                  ersum3[4]+ersum3[5]+ersum3[6]+ersum3[7]+ersum3[8]+ersum3[9];
sum3pp_rebin[0] = sumpp3[0]+sumpp3[1]+sumpp3[3]+sumpp3[3]+
                  sumpp3[4]+sumpp3[5]+sumpp3[6]+sumpp3[7]+sumpp3[8]+sumpp3[9];
ersumpp3_rebin[0] = ersumpp3[0]+ersumpp3[1]+ersumpp3[2]+ersumpp3[3]+
                    ersumpp3[4]+ersumpp3[5]+ersumpp3[6]+ersumpp3[7]+ersumpp3[8]+ersumpp3[9];
erraa3_rebin[0] = raa3[0]*sqrt(ersum3_rebin[0]/sum3_rebin[0]/sum3_rebin[0] + ersumpp3_rebin[0]/sum3pp_rebin[0]/sum3pp_rebin[0]);
*/

//-------------------------------------------------
//  Plot RAA
//-------------------------------------------------

int npts1 = nbins;
int npts2 = nbins;
int npts3 = nbins;
int npts2_rebin = 4;
int npts3_rebin = 2;

TCanvas* craa = new TCanvas("craa","R_{AA}",120,120,800,600);
TH2F* hh2 = new TH2F("hh2"," ",10,0.,400.,10,0.,1.1);
hh2->GetXaxis()->SetTitle("N_{part}");
hh2->GetXaxis()->SetTitleOffset(0.9);
hh2->GetXaxis()->SetTitleColor(1);
hh2->GetXaxis()->SetTitleSize(0.050);
hh2->GetXaxis()->SetLabelSize(0.040);
hh2->GetYaxis()->SetTitle("R_{AA}");
hh2->GetYaxis()->SetTitleOffset(0.7);
hh2->GetYaxis()->SetTitleSize(0.050);
hh2->GetYaxis()->SetLabelSize(0.040);
hh2->Draw();

double xx1[nbins+1]; for(int i=0; i<nbins; i++) {xx1[i] = Npart[i];}
double xx2[nbins+1]; for(int i=0; i<nbins; i++) {xx2[i] = Npart[i] - 1.;}
double xx3[nbins+1]; for(int i=0; i<nbins; i++) {xx3[i] = Npart[i] + 1.;}
//double xx3_rebin[nbins+1]; for(int i=0; i<npts3_rebin; i++) {xx3_rebin[i] = raapt_rebin3[i];}

TGraphErrors* gr1 = new TGraphErrors(npts1,xx1,raa1,0,erraa1);
gr1->SetMarkerStyle(20);
gr1->SetMarkerColor(kBlack);
gr1->SetLineColor(kBlack);
gr1->SetLineWidth(2);
gr1->SetMarkerSize(1.5);
gr1->SetName("gr1");
gr1->Draw("p");

TGraphErrors* gr2 = new TGraphErrors(npts2,xx2,raa2,0,erraa2);
gr2->SetMarkerStyle(20);
gr2->SetMarkerColor(kRed);
gr2->SetLineColor(kRed);
gr2->SetLineWidth(2);
gr2->SetMarkerSize(1.5);
gr2->SetName("gr2");
gr2->Draw("p");

//TGraphErrors* gr2_rebin = new TGraphErrors(npts2_rebin,xx2_rebin,raa2_rebin,0,erraa2_rebin);
//gr2_rebin->SetMarkerStyle(20);
//gr2_rebin->SetMarkerColor(kRed);
//gr2_rebin->SetLineColor(kRed);
//gr2_rebin->SetLineWidth(2);
//gr2_rebin->SetMarkerSize(1.5);
//gr2_rebin->SetName("gr2");
//gr2_rebin->Draw("p");

//--- 3S state -------------------
// double dummy[9]; for(int i=0; i<9; i++) {dummy[i]=-99.;}
 TGraphErrors* gr3 = new TGraphErrors(nbins,xx3,raa3,0,erraa3);
 gr3->SetMarkerStyle(20);
 gr3->SetMarkerColor(kBlue);
 gr3->SetLineColor(kBlue);
 gr3->SetLineWidth(2);
 gr3->SetMarkerSize(1.5);
 gr3->SetName("gr3");
// gr3->Draw("p");
/*
 TGraphErrors* gr3_rebin = new TGraphErrors(npts3_rebin,xx3_rebin,raa3_rebin,0,erraa3_rebin);
 gr3_rebin->SetMarkerStyle(20);
 gr3_rebin->SetMarkerColor(kBlue);
 gr3_rebin->SetLineColor(kBlue);
 gr3_rebin->SetLineWidth(2);
 gr3_rebin->SetMarkerSize(1.5);
 gr3_rebin->SetName("gr3");
 gr3_rebin->Draw("p");
*/
/*
 TGraphErrors* gr3_rebin = new TGraphErrors(1,xx3_rebin,raa3_rebin,0,erraa3_rebin);
 gr3_rebin->SetMarkerStyle(20);
 gr3_rebin->SetMarkerColor(kBlue);
 gr3_rebin->SetLineColor(kBlue);
 gr3_rebin->SetLineWidth(2);
 gr3_rebin->SetMarkerSize(1.5);
 gr3_rebin->SetName("gr3");
 gr3_rebin->Draw("p");
*/
//TArrow* aa[9];
//TLine* ll[9];
//erraa3[3] = erraa3[3]*1.5;
//erraa3[4] = erraa3[4]*0.6;
//erraa3[4] = erraa3[4]*1.5;
//erraa3[6] = erraa3[6]*1.2;
/*
for(int i=0; i<npts3; i++) {
  aa[i] = new TArrow(xx3[i],1.64*erraa3[i],xx3[i],-0.02,0.02);
  aa[i]->SetLineColor(kBlue);
  aa[i]->SetLineWidth(2);
  aa[i]->Draw();
  ll[i] = new TLine(xx3[i]-0.15,1.64*erraa3[i],xx3[i]+0.15,1.64*erraa3[i]);
  ll[i]->SetLineColor(kBlue);
  ll[i]->SetLineWidth(2);
  ll[i]->Draw();
}
*/
//--- end 3S state --------------

//TLegend *leg = new TLegend(0.70,0.70,0.88,0.88);
TLegend *leg = new TLegend(0.73,0.76,0.89,0.88);
leg->SetBorderSize(0);
leg->SetFillColor(10);
leg->SetFillStyle(1001);
TLegendEntry *entry1=leg->AddEntry("gr1","Y(1S)","p");
TLegendEntry *entry2=leg->AddEntry("gr2","Y(2S)","p");
//TLegendEntry *entry3=leg->AddEntry("gr3","Y(3S)","l");
//TLegendEntry *entry3=leg->AddEntry("gr3","Y(3S)","p");
leg->Draw();

TLatex* l1 = new TLatex(155.,1.02,"#font[72]{sPHENIX} Projection"); l1->SetTextFont(42); l1->Draw();
//TLatex* l11 = new TLatex(20.,0.90,"0-10% cent. Au+Au, Years 1-3"); l11->SetTextFont(42); l11->Draw();
TLatex* l2 = new TLatex(155.,0.93,"21 nb^{-1} rec. Au+Au"); l2->SetTextFont(42); l2->Draw();
TLatex* l3 = new TLatex(155.,0.84,"62 pb^{-1} samp. #it{p+p}"); l3->SetTextFont(42); l3->Draw();

TLine* lll = new TLine(0.6,0.64,1.3,0.64);
lll->SetLineColor(kBlue);
lll->SetLineWidth(2);
//lll->Draw();

grRAA1S->Draw("l");
grRAA1S_eta1->Draw("l");
grRAA1S_eta3->Draw("l");
grRAA2S->Draw("l");
grRAA2S_eta1->Draw("l");
grRAA2S_eta3->Draw("l");
//grRAA3S->Draw("l");
//grRAA3S_eta1->Draw("l");
//grRAA3S_eta3->Draw("l");

//==================================================================================

/*
fout = new TFile("test.root","recreate");
for(int i=0; i<nbins+1; i++) {
  sprintf(tmpname,"hhfit_%d",i);
  hhfit[i]->Write();
  hhups1pp[i]->Write();
  hhups2pp[i]->Write();
  hhups3pp[i]->Write();
  hhupspp[i]->Write();
  hhups1[i]->Write();
  hhups2[i]->Write();
  hhups3[i]->Write();
  hhups[i]->Write();
  hhall_pp[i]->Write();
  hhcorrbg_pp[i]->Write();
  hhcorrbg_scaled[i]->Write();
}
fout->Write();
fout->Close();
*/

}

