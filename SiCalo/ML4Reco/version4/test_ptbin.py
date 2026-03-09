import torch
import ROOT
import numpy as np
import joblib
from model import TrackCaloRegressor
from data import TrackCaloDataset
from test_func import visualize_model

device = "cuda" if torch.cuda.is_available() else "cpu"
# pt_bins = [(0, 2), (2, 4), (4, 6), (6, 8), (8, 10)]
pt_bins = [(0, 10)]

out_file = ROOT.TFile("outputFile/pt_relative_error_INTT_CaloI.root", "RECREATE")

name_010 = f"h2_pt_vs_relerr_all"
title_010 = f"pt vs RelErr for 0 < pt < 10;truth pt;(pred - truth)/truth"
hist_010 = ROOT.TH2D(name_010, title_010, 100, 0, 10, 250, -2., 2.)

for pt_min, pt_max in pt_bins:
    print(f"\n=== Testing model for pt in [{pt_min}, {pt_max}) GeV ===")

    # 加载 scaler 和数据（注意要传 pt 范围）
    # scaler = joblib.load(f"model_weight/scaler_pt_{pt_min:.1f}_{pt_max:.1f}_INTT_CaloIwoE.pkl")
    # scaler = joblib.load(f"model_weight/scaler_pt_0.0_10.0_INTT_CaloI.pkl")
    dataset = TrackCaloDataset("../test500k.list", scaler=None, pt_min=pt_min, pt_max=pt_max)
    loader = torch.utils.data.DataLoader(dataset, batch_size=256)

    # 加载模型
    model = TrackCaloRegressor()
    model.load_state_dict(torch.load(f"model_weight/best_model_pt_{pt_min:.1f}_{pt_max:.1f}_INTT_CaloIwoE.pt"))
    # model.load_state_dict(torch.load(f"model_weight/best_model_pt_0.0_10.0_INTT_CaloI.pt"))
    model.to(device)
    model.eval()

    # visualize_model(model, device, x_min=0.01, x_max=0.5, num_points=500)

    # 直方图
    name = f"h_relerr_{pt_min}_{pt_max}"
    title = f"RelErr for {pt_min} < pt < {pt_max};(pred - truth)/truth;Counts"
    hist = ROOT.TH1D(name, title, 200, -1, 1)

    # 2D 直方图
    name2d = f"h2_pt_vs_relerr_{pt_min}_{pt_max}"
    title2d = f"pt vs RelErr for {pt_min} < pt < {pt_max};truth pt;(pred - truth)/truth"
    nbins_2dy = (pt_max - pt_min) * 10
    hist2d = ROOT.TH2D(name2d, title2d, int(nbins_2dy), float(pt_min), float(pt_max), 250, -2., 2.)

    # 推理并填充
    with torch.no_grad():
        for x, y in loader:
            x = x.to(device)
            y = y.to(device)
            pred = model(x)

            y_np = y.cpu().numpy().flatten()
            pred_np = pred.cpu().numpy().flatten()
            rel_err = (pred_np - y_np) / (y_np)

            for yi, rei in zip(y_np, rel_err):
                hist.Fill(rei)
                hist2d.Fill(yi, rei)
                hist_010.Fill(yi, rei)

    # hist.Fit("gaus", "", "", -0.1, 0.1)
    out_file.cd()
    hist.Write()
    hist2d.Write()

out_file.cd()    
hist_010.Write()

out_file.Close()
print("✅ 每个 pt 区间的相对误差已保存为 pt_relative_error_bybin.root")
