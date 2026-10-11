"""Stage 2: resolution model (ResoNet, ResoGlobals), mass-width NLL, training, reduced global fit."""
import math
import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F

from .constants import A_SCALE, B_SCALE, C_INIT, F_MAX, K_MAX, K_MIN, LOG_SQRT_2PI, MPDG, M_K0S, M_LAMBDA, SIG0, SQRT2, S_SCALE, WIN
from .kinematics import beta_of, mass_jacobians
from .peakfit import fit_peak, signal_weights

# ================================================================ STAGE 2
class ResoNet(nn.Module):
    """(eta, sin phi, cos phi) -> fields, mode-dependent.

    absolute: two heads (a, b), r^2 = (a/beta)^2 + (b pT)^2 -- a MEASUREMENT
              of the resolution.
    pull:     one head s(eta, phi), r = s * pT^{u_p} * r_cov -- a CALIBRATION
              of the covariance-reported resolution (s == 1, u_p == 0 means
              the tracker's errors are right).
    Deliberately NOT a function of pT (identifiability); species enters only
    through beta (absolute) or through r_cov itself (pull).
    """

    def __init__(self, mode, hidden=32, layers=3):
        super().__init__()
        self.mode = mode
        n_out = 2 if mode == "absolute" else 1
        dims = [3] + [hidden] * layers + [n_out]
        seq = []
        for i in range(len(dims) - 1):
            seq.append(nn.Linear(dims[i], dims[i + 1]))
            if i < len(dims) - 2:
                seq.append(nn.SiLU())
        self.net = nn.Sequential(*seq)
        with torch.no_grad():
            self.net[-1].weight *= 0.01
            if mode == "absolute":
                # r ~ 1.5% at 1 GeV pion: a ~ b ~ 0.0106; 0.05*softplus(z)=0.0106
                self.net[-1].bias.fill_(-1.443)
            else:
                # s = S_SCALE*softplus(z) = 1  ->  softplus(z) = 0.5
                self.net[-1].bias.fill_(math.log(math.expm1(1.0 / S_SCALE)))

    def features(self, eta, phi):
        return torch.stack([eta / 1.0, torch.sin(phi), torch.cos(phi)], -1)

    def ab(self, eta, phi):
        out = self.net(self.features(eta, phi))
        return A_SCALE * F.softplus(out[..., 0]), B_SCALE * F.softplus(out[..., 1])

    def s(self, eta, phi):
        out = self.net(self.features(eta, phi))
        return S_SCALE * F.softplus(out[..., 0])


class ResoGlobals(nn.Module):
    """Floor c (1 or per-resonance), per-group (f, k), and the pull exponent u_p."""

    def __init__(self, n_groups, n_c=1):
        super().__init__()
        c_raw0 = math.log(math.expm1(C_INIT))          # softplus^-1(C_INIT)
        self.c_raw = nn.Parameter(torch.full((n_c,), c_raw0, dtype=torch.float64))
        self.f_raw = nn.Parameter(torch.full((n_groups,), -2.0, dtype=torch.float64))
        self.k_raw = nn.Parameter(torch.zeros(n_groups, dtype=torch.float64))
        self.up = nn.Parameter(torch.zeros(1, dtype=torch.float64))   # pull only

    def c(self, i=0):
        return F.softplus(self.c_raw[i])

    def f(self, i):
        return F_MAX * torch.sigmoid(self.f_raw[i])

    def k(self, i):
        return K_MIN + (K_MAX - K_MIN) * torch.sigmoid(self.k_raw[i])

    def u_p(self):
        return self.up[0]


def r_terms(a, b, pt, beta):
    return a / beta, b * pt


def sigma_mass_ab(R, a1, b1, a2, b2, c):
    t1a, t1b = r_terms(a1, b1, R["pt1"], R["beta1"])
    t2a, t2b = r_terms(a2, b2, R["pt2"], R["beta2"])
    r1sq = t1a**2 + t1b**2
    r2sq = t2a**2 + t2b**2
    return torch.sqrt((R["J1"]**2 * r1sq + R["J2"]**2 * r2sq + c**2).clamp(min=1e-12))


def pull_r(R, leg, s, up):
    return s * R[f"pt{leg}"].clamp(min=1e-6)**up * R[f"rcov{leg}"]


def sigma_mass_pull(R, s1, s2, up, c):
    r1 = pull_r(R, 1, s1, up)
    r2 = pull_r(R, 2, s2, up)
    return torch.sqrt((R["J1"]**2 * r1**2 + R["J2"]**2 * r2**2 + c**2).clamp(min=1e-12))


def sigma_mass_net(R, net, glob, c):
    if net.mode == "absolute":
        a1, b1 = net.ab(R["eta1"], R["phi1"])
        a2, b2 = net.ab(R["eta2"], R["phi2"])
        return sigma_mass_ab(R, a1, b1, a2, b2, c)
    s1 = net.s(R["eta1"], R["phi1"])
    s2 = net.s(R["eta2"], R["phi2"])
    return sigma_mass_pull(R, s1, s2, glob.u_p(), c)


def log_mixture_pdf(m, M, sigma, f, k, lo, hi):
    """ln of the window-normalised core+tail Gaussian mixture."""
    s1, s2 = sigma, k * sigma
    lg1 = -0.5 * ((m - M) / s1)**2 - torch.log(s1) - LOG_SQRT_2PI
    lg2 = -0.5 * ((m - M) / s2)**2 - torch.log(s2) - LOG_SQRT_2PI
    lp = torch.logaddexp(torch.log1p(-f) + lg1, torch.log(f) + lg2)
    frac = lambda s: 0.5 * (torch.erf((hi - M) / (s * SQRT2))
                            - torch.erf((lo - M) / (s * SQRT2)))
    Z = (1.0 - f) * frac(s1) + f * frac(s2)
    return lp - torch.log(Z.clamp(min=1e-12))


def width_nll_sum(R, M, sigma, w, f, k, lo, hi):
    return -(w * log_mixture_pdf(R["m"], M, sigma, f, k, lo, hi)).sum()


# ---------------------------------------------------------------- reso inputs
def prepare_reso_inputs(model, S):
    """Freeze the scale, precompute everything stage 2 needs (constant with
    KappaNet frozen).  Jacobians and beta use the CORRECTED momenta.  r_cov is
    carried through unchanged: it is a fractional resolution, invariant under
    the kappa scale to first order (and identical in pT and curvature)."""
    with torch.no_grad():
        k1, _, _ = model.kappa(S["pt1"], S["eta1"], S["phi1"], S["q1"])
        k2, _, _ = model.kappa(S["pt2"], S["eta2"], S["phi2"], S["q2"])
        pt1, pt2 = k1 * S["pt1"], k2 * S["pt2"]
        m, J1, J2 = mass_jacobians(pt1, S["eta1"], S["phi1"], S["m1"],
                                   pt2, S["eta2"], S["phi2"], S["m2"])
        if "dm" in S:            # injection toys only: explicit floor smearing
            m = m + S["dm"]
        R = dict(m=m, J1=J1, J2=J2,
                 pt1=pt1, eta1=S["eta1"], phi1=S["phi1"],
                 pt2=pt2, eta2=S["eta2"], phi2=S["phi2"],
                 beta1=beta_of(pt1, S["eta1"], S["m1"]),
                 beta2=beta_of(pt2, S["eta2"], S["m2"]),
                 cpt=S["cpt"], ceta=S["ceta"], cphi=S["cphi"],
                 m1=S["m1"], m2=S["m2"])
        for key in ("rcov1", "rcov2"):
            if key in S:
                R[key] = S[key]
    return R


def subset_R(R, sel):
    out = {}
    for k, v in R.items():
        out[k] = v[sel] if torch.is_tensor(v) else v
    return out


def build_groups(model, K, L, pure_signal=False):
    """Three groups: K0s, Lambda, Lambdabar (own tail params; shared ResoNet)."""
    RK = prepare_reso_inputs(model, K)
    RL = prepare_reso_inputs(model, L)

    if pure_signal:
        wK = torch.ones_like(RK["m"])
        wL = torch.ones_like(RL["m"])
    else:
        _, _, pK = fit_peak(RK["m"].numpy(), M_K0S, SIG0["K0s"], *WIN["K0s"])
        _, _, pL = fit_peak(RL["m"].numpy(), M_LAMBDA, SIG0["Lambda"], *WIN["Lambda"])
        wK = torch.tensor(signal_weights(RK["m"].numpy(), pK))
        wL = torch.tensor(signal_weights(RL["m"].numpy(), pL))

    isl = L["is_lam"]
    groups = [
        dict(name="K0s",       R=RK,                  w=wK,        c_idx=0),
        dict(name="Lambda",    R=subset_R(RL, isl),   w=wL[isl],   c_idx=0),
        dict(name="Lambdabar", R=subset_R(RL, ~isl),  w=wL[~isl],  c_idx=0),
    ]
    for g in groups:
        g["M"] = MPDG[g["name"]]
        g["lo"], g["hi"] = WIN[g["name"]]
    return groups


def set_c_indices(groups, per_resonance):
    if not per_resonance:
        for g in groups:
            g["c_idx"] = 0
        return 1
    for g in groups:
        g["c_idx"] = 0 if g["name"] == "K0s" else 1
    return 2


# ---------------------------------------------------------------- stage 2 train
def train_resolution(groups, args, n_c=1, verbose=True):
    net = ResoNet(args.reso_model, hidden=args.reso_hidden,
                  layers=args.reso_layers).double()
    glob = ResoGlobals(len(groups), n_c=n_c).double()
    # globals get 5x LR: Adam stalls along the c <-> (s, u_p) soft direction
    opt = torch.optim.Adam([
        {"params": net.parameters(), "lr": args.reso_lr},
        {"params": glob.parameters(), "lr": 5.0 * args.reso_lr},
    ], weight_decay=0.0)
    wtot = sum(float(g["w"].sum()) for g in groups)

    def total_nll():
        nll = torch.tensor(0.0, dtype=torch.float64)
        for gi, g in enumerate(groups):
            sig = sigma_mass_net(g["R"], net, glob, glob.c(g["c_idx"]))
            nll = nll + width_nll_sum(g["R"], g["M"], sig, g["w"],
                                      glob.f(gi), glob.k(gi), g["lo"], g["hi"])
        return nll

    for ep in range(args.reso_epochs):
        opt.zero_grad()
        loss = total_nll() / wtot
        loss.backward()
        opt.step()
        if verbose and ep % max(args.reso_epochs // 10, 1) == 0:
            with torch.no_grad():
                cs = " ".join(f"c{i}={float(glob.c(i))*1e3:.3f}" for i in range(n_c))
                extra = (f" u_p={float(glob.u_p()):+.4f}"
                         if args.reso_model == "pull" else "")
                fs = " ".join(f"{g['name'][:4]}: f={float(glob.f(gi)):.3f} "
                              f"k={float(glob.k(gi)):.2f}"
                              for gi, g in enumerate(groups))
            print(f"  reso ep {ep+1:4d}  -lnL/w {loss.item():10.5f}  "
                  f"{cs} MeV{extra}   {fs}")

    # LBFGS polish of the GLOBALS with the field frozen
    for p in net.parameters():
        p.requires_grad_(False)
    with torch.no_grad():
        best = float(total_nll())
        state0 = {k: v.clone() for k, v in glob.state_dict().items()}
    lb = torch.optim.LBFGS(list(glob.parameters()), lr=0.5, max_iter=100,
                           tolerance_grad=1e-10, line_search_fn="strong_wolfe")

    def closure():
        lb.zero_grad()
        v = total_nll()
        v.backward()
        return v
    try:
        lb.step(closure)
    except Exception as e:
        print(f"  [train_resolution] LBFGS polish skipped: {e}")
    with torch.no_grad():
        v_pol = float(total_nll())
    if not np.isfinite(v_pol) or v_pol > best:
        glob.load_state_dict(state0)
        print("  [train_resolution] LBFGS polish rejected (no improvement)")
    else:
        print(f"  [train_resolution] LBFGS polish: nll {best:.3f} -> {v_pol:.3f}")
    for p in net.parameters():
        p.requires_grad_(True)
    return net, glob


# ---------------------------------------------------------------- reduced fit
def fit_global_reduced(groups, mode, n_c=1, steps=3000, lr=0.02):
    """Reduced fit with constant fields, for a Hessian covariance.

    absolute: theta = (ln a, ln b, c..., f..., k...)
    pull:     theta = (ln s, u_p,  c..., f..., k...)
    Purpose is the injection go/no-go ('recovered within uncertainties');
    ResoNet itself carries no error bars.
    """
    ng = len(groups)
    if mode == "absolute":
        head0 = [math.log(0.0106), math.log(0.0106)]
    else:
        head0 = [0.0, 0.0]                       # ln s = 0, u_p = 0
    theta0 = torch.tensor(head0 +
                          [math.log(math.expm1(C_INIT))] * n_c +
                          [-2.0] * ng + [0.0] * ng, dtype=torch.float64)

    def unpack(t):
        # numerical bounds only; they never bind at a physical minimum
        if mode == "absolute":
            h1, h2 = torch.exp(t[0].clamp(-12, 2)), torch.exp(t[1].clamp(-12, 2))
        else:
            h1, h2 = torch.exp(t[0].clamp(-6, 6)), t[1].clamp(-3, 3)   # s, u_p
        c = F.softplus(t[2:2 + n_c])
        f = F_MAX * torch.sigmoid(t[2 + n_c:2 + n_c + ng])
        k = K_MIN + (K_MAX - K_MIN) * torch.sigmoid(t[2 + n_c + ng:])
        return h1, h2, c, f, k

    def nll(t):
        h1, h2, c, f, k = unpack(t)
        tot = torch.tensor(0.0, dtype=torch.float64)
        for gi, g in enumerate(groups):
            R = g["R"]
            if mode == "absolute":
                sig = sigma_mass_ab(R, h1, h2, h1, h2, c[g["c_idx"]])
            else:
                sig = sigma_mass_pull(R, h1, h1, h2, c[g["c_idx"]])
            tot = tot + width_nll_sum(R, g["M"], sig, g["w"], f[gi], k[gi],
                                      g["lo"], g["hi"])
        return tot

    t = theta0.clone().requires_grad_(True)
    opt = torch.optim.Adam([t], lr=lr)
    best, t_best = float("inf"), theta0.clone()
    for _ in range(steps):
        opt.zero_grad()
        v = nll(t)
        v.backward()
        opt.step()
        fv = float(v.detach())
        if np.isfinite(fv) and fv < best:
            best, t_best = fv, t.detach().clone()

    lb = torch.optim.LBFGS([t], lr=0.5, max_iter=200,
                           tolerance_grad=1e-10, line_search_fn="strong_wolfe")

    def closure():
        lb.zero_grad()
        v = nll(t)
        v.backward()
        return v
    try:
        lb.step(closure)
    except Exception as e:
        print(f"  [fit_global_reduced] LBFGS polish skipped: {e}")
    with torch.no_grad():
        v_pol = float(nll(t)) if torch.isfinite(t).all() else float("inf")
    if not np.isfinite(v_pol) or v_pol > best:
        with torch.no_grad():
            t.copy_(t_best)
    else:
        best = v_pol

    td = t.detach()
    H = torch.autograd.functional.hessian(nll, td)
    if not torch.isfinite(H).all():
        print("  [fit_global_reduced] Hessian non-finite at the polished point; "
              "retrying at the Adam-best point")
        td = t_best.clone()
        H = torch.autograd.functional.hessian(nll, td)
    pd, ev = False, None
    cov = torch.full_like(H, float("nan"))
    if torch.isfinite(H).all():
        try:
            ev = torch.linalg.eigvalsh(0.5 * (H + H.T))
            tol = 1e-8 * float(ev.abs().max())
            pd = bool((ev > -tol).all())
            cov = torch.linalg.inv(H) if pd else torch.linalg.pinv(H)
        except Exception as e:
            print(f"  [fit_global_reduced] eigen-decomposition failed: {e}")
    else:
        print("  [fit_global_reduced] Hessian contains non-finite entries")
    if not torch.isfinite(cov).all():
        print("  [fit_global_reduced] covariance not usable; errors reported as nan")
        cov = torch.full_like(H, float("nan"))
    if ev is not None and not pd:
        print(f"  [fit_global_reduced] Hessian not positive definite "
              f"(min eig {float(ev.min()):.3e}) -- fit is not at a minimum")

    def _err(i):
        v = float(cov[i, i])
        return math.sqrt(v) if np.isfinite(v) and v > 0 else float("nan")

    def _rho(i, j):
        d = math.sqrt(float(cov[i, i]) * float(cov[j, j]))
        return float(cov[i, j]) / d if np.isfinite(d) and d > 0 else float("nan")

    h1, h2, c, f, k = unpack(td)
    out = dict(pos_def=pd, c=c.detach().numpy(), f=f.detach().numpy(),
               k=k.detach().numpy(), nll=float(nll(td)))
    if mode == "absolute":
        out.update(a=float(h1), b=float(h2),
                   sa=float(h1) * _err(0), sb=float(h2) * _err(1),
                   rho_ab=_rho(0, 1), rho_ac=_rho(0, 2))
    else:
        out.update(s=float(h1), u_p=float(h2),
                   ss=float(h1) * _err(0), su=_err(1),
                   rho_su=_rho(0, 1), rho_sc=_rho(0, 2))
    return out
