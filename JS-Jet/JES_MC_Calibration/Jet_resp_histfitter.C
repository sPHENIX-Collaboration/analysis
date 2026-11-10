#include "AtlasStyle.C"
#include "AtlasUtils.C"
#include "Jet_resp_Def.h"
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TF1.h>
#include <TMath.h>
#include <TSystem.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//For normal analysis
void fit_resogaus(int index, TH1D* h, TF1* f_gaus, float &fit_mean, float &fit_sigma, float &fit_meanerr, float &fit_sigmaerr, float &fit_chi2ndf) {
  if (!h || !f_gaus) {
    fit_mean = 0; fit_sigma = 0; fit_meanerr = 0; fit_sigmaerr = 0; fit_chi2ndf = -1;
    std::cerr << "Error: Input TH1D or TF1 is null!" << std::endl;
    return;
  }

  f_gaus->SetParLimits(1, 0.2, 1.1); // Mean should be within a reasonable range for response (loose lower bound for small radii)

  double mean = h->GetMean();
  if ((mean < 0.4 || mean > 0.8) && index >= 0 && index < 7) mean = 0.5;
  else if ((mean < 0.4 || mean > 0.8) && index >= 7) mean = 0.75;
  double sigma = h->GetRMS();
  if (sigma > 0.5 || sigma < 0.05) sigma = 0.15;
  double amplitude = h->GetMaximum();
  
  f_gaus->SetParameters(amplitude, mean, sigma);

  // Fit windows: seed pass at mu +/- 2 sigma, then mu +/- max(0.2, 1.5 sigma),
  // clamped to [0.3, 1.3].
  double fit_min = mean - 2*sigma;
  double fit_max = mean + 2*sigma;
  if (fit_min < 0.3) fit_min = 0.3;
  if (fit_max > 1.3) fit_max = 1.3;
  h->Fit(f_gaus, "RQ0", "", fit_min, fit_max);

  amplitude = f_gaus->GetParameter(0);
  mean = f_gaus->GetParameter(1);
  if ((mean < 0.4 || mean > 0.9) && index >= 0 && index < 7) mean = 0.5;
  else if ((mean < 0.4 || mean > 0.9) && index >= 7) mean = 0.75;
  sigma = f_gaus->GetParameter(2);
  if (sigma > 0.7 || sigma < 0) sigma = 0.15;

  f_gaus->SetParameters(amplitude, mean, sigma);
  double half_width = TMath::Max(0.2, 1.5*sigma);
  fit_min = mean - half_width;
  fit_max = mean + half_width;
  if (fit_min < 0.3) fit_min = 0.3;
  if (fit_max > 1.3) fit_max = 1.3;
  h->Fit(f_gaus, "RQ0", "", fit_min, fit_max);

  if (f_gaus->GetNDF() > 0 && f_gaus->GetChisquare() / (float)f_gaus->GetNDF() > 10) {
    if(index >= 0 && index < 5) f_gaus->SetParameters(amplitude, 0.55, 0.2);
    else if(index >= 5 && index < 10) f_gaus->SetParameters(amplitude, 0.6, 0.15);
    else if(index >= 10 && index < 20) f_gaus->SetParameters(amplitude, 0.68, 0.15);
    else f_gaus->SetParameters(amplitude, 0.75, 0.15);

    half_width = TMath::Max(0.2, 1.5*sigma);
    fit_min = mean - half_width;
    fit_max = mean + half_width;
    if (fit_min < 0.3) fit_min = 0.3;
    if (fit_max > 1.3) fit_max = 1.3;
    h->Fit(f_gaus, "RQ0", "", fit_min, fit_max);
  }

  amplitude = f_gaus->GetParameter(0);
  mean = f_gaus->GetParameter(1);
  sigma = f_gaus->GetParameter(2);
  f_gaus->SetParameters(amplitude, mean, sigma);

  half_width = TMath::Max(0.2, 1.5*sigma);
  fit_min = mean - half_width;
  fit_max = mean + half_width;
  if (fit_min < 0.3) fit_min = 0.3;
  if (fit_max > 1.3) fit_max = 1.3;

  h->Fit(f_gaus, "RQ", "", fit_min, fit_max);

  fit_mean = f_gaus->GetParameter(1);
  fit_sigma = f_gaus->GetParameter(2);
  fit_meanerr = f_gaus->GetParError(1);
  fit_sigmaerr = f_gaus->GetParError(2);
  fit_chi2ndf = (f_gaus->GetNDF() > 0) ? f_gaus->GetChisquare() / (float)f_gaus->GetNDF() : -1;
}

// For closure test.
// Self-adaptive setup so the same code handles small |z| (narrow, centered at 1) and
// large |z| (broader, mean below 1): the seed comes from the peak position (robust
// against tails) and the fit window scales with the fitted sigma. Sparse slices are
// rebinned adaptively upstream, so chi2 fits stay stable despite low statistics.
void fit_resogaus_closure(int index, TH1D* h, TF1* f_gaus, float &fit_mean, float &fit_sigma, float &fit_meanerr, float &fit_sigmaerr, float &fit_chi2ndf) {
  if (!h || !f_gaus) {
    fit_mean = 0; fit_sigma = 0; fit_meanerr = 0; fit_sigmaerr = 0; fit_chi2ndf = -1;
    std::cerr << "Error: Input TH1D or TF1 is null!" << std::endl;
    return;
  }

  f_gaus->SetParLimits(1, 0.5, 1.2); // mean can sit below 1 at large |z|

  // Chi2 fits on the weighted (Sumw2) slices: empty bins carry zero error and are simply
  // skipped by the fit; the rebinning upstream keeps sparse slices well-populated.
  const char* opt_quiet = "RQ0";
  const char* opt_final = "RQ";

  // Seed the mean from the peak position (robust against tails); fall back to the mean
  double mean = h->GetBinCenter(h->GetMaximumBin());
  if (mean < 0.5 || mean > 1.2) mean = h->GetMean();
  if (mean < 0.5 || mean > 1.2) mean = 1.0;
  double sigma = h->GetRMS();
  if (sigma > 0.5 || sigma < 0.05) sigma = 0.2;
  double amplitude = h->GetMaximum();

  f_gaus->SetParameters(amplitude, mean, sigma);

  // Fit windows (same convention as the first pass): seed pass at mu +/- 2 sigma,
  // then mu +/- max(0.2, 1.5 sigma), clamped to [0.5, 1.2].
  double fit_min, fit_max, half_width;
  fit_min = mean - 2*sigma;
  fit_max = mean + 2*sigma;
  if (fit_min < 0.5) fit_min = 0.5;
  if (fit_max > 1.2) fit_max = 1.2;
  h->Fit(f_gaus, opt_quiet, "", fit_min, fit_max);

  // Second Gaussian fitting: window follows the fitted core
  mean = f_gaus->GetParameter(1);
  if (mean < 0.5 || mean > 1.2) mean = 1.0;
  sigma = f_gaus->GetParameter(2);
  if (sigma > 0.5 || sigma < 0.05) sigma = 0.2;
  amplitude = f_gaus->GetParameter(0);
  f_gaus->SetParameters(amplitude, mean, sigma);
  half_width = TMath::Max(0.2, 1.5*sigma);
  fit_min = mean - half_width;
  fit_max = mean + half_width;
  if (fit_min < 0.5) fit_min = 0.5;
  if (fit_max > 1.2) fit_max = 1.2;
  h->Fit(f_gaus, opt_quiet, "", fit_min, fit_max);

  // Bad fit redo
  if (f_gaus->GetNDF() > 0 && f_gaus->GetChisquare() / (float)f_gaus->GetNDF() > 10) {
    f_gaus->SetParameters(amplitude, 1.0, 0.2);
    half_width = TMath::Max(0.2, 1.5*0.2);
    fit_min = 1.0 - half_width;
    fit_max = 1.0 + half_width;
    if (fit_min < 0.5) fit_min = 0.5;
    if (fit_max > 1.2) fit_max = 1.2;
    h->Fit(f_gaus, opt_quiet, "", fit_min, fit_max);
  }

  // Third (final) Gaussian fitting
  mean = f_gaus->GetParameter(1);
  sigma = f_gaus->GetParameter(2);
  if (sigma > 0.5 || sigma < 0.05) sigma = 0.2;
  amplitude = f_gaus->GetParameter(0);
  f_gaus->SetParameters(amplitude, mean, sigma);
  half_width = TMath::Max(0.2, 1.5*sigma);
  fit_min = mean - half_width;
  fit_max = mean + half_width;
  if (fit_min < 0.5) fit_min = 0.5;
  if (fit_max > 1.2) fit_max = 1.2;
  h->Fit(f_gaus, opt_final, "", fit_min, fit_max);

  fit_mean = f_gaus->GetParameter(1);
  fit_sigma = f_gaus->GetParameter(2);
  fit_meanerr = f_gaus->GetParError(1);
  fit_sigmaerr = f_gaus->GetParError(2);
  fit_chi2ndf = (f_gaus->GetNDF() > 0) ? f_gaus->GetChisquare() / (float)f_gaus->GetNDF() : -1;
}

void process_and_plot_single_TH3D(TH3D* h3, int r_val, int z_val, int eta_val, TFile* fout, TF1* f_gaus, bool do_closure, bool save_figures) {
// eta_val < 0: eta-integrated (original behavior); eta_val >= 0: one eta-fraction bin.
  TString eta_suffix = (eta_val < 0) ? "" : Form("_eta%d", eta_val);
// 1. Create Output TH2Ds
  TString mean_name = Form("h2_mean_r0%d_z%d%s", r_val, z_val, eta_suffix.Data());
  TString sigma_name = Form("h2_sigma_r0%d_z%d%s", r_val, z_val, eta_suffix.Data());
  TString chi2_name = Form("h2_chi2_r0%d_z%d%s", r_val, z_val, eta_suffix.Data());

  TH2D *h2_mean = new TH2D(mean_name, Form("Fitted Mean (R=0.%d, Z=%d);p_{T}^{truth jet} [GeV];E_{EMfrac}", r_val, z_val), n_truthptbin, truthptbins, n_emfracbin, emfracbins);
  TH2D *h2_sigma = new TH2D(sigma_name, Form("Fitted Sigma (R=0.%d, Z=%d);p_{T}^{truth jet} [GeV];E_{EMfrac}", r_val, z_val), n_truthptbin, truthptbins, n_emfracbin, emfracbins);
  // Fit quality per cell: chi2/ndf of the Gaussian fit, -1 = raw mean/RMS fallback
  TH2D *h2_chi2 = new TH2D(chi2_name, Form("Fit #chi^{2}/ndf (R=0.%d, Z=%d);p_{T}^{truth jet} [GeV];E_{EMfrac}", r_val, z_val), n_truthptbin, truthptbins, n_emfracbin, emfracbins);
 
  // Grid layout for 30 emfrac bins: 6 columns x 5 rows
  int n_cols = 6;
  int n_rows = (n_emfracbin + n_cols - 1) / n_cols; // dynamically calculates to 5
 
  // 2. Loop over truthpt bins (Create ONE canvas per truthpt bin)
  for (int ix = 1; ix <= n_truthptbin; ++ix) {
    
    // Create Canvas for THIS truthpt bin (only when figures are requested)
    TCanvas *c = nullptr;
    if (save_figures) {
      TString canvas_name = Form("c_r0%d_z%d%s_pt%02d", r_val, z_val, eta_suffix.Data(), ix);
      c = new TCanvas(canvas_name, Form("Fits R=0.%d Z=%d PT bin %d", r_val, z_val, ix), n_cols*300, n_rows*300);
      c->Divide(n_cols, n_rows);
    }
 
    // Vector to temporarily hold slices for this specific canvas
    std::vector<TH1D*> slices_this_pt;
 
    // Loop over emfrac bins (Fill the pads in the current canvas)
    for (int iy = 1; iy <= n_emfracbin; ++iy) {
 
      TString slice_name = Form("slice_r0%d_z%d%s_pt%d_em%d", r_val, z_val, eta_suffix.Data(), ix, iy);
      TH1D *h_slice = h3->ProjectionZ(slice_name, ix, ix, iy, iy);
      // Smooth low-pt / low-emfrac slices to stabilize fits (value-based: pt < 15 GeV,
      // emfrac < 0.3 / < 0.1 by upper bin edge, independent of the binning arrays)
      if (truthptbins[ix] <= 15.0 && emfracbins[iy] <= 0.3 && do_closure == false) h_slice->Rebin(2);
      if (truthptbins[ix] <= 15.0 && emfracbins[iy] <= 0.1 && do_closure == false) h_slice->Rebin(2);
      if (do_closure) {
        // Fixed rebin x2 for all closure slices; one more x2 for genuinely sparse ones.
        // The sparseness decision uses the effective entries (Sum w)^2 / (Sum w^2):
        // equal to the raw pre-reweighting count for uniform weights, and immune to
        // the z-vertex reweighting and cross-section combination scales (a plain
        // entry/integral count would be distorted by the weights).
        h_slice->Rebin(2);
        if (h_slice->GetEffectiveEntries() < 300) h_slice->Rebin(2);
      }
      h_slice->SetDirectory(0); // Detach from ROOT memory
 
      // Set x-axis display range to [0, 2]
      h_slice->GetXaxis()->SetRangeUser(0.0, 2.0);
 
      // Put bin info in the title so it's clean and doesn't overlap
      h_slice->SetTitle(Form("p_{T} [%.0f, %.0f], E_{EM} [%.2f, %.2f];p_{T}^{reco}/p_{T}^{truth};Counts", truthptbins[ix-1], truthptbins[ix], emfracbins[iy-1], emfracbins[iy]));
      
      slices_this_pt.push_back(h_slice);
 
      float fit_mean = 0, fit_sigma = 0, fit_meanerr = 0, fit_sigmaerr = 0, fit_chi2ndf = 0;
      int index = ix - 1;

      // Pad maps directly to the emfrac bin index (1 to 30)
      if (save_figures) {
        c->cd(iy);
        gPad->SetLeftMargin(0.15);
        gPad->SetBottomMargin(0.15);
      }

      if (h_slice->GetEntries() > 10) {
        //if (iy <= 7) h_slice->Rebin(5);
        //else h_slice->Rebin(3);
        if (do_closure) fit_resogaus_closure(index, h_slice, f_gaus, fit_mean, fit_sigma, fit_meanerr, fit_sigmaerr, fit_chi2ndf);
        else fit_resogaus(index, h_slice, f_gaus, fit_mean, fit_sigma, fit_meanerr, fit_sigmaerr, fit_chi2ndf);
        // Keep the Gaussian result unless the fit is genuinely bad. The chi2 gate is
        // loose (10): chi2/ndf grows with statistics for a fixed small non-Gaussian
        // deviation, so a tight gate demotes visually excellent high-statistics fits
        // to the (tail-biased) raw mean. Parameter-sanity checks catch the actually
        // broken fits instead.
        bool fit_bad = (fit_chi2ndf < 0) || (fit_chi2ndf > 10.0)
                    || (fit_mean < 0.3 || fit_mean > 1.3)
                    || (fit_sigma < 0.01 || fit_sigma > 0.5)
                    || (fit_meanerr <= 0);
        if (fit_bad) {
          fit_mean = h_slice->GetMean();
          fit_sigma = h_slice->GetRMS();
          fit_meanerr = h_slice->GetMeanError();
          fit_sigmaerr = h_slice->GetRMSError();
          fit_chi2ndf = -1;
        }
 
        h2_mean->SetBinContent(ix, iy, fit_mean);
        h2_mean->SetBinError(ix, iy, fit_meanerr);
        h2_sigma->SetBinContent(ix, iy, fit_sigma);
        h2_sigma->SetBinError(ix, iy, fit_sigmaerr);
        h2_chi2->SetBinContent(ix, iy, fit_chi2ndf);

        if (save_figures) {
          h_slice->Draw("HIST");
          f_gaus->DrawCopy("SAME");
        }
      } else {
        if (save_figures) h_slice->Draw("HIST");
      }

      // Draw Fit Results Text
      if (save_figures && fit_mean > 0) {
        TLatex tex;
        tex.SetNDC();
        tex.SetTextSize(0.06);
        tex.DrawLatex(0.18, 0.89, Form("#mu = %.3f #pm %.3f", fit_mean, fit_meanerr));
        tex.DrawLatex(0.18, 0.82, Form("#sigma = %.3f #pm %.3f", fit_sigma, fit_sigmaerr));
        tex.DrawLatex(0.18, 0.75, Form("#chi^{2}/ndf = %.2f", fit_chi2ndf));
      }
    }

    // 3. Save the Canvas for this pt bin (linear y), then re-save with log y
    if (save_figures) {
      c->SaveAs(Form("figure_gaus/fits_r0%d_z%d%s_pt%02d_closure%d.png", r_val, z_val, eta_suffix.Data(), ix, do_closure));

      // --- Log-y version: switch every pad to log scale and re-save ---
      for (int ipad = 1; ipad <= n_cols * n_rows; ++ipad) {
        TVirtualPad *p = c->cd(ipad);
        if (p) {
          p->SetLogy(1);
          TH1 *h = dynamic_cast<TH1*>(p->GetPrimitive(""));
          if (!h) {
            TList *prims = p->GetListOfPrimitives();
            if (prims) {
              for (TObject *obj : *prims) {
                h = dynamic_cast<TH1*>(obj);
                if (h) break;
              }
            }
          }
          if (h) h->SetMinimum(1e-17);
        }
      }
      c->Modified();
      c->Update();
      c->SaveAs(Form("figure_gaus/fits_r0%d_z%d%s_pt%02d_closure%d_logy.png", r_val, z_val, eta_suffix.Data(), ix, do_closure));

      delete c;
    }
    for (auto slice : slices_this_pt) {
      delete slice;
    }
  }
  
  // 4. Write final 2D histograms to output file
  fout->cd();
  h2_mean->Write();
  h2_sigma->Write();
  h2_chi2->Write();
}

// save_figures: per-(pT, EMfrac) fit canvases in figure_gaus/ (off by default; the fitted
// h2_mean / h2_sigma outputs are unaffected).
void Jet_resp_histfitter(int r_val = 4, int z_val = 1, bool do_closure = false, int eta_val = -1, bool save_figures = false) {
  SetAtlasStyle();
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  // ********** Fit histograms ********** //
  std::cout << "Histogram fitting start:" << std::endl;

  if (eta_val >= get_n_etafracbin(r_val)) {
    std::cerr << "Error: eta_val = " << eta_val << " out of range for R=0." << r_val << " (n_etafracbin = " << get_n_etafracbin(r_val) << ")" << std::endl;
    return;
  }

  TFile *fin = TFile::Open("output_combined.root", "READ");
  if (!fin || fin->IsZombie()) {
    std::cerr << "Error opening output_combined.root" << std::endl;
    return;
  }

  TString eta_suffix = (eta_val < 0) ? "" : Form("_eta%d", eta_val);
  gSystem->mkdir("output_gaus", kTRUE);
  TFile *fout = new TFile(Form("output_gaus/output_gaus_r0%d_z%d%s.root", r_val, z_val, eta_suffix.Data()), "RECREATE");

  TF1 *f_gaus = new TF1("f_gaus", "gaus", 0, 2);
  f_gaus->SetLineColor(kRed);

  TString hist_name = Form("h_truthpt_emfrac_ptratio_comb_r0%d_z%d%s", r_val, z_val, eta_suffix.Data());
  TH3D *h3 = (TH3D*)fin->Get(hist_name);
  if (!h3) {
    std::cerr << "Warning: Could not find " << hist_name << " in file." << std::endl;
    return;
  }

  process_and_plot_single_TH3D(h3, r_val, z_val, eta_val, fout, f_gaus, do_closure, save_figures);

  // Barycenter maps for the bin-centering correction (eta-integrated only): the
  // spectrum-weighted mean <truthpt> and <EMfrac> per cell, used by the slicefitter
  // as fit-point positions instead of the geometric bin centers.
  if (eta_val < 0) {
    TH2D *h2_sumw = (TH2D*)fin->Get(Form("h2_sumw_comb_r0%d_z%d", r_val, z_val));
    TH2D *h2_sumwpt = (TH2D*)fin->Get(Form("h2_sumwpt_comb_r0%d_z%d", r_val, z_val));
    TH2D *h2_sumwem = (TH2D*)fin->Get(Form("h2_sumwem_comb_r0%d_z%d", r_val, z_val));
    if (h2_sumw && h2_sumwpt && h2_sumwem) {
      fout->cd();
      TH2D *h2_barypt = (TH2D*)h2_sumw->Clone(Form("h2_barypt_r0%d_z%d", r_val, z_val));
      TH2D *h2_baryem = (TH2D*)h2_sumw->Clone(Form("h2_baryem_r0%d_z%d", r_val, z_val));
      h2_barypt->Reset();
      h2_baryem->Reset();
      for (int ix = 1; ix <= h2_sumw->GetNbinsX(); ++ix) {
        for (int iy = 1; iy <= h2_sumw->GetNbinsY(); ++iy) {
          double sw = h2_sumw->GetBinContent(ix, iy);
          double bx = h2_sumw->GetXaxis()->GetBinCenter(ix);
          double by = h2_sumw->GetYaxis()->GetBinCenter(iy);
          if (sw > 0) {
            bx = h2_sumwpt->GetBinContent(ix, iy) / sw;
            by = h2_sumwem->GetBinContent(ix, iy) / sw;
          }
          h2_barypt->SetBinContent(ix, iy, bx);
          h2_baryem->SetBinContent(ix, iy, by);
        }
      }
      h2_barypt->Write();
      h2_baryem->Write();
    } else {
      std::cout << "Note: no h2_sumw*_comb histograms in output_combined.root; barycenter maps not written (smoothing will use bin centers)." << std::endl;
    }
  }

  fout->Close();
  fin->Close();
  std::cout << "Processing complete. Output saved to output_gaus/ and figure_gaus/" << std::endl;
}