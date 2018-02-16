int makePlot_truth_EtaDistribution(
				   const TString infile = "../../data/Sample_DISReco_ep.root",
				   //const TString infile = "../../data/EventGenAna_Pythia6_DIS_10x250_100k.root",
				   const bool save_figures = true
                                   )
{
  gStyle->SetOptStat(kFALSE);

  TFile *f_truth = new TFile( infile, "OPEN");
  TTree* t_truth = (TTree*)f_truth->Get("event_truth");
  t_truth->Print();

  TCut electron_cut = "em_evtgen_pid == 11";
  TCut Pion_cut = "abs( em_evtgen_pid ) == 211";

  TH1F *h_eta = new TH1F("h_eta", "", 60, -6, 6);
  TH1F *h_eta_e = (TH1F*)h_eta->Clone();

  TCanvas *c_eta = new TCanvas("c_eta");

  t_truth->Draw("-1 * log( tan( em_evtgen_theta / 2.0 ) ) >> h_eta", Pion_cut);
  h_eta->SetLineStyle(2);
  t_truth->Draw("-1 * log( tan( em_evtgen_theta / 2.0 ) ) >> h_eta_e", electron_cut, "sames");

  h_eta->GetXaxis()->SetTitle("Pseudorapidity #eta");
  h_eta->GetYaxis()->SetTitle("dN/d#eta");

  TLegend* leg_eta = new TLegend( 0.25, 0.70, 0.45, 0.90);
  leg_eta->AddEntry(h_eta, "Charged Pions", "L");
  leg_eta->AddEntry(h_eta_e, "Electrons", "L");
  leg_eta->Draw();

  if ( save_figures )
    {
      c_eta->Print("plots/truth_EtaDistribution.eps");
      c_eta->Print("plots/truth_EtaDistribution.png");
    }

  return 0;
}
