#!/usr/bin/env python3
"""
ml_momentum_calibration_v4.py

Successor to ml_momentum_calibration_reso.py.  Five structural changes:

TASK 1 -- DIRECT KFPARTICLE NTUPLE INPUT (+ candidate-count option)
    --kshort / --lam accept either a KFParticle nTuple ROOT file or a legacy
    CSV (detected by extension).  ROOT files are read with uproot; tree name
    defaults to "DecayTree" (--tree).  Branch names follow KFParticle_nTuple.cc
    (coresoftware): per daughter i in {1, 2}
        track_i_pT, track_i_pseudorapidity, track_i_phi, track_i_charge,
        track_i_PDG_ID, track_i_pTErr, track_i_Covariance[21]
    and per mother (prefix auto-detected, K_S0 / Lambda0)
        <res>_mass, <res>_pT, <res>_pseudorapidity, <res>_phi.
    --max_candidates N randomly subsamples each species to N candidates after
    the selection (0 = use all; seeded by --subsample_seed).
    CSV input carries no covariance, so it forces --reso_model absolute.

TASK 2 -- CURVATURE FEATURE AND CURVATURE-MULTIPLICATIVE, UNCAPPED CORRECTION
    Stage-1 input feature is unsigned curvature u = 1/pT, normalised with
    ROBUST QUANTILES (median and half the 16-84 span) because with the pT cut
    removed (task 4) the u distribution has a long high-u tail that would
    dominate a mean/std normalisation.  u_n is clamped to +-U_CLAMP as a
    numerical guard only; the clamp makes kappa constant in curvature outside
    it, and the fraction of training tracks hitting it is written to
    summary.txt.
    Output convention:
        (1/pT)_corr = (1/pT) * kappa_curv
        kappa_curv  = 1 + eps(u, eta, phi) + q * delta(u, eta, phi)
    so the pT multiplier applied downstream is 1/kappa_curv.  The even/odd
    decomposition is exact in curvature space; q enters only through the
    +q*delta output term, never as a network input.  eps and delta are
    UNCAPPED (no tanh, no S_CAP): the size of the correction is learned, not
    constrained.  The only guard is numerical, kappa_curv >= KAPPA_GUARD,
    which prevents a momentum sign flip during early optimisation; its hit
    fraction must be 0 at convergence (reported, warned on).
    The normalisation constants (u_ref, u_scale, U_CLAMP) are persisted inside
    model.pt and stamped into kappa_lookup.csv; --load_stage1 restores them
    and FAILS LOUDLY on a checkpoint that predates this convention.

TASK 3 -- DAUGHTER-TRACK COVARIANCE => STAGE 2 IS A COVARIANCE CALIBRATION
    track_i_Covariance is the packed lower triangle of the symmetric
    covariance over (x, y, z, px, py, pz), idx(a,b) = a(a+1)/2 + b for a >= b
    (momentum block 9/13/14/18/19/20).  Length 21 = 6x6 state; length 28 =
    7x7 with E appended -- the momentum-block indices are identical; the
    detected layout is printed.  Per track,
        sigma^2(pT) = ( px^2 Vxx + 2 px py Vxy + py^2 Vyy ) / pT^2
        r_cov       = sigma(pT) / pT
    Note sigma(1/pT)/(1/pT) = sigma(pT)/pT to first order, so r_cov is
    simultaneously the fractional curvature resolution -- consistent with the
    rest of the script working in curvature.  Candidates with non-finite
    entries, non-positive Vxx/Vyy, or r_cov outside (1e-5, 1) are dropped and
    counted per category.  sqrt(V(px,px)-projection) is cross-checked against
    track_i_pTErr; a bulk disagreement beyond ~20% aborts (packing convention
    wrong => everything downstream invalid).

    --reso_model {absolute, pull}, default pull.
      absolute:  r_i^2 = (a(eta,phi)/beta_i)^2 + (b(eta,phi)*pT_i)^2  (v2)
      pull:      r_i   = s(eta_i, phi_i) * pT_i^{u_p} * r_cov,i
    with s = S_SCALE*softplus(.) initialised at 1 and u_p a single global
    exponent initialised at 0.

    INTERPRETATION CHANGE (explicit, per request; also stamped into
    reso_lookup.csv and summary.txt when pull is active):
      1. ResoNet no longer measures the momentum resolution.  In `absolute`,
         a and b ARE the detector resolution, measured from the peak widths.
         In `pull`, the absolute scale of r is inherited from the KFParticle
         covariance; what is fitted is s(eta,phi) and u_p, i.e. the
         MISCALIBRATION of the reported covariance.  Perfect calibration is
         s == 1, u_p == 0.
      2. s mixes two effects that cannot be separated here: (i) genuine
         resolution the Kalman hit-error/material model misses, and (ii)
         systematic mis-estimation of the covariance by the fit itself.
      3. The stored covariance is POST-VERTEX-CONSTRAINT
         (m_extrapolateTracksToSV_nTuple defaults true): the constraint
         shrinks the momentum covariance relative to the raw track fit, so s
         is defined relative to CONSTRAINED covariances.  Applying s to raw
         SvtxTrack covariances is wrong by the (unmeasured) constraint
         shrinkage factor.  The exported field is only valid together with
         the same covariance the producer used.
      4. s at low pT and the floor c are partially degenerate (rho(s,c) is
         reported from the reduced global fit, as rho(a,c) was in absolute).
      5. reso_lookup.csv exports (eta, phi, s) plus u_p in the header -- NOT
         an evaluated r grid: r depends on the per-track covariance, which a
         lookup cannot carry.  The consumer rebuilds
         r = s(eta,phi) * pT^{u_p} * r_cov from its own covariance.
    summary.txt reports the median and 16/84 quantiles of r_pred/r_cov per
    resonance group -- the headline factor by which KFParticle's reported
    momentum uncertainty is wrong -- and fig8 gains a raw-covariance pull
    overlay (s = 1, u_p = 0, no floor) whose core width says the same thing
    before calibration.

TASK 4 -- NO pT CUT ON DAUGHTER TRACKS
    The fiducial selection keeps only |eta| < 1.5, a pT > 1 MeV sanity floor,
    finiteness, and the mass windows.  All pT-dependent internals (loss-bin
    edges, kappa-map slices, lookup grid, plot ranges) are derived from data
    quantiles at run time instead of a hard-coded (0.15, 3) GeV range.

TASK 5 -- KAPPA MAPS PRESENTED IN 1/pT
    fig6 (charge-even eps, charge-odd delta) and fig7 (final per-charge pT
    multiplier) are sliced uniformly in curvature 1/pT across the measured
    range, each panel labelled "1/pT = X GeV^-1 (pT = Y GeV)".  The lookup
    grid is likewise uniform in 1/pT and carries both columns.

STAGE 2 MODEL (shared machinery, unchanged from the anchor)
    sigma_m^2 = J1^2 r1^2 + J2^2 r2^2 + c^2,
    J_i = dm/dln(pT_i) = ( E_j |p_i|^2 / E_i - p_i . p_j ) / m  (exact, not
    leg-symmetric), window-normalised core+tail Gaussian mixture NLL with
    mu fixed to M_PDG, per-group tail (f, k), global floor c (or one per
    resonance with --c_per_resonance).  Stage 1 is frozen before stage 2.
    Convergence: the global scalars (c, u_p, f, k) get 5x the field learning
    rate, and an LBFGS polish with the field frozen follows the Adam epochs
    (Adam stalls along the c <-> (s, u_p) soft direction).

INJECTION CAMPAIGN (mandatory go/no-go, --inject / --inject_only)
    Toys are isotropic two-body decays of resonances resampled from the data
    (truth mass exactly M_PDG), same eta/mass-window selection, kappa == 1.
    pull:      each toy leg draws r_cov from the DATA r_cov distribution of
               the matching leg; truth smearing is s0 * pT^{u0} * r_cov plus
               the mass floor c0.  Go/no-go: (s0, u0) recovered within the
               Hessian errors of the reduced global fit + pull closure.
    absolute:  the v2 (a0, b0) recovery test.
    A PASS certifies recovery under the model's own assumptions only.

NOTE ON CHECKPOINTS: model.pt is a dict {state_dict, norm, convention}.
Checkpoints from any earlier version (bare state_dict, log(pT) feature,
tanh-capped output) are rejected by --load_stage1.  Retrain stage 1 and
refit stage 2; a stage-2 refit against an old stage 1 is not a valid
configuration.
"""

import argparse
import math
import os
import sys
import numpy as np
import pandas as pd
import torch
import torch.nn as nn
import torch.nn.functional as F
from scipy.optimize import curve_fit
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

# ---------------------------------------------------------------- constants
PION_MASS   = 0.13957039     # GeV  (never name this M_PI)
PROTON_MASS = 0.93827209
M_K0S       = 0.497611
M_LAMBDA    = 1.115683

ETA_RANGE = (-1.5, 1.5)
PT_SANITY = 1e-3             # GeV; reject unphysical/zero pT, NOT a fiducial cut

KAPPA_GUARD = 0.05           # numerical floor on kappa_curv = 1 + eps + q*delta.
                             # NOT a physics cap; hit fraction must be 0.
U_CLAMP = 6.0                # numerical clamp on the normalised curvature
                             # feature; outside it kappa is constant in 1/pT.

# mass windows used for the width NLL (must match the load() selection)
WIN = {"K0s": (0.42, 0.58), "Lambda": (1.09, 1.145), "Lambdabar": (1.09, 1.145)}
MPDG = {"K0s": M_K0S, "Lambda": M_LAMBDA, "Lambdabar": M_LAMBDA}
SIG0 = {"K0s": 0.010, "Lambda": 0.004, "Lambdabar": 0.004}

# resolution-head parametrisation
A_SCALE = 0.05               # absolute: a = A_SCALE * softplus(.)  dimensionless
B_SCALE = 0.05               # absolute: b = B_SCALE * softplus(.)  [1/GeV]
S_SCALE = 2.0                # pull:     s = S_SCALE * softplus(.)  (init -> 1)
C_INIT  = 0.0015             # GeV, initial mass-width floor
F_MAX   = 0.30               # tail fraction upper bound
K_MIN, K_MAX = 1.2, 4.0      # tail width ratio bounds
RCOV_VALID = (1e-5, 1.0)     # accepted r_cov range

MOTHER_PREFIXES = ("K_S0", "Lambda0")

LOG_SQRT_2PI = 0.5 * math.log(2.0 * math.pi)
SQRT2 = math.sqrt(2.0)

torch.manual_seed(17)
np.random.seed(17)


# ---------------------------------------------------------------- covariance
def cov_pack_index(a, b):
    """Packed lower-triangle index for a symmetric matrix, a >= b."""
    if a < b:
        a, b = b, a
    return a * (a + 1) // 2 + b


def unpack_cov6(arr21):
    """Generic (21,) -> (6, 6) symmetric unpack from the index formula."""
    M = np.empty((6, 6), dtype=float)
    for a in range(6):
        for b in range(a + 1):
            M[a, b] = M[b, a] = arr21[cov_pack_index(a, b)]
    return M


def _selftest_cov_packing():
    """Round-trip a synthetic symmetric 6x6 through the packing formula."""
    rng = np.random.default_rng(7)
    S = rng.standard_normal((6, 6))
    S = 0.5 * (S + S.T)
    packed = np.array([S[a, b] for a in range(6) for b in range(a + 1)])
    assert packed.size == 21
    assert np.allclose(unpack_cov6(packed), S), "cov packing self-test FAILED"


_selftest_cov_packing()


def cov_pt_frac_sigma(cov, phi):
    """r_cov-numerator: sigma(pT) from the packed covariance, per track.

    sigma^2(pT) = (px^2 Vxx + 2 px py Vxy + py^2 Vyy) / pT^2
                =  cos^2(phi) Vxx + 2 sin cos Vxy + sin^2(phi) Vyy.
    Momentum-block indices 9/13/14 hold for both the 21-entry (6x6 state) and
    28-entry (7x7 with E) layouts, because px, py stay at positions 3, 4.
    Returns (sigma_pT, layout_string).
    """
    ncol = cov.shape[1]
    if ncol == 21:
        layout = "6x6 (x,y,z,px,py,pz)"
    elif ncol == 28:
        layout = "7x7 (x,y,z,px,py,pz,E)"
    else:
        raise RuntimeError(f"Covariance branch has {ncol} entries; expected 21 or 28")
    ixx, ixy, iyy = (cov_pack_index(3, 3), cov_pack_index(4, 3),
                     cov_pack_index(4, 4))          # 9, 13, 14 by construction
    Vxx, Vxy, Vyy = cov[:, ixx], cov[:, ixy], cov[:, iyy]
    c, s = np.cos(phi), np.sin(phi)
    var = c * c * Vxx + 2.0 * s * c * Vxy + s * s * Vyy
    return np.sqrt(np.maximum(var, 0.0)), layout, Vxx, Vyy


# ---------------------------------------------------------------- data loading
def _read_root_species(path, tree_name, need_pdg, max_candidates=0, rng=None, label=""):
    """Read one KFParticle nTuple.  Returns a dict of numpy arrays.

    If max_candidates is non-zero, select candidates while iterating over ROOT
    chunks instead of materialising the full multi-GB tree before subsampling.
    """
    import uproot
    t = uproot.open(path)[tree_name]
    keys = set(t.keys())

    mother = next((p for p in MOTHER_PREFIXES if f"{p}_mass" in keys), None)
    if mother is None:
        cand = sorted({k[:-5] for k in keys
                       if k.endswith("_mass") and not k.startswith("track_")})
        if len(cand) != 1:
            raise RuntimeError(f"{path}: cannot identify mother prefix among {cand}")
        mother = cand[0]

    want = {f"{mother}_{x}": f"c{x}" for x in ("mass", "pT", "pseudorapidity", "phi")}
    for i in (1, 2):
        for x in ("pT", "pseudorapidity", "phi"):
            want[f"track_{i}_{x}"] = f"t{i}_{x}"
    opt = {}
    for i in (1, 2):
        for x in ("charge", "PDG_ID", "pTErr", "Covariance"):
            b = f"track_{i}_{x}"
            if b in keys:
                opt[b] = f"t{i}_{x}"
    missing = [b for b in want if b not in keys]
    if missing:
        raise RuntimeError(f"{path}: missing branches {missing}")
    if need_pdg and "track_2_PDG_ID" not in keys:
        raise RuntimeError(f"{path}: Lambda file needs track_i_PDG_ID branches")

    branches = list(want) + list(opt)

    def convert(raw):
        out = {"mother": mother}
        for b, name in {**want, **opt}.items():
            a = raw[b]
            if a.dtype == object:                   # jagged-stored fixed array
                a = np.stack(a)
            out[name] = np.asarray(a)
        return out

    if not max_candidates:
        print(f"[{label}] reading {t.num_entries} ROOT entries from {path}", flush=True)
        return convert(t.arrays(branches, library="np"))

    if rng is None:
        rng = np.random.default_rng(42)
    chunks = []
    n_keep = 0
    n_seen = 0
    print(f"[{label}] reading ROOT chunks until {max_candidates} selected candidates", flush=True)
    for raw in t.iterate(branches, library="np", step_size="100 MB"):
        d = convert(raw)
        n_chunk = d["cmass"].size
        n_seen += n_chunk

        if label == "K0s":
            lo, hi = 0.40, 0.60
        else:
            lo, hi = 1.08, 1.15
        m = np.isfinite(d["cmass"])
        for i in (1, 2):
            for x in ("pT", "pseudorapidity", "phi"):
                m &= np.isfinite(d[f"t{i}_{x}"])
        m &= (d["t1_pT"] > PT_SANITY) & (d["t2_pT"] > PT_SANITY)
        m &= (np.abs(d["t1_pseudorapidity"]) < ETA_RANGE[1])
        m &= (np.abs(d["t2_pseudorapidity"]) < ETA_RANGE[1])
        m &= (d["cmass"] > lo) & (d["cmass"] < hi)
        idx = np.flatnonzero(m)

        need = max_candidates - n_keep
        if idx.size > need:
            idx = rng.choice(idx, size=need, replace=False)
            idx.sort()
        chunks.append({k: (v[idx] if isinstance(v, np.ndarray) else v) for k, v in d.items()})
        n_keep += idx.size
        print(f"[{label}] scanned {n_seen} / {t.num_entries} entries, kept {n_keep}", flush=True)
        if n_keep >= max_candidates:
            break

    if not chunks:
        return convert(t.arrays(branches, entry_stop=0, library="np"))
    out = {"mother": mother}
    for k in chunks[0]:
        if isinstance(chunks[0][k], np.ndarray):
            out[k] = np.concatenate([c[k] for c in chunks])
    return out


def _read_csv_species(path):
    df = pd.read_csv(path)
    df.columns = [c.strip() for c in df.columns]
    mother = next((p for p in MOTHER_PREFIXES if f"{p}_mass" in df.columns), None)
    if mother is None:
        raise RuntimeError(f"{path}: no recognised mother prefix in columns")
    out = {"mother": mother}
    for x in ("mass", "pT", "pseudorapidity", "phi"):
        out[f"c{x}"] = df[f"{mother}_{x}"].to_numpy(float)
    for i in (1, 2):
        for x in ("pT", "pseudorapidity", "phi"):
            out[f"t{i}_{x}"] = df[f"track_{i}_{x}"].to_numpy(float)
        if f"track_{i}_PDG_ID" in df.columns:
            out[f"t{i}_PDG_ID"] = df[f"track_{i}_PDG_ID"].to_numpy()
    return out


def _species_selection(d, mass_lo, mass_hi, has_cov, label):
    """Eta acceptance + pT sanity + mass window + covariance validity.

    NO daughter pT cut (task 4): only a 1 MeV sanity floor.
    Rejection categories are counted and printed.
    """
    n0 = d["cmass"].size
    counts = {}

    def apply(mask, name):
        counts[name] = int((~mask).sum())
        return mask

    m = np.ones(n0, bool)
    fin = np.ones(n0, bool)
    for i in (1, 2):
        for x in ("pT", "pseudorapidity", "phi"):
            fin &= np.isfinite(d[f"t{i}_{x}"])
    fin &= np.isfinite(d["cmass"])
    m &= apply(fin, "non-finite kinematics")
    pts = (d["t1_pT"] > PT_SANITY) & (d["t2_pT"] > PT_SANITY)
    m &= apply(pts, f"pT <= {PT_SANITY} GeV sanity")
    eta = (np.abs(d["t1_pseudorapidity"]) < ETA_RANGE[1]) & \
          (np.abs(d["t2_pseudorapidity"]) < ETA_RANGE[1])
    m &= apply(eta, "|eta| acceptance")
    win = (d["cmass"] > mass_lo) & (d["cmass"] < mass_hi)
    m &= apply(win, "mass window")

    if has_cov:
        for i in (1, 2):
            cov = d[f"t{i}_Covariance"].astype(float)
            sig, layout, Vxx, Vyy = cov_pt_frac_sigma(cov, d[f"t{i}_phi"])
            if i == 1:
                print(f"[{label}] covariance layout: {layout}")
            rcov = sig / np.maximum(d[f"t{i}_pT"], 1e-12)
            ok_fin = np.isfinite(sig) & np.isfinite(Vxx) & np.isfinite(Vyy)
            ok_pos = (Vxx > 0) & (Vyy > 0)
            ok_rng = (rcov > RCOV_VALID[0]) & (rcov < RCOV_VALID[1])
            m &= apply(ok_fin, f"track_{i} cov non-finite")
            m &= apply(ok_pos, f"track_{i} cov diag <= 0")
            m &= apply(ok_rng, f"track_{i} r_cov outside {RCOV_VALID}")
            d[f"t{i}_rcov"] = rcov
            d[f"t{i}_sigpt"] = sig

    for name, ncut in counts.items():
        if ncut:
            print(f"[{label}] rejected {ncut:7d} : {name}")
    print(f"[{label}] selected {int(m.sum())} / {n0} candidates "
          f"(no daughter pT cut)")
    return {k: (v[m] if isinstance(v, np.ndarray) else v) for k, v in d.items()}


def _pterr_crosscheck(d, label):
    """sqrt(cov projection) vs the stored pTErr branch; abort on bulk mismatch."""
    for i in (1, 2):
        if f"t{i}_pTErr" not in d or f"t{i}_sigpt" not in d:
            continue
        pterr = d[f"t{i}_pTErr"].astype(float)
        ok = np.isfinite(pterr) & (pterr > 0)
        if ok.sum() < 100:
            continue
        ratio = d[f"t{i}_sigpt"][ok] / pterr[ok]
        med = float(np.median(ratio))
        frac_bad = float(np.mean(np.abs(ratio - 1.0) > 0.2))
        print(f"[{label}] track_{i}: median sigma_pT(cov)/pTErr = {med:.4f}, "
              f"frac |ratio-1|>20% = {frac_bad:.3f}")
        if abs(med - 1.0) > 0.2:
            sys.exit(f"[{label}] FATAL: covariance-projected sigma(pT) disagrees "
                     f"with pTErr by >20% on the bulk -- packing convention is "
                     f"wrong; everything downstream would be invalid.")


def _subsample(d, n_max, rng, label):
    n = d["cmass"].size
    if n_max and n > n_max:
        idx = rng.choice(n, size=n_max, replace=False)
        idx.sort()
        d = {k: (v[idx] if isinstance(v, np.ndarray) else v) for k, v in d.items()}
        print(f"[{label}] subsampled {n} -> {n_max} candidates (--max_candidates)")
    return d


def load(args):
    """Returns (K, L, meta).  meta carries has_cov, the curvature-feature
    normalisation, the loss-bin pT edges and the data-driven plot ranges."""
    rng = np.random.default_rng(args.subsample_seed)

    def read(path, need_pdg):
        if path.endswith(".csv"):
            return _read_csv_species(path), False
        d = _read_root_species(path, args.tree, need_pdg,
                               max_candidates=args.max_candidates,
                               rng=rng, label=("Lambda" if need_pdg else "K0s"))
        return d, ("t1_Covariance" in d and "t2_Covariance" in d)

    dK, covK = read(args.kshort, need_pdg=False)
    dL, covL = read(args.lam, need_pdg=True)
    has_cov = covK and covL
    if args.reso_model == "pull" and not has_cov:
        sys.exit("--reso_model pull requires Covariance branches in BOTH inputs "
                 "(ROOT ntuples). CSV input carries none: use --reso_model absolute.")

    dK = _species_selection(dK, 0.40, 0.60, covK, "K0s")
    dL = _species_selection(dL, 1.08, 1.15, covL, "Lambda")
    if has_cov:
        _pterr_crosscheck(dK, "K0s")
        _pterr_crosscheck(dL, "Lambda")
    dK = _subsample(dK, args.max_candidates, rng, "K0s")
    dL = _subsample(dL, args.max_candidates, rng, "Lambda")

    T = lambda x: torch.tensor(np.ascontiguousarray(x), dtype=torch.float64)

    # K0s daughter charges: branch if present, else pi- / pi+ convention
    if "t1_charge" in dK:
        q1K = T(np.sign(dK["t1_charge"]).astype(float))
        q2K = T(np.sign(dK["t2_charge"]).astype(float))
    else:
        q1K = torch.full((dK["cmass"].size,), -1.0, dtype=torch.float64)
        q2K = torch.full((dK["cmass"].size,), +1.0, dtype=torch.float64)

    K = dict(pt1=T(dK["t1_pT"]), eta1=T(dK["t1_pseudorapidity"]), phi1=T(dK["t1_phi"]),
             pt2=T(dK["t2_pT"]), eta2=T(dK["t2_pseudorapidity"]), phi2=T(dK["t2_phi"]),
             q1=q1K, q2=q2K, m1=PION_MASS, m2=PION_MASS, M=M_K0S,
             mass=T(dK["cmass"]), cpt=T(dK["cpT"]),
             ceta=T(dK["cpseudorapidity"]), cphi=T(dK["cphi"]))
    qpi = torch.tensor(np.sign(dL["t1_PDG_ID"]).astype(float), dtype=torch.float64)
    qp  = torch.tensor(np.sign(dL["t2_PDG_ID"]).astype(float), dtype=torch.float64)
    L = dict(pt1=T(dL["t1_pT"]), eta1=T(dL["t1_pseudorapidity"]), phi1=T(dL["t1_phi"]),
             pt2=T(dL["t2_pT"]), eta2=T(dL["t2_pseudorapidity"]), phi2=T(dL["t2_phi"]),
             q1=qpi, q2=qp, m1=PION_MASS, m2=PROTON_MASS, M=M_LAMBDA,
             mass=T(dL["cmass"]),
             is_lam=torch.tensor(dL["t2_PDG_ID"] > 0, dtype=torch.bool),
             cpt=T(dL["cpT"]), ceta=T(dL["cpseudorapidity"]), cphi=T(dL["cphi"]))
    if has_cov:
        for S, d in ((K, dK), (L, dL)):
            S["rcov1"] = T(d["t1_rcov"])
            S["rcov2"] = T(d["t2_rcov"])

    # -------- data-driven curvature normalisation, bins and plot ranges
    all_pt = np.concatenate([dK["t1_pT"], dK["t2_pT"], dL["t1_pT"], dL["t2_pT"]])
    u = 1.0 / all_pt
    q16, q50, q84 = np.quantile(u, [0.16, 0.50, 0.84])
    u_ref = float(q50)
    u_scale = float(max(0.5 * (q84 - q16), 1e-3))    # robust sigma
    u_lo, u_hi = np.quantile(u, [0.005, 0.995])
    pt_lo, pt_hi = np.quantile(all_pt, [0.005, 0.995])
    pt_lo = min(float(pt_lo), float(args.lookup_pt_min))
    pt_hi = max(float(pt_hi), float(args.lookup_pt_max))
    u_lo = 1.0 / pt_hi
    u_hi = 1.0 / pt_lo
    pt_edges = np.unique(np.quantile(all_pt, np.linspace(0.0, 1.0, 7)))
    if pt_edges.size < 4:                            # degenerate: fall back
        pt_edges = np.linspace(pt_lo, pt_hi, 7)
    pt_slices = semi_even_pt_grid(pt_lo, pt_hi, args.kappa_slices)
    curv_slices = 1.0 / pt_slices

    meta = dict(has_cov=has_cov,
                norm=dict(u_ref=u_ref, u_scale=u_scale, u_clamp=U_CLAMP),
                pt_edges=pt_edges, pt_slices=pt_slices, curv_slices=curv_slices,
                pt_plot=(float(pt_lo), float(pt_hi)),
                curv_plot=(float(u_lo), float(u_hi)))
    print(f"[norm] curvature feature: u_ref = {u_ref:.4f} GeV^-1, "
          f"u_scale = {u_scale:.4f} GeV^-1, clamp +-{U_CLAMP}")
    print(f"[bins] daughter pT loss-bin edges (data quantiles): "
          f"{np.array2string(pt_edges, precision=3)}")
    return K, L, meta


# ---------------------------------------------------------------- scale model
CONVENTION_TAG = "curvature_mult_uncapped_v4"


class KappaNet(nn.Module):
    """x = (u_n, eta_n, sin phi, cos phi) -> (eps, delta), UNCAPPED.

    u = 1/pT (unsigned curvature); u_n = clamp((u - u_ref)/u_scale, +-U_CLAMP).
    The clamp is a numerical guard: outside it kappa is constant in curvature.
    Convention:  (1/pT)_corr = (1/pT) * (1 + eps + q*delta);  kappa() returns
    the pT MULTIPLIER 1/(1+eps+q*delta) so call sites keep pT_corr = kappa*pT.
    q enters only through the output combination -> exact even/odd split in
    curvature space.  Near-identity init; the annealed L2 prior in train() is
    the only soft pull to identity and vanishes at the last epoch.
    """

    def __init__(self, norm, hidden=48, layers=3):
        super().__init__()
        self.u_ref = float(norm["u_ref"])
        self.u_scale = float(norm["u_scale"])
        self.u_clamp = float(norm["u_clamp"])
        dims = [4] + [hidden] * layers + [2]
        seq = []
        for i in range(len(dims) - 1):
            seq.append(nn.Linear(dims[i], dims[i + 1]))
            if i < len(dims) - 2:
                seq.append(nn.SiLU())
        self.net = nn.Sequential(*seq)
        with torch.no_grad():
            self.net[-1].weight *= 0.01
            self.net[-1].bias.zero_()          # eps = delta = 0  ->  kappa = 1

    def norm_dict(self):
        return dict(u_ref=self.u_ref, u_scale=self.u_scale, u_clamp=self.u_clamp)

    def features(self, pt, eta, phi):
        u = 1.0 / pt.clamp(min=1e-6)
        u_n = ((u - self.u_ref) / self.u_scale).clamp(-self.u_clamp, self.u_clamp)
        return torch.stack([u_n, eta / 1.0, torch.sin(phi), torch.cos(phi)], -1)

    def u_clamp_frac(self, pt):
        with torch.no_grad():
            u = 1.0 / pt.clamp(min=1e-6)
            u_n = (u - self.u_ref) / self.u_scale
            return float((u_n.abs() >= self.u_clamp).double().mean())

    def eps_delta(self, pt, eta, phi):
        out = self.net(self.features(pt, eta, phi))
        return out[..., 0], out[..., 1]

    def kappa(self, pt, eta, phi, q):
        """Returns (pT multiplier, eps, delta);  pT_corr = multiplier * pT."""
        eps, dlt = self.eps_delta(pt, eta, phi)
        kcurv = (1.0 + eps + q * dlt).clamp(min=KAPPA_GUARD)
        return 1.0 / kcurv, eps, dlt


class IdentityKappa(nn.Module):
    """kappa == 1; used to run stage 2 on injection toys."""

    def kappa(self, pt, eta, phi, q):
        z = torch.zeros_like(pt)
        return torch.ones_like(pt), z, z


def save_stage1(model, path):
    torch.save({"state_dict": model.state_dict(),
                "norm": model.norm_dict(),
                "convention": CONVENTION_TAG}, path)


def load_stage1(path, hidden):
    ckpt = torch.load(path, weights_only=False)
    if not (isinstance(ckpt, dict) and ckpt.get("convention") == CONVENTION_TAG):
        sys.exit(f"--load_stage1 {path}: checkpoint predates the uncapped "
                 f"curvature convention '{CONVENTION_TAG}' (or is a bare "
                 f"state_dict). It would load silently with WRONG semantics. "
                 f"Retrain stage 1.")
    model = KappaNet(ckpt["norm"], hidden=hidden).double()
    model.load_state_dict(ckpt["state_dict"])
    return model


# ---------------------------------------------------------------- kinematics
def inv_mass(pt1, eta1, phi1, m1, pt2, eta2, phi2, m2):
    px1, py1, pz1 = pt1 * torch.cos(phi1), pt1 * torch.sin(phi1), pt1 * torch.sinh(eta1)
    px2, py2, pz2 = pt2 * torch.cos(phi2), pt2 * torch.sin(phi2), pt2 * torch.sinh(eta2)
    e1 = torch.sqrt(px1**2 + py1**2 + pz1**2 + m1**2)
    e2 = torch.sqrt(px2**2 + py2**2 + pz2**2 + m2**2)
    m2v = (e1 + e2)**2 - (px1 + px2)**2 - (py1 + py2)**2 - (pz1 + pz2)**2
    return torch.sqrt(m2v.clamp(min=1e-12))


def _p4(pt, eta, phi, m):
    px, py, pz = pt * torch.cos(phi), pt * torch.sin(phi), pt * torch.sinh(eta)
    p2 = px * px + py * py + pz * pz
    return px, py, pz, torch.sqrt(p2 + m * m), p2


def mass_jacobians(pt1, eta1, phi1, m1, pt2, eta2, phi2, m2):
    """(m, J1, J2), J_i = dm/dln(pT_i) at fixed (eta, phi); not leg-symmetric."""
    px1, py1, pz1, E1, p1sq = _p4(pt1, eta1, phi1, m1)
    px2, py2, pz2, E2, p2sq = _p4(pt2, eta2, phi2, m2)
    msq = (E1 + E2)**2 - (px1 + px2)**2 - (py1 + py2)**2 - (pz1 + pz2)**2
    m = torch.sqrt(msq.clamp(min=1e-12))
    pdot = px1 * px2 + py1 * py2 + pz1 * pz2
    J1 = (E2 * p1sq / E1 - pdot) / m
    J2 = (E1 * p2sq / E2 - pdot) / m
    return m, J1, J2


def beta_of(pt, eta, m):
    p = pt * torch.cosh(eta)
    return p / torch.sqrt(p * p + m * m)


# ---------------------------------------------------------------- signal weights
def gauss_lin(x, N, mu, sig, a, b):
    return N * np.exp(-0.5 * ((x - mu) / sig)**2) + a + b * x


def fit_peak(masses, mu0, sig0, lo, hi, nbins=100, weights=None):
    """Gaussian+linear fit with floating peak. Returns (mu, sig, popt)."""
    h, edges = np.histogram(masses, bins=nbins, range=(lo, hi), weights=weights)
    c = 0.5 * (edges[:-1] + edges[1:])
    p0 = [h.max() - np.median(h), mu0, sig0, np.median(h), 0.0]
    try:
        popt, _ = curve_fit(gauss_lin, c, h, p0=p0,
                            sigma=np.sqrt(np.maximum(h, 1)), maxfev=20000)
        if not (lo < popt[1] < hi) or not (0.2 * sig0 < abs(popt[2]) < 5 * sig0):
            raise RuntimeError
    except Exception:
        popt = p0  # fall back to seed; weights degrade gracefully
    return popt[1], abs(popt[2]), popt


def signal_weights(masses, popt):
    s = popt[0] * np.exp(-0.5 * ((masses - popt[1]) / popt[2])**2)
    b = np.maximum(popt[3] + popt[4] * masses, 0.0)
    w = s / np.maximum(s + b, 1e-9)
    return np.clip(w, 0.0, 1.0)


# ---------------------------------------------------------------- binning
def semi_even_pt_grid(pt_min, pt_max, n):
    """Human-readable pT grid: dense below 3 GeV, sparse at the high end."""
    anchors = np.array([0.10, 0.25, 0.50, 0.75, 1.00, 1.25, 1.50, 1.75,
                        2.00, 2.25, 2.50, 2.75, 3.00, 4.00, 5.00], dtype=float)
    lo, hi = float(pt_min), float(pt_max)
    vals = anchors[(anchors >= lo) & (anchors <= hi)]
    vals = np.unique(np.concatenate(([lo], vals, [hi])))
    if vals.size == n:
        return vals
    q = np.linspace(0.0, 1.0, n)
    return np.interp(q, np.linspace(0.0, 1.0, vals.size), vals)


def bin_index(pt, eta, phi, pt_edges, n_eta, n_phi):
    ipt = torch.bucketize(pt, pt_edges) - 1
    ipt = ipt.clamp(0, len(pt_edges) - 2)
    ieta = ((eta - ETA_RANGE[0]) / (ETA_RANGE[1] - ETA_RANGE[0]) * n_eta).long().clamp(0, n_eta - 1)
    iphi = ((phi + np.pi) / (2 * np.pi) * n_phi).long().clamp(0, n_phi - 1)
    return (ipt * n_eta + ieta) * n_phi + iphi, (len(pt_edges) - 1) * n_eta * n_phi


def binned_mean_sq(idx, nbins, values, weights, min_w=5.0):
    """sum_b w_b * mean_b^2, weighted means via scatter-add (differentiable)."""
    sw = torch.zeros(nbins, dtype=values.dtype).index_add_(0, idx, weights)
    swv = torch.zeros(nbins, dtype=values.dtype).index_add_(0, idx, weights * values)
    mask = sw > min_w
    mean = swv[mask] / sw[mask]
    return (sw[mask] * mean**2).sum() / sw[mask].sum().clamp(min=1e-9)


def corrected_mass(model, S):
    k1, e1, d1 = model.kappa(S["pt1"], S["eta1"], S["phi1"], S["q1"])
    k2, e2, d2 = model.kappa(S["pt2"], S["eta2"], S["phi2"], S["q2"])
    m = inv_mass(k1 * S["pt1"], S["eta1"], S["phi1"], S["m1"],
                 k2 * S["pt2"], S["eta2"], S["phi2"], S["m2"])
    reg = (e1**2 + d1**2 + e2**2 + d2**2).mean()
    return m, (k1, k2), reg


# ---------------------------------------------------------------- stage 1
def train(args, K, L, meta):
    model = KappaNet(meta["norm"], hidden=args.hidden).double()
    opt = torch.optim.Adam(model.parameters(), lr=args.lr, weight_decay=1e-5)

    pt_edges = torch.tensor(meta["pt_edges"], dtype=torch.float64)
    N_ETA, N_PHI = 10, 8

    def refresh_weights():
        with torch.no_grad():
            mK, _, _ = corrected_mass(model, K)
            mL, _, _ = corrected_mass(model, L)
        muK, sK, pK = fit_peak(mK.numpy(), M_K0S, 0.010, 0.42, 0.58)
        muL, sL, pL = fit_peak(mL.numpy(), M_LAMBDA, 0.004, 1.09, 1.145)
        K["w"] = torch.tensor(signal_weights(mK.numpy(), pK))
        L["w"] = torch.tensor(signal_weights(mL.numpy(), pL))
        return (muK, sK), (muL, sL)

    (muK0, _), (muL0, _) = refresh_weights()
    print(f"raw peaks:  K0s {muK0*1e3:8.3f} MeV (PDG {M_K0S*1e3:.3f}), "
          f"Lambda {muL0*1e3:8.3f} MeV (PDG {M_LAMBDA*1e3:.3f})")

    for ep in range(args.epochs):
        opt.zero_grad()
        loss = torch.tensor(0.0, dtype=torch.float64)
        prior_w = args.lam_prior * (1.0 - ep / max(args.epochs - 1, 1))  # anneal -> 0

        for S in (K, L):
            m, _, reg = corrected_mass(model, S)
            pull = (m - S["M"]) / (0.010 if S is K else 0.004)
            w = S["w"]
            for (pt, eta, phi, q) in ((S["pt1"], S["eta1"], S["phi1"], S["q1"]),
                                      (S["pt2"], S["eta2"], S["phi2"], S["q2"])):
                idx, nb = bin_index(pt, eta, phi, pt_edges, N_ETA, N_PHI)
                idx = idx + ((q > 0).long() * nb)
                loss = loss + binned_mean_sq(idx, 2 * nb, pull, w)
            loss = loss + prior_w * reg

        mL, _, _ = corrected_mass(model, L)
        pullL = (mL - M_LAMBDA) / 0.004
        idx, nb = bin_index(L["pt1"], L["eta1"], L["phi1"], pt_edges, N_ETA, N_PHI)
        sel_l, sel_b = L["is_lam"], ~L["is_lam"]
        wl = L["w"]
        swl  = torch.zeros(nb, dtype=torch.float64).index_add_(0, idx[sel_l], wl[sel_l])
        swvl = torch.zeros(nb, dtype=torch.float64).index_add_(0, idx[sel_l], (wl * pullL)[sel_l])
        swb  = torch.zeros(nb, dtype=torch.float64).index_add_(0, idx[sel_b], wl[sel_b])
        swvb = torch.zeros(nb, dtype=torch.float64).index_add_(0, idx[sel_b], (wl * pullL)[sel_b])
        mk = (swl > 5) & (swb > 5)
        split = swvl[mk] / swl[mk] - swvb[mk] / swb[mk]
        wsum = (swl[mk] * swb[mk]) / (swl[mk] + swb[mk])
        loss = loss + args.lam_split * (wsum * split**2).sum() / wsum.sum().clamp(min=1e-9)

        k1, _, _ = model.kappa(K["pt1"], K["eta1"], K["phi1"], K["q1"])
        k2, _, _ = model.kappa(K["pt2"], K["eta2"], K["phi2"], K["q2"])
        p1, p2 = k1 * K["pt1"], k2 * K["pt2"]
        alpha = (p1 - p2) / (p1 + p2)                      # pi- minus pi+
        idxc, nbc = bin_index(K["cpt"], K["ceta"], K["cphi"],
                              pt_edges, N_ETA, N_PHI)
        loss = loss + args.lam_alpha * binned_mean_sq(idxc, nbc, alpha, K["w"])

        loss.backward()
        opt.step()

        if ep % args.refresh == args.refresh - 1:
            (muK, _), (muL, _) = refresh_weights()
            print(f"ep {ep+1:4d}  loss {loss.item():9.5f}  "
                  f"K0s peak {muK*1e3:8.3f}  Lam peak {muL*1e3:8.3f} MeV")

    return model


# ================================================================ STAGE 2
class ResoNet(nn.Module):
    """(eta, sin phi, cos phi) -> fields, mode-dependent.

    absolute: two heads (a, b), r^2 = (a/beta)^2 + (b pT)^2 -- a MEASUREMENT
              of the resolution.
    pull:     one head s(eta, phi), r = s * pT^{u_p} * r_cov -- a CALIBRATION
              of the covariance-reported resolution (s == 1, u_p == 0 means
              the tracker's errors are right).
    Deliberately NOT a function of pT (identifiability); species enters only
    through beta (absolute) or through r_cov itself (pull).
    """

    def __init__(self, mode, hidden=32, layers=3):
        super().__init__()
        self.mode = mode
        n_out = 2 if mode == "absolute" else 1
        dims = [3] + [hidden] * layers + [n_out]
        seq = []
        for i in range(len(dims) - 1):
            seq.append(nn.Linear(dims[i], dims[i + 1]))
            if i < len(dims) - 2:
                seq.append(nn.SiLU())
        self.net = nn.Sequential(*seq)
        with torch.no_grad():
            self.net[-1].weight *= 0.01
            if mode == "absolute":
                # r ~ 1.5% at 1 GeV pion: a ~ b ~ 0.0106; 0.05*softplus(z)=0.0106
                self.net[-1].bias.fill_(-1.443)
            else:
                # s = S_SCALE*softplus(z) = 1  ->  softplus(z) = 0.5
                self.net[-1].bias.fill_(math.log(math.expm1(1.0 / S_SCALE)))

    def features(self, eta, phi):
        return torch.stack([eta / 1.0, torch.sin(phi), torch.cos(phi)], -1)

    def ab(self, eta, phi):
        out = self.net(self.features(eta, phi))
        return A_SCALE * F.softplus(out[..., 0]), B_SCALE * F.softplus(out[..., 1])

    def s(self, eta, phi):
        out = self.net(self.features(eta, phi))
        return S_SCALE * F.softplus(out[..., 0])


class ResoGlobals(nn.Module):
    """Floor c (1 or per-resonance), per-group (f, k), and the pull exponent u_p."""

    def __init__(self, n_groups, n_c=1):
        super().__init__()
        c_raw0 = math.log(math.expm1(C_INIT))          # softplus^-1(C_INIT)
        self.c_raw = nn.Parameter(torch.full((n_c,), c_raw0, dtype=torch.float64))
        self.f_raw = nn.Parameter(torch.full((n_groups,), -2.0, dtype=torch.float64))
        self.k_raw = nn.Parameter(torch.zeros(n_groups, dtype=torch.float64))
        self.up = nn.Parameter(torch.zeros(1, dtype=torch.float64))   # pull only

    def c(self, i=0):
        return F.softplus(self.c_raw[i])

    def f(self, i):
        return F_MAX * torch.sigmoid(self.f_raw[i])

    def k(self, i):
        return K_MIN + (K_MAX - K_MIN) * torch.sigmoid(self.k_raw[i])

    def u_p(self):
        return self.up[0]


def r_terms(a, b, pt, beta):
    return a / beta, b * pt


def sigma_mass_ab(R, a1, b1, a2, b2, c):
    t1a, t1b = r_terms(a1, b1, R["pt1"], R["beta1"])
    t2a, t2b = r_terms(a2, b2, R["pt2"], R["beta2"])
    r1sq = t1a**2 + t1b**2
    r2sq = t2a**2 + t2b**2
    return torch.sqrt((R["J1"]**2 * r1sq + R["J2"]**2 * r2sq + c**2).clamp(min=1e-12))


def pull_r(R, leg, s, up):
    return s * R[f"pt{leg}"].clamp(min=1e-6)**up * R[f"rcov{leg}"]


def sigma_mass_pull(R, s1, s2, up, c):
    r1 = pull_r(R, 1, s1, up)
    r2 = pull_r(R, 2, s2, up)
    return torch.sqrt((R["J1"]**2 * r1**2 + R["J2"]**2 * r2**2 + c**2).clamp(min=1e-12))


def sigma_mass_net(R, net, glob, c):
    if net.mode == "absolute":
        a1, b1 = net.ab(R["eta1"], R["phi1"])
        a2, b2 = net.ab(R["eta2"], R["phi2"])
        return sigma_mass_ab(R, a1, b1, a2, b2, c)
    s1 = net.s(R["eta1"], R["phi1"])
    s2 = net.s(R["eta2"], R["phi2"])
    return sigma_mass_pull(R, s1, s2, glob.u_p(), c)


def log_mixture_pdf(m, M, sigma, f, k, lo, hi):
    """ln of the window-normalised core+tail Gaussian mixture."""
    s1, s2 = sigma, k * sigma
    lg1 = -0.5 * ((m - M) / s1)**2 - torch.log(s1) - LOG_SQRT_2PI
    lg2 = -0.5 * ((m - M) / s2)**2 - torch.log(s2) - LOG_SQRT_2PI
    lp = torch.logaddexp(torch.log1p(-f) + lg1, torch.log(f) + lg2)
    frac = lambda s: 0.5 * (torch.erf((hi - M) / (s * SQRT2))
                            - torch.erf((lo - M) / (s * SQRT2)))
    Z = (1.0 - f) * frac(s1) + f * frac(s2)
    return lp - torch.log(Z.clamp(min=1e-12))


def width_nll_sum(R, M, sigma, w, f, k, lo, hi):
    return -(w * log_mixture_pdf(R["m"], M, sigma, f, k, lo, hi)).sum()


# ---------------------------------------------------------------- reso inputs
def prepare_reso_inputs(model, S):
    """Freeze the scale, precompute everything stage 2 needs (constant with
    KappaNet frozen).  Jacobians and beta use the CORRECTED momenta.  r_cov is
    carried through unchanged: it is a fractional resolution, invariant under
    the kappa scale to first order (and identical in pT and curvature)."""
    with torch.no_grad():
        k1, _, _ = model.kappa(S["pt1"], S["eta1"], S["phi1"], S["q1"])
        k2, _, _ = model.kappa(S["pt2"], S["eta2"], S["phi2"], S["q2"])
        pt1, pt2 = k1 * S["pt1"], k2 * S["pt2"]
        m, J1, J2 = mass_jacobians(pt1, S["eta1"], S["phi1"], S["m1"],
                                   pt2, S["eta2"], S["phi2"], S["m2"])
        if "dm" in S:            # injection toys only: explicit floor smearing
            m = m + S["dm"]
        R = dict(m=m, J1=J1, J2=J2,
                 pt1=pt1, eta1=S["eta1"], phi1=S["phi1"],
                 pt2=pt2, eta2=S["eta2"], phi2=S["phi2"],
                 beta1=beta_of(pt1, S["eta1"], S["m1"]),
                 beta2=beta_of(pt2, S["eta2"], S["m2"]),
                 cpt=S["cpt"], ceta=S["ceta"], cphi=S["cphi"],
                 m1=S["m1"], m2=S["m2"])
        for key in ("rcov1", "rcov2"):
            if key in S:
                R[key] = S[key]
    return R


def subset_R(R, sel):
    out = {}
    for k, v in R.items():
        out[k] = v[sel] if torch.is_tensor(v) else v
    return out


def build_groups(model, K, L, pure_signal=False):
    """Three groups: K0s, Lambda, Lambdabar (own tail params; shared ResoNet)."""
    RK = prepare_reso_inputs(model, K)
    RL = prepare_reso_inputs(model, L)

    if pure_signal:
        wK = torch.ones_like(RK["m"])
        wL = torch.ones_like(RL["m"])
    else:
        _, _, pK = fit_peak(RK["m"].numpy(), M_K0S, SIG0["K0s"], *WIN["K0s"])
        _, _, pL = fit_peak(RL["m"].numpy(), M_LAMBDA, SIG0["Lambda"], *WIN["Lambda"])
        wK = torch.tensor(signal_weights(RK["m"].numpy(), pK))
        wL = torch.tensor(signal_weights(RL["m"].numpy(), pL))

    isl = L["is_lam"]
    groups = [
        dict(name="K0s",       R=RK,                  w=wK,        c_idx=0),
        dict(name="Lambda",    R=subset_R(RL, isl),   w=wL[isl],   c_idx=0),
        dict(name="Lambdabar", R=subset_R(RL, ~isl),  w=wL[~isl],  c_idx=0),
    ]
    for g in groups:
        g["M"] = MPDG[g["name"]]
        g["lo"], g["hi"] = WIN[g["name"]]
    return groups


def set_c_indices(groups, per_resonance):
    if not per_resonance:
        for g in groups:
            g["c_idx"] = 0
        return 1
    for g in groups:
        g["c_idx"] = 0 if g["name"] == "K0s" else 1
    return 2


# ---------------------------------------------------------------- stage 2 train
def train_resolution(groups, args, n_c=1, verbose=True):
    net = ResoNet(args.reso_model, hidden=args.reso_hidden,
                  layers=args.reso_layers).double()
    glob = ResoGlobals(len(groups), n_c=n_c).double()
    # globals get 5x LR: Adam stalls along the c <-> (s, u_p) soft direction
    opt = torch.optim.Adam([
        {"params": net.parameters(), "lr": args.reso_lr},
        {"params": glob.parameters(), "lr": 5.0 * args.reso_lr},
    ], weight_decay=0.0)
    wtot = sum(float(g["w"].sum()) for g in groups)

    def total_nll():
        nll = torch.tensor(0.0, dtype=torch.float64)
        for gi, g in enumerate(groups):
            sig = sigma_mass_net(g["R"], net, glob, glob.c(g["c_idx"]))
            nll = nll + width_nll_sum(g["R"], g["M"], sig, g["w"],
                                      glob.f(gi), glob.k(gi), g["lo"], g["hi"])
        return nll

    for ep in range(args.reso_epochs):
        opt.zero_grad()
        loss = total_nll() / wtot
        loss.backward()
        opt.step()
        if verbose and ep % max(args.reso_epochs // 10, 1) == 0:
            with torch.no_grad():
                cs = " ".join(f"c{i}={float(glob.c(i))*1e3:.3f}" for i in range(n_c))
                extra = (f" u_p={float(glob.u_p()):+.4f}"
                         if args.reso_model == "pull" else "")
                fs = " ".join(f"{g['name'][:4]}: f={float(glob.f(gi)):.3f} "
                              f"k={float(glob.k(gi)):.2f}"
                              for gi, g in enumerate(groups))
            print(f"  reso ep {ep+1:4d}  -lnL/w {loss.item():10.5f}  "
                  f"{cs} MeV{extra}   {fs}")

    # LBFGS polish of the GLOBALS with the field frozen
    for p in net.parameters():
        p.requires_grad_(False)
    with torch.no_grad():
        best = float(total_nll())
        state0 = {k: v.clone() for k, v in glob.state_dict().items()}
    lb = torch.optim.LBFGS(list(glob.parameters()), lr=0.5, max_iter=100,
                           tolerance_grad=1e-10, line_search_fn="strong_wolfe")

    def closure():
        lb.zero_grad()
        v = total_nll()
        v.backward()
        return v
    try:
        lb.step(closure)
    except Exception as e:
        print(f"  [train_resolution] LBFGS polish skipped: {e}")
    with torch.no_grad():
        v_pol = float(total_nll())
    if not np.isfinite(v_pol) or v_pol > best:
        glob.load_state_dict(state0)
        print("  [train_resolution] LBFGS polish rejected (no improvement)")
    else:
        print(f"  [train_resolution] LBFGS polish: nll {best:.3f} -> {v_pol:.3f}")
    for p in net.parameters():
        p.requires_grad_(True)
    return net, glob


# ---------------------------------------------------------------- reduced fit
def fit_global_reduced(groups, mode, n_c=1, steps=3000, lr=0.02):
    """Reduced fit with constant fields, for a Hessian covariance.

    absolute: theta = (ln a, ln b, c..., f..., k...)
    pull:     theta = (ln s, u_p,  c..., f..., k...)
    Purpose is the injection go/no-go ('recovered within uncertainties');
    ResoNet itself carries no error bars.
    """
    ng = len(groups)
    if mode == "absolute":
        head0 = [math.log(0.0106), math.log(0.0106)]
    else:
        head0 = [0.0, 0.0]                       # ln s = 0, u_p = 0
    theta0 = torch.tensor(head0 +
                          [math.log(math.expm1(C_INIT))] * n_c +
                          [-2.0] * ng + [0.0] * ng, dtype=torch.float64)

    def unpack(t):
        # numerical bounds only; they never bind at a physical minimum
        if mode == "absolute":
            h1, h2 = torch.exp(t[0].clamp(-12, 2)), torch.exp(t[1].clamp(-12, 2))
        else:
            h1, h2 = torch.exp(t[0].clamp(-6, 6)), t[1].clamp(-3, 3)   # s, u_p
        c = F.softplus(t[2:2 + n_c])
        f = F_MAX * torch.sigmoid(t[2 + n_c:2 + n_c + ng])
        k = K_MIN + (K_MAX - K_MIN) * torch.sigmoid(t[2 + n_c + ng:])
        return h1, h2, c, f, k

    def nll(t):
        h1, h2, c, f, k = unpack(t)
        tot = torch.tensor(0.0, dtype=torch.float64)
        for gi, g in enumerate(groups):
            R = g["R"]
            if mode == "absolute":
                sig = sigma_mass_ab(R, h1, h2, h1, h2, c[g["c_idx"]])
            else:
                sig = sigma_mass_pull(R, h1, h1, h2, c[g["c_idx"]])
            tot = tot + width_nll_sum(R, g["M"], sig, g["w"], f[gi], k[gi],
                                      g["lo"], g["hi"])
        return tot

    t = theta0.clone().requires_grad_(True)
    opt = torch.optim.Adam([t], lr=lr)
    best, t_best = float("inf"), theta0.clone()
    for _ in range(steps):
        opt.zero_grad()
        v = nll(t)
        v.backward()
        opt.step()
        fv = float(v.detach())
        if np.isfinite(fv) and fv < best:
            best, t_best = fv, t.detach().clone()

    lb = torch.optim.LBFGS([t], lr=0.5, max_iter=200,
                           tolerance_grad=1e-10, line_search_fn="strong_wolfe")

    def closure():
        lb.zero_grad()
        v = nll(t)
        v.backward()
        return v
    try:
        lb.step(closure)
    except Exception as e:
        print(f"  [fit_global_reduced] LBFGS polish skipped: {e}")
    with torch.no_grad():
        v_pol = float(nll(t)) if torch.isfinite(t).all() else float("inf")
    if not np.isfinite(v_pol) or v_pol > best:
        with torch.no_grad():
            t.copy_(t_best)
    else:
        best = v_pol

    td = t.detach()
    H = torch.autograd.functional.hessian(nll, td)
    if not torch.isfinite(H).all():
        print("  [fit_global_reduced] Hessian non-finite at the polished point; "
              "retrying at the Adam-best point")
        td = t_best.clone()
        H = torch.autograd.functional.hessian(nll, td)
    pd, ev = False, None
    cov = torch.full_like(H, float("nan"))
    if torch.isfinite(H).all():
        try:
            ev = torch.linalg.eigvalsh(0.5 * (H + H.T))
            tol = 1e-8 * float(ev.abs().max())
            pd = bool((ev > -tol).all())
            cov = torch.linalg.inv(H) if pd else torch.linalg.pinv(H)
        except Exception as e:
            print(f"  [fit_global_reduced] eigen-decomposition failed: {e}")
    else:
        print("  [fit_global_reduced] Hessian contains non-finite entries")
    if not torch.isfinite(cov).all():
        print("  [fit_global_reduced] covariance not usable; errors reported as nan")
        cov = torch.full_like(H, float("nan"))
    if ev is not None and not pd:
        print(f"  [fit_global_reduced] Hessian not positive definite "
              f"(min eig {float(ev.min()):.3e}) -- fit is not at a minimum")

    def _err(i):
        v = float(cov[i, i])
        return math.sqrt(v) if np.isfinite(v) and v > 0 else float("nan")

    def _rho(i, j):
        d = math.sqrt(float(cov[i, i]) * float(cov[j, j]))
        return float(cov[i, j]) / d if np.isfinite(d) and d > 0 else float("nan")

    h1, h2, c, f, k = unpack(td)
    out = dict(pos_def=pd, c=c.detach().numpy(), f=f.detach().numpy(),
               k=k.detach().numpy(), nll=float(nll(td)))
    if mode == "absolute":
        out.update(a=float(h1), b=float(h2),
                   sa=float(h1) * _err(0), sb=float(h2) * _err(1),
                   rho_ab=_rho(0, 1), rho_ac=_rho(0, 2))
    else:
        out.update(s=float(h1), u_p=float(h2),
                   ss=float(h1) * _err(0), su=_err(1),
                   rho_su=_rho(0, 1), rho_sc=_rho(0, 2))
    return out


# ---------------------------------------------------------------- outputs
def export_lookup(model, meta, path):
    """kappa is the pT MULTIPLIER (pT_corr = kappa * pT); the grid is uniform
    in 1/pT (task 5) and both columns are exported.  Normalisation constants
    are stamped so the map is reproducible at deployment time."""
    pts = semi_even_pt_grid(meta["pt_plot"][0], meta["pt_plot"][1], 60)
    curvs = 1.0 / pts
    etas = np.linspace(*ETA_RANGE, 13)
    phis = np.linspace(-np.pi, np.pi, 25)
    rows = []
    with torch.no_grad():
        for q in (-1.0, 1.0):
            U, E, Fm = np.meshgrid(curvs, etas, phis, indexing="ij")
            P = 1.0 / U
            t = lambda a: torch.tensor(a.ravel(), dtype=torch.float64)
            kap, eps, dlt = model.kappa(t(P), t(E), t(Fm),
                                        torch.full((P.size,), q, dtype=torch.float64))
            rows.append(np.column_stack([np.full(P.size, q), P.ravel(), U.ravel(),
                                         E.ravel(), Fm.ravel(), kap.numpy(),
                                         (1.0 / kap).numpy(),
                                         eps.numpy(), dlt.numpy()]))
    with open(path, "w") as fh:
        fh.write("# convention: (1/pT)_corr = (1/pT) * kappa_curv, "
                 "kappa_curv = 1 + eps + q*delta (uncapped)\n")
        fh.write("# kappa is the pT multiplier consumed downstream: "
                 "pT_corr = kappa * pT = pT / kappa_curv\n")
        fh.write(f"# feature norm: u_ref = {model.u_ref:.6f} GeV^-1, "
                 f"u_scale = {model.u_scale:.6f} GeV^-1, "
                 f"u_clamp = {model.u_clamp:.1f} (kappa constant in 1/pT "
                 f"outside the clamp)\n")
        fh.write(f"# convention tag: {CONVENTION_TAG}\n")
        fh.write("# CAVEAT: trained on vertex-constrained daughter kinematics; "
                 "deployed on raw SvtxTrackMap tracks (calibration/deployment "
                 "mismatch is documented in the analysis note)\n")
        fh.write("q,pT,curv,eta,phi,kappa,kappa_curv,eps,delta\n")
        np.savetxt(fh, np.vstack(rows), delimiter=",", fmt="%.6f")


PULL_INTERPRETATION = [
    "PULL MODEL: s and u_p are a CALIBRATION of the covariance-reported",
    "resolution, r = s(eta,phi) * pT^{u_p} * r_cov -- not a measurement of the",
    "resolution itself. s==1, u_p==0 means the tracker's errors are correct.",
    "s mixes (i) resolution missed by the Kalman hit-error/material model and",
    "(ii) covariance mis-estimation; nothing here separates them.",
    "The stored covariance is POST-VERTEX-CONSTRAINT: s is defined relative to",
    "constrained covariances and must NOT be applied to raw SvtxTrack",
    "covariances (unmeasured constraint-shrinkage factor).",
    "s at low pT is partially degenerate with the floor c: see rho(s,c).",
    "No r grid is exported: r depends on the per-track covariance; consumers",
    "rebuild r = s(eta,phi) * pT^{u_p} * r_cov from their own covariance.",
]


def export_reso_lookup(net, glob, groups, path, n_c=1):
    """absolute: (eta, phi, a, b) fields.  pull: (eta, phi, s) plus u_p in the
    header.  Never an evaluated r grid (species/covariance dependence)."""
    etas = np.linspace(*ETA_RANGE, 25)
    phis = np.linspace(-np.pi, np.pi, 25)
    E, P = np.meshgrid(etas, phis, indexing="ij")
    t = lambda x: torch.tensor(x.ravel(), dtype=torch.float64)
    hdr = []
    with torch.no_grad():
        if net.mode == "absolute":
            a, b = net.ab(t(E), t(P))
            cols = np.column_stack([E.ravel(), P.ravel(), a.numpy(), b.numpy()])
            colhdr = "eta,phi,a,b"
            hdr.append("model: r^2 = (a(eta,phi)/beta)^2 + (b(eta,phi)*pT)^2 ; "
                       "sigma_m^2 = J1^2 r1^2 + J2^2 r2^2 + c^2")
            hdr.append("a dimensionless (enters as a/beta); b in 1/GeV; c in GeV")
        else:
            s = net.s(t(E), t(P))
            cols = np.column_stack([E.ravel(), P.ravel(), s.numpy()])
            colhdr = "eta,phi,s"
            hdr.append("model: r = s(eta,phi) * pT^{u_p} * r_cov ; "
                       "sigma_m^2 = J1^2 r1^2 + J2^2 r2^2 + c^2")
            hdr.append(f"u_p = {float(glob.u_p()):+.6f}")
            hdr.extend(PULL_INTERPRETATION)
        for i in range(n_c):
            hdr.append(f"c[{i}] = {float(glob.c(i)):.6e} GeV")
        for gi, g in enumerate(groups):
            hdr.append(f"tail {g['name']}: f = {float(glob.f(gi)):.4f}, "
                       f"k = {float(glob.k(gi)):.4f}")
    with open(path, "w") as fh:
        for h in hdr:
            fh.write("# " + h + "\n")
        fh.write(colhdr + "\n")
        np.savetxt(fh, cols, delimiter=",", fmt="%.6e")


def _savefig(fig, outdir, name):
    fig.tight_layout()
    fig.savefig(os.path.join(outdir, name + ".png"), dpi=130)
    fig.savefig(os.path.join(outdir, name + ".pdf"))
    plt.close(fig)


def export_mass_histograms_root(K, L, mK, mL, outdir):
    """Write ROOT TH1D mass spectra before/after stage-1 correction."""
    import uproot

    isl = L["is_lam"].numpy()
    specs = [
        ("kshort",      K["mass"].numpy(),        mK,       (0.45, 0.55)),
        ("lambda",      L["mass"].numpy()[isl],   mL[isl],  (1.09, 1.15)),
        ("anti_lambda", L["mass"].numpy()[~isl],  mL[~isl], (1.09, 1.15)),
    ]
    path = os.path.join(outdir, "stage1_mass_histograms.root")
    with uproot.recreate(path) as f:
        for name, raw, cor, rng in specs:
            edges = np.linspace(rng[0], rng[1], 51)
            h_raw, _ = np.histogram(raw, bins=edges)
            h_cor, _ = np.histogram(cor, bins=edges)
            f[f"{name}_mass_raw"] = (h_raw.astype(np.float64), edges)
            f[f"{name}_mass_corrected"] = (h_cor.astype(np.float64), edges)
    print(f"wrote stage-1 raw/corrected mass histograms to {path}")


def plot_mass_1d(K, L, mK, mL, outdir):
    """Figure 1: raw vs corrected 1D mass, K0s / Lambda / Lambdabar."""
    isl = L["is_lam"].numpy()
    fig, ax = plt.subplots(1, 3, figsize=(15, 4.2))
    for a, raw, cor, pdg, rng, t in (
        (ax[0], K["mass"].numpy(), mK, M_K0S, (0.42, 0.58), "K0s"),
        (ax[1], L["mass"].numpy()[isl], mL[isl], M_LAMBDA, (1.09, 1.145), "Lambda"),
        (ax[2], L["mass"].numpy()[~isl], mL[~isl], M_LAMBDA, (1.09, 1.145), "Lambdabar")):
        a.hist(raw, bins=80, range=rng, histtype="step", label="raw")
        a.hist(cor, bins=80, range=rng, histtype="step", label="corrected")
        a.axvline(pdg, ls="--", c="k", lw=0.8, label="PDG")
        a.set_title(t); a.set_xlabel("m [GeV]"); a.set_ylabel("candidates / bin")
        a.legend(fontsize=8)
    fig.suptitle("Invariant mass: raw vs corrected")
    _savefig(fig, outdir, "fig1_mass_1d")


def plot_mass_vs_kin_2d(K, L, mK, mL, outdir, kind, pt_plot):
    """Figures 2/3: 2D histogram of mass (y) vs candidate pT or eta (x)."""
    isl = L["is_lam"].numpy()
    if kind == "pt":
        xK, xL = K["cpt"].numpy(), L["cpt"].numpy()
        xrange, xlabel = (0.0, 1.3 * pt_plot[1]), r"candidate $p_T$ [GeV]"
        figname = "fig2_mass_vs_pt_2d"
    else:
        xK, xL = K["ceta"].numpy(), L["ceta"].numpy()
        xrange, xlabel = (-2.5, 2.5), "candidate eta"
        figname = "fig3_mass_vs_eta_2d"

    rows = [
        ("K0s",       xK,       K["mass"].numpy(),       mK,       (0.42, 0.58),  M_K0S),
        ("Lambda",    xL[isl],  L["mass"].numpy()[isl],  mL[isl],  (1.09, 1.145), M_LAMBDA),
        ("Lambdabar", xL[~isl], L["mass"].numpy()[~isl], mL[~isl], (1.09, 1.145), M_LAMBDA),
    ]
    fig, ax = plt.subplots(3, 2, figsize=(10, 12))
    for i, (name, x, mraw, mcor, mrange, pdg) in enumerate(rows):
        for j, (m, lab) in enumerate(((mraw, "raw"), (mcor, "corrected"))):
            a = ax[i, j]
            h = a.hist2d(x, m, bins=[60, 70], range=[xrange, mrange],
                        cmap="viridis", cmin=1)
            a.axhline(pdg, ls="--", c="w", lw=1.0)
            a.set_title(f"{name} ({lab})")
            a.set_xlabel(xlabel); a.set_ylabel("m [GeV]")
            cb = fig.colorbar(h[3], ax=a)
            cb.set_label("candidates / bin")
    fig.suptitle(f"Invariant mass vs candidate {kind}: raw vs corrected")
    _savefig(fig, outdir, figname)


def plot_alpha_vs_eta_1d(K, k1, k2, outdir):
    """Figure 4: signal-weighted <alpha> vs candidate eta, raw vs corrected."""
    p1r, p2r = K["pt1"].numpy(), K["pt2"].numpy()
    p1c, p2c = (k1 * K["pt1"]).numpy(), (k2 * K["pt2"]).numpy()
    eta_c = K["ceta"].numpy(); w = K["w"].numpy()
    eb = np.linspace(*ETA_RANGE, 13)
    fig, ax = plt.subplots(figsize=(6, 4.5))
    for lab, a1, a2 in (("raw", p1r, p2r), ("corrected", p1c, p2c)):
        al = (a1 - a2) / (a1 + a2)
        num, _ = np.histogram(eta_c, eb, weights=w * al)
        den, _ = np.histogram(eta_c, eb, weights=w)
        ax.plot(0.5 * (eb[:-1] + eb[1:]), num / np.maximum(den, 1e-9), "o-", label=lab)
    ax.axhline(0, c="k", lw=0.8)
    ax.set_xlabel("K0s candidate eta")
    ax.set_ylabel(r"$\langle\alpha\rangle$ (signal-weighted)")
    ax.set_title(r"K0s daughter $p_T$ asymmetry vs eta")
    ax.legend()
    _savefig(fig, outdir, "fig4_alpha_vs_eta_1d")


def plot_alpha_vs_mass_2d(K, mK, k1, k2, outdir):
    """Figure 5: daughter pT asymmetry alpha (x) vs K0s mass (y)."""
    p1r, p2r = K["pt1"].numpy(), K["pt2"].numpy()
    p1c, p2c = (k1 * K["pt1"]).numpy(), (k2 * K["pt2"]).numpy()
    alpha_raw = (p1r - p2r) / (p1r + p2r)
    alpha_cor = (p1c - p2c) / (p1c + p2c)
    mraw = K["mass"].numpy()
    arange, mrange = (-1, 1), (0.4, 0.6)
    fig, ax = plt.subplots(1, 2, figsize=(11, 4.6))
    for a, al, m, lab in ((ax[0], alpha_raw, mraw, "raw"),
                          (ax[1], alpha_cor, mK, "corrected")):
        h = a.hist2d(al, m, bins=[100, 100], range=[arange, mrange],
                    cmap="viridis", cmin=1)
        a.axhline(M_K0S, ls="--", c="w", lw=1.0)
        a.axvline(0.0, ls=":", c="w", lw=0.8)
        a.set_title(f"K0s ({lab})")
        a.set_xlabel(r"$\alpha=(p_T^{\pi^-}-p_T^{\pi^+})/(p_T^{\pi^-}+p_T^{\pi^+})$")
        a.set_ylabel("m [GeV]")
        cb = fig.colorbar(h[3], ax=a)
        cb.set_label("candidates / bin")
    fig.suptitle("K0s daughter pT asymmetry vs invariant mass")
    _savefig(fig, outdir, "fig5_alpha_vs_mass_2d")


def _slice_title(u):
    return f"1/pT = {u:.2f} GeV$^{{-1}}$ (pT = {1.0/u:.2f} GeV)"


def _plot_map_grid(maps, curv_slices, outdir, name, suptitle, cmap, vmin, vmax,
                   colorbar_label, title_prefix=""):
    ee = np.linspace(*ETA_RANGE, 50); pp = np.linspace(-np.pi, np.pi, 50)
    n = len(curv_slices); ncols = int(np.ceil(np.sqrt(n)));
    nrows = int(np.ceil(n / ncols))
    fig, ax = plt.subplots(nrows, ncols, figsize=(4.6 * ncols, 4.1 * nrows))
    ax = np.atleast_2d(ax)
    for idx, u in enumerate(curv_slices):
        r, c = divmod(idx, ncols)
        a = ax[r, c]
        im = a.pcolormesh(ee, pp, maps[idx].T, shading="auto", cmap=cmap,
                          vmin=vmin, vmax=vmax)
        a.set_title(title_prefix + _slice_title(u), fontsize=9)
        a.set_xlabel("eta"); a.set_ylabel("phi")
        cb = fig.colorbar(im, ax=a)
        cb.set_label(colorbar_label)
    for idx in range(n, nrows * ncols):
        r, c = divmod(idx, ncols); ax[r, c].axis("off")
    fig.suptitle(suptitle)
    _savefig(fig, outdir, name)


def plot_kappa_maps_vs_curv(model, outdir, curv_slices):
    """Figure 6: charge-even (eps) and charge-odd (delta) curvature-scale maps,
    sliced UNIFORMLY IN 1/pT (task 5), each panel labelled with both 1/pT and
    the equivalent pT.  These are the exact even/odd fields of the convention
    (1/pT)_corr = (1/pT)(1 + eps + q*delta), read directly off the network."""
    ee = np.linspace(*ETA_RANGE, 50); pp = np.linspace(-np.pi, np.pi, 50)
    E, P = np.meshgrid(ee, pp, indexing="ij")
    t = lambda a: torch.tensor(a.ravel(), dtype=torch.float64)
    evens, odds = [], []
    with torch.no_grad():
        for u in curv_slices:
            eps, dlt = model.eps_delta(t(np.full(E.size, 1.0 / u)), t(E), t(P))
            evens.append(eps.numpy().reshape(E.shape))
            odds.append(dlt.numpy().reshape(E.shape))
    vmax_e = max(1e-4, max(np.abs(z).max() for z in evens))
    vmax_o = max(1e-4, max(np.abs(z).max() for z in odds))

    _plot_map_grid(evens, curv_slices, outdir, "fig6a_kappa_even_maps_vs_pt",
                   "Charge-even curvature-scale maps across pT",
                   "RdBu_r", -vmax_e, vmax_e,
                   r"$\epsilon$  (charge-even fractional $1/p_T$ scale)")
    _plot_map_grid(odds, curv_slices, outdir, "fig6b_kappa_odd_maps_vs_pt",
                   "Charge-odd curvature-scale maps across pT",
                   "RdBu_r", -vmax_o, vmax_o,
                   r"$\delta$  (charge-odd fractional $1/p_T$ scale)")


def plot_kappa_final_vs_curv(model, outdir, curv_slices):
    """Figure 7: deployed pT multiplier kappa_q = 1/(1+eps+q*delta) per charge,
    sliced uniformly in 1/pT (task 5), shared color scale."""
    ee = np.linspace(*ETA_RANGE, 50); pp = np.linspace(-np.pi, np.pi, 50)
    E, P = np.meshgrid(ee, pp, indexing="ij")
    t = lambda a: torch.tensor(a.ravel(), dtype=torch.float64)
    kp_maps, km_maps = [], []
    with torch.no_grad():
        for u in curv_slices:
            kp, _, _ = model.kappa(t(np.full(E.size, 1.0 / u)), t(E), t(P),
                                   torch.ones(E.size, dtype=torch.float64))
            km, _, _ = model.kappa(t(np.full(E.size, 1.0 / u)), t(E), t(P),
                                   -torch.ones(E.size, dtype=torch.float64))
            kp_maps.append(kp.numpy().reshape(E.shape))
            km_maps.append(km.numpy().reshape(E.shape))
    dev = max(1e-4, max(np.abs(z - 1.0).max() for z in kp_maps + km_maps))

    _plot_map_grid(kp_maps, curv_slices, outdir, "fig7a_kappa_qplus_maps_vs_pt",
                   r"Final $p_T$ scale correction for positive tracks",
                   "RdBu_r", 1 - dev, 1 + dev,
                   r"$\kappa(q{=}{+}1)$  final $p_T$ multiplier",
                   title_prefix="q = +1, ")
    _plot_map_grid(km_maps, curv_slices, outdir, "fig7b_kappa_qminus_maps_vs_pt",
                   r"Final $p_T$ scale correction for negative tracks",
                   "RdBu_r", 1 - dev, 1 + dev,
                   r"$\kappa(q{=}{-}1)$  final $p_T$ multiplier",
                   title_prefix="q = -1, ")


# ---------------------------------------------------------------- stage-2 figs
def core_width(x, w, rng=(-6, 6), nbins=120, fit_range=3.0, min_w=200.0):
    """Weighted Gaussian-core width of a pull distribution (tail-insensitive).

    Mildly biased high (tail leakage under |pull| < fit_range): a perfectly
    closing fit reads ~2-4% above 1 for f ~ 0.02, k ~ 2.  Interpret residual
    structure vs pT/eta/phi, not the absolute offset.
    """
    if np.sum(w) < min_w:
        return np.nan, np.nan
    h, edges = np.histogram(x, bins=nbins, range=rng, weights=w)
    c = 0.5 * (edges[:-1] + edges[1:])
    sel = np.abs(c) < fit_range
    g = lambda t, N, mu, s, b0: N * np.exp(-0.5 * ((t - mu) / s)**2) + b0
    try:
        popt, _ = curve_fit(g, c[sel], h[sel],
                            p0=[h[sel].max(), 0.0, 1.0, 0.0], maxfev=20000)
        s = abs(popt[2])
        if not (0.1 < s < 5.0) or not (-2 < popt[1] < 2):
            raise RuntimeError
    except Exception:
        return np.nan, np.nan
    return popt[1], abs(popt[2])


def weighted_rms(x, w):
    mu = np.sum(w * x) / np.sum(w)
    return math.sqrt(np.sum(w * (x - mu)**2) / np.sum(w))


def _quantile_edges(x, w, n):
    q = np.linspace(0, 1, n + 1)
    return np.unique(np.quantile(x, q))


def plot_pull_closure(groups, net, glob, outdir):
    """Figure 8 (primary go/no-go): pull = (m - M_PDG) / sigma_m.

    In pull mode a RAW-COVARIANCE overlay is added: the pull computed with
    s = 1, u_p = 0 and no floor -- its core width is the single number saying
    how wrong the shipped covariances are (subject to the SV-constraint
    caveat).  Deviations of the calibrated core from 1 are a mis-modelled
    resolution field, NOT a scale problem (mu is fixed to PDG; fig1).
    """
    pulls, ws, cs, raws = {}, {}, {}, {}
    with torch.no_grad():
        for gi, g in enumerate(groups):
            sig = sigma_mass_net(g["R"], net, glob, glob.c(g["c_idx"]))
            pulls[g["name"]] = ((g["R"]["m"] - g["M"]) / sig).numpy()
            ws[g["name"]] = g["w"].numpy()
            cs[g["name"]] = (g["R"]["cpt"].numpy(), g["R"]["ceta"].numpy(),
                             g["R"]["cphi"].numpy())
            if net.mode == "pull":
                one = torch.ones_like(g["R"]["m"])
                sig0 = sigma_mass_pull(g["R"], one, one,
                                       torch.tensor(0.0, dtype=torch.float64),
                                       torch.tensor(0.0, dtype=torch.float64))
                raws[g["name"]] = ((g["R"]["m"] - g["M"]) / sig0).numpy()

    fig, ax = plt.subplots(2, 3, figsize=(15.5, 9))
    raw_cores = {}
    for i, g in enumerate(groups):
        n = g["name"]
        a = ax[0, i]
        h, edges, _ = a.hist(pulls[n], bins=120, range=(-6, 6), weights=ws[n],
                             histtype="step", color="C0", label="calibrated")
        if n in raws:
            a.hist(raws[n], bins=120, range=(-6, 6), weights=ws[n],
                   histtype="step", color="C2", ls="--",
                   label="raw covariance (s=1, u_p=0, no floor)")
            raw_cores[n] = core_width(raws[n], ws[n])[1]
        mu, s = core_width(pulls[n], ws[n])
        if np.isfinite(s):
            c = 0.5 * (edges[:-1] + edges[1:])
            amp = h[np.argmin(np.abs(c - mu))]
            a.plot(c, amp * np.exp(-0.5 * ((c - mu) / s)**2), "r-", lw=1.2,
                   label=f"core fit  $\\sigma$={s:.3f}")
        rms = weighted_rms(pulls[n], ws[n])
        a.axvline(0, c="k", lw=0.7)
        ttl = f"{n}: core {s:.3f}, RMS {rms:.3f}"
        if n in raw_cores and np.isfinite(raw_cores[n]):
            ttl += f", raw-cov core {raw_cores[n]:.3f}"
        a.set_title(ttl, fontsize=9)
        a.set_xlabel(r"pull $=(m-M_{PDG})/\sigma_m$")
        a.set_ylabel("weighted candidates / bin")
        a.legend(fontsize=7)

    for j, (lab, ix, rng) in enumerate((("candidate $p_T$ [GeV]", 0, None),
                                        ("candidate $\\eta$", 1, ETA_RANGE),
                                        ("candidate $\\phi$", 2, (-np.pi, np.pi)))):
        a = ax[1, j]
        for g in groups:
            n = g["name"]
            x = cs[n][ix]
            edges = _quantile_edges(x, ws[n], 10) if rng is None else np.linspace(*rng, 11)
            xs, ss = [], []
            for lo, hi in zip(edges[:-1], edges[1:]):
                sel = (x >= lo) & (x < hi)
                if sel.sum() < 50:
                    continue
                _, s = core_width(pulls[n][sel], ws[n][sel])
                xs.append(0.5 * (lo + hi)); ss.append(s)
            a.plot(xs, ss, "o-", ms=3, label=n)
        a.axhline(1.0, c="k", ls="--", lw=0.8)
        a.set_ylim(0.6, 1.6)
        a.set_xlabel(lab)
        a.set_ylabel("core pull width")
        a.legend(fontsize=8)
    fig.suptitle("Pull closure: core width must be 1.0, globally and in every bin")
    _savefig(fig, outdir, "fig8_pull_closure")
    out = {g["name"]: (core_width(pulls[g["name"]], ws[g["name"]])[1],
                       weighted_rms(pulls[g["name"]], ws[g["name"]]))
           for g in groups}
    return out, raw_cores


def plot_width_overlay(groups, net, glob, outdir):
    """Figure 9: fitted Gaussian core width of the corrected mass peak vs the
    predicted median sigma_m, per (candidate pT, eta) bin.  fig8 is the
    unbiased test (per-bin mixture over sigma_m biases this one slightly)."""
    fig, ax = plt.subplots(3, 2, figsize=(11, 12))
    with torch.no_grad():
        for i, g in enumerate(groups):
            n = g["name"]
            sig = sigma_mass_net(g["R"], net, glob, glob.c(g["c_idx"])).numpy()
            m = g["R"]["m"].numpy(); w = g["w"].numpy()
            for j, (xv, lab, rng) in enumerate(
                    ((g["R"]["cpt"].numpy(), r"candidate $p_T$ [GeV]", None),
                     (g["R"]["ceta"].numpy(), r"candidate $\eta$", ETA_RANGE))):
                edges = _quantile_edges(xv, w, 10) if rng is None else np.linspace(*rng, 11)
                xs, fit, pred = [], [], []
                for lo, hi in zip(edges[:-1], edges[1:]):
                    sel = (xv >= lo) & (xv < hi)
                    if w[sel].sum() < 200:
                        continue
                    _, sfit, _ = fit_peak(m[sel], g["M"], SIG0[n], g["lo"], g["hi"],
                                          weights=w[sel])
                    xs.append(0.5 * (lo + hi))
                    fit.append(sfit * 1e3)
                    pred.append(np.median(sig[sel]) * 1e3)
                a = ax[i, j]
                a.plot(xs, fit, "o-", ms=3, label="fitted core width")
                a.plot(xs, pred, "s--", ms=3, label=r"median predicted $\sigma_m$")
                a.set_xlabel(lab); a.set_ylabel(r"$\sigma_m$ [MeV]")
                a.set_title(n); a.legend(fontsize=8)
    fig.suptitle("Mass-peak core width vs predicted resolution")
    _savefig(fig, outdir, "fig9_width_overlay")


def plot_r_vs_pt(groups, net, glob, outdir, pt_plot):
    """Figure 10: per-track fractional resolution vs pT.

    absolute: the field prediction decomposed into a/beta and b*pT at eta
    references (as v2), pion vs proton hypothesis.
    pull: per-leg binned medians of raw r_cov and calibrated
    r = s*pT^{u_p}*r_cov, split into pion legs and proton legs.
    """
    if net.mode == "absolute":
        pts = np.geomspace(max(pt_plot[0], 5e-2), pt_plot[1], 120)
        tp = torch.tensor(pts, dtype=torch.float64)
        eta_ref = (-1.0, 0.0, 1.0)
        fig, ax = plt.subplots(1, len(eta_ref), figsize=(5.2 * len(eta_ref), 4.4),
                               squeeze=False)
        with torch.no_grad():
            c_mev = float(glob.c(0)) * 1e3
            for i, eta in enumerate(eta_ref):
                a_f, b_f = net.ab(torch.tensor([eta], dtype=torch.float64),
                                  torch.tensor([0.0], dtype=torch.float64))
                a_f, b_f = float(a_f), float(b_f)
                axx = ax[0, i]
                for mass, name, col in ((PION_MASS, r"$\pi$", "C0"),
                                        (PROTON_MASS, "p", "C3")):
                    beta = beta_of(tp, torch.full_like(tp, eta), mass).numpy()
                    t_ms = a_f / beta
                    r = np.sqrt(t_ms**2 + (b_f * pts)**2)
                    axx.plot(pts, 1e2 * r, "-", c=col, label=f"{name}: total")
                    axx.plot(pts, 1e2 * t_ms, ":", c=col, lw=1.0,
                             label=f"{name}: $a/\\beta$")
                axx.plot(pts, 1e2 * b_f * pts, "--", c="k", lw=1.0, label=r"$b\,p_T$")
                axx.set_xscale("log"); axx.set_xlabel(r"$p_T$ [GeV]")
                axx.set_ylabel(r"$\sigma(p_T)/p_T$ [%]")
                axx.set_title(f"$\\eta$ = {eta:+.1f}: a = {a_f:.4f}, "
                              f"b = {b_f:.4f} GeV$^{{-1}}$, c = {c_mev:.2f} MeV")
                axx.legend(fontsize=7); axx.grid(alpha=0.25)
        fig.suptitle("Per-track fractional momentum resolution (absolute model)")
    else:
        legs = {"pion": [], "proton": []}
        with torch.no_grad():
            up = glob.u_p()
            for g in groups:
                R, w = g["R"], g["w"]
                for leg in (1, 2):
                    kind = "proton" if (R["m2"] == PROTON_MASS and leg == 2) else "pion"
                    s = net.s(R[f"eta{leg}"], R[f"phi{leg}"])
                    rp = pull_r(R, leg, s, up)
                    legs[kind].append((R[f"pt{leg}"].numpy(),
                                       R[f"rcov{leg}"].numpy(), rp.numpy(),
                                       w.numpy()))
        fig, ax = plt.subplots(1, 2, figsize=(11, 4.6))
        for axx, kind in zip(ax, ("pion", "proton")):
            pt = np.concatenate([x[0] for x in legs[kind]])
            rc = np.concatenate([x[1] for x in legs[kind]])
            rp = np.concatenate([x[2] for x in legs[kind]])
            edges = np.unique(np.quantile(pt, np.linspace(0, 1, 13)))
            xs, med_c, med_p = [], [], []
            for lo, hi in zip(edges[:-1], edges[1:]):
                sel = (pt >= lo) & (pt < hi)
                if sel.sum() < 100:
                    continue
                xs.append(0.5 * (lo + hi))
                med_c.append(1e2 * np.median(rc[sel]))
                med_p.append(1e2 * np.median(rp[sel]))
            axx.plot(xs, med_c, "s--", ms=3, c="C2", label=r"median raw $r_{cov}$")
            axx.plot(xs, med_p, "o-", ms=3, c="C0",
                     label=r"median calibrated $s\,p_T^{u_p}\,r_{cov}$")
            axx.set_xlabel(r"track $p_T$ [GeV]")
            axx.set_ylabel(r"$\sigma(p_T)/p_T$ [%]")
            axx.set_title(f"{kind} legs")
            axx.legend(fontsize=8); axx.grid(alpha=0.25)
        fig.suptitle("Covariance-reported vs calibrated per-track resolution "
                     "(pull model)")
    _savefig(fig, outdir, "fig10_r_vs_pt")


def plot_ab_maps(net, outdir):
    """Figure 11 (absolute mode): (eta, phi) maps of the two fields."""
    ee = np.linspace(*ETA_RANGE, 60); pp = np.linspace(-np.pi, np.pi, 60)
    E, P = np.meshgrid(ee, pp, indexing="ij")
    t = lambda x: torch.tensor(x.ravel(), dtype=torch.float64)
    with torch.no_grad():
        a, b = net.ab(t(E), t(P))
    a = a.numpy().reshape(E.shape); b = b.numpy().reshape(E.shape)
    fig, ax = plt.subplots(1, 2, figsize=(12.5, 4.8))
    for axx, z, lab, ttl in (
        (ax[0], a, "MS-like fractional resolution coefficient "
                   r"(dimensionless, enters as $a/\beta$)", r"$a(\eta,\phi)$"),
        (ax[1], b, r"curvature term coefficient [1/GeV], enters as $b\,p_T$",
                   r"$b(\eta,\phi)$")):
        im = axx.pcolormesh(ee, pp, z.T, shading="auto", cmap="viridis")
        axx.set_xlabel(r"$\eta$"); axx.set_ylabel(r"$\phi$"); axx.set_title(ttl)
        cb = fig.colorbar(im, ax=axx); cb.set_label(lab, fontsize=8)
    fig.suptitle("Resolution fields (charge-even by construction)")
    _savefig(fig, outdir, "fig11_ab_maps")


def plot_r_maps_vs_curv(net, glob, outdir, curv_slices):
    """Figure 12 (absolute mode): r(eta, phi) maps across 1/pT, pi vs p."""
    ee = np.linspace(*ETA_RANGE, 50); pp = np.linspace(-np.pi, np.pi, 50)
    E, P = np.meshgrid(ee, pp, indexing="ij")
    t = lambda a: torch.tensor(a.ravel(), dtype=torch.float64)
    n = len(curv_slices); ncols = 4
    nrows_each = int(np.ceil(n / ncols))

    with torch.no_grad():
        a_f, b_f = net.ab(t(E), t(P))
    a_f, b_f = a_f.numpy(), b_f.numpy()

    r_pi, r_p = [], []
    for u in curv_slices:
        pt = 1.0 / u
        beta_pi = beta_of(torch.tensor(np.full(E.size, pt)), t(E), PION_MASS).numpy()
        beta_p = beta_of(torch.tensor(np.full(E.size, pt)), t(E), PROTON_MASS).numpy()
        r_pi.append((1e2 * np.sqrt((a_f / beta_pi)**2 + (b_f * pt)**2)).reshape(E.shape))
        r_p.append((1e2 * np.sqrt((a_f / beta_p)**2 + (b_f * pt)**2)).reshape(E.shape))
    vmax_pi = max(z.max() for z in r_pi)
    vmax_p = max(z.max() for z in r_p)

    fig, ax = plt.subplots(2 * nrows_each, ncols,
                           figsize=(4.6 * ncols, 4.1 * 2 * nrows_each))
    for block, (maps, vmax, lab) in enumerate((
            (r_pi, vmax_pi, r"$r=\sigma(p_T)/p_T$ [%]  (pion hypothesis)"),
            (r_p, vmax_p, r"$r=\sigma(p_T)/p_T$ [%]  (proton hypothesis)"))):
        for idx, u in enumerate(curv_slices):
            r, c = divmod(idx, ncols)
            a = ax[block * nrows_each + r, c]
            im = a.pcolormesh(ee, pp, maps[idx].T, shading="auto", cmap="viridis",
                              vmin=0, vmax=vmax)
            a.set_title(("$\\pi$, " if block == 0 else "p, ") + _slice_title(u),
                        fontsize=9)
            a.set_xlabel("eta"); a.set_ylabel("phi")
            cb = fig.colorbar(im, ax=a)
            cb.set_label(lab)
        for idx in range(n, nrows_each * ncols):
            r, c = divmod(idx, ncols); ax[block * nrows_each + r, c].axis("off")
    fig.suptitle(r"Per-track fractional momentum resolution $r(\eta,\phi)$ "
                r"across 1/pT, by mass hypothesis")
    _savefig(fig, outdir, "fig12_r_maps_vs_curv")


def plot_calibration_factor(net, glob, outdir):
    """Figure 13 (pull mode): s(eta, phi) map plus 1D profiles.

    Read-off: 'the tracker under/over-estimates sigma(pT) by factor s'."""
    ee = np.linspace(*ETA_RANGE, 60); pp = np.linspace(-np.pi, np.pi, 60)
    E, P = np.meshgrid(ee, pp, indexing="ij")
    t = lambda x: torch.tensor(x.ravel(), dtype=torch.float64)
    with torch.no_grad():
        s = net.s(t(E), t(P)).numpy().reshape(E.shape)
        up = float(glob.u_p())
    fig, ax = plt.subplots(1, 3, figsize=(16, 4.6))
    im = ax[0].pcolormesh(ee, pp, s.T, shading="auto", cmap="viridis")
    ax[0].set_xlabel(r"$\eta$"); ax[0].set_ylabel(r"$\phi$")
    ax[0].set_title(rf"$s(\eta,\phi)$   ($u_p$ = {up:+.4f})")
    cb = fig.colorbar(im, ax=ax[0])
    cb.set_label("covariance calibration factor s  (s = 1: errors correct)")
    ax[1].plot(ee, s.mean(axis=1), "o-", ms=3)
    ax[1].axhline(1.0, c="k", ls="--", lw=0.8)
    ax[1].set_xlabel(r"$\eta$"); ax[1].set_ylabel(r"$\langle s\rangle_\phi$")
    ax[1].set_title("phi-averaged profile")
    ax[2].plot(pp, s.mean(axis=0), "o-", ms=3)
    ax[2].axhline(1.0, c="k", ls="--", lw=0.8)
    ax[2].set_xlabel(r"$\phi$"); ax[2].set_ylabel(r"$\langle s\rangle_\eta$")
    ax[2].set_title("eta-averaged profile")
    fig.suptitle("Covariance calibration field: r = s(eta,phi) * pT^{u_p} * r_cov")
    _savefig(fig, outdir, "fig13_calibration_factor")


# ---------------------------------------------------------------- diagnostics
def diagnostics(model, K, L, meta, outdir):
    with torch.no_grad():
        mK, _, _ = corrected_mass(model, K)
        mL, _, _ = corrected_mass(model, L)
        k1, e1, d1 = model.kappa(K["pt1"], K["eta1"], K["phi1"], K["q1"])
        k2, e2, d2 = model.kappa(K["pt2"], K["eta2"], K["phi2"], K["q2"])
    mK, mL = mK.numpy(), mL.numpy()
    isl = L["is_lam"].numpy()

    lines = []
    for name, raw, cor, pdg, lo, hi, s0 in (
        ("K0s",    K["mass"].numpy(), mK, M_K0S,    0.42, 0.58, 0.010),
        ("Lambda", L["mass"].numpy()[isl],  mL[isl],  M_LAMBDA, 1.09, 1.145, 0.004),
        ("Lambdabar", L["mass"].numpy()[~isl], mL[~isl], M_LAMBDA, 1.09, 1.145, 0.004)):
        mu_r, s_r, _ = fit_peak(raw, pdg, s0, lo, hi)
        mu_c, s_c, _ = fit_peak(cor, pdg, s0, lo, hi)
        lines.append(f"{name:10s} raw peak {mu_r*1e3:9.3f} MeV  ->  corrected "
                     f"{mu_c*1e3:9.3f} MeV   (PDG {pdg*1e3:.3f}, "
                     f"pull {(mu_c-pdg)*1e3:+.3f} MeV)")
    e_all = np.concatenate([e1.numpy(), e2.numpy()])
    d_all = np.concatenate([d1.numpy(), d2.numpy()])
    kc = np.concatenate([(1.0 + e1 + K["q1"] * d1).numpy(),
                         (1.0 + e2 + K["q2"] * d2).numpy()])
    lines.append(f"{'fields':10s} eps mean {e_all.mean():+.5f} rms {e_all.std():.5f} "
                 f"max|.| {np.abs(e_all).max():.5f};  "
                 f"delta mean {d_all.mean():+.5f} rms {d_all.std():.5f} "
                 f"max|.| {np.abs(d_all).max():.5f}   (uncapped)")
    guard_frac = np.mean(kc <= KAPPA_GUARD + 1e-12)
    lines.append(f"{'guard':10s} kappa_curv <= {KAPPA_GUARD} fraction = "
                 f"{guard_frac:.2e}  (numerical guard, must be 0)")
    if guard_frac > 0:
        lines.append(f"{'WARNING':10s} numerical guard active on data -- the "
                     f"converged model is not trustworthy; investigate.")
    all_pt = torch.cat([K["pt1"], K["pt2"], L["pt1"], L["pt2"]])
    uc = model.u_clamp_frac(all_pt)
    lines.append(f"{'feature':10s} u = 1/pT, u_ref = {model.u_ref:.4f}, "
                 f"u_scale = {model.u_scale:.4f} GeV^-1; |u_n| >= "
                 f"{model.u_clamp:.1f} clamp fraction = {uc:.2e} "
                 f"(kappa constant in 1/pT beyond the clamp)")
    txt = "\n".join(lines)
    print(txt)
    with open(os.path.join(outdir, "summary.txt"), "w") as f:
        f.write(txt + "\n")

    export_mass_histograms_root(K, L, mK, mL, outdir)
    plot_mass_1d(K, L, mK, mL, outdir)
    plot_mass_vs_kin_2d(K, L, mK, mL, outdir, "pt", meta["pt_plot"])
    plot_mass_vs_kin_2d(K, L, mK, mL, outdir, "eta", meta["pt_plot"])
    plot_alpha_vs_eta_1d(K, k1, k2, outdir)
    plot_alpha_vs_mass_2d(K, mK, k1, k2, outdir)
    plot_kappa_maps_vs_curv(model, outdir, meta["curv_slices"])
    plot_kappa_final_vs_curv(model, outdir, meta["curv_slices"])


def resolution_diagnostics(net, glob, groups, outdir, meta, n_c=1, scalars=None):
    pw, raw_cores = plot_pull_closure(groups, net, glob, outdir)
    plot_width_overlay(groups, net, glob, outdir)
    plot_r_vs_pt(groups, net, glob, outdir, meta["pt_plot"])
    if net.mode == "absolute":
        plot_ab_maps(net, outdir)
        plot_r_maps_vs_curv(net, glob, outdir, meta["curv_slices"])
    else:
        plot_calibration_factor(net, glob, outdir)

    lines = ["", f"--- stage 2: momentum resolution ({net.mode} model) ---"]
    if net.mode == "pull":
        lines += ["# " + s for s in PULL_INTERPRETATION]
    for gi, g in enumerate(groups):
        core, rms = pw[g["name"]]
        with torch.no_grad():
            fg, kg = float(glob.f(gi)), float(glob.k(gi))
        exp_rms = math.sqrt(1 - fg + fg * kg**2)
        line = (f"{g['name']:10s} pull core width {core:6.4f}  RMS {rms:6.4f} "
                f"(expected RMS from tail {exp_rms:5.3f})   "
                f"f = {fg:6.4f}, k = {kg:5.3f}")
        if g["name"] in raw_cores and np.isfinite(raw_cores[g["name"]]):
            line += f"   raw-cov pull core {raw_cores[g['name']]:6.4f}"
        lines.append(line)
    with torch.no_grad():
        for i in range(n_c):
            lines.append(f"{'floor':10s} c[{i}] = {float(glob.c(i))*1e3:7.4f} MeV")
    with torch.no_grad():
        if net.mode == "absolute":
            aa, bb = [], []
            for g in groups:
                for leg in ("1", "2"):
                    a, b = net.ab(g["R"]["eta" + leg], g["R"]["phi" + leg])
                    aa.append(a.numpy()); bb.append(b.numpy())
            aa = np.concatenate(aa); bb = np.concatenate(bb)
            lines.append(f"{'fields':10s} track-averaged a = {aa.mean():.5f} "
                         f"(spread {aa.std():.5f}),  b = {bb.mean():.5f} "
                         f"(spread {bb.std():.5f}) GeV^-1")
        else:
            up = glob.u_p()
            lines.append(f"{'exponent':10s} u_p = {float(up):+.5f} "
                         f"(0 = no residual pT shape on top of r_cov)")
            for g in groups:
                R, ratio = g["R"], []
                for leg in (1, 2):
                    s = net.s(R[f"eta{leg}"], R[f"phi{leg}"])
                    ratio.append((pull_r(R, leg, s, up) /
                                  R[f"rcov{leg}"]).numpy())
                ratio = np.concatenate(ratio)
                q16, q50, q84 = np.quantile(ratio, [0.16, 0.50, 0.84])
                lines.append(f"{g['name']:10s} r_pred/r_cov median {q50:.4f}  "
                             f"[16%, 84%] = [{q16:.4f}, {q84:.4f}]   "
                             f"<- factor by which the reported momentum "
                             f"uncertainty is wrong")
    if scalars is not None:
        if net.mode == "absolute":
            lines.append(f"{'reduced':10s} constant-field fit: "
                         f"a = {scalars['a']:.5f} +- {scalars['sa']:.5f}, "
                         f"b = {scalars['b']:.5f} +- {scalars['sb']:.5f};  "
                         f"rho(a,b) = {scalars['rho_ab']:+.3f}, "
                         f"rho(a,c) = {scalars['rho_ac']:+.3f}"
                         f"{'' if scalars['pos_def'] else '   [Hessian NOT pos.def.]'}")
        else:
            lines.append(f"{'reduced':10s} constant-field fit: "
                         f"s = {scalars['s']:.5f} +- {scalars['ss']:.5f}, "
                         f"u_p = {scalars['u_p']:+.5f} +- {scalars['su']:.5f};  "
                         f"rho(s,u_p) = {scalars['rho_su']:+.3f}, "
                         f"rho(s,c) = {scalars['rho_sc']:+.3f}"
                         f"{'' if scalars['pos_def'] else '   [Hessian NOT pos.def.]'}")
        lines.append(f"{'':10s} NB: on data this reduced fit is misspecified "
                     f"(the field really depends on eta, phi); central values "
                     f"are an occupancy-weighted compromise and only the "
                     f"injection-toy errors are strictly interpretable.")
    txt = "\n".join(lines)
    print(txt)
    with open(os.path.join(outdir, "summary.txt"), "a") as f:
        f.write(txt + "\n")


# ---------------------------------------------------------------- injection
def _two_body_toy(M, m1, m2, cpt, ceta, cphi, rng):
    """Isotropic two-body decay boosted to the lab; truth mass exactly M."""
    pstar = math.sqrt((M**2 - (m1 + m2)**2) * (M**2 - (m1 - m2)**2)) / (2 * M)
    ct = rng.uniform(-1, 1, cpt.size)
    st = np.sqrt(1 - ct**2)
    ph = rng.uniform(-np.pi, np.pi, cpt.size)
    p1 = np.stack([pstar * st * np.cos(ph), pstar * st * np.sin(ph), pstar * ct], -1)
    p2 = -p1
    E1s = math.sqrt(pstar**2 + m1**2)
    E2s = math.sqrt(pstar**2 + m2**2)

    pc = np.stack([cpt * np.cos(cphi), cpt * np.sin(cphi), cpt * np.sinh(ceta)], -1)
    Ec = np.sqrt((pc**2).sum(-1) + M**2)
    bv = pc / Ec[:, None]
    g = Ec / M
    out = []
    for ps, Es in ((p1, E1s), (p2, E2s)):
        bp = (bv * ps).sum(-1)
        p = ps + (g**2 / (g + 1) * bp + g * Es)[:, None] * bv
        pt = np.hypot(p[:, 0], p[:, 1])
        eta = np.arcsinh(p[:, 2] / np.maximum(pt, 1e-12))
        phi = np.arctan2(p[:, 1], p[:, 0])
        out.append((pt, eta, phi))
    return out


def run_injection(args, K, L, meta, outdir):
    """Mandatory go/no-go.  pull mode: recover (s0, u0) drawn against r_cov
    resampled from data.  absolute mode: recover (a0, b0)."""
    os.makedirs(outdir, exist_ok=True)
    rng = np.random.default_rng(args.inject_seed)
    mode = args.reso_model
    n = args.inject_n

    def rcov_pool(S, leg):
        return S[f"rcov{leg}"].numpy()

    def make(Sdata, M, m1, m2):
        idx = rng.integers(0, Sdata["cpt"].numel(), n)
        cpt = Sdata["cpt"].numpy()[idx]
        ceta = Sdata["ceta"].numpy()[idx]
        cphi = Sdata["cphi"].numpy()[idx]
        (pt1, eta1, phi1), (pt2, eta2, phi2) = _two_body_toy(M, m1, m2,
                                                             cpt, ceta, cphi, rng)
        d = dict(cpt=cpt, ceta=ceta, cphi=cphi, M=M, m1=m1, m2=m2,
                 eta1=eta1, phi1=phi1, eta2=eta2, phi2=phi2)
        if mode == "pull":
            rc1 = rng.choice(rcov_pool(Sdata, 1), n)
            rc2 = rng.choice(rcov_pool(Sdata, 2), n)
            r1 = args.inject_s0 * pt1**args.inject_u0 * rc1
            r2 = args.inject_s0 * pt2**args.inject_u0 * rc2
            d["rcov1"], d["rcov2"] = rc1, rc2
        else:
            def r_ab(pt, eta, m):
                beta = beta_of(torch.tensor(pt), torch.tensor(eta), m).numpy()
                return np.sqrt((args.inject_a0 / beta)**2 + (args.inject_b0 * pt)**2)
            r1 = r_ab(pt1, eta1, m1)
            r2 = r_ab(pt2, eta2, m2)
        d["pt1"] = np.maximum(pt1 * (1 + r1 * rng.standard_normal(n)), 1e-3)
        d["pt2"] = np.maximum(pt2 * (1 + r2 * rng.standard_normal(n)), 1e-3)
        d["dm"] = args.inject_c0 * rng.standard_normal(n)
        return d

    T = lambda x: torch.tensor(x, dtype=torch.float64)

    def to_S(d):
        S = dict(pt1=T(d["pt1"]), eta1=T(d["eta1"]), phi1=T(d["phi1"]),
                 pt2=T(d["pt2"]), eta2=T(d["eta2"]), phi2=T(d["phi2"]),
                 q1=torch.zeros(n, dtype=torch.float64),
                 q2=torch.zeros(n, dtype=torch.float64),
                 m1=d["m1"], m2=d["m2"], M=d["M"], dm=T(d["dm"]),
                 cpt=T(d["cpt"]), ceta=T(d["ceta"]), cphi=T(d["cphi"]))
        for key in ("rcov1", "rcov2"):
            if key in d:
                S[key] = T(d[key])
        return S

    SK = to_S(make(K, M_K0S, PION_MASS, PION_MASS))
    SL = to_S(make(L, M_LAMBDA, PION_MASS, PROTON_MASS))
    SL["is_lam"] = torch.tensor(rng.random(n) < 0.5)

    idk = IdentityKappa()

    def fiducial(S, lo, hi):
        """Eta acceptance + mass window; NO pT cut, matching the data path."""
        with torch.no_grad():
            m, _, _ = mass_jacobians(S["pt1"], S["eta1"], S["phi1"], S["m1"],
                                     S["pt2"], S["eta2"], S["phi2"], S["m2"])
            m = m + S["dm"]
        sel = ((S["pt1"] > PT_SANITY) & (S["pt2"] > PT_SANITY) &
               (S["eta1"].abs() < ETA_RANGE[1]) & (S["eta2"].abs() < ETA_RANGE[1]) &
               (m > lo) & (m < hi))
        return {k: (v[sel] if torch.is_tensor(v) else v) for k, v in S.items()}

    SK = fiducial(SK, *WIN["K0s"])
    SL = fiducial(SL, *WIN["Lambda"])
    print(f"[inject] toy after selection: {SK['pt1'].numel()} K0s, "
          f"{SL['pt1'].numel()} Lambda(+bar)")

    groups = build_groups(idk, SK, SL, pure_signal=True)
    n_c = set_c_indices(groups, args.c_per_resonance)

    net, glob = train_resolution(groups, args, n_c=n_c)
    scal = fit_global_reduced(groups, mode, n_c=n_c, steps=args.inject_scalar_steps)

    resolution_diagnostics(net, glob, groups, outdir, meta, n_c=n_c, scalars=scal)

    if mode == "pull":
        pa = (scal["s"] - args.inject_s0) / max(scal["ss"], 1e-12)
        pb = (scal["u_p"] - args.inject_u0) / max(scal["su"], 1e-12)
        rec = [f"injected  s0 = {args.inject_s0:.5f}   u0 = {args.inject_u0:+.5f}   "
               f"c0 = {args.inject_c0*1e3:.4f} MeV",
               f"recovered s   = {scal['s']:.5f} +- {scal['ss']:.5f}  pull {pa:+.2f}",
               f"recovered u_p = {scal['u_p']:+.5f} +- {scal['su']:.5f}  pull {pb:+.2f}",
               f"recovered c   = {', '.join(f'{x*1e3:.4f}' for x in scal['c'])} MeV "
               f"(injected {args.inject_c0*1e3:.4f}; rho(s,c) = {scal['rho_sc']:+.3f})"]
    else:
        pa = (scal["a"] - args.inject_a0) / max(scal["sa"], 1e-12)
        pb = (scal["b"] - args.inject_b0) / max(scal["sb"], 1e-12)
        rec = [f"injected  a0 = {args.inject_a0:.5f}   b0 = {args.inject_b0:.5f} GeV^-1   "
               f"c0 = {args.inject_c0*1e3:.4f} MeV",
               f"recovered a  = {scal['a']:.5f} +- {scal['sa']:.5f}  pull {pa:+.2f}",
               f"recovered b  = {scal['b']:.5f} +- {scal['sb']:.5f}  pull {pb:+.2f}",
               f"recovered c  = {', '.join(f'{x*1e3:.4f}' for x in scal['c'])} MeV "
               f"(injected {args.inject_c0*1e3:.4f}; rho(a,c) = {scal['rho_ac']:+.3f})"]

    with torch.no_grad():
        pw = {}
        for g in groups:
            sig = sigma_mass_net(g["R"], net, glob, glob.c(g["c_idx"]))
            pl = ((g["R"]["m"] - g["M"]) / sig).numpy()
            pw[g["name"]] = core_width(pl, g["w"].numpy())[1]

    lines = ["", f"--- injection campaign (go/no-go, {mode} model) ---"] + rec + \
            ["pull core widths: " + ", ".join(f"{k} {v:.4f}" for k, v in pw.items())]
    ok = (abs(pa) < 3 and abs(pb) < 3 and
          all(np.isfinite(v) and abs(v - 1) < 0.05 for v in pw.values()))
    lines.append(f"VERDICT: {'PASS' if ok else 'FAIL'}")
    txt = "\n".join(lines)
    print(txt)
    with open(os.path.join(outdir, "summary.txt"), "a") as f:
        f.write(txt + "\n")
    return ok


# ---------------------------------------------------------------- main
def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--kshort", default="kshort_kfparticle.root",
                    help="K0s KFParticle nTuple (.root) or legacy CSV")
    ap.add_argument("--lam", default="lambda_kfparticle.root",
                    help="Lambda KFParticle nTuple (.root) or legacy CSV")
    ap.add_argument("--tree", default="DecayTree", help="TTree name in the ROOT files")
    ap.add_argument("--max_candidates", type=int, default=0,
                    help="use at most N candidates per species (0 = all), "
                         "random subsample")
    ap.add_argument("--subsample_seed", type=int, default=42)
    ap.add_argument("--outdir", default="calib_out")
    ap.add_argument("--lookup_pt_min", type=float, default=0.10,
                    help="lower pT [GeV] covered by kappa lookup and kappa-map diagnostics")
    ap.add_argument("--lookup_pt_max", type=float, default=5.0,
                    help="upper pT [GeV] covered by kappa lookup and kappa-map diagnostics")
    ap.add_argument("--kappa_slices", type=int, default=25,
                    help="number of curvature slices in fig6/fig7 kappa diagnostics")
    # stage 1
    ap.add_argument("--epochs", type=int, default=400)
    ap.add_argument("--lr", type=float, default=2e-3)
    ap.add_argument("--hidden", type=int, default=48)
    ap.add_argument("--refresh", type=int, default=50, help="weight-refresh cadence")
    ap.add_argument("--lam_prior", type=float, default=0.05,
                    help="L2(eps,delta) weight at epoch 0; annealed linearly to 0")
    ap.add_argument("--lam_split", type=float, default=1.0)
    ap.add_argument("--lam_alpha", type=float, default=2.0)
    ap.add_argument("--load_stage1", default=None,
                    help="existing model.pt (must carry the v4 convention tag; "
                         "older checkpoints are rejected)")
    # stage 2
    ap.add_argument("--no_reso", action="store_true", help="stage 1 only", default=False)
    ap.add_argument("--reso_model", choices=("absolute", "pull"), default="pull",
                    help="absolute: measure (a,b) fields from peak widths; "
                         "pull: calibrate the KFParticle covariance, "
                         "r = s(eta,phi)*pT^u_p*r_cov (needs Covariance branches)")
    ap.add_argument("--reso_epochs", type=int, default=300)
    ap.add_argument("--reso_lr", type=float, default=2e-3)
    ap.add_argument("--reso_hidden", type=int, default=32)
    ap.add_argument("--reso_layers", type=int, default=3)
    ap.add_argument("--c_per_resonance", action="store_true",
                    help="separate mass-width floor for K0s and Lambda")
    ap.add_argument("--reso_scalar_steps", type=int, default=1500,
                    help="reduced global-scalar fit steps (for Hessian errors)")
    # injection
    ap.add_argument("--inject", action="store_true",
                    help="append the go/no-go toy campaign after the data pass")
    ap.add_argument("--inject_only", action="store_true",
                    help="run the toy campaign and skip the data pass")
    ap.add_argument("--inject_n", type=int, default=200000)
    ap.add_argument("--inject_s0", type=float, default=1.30,
                    help="pull mode: injected covariance miscalibration factor")
    ap.add_argument("--inject_u0", type=float, default=0.0,
                    help="pull mode: injected pT exponent")
    ap.add_argument("--inject_a0", type=float, default=0.010,
                    help="absolute mode: injected MS-like coefficient")
    ap.add_argument("--inject_b0", type=float, default=0.012,
                    help="absolute mode: injected curvature coefficient [1/GeV]")
    ap.add_argument("--inject_c0", type=float, default=0.0015,
                    help="injected mass-width floor [GeV]")
    ap.add_argument("--inject_seed", type=int, default=1234)
    ap.add_argument("--inject_scalar_steps", type=int, default=1500)
    args = ap.parse_args()

    os.makedirs(args.outdir, exist_ok=True)
    K, L, meta = load(args)
    print(f"loaded {len(K['mass'])} K0s, {len(L['mass'])} Lambda candidates")

    if args.inject or args.inject_only:
        ok = run_injection(args, K, L, meta, os.path.join(args.outdir, "injection"))
        if not ok:
            print("[inject] FAILED -- do not deploy the resolution map. "
                  "Debug before running on data.")
        if args.inject_only:
            return

    # ---- stage 1: momentum scale
    if args.load_stage1:
        model = load_stage1(args.load_stage1, hidden=args.hidden)
        print(f"loaded stage-1 model from {args.load_stage1} "
              f"(convention '{CONVENTION_TAG}' verified)")
        with torch.no_grad():
            mK, _, _ = corrected_mass(model, K)
            mL, _, _ = corrected_mass(model, L)
        _, _, pK = fit_peak(mK.numpy(), M_K0S, 0.010, 0.42, 0.58)
        _, _, pL = fit_peak(mL.numpy(), M_LAMBDA, 0.004, 1.09, 1.145)
        K["w"] = torch.tensor(signal_weights(mK.numpy(), pK))
        L["w"] = torch.tensor(signal_weights(mL.numpy(), pL))
    else:
        model = train(args, K, L, meta)
        save_stage1(model, os.path.join(args.outdir, "model.pt"))

    export_lookup(model, meta, os.path.join(args.outdir, "kappa_lookup.csv"))
    diagnostics(model, K, L, meta, args.outdir)

    if args.no_reso:
        print(f"outputs written to {args.outdir}/  (stage 1 only)")
        return

    # ---- stage 2, KappaNet frozen
    for p in model.parameters():
        p.requires_grad_(False)
    model.eval()

    groups = build_groups(model, K, L, pure_signal=False)
    n_c = set_c_indices(groups, args.c_per_resonance)
    print(f"stage 2 ({args.reso_model}): {len(groups)} resonance groups, "
          f"{n_c} floor parameter(s)")
    net, glob = train_resolution(groups, args, n_c=n_c)
    scal = fit_global_reduced(groups, args.reso_model, n_c=n_c,
                              steps=args.reso_scalar_steps)

    torch.save({"reso_net": net.state_dict(), "globals": glob.state_dict(),
                "mode": args.reso_model, "n_c": n_c,
                "groups": [g["name"] for g in groups]},
               os.path.join(args.outdir, "reso_model.pt"))
    export_reso_lookup(net, glob, groups,
                       os.path.join(args.outdir, "reso_lookup.csv"), n_c=n_c)
    resolution_diagnostics(net, glob, groups, args.outdir, meta,
                           n_c=n_c, scalars=scal)

    print(f"outputs written to {args.outdir}/")


if __name__ == "__main__":
    main()
