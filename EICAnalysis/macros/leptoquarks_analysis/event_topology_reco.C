#include "tau_commons.h"

/**
 * Plot missing trasnverse energy and other event-wise observables
 *
 * Written by nils.feege@stonybrook.edu
 */
int event_topology_reco()
{
  gStyle->SetOptStat(0);

  unsigned col1 = kOrange+7;
  unsigned col2 = kBlue+2;

  /* open inout files and merge trees */
  TChain chain("event");
  chain.Add("data_3pions/p250_e20_1000events_file1_LeptoAna_r05.root");

  /* Create temporary canvas */
  TCanvas* ctemp = new TCanvas();

  TH1F* h_ptmiss =  new TH1F( "h_ptmiss", ";p_{T}^{miss} (GeV);# entries / #Sigma entries",  25, 0, 50 );
  chain.Draw( "Et_miss >> h_ptmiss" , "","");
  h_ptmiss->Scale(1./h_ptmiss->Integral());
  //h_ptmiss->GetYaxis()->SetRangeUser(0, 1);
  h_ptmiss->SetLineColor(col2);
  h_ptmiss->SetFillColor(col2);
  h_ptmiss->SetFillStyle(1001);

  TH1F* h_acoplan = new TH1F( "h_acoplan", ";#Delta#phi_{miss - #tau jet};# entries / #Sigma entries", 45, 0, 180 );
  chain.Draw( "abs(Et_miss_phi - reco_tau_phi) / TMath::Pi() * 180.  >> h_acoplan" ,"" , "");
  h_acoplan->Scale(1./h_acoplan->Integral());
  //h_acoplan->GetYaxis()->SetRangeUser(0, 1);
  h_acoplan->SetLineColor(col2);
  h_acoplan->SetFillColor(col2);
  h_acoplan->SetFillStyle(1001);

  /* create Canvas and draw histograms for pt_miss */
  TCanvas *c1 = new TCanvas();

  h_ptmiss->Draw("");
  gPad->RedrawAxis();

  c1->Print( "plots/event_topology_reco_ptmiss.eps" );
  c1->Print( "plots/event_topology_reco_ptmiss.png" );

  /* create Canvas and draw histograms for acoplanarity */
  TCanvas *c2 = new TCanvas();

  h_acoplan->Draw("");
  gPad->RedrawAxis();

  c2->Print( "plots/event_topology_reco_acoplanarity.eps" );
  c2->Print( "plots/event_topology_reco_acoplanarity.png" );

  return 0;
}
