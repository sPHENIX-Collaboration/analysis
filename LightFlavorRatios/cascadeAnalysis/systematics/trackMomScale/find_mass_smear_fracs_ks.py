#!/usr/bin/env python3
"""
find_mass_smear_fracs_ks.py
---------------------------
Finds the per-pT-bin smearing fraction that makes the Gaussian sigma of the
K0S invariant mass peak in smeared SV simulation match the data.

Method (per parent K0S pT bin):
  1. Fit Gaussian to data K0S mass in FIT window → sigma_data
  2. Generate one set of N(0,1) draws (fixed seed) for the sim events in
     the bin; reuse the same draws for every fraction value so sigma(f)
     is a smooth, deterministic curve.
  3. Scan frac from 0 to FRAC_MAX; for each f smear both pion tracks
     (dpT = f * pT * z), recompute K0S mass, fit Gaussian → sigma(f)
  4. Interpolate sigma(f) to find f* where sigma(f*) = sigma_data.
  5. Verify with an independent random seed.

Output:
  mass_smear_fracs_ks.txt    — fracs table (compatible with smear_sv_ks.py --fracs)
  plot_mass_smear_fracs_ks.C — ROOT macro: sigma comparison + frac vs pT

Usage:
    python3 find_mass_smear_fracs_ks.py

Notes:
  - avg_pt in the output is the mean K0S pT within each bin; smear_sv_ks.py
    uses avg_pt as a track-pT control point, so treat these fracs as an
    approximate pT-dependent calibration.
  - To use different bins change PT_EDGES below.
"""

import sys
import numpy as np
import uproot
from scipy.optimize import curve_fit

# ── configuration ──────────────────────────────────────────────────────────────
DATA_FILE = "KShort6RunCombined.root"
SIM_FILE  = "outputKFParticleKShortRecoSV_filtered.root"

PT_EDGES  = [0.5, 0.8, 1.1, 1.4, 1.8, 2.2, 3.0, 4.0]   # change freely

FRAC_SCAN = np.linspace(0.0, 0.20, 81)   # 0 → 20 %, step 0.25 %
FIT_LO    = 0.475   # GeV — Gaussian fit window
FIT_HI    = 0.520   # GeV
FIT_NBINS = 45
PION_MASS = 0.13957018  # GeV
RNG_SEED  = 42

# ── physics helpers ─────────────────────────────────────────────────────────────
def ks_mass_from_p3(px1, py1, pz1, px2, py2, pz2):
    """K0S invariant mass from two pion 3-momenta."""
    m = PION_MASS
    e1 = np.sqrt(px1**2 + py1**2 + pz1**2 + m**2)
    e2 = np.sqrt(px2**2 + py2**2 + pz2**2 + m**2)
    m2 = (e1+e2)**2 - (px1+px2)**2 - (py1+py2)**2 - (pz1+pz2)**2
    return np.sqrt(np.maximum(m2, 0.0))


def smear_p3(px, py, pz, f, z):
    """Scale transverse momentum by (1 + f*z), preserve pz (i.e. preserve eta)."""
    scale = 1.0 + f * z
    return px * scale, py * scale, pz


def _gauss(x, A, mu, sigma):
    return A * np.exp(-0.5 * ((x - mu) / sigma)**2)


def fit_sigma(masses_gev):
    """Gaussian fit to mass histogram in [FIT_LO, FIT_HI]. Returns (sigma_MeV, err_MeV)."""
    sel = masses_gev[(masses_gev >= FIT_LO) & (masses_gev < FIT_HI)]
    if len(sel) < 20:
        return np.nan, np.nan
    counts, edges = np.histogram(sel, bins=FIT_NBINS, range=(FIT_LO, FIT_HI))
    cx = 0.5 * (edges[:-1] + edges[1:])
    try:
        imax = int(np.argmax(counts))
        popt, pcov = curve_fit(
            _gauss, cx, counts.astype(float),
            p0=[float(counts[imax]), float(cx[imax]), 0.010],
            bounds=([0, FIT_LO, 0.001], [np.inf, FIT_HI, 0.050]),
            maxfev=10000,
        )
        s     = abs(popt[2]) * 1000.0
        s_err = np.sqrt(pcov[2, 2]) * 1000.0 if pcov[2, 2] >= 0 else np.nan
        return s, s_err
    except Exception:
        return np.nan, np.nan


# ── main ────────────────────────────────────────────────────────────────────────
def main():
    # ── read data ──────────────────────────────────────────────────────────────
    print(f"Reading {DATA_FILE} ...")
    with uproot.open(DATA_FILE) as f:
        t = f["DecayTree"]
        d_kspt   = t["K_S0_pT"].array(library="np")
        d_ksmass = t["K_S0_mass"].array(library="np")
    print(f"  {len(d_kspt)} data events")

    # ── read sim ────────────────────────────────────────────────────────────────
    print(f"Reading {SIM_FILE} ...")
    with uproot.open(SIM_FILE) as f:
        t = f["DecayTree"]
        s_kspt = t["K_S0_pT"].array(library="np")
        s_px1  = t["track_1_px"].array(library="np")
        s_py1  = t["track_1_py"].array(library="np")
        s_pz1  = t["track_1_pz"].array(library="np")
        s_px2  = t["track_2_px"].array(library="np")
        s_py2  = t["track_2_py"].array(library="np")
        s_pz2  = t["track_2_pz"].array(library="np")
    print(f"  {len(s_kspt)} sim events\n")

    # ── per-bin optimisation ────────────────────────────────────────────────────
    results = []
    nbins   = len(PT_EDGES) - 1

    hdr = (f"{'pT bin':>16}  {'N_data':>7}  {'N_sim':>6}  "
           f"{'σ_data':>8}  {'σ_sim':>7}  {'frac%*':>7}  {'σ_verify':>9}")
    print(hdr)
    print("─" * len(hdr))

    for b in range(nbins):
        lo, hi = PT_EDGES[b], PT_EDGES[b+1]

        # data sigma
        dmask        = (d_kspt >= lo) & (d_kspt < hi)
        sig_d, sig_d_err = fit_sigma(d_ksmass[dmask])
        n_d          = int(dmask.sum())

        # sim events in this bin
        smask = (s_kspt >= lo) & (s_kspt < hi)
        n_s   = int(smask.sum())
        avg_pt = float(s_kspt[smask].mean()) if n_s > 0 else 0.5*(lo+hi)

        if n_d < 20 or n_s < 20 or np.isnan(sig_d):
            print(f"  [{lo:.1f},{hi:.1f})  skipped (data={n_d}, sim={n_s})")
            results.append(dict(lo=lo, hi=hi, avg_pt=avg_pt,
                                sig_d=np.nan, sig_d_err=np.nan,
                                sig_s0=np.nan, frac_pct=np.nan,
                                sig_verify=np.nan, n_d=n_d, n_s=n_s,
                                scan_f=None, scan_s=None, note="skipped"))
            continue

        px1 = s_px1[smask]; py1 = s_py1[smask]; pz1 = s_pz1[smask]
        px2 = s_px2[smask]; py2 = s_py2[smask]; pz2 = s_pz2[smask]

        # generate one set of draws; reuse for all fracs → smooth sigma(f) curve
        rng = np.random.default_rng(RNG_SEED)
        z1  = rng.standard_normal(n_s)
        z2  = rng.standard_normal(n_s)

        # scan
        scan_s = []
        for f in FRAC_SCAN:
            px1s, py1s, pz1s = smear_p3(px1, py1, pz1, f, z1)
            px2s, py2s, pz2s = smear_p3(px2, py2, pz2, f, z2)
            ms   = ks_mass_from_p3(px1s, py1s, pz1s, px2s, py2s, pz2s)
            s, _ = fit_sigma(ms)
            scan_s.append(s)
        scan_s = np.array(scan_s)
        sig_s0 = float(scan_s[0])

        # interpolate to find optimal fraction
        valid = np.isfinite(scan_s)
        if not valid.any():
            frac_opt = np.nan
            note = "scan_failed"
        elif sig_d <= scan_s[valid][0]:
            frac_opt = 0.0
            note = "no_smear_needed"
        elif sig_d >= scan_s[valid][-1]:
            frac_opt = float(FRAC_SCAN[valid][-1])
            note = "exceeded_range"
            print(f"  WARNING [{lo:.1f},{hi:.1f}): σ_data={sig_d:.1f} > "
                  f"σ_max={scan_s[valid][-1]:.1f}; try increasing FRAC_MAX")
        else:
            frac_opt = float(np.interp(sig_d, scan_s[valid], FRAC_SCAN[valid]))
            note = "ok"

        # verify with independent random draws
        if not np.isnan(frac_opt):
            rng2 = np.random.default_rng(RNG_SEED + 1)
            z1v  = rng2.standard_normal(n_s)
            z2v  = rng2.standard_normal(n_s)
            px1v, py1v, pz1v = smear_p3(px1, py1, pz1, frac_opt, z1v)
            px2v, py2v, pz2v = smear_p3(px2, py2, pz2, frac_opt, z2v)
            mv   = ks_mass_from_p3(px1v, py1v, pz1v, px2v, py2v, pz2v)
            sig_verify, _ = fit_sigma(mv)
        else:
            sig_verify = np.nan

        frac_pct = frac_opt * 100.0 if not np.isnan(frac_opt) else np.nan

        results.append(dict(
            lo=lo, hi=hi, avg_pt=avg_pt,
            sig_d=sig_d, sig_d_err=sig_d_err,
            sig_s0=sig_s0, frac_pct=frac_pct,
            sig_verify=sig_verify, n_d=n_d, n_s=n_s,
            scan_f=FRAC_SCAN, scan_s=scan_s, note=note,
        ))

        v_str = f"{sig_verify:>8.2f}" if not np.isnan(sig_verify) else "     n/a"
        print(f"  [{lo:.1f},{hi:.1f})  {n_d:>7}  {n_s:>6}  "
              f"{sig_d:>6.2f}±{sig_d_err:<4.2f}  {sig_s0:>6.2f}  "
              f"{frac_pct:>6.3f}%  {v_str}")

    # ── write fracs file (smear_sv_ks.py compatible) ───────────────────────────
    valid_r = [r for r in results if not np.isnan(r["frac_pct"])]
    out_fracs = "mass_smear_fracs_ks.txt"
    with open(out_fracs, "w") as fh:
        fh.write("# Auto-generated by find_mass_smear_fracs_ks.py\n")
        fh.write("# Fracs chosen to match K0S mass Gaussian sigma between smeared sim and data.\n")
        fh.write("# avg_pt is mean K0S pT in the bin; fracs are applied per-track in smear_sv_ks.py.\n")
        fh.write("bin_lo,bin_hi,avg_pt,best_frac_pct,sig_data_mev,sig_data_err_mev,sig_sim0_mev,sig_verify_mev,n_data,n_sim,note\n")
        for r in valid_r:
            fh.write(f"{r['lo']:.2f},{r['hi']:.2f},{r['avg_pt']:.4f},"
                     f"{r['frac_pct']:.4f},{r['sig_d']:.4f},{r['sig_d_err']:.4f},{r['sig_s0']:.4f},"
                     f"{r['sig_verify']:.4f},{r['n_d']},{r['n_s']},{r['note']}\n")
    print(f"\nFracs → {out_fracs}")

    # ── ROOT macro ─────────────────────────────────────────────────────────────
    n        = len(valid_r)
    pts      = ", ".join(f"{r['avg_pt']:.4f}"    for r in valid_r)
    sig_d    = ", ".join(f"{r['sig_d']:.4f}"     for r in valid_r)
    sig_d_err= ", ".join(f"{r['sig_d_err']:.4f}" for r in valid_r)
    sig_s0   = ", ".join(f"{r['sig_s0']:.4f}"    for r in valid_r)
    sig_ver  = ", ".join(f"{r['sig_verify']:.4f}"for r in valid_r)
    fracs    = ", ".join(f"{r['frac_pct']:.4f}"  for r in valid_r)
    xerrs    = ", ".join(f"{(r['hi']-r['lo'])/2:.3f}" for r in valid_r)
    zeros    = ", ".join("0" for _ in valid_r)
    pt_lo    = PT_EDGES[0]
    pt_hi    = PT_EDGES[-1]

    macro = f"""// plot_mass_smear_fracs_ks.C
// Visualises mass-width-matched K0S smearing fractions.
// Run with:  root -l -b -q plot_mass_smear_fracs_ks.C

#include <ctime>
#include <sstream>
#include <algorithm>
#include <cmath>

std::string _getDate(){{
    std::time_t t=std::time(0); std::tm* n=std::localtime(&t);
    std::stringstream s;
    s<<(n->tm_mon+1)<<'/'<<n->tm_mday<<'/'<<(n->tm_year+1900);
    return s.str();
}}
void _label(double x1,double y1,double x2,double y2){{
    TPaveText *p=new TPaveText(x1,y1,x2,y2,"NDC");
    p->SetFillStyle(0); p->SetBorderSize(0); p->SetTextFont(42);
    p->AddText("#it{{#bf{{sPHENIX}}}} Internal,  #it{{p}}+#it{{p}}  #sqrt{{s}} = 200 GeV");
    p->Draw();
}}
void _date(double x1,double y1){{
    TLatex l; l.SetNDC(); l.SetTextFont(42); l.SetTextSize(0.035);
    l.SetTextColor(kGray+2); l.DrawLatex(x1,y1,_getDate().c_str());
}}

void plot_mass_smear_fracs_ks() {{
    gStyle->SetOptStat(0); gStyle->SetOptTitle(0);

    const int N = {n};
    double avg_pt[]    = {{{pts}}};
    double sig_data[]  = {{{sig_d}}};
    double sig_derr[]  = {{{sig_d_err}}};
    double sig_sim0[]  = {{{sig_s0}}};
    double sig_check[] = {{{sig_ver}}};
    double frac_pct[]  = {{{fracs}}};
    double xerr[]      = {{{xerrs}}};
    double zero[]      = {{{zeros}}};

    // ── canvas 1: sigma comparison ─────────────────────────────────────────────
    TCanvas *c1 = new TCanvas("c1","K0S mass sigma comparison",900,650);
    c1->SetLeftMargin(0.13); c1->SetBottomMargin(0.13); c1->SetRightMargin(0.06);

    TGraphErrors *gD = new TGraphErrors(N, avg_pt, sig_data, xerr, sig_derr);
    TGraph       *gS = new TGraph(N, avg_pt, sig_sim0);
    TGraph       *gC = new TGraph(N, avg_pt, sig_check);

    gD->SetMarkerStyle(20); gD->SetMarkerSize(1.3);
    gD->SetMarkerColor(kBlack);  gD->SetLineColor(kBlack);  gD->SetLineWidth(2);
    gS->SetMarkerStyle(24); gS->SetMarkerSize(1.3);
    gS->SetMarkerColor(kAzure+7); gS->SetLineColor(kAzure+7); gS->SetLineWidth(2);
    gC->SetMarkerStyle(21); gC->SetMarkerSize(1.3);
    gC->SetMarkerColor(kRed+1);  gC->SetLineColor(kRed+1);  gC->SetLineWidth(2);

    double ymax = 0;
    for(int i=0;i<N;++i) ymax = std::max(ymax, std::max({{sig_data[i]+sig_derr[i], sig_sim0[i]}}));
    ymax *= 1.35;

    TMultiGraph *mg1 = new TMultiGraph();
    mg1->Add(gS,"PL"); mg1->Add(gD,"PE"); mg1->Add(gC,"PL");
    mg1->Draw("A");
    mg1->GetXaxis()->SetTitle("#it{{p}}_{{T}}^{{K_{{S}}^{{0}}}} (GeV/#it{{c}})");
    mg1->GetYaxis()->SetTitle("Gaussian #sigma of K_{{S}}^{{0}} mass (MeV/#it{{c}}^{{2}})");
    mg1->GetYaxis()->SetRangeUser(0, ymax);
    mg1->GetXaxis()->SetTitleSize(0.05); mg1->GetYaxis()->SetTitleSize(0.047);
    mg1->GetYaxis()->SetTitleOffset(1.25);

    TLegend *leg1 = new TLegend(0.14,0.72,0.65,0.87);
    leg1->SetBorderSize(0); leg1->SetFillStyle(0); leg1->SetTextSize(0.033);
    leg1->AddEntry(gD,"Data",                       "lpe");
    leg1->AddEntry(gS,"SV sim (unsmeared)",          "lp");
    leg1->AddEntry(gC,"SV sim (mass-width smeared)","lp");
    leg1->Draw();
    _label(0.14,0.88,0.68,0.96);
    _date(0.72,0.90);

    c1->SaveAs("plot_mass_smear_sigma_comparison.pdf");
    c1->SaveAs("plot_mass_smear_sigma_comparison.png");
    std::cout << "Saved plot_mass_smear_sigma_comparison.pdf/.png\\n";

    // ── canvas 2: optimal fracs vs pT ──────────────────────────────────────────
    TCanvas *c2 = new TCanvas("c2","Mass-width smearing fractions",900,650);
    c2->SetLeftMargin(0.13); c2->SetBottomMargin(0.13); c2->SetRightMargin(0.06);

    TGraphErrors *gF = new TGraphErrors(N, avg_pt, frac_pct, xerr, zero);
    gF->SetMarkerStyle(20); gF->SetMarkerSize(1.4);
    gF->SetMarkerColor(kViolet+1); gF->SetLineColor(kViolet+1); gF->SetLineWidth(2);
    gF->Draw("APE");
    gF->GetXaxis()->SetTitle("#it{{p}}_{{T}}^{{K_{{S}}^{{0}}}} (GeV/#it{{c}})");
    gF->GetYaxis()->SetTitle("Smearing fraction (%)");
    double fmax = *std::max_element(frac_pct,frac_pct+N)*1.4 + 0.2;
    gF->GetYaxis()->SetRangeUser(0, fmax);
    gF->GetXaxis()->SetTitleSize(0.05); gF->GetYaxis()->SetTitleSize(0.05);
    gF->GetYaxis()->SetTitleOffset(1.2);

    _label(0.14,0.88,0.68,0.96);
    _date(0.72,0.90);

    TLegend *leg2 = new TLegend(0.14,0.74,0.60,0.85);
    leg2->SetBorderSize(0); leg2->SetFillStyle(0); leg2->SetTextSize(0.033);
    leg2->AddEntry(gF,"Frac needed to match K_{{S}}^{{0}} mass #sigma","lp");
    leg2->Draw();

    c2->SaveAs("plot_mass_smear_fracs_ks.pdf");
    c2->SaveAs("plot_mass_smear_fracs_ks.png");
    std::cout << "Saved plot_mass_smear_fracs_ks.pdf/.png\\n";
}}
"""
    out_mac = "plot_mass_smear_fracs_ks.C"
    with open(out_mac, "w") as fh:
        fh.write(macro)
    print(f"Macro  → {out_mac}")
    print(f"\nRun:  root -l -b -q {out_mac}")

    # ── summary stats ──────────────────────────────────────────────────────────
    fp = [r["frac_pct"] for r in valid_r]
    if fp:
        print(f"\n  Frac range: {min(fp):.3f}% – {max(fp):.3f}%")
        print(f"  Mean frac:  {np.mean(fp):.3f}%")


if __name__ == "__main__":
    main()
