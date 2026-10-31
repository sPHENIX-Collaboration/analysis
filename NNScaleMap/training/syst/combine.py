#!/usr/bin/env python3
"""Per-grid-point uncertainty of the kappa map from the syst/ campaign.

Campaign layout: <campaign>/<source>/<variation>/kappa_lookup.csv, with the
nominal map in <campaign>/nominal/nominal/. At every (q, pT, eta, phi) point
of the lookup grid, each source gives an uncertainty on kappa:
  stat, seed      standard deviation of kappa over the variations
                  (bootstrap replicas; network-initialisation seeds)
  other sources   largest |kappa_variation - kappa_nominal| over the variations
sigma_stat is the "stat" source; sigma_syst is the quadrature sum of all others.

usage (from training/, in the venv shell):
  python syst/combine.py <campaign dir>    ->  <campaign dir>/kappa_lookup_unc.csv
"""
import argparse
import glob
import os
import sys
import numpy as np

SPREAD_SOURCES = ("stat", "seed")
GRID = ("q", "pT", "eta", "phi")


def read_lookup(path):
    """Returns (comment header lines, column names, values) of a kappa_lookup.csv."""
    with open(path) as fh:
        lines = fh.readlines()
    header = [l for l in lines if l.startswith("#")]
    body = [l for l in lines if not l.startswith("#")]
    return header, body[0].strip().split(","), np.loadtxt(body[1:], delimiter=",", ndmin=2)


ap = argparse.ArgumentParser(description=__doc__,
                             formatter_class=argparse.RawDescriptionHelpFormatter)
ap.add_argument("campaign", help="directory holding <source>/<variation>/ outputs")
a = ap.parse_args()

header, names, nom = read_lookup(os.path.join(a.campaign, "nominal", "nominal", "kappa_lookup.csv"))
grid_cols = [names.index(c) for c in GRID]
ik = names.index("kappa")

sigma = {}
for source in sorted(os.listdir(a.campaign)):
    paths = sorted(glob.glob(os.path.join(a.campaign, source, "*", "kappa_lookup.csv")))
    if source == "nominal" or not paths:
        continue
    kappas = []
    for p in paths:
        _, _, v = read_lookup(p)
        if not np.array_equal(v[:, grid_cols], nom[:, grid_cols]):
            sys.exit(f"{p}: lookup grid differs from the nominal one")
        kappas.append(v[:, ik])
    kappas = np.array(kappas)
    if source in SPREAD_SOURCES:
        sigma[source] = kappas.std(axis=0, ddof=1)
    else:
        sigma[source] = np.abs(kappas - nom[:, ik]).max(axis=0)
    print(f"{source:10s} {len(paths):3d} variation(s): sigma_kappa median {np.median(sigma[source]):.2e}, "
          f"max {sigma[source].max():.2e}")

if "stat" not in sigma:
    sys.exit(f"no bootstrap replica outputs under {a.campaign}/stat/")
sigma_stat = sigma["stat"]
syst_sources = [s for s in sigma if s != "stat"]
sigma_syst = np.sqrt(sum(sigma[s]**2 for s in syst_sources))

print("\nfraction of grid points where a source exceeds sigma_stat:")
for s in syst_sources:
    print(f"  {s:10s} {np.mean(sigma[s] > sigma_stat):.2f}")

out = os.path.join(a.campaign, "kappa_lookup_unc.csv")
cols = names + ["sigma_stat", "sigma_syst"] + [f"sigma_{s}" for s in syst_sources]
table = np.column_stack([nom, sigma_stat, sigma_syst] + [sigma[s] for s in syst_sources])
with open(out, "w") as fh:
    fh.writelines(header)
    fh.write("# sigma_*: uncertainty on kappa; stat = bootstrap spread, syst = quadrature sum of "
             + ", ".join(syst_sources) + "\n")
    fh.write(",".join(cols) + "\n")
    np.savetxt(fh, table, delimiter=",",
               fmt=["%.6f"] * len(names) + ["%.6e"] * (len(cols) - len(names)))
print(f"wrote {out}")
