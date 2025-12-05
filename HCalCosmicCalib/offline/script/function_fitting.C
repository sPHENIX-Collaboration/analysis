#include <iostream>
#include <string>
#include <fstream>
#include <TAxis.h>
#include <TGraph.h>
#include <TSpectrum.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TROOT.h>
#include <TFile.h>
#include "TError.h"

double gamma_function(double *x, double *par);
float get_temp_shift(TH1F* hist, float y);
float get_temp_scale(TH1F* hist, float x);
float get_temp_N(float x, float y, float shift, float scale);
float get_fit_min(TH1F* hist, float y);
float get_fit_max(TH1F* hist, float y);
void do_peak_finder(TH1F* hist, float& x, float& y, float& shift, float& scale, float& N, float& min, float& max);
void do_peak_fit(TH1F* hist, TF1* func);

void function_fitting(int min_run, int max_run, std::string calo_type) {
  static const int n_etabin = 24;
  static const int n_phibin = 64;
  
  // Output file for each run.
  std::string input_filename, output_filename, check_filename, mpv_filename;
  if (calo_type == "ihcal") {
    input_filename = "hist/ihcal_hist_" + std::to_string(min_run) + "_" + std::to_string(max_run) + ".root";
    output_filename = "output/fitresult_ihcal_" + std::to_string(min_run) + "_" + std::to_string(max_run) + ".root";
    check_filename = "output/fitcheck_ihcal_" + std::to_string(min_run) + "_" + std::to_string(max_run) + ".root";
    mpv_filename = "output/ihcal_mpv_" + std::to_string(min_run) + "_" + std::to_string(max_run) + ".txt";
  } else if (calo_type == "ohcal") {
    input_filename = "hist/ohcal_hist_" + std::to_string(min_run) + "_" + std::to_string(max_run) + ".root";
    output_filename = "output/fitresult_ohcal_" + std::to_string(min_run) + "_" + std::to_string(max_run) + ".root";
    check_filename = "output/fitcheck_ohcal_" + std::to_string(min_run) + "_" + std::to_string(max_run) + ".root";
    mpv_filename = "output/ohcal_mpv_" + std::to_string(min_run) + "_" + std::to_string(max_run) + ".txt";
  } else {
    std::cout << "Invalid calo_type specified. Use 'ihcal' or 'ohcal'." << std::endl;
    return;
  }

  TFile* hist_input = new TFile(input_filename.c_str(), "READ");
  TFile* output_file = new TFile(output_filename.c_str(), "RECREATE");
  TFile* check_file = new TFile(check_filename.c_str(), "RECREATE");
  ofstream mpv_file(mpv_filename.c_str(), std::ios::trunc);

  if (!hist_input || hist_input->IsZombie()) {
    std::cout << "Input file not found: " << input_filename << std::endl;
    return;
  }

  TH1F* h_event = dynamic_cast<TH1F*>(hist_input->Get("h_event"));
  TH1F* h_channel[n_etabin][n_phibin];
  for (int ieta = 0; ieta < n_etabin; ++ieta) {
    for (int iphi = 0; iphi < n_phibin; ++iphi) {
      std::string hist_name = Form("h_channel_%d_%d", ieta, iphi);
      h_channel[ieta][iphi] = (TH1F*)hist_input->Get(hist_name.c_str());
    }
  }
    
  // Setup check hist.
  TH1F* h_mip; h_mip = (TH1F*)hist_input->Get("h_mip");
  TH2F* h_2D_hit = new TH2F("h_2D_hit", ";ieta;iphi", n_etabin, 0, n_etabin, n_phibin, 0, n_phibin);
  TH2F* h_2D_mpv = new TH2F("h_2D_mpv", ";ieta;iphi", n_etabin, 0, n_etabin, n_phibin, 0, n_phibin);
  TH2F* h_2D_mean = new TH2F("h_2D_mean", ";ieta;iphi", n_etabin, 0, n_etabin, n_phibin, 0, n_phibin);
  TH2F* h_2D_mpvuncertainty = new TH2F("h_2D_mpvuncertainty", ";ieta;iphi", n_etabin, 0, n_etabin, n_phibin, 0, n_phibin);
  TH2F* h_2D_meanuncertainty = new TH2F("h_2D_meanuncertainty", ";ieta;iphi", n_etabin, 0, n_etabin, n_phibin, 0, n_phibin);
  TH2F* h_2D_fitchi2 = new TH2F("h_2D_fitchi2", ";ieta;iphi", n_etabin, 0, n_etabin, n_phibin, 0, n_phibin);
    
  // Fit channel hist and fill check hist & check file.
  TF1 *f_gamma = new TF1("f_gamma", gamma_function, 0, 10000, 4);
  f_gamma->SetParName(0,"Peak(ADC)");
  f_gamma->SetParName(1,"Shift");
  f_gamma->SetParName(2,"Scale");
  f_gamma->SetParName(3,"N");
  do_peak_fit(h_mip, f_gamma);
  for (int ieta = 0; ieta < n_etabin; ++ieta) {
    for (int iphi = 0; iphi < n_phibin; ++iphi) {
      //std::cout << "Fitting ieta: " << ieta << " iphi: " << iphi << std::endl;
      do_peak_fit(h_channel[ieta][iphi], f_gamma);
      h_2D_hit->SetBinContent(ieta+1, iphi+1, h_channel[ieta][iphi]->GetEntries());
      h_2D_mpv->SetBinContent(ieta+1, iphi+1, f_gamma->GetParameter(0));
      h_2D_mean->SetBinContent(ieta+1, iphi+1, h_channel[ieta][iphi]->GetMean());
      h_2D_mpvuncertainty->SetBinContent(ieta+1, iphi+1, f_gamma->GetParError(0)/(float)f_gamma->GetParameter(0));
      h_2D_meanuncertainty->SetBinContent(ieta+1, iphi+1, h_channel[ieta][iphi]->GetMeanError()/(float)h_channel[ieta][iphi]->GetMean());
      h_2D_fitchi2->SetBinContent(ieta+1, iphi+1, f_gamma->GetChisquare()/(float)f_gamma->GetNDF());

      if (h_channel[ieta][iphi]->GetEntries() == 0) {
        mpv_file << ieta << " " << iphi << " " << 0 << std::endl;
      } else {
        mpv_file << ieta << " " << iphi << " " << f_gamma->GetParameter(0) << std::endl;
      }
    }
  }
    
  // Save hist.
  output_file->cd();
  for (int ieta = 0; ieta < n_etabin; ++ieta) {
    for (int iphi = 0; iphi < n_phibin; ++iphi) {
      h_channel[ieta][iphi]->Write();
    }
  }
  check_file->cd();
  h_mip->Write();
  h_2D_hit->Write();
  h_2D_mpv->Write();
  h_2D_mean->Write();
  h_2D_mpvuncertainty->Write();
  h_2D_meanuncertainty->Write();
  h_2D_fitchi2->Write();
  h_event->Write();
}

double gamma_function(double *x, double *par) {
  double peak = par[0];
  double shift = par[1];
  double scale = par[2];
  double N = par[3];

  if (scale==0) return 0;
  double arg_para = (x[0] - shift) / scale;
  if (arg_para < 0) arg_para=0;
  double peak_para = (peak - shift) / scale;
  double numerator = N * pow(arg_para, peak_para) * TMath::Exp(-arg_para);
  double denominator = ROOT::Math::tgamma(peak_para + 1) * scale;
  if (denominator==0) return 1e8;
  double val = numerator / denominator;
  if (isnan(val)) return 0;
  return val;
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
      min = hist->GetBinCenter(i-2);
      if (hist->GetBinContent(i) > 3*y/4.) {
        min = hist->GetBinCenter(i-3);
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
      max = hist->GetBinCenter(i-3);
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

void set_parameters(TF1* func, float peak_position, float shift, float scale, float N) {
  func->SetParameter(0, peak_position);
  func->SetParameter(1, shift);
  func->SetParameter(2, scale);
  func->SetParameter(3, N);
}

void do_peak_fit(TH1F* hist, TF1* func) {
  int n_rebin = 5;
  float bin_width = hist->GetBinWidth(1);

  // *** First fitting try ***//
  int min_nloop = 9; // loop through min ?bins
  int min_lowloop = 4; // loop from finder_min - ?bins
  int max_nloop = 13; // loop through max ?bins
  int max_lowloop = 3; // loop from finder_max - ?bins

  int min_lowlimit = 2; // min limit: shift + ?bins
  int min_highlimit = 4; // min limit: peak - ?bins
  int max_lowlimit = 4; // max limit: peak + ?bins

  gErrorIgnoreLevel = kError;
  hist->Rebin(n_rebin);
  gErrorIgnoreLevel = kWarning;
  float peak_position_finder, peak_value_finder, shift_finder, scale_finder, N_finder, min_finder, max_finder;
  do_peak_finder(hist, peak_position_finder, peak_value_finder, shift_finder, scale_finder, N_finder, min_finder, max_finder);
  if (std::isnan(N_finder)) N_finder = hist->GetEntries();
  float fitting_best_chi2 = 1e8;
  float fitting_min_recorder, fitting_max_recorder;
  float fitting_min, fitting_max;
  for (int imin = 0; imin < min_nloop; ++imin) {
    bool reach_max = false;
    if (min_finder + imin*bin_width*n_rebin - min_lowloop*bin_width*n_rebin <= shift_finder + min_lowlimit*bin_width*n_rebin) continue;
    if (reach_max) continue;
    if (min_finder + imin*bin_width*n_rebin - min_lowloop*bin_width*n_rebin >= peak_position_finder - min_highlimit*bin_width*n_rebin) {
      fitting_min = peak_position_finder - min_highlimit*bin_width*n_rebin;
      reach_max = true;
    } else {
      fitting_min = min_finder + imin*bin_width*n_rebin - min_lowloop*bin_width*n_rebin;
    }
    for (int imax = 0; imax < max_nloop; ++imax) {
      if (max_finder + imax*bin_width*n_rebin - max_lowloop*bin_width*n_rebin <= peak_position_finder + max_lowlimit*bin_width*n_rebin) continue;
      set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
      fitting_max = max_finder + imax*bin_width*n_rebin - max_lowloop*bin_width*n_rebin;
      hist->Fit(func, "Q", "", fitting_min, fitting_max);
      float chi2ndf = func->GetChisquare()/(float)func->GetNDF();
      if (chi2ndf < fitting_best_chi2 && !std::isnan(func->GetParError(0)) && !std::isnan(func->GetParError(1)) && !std::isnan(func->GetParError(2)) && !std::isnan(func->GetParError(3))) {
        fitting_best_chi2 = chi2ndf;
        fitting_min_recorder = fitting_min;
        fitting_max_recorder = fitting_max;
      }
    }
  }
  set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
  hist->Fit(func, "Q", "", fitting_min_recorder, fitting_max_recorder);

  // Try to fit again if the error is large. (Usually caused by low fitting range.)
  if (func->GetParError(0) / (float)func->GetParameter(0) > 1.1*(0.0121+9.323/(float)hist->GetEntries()) || func->GetParError(0) / (float)func->GetParameter(0) < 0) {
    float fitting_min_recorder_test1 = fitting_min_recorder - 2*min_lowloop*bin_width;
    float fitting_max_recorder_test1 = fitting_max_recorder + 2*min_lowloop*bin_width;
    set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
    hist->Fit(func, "Q", "", fitting_min_recorder_test1, fitting_max_recorder_test1);
    if (std::isnan(func->GetParError(0)) || std::isnan(func->GetParError(1)) || std::isnan(func->GetParError(2)) || std::isnan(func->GetParError(3))) {
      fitting_min_recorder_test1 = fitting_min_recorder;
      fitting_max_recorder_test1 = fitting_max_recorder;
      set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
      hist->Fit(func, "Q", "", fitting_min_recorder_test1, fitting_max_recorder_test1);
    }
    if (func->GetParError(0) / (float)func->GetParameter(0) > 1.1*(0.0121+9.323/(float)hist->GetEntries()) || func->GetParError(0) / (float)func->GetParameter(0) < 0) {
      float fitting_min_recorder_test2 = fitting_min_recorder - 4*min_lowloop*bin_width;
      float fitting_max_recorder_test2 = fitting_max_recorder + 4*min_lowloop*bin_width;
      set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
      hist->Fit(func, "Q", "", fitting_min_recorder_test2, fitting_max_recorder_test2);
      if (std::isnan(func->GetParError(0)) || std::isnan(func->GetParError(1)) || std::isnan(func->GetParError(2)) || std::isnan(func->GetParError(3))) {
        fitting_min_recorder_test2 = fitting_min_recorder_test1;
        fitting_max_recorder_test2 = fitting_max_recorder_test1;
        set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
        hist->Fit(func, "Q", "", fitting_min_recorder_test2, fitting_max_recorder_test2);
      }
      if (func->GetParError(0) / (float)func->GetParameter(0) > 1.1*(0.0121+9.323/(float)hist->GetEntries()) || func->GetParError(0) / (float)func->GetParameter(0) < 0) {
        float fitting_min_recorder_test3 = fitting_min_recorder - 6*min_lowloop*bin_width;
        float fitting_max_recorder_test3 = fitting_max_recorder + 6*min_lowloop*bin_width;
        set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
        hist->Fit(func, "Q", "", fitting_min_recorder_test3, fitting_max_recorder_test3);
        if (std::isnan(func->GetParError(0)) || std::isnan(func->GetParError(1)) || std::isnan(func->GetParError(2)) || std::isnan(func->GetParError(3))) {
          fitting_min_recorder_test3 = fitting_min_recorder_test2;
          fitting_max_recorder_test3 = fitting_max_recorder_test2;
          set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
          hist->Fit(func, "Q", "", fitting_min_recorder_test3, fitting_max_recorder_test3);
        }
      }
    }
  }

  // A quick try to fix fitting failure. (Usually caused by parameter preset.)
  if (std::isnan(func->GetParError(0)) || std::isnan(func->GetParError(1)) || std::isnan(func->GetParError(2)) || std::isnan(func->GetParError(3))) {
    set_parameters(func, peak_position_finder, 2, 1.5, N_finder);
    hist->Fit(func, "Q", "", fitting_min_recorder - min_lowloop*bin_width, fitting_max_recorder + 15*min_lowloop*bin_width);
  }


  // *** Second fitting try (with larger range) ***//
  if (func->GetChisquare()/(float)func->GetNDF() > 2 || std::isnan(func->GetParError(0)) || std::isnan(func->GetParError(1)) || std::isnan(func->GetParError(2)) || std::isnan(func->GetParError(3))) {
    min_nloop = 15; // loop through min ?bins
    min_lowloop = 8; // loop from finder_min - ?bins
    max_nloop = 25; // loop through max ?bins
    max_lowloop = 5; // loop from finder_max - ?bins

    min_lowlimit = 1; // min limit: shift + ?bins
    min_highlimit = 3; // min limit: peak - ?bins
    max_lowlimit = 4; // max limit: peak + ?bins

    fitting_best_chi2 = 1e8;
    for (int imin = 0; imin < min_nloop; ++imin) {
      bool reach_max = false;
      if (min_finder + imin*bin_width*n_rebin - min_lowloop*bin_width*n_rebin <= shift_finder + min_lowlimit*bin_width*n_rebin) continue;
      if (reach_max) continue;
      if (min_finder + imin*bin_width*n_rebin - min_lowloop*bin_width*n_rebin >= peak_position_finder - min_highlimit*bin_width*n_rebin) {
        fitting_min = peak_position_finder - min_highlimit*bin_width*n_rebin;
        reach_max = true;
      } else {
        fitting_min = min_finder + imin*bin_width*n_rebin - min_lowloop*bin_width*n_rebin;
      }
      for (int imax = 0; imax < max_nloop; ++imax) {
      if (max_finder + imax*bin_width*n_rebin - max_lowloop*bin_width*n_rebin <= peak_position_finder + max_lowlimit*bin_width*n_rebin) continue;
        set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
        fitting_max = max_finder + imax*bin_width*n_rebin - max_lowloop*bin_width*n_rebin;
        hist->Fit(func, "Q", "", fitting_min, fitting_max);
        float chi2ndf = func->GetChisquare()/(float)func->GetNDF();
        if (chi2ndf < fitting_best_chi2 && !std::isnan(func->GetParError(0)) && !std::isnan(func->GetParError(1)) && !std::isnan(func->GetParError(2)) && !std::isnan(func->GetParError(3))) {
          fitting_best_chi2 = chi2ndf;
          fitting_min_recorder = fitting_min;
          fitting_max_recorder = fitting_max;
        }
      }
    }
    set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
    hist->Fit(func, "Q", "", fitting_min_recorder, fitting_max_recorder);

    // Try to fit again if the error is large. (Usually caused by low fitting range.)
    if (func->GetParError(0) / (float)func->GetParameter(0) > 1.1*(0.0121+9.323/(float)hist->GetEntries()) || func->GetParError(0) / (float)func->GetParameter(0) < 0) {
      float fitting_min_recorder_test1 = fitting_min_recorder - 2*min_lowloop*bin_width;
      float fitting_max_recorder_test1 = fitting_max_recorder + 2*min_lowloop*bin_width;
      set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
      hist->Fit(func, "Q", "", fitting_min_recorder_test1, fitting_max_recorder_test1);
      if (std::isnan(func->GetParError(0)) || std::isnan(func->GetParError(1)) || std::isnan(func->GetParError(2)) || std::isnan(func->GetParError(3))) {
        fitting_min_recorder_test1 = fitting_min_recorder;
        fitting_max_recorder_test1 = fitting_max_recorder;
        set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
        hist->Fit(func, "Q", "", fitting_min_recorder_test1, fitting_max_recorder_test1);
      }
      if (func->GetParError(0) / (float)func->GetParameter(0) > 1.1*(0.0121+9.323/(float)hist->GetEntries()) || func->GetParError(0) / (float)func->GetParameter(0) < 0) {
        float fitting_min_recorder_test2 = fitting_min_recorder - 4*min_lowloop*bin_width;
        float fitting_max_recorder_test2 = fitting_max_recorder + 4*min_lowloop*bin_width;
        set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
        hist->Fit(func, "Q", "", fitting_min_recorder_test2, fitting_max_recorder_test2);
        if (std::isnan(func->GetParError(0)) || std::isnan(func->GetParError(1)) || std::isnan(func->GetParError(2)) || std::isnan(func->GetParError(3))) {
          fitting_min_recorder_test2 = fitting_min_recorder_test1;
          fitting_max_recorder_test2 = fitting_max_recorder_test1;
          set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
          hist->Fit(func, "Q", "", fitting_min_recorder_test2, fitting_max_recorder_test2);
        }
        if (func->GetParError(0) / (float)func->GetParameter(0) > 1.1*(0.0121+9.323/(float)hist->GetEntries()) || func->GetParError(0) / (float)func->GetParameter(0) < 0) {
          float fitting_min_recorder_test3 = fitting_min_recorder - 6*min_lowloop*bin_width;
          float fitting_max_recorder_test3 = fitting_max_recorder + 6*min_lowloop*bin_width;
          set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
          hist->Fit(func, "Q", "", fitting_min_recorder_test3, fitting_max_recorder_test3);
          if (std::isnan(func->GetParError(0)) || std::isnan(func->GetParError(1)) || std::isnan(func->GetParError(2)) || std::isnan(func->GetParError(3))) {
            fitting_min_recorder_test3 = fitting_min_recorder_test2;
            fitting_max_recorder_test3 = fitting_max_recorder_test2;
            set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
            hist->Fit(func, "Q", "", fitting_min_recorder_test3, fitting_max_recorder_test3);
          }
        }
      }
    }
  }


  // *** Third fitting try (with larger range and certain preset) ***//
  if (std::isnan(func->GetParError(0)) || std::isnan(func->GetParError(1)) || std::isnan(func->GetParError(2)) || std::isnan(func->GetParError(3))) {
    min_nloop = 15; // loop through min ?bins
    min_lowloop = 8; // loop from finder_min - ?bins
    max_nloop = 25; // loop through max ?bins
    max_lowloop = 5; // loop from finder_max - ?bins

    min_lowlimit = 1; // min limit: shift + ?bins
    min_highlimit = 3; // min limit: peak - ?bins
    max_lowlimit = 4; // max limit: peak + ?bins

    fitting_best_chi2 = 1e8;
    for (int imin = 0; imin < min_nloop; ++imin) {
      bool reach_max = false;
      if (min_finder + imin*bin_width*n_rebin - min_lowloop*bin_width*n_rebin <= shift_finder + min_lowlimit*bin_width*n_rebin) continue;
      if (reach_max) continue;
      if (min_finder + imin*bin_width*n_rebin - min_lowloop*bin_width*n_rebin >= peak_position_finder - min_highlimit*bin_width*n_rebin) {
        fitting_min = peak_position_finder - min_highlimit*bin_width*n_rebin;
        reach_max = true;
      } else {
        fitting_min = min_finder + imin*bin_width*n_rebin - min_lowloop*bin_width*n_rebin;
      }
      for (int imax = 0; imax < max_nloop; ++imax) {
      if (max_finder + imax*bin_width*n_rebin - max_lowloop*bin_width*n_rebin <= peak_position_finder + max_lowlimit*bin_width*n_rebin) continue;
        set_parameters(func, peak_position_finder, 2, 1.5, N_finder);
        fitting_max = max_finder + imax*bin_width*n_rebin - max_lowloop*bin_width*n_rebin;
        hist->Fit(func, "Q", "", fitting_min, fitting_max);
        float chi2ndf = func->GetChisquare()/(float)func->GetNDF();
        if (chi2ndf < fitting_best_chi2 && !std::isnan(func->GetParError(0)) && !std::isnan(func->GetParError(1)) && !std::isnan(func->GetParError(2)) && !std::isnan(func->GetParError(3))) {
          fitting_best_chi2 = chi2ndf;
          fitting_min_recorder = fitting_min;
          fitting_max_recorder = fitting_max;
        }
      }
    }
    set_parameters(func, peak_position_finder, 2, 1.5, N_finder);
    hist->Fit(func, "Q", "", fitting_min_recorder, fitting_max_recorder);

    // Try to fit again if the error is large. (Usually caused by low fitting range.)
    if (func->GetParError(0) / (float)func->GetParameter(0) > 1.1*(0.0121+9.323/(float)hist->GetEntries()) || func->GetParError(0) / (float)func->GetParameter(0) < 0) {
      float fitting_min_recorder_test1 = fitting_min_recorder - 2*min_lowloop*bin_width;
      float fitting_max_recorder_test1 = fitting_max_recorder + 2*min_lowloop*bin_width;
      set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
      hist->Fit(func, "Q", "", fitting_min_recorder_test1, fitting_max_recorder_test1);
      if (std::isnan(func->GetParError(0)) || std::isnan(func->GetParError(1)) || std::isnan(func->GetParError(2)) || std::isnan(func->GetParError(3))) {
        fitting_min_recorder_test1 = fitting_min_recorder;
        fitting_max_recorder_test1 = fitting_max_recorder;
        set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
        hist->Fit(func, "Q", "", fitting_min_recorder_test1, fitting_max_recorder_test1);
      }
      if (func->GetParError(0) / (float)func->GetParameter(0) > 1.1*(0.0121+9.323/(float)hist->GetEntries()) || func->GetParError(0) / (float)func->GetParameter(0) < 0) {
        float fitting_min_recorder_test2 = fitting_min_recorder - 4*min_lowloop*bin_width;
        float fitting_max_recorder_test2 = fitting_max_recorder + 4*min_lowloop*bin_width;
        set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
        hist->Fit(func, "Q", "", fitting_min_recorder_test2, fitting_max_recorder_test2);
        if (std::isnan(func->GetParError(0)) || std::isnan(func->GetParError(1)) || std::isnan(func->GetParError(2)) || std::isnan(func->GetParError(3))) {
          fitting_min_recorder_test2 = fitting_min_recorder_test1;
          fitting_max_recorder_test2 = fitting_max_recorder_test1;
          set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
          hist->Fit(func, "Q", "", fitting_min_recorder_test2, fitting_max_recorder_test2);
        }
        if (func->GetParError(0) / (float)func->GetParameter(0) > 1.1*(0.0121+9.323/(float)hist->GetEntries()) || func->GetParError(0) / (float)func->GetParameter(0) < 0) {
          float fitting_min_recorder_test3 = fitting_min_recorder - 6*min_lowloop*bin_width;
          float fitting_max_recorder_test3 = fitting_max_recorder + 6*min_lowloop*bin_width;
          set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
          hist->Fit(func, "Q", "", fitting_min_recorder_test3, fitting_max_recorder_test3);
          if (std::isnan(func->GetParError(0)) || std::isnan(func->GetParError(1)) || std::isnan(func->GetParError(2)) || std::isnan(func->GetParError(3))) {
            fitting_min_recorder_test3 = fitting_min_recorder_test2;
            fitting_max_recorder_test3 = fitting_max_recorder_test2;
            set_parameters(func, peak_position_finder, shift_finder, scale_finder, N_finder);
            hist->Fit(func, "Q", "", fitting_min_recorder_test3, fitting_max_recorder_test3);
          }
        }
      }
    }
  }
}
