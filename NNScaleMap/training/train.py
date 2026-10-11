#!/usr/bin/env python3
import argparse
from glob import glob as expand_glob
import os
import sys
import tomllib
import torch

from nnscalemap.constants import FEATURE_KINDS, M_K0S, M_LAMBDA
from nnscalemap.peakfit import fit_peak, signal_weights
from nnscalemap.data import load
from nnscalemap.stage1 import corrected_mass, load_stage1, save_stage1, train
from nnscalemap.stage2 import build_groups, fit_global_reduced, set_c_indices, train_resolution
from nnscalemap.export import export_lookup, export_reso_lookup, write_metadata
from nnscalemap.figures_stage1 import diagnostics, plot_stage1_training_history
from nnscalemap.figures_stage2 import resolution_diagnostics
from nnscalemap.injection import run_injection


# ---------------------------------------------------------------- main
def main():
    pre = argparse.ArgumentParser(add_help=False)
    pre.add_argument("--config", default=None,
                     help="TOML run card; keys are option names, sections only "
                          "organise the file; flags given on the command line "
                          "override it")
    cli, _ = pre.parse_known_args()

    cfg_argv = []
    if cli.config:
        with open(cli.config, "rb") as fh:
            cfg = tomllib.load(fh)
        for section in cfg.values():
            for key, val in section.items():
                if isinstance(val, bool):           # store_true options
                    if val:
                        cfg_argv.append(f"--{key}")
                elif isinstance(val, list):
                    cfg_argv += [f"--{key}"] + [str(v) for v in val]
                else:
                    cfg_argv += [f"--{key}", str(val)]

    ap = argparse.ArgumentParser(parents=[pre])
    ap.add_argument("--kshort", nargs="+", default=["kshort_kfparticle.root"],
                    help="K0s KFParticle nTuple(s) (.root); "
                         "several files and glob patterns are concatenated")
    ap.add_argument("--lam", nargs="+", default=["lambda_kfparticle.root"],
                    help="Lambda KFParticle nTuple(s) (.root); "
                         "several files and glob patterns are concatenated")
    ap.add_argument("--tree", default="DecayTree", help="TTree name in the ROOT files")
    ap.add_argument("--max_candidates", type=int, default=0,
                    help="use at most N candidates per species (0 = all), "
                         "random subsample, shared across files in proportion "
                         "to their entries")
    ap.add_argument("--subsample_seed", type=int, default=42)
    ap.add_argument("--outdir", default="calib_out")
    ap.add_argument("--lookup_pt_min", type=float, default=0.10,
                    help="lower pT [GeV] covered by kappa lookup and kappa-map diagnostics")
    ap.add_argument("--lookup_pt_max", type=float, default=5.0,
                    help="upper pT [GeV] covered by kappa lookup and kappa-map diagnostics")
    ap.add_argument("--kappa_slices", type=int, default=25,
                    help="number of curvature slices in fig6/fig7 kappa diagnostics")
    # stage 1
    ap.add_argument("--feature", choices=FEATURE_KINDS, default="inv_pt",
                    help="stage-1 momentum feature: inv_pt = 1/pT (default), "
                         "pt, log_pt. Information-equivalent; changes the "
                         "smoothness prior and which tail the numerical clamp "
                         "truncates.")
    ap.add_argument("--history_every", type=int, default=0,
                    help="stage-1 mass-history fit cadence in epochs "
                         "(0 = --refresh; use 1 for every epoch)")
    ap.add_argument("--epochs", type=int, default=400)
    ap.add_argument("--lr", type=float, default=2e-3)
    ap.add_argument("--hidden", type=int, default=48)
    ap.add_argument("--refresh", type=int, default=50, help="weight-refresh cadence")
    ap.add_argument("--lam_prior", type=float, default=0.05,
                    help="L2(eps,delta) weight at epoch 0; annealed linearly to 0")
    ap.add_argument("--lam_split", type=float, default=1.0)
    ap.add_argument("--lam_alpha", type=float, default=2.0)
    ap.add_argument("--loss_pt_binning", choices=("quantile", "log"), default="quantile",
                    help="daughter-pT loss-bin edges: quantile = equal population; "
                         "log = equal width in ln pT over [lookup_pt_min, lookup_pt_max]")
    ap.add_argument("--loss_pt_bins", type=int, default=6, help="number of daughter-pT loss bins")
    ap.add_argument("--chunk_size", type=int, default=500000,
                    help="candidates per gradient chunk; the full-batch gradient is "
                         "accumulated over chunks, so memory scales with this, not "
                         "with the sample size")
    ap.add_argument("--load_stage1", default=None,
                    help="existing model.pt (must carry the current convention "
                         "tag; older checkpoints are rejected)")
    # stage 2
    ap.add_argument("--no_reso", action="store_true", help="stage 1 only", default=False)
    ap.add_argument("--reso_model", choices=("absolute", "pull"), default="pull",
                    help="absolute: measure (a,b) fields from peak widths; "
                         "pull: calibrate the KFParticle covariance, "
                         "r = s(eta,phi)*pT^u_p*r_cov (needs Covariance branches)")
    ap.add_argument("--reso_epochs", type=int, default=300)
    ap.add_argument("--reso_lr", type=float, default=2e-3)
    ap.add_argument("--reso_hidden", type=int, default=32)
    ap.add_argument("--reso_layers", type=int, default=3)
    ap.add_argument("--c_per_resonance", action="store_true",
                    help="separate mass-width floor for K0s and Lambda")
    ap.add_argument("--reso_scalar_steps", type=int, default=1500,
                    help="reduced global-scalar fit steps (for Hessian errors)")
    # injection
    ap.add_argument("--inject", action="store_true",
                    help="append the go/no-go toy campaign after the data pass")
    ap.add_argument("--inject_only", action="store_true",
                    help="run the toy campaign and skip the data pass")
    ap.add_argument("--inject_n", type=int, default=200000)
    ap.add_argument("--inject_s0", type=float, default=1.30,
                    help="pull mode: injected covariance miscalibration factor")
    ap.add_argument("--inject_u0", type=float, default=0.0,
                    help="pull mode: injected pT exponent")
    ap.add_argument("--inject_a0", type=float, default=0.010,
                    help="absolute mode: injected MS-like coefficient")
    ap.add_argument("--inject_b0", type=float, default=0.012,
                    help="absolute mode: injected curvature coefficient [1/GeV]")
    ap.add_argument("--inject_c0", type=float, default=0.0015,
                    help="injected mass-width floor [GeV]")
    ap.add_argument("--inject_seed", type=int, default=1234)
    ap.add_argument("--inject_scalar_steps", type=int, default=1500)
    args = ap.parse_args(cfg_argv + sys.argv[1:])

    for key in ("kshort", "lam"):
        paths = []
        for pattern in getattr(args, key):
            matches = sorted(expand_glob(pattern))
            if not matches:
                sys.exit(f"--{key}: no file matches '{pattern}'")
            paths += matches
        setattr(args, key, paths)

    os.makedirs(args.outdir, exist_ok=True)
    K, L, meta = load(args)
    print(f"loaded {len(K['mass'])} K0s, {len(L['mass'])} Lambda candidates")

    if args.inject or args.inject_only:
        ok = run_injection(args, K, L, meta, os.path.join(args.outdir, "injection"))
        if not ok:
            print("[inject] FAILED -- do not deploy the resolution map. "
                  "Debug before running on data.")
        if args.inject_only:
            write_metadata(args)
            return

    # ---- stage 1: momentum scale
    if args.load_stage1:
        model = load_stage1(args.load_stage1, hidden=args.hidden)
        print(f"loaded stage-1 model from {args.load_stage1} "
              f"(convention '{model.tag()}' verified)")
        with torch.no_grad():
            mK, _, _ = corrected_mass(model, K)
            mL, _, _ = corrected_mass(model, L)
        _, _, pK = fit_peak(mK.numpy(), M_K0S, 0.010, 0.42, 0.58)
        _, _, pL = fit_peak(mL.numpy(), M_LAMBDA, 0.004, 1.09, 1.145)
        K["w"] = torch.tensor(signal_weights(mK.numpy(), pK))
        L["w"] = torch.tensor(signal_weights(mL.numpy(), pL))
    else:
        model, history = train(args, K, L, meta)
        plot_stage1_training_history(history, args.outdir)
        save_stage1(model, os.path.join(args.outdir, "model.pt"))

    export_lookup(model, meta, os.path.join(args.outdir, "kappa_lookup.csv"))
    diagnostics(model, K, L, meta, args.outdir)

    if args.no_reso:
        write_metadata(args)
        print(f"outputs written to {args.outdir}/  (stage 1 only)")
        return

    # ---- stage 2, KappaNet frozen
    for p in model.parameters():
        p.requires_grad_(False)
    model.eval()

    groups = build_groups(model, K, L, pure_signal=False)
    n_c = set_c_indices(groups, args.c_per_resonance)
    print(f"stage 2 ({args.reso_model}): {len(groups)} resonance groups, "
          f"{n_c} floor parameter(s)")
    net, glob = train_resolution(groups, args, n_c=n_c)
    scal = fit_global_reduced(groups, args.reso_model, n_c=n_c,
                              steps=args.reso_scalar_steps)

    torch.save({"reso_net": net.state_dict(), "globals": glob.state_dict(),
                "mode": args.reso_model, "n_c": n_c,
                "groups": [g["name"] for g in groups]},
               os.path.join(args.outdir, "reso_model.pt"))
    export_reso_lookup(net, glob, groups,
                       os.path.join(args.outdir, "reso_lookup.csv"), n_c=n_c)
    resolution_diagnostics(net, glob, groups, args.outdir, meta,
                           n_c=n_c, scalars=scal)

    write_metadata(args)
    print(f"outputs written to {args.outdir}/")


if __name__ == "__main__":
    main()
