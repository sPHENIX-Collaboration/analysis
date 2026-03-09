### train_combined.py
import torch
import torch.nn as nn
from torch.utils.data import DataLoader, random_split
import ROOT

from data_combined import FusionDataset
from model_combined import FusionRegressor


def train_fusion(
    list_file,
    pt_min=0.0,
    pt_max=10.0,
    batch_size=512,
    epochs=1,
    lr=5e-5,
    weight_decay=1e-5,
    hidden_dim=32,
    device="cuda" if torch.cuda.is_available() else "cpu"
):
    print(f"=== Training FusionRegressor [{pt_min},{pt_max}) GeV on {device} ===")

    # —— 准备数据集并划分 —— #
    dataset     = FusionDataset(list_file, pt_min=pt_min, pt_max=pt_max)
    train_size  = int(0.7 * len(dataset))
    val_size    = len(dataset) - train_size
    train_set, val_set = random_split(dataset, [train_size, val_size])

    train_loader = DataLoader(train_set, batch_size=batch_size, shuffle=True)
    val_loader   = DataLoader(val_set,   batch_size=batch_size)

    # —— 模型与优化器 —— #
    model     = FusionRegressor(input_dim=2, hidden_dim=hidden_dim).to(device)
    optimizer = torch.optim.Adam(model.parameters(), lr=lr, weight_decay=weight_decay)
    criterion = nn.MSELoss(reduction="none")

    best_val = float('inf')
    for epoch in range(1, epochs+1):
        model.train()
        train_loss = 0
        for xb, yb in train_loader:
            xb, yb = xb.to(device), yb.to(device)
            pred = model(xb)

            pt_reso = (pred - yb) / (yb)
            weights = (pt_reso.abs() < 0.2).float() * 2.0 + 1.0
            loss = ((pt_reso) ** 2 * weights).mean()
            # loss = (pt_reso ** 2).mean()

            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
            train_loss += loss.item() * xb.size(0)
        train_loss /= len(train_loader.dataset)

        model.eval()
        val_loss = 0.0
        with torch.no_grad():
            for xb, yb in val_loader:
                xb, yb = xb.to(device), yb.to(device)
                pred = model(xb)
                pt_reso = (pred - yb) / (yb)
                weights = (pt_reso.abs() < 0.2).float() * 2.0 + 1.0
                loss = ((pt_reso) ** 2 * weights).mean()

                val_loss += loss.item() * xb.size(0)

        val_loss /= len(val_loader.dataset)
        
        print(f"Epoch {epoch:03d} | Train: {train_loss:.4f} | Val: {val_loss:.4f}")

        if val_loss < best_val:
            best_val = val_loss
            torch.save(model.state_dict(), "model_weight/best_model_combined.pt")
            print(f"✓ Saved best model (Val {val_loss:.4f})")

    print("✅ Training complete.")

    # —— 在训练结束后，用 dataset.X (pred1,pred2) 和 dataset.Y (truth) 填充两个 TH2D —— #
    # 创建直方图
    nbins_pt = int((pt_max - pt_min) * 10)
    h2_dphi = ROOT.TH2D(
        "h2_dphi_diff",
        "dphi-model: truth pt vs (truth-pred);truth pt [GeV];truth-pred [GeV]",
        nbins_pt, pt_min, pt_max, 200, -2.0, 2.0
    )
    h2_energy = ROOT.TH2D(
        "h2_energy_diff",
        "energy-model: truth pt vs (truth-pred);truth pt [GeV];truth-pred [GeV]",
        nbins_pt, pt_min, pt_max, 200, -2.0, 2.0
    )

    # 从 FusionDataset 中获取预测和真实值填图，并打印前几条用于调试
    for i in range(len(dataset.X)):
        truth = dataset.Y[i].item()
        pred1 = dataset.X[i, 0].item()
        pred2 = dataset.X[i, 1].item()
        # 打印前10条样本的预测和真实值
        if i < 2:
            print(f"[Sample {i}] pred1={pred1:.4f}, pred2={pred2:.4f}, truth={truth:.4f}")
        # 填充相对误差直方图
        h2_dphi.Fill(truth, (pred1 - truth) / truth)
        h2_energy.Fill(truth, (pred2 - truth) / truth)

    # 写入 ROOT 文件
    fout = ROOT.TFile("outputFile/submodel_performance.root", "RECREATE")
    h2_dphi.Write()
    h2_energy.Write()
    fout.Close()
    print("✅ Sub-model histograms saved to submodel_performance.root")

if __name__ == "__main__":
    train_fusion(
        "../train500k.list",
        pt_min=0.0,
        pt_max=10.0,
        batch_size=512,
        epochs=100,
        lr=5e-5,
        weight_decay=1e-5,
        hidden_dim=256
    )