/* em_cluster_caloid values:
   1 = CEMC
   4 = EEMC
   5 = FEMC
*/

int
makePlot_reco_electron_id()
{
  gStyle->SetOptStat(0);

  TFile *fin = new TFile("../../data/Sample_DISReco_ep.root", "OPEN");
  //TFile *fin = new TFile("../../data/DISReco_Pythia6_DIS_20x250_1k.root", "OPEN");

  TTree *t_reco = (TTree*)fin->Get("event_cluster");

  /* define cuts */
  TCut cut_pid_e("em_evtgen_pid == 11");
  TCut cut_pid_pion("abs(em_evtgen_pid) == 211");
  TCut cut_eta( "1" );

  TCanvas *ctemp = new TCanvas();

  /* E over p ratio */
  ctemp->cd();

  TH1F* h_e_over_p_electron = new TH1F("h_e_over_p_electron", ";E / p", 100, 0, 2);
  TH1F* h_e_over_p_pion = (TH1F*)h_e_over_p_electron->Clone("h_e_over_p_pion");

  h_e_over_p_electron->SetLineColor( kBlue );
  h_e_over_p_pion->SetLineColor( kRed );

  t_reco->Draw( "em_cluster_e / em_track_ptotal >> h_e_over_p_electron", cut_pid_e && cut_eta );
  t_reco->Draw( "em_cluster_e / em_track_ptotal >> h_e_over_p_pion", cut_pid_pion && cut_eta );

  TCanvas *c1 = new TCanvas();
  c1->SetLogy(1);
  h_e_over_p_electron->Draw("");
  h_e_over_p_pion->Draw("sames");

  c1->Print("plots/electron_id_c1.eps");
  c1->Print("plots/electron_id_c1.png");


  /* Cluster shape */
  ctemp->cd();

  TH1F* h_em_prob_electron = new TH1F("h_em_prob_electron", ";electron shape probability", 100, 0, 1.0);
  TH1F* h_em_prob_pion = (TH1F*)h_em_prob_electron->Clone("h_em_prob_pion");

  h_em_prob_electron->SetLineColor( kBlue );
  h_em_prob_pion->SetLineColor( kRed );

  t_reco->Draw( "em_cluster_prob >> h_em_prob_electron", cut_pid_e && cut_eta );
  t_reco->Draw( "em_cluster_prob >> h_em_prob_pion", cut_pid_pion && cut_eta );

  TCanvas *c2 = new TCanvas();
  c2->SetLogy(1);
  h_em_prob_pion->Draw("");
  h_em_prob_electron->Draw("sames");

  c2->Print("plots/electron_id_c2.eps");
  c2->Print("plots/electron_id_c2.png");


  /* Cluster pseudorapidity */
  ctemp->cd();

  TH1F* h_em_eta_electron = new TH1F("h_em_eta_electron", ";#eta", 101, -5.05, 5.05);
  TH1F* h_em_eta_pion = (TH1F*)h_em_eta_electron->Clone("h_em_eta_pion");

  h_em_eta_electron->SetLineColor( kBlue );
  h_em_eta_pion->SetLineColor( kRed );

  //t_reco->Draw( "-1*log(tan(em_cluster_theta/2.0)) >> h_em_eta_electron", cut_pid_e );
  //t_reco->Draw( "-1*log(tan(em_cluster_theta/2.0)) >> h_em_eta_pion", cut_pid_pion );

  t_reco->Draw( "em_cluster_eta >> h_em_eta_electron", cut_pid_e );
  t_reco->Draw( "em_cluster_eta >> h_em_eta_pion", cut_pid_pion );

  TCanvas *c3 = new TCanvas();
  c3->SetLogy(1);
  h_em_eta_pion->Draw("");
  h_em_eta_electron->Draw("sames");

  c3->Print("plots/electron_id_c3.eps");
  c3->Print("plots/electron_id_c3.png");


  return 1;
}
