#include <cmath>

#include <uspin/SpinDBOutput.h>
#include <uspin/SpinDBInput.h>

float sqrtasym(float N1, float N2, float N3, float N4);
float sqrtasymerr(float N1, float N2, float N3, float N4, float E1, float E2, float E3, float E4);
void FitAsym(TGraphErrors *gAsym, double &eps, double &epserr, double &chi2);
void DrawAsym(int runnumber, TGraphErrors *gSqrtAsymBlue, TGraphErrors *gSqrtAsymYellow);
void DrawCanvas(int runnumber, TH2 *hits_up, TH2 *hits_down, TGraphErrors *gRawSimpleAsym, TGraphErrors *gRawSqrtAsym);


R__LOAD_LIBRARY(libuspin.so)

float ZDC1CUT = 100; // keep above (nominal 65)
float ZDC2CUT = 15; // keep above (nominal 25)
float VETOCUT = 150; // keep below (nominal 150)
float RMINCUT = 2; // keep above (nominal 2)
float RMAXCUT = 4; // keep below (nominal 4)


//void drawAsym(const std::string infile = "fakeAsymmetry/FakeAsymmetry.root", int storenumber = 34485, int runnumber = 42797)
//void drawAsym(const std::string infile = "store34485/42797/smdmerge.root", int storenumber = 34485, int runnumber = 42797)
void drawAsym(const std::string infile = "store34485/smdmerge.root", int storenumber = 34485, int runnumber = 42797)
//void drawAsym(const std::string infile = "store34492/42836/smdmerge_old.root", int storenumber = 34492, int runnumber = 42836)
{

  TFile *f = new TFile(infile.c_str());

  //========================== Hists/Graphs ==============================//
  TString beam[2] = {"Blue", "Yellow"};

  //== 1: Blue North, 2: Yellow North, 3: Blue South, 4: Yellow South
  TGraphErrors *gSqrtAsym[4];
  
  TH2D *nxy_hits_up[2];
  TH2D *nxy_hits_down[2];
  TH2D *sxy_hits_up[2];
  TH2D *sxy_hits_down[2];

  TH1D *Nup[2];
  TH1D *Ndown[2];
  TH1D *Sup[2];
  TH1D *Sdown[2];
  
  int nasymbins = 16; // defined over entire [-pi,pi]: For sqrt asym there are nasymbins/2 total bins so this must be divisible by 2

  for (int i = 0; i < 2; i++)
  {
    gSqrtAsym[i%2] = new TGraphErrors();
    gSqrtAsym[i%2]->SetTitle(Form("Square root asymmetry %s North",beam[i].Data()));
    gSqrtAsym[i%2+2] = new TGraphErrors();
    gSqrtAsym[i%2+2]->SetTitle(Form("Square root asymmetry %s South",beam[i].Data()));

    nxy_hits_up[i] = new TH2D(Form("nxy_hits_up_%s",beam[i].Data()),Form("nxy_hits_up_%s",beam[i].Data()),48,-5.5,5.5,48, -5.92, 5.92);
    nxy_hits_down[i] = new TH2D(Form("nxy_hits_down_%s",beam[i].Data()),Form("nxy_hits_down_%s",beam[i].Data()),48,-5.5,5.5,48, -5.92, 5.92);
    sxy_hits_up[i] = new TH2D(Form("sxy_hits_up_%s",beam[i].Data()),Form("sxy_hits_up_%s",beam[i].Data()),48,-5.5,5.5,48, -5.92, 5.92);
    sxy_hits_down[i] = new TH2D(Form("sxy_hits_down_%s",beam[i].Data()),Form("sxy_hits_down_%s",beam[i].Data()),48,-5.5,5.5,48, -5.92, 5.92);

    Nup[i] = new TH1D(Form("Nup_%s",beam[i].Data()),Form("Nup_%s",beam[i].Data()),nasymbins,-TMath::Pi(),TMath::Pi()); Nup[i]->Sumw2();
    Ndown[i] = new TH1D(Form("Ndown_%s",beam[i].Data()),Form("Ndown_%s",beam[i].Data()),nasymbins,-TMath::Pi(),TMath::Pi()); Ndown[i]->Sumw2();
    Sup[i] = new TH1D(Form("Sup_%s",beam[i].Data()),Form("Sup_%s",beam[i].Data()),nasymbins,-TMath::Pi(),TMath::Pi()); Sup[i]->Sumw2();
    Sdown[i] = new TH1D(Form("Sdown_%s",beam[i].Data()),Form("Sdown_%s",beam[i].Data()),nasymbins,-TMath::Pi(),TMath::Pi()); Sdown[i]->Sumw2();

  }
  //======================================================================//

  //========================== SMD Hit Tree ==============================//
  TTree *smdHits = (TTree*)f->Get("smdHits");
  int bunchnumber, showerCutN, showerCutS;
  float n_x, n_y, s_x, s_y;
  float zdcN1_adc, zdcN2_adc, veto_NF, veto_NB;
  float zdcS1_adc, zdcS2_adc, veto_SF, veto_SB;
  smdHits -> SetBranchAddress ("bunchnumber",       &bunchnumber);
  smdHits -> SetBranchAddress ("showerCutN",       &showerCutN);
  smdHits -> SetBranchAddress ("showerCutS",       &showerCutS);
  smdHits -> SetBranchAddress ("n_x",       &n_x);
  smdHits -> SetBranchAddress ("n_y",       &n_y);
  smdHits -> SetBranchAddress ("s_x",       &s_x);
  smdHits -> SetBranchAddress ("s_y",       &s_y);
  smdHits -> SetBranchAddress ("zdcN1_adc",       &zdcN1_adc);
  smdHits -> SetBranchAddress ("zdcN2_adc",       &zdcN2_adc);
  smdHits -> SetBranchAddress ("zdcS1_adc",       &zdcS1_adc);
  smdHits -> SetBranchAddress ("zdcS2_adc",       &zdcS2_adc);
  smdHits -> SetBranchAddress ("veto_NF",       &veto_NF);
  smdHits -> SetBranchAddress ("veto_NB",       &veto_NB);
  smdHits -> SetBranchAddress ("veto_SF",       &veto_SF);
  smdHits -> SetBranchAddress ("veto_SB",       &veto_SB);
  int nentries = smdHits->GetEntries();
  //======================================================================//
  
  //============================= Option A: Spin DB ================================//
  unsigned int qa_level = 0xffff;
  SpinDBOutput spin_out("phnxrc");
  SpinDBContent spin_cont;
  spin_out.StoreDBContent(runnumber,runnumber,qa_level);
  spin_out.GetDBContentStore(spin_cont,runnumber);
  
  int crossingshift = spin_cont.GetCrossingShift();
  //crossingshift = 0;
  //crossingshift = ixs;
  //if (ixs < 0){crossingshift = 120+ixs;}
  std::cout << crossingshift << std::endl;
  
  int bspinpat[120] = {0};
  int yspinpat[120] = {0};
  for (int i = 0; i < 120; i++)
  {
    bspinpat[i] = spin_cont.GetSpinPatternBlue(i);
    yspinpat[i] = spin_cont.GetSpinPatternYellow(i);

  }
  //======================================================================//

  
  
  //============================= Option B: Get spin pattern manually ================================//
  int bpat[120] = {0};
  int ypat[120] = {0};
  std::string preset_pattern_blue[8];
  std::string preset_pattern_yellow[8];
  preset_pattern_blue[0] = "+-+--+-+-+-++-+--+-++-+-+-+--+-+-+-++-+--+-++-+-+-+--+-+-+-++-+--+-++-+-+-+--+-+-+-++-+--+-++-+-+-+--+-+-+-++-+*********";
  preset_pattern_blue[1] = "-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-*********";
  preset_pattern_blue[2] = "+-+--+-+-+-++-+--+-++-+-+-+--+-+-+-++-+--+-++-+-+-+--+-+-+-++-+--+-++-+-+-+--+-+-+-++-+--+-++-+-+-+--+-+-+-++-+*********";
  preset_pattern_blue[3] = "-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-*********";
  preset_pattern_blue[4] = "++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++-*********";
  preset_pattern_blue[5] = "--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--+*********";
  preset_pattern_blue[6] = "++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++-*********";
  preset_pattern_blue[7] = "--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--+*********";

  
  preset_pattern_yellow[0] = "++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++-*********";
  preset_pattern_yellow[1] = "++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++-*********";
  preset_pattern_yellow[2] = "--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--+*********";
  preset_pattern_yellow[3] = "--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--++--+*********";
  preset_pattern_yellow[4] = "+-+--+-+-+-++-+--+-++-+-+-+--+-+-+-++-+--+-++-+-+-+--+-+-+-++-+--+-++-+-+-+--+-+-+-++-+--+-++-+-+-+--+-+-+-++-+*********";
  preset_pattern_yellow[5] = "+-+--+-+-+-++-+--+-++-+-+-+--+-+-+-++-+--+-++-+-+-+--+-+-+-++-+--+-++-+-+-+--+-+-+-++-+--+-++-+-+-+--+-+-+-++-+*********";
  preset_pattern_yellow[6] = "-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-*********";
  preset_pattern_yellow[7] = "-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-++-+--+-+-+-++-+-+-+--+-*********";

  int spinpatternNo = 3;
  if (storenumber == 34485){spinpatternNo = 3;} //pattern names start at '111x111_P1' so index 3  means '111x111_P4' and so on
  
  for (int i = 0; i < 120; i++)
  {
    if (preset_pattern_blue[spinpatternNo].at(i) == '+') {bpat[i] = 1;}
    else if (preset_pattern_blue[spinpatternNo].at(i) == '-'){bpat[i] = -1;}
    else if (preset_pattern_blue[spinpatternNo].at(i) == '*'){bpat[i] = 10;}

    if (preset_pattern_yellow[spinpatternNo].at(i) == '+'){ypat[i] = 1;}
    else if (preset_pattern_yellow[spinpatternNo].at(i) == '-'){ypat[i] = -1;}
    else if (preset_pattern_yellow[spinpatternNo].at(i) == '*'){ypat[i] = 10;}

  }
  //=====================================================================================//

  //======================= process event ==================================//
  std::cout << nentries << std::endl;
  
  int clockOffset = 1; //this is the offset between GL1 and ZDC events

  for (int i = 0; i < nentries - clockOffset; i++)
  {
    if (i % 1000000 == 0){std::cout << "Entry: " << i << std::endl;}
    smdHits->GetEntry(i+clockOffset);
   
    int sphenix_cross = (bunchnumber + crossingshift) % 120;

    
    smdHits->GetEntry(i);

    int bspin = bspinpat[sphenix_cross]; //option A: spinDB
    int yspin = yspinpat[sphenix_cross]; //option A: spinDB
    //int bspin = bpat[sphenix_cross]; //option B: preset patterns
    //int yspin = ypat[sphenix_cross]; //option B: preset patterns

    /*
    float ny_offset = 0.85; float nx_offset = 0.275;
    float sy_offset = 0.0; float sx_offset = 0.0;

    n_y = n_y - ny_offset;
    n_x = n_x - nx_offset;
    s_y = s_y - sy_offset;
    s_x = s_x - sx_offset;
    */

    // phi = -pi/2 to the left of polarized proton going direction (PHENIX convention - https://journals.aps.org/prd/pdf/10.1103/PhysRevD.88.032006)
    // SMD goes from -x to +x from left to right when looking at detector (from perspective of Blue-North, Yellow-South)
    float n_phi = atan2(n_y, -n_x) - TMath::Pi() / 2;
    if (n_phi < -TMath::Pi()) {n_phi += 2 * TMath::Pi();} 
    else if (n_phi > TMath::Pi()) {n_phi -= 2 * TMath::Pi();}

    float s_phi = atan2(s_y, -s_x) - TMath::Pi() / 2;
    if (s_phi < -TMath::Pi()) {s_phi += 2 * TMath::Pi();} 
    else if (s_phi > TMath::Pi()) {s_phi -= 2 * TMath::Pi();}
    

    if (zdcN1_adc > ZDC1CUT && zdcN2_adc > ZDC2CUT && veto_NF < VETOCUT && veto_NB < VETOCUT && showerCutN == 1)
    {
      if (sqrt(n_x*n_x + n_y*n_y) > RMINCUT && sqrt(n_x*n_x+n_y*n_y) < RMAXCUT)
      {
	if (bspin == 1)
	{
	  nxy_hits_up[0]->Fill(n_x,n_y);
	  Nup[0]->Fill(n_phi);
	}
	else if (bspin == -1)
	{
	  nxy_hits_down[0]->Fill(n_x,n_y);
	  Ndown[0]->Fill(n_phi);
	}
        
	if (yspin == 1)
	{
	  nxy_hits_up[1]->Fill(n_x,n_y);
	  Nup[1]->Fill(n_phi);
	}
	else if (yspin == -1)
	{
	  nxy_hits_down[1]->Fill(n_x,n_y);
	  Ndown[1]->Fill(n_phi);
	}

      }
    }


    if (zdcS1_adc > ZDC1CUT && zdcS2_adc > ZDC2CUT  && veto_SF < VETOCUT && veto_SB < VETOCUT && showerCutS == 1)
    {
      if (sqrt(s_x*s_x + s_y*s_y) > RMINCUT && sqrt(s_x*s_x+s_y*s_y) < RMAXCUT)
      {
        if (bspin == 1)
	{
	  sxy_hits_up[0]->Fill(s_x,s_y);
	  Sup[0]->Fill(s_phi);
	}
	else if (bspin == -1)
	{
	  sxy_hits_down[0]->Fill(s_x,s_y);
	  Sdown[0]->Fill(s_phi);
	}

	if (yspin == 1)
	{
	  sxy_hits_up[1]->Fill(s_x,s_y);
	  Sup[1]->Fill(s_phi);
	}
	else if (yspin == -1)
	{
	  sxy_hits_down[1]->Fill(s_x,s_y);
	  Sdown[1]->Fill(s_phi);
	}
      }
    }
  }
  //=====================================================================================//

  
  

  // get sqrt asymmetry
  for (int i = 0; i < nasymbins; i++)
  {
    float phi = i*(2*TMath::Pi()/nasymbins) - (TMath::Pi() - TMath::Pi()/nasymbins);

    int phibin = i; // N_Left
    int phibin2 = (phibin + (int)(nasymbins/2.)) % nasymbins; // N_Right
    

    //== 1: Blue North, 2: Yellow North, 3: Blue South, 4: Yellow South
    float sqrtrawasym[4] = {0};
    float sqrtrawasymerr[4] = {0};

    for (int j = 0; j < 2; j++)
    {

      sqrtrawasym[j%2] = sqrtasym(Nup[j%2]->GetBinContent(phibin+1),Nup[j%2]->GetBinContent(phibin2+1),
				  Ndown[j%2]->GetBinContent(phibin+1),Ndown[j%2]->GetBinContent(phibin2+1));

      sqrtrawasymerr[j%2] = sqrtasymerr(Nup[j%2]->GetBinContent(phibin+1),Nup[j%2]->GetBinContent(phibin2+1),
					Ndown[j%2]->GetBinContent(phibin+1),Ndown[j%2]->GetBinContent(phibin2+1),
					Nup[j%2]->GetBinError(phibin+1),Nup[j%2]->GetBinError(phibin2+1),
					Ndown[j%2]->GetBinError(phibin+1),Ndown[j%2]->GetBinError(phibin2+1));

      sqrtrawasym[j%2+2] = sqrtasym(Sup[j%2]->GetBinContent(phibin+1),Sup[j%2]->GetBinContent(phibin2+1),
				    Sdown[j%2]->GetBinContent(phibin+1),Sdown[j%2]->GetBinContent(phibin2+1));

      sqrtrawasymerr[j%2+2] = sqrtasymerr(Sup[j%2]->GetBinContent(phibin+1),Sup[j%2]->GetBinContent(phibin2+1),
					  Sdown[j%2]->GetBinContent(phibin+1),Sdown[j%2]->GetBinContent(phibin2+1),
					  Sup[j%2]->GetBinError(phibin+1),Sup[j%2]->GetBinError(phibin2+1),
					  Sdown[j%2]->GetBinError(phibin+1),Sdown[j%2]->GetBinError(phibin2+1));	
      
      gSqrtAsym[j%2]->SetPoint(i,phi,sqrtrawasym[j%2]);
      gSqrtAsym[j%2]->SetPointError(i,0,sqrtrawasymerr[j%2]);

      gSqrtAsym[j%2+2]->SetPoint(i,phi,sqrtrawasym[j%2+2]);
      gSqrtAsym[j%2+2]->SetPointError(i,0,sqrtrawasymerr[j%2+2]);
    }

  }


  
  TCanvas *ncall = new TCanvas("ncall","ncall",1200,600);
  //DrawCanvas(runnumber,nxy_hits_up[0],nxy_hits_down[0],gSqrtAsym[0],gSqrtAsym[1]);
  DrawAsym(runnumber,gSqrtAsym[0],gSqrtAsym[1]);

  TCanvas *scall = new TCanvas("scall","scall",1200,600);
  //DrawCanvas(runnumber,sxy_hits_up[1],sxy_hits_down[1],gSqrtAsym[2],gSqrtAsym[3]);
  DrawAsym(runnumber,gSqrtAsym[2],gSqrtAsym[3]);


}



float sqrtasym(float N1, float N2, float N3, float N4){
  float asym = (sqrt(N1*N4)-sqrt(N3*N2))/(sqrt(N1*N4)+sqrt(N3*N2));
  return asym;
}



float sqrtasymerr(float N1, float N2, float N3, float N4, float E1, float E2, float E3, float E4){
  float D1 = sqrt(N2*N3*N4/N1)*E1;
  float D2 = sqrt(N1*N3*N4/N2)*E2;
  float D3 = sqrt(N1*N2*N4/N3)*E3;
  float D4 = sqrt(N1*N2*N3/N4)*E4;
  float asym_err = (1/pow(sqrt(N1*N4)+sqrt(N3*N2),2))*sqrt(pow(D1,2)+pow(D2,2)+pow(D3,2)+pow(D4,2));
  return asym_err;
}


void FitAsym(TGraphErrors *gAsym, double &eps, double &epserr, double &chi2)
{
  double fitLow = -TMath::Pi()/2;
  double fitHigh = TMath::Pi()/2;

  TF1 *sin1 = new TF1("sin1","-[0] * TMath::Sin(x - [1])",fitLow,fitHigh);
  sin1->SetParLimits(0,0,0.2);
  sin1->SetParLimits(1,0,0.05);

  gAsym->GetYaxis()->SetRangeUser(-0.05,0.05);
  gAsym->GetXaxis()->SetRangeUser(fitLow,fitHigh);
  gAsym->Fit("sin1","MR");
  eps = sin1->GetParameter(0);
  epserr = sin1->GetParError(0);
  chi2 = sin1->GetChisquare();
}


void DrawAsym(int runnumber, TGraphErrors *gSqrtAsymBlue, TGraphErrors *gSqrtAsymYellow)
{
  TPad *pad1 = new TPad("pad1", "Pad1", 0.0, 0.0, 0.5, 1.0);
  pad1->SetRightMargin(0.1);
  pad1->SetTopMargin(0.1); 
  pad1->SetLeftMargin(0.15); 
  pad1->SetBottomMargin(0.15); 
  TPad *pad2 = new TPad("pad2", "Pad2", 0.5, 0.0, 1.0, 1.0);
  pad2->SetRightMargin(0.1);
  pad2->SetTopMargin(0.1); 
  pad2->SetLeftMargin(0.15); 
  pad2->SetBottomMargin(0.15);

  pad1->Draw();
  pad2->Draw();

  // only fit in a length pi interval
  double fitLow = -TMath::Pi()/2;
  double fitHigh = TMath::Pi()/2;


  pad1->cd();

  TF1 *sin1 = new TF1("sin1","-[0] * TMath::Sin(x - [1])",fitLow,fitHigh);
  sin1->SetParLimits(0,0,0.2);
  //sin1->SetParLimits(1,0,0.2);

  gSqrtAsymBlue->GetXaxis()->SetTitle("#phi, rad");
  gSqrtAsymBlue->GetXaxis()->SetTitleSize(0.045);
  gSqrtAsymBlue->GetXaxis()->SetTitleOffset(0.85);
  gSqrtAsymBlue->GetYaxis()->SetTitle("#epsilon(#phi)");
  gSqrtAsymBlue->GetYaxis()->SetTitleSize(0.045);
  gSqrtAsymBlue->GetYaxis()->SetTitleOffset(1.35);
  gSqrtAsymBlue->GetYaxis()->SetRangeUser(-0.05,0.05);
  gSqrtAsymBlue->GetXaxis()->SetRangeUser(fitLow,fitHigh);
  gSqrtAsymBlue->SetMarkerStyle(21);
  gSqrtAsymBlue->Fit("sin1","MR");
  gSqrtAsymBlue->Draw("ap");
  
  TLine *line1 = new TLine(fitLow, 0, fitHigh, 0);
  line1->SetLineStyle(2);
  line1->SetLineWidth(2);
  line1->SetLineColor(kBlack);
  line1->Draw();

  TLatex *latex = new TLatex();
  latex->SetTextSize(0.04);
  latex->SetTextFont(42);
  latex->DrawLatexNDC(0.6, 0.8, "#bf{sPHENIX} internal");
  latex->DrawLatexNDC(0.6, 0.75, Form("Run %d", runnumber));

  TLatex *latexFit = new TLatex();
  latexFit->SetTextSize(0.03);
  latexFit->SetTextAlign(13);

  
  latexFit->DrawLatexNDC(0.2, 0.875, "|#epsilon_{raw}|sin(#phi - #phi_{0}) fit:");
  latexFit->DrawLatexNDC(0.2, 0.825, Form("|#epsilon_{raw}| = %.2f%% #pm %.2f%%", sin1->GetParameter(0)*100, sin1->GetParError(0)*100));
  latexFit->DrawLatexNDC(0.2, 0.775, Form("#phi_{0} + #pi = %.2f #pm %.2f", sin1->GetParameter(1), sin1->GetParError(1)));
  latexFit->DrawLatexNDC(0.2, 0.715, Form("#chi^{2}/NDF = %.2f/%d", sin1->GetChisquare(), sin1->GetNDF()));


  pad2->cd();
  TF1 *sin2 = new TF1("sin2","-[0] * TMath::Sin(x - [1])",fitLow,fitHigh);
  sin2->SetParLimits(0,0,0.2);
  //sin2->SetParLimits(1,0,0.2);

  gSqrtAsymYellow->GetXaxis()->SetTitle("#phi, rad");
  gSqrtAsymYellow->GetXaxis()->SetTitleSize(0.045);
  gSqrtAsymYellow->GetXaxis()->SetTitleOffset(0.85);
  gSqrtAsymYellow->GetYaxis()->SetTitle("#epsilon(#phi)");
  gSqrtAsymYellow->GetYaxis()->SetTitleSize(0.045);
  gSqrtAsymYellow->GetYaxis()->SetTitleOffset(1.35);
  gSqrtAsymYellow->GetYaxis()->SetRangeUser(-0.05,0.05);
  gSqrtAsymYellow->GetXaxis()->SetRangeUser(fitLow,fitHigh);
  gSqrtAsymYellow->SetMarkerStyle(21);
  gSqrtAsymYellow->Fit("sin2","MR");
  gSqrtAsymYellow->Draw("ap");
  
  
  TLine *line2 = new TLine(fitLow, 0, fitHigh, 0);
  line2->SetLineStyle(2);
  line2->SetLineWidth(2);
  line2->SetLineColor(kBlack);
  line2->Draw();


  TLatex *latexFit2 = new TLatex();
  latexFit2->SetTextSize(0.03);
  latexFit2->SetTextAlign(13);

  
  latexFit2->DrawLatexNDC(0.2, 0.875, "|#epsilon_{raw}|sin(#phi - #phi_{0}) fit:");
  latexFit2->DrawLatexNDC(0.2, 0.825, Form("|#epsilon_{raw}| = %.2f%% #pm %.2f%%", sin2->GetParameter(0)*100, sin2->GetParError(0)*100));
  latexFit2->DrawLatexNDC(0.2, 0.775, Form("#phi_{0} + #pi = %.2f #pm %.2f", sin2->GetParameter(1), sin2->GetParError(1)));
  latexFit2->DrawLatexNDC(0.2, 0.715, Form("#chi^{2}/NDF = %.2f/%d", sin2->GetChisquare(), sin2->GetNDF()));


}


void DrawCanvas(int runnumber, TH2 *hits_up, TH2 *hits_down, TGraphErrors *gSqrtAsymBlue, TGraphErrors *gSqrtAsymYellow)
{

// Create custom pads
  TPad *pad1 = new TPad("pad1", "Pad1", 0.0, 0.5, 0.5, 1.0);
  TPad *pad2 = new TPad("pad2", "Pad2", 0.5, 0.5, 1.0, 1.0);
  TPad *pad3 = new TPad("pad3", "Pad3", 0.0, 0.0, 0.5, 0.5); 
  TPad *pad4 = new TPad("pad4", "Pad4", 0.5, 0.0, 1.0, 0.5);

  // Draw pads on the canvas
  pad1->Draw();
  pad2->Draw();
  pad3->Draw();
  pad4->Draw();

  

  pad1->cd();
  hits_up->SetTitle("p^{#uparrow} + p");
  hits_up->GetXaxis()->SetTitle("x (cm)");
  hits_up->GetYaxis()->SetTitle("y (cm)");
  hits_up->Draw("colz");
  pad2->cd();
  hits_down->SetTitle("p^{#downarrow} + p");
  hits_down->Draw("colz");
  hits_down->GetXaxis()->SetTitle("x (cm)");
  hits_down->GetYaxis()->SetTitle("y (cm)");
  

  // only fit in a length pi interval
  double fitLow = -TMath::Pi()/2;
  double fitHigh = TMath::Pi()/2;

  pad3->cd();
  TF1 *sin1 = new TF1("sin1","-[0] * TMath::Sin(x - [1])",fitLow,fitHigh);
  sin1->SetParLimits(0,0,0.1);

  gSqrtAsymBlue->GetXaxis()->SetTitle("#phi, rad");
  gSqrtAsymBlue->GetXaxis()->SetTitleSize(0.045);
  gSqrtAsymBlue->GetXaxis()->SetTitleOffset(0.85);
  gSqrtAsymBlue->GetYaxis()->SetTitle("#epsilon(#phi)");
  gSqrtAsymBlue->GetYaxis()->SetTitleSize(0.045);
  gSqrtAsymBlue->GetYaxis()->SetTitleOffset(0.85);
  gSqrtAsymBlue->GetYaxis()->SetRangeUser(-0.05,0.05);
  gSqrtAsymBlue->GetXaxis()->SetRangeUser(fitLow,fitHigh);
  gSqrtAsymBlue->SetMarkerStyle(21);
  gSqrtAsymBlue->Fit("sin1","MR");
  gSqrtAsymBlue->Draw("ap");
  
  TLine *line = new TLine(fitLow, 0, fitHigh, 0); 
  line->SetLineStyle(2);
  line->SetLineWidth(2);
  line->SetLineColor(kBlack);
  line->Draw();
  line->Draw();

  TLatex *latex = new TLatex();
  latex->SetTextSize(0.04);
  latex->SetTextFont(42);
  latex->DrawLatexNDC(0.65, 0.8, "#bf{sPHENIX} internal");
  latex->DrawLatexNDC(0.65, 0.75, Form("Run %d", runnumber));

  TLatex *latexFit = new TLatex();
  latexFit->SetTextSize(0.04);
  latexFit->SetTextAlign(13); 

  
  latexFit->DrawLatexNDC(0.2, 0.85, "|#epsilon_{raw}|sin(#phi - #phi_{0}) fit:");
  latexFit->DrawLatexNDC(0.2, 0.8, Form("|#epsilon_{raw}| = %.2f%% #pm %.2f%%", sin1->GetParameter(0)*100, sin1->GetParError(0)*100));
  latexFit->DrawLatexNDC(0.2, 0.75, Form("#phi_{0} + #pi = %.2f #pm %.2f", sin1->GetParameter(1), sin1->GetParError(1)));
  latexFit->DrawLatexNDC(0.2, 0.7, Form("#chi^{2}/NDF = %.2f/%d", sin1->GetChisquare(), sin1->GetNDF()));


  pad4->cd();

  TF1 *sin2 = new TF1("sin2","-[0] * TMath::Sin(x - [1])",fitLow,fitHigh);
  sin2->SetParLimits(0,0,0.1);


  
  gSqrtAsymYellow->GetXaxis()->SetTitle("#phi, rad");
  gSqrtAsymYellow->GetXaxis()->SetTitleSize(0.045);
  gSqrtAsymYellow->GetXaxis()->SetTitleOffset(0.85);
  gSqrtAsymYellow->GetYaxis()->SetTitle("#epsilon(#phi)");
  gSqrtAsymYellow->GetYaxis()->SetTitleSize(0.045);
  gSqrtAsymYellow->GetYaxis()->SetTitleOffset(0.85);
  gSqrtAsymYellow->GetYaxis()->SetRangeUser(-0.05,0.05);
  gSqrtAsymYellow->GetXaxis()->SetRangeUser(fitLow,fitHigh);
  gSqrtAsymYellow->SetMarkerStyle(21);
  gSqrtAsymYellow->Fit("sin2","MR");
  gSqrtAsymYellow->Draw("ap");
  
  
  line->SetLineStyle(2);
  line->SetLineWidth(2);
  line->SetLineColor(kBlack);
  line->Draw();
  line->Draw();


  TLatex *latexFit2 = new TLatex();
  latexFit2->SetTextSize(0.04);
  latexFit2->SetTextAlign(13);  

  // Draw each line
  latexFit2->DrawLatexNDC(0.2, 0.85, "|#epsilon_{raw}|sin(#phi - #phi_{0}) fit:");
  latexFit2->DrawLatexNDC(0.2, 0.8, Form("|#epsilon_{raw}| = %.2f%% #pm %.2f%%", sin2->GetParameter(0)*100, sin2->GetParError(0)*100));
  latexFit2->DrawLatexNDC(0.2, 0.75, Form("#phi_{0} + #pi = %.2f #pm %.2f", sin2->GetParameter(1), sin2->GetParError(1)));
  latexFit2->DrawLatexNDC(0.2, 0.7, Form("#chi^{2}/NDF = %.2f/%d", sin2->GetChisquare(), sin2->GetNDF()));



}
