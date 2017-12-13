calo_towers()
{
  gStyle->SetOptStat(0);

  unsigned col1 = kOrange+7;
  unsigned col2 = kBlue+2;

  /* open inout files and merge trees */
  TChain chain_tower("ntp_tower");
  chain_tower.Add("data/p250_e20_1000events_file1_LeptoAna_r05.root");

  /* EMCal segmentation eta-phi: 0.025 x 0.025, cover 1.1 in eta, 96 x 256 towers
   * HCAL  segmentation eta-phi: 0.1 x 0.1 */
  TH2F* h2_calo_tau = new TH2F("h2_calo_tau", ";#Delta#phi;#Delta#eta;E/#SigmaE", 16, -0.2, 0.2, 16, -0.2, 0.2 );
  h2_calo_tau->SetLineColor( col1 );
  h2_calo_tau->GetXaxis()->SetNdivisions(505);
  h2_calo_tau->GetYaxis()->SetNdivisions(505);
  h2_calo_tau->GetZaxis()->SetNdivisions(505);
  h2_calo_tau->GetZaxis()->SetTitleOffset(1.35);
  chain_tower.Draw("( tower_eta - tau_eta ) : ( tower_phi - tau_phi ) >> h2_calo_tau","tower_e * (is_tau==1 && tau_decay_prong==3)");
  h2_calo_tau->Scale( 1. / h2_calo_tau->Integral() );

  TH2F* h2_calo_uds = new TH2F("h2_calo_uds", ";#Delta#phi;#Delta#eta;E/#SigmaE", 16, -0.2, 0.2, 16, -0.2, 0.2 );
  h2_calo_uds->SetLineColor( col2 );
  h2_calo_uds->GetXaxis()->SetNdivisions(505);
  h2_calo_uds->GetYaxis()->SetNdivisions(505);
  h2_calo_uds->GetZaxis()->SetNdivisions(505);
  h2_calo_uds->GetZaxis()->SetTitleOffset(1.35);
  chain_tower.Draw("( tower_eta - uds_eta ) : ( tower_phi - uds_phi ) >> h2_calo_uds","tower_e * (is_uds==1)");
  h2_calo_uds->Scale( 1. / h2_calo_uds->Integral() );

  /* plot histograms */
  TCanvas *c1 = new TCanvas();
  h2_calo_tau->Draw("lego");
  c1->Print("plots/calo_towers_2d_tau.eps");
  c1->Print("plots/calo_towers_2d_tau.png");

  TCanvas *c2 = new TCanvas();
  h2_calo_uds->Draw("lego");
  c2->Print("plots/calo_towers_2d_uds.eps");
  c2->Print("plots/calo_towers_2d_uds.png");
}
