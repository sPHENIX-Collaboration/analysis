#!/usr/bin/env python3
"""Shift of the kappa map under every syst/ variation (internal diagnostic).

For each source, one panel with the distribution over the lookup grid of
  Delta kappa = kappa_variation - kappa_nominal
for every variation (<campaign>/<source>/<variation>/kappa_lookup.csv against
<campaign>/nominal/nominal/), both charges together, the repeated phi = +pi column
counted once. A narrow distribution centred off zero is a coherent shift of the
scale; a wide one centred on zero is shape noise. Sources follow the order of
syst/variations.txt.

usage (from training/, in the venv shell):
  python syst/plot_variations.py <campaign dir> [--outdir DIR]
      ->  <outdir>/syst_delta_kappa.{png,pdf}   (default outdir <campaign>/figures)
"""
import argparse
import math
import os
import sys
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nnscalemap.export import _savefig

ap = argparse.ArgumentParser(description=__doc__,
                             formatter_class=argparse.RawDescriptionHelpFormatter)
ap.add_argument("campaign", help="directory holding <source>/<variation>/ outputs")
ap.add_argument("--outdir", default=None, help="default: <campaign>/figures")
a = ap.parse_args()
outdir = a.outdir or os.path.join(a.campaign, "figures")
os.makedirs(outdir, exist_ok=True)

# source and variation order: the job list of the campaign
order = []
with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "variations.txt")) as fh:
    for line in fh:
        if line.strip() and not line.startswith("nominal"):
            order.append(tuple(line.split()[:2]))
found = {}
for source, variation in order:
    path = os.path.join(a.campaign, source, variation, "kappa_lookup.csv")
    if os.path.exists(path):
        found.setdefault(source, []).append((variation, path))
if not found:
    sys.exit(f"no variation kappa_lookup.csv under {a.campaign}")

nominal = pd.read_csv(os.path.join(a.campaign, "nominal", "nominal", "kappa_lookup.csv"), comment="#")
keep = nominal["phi"] < math.pi - 1e-3

delta = {}
for source, items in found.items():
    for variation, path in items:
        v = pd.read_csv(path, comment="#")
        delta[source, variation] = 1e3 * (v["kappa"] - nominal["kappa"])[keep].to_numpy()

ncol = min(3, len(found))
nrow = math.ceil(len(found) / ncol)
fig, axes = plt.subplots(nrow, ncol, figsize=(5.2 * ncol, 4.0 * nrow), squeeze=False)
for ax, (source, items) in zip(axes.flat, found.items()):
    cmap = plt.get_cmap("viridis", len(items) + 1)
    # symmetric range per source, so a small source is not squeezed by a large one
    lim = max(np.percentile(np.abs(delta[source, v]), 99.5) for v, _ in items)
    bins = np.linspace(-lim, lim, 61)
    for i, (variation, _) in enumerate(items):
        d = delta[source, variation]
        label = f"{variation}: mean {d.mean():+.2f}, RMS {d.std():.2f}"
        ax.hist(d, bins=bins, histtype="step", lw=1.5, color=cmap(i),
                label=label if len(items) <= 6 else None)
    if len(items) > 6:
        ax.text(0.03, 0.95, f"{len(items)} variations", transform=ax.transAxes, va="top")
    ax.axvline(0.0, color="k", ls="--", lw=0.8)
    ax.set_title(f"Source: {source}")
    ax.set_xlabel(r"$\Delta\kappa = \kappa_{\rm var} - \kappa_{\rm nom}$ [$10^{-3}$]")
    ax.set_ylabel("Grid points")
    ax.grid(True, alpha=0.25)
    if len(items) <= 6:
        ax.legend(fontsize=8, loc="upper right", frameon=False)
    ax.set_ylim(top=1.35 * ax.get_ylim()[1])
for ax in axes.flat[len(found):]:
    ax.set_visible(False)
fig.suptitle(r"sPHENIX Internal: $\kappa$-map shift per variation over the (q, $p_T$, $\eta$, $\phi$) lookup grid",
             fontsize=12)

_savefig(fig, outdir, "syst_delta_kappa")
print(f"wrote {os.path.join(outdir, 'syst_delta_kappa')}.png/.pdf ({len(delta)} variations)")
