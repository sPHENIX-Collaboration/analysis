import torch
import torch.nn as nn
from torch.utils.data import DataLoader, random_split
from data import TrackCaloDataset
from model import TrackCaloRegressor
import os
import joblib

def train(list_file, batch_size=1024, epochs=200, lr=5e-5, val_ratio=0.3, device="cuda" if torch.cuda.is_available() else "cpu"):
    print(f"Using device: {device}")

    dataset = TrackCaloDataset(list_file)
    train_size = int((1 - val_ratio) * len(dataset))
    val_size = len(dataset) - train_size
    train_set, val_set = random_split(dataset, [train_size, val_size])

    train_loader = DataLoader(train_set, batch_size=batch_size, shuffle=True)
    val_loader = DataLoader(val_set, batch_size=batch_size)

    model = TrackCaloRegressor().to(device)
    optimizer = torch.optim.Adam(model.parameters(), lr=lr)
    criterion = nn.MSELoss()

    best_val_loss = float("inf")

    for epoch in range(epochs):
        model.train()
        train_loss = 0
        for xb, yb in train_loader:
            xb, yb = xb.to(device), yb.to(device)
            pred = model(xb)

            # loss function
            # pt_reso = (pred - yb) / (yb + 1e-3)
            # pt_weights = torch.clamp(1.0 / (yb + 1e-3), max=10.0)
            # loss = (pt_reso**2 * pt_weights).mean()

            pt_reso = (yb - pred) / (yb + 1e-6)
            weights = (pt_reso.abs() < 0.2).float() * 2.0 + 1.0  # 主峰权重变为 3.0，其它为 1.0
            loss = ((pt_reso) ** 2 * weights).mean()

            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
            train_loss += loss.item() * xb.size(0)
        train_loss /= len(train_loader.dataset)

        model.eval()
        val_loss = 0
        with torch.no_grad():
            for xb, yb in val_loader:
                xb, yb = xb.to(device), yb.to(device)
                pred = model(xb)

                # pt_reso = (pred - yb) / (yb + 1e-3)
                # pt_weights = torch.clamp(1.0 / (yb + 1e-3), max=10.0)
                # loss = (pt_reso**2 * pt_weights).mean()

                pt_reso = (yb - pred) / (yb + 1e-6)
                weights = (pt_reso.abs() < 0.2).float() * 2.0 + 1.0  # 主峰权重变为 3.0，其它为 1.0
                loss = ((pt_reso) ** 2 * weights).mean()

                val_loss += loss.item() * xb.size(0)
        val_loss /= len(val_loader.dataset)

        print(f"Epoch {epoch+1:03d} | Train Loss: {train_loss:.4f} | Val Loss: {val_loss:.4f}")

        if val_loss < best_val_loss:
            best_val_loss = val_loss
            torch.save(model.state_dict(), "best_model.pt")
            print(f"✓ Saved best model (val loss = {val_loss:.4f})")

    torch.save(model.state_dict(), "final_model.pt")
    print("✅ 训练完成。最优模型保存在 best_model.pt, and final_model.pt")

    # === 保存 scaler（标准化器） ===
    joblib.dump(train_set.dataset.scaler, "scaler.pkl")
    print("✅ 标准化器 scaler.pkl 已保存")

if __name__ == "__main__":
    list_file = "../train500k.list"
    if not os.path.exists(list_file):
        print(f"❌ 找不到 {list_file}")
    else:
        train(list_file)
