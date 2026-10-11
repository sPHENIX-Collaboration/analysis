"""Stage 1: momentum-scale network (KappaNet), checkpoint I/O, training."""
import sys
import numpy as np
import torch
import torch.nn as nn

from .constants import KAPPA_GUARD, M_K0S, M_LAMBDA, STAGE1_HISTORY_SPECIES
from .kinematics import bin_index, binned_mean_sq, feature_raw, inv_mass
from .peakfit import fit_peak, signal_weights

# ---------------------------------------------------------------- scale model
CONVENTION_TAG = "curvature_mult_uncapped"     # full tag: CONVENTION_TAG:feature=<kind>


class KappaNet(nn.Module):
    """(x_n, eta_n, sin phi, cos phi) -> (eps, delta), UNCAPPED.

    x = x(pT) is the momentum feature (1/pT, pT or ln pT);
    x_n = clamp((x - x_ref)/x_scale, +-X_CLAMP).
    The clamp is a numerical guard: outside it kappa is constant in x.
    Convention:  (1/pT)_corr = (1/pT) * (1 + eps + q*delta);  kappa() returns
    the pT MULTIPLIER 1/(1+eps+q*delta) so call sites keep pT_corr = kappa*pT.
    q enters only through the output combination -> exact even/odd split in
    curvature space.  Near-identity init; the annealed L2 prior in train() is
    the only soft pull to identity and vanishes at the last epoch.
    """

    def __init__(self, norm, hidden=48, layers=3):
        super().__init__()
        self.kind = str(norm["kind"])
        self.x_ref = float(norm["x_ref"])
        self.x_scale = float(norm["x_scale"])
        self.x_clamp = float(norm["x_clamp"])
        dims = [4] + [hidden] * layers + [2]
        seq = []
        for i in range(len(dims) - 1):
            seq.append(nn.Linear(dims[i], dims[i + 1]))
            if i < len(dims) - 2:
                seq.append(nn.SiLU())
        self.net = nn.Sequential(*seq)
        with torch.no_grad():
            self.net[-1].weight *= 0.01
            self.net[-1].bias.zero_()          # eps = delta = 0  ->  kappa = 1

    def norm_dict(self):
        return dict(kind=self.kind, x_ref=self.x_ref, x_scale=self.x_scale,
                    x_clamp=self.x_clamp)

    def tag(self):
        return f"{CONVENTION_TAG}:feature={self.kind}"

    def features(self, pt, eta, phi):
        x = feature_raw(self.kind, pt.clamp(min=1e-6))
        x_n = ((x - self.x_ref) / self.x_scale).clamp(-self.x_clamp, self.x_clamp)
        return torch.stack([x_n, eta / 1.0, torch.sin(phi), torch.cos(phi)], -1)

    def clamp_frac(self, pt):
        with torch.no_grad():
            x = feature_raw(self.kind, pt.clamp(min=1e-6))
            x_n = (x - self.x_ref) / self.x_scale
            return float((x_n.abs() >= self.x_clamp).double().mean())

    def eps_delta(self, pt, eta, phi):
        out = self.net(self.features(pt, eta, phi))
        return out[..., 0], out[..., 1]

    def kappa(self, pt, eta, phi, q):
        """Returns (pT multiplier, eps, delta);  pT_corr = multiplier * pT."""
        eps, dlt = self.eps_delta(pt, eta, phi)
        kcurv = (1.0 + eps + q * dlt).clamp(min=KAPPA_GUARD)
        return 1.0 / kcurv, eps, dlt


class IdentityKappa(nn.Module):
    """kappa == 1; used to run stage 2 on injection toys."""

    def kappa(self, pt, eta, phi, q):
        z = torch.zeros_like(pt)
        return torch.ones_like(pt), z, z


def save_stage1(model, path):
    torch.save({"state_dict": model.state_dict(),
                "norm": model.norm_dict(),
                "convention": model.tag()}, path)


def load_stage1(path, hidden):
    ckpt = torch.load(path, weights_only=False)
    tag = ckpt.get("convention", "") if isinstance(ckpt, dict) else ""
    if not tag.startswith(f"{CONVENTION_TAG}:feature="):
        sys.exit(f"--load_stage1 {path}: checkpoint predates the "
                 f"'{CONVENTION_TAG}:feature=<kind>' convention (or is a bare "
                 f"state_dict). It would load silently with WRONG semantics. "
                 f"Retrain stage 1.")
    model = KappaNet(ckpt["norm"], hidden=hidden).double()
    model.load_state_dict(ckpt["state_dict"])
    return model


def corrected_mass(model, S):
    k1, e1, d1 = model.kappa(S["pt1"], S["eta1"], S["phi1"], S["q1"])
    k2, e2, d2 = model.kappa(S["pt2"], S["eta2"], S["phi2"], S["q2"])
    m = inv_mass(k1 * S["pt1"], S["eta1"], S["phi1"], S["m1"],
                 k2 * S["pt2"], S["eta2"], S["phi2"], S["m2"])
    reg = (e1**2 + d1**2 + e2**2 + d2**2).mean()
    return m, (k1, k2), reg


# ---------------------------------------------------------------- training history
def empty_stage1_history_row(epoch, loss=float("nan")):
    row = {"epoch": int(epoch), "loss": float(loss)}
    for key, _, _, _, _ in STAGE1_HISTORY_SPECIES:
        row[f"{key}_mean_GeV"] = float("nan")
        row[f"{key}_width_GeV"] = float("nan")
    return row


def stage1_mass_history_from_masses(mK, mL, is_lam):
    mK = np.asarray(mK)
    mL = np.asarray(mL)
    is_lam = np.asarray(is_lam, dtype=bool)
    masses = {
        "kshort": mK,
        "lambda": mL[is_lam],
        "anti_lambda": mL[~is_lam],
    }
    out = {}
    for key, _, pdg, sig0, win in STAGE1_HISTORY_SPECIES:
        vals = masses[key]
        if vals.size < 10:
            mu, sig = float("nan"), float("nan")
        else:
            mu, sig, _ = fit_peak(vals, pdg, sig0, *win)
        out[f"{key}_mean_GeV"] = float(mu)
        out[f"{key}_width_GeV"] = float(sig)
    return out


def stage1_mass_history_from_model(model, K, L):
    with torch.no_grad():
        mK, _, _ = corrected_mass(model, K)
        mL, _, _ = corrected_mass(model, L)
    return stage1_mass_history_from_masses(
        mK.numpy(), mL.numpy(), L["is_lam"].numpy()
    )


# ---------------------------------------------------------------- stage 1
def train(args, K, L, meta):
    """Returns (model, history); history is the per-epoch list of rows for
    plot_stage1_training_history."""
    model = KappaNet(meta["norm"], hidden=args.hidden).double()
    opt = torch.optim.Adam(model.parameters(), lr=args.lr, weight_decay=1e-5)
    history = {0: empty_stage1_history_row(0)}
    history_every = max(1, args.history_every if args.history_every > 0 else args.refresh)

    pt_edges = torch.tensor(meta["pt_edges"], dtype=torch.float64)
    N_ETA, N_PHI = 10, 8

    def refresh_weights():
        with torch.no_grad():
            mK, _, _ = corrected_mass(model, K)
            mL, _, _ = corrected_mass(model, L)
        muK, sK, pK = fit_peak(mK.numpy(), M_K0S, 0.010, 0.42, 0.58)
        muL, sL, pL = fit_peak(mL.numpy(), M_LAMBDA, 0.004, 1.09, 1.145)
        K["w"] = torch.tensor(signal_weights(mK.numpy(), pK))
        L["w"] = torch.tensor(signal_weights(mL.numpy(), pL))
        metrics = stage1_mass_history_from_masses(
            mK.numpy(), mL.numpy(), L["is_lam"].numpy()
        )
        return (muK, sK), (muL, sL), metrics

    (muK0, _), (muL0, _), metrics0 = refresh_weights()
    history[0].update(metrics0)
    print(f"raw peaks:  K0s {muK0*1e3:8.3f} MeV (PDG {M_K0S*1e3:.3f}), "
          f"Lambda {muL0*1e3:8.3f} MeV (PDG {M_LAMBDA*1e3:.3f})")

    # loss bins depend only on the uncorrected kinematics: fixed for the whole training
    daughter_bins = {}
    for name, S in (("K", K), ("L", L)):
        daughter_bins[name] = []
        for (pt, eta, phi, q) in ((S["pt1"], S["eta1"], S["phi1"], S["q1"]),
                                  (S["pt2"], S["eta2"], S["phi2"], S["q2"])):
            idx, nb = bin_index(pt, eta, phi, pt_edges, N_ETA, N_PHI)
            daughter_bins[name].append((idx + ((q > 0).long() * nb), 2 * nb))
    idx_split, nb_split = bin_index(L["pt1"], L["eta1"], L["phi1"], pt_edges, N_ETA, N_PHI)
    idxc, nbc = bin_index(K["cpt"], K["ceta"], K["cphi"], pt_edges, N_ETA, N_PHI)
    sel_l, sel_b = L["is_lam"], ~L["is_lam"]
    pull_scale = {"K": 0.010, "L": 0.004}     # GeV, mass-pull normalisation of the loss

    def chunks(S):
        """Candidate slices of at most --chunk_size: the gradient graph of one
        chunk is held at a time, so memory does not grow with the sample."""
        n = S["mass"].numel()
        for a in range(0, n, args.chunk_size):
            yield a, {k: (v[a:a + args.chunk_size] if torch.is_tensor(v) else v)
                      for k, v in S.items()}

    for ep in range(args.epochs):
        opt.zero_grad()
        prior_w = args.lam_prior * (1.0 - ep / max(args.epochs - 1, 1))  # anneal -> 0

        # pass 1, no graph: full-sample per-candidate values -> loss and the
        # per-candidate derivatives of the loss w.r.t. those values
        val = {}
        with torch.no_grad():
            for name, S in (("K", K), ("L", L)):
                m, alpha, reg_sum = [], [], 0.0
                for a, C in chunks(S):
                    mc, (k1, k2), reg = corrected_mass(model, C)
                    m.append(mc)
                    p1, p2 = k1 * C["pt1"], k2 * C["pt2"]
                    alpha.append((p1 - p2) / (p1 + p2))        # pi- minus pi+ (used for K0s)
                    reg_sum += reg.item() * mc.numel()
                val[name] = ((torch.cat(m) - S["M"]) / pull_scale[name],
                             torch.cat(alpha), reg_sum / S["mass"].numel())

        loss = 0.0
        coef = {}
        for name, S in (("K", K), ("L", L)):
            pull, _, reg = val[name]
            coef[name] = torch.zeros_like(pull)
            for idx, nb in daughter_bins[name]:
                v, c = binned_mean_sq(idx, nb, pull, S["w"])
                loss += v.item()
                coef[name] += c
            loss += prior_w * reg

        pullL = val["L"][0]
        wl = L["w"]
        swl  = torch.zeros(nb_split, dtype=torch.float64).index_add_(0, idx_split[sel_l], wl[sel_l])
        swvl = torch.zeros(nb_split, dtype=torch.float64).index_add_(0, idx_split[sel_l], (wl * pullL)[sel_l])
        swb  = torch.zeros(nb_split, dtype=torch.float64).index_add_(0, idx_split[sel_b], wl[sel_b])
        swvb = torch.zeros(nb_split, dtype=torch.float64).index_add_(0, idx_split[sel_b], (wl * pullL)[sel_b])
        mk = (swl > 5) & (swb > 5)
        split = torch.zeros(nb_split, dtype=torch.float64)
        wsum = torch.zeros(nb_split, dtype=torch.float64)
        split[mk] = swvl[mk] / swl[mk] - swvb[mk] / swb[mk]
        wsum[mk] = (swl[mk] * swb[mk]) / (swl[mk] + swb[mk])
        norm_split = wsum[mk].sum().clamp(min=1e-9)
        loss += args.lam_split * ((wsum[mk] * split[mk]**2).sum() / norm_split).item()
        # d/d pull_i of sum_b wsum_b split_b^2 / norm: +-2 wsum_b split_b w_i / sw_b / norm
        g = 2.0 * args.lam_split * wsum * split / norm_split
        coef["L"][sel_l] += g[idx_split[sel_l]] * wl[sel_l] / swl[idx_split[sel_l]].clamp(min=1e-9)
        coef["L"][sel_b] -= g[idx_split[sel_b]] * wl[sel_b] / swb[idx_split[sel_b]].clamp(min=1e-9)

        v, coef_alpha = binned_mean_sq(idxc, nbc, val["K"][1], K["w"])
        loss += args.lam_alpha * v.item()
        coef_alpha = args.lam_alpha * coef_alpha

        # pass 2, with graph, chunk by chunk: sum_i coef_i * value_i has the
        # gradient of the full-sample loss; gradients accumulate across chunks
        for name, S in (("K", K), ("L", L)):
            n = S["mass"].numel()
            for a, C in chunks(S):
                mc, (k1, k2), reg = corrected_mass(model, C)
                pull = (mc - S["M"]) / pull_scale[name]
                surrogate = (coef[name][a:a + pull.numel()] * pull).sum() \
                    + prior_w * reg * pull.numel() / n
                if S is K:
                    p1, p2 = k1 * C["pt1"], k2 * C["pt2"]
                    alpha = (p1 - p2) / (p1 + p2)
                    surrogate = surrogate + (coef_alpha[a:a + alpha.numel()] * alpha).sum()
                surrogate.backward()
        opt.step()
        loss = torch.tensor(loss, dtype=torch.float64)
        epoch = ep + 1
        history[epoch] = empty_stage1_history_row(epoch, loss.item())

        should_refresh = ep % args.refresh == args.refresh - 1
        should_record_metrics = (epoch % history_every == 0) or (epoch == args.epochs)
        if should_refresh:
            (muK, _), (muL, _), metrics = refresh_weights()
            history[epoch].update(metrics)
            print(f"ep {epoch:4d}  loss {loss.item():9.5f}  "
                  f"K0s peak {muK*1e3:8.3f}  Lam peak {muL*1e3:8.3f} MeV")
        elif should_record_metrics:
            history[epoch].update(stage1_mass_history_from_model(model, K, L))

    return model, [history[k] for k in sorted(history)]
