// $Id: $                                                                                             

/*!
 * \file Draw.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#include <cmath>
#include <TFile.h>
#include <TString.h>
#include <TLine.h>
#include <TTree.h>
#include <TLatex.h>
#include <TGraphErrors.h>
#include <cassert>
#include "SaveCanvas.C"
#include "SetOKStyle.C"
using namespace std;

void
DrawEMCalTower_LineShape( //
    const TString base =
        "../../sPHENIX_work/Prototype_2016/test_sim/15Degree_1Col_No0Sup/", //
    const double E = 12)
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  const TString sE = Form("%.0f", E);

  TText * t;
  TCanvas *c1 = new TCanvas("DrawEMCalTower_LineShape_" + sE,
      "DrawEMCalTower_LineShape_" + sE, 1000, 600);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  p->DrawFrame(0, .0008, E * 1.3, 1, Form("Line shape for %.1f GeV beam;Energy Sum (GeV);Probability / bin", E));

  MakeOnePlot(base, "e-", sE, kGreen + 3)->DrawClone("same");
  MakeOnePlot(base, "pi-", sE, kRed )->DrawClone("same");
  MakeOnePlot(base, "kaon-", sE, kBlue )->DrawClone("same");
  MakeOnePlot(base, "mu-", sE, kBlack )->DrawClone("same");

  SaveCanvas(c1, base + TString(c1->GetName()), kTRUE);
}

TH1 *
MakeOnePlot(const TString base, const TString particle, const TString sE,
    const Color_t color)
{

  TString fname = base + TString("Prototype_") + particle + "_" + sE
      + "_Seg0_DSTReader.root_DrawEMCalTower_EMCDistribution_SUMall_event.root";
//  Prototype_e-_2_Seg0_DSTReader.root_DrawEMCalTower_EMCDistribution_SUMall_event.root

  TFile * f = new TFile(fname);
  assert(f->IsOpen());

  TH1 * hEnergySum = (TH1 *) f->GetObjectChecked("hEnergySum", "TH1");
  assert(hEnergySum);

  hEnergySum->Scale(1. / hEnergySum->Integral(1, -1));
  hEnergySum->GetFunction("fgaus")->Delete();
  hEnergySum->SetStats(false);

  hEnergySum->SetFillColor(color);
  hEnergySum->SetLineColor(color);
  hEnergySum->SetMarkerColor(color);

  return hEnergySum;
}
