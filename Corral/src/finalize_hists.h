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
// FINALIZE_SET : default dataset to combine = lists/<set>/ (manifest.txt gives nlists) and
//                root/<set>/chunks/<prefix>_NN.root (made by run_m_array.bash; <prefix> = CorralFilePrefix(set):
//                corral_m for ana532, corral from ana573 on); -d <dataset> overrides
// FINALIZE_REF : single-job full-stats reference to compare against, root/<dataset>/FINALIZE_REF
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
static const char*	FINALIZE_SET	= "ana532";		// default dataset; -d <dataset> overrides
//---- File-name convention per dataset (user 2026-09-27: the dataset decides, not a file-exists guess).
//---- ana532 was made when the binary was corral_m: root/ana532/corral_m.root, chunks/corral_m_NN.root.
//---- Every later dataset (ana573 on): root/<set>/corral.root, chunks/corral_NN.root. Add a dataset to
//---- the legacy list only if it was produced with the old names.
inline const char* CorralFilePrefix(const char* set){
	static const char* LEGACY[]	= { "ana532" };
	for (const char* l : LEGACY) if (TString(set)==l) return "corral_m";
	return "corral";
}
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
