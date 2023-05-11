/* em_cluster_caloid values:
   1 = CEMC
   4 = EEMC
   5 = FEMC
*/

#include "plot_commons.h"

int
makePlot_reco_xQ2()
{
  gStyle->SetOptStat(0);

  TFile *fin = new TFile("../../data/Sample_DISReco_ep.root", "OPEN");
  //TFile *fin = new TFile("../../data/DISReco_Pythia6_DIS_20x250_1k.root", "OPEN");

  TTree *t_reco = (TTree*)fin->Get("event_cluster");

  /* Q2: Create histogram */
  TH2F* hQ2_reco_truth = make_TH2D_xylog( 60, 0, 3, 60, 0, 3 );
  hQ2_reco_truth->SetName("hQ2_reco_truth");
  hQ2_reco_truth->GetXaxis()->SetTitle("Q^{2}_{truth} (GeV^{2})");
  hQ2_reco_truth->GetYaxis()->SetTitle("Q^{2}_{reco} (GeV^{2})");

  /* Q2: Fill histogram */
  TCanvas *c1 = new TCanvas();
  c1->SetLogx(1);
  c1->SetLogy(1);
  c1->SetLogz(1);
  t_reco->Draw("em_reco_q2_e:evtgen_Q2 >> hQ2_reco_truth","em_evtgen_pid==11", "colz");
  c1->Print("plots/reco_xQ2_1.eps");

  /* x: Create histogram */
  TH2F* hx_reco_truth = make_TH2D_xylog( 40, -4, 0, 40, -4, 0 );
  hx_reco_truth->SetName("hx_reco_truth");
  hx_reco_truth->GetXaxis()->SetTitle("x_{truth}");
  hx_reco_truth->GetYaxis()->SetTitle("x_{reco}");

  /* x: Fill histogram */
  TCanvas *c2 = new TCanvas();
  c2->SetLogx(1);
  c2->SetLogy(1);
  c2->SetLogz(1);
  t_reco->Draw("em_reco_q2_e:evtgen_x >> hx_reco_truth","em_evtgen_pid==11", "colz");
  c2->Print("plots/reco_xQ2_2.eps");

  /* W: Create histogram */
  TH2F* hW_reco_truth = new TH2F("hW_reco_truth","W_{truth};W_{reco}",50,0,100,50,0,100);
  hW_reco_truth->SetName("hW_reco_truth");
  hW_reco_truth->GetXaxis()->SetTitle("W_{truth}");
  hW_reco_truth->GetYaxis()->SetTitle("W_{reco}");

  /* W: Fill histogram */
  TCanvas *c3 = new TCanvas();
  //c3->SetLogx(1);
  //c3->SetLogy(1);
  c3->SetLogz(1);
  t_reco->Draw("em_reco_w_e:evtgen_W >> hW_reco_truth","em_evtgen_pid==11", "colz");
  c3->Print("plots/reco_xQ2_3.eps");

  /* y: Create histogram */
  TH2F* hy_reco_truth = new TH2F("hy_reco_truth","y_{truth};y_{reco}",100,0,1,100,0,1);
  hy_reco_truth->SetName("hy_reco_truth");
  hy_reco_truth->GetXaxis()->SetTitle("y_{truth}");
  hy_reco_truth->GetYaxis()->SetTitle("y_{reco}");

  /* y: Fill histogram */
  TCanvas *c4 = new TCanvas();
  //c4->SetLogx(1);
  //c4->SetLogy(1);
  c3->SetLogz(1);
  t_reco->Draw("em_reco_y_e:evtgen_y >> hy_reco_truth","em_evtgen_pid==11", "colz");
  c4->Print("plots/reco_xQ2_4.eps");

  return 1;
}
