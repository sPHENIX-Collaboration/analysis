tracks()
{
  gStyle->SetOptStat(0);

  unsigned col1 = kOrange+7;
  unsigned col2 = kBlue+2;

  /* open inout files and merge trees */
  TChain chain_track("ntp_track");
  chain_track.Add("data_3pions/p250_e20_0events_file1093_LeptoAna_r05.root");
  chain_track.Add("data_3pions/p250_e20_0events_file1096_LeptoAna_r05.root");
  chain_track.Add("data_3pions/p250_e20_0events_file1101_LeptoAna_r05.root");
  chain_track.Add("data_3pions/p250_e20_0events_file1115_LeptoAna_r05.root");
  chain_track.Add("data_3pions/p250_e20_0events_file1122_LeptoAna_r05.root");
  chain_track.Add("data_3pions/p250_e20_0events_file1127_LeptoAna_r05.root");
  chain_track.Add("data_3pions/p250_e20_0events_file1131_LeptoAna_r05.root");
  chain_track.Add("data_3pions/p250_e20_0events_file1164_LeptoAna_r05.root");

  TCut select_tracks_tau("1 * (is_tau==1 && abs(jet_eta) < 1.0 && jet_etotal>5 && sqrt( (tau_eta-jet_eta)*(tau_eta-jet_eta) + (tau_phi-jet_phi)*(tau_phi-jet_phi) ) < 0.1)");

  TCut select_tracks_uds("1 * (is_tau==0 && abs(jet_eta) < 1.0 && jet_etotal>5)");

  TH2F* h2_tracks_tau = new TH2F("h2_tracks_tau", ";#Delta#phi;#Delta#eta;N/#SigmaN", 40, -0.2, 0.2, 40, -0.2, 0.2 );
  h2_tracks_tau->SetLineColor( col2 );
  h2_tracks_tau->GetXaxis()->SetNdivisions(505);
  h2_tracks_tau->GetYaxis()->SetNdivisions(505);
  h2_tracks_tau->GetZaxis()->SetNdivisions(505);
  h2_tracks_tau->GetZaxis()->SetTitleOffset(1.35);
  chain_track.Draw("( track_eta - jet_eta ) : ( track_phi - jet_phi ) >> h2_tracks_tau",select_tracks_tau);
  h2_tracks_tau->Scale( 1. / h2_tracks_tau->Integral() );

  TH2F* h2_tracks_uds = new TH2F("h2_tracks_uds", ";#Delta#phi;#Delta#eta;N/#SigmaN", 40, -0.2, 0.2, 40, -0.2, 0.2 );
  h2_tracks_uds->SetLineColor( col1 );
  h2_tracks_uds->GetXaxis()->SetNdivisions(505);
  h2_tracks_uds->GetYaxis()->SetNdivisions(505);
  h2_tracks_uds->GetZaxis()->SetNdivisions(505);
  h2_tracks_uds->GetZaxis()->SetTitleOffset(1.35);
  chain_track.Draw("( track_eta - jet_eta ) : ( track_phi - jet_phi ) >> h2_tracks_uds",select_tracks_uds);
  h2_tracks_uds->Scale( 1. / h2_tracks_uds->Integral() );

  /* plot histograms */
  TCanvas *c1 = new TCanvas();
  h2_tracks_tau->Draw("lego");
  c1->Print("plots/tracks_2d_tau.eps");
  c1->Print("plots/tracks_2d_tau.png");

  TCanvas *c2 = new TCanvas();
  h2_tracks_uds->Draw("lego");
  c2->Print("plots/tracks_2d_uds.eps");
  c2->Print("plots/tracks_2d_uds.png");
}
