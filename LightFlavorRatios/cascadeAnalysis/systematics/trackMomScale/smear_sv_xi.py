#!/usr/bin/env python3
"""
smear_sv_xi.py
------------------
Reads outputKFParticleXiSV_filtered.root, applies per-track Gaussian pT smearing,
and writes outputKFParticleXiSV_smeared.root with updated kinematics.

Two independent smearing calibrations are applied:
  Track 1 (pion, ±211)  — fracs from optimal_sv_smear_fracs.txt  (K0S calibration)
  Track 2 (proton, 2212)— fracs from optimal_sv_xi_fracs.txt  (Xi calibration)

Both frac tables use linear interpolation between calibrated avg_pt points,
with flat extrapolation outside the range.  eta is preserved for each track.

Branches updated
----------------
  track_{1,2}_px, _py, _pz, _pT, _p, _pE
  track_{1,2}_phi, _pseudorapidity, _rapidity, _theta
  Lambda0_px, _py, _pz, _pT, _p, _pE
  Lambda0_phi, _pseudorapidity, _rapidity, _theta
  Lambda0_mass                  (pion+proton inv mass from smeared daughters)
  secondary_vertex_mass_pionPID (pion+pion inv mass from smeared daughters)

Branches NOT updated
--------------------
  *_chi2, *_nDoF, *_Covariance, *Err  KFP fitter outputs
  *_IP*, *_DCA, *_DIRA                require track re-propagation
  *_decayLength*, *_decayTime*        vertex geometry
  *_x, *_y, *_z                       vertex positions
  true_* branches                     MC truth

Usage
-----
  python3 smear_sv_lambda.py
  python3 smear_sv_lambda.py --input foo.root --output foo_smeared.root
"""

import argparse
import numpy as np
import uproot

PION_MASS   = 0.13957018   # GeV
PROTON_MASS = 0.93827209   # GeV
TREE_NAME   = "DecayTree"
RNG_SEED    = 42


# ── helpers (shared with smear_sv_ks.py) ──────────────────────────────────

def load_fracs(csv_path):
    """Parse a fracs CSV, return (avg_pt, frac) arrays for np.interp."""
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
    """Linear interpolation with flat extrapolation."""
    return np.interp(pt_arr.astype(np.float64), pts, fracs)


def smear_track(px, py, pz, frac_arr, z):
    """Smear pT = pT*(1 + frac*z), preserve eta. Returns float64."""
    px, py, pz = (np.asarray(a, dtype=np.float64) for a in (px, py, pz))
    pT   = np.sqrt(px**2 + py**2)
    phi  = np.arctan2(py, px)
    ptot = np.sqrt(pT**2 + pz**2)
    eta  = np.arctanh(np.clip(pz / np.where(ptot > 0, ptot, 1e-12),
                               -1+1e-9, 1-1e-9))
    pT_s = np.maximum(pT * (1.0 + frac_arr * z), 0.0)
    return pT_s*np.cos(phi), pT_s*np.sin(phi), pT_s*np.sinh(eta)


def derived(px, py, pz, mass_gev):
    """(pT, p, pE, phi, eta, rapidity, theta) from 3-momentum + mass."""
    px, py, pz = (np.asarray(a, dtype=np.float64) for a in (px, py, pz))
    pT    = np.sqrt(px**2 + py**2)
    p     = np.sqrt(px**2 + py**2 + pz**2)
    pE    = np.sqrt(p**2 + mass_gev**2)
    phi   = np.arctan2(py, px)
    p_s   = np.where(p > 0, p, 1e-12)
    eta   = np.arctanh(np.clip(pz / p_s, -1+1e-9, 1-1e-9))
    denom = np.abs(pE - np.abs(pz))
    rap   = np.where(denom > 1e-12,
                     0.5*np.log((pE+pz) / np.where(denom>1e-12, np.abs(pE-pz), 1e-12)),
                     np.sign(pz)*1e6)
    theta = np.arctan2(pT, pz)
    return pT, p, pE, phi, eta, rap, theta


def inv_mass(px1, py1, pz1, m1, px2, py2, pz2, m2, px3, py3, pz3, m3):
    """Invariant mass in GeV."""
    E1 = np.sqrt(px1**2+py1**2+pz1**2 + m1**2)
    E2 = np.sqrt(px2**2+py2**2+pz2**2 + m2**2)
    E3 = np.sqrt(px3**2+py3**2+pz3**2 + m3**2)
    m2v = (E1+E2+E3)**2 - (px1+px2+px3)**2 - (py1+py2+py3)**2 - (pz1+pz2+pz3)**2
    return np.sqrt(np.maximum(m2v, 0.0))


# ── main ───────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--input",       default="outputKFParticleXiminusSV_filtered.root")
    parser.add_argument("--pion_fracs",  default="mass_smear_fracs_ks.txt",
                        help="K0S pion smearing fracs")
    parser.add_argument("--proton_fracs",default="mass_smear_fracs_xi_proton.txt",
                        help="Xi proton smearing fracs")
    parser.add_argument("--output",      default="outputKFParticleXiSV_smeared.root")
    parser.add_argument("--seed",        type=int, default=RNG_SEED)
    args = parser.parse_args()

    # ── calibration ────────────────────────────────────────────────────────
    pi_pts,  pi_fracs  = load_fracs(args.pion_fracs)
    pro_pts, pro_fracs = load_fracs(args.proton_fracs)

    print(f"Pion fracs ({args.pion_fracs}):")
    for p,f in zip(pi_pts, pi_fracs): print(f"  {p:.4f} GeV → {f*100:.4f}%")
    print(f"\nProton fracs ({args.proton_fracs}):")
    for p,f in zip(pro_pts, pro_fracs): print(f"  {p:.4f} GeV → {f*100:.4f}%")
    print()

    # ── branch list ────────────────────────────────────────────────────────
    with uproot.open(args.input) as fin:
        tree = fin[TREE_NAME]
        skip = [b for b in tree.keys()
                if "vector" in tree[b].typename.lower()
                or "std::" in tree[b].typename.lower()]
        load_br = [b for b in tree.keys() if b not in skip]

    print(f"Reading {args.input} ...")
    print(f"  Loading {len(load_br)} branches, skipping {len(skip)} vector branches")

    with uproot.open(args.input) as fin:
        d = fin[TREE_NAME].arrays(load_br, library="np")
    n = len(d[load_br[0]])
    print(f"  {n:,} events read\n")

    # ── smear ──────────────────────────────────────────────────────────────
    rng = np.random.default_rng(args.seed)
    z1  = rng.standard_normal(n)   # lam pion
    z2  = rng.standard_normal(n)   # proton
    z3  = rng.standard_normal(n)   # bach pion

    pT1 = d["Lambda0_track_1_pT"].astype(np.float64)   # lam pion pT
    pT2 = d["Lambda0_track_2_pT"].astype(np.float64)   # proton pT
    pT3 = d["track_3_pT"].astype(np.float64)   # bach pion pT

    f_pi1  = interp_frac(pT1, pi_pts,  pi_fracs)
    f_pro = interp_frac(pT2, pro_pts, pro_fracs)
    f_pi3  = interp_frac(pT3, pi_pts,  pi_fracs)

    print("Smearing tracks ...")
    px1s, py1s, pz1s = smear_track(d["Lambda0_track_1_px"], d["Lambda0_track_1_py"], d["Lambda0_track_1_pz"], f_pi1, z1)
    px2s, py2s, pz2s = smear_track(d["Lambda0_track_2_px"], d["Lambda0_track_2_py"], d["Lambda0_track_2_pz"], f_pro, z2)
    px3s, py3s, pz3s = smear_track(d["track_3_px"], d["track_3_py"], d["track_3_pz"], f_pi3, z3)

    # ── derived kinematics ─────────────────────────────────────────────────
    pT1s, p1s, pE1s, phi1s, eta1s, rap1s, th1s = derived(px1s, py1s, pz1s, PION_MASS)
    pT2s, p2s, pE2s, phi2s, eta2s, rap2s, th2s = derived(px2s, py2s, pz2s, PROTON_MASS)
    pT3s, p3s, pE3s, phi3s, eta3s, rap3s, th3s = derived(px3s, py3s, pz3s, PION_MASS)

    xi_mass  = inv_mass(px1s,py1s,pz1s, PION_MASS,  px2s,py2s,pz2s, PROTON_MASS, px3s,py3s,pz3s, PION_MASS)
    pion_mass = inv_mass(px1s,py1s,pz1s, PION_MASS,  px2s,py2s,pz2s, PION_MASS, px3s,py3s,pz3s, PION_MASS)

    xi_px = px1s+px2s+px3s;  xi_py = py1s+py2s+py3s;  xi_pz = pz1s+pz2s+pz3s
    xi_pTs, xi_ps, xi_pEs, xi_phis, xi_etas, xi_raps, xi_ths = derived(
        xi_px, xi_py, xi_pz, xi_mass)

    # sanity
    win = (d["Ximinus_mass"]*1000 > 1300) & (d["Ximinus_mass"]*1000 < 1340)
    before = d["Ximinus_mass"][win].astype(np.float64)*1000
    after  = xi_mass[win]*1000
    print(f"Signal-window mass (MeV) [1300,1340]:")
    print(f"  Before smearing: mean={before.mean():.2f}  σ_rms={before.std():.2f}")
    print(f"  After  smearing: mean={after.mean():.2f}   σ_rms={after.std():.2f}\n")

    # ── replacement map ────────────────────────────────────────────────────
    replacements = {
        "Lambda0_track_1_px":              px1s,
        "Lambda0_track_1_py":              py1s,
        "Lambda0_track_1_pz":              pz1s,
        "Lambda0_track_1_pT":              pT1s,
        "Lambda0_track_1_p":               p1s,
        "Lambda0_track_1_pE":              pE1s,
        "Lambda0_track_1_phi":             phi1s,
        "Lambda0_track_1_pseudorapidity":  eta1s,
        "Lambda0_track_1_rapidity":        rap1s,
        "Lambda0_track_1_theta":           th1s,
        "Lambda0_track_2_px":              px2s,
        "Lambda0_track_2_py":              py2s,
        "Lambda0_track_2_pz":              pz2s,
        "Lambda0_track_2_pT":              pT2s,
        "Lambda0_track_2_p":               p2s,
        "Lambda0_track_2_pE":              pE2s,
        "Lambda0_track_2_phi":             phi2s,
        "Lambda0_track_2_pseudorapidity":  eta2s,
        "Lambda0_track_2_rapidity":        rap2s,
        "Lambda0_track_2_theta":           th2s,
        "track_3_px":              px3s,
        "track_3_py":              py3s,
        "track_3_pz":              pz3s,
        "track_3_pT":              pT3s,
        "track_3_p":               p3s,
        "track_3_pE":              pE3s,
        "track_3_phi":             phi3s,
        "track_3_pseudorapidity":  eta3s,
        "track_3_rapidity":        rap3s,
        "track_3_theta":           th3s,
        "Ximinus_px":              xi_px,
        "Ximinus_py":              xi_py,
        "Ximinus_pz":              xi_pz,
        "Ximinus_pT":              xi_pTs,
        "Ximinus_p":               xi_ps,
        "Ximinus_pE":              xi_pEs,
        "Ximinus_phi":             xi_phis,
        "Ximinus_pseudorapidity":  xi_etas,
        "Ximinus_rapidity":        xi_raps,
        "Ximinus_theta":           xi_ths,
        "Ximinus_mass":            xi_mass,
        "secondary_vertex_mass_pionPID": pion_mass,
    }

    # ── assemble & write ───────────────────────────────────────────────────
    out = {}
    for b in load_br:
        out[b] = replacements[b].astype(d[b].dtype) if b in replacements else d[b]

    not_found = [b for b in replacements if b not in load_br]
    if not_found:
        print(f"WARNING: branches not found in file: {not_found}")

    print(f"Writing {args.output} ...")
    with uproot.recreate(args.output) as fout:
        fout[TREE_NAME] = out

    import os
    print(f"Done — {n:,} events, {len(out)} branches → {args.output} "
          f"({os.path.getsize(args.output)/1e6:.1f} MB)")
    if skip:
        print(f"\nNote: {len(skip)} vector branches omitted (hit IDs, residuals, etc.)")


if __name__ == "__main__":
    main()
