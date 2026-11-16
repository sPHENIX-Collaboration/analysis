#include <TFile.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TSystem.h>
#include <iostream>
#include <vector>

// Fit-point x position for cell (ix, iy): the barycenter from h2_bary when available
// and inside the bin (bin-centering correction), otherwise the geometric bin center.
// axis = 'x' for the truthpt direction, 'y' for the emfrac direction.
double get_fitpoint_x(TH2D* h2, TH2D* h2_bary, int ix, int iy, char axis) {
  TAxis *a = (axis == 'x') ? h2->GetXaxis() : h2->GetYaxis();
  int ibin = (axis == 'x') ? ix : iy;
  double x = a->GetBinCenter(ibin);
  if (h2_bary) {
    double b = h2_bary->GetBinContent(ix, iy);
    if (b > a->GetBinLowEdge(ibin) && b < a->GetBinUpEdge(ibin)) x = b;
  }
  return x;
}

void perform_truthpt_fit(TGraphErrors* g, TF1* f_exp, float fit_min, float fit_max) {
    // [0] + [1] * exp([2] * (x - [3]))
    // p0: Asymptote (~0.8), p1: Amplitude drop, p2: Convergence rate (negative), p3: start point
    //f_exp->SetParameters(0.7, -0.3, -0.01, 10);
    //f_exp->SetParLimits(0, 0.4, 1.0);  // Plateau shouldn't be crazy
    //f_exp->SetParLimits(1, -2.0, 0.0); // Must be negative to drop at low pT
    //f_exp->SetParLimits(2, -1.0, 0.0); // Must be negative to plateau at high pT
    //f_exp->SetParLimits(3, -30.0, 30.0); // Start point should be within the pT range

    f_exp->SetParameters(0.7, 0.1, 0.1);
    f_exp->SetParLimits(0, 0., 1.1);
    f_exp->SetParLimits(1, 0., 1.0);
    f_exp->SetParLimits(2, 0., 1.0);

    if (g->GetN() > 0) g->Fit(f_exp, "QW", "", fit_min, fit_max);
}

void perform_emfrac_fits(TGraphErrors* g, TF1* f_low, TF1* f_mid, TF1* f_high, double val_040, double val_070) {
  // Seed low fit (linear): [0] value at EMfrac = 0, [1] slope
  f_low->SetParameters(0.5, 0.1);

  // Seed mid fit: [0] is baseline, [1] is slope
  f_mid->SetParameters((val_040+val_070)/2., 0.1);
  f_mid->SetParLimits(0, 0.3, 1.0);
  f_mid->SetParLimits(1, -0.1, 0.5);

  // Seed high fit: [0] is baseline, [1] is amplitude, [2] is slope
  f_high->SetParameters((val_040+val_070)/2., 0.3, 10.0);
  f_high->SetParLimits(1, 0, 5); // Force [1] positive
  f_high->SetParLimits(2, 0, 50); // Force [2] positive for exponential growth

  if (g->GetN() > 0) {
    g->Fit(f_low, "Q", "", 0, 0.4);
    g->Fit(f_mid, "Q", "", 0.3, 0.7);
    g->Fit(f_high, "Q", "", 0.65, 1.0);
  }
}

void process_truthpt_dependence(TH2D* h2_mean, TH2D* h2_smooth, int r_val, int z_val, TString tag, float fit_min, float fit_max, TH2D* h2_barypt = nullptr, TH2D* h2_chi2 = nullptr) {
    int n_emfracbin = h2_mean->GetNbinsY();
    TCanvas *c = new TCanvas(Form("c_pt_dep_%s_r0%d_z%d", tag.Data(), r_val, z_val), "Response vs pT", 1800, 1500);
    c->Divide(5, (n_emfracbin + 4) / 5); // Grid layout

    //TF1 *f_exp = new TF1("f_exp", "[0] + [1] * (TMath::Exp([2] * (x - [3])))", 0, 200);
    TF1 *f_exp = new TF1("f_exp", "([0] + [1]*x) / (1 + [2]*x)", 0, 200);
    f_exp->SetLineColor(kGreen+2);
    f_exp->SetLineWidth(2);

    for (int iy = 1; iy <= n_emfracbin; ++iy) {
      c->cd(iy);
      TString proj_name = Form("proj_pt_%s_r0%d_z%d_em%d", tag.Data(), r_val, z_val, iy);
      TH1D *h_proj = h2_mean->ProjectionX(proj_name, iy, iy); // Slice Y, Project X (display: open markers at bin centers)

      double em_low = h2_mean->GetYaxis()->GetBinLowEdge(iy);
      double em_high = h2_mean->GetYaxis()->GetBinUpEdge(iy);
      h_proj->SetTitle(Form("E_{EM} #in [%.2f, %.2f];p_{T}^{truth};Mean Response", em_low, em_high));
      h_proj->SetMarkerStyle(24);
      h_proj->GetYaxis()->SetRangeUser(0.3, 1.1);

      // Fit points: x at the cell barycenter <truthpt> (bin-centering correction);
      // the outermost bins stay at their centers (outside the fit ranges anyway).
      // Cells whose Gaussian fit failed (chi2/ndf = -1, raw mean/RMS fallback) are
      // excluded from the fit; their smoothed values come from the fitted trend
      // evaluated at the bin center like every other cell.
      std::vector<double> gx, gy, gye;
      for (int ix = 1; ix <= h2_mean->GetNbinsX(); ++ix) {
        double val = h2_mean->GetBinContent(ix, iy);
        if (val == 0) continue;
        if (h2_chi2 && h2_chi2->GetBinContent(ix, iy) < 0) continue; // failed Gaussian fit
        TH2D* bary = (ix == 1 || ix == h2_mean->GetNbinsX()) ? nullptr : h2_barypt;
        gx.push_back(get_fitpoint_x(h2_mean, bary, ix, iy, 'x'));
        gy.push_back(val);
        gye.push_back(h2_mean->GetBinError(ix, iy));
      }
      TGraphErrors *g = new TGraphErrors(gx.size(), gx.data(), gy.data(), nullptr, gye.data());
      g->SetMarkerStyle(20);
      g->SetMarkerSize(0.7);

      perform_truthpt_fit(g, f_exp, fit_min, fit_max);
      h_proj->Draw("E1");
      g->Draw("P same");
      f_exp->DrawCopy("same");

      for (int ix = 1; ix <= h2_smooth->GetNbinsX(); ++ix) {
        double x_center = h2_smooth->GetXaxis()->GetBinCenter(ix);
        double fit_val = f_exp->Eval(x_center);
        if (fit_val < 0.5) fit_val = 0.5;
        h2_smooth->SetBinContent(ix, iy, fit_val);
      }
    }
    c->SaveAs(Form("figure_gaus/smooth_truthpt_dependence_%s_r0%d_z%d.png", tag.Data(), r_val, z_val));
    delete f_exp;
}

void process_emfrac_dependence(TH2D* h2_mean, TH2D* h2_smooth, int r_val, int z_val, TString tag, TH2D* h2_baryem = nullptr) {
  int n_truthptbin = h2_mean->GetNbinsX();

  // Define Functions
  // Straight line: the low-emfrac response is flat or slowly rising, so a linear
  // form is sufficient and always stable (the previous rational form could place
  // its pole near the fit range and blow up).
  TF1 *f_low = new TF1("f_low", "[0] + [1]*x", -0.1, 0.5);
  f_low->SetLineColor(kBlue);
  f_low->SetLineWidth(2);

  TF1 *f_mid = new TF1("f_mid", "[0] + [1]*x", 0.3, 0.7);
  f_mid->SetLineColor(kGreen+2);
  f_mid->SetLineWidth(2);

  TF1 *f_high = new TF1("f_high", "[0] + [1]*exp([2]*(x-0.7))", 0.5, 1.05);
  f_high->SetLineColor(kRed);
  f_high->SetLineWidth(2);

  // Setup Canvas
  TCanvas *c = new TCanvas(Form("c_smooth_exp_%s_r0%d_z%d", tag.Data(), r_val, z_val), "Dual Exponential Fits", 1800, 1500);
  c->Divide(5, (n_truthptbin + 4) / 5);

  // Loop Over Truth pT Slices
  for (int ix = 1; ix <= n_truthptbin; ++ix) {
    c->cd(ix);
    
    TString proj_name = Form("proj_%s_r0%d_z%d_pt%d", tag.Data(), r_val, z_val, ix);
    TH1D *h_proj = h2_mean->ProjectionY(proj_name, ix, ix); // display: open markers at bin centers

    double pt_low = h2_mean->GetXaxis()->GetBinLowEdge(ix);
    double pt_high = h2_mean->GetXaxis()->GetBinUpEdge(ix);
    h_proj->SetTitle(Form("p_{T} #in [%.0f, %.0f] GeV;E_{EMfrac};Mean Response", pt_low, pt_high));
    h_proj->SetMarkerStyle(24);
    h_proj->SetMarkerSize(0.8);
    h_proj->GetYaxis()->SetRangeUser(0.3,  1.1);

    // Seeds from the values nearest EMfrac = 0.4 / 0.7 (as before)
    double val_040 = h_proj->GetBinContent(h_proj->FindBin(0.4));
    double val_070 = h_proj->GetBinContent(h_proj->FindBin(0.7));

    // Fit points: x at the cell barycenter <EMfrac> (bin-centering correction).
    // Cells with zero error carry values propagated into empty cells by the pT
    // smoothing; the histogram fit skipped them (zero weight), so skip them here too.
    std::vector<double> gx, gy, gye;
    for (int iy = 1; iy <= h2_mean->GetNbinsY(); ++iy) {
      double val = h2_mean->GetBinContent(ix, iy);
      double err = h2_mean->GetBinError(ix, iy);
      if (val == 0 || err <= 0) continue;
      gx.push_back(get_fitpoint_x(h2_mean, h2_baryem, ix, iy, 'y'));
      gy.push_back(val);
      gye.push_back(err);
    }
    TGraphErrors *g = new TGraphErrors(gx.size(), gx.data(), gy.data(), nullptr, gye.data());
    g->SetMarkerStyle(20);
    g->SetMarkerSize(0.8);

    perform_emfrac_fits(g, f_low, f_mid, f_high, val_040, val_070);
    h_proj->Draw("E1");
    g->Draw("P same");
    f_low->DrawCopy("same");
    f_mid->DrawCopy("same");
    f_high->DrawCopy("same");

    // Region assignment by EMfrac value (binning-independent), aligned with the three
    // fit ranges: f_low below 0.3, f_mid in [0.3, 0.7], f_high above 0.7.
    for (int iy = 1; iy <= h2_smooth->GetNbinsY(); ++iy) {
      double y_center = h2_smooth->GetYaxis()->GetBinCenter(iy);
      double fit_val;
      if (y_center < 0.3) {
        fit_val = f_low->Eval(y_center);
      } else if (y_center <= 0.7) {
        fit_val = f_mid->Eval(y_center);
      } else {
        fit_val = f_high->Eval(y_center);
        if (fit_val > 1.0) fit_val = 1.0; // Cap at 1.0 for physicality
      }
      h2_smooth->SetBinContent(ix, iy, fit_val);
    }
  }

  c->SaveAs(Form("figure_gaus/smooth_emfrac_dependence_%s_r0%d_z%d.png", tag.Data(), r_val, z_val));

  delete c;
  delete f_low;
  delete f_mid;
  delete f_high;
}

void Jet_resp_slicefitter(int r_val = 4, int z_val = 1) {
  // Global styling
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0); 

  // Open File
  TString filename = Form("output_gaus/output_gaus_r0%d_z%d.root", r_val, z_val);
  TFile *fin = TFile::Open(filename, "READ");
  if (!fin || fin->IsZombie()) {
      std::cerr << "Error: Could not open " << filename << std::endl;
      return;
  }
  TFile *fout = new TFile(Form("output_smooth_r0%d_z%d.root", r_val, z_val), "RECREATE");

  // Get Histogram
  TString histname = Form("h2_mean_r0%d_z%d", r_val, z_val);
  TH2D *h2_mean = (TH2D*)fin->Get(histname);
  if (!h2_mean) {
      std::cerr << "Error: Could not find " << histname << " in file." << std::endl;
      fin->Close();
      return;
  }

  // The first and last pT bins are kept as measured: their centers lie outside the
  // smoothing fit ranges, so they do not influence the fits; their smoothed values
  // come from the fitted trends like every other bin.

  //TH2D* h2_emsmooth = (TH2D*)h2_mean->Clone(Form("h2_mean_r0%d_z%d_emsmooth", r_val, z_val));
  //TH2D* h2_emptsmooth = (TH2D*)h2_mean->Clone(Form("h2_mean_r0%d_z%d_emptsmooth", r_val, z_val));
  //
  //process_emfrac_dependence(h2_mean, h2_emsmooth, r_val, z_val, "Orig");
  //process_truthpt_dependence(h2_emsmooth, h2_emptsmooth, r_val, z_val, "Double");

  TH2D* h2_ptsmooth = (TH2D*)h2_mean->Clone(Form("h2_mean_r0%d_z%d_ptsmooth", r_val, z_val));
  TH2D* h2_ptemsmooth = (TH2D*)h2_mean->Clone(Form("h2_mean_r0%d_z%d_ptemsmooth", r_val, z_val));
  TH2D* h2_finalsmooth = (TH2D*)h2_mean->Clone(Form("h2_mean_r0%d_z%d_finalsmooth", r_val, z_val));

  // Barycenter maps for the bin-centering correction (written by the histfitter when
  // the sum-of-weights histograms exist). The first fit in each direction uses the
  // barycenter positions and evaluates at the bin centers, so its output is
  // center-valued; the final "Double" pT fit therefore runs on bin centers.
  TH2D *h2_barypt = (TH2D*)fin->Get(Form("h2_barypt_r0%d_z%d", r_val, z_val));
  TH2D *h2_baryem = (TH2D*)fin->Get(Form("h2_baryem_r0%d_z%d", r_val, z_val));
  if (!h2_barypt || !h2_baryem) {
    std::cout << "Note: barycenter maps not found in " << filename << "; smoothing fits use bin centers." << std::endl;
    h2_barypt = nullptr;
    h2_baryem = nullptr;
  }
  // Per-cell Gaussian fit quality (-1 = raw mean/RMS fallback): failed cells are
  // excluded from the First truthpt fit.
  TH2D *h2_chi2 = (TH2D*)fin->Get(Form("h2_chi2_r0%d_z%d", r_val, z_val));
  if (!h2_chi2) {
    std::cout << "Note: h2_chi2 map not found in " << filename << "; failed-fit cells are not excluded from the smoothing." << std::endl;
  }

  process_truthpt_dependence(h2_mean, h2_ptsmooth, r_val, z_val, "First", 10, 80, h2_barypt, h2_chi2);
  process_emfrac_dependence(h2_ptsmooth, h2_ptemsmooth, r_val, z_val, "First", h2_baryem);
  process_truthpt_dependence(h2_ptemsmooth, h2_finalsmooth, r_val, z_val, "Double", 5, 80);

  fout->cd();
  //h2_mean->Write();
  //h2_emsmooth->Write();
  //h2_emptsmooth->Write();

  h2_mean->Write();
  h2_ptsmooth->Write();
  h2_ptemsmooth->Write();
  h2_finalsmooth->Write();

  fin->Close();
  fout->Close();
}
