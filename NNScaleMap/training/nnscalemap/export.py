"""Outputs: kappa/reso lookup CSVs, run metadata (TOML), stage-1 mass histograms (ROOT), figure saving."""
import hashlib
import json
import os
import shlex
import sys
from datetime import datetime
import numpy as np
import torch
import matplotlib.pyplot as plt

from .constants import ETA_RANGE, FEATURE_UNITS

# ---------------------------------------------------------------- outputs
def export_lookup(model, args, path):
    """kappa is the pT MULTIPLIER (pT_corr = kappa * pT); the grid is
    --lookup_pt_values x --lookup_n_eta x --lookup_n_phi points, and both pT
    and 1/pT columns are exported.  Normalisation constants are stamped so the
    map is reproducible at deployment time."""
    pts = np.asarray(args.lookup_pt_values, dtype=float)
    curvs = 1.0 / pts
    etas = np.linspace(*ETA_RANGE, args.lookup_n_eta)
    phis = np.linspace(-np.pi, np.pi, args.lookup_n_phi)
    rows = []
    with torch.no_grad():
        for q in (-1.0, 1.0):
            U, E, Fm = np.meshgrid(curvs, etas, phis, indexing="ij")
            P = 1.0 / U
            t = lambda a: torch.tensor(a.ravel(), dtype=torch.float64)
            kap, eps, dlt = model.kappa(t(P), t(E), t(Fm),
                                        torch.full((P.size,), q, dtype=torch.float64))
            rows.append(np.column_stack([np.full(P.size, q), P.ravel(), U.ravel(),
                                         E.ravel(), Fm.ravel(), kap.numpy(),
                                         (1.0 / kap).numpy(),
                                         eps.numpy(), dlt.numpy()]))
    with open(path, "w") as fh:
        fh.write("# convention: (1/pT)_corr = (1/pT) * kappa_curv, "
                 "kappa_curv = 1 + eps + q*delta (uncapped)\n")
        fh.write("# kappa is the pT multiplier consumed downstream: "
                 "pT_corr = kappa * pT = pT / kappa_curv\n")
        fh.write(f"# feature: x = {model.kind}; x_ref = {model.x_ref:.6f} "
                 f"{FEATURE_UNITS[model.kind]}, x_scale = {model.x_scale:.6f}, "
                 f"x_clamp = {model.x_clamp:.1f} (kappa constant in x "
                 f"outside the clamp)\n")
        fh.write(f"# convention tag: {model.tag()}\n")
        fh.write("# CAVEAT: trained on vertex-constrained daughter kinematics; "
                 "deployed on raw SvtxTrackMap tracks (calibration/deployment "
                 "mismatch is documented in the analysis note)\n")
        fh.write("q,pT,curv,eta,phi,kappa,kappa_curv,eps,delta\n")
        np.savetxt(fh, np.vstack(rows), delimiter=",", fmt="%.6f")


PULL_INTERPRETATION = [
    "PULL MODEL: s and u_p are a CALIBRATION of the covariance-reported",
    "resolution, r = s(eta,phi) * pT^{u_p} * r_cov -- not a measurement of the",
    "resolution itself. s==1, u_p==0 means the tracker's errors are correct.",
    "s mixes (i) resolution missed by the Kalman hit-error/material model and",
    "(ii) covariance mis-estimation; nothing here separates them.",
    "The stored covariance is POST-VERTEX-CONSTRAINT: s is defined relative to",
    "constrained covariances and must NOT be applied to raw SvtxTrack",
    "covariances (unmeasured constraint-shrinkage factor).",
    "s at low pT is partially degenerate with the floor c: see rho(s,c).",
    "No r grid is exported: r depends on the per-track covariance; consumers",
    "rebuild r = s(eta,phi) * pT^{u_p} * r_cov from their own covariance.",
]


def export_reso_lookup(net, glob, groups, path, n_c=1):
    """absolute: (eta, phi, a, b) fields.  pull: (eta, phi, s) plus u_p in the
    header.  Never an evaluated r grid (species/covariance dependence)."""
    etas = np.linspace(*ETA_RANGE, 25)
    phis = np.linspace(-np.pi, np.pi, 25)
    E, P = np.meshgrid(etas, phis, indexing="ij")
    t = lambda x: torch.tensor(x.ravel(), dtype=torch.float64)
    hdr = []
    with torch.no_grad():
        if net.mode == "absolute":
            a, b = net.ab(t(E), t(P))
            cols = np.column_stack([E.ravel(), P.ravel(), a.numpy(), b.numpy()])
            colhdr = "eta,phi,a,b"
            hdr.append("model: r^2 = (a(eta,phi)/beta)^2 + (b(eta,phi)*pT)^2 ; "
                       "sigma_m^2 = J1^2 r1^2 + J2^2 r2^2 + c^2")
            hdr.append("a dimensionless (enters as a/beta); b in 1/GeV; c in GeV")
        else:
            s = net.s(t(E), t(P))
            cols = np.column_stack([E.ravel(), P.ravel(), s.numpy()])
            colhdr = "eta,phi,s"
            hdr.append("model: r = s(eta,phi) * pT^{u_p} * r_cov ; "
                       "sigma_m^2 = J1^2 r1^2 + J2^2 r2^2 + c^2")
            hdr.append(f"u_p = {float(glob.u_p()):+.6f}")
            hdr.extend(PULL_INTERPRETATION)
        for i in range(n_c):
            hdr.append(f"c[{i}] = {float(glob.c(i)):.6e} GeV")
        for gi, g in enumerate(groups):
            hdr.append(f"tail {g['name']}: f = {float(glob.f(gi)):.4f}, "
                       f"k = {float(glob.k(gi)):.4f}")
    with open(path, "w") as fh:
        for h in hdr:
            fh.write("# " + h + "\n")
        fh.write(colhdr + "\n")
        np.savetxt(fh, cols, delimiter=",", fmt="%.6e")


def sha256sum(path):
    """sha256 hex digest of a file, read in 16 MB blocks."""
    sha = hashlib.sha256()
    with open(path, "rb") as fh:
        while block := fh.read(16 << 20):
            sha.update(block)
    return sha.hexdigest()


def write_metadata(args):
    """Read-only <outdir>/run_metadata.toml: command, input files with sha256
    checksums, and every resolved option (unset ones as comments)."""
    lines = ["# NNScaleMap training metadata, written when the run ends",
             "", "[run]",
             f"finished = {json.dumps(datetime.now().isoformat(timespec='seconds'))}",
             f"command = {json.dumps(shlex.join(sys.argv))}"]
    for species, paths in (("K0s", args.kshort), ("Lambda", args.lam)):
        for p in paths:
            lines += ["", "[[inputs]]",
                      f"species = {json.dumps(species)}",
                      f"path = {json.dumps(os.path.abspath(p))}",
                      f"size_bytes = {os.path.getsize(p)}",
                      f"sha256 = {json.dumps(sha256sum(p))}"]
    lines += ["", "[options]"]
    for key, val in vars(args).items():
        if val is None:
            lines.append(f"# {key} = (not set)")
        else:
            lines.append(f"{key} = {json.dumps(val)}")

    path = os.path.join(args.outdir, "run_metadata.toml")
    if os.path.exists(path):                    # read-only copy from an earlier run
        os.remove(path)
    with open(path, "w") as fh:
        fh.write("\n".join(lines) + "\n")
    os.chmod(path, 0o444)
    print(f"wrote run metadata to {path}")


def _savefig(fig, outdir, name):
    fig.tight_layout()
    fig.savefig(os.path.join(outdir, name + ".png"), dpi=130)
    fig.savefig(os.path.join(outdir, name + ".pdf"))
    plt.close(fig)


def export_mass_histograms_root(K, L, mK, mL, outdir):
    """Write ROOT TH1D mass spectra before/after stage-1 correction."""
    import uproot

    isl = L["is_lam"].numpy()
    specs = [
        ("kshort",      K["mass"].numpy(),        mK,       (0.45, 0.55)),
        ("lambda",      L["mass"].numpy()[isl],   mL[isl],  (1.09, 1.15)),
        ("anti_lambda", L["mass"].numpy()[~isl],  mL[~isl], (1.09, 1.15)),
    ]
    path = os.path.join(outdir, "stage1_mass_histograms.root")
    with uproot.recreate(path) as f:
        for name, raw, cor, rng in specs:
            edges = np.linspace(rng[0], rng[1], 51)
            h_raw, _ = np.histogram(raw, bins=edges)
            h_cor, _ = np.histogram(cor, bins=edges)
            f[f"{name}_mass_raw"] = (h_raw.astype(np.float64), edges)
            f[f"{name}_mass_corrected"] = (h_cor.astype(np.float64), edges)
    print(f"wrote stage-1 raw/corrected mass histograms to {path}")
