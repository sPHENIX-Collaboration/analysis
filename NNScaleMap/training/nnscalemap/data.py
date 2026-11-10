"""KFParticle nTuple loading, daughter-track covariance unpacking, fiducial selection."""
import math
import sys
import numpy as np
import torch

from .constants import ETA_RANGE, FEATURE_UNITS, MOTHER_PREFIXES, M_K0S, M_LAMBDA, PION_MASS, PROTON_MASS, PT_SANITY, RCOV_VALID, X_CLAMP
from .kinematics import feature_raw, semi_even_pt_grid

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
# branches the reader requires: <mother>_<MOTHER_VARS>, track_{1,2}_<TRACK_VARS>;
# Lambda inputs also need track_{1,2}_PDG_ID
MOTHER_VARS = ("mass", "pT", "pseudorapidity", "phi")
TRACK_VARS = ("pT", "pseudorapidity", "phi")


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

    want = {f"{mother}_{x}": f"c{x}" for x in MOTHER_VARS}
    for i in (1, 2):
        for x in TRACK_VARS:
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
    # entry-count steps: a memory-size step stalls on one-basket-per-entry Covariance branches
    for raw in t.iterate(branches, library="np", step_size=100000):
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
    return _concat_species(chunks, label)


def _concat_species(parts, label):
    """Concatenate species dicts (ROOT chunks or input files) candidate-wise.

    Mother prefix, branch set and per-candidate array shape (e.g. the 21 vs 28
    entry Covariance layout) must agree, otherwise the inputs are rejected.
    """
    ref = parts[0]
    for p in parts[1:]:
        if p["mother"] != ref["mother"]:
            raise RuntimeError(f"[{label}] mother prefix differs across inputs: "
                               f"'{ref['mother']}' vs '{p['mother']}'")
        if set(p) != set(ref):
            raise RuntimeError(f"[{label}] branch sets differ across inputs: "
                               f"{sorted(set(p) ^ set(ref))}")
        for k in ref:
            if isinstance(ref[k], np.ndarray) and p[k].shape[1:] != ref[k].shape[1:]:
                raise RuntimeError(f"[{label}] '{k}' shape differs across inputs: "
                                   f"{ref[k].shape[1:]} vs {p[k].shape[1:]}")
    out = {"mother": ref["mother"]}
    for k in ref:
        if isinstance(ref[k], np.ndarray):
            out[k] = np.concatenate([p[k] for p in parts])
    return out


def _species_selection(d, mass_lo, mass_hi, has_cov, label):
    """Eta acceptance + pT sanity + mass window + covariance validity.

    NO daughter pT cut: only a 1 MeV sanity floor.
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
    """Returns (K, L, meta).  meta carries has_cov, the momentum-feature
    normalisation, the loss bins (pT edges and per-bin eta x phi cells) and
    the data-driven plot ranges."""
    rng = np.random.default_rng(args.subsample_seed)

    def read(paths, need_pdg):
        label = "Lambda" if need_pdg else "K0s"
        quota = {p: args.max_candidates for p in paths}
        if args.max_candidates and len(paths) > 1:
            # share --max_candidates across files in proportion to their
            # entries, so early files cannot exhaust it; _subsample trims to N
            import uproot
            n = {p: uproot.open(p)[args.tree].num_entries for p in paths}
            total = sum(n.values())
            for p in paths:
                quota[p] = math.ceil(args.max_candidates * n[p] / total)
                print(f"[{label}] {p}: {n[p]} entries, candidate quota {quota[p]}")
        parts = []
        for path in paths:
            parts.append(_read_root_species(path, args.tree, need_pdg,
                                            max_candidates=quota[path],
                                            rng=rng, label=label))
        d = _concat_species(parts, label)
        return d, ("t1_Covariance" in d and "t2_Covariance" in d)

    dK, covK = read(args.kshort, need_pdg=False)
    dL, covL = read(args.lam, need_pdg=True)
    has_cov = covK and covL
    if not args.no_reso and args.reso_model == "pull" and not has_cov:
        sys.exit("--reso_model pull requires Covariance branches in BOTH inputs: "
                 "use --reso_model absolute.")

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

    # -------- data-driven feature normalisation, bins and plot ranges
    all_pt = np.concatenate([dK["t1_pT"], dK["t2_pT"], dL["t1_pT"], dL["t2_pT"]])
    x_all = feature_raw(args.feature, all_pt)
    q16, q50, q84 = np.quantile(x_all, [0.16, 0.50, 0.84])
    x_ref = float(q50)
    x_scale = float(max(0.5 * (q84 - q16), 1e-3))    # robust sigma
    # widen the clamp so it never binds inside the exported lookup range
    lookup_pt_min, lookup_pt_max = min(args.lookup_pt_values), max(args.lookup_pt_values)
    x_lookup = feature_raw(args.feature, np.array([lookup_pt_min, lookup_pt_max]))
    x_clamp = max(X_CLAMP, float(np.abs((x_lookup - x_ref) / x_scale).max()))
    pt_lo, pt_hi = np.quantile(all_pt, [0.005, 0.995])
    pt_lo = min(float(pt_lo), float(lookup_pt_min))
    pt_hi = max(float(pt_hi), float(lookup_pt_max))
    u_lo = 1.0 / pt_hi
    u_hi = 1.0 / pt_lo
    if args.loss_pt_binning == "fixed":
        # edges as given; daughters outside them fall in the first / last bin
        pt_edges = np.asarray(args.loss_pt_edges, dtype=float)
    elif args.loss_pt_binning == "log":
        # equal width in ln pT over the lookup range; daughters outside it fall in the first / last bin
        pt_edges = np.geomspace(lookup_pt_min, lookup_pt_max, args.loss_pt_bins + 1)
    else:
        pt_edges = np.unique(np.quantile(all_pt, np.linspace(0.0, 1.0, args.loss_pt_bins + 1)))
        if pt_edges.size < 4:                            # degenerate: fall back
            pt_edges = np.linspace(pt_lo, pt_hi, args.loss_pt_bins + 1)
    pt_slices = semi_even_pt_grid(pt_lo, pt_hi, args.kappa_slices)
    curv_slices = 1.0 / pt_slices

    # stage-1 loss cells: n_eta x n_phi per daughter-pT bin, the coarse grid in the
    # bins whose lower edge is >= loss_coarse_above_pt (negative: no coarse bins)
    coarse = torch.tensor(pt_edges[:-1] >= args.loss_coarse_above_pt) & (args.loss_coarse_above_pt >= 0)
    ne = torch.where(coarse, args.loss_n_eta_coarse, args.loss_n_eta)
    nph = torch.where(coarse, args.loss_n_phi_coarse, args.loss_n_phi)
    n_cells_pt = ne * nph
    loss_bins = dict(pt_edges=torch.tensor(pt_edges, dtype=torch.float64), ne=ne, nph=nph,
                     offset=torch.cumsum(n_cells_pt, 0) - n_cells_pt,
                     n_cells=int(n_cells_pt.sum()))

    meta = dict(has_cov=has_cov,
                norm=dict(kind=args.feature, x_ref=x_ref, x_scale=x_scale,
                          x_clamp=x_clamp),
                pt_edges=pt_edges, loss_bins=loss_bins,
                pt_slices=pt_slices, curv_slices=curv_slices,
                pt_plot=(float(pt_lo), float(pt_hi)),
                curv_plot=(float(u_lo), float(u_hi)))
    print(f"[norm] feature '{args.feature}': x_ref = {x_ref:.4f} "
          f"{FEATURE_UNITS[args.feature]}, x_scale = {x_scale:.4f}, "
          f"clamp +-{x_clamp:.2f}")
    print(f"[bins] daughter pT loss-bin edges ({args.loss_pt_binning}): "
          f"{np.array2string(pt_edges, precision=3)}")
    print(f"[bins] (eta x phi) cells per daughter-pT bin: "
          f"{' '.join(f'{a}x{b}' for a, b in zip(ne.tolist(), nph.tolist()))}")
    return K, L, meta
