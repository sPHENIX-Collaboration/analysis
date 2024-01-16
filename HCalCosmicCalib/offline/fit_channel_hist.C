#include <TH1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TF1.h>
#include "TROOT.h"
#include <iostream>
#include <string>
#include <fstream>
#include <TAxis.h>
#include <TGraph.h>
#include <TSpectrum.h>

double gamma_function(double *x, double *par) {
  double peak = par[0];
  double shift = par[1];
  double scale = par[2];
  double N = par[3];

  double arg_para = (x[0] - shift) / scale;
  double peak_para = (peak - shift) / scale;
  double numerator = N * pow(arg_para, peak_para) * TMath::Exp(-arg_para);
  double denominator = ROOT::Math::tgamma(peak_para + 1) * scale;

  if (denominator != 0) return (double)(numerator / denominator);
  else return 0;
}

float get_temp_shift(TH1F* hist, float y) {
  float shift = 0.;
  for (int i = 1; i <= hist->GetNbinsX(); i++) {
    if (hist->GetBinContent(i) > y/10.) {
      shift = hist->GetBinCenter(i);
      break;
    }
  }
  return shift;
}

float get_temp_scale(TH1F* hist, float x) {
  float scale = (hist->GetMean() - x);
  return scale;
}

float get_temp_N(float x, float y, float shift, float scale) {
  float temp_par0 = (x - shift) / scale;
  float temp_par1 = pow(temp_par0, temp_par0) * TMath::Exp(-temp_par0);
  float temp_par2 = ROOT::Math::tgamma(temp_par0 + 1) * scale * y;
  float temp_N = temp_par2 / temp_par1;
  return temp_N;
}

float get_fit_min(TH1F* hist, float y) {
  float min = 0.;
  for (int i = 1; i <= hist->GetNbinsX(); i++) {
    if (hist->GetBinContent(i) > 2*y/3.) {
      min = hist->GetBinCenter(i);
      if (hist->GetBinContent(i) > 3*y/4.) {
        min = hist->GetBinCenter(i-1);
      }
      break;
    }
  }
  return min;
}

float get_fit_max(TH1F* hist, float y) {
  float max = 0.;
  for (int i = hist->GetNbinsX(); i >= 1; i--) {
    if (hist->GetBinContent(i) > y/5.) {
      max = hist->GetBinCenter(i);
      break;
    }
  }
  return max;
}

void do_peak_finder(TH1F* hist, float& x, float& y, float& shift, float& scale, float& N, float& min, float& max) {
  TH1F *h_peakfinder = (TH1F*)hist->Clone("h_peakfinder");
  h_peakfinder->Smooth(2);
  TSpectrum *s = new TSpectrum(10);
  int nfound = 0;
  nfound = s->Search(h_peakfinder, 2, "nobackground new", 0.6);
  if (nfound == 0) {
    int index = hist->GetMaximumBin();
    x = hist->GetBinCenter(index);
    y = hist->GetBinContent(index);
  } else {
    double *xtemp = s->GetPositionX();
    double *ytemp = s->GetPositionY();
    x = xtemp[0];
    y = ytemp[0];
  }
  shift = get_temp_shift(h_peakfinder, y);
  scale = get_temp_scale(hist, x);
  N = get_temp_N(x, y, shift, scale);
  min = get_fit_min(h_peakfinder, y);
  max = get_fit_max(h_peakfinder, y);
  delete h_peakfinder;
  delete s;
}

void do_peak_fit(TH1F* hist, TF1* func) {
  float peak_position_finder, peak_value_finder, shift_finder, scale_finder, N_finder, min_finder, max_finder;
  do_peak_finder(hist, peak_position_finder, peak_value_finder, shift_finder, scale_finder, N_finder, min_finder, max_finder);
  func->SetParameter(0, peak_position_finder);
  func->SetParameter(1, shift_finder);
  func->SetParameter(2, scale_finder);
  func->SetParameter(3, N_finder);
  hist->Fit(func, "", "", min_finder, max_finder);
}

void fit_channel_hist() {
  // Constant
  static const int n_etabin = 24;
  static const int n_phibin = 64;

  // File
  TFile *ohcal_input = new TFile("../macro/ohcal_hist.root","READ");
  TFile *ihcal_input = new TFile("../macro/ihcal_hist.root","READ");
  std::ofstream total_mpvinfo("total_mpvinfo.txt",ios::trunc);
  TFile *ohcal_output = new TFile("ohcal_fitresult.root","recreate");
  std::ofstream ohcal_mpvinfo("ohcal_mpvinfo.txt",ios::trunc);
  std::ofstream ohcal_calibinfo("ohcal_calibinfo.txt",ios::trunc);
  TFile *ihcal_output = new TFile("ihcal_fitresult.root","recreate");
  std::ofstream ihcal_mpvinfo("ihcal_mpvinfo.txt",ios::trunc);
  std::ofstream ihcal_calibinfo("ihcal_calibinfo.txt",ios::trunc);

  // oHCal setup
  TH2F *h_2Dohcal_hit = new TH2F("h_2Dohcal_hit", "", n_etabin, 0, n_etabin, n_phibin, 0, n_phibin);
  h_2Dohcal_hit->GetXaxis()->SetTitle("ieta");
  h_2Dohcal_hit->GetYaxis()->SetTitle("iphi");
  TH1F *h_ohcal_total;
  h_ohcal_total = new TH1F("h_ohcal_total","",200,0,10000);
  h_ohcal_total->GetXaxis()->SetTitle("ADC");
  h_ohcal_total->GetYaxis()->SetTitle("Counts");
  TH1F *h_ohcal_channel[n_etabin][n_phibin];
  for (int ieta = 0; ieta < n_etabin; ++ieta) {
    for (int iphi = 0; iphi < n_phibin; ++iphi) {
      std::ostringstream h_ohcal_channelname;
      h_ohcal_channelname << "h_ohcal_channel_" << ieta << "_" << iphi;
      h_ohcal_channel[ieta][iphi] = new TH1F(h_ohcal_channelname.str().c_str(), "", 200, 0, 10000);
      h_ohcal_channel[ieta][iphi]->GetXaxis()->SetTitle("ADC");
      h_ohcal_channel[ieta][iphi]->GetYaxis()->SetTitle("Counts");
    }
  }
  TH1F *h_ohcal_chindf = new TH1F("h_ohcal_chindf","",50,0,10);
  h_ohcal_chindf->GetXaxis()->SetTitle("Chi^2/NDF");
  h_ohcal_chindf->GetYaxis()->SetTitle("Counts");

  // iHCal setup
  TH2F *h_2Dihcal_hit = new TH2F("h_2Dihcal_hit", "", n_etabin, 0, n_etabin, n_phibin, 0, n_phibin);
  h_2Dihcal_hit->GetXaxis()->SetTitle("ieta");
  h_2Dihcal_hit->GetYaxis()->SetTitle("iphi");
  TH1F *h_ihcal_total;
  h_ihcal_total = new TH1F("h_ihcal_total","",200,0,10000);
  h_ihcal_total->GetXaxis()->SetTitle("ADC");
  h_ihcal_total->GetYaxis()->SetTitle("Counts");
  TH1F *h_ihcal_channel[n_etabin][n_phibin];
  for (int ieta = 0; ieta < n_etabin; ++ieta) {
    for (int iphi = 0; iphi < n_phibin; ++iphi) {
      std::ostringstream h_ihcal_channelname;
      h_ihcal_channelname << "h_ihcal_channel_" << ieta << "_" << iphi;
      h_ihcal_channel[ieta][iphi] = new TH1F(h_ihcal_channelname.str().c_str(), "", 200, 0, 10000);
      h_ihcal_channel[ieta][iphi]->GetXaxis()->SetTitle("ADC");
      h_ihcal_channel[ieta][iphi]->GetYaxis()->SetTitle("Counts");
    }
  }
  TH1F *h_ihcal_chindf = new TH1F("h_ihcal_chindf","",50,0,10);
  h_ihcal_chindf->GetXaxis()->SetTitle("Chi^2/NDF");
  h_ihcal_chindf->GetYaxis()->SetTitle("Counts");

  // Read hist
  TH1F *h_temp_channel;
  for (int ieta = 0; ieta < n_etabin; ++ieta) {
    for (int iphi = 0; iphi < n_phibin; ++iphi) {
      std::ostringstream h_temp_channelname;
      h_temp_channelname << "h_channel_" << ieta << "_" << iphi;
      h_temp_channel = (TH1F*)ohcal_input->Get(h_temp_channelname.str().c_str());
      h_ohcal_channel[ieta][iphi]->Add(h_temp_channel);
      h_ohcal_total->Add(h_temp_channel);
      h_temp_channel = (TH1F*)ihcal_input->Get(h_temp_channelname.str().c_str());
      h_ihcal_channel[ieta][iphi]->Add(h_temp_channel);
      h_ihcal_total->Add(h_temp_channel);
    }
  }
  ohcal_input->Close();
  ihcal_input->Close();

  // Fit hist.
  float peak_ohcal_total, peak_ihcal_total, chindf_ohcal_total, chindf_ihcal_total;
  float peak_ohcal[n_etabin][n_phibin], peak_ihcal[n_etabin][n_phibin];
  float chindf_ohcal[n_etabin][n_phibin], chindf_ihcal[n_etabin][n_phibin];
  TF1 *f_gamma = new TF1("f_gamma", gamma_function, 0, 10000, 4);
  f_gamma->SetParName(0,"Peak(ADC)");
  f_gamma->SetParName(1,"Shift");
  f_gamma->SetParName(2,"Scale");
  f_gamma->SetParName(3,"N");

  do_peak_fit(h_ohcal_total, f_gamma);
  peak_ohcal_total = f_gamma->GetParameter(0);
  chindf_ohcal_total = f_gamma->GetChisquare() / (float)f_gamma->GetNDF();

  do_peak_fit(h_ihcal_total, f_gamma);
  peak_ihcal_total = f_gamma->GetParameter(0);
  chindf_ihcal_total = f_gamma->GetChisquare() / (float)f_gamma->GetNDF();
  for (int ieta = 0; ieta < n_etabin; ++ieta) {
    for (int iphi = 0; iphi < n_phibin; ++iphi) {
      do_peak_fit(h_ohcal_channel[ieta][iphi], f_gamma);
      peak_ohcal[ieta][iphi] = f_gamma->GetParameter(0);
      chindf_ohcal[ieta][iphi] = f_gamma->GetChisquare() / (float)f_gamma->GetNDF();
      h_ohcal_chindf->Fill(chindf_ohcal[ieta][iphi]);
      do_peak_fit(h_ihcal_channel[ieta][iphi], f_gamma);
      peak_ihcal[ieta][iphi] = f_gamma->GetParameter(0);
      chindf_ihcal[ieta][iphi] = f_gamma->GetChisquare() / (float)f_gamma->GetNDF();
      h_ihcal_chindf->Fill(chindf_ihcal[ieta][iphi]);

      h_2Dohcal_hit->SetBinContent(ieta+1, iphi+1, h_ohcal_channel[ieta][iphi]->GetEntries());
      h_2Dihcal_hit->SetBinContent(ieta+1, iphi+1, h_ihcal_channel[ieta][iphi]->GetEntries());
    }
  }
  delete f_gamma;

  // Output.
  // Get Calib info.
  float ihcal_samplingfraction = 0.162166;
  float ohcal_samplingfraction = 3.38021e-02;
  float ihcal_mipcalib, ohcal_mipcalib, ihcal_abscalib, ohcal_abscalib;
  for (int ieta = 0; ieta < n_etabin; ++ieta) {
    for (int iphi = 0; iphi < n_phibin; ++iphi) {
      ihcal_mipcalib = peak_ihcal[ieta][iphi] / 32.;
      ohcal_mipcalib = peak_ohcal[ieta][iphi] / 32.;
      ihcal_abscalib = 1. / ihcal_mipcalib / ihcal_samplingfraction; // abs calib in GeV per ADC
      ohcal_abscalib = 1. / ohcal_mipcalib / ohcal_samplingfraction; // abs calib in GeV per ADC
      ihcal_calibinfo << ieta << "\t" << iphi << "\t" << ihcal_abscalib << std::endl;
      ohcal_calibinfo << ieta << "\t" << iphi << "\t" << ohcal_abscalib << std::endl;
    }
  }

  total_mpvinfo << "oHCal: MPV = " << peak_ohcal_total << ", Chi^2/NDF = " << chindf_ohcal_total << std::endl;
  total_mpvinfo << "iHCal: MPV = " << peak_ihcal_total << ", Chi^2/NDF = " << chindf_ihcal_total << std::endl;
  total_mpvinfo << std::endl << "Channel-by-channel Chi^2/NDF value: " << std::endl << "    oHCal:" << h_ohcal_chindf->GetMean() << std::endl << "    iHCal:" << h_ihcal_chindf->GetMean() << std::endl;
  std::vector<int> badfit_eta_ohcal, badfit_phi_ohcal, badfit_eta_ihcal, badfit_phi_ihcal;
  for (int ieta = 0; ieta < n_etabin; ++ieta) {
    for (int iphi = 0; iphi < n_phibin; ++iphi) {
      if (chindf_ohcal[ieta][iphi] > 2) {
        badfit_eta_ohcal.push_back(ieta);
        badfit_phi_ohcal.push_back(iphi);
      }
      if (chindf_ihcal[ieta][iphi] > 2) {
        badfit_eta_ihcal.push_back(ieta);
        badfit_phi_ihcal.push_back(iphi);
      }
      ohcal_output->cd();
      h_ohcal_channel[ieta][iphi]->Write();
      delete h_ohcal_channel[ieta][iphi];
      ihcal_output->cd();
      h_ihcal_channel[ieta][iphi]->Write();
      delete h_ihcal_channel[ieta][iphi];
      ohcal_mpvinfo << ieta << "\t" << iphi << "\t" << peak_ohcal[ieta][iphi] << "\t" << chindf_ohcal[ieta][iphi] << std::endl;
      ihcal_mpvinfo << ieta << "\t" << iphi << "\t" << peak_ihcal[ieta][iphi] << "\t" << chindf_ihcal[ieta][iphi] << std::endl;
    }
  }
  ohcal_mpvinfo.close();
  ihcal_mpvinfo.close();

  total_mpvinfo << std::endl << "oHCal bad fit channel:" << std::endl;
  for (int n_bad = 0; n_bad < badfit_eta_ohcal.size(); ++n_bad) {
    total_mpvinfo << badfit_eta_ohcal[n_bad] << "\t" << badfit_phi_ohcal[n_bad] << "\t" << chindf_ohcal[badfit_eta_ohcal[n_bad]][badfit_phi_ohcal[n_bad]];
  }
  total_mpvinfo << std::endl << "iHCal bad fit channel:" << std::endl;
  for (int n_bad = 0; n_bad < badfit_eta_ihcal.size(); ++n_bad) {
    total_mpvinfo << badfit_eta_ihcal[n_bad] << "\t" << badfit_phi_ihcal[n_bad] << "\t" << chindf_ihcal[badfit_eta_ihcal[n_bad]][badfit_phi_ihcal[n_bad]];
  }
  total_mpvinfo.close();

  ohcal_output->cd();
  h_ohcal_total->Write();
  h_ohcal_chindf->Write();
  h_2Dohcal_hit->Write();
  delete h_ohcal_total;
  delete h_ohcal_chindf;
  ohcal_output->Close();

  ihcal_output->cd();
  h_ihcal_total->Write();
  h_ihcal_chindf->Write();
  h_2Dihcal_hit->Write();
  delete h_ihcal_total;
  delete h_ihcal_chindf;
  ihcal_output->Close();
}
