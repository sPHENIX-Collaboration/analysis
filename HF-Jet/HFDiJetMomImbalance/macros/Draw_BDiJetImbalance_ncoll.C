////////////////////////////////////////////////////////////////////////////////
//
// Draw the Di b-jet <xj> vs Ncoll
//
// Useful references:
// CMS di b-jet: http://cds.cern.ch/record/2202805/files/HIN-16-005-pas.pdf
//
////////////////////////////////////////////////////////////////////////////////
//
// Darren McGlinchey
// 29 Jun 2017
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
#include <TGraphErrors.h>

#include <iostream>

using namespace std;

void calc_mxj(TH1D* htmp, double &mxj, double &mxj_e)
{

  double sum = 0;
  double w = 0;
  double err = 0;
  for (int i = 1; i <= htmp->GetNbinsX(); i++)
  {
    double c = htmp->GetBinContent(i);
    if (c > 0)
    {
      sum += c * htmp->GetBinCenter(i);
      w += c;
      err += TMath::Power(c * htmp->GetBinError(i), 2);
    }
  }
  double mean = sum / w;
  err = TMath::Sqrt(err) / w;

  mxj = mean;
  mxj_e = err;
}

void Draw_BDiJetImbalance_ncoll()
{
  //==========================================================================//
  // SET RUNNING CONDITIONS
  //==========================================================================//
  bool print_plots = true;

  const char* ppFile = "dibjet_imbalance_histos_pp.root";
  const char* auauFile = "dibjet_imbalance_histos_AuAu0-10.root";

  const int NCENT = 4;
  const char* centlim[] = {"0-10%", "10-20%", "20-40%", "40-60%", "60-92%"};
  double ncoll[] = {962, 603, 296, 94, 15};
  double lcent[] = {549, 344, 338, 107, 27}; // nn luminosity (for scaling)


  //==========================================================================//
  // DECLARE VARIABLES
  //==========================================================================//

  TH1D* hxj_pp;
  TH1D* hxj_auau[NCENT];

  TGraphErrors *gmxj = new TGraphErrors();
  gmxj->SetMarkerStyle(kFullCircle);
  gmxj->SetMarkerColor(kBlack);
  gmxj->SetLineColor(kBlack);


  double mxj_pp;
  double mxj_e_pp;

  double mxj_auau[NCENT];
  double mxj_e_auau[NCENT];

  //-- other
  TFile *fin;
  char name[500];

  //==========================================================================//
  // READ RESULTS FROM FILE
  //==========================================================================//
  cout << endl;
  cout << "--> Reading results from file" << endl;

  cout << "----> Reading pp from " << ppFile << endl;
  fin = TFile::Open(ppFile);
  if (!fin)
  {
    cout << "ERROR!! Unable to read " << ppFile << endl;
    return;
  }

  sprintf(name, "hdijet_xj_fix");
  hxj_pp = (TH1D*) fin->Get(name);
  if ( !hxj_pp )
  {
    cout << "ERRRO! Unable to find " << name << " in " << ppFile << endl;
    return;
  }
  hxj_pp->SetDirectory(0);
  hxj_pp->SetName("hxj_pp");

  fin->Close();
  delete fin;



  cout << "----> Reading auau from " << auauFile << endl;
  fin = TFile::Open(auauFile);
  if (!fin)
  {
    cout << "ERROR!! Unable to read " << auauFile << endl;
    return;
  }

  sprintf(name, "hdijet_xj_fix");
  hxj_auau[0] = (TH1D*) fin->Get(name);
  if ( !hxj_auau[0] )
  {
    cout << "ERRRO! Unable to find " << name << " in " << auauFile << endl;
    return;
  }
  hxj_auau[0]->SetDirectory(0);
  hxj_auau[0]->SetName("hxj_auau_0");

  fin->Close();
  delete fin;

  //==========================================================================//
  // SCALE UNCERTAINTIES FOR OTHER CENTRALITY BINS
  //==========================================================================//
  cout << endl;
  cout << "--> Scaling AuAu centrality bins" << endl;

  for (int icent = 1; icent < NCENT; icent++)
  {
    sprintf(name, "hxj_auau_%i", icent);
    hxj_auau[icent] = (TH1D*) hxj_auau[0]->Clone(name);

    double sf = 1./TMath::Sqrt(lcent[icent] / lcent[0]);
    for (int ix = 1; ix <= hxj_auau[icent]->GetNbinsX(); ix++)
    {
      double be = hxj_auau[icent]->GetBinError(ix);

      hxj_auau[icent]->SetBinError(ix, be * sf);
    } // ix
  }


  //==========================================================================//
  // CALCULATE <XJ>
  //==========================================================================//
  cout << endl;
  cout << "--> Calculating <x_j>" << endl;

  //-- pp
  calc_mxj(hxj_pp, mxj_pp, mxj_e_pp);

  gmxj->SetPoint(0, 1, mxj_pp);
  gmxj->SetPointError(0, 0, mxj_e_pp);

  //-- AuAu
  for (int i = 0; i < NCENT; i++)
  {
    calc_mxj(hxj_auau[i], mxj_auau[i], mxj_e_auau[i]);

    gmxj->SetPoint(i + 1, ncoll[i], mxj_auau[i]);
    gmxj->SetPointError(i + 1, 0, mxj_e_auau[i]);

  } // i


  //==========================================================================//
  // PLOT OBJECTS
  //==========================================================================//

  TH1D* haxis = new TH1D("haxis", ";N_{coll};#LTx_{j}#GT", 1200, -100, 1100);
  // haxis->SetMinimum(0.551);
  // haxis->SetMaximum(0.749);
  haxis->SetMinimum(0.601);
  haxis->SetMaximum(0.779);

  TLatex lbl;
  lbl.SetTextAlign(22);

  //==========================================================================//
  // PLOT
  //==========================================================================//

  TCanvas *cxj = new TCanvas("cxj", "xj", 800, 600);

  cxj->cd();
  // gPad->SetLogx();
  haxis->Draw();

  gmxj->Draw("P");

  // cent labels
  lbl.DrawLatex(1, mxj_pp + 0.02, "p+p");

  for (int i = 0; i < NCENT; i++)
    lbl.DrawLatex(ncoll[i], mxj_auau[i] + 0.02 + 0.01*(i%2), centlim[i]);

  TLegend *leg = new TLegend(0.10, 0.20, 0.70, 0.55);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->AddEntry("", "#it{#bf{sPHENIX}} Simulation", "");
  leg->AddEntry("", "#sqrt{s_{_{NN}}} = 200 GeV", "");
  leg->AddEntry("", "Di b-jets (Pythia8, CTEQ6L)", "");
  leg->AddEntry("", "anti-k_{T}, R = 0.4, |#eta|<0.7", "");
  leg->AddEntry("", "p+p: 200 pb^{-1}, 60% Eff., 40% Pur.", "");
  leg->AddEntry("", "Au+Au: 240B Events, 40% Eff., 40% Pur.", "");
  // leg->AddEntry("", "200 pb^{-1} p+p, 240B Au+Au", "");
  leg->Draw("same");

  //==========================================================================//
  // PRINT PLOTS
  //==========================================================================//
  if ( print_plots )
  {
    cout << endl;
    cout << "--> Printing results" << endl;

    cout << "p+p <xj> = " << mxj_pp << " +/- " << mxj_e_pp << endl;
    for (int i = 0; i < NCENT; i++)
    {
      cout << "Au+Au " << centlim[i] << " <xj> = " << mxj_auau[i] << " +/- " << mxj_e_auau[i] << endl;
    }

    cxj->Print("dibjet_imbalance_ncoll.pdf");
    cxj->Print("dibjet_imbalance_ncoll.C");
    cxj->Print("dibjet_imbalance_ncoll.root");
  }





}