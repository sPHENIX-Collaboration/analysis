from __future__ import annotations

import math
from dataclasses import dataclass
from typing import Dict, List, Optional

import numpy as np
import torch
from torch.utils.data import Dataset, DataLoader, random_split

try:
    import uproot
except ImportError as exc:
    raise ImportError("Install with: pip install uproot awkward") from exc


PI = math.pi


def delta_phi(phi1: float, phi2: float) -> float:
    dphi = phi1 - phi2
    while dphi >= PI:
        dphi -= 2.0 * PI
    while dphi < -PI:
        dphi += 2.0 * PI
    return dphi


def calc_r(x: float, y: float) -> float:
    return float(math.sqrt(x * x + y * y))


def calc_phi(x: float, y: float) -> float:
    return float(math.atan2(y, x))


def calc_eta_from_xyz(x: float, y: float, z: float) -> float:
    r = math.sqrt(x * x + y * y)
    if r <= 0.0:
        return 0.0
    return float(np.arcsinh(z / r))


@dataclass
class ECPMFeatureStats:
    global_mean: torch.Tensor
    global_std: torch.Tensor
    tower_mean: torch.Tensor
    tower_std: torch.Tensor
    target_mean: torch.Tensor
    target_std: torch.Tensor


class ECPMDataset(Dataset):
    global_feature_names = [
        "clus_E",
        "n_tower",
        "clus_R",
        "clus_eta",
        "sin_clus_phi",
        "cos_clus_phi",
        "PrimaryG4P_Pt",
    ]

    tower_feature_names = [
        "tower_dphi",
        "tower_dR",
        "tower_deta",
        "tower_Efrac",
    ]

    target_names = ["target_dphi", "target_dR"]

    def __init__(
        self,
        root_file: str,
        tree_name: str = "tree",
        require_single_emcal_cluster: bool = True,
        require_single_cog: bool = True,
        min_tower_e: float = 0.0,
        max_abs_target_dphi: Optional[float] = None,
        stats: Optional[ECPMFeatureStats] = None,
        normalize: bool = True,
        verbose: bool = True,
    ):
        super().__init__()

        self.root_file = root_file
        self.tree_name = tree_name
        self.require_single_emcal_cluster = require_single_emcal_cluster
        self.require_single_cog = require_single_cog
        self.min_tower_e = min_tower_e
        self.max_abs_target_dphi = max_abs_target_dphi
        self.stats = stats
        self.normalize = normalize

        self.samples: List[Dict[str, object]] = []

        self.counters: Dict[str, int] = {
            "total": 0,
            "used": 0,
            "skip_no_single_emcal_cluster": 0,
            "skip_no_single_cog": 0,
            "skip_bad_tower_index": 0,
            "skip_empty_tower": 0,
            "skip_nonfinite": 0,
            "skip_large_abs_dphi": 0,
        }

        self._load_root()

        if verbose:
            print("[ECPMDataset] load summary")
            for k, v in self.counters.items():
                print(f"  {k:32s}: {v}")
            print(f"  global features: {self.global_feature_names}")
            print(f"  tower features : {self.tower_feature_names}")
            print(f"  targets        : {self.target_names}")

    def _load_root(self) -> None:
        branches = [
            "caloClus_system",
            "caloClus_X",
            "caloClus_Y",
            "caloClus_Z",
            "caloClus_edep",
            "calo_tower_e",
            "calo_tower_x",
            "calo_tower_y",
            "calo_tower_z",
            "CEMC_Hit_CoG_x",
            "CEMC_Hit_CoG_y",
            "CEMC_Hit_CoG_z",
            "PrimaryG4P_Pt",
        ]

        with uproot.open(self.root_file) as f:
            tree = f[self.tree_name]
            arr = tree.arrays(branches, library="np")

        nentries = len(arr["caloClus_system"])
        self.counters["total"] = nentries

        for iev in range(nentries):
            clus_system = arr["caloClus_system"][iev]

            emcal_clus_index = -1
            emcal_tower_index = -1
            n_emcal_clus = 0

            for ic, system in enumerate(clus_system):
                if int(system) == 0:
                    if emcal_clus_index < 0:
                        emcal_clus_index = ic
                        emcal_tower_index = n_emcal_clus
                    n_emcal_clus += 1

            if self.require_single_emcal_cluster and n_emcal_clus != 1:
                self.counters["skip_no_single_emcal_cluster"] += 1
                continue

            if emcal_clus_index < 0:
                self.counters["skip_no_single_emcal_cluster"] += 1
                continue

            cog_x_vec = arr["CEMC_Hit_CoG_x"][iev]
            cog_y_vec = arr["CEMC_Hit_CoG_y"][iev]
            cog_z_vec = arr["CEMC_Hit_CoG_z"][iev]

            if self.require_single_cog and not (
                len(cog_x_vec) == len(cog_y_vec) == len(cog_z_vec) == 1
            ):
                self.counters["skip_no_single_cog"] += 1
                continue

            if len(cog_x_vec) < 1:
                self.counters["skip_no_single_cog"] += 1
                continue

            tower_e_all = arr["calo_tower_e"][iev]

            if emcal_tower_index < 0 or emcal_tower_index >= len(tower_e_all):
                self.counters["skip_bad_tower_index"] += 1
                continue

            tw_e = np.asarray(list(arr["calo_tower_e"][iev][emcal_tower_index]), dtype=np.float32)
            tw_x = np.asarray(list(arr["calo_tower_x"][iev][emcal_tower_index]), dtype=np.float32)
            tw_y = np.asarray(list(arr["calo_tower_y"][iev][emcal_tower_index]), dtype=np.float32)
            tw_z = np.asarray(list(arr["calo_tower_z"][iev][emcal_tower_index]), dtype=np.float32)

            nt = len(tw_e)

            if not (len(tw_x) == len(tw_y) == len(tw_z) == nt):
                self.counters["skip_bad_tower_index"] += 1
                continue

            keep = tw_e > self.min_tower_e

            tw_e = tw_e[keep]
            tw_x = tw_x[keep]
            tw_y = tw_y[keep]
            tw_z = tw_z[keep]

            nt = len(tw_e)

            if nt < 1:
                self.counters["skip_empty_tower"] += 1
                continue

            clus_e = float(arr["caloClus_edep"][iev][emcal_clus_index])
            clus_x = float(arr["caloClus_X"][iev][emcal_clus_index])
            clus_y = float(arr["caloClus_Y"][iev][emcal_clus_index])
            clus_z = float(arr["caloClus_Z"][iev][emcal_clus_index])

            clus_r = calc_r(clus_x, clus_y)
            clus_phi = calc_phi(clus_x, clus_y)
            clus_eta = calc_eta_from_xyz(clus_x, clus_y, clus_z)

            cog_x = float(cog_x_vec[0])
            cog_y = float(cog_y_vec[0])
            cog_z = float(cog_z_vec[0])

            cog_r = calc_r(cog_x, cog_y)
            cog_phi = calc_phi(cog_x, cog_y)
            cog_eta = calc_eta_from_xyz(cog_x, cog_y, cog_z)

            pt_vec = arr["PrimaryG4P_Pt"][iev]
            if len(pt_vec) != 1:
                self.counters["skip_no_single_cog"] += 1
                continue
            
            primary_pt = float(pt_vec[0])
            
            sum_e = float(np.sum(tw_e))

            if sum_e <= 0.0 or not np.isfinite(sum_e):
                self.counters["skip_empty_tower"] += 1
                continue

            tw_r = np.sqrt(tw_x * tw_x + tw_y * tw_y).astype(np.float32)
            tw_phi = np.arctan2(tw_y, tw_x).astype(np.float32)
            tw_eta = np.arcsinh(tw_z / np.maximum(tw_r, 1.0e-8)).astype(np.float32)

            tower_dphi = np.array(
                [delta_phi(float(p), clus_phi) for p in tw_phi],
                dtype=np.float32,
            )

            tower_dR = tw_r - np.float32(clus_r)
            tower_deta = tw_eta - np.float32(clus_eta)
            tower_Efrac = tw_e / np.float32(sum_e)

            tower_features = np.stack(
                [
                    tower_dphi,
                    tower_dR,
                    tower_deta,
                    tower_Efrac,
                ],
                axis=1,
            ).astype(np.float32)

            global_features = np.array(
                [
                    clus_e,
                    float(nt),
                    clus_r,
                    clus_eta,
                    math.sin(clus_phi),
                    math.cos(clus_phi),
                    primary_pt,
                ],
                dtype=np.float32,
            )

            target = np.array(
                [
                    delta_phi(cog_phi, clus_phi),
                    # delta_phi(cog_phi, clus_phi) - dphi_peak,
                    cog_r - clus_r,
                ],
                dtype=np.float32,
            )
            if self.max_abs_target_dphi is not None:
                if abs(float(target[0])) > self.max_abs_target_dphi:
                    self.counters["skip_large_abs_dphi"] += 1
                    continue

            if not (
                np.all(np.isfinite(global_features))
                and np.all(np.isfinite(tower_features))
                and np.all(np.isfinite(target))
            ):
                self.counters["skip_nonfinite"] += 1
                continue

            self.samples.append(
                {
                    "global": torch.tensor(global_features, dtype=torch.float32),
                    "towers": torch.tensor(tower_features, dtype=torch.float32),
                    "target": torch.tensor(target, dtype=torch.float32),
                    "meta": {
                        "event": iev,
                        "n_tower": nt,
                        "clus_x": clus_x,
                        "clus_y": clus_y,
                        "clus_z": clus_z,
                        "clus_r": clus_r,
                        "clus_phi": clus_phi,
                        "clus_eta": clus_eta,
                        "cog_x": cog_x,
                        "cog_y": cog_y,
                        "cog_z": cog_z,
                        "cog_r": cog_r,
                        "cog_phi": cog_phi,
                        "cog_eta": cog_eta,
                        "primary_pt": primary_pt,
                    },
                }
            )

            self.counters["used"] += 1

    def __len__(self) -> int:
        return len(self.samples)

    def __getitem__(self, idx: int) -> Dict[str, object]:
        item = self.samples[idx]

        g = item["global"].clone()
        t = item["towers"].clone()
        y = item["target"].clone()

        if self.normalize and self.stats is not None:
            g = (g - self.stats.global_mean) / self.stats.global_std
            t = (t - self.stats.tower_mean) / self.stats.tower_std
            y = (y - self.stats.target_mean) / self.stats.target_std

        return {
            "global": g,
            "towers": t,
            "target": y,
            "meta": item["meta"],
        }

    def compute_stats(self, indices: Optional[List[int]] = None) -> ECPMFeatureStats:
        if indices is None:
            indices = list(range(len(self.samples)))

        globals_ = torch.stack([self.samples[i]["global"] for i in indices], dim=0)
        targets_ = torch.stack([self.samples[i]["target"] for i in indices], dim=0)
        towers_ = torch.cat([self.samples[i]["towers"] for i in indices], dim=0)

        eps = 1.0e-8

        return ECPMFeatureStats(
            global_mean=globals_.mean(dim=0),
            global_std=globals_.std(dim=0).clamp_min(eps),
            tower_mean=towers_.mean(dim=0),
            tower_std=towers_.std(dim=0).clamp_min(eps),
            target_mean=targets_.mean(dim=0),
            target_std=targets_.std(dim=0).clamp_min(eps),
        )

    def denormalize_target(self, y: torch.Tensor) -> torch.Tensor:
        if self.stats is None or not self.normalize:
            return y

        return (
            y * self.stats.target_std.to(y.device)
            + self.stats.target_mean.to(y.device)
        )


def ecpm_collate_fn(batch: List[Dict[str, object]]) -> Dict[str, object]:
    batch_size = len(batch)
    max_ntower = max(item["towers"].shape[0] for item in batch)
    tower_dim = batch[0]["towers"].shape[1]

    global_x = torch.stack([item["global"] for item in batch], dim=0)
    target = torch.stack([item["target"] for item in batch], dim=0)

    towers = torch.zeros(batch_size, max_ntower, tower_dim, dtype=torch.float32)
    mask = torch.zeros(batch_size, max_ntower, dtype=torch.bool)

    for ib, item in enumerate(batch):
        nt = item["towers"].shape[0]
        towers[ib, :nt, :] = item["towers"]
        mask[ib, :nt] = True

    return {
        "global": global_x,
        "towers": towers,
        "tower_mask": mask,
        "target": target,
        "meta": [item["meta"] for item in batch],
    }


class ECPMDataModule:
    def __init__(
        self,
        root_file: str,
        tree_name: str = "tree",
        batch_size: int = 128,
        train_frac: float = 0.8,
        val_frac: float = 0.1,
        num_workers: int = 0,
        seed: int = 12345,
        normalize: bool = True,
        min_tower_e: float = 0.0,
        max_abs_target_dphi: Optional[float] = None,
    ):
        self.root_file = root_file
        self.tree_name = tree_name
        self.batch_size = batch_size
        self.train_frac = train_frac
        self.val_frac = val_frac
        self.num_workers = num_workers
        self.seed = seed
        self.normalize = normalize
        self.min_tower_e = min_tower_e
        self.max_abs_target_dphi = max_abs_target_dphi

        self.dataset: Optional[ECPMDataset] = None
        self.train_set = None
        self.val_set = None
        self.test_set = None
        self.stats: Optional[ECPMFeatureStats] = None

    def setup(self) -> None:
        dataset = ECPMDataset(
            root_file=self.root_file,
            tree_name=self.tree_name,
            min_tower_e=self.min_tower_e,
            max_abs_target_dphi=self.max_abs_target_dphi,
            normalize=False,
            verbose=True,
        )

        n = len(dataset)
        n_train = int(self.train_frac * n)
        n_val = int(self.val_frac * n)
        n_test = n - n_train - n_val

        generator = torch.Generator().manual_seed(self.seed)

        train_set, val_set, test_set = random_split(
            dataset,
            [n_train, n_val, n_test],
            generator=generator,
        )

        train_indices = list(train_set.indices)

        self.stats = dataset.compute_stats(train_indices)
        dataset.stats = self.stats
        dataset.normalize = self.normalize

        self.dataset = dataset
        self.train_set = train_set
        self.val_set = val_set
        self.test_set = test_set

        print("[ECPMDataModule] split")
        print(f"  train: {n_train}")
        print(f"  val  : {n_val}")
        print(f"  test : {n_test}")

    def train_dataloader(self) -> DataLoader:
        return DataLoader(
            self.train_set,
            batch_size=self.batch_size,
            shuffle=True,
            num_workers=self.num_workers,
            collate_fn=ecpm_collate_fn,
        )

    def val_dataloader(self) -> DataLoader:
        return DataLoader(
            self.val_set,
            batch_size=self.batch_size,
            shuffle=False,
            num_workers=self.num_workers,
            collate_fn=ecpm_collate_fn,
        )

    def test_dataloader(self) -> DataLoader:
        return DataLoader(
            self.test_set,
            batch_size=self.batch_size,
            shuffle=False,
            num_workers=self.num_workers,
            collate_fn=ecpm_collate_fn,
        )


if __name__ == "__main__":
    dm = ECPMDataModule(
        root_file="/mnt/e/sphenix/EMshowerPositionModify/Dataset/results_test.root",
        tree_name="tree",
        batch_size=8,
        normalize=True,
    )

    dm.setup()

    batch = next(iter(dm.train_dataloader()))

    print("batch global     :", batch["global"].shape)
    print("batch towers     :", batch["towers"].shape)
    print("batch tower_mask :", batch["tower_mask"].shape)
    print("batch target     :", batch["target"].shape)
    print("first meta       :", batch["meta"][0])