
int makePlot_EtaDistribution(
			     const TString infile = "data/pythia.ep.100.test.root"
			     )
{
  gStyle->SetOptStat(kFALSE);

  TFile *f_pyth = new TFile( infile, "OPEN");
  TTree* T_pyth = (TTree*)f_pyth->Get("tree");
  tree->Print();

  TCut electron_cut = "p.fKF == 11";
  TCut Pion_cut = "abs(p.fKF) == 211 || p.fKF == 111";

  TH1F *h_eta = new TH1F("h_eta", "", 60, -6, 6);
  TH1F *h_eta_e = (TH1F*)h_eta->Clone();
  TCanvas *c_eta = new TCanvas("c_eta");

  tree->Draw("TMath::ASinH(p.fPz/sqrt(((p.fPx)**2 + (p.fPy)**2)))>>h_eta", Pion_cut);
  h_eta->SetLineStyle(2);
  tree->Draw("TMath::ASinH(p.fPz/sqrt(((p.fPx)**2 + (p.fPy)**2)))>>h_eta_e", electron_cut && "p.fParent == 0", "same");

  h_eta->GetXaxis()->SetTitle("Pseudorapidity #eta");
  h_eta->GetYaxis()->SetTitle("dN/d#eta");

  TLegend* leg_eta = new TLegend( 0.25, 0.70, 0.45, 0.90);
  leg_eta->AddEntry(h_eta, "Pions", "L");
  leg_eta->AddEntry(h_eta_e, "Electrons", "L");
  leg_eta->Draw();

  c_eta->Print("EtaDistributions.eps");
  c_eta->Print("EtaDistributions.png");

  return 0;
}
