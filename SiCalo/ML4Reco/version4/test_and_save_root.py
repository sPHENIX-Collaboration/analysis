import torch
import ROOT
import numpy as np
import joblib
from model import TrackCaloRegressor
from data import TrackCaloDataset

# ==== 加载 scaler ====
scaler = joblib.load("model_weight/scaler.pkl")

# ==== 加载测试数据 ====
dataset = TrackCaloDataset("../test500k.list", scaler=scaler)
loader = torch.utils.data.DataLoader(dataset, batch_size=128)
device = "cuda" if torch.cuda.is_available() else "cpu"

# ==== 加载模型 ====
model = TrackCaloRegressor()
model.load_state_dict(torch.load("model_weight/best_model.pt"))
model.to(device)
model.eval()

# ==== 设置 pt 区间划分 ====
pt_bins = [(0, 3), (3, 6), (6, 10), (10, 15)]
hists = {}
for (ptmin, ptmax) in pt_bins:
    name = f"h_relerr_{ptmin}_{ptmax}"
    title = f"RelErr for {ptmin} < pt < {ptmax};(pred - truth)/truth;Counts"
    hists[(ptmin, ptmax)] = ROOT.TH1D(name, title, 200, -1, 1)

# ==== 推理并按区间填充 ====
with torch.no_grad():
    for x, y in loader:
        x = x.to(device)
        y = y.to(device)
        pred = model(x)

        y_np = y.cpu().numpy().flatten()
        pred_np = pred.cpu().numpy().flatten()
        rel_err = (pred_np - y_np) / (y_np + 1e-6)

        for yi, rei in zip(y_np, rel_err):
            for (ptmin, ptmax) in pt_bins:
                if ptmin <= yi < ptmax:
                    hists[(ptmin, ptmax)].Fill(rei)
                    break

# ==== 拟合 & 保存 ====
out_file = ROOT.TFile("outputFile/pt_relative_error_bybin.root", "RECREATE")
for (ptmin, ptmax), hist in hists.items():
    hist.Fit("gaus", "", "", -0.1, 0.1)
    hist.Write()
out_file.Close()

print("✅ 分 pt 区间的相对误差直方图已保存到 pt_relative_error_bybin.root")
