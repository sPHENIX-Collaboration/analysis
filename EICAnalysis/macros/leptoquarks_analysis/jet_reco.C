jet_reco()
{
  gStyle->SetOptStat(0);

  unsigned col1 = kOrange+7;
  unsigned col2 = kBlue+2;

  /* open inout files and merge trees */
  TChain chain_r05("ntp_recojet");
  chain_r05.Add("data_3pions/p250_e20_0events_file1093_jeteval_r05.root");
  chain_r05.Add("data_3pions/p250_e20_0events_file1096_jeteval_r05.root");
  chain_r05.Add("data_3pions/p250_e20_0events_file1101_jeteval_r05.root");
  chain_r05.Add("data_3pions/p250_e20_0events_file1115_jeteval_r05.root");
  chain_r05.Add("data_3pions/p250_e20_0events_file1122_jeteval_r05.root");
  chain_r05.Add("data_3pions/p250_e20_0events_file1127_jeteval_r05.root");
  chain_r05.Add("data_3pions/p250_e20_0events_file1131_jeteval_r05.root");
  chain_r05.Add("data_3pions/p250_e20_0events_file1164_jeteval_r05.root");

  TChain chain_r10("ntp_recojet");
  chain_r10.Add("data_3pions/p250_e20_0events_file1093_jeteval_r10.root");
  chain_r10.Add("data_3pions/p250_e20_0events_file1096_jeteval_r10.root");
  chain_r10.Add("data_3pions/p250_e20_0events_file1101_jeteval_r10.root");
  chain_r10.Add("data_3pions/p250_e20_0events_file1115_jeteval_r10.root");
  chain_r10.Add("data_3pions/p250_e20_0events_file1122_jeteval_r10.root");
  chain_r10.Add("data_3pions/p250_e20_0events_file1127_jeteval_r10.root");
  chain_r10.Add("data_3pions/p250_e20_0events_file1131_jeteval_r10.root");
  chain_r10.Add("data_3pions/p250_e20_0events_file1164_jeteval_r10.root");

  /* Jet selection */
  TCut select_jet( "abs(eta) < 1 && pt > 5 && ge < 100" );

  /* Jet energy */
  TH1F* h1_jet_r05_e = new TH1F("h1_jet_r05_e", ";(E_{reco} - E_{true}) / E_{true};", 100, -1, 1 );
  h1_jet_r05_e->GetXaxis()->SetNdivisions(505);
  h1_jet_r05_e->GetYaxis()->SetNdivisions(505);
  h1_jet_r05_e->SetLineColor(col2);

  TH1F* h1_jet_r10_e = (TH1F*)h1_jet_r05_e->Clone("h1_jet_r10_e");
  h1_jet_r10_e->SetLineColor(col1);

  cout << ">>>>> Jet energy: R=0.5" << endl;
  chain_r05.Draw("(e-ge) / ge >> h1_jet_r05_e", select_jet );
  h1_jet_r05_e->Scale( 1. / h1_jet_r05_e->Integral() );
  h1_jet_r05_e->Fit("gaus");

  cout << ">>>>> Jet energy: R=1.0" << endl;
  chain_r10.Draw("(e-ge) / ge >> h1_jet_r10_e", select_jet );
  h1_jet_r10_e->Scale( 1. / h1_jet_r10_e->Integral() );
  h1_jet_r10_e->Fit("gaus");

  /* Jet eta */
  TH1F* h1_jet_r05_eta = new TH1F("h1_jet_r05_eta", ";#eta_{reco}-#eta_{true};", 100, -0.25, 0.25 );
  h1_jet_r05_eta->GetXaxis()->SetNdivisions(505);
  h1_jet_r05_eta->GetYaxis()->SetNdivisions(505);
  h1_jet_r05_eta->SetLineColor(col2);

  TH1F* h1_jet_r10_eta = (TH1F*)h1_jet_r05_eta->Clone("h1_jet_r10_eta");
  h1_jet_r10_eta->SetLineColor(col1);

  cout << ">>>>> Jet eta: R=0.5" << endl;
  chain_r05.Draw("( eta - geta ) >> h1_jet_r05_eta", select_jet );
  h1_jet_r05_eta->Scale( 1. / h1_jet_r05_eta->Integral() );
  h1_jet_r05_eta->Fit("gaus");

  cout << ">>>>> Jet eta: R=1.0" << endl;
  chain_r10.Draw("( eta - geta ) >> h1_jet_r10_eta", select_jet );
  h1_jet_r10_eta->Scale( 1. / h1_jet_r10_eta->Integral() );
  h1_jet_r10_eta->Fit("gaus");

  /* Jet phi */
  TH1F* h1_jet_r05_phi = new TH1F("h1_jet_r05_phi", ";#phi_{reco}-#phi_{true};", 100, -0.25, 0.25 );
  h1_jet_r05_phi->GetXaxis()->SetNdivisions(505);
  h1_jet_r05_phi->GetYaxis()->SetNdivisions(505);
  h1_jet_r05_phi->SetLineColor(col2);

  TH1F* h1_jet_r10_phi = (TH1F*)h1_jet_r05_phi->Clone("h1_jet_r10_phi");
  h1_jet_r10_phi->SetLineColor(col1);

  cout << ">>>>> Jet phi: R=0.5" << endl;
  chain_r05.Draw("( phi - gphi ) >> h1_jet_r05_phi", select_jet );
  h1_jet_r05_phi->Scale( 1. / h1_jet_r05_phi->Integral() );
  h1_jet_r05_phi->Fit("gaus");

  cout << ">>>>> Jet phi: R=1.0" << endl;
  chain_r10.Draw("( phi - gphi ) >> h1_jet_r10_phi", select_jet );
  h1_jet_r10_phi->Scale( 1. / h1_jet_r10_phi->Integral() );
  h1_jet_r10_phi->Fit("gaus");

  /* plot histograms */
  TCanvas *c1 = new TCanvas();
  h1_jet_r05_e->Draw("");
  h1_jet_r10_e->Draw("same");
  c1->Print("plots/jet_reco_escale.eps");
  c1->Print("plots/jet_reco_escale.png");

  TCanvas *c2 = new TCanvas();
  h1_jet_r05_eta->Draw("");
  h1_jet_r10_eta->Draw("same");
  c2->Print("plots/jet_reco_eta.eps");
  c2->Print("plots/jet_reco_eta.png");

  TCanvas *c3 = new TCanvas();
  h1_jet_r05_phi->Draw("");
  h1_jet_r10_phi->Draw("same");
  c3->Print("plots/jet_reco_phi.eps");
  c3->Print("plots/jet_reco_phi.png");

  TCanvas *c6 = new TCanvas();
  chain_r05.Draw("e : ge", select_jet, "colz");

  TCanvas *c4 = new TCanvas();
  chain_r05.Draw("eta : geta", select_jet, "colz");

  TCanvas *c5 = new TCanvas();
  chain_r05.Draw("phi : gphi", select_jet, "colz");

}
