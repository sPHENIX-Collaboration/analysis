// $Id: $

/*!
 * \file Draw_HFJetTruth.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TGraphErrors.h>
#include <TLatex.h>
#include <TLine.h>
#include <TString.h>
#include <TTree.h>
#include <cassert>
#include <cmath>
#include "SaveCanvas.C"
#include "sPhenixStyle.C"

TFile *_file0 = NULL;
TTree *T = NULL;
int total_event = 0;

void Draw_EICRate(const TString infile =
                      //    "/phenix/u/jinhuang/links/sPHENIX_work/EIC/DAQ/DIS/DIS_3197.cfg_DSTReader.root",
                  //    "/phenix/u/jinhuang/links/sPHENIX_work/EIC/DAQ/DIS/DIS_ALL.cfg_DSTReader.root",
                  //                  double xsection = 6.613184211e-4  // mb for 10 f + f' -> f + f' (QFD)
                  //    "/phenix/u/jinhuang/links/sPHENIX_work/EIC/DAQ/DIS2/DIS2_3197.cfg_DSTReader.root",
                  //    "/phenix/u/jinhuang/links/sPHENIX_work/EIC/DAQ/DIS2/DIS2_ALL.cfg_DSTReader.root",
                  //    "/phenix/u/jinhuang/links/sPHENIX_work/EIC/DAQ/DIS/DIS_ALL.cfg_DSTReader.root",
                  //                  double xsection = 1.05E-03  // mb for all subprocesses
                  //                  "/phenix/u/jinhuang/links/sPHENIX_work/EIC/DAQ/DIS20x250/DIS20x250_3179.cfg_DSTReader.root",
                  //    "/phenix/u/jinhuang/links/sPHENIX_work/EIC/DAQ/DIS20x250/DIS20x250_ALL.cfg_DSTReader.root",
                  //    double xsection = 1.15E-03  // mb for all subprocesses
                  "/phenix/u/jinhuang/links/sPHENIX_work/EIC/DAQ/pythiaEIC_MB/ALL_DSTReader.root",
                  //                  "/phenix/u/jinhuang/links/sPHENIX_work/EIC/DAQ/pythiaEIC_MB/49610_DSTReader.root",
                  double xsection = 5.488E-02  //  I   0 All included subprocesses    I      1000000      30948998 I  5.488D-02 I
)
{
  SetsPhenixStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");

  gSystem->Load("libg4eval.so");

  if (!_file0)
  {
    TString chian_str = infile;
    chian_str.ReplaceAll("ALL", "*");

    TChain *t = new TChain("T");
    const int n = t->Add(chian_str);

    cout << "Loaded " << n << " root files with " << chian_str << endl;
    assert(n > 0);
    T = t;

    _file0 = new TFile;
    _file0->SetName(infile);
  }

  const double lumi = 1e33;  // cm^-2/s
  const double event_rate = (xsection * 1e-3 * 1e-24) * lumi;
  total_event = T->GetEntries();
  const double time = total_event / event_rate;

  cout << "Config summary: " << endl;
  cout << "lumi = " << lumi << "/cm2/s" << endl;
  cout << "xsection = " << xsection << " mb" << endl;
  cout << "event_rate = " << event_rate << " hz" << endl;
  cout << "total_event = " << total_event << "" << endl;
  cout << "time = " << time << "s" << endl;

  T->SetAlias("p_Q2", "PHG4Particle.fpx**2 + PHG4Particle.fpy**2 + (PHG4Particle.fpz+20)**2 - (PHG4Particle.fe-20)**2");
  T->SetAlias("PHG4Particle_p",
              "1*sqrt(PHG4Particle.fpx**2+PHG4Particle.fpy**2+PHG4Particle.fpz**2)");
  T->SetAlias("PHG4Particle_pT",
              "1*sqrt(PHG4Particle.fpx**2+PHG4Particle.fpy**2)");
  T->SetAlias("p_eta",
              "0.5*log((PHG4Particle_p+PHG4Particle.fpz)/(PHG4Particle_p-PHG4Particle.fpz))");
  T->SetAlias("SVTXHitLength",
              "1*sqrt((G4HIT_SVTX.get_x(0) - G4HIT_SVTX.get_x(1))**2 + (G4HIT_SVTX.get_y(0) - G4HIT_SVTX.get_y(1))**2 + (G4HIT_SVTX.get_z(0) - G4HIT_SVTX.get_z(1))**2)");

  //  KinematicsChecks();
  TrackerRate();

  CentralCalorimeterRate();
  ForwardCalorimeterRate();
}

void TrackerRate()
{
  TText *t;
  TCanvas *c1 = new TCanvas("TrackerRate",
                            "TrackerRate", 1900, 600);

  c1->Divide(3, 1, 0, 0);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("Sum$(G4HIT_MAPS.edep>0)>>hMAPSHit(200,-.5,199.5)");
  TH1 *h = (TH1 *) gDirectory->Get("hMAPSHit");
  assert(h);
  h->SetTitle(";# of MAPS vertex tracker hit per event;Count [A.U.]");
  h->SetMaximum(h->GetMaximum() * 10);
  h->GetXaxis()->SetRangeUser(0, 100);
  h->GetXaxis()->SetRangeUser(0, 1000);
  double meanhit = h->GetMean();

  TLegend *leg = new TLegend(.2, .70, .95, .93);
  leg->AddEntry("", "#it{#bf{EIC-sPHENIX}} Simualtion", "");
  leg->AddEntry("", "e+p, 20+250 GeV/c, #sqrt{s_{ep}}=140 GeV", "");
  leg->AddEntry(h, Form("Average MAPS hit / event = %.1f", meanhit), "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("Sum$(G4HIT_SVTX.edep>1e-7 && SVTXHitLength>1)>>hSVTXHit(2000,-.5,1999.5)");
  TH1 *h = (TH1 *) gDirectory->Get("hSVTXHit");
  h->SetTitle(";# of TPC hit per event;Count [A.U.]");
  h->SetMaximum(h->GetMaximum() * 10);
  h->GetXaxis()->SetRangeUser(0, 1000);
  double meanhit = h->GetMean();

  TLegend *leg = new TLegend(.1, .80, .95, .93);
  //  leg->AddEntry("", "#it{#bf{EIC-sPHENIX}} Simualtion", "");
  //  leg->AddEntry("", "e+p, 20+250 GeV/c, #sqrt{s_{ep}}=140 GeV", "");
  leg->AddEntry(h, Form("Average TPC cluster / event = %.1f", meanhit), "l");
  leg->Draw();

  //
  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetRightMargin(.05);

  T->Draw("Sum$(G4HIT_EGEM_1.edep>1e-7)+Sum$(G4HIT_FGEM_1.edep>1e-7)+Sum$(G4HIT_FGEM_2.edep>1e-7)+Sum$(G4HIT_FGEM_3.edep>1e-7)+Sum$(G4HIT_FGEM_4.edep>1e-7)>>hGEMHit(1000,-.5,999.5)");

  TH1 *h = (TH1 *) gDirectory->Get("hGEMHit");
  h->SetTitle(";Total GEM hit per event (E>0.1 keV);Count [A.U.]");
  h->SetMaximum(h->GetMaximum() * 10);
  h->GetXaxis()->SetRangeUser(0, 400);
  double meanhit = h->GetMean();

  TLegend *leg = new TLegend(.1, .80, .95, .93);
  //  leg->AddEntry("", "#it{#bf{EIC-sPHENIX}} Simualtion", "");
  //  leg->AddEntry("", "e+p, 20+250 GeV/c, #sqrt{s_{ep}}=140 GeV", "");
  leg->AddEntry(h, Form("Average GEM hit / event = %.1f", meanhit), "l");
  leg->Draw();

  SaveCanvas(c1,
             TString(_file0->GetName()) + TString("_Draw_EICRate_") + TString(c1->GetName()), true);
}

void CentralCalorimeterRate()
{
  TText *t;
  TCanvas *c1 = new TCanvas("CentralCalorimeterRate_Energy",
                            "CentralCalorimeterRate_Energy", 1900, 425);

  c1->Divide(3, 1, 0, 0);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetLogy();
  T->Draw("TOWER_SIM_CEMC.energy/0.026>>hEMCalADC(210,0,20)");
  TH1 *h = (TH1 *) gDirectory->Get("hEMCalADC");
  assert(h);
  h->SetTitle(";Central EMCal Tower Energy [GeV];Count [A.U.]");
  h->SetMaximum(h->GetMaximum() * 10);

  TLegend *leg = new TLegend(.2, .70, .95, .93);
  leg->AddEntry("", "#it{#bf{EIC-sPHENIX}} Simualtion", "");
  leg->AddEntry("", "e+p, 20+250 GeV/c, #sqrt{s_{ep}}=140 GeV", "");
  leg->AddEntry(h, Form("Central EMCal Energy/Tower"), "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetLogy();
  T->Draw("TOWER_SIM_HCALIN.energy/ 0.162166>>hHCalInADC(210,0,20)");
  TH1 *h = (TH1 *) gDirectory->Get("hHCalInADC");
  assert(h);
  h->SetTitle(";Central Inner HCal Tower Energy [GeV];Count [A.U.]");
  h->SetMaximum(h->GetMaximum() * 10);

  TLegend *leg = new TLegend(.1, .80, .95, .93);
  //  leg->AddEntry("", "#it{#bf{EIC-sPHENIX}} Simualtion", "");
  //  leg->AddEntry("", "e+p, 20+250 GeV/c, #sqrt{s_{ep}}=140 GeV", "");
  leg->AddEntry(h, Form("Central Inner HCal Energy/Tower"), "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetRightMargin(.05);

  p->SetLogy();
  T->Draw("TOWER_SIM_HCALOUT.energy/3.38021e-02>>hHCalOutADC(210,0,20)");
  TH1 *h = (TH1 *) gDirectory->Get("hHCalOutADC");
  assert(h);
  h->SetTitle(";Central Outer HCal Tower Energy [GeV];Count [A.U.]");
  h->SetMaximum(h->GetMaximum() * 10);

  TLegend *leg = new TLegend(.1, .80, .95, .93);
  //  leg->AddEntry("", "#it{#bf{EIC-sPHENIX}} Simualtion", "");
  //  leg->AddEntry("", "e+p, 20+250 GeV/c, #sqrt{s_{ep}}=140 GeV", "");
  leg->AddEntry(h, Form("Central Outer HCal Energy/Tower"), "l");
  leg->Draw();

  SaveCanvas(c1,
             TString(_file0->GetName()) + TString("_Draw_EICRate_") + TString(c1->GetName()), true);

  c1 = new TCanvas("CentralCalorimeterRate_Multiplicity",
                   "CentralCalorimeterRate_Multiplicity", 1900, 425);

  c1->Divide(3, 1, 0, 0);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("Sum$(TOWER_SIM_CEMC.energy/0.026>0.03)>>hCEMCHit(200,-.5,199.5)");
  TH1 *h = (TH1 *) gDirectory->Get("hCEMCHit");
  assert(h);
  h->SetTitle(";# of CEMC tower per event (E>30 MeV);Count [A.U.]");
  h->SetMaximum(h->GetMaximum() * 10);
  h->GetXaxis()->SetRangeUser(0, 100);
  double meanhit = h->GetMean();

  TLegend *leg = new TLegend(.2, .70, .95, .93);
  leg->AddEntry("", "#it{#bf{EIC-sPHENIX}} Simualtion", "");
  leg->AddEntry("", "e+p, 20+250 GeV/c, #sqrt{s_{ep}}=140 GeV", "");
  leg->AddEntry(h, Form("Average CEMC tower / event = %.1f", meanhit), "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("Sum$(TOWER_SIM_HCALIN.energy/ 0.162166>0.03)>>hHCALINHit(200,-.5,199.5)");
  TH1 *h = (TH1 *) gDirectory->Get("hHCALINHit");
  assert(h);
  h->SetTitle(";# of Inner HCal tower per event (E>30 MeV);Count [A.U.]");
  h->SetMaximum(h->GetMaximum() * 10);
  h->GetXaxis()->SetRangeUser(0, 100);
  double meanhit = h->GetMean();

  TLegend *leg = new TLegend(.1, .80, .95, .93);
  //  leg->AddEntry("", "#it{#bf{EIC-sPHENIX}} Simualtion", "");
  //  leg->AddEntry("", "e+p, 20+250 GeV/c, #sqrt{s_{ep}}=140 GeV", "");
  leg->AddEntry(h, Form("Average Inner HCal tower / event = %.1f", meanhit), "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetRightMargin(.05);

  T->Draw("Sum$(TOWER_SIM_HCALOUT.energy/3.38021e-02>0.03)>>hHCALOUTHit(200,-.5,199.5)");
  TH1 *h = (TH1 *) gDirectory->Get("hHCALOUTHit");
  assert(h);
  h->SetTitle(";# of Outer HCal tower per event (E>30 MeV);Count [A.U.]");
  h->SetMaximum(h->GetMaximum() * 10);
  h->GetXaxis()->SetRangeUser(0, 100);
  double meanhit = h->GetMean();

  TLegend *leg = new TLegend(.1, .80, .95, .93);
  //  leg->AddEntry("", "#it{#bf{EIC-sPHENIX}} Simualtion", "");
  //  leg->AddEntry("", "e+p, 20+250 GeV/c, #sqrt{s_{ep}}=140 GeV", "");
  leg->AddEntry(h, Form("Average Outer HCal tower / event = %.1f", meanhit), "l");
  leg->Draw();

  SaveCanvas(c1,
             TString(_file0->GetName()) + TString("_Draw_EICRate_") + TString(c1->GetName()), true);
}

void ForwardCalorimeterRate()
{
  TText *t;
  TCanvas *c1 = new TCanvas("ForwardCalorimeterRate_Energy",
                            "ForwardCalorimeterRate_Energy", 1900, 425);

  c1->Divide(3, 1, 0, 0);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetLogy();
  T->Draw("TOWER_SIM_EEMC.energy>>hEEMCal(801,0,20)");
  TH1 *h = (TH1 *) gDirectory->Get("hEEMCal");
  assert(h);
  h->SetTitle(";e-going EMCal Tower Energy [GeV];Count [A.U.]");
  h->SetMaximum(h->GetMaximum() * 10);

  TLegend *leg = new TLegend(.2, .70, .95, .93);
  leg->AddEntry("", "#it{#bf{EIC-sPHENIX}} Simualtion", "");
  leg->AddEntry("", "e+p, 20+250 GeV/c, #sqrt{s_{ep}}=140 GeV", "");
  leg->AddEntry(h, Form("e-going EMCal Energy/Tower"), "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetLogy();
  T->Draw("TOWER_SIM_FEMC.energy/0.249>>hFEMCal(801,0,200)");
  TH1 *h = (TH1 *) gDirectory->Get("hFEMCal");
  assert(h);
  h->SetTitle(";h-going EMCal Tower Energy [GeV];Count [A.U.]");
  h->SetMaximum(h->GetMaximum() * 10);

  TLegend *leg = new TLegend(.1, .80, .95, .93);
  //  leg->AddEntry("", "#it{#bf{EIC-sPHENIX}} Simualtion", "");
  //  leg->AddEntry("", "e+p, 20+250 GeV/c, #sqrt{s_{ep}}=140 GeV", "");
  leg->AddEntry(h, Form("h-going EMCal Energy/Tower"), "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetRightMargin(.05);

  p->SetLogy();
  T->Draw("TOWER_SIM_FHCAL.energy/0.03898>>hFHCAL(801,0,200)");
  TH1 *h = (TH1 *) gDirectory->Get("hFHCAL");
  assert(h);
  h->SetTitle(";h-going HCal Tower Energy [GeV];Count [A.U.]");
  h->SetMaximum(h->GetMaximum() * 10);

  TLegend *leg = new TLegend(.1, .80, .95, .93);
  //  leg->AddEntry("", "#it{#bf{EIC-sPHENIX}} Simualtion", "");
  //  leg->AddEntry("", "e+p, 20+250 GeV/c, #sqrt{s_{ep}}=140 GeV", "");
  leg->AddEntry(h, Form("h-going HCal Energy/Tower"), "l");
  leg->Draw();

  SaveCanvas(c1,
             TString(_file0->GetName()) + TString("_Draw_EICRate_") + TString(c1->GetName()), true);

  c1 = new TCanvas("ForwardCalorimeterRate_Multiplicity",
                   "ForwardCalorimeterRate_Multiplicity", 1900, 425);

  c1->Divide(3, 1, 0, 0);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("Sum$(TOWER_SIM_EEMC.energy>0.03)>>hEEMCHit(400,-.5,399.5)");
  TH1 *h = (TH1 *) gDirectory->Get("hEEMCHit");
  assert(h);
  h->SetTitle(";# of e-going EMCal tower per event (E>30 MeV);Count [A.U.]");
  h->SetMaximum(h->GetMaximum() * 10);
  h->GetXaxis()->SetRangeUser(0, 100);
  double meanhit = h->GetMean();

  TLegend *leg = new TLegend(.2, .70, .95, .93);
  leg->AddEntry("", "#it{#bf{EIC-sPHENIX}} Simualtion", "");
  leg->AddEntry("", "e+p, 20+250 GeV/c, #sqrt{s_{ep}}=140 GeV", "");
  leg->AddEntry(h, Form("Average e-going EMCal tower / event = %.1f", meanhit), "l");
  leg->Draw();
  //
  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();

  T->Draw("Sum$(TOWER_SIM_FEMC.energy/0.249>0.03)>>hHEMCHit(800,-.5,799.5)");
  TH1 *h = (TH1 *) gDirectory->Get("hHEMCHit");
  assert(h);
  h->SetTitle(";# of h-going EMCal tower per event (E>30 MeV);Count [A.U.]");
  h->SetMaximum(h->GetMaximum() * 10);
  h->GetXaxis()->SetRangeUser(0, 300);
  double meanhit = h->GetMean();

  TLegend *leg = new TLegend(.1, .80, .95, .93);
  //  leg->AddEntry("", "#it{#bf{EIC-sPHENIX}} Simualtion", "");
  //  leg->AddEntry("", "e+p, 20+250 GeV/c, #sqrt{s_{ep}}=140 GeV", "");
  leg->AddEntry(h, Form("Average h-going EMCal tower / event = %.1f", meanhit), "l");
  leg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetRightMargin(.05);
  p->SetLogy();

  T->Draw("Sum$(TOWER_SIM_FHCAL.energy/0.03898>0.03)>>hHHCalHit(400,-.5,399.5)");
  TH1 *h = (TH1 *) gDirectory->Get("hHHCalHit");
  assert(h);
  h->SetTitle(";# of h-going HCal tower per event (E>30 MeV);Count [A.U.]");
  h->SetMaximum(h->GetMaximum() * 10);
  h->GetXaxis()->SetRangeUser(0, 100);
  double meanhit = h->GetMean();

  TLegend *leg = new TLegend(.1, .80, .95, .93);
  //  leg->AddEntry("", "#it{#bf{EIC-sPHENIX}} Simualtion", "");
  //  leg->AddEntry("", "e+p, 20+250 GeV/c, #sqrt{s_{ep}}=140 GeV", "");
  leg->AddEntry(h, Form("Average h-going EMCal tower / event = %.1f", meanhit), "l");
  leg->Draw();

  SaveCanvas(c1,
             TString(_file0->GetName()) + TString("_Draw_EICRate_") + TString(c1->GetName()), true);
}

void KinematicsChecks()
{
  TText *t;
  TCanvas *c1 = new TCanvas("KinematicsChecks",
                            "KinematicsChecks", 1900, 1100);

  c1->Divide(4, 2);
  int idx = 1;
  TPad *p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  T->Draw("p_Q2>>hQ2_inc(400,0,100)", "PHG4Particle.fpid==11");
  hQ2_inc->SetTitle(";Q2 from truth electron [(GeV/c)^2];Count / bin");

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw("p_eta>>hp_eta(100,-4,4)", "PHG4Particle_pT>0.01");
  hp_eta->SetTitle(";#eta for any primary particle with p_{T}>10 MeV;dN/d#eta/Event");

  hp_eta->Scale(1. / total_event / (8. / 100.));

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw("(G4HIT_SVTX.edep*1e6)>>hSVTXEdep(1000,0,100)", "SVTXHitLength>1");
  hSVTXEdep->SetTitle(";Total TPC hit energy deposition [keV]");

  p->SetLogy();

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw("G4HIT_EGEM_1.edep*1e6>>hGEMEdep(1000,0,100)");

  hGEMEdep->SetTitle(";GEM hit energy deposition [keV]");

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetLogy();
  T->Draw("TOWER_SIM_CEMC.energy/0.026>>hEMCalADC(210,0,20)");
  hEMCalADC->SetTitle(";Central EMCal Tower Energy [GeV]");

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetLogy();
  T->Draw("TOWER_SIM_EEMC.energy>>hEEMCal(801,0,20)");
  hEEMCal->SetTitle(";e-going EMCal Tower Energy [GeV]");

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetLogy();
  T->Draw("TOWER_SIM_FEMC.energy/0.249>>hFEMCal(801,0,200)");
  hFEMCal->SetTitle(";h-going EMCal Tower Energy [GeV]");

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetLogy();
  T->Draw("TOWER_SIM_FHCAL.energy/0.03898>>hFHCAL(801,0,200)");
  hFHCAL->SetTitle(";h-going HCal Tower Energy [GeV]");

  SaveCanvas(c1,
             TString(_file0->GetName()) + TString("_Draw_EICRate_") + TString(c1->GetName()), true);
}
