import torch
import ROOT
import numpy as np

from model_combined import FusionRegressor
from data_combined import FusionDataset

def test_fusion(
    list_file,
    model_path="model_weight/best_model_combined.pt",
    output_path="outputFile/pt_relative_error_combined.root",
    pt_bins=[(0.0, 10.0)],
    batch_size=512,
    device="cuda" if torch.cuda.is_available() else "cpu"
):
    print(f"Using device: {device}")
    out_file = ROOT.TFile(output_path, "RECREATE")

    for pt_min, pt_max in pt_bins:
        print(f"\n=== Testing FusionRegressor for pt in [{pt_min}, {pt_max}) GeV ===")
        # —— 数据集 & DataLoader —— #
        dataset = FusionDataset(list_file, pt_min=pt_min, pt_max=pt_max)
        loader  = torch.utils.data.DataLoader(dataset, batch_size=batch_size, shuffle=False)

        # —— 模型加载 —— #
        model = FusionRegressor(hidden_dim=256).to(device)
        model.load_state_dict(torch.load(model_path, map_location=device))
        model.eval()

        # —— 准备直方图 —— #
        nbins_pt = int((pt_max - pt_min) * 10)
        hist2d = ROOT.TH2D(
            f"h2_pt_vs_relerr_{pt_min}_{pt_max}",
            f"pt vs RelErr Combined;truth pt;(pred - truth)/truth",
            nbins_pt, pt_min, pt_max,
            250, -2.0, 2.0
        )
        hist1d = ROOT.TH1D(
            f"h_relerr_{pt_min}_{pt_max}",
            f"RelErr Combined;(pred - truth)/truth;Counts",
            200, -1.0, 1.0
        )

        # —— 推理并填图 —— #
        with torch.no_grad():
            for xb, yb in loader:
                xb, yb = xb.to(device), yb.to(device)
                pred = model(xb)

                y_np    = yb.cpu().numpy().flatten()
                pred_np = pred.cpu().numpy().flatten()

                # （可选）对 reco_pt>=8.8 做后处理
                # for i, reco_pt in enumerate(pred_np):
                #     if reco_pt >= 8.8:
                #         cf = 0.02 + 0.08 * (reco_pt - 8.8)
                #         pred_np[i] = reco_pt * (1.0 + cf)

                rel_err = (pred_np - y_np) / y_np

                for truth_pt, rei in zip(y_np, rel_err):
                    hist1d.Fill(rei)
                    hist2d.Fill(truth_pt, rei)

        # —— 写入文件 —— #
        out_file.cd()
        hist1d.Write()
        hist2d.Write()

    out_file.Close()
    print(f"✅ Test complete. Results saved to {output_path}")


if __name__ == "__main__":
    test_fusion(
        list_file="../test500k.list",
        model_path="model_weight/best_model_combined.pt",
        output_path="outputFile/pt_relative_error_combined.root",
        pt_bins=[(0.0, 10.0)],
        batch_size=512
    )
