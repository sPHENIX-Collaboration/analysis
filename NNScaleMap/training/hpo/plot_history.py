#!/usr/bin/env python3
"""Stage-1 training histories of the hpo/ trials, overlaid (internal diagnostic).

Reads <campaign>/<trial>/stage1_training_history.csv of every finished trial:
  training loss every epoch; fitted K0s, Lambda, anti-Lambda peak mean and width at
  the history cadence (on the training half).
Trials follow the order of hpo/trials.txt; the nominal seeds are drawn in grey, so the
hyperparameter variations stand out against the seed-to-seed spread.

usage (from training/, in the venv shell):
  python hpo/plot_history.py <campaign dir> [--outdir DIR]
      ->  <outdir>/hpo_training_history.{png,pdf}   (default outdir <campaign>/figures)
"""
import argparse
import os
import sys
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from nnscalemap.constants import STAGE1_HISTORY_SPECIES
from nnscalemap.export import _savefig

SPECIES_LABEL = {"kshort": r"$K^0_S$", "lambda": r"$\Lambda$", "anti_lambda": r"$\bar{\Lambda}$"}

ap = argparse.ArgumentParser(description=__doc__,
                             formatter_class=argparse.RawDescriptionHelpFormatter)
ap.add_argument("campaign", help="directory holding one subdirectory per trial")
ap.add_argument("--outdir", default=None, help="default: <campaign>/figures")
a = ap.parse_args()
outdir = a.outdir or os.path.join(a.campaign, "figures")
os.makedirs(outdir, exist_ok=True)

# trial order: the job list of the campaign
with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "trials.txt")) as fh:
    order = [line.split()[0] for line in fh if line.strip()]
trials = [t for t in order
          if os.path.exists(os.path.join(a.campaign, t, "stage1_training_history.csv"))]
if not trials:
    sys.exit(f"no stage1_training_history.csv of a trials.txt trial under {a.campaign}")
variations = [t for t in trials if not t.startswith("nominal")]
# tab10 without its grey (index 7), which is reserved for the nominal seeds; alternate solid/dashed so
# overlapping variations stay visible
palette = [c for i, c in enumerate(plt.get_cmap("tab10").colors) if i != 7]
colors = dict(zip(variations, palette))
dashes = {t: "-" if i % 2 == 0 else "--" for i, t in enumerate(variations)}

fig = plt.figure(figsize=(14, 11))
grid = fig.add_gridspec(3, 3, height_ratios=(1.1, 1, 1))
ax_loss = fig.add_subplot(grid[0, :])
ax_mean = [fig.add_subplot(grid[1, i]) for i in range(3)]
ax_width = [fig.add_subplot(grid[2, i], sharex=ax_mean[i]) for i in range(3)]

for t in trials:
    df = pd.read_csv(os.path.join(a.campaign, t, "stage1_training_history.csv")).sort_values("epoch")
    nominal = t.startswith("nominal")
    # variations above the seeds; among them shorter runs on top, so a run that stops early is not
    # covered by a longer one following the same path
    style = dict(color="0.55" if nominal else colors[t], lw=1.0 if nominal else 1.8,
                 ls="-" if nominal else dashes[t], label=t,
                 zorder=1 if nominal else 2 + 1.0 / df["epoch"].max())
    loss = df.dropna(subset=["loss"])
    ax_loss.plot(loss["epoch"], loss["loss"], **style)
    for i, (key, _, pdg, _, _) in enumerate(STAGE1_HISTORY_SPECIES):
        peak = df.dropna(subset=[f"{key}_mean_GeV"])
        ax_mean[i].plot(peak["epoch"], 1e3 * (peak[f"{key}_mean_GeV"] - pdg), marker="o", ms=3, **style)
        ax_width[i].plot(peak["epoch"], 1e3 * peak[f"{key}_width_GeV"], marker="o", ms=3, **style)

ax_loss.set_yscale("log")
ax_loss.set_xlabel("Training epoch")
ax_loss.set_ylabel("Stage-1 training loss")
ax_loss.legend(loc="upper left", bbox_to_anchor=(1.01, 1.0), fontsize=9, frameon=False,
               title="Trial (grey: nominal seeds)", title_fontsize=9)
ax_loss.set_title("sPHENIX Internal", loc="left", fontsize=11, style="italic", weight="bold")
for i, (key, _, _, _, _) in enumerate(STAGE1_HISTORY_SPECIES):
    ax_mean[i].axhline(0.0, color="k", ls="--", lw=0.8)
    ax_mean[i].set_title(SPECIES_LABEL[key] + " peak (training half)")
    ax_mean[i].set_ylabel(r"Fitted mean $-$ PDG mass [MeV]")
    ax_width[i].set_ylabel("Fitted width [MeV]")
    ax_width[i].set_xlabel("Training epoch")
    ax_mean[i].tick_params(labelbottom=False)
for ax in [ax_loss] + ax_mean + ax_width:
    ax.grid(True, alpha=0.25)

_savefig(fig, outdir, "hpo_training_history")
print(f"wrote {os.path.join(outdir, 'hpo_training_history')}.png/.pdf ({len(trials)} trials)")
