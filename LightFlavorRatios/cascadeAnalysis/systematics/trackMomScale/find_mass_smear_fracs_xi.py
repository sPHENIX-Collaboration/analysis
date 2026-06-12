#!/usr/bin/env python3
"""
find_mass_smear_fracs_xi.py
--------------------------------
Finds the per-pT-bin proton smearing fraction that makes the Gaussian sigma
of the Ximinus invariant mass peak in smeared SV simulation match the data.

The pion (track_1) smearing is fixed using the K0S mass-matched fracs from
mass_smear_fracs_ks.txt, looked up by parent Xi pT.  Only the proton
(track_2) fraction is scanned.

Method (per Xi pT bin):
  1. Load pion fracs from KS fracs file; fix f_pi = interp(Ximinus_pT).
  2. Fit Gaussian + linear background to data Xi mass → sigma_data (signal only).
  3. Generate one set of N(0,1) draws (fixed seed), reused for all proton
     fracs so sigma(f_pro) is a smooth deterministic curve.
  4. Scan f_pro; for each: smear pion with f_pi, proton with f_pro, recompute
     Xi mass, fit Gaussian → sigma(f_pro).
  5. Interpolate to find f_pro* where sigma(f_pro*) = sigma_data.

Output:
  mass_smear_fracs_xi_proton.txt  — proton fracs (smear_sv_xi.py --proton_fracs)
  plot_mass_smear_fracs_xi.C      — ROOT macro: sigma comparison + frac vs pT

Usage:
    python3 find_mass_smear_fracs_xi.py
"""

import sys
import numpy as np
import uproot
from scipy.optimize import curve_fit

# ── configuration ──────────────────────────────────────────────────────────────
DATA_FILE  = "Ximinus_fullDataset_finalCuts_0p2pTCut_rapidity1p0Cut_BCOcut_charge_.root"
SIM_FILE   = "outputKFParticleXiminusSV_filtered.root"
PION_FRACS = "mass_smear_fracs_ks.txt"   # fixed pion smearing from K0S analysis

PT_EDGES   = [0.5, 0.8, 1.1, 1.4, 1.8, 2.2, 3.0]

PRO_FRAC_SCAN = np.linspace(0.0, 0.60, 241)  # proton scan 0 → 60 %, step 0.25 %
#N_AVG_SEEDS  = 5    # tile sim events this many times for stable σ in low-stat bins
FIT_LO    = 1.30   # GeV
FIT_HI    = 1.34   # GeV
FIT_NBINS = 35
PION_MASS   = 0.13957018   # GeV
PROTON_MASS = 0.93827209   # GeV
RNG_SEED  = 42


# ── load pion fracs ─────────────────────────────────────────────────────────────
def load_fracs(csv_path):
    """Parse fracs CSV (same format as smear_sv_ks.py); return (avg_pt, frac) arrays."""
    pts, fracs = [], []
    avg_col = frc_col = None
    with open(csv_path) as fh:
        for line in fh:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            if line.startswith("bin_lo"):
                cols = line.split(",")
                avg_col = cols.index("avg_pt")
                frc_col = cols.index("best_frac_pct")
                continue
            parts = line.split(",")
            frc = parts[frc_col]
            if frc == "nan":
                continue
            pts.append(float(parts[avg_col]))
            fracs.append(float(frc) / 100.0)
    return np.array(pts, dtype=np.float64), np.array(fracs, dtype=np.float64)


# ── physics helpers ─────────────────────────────────────────────────────────────
def xi_mass(px1, py1, pz1, px2, py2, pz2, px3, py3, pz3):
    """Xi invariant mass: track1 = pion, track2 = proton, track3 = pion."""
    E1 = np.sqrt(px1**2 + py1**2 + pz1**2 + PION_MASS**2)
    E2 = np.sqrt(px2**2 + py2**2 + pz2**2 + PROTON_MASS**2)
    E3 = np.sqrt(px3**2 + py3**2 + pz3**2 + PION_MASS**2)
    m2 = (E1+E2+E3)**2 - (px1+px2+px3)**2 - (py1+py2+py3)**2 - (pz1+pz2+pz3)**2
    return np.sqrt(np.maximum(m2, 0.0))


def smear_p3(px, py, pz, f, z):
    """Scale transverse momentum by (1 + f*z); preserve pz (eta preserved)."""
    scale = 1.0 + f * z
    return px * scale, py * scale, pz


def _gauss(x, A, mu, sigma):
    return A * np.exp(-0.5 * ((x - mu) / sigma)**2)

def _gauss_plus_pol1(x, A, mu, sigma, C, D):
    return A * np.exp(-0.5 * ((x - mu) / sigma)**2) + C + D * x


def fit_sigma_data(masses_gev):
    """Gaussian + linear background fit for data; return (sigma_MeV, err_MeV).
    Uses the full [FIT_LO, FIT_HI] range so the polynomial absorbs combinatoric
    background while the Gaussian component gives the signal width."""
    sel = masses_gev[(masses_gev >= FIT_LO) & (masses_gev < FIT_HI)]
    if len(sel) < 30:
        return np.nan, np.nan
    counts, edges = np.histogram(sel, bins=FIT_NBINS, range=(FIT_LO, FIT_HI))
    cx = 0.5 * (edges[:-1] + edges[1:])
    try:
        imax    = int(np.argmax(counts))
        bg_lvl  = 0.5 * (float(counts[0]) + float(counts[-1]))
        bg_slp  = (float(counts[-1]) - float(counts[0])) / (cx[-1] - cx[0])
        p0 = [float(counts[imax]) - bg_lvl, float(cx[imax]), 0.004,
              bg_lvl, bg_slp]
        popt, pcov = curve_fit(
            _gauss_plus_pol1, cx, counts.astype(float), p0=p0,
            bounds=([0, FIT_LO, 0.0003, -np.inf, -np.inf],
                    [np.inf, FIT_HI, 0.020,  np.inf,  np.inf]),
            maxfev=10000,
        )
        s     = abs(popt[2]) * 1000.0
        s_err = np.sqrt(pcov[2, 2]) * 1000.0 if pcov[2, 2] >= 0 else np.nan
        return s, s_err
    except Exception:
        return np.nan, np.nan


def fit_sigma(masses_gev):
    """Pure Gaussian fit for simulation (no background); return (sigma_MeV, err_MeV)."""
    sel = masses_gev[(masses_gev >= FIT_LO) & (masses_gev < FIT_HI)]
    if len(sel) < 20:
        return np.nan, np.nan
    counts, edges = np.histogram(sel, bins=FIT_NBINS, range=(FIT_LO, FIT_HI))
    cx = 0.5 * (edges[:-1] + edges[1:])
    try:
        imax = int(np.argmax(counts))
        popt, pcov = curve_fit(
            _gauss, cx, counts.astype(float),
            p0=[float(counts[imax]), float(cx[imax]), 0.004],
            bounds=([0, FIT_LO, 0.0005], [np.inf, FIT_HI, 0.030]),
            maxfev=10000,
        )
        s     = abs(popt[2]) * 1000.0
        s_err = np.sqrt(pcov[2, 2]) * 1000.0 if pcov[2, 2] >= 0 else np.nan
        return s, s_err
    except Exception:
        return np.nan, np.nan


# ── main ────────────────────────────────────────────────────────────────────────
def main():
    # ── pion fracs ─────────────────────────────────────────────────────────────
    pi_pts, pi_fracs = load_fracs(PION_FRACS)
    # Anchor to (pT=0, frac=0) so interpolation below the lowest calibration
    # point slopes linearly to zero rather than holding flat.
    pi_pts   = np.concatenate([[0.0], pi_pts])
    pi_fracs = np.concatenate([[0.0], pi_fracs])
    print(f"Pion fracs from {PION_FRACS} (with (0,0) anchor):")
    for p, f in zip(pi_pts, pi_fracs):
        print(f"  avg_pT={p:.4f} GeV  →  {f*100:.4f}%")

    # ── read data ──────────────────────────────────────────────────────────────
    print(f"\nReading {DATA_FILE} ...")
    with uproot.open(DATA_FILE) as f:
        t = f["DecayTree"]
        d_xipt   = t["Ximinus_pT"].array(library="np")
        d_ximass = t["Ximinus_mass"].array(library="np")
    print(f"  {len(d_xipt)} data events")

    # ── read sim ────────────────────────────────────────────────────────────────
    print(f"Reading {SIM_FILE} ...")
    with uproot.open(SIM_FILE) as f:
        t = f["DecayTree"]
        s_xipt = t["Ximinus_pT"].array(library="np")
        s_px1   = t["Lambda0_track_1_px"].array(library="np")
        s_py1   = t["Lambda0_track_1_py"].array(library="np")
        s_pz1   = t["Lambda0_track_1_pz"].array(library="np")
        s_px2   = t["Lambda0_track_2_px"].array(library="np")
        s_py2   = t["Lambda0_track_2_py"].array(library="np")
        s_pz2   = t["Lambda0_track_2_pz"].array(library="np")
        s_px3   = t["track_3_px"].array(library="np")
        s_py3   = t["track_3_py"].array(library="np")
        s_pz3   = t["track_3_pz"].array(library="np")
    s_pt1 = np.sqrt(s_px1**2 + s_py1**2)   # lam pion track pT (computed from stored px,py)
    s_pt3 = np.sqrt(s_px3**2 + s_py3**2)   # bach pion track pT (computed from stored px,py)
    print(f"  {len(s_xipt)} sim events\n")

    results = []
    nbins   = len(PT_EDGES) - 1

    hdr = (f"{'pT bin':>16}  {'N_data':>7}  {'N_sim':>6}  "
           f"{'σ_data':>8}  {'σ_sim':>7}  {'<pT_π>':>7}  {'f_pi%':>6}  {'f_pro%*':>8}  {'σ_verify':>9}")
    print(hdr)
    print("─" * len(hdr))

    for b in range(nbins):
        lo, hi = PT_EDGES[b], PT_EDGES[b+1]

        # data sigma — Gaussian + linear background to account for combinatorics
        dmask            = (d_xipt >= lo) & (d_xipt < hi)
        sig_d, sig_d_err = fit_sigma_data(d_ximass[dmask])
        n_d              = int(dmask.sum())

        # sim events in bin
        smask     = (s_xipt >= lo) & (s_xipt < hi)
        n_s       = int(smask.sum())
        avg_pt    = float(s_xipt[smask].mean()) if n_s > 0 else 0.5*(lo+hi)
        avg_pi1_pt = float(s_pt1[smask].mean())   if n_s > 0 else 0.0
        avg_pi3_pt = float(s_pt3[smask].mean())   if n_s > 0 else 0.0

        if n_d < 20 or n_s < 20 or np.isnan(sig_d):
            print(f"  [{lo:.1f},{hi:.1f})  skipped (data={n_d}, sim={n_s})")
            results.append(dict(lo=lo, hi=hi, avg_pt=avg_pt,
                                sig_d=np.nan, sig_d_err=np.nan, sig_s0=np.nan,
                                f_pi1_pct=np.nan, f_pi3_pct=np.nan, frac_pct=np.nan,
                                sig_verify=np.nan, n_d=n_d, n_s=n_s, note="skipped"))
            continue

        px1 = s_px1[smask]; py1 = s_py1[smask]; pz1 = s_pz1[smask]
        px2 = s_px2[smask]; py2 = s_py2[smask]; pz2 = s_pz2[smask]
        px3 = s_px3[smask]; py3 = s_py3[smask]; pz3 = s_pz3[smask]

        # pion frac looked up by mean pion track pT in this bin
        f_pi1 = float(np.interp(avg_pi1_pt, pi_pts, pi_fracs))
        f_pi3 = float(np.interp(avg_pi3_pt, pi_pts, pi_fracs))

        # generate random draws once; reuse for all proton fracs
        rng    = np.random.default_rng(RNG_SEED)
        z1     = rng.standard_normal(n_s)   # lam pion draws (fixed)
        z3     = rng.standard_normal(n_s)   # bach pion draws (fixed)
        z2_base= rng.standard_normal(n_s)   # proton draws (fixed, reused)

        # smear pion tracks once (frac is fixed for this bin)
        px1s, py1s, pz1s = smear_p3(px1, py1, pz1, f_pi1, z1)
        px3s, py3s, pz3s = smear_p3(px3, py3, pz3, f_pi3, z3)

        # scan proton frac
        scan_s = []
        for f_pro in PRO_FRAC_SCAN:
            px2s, py2s, pz2s = smear_p3(px2, py2, pz2, f_pro, z2_base)
            ms   = xi_mass(px1s, py1s, pz1s, px2s, py2s, pz2s, px3s, py3s, pz3s)
            s, _ = fit_sigma(ms)
            scan_s.append(s)
        scan_s = np.array(scan_s)
        sig_s0 = float(scan_s[0])  # sigma with pion smeared, proton unsmeared

        # interpolate to find optimal proton frac
        valid = np.isfinite(scan_s)
        if not valid.any():
            frac_opt = np.nan
            note = "scan_failed"
        elif sig_d <= scan_s[valid][0]:
            frac_opt = 0.0
            note = "no_proton_smear_needed"
        elif sig_d >= scan_s[valid][-1]:
            frac_opt = float(PRO_FRAC_SCAN[valid][-1])
            note = "exceeded_range"
            print(f"  WARNING [{lo:.1f},{hi:.1f}): σ_data={sig_d:.1f} > "
                  f"σ_max={scan_s[valid][-1]:.1f}; try increasing PRO_FRAC_SCAN max")
        else:
            frac_opt = float(np.interp(sig_d, scan_s[valid], PRO_FRAC_SCAN[valid]))
            note = "ok"

        # verify with independent draws
        if not np.isnan(frac_opt):
            rng2 = np.random.default_rng(RNG_SEED + 1)
            z1v  = rng2.standard_normal(n_s)
            z2v  = rng2.standard_normal(n_s)
            z3v  = rng2.standard_normal(n_s)
            px1v, py1v, pz1v = smear_p3(px1, py1, pz1, f_pi1, z1v)
            px2v, py2v, pz2v = smear_p3(px2, py2, pz2, frac_opt, z2v)
            px3v, py3v, pz3v = smear_p3(px3, py3, pz3, f_pi3, z3v)
            mv = xi_mass(px1v, py1v, pz1v, px2v, py2v, pz2v, px3v, py3v, pz3v)
            sig_verify, _ = fit_sigma(mv)
        else:
            sig_verify = np.nan

        frac_pct = frac_opt * 100.0 if not np.isnan(frac_opt) else np.nan

        results.append(dict(
            lo=lo, hi=hi, avg_pt=avg_pt, avg_pi1_pt=avg_pi1_pt, avg_pi3_pt=avg_pi3_pt,
            sig_d=sig_d, sig_d_err=sig_d_err,
            sig_s0=sig_s0, f_pi1_pct=f_pi1*100, f_pi3_pct=f_pi3*100,
            frac_pct=frac_pct, sig_verify=sig_verify,
            n_d=n_d, n_s=n_s, note=note,
        ))

        v_str = f"{sig_verify:>8.2f}" if not np.isnan(sig_verify) else "     n/a"
        print(f"  [{lo:.1f},{hi:.1f})  {n_d:>7}  {n_s:>6}  "
              f"{sig_d:>6.2f}±{sig_d_err:<4.2f}  {sig_s0:>6.2f}  "
              f"{avg_pi1_pt:>6.3f} {avg_pi3_pt:>6.3f}  {f_pi1*100:>5.2f}%  {f_pi3*100:>5.2f}% {frac_pct:>7.3f}%  {v_str}")

    # ── write proton fracs file ────────────────────────────────────────────────
    valid_r   = [r for r in results if not np.isnan(r["frac_pct"])]
    out_fracs = "mass_smear_fracs_xi_proton.txt"
    with open(out_fracs, "w") as fh:
        fh.write("# Auto-generated by find_mass_smear_fracs_xi.py\n")
        fh.write(f"# Proton fracs from Xi mass-width matching; pion fracs fixed from {PION_FRACS}\n")
        fh.write("bin_lo,bin_hi,avg_pt,avg_pi1_pt,avg_pi3_pt,best_frac_pct,sig_data_mev,sig_data_err_mev,sig_sim0_mev,sig_verify_mev,f_pi1_pct,f_pi3_pct,n_data,n_sim,note\n")
        for r in valid_r:
            fh.write(f"{r['lo']:.2f},{r['hi']:.2f},{r['avg_pt']:.4f},{r['avg_pi1_pt']:.4f},{r['avg_pi3_pt']:.4f},"
                     f"{r['frac_pct']:.4f},{r['sig_d']:.4f},{r['sig_d_err']:.4f},{r['sig_s0']:.4f},"
                     f"{r['sig_verify']:.4f},{r['f_pi1_pct']:.4f},{r['f_pi3_pct']:.4f},"
                     f"{r['n_d']},{r['n_s']},{r['note']}\n")
    print(f"\nProton fracs → {out_fracs}")

    # ── ROOT macro ─────────────────────────────────────────────────────────────
    n        = len(valid_r)
    pts      = ", ".join(f"{r['avg_pt']:.4f}"     for r in valid_r)
    sig_d_v  = ", ".join(f"{r['sig_d']:.4f}"      for r in valid_r)
    sig_derr = ", ".join(f"{r['sig_d_err']:.4f}"  for r in valid_r)
    sig_s0_v = ", ".join(f"{r['sig_s0']:.4f}"     for r in valid_r)
    sig_ver  = ", ".join(f"{r['sig_verify']:.4f}" for r in valid_r)
    fracs    = ", ".join(f"{r['frac_pct']:.4f}"   for r in valid_r)
    xerrs    = ", ".join(f"{(r['hi']-r['lo'])/2:.3f}" for r in valid_r)
    zeros    = ", ".join("0" for _ in valid_r)
    pt_lo    = PT_EDGES[0]; pt_hi = PT_EDGES[-1]

    macro = f"""// plot_mass_smear_fracs_xi.C
// Xi mass-width-matched proton smearing fractions.
// Pion fracs fixed from K0S analysis; only proton frac was scanned.
// Run with:  root -l -b -q plot_mass_smear_fracs_xi.C

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

void plot_mass_smear_fracs_xi() {{
    gStyle->SetOptStat(0); gStyle->SetOptTitle(0);

    const int N = {n};
    double avg_pt[]    = {{{pts}}};
    double sig_data[]  = {{{sig_d_v}}};
    double sig_derr[]  = {{{sig_derr}}};
    double sig_sim0[]  = {{{sig_s0_v}}};
    double sig_check[] = {{{sig_ver}}};
    double frac_pct[]  = {{{fracs}}};
    double xerr[]      = {{{xerrs}}};
    double zero[]      = {{{zeros}}};

    // ── canvas 1: sigma comparison ─────────────────────────────────────────────
    TCanvas *c1 = new TCanvas("c1","Xi mass sigma comparison",900,650);
    c1->SetLeftMargin(0.13); c1->SetBottomMargin(0.13); c1->SetRightMargin(0.06);

    TGraphErrors *gD = new TGraphErrors(N, avg_pt, sig_data, xerr, sig_derr);
    TGraph       *gS = new TGraph(N, avg_pt, sig_sim0);
    TGraph       *gC = new TGraph(N, avg_pt, sig_check);

    gD->SetMarkerStyle(20); gD->SetMarkerSize(1.3);
    gD->SetMarkerColor(kBlack);   gD->SetLineColor(kBlack);   gD->SetLineWidth(2);
    gS->SetMarkerStyle(24); gS->SetMarkerSize(1.3);
    gS->SetMarkerColor(kAzure+7); gS->SetLineColor(kAzure+7); gS->SetLineWidth(2);
    gC->SetMarkerStyle(21); gC->SetMarkerSize(1.3);
    gC->SetMarkerColor(kRed+1);   gC->SetLineColor(kRed+1);   gC->SetLineWidth(2);

    double ymax = 0;
    for(int i=0;i<N;++i) ymax=std::max(ymax,std::max(sig_data[i]+sig_derr[i],sig_sim0[i]));
    ymax *= 1.35;

    TMultiGraph *mg1 = new TMultiGraph();
    mg1->Add(gS,"PL"); mg1->Add(gD,"PE"); mg1->Add(gC,"PL");
    mg1->Draw("A");
    mg1->GetXaxis()->SetTitle("#it{{p}}_{{T}}^{{#Xi^{{-}}}} (GeV/#it{{c}})");
    mg1->GetYaxis()->SetTitle("Gaussian #sigma of #Xi^{{-}} mass (MeV/#it{{c}}^{{2}})");
    mg1->GetYaxis()->SetRangeUser(0, ymax);
    mg1->GetXaxis()->SetTitleSize(0.05); mg1->GetYaxis()->SetTitleSize(0.045);
    mg1->GetYaxis()->SetTitleOffset(1.30);

    TLegend *leg1 = new TLegend(0.14,0.72,0.70,0.87);
    leg1->SetBorderSize(0); leg1->SetFillStyle(0); leg1->SetTextSize(0.033);
    leg1->AddEntry(gD,"Data",                             "lpe");
    leg1->AddEntry(gS,"SV sim (pion smeared only)",       "lp");
    leg1->AddEntry(gC,"SV sim (pion + proton smeared)",   "lp");
    leg1->Draw();
    _label(0.14,0.88,0.68,0.96);
    _date(0.72,0.90);

    c1->SaveAs("plot_mass_smear_sigma_xi.pdf");
    c1->SaveAs("plot_mass_smear_sigma_xi.png");
    std::cout << "Saved plot_mass_smear_sigma_xi.pdf/.png\\n";

    // ── canvas 2: proton fracs vs pT ───────────────────────────────────────────
    TCanvas *c2 = new TCanvas("c2","Xi proton smearing fractions",900,650);
    c2->SetLeftMargin(0.13); c2->SetBottomMargin(0.13); c2->SetRightMargin(0.06);

    TGraphErrors *gF = new TGraphErrors(N, avg_pt, frac_pct, xerr, zero);
    gF->SetMarkerStyle(20); gF->SetMarkerSize(1.4);
    gF->SetMarkerColor(kOrange+1); gF->SetLineColor(kOrange+1); gF->SetLineWidth(2);
    gF->Draw("APE");
    gF->GetXaxis()->SetTitle("#it{{p}}_{{T}}^{{#Xi^{{-}}}} (GeV/#it{{c}})");
    gF->GetYaxis()->SetTitle("Proton smearing fraction (%)");
    double fmax = *std::max_element(frac_pct,frac_pct+N)*1.4 + 0.5;
    gF->GetYaxis()->SetRangeUser(0, fmax);
    gF->GetXaxis()->SetTitleSize(0.05); gF->GetYaxis()->SetTitleSize(0.05);
    gF->GetYaxis()->SetTitleOffset(1.2);

    _label(0.14,0.88,0.68,0.96);
    _date(0.72,0.90);

    TLegend *leg2 = new TLegend(0.14,0.74,0.65,0.85);
    leg2->SetBorderSize(0); leg2->SetFillStyle(0); leg2->SetTextSize(0.033);
    leg2->AddEntry(gF,"Proton frac to match #Xi^{{-}} mass #sigma (pion fixed from K_{{S}}^{{0}})","lp");
    leg2->Draw();

    c2->SaveAs("plot_mass_smear_fracs_xi.pdf");
    c2->SaveAs("plot_mass_smear_fracs_xi.png");
    std::cout << "Saved plot_mass_smear_fracs_xi.pdf/.png\\n";
}}
"""
    out_mac = "plot_mass_smear_fracs_xi.C"
    with open(out_mac, "w") as fh:
        fh.write(macro)
    print(f"Macro  → {out_mac}")
    print(f"\nRun:  root -l -b -q {out_mac}")

    fp = [r["frac_pct"] for r in valid_r]
    if fp:
        print(f"\n  Proton frac range: {min(fp):.3f}% – {max(fp):.3f}%")
        print(f"  Mean proton frac:  {np.mean(fp):.3f}%")


if __name__ == "__main__":
    main()
