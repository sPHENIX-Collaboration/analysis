////////////////////////////////////////////////////////////////////////////////
//
// Draw the Di b-jet momentum imbalance
//
// Useful references:
// CMS di b-jet: http://cds.cern.ch/record/2202805/files/HIN-16-005-pas.pdf
//
////////////////////////////////////////////////////////////////////////////////
//
// Darren McGlinchey
// 6 Jan 2017
//
////////////////////////////////////////////////////////////////////////////////

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TMath.h>
#include <TLegend.h>
#include <TRandom3.h>

#include <iostream>

using namespace std;

void Draw_BDiJetImbalance()
{
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);

  //==========================================================================//
  // SET RUNNING CONDITIONS
  //==========================================================================//

  const char* inFile = "HFtag_bjet.root";

  double pT1min = 20;
  double pT2min = 10;

  double bJetEff = 1.; // b-jet tagging efficiency
  double bJetPur = 1.; // b-jet tagging purity

  //==========================================================================//
  // DECLARE VARIABLES
  //==========================================================================//

  //-- tree variables
  TTree *ttree;
  Int_t           event;
  Int_t           truthjet_n;
  Int_t           truthjet_parton_flavor[100];
  Int_t           truthjet_hadron_flavor[100];
  Float_t         truthjet_pt[100];
  Float_t         truthjet_eta[100];
  Float_t         truthjet_phi[100];

  //-- histograms

  TH1D* hjet_pT = new TH1D("hjet_pT", ";p_{T}^{jet} [GeV/c]", 40, 0, 200);
  TH1D* hjet_eta = new TH1D("hjet_eta", ";#eta^{jet}", 100, -1., 1.);
  TH1D* hjet_phi = new TH1D("hjet_phi", ";#phi^{jet}", 40, -4, 4);

  TH1D* hdijet_pT1 = new TH1D("hdijet_pT1", ";p_{T,1}^{jet} [GeV/c]", 40, 0, 200);
  TH1D* hdijet_pT2 = new TH1D("hdijet_pT2", ";p_{T,2}^{jet} [GeV/c]", 40, 0, 200);
  TH1D *hdijet_xj = new TH1D("hdijet_xj", ";x_{j} = p_{T,2} / p_{T,1}; event fraction", 10, 0, 1);
  TH1D* hdijet_dphi = new TH1D("hdijet_dphi", ";|#Delta#phi_{12}|; event fraction", 10, 0, TMath::Pi());


  hjet_pT->Sumw2();
  hjet_eta->Sumw2();
  hjet_phi->Sumw2();

  hdijet_pT1->Sumw2();
  hdijet_pT2->Sumw2();
  hdijet_dphi->Sumw2();
  hdijet_xj->Sumw2();

  //-- other
  TRandom3 *rand = new TRandom3();

  //==========================================================================//
  // LOAD TTREE
  //==========================================================================//
  cout << endl;
  cout << "--> Reading data from " << inFile << endl;

  TFile *fin = TFile::Open(inFile);
  if (!fin)
  {
    cout << "ERROR!! Unable to open " << inFile << endl;
    return;
  }

  ttree = (TTree*) fin->Get("ttree");
  if (!ttree)
  {
    cout << "ERROR!! Unable to find ttree in " << inFile << endl;
    return;
  }

  ttree->SetBranchAddress("event", &event);
  ttree->SetBranchAddress("truthjet_n", &truthjet_n);
  ttree->SetBranchAddress("truthjet_parton_flavor", truthjet_parton_flavor);
  ttree->SetBranchAddress("truthjet_hadron_flavor", truthjet_hadron_flavor);
  ttree->SetBranchAddress("truthjet_pt", truthjet_pt);
  ttree->SetBranchAddress("truthjet_eta", truthjet_eta);
  ttree->SetBranchAddress("truthjet_phi", truthjet_phi);

  Long64_t nentries = ttree->GetEntries();


  //==========================================================================//
  // GET MOMENTUM IMBALANCE
  //==========================================================================//
  cout << endl;
  cout << "--> Running over " << nentries << endl;

  for (Long64_t ientry = 0; ientry < nentries; ientry++)
  {
    ttree->GetEntry(ientry);

    if (ientry % 1000 == 0) cout << "----> Processing event " << ientry << endl;


    // get kinematics for all b-jets
    for (int i = 0; i < truthjet_n; i++)
    {
      if (TMath::Abs(truthjet_parton_flavor[i]) != 5)
        continue;

      // check if we accept this jet
      if (rand->Uniform() > bJetEff)
        continue;

      // single b-jet kinematics
      hjet_pT->Fill(truthjet_pt[i]);
      hjet_eta->Fill(truthjet_eta[i]);
      hjet_phi->Fill(truthjet_phi[i]);

      for (int j = i + 1; j < truthjet_n; j++)
      {
        if (TMath::Abs(truthjet_parton_flavor[j]) != 5)
          continue;

        // check if we accept this jet
        if (rand->Uniform() > bJetEff)
          continue;

        // find the leading and subleading jets
        double pT1, pT2;

        if (truthjet_pt[i] > truthjet_pt[j])
        {
          pT1 = truthjet_pt[i];
          pT2 = truthjet_pt[j];
        }
        else
        {
          pT1 = truthjet_pt[j];
          pT2 = truthjet_pt[i];
        }

        // require leading and subleading jet pT
        if (pT1 < pT1min || pT2 < pT2min)
          continue;

        // check the delta phi
        double phi1 = truthjet_phi[i];
        double phi2 = truthjet_phi[j];
        double dphi = TMath::Abs(TMath::ATan2(TMath::Sin(phi1 - phi2), TMath::Cos(phi1 - phi2)));
        if (dphi > TMath::Pi())
          cout << " " << truthjet_phi[i] << " " << truthjet_phi[j] << " " << dphi << endl;


        hdijet_dphi->Fill(dphi);

        if ( dphi < 2.*TMath::Pi() / 3)
          continue;


        // fill diagnostic histograms
        hdijet_pT1->Fill(pT1);
        hdijet_pT2->Fill(pT2);
        hdijet_xj->Fill(pT2 / pT1);

      } // j
    } // i

  } // ientry

  // first get some statistics
  cout << " N b dijets: " << hdijet_dphi->Integral() << endl;
  cout << " N b dijets w/ dphi cut: " << hdijet_xj->Integral() << endl;

  //-- normalize to integral 1
  hdijet_dphi->Scale(1. / hdijet_dphi->Integral());
  hdijet_xj->Scale(1. / hdijet_xj->Integral());

  //==========================================================================//
  // PLOT OBJECTS
  //==========================================================================//
  cout << endl;
  cout << "--> Plotting" << endl;

  hdijet_xj->SetMarkerStyle(kFullCircle);
  hdijet_xj->SetMarkerColor(kBlack);
  hdijet_xj->SetLineColor(kBlack);
  hdijet_xj->GetYaxis()->SetTitleFont(63);
  hdijet_xj->GetYaxis()->SetTitleSize(24);
  hdijet_xj->GetYaxis()->SetTitleOffset(1.4);
  hdijet_xj->GetYaxis()->SetLabelFont(63);
  hdijet_xj->GetYaxis()->SetLabelSize(20);
  hdijet_xj->GetXaxis()->SetTitleFont(63);
  hdijet_xj->GetXaxis()->SetTitleSize(24);
  hdijet_xj->GetXaxis()->SetTitleOffset(1.0);
  hdijet_xj->GetXaxis()->SetLabelFont(63);
  hdijet_xj->GetXaxis()->SetLabelSize(20);

  hdijet_dphi->SetMarkerStyle(kFullCircle);
  hdijet_dphi->SetMarkerColor(kBlack);
  hdijet_dphi->SetLineColor(kBlack);
  hdijet_dphi->GetYaxis()->SetTitleFont(63);
  hdijet_dphi->GetYaxis()->SetTitleSize(24);
  hdijet_dphi->GetYaxis()->SetTitleOffset(1.4);
  hdijet_dphi->GetYaxis()->SetLabelFont(63);
  hdijet_dphi->GetYaxis()->SetLabelSize(20);
  hdijet_dphi->GetXaxis()->SetTitleFont(63);
  hdijet_dphi->GetXaxis()->SetTitleSize(24);
  hdijet_dphi->GetXaxis()->SetTitleOffset(1.0);
  hdijet_dphi->GetXaxis()->SetLabelFont(63);
  hdijet_dphi->GetXaxis()->SetLabelSize(20);

  hdijet_pT1->SetLineColor(kBlue);
  hdijet_pT2->SetLineColor(kRed);


  //-- legends
  TLegend *leg_jetpt = new TLegend(0.6, 0.5, 0.95, 0.95);
  leg_jetpt->SetFillStyle(0);
  leg_jetpt->SetBorderSize(0);
  leg_jetpt->SetTextFont(63);
  leg_jetpt->SetTextSize(12);
  leg_jetpt->AddEntry(hjet_pT, "Inclusive b-jet", "L");
  leg_jetpt->AddEntry(hdijet_pT1, "leading b-jet", "L");
  leg_jetpt->AddEntry(hdijet_pT2, "subleading b-jet", "L");


  //-- other
  TLatex ltxt;
  ltxt.SetNDC();
  ltxt.SetTextFont(63);
  ltxt.SetTextSize(20);

  //==========================================================================//
  // PLOT
  //==========================================================================//

  // plot b-jet kinematics
  TCanvas *cjet = new TCanvas("cjet", "b-jet", 1200, 400);
  cjet->SetMargin(0, 0, 0, 0);
  cjet->Divide(3, 1);

  cjet->GetPad(1)->SetMargin(0.12, 0.02, 0.12, 0.02);
  cjet->GetPad(2)->SetMargin(0.12, 0.02, 0.12, 0.02);
  cjet->GetPad(3)->SetMargin(0.12, 0.02, 0.12, 0.02);

  cjet->cd(1);
  gPad->SetLogy();
  hjet_pT->GetXaxis()->SetRangeUser(0, 100);
  hjet_pT->Draw();
  hdijet_pT1->Draw("same");
  hdijet_pT2->Draw("same");

  leg_jetpt->Draw("same");

  cjet->cd(2);
  hjet_eta->Draw("HIST");

  cjet->cd(3);
  hjet_phi->Draw("HIST");


  // plot dijet checks
  TCanvas *cdijet = new TCanvas("cdijet", "dijet", 1200, 600);
  cdijet->SetMargin(0, 0, 0, 0);
  cdijet->Divide(2, 1);

  cdijet->GetPad(1)->SetMargin(0.12, 0.02, 0.12, 0.02);
  cdijet->GetPad(2)->SetMargin(0.12, 0.02, 0.12, 0.02);

  cdijet->cd(1);
  hdijet_xj->Draw();

  ltxt.DrawLatex(0.2, 0.92, "pythia8 b-jets");
  ltxt.DrawLatex(0.2, 0.86, "p+p #sqrt{s_{_{NN}}}=200 GeV");
  ltxt.DrawLatex(0.2, 0.80, "anti-k_{T}, R=0.4");
  ltxt.DrawLatex(0.2, 0.74, Form("p_{T,1} > %.0f GeV", pT1min));
  ltxt.DrawLatex(0.2, 0.68, Form("p_{T,2} > %.0f GeV", pT2min));
  ltxt.DrawLatex(0.2, 0.62, Form("|#Delta#phi_{12}| > 2#pi/3"));

  cdijet->cd(2);
  hdijet_dphi->Draw();

  cdijet->Print("dibjet_imbalance.pdf");

}