#!/usr/bin/env python3
"""Training-procedure validation: every convergence trial against the float64 Adam reference.

Trials are <method>_e<epochs>_s<seed> (method: adam64, adam32, cosine64, cosine32,
lbfgs64, lbfgs32). The reference kappa map is the seed mean of adam64 at its largest
epoch count. kappa differences are in units of sigma_stat (bootstrap,
from the syst campaign) and summarised as the RMS over the lookup grid, for
pT < PT_CONSTRAINED and for the full grid.

The trials ran with the code in <campaign>/code_snapshot/, whose logs carry the
"stage 1: N loss/gradient evaluations" line read here (n_eval); the analysis code
no longer prints it.

All comparisons are reported as numbers, with the seed-to-seed spread as the
yardstick: the map is not expected to reach a fixed point at the sigma_stat
level, and the procedure dependence enters the map uncertainty as the seed and
training-length sources of the syst campaign.

usage (from training/, in the venv shell):
  python convergence/compare.py <convergence campaign dir> <syst campaign dir>
  -> <campaign>/convergence_summary.csv   one row per trial
     <campaign>/convergence_vs_pt.csv     RMS over (q, eta, phi) of dkappa/sigma_stat per pT, per comparison
"""
import argparse
import csv
import glob
import os
import re
from datetime import datetime
import numpy as np

PT_CONSTRAINED = 2.0        # GeV; above it the map is an extrapolation (see the syst campaign)


def read_lookup(path):
    """Returns (column names, values) of a kappa_lookup.csv."""
    with open(path) as fh:
        body = [l for l in fh if not l.startswith("#")]
    return body[0].strip().split(","), np.loadtxt(body[1:], delimiter=",", ndmin=2)


ap = argparse.ArgumentParser(description=__doc__,
                             formatter_class=argparse.RawDescriptionHelpFormatter)
ap.add_argument("campaign")
ap.add_argument("syst_campaign")
a = ap.parse_args()

names, unc = read_lookup(os.path.join(a.syst_campaign, "kappa_lookup_unc.csv"))
pt = unc[:, names.index("pT")]
sigma_stat = unc[:, names.index("sigma_stat")]
constrained = pt < PT_CONSTRAINED
pt_values = np.unique(pt)

trials = {}
for meta in sorted(glob.glob(os.path.join(a.campaign, "*", "run_metadata.toml"))):
    tdir = os.path.dirname(meta)
    name = os.path.basename(tdir)
    method, epochs, seed = re.fullmatch(r"(\w+?)_e(\d+)_s(\d+)", name).groups()
    cols, lk = read_lookup(os.path.join(tdir, "kappa_lookup.csv"))
    if not np.array_equal(lk[:, cols.index("pT")], pt):
        raise SystemExit(f"{name}: lookup grid differs from the syst campaign grid")
    with open(os.path.join(tdir, "stage1_training_history.csv")) as fh:
        last = list(csv.DictReader(fh))[-1]
    with open(os.path.join(a.campaign, "logs", f"{name}.out")) as fh:
        log = fh.read()
    start = re.search(r"start (\S+),", log).group(1)
    end = re.search(r"^end (\S+)", log, re.M).group(1)
    trials[name] = dict(
        trial=name, method=method, epochs=int(epochs), seed=int(seed),
        n_eval=int(re.search(r"stage 1: (\d+) loss/gradient evaluations", log).group(1)),
        wall_min=(datetime.fromisoformat(end) - datetime.fromisoformat(start)).total_seconds() / 60,
        host=re.search(r"^host (\S+),", log, re.M).group(1),
        final_loss=float(last["loss"]),
        kshort_mean_MeV=1e3 * float(last["kshort_mean_GeV"]),
        kshort_width_MeV=1e3 * float(last["kshort_width_GeV"]),
        lambda_width_MeV=1e3 * float(last["lambda_width_GeV"]),
        anti_lambda_width_MeV=1e3 * float(last["anti_lambda_width_GeV"]),
        kappa=lk[:, cols.index("kappa")])
print(f"{len(trials)} finished trials")

# seed-mean kappa and loss per (method, epochs)
groups = {}
for t in trials.values():
    groups.setdefault((t["method"], t["epochs"]), []).append(t)
mean_kappa = {k: np.mean([t["kappa"] for t in g], axis=0) for k, g in groups.items()}
ref_key = ("adam64", max(e for m, e in groups if m == "adam64"))
ref_losses = [t["final_loss"] for t in groups[ref_key]]
ref_loss = np.mean(ref_losses)
ref_loss_spread = np.std(ref_losses, ddof=1) if len(ref_losses) > 1 else 0.0
print(f"reference: {ref_key[0]} {ref_key[1]} epochs, {len(groups[ref_key])} seeds, "
      f"final loss {ref_loss:.5f} +- {ref_loss_spread:.5f} (seed s.d.)")

rms_rows = []        # (comparison label, dkappa) -> convergence_vs_pt.csv


def rms_ratio(dk, mask):
    return float(np.sqrt(np.mean((dk[mask] / sigma_stat[mask])**2)))


def record(label, dk):
    rms_rows.append((label, dk))
    return rms_ratio(dk, constrained), rms_ratio(dk, np.ones_like(constrained))


for t in trials.values():
    t["rms_vs_ref_constrained"], t["rms_vs_ref_all"] = (
        rms_ratio(t["kappa"] - mean_kappa[ref_key], constrained),
        rms_ratio(t["kappa"] - mean_kappa[ref_key], np.ones_like(constrained)))

for chain in ("adam64", "adam32"):
    adam_epochs = sorted(e for m, e in groups if m == chain)
    if len(adam_epochs) < 2:
        continue
    print(f"\n[1] {chain} epoch doublings: RMS dkappa/sigma_stat, pT < {PT_CONSTRAINED} GeV (all pT)")
    for e1, e2 in zip(adam_epochs, adam_epochs[1:]):
        c, f = record(f"{chain} {e1} -> {e2}", mean_kappa[(chain, e2)] - mean_kappa[(chain, e1)])
        print(f"  {e1:5d} -> {e2:5d} epochs: {c:5.2f} ({f:5.2f})")

print(f"\n[2] seed-to-seed spread |k_s17 - k_s18|/sqrt2 per group, pT < {PT_CONSTRAINED} GeV (all pT)")
for (m, e), g in sorted(groups.items()):
    if len(g) == 2:
        c, f = record(f"seed spread {m} {e}", (g[0]["kappa"] - g[1]["kappa"]) / np.sqrt(2.0))
        print(f"  {m:9s} {e:5d}: {c:5.2f} ({f:5.2f})")

print(f"\n[3] float32 vs float64, same seed and epochs (compare with the seed spread in [2])")
for t in sorted(trials.values(), key=lambda t: t["trial"]):
    if not t["method"].endswith("32"):
        continue
    twin = trials.get(t["trial"].replace("32_", "64_", 1))
    if twin is None:
        continue
    c, f = record(f"float32 - float64 {t['method'][:-2]} {t['epochs']} s{t['seed']}", t["kappa"] - twin["kappa"])
    print(f"  {t['trial']:18s} vs {twin['trial']:18s}: {c:5.3f} ({f:5.3f})")

print(f"\n[4] each procedure (seed mean) vs reference {ref_key[0]} {ref_key[1]} "
      f"(final loss {ref_loss:.5f} +- {ref_loss_spread:.5f})")
print(f"  {'procedure':16s} {'evals/seed':>10s} {'wall/seed [min]':>15s} {'RMS (pT<2)':>10s} {'RMS (all)':>9s} "
      f"{'loss':>8s} {'K0s width':>9s}")
for (m, e), g in sorted(groups.items(), key=lambda kv: (kv[0][0], kv[0][1])):
    if (m, e) == ref_key:
        continue
    c, f = record(f"{m} {e} vs reference", mean_kappa[(m, e)] - mean_kappa[ref_key])
    loss = np.mean([t["final_loss"] for t in g])
    print(f"  {m + ' ' + str(e):16s} {np.mean([t['n_eval'] for t in g]):10.0f} "
          f"{np.mean([t['wall_min'] for t in g]):15.0f} {c:10.2f} {f:9.2f} {loss:8.5f} "
          f"{np.mean([t['kshort_width_MeV'] for t in g]):9.3f}")

out = os.path.join(a.campaign, "convergence_summary.csv")
fields = [k for k in next(iter(trials.values())) if k != "kappa"]
with open(out, "w", newline="") as fh:
    wr = csv.DictWriter(fh, fieldnames=fields, extrasaction="ignore")
    wr.writeheader()
    wr.writerows(sorted(trials.values(), key=lambda t: (t["method"], t["epochs"], t["seed"])))
out_pt = os.path.join(a.campaign, "convergence_vs_pt.csv")
with open(out_pt, "w", newline="") as fh:
    wr = csv.writer(fh)
    wr.writerow(["comparison", "pT", "rms_dkappa_over_sigma_stat"])
    for label, dk in rms_rows:
        for p in pt_values:
            m = pt == p
            wr.writerow([label, f"{p:.6f}", f"{rms_ratio(dk, m):.5f}"])
print(f"\nwrote {out}\nwrote {out_pt}")
