/**
 * Plot energies in cones around jet axis
 *
 * Written by nils.feege@stonybrook.edu
 */

int cone_energies()
{
  gStyle->SetOptStat(0);

  unsigned col1 = kOrange+7;
  unsigned col2 = kBlue+2;

  /* open inout files and merge trees */
  TChain chain("ntp_jet2");
  chain.Add("data/p250_e20_1000events_file1_LeptoAna_r05.root");
//  chain.Add("data/p250_e20_1000events_file2_LeptoAna_r05.root");
//  chain.Add("data/p250_e20_1000events_file3_LeptoAna_r05.root");
//  chain.Add("data/p250_e20_1000events_file4_LeptoAna_r05.root");
//  chain.Add("data/p250_e20_1000events_file5_LeptoAna_r05.root");
//  chain.Add("data/p250_e20_1000events_file6_LeptoAna_r05.root");
//  chain.Add("data/p250_e20_1000events_file7_LeptoAna_r05.root");
//  chain.Add("data/p250_e20_1000events_file8_LeptoAna_r05.root");
//  chain.Add("data/p250_e20_1000events_file9_LeptoAna_r05.root");
//  chain.Add("data/p250_e20_1000events_file10_LeptoAna_r05.root");

  /* Create temporary canvas */
  TCanvas* ctemp = new TCanvas();

  ctemp->cd();

  TH1F* h_uds = new TH1F( "prof_uds", "", 10, 0, 1);
  TH1F* h_tau = new TH1F( "prof_tau", "", 10, 0, 1);

  for ( int r = 1; r <= 10; r++ )
    {
      /* particle selection */
      TCut select_true_uds( Form( "jetshape_econe_r%d * ( is_uds==1 && abs(uds_eta)<0.8 )", r ) );
      TCut select_true_tau( Form( "jetshape_econe_r%d * ( is_tau==1 && tau_decay_prong==3 && abs(tau_eta)<0.8 )", r ) );

      chain.Draw( Form("0.9*(%f) >>+ prof_uds", r*0.1), select_true_uds );
      chain.Draw( Form("0.9*(%f) >>+ prof_tau", r*0.1), select_true_tau );
    }

  h_uds->Scale(1./h_uds->Integral());
  h_uds->GetXaxis()->SetTitle( "#DeltaR" );
  h_uds->GetYaxis()->SetRangeUser(0, 0.5);
  h_uds->SetLineColor(col1);
  h_uds->SetFillColor(col1);
  h_uds->SetFillStyle(1001);

  h_tau->Scale(1./h_tau->Integral());
  h_tau->SetLineColor(col2);
  h_tau->SetFillColor(col2);
  h_tau->SetFillStyle(0);

  /* create Canvas and draw histograms */
  TCanvas *c1 = new TCanvas();

  h_uds->DrawClone("");
  h_tau->DrawClone("sames");

  gPad->RedrawAxis();

  c1->Print("plots/cone_energies.eps");
  c1->Print("plots/cone_energies.png");

  return 0;
}
