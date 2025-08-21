import torch
import ROOT
import numpy as np
import joblib
from model import TrackCaloRegressor
from data import TrackCaloDataset

# ==== 加载 scaler ====
scaler = joblib.load("scaler.pkl")

# ==== 加载测试数据 ====
dataset = TrackCaloDataset("../test50k.list", scaler=scaler)
loader = torch.utils.data.DataLoader(dataset, batch_size=128)
device = "cuda" if torch.cuda.is_available() else "cpu"

# ==== 加载模型 ====
model = TrackCaloRegressor()
model.load_state_dict(torch.load("best_model.pt"))
model.to(device)
model.eval()

# ==== 创建直方图 ====
h1 = ROOT.TH1D("h_relative_error", "Relative Error: (pred - truth)/truth;Rel Error;Counts", 400, -2, 2)

# ==== 推理并填充 ====
with torch.no_grad():
    for x, y in loader:
        x = x.to(device)
        y = y.to(device)
        pred_log = model(x)
        pred = torch.exp(pred_log)  # 恢复 pt
        rel_err = ((pred - y) / y).cpu().numpy().flatten()
        for val in rel_err:
            h1.Fill(val)

# ==== 高斯拟合并保存 ====
h1.Fit("gaus", "", "", -0.05, 0.10)
fit_func = h1.GetFunction("gaus")

out_file = ROOT.TFile("pt_relative_error_logreco.root", "RECREATE")
h1.Write()
if fit_func:
    fit_func.Write()
out_file.Close()

print("✅ log(pt) 模型测试完成，已写入 pt_relative_error.root")
