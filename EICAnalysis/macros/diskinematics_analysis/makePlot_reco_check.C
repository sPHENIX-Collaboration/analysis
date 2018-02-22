int
makePlot_reco_check()
{
  gStyle->SetOptStat(1);

  TFile *fin = new TFile("../../data/Sample_DISReco_ep.root", "OPEN");
  //TFile *fin = new TFile("../../data/DISReco_Pythia6_DIS_20x250_1k.root", "OPEN");

  TTree *t_reco = (TTree*)fin->Get("event_cluster");

  TCanvas *c1 = new TCanvas();
  t_reco->Draw( "em_cluster_e:em_cluster_eta", "em_evtgen_pid==11", "colz" );
  htemp->GetXaxis()->SetTitle("#eta_{cluster}");
  htemp->GetYaxis()->SetTitle("E_{cluster} (GeV)");
  c1->Print("plots/reco_check_c1.eps");

  TCanvas *c2 = new TCanvas();
  t_reco->Draw( "em_evtgen_ptotal:em_evtgen_eta", "em_evtgen_pid==11", "colz" );
  htemp->GetXaxis()->SetTitle("#eta_{truth}");
  htemp->GetYaxis()->SetTitle("|p_{truth}| (GeV)");
  c2->Print("plots/reco_check_c2.eps");

  TCanvas *c3 = new TCanvas();
  t_reco->Draw( "em_cluster_eta:em_evtgen_eta", "em_evtgen_pid==11", "colz" );
  htemp->GetXaxis()->SetTitle("#eta_{truth}");
  htemp->GetYaxis()->SetTitle("#eta_{cluster}");
  c3->Print("plots/reco_check_c3.eps");

  TCanvas *c4 = new TCanvas();
  t_reco->Draw( "em_cluster_e:em_evtgen_ptotal", "em_evtgen_pid==11", "colz" );
  htemp->GetXaxis()->SetTitle("|p_{truth}| (GeV)");
  htemp->GetYaxis()->SetTitle("E_{cluster} (GeV)");
  c4->Print("plots/reco_check_c4.eps");

  TCanvas *c5 = new TCanvas();
  t_reco->Draw( "em_cluster_phi:em_evtgen_phi", "em_evtgen_pid==11", "colz" );
  htemp->GetXaxis()->SetTitle("#phi_{truth}");
  htemp->GetYaxis()->SetTitle("#phi_{cluster}");
  c5->Print("plots/reco_check_c5.eps");

  return 1;
}
