#!/usr/bin/env python3
"""
find_sv_smear_fracs.py
----------------------
Finds per-pT-bin Gaussian smearing fractions for the SV-extrapolated simulation
files so that the smeared SV-sim invariant mass from daughters matches the data
KFP mass width.

Why this is the correct comparison
-----------------------------------
  • SV-sim stores momenta at the secondary vertex, which are essentially the
    KFP-refitted values → inv mass from SV daughters ≈ KFP mass in sim.
  • Data KFP mass is the definitive measured width.
  • Both sides use the same (post-KFP / SV) momenta, so smearing the SV-sim
    daughters to reach the data KFP sigma is a clean, apples-to-apples match.

Two-step procedure
------------------
  Step 1  K0S → π⁺π⁻
    Both daughters are pions; smear each by the same fraction f per pT bin.
    Target: σ_data_KFP  (data K_S0_mass width).

  Step 2  Λ → pπ⁻
    Fix pion smearing from Step 1 (per pion-pT lookup).
    Vary proton fraction f_p per proton-pT bin.
    Target: σ_data_KFP  (data Lambda0_mass width).

Outputs
-------
  optimal_sv_smear_fracs.txt      K0S pion fracs (CSV)
  optimal_sv_xi_fracs.txt     Lambda proton fracs (CSV)
  plot_sv_ks_result.C             ROOT macro — K0S sigma vs pT
  plot_sv_xi_result.C         ROOT macro — Lambda sigma vs pT
"""

import argparse, sys
import numpy as np
import uproot
from scipy.optimize import brentq, curve_fit

# ── files ─────────────────────────────────────────────────────────────────────
KS_DATA_FILE  = "output_Kshort_run3pp_ana530_2025p009_v001.root"
KS_SV_FILE    = "outputKFParticleKShortRecoSV.root"
XI_DATA_FILE = "Ximinus_fullDataset_finalCuts_0p2pTCut_rapidity1p0Cut_BCOcut_charge_.root"
XI_SV_FILE   = "outputKFParticleXiminusSV.root"
TREE_NAME     = "DecayTree"

# ── pT bins ───────────────────────────────────────────────────────────────────
KS_PT_BINS  = [0.2, 0.25, 0.3, 0.4, 0.5, 0.6, 0.8, 1.0, 1e9]
XI_PT_BINS = [0.3, 0.5, 0.75, 1.0, 1.5, 1e9]   # proton pT

# ── mass fit windows ──────────────────────────────────────────────────────────
KS_LO,  KS_HI,  KS_BINS  = 460.0, 540.0,  80
XI_LO, XI_HI, XI_BINS = 1300.0, 1340.0, 80

# ── constants ─────────────────────────────────────────────────────────────────
PION_MASS   = 0.13957
PROTON_MASS = 0.938272
RNG_SEED    = 42

# ── quality-cut branch lists ──────────────────────────────────────────────────
KS_CUT_BR = [
    "track_1_MVTX_nStates", "track_2_MVTX_nStates",
    "track_1_INTT_nStates",  "track_2_INTT_nStates",
    "track_1_TPC_nStates",   "track_2_TPC_nStates",
    "track_1_chi2", "track_1_nDoF", "track_2_chi2", "track_2_nDoF",
    "track_1_IP_xy", "track_2_IP_xy",
    "track_1_track_2_DCA", "track_1_track_2_DCA_xy",
    "K_S0_DIRA", "K_S0_chi2", "K_S0_nDoF",
]

XI_CUT_BR = [
    "Lambda0_track_1_MVTX_nStates", "Lambda0_track_2_MVTX_nStates",
    "Lambda0_track_1_INTT_nStates",  "Lambda0_track_2_INTT_nStates",
    "Lambda0_track_1_TPC_nStates",   "Lambda0_track_2_TPC_nStates",
    "track_3_MVTX_nStates", "track_3_INTT_nStates", "track_3_TPC_nStates",
    "Lambda0_track_1_chi2", "Lambda0_track_1_nDoF", "Lambda0_track_2_chi2", "Lambda0_track_2_nDoF",
    "track_3_chi2", "track_3_nDoF",
    "track_1_track_2_DCA", "track_1_track_3_DCA", "track_2_track_3_DCA",
    "Ximinus_decayLength_xy", "Ximinus_chi2", "Ximinus_nDoF",
    "Lambda0_mass", "Lambda0_decayLength_xy", "Lambda0_chi2", "Lambda0_nDoF",
]

# ── physics helpers ───────────────────────────────────────────────────────────

def ks_mass_mev(px1, py1, pz1, px2, py2, pz2):
    E1 = np.sqrt(px1**2 + py1**2 + pz1**2 + PION_MASS**2)
    E2 = np.sqrt(px2**2 + py2**2 + pz2**2 + PION_MASS**2)
    m2 = (E1+E2)**2 - (px1+px2)**2 - (py1+py2)**2 - (pz1+pz2)**2
    return np.sqrt(np.maximum(m2, 0.0)) * 1000.0

def xi_mass_mev(px_p, py_p, pz_p, px_pi1, py_pi1, pz_pi1, px_pi2, py_pi2, pz_pi2):
    Ep  = np.sqrt(px_p**2  + py_p**2  + pz_p**2  + PROTON_MASS**2)
    Epi1 = np.sqrt(px_pi1**2 + py_pi1**2 + pz_pi1**2 + PION_MASS**2)
    Epi2 = np.sqrt(px_pi2**2 + py_pi2**2 + pz_pi2**2 + PION_MASS**2)
    m2  = (Ep+Epi1+Epi2)**2 - (px_p+px_pi1+px_pi2)**2 - (py_p+py_pi1+px_pi2)**2 - (pz_p+pz_pi1+pz_pi2)**2
    return np.sqrt(np.maximum(m2, 0.0)) * 1000.0

def apply_smear(px, py, pz, frac, z):
    """Smear pT by frac × pT × z (same z draws → smooth σ(frac))."""
    pt   = np.sqrt(px**2 + py**2)
    phi  = np.arctan2(py, px)
    ptot = np.sqrt(pz**2 + pt**2)
    eta  = np.arctanh(np.clip(pz / ptot, -1+1e-9, 1-1e-9))
    spt  = pt + frac * pt * z
    return spt*np.cos(phi), spt*np.sin(phi), spt*np.sinh(eta)

def apply_smear_pervec(px, py, pz, fracs, z):
    """Same but fracs is a per-event array."""
    pt   = np.sqrt(px**2 + py**2)
    phi  = np.arctan2(py, px)
    ptot = np.sqrt(pz**2 + pt**2)
    eta  = np.arctanh(np.clip(pz / ptot, -1+1e-9, 1-1e-9))
    spt  = pt + fracs * pt * z
    return spt*np.cos(phi), spt*np.sin(phi), spt*np.sinh(eta)


# ── quality cut masks ─────────────────────────────────────────────────────────

def ks_cut_mask(d):
    return (
        (np.minimum(d["track_1_MVTX_nStates"], d["track_2_MVTX_nStates"]) > 0) &
        (np.minimum(d["track_1_INTT_nStates"],  d["track_2_INTT_nStates"]) > 0) &
        (np.minimum(d["track_1_TPC_nStates"],   d["track_2_TPC_nStates"])  >= 20) &
        (np.maximum(d["track_1_chi2"]/d["track_1_nDoF"],
                    d["track_2_chi2"]/d["track_2_nDoF"]) <= 300) &
        (np.minimum(np.abs(d["track_1_IP_xy"]),
                    np.abs(d["track_2_IP_xy"])) >= 0.05) &
        (d["track_1_track_2_DCA"]    <= 0.5) &
        (d["track_1_track_2_DCA_xy"] <= 1.0) &
        (d["K_S0_DIRA"]              >= 0.99) &
        (d["K_S0_chi2"] / d["K_S0_nDoF"] <= 20)
    )

def xi_cut_mask(d):
    return (
        (np.minimum(d["Lambda0_track_1_MVTX_nStates"], d["Lambda0_track_2_MVTX_nStates"], d["track_3_MVTX_nStates"]) > 0) &
        (np.minimum(d["Lambda0_track_1_INTT_nStates"], d["Lambda0_track_2_INTT_nStates"], d["track_3_INTT_nStates"]) > 0) &
        (np.minimum(d["Lambda0_track_1_TPC_nStates"], d["Lambda0_track_2_TPC_nStates"], d["track_3_TPC_nStates"]) >= 20) &
        (np.maximum(d["Lambda0_track_1_chi2"]/d["Lambda0_track_1_nDoF"],
                    d["Lambda0_track_2_chi2"]/d["Lambda0_track_2_nDoF"],
                    d["track_3_chi2"]/d["track_3_nDoF"]) <= 400) &
        (np.maximum(d["track_1_track_2_DCA"],
                    d["track_1_track_3_DCA"],
                    d["track_2_track_3_DCA"]) <= 0.5) &
        (d["Ximinus_chi2"]/d["Ximinus_nDoF"] <= 50) &
        (d["Lambda0_chi2"]/d["Lambda0_nDoF"] <= 50) &
        (np.abs(d["Lambda0_mass"] - 1.1157) <= 0.01) &
        (d["Ximinus_decayLength_xy"] >= 0.15) &
        (d["Lambda0_decayLength_xy"] >= 0.01)
    )

# ── Gaussian fit ──────────────────────────────────────────────────────────────

def _gauss(x, amp, mu, sig):
    return amp * np.exp(-0.5 * ((x - mu) / sig)**2)

def fit_sigma(masses_mev, lo, hi, nbins):
    counts, edges = np.histogram(masses_mev, bins=nbins, range=(lo, hi))
    if counts.sum() < 20:
        return np.nan, np.nan
    ctrs = 0.5*(edges[:-1]+edges[1:])
    pk   = np.argmax(counts)
    try:
        popt, _ = curve_fit(_gauss, ctrs, counts.astype(float),
                            p0=[counts[pk], ctrs[pk], 5.0],
                            bounds=([0, lo, 0.5], [1e9, hi, 60.0]))
        return popt[1], abs(popt[2])
    except Exception:
        return np.nan, np.nan

def fit_ks(m):  return fit_sigma(m, KS_LO,  KS_HI,  KS_BINS)
def fit_xi(m): return fit_sigma(m, XI_LO, XI_HI, XI_BINS)


# ── pion-frac lookup (built from K0S results) ─────────────────────────────────

def make_pion_lookup(edges, sigmas):
    """Return a function pt_arr → per-event frac array."""
    edges_arr  = np.array(edges)
    sigmas_arr = np.array(sigmas)
    def lookup(pt_arr):
        idx = np.searchsorted(edges_arr, pt_arr, side='right') - 1
        idx = np.clip(idx, 0, len(sigmas_arr)-1)
        return sigmas_arr[idx]
    return lookup


# ─────────────────────────────────────────────────────────────────────────────
# Step 1: K0S pion smearing
# ─────────────────────────────────────────────────────────────────────────────

def run_ks(frac_max):
    print("=" * 68)
    print("STEP 1 — K0S → π⁺π⁻  (SV sim inv mass → data KFP mass)")
    print("=" * 68)

    # load data
    print(f"\nLoading KS DATA : {KS_DATA_FILE}")
    ks_phys = ["track_1_px","track_1_py","track_1_pz","track_1_pT",
               "track_2_px","track_2_py","track_2_pz","track_2_pT","K_S0_mass"]
    with uproot.open(KS_DATA_FILE) as f:
        d = f[TREE_NAME].arrays(ks_phys + KS_CUT_BR, library="np")
    dm = ks_cut_mask(d)
    print(f"  {dm.sum():,} / {len(dm):,} pass quality cuts")
    d = {k: v[dm] for k, v in d.items()}
    d_pt1 = d["track_1_pT"].astype(np.float64)
    d_pt2 = d["track_2_pT"].astype(np.float64)
    d_kfp = d["K_S0_mass"].astype(np.float64) * 1000.0

    # load SV sim
    print(f"Loading KS SV sim: {KS_SV_FILE}")
    ks_sim_phys = ks_phys + ["track_1_PDG_ID","track_2_PDG_ID"]
    with uproot.open(KS_SV_FILE) as f:
        s = f[TREE_NAME].arrays(ks_sim_phys + KS_CUT_BR, library="np")
    sm = (ks_cut_mask(s) &
          (np.abs(s["track_1_PDG_ID"])==211) &
          (np.abs(s["track_2_PDG_ID"])==211))
    print(f"  {sm.sum():,} / {len(sm):,} pass quality + pion-PDG cuts\n")
    s = {k: v[sm] for k, v in s.items()}
    s_pt1 = s["track_1_pT"].astype(np.float64)
    s_pt2 = s["track_2_pT"].astype(np.float64)
    s_px1 = s["track_1_px"].astype(np.float64)
    s_py1 = s["track_1_py"].astype(np.float64)
    s_pz1 = s["track_1_pz"].astype(np.float64)
    s_px2 = s["track_2_px"].astype(np.float64)
    s_py2 = s["track_2_py"].astype(np.float64)
    s_pz2 = s["track_2_pz"].astype(np.float64)

    nBins   = len(KS_PT_BINS) - 1
    results = []

    HDR = (f"{'Bin':>3}  {'pT range':>18}  "
           f"{'σ_data_KFP':>11}  {'σ_sv_inv0':>10}  "
           f"{'best%':>7}  {'σ_matched':>10}")
    print(HDR)
    print("─" * len(HDR.rstrip()))

    for i in range(nBins):
        lo, hi = KS_PT_BINS[i], KS_PT_BINS[i+1]
        hi_s   = f"{hi:.3f}" if hi < 1e8 else "∞      "

        dm_b = (np.minimum(d_pt1,d_pt2)>=lo) & (np.maximum(d_pt1,d_pt2)<hi)
        sm_b = (np.minimum(s_pt1,s_pt2)>=lo) & (np.maximum(s_pt1,s_pt2)<hi)
        n_d, n_s = dm_b.sum(), sm_b.sum()
        avg_pt = float((d_pt1[dm_b].mean()+d_pt2[dm_b].mean())/2) if n_d>0 else (lo+hi)/2

        _, sig_data_kfp = fit_ks(d_kfp[dm_b])
        px1_b = s_px1[sm_b]; py1_b = s_py1[sm_b]; pz1_b = s_pz1[sm_b]
        px2_b = s_px2[sm_b]; py2_b = s_py2[sm_b]; pz2_b = s_pz2[sm_b]
        sv_inv0 = ks_mass_mev(px1_b,py1_b,pz1_b, px2_b,py2_b,pz2_b)
        _, sig_sv_inv0 = fit_ks(sv_inv0)

        def _bad(note):
            results.append(dict(lo=lo, hi=hi, frac=np.nan, avg_pt=avg_pt,
                                sig_data_kfp=sig_data_kfp, sig_sv_inv0=sig_sv_inv0,
                                sig_match=np.nan, note=note, n_d=n_d, n_s=n_s))

        if n_d < 20 or np.isnan(sig_data_kfp):
            _bad("too few data"); print(f"{i:>3}  [{lo:.3f},{hi_s})  too few data"); continue
        if n_s < 20 or np.isnan(sig_sv_inv0):
            _bad("too few sim");  print(f"{i:>3}  [{lo:.3f},{hi_s})  too few sim");  continue

        rng = np.random.default_rng(RNG_SEED + i)
        z1  = rng.standard_normal(n_s)
        z2  = rng.standard_normal(n_s)

        def sim_sigma(frac):
            spx1,spy1,spz1 = apply_smear(px1_b,py1_b,pz1_b,frac,z1)
            spx2,spy2,spz2 = apply_smear(px2_b,py2_b,pz2_b,frac,z2)
            _, sig = fit_ks(ks_mass_mev(spx1,spy1,spz1, spx2,spy2,spz2))
            return sig

        def residual(frac): return sim_sigma(frac) - sig_data_kfp

        r0 = residual(0.0)
        if r0 >= 0:
            note = "sim≥data at 0%"
            results.append(dict(lo=lo, hi=hi, frac=0.0, avg_pt=avg_pt,
                                sig_data_kfp=sig_data_kfp, sig_sv_inv0=sig_sv_inv0,
                                sig_match=sig_sv_inv0, note=note, n_d=n_d, n_s=n_s))
            print(f"{i:>3}  [{lo:.3f},{hi_s})  {sig_data_kfp:>11.3f}  "
                  f"{sig_sv_inv0:>10.3f}  {'0.00%':>7}  {note}")
            continue

        r_max = residual(frac_max)
        if r_max < 0:
            sig_at_max = sim_sigma(frac_max)
            note = f">={frac_max*100:.0f}%"
            results.append(dict(lo=lo, hi=hi, frac=frac_max, avg_pt=avg_pt,
                                sig_data_kfp=sig_data_kfp, sig_sv_inv0=sig_sv_inv0,
                                sig_match=sig_at_max, note=note, n_d=n_d, n_s=n_s))
            print(f"{i:>3}  [{lo:.3f},{hi_s})  {sig_data_kfp:>11.3f}  "
                  f"{sig_sv_inv0:>10.3f}  {note:>7}  {sig_at_max:>10.3f}")
            continue

        best = brentq(residual, 0.0, frac_max, xtol=5e-5, maxiter=80)
        sig_m = sim_sigma(best)
        results.append(dict(lo=lo, hi=hi, frac=best, avg_pt=avg_pt,
                            sig_data_kfp=sig_data_kfp, sig_sv_inv0=sig_sv_inv0,
                            sig_match=sig_m, note="ok", n_d=n_d, n_s=n_s))
        print(f"{i:>3}  [{lo:.3f},{hi_s})  {sig_data_kfp:>11.3f}  "
              f"{sig_sv_inv0:>10.3f}  {best*100:>6.2f}%  {sig_m:>10.3f}")

    valid = [r for r in results if not np.isnan(r["frac"])]
    edges_cpp  = ", ".join(f"{r['lo']:.4f}" for r in valid)
    sigmas_cpp = ", ".join(f"{r['frac']:.5f}" for r in valid)
    print(f"\n{'─'*68}")
    print("K0S pion fracs (C++ for smear_KS_decaytree_ptbin.C):\n")
    print(f"  pt_bin_edges  = {{{edges_cpp}}}")
    print(f"  pt_bin_sigmas = {{{sigmas_cpp}}}")

    with open("optimal_sv_smear_fracs.txt","w") as fh:
        fh.write("# Auto-generated by find_sv_smear_fracs.py\n")
        fh.write("# SV sim inv mass smeared to match data KFP mass.\n")
        fh.write(f"# pt_bin_edges  = {{{edges_cpp}}}\n")
        fh.write(f"# pt_bin_sigmas = {{{sigmas_cpp}}}\n\n")
        fh.write("bin_lo,bin_hi,avg_pt,best_frac_pct,sig_data_kfp,sig_sv_inv0,sig_match,n_data,n_sim,note\n")
        for r in results:
            hi_w = f"{r['hi']:.4f}" if r["hi"]<1e8 else "inf"
            _f   = lambda v: f"{v:.4f}" if not np.isnan(v) else "nan"
            fp   = f"{r['frac']*100:.4f}" if not np.isnan(r["frac"]) else "nan"
            fh.write(f"{r['lo']:.4f},{hi_w},{r['avg_pt']:.4f},{fp},{_f(r['sig_data_kfp'])},"
                     f"{_f(r['sig_sv_inv0'])},{_f(r['sig_match'])},"
                     f"{r['n_d']},{r['n_s']},{r['note']}\n")
    print(f"\nCSV saved → optimal_sv_smear_fracs.txt")

    return results

# ─────────────────────────────────────────────────────────────────────────────
# Step 2: Lambda proton smearing
# ─────────────────────────────────────────────────────────────────────────────

def run_xi(ks_results, frac_max):
    print("\n" + "=" * 68)
    print("STEP 2 — Xi → ppipi  (SV sim inv mass → data KFP mass)")
    print("=" * 68)

    # Build pion lookup from K0S results
    valid_ks  = [r for r in ks_results if not np.isnan(r["frac"])]
    pi_edges  = [r["lo"] for r in valid_ks]
    pi_sigmas = [r["frac"] for r in valid_ks]
    pion_frac = make_pion_lookup(pi_edges, pi_sigmas)
    print(f"\nPion smearing calibration from K0S SV results:")
    for lo, frac in zip(pi_edges, pi_sigmas):
        print(f"  pT >= {lo:.3f} GeV  →  {frac*100:.3f}%")
    print(f"  (below {pi_edges[0]:.2f} GeV → {pi_sigmas[0]*100:.3f}%  [lowest bin])")

    # load Lambda data
    print(f"\nLoading Xi DATA : {XI_DATA_FILE}")
    xi_phys = ["Lambda0_track_1_px","Lambda0_track_1_py","Lambda0_track_1_pz","Lambda0_track_1_pT",
                "Lambda0_track_2_px","Lambda0_track_2_py","Lambda0_track_2_pz","Lambda0_track_2_pT",
                "track_3_px","track_3_py","track_3_pz","track_3_pT",
                "Ximinus_mass","Lambda0_track_1_PDG_ID","Lambda0_track_2_PDG_ID","track_3_PDG_ID"]
    with uproot.open(XI_DATA_FILE) as f:
        d = f[TREE_NAME].arrays(xi_phys + XI_CUT_BR, library="np")
    dm = (xi_cut_mask(d) &
          (np.abs(d["Lambda0_track_1_PDG_ID"])==211) &
          (np.abs(d["Lambda0_track_2_PDG_ID"])==2212) &
          (np.abs(d["track_3_PDG_ID"])==211))
    print(f"  {dm.sum():,} / {len(dm):,} pass quality + PDG cuts")
    d = {k: v[dm] for k, v in d.items()}
    d_p_pt  = d["Lambda0_track_2_pT"].astype(np.float64)
    d_kfp   = d["Ximinus_mass"].astype(np.float64) * 1000.0

    # load Lambda SV sim
    print(f"Loading Xi SV sim: {XI_SV_FILE}")
    with uproot.open(XI_SV_FILE) as f:
        s = f[TREE_NAME].arrays(xi_phys + XI_CUT_BR, library="np")
    sm = (xi_cut_mask(s) &
          (np.abs(s["Lambda0_track_1_PDG_ID"])==211) &
          (np.abs(s["Lambda0_track_2_PDG_ID"])==2212) &
          (np.abs(s["track_3_PDG_ID"])==211))
    print(f"  {sm.sum():,} / {len(sm):,} pass quality + PDG cuts\n")
    s = {k: v[sm] for k, v in s.items()}
    s_pi1_pt = s["Lambda0_track_1_pT"].astype(np.float64)
    s_pi1_px = s["Lambda0_track_1_px"].astype(np.float64)
    s_pi1_py = s["Lambda0_track_1_py"].astype(np.float64)
    s_pi1_pz = s["Lambda0_track_1_pz"].astype(np.float64)
    s_p_pt  = s["Lambda0_track_2_pT"].astype(np.float64)
    s_p_px  = s["Lambda0_track_2_px"].astype(np.float64)
    s_p_py  = s["Lambda0_track_2_py"].astype(np.float64)
    s_p_pz  = s["Lambda0_track_2_pz"].astype(np.float64)
    s_pi2_pt  = s["track_3_pT"].astype(np.float64)
    s_pi2_px  = s["track_3_px"].astype(np.float64)
    s_pi2_py  = s["track_3_py"].astype(np.float64)
    s_pi2_pz  = s["track_3_pz"].astype(np.float64)

    nBins   = len(XI_PT_BINS) - 1
    results = []

    HDR = (f"{'Bin':>3}  {'proton pT':>16}  "
           f"{'σ_data_KFP':>11}  {'σ_sv_inv0':>10}  "
           f"{'σ_sv_pi-smr':>12}  {'best%':>7}  {'σ_matched':>10}")
    print(HDR)
    print("─" * len(HDR.rstrip()))

    for i in range(nBins):
        lo, hi = XI_PT_BINS[i], XI_PT_BINS[i+1]
        hi_s   = f"{hi:.3f}" if hi<1e8 else "∞      "

        dm_b = (d_p_pt>=lo) & (d_p_pt<hi)
        sm_b = (s_p_pt>=lo) & (s_p_pt<hi)
        n_d, n_s = int(dm_b.sum()), int(sm_b.sum())
        avg_pt = float(d_p_pt[dm_b].mean()) if n_d>0 else (lo+hi)/2

        _, sig_data_kfp = fit_xi(d_kfp[dm_b])

        pi1_px_b = s_pi1_px[sm_b]; pi1_py_b = s_pi1_py[sm_b]; pi1_pz_b = s_pi1_pz[sm_b]
        pi1_pt_b = s_pi1_pt[sm_b]
        pi2_px_b = s_pi2_px[sm_b]; pi2_py_b = s_pi2_py[sm_b]; pi2_pz_b = s_pi2_pz[sm_b]
        pi2_pt_b = s_pi2_pt[sm_b]
        p_px_b  = s_p_px[sm_b];  p_py_b  = s_p_py[sm_b];  p_pz_b  = s_p_pz[sm_b]
        sv_inv0 = xi_mass_mev(p_px_b,p_py_b,p_pz_b,pi1_px_b,pi1_py_b,pi1_pz_b,pi2_px_b,pi2_py_b,pi2_pz_b)
        _, sig_sv_inv0 = fit_xi(sv_inv0)

        def _bad(note):
            results.append(dict(lo=lo, hi=hi, frac=np.nan, avg_pt=avg_pt,
                                sig_data_kfp=sig_data_kfp, sig_sv_inv0=sig_sv_inv0,
                                sig_pi_only=np.nan, sig_match=np.nan,
                                note=note, n_d=n_d, n_s=n_s))

        if n_d < 20 or np.isnan(sig_data_kfp):
            _bad("too few data"); print(f"{i:>3}  [{lo:.3f},{hi_s})  too few data"); continue
        if n_s < 20 or np.isnan(sig_sv_inv0):
            _bad("too few sim");  print(f"{i:>3}  [{lo:.3f},{hi_s})  too few sim");  continue

        pi1_fracs = pion_frac(pi1_pt_b)
        pi2_fracs = pion_frac(pi2_pt_b)
        rng  = np.random.default_rng(RNG_SEED + i)
        z_pi1 = rng.standard_normal(n_s)
        z_pi2 = rng.standard_normal(n_s)
        z_p  = rng.standard_normal(n_s)

        def sim_sigma(frac_p):
            spx_pi1,spy_pi1,spz_pi1 = apply_smear_pervec(pi1_px_b,pi1_py_b,pi1_pz_b,pi1_fracs,z_pi1)
            spx_pi2,spy_pi2,spz_pi2 = apply_smear_pervec(pi2_px_b,pi2_py_b,pi2_pz_b,pi2_fracs,z_pi2)
            spx_p, spy_p, spz_p  = apply_smear(p_px_b,p_py_b,p_pz_b,frac_p,z_p)
            _, sig = fit_xi(xi_mass_mev(spx_p,spy_p,spz_p,spx_pi1,spy_pi1,spz_pi1,spx_pi2,spy_pi2,spz_pi2))
            return sig

        def residual(frac_p): return sim_sigma(frac_p) - sig_data_kfp

        # pion-only baseline
        r0         = residual(0.0)
        sig_pi_only = sig_data_kfp + r0   # = sim_sigma(0.0)

        if r0 >= 0:
            note = "sim≥data at 0%"
            results.append(dict(lo=lo, hi=hi, frac=0.0, avg_pt=avg_pt,
                                sig_data_kfp=sig_data_kfp, sig_sv_inv0=sig_sv_inv0,
                                sig_pi_only=sig_pi_only, sig_match=sig_pi_only,
                                note=note, n_d=n_d, n_s=n_s))
            print(f"{i:>3}  [{lo:.3f},{hi_s})  {sig_data_kfp:>11.3f}  "
                  f"{sig_sv_inv0:>10.3f}  {sig_pi_only:>12.3f}  {'0.00%':>7}  "
                  f"{sig_pi_only:>10.3f} ({note})")
            continue

        r_max = residual(frac_max)
        if r_max < 0:
            sig_at_max = sim_sigma(frac_max)
            note = f">={frac_max*100:.0f}%"
            results.append(dict(lo=lo, hi=hi, frac=frac_max, avg_pt=avg_pt,
                                sig_data_kfp=sig_data_kfp, sig_sv_inv0=sig_sv_inv0,
                                sig_pi_only=sig_pi_only, sig_match=sig_at_max,
                                note=note, n_d=n_d, n_s=n_s))
            print(f"{i:>3}  [{lo:.3f},{hi_s})  {sig_data_kfp:>11.3f}  "
                  f"{sig_sv_inv0:>10.3f}  {sig_pi_only:>12.3f}  {note:>7}  {sig_at_max:>10.3f}")
            continue

        best  = brentq(residual, 0.0, frac_max, xtol=5e-5, maxiter=80)
        sig_m = sim_sigma(best)
        results.append(dict(lo=lo, hi=hi, frac=best, avg_pt=avg_pt,
                            sig_data_kfp=sig_data_kfp, sig_sv_inv0=sig_sv_inv0,
                            sig_pi_only=sig_pi_only, sig_match=sig_m,
                            note="ok", n_d=n_d, n_s=n_s))
        print(f"{i:>3}  [{lo:.3f},{hi_s})  {sig_data_kfp:>11.3f}  "
              f"{sig_sv_inv0:>10.3f}  {sig_pi_only:>12.3f}  {best*100:>6.2f}%  {sig_m:>10.3f}")

    valid = [r for r in results if not np.isnan(r["frac"])]
    edges_cpp  = ", ".join(f"{r['lo']:.4f}" for r in valid)
    sigmas_cpp = ", ".join(f"{r['frac']:.5f}" for r in valid)
    print(f"\n{'─'*68}")
    print("Xi proton fracs (C++ for smear_Xi_ptbin.C):\n")
    print(f"  pt_bin_edges  = {{{edges_cpp}}}")
    print(f"  pt_bin_sigmas = {{{sigmas_cpp}}}")

    with open("optimal_sv_xi_fracs.txt","w") as fh:
        fh.write("# Auto-generated by find_sv_smear_fracs.py\n")
        fh.write("# SV sim inv mass smeared to match data KFP mass.\n")
        fh.write("# Pion fracs fixed from K0S SV analysis (see optimal_sv_smear_fracs.txt).\n")
        fh.write(f"# pt_bin_edges  = {{{edges_cpp}}}\n")
        fh.write(f"# pt_bin_sigmas = {{{sigmas_cpp}}}\n\n")
        fh.write("bin_lo,bin_hi,avg_pt,best_frac_pct,sig_data_kfp,sig_sv_inv0,"
                 "sig_pi_only,sig_match,n_data,n_sim,note\n")
        for r in results:
            hi_w = f"{r['hi']:.4f}" if r["hi"]<1e8 else "inf"
            _f   = lambda v: f"{v:.4f}" if not np.isnan(v) else "nan"
            fp   = f"{r['frac']*100:.4f}" if not np.isnan(r["frac"]) else "nan"
            fh.write(f"{r['lo']:.4f},{hi_w},{r['avg_pt']:.4f},{fp},{_f(r['sig_data_kfp'])},"
                     f"{_f(r['sig_sv_inv0'])},{_f(r['sig_pi_only'])},"
                     f"{_f(r['sig_match'])},{r['n_d']},{r['n_s']},{r['note']}\n")
    print(f"CSV saved → optimal_sv_xi_fracs.txt")

    return results


# ─────────────────────────────────────────────────────────────────────────────
# ROOT macro writers
# ─────────────────────────────────────────────────────────────────────────────

def _fmt(lst, fmt=".4f"):
    return "{" + ", ".join(format(v if not np.isnan(v) else 0, fmt) for v in lst) + "}"

def write_ks_macro(results):
    n        = len(results)
    pts      = [r["avg_pt"]       for r in results]
    sig_dk   = [r["sig_data_kfp"] if not np.isnan(r["sig_data_kfp"]) else 0 for r in results]
    sig_sv0  = [r["sig_sv_inv0"]  if not np.isnan(r["sig_sv_inv0"])  else 0 for r in results]
    sig_m    = [r["sig_match"]    if not np.isnan(r["sig_match"])    else 0 for r in results]
    fracs    = [r["frac"]*100     if not np.isnan(r["frac"])         else 0 for r in results]
    ymax     = max(max(sig_dk), max(sig_m)) * 1.35

    macro = f"""// Auto-generated by find_sv_smear_fracs.py
// K0S: SV sim inv mass smeared to match data KFP mass.
// Run with:  root -l -b -q plot_sv_ks_result.C

void plot_sv_ks_result() {{
    gStyle->SetOptStat(0); gStyle->SetOptTitle(0);

    const int N = {n};
    double avg_pt[]  = {_fmt(pts)};
    double sig_dk[]  = {_fmt(sig_dk)};   // data KFP mass sigma  ← TARGET
    double sig_sv0[] = {_fmt(sig_sv0)};  // SV sim inv mass (no smear)
    double sig_m[]   = {_fmt(sig_m)};    // SV sim inv mass (matched smear)
    double fracs[]   = {_fmt(fracs)};    // optimal pion smearing %

    TCanvas *c1 = new TCanvas("c1","K0S SV smearing",950,620);
    c1->SetLeftMargin(0.12); c1->SetBottomMargin(0.13); c1->SetRightMargin(0.05);

    auto sty = [](TGraph* g, Color_t col, Style_t mrk, Style_t ls=1){{
        g->SetMarkerColor(col); g->SetLineColor(col);
        g->SetMarkerStyle(mrk); g->SetMarkerSize(1.6);
        g->SetLineWidth(2); g->SetLineStyle(ls);
    }};

    TGraph *gDK = new TGraph(N,avg_pt,sig_dk);
    TGraph *gS0 = new TGraph(N,avg_pt,sig_sv0);
    TGraph *gM  = new TGraph(N,avg_pt,sig_m);

    sty(gDK, kBlack,  20, 1);
    sty(gS0, kBlue+1, 24, 2);
    sty(gM,  kRed+1,  22, 1);

    TMultiGraph *mg = new TMultiGraph();
    mg->Add(gS0,"PL"); mg->Add(gM,"PL"); mg->Add(gDK,"PL");
    mg->Draw("A");
    mg->GetXaxis()->SetTitle("p_{{T}} (GeV/#it{{c}})");
    mg->GetYaxis()->SetTitle("#sigma_{{K^{{0}}_{{S}}}} (MeV/#it{{c}}^{{2}})");
    mg->GetYaxis()->SetRangeUser(0,{ymax:.1f});
    mg->GetXaxis()->SetTitleSize(0.05);
    mg->GetYaxis()->SetTitleSize(0.05);

    TLegend *leg = new TLegend(0.14,0.68,0.72,0.93);
    leg->SetBorderSize(0); leg->SetFillStyle(0); leg->SetTextSize(0.032);
    leg->AddEntry(gDK,"Data      (K_{{S}}^{{0}} mass, KFP) — target","lp");
    leg->AddEntry(gS0,"SV sim    (inv mass, no smear)","lp");
    leg->AddEntry(gM, "SV sim    (inv mass, matched smear)","lp");
    leg->Draw();

    c1->SaveAs("plot_sv_ks_result.pdf");
    c1->SaveAs("plot_sv_ks_result.png");
    std::cout << "Saved plot_sv_ks_result.pdf/.png\\n";

    TCanvas *c2 = new TCanvas("c2","K0S pion smearing frac",900,550);
    c2->SetLeftMargin(0.13); c2->SetBottomMargin(0.13);
    TGraph *gF = new TGraph(N,avg_pt,fracs);
    sty(gF, kGreen+2, 23);
    gF->SetTitle(";p_{{T}} (GeV/#it{{c}});Optimal pion smearing (% of p_{{T}})");
    gF->GetYaxis()->SetRangeUser(0,15);
    gF->GetXaxis()->SetTitleSize(0.05); gF->GetYaxis()->SetTitleSize(0.05);
    gF->Draw("APL");
    c2->SaveAs("plot_sv_ks_frac.pdf");
    c2->SaveAs("plot_sv_ks_frac.png");
    std::cout << "Saved plot_sv_ks_frac.pdf/.png\\n";
}}
"""
    with open("plot_sv_ks_result.C","w") as fh:
        fh.write(macro)


def write_xi_macro(results):
    n        = len(results)
    pts      = [r["avg_pt"]       for r in results]
    sig_dk   = [r["sig_data_kfp"] if not np.isnan(r["sig_data_kfp"]) else 0 for r in results]
    sig_sv0  = [r["sig_sv_inv0"]  if not np.isnan(r["sig_sv_inv0"])  else 0 for r in results]
    sig_pi   = [r["sig_pi_only"]  if not np.isnan(r["sig_pi_only"])  else 0 for r in results]
    sig_m    = [r["sig_match"]    if not np.isnan(r["sig_match"])    else 0 for r in results]
    fracs    = [r["frac"]*100     if not np.isnan(r["frac"])         else 0 for r in results]
    ymax     = max(max(sig_dk), max(sig_m), max(sig_pi)) * 1.35

    macro = f"""// Auto-generated by find_sv_smear_fracs.py
// Xi: SV sim inv mass smeared to match data KFP mass.
// Run with:  root -l -b -q plot_sv_xi_result.C

void plot_sv_xi_result() {{
    gStyle->SetOptStat(0); gStyle->SetOptTitle(0);

    const int N = {n};
    double avg_pt[]  = {_fmt(pts)};
    double sig_dk[]  = {_fmt(sig_dk)};   // data KFP mass sigma  ← TARGET
    double sig_sv0[] = {_fmt(sig_sv0)};  // SV sim inv mass (no smear)
    double sig_pi[]  = {_fmt(sig_pi)};   // SV sim inv mass (pion smeared, proton 0%)
    double sig_m[]   = {_fmt(sig_m)};    // SV sim inv mass (pion + proton matched)
    double fracs[]   = {_fmt(fracs)};    // optimal proton smearing %

    TCanvas *c1 = new TCanvas("c1","Xi SV smearing",950,620);
    c1->SetLeftMargin(0.12); c1->SetBottomMargin(0.13); c1->SetRightMargin(0.05);

    auto sty = [](TGraph* g, Color_t col, Style_t mrk, Style_t ls=1){{
        g->SetMarkerColor(col); g->SetLineColor(col);
        g->SetMarkerStyle(mrk); g->SetMarkerSize(1.6);
        g->SetLineWidth(2); g->SetLineStyle(ls);
    }};

    TGraph *gDK = new TGraph(N,avg_pt,sig_dk);
    TGraph *gS0 = new TGraph(N,avg_pt,sig_sv0);
    TGraph *gPI = new TGraph(N,avg_pt,sig_pi);
    TGraph *gM  = new TGraph(N,avg_pt,sig_m);

    sty(gDK, kBlack,    20, 1);
    sty(gS0, kBlue+1,   24, 2);
    sty(gPI, kOrange+1, 25, 2);
    sty(gM,  kRed+1,    22, 1);

    TMultiGraph *mg = new TMultiGraph();
    mg->Add(gS0,"PL"); mg->Add(gPI,"PL"); mg->Add(gM,"PL"); mg->Add(gDK,"PL");
    mg->Draw("A");
    mg->GetXaxis()->SetTitle("Proton p_{{T}} (GeV/#it{{c}})");
    mg->GetYaxis()->SetTitle("#sigma_{{#Xi}} (MeV/#it{{c}}^{{2}})");
    mg->GetYaxis()->SetRangeUser(0,{ymax:.1f});
    mg->GetXaxis()->SetTitleSize(0.05);
    mg->GetYaxis()->SetTitleSize(0.05);

    TLegend *leg = new TLegend(0.14,0.60,0.76,0.93);
    leg->SetBorderSize(0); leg->SetFillStyle(0); leg->SetTextSize(0.030);
    leg->AddEntry(gDK,"Data      (#Xi mass, KFP) — target","lp");
    leg->AddEntry(gS0,"SV sim    (inv mass, no smear)","lp");
    leg->AddEntry(gPI,"SV sim    (inv mass, #pi smeared, proton 0%)","lp");
    leg->AddEntry(gM, "SV sim    (inv mass, #pi + proton matched)","lp");
    leg->Draw();

    c1->SaveAs("plot_sv_xi_result.pdf");
    c1->SaveAs("plot_sv_xi_result.png");
    std::cout << "Saved plot_sv_xi_result.pdf/.png\\n";

    TCanvas *c2 = new TCanvas("c2","Xi proton smearing frac",900,550);
    c2->SetLeftMargin(0.13); c2->SetBottomMargin(0.13);
    TGraph *gF = new TGraph(N,avg_pt,fracs);
    sty(gF, kMagenta+1, 23);
    gF->SetTitle(";Proton p_{{T}} (GeV/#it{{c}});Optimal proton smearing (% of p_{{T}})");
    gF->GetYaxis()->SetRangeUser(0,15);
    gF->GetXaxis()->SetTitleSize(0.05); gF->GetYaxis()->SetTitleSize(0.05);
    gF->Draw("APL");
    c2->SaveAs("plot_sv_xi_frac.pdf");
    c2->SaveAs("plot_sv_xi_frac.png");
    std::cout << "Saved plot_sv_xi_frac.pdf/.png\\n";
}}
"""
    with open("plot_sv_xi_result.C","w") as fh:
        fh.write(macro)


# ─────────────────────────────────────────────────────────────────────────────
# Entry point
# ─────────────────────────────────────────────────────────────────────────────

def main():
    ap = argparse.ArgumentParser(description=__doc__,
             formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--frac-max", type=float, default=0.15,
                    help="Upper smearing fraction bound (default 0.15 = 15%%)")
    args = ap.parse_args()

    ks_results  = run_ks(args.frac_max)
    xi_results = run_xi(ks_results, args.frac_max)

    write_ks_macro(ks_results)
    write_xi_macro(xi_results)

    print("\nROOT macros written:")
    print("  plot_sv_ks_result.C     plot_sv_xi_result.C")
    print("Run with:")
    print("  root -l -b -q plot_sv_ks_result.C")
    print("  root -l -b -q plot_sv_xi_result.C")


if __name__ == "__main__":
    main()
