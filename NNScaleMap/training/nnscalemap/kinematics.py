"""Two-body kinematics (invariant mass, mass Jacobians, beta) and (pT, eta, phi) binning."""
import numpy as np
import torch

from .constants import ETA_RANGE, FEATURE_KINDS

# ---------------------------------------------------------------- feature map
def feature_raw(kind, pt):
    """Momentum variable x(pT) fed to KappaNet.  Works for torch or numpy.

    Monotone in pT for every kind, hence information-preserving; the choice
    only sets where the network's smoothness prior and the clamp act.
    """
    if kind == "inv_pt":
        return 1.0 / pt
    if kind == "pt":
        return pt * 1.0
    if kind == "log_pt":
        return torch.log(pt) if torch.is_tensor(pt) else np.log(pt)
    raise RuntimeError(f"unknown feature kind '{kind}' (expected {FEATURE_KINDS})")


# ---------------------------------------------------------------- kinematics
def inv_mass(pt1, eta1, phi1, m1, pt2, eta2, phi2, m2):
    px1, py1, pz1 = pt1 * torch.cos(phi1), pt1 * torch.sin(phi1), pt1 * torch.sinh(eta1)
    px2, py2, pz2 = pt2 * torch.cos(phi2), pt2 * torch.sin(phi2), pt2 * torch.sinh(eta2)
    e1 = torch.sqrt(px1**2 + py1**2 + pz1**2 + m1**2)
    e2 = torch.sqrt(px2**2 + py2**2 + pz2**2 + m2**2)
    m2v = (e1 + e2)**2 - (px1 + px2)**2 - (py1 + py2)**2 - (pz1 + pz2)**2
    return torch.sqrt(m2v.clamp(min=1e-12))


def _p4(pt, eta, phi, m):
    px, py, pz = pt * torch.cos(phi), pt * torch.sin(phi), pt * torch.sinh(eta)
    p2 = px * px + py * py + pz * pz
    return px, py, pz, torch.sqrt(p2 + m * m), p2


def mass_jacobians(pt1, eta1, phi1, m1, pt2, eta2, phi2, m2):
    """(m, J1, J2), J_i = dm/dln(pT_i) at fixed (eta, phi); not leg-symmetric."""
    px1, py1, pz1, E1, p1sq = _p4(pt1, eta1, phi1, m1)
    px2, py2, pz2, E2, p2sq = _p4(pt2, eta2, phi2, m2)
    msq = (E1 + E2)**2 - (px1 + px2)**2 - (py1 + py2)**2 - (pz1 + pz2)**2
    m = torch.sqrt(msq.clamp(min=1e-12))
    pdot = px1 * px2 + py1 * py2 + pz1 * pz2
    J1 = (E2 * p1sq / E1 - pdot) / m
    J2 = (E1 * p2sq / E2 - pdot) / m
    return m, J1, J2


def beta_of(pt, eta, m):
    p = pt * torch.cosh(eta)
    return p / torch.sqrt(p * p + m * m)


# ---------------------------------------------------------------- binning
def semi_even_pt_grid(pt_min, pt_max, n):
    """Human-readable pT grid: dense below 3 GeV, sparse at the high end."""
    anchors = np.array([0.10, 0.25, 0.50, 0.75, 1.00, 1.25, 1.50, 1.75,
                        2.00, 2.25, 2.50, 2.75, 3.00, 4.00, 5.00], dtype=float)
    lo, hi = float(pt_min), float(pt_max)
    vals = anchors[(anchors >= lo) & (anchors <= hi)]
    vals = np.unique(np.concatenate(([lo], vals, [hi])))
    if vals.size == n:
        return vals
    q = np.linspace(0.0, 1.0, n)
    return np.interp(q, np.linspace(0.0, 1.0, vals.size), vals)


def bin_index(pt, eta, phi, pt_edges, n_eta, n_phi):
    ipt = torch.bucketize(pt, pt_edges) - 1
    ipt = ipt.clamp(0, len(pt_edges) - 2)
    ieta = ((eta - ETA_RANGE[0]) / (ETA_RANGE[1] - ETA_RANGE[0]) * n_eta).long().clamp(0, n_eta - 1)
    iphi = ((phi + np.pi) / (2 * np.pi) * n_phi).long().clamp(0, n_phi - 1)
    return (ipt * n_eta + ieta) * n_phi + iphi, (len(pt_edges) - 1) * n_eta * n_phi


def binned_mean_sq(idx, nbins, values, weights, min_w=5.0):
    """sum_b w_b * mean_b^2 / sum_b w_b, weighted means via scatter-add.

    Returns (value, coef) with coef_i = d value / d values_i, so the exact
    gradient can be accumulated over candidate chunks as sum_i coef_i * values_i.
    """
    sw = torch.zeros(nbins, dtype=values.dtype).index_add_(0, idx, weights)
    swv = torch.zeros(nbins, dtype=values.dtype).index_add_(0, idx, weights * values)
    mask = sw > min_w
    mean = torch.zeros(nbins, dtype=values.dtype)
    mean[mask] = swv[mask] / sw[mask]
    norm = sw[mask].sum().clamp(min=1e-9)
    value = (sw[mask] * mean[mask]**2).sum() / norm
    return value, 2.0 * mean[idx] * weights / norm
