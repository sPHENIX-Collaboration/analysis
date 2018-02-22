/**

 *\Brief Macro to reproduce ePHENIX LoI Feb. 2014 (arXiv:1402.1209v1) Fig. 2.1, 2.2, & 2.4
 *\Author: Thomas Krahulik <thomas.krahulik@stonybrook.edu>

 **/

int makePlot_truth_LoI_Momentum(
				const TString infile = "../../data/Sample_DISReco_ep.root",
				//const TString infile = "../../data/EventGenAna_Pythia6_DIS_10x250_100k.root",
				const bool save_figures = true
				)
{
  gStyle->SetOptStat(kFALSE);

  /*--------------Get Input File--------------*/

  TFile *f_truth = new TFile( infile, "OPEN");
  TTree* t_truth = (TTree*)f_truth->Get("event_truth");
  t_truth->Print();

  /*------------------------------------------*/
  /*--------------Define Cuts-----------------*/

  //TCut Q2_cut = "evtgen_Q2 > 1";

  TCut electron_cut = "em_evtgen_pid == 11";
  TCut hadron_cut = "abs(em_evtgen_pid) > 100";
  TCut proton_cut = "em_evtgen_pid == 2212";
  TCut neutron_cut = "em_evtgen_pid == 2112";
  TCut Kaon_cut = "abs(em_evtgen_pid) == 321 || em_evtgen_pid == 311";
  TCut Pion_charged_cut = "abs(em_evtgen_pid) == 211";
  //TCut Pion_cut = "abs(em_evtgen_pid) == 211 || em_evtgen_pid == 111";
  TCut photon_cut = "em_evtgen_pid == 22";

  TCut mother_cut = "1"; //"p.fParent == 0";
  TCut status_cut = "1"; //"p.fKS < 10";

  TCut eta_cut_n3n2 = "(-1 * log( tan( em_evtgen_theta / 2.0 ) )) > -3 && (-1 * log( tan( em_evtgen_theta / 2.0 ) )) < -2";
  TCut eta_cut_n2n1 = "(-1 * log( tan( em_evtgen_theta / 2.0 ) )) > -2 && (-1 * log( tan( em_evtgen_theta / 2.0 ) )) < -1";
  TCut eta_cut_n1z0 = "(-1 * log( tan( em_evtgen_theta / 2.0 ) )) > -1 && (-1 * log( tan( em_evtgen_theta / 2.0 ) )) < -0";

  /*------------------------------------------*/
  /*-------Momentum vs. Pseudorapidity--------*/
  /*------------------------------------------*/

  /*------------------------------------------*/
  /*---------Electrons (LoI Fig. 2-1)---------*/
  /*------------------------------------------*/
  TH2F *h_peta_e = new TH2F("h_peta_e", "", 100, -4, 3, 100, 0, 50 ); //250x015 10M

  TCanvas *c_peta_e = new TCanvas( "c_peta_e" );
  c_peta_e->SetLogz();

  t_truth->Draw("em_evtgen_ptotal:(-1 * log( tan( em_evtgen_theta / 2.0 ) ))>>h_peta_e", electron_cut && mother_cut && "evtgen_Q2 > 1", "colz");

  h_peta_e->GetXaxis()->SetTitle("Pseudorapidity #eta");
  h_peta_e->GetYaxis()->SetTitle("Electron Momentum p_{e-} [GeV]");

  if ( save_figures )
    {
      c_peta_e->Print("plots/Pythia_peta_e.eps");
      c_peta_e->Print("plots/Pythia_peta_e.png");
    }

  /*------------------------------------------*/
  /*----------Hadrons (LoI Fig. 2-4)----------*/
  /*------------------------------------------*/
  TH2F *h_p_eta_h = new TH2F("h_p_eta_h", "", 60,-6,6, 200,0,225); //250x015 10M

  TCanvas *c_p_eta_h = new TCanvas( "c_p_eta_h" );
  t_truth->Draw("em_evtgen_ptotal:(-1 * log( tan( em_evtgen_theta / 2.0 ) ))>>h_p_eta_h",hadron_cut && "evtgen_Q2 > 1 && 0.01 < evtgen_y < 0.80", "colz");

  h_p_eta_h->GetXaxis()->SetTitle("Pseudorapidity #eta");
  h_p_eta_h->GetYaxis()->SetTitle("Hadron Momentum p_{Hadron} [GeV]");
  c_p_eta_h->SetLogz();

  if ( save_figures )
    {
      c_p_eta_h->Print("plots/Pythia_peta_h.eps");
      c_p_eta_h->Print("plots/Pythia_peta_h.png");
    }

  /*------------Momentum Spectra (Figure 2.2)--------------*/

  /*-------- -3 < Eta < -2 ---------*/

  TH1F* hp_e_n3n2 = new TH1F("hp_e_n3n2", "dN/dp vs. p", 60, 0, 30);
  TH1F* hp_p_n3n2 = new TH1F("hp_p_n3n2", "dN/dp vs. p", 60, 0, 30);
  TH1F* hp_y_n3n2 = new TH1F("hp_y_n3n2", "dN/dp vs. p", 60, 0, 30);

  TCanvas *cp_e_n3n2 = new TCanvas("cp_e_n3n2");
  t_truth->Draw("em_evtgen_ptotal>>hp_e_n3n2", electron_cut && eta_cut_n3n2 && "evtgen_Q2 > 0.01", "goff");
  t_truth->Draw("em_evtgen_ptotal>>hp_p_n3n2", Pion_charged_cut && eta_cut_n3n2 && "evtgen_Q2 > 0.01", "goff");
  t_truth->Draw("em_evtgen_ptotal>>hp_y_n3n2", photon_cut && eta_cut_n3n2 && status_cut && "evtgen_Q2 > 0.01", "goff");


  TH1F* htmp_n3n2 = hp_e_n3n2->Clone();
  htmp_n3n2->SetTitle("");
  htmp_n3n2->GetXaxis()->SetTitle("p [GeV]");
  htmp_n3n2->GetYaxis()->SetTitle("dN/dp");
  htmp_n3n2->SetMaximum( 0.99e7);
  htmp_n3n2->Draw();

  hp_e_n3n2->SetLineColor(2);
  hp_e_n3n2->Draw("same");
  hp_p_n3n2->SetLineColor(1);
  hp_p_n3n2->Draw("same");
  hp_y_n3n2->SetLineColor(4);
  hp_y_n3n2->Draw("same");
  cp_e_n3n2->SetLogy();

  TLegend* leg_n3n2 = new TLegend(0.53,0.67,0.73,0.90);
  hp_e_n3n2->SetTitle("DIS electron");
  hp_p_n3n2->SetTitle("#pi#pm");
  hp_y_n3n2->SetTitle("Photons");
  leg_n3n2->AddEntry(hp_e_n3n2, "", "L");
  leg_n3n2->AddEntry(hp_p_n3n2, "", "L");
  leg_n3n2->AddEntry(hp_y_n3n2, "", "L");
  leg_n3n2->SetTextSize(0.04);
  leg_n3n2->Draw();

  if ( save_figures )
    {
      cp_e_n3n2->Print("plots/Pythia_MomSpec_n3n2.eps");
      cp_e_n3n2->Print("plots/Pythia_MomSpec_n3n2.png");
    }

  /*Temporary Electron Purity Test*/


  /*-------- -2 < Eta < -1 ---------*/

  TH1F* hp_e_n2n1 = new TH1F("hp_e_n2n1", "dN/dp vs. p", 60, 0, 30);
  TH1F* hp_p_n2n1 = new TH1F("hp_p_n2n1", "dN/dp vs. p", 60, 0, 30);
  TH1F* hp_y_n2n1 = new TH1F("hp_y_n2n1", "dN/dp vs. p", 60, 0, 30);

  TCanvas *cp_e_n2n1 = new TCanvas("cp_e_n2n1");
  t_truth->Draw("em_evtgen_ptotal>>hp_e_n2n1", electron_cut && eta_cut_n2n1 && "evtgen_Q2 > 0.01", "goff");
  t_truth->Draw("em_evtgen_ptotal>>hp_p_n2n1", Pion_charged_cut && eta_cut_n2n1 && "evtgen_Q2 > 0.01", "goff");
  t_truth->Draw("em_evtgen_ptotal>>hp_y_n2n1", photon_cut && eta_cut_n2n1 && status_cut && "evtgen_Q2 > 0.01", "goff");

  TH1F* htmp_n2n1 = hp_e_n2n1->Clone();
  htmp_n2n1->SetTitle("");
  htmp_n2n1->GetXaxis()->SetTitle("p [GeV]");
  htmp_n2n1->GetYaxis()->SetTitle("dN/dp");
  htmp_n2n1->SetMaximum( 0.99e7);
  htmp_n2n1->Draw();

  hp_e_n2n1->SetLineColor(2);
  hp_e_n2n1->Draw("same");
  hp_p_n2n1->SetLineColor(1);
  hp_p_n2n1->Draw("same");
  hp_y_n2n1->SetLineColor(4);
  hp_y_n2n1->Draw("same");
  cp_e_n2n1->SetLogy();

  TLegend* leg_n2n1 = new TLegend(0.53,0.67,0.73,0.90);
  hp_e_n2n1->SetTitle("DIS electron");
  hp_p_n2n1->SetTitle("#pi#pm");
  hp_y_n2n1->SetTitle("Photons");
  leg_n2n1->AddEntry(hp_e_n2n1, "", "L");
  leg_n2n1->AddEntry(hp_p_n2n1, "", "L");
  leg_n2n1->AddEntry(hp_y_n2n1, "", "L");
  leg_n2n1->SetTextSize(0.04);
  leg_n2n1->Draw();

  if ( save_figures )
    {
      cp_e_n2n1->Print("plots/Pythia_MomSpec_n2n1.eps");
      cp_e_n2n1->Print("plots/Pythia_MomSpec_n2n1.png");
    }

  /*-------- -1 < Eta < 0 ---------*/

  TH1F* hp_e_n1z0 = new TH1F("hp_e_n1z0", "dN/dp vs. p", 60, 0, 30);
  TH1F* hp_p_n1z0 = new TH1F("hp_p_n1z0", "dN/dp vs. p", 60, 0, 30);
  TH1F* hp_y_n1z0 = new TH1F("hp_y_n1z0", "dN/dp vs. p", 60, 0, 30);

  TCanvas *cp_e_n1z0 = new TCanvas("cp_e_n1z0");
  t_truth->Draw("em_evtgen_ptotal>>hp_e_n1z0", electron_cut && eta_cut_n1z0 && "evtgen_Q2 > 0.01", "goff");
  t_truth->Draw("em_evtgen_ptotal>>hp_p_n1z0", Pion_charged_cut && eta_cut_n1z0 && "evtgen_Q2 > 0.01", "goff");
  t_truth->Draw("em_evtgen_ptotal>>hp_y_n1z0", photon_cut && eta_cut_n1z0 && status_cut && "evtgen_Q2 > 0.01", "goff");

  TH1F* htmp_n1z0 = hp_e_n1z0->Clone();
  htmp_n1z0->SetTitle("");
  htmp_n1z0->GetXaxis()->SetTitle("p [GeV]");
  htmp_n1z0->GetYaxis()->SetTitle("dN/dp");
  htmp_n1z0->SetMaximum( 0.99e7 );
  htmp_n1z0->Draw();

  hp_e_n1z0->SetLineColor(2);
  hp_e_n1z0->Draw("same");
  hp_p_n1z0->SetLineColor(1);
  hp_p_n1z0->Draw("same");
  hp_y_n1z0->SetLineColor(4);
  hp_y_n1z0->Draw("same");
  cp_e_n1z0->SetLogy();

  TLegend* leg_n1z0 = new TLegend(0.53,0.67,0.73,0.90);
  hp_e_n1z0->SetTitle("DIS electron");
  hp_p_n1z0->SetTitle("#pi#pm");
  hp_y_n1z0->SetTitle("Photons");
  leg_n1z0->AddEntry(hp_e_n1z0, "", "L");
  leg_n1z0->AddEntry(hp_p_n1z0, "", "L");
  leg_n1z0->AddEntry(hp_y_n1z0, "", "L");
  leg_n1z0->SetTextSize(0.04);
  leg_n1z0->Draw();

  if ( save_figures )
    {
      cp_e_n1z0->Print("plots/Pythia_MomSpec_n1z0.eps");
      cp_e_n1z0->Print("plots/Pythia_MomSpec_n1z0.png");
    }

  return 0 ;
}
