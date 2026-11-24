from __future__ import annotations

import argparse
import math
import os
from array import array
from typing import Dict, Tuple

import numpy as np
import torch
from torch.utils.data import DataLoader
import uproot
import ROOT

from ecpm_data import ECPMDataset, ECPMFeatureStats, ecpm_collate_fn
from ecpm_model import build_ecpm_model


PI = math.pi


def safe_torch_load(path: str, map_location):
    try:
        return torch.load(path, map_location=map_location, weights_only=False)
    except TypeError:
        return torch.load(path, map_location=map_location)


def wrap_phi(phi: np.ndarray) -> np.ndarray:
    return (phi + np.pi) % (2.0 * np.pi) - np.pi


def get_device(device_arg: str) -> torch.device:
    if device_arg == "auto":
        return torch.device("cuda" if torch.cuda.is_available() else "cpu")
    if device_arg.startswith("cuda") and not torch.cuda.is_available():
        print("[WARN] CUDA requested but not available. Use CPU instead.")
        return torch.device("cpu")
    return torch.device(device_arg)


def build_stats_from_ckpt(ckpt: Dict[str, object]) -> ECPMFeatureStats:
    return ECPMFeatureStats(
        global_mean=ckpt["global_mean"].cpu(),
        global_std=ckpt["global_std"].cpu(),
        tower_mean=ckpt["tower_mean"].cpu(),
        tower_std=ckpt["tower_std"].cpu(),
        target_mean=ckpt["target_mean"].cpu(),
        target_std=ckpt["target_std"].cpu(),
    )


@torch.no_grad()
def predict_one_target(
    root_file: str,
    tree_name: str,
    ckpt_path: str,
    target_mode_expected: str,
    batch_size: int,
    min_tower_e: float,
    max_abs_target_dphi,
    device: torch.device,
) -> Tuple[np.ndarray, np.ndarray]:
    """
    Return:
        event_indices: original TTree entry index for selected events
        pred_values  : predicted target in physical unit

    target_mode_expected is "dphi" or "dR".
    """
    ckpt = safe_torch_load(ckpt_path, map_location=device)
    target_mode = ckpt.get("target_mode", target_mode_expected)

    if target_mode != target_mode_expected:
        raise ValueError(
            f"Checkpoint {ckpt_path} has target_mode={target_mode}, "
            f"but expected {target_mode_expected}"
        )

    stats = build_stats_from_ckpt(ckpt)

    dataset = ECPMDataset(
        root_file=root_file,
        tree_name=tree_name,
        min_tower_e=min_tower_e,
        max_abs_target_dphi=max_abs_target_dphi,
        stats=stats,
        normalize=True,
        verbose=True,
    )

    loader = DataLoader(
        dataset,
        batch_size=batch_size,
        shuffle=False,
        num_workers=0,
        collate_fn=ecpm_collate_fn,
    )

    model = build_ecpm_model(target_mode=target_mode).to(device)
    model.load_state_dict(ckpt["model_state_dict"])
    model.eval()

    event_indices = []
    pred_values = []

    for batch in loader:
        global_x = batch["global"].to(device)
        towers = batch["towers"].to(device)
        tower_mask = batch["tower_mask"].to(device)

        pred_norm = model(global_x, towers, tower_mask)

        pred_full = torch.zeros(pred_norm.shape[0], 2, device=device)
        if target_mode == "dphi":
            pred_full[:, 0:1] = pred_norm
            pred_phys = dataset.denormalize_target(pred_full)[:, 0]
        elif target_mode == "dR":
            pred_full[:, 1:2] = pred_norm
            pred_phys = dataset.denormalize_target(pred_full)[:, 1]
        else:
            raise ValueError(f"Unsupported target_mode: {target_mode}")

        event_indices.extend([int(m["event"]) for m in batch["meta"]])
        pred_values.extend(pred_phys.detach().cpu().numpy().astype(np.float64).tolist())

    return np.asarray(event_indices, dtype=np.int64), np.asarray(pred_values, dtype=np.float64)


def find_single_emcal_cluster(arr: Dict[str, object], n_entries: int) -> np.ndarray:
    """
    Return the index of the unique EMCal cluster for each event.
    If not unique, return -1.
    """
    idx = np.full(n_entries, -1, dtype=np.int64)

    systems_all = arr["caloClus_system"]
    for iev in range(n_entries):
        systems = np.asarray(systems_all[iev], dtype=np.int64)
        emcal_indices = np.where(systems == 0)[0]
        if len(emcal_indices) == 1:
            idx[iev] = int(emcal_indices[0])

    return idx


def write_clonetree_with_new_branches(
    input_file: str,
    output_file: str,
    tree_name: str,
    pred_dphi: np.ndarray,
    pred_dR: np.ndarray,
    corr_x: np.ndarray,
    corr_y: np.ndarray,
    corr_z: np.ndarray,
    corr_R: np.ndarray,
    corr_phi: np.ndarray,
    raw_x: np.ndarray,
    raw_y: np.ndarray,
    raw_z: np.ndarray,
    raw_R: np.ndarray,
    raw_phi: np.ndarray,
    raw_e: np.ndarray,
    valid: np.ndarray,
) -> None:
    """
    Use PyROOT CloneTree to preserve all original branches, including
    vector<vector<...>> branches that uproot cannot rewrite easily.
    """
    fin = ROOT.TFile.Open(input_file, "READ")
    if not fin or fin.IsZombie():
        raise RuntimeError(f"Cannot open input ROOT file: {input_file}")

    tin = fin.Get(tree_name)
    if not tin:
        fin.ls()
        raise RuntimeError(f"Cannot find tree: {tree_name}")

    fout = ROOT.TFile.Open(output_file, "RECREATE")
    if not fout or fout.IsZombie():
        raise RuntimeError(f"Cannot create output ROOT file: {output_file}")

    tout = tin.CloneTree(0)

    b_pred_dphi = array("f", [0.0])
    b_pred_dR = array("f", [0.0])
    b_x = array("f", [0.0])
    b_y = array("f", [0.0])
    b_z = array("f", [0.0])
    b_R = array("f", [0.0])
    b_phi = array("f", [0.0])
    b_raw_x = array("f", [0.0])
    b_raw_y = array("f", [0.0])
    b_raw_z = array("f", [0.0])
    b_raw_R = array("f", [0.0])
    b_raw_phi = array("f", [0.0])
    b_e = array("f", [0.0])
    b_valid = array("i", [0])

    tout.Branch("caloCorr_pred_dphi", b_pred_dphi, "caloCorr_pred_dphi/F")
    tout.Branch("caloCorr_pred_dR", b_pred_dR, "caloCorr_pred_dR/F")
    tout.Branch("caloCorr_X", b_x, "caloCorr_X/F")
    tout.Branch("caloCorr_Y", b_y, "caloCorr_Y/F")
    tout.Branch("caloCorr_Z", b_z, "caloCorr_Z/F")
    tout.Branch("caloCorr_R", b_R, "caloCorr_R/F")
    tout.Branch("caloCorr_Phi", b_phi, "caloCorr_Phi/F")
    tout.Branch("caloCorr_raw_X", b_raw_x, "caloCorr_raw_X/F")
    tout.Branch("caloCorr_raw_Y", b_raw_y, "caloCorr_raw_Y/F")
    tout.Branch("caloCorr_raw_Z", b_raw_z, "caloCorr_raw_Z/F")
    tout.Branch("caloCorr_raw_R", b_raw_R, "caloCorr_raw_R/F")
    tout.Branch("caloCorr_raw_Phi", b_raw_phi, "caloCorr_raw_Phi/F")
    tout.Branch("caloCorr_E", b_e, "caloCorr_E/F")
    tout.Branch("caloCorr_isValid", b_valid, "caloCorr_isValid/I")

    n_entries = tin.GetEntries()
    for iev in range(n_entries):
        tin.GetEntry(iev)

        b_pred_dphi[0] = float(pred_dphi[iev])
        b_pred_dR[0] = float(pred_dR[iev])
        b_x[0] = float(corr_x[iev])
        b_y[0] = float(corr_y[iev])
        b_z[0] = float(corr_z[iev])
        b_R[0] = float(corr_R[iev])
        b_phi[0] = float(corr_phi[iev])
        b_raw_x[0] = float(raw_x[iev])
        b_raw_y[0] = float(raw_y[iev])
        b_raw_z[0] = float(raw_z[iev])
        b_raw_R[0] = float(raw_R[iev])
        b_raw_phi[0] = float(raw_phi[iev])
        b_e[0] = float(raw_e[iev])
        b_valid[0] = int(valid[iev])

        tout.Fill()

    fout.cd()
    tout.Write()
    fout.Close()
    fin.Close()


def main():
    parser = argparse.ArgumentParser(
        description=(
            "Read an ECPM ROOT tree, apply trained dphi/dR correction models, "
            "and write a new ROOT file with corrected EMCal cluster position branches."
        )
    )

    parser.add_argument(
        "--input",
        type=str,
        default="../Dataset/ECPM_ana527_Electron_5GeV_1M_train.root",
        help="Input ROOT file",
    )
    parser.add_argument(
        "--output",
        type=str,
        default="../Dataset/ECPM_ana527_Electron_5GeV_1M_train_corr.root",
        help="Output ROOT file",
    )
    parser.add_argument("--tree-name", type=str, default="tree")

    parser.add_argument(
        "--ckpt-dphi",
        type=str,
        default="ECPM_out/best_ecpm_dphi.pt",
        help="Checkpoint trained with --target-mode dphi",
    )
    parser.add_argument(
        "--ckpt-dR",
        type=str,
        default="ECPM_out/best_ecpm_dR.pt",
        help="Checkpoint trained with --target-mode dR",
    )

    parser.add_argument("--batch-size", type=int, default=256)
    parser.add_argument("--min-tower-e", type=float, default=0.0)
    parser.add_argument(
        "--max-abs-target-dphi",
        type=float,
        default=None,
        help=(
            "Same clean-sample cut as testing/training. Use -1 to disable. "
            "For pure application to all events, usually use -1."
        ),
    )
    parser.add_argument("--device", type=str, default="auto", help="auto, cpu, cuda, cuda:0, ...")
    parser.add_argument(
        "--overwrite",
        action="store_true",
        help="Allow overwriting output file if it already exists.",
    )

    args = parser.parse_args()

    if os.path.exists(args.output) and not args.overwrite:
        raise FileExistsError(f"Output file already exists: {args.output}. Use --overwrite to replace it.")

    max_abs_target_dphi = args.max_abs_target_dphi
    if max_abs_target_dphi is not None and max_abs_target_dphi < 0:
        max_abs_target_dphi = None

    device = get_device(args.device)
    print(f"[INFO] device = {device}")

    print("[INFO] predict dphi correction")
    idx_dphi, pred_dphi = predict_one_target(
        root_file=args.input,
        tree_name=args.tree_name,
        ckpt_path=args.ckpt_dphi,
        target_mode_expected="dphi",
        batch_size=args.batch_size,
        min_tower_e=args.min_tower_e,
        max_abs_target_dphi=max_abs_target_dphi,
        device=device,
    )

    print("[INFO] predict dR correction")
    idx_dR, pred_dR = predict_one_target(
        root_file=args.input,
        tree_name=args.tree_name,
        ckpt_path=args.ckpt_dR,
        target_mode_expected="dR",
        batch_size=args.batch_size,
        min_tower_e=args.min_tower_e,
        max_abs_target_dphi=max_abs_target_dphi,
        device=device,
    )

    with uproot.open(args.input) as fin:
        tree = fin[args.tree_name]
        arr = tree.arrays(
            [
                "caloClus_system",
                "caloClus_X",
                "caloClus_Y",
                "caloClus_Z",
                "caloClus_edep",
            ],
            library="ak",
        )
        n_entries = tree.num_entries

    pred_dphi_full = np.full(n_entries, np.nan, dtype=np.float64)
    pred_dR_full = np.full(n_entries, np.nan, dtype=np.float64)

    pred_dphi_full[idx_dphi] = pred_dphi
    pred_dR_full[idx_dR] = pred_dR

    valid_corr = np.isfinite(pred_dphi_full) & np.isfinite(pred_dR_full)

    emcal_idx = find_single_emcal_cluster(arr, n_entries)

    raw_x = np.full(n_entries, np.nan, dtype=np.float64)
    raw_y = np.full(n_entries, np.nan, dtype=np.float64)
    raw_z = np.full(n_entries, np.nan, dtype=np.float64)
    raw_e = np.full(n_entries, np.nan, dtype=np.float64)

    for iev in range(n_entries):
        ic = emcal_idx[iev]
        if ic < 0:
            continue
        raw_x[iev] = float(arr["caloClus_X"][iev][ic])
        raw_y[iev] = float(arr["caloClus_Y"][iev][ic])
        raw_z[iev] = float(arr["caloClus_Z"][iev][ic])
        raw_e[iev] = float(arr["caloClus_edep"][iev][ic])

    raw_R = np.sqrt(raw_x * raw_x + raw_y * raw_y)
    raw_phi = np.arctan2(raw_y, raw_x)

    corr_phi = wrap_phi(raw_phi + pred_dphi_full)
    corr_R = raw_R + pred_dR_full

    corr_x = corr_R * np.cos(corr_phi)
    corr_y = corr_R * np.sin(corr_phi)
    corr_z = raw_z.copy()

    valid_corr &= np.isfinite(raw_R)
    valid_corr &= np.isfinite(raw_phi)
    valid_corr &= np.isfinite(raw_z)
    valid_corr &= np.isfinite(corr_R)
    valid_corr &= corr_R > 0.0

    corr_x[~valid_corr] = np.nan
    corr_y[~valid_corr] = np.nan
    corr_z[~valid_corr] = np.nan
    corr_R[~valid_corr] = np.nan
    corr_phi[~valid_corr] = np.nan

    print("[INFO] write output with PyROOT CloneTree")
    write_clonetree_with_new_branches(
        input_file=args.input,
        output_file=args.output,
        tree_name=args.tree_name,
        pred_dphi=pred_dphi_full,
        pred_dR=pred_dR_full,
        corr_x=corr_x,
        corr_y=corr_y,
        corr_z=corr_z,
        corr_R=corr_R,
        corr_phi=corr_phi,
        raw_x=raw_x,
        raw_y=raw_y,
        raw_z=raw_z,
        raw_R=raw_R,
        raw_phi=raw_phi,
        raw_e=raw_e,
        valid=valid_corr.astype(np.int32),
    )

    print("[INFO] finished")
    print(f"  input entries          : {n_entries}")
    print(f"  dphi predicted entries : {len(idx_dphi)}")
    print(f"  dR predicted entries   : {len(idx_dR)}")
    print(f"  valid corrected entries: {int(np.sum(valid_corr))}")
    print(f"  output                 : {args.output}")
    print("[INFO] new branches:")
    print("  caloCorr_pred_dphi, caloCorr_pred_dR")
    print("  caloCorr_X, caloCorr_Y, caloCorr_Z, caloCorr_R, caloCorr_Phi")
    print("  caloCorr_raw_X, caloCorr_raw_Y, caloCorr_raw_Z, caloCorr_raw_R, caloCorr_raw_Phi")
    print("  caloCorr_E, caloCorr_isValid")


if __name__ == "__main__":
    main()
