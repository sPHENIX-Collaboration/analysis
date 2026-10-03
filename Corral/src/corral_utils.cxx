#include "corral_class.h"
#include "fluct_common.h"	// SIPHIMASK windows (README_PID.md sec 10.3)

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
	//---- $CORRAL_TREES if set, else the WSU copy of ana573 below (ana532 until 2026-09-27).
	//----   e.g. export CORRAL_TREES='/path/to/production/outputCollect_*.root'  (quote the glob)
	const char* env	= getenv("CORRAL_TREES");
	TString GLOB	= (env && *env) ? TString(env) : TString("/rs/rs_grp_rhi/sPHENIX/ana573/outputCollect_*.root");
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
  , doOldPID(false)
  , doSiPhiMask(true)
  , doCMMask(true)		// README_SplitTracks573 sec 38: default ON (2026-09-30), w in [-0.07,0.10); "nocmmask" off, "cmmaskAABB" override
  , valCMMaskLo(-0.07)
  , valCMMaskHi(0.10)
  , nSiPhiMask(NSIPHIMASK_MAIN)
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
  , doLooperVeto(true)	// README_SplitTracks573 sec 35 (2026-09-29): ON; "nolooper" turns it off
  , valLooperRSum(0.08)	// sec 35: default 0.08 (2026-09-29, tests at 0.04/0.06/0.08); "looperNN" (0.02-0.10)
  , valZvtxNB(16)		// 16 zvtx slices = 2 cm over +-16 cm (sec 38: finer slices do not help)
  , doULSTest(true)		// ON by default as of 2026-09-22 (user's call, after sec 15.7/15.8): the ULS
						// pair-level veto (drop flagged pi+pi- SIBLING pair; sec 16.7: never mixed) at the
						// sec 15.7 working point below. "noulstest" in RunString turns it off;
						// "ulstestNN" overrides the threshold. Independently re-derived and reconfirmed
						// after the sec 16 restart, via a different method (fixed-k windowed survival
						// function, sec 18.5/18.7): same conclusion both times.
  , doDauCheck(true)	// README_CQcomparison sec 2.2 (2026-10-03, option 2): ON; "nodau" turns it off
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
  , doTFDup(true)			// 2026-09-26: ON by default -- "noTFdup" turns it off (no-op without a bco branch)
  , nTFDup_rows(0), TFDup_maxAbsDvz(0)
  , ONLY_FIRST_XINGPOS(false)	// sec 18.21 follow-up 2, OFF by default -- "XingPos" turns it on
  , ONLY_FIRST_TF(false)		// sec 18.21, OFF by default -- "onlyfirsttf" in RunString turns it on
  , NTPCCUT(18)				// LL=8 (inclusive), 18 is open but better
  , PTMINCUT(0.1)
  , valFullMVTX(2), valFullMVTXdinv(0.0), valPregateDEta(0.040)	// README_SplitTracks573 sec 14/15/18:
  , valTSepDy(0.06), valTSepDphi(2.0), valISepDphi(2.), valVzMax(16.), valNchLo(0), valNchHi(9999), valFinZMax(8.)							// constants except dps / tsepm (tokens)
  , valDPsW(2.0), valDPsR(0.06)
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
	//---- README_PID.md (2026-09-28): PID from the KFP dE/dx gates (pi, then p, then K) is the DEFAULT.
	//---- "oldpid" goes back to the legacy pion-only cut (dedx70s<400, any momentum).
	if (RunString.Contains("oldpid",TString::kIgnoreCase)){
		doOldPID = true;
		cout<<"RunString="<<RunString<<"\t doOldPID="<<(int)doOldPID<<endl;
	}
	//---- README_PID.md sec 10.3 (2026-09-29): the ana573 vertex-phi mask is the DEFAULT; "nosiphimask" turns it off.
	if (RunString.Contains("nosiphimask",TString::kIgnoreCase)){
		doSiPhiMask = false;
		cout<<"RunString="<<RunString<<"\t doSiPhiMask="<<(int)doSiPhiMask<<endl;
	}
	if (RunString.Contains("siphimaskall",TString::kIgnoreCase)){
		nSiPhiMask = NSIPHIMASK;
		cout<<"RunString="<<RunString<<"\t nSiPhiMask="<<nSiPhiMask<<" (all vertex-phi windows)"<<endl;
	}
	//---- README_SplitTracks573 sec 18: value tokens are accepted only within sane limits; anything else
	//---- (including a malformed number) stops the job, it is never silently clamped.
	auto saneOrDie	= [&](const char* tok, double v, double lo, double hi){
		if (v<lo || v>hi){
			cout<<"reader::reader -- run-string token '"<<tok<<"' gives "<<v<<", outside the sane range ["<<lo<<","<<hi<<"]. Exiting."<<endl;
			exit(1);
		}
	};
	if (RunString.Contains("ntpc",TString::kIgnoreCase)){
		int ij			= RunString.Index("ntpc",0,TString::kIgnoreCase);
		TString sntpc	= RunString(ij+4,2);
		NTPCCUT			= sntpc.IsDigit() ? atoi(sntpc.Data()) : -1;
		saneOrDie("ntpcNN",NTPCCUT,10,40);
		cout<<"RunString="<<RunString<<"\t      sntpc="<<sntpc<<"\t NTPCCUT="<<NTPCCUT<<endl;
	}
	//
	//
	//---- README_SplitTracks573 sec 18 (token trim): the LS pregate half-width "dpsNN" (1.0-4.0 deg) and the
	//---- two-track cut "tsepmYYPP" (abs(dy) 0.02-0.10, abs(dphi*) 1.0-4.0 deg) / "notsep". Everything else of
	//---- the ana573 LS working point is a constant (corral_class.h). Checked clean against every other
	//---- token (none contains "dps" or "tsep"); "notsep" is tested before "tsepm".
	if (RunString.Contains("dps",TString::kIgnoreCase)){
		int ij	= RunString.Index("dps",0,TString::kIgnoreCase);
		TString sw = RunString(ij+3,2);
		valDPsW	= sw.IsDigit() ? atoi(sw.Data())/10. : -1;
		saneOrDie("dpsNN",valDPsW,1.0,4.0);
	}
	if (RunString.Contains("notsep",TString::kIgnoreCase)){
		valTSepDy	= 0.;
	} else if (RunString.Contains("tsepm",TString::kIgnoreCase)){
		int ij	= RunString.Index("tsepm",0,TString::kIgnoreCase);
		TString sy = RunString(ij+5,2), sp = RunString(ij+7,2);
		valTSepDy	= sy.IsDigit() ? atoi(sy.Data())/100. : -1;
		valTSepDphi	= sp.IsDigit() ? atoi(sp.Data())/10.  : -1;
		saneOrDie("tsepmYYPP (dy)",valTSepDy,0.02,0.10);
		saneOrDie("tsepmYYPP (dphi*)",valTSepDphi,1.0,4.0);
	}
	//---- near-vertex two-track cut (sec 29): ON by default at 2.0 deg (user, 2026-09-28). "noisep" turns it off (tested
	//---- first: it contains "isep"); "isepPPP" sets the half-width, 0.3-4.0 deg. No other token contains "isep".
	if (RunString.Contains("noisep",TString::kIgnoreCase)){
		valISepDphi	= 0.;
	} else if (RunString.Contains("isep",TString::kIgnoreCase)){
		int ij	= RunString.Index("isep",0,TString::kIgnoreCase);
		TString sw = RunString(ij+4,3);
		valISepDphi	= sw.IsDigit() ? atoi(sw.Data())/10. : -1;
		saneOrDie("isepPPP",valISepDphi,0.3,4.0);
	}
	//---- "vzNN": event abs(zvtx) max in cm, 6-16 (sec 30). Checked clean against every other token (none contains "vz").
	if (RunString.Contains("vz",TString::kIgnoreCase)){
		int ij	= RunString.Index("vz",0,TString::kIgnoreCase);
		TString sw = RunString(ij+2,2);
		valVzMax	= sw.IsDigit() ? atoi(sw.Data()) : -1;
		saneOrDie("vzNN",valVzMax,6.,16.);
	}
	cout<<"reader::reader -- event abs(zvtx) < "<<valVzMax<<" cm"<<endl;
	//---- "nchLLHH": event multiplicity class LL <= N_ch <= HH (README_CQcomparison.md sec 2.1). Checked clean against every
	//---- other token (none contains "nch").
	if (RunString.Contains("nch",TString::kIgnoreCase)){
		int ij	= RunString.Index("nch",0,TString::kIgnoreCase);
		TString sl = RunString(ij+3,2), sh = RunString(ij+5,2);
		valNchLo	= sl.IsDigit() ? atoi(sl.Data()) : -1;
		valNchHi	= sh.IsDigit() ? atoi(sh.Data()) : -1;
		saneOrDie("nchLLHH (LL)",valNchLo,0,40);
		saneOrDie("nchLLHH (HH)",valNchHi,valNchLo,99);
		cout<<"reader::reader -- multiplicity class: only events with "<<valNchLo<<" <= N_ch <= "<<valNchHi<<" reach CalcRm"<<endl;
	}
	//---- "fzNN": Finalize's physics abs(zvtx) range, 2-16 cm (sec 30). Checked clean (no other token contains "fz").
	if (RunString.Contains("fz",TString::kIgnoreCase)){
		int ij	= RunString.Index("fz",0,TString::kIgnoreCase);
		TString sw = RunString(ij+2,2);
		valFinZMax	= sw.IsDigit() ? atoi(sw.Data()) : -1;
		saneOrDie("fzNN",valFinZMax,2.,16.);
	}
	cout<<"reader::reader -- near-vertex two-track pair cut (isep) "<<(valISepDphi>0?Form("ON: reject abs(dphi*(R=3 cm))<%.1f deg at abs(dy)<1.0, LS and ULS, sibling and mixed",valISepDphi):"OFF")<<endl;
	cout<<"reader::reader -- LS pregate: centred on the split peak, abs(dphi) within "<<valDPsW<<" deg of dphi0(R="<<valDPsR<<" m), deta semi-axis "<<valPregateDEta<<endl;
	//---- "noMVTX": OFF switch for LS path 3 (the MVTX-match path), README_SplitTracks573 sec 22. Checked clean
	//---- against every other token (none contains "mvtx").
	if (RunString.Contains("noMVTX",TString::kIgnoreCase)) valFullMVTX = 0;
	cout<<"reader::reader -- LS path 3 (MVTX match): "<<(valFullMVTX>0?Form(">= %d matched MVTX layers",valFullMVTX):"DISABLED (noMVTX)")<<endl;
	cout<<"reader::reader -- two-track LS pair cut "<<(valTSepDy>0?Form("ON: reject abs(dy)<%.2f && min over R=0.30-0.78 m of abs(dphi*)<%.1f deg, sibling and mixed",valTSepDy,valTSepDphi):"OFF (notsep)")<<endl;
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
	if (RunString.Contains("skf",TString::kIgnoreCase)){
		int ij		= RunString.Index("skf",0,TString::kIgnoreCase);
		TString sskf	= RunString(ij+3,2);
		valSiKeyCut	= sskf.IsDigit() ? atoi(sskf.Data())/100. : -1;
		saneOrDie("skfNN",valSiKeyCut,0.05,0.95);
		cout<<"reader::reader -- pre-pass SiSplitScore gate overridden via 'skf' token... valSiKeyCut = "<<valSiKeyCut<<endl;
	}
	//---- SL threshold VALUE override for the same pre-pass -- "removecutNN" (e.g. for a future
	//---- re-scan), without touching whether the pre-pass itself is on. If omitted, uses valSLCut's
	//---- default (0.18, sec 17.6).
	if (RunString.Contains("removecut",TString::kIgnoreCase)){
		int ij			= RunString.Index("removecut",0,TString::kIgnoreCase);
		TString sremove	= RunString(ij+9,2);
		valSLCut		= sremove.IsDigit() ? atoi(sremove.Data())/100. : -1;
		saneOrDie("removecutNN",valSLCut,0.05,0.60);
		cout<<"reader::reader -- pre-pass SL threshold overridden via 'removecut' token... valSLCut = "<<valSLCut<<endl;
	}
	//---- RadialGap threshold VALUE override for the pre-pass's SECOND flagging path (sec 17.14) --
	//---- "radialgapNN" (e.g. for a future re-scan), without touching whether the pre-pass itself is
	//---- on. If omitted, uses valRadialGapCut's default (0.45, sec 17.14).
	if (RunString.Contains("radialgap",TString::kIgnoreCase)){
		int ij			= RunString.Index("radialgap",0,TString::kIgnoreCase);
		TString srg		= RunString(ij+9,2);
		valRadialGapCut	= srg.IsDigit() ? atoi(srg.Data())/100. : -1;
		saneOrDie("radialgapNN",valRadialGapCut,0.20,0.90);
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
	//---- README_SplitTracks573 sec 38: central-membrane mask, default ON, w in [-0.07,0.10); "nocmmask" = off; "cmmaskAABB" = w in [-0.AA, 0.BB) (sane: each 0.00-0.20)
	if (RunString.Contains("nocmmask",TString::kIgnoreCase)){
		doCMMask	= false;
		cout<<"RunString="<<RunString<<"\t doCMMask="<<(int)doCMMask<<endl;
	} else if (RunString.Contains("cmmask",TString::kIgnoreCase)){
		int ij		= RunString.Index("cmmask",0,TString::kIgnoreCase);	// the occurrence followed by 4 digits
		while (ij>=0 && !TString(RunString(ij+6,4)).IsDigit()) ij = RunString.Index("cmmask",ij+1,TString::kIgnoreCase);
		if (ij<0) ij = RunString.Index("cmmask",0,TString::kIgnoreCase);
		TString sl	= RunString(ij+6,2), sh = RunString(ij+8,2);
		if (!sl.IsDigit() || !sh.IsDigit()){ cout<<"RunString="<<RunString<<": cmmask needs 4 digits (cmmaskAABB)"<<endl; exit(1); }
		doCMMask	= true;
		valCMMaskLo	= -atoi(sl.Data())/100.; valCMMaskHi = atoi(sh.Data())/100.;
		saneOrDie("cmmaskAABB (lo)",-valCMMaskLo,0.,0.20); saneOrDie("cmmaskAABB (hi)",valCMMaskHi,0.,0.20);
		cout<<"RunString="<<RunString<<"\t central-membrane mask ON, w in ["<<valCMMaskLo<<","<<valCMMaskHi<<")"<<endl;
	}
	//---- README_SplitTracks573 sec 35: the looper veto is ON by default; "nolooper" turns it off.
	if (RunString.Contains("nolooper",TString::kIgnoreCase)){
		doLooperVeto	= false;
		cout<<"RunString="<<RunString<<"\t doLooperVeto="<<(int)doLooperVeto<<endl;
	} else if (RunString.Contains("looper",TString::kIgnoreCase)){		// "looperNN": the relative-sum cut, sane range only
		int ij		= RunString.Index("looper",0,TString::kIgnoreCase);
		TString sl	= RunString(ij+6,2);
		if (sl.IsDigit()){ valLooperRSum = atoi(sl.Data())/100.; saneOrDie("looperNN",valLooperRSum,0.02,0.10); }
		cout<<"RunString="<<RunString<<"\t looper veto relative-sum cut = "<<valLooperRSum<<endl;
	}
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
		valULSTestCut	= sulstest.IsDigit() ? atoi(sulstest.Data())/100. : -1;
		saneOrDie("ulstestNN",valULSTestCut,0.02,0.95);
	}
	cout<<"reader::reader -- ULS pair veto (sec 15) "<<(doULSTest?"enabled":"DISABLED")
		<<"... valULSTestCut = "<<valULSTestCut<<endl;
	//---- README_CQcomparison sec 2.2 (2026-10-03, option 2): V0-daughter partner check, ON; "nodau" turns it off.
	//---- Checked clean: no other token contains "nodau".
	if (RunString.Contains("nodau",TString::kIgnoreCase))	doDauCheck	= false;
	cout<<"reader::reader -- V0-daughter partners (README_CQcomparison sec 2.2): "<<(doDauCheck?"on (Lambda/Lbar daughters: LS paths dR<0.05 except pion-tagged partners of a p/pbar, ULS never an (anti)proton; not K0S)":"OFF")<<endl;
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
	if (RunString.Contains("XTFclean",TString::kIgnoreCase)){		// sec 18: non-strict ("XTFcleanStrict" dropped: it is the default)
		doXTFClean		= true;
		doXTFStrict		= false;
	}
	if (RunString.Contains("mixAdjTF",TString::kIgnoreCase)){
		doMixNoAdjTF	= false;
	}
	cout<<"reader::reader -- doMixNoAdjTF (sec 18.23) "<<(doMixNoAdjTF?(doXTFClean?"enabled -- skip |devt|==1":"enabled -- XTFclean OFF, so skip |devt|<=1 (same-TF too)"):"DISABLED")<<endl;
	cout<<"reader::reader -- doXTFClean (sec 18.22) "<<(doXTFClean?"enabled":"DISABLED")
		<<(doXTFStrict?" -- STRICT (neither-case: drop both copies)":"")<<endl;
	//---- overlapping-TF collision copies (needs the Collect "bco" branch). "noTFdup" checked clean
	//---- against every other token (none contains "tfdup").
	if (RunString.Contains("noTFdup",TString::kIgnoreCase)){
		doTFDup	= false;
	}
	cout<<"reader::reader -- doTFDup (overlapping-TF copies, key run+bco+crossing) "<<(doTFDup?"enabled":"DISABLED")<<endl;
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
	//---- total entries of the default glob, hardcoded to skip the slow GetEntries() rescan. Used ONLY
	//---- for the default glob; -l lists and $CORRAL_TREES are counted below. If files are added to the
	//---- default dataset, update the value (lists/count_entries.C) or set it to 0 to force the rescan.
	const Long64_t nentriesfile_ana532	= 50498542;		// ana532 (was run_ecuts_cf) re-cut (5k TF/segment, sec 12/13),
														// all 1554 files, live GetEntries() 2026-09-21
	const Long64_t nentriesfile_ana573	= 642480426;	// ana573 complete: all 4581 files (79514 798, 79515 1891, 79516 1892
														// chunks; the 79514 rerun landed 2026-09-28), count_entries.C
														// 2026-09-28 (lists/ana573/segment_entries.txt, 0 unreadable)
	(void)nentriesfile_ana532;
	const char* envTrees	= getenv("CORRAL_TREES");
	if (tree==0 && !(envTrees && *envTrees))	// default glob (ana573) only
	nentriesfile	= nentriesfile_ana573;
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
