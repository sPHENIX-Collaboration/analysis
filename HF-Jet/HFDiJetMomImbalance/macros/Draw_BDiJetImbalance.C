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

  bool is_pp = true;

  // const char* inFile = "HFtag_bjet.root";
  const char* inFile = "HFtag_bjet_pp.root";
  if (!is_pp)
    const char* inFile = "HFtag_bjet_AuAu0-10.root";


  double pT1min = 20;
  double pT2min = 10;
  double etamax = 1.0;

  double bJetEff = 0.5; // b-jet tagging efficiency
  double bJetPur = 1.; // b-jet tagging purity

  double RAA = 1.0;
  if (!is_pp)
    RAA = 0.6;

  const int NETACUT = 3;
  double etacut[] = {1.3, 1.0, 0.7};
  double etaColor[] = {kBlue, kRed, kBlack};

  const int PTCUT = 3;
  double ptcut[] = {10, 20, 40};
  double ptColor[] = {kBlue, kRed, kBlack};

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

  TH1D* hjet_pT = new TH1D("hjet_pT", ";p_{T}^{jet} [GeV/c]", 20, 0, 100);
  TH1D* hjet_eta = new TH1D("hjet_eta", ";#eta^{jet}", 40, -2., 2.);
  TH1D* hjet_phi = new TH1D("hjet_phi", ";#phi^{jet}", 40, -4, 4);

  TH1D* hdijet_pT1 = new TH1D("hdijet_pT1", ";p_{T,1}^{jet} [GeV/c]", 20, 0, 100);
  TH1D* hdijet_pT2 = new TH1D("hdijet_pT2", ";p_{T,2}^{jet} [GeV/c]", 20, 0, 100);
  TH1D* hdijet_xj = new TH1D("hdijet_xj", ";x_{j} = p_{T,2} / p_{T,1}; event fraction", 10, 0, 1);
  TH1D* hdijet_dphi = new TH1D("hdijet_dphi", ";|#Delta#phi_{12}|; event fraction", 10, 0, TMath::Pi());

  TH1D* hdijet_sing_pT1 = new TH1D("hdijet_sing_pT1",
                                   ";p_{T,1} [GeV/c];Fraction of Dijet / Single Accepted",
                                   20, 00, 100);
  TH1D* hdijet_sing_eta2 = new TH1D("hdijet_sing_eta2",
                                    ";#eta_{2};Fraction of Dijet / Single Accepted",
                                    15, -1.5, 1.5);
  TH1D* hdijet_eff_pT1[3];
  TH1D* hdijet_eff_eta2[3];
  for (int i = 0; i < NETACUT; i++)
  {
    hdijet_eff_pT1[i] = new TH1D(Form("hdijet_eff_pT1_%i", i),
                                 ";p_{T,1} [GeV/c];Fraction of Dijet / Single Accepted",
                                 20, 00, 100);
    hdijet_eff_pT1[i]->SetLineColor(etaColor[i]);
    hdijet_eff_pT1[i]->SetLineWidth(2);
  } // i
  for (int i = 0; i < PTCUT; i++)
  {
    hdijet_eff_eta2[i] = new TH1D(Form("hdijet_eff_eta2_%i", i),
                                  ";#eta;Fraction of Dijet / Single Accepted",
                                  15, -1.5, 1.5);
    hdijet_eff_eta2[i]->SetLineColor(etaColor[i]);
    hdijet_eff_eta2[i]->SetLineWidth(2);
  } // i

  hjet_pT->Sumw2();
  hjet_eta->Sumw2();
  hjet_phi->Sumw2();

  hdijet_pT1->Sumw2();
  hdijet_pT2->Sumw2();
  hdijet_dphi->Sumw2();
  hdijet_xj->Sumw2();

  //-- other
  TRandom3 *rand = new TRandom3();

  bool acc[100];

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

  int iprint = 0;
  for (Long64_t ientry = 0; ientry < nentries; ientry++)
  {
    ttree->GetEntry(ientry);

    if (ientry % 1000 == 0) cout << "----> Processing event " << ientry << endl;

    //-- apply acceptance to each jet
    for (int i = 0; i < truthjet_n; i++)
      acc[i] = (rand->Uniform() < bJetEff) && (rand->Uniform() < RAA);

    //-- get kinematics for all b-jets
    for (int i = 0; i < truthjet_n; i++)
    {
      if (TMath::Abs(truthjet_parton_flavor[i]) != 5)
        continue;

      // single b-jet kinematics
      hjet_pT->Fill(truthjet_pt[i]);
      hjet_eta->Fill(truthjet_eta[i]);
      hjet_phi->Fill(truthjet_phi[i]);

      for (int j = i + 1; j < truthjet_n; j++)
      {
        if (TMath::Abs(truthjet_parton_flavor[j]) != 5)
          continue;


        // find the leading and subleading jets
        int i1, i2;
        double pT1, pT2;
        double phi1, phi2;
        double eta1, eta2;
        bool acc1, acc2;

        if (truthjet_pt[i] > truthjet_pt[j])
        {
          i1 = i;
          i2 = j;
        }
        else
        {
          i1 = j;
          i2 = i;
        }

        pT1 = truthjet_pt[i1];
        phi1 = truthjet_phi[i1];
        eta1 = truthjet_eta[i1];
        acc1 = acc[i1];

        pT2 = truthjet_pt[i2];
        phi2 = truthjet_phi[i2];
        eta2 = truthjet_eta[i2];
        acc2 = acc[i2];



        if (iprint < 20)
        {
          cout << " ientry: " << ientry << endl
               << "        i1:" << i1 << " pT1:" << pT1 << " eta1:" << eta1 << " acc1:" << acc1 << endl
               << "        i2:" << i2 << " pT2:" << pT2 << " eta2:" << eta2 << " acc2:" << acc2 << endl;
          iprint++;
        }



        // check if we accept the leading jet
        if ( pT1 < pT1min || TMath::Abs(eta1) > etamax || !acc1 )
          continue;

        // calculate the delta phi
        double dphi = TMath::Abs(TMath::ATan2(TMath::Sin(phi1 - phi2), TMath::Cos(phi1 - phi2)));
        if (dphi > TMath::Pi())
          cout << " " << truthjet_phi[i] << " " << truthjet_phi[j] << " " << dphi << endl;

        // calculate efficiency for finding the dijet
        hdijet_sing_pT1->Fill(pT1);

        // cut on the subleading jet
        if ( pT2 < pT2min || TMath::Abs(eta2) > etamax || !acc2 )
          continue;

        hdijet_sing_eta2->Fill(eta2);


        // fill efficiency for finding the dijet
        for (int k = 0; k < NETACUT; k++)
        {
          if ( TMath::Abs(eta2) < etacut[k])
            hdijet_eff_pT1[k]->Fill(pT1);
        }
        for (int k = 0; k < PTCUT; k++)
        {
          if (pT2 > ptcut[k])
            hdijet_eff_eta2[k]->Fill(eta2);
        }

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
  cout << " N b dijets w / dphi cut: " << hdijet_xj->Integral() << endl;

  // calculate efficiencies
  for (int i = 0; i < NETACUT; i++)
  {
    hdijet_eff_pT1[i]->Divide(hdijet_sing_pT1);
    cout << " i: " << hdijet_eff_pT1[i]->GetMaximum() << endl;
  }
  for (int i = 0; i < PTCUT; i++)
  {
    hdijet_eff_eta2[i]->Divide(hdijet_sing_eta2);
  }
  //-- normalize to integral 1
  hdijet_dphi->Scale(1. / hdijet_dphi->Integral());
  hdijet_xj->Scale(1. / hdijet_xj->Integral());

  //==========================================================================//
  // PLOT OBJECTS
  //==========================================================================//
  cout << endl;
  cout << "-- > Plotting" << endl;

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

  TH1D* heff_axis_pt = new TH1D("heff_axis_pt",
                                "; p_{T, 1} [GeV / c]; Fraction of Dijet / Single Accepted",
                                20, 00, 100);
  heff_axis_pt->SetMinimum(0);
  heff_axis_pt->SetMaximum(1.);
  heff_axis_pt->GetYaxis()->SetTitleFont(63);
  heff_axis_pt->GetYaxis()->SetTitleSize(24);
  heff_axis_pt->GetYaxis()->SetTitleOffset(1.4);
  heff_axis_pt->GetYaxis()->SetLabelFont(63);
  heff_axis_pt->GetYaxis()->SetLabelSize(20);
  heff_axis_pt->GetXaxis()->SetTitleFont(63);
  heff_axis_pt->GetXaxis()->SetTitleSize(24);
  heff_axis_pt->GetXaxis()->SetTitleOffset(1.0);
  heff_axis_pt->GetXaxis()->SetLabelFont(63);
  heff_axis_pt->GetXaxis()->SetLabelSize(20);

  TH1D* heff_axis_eta = new TH1D("heff_axis_eta",
                                 "; #eta_{2}; Fraction of Dijet / Single Accepted",
                                 150, -1.5, 1.5);
  heff_axis_eta->SetMinimum(0);
  heff_axis_eta->SetMaximum(1.);
  heff_axis_eta->GetYaxis()->SetTitleFont(63);
  heff_axis_eta->GetYaxis()->SetTitleSize(24);
  heff_axis_eta->GetYaxis()->SetTitleOffset(1.4);
  heff_axis_eta->GetYaxis()->SetLabelFont(63);
  heff_axis_eta->GetYaxis()->SetLabelSize(20);
  heff_axis_eta->GetXaxis()->SetTitleFont(63);
  heff_axis_eta->GetXaxis()->SetTitleSize(24);
  heff_axis_eta->GetXaxis()->SetTitleOffset(1.0);
  heff_axis_eta->GetXaxis()->SetLabelFont(63);
  heff_axis_eta->GetXaxis()->SetLabelSize(20);

  for (int i = 0; i < NETACUT; i++)
    hdijet_eff_pT1[i]->SetLineColor(etaColor[i]);

//-- legends
  TLegend *leg_jetpt = new TLegend(0.6, 0.5, 0.95, 0.95);
  leg_jetpt->SetFillStyle(0);
  leg_jetpt->SetBorderSize(0);
  leg_jetpt->SetTextFont(63);
  leg_jetpt->SetTextSize(12);
  leg_jetpt->AddEntry(hjet_pT, "Inclusive b - jet", "L");
  leg_jetpt->AddEntry(hdijet_pT1, "leading b - jet", "L");
  leg_jetpt->AddEntry(hdijet_pT2, "subleading b - jet", "L");


//-- other
  TLatex ltxt;
  ltxt.SetNDC();
  ltxt.SetTextFont(63);
  ltxt.SetTextSize(20);

//==========================================================================//
// PLOT
//==========================================================================//

// plot b-jet kinematics
  TCanvas *cjet = new TCanvas("cjet", "b - jet", 1200, 400);
  cjet->SetMargin(0, 0, 0, 0);
  cjet->Divide(3, 1);

  cjet->GetPad(1)->SetMargin(0.12, 0.02, 0.12, 0.02);
  cjet->GetPad(2)->SetMargin(0.12, 0.02, 0.12, 0.02);
  cjet->GetPad(3)->SetMargin(0.12, 0.02, 0.12, 0.02);

  cjet->cd(1);
  gPad->SetLogy();
  hjet_pT->GetYaxis()->SetRangeUser(0.5, 1.1 * hjet_pT->GetMaximum());
  hjet_pT->GetXaxis()->SetRangeUser(0, 50);
  hjet_pT->Draw();
  hdijet_pT1->Draw("same");
  hdijet_pT2->Draw("same");

  leg_jetpt->Draw("same");

  cjet->cd(2);
  hjet_eta->Draw("HIST");

  cjet->cd(3);
  hjet_phi->Draw("HIST");


// plot dijet eff
  TCanvas *ceff = new TCanvas("ceff", "eff", 1200, 600);
  ceff->Divide(2, 1);
  ceff->GetPad(1)->SetMargin(0.12, 0.02, 0.12, 0.02);
  ceff->GetPad(1)->SetTicks(1, 1);
  ceff->GetPad(2)->SetMargin(0.12, 0.02, 0.12, 0.02);
  ceff->GetPad(2)->SetTicks(1, 1);

  ceff->cd(1);

  heff_axis_pt->Draw();

  for (int i = 0; i < NETACUT; i++)
    hdijet_eff_pT1[i]->Draw("L same");

  ceff->cd(2);

  heff_axis_eta->Draw();

  for (int i = 0; i < NETACUT; i++)
    hdijet_eff_eta2[i]->Draw("L same");

// plot dijet checks
  TCanvas *cdijet2 = new TCanvas("cdijet2", "dijet", 1200, 600);
  cdijet2->SetMargin(0, 0, 0, 0);
  cdijet2->Divide(2, 1);

  cdijet2->GetPad(1)->SetMargin(0.12, 0.02, 0.12, 0.02);
  cdijet2->GetPad(2)->SetMargin(0.12, 0.02, 0.12, 0.02);

  cdijet2->cd(1);
  hdijet_xj->Draw();

  ltxt.DrawLatex(0.2, 0.92, "Di b-jets (Pythia8)");
  ltxt.DrawLatex(0.2, 0.86, "p + p #sqrt{s_{_{NN}}}=200 GeV");
  ltxt.DrawLatex(0.2, 0.80, "anti - k_ {T}, R = 0.4");
  ltxt.DrawLatex(0.2, 0.74, Form("p_{T, 1} > % .0f GeV", pT1min));
  ltxt.DrawLatex(0.2, 0.68, Form("p_{T, 2} > % .0f GeV", pT2min));
  ltxt.DrawLatex(0.2, 0.62, Form(" | #Delta#phi_{12}| > 2#pi/3"));

  cdijet2->cd(2);
  hdijet_dphi->Draw();

  TCanvas *cdijet = new TCanvas("cdijet", "dijet", 600, 600);
  cdijet->SetMargin(0.12, 0.02, 0.12, 0.02);
  cdijet->SetTicks(1, 1);
  cdijet->cd();
  hdijet_xj->Draw();

  ltxt.DrawLatex(0.2, 0.92, "Di b-jets (Pythia8)");
  if (is_pp)
  {
    ltxt.DrawLatex(0.2, 0.86, "p + p #sqrt{s_{_{NN}}} = 200 GeV");
    ltxt.DrawLatex(0.2, 0.80, "#int L dt = 175 pb^{-1} |z| < 10 cm");
  }
  else
  {
    ltxt.DrawLatex(0.2, 0.86, "0-10% Au + Au #sqrt{s_{_{NN}}}=200 GeV");
    ltxt.DrawLatex(0.2, 0.80, "10B events |z| < 10 cm");
  }
  ltxt.DrawLatex(0.2, 0.74, "anti-k_{T}, R = 0.4");
  ltxt.DrawLatex(0.2, 0.68, Form("p_{T, 1} > % .0f GeV", pT1min));
  ltxt.DrawLatex(0.2, 0.62, Form("p_{T, 2} > % .0f GeV", pT2min));
  ltxt.DrawLatex(0.2, 0.56, Form(" | #Delta#phi_{12}| > 2#pi/3"));

  if (is_pp)
    cdijet->Print("dibjet_imbalance_pp.pdf");
  else
    cdijet->Print("dibjet_imbalance_AuAu0-10.pdf");


}