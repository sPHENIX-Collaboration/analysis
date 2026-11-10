#include "AtlasStyle.C"
#include "AtlasUtils.C"
#include "Jet_resp_Def.h"
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TLine.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TString.h>
#include <TMath.h>
#include <TMatrixD.h>
#include <TVectorD.h>
#include <algorithm>
#include <iostream>
#include <vector>

// ============================================================================
// Jet_resp_zeta_interpolation.C
//
// Builds the (z-vertex x eta) residual correction on top of the first-pass
// (pT x EMfrac) JES calibration, from the closure-test Gaussian fits.
//
// Workflow (SIGNED z bins from -200 to +200 cm; no mirror folding):
//   Step 1: For each (z, eta-fraction) bin, take h2_mean (pT x EMfrac Gaussian
//           means) from output_gaus_r0R_zZ_etaE.root, build the distribution of
//           its bin contents and fit it with a Gaussian -> mean mu(z, eta).
//           The distributions are rebinned more coarsely at large |z| (extra
//           factor 2 above 100 cm, 4 above 150 cm) where they are broader.
//           Quality gates reject bad fits (fallback: raw distribution mean).
//           The no-zvertex bin (last z index) is processed here too, but kept
//           out of the 2D map: its means go to a separate 1D histogram
//           h_zeta_mean_noz_r0R with its own symmetric-quadratic fit.
//   Step 2: The ONLY smoothing, along z: each f column is fit versus the SIGNED z
//           directly on the Step-1 means (no f smoothing), with
//           Z(z) = p0 + p1*(z/100) + p2*(z/100)^2 + p3*(z/100)^4 (analytic solve).
//           The quartic captures the flat-then-falling shape; the linear term a
//           mild +-z asymmetry. Points sit at their <z> barycenters; raw-mean
//           fallback points are excluded (included only if a column would drop
//           below the minimum point count).
//   Step 3: The smooth surface (per-column z fits, linearly interpolated in f
//           between the column centers) is evaluated on a fine (z, f) grid,
//           clamped, and inverted -> scale-factor map. The residual correction is
//           inverted -> scale-factor map, which is then strictly limited to
//           [0.9, 1.1] (a z fit extrapolating into a thin-statistics region must
//           not produce a large correction). The limit is applied AFTER inversion,
//           to the applied correction only: the smoothed-response maps keep the
//           fitted values and the no-vertex function is NOT limited.
//
// Outputs:
//   zetadependence_r0R.root      : diagnostics (raw means, no-z 1D histogram,
//                                  graphs, fits, maps)
//   JES_Calibration_r0R.root     : h2d_jes_calib_r0R (first pass, from
//                                  calib_r0R.root) + h2_zeta_corr_r0R
//                                  (fine-binned scale factor vs (SIGNED z, f))
//
// Application convention (must match get_etafrac_bin in Jet_resp_histmaker.C):
//   f = (eta - eta_min(z)) / (eta_max(z) - eta_min(z)), acceptance limits from
//   get_eta_acceptance with the SIGNED z; NO mirror. Scale factor =
//   h2_zeta_corr->GetBinContent / Interpolate at (z, f), with z clamped to the
//   histogram range; the stored map factor is always within [0.9, 1.1], while the
//   no-vertex function is used as fitted. Events without a z-vertex use the symmetric-quadratic fit
//   instead: scale = f_zeta_noz_corr_r0R(f), with f computed at z = 0.
// ============================================================================

// ---------- Step-1 helper: Gaussian mean of the h2_mean bin-content distribution ----------
// extra_rebin: additional rebin factor on top of the base Rebin(2), for the broader
// distributions at large |z| (1 = none, 2 for bins starting at |z| >= 100 cm,
// 4 for bins starting at |z| >= 150 cm).
// Returns: 0 = good fit, 1 = fallback to raw mean, -1 = rejected.
int get_zeta_gausmean(TH2D* h2, TH2D* h2_chi2, TH1D* h1, TF1* f, int extra_rebin, double &mean, double &meanerr, double &sigma) {
  for (int ix = 1; ix <= h2->GetNbinsX(); ++ix) {
    for (int iy = 1; iy <= h2->GetNbinsY(); ++iy) {
      double v = h2->GetBinContent(ix, iy);
      if (v == 0) continue; // skip empty / unfit (pT, EMfrac) cells
      if (h2_chi2 && h2_chi2->GetBinContent(ix, iy) < 0) continue; // skip failed-fit cells (raw mean/RMS fallback)
      h1->Fill(v);
    }
  }
  if (h1->GetEntries() < 8) return -1; // too few filled cells to be meaningful

  double seed_mean = h1->GetMean();
  double seed_rms  = (h1->GetRMS() > 0) ? h1->GetRMS() : 0.05;
  f->SetParameters(h1->GetMaximum(), seed_mean, seed_rms);
  h1->Rebin(2 * extra_rebin);
  double half_width = TMath::Max(0.05, 1.5 * seed_rms); // fit window: mean +/- 1.5 sigma, floored at +/- 0.05
  double fit_lo = TMath::Max(0.7, seed_mean - half_width);
  double fit_hi = TMath::Min(1.3, seed_mean + half_width);
  h1->Fit(f, "Q R", "", fit_lo, fit_hi);

  mean    = f->GetParameter(1);
  meanerr = f->GetParError(1);
  sigma   = f->GetParameter(2);
  double chi2ndf = (f->GetNDF() > 0) ? f->GetChisquare() / (double)f->GetNDF() : -1;

  // Quality gates on the Gaussian fit (loose enough for the lower means / larger spread at large |z|)
  bool good_fit = (TMath::Abs(mean - 1.0) < 0.2) &&
                  (sigma > 0 && sigma < 0.15) &&
                  (meanerr > 0 && meanerr < 0.02) &&
                  (chi2ndf < 10);
  if (good_fit) return 0;

  // Fallback: raw mean of the bin-content distribution
  mean    = h1->GetMean();
  meanerr = h1->GetMeanError();
  sigma   = h1->GetRMS();
  if (TMath::Abs(mean - 1.0) < 0.2 && meanerr > 0 && meanerr < 0.03) return 1;
  return -1;
}

// ---------- Outlier pruning: iteratively drop the WORST point while it is > nsigma away ----------
// Pulls use the point errors scaled by sqrt(chi2/ndf) of the current fit (capped at >= 1):
// the statistical errors of the means are tiny compared to the accuracy of the smooth
// model, so unscaled pulls would flag ordinary model imperfection as outliers. Only the
// single worst point is removed per iteration (order-independent), then the fit repeats.
int prune_and_refit(TGraphErrors* g, TF1* f, double nsigma, int min_points, double fit_lo, double fit_hi) {
  int n_removed = 0;
  while (g->GetN() > min_points) {
    double err_scale = 1.0;
    if (f->GetNDF() > 0 && f->GetChisquare() / f->GetNDF() > 1.0) {
      err_scale = TMath::Sqrt(f->GetChisquare() / f->GetNDF());
    }
    int ip_worst = -1;
    double pull_worst = 0;
    for (int ip = 0; ip < g->GetN(); ++ip) {
      double x, y;
      g->GetPoint(ip, x, y);
      double ey = g->GetErrorY(ip);
      if (ey <= 0) continue;
      double pull = TMath::Abs(y - f->Eval(x)) / (ey * err_scale);
      if (pull > pull_worst) { pull_worst = pull; ip_worst = ip; }
    }
    if (ip_worst < 0 || pull_worst <= nsigma) break;
    g->RemovePoint(ip_worst);
    ++n_removed;
    g->Fit(f, "Q R", "", fit_lo, fit_hi);
  }
  return n_removed;
}

// ============================================================================
void Jet_resp_zeta_interpolation(int r_val = 4) {
  SetAtlasStyle();
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  const char* figdir = "figure_dependence";
  gSystem->mkdir(figdir, kTRUE);

  // Hard limits on the residual (z, f) correction actually applied to jets, i.e. the
  // inverted map h2_zeta_corr only. A correction larger than this cannot be a real
  // effect: it means the z fit is extrapolating into a region without the statistics to
  // constrain it. The smoothed-response maps keep the fitted values (so a runaway fit
  // stays visible in the diagnostics), and the no-vertex function is NOT limited.
  const double zeta_corr_min = 0.9;
  const double zeta_corr_max = 1.1;

  const int n_eta = get_n_etafracbin(r_val);
  int eta_color[n_etafracbin_max] = {kBlack, kBlue+1, kGreen+2, kRed+1, kOrange+7, kMagenta+1};

  // Physical SIGNED z bins used for the correction (|z| > 200 cm events are excluded
  // upstream); the no-zvertex bin (last z index) is processed separately (1D only).
  const int n_zphys = n_zvertex_phys;    // signed z bins, edges from Jet_resp_Def.h
  const int n_zall = n_zphys + 1;        // last slot = no-zvertex
  const int i_noz = n_zphys;             // storage index of the no-z slot
  double *z_edge = zvertex_edges;

  double mu[n_zall][n_etafracbin_max];
  double mu_err[n_zall][n_etafracbin_max];
  double mu_sig[n_zall][n_etafracbin_max];    // Gaussian sigma (RMS for the raw-mean fallback)
  int    mu_status[n_zall][n_etafracbin_max]; // 0 good fit, 1 fallback, -1 rejected
  TH1D  *h_dist[n_zall][n_etafracbin_max];
  TF1   *f_dist[n_zall][n_etafracbin_max];

  // ---------------- Barycenters (bin-centering correction) ----------------
  // Spectrum-weighted mean position <z> and <f> per (z, f) bin (and <f> per f bin for
  // the no-vertex events), from output_combined.root. The Step-2/Step-3/no-z fits
  // place each point at its barycenter and the fitted curves are evaluated at the bin
  // centers or continuously (z fits -> fine z grid), removing
  // the bias from reading a bin average as the value at the bin center.
  double zbar[n_zvertex_phys][n_etafracbin_max];
  double fbar[n_zvertex_phys][n_etafracbin_max];
  double fbar_noz[n_etafracbin_max];
  for (int iz = 0; iz < n_zphys; ++iz) {
    for (int ie = 0; ie < n_eta; ++ie) {
      zbar[iz][ie] = 0.5 * (z_edge[iz] + z_edge[iz+1]);
      fbar[iz][ie] = (ie + 0.5) / (double)n_eta;
    }
  }
  for (int ie = 0; ie < n_eta; ++ie) fbar_noz[ie] = (ie + 0.5) / (double)n_eta;
  {
    TFile *f_comb = TFile::Open("output_combined.root", "READ");
    TH2D *h_zfw = nullptr, *h_zfz = nullptr, *h_zff = nullptr;
    TH1D *h_nozw = nullptr, *h_nozf = nullptr;
    if (f_comb && !f_comb->IsZombie()) {
      h_zfw = (TH2D*)f_comb->Get(Form("h2_zf_sumw_comb_r0%d", r_val));
      h_zfz = (TH2D*)f_comb->Get(Form("h2_zf_sumwz_comb_r0%d", r_val));
      h_zff = (TH2D*)f_comb->Get(Form("h2_zf_sumwf_comb_r0%d", r_val));
      h_nozw = (TH1D*)f_comb->Get(Form("h_noz_sumw_comb_r0%d", r_val));
      h_nozf = (TH1D*)f_comb->Get(Form("h_noz_sumwf_comb_r0%d", r_val));
    }
    if (h_zfw && h_zfz && h_zff) {
      for (int iz = 0; iz < n_zphys; ++iz) {
        for (int ie = 0; ie < n_eta; ++ie) {
          double sw = h_zfw->GetBinContent(iz+1, ie+1);
          if (sw <= 0) continue;
          double bz = h_zfz->GetBinContent(iz+1, ie+1) / sw;
          double bf = h_zff->GetBinContent(iz+1, ie+1) / sw;
          if (bz > z_edge[iz] && bz < z_edge[iz+1]) zbar[iz][ie] = bz;
          if (bf > (double)ie/n_eta && bf < (double)(ie+1)/n_eta) fbar[iz][ie] = bf;
        }
      }
    } else {
      std::cout << "Note: (z, f) barycenter maps not found in output_combined.root; fits use bin centers." << std::endl;
    }
    if (h_nozw && h_nozf) {
      for (int ie = 0; ie < n_eta; ++ie) {
        double sw = h_nozw->GetBinContent(ie+1);
        if (sw <= 0) continue;
        double bf = h_nozf->GetBinContent(ie+1) / sw;
        if (bf > (double)ie/n_eta && bf < (double)(ie+1)/n_eta) fbar_noz[ie] = bf;
      }
    }
    if (f_comb) { f_comb->Close(); delete f_comb; }
  }

  TFile *f_diag = new TFile(Form("zetadependence_r0%d.root", r_val), "RECREATE");

  // Raw measured means (physical z bins only), for the record
  TH2D *h2_raw = new TH2D(Form("h2_zeta_mean_raw_r0%d", r_val),
                          ";z_{vertex} [cm];#eta fraction f;<p_{T}^{calib} / p_{T}^{truth}>",
                          n_zphys, z_edge, n_eta, 0.0, 1.0);

  // ---------------- Step 1: Gaussian means per (z, eta) ----------------
  std::cout << "Step 1: Gaussian means per (z, eta) bin" << std::endl;
  bool warned_nochi2 = false;
  for (int izz = 0; izz < n_zall; ++izz) {
    int file_z = (izz < n_zphys) ? izz : (n_zvertex - 1); // no-z histograms carry the last z index
    for (int ie = 0; ie < n_etafracbin_max; ++ie) {
      h_dist[izz][ie] = nullptr;
      f_dist[izz][ie] = nullptr;
    }

    for (int ie = 0; ie < n_eta; ++ie) {
      mu[izz][ie] = 0;
      mu_err[izz][ie] = 0;
      mu_sig[izz][ie] = 0;
      mu_status[izz][ie] = -1;

      TString fname = Form("output_gaus/output_gaus_r0%d_z%d_eta%d.root", r_val, file_z, ie);
      TFile *fin = TFile::Open(fname, "READ");
      if (!fin || fin->IsZombie()) {
        std::cerr << "  Cannot open " << fname << std::endl;
        continue;
      }
      TH2D *h2 = (TH2D*) fin->Get(Form("h2_mean_r0%d_z%d_eta%d", r_val, file_z, ie));
      if (!h2) {
        std::cerr << "  Missing h2_mean in " << fname << std::endl;
        fin->Close();
        delete fin;
        continue;
      }
      TH2D *h2c = (TH2D*) fin->Get(Form("h2_chi2_r0%d_z%d_eta%d", r_val, file_z, ie));
      if (!h2c && !warned_nochi2) {
        std::cout << "  Note: h2_chi2 maps not found; failed-fit cells are not excluded from the distributions." << std::endl;
        warned_nochi2 = true;
      }

      TH1D *h1 = new TH1D(Form("h_bincontent_r0%d_z%d_eta%d", r_val, file_z, ie),
                          ";Bin content (Gaussian mean);Number of bins", 120, 0.7, 1.3);
      h1->SetDirectory(0);
      TF1 *fg = new TF1(Form("f_gaus_z%d_eta%d", file_z, ie), "gaus", 0.7, 1.3);

      int extra_rebin = 1;
      if (izz < n_zphys) {
        double az_low = TMath::Min(TMath::Abs(z_edge[izz]), TMath::Abs(z_edge[izz+1]));
        if (az_low >= 150) extra_rebin = 4;
        else if (az_low >= 100) extra_rebin = 2;
      }
      double m, me, s;
      int status = get_zeta_gausmean(h2, h2c, h1, fg, extra_rebin, m, me, s);
      mu_status[izz][ie] = status;
      if (status >= 0) {
        mu[izz][ie] = m;
        mu_err[izz][ie] = me;
        mu_sig[izz][ie] = s;
        if (izz < n_zphys) {
          h2_raw->SetBinContent(izz+1, ie+1, m);
          h2_raw->SetBinError(izz+1, ie+1, me);
        }
      }
      h_dist[izz][ie] = h1;
      f_dist[izz][ie] = fg;

      fin->Close();
      delete fin;

      std::cout << "  z" << file_z << " eta" << ie << "  status=" << status
                << "  mu=" << mu[izz][ie] << " +/- " << mu_err[izz][ie]
                << "  sigma=" << mu_sig[izz][ie] << std::endl;
    }
  }

  // ---------------- Step-1 check canvas: one pad per (z, eta) bin ----------------
  // Grid of n_zphys rows x n_eta columns; each pad shows one bin-content distribution
  // with its own Gaussian fit (solid red: passed the gates; dashed red: fit shown but
  // the gates failed, so the raw mean or rejection applied).
  {
    TCanvas *c_grid = new TCanvas("c_zeta_gauscheck", "", n_eta*300, n_zphys*250);
    c_grid->Divide(n_eta, n_zphys, 0.001, 0.001);
    for (int iz = 0; iz < n_zphys; ++iz) {
      for (int ie = 0; ie < n_eta; ++ie) {
        c_grid->cd(iz*n_eta + ie + 1);
        gPad->SetLeftMargin(0.15);
        gPad->SetBottomMargin(0.13);
        TH1D *h1 = h_dist[iz][ie];
        if (!h1) continue;
        h1->SetLineColor(kBlack);
        h1->SetLineWidth(2);
        h1->SetTitle("");
        h1->Draw("HIST");
        if (f_dist[iz][ie] && h1->GetEntries() >= 8) {
          f_dist[iz][ie]->SetLineColor(kRed);
          f_dist[iz][ie]->SetLineWidth(2);
          f_dist[iz][ie]->SetLineStyle((mu_status[iz][ie] == 0) ? 1 : 2);
          f_dist[iz][ie]->Draw("same");
        }
        TLatex tex;
        tex.SetNDC();
        tex.SetTextSize(0.07);
        tex.DrawLatex(0.17, 0.88, z_label[iz]);
        tex.DrawLatex(0.17, 0.79, Form("f: %.2f - %.2f", (double)ie/n_eta, (double)(ie+1)/n_eta));
        if (mu_status[iz][ie] >= 0) {
          tex.DrawLatex(0.17, 0.70, Form("#mu = %.4f%s", mu[iz][ie], (mu_status[iz][ie] == 1) ? " (raw)" : ""));
          tex.DrawLatex(0.17, 0.61, Form("#sigma = %.4f", mu_sig[iz][ie]));
        } else {
          tex.DrawLatex(0.17, 0.70, "rejected");
        }
      }
    }
    c_grid->SaveAs(Form("%s/zeta_gauscheck_r0%d.png", figdir, r_val));
    delete c_grid;
  }

  // No-vertex bin fits: one row
  {
    TCanvas *c_noz_fits = new TCanvas("c_zeta_gauscheck_noz", "", n_eta*300, 280);
    c_noz_fits->Divide(n_eta, 1);
    for (int ie = 0; ie < n_eta; ++ie) {
      c_noz_fits->cd(ie+1);
      gPad->SetLeftMargin(0.15);
      gPad->SetBottomMargin(0.13);
      TH1D *h1 = h_dist[i_noz][ie];
      if (!h1) continue;
      h1->SetLineColor(kBlack);
      h1->SetLineWidth(2);
      h1->SetTitle("");
      h1->Draw("HIST");
      if (f_dist[i_noz][ie] && h1->GetEntries() >= 8) {
        f_dist[i_noz][ie]->SetLineColor(kRed);
        f_dist[i_noz][ie]->SetLineWidth(2);
        f_dist[i_noz][ie]->SetLineStyle((mu_status[i_noz][ie] == 0) ? 1 : 2);
        f_dist[i_noz][ie]->Draw("same");
      }
      TLatex tex;
      tex.SetNDC();
      tex.SetTextSize(0.07);
      tex.DrawLatex(0.17, 0.88, "No zvertex");
      tex.DrawLatex(0.17, 0.79, Form("f: %.2f - %.2f", (double)ie/n_eta, (double)(ie+1)/n_eta));
      if (mu_status[i_noz][ie] >= 0) {
        tex.DrawLatex(0.17, 0.70, Form("#mu = %.4f%s", mu[i_noz][ie], (mu_status[i_noz][ie] == 1) ? " (raw)" : ""));
        tex.DrawLatex(0.17, 0.61, Form("#sigma = %.4f", mu_sig[i_noz][ie]));
      } else {
        tex.DrawLatex(0.17, 0.70, "rejected");
      }
    }
    c_noz_fits->SaveAs(Form("%s/zeta_gauscheck_noz_r0%d.png", figdir, r_val));
    delete c_noz_fits;
  }

  // ---------------- Raw measured (z, f) map canvas ----------------
  {
    TCanvas *c_raw = new TCanvas("c_zeta_raw", "", 1000, 700);
    c_raw->SetLeftMargin(0.10);
    c_raw->SetRightMargin(0.15);
    c_raw->SetTopMargin(0.08);
    c_raw->SetBottomMargin(0.12);
    gStyle->SetPaintTextFormat("4.3f");
    h2_raw->GetZaxis()->SetRangeUser(0.9, 1.1);
    h2_raw->Draw("COLZ TEXT E");
    TLatex tex_raw;
    tex_raw.SetNDC();
    tex_raw.SetTextFont(42);
    tex_raw.SetTextSize(0.04);
    tex_raw.DrawLatex(0.12, 0.94, Form("Anti-k_{T} R=0.%d, measured <p_{T}^{calib}/p_{T}^{truth}> vs (z, f)", r_val));
    c_raw->SaveAs(Form("%s/zeta_rawmap_r0%d.png", figdir, r_val));
    delete c_raw;
  }

  // ---------------- No-zvertex: 1D distribution of eta-bin means (kept out of the 2D map) ----------------
  TH1D *h_noz = new TH1D(Form("h_zeta_mean_noz_r0%d", r_val),
                         "No zvertex;#eta fraction f;<p_{T}^{calib} / p_{T}^{truth}>",
                         n_eta, 0.0, 1.0);
  for (int ie = 0; ie < n_eta; ++ie) {
    if (mu_status[i_noz][ie] < 0) continue;
    h_noz->SetBinContent(ie+1, mu[i_noz][ie]);
    h_noz->SetBinError(ie+1, mu_err[i_noz][ie]);
  }
  // Symmetric quadratic fit of the no-vertex means (middle high, edges low, symmetric
  // about f = 0.5 by construction). This TF1 is the no-vertex residual correction:
  // scale factor = 1 / f_zeta_noz(f), with f computed at z = 0 (no mirror).
  // Only good Gaussian points (status 0) enter the fit; raw-mean fallback points
  // (status 1) are excluded and shown as open markers.
  std::vector<double> nzx, nzxe, nzy, nzye;   // fit points
  std::vector<double> nrx, nrxe, nry, nrye;   // excluded fallback points (display only)
  for (int ie = 0; ie < n_eta; ++ie) {
    if (mu_status[i_noz][ie] == 0) {
      nzx.push_back(fbar_noz[ie]); // barycenter <f> (bin-centering correction)
      nzxe.push_back(0.5 / (double)n_eta);
      nzy.push_back(mu[i_noz][ie]);
      nzye.push_back(mu_err[i_noz][ie]);
    } else if (mu_status[i_noz][ie] == 1) {
      nrx.push_back(fbar_noz[ie]);
      nrxe.push_back(0.5 / (double)n_eta);
      nry.push_back(mu[i_noz][ie]);
      nrye.push_back(mu_err[i_noz][ie]);
    }
  }
  TGraphErrors *g_noz = new TGraphErrors(nzx.size(), nzx.data(), nzy.data(), nzxe.data(), nzye.data());
  g_noz->SetName(Form("g_zeta_noz_r0%d", r_val));
  TGraphErrors *g_noz_excl = nrx.empty() ? nullptr
      : new TGraphErrors(nrx.size(), nrx.data(), nry.data(), nrxe.data(), nrye.data());
  TF1 *f_noz = new TF1(Form("f_zeta_noz_r0%d", r_val), "[0] + [1]*(x - 0.5)*(x - 0.5)", 0, 1);
  f_noz->SetParameters(1.0, -0.1);
  if (g_noz->GetN() >= 3) g_noz->Fit(f_noz, "Q R", "", 0, 1);
  else if (h_noz->GetEntries() > 0) {
    std::cout << "  Warning: fewer than 3 good no-vertex points; fitting all points incl. fallbacks." << std::endl;
    h_noz->Fit(f_noz, "Q R", "", 0, 1);
  }
  std::cout << "No-vertex fit: mu_noz(f) = " << f_noz->GetParameter(0)
            << " + " << f_noz->GetParameter(1) << "*(f - 0.5)^2" << std::endl;
  // Direct no-vertex calibration function: the reciprocal of the fit (scale factor vs f)
  // No limit here on purpose: the no-vertex correction is used exactly as fitted.
  TF1 *f_noz_corr = new TF1(Form("f_zeta_noz_corr_r0%d", r_val), "1.0 / ([0] + [1]*(x - 0.5)*(x - 0.5))", 0, 1);
  f_noz_corr->SetParameters(f_noz->GetParameter(0), f_noz->GetParameter(1));
  {
    // Range over 0 <= f <= 1, for reference only (not limited)
    double raw_lo = 1e9, raw_hi = -1e9;
    for (int i = 0; i <= 100; ++i) {
      double f_i = i / 100.0;
      double v = 1.0 / f_noz->Eval(f_i);
      raw_lo = std::min(raw_lo, v);
      raw_hi = std::max(raw_hi, v);
    }
    std::cout << "No-vertex correction range: [" << raw_lo << ", " << raw_hi << "] (not limited)" << std::endl;
  }
  {
    TCanvas *c_noz = new TCanvas("c_zeta_noz", "", 900, 650);
    c_noz->SetLeftMargin(0.12);
    c_noz->SetRightMargin(0.05);
    c_noz->SetTopMargin(0.08);
    c_noz->SetBottomMargin(0.13);
    h_noz->SetMarkerStyle(20);
    h_noz->SetLineColor(kBlack);
    h_noz->SetMarkerColor(kBlack);
    h_noz->GetYaxis()->SetRangeUser(0.8, 1.1);
    h_noz->Draw("AXIS");
    g_noz->SetMarkerStyle(20);
    g_noz->SetMarkerColor(kBlack);
    g_noz->SetLineColor(kBlack);
    g_noz->Draw("PE same");
    if (g_noz_excl) { // open markers: fallback points excluded from the fit
      g_noz_excl->SetMarkerStyle(24);
      g_noz_excl->SetMarkerColor(kGray+2);
      g_noz_excl->SetLineColor(kGray+2);
      g_noz_excl->Draw("PE same");
    }
    f_noz->SetLineColor(kRed);
    f_noz->Draw("same");
    TLine *l_one = new TLine(0, 1.0, 1.0, 1.0);
    l_one->SetLineColor(kGray+2);
    l_one->SetLineStyle(2);
    l_one->Draw("same");
    TLatex tex;
    tex.SetNDC();
    tex.SetTextFont(42);
    tex.SetTextSize(0.04);
    tex.DrawLatex(0.15, 0.85, Form("Anti-k_{T} R=0.%d, no z-vertex", r_val));
    tex.DrawLatex(0.15, 0.80, Form("#mu(f) = %.4f %+.4f (f - 0.5)^{2}", f_noz->GetParameter(0), f_noz->GetParameter(1)));
    c_noz->SaveAs(Form("%s/zeta_noz_r0%d.png", figdir, r_val));
    delete c_noz;
    delete l_one;
  }

  // ---------------- Eta-dependence check canvas (no f smoothing) ----------------
  // The f dependence is NOT smoothed: the z-fit smoothing (Step 2) acts directly on
  // the Step-1 means, per f column. This canvas displays the Step-1 means versus f
  // for each signed z bin (filled: good Gaussian fits; open gray: raw-mean fallback
  // points, which are excluded from the z fits).
  {
    TCanvas *c_eta = new TCanvas("c_zeta_etacheck", "", 1600, 1200);
    c_eta->Divide(4, (n_zphys + 3) / 4);
    for (int iz = 0; iz < n_zphys; ++iz) {
      c_eta->cd(iz+1);
      gPad->SetLeftMargin(0.15);
      gPad->SetBottomMargin(0.15);
      TH1D *frame = new TH1D(Form("frame_eta_r0%d_z%d", r_val, iz), ";#eta fraction f;<p_{T}^{calib} / p_{T}^{truth}>", 1, 0, 1);
      frame->SetDirectory(0);
      frame->GetYaxis()->SetRangeUser(0.85, 1.1);
      frame->Draw("AXIS");
      std::vector<double> gx, gxe, gy, gye;
      std::vector<double> rx, rxe, ry, rye;
      for (int ie = 0; ie < n_eta; ++ie) {
        if (mu_status[iz][ie] < 0) continue;
        if (mu_status[iz][ie] == 1) {
          rx.push_back(fbar[iz][ie]);
          rxe.push_back(0.5 / (double)n_eta);
          ry.push_back(mu[iz][ie]);
          rye.push_back(mu_err[iz][ie]);
        } else {
          gx.push_back(fbar[iz][ie]);
          gxe.push_back(0.5 / (double)n_eta);
          gy.push_back(mu[iz][ie]);
          gye.push_back(mu_err[iz][ie]);
        }
      }
      if (!gx.empty()) {
        TGraphErrors *g = new TGraphErrors(gx.size(), gx.data(), gy.data(), gxe.data(), gye.data());
        g->SetName(Form("g_etadependence_r0%d_z%d", r_val, iz));
        g->SetMarkerStyle(20);
        g->SetMarkerColor(kBlack);
        g->Draw("PE same");
      }
      if (!rx.empty()) {
        TGraphErrors *gr = new TGraphErrors(rx.size(), rx.data(), ry.data(), rxe.data(), rye.data());
        gr->SetMarkerStyle(24);
        gr->SetMarkerColor(kGray+2);
        gr->SetLineColor(kGray+2);
        gr->Draw("PE same");
      }
      TLatex tex;
      tex.SetNDC();
      tex.SetTextSize(0.055);
      tex.DrawLatex(0.18, 0.88, z_label[iz]);
      if (iz == 0) tex.DrawLatex(0.18, 0.18, "open: raw-mean fallback (not fit)");
    }
    c_eta->SaveAs(Form("%s/zeta_etacheck_r0%d.png", figdir, r_val));
    delete c_eta;
  }

  // ---------------- Step 2: z-dependence fit per f column (the only smoothing) ----------------
  // Each f column is fit versus the SIGNED z directly on the Step-1 means (no f
  // smoothing), each point placed at its <z> barycenter. Raw-mean fallback points
  // (status 1) are excluded, unless the column would drop below the minimum point
  // count, in which case they are included and the column is flagged.
  std::cout << "Step 2: z-dependence fits per f column" << std::endl;
  std::vector<TGraphErrors*> g_zcol(n_eta, nullptr);
  std::vector<TGraphErrors*> g_zcol_excl(n_eta, nullptr); // excluded fallback points (display only)
  std::vector<TF1*> f_zfit(n_eta, nullptr);
  bool col_fallback[n_etafracbin_max];
  bool col_valid_all = true;
  for (int ie = 0; ie < n_eta; ++ie) {
    col_fallback[ie] = false;
    std::vector<double> zx, zxe, zy, zye;
    std::vector<double> rx, rxe, ry, rye;
    for (int iz = 0; iz < n_zphys; ++iz) {
      if (mu_status[iz][ie] < 0) continue;
      if (mu_status[iz][ie] == 1) { // raw-mean fallback: excluded from the fit
        rx.push_back(zbar[iz][ie]);
        rxe.push_back(0.5*(z_edge[iz+1] - z_edge[iz]));
        ry.push_back(mu[iz][ie]);
        rye.push_back(mu_err[iz][ie]);
        continue;
      }
      zx.push_back(zbar[iz][ie]); // barycenter <z> (bin-centering correction)
      zxe.push_back(0.5*(z_edge[iz+1] - z_edge[iz]));
      zy.push_back(mu[iz][ie]);
      zye.push_back(mu_err[iz][ie]);
    }
    // Fewer than 6 good points: fall back to fitting with the raw-mean points
    // included, so the column stays constrained; flagged on the check canvas.
    if ((int)zx.size() < 6 && !rx.empty()) {
      std::cout << "  f column " << ie << ": only " << zx.size()
                << " good z points, fitting with " << rx.size()
                << " raw-mean points included" << std::endl;
      zx.insert(zx.end(), rx.begin(), rx.end());
      zxe.insert(zxe.end(), rxe.begin(), rxe.end());
      zy.insert(zy.end(), ry.begin(), ry.end());
      zye.insert(zye.end(), rye.begin(), rye.end());
      rx.clear(); rxe.clear(); ry.clear(); rye.clear();
      col_fallback[ie] = true;
    }
    if (!rx.empty()) {
      g_zcol_excl[ie] = new TGraphErrors(rx.size(), rx.data(), ry.data(), rxe.data(), rye.data());
      g_zcol_excl[ie]->SetName(Form("g_zdependence_excl_r0%d_eta%d", r_val, ie));
    }
    if ((int)zx.size() < 6) { // out of 11 z bins; the 4-parameter z fit needs solid constraint
      std::cerr << "Error: f column " << ie << " has only " << zx.size() << " valid z points." << std::endl;
      col_valid_all = false;
      continue;
    }
    TGraphErrors *g = new TGraphErrors(zx.size(), zx.data(), zy.data(), zxe.data(), zye.data());
    g->SetName(Form("g_zdependence_r0%d_eta%d", r_val, ie));
    g->SetTitle(Form("f: %.2f - %.2f;z_{vertex} [cm];<p_{T}^{calib} / p_{T}^{truth}>", (double)ie/n_eta, (double)(ie+1)/n_eta));
    // Quadratic + quartic in the signed z, with a linear term for mild +-z asymmetry:
    //   Z(z) = a + b*(z/100) + c*(z/100)^2 + d*(z/100)^4.
    // The quartic captures the observed flat-then-falling shape (slow decrease out to
    // ~100 cm, faster drop toward +-200 cm), which a plain parabola cannot describe.
    // The coefficients are obtained by an ANALYTIC weighted least-squares solve
    // (normal equations) - no minimizer is involved, so the fit cannot fail; the
    // resulting curve is exactly the optimal polynomial for the given points/weights.
    TF1 *fz = new TF1(Form("f_zfit_r0%d_eta%d", r_val, ie), "[0] + [1]*(x/100.0) + [2]*TMath::Power(x/100.0, 2) + [3]*TMath::Power(x/100.0, 4)", -200, 200);
    {
      const int NP = 4;
      TMatrixD M(NP, NP);
      TVectorD V(NP);
      for (int ip = 0; ip < g->GetN(); ++ip) {
        double x, y;
        g->GetPoint(ip, x, y);
        double ey = g->GetErrorY(ip);
        double w = (ey > 0) ? 1.0/(ey*ey) : 0.0;
        if (w <= 0) continue;
        double u = x/100.0;
        double basis[NP] = {1.0, u, u*u, u*u*u*u};
        for (int a = 0; a < NP; ++a) {
          V[a] += w * basis[a] * y;
          for (int b = 0; b < NP; ++b) M[a][b] += w * basis[a] * basis[b];
        }
      }
      M.Invert();
      TVectorD P = M * V;
      fz->SetParameters(P[0], P[1], P[2], P[3]);
      // chi2 bookkeeping for the printout
      double chi2 = 0;
      int npt = 0;
      for (int ip = 0; ip < g->GetN(); ++ip) {
        double x, y;
        g->GetPoint(ip, x, y);
        double ey = g->GetErrorY(ip);
        if (ey <= 0) continue;
        double r = (y - fz->Eval(x)) / ey;
        chi2 += r*r;
        ++npt;
      }
      fz->SetChisquare(chi2);
      fz->SetNDF(TMath::Max(0, npt - NP));
    }
    g_zcol[ie] = g;
    f_zfit[ie] = fz;
    std::cout << "  f column " << ie << "  Z(z) = " << fz->GetParameter(0)
              << " + " << fz->GetParameter(1) << "*(z/100)"
              << " + " << fz->GetParameter(2) << "*(z/100)^2"
              << " + " << fz->GetParameter(3) << "*(z/100)^4"
              << "  (chi2/ndf = " << ((fz->GetNDF() > 0) ? fz->GetChisquare()/fz->GetNDF() : -1) << ")" << std::endl;
  }
  if (!col_valid_all) {
    std::cerr << "Error: incomplete f columns, cannot build the correction." << std::endl;
    f_diag->cd();
    h2_raw->Write();
    h_noz->Write();
    f_diag->Close();
    return;
  }

  // Check canvas: per-column z fits
  // (filled black: Step-1 means used in the fit; open gray: excluded fallback points; red: fit)
  {
    int n_cols_c = 3;
    int n_rows_c = (n_eta + n_cols_c - 1) / n_cols_c;
    TCanvas *c_z = new TCanvas("c_zeta_zfits", "", n_cols_c*500, n_rows_c*400);
    c_z->Divide(n_cols_c, n_rows_c);
    for (int ie = 0; ie < n_eta; ++ie) {
      c_z->cd(ie+1);
      gPad->SetLeftMargin(0.13);
      gPad->SetBottomMargin(0.13);
      if (!g_zcol[ie]) continue;
      g_zcol[ie]->SetMarkerStyle(20);
      g_zcol[ie]->SetMarkerColor(kBlack);
      g_zcol[ie]->GetXaxis()->SetLimits(-200, 200); // fixed x range
      g_zcol[ie]->GetYaxis()->SetRangeUser(0.85, 1.1);
      g_zcol[ie]->Draw("APE");
      if (g_zcol_excl[ie]) { // open markers: fallback points excluded from the fit
        g_zcol_excl[ie]->SetMarkerStyle(24);
        g_zcol_excl[ie]->SetMarkerColor(kGray+2);
        g_zcol_excl[ie]->SetLineColor(kGray+2);
        g_zcol_excl[ie]->Draw("PE same");
      }
      f_zfit[ie]->SetLineColor(kRed);
      f_zfit[ie]->Draw("same");
      TLatex tex;
      tex.SetNDC();
      tex.SetTextSize(0.05);
      tex.DrawLatex(0.16, 0.88, Form("f: %.2f - %.2f", (double)ie/n_eta, (double)(ie+1)/n_eta));
      if (col_fallback[ie]) {
        tex.SetTextColor(kRed);
        tex.DrawLatex(0.16, 0.81, "raw-mean points included in fit");
        tex.SetTextColor(kBlack);
      }
      if (ie == 0) {
        tex.DrawLatex(0.16, 0.16, "open: raw-mean fallback (not fit)");
      }
    }
    c_z->SaveAs(Form("%s/zeta_zfits_r0%d.png", figdir, r_val));
    delete c_z;
  }

  // Per-column evaluation in z: the fitted curve, evaluated over the full -200..200 cm
  // range (its smooth extension covers the outermost bins).
  auto eval_col = [&](int ie, double z) {
    return f_zfit[ie]->Eval(z);
  };

  // Surface evaluation: the per-column z curves, linearly interpolated in f between the
  // column centers (clamped to the first/last column beyond their centers).
  auto eval_surface = [&](double z, double f) {
    double fc_first = 0.5 / (double)n_eta;
    double fc_last  = 1.0 - 0.5 / (double)n_eta;
    if (f <= fc_first) return eval_col(0, z);
    if (f >= fc_last)  return eval_col(n_eta-1, z);
    int ie = (int)((f - fc_first) * n_eta); // column to the left of f
    if (ie > n_eta - 2) ie = n_eta - 2;
    double f_lo = (ie + 0.5) / (double)n_eta;
    double w = (f - f_lo) * n_eta; // linear weight toward the next column
    return (1.0 - w) * eval_col(ie, z) + w * eval_col(ie+1, z);
  };

  // ---------------- Step 3: fine smooth surface and inverted scale factor ----------------
  std::cout << "Step 3: fine map and inversion" << std::endl;

  // Applied to the scale factor after inversion, so where the limit bites
  // h2_zeta_corr is deliberately NOT 1 / h2_zeta_mean_smooth.
  int n_limited = 0;
  auto limit_corr = [&](double corr) {
    if (corr < zeta_corr_min) { ++n_limited; return zeta_corr_min; }
    if (corr > zeta_corr_max) { ++n_limited; return zeta_corr_max; }
    return corr;
  };
  const int n_zfine = 400;  // 1 cm bins, -200..200 cm
  const int n_ffine = 50;   // 0.02 bins in f
  TH2D *h2_smooth = new TH2D(Form("h2_zeta_mean_smooth_r0%d", r_val),
                             ";z_{vertex} [cm];#eta fraction f;<p_{T}^{calib} / p_{T}^{truth}> (smoothed)",
                             n_zfine, -200, 200, n_ffine, 0, 1);
  TH2D *h2_corr = new TH2D(Form("h2_zeta_corr_r0%d", r_val),
                           ";z_{vertex} [cm];#eta fraction f;scale factor (1/mean)",
                           n_zfine, -200, 200, n_ffine, 0, 1);
  for (int ix = 1; ix <= n_zfine; ++ix) {
    double z = h2_smooth->GetXaxis()->GetBinCenter(ix);
    for (int iy = 1; iy <= n_ffine; ++iy) {
      double f = h2_smooth->GetYaxis()->GetBinCenter(iy);
      double m = eval_surface(z, f);
      if (m < 0.5) m = 0.5; // safety clamp against pathological fit values
      if (m > 1.5) m = 1.5;
      h2_smooth->SetBinContent(ix, iy, m);
      h2_corr->SetBinContent(ix, iy, limit_corr(1.0 / m));
    }
  }

  // Smoothed surface at the measurement binning (after the fits, before the fine
  // interpolation), for direct bin-by-bin comparison with h2_zeta_mean_raw (e.g. pulls).
  TH2D *h2_smooth_coarse = new TH2D(Form("h2_zeta_mean_smooth_coarse_r0%d", r_val),
                                    ";z_{vertex} [cm];#eta fraction f;<p_{T}^{calib} / p_{T}^{truth}> (smoothed, measurement bins)",
                                    n_zphys, z_edge, n_eta, 0.0, 1.0);
  for (int ix = 1; ix <= n_zphys; ++ix) {
    double z = h2_smooth_coarse->GetXaxis()->GetBinCenter(ix);
    for (int iy = 1; iy <= n_eta; ++iy) {
      double f = h2_smooth_coarse->GetYaxis()->GetBinCenter(iy);
      double m = eval_surface(z, f);
      if (m < 0.5) m = 0.5;
      if (m > 1.5) m = 1.5;
      h2_smooth_coarse->SetBinContent(ix, iy, m);
    }
  }
  std::cout << "h2_zeta_corr limited to [" << zeta_corr_min << ", " << zeta_corr_max << "]: "
            << n_limited << " / " << n_zfine * n_ffine << " bins at the limit";
  if (n_limited > 0) std::cout << "  <-- check zeta_zfits_r0" << r_val << ".png for the columns responsible";
  std::cout << std::endl;

  {
    TCanvas *c_sc = new TCanvas("c_zeta_smooth_coarse", "", 1000, 700);
    c_sc->SetLeftMargin(0.10);
    c_sc->SetRightMargin(0.15);
    c_sc->SetTopMargin(0.08);
    c_sc->SetBottomMargin(0.12);
    gStyle->SetPaintTextFormat("4.3f");
    h2_smooth_coarse->GetZaxis()->SetRangeUser(0.9, 1.1);
    h2_smooth_coarse->Draw("COLZ TEXT");
    TLatex tex_sc;
    tex_sc.SetNDC();
    tex_sc.SetTextFont(42);
    tex_sc.SetTextSize(0.04);
    tex_sc.DrawLatex(0.12, 0.94, Form("Anti-k_{T} R=0.%d, smoothed <p_{T}^{calib}/p_{T}^{truth}> at measurement bins", r_val));
    c_sc->SaveAs(Form("%s/zeta_smoothmap_coarse_r0%d.png", figdir, r_val));
    delete c_sc;
  }

  TCanvas *c_map = new TCanvas("c_zeta_map", "", 1400, 600);
  c_map->Divide(2, 1);
  c_map->cd(1);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.18);
  h2_smooth->GetZaxis()->SetRangeUser(0.9, 1.1);
  h2_smooth->Draw("COLZ");
  c_map->cd(2);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.18);
  h2_corr->GetZaxis()->SetRangeUser(0.9, 1.1);
  h2_corr->Draw("COLZ");
  c_map->SaveAs(Form("%s/zeta_corr_map_r0%d.png", figdir, r_val));
  delete c_map;

  // ---------------- Write outputs ----------------
  f_diag->cd();
  h2_raw->Write();
  h_noz->Write();
  f_noz->Write();
  f_noz_corr->Write();
  for (int ie = 0; ie < n_eta; ++ie) {
    if (g_zcol[ie]) g_zcol[ie]->Write();
    if (g_zcol_excl[ie]) g_zcol_excl[ie]->Write();
    if (f_zfit[ie]) f_zfit[ie]->Write();
  }
  h2_smooth_coarse->Write();
  h2_smooth->Write();
  h2_corr->Write();

  // Calibration file: first-pass 2D map + the new (z, eta) scale-factor map
  TFile *f_firstpass = TFile::Open(Form("calib_r0%d.root", r_val), "READ");
  TH2D *h_calib = nullptr;
  if (f_firstpass && !f_firstpass->IsZombie()) {
    h_calib = (TH2D*)f_firstpass->Get(Form("h2_jes_r0%d_z0", r_val));
    if (!h_calib) h_calib = (TH2D*)f_firstpass->Get(Form("h2_jes_r0%d_z1", r_val)); // legacy first-pass naming
  }
  TFile *f_out = new TFile(Form("JES_Calibration_r0%d.root", r_val), "RECREATE");
  if (h_calib) {
    h_calib->SetTitle(Form("h2d_jes_calib_r0%d", r_val));
    h_calib->Write(Form("h2d_jes_calib_r0%d", r_val));
  } else {
    std::cerr << "Warning: first-pass calibration calib_r0" << r_val
              << ".root not found; JES_Calibration file contains only the (z, eta) correction." << std::endl;
  }
  h2_corr->Write();
  f_noz->Write();
  f_noz_corr->Write();
  for (int ie = 0; ie < n_eta; ++ie) {
    if (f_zfit[ie]) f_zfit[ie]->Write();
  }
  f_out->Close();
  if (f_firstpass) f_firstpass->Close();

  f_diag->Close();

  for (int izz = 0; izz < n_zall; ++izz) {
    for (int ie = 0; ie < n_etafracbin_max; ++ie) {
      if (h_dist[izz][ie]) delete h_dist[izz][ie];
      if (f_dist[izz][ie]) delete f_dist[izz][ie];
    }
  }

  std::cout << "Done. Diagnostics: " << Form("zetadependence_r0%d.root", r_val)
            << ", calibration: " << Form("JES_Calibration_r0%d.root", r_val)
            << ", figures in " << figdir << "/" << std::endl;
}
