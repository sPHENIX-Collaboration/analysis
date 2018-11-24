#include <vector>
int eicsmear_dvmp_plot2()
{

  /* PRELIMINARY ROOT STUFF */
  /*gSystem->Load("libeicsmear");
  gROOT->LoadMacro("/sphenix/user/gregtom3/SBU/research/macros/macros/sPHENIXStyle/sPhenixStyle.C");
  SetsPhenixStyle();*/


  /* Open TFile */
  TFile *input = new TFile("/sphenix/user/gregtom3/data/Fall2018/JPsi_reco_studies/theEvents.root","READ");

  /* Open TTree in TFile */
  TTree *t = (TTree*)input->Get("Tree");
  TH1F *h1_p_1 = new TH1F("h1_p_1","h1_p_1",100,-4,4);
  TH1F *h1_p_2 = new TH1F("h1_p_2","h1_p_2",100,0,50);
  TH1F *h1_p_3 = new TH1F("h1_p_3","h1_p_3",100,0,50);
  TH1F *h1_p_4 = new TH1F("h1_p_4","h1_p_4",100,0,50);
  TH2F* h2_p = new TH2F("h2_p","h2_p",200,-6,6,200,0,2);
  //BinLogX(h2_p);
  BinLogY(h2_p);

  // t->Draw("2*18*se_p_truth*(1-cos(3.14159265-2*atan(exp(-se_eta_truth)))):2*275*sp_p_truth*(1-cos(2*atan(exp(-sp_eta_truth))))>>h2_p","jpsi_eta_truth>4","colz");
  // t->Draw("sqrt(jpsi_px_reco*jpsi_px_reco+jpsi_py_reco*jpsi_py_reco+jpsi_pz_reco*jpsi_pz_reco):jpsi_eta_reco>>h2_p","","colz");

  // t->Draw("se_eta_truth>>h1_p_1","(se_eta_reco!=0&&de_eta_reco!=0&&dp_eta_reco!=0)");
  
  // X - Q2 PLOT //
  //t->Draw("2*18*se_p_reco*(1-cos(3.14159265-2*atan(exp(-se_eta_reco)))):2*18*se_p_reco*(1-cos(3.14159265-2*atan(exp(-se_eta_reco))))/(4*18*275*(1-(se_p_reco/18)+(2*18*se_p_reco*(1-cos(3.14159265-2*atan(exp(-se_eta_reco))))/(4*18*18))))>>h2_p","","colz");
  // gPad->SetLogx();
  //gPad->SetLogy();
  t->Draw("2*18*se_p_reco*(1-cos(3.14159265-2*atan(exp(-se_eta_reco)))):jpsi_eta_reco>>h2_p","sp_p_truth>274","colz");
  h2_p->SetTitle("x vs. Q2 (18x275 Sartre DVMP Reco)");
  h2_p->GetXaxis()->SetTitle("Reco x");
  h2_p->GetYaxis()->SetTitle("Reco Q2");
  
}

void BinLogY(TH2F*h) 
{

   TAxis *axis = h->GetYaxis(); 
   int bins = axis->GetNbins();

   Axis_t from = axis->GetXmin();
   Axis_t to = axis->GetXmax();
   Axis_t width = (to - from) / bins;
   Axis_t *new_bins = new Axis_t[bins + 1];

   for (int i = 0; i <= bins; i++) {
     new_bins[i] = TMath::Power(10, from + i * width);
   } 
   axis->Set(bins, new_bins); 
   delete new_bins; 
}

void BinLogX(TH2F*h) 
{

   TAxis *axis = h->GetXaxis(); 
   int bins = axis->GetNbins();

   Axis_t from = axis->GetXmin();
   Axis_t to = axis->GetXmax();
   Axis_t width = (to - from) / bins;
   Axis_t *new_bins = new Axis_t[bins + 1];

   for (int i = 0; i <= bins; i++) {
     new_bins[i] = TMath::Power(10, from + i * width);
   } 
   axis->Set(bins, new_bins); 
   delete new_bins; 
}
