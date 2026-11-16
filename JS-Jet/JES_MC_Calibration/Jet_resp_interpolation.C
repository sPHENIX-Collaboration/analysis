#include <algorithm>
#include <iostream>
#include "TFile.h"
#include "TH2D.h"
#include "TAxis.h"
#include "TString.h"

// 1D Catmull-Rom cubic interpolation
double CubicInterpolate(double p[4], double t) {
    return p[1] + 0.5 * t * (p[2] - p[0]
         + t * (2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3]
         + t * (3.0*p[1] - 3.0*p[2] + p[3] - p[0])));
}

double GetSmartInterpolation(TH2D* h, double x, double y) {
  TAxis *xAxis = h->GetXaxis();
  TAxis *yAxis = h->GetYaxis();
  int nBinsX = xAxis->GetNbins();
  int nBinsY = yAxis->GetNbins();

  // Clamp to bin-center range so Interpolate() and bicubic neighbors are valid
  double x_lo = xAxis->GetBinCenter(1);
  double x_hi = xAxis->GetBinCenter(nBinsX);
  double y_lo = yAxis->GetBinCenter(1);
  double y_hi = yAxis->GetBinCenter(nBinsY);
  x = std::clamp(x, x_lo, x_hi);
  y = std::clamp(y, y_lo, y_hi);

  int binX = xAxis->FindBin(x);
  int binY = yAxis->FindBin(y);

  // Bicubic needs bins i0-1 .. i0+2 where i0 = binX-1 or binX
  // Safe range for bicubic: binX in [3, nBinsX-2] and same for y
  bool nearEdge = (binX <= 2 || binX >= nBinsX - 1 || binY <= 2 || binY >= nBinsY - 1);

  if (nearEdge) {
    return h->Interpolate(x, y);   // bilinear
  }

  // Bicubic
  int i0 = (x < xAxis->GetBinCenter(binX)) ? binX - 1 : binX;
  int j0 = (y < yAxis->GetBinCenter(binY)) ? binY - 1 : binY;

  double tx = (x - xAxis->GetBinCenter(i0)) / (xAxis->GetBinCenter(i0+1) - xAxis->GetBinCenter(i0));
  double ty = (y - yAxis->GetBinCenter(j0)) / (yAxis->GetBinCenter(j0+1) - yAxis->GetBinCenter(j0));

  double rowResults[4];
  for (int j = 0; j < 4; j++) {
    double p[4];
    for (int i = 0; i < 4; i++) {
      p[i] = h->GetBinContent(i0 - 1 + i, j0 - 1 + j);
    }
    rowResults[j] = CubicInterpolate(p, tx);
  }
  return CubicInterpolate(rowResults, ty);
}

TH2D* InvertResponseMap(TH2D *h2_fine, const TString &outname, double recopt_min = 0.0, double recopt_max = 150.0, double recopt_step = 0.05, double pt_plateau = 100.0) {

  // Effective response: flat above pt_plateau
  auto Response = [&](double pt, double emfrac) {
    TAxis *xa = h2_fine->GetXaxis();
    TAxis *ya = h2_fine->GetYaxis();
    double x_lo = xa->GetBinCenter(1);
    double x_hi = xa->GetBinCenter(xa->GetNbins());
    double y_lo = ya->GetBinCenter(1);
    double y_hi = ya->GetBinCenter(ya->GetNbins());
  
    double pt_eff = std::min(pt, pt_plateau);
    pt_eff = std::clamp(pt_eff, x_lo, x_hi);
    double em_eff = std::clamp(emfrac, y_lo, y_hi);
  
    return h2_fine->Interpolate(pt_eff, em_eff);
  };

  // Forward map: recopt(pt, emfrac) = pt * Response(pt, emfrac)
  // Above pt_plateau, this grows linearly with slope Response(pt_plateau, emfrac).

  auto FindTruthpt = [&](double recopt_target, double emfrac,
                         double pt_lo, double pt_hi) -> double {
    auto forward = [&](double pt) {
      return pt * Response(pt, emfrac) - recopt_target;
    };
    double a = pt_lo, b = pt_hi;
    double fa = forward(a);
    double fb = forward(b);
    if (fa * fb > 0) return -1.0;
    for (int iter = 0; iter < 80; ++iter) {
      double m  = 0.5 * (a + b);
      double fm = forward(m);
      if (std::fabs(fm) < 1e-5) return m;
      if (fa * fm < 0) { b = m; fb = fm; }
      else             { a = m; fa = fm; }
      if ((b - a) < 1e-5) return 0.5 * (a + b);
    }
    return 0.5 * (a + b);
  };

  int n_recopt = static_cast<int>((recopt_max - recopt_min) / recopt_step + 0.5);

  int    n_em   = h2_fine->GetYaxis()->GetNbins();
  double em_min = h2_fine->GetYaxis()->GetXmin();
  double em_max = h2_fine->GetYaxis()->GetXmax();

  TH2D *h2_inv = new TH2D(outname, "truthpt vs (recopt, emfrac);recopt;emfrac;truthpt", n_recopt, recopt_min, recopt_max, n_em, em_min, em_max);

  // Search range: low end from h2_fine, high end extended to cover
  // recopt_max even when response is small.
  double pt_lo = h2_fine->GetXaxis()->GetBinCenter(1);
  double pt_hi = 10.0 * pt_plateau;   // 1000 by default; well above any reachable inverse

  std::cout << "Inverting response map (flat extrapolation above pt = " << pt_plateau << ")..." << std::endl;
  int n_filled = 0, n_empty = 0;

  for (int ix = 1; ix <= n_recopt; ++ix) {
    double recopt = h2_inv->GetXaxis()->GetBinCenter(ix);
    for (int iy = 1; iy <= n_em; ++iy) {
      double emfrac = h2_inv->GetYaxis()->GetBinCenter(iy);
      double truthpt = FindTruthpt(recopt, emfrac, pt_lo, pt_hi);
      if (truthpt > 0) {
        h2_inv->SetBinContent(ix, iy, truthpt);
        ++n_filled;
      } else {
        ++n_empty;
      }
    }
    if (n_recopt >= 10 && ix % (n_recopt / 10) == 0) {
      std::cout << "  " << (100 * ix / n_recopt) << "%" << std::endl;
    }
  }

  std::cout << "Done. Filled " << n_filled << " bins, " << n_empty << " bins had no solution." << std::endl;
  return h2_inv;
}

void Jet_resp_interpolation(int r_val = 4, int z_val = 1) {
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0); 

  TString filename = Form("output_smooth_r0%d_z%d.root", r_val, z_val);
  TFile *fin = TFile::Open(filename, "READ");
  if (!fin || fin->IsZombie()) {
    std::cerr << "Error: Could not open " << filename << std::endl;
    return;
  }

  TFile *fout = new TFile(Form("output_final_r0%d_z%d.root", r_val, z_val), "RECREATE");

  TString histname = Form("h2_mean_r0%d_z%d", r_val, z_val);
  TH2D *h2_mean = (TH2D*)fin->Get(histname);
  if (!h2_mean) {
    std::cerr << "Error: Could not find " << histname << std::endl;
    fin->Close();
    return;
  }

  TString histsmoothname = Form("h2_mean_r0%d_z%d_finalsmooth", r_val, z_val);
  TH2D *h2_smooth = (TH2D*)fin->Get(histsmoothname);
  if (!h2_smooth) {
    std::cerr << "Error: Could not find " << histsmoothname << std::endl;
    fin->Close();
    return;
  }

  // --- Build the fine-binned output histogram ---
  // Identify which axis is truthpt and which is emfrac. From your binning arrays
  // and your earlier description (x: 10–90 truthpt, y: -0.1–1.1 emfrac), x = truthpt,
  // y = emfrac. Adjust if your file has them swapped.

  const double pt_min = 0.0,  pt_max = 100.0,  pt_step = 0.05;
  const double em_min = 0.0,  em_max = 1.0,   em_step = 0.0005;

  int n_pt_fine = static_cast<int>((pt_max - pt_min) / pt_step + 0.5);  // 2000
  int n_em_fine = static_cast<int>((em_max - em_min) / em_step + 0.5);  // 2000

  TString outname = Form("h2_mean_r0%d_z%d_fine", r_val, z_val);
  TH2D *h2_fine = new TH2D(outname, outname, n_pt_fine, pt_min, pt_max, n_em_fine, em_min, em_max);
  h2_fine->GetXaxis()->SetTitle(h2_smooth->GetXaxis()->GetTitle());
  h2_fine->GetYaxis()->SetTitle(h2_smooth->GetYaxis()->GetTitle());

  // --- Fill by interpolation ---
  for (int ix = 1; ix <= n_pt_fine; ++ix) {
    double x = h2_fine->GetXaxis()->GetBinCenter(ix);
    for (int iy = 1; iy <= n_em_fine; ++iy) {
      double y = h2_fine->GetYaxis()->GetBinCenter(iy);
      double val = GetSmartInterpolation(h2_smooth, x, y);
      h2_fine->SetBinContent(ix, iy, val);
    }
  }

  TString invname = Form("h2_jes_r0%d_z%d", r_val, z_val);
  TH2D *h2_inv = InvertResponseMap(h2_fine, invname, 0.0, 100.0, 0.05);

  fout->cd();
  h2_mean->Write();
  h2_smooth->Write();
  h2_fine->Write();
  h2_inv->Write();
  fout->Close();
  fin->Close();
  std::cout << "Wrote " << outname << " ("<< n_pt_fine << " x " << n_em_fine << " bins)" << std::endl;
}