from __future__ import annotations

import os
import argparse
import math
import numpy as np

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

import torch
import torch.nn as nn
from torch.optim import Adam
from torch.optim.lr_scheduler import ReduceLROnPlateau

from ecpm_data import ECPMDataModule
from ecpm_model import build_ecpm_model, select_target


def get_device():
    return torch.device("cuda" if torch.cuda.is_available() else "cpu")


def denormalize_selected_target(dm, y, target_mode):
    y_full = torch.zeros(y.shape[0], 2, device=y.device)

    if target_mode == "dphi":
        y_full[:, 0:1] = y
    elif target_mode == "dR":
        y_full[:, 1:2] = y
    elif target_mode == "both":
        y_full = y
    else:
        raise ValueError(target_mode)

    y_full = dm.dataset.denormalize_target(y_full)

    if target_mode == "dphi":
        return y_full[:, 0:1]
    if target_mode == "dR":
        return y_full[:, 1:2]
    return y_full


def run_one_epoch(model, loader, criterion, optimizer, device, target_mode):
    model.train()

    total_loss = 0.0
    total_count = 0

    for batch in loader:
        global_x = batch["global"].to(device)
        towers = batch["towers"].to(device)
        tower_mask = batch["tower_mask"].to(device)
        target = batch["target"].to(device)

        y = select_target(target, target_mode)

        pred = model(global_x, towers, tower_mask)
        loss = criterion(pred, y)

        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

        batch_size = global_x.shape[0]
        total_loss += loss.item() * batch_size
        total_count += batch_size

    return total_loss / max(total_count, 1)


@torch.no_grad()
def evaluate(model, loader, criterion, device, target_mode):
    model.eval()

    total_loss = 0.0
    total_count = 0

    for batch in loader:
        global_x = batch["global"].to(device)
        towers = batch["towers"].to(device)
        tower_mask = batch["tower_mask"].to(device)
        target = batch["target"].to(device)

        y = select_target(target, target_mode)

        pred = model(global_x, towers, tower_mask)
        loss = criterion(pred, y)

        batch_size = global_x.shape[0]
        total_loss += loss.item() * batch_size
        total_count += batch_size

    return total_loss / max(total_count, 1)


@torch.no_grad()
def test_resolution(model, loader, dm, device, target_mode):
    model.eval()

    before = []
    after = []

    for batch in loader:
        global_x = batch["global"].to(device)
        towers = batch["towers"].to(device)
        tower_mask = batch["tower_mask"].to(device)
        target_norm = batch["target"].to(device)

        pred_norm = model(global_x, towers, tower_mask)

        target = dm.dataset.denormalize_target(target_norm)
        pred = denormalize_selected_target(dm, pred_norm, target_mode)

        if target_mode == "dphi":
            true_dphi = target[:, 0:1]
            pred_dphi = pred

            residual_before = true_dphi
            residual_after = true_dphi - pred_dphi

        elif target_mode == "dR":
            true_dR = target[:, 1:2]
            pred_dR = pred

            residual_before = true_dR
            residual_after = true_dR - pred_dR

        elif target_mode == "both":
            residual_before = target
            residual_after = target - pred

        else:
            raise ValueError(target_mode)

        before.append(residual_before.cpu())
        after.append(residual_after.cpu())

    # before = torch.cat(before, dim=0)
    # after = torch.cat(after, dim=0)

    # return {
    #     "before_mean": before.mean(dim=0),
    #     "before_std": before.std(dim=0),
    #     "after_mean": after.mean(dim=0),
    #     "after_std": after.std(dim=0),
    # }
    
    before = torch.cat(before, dim=0)
    after = torch.cat(after, dim=0)

    def q68_width(x):
        q16 = torch.quantile(x, 0.16, dim=0)
        q84 = torch.quantile(x, 0.84, dim=0)
        return 0.5 * (q84 - q16)

    return {
        "before_mean": before.mean(dim=0),
        "before_std": before.std(dim=0),
        "before_q68": q68_width(before),
        "after_mean": after.mean(dim=0),
        "after_std": after.std(dim=0),
        "after_q68": q68_width(after),
    }


@torch.no_grad()
def plot_pred_vs_true(model, loader, dm, device, target_mode, outdir):
    model.eval()

    true_list = []
    pred_list = []

    for batch in loader:
        global_x = batch["global"].to(device)
        towers = batch["towers"].to(device)
        tower_mask = batch["tower_mask"].to(device)
        target_norm = batch["target"].to(device)

        pred_norm = model(global_x, towers, tower_mask)

        target = dm.dataset.denormalize_target(target_norm)
        pred = denormalize_selected_target(dm, pred_norm, target_mode)

        if target_mode == "dphi":
            true = target[:, 0:1]
        elif target_mode == "dR":
            true = target[:, 1:2]
        elif target_mode == "both":
            true = target[:, 0:1]
            pred = pred[:, 0:1]
        else:
            raise ValueError(target_mode)

        true_list.append(true.cpu())
        pred_list.append(pred.cpu())

    true = torch.cat(true_list, dim=0).numpy().reshape(-1)
    pred = torch.cat(pred_list, dim=0).numpy().reshape(-1)

    corr = np.corrcoef(true, pred)[0, 1]

    plt.figure(figsize=(6, 5))
    plt.hist2d(true, pred, bins=100)
    plt.xlabel(f"true {target_mode}")
    plt.ylabel(f"pred {target_mode}")
    plt.title(f"pred vs true, corr = {corr:.4f}")
    plt.colorbar(label="counts")

    lim_min = min(true.min(), pred.min())
    lim_max = max(true.max(), pred.max())
    plt.plot([lim_min, lim_max], [lim_min, lim_max], "k--", linewidth=1)

    outname = os.path.join(outdir, f"pred_vs_true_{target_mode}.png")
    plt.tight_layout()
    plt.savefig(outname, dpi=150)
    plt.close()

    print(f"[PLOT] saved: {outname}")
    print(f"[DIAG] corr(true, pred) = {corr:.6f}")

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--root-file",
        type=str,
        default="/root/autodl-tmp/Dataset/ECPM_ana527_Electron_5GeV_1M_train.root",
    )
    parser.add_argument("--tree-name", type=str, default="tree")
    parser.add_argument("--target-mode", type=str, default="dphi", choices=["dphi", "dR", "both"])

    parser.add_argument("--batch-size", type=int, default=128)
    parser.add_argument("--epochs", type=int, default=50)
    parser.add_argument("--lr", type=float, default=1.0e-3)
    parser.add_argument("--weight-decay", type=float, default=1.0e-5)
    parser.add_argument("--smoothl1-beta", type=float, default=0.5)
    
    parser.add_argument("--train-frac", type=float, default=0.8)
    parser.add_argument("--val-frac", type=float, default=0.1)
    parser.add_argument("--seed", type=int, default=12345)
    parser.add_argument("--min-tower-e", type=float, default=0.0)

    parser.add_argument("--max-abs-target-dphi", type=float, default=0.007)

    parser.add_argument("--outdir", type=str, default="ECPM_out")

    
    
    args = parser.parse_args()

    os.makedirs(args.outdir, exist_ok=True)

    device = get_device()
    print(f"[INFO] device = {device}")

    dm = ECPMDataModule(
        root_file=args.root_file,
        tree_name=args.tree_name,
        batch_size=args.batch_size,
        train_frac=args.train_frac,
        val_frac=args.val_frac,
        seed=args.seed,
        normalize=True,
        min_tower_e=args.min_tower_e,
        max_abs_target_dphi=args.max_abs_target_dphi,
    )

    dm.setup()

    train_loader = dm.train_dataloader()
    val_loader = dm.val_dataloader()
    test_loader = dm.test_dataloader()

    model = build_ecpm_model(target_mode=args.target_mode).to(device)

    # criterion = nn.MSELoss()
    criterion = nn.SmoothL1Loss(beta=args.smoothl1_beta)
    print(f"[INFO] SmoothL1 beta = {args.smoothl1_beta}")

    optimizer = Adam(
        model.parameters(),
        lr=args.lr,
        weight_decay=args.weight_decay,
    )

    scheduler = ReduceLROnPlateau(
        optimizer,
        mode="min",
        factor=0.5,
        patience=10,
    )

    best_val_loss = math.inf
    best_path = os.path.join(args.outdir, f"best_ecpm_{args.target_mode}.pt")

    print(model)
    print("[INFO] start training")

    for epoch in range(1, args.epochs + 1):
        train_loss = run_one_epoch(
            model,
            train_loader,
            criterion,
            optimizer,
            device,
            args.target_mode,
        )

        val_loss = evaluate(
            model,
            val_loader,
            criterion,
            device,
            args.target_mode,
        )

        scheduler.step(val_loss)

        if val_loss < best_val_loss:
            best_val_loss = val_loss

            torch.save(
                {
                    "model_state_dict": model.state_dict(),
                    "target_mode": args.target_mode,
                    "global_mean": dm.stats.global_mean,
                    "global_std": dm.stats.global_std,
                    "tower_mean": dm.stats.tower_mean,
                    "tower_std": dm.stats.tower_std,
                    "target_mean": dm.stats.target_mean,
                    "target_std": dm.stats.target_std,
                    "args": vars(args),
                },
                best_path,
            )

        if epoch == 1 or epoch % 10 == 0:
            lr = optimizer.param_groups[0]["lr"]
            print(
                f"Epoch {epoch:04d} | "
                f"train loss = {train_loss:.6e} | "
                f"val loss = {val_loss:.6e} | "
                f"best val = {best_val_loss:.6e} | "
                f"lr = {lr:.3e}"
            )

    print(f"[INFO] best model saved to: {best_path}")

    ckpt = torch.load(best_path, map_location=device)
    model.load_state_dict(ckpt["model_state_dict"])

    result = test_resolution(
        model,
        test_loader,
        dm,
        device,
        args.target_mode,
    )

    plot_pred_vs_true(
        model,
        test_loader,
        dm,
        device,
        args.target_mode,
        args.outdir,
    )

    print("\n[TEST resolution in original physical units]")

    if args.target_mode == "dphi":
        print(f"before dphi mean = {result['before_mean'][0].item():.6e}")
        print(f"before dphi std  = {result['before_std'][0].item():.6e}")
        print(f"after  dphi mean = {result['after_mean'][0].item():.6e}")
        print(f"after  dphi std  = {result['after_std'][0].item():.6e}")

    elif args.target_mode == "dR":
        print(f"before dR mean = {result['before_mean'][0].item():.6e}")
        print(f"before dR std  = {result['before_std'][0].item():.6e}")
        print(f"after  dR mean = {result['after_mean'][0].item():.6e}")
        print(f"after  dR std  = {result['after_std'][0].item():.6e}")

    elif args.target_mode == "both":
        print(f"before dphi mean = {result['before_mean'][0].item():.6e}")
        print(f"before dphi std  = {result['before_std'][0].item():.6e}")
        print(f"after  dphi mean = {result['after_mean'][0].item():.6e}")
        print(f"after  dphi std  = {result['after_std'][0].item():.6e}")

        print(f"before dR mean   = {result['before_mean'][1].item():.6e}")
        print(f"before dR std    = {result['before_std'][1].item():.6e}")
        print(f"after  dR mean   = {result['after_mean'][1].item():.6e}")
        print(f"after  dR std    = {result['after_std'][1].item():.6e}")


if __name__ == "__main__":
    main()