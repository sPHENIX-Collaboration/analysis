"""Gaussian + linear peak fit and per-candidate signal weights."""
import math
import numpy as np
from scipy.optimize import curve_fit

# ---------------------------------------------------------------- signal weights
def gauss_lin(x, N, mu, sig, a, b):
    return N * np.exp(-0.5 * ((x - mu) / sig)**2) + a + b * x


def fit_peak(masses, mu0, sig0, lo, hi, nbins=100, weights=None):
    """Gaussian+linear fit with floating peak. Returns (mu, sig, popt)."""
    h, edges = np.histogram(masses, bins=nbins, range=(lo, hi), weights=weights)
    c = 0.5 * (edges[:-1] + edges[1:])
    p0 = [h.max() - np.median(h), mu0, sig0, np.median(h), 0.0]
    try:
        popt, _ = curve_fit(gauss_lin, c, h, p0=p0,
                            sigma=np.sqrt(np.maximum(h, 1)), maxfev=20000)
        if not (lo < popt[1] < hi) or not (0.2 * sig0 < abs(popt[2]) < 5 * sig0):
            raise RuntimeError
    except Exception:
        popt = p0  # fall back to seed; weights degrade gracefully
    return popt[1], abs(popt[2]), popt


def fit_peak_with_error(masses, mu0, sig0, lo, hi, nbins=80):
    """Gaussian+linear fit for slice diagnostics, including fit covariance.

    Returns (mu, sigma, sigma_mu, sigma_sigma, cov_mu_sigma).  Unlike
    fit_peak(), a failed or unphysical slice fit returns NaNs rather than the
    seed values, so bad bins are not drawn as apparently valid points.
    """
    h, edges = np.histogram(masses, bins=nbins, range=(lo, hi))
    c = 0.5 * (edges[:-1] + edges[1:])
    p0 = [h.max() - np.median(h), mu0, sig0, np.median(h), 0.0]
    try:
        popt, pcov = curve_fit(
            gauss_lin, c, h, p0=p0,
            sigma=np.sqrt(np.maximum(h, 1)), absolute_sigma=True,
            maxfev=20000
        )
        mu = float(popt[1])
        sig_signed = float(popt[2])
        sig = abs(sig_signed)
        if not (lo < mu < hi) or not (0.2 * sig0 < sig < 5.0 * sig0):
            raise RuntimeError
        if pcov.shape != (5, 5) or not np.all(np.isfinite(pcov)):
            raise RuntimeError
        emu = math.sqrt(max(float(pcov[1, 1]), 0.0))
        esig = math.sqrt(max(float(pcov[2, 2]), 0.0))
        # sigma is reported as |popt[2]|, so carry the sign into its covariance.
        cov_mu_sig = math.copysign(1.0, sig_signed) * float(pcov[1, 2])
        return mu, sig, emu, esig, cov_mu_sig
    except Exception:
        return (float("nan"),) * 5


def signal_weights(masses, popt):
    s = popt[0] * np.exp(-0.5 * ((masses - popt[1]) / popt[2])**2)
    b = np.maximum(popt[3] + popt[4] * masses, 0.0)
    w = s / np.maximum(s + b, 1e-9)
    return np.clip(w, 0.0, 1.0)
