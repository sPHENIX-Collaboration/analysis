
#include <uspin/SpinDBInput.h>
#include <uspin/SpinDBOutput.h>
#include "sPhenixStyle.C"
R__LOAD_LIBRARY(libuspin.so)

float ZDC1CUT = 100;  // keep above (nominal 65)
float ZDC2CUT = 15;   // keep above (nominal 25)
float VETOCUT = 150;  // keep below (nominal 150)
bool ismerged = true;
bool isflipON = true;
void drawBunchBunch(const std::string infile = "store34485/store34485-2.root", int storenumber = 34485, int runnumber = 42797)
{
  SetsPhenixStyle();
  TFile *f = new TFile(infile.c_str());
  //========================== Hists/Graphs ==============================//

  TH1D *n_smd_L = new TH1D("n_smd_L", "n_smd_L", 128, 0, 128);
  TH1D *n_smd_R = new TH1D("n_smd_R", "n_smd_R", 128, 0, 128);
  TH1D *s_smd_L = new TH1D("s_smd_L", "s_smd_L", 128, 0, 128);
  TH1D *s_smd_R = new TH1D("s_smd_R", "s_smd_R", 128, 0, 128);

  TH1D *n_smd_U = new TH1D("n_smd_U", "n_smd_U", 128, 0, 128);
  TH1D *n_smd_D = new TH1D("n_smd_D", "n_smd_D", 128, 0, 128);
  TH1D *s_smd_U = new TH1D("s_smd_U", "s_smd_U", 128, 0, 128);
  TH1D *s_smd_D = new TH1D("s_smd_D", "s_smd_D", 128, 0, 128);

  //======================================================================//

  //========================== SMD Hit Tree ==============================//
  TTree *smdHits = (TTree *) f->Get("smdHits");
  int bunchnumber, showerCutN, showerCutS;
  float n_x, n_y, s_x, s_y;
  float zdcN1_adc, zdcN2_adc, veto_NF, veto_NB;
  float zdcS1_adc, zdcS2_adc, veto_SF, veto_SB;
  float smdS1_adc, smdS2_adc, smdS6_adc, smdS7_adc;
  float smdN1_adc, smdN2_adc, smdN6_adc, smdN7_adc;
  float smdS1_v_adc, smdS2_v_adc, smdS7_v_adc, smdS8_v_adc;
  float smdN1_v_adc, smdN2_v_adc, smdN7_v_adc, smdN8_v_adc;
  smdHits->SetBranchAddress("bunchnumber", &bunchnumber);
  smdHits->SetBranchAddress("showerCutN", &showerCutN);
  smdHits->SetBranchAddress("showerCutS", &showerCutS);
  smdHits->SetBranchAddress("n_x", &n_x);
  smdHits->SetBranchAddress("n_y", &n_y);
  smdHits->SetBranchAddress("s_x", &s_x);
  smdHits->SetBranchAddress("s_y", &s_y);
  smdHits->SetBranchAddress("zdcN1_adc", &zdcN1_adc);
  smdHits->SetBranchAddress("zdcN2_adc", &zdcN2_adc);
  smdHits->SetBranchAddress("zdcS1_adc", &zdcS1_adc);
  smdHits->SetBranchAddress("zdcS2_adc", &zdcS2_adc);
  smdHits->SetBranchAddress("veto_NF", &veto_NF);
  smdHits->SetBranchAddress("veto_NB", &veto_NB);
  smdHits->SetBranchAddress("veto_SF", &veto_SF);
  smdHits->SetBranchAddress("veto_SB", &veto_SB);
  // smdHits->SetBranchStatus("n_x",0);
  // smdHits->SetBranchStatus("s_y",0);
  
  smdHits -> SetBranchAddress ("smdN1_adc",       &smdN1_adc);
  smdHits -> SetBranchAddress ("smdN2_adc",       &smdN2_adc);
  smdHits -> SetBranchAddress ("smdN6_adc",       &smdN6_adc);
  smdHits -> SetBranchAddress ("smdN7_adc",       &smdN7_adc);
  smdHits -> SetBranchAddress ("smdS1_adc",       &smdS1_adc);
  smdHits -> SetBranchAddress ("smdS2_adc",       &smdS2_adc);
  smdHits -> SetBranchAddress ("smdS6_adc",       &smdS6_adc);
  smdHits -> SetBranchAddress ("smdS7_adc",       &smdS7_adc);
  smdHits -> SetBranchAddress ("smdN1_v_adc",       &smdN1_v_adc);
  smdHits -> SetBranchAddress ("smdN2_v_adc",       &smdN2_v_adc);
  smdHits -> SetBranchAddress ("smdN7_v_adc",       &smdN7_v_adc);
  smdHits -> SetBranchAddress ("smdN8_v_adc",       &smdN8_v_adc);
  smdHits -> SetBranchAddress ("smdS1_v_adc",       &smdS1_v_adc);
  smdHits -> SetBranchAddress ("smdS2_v_adc",       &smdS2_v_adc);
  smdHits -> SetBranchAddress ("smdS7_v_adc",       &smdS7_v_adc);
  smdHits -> SetBranchAddress ("smdS8_v_adc",       &smdS8_v_adc);




  smdHits->SetBranchStatus("zdcN3_adc",0);
  smdHits->SetBranchStatus("zdcS3_adc",0);
  int nentries = smdHits->GetEntries();
  //======================================================================//

  //============================= Option A: Spin DB ================================//
  unsigned int qa_level = 0xffff;
  SpinDBOutput spin_out("phnxrc");
  SpinDBContent spin_cont;
  spin_out.StoreDBContent(runnumber, runnumber, qa_level);
  spin_out.GetDBContentStore(spin_cont, runnumber);

  int crossingshift = spin_cont.GetCrossingShift();
  // crossingshift = 0;
  // crossingshift = ixs;
  // if (ixs < 0){crossingshift = 120+ixs;}
  std::cout << crossingshift << std::endl;

  int bspinpat[120] = {0};
  int yspinpat[120] = {0};
  for (int i = 0; i < 120; i++)
  {
    bspinpat[i] = spin_cont.GetSpinPatternBlue(i);
    yspinpat[i] = spin_cont.GetSpinPatternYellow(i);
    if(isflipON)
    {
      bspinpat[i]*=-1;
      yspinpat[i]*=-1;
    }
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
  if (storenumber == 34485)
  {
    spinpatternNo = 3;
  }  // pattern names start at '111x111_P1' so index 3  means '111x111_P4' and so on

  for (int i = 0; i < 120; i++)
  {
    if (preset_pattern_blue[spinpatternNo].at(i) == '+')
    {
      bpat[i] = 1;
    }
    else if (preset_pattern_blue[spinpatternNo].at(i) == '-')
    {
      bpat[i] = -1;
    }
    else if (preset_pattern_blue[spinpatternNo].at(i) == '*')
    {
      bpat[i] = 10;
    }

    if (preset_pattern_yellow[spinpatternNo].at(i) == '+')
    {
      ypat[i] = 1;
    }
    else if (preset_pattern_yellow[spinpatternNo].at(i) == '-')
    {
      ypat[i] = -1;
    }
    else if (preset_pattern_yellow[spinpatternNo].at(i) == '*')
    {
      ypat[i] = 10;
    }
  }
  //=====================================================================================//

  //======================= process event ==================================//
  std::cout << nentries << std::endl;

  int clockOffset = 1;  // this is the offset between GL1 and ZDC events

//for (int i = 0; i <1000000; i++)
  for (int i = 0; i < nentries - clockOffset; i++)
  {
    if (i % 1000000 == 0)
    {
      std::cout << "Entry: " << i << std::endl;
    }
    smdHits->GetEntry(i + clockOffset);

    int sphenix_cross = (bunchnumber + crossingshift) % 120;

    smdHits->GetEntry(i);

    int bspin = bspinpat[sphenix_cross];  // option A: spinDB
    int yspin = yspinpat[sphenix_cross];  // option A: spinDB
    // int bspin = bpat[sphenix_cross]; //option B: preset patterns
    // int yspin = ypat[sphenix_cross]; //option B: preset patterns

    if (zdcN1_adc > ZDC1CUT && zdcN2_adc > ZDC2CUT && veto_NF < VETOCUT && veto_NB < VETOCUT && showerCutN == 1)
    {
      if (smdN1_adc > 100. && smdN2_adc > 100.)
      {
        n_smd_L->Fill(sphenix_cross);
      }
      if (smdN6_adc > 100. && smdN7_adc > 100.)
        n_smd_R->Fill(sphenix_cross);
      if (smdN1_v_adc > 100. && smdN2_v_adc > 100.)
      {
        n_smd_D->Fill(sphenix_cross);
      }
      if (smdN7_v_adc > 100. && smdN8_v_adc > 100.)
      {
        n_smd_U->Fill(sphenix_cross);
      }
      //////////// OLD //////////////////
      // if (n_x < -3)
      // {
      //   n_smd_L->Fill(sphenix_cross);
      // }
      // else if (n_x > 3)
      //   n_smd_R->Fill(sphenix_cross);

      // if (n_y > 3)
      // {
      //   n_smd_U->Fill(sphenix_cross);
      // }
      // else if (n_y < -3)
      // {
      //   n_smd_D->Fill(sphenix_cross);
      // }
      //////////// OLD ////////////////
    }

    if (zdcS1_adc > ZDC1CUT && zdcS2_adc > ZDC2CUT && veto_SF < VETOCUT && veto_SB < VETOCUT && showerCutS == 1)
    {
      if (smdS1_adc > 100. && smdS2_adc > 100.)
      {
        s_smd_L->Fill(sphenix_cross);
      }
      if (smdS6_adc > 100. && smdS7_adc > 100.)
        s_smd_R->Fill(sphenix_cross);
      if (smdS1_v_adc > 100. && smdS2_v_adc > 100.)
      {
        s_smd_D->Fill(sphenix_cross);
      }
      if (smdS7_v_adc > 100. && smdS8_v_adc > 100.)
      {
        s_smd_U->Fill(sphenix_cross);
      }

      /////////////// OLD ///////////////
      // if (s_x < -3)
      // {
      //   s_smd_L->Fill(sphenix_cross);
      // }
      // else if (s_x > 3)
      //   s_smd_R->Fill(sphenix_cross);
      // if (s_y > 3)
      // {
      //   s_smd_U->Fill(sphenix_cross);
      // }
      // else if (s_y < -3)
      // {
      //   s_smd_D->Fill(sphenix_cross);
      // }
      ///////////// OLD //////////////////
    }
  }
  //=====================================================================================//

  std::vector<double> x_spinup_blueUD, y_spinup_blueUD, ex_spinup_blueUD, ey_spinup_blueUD;
  std::vector<double> x_spindown_blueUD, y_spindown_blueUD, ex_spindown_blueUD, ey_spindown_blueUD;
  std::vector<double> x_spinup_yellowUD, y_spinup_yellowUD, ex_spinup_yellowUD, ey_spinup_yellowUD;
  std::vector<double> x_spindown_yellowUD, y_spindown_yellowUD, ex_spindown_yellowUD, ey_spindown_yellowUD;

  for (int i = 1; i <= 115; ++i)
  {
    double A = n_smd_U->GetBinContent(i);
    double B = n_smd_D->GetBinContent(i);
    double A_y = s_smd_U->GetBinContent(i);
    double B_y = s_smd_D->GetBinContent(i);
    std::cout << "A : " << A << " //// B : " << B << std::endl;
    int nbunch = i - 1;
    int b_spin = bspinpat[nbunch];
    int y_spin = yspinpat[nbunch];
    if (b_spin != 1 && b_spin != -1)
      continue;
    if (A != 0 || B != 0)
    {
      double aym = (A - B) / (A + B);
      double error = TMath::Sqrt(4 * (A * B) / ((A + B) * (A + B) * (A + B)));
      if (b_spin == 1)
      {  // spinup_blueUD bin
        x_spinup_blueUD.push_back(nbunch);
        y_spinup_blueUD.push_back(aym);
        ex_spinup_blueUD.push_back(0);  // No error on x
        ey_spinup_blueUD.push_back(error);
      }
      else if (b_spin == -1)
      {  // spindown_blueUD bin
        x_spindown_blueUD.push_back(nbunch);
        y_spindown_blueUD.push_back(aym);
        ex_spindown_blueUD.push_back(0);  // No error on x
        ey_spindown_blueUD.push_back(error);
      }
    }
    if (A_y != 0 || B_y != 0)
    {
      double aym = (A_y - B_y) / (A_y + B_y);
      double error = TMath::Sqrt(4 * (A_y * B_y) / ((A_y + B_y) * (A_y + B_y) * (A_y + B_y)));
      if (y_spin == 1)
      {  // spinup_blueUD bin
        x_spinup_yellowUD.push_back(nbunch);
        y_spinup_yellowUD.push_back(aym);
        ex_spinup_yellowUD.push_back(0);  // No error on x
        ey_spinup_yellowUD.push_back(error);
      }
      else if (y_spin == -1)
      {  // spindown_yellowUD bin
        x_spindown_yellowUD.push_back(nbunch);
        y_spindown_yellowUD.push_back(aym);
        ex_spindown_yellowUD.push_back(0);  // No error on x
        ey_spindown_yellowUD.push_back(error);
      }
    }
  }

  std::vector<double> x_spinup_blue, y_spinup_blue, ex_spinup_blue, ey_spinup_blue;
  std::vector<double> x_spindown_blue, y_spindown_blue, ex_spindown_blue, ey_spindown_blue;
  std::vector<double> x_spinup_yellow, y_spinup_yellow, ex_spinup_yellow, ey_spinup_yellow;
  std::vector<double> x_spindown_yellow, y_spindown_yellow, ex_spindown_yellow, ey_spindown_yellow;

  int nBins = n_smd_L->GetNbinsX();
  for (int i = 1; i <= 115; ++i)
  {
    double A = n_smd_L->GetBinContent(i);
    double B = n_smd_R->GetBinContent(i);
    double A_y = s_smd_L->GetBinContent(i);
    double B_y = s_smd_R->GetBinContent(i);
    std::cout << "A : " << A << " //// B : " << B << std::endl;
    int nbunch = i - 1;
    int b_spin = bspinpat[nbunch];
    int y_spin = yspinpat[nbunch];
    if (b_spin != 1 && b_spin != -1)
      continue;
    if (A != 0 || B != 0)
    {
      double aym = (A - B) / (A + B);
      double error = TMath::Sqrt(4 * (A * B) / ((A + B) * (A + B) * (A + B)));
      if (b_spin == 1)
      {  // spinup_blue bin
        x_spinup_blue.push_back(nbunch);
        y_spinup_blue.push_back(aym);
        ex_spinup_blue.push_back(0);  // No error on x
        ey_spinup_blue.push_back(error);
      }
      else if (b_spin == -1)
      {  // spindown_blue bin
        x_spindown_blue.push_back(nbunch);
        y_spindown_blue.push_back(aym);
        ex_spindown_blue.push_back(0);  // No error on x
        ey_spindown_blue.push_back(error);
      }
    }
    if (A_y != 0 || B_y != 0)
    {
      double aym = (A_y - B_y) / (A_y + B_y);
      double error = TMath::Sqrt(4 * (A_y * B_y) / ((A_y + B_y) * (A_y + B_y) * (A_y + B_y)));
      if (y_spin == 1)
      {  // spinup_blue bin
        x_spinup_yellow.push_back(nbunch);
        y_spinup_yellow.push_back(aym);
        ex_spinup_yellow.push_back(0);  // No error on x
        ey_spinup_yellow.push_back(error);
      }
      else if (y_spin == -1)
      {  // spindown_yellow bin
        x_spindown_yellow.push_back(nbunch);
        y_spindown_yellow.push_back(aym);
        ex_spindown_yellow.push_back(0);  // No error on x
        ey_spindown_yellow.push_back(error);
      }
    }
  }

  // Create the TGraphErrors for spinup_blueUD bins
  TGraphErrors *graph_spinup_blueUD = new TGraphErrors(x_spinup_blueUD.size(), x_spinup_blueUD.data(), y_spinup_blueUD.data(), ex_spinup_blueUD.data(), ey_spinup_blueUD.data());
  graph_spinup_blueUD->SetMarkerStyle(21);
  graph_spinup_blueUD->SetTitle("SMD North Up Down asymmetry");
  graph_spinup_blueUD->SetMarkerColor(kRed);
  graph_spinup_blueUD->SetLineColor(kRed);

  // Create the TGraphErrors for spindown_blueUD bins
  TGraphErrors *graph_spindown_blueUD = new TGraphErrors(x_spindown_blueUD.size(), x_spindown_blueUD.data(), y_spindown_blueUD.data(), ex_spindown_blueUD.data(), ey_spindown_blueUD.data());
  graph_spindown_blueUD->SetMarkerStyle(21);
  graph_spindown_blueUD->SetTitle("SMD North Up Down asymmetry");
  graph_spindown_blueUD->SetMarkerColor(kBlue);
  graph_spindown_blueUD->SetLineColor(kBlue);

  // Create the TGraphErrors for spinup_yellowUD bins
  TGraphErrors *graph_spinup_yellowUD = new TGraphErrors(x_spinup_yellowUD.size(), x_spinup_yellowUD.data(), y_spinup_yellowUD.data(), ex_spinup_yellowUD.data(), ey_spinup_yellowUD.data());
  graph_spinup_yellowUD->SetMarkerStyle(21);
  graph_spinup_yellowUD->SetTitle("SMD South Up Down asymmetry");
  graph_spinup_yellowUD->SetMarkerColor(kRed);
  graph_spinup_yellowUD->SetLineColor(kRed);

  // Create the TGraphErrors for spindown_yellowUD bins
  TGraphErrors *graph_spindown_yellowUD = new TGraphErrors(x_spindown_yellowUD.size(), x_spindown_yellowUD.data(), y_spindown_yellowUD.data(), ex_spindown_yellowUD.data(), ey_spindown_yellowUD.data());
  graph_spindown_yellowUD->SetMarkerStyle(21);
  graph_spindown_yellowUD->SetTitle("SMD South Up Down asymmetry ");
  graph_spindown_yellowUD->SetMarkerColor(kBlue);
  graph_spindown_yellowUD->SetLineColor(kBlue);

  // Create the TGraphErrors for spinup_blue bins
  TGraphErrors *graph_spinup_blue = new TGraphErrors(x_spinup_blue.size(), x_spinup_blue.data(), y_spinup_blue.data(), ex_spinup_blue.data(), ey_spinup_blue.data());
  graph_spinup_blue->SetMarkerStyle(21);
  graph_spinup_blue->SetTitle("SMD North Left Right asymmetry");
  graph_spinup_blue->SetMarkerColor(kRed);
  graph_spinup_blue->SetLineColor(kRed);

  // Create the TGraphErrors for spindown_blue bins
  TGraphErrors *graph_spindown_blue = new TGraphErrors(x_spindown_blue.size(), x_spindown_blue.data(), y_spindown_blue.data(), ex_spindown_blue.data(), ey_spindown_blue.data());
  graph_spindown_blue->SetMarkerStyle(21);
  graph_spindown_blue->SetTitle("SMD North Left Right asymmetry");
  graph_spindown_blue->SetMarkerColor(kBlue);
  graph_spindown_blue->SetLineColor(kBlue);

  // Create the TGraphErrors for spinup_yellow bins
  TGraphErrors *graph_spinup_yellow = new TGraphErrors(x_spinup_yellow.size(), x_spinup_yellow.data(), y_spinup_yellow.data(), ex_spinup_yellow.data(), ey_spinup_yellow.data());
  graph_spinup_yellow->SetMarkerStyle(21);
  graph_spinup_yellow->SetTitle("SMD South Left Right asymmetry");
  graph_spinup_yellow->SetMarkerColor(kRed);
  graph_spinup_yellow->SetLineColor(kRed);

  // Create the TGraphErrors for spindown_yellow bins
  TGraphErrors *graph_spindown_yellow = new TGraphErrors(x_spindown_yellow.size(), x_spindown_yellow.data(), y_spindown_yellow.data(), ex_spindown_yellow.data(), ey_spindown_yellow.data());
  graph_spindown_yellow->SetMarkerStyle(21);
  graph_spindown_yellow->SetTitle("SMD South Left Right asymmetry ");
  graph_spindown_yellow->SetMarkerColor(kBlue);
  graph_spindown_yellow->SetLineColor(kBlue);

  TLatex *latex = new TLatex();
  latex->SetNDC();
  latex->SetTextSize(0.045);
  latex->SetTextAlign(31);
  TCanvas *canvas = new TCanvas("canvas_blue", "North SMD", 950, 800);
  graph_spinup_blue->GetXaxis()->SetLimits(0, 111);
  graph_spinup_blue->GetYaxis()->SetTitle("(L-R)/(L+R)");
  graph_spinup_blue->GetXaxis()->SetTitle("Bunch number");
  graph_spinup_blue->SetMinimum(-0.08);
  graph_spinup_blue->SetMaximum(0.02);
  canvas->cd();
  graph_spinup_blue->Draw("AP");
  graph_spindown_blue->Draw("P SAME");
  TFitResultPtr fit_spinup_blue = graph_spinup_blue->Fit("pol0", "S");
  TFitResultPtr fit_spindown_blue = graph_spindown_blue->Fit("pol0", "S");
  // Add legend
  TLegend leg1(.20, .70, .60, .90);
  leg1.AddEntry((TObject *) 0, Form("SMD North / Run 42796+42797"), "");
  leg1.AddEntry(graph_spinup_blue, Form("blue #uparrow :  %.5f #pm %.5f", fit_spinup_blue->Parameter(0), fit_spinup_blue->ParError(0)), "LP");
  leg1.AddEntry(graph_spindown_blue, Form("blue #downarrow : %.5f #pm %.5f", fit_spindown_blue->Parameter(0), fit_spindown_blue->ParError(0)), "LP");
  leg1.SetTextSize(0.03);
  leg1.Draw();
  latex->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{p+p}, 200 GeV #it{#bf{sPHENIX}} internal"));

  TCanvas *canvas2 = new TCanvas("canvas_blueUD", "North SMD UD", 950, 800);
  graph_spinup_blueUD->GetXaxis()->SetLimits(0, 111);
  graph_spinup_blueUD->GetYaxis()->SetTitle("(U-D)/(U+D)");
  graph_spinup_blueUD->GetXaxis()->SetTitle("Bunch number");
  graph_spinup_blueUD->SetMinimum(0.18);
  graph_spinup_blueUD->SetMaximum(0.18+0.1);
  canvas2->cd();
  graph_spinup_blueUD->Draw("AP");
  graph_spindown_blueUD->Draw("P SAME");
  // Add legend
  TFitResultPtr fit_spinup_blueUD = graph_spinup_blueUD->Fit("pol0", "S");
  TFitResultPtr fit_spindown_blueUD = graph_spindown_blueUD->Fit("pol0", "S");
  TLegend leg2(.20, .70, .60, .90);
  leg2.AddEntry((TObject *) 0, Form("SMD North / Run 42796+42797"), "");
  leg2.AddEntry(graph_spinup_blueUD, Form("blue #uparrow :  %.5f #pm %.5f", fit_spinup_blueUD->Parameter(0), fit_spinup_blueUD->ParError(0)), "LP");
  leg2.AddEntry(graph_spindown_blueUD, Form("blue #downarrow : %.5f #pm %.5f", fit_spindown_blueUD->Parameter(0), fit_spindown_blueUD->ParError(0)), "LP");
  leg2.SetTextSize(0.03);
  leg2.Draw();
  latex->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{p+p}, 200 GeV #it{#bf{sPHENIX}} internal"));

  TCanvas *canvas3 = new TCanvas("canvas_yellow", "South SMD", 950, 800);
  graph_spinup_yellow->GetXaxis()->SetLimits(0, 111);
  graph_spinup_yellow->GetYaxis()->SetTitle("(L-R)/(L+R)");
  graph_spinup_yellow->GetXaxis()->SetTitle("Bunch number");
  graph_spinup_yellow->SetMaximum(0.07);
  graph_spinup_yellow->SetMinimum(-0.03);
  canvas3->cd();
  graph_spinup_yellow->Draw("AP");
  graph_spindown_yellow->Draw("P SAME");
  // Add legend
  TFitResultPtr fit_spinup_yellow = graph_spinup_yellow->Fit("pol0", "S");
  TFitResultPtr fit_spindown_yellow = graph_spindown_yellow->Fit("pol0", "S");
  TLegend leg3(.20, .70, .60, .90);
  leg3.AddEntry((TObject *) 0, Form("SMD South / Run 42796+42797"), "");
  leg3.AddEntry(graph_spinup_yellow, Form("yellow #uparrow :  %.5f #pm %.5f", fit_spinup_yellow->Parameter(0), fit_spinup_yellow->ParError(0)), "LP");
  leg3.AddEntry(graph_spindown_yellow, Form("yellow #downarrow : %.5f #pm %.5f", fit_spindown_yellow->Parameter(0), fit_spindown_yellow->ParError(0)), "LP");
  leg3.SetTextSize(0.03);
  leg3.Draw();
  latex->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{p+p}, 200 GeV #it{#bf{sPHENIX}} internal"));

  TCanvas *canvas4 = new TCanvas("canvas_yellow_UD", "South SMD", 950, 800);
  graph_spinup_yellowUD->GetXaxis()->SetLimits(0, 111);
  graph_spinup_yellowUD->GetYaxis()->SetTitle("(U-D)/(U+D)");
  graph_spinup_yellowUD->GetXaxis()->SetTitle("Bunch number");
  graph_spinup_yellowUD->SetMaximum(-0.04 + 0.1);
  graph_spinup_yellowUD->SetMinimum(-0.04);
  canvas4->cd();
  graph_spinup_yellowUD->Draw("AP");
  graph_spindown_yellowUD->Draw("P SAME");
  // Add legend
  // TLegend *legend4 = new TLegend(0.7, 0.9, 0.8, 0.95);
  // legend4->AddEntry(graph_spinup_yellowUD, "yellow spin up Bunches", "p");
  // legend4->AddEntry(graph_spindown_yellowUD, "yellow spin down Bunches", "p");
  // legend4->Draw();
  // latex->DrawLatexNDC(0.65, 0.8, "#bf{sPHENIX} internal");
  // if (!ismerged)
  //   latex->DrawLatexNDC(0.65, 0.75, Form("Run %d", runnumber));
  // else
  //   latex->DrawLatexNDC(0.65, 0.75, Form("Store %d", storenumber));
  TFitResultPtr fit_spinup_yellowUD = graph_spinup_yellowUD->Fit("pol0", "S");
  TFitResultPtr fit_spindown_yellowUD = graph_spindown_yellowUD->Fit("pol0", "S");
  TLegend leg4(.20, .70, .60, .90);
  leg4.AddEntry((TObject *) 0, Form("SMD South / Run 42796+42797"), "");
  leg4.AddEntry(graph_spinup_yellowUD, Form("yellow #uparrow :  %.5f #pm %.5f", fit_spinup_yellowUD->Parameter(0), fit_spinup_yellowUD->ParError(0)), "LP");
  leg4.AddEntry(graph_spindown_yellowUD, Form("yellow #downarrow : %.5f #pm %.5f", fit_spindown_yellowUD->Parameter(0), fit_spindown_yellowUD->ParError(0)), "LP");
  leg4.SetTextSize(0.03);
  leg4.Draw();
  latex->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{p+p}, 200 GeV #it{#bf{sPHENIX}} internal"));

  TCanvas *canvas5 = new TCanvas("canvas5", "canvas5", 950, 800);
  canvas5->cd();
  canvas5->SetLogy();
  // n_smd_L->SetLineWidth(1.5);
  n_smd_L->SetXTitle("bunch number");
  // n_smd_L->SetMarkerStyle(kFullCircle);
  n_smd_L->Draw("E");
  latex->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{p+p}, 200 GeV #it{#bf{sPHENIX}} internal"));

  canvas5->SaveAs("north_smd1.pdf");
  TCanvas *canvas6 = new TCanvas("canvas6", "canvas6", 950, 800);
  canvas6->cd();
  canvas6->SetLogy();
  // n_smd_L->SetLineWidth(1.5);
  n_smd_R->SetXTitle("bunch number");
  // n_smd_R->SetMarkerStyle(kFullCircle);
  n_smd_R->Draw("E");
  latex->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{p+p}, 200 GeV #it{#bf{sPHENIX}} internal"));
  canvas6->SaveAs("north_smd2.pdf");

  TFile *sfile = new TFile("sfile.root", "RECREATE");
  TString pdfName;
  TString fileName;
  sfile->cd();
  canvas->Write();
  pdfName = canvas->GetName();
  fileName = pdfName + ".pdf";
  canvas->SaveAs(fileName);
  canvas->Update();
  canvas2->Write();
  pdfName = canvas2->GetName();
  fileName = pdfName + ".pdf";
  canvas2->SaveAs(fileName);
  canvas2->Update();
  canvas3->Write();
  pdfName = canvas3->GetName();
  fileName = pdfName + ".pdf";
  canvas3->SaveAs(fileName);
  canvas3->Update();
  canvas4->Write();
  pdfName = canvas4->GetName();
  fileName = pdfName + ".pdf";
  canvas4->SaveAs(fileName);
  canvas4->Update();

  n_smd_L->Write();
  n_smd_R->Write();
  s_smd_L->Write();
  s_smd_R->Write();
  sfile->Close();
}
