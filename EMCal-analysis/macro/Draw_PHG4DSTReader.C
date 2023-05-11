// $Id: $

/*!
 * \file Draw_PHG4DSTReader.C
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
#include <cassert>
#include "SaveCanvas.C"
#include "SetOKStyle.C"
using namespace std;

TFile * _infile = NULL;
TTree * T = NULL;
TString cuts = "";

void
Draw_PHG4DSTReader( //
//          const TString infile = "G4Hits_sPHENIX_e-_eta0_8GeV.root_DSTReader.root"
//        const TString infile = "G4Hits_sPHENIX_e-_eta0_8GeV.root_Ana.root_DSTReader.root"//
//    const TString infile =
//        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/single_particle/spacal2d/zerofield/G4Hits_sPHENIX_gamma_etaALL_50GeV.root_DSTReader.root" //
//    const TString infile = "G4sPHENIXCells.root_DSTReader.root" //
//    const TString infile = "G4sPHENIXCells_2DSpacal_100e10GeV.root_Ana.root_DSTReader.root"//
//    const TString infile =
//         "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/SPACAL_TestBeam_8GeV_CALICEBirk/Sim1096_eneg_DSTReader.root" //
//    const TString infile =
//        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/SPACAL_TestBeam_8GeV/Sum_muonneg.lst_Process.root_DSTReader.root" //
//    const TString infile = "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/Spacal2D_Leakage/Job_ALL_e-_DSTReader.root"//
//        const TString infile = "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/Spacal2D_InsPHENIX/Sim1393ALL_eneg_DSTReader.root"//
    )
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");
  gSystem->Load("libg4eval.so");

  if (!_infile)
    {
      TString chian_str = infile;
      chian_str.ReplaceAll("ALL", "*");

      TChain * t = new TChain("T");
      const int n = t->Add(chian_str);

      cout << "Loaded " << n << " root files with " << chian_str << endl;
      assert(n>0);

      T = t;

      _infile = new TFile;
      _infile->SetName(infile);
    }

  assert(_infile);

  T->SetAlias("CEMC_Sample",
      "Sum$(G4HIT_CEMC.light_yield)/(Sum$(G4HIT_CEMC.edep) + Sum$(G4HIT_ABSORBER_CEMC.edep))");
  T->SetAlias("HCALIN_Sample",
      "Sum$(G4HIT_HCALIN.light_yield)/(Sum$(G4HIT_HCALIN.edep) + Sum$(G4HIT_ABSORBER_HCALIN.edep))");
  T->SetAlias("HCALOUT_Sample",
      "Sum$(G4HIT_HCALOUT.light_yield)/(Sum$(G4HIT_HCALOUT.edep) + Sum$(G4HIT_ABSORBER_HCALOUT.edep))");
  T->SetAlias("FHCAL_Sample",
      "Sum$(G4HIT_FHCAL.light_yield)/(Sum$(G4HIT_FHCAL.edep) + Sum$(G4HIT_ABSORBER_FHCAL.edep))");

  T->SetAlias("PHG4Particle0_e", "0+(PHG4Particle[0].fe)");
  T->SetAlias("PHG4Particle0_p",
      "1*sqrt(PHG4Particle[0].fpx**2+PHG4Particle[0].fpy**2+PHG4Particle[0].fpz**2)");
  T->SetAlias("PHG4Particle0_eta",
      "0.5*log((PHG4Particle0_p+PHG4Particle[0].fpz)/(PHG4Particle0_p-PHG4Particle[0].fpz))");

  T->SetAlias("PHG4Particle_p",
      "1*sqrt(PHG4Particle.fpx**2+PHG4Particle.fpy**2+PHG4Particle.fpz**2)");
  T->SetAlias("PHG4Particle_eta",
      "0.5*log((PHG4Particle_p+PHG4Particle.fpz)/(PHG4Particle_p-PHG4Particle.fpz))");

  T->SetAlias("Entrace_CEMC_x", "G4HIT_ABSORBER_CEMC[0].get_avg_x()+0");
  T->SetAlias("Entrace_CEMC_y", "G4HIT_ABSORBER_CEMC[0].get_avg_y()+0");
  T->SetAlias("Entrace_CEMC_z", "G4HIT_ABSORBER_CEMC[0].get_avg_z()+0");
  T->SetAlias("Entrace_CEMC_azimuthal",
      "atan2(Entrace_CEMC_y, Entrace_CEMC_x)*95.");
  T->SetAlias("Average_CEMC_z",
      "Sum$(G4HIT_ABSORBER_CEMC.get_avg_z()*G4HIT_ABSORBER_CEMC.edep)/Sum$(G4HIT_ABSORBER_CEMC.edep)");
  T->SetAlias("Average_BH_1_z",
      "Sum$(G4HIT_BH_1.get_avg_z())/Sum$(G4HIT_BH_1.hit_id>-1)");

  T->SetAlias("CEMC_E", "Sum$(G4HIT_CEMC.light_yield)+0");
  T->SetAlias("TOWER_CEMC_E",
      "Sum$(TOWER_CEMC.light_yield * (TOWER_CEMC.light_yield>32))");
  T->SetAlias("ABSORBER_CEMC_E", "Sum$(G4HIT_ABSORBER_CEMC.edep)+0");
  T->SetAlias("Total_CEMC_E",
      "Sum$(G4HIT_ABSORBER_CEMC.edep)+Sum$(G4HIT_CEMC.edep)");
  T->SetAlias("HCALIN_E", "Sum$(G4HIT_HCALIN.light_yield)+0");
  T->SetAlias("HCALOUT_E", "Sum$(G4HIT_HCALOUT.light_yield)+0");
  T->SetAlias("Total_HCALIN_E",
      "Sum$(G4HIT_ABSORBER_HCALIN.edep)+Sum$(G4HIT_HCALIN.edep)");
  T->SetAlias("Total_HCALOUT_E",
      "Sum$(G4HIT_ABSORBER_HCALOUT.edep)+Sum$(G4HIT_HCALOUT.edep)");
  T->SetAlias("BH_1_E", "Sum$(G4HIT_BH_1.edep)+0");
  T->SetAlias("EMCScale", "1*1");

  T->SetAlias("PHG4Particle0_pT",
      "1*sqrt(PHG4Particle[0].fpx**2+PHG4Particle[0].fpy**2)");
  T->SetAlias("PHG4Particle0_theta",
      "1*atan2(PHG4Particle0_pT,PHG4Particle[0].fpz)");
  T->SetAlias("PHG4Particle0_phi",
      "1*atan2(PHG4Particle[0].fpy, PHG4Particle[0].fpx)");

  T->SetAlias("PHG4Particle0_xhx",
      "cos(PHG4Particle0_theta)*cos(PHG4Particle0_phi)");
  T->SetAlias("PHG4Particle0_xhy",
      "cos(PHG4Particle0_theta)*sin(PHG4Particle0_phi)");
  T->SetAlias("PHG4Particle0_xhz", "-sin(PHG4Particle0_theta)*1");
  T->SetAlias("PHG4Particle0_yhx", "1*cos(PHG4Particle0_phi + pi/2)");
  T->SetAlias("PHG4Particle0_yhy", "1*sin(PHG4Particle0_phi + pi/2)");
  T->SetAlias("PHG4Particle0_yhz", "0*1");
  T->SetAlias("PHG4Particle0_zhz", "cos(PHG4Particle0_theta)*1");
  T->SetAlias("PHG4Particle0_zhy",
      "sin(PHG4Particle0_theta)*cos(PHG4Particle0_phi)");
  T->SetAlias("PHG4Particle0_zhz",
      "sin(PHG4Particle0_theta)*sin(PHG4Particle0_phi)");

  T->SetAlias("Field", "0*0");
  TString field_cor = "";
//  T->SetAlias("Field", "1*1");
//  TString field_cor = "_field_cor";
  T->SetAlias("CEMC_yp_cor",
      "(-19.984 + 5.64862 * PHG4Particle0_pT -0.531359* PHG4Particle0_pT * PHG4Particle0_pT) * (-Field) ");
  T->SetAlias("ABSORBER_CEMC_yp_cor", "CEMC_yp_cor + 0");
  T->SetAlias("HCALIN_yp_cor",
      "(-25.6969 + 6.24025 * PHG4Particle0_pT -0.472037 * PHG4Particle0_pT * PHG4Particle0_pT) * (-Field) ");
  T->SetAlias("HCALOUT_yp_cor",
      "(-40.6969 + 6.24025 * PHG4Particle0_pT -0.472037 * PHG4Particle0_pT * PHG4Particle0_pT) * (-Field) ");

  T->SetAlias("CEMC_xp",
      "G4HIT_CEMC.get_avg_x() * PHG4Particle0_xhx + G4HIT_CEMC.get_avg_y() * PHG4Particle0_xhy + G4HIT_CEMC.get_avg_z() * PHG4Particle0_xhz");
  T->SetAlias("CEMC_yp",
      "CEMC_yp_cor + G4HIT_CEMC.get_avg_x() * PHG4Particle0_yhx + G4HIT_CEMC.get_avg_y() * PHG4Particle0_yhy + G4HIT_CEMC.get_avg_z() * PHG4Particle0_yhz");
  T->SetAlias("CEMC_zp",
      "G4HIT_CEMC.get_avg_x() * PHG4Particle0_zhx + G4HIT_CEMC.get_avg_y() * PHG4Particle0_zhy + G4HIT_CEMC.get_avg_z() * PHG4Particle0_zhz");
  T->SetAlias("CEMC_R", "sqrt(CEMC_xp**2 + CEMC_yp**2)");

  T->SetAlias("ABSORBER_CEMC_xp",
      "G4HIT_ABSORBER_CEMC.get_avg_x() * PHG4Particle0_xhx + G4HIT_ABSORBER_CEMC.get_avg_y() * PHG4Particle0_xhy + G4HIT_ABSORBER_CEMC.get_avg_z() * PHG4Particle0_xhz");
  T->SetAlias("ABSORBER_CEMC_yp",
      "ABSORBER_CEMC_yp_cor + G4HIT_ABSORBER_CEMC.get_avg_x() * PHG4Particle0_yhx + G4HIT_ABSORBER_CEMC.get_avg_y() * PHG4Particle0_yhy + G4HIT_ABSORBER_CEMC.get_avg_z() * PHG4Particle0_yhz");
  T->SetAlias("ABSORBER_CEMC_zp",
      "G4HIT_ABSORBER_CEMC.get_avg_x() * PHG4Particle0_zhx + G4HIT_ABSORBER_CEMC.get_avg_y() * PHG4Particle0_zhy + G4HIT_ABSORBER_CEMC.get_avg_z() * PHG4Particle0_zhz");
  T->SetAlias("ABSORBER_CEMC_R",
      "sqrt(ABSORBER_CEMC_xp**2 + ABSORBER_CEMC_yp**2)");

  T->SetAlias("HCALIN_xp",
      "G4HIT_HCALIN.get_avg_x() * PHG4Particle0_xhx + G4HIT_HCALIN.get_avg_y() * PHG4Particle0_xhy + G4HIT_HCALIN.get_avg_z() * PHG4Particle0_xhz");
  T->SetAlias("HCALIN_yp",
      "HCALIN_yp_cor + G4HIT_HCALIN.get_avg_x() * PHG4Particle0_yhx + G4HIT_HCALIN.get_avg_y() * PHG4Particle0_yhy + G4HIT_HCALIN.get_avg_z() * PHG4Particle0_yhz");
  T->SetAlias("HCALIN_zp",
      "G4HIT_HCALIN.get_avg_x() * PHG4Particle0_zhx + G4HIT_HCALIN.get_avg_y() * PHG4Particle0_zhy + G4HIT_HCALIN.get_avg_z() * PHG4Particle0_zhz");
  T->SetAlias("HCALIN_R", "sqrt(HCALIN_xp**2 + HCALIN_yp**2)");

  T->SetAlias("HCALOUT_xp",
      "G4HIT_HCALOUT.get_avg_x() * PHG4Particle0_xhx + G4HIT_HCALOUT.get_avg_y() * PHG4Particle0_xhy + G4HIT_HCALOUT.get_avg_z() * PHG4Particle0_xhz");
  T->SetAlias("HCALOUT_yp",
      "HCALOUT_yp_cor + G4HIT_HCALOUT.get_avg_x() * PHG4Particle0_yhx + G4HIT_HCALOUT.get_avg_y() * PHG4Particle0_yhy + G4HIT_HCALOUT.get_avg_z() * PHG4Particle0_yhz");
  T->SetAlias("HCALOUT_zp",
      "G4HIT_HCALOUT.get_avg_x() * PHG4Particle0_zhx + G4HIT_HCALOUT.get_avg_y() * PHG4Particle0_zhy + G4HIT_HCALOUT.get_avg_z() * PHG4Particle0_zhz");
  T->SetAlias("HCALOUT_R", "sqrt(HCALOUT_xp**2 + HCALOUT_yp**2)");

  T->SetAlias("TOWER_CEMC_eta_mean",
      "Sum$(TOWER_CEMC.bineta*TOWER_CEMC.get_energy())/Sum$(TOWER_CEMC.get_energy())");
  T->SetAlias("G4HIT_CEMC_eta_mean",
      "Sum$(-log(tan(0.5*atan2(sqrt(G4HIT_CEMC.get_avg_x()*G4HIT_CEMC.get_avg_x() + G4HIT_CEMC.get_avg_y()*G4HIT_CEMC.get_avg_y()) , G4HIT_CEMC.get_avg_z())))  * G4HIT_CEMC.edep)/Sum$(G4HIT_CEMC.edep)");

  T->SetAlias("TOWER_CEMC_phi_mean",
      "Sum$(TOWER_CEMC.binphi*TOWER_CEMC.get_energy())/Sum$(TOWER_CEMC.get_energy())");
  T->SetAlias("G4HIT_CEMC_phi_mean",
      "Sum$(atan2(G4HIT_CEMC.get_avg_y(),G4HIT_CEMC.get_avg_x()) * G4HIT_CEMC.edep)/Sum$(G4HIT_CEMC.edep)");

  const TCut event_sel = "1*1";
  cuts = "_all_event";

  cout << "Build event selection of " << (const char *) event_sel << endl;

  T->Draw(">>EventList", event_sel);
  TEventList * elist = gDirectory->GetObjectChecked("EventList", "TEventList");
  cout << elist->GetN() << " / " << T->GetEntriesFast() << " events selected"
      << endl;

  T->SetEventList(elist);
////
//  DrawDist(infile);
//  DrawLeakage(infile);
//  DrawLeakage_Wide(infile);
//  DrawLeakage_Phi(infile);
  Sampling(infile);
//  DrawTower_Raw_E(infile);

//  DrawCalibratedE(infile, 0.02206);
//  DrawCalibratedE(infile, 0.02206*9.71762904052642762e-01);
//
//    DrawCalibratedE(infile, 0.02206*7.71244e+00/7.86120e+00);

//  DrawCalibratedE_Tower(infile, 500.);
//  DrawCalibratedE_Tower(infile, 500.*9.71762904052642762e-01);
//  DrawCalibratedE_Tower(infile, 500.*7.71244e+00/7.86120e+00);

//  DrawLeakage_LY(infile);
//
//  DrawTowerIDCheck(infile);
}

void
DrawCalibratedE(TString infile, const double SF = 0.021)
{

  TCanvas *c1 = new TCanvas("DrawCalibratedE" + cuts, "DrawCalibratedE" + cuts,
      1800, 900);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw("CEMC_E>>hCEMC_E(1000,0,.5)");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw(Form("CEMC_E/%f>>hCEMC_E_SF(1000,0,15)", SF));

  SaveCanvas(c1,
      TString(_infile->GetName()) + TString("_DrawJet_")
          + TString(c1->GetName()), kFALSE);
}

void
DrawCalibratedE_Tower(TString infile, const double SF = 0.021)
{

  TCanvas *c1 = new TCanvas("DrawCalibratedE" + cuts, "DrawCalibratedE" + cuts,
      1800, 900);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw("TOWER_CEMC_E>>hCEMC_E(1000,0,8000)");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw(Form("TOWER_CEMC_E/%f>>hCEMC_E_SF(1000,0,15)", SF));

  SaveCanvas(c1,
      TString(_infile->GetName()) + TString("_DrawJet_")
          + TString(c1->GetName()), kFALSE);
}

void
DrawTower_Raw_E(TString infile)
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  TVirtualFitter::SetDefaultFitter("Minuit2");
  gSystem->Load("libg4eval.so");

  TCanvas *c1 = new TCanvas("DrawTower_Raw_E" + cuts, "DrawTower_Raw_E" + cuts,
      1100, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  T->Draw("TOWER_RAW_CEMC.energy>>he(100,0,25000)");

  he->SetTitle(";Photo-electron count per tower; A. U.");
  he->SetLineWidth(4);
  he->SetLineColor(kBlue + 1);

  SaveCanvas(c1,
      TString(_infile->GetName()) + "_Draw_PHG4DSTReader_"
          + TString(c1->GetName()), true);
}

void
DrawCalibratedE_PlotTestBeam(
    const TString base_name =
        "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/SPACAL_TestBeam_Data")
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  TVirtualFitter::SetDefaultFitter("Minuit2");
  gSystem->Load("libg4eval.so");

  const TString config = "nocut";

  const int N = 8;
  const double beam_res = 2.7e-2;

  const double beam_E[8] =
    { 0.995, 2.026, 3.0, 4.12, 6.0, 8.0, 10.0, 12.0 };
  double res[100] =
    { 0 };
  double res_sub[100] =
    { 0 };
  double res_err[100] =
    { 0 };

  TCanvas *c1 = new TCanvas(config, config);
  c1->Print(base_name + "/input_" + config + ".pdf[");

  for (int i = 0; i < N; i++)
    {
      TFile *f = new TFile(
          base_name + Form("/%.0fGeV_", beam_E[i]) + config + ".root");
      assert(f);

      TH1D * ECalSumElectron = (TH1D *) f->GetObjectChecked("ECalSumElectron",
          "TH1D");
      TH1D * ECalSumNonElectron = (TH1D *) f->GetObjectChecked(
          "ECalSumNonElectron", "TH1D");
      assert(ECalSumElectron);
      assert(ECalSumNonElectron);
//      ECalSumElectron->SetStats(false);
//      ECalSumNonElectron->SetStats(false);
      TF1 * fgaus =
      ECalSumElectron->GetListOfFunctions()->At(0);
      assert(fgaus);

      ECalSumElectron->Draw();
      c1->Print(base_name + "/input_" + config + ".pdf");

      res[i] = fgaus->GetParameter(2)/fgaus->GetParameter(1);
      res_sub[i] = sqrt(res[i]*res[i] - beam_res*beam_res);
      res_err[i] = fgaus->GetParError(1)/fgaus->GetParameter(1);
    }
  c1->Print(base_name + "/input_" + config + ".pdf]");

  TGraphErrors * gamma_eta0 = new TGraphErrors(N, beam_E, res_sub, 0, res_err);
  gamma_eta0->Print();
  TGraphErrors * gamma_eta9 = new TGraphErrors(N, beam_E, res, 0, res_err);
  gamma_eta0->Print();

  TCanvas *c1 = new TCanvas("DrawCalibratedE_PlotTestBeam", "DrawCalibratedE_PlotTestBeam",
      1100, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  p->SetGridx(0);
  p->SetGridy(0);


  p->DrawFrame(0, 0, 14, 20e-2,
      ";Beam Energy (GeV);Relative energy resolution, #DeltaE/E");

  TLegend * lg = new TLegend(2.*14./35., 9.6e-2, 17.*14./35., 19.6e-2, NULL, "br");
  TLegend * lg2 = new TLegend(4, 9e-2, 33.*14./35., 19e-2, NULL, "br");

  TF1 * f_calo = new TF1("f_calo_gamma_eta0", "sqrt([0]*[0]+[1]*[1]/x)/100",
      0.5, 60);
  TF1 * f_calo_l = new TF1("f_calo_l_gamma_eta0", "([0]+[1]/sqrt(x))/100", 0.5,
      60);
  gamma_eta0->Fit(f_calo, "RM0");
  f_calo->Print();
  gamma_eta0->Fit(f_calo_l, "RM0");
  f_calo_l->Print();

  TF1 * f_caloR = new TF1("f_calo_gamma_eta0R", "sqrt([0]*[0]+[1]*[1]/x)/100",
      1.5, 60);
  TF1 * f_calo_lR = new TF1("f_calo_l_gamma_eta0R", "([0]+[1]/sqrt(x))/100", 1.5,
      60);
  gamma_eta0->Fit(f_caloR, "RM0");
  f_caloR->Print();
  gamma_eta0->Fit(f_calo_lR, "RM0");
  f_calo_lR->Print();


  gamma_eta0->SetLineColor(kBlack);
  gamma_eta0->SetMarkerColor(kBlack);
  gamma_eta0->SetLineWidth(2);
  gamma_eta0->SetMarkerStyle(kFullSquare);
  gamma_eta0->SetMarkerSize(2);

  gamma_eta9->SetLineColor(kBlack);
  gamma_eta9->SetMarkerColor(kBlack);
  gamma_eta9->SetLineWidth(2);
  gamma_eta9->SetMarkerStyle(kOpenSquare);
  gamma_eta9->SetMarkerSize(2);

  f_calo->SetLineColor(kRed + 1);
  f_calo->SetLineWidth(2);
  f_calo_l->SetLineColor(kRed + 1);
  f_calo_l->SetLineWidth(2);
  f_calo_l->SetLineStyle(kDashed);

  f_caloR->SetLineColor(kBlue + 3);
  f_caloR->SetLineWidth(2);
  f_calo_lR->SetLineColor(kBlue + 3);
  f_calo_lR->SetLineWidth(2);
  f_calo_lR->SetLineStyle(kDashed);

  f_calo->Draw("same");
  f_calo_l->Draw("same");
  f_caloR->Draw("same");
  f_calo_lR->Draw("same");
  gamma_eta0->Draw("p");
  gamma_eta9->Draw("p");

  lg2->AddEntry(gamma_eta9,
      Form("Electrons Data, #eta = 0.3-0.4", abs(f_calo->GetParameter(0)),
          f_calo->GetParameter(1)), "p");
  lg2->AddEntry(gamma_eta0,
      Form("Electrons Data - 2.7%% Beam #DeltaE", f_calo->GetParameter(0),
          f_calo->GetParameter(1)), "p");
  lg2->AddEntry(f_calo,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}",abs( f_calo->GetParameter(0)),
          f_calo->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_l,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}", abs(f_calo_l->GetParameter(0)),
          f_calo_l->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);
  lg2->AddEntry(f_caloR,
      Form("#DeltaE/E = %.1f%% #oplus %.1f%%/#sqrt{E}, E #geq 2 GeV", abs(f_caloR->GetParameter(0)),
          f_caloR->GetParameter(1)), "l");
  TLegendEntry * entry = lg2->AddEntry(f_calo_lR,
      Form("#DeltaE/E = %.1f%%  + %.1f%%/#sqrt{E}, E #geq 2 GeV",abs( f_calo_lR->GetParameter(0)),
          f_calo_lR->GetParameter(1)), "l");
  entry->SetTextColor(kGray + 1);

//  lg->Draw();
  lg2->Draw();

  SaveCanvas(c1, base_name + "/" + TString(c1->GetName()), true);
}

void
//DrawCalibratedE_Plot(TString ID = "4.12GeV", TString config = "")
//DrawCalibratedE_Plot(TString ID = "8GeV", TString config = "")
DrawCalibratedE_Plot(TString ID = "12GeV",TString config = "")
//DrawCalibratedE_Plot(TString ID = "8GeV",TString config = "_Range1um")
//DrawCalibratedE_Plot(TString ID = "8GeV",TString config = "_CALICEBirk")
//DrawCalibratedE_Plot(TString ID = "4GeV",TString config = "")
//DrawCalibratedE_Plot(TString ID = "4GeV",TString config = "_Range1um")
//DrawCalibratedE_Plot(TString ID = "4GeV",TString config = "_CALICEBirk")
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  TVirtualFitter::SetDefaultFitter("Minuit2");
  gSystem->Load("libg4eval.so");

//  /direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/SPACAL_TestBeam_8GeV//SimALL_gamma_DSTReader.root_DrawJet_DrawCalibratedE_all_event.root

//  TFile *f = new TFile("/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/SPACAL_TestBeam_"+ID+config+"//SimALL_eneg_DSTReader.root_DrawJet_DrawCalibratedE_all_event.root");
//  assert(f);
//  TH1F * h_eneg = (TH1F *)f->GetObjectChecked("hCEMC_E_SF","TH1F");
//  assert(h_eneg);
//  TFile *f = new TFile("/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/SPACAL_TestBeam_"+ID+config+"//SimALL_pineg_DSTReader.root_DrawJet_DrawCalibratedE_all_event.root");
//  assert(f);
//  TH1F * h_pineg = (TH1F *)f->GetObjectChecked("hCEMC_E_SF","TH1F");
//  assert(h_pineg);
//  TFile *f = new TFile("/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/SPACAL_TestBeam_"+ID+config+"//SimALL_kaonneg_DSTReader.root_DrawJet_DrawCalibratedE_all_event.root");
//  assert(f);
//  TH1F * h_kaonneg = (TH1F *)f->GetObjectChecked("hCEMC_E_SF","TH1F");
//  assert(h_kaonneg);

  TFile *f =
      new TFile(
          "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/SPACAL_TestBeam_"
              + ID + config
              + "//Sum_eneg.lst_Process.root_DSTReader.root_DrawJet_DrawCalibratedE_all_event.root");
  assert(f);
  TH1F * h_eneg = (TH1F *) f->GetObjectChecked("hCEMC_E_SF", "TH1F");
  assert(h_eneg);
  TFile *f =
      new TFile(
          "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/SPACAL_TestBeam_"
              + ID + config
              + "//Sum_pineg.lst_Process.root_DSTReader.root_DrawJet_DrawCalibratedE_all_event.root");
  assert(f);
  TH1F * h_pineg = (TH1F *) f->GetObjectChecked("hCEMC_E_SF", "TH1F");
  assert(h_pineg);
  TFile *f =
      new TFile(
          "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/SPACAL_TestBeam_"
              + ID + config
              + "//Sum_kaonneg.lst_Process.root_DSTReader.root_DrawJet_DrawCalibratedE_all_event.root");
  assert(f);
  TH1F * h_kaonneg = (TH1F *) f->GetObjectChecked("hCEMC_E_SF", "TH1F");
  assert(h_kaonneg);
  TFile *f =
      new TFile(
          "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/SPACAL_TestBeam_"
              + ID + config
              + "//Sum_muonneg.lst_Process.root_DSTReader.root_DrawJet_DrawCalibratedE_all_event.root");
  assert(f);
  TH1F * h_muonneg = (TH1F *) f->GetObjectChecked("hCEMC_E_SF", "TH1F");
  assert(h_muonneg);

  TH1F * h_eneg_scale_hadron_data_tail = (TH1F *) h_eneg->Clone(
      "h_eneg_scale_hadron_data_tail");

  TFile *f =
      new TFile(
          "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/SPACAL_TestBeam_8GeV/beam_data/"
              + ID + ".root");
  assert(f);

  TH1D * ECalSumElectron = (TH1D *) f->GetObjectChecked("ECalSumElectron",
      "TH1D");
  TH1D * ECalSumNonElectron = (TH1D *) f->GetObjectChecked("ECalSumNonElectron",
      "TH1D");
  assert(ECalSumElectron);
  assert(ECalSumNonElectron);
  ECalSumElectron->SetStats(false);
  ECalSumNonElectron->SetStats(false);
  ECalSumElectron->GetListOfFunctions()->RemoveAt(0);

  const double sim_E = 7.86131e+00;
  const double data_E = 1.09505e+03;

  ECalSumElectron->GetXaxis()->Set(ECalSumElectron->GetXaxis()->GetNbins(),
      ECalSumElectron->GetXaxis()->GetXmin() * sim_E / data_E,
      ECalSumElectron->GetXaxis()->GetXmax() * sim_E / data_E //
          );
  ECalSumNonElectron->GetXaxis()->Set(
      ECalSumNonElectron->GetXaxis()->GetNbins(),
      ECalSumNonElectron->GetXaxis()->GetXmin() * sim_E / data_E,
      ECalSumNonElectron->GetXaxis()->GetXmax() * sim_E / data_E //
          );

  const double energy = h_eneg->GetMean();

  double eneg_scale = ECalSumElectron->Integral(
      ECalSumElectron->GetXaxis()->FindBin(energy * .7),
      ECalSumElectron->GetXaxis()->FindBin(energy * 1.3))
      / h_eneg->Integral(h_eneg->GetXaxis()->FindBin(energy * .7),
          h_eneg->GetXaxis()->FindBin(energy * 1.3));

  const double hadron_norm_max_range = 0.7;

  double muonneg_scale = 0.1 * //10% muon in hadron sample
      ECalSumNonElectron->Integral(ECalSumNonElectron->GetXaxis()->FindBin(.01),
          ECalSumNonElectron->GetXaxis()->FindBin(energy * 1.3))
      / h_muonneg->Integral(h_muonneg->GetXaxis()->FindBin(.01),
          h_muonneg->GetXaxis()->FindBin(energy * 1.3));

  const double muon_ratio_in_MIP = h_muonneg->Integral(
      h_muonneg->GetXaxis()->FindBin(.01),
      h_muonneg->GetXaxis()->FindBin(hadron_norm_max_range))
      / h_muonneg->Integral(h_muonneg->GetXaxis()->FindBin(.01),
          h_muonneg->GetXaxis()->FindBin(energy * 1.3));
  const double muon_count_in_MIP = muon_ratio_in_MIP //
  * 0.1 * //10% muon in hadron sample
      ECalSumNonElectron->Integral(ECalSumNonElectron->GetXaxis()->FindBin(.01),
          ECalSumNonElectron->GetXaxis()->FindBin(energy * 1.3));

  cout << "DrawCalibratedE_Plot: muon_ratio_in_MIP = " << muon_ratio_in_MIP
      << ", muon_count_in_MIP = " << muon_count_in_MIP << endl;

  double pineg_scale = (ECalSumNonElectron->Integral(
      ECalSumNonElectron->GetXaxis()->FindBin(.01),
      ECalSumNonElectron->GetXaxis()->FindBin(hadron_norm_max_range))
      - muon_count_in_MIP)
      / h_pineg->Integral(h_pineg->GetXaxis()->FindBin(.01),
          h_pineg->GetXaxis()->FindBin(hadron_norm_max_range));

  double kaonneg_scale = (ECalSumNonElectron->Integral(
      ECalSumNonElectron->GetXaxis()->FindBin(.01),
      ECalSumNonElectron->GetXaxis()->FindBin(hadron_norm_max_range))
      - muon_count_in_MIP)
      / h_kaonneg->Integral(h_kaonneg->GetXaxis()->FindBin(.01),
          h_kaonneg->GetXaxis()->FindBin(hadron_norm_max_range));

  double eneg_scale_hadron_data_tail = ECalSumNonElectron->Integral(
      ECalSumNonElectron->GetXaxis()->FindBin(energy * 0.95),
      ECalSumNonElectron->GetXaxis()->FindBin(energy * 1.15))
      / h_eneg_scale_hadron_data_tail->Integral(
          h_eneg_scale_hadron_data_tail->GetXaxis()->FindBin(energy * 0.95),
          h_eneg_scale_hadron_data_tail->GetXaxis()->FindBin(energy * 1.15));

  h_eneg->Rebin(5);
  h_pineg->Rebin(5);
  h_kaonneg->Rebin(5);
  h_muonneg->Rebin(5);
  h_eneg_scale_hadron_data_tail->Rebin(5);

//  ECalSumElectron->Rebin();
  ECalSumNonElectron->Rebin(40);

  ECalSumElectron->Sumw2();
  ECalSumNonElectron->Sumw2();

  ECalSumElectron->SetMarkerStyle(kFullCircle);
  ECalSumElectron->SetMarkerSize(1);
  ECalSumNonElectron->SetMarkerStyle(kFullCircle);
  ECalSumNonElectron->SetMarkerSize(1);

  eneg_scale *= ECalSumElectron->GetBinWidth(1) / h_eneg->GetBinWidth(1);
  pineg_scale *= ECalSumNonElectron->GetBinWidth(1) / h_pineg->GetBinWidth(1);
  kaonneg_scale *= ECalSumNonElectron->GetBinWidth(1)
      / h_kaonneg->GetBinWidth(1);
  muonneg_scale *= ECalSumNonElectron->GetBinWidth(1)
      / h_muonneg->GetBinWidth(1);
  eneg_scale_hadron_data_tail *= ECalSumNonElectron->GetBinWidth(1)
      / h_eneg_scale_hadron_data_tail->GetBinWidth(1);

  //Resolution for electron sample

  TF1 * f_gaus_sim = new TF1("f_gaus_sim", "gaus",
      h_eneg->GetMean() - 2 * h_eneg->GetRMS(),
      h_eneg->GetMean() + 2 * h_eneg->GetRMS());
  f_gaus_sim->SetParameters(1, h_eneg->GetMean(), h_eneg->GetRMS());
  new TCanvas;
  h_eneg->Fit(f_gaus_sim, "RM0");
  h_eneg->DrawClone();
  f_gaus_sim->Draw("same");

  TF1 * f_gaus_data = new TF1("f_gaus_data", "gaus",
      h_eneg->GetMean() - 2 * h_eneg->GetRMS(),
      h_eneg->GetMean() + 2 * h_eneg->GetRMS());
  f_gaus_data->SetParameters(1, h_eneg->GetMean(), h_eneg->GetRMS());
  new TCanvas;
  ECalSumElectron->Fit(f_gaus_data, "RM0");
  ECalSumElectron->DrawClone();
  f_gaus_data->Draw("same");

  h_eneg->Scale(eneg_scale);
  h_pineg->Scale(pineg_scale);
  h_kaonneg->Scale(kaonneg_scale);
  h_muonneg->Scale(muonneg_scale);
  h_eneg_scale_hadron_data_tail->Scale(eneg_scale_hadron_data_tail);

//  h_pineg -> Add(h_eneg_scale_hadron_data_tail);
//  h_kaonneg -> Add(h_eneg_scale_hadron_data_tail);

  TCanvas *c1 = new TCanvas("DrawCalibratedE_Plot" + cuts,
      "DrawCalibratedE_Plot" + cuts, 1800, 700);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  h_eneg->SetLineWidth(2);

  ECalSumElectron->GetXaxis()->SetRangeUser(0, energy * 1.5);
  ECalSumElectron->Draw();
  h_eneg->SetLineWidth(3);
  h_eneg->SetLineColor(kGreen + 3);
  h_eneg->SetFillColor(kGreen - 7);
  h_eneg->SetFillStyle(1001);
  h_eneg->Draw("same");
  ECalSumElectron->Draw("same");

  ECalSumElectron->SetTitle(";Calibrated Energy (GeV);Count/bin");

  TLegend * lg = new TLegend(0.13, 0.65, 0.55, 0.85);
  lg->AddEntry(ECalSumElectron,
      Form("#splitline{2014 eRD1 beam test (e-cut)}{#DeltaE/E = %.1f%%}",
          f_gaus_data->GetParameter(2) / f_gaus_data->GetParameter(1) * 100),
      "pe");
  lg->AddEntry(h_eneg,
      Form("#splitline{e^{-} sim using sPHENIX Geant4,}{#DeltaE/E = %.1f%%}",
          f_gaus_sim->GetParameter(2) / f_gaus_sim->GetParameter(1) * 100),
      "lf");
  lg->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  p->SetLogy();
  p->SetGridx(0);
  p->SetGridy(0);

  h_pineg->SetLineColor(kRed);
  h_pineg->SetLineWidth(3);
  h_kaonneg->SetLineColor(kBlue);
  h_kaonneg->SetLineWidth(3);
  h_muonneg->SetLineColor(kBlack);
  h_muonneg->SetFillColor(kGray);
  h_muonneg->SetLineWidth(3);
  h_muonneg->SetFillStyle(1001);
  h_eneg_scale_hadron_data_tail->SetLineColor(kGreen + 3);
  h_eneg_scale_hadron_data_tail->SetLineWidth(2);
  h_eneg_scale_hadron_data_tail->SetFillColor(kGreen - 7);
  h_eneg_scale_hadron_data_tail->SetFillStyle(1001);

  ECalSumNonElectron->GetXaxis()->SetRangeUser(0, energy * 1.5);

  ECalSumNonElectron->Draw();
  h_muonneg->Draw("same");
//  h_eneg_scale_hadron_data_tail->Draw("same");  ////////////// Guess of electron contamination
  h_pineg->Draw("same");
  h_kaonneg->Draw("same");
  ECalSumNonElectron->Draw("same");

  ECalSumNonElectron->SetTitle(";Calibrated Energy (GeV);Count/bin");

  TLegend * lg = new TLegend(0.35, 0.65, 0.85, 0.9);
  lg->AddEntry(ECalSumNonElectron, Form("2014 eRD1 beam test (#bar{e-cut})"),
      "pe");
  lg->AddEntry(h_pineg, Form("#pi^{-} sim using sPHENIX Geant4"), "l");
  lg->AddEntry(h_kaonneg, Form("K^{-} sim using sPHENIX Geant4"), "l");
//  lg->AddEntry(h_eneg_scale_hadron_data_tail, Form("e^{-} sim using sPHENIX Geant4"),////////////// Guess of electron contamination
//      "lf");////////////// Guess of electron contamination
  lg->AddEntry(h_muonneg, Form("#mu^{-} (~10%% data) sim using sPHENIX Geant4"), "lf");
  lg->Draw();

  SaveCanvas(c1,
      TString(
          "/direct/phenix+sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/SPACAL_TestBeam_"
              + ID + config + "/DrawCalibratedE_Plot_DrawJet_")
          + TString(c1->GetName()), true);
}

void
DrawTowerIDCheck(TString infile)
{

  TCanvas *c1 = new TCanvas("DrawTowerIDCheck" + cuts,
      "DrawTowerIDCheck" + cuts, 1800, 900);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw("TOWER_CEMC_eta_mean:G4HIT_CEMC_eta_mean", "", "*");

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw("TOWER_CEMC_phi_mean:G4HIT_CEMC_phi_mean", "", "*");

  SaveCanvas(c1,
      TString(_infile->GetName()) + TString("_DrawJet_")
          + TString(c1->GetName()), kFALSE);
}

void
DrawDist(TString infile)
{

  TCanvas *c1 = new TCanvas("DrawDist" + cuts, "DrawDist" + cuts, 1800, 900);
  c1->Divide(2, 1);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw(
      "G4HIT_CEMC.get_avg_y():G4HIT_CEMC.get_avg_x()>>h_EMC_Azim(600,-300,300,600,-300,300)",
      "G4HIT_CEMC.edep", "goff");
//  T->Draw(
//      "G4HIT_HCALIN.get_avg_y():G4HIT_HCALIN.get_avg_x()>>h_HCALIN_Azim(600,-300,300,600,-300,300)",
//      "G4HIT_HCALIN.edep", "goff");
//  T->Draw(
//      "G4HIT_HCALOUT.get_avg_y():G4HIT_HCALOUT.get_avg_x()>>h_HCALOUT_Azim(600,-300,300,600,-300,300)",
//      "G4HIT_HCALOUT.edep", "goff");
//  TGraph * g_EMC =
//      (TGraph *) gPad->GetListOfPrimitives()->FindObject("Graph")->Clone(
//          "g_EMC");
//  assert(g_EMC);
//  g_EMC->SetName("g_EMC");
//  g_EMC->SetMarkerColor(kRed);
//  g_EMC->SetMarkerStyle(kDot);

  p->DrawFrame(-300, -300, 300, 300,
      "Azimuthal distribution of hits;X (cm);Y (cm)");
  h_EMC_Azim->Draw("colz same");
//  h_HCALIN_Azim->Draw("colz same");
//  h_HCALOUT_Azim->Draw("colz same");

  TEllipse * cBaBar = new TEllipse(0, 0, 140);

  cBaBar->SetFillStyle(0);
  cBaBar->SetLineWidth(3);
  cBaBar->SetLineColor(kMagenta);
  cBaBar->Draw();
  TEllipse * cBaBar = new TEllipse(0, 0, 170);

  cBaBar->SetFillStyle(0);
  cBaBar->SetLineWidth(3);
  cBaBar->SetLineColor(kMagenta);
  cBaBar->Draw();

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw(
      "sqrt(G4HIT_CEMC.get_avg_x()**2 + G4HIT_CEMC.get_avg_y()**2):G4HIT_CEMC.get_avg_z()>>h_CEMC_Rz(600,-300,300,300,0,300)",
      "G4HIT_CEMC.edep", "goff");
//  T->Draw(
//      "sqrt(G4HIT_HCALIN.get_avg_x()**2 + G4HIT_HCALIN.get_avg_y()**2):G4HIT_HCALIN.get_avg_z()>>h_HCALIN_Rz(600,-300,300,300,0,300)",
//      "G4HIT_HCALIN.edep", "goff");
//  T->Draw(
//      "sqrt(G4HIT_HCALOUT.get_avg_x()**2 + G4HIT_HCALOUT.get_avg_y()**2):G4HIT_HCALOUT.get_avg_z()>>h_HCALOUT_Rz(600,-300,300,300,0,300)",
//      "G4HIT_HCALOUT.edep", "goff");

//  TGraph * g_EMC =
//      (TGraph *) gPad->GetListOfPrimitives()->FindObject("Graph")->Clone(
//          "g_EMC");
//  assert(g_EMC);
//  g_EMC->SetName("g_EMC");
//  g_EMC->SetMarkerColor(kRed);
//  g_EMC->SetMarkerStyle(kDot);

  p->DrawFrame(-300, 00, 300, 300,
      "Azimuthal distribution of hits;Z (cm);R (cm)");
  h_CEMC_Rz->Draw("colz same");
//  h_HCALIN_Rz->Draw("colz same");
//  h_HCALOUT_Rz->Draw("colz same");

  TLine * lBaBar = new TLine(-174.5, 140, 174.5, 140);

  lBaBar->SetLineWidth(3);
  lBaBar->SetLineColor(kMagenta);
  lBaBar->Draw();
  TLine * lBaBar = new TLine(-174.5, 170, 174.5, 170);

  lBaBar->SetLineWidth(3);
  lBaBar->SetLineColor(kMagenta);
  lBaBar->Draw();

  SaveCanvas(c1,
      TString(_infile->GetName()) + TString("_DrawJet_")
          + TString(c1->GetName()), kFALSE);
}

void
DrawLeakage(TString infile)
{

  TCanvas *c1 = new TCanvas("DrawLeakage" + cuts, "DrawLeakage" + cuts, 1800,
      900);
  c1->Divide(2, 3);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw(
      "Average_CEMC_z:PHG4Particle0_eta>>hAverage_CEMC_z_PHG4Particle0_eta(120,0.8,0.9,120,75,125)",
      "", "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw(
      "Entrace_CEMC_z:PHG4Particle0_eta>>hEntrace_CEMC_z_PHG4Particle0_eta(120,0.8,0.9,120,75,125)",
      "", "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw(
      "Total_HCALIN_E/PHG4Particle0_e:Average_CEMC_z>>hLeakage_Average_CEMC_z(120,75,125,100,0,0.25)",
      "", "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw(
      "Total_HCALIN_E/PHG4Particle0_e:PHG4Particle0_eta>>hLeakage_PHG4Particle0_eta(120,0.8,0.9,100,0,0.25)",
      "", "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw(
      "Total_CEMC_E/PHG4Particle0_e:Average_CEMC_z>>hTotal_CEMC_E_Entrance_Z_Entrace_CEMC_z(120,75,125,100,0,1.2)",
      "", "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw(
      "Total_CEMC_E/PHG4Particle0_e:PHG4Particle0_eta>>hTotal_CEMC_E_PHG4Particle0_eta(120,0.8,0.9,100,0,1.2)",
      "", "colz");

  SaveCanvas(c1,
      TString(_infile->GetName()) + TString("_DrawJet_")
          + TString(c1->GetName()), kFALSE);
}
void
DrawLeakage_LY(TString infile)
{

  TCanvas *c1 = new TCanvas("DrawLeakage_LY" + cuts, "DrawLeakage_LY" + cuts,
      1800, 900);
  c1->Divide(2, 3);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw(
      "Sum$(G4HIT_CEMC.light_yield)/PHG4Particle0_e:Average_CEMC_z>>hTotal_CEMC_E_Entrance_Z_Entrace_CEMC_z(120,75,125,100,0,.05)",
      "", "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw(
      "Sum$(G4HIT_CEMC.light_yield)/PHG4Particle0_e:PHG4Particle0_eta>>hTotal_CEMC_E_PHG4Particle0_eta(120,0.8,0.9,100,0,.05)",
      "", "colz");

  SaveCanvas(c1,
      TString(_infile->GetName()) + TString("_DrawJet_")
          + TString(c1->GetName()), kFALSE);
}

void
DrawLeakage_Wide(TString infile)
{

  TCanvas *c1 = new TCanvas("DrawLeakage_Wide" + cuts,
      "DrawLeakage_Wide" + cuts, 1800, 900);
  c1->Divide(2, 3);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw(
      "Average_CEMC_z:PHG4Particle0_eta>>hAverage_CEMC_z_PHG4Particle0_eta(300,0,1.1,300,0,150)",
      "", "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw(
      "Entrace_CEMC_z:PHG4Particle0_eta>>hEntrace_CEMC_z_PHG4Particle0_eta(300,0,1.1,300,0,150)",
      "", "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw(
      "Total_HCALIN_E/PHG4Particle0_e:Average_CEMC_z>>hLeakage_Average_CEMC_z(300,0,150,100,0,0.25)",
      "", "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw(
      "Total_HCALIN_E/PHG4Particle0_e:PHG4Particle0_eta>>hLeakage_PHG4Particle0_eta(300,0,1.1,100,0,0.25)",
      "", "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw(
      "Total_CEMC_E/PHG4Particle0_e:Average_CEMC_z>>hTotal_CEMC_E_Entrance_Z_Entrace_CEMC_z(300,0,150,100,0,1.2)",
      "", "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw(
      "Total_CEMC_E/PHG4Particle0_e:PHG4Particle0_eta>>hTotal_CEMC_E_PHG4Particle0_eta(300,0,1.1,100,0,1.2)",
      "", "colz");

  SaveCanvas(c1,
      TString(_infile->GetName()) + TString("_DrawJet_")
          + TString(c1->GetName()), kFALSE);
}

void
DrawLeakage_Phi(TString infile)
{

  TCanvas *c1 = new TCanvas("DrawLeakage_Phi" + cuts, "DrawLeakage_Phi" + cuts,
      1800, 900);
  c1->Divide(1, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();

  T->Draw(
      "Total_HCALIN_E/PHG4Particle0_e:PHG4Particle0_phi>>hLeakage_PHG4Particle0_e(320,-3.14159,3.14159,100,0,0.25)",
      "", "colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  T->Draw(
      "Total_CEMC_E/PHG4Particle0_e:PHG4Particle0_phi>>hTotal_CEMC_E_PHG4Particle0_e(320,-3.14159,3.14159,100,0,1.2)",
      "", "colz");

  SaveCanvas(c1,
      TString(_infile->GetName()) + TString("_DrawJet_")
          + TString(c1->GetName()), kFALSE);
}

void
DrawLeakage_Phi_Folding(const int N_fold = 16)
{
  SetOKStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  TVirtualFitter::SetDefaultFitter("Minuit2");
  gSystem->Load("libg4eval.so");

  TH2F * hLeakage_PHG4Particle0_e = (TH2F *) gDirectory->GetObjectChecked(
      "hLeakage_PHG4Particle0_e", "TH2F");
  TH2F * hTotal_CEMC_E_PHG4Particle0_e = (TH2F *) gDirectory->GetObjectChecked(
      "hTotal_CEMC_E_PHG4Particle0_e", "TH2F");

  TH2F * hLeakage_PHG4Particle0_e_Fold = new TH2F(
      "hLeakage_PHG4Particle0_e_Fold", ";Phi (rad);Leakage Ratio", 320 / N_fold,
      -3.14159 / N_fold, 3.14159 / N_fold, 100, 0, 0.25);
  TH2F * hTotal_CEMC_E_PHG4Particle0_e_Fold = new TH2F(
      "hTotal_CEMC_E_PHG4Particle0_e_Fold", ";Phi (rad);EMCal Energy Ratio",
      320 / N_fold, -3.14159 / N_fold, 3.14159 / N_fold, 100, 0, 1.2);

  for (int biny = 1; biny <= hLeakage_PHG4Particle0_e->GetNbinsY(); biny++)
    for (int bin = 1; bin <= hLeakage_PHG4Particle0_e->GetNbinsX(); bin++)
      {
        const int folded_bin = (bin - 1) % (320 / N_fold) + 1;

        hLeakage_PHG4Particle0_e_Fold->SetBinContent(folded_bin, biny,
            hLeakage_PHG4Particle0_e_Fold->GetBinContent(folded_bin, biny)
                + hLeakage_PHG4Particle0_e->GetBinContent(bin, biny));
        hTotal_CEMC_E_PHG4Particle0_e_Fold->SetBinContent(folded_bin, biny,
            hTotal_CEMC_E_PHG4Particle0_e_Fold->GetBinContent(folded_bin, biny)
                + hTotal_CEMC_E_PHG4Particle0_e->GetBinContent(bin, biny));

      }

  TCanvas *c1 = new TCanvas("DrawLeakage_Phi_Folding" + cuts,
      "DrawLeakage_Phi_Folding" + cuts, 1000, 900);
  c1->Divide(1, 2);
  int idx = 1;
  TPad * p;

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  hLeakage_PHG4Particle0_e_Fold->Draw("colz");

  p = (TPad *) c1->cd(idx++);
  c1->Update();
  hTotal_CEMC_E_PHG4Particle0_e_Fold->Draw("colz");

  SaveCanvas(c1, TString("_DrawJet_") + TString(c1->GetName()), kFALSE);
}

void
Sampling(TString infile)
{

  TCanvas *c1 = new TCanvas("Sampling" + cuts, "Sampling" + cuts, 900, 900);
  c1->Divide(1, 1);
  int idx = 1;
  TPad * p = gPad;

  T->Draw(
      "Sum$(G4HIT_CEMC.light_yield):(Sum$(G4HIT_CEMC.edep) + Sum$(G4HIT_ABSORBER_CEMC.edep))",
      "", "*");
  TGraph * gh2D =
      (TGraph *) gPad->GetListOfPrimitives()->FindObject("Graph")->Clone(
          "gh2D");
  assert(gh2D);
  gh2D->SetName("gh2D");
  gh2D->SetMarkerStyle(kDot);

  p->DrawFrame(0, 0, 10, 1,
      "EMC Energy Deposition;Absorber+Scintillator (GeV);Scintillator (GeV)");
  gh2D->Draw("p");

  TF1 * fsampling = new TF1("fsampling", "[0]*x", 0.4, 100);
  gh2D->Fit(fsampling, "MR");

  SaveCanvas(c1,
      TString(_infile->GetName()) + TString("_DrawJet_")
          + TString(c1->GetName()), kFALSE);
}
