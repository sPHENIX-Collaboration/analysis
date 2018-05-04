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

  string type = "DISneutral";
  string nevents = "100";
  string seed[9] = {"1","2","3","4","5","6","7","8","9"};
  //string seed[1] = {"1"};
  string R_max = "5";
  /* open inout files and merge trees */
  TChain chain("event");
  
  for (int g = 0; g<9;g++){
    //if(g==0) continue;
    string file = "/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/data/LeptoAna_p250_e20_"+nevents+"events_"+seed[g]+"seed_"+type+"_tau_r0"+R_max+".root";
    chain.Add(file.c_str());
  }
  
  //  chain.Add("/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/data/LeptoAna_p250_e20_100events_1seed_DIScharged_tau_r05.root");
 
  TH2F *h1 = new TH2F("h1","",100,0,40,100,0,80);

  /* Create temporary canvas */
  TCanvas* ctemp = new TCanvas();

  TH1F* h_ptmiss =  new TH1F( "h_ptmiss", ";p_{T}^{miss} (GeV);# entries / #Sigma entries",  25, 0, 50 );
  int n = chain.Draw( "Et_miss >> h_ptmiss" , "","");
  h_ptmiss->Scale(1./h_ptmiss->Integral());
  //h_ptmiss->GetYaxis()->SetRangeUser(0, 1);
  h_ptmiss->SetLineColor(col2);
  h_ptmiss->SetFillColor(col2);
  h_ptmiss->SetFillStyle(1001);
  h_ptmiss->GetYaxis()->SetTitleOffset(1.6);

  TH1F* h_acoplan = new TH1F( "h_acoplan", ";#Delta#phi_{miss - #tau jet};# entries / #Sigma entries", 45, 0, 180 );
  //TH1F* h_acoplan = new TH1F( "h_acoplan", ";#Delta#phi_{miss};# entries / #Sigma entries", 45, -2, 2 );
  //chain.Draw( "abs(Et_miss_phi - reco_tau_phi) / TMath::Pi() * 180.  >> h_acoplan" ,"Et_miss_phi*reco_tau_phi > 0" , "");
  //chain.Draw( "Et_miss_phi >> h_acoplan" ,"" , "");
  
  float Et_miss_phi, Et_miss;
  float tau_phi, tau_eta, tau_ptotal, nu_pt;
  
  chain.SetBranchAddress("Et_miss",&Et_miss);
  chain.SetBranchAddress("Et_miss_phi",&Et_miss_phi);
  chain.SetBranchAddress("reco_tau_phi",&tau_phi);
  chain.SetBranchAddress("reco_tau_eta",&tau_eta);
  chain.SetBranchAddress("reco_tau_ptotal",&tau_ptotal);
  chain.SetBranchAddress("neutrino_pt",&nu_pt);

  for(int i = 0; i<n;i++){
    double delta_phi;
    
    chain.GetEntry(i);
    delta_phi = fabs(Et_miss_phi - tau_phi) * 180 / TMath::Pi();
    if(delta_phi > 180) delta_phi = 360 - delta_phi; 
    if(delta_phi < 10)  
    h1->Fill(nu_pt,tau_ptotal);
    h_acoplan->Fill(delta_phi);
  }

  h_acoplan->Scale(1./h_acoplan->Integral());
  //h_acoplan->GetYaxis()->SetRangeUser(0, 1);
  h_acoplan->SetLineColor(col2);
  h_acoplan->SetFillColor(col2);
  h_acoplan->SetFillStyle(1001);
  h_acoplan->GetYaxis()->SetTitleOffset(1.6);
  h_acoplan->SetTitle("Neutral Current Tau");

  /* create Canvas and draw histograms for pt_miss */
  TCanvas *c1 = new TCanvas();

  h_ptmiss->Draw("");
  h_ptmiss->SetTitle("Neutral Current Tau");
  gPad->RedrawAxis();

  //c1->Print( "plots/event_topology_reco_ptmiss.eps" );
  //c1->Print( "plots/event_topology_reco_ptmiss.png" );

  /* create Canvas and draw histograms for acoplanarity */
  TCanvas *c2 = new TCanvas();

  h_acoplan->Draw("");
  gPad->RedrawAxis();

  TCanvas *c3 = new TCanvas();
  //chain.Draw("jet_eta","","");
  //h1->Draw("colz");
  //h1->GetXaxis()->SetTitle("#Delta#phi_{miss - #tau jet}");
  //h1->GetYaxis()->SetTitle("p_{T}^{Jet}");
  
  //c2->Print( "plots/event_topology_reco_acoplanarity.eps" );
  //c2->Print( "plots/event_topology_reco_acoplanarity.png" );

  return 0;
}
