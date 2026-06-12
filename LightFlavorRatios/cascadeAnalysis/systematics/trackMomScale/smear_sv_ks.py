#!/usr/bin/env python3
"""
smear_sv_ks.py
--------------
Reads outputKFParticleKShortRecoSV.root (or any KFParticle K0S SV file),
applies per-pion Gaussian pT smearing using the calibrated fracs from
optimal_sv_smear_fracs.txt, and writes a new ROOT file with updated
track and K0S kinematics.

Smearing model
--------------
  δpT = frac(pT) × pT × z,   z ~ N(0,1)

  frac(pT) is **linearly interpolated** between the calibrated (avg_pt, frac)
  control points, with flat extrapolation outside the range.  Each pion is
  smeared independently using its own pT.  eta is preserved: only pT is
  scaled, pz follows via  pz_new = pT_new × sinh(eta).

Branches updated
----------------
  track_{1,2}_px, _py, _pz          smeared momenta
  track_{1,2}_pT, _p, _pE           recomputed from smeared 3-momentum
  track_{1,2}_phi                   recomputed
  track_{1,2}_pseudorapidity        recomputed (= eta)
  track_{1,2}_rapidity              recomputed from smeared E, pz
  track_{1,2}_theta                 recomputed
  K_S0_px, _py, _pz                 sum of smeared daughter momenta
  K_S0_pT, _p, _pE                  recomputed
  K_S0_phi, _pseudorapidity,        recomputed
    _rapidity, _theta
  K_S0_mass                         invariant mass from smeared daughters (GeV)
  secondary_vertex_mass_pionPID     same as K_S0_mass (pion-PID assumption)

Branches NOT updated (KFP-fitter outputs / vertex geometry)
-----------------------------------------------------------
  *_chi2, *_nDoF, *_Covariance      KFP fitter quantities
  *_massErr, *_pTErr, *_pErr        KFP uncertainties
  *_IP*, *_DCA, *_DIRA              require re-propagating track to vertex
  *_decayLength*, *_decayTime*      vertex geometry
  *_x, *_y, *_z (positions)        vertex positions unchanged
  true_* branches                   MC truth unchanged

Note on vector branches
-----------------------
  Variable-length branches (std::vector<T>: hit IDs, residuals, IP_allPV,
  track history, etc.) are NOT included in the output.  None of these are
  used by the standard quality cuts or kinematic analysis.  If you need them,
  use ROOT's hadd or TTree::AddFriend to merge with the original file.

Usage
-----
  python3 smear_sv_ks.py                           # defaults
  python3 smear_sv_ks.py --input foo.root --output foo_smeared.root
  python3 smear_sv_ks.py --seed 123
"""

import argparse, sys
import numpy as np
import uproot

# ── physical constants ─────────────────────────────────────────────────────
PION_MASS   = 0.13957018   # GeV
TREE_NAME   = "DecayTree"
RNG_SEED    = 42


# ── helpers ────────────────────────────────────────────────────────────────

def load_fracs(csv_path):
    """
    Parse optimal_sv_smear_fracs.txt.
    Returns (avg_pt_array, frac_array) suitable for np.interp.
    Skips rows with nan frac.
    """
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


def interp_frac(pt_arr, pts, fracs):
    """
    Linearly interpolate smearing frac at each pT value.
    Flat extrapolation outside [pts[0], pts[-1]].
    """
    return np.interp(pt_arr.astype(np.float64), pts, fracs)


def smear_track(px, py, pz, frac_arr, z):
    """
    Smear pT of a track array; preserve eta.
    pT_new = pT * (1 + frac * z)
    Returns (px_new, py_new, pz_new) as float64.
    """
    px, py, pz = (np.asarray(a, dtype=np.float64) for a in (px, py, pz))
    pT   = np.sqrt(px**2 + py**2)
    phi  = np.arctan2(py, px)
    ptot = np.sqrt(pT**2 + pz**2)
    eta  = np.arctanh(np.clip(pz / np.where(ptot > 0, ptot, 1e-12),
                               -1 + 1e-9, 1 - 1e-9))
    pT_s = np.maximum(pT * (1.0 + frac_arr * z), 0.0)
    return pT_s * np.cos(phi), pT_s * np.sin(phi), pT_s * np.sinh(eta)


def derived(px, py, pz, mass_gev):
    """
    Compute (pT, p, pE, phi, eta, rapidity, theta) from 3-momentum + mass.
    All inputs/outputs float64.
    """
    px, py, pz = (np.asarray(a, dtype=np.float64) for a in (px, py, pz))
    pT    = np.sqrt(px**2 + py**2)
    p     = np.sqrt(px**2 + py**2 + pz**2)
    pE    = np.sqrt(p**2 + mass_gev**2)
    phi   = np.arctan2(py, px)
    p_s   = np.where(p > 0, p, 1e-12)
    eta   = np.arctanh(np.clip(pz / p_s, -1 + 1e-9, 1 - 1e-9))
    # rapidity = 0.5 * ln((E+pz)/(E-pz))
    denom = np.abs(pE - np.abs(pz))
    rap   = np.where(denom > 1e-12,
                     0.5 * np.log((pE + pz) / np.where(denom > 1e-12, np.abs(pE - pz), 1e-12)),
                     np.sign(pz) * 1e6)
    theta = np.arctan2(pT, pz)
    return pT, p, pE, phi, eta, rap, theta


def inv_mass(px1, py1, pz1, px2, py2, pz2, m1=PION_MASS, m2=PION_MASS):
    """K0S invariant mass in GeV from two daughters."""
    E1 = np.sqrt(px1**2 + py1**2 + pz1**2 + m1**2)
    E2 = np.sqrt(px2**2 + py2**2 + pz2**2 + m2**2)
    m2v = (E1+E2)**2 - (px1+px2)**2 - (py1+py2)**2 - (pz1+pz2)**2
    return np.sqrt(np.maximum(m2v, 0.0))


# ── main ───────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(
        description="Apply SV K0S pion smearing and write updated ROOT file.")
    parser.add_argument("--input",  default="outputKFParticleKShortRecoSV_filtered.root")
    parser.add_argument("--fracs",  default="mass_smear_fracs_ks.txt")
    parser.add_argument("--output", default="outputKFParticleKShortRecoSV_smeared.root")
    parser.add_argument("--seed",   type=int, default=RNG_SEED)
    args = parser.parse_args()

    # ── calibration ────────────────────────────────────────────────────────
    pts, fracs = load_fracs(args.fracs)
    print(f"Loaded {len(fracs)} calibration points from {args.fracs}:")
    for p, f in zip(pts, fracs):
        print(f"  avg_pT = {p:.4f} GeV  →  frac = {f*100:.4f}%")
    print(f"  (linear interp; flat extrapolation outside [{pts[0]:.3f}, {pts[-1]:.3f}] GeV)\n")

    # ── identify branches to load ──────────────────────────────────────────
    # Skip std::vector<T> branches — none are used by quality cuts or
    # kinematic analysis.  Write a note in the output.
    print(f"Reading branch list from {args.input} ...")
    with uproot.open(args.input) as fin:
        tree = fin[TREE_NAME]
        all_branches = tree.keys()
        skip_vector  = [b for b in all_branches
                        if "vector" in tree[b].typename.lower()
                        or "std::" in tree[b].typename.lower()]
        load_branches = [b for b in all_branches if b not in skip_vector]

    print(f"  Loading {len(load_branches)} scalar/fixed-array branches "
          f"(skipping {len(skip_vector)} vector branches)")
    if skip_vector:
        print(f"  Skipped: {', '.join(skip_vector[:6])}"
              + (" ..." if len(skip_vector) > 6 else ""))

    # ── load data ──────────────────────────────────────────────────────────
    print(f"\nReading events ...")
    with uproot.open(args.input) as fin:
        d = fin[TREE_NAME].arrays(load_branches, library="np")
    n = len(d[load_branches[0]])
    print(f"  {n:,} events read\n")

    # ── smear ──────────────────────────────────────────────────────────────
    rng = np.random.default_rng(args.seed)
    z1  = rng.standard_normal(n)
    z2  = rng.standard_normal(n)

    pT1 = d["track_1_pT"].astype(np.float64)
    pT2 = d["track_2_pT"].astype(np.float64)

    f1  = interp_frac(pT1, pts, fracs)
    f2  = interp_frac(pT2, pts, fracs)

    print("Smearing tracks ...")
    px1s, py1s, pz1s = smear_track(d["track_1_px"], d["track_1_py"], d["track_1_pz"], f1, z1)
    px2s, py2s, pz2s = smear_track(d["track_2_px"], d["track_2_py"], d["track_2_pz"], f2, z2)

    # ── derived kinematics ─────────────────────────────────────────────────
    pT1s, p1s, pE1s, phi1s, eta1s, rap1s, th1s = derived(px1s, py1s, pz1s, PION_MASS)
    pT2s, p2s, pE2s, phi2s, eta2s, rap2s, th2s = derived(px2s, py2s, pz2s, PION_MASS)

    ks_mass = inv_mass(px1s, py1s, pz1s, px2s, py2s, pz2s)
    ks_px, ks_py, ks_pz = px1s+px2s, py1s+py2s, pz1s+pz2s
    ks_pTs, ks_ps, ks_pEs, ks_phis, ks_etas, ks_raps, ks_ths = derived(
        ks_px, ks_py, ks_pz, ks_mass)

    # ── sanity check (signal-window mean & sigma) ──────────────────────────
    win = (d["K_S0_mass"]*1000 > 480) & (d["K_S0_mass"]*1000 < 516)
    before = d["K_S0_mass"][win].astype(np.float64)*1000
    after  = ks_mass[win]*1000
    print(f"Signal-window mass (MeV):")
    print(f"  Before smearing: mean={before.mean():.2f}  σ_rms={before.std():.2f}")
    print(f"  After  smearing: mean={after.mean():.2f}  σ_rms={after.std():.2f}\n")

    # ── replacement map: branch → new float64 array ───────────────────────
    replacements = {
        "track_1_px":            px1s,
        "track_1_py":            py1s,
        "track_1_pz":            pz1s,
        "track_1_pT":            pT1s,
        "track_1_p":             p1s,
        "track_1_pE":            pE1s,
        "track_1_phi":           phi1s,
        "track_1_pseudorapidity": eta1s,
        "track_1_rapidity":      rap1s,
        "track_1_theta":         th1s,
        "track_2_px":            px2s,
        "track_2_py":            py2s,
        "track_2_pz":            pz2s,
        "track_2_pT":            pT2s,
        "track_2_p":             p2s,
        "track_2_pE":            pE2s,
        "track_2_phi":           phi2s,
        "track_2_pseudorapidity": eta2s,
        "track_2_rapidity":      rap2s,
        "track_2_theta":         th2s,
        "K_S0_px":               ks_px,
        "K_S0_py":               ks_py,
        "K_S0_pz":               ks_pz,
        "K_S0_pT":               ks_pTs,
        "K_S0_p":                ks_ps,
        "K_S0_pE":               ks_pEs,
        "K_S0_phi":              ks_phis,
        "K_S0_pseudorapidity":   ks_etas,
        "K_S0_rapidity":         ks_raps,
        "K_S0_theta":            ks_ths,
        "K_S0_mass":             ks_mass,
        "secondary_vertex_mass_pionPID": ks_mass,
    }

    # ── build output dict (all numpy, preserve original dtypes) ───────────
    out = {}
    for b in load_branches:
        if b in replacements:
            out[b] = replacements[b].astype(d[b].dtype)
        else:
            out[b] = d[b]

    not_found = [b for b in replacements if b not in load_branches]
    if not_found:
        print(f"WARNING: replacement branches not in file: {not_found}")

    # ── write ──────────────────────────────────────────────────────────────
    print(f"Writing {args.output} ...")
    with uproot.recreate(args.output) as fout:
        fout[TREE_NAME] = out

    import os
    sz = os.path.getsize(args.output) / 1e6
    print(f"Done — {n:,} events, {len(out)} branches → {args.output}  ({sz:.1f} MB)")
    if skip_vector:
        print(f"\nNote: {len(skip_vector)} vector branches omitted (hit IDs, "
              f"residuals, IP_allPV, track history).")
        print(f"To recover them use ROOT:\n"
              f"  new_tree->AddFriend(original_tree)  # or hadd after friend setup")


if __name__ == "__main__":
    main()
