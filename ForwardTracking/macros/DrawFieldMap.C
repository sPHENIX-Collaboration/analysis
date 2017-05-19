// $Id: $

/*!
 * \file DrawFieldMap.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#include "SaveCanvas.C"

void DrawFieldMap()
{
  gROOT->LoadMacro("sPhenixStyle.C");
  SetsPhenixStyle();

  TFile *_file0 = TFile::Open("/phenix/upgrades/decadal/fieldmaps/fsPHENIX.2d.root");

  TH2F *hfield = new TH2F("hfield", "hfield", 480 / 2, -30, 450, 280 / 2, 0, 280);
  TH2F *hbendingfield = hfield->Clone("hbendingfield");

  fieldmap->SetAlias("perb_angle", "atan2(r,z) + pi/2");
  fieldmap->SetAlias("bendingfield", "br*cos(perb_angle) + bz*sin(perb_angle)");

  fieldmap->Draw("r:z>>hfield", "sqrt(br**2 + bz**2)", "goff");
  fieldmap->Draw("r:z>>hbendingfield", "bendingfield", "goff");

  TCanvas *c1 = new TCanvas("DrawFieldMap_hfield", "DrawFieldMap_hfield", 1800,
                            900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad *p;
  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetRightMargin(0.2);

  hfield->GetZaxis()->SetRangeUser(0., 2.5);
  hfield->SetTitle("#it{#bf{sPHENIX}} Field Map;z [cm];R [cm];Field strength [T]");
  hfield->GetYaxis()->SetTitleOffset(0.9);
  hfield->Draw("colz");

  //  %h-going ECAL
  //  \newcommand{ \hecalfsphenix }
  //  {
  //
  //    % towerMap_FEMC_v005.txt
  //    \draw[fill=blizzardblue] ( 3.28250, 1.82655 ) -- ( 3.28250, 0.1 ) -- ( 2.91750, 0.1 ) -- ( 2.91750, 1.82655 ) -- ( 3.28250, 1.82655 );
  //
  //  }
  TBox *b = new TBox(2.91750 * 100, 0.1 * 100, 3.2 * 100, 1.82655 * 100);
  b->SetFillColor(0);
  b->SetFillStyle(0);
  b->SetLineColor(kGray);
  b->SetLineWidth(4);
  b->Draw();
  //
  //  %h-going HCAL
  //  \newcommand{ \hhcalfsphenix }
  //  {
  //
  //    % towerMap_FHCAL_v004.txt
  //  \draw[fill=bluegray] ( 4.5, 2.62 ) -- ( 4.5, 0.05 ) -- ( 3.5, 0.05 ) -- ( 3.5, 2.62 ) -- ( 4.5, 2.62 );
  //
  //  }
  TBox *b = new TBox(3.5 * 100, 0.05 * 100, 4.5 * 100, 2.62 * 100);
  b->SetFillColor(0);
  b->SetFillStyle(0);
  b->SetLineColor(kGray);
  b->SetLineWidth(4);
  b->Draw();

  //  %h-going GEM fsPhenix
  //  \newcommand{ \hgemfsphenix }
  //  {
  //
  //  \draw[ultra thick,bazaar] ( 1.2, 0.1 ) -- ( 1.2, 0.8 );
  //  \draw[ultra thick,bazaar] ( 1.5, 0.1 ) -- ( 1.5, 1 );
  //  \draw[ultra thick,bazaar] ( 2.7, 0.1 ) -- ( 2.7, 0.8 ) -- ( 2.5, 1.3);
  //
  //  }
  TLine *l = new TLine(1.2 * 100,0.1 * 100,  1.2 * 100, 0.8 * 100);
  l->SetLineColor(kGray);
  l->SetLineWidth(4);
  l->Draw();
  TLine *l = new TLine(1.5 * 100,0.1 * 100,  1.5 * 100, 1 * 100);
  l->SetLineColor(kGray);
  l->SetLineWidth(4);
  l->Draw();
  TLine *l = new TLine(2.7 * 100,0.1 * 100,  2.7 * 100, 0.8 * 100);
  l->SetLineColor(kGray);
  l->SetLineWidth(4);
  l->Draw();
  TLine *l = new TLine(2.7 * 100, 0.8 * 100, 2.5 * 100, 1.3 * 100);
  l->SetLineColor(kGray);
  l->SetLineWidth(4);
  l->Draw();


  TBox *b = new TBox(-30 , 0.02 * 100, 100, 78);
  b->SetFillColor(0);
  b->SetFillStyle(0);
  b->SetLineColor(kGray);
  b->SetLineWidth(4);
  b->Draw();


  SaveCanvas(c1, TString(c1->GetName()), kTRUE);
  //  c1->Print(TString(c1->GetName())+".pdf");
  c1->Print(TString(c1->GetName()) + ".eps");
  //  c1->Print(TString(c1->GetName())+".svg");

  //  TCanvas *c1 = new TCanvas("DrawFieldMap_hbendingfield", "DrawFieldMap_hbendingfield", 1800,
  //                            900);
  //  c1->Divide(1, 1);
  //  int idx = 1;
  //  TPad *p;
  //  p = (TPad *) c1->cd(idx++);
  //  c1->Update();
  //
  //  p->SetRightMargin(0.1);
  //
  //  hbendingfield->Draw("colz");

  //  SaveCanvas(c1, TString(c1->GetName()), kTRUE);
}
