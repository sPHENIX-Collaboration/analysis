"""Stage-1 diagnostic figures (fig1-fig7) and summary."""
import os
import numpy as np
import pandas as pd
import torch
import matplotlib.pyplot as plt

from .constants import ETA_RANGE, FEATURE_UNITS, KAPPA_GUARD, M_K0S, M_LAMBDA, STAGE1_HISTORY_SPECIES
from .kinematics import inv_mass
from .peakfit import fit_peak, fit_peak_with_error
from .stage1 import corrected_mass
from .export import _savefig, export_mass_histograms_root

def plot_mass_1d(K, L, mK, mL, outdir):
    """Figure 1: raw vs corrected 1D mass, K0s / Lambda / Lambdabar."""
    isl = L["is_lam"].numpy()
    fig, ax = plt.subplots(1, 3, figsize=(15, 4.2))
    for a, raw, cor, pdg, rng, t in (
        (ax[0], K["mass"].numpy(), mK, M_K0S, (0.42, 0.58), "K0s"),
        (ax[1], L["mass"].numpy()[isl], mL[isl], M_LAMBDA, (1.09, 1.145), "Lambda"),
        (ax[2], L["mass"].numpy()[~isl], mL[~isl], M_LAMBDA, (1.09, 1.145), "Lambdabar")):
        a.hist(raw, bins=80, range=rng, histtype="step", label="raw")
        a.hist(cor, bins=80, range=rng, histtype="step", label="corrected")
        a.axvline(pdg, ls="--", c="k", lw=0.8, label="PDG")
        a.set_title(t); a.set_xlabel("m [GeV]"); a.set_ylabel("candidates / bin")
        a.legend(fontsize=8)
    fig.suptitle("Invariant mass: raw vs corrected")
    _savefig(fig, outdir, "fig1_mass_1d")


def plot_mass_vs_kin_2d(K, L, mK, mL, outdir, kind, pt_plot):
    """Figures 2/3: 2D histogram of mass (y) vs candidate pT or eta (x)."""
    isl = L["is_lam"].numpy()
    if kind == "pt":
        xK, xL = K["cpt"].numpy(), L["cpt"].numpy()
        xrange, xlabel = (0.0, 1.3 * pt_plot[1]), r"candidate $p_T$ [GeV]"
        figname = "fig2_mass_vs_pt_2d"
    else:
        xK, xL = K["ceta"].numpy(), L["ceta"].numpy()
        xrange, xlabel = (-2.5, 2.5), "candidate eta"
        figname = "fig3_mass_vs_eta_2d"

    rows = [
        ("K0s",       xK,       K["mass"].numpy(),       mK,       (0.42, 0.58),  M_K0S),
        ("Lambda",    xL[isl],  L["mass"].numpy()[isl],  mL[isl],  (1.09, 1.145), M_LAMBDA),
        ("Lambdabar", xL[~isl], L["mass"].numpy()[~isl], mL[~isl], (1.09, 1.145), M_LAMBDA),
    ]
    fig, ax = plt.subplots(3, 2, figsize=(10, 12))
    for i, (name, x, mraw, mcor, mrange, pdg) in enumerate(rows):
        for j, (m, lab) in enumerate(((mraw, "raw"), (mcor, "corrected"))):
            a = ax[i, j]
            h = a.hist2d(x, m, bins=[60, 70], range=[xrange, mrange],
                        cmap="viridis", cmin=1)
            a.axhline(pdg, ls="--", c="w", lw=1.0)
            a.set_title(f"{name} ({lab})")
            a.set_xlabel(xlabel); a.set_ylabel("m [GeV]")
            cb = fig.colorbar(h[3], ax=a)
            cb.set_label("candidates / bin")
    fig.suptitle(f"Invariant mass vs candidate {kind}: raw vs corrected")
    _savefig(fig, outdir, figname)


def plot_alpha_vs_eta_1d(K, k1, k2, outdir):
    """Figure 4: signal-weighted <alpha> vs candidate eta, raw vs corrected."""
    p1r, p2r = K["pt1"].numpy(), K["pt2"].numpy()
    p1c, p2c = (k1 * K["pt1"]).numpy(), (k2 * K["pt2"]).numpy()
    eta_c = K["ceta"].numpy(); w = K["w"].numpy()
    eb = np.linspace(*ETA_RANGE, 13)
    fig, ax = plt.subplots(figsize=(6, 4.5))
    for lab, a1, a2 in (("raw", p1r, p2r), ("corrected", p1c, p2c)):
        al = (a1 - a2) / (a1 + a2)
        num, _ = np.histogram(eta_c, eb, weights=w * al)
        den, _ = np.histogram(eta_c, eb, weights=w)
        ax.plot(0.5 * (eb[:-1] + eb[1:]), num / np.maximum(den, 1e-9), "o-", label=lab)
    ax.axhline(0, c="k", lw=0.8)
    ax.set_xlabel("K0s candidate eta")
    ax.set_ylabel(r"$\langle\alpha\rangle$ (signal-weighted)")
    ax.set_title(r"K0s daughter $p_T$ asymmetry vs eta")
    ax.legend()
    _savefig(fig, outdir, "fig4_alpha_vs_eta_1d")


def plot_alpha_vs_mass_2d(K, mK, k1, k2, outdir):
    """Figure 5: daughter pT asymmetry alpha (x) vs K0s mass (y)."""
    p1r, p2r = K["pt1"].numpy(), K["pt2"].numpy()
    p1c, p2c = (k1 * K["pt1"]).numpy(), (k2 * K["pt2"]).numpy()
    alpha_raw = (p1r - p2r) / (p1r + p2r)
    alpha_cor = (p1c - p2c) / (p1c + p2c)
    mraw = K["mass"].numpy()
    arange, mrange = (-1, 1), (0.4, 0.6)
    fig, ax = plt.subplots(1, 2, figsize=(11, 4.6))
    for a, al, m, lab in ((ax[0], alpha_raw, mraw, "raw"),
                          (ax[1], alpha_cor, mK, "corrected")):
        h = a.hist2d(al, m, bins=[100, 100], range=[arange, mrange],
                    cmap="viridis", cmin=1)
        a.axhline(M_K0S, ls="--", c="w", lw=1.0)
        a.axvline(0.0, ls=":", c="w", lw=0.8)
        a.set_title(f"K0s ({lab})")
        a.set_xlabel(r"$\alpha=(p_T^{\pi^-}-p_T^{\pi^+})/(p_T^{\pi^-}+p_T^{\pi^+})$")
        a.set_ylabel("m [GeV]")
        cb = fig.colorbar(h[3], ax=a)
        cb.set_label("candidates / bin")
    fig.suptitle("K0s daughter pT asymmetry vs invariant mass")
    _savefig(fig, outdir, "fig5_alpha_vs_mass_2d")


def _slice_title(u):
    return f"1/pT = {u:.2f} GeV$^{{-1}}$ (pT = {1.0/u:.2f} GeV)"


def _plot_map_grid(maps, curv_slices, outdir, name, suptitle, cmap, vmin, vmax,
                   colorbar_label, title_prefix=""):
    ee = np.linspace(*ETA_RANGE, 50); pp = np.linspace(-np.pi, np.pi, 50)
    n = len(curv_slices); ncols = int(np.ceil(np.sqrt(n)));
    nrows = int(np.ceil(n / ncols))
    fig, ax = plt.subplots(nrows, ncols, figsize=(4.6 * ncols, 4.1 * nrows))
    ax = np.atleast_2d(ax)
    for idx, u in enumerate(curv_slices):
        r, c = divmod(idx, ncols)
        a = ax[r, c]
        im = a.pcolormesh(ee, pp, maps[idx].T, shading="auto", cmap=cmap,
                          vmin=vmin, vmax=vmax)
        a.set_title(title_prefix + _slice_title(u), fontsize=9)
        a.set_xlabel("eta"); a.set_ylabel("phi")
        cb = fig.colorbar(im, ax=a)
        cb.set_label(colorbar_label)
    for idx in range(n, nrows * ncols):
        r, c = divmod(idx, ncols); ax[r, c].axis("off")
    fig.suptitle(suptitle)
    _savefig(fig, outdir, name)


def plot_kappa_maps_vs_curv(model, outdir, curv_slices):
    """Figure 6: charge-even (eps) and charge-odd (delta) curvature-scale maps,
    sliced at curv_slices = 1/pT_slices (pT_slices from semi_even_pt_grid),
    each panel labelled with both 1/pT and the equivalent pT.  These are the exact even/odd fields of the convention
    (1/pT)_corr = (1/pT)(1 + eps + q*delta), read directly off the network."""
    ee = np.linspace(*ETA_RANGE, 50); pp = np.linspace(-np.pi, np.pi, 50)
    E, P = np.meshgrid(ee, pp, indexing="ij")
    t = lambda a: torch.tensor(a.ravel(), dtype=torch.float64)
    evens, odds = [], []
    with torch.no_grad():
        for u in curv_slices:
            eps, dlt = model.eps_delta(t(np.full(E.size, 1.0 / u)), t(E), t(P))
            evens.append(eps.numpy().reshape(E.shape))
            odds.append(dlt.numpy().reshape(E.shape))
    vmax_e = max(1e-4, max(np.abs(z).max() for z in evens))
    vmax_o = max(1e-4, max(np.abs(z).max() for z in odds))

    _plot_map_grid(evens, curv_slices, outdir, "fig6a_kappa_even_maps_vs_pt",
                   "Charge-even curvature-scale maps across pT",
                   "RdBu_r", -vmax_e, vmax_e,
                   r"$\epsilon$  (charge-even fractional $1/p_T$ scale)")
    _plot_map_grid(odds, curv_slices, outdir, "fig6b_kappa_odd_maps_vs_pt",
                   "Charge-odd curvature-scale maps across pT",
                   "RdBu_r", -vmax_o, vmax_o,
                   r"$\delta$  (charge-odd fractional $1/p_T$ scale)")


def plot_kappa_final_vs_curv(model, outdir, curv_slices):
    """Figure 7: deployed pT multiplier kappa_q = 1/(1+eps+q*delta) per charge,
    sliced at curv_slices as in Figure 6, shared color scale."""
    ee = np.linspace(*ETA_RANGE, 50); pp = np.linspace(-np.pi, np.pi, 50)
    E, P = np.meshgrid(ee, pp, indexing="ij")
    t = lambda a: torch.tensor(a.ravel(), dtype=torch.float64)
    kp_maps, km_maps = [], []
    with torch.no_grad():
        for u in curv_slices:
            kp, _, _ = model.kappa(t(np.full(E.size, 1.0 / u)), t(E), t(P),
                                   torch.ones(E.size, dtype=torch.float64))
            km, _, _ = model.kappa(t(np.full(E.size, 1.0 / u)), t(E), t(P),
                                   -torch.ones(E.size, dtype=torch.float64))
            kp_maps.append(kp.numpy().reshape(E.shape))
            km_maps.append(km.numpy().reshape(E.shape))
    dev = max(1e-4, max(np.abs(z - 1.0).max() for z in kp_maps + km_maps))

    _plot_map_grid(kp_maps, curv_slices, outdir, "fig7a_kappa_qplus_maps_vs_pt",
                   r"Final $p_T$ scale correction for positive tracks",
                   "RdBu_r", 1 - dev, 1 + dev,
                   r"$\kappa(q{=}{+}1)$  final $p_T$ multiplier",
                   title_prefix="q = +1, ")
    _plot_map_grid(km_maps, curv_slices, outdir, "fig7b_kappa_qminus_maps_vs_pt",
                   r"Final $p_T$ scale correction for negative tracks",
                   "RdBu_r", 1 - dev, 1 + dev,
                   r"$\kappa(q{=}{-}1)$  final $p_T$ multiplier",
                   title_prefix="q = -1, ")


def plot_stage1_training_history(history, outdir):
    """Stage-1 training trace: loss plus fitted mass mean/width vs epoch."""
    if not history:
        return
    df = pd.DataFrame(history).sort_values("epoch")
    csv_path = os.path.join(outdir, "stage1_training_history.csv")
    df.to_csv(csv_path, index=False)

    colors = {
        "kshort": "tab:blue",
        "lambda": "tab:orange",
        "anti_lambda": "tab:green",
    }
    fig, ax = plt.subplots(3, 1, figsize=(8.5, 9.0), sharex=True)

    loss = df["loss"].to_numpy(dtype=float)
    ep = df["epoch"].to_numpy(dtype=float)
    good_loss = np.isfinite(loss)
    ax[0].plot(ep[good_loss], loss[good_loss], lw=1.5, color="k")
    ax[0].set_ylabel("training loss")
    if np.all(loss[good_loss] > 0):
        ax[0].set_yscale("log")
    ax[0].grid(True, alpha=0.25)

    ax[1].axhline(0.0, color="0.35", lw=0.8, ls="--")
    for key, label, pdg, _, _ in STAGE1_HISTORY_SPECIES:
        mean = df[f"{key}_mean_GeV"].to_numpy(dtype=float)
        good = np.isfinite(mean)
        ax[1].plot(ep[good], (mean[good] - pdg) * 1e3, marker="o", ms=3,
                   lw=1.2, color=colors[key], label=label)
    ax[1].set_ylabel("fitted mean - PDG [MeV]")
    ax[1].grid(True, alpha=0.25)
    ax[1].legend(fontsize=8, ncols=3)

    for key, label, _, _, _ in STAGE1_HISTORY_SPECIES:
        width = df[f"{key}_width_GeV"].to_numpy(dtype=float)
        good = np.isfinite(width)
        ax[2].plot(ep[good], width[good] * 1e3, marker="o", ms=3,
                   lw=1.2, color=colors[key], label=label)
    ax[2].set_xlabel("training epoch")
    ax[2].set_ylabel("fitted width [MeV]")
    ax[2].grid(True, alpha=0.25)

    fig.suptitle("Stage-1 training history")
    _savefig(fig, outdir, "stage1_training_history")


def plot_k0s_peak_vs_candidate_kinematics(K, mK_corr, outdir, mK_raw_recalc=None,
                                          min_candidates=200):
    """Quick stage-1 closure plot in bins of original K0s candidate kinematics.

    The 'before' mass is recomputed from the original daughter (pT, eta, phi),
    and the 'after' mass uses the same daughters with only pT -> kappa*pT.
    Both curves therefore use exactly the same candidates and the same original
    KFParticle candidate pT/eta/phi bin assignment.  This intentionally does
    not rerun KFParticle; the production-level validation must still apply the
    correction upstream of KFParticle and reconstruct the candidates again.
    """
    if mK_raw_recalc is None:
        with torch.no_grad():
            mK_raw_recalc = inv_mass(
                K["pt1"], K["eta1"], K["phi1"], K["m1"],
                K["pt2"], K["eta2"], K["phi2"], K["m2"]
            ).numpy()

    m_raw = np.asarray(mK_raw_recalc)
    m_cor = np.asarray(mK_corr)
    cpt = K["cpt"].numpy()
    ceta = K["ceta"].numpy()
    cphi = K["cphi"].numpy()

    scans = [
        (cpt,
         np.array([0.0, 0.35, 0.45, 0.55, 0.65, 0.75, 0.85, 0.95, 1.0, 1.25, 1.5, 1.75, 2.0, 2.5, 3.0, 5.0]),
         r"$K_S^0$ candidate $p_T$ [GeV]"),
        (ceta, np.linspace(-1.5, 1.5, 13), r"$K_S^0$ candidate $\eta$"),
        (cphi, np.linspace(-np.pi, np.pi, 13), r"$K_S^0$ candidate $\phi$ [rad.]"),
    ]

    results = []
    for x, edges, xlabel in scans:
        xc = 0.5 * (edges[:-1] + edges[1:])
        ex = 0.5 * (edges[1:] - edges[:-1])
        vals = {k: [] for k in ("mu_raw", "sig_raw", "emu_raw", "esig_raw", "cov_raw",
                                "mu_cor", "sig_cor", "emu_cor", "esig_cor", "cov_cor")}

        for ib, (lo, hi) in enumerate(zip(edges[:-1], edges[1:])):
            # Include the upper boundary in the final bin only.
            sel = (x >= lo) & ((x <= hi) if ib == len(edges) - 2 else (x < hi))
            if np.count_nonzero(sel) < min_candidates:
                rr = (float("nan"),) * 5
                cc = (float("nan"),) * 5
            else:
                rr = fit_peak_with_error(m_raw[sel], M_K0S, 0.010, 0.42, 0.58)
                cc = fit_peak_with_error(m_cor[sel], M_K0S, 0.010, 0.42, 0.58)

            for key, value in zip(("mu_raw", "sig_raw", "emu_raw", "esig_raw", "cov_raw"), rr):
                vals[key].append(value)
            for key, value in zip(("mu_cor", "sig_cor", "emu_cor", "esig_cor", "cov_cor"), cc):
                vals[key].append(value)

        result = dict(x=xc, ex=ex, xlabel=xlabel)
        result.update({k: np.asarray(v, dtype=float) for k, v in vals.items()})
        results.append(result)

    # Fitted mass peak position.
    fig, ax = plt.subplots(1, 3, figsize=(15, 4.5), sharey=True)
    for a, r in zip(ax, results):
        a.errorbar(r["x"], r["mu_raw"], xerr=r["ex"], yerr=r["emu_raw"],
                   fmt="o-", ms=3, lw=1, color="C0", label="Before correction")
        a.errorbar(r["x"], r["mu_cor"], xerr=r["ex"], yerr=r["emu_cor"],
                   fmt="s-", ms=3, lw=1, color="C3", label="After correction")
        a.axhline(M_K0S, ls="--", c="k", lw=0.9, label="PDG mass")
        a.set_xlabel(r["xlabel"])
    ax[0].set_ylabel(r"Fitted $K_S^0$ mass $\mu_{\rm fit}$ [GeV]")
    ax[1].legend(fontsize=8)
    fig.suptitle(r"Stage-1 quick closure: fitted $K_S^0$ mass vs candidate kinematics")
    _savefig(fig, outdir, "stage1_k0s_fitted_mass_vs_candidate_kin")

    # Fitted relative mass width.  Propagate the mu-sigma covariance from the
    # same slice fit into sigma/mu.
    fig, ax = plt.subplots(1, 3, figsize=(15, 4.5), sharey=True)
    for a, r in zip(ax, results):
        for prefix, fmt, color, label in (("raw", "o-", "C0", "Before correction"),
                                          ("cor", "s-", "C3", "After correction")):
            mu = r[f"mu_{prefix}"]
            sig = r[f"sig_{prefix}"]
            emu = r[f"emu_{prefix}"]
            esig = r[f"esig_{prefix}"]
            cov = r[f"cov_{prefix}"]
            ratio = 100.0 * sig / mu
            dmu = -100.0 * sig / (mu * mu)
            dsig = 100.0 / mu
            var = dmu * dmu * emu * emu + dsig * dsig * esig * esig \
                  + 2.0 * dmu * dsig * cov
            eratio = np.sqrt(np.maximum(var, 0.0))
            a.errorbar(r["x"], ratio, xerr=r["ex"], yerr=eratio,
                       fmt=fmt, ms=3, lw=1, color=color, label=label)
        a.set_xlabel(r["xlabel"])
    ax[0].set_ylabel(r"$K_S^0$ mass resolution $\sigma_{\rm fit}/\mu_{\rm fit}$ [%]")
    ax[1].legend(fontsize=8)
    fig.suptitle(r"Stage-1 quick closure: fitted $K_S^0$ mass width vs candidate kinematics")
    _savefig(fig, outdir, "stage1_k0s_resolution_vs_candidate_kin")


# ---------------------------------------------------------------- diagnostics
def diagnostics(model, K, L, meta, outdir):
    with torch.no_grad():
        mK, _, _ = corrected_mass(model, K)
        mL, _, _ = corrected_mass(model, L)
        k1, e1, d1 = model.kappa(K["pt1"], K["eta1"], K["phi1"], K["q1"])
        k2, e2, d2 = model.kappa(K["pt2"], K["eta2"], K["phi2"], K["q2"])
    mK, mL = mK.numpy(), mL.numpy()
    isl = L["is_lam"].numpy()

    # Controlled before/after mass pair for the quick stage-1 closure plots:
    # same daughters and same original candidate bins, with only pT rescaled.
    with torch.no_grad():
        mK_raw_recalc = inv_mass(
            K["pt1"], K["eta1"], K["phi1"], K["m1"],
            K["pt2"], K["eta2"], K["phi2"], K["m2"]
        ).numpy()
    dm_raw_mev = 1e3 * (mK_raw_recalc - K["mass"].numpy())
    dm_raw_med = float(np.median(dm_raw_mev))
    dm_raw_rms = float(np.sqrt(np.mean(dm_raw_mev**2)))

    lines = []
    lines.append(f"{'K0s mass':10s} daughter-recomputed raw minus stored KFParticle mass: "
                 f"median {dm_raw_med:+.4f} MeV, RMS {dm_raw_rms:.4f} MeV")
    lines.append(f"{'quick plot':10s} before/after uses daughter-recomputed mass with identical "
                 f"original KFParticle candidate pT/eta/phi bins; KFParticle is not rerun")
    for name, raw, cor, pdg, lo, hi, s0 in (
        ("K0s",    K["mass"].numpy(), mK, M_K0S,    0.42, 0.58, 0.010),
        ("Lambda", L["mass"].numpy()[isl],  mL[isl],  M_LAMBDA, 1.09, 1.145, 0.004),
        ("Lambdabar", L["mass"].numpy()[~isl], mL[~isl], M_LAMBDA, 1.09, 1.145, 0.004)):
        mu_r, s_r, _ = fit_peak(raw, pdg, s0, lo, hi)
        mu_c, s_c, _ = fit_peak(cor, pdg, s0, lo, hi)
        lines.append(f"{name:10s} raw peak {mu_r*1e3:9.3f} MeV  ->  corrected "
                     f"{mu_c*1e3:9.3f} MeV   (PDG {pdg*1e3:.3f}, "
                     f"pull {(mu_c-pdg)*1e3:+.3f} MeV)")
    e_all = np.concatenate([e1.numpy(), e2.numpy()])
    d_all = np.concatenate([d1.numpy(), d2.numpy()])
    kc = np.concatenate([(1.0 + e1 + K["q1"] * d1).numpy(),
                         (1.0 + e2 + K["q2"] * d2).numpy()])
    lines.append(f"{'fields':10s} eps mean {e_all.mean():+.5f} rms {e_all.std():.5f} "
                 f"max|.| {np.abs(e_all).max():.5f};  "
                 f"delta mean {d_all.mean():+.5f} rms {d_all.std():.5f} "
                 f"max|.| {np.abs(d_all).max():.5f}   (uncapped)")
    guard_frac = np.mean(kc <= KAPPA_GUARD + 1e-12)
    lines.append(f"{'guard':10s} kappa_curv <= {KAPPA_GUARD} fraction = "
                 f"{guard_frac:.2e}  (numerical guard, must be 0)")
    if guard_frac > 0:
        lines.append(f"{'WARNING':10s} numerical guard active on data -- the "
                     f"converged model is not trustworthy; investigate.")
    all_pt = torch.cat([K["pt1"], K["pt2"], L["pt1"], L["pt2"]])
    uc = model.clamp_frac(all_pt)
    lines.append(f"{'feature':10s} x = {model.kind}, x_ref = {model.x_ref:.4f} "
                 f"{FEATURE_UNITS[model.kind]}, x_scale = {model.x_scale:.4f}; "
                 f"|x_n| >= {model.x_clamp:.1f} clamp fraction = {uc:.2e} "
                 f"(kappa constant in x beyond the clamp)")
    txt = "\n".join(lines)
    print(txt)
    with open(os.path.join(outdir, "summary.txt"), "w") as f:
        f.write(txt + "\n")

    export_mass_histograms_root(K, L, mK, mL, outdir)
    plot_mass_1d(K, L, mK, mL, outdir)
    plot_k0s_peak_vs_candidate_kinematics(K, mK, outdir,
                                          mK_raw_recalc=mK_raw_recalc)
    plot_mass_vs_kin_2d(K, L, mK, mL, outdir, "pt", meta["pt_plot"])
    plot_mass_vs_kin_2d(K, L, mK, mL, outdir, "eta", meta["pt_plot"])
    plot_alpha_vs_eta_1d(K, k1, k2, outdir)
    plot_alpha_vs_mass_2d(K, mK, k1, k2, outdir)
    plot_kappa_maps_vs_curv(model, outdir, meta["curv_slices"])
    plot_kappa_final_vs_curv(model, outdir, meta["curv_slices"])
