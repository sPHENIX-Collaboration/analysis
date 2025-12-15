#!/usr/bin/env python3
"""
export_to_onnx.py
---------------------
Export a trained PyTorch TrackCaloRegressor to ONNX and dump StandardScaler to JSON,
using ABSOLUTE PATHS so it works no matter where you run it from.

python export_to_onnx.py \
    --model-dir   /abs/path/to/your/code/dir \
    --weights     /abs/path/to/model_weight/best_model_pt_0to10.pt \
    --scaler      /abs/path/to/model_weight/scaler_0to10.joblib \
    --out         /abs/path/to/out/model.onnx \
    --json        /abs/path/to/out/scaler.json \
    --in-dim 7

Examples:
    python export_to_onnx.py \
        --model-dir   /mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/version2 \
        --weights     /mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/version2/model_weight/best_model_pt_0.0_10.0_INTT_CaloIwoE.pt \
        --scaler      /mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/version2/model_weight/scaler_pt_0.0_10.0_INTT_CaloIwoE.pkl \
        --out         /mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/Implement/ML_Weight_Scaler/model_MLEMD.onnx \
        --json        /mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/Implement/ML_Weight_Scaler/scaler_MLEMD.json \
        --in-dim 7

    python export_to_onnx.py \
        --model-dir   /mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/version4 \
        --weights     /mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/version4/model_weight/best_model_pt_0.0_10.0_INTT_CaloIwoE.pt \
        --scaler      /mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/version4/ML_Weight_Scaler/scaler_identity.pkl \
        --out         /mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/Implement/ML_Weight_Scaler/model_MLEproj.onnx \
        --json        /mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/Implement/ML_Weight_Scaler/scaler_MLEproj.json \
        --in-dim 2

    python export_to_onnx.py \
       --model-dir   /mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/combine2_gate \
       --weights     /mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/combine2_gate/model_weight/best_model_combined.pt \
       --scaler      /mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/combine2_gate/ML_Weight_Scaler/scaler_identity.pkl \
       --out         /mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/Implement/ML_Weight_Scaler/model_MLCombined.onnx \
       --json        /mnt/e/sphenix/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/Implement/ML_Weight_Scaler/scaler_MLCombined.json \
       --in-dim 2
        
Notes:
- --model-dir should be the directory that contains model.py (with TrackCaloRegressor).
- All other paths should be absolute. The script will create parent folders for outputs.
"""
import argparse
import json
import sys
from pathlib import Path

def abspath(p: str, must_exist: bool=False) -> Path:
    path = Path(p).expanduser().absolute()
    if must_exist and not path.exists():
        raise FileNotFoundError(f"Path does not exist: {path}")
    return path

def main():
    ap = argparse.ArgumentParser(description="Export TrackCaloRegressor to ONNX (absolute paths)")
    ap.add_argument("--model-dir", required=True, help="Absolute path to directory containing model.py")
    ap.add_argument("--weights",   required=True, help="Absolute path to .pt/.pth weights (state_dict)")
    ap.add_argument("--scaler",    required=True, help="Absolute path to StandardScaler .joblib")
    ap.add_argument("--out",       required=True, help="Absolute path to output model.onnx")
    ap.add_argument("--json",      required=True, help="Absolute path to output scaler.json")
    ap.add_argument("--in-dim",    type=int, default=7, help="Input feature dimension (default 7)")
    ap.add_argument("--hidden-dim",type=int, default=256, help="Hidden size, if customized")
    # ap.add_argument("--class-name",default="TrackCaloRegressor", help="Model class name in model.py")
    ap.add_argument("--class-name",default="FusionRegressor", help="Model class name in model.py")
    args = ap.parse_args()

    # Resolve and validate paths
    model_dir = abspath(args.model_dir, must_exist=True)
    weights   = abspath(args.weights, must_exist=True)
    scalerjb  = abspath(args.scaler,  must_exist=False)
    out_onnx  = abspath(args.out,     must_exist=False)
    out_json  = abspath(args.json,    must_exist=False)

    # Ensure output directories exist
    out_onnx.parent.mkdir(parents=True, exist_ok=True)
    out_json.parent.mkdir(parents=True, exist_ok=True)

    # Add model_dir to sys.path BEFORE importing model.py
    if str(model_dir) not in sys.path:
        sys.path.insert(0, str(model_dir))

    # Now import the model class dynamically
    try:
        import importlib
        # model_module = importlib.import_module("model")
        model_module = importlib.import_module("model_combined")
        ModelClass = getattr(model_module, args.class_name)
    except Exception as e:
        print(f"[ERROR] Failed to import '{args.class_name}' from {model_dir}/model.py: {e}", file=sys.stderr)
        sys.exit(2)

    # Torch & IO
    import torch, joblib, numpy as np

    # Build model and load weights
    model = ModelClass(input_dim=args.in_dim, hidden_dim=args.hidden_dim) if "hidden_dim" in ModelClass.__init__.__code__.co_varnames else ModelClass(input_dim=args.in_dim)
    try:
        ckpt = torch.load(str(weights), map_location="cpu")
        if isinstance(ckpt, dict) and "state_dict" in ckpt:
            state_dict = ckpt["state_dict"]
        elif isinstance(ckpt, dict):
            # Likely a raw state_dict
            state_dict = ckpt
        else:
            raise RuntimeError(f"Unsupported checkpoint object: {type(ckpt)}")
        model.load_state_dict(state_dict, strict=False)
    except Exception as e:
        print(f"[ERROR] Failed to load weights: {e}", file=sys.stderr)
        sys.exit(3)
    model.eval()

    # Export ONNX (dynamic batch)
    try:
        dummy = torch.randn(1, args.in_dim, dtype=torch.float32)
        torch.onnx.export(
            model, dummy, str(out_onnx),
            input_names=["x"], output_names=["pt_hat"],
            dynamic_axes={"x": {0: "N"}, "pt_hat": {0: "N"}},
            opset_version=13,
        )
        print(f"[OK] Exported ONNX -> {out_onnx}")
    except Exception as e:
        print(f"[ERROR] Failed to export ONNX: {e}", file=sys.stderr)
        sys.exit(4)

    # Dump scaler JSON
    from pathlib import Path
    if not Path(scalerjb).exists():
        print(f"[INFO] Scaler file not found: {scalerjb}. Skip writing scaler JSON.")
        # 直接跳过整个 scaler 处理
        return
    
    try:
        scaler = joblib.load(str(scalerjb))
        required = ["mean_", "scale_", "n_features_in_"]
        for a in required:
            if not hasattr(scaler, a):
                raise AttributeError(f"Scaler missing '{a}'. Is it sklearn StandardScaler?")
        payload = {
            "mean": np.asarray(scaler.mean_, dtype=float).tolist(),
            "scale": np.asarray(scaler.scale_, dtype=float).tolist(),
            "n_features": int(scaler.n_features_in_),
        }
        out_json.write_text(json.dumps(payload, indent=2))
        print(f"[OK] Wrote scaler JSON -> {out_json} (n_features={payload['n_features']})")
        if payload["n_features"] != args.in_dim:
            print(f"[WARN] scaler n_features ({payload['n_features']}) != --in-dim ({args.in_dim})", file=sys.stderr)
    except Exception as e:
        print(f"[ERROR] Failed to write scaler JSON: {e}", file=sys.stderr)
        sys.exit(5)

if __name__ == "__main__":
    main()
