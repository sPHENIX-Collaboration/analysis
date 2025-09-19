import torch
import ROOT
import numpy as np
from model_pointnet import PointNetRegressor  # 你的 PointNet 模型
from data_pointnet import TrackPointNetDataset  # 你的 Dataset

device = "cuda" if torch.cuda.is_available() else "cpu"
pt_bins = [(0, 10)]  # 你可以扩展为多个 pt 区间

out_file = ROOT.TFile("outputFile/pt_relative_error_INTT_CaloI_pointnet.root", "RECREATE")

# 汇总直方图
hist_010 = ROOT.TH2D("h2_pt_vs_relerr_all", "pt vs RelErr for 0 < pt < 10;truth pt;(pred - truth)/truth",
                     100, 0, 10, 250, -2., 2.)

for pt_min, pt_max in pt_bins:
    print(f"\n=== Testing PointNet for pt in [{pt_min}, {pt_max}) GeV ===")

    dataset = TrackPointNetDataset("../test50k.list", pt_min=pt_min, pt_max=pt_max)
    loader = torch.utils.data.DataLoader(dataset, batch_size=256)


    # 加载 PointNet 模型
    model = PointNetRegressor()
    model.load_state_dict(torch.load(f"model_weight/best_pointnet_pt_{pt_min:.1f}_{pt_max:.1f}.pt"))
    model.to(device)
    model.eval()

    # 相对误差直方图
    hist = ROOT.TH1D(f"h_relerr_{pt_min}_{pt_max}",
                     f"RelErr for {pt_min} < pt < {pt_max};(pred - truth)/truth;Counts",
                     200, -1, 1)

    # pt vs 相对误差 2D图
    nbins_2dy = (pt_max - pt_min) * 10
    hist2d = ROOT.TH2D(f"h2_pt_vs_relerr_{pt_min}_{pt_max}",
                       f"pt vs RelErr for {pt_min} < pt < {pt_max};truth pt;(pred - truth)/truth",
                       nbins_2dy, pt_min, pt_max, 250, -2., 2.)

    # 推理 + 填充
    with torch.no_grad():
        for x, y in loader:
            x = x.to(device)
            y = y.to(device)
            pred = model(x)

            y_np = y.cpu().numpy().flatten()
            pred_np = pred.cpu().numpy().flatten()
            rel_err = (pred_np - y_np) / y_np

            for yi, rei in zip(y_np, rel_err):
                hist.Fill(rei)
                hist2d.Fill(yi, rei)
                hist_010.Fill(yi, rei)

    hist.Fit("gaus", "", "", -0.1, 0.1)
    hist.Write()
    hist2d.Write()

hist_010.Write()
out_file.Close()

print("✅ 每个 pt 区间的 PointNet 相对误差已保存为 pt_relative_error_INTT_CaloI_pointnet.root")
