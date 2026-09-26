//
//---- fluct_common.h		(PairTypes Definition REMOVED!)
//---- main definitions used by many codes and macros...
//
	//---- TPC-only bits of `layermask` (README_SplitTracks.md sec 9.1/9.2): bits 0-2 are MVTX,
	//---- 3-6 are INTT, 7-54 are TPC. The STAR SL formula (sec 9.2/6) must run on the TPC portion
	//---- only -- masking out the silicon bits, which every genuine split pair shares by
	//---- construction (same seed), would otherwise bias SL for every pair.
	static const ULong64_t kTPCLayerMask	= ((1ULL<<48)-1) << 7;
	//
	//---- STAR-style pairwise Splitting Level (arXiv:nucl-ex/0411036 sec 2.4.1), computed from two
	//---- tracks' TPC pad-row occupancy masks. SL=-0.5 means the pair shares essentially every TPC
	//---- pad-row hit (one real track, split); SL=1.0 means no shared pad-rows (two independent
	//---- tracks). Callers must treat hitmask==0 as "not a real track" (e.g. a V0 slot, sec 10.2)
	//---- and skip this function in that case -- it has no way to detect that condition itself.
	//---- Shared by CalcRm::ComputeSL and corral_loop.cxx's split-track SL monitor (sec 10.7/10.9)
	//---- so the formula only lives in one place.
	static inline double ComputeSplitSL(ULong64_t maskA, ULong64_t maskB){
		ULong64_t a		= maskA & kTPCLayerMask;
		ULong64_t b		= maskB & kTPCLayerMask;
		int Nboth		= __builtin_popcountll(a & b);
		int Ndiff		= __builtin_popcountll(a ^ b);
		int Ntot		= __builtin_popcountll(a) + __builtin_popcountll(b);
		if (Ntot==0) return 1.0;		// no TPC hits on either leg -- can't judge splitting, don't cut
		return ((double)(Ndiff - Nboth)) / ((double)Ntot);
	}
	//
	//---- RadialGap (README_SplitTracks.md sec 17.11): SL above only sees pad-row OVERLAP, so it is
	//---- structurally blind to a clean STAR-fig-1b radial split (one real track, TPC clusters
	//---- partitioned into disjoint inner/outer halves) -- that forces Nboth=0, Ndiff=Ntot, SL=+1
	//---- exactly, indistinguishable from two genuinely independent tracks. RadialGap looks at what
	//---- SL discards: the TPC bit INDEX itself (bits 7-54, sec 9.1) maps directly to physical
	//---- pad-row/radius, so this measures how cleanly the two legs' occupied layers separate by
	//---- radius -- |mean layer index[A] - mean layer index[B]|, normalized to [0,1] by the 47-layer
	//---- span (0 = no radial separation between the legs' mean hit radius, ~1 = fully separated,
	//---- inner vs outer half of the TPC). Returns -1.0 (undefined, caller must skip) if either leg
	//---- has zero TPC hits, same convention as SiSplitScore's -1.0.
	static inline double ComputeRadialGap(ULong64_t maskA, ULong64_t maskB){
		ULong64_t a		= maskA & kTPCLayerMask;
		ULong64_t b		= maskB & kTPCLayerMask;
		double sumA=0,sumB=0; int nA=0,nB=0;
		for (int L=0; L<48; L++){
			if (a & (1ULL<<(7+L))){ sumA+=L; ++nA; }
			if (b & (1ULL<<(7+L))){ sumB+=L; ++nB; }
		}
		if (nA==0 || nB==0) return -1.0;
		return fabs(sumA/nA - sumB/nB) / 47.0;
	}
	//
	//---- Si (MVTX+INTT) bits of `layermask`, bits 0-6 -- see 9.1. A genuine split pair is built on
	//---- one identical silicon seed by construction (sec 3/9.4), so an *exact* bit-for-bit match here
	//---- is a much sharper same-track signature than TPC-only SL alone: two independent, well-
	//---- reconstructed tracks routinely share most of their ~48 TPC layers just by detector geometry
	//---- (2026-09-20 measurement: 95% Si-match rate for real split candidates vs. only 14% for
	//---- manifestly-unrelated same-charge pairs, far tighter than SL's own separation). Used as a
	//---- pre-filter alongside SL (2026-09-20 hybrid cut, README sec 10.9 update) rather than SL alone,
	//---- which was found to reject ~99% of unrelated pairs too at any threshold that actually
	//---- suppresses the (0,0) spike -- collateral damage the Si-match requirement avoids.
	static const ULong64_t kSiLayerMask	= (1ULL<<7)-1;
	static inline bool SiSeedMatch(ULong64_t maskA, ULong64_t maskB){
		return (maskA & kSiLayerMask) == (maskB & kSiLayerMask);
	}
	//
	//---- SiKeyFrac: a real hit-IDENTITY check from `siclukey` (README_SplitTracks.md sec 12a.4/
	//---- 13.x), not just a coverage-pattern match like SiSeedMatch above. Graded fraction of the
	//---- 7 Si layer slots where the two legs' cluster keys are bit-for-bit identical, over the
	//---- slots either leg actually has a hit in. keyA/keyB point at a track's 7-slot siclukey row
	//---- (flattened [itrk*7+slot] convention, sec 13.2); maskA/maskB are the tracks' full
	//---- layermask (only bits 0-6 consulted here). Standalone close(|deta|<0.01,|dphi|<0.03)-vs-
	//---- far(|deta|>0.3) measurement (2026-09-21, two runs, ~100k events each, cross-validated):
	//---- SiKeyFrac>=0.6 catches 97.0% of close/likely-split pairs (vs SiSeedMatch's 94.6%) while
	//---- false-flagging only 0.003% of far/independent pairs (vs SiSeedMatch's 13.9%) -- SiSeedMatch's
	//---- false-positive rate is confirmed to be almost entirely "coincidental full Si coverage,"
	//---- not real shared-cluster identity (cross-tab: of far-population pairs SiSeedMatch flags,
	//---- ~0% also have high SiKeyFrac). Returns -1.0 if neither leg has any Si hit at all
	//---- (undefined -- caller must skip, same convention as ComputeSplitSL returning 1.0 for
	//---- "no TPC hits on either leg").
	static const ULong64_t kSiKeySentinel	= ~0ULL;	// "no cluster in this Si slot", sec 12a.1 -- NOT 0
	static inline double SiKeyFrac(const ULong64_t *keyA, const ULong64_t *keyB,
	                                ULong64_t maskA, ULong64_t maskB){
		int num=0, den=0;
		for (int L=0; L<7; L++){
			bool haveA	= (maskA>>L) & 1ULL;
			bool haveB	= (maskB>>L) & 1ULL;
			if (haveA || haveB) ++den;
			if (keyA[L]==keyB[L] && keyA[L]!=kSiKeySentinel) ++num;
		}
		if (den==0) return -1.0;
		return ((double)num) / ((double)den);
	}
	//
	//---- SiKeyFracSplit: exploratory split of the above into an MVTX-only (slots 0-2) and an
	//---- INTT-only (slots 3-6) fraction (README_SplitTracks.md sec 13.9 -- the flagged-but-unacted
	//---- MVTX/INTT equal-weighting concern, sec 13.8's closing update). MVTX pixels (~27x29um) are
	//---- ~1000x finer than an INTT strip's long dimension (78umx16-20mm), so an accidental match on
	//---- an INTT slot between two UNRELATED tracks is intrinsically far more likely than on an MVTX
	//---- slot, yet the combined SiKeyFrac above weighs a matched slot of either type identically and
	//---- floats its denominator (an INTT-only 4/4 match reports the same 1.0 as a true 7/7 match).
	//---- Diagnostic only right now -- not wired into any cut, purely for measuring how much this
	//---- actually matters before deciding whether/how to build a resolution-aware replacement.
	//---- den==0 (no hit on either leg, either sub-detector) reported as -1.0, same sentinel
	//---- convention as SiKeyFrac above.
	static inline void SiKeyFracSplit(const ULong64_t *keyA, const ULong64_t *keyB,
	                                    ULong64_t maskA, ULong64_t maskB,
	                                    double &mvtxFrac, int &mvtxDen,
	                                    double &inttFrac, int &inttDen){
		int numM=0, denM=0, numI=0, denI=0;
		for (int L=0; L<7; L++){
			bool haveA	= (maskA>>L) & 1ULL;
			bool haveB	= (maskB>>L) & 1ULL;
			bool have	= haveA || haveB;
			bool match	= (keyA[L]==keyB[L] && keyA[L]!=kSiKeySentinel);
			if (L<3){			// MVTX: layermask bits 0-2
				if (have)  ++denM;
				if (match) ++numM;
			} else {			// INTT: layermask bits 3-6
				if (have)  ++denI;
				if (match) ++numI;
			}
		}
		mvtxDen		= denM;
		inttDen		= denI;
		mvtxFrac	= (denM>0) ? ((double)numM/denM) : -1.0;
		inttFrac	= (denI>0) ? ((double)numI/denI) : -1.0;
	}
	//
	//---- SiSplitScore: MVTX-primary, INTT-secondary split-track discriminator
	//---- (README_SplitTracks.md sec 13.9) -- REPLACES the plain, unweighted SiKeyFrac() above as
	//---- the production pre-pass discriminator (corral_loop.cxx). Direct data-driven design from
	//---- a 152,432-candidate measurement (sec 13.9): MVTX evidence is (a) ALWAYS available in the
	//---- candidate population (mvtxDen is 2 or 3 for every single measured candidate, never 0-1)
	//---- and (b) far more reliable than INTT as standalone evidence. Both effects are real, small,
	//---- and directionally exactly what MVTX's ~1000x-finer pixel granularity predicts (an
	//---- accidental INTT-strip coincidence between unrelated tracks is far likelier than an
	//---- accidental MVTX-pixel one): when MVTX unambiguously reads "no match" (mvtxFrac==0), INTT
	//---- alone would spuriously agree ("full match") 5.1% of the time; when MVTX unambiguously
	//---- reads "exact match" (mvtxFrac==1), INTT alone would spuriously disagree ("zero overlap")
	//---- 8.0% of the time. So this discriminator trusts MVTX's own unambiguous calls outright, and
	//---- only consults INTT to break a tie when MVTX itself is genuinely partial (denM==3 only:
	//---- 1/3 or 2/3 -- denM==2 has no middle value). Returns -1.0 (undefined, caller must skip) only
	//---- in the never-yet-observed case of zero MVTX evidence on either leg, falling back to INTT
	//---- alone as the best signal left in that case.
	static inline double SiSplitScore(const ULong64_t *keyA, const ULong64_t *keyB,
	                                    ULong64_t maskA, ULong64_t maskB){
		double mvtxFrac,inttFrac; int mvtxDen,inttDen;
		SiKeyFracSplit(keyA,keyB,maskA,maskB,mvtxFrac,mvtxDen,inttFrac,inttDen);
		if (mvtxDen==0) return inttFrac;			// not observed in sec 13.9's measurement, kept for
													// safety -- itself -1.0 if inttDen==0 too
		if (mvtxFrac<=0.0) return 0.0;				// MVTX unambiguous "no match" -- trust it, ignore INTT
		if (mvtxFrac>=1.0) return 1.0;				// MVTX unambiguous "exact match" -- trust it, ignore INTT
		if (inttDen==0) return mvtxFrac;			// genuinely partial MVTX, no INTT evidence either
		return 0.5*(mvtxFrac+inttFrac);				// genuinely partial MVTX -- let INTT corroborate
	}
	//
	static const int 		NSpecies	= 8;
	static const double 	Species_mass[NSpecies] = {0.13956995,0.493677,0.93827231,0.9395654133,1.87561339,
														0.497763,1.115,1.115		}; //,2.80925,2.80923,3.727417};
	//
	static const char *SpeciesNames[NSpecies]   	   = {"#pi", "K", "p", "n", "d",	//,  "t","^{3}He","#alpha"};
															"K^{0}_{S}","#Lambda_{0}","#bar{#Lambda}_{0}" }; 						
	//
	static const double Species_ptmin[NSpecies]           = {  0.1, 0.1, 0.1,  0.1,  0.1,   0.1, 0.1, 0.1 };	//,  1.2,     1.2,      1.6};
	static const double Species_ptmax[NSpecies]           = { 20.1, 20.1,20.1,20.1, 20.1,  20.1,20.1,20.1 };	//,  4.0,     4.0,      4.0};	
// 	static const double Species_pmax_dedx[NSpecies]       = {  0.7, 0.7, 1.0,  1.5,  8.0,     8.0,      8.0};	
// 	static const double Species_pmax_tof[NSpecies]        = {  1.7, 1.7, 2.9,  4.0,  8.0,     8.0,      8.0};	
// 	static const double Species_pmax_dedx_FXT[NSpecies]   = {  1.3, 0.6, 2.9,  2.5,  3.4,     8.0,      8.0};	
// 	static const double Species_pmax_tof_FXT[NSpecies]    = {  1.8, 1.8, 4.0,  5.0,  6.0,     8.0,      8.0};	
	//
	static const double Species_Charge[NSpecies]          = {    1,   1,   1,  0,  1,    0,0,0};	//,    1,       2,        2};	
	static const double Species_Z[NSpecies]               = {    0,   0,   1,  0,  1,    0,0,0};	//,    1,       2,        2};	
	static const double Species_A[NSpecies]               = {    0,   0,   1,  1,  2,    0,0,0};	//,    3,       3,        4};	
	//
// 	static const double Species_Ddedxcut[NSpecies]        = {    0,   0,   0,    1,    1,       3,        2};	
// 	static const double Species_bichdiffcutu[NSpecies]    = {    0, 0.0, 0.0,  1.0,  1.1,     1.0,      1.0};	
// 	static const double Species_bichdiffcutd[NSpecies]    = {    0, 0.0, 0.0,  1.0,  1.1,     1.0,      1.0};	
// 	static const double Species_ynbfxt[NSpecies]	= {  28,  24,  22,  16,  18,  18,  16};	
// 	static const double Species_ylfxt[NSpecies]		= {-1.0,-1.0,-1.0,-1.0,-1.0,-1.0,-1.0};	
// 	static const double Species_yufxt[NSpecies]		= { 0.4, 0.2, 0.1,-0.2,-0.1,-0.1,-0.2};	
	//
	//	ParticleID = SpeciesID for chg>0, and =SpeciesID+NSpecies for chg<0
	//
	static const int NPARTICLENAMES					= 13;
	static const int kParticleIDPionPlus			=  0;
	static const int kParticleIDKaonPlus			=  1;
	static const int kParticleIDProton				=  2;
	static const int kParticleIDNeutron				=  3;
	static const int kParticleIDDeuteron			=  4;
	static const int kParticleIDPionMinus			=  5;
	static const int kParticleIDKaonMinus			=  6;
	static const int kParticleIDAntiProton			=  7;
	static const int kParticleIDAntiNeutron			=  8;
	static const int kParticleIDAntiDeuteron		=  9;	
	static const int kParticleIDKshort				= 10;
	static const int kParticleIDLambda				= 11;
	static const int kParticleIDAntiLambda			= 12;
//	static const int kParticleIDTritonPlus			=  5;
//	static const int kParticleID3HePlus				=  6;
//	static const int kParticleID4HePlus				=  7;
//	static const int kParticleIDTritonMinus			= 13;
//	static const int kParticleID3HeMinus			= 14;
//	static const int kParticleID4HeMinus			= 15;
	static const char *ParticleIDNames[NPARTICLENAMES] = {"#pi+", "K+", "p"      , "n"      , "d"      ,	// "t+","^{3}He+","#alpha+",
											   		 	  "#pi-", "K-", "#bar{p}", "#bar{n}", "#bar{d}",	// "t-","^{3}He-","#alpha-" 
											   		 	  "K^{0}_{S}","#Lambda_{0}","#bar{#Lambda}_{0}"  };
	static const int ParticlePDGids[NPARTICLENAMES]	= { 211, 321, 2212, 2112, 1000010020,	//  1000010030, 1000020030, 1000020040,
	                                                   -211,-321,-2212,-2112,-1000010020,	// -1000010030,-1000020030,-1000020040 
	                                                    310, 3122, -3122};
	static const int ParticleBaryNum[NPARTICLENAMES]= { 0, 0,  1,  1,  2,					//  1000010030, 1000020030, 1000020040,
	                                                    0, 0, -1, -1, -2,					// -1000010030,-1000020030,-1000020040 
	                                                    0, 1, -1 };
	static const int ParticleCharge[NPARTICLENAMES]= { 1,1,1,1,1,							//  1000010030, 1000020030, 1000020040,
	                                                  -1,-1,-1,-1,-1,						// -1000010030,-1000020030,-1000020040 
	                                                   0, 0, 0 };
	//
	//
	static const int NR2TYPES	= 3;	// used in reader to get hists. Also defined in class CalcRm - ensure consistency!!!
	//
	static const int NTRIGS		= 39;
	static const char* TrigNames[64]	= { "Clock",				// 0
											"ZDCs",					// 1
											"ZDCn",					// 2
											"ZDCns",				// 3
											"HCAL Singles/Coincidence",		// 4
											"Clock2",				// 5
											"",						// 6
											"",						// 7
											"MBDs>0",				// 8
											"MBDn>0",				// 9
											"MBDns>0",				//10
											"MBDns>1",				//11
											"MBDns>0+vtx<10",		//12
											"MBDns>0+vtx<30",		//13
											"MBDns>0+vtx<60",		//14
											"HCALsingles+MBDns>0",	//15
											"Jet6GeV+MBDns>0",		//16
											"Jet8GeV+MBDns>0",		//17
											"Jet10GeV+MBDns>0",		//18
											"Jet12GeV+MBDns>0",		//19
											"Jet6GeV",				//20
											"Jet8GeV",				//21
											"Jet10GeV",				//22
											"Jet12GeV",				//23
											"Photon2GeV+MBDns>0",	//24
											"Photon3GeV+MBDns>0",	//25
											"Photon4GeV+MBDns>0",	//26
											"Photon5GeV+MBDns>0",	//27
											"Photon2GeV",			//28
											"Photon3GeV",			//29
											"Photon4GeV",			//30
											"Photon5GeV",			//31
											"Jet6GeV+MBDns>0+vtx<10",		//32
											"Jet8GeV+MBDns>0+vtx<10",		//33
											"Jet10GeV+MBDns>0+vtx<10",		//34
											"Jet12GeV+MBDns>0+vtx<10",		//35
											"Photon3GeV+MBDns>0+vtx<10",	//36
											"Photon4GeV+MBDns>0+vtx<10",	//37
											"Photon5GeV+MBDns>0+vtx<10",	//38
											"",		//39
											"",		//40
											"",		//41
											"",		//42
											"",		//43
											"",		//44
											"",		//45
											"",		//46
											"",		//47
											"",		//48
											"",		//49
											"",		//50
											"",		//51
											"",		//52
											"",		//53
											"",		//54
											"",		//55
											"",		//56
											"",		//57
											"",		//58
											"",		//59
											"",		//60
											"",		//61
											"",		//62
											"" };	//63

//----- END, rest older....	
	
// 	static const int NE					=  11;
// 	static const float roots[NE] 		= { 7.7 ,  11.5,   14.5 ,   19.6,    27,    39,   62.4,   200,   3.0,    200,   200  };
// 	static const char* energies[NE]		= {"7.7", "11.5", "14.5", "19.6",  "27",  "39", "62.4", "200", "3.0",  "200",  "200" };
// 	static const char* estrings[NE]		= {"007", "011",   "015",  "019", "027", "039",  "062", "200", "003","20011","20014_83M" };
// 	static const int   edsid[NE]		= {  19 ,   20 ,     31 ,    23 ,   25 ,   18 ,    17 ,    16,    48,     24,    32  };
	//
	//static const char* npartfiles[NE]	= {"007","011","019","019","027","039","062","200run11","3.0fxt","009"};
	//
// 	static const int NMODEL = 2;
// 	static const char* modnames[NMODEL] = {"urqmd","pythia"};	
	//
// 	static const int NCent			=  17;
// 	static const int NCentUse		=  10;
// 	static const int NPart			=   8;
// 	static const int NBIN			=   2;
// 	static const int NCUM			=   9;
	//
// 	static const char *particleids[NPart]	= {"#pi^{+}", "#pi^{-}", 
// 	                            	 			"K^{+}"  , "K^{-}"  , 
// 	                            	 			"p"      , "#bar{p}",
// 	                            	 			"e^{+}"  , "e^{-}"};
// 	static const char* centnames17[NCent]	= {"MB","75-80%","70-75%","65-70%","60-65%",
// 								  				"55-60%","50-55%","45-50%","40-45%","35-40%",
// 								  				"30-35%","25-30%","20-25%","15-20%","10-15%",
// 												"5-10%","0-5%"};
// 	static const char* centnames[NCentUse]	= {"MB","70-80%","60-70%","50-60%","40-50%",
// 								  				"30-40%","20-30%","10-20%","5-10%","0-5%"};
// 	static const char* centstrings[NCentUse]= {"MB","7080","6070","5060","4050",
// 								  				"3040","2030","1020","0510","0005"};
// 	static const char* centnamessim[12]		= {"MB","100-90%","90-80%","70-80%","60-70%","50-60%","40-50%",
// 								  				"30-40%","20-30%","10-20%","5-10%","0-5%"};
// 	//
// 	static const char* centnamesSD0[NCentUse]	= {"MB","70-80%","60-70%","50-60%","40-50%",
// 								  					"30-40%","20-30%","10-20%","5-10%","0-5%"};
// 	static const char* centnamesSD1[NCentUse]	= {"MB","30-35%","25-30%","20-25%","15-20%",
// 								  					"10-15%","7.5-10%","5-7.5%","2.5-5%","0-2.5%"};
// 	static const char* centnamesSD2[NCentUse]	= {"MB","10-12.5%","8.75-10%","7.5-8.75%","6.25-7.5%",
// 								  					"5-6.25%","3.75-5%","2.5-3.75%","1.25-2.5%","0-1.25%"};
// 	static const char *particlenames[NPart]	= {"#pi^{+}+#pi^{-}", "#pi^{+}-#pi^{-}",
// 	                            	   			"K^{+}+K^{-}"    , "K^{+}-K^{-}"    ,
// 	                            	   			"p+#bar{p}"      , "p-#bar{p}"      };	                               
// 	static const char* cumnames[NCUM]		= {"C_{1}","C_{2}","C_{3}","C_{4}",
// 												"R_{12}","R_{31}",
// 												"R_{32}","R_{42}","R_{43}"};


