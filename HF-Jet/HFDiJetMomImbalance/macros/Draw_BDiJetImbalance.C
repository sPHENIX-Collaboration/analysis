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
#include <TChain.h>
#include <TTree.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TMath.h>
#include <TLegend.h>
#include <TRandom3.h>
#include <TString.h>
#include <TSystem.h>

#include <iostream>

using namespace std;

void Draw_BDiJetImbalance()
{
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);

  //==========================================================================//
  // SET RUNNING CONDITIONS
  //==========================================================================//
  bool print_plots = true;

  bool is_pp = false;

  // const char* inFile = "HFtag_bjet.root";
  // const char* inFile = "HFtag_bjet_pp.root";
  // const char* inFile = "/phenix/plhf/dcm07e/sPHENIX/bjetsims/test.root";
  // if (!is_pp)
  //   const char* inFile = "HFtag_bjet_AuAu0-10.root";

  const char* inDir = "/phenix/plhf/dcm07e/sPHENIX/bjetsims/ana";

  // // desired nn integrated luminosity [pb^{-1}]
  // double lumiDesired = 175;
  // if ( !is_pp )
  //   lumiDesired = 229; // AuAu 0-10% central (10B events)
  // desired nn integrated luminosity [pb^{-1}]
  double lumiDesired = 200;
  TString scol("p + p #sqrt{s_{_{NN}}} = 200 GeV");
  TString slumi(Form("#int L dt = %.0f pb^{-1} |z| < 10 cm", lumiDesired));
  if ( !is_pp )
  {
    // equivelant nn luminosity:
    // N_{evt}^{NN} / sigma_NN = N_{evt}^{AA}*Ncoll / sigma_NN
    // = 10e9 * 962 / 42e9 pb = 229 pb^-1
    double evntDesired = 24e9; // AuAu 0-10% central
    lumiDesired = evntDesired * 962 / 42e9; // AuAu 0-10% central
    scol = "0-10% Au + Au #sqrt{s_{_{NN}}}=200 GeV";
    slumi = Form("%.0fB events |z| < 10 cm", evntDesired / 1.e9);
  }

  // luminosity per file generated [pb^{-1}]
  double lumiPerFile = 99405 / (4.641e-06 * 1e12);
  double lumiRead = 0;

  double pT1min = 20;
  double pT2min = 10;
  // double etamax = 1.0;
  double etamax = 0.7;

  // double bJetEff = 0.5; // b-jet tagging efficiency
  double bJetEff = 0.60; // p+p b-jet tagging efficiency
  if ( !is_pp )
    bJetEff = 0.40;

  double bJetPur = 0.40; // b-jet tagging purity

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
  TChain *ttree;
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

  // for fixing the uncertainties
  TH1D* hdijet_xj_fix;

  //-- other
  TRandom3 *rand = new TRandom3();

  bool acc[100];

  //==========================================================================//
  // LOAD TTREE
  //==========================================================================//
  // cout << endl;
  // cout << "--> Reading data from " << inFile << endl;

  // TFile *fin = TFile::Open(inFile);
  // if (!fin)
  // {
  //   cout << "ERROR!! Unable to open " << inFile << endl;
  //   return;
  // }

  // ttree = (TTree*) fin->Get("ttree");
  // if (!ttree)
  // {
  //   cout << "ERROR!! Unable to find ttree in " << inFile << endl;
  //   return;
  // }

  cout << endl;
  cout << "--> Reading data from dir: " << inDir << endl;

  // calculate the number of files necessary for the desired luminosity
  int nfiles = (int)(lumiDesired / lumiPerFile);
  cout << "  desired luminosity : " << lumiDesired << endl;
  cout << "  luminosity per file: " << lumiPerFile << endl;
  cout << "  N files needed     : " << nfiles << endl;


  // check the number of files found in the directory
  int nfound = (gSystem->GetFromPipe(Form("ls %s/*.root | wc -l", inDir))).Atoi();
  cout << "  N files found      : " << nfound << endl;

  if (nfound < nfiles)
  {
    cout << "WARNING!! There are not enough files for the desired luminosity." << endl;
    cout << "          Will scale the statistical uncertainties accordingly." << endl;
    // return;
  }

  // chain the desired files
  ttree = new TChain("ttree");
  int nread = 0;

  TString fileList = gSystem->GetFromPipe(Form("ls %s/*.root", inDir));

  int spos = fileList.First("\n");
  while (spos > 0 && nread < nfiles)
  {
    TString tfile = fileList(0, spos);
    // cout << tsub << endl;

    ttree->Add(tfile.Data());


    // chop off the file
    fileList = fileList(spos + 1, fileList.Length());
    spos = fileList.First("\n");
    nread++;
  }

  cout << " successfully read in " << nread << " files" << endl;
  lumiRead = lumiPerFile * nread;


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

    if (ientry % 100000 == 0) cout << "----> Processing event " << ientry << endl;

    //-- apply acceptance to each jet
    for (int i = 0; i < truthjet_n; i++)
      acc[i] = (rand->Uniform() < bJetEff);
      // acc[i] = (rand->Uniform() < bJetEff) && (rand->Uniform() < RAA);

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
        // only take RAA hit once
        if ( pT1 < pT1min || TMath::Abs(eta1) > etamax || !acc1  || rand->Uniform() > RAA)
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

  //-- scale statistical uncertainties if not enough simulated events
  if ( lumiRead < lumiDesired )
  {
    cout << endl;
    cout << "-- Scaling statistical uncertainties by:" << endl;
    cout << "  sqrt(" << lumiRead << "/" << lumiDesired << ") = "
         << TMath::Sqrt(lumiRead / lumiDesired) << endl;
    for (int ix = 1; ix <= hdijet_xj->GetNbinsX(); ix++)
    {
      double be = hdijet_xj->GetBinError(ix);
      be = be * TMath::Sqrt(lumiRead / lumiDesired);
      hdijet_xj->SetBinError(ix, be);
    } // ix
  }

  //-- fix statistical uncertainties for purity
  hdijet_xj_fix = (TH1D*) hdijet_xj->Clone("hdijet_xj_fix");
  hdijet_xj_fix->SetLineColor(kRed);
  hdijet_xj_fix->SetMarkerColor(kRed);
  for (int ix = 1; ix <= hdijet_xj->GetNbinsX(); ix++)
  {
    double bc = hdijet_xj->GetBinContent(ix);
    double be = hdijet_xj->GetBinError(ix);

    // increase the bin error due to the purity
    // need to square it, once for each bjet
    be = be / TMath::Sqrt(bJetPur * bJetPur);
    hdijet_xj_fix->SetBinError(ix, be);
  } // ix



  //==========================================================================//
  // <x_j>
  //==========================================================================//
  cout << endl;
  cout << "--> Calculating <x_j>" << endl;

  double sum = 0;
  double w = 0;
  double err = 0;
  for (int i = 1; i <= hdijet_xj->GetNbinsX(); i++)
  {
    double c = hdijet_xj->GetBinContent(i);
    if (c > 0)
    {
      sum += c * hdijet_xj->GetBinCenter(i);
      w += c;
      err += TMath::Power(c * hdijet_xj->GetBinError(i), 2);
    }
  }
  double mean = sum / w;
  err = TMath::Sqrt(err) / w;

  cout << "  <x_j>=" << mean << " +/- " << err << endl;

  //==========================================================================//
  // PLOT OBJECTS
  //==========================================================================//
  cout << endl;
  cout << "-- > Plotting" << endl;

  hdijet_xj->SetMarkerStyle(kFullCircle);
  hdijet_xj->SetMarkerColor(kBlack);
  hdijet_xj->SetLineColor(kBlack);
  // hdijet_xj->GetYaxis()->SetTitleFont(63);
  // hdijet_xj->GetYaxis()->SetTitleSize(24);
  // hdijet_xj->GetYaxis()->SetTitleOffset(1.4);
  // hdijet_xj->GetYaxis()->SetLabelFont(63);
  // hdijet_xj->GetYaxis()->SetLabelSize(20);
  // hdijet_xj->GetXaxis()->SetTitleFont(63);
  // hdijet_xj->GetXaxis()->SetTitleSize(24);
  // hdijet_xj->GetXaxis()->SetTitleOffset(1.0);
  // hdijet_xj->GetXaxis()->SetLabelFont(63);
  // hdijet_xj->GetXaxis()->SetLabelSize(20);

  hdijet_xj_fix->SetMarkerStyle(kFullCircle);
  hdijet_xj_fix->SetMarkerColor(kBlack);
  hdijet_xj_fix->SetLineColor(kBlack);

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
  hjet_pT->GetYaxis()->SetRangeUser(0.5, 1.5 * hjet_pT->GetMaximum());
  hjet_pT->GetXaxis()->SetRangeUser(0, 50);
  hjet_pT->Draw();
  // hdijet_pT1->Draw("same");
  // hdijet_pT2->Draw("same");

  // leg_jetpt->Draw("same");

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






  // make this one consistent with sPHENIX style
  TCanvas *cdijet = new TCanvas("cdijet", "dijet", 600, 600);
  // cdijet->SetMargin(0.12, 0.02, 0.12, 0.02);
  // cdijet->SetTicks(1, 1);
  cdijet->cd();
  hdijet_xj_fix->GetYaxis()->SetRangeUser(0, 0.3);
  hdijet_xj_fix->Draw();
  // hdijet_xj->Draw("same");


  TLegend *legsphenix = new TLegend(0.15, 0.5, 0.5, 0.9);
  legsphenix->SetFillStyle(0);
  legsphenix->SetBorderSize(0);
  legsphenix->AddEntry("", "#it{#bf{sPHENIX}} Simulation", "");
  legsphenix->AddEntry("", "Di b-jets (Pythia8, CTEQ6L)", "");
  // if (is_pp)
  // {
  //   legsphenix->AddEntry("", "p + p #sqrt{s_{_{NN}}} = 200 GeV", "");
  //   legsphenix->AddEntry("", Form("#int L dt = %.0f pb^{-1} |z| < 10 cm", lumiDesired), "");
  // }
  // else
  // {
  //   legsphenix->AddEntry("", "0-10% Au + Au #sqrt{s_{_{NN}}}=200 GeV", "");
  //   legsphenix->AddEntry("", "10B events |z| < 10 cm", "");
  // }
  legsphenix->AddEntry("", scol.Data(), "");
  legsphenix->AddEntry("", slumi.Data(), "");
  legsphenix->AddEntry("", "anti-k_{T}, R = 0.4", "");
  legsphenix->AddEntry("", Form(" |#eta| < %.1f", etamax), "");
  legsphenix->AddEntry("", Form(" |#Delta#phi_{12}| > 2#pi/3"), "");
  legsphenix->AddEntry("", Form("p_{T, 1} > % .0f GeV", pT1min), "");
  legsphenix->AddEntry("", Form("p_{T, 2} > % .0f GeV", pT2min), "");
  legsphenix->AddEntry("", Form("%.0f%% b-jet Eff, %.0f%% b-jet Pur.", bJetEff * 100., bJetPur * 100.), "");
  legsphenix->Draw("same");

  // ltxt.DrawLatex(0.2, 0.92, "Di b-jets (Pythia8)");
  // if (is_pp)
  // {
  //   ltxt.DrawLatex(0.2, 0.86, "p + p #sqrt{s_{_{NN}}} = 200 GeV");
  //   ltxt.DrawLatex(0.2, 0.80, "#int L dt = 175 pb^{-1} |z| < 10 cm");
  // }
  // else
  // {
  //   ltxt.DrawLatex(0.2, 0.86, "0-10% Au + Au #sqrt{s_{_{NN}}}=200 GeV");
  //   ltxt.DrawLatex(0.2, 0.80, "10B events |z| < 10 cm");
  // }
  // ltxt.DrawLatex(0.2, 0.74, "anti-k_{T}, R = 0.4");
  // ltxt.DrawLatex(0.2, 0.68, Form("p_{T, 1} > % .0f GeV", pT1min));
  // ltxt.DrawLatex(0.2, 0.62, Form("p_{T, 2} > % .0f GeV", pT2min));
  // ltxt.DrawLatex(0.2, 0.56, Form(" |#eta| < %.0f", etamax));
  // ltxt.DrawLatex(0.2, 0.50, Form(" |#Delta#phi_{12}| > 2#pi/3"));


  //==========================================================================//
  // PRINT PLOTS
  //==========================================================================//
  if (print_plots)
  {
    if (is_pp)
    {
      // cjet->Print("bjet_kinematics_pp.pdf");
      // cdijet2->Print("dibjet_2panel_pp.pdf");
      cdijet->Print("dibjet_imbalance_pp.pdf");
      cdijet->Print("dibjet_imbalance_pp.C");
      cdijet->Print("dibjet_imbalance_pp.root");

      TFile *fout = new TFile("dibjet_imbalance_histos_pp.root","RECREATE");
      fout->cd();
      hdijet_xj->Write();
      hdijet_xj_fix->Write();
      fout->Close();
      delete fout;

    }
    else
    {
      cdijet->Print("dibjet_imbalance_AuAu0-10.pdf");
      cdijet->Print("dibjet_imbalance_AuAu0-10.C");
      cdijet->Print("dibjet_imbalance_AuAu0-10.root");

      TFile *fout = new TFile("dibjet_imbalance_histos_AuAu0-10.root","RECREATE");
      fout->cd();
      hdijet_xj->Write();
      hdijet_xj_fix->Write();
      fout->Close();
      delete fout;
    }
  }

}