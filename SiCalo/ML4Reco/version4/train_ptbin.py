import numpy as np
import torch
import torch.nn as nn
from torch.utils.data import DataLoader, random_split
from data import TrackCaloDataset
from model import TrackCaloRegressor
import os
import joblib
import ROOT
import matplotlib.pyplot as plt

def monotonic_loss(y_sorted):
    dy = y_sorted[1:] - y_sorted[:-1]
    # penalty = torch.relu(dy).sum()
    penalty = torch.relu(-dy).sum()
    return penalty

def train(list_file, pt_min=0.0, pt_max=2.0, batch_size=1024, epochs=500, lr=5e-5, val_ratio=0.2, device="cuda" if torch.cuda.is_available() else "cpu"):
    print(f"Training pt range: [{pt_min}, {pt_max}) GeV")
    print(f"Using device: {device}")

    out_file = ROOT.TFile("outputFile/proxy_vs_pt.root", "RECREATE")
    hist2d = ROOT.TH2D("h2_proxy_vs_pt", "proxy vs pt;proxy;pt", 510, 2, 200, 100, 0, 10)
    hist_prof = ROOT.TProfile("h_profile_proxy_vs_pt", "proxy vs pt;proxy;pt", 510, -0.1, 0.5)
    h2_p34 = ROOT.TH2D("h2_p34", "XY of p34;X (cm);Y (cm)", 300, -30, 30, 300, -30, 30)
    h2_p56 = ROOT.TH2D("h2_p56", "XY of p56;X (cm);Y (cm)", 300, -30, 30, 300, -30, 30)
    h2_calo = ROOT.TH2D("h2_calo", "XY of calo;X (cm);Y (cm)", 600, -150, 150, 600, -150, 150)
    h2_SiCalo = ROOT.TH2D("h2_SiCalo", "XY of calo;X (cm);Y (cm)", 600, -150, 150, 600, -150, 150)

    dataset = TrackCaloDataset(list_file, pt_min=pt_min, pt_max=pt_max)
    train_size = int((1 - val_ratio) * len(dataset))
    val_size = len(dataset) - train_size
    train_set, val_set = random_split(dataset, [train_size, val_size])

    train_loader = DataLoader(train_set, batch_size=batch_size, shuffle=True)
    val_loader = DataLoader(val_set, batch_size=batch_size)

    model = TrackCaloRegressor().to(device)
    optimizer = torch.optim.Adam(model.parameters(), lr=lr, weight_decay=1e-5)
    criterion = nn.MSELoss()

    best_val_loss = float("inf")

    lambda_mono = 0.3
    # lambda_boundary = 0.1

    max_draw_event = 10
    drawn_event_count = 0

    for epoch in range(epochs):
        # === train ===
        model.train()
        train_loss = 0

        for xb, yb in train_loader:
            xb, yb = xb.to(device), yb.to(device)
            pred = model(xb)

            pt_reso = (pred - yb) / (yb)
            weights = (pt_reso.abs() < 0.2).float() * 2.0 + 1.0
            main_loss = ((pt_reso) ** 2 * weights).mean()

            # === boundary loss ===
            x1 = np.array([0, 0.5, 1, 2, 10, 15, 25, 50, 100, 200])
            x2 = np.zeros_like(x1)    
            # x_boundary_np = np.stack([x1], axis=1)
            x_boundary_np = np.stack([x1, x2], axis=1)
            x_boundary = torch.tensor(x_boundary_np, dtype=torch.float32).to(device)
            y_boundary_target = torch.tensor([0, 0.0961, 0.1922, 0.3844, 1.922, 2.883, 4.805, 9.61, 19.22, 38.44], dtype=torch.float32).unsqueeze(1).to(device)

            y_boundary_pred = model(x_boundary)
            boundary_loss = nn.MSELoss()(y_boundary_pred, y_boundary_target)

            lambda_boundary = min(0.005 * epoch, 0.2)

            # === monotonic penalty ===
            x_sorted, indices = torch.sort(xb[:,0])
            pred_sorted = pred[indices]
            mono_penalty = monotonic_loss(pred_sorted)

            # === 总 loss ===
            loss = main_loss + lambda_mono * mono_penalty + lambda_boundary * boundary_loss

            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
            train_loss += loss.item() * xb.size(0)

            # === fill ROOT hist + 画图 ===
            # for i in range(xb.shape[0]):
            #     proxy = xb[i][0].item()
            #     pt = yb[i][0].item()
            #     dphi_recovered = proxy  # 你没做特征变换

            #     hist2d.Fill(proxy, pt)
            #     hist_prof.Fill(dphi_recovered, pt)

            #     x34, y34 = xb[i][0].item(), xb[i][1].item()
            #     x56, y56 = xb[i][2].item(), xb[i][3].item()
            #     xcalo, ycalo = xb[i][4].item(), xb[i][5].item()
            #     h2_p34.Fill(x34, y34)
            #     h2_p56.Fill(x56, y56)
            #     h2_calo.Fill(xcalo, ycalo)

            #     if drawn_event_count < max_draw_event:
            #         plt.figure(figsize=(6, 6))
            #         plt.scatter([x34], [y34], c='red', label='p34', s=10)
            #         plt.scatter([x56], [y56], c='green', label='p56', s=10)
            #         plt.scatter([xcalo], [ycalo], c='blue', label='calo', s=10)

            #         dx = x56 - x34
            #         dy = y56 - y34
            #         if dx != 0:
            #             k = dy / dx
            #             b = y34 - k * x34
            #             x_left = -120
            #             x_right = 120
            #             y_left = k * x_left + b
            #             y_right = k * x_right + b
            #             plt.plot([x_left, x_right], [y_left, y_right], color='black', linestyle='-', linewidth=1, label='p34->p56 extended')
            #         else:
            #             plt.plot([x34, x34], [-120, 120], color='black', linestyle='-', linewidth=1, label='p34->p56 extended')

            #         plt.plot([x56, xcalo], [y56, ycalo], color='purple', linestyle='--', linewidth=1)

            #         plt.xlim(-120, 120)
            #         plt.ylim(-120, 120)
            #         plt.xlabel("X (cm)")
            #         plt.ylabel("Y (cm)")
            #         plt.title(f"Train Event {drawn_event_count+1}, Pt = {pt:.2f} GeV")
            #         plt.legend()
            #         plt.grid(True)
            #         plt.savefig(f"event_images/train_event_{drawn_event_count+1:03d}.png")
            #         plt.close()

            #         drawn_event_count += 1

        train_loss /= len(train_loader.dataset)

        # === val ===
        model.eval()
        val_loss = 0
        with torch.no_grad():
            for xb, yb in val_loader:
                xb, yb = xb.to(device), yb.to(device)
                pred = model(xb)
                pt_reso = (pred - yb) / (yb)
                weights = (pt_reso.abs() < 0.2).float() * 2.0 + 1.0
                main_loss = ((pt_reso) ** 2 * weights).mean()

                x1 = np.array([0, 0.5, 1, 2, 10, 15, 25, 50, 100, 200])
                x2 = np.zeros_like(x1)    
                # x_boundary_np = np.stack([x1], axis=1)
                x_boundary_np = np.stack([x1, x2], axis=1)
                x_boundary = torch.tensor(x_boundary_np, dtype=torch.float32).to(device)
                y_boundary_target = torch.tensor([0, 0.0961, 0.1922, 0.3844, 1.922, 2.883, 4.805, 9.61, 19.22, 38.44], dtype=torch.float32).unsqueeze(1).to(device)

                y_boundary_pred = model(x_boundary)
                boundary_loss = nn.MSELoss()(y_boundary_pred, y_boundary_target)

                lambda_boundary = min(0.005 * epoch, 0.2)

                x_sorted, indices = torch.sort(xb[:,0])
                pred_sorted = pred[indices]
                mono_penalty = monotonic_loss(pred_sorted)

                loss = main_loss + lambda_mono * mono_penalty + lambda_boundary * boundary_loss

                val_loss += loss.item() * xb.size(0)

        val_loss /= len(val_loader.dataset)

        print(f"Epoch {epoch+1:03d} | Train Loss: {train_loss:.4f} | Val Loss: {val_loss:.4f}")

        if epoch >= 200 and val_loss < best_val_loss:
            best_val_loss = val_loss
            torch.save(model.state_dict(), f"model_weight/best_model_pt_{pt_min:.1f}_{pt_max:.1f}_INTT_CaloIwoE.pt")
            print(f"✓ Saved best model (val loss = {val_loss:.4f})")

    print("✅ 训练完成。最优模型保存在 best_model_*.pt")

    out_file.cd()
    hist2d.Write()
    hist_prof.Write()
    h2_p34.Write()
    h2_p56.Write()
    h2_calo.Write()
    h2_SiCalo.Write()
    out_file.Close()

if __name__ == "__main__":
    list_file = "../train500k.list"
    pt_bins = [(0, 10)]
    for pt_min, pt_max in pt_bins:
        train(list_file, pt_min=pt_min, pt_max=pt_max)
