### data_combined.py
import torch
from torch.utils.data import Dataset
import numpy as np
import joblib
import ROOT

from data_dphi import TrackCaloDataset as dphiDataset
from data_energy import TrackCaloDataset as energyDataset
from model_dphi import TrackCaloRegressor as dphiRegressor
from model_energy import TrackCaloRegressor as energyRegressor

class FusionDataset(Dataset):
    def __init__(self,
                 list_file,
                 pt_min=0.0,
                 pt_max=10.0,
                 device="cuda" if torch.cuda.is_available() else "cpu"):
        self.device = device
        self.pt_min  = pt_min
        self.pt_max  = pt_max

        # —— 加载并冻结子模型 —— #
        model_dphi = dphiRegressor().to(device)
        model_dphi.load_state_dict(torch.load(
            "/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/version4/"
            "model_weight/best_model_pt_0.0_10.0_INTT_CaloIwoE.pt",
            map_location=device
        ))
        model_dphi.eval()

        model_energy = energyRegressor().to(device)
        model_energy.load_state_dict(torch.load(
            "/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/version2/"
            "model_weight/best_model_pt_0.0_10.0_INTT_CaloIwoE.pt",
            map_location=device
        ))
        model_energy.eval()

        # —— 加载 energy 子模型的 scaler —— #
        scaler_energy = joblib.load(
            "/mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/version2/"
            "model_weight/scaler_pt_0.0_10.0_INTT_CaloIwoE.pkl"
        )

        # —— 准备原始子数据集 —— #
        ds_dphi   = dphiDataset(list_file, pt_min=pt_min, pt_max=pt_max)
        ds_energy = energyDataset(list_file, scaler=scaler_energy, pt_min=pt_min, pt_max=pt_max)

        assert len(ds_dphi) == len(ds_energy)
        N = len(ds_dphi)

        # —— 构造融合特征 & 标签 & 子模型直方图 —— #
        X_fusion = []
        Y_fusion = []

        # —— 遍历填充 —— #
        with torch.no_grad():
            for i in range(N):
                x1, y  = ds_dphi[i]
                x2, _  = ds_energy[i]
                truth  = y.item()

                # 输出前两个样本的原始特征和真值用于检查
                if i < 2:
                    print(f"[Sample {i}] x1={x1.tolist()}, x2={x2.tolist()}, truth={truth:.4f}")

                # 子模型预测
                pt_pred1 = model_dphi(x1.unsqueeze(0).to(device)).cpu().item()
                pt_pred2 = model_energy(x2.unsqueeze(0).to(device)).cpu().item()

                X_fusion.append([pt_pred1, pt_pred2])
                Y_fusion.append(truth)

        # —— 转为 Tensor —— #
        self.X = torch.tensor(np.array(X_fusion), dtype=torch.float32)
        self.Y = torch.tensor(np.array(Y_fusion), dtype=torch.float32).view(-1, 1)

    def __len__(self):
        return len(self.X)

    def __getitem__(self, idx):
        return self.X[idx], self.Y[idx]