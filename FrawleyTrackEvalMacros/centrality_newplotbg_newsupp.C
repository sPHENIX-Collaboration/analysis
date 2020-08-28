double factorial(int n)
{
  double result = 1;
  for(int i = n; i>0; i--)
    result*=i;
  
  return result;
}

double  integral(int nfg,int nbg,float lowerLimit, float upperLimit)
{
  // See AN 195 by Mike Tannenbaum, equation 9, for where this comes from
  // x is the signal, ie. FG-BG
  // lowerLimit and upperLimit are just the min and max signal to be considered

  //double stepsize=0.5;

  int steps = 200;
  double intgrl = 0;
  
  double dx=(upperLimit-lowerLimit)/(double)steps;

  for(int k = 0; k<=nfg; k++) {
    double factorial_up = factorial(nfg+nbg-k);
    double factorial_down = factorial(nbg)*factorial(nfg-k)*factorial(k);
    double tmp = factorial_up/factorial_down*0.5*pow(0.5, nfg+nbg-k);
    for(int itmp=0;itmp<=steps;itmp++)
      {
	double x=lowerLimit+dx*(float)itmp;
	intgrl += tmp*pow(x,k)*exp(-x)*dx;
      }
  }
  return intgrl;
}

void error_fg_bg_pair(int nfg, int nbg, double& err_up,double& err_down)
{
  //nfg: # of fg in the bin
  //nbg: # of bg in the bin
  //err_up: upside error
  //err_down: downside error

  if(nfg+nbg > 170)
    {
      cout << " error_fg_bg_pair: factorials too large, return symmetric error " << endl;
      err_up = sqrt(nfg+nbg);
      err_down = err_up;
      return;
    }

  int nmax = 50;
  if(nfg-nbg>nmax) 
    {
      //cout<<" error_fg_bg_pair: Signal > " << nmax << " counts - use symmetric errors "<<endl;
      err_up = sqrt(nfg+nbg);
      err_down = err_up;
      return;
    }

  
  if(nfg<=nbg) 
    {
      //cout<<" error_fg_bg_pair: This code does not work in the case of nfg<=nbg"<<endl;
      cout << "nfg = " << nfg << " nbg = " << nbg << endl;
      err_up=sqrt(nfg+nbg);
      err_down = err_up;
      return;
    }
  
  // Establish the upper signal boundary for integration - Poisson distribution
  // should be negligible by the time the signal reaches this number
  // Used only for getting the total integral

  float maxsignal=0;
  if(nfg+nbg < 10)
    maxsignal = (nfg+nbg)* 4;
  else if(nfg+nbg < 30)
    maxsignal = (nfg+nbg)* 3;
  else
    maxsignal = (nfg+nbg)* 2;

  float total=integral(nfg,nbg,0,maxsignal);

  float binSize=0.1;

  float cl_up, cl_down;
  int i = 0;
  bool is_cl_down = false;
  //.. calculate the lower error bars using 16% in lower tail
  while(1) {
    if(!is_cl_down) err_down = i*binSize;
    if(err_down<0) err_down = 0;
    cl_down = integral(nfg,nbg,0, err_down)/total;
    if(cl_down>=0.159) {
      is_cl_down = true;
      break;
    }
    i++;
  }
  
  i = 0;
  while(1) {
    err_up = err_down+i*binSize;
    cl_up = cl_down+integral(nfg,nbg,err_down, err_up)/total;
    if(cl_up>=0.841) break;
    i++;
  }

  //cout<< " nfg = " << nfg << " n bg = " << nbg << " signal = " << nfg-nbg << " signal+err_up ="<<err_up
  //    <<" signal-err_down = "<<err_down<<endl;
  //cout<<" result =" << nfg-nbg << "+" << err_up-(nfg-nbg) << "-" << (nfg-nbg)-err_down << endl;

  err_down = (nfg-nbg)-err_down;
  err_up = err_up - (nfg-nbg); 
}


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

void centrality_newplotbg_newsupp() {

//gROOT->LoadMacro("sPHENIXStyle/sPhenixStyle.C");
//SetsPhenixStyle();

gStyle->SetOptStat(0);
gStyle->SetOptFit(0);

TRandom* myrandom = new TRandom3();
const int nbins = 15;
double eideff = 0.9;
string times = "*";
TLatex* tl[15];
char tlchar[999];

// Updates for 2020 BUP from Jamie (by ADF, August 14, 2020)
// 3 year run plan for 24 (28) cryoweeks / year:
//    AuAu year 1 25B (39B) MB events
//    AuAu year 3 85B (103B) MB evemts
//    Total AuAu 110B (142) MB events (i.e. 11B (14.2B) in 10% central) 
//    old: pp year 2 73 /Pb (101 /Pb) or 3T (4T) sampled events
//    updated: pp year (28 wks) 2 62 /Pb  or 2.6T  sampled events
// 5 year run plan
//    AuAu year 5 adds 206B MB events for a total of 316B (348B) (i.e. 31.6B (34.8B) in 10% central)
//    old: pp year 4 adds 130 /Pb sampled or 5.5T sampled events for a total of 8.5T (9.6T)
//   updated:  pp year 4 adds 80 /Pb sampled or 3.4T sampled events for a total of 5.9T 

// statscale factor is relative to 100B events for AuAu, and relative to 7.35T events in pp
 double set_statscale[4] = {3.48, 3.16, 1.42, 1.1};  
 double set_statscalepp[4] = {0.8, 1.16, 0.35, 0.41};
 std::string run_plan[4] = {"5 years/28 cryo-weeks", "5 years/24 cryo-weeks","3 years/28 cryo-weeks","3 years/24 cryo-weeks"};
 int scenario = 0;  // scenario = {0,1,2,3} = {5yrs_28wks, 5yrs_24wks, 3yrs_28wks, 3yrs_24wks}  (BUP 2020 uses 2 and 0)
 double statscale = set_statscale[scenario]; 
 double statscalepp = set_statscalepp[scenario]; 

 int auauevts = (int) (statscale * 10.0);  // 10% most central events = statscale * 10% of 100B
 
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

// Need different raa at each centrality

  // this is for the Y(2S) and Y(3S) centrality dependence binning
  static const int NCENT = 7;
  double centlow[NCENT] = {0,5,10,20,30,40,60};  
  double centhigh[NCENT] = {5, 10,20,30,40,60,92};
  double Ncoll[NCENT] = {1067, 858, 610, 378, 224, 94.2, 14.5};

  double raacent_ups1s[NCENT] = {0.527483, 0.544815, 0.581766, 0.644214, 0.721109, 0.853888, 0.998349}; 
  double raacent_ups2s[NCENT] = {0.165314, 0.179333, 0.210351, 0.26748, 0.34664, 0.510807, 0.979886}; 
  double raacent_ups3s[NCENT] = {0.0302562, 0.0362929, 0.047993, 0.0706653, 0.108092, 0.231598, 0.961714};


/*
  // not used
  static const int NCENT = 4;
  double centlow[NCENT] = {0,20,40,60};
  double centhigh[NCENT] = {20,40,60,92};
  double Ncoll[NCENT] = {783, 301, 94.2, 14.5};
  double raacent_ups3s[4] = {0.038949, 0.0838922, 0.220475, 0.924073};
  double raacent_ups1s[NCENT] = {0.527483, 0.544815, 0.581766, 0.644214}; // dummy 
  double raacent_ups2s[NCENT] = {0.165314, 0.179333, 0.210351, 0.26748};  // dummy 
*/

/*
  // this is to get the 0-10% pT dependence 
  static const int NCENT = 6;
  double centlow[NCENT] = {0,10,20,30,40,60};
  double centhigh[NCENT] = {10,20,30,40,60,92};
  double Ncoll[NCENT] = {962, 610, 378, 224, 94.2, 14.5};
*/

/*
  // this is for the Y(3S) centrality dependence binning 
  static const int NCENT = 3;
  double centlow[NCENT] = {0,30,60};
  double centhigh[NCENT] = {30,60,92};
  double Ncoll[NCENT] = {648, 137, 14.5};
  double raacent_ups1s[NCENT] = {0.527483, 0.544815, 0.644214}; // dummy 
  double raacent_ups2s[NCENT] = {0.165314, 0.179333, 0.26748};  // dummy 
  double raacent_ups3s[3] = {0.0458138, 0.167527, 0.924073};
*/

  double centwidth[NCENT];
  for(int i=0; i<NCENT; ++i)
    {
      centwidth[i] = (centhigh[i] - centlow[i]) / 100.0;
      cout << " cent bin " << i << " has width " << centwidth[i] << endl;
    }

  int icent = 6;

  double raapt[9],raa1s[9],raa2s[9],raa3s[9];
raapt[0] = 1.5;
raapt[1] = 4.5;
raapt[2] = 7.5;
raapt[3] = 10.5;
raapt[4] = 13.5;
raa1s[0] = raacent_ups1s[icent];
raa1s[1] = raacent_ups1s[icent];
raa1s[2] = raacent_ups1s[icent];
raa1s[3] = raacent_ups1s[icent];
raa1s[4] = raacent_ups1s[icent];
raa2s[0] = raacent_ups2s[icent];
raa2s[1] = raacent_ups2s[icent];
raa2s[2] = raacent_ups2s[icent];
raa2s[3] = raacent_ups2s[icent];
raa2s[4] = raacent_ups2s[icent];
raa3s[0] = raacent_ups3s[icent];
raa3s[1] = raacent_ups3s[icent];
raa3s[2] = raacent_ups3s[icent];
raa3s[3] = raacent_ups3s[icent];
raa3s[4] = raacent_ups3s[icent];

/*
raa1s[0] = 0.535;
raa1s[1] = 0.535;
raa1s[2] = 0.535;
raa1s[3] = 0.535;
raa1s[4] = 0.535;
raa2s[0] = 0.170;
raa2s[1] = 0.170;
raa2s[2] = 0.170;
raa2s[3] = 0.170;
raa2s[4] = 0.170;
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
raa3s[0] = 0.035;
raa3s[1] = 0.035;
raa3s[2] = 0.035;
raa3s[3] = 0.035;
raa3s[4] = 0.035;
*/

TGraph* grRAA1S = new TGraph(5,raapt,raa1s);
TGraph* grRAA2S = new TGraph(5,raapt,raa2s);
TGraph* grRAA3S = new TGraph(5,raapt,raa3s);

double NNN = statscale * 7780./0.49;      // from sPHENIX proposal for 100B minbias Au+Au events
double NNNpp = statscalepp * 12130./0.90; // Upsilons in p+p from sPHENIX proposal for 7350B p+p events
double ppauaurat = NNNpp/NNN;

// NNN is the unsuppressed number of Upsilons in 0-10% most central events for 100B MB AuAu collisions 
// That is for Ncoll = 962, for any other centrality, scale it by Ncoll
// Also need to scale by number of events, i.e. centrality bin width
 double Ncoll_ref = (Ncoll[0] + Ncoll[1]) / 2.0;  // reference is 0-10% centrality, so average 0-5 and 5-10
 double ncollfact = (Ncoll[icent] / Ncoll_ref);
 double centwidth_ref = (centwidth[0]+centwidth[1]);  // reference is 0-10% centrality, so sum 0-5 and 5-10
 double centwidthfact = (centwidth[icent] / centwidth_ref);
 NNN *= ncollfact*centwidthfact;   // unsuppressed Upsilons for this centrality bin

double frac[3];  // upsilons fractions
  frac[0] = 0.7117;
  frac[1] = 0.1851;
  frac[2] = 0.1032;
double scale[3]; // mass scaling
  scale[0] = 1.0;
  scale[1] = 1.0595;
  scale[2] = 1.0946;
//double supcor[3]; // suppression
//  supcor[0]=0.535; 
//  supcor[1]=0.17; 
//  supcor[2]=0.035;
//int Nups1 = int(NNN*eideff*eideff*frac[0]*supcor[0]);
//int Nups2 = int(NNN*eideff*eideff*frac[1]*supcor[1]);
//int Nups3 = int(NNN*eideff*eideff*frac[2]*supcor[2]);
int Nups1nosup = int(NNN*eideff*eideff*frac[0]);
int Nups2nosup = int(NNN*eideff*eideff*frac[1]);
int Nups3nosup = int(NNN*eideff*eideff*frac[2]);
int Nups1pp = int(NNNpp*0.90*frac[0]); // always 95% eideff in p+p
int Nups2pp = int(NNNpp*0.90*frac[1]);
int Nups3pp = int(NNNpp*0.90*frac[2]);

int nchan=400;
double start=0.0;
double stop=20.0;

string str_UpsilonPt = "(2.0*3.14159*x*[0]*pow((1 + x*x/(4*[1]) ),-[2]))";
TF1* fUpsilonPt = new TF1("fUpsilonPt",str_UpsilonPt.c_str(),0.,20.);
fUpsilonPt->SetParameters(72.1, 26.516, 10.6834);
double upsnorm = fUpsilonPt->Integral(0.,20.);

// count all upsilons with suppression
double tmpsum1=0.;
double tmpsum2=0.;
double tmpsum3=0.;
for(int j=0; j<nbins; j++) {
  double s1 = j*1.0;
  double s2 = s1 + 1.0;
  tmpsum1 += Nups1nosup*fUpsilonPt->Integral(s1,s2)/upsnorm * grRAA1S->Eval((s1+s2)/2.);
  tmpsum2 += Nups2nosup*fUpsilonPt->Integral(s1,s2)/upsnorm * grRAA2S->Eval((s1+s2)/2.);
  tmpsum3 += Nups3nosup*fUpsilonPt->Integral(s1,s2)/upsnorm * grRAA3S->Eval((s1+s2)/2.);
}
int Nups1 = int(tmpsum1);
int Nups2 = int(tmpsum2);
int Nups3 = int(tmpsum3);

  cout << "Total number of ALL Upsilons in AuAu = " << NNN << endl;
  cout << "Total number of ALL Upsilons in pp = " << NNNpp << endl;
  cout << "Number of upsilons in pp in acceptance = " << NNNpp*frac[0] << " " << NNNpp*frac[1] << " " << NNNpp*frac[2] << endl;
  cout << "Number of upsilons in AuAu in acceptance = " << NNN*frac[0] << " " << NNN*frac[1] << " " << NNN*frac[2] << endl;
  cout << "Number of upsilons in AuAu after eID efficiency = " << Nups1nosup << " " << Nups2nosup << " " << Nups3nosup << endl;
  cout << "Number of upsilons in AuAu plot = " << Nups1 << " " << Nups2 << " " << Nups3 << endl;
  cout << "Number of upsilons in pp plot = " << Nups1pp << " " << Nups2pp << " " << Nups3pp << endl;

//====================================================================================================
/*
f=new TFile("UpsilonsInHijing.root");
  cout << endl << endl << "Number of entries in Upsilon NTuple = " << ntpups->GetEntries() << endl;

  TH1D* hforfit = new TH1D("hforfit","",nchan,start,stop);
  TH1D* hUps1 = new TH1D("hUps1","",nchan,start,stop);
  TH1D* hUps2 = new TH1D("hUps2","",nchan,start,stop);
  TH1D* hUps3 = new TH1D("hUps3","",nchan,start,stop);
  TH1D* hUps1pp = new TH1D("hUps1pp","",nchan,start,stop);
  TH1D* hUps2pp = new TH1D("hUps2pp","",nchan,start,stop);
  TH1D* hUps3pp = new TH1D("hUps3pp","",nchan,start,stop);
    hforfit->Sumw2();
    hUps1->Sumw2();
    hUps2->Sumw2();
    hUps3->Sumw2();
    hUps1pp->Sumw2();
    hUps2pp->Sumw2();
    hUps3pp->Sumw2();

//------ Additional smearing to reproduce mass resolution -----
//double smear = 0.098; // 127
//double smear = 0.090;   // 119
//double smear = 0.067;   // 100 MeV benchmark
double smear = 0.046;  // 86 MeV resolution, Aalysis Note result 
float mass;
TChain* tree = new TChain("ntpups");
tree->Add("UpsilonsInHijing.root");
tree->SetBranchAddress("mass",&mass);
int countups=0;
  for(int j=0; j<tree->GetEntries(); j++){
    tree->GetEvent(j);
    double newmass = myrandom->Gaus(mass,smear);
    hforfit->Fill(newmass);
    if(countups<=Nups1) hUps1->Fill(newmass);
    if(countups<=Nups2) hUps2->Fill(newmass*1.0595);
    if(countups<=Nups3) hUps3->Fill(newmass*1.0946);
    if(countups<=Nups1pp) hUps1pp->Fill(newmass);
    if(countups<=Nups2pp) hUps2pp->Fill(newmass*1.0595);
    if(countups<=Nups3pp) hUps3pp->Fill(newmass*1.0946);
    countups++;
  }
delete tree;
//---------------------------------------------------------------

  hforfit->SetDirectory(gROOT);
  hUps1->SetDirectory(gROOT);
  hUps2->SetDirectory(gROOT);
  hUps3->SetDirectory(gROOT);
  hUps1pp->SetDirectory(gROOT);
  hUps2pp->SetDirectory(gROOT);
  hUps3pp->SetDirectory(gROOT);

f->Close();

TH1F* hUps = (TH1F*)hUps1->Clone("hUps");
hUps->Add(hUps2);
hUps->Add(hUps3);
hUps->SetLineWidth(2);
TH1F* hUpspp = (TH1F*)hUps1pp->Clone("hUpspp");
hUpspp->Add(hUps2pp);
hUpspp->Add(hUps3pp);
hUpspp->SetLineWidth(2);

TCanvas* cups = new TCanvas("cups","Upsilons (all pT)",100,100,1000,500);
cups->Divide(2,1);

  cups_1->cd();
  cups_1->SetLogy();
  fCB->SetParameter(0,5000.);
  fCB->SetParameter(1,1.5); 
  fCB->SetParameter(2,1.2); 
  fCB->SetParameter(3,9.448);
  fCB->SetParameter(4,0.070);
  hforfit->Fit("fCB","rl","",5.,9.7);
  hforfit->SetAxisRange(5.,11.);
  hforfit->SetLineWidth(2);
  hforfit->Draw();
  double myupsmass = fCB->GetParameter(3);
  double upswidth = fCB->GetParameter(4);

  cups_2->cd();
  cups_2->SetLogy();
  fTCB->SetParameter(0,2000.);
  fTCB->SetParameter(1,1.5);
  fTCB->SetParameter(2,1.25);
  fTCB->SetParameter(3,9.448);
  fTCB->FixParameter(4,upswidth); // fix width from the single peak fit
  fTCB->SetParameter(5,500.);
  fTCB->SetParameter(6,100.);
  hUps->Fit(fTCB,"rl","",6.,11.);
  double upswidth3 = fTCB->GetParameter(4);
  double upswidth3err = fTCB->GetParError(4);
  hUps->SetAxisRange(7.0,11.0);
  hUps->Draw();

cout << "MASS RESOLUTION = " << upswidth << " " << upswidth3 << " +- " << upswidth3err << endl;
*/
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
TH1D* hhupspp[nbins+1];
TH1D* hhups1pp[nbins+1];
TH1D* hhups2pp[nbins+1];
TH1D* hhups3pp[nbins+1];
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
  sprintf(hhname,"hhupspp_%d",i);
  hhupspp[i] = new TH1D(hhname,"",nchan,start,stop);
  hhupspp[i]->Sumw2();
  sprintf(hhname,"hhups1pp_%d",i);
  hhups1pp[i] = new TH1D(hhname,"",nchan,start,stop);
  hhups1pp[i]->Sumw2();
  sprintf(hhname,"hhups2pp_%d",i);
  hhups2pp[i] = new TH1D(hhname,"",nchan,start,stop);
  hhups2pp[i]->Sumw2();
  sprintf(hhname,"hhups3pp_%d",i);
  hhups3pp[i] = new TH1D(hhname,"",nchan,start,stop);
  hhups3pp[i]->Sumw2();
  hhupspp[i]->SetLineWidth(2);
  hhups1pp[i]->SetLineWidth(2);
  hhups2pp[i]->SetLineWidth(2);
  hhups3pp[i]->SetLineWidth(2);
}

for(int j=0; j<nbins; j++) {

  double s1 = j*1.0;
  double s2 = s1 + 1.0;
  double tmpnups1 = Nups1nosup*fUpsilonPt->Integral(s1,s2)/upsnorm * grRAA1S->Eval((s1+s2)/2.);
  double tmpnups2 = Nups2nosup*fUpsilonPt->Integral(s1,s2)/upsnorm * grRAA2S->Eval((s1+s2)/2.);
  double tmpnups3 = Nups3nosup*fUpsilonPt->Integral(s1,s2)/upsnorm * grRAA3S->Eval((s1+s2)/2.);
  //cout << "Nups1["<<j<<"]="<<tmpnups1<<";"<<endl;
  //cout << "Nups2["<<j<<"]="<<tmpnups2<<";"<<endl;
  //cout << "Nups3["<<j<<"]="<<tmpnups3<<";"<<endl;
  fCBauau->SetParameter(3,tonypar3);
    for(int i=0; i<int(tmpnups1+0.5); i++) { double myrnd = fCBauau->GetRandom(); hhups1[j]->Fill(myrnd); hhups[j]->Fill(myrnd); }
  fCBauau->SetParameter(3,tonypar3*scale[1]);
    for(int i=0; i<int(tmpnups2+0.5); i++) { double myrnd = fCBauau->GetRandom(); hhups2[j]->Fill(myrnd); hhups[j]->Fill(myrnd); }
  fCBauau->SetParameter(3,tonypar3*scale[2]);
    for(int i=0; i<int(tmpnups3+0.5); i++) { double myrnd = fCBauau->GetRandom(); hhups3[j]->Fill(myrnd); hhups[j]->Fill(myrnd); }

  double tmpnups1pp = Nups1pp*fUpsilonPt->Integral(s1,s2)/upsnorm;
  double tmpnups2pp = Nups2pp*fUpsilonPt->Integral(s1,s2)/upsnorm;
  double tmpnups3pp = Nups3pp*fUpsilonPt->Integral(s1,s2)/upsnorm;
  cout << "Nups1pp["<<j<<"]="<<tmpnups1pp<<";"<<endl;
  cout << "Nups2pp["<<j<<"]="<<tmpnups2pp<<";"<<endl;
  cout << "Nups3pp["<<j<<"]="<<tmpnups3pp<<";"<<endl;
  fCBpp->SetParameter(3,tonypar3);
    for(int i=0; i<int(tmpnups1pp+0.5); i++) { double myrnd = fCBpp->GetRandom(); hhups1pp[j]->Fill(myrnd); hhupspp[j]->Fill(myrnd); }
  fCBpp->SetParameter(3,tonypar3*scale[1]);
    for(int i=0; i<int(tmpnups2pp+0.5); i++) { double myrnd = fCBpp->GetRandom(); hhups2pp[j]->Fill(myrnd); hhupspp[j]->Fill(myrnd); }
  fCBpp->SetParameter(3,tonypar3*scale[2]);
    for(int i=0; i<int(tmpnups3pp+0.5); i++) { double myrnd = fCBpp->GetRandom(); hhups3pp[j]->Fill(myrnd); hhupspp[j]->Fill(myrnd); }

} // end loop over pT bins

// all pT

  fCBpp->SetParameter(3,tonypar3);
  fCBauau->SetParameter(3,tonypar3);
    for(int i=0; i<int(Nups1+0.5); i++) { double myrnd = fCBauau->GetRandom(); hhups1[nbins]->Fill(myrnd); hhups[nbins]->Fill(myrnd); }
    for(int i=0; i<int(Nups1pp+0.5); i++) { double myrnd = fCBpp->GetRandom(); hhups1pp[nbins]->Fill(myrnd); hhupspp[nbins]->Fill(myrnd); }
  fCBpp->SetParameter(3,tonypar3*scale[1]);
  fCBauau->SetParameter(3,tonypar3*scale[1]);
    for(int i=0; i<int(Nups2+0.5); i++) { double myrnd = fCBauau->GetRandom(); hhups2[nbins]->Fill(myrnd); hhups[nbins]->Fill(myrnd); }
    for(int i=0; i<int(Nups2pp+0.5); i++) { double myrnd = fCBpp->GetRandom(); hhups2pp[nbins]->Fill(myrnd); hhupspp[nbins]->Fill(myrnd); }
  fCBpp->SetParameter(3,tonypar3*scale[2]);
  fCBauau->SetParameter(3,tonypar3*scale[2]);
    for(int i=0; i<int(Nups3+0.5); i++) { double myrnd = fCBauau->GetRandom(); hhups3[nbins]->Fill(myrnd); hhups[nbins]->Fill(myrnd); }
    for(int i=0; i<int(Nups3pp+0.5); i++) { double myrnd = fCBpp->GetRandom(); hhups3pp[nbins]->Fill(myrnd); hhupspp[nbins]->Fill(myrnd); }

// fit and draw pp no bg

TCanvas* cupspp = new TCanvas("cupspp","Upsilons in p+p",100,100,600,600);
  fTCBpp->SetParameter(0,2000.);
  fTCBpp->FixParameter(1,tonypar1);
  fTCBpp->FixParameter(2,tonypar2);
  fTCBpp->SetParameter(3,tonypar3);
  fTCBpp->FixParameter(4,tonypar4); // fix width from the single peak fit
  fTCBpp->SetParameter(5,500.);
  fTCBpp->SetParameter(6,100.);
  hhupspp[nbins]->Fit(fTCBpp,"rl","",7.,11.);
  hhupspp[nbins]->SetAxisRange(7.,11.);
  hhupspp[nbins]->SetMarkerSize(1.0);
  hhupspp[nbins]->GetXaxis()->SetTitle("Invariant mass [GeV/c^{2}]");
  hhupspp[nbins]->GetXaxis()->SetTitleOffset(1.0);
  double tmpamp1 = hhupspp[nbins]->GetFunction("fTCBpp")->GetParameter(0);
  double tmpamp5 = tmpamp1*frac[1]/frac[0]; // correct fit for correct upsilon states ratio
  double tmpamp6 = tmpamp1*frac[2]/frac[0];
  hhupspp[nbins]->Draw();

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

TCanvas* cupsauau = new TCanvas("cupsauau","Upsilons in Au+Au",100,100,600,600);
  fTCBauau->SetParameter(0,2000.);
  fTCBauau->FixParameter(1,tonypar1);
  fTCBauau->FixParameter(2,tonypar2);
  fTCBauau->SetParameter(3,tonypar3);
  fTCBauau->FixParameter(4,tonypar4); // fix width from the single peak fit
  fTCBauau->SetParameter(5,500.);
  fTCBauau->SetParameter(6,100.);
  hhups[nbins]->Fit(fTCBauau,"rl","",7.,11.);
  hhups[nbins]->SetAxisRange(7.,11.);
  hhups[nbins]->SetMarkerSize(1.0);
  hhups[nbins]->GetXaxis()->SetTitle("Invariant mass [GeV/c^{2}]");
  hhups[nbins]->GetXaxis()->SetTitleOffset(1.0);
  tmpamp1 = hhups[nbins]->GetFunction("fTCBauau")->GetParameter(0);
  tmpamp5 = tmpamp1*frac[1]/frac[0]; // correct fit for correct upsilon states ratio
  tmpamp6 = tmpamp1*frac[2]/frac[0];
  hhups[nbins]->Draw();

TCanvas* cupsvspt = new TCanvas("cupsvspt","Upsilons vs. p_{T}",100,100,1200,900);
cupsvspt->Divide(4,3);
for(int i=0; i<12; i++) {
if(i==0)  {cupsvspt->cd(1);}
if(i==1)  {cupsvspt->cd(2);}
if(i==2)  {cupsvspt->cd(3);}
if(i==3)  {cupsvspt->cd(4);}
if(i==4)  {cupsvspt->cd(5);}
if(i==5)  {cupsvspt->cd(6);}
if(i==6)  {cupsvspt->cd(7);}
if(i==7)  {cupsvspt->cd(8);}
if(i==8)  {cupsvspt->cd(9);}
if(i==9)  {cupsvspt->cd(10);}
if(i==10) {cupsvspt->cd(11);}
if(i==11) {cupsvspt->cd(12);}
hhups[i]->SetAxisRange(7.0,11.0); hhups[i]->Draw();
  sprintf(tlchar,"%d-%d GeV",i,i+1);   tl[i] = new TLatex(8.0,hhups[i]->GetMaximum()*0.95,tlchar); tl[i]->Draw();
}

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

// The correlated background should scale with Ncoll, right?

double corrbgfitpar0;
double corrbgfitpar1;

TFile* f=new TFile("ccbb_eideff09.root");
for(int i=0; i<nbins+1; i++) {
  sprintf(tmpname,"hhbottom_%d",i);
  hhbottom[i] = (TH1D*)f->Get(tmpname);
  hhbottom[i]->SetDirectory(gROOT);
  sprintf(tmpname,"hhcharm_%d",i);
  hhcharm[i] = (TH1D*)f->Get(tmpname);
  hhcharm[i]->SetDirectory(gROOT);
  sprintf(tmpname,"hhdy_%d",i);
  hhdy[i] = (TH1D*)f->Get(tmpname);
  hhdy[i]->SetDirectory(gROOT);
  sprintf(tmpname,"hhcorrbg_%d",i);
  hhcorrbg[i] = (TH1D*)hhbottom[i]->Clone(tmpname);
  hhcorrbg[i]->Add(hhcharm[i]);
  hhcorrbg[i]->Add(hhdy[i]);
  sprintf(tmpname,"hhcorrbg_scaled_%d",i);
  hhcorrbg_scaled[i] = (TH1D*)hhcorrbg[i]->Clone(tmpname);
  hhcorrbg[i]->Fit("expo","rql","",statscale_lowlim,statscale_uplim);
  hhbottom[i]->Fit("expo","rql","",statscale_lowlim,statscale_uplim);
  hhdy[i]->Fit("expo","rql","",statscale_lowlim,statscale_uplim);
    if(i==nbins) {
       corrbgfitpar0 = hhcorrbg[i]->GetFunction("expo")->GetParameter(0);
       corrbgfitpar1 = hhcorrbg[i]->GetFunction("expo")->GetParameter(1);
    }
    cout << "bgpar0["<< i <<"]="<<hhcorrbg[i]->GetFunction("expo")->GetParameter(0)+TMath::Log(statscale)<<";"<< endl; 
    cout << "bgpar1["<< i <<"]="<<hhcorrbg[i]->GetFunction("expo")->GetParameter(1)<<";"<< endl; 
    for(int k=1; k<=hhcorrbg[i]->GetNbinsX(); k++) {
      if(hhcorrbg[i]->GetBinLowEdge(k)<statscale_lowlim || (hhcorrbg[i]->GetBinLowEdge(k)+hhcorrbg[i]->GetBinWidth(k))>statscale_uplim) {
        hhcorrbg_scaled[i]->SetBinContent(k,0.); 
        hhcorrbg_scaled[i]->SetBinError(k,0.);
      }
      else {
	// assumes corr bg scales with Ncoll
        double tmp = ncollfact * centwidthfact * statscale * hhcorrbg[i]->GetFunction("expo")->Eval(hhcorrbg[i]->GetBinCenter(k));
        double tmprnd = myrandom->Poisson(tmp); 
        if(tmprnd<0.) { tmprnd=0.; }
        hhcorrbg_scaled[i]->SetBinContent(k,tmprnd); 
        hhcorrbg_scaled[i]->SetBinError(k,sqrt(tmprnd));
      }
    }
  hhcorrbg_scaled[i]->Fit("expo","rql","",statscale_lowlim,statscale_uplim);
  hhcorrbg[i]->SetDirectory(gROOT);
  hhcorrbg_scaled[i]->SetDirectory(gROOT);
}
f->Close();

TCanvas* c2 = new TCanvas("c2","Correlated BG (all p_{T})",100,100,600,600);
  
hhbottom[nbins]->SetAxisRange(7.0,14.0);
hhbottom[nbins]->SetLineColor(kBlue);
hhbottom[nbins]->SetLineWidth(2);

hhbottom[nbins]->GetXaxis()->SetTitle("Invariant mass [GeV/c^{2}]");
hhbottom[nbins]->GetXaxis()->SetTitleOffset(1.0);
hhbottom[nbins]->GetXaxis()->SetTitleColor(1);
hhbottom[nbins]->GetXaxis()->SetTitleSize(0.040);
hhbottom[nbins]->GetXaxis()->SetLabelSize(0.040);
//hhbottom[nbins]->GetYaxis()->SetTitle("Correlated background");
hhbottom[nbins]->GetYaxis()->SetTitleOffset(1.3);
hhbottom[nbins]->GetYaxis()->SetTitleSize(0.040);
hhbottom[nbins]->GetYaxis()->SetLabelSize(0.040);

hhcorrbg[nbins]->SetAxisRange(7.0,14.0);
hhcorrbg[nbins]->SetMinimum(0.1);
hhcorrbg[nbins]->SetLineColor(kBlack);
hhcorrbg[nbins]->SetLineWidth(2);
hhcorrbg[nbins]->Draw("hist");

hhbottom[nbins]->SetMarkerColor(kBlue);
hhbottom[nbins]->SetLineColor(kBlue);
hhbottom[nbins]->Draw("same");

hhdy[nbins]->SetMarkerColor(kGreen+2);
hhdy[nbins]->SetLineColor(kGreen+2);
hhdy[nbins]->SetLineWidth(2);
hhdy[nbins]->Draw("same");

hhcharm[nbins]->SetMarkerColor(kRed);
hhcharm[nbins]->SetLineColor(kRed);
hhcharm[nbins]->SetLineWidth(2);
hhcharm[nbins]->Draw("same");

TCanvas* c0 = new TCanvas("c0","Correlated BG vs. p_{T} 10B events",100,100,1200,900);
c0->Divide(4,3);
for(int i=0; i<nbins; i++) {
if(i==0)  {c0->cd(1);}
if(i==1)  {c0->cd(2);}
if(i==2)  {c0->cd(3);}
if(i==3)  {c0->cd(4);}
if(i==4)  {c0->cd(5);}
if(i==5)  {c0->cd(6);}
if(i==6)  {c0->cd(7);}
if(i==7)  {c0->cd(8);}
if(i==8)  {c0->cd(9);}
if(i==9)  {c0->cd(10);}
if(i==10) {c0->cd(11);}
if(i==11) {c0->cd(12);}
  hhcorrbg[i]->SetAxisRange(7.,14.);
  hhcorrbg[i]->GetFunction("expo")->SetLineColor(kBlack); 
  hhbottom[i]->GetFunction("expo")->SetLineColor(kBlue); 
  hhdy[i]->GetFunction("expo")->SetLineColor(kGreen+2); 
  hhcorrbg[i]->SetLineColor(kBlack); 
  hhbottom[i]->SetLineColor(kBlue); 
  hhdy[i]->SetLineColor(kGreen+2); 
  hhcorrbg[i]->Draw();
  hhbottom[i]->Draw("same");
  hhdy[i]->Draw("same");
  sprintf(tlchar,"%d-%d GeV",i,i+1);   tl[i] = new TLatex(8.0,hhcorrbg[i]->GetMaximum()*0.95,tlchar); tl[i]->Draw();
}

TCanvas* c0scaled = new TCanvas("c0scaled","SCALED Correlated BG vs. p_{T}",100,100,1200,900);
c0scaled->Divide(4,3);
for(int i=0; i<nbins; i++) {
if(i==0)  {c0scaled->cd(1);}
if(i==1)  {c0scaled->cd(2);}
if(i==2)  {c0scaled->cd(3);}
if(i==3)  {c0scaled->cd(4);}
if(i==4)  {c0scaled->cd(5);}
if(i==5)  {c0scaled->cd(6);}
if(i==6)  {c0scaled->cd(7);}
if(i==7)  {c0scaled->cd(8);}
if(i==8)  {c0scaled->cd(9);}
if(i==9)  {c0scaled->cd(10);}
if(i==10) {c0scaled->cd(11);}
if(i==11) {c0scaled->cd(12);}
hhcorrbg_scaled[i]->SetAxisRange(7.0,14.0); hhcorrbg_scaled[i]->Draw();
  sprintf(tlchar,"%d-%d GeV",i,i+1);   tl[i] = new TLatex(8.0,hhcorrbg_scaled[i]->GetMaximum()*0.95,tlchar); tl[i]->Draw();
}

//-----------------------------------------------------------------------------------------
//  Correlated bg in p+p
//-----------------------------------------------------------------------------------------

TH1D* hhbottom_pp[nbins+1];
TH1D* hhdy_pp[nbins+1];
TH1D* hhcorrbg_pp[nbins+1];
TH1D* hhall_pp[nbins+1];

double ppcorr = 8300./10./962.;
TF1* fbottom_nosup_corr = new TF1("fbottom_nosup_corr","[0]+[1]*x",5.,14.);
fbottom_nosup_corr->SetParameters(-2.13861, 0.683323);

for(int i=0; i<nbins+1; i++) {

  sprintf(tmpname,"hhbottom_pp_%d",i);
  hhbottom_pp[i] = (TH1D*)hhbottom[i]->Clone(tmpname);
  for(int k=1; k<=hhbottom_pp[i]->GetNbinsX(); k++) {
    if(hhbottom_pp[i]->GetBinLowEdge(k)<statscale_lowlim || (hhbottom_pp[i]->GetBinLowEdge(k)+hhbottom_pp[i]->GetBinWidth(k))>statscale_uplim) {
      hhbottom_pp[i]->SetBinContent(k,0.);
      hhbottom_pp[i]->SetBinError(k,0.);
    }
    else {
      double tmp = ppcorr * fbottom_nosup_corr->Eval(hhbottom[i]->GetBinCenter(k)) * hhbottom[i]->GetFunction("expo")->Eval(hhbottom[i]->GetBinCenter(k));
      double tmprnd = myrandom->Poisson(tmp);
      if(tmprnd<0.) { tmprnd=0.; }
      hhbottom_pp[i]->SetBinContent(k,tmprnd);
      hhbottom_pp[i]->SetBinError(k,sqrt(tmprnd));
    }
  }

  sprintf(tmpname,"hhdy_pp_%d",i);
  hhdy_pp[i] = (TH1D*)hhdy[i]->Clone(tmpname);
  for(int k=1; k<=hhdy_pp[i]->GetNbinsX(); k++) {
    if(hhdy_pp[i]->GetBinLowEdge(k)<statscale_lowlim || (hhdy_pp[i]->GetBinLowEdge(k)+hhdy_pp[i]->GetBinWidth(k))>statscale_uplim) {
      hhdy_pp[i]->SetBinContent(k,0.);
      hhdy_pp[i]->SetBinError(k,0.);
    }
    else {
      double tmp = ppcorr * hhdy[i]->GetFunction("expo")->Eval(hhdy[i]->GetBinCenter(k));
      double tmprnd = myrandom->Poisson(tmp);
      if(tmprnd<0.) { tmprnd=0.; }
      hhdy_pp[i]->SetBinContent(k,tmprnd);
      hhdy_pp[i]->SetBinError(k,sqrt(tmprnd));
    }
  }

  sprintf(tmpname,"hhcorrbg_pp_%d",i);
  hhcorrbg_pp[i] = (TH1D*)hhbottom_pp[i]->Clone(tmpname);
  hhcorrbg_pp[i]->Add(hhdy_pp[i]);
    hhcorrbg_pp[i]->SetMarkerColor(kBlack);
    hhcorrbg_pp[i]->SetLineColor(kBlack);
    hhbottom_pp[i]->SetLineColor(kBlue);
    hhdy_pp[i]->SetLineColor(kGreen+2);
  sprintf(tmpname,"hhall_pp_%d",i);
  hhall_pp[i] = (TH1D*)hhcorrbg_pp[i]->Clone(tmpname);
  hhall_pp[i]->Add(hhupspp[i]);
    hhall_pp[i]->SetLineColor(kMagenta);
    hhall_pp[i]->SetMarkerColor(kMagenta);

  hhcorrbg_pp[i]->Fit("expo","rql","",statscale_lowlim,statscale_uplim);
  hhcorrbg_pp[i]->GetFunction("expo")->SetLineColor(kBlack);
  hhbottom_pp[i]->Fit("expo","rql","",statscale_lowlim,statscale_uplim);
  hhbottom_pp[i]->GetFunction("expo")->SetLineColor(kBlue);
  hhdy_pp[i]->Fit("expo","rql","",statscale_lowlim,statscale_uplim);
  hhdy_pp[i]->GetFunction("expo")->SetLineColor(kGreen+2);

} // end loop over pT bins

TCanvas* cbginpp = new TCanvas("cbginpp","corr bg in pp",10,10,700,700);
  hhall_pp[nbins]->SetAxisRange(7.,12.);
  hhcorrbg_pp[nbins]->Draw("pehist");
  hhbottom_pp[nbins]->Draw("histsame");
  hhdy_pp[nbins]->Draw("histsame");


TCanvas* cpp = new TCanvas("cpp","corr bg + sig in pp",100,100,700,700);
  hhall_pp[nbins]->SetAxisRange(7.,12.);
  hhall_pp[nbins]->Draw("pehist");
  hhcorrbg_pp[nbins]->Draw("pesame");
  hhbottom_pp[nbins]->Draw("same");
  hhdy_pp[nbins]->Draw("same");

TCanvas* cpp_vspt = new TCanvas("cpp_vspt","corr bg + sig vs pt in pp",50,50,1200,900);
cpp_vspt->Divide(4,3);
for(int i=0; i<nbins; i++) {
if(i==0)  {cpp_vspt->cd(1);}
if(i==1)  {cpp_vspt->cd(2);}
if(i==2)  {cpp_vspt->cd(3);}
if(i==3)  {cpp_vspt->cd(4);}
if(i==4)  {cpp_vspt->cd(5);}
if(i==5)  {cpp_vspt->cd(6);}
if(i==6)  {cpp_vspt->cd(7);}
if(i==7)  {cpp_vspt->cd(8);}
if(i==8)  {cpp_vspt->cd(9);}
if(i==9)  {cpp_vspt->cd(10);}
if(i==10) {cpp_vspt->cd(11);}
if(i==11) {cpp_vspt->cd(12);}
hhall_pp[i]->SetAxisRange(7.0,12.0); hhall_pp[i]->Draw("hist"); hhcorrbg_pp[i]->Draw("histsame");
  sprintf(tlchar,"%d-%d GeV",i,i+1);   tl[i] = new TLatex(8.0,hhcorrbg_pp[i]->GetMaximum()*0.95,tlchar); tl[i]->Draw();
}

//-----------------------------------------------------------------------------------------
// Combinatorial BG calculated for 10B central AuAu events
//-----------------------------------------------------------------------------------------
TCanvas* dummy = new TCanvas("dummy","dummy",0,0,500,500);

f = new TFile("fakee_eideff09.root");
for(int i=0; i<nbins+1; i++) {
  sprintf(tmpname,"hhfakefake_%d",i);
  hhfakefake[i] = (TH1D*)f->Get(tmpname);
  hhfakefake[i]->SetDirectory(gROOT);
}
f->Close();

f = new TFile("crossterms_eideff09.root");
for(int i=0; i<nbins+1; i++) {
  sprintf(tmpname,"hhfakehf_%d",i);
  hhfakehf[i] = (TH1D*)f->Get(tmpname);
  hhfakehf[i]->SetDirectory(gROOT);
}
f->Close();

TF1* fbg = new TF1("fbg","exp([0]+[1]*x)+exp([2]+[3]*x)",8.,11.);
fbg->SetParameters(10., -1.0, 4., -0.1);
fbg->SetParLimits(1.,-999.,0.);
fbg->SetParLimits(3.,-999.,0.);

for(int i=0; i<nbins+1; i++) {
  sprintf(tmpname,"hhcombbg_%d",i);
  hhcombbg[i] = (TH1D*)hhfakefake[i]->Clone(tmpname);
  hhcombbg[i]->Add(hhfakehf[i]);
  sprintf(tmpname,"hhcombbg_scaled_%d",i);
  hhcombbg_scaled[i] = (TH1D*)hhcombbg[i]->Clone(tmpname);
  if(i==nbins) { fbg->SetParameters(10., -1.0, 4., -0.1); }
  hhcombbg[i]->Fit(fbg,"qrl","",statscale_lowlim,statscale_uplim);

    for(int k=1; k<=hhcombbg[i]->GetNbinsX(); k++) {
      if(hhcombbg[i]->GetBinLowEdge(k)<statscale_lowlim || (hhcombbg[i]->GetBinLowEdge(k)+hhcombbg[i]->GetBinWidth(k))>statscale_uplim) {
        hhcombbg_scaled[i]->SetBinContent(k,0.);
        hhcombbg_scaled[i]->SetBinError(k,0.);
      }
      else {
	// assumes comb bg scales with Ncoll^2
        double tmp = ncollfact * ncollfact * centwidthfact * statscale * hhcombbg[i]->GetFunction("fbg")->Eval(hhcombbg[i]->GetBinCenter(k));
        double tmprnd = myrandom->Poisson(tmp);
        if(tmprnd<0.) { tmprnd=0.; }
        hhcombbg_scaled[i]->SetBinContent(k,tmprnd);
        hhcombbg_scaled[i]->SetBinError(k,sqrt(tmprnd));
      }
    }
  hhcombbg_scaled[i]->Fit(fbg,"qrl","",statscale_lowlim,statscale_uplim);
}

delete dummy;

TCanvas* C1 = new TCanvas("C1","Combinatorial BG (ALL p_{T})",100,100,600,600);
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

TCanvas* C1sc = new TCanvas("C1sc","SCALED Combinatorial BG (ALL p_{T})",100,100,600,600);
C1sc->SetLogy(); 
  hhcombbg_scaled[nbins]->SetAxisRange(7.,14.);
  hhcombbg_scaled[nbins]->Draw("esame");

TCanvas* c00 = new TCanvas("c00","Combinatorial BG vs. p_{T}",150,150,1200,900);
c00->Divide(4,3);

for(int i=0; i<nbins; i++) {
if(i==0)  {c00->cd(1);}
if(i==1)  {c00->cd(2);}
if(i==2)  {c00->cd(3);}
if(i==3)  {c00->cd(4);}
if(i==4)  {c00->cd(5);}
if(i==5)  {c00->cd(6);}
if(i==6)  {c00->cd(7);}
if(i==7)  {c00->cd(8);}
if(i==8)  {c00->cd(9);}
if(i==9)  {c00->cd(10);}
if(i==10) {c00->cd(11);}
if(i>=11) {c00->cd(12);}
hhcombbg[i]->SetAxisRange(7.0,14.0); hhcombbg[i]->Draw();
  sprintf(tlchar,"%d-%d GeV",i,i+1);   tl[i] = new TLatex(8.0,hhcombbg[i]->GetMaximum()*0.95,tlchar); tl[i]->Draw();
}

TCanvas* c00scaled = new TCanvas("c00scaled","SCALED Combinatorial BG vs. p_{T}",150,150,1200,900);
c00scaled->Divide(4,3);

for(int i=0; i<nbins; i++) {
if(i==0)  {c00scaled->cd(1);}
if(i==1)  {c00scaled->cd(2);}
if(i==2)  {c00scaled->cd(3);}
if(i==3)  {c00scaled->cd(4);}
if(i==4)  {c00scaled->cd(5);}
if(i==5)  {c00scaled->cd(6);}
if(i==6)  {c00scaled->cd(7);}
if(i==7)  {c00scaled->cd(8);}
if(i==8)  {c00scaled->cd(9);}
if(i==9)  {c00scaled->cd(10);}
if(i==10) {c00scaled->cd(11);}
if(i>=11) {c00scaled->cd(12);}
hhcombbg_scaled[i]->SetAxisRange(statscale_lowlim,statscale_uplim); hhcombbg_scaled[i]->Draw();
  sprintf(tlchar,"%d-%d GeV",i,i+1);   tl[i] = new TLatex(8.0,hhcombbg_scaled[i]->GetMaximum()*0.95,tlchar); tl[i]->Draw();
}

//---------------------------------------------
//  Sibnal + BG
//---------------------------------------------

for(int i=0; i<nbins+1; i++) {
  sprintf(tmpname,"hhtotbg_scaled_%d",i);
  hhtotbg_scaled[i] = (TH1D*)hhcombbg_scaled[i]->Clone(tmpname);
  hhtotbg_scaled[i]->Add(hhcorrbg_scaled[i]);
}

for(int i=0; i<nbins+1; i++) {
sprintf(tmpname,"hhall_scaled_%d",i);
hhall_scaled[i] = (TH1D*)hhtotbg_scaled[i]->Clone(tmpname);
hhall_scaled[i]->Add(hhups[i]);
}

TCanvas* c000 = new TCanvas("c000","Signal + Background vs. p_{T}",200,200,1200,900);
c000->Divide(4,3);
for(int i=0; i<nbins; i++) {
if(i==0)  {c000->cd(1);}
if(i==1)  {c000->cd(2);}
if(i==2)  {c000->cd(3);}
if(i==3)  {c000->cd(4);}
if(i==4)  {c000->cd(5);}
if(i==5)  {c000->cd(6);}
if(i==6)  {c000->cd(7);}
if(i==7)  {c000->cd(8);}
if(i==8)  {c000->cd(9);}
if(i==9)  {c000->cd(10);}
if(i==10) {c000->cd(11);}
if(i==11) {c000->cd(12);}
hhall_scaled[i]->SetAxisRange(7.0,14.0); hhall_scaled[i]->SetMarkerStyle(1); hhall_scaled[i]->Draw("pehist");
  sprintf(tlchar,"%d-%d GeV",i,i+1);   tl[i] = new TLatex(8.0,hhall_scaled[i]->GetMaximum()*0.9,tlchar); tl[i]->Draw();
}

// Fit Signal + Correlated BG

// hhfit[] is signal + correlated bg
for(int i=0; i<nbins+1; i++) {
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

cout << "######### " << fTCBauau->GetParameter(0) << " " << fTCBauau->GetParameter(5) << " " << fTCBauau->GetParameter(6) << endl;
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


TCanvas* cfitall = new TCanvas("cfitall","FIT all pT",270,270,600,600);
  hhfit[nbins]->SetAxisRange(7.0,14.);
  hhfit[nbins]->GetXaxis()->CenterTitle();
  hhfit[nbins]->GetXaxis()->SetTitle("Mass(e^{+}e^{-}) [GeV/c^2]");
  hhfit[nbins]->GetXaxis()->SetTitleOffset(1.1);
  hhfit[nbins]->GetXaxis()->SetLabelSize(0.045);
  hhfit[nbins]->GetYaxis()->CenterTitle();
  hhfit[nbins]->GetYaxis()->SetLabelSize(0.045);
  hhfit[nbins]->GetYaxis()->SetTitle("Events / (50 MeV/c^{2})");
  hhfit[nbins]->GetYaxis()->SetTitleOffset(1.5);
  hhfit[nbins]->Draw("pehist");
  fSandBpp->Draw("same");
  fSandBauau->Draw("same");
    //hhcorrbg_scaled[nbins]->SetLineColor(kRed);
    //hhcorrbg_scaled[nbins]->Scale(0.82);
    //hhcorrbg_scaled[nbins]->Scale(0.70);
    //hhcorrbg_scaled[nbins]->Draw("histsame");

  TF1* fmycorrbg = new TF1("fmycorrbg","exp([0]+[1]*x)",7.,14.);
  fmycorrbg->SetParameters(tmppar0,tmppar1);
  fmycorrbg->SetLineStyle(2);
  fmycorrbg->SetLineColor(kRed);
  fmycorrbg->Draw("same");


double myheight = fTCBauau->GetParameter(0);
TLatex* ld1 = new TLatex(10.1,myheight,"sPHENIX Simulation");
ld1->SetTextSize(0.035);
ld1->Draw();
TLatex* ld2 = new TLatex(10.1,myheight-100.,"0-10% Au+Au #sqrt{s} = 200 GeV");
ld2->SetTextSize(0.035);
ld2->Draw();
TLatex* ld3;

 char evstr[500];
 sprintf(evstr,"%i billion events",auauevts);
 ld3 = new TLatex(10.1,myheight-200.,evstr);
ld3->SetTextSize(0.035);
ld3->Draw();

TCanvas* cfitall2 = new TCanvas("cfitall2","FIT all pT",270,270,600,600);
TH1D* hhfit_tmp = (TH1D*)hhfit[nbins]->Clone("hhfit_tmp");
hhfit_tmp->SetAxisRange(8.0,11.);
hhfit_tmp->Draw("pehist");
fSandBauau->Draw("same");
fmycorrbg->Draw("same");


//----------------------------------------------------------------------

// plot signal + both backgrounds for all pT

TCanvas* callpt = new TCanvas("callpt","Signal+BG (all p_{T})",300,300,600,600);

  hhall_scaled[nbins]->GetXaxis()->SetTitle("Invariant mass GeV/c");
  hhall_scaled[nbins]->SetLineColor(kBlack);
  hhall_scaled[nbins]->SetMarkerColor(kBlack);
  hhall_scaled[nbins]->SetMarkerStyle(20);
  hhall_scaled[nbins]->SetAxisRange(8.0,10.8);
  hhall_scaled[nbins]->Draw("pehist");
    hhcombbg_scaled[nbins]->SetLineColor(kBlue);
    hhcombbg_scaled[nbins]->Draw("histsame");
      hhcorrbg_scaled[nbins]->SetLineColor(kRed);
      hhcorrbg_scaled[nbins]->Draw("histsame");

//----------------------------------------------------------------
//  Calculate RAA
//----------------------------------------------------------------

double u1start = 9.25;
double u1stop  = 9.65;
double u2start = 9.80;
double u2stop  = 10.20;
double u3start = 10.20;
double u3stop  = 10.55;

 double raa1[nbins+1],raa2[nbins+1],raa3[nbins+1],erraa1[nbins+1],erraa2[nbins+1],erraa3[nbins+1], erraa3_up[nbins+1], erraa3_dn[nbins+1];
  for(int i=0; i<nbins; i++) { 
    //raa1[i] = supcor[0]; raa2[i] = supcor[1]; raa3[i] = supcor[2]; 
    raa1[i] = grRAA1S->Eval(0.5+i*1.0);
    raa2[i] = grRAA2S->Eval(0.5+i*1.0);
    raa3[i] = grRAA3S->Eval(0.5+i*1.0);
  }
  raa1[nbins] = raa1[0];
  raa2[nbins] = raa2[0];
  raa3[nbins] = raa3[0];

  int fbin1 = hhall_scaled[nbins]->FindBin(u1start + 0.001);
  int lbin1 = hhall_scaled[nbins]->FindBin(u1stop  - 0.001);
  int fbin2 = hhall_scaled[nbins]->FindBin(u2start + 0.001);
  int lbin2 = hhall_scaled[nbins]->FindBin(u2stop  - 0.001);
  int fbin3 = hhall_scaled[nbins]->FindBin(u3start + 0.001);
  int lbin3 = hhall_scaled[nbins]->FindBin(u3stop  - 0.001);
  cout << "Y(1S) bin range: " << fbin1 << " - " << lbin1 << endl;
  cout << "Y(1S) inv. mass range: " << u1start << " - " << u1stop << endl;
  cout << "Y(2S) bin range: " << fbin2 << " - " << lbin2 << endl;
  cout << "Y(2S) inv. mass range: " << u2start << " - " << u2stop << endl;
  cout << "Y(3S) bin range: " << fbin3 << " - " << lbin3 << endl;
  cout << "Y(3S) inv. mass range: " << u3start << " - " << u3stop << endl;

  double sum1[99]   = {0.};
  double truesum1[99]   = {0.};
  double ersum1[99] = {0.};
  double sumpp1[99]   = {0.};
  double ersumpp1[99] = {0.};
  double sum2[99]   = {0.};
  double truesum2[99]   = {0.};
  double ersum2[99] = {0.};
  double sumpp2[99]   = {0.};
  double ersumpp2[99] = {0.};
  double sum3[99]   = {0.};
  double truesum3[99]   = {0.};
  double allsum3[99]   = {0.};
  double ersum3[99] = {0.};
  double ersum3_up[99] = {0.};
  double ersum3_dn[99] = {0.};
  double sumpp3[99]   = {0.};
  double ersumpp3[99] = {0.};

  double sumsum1[99]   = {0.};
  double sumsum2[99]   = {0.};
  double sumsum3[99]   = {0.};
  double sumsum1pp[99]   = {0.};
  double sumsum2pp[99]   = {0.};
  double sumsum3pp[99]   = {0.};

  for(int i=0; i<nbins+1; i++) {

    double s1 = double(i); double s2 = double(i+1); if(i==nbins) {s1 = 0.;}

    for(int j=fbin1; j<=lbin1; j++) {
      sum1[i]   += (hhall_scaled[i]->GetBinContent(j) - hhcombbg_scaled[i]->GetFunction("fbg")->Eval(hhall_scaled[i]->GetBinCenter(j)) - hhcorrbg_scaled[i]->GetFunction("expo")->Eval(hhall_scaled[i]->GetBinCenter(j)));
      truesum1[i] += hhups1[i]->GetBinContent(j);
      ersum1[i] += hhall_scaled[i]->GetBinError(j)*hhall_scaled[i]->GetBinError(j);
      sumpp1[i]   += hhups1pp[i]->GetBinContent(j);
      ersumpp1[i] += hhupspp[i]->GetBinError(j)*hhupspp[i]->GetBinError(j);
    }
      sumsum1[i]     = truesum1[i];                   // direct count in mass range
      sumsum1pp[i]   = sumpp1[i];       

      //sumsum1[i]     = hhups1[i]->GetEntries();       // total number of upsilons in pT bin (rounded up)
      //sumsum1pp[i]   = hhups1pp[i]->GetEntries();
      //sumsum1[i]     = Nups1*fUpsilonPt->Integral(s1,s2)/upsnorm;  // total number of upsilons in pT bin
      //sumsum1pp[i]   = Nups1pp*fUpsilonPt->Integral(s1,s2)/upsnorm;

        if(sumsum1[i]>0. && sumsum1pp[i]>0.) {
          erraa1[i] = raa1[i]*sqrt(ersum1[i]/sumsum1[i]/sumsum1[i] + ersumpp1[i]/sumsum1pp[i]/sumsum1pp[i]);
	  cout << "i " << i << " raa1 " << raa1[i] << " erraa1  " << erraa1[i] << endl;
        } else {raa1[i]=-1.0; erraa1[i] = 999.; }

    for(int j=fbin2; j<=lbin2; j++) {
      sum2[i]   += (hhall_scaled[i]->GetBinContent(j) - hhcombbg_scaled[i]->GetFunction("fbg")->Eval(hhall_scaled[i]->GetBinCenter(j)) - hhcorrbg_scaled[i]->GetFunction("expo")->Eval(hhall_scaled[i]->GetBinCenter(j)));
      truesum2[i] += hhups2[i]->GetBinContent(j);
      ersum2[i] += hhall_scaled[i]->GetBinError(j)*hhall_scaled[i]->GetBinError(j);
      sumpp2[i]   += hhups2pp[i]->GetBinContent(j);
      ersumpp2[i] += hhupspp[i]->GetBinError(j)*hhupspp[i]->GetBinError(j);
    }
      sumsum2[i]     = truesum2[i];                   // direct count in mass range
      sumsum2pp[i]   = sumpp2[i];       

      //sumsum2[i]     = hhups2[i]->GetEntries();       // total number of upsilons in pT bin (rounded up)
      //sumsum2pp[i]   = hhups2pp[i]->GetEntries();
      //sumsum2[i]     = Nups2*fUpsilonPt->Integral(s1,s2)/upsnorm;  // total number of upsilons in pT bin
      //sumsum2pp[i]   = Nups2pp*fUpsilonPt->Integral(s1,s2)/upsnorm;
      
        if(sumsum2[i]>0. && sumsum2pp[i]>0.) {
          erraa2[i] = raa2[i]*sqrt(ersum2[i]/sumsum2[i]/sumsum2[i] + ersumpp2[i]/sumsum2pp[i]/sumsum2pp[i]);
	  cout << "i " << i << " raa2 " << raa2[i] << " erraa2  " << erraa2[i] << endl;
        } else {raa2[i]=-1.0; erraa2[i] = 999.; }

    for(int j=fbin3; j<=lbin3; j++) {
      sum3[i]   += (hhall_scaled[i]->GetBinContent(j) - hhcombbg_scaled[i]->GetFunction("fbg")->Eval(hhall_scaled[i]->GetBinCenter(j)) - hhcorrbg_scaled[i]->GetFunction("expo")->Eval(hhall_scaled[i]->GetBinCenter(j)));
      truesum3[i] += hhups3[i]->GetBinContent(j);
      ersum3[i] += hhall_scaled[i]->GetBinError(j)*hhall_scaled[i]->GetBinError(j);
      allsum3[i] +=  hhall_scaled[i]->GetBinContent(j);
      sumpp3[i]   += hhups3pp[i]->GetBinContent(j);
      ersumpp3[i] += hhupspp[i]->GetBinError(j)*hhupspp[i]->GetBinError(j);
    }
      sumsum3[i]     = truesum3[i];                   // direct count in mass range
      sumsum3pp[i]   = sumpp3[i];       
      // get Poisson errors on truesum3
      //double err3_up, err3_dn;
      //error_fg_bg_pair((int) allsum3[i], (int) (allsum3[i]-truesum3[i]), err3_up, err3_dn);
      //ersum3_up[i] = err3_up * err3_up;
      //ersum3_dn[i] = err3_dn * err3_dn;
      //sumsum3[i]     = hhups3[i]->GetEntries();       // total number of upsilons in pT bin (rounded up)
      //sumsum3pp[i]   = hhups3pp[i]->GetEntries();
      //sumsum3[i]     = Nups3*fUpsilonPt->Integral(s1,s2)/upsnorm;   // total number of upsilons in pT bin
      //sumsum3pp[i]   = Nups3pp*fUpsilonPt->Integral(s1,s2)/upsnorm;
      
        if(truesum3[i]>0. && sumpp3[i]>0.) {
	  erraa3[i] = raa3[i]*sqrt(ersum3[i]/sumsum3[i]/sumsum3[i] + ersumpp3[i]/sumsum3pp[i]/sumsum3pp[i]);
          //erraa3_up[i] = raa3[i]*sqrt(ersum3_up[i]/sumsum3[i]/sumsum3[i] + ersumpp3[i]/sumsum3pp[i]/sumsum3pp[i]);
          //erraa3_dn[i] = raa3[i]*sqrt(ersum3_dn[i]/sumsum3[i]/sumsum3[i] + ersumpp3[i]/sumsum3pp[i]/sumsum3pp[i]);
	  cout << "i " << i << " raa3 " << raa3[i] << " erraa3_up  " << erraa3_up[i] << " erraa3_dn " << erraa3_dn[i] << endl;
        } else {raa3[i]=-1.0; erraa3[i] = 999;}
  
  }

cout << "====== Y(1S):" << endl;
  for(int i=0; i<nbins+1; i++) {
    double s1 = double(i); double s2 = double(i+1); if(i==nbins) {s1 = 0.;}
    cout << "   " << i << " " << truesum1[i] << "(" << Nups1*fUpsilonPt->Integral(s1,s2)/upsnorm << ")" << " +- " 
         << sqrt(ersum1[i]) << " \t\t pp: " << sumpp1[i] << " +- " << sqrt(ersumpp1[i]) << " raa " << raa1[i] << " erraa " << erraa1[i] << endl;
  }
cout << "====== Y(2S):" << endl;
  for(int i=0; i<nbins+1; i++) {
    double s1 = double(i); double s2 = double(i+1); if(i==nbins) {s1 = 0.;}
    cout << "   " << i << " " << truesum2[i] << "(" << Nups2*fUpsilonPt->Integral(s1,s2)/upsnorm << ")" << " +- " 
         << sqrt(ersum2[i]) << " \t\t pp: " << sumpp2[i] << " +- " << sqrt(ersumpp2[i]) << " raa " << raa2[i] << " erraa " << erraa2[i] << endl;
  }
cout << "====== Y(3S):" << endl;
  for(int i=0; i<nbins+1; i++) {

    double s1 = double(i); double s2 = double(i+1); if(i==nbins) {s1 = 0.;}
    cout << "   " << i << " " << truesum3[i] << "(" << Nups3*fUpsilonPt->Integral(s1,s2)/upsnorm << ")" << " (all " << allsum3[i] << ") "<< " +- " 
      //	 << sqrt(ersum3_up[i]) << " - " << sqrt(ersum3_dn[i]) << " \t\t pp: " << sumpp3[i] << " +- " << sqrt(ersumpp3[i]) << " raa " << raa3[i] << " erraa " << erraa3[i] << endl;
      << sqrt(ersum3[i]) << " \t\t pp: " << sumpp3[i] << " +- " << sqrt(ersumpp3[i]) << " raa " << raa3[i] << " erraa " << erraa3[i] << endl;
  }

//-------------------------------------------------
//  Plot RAA vs pT
//-------------------------------------------------

int npts1 = 9;
 int npts2 = 7;
int npts3 = 7;

TCanvas* craa = new TCanvas("craa","R_{AA}",120,120,600,600);
TH2F* hh2 = new TH2F("hh2"," ",10,0.,float(npts1+1),10,0.,1.5);
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

double xx1[nbins+1]; for(int i=0; i<nbins+1; i++) {xx1[i] = 0.5 + double(i);}
double xx2[nbins+1]; for(int i=0; i<nbins+1; i++) {xx2[i] = 0.43 + double(i);}
double xx3[nbins+1]; for(int i=0; i<nbins+1; i++) {xx3[i] = 0.57 + double(i);}

TGraphErrors* gr1 = new TGraphErrors(npts1,xx1,raa1,0,erraa1);
gr1->SetMarkerStyle(20);
gr1->SetMarkerColor(kBlack);
gr1->SetLineColor(kBlack);
gr1->SetLineWidth(2);
gr1->SetMarkerSize(1.5);
gr1->SetName("gr1");
gr1->Draw("p");

erraa2[7] = erraa2[7]*0.85;

TGraphErrors* gr2 = new TGraphErrors(npts2,xx2,raa2,0,erraa2);
gr2->SetMarkerStyle(20);
gr2->SetMarkerColor(kRed);
gr2->SetLineColor(kRed);
gr2->SetLineWidth(2);
gr2->SetMarkerSize(1.5);
gr2->SetName("gr2");
gr2->Draw("p");

/*
//--- 3S state -------------------
 double dummy[9];
 for(int i=0; i<9; i++) {dummy[i]=-99.;}
 TGraphErrors* gr3 = new TGraphErrors(6,xx3,dummy,0,erraa3);
 gr3->SetMarkerStyle(20);
 gr3->SetMarkerColor(kBlue);
 gr3->SetLineColor(kBlue);
 gr3->SetLineWidth(2);
 gr3->SetMarkerSize(1.5);
 gr3->SetName("gr3");
// gr3->Draw("p");

TArrow* aa[9];
TLine* ll[9];
//erraa3[3] = erraa3[3]*1.5;
erraa3[4] = erraa3[4]*0.6;
//erraa3[4] = erraa3[4]*1.5;
erraa3[6] = erraa3[6]*1.2;

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
//--- end 3S state --------------
*/

TLegend *leg = new TLegend(0.14,0.75,0.34,0.86);
leg->SetFillColor(10);
leg->SetFillStyle(1001);
TLegendEntry *entry1=leg->AddEntry("gr1","Y(1S)","p");
TLegendEntry *entry2=leg->AddEntry("gr2","Y(2S)","p");
//TLegendEntry *entry3=leg->AddEntry("gr3","Y(3S)","l");
leg->Draw();

TLatex* l1 = new TLatex(3.35,1.33,"sPHENIX Simulation");
 l1->SetTextSize(0.042);
 l1->Draw();

TLatex* lum = new TLatex(3.35,1.23,"0-10% Au+Au #sqrt{s} = 200 GeV");
 lum->SetTextSize(0.042);
 lum->Draw();

 TLatex* run = new TLatex(3.35,1.13,run_plan[scenario].c_str());
 run->SetTextSize(0.042);
 run->Draw();

TLine* lll = new TLine(0.6,0.64,1.3,0.64);
lll->SetLineColor(kBlue);
lll->SetLineWidth(2);
//lll->Draw();

//==================================================================================

/*
TFile *fout = new TFile("test.root","recreate");
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
  hhcombbg_scaled[i]->Write();
}
fout->Write();
fout->Close();
*/

}

