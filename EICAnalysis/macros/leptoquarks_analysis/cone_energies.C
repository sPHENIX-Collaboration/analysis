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
  TChain chain("candidates");
  chain.Add("data_3pions/p250_e20_0events_file1093_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1096_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1101_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1115_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1122_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1127_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1131_LeptoAna_r05.root");
  chain.Add("data_3pions/p250_e20_0events_file1164_LeptoAna_r05.root");

  /* Create temporary canvas */
  TCanvas* ctemp = new TCanvas();

  ctemp->cd();

  TH1F* h_uds = new TH1F( "prof_uds", "", 5, 0, 0.5);
  h_uds->GetXaxis()->SetNdivisions(505);
  TH1F* h_tau = new TH1F( "prof_tau", "", 5, 0, 0.5);

  for ( int r = 1; r <= 5; r++ )
    {
      /* particle selection */
      TCut select_true_uds( Form( "jetshape_econe_r0%d * ( evtgen_is_uds==1 && abs(evtgen_uds_eta)<0.8 )", r ) );
      TCut select_true_tau( Form( "jetshape_econe_r0%d * ( evtgen_is_tau==1 && evtgen_tau_decay_prong==3 && abs(evtgen_tau_eta)<0.8 )", r ) );

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
