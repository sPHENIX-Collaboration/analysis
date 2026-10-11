"""Physics constants, mass windows, numerical guards, resolution-head scales; global RNG seeds."""
import math
import numpy as np
import torch

# constants
PION_MASS   = 0.13957039     # GeV  (never name this M_PI)
PROTON_MASS = 0.93827209
M_K0S       = 0.497611
M_LAMBDA    = 1.115683

ETA_RANGE = (-1.5, 1.5)
PT_SANITY = 1e-3             # GeV; reject unphysical/zero pT, NOT a fiducial cut

KAPPA_GUARD = 0.05           # numerical floor on kappa_curv = 1 + eps + q*delta. NOT a physics cap; hit fraction must be 0.
X_CLAMP = 6.0                # numerical clamp on the normalised momentum feature x; outside it kappa is constant in x.

# stage-1 momentum feature x(pT) fed to KappaNet
FEATURE_KINDS = ("inv_pt", "pt", "log_pt")
FEATURE_UNITS = {"inv_pt": "GeV^-1", "pt": "GeV", "log_pt": "ln(GeV)"}

# mass windows used for the width NLL (must match the load() selection)
WIN = {"K0s": (0.42, 0.58), "Lambda": (1.09, 1.145), "Lambdabar": (1.09, 1.145)}
MPDG = {"K0s": M_K0S, "Lambda": M_LAMBDA, "Lambdabar": M_LAMBDA}
SIG0 = {"K0s": 0.010, "Lambda": 0.004, "Lambdabar": 0.004}
STAGE1_HISTORY_SPECIES = (
    ("kshort", "K0s", M_K0S, SIG0["K0s"], WIN["K0s"]),
    ("lambda", "Lambda", M_LAMBDA, SIG0["Lambda"], WIN["Lambda"]),
    ("anti_lambda", "Lambdabar", M_LAMBDA, SIG0["Lambdabar"], WIN["Lambdabar"]),
)

# resolution-head parametrisation
A_SCALE = 0.05               # absolute: a = A_SCALE * softplus(.)  dimensionless
B_SCALE = 0.05               # absolute: b = B_SCALE * softplus(.)  [1/GeV]
S_SCALE = 2.0                # pull:     s = S_SCALE * softplus(.)  (init -> 1)
C_INIT  = 0.0015             # GeV, initial mass-width floor
F_MAX   = 0.30               # tail fraction upper bound
K_MIN, K_MAX = 1.2, 4.0      # tail width ratio bounds
RCOV_VALID = (1e-5, 1.0)     # accepted r_cov range

MOTHER_PREFIXES = ("K_S0", "Lambda0")

LOG_SQRT_2PI = 0.5 * math.log(2.0 * math.pi)
SQRT2 = math.sqrt(2.0)

torch.manual_seed(17)
np.random.seed(17)
