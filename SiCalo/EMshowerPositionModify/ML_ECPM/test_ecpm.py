from __future__ import annotations

import os
import argparse
import numpy as np

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

import torch
from torch.utils.data import DataLoader
from scipy.optimize import curve_fit
from scipy.stats import crystalball

from ecpm_data import ECPMDataset, ECPMFeatureStats, ecpm_collate_fn
from ecpm_model import build_ecpm_model


def get_device():
    return torch.device("cuda" if torch.cuda.is_available() else "cpu")


def safe_torch_load(path, map_location):
    try:
        return torch.load(path, map_location=map_location, weights_only=False)
    except TypeError:
        return torch.load(path, map_location=map_location)


@torch.no_grad()
def collect_before_after(model, loader, dataset, device, target_mode):
    model.eval()

    before_list = []
    after_list = []
    pred_list = []
    pt_list = []

    for batch in loader:
        global_x = batch["global"].to(device)
        towers = batch["towers"].to(device)
        tower_mask = batch["tower_mask"].to(device)
        target_norm = batch["target"].to(device)

        pred_norm = model(global_x, towers, tower_mask)
        target = dataset.denormalize_target(target_norm)

        pred_full = torch.zeros(target.shape[0], 2, device=device)

        if target_mode == "dphi":
            pred_full[:, 0:1] = pred_norm
            pred = dataset.denormalize_target(pred_full)[:, 0:1]
            true = target[:, 0:1]

        elif target_mode == "dR":
            pred_full[:, 1:2] = pred_norm
            pred = dataset.denormalize_target(pred_full)[:, 1:2]
            true = target[:, 1:2]

        else:
            raise ValueError(f"Unsupported target_mode for this test script: {target_mode}")

        before = true
        after = true - pred

        pt = np.array([m["primary_pt"] for m in batch["meta"]], dtype=np.float32)

        before_list.append(before.cpu())
        after_list.append(after.cpu())
        pred_list.append(pred.cpu())
        pt_list.append(pt)

    before = torch.cat(before_list, dim=0).numpy().reshape(-1)
    after = torch.cat(after_list, dim=0).numpy().reshape(-1)
    pred = torch.cat(pred_list, dim=0).numpy().reshape(-1)
    pt = np.concatenate(pt_list, axis=0).reshape(-1)

    return before, after, pred, pt


def q68_width(x):
    q16, q84 = np.quantile(x, [0.16, 0.84])
    return 0.5 * (q84 - q16)


def print_stats(name, x):
    print(f"{name:12s} mean = {np.mean(x): .6e}")
    print(f"{name:12s} std  = {np.std(x): .6e}")
    print(f"{name:12s} q68  = {q68_width(x): .6e}")


def gauss(x, amp, mean, sigma, offset):
    return amp * np.exp(-0.5 * ((x - mean) / sigma) ** 2) + offset

def crystalball_func(x, amp, beta, m, loc, scale, offset):
    return amp * crystalball.pdf(x, beta, m, loc=loc, scale=scale) + offset



def fit_hist_peak(x, fit_min, fit_max, bins=80):
    """
    Unified fit function.

    This function is called only once per pt bin.
    The returned fit result is reused by:
      1) peak_mean_vs_pt / peak_sigma_vs_pt
      2) fitshow plots

    Crystal Ball fit and Gaussian fit.
    """
    in_fit = (x >= fit_min) & (x <= fit_max)
    x_fit = x[in_fit]

    if len(x_fit) < 50:
        return None

    hist, edges = np.histogram(x_fit, bins=bins, range=(fit_min, fit_max))
    centers = 0.5 * (edges[:-1] + edges[1:])

    if np.sum(hist) < 50 or np.max(hist) <= 0:
        return None

    amp0 = float(np.max(hist))
    mean0 = float(centers[np.argmax(hist)])
    sigma0 = float(np.std(x_fit))

    if not np.isfinite(sigma0) or sigma0 <= 0:
        sigma0 = 0.002

    # ============================================================
    # Gaussian fit call
    # ============================================================
    # try:
    #     popt, _ = curve_fit(
    #         gauss,
    #         centers,
    #         hist,
    #         p0=[amp0, mean0, sigma0, 0.0],
    #         bounds=(
    #             [0.0, fit_min, 1.0e-9, 0.0],
    #             [np.inf, fit_max, np.inf, np.inf],
    #         ),
    #         maxfev=10000,
    #     )
    
    #     amp, mean, sigma, offset = popt
    
    #     return {
    #         "model": "gauss",
    #         "func": gauss,
    #         "centers": centers,
    #         "hist": hist,
    #         "popt": popt,
    #         "peak": float(mean),
    #         "width": abs(float(sigma)),
    #         "fit_label": (
    #             f"Gaussian fit\n"
    #             f"mean={mean:.3e}, sigma={abs(sigma):.3e}"
    #         ),
    #     }
    
    # except Exception:
    #     return None

    # ============================================================
    # Crystal Ball fit call
    # ============================================================
    try:
        popt, _ = curve_fit(
            crystalball_func,
            centers,
            hist,
            p0=[amp0 * sigma0, 2.0, 3.0, mean0, sigma0, 0.0],
            bounds=(
                [0.0, 0.5, 1.01, fit_min, 1.0e-9, 0.0],
                [np.inf, 20.0, 100.0, fit_max, np.inf, np.inf],
            ),
            maxfev=20000,
        )

        amp, beta, m, loc, scale, offset = popt

        return {
            "model": "crystalball",
            "func": crystalball_func,
            "centers": centers,
            "hist": hist,
            "popt": popt,
            "peak": float(loc),
            "width": abs(float(scale)),
            "fit_label": (
                f"CB fit\n"
                f"loc={loc:.3e}, scale={abs(scale):.3e}\n"
                f"beta={beta:.2f}, m={m:.2f}"
            ),
        }

    except Exception:
        return None


def get_label_and_unit(target_mode):
    if target_mode == "dphi":
        return r"$\Delta\phi = \phi_{\mathrm{CoG}} - \phi_{\mathrm{cluster}}$", "rad"
    if target_mode == "dR":
        return r"$\Delta R = R_{\mathrm{CoG}} - R_{\mathrm{cluster}}$", "cm"
    raise ValueError(target_mode)


def plot_compare(before, after, outdir, target_mode, bins=120, xmin=None, xmax=None, logy=False):
    if xmin is None:
        xmin = min(np.min(before), np.min(after))
    if xmax is None:
        xmax = max(np.max(before), np.max(after))

    xlabel, unit = get_label_and_unit(target_mode)

    plt.figure(figsize=(7, 5))

    plt.hist(
        before,
        bins=bins,
        range=(xmin, xmax),
        histtype="step",
        linewidth=1.8,
        label=f"before model\nstd={np.std(before):.3e}, q68={q68_width(before):.3e}",
    )

    plt.hist(
        after,
        bins=bins,
        range=(xmin, xmax),
        histtype="step",
        linewidth=1.8,
        label=f"after model\nstd={np.std(after):.3e}, q68={q68_width(after):.3e}",
    )

    plt.xlabel(f"{xlabel} [{unit}]")
    plt.ylabel("Counts")
    plt.title(f"{target_mode} distribution before/after model correction")
    plt.legend()
    plt.grid(True, alpha=0.3)

    if logy:
        plt.yscale("log")
        outname = os.path.join(outdir, f"{target_mode}_before_after_logy.png")
    else:
        outname = os.path.join(outdir, f"{target_mode}_before_after.png")

    plt.tight_layout()
    plt.savefig(outname, dpi=150)
    plt.close()

    print(f"[PLOT] saved: {outname}")


def plot_value_vs_pt(pt, value, outdir, target_mode, name, pt_min, pt_max, value_min, value_max):
    ylabel, unit = get_label_and_unit(target_mode)

    plt.figure(figsize=(7, 5))

    plt.hist2d(
        pt,
        value,
        bins=[100, 120],
        range=[[pt_min, pt_max], [value_min, value_max]],
        cmin=1,
    )

    plt.xlabel(r"$p_T$ [GeV]")
    plt.ylabel(f"{ylabel} [{unit}]")
    plt.title(f"{name}: {target_mode} vs pt")
    plt.colorbar(label="Counts")
    plt.tight_layout()

    outname = os.path.join(outdir, f"{target_mode}_vs_pt_{name}.png")
    plt.savefig(outname, dpi=150)
    plt.close()

    print(f"[PLOT] saved: {outname}")



def fit_vs_pt(pt, value, pt_min, pt_max, pt_step, fit_min, fit_max, bins=80):
    """
    Fit once for each pt bin.

    The returned list is the single source of truth for:
      - peak mean vs pt
      - peak sigma/width vs pt
      - fitshow plots
    """
    results = []
    edges = np.arange(pt_min, pt_max + 1.0e-9, pt_step)

    for lo, hi in zip(edges[:-1], edges[1:]):
        mask = (pt >= lo) & (pt < hi)
        count = int(np.sum(mask))

        if count < 50:
            continue

        fit_result = fit_hist_peak(
            value[mask],
            fit_min=fit_min,
            fit_max=fit_max,
            bins=bins,
        )

        if fit_result is None:
            print(f"[WARN] fit failed: pt=[{lo},{hi}], count={count}")
            continue

        fit_result["pt_lo"] = float(lo)
        fit_result["pt_hi"] = float(hi)
        fit_result["pt_center"] = 0.5 * (float(lo) + float(hi))
        fit_result["count"] = count

        results.append(fit_result)

    return results


def get_fit_model_name(fit_results_before, fit_results_after):
    for item in fit_results_before:
        return item["model"]
    for item in fit_results_after:
        return item["model"]
    return "fit"


def get_peak_ylabel(target_mode, fit_model):
    ylabel, unit = get_label_and_unit(target_mode)

    if fit_model == "gauss":
        mean_label = f"Gaussian peak mean of {ylabel} [{unit}]"
        width_label = f"Gaussian sigma of {ylabel} [{unit}]"
    elif fit_model == "crystalball":
        mean_label = f"Crystal Ball loc of {ylabel} [{unit}]"
        width_label = f"Crystal Ball scale of {ylabel} [{unit}]"
    else:
        mean_label = f"Fitted peak position of {ylabel} [{unit}]"
        width_label = f"Fitted width of {ylabel} [{unit}]"

    return mean_label, width_label


def extract_fit_arrays(fit_results):
    pt_centers = np.array([r["pt_center"] for r in fit_results])
    peaks = np.array([r["peak"] for r in fit_results])
    widths = np.array([r["width"] for r in fit_results])
    counts = np.array([r["count"] for r in fit_results])
    return pt_centers, peaks, widths, counts


def plot_peak_mean_compare_vs_pt(fit_results_before, fit_results_after, outdir, target_mode):
    pt_b, mean_b, _, _ = extract_fit_arrays(fit_results_before)
    pt_a, mean_a, _, _ = extract_fit_arrays(fit_results_after)

    fit_model = get_fit_model_name(fit_results_before, fit_results_after)
    mean_label, _ = get_peak_ylabel(target_mode, fit_model)

    plt.figure(figsize=(7, 5))
    plt.scatter(pt_b, mean_b, label="before model")
    plt.scatter(pt_a, mean_a, label="after model")
    plt.axhline(0.0, linestyle="--", linewidth=1)

    plt.xlabel(r"$p_T$ [GeV]")
    plt.ylabel(mean_label)
    plt.title(f"{target_mode}: peak position vs pt")
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    outname = os.path.join(outdir, f"peak_mean_vs_pt_{target_mode}_compare.png")
    plt.savefig(outname, dpi=150)
    plt.close()

    print(f"[PLOT] saved: {outname}")


def plot_peak_sigma_compare_vs_pt(fit_results_before, fit_results_after, outdir, target_mode):
    pt_b, _, sigma_b, _ = extract_fit_arrays(fit_results_before)
    pt_a, _, sigma_a, _ = extract_fit_arrays(fit_results_after)

    fit_model = get_fit_model_name(fit_results_before, fit_results_after)
    _, width_label = get_peak_ylabel(target_mode, fit_model)

    plt.figure(figsize=(7, 5))
    plt.scatter(pt_b, sigma_b, label="before model")
    plt.scatter(pt_a, sigma_a, label="after model")

    plt.xlabel(r"$p_T$ [GeV]")
    plt.ylabel(width_label)
    plt.title(f"{target_mode}: peak width vs pt")
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    outname = os.path.join(outdir, f"peak_sigma_vs_pt_{target_mode}_compare.png")
    plt.savefig(outname, dpi=150)
    plt.close()

    print(f"[PLOT] saved: {outname}")


def plot_fit_show_one_from_result(fit_result, outdir, target_mode, name, fit_min, fit_max):
    centers = fit_result["centers"]
    hist = fit_result["hist"]
    popt = fit_result["popt"]
    fit_func = fit_result["func"]

    pt_lo = fit_result["pt_lo"]
    pt_hi = fit_result["pt_hi"]

    xx = np.linspace(fit_min, fit_max, 500)
    yy = fit_func(xx, *popt)

    xlabel, unit = get_label_and_unit(target_mode)

    plt.figure(figsize=(7, 5))
    plt.step(
        centers,
        hist,
        where="mid",
        linewidth=1.8,
        label="data",
    )
    plt.plot(
        xx,
        yy,
        linewidth=2,
        label=fit_result["fit_label"],
    )

    plt.xlabel(f"{xlabel} [{unit}]")
    plt.ylabel("Counts")
    plt.title(f"{name}, {pt_lo:.1f} < pT < {pt_hi:.1f} GeV")
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    safe_name = name.replace(" ", "_")
    outname = os.path.join(
        outdir,
        f"{target_mode}_{safe_name}_pt{pt_lo:.1f}_{pt_hi:.1f}_fit.png",
    )
    plt.savefig(outname, dpi=150)
    plt.close()

    print(f"[PLOT] saved: {outname}")


def plot_fit_show_vs_pt(fit_results_before, fit_results_after, outdir, target_mode,
                        fit_min, fit_max):
    fitshow_dir = os.path.join(outdir, "fitshow")
    os.makedirs(fitshow_dir, exist_ok=True)

    for fit_result in fit_results_before:
        plot_fit_show_one_from_result(
            fit_result=fit_result,
            outdir=fitshow_dir,
            target_mode=target_mode,
            name="before_model",
            fit_min=fit_min,
            fit_max=fit_max,
        )

    for fit_result in fit_results_after:
        plot_fit_show_one_from_result(
            fit_result=fit_result,
            outdir=fitshow_dir,
            target_mode=target_mode,
            name="after_model",
            fit_min=fit_min,
            fit_max=fit_max,
        )

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--root-file",
        type=str,
        default="/root/autodl-tmp/Dataset/ECPM_ana527_Electron_5GeV_1M_test.root",
    )
    parser.add_argument(
        "--ckpt",
        type=str,
        default="./ECPM_out/best_ecpm_dphi.pt",
    )

    parser.add_argument("--tree-name", type=str, default="tree")
    parser.add_argument("--batch-size", type=int, default=256)
    parser.add_argument("--min-tower-e", type=float, default=0.0)

    # 注意：这里仍然保留 dphi clean sample cut
    parser.add_argument("--max-abs-target-dphi", type=float, default=0.7)

    parser.add_argument("--outdir", type=str, default="ECPM_test_out")
    parser.add_argument("--bins", type=int, default=120)

    # dphi 推荐 -0.02 ~ 0.02
    # dR 推荐比如 -20 ~ 20，或者你自己按分布改
    parser.add_argument("--xmin", type=float, default=None)
    parser.add_argument("--xmax", type=float, default=None)

    parser.add_argument("--pt-min", type=float, default=0.0)
    parser.add_argument("--pt-max", type=float, default=5.0)
    parser.add_argument("--pt-step", type=float, default=1.0)

    # dphi 推荐 -0.005 ~ 0.005
    # dR 推荐比如 -5 ~ 5
    parser.add_argument("--fit-min", type=float, default=None)
    parser.add_argument("--fit-max", type=float, default=None)

    args = parser.parse_args()

    os.makedirs(args.outdir, exist_ok=True)

    device = get_device()
    print(f"[INFO] device = {device}")

    ckpt = safe_torch_load(args.ckpt, map_location=device)
    target_mode = ckpt.get("target_mode", "dphi")

    if target_mode not in ["dphi", "dR"]:
        raise ValueError(f"This test script supports dphi/dR only, got target_mode = {target_mode}")

    print(f"[INFO] ckpt target_mode = {target_mode}")

    if args.xmin is None or args.xmax is None:
        if target_mode == "dphi":
            xmin, xmax = -0.02, 0.02
        elif target_mode == "dR":
            xmin, xmax = -20.0, 20.0
    else:
        xmin, xmax = args.xmin, args.xmax

    if args.fit_min is None or args.fit_max is None:
        if target_mode == "dphi":
            fit_min, fit_max = -0.02, 0.02
        elif target_mode == "dR":
            fit_min, fit_max = -5.0, 5.0
    else:
        fit_min, fit_max = args.fit_min, args.fit_max

    print(f"[INFO] plot range = [{xmin}, {xmax}]")
    print(f"[INFO] fit range  = [{fit_min}, {fit_max}]")

    stats = ECPMFeatureStats(
        global_mean=ckpt["global_mean"].cpu(),
        global_std=ckpt["global_std"].cpu(),
        tower_mean=ckpt["tower_mean"].cpu(),
        tower_std=ckpt["tower_std"].cpu(),
        target_mean=ckpt["target_mean"].cpu(),
        target_std=ckpt["target_std"].cpu(),
    )

    dataset = ECPMDataset(
        root_file=args.root_file,
        tree_name=args.tree_name,
        min_tower_e=args.min_tower_e,
        max_abs_target_dphi=args.max_abs_target_dphi,
        stats=stats,
        normalize=True,
        verbose=True,
    )

    loader = DataLoader(
        dataset,
        batch_size=args.batch_size,
        shuffle=False,
        num_workers=0,
        collate_fn=ecpm_collate_fn,
    )

    model = build_ecpm_model(target_mode=target_mode).to(device)
    model.load_state_dict(ckpt["model_state_dict"])

    before, after, pred, pt = collect_before_after(
        model=model,
        loader=loader,
        dataset=dataset,
        device=device,
        target_mode=target_mode,
    )

    print(f"\n[{target_mode} resolution]")
    print_stats("before model", before)
    print_stats("after model", after)
    print_stats("pred", pred)

    plot_compare(
        before,
        after,
        args.outdir,
        target_mode=target_mode,
        bins=args.bins,
        xmin=xmin,
        xmax=xmax,
        logy=False,
    )

    plot_compare(
        before,
        after,
        args.outdir,
        target_mode=target_mode,
        bins=args.bins,
        xmin=xmin,
        xmax=xmax,
        logy=True,
    )

    plot_value_vs_pt(
        pt=pt,
        value=before,
        outdir=args.outdir,
        target_mode=target_mode,
        name="before_model",
        pt_min=args.pt_min,
        pt_max=args.pt_max,
        value_min=xmin,
        value_max=xmax,
    )

    plot_value_vs_pt(
        pt=pt,
        value=after,
        outdir=args.outdir,
        target_mode=target_mode,
        name="after_model",
        pt_min=args.pt_min,
        pt_max=args.pt_max,
        value_min=xmin,
        value_max=xmax,
    )

    fit_results_before = fit_vs_pt(
        pt=pt,
        value=before,
        pt_min=args.pt_min,
        pt_max=args.pt_max,
        pt_step=args.pt_step,
        fit_min=fit_min,
        fit_max=fit_max,
        bins=80,
    )

    fit_results_after = fit_vs_pt(
        pt=pt,
        value=after,
        pt_min=args.pt_min,
        pt_max=args.pt_max,
        pt_step=args.pt_step,
        fit_min=fit_min,
        fit_max=fit_max,
        bins=80,
    )

    plot_peak_mean_compare_vs_pt(
        fit_results_before=fit_results_before,
        fit_results_after=fit_results_after,
        outdir=args.outdir,
        target_mode=target_mode,
    )

    plot_peak_sigma_compare_vs_pt(
        fit_results_before=fit_results_before,
        fit_results_after=fit_results_after,
        outdir=args.outdir,
        target_mode=target_mode,
    )

    plot_fit_show_vs_pt(
        fit_results_before=fit_results_before,
        fit_results_after=fit_results_after,
        outdir=args.outdir,
        target_mode=target_mode,
        fit_min=fit_min,
        fit_max=fit_max,
    )


if __name__ == "__main__":
    main()