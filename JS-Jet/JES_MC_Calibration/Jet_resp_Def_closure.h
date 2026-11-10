// ============================================================================
// CLOSURE (second pass / full closure) configuration. Source file:
// Jet_resp_Def_closure.h. Edit the source file and run ./set_config.sh closure
// to install it as Jet_resp_Def.h (never edit the installed copy directly).
// ============================================================================

// Configuration flag checked at run time by Jet_resp_histmaker.C
// (selects the signed z binning and guards against a mismatched closure_mode).
const bool config_firstpass = false;

// Jet radii: all radii for the production; reduce to a single entry for quick tests.
const int n_jetradius = 7;
std::vector<int> jet_radius_index = {2, 3, 4, 5, 6, 7, 8};

// Closure-test z binning. SIGNED z-vertex bins: negative and positive ranges are kept
// separate (no mirror folding). Events with |z| > 200 cm are excluded
// (get_zvertex_bin returns -1 and the event is skipped).
// bins 0-10: z from -200 to +200 cm with edges -200,-150,-100,-60,-30,-10,10,30,60,100,150,200; bin 11: No-z
const int n_zvertex = 12;
std::vector<int> zvertex_index = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
const char* z_label[n_zvertex] = {
  "-200 #leq z < -150 cm",
  "-150 #leq z < -100 cm",
  "-100 #leq z < -60 cm",
  "-60 #leq z < -30 cm",
  "-30 #leq z < -10 cm",
  "-10 #leq z < 10 cm",
  "10 #leq z < 30 cm",
  "30 #leq z < 60 cm",
  "60 #leq z < 100 cm",
  "100 #leq z < 150 cm",
  "150 #leq z #leq 200 cm",
  "No zvertex"
};
// Signed z bin edges shared by the analysis macros (11 physical bins)
const int n_zvertex_phys = 11;
double zvertex_edges[n_zvertex_phys + 1] = {-200, -150, -100, -60, -30, -10, 10, 30, 60, 100, 150, 200};


// Eta binning for the (z-vertex x eta) dependence check.
// The physical eta acceptance depends on z-vertex, so bins are defined in the fractional
// coordinate f = (eta - eta_min(z)) / (eta_max(z) - eta_min(z)), where eta_min/eta_max are
// the z-corrected acceptance limits (including the jet-radius padding) used in the eta cut.
// No mirror folding: negative and positive z are separate bins, and f is used as-is.
// Bins are uniform in f. The bin count scales with the jet radius: the z=0 acceptance width
// is about 2*(1.109 - R), so the count keeps roughly the same eta width per bin (~0.24,
// i.e. 6 bins for R=0.4), with a minimum of 3 bins for the largest radii and a maximum
// of 6 bins (radii below 0.4 are capped at the R=0.4 count).
const int n_etafracbin_max = 6;
int get_n_etafracbin(int radius_index) { // radius_index: 2..8 for R = 0.2..0.8
  if (radius_index == 2) return 6;
  if (radius_index == 3) return 6;
  if (radius_index == 4) return 6;
  if (radius_index == 5) return 5;
  if (radius_index == 6) return 4;
  if (radius_index == 7) return 3;
  if (radius_index == 8) return 3;
  return 6;
}


// Truth-pt binning: coarse (closure).
double truthptbins[] = {0, 8, 16, 24, 32, 40, 50, 60, 70, 80, 100};
const int n_truthptbin = sizeof(truthptbins) / sizeof(double) - 1;

// EMfrac binning: coarse (closure).
double emfracbins[] = {0.0, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875, 1.0};
const int n_emfracbin = sizeof(emfracbins) / sizeof(double) - 1;

double ptratiobins[] = {0, 0.02, 0.04, 0.06, 0.08, 0.1, 0.12, 0.14, 0.16, 0.18, 0.2, 0.22, 0.24, 0.26, 0.28, 0.3, 0.32, 0.34, 0.36, 0.38, 0.4, 0.42, 0.44, 0.46, 0.48, 0.5, 0.52, 0.54, 0.56, 0.58, 0.6, 0.62, 0.64, 0.66, 0.68, 0.7, 0.72, 0.74, 0.76, 0.78, 0.8, 0.82, 0.84, 0.86, 0.88, 0.9, 0.92, 0.94, 0.96, 0.98, 1.0, 1.02, 1.04, 1.06, 1.08, 1.1, 1.12, 1.14, 1.16, 1.18, 1.2, 1.22, 1.24, 1.26, 1.28, 1.3, 1.32, 1.34, 1.36, 1.38, 1.4, 1.42, 1.44, 1.46, 1.48, 1.5, 1.52, 1.54, 1.56, 1.58, 1.6, 1.62, 1.64, 1.66, 1.68, 1.7, 1.72, 1.74, 1.76, 1.78, 1.8, 1.82, 1.84, 1.86, 1.88, 1.9, 1.92, 1.94, 1.96, 1.98, 2.0};
const int n_ptratiobin = sizeof(ptratiobins) / sizeof(double) - 1;
