/**
 * Compare reconstructed observables between tau jets and quark jets
 *
 * Written by nils.feege@stonybrook.edu
 */

int true_kinematics()
{
  gStyle->SetOptStat(0);

  /* open inout files and merge trees */
  TChain chain("ntp_event");
  chain.Add("data/p250_e20_1000events_file1_LeptoAna_TruthEvent.root");
  chain.Add("data/p250_e20_1000events_file2_LeptoAna_TruthEvent.root");
  chain.Add("data/p250_e20_1000events_file3_LeptoAna_TruthEvent.root");
  chain.Add("data/p250_e20_1000events_file4_LeptoAna_TruthEvent.root");
  chain.Add("data/p250_e20_1000events_file5_LeptoAna_TruthEvent.root");
  chain.Add("data/p250_e20_1000events_file6_LeptoAna_TruthEvent.root");
  chain.Add("data/p250_e20_1000events_file7_LeptoAna_TruthEvent.root");
  chain.Add("data/p250_e20_1000events_file8_LeptoAna_TruthEvent.root");
  chain.Add("data/p250_e20_1000events_file9_LeptoAna_TruthEvent.root");
  chain.Add("data/p250_e20_1000events_file10_LeptoAna_TruthEvent.root");

  TH2F* h2_tau = new TH2F( "h2_tau", "Tau;#eta;p (GeV)", 50, -3, 3, 50, 0, 100 );
  TH2F* h2_uds = new TH2F( "h2_uds", "Tau;#eta;p (GeV)", 50, -3, 3, 50, 0, 100 );
  TH2F* h2_cor_eta = new TH2F( "h2_cor_eta", ";#eta (tau);#eta (quark)", 50, -3, 3, 50, -3, 3 );
  TH2F* h2_cor_phi = new TH2F( "h2_cor_phi", ";#phi (tau);#phi (quark)", 50, -3.14, 3.14, 50, -3.14, 3.14 );

  /* Fill histograms */
  chain.Draw( "tau_p : tau_eta >> h2_tau" );
  h2_tau->GetYaxis()->SetNdivisions(505);

  chain.Draw( "uds_p : uds_eta >> h2_uds" );
  h2_uds->GetYaxis()->SetNdivisions(505);

  chain.Draw( "uds_eta : tau_eta >> h2_cor_eta" );
  h2_cor_eta->GetYaxis()->SetNdivisions(505);

  chain.Draw( "uds_phi : tau_phi >> h2_cor_phi", "" );
  //  chain.Draw( "uds_phi : tau_phi >> h2_cor_phi", "(tau_phi - uds_phi) >= 0" );
  //  chain.Draw( "uds_phi+3.14 : tau_phi+3.14 >>+ h2_cor_phi", "(tau_phi - uds_phi) < 0" );
  h2_cor_phi->GetYaxis()->SetNdivisions(505);

  /* create Canvas and draw histograms */
  TCanvas *c1 = new TCanvas();
  h2_tau->Draw("colz");
  gPad->RedrawAxis();

  TCanvas *c2 = new TCanvas();
  h2_uds->Draw("colz");
  gPad->RedrawAxis();

  TCanvas *c3 = new TCanvas();
  h2_cor_eta->Draw("colz");
  gPad->RedrawAxis();

  TCanvas *c3 = new TCanvas();
  h2_cor_phi->Draw("colz");
  gPad->RedrawAxis();

  return 0;
}
