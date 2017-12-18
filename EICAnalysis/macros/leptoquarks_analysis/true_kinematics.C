/**
 * Compare reconstructed observables between tau jets and quark jets
 *
 * Written by nils.feege@stonybrook.edu
 */

int true_kinematics()
{
  gStyle->SetOptStat(0);

  unsigned col1 = kOrange+7;
  unsigned col2 = kBlue+2;

  /* open inout files and merge trees */
  TChain chain("ntp_event");
  chain.Add("data_3pions/p250_e20_0events_file1093_LeptoAna_TruthEvent.root");
  chain.Add("data_3pions/p250_e20_0events_file1096_LeptoAna_TruthEvent.root");
  chain.Add("data_3pions/p250_e20_0events_file1101_LeptoAna_TruthEvent.root");
  chain.Add("data_3pions/p250_e20_0events_file1115_LeptoAna_TruthEvent.root");
  chain.Add("data_3pions/p250_e20_0events_file1122_LeptoAna_TruthEvent.root");
  chain.Add("data_3pions/p250_e20_0events_file1127_LeptoAna_TruthEvent.root");
  chain.Add("data_3pions/p250_e20_0events_file1131_LeptoAna_TruthEvent.root");
  chain.Add("data_3pions/p250_e20_0events_file1164_LeptoAna_TruthEvent.root");

  TH2F* h2_tau = new TH2F( "h2_tau", "Tau;#eta;p (GeV)", 50, -3, 3, 50, 0, 100 );
  TH2F* h2_uds = new TH2F( "h2_uds", "Tau;#eta;p (GeV)", 50, -3, 3, 50, 0, 100 );
  TH2F* h2_cor_eta = new TH2F( "h2_cor_eta", ";#eta (tau);#eta (quark)", 50, -3, 3, 50, -3, 3 );
  TH2F* h2_cor_phi = new TH2F( "h2_cor_phi", ";#phi (tau);#phi (quark)", 50, -3.14, 3.14, 50, -3.14, 3.14 );

  /* Fill histograms */
  chain.Draw( "tau_p : tau_eta >> h2_tau" );
  h2_tau->GetYaxis()->SetNdivisions(505);
  h2_tau->GetZaxis()->SetNdivisions(505);
  h2_tau->Scale(1./ h2_tau->GetEntries() );

  chain.Draw( "uds_p : uds_eta >> h2_uds" );
  h2_uds->GetYaxis()->SetNdivisions(505);

  chain.Draw( "uds_eta : tau_eta >> h2_cor_eta" );
  h2_cor_eta->GetYaxis()->SetNdivisions(505);

  chain.Draw( "uds_phi : tau_phi >> h2_cor_phi", "" );
  //  chain.Draw( "uds_phi : tau_phi >> h2_cor_phi", "(tau_phi - uds_phi) >= 0" );
  //  chain.Draw( "uds_phi+3.14 : tau_phi+3.14 >>+ h2_cor_phi", "(tau_phi - uds_phi) < 0" );
  h2_cor_phi->GetYaxis()->SetNdivisions(505);

  /* Print some stats */
  cout << "Total events in TChain: " << chain.GetEntries() << endl;
  cout << "Total events in h2_tau: " << h2_tau->GetEntries() << endl;
  cout << "Total events within |eta|<1.0: " << chain.GetEntries("abs(tau_eta)<1.0") << endl;

  /* create Canvas and draw histograms */
  TCanvas *c1 = new TCanvas( "c1", h2_tau->GetTitle() );
  c1->SetRightMargin(0.14);
  h2_tau->Draw("colz");
  gPad->RedrawAxis();
  c1->Print("plots/true_kinematics_tau_eta.eps");
  c1->Print("plots/true_kinematics_tau_eta.png");

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
