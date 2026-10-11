#!/usr/bin/env python3
"""pT-stratified skim of KFParticle K0s / Lambda nTuples for NNScaleMap training.

Keeps every candidate whose larger daughter pT exceeds ptmax_threshold and a
random bulk_fraction of the rest, writing only the configured branches.
Output: <skim_dir>/<production>/<tag>/{Kshort,Lambda0}/skim_<run>.root, each
with a read-only .toml sidecar (card values, source sha256, kept counts).

    python skim.py --config configs/skim_V0_polyseeding_ana573.toml
    python skim.py --config configs/skim_V0_polyseeding_ana573.toml --file <one input>
"""
import argparse
import json
import os
import re
import sys
import tomllib
import zlib
from datetime import datetime
from glob import glob as expand_glob

import numpy as np
import uproot

from nnscalemap.constants import MOTHER_PREFIXES
from nnscalemap.data import MOTHER_VARS, TRACK_VARS
from nnscalemap.export import sha256sum

SPECIES = {"kshort": "Kshort", "lam": "Lambda0"}     # card key -> output subfolder


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--config", required=True, help="TOML skim card")
    ap.add_argument("--file", default=None,
                    help="skim only this input (one of the card's files), e.g. one Condor job per file")
    cli = ap.parse_args()
    with open(cli.config, "rb") as fh:
        card = tomllib.load(fh)
    inp, sel, br, out = card["input"], card["selection"], card["branches"], card["output"]

    # branches to write, and the reader's required set checked before any file is read
    branches = {}
    for key in SPECIES:
        branches[key] = list(br["stage1"][key])
        if br["keep_stage2"]:
            branches[key] += br["stage2"][key]
        listed = set(branches[key])
        if not any(all(f"{m}_{x}" in listed for x in MOTHER_VARS) for m in MOTHER_PREFIXES):
            sys.exit(f"[branches] {key}: the training reader needs <mother>_{MOTHER_VARS} "
                     f"for one mother in {MOTHER_PREFIXES}")
        required = [f"track_{i}_{x}" for i in (1, 2) for x in TRACK_VARS]
        if key == "lam":
            required += ["track_1_PDG_ID", "track_2_PDG_ID"]
        missing = [b for b in required if b not in listed]
        if missing:
            sys.exit(f"[branches] {key}: the training reader needs {missing}")

    thr, frac, seed = float(sel["ptmax_threshold"]), float(sel["bulk_fraction"]), int(sel["seed"])
    tag = (f"ptmax{thr}_bulk{frac}_seed{seed}".replace(".", "p")
           + ("_stage1stage2" if br["keep_stage2"] else "_stage1"))
    tagdir = os.path.join(out["skim_dir"], inp["production"], tag)

    jobs = []
    for key, sub in SPECIES.items():
        for pattern in inp[key]:
            matches = sorted(expand_glob(pattern))
            if not matches:
                sys.exit(f"[input] {key}: no file matches '{pattern}'")
            jobs += [(key, sub, p) for p in matches]
    if cli.file:
        jobs = [j for j in jobs if os.path.abspath(j[2]) == os.path.abspath(cli.file)]
        if not jobs:
            sys.exit(f"--file {cli.file} is not among the card's inputs")

    failed = []
    for key, sub, path in jobs:
        base = os.path.basename(path)
        run = re.search(r"run(\d+)", base)
        name = f"skim_run{run.group(1)}" if run else f"skim_{os.path.splitext(base)[0]}"
        odir = os.path.join(tagdir, sub)
        os.makedirs(odir, exist_ok=True)
        print(f"[{sub}] {path} -> {odir}/{name}.root", flush=True)

        # per-file seed: reproducible whatever order or job split the files are skimmed in
        rng = np.random.default_rng([seed, zlib.crc32(base.encode())])
        kept = {b: [] for b in branches[key]}
        n_seen = n_high = n_bulk = 0
        try:
            t = uproot.open(path)[inp["tree"]]
            for raw in t.iterate(branches[key], library="np", step_size=100000):
                ptmax = np.maximum(raw["track_1_pT"], raw["track_2_pT"])
                high = ptmax > thr
                keep = high | (rng.random(ptmax.size) < frac)
                n_seen += ptmax.size
                n_high += int(high.sum())
                n_bulk += int((keep & ~high).sum())
                for b in branches[key]:
                    a = raw[b][keep]
                    kept[b].append(np.stack(a) if a.dtype == object and a.size else a)
        except Exception as e:
            print(f"[{sub}] FAILED {path}: {type(e).__name__}: {str(e).splitlines()[0]}", flush=True)
            failed.append(path)
            continue

        opath = os.path.join(odir, f"{name}.root")
        data = {b: np.concatenate(v) for b, v in kept.items()}
        with uproot.recreate(opath) as f:
            # mktree writes a TTree like the source nTuples (dict assignment writes an RNTuple)
            f.mktree(inp["tree"], {b: np.dtype((a.dtype, a.shape[1:])) for b, a in data.items()})
            f[inp["tree"]].extend(data)

        lines = ["# NNScaleMap skim metadata", "", "[skim]",
                 f"finished = {json.dumps(datetime.now().isoformat(timespec='seconds'))}",
                 f"card = {json.dumps(os.path.abspath(cli.config))}",
                 f"production = {json.dumps(inp['production'])}",
                 f"tag = {json.dumps(tag)}",
                 f"species = {json.dumps(sub)}",
                 f"ptmax_threshold = {json.dumps(thr)}",
                 f"bulk_fraction = {json.dumps(frac)}",
                 f"seed = {seed}",
                 f"keep_stage2 = {json.dumps(br['keep_stage2'])}",
                 f"branches = {json.dumps(branches[key])}",
                 "", "[source]",
                 f"path = {json.dumps(os.path.abspath(path))}",
                 f"size_bytes = {os.path.getsize(path)}",
                 f"sha256 = {json.dumps(sha256sum(path))}",
                 f"entries = {n_seen}",
                 "", "[output]",
                 f"path = {json.dumps(opath)}",
                 f"kept = {n_high + n_bulk}",
                 f"kept_above_threshold = {n_high}",
                 f"kept_bulk = {n_bulk}"]
        mpath = os.path.join(odir, f"{name}.toml")
        if os.path.exists(mpath):                  # read-only copy from an earlier skim
            os.remove(mpath)
        with open(mpath, "w") as fh:
            fh.write("\n".join(lines) + "\n")
        os.chmod(mpath, 0o444)
        print(f"[{sub}] kept {n_high + n_bulk} / {n_seen} "
              f"({n_high} above {thr} GeV, {n_bulk} bulk)", flush=True)

    if failed:
        sys.exit("skim FAILED for:\n  " + "\n  ".join(failed))


if __name__ == "__main__":
    main()
