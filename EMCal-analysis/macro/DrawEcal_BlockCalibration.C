// $Id: $

/*!
 * \file DrawEcal_BlockCalibration.C
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

using namespace std;

void DrawEcal_BlockCalibration(
    const TString infile =
        "/phenix/u/jinhuang/links/sPHENIX_work/prod_analysis/EMCal_photon_calib/EMCalAna.root")
{
  SetsPhenixStyle();
  TVirtualFitter::SetDefaultFitter("Minuit2");

  TFile* f =
      TFile::Open(infile);

  assert(f);
  assert(f->IsOpen());

  TH1* EMCalAna_h_CEMC_BLOCK_ETotal = f->Get("EMCalAna_h_CEMC_BLOCK_ETotal");
  assert(EMCalAna_h_CEMC_BLOCK_ETotal);
  TH1* EMCalAna_h_CEMC_BLOCK_EVis = f->Get("EMCalAna_h_CEMC_BLOCK_EVis");
  assert(EMCalAna_h_CEMC_BLOCK_EVis);

  TH1* hSamplingFraction =
      EMCalAna_h_CEMC_BLOCK_EVis->Clone("hSamplingFraction");

  hSamplingFraction->Divide(EMCalAna_h_CEMC_BLOCK_ETotal);

  hSamplingFraction->SetTitle("CEMC visible sampling fraction, 4 GeV photon, |z|<10 cm; EMCal block ID; Sampling Fraction");
  hSamplingFraction->GetXaxis()->SetRangeUser(-.5, 47.5);
  hSamplingFraction->GetYaxis()->SetRangeUser(0.01, 0.03);
  hSamplingFraction->SetFillColor(kBlue - 3);
  hSamplingFraction->SetFillStyle(1001);

  TCanvas* c1 = new TCanvas("DrawEcal_BlockCalibration", "DrawEcal_BlockCalibration", 900, 600);
  c1->Divide(1, 1);
  int idx = 1;
  TPad* p;

  p = (TPad*) c1->cd(idx++);
  c1->Update();

  hSamplingFraction->Draw();

  TLegend* leg = new TLegend(.0, .7, .95, .93);
  leg->SetFillStyle(0);
  leg->AddEntry("", "#it{#bf{sPHENIX}}  Geant4 Simulation", "");
  leg->AddEntry("", "CEMC visible sampling fraction for 4 GeV photon, |z|<10 cm", "");
  //  leg->AddEntry("", "CEMC visible sampling fraction", "");
  //  leg->AddEntry("", "4 GeV photon, |z|<10 cm", "");
  leg->AddEntry("", "2017 projective SPACAL design", "");
  leg->AddEntry("", "1-D projective blocks for #21-26, rest are 2-D projective blocks", "");
  leg->Draw();

  SaveCanvas(c1, infile + "_" + TString(c1->GetName()), kTRUE);

  //Save result
  gSystem->Load("libg4detectors.so");

  PHParameters* param = new PHParameters("CEMC");

  param->set_string_param("description",
                          Form(
                              "2017 projective SPACAL design, Calibration based on CEMC visible sampling fraction for 4 GeV photon, source file: %s",
                              infile.Data()));

  // additional scale for the calibration constant
  // negative pulse -> positive with -1
  //  param->set_double_param("calib_const_scale", 1 / 0.02);  // overall scale the calibration constant to near 1.0
  const double calib_const_scale = 0.02;  // overall scale the calibration constant to near 1.0

  // use channel by channel stuff
  param->set_int_param("use_chan_calibration", 1);

  for (int eta = 0; eta < 96; ++eta)
  {
    for (int phi = 0; phi < 256; ++phi)
    {
      const int block_eta_id = eta / 2;

      string calib_const_name(Form("calib_const_eta%d_phi%d", eta, phi));
      param->set_double_param(calib_const_name,
                              calib_const_scale / hSamplingFraction->GetBinContent(hSamplingFraction->GetXaxis()->FindBin(block_eta_id)));
    }
  }

  param->WriteToFile("xml", "/phenix/u/jinhuang/links/sPHENIX_work/prod_analysis/EMCal_photon_calib/");
}
