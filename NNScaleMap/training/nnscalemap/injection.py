"""Injection campaign: toy two-body decays and the go/no-go recovery test."""
import math
import os
import numpy as np
import torch

from .constants import ETA_RANGE, M_K0S, M_LAMBDA, PION_MASS, PROTON_MASS, PT_SANITY, WIN
from .kinematics import beta_of, mass_jacobians
from .stage1 import IdentityKappa
from .stage2 import build_groups, fit_global_reduced, set_c_indices, sigma_mass_net, train_resolution
from .figures_stage2 import core_width, resolution_diagnostics

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
