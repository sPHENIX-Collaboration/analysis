#!/usr/bin/env python3
"""Leg-2 Lambda/anti-Lambda split term: A/B comparison (internal diagnostic).

Campaign layout: <campaign>/A_s<seed>/ (lam_split2 = 0) and <campaign>/B_s<seed>/
(lam_split2 = 1), stage 1 trained on the same events, one network seed per
directory. On those events, for every model:
  - Lambda minus anti-Lambda corrected peak position in bins of proton pT (leg 2,
    the bins of the leg-2 term) and of pion pT (leg 1, the bins of the leg-1 term)
  - Lambda + anti-Lambda peak position minus PDG in bins of proton pT
  - K0s peak position minus PDG in bins of leg-1 pion pT
  - kappa(B) - kappa(A) over the lookup grid
pT bins are the stage-1 loss-bin edges. Points are the seed mean, error bars the
seed-to-seed standard deviation; the uncorrected peaks are shown for reference.

usage (from training/, in the venv shell):
  python leg2_ab/compare.py <campaign dir>
      ->  <campaign>/figures/leg2_ab_{peaks,kappa}.{png,pdf}, <campaign>/summary.md
"""
import argparse
import glob
import math
import os
import sys
import tomllib
from argparse import Namespace
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import torch

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nnscalemap.constants import M_K0S, M_LAMBDA, SIG0
from nnscalemap.data import load
from nnscalemap.export import _savefig
from nnscalemap.peakfit import fit_peak_with_error
from nnscalemap.stage1 import IdentityKappa, corrected_mass, load_stage1

MIN_FIT_CANDIDATES = 200          # per pT bin and species; fewer -> no peak fit
VARIANTS = {"A": r"A: $\lambda_{\rm split,2} = 0$", "B": r"B: $\lambda_{\rm split,2} = 1$"}
COLORS = {"A": "#0072B2", "B": "#E69F00", "raw": "0.55"}     # Okabe-Ito blue, orange; grey

ap = argparse.ArgumentParser(description=__doc__,
                             formatter_class=argparse.RawDescriptionHelpFormatter)
ap.add_argument("campaign", help="directory holding A_s<seed>/ and B_s<seed>/")
a = ap.parse_args()
outdir = os.path.join(a.campaign, "figures")
os.makedirs(outdir, exist_ok=True)

runs = {v: sorted(glob.glob(os.path.join(a.campaign, f"{v}_s*", "run_metadata.toml"))) for v in VARIANTS}
if not runs["A"] or not runs["B"]:
    sys.exit(f"need finished A_s* and B_s* runs (run_metadata.toml) under {a.campaign}")
with open(runs["A"][0], "rb") as fh:
    opts = tomllib.load(fh)["options"]
K, L, meta = load(Namespace(**opts))
edges = meta["pt_edges"]
centres = np.sqrt(np.maximum(edges[:-1], 0.05 * edges[1:]) * edges[1:])   # geometric, first edge may be 0

pt_p, pt_pi, pt_k = L["pt2"].numpy(), L["pt1"].numpy(), K["pt1"].numpy()
is_lam = L["is_lam"].numpy()
everyone = np.ones_like(is_lam)


def peaks(m, pt, sel, mpdg, sig0, window):
    """Fitted peak position [MeV] per pT bin, NaN below MIN_FIT_CANDIDATES or for a failed fit."""
    mu = np.full(len(edges) - 1, np.nan)
    for b in range(len(edges) - 1):
        s = sel & (pt >= edges[b]) & (pt < edges[b + 1])
        if s.sum() >= MIN_FIT_CANDIDATES:
            mu[b] = 1e3 * fit_peak_with_error(m[s], mpdg, sig0, *window)[0]
    return mu


def observables(model):
    with torch.no_grad():
        mK = corrected_mass(model, K)[0].numpy()
        mL = corrected_mass(model, L)[0].numpy()
    lam = lambda pt, sel: peaks(mL, pt, sel, M_LAMBDA, SIG0["Lambda"], opts["lam_window"])
    return {
        "split_p": lam(pt_p, is_lam) - lam(pt_p, ~is_lam),
        "split_pi": lam(pt_pi, is_lam) - lam(pt_pi, ~is_lam),
        "lam_pdg": lam(pt_p, everyone) - 1e3 * M_LAMBDA,
        "k0s_pdg": peaks(mK, pt_k, np.ones(pt_k.size, bool), M_K0S, SIG0["K0s"], opts["k0s_window"])
                   - 1e3 * M_K0S,
    }


raw = observables(IdentityKappa())
res, kappa = {}, {}
for v, paths in runs.items():
    per_seed = []
    kappa[v] = []
    for p in paths:
        d = os.path.dirname(p)
        model = load_stage1(os.path.join(d, "model.pt"), hidden=opts["hidden"], layers=opts["layers"])
        per_seed.append(observables(model))
        kappa[v].append(pd.read_csv(os.path.join(d, "kappa_lookup.csv"), comment="#"))
        print(f"[{v}] {os.path.basename(d)} done")
    res[v] = {key: np.array([s[key] for s in per_seed]) for key in raw}

# ---------------------------------------------------------------- peak figure
panels = (("split_p", r"Proton $p_T$ [GeV]", r"$M_{\Lambda} - M_{\bar\Lambda}$ [MeV]",
           r"$\Lambda$ vs $\bar\Lambda$ peak, binned in proton $p_T$ (leg-2 term)"),
          ("split_pi", r"Pion $p_T$ [GeV]", r"$M_{\Lambda} - M_{\bar\Lambda}$ [MeV]",
           r"$\Lambda$ vs $\bar\Lambda$ peak, binned in pion $p_T$ (leg-1 term)"),
          ("lam_pdg", r"Proton $p_T$ [GeV]", r"$M_{\Lambda+\bar\Lambda} - M_{\rm PDG}$ [MeV]",
           r"$\Lambda + \bar\Lambda$ peak position"),
          ("k0s_pdg", r"$\pi^-$ $p_T$ [GeV]", r"$M_{K^0_S} - M_{\rm PDG}$ [MeV]",
           r"$K^0_S$ peak position"))
fig, axes = plt.subplots(2, 2, figsize=(11.0, 8.4))
for ax, (key, xlabel, ylabel, title) in zip(axes.flat, panels):
    ax.plot(centres, raw[key], "s", mfc="none", color=COLORS["raw"], label="Uncorrected")
    for i, v in enumerate(VARIANTS):
        mean = np.nanmean(res[v][key], axis=0) if len(runs[v]) else None
        std = np.nanstd(res[v][key], axis=0, ddof=1) if len(runs[v]) > 1 else np.zeros_like(mean)
        ax.errorbar(centres * (0.97 if i == 0 else 1.03), mean, yerr=std, fmt="o", ms=5, capsize=2,
                    color=COLORS[v], label=f"{VARIANTS[v]} ({len(runs[v])} seeds)")
    ax.axhline(0.0, color="k", ls="--", lw=0.8)
    ax.set_xscale("log")
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.tick_params(which="both", direction="in", top=True, right=True)
    lo, hi = ax.get_ylim()
    ax.set_ylim(lo, hi + 0.45 * (hi - lo))              # headroom for header and legend
    ax.text(0.03, 0.96, "sPHENIX Internal\n" + title, transform=ax.transAxes, va="top", fontsize=9)
    ax.legend(fontsize=8, loc="upper right", frameon=False)
fig.suptitle("Error bars: seed-to-seed standard deviation; empty bins have fewer than "
             f"{MIN_FIT_CANDIDATES} candidates", fontsize=9)
_savefig(fig, outdir, "leg2_ab_peaks")

# ---------------------------------------------------------------- kappa figure
grid = kappa["A"][0][["q", "pT", "eta", "phi"]]
keep = (grid["phi"] < math.pi - 1e-3).to_numpy()                 # phi = +pi repeats -pi
kA = np.array([k["kappa"].to_numpy() for k in kappa["A"]])
kB = np.array([k["kappa"].to_numpy() for k in kappa["B"]])
diff = 1e3 * (kB.mean(0) - kA.mean(0))
seed_sd = 1e3 * kA.std(0, ddof=1) if len(kA) > 1 else np.full(diff.shape, np.nan)
fig, ax = plt.subplots(figsize=(6.4, 4.8))
pts = np.sort(grid["pT"].unique())
for q, marker in ((-1.0, "v"), (1.0, "^")):
    sel = keep & (grid["q"] == q).to_numpy()
    med = [np.median(diff[sel & (grid["pT"] == p).to_numpy()]) for p in pts]
    lo = [np.percentile(diff[sel & (grid["pT"] == p).to_numpy()], 16) for p in pts]
    hi = [np.percentile(diff[sel & (grid["pT"] == p).to_numpy()], 84) for p in pts]
    ax.errorbar(pts, med, yerr=[np.subtract(med, lo), np.subtract(hi, med)], fmt=marker, ms=5,
                capsize=2, color=COLORS["B"] if q > 0 else COLORS["A"],
                label=fr"$q = {q:+.0f}$: median and 16-84% over ($\eta$, $\phi$)")
sd = [np.median(seed_sd[keep & (grid["pT"] == p).to_numpy()]) for p in pts]
ax.plot(pts, sd, "k:", label="Seed-to-seed SD of A (median)")
ax.plot(pts, -np.asarray(sd), "k:")
ax.axhline(0.0, color="k", ls="--", lw=0.8)
ax.set_xscale("log")
ax.set_xlabel(r"Lookup $p_T$ [GeV]")
ax.set_ylabel(r"$\kappa_B - \kappa_A$ (seed means) [$10^{-3}$]")
ax.tick_params(which="both", direction="in", top=True, right=True)
lo, hi = ax.get_ylim()
ax.set_ylim(lo, hi + 0.45 * (hi - lo))
ax.text(0.03, 0.96, "sPHENIX Internal\nEffect of the leg-2 split term on the $\\kappa$ map",
        transform=ax.transAxes, va="top", fontsize=9)
ax.legend(fontsize=8, loc="upper right", frameon=False)
_savefig(fig, outdir, "leg2_ab_kappa")

# ---------------------------------------------------------------- summary
def fmt(x):
    return "-" if not np.isfinite(x) else f"{x:+.3f}"


lines = ["# Leg-2 Lambda/anti-Lambda split term: A/B comparison", "",
         f"Campaign: `{a.campaign}`. A: lam_split2 = 0 ({len(runs['A'])} seeds); "
         f"B: lam_split2 = 1 ({len(runs['B'])} seeds); same events, same card otherwise.",
         f"Peaks in MeV, mean ± seed-to-seed SD; ratio = |B - A| / SD(A). Bins with fewer than "
         f"{MIN_FIT_CANDIDATES} candidates are empty.", "",
         "Figures: `figures/leg2_ab_peaks.{png,pdf}`, `figures/leg2_ab_kappa.{png,pdf}`.", ""]
for key, xlabel, ylabel, title in panels:
    plain = title.replace("$", "").replace("\\bar\\Lambda", "anti-Lambda").replace("\\Lambda", "Lambda") \
                 .replace("K^0_S", "K0s").replace("p_T", "pT")
    lines += [f"## {plain}", "", "| pT bin [GeV] | Uncorrected | A | B | B - A | ratio |",
              "|---|---|---|---|---|---|"]
    for b in range(len(edges) - 1):
        mA, mB = np.nanmean(res["A"][key][:, b]), np.nanmean(res["B"][key][:, b])
        sA = np.nanstd(res["A"][key][:, b], ddof=1) if len(runs["A"]) > 1 else np.nan
        sB = np.nanstd(res["B"][key][:, b], ddof=1) if len(runs["B"]) > 1 else np.nan
        ratio = abs(mB - mA) / sA if np.isfinite(sA) and sA > 0 else np.nan
        lines.append(f"| {edges[b]:.3g}-{edges[b + 1]:.3g} | {fmt(raw[key][b])} | {fmt(mA)} ± {sA:.3f} | "
                     f"{fmt(mB)} ± {sB:.3f} | {fmt(mB - mA)} | {ratio:.1f} |")
    lines.append("")
lines += ["## kappa map", "", "| lookup pT [GeV] | median |kappa_B - kappa_A| [1e-3] | median seed SD of A [1e-3] |",
          "|---|---|---|"]
for p, s in zip(pts, sd):
    sel = keep & (grid["pT"] == p).to_numpy()
    lines.append(f"| {p:g} | {np.median(np.abs(diff[sel])):.3f} | {s:.3f} |")
log = os.path.join(a.campaign, "logs", os.path.basename(os.path.dirname(runs["A"][0])) + ".out")
if os.path.exists(log):
    with open(log) as fh:
        lines += ["", "## Loss-cell occupancy (from the A training log)", "", "```"]
        lines += [l.rstrip() for l in fh if l.startswith("[bins]")]
        lines += ["```"]
with open(os.path.join(a.campaign, "summary.md"), "w") as fh:
    fh.write("\n".join(lines) + "\n")
print(f"wrote {outdir}/leg2_ab_{{peaks,kappa}}.png/.pdf and {a.campaign}/summary.md")
