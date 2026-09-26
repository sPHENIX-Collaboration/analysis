#ifndef FINALIZE_HISTS_H
#define FINALIZE_HISTS_H
#include <cstdlib>
#include "TString.h"
#ifndef CORRAL_DIR_DEFAULT
#define CORRAL_DIR_DEFAULT "./"
#endif
inline TString CorralDir(){
	const char* e	= getenv("CORRAL_DIR");
	TString d		= (e && *e) ? TString(e) : TString(CORRAL_DIR_DEFAULT);
	if (!d.EndsWith("/")) d += "/";
	return d;
}
//
// Finalize configuration (README_Finalize.md). Edit this file to choose what Finalize processes.
//
// FINALIZE_SET : chunk production to combine = lists/<set>/ (manifest.txt gives nlists) and
//                root/<set>/corral_m_NN.root (made by run_m_array.bash)
// FINALIZE_REF : single-job full-stats reference to compare against (relative to CorralDir())
// CorralDir()  : project directory holding lists/ and root/ = $CORRAL_DIR if set, else the source
//                tree the binary was built from (CORRAL_DIR_DEFAULT, set by CMakeLists.txt)
// FinalizeHists: histogram BASE names; Finalize appends _<ipaty> for every pairtype, so
//                "hmult" -> hmult_0 .. hmult_15, "hrho2_1" -> hrho2_1_0 .. hrho2_1_15.
//                Naming in the corral_m root files:
//                  hzvtx, hmult, hmult1, hmult2, hy1, hy2          (per pairtype)
//                  hrho2_<ir2>      rho2(S)   ir2: 0=(y1,y2) 1=(dy,dphi) 2=(dy,dq)
//                  hrho1rho1_<ir2>  rho2(M)
//                  hC2_<ir2>, hR2_<ir2>
//                  hrho2C_1, hC2C_1, hR2C_1   crossing-corrected (dy,dphi)
//                  hMempty_1                  # used Zvtx slices with empty rho2(M)
//                  hR2dy, hR2dphi, hR2yydy, hR2dq, hCQ, hQsib, hQmix, hMinv_S, hMinv_M, hMinv
//
static const char*	FINALIZE_SET	= "run_ecuts_cf";
static const char*	FINALIZE_REF	= "root/corral_m_CrossingCorrected.root";
static const double	FINALIZE_FIELD	= 1.4;		// must match `double field` in corral_loop.cxx (crossing dirty side)
//
// Zvtx-average validity (README_Finalize step 3, option A): the threshold N_min is now PER PAIRTYPE,
// the 3rd field of PairTypes_Info in PairTypes.h (user 2026-09-26). The environment variable
// FINALIZE_NEXPMIN overrides it for all pairtypes (scans).
//
// CF names (hrho2_<ir2>, hrho1rho1_<ir2>, hC2_<ir2>, hR2_<ir2>, hrho2C_1, hC2C_1, hR2C_1) get the full
// per-zvtx-bin treatment (README_Finalize step 2); any other name is a plain sum over chunks.
static const char*	FinalizeHists[]	= {
						"hzvtx",
						"hmult",
						"hmult1",
						"hmult2",
						"hrho2_0",
						"hrho1rho1_0",
						"hC2_0",
						"hR2_0",
						"hrho2_1",
						"hrho1rho1_1",
						"hC2_1",
						"hR2_1",
						"hrho2C_1",
						"hC2C_1",
						"hR2C_1",
					};
static const int	NFINALIZEHISTS	= sizeof(FinalizeHists)/sizeof(FinalizeHists[0]);

#endif
