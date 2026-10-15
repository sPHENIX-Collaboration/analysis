#!/usr/bin/env python3
"""
raw_time_utils.py

Shared helpers for the raw-tower-time based ±5 ns ZS cross-calibration, used by both
generate_zs_crosscalib_cdb.py (production) and plot_calib_validation.py (validation):
- FileCatalog lookup of the segment-0 HIST_JETQA file per run (latest by file time).
- Loading mean raw tower time maps from h_CaloValid_<det>_etaphi_time_raw [ADC samples].
- Polynomial fit of the scale factor SF_win = H_win / T0 Integrated against raw time, and clamped prediction.
"""

from typing import Dict, List, Optional, Tuple

import numpy as np
import uproot

# Tower grid and CaloValid histogram prefix per detector
TIME_HIST_GEOMETRY = {
    "CEMC": {"neta": 96, "nphi": 256, "hist_det": "cemc"},
    "HCALIN": {"neta": 24, "nphi": 64, "hist_det": "ihcal"},
    "HCALOUT": {"neta": 24, "nphi": 64, "hist_det": "ohcal"},
}

# FileCatalog dataset type holding the CaloValid raw time histograms
TIME_HIST_DSTTYPE = "HIST_JETQA%"

# Sample windows calibrated by the raw-time polynomial fit (Main stays on the candidate-run method)
TIME_FIT_WINDOWS = ("minus5", "plus5")

# Polynomial order of the SF(t) fit. Cubic improves the low-time tail over quadratic; quartic adds nothing.
TIME_FIT_ORDER = 3


def time_fit_label(order: int = TIME_FIT_ORDER) -> str:
    """Human-readable name for a polynomial order (e.g. 'Cubic')."""
    return {1: "Linear", 2: "Quadratic", 3: "Cubic"}.get(order, f"Order-{order} polynomial")


def query_time_hist_paths(runs: List[int], dsttype_pattern: str = TIME_HIST_DSTTYPE) -> Dict[int, Tuple[str, str]]:
    """
    Look up the segment-0 QA hist file per run in FileCatalog, keeping the latest by file time.
    Connection host/user come from the standard PGHOST / PGUSER environment. Returns {run: (tag, path)}.
    """
    import psycopg2

    query = """
        SELECT DISTINCT ON (d.runnumber) d.runnumber, d.tag, f.full_file_path
        FROM datasets d JOIN files f ON d.filename = f.lfn
        WHERE d.runnumber = ANY(%s) AND d.segment = 0 AND d.dsttype LIKE %s
        ORDER BY d.runnumber, f.time DESC NULLS LAST
    """
    conn = psycopg2.connect(dbname="FileCatalog")
    try:
        with conn.cursor() as cur:
            cur.execute(query, (list(runs), dsttype_pattern))
            return {int(run): (tag, path) for run, tag, path in cur.fetchall()}
    finally:
        conn.close()


def load_time_map(det: str, hist_file: str) -> Optional[np.ndarray]:
    """Load mean raw tower time [samples] as an (neta, nphi) array, NaN where a tower has no entries."""
    cfg = TIME_HIST_GEOMETRY[det]
    neta, nphi = cfg["neta"], cfg["nphi"]
    hname = f"h_CaloValid_{cfg['hist_det']}_etaphi_time_raw"

    try:
        with uproot.open(hist_file) as f:
            if hname not in f:
                print(f"Warning: {hname} missing in {hist_file}.")
                return None
            h = f[hname]
            vals = h.values()
            if vals.shape != (neta, nphi):
                return None
            # Empty bins read as 0, which is also a valid time, so mask on entries instead.
            # uproot's TProfile2D.counts() fails here; fBinEntries is ROOT-ordered (x fastest, with flow bins).
            entries = np.asarray(h.member("fBinEntries")).reshape((nphi + 2, neta + 2)).T[1:-1, 1:-1]
        valid = (entries > 0) & np.isfinite(vals)
        if not np.any(valid):
            return None
        return np.where(valid, vals, np.nan)
    except Exception as e:
        print(f"Time map load error for {det} from {hist_file}: {e}")
        return None


def fit_sf_vs_time_poly(t: np.ndarray, sf: np.ndarray, order: int = TIME_FIT_ORDER) -> Optional[np.ndarray]:
    """Least-squares polynomial SF(t) over towers valid in both. Returns np.polyfit coefficients (highest power first) or None."""
    ok = np.isfinite(t) & np.isfinite(sf)
    if np.sum(ok) <= order:
        return None
    return np.polyfit(t[ok], sf[ok], order)


def predict_sf_from_time(coef: np.ndarray, t: np.ndarray, t_range: Tuple[float, float]) -> np.ndarray:
    """
    Evaluate the fitted SF(t) with t clamped to the fit's time range, so the polynomial is never extrapolated.
    Towers without a valid time, or with a non-positive prediction, return NaN (caller falls back).
    """
    pred = np.polyval(coef, np.clip(t, *t_range))
    return np.where(np.isfinite(t) & np.isfinite(pred) & (pred > 0), pred, np.nan)
