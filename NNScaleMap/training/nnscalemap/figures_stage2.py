"""Stage-2 diagnostic figures (fig8-fig13) and summary."""
import math
import os
import numpy as np
import torch
from scipy.optimize import curve_fit
import matplotlib.pyplot as plt

from .constants import ETA_RANGE, PION_MASS, PROTON_MASS, SIG0
from .kinematics import beta_of
from .peakfit import fit_peak
from .stage2 import pull_r, sigma_mass_net, sigma_mass_pull
from .export import PULL_INTERPRETATION, _savefig
from .figures_stage1 import _slice_title

# ---------------------------------------------------------------- stage-2 figs
def core_width(x, w, rng=(-6, 6), nbins=120, fit_range=3.0, min_w=200.0):
    """Weighted Gaussian-core width of a pull distribution (tail-insensitive).

    Mildly biased high (tail leakage under |pull| < fit_range): a perfectly
    closing fit reads ~2-4% above 1 for f ~ 0.02, k ~ 2.  Interpret residual
    structure vs pT/eta/phi, not the absolute offset.
    """
    if np.sum(w) < min_w:
        return np.nan, np.nan
    h, edges = np.histogram(x, bins=nbins, range=rng, weights=w)
    c = 0.5 * (edges[:-1] + edges[1:])
    sel = np.abs(c) < fit_range
    g = lambda t, N, mu, s, b0: N * np.exp(-0.5 * ((t - mu) / s)**2) + b0
    try:
        popt, _ = curve_fit(g, c[sel], h[sel],
                            p0=[h[sel].max(), 0.0, 1.0, 0.0], maxfev=20000)
        s = abs(popt[2])
        if not (0.1 < s < 5.0) or not (-2 < popt[1] < 2):
            raise RuntimeError
    except Exception:
        return np.nan, np.nan
    return popt[1], abs(popt[2])


def weighted_rms(x, w):
    mu = np.sum(w * x) / np.sum(w)
    return math.sqrt(np.sum(w * (x - mu)**2) / np.sum(w))


def _quantile_edges(x, w, n):
    q = np.linspace(0, 1, n + 1)
    return np.unique(np.quantile(x, q))


def plot_pull_closure(groups, net, glob, outdir):
    """Figure 8 (primary go/no-go): pull = (m - M_PDG) / sigma_m.

    In pull mode a RAW-COVARIANCE overlay is added: the pull computed with
    s = 1, u_p = 0 and no floor -- its core width is the single number saying
    how wrong the shipped covariances are (subject to the SV-constraint
    caveat).  Deviations of the calibrated core from 1 are a mis-modelled
    resolution field, NOT a scale problem (mu is fixed to PDG; fig1).
    """
    pulls, ws, cs, raws = {}, {}, {}, {}
    with torch.no_grad():
        for gi, g in enumerate(groups):
            sig = sigma_mass_net(g["R"], net, glob, glob.c(g["c_idx"]))
            pulls[g["name"]] = ((g["R"]["m"] - g["M"]) / sig).numpy()
            ws[g["name"]] = g["w"].numpy()
            cs[g["name"]] = (g["R"]["cpt"].numpy(), g["R"]["ceta"].numpy(),
                             g["R"]["cphi"].numpy())
            if net.mode == "pull":
                one = torch.ones_like(g["R"]["m"])
                sig0 = sigma_mass_pull(g["R"], one, one,
                                       torch.tensor(0.0, dtype=torch.float64),
                                       torch.tensor(0.0, dtype=torch.float64))
                raws[g["name"]] = ((g["R"]["m"] - g["M"]) / sig0).numpy()

    fig, ax = plt.subplots(2, 3, figsize=(15.5, 9))
    raw_cores = {}
    for i, g in enumerate(groups):
        n = g["name"]
        a = ax[0, i]
        h, edges, _ = a.hist(pulls[n], bins=120, range=(-6, 6), weights=ws[n],
                             histtype="step", color="C0", label="calibrated")
        if n in raws:
            a.hist(raws[n], bins=120, range=(-6, 6), weights=ws[n],
                   histtype="step", color="C2", ls="--",
                   label="raw covariance (s=1, u_p=0, no floor)")
            raw_cores[n] = core_width(raws[n], ws[n])[1]
        mu, s = core_width(pulls[n], ws[n])
        if np.isfinite(s):
            c = 0.5 * (edges[:-1] + edges[1:])
            amp = h[np.argmin(np.abs(c - mu))]
            a.plot(c, amp * np.exp(-0.5 * ((c - mu) / s)**2), "r-", lw=1.2,
                   label=f"core fit  $\\sigma$={s:.3f}")
        rms = weighted_rms(pulls[n], ws[n])
        a.axvline(0, c="k", lw=0.7)
        ttl = f"{n}: core {s:.3f}, RMS {rms:.3f}"
        if n in raw_cores and np.isfinite(raw_cores[n]):
            ttl += f", raw-cov core {raw_cores[n]:.3f}"
        a.set_title(ttl, fontsize=9)
        a.set_xlabel(r"pull $=(m-M_{PDG})/\sigma_m$")
        a.set_ylabel("weighted candidates / bin")
        a.legend(fontsize=7)

    for j, (lab, ix, rng) in enumerate((("candidate $p_T$ [GeV]", 0, None),
                                        ("candidate $\\eta$", 1, ETA_RANGE),
                                        ("candidate $\\phi$", 2, (-np.pi, np.pi)))):
        a = ax[1, j]
        for g in groups:
            n = g["name"]
            x = cs[n][ix]
            edges = _quantile_edges(x, ws[n], 10) if rng is None else np.linspace(*rng, 11)
            xs, ss = [], []
            for lo, hi in zip(edges[:-1], edges[1:]):
                sel = (x >= lo) & (x < hi)
                if sel.sum() < 50:
                    continue
                _, s = core_width(pulls[n][sel], ws[n][sel])
                xs.append(0.5 * (lo + hi)); ss.append(s)
            a.plot(xs, ss, "o-", ms=3, label=n)
        a.axhline(1.0, c="k", ls="--", lw=0.8)
        a.set_ylim(0.6, 1.6)
        a.set_xlabel(lab)
        a.set_ylabel("core pull width")
        a.legend(fontsize=8)
    fig.suptitle("Pull closure: core width must be 1.0, globally and in every bin")
    _savefig(fig, outdir, "fig8_pull_closure")
    out = {g["name"]: (core_width(pulls[g["name"]], ws[g["name"]])[1],
                       weighted_rms(pulls[g["name"]], ws[g["name"]]))
           for g in groups}
    return out, raw_cores


def plot_width_overlay(groups, net, glob, outdir):
    """Figure 9: fitted Gaussian core width of the corrected mass peak vs the
    predicted median sigma_m, per (candidate pT, eta) bin.  fig8 is the
    unbiased test (per-bin mixture over sigma_m biases this one slightly)."""
    fig, ax = plt.subplots(3, 2, figsize=(11, 12))
    with torch.no_grad():
        for i, g in enumerate(groups):
            n = g["name"]
            sig = sigma_mass_net(g["R"], net, glob, glob.c(g["c_idx"])).numpy()
            m = g["R"]["m"].numpy(); w = g["w"].numpy()
            for j, (xv, lab, rng) in enumerate(
                    ((g["R"]["cpt"].numpy(), r"candidate $p_T$ [GeV]", None),
                     (g["R"]["ceta"].numpy(), r"candidate $\eta$", ETA_RANGE))):
                edges = _quantile_edges(xv, w, 10) if rng is None else np.linspace(*rng, 11)
                xs, fit, pred = [], [], []
                for lo, hi in zip(edges[:-1], edges[1:]):
                    sel = (xv >= lo) & (xv < hi)
                    if w[sel].sum() < 200:
                        continue
                    _, sfit, _ = fit_peak(m[sel], g["M"], SIG0[n], g["lo"], g["hi"],
                                          weights=w[sel])
                    xs.append(0.5 * (lo + hi))
                    fit.append(sfit * 1e3)
                    pred.append(np.median(sig[sel]) * 1e3)
                a = ax[i, j]
                a.plot(xs, fit, "o-", ms=3, label="fitted core width")
                a.plot(xs, pred, "s--", ms=3, label=r"median predicted $\sigma_m$")
                a.set_xlabel(lab); a.set_ylabel(r"$\sigma_m$ [MeV]")
                a.set_title(n); a.legend(fontsize=8)
    fig.suptitle("Mass-peak core width vs predicted resolution")
    _savefig(fig, outdir, "fig9_width_overlay")


def plot_r_vs_pt(groups, net, glob, outdir, pt_plot):
    """Figure 10: per-track fractional resolution vs pT.

    absolute: the field prediction decomposed into a/beta and b*pT at eta
    references, pion vs proton hypothesis.
    pull: per-leg binned medians of raw r_cov and calibrated
    r = s*pT^{u_p}*r_cov, split into pion legs and proton legs.
    """
    if net.mode == "absolute":
        pts = np.geomspace(max(pt_plot[0], 5e-2), pt_plot[1], 120)
        tp = torch.tensor(pts, dtype=torch.float64)
        eta_ref = (-1.0, 0.0, 1.0)
        fig, ax = plt.subplots(1, len(eta_ref), figsize=(5.2 * len(eta_ref), 4.4),
                               squeeze=False)
        with torch.no_grad():
            c_mev = float(glob.c(0)) * 1e3
            for i, eta in enumerate(eta_ref):
                a_f, b_f = net.ab(torch.tensor([eta], dtype=torch.float64),
                                  torch.tensor([0.0], dtype=torch.float64))
                a_f, b_f = float(a_f), float(b_f)
                axx = ax[0, i]
                for mass, name, col in ((PION_MASS, r"$\pi$", "C0"),
                                        (PROTON_MASS, "p", "C3")):
                    beta = beta_of(tp, torch.full_like(tp, eta), mass).numpy()
                    t_ms = a_f / beta
                    r = np.sqrt(t_ms**2 + (b_f * pts)**2)
                    axx.plot(pts, 1e2 * r, "-", c=col, label=f"{name}: total")
                    axx.plot(pts, 1e2 * t_ms, ":", c=col, lw=1.0,
                             label=f"{name}: $a/\\beta$")
                axx.plot(pts, 1e2 * b_f * pts, "--", c="k", lw=1.0, label=r"$b\,p_T$")
                axx.set_xscale("log"); axx.set_xlabel(r"$p_T$ [GeV]")
                axx.set_ylabel(r"$\sigma(p_T)/p_T$ [%]")
                axx.set_title(f"$\\eta$ = {eta:+.1f}: a = {a_f:.4f}, "
                              f"b = {b_f:.4f} GeV$^{{-1}}$, c = {c_mev:.2f} MeV")
                axx.legend(fontsize=7); axx.grid(alpha=0.25)
        fig.suptitle("Per-track fractional momentum resolution (absolute model)")
    else:
        legs = {"pion": [], "proton": []}
        with torch.no_grad():
            up = glob.u_p()
            for g in groups:
                R, w = g["R"], g["w"]
                for leg in (1, 2):
                    kind = "proton" if (R["m2"] == PROTON_MASS and leg == 2) else "pion"
                    s = net.s(R[f"eta{leg}"], R[f"phi{leg}"])
                    rp = pull_r(R, leg, s, up)
                    legs[kind].append((R[f"pt{leg}"].numpy(),
                                       R[f"rcov{leg}"].numpy(), rp.numpy(),
                                       w.numpy()))
        fig, ax = plt.subplots(1, 2, figsize=(11, 4.6))
        for axx, kind in zip(ax, ("pion", "proton")):
            pt = np.concatenate([x[0] for x in legs[kind]])
            rc = np.concatenate([x[1] for x in legs[kind]])
            rp = np.concatenate([x[2] for x in legs[kind]])
            edges = np.unique(np.quantile(pt, np.linspace(0, 1, 13)))
            xs, med_c, med_p = [], [], []
            for lo, hi in zip(edges[:-1], edges[1:]):
                sel = (pt >= lo) & (pt < hi)
                if sel.sum() < 100:
                    continue
                xs.append(0.5 * (lo + hi))
                med_c.append(1e2 * np.median(rc[sel]))
                med_p.append(1e2 * np.median(rp[sel]))
            axx.plot(xs, med_c, "s--", ms=3, c="C2", label=r"median raw $r_{cov}$")
            axx.plot(xs, med_p, "o-", ms=3, c="C0",
                     label=r"median calibrated $s\,p_T^{u_p}\,r_{cov}$")
            axx.set_xlabel(r"track $p_T$ [GeV]")
            axx.set_ylabel(r"$\sigma(p_T)/p_T$ [%]")
            axx.set_title(f"{kind} legs")
            axx.legend(fontsize=8); axx.grid(alpha=0.25)
        fig.suptitle("Covariance-reported vs calibrated per-track resolution "
                     "(pull model)")
    _savefig(fig, outdir, "fig10_r_vs_pt")


def plot_ab_maps(net, outdir):
    """Figure 11 (absolute mode): (eta, phi) maps of the two fields."""
    ee = np.linspace(*ETA_RANGE, 60); pp = np.linspace(-np.pi, np.pi, 60)
    E, P = np.meshgrid(ee, pp, indexing="ij")
    t = lambda x: torch.tensor(x.ravel(), dtype=torch.float64)
    with torch.no_grad():
        a, b = net.ab(t(E), t(P))
    a = a.numpy().reshape(E.shape); b = b.numpy().reshape(E.shape)
    fig, ax = plt.subplots(1, 2, figsize=(12.5, 4.8))
    for axx, z, lab, ttl in (
        (ax[0], a, "MS-like fractional resolution coefficient "
                   r"(dimensionless, enters as $a/\beta$)", r"$a(\eta,\phi)$"),
        (ax[1], b, r"curvature term coefficient [1/GeV], enters as $b\,p_T$",
                   r"$b(\eta,\phi)$")):
        im = axx.pcolormesh(ee, pp, z.T, shading="auto", cmap="viridis")
        axx.set_xlabel(r"$\eta$"); axx.set_ylabel(r"$\phi$"); axx.set_title(ttl)
        cb = fig.colorbar(im, ax=axx); cb.set_label(lab, fontsize=8)
    fig.suptitle("Resolution fields (charge-even by construction)")
    _savefig(fig, outdir, "fig11_ab_maps")


def plot_r_maps_vs_curv(net, glob, outdir, curv_slices):
    """Figure 12 (absolute mode): r(eta, phi) maps across 1/pT, pi vs p."""
    ee = np.linspace(*ETA_RANGE, 50); pp = np.linspace(-np.pi, np.pi, 50)
    E, P = np.meshgrid(ee, pp, indexing="ij")
    t = lambda a: torch.tensor(a.ravel(), dtype=torch.float64)
    n = len(curv_slices); ncols = 4
    nrows_each = int(np.ceil(n / ncols))

    with torch.no_grad():
        a_f, b_f = net.ab(t(E), t(P))
    a_f, b_f = a_f.numpy(), b_f.numpy()

    r_pi, r_p = [], []
    for u in curv_slices:
        pt = 1.0 / u
        beta_pi = beta_of(torch.tensor(np.full(E.size, pt)), t(E), PION_MASS).numpy()
        beta_p = beta_of(torch.tensor(np.full(E.size, pt)), t(E), PROTON_MASS).numpy()
        r_pi.append((1e2 * np.sqrt((a_f / beta_pi)**2 + (b_f * pt)**2)).reshape(E.shape))
        r_p.append((1e2 * np.sqrt((a_f / beta_p)**2 + (b_f * pt)**2)).reshape(E.shape))
    vmax_pi = max(z.max() for z in r_pi)
    vmax_p = max(z.max() for z in r_p)

    fig, ax = plt.subplots(2 * nrows_each, ncols,
                           figsize=(4.6 * ncols, 4.1 * 2 * nrows_each))
    for block, (maps, vmax, lab) in enumerate((
            (r_pi, vmax_pi, r"$r=\sigma(p_T)/p_T$ [%]  (pion hypothesis)"),
            (r_p, vmax_p, r"$r=\sigma(p_T)/p_T$ [%]  (proton hypothesis)"))):
        for idx, u in enumerate(curv_slices):
            r, c = divmod(idx, ncols)
            a = ax[block * nrows_each + r, c]
            im = a.pcolormesh(ee, pp, maps[idx].T, shading="auto", cmap="viridis",
                              vmin=0, vmax=vmax)
            a.set_title(("$\\pi$, " if block == 0 else "p, ") + _slice_title(u),
                        fontsize=9)
            a.set_xlabel("eta"); a.set_ylabel("phi")
            cb = fig.colorbar(im, ax=a)
            cb.set_label(lab)
        for idx in range(n, nrows_each * ncols):
            r, c = divmod(idx, ncols); ax[block * nrows_each + r, c].axis("off")
    fig.suptitle(r"Per-track fractional momentum resolution $r(\eta,\phi)$ "
                r"across 1/pT, by mass hypothesis")
    _savefig(fig, outdir, "fig12_r_maps_vs_curv")


def plot_calibration_factor(net, glob, outdir):
    """Figure 13 (pull mode): s(eta, phi) map plus 1D profiles.

    Read-off: 'the tracker under/over-estimates sigma(pT) by factor s'."""
    ee = np.linspace(*ETA_RANGE, 60); pp = np.linspace(-np.pi, np.pi, 60)
    E, P = np.meshgrid(ee, pp, indexing="ij")
    t = lambda x: torch.tensor(x.ravel(), dtype=torch.float64)
    with torch.no_grad():
        s = net.s(t(E), t(P)).numpy().reshape(E.shape)
        up = float(glob.u_p())
    fig, ax = plt.subplots(1, 3, figsize=(16, 4.6))
    im = ax[0].pcolormesh(ee, pp, s.T, shading="auto", cmap="viridis")
    ax[0].set_xlabel(r"$\eta$"); ax[0].set_ylabel(r"$\phi$")
    ax[0].set_title(rf"$s(\eta,\phi)$   ($u_p$ = {up:+.4f})")
    cb = fig.colorbar(im, ax=ax[0])
    cb.set_label("covariance calibration factor s  (s = 1: errors correct)")
    ax[1].plot(ee, s.mean(axis=1), "o-", ms=3)
    ax[1].axhline(1.0, c="k", ls="--", lw=0.8)
    ax[1].set_xlabel(r"$\eta$"); ax[1].set_ylabel(r"$\langle s\rangle_\phi$")
    ax[1].set_title("phi-averaged profile")
    ax[2].plot(pp, s.mean(axis=0), "o-", ms=3)
    ax[2].axhline(1.0, c="k", ls="--", lw=0.8)
    ax[2].set_xlabel(r"$\phi$"); ax[2].set_ylabel(r"$\langle s\rangle_\eta$")
    ax[2].set_title("eta-averaged profile")
    fig.suptitle("Covariance calibration field: r = s(eta,phi) * pT^{u_p} * r_cov")
    _savefig(fig, outdir, "fig13_calibration_factor")


def resolution_diagnostics(net, glob, groups, outdir, meta, n_c=1, scalars=None):
    pw, raw_cores = plot_pull_closure(groups, net, glob, outdir)
    plot_width_overlay(groups, net, glob, outdir)
    plot_r_vs_pt(groups, net, glob, outdir, meta["pt_plot"])
    if net.mode == "absolute":
        plot_ab_maps(net, outdir)
        plot_r_maps_vs_curv(net, glob, outdir, meta["curv_slices"])
    else:
        plot_calibration_factor(net, glob, outdir)

    lines = ["", f"--- stage 2: momentum resolution ({net.mode} model) ---"]
    if net.mode == "pull":
        lines += ["# " + s for s in PULL_INTERPRETATION]
    for gi, g in enumerate(groups):
        core, rms = pw[g["name"]]
        with torch.no_grad():
            fg, kg = float(glob.f(gi)), float(glob.k(gi))
        exp_rms = math.sqrt(1 - fg + fg * kg**2)
        line = (f"{g['name']:10s} pull core width {core:6.4f}  RMS {rms:6.4f} "
                f"(expected RMS from tail {exp_rms:5.3f})   "
                f"f = {fg:6.4f}, k = {kg:5.3f}")
        if g["name"] in raw_cores and np.isfinite(raw_cores[g["name"]]):
            line += f"   raw-cov pull core {raw_cores[g['name']]:6.4f}"
        lines.append(line)
    with torch.no_grad():
        for i in range(n_c):
            lines.append(f"{'floor':10s} c[{i}] = {float(glob.c(i))*1e3:7.4f} MeV")
    with torch.no_grad():
        if net.mode == "absolute":
            aa, bb = [], []
            for g in groups:
                for leg in ("1", "2"):
                    a, b = net.ab(g["R"]["eta" + leg], g["R"]["phi" + leg])
                    aa.append(a.numpy()); bb.append(b.numpy())
            aa = np.concatenate(aa); bb = np.concatenate(bb)
            lines.append(f"{'fields':10s} track-averaged a = {aa.mean():.5f} "
                         f"(spread {aa.std():.5f}),  b = {bb.mean():.5f} "
                         f"(spread {bb.std():.5f}) GeV^-1")
        else:
            up = glob.u_p()
            lines.append(f"{'exponent':10s} u_p = {float(up):+.5f} "
                         f"(0 = no residual pT shape on top of r_cov)")
            for g in groups:
                R, ratio = g["R"], []
                for leg in (1, 2):
                    s = net.s(R[f"eta{leg}"], R[f"phi{leg}"])
                    ratio.append((pull_r(R, leg, s, up) /
                                  R[f"rcov{leg}"]).numpy())
                ratio = np.concatenate(ratio)
                q16, q50, q84 = np.quantile(ratio, [0.16, 0.50, 0.84])
                lines.append(f"{g['name']:10s} r_pred/r_cov median {q50:.4f}  "
                             f"[16%, 84%] = [{q16:.4f}, {q84:.4f}]   "
                             f"<- factor by which the reported momentum "
                             f"uncertainty is wrong")
    if scalars is not None:
        if net.mode == "absolute":
            lines.append(f"{'reduced':10s} constant-field fit: "
                         f"a = {scalars['a']:.5f} +- {scalars['sa']:.5f}, "
                         f"b = {scalars['b']:.5f} +- {scalars['sb']:.5f};  "
                         f"rho(a,b) = {scalars['rho_ab']:+.3f}, "
                         f"rho(a,c) = {scalars['rho_ac']:+.3f}"
                         f"{'' if scalars['pos_def'] else '   [Hessian NOT pos.def.]'}")
        else:
            lines.append(f"{'reduced':10s} constant-field fit: "
                         f"s = {scalars['s']:.5f} +- {scalars['ss']:.5f}, "
                         f"u_p = {scalars['u_p']:+.5f} +- {scalars['su']:.5f};  "
                         f"rho(s,u_p) = {scalars['rho_su']:+.3f}, "
                         f"rho(s,c) = {scalars['rho_sc']:+.3f}"
                         f"{'' if scalars['pos_def'] else '   [Hessian NOT pos.def.]'}")
        lines.append(f"{'':10s} NB: on data this reduced fit is misspecified "
                     f"(the field really depends on eta, phi); central values "
                     f"are an occupancy-weighted compromise and only the "
                     f"injection-toy errors are strictly interpretable.")
    txt = "\n".join(lines)
    print(txt)
    with open(os.path.join(outdir, "summary.txt"), "a") as f:
        f.write(txt + "\n")
