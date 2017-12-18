calo_towers()
{
  gStyle->SetOptStat(0);

  unsigned col1 = kOrange+7;
  unsigned col2 = kBlue+2;

  /* open inout files and merge trees */
  TChain chain_tower("ntp_tower");
  chain_tower.Add("data_3pions/p250_e20_0events_file1093_LeptoAna_r05.root");
  chain_tower.Add("data_3pions/p250_e20_0events_file1096_LeptoAna_r05.root");
  chain_tower.Add("data_3pions/p250_e20_0events_file1101_LeptoAna_r05.root");
  chain_tower.Add("data_3pions/p250_e20_0events_file1115_LeptoAna_r05.root");
  chain_tower.Add("data_3pions/p250_e20_0events_file1122_LeptoAna_r05.root");
  chain_tower.Add("data_3pions/p250_e20_0events_file1127_LeptoAna_r05.root");
  chain_tower.Add("data_3pions/p250_e20_0events_file1131_LeptoAna_r05.root");
  chain_tower.Add("data_3pions/p250_e20_0events_file1164_LeptoAna_r05.root");

  TCut select_tower_tau("tower_e * (is_tau==1 && abs(jet_eta) < 1.0 && jet_etotal>5 && sqrt( (tau_eta-jet_eta)*(tau_eta-jet_eta) + (tau_phi-jet_phi)*(tau_phi-jet_phi) ) < 0.1)");

  TCut select_tower_uds("tower_e * (is_tau==0 && abs(jet_eta) < 1.0 && jet_etotal>5)");

  /* EMCal segmentation eta-phi: 0.025 x 0.025, cover 1.1 in eta, 96 x 256 towers
   * HCAL  segmentation eta-phi: 0.1 x 0.1 */
  int nbins = 10;
  float maxmin = 0.5;

  TH2F* h2_calo_tau = new TH2F("h2_calo_tau", ";#Delta#phi;#Delta#eta;E/#SigmaE", nbins, -maxmin, maxmin, nbins, -maxmin, maxmin );
  h2_calo_tau->SetLineColor( col2 );
  h2_calo_tau->GetXaxis()->SetNdivisions(505);
  h2_calo_tau->GetYaxis()->SetNdivisions(505);
  h2_calo_tau->GetZaxis()->SetNdivisions(505);
  h2_calo_tau->GetZaxis()->SetTitleOffset(1.35);
  chain_tower.Draw("( tower_eta - jet_eta ) : ( tower_phi - jet_phi ) >> h2_calo_tau",select_tower_tau);
  h2_calo_tau->Scale( 1. / h2_calo_tau->Integral() );
  //h2_calo_tau->GetZaxis()->SetRangeUser(0,0.04);

  TH2F* h2_calo_uds = new TH2F("h2_calo_uds", ";#Delta#phi;#Delta#eta;E/#SigmaE", nbins, -maxmin, maxmin, nbins, -maxmin, maxmin );
  h2_calo_uds->SetLineColor( col1 );
  h2_calo_uds->GetXaxis()->SetNdivisions(505);
  h2_calo_uds->GetYaxis()->SetNdivisions(505);
  h2_calo_uds->GetZaxis()->SetNdivisions(505);
  h2_calo_uds->GetZaxis()->SetTitleOffset(1.35);
  chain_tower.Draw("( tower_eta - jet_eta ) : ( tower_phi - jet_phi ) >> h2_calo_uds",select_tower_uds);
  h2_calo_uds->Scale( 1. / h2_calo_uds->Integral() );
  //h2_calo_uds->GetZaxis()->SetRangeUser(0,0.04);

  /* plot histograms */
  TCanvas *c1 = new TCanvas();
  //c1->SetRightMargin(1.5);
  h2_calo_tau->Draw("lego");
  c1->Print("plots/calo_towers_2d_tau.eps");
  c1->Print("plots/calo_towers_2d_tau.png");

  TCanvas *c2 = new TCanvas();
  //c2->SetRightMargin(1.5);
  h2_calo_uds->Draw("lego");
  c2->Print("plots/calo_towers_2d_uds.eps");
  c2->Print("plots/calo_towers_2d_uds.png");

  //TCanvas *c3 = new TCanvas();
  //chain_tower.Draw("sqrt( ( tower_eta - jet_eta ) * ( tower_eta - jet_eta ) + ( tower_phi - jet_phi ) * ( tower_phi - jet_phi ) ) >> htest_uds", select_tower_uds);
}
