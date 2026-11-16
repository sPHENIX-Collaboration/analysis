#!/usr/bin/env python3
"""Held-out closure score of the hpo/ trials.

Every trial trains stage 1 on the even half; its run_metadata.toml records the
inputs and the resolved options. Each trial is scored on the odd half
(held out) and on the even half (training), whose difference is the
overfitting gap:
  mass_term   signal-weighted mean pull^2 of the V0 mass in the stage-1 loss
              cells (daughter pT x eta x phi x charge), K0s + Lambda, as in
              the training loss
  alpha_term  signal-weighted mean <alpha>^2 of K0s in candidate (pT, eta, phi) cells
  peaks       fitted K0s, Lambda and anti-Lambda peak positions and widths [MeV]
The loss cells come from the nominal card, so all trials are scored in the same cells.

usage (from training/, in the venv shell):
  python hpo/score.py <campaign dir>    ->  <campaign dir>/hpo_summary.csv
"""
import argparse
import csv
import glob
import os
import sys
import tomllib
from argparse import Namespace
import numpy as np
import torch

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nnscalemap.constants import M_K0S, M_LAMBDA, SIG0
from nnscalemap.data import load
from nnscalemap.kinematics import bin_index, binned_mean_sq
from nnscalemap.peakfit import fit_peak, signal_weights
from nnscalemap.stage1 import corrected_mass, load_stage1, stage1_mass_history_from_masses

ap = argparse.ArgumentParser(description=__doc__,
                             formatter_class=argparse.RawDescriptionHelpFormatter)
ap.add_argument("campaign", help="directory holding one subdirectory per trial")
a = ap.parse_args()

# run_metadata.toml is written when a run ends, so this lists finished trials only
trials = sorted(os.path.dirname(p) for p in
                glob.glob(os.path.join(a.campaign, "*", "run_metadata.toml")))
if not trials:
    sys.exit(f"no finished trial (run_metadata.toml) under {a.campaign}")
opts = {}
for t in trials:
    with open(os.path.join(t, "run_metadata.toml"), "rb") as fh:
        opts[t] = tomllib.load(fh)["options"]
print(f"{len(trials)} finished trials")

# all trials share the card and the even-half inputs: read both halves once
halves = {}
for half in ("odd", "even"):
    args = Namespace(**opts[trials[0]])
    args.kshort = [p.replace("_even.root", f"_{half}.root") for p in args.kshort]
    args.lam = [p.replace("_even.root", f"_{half}.root") for p in args.lam]
    halves[half] = load(args)

rows = []
for t in trials:
    o = opts[t]
    model = load_stage1(os.path.join(t, "model.pt"), hidden=o["hidden"], layers=o["layers"])
    for half, (K, L, meta) in halves.items():
        with torch.no_grad():
            mK, (k1, k2), _ = corrected_mass(model, K)
            mL, _, _ = corrected_mass(model, L)
        _, _, pK = fit_peak(mK.numpy(), M_K0S, SIG0["K0s"], *o["k0s_window"],
                            bkg_order=o["bkg_order"])
        _, _, pL = fit_peak(mL.numpy(), M_LAMBDA, SIG0["Lambda"], *o["lam_window"],
                            bkg_order=o["bkg_order"])
        wK = torch.tensor(signal_weights(mK.numpy(), pK))
        wL = torch.tensor(signal_weights(mL.numpy(), pL))
        mass_term = 0.0
        for S, m, w, sig0 in ((K, mK, wK, SIG0["K0s"]), (L, mL, wL, SIG0["Lambda"])):
            pull = (m - S["M"]) / sig0
            for i in ("1", "2"):
                idx, nb = bin_index(S["pt" + i], S["eta" + i], S["phi" + i], meta["loss_bins"])
                v, _ = binned_mean_sq(idx + (S["q" + i] > 0).long() * nb, 2 * nb, pull, w)
                mass_term += v.item()
        p1, p2 = k1 * K["pt1"], k2 * K["pt2"]
        idxc, nbc = bin_index(K["cpt"], K["ceta"], K["cphi"], meta["loss_bins"])
        alpha_term, _ = binned_mean_sq(idxc, nbc, (p1 - p2) / (p1 + p2), wK)

        peaks = stage1_mass_history_from_masses(mK.numpy(), mL.numpy(), L["is_lam"].numpy())
        row = dict(trial=os.path.basename(t), half=half,
                   mass_term=mass_term, alpha_term=alpha_term.item())
        for key, val in peaks.items():
            row[key.replace("_GeV", "_MeV")] = 1e3 * val
        rows.append(row)

out = os.path.join(a.campaign, "hpo_summary.csv")
with open(out, "w", newline="") as fh:
    wr = csv.DictWriter(fh, fieldnames=list(rows[0]))
    wr.writeheader()
    wr.writerows(rows)

train_mass = {r["trial"]: r["mass_term"] for r in rows if r["half"] == "even"}
held = sorted((r for r in rows if r["half"] == "odd"), key=lambda r: r["mass_term"])
print(f"\n{'trial':16s} {'mass (odd)':>11s} {'mass (even)':>12s} "
      f"{'alpha (odd)':>12s} {'K0s width (odd) [MeV]':>22s}")
for r in held:
    print(f"{r['trial']:16s} {r['mass_term']:11.5f} {train_mass[r['trial']]:12.5f} "
          f"{r['alpha_term']:12.3e} {r['kshort_width_MeV']:22.3f}")
nominal = [r["mass_term"] for r in held if r["trial"].startswith("nominal")]
if len(nominal) > 1:
    print(f"\nseed-to-seed spread of the held-out mass term (nominal_*): "
          f"std {np.std(nominal, ddof=1):.5f} over {len(nominal)} seeds")
print(f"wrote {out}")
