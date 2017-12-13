tracks()
{
  gStyle->SetOptStat(0);

  unsigned col1 = kOrange+7;
  unsigned col2 = kBlue+2;

  /* open inout files and merge trees */
  TChain chain_track("ntp_track");
  chain_track.Add("data/p250_e20_1000events_file1_LeptoAna_r05.root");

  TH2F* h2_tracks_tau = new TH2F("h2_tracks_tau", ";#Delta#phi;#Delta#eta;N/#SigmaN", 50, -0.5, 0.5, 50, -0.5, 0.5 );
  h2_tracks_tau->SetLineColor( col1 );
  h2_tracks_tau->GetXaxis()->SetNdivisions(505);
  h2_tracks_tau->GetYaxis()->SetNdivisions(505);
  h2_tracks_tau->GetZaxis()->SetNdivisions(505);
  h2_tracks_tau->GetZaxis()->SetTitleOffset(1.35);
  chain_track.Draw("( track_eta - tau_eta ) : ( track_phi - tau_phi ) >> h2_tracks_tau","1 * (is_tau==1 && tau_decay_prong==3)");
  h2_tracks_tau->Scale( 1. / h2_tracks_tau->Integral() );

  TH2F* h2_tracks_uds = new TH2F("h2_tracks_uds", ";#Delta#phi;#Delta#eta;N/#SigmaN", 50, -0.5, 0.5, 50, -0.5, 0.5 );
  h2_tracks_uds->SetLineColor( col2 );
  h2_tracks_uds->GetXaxis()->SetNdivisions(505);
  h2_tracks_uds->GetYaxis()->SetNdivisions(505);
  h2_tracks_uds->GetZaxis()->SetNdivisions(505);
  h2_tracks_uds->GetZaxis()->SetTitleOffset(1.35);
  chain_track.Draw("( track_eta - uds_eta ) : ( track_phi - uds_phi ) >> h2_tracks_uds","1 * (is_uds==1)");
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
