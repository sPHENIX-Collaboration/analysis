import matplotlib.pyplot as plt
import torch
import numpy as np
from data import TrackCaloDataset
from model import TrackCaloRegressor
import joblib
from sklearn.preprocessing import StandardScaler

# === 加载数据和模型 ===
scaler = joblib.load("scaler.pkl")  # 确保和训练保存的路径一致

dataset = TrackCaloDataset("../test50k.list", scaler=scaler)
loader = torch.utils.data.DataLoader(dataset, batch_size=128)
device = "cuda" if torch.cuda.is_available() else "cpu"

model = TrackCaloRegressor()
model.load_state_dict(torch.load("best_model.pt"))
model.to(device)
model.eval()

# === 收集所有预测和真实值 ===
all_preds = []
all_truths = []

with torch.no_grad():
    for x, y in loader:
        x = x.to(device)
        y = y.to(device)
        
        # pred = model(x)
        pred_log = model(x)
        pred = torch.exp(pred_log)  # 恢复 pt

        all_preds.append(pred.cpu().numpy())
        all_truths.append(y.cpu().numpy())

# === 合并结果 ===
all_preds = np.concatenate(all_preds).flatten()
all_truths = np.concatenate(all_truths).flatten()

# === 画图: truth vs pred ===
plt.figure(figsize=(6, 6))
plt.hist2d(all_truths, all_preds, bins=200, range=[[0, 15], [0, 15]], cmap='viridis')
plt.plot([0, 20], [0, 20], color='red', linestyle='--', label='Ideal')  # y=x 参考线
plt.xlabel("Truth pt")
plt.ylabel("Predicted pt")
plt.title("Truth vs Predicted pt")
plt.colorbar(label='Counts')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("truth_vs_pred.png", dpi=300)
plt.show()
