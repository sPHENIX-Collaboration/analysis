#include <TFile.h>
#include <TH2D.h>
#include <TF1.h>
#include <TString.h>
#include <iostream>

// ============================================================================
// Assemble the final delivered calibration file: for every jet radius R = 0.2-0.8,
// collect from JES_Calibration_r0R.root
//   h2d_jes_calib_r0R    : first-pass calibration map, calibrated (truth-equivalent)
//                          pT vs (reco pT, EMfrac)
//   h2_zeta_corr_r0R     : residual (z, f) correction map, scale factor vs
//                          (signed z-vertex, eta fraction f)
//   f_zeta_noz_corr_r0R  : no-vertex correction function, scale factor vs f
//                          (f computed at z = 0)
// and write all of them into ONE file.
//
// Usage:  root -l -q -b 'make_final_calibration.C()'
// Output: JES_Calibration_Final.root
//
// Radii with a missing input file or missing objects are reported and skipped.
// ============================================================================

void make_final_calibration(const char* outname = "JES_Calibration_Final.root") {
  const int radii[] = {2, 3, 4, 5, 6, 7, 8};
  const int n_radii = sizeof(radii) / sizeof(int);

  TFile *fout = new TFile(outname, "RECREATE");
  int n_complete = 0;

  for (int ir = 0; ir < n_radii; ++ir) {
    int r_val = radii[ir];
    TString inname = Form("JES_Calibration_r0%d.root", r_val);
    TFile *fin = TFile::Open(inname, "READ");
    if (!fin || fin->IsZombie()) {
      std::cerr << "Warning: cannot open " << inname << "; R=0." << r_val << " skipped." << std::endl;
      continue;
    }

    TH2D *h2_jes = (TH2D*)fin->Get(Form("h2d_jes_calib_r0%d", r_val));
    TH2D *h2_zeta = (TH2D*)fin->Get(Form("h2_zeta_corr_r0%d", r_val));
    TF1  *f_noz  = (TF1*)fin->Get(Form("f_zeta_noz_corr_r0%d", r_val));
    if (!h2_jes || !h2_zeta || !f_noz) {
      std::cerr << "Warning: missing object(s) in " << inname << " ("
                << (h2_jes ? "" : "h2d_jes_calib ") << (h2_zeta ? "" : "h2_zeta_corr ")
                << (f_noz ? "" : "f_zeta_noz_corr") << "); R=0." << r_val << " skipped." << std::endl;
      fin->Close();
      delete fin;
      continue;
    }

    h2_jes->SetDirectory(0);
    h2_zeta->SetDirectory(0);
    fin->Close();
    delete fin;

    fout->cd();
    h2_jes->Write(Form("h2d_jes_calib_r0%d", r_val));
    h2_zeta->Write(Form("h2_zeta_corr_r0%d", r_val));
    f_noz->Write(Form("f_zeta_noz_corr_r0%d", r_val));
    delete h2_jes;
    delete h2_zeta;
    ++n_complete;
    std::cout << "R=0." << r_val << ": first-pass map + (z, f) correction + no-vertex function written." << std::endl;
  }

  fout->Close();
  std::cout << outname << " written with " << n_complete << " / " << n_radii << " radii complete." << std::endl;
}
