#!/usr/bin/env python3
"""Export scale maps to TH2D for ROOT plotting."""
import argparse, numpy as np, torch, uproot
from ml_momentum_calibration_reso_v2 import (KappaNet, ETA_RANGE, semi_even_pt_grid, load_stage1)

DEFAULT_PT_VALUES = (0.3, 0.5, 1.0, 2.0, 3.0)

ap = argparse.ArgumentParser()
ap.add_argument("--model",  default="calib_out_upgrade20260731/model.pt")
ap.add_argument("--out",    default="calib_out_upgrade20260731/kappa_maps.root")
ap.add_argument("--hidden", type=int,   default=48)     # must match training
ap.add_argument("--pt_mode", choices=("fixed", "even"), default="fixed",
                help="fixed: use --pt_values; even: use --slices/--pt_min/--pt_max")
ap.add_argument("--pt_values", nargs="+", default=DEFAULT_PT_VALUES,
                help="fixed pT slices in GeV; accepts spaces or commas")
ap.add_argument("--slices", type=int,   default=9)
ap.add_argument("--pt_min", type=float, default=0.10)
ap.add_argument("--pt_max", type=float, default=5.0)
ap.add_argument("--n_eta",  type=int,   default=200)
ap.add_argument("--n_phi",  type=int,   default=200)
a = ap.parse_args()

def parse_pt_values(values):
    pts = []
    for value in values:
        if isinstance(value, (int, float)):
            pts.append(float(value))
            continue
        pts.extend(float(item) for item in value.split(",") if item.strip())
    return np.asarray(pts, dtype=float)

def pt_slices(args):
    if args.pt_mode == "even":
        return semi_even_pt_grid(args.pt_min, args.pt_max, args.slices)

    pts = parse_pt_values(args.pt_values)
    if pts.size == 0:
        raise ValueError("--pt_values must contain at least one pT slice")
    if np.any(~np.isfinite(pts)) or np.any(pts <= 0.0):
        raise ValueError("--pt_values must be finite positive values")
    return pts

model = load_stage1(a.model, hidden=a.hidden).eval()

eta_e = np.linspace(*ETA_RANGE, a.n_eta + 1)
phi_e = np.linspace(-np.pi, np.pi, a.n_phi + 1)
E, P  = np.meshgrid(0.5 * (eta_e[1:] + eta_e[:-1]),
                    0.5 * (phi_e[1:] + phi_e[:-1]), indexing="ij")
t = lambda x: torch.tensor(np.ascontiguousarray(x.ravel()), dtype=torch.float64)
r = lambda v: v.numpy().reshape(E.shape)          # (n_eta, n_phi) -> TH2 (x,y)

pts = pt_slices(a)
tag = lambda pt: f"pt{pt:.2f}".replace(".", "p")

with uproot.recreate(a.out) as f, torch.no_grad():
    for pt in pts:
        PT = t(np.full(E.shape, pt))
        eps, dlt = model.eps_delta(PT, t(E), t(P))
        f[f"eps_{tag(pt)}"]   = (r(eps), eta_e, phi_e)
        f[f"delta_{tag(pt)}"] = (r(dlt), eta_e, phi_e)
        for q, lab in ((+1.0, "qplus"), (-1.0, "qminus")):
            k, _, _ = model.kappa(PT, t(E), t(P), torch.full_like(PT, q))
            f[f"kappa_{lab}_{tag(pt)}"] = (r(k), eta_e, phi_e)
    f["slices"] = {"pt": pts, "curv": 1.0 / pts}

print(f"wrote {len(pts)} pT slices ({a.n_eta}x{a.n_phi}) to {a.out}")
