#include "corral_class.h"

//------------------------------------------------------------
// Build (or adopt) the input TChain. This must not touch any other member of
// 'this' -- it runs in the constructor's member-initializer-list, before
// fReader (and everything bound to fReader) exists yet.
TChain* corral::BuildInputChain(TTree *tree){
	//
	if (tree != 0){
		// caller supplied a tree/chain directly (e.g. for local testing against
		// a single already-open file/tree rather than the default grid glob)
		TChain *given = dynamic_cast<TChain*>(tree);
		if (given) return given;
		TChain *wrap = new TChain(tree->GetName());
		wrap->Add(tree->GetCurrentFile()->GetName());
		return wrap;
	}
	//
	//---- no job list (-l): the whole production in one job (useful for testing). The file glob is
	//---- $CORRAL_TREES if set, else the WSU copy of ana532 below.
	//----   e.g. export CORRAL_TREES='/path/to/production/outputCollect_*.root'  (quote the glob)
	const char* env	= getenv("CORRAL_TREES");
	TString GLOB	= (env && *env) ? TString(env) : TString("/rs/rs_grp_rhi/sPHENIX/ana532/outputCollect_*.root");
	TChain *chain	= new TChain("outTree","Collect tree chain");
	int nf			= chain->Add(GLOB.Data());
	cout<<"corral -- input trees "<<GLOB<<((env && *env)?"  ($CORRAL_TREES)":"  (default; set CORRAL_TREES to change)")<<": "<<nf<<" files"<<endl;
	if (nf==0) cout<<"corral -- WARNING: no input files match "<<GLOB<<" (unmounted disk? wrong CORRAL_TREES?)"<<endl;
	return chain;
}

//------------------------------------------------------------
corral::corral(TTree *tree, int ntd, TString runstr, TString outname)
  : ntodo(ntd)
  , RunString(runstr)
  , OutputName(outname)
  , doCrossing(true)
  , doQCut(false)
  , FinePhiBinning(false)	// only set true below if RunString requests it
  , valQCut(-1.0)
  , valSLCut(0.18)		// pre-pass production default (sec 17.6, superseding sec 14.1's 0.60):
						// round-2/refinement LS SL x SKF survey's SKF>=0.10 row crosses its
						// aggregate canaries (P/H ring, C(Q) 10-20MeV, R2 step) around SL~0.13-0.14,
						// but the zoom maps show residual "satellite" structure flanking the (0,0)
						// ridge that only clears by SL~0.17; 0.18 is the nearest grid point past
						// that, at a small extra cost (~13% relative) in kept sibling pairs vs 0.14.
  , valSiKeyCut(0.10)	// pre-pass production default (sec 17.6, superseding sec 14.1's 0.20): of
						// the round-2/refinement grid's four SKF gates (10/40/60/70/90), only
						// SKF>=0.10 reaches the genuinely-partial-MVTX, INTT-disagreeing candidate
						// population (SiSplitScore~.167/.333) that sec 17.4/17.5 showed is real
						// duplicate signal, not background (position concentration 46-53% vs 10.1%
						// for the mvtxFrac==0 reference) -- gates >=0.40 silently leave those
						// genuine duplicates unflagged. sec 17.5: SiSplitScore (MVTX-primary/
						// INTT-secondary, sec 13.9) itself remains the discriminator -- checked
						// directly against a pure MVTX-only fraction ("MKF") and kept, INTT's
						// tiebreak vote in the partial band measurably tracks real duplicates
						// rather than adding noise.
  , valRadialGapCut(0.45)	// sec 17.14 second-path threshold, see corral_class.h member comment --
							// already-measured sideband dividing line (sec 17.11), not yet a scanned
							// optimum.
  , DISABLE_LScuts(false)	// sec 17.9: default OFF, i.e. the pregate + SL/SKF/RadialGap split-track
							// pre-pass is ON by default. "noLS" in RunString sets this true and disables it.
  , DISABLE_RG(false)	// sec 17.17: default OFF, i.e. the RadialGap path (path 2) is ON by default
						// alongside path 1. "noRG" in RunString sets this true and disables path 2 only.
  , doSplitRemoval(true)	// ON by default as of sec 13.8.18/14.1, values finalized sec 17.6, switch
							// philosophy inverted sec 17.9 -- this pre-pass, at the valSLCut/
							// valSiKeyCut defaults above (the finalized LS working point, sec 17.6),
							// is the production split-track mitigation, driven by !DISABLE_LScuts
							// (recomputed below once RunString is parsed). With NO RunString at all
							// ("-s" omitted, or "-s \"\""), these defaults ARE the present best-effort
							// split-track removal -- no token is required to get it (sec 14.4).
  , doULSTest(true)		// ON by default as of 2026-09-22 (user's call, after sec 15.7/15.8): the ULS
						// pair-level veto (drop flagged pi+pi- SIBLING pair; sec 16.7: never mixed) at the
						// sec 15.7 working point below. "noulstest" in RunString turns it off;
						// "ulstestNN" overrides the threshold. Independently re-derived and reconfirmed
						// after the sec 16 restart, via a different method (fixed-k windowed survival
						// function, sec 18.5/18.7): same conclusion both times.
  , valULSTestCut(0.05)		// sec 15.7 full-stats 14-point scan: best non-degenerate point (>=0.05,
							// 0.10, 0.15 bit-identical; >=0.00 degenerate). Was 0.60 (sec 15.6 reuse).
							// sec 18.7 (2026-09-23): reconfirmed via fine 0.01/0.02/0.03 grid -- gap below
							// 0.05 is empty (SiSplitScore is quantized: 0 by default, jumps straight to a
							// discrete nonzero value), and the 0.05-0.99 falloff is gradual with no sharp
							// optimum, so this value is not tuned to a peak -- it just clears the floor.
  , ONLY_CROSSING0(false)		// sec 18.21 follow-up, OFF by default -- "Xing0" turns it on
  , doXTFClean(true)		// sec 18.22/18.25: ON by default -- "noXTF" turns it (and the two below) off
  , doMixNoAdjTF(true)	// sec 18.23/18.25: ON by default -- "noXTF" or "mixAdjTF" turns it off
  , doXTFStrict(true)		// sec 18.22/18.25: ON by default -- "noXTF" off; plain "XTFclean" selects non-strict
  , nXTF_pairs(0), nXTF_byINTT(0), nXTF_byQuality(0), nXTF_neither(0), nXTF_losers(0)
  , ONLY_FIRST_XINGPOS(false)	// sec 18.21 follow-up 2, OFF by default -- "XingPos" turns it on
  , ONLY_FIRST_TF(false)		// sec 18.21, OFF by default -- "onlyfirsttf" in RunString turns it on
  , NTPCCUT(18)				// LL=8 (inclusive), 18 is open but better
  , hRunIndex(0)
  , nentriesfile(0)
  , fChain(BuildInputChain(tree))
  , fReader(fChain)
  , run(fReader,"run")
  , segment(fReader,"segment")
  , evt(fReader,"evt")
  , trigVec(fReader,"trigVec")
  , nvtx(fReader,"nvtx")
  , vtxntr(fReader,"vtxntr")
  , vtxx(fReader,"vtxx")
  , vtxy(fReader,"vtxy")
  , vtxz(fReader,"vtxz")
  , crossing(fReader,"crossing")
  , etotem(fReader,"etotem")
  , etotih(fReader,"etotih")
  , etotoh(fReader,"etotoh")
  , etotioh(fReader,"etotioh")
  , etotepd(fReader,"etotepd")
  , ntr(fReader,"ntr")
  , pid(fReader,"pid")
  , indv0(fReader,"indv0")
  , primary(fReader,"primary")
  , quality(fReader,"quality")
  , chisq(fReader,"chisq")
  , chg(fReader,"chg")
  , ptot(fReader,"ptot")
  , eta(fReader,"eta")
  , phi(fReader,"phi")
  , pt(fReader,"pt")
  , seedeta(fReader,"seedeta")
  , seedphi(fReader,"seedphi")
  , seedpt(fReader,"seedpt")
  , ntpc(fReader,"ntpc")
  , nmvtx(fReader,"nmvtx")
  , nintt(fReader,"nintt")
  , dcaxy(fReader,"dcaxy")
  , dcaz(fReader,"dcaz")
  , dedx70n(fReader,"dedx70n")
  , dedx70s(fReader,"dedx70s")
  , dedxKFP(fReader,"dedxKFP")
  , layermask(fReader,"layermask")
  , siclukey(fReader,"siclukey")
  , tpcsector(fReader,"tpcsector")
  , tpcarclen(fReader,"tpcarclen")
  , tpcz(fReader,"tpcz")
  , nv0(fReader,"nv0")
  , v0pid(fReader,"v0pid")
  , v0x(fReader,"v0x")
  , v0y(fReader,"v0y")
  , v0z(fReader,"v0z")
  , v0px(fReader,"v0px")
  , v0py(fReader,"v0py")
  , v0pz(fReader,"v0pz")
  , v0pt(fReader,"v0pt")
  , v0ptot(fReader,"v0ptot")
  , v0eta(fReader,"v0eta")
  , v0phi(fReader,"v0phi")
  , v0ene(fReader,"v0ene")
  , v0mass(fReader,"v0mass")
  , v0ctau(fReader,"v0ctau")
  , v0decaylen(fReader,"v0decaylen")
  , v0chi2ndf(fReader,"v0chi2ndf")
  , v0dira(fReader,"v0dira")
  , v0pvdca(fReader,"v0pvdca")
  , v0decayx(fReader,"v0decayx")
  , v0decayy(fReader,"v0decayy")
  , v0decayz(fReader,"v0decayz")
  , v0indtr1(fReader,"v0indtr1")
  , v0indtr2(fReader,"v0indtr2")
{
	//
	gPrintViaErrorHandler = kTRUE;
	gErrorIgnoreLevel = kWarning;
	//
	//---- README_Crossing.md (2026-09-25): crossing correction (pt-ordering + post-CF bin correction)
	//---- is the DEFAULT. "nocross" turns off both. The old "cross" token is removed.
	if (RunString.Contains("nocross",TString::kIgnoreCase)){
		doCrossing = false;
		cout<<"RunString="<<RunString<<"\t doCrossing="<<(int)doCrossing<<endl;
	}
	if (RunString.Contains("ntpc",TString::kIgnoreCase)){
		int ij			= RunString.Index("ntpc");
		TString sntpc	= RunString(ij+4,2);
		NTPCCUT			= atoi(sntpc.Data());
		cout<<"RunString="<<RunString<<"\t      sntpc="<<sntpc<<"\t NTPCCUT="<<NTPCCUT<<endl;
	}
	//
	if (RunString.Contains("qcut",TString::kIgnoreCase)){	// Default qcut is 150 MeV/c
		doQCut		= true;
		valQCut		= 0.150;
	}
	if (doQCut){ cout<<"reader::reader -- Q cut enabled... valQCut = "<<valQCut<<endl; }
	//
	//---- SiSplitScore (MVTX-led SKF) gate VALUE for the split-track REMOVAL pre-pass
	//---- (corral_loop.cxx, sec 13.8.4/17.6) -- "skfNN" overrides the VALUE only (e.g. for a future
	//---- re-scan), without touching whether the pre-pass itself is on. If omitted, uses valSiKeyCut's
	//---- default (0.10, sec 17.6).
	if (RunString.Contains("skf")){
		int ij		= RunString.Index("skf");
		TString sskf	= RunString(ij+3,2);
		valSiKeyCut	= atoi(sskf.Data())/100.;
		cout<<"reader::reader -- pre-pass SiSplitScore gate overridden via 'skf' token... valSiKeyCut = "<<valSiKeyCut<<endl;
	}
	//---- SL threshold VALUE override for the same pre-pass -- "removecutNN" (e.g. for a future
	//---- re-scan), without touching whether the pre-pass itself is on. If omitted, uses valSLCut's
	//---- default (0.18, sec 17.6).
	if (RunString.Contains("removecut",TString::kIgnoreCase)){
		int ij			= RunString.Index("removecut");
		int len			= (RunString[ij+9]=='-') ? 3 : 2;
		TString sremove	= RunString(ij+9,len);
		valSLCut		= atoi(sremove.Data())/100.;
		cout<<"reader::reader -- pre-pass SL threshold overridden via 'removecut' token... valSLCut = "<<valSLCut<<endl;
	}
	//---- RadialGap threshold VALUE override for the pre-pass's SECOND flagging path (sec 17.14) --
	//---- "radialgapNN" (e.g. for a future re-scan), without touching whether the pre-pass itself is
	//---- on. If omitted, uses valRadialGapCut's default (0.45, sec 17.14).
	if (RunString.Contains("radialgap")){
		int ij			= RunString.Index("radialgap");
		TString srg		= RunString(ij+9,2);
		valRadialGapCut	= atoi(srg.Data())/100.;
		cout<<"reader::reader -- pre-pass RadialGap threshold overridden via 'radialgap' token... valRadialGapCut = "<<valRadialGapCut<<endl;
	}
	//---- split-track REMOVAL pre-pass (corral_loop.cxx, sec 13.8.4/17.6/17.9, RadialGap path added
	//---- 17.14) -- ON by default (the finalized LS working point, sec 17.6). New philosophy as of sec
	//---- 17.9: once a cut is finalized it defaults ON, and the RunString provides only an OFF switch
	//---- -- no separate "enable" token is needed any more (superseding "removeNN"/bare "REMOVE"/
	//---- "noremove" below sec 13.8.4-era). "noLS" sets DISABLE_LScuts, turning the whole pre-pass
	//---- (both flagging paths) off; "noRG" (sec 17.17) turns off ONLY the RadialGap path, path 1
	//---- (SL/SKF) stays live -- a finer-grained switch for isolating path 2's own effect.
	if (RunString.Contains("noLS",TString::kIgnoreCase)){
		DISABLE_LScuts	= true;
	}
	if (RunString.Contains("noRG",TString::kIgnoreCase)){
		DISABLE_RG	= true;
	}
	doSplitRemoval	= !DISABLE_LScuts;
	cout<<"reader::reader -- split-track REMOVAL pre-pass "<<(doSplitRemoval?"enabled":"DISABLED")
		<<"... valSLCut = "<<valSLCut<<", valSiKeyCut = "<<valSiKeyCut<<", valRadialGapCut = "<<valRadialGapCut
		<<", RadialGap path "<<(DISABLE_RG?"DISABLED (noRG)":"enabled")<<endl;
	//
	//---- ULS veto (README sec 15/18.10/18.24) -- opposite-charge SiSplitScore TRACK-level removal
	//---- in corral_loop.cxx (no angular gate since sec 18.24), ON by default at 0.05 since 2026-09-22; completely independent of
	//---- doSplitRemoval above. "noulstest" turns it off (checked FIRST -- it contains
	//---- "ulstest" as a substring); "ulstestNN" turns it on at threshold 0.NN.
	if (RunString.Contains("noulstest",TString::kIgnoreCase)){
		doULSTest		= false;
	} else if (RunString.Contains("ulstest",TString::kIgnoreCase)){
		int ij			= RunString.Index("ulstest",0,TString::kIgnoreCase);
		TString sulstest	= RunString(ij+7,2);
		doULSTest		= true;
		valULSTestCut	= atoi(sulstest.Data())/100.;
	}
	cout<<"reader::reader -- ULS pair veto (sec 15) "<<(doULSTest?"enabled":"DISABLED")
		<<"... valULSTestCut = "<<valULSTestCut<<endl;
	//
	//---- sec 18.21: opt-in TF-deduplication diagnostic, OFF by default. "onlyfirsttf" verified
	//---- clean against every token this codebase parses (ulstest/SL/REMOVE/SIKEY/cross/qcut/ntpc/
	//---- slcut/sikeycut/removecut/radialgap/noRG/noLS).
	if (RunString.Contains("onlyfirsttf",TString::kIgnoreCase)){
		ONLY_FIRST_TF	= true;
	}
	cout<<"reader::reader -- ONLY_FIRST_TF (sec 18.21) "<<(ONLY_FIRST_TF?"enabled -- ~85% stats cost":"DISABLED")<<endl;
	//---- sec 18.21 follow-up: keep only crossing==0 (the triggered crossing, one per TF).
	//---- "Xing0" checked clean against every token above; leaving it out keeps all crossings.
	if (RunString.Contains("Xing0",TString::kIgnoreCase)){
		ONLY_CROSSING0	= true;
	}
	//---- sec 18.22/18.23, DEFAULT since sec 18.25 (2026-09-24): cross-crossing duplicate-track cleaner
	//---- (strict variant) + neighboring-TF mixed-event-pair exclusion. Tokens (all checked clean
	//---- against every token above, case-insensitive substring matching):
	//----   "noXTF"          -- turn OFF all three (restores the pre-sec-18.22 behavior exactly)
	//----   "XTFclean"       -- cleaner ON, NON-strict ("neither" case keeps the better-quality copy)
	//----   "XTFcleanStrict" -- cleaner ON, strict (the default)
	//----   "mixNoAdjTF"     -- adjacent-TF exclusion ON (the default; re-enables it after "noXTF")
	//----   "mixAdjTF"       -- adjacent-TF exclusion OFF (NOT "noAdjTF": that is a substring of "mixNoAdjTF")
	if (RunString.Contains("noXTF",TString::kIgnoreCase)){
		doXTFClean		= false;
		doXTFStrict		= false;
		doMixNoAdjTF	= false;
	}
	if (RunString.Contains("XTFclean",TString::kIgnoreCase)){		// also matched by "XTFcleanStrict"
		doXTFClean		= true;
		doXTFStrict		= RunString.Contains("XTFcleanStrict",TString::kIgnoreCase);
	}
	if (RunString.Contains("mixNoAdjTF",TString::kIgnoreCase)){
		doMixNoAdjTF	= true;
	}
	if (RunString.Contains("mixAdjTF",TString::kIgnoreCase)){
		doMixNoAdjTF	= false;
	}
	cout<<"reader::reader -- doMixNoAdjTF (sec 18.23) "<<(doMixNoAdjTF?(doXTFClean?"enabled -- skip |devt|==1":"enabled -- XTFclean OFF, so skip |devt|<=1 (same-TF too)"):"DISABLED")<<endl;
	cout<<"reader::reader -- doXTFClean (sec 18.22) "<<(doXTFClean?"enabled":"DISABLED")
		<<(doXTFStrict?" -- STRICT (neither-case: drop both copies)":"")<<endl;
	//---- sec 18.21 follow-up 2: keep only the first crossing>0 row per TF (pure streaming, <=1/TF).
	//---- "XingPos" checked clean against every token above (incl. Xing0).
	if (RunString.Contains("XingPos",TString::kIgnoreCase)){
		ONLY_FIRST_XINGPOS	= true;
	}
	cout<<"reader::reader -- ONLY_FIRST_XINGPOS (sec 18.21) "<<(ONLY_FIRST_XINGPOS?"enabled -- first crossing>0 row per TF only":"DISABLED")<<endl;
	cout<<"reader::reader -- ONLY_CROSSING0 (sec 18.21) "<<(ONLY_CROSSING0?"enabled -- crossing==0 rows only":"DISABLED -- all crossings")<<endl;
	//
	if (RunString.Contains("548",TString::kIgnoreCase)){ FinePhiBinning = true; }
	if (FinePhiBinning){ cout<<"reader::reader -- FinePhiBinning enabled..."<<endl; }
	//
	//nentriesfile	= 362192817;		// 1st pass, 240 jobs failed
	//nentriesfile	= 376229665;		// 2nd pass, all 8522 jobs successful
	//nentriesfile	= 512609163;		// 3rd pass, 8898 jobs successful -- old dataset, not this one
	//nentriesfile	= 30171889;			// run_ecuts_cf pre-recut (4k TF/segment), superseded 2026-09-21
	const char* envTrees	= getenv("CORRAL_TREES");
	if (tree==0 && !(envTrees && *envTrees))	// default ana532 glob only; -l lists and $CORRAL_TREES are counted below
	nentriesfile	= 50498542;			// ana532 (was run_ecuts_cf) re-cut (5k TF/segment, sec 12/13), all 1554 files,
										// confirmed via live GetEntries() 2026-09-21, hardcoded to skip the
										// slow rescan on reruns. If more files ever land, delete this line
										// (falls back to the GetEntries() scan below) and re-confirm.
	//
	if (nentriesfile==0){
		cout<<"Getting tree Nevt..."<<endl;
		nentriesfile	= fChain->GetEntries();
	}
	cout<<"Nentriesfile = "<<nentriesfile<<endl;
	//
}

//------------------------------------------------------------
corral::~corral()
{
	// Members destruct in reverse declaration order: all TTreeReaderValue/Array
	// members go first, then fReader, then fChain last -- exactly the order that
	// keeps each object valid while whatever depends on it is torn down. Nothing
	// here needs manual teardown.
}
