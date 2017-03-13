/**

 *\Brief Macro to reproduce ePHENIX LoI Feb. 2014 (arXiv:1402.1209v1) Fig. 2.1, 2.2, & 2.4
 *\Author: Thomas Krahulik <thomas.krahulik@stonybrook.edu>

 **/

int makePlot_LoI_Momentum(
                          const TString infile = "data/pythia.ep.100.test.root"
                          )
{
  gStyle->SetOptStat(kFALSE);

  /*--------------Get Input File--------------*/

  TFile *f_pyth = new TFile( infile, "OPEN");
  TTree* T = (TTree*)f_pyth->Get("tree");
  T->Print();

  /*------------------------------------------*/
  /*--------------Define Cuts-----------------*/

  //TCut Q2_cut = "Q2 > 1";

  TCut electron_cut = "p.fKF == 11";
  TCut hadron_cut = "abs(p.fKF) > 100";
  TCut proton_cut = "p.fKF == 2212";
  TCut neutron_cut = "p.fKF == 2112";
  TCut Kaon_cut = "abs(p.fKF) == 321 || p.fKF == 311";
  TCut Pion_charged_cut = "abs(p.fKF) == 211";
  //TCut Pion_cut = "abs(p.fKF) == 211 || p.fKF == 111";
  TCut photon_cut = "p.fKF == 22";

  TCut mother_cut = "p.fParent == 0";
  TCut status_cut = "p.fKS < 10";

  TCut eta_cut_n3n2 = "TMath::ASinH(p.fPz/sqrt((p.fPx)**2 + (p.fPy)**2)) > -3 && TMath::ASinH(p.fPz/sqrt((p.fPx)**2 + (p.fPy)**2)) < -2";
  TCut eta_cut_n2n1 = "TMath::ASinH(p.fPz/sqrt((p.fPx)**2 + (p.fPy)**2)) > -2 && TMath::ASinH(p.fPz/sqrt((p.fPx)**2 + (p.fPy)**2)) < -1";
  TCut eta_cut_n1z0 = "TMath::ASinH(p.fPz/sqrt((p.fPx)**2 + (p.fPy)**2)) > -1 && TMath::ASinH(p.fPz/sqrt((p.fPx)**2 + (p.fPy)**2)) < -0";

  /*------------------------------------------*/
  /*-------Momentum vs. Pseudorapidity--------*/
  /*------------------------------------------*/

  /*------------------------------------------*/
  /*---------Electrons (LoI Fig. 2-1)---------*/
  /*------------------------------------------*/
  TH2F *h_peta_e = new TH2F("h_peta_e", "", 100, -4, 3, 100, 0, 50 ); //250x015 10M

  TCanvas *c_peta_e = new TCanvas( "c_peta_e" );
  c_peta_e->SetLogz();

  T->Draw("sqrt((p.fPx)**2 + (p.fPy)**2 + (p.fPz)**2 ):TMath::ASinH(p.fPz/sqrt((p.fPx)**2 + (p.fPy)**2))>>h_peta_e", electron_cut && mother_cut && "Q2 > 1", "colz");

  h_peta_e->GetXaxis()->SetTitle("Pseudorapidity #eta");
  h_peta_e->GetYaxis()->SetTitle("Electron Momentum p_{e-} [GeV]");

  //  c_p_eta_e->Print("Plots/Pythia_peta_e_10M_250x010.eps");
  //  c_p_eta_e->Print("Plots/Pythia_peta_e_10M_250x010.png");

  /*------------------------------------------*/
  /*----------Hadrons (LoI Fig. 2-4)----------*/
  /*------------------------------------------*/
  TH2F *h_p_eta_h = new TH2F("h_p_eta_h", "", 60,-6,6, 200,0,225); //250x015 10M

  TCanvas *c_p_eta_h = new TCanvas( "c_p_eta_h" );
  // T->Draw("sqrt((p.fPx)**2 + (p.fPy)**2 + (p.fPz)**2 ):TMath::ASinH(p.fPz/sqrt((p.fPx)**2 + (p.fPy)**2))>>h_p_eta_h",hadron_cut && "Q2 > 1 && 0.01 < y < 0.80 && W2 > 10", "colz");
  T->Draw("sqrt((p.fPx)**2 + (p.fPy)**2 + (p.fPz)**2 ):TMath::ASinH(p.fPz/sqrt((p.fPx)**2 + (p.fPy)**2))>>h_p_eta_h",hadron_cut && "Q2 > 1 && 0.01 < y < 0.80", "colz");

  h_p_eta_h->GetXaxis()->SetTitle("Pseudorapidity #eta");
  h_p_eta_h->GetYaxis()->SetTitle("Hadron Momentum p_{Hadron} [GeV]");
  c_p_eta_h->SetLogz();

  //  c_p_eta_h->Print("Plots/Pythia_peta_h_10M_250x010.eps");
  //  c_p_eta_h->Print("Plots/Pythia_peta_h_10M_250x010.png");


  /*------------Momentum Spectra (Figure 2.2)--------------*/

  /*-------- -3 < Eta < -2 ---------*/

  TH1F* hp_e_n3n2 = new TH1F("hp_e_n3n2", "dN/dp vs. p", 60, 0, 30);
  TH1F* hp_p_n3n2 = new TH1F("hp_p_n3n2", "dN/dp vs. p", 60, 0, 30);
  TH1F* hp_y_n3n2 = new TH1F("hp_y_n3n2", "dN/dp vs. p", 60, 0, 30);

  TCanvas *cp_e_n3n2 = new TCanvas("cp_e_n3n2");
  T->Draw("sqrt(p.fPx**2 + p.fPy**2 + p.fPz**2)>>hp_e_n3n2", electron_cut && eta_cut_n3n2 && "Q2 > 0.01", "goff");
  T->Draw("sqrt(p.fPx**2 + p.fPy**2 + p.fPz**2)>>hp_p_n3n2", Pion_charged_cut && eta_cut_n3n2 && "Q2 > 0.01", "goff");
  T->Draw("sqrt(p.fPx**2 + p.fPy**2 + p.fPz**2)>>hp_y_n3n2", photon_cut && eta_cut_n3n2 && status_cut && "Q2 > 0.01", "goff");


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

  //  cp_e_n3n2->Print("Plots/Pythia_MomSpec_n3n2_10M_250x010.eps");
  //  cp_e_n3n2->Print("Plots/Pythia_MomSpec_n3n2_10M_250x010.png");


  /*Temporary Electron Purity Test*/






  /*-------- -2 < Eta < -1 ---------*/

  TH1F* hp_e_n2n1 = new TH1F("hp_e_n2n1", "dN/dp vs. p", 60, 0, 30);
  TH1F* hp_p_n2n1 = new TH1F("hp_p_n2n1", "dN/dp vs. p", 60, 0, 30);
  TH1F* hp_y_n2n1 = new TH1F("hp_y_n2n1", "dN/dp vs. p", 60, 0, 30);

  TCanvas *cp_e_n2n1 = new TCanvas("cp_e_n2n1");
  T->Draw("sqrt(p.fPx**2 + p.fPy**2 + p.fPz**2)>>hp_e_n2n1", electron_cut && eta_cut_n2n1 && "Q2 > 0.01", "goff");
  T->Draw("sqrt(p.fPx**2 + p.fPy**2 + p.fPz**2)>>hp_p_n2n1", Pion_charged_cut && eta_cut_n2n1 && "Q2 > 0.01", "goff");
  T->Draw("sqrt(p.fPx**2 + p.fPy**2 + p.fPz**2)>>hp_y_n2n1", photon_cut && eta_cut_n2n1 && status_cut && "Q2 > 0.01", "goff");

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

  //  cp_e_n2n1->Print("Plots/Pythia_MomSpec_n2n1_10M_250x010.eps");
  //  cp_e_n2n1->Print("Plots/Pythia_MomSpec_n2n1_10M_250x010.png");

  /*-------- -1 < Eta < 0 ---------*/

  TH1F* hp_e_n1z0 = new TH1F("hp_e_n1z0", "dN/dp vs. p", 60, 0, 30);
  TH1F* hp_p_n1z0 = new TH1F("hp_p_n1z0", "dN/dp vs. p", 60, 0, 30);
  TH1F* hp_y_n1z0 = new TH1F("hp_y_n1z0", "dN/dp vs. p", 60, 0, 30);

  TCanvas *cp_e_n1z0 = new TCanvas("cp_e_n1z0");
  T->Draw("sqrt(p.fPx**2 + p.fPy**2 + p.fPz**2)>>hp_e_n1z0", electron_cut && eta_cut_n1z0 && "Q2 > 0.01", "goff");
  T->Draw("sqrt(p.fPx**2 + p.fPy**2 + p.fPz**2)>>hp_p_n1z0", Pion_charged_cut && eta_cut_n1z0 && "Q2 > 0.01", "goff");
  T->Draw("sqrt(p.fPx**2 + p.fPy**2 + p.fPz**2)>>hp_y_n1z0", photon_cut && eta_cut_n1z0 && status_cut && "Q2 > 0.01", "goff");

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

  //  cp_e_n1z0->Print("Plots/Pythia_MomSpec_n1z0_10M_250x010.eps");
  //  cp_e_n1z0->Print("Plots/Pythia_MomSpec_n1z0_10M_250x010.png");

  return 0 ;
}
