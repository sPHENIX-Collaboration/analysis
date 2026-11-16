#ifndef ZVTXAVERAGE_H
#define ZVTXAVERAGE_H
//
// Shared by CalcRm::Calculate and corral::Finalize (README_Finalize.md), so that the per-run and the
// full-statistics Zvtx averages use exactly the same rules. Moved verbatim from CalcRm.cxx.
//
#include <cmath>
#include "TH1.h"

//------------------------------------------------------------
// Zvtx average of a CF-type histogram, bin by bin, over the slices where that bin is VALID
// (README_Crossing sec 8): slice ok (computed), weight>0, and mask>0 (mask = rho2(M) of the slice,
// or the crossing-corrected validity map); mask==0 -> valid iff the slice's bin error>0 (used for
// the 1D projections, which are only filled from valid bins). Weights are renormalized per bin:
// v = sum(w v)/sum(w), e = sqrt(sum(w^2 e^2))/sum(w). A bin valid in no slice gets `sentinel`, error 0.
// Per-run errors only; Finalize replaces them with subgroup errors.
inline void ZvtxAverageValid(TH1* avg, TH1** sl, TH1** mask, const double* w, const bool* ok, int n, double sentinel){
	avg->Reset();
	double nent	= 0;
	for (int z=0;z<n;z++) if (ok[z] && w[z]>0.) nent += w[z]*sl[z]->GetEntries();
	for (int ib=0;ib<avg->GetNcells();ib++){
		double sw=0, swv=0, sw2e2=0;
		for (int z=0;z<n;z++){
			if (!ok[z] || w[z]<=0.) continue;
			bool valid	= mask ? (mask[z]->GetBinContent(ib)>0.) : (sl[z]->GetBinError(ib)>0.);
			if (!valid) continue;
			double e	= sl[z]->GetBinError(ib);
			sw		+= w[z];
			swv		+= w[z]*sl[z]->GetBinContent(ib);
			sw2e2	+= w[z]*w[z]*e*e;
		}
		if (sw>0.){ avg->SetBinContent(ib,swv/sw);   avg->SetBinError(ib,sqrt(sw2e2)/sw); }
		else      { avg->SetBinContent(ib,sentinel); avg->SetBinError(ib,0.); }
	}
	avg->SetEntries(nent);
}

#endif
