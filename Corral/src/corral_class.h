#ifndef corral_h
#define corral_h

#include <iostream>
#include <vector>
#include <TROOT.h>
#include <TRandom3.h>
#include <TChain.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TError.h>
#include <unordered_map>

using std::vector;
using std::cout;
using std::endl;

static const bool JUSTFILLRUNSEG	= false;	// make true in first pass through new data...
static const bool USESEEDTRACKS		= false;	// get eta,phi,pt from tpc seed not svtxtrack...
static const bool NOCORRELATIONS	= false;	// skip the correlations classes for speed...
// KILLSPLITTRACKS (same-Si-seed proxy pre-pass) retired 2026-09-19 -- see README_SplitTracks.md
// sec 9/10. Replaced by the split-track removal pre-pass (doSplitRemoval/valSLCut/valSiKeyCut
// below, corral_loop.cxx, finalized sec 17.6) plus a diagnostic-only SL/SKF monitor that fills
// the same "flagged (split-candidate)" QA page the old pruner used to (sec 10.9).

//------------------------------------------------------------------------------------
// corral -- TTreeReader-based replacement for the old MakeClass skeleton.
//
//   No more fixed-size C arrays (NTRKMAX/NV0MAX) mapped onto branches via
//   SetBranchAddress: every per-track/per-V0 branch below is a TTreeReaderArray,
//   which ROOT sizes to the *actual* per-entry leaf count (ntr / nv0) every time
//   GetEntry-equivalent happens. There is therefore no way for a high-multiplicity
//   event to overflow a fixed buffer and silently corrupt adjacent class members
//   (the old failure mode this class used to be vulnerable to).
//------------------------------------------------------------------------------------
class corral {
public :
	//
	corral(TTree *tree=0, int ntd=0, TString rs="", TString outname="");
	virtual ~corral();
	virtual void     Loop();
	virtual void     Finalize();		// README_Finalize.md: combine chunk outputs (RunString "Finalize")
	//
	virtual bool 	AcceptEvent();
	virtual bool 	AcceptTrack(int it);
	virtual void	BuildXTFLosers(Long64_t nentries);	// sec 18.22, see doXTFClean
	//
	virtual int		GetSpecies(int ipid){
		int ival	= -1;
		if (ipid<  5){ ival = ipid; } else
		if (ipid< 10){ ival = ipid-5; } else
		if (ipid==10){ ival = 5; } else
		if (ipid==11){ ival = 6; } else
		if (ipid==12){ ival = 7; }
		return ival;
	}
	//
	int 	ntodo;
	TString	RunString;
	TString	OutputName;		// -o: output base name (root/<name>.root, pdf/<name>.pdf); empty -> corral_<class>[_<RunString>]
	TString	Dataset;		// -d: dataset for Finalize (lists|root/<dataset>/); empty -> FINALIZE_SET (finalize_hists.h)
	bool	doCrossing;
	bool	doQCut;
	bool	FinePhiBinning;
	double	valQCut;
	double	valSLCut;	// production pre-pass's SL threshold, default 0.18 (sec 17.6)
	double	valSiKeyCut;	// production pre-pass's SiSplitScore gate, default 0.10 (sec 17.6)
	double	valRadialGapCut;	// sec 17.14: SECOND flagging path's threshold -- pairs that share a Si
								// seed (SKF>=valSiKeyCut) but fail the SL<valSLCut "duplicate" test are
								// flagged anyway if RadialGap>=this (the "complementary"/STAR-fig-1b
								// mechanism SL cannot see, sec 17.11). Default 0.45 -- sec 17.16's 12-point
								// survey (SL/SKF held fixed) confirms this lands inside the good window:
								// visible overcut hole below ~0.35, ridge fully returns above ~0.55.
	bool	DISABLE_LScuts;	// sec 17.9: "noLS" in RunString -> true, and the pregate + SL/SKF/RadialGap
							// split-track pre-pass are NOT applied (sec 17.14 folds the RadialGap path into
							// the SAME switch -- it's a refinement of this pre-pass, not a separate
							// mechanism). New philosophy as of sec 17.6/17.9: once a cut is finalized it
							// defaults ON (doSplitRemoval below = !DISABLE_LScuts), and the RunString
							// provides only the OFF switch -- no separate "enable" token needed.
	bool	DISABLE_RG;	// sec 17.17: "noRG" in RunString -> true, and ONLY the RadialGap path (path 2)
						// is skipped -- path 1 (SL/SKF "duplicate" flagging) stays live. Finer-grained
						// than DISABLE_LScuts (which turns off both paths together): lets a run isolate
						// the RadialGap path's own before/after effect specifically.
	bool	doSplitRemoval;	// split-track REMOVAL pre-pass (corral_loop.cxx, sec 13.8.4/13.8.6/13.8.18,
							// values finalized sec 17.6, RadialGap path added sec 17.14) -- the production
							// split-track mitigation. Driven by !DISABLE_LScuts (sec 17.9); uses
							// valSLCut/valSiKeyCut/valRadialGapCut as its threshold values.
	bool	doULSTest;	// ULS opposite-charge SiSplitScore removal (README sec 15) -- ON by default since
						// 2026-09-22 ("noulstest" turns it off); "ulstestNN" sets the threshold. sec 18.10:
						// converted from a CalcRm::PairInfo sibling-only PAIR veto to a TRACK-level removal
						// (folded into corral_loop.cxx's nSplitFlagged_thisevt, same mechanism as
						// doSplitRemoval/LS below) -- the pair-only version left the loser track sitting in
						// Pvec_1/Pvec_2 and hence in the mixing buffer for every OTHER pairtype, which sec
						// 18's own survey found as a genuine compact spike in the raw MIXED density
						// (hzoomM_0) at the same (dy,dphi) as the sibling ridge -- a single-particle-density
						// defect no sibling-only veto could ever reach. sec 18.24: CalcRm's own sibling-only
						// pair veto (fDoULSTest) is REMOVED -- it was not a no-op (it caught 0.40% of sibling
						// pi+pi- pairs beyond the 4.1deg pregate); instead this track-level veto no longer
						// uses any angular gate, which covers the same pairs symmetrically.
	double	valULSTestCut;	// SiSplitScore threshold for the above, default 0.05 (sec 15.7, reconfirmed 18.7)
	bool	ONLY_CROSSING0;	// sec 18.21 follow-up: opt-in diagnostic, OFF by default ("Xing0" in RunString
							// turns it on; absent = all crossings kept). NOT "crossing0only" -- that
							// contains "cross" and would silently enable doCrossing. Keeps only rows
							// with `crossing==0` -- the physically-meaningful TRIGGERED collision,
							// exactly one per TF by construction (user's suggestion). Pure per-row cut,
							// no state tracking. Differs from ONLY_FIRST_TF below: rows within a TF are
							// written in ASCENDING crossing order (checked: 4681/4681 TFs in
							// outputCollect_79510_0.root -- NOT "most tracks first", as an earlier note
							// here wrongly said), and 55% of TFs have a negative crossing, so the first
							// row is crossing 0 in only ~25% of TFs -- ONLY_FIRST_TF mostly selects
							// pre-trigger streaming crossings. Only ~57% of TFs have a crossing==0 row at
							// all, so many TFs contribute zero rows here -- expected, not a bug.
	bool	doXTFClean;	// sec 18.22: ON by default since sec 18.25 ("noXTF" turns it off, see corral_utils.cxx). Cross-crossing
							// duplicate-track cleaner. Cause (sec 18.22): the tracking matcher lets ONE
							// silicon seed join SEVERAL TPC seeds, each combination gets its own crossing,
							// so copies of one Si seed land in DIFFERENT crossing rows of the same TF and
							// survive into mixed pairs (the ULS rho2(M) ridge / LS mixed-origin excess).
							// A pre-pass (BuildXTFLosers) groups consecutive rows by (run,evt); any two
							// accepted tracks in DIFFERENT rows sharing >=2 siclukey slots (within one TF a
							// shared key IS the same physical cluster) are duplicates; the loser is the
							// one whose row crossing != its own INTT bucket, else the worse `quality`.
							// Losers join nSplitFlagged_thisevt (track-level removal, like LS/ULS).
	bool	doXTFStrict;	// sec 18.22 variant, ON by default since sec 18.25 (plain "XTFclean" = non-strict): in the "neither" case
							// (both copies have INTT, neither row crossing == its INTT bucket, ~12%) drop
							// BOTH copies instead of keeping the better-quality one (SDCC-claude's view:
							// neither copy's crossing can be trusted there).
	bool	doMixNoAdjTF;	// sec 18.23: ON by default since sec 18.25 ("noXTF" or "mixAdjTF" turns it off). Mixed EVENT pairs from
							// neighboring trigger frames (same run, |devt|==1) are skipped in every pairtype's
							// mixing, with per-zvtx-bin mixed normalization (CalcRm::SetExcludeAdjTF). With
							// doXTFClean off it also skips same-TF event pairs (|devt|<=1), since those are
							// only safe to mix after the same-TF cleaner.
	std::unordered_map<Long64_t,std::vector<int>> xtfLosers;	// chain entry -> track indices to drop
	long	nXTF_pairs, nXTF_byINTT, nXTF_byQuality, nXTF_neither, nXTF_losers;
	bool	ONLY_FIRST_XINGPOS;	// sec 18.21 follow-up 2: opt-in, OFF by default ("XingPos" turns it on).
							// Keeps only the FIRST row with crossing>0 in each (run,evt) TF, skips every
							// other row -- pure STREAMING (non-triggered) events, still <=1 row/TF.
							// Complement of ONLY_CROSSING0 (pure triggered, <=1 row/TF). Rows within a
							// TF are written in ascending crossing order (checked: 4681/4681 TFs,
							// outputCollect_79510_0.root), so "first positive row" == lowest crossing>0.
	bool	ONLY_FIRST_TF;	// sec 18.21: opt-in diagnostic, OFF by default ("onlyfirsttf" turns it on).
							// Keeps only the FIRST row seen for each (run,evt) -- evt IS the trigger
							// frame's own EvtSequence (SDCC-claude, sec 18.20), constant across every row
							// from that TF -- and skips every subsequent row from the same TF entirely,
							// before ANY processing. Makes same-TF mixed-pair contamination structurally
							// impossible (no two rows that ever enter the pipeline can share a TF), unlike
							// sec 18.20's per-pair exclusion (which left single-particle multiplicities
							// inconsistent with the reduced pair count -- a real normalization bug the user
							// caught). Costs ~85% of statistics (~1/6-7 rows survive per TF) -- diagnostic
							// only, never a production default.
	int		NTPCCUT;
	bool	KILLETA0SPIKE;
	//
	TH2D *hRunIndex;
	//
	Long64_t	nentriesfile;
	//
private:
	// builds/returns the input TChain; must be callable before any other member exists
	// (it runs in the constructor's member-initializer-list, ahead of fReader).
	static TChain* BuildInputChain(TTree *tree);
	//
	TChain      *fChain;	// owns the input chain for the lifetime of this object
	TTreeReader fReader;	// drives fChain; every reader value/array below binds to this
	//
public:
	// Tree contents, read via TTreeReader (declaration order here MUST match the
	// member-initializer-list order in corral_utils.cxx, since fChain must be
	// constructed before fReader, and fReader before everything that follows).
	TTreeReaderValue<Int_t>            run;
	TTreeReaderValue<Int_t>            segment;
	TTreeReaderValue<Int_t>            evt;
	TTreeReaderValue<vector<bool> >    trigVec;
	TTreeReaderValue<Int_t>            nvtx;
	TTreeReaderValue<Int_t>            vtxntr;
	TTreeReaderValue<Double_t>         vtxx;
	TTreeReaderValue<Double_t>         vtxy;
	TTreeReaderValue<Double_t>         vtxz;
	TTreeReaderValue<Int_t>            crossing;
	TTreeReaderValue<Double_t>         etotem;
	TTreeReaderValue<Double_t>         etotih;
	TTreeReaderValue<Double_t>         etotoh;
	TTreeReaderValue<Double_t>         etotioh;
	TTreeReaderValue<Double_t>         etotepd;
	TTreeReaderValue<Int_t>            ntr;
	TTreeReaderArray<Char_t>           pid;
	TTreeReaderArray<Short_t>          indv0;
	TTreeReaderArray<Char_t>           primary;
	TTreeReaderArray<Float_t>          quality;
	TTreeReaderArray<Float_t>          chisq;
	TTreeReaderArray<Float_t>          chg;
	TTreeReaderArray<Float_t>          ptot;
	TTreeReaderArray<Float_t>          eta;
	TTreeReaderArray<Float_t>          phi;
	TTreeReaderArray<Float_t>          pt;
	TTreeReaderArray<Float_t>          seedeta;
	TTreeReaderArray<Float_t>          seedphi;
	TTreeReaderArray<Float_t>          seedpt;
	TTreeReaderArray<UChar_t>          ntpc;
	TTreeReaderArray<UChar_t>          nmvtx;
	TTreeReaderArray<UChar_t>          nintt;
	TTreeReaderArray<Float_t>          dcaxy;
	TTreeReaderArray<Float_t>          dcaz;
	TTreeReaderArray<UChar_t>          dedx70n;
	TTreeReaderArray<Float_t>          dedx70s;
	TTreeReaderArray<Float_t>          dedxKFP;	// dedx value KFP actually cuts on (PID set via CDB, not ours) -- for QA vs dedxQA bands
	TTreeReaderArray<ULong64_t>        layermask;	// bit per absolute TrkrDefs layer w/ a cluster on this track (0-2 MVTX, 3-6 INTT, 7-54 TPC) -- see README_SplitTracks.md sec 9/10
	// siclukey/tpcsector/tpcarclen/tpcz below are new-this-round raw per-track/per-Si-or-TPC-layer
	// data (README_SplitTracks.md sec 12a.1/12b.5) -- NOT yet used by any pairwise cut logic here
	// (that's sec 12a.4/12b, deliberately deferred). TTreeReaderArray flattens the tree's fixed
	// second dimension into one contiguous ntr*N array; index element (itrk, slot) as
	// arr[itrk*N + slot] where N=7 for siclukey (Si layer slot, bit 0-6 of layermask) or N=48 for
	// the tpc* triplet (TPC layer slot = absolute layer - 7, range 0-47 for layers 7-54).
	TTreeReaderArray<ULong64_t>        siclukey;	// [ntr][7], sentinel ~0ULL == no cluster in that Si slot -- sec 12a.1/12a.2
	TTreeReaderArray<UChar_t>          tpcsector;	// [ntr][48], sentinel 255 == no TPC cluster on that layer -- sec 12b.5
	TTreeReaderArray<Char_t>           tpcarclen;	// [ntr][48], 2.0 mm/count local arc-length, valid only where tpcsector!=255 -- sec 12b.5
	TTreeReaderArray<Short_t>          tpcz;		// [ntr][48], 0.1 mm/count crossing-corrected global z, valid only where tpcsector!=255 -- sec 12b.5
	TTreeReaderValue<Int_t>            nv0;
	TTreeReaderArray<Int_t>            v0pid;
	TTreeReaderArray<Double_t>         v0x;
	TTreeReaderArray<Double_t>         v0y;
	TTreeReaderArray<Double_t>         v0z;
	TTreeReaderArray<Double_t>         v0px;
	TTreeReaderArray<Double_t>         v0py;
	TTreeReaderArray<Double_t>         v0pz;
	TTreeReaderArray<Double_t>         v0pt;
	TTreeReaderArray<Double_t>         v0ptot;
	TTreeReaderArray<Double_t>         v0eta;
	TTreeReaderArray<Double_t>         v0phi;
	TTreeReaderArray<Double_t>         v0ene;
	TTreeReaderArray<Double_t>         v0mass;
	TTreeReaderArray<Double_t>         v0ctau;
	TTreeReaderArray<Double_t>         v0decaylen;
	TTreeReaderArray<Double_t>         v0chi2ndf;
	TTreeReaderArray<Double_t>         v0dira;
	TTreeReaderArray<Double_t>         v0pvdca;
	TTreeReaderArray<Double_t>         v0decayx;
	TTreeReaderArray<Double_t>         v0decayy;
	TTreeReaderArray<Double_t>         v0decayz;
	TTreeReaderArray<Int_t>            v0indtr1;
	TTreeReaderArray<Int_t>            v0indtr2;
	//
};

#endif
