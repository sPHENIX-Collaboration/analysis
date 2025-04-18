#include <cmath>

#include <uspin/SpinDBOutput.h>
#include <uspin/SpinDBInput.h>
#include "sPhenixStyle.C"

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
void smdprofileMaker(const std::string infile = "store34485/store34485-2.root", int storenumber = 34485, int runnumber = 42797)
{
  SetsPhenixStyle();
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

  TH2D* nxy_hits_south = new TH2D(Form("nxy_hits_all_south"),Form("nxy_hits_all_south"),48,-5.5,5.5,48, -5.92, 5.92);
  TH2D* nxy_hits_north = new TH2D(Form("nxy_hits_all_north"),Form("nxy_hits_all_north"),48,-5.5,5.5,48, -5.92, 5.92);
 
  TH2D* nxy_hits_south_cut = new TH2D(Form("nxy_hits_all_south_cut"),Form("nxy_hits_all_south_cut"),48,-5.5,5.5,48, -5.92, 5.92);
  TH2D* nxy_hits_north_cut = new TH2D(Form("nxy_hits_all_north_cut"),Form("nxy_hits_all_north_cut"),48,-5.5,5.5,48, -5.92, 5.92);
  TH2D* nxy_hits_south_cut_up = new TH2D(Form("nxy_hits_all_south_cut_up"),Form("nxy_hits_all_south_cut_up"),48,-5.5,5.5,48, -5.92, 5.92);
  TH2D* nxy_hits_north_cut_up = new TH2D(Form("nxy_hits_all_north_cut_up"),Form("nxy_hits_all_north_cut_up"),48,-5.5,5.5,48, -5.92, 5.92);
  TH2D* nxy_hits_south_up = new TH2D(Form("nxy_hits_all_south_up"),Form("nxy_hits_all_south_up"),48,-5.5,5.5,48, -5.92, 5.92);
  TH2D* nxy_hits_north_up = new TH2D(Form("nxy_hits_all_north_up"),Form("nxy_hits_all_north_up"),48,-5.5,5.5,48, -5.92, 5.92);
   TH2D* nxy_hits_south_cut_down = new TH2D(Form("nxy_hits_all_south_cut_down"),Form("nxy_hits_all_south_cut_down"),48,-5.5,5.5,48, -5.92, 5.92);
  TH2D* nxy_hits_north_cut_down = new TH2D(Form("nxy_hits_all_north_cut_down"),Form("nxy_hits_all_north_cut_down"),48,-5.5,5.5,48, -5.92, 5.92);
  TH2D* nxy_hits_south_down = new TH2D(Form("nxy_hits_all_south_down"),Form("nxy_hits_all_south_down"),48,-5.5,5.5,48, -5.92, 5.92);
  TH2D* nxy_hits_north_down = new TH2D(Form("nxy_hits_all_north_down"),Form("nxy_hits_all_north_down"),48,-5.5,5.5,48, -5.92, 5.92);
  
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
    bspinpat[i] *= -1;
    yspinpat[i] *= -1;
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
      nxy_hits_north->Fill(n_x, n_y);
      if (bspin == 1)
      {
        nxy_hits_north_up->Fill(n_x, n_y);
      }
      else if (bspin == -1)
      {
        nxy_hits_north_down->Fill(n_x, n_y);
      }

      if (sqrt(n_x * n_x + n_y * n_y) > RMINCUT && sqrt(n_x * n_x + n_y * n_y) < RMAXCUT)
      {
        nxy_hits_north_cut->Fill(n_x, n_y);
        if (bspin == 1)
        {
          nxy_hits_north_cut_up->Fill(n_x, n_y);
        }
        else if (bspin == -1)
        {
          nxy_hits_north_cut_down->Fill(n_x, n_y);
        }

        if (yspin == 1)
        {
        }
        else if (yspin == -1)
        {
        }
      }
    }

    if (zdcS1_adc > ZDC1CUT && zdcS2_adc > ZDC2CUT  && veto_SF < VETOCUT && veto_SB < VETOCUT && showerCutS == 1)
    {
      nxy_hits_south->Fill(s_x, s_y);
      if (yspin == 1)
      {
        nxy_hits_south_up->Fill(s_x, s_y);
      }
      else if (yspin == -1)
      {
        nxy_hits_south_down->Fill(s_x, s_y);
      }
      if (sqrt(s_x * s_x + s_y * s_y) > RMINCUT && sqrt(s_x * s_x + s_y * s_y) < RMAXCUT)
      {
        nxy_hits_south_cut->Fill(s_x, s_y);
        if (yspin == 1)
        {
          nxy_hits_south_cut_up->Fill(s_x, s_y);
        }
        else if (yspin == -1)
        {
          nxy_hits_south_cut_down->Fill(s_x, s_y);
        }
      }
    }
  }
  //=====================================================================================//

  
  

  TFile* ofile = new TFile("Profile.root","RECREATE");
  ofile->cd();
  nxy_hits_south -> Write();
  nxy_hits_south_cut -> Write();
  nxy_hits_north -> Write();
  nxy_hits_north_cut -> Write();
  nxy_hits_south_up -> Write();
  nxy_hits_south_cut_up -> Write();
  nxy_hits_north_up -> Write();
  nxy_hits_north_cut_up -> Write();
  nxy_hits_south_down -> Write();
  nxy_hits_south_cut_down -> Write();
  nxy_hits_north_down -> Write();
  nxy_hits_north_cut_down -> Write();



  ofile->Close();
}