#!/usr/bin/env python3
"""Resampled copies of skimmed nTuples for the hpo/ and syst/ campaigns.

  --halves         <name>_even.root, <name>_odd.root: entries split by parity
                   (the skims carry no event number, so the split is per candidate)
  --bootstrap N    <name>_boot0.root ... <name>_boot{N-1}.root: N draws with
                   replacement, each the size of the input; reproducible from
                   --seed and the file name
"""
import argparse
import os
import zlib
import numpy as np
import uproot

ap = argparse.ArgumentParser(description=__doc__,
                             formatter_class=argparse.RawDescriptionHelpFormatter)
ap.add_argument("inputs", nargs="+", help="skimmed nTuple(s)")
ap.add_argument("--outdir", required=True)
ap.add_argument("--tree", default="DecayTree")
mode = ap.add_mutually_exclusive_group(required=True)
mode.add_argument("--halves", action="store_true")
mode.add_argument("--bootstrap", type=int, default=0)
ap.add_argument("--seed", type=int, default=42, help="bootstrap seed")
a = ap.parse_args()

os.makedirs(a.outdir, exist_ok=True)
for path in a.inputs:
    base = os.path.splitext(os.path.basename(path))[0]
    data = uproot.open(path)[a.tree].arrays(library="np")
    n = len(next(iter(data.values())))
    if a.halves:
        picks = {"even": np.arange(0, n, 2), "odd": np.arange(1, n, 2)}
    else:
        rng = np.random.default_rng([a.seed, zlib.crc32(base.encode())])
        picks = {f"boot{i}": rng.integers(0, n, n) for i in range(a.bootstrap)}
    for tag, idx in picks.items():
        out = os.path.join(a.outdir, f"{base}_{tag}.root")
        with uproot.recreate(out) as f:
            f[a.tree] = {k: v[idx] for k, v in data.items()}
        print(f"{out}: {idx.size} entries from {n}")
