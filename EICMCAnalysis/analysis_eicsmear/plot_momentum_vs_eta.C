/**

 *\Brief Macro to create plots like ePHENIX LoI Feb. 2014 (arXiv:1402.1209v1) Fig. 2.1, 2.2, & 2.4

 **/

int plot_momentum_vs_eta(
			 //const TString infile = "data/pythia.ep.100.test.root"
			 )
{
  gStyle->SetOptStat(kFALSE);

  /*--------------Get Input File--------------*/

//  float xsec_T1 = 0.94568934478641919;
//  float xsec_T2 = 7.80038488753058562E-002;
//  float xsec_T3 = 3.38307113522152677E-003;
//
  TChain* T1 = new TChain("EICTree");
  T1->AddFile("data/bpage/origin/pythia.ep.20x250.250Kevents.Q2=1.0-10.0.PDF=10800_1.root");
  T1->AddFile("data/bpage/origin/pythia.ep.20x250.250Kevents.Q2=1.0-10.0.PDF=10800_2.root");
  T1->AddFile("data/bpage/origin/pythia.ep.20x250.250Kevents.Q2=1.0-10.0.PDF=10800_3.root");
  T1->AddFile("data/bpage/origin/pythia.ep.20x250.250Kevents.Q2=1.0-10.0.PDF=10800_4.root");
  T1->AddFile("data/bpage/origin/pythia.ep.20x250.250Kevents.Q2=1.0-10.0.PDF=10800_5.root");
  T1->AddFile("data/bpage/origin/pythia.ep.20x250.250Kevents.Q2=1.0-10.0.PDF=10800_6.root");
  T1->AddFile("data/bpage/origin/pythia.ep.20x250.250Kevents.Q2=1.0-10.0.PDF=10800_7.root");
  T1->AddFile("data/bpage/origin/pythia.ep.20x250.250Kevents.Q2=1.0-10.0.PDF=10800_8.root");
  T1->AddFile("data/bpage/origin/pythia.ep.20x250.250Kevents.Q2=1.0-10.0.PDF=10800_9.root");
  T1->AddFile("data/bpage/origin/pythia.ep.20x250.250Kevents.Q2=1.0-10.0.PDF=10800_10.root");

//  T1->SetWeight(xsec_T1 / xsec_T1,"global");
//
//  TChain* T2 = new TChain("EICTree");
//  T2->AddFile("data/bpage/origin/pythia.ep.20x250.250Kevents.Q2=10.0-100.0.PDF=10800_1.root");
//  T2->SetWeight(xsec_T2 / xsec_T1,"global");
//
//  TChain* T3 = new TChain("EICTree");
//  T3->AddFile("data/bpage/origin/pythia.ep.20x250.50Kevents.Q2=100.0-1000.0.PDF=10800_1.root");
//  T3->SetWeight(xsec_T3 / xsec_T1,"global");


  TChain* T = new TChain("EICTree");
  //  T->AddFile("data/TEST/pythia.ep.20x250.1Mevents.1.RadCor=0.Q2gt1.root");
  //  T->AddFile("data/pythiaeRHIC/TREES/pythia.ep.20x250.RadCor=0.Q2gt1.50kevts.root");
  T->Add(T1);
  //  T->Add(T2);
  //  T->Add(T3);

  /*------------------------------------------*/
  /*--------------Define Cuts-----------------*/

  //TCut Q2_cut = "Q2 > 1";

  TCut electron_cut = "particles.id == 11";
  TCut hadron_cut = "abs(particles.id) > 100";
  TCut proton_cut = "particles.id == 2212";
  TCut neutron_cut = "particles.id == 2112";
  TCut Kaon_cut = "abs(particles.id) == 321 || particles.id == 311";
  TCut Pion_charged_cut = "abs(particles.id) == 211";
  //TCut Pion_cut = "abs(particles.id) == 211 || particles.id == 111";
  TCut photon_cut = "particles.id == 22";

  TCut mother_cut = "particles.parentId == 0";
  TCut status_cut = "particles.KS == 1";

  TCut eta_cut_n3n2 = "particles.eta > -3 && particles.eta < -2";
  TCut eta_cut_n2n1 = "particles.eta > -2 && particles.eta < -1";
  TCut eta_cut_n1z0 = "particles.eta > -1 && particles.eta < -0";

  /*------------------------------------------*/
  /*-------Momentum vs. Pseudorapidity--------*/
  /*------------------------------------------*/

  /*------------------------------------------*/
  /*---------Electrons (LoI Fig. 2-1)---------*/
  /*------------------------------------------*/
//  TH2F *h_peta_e = new TH2F("h_peta_e", "", 100, -4, 3, 100, 0, 50 ); //250x015 10M
//
//  TCanvas *c_peta_e = new TCanvas( "c_peta_e" );
//  c_peta_e->SetLogz();
//
//  //  T->Draw("particles.p : particles.eta>>h_peta_e", electron_cut && mother_cut && "trueQ2 > 1", "colz");
//  T->Draw("particles.p : particles.eta>>h_peta_e", electron_cut && "trueQ2 > 1", "colz");
//
//  h_peta_e->GetXaxis()->SetTitle("Pseudorapidity #eta");
//  h_peta_e->GetYaxis()->SetTitle("Electron Momentum p_{e-} [GeV]");

  //  c_p_eta_e->Print("Plots/pythia_peta_e_10M_250x010.eps");
  //  c_p_eta_e->Print("Plots/pythia_peta_e_10M_250x010.png");

  /*------------------------------------------*/
  /*----------Hadrons (LoI Fig. 2-4)----------*/
  /*------------------------------------------*/
  TH2F *h_p_eta_h = new TH2F("h_p_eta_h", "", 80,-5,5, 50,0,70); //250x015

  TCanvas *c_p_eta_h = new TCanvas( "c_p_eta_h" );
  c_p_eta_h->SetRightMargin(0.12);

  T->Draw("particles.p : particles.eta>>h_p_eta_h",hadron_cut && "trueQ2 > 1 && trueY > 0.01 && trueY < 0.80 && trueW2 > 10 && particles.z > 0.2", "colz");

//  TH2F *h_p_eta_h1 = (TH2F*)h_p_eta_h->Clone("h_p_eta_h1");
//  TH2F *h_p_eta_h2 = (TH2F*)h_p_eta_h->Clone("h_p_eta_h2");
//  TH2F *h_p_eta_h3 = (TH2F*)h_p_eta_h->Clone("h_p_eta_h3");
//
//  T1->Draw("particles.p : particles.eta>>h_p_eta_h1",hadron_cut && "trueQ2 > 1 && trueY > 0.01 && trueY < 0.80 && trueW2 > 10 && particles.z > 0.2", "colz");
//  T2->Draw("particles.p : particles.eta>>h_p_eta_h2",hadron_cut && "trueQ2 > 1 && trueY > 0.01 && trueY < 0.80 && trueW2 > 10 && particles.z > 0.2", "colz");
//  T3->Draw("particles.p : particles.eta>>h_p_eta_h3",hadron_cut && "trueQ2 > 1 && trueY > 0.01 && trueY < 0.80 && trueW2 > 10 && particles.z > 0.2", "colz");
//
//  h_p_eta_h->Add(h_p_eta_h1);
//  h_p_eta_h->Add(h_p_eta_h2);
//  h_p_eta_h->Add(h_p_eta_h3);

  h_p_eta_h->GetXaxis()->SetTitle("Pseudorapidity #eta");
  h_p_eta_h->GetYaxis()->SetTitle("Hadron Momentum p_{Hadron} [GeV]");
  c_p_eta_h->SetLogz();

  h_p_eta_h->Draw("colz");

  /* Add boxes for PID acceptances */
  TBox * b_cpid = new TBox(-1.55,0.8,1.242,6); // DIRC
  b_cpid->SetFillStyle(0);
  b_cpid->SetLineWidth(2);
  b_cpid->Draw("sameL");

  TBox * b_epid = new TBox(-4,2,-1.55,8); // mRICH
  b_epid->SetFillStyle(0);
  b_epid->SetLineWidth(2);
  b_epid->Draw("sameL");

  TBox * b_fpid0 = new TBox(1.242,15,4,50); // Gas RICH
  b_fpid0->SetFillStyle(0);
  b_fpid0->SetLineWidth(2);
  b_fpid0->Draw("sameL");

  TBox * b_fpid1 = new TBox(1.242,2,1.85,8); // mRICH
  b_fpid1->SetFillStyle(0);
  b_fpid1->SetLineWidth(2);
  b_fpid1->Draw("sameL");

  TBox * b_fpid2 = new TBox(1.85,0,4,8); // TOF
  b_fpid2->SetFillStyle(0);
  b_fpid2->SetLineWidth(2);
  b_fpid2->Draw("sameL");

  //  c_p_eta_h->Print("Plots/pythia_peta_h_10M_250x010.eps");
  //  c_p_eta_h->Print("Plots/pythia_peta_h_10M_250x010.png");


//  /*------------Momentum Spectra (Figure 2.2)--------------*/
//
//  /*-------- -3 < Eta < -2 ---------*/
//
//  TH1F* hp_e_n3n2 = new TH1F("hp_e_n3n2", "dN/dp vs. p", 60, 0, 30);
//  TH1F* hp_p_n3n2 = new TH1F("hp_p_n3n2", "dN/dp vs. p", 60, 0, 30);
//  TH1F* hp_y_n3n2 = new TH1F("hp_y_n3n2", "dN/dp vs. p", 60, 0, 30);
//
//  TCanvas *cp_e_n3n2 = new TCanvas("cp_e_n3n2");
//  T->Draw("sqrt(particles.px**2 + particles.py**2 + particles.pz**2)>>hp_e_n3n2", electron_cut && eta_cut_n3n2 && "Q2 > 0.01", "goff");
//  T->Draw("sqrt(particles.px**2 + particles.py**2 + particles.pz**2)>>hp_p_n3n2", Pion_charged_cut && eta_cut_n3n2 && "Q2 > 0.01", "goff");
//  T->Draw("sqrt(particles.px**2 + particles.py**2 + particles.pz**2)>>hp_y_n3n2", photon_cut && eta_cut_n3n2 && status_cut && "Q2 > 0.01", "goff");
//
//
//  TH1F* htmp_n3n2 = hp_e_n3n2->Clone();
//  htmp_n3n2->SetTitle("");
//  htmp_n3n2->GetXaxis()->SetTitle("p [GeV]");
//  htmp_n3n2->GetYaxis()->SetTitle("dN/dp");
//  htmp_n3n2->SetMaximum( 0.99e7);
//  htmp_n3n2->Draw();
//
//  hp_e_n3n2->SetLineColor(2);
//  hp_e_n3n2->Draw("same");
//  hp_p_n3n2->SetLineColor(1);
//  hp_p_n3n2->Draw("same");
//  hp_y_n3n2->SetLineColor(4);
//  hp_y_n3n2->Draw("same");
//  cp_e_n3n2->SetLogy();
//
//  TLegend* leg_n3n2 = new TLegend(0.53,0.67,0.73,0.90);
//  hp_e_n3n2->SetTitle("DIS electron");
//  hp_p_n3n2->SetTitle("#pi#pm");
//  hp_y_n3n2->SetTitle("Photons");
//  leg_n3n2->AddEntry(hp_e_n3n2, "", "L");
//  leg_n3n2->AddEntry(hp_p_n3n2, "", "L");
//  leg_n3n2->AddEntry(hp_y_n3n2, "", "L");
//  leg_n3n2->SetTextSize(0.04);
//  leg_n3n2->Draw();
//
//  //  cp_e_n3n2->Print("Plots/pythia_MomSpec_n3n2_10M_250x010.eps");
//  //  cp_e_n3n2->Print("Plots/pythia_MomSpec_n3n2_10M_250x010.png");
//
//
//  /*Temporary Electron Purity Test*/
//
//
//
//
//
//
//  /*-------- -2 < Eta < -1 ---------*/
//
//  TH1F* hp_e_n2n1 = new TH1F("hp_e_n2n1", "dN/dp vs. p", 60, 0, 30);
//  TH1F* hp_p_n2n1 = new TH1F("hp_p_n2n1", "dN/dp vs. p", 60, 0, 30);
//  TH1F* hp_y_n2n1 = new TH1F("hp_y_n2n1", "dN/dp vs. p", 60, 0, 30);
//
//  TCanvas *cp_e_n2n1 = new TCanvas("cp_e_n2n1");
//  T->Draw("sqrt(particles.px**2 + particles.py**2 + particles.pz**2)>>hp_e_n2n1", electron_cut && eta_cut_n2n1 && "Q2 > 0.01", "goff");
//  T->Draw("sqrt(particles.px**2 + particles.py**2 + particles.pz**2)>>hp_p_n2n1", Pion_charged_cut && eta_cut_n2n1 && "Q2 > 0.01", "goff");
//  T->Draw("sqrt(particles.px**2 + particles.py**2 + particles.pz**2)>>hp_y_n2n1", photon_cut && eta_cut_n2n1 && status_cut && "Q2 > 0.01", "goff");
//
//  TH1F* htmp_n2n1 = hp_e_n2n1->Clone();
//  htmp_n2n1->SetTitle("");
//  htmp_n2n1->GetXaxis()->SetTitle("p [GeV]");
//  htmp_n2n1->GetYaxis()->SetTitle("dN/dp");
//  htmp_n2n1->SetMaximum( 0.99e7);
//  htmp_n2n1->Draw();
//
//  hp_e_n2n1->SetLineColor(2);
//  hp_e_n2n1->Draw("same");
//  hp_p_n2n1->SetLineColor(1);
//  hp_p_n2n1->Draw("same");
//  hp_y_n2n1->SetLineColor(4);
//  hp_y_n2n1->Draw("same");
//  cp_e_n2n1->SetLogy();
//
//  TLegend* leg_n2n1 = new TLegend(0.53,0.67,0.73,0.90);
//  hp_e_n2n1->SetTitle("DIS electron");
//  hp_p_n2n1->SetTitle("#pi#pm");
//  hp_y_n2n1->SetTitle("Photons");
//  leg_n2n1->AddEntry(hp_e_n2n1, "", "L");
//  leg_n2n1->AddEntry(hp_p_n2n1, "", "L");
//  leg_n2n1->AddEntry(hp_y_n2n1, "", "L");
//  leg_n2n1->SetTextSize(0.04);
//  leg_n2n1->Draw();
//
//  //  cp_e_n2n1->Print("Plots/pythia_MomSpec_n2n1_10M_250x010.eps");
//  //  cp_e_n2n1->Print("Plots/pythia_MomSpec_n2n1_10M_250x010.png");
//
//  /*-------- -1 < Eta < 0 ---------*/
//
//  TH1F* hp_e_n1z0 = new TH1F("hp_e_n1z0", "dN/dp vs. p", 60, 0, 30);
//  TH1F* hp_p_n1z0 = new TH1F("hp_p_n1z0", "dN/dp vs. p", 60, 0, 30);
//  TH1F* hp_y_n1z0 = new TH1F("hp_y_n1z0", "dN/dp vs. p", 60, 0, 30);
//
//  TCanvas *cp_e_n1z0 = new TCanvas("cp_e_n1z0");
//  T->Draw("sqrt(particles.px**2 + particles.py**2 + particles.pz**2)>>hp_e_n1z0", electron_cut && eta_cut_n1z0 && "Q2 > 0.01", "goff");
//  T->Draw("sqrt(particles.px**2 + particles.py**2 + particles.pz**2)>>hp_p_n1z0", Pion_charged_cut && eta_cut_n1z0 && "Q2 > 0.01", "goff");
//  T->Draw("sqrt(particles.px**2 + particles.py**2 + particles.pz**2)>>hp_y_n1z0", photon_cut && eta_cut_n1z0 && status_cut && "Q2 > 0.01", "goff");
//
//  TH1F* htmp_n1z0 = hp_e_n1z0->Clone();
//  htmp_n1z0->SetTitle("");
//  htmp_n1z0->GetXaxis()->SetTitle("p [GeV]");
//  htmp_n1z0->GetYaxis()->SetTitle("dN/dp");
//  htmp_n1z0->SetMaximum( 0.99e7 );
//  htmp_n1z0->Draw();
//
//  hp_e_n1z0->SetLineColor(2);
//  hp_e_n1z0->Draw("same");
//  hp_p_n1z0->SetLineColor(1);
//  hp_p_n1z0->Draw("same");
//  hp_y_n1z0->SetLineColor(4);
//  hp_y_n1z0->Draw("same");
//  cp_e_n1z0->SetLogy();
//
//  TLegend* leg_n1z0 = new TLegend(0.53,0.67,0.73,0.90);
//  hp_e_n1z0->SetTitle("DIS electron");
//  hp_p_n1z0->SetTitle("#pi#pm");
//  hp_y_n1z0->SetTitle("Photons");
//  leg_n1z0->AddEntry(hp_e_n1z0, "", "L");
//  leg_n1z0->AddEntry(hp_p_n1z0, "", "L");
//  leg_n1z0->AddEntry(hp_y_n1z0, "", "L");
//  leg_n1z0->SetTextSize(0.04);
//  leg_n1z0->Draw();
//
//  //  cp_e_n1z0->Print("Plots/pythia_MomSpec_n1z0_10M_250x010.eps");
//  //  cp_e_n1z0->Print("Plots/pythia_MomSpec_n1z0_10M_250x010.png");

  return 0 ;
}
