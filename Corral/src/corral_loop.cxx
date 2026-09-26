
#include "corral_class.h"
#include <algorithm>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TCanvas.h>
#include <TProfile.h>
#include <TSystem.h>

#include "fluct_common.h"
#include "CalcRm.h"
//#include "CalcR3mid.h"
#include "PairTypes.h"

double fDEDXexpected(double *x, double *par) {	// bichsel function...
	double amass	= par[1];
	double momentum	= x[0];
	//double atno		= 1.0;
	//if ( fabs(amass-1.875)<0.1) atno = 2;
	return par[0]*(1.0 + ( amass*amass/momentum/momentum ));
}

//---- README_CQ: C(Q) signposts. Q_inv (= dq = 2k*, CalcRm::PairInfo) at which a two-body decay
//---- R -> 1+2 lands: dq = sqrt((M^2-(m1+m2)^2)(M^2-(m1-m2)^2))/M. Order of the two legs doesn't
//---- matter. Draws a dashed line + label for each one inside the current pad's x range.
double CQResonanceQ(double M, double m1, double m2){
	double a	= (M*M-(m1+m2)*(m1+m2))*(M*M-(m1-m2)*(m1-m2));
	return (a>0.) ? sqrt(a)/M : -1.;
}
void DrawCQSignposts(int ipid1, int ipid2, double ymin, double ymax, double xmax){
	const double mpi = 0.13957, mK0 = 0.497611, mL = 1.115683;		// PDG
	const int PIP = kParticleIDPionPlus, PIM = kParticleIDPionMinus;
	const int KS = kParticleIDKshort, LA = kParticleIDLambda, AL = kParticleIDAntiLambda;
	struct Post { int a, b; double M, m1, m2; const char* lab; };
	const Post posts[]	= {
		{ PIP, PIM, 0.497611, mpi, mpi, "K^{0}_{S}"        },
		{ PIP, PIM, 0.77526,  mpi, mpi, "#rho^{0}"         },
		{ PIP, PIM, 0.990,    mpi, mpi, "f_{0}(980)"       },
		{ KS,  PIP, 0.89167,  mK0, mpi, "K*^{+}"           },
		{ KS,  PIM, 0.89167,  mK0, mpi, "K*^{-}"           },
		{ LA,  PIP, 1.38280,  mL,  mpi, "#Sigma*^{+}"      },
		{ LA,  PIM, 1.3872,   mL,  mpi, "#Sigma*^{-}"      },
		{ LA,  PIM, 1.32171,  mL,  mpi, "#Xi^{-}"          },
		{ AL,  PIM, 1.38280,  mL,  mpi, "#bar{#Sigma*}^{-}"},
		{ AL,  PIP, 1.3872,   mL,  mpi, "#bar{#Sigma*}^{+}"},
		{ AL,  PIP, 1.32171,  mL,  mpi, "#bar{#Xi}^{+}"    },
	};
	int iy	= 0;
	for (const Post &p : posts){
		if (!((p.a==ipid1 && p.b==ipid2) || (p.a==ipid2 && p.b==ipid1))) continue;
		double q	= CQResonanceQ(p.M,p.m1,p.m2);
		if (q<=0. || q>=xmax) continue;
		TLine *ln	= new TLine(q,ymin,q,ymax);
		ln->SetLineStyle(2); ln->SetLineColor(kGray+2); ln->Draw();
		TLatex *tx	= new TLatex(q,ymax-(0.16+0.06*iy)*(ymax-ymin),Form(" %s (%.3f)",p.lab,q));
		tx->SetTextFont(42); tx->SetTextSize(0.030); tx->SetTextColor(kGray+2); tx->Draw();
		++iy;
	}
}

//---- offline "worse-daughter-track PV_DCA" -- see README_treeHandoff.md section 6:
//---- KFParticle_sPHENIX's own online version of this cut is broken (calcMinPV_DCA()
//---- pulls the minimum over every vertex in the whole trigger frame, not just the
//---- crossing-matched one), so it was left un-enforced at production time. it1/it2 --
//---- via v0indtr1[iv0]/v0indtr2[iv0] -- index into THIS ROW's track arrays, so dcaxy/
//---- dcaz here are already correctly crossing-matched to this candidate's own vertex.
double WorseDaughterPVDCA(TTreeReaderArray<Float_t> &dcaxy, TTreeReaderArray<Float_t> &dcaz, int it1, int it2){
	double dca1	= sqrt(dcaxy[it1]*dcaxy[it1] + dcaz[it1]*dcaz[it1]);
	double dca2	= sqrt(dcaxy[it2]*dcaxy[it2] + dcaz[it2]*dcaz[it2]);
	return (dca1>dca2) ? dca1 : dca2;
}

//---- worse-track PV_DCA thresholds (cm), p-cuts working point (tighter, purity-floor --
//---- e-cuts = efficiency-oriented, p-cuts = purity-oriented, per README_settingKFPcuts.md
//---- naming) -- from README_settingKFPcuts.md. The *_sig / *_sig_xy significance variants
//---- recommended alongside these can't be computed here: Collect's outTree has no
//---- per-track DCA-uncertainty branch, only the plain dcaxy/dcaz used above.
//---- LA: Part 4 p-cuts value (0.0091 cm), skeleton unchanged by the Part 5 ana532
//---- re-derivation (LA's re-derived lever was PV_DCA_sig_xy on top of this, not a
//---- replacement for it).
//---- AL: Part 5's ana532-re-derived p-cuts value (0.057 cm), NOT Part 4's ana532_0 value
//---- (0.050 -- close by coincidence at p-cuts, but Part 4's e-cuts value of 0.050 is
//---- wrong for this dataset) -- this run's raw-hit tag (ana532_nocdbtag_v001,
//---- README_treeHandoff.md section 2) matches Part 5's "ana532" sample, where Part 5
//---- found ana532's p-cuts at this threshold actually beat Part 4's original at the
//---- same efficiency floor.
static const double LA_WORSEDCA_CUT	= 0.0091;
static const double AL_WORSEDCA_CUT	= 0.057;

//---------------------------------------------------------------------------
void corral::Loop(){

	if (fChain == 0) return;

	//---- RunIndex (runindex.root, hRunIndex, kRunIndex, hri_* QA hists) is disabled --
	//---- it's a QA-only convenience for plotting vs. a chronological run/segment
	//---- index and isn't needed to process events or fill CFs. The hRunIndex member is
	//---- gone (2026-09-26); the hri_* histogram block below is also disabled.
	//int NRUNSSEEN = 0;
	//cout<<"Reading ~/corral/runindex.root..."<<endl;
	//	fri->cd();
	//	TH1D* htemp		= (TH1D*)fri->Get("hnrs");
	//	if (htemp){
	//		NRUNSSEEN	= htemp->GetBinContent(1);
	//		cout<<"NRUNSSEEN = "<<NRUNSSEEN<<endl;
	//		hRunIndex	= (TH2D*)fri->Get("hrunindex");
	//		hRunIndex	->SetDirectory(0);
	//		delete htemp; htemp=0;
	//	}
	//fri->Close();
	const int NCOUNTERS		= 10;
	int counters[NCOUNTERS]	= {0};
	long nv0fixEta=0,nv0fixPhi=0,nv0fixMass=0,nv0fixCtau=0;	// README_v0etaSpike.md: V0s whose eta/phi/mass/ctau branch was a 0 sentinel, recomputed
	//
	//---- set up dedx curves...
	const int NPART				=  4;
	const double masses[NPART]	= {0.139570392,0.493677,0.9382720894,1.875612945};
	TF1 *fdedxexp[NPART];
	for (int ip=0;ip<NPART;ip++){
		fdedxexp[ip]	= new TF1(Form("fdedxexp%d",ip),fDEDXexpected,0.1,5.1,2);
		fdedxexp[ip]	->SetNpx(1000);
		fdedxexp[ip]	->SetParameter(0, 200.0);		// free! (overall dE/dx scale -- was 320, rescaled for this data)
		fdedxexp[ip]	->SetParameter(1,masses[ip]);	// hypothesis mass
		fdedxexp[ip]	->SetLineWidth(1);
		fdedxexp[ip]	->SetLineColor(1);
	}	
	//
	//---- class definitions...
	double field			= 1.4;	
	bool WeightDensities	= false;				//!!!!!! should be settable!!!!
	const int 	MAX_CALCR_N				=  200;		//!!! set same as in both CalcRx.h variable ntrkmax!!!
	int     	calcr_n_1[NPairTypes]	=  {0}; 
	int     	calcr_n_2[NPairTypes]	=  {0}; 
	double		Pvec_1[NPairTypes][MAX_CALCR_N][3];
	double		Pvec_2[NPairTypes][MAX_CALCR_N][3];
	double		Pvec_1_call[MAX_CALCR_N][3];
	double		Pvec_2_call[MAX_CALCR_N][3];
	double		eff_1[MAX_CALCR_N];
	double		eff_2[MAX_CALCR_N];
	//
	TString ClassStr;
			ClassStr = TString("m");
	if (NOCORRELATIONS) ClassStr = TString("x");
	//
	int 	thisNMIX	=   10;		//50
	double	thisPTNB	=    1;
	int 	thisMAXMULT	=  200;
	int     thisYNB0	=   32;		// reverted (sec 13.8.15) after both crossing-on imaging jobs
										// (92x96 job 40411028, 128x120 job 40411030) confirmed their
										// binary copy -- back to the permanent production default.
									// 2026-09-21 (sec 13.8.9/13.8.10): pushed to 52 to study the raw
									// (dy,dphi)=(0,0) spike's physical footprint (stayed a clean 1x2
									// bin pattern through 22->32->42->52, no spillover into neighbors --
									// confirmed delta-function-like, see 13.8.9), then settled back to
									// 32 (was 22) as the new physics-analysis production default -- a
									// deliberate, permanent choice, not a revert to the original value.
									// The split-track candidate SEARCH window is now a fixed box
									// (PREGATE_DETA/PREGATE_DPHI below) independent of this binning choice,
									// so changing this default no longer changes what counts as a
									// split-track candidate (sec 13.8.10).
	int		thisPHINB0	=   36;		// reverted (sec 13.8.15) -- see thisYNB0 comment above
	int     thisYNB;
	int		thisPHINB;
	if (FinePhiBinning){  thisYNB = 5; thisPHINB = 48; }
	double  thisYL		= -1.1;	
	double  thisYU		= +1.1;
	double  thisPTL		=  0.1;
	double  thisPTU		= 20.1;
	double	thisPTBWMEV	= 2000;		// MeV!		(20 GeV/2 GeV -> 10 PT bins),  
	//							1000,2000,2500,4000,5000,10000,20000
	//							  20,  10,   8,   5,   4,    2,    1  
	TH1::AddDirectory(kFALSE);
		cout<<"MIXING with NMIX="<<thisNMIX<<endl;
		CalcRm*	R[NPairTypes];
	//
	cout<<"doCrossing = "<<(int)doCrossing<<endl;
	cout<<"   NTPCCUT = "<<NTPCCUT<<endl;
	if (doQCut){ cout<<"QCut Enabled..."<<endl;
				 cout<<"Qcut value = "<<valQCut<<endl; }
	if (doSplitRemoval){ cout<<"SplitRemoval pre-pass Enabled..."<<endl;
						  cout<<"pre-pass valSLCut = "<<valSLCut<<", valSiKeyCut = "<<valSiKeyCut<<", valRadialGapCut = "<<valRadialGapCut<<endl; }
	if (doULSTest){ cout<<"ULS split-track removal (sec 18.10: TRACK-level, folded into the same pre-pass as LS) Enabled... valULSTestCut = "<<valULSTestCut<<endl; }
	cout<<"   FinePhi = "<<(int)FinePhiBinning<<endl;
	if (NOCORRELATIONS){
		cout<<"Correlations DISABLED..."<<endl;
	}
	//
	if (!NOCORRELATIONS){
		for (int ipaty=0;ipaty<NPairTypes;ipaty++){
			//
			int ipid1			= PairTypes_Info[ipaty][0];
			int ipid2			= PairTypes_Info[ipaty][1];
			//int kCentralityStyle	= PairTypes_Info[ipaty][2];
			int iSpecies1		= GetSpecies(ipid1);
			int iSpecies2		= GetSpecies(ipid2);
			int iChg1			= ParticleCharge[ipid1];
			int iChg2			= ParticleCharge[ipid2];
			cout<<"Instantiating CalcR2m/c class for ipaty = "<<ipaty
				<<"\t pid: "<<ParticleIDNames[ipid1]<<" "<<ParticleIDNames[ipid2]<<endl;
			thisYNB		= thisYNB0;		// default 
			thisPHINB	= thisPHINB0;	// default 
			if (ipid1>=kParticleIDKshort || ipid2>=kParticleIDKshort){	// This pairtype includes a V0
				thisYNB		=  8;
				thisPHINB	= 12;
			}
			//
				R[ipaty]	= new CalcRm();		//---- Instantiate Mixing Class
				R[ipaty]	->SetNMIX(thisNMIX);
				if (doMixNoAdjTF) R[ipaty]->SetExcludeAdjTF(doXTFClean ? 1 : 2);	// sec 18.23
			//
			R[ipaty]	->SetMAXMULT(thisMAXMULT);	//
				R[ipaty]	->SetField(field);			// needed in Book() for the crossing dirty-side rule
			R[ipaty]	->SetDoCrossing(doCrossing);	// enable pt-ordering in the class, bin-flipping will be done in this code below
			R[ipaty]	->SetFlipDy(    false);		// README_Crossing sec 8 (2026-09-25): pt-ordering flips dphi ONLY; the correction symmetrizes in dphi only
			R[ipaty]	->SetDoBaseline(false);		// enable additive shift of R2 using factorial cumulants
			R[ipaty]	->SetDoQcut(doQCut);		// enable Q cut
			R[ipaty]	->SetQcut( valQCut);		// set Q cut (a lower limit)
			//R[ipaty]	->SetDoMinvCut( doMcut);	// Minv cut only (mixing num & denom, convolution num only)
			//R[ipaty]	->SetDoMinvLLCut(false);	// Minv LL cut only (mixing num & denom, convolution num only)
			R[ipaty]	->SetPid1(ipid1);			//
			R[ipaty]	->SetPid2(ipid2);			//
			R[ipaty]	->SetYNB1(thisYNB);
			R[ipaty]	->SetYL1( thisYL );
			R[ipaty]	->SetYU1( thisYU );
			R[ipaty]	->SetYNB2(thisYNB);
			R[ipaty]	->SetYL2( thisYL );
			R[ipaty]	->SetYU2( thisYU );
			R[ipaty]	->SetPHINB(thisPHINB);
			R[ipaty]	->SetPTL1( thisPTL  );
			R[ipaty]	->SetPTU1( thisPTU  );
			R[ipaty]	->SetPTL2( thisPTL  );
			R[ipaty]	->SetPTU2( thisPTU  );
			R[ipaty]	->SetZVTXNB( 16);
			R[ipaty]	->SetZVTXL( -16);
			R[ipaty]	->SetZVTXU( +16);
			//
			R[ipaty]	->Book();
			//
		}	// end PairTypes loop
	}	// end nocorrelations
	//
	TH1::AddDirectory(kTRUE);
	//
	//---- histograms...
	//	
	TString OutputFileBase;
	if (OutputName!="") OutputFileBase	= OutputName;	// -o (job lists): exactly this name
	else
	if (RunString.Length()){
		OutputFileBase	= TString(Form("corral_%s_%s",ClassStr.Data(),RunString.Data()));
	} else {
		OutputFileBase	= TString(Form("corral_%s"   ,ClassStr.Data()));
	}
	TString RootFileName	= TString("./root/") + OutputFileBase.Data() + ".root";
	TString OutputFileNameO	= TString("./pdf/")  + OutputFileBase.Data() + ".ps(";
	TString OutputFileName	= TString("./pdf/")  + OutputFileBase.Data() + ".ps";
	TString OutputFileNameC	= TString("./pdf/")  + OutputFileBase.Data() + ".ps]";
	TString OutputFileNameP	= TString("./pdf/")  + OutputFileBase.Data() + ".pdf";
	//
	gStyle->SetHistMinimumZero();
	cout<<"Opening root output file "<<RootFileName.Data()<<endl;
	TFile *fout			= new TFile(RootFileName.Data(),"RECREATE");
	//
	//---- run axis for all per-run QA: Run-3 pp (RUNAXIS_*, corral_class.h; was 49700-54200, so every
	//---- Run-3 run landed in overflow and these pages were empty). The Nevt-vs-(run,segment) map
	//---- (hRunSeg) and its JUSTFILLRUNSEG first-pass mode are gone: they only fed the RunIndex
	//---- machinery (disabled, see top of Loop), and condor already says which segments ran.
	TH2D *htrigRun	= new TH2D("htrigRun","Trigger vs RunNumber" ,RUNAXIS_NBIN,RUNAXIS_LO,RUNAXIS_HI,64,-0.5,63.5);
	//
	TH1D *htrig0	= new TH1D("htrig0","htrig IDs raw",64,-0.5,63.5);
	TH1D *heta0		= new TH1D("heta0","heta raw",240,-1.5,1.5);
	TH1D *hphi0		= new TH1D("hphi0","hphi raw",96,-M_PI,M_PI);
	TH1D *hpt0		= new TH1D("hpt0","hpt raw" ,200,0,20.0);
	TH1D *hntpc0	= new TH1D("hntpc0","hntpc raw",70,0.5,70.5);
	TH1D *hnmvtx0	= new TH1D("hnmvtx0","hnmvtx raw",20,-0.5,19.5);
	TH1D *hnintt0	= new TH1D("hnintt0","hnintt raw",10,-0.5,9.5);
	TH2D *hphieta0	= new TH2D("hphieta0","hphieta raw",75,-1.5,1.5,60,-M_PI,M_PI);
	TH2D *hpteta0	= new TH2D("hpteta0","hpteta raw",90,-1.5,1.5,100,0.,20.);
	TH2D *hptphi0	= new TH2D("hptphi0","hptphi raw",192,-M_PI,M_PI,100,0.,20.);
	TH1D *hdcaxy0	= new TH1D("hdcaxy0","hdcaxy raw",400,-2,2);
	TH1D *hdcaz0	= new TH1D("hdcaz0","hdcaz raw",400,-2,2);
	TH2D *hdcaxyz0	= new TH2D("hdcaxyz0","hdcaxyz raw",400,-2,2,400,-2,2);
	TH1D *hvtxx0	= new TH1D("hvtxx0","hvtxx raw",200,-1,1);
	TH1D *hvtxy0	= new TH1D("hvtxy0","hvtxy raw",200,-1,1);
	TH1D *hvtxz0	= new TH1D("hvtxz0","hvtxz raw",200,-20,20);
	TH1D *hntrk0	= new TH1D("hntrk0","hntrk raw",50,-0.5,49.5);
	TH1D *hdedx0	= new TH1D("hdedx0","hdedx raw",500,0,5000.);
	TH1D *hndedx0	= new TH1D("hndedx0","hndedx raw",49,-0.5,48.5);
	TProfile *hdedxphi0		= new TProfile("hdedxphi0"    ,"raw mean dedx vs phi"       ,48,-M_PI,M_PI,0,5000);
	TProfile *hdedxphi_etap0	= new TProfile("hdedxphi_etap0","raw mean dedx vs phi, eta>0",48,-M_PI,M_PI,0,5000);
	TProfile *hdedxphi_etan0	= new TProfile("hdedxphi_etan0","raw mean dedx vs phi, eta<0",48,-M_PI,M_PI,0,5000);
	//
	//---- "kept" (post-AcceptTrack/AcceptEvent) counterparts of the _0 raw histograms above --
	//---- drawn as a green overlay on top of the raw ones in the paint section below (cosmetics below).
	TH1D *htrig		= new TH1D("htrig","htrig IDs",64,-0.5,63.5);
	TH1D *heta		= new TH1D("heta","heta",240,-1.5,1.5);
	TH1D *hphi		= new TH1D("hphi","hphi",96,-M_PI,M_PI);
	TH1D *hpt		= new TH1D("hpt","hpt ",200,0,20.0);
	TH1D *hntpc		= new TH1D("hntpc" ,"hntpc",70,0.5,70.5);
	TH1D *hnmvtx	= new TH1D("hnmvtx","hnmvtx",20,-0.5,19.5);
	TH1D *hnintt	= new TH1D("hnintt","hnintt",10,-0.5,9.5);
	TH2D *hphieta	= new TH2D("hphieta","hphieta",75,-1.5,1.5,60,-M_PI,M_PI);
	TH2D *hpteta	= new TH2D("hpteta" ,"hpteta",90,-1.5,1.5,100,0.,20.);
	TH2D *hptphi	= new TH2D("hptphi" ,"hptphi",192,-M_PI,M_PI,100,0.,20.);
	TH1D *hdcaxy	= new TH1D("hdcaxy","hdcaxy",400,-2,2);
	TH1D *hdcaz		= new TH1D("hdcaz","hdcaz",400,-2,2);
	TH2D *hdcaxyz	= new TH2D("hdcaxyz","hdcaxyz",400,-2,2,400,-2,2);
	//---- range widened 0-20 -> 0-2000 (sec 15): quality has no upper bound enforced by
	//---- AcceptTrack() and a real long tail -- the old 0-20 range silently truncated ~35% of
	//---- entries into the overflow bin (caught while doing a sec-15 loser-leg comparison that
	//---- needed real, untruncated quality means; this production histogram had the exact same
	//---- bug independently discovered in the sec-15 diagnostic histograms).
	TH1D *hquality	= new TH1D("hquality","hquality (kept tracks)",200,0,2000);
	//
	//---- split-track SL monitor QA (see README_SplitTracks.md sec 9/10) -- diagnostic only, does
	//---- NOT remove anything from PID tagging/QA/CF pair-building any more (that's now CalcRm's
	//---- job, sec 10.4). Same shape as the "kept" hists above, filled for tracks this event's SL
	//---- test would flag (only computed when doSplitRemoval is set -- see corral_class.h, sec 13.8.6).
	//---- Retires the old same-Si-seed proxy mechanism (KILLSPLITTRACKS) in favor of the real layermask-based SL.
	TH1D *heta_killed		= new TH1D("heta_killed"   ,"flagged (split-candidate) tracks -- eta"   ,240,-1.5,1.5);
	TH1D *hphi_killed		= new TH1D("hphi_killed"   ,"flagged (split-candidate) tracks -- phi"   ,96,-M_PI,M_PI);
	TH1D *hpt_killed		= new TH1D("hpt_killed"    ,"flagged (split-candidate) tracks -- pt"    ,200,0,20.0);
	TH1D *hntpc_killed		= new TH1D("hntpc_killed"  ,"flagged (split-candidate) tracks -- ntpc"  ,70,0.5,70.5);
	TH2D *hphieta_killed	= new TH2D("hphieta_killed","flagged (split-candidate) tracks -- eta vs phi",75,-1.5,1.5,60,-M_PI,M_PI);
	TH1D *hdcaxy_killed	= new TH1D("hdcaxy_killed" ,"flagged (split-candidate) tracks -- dcaxy" ,400,-2,2);
	TH1D *hdcaz_killed		= new TH1D("hdcaz_killed"  ,"flagged (split-candidate) tracks -- dcaz"  ,400,-2,2);
	TH1D *hquality_killed	= new TH1D("hquality_killed","flagged (split-candidate) tracks -- quality",200,0,2000);	// range widened, see hquality above
	//---- new-mechanism-specific plots (sec 10): filled for every same-charge, kinematically-close
	//---- candidate pair every event, regardless of doSplitRemoval, so the threshold scan (sec 10.9
	//---- step 3) always has something to look at even before a cut value is chosen.
	TH1D *hSL_cand			= new TH1D("hSL_cand","split-candidate pairs -- Splitting Level (STAR sec 2.4.1)",120,-1.1,1.1);
	TH2D *hntpc_ij_cand	= new TH2D("hntpc_ij_cand","split-candidate pairs -- ntpc[i] vs ntpc[j] (sec 9.6)",49,-0.5,48.5,49,-0.5,48.5);
	//---- SiKeyFrac (sec 12a.4/13.8) monitor, same convention as hSL_cand above
	TH1D *hSiKeyFrac_cand	= new TH1D("hSiKeyFrac_cand","split-candidate pairs -- SiKeyFrac (sec 12a.4)",44,-1.05,1.05);
	//---- MVTX/INTT split diagnostics (sec 13.9) -- also feed the production discriminator below
	//---- (SiSplitScore, fluct_common.h) as of this update. See fluct_common.h::SiKeyFracSplit for
	//---- why: INTT strips are far coarser than MVTX pixels, so an accidental match on an INTT slot
	//---- is intrinsically more likely, yet the combined SiKeyFrac above weighs both sub-detectors'
	//---- slots identically.
	TH1D *hMVTXKeyFrac_cand		= new TH1D("hMVTXKeyFrac_cand","split-candidate pairs -- MVTX-only SiKeyFrac (sec 13.9)",42,-1.05,1.05);
	TH1D *hINTTKeyFrac_cand		= new TH1D("hINTTKeyFrac_cand","split-candidate pairs -- INTT-only SiKeyFrac (sec 13.9)",42,-1.05,1.05);
	TH2D *hMVTXvsINTTavail_cand	= new TH2D("hMVTXvsINTTavail_cand","split-candidate pairs -- available slots, MVTX vs INTT (sec 13.9)",4,-0.5,3.5,5,-0.5,4.5);
	TH2D *hMVTXvsINTTKeyFrac_cand	= new TH2D("hMVTXvsINTTKeyFrac_cand","split-candidate pairs -- MVTX-only vs INTT-only SiKeyFrac (sec 13.9)",42,-1.05,1.05,42,-1.05,1.05);
	//---- production discriminator distribution (sec 13.9) -- what valSiKeyCut is actually gated
	//---- against below, as of this update; hSiKeyFrac_cand above is now the LEGACY combined metric,
	//---- kept purely as a retained comparison point in case this change needs backing out.
	TH1D *hSiSplitScore_cand	= new TH1D("hSiSplitScore_cand","split-candidate pairs -- SiSplitScore, MVTX-primary/INTT-secondary (sec 13.9)",44,-1.05,1.05);
	//---- RadialGap (sec 17.11) -- population-wide, every candidate, not just the two restricted
	//---- ridge-box buckets checked so far. Lets us see its full distribution and how it relates to
	//---- SL/SKF across the whole candidate population, not just the two extreme corners.
	TH1D *hRadialGap_cand		= new TH1D("hRadialGap_cand","split-candidate pairs -- RadialGap (sec 17.11)",48,-0.02,1.02);
	TH2D *hRadialGap_vs_SL_cand	= new TH2D("hRadialGap_vs_SL_cand","split-candidate pairs -- SL vs RadialGap (sec 17.11)",120,-1.1,1.1,48,-0.02,1.02);
	TH2D *hRadialGap_vs_SKF_cand	= new TH2D("hRadialGap_vs_SKF_cand","split-candidate pairs -- SiSplitScore vs RadialGap (sec 17.11)",44,-1.05,1.05,48,-0.02,1.02);
	//---- sec 17: does INTT actually help discriminate WITHIN the genuinely-partial-MVTX band
	//---- (mvtxFrac=1/3 or 2/3), or does it just add scatter? Two independent, purely diagnostic
	//---- checks (neither touches doSplitRemoval/any cut): (a) fine (deta,dphi) position maps,
	//---- one per partial-MVTX x INTT-agreement sub-population -- true split-track duplicates
	//---- should sit essentially exactly at (0,0) like the mvtxFrac==1 reference population, so a
	//---- sub-population that's spread across the pregate like the mvtxFrac==0 background instead
	//---- looks like INTT noise, not corroboration; (b) INTT-agreement rate in the sideband
	//---- (background-only, no duplicates expected, hMVTXvsINTTKeyFrac_far_LS below) vs this
	//---- in-box candidate population -- if INTT reads "full match" just as often on unrelated
	//---- sideband pairs, its corroboration in the partial band isn't informative. Ranges below
	//---- are literal copies of PREGATE_DETA/PREGATE_DPHI (declared later, inside the loop) --
	//---- keep in sync if the pregate ever changes.
	TH2D *hPos_mvtx13_inttAgree	= new TH2D("hPos_mvtx13_inttAgree","cand, mvtxFrac~1/3 & INTT agrees (>=0.75);d#eta;d#phi(deg)",22,-0.022,0.022,22,-4.1,4.1);
	TH2D *hPos_mvtx13_inttDisagree	= new TH2D("hPos_mvtx13_inttDisagree","cand, mvtxFrac~1/3 & INTT disagrees (<0.75);d#eta;d#phi(deg)",22,-0.022,0.022,22,-4.1,4.1);
	TH2D *hPos_mvtx23_inttAgree	= new TH2D("hPos_mvtx23_inttAgree","cand, mvtxFrac~2/3 & INTT agrees (>=0.75);d#eta;d#phi(deg)",22,-0.022,0.022,22,-4.1,4.1);
	TH2D *hPos_mvtx23_inttDisagree	= new TH2D("hPos_mvtx23_inttDisagree","cand, mvtxFrac~2/3 & INTT disagrees (<0.75);d#eta;d#phi(deg)",22,-0.022,0.022,22,-4.1,4.1);
	TH2D *hPos_mvtx0		= new TH2D("hPos_mvtx0","cand, mvtxFrac==0 (background reference);d#eta;d#phi(deg)",22,-0.022,0.022,22,-4.1,4.1);
	TH2D *hPos_mvtx1		= new TH2D("hPos_mvtx1","cand, mvtxFrac==1 (unambiguous-match reference);d#eta;d#phi(deg)",22,-0.022,0.022,22,-4.1,4.1);
	//---- sec 17.10: "hot line of death" box test -- characterizes the residual (dy,dphi) excess
	//---- that survives the finalized SL<0.18/SKF>=0.10 cut (sec 17.6), reusing sec 15's ULS
	//---- "is this a real split track" diagnostics (pt/ptot symmetry, ntpc/quality asymmetry
	//---- between legs). Box measured directly off hzoom_S/hzoom_M at the finalized working point
	//---- (root/corral_m_mvled_removecut18skf10.root): |dy|<0.010, |dphi|<0.2deg. Two
	//---- populations per variable: "survive" = box pairs the CURRENT cut does NOT flag (i.e. what's
	//---- still visible in the zoom map after cuts) vs "flagged" = box pairs the cut WOULD remove
	//---- (already being removed in production) -- a built-in contrast/sanity check.
	TH1D *hptreldiff_ridge_survive		= new TH1D("hptreldiff_ridge_survive","ridge box, NOT flagged by SL/SKF -- relative |pt[i]-pt[j]| (sec 17.10)",100,0,2.1);
	TH1D *hptreldiff_ridge_flagged		= new TH1D("hptreldiff_ridge_flagged","ridge box, flagged by SL/SKF -- relative |pt[i]-pt[j]| (sec 17.10)",100,0,2.1);
	TH1D *hptotreldiff_ridge_survive	= new TH1D("hptotreldiff_ridge_survive","ridge box, NOT flagged by SL/SKF -- relative |ptot[i]-ptot[j]| (sec 17.10)",100,0,2.1);
	TH1D *hptotreldiff_ridge_flagged	= new TH1D("hptotreldiff_ridge_flagged","ridge box, flagged by SL/SKF -- relative |ptot[i]-ptot[j]| (sec 17.10)",100,0,2.1);
	TH1D *hqualitydiff_ridge_survive	= new TH1D("hqualitydiff_ridge_survive","ridge box, NOT flagged by SL/SKF -- |quality[i]-quality[j]| (sec 17.10)",200,0,2000);
	TH1D *hqualitydiff_ridge_flagged	= new TH1D("hqualitydiff_ridge_flagged","ridge box, flagged by SL/SKF -- |quality[i]-quality[j]| (sec 17.10)",200,0,2000);
	TH2D *hntpc_ij_ridge_survive		= new TH2D("hntpc_ij_ridge_survive","ridge box, NOT flagged by SL/SKF -- ntpc[i] vs ntpc[j] (sec 17.10)",49,-0.5,48.5,49,-0.5,48.5);
	TH2D *hntpc_ij_ridge_flagged		= new TH2D("hntpc_ij_ridge_flagged","ridge box, flagged by SL/SKF -- ntpc[i] vs ntpc[j] (sec 17.10)",49,-0.5,48.5,49,-0.5,48.5);
	//---- test #3 (sec 17.10 follow-up, pt/ptot confirmed redundant -- see sec 15.4, same box
	//---- selection pins eta/y so pz collapses onto pt): DCA correlation between legs. A real
	//---- single-particle split shares essentially the same trajectory (same DCA); two
	//---- coincidentally-nearby independent tracks don't -- nothing about matching (y,phi,pt)
	//---- already implies matching DCA, so this is a genuinely independent axis.
	TH1D *hdcaxydiff_ridge_survive	= new TH1D("hdcaxydiff_ridge_survive","ridge box, NOT flagged by SL/SKF -- |dcaxy[i]-dcaxy[j]| (sec 17.10)",200,0,1.0);
	TH1D *hdcaxydiff_ridge_flagged	= new TH1D("hdcaxydiff_ridge_flagged","ridge box, flagged by SL/SKF -- |dcaxy[i]-dcaxy[j]| (sec 17.10)",200,0,1.0);
	TH1D *hdcazdiff_ridge_survive	= new TH1D("hdcazdiff_ridge_survive","ridge box, NOT flagged by SL/SKF -- |dcaz[i]-dcaz[j]| (sec 17.10)",200,0,2.0);
	TH1D *hdcazdiff_ridge_flagged	= new TH1D("hdcazdiff_ridge_flagged","ridge box, flagged by SL/SKF -- |dcaz[i]-dcaz[j]| (sec 17.10)",200,0,2.0);
	//---- user question (sec 17.19): is dE/dx also similar between legs of a "duplicate" pair, the
	//---- way ntpc/quality already show measurable asymmetry (not identical copies)?
	TH1D *hdedxdiff_ridge_survive	= new TH1D("hdedxdiff_ridge_survive","ridge box, NOT flagged by SL/SKF -- |dedx70s[i]-dedx70s[j]| (sec 17.19)",200,0,1000);
	TH1D *hdedxdiff_ridge_flagged	= new TH1D("hdedxdiff_ridge_flagged","ridge box, flagged by SL/SKF -- |dedx70s[i]-dedx70s[j]| (sec 17.19)",200,0,1000);
	//---- user's sharper follow-up (sec 17.19): track-level <dedx> is fluctuation-dominated, not a
	//---- clean test. Real test: per-SHARED-TPC-layer cluster position agreement (sec 12b.5/12b.6's
	//---- d_mm formula) -- if "duplicate" really means the same physical cluster reused by both
	//---- legs, d_mm should sit at ~0 (detector resolution) at every shared layer, not just close.
	TH1D *hDmm_ridge_survive		= new TH1D("hDmm_ridge_survive","ridge box, NOT flagged -- d_mm at shared TPC layers, matching sector (sec 17.19)",150,0,300);
	TH1D *hDmm_ridge_flagged		= new TH1D("hDmm_ridge_flagged","ridge box, flagged (duplicate) -- d_mm at shared TPC layers, matching sector (sec 17.19)",150,0,300);
	TH1D *hSectorMatchFrac_ridge_survive	= new TH1D("hSectorMatchFrac_ridge_survive","ridge box, NOT flagged -- frac of shared TPC layers w/ matching sector (sec 17.19)",22,0,1.1);
	TH1D *hSectorMatchFrac_ridge_flagged	= new TH1D("hSectorMatchFrac_ridge_flagged","ridge box, flagged (duplicate) -- frac of shared TPC layers w/ matching sector (sec 17.19)",22,0,1.1);
	//---- user question: is the "survive" DCA tightness circular -- do survivors still share a Si
	//---- seed (high SKF, just failing SL) or not (low SKF, genuinely no Si match)? "flagged" pairs
	//---- share both by definition, so their DCA tightness is close to tautological; this settles
	//---- which case "survive" actually is before reading anything into its DCA result.
	TH2D *hSLvsSKF_ridge_survive	= new TH2D("hSLvsSKF_ridge_survive","ridge box, NOT flagged by SL/SKF -- SL vs SiSplitScore (sec 17.10)",44,-1.05,1.05,44,-1.05,1.05);
	TH2D *hSLvsSKF_ridge_flagged	= new TH2D("hSLvsSKF_ridge_flagged","ridge box, flagged by SL/SKF -- SL vs SiSplitScore (sec 17.10)",44,-1.05,1.05,44,-1.05,1.05);
	//---- sec 17.11: radial structure test. SL (pad-row OVERLAP only) is structurally blind to a
	//---- clean STAR-fig-1b radial split (one real track, TPC clusters partitioned into disjoint
	//---- inner/outer halves) -- such a split forces Nboth=0, Ndiff=Ntot, SL=+1 exactly, identical
	//---- to two genuinely independent tracks. The layermask's TPC bit INDEX (bits 7-54, sec 9.1)
	//---- corresponds directly to physical pad-row/radius, so this checks what SL discards: per
	//---- pair, sort legs by mean occupied TPC-bit index (proxy for mean radius) into
	//---- "lower"/"upper", fill occupancy-vs-layer for each. A clean radial split (fig 1b) shows up
	//---- as lower/upper occupying disjoint, non-interleaved bit ranges; the KNOWN-duplicate
	//---- "flagged" population (SKF>=0.10 && SL<0.18, shares pad-rows by definition) is the negative
	//---- control -- it should show heavy lower/upper overlap, not a clean split.
	TH1D *hTPClayer_lower_complementary	= new TH1D("hTPClayer_lower_complementary","ridge box, SKF>=0.95 & SL>0.9 -- lower-mean-index leg, TPC bit occupancy (sec 17.11)",48,-0.5,47.5);
	TH1D *hTPClayer_upper_complementary	= new TH1D("hTPClayer_upper_complementary","ridge box, SKF>=0.95 & SL>0.9 -- upper-mean-index leg, TPC bit occupancy (sec 17.11)",48,-0.5,47.5);
	TH2D *hTPCmeanIdx_complementary		= new TH2D("hTPCmeanIdx_complementary","ridge box, SKF>=0.95 & SL>0.9 -- mean TPC bit index, lower leg vs upper leg (sec 17.11)",48,-0.5,47.5,48,-0.5,47.5);
	TH1D *hTPClayer_lower_flagged		= new TH1D("hTPClayer_lower_flagged","ridge box, flagged by SL/SKF -- lower-mean-index leg, TPC bit occupancy (sec 17.11)",48,-0.5,47.5);
	TH1D *hTPClayer_upper_flagged		= new TH1D("hTPClayer_upper_flagged","ridge box, flagged by SL/SKF -- upper-mean-index leg, TPC bit occupancy (sec 17.11)",48,-0.5,47.5);
	TH2D *hTPCmeanIdx_flagged		= new TH2D("hTPCmeanIdx_flagged","ridge box, flagged by SL/SKF -- mean TPC bit index, lower leg vs upper leg (sec 17.11)",48,-0.5,47.5,48,-0.5,47.5);
	//---- ULS (opposite-charge pi+pi-) diagnostic (README sec 15/11.9 item 3) -- re-checking the
	//---- still-open opposite-charge (0,0) spike against the SHARP siclukey-based discriminators
	//---- now available, since the only prior check (sec 5) used a much blunter nmvtx/nintt/dcaxy
	//---- proxy and found no elevation -- that same blunt proxy badly underestimated the
	//---- same-charge signal too (13.9% false-positive rate vs. the real 0.003% once cluster
	//---- identity was available), so "no effect" from the old proxy is not conclusive. Purely
	//---- diagnostic -- does NOT feed doSplitRemoval/any cut, opposite-charge pairs are never
	//---- flagged or removed by anything in this codebase (by design, sec 5).
	//---- "_cand"/"LS" = inside the same close box used for the production same-charge cut;
	//---- "_far"/sideband = clearly-unrelated control region (0.05<=dr<0.5, same definition as
	//---- sec 5's table), for BOTH charge combinations, so a close-vs-sideband elevation ratio
	//---- can be read the same way sec 5's original table did, just with a sharper metric.
	TH1D *hSL_cand_ULS			= new TH1D("hSL_cand_ULS","ULS (opp-charge) close-box pairs -- Splitting Level (sec 15)",120,-1.1,1.1);
	TH1D *hSiKeyFrac_cand_ULS	= new TH1D("hSiKeyFrac_cand_ULS","ULS (opp-charge) close-box pairs -- flat SiKeyFrac (sec 15)",44,-1.05,1.05);
	TH1D *hSiSplitScore_cand_ULS	= new TH1D("hSiSplitScore_cand_ULS","ULS (opp-charge) close-box pairs -- MVTX-led SiSplitScore (sec 15)",44,-1.05,1.05);
	//---- sec 15 follow-up: SL and fit-quality asymmetry WITHIN the ULS close-box population,
	//---- split by MVTX-led SiSplitScore (>=0.60 "high" -- presumed real duplicates, mirrors the
	//---- production threshold's own value threshold -- vs <0.60 "low" -- presumed background) --
	//---- checking whether the high-SKF subpopulation shows the SAME winner/loser fit-quality
	//---- asymmetry the confirmed same-charge duplicates show (sec 9.5), which would support the
	//---- "one leg is a poorly-reconstructed fragment, more susceptible to curvature-sign
	//---- confusion" mechanism directly, not just cluster-sharing alone.
	TH2D *hSLvsSKF_cand_ULS		= new TH2D("hSLvsSKF_cand_ULS","ULS close-box pairs -- SL vs MVTX-led SiSplitScore (sec 15)",60,-1.1,1.1,44,-1.05,1.05);
	//---- sec 18.18: SIBLING d_mm (sec 17.19's formula), peak circle vs exact mirror circle vs true
	//---- origin -- cheap, reuses raw per-event tpcarclen/tpcz/tpcsector directly, no new buffer.
	TH1D *hDmm_ULSridge_sibling	= new TH1D("hDmm_ULSridge_sibling","ULS SIBLING, ridge circle;d_mm at shared TPC layer, matching sector (mm);pairs",150,0,300);
	TH1D *hDmm_ULSmirror_sibling	= new TH1D("hDmm_ULSmirror_sibling","ULS SIBLING, mirror circle;d_mm at shared TPC layer, matching sector (mm);pairs",150,0,300);
	TH1D *hDmm_ULSorigin_sibling	= new TH1D("hDmm_ULSorigin_sibling","ULS SIBLING, true-origin circle;d_mm at shared TPC layer, matching sector (mm);pairs",150,0,300);
	TH1D *hntpc_loser_ULS_hiSKF	= new TH1D("hntpc_loser_ULS_hiSKF","ULS close-box, SKF>=0.60 -- ntpc of would-be loser leg (sec 15)",49,-0.5,48.5);
	TH1D *hntpc_winner_ULS_hiSKF	= new TH1D("hntpc_winner_ULS_hiSKF","ULS close-box, SKF>=0.60 -- ntpc of would-be winner leg (sec 15)",49,-0.5,48.5);
	TH1D *hntpc_loser_ULS_loSKF	= new TH1D("hntpc_loser_ULS_loSKF","ULS close-box, SKF<0.60 -- ntpc of would-be loser leg (sec 15)",49,-0.5,48.5);
	TH1D *hntpc_winner_ULS_loSKF	= new TH1D("hntpc_winner_ULS_loSKF","ULS close-box, SKF<0.60 -- ntpc of would-be winner leg (sec 15)",49,-0.5,48.5);
	TH1D *hquality_loser_ULS_hiSKF	= new TH1D("hquality_loser_ULS_hiSKF","ULS close-box, SKF>=0.60 -- quality of would-be loser leg (sec 15)",200,0,2000);
	TH1D *hquality_winner_ULS_hiSKF	= new TH1D("hquality_winner_ULS_hiSKF","ULS close-box, SKF>=0.60 -- quality of would-be winner leg (sec 15)",200,0,2000);
	TH1D *hquality_loser_ULS_loSKF	= new TH1D("hquality_loser_ULS_loSKF","ULS close-box, SKF<0.60 -- quality of would-be loser leg (sec 15)",200,0,2000);
	TH1D *hquality_winner_ULS_loSKF	= new TH1D("hquality_winner_ULS_loSKF","ULS close-box, SKF<0.60 -- quality of would-be winner leg (sec 15)",200,0,2000);
	//---- proper (non-tautological) within-pair asymmetry, symmetric under i<->j swap, unlike the
	//---- loser/winner split above (which is a min/max split by construction and, as measured,
	//---- carries no discriminating signal -- see README sec 15). |quality[i]-quality[j]| directly
	//---- tests whether real-duplicate-like ULS pairs (high SKF) show a BIGGER internal quality
	//---- gap than background-like ones (low SKF), independent of which leg happens to "win."
	TH1D *hqualitydiff_ULS_hiSKF	= new TH1D("hqualitydiff_ULS_hiSKF","ULS close-box, SKF>=0.60 -- |quality[i]-quality[j]| (sec 15)",200,0,2000);
	TH1D *hqualitydiff_ULS_loSKF	= new TH1D("hqualitydiff_ULS_loSKF","ULS close-box, SKF<0.60 -- |quality[i]-quality[j]| (sec 15)",200,0,2000);
	//---- pt-similarity mechanism test (sec 15, user's proposal): if the high-SKF ULS population
	//---- really is one physical trajectory with a curvature-sign error, both legs should
	//---- reconstruct to roughly the SAME |pt| (a sign flip on a shared fit barely changes the
	//---- fitted radius) -- a much tighter loser/winner pt correlation than the low-SKF background
	//---- population would show. pt is bounded [0.1,20.1] by AcceptTrack(), unlike quality (no
	//---- learned that lesson the hard way just now with the quality histograms' overflow bug) --
	//---- range chosen accordingly, no overflow risk expected.
	//---- Relative difference (symmetric under i<->j, non-tautological, same spirit as
	//---- hqualitydiff above): |pt[i]-pt[j]| / (0.5*(pt[i]+pt[j])), range [0,2).
	TH1D *hptreldiff_ULS_hiSKF	= new TH1D("hptreldiff_ULS_hiSKF","ULS close-box, SKF>=0.60 -- relative |pt[i]-pt[j]| (sec 15)",100,0,2.1);
	TH1D *hptreldiff_ULS_loSKF	= new TH1D("hptreldiff_ULS_loSKF","ULS close-box, SKF<0.60 -- relative |pt[i]-pt[j]| (sec 15)",100,0,2.1);
	//---- ptot (full 3-momentum magnitude, already a branch) cross-check (user's suggestion) --
	//---- with eta already tightly fixed by the box (|deta|<0.02), pt and pz=pt*sinh(eta) are
	//---- largely redundant for THIS population, so this is mainly a robustness cross-check
	//---- rather than genuinely independent information -- still cheap and worth confirming
	//---- directly rather than assuming. Relative-difference form again, so no need to guess an
	//---- absolute range for ptot itself (learned that lesson from the quality histograms).
	TH1D *hptotreldiff_ULS_hiSKF	= new TH1D("hptotreldiff_ULS_hiSKF","ULS close-box, SKF>=0.60 -- relative |ptot[i]-ptot[j]| (sec 15)",100,0,2.1);
	TH1D *hptotreldiff_ULS_loSKF	= new TH1D("hptotreldiff_ULS_loSKF","ULS close-box, SKF<0.60 -- relative |ptot[i]-ptot[j]| (sec 15)",100,0,2.1);
	//---- Q_inv check (sec 15, user's proposal): the real production C(Q) plot (root/corral_m.root,
	//---- no RunString, sec 14.4) shows a genuine low-Q Coulomb enhancement below ~10 MeV for ULS
	//---- pion pairs (expected physics) PLUS a distinct second enhancement around Q~25 MeV. Checked
	//---- directly: the rho(770) (M~775 MeV) maps to Q_inv=sqrt(M^2-4*mpi^2)~723 MeV -- nowhere near
	//---- 25 MeV, and outside the hCQ histograms' own 0-300 MeV range entirely, so NOT the rho. Given
	//---- high-SKF ULS candidates were just measured to have large pt/ptot mismatch between legs
	//---- (~63% relative, sec 15) despite being angularly adjacent (that's the box definition), their
	//---- Q_inv (which depends on the FULL 4-momentum difference, not just angle) should land away
	//---- from 0 at some characteristic value, not smear across the whole range like independent
	//---- background pairs -- checking directly whether that characteristic value lands near the
	//---- observed ~25 MeV bump. Same identical-mass Q_inv formula as CalcRm::PairInfo
	//---- (Qinv=sqrt(dPx^2+dPy^2+dPz^2-dPE^2)), built directly from (pt,eta,phi) since eta/pt alone
	//---- geometrically fix the 3-momentum regardless of assumed mass -- no need to go through
	//---- CalcRm's own rapidity-based internal parameterization.
	TH1D *hQinv_cand_ULS_hiSKF	= new TH1D("hQinv_cand_ULS_hiSKF","ULS close-box, SKF>=0.60 -- Q_inv (sec 15)",300,0,0.3);
	TH1D *hQinv_cand_ULS_loSKF	= new TH1D("hQinv_cand_ULS_loSKF","ULS close-box, SKF<0.60 -- Q_inv (sec 15)",300,0,0.3);
	//---- sec 15 follow-up: the box-restricted Qinv check above came back null (neither hiSKF nor
	//---- loSKF showed a localized peak near the real hCQ_0's observed Q~25MeV bump) -- the tight
	//---- (dy,dphi) box likely biases toward the SMALLEST-divergence (hence smallest Q_inv)
	//---- duplicates, systematically excluding whatever produces the mid-Q feature. This time: NO
	//---- angular restriction at all -- every opposite-charge pion pair in the event, correlating
	//---- SiSplitScore directly against Q_inv, same binning as hCQ_0 for direct shape comparison.
	TH1D *hQinv_allULS_hiSKF	= new TH1D("hQinv_allULS_hiSKF","ALL ULS pairs (no box), SKF>=0.60 -- Q_inv (sec 15)",300,0,0.3);
	TH1D *hQinv_allULS_loSKF	= new TH1D("hQinv_allULS_loSKF","ALL ULS pairs (no box), SKF<0.60 -- Q_inv (sec 15)",300,0,0.3);
	//---- full 2D picture, loser/winner convention (sec 9.5 rule, same as elsewhere in this file).
	TH2D *hpt_loser_vs_winner_ULS_hiSKF	= new TH2D("hpt_loser_vs_winner_ULS_hiSKF","ULS close-box, SKF>=0.60 -- pt[loser] vs pt[winner] (sec 15)",100,0,20,100,0,20);
	TH2D *hpt_loser_vs_winner_ULS_loSKF	= new TH2D("hpt_loser_vs_winner_ULS_loSKF","ULS close-box, SKF<0.60 -- pt[loser] vs pt[winner] (sec 15)",100,0,20,100,0,20);
	TH1D *hSL_far_LS			= new TH1D("hSL_far_LS","LS (same-charge) sideband (0.05<=dr<0.5) pairs -- Splitting Level (sec 15)",120,-1.1,1.1);
	TH1D *hSiKeyFrac_far_LS		= new TH1D("hSiKeyFrac_far_LS","LS (same-charge) sideband pairs -- flat SiKeyFrac (sec 15)",44,-1.05,1.05);
	TH1D *hSiSplitScore_far_LS	= new TH1D("hSiSplitScore_far_LS","LS (same-charge) sideband pairs -- MVTX-led SiSplitScore (sec 15)",44,-1.05,1.05);
	//---- RadialGap sideband reference (sec 17.11) -- background-only population, to check RadialGap
	//---- doesn't just reflect generic uncorrelated tracks (it shouldn't: real independent
	//---- full-length tracks typically span, and so overlap on, most of the 48 TPC layers).
	TH1D *hRadialGap_far_LS	= new TH1D("hRadialGap_far_LS","LS sideband pairs -- RadialGap (sec 17.11)",48,-0.02,1.02);
	//---- sec 17: MVTX/INTT split for the LS sideband, mirroring hMVTXKeyFrac_cand/hINTTKeyFrac_cand/
	//---- hMVTXvsINTTKeyFrac_cand above -- gives the background-only INTT-agreement rate to compare
	//---- against the in-box candidate population.
	TH1D *hMVTXKeyFrac_far_LS		= new TH1D("hMVTXKeyFrac_far_LS","LS sideband pairs -- MVTX-only SiKeyFrac (sec 17)",42,-1.05,1.05);
	TH1D *hINTTKeyFrac_far_LS		= new TH1D("hINTTKeyFrac_far_LS","LS sideband pairs -- INTT-only SiKeyFrac (sec 17)",42,-1.05,1.05);
	TH2D *hMVTXvsINTTKeyFrac_far_LS	= new TH2D("hMVTXvsINTTKeyFrac_far_LS","LS sideband pairs -- MVTX-only vs INTT-only SiKeyFrac (sec 17)",42,-1.05,1.05,42,-1.05,1.05);
	TH1D *hSL_far_ULS			= new TH1D("hSL_far_ULS","ULS (opp-charge) sideband pairs -- Splitting Level (sec 15)",120,-1.1,1.1);
	TH1D *hSiKeyFrac_far_ULS	= new TH1D("hSiKeyFrac_far_ULS","ULS (opp-charge) sideband pairs -- flat SiKeyFrac (sec 15)",44,-1.05,1.05);
	TH1D *hSiSplitScore_far_ULS	= new TH1D("hSiSplitScore_far_ULS","ULS (opp-charge) sideband pairs -- MVTX-led SiSplitScore (sec 15)",44,-1.05,1.05);
	long nSplitFlagged_total	= 0;
	long nSplitFlagged_evts	= 0;
	long nFlagged_duplicate	= 0;	// sec 17.14: per-path breakdown -- path 1 (SL<cut, live since 17.6)
	long nFlagged_complementary	= 0;	// path 2 (RadialGap>=cut, new sec 17.14)
	long nFlagged_ULS			= 0;	// sec 18.10: opposite-charge SiSplitScore path -- now TRACK-level
									// removal (folded into nSplitFlagged_thisevt), not just a sibling-only
									// pair veto -- fixes the mixed-side contamination sec 18's own work found
									// (a phantom track surviving into mix_part1/2 for every other pairtype).
	TH1D *hvtxx		= new TH1D("hvtxx","hvtxx",200,-1,1);
	TH1D *hvtxy		= new TH1D("hvtxy","hvtxy",200,-1,1);
	TH1D *hvtxz		= new TH1D("hvtxz","hvtxz",200,-20,20);
	TH1D *hntrk		= new TH1D("hntrk","hntrk",50,-0.5,49.5);
	TH1D *hdedx		= new TH1D("hdedx","hdedx",500,0,5000.);
	TH1D *hndedx	= new TH1D("hndedx","hndedx",49,-0.5,48.5);
	TProfile *hdedxphi		= new TProfile("hdedxphi"     ,"mean dedx vs phi"       ,48,-M_PI,M_PI,0,5000);
	TProfile *hdedxphi_etap	= new TProfile("hdedxphi_etap","mean dedx vs phi, eta>0",48,-M_PI,M_PI,0,5000);
	TProfile *hdedxphi_etan	= new TProfile("hdedxphi_etan","mean dedx vs phi, eta<0",48,-M_PI,M_PI,0,5000);
	TH1D *heta_mvt		= new TH1D("heta_mvt"   ,"heta_mvt"   ,240,-1.5,1.5);
	TH1D *heta_int		= new TH1D("heta_int"   ,"heta_int"   ,240,-1.5,1.5);
	TH1D *heta_mvtint	= new TH1D("heta_mvtint","heta_mvtint",240,-1.5,1.5);
	TH2D *hetazvtx		= new TH2D("hetazvtx"     ,"hetazvtx"              ,40,-20.,20.,320,-2.,2.);
	TH2D *hetazvtx_pta	= new TH2D("hetazvtx_pta" ,"hetazvtx, pt>0.5 GeV/c",40,-20.,20.,320,-2.,2.);
	TH2D *hetazvtx_ptb	= new TH2D("hetazvtx_ptb" ,"hetazvtx, pt>1.0 GeV/c",40,-20.,20.,320,-2.,2.);
	TH2D     *hntpc_phieta	= new TH2D(    "hntpc_phieta",    "hntpc_phieta",75,-1.5,1.5,60,-M_PI,M_PI);
	TH2D *habsdcaxy_phieta	= new TH2D("habsdcaxy_phieta","habsdcaxy_phieta",75,-1.5,1.5,60,-M_PI,M_PI);
	TH2D  *habsdcaz_phieta	= new TH2D( "habsdcaz_phieta", "habsdcaz_phieta",75,-1.5,1.5,60,-M_PI,M_PI);
	TH2D     *hdedx_phieta	= new TH2D(    "hdedx_phieta",    "hdedx_phieta",75,-1.5,1.5,60,-M_PI,M_PI);
	//
	const char* strch[2]	= {"pos","neg"};
	const char* strv0[3]	= {"K^{0}_{S}","#Lambda_{0}","#bar{#Lambda}_{0}"};	
	TH1D*		hpt_tagged[2][3];
	TH2D*		hdedxp_tagged[2][3];
	for (int iv=0;iv<3;iv++){
		for (int ic=0;ic<2;ic++){
			hpt_tagged[ic][iv]			= new TH1D(Form("hpt_tagged_%d_%d",ic,iv),Form("P_{T}, %s#rightarrow%s",strv0[iv],strch[ic]),1000,0.,5.);
			hdedxp_tagged[ic][iv]		= new TH2D(Form("hdedxp_tagged_%d_%d",ic,iv),Form("dE/dx vs p, %s#rightarrow%s",strv0[iv],strch[ic]),250,0.,5.,400,0.,8000.);
		}
	}
	//
	TH2D *hdedxp		= new TH2D("hdedxp"  ,"dedx vs p"      ,400,0,4.,1000,0.,10000.);
	TH2D *hdedxpz		= new TH2D("hdedxpz" ,"dedx vs p, zoom",200,0,2.,200,0., 2000.);
	TH1D *hhighestpt	= new TH1D("hhighestpt","hhighestpt",400,0.0,20.0);
	//
	TH1D *hxing			= new TH1D("hxing","crossing, all events",600,-100.5,499.5);
	TH1D *hKSxing		= new TH1D("hKSxing","crossing, KS found",600,-100.5,499.5);
	TH1D *hLAxing		= new TH1D("hLAxing","crossing, LA found",600,-100.5,499.5);
	TH1D *hALxing		= new TH1D("hALxing","crossing, AL found",600,-100.5,499.5);
	//---- coarser than hxing/hKSxing/etc -- with only O(10-100) V0 candidates, the fine
	//---- 600-bin crossing axis left entries too sparse (sub-pixel-wide bins) to actually see.
	TH2D *hKSmass_xing	= new TH2D("hKSmass_xing","hKSmass_xing" ,120,-100.5,499.5,100, 0.4, 0.6);
	TH2D *hLAmass_xing	= new TH2D("hLAmass_xing","hLAmass_xing" ,120,-100.5,499.5,100,1.085,1.145);
	TH2D *hALmass_xing	= new TH2D("hALmass_xing","hALmass_xing" ,120,-100.5,499.5,100,1.085,1.145);
	//
	//---- V0 kinematics, by species (KS/LA/AL)...
	TH1D *hKSmass	= new TH1D("hKSmass" ,"K^{0}_{S} mass"           ,200,0.4,0.6);
	TH1D *hKSpt		= new TH1D("hKSpt"   ,"K^{0}_{S} p_{T}"          ,200,0.,10.);
	TH1D *hKSeta	= new TH1D("hKSeta"  ,"K^{0}_{S} #eta"           ,120,-1.5,1.5);
	TH1D *hKSphi	= new TH1D("hKSphi"  ,"K^{0}_{S} #varphi"        ,96,-M_PI,M_PI);
	TH1D *hKSdira	= new TH1D("hKSdira" ,"K^{0}_{S} DIRA"           ,200,0.99,1.0);
	TH1D *hKSpvdca	= new TH1D("hKSpvdca","K^{0}_{S} PV_DCA"         ,200,0.,2.);
	TH1D *hKSworsedca	= new TH1D("hKSworsedca","K^{0}_{S} worse-daughter DCA (offline)"  ,200,0.,2.);
	TH1D *hLAmass	= new TH1D("hLAmass" ,"#Lambda_{0} mass"         ,60,1.085,1.145);
	TH1D *hLAmass_clean	= new TH1D("hLAmass_clean","#Lambda_{0} mass, worse-track PV_DCA cut",60,1.085,1.145);
	hLAmass_clean	->SetFillColor(3);	hLAmass_clean	->SetLineColor(1);
	TH1D *hLApt		= new TH1D("hLApt"   ,"#Lambda_{0} p_{T}"        ,200,0.,10.);
	TH1D *hLAeta	= new TH1D("hLAeta"  ,"#Lambda_{0} #eta"         ,120,-1.5,1.5);
	TH1D *hLAphi	= new TH1D("hLAphi"  ,"#Lambda_{0} #varphi"      ,96,-M_PI,M_PI);
	TH1D *hLAdira	= new TH1D("hLAdira" ,"#Lambda_{0} DIRA"         ,200,0.99,1.0);
	TH1D *hLApvdca	= new TH1D("hLApvdca","#Lambda_{0} PV_DCA"       ,200,0.,2.);
	TH1D *hLAworsedca	= new TH1D("hLAworsedca","#Lambda_{0} worse-daughter DCA (offline)",200,0.,2.);
	TH1D *hALmass	= new TH1D("hALmass" ,"#bar{#Lambda}_{0} mass"   ,60,1.085,1.145);
	TH1D *hALmass_clean	= new TH1D("hALmass_clean","#bar{#Lambda}_{0} mass, worse-track PV_DCA cut",60,1.085,1.145);
	hALmass_clean	->SetFillColor(3);	hALmass_clean	->SetLineColor(1);
	TH1D *hALpt		= new TH1D("hALpt"   ,"#bar{#Lambda}_{0} p_{T}"  ,200,0.,10.);
	TH1D *hALeta	= new TH1D("hALeta"  ,"#bar{#Lambda}_{0} #eta"   ,120,-1.5,1.5);
	TH1D *hALphi	= new TH1D("hALphi"  ,"#bar{#Lambda}_{0} #varphi",96,-M_PI,M_PI);
	TH1D *hALdira	= new TH1D("hALdira" ,"#bar{#Lambda}_{0} DIRA"   ,200,0.99,1.0);
	TH1D *hALpvdca	= new TH1D("hALpvdca","#bar{#Lambda}_{0} PV_DCA" ,200,0.,2.);
	TH1D *hALworsedca	= new TH1D("hALworsedca","#bar{#Lambda}_{0} worse-daughter DCA (offline)",200,0.,2.);
	//
	TH1D* hetotem	= new TH1D("hetotem","hetotem",500,-2,48);
	TH1D* hetotih	= new TH1D("hetotih","hetotih",500,-2,48);
	TH1D* hetotoh	= new TH1D("hetotoh","hetotoh",500,-2,48);
	TH1D* hetotioh	= new TH1D("hetotioh","hetotioh",500,-2,48);
	TH1D* hetotepd	= new TH1D("hetotepd","hetotepd",700,-2,698);
	//
	hntrk0	->SetLineWidth(1);
	hntpc0	->SetLineWidth(1);
	hnmvtx0	->SetLineWidth(1);
	hnintt0	->SetLineWidth(1);
	heta0	->SetLineWidth(1);
	hphi0	->SetLineWidth(1);
	hpt0	->SetLineWidth(1);
	hdcaxy0	->SetLineWidth(1);
	hdcaz0	->SetLineWidth(1);
	hvtxx0	->SetLineWidth(1);
	hvtxy0	->SetLineWidth(1);
	hvtxz0	->SetLineWidth(1);
	htrig0	->SetLineWidth(1);
	hntrk	->SetFillColor(3);		hntrk	->SetLineColor(1);
	hntpc	->SetFillColor(3);		hntpc	->SetLineColor(1);
	hnmvtx	->SetFillColor(3);		hnmvtx	->SetLineColor(1);
	hnintt	->SetFillColor(3);		hnintt	->SetLineColor(1);
	heta	->SetFillColor(3);		heta	->SetLineColor(1);
	hphi	->SetFillColor(3);		hphi	->SetLineColor(1);
	hpt		->SetFillColor(3);		hpt		->SetLineColor(1);
	hdcaxy	->SetFillColor(3);		hdcaxy	->SetLineColor(1);
	hdcaz	->SetFillColor(3);		hdcaz	->SetLineColor(1);
	hvtxx	->SetFillColor(3);		hvtxx	->SetLineColor(1);
	hvtxy	->SetFillColor(3);		hvtxy	->SetLineColor(1);
	hvtxz	->SetFillColor(3);		hvtxz	->SetLineColor(1);
	htrig	->SetFillColor(3);		htrig	->SetLineColor(1);

	//---- by (*run), filled over events
 	TProfile* hrirun_ntrk	= new TProfile("hrirun_ntrk"   ,"hrirun_ntrk"   ,RUNAXIS_NBIN,RUNAXIS_LO,RUNAXIS_HI, -0.5,399.5);
 	TProfile* hrirun_vtxx	= new TProfile("hrirun_vtxx"   ,"hrirun_vtxx"   ,RUNAXIS_NBIN,RUNAXIS_LO,RUNAXIS_HI,     -5,5  );
 	TProfile* hrirun_vtxy	= new TProfile("hrirun_vtxy"   ,"hrirun_vtxy"   ,RUNAXIS_NBIN,RUNAXIS_LO,RUNAXIS_HI,     -5,5  );
 	TProfile* hrirun_vtxz	= new TProfile("hrirun_vtxz"   ,"hrirun_vtxz"   ,RUNAXIS_NBIN,RUNAXIS_LO,RUNAXIS_HI,    -20,20 );
 	TProfile* hrirun_negfr	= new TProfile("hrirun_negfr"  ,"hrirun_negfr"  ,RUNAXIS_NBIN,RUNAXIS_LO,RUNAXIS_HI,      0,1  );
 	TH1D*     hrirun_nev	= new     TH1D("hrirun_nev"    ,"hrirun_nev"    ,RUNAXIS_NBIN,RUNAXIS_LO,RUNAXIS_HI);
 	TH1D*     hrirun_nevmb	= new     TH1D("hrirun_nevmb"  ,"hrirun_nevmb"  ,RUNAXIS_NBIN,RUNAXIS_LO,RUNAXIS_HI);
 	TH1D*     hrirun_nevjet	= new     TH1D("hrirun_nevjet" ,"hrirun_nevjet" ,RUNAXIS_NBIN,RUNAXIS_LO,RUNAXIS_HI);
 	TH1D*     hrirun_nevpho	= new     TH1D("hrirun_nevpho" ,"hrirun_nevpho" ,RUNAXIS_NBIN,RUNAXIS_LO,RUNAXIS_HI);
	//---- by (*run)(index), filled over tracks
 	TProfile* hrirun_eta	= new TProfile("hrirun_eta"    ,"hrirun_eta"    ,RUNAXIS_NBIN,RUNAXIS_LO,RUNAXIS_HI,   -1.5,1.5);
 	TProfile* hrirun_phi	= new TProfile("hrirun_phi"    ,"hrirun_phi"    ,RUNAXIS_NBIN,RUNAXIS_LO,RUNAXIS_HI, -M_PI,M_PI);
 	TProfile* hrirun_pt		= new TProfile("hrirun_pt"     ,"hrirun_pt"     ,RUNAXIS_NBIN,RUNAXIS_LO,RUNAXIS_HI,    0.,100.);
 	TProfile* hrirun_ntpc	= new TProfile("hrirun_ntpc"   ,"hrirun_ntpc"   ,RUNAXIS_NBIN,RUNAXIS_LO,RUNAXIS_HI,     0.,50.);
 	TProfile* hrirun_dedx	= new TProfile("hrirun_dedx"   ,"hrirun_dedx"   ,RUNAXIS_NBIN,RUNAXIS_LO,RUNAXIS_HI,   0.,2000.);
 	TProfile* hrirun_ndedx	= new TProfile("hrirun_ndedx"  ,"hrirun_ndedx"  ,RUNAXIS_NBIN,RUNAXIS_LO,RUNAXIS_HI,     0.,50.);
  
	//---- by runindex, filled over events
//	const int MAXRUNSSEEN	= 200;
	const int NTRIGTYPES	=   5;		// all,clock,mb,jet,pho
	//TProfile* hri_ntrk[NTRIGTYPES];
	//TProfile* hri_vtxx[NTRIGTYPES];
	//TProfile* hri_vtxy[NTRIGTYPES];
	//TProfile* hri_vtxz[NTRIGTYPES];
	//TProfile* hri_negfr[NTRIGTYPES];
	//TH1D*     hri_nev[NTRIGTYPES];
	//TH1D*     hri_nevmb[NTRIGTYPES];
	//TH1D*     hri_nevjet[NTRIGTYPES];
	//TH1D*     hri_nevpho[NTRIGTYPES];
	//TProfile* hri_eta[NTRIGTYPES];
	//TProfile* hri_phi[NTRIGTYPES];
	//TProfile* hri_pt[NTRIGTYPES];
	//TProfile* hri_ntpc[NTRIGTYPES];
	//TProfile* hri_dedx[NTRIGTYPES];
	//TProfile* hri_ndedx[NTRIGTYPES];
	for (int itt=0;itt<NTRIGTYPES;itt++){	
		//hri_ntrk[itt]	= new TProfile(Form("hri_ntrk%d"  ,itt),Form("hri_ntrk%d"  ,itt),NRUNSSEEN,-0.5,((double)NRUNSSEEN)-0.5, -0.5,399.5);
		//hri_vtxx[itt]	= new TProfile(Form("hri_vtxx%d"  ,itt),Form("hri_vtxx%d"  ,itt),NRUNSSEEN,-0.5,((double)NRUNSSEEN)-0.5,     -5,5  );
		//hri_vtxy[itt]	= new TProfile(Form("hri_vtxy%d"  ,itt),Form("hri_vtxy%d"  ,itt),NRUNSSEEN,-0.5,((double)NRUNSSEEN)-0.5,     -5,5  );
		//hri_vtxz[itt]	= new TProfile(Form("hri_vtxz%d"  ,itt),Form("hri_vtxz%d"  ,itt),NRUNSSEEN,-0.5,((double)NRUNSSEEN)-0.5,    -20,20 );
		//hri_negfr[itt]	= new TProfile(Form("hri_negfr%d" ,itt),Form("hri_negfr%d" ,itt),NRUNSSEEN,-0.5,((double)NRUNSSEEN)-0.5,      0,1  );
		//hri_nev[itt]	= new     TH1D(Form("hri_nev%d"   ,itt),Form("hri_nev%d"   ,itt),NRUNSSEEN,-0.5,((double)NRUNSSEEN)-0.5);
		//hri_nevmb[itt]	= new     TH1D(Form("hri_nevmb%d" ,itt),Form("hri_nevmb%d" ,itt),NRUNSSEEN,-0.5,((double)NRUNSSEEN)-0.5);
		//hri_nevjet[itt]	= new     TH1D(Form("hri_nevjet%d",itt),Form("hri_nevjet%d",itt),NRUNSSEEN,-0.5,((double)NRUNSSEEN)-0.5);
		//hri_nevpho[itt]	= new     TH1D(Form("hri_nevpho%d",itt),Form("hri_nevpho%d",itt),NRUNSSEEN,-0.5,((double)NRUNSSEEN)-0.5);
		//hri_eta[itt]	= new TProfile(Form("hri_eta%d"   ,itt),Form("hri_eta%d"   ,itt),NRUNSSEEN,-0.5,((double)NRUNSSEEN)-0.5,   -1.5,1.5);
		//hri_phi[itt]	= new TProfile(Form("hri_phi%d"   ,itt),Form("hri_phi%d"   ,itt),NRUNSSEEN,-0.5,((double)NRUNSSEEN)-0.5, -M_PI,M_PI);
		//hri_pt[itt]		= new TProfile(Form("hri_pt%d"    ,itt),Form("hri_pt%d"    ,itt),NRUNSSEEN,-0.5,((double)NRUNSSEEN)-0.5,    0.,100.);
		//hri_ntpc[itt]	= new TProfile(Form("hri_ntpc%d"  ,itt),Form("hri_ntpc%d"  ,itt),NRUNSSEEN,-0.5,((double)NRUNSSEEN)-0.5,     0.,50.);
		//hri_dedx[itt]	= new TProfile(Form("hri_dedx%d"  ,itt),Form("hri_dedx%d"  ,itt),NRUNSSEEN,-0.5,((double)NRUNSSEEN)-0.5,   0.,2000.);
		//hri_ndedx[itt]	= new TProfile(Form("hri_ndedx%d" ,itt),Form("hri_ndedx%d" ,itt),NRUNSSEEN,-0.5,((double)NRUNSSEEN)-0.5,     0.,50.);
	}
	//TH1D* hri_counters[NCOUNTERS];
	for (int icounter=0;icounter<NCOUNTERS;icounter++){
		//hri_counters[icounter]	= new TH1D(Form("hri_counters%d",icounter),Form("hri_counters%d",icounter),NRUNSSEEN,-0.5,((double)NRUNSSEEN)-0.5);	
	}
	//
	TH1::AddDirectory(kFALSE);
	//
	//---- end booking
	
	//---- loop through the tree...
	//
	int	run_highest	= 0;
	int	run_lowest	= 999999;
	//
	Long64_t nentries;
	if (ntodo!=0){ nentries = ntodo;        } 
	        else { nentries = nentriesfile; }
	if (nentries>nentriesfile){ nentries = nentriesfile; }
	cout<<"corral::Loop -- Event Loop over "<<nentries<<" events starting..."<<endl;	
	//
	if (doXTFClean) BuildXTFLosers(nentries);	// sec 18.22
	if (doTFDup)    BuildTFDupRows(nentries);	// overlapping-TF collision copies
	//
	int runprev			= -1;
	int segmentprev		= -1;
	//int kRunIndex		= -1;
	//int kRunIndexprev	= -1;
	int lastKeptTF_run	= -1;	// sec 18.21
	int lastKeptTF_evt	= -1;
	long nSkippedSameTF	= 0;
	long nSkippedXingNot0	= 0;	// sec 18.21 follow-up
	int lastPosTF_run	= -1;	// sec 18.21 follow-up 2
	int lastPosTF_evt	= -1;
	long nSkippedXingPos	= 0;
	for (Long64_t jentry=0; jentry<nentries && fReader.Next(); jentry++) {
		if (jentry%100000==0) cout<<"processing "<<jentry<<endl;
		//
		//---- a collision already seen in an overlapping earlier TF: not a new event (see doTFDup)
		if (doTFDup && jentry<(Long64_t)tfDupRows.size() && tfDupRows[jentry]) continue;
		//
		//---- new event processing...
		//---- sec 18.21: opt-in TF-deduplication -- keep only the FIRST row seen for each (run,evt)
		//---- (evt IS the trigger frame's own EvtSequence, constant across every row from that TF,
		//---- SDCC-claude sec 18.20), skip every subsequent row from the same TF entirely, before
		//---- ANY processing. Relies on same-TF rows being written consecutively (SDCC-claude
		//---- confirmed this within one file; cross-file TChain boundaries are not guaranteed to
		//---- preserve it, but a same-(run,evt) coincidence exactly at a file boundary is negligible).
		if (ONLY_FIRST_TF){
			if ((*run)==lastKeptTF_run && (*evt)==lastKeptTF_evt){
				++nSkippedSameTF;
				continue;
			}
			lastKeptTF_run	= (*run);
			lastKeptTF_evt	= (*evt);
		}
		//---- sec 18.21 follow-up: "Xing0" -- keep only the triggered crossing (crossing==0), at most
		//---- one row per TF by construction; pure per-row cut, before ANY processing.
		if (ONLY_CROSSING0 && (*crossing)!=0){
			++nSkippedXingNot0;
			continue;
		}
		//---- sec 18.21 follow-up 2: "XingPos" -- keep only the first crossing>0 row of each TF (pure
		//---- streaming, <=1 row/TF). Same consecutive-rows assumption as ONLY_FIRST_TF above.
		if (ONLY_FIRST_XINGPOS){
			if ((*crossing)<=0 || ((*run)==lastPosTF_run && (*evt)==lastPosTF_evt)){
				++nSkippedXingPos;
				continue;
			}
			lastPosTF_run	= (*run);
			lastPosTF_evt	= (*evt);
		}
		//
		//---- get runindex...		
		if ((*run)!=runprev || (*segment)!=segmentprev ){
			//int krunbin	= hRunIndex->GetXaxis()->FindBin((*run));
			//int ksegbin	= hRunIndex->GetYaxis()->FindBin((*segment));
			//kRunIndex	= hRunIndex->GetBinContent(krunbin,ksegbin);
			//cout<<"New run/segment seen... run="<<(*run)
			//	<<"\t bins="<<krunbin<<" "<<ksegbin
			//	<<"\t runc="<<hRunIndex->GetXaxis()->GetBinCenter(krunbin)
			//	<<"\t index="<<kRunIndex<<endl;
			if (runprev!=-1){	// don't fill in very first event!
				for (int icounter=0;icounter<NCOUNTERS;icounter++){
					//hri_counters[icounter]	->Fill(kRunIndexprev,counters[icounter]);
					counters[icounter]	= 0;
				}
			}
		}	
		//runprev	= (*run);	segmentprev	= (*segment);	kRunIndexprev = kRunIndex;
		//if (kRunIndex<0){ cout<<"No RunIndex Found for Run="<<(*run)<<endl; exit(0); }
		if ((*run)>run_highest) run_highest = (*run);
		if ((*run)<run_lowest)  run_lowest  = (*run);
		//		
		bool triggerIDs[64]	= {0};
		int  itrig			=  0 ;
		vector<bool>::iterator ptr; 
		for (ptr=trigVec->begin(); ptr<trigVec->end(); ptr++){
			if (*ptr){ 
				triggerIDs[itrig] = true; 
				htrig0		->Fill(itrig,1.); 
				htrigRun	->Fill((*run),itrig,1.); 
			}	// bit is on!
			++itrig;
		}
		hvtxx0		->Fill((*vtxx));
		hvtxy0		->Fill((*vtxy));
		hvtxz0		->Fill((*vtxz));
		hntrk0		->Fill((*ntr));
		//------------------------------
		bool keepevt	= AcceptEvent();
		if (!keepevt) continue;
		//------------------------------
		hvtxx		->Fill((*vtxx));
		hvtxy		->Fill((*vtxy));
		hvtxz		->Fill((*vtxz));
		//
		itrig			= 0;
		bool goodClock	= false;
		bool goodMB		= false;
		bool goodJet	= false;
		bool goodPho	= false;
		for (int itrig=0;itrig<NTRIGS;itrig++){
			if (triggerIDs[itrig]){		// this bit is ON!
				htrig->Fill(itrig,1.); 
				if (itrig== 0           ) goodClock	= true;
				if (itrig== 3           ) goodMB	= true;
				if (itrig>=10&&itrig<=14) goodMB	= true;
				if (itrig>=16&&itrig<=23) goodJet	= true;
				if (itrig>=24&&itrig<=31) goodPho	= true;
				if (itrig>=32&&itrig<=35) goodJet	= true;
				if (itrig>=36&&itrig<=38) goodPho	= true;
			}	// end check of trigger bit
		}	// end loop over trigger bits
		//
		//
		double	highestpt		= 0.0;
		//int	highestpt_it	= 0.0;
		for (int it=0;it<(*ntr);it++){
			if (AcceptTrack(it)){
				if (pt[it]>highestpt){
					highestpt		= pt[it];
					//highestpt_it	= it;
				}
			}
		}
		hhighestpt	->Fill(highestpt);
		hxing		->Fill((*crossing));
		if ((*crossing)==0){
			hetotem		->Fill((*etotem) );
			hetotih		->Fill((*etotih) );
			hetotoh		->Fill((*etotoh) );
			hetotioh	->Fill((*etotioh));
			hetotepd	->Fill((*etotepd));
		}
		//
		//---- first loop through the found V0's and make sure all daughters are distinct!!  NOW DONE ON SDCC
		//---- must search across V0 PIDs too!!!!
		//std::vector<int> tr1ids;
		//std::vector<int> tr2ids;
		//if ((*nv0)>1){
		//	//cout<<"-----------------------------------------"<<endl;
		//	for (int iv0=0;iv0<(*nv0);iv0++){
		//		//cout<<"IV0 PRELOOP .. "<<iv0<<" "<<(*nv0)<<"\t "<<v0pid[iv0]<<" "<<v0mass[iv0]<<"\t "<<v0indtr1[iv0]<<" "<<v0indtr2[iv0]<<endl;
		//		tr1ids.push_back(v0indtr1[iv0]);
		//		tr2ids.push_back(v0indtr2[iv0]);
		//		for (int jv0=0;jv0<iv0;jv0++){	// compare to all previous values
		//			if (v0indtr1[iv0]!=-1 && v0indtr1[iv0]==tr1ids.at(jv0)){
		//				//cout<<"TR1 dup "<<jentry<<"\t "<<v0indtr1[iv0]<<"\t jv0="<<jv0<<" iv0="<<iv0<<"\t "<<v0pid[jv0]<<" "<<v0pid[iv0]
		//				//	<<"\t "<<v0chi2ndf[jv0]<<" "<<v0chi2ndf[iv0]<<endl;
		//			}
		//			if (v0indtr2[iv0]!=-1 && v0indtr2[iv0]==tr2ids.at(jv0)){
		//				//cout<<"TR2 dup "<<jentry<<"\t "<<v0indtr2[iv0]<<"\t jv0="<<jv0<<" iv0="<<iv0<<"\t "<<v0pid[jv0]<<" "<<v0pid[iv0]
		//				//	<<"\t "<<v0chi2ndf[jv0]<<" "<<v0chi2ndf[iv0]<<endl;
		//			}
		//		}
		//	}
		//	tr1ids.clear();
		//	tr2ids.clear();
		//	//cout<<"\t\t that was event "<<jentry<<" ----------- "<<endl;
		//} 
		//
		//---- split-track removal pre-pass (README_SplitTracks.md sec 13.8.4/13.8.6/13.8.8): computes
		//---- the real STAR-style SL (layermask, sec 9.1/9.2) AND SiKeyFrac (siclukey hit-identity,
		//---- sec 12a.4) for every same-charge candidate pair inside the actual (dy,dphi) spike --
		//---- |deta|<0.05, |dphi|<10deg, the EXACT edges of the two dphi bins straddling dphi=0 in
		//---- the hR2_1_<ipaty> histogram itself (sec 13.8.8), replacing the earlier, narrower,
		//---- ad-hoc |deta|<0.01,|dphi|<0.03 guess (sec 3) outright -- a direct measurement (sec
		//---- 13.8.8) showed same-charge pairs inside the true spike bin but outside that narrower
		//---- window still carry a real split-track signature (mean SiKeyFrac 0.357 and 29.1% exact
		//---- Si-cluster match, vs ~0% for a genuinely far/independent control) that the old window
		//---- was silently missing -- fills hSL_cand/hSiKeyFrac_cand/
		//---- hntpc_ij_cand every event regardless of doSplitRemoval, so a threshold scan always has
		//---- something to look at. Gated by its OWN switch, doSplitRemoval/valSLCut/valSiKeyCut
		//---- (sec 13.8.6: this used to also share a boolean with a since-deleted standalone
		//---- CalcRm::PairInfo SL+SiSeedMatch cut that was NOT kinematically-gated at all -- that
		//---- confound is why the two were split apart, and sec 17.9 later removed the old cut
		//---- entirely). A pair is flagged split when BOTH SiKeyFrac>=valSiKeyCut (literal
		//---- shared-cluster identity -- the sharp gate) AND SL<valSLCut (shared TPC coverage) --
		//---- this REPLACES the old SiSeedMatch-based hybrid gate used here through 2026-09-20, now
		//---- that SiKeyFrac (~0.003% far-population false-positive rate) is a much sharper signal
		//---- than SiSeedMatch's coverage-pattern match (~14%, sec 11.5/13.8.1). Winner/loser: fewer
		//---- ntpc loses, tie-break worse/higher quality loses, else no loser (sec 9.5, unchanged).
		//---- Unlike the SL-monitor-only era (2026-09-19 through 2026-09-20), the loser is now
		//---- REALLY REMOVED below (revives the pre-`layermask` KILLSPLITTRACKS mechanism's
		//---- `continue`, sec 5, but driven by this far sharper discriminator) -- this fixes event
		//---- multiplicity/rho1 double-counting that a CalcRm::PairInfo-only pairwise exclusion
		//---- cannot reach (sec 13.8.4's tradeoff writeup). The old, separate CalcRm::PairInfo
		//---- SL+SiSeedMatch cut (sec 10.4) this pre-pass replaced is gone entirely (sec 17.9).
		std::vector<int> nSplitFlagged_thisevt;
		//---- sec 18.22: cross-crossing duplicates found by BuildXTFLosers() -- same track-level
		//---- removal as the LS/ULS paths below (they skip anything already in this list).
		if (doXTFClean){
			auto itx = xtfLosers.find(jentry);
			if (itx!=xtfLosers.end()) nSplitFlagged_thisevt = itx->second;
		}
		{
			//---- fixed, measured, deliberately-margined box (sec 13.8.13), NOT the old ad-hoc
			//---- |deta|<0.01,|dphi|<0.03 guess
			//---- FIXED box (sec 13.8.13), pure track-level thresholds -- deliberately NOT tied to any
			//---- histogram binning (thisYNB0/thisPHINB0), so changing the analysis binning never
			//---- again silently changes what counts as a split-track candidate (sec 13.8.8's bug).
			//---- A 92x96 full-stats, no-cut "imaging" pass (sec 13.8.11) directly measured the true
			//---- spike's extent at |deta|<0.012,|dphi|<1.875deg, comfortably inside even the previous
			//---- 52x72-derived box (1.1/52, 5deg) -- zero spillover found at every binning tested,
			//---- 22x36 up through 92x96 (secs 13.8.9/13.8.11). Rather than match that measurement
			//---- exactly, deliberately kept a real (not maximal) safety margin given this is
			//---- preliminary, self-produced data, not a finalized production (user's call, sec
			//---- 13.8.13) -- 0.02 in |deta|, 3deg in |dphi|, tighter than the old 52x72-derived box
			//---- but looser than the measured true edge. See sec 13.8.13 for the monitoring plan
			//---- (watch the post-cut (dy,dphi)=(0,0) bin for a reappearing spike) if a future
			//---- production's real spike turns out to need a looser box than this.
			//---- 2026-09-22 (README sec 16.8/16.9): ELLIPTICAL pregate, replacing the old 0.02 x 4deg
			//---- rectangle (whose "measured edges" were only bin widths at 92x96/128x120, i.e. upper
			//---- limits). Sized from the full-stats fine zoom (hzoomS/hzoomM, 0.001 x 0.1deg, job
			//---- 40415627) with SpikeExtent.C: smallest ellipse containing every NON-ISOLATED >3 sigma
			//---- sibling/mixed bin -- pi+pi+ a=0.0216 b=3.99deg, pi-pi- a=0.0198 b=4.06deg; this is their
			//---- envelope rounded up to the zoom's bin size. User's rule: contain the ENTIRE spike, as
			//---- narrow as possible around it, NO safety margin. LOCKED (user, 2026-09-22), modulo the
			//---- pt-ordered zoom run (job 40415716). The ellipse avoids the rectangle's corners, where
			//---- the neighbouring acceptance (a ~25-30% dip, possibly crossing) sits.
			static const double PREGATE_DETA	= 0.022;			// ellipse semi-axis in deta
			static const double PREGATE_DPHI	= 4.1*M_PI/180.0;	// ellipse semi-axis in dphi (rad)
			//---- ULS diagnostic sideband (sec 15) -- clearly-unrelated control region, same
			//---- definition as sec 5's original close-vs-sideband table (0.05<=dr<0.5).
			static const double ULSSIDEBAND_LO	= 0.05;
			static const double ULSSIDEBAND_HI	= 0.5;
			std::vector<int> pionidx;
			for (int jt=0;jt<(*ntr);jt++){
				if (indv0[jt]>=0) continue;
				if (!AcceptTrack(jt)) continue;
				if (dedx70s[jt]>=400.) continue;
				if (chg[jt]==0) continue;
				pionidx.push_back(jt);
			}
			for (size_t ii=0;ii<pionidx.size();ii++){
				for (size_t jj=ii+1;jj<pionidx.size();jj++){
					int i=pionidx[ii], j=pionidx[jj];
					//---- sec 15: no longer charge-gated here -- classify same-charge (LS) vs
					//---- opposite-charge (ULS) below instead of skipping ULS outright, so the ULS
					//---- diagnostic (this section) can run alongside the unchanged LS production
					//---- logic. ULS never reaches the doSplitRemoval flagging/removal block below.
					double deta	= eta[i]-eta[j];
					double dphi	= phi[i]-phi[j];
					if (dphi> M_PI) dphi -= 2*M_PI;
					if (dphi<-M_PI) dphi += 2*M_PI;
					bool sameCharge	= (chg[i]*chg[j] > 0);
					bool inBox		= ( (deta/PREGATE_DETA)*(deta/PREGATE_DETA) + (dphi/PREGATE_DPHI)*(dphi/PREGATE_DPHI) < 1.0 );	// elliptical pregate, sec 16.9
					double dr		= sqrt(deta*deta+dphi*dphi);
					bool inSideband	= (!inBox && dr>=ULSSIDEBAND_LO && dr<ULSSIDEBAND_HI);
					//---- sec 18.24: ULS TRACK-level veto, NO angular gate. Was inside the LS pregate
					//---- ellipse below (dphi semi-axis 4.1deg), but the shared-Si-seed pi+pi- ridge sits at
					//---- dphi ~ 1.25 + 1.13/pt deg (sec 18.22), i.e. beyond 4.1deg at low pt -- those pairs
					//---- were only caught by CalcRm::PairInfo's sibling-only pair veto (fDoULSTest, now
					//---- removed: a numerator-only cut, 0.40% of sibling pi+pi- pairs). Within one event a
					//---- shared siclukey IS a shared physical cluster, so no angular gate is needed.
					//---- Same loser rule as before (fewer ntpc loses, tie-break worse/higher quality).
					if (!sameCharge && doULSTest){
						double SKFv	= SiSplitScore(&siclukey[i*7],&siclukey[j*7],layermask[i],layermask[j]);
						if (SKFv>=valULSTestCut){
							int loserv = -1;
							if (ntpc[i]!=ntpc[j]){
								loserv	= (ntpc[i]<ntpc[j]) ? i : j;
							} else if (quality[i]!=quality[j]){
								loserv	= (quality[i]>quality[j]) ? i : j;
							}
							if (loserv>=0){
								++nFlagged_ULS;
								if (std::find(nSplitFlagged_thisevt.begin(),nSplitFlagged_thisevt.end(),loserv)==nSplitFlagged_thisevt.end()){
									nSplitFlagged_thisevt.push_back(loserv);
								}
							}
						}
					}
					if (!inBox && !inSideband) continue;
					if (inBox && !sameCharge){
						//---- ULS close-box diagnostic (sec 15) -- NOT the production same-charge
						//---- path below, purely a measurement of whether the sharp cluster-identity
						//---- discriminators show any elevation the old blunt proxy (sec 5) missed.
						double SLu		= ComputeSplitSL(layermask[i],layermask[j]);
						double SKFlegacyu	= SiKeyFrac(&siclukey[i*7],&siclukey[j*7],layermask[i],layermask[j]);
						double SKFu		= SiSplitScore(&siclukey[i*7],&siclukey[j*7],layermask[i],layermask[j]);
						hSL_cand_ULS		->Fill(SLu);
						hSiKeyFrac_cand_ULS	->Fill(SKFlegacyu);
						hSiSplitScore_cand_ULS	->Fill(SKFu);
						hSLvsSKF_cand_ULS	->Fill(SLu,SKFu);
						//---- sec 18.18: SIBLING d_mm, ridge/mirror/origin circles (same size, mirror is
						//---- the exact opposite-sign dphi window per user's request, sec 18.17)
						{
							double dphiDeg	= dphi*180.0/M_PI;
							bool isRidgeS	= (fabs(deta)<=0.003 && dphiDeg>=3.0 && dphiDeg<4.0);
							bool isMirrorS	= (fabs(deta)<=0.003 && dphiDeg>=-4.0 && dphiDeg<-3.0);
							bool isOriginS	= (fabs(deta)<=0.003 && dphiDeg>=-0.3 && dphiDeg<0.3);
							if (isRidgeS || isMirrorS || isOriginS){
								TH1D *target = isRidgeS? hDmm_ULSridge_sibling : (isMirrorS? hDmm_ULSmirror_sibling : hDmm_ULSorigin_sibling);
								for (int L=0; L<48; L++){
									bool hasI = layermask[i] & (1ULL<<(7+L));
									bool hasJ = layermask[j] & (1ULL<<(7+L));
									if (!hasI || !hasJ) continue;
									int si = tpcsector[i*48+L], sj = tpcsector[j*48+L];
									if (si==255 || sj==255 || si!=sj) continue;
									double dArc = (tpcarclen[i*48+L]-tpcarclen[j*48+L])*2.0;
									double dZ   = (tpcz[i*48+L]-tpcz[j*48+L])*0.1;
									double dmm  = sqrt(dArc*dArc+dZ*dZ);
									target->Fill(dmm);
								}
							}
						}
						//---- SL/fit-quality asymmetry follow-up (sec 15) -- same winner/loser rule as
						//---- the production same-charge cut (sec 9.5: fewer ntpc loses, tie-break
						//---- worse/higher quality loses), applied here purely as a diagnostic label,
						//---- split by whether this ULS pair looks like a real duplicate (SKFu>=0.60)
						//---- or background (SKFu<0.60).
						//---- proper, non-tautological asymmetry check (sec 15) -- symmetric under
						//---- i<->j swap, filled for every ULS close-box pair regardless of the
						//---- loser/winner labeling below.
						double ptreldiff	= fabs(pt[i]-pt[j]) / (0.5*(pt[i]+pt[j]));
						double ptotreldiff	= fabs(ptot[i]-ptot[j]) / (0.5*(ptot[i]+ptot[j]));
						//---- Q_inv (sec 15) -- build 4-vectors directly from (pt,eta,phi) + assumed
						//---- pion mass, same identical-mass formula as CalcRm::PairInfo.
						double mpi	= Species_mass[0];
						double pxA	= pt[i]*cos(phi[i]), pyA = pt[i]*sin(phi[i]), pzA = pt[i]*sinh(eta[i]);
						double eA	= sqrt(pxA*pxA+pyA*pyA+pzA*pzA+mpi*mpi);
						double pxB	= pt[j]*cos(phi[j]), pyB = pt[j]*sin(phi[j]), pzB = pt[j]*sinh(eta[j]);
						double eB	= sqrt(pxB*pxB+pyB*pyB+pzB*pzB+mpi*mpi);
						double Qinv	= sqrt(std::max(0.0, (pxA-pxB)*(pxA-pxB)+(pyA-pyB)*(pyA-pyB)+(pzA-pzB)*(pzA-pzB)-(eA-eB)*(eA-eB)));
						if (SKFu>=0.60){
							hqualitydiff_ULS_hiSKF	->Fill(fabs(quality[i]-quality[j]));
							hptreldiff_ULS_hiSKF	->Fill(ptreldiff);
							hptotreldiff_ULS_hiSKF	->Fill(ptotreldiff);
							hQinv_cand_ULS_hiSKF	->Fill(Qinv);
						} else {
							hqualitydiff_ULS_loSKF	->Fill(fabs(quality[i]-quality[j]));
							hptreldiff_ULS_loSKF	->Fill(ptreldiff);
							hptotreldiff_ULS_loSKF	->Fill(ptotreldiff);
							hQinv_cand_ULS_loSKF	->Fill(Qinv);
						}
						{
							int loseru = -1, winneru = -1;
							if (ntpc[i]!=ntpc[j]){
								loseru	= (ntpc[i]<ntpc[j]) ? i : j;
							} else if (quality[i]!=quality[j]){
								loseru	= (quality[i]>quality[j]) ? i : j;
							}
							if (loseru>=0){
								winneru	= (loseru==i) ? j : i;
								if (SKFu>=0.60){
									hntpc_loser_ULS_hiSKF		->Fill(ntpc[loseru]);
									hntpc_winner_ULS_hiSKF		->Fill(ntpc[winneru]);
									hquality_loser_ULS_hiSKF	->Fill(quality[loseru]);
									hquality_winner_ULS_hiSKF	->Fill(quality[winneru]);
									hpt_loser_vs_winner_ULS_hiSKF	->Fill(pt[loseru],pt[winneru]);
								} else {
									hntpc_loser_ULS_loSKF		->Fill(ntpc[loseru]);
									hntpc_winner_ULS_loSKF		->Fill(ntpc[winneru]);
									hquality_loser_ULS_loSKF	->Fill(quality[loseru]);
									hquality_winner_ULS_loSKF	->Fill(quality[winneru]);
									hpt_loser_vs_winner_ULS_loSKF	->Fill(pt[loseru],pt[winneru]);
								}
								//---- sec 18.10: TRACK-level removal, same as LS's cascade below -- was a
								//---- sibling-pair-only veto inside CalcRm::PairInfo (fDoULSTest), which left
								//---- the loser track sitting in Pvec_1/Pvec_2 and hence mix_part1/2 for every
								//---- OTHER event pairing (sec 18's own finding: a compact, real spike in the
								//---- raw MIXED density, hzoomM_0, at the same (dy,dphi) as the sibling ridge --
								//---- a single-particle-density defect that no sibling-only veto can reach,
								//---- since event mixing only kills genuine two-body correlations, not a
								//---- reconstruction artifact reproduced independently in every event).
								//---- sec 18.24: the flagging itself moved OUT of this pregate block (see the
								//---- ungated ULS veto just above the pregate `continue`) -- this block is now
								//---- diagnostics only.
							}
						}
						continue;
					}
					if (inSideband){
						//---- sideband diagnostic (sec 15), both charge combinations -- the
						//---- clearly-unrelated control population for the close-box elevation ratio.
						double SLf		= ComputeSplitSL(layermask[i],layermask[j]);
						double SKFlegacyf	= SiKeyFrac(&siclukey[i*7],&siclukey[j*7],layermask[i],layermask[j]);
						double SKFf		= SiSplitScore(&siclukey[i*7],&siclukey[j*7],layermask[i],layermask[j]);
						if (sameCharge){
							hSL_far_LS		->Fill(SLf);
							hSiKeyFrac_far_LS	->Fill(SKFlegacyf);
							hSiSplitScore_far_LS	->Fill(SKFf);
							double mvtxFracf,inttFracf; int mvtxDenf,inttDenf;		// sec 17
							SiKeyFracSplit(&siclukey[i*7],&siclukey[j*7],layermask[i],layermask[j],
							               mvtxFracf,mvtxDenf,inttFracf,inttDenf);
							hMVTXKeyFrac_far_LS		->Fill(mvtxFracf);
							hINTTKeyFrac_far_LS		->Fill(inttFracf);
							hMVTXvsINTTKeyFrac_far_LS	->Fill(mvtxFracf,inttFracf);
							double RGf	= ComputeRadialGap(layermask[i],layermask[j]);		// sec 17.11
							if (RGf>=0.0) hRadialGap_far_LS->Fill(RGf);
						} else {
							hSL_far_ULS		->Fill(SLf);
							hSiKeyFrac_far_ULS	->Fill(SKFlegacyf);
							hSiSplitScore_far_ULS	->Fill(SKFf);
						}
						continue;
					}
					//---- from here on: inBox && sameCharge -- the EXISTING, unchanged production
					//---- same-charge path (sec 13.8/13.9/14), byte-identical to before sec 15.
					double SL		= ComputeSplitSL(layermask[i],layermask[j]);
					double SKFlegacy	= SiKeyFrac(&siclukey[i*7],&siclukey[j*7],layermask[i],layermask[j]);
					hSL_cand		->Fill(SL);
					hSiKeyFrac_cand	->Fill(SKFlegacy);			// LEGACY combined metric, retained for comparison
					hntpc_ij_cand	->Fill(ntpc[i],ntpc[j]);
					//---- MVTX/INTT split (sec 13.9) -- SKF below (the actual cut-driving value, replacing
					//---- SKFlegacy as of this update) is SiSplitScore, MVTX-primary/INTT-secondary
					double mvtxFrac,inttFrac; int mvtxDen,inttDen;
					SiKeyFracSplit(&siclukey[i*7],&siclukey[j*7],layermask[i],layermask[j],
					               mvtxFrac,mvtxDen,inttFrac,inttDen);
					hMVTXKeyFrac_cand		->Fill(mvtxFrac);
					hINTTKeyFrac_cand		->Fill(inttFrac);
					hMVTXvsINTTavail_cand	->Fill(mvtxDen,inttDen);
					hMVTXvsINTTKeyFrac_cand	->Fill(mvtxFrac,inttFrac);
					double SKF	= SiSplitScore(&siclukey[i*7],&siclukey[j*7],layermask[i],layermask[j]);
					hSiSplitScore_cand	->Fill(SKF);
					double RG	= ComputeRadialGap(layermask[i],layermask[j]);		// sec 17.11
					if (RG>=0.0){
						hRadialGap_cand		->Fill(RG);
						hRadialGap_vs_SL_cand	->Fill(SL,RG);
						hRadialGap_vs_SKF_cand	->Fill(SKF,RG);
					}
					//---- sec 17: fine position map, split by partial-MVTX sub-population -- does the
					//---- INTT-agreeing half of each partial band cluster at (0,0) like true duplicates,
					//---- or spread out like the mvtxFrac==0 background reference?
					{
						double dphideg	= dphi*180.0/M_PI;
						bool mvtx13	= (fabs(mvtxFrac-1.0/3.0)<0.01);
						bool mvtx23	= (fabs(mvtxFrac-2.0/3.0)<0.01);
						bool inttAgree	= (inttFrac>=0.75);
						if (mvtxFrac<=0.0)			hPos_mvtx0			->Fill(deta,dphideg);
						else if (mvtxFrac>=1.0)			hPos_mvtx1			->Fill(deta,dphideg);
						else if (mvtx13 && inttAgree)		hPos_mvtx13_inttAgree		->Fill(deta,dphideg);
						else if (mvtx13 && !inttAgree)		hPos_mvtx13_inttDisagree	->Fill(deta,dphideg);
						else if (mvtx23 && inttAgree)		hPos_mvtx23_inttAgree		->Fill(deta,dphideg);
						else if (mvtx23 && !inttAgree)		hPos_mvtx23_inttDisagree	->Fill(deta,dphideg);
					}
					//---- sec 17.10: ridge box test -- see histogram declarations above for the
					//---- rationale. Independent of doSplitRemoval (like the diagnostics above), so it
					//---- always has something to look at.
					{
						double mpiR	= Species_mass[0];
						double pxAR	= pt[i]*cos(phi[i]), pyAR = pt[i]*sin(phi[i]), pzAR = pt[i]*sinh(eta[i]);
						double eAR	= sqrt(pxAR*pxAR+pyAR*pyAR+pzAR*pzAR+mpiR*mpiR);
						double pxBR	= pt[j]*cos(phi[j]), pyBR = pt[j]*sin(phi[j]), pzBR = pt[j]*sinh(eta[j]);
						double eBR	= sqrt(pxBR*pxBR+pyBR*pyBR+pzBR*pzBR+mpiR*mpiR);
						double dyR	= 0.5*log((eAR+pzAR)/(eAR-pzAR)) - 0.5*log((eBR+pzBR)/(eBR-pzBR));
						double dphidegR	= dphi*180.0/M_PI;
						if (fabs(dyR)<0.010 && fabs(dphidegR)<0.2){
							bool wouldFlag		= (SKF>=valSiKeyCut && SL<valSLCut);
							double ptreldiffR	= fabs(pt[i]-pt[j]) / (0.5*(pt[i]+pt[j]));
							double ptotreldiffR	= fabs(ptot[i]-ptot[j]) / (0.5*(ptot[i]+ptot[j]));
							double qualitydiffR	= fabs(quality[i]-quality[j]);
							double dcaxydiffR	= fabs(dcaxy[i]-dcaxy[j]);
							double dcazdiffR	= fabs(dcaz[i]-dcaz[j]);
							double dedxdiffR	= fabs(dedx70s[i]-dedx70s[j]);
							(wouldFlag? hptreldiff_ridge_flagged    : hptreldiff_ridge_survive   )->Fill(ptreldiffR);
							(wouldFlag? hptotreldiff_ridge_flagged  : hptotreldiff_ridge_survive )->Fill(ptotreldiffR);
							(wouldFlag? hqualitydiff_ridge_flagged  : hqualitydiff_ridge_survive )->Fill(qualitydiffR);
							(wouldFlag? hntpc_ij_ridge_flagged      : hntpc_ij_ridge_survive      )->Fill(ntpc[i],ntpc[j]);
							(wouldFlag? hdcaxydiff_ridge_flagged    : hdcaxydiff_ridge_survive    )->Fill(dcaxydiffR);
							(wouldFlag? hdcazdiff_ridge_flagged     : hdcazdiff_ridge_survive     )->Fill(dcazdiffR);
							(wouldFlag? hdedxdiff_ridge_flagged     : hdedxdiff_ridge_survive     )->Fill(dedxdiffR);
							//---- sec 17.19: per-shared-TPC-layer cluster position agreement
							{
								int nSharedLayer=0, nSectorMatch=0;
								for (int L=0; L<48; L++){
									bool hasI = layermask[i] & (1ULL<<(7+L));
									bool hasJ = layermask[j] & (1ULL<<(7+L));
									if (!hasI || !hasJ) continue;
									++nSharedLayer;
									int si = tpcsector[i*48+L], sj = tpcsector[j*48+L];
									if (si==255 || sj==255 || si!=sj) continue;
									++nSectorMatch;
									double dArc = (tpcarclen[i*48+L]-tpcarclen[j*48+L])*2.0;
									double dZ   = (tpcz[i*48+L]-tpcz[j*48+L])*0.1;
									double dmm  = sqrt(dArc*dArc+dZ*dZ);
									(wouldFlag? hDmm_ridge_flagged : hDmm_ridge_survive)->Fill(dmm);
								}
								if (nSharedLayer>0){
									(wouldFlag? hSectorMatchFrac_ridge_flagged : hSectorMatchFrac_ridge_survive)
										->Fill((double)nSectorMatch/nSharedLayer);
								}
							}
							(wouldFlag? hSLvsSKF_ridge_flagged      : hSLvsSKF_ridge_survive      )->Fill(SL,SKF);
							//---- sec 17.11: radial structure -- only for the two populations we're
							//---- actually contrasting (the suspected fig-1b population vs the known-
							//---- duplicate negative control), not every survivor.
							bool isComplementary	= (!wouldFlag && SKF>=0.95 && SL>0.9);
							if (wouldFlag || isComplementary){
								ULong64_t ma	= layermask[i] & kTPCLayerMask;
								ULong64_t mb	= layermask[j] & kTPCLayerMask;
								double sumA=0,sumB=0; int nA=0,nB=0;
								for (int L=0; L<48; L++){
									if (ma & (1ULL<<(7+L))){ sumA+=L; ++nA; }
									if (mb & (1ULL<<(7+L))){ sumB+=L; ++nB; }
								}
								if (nA>0 && nB>0){
									double meanA	= sumA/nA;
									double meanB	= sumB/nB;
									ULong64_t mLo	= (meanA<=meanB) ? ma : mb;
									ULong64_t mHi	= (meanA<=meanB) ? mb : ma;
									double idxLo	= (meanA<=meanB) ? meanA : meanB;
									double idxHi	= (meanA<=meanB) ? meanB : meanA;
									TH1D *hLo	= wouldFlag? hTPClayer_lower_flagged : hTPClayer_lower_complementary;
									TH1D *hHi	= wouldFlag? hTPClayer_upper_flagged : hTPClayer_upper_complementary;
									TH2D *h2	= wouldFlag? hTPCmeanIdx_flagged     : hTPCmeanIdx_complementary;
									for (int L=0; L<48; L++){
										if (mLo & (1ULL<<(7+L))) hLo->Fill(L);
										if (mHi & (1ULL<<(7+L))) hHi->Fill(L);
									}
									h2->Fill(idxLo,idxHi);
								}
							}
						}
					}
					if (!doSplitRemoval) continue;				// scan not yet enabled -- monitor only, don't flag
					if (SKF < valSiKeyCut) continue;			// sharp gate, sec 12a.4/13.8.1/13.9 -- SiSplitScore --
																// same-Si-seed requirement for BOTH paths below
					//---- sec 17.14: two independent flagging paths, both requiring the SKF gate above
					//---- (same real particle already established) -- SL alone only ever sees pad-row
					//---- OVERLAP (sec 17.11), so it can only catch the "duplicate" mechanism (path 1).
					//---- RadialGap sees radial POSITION instead, catching the "complementary"/STAR-fig-1b
					//---- mechanism SL is structurally blind to (path 2, mutually exclusive with path 1 by
					//---- construction -- RG was only computed/checked because SL already failed path 1).
					bool flagDuplicate	= (SL < valSLCut);					// path 1 -- live since sec 17.6
					bool flagComplementary	= (!flagDuplicate && !DISABLE_RG && RG>=0.0 && RG>=valRadialGapCut);	// path 2 -- sec 17.14/17.17
					if (!flagDuplicate && !flagComplementary) continue;		// not flagged by either path
					if (flagDuplicate) ++nFlagged_duplicate; else ++nFlagged_complementary;
					int loser = -1;
					if (ntpc[i]!=ntpc[j]){
						loser	= (ntpc[i]<ntpc[j]) ? i : j;
					} else if (quality[i]!=quality[j]){
						loser	= (quality[i]>quality[j]) ? i : j;	// worse (higher) quality loses, sec 9.5
					}													// else: still tied -> no loser, sec 9.5 pt.3
					if (loser>=0
					 && std::find(nSplitFlagged_thisevt.begin(),nSplitFlagged_thisevt.end(),loser)==nSplitFlagged_thisevt.end()){
						nSplitFlagged_thisevt.push_back(loser);	// don't double-count if flagged by >1 partner
					}
				}
			}
			//---- sec 15 follow-up: unrestricted-angle Qinv-vs-SiSplitScore check (see the
			//---- histogram declarations above for the full rationale). Separate loop, same
			//---- pionidx list, deliberately NOT interleaved with the box/sideband logic above to
			//---- keep this purely diagnostic addition easy to reason about/remove independently.
			for (size_t ii=0;ii<pionidx.size();ii++){
				for (size_t jj=ii+1;jj<pionidx.size();jj++){
					int i=pionidx[ii], j=pionidx[jj];
					if (chg[i]*chg[j]>0) continue;			// opposite-charge (ULS) only, any angle
					double mpiA		= Species_mass[0];
					double pxA2		= pt[i]*cos(phi[i]), pyA2 = pt[i]*sin(phi[i]), pzA2 = pt[i]*sinh(eta[i]);
					double eA2		= sqrt(pxA2*pxA2+pyA2*pyA2+pzA2*pzA2+mpiA*mpiA);
					double pxB2		= pt[j]*cos(phi[j]), pyB2 = pt[j]*sin(phi[j]), pzB2 = pt[j]*sinh(eta[j]);
					double eB2		= sqrt(pxB2*pxB2+pyB2*pyB2+pzB2*pzB2+mpiA*mpiA);
					double Qinv2	= sqrt(std::max(0.0,(pxA2-pxB2)*(pxA2-pxB2)+(pyA2-pyB2)*(pyA2-pyB2)+(pzA2-pzB2)*(pzA2-pzB2)-(eA2-eB2)*(eA2-eB2)));
					if (Qinv2>=0.3) continue;				// match hCQ_0's own range
					double SKFall	= SiSplitScore(&siclukey[i*7],&siclukey[j*7],layermask[i],layermask[j]);
					if (SKFall>=0.60){
						hQinv_allULS_hiSKF	->Fill(Qinv2);
					} else {
						hQinv_allULS_loSKF	->Fill(Qinv2);
					}
				}
			}
			if (nSplitFlagged_thisevt.size()>0) ++nSplitFlagged_evts;
			nSplitFlagged_total	+= (long)nSplitFlagged_thisevt.size();
		}
		//
		//---- track loop...
		int ntrkept		= 0;
		double npos=0,nneg=0;
		for (int ipaty=0;ipaty<NPairTypes;ipaty++){
			calcr_n_1[ipaty] = 0;
			calcr_n_2[ipaty] = 0;
		}
		for (int it=0;it<(*ntr);it++){
			//
			hntpc0		->Fill(ntpc[it]);
			hnmvtx0		->Fill(nmvtx[it]);
			hnintt0		->Fill(nintt[it]);
			heta0		->Fill(eta[it]);
			hphi0		->Fill(phi[it]);
			hpt0		->Fill(pt[it]);
			hphieta0	->Fill(eta[it],phi[it]);
			hpteta0		->Fill(eta[it],pt[it]);
			hptphi0		->Fill(phi[it],pt[it]);
			hdcaxy0		->Fill(dcaxy[it]);
			hdcaz0		->Fill(dcaz[it]);
			hdcaxyz0	->Fill(dcaz[it],dcaxy[it]);
			hdedx0		->Fill(dedx70s[it]);
			hndedx0		->Fill(dedx70n[it]);
			hdedxphi0	->Fill(phi[it],dedx70s[it]);
			if (eta[it]>=0) hdedxphi_etap0->Fill(phi[it],dedx70s[it]);
			if (eta[it]< 0) hdedxphi_etan0->Fill(phi[it],dedx70s[it]);
			//
			//------------------------------
			//
			//---- track quality cuts...
			bool keeptrk	= AcceptTrack(it);
			if (!keeptrk) continue;
			//---- split-track removal (see README_SplitTracks.md sec 13.8.4): this track was flagged
			//---- as the loser of a split-candidate pair above (SiKeyFrac+SL cascade, doSplitRemoval-gated).
			//---- `continue`s past PID tagging, the regular "kept" QA histograms, and Pvec_1/Pvec_2
			//---- (CF pair-building) entirely -- revives the pre-layermask KILLSPLITTRACKS mechanism
			//---- (sec 5), now driven by a far sharper discriminator. This also means the removed
			//---- track no longer inflates event multiplicity or rho1 (single-particle density) --
			//---- the gap a CalcRm::PairInfo-only pairwise exclusion could not close (sec 13.8.4).
			if (std::find(nSplitFlagged_thisevt.begin(),nSplitFlagged_thisevt.end(),it)!=nSplitFlagged_thisevt.end()){
				heta_killed		->Fill(eta[it]);
				hphi_killed		->Fill(phi[it]);
				hpt_killed		->Fill(pt[it]);
				hntpc_killed	->Fill(ntpc[it]);
				hphieta_killed	->Fill(eta[it],phi[it]);
				hdcaxy_killed	->Fill(dcaxy[it]);
				hdcaz_killed	->Fill(dcaz[it]);
				hquality_killed	->Fill(quality[it]);
				continue;
			}
			++ntrkept;
			if (chg[it]  >   0.){ npos+=1.; }
			if (chg[it]  <   0.){ nneg+=1.; }
			//
			//---- assign PID...
			int thisParticleID	= -1;
			int thisSpecies		= -1;
			int thisindV0		= -1;
			if (chg[it]>0 && dedx70s[it]<400. ){ 			// Pi+
				thisParticleID	= kParticleIDPionPlus;
				thisSpecies		= 0;
			} else if (chg[it]<0 && dedx70s[it]<400. ){ 	// Pi-
				thisParticleID = kParticleIDPionMinus;
				thisSpecies		= 0;
			}
			thisindV0			= indv0[it];
			//
			//------------------------------
			//
			hrirun_eta		->Fill((*run),eta[it]);
			hrirun_phi		->Fill((*run),phi[it]);
			hrirun_pt		->Fill((*run),pt[it]);
			hrirun_ntpc		->Fill((*run),ntpc[it]);
			hrirun_dedx		->Fill((*run),dedx70s[it]);
			hrirun_ndedx	->Fill((*run),dedx70n[it]);
			//
				//hri_eta[0]		->Fill(kRunIndex,eta[it]);
				//hri_phi[0]		->Fill(kRunIndex,phi[it]);
				//hri_pt[0]		->Fill(kRunIndex,pt[it]);
				//hri_ntpc[0]		->Fill(kRunIndex,ntpc[it]);
				//hri_dedx[0]		->Fill(kRunIndex,dedx70s[it]);
				//hri_ndedx[0]	->Fill(kRunIndex,dedx70n[it]);
			if (goodClock){
				//hri_eta[1]		->Fill(kRunIndex,eta[it]);
				//hri_phi[1]		->Fill(kRunIndex,phi[it]);
				//hri_pt[1]		->Fill(kRunIndex,pt[it]);
				//hri_ntpc[1]		->Fill(kRunIndex,ntpc[it]);
				//hri_dedx[1]		->Fill(kRunIndex,dedx70s[it]);
				//hri_ndedx[1]	->Fill(kRunIndex,dedx70n[it]);
			}
			if (goodMB){
				//hri_eta[2]		->Fill(kRunIndex,eta[it]);
				//hri_phi[2]		->Fill(kRunIndex,phi[it]);
				//hri_pt[2]		->Fill(kRunIndex,pt[it]);
				//hri_ntpc[2]		->Fill(kRunIndex,ntpc[it]);
				//hri_dedx[2]		->Fill(kRunIndex,dedx70s[it]);
				//hri_ndedx[2]	->Fill(kRunIndex,dedx70n[it]);
			}
			if (goodJet){
				//hri_eta[3]		->Fill(kRunIndex,eta[it]);
				//hri_phi[3]		->Fill(kRunIndex,phi[it]);
				//hri_pt[3]		->Fill(kRunIndex,pt[it]);
				//hri_ntpc[3]		->Fill(kRunIndex,ntpc[it]);
				//hri_dedx[3]		->Fill(kRunIndex,dedx70s[it]);
				//hri_ndedx[3]	->Fill(kRunIndex,dedx70n[it]);
			}
			if (goodPho){
				//hri_eta[4]		->Fill(kRunIndex,eta[it]);
				//hri_phi[4]		->Fill(kRunIndex,phi[it]);
				//hri_pt[4]		->Fill(kRunIndex,pt[it]);
				//hri_ntpc[4]		->Fill(kRunIndex,ntpc[it]);
				//hri_dedx[4]		->Fill(kRunIndex,dedx70s[it]);
				//hri_ndedx[4]	->Fill(kRunIndex,dedx70n[it]);
			}
			//
//			hntpc		->Fill(ntpc[it]+nmvtx[it]+nintt[it]);
			hntpc		->Fill(ntpc[it]);
			hnmvtx		->Fill(nmvtx[it]);
			hnintt		->Fill(nintt[it]);
			heta		->Fill(eta[it]);
			hphi		->Fill(phi[it]);
			hpt			->Fill(pt[it]);
			hphieta			->Fill(eta[it],phi[it],1.);
			hntpc_phieta	->Fill(eta[it],phi[it],      ntpc[it] );
			habsdcaxy_phieta->Fill(eta[it],phi[it],fabs(dcaxy[it]));
			habsdcaz_phieta	->Fill(eta[it],phi[it], fabs(dcaz[it]));
			hdedx_phieta	->Fill(eta[it],phi[it],   dedx70s[it] );
			//
			int kch=0; if(chg[it]<0){ kch=1; }
			if (thisindV0>=0){				// is this track a v0 daughter?
				int kv0=0;
				if (v0pid[thisindV0]==  310 ) kv0 = 0;
				if (v0pid[thisindV0]==  3122) kv0 = 1;
				if (v0pid[thisindV0]== -3122) kv0 = 2;
				if (chg[it]         <   0   ) kch = 1;		// pos=0, neg=1
				double fillpt	= pt[it]; if (fillpt>4.99){ fillpt = 4.99; }
								  hpt_tagged[kch][kv0]	->Fill(fillpt);
				               hdedxp_tagged[kch][kv0]	->Fill(ptot[it],dedx70s[it]);
			}
			//
			if (nmvtx[it]>0) heta_mvt	->Fill(eta[it]);
			if (nintt[it]>0) heta_int	->Fill(eta[it]);
			if (nmvtx[it]>0&&nintt[it]>0) heta_mvtint	->Fill(eta[it]);
			hpteta		->Fill(eta[it],pt[it]);
			hptphi		->Fill(phi[it],pt[it]);
			hdedxp		->Fill(ptot[it],dedx70s[it]);
			hdedxpz		->Fill(ptot[it],dedx70s[it]);
			hdcaxy		->Fill(dcaxy[it]);
			hdcaz		->Fill(dcaz[it]);
			hdcaxyz		->Fill(dcaz[it],dcaxy[it]);
			hquality	->Fill(quality[it]);
			hdedx		->Fill(dedx70s[it]);
			hndedx		->Fill(dedx70n[it]);
			hdedxphi	->Fill(phi[it],dedx70s[it]);
			if (eta[it]>=0) hdedxphi_etap->Fill(phi[it],dedx70s[it]);
			if (eta[it]< 0) hdedxphi_etan->Fill(phi[it],dedx70s[it]);
			//
			hetazvtx						->Fill((*vtxz),eta[it]);
			if (pt[it]>0.5) hetazvtx_pta	->Fill((*vtxz),eta[it]);
			if (pt[it]>1.0) hetazvtx_ptb	->Fill((*vtxz),eta[it]);
			//
			//----- save for the class....
			//
			if (thisParticleID<0) continue;
			//
			double thiseta,thisphi,thispt;
			if (!USESEEDTRACKS){
				thiseta	= eta[it];
				thisphi	= phi[it];
				thispt	=  pt[it];
			} else if (USESEEDTRACKS){
				thiseta	= seedeta[it];
				thisphi	= seedphi[it];
				thispt	=  seedpt[it];
			}
			//---- add this track to all PairType indices that this species is included in...
			if (!NOCORRELATIONS){
				for (int ipaty=0;ipaty<NPairTypes;ipaty++){
					//
					//---- protect against keeping a track for correlations that is a daughter of a V0 IN THIS SPECIFIC PAIR
					//		keep track if not known as a V0 daughter
					//		COULD keep track if a known daughter of V0, but that V0 is NOT part of this pair
					int kIndexToV0	= indv0[it];
					if ( kIndexToV0 >= 0 ){
						continue;
					}
					//
					//---- check if this track is particle 1 for this pair type
					bool accept1	= false;
					if (thisParticleID==PairTypes_Info[ipaty][0]) accept1 = true;
					if (accept1){
						if (thispt < Species_ptmin[thisSpecies]) accept1	= false;
						if (thispt >=Species_ptmax[thisSpecies]) accept1	= false;
						if (thiseta< thisYL ) accept1	= false;
						if (thiseta>=thisYU ) accept1	= false;
					}
					//---- check if this track is particle 2 for this pair type
					bool accept2	= false;
					if (thisParticleID==PairTypes_Info[ipaty][1]) accept2 = true;
					if (accept2){
						if (thispt < Species_ptmin[thisSpecies]) accept2	= false;
						if (thispt >=Species_ptmax[thisSpecies]) accept2	= false;
						if (thiseta< thisYL ) accept2	= false;
						if (thiseta>=thisYU ) accept2	= false;
					}
					if (accept1){		// this track is "Particle 1" in this PairType...
						int kk = calcr_n_1[ipaty];
						if (kk== MAX_CALCR_N-1){ 
							cout<<"reader::Loop -- PVEC1 FULL .. max="<<MAX_CALCR_N<<"\t ipaty="<<ipaty<<endl; 
							exit(0);
						}
						Pvec_1[ipaty][kk][0]	= thiseta;
						Pvec_1[ipaty][kk][1]	= thisphi;
						Pvec_1[ipaty][kk][2]	= thispt;
						++calcr_n_1[ipaty];
					} 	// end accept1
					if (accept2){		// this track is "Particle 2" in this PairType...
						int kk = calcr_n_2[ipaty];
						if (kk== MAX_CALCR_N-1){
							cout<<"reader::Loop -- PVEC2 FULL .. max="<<MAX_CALCR_N<<"\t ipaty="<<ipaty<<endl;
							exit(0);
						}
						Pvec_2[ipaty][kk][0]	= thiseta;
						Pvec_2[ipaty][kk][1]	= thisphi;
						Pvec_2[ipaty][kk][2]	= thispt;
						++calcr_n_2[ipaty];
					} 	// end accept2
				}	// end pairtypes loop
			}	// end nocorrelations
			//
		}	// end track loop
		//
		//---- collect uncharged particles for class from V0 tree
		bool SHOWV0	= false;
		int nks=0,nla=0,nal=0;
		for (int iv0=0;iv0<(*nv0);iv0++){
			if (v0pid[iv0] ==   310){ ++nks; } else 
			if (v0pid[iv0] ==  3122){ ++nla; } else 
			if (v0pid[iv0] == -3122){ ++nal; }
		}
		if (nks) hKSxing->Fill((*crossing));		// this (*crossing) has a KS
		if (nla) hLAxing->Fill((*crossing));		// this (*crossing) has a LA
		if (nal) hALxing->Fill((*crossing));		// this (*crossing) has a AL
		//if ((*nv0)>1) SHOWV0 = true;	// 2026-09-24: multi-V0 row printout silenced (user request)
		if ((*nv0)>0){	counters[0] += 1; counters[1] += (*nv0); }		// count Total Number of Events w/ a V0, and Total Number of V0s
		if ((*nv0)>1){	counters[2] += 1; }							// count Total Number of Events w/ TWO or more V0s
		for (int iv0=0;iv0<(*nv0);iv0++){
			int thisParticleID	= -1;
			int thisSpecies		= -1;
			double thiseta	= v0eta[iv0];
			double thisphi	= v0phi[iv0];
			double thispt	=  v0pt[iv0];
			double thismass	= v0mass[iv0];
			//---- README_v0etaSpike.md: v0eta/v0phi/v0mass/v0ctau are sometimes an exact-0 sentinel
			//---- (likely KFParticle getter whose error calc failed); px,py,pz,ene are always good.
			if (v0eta[iv0]==0.0){	thiseta	= asinh(v0pz[iv0]/v0pt[iv0]);	++nv0fixEta;	}
			if (v0phi[iv0]==0.0){	thisphi	= atan2(v0py[iv0],v0px[iv0]);	++nv0fixPhi;	}
			if (v0mass[iv0]==0.0){
				double m2	= v0ene[iv0]*v0ene[iv0] - v0ptot[iv0]*v0ptot[iv0];
				thismass	= (m2>0.) ? sqrt(m2) : 0.;
				++nv0fixMass;
			}
			double thisctau	= v0ctau[iv0];
			if (v0ctau[iv0]==0.0){	thisctau	= v0decaylen[iv0]*thismass/v0ptot[iv0];	++nv0fixCtau;	}	// ctau = L*m/p
			double worseDCA	= WorseDaughterPVDCA(dcaxy,dcaz,v0indtr1[iv0],v0indtr2[iv0]);
			if (v0pid[iv0] == 310){
				thisParticleID	= kParticleIDKshort;
				thisSpecies		= 5;
				++counters[3];
				hKSmass_xing	->Fill((*crossing),thismass);
				hKSmass			->Fill(thismass);
				hKSpt			->Fill(v0pt[iv0]);
				hKSeta			->Fill(thiseta);
				hKSphi			->Fill(thisphi);
				hKSdira			->Fill(v0dira[iv0]);
				hKSpvdca		->Fill(v0pvdca[iv0]);
				hKSworsedca		->Fill(worseDCA);
			} else if (v0pid[iv0] == 3122){
				thisParticleID	= kParticleIDLambda;
				thisSpecies		= 6;
				++counters[4];
				hLAmass_xing	->Fill((*crossing),thismass);
				hLAmass			->Fill(thismass);
				hLApt			->Fill(v0pt[iv0]);
				hLAeta			->Fill(thiseta);
				hLAphi			->Fill(thisphi);
				hLAdira			->Fill(v0dira[iv0]);
				hLApvdca		->Fill(v0pvdca[iv0]);
				hLAworsedca		->Fill(worseDCA);
				if (worseDCA>=LA_WORSEDCA_CUT) hLAmass_clean->Fill(thismass);
			} else if (v0pid[iv0] == -3122){
				thisParticleID	= kParticleIDAntiLambda;
				thisSpecies		= 7;
				++counters[5];
				hALmass_xing	->Fill((*crossing),thismass);
				hALmass			->Fill(thismass);
				hALpt			->Fill(v0pt[iv0]);
				hALeta			->Fill(thiseta);
				hALphi			->Fill(thisphi);
				hALdira			->Fill(v0dira[iv0]);
				hALpvdca		->Fill(v0pvdca[iv0]);
				hALworsedca		->Fill(worseDCA);
				if (worseDCA>=AL_WORSEDCA_CUT) hALmass_clean->Fill(thismass);
			}
			//
			if (SHOWV0){
				cout<<iv0<<"\t pid= "<<v0pid[iv0]<<", m= "<<thismass
					<<"\t eta= "<<thiseta<<" phi= "<<thisphi<<" pt= "<<v0pt[iv0]
					<<" ctau= "<<thisctau
					<<"\t trkID: "<<v0indtr1[iv0]<<" "<<v0indtr2[iv0]
					<<"\t dedx: "<<dedx70s[v0indtr1[iv0]]<<" "<<dedx70s[v0indtr2[iv0]]
					<<endl;
			}
			//
			//---- add this V0 to all PairType indices that this species is included in...
			if (!NOCORRELATIONS){
				for (int ipaty=0;ipaty<NPairTypes;ipaty++){	
					//---- check if this track is particle 1 for this pair type
					bool accept1	= false;
					if (thisParticleID==PairTypes_Info[ipaty][0]) accept1 = true;
					if (accept1){
						if (thispt < Species_ptmin[thisSpecies]) accept1	= false;
						if (thispt >=Species_ptmax[thisSpecies]) accept1	= false;
						if (thiseta< thisYL ) accept1	= false;
						if (thiseta>=thisYU ) accept1	= false;
					}
					//---- check if this track is particle 2 for this pair type
					bool accept2	= false;
					if (thisParticleID==PairTypes_Info[ipaty][1]) accept2 = true;
					if (accept2){
						if (thispt < Species_ptmin[thisSpecies]) accept2	= false;
						if (thispt >=Species_ptmax[thisSpecies]) accept2	= false;
						if (thiseta< thisYL ) accept2	= false;
						if (thiseta>=thisYU ) accept2	= false;
					}
					if (accept1){		// this track is "Particle 1" in this PairType...
						int kk = calcr_n_1[ipaty];
						if (kk== MAX_CALCR_N-1){ 
							cout<<"reader::Loop -- PVEC1 FULL .. max="<<MAX_CALCR_N<<"\t ipaty="<<ipaty<<endl; 
							exit(0);
						}
						//cout<<"Incrementing 1... ipaty="<<ipaty<<"  k="<<kk<<"  n1="<<calcr_n_1[ipaty]<<" \t "<<ety<<endl;
						Pvec_1[ipaty][kk][0]	= thiseta;
						Pvec_1[ipaty][kk][1]	= thisphi;
						Pvec_1[ipaty][kk][2]	= thispt;
						++calcr_n_1[ipaty];
					}
					if (accept2){		// this track is "Particle 2" in this PairType...
						int kk = calcr_n_2[ipaty];
						if (kk== MAX_CALCR_N-1){
							cout<<"reader::Loop -- PVEC2 FULL .. max="<<MAX_CALCR_N<<"\t ipaty="<<ipaty<<endl;
							exit(0);
						}
						//cout<<"Incrementing 2... ipaty="<<ipaty<<"  k="<<kk<<"  n2="<<calcr_n_2[ipaty]<<endl;
						Pvec_2[ipaty][kk][0]	= thiseta;
						Pvec_2[ipaty][kk][1]	= thisphi;
						Pvec_2[ipaty][kk][2]	= thispt;
						++calcr_n_2[ipaty];
					}
					//
				}	// end pairtypes loop
			}	// end nocorrelations
			//
		}	// end v0 loop
		//
		//---- fill some event variable histograms...
		hntrk			->Fill(ntrkept);
		double negfr	= -1;
		if (npos+nneg>0.){ negfr = nneg/(npos+nneg); }
		hrirun_ntrk		->Fill((*run),(*ntr) );
		hrirun_vtxx		->Fill((*run),(*vtxx));
		hrirun_vtxy		->Fill((*run),(*vtxy));
		hrirun_vtxz		->Fill((*run),(*vtxz));
		hrirun_negfr	->Fill((*run),negfr);
		hrirun_nev		->Fill((*run),1.0);
		hrirun_nevmb	->Fill((*run),(int)goodMB );
		hrirun_nevjet	->Fill((*run),(int)goodJet);
		hrirun_nevpho	->Fill((*run),(int)goodPho);
		//
			//hri_ntrk[0]	->Fill(kRunIndex,(*ntr) );
			//hri_vtxx[0]	->Fill(kRunIndex,(*vtxx));
			//hri_vtxy[0]	->Fill(kRunIndex,(*vtxy));
			//hri_vtxz[0]	->Fill(kRunIndex,(*vtxz));
			//hri_negfr[0]->Fill(kRunIndex,negfr);
			//hri_nev[0]	->Fill(kRunIndex,1.0);
			//hri_nevmb[0]->Fill(kRunIndex,(int)goodMB );
			//hri_nevjet[0]->Fill(kRunIndex,(int)goodJet);
			//hri_nevpho[0]->Fill(kRunIndex,(int)goodPho);
		if (goodClock){
			//hri_ntrk[1]	->Fill(kRunIndex,(*ntr) );
			//hri_vtxx[1]	->Fill(kRunIndex,(*vtxx));
			//hri_vtxy[1]	->Fill(kRunIndex,(*vtxy));
			//hri_vtxz[1]	->Fill(kRunIndex,(*vtxz));
			//hri_negfr[1]->Fill(kRunIndex,negfr);
			//hri_nev[1]	->Fill(kRunIndex,1.0);
			//hri_nevmb[1]->Fill(kRunIndex,(int)goodMB );
			//hri_nevjet[1]->Fill(kRunIndex,(int)goodJet);
			//hri_nevpho[1]->Fill(kRunIndex,(int)goodPho);
		}
		if (goodMB){
			//hri_ntrk[2]	->Fill(kRunIndex,(*ntr) );
			//hri_vtxx[2]	->Fill(kRunIndex,(*vtxx));
			//hri_vtxy[2]	->Fill(kRunIndex,(*vtxy));
			//hri_vtxz[2]	->Fill(kRunIndex,(*vtxz));
			//hri_negfr[2]->Fill(kRunIndex,negfr);
			//hri_nev[2]	->Fill(kRunIndex,1.0);
			//hri_nevmb[2]->Fill(kRunIndex,(int)goodMB );
			//hri_nevjet[2]->Fill(kRunIndex,(int)goodJet);
			//hri_nevpho[2]->Fill(kRunIndex,(int)goodPho);
		}
		if (goodJet){
			//hri_ntrk[3]	->Fill(kRunIndex,(*ntr) );
			//hri_vtxx[3]	->Fill(kRunIndex,(*vtxx));
			//hri_vtxy[3]	->Fill(kRunIndex,(*vtxy));
			//hri_vtxz[3]	->Fill(kRunIndex,(*vtxz));
			//hri_negfr[3]->Fill(kRunIndex,negfr);
			//hri_nev[3]	->Fill(kRunIndex,1.0);
			//hri_nevmb[3]->Fill(kRunIndex,(int)goodMB );
			//hri_nevjet[3]->Fill(kRunIndex,(int)goodJet);
			//hri_nevpho[3]->Fill(kRunIndex,(int)goodPho);
		}
		if (goodPho){
			//hri_ntrk[4]	->Fill(kRunIndex,(*ntr) );
			//hri_vtxx[4]	->Fill(kRunIndex,(*vtxx));
			//hri_vtxy[4]	->Fill(kRunIndex,(*vtxy));
			//hri_vtxz[4]	->Fill(kRunIndex,(*vtxz));
			//hri_negfr[4]->Fill(kRunIndex,negfr);
			//hri_nev[4]	->Fill(kRunIndex,1.0);
			//hri_nevmb[4]->Fill(kRunIndex,(int)goodMB );
			//hri_nevjet[4]->Fill(kRunIndex,(int)goodJet);
			//hri_nevpho[4]->Fill(kRunIndex,(int)goodPho);
		}
		//
		//cout<<"----------------------------------"<<endl;
		//cout<<npart1<<" "<<npart2<<endl;
		//for (int ip=0;ip<npart1;ip++){
		//	cout<<"POS \t"<<ip<<"\t "<<part1[ip][0]<<" "<<part1[ip][1]<<" "<<part1[ip][2]<<endl;
		//}
		//for (int in=0;in<npart2;in++){
		//	cout<<"NEG \t"<<in<<"\t "<<part2[in][0]<<" "<<part2[in][1]<<" "<<part2[in][2]<<endl;
		//}
		//
		//
		if (!NOCORRELATIONS){
			for (int ipaty=0;ipaty<NPairTypes;ipaty++){	
				int ipid1			= PairTypes_Info[ipaty][0];
				int ipid2			= PairTypes_Info[ipaty][1];
				//int iSpecies1		= ipid1%NSpecies;
				//int iSpecies2		= ipid2%NSpecies;
				//
				int calcr_n_1_call	= calcr_n_1[ipaty];
				bool abort_zeroeff	= false;			// set to true if any particle1 OR particle2 has zero efficiency from eff map
				for (int i=0;i<calcr_n_1_call;i++){ 
					eff_1[i]=1.0;
					double yloc			= Pvec_1[ipaty][i][0];				// rapidity
					double ptloc		= Pvec_1[ipaty][i][2];				// pt
					if (!WeightDensities){
						eff_1[i]	= 1.;
					} else {
						//int kbin	= heffpty_cent[ipid1][kCentrality16]->FindBin(yloc,min(ptloc,MaxPtWithEff-0.05));
						//eff_1[i]	= heffpty_cent[ipid1][kCentrality16]->GetBinContent(kbin);
						eff_1[i]	= 1.;
					}
					if (eff_1[i]<=0.){
						abort_zeroeff	= true;
					}
					//
					for(int k=0;k<3;k++){ Pvec_1_call[i][k] = Pvec_1[ipaty][i][k]; }
				}
				int calcr_n_2_call	= calcr_n_2[ipaty];
				for (int i=0;i<calcr_n_2_call;i++){ 
					eff_2[i]=1.0;
					double yloc			= Pvec_2[ipaty][i][0];				// rapidity
					double ptloc		= Pvec_2[ipaty][i][2];				// pt
					if (!WeightDensities){
						eff_2[i]	= 1.;
					} else {
						//int kbin	= heffpty_cent[ipid2][kCentrality16]->FindBin(yloc,min(ptloc,MaxPtWithEff-0.05));
						//eff_2[i]	= heffpty_cent[ipid2][kCentrality16]->GetBinContent(kbin);
						eff_2[i]	= 1.;
					}
					if (eff_2[i]<=0.){
						abort_zeroeff	= true;
					}
					//
					for(int k=0;k<3;k++){ Pvec_2_call[i][k] = Pvec_2[ipaty][i][k]; }
				}
				//
	// 			++nseenzeroeff[ipaty];
	// 			if (abort_zeroeff){			// skip this event if a particle has zero efficiency in map
	// 				++nabortedzeroeff[ipaty];
	// 				continue;
	// 			}
				//
					R[ipaty]	->SetCurrentRunEvt((*run),(*evt));	// sec 18.20/18.23: (run,evt) uniquely IDs the
																	// trigger frame -- used by the mixNoAdjTF
																	// neighboring-TF event-pair exclusion
				R[ipaty]	->Increment((*vtxz),field,
										calcr_n_1_call,eff_1,Pvec_1_call,
										calcr_n_2_call,eff_2,Pvec_2_call);
				//		
			}	// end ipaty loop...
		}	// end nocorrelations
		//
	}	// end event loop...

	//---- final calculations...
	//
	TH1::AddDirectory(kTRUE);		// needed for clones in following calculations
	//
	cout<<"Run numbers seen = "<<run_lowest<<" - "<<run_highest<<endl;
	cout<<"V0 zero-sentinel fixes (README_v0etaSpike.md): eta "<<nv0fixEta<<"  phi "<<nv0fixPhi<<"  mass "<<nv0fixMass<<"  ctau "<<nv0fixCtau<<endl;
	htrigRun		->GetXaxis()->SetRangeUser(run_lowest-10.5,run_highest+10.5);
	hrirun_ntrk		->GetXaxis()->SetRangeUser(run_lowest-10.5,run_highest+10.5);
	hrirun_vtxx		->GetXaxis()->SetRangeUser(run_lowest-10.5,run_highest+10.5);
	hrirun_vtxy		->GetXaxis()->SetRangeUser(run_lowest-10.5,run_highest+10.5);
	hrirun_vtxz		->GetXaxis()->SetRangeUser(run_lowest-10.5,run_highest+10.5);
	hrirun_negfr	->GetXaxis()->SetRangeUser(run_lowest-10.5,run_highest+10.5);
	hrirun_nev		->GetXaxis()->SetRangeUser(run_lowest-10.5,run_highest+10.5);
	hrirun_nevmb	->GetXaxis()->SetRangeUser(run_lowest-10.5,run_highest+10.5);
	hrirun_nevjet	->GetXaxis()->SetRangeUser(run_lowest-10.5,run_highest+10.5);
	hrirun_nevpho	->GetXaxis()->SetRangeUser(run_lowest-10.5,run_highest+10.5);
	hrirun_eta		->GetXaxis()->SetRangeUser(run_lowest-10.5,run_highest+10.5);
	hrirun_phi		->GetXaxis()->SetRangeUser(run_lowest-10.5,run_highest+10.5);
	hrirun_pt		->GetXaxis()->SetRangeUser(run_lowest-10.5,run_highest+10.5);
	hrirun_ntpc		->GetXaxis()->SetRangeUser(run_lowest-10.5,run_highest+10.5);
	hrirun_dedx		->GetXaxis()->SetRangeUser(run_lowest-10.5,run_highest+10.5);
	hrirun_ndedx	->GetXaxis()->SetRangeUser(run_lowest-10.5,run_highest+10.5);
	//
	hrirun_nevmb	->Divide(hrirun_nev);
	hrirun_nevjet	->Divide(hrirun_nev);
	hrirun_nevpho	->Divide(hrirun_nev);
	for (int itt=0;itt<NTRIGTYPES;itt++){	
		//hri_nevmb[itt]	->Divide(hri_nev[itt]);
		//hri_nevjet[itt]	->Divide(hri_nev[itt]);
		//hri_nevpho[itt]	->Divide(hri_nev[itt]);
	}
	//
	hntpc_phieta	->Divide(hphieta);
	habsdcaxy_phieta->Divide(hphieta);
	habsdcaz_phieta	->Divide(hphieta);
	hdedx_phieta	->Divide(hphieta);

	//---- v0 vs (*crossing)
	TH1D *hKSprob_xing	= (TH1D*)hKSxing->Clone(Form("hKSprob_xing"));
	TH1D *hLAprob_xing	= (TH1D*)hLAxing->Clone(Form("hLAprob_xing"));
	TH1D *hALprob_xing	= (TH1D*)hALxing->Clone(Form("hALprob_xing"));
	hKSprob_xing->Divide(hxing);
	hLAprob_xing->Divide(hxing);
	hALprob_xing->Divide(hxing);

	//---- find pt bins for dedx vs eta,phi	
	for (int ipar=0;ipar<3;ipar++){
		for (int ichg=0;ichg<2;ichg++){
			cout<<"------ ipar= "<<ipar<<" ichg="<<ichg<<" ----------- "<<endl;
			TH1D *htemp	= (TH1D*)hpt_tagged[ichg][ipar]->Clone("htemp");
			htemp		->Scale(1./htemp->Integral());
			int ptbin	= 0;
			double sum	= 0;
			for (int ibin=1;ibin<=htemp->GetNbinsX();ibin++){
				double apt	= htemp->GetBinCenter (ibin);
				double anc	= htemp->GetBinContent(ibin);
				sum	+= anc;
				if (sum>0.2*(ptbin+1)){
					cout<<ibin<<"\t "<<ptbin<<" "<<apt<<"\t "<<anc<<" "<<sum<<"\t BOUNDRY"<<endl;
					ptbin+=1;
				}
			}
			delete htemp;
		}
	}


	//---- class hists...
	TH1D *hzvtx[NPairTypes]					= {0};
	TH2D *hmult[NPairTypes]					= {0};
	TH1D *hmult1[NPairTypes]				= {0};
	TH1D *hmult2[NPairTypes]				= {0};
	TH1D *hy1[NPairTypes]					= {0};
	TH1D *hy2[NPairTypes]					= {0};
	TH2D *hrho2[NR2TYPES][NPairTypes]		= {0};		// uncorrected 
	TH2D *hrho1rho1[NR2TYPES][NPairTypes]	= {0};		// uncorrected 
	TH2D *hC2[NR2TYPES][NPairTypes]			= {0};		// uncorrected C2
	TH2D *hR2[NR2TYPES][NPairTypes]			= {0};		// uncorrected R2
	TH2D *hrho2C[NPairTypes]				= {0};		// README_Crossing: crossing-corrected (dy,dphi) rho2(S)
	TH2D *hC2C[NPairTypes]					= {0};		// crossing-corrected (dy,dphi) C2
	TH2D *hR2C[NPairTypes]					= {0};		// crossing-corrected (dy,dphi) R2
	TH2D *hMempty[NPairTypes]				= {0};		// # of used Zvtx slices with rho2(M)=0, per (dy,dphi) bin
	TH1D *hR2yydy[NPairTypes]				= {0};		// 
	TH1D *hR2dy[NPairTypes]					= {0};		// 
	TH1D *hR2dphi[NPairTypes]				= {0};		// 
	TH1D *hMinv_S[NPairTypes]				= {0};
	TH1D *hMinv_M[NPairTypes]				= {0};
	TH1D *hMinv[NPairTypes]					= {0};
	TH1D *hCQ[NPairTypes]					= {0};		// femtoscopic C(Q)=Qsib/Qmix, Zvtx-averaged (CalcRm only)
	TH1D *hQsib[NPairTypes]					= {0};		// raw sibling numerator behind hCQ -- not written before 2026-09-21 (sec 13.8.5)
	TH2D *hzoomS[NPairTypes]				= {0};		// README sec 16.8 spike-extent zoom (sibling)
	TH2D *hzoomM[NPairTypes]				= {0};		// README sec 16.8 spike-extent zoom (mixed)
	TH1D *hMinvFS[NPairTypes]				= {0};		// fine Minv near threshold, sibling (C(Q) page)
	TH1D *hMinvFM[NPairTypes]				= {0};		// same, mixed
	TH1D *hQmix[NPairTypes]					= {0};		// raw mixed denominator behind hCQ -- not written before 2026-09-21 (sec 13.8.5)
	TH2D *hCQKT[NPairTypes]					= {0};		// README_CQ: C(Q) vs (Qinv,kT), STAR's 4 kT bins, Zvtx-averaged
	TH2D *hQsibKT[NPairTypes]				= {0};		// its sibling numerator, Zvtx-summed
	TH2D *hQmixKT[NPairTypes]				= {0};		// its mixed denominator, Zvtx-summed
	if (!NOCORRELATIONS){
		TH1::AddDirectory(kFALSE);
		for (int ipaty=0;ipaty<NPairTypes;ipaty++){
			int ipid1			= PairTypes_Info[ipaty][0];
			int ipid2			= PairTypes_Info[ipaty][1];
			//int kCentralityStyle	= PairTypes_Info[ipaty][2];
			int iSpecies1		= GetSpecies(ipid1);
			int iSpecies2		= GetSpecies(ipid2);
			int iChg1			= ParticleCharge[ipid1];
			int iChg2			= ParticleCharge[ipid2];
			TString part1name	= TString(ParticleIDNames[ipid1]);
			TString part2name	= TString(ParticleIDNames[ipid2]);
			//
			R[ipaty]->Calculate();
			//
			hzvtx[ipaty]	= (TH1D*)R[ipaty]->Gethzvtx();
			hzvtx[ipaty]	->SetName(Form("hzvtx_%d",ipaty));
			hmult[ipaty]	= (TH2D*)R[ipaty]->Gethmult();
			hmult[ipaty]	->SetTitle(Form("%s%s, N/evt;n_{1};n_{2}",part1name.Data(),part2name.Data()));
			hmult[ipaty]	->SetName(Form("hmult_%d",ipaty));
			hmult1[ipaty]	= (TH1D*)hmult[ipaty]->ProjectionX();
			hmult1[ipaty]	->SetTitle(Form("%s, N/evt;n_{1}",part1name.Data()));
			hmult1[ipaty]	->SetName(Form("hmult1_%d",ipaty));
			hmult2[ipaty]	= (TH1D*)hmult[ipaty]->ProjectionY();
			hmult2[ipaty]	->SetTitle(Form("%s, N/evt;n_{2}",part2name.Data()));
			hmult2[ipaty]	->SetName(Form("hmult2_%d",ipaty));
			//
			hy1[ipaty]		= (TH1D*)R[ipaty]->Gethy1_();
			hy1[ipaty]		->SetTitle(Form("%s%s, dN1/dy/evt;dy",part1name.Data(),part2name.Data()));
			hy1[ipaty]		->SetName(Form("hy1_%d",ipaty));
			hy2[ipaty]		= (TH1D*)R[ipaty]->Gethy2_();
			hy2[ipaty]		->SetTitle(Form("%s%s, dN2/dy/evt;dy",part1name.Data(),part2name.Data()));
			hy2[ipaty]		->SetName(Form("hy2_%d",ipaty));
			hMinv_S[ipaty]	= (TH1D*)R[ipaty]->GethMinv_S();
			hMinv_S[ipaty]	->SetTitle(Form("%s%s, M_{inv}(S)",part1name.Data(),part2name.Data()));
			hMinv_S[ipaty]	->SetName(Form("hMinv_S_%d",ipaty));
			hMinv_M[ipaty]	= (TH1D*)R[ipaty]->GethMinv_M();
			hMinv_M[ipaty]	->SetTitle(Form("%s%s, M_{inv}(M)",part1name.Data(),part2name.Data()));
			hMinv_M[ipaty]	->SetName(Form("hMinv_M_%d",ipaty));
			hMinv[ipaty]	= (TH1D*)R[ipaty]->GethMinv();
			hMinv[ipaty]	->SetTitle(Form("%s%s, M_{inv}",part1name.Data(),part2name.Data()));
			hMinv[ipaty]	->SetName(Form("hMinv_%d",ipaty));
			hCQ[ipaty]		= (TH1D*)R[ipaty]->GethCQ();
			hCQ[ipaty]		->SetTitle(Form("%s%s, C(Q)=Q_{sib}/Q_{mix};Q_{inv} (GeV)",part1name.Data(),part2name.Data()));
			hCQ[ipaty]		->SetName(Form("hCQ_%d",ipaty));
			hQsib[ipaty]	= (TH1D*)R[ipaty]->GethQsib();
			hQsib[ipaty]	->SetTitle(Form("%s%s, Q_{sib} (raw sibling pairs);Q_{inv} (GeV)",part1name.Data(),part2name.Data()));
			hQsib[ipaty]	->SetName(Form("hQsib_%d",ipaty));
			hQmix[ipaty]	= (TH1D*)R[ipaty]->GethQmix();
			hQmix[ipaty]	->SetTitle(Form("%s%s, Q_{mix} (raw mixed pairs);Q_{inv} (GeV)",part1name.Data(),part2name.Data()));
			hQmix[ipaty]	->SetName(Form("hQmix_%d",ipaty));
			hCQKT[ipaty]	= (TH2D*)R[ipaty]->GethCQKT();
			hCQKT[ipaty]	->SetTitle(Form("%s%s, C(Q) vs k_{T};Q_{inv} (GeV);k_{T} (GeV)",part1name.Data(),part2name.Data()));
			hCQKT[ipaty]	->SetName(Form("hCQKT_%d",ipaty));
			hQsibKT[ipaty]	= (TH2D*)R[ipaty]->GethQsibKT();
			hQsibKT[ipaty]	->SetTitle(Form("%s%s, Q_{sib} vs k_{T} (raw sibling pairs);Q_{inv} (GeV);k_{T} (GeV)",part1name.Data(),part2name.Data()));
			hQsibKT[ipaty]	->SetName(Form("hQsibKT_%d",ipaty));
			hQmixKT[ipaty]	= (TH2D*)R[ipaty]->GethQmixKT();
			hQmixKT[ipaty]	->SetTitle(Form("%s%s, Q_{mix} vs k_{T} (raw mixed pairs);Q_{inv} (GeV);k_{T} (GeV)",part1name.Data(),part2name.Data()));
			hQmixKT[ipaty]	->SetName(Form("hQmixKT_%d",ipaty));
			hzoomS[ipaty]	= (TH2D*)R[ipaty]->GethZoomS();
			hzoomS[ipaty]	->SetName(Form("hzoomS_%d",ipaty));
			hzoomS[ipaty]	->SetTitle(Form("%s%s, sibling pairs, zoom;#Delta#eta;#Delta#phi (deg)",part1name.Data(),part2name.Data()));
			hrho2C[ipaty]	= (TH2D*)R[ipaty]->Gethrho2C_S();
			hrho2C[ipaty]	->SetName(Form("hrho2C_1_%d",ipaty));
			hrho2C[ipaty]	->SetTitle(Form("%s%s, #rho_{2}(S) crossing-corrected vs. (dy,d#phi);dy;d#phi",part1name.Data(),part2name.Data()));
			hC2C[ipaty]		= (TH2D*)R[ipaty]->GethC2C();
			hC2C[ipaty]		->SetName(Form("hC2C_1_%d",ipaty));
			hC2C[ipaty]		->SetTitle(Form("%s%s, C_{2} crossing-corrected vs. (dy,d#phi);dy;d#phi",part1name.Data(),part2name.Data()));
			hR2C[ipaty]		= (TH2D*)R[ipaty]->GethR2C();
			hR2C[ipaty]		->SetName(Form("hR2C_1_%d",ipaty));
			hR2C[ipaty]		->SetTitle(Form("%s%s, R_{2} crossing-corrected vs. (dy,d#phi);dy;d#phi",part1name.Data(),part2name.Data()));
			hMempty[ipaty]	= (TH2D*)R[ipaty]->GethMempty();
			hMempty[ipaty]	->SetName(Form("hMempty_1_%d",ipaty));
			hMempty[ipaty]	->SetTitle(Form("%s%s, # Zvtx slices with empty #rho_{2}(M) vs. (dy,d#phi);dy;d#phi",part1name.Data(),part2name.Data()));
			hzoomM[ipaty]	= (TH2D*)R[ipaty]->GethZoomM();
			hzoomM[ipaty]	->SetName(Form("hzoomM_%d",ipaty));
			hzoomM[ipaty]	->SetTitle(Form("%s%s, mixed pairs, zoom;#Delta#eta;#Delta#phi (deg)",part1name.Data(),part2name.Data()));
			hMinvFS[ipaty]	= (TH1D*)R[ipaty]->GethMinvFineS();
			hMinvFS[ipaty]	->SetName(Form("hMinvFS_%d",ipaty));
			hMinvFS[ipaty]	->SetTitle(Form("%s%s, M_{inv} near threshold;M_{inv} (GeV);pairs",part1name.Data(),part2name.Data()));
			hMinvFM[ipaty]	= (TH1D*)R[ipaty]->GethMinvFineM();
			hMinvFM[ipaty]	->SetName(Form("hMinvFM_%d",ipaty));
			hMinvFM[ipaty]	->SetTitle(Form("%s%s, M_{inv} near threshold, mixed;M_{inv} (GeV);pairs",part1name.Data(),part2name.Data()));
			for (int ir2=0;ir2<NR2TYPES;ir2++){
				TString r2string1		= TString("y_{1},y_{2}");	// ir==0 is (y1,y2)
				TString r2string2		= TString(";y_{1};y_{2}");	// ir==0 is (y1,y2)
				if (ir2==1) r2string1	= TString("dy,d#phi");		// ir==1 is (dy,dphi)
				if (ir2==1) r2string2	= TString(";dy;d#phi");		// ir==1 is (dy,dphi)
				if (ir2==2) r2string1	= TString("dy,dq");			// ir==2 is (dy,dq)
				if (ir2==2) r2string2	= TString(";dy;dq");		// ir==2 is (dy,dq)
				//---- get Zvtx-averaged densities...
					hrho2[ir2][ipaty]		= (TH2D*)R[ipaty]->Gethrho2_S(ir2);
					hrho2[ir2][ipaty]		->SetName(Form("hrho2_%d_%d",ir2,ipaty));
					hrho2[ir2][ipaty]		->SetTitle(Form("%s%s, #rho_{2}(S) vs. (%s)%s",part1name.Data(),part2name.Data(),r2string1.Data(),r2string2.Data()));
					hrho1rho1[ir2][ipaty]	= (TH2D*)R[ipaty]->Gethrho2_M(ir2);
					hrho1rho1[ir2][ipaty]	->SetTitle(Form("%s%s, #rho_{2}(M) vs. (%s)%s",part1name.Data(),part2name.Data(),r2string1.Data(),r2string2.Data()));
					hrho1rho1[ir2][ipaty]	->SetName(Form("hrho1rho1_%d_%d",ir2,ipaty));
				hC2[ir2][ipaty]	= (TH2D*)R[ipaty]->GethC2(ir2);
				hC2[ir2][ipaty]	->SetName(Form("hC2_%d_%d",ir2,ipaty));
				hC2[ir2][ipaty]  	->SetTitle(Form("%s%s, C_{2} vs. (%s)%s",part1name.Data(),part2name.Data(),r2string1.Data(),r2string2.Data()));
				hR2[ir2][ipaty]	= (TH2D*)R[ipaty]->GethR2(ir2);
				hR2[ir2][ipaty]	->SetName(Form("hR2_%d_%d",ir2,ipaty));
				hR2[ir2][ipaty]  	->SetTitle(Form("%s%s, R_{2} vs. (%s)%s",part1name.Data(),part2name.Data(),r2string1.Data(),r2string2.Data()));
			}	// end ir2 loop
			//
			hR2yydy[ipaty]	= (TH1D*)R[ipaty]->GethR2yydy();
			hR2yydy[ipaty]	->SetTitle(Form("%s%s, R_{2yy}(dy)",part1name.Data(),part2name.Data()));
			hR2yydy[ipaty]	->SetName(Form("hR2yydy_%d",ipaty));
			hR2dy[ipaty]	= (TH1D*)R[ipaty]->GethR2dy();
			hR2dy[ipaty]	->SetTitle(Form("%s%s, R_{2}(dy)",part1name.Data(),part2name.Data()));
			hR2dy[ipaty]	->SetName(Form("hR2dy_%d",ipaty));
			hR2dphi[ipaty]	= (TH1D*)R[ipaty]->GethR2dphi();
			hR2dphi[ipaty]	->SetTitle(Form("%s%s, R_{2}(dphi)",part1name.Data(),part2name.Data()));
			hR2dphi[ipaty]	->SetName(Form("hR2dphi_%d",ipaty));
			//
		}	// end ipaty...
	}	// end nocorrelations...


	//---- paint
	//
	int ican	= -1;
	TCanvas *ccan[1000];
	int itext	= -1;
	TLatex *text[1000];
	for (int i=0;i<1000;i++){
		text[i] = new TLatex();
		text[i]	->SetNDC();
		text[i]	->SetTextAlign(31);
		text[i]	->SetTextSize(0.09);
	}
	//
	gROOT->SetStyle("Modern");
	gStyle->SetOptStat(0);
	gStyle->SetPadRightMargin(0.12);
	gStyle->SetPadTopMargin(0.01);
	gStyle->SetPadBottomMargin(0.08);
	gStyle->SetPadLeftMargin(0.14);
	//
	//---- split-track SL monitor summary (see README_SplitTracks.md sec 9/10) -- always first page,
	//---- even if doSplitRemoval=false (then the flagged/"killed" hists are just empty -- hSL_cand
	//---- and hntpc_ij_cand are still filled regardless, for choosing the next threshold to scan).
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1400,900);
	ccan[ican]->cd(); ccan[ican]->Divide(4,3,0.0001,0.0001);
	ccan[ican]->cd(1); hphieta_killed->Draw("colz");
	{
		TH1D* pairs[7][2]	= { {heta,heta_killed}, {hphi,hphi_killed}, {hpt,hpt_killed}, {hntpc,hntpc_killed},
								{hquality,hquality_killed}, {hdcaxy,hdcaxy_killed}, {hdcaz,hdcaz_killed} };
		for (int ip=0;ip<7;ip++){
			ccan[ican]->cd(2+ip);
			TH1D* hk	= (TH1D*)pairs[ip][0]->Clone(Form("hcmp_kept_%d",ip));
			TH1D* hr	= (TH1D*)pairs[ip][1]->Clone(Form("hcmp_killed_%d",ip));
			hk->SetLineColor(1); hr->SetLineColor(2);
			if (hk->Integral()>0) hk->Scale(1./hk->Integral());
			if (hr->Integral()>0) hr->Scale(1./hr->Integral());
			hk->SetTitle(Form("%s (black=kept, red=flagged)",pairs[ip][0]->GetTitle()));
			hk->Draw("hist"); hr->Draw("hist same");
		}
	}
	//---- new-mechanism-specific plots (sec 10) -- SL distribution (with the current cut value
	//---- marked, if enabled) and the ntpc[i] vs ntpc[j] symmetry check from sec 9.6.
	ccan[ican]->cd(9);
	hSL_cand->Draw("hist");
	TLine *lineSLcut	= 0;
	if (doSplitRemoval){
		lineSLcut	= new TLine(valSLCut,0,valSLCut,hSL_cand->GetMaximum());
		lineSLcut	->SetLineColor(2); lineSLcut->SetLineWidth(2); lineSLcut->SetLineStyle(2);
		lineSLcut	->Draw();
	}
	ccan[ican]->cd(10); hntpc_ij_cand->Draw("colz");
	ccan[ican]->cd(11);
	++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.07);
	text[itext]->DrawLatex(0.88,0.90,Form("doSplitRemoval=%d",(int)doSplitRemoval));
	++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.07);
	text[itext]->DrawLatex(0.88,0.81,Form("DISABLE_LScuts=%d  DISABLE_RG=%d",(int)DISABLE_LScuts,(int)DISABLE_RG));
	++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.07);
	text[itext]->DrawLatex(0.88,0.72,Form("valSLCut=%.2f  valSiKeyCut=%.2f  valRadialGapCut=%.2f",valSLCut,valSiKeyCut,valRadialGapCut));
	++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.07);
	text[itext]->DrawLatex(0.88,0.63,Form("tracks flagged=%ld",nSplitFlagged_total));
	++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.07);
	text[itext]->DrawLatex(0.88,0.54,Form("events w/ a flag=%ld",nSplitFlagged_evts));
	++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.07);
	text[itext]->DrawLatex(0.88,0.45,Form("pairs: duplicate=%ld complementary=%ld (sec 17.14)",nFlagged_duplicate,nFlagged_complementary));
	++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.07);
	text[itext]->DrawLatex(0.88,0.36,Form("ULS (opp-charge) tracks flagged=%ld (sec 18.10, track-level as of now)",nFlagged_ULS));
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileNameO.Data());

	//---- event QA
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
	ccan[ican]->cd(); ccan[ican]->Divide(3,2,0.0001,0.0001);
	ccan[ican]->cd(1); hxing	->Draw();
	ccan[ican]->cd(2); hvtxx0	->Draw(); hvtxx	->Draw("same");
	ccan[ican]->cd(3); hvtxy0	->Draw(); hvtxy	->Draw("same");
	ccan[ican]->cd(4); hvtxz0	->Draw(); hvtxz	->Draw("same");
	ccan[ican]->cd(5); hntrk0	->Draw(); hntrk	->Draw("same");
	ccan[ican]->cd(6); htrig0	->Draw(); htrig	->Draw("same");
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());

	//---- track kinematics (raw = black, kept = green overlay)
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
	ccan[ican]->cd(); ccan[ican]->Divide(3,3,0.0001,0.0001);
	ccan[ican]->cd(1); heta0	->Draw(); heta	->Draw("same");
	ccan[ican]->cd(2); hphi0	->Draw(); hphi	->Draw("same");
	ccan[ican]->cd(3); gPad->SetLogy(1); hpt0	->Draw(); hpt	->Draw("same");
	ccan[ican]->cd(4); hphieta			->Draw("colz");
	ccan[ican]->cd(5); hpteta			->Draw("colz");
	ccan[ican]->cd(6); hptphi			->Draw("colz");
	ccan[ican]->cd(7); hetazvtx		->Draw("colz");
	ccan[ican]->cd(8); hetazvtx_pta	->Draw("colz");
	ccan[ican]->cd(9); hetazvtx_ptb	->Draw("colz");
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());

	//---- track quality (raw = black, kept = green overlay)
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
	ccan[ican]->cd(); ccan[ican]->Divide(3,3,0.0001,0.0001);
	ccan[ican]->cd(1); hntpc0	->Draw(); hntpc	->Draw("same");
	ccan[ican]->cd(2); hnmvtx0	->Draw(); hnmvtx	->Draw("same");
	ccan[ican]->cd(3); hnintt0	->Draw(); hnintt	->Draw("same");
	ccan[ican]->cd(4); hdcaxy0	->Draw(); hdcaxy	->Draw("same");
	ccan[ican]->cd(5); hdcaz0	->Draw(); hdcaz	->Draw("same");
	ccan[ican]->cd(6); hdcaxyz				->Draw("colz");
	ccan[ican]->cd(7); hntpc_phieta		->Draw("colz");
	ccan[ican]->cd(8); habsdcaxy_phieta	->Draw("colz");
	ccan[ican]->cd(9); habsdcaz_phieta		->Draw("colz");
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());

	//---- calorimeter QA
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
	ccan[ican]->cd(); ccan[ican]->Divide(2,3,0.0001,0.0001);
	ccan[ican]->cd(1); gPad->SetLogy(1); hetotem	->Draw();
	ccan[ican]->cd(2); gPad->SetLogy(1); hetotih	->Draw();
	ccan[ican]->cd(3); gPad->SetLogy(1); hetotoh	->Draw();
	ccan[ican]->cd(4); gPad->SetLogy(1); hetotioh	->Draw();
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());

	//---- dedx QA
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
	ccan[ican]->cd(); ccan[ican]->Divide(3,2,0.0001,0.0001);
	ccan[ican]->cd(1); hdedx			->Draw();
	ccan[ican]->cd(2); hndedx			->Draw();
	ccan[ican]->cd(3); hdedxphi		->Draw();
	ccan[ican]->cd(4); hdedxphi_etap	->Draw();
	ccan[ican]->cd(5); hdedxphi_etan	->Draw();
	ccan[ican]->cd(6); hdedx_phieta	->Draw("colz");
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());

	//---- dedx vs p (wide, then zoom) -- same dedx expectation curves on both frames
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
	ccan[ican]->cd(); ccan[ican]->Divide(2,1,0.0001,0.0001);
	ccan[ican]->cd(1); gPad->SetLogz(1); hdedxp	->Draw("colz");
	for (int ip=0;ip<NPART;ip++){ fdedxexp[ip]->Draw("same"); }
	ccan[ican]->cd(2); gPad->SetLogz(1); hdedxpz	->Draw("colz");
	for (int ip=0;ip<NPART;ip++){ fdedxexp[ip]->Draw("same"); }
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());

	//---- V0 timing QA
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
	ccan[ican]->cd(); ccan[ican]->Divide(3,2,0.0001,0.0001);
	ccan[ican]->cd(1); hKSxing			->Draw();
	ccan[ican]->cd(2); hLAxing			->Draw();
	ccan[ican]->cd(3); hALxing			->Draw();
	ccan[ican]->cd(4); hKSmass_xing	->Draw("colz");
	ccan[ican]->cd(5); hLAmass_xing	->Draw("colz");
	ccan[ican]->cd(6); hALmass_xing	->Draw("colz");
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());

	//---- V0 kinematics, K-short (worse-daughter DCA = offline README-sec-6 diagnostic, not yet cut on)
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
	ccan[ican]->cd(); ccan[ican]->Divide(4,2,0.0001,0.0001);
	ccan[ican]->cd(1); hKSmass		->Draw();
	ccan[ican]->cd(2); hKSpt		->Draw();
	ccan[ican]->cd(3); hKSeta		->Draw();
	ccan[ican]->cd(4); hKSphi		->Draw();
	ccan[ican]->cd(5); hKSdira		->Draw();
	ccan[ican]->cd(6); hKSpvdca	->Draw();
	ccan[ican]->cd(7); gPad->SetLogy(1); hKSworsedca->Draw();
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());

	//---- V0 kinematics, Lambda (mass panel: green = worse-track PV_DCA offline cut applied,
	//---- README_settingKFPcuts.md Part4/5 -- LA_WORSEDCA_CUT, see WorseDaughterPVDCA above)
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
	ccan[ican]->cd(); ccan[ican]->Divide(4,2,0.0001,0.0001);
	ccan[ican]->cd(1); hLAmass		->Draw(); hLAmass_clean->Draw("same");
	ccan[ican]->cd(2); hLApt		->Draw();
	ccan[ican]->cd(3); hLAeta		->Draw();
	ccan[ican]->cd(4); hLAphi		->Draw();
	ccan[ican]->cd(5); hLAdira		->Draw();
	ccan[ican]->cd(6); hLApvdca	->Draw();
	ccan[ican]->cd(7); gPad->SetLogy(1); hLAworsedca->Draw();
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());

	//---- V0 kinematics, anti-Lambda (mass panel: green = worse-track PV_DCA offline cut applied,
	//---- README_settingKFPcuts.md Part5 ana532 re-derivation -- AL_WORSEDCA_CUT, see WorseDaughterPVDCA above)
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
	ccan[ican]->cd(); ccan[ican]->Divide(4,2,0.0001,0.0001);
	ccan[ican]->cd(1); hALmass		->Draw(); hALmass_clean->Draw("same");
	ccan[ican]->cd(2); hALpt		->Draw();
	ccan[ican]->cd(3); hALeta		->Draw();
	ccan[ican]->cd(4); hALphi		->Draw();
	ccan[ican]->cd(5); hALdira		->Draw();
	ccan[ican]->cd(6); hALpvdca	->Draw();
	ccan[ican]->cd(7); gPad->SetLogy(1); hALworsedca->Draw();
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());

	//---- original V0-daughter dE/dx QA, moved to the very end (post-processing, MILES away for now)...
	//
	//---- gated dedx vs p
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
	ccan[ican]->cd(); ccan[ican]->Divide(3,2,0.0001,0.0001);
	for (int ipar=0;ipar<3;ipar++){
		for (int ichg=0;ichg<2;ichg++){
			double ymax=2000.;
			if (ipar==1&&ichg==0) ymax=4000;
			if (ipar==2&&ichg==1) ymax=4000;
			int ipad	= 3*ichg + ipar + 1;
			ccan[ican]	->cd(ipad);
			gPad		->SetLogz(1);
			hdedxp_tagged[ichg][ipar]->GetYaxis()->SetRangeUser(0.,ymax);
			hdedxp_tagged[ichg][ipar]->Draw("colz");
			for (int ip=0;ip<NPART;ip++){
				fdedxexp[ip]->Draw("same");
			}
		}
	}
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());


	//---- README_CQ: pion C(Q) summary page, 1x2: pi+pi- (black), pi+pi+ (red), pi-pi- (blue) C(Q),
	//---- 5 MeV bins, raw levels (per-event norm, i.e. the 1+R2 baseline). Left 0-1.2 GeV with the
	//---- pi+pi- decay signposts (the K0s line validates dq itself), right 0-0.3 GeV.
	if (!NOCORRELATIONS){
		int ipcq[3]		= {-1,-1,-1};		// pi+pi-, pi+pi+, pi-pi-
		for (int ipaty=0;ipaty<NPairTypes;ipaty++){
			int a	= PairTypes_Info[ipaty][0], b = PairTypes_Info[ipaty][1];
			if (a==kParticleIDPionPlus  && b==kParticleIDPionMinus && ipcq[0]<0) ipcq[0] = ipaty;
			if (a==kParticleIDPionPlus  && b==kParticleIDPionPlus  && ipcq[1]<0) ipcq[1] = ipaty;
			if (a==kParticleIDPionMinus && b==kParticleIDPionMinus && ipcq[2]<0) ipcq[2] = ipaty;
		}
		if (ipcq[0]>=0 && ipcq[1]>=0 && ipcq[2]>=0){
			++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
			ccan[ican]->cd(); ccan[ican]->Divide(2,1,0.0001,0.0001);
			const int	pcol[3]		= {kBlack,kRed,kBlue};
			const char*	plab[3]		= {"#pi^{+}#pi^{-}","#pi^{+}#pi^{+}","#pi^{-}#pi^{-}"};
			for (int ipad=0;ipad<2;ipad++){
				ccan[ican]->cd(ipad+1);
				double xmax	= (ipad==0) ? 1.2 : 0.3;
				TH1D *hp[3];
				double ymn=1e30, ymx=-1e30;
				for (int k=0;k<3;k++){
					hp[k]	= (TH1D*)hCQ[ipcq[k]]->Clone(Form("hCQpions_%d_%d",ipad,k));
					hp[k]->SetDirectory(0);
					hp[k]->Rebin(5); hp[k]->Scale(1./5.);
					hp[k]->SetLineColor(pcol[k]); hp[k]->SetMarkerColor(pcol[k]);
					hp[k]->GetXaxis()->SetRangeUser(0.,xmax);
					for (int ib=hp[k]->FindBin(0.0201);ib<=hp[k]->GetXaxis()->GetLast();ib++){	// display range from Q>20 MeV
						if (hp[k]->GetBinError(ib)<=0.) continue;
						double v	= hp[k]->GetBinContent(ib);
						ymn	= std::min(ymn,v); ymx = std::max(ymx,v);
					}
				}
				hp[0]->SetTitle(Form("pion C(Q), 5 MeV bins, raw level;Q_{inv} (GeV);C(Q)"));
				if (ymx>ymn){ hp[0]->SetMinimum(ymn-0.05*(ymx-ymn)); hp[0]->SetMaximum(ymx+0.25*(ymx-ymn)); }
				hp[0]->Draw("hist");
				for (int k=1;k<3;k++) hp[k]->Draw("hist same");
				gPad->Update();
				DrawCQSignposts(kParticleIDPionPlus,kParticleIDPionMinus,gPad->GetUymin(),gPad->GetUymax(),xmax);
				TLegend *lgp	= new TLegend(0.72,0.40,0.89,0.58);
				lgp->SetBorderSize(0); lgp->SetFillStyle(0); lgp->SetTextSize(0.04);
				for (int k=0;k<3;k++){
					TLegendEntry *lep	= lgp->AddEntry(hp[k],plab[k],"l");
					lep->SetLineColor(pcol[k]); lep->SetLineWidth(2); lep->SetTextColor(pcol[k]);	// legend line colors don't survive the .ps
				}
				lgp->Draw();
			}
			ccan[ican]->cd(); ccan[ican]->Update();
			ccan[ican]->Print(OutputFileName.Data());
		}
	}

	//---- CF summary pages, one 4x3 page per PairTypes_Info entry, in order.
	//---- Row1: N/evt (+text), R2(dy), R2(dphi), [v0-species-1 mass, if either leg is a v0]
	//---- Row2: rho2(S)/rho2(M)/R2 vs (y1,y2), [v0-species-2 mass, if a 2nd distinct v0 species]
	//---- Row3: rho2(S)/rho2(M)/R2 vs (dy,dphi), R2(dy,dphi) again as colz
	if (!NOCORRELATIONS){
		for (int ipaty=0;ipaty<NPairTypes;ipaty++){
			int ipid1			= PairTypes_Info[ipaty][0];
			int ipid2			= PairTypes_Info[ipaty][1];
			TString part1name	= TString(ParticleIDNames[ipid1]);
			TString part2name	= TString(ParticleIDNames[ipid2]);
			//
			//---- individual v0-species mass panels (the v0's OWN reconstructed mass, e.g.
			//---- hKSmass from v0mass -- NOT CalcRm's Minv, which is the combined PAIR's
			//---- invariant mass and isn't meaningful as "the v0 mass" here). Up to 2 distinct
			//---- v0 species can appear in one pairtype (e.g. KS-LA); pick them up in order.
			const int NV0MASSPAD			= 2;
			TH1D* hV0MassPad[NV0MASSPAD]	= {0,0};
			TH1D* hV0MassPadClean[NV0MASSPAD]	= {0,0};
			int nv0MassPad	= 0;
			for (int ip=0;ip<2;ip++){
				int ipid	= (ip==0) ? ipid1 : ipid2;
				bool dupe	= (ip==1 && ipid2==ipid1);
				if (dupe) continue;
				if      (ipid==kParticleIDKshort    && nv0MassPad<NV0MASSPAD){ hV0MassPad[nv0MassPad]=hKSmass; ++nv0MassPad; }
				else if (ipid==kParticleIDLambda     && nv0MassPad<NV0MASSPAD){ hV0MassPad[nv0MassPad]=hLAmass; hV0MassPadClean[nv0MassPad]=hLAmass_clean; ++nv0MassPad; }
				else if (ipid==kParticleIDAntiLambda && nv0MassPad<NV0MASSPAD){ hV0MassPad[nv0MassPad]=hALmass; hV0MassPadClean[nv0MassPad]=hALmass_clean; ++nv0MassPad; }
			}
			//
			double nevt		= hmult[ipaty]	->GetEntries();
			double meanN1	= hmult1[ipaty]	->GetMean();
			double meanN2	= hmult2[ipaty]	->GetMean();
			double intN1	= hy1[ipaty]	->Integral();
			double intN2	= hy2[ipaty]	->Integral();
			//
			++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
			ccan[ican]->cd(); ccan[ican]->Divide(4,3,0.0001,0.0001);
			//---- row 1...
			ccan[ican]->cd(1); gPad->SetLogy(1); hmult1[ipaty]->Draw();
			++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.06); text[itext]->DrawLatex(0.88,0.85,Form("N_{evt}=%.0f"        ,nevt  ));
			++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.06); text[itext]->DrawLatex(0.88,0.78,Form("<N_{1}>=%.2f"        ,meanN1));
			++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.06); text[itext]->DrawLatex(0.88,0.71,Form("INT #rho_{1}(1)=%.2f",intN1 ));
			++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.06); text[itext]->DrawLatex(0.88,0.60,Form("<N_{2}>=%.2f"        ,meanN2));
			++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.06); text[itext]->DrawLatex(0.88,0.53,Form("INT #rho_{1}(2)=%.2f",intN2 ));
			ccan[ican]->cd(2); hR2yydy[ipaty]			->Draw();
			ccan[ican]->cd(3); hR2dphi[ipaty]			->Draw();
			if (nv0MassPad>0){
				ccan[ican]->cd(4); hV0MassPad[0]->Draw(); if (hV0MassPadClean[0]) hV0MassPadClean[0]->Draw("same");
			}
			//---- row 2...
			ccan[ican]->cd(5); hrho2[0][ipaty]			->Draw("surf3");
			ccan[ican]->cd(6); hrho1rho1[0][ipaty]		->Draw("surf3");
			ccan[ican]->cd(7); hR2[0][ipaty]			->Draw("lego2");
			if (nv0MassPad>1){
				ccan[ican]->cd(8); hV0MassPad[1]->Draw(); if (hV0MassPadClean[1]) hV0MassPadClean[1]->Draw("same");
			}
			//---- row 3...
			ccan[ican]->cd(9);  hrho2[1][ipaty]		->Draw("surf3");
			ccan[ican]->cd(10); hrho1rho1[1][ipaty]	->Draw("surf3");
			ccan[ican]->cd(11); hR2[1][ipaty]			->Draw("lego2");
			ccan[ican]->cd(12); hR2[1][ipaty]			->Draw("colz");
			ccan[ican]->cd(); ccan[ican]->Update();
			ccan[ican]->Print(OutputFileName.Data());
			//
			//---- femtoscopic C(Q) page, 2x2
			//---- (README_CQ): C(Q) 0-1.2 GeV with decay signposts | C(Q) in STAR's 4 kT bins |
			//---- C(Q) 0-0.3 GeV zoom | Minv near threshold. Drawn from clones, the written hists keep full range.
			++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
			ccan[ican]->cd(); ccan[ican]->Divide(2,2,0.0001,0.0001);
			ccan[ican]->cd(1);
			{
				TH1D *hf	= (TH1D*)hCQ[ipaty]->Clone(Form("hCQfull_%d",ipaty));
				hf->SetDirectory(0);
				hf->Rebin(5); hf->Scale(1./5.);			// 5 MeV for display
				hf->SetTitle(Form("%s (5 MeV bins)",hCQ[ipaty]->GetTitle()));
				double ymn=1e30, ymx=-1e30;			// display range from Q>20 MeV (lowest bins are noisy)
				for (int ib=hf->FindBin(0.0201);ib<=hf->GetNbinsX();ib++){
					if (hf->GetBinError(ib)<=0.) continue;
					ymn	= std::min(ymn,hf->GetBinContent(ib)); ymx = std::max(ymx,hf->GetBinContent(ib));
				}
				if (ymx>ymn){ hf->SetMinimum(ymn-0.05*(ymx-ymn)); hf->SetMaximum(ymx+0.25*(ymx-ymn)); }
				hf->Draw();
				gPad->Update();
				DrawCQSignposts(ipid1,ipid2,gPad->GetUymin(),gPad->GetUymax(),1.2);
			}
			ccan[ican]->cd(2);
			{
				const int kcol[4]	= {kBlue,kGreen+2,kOrange+1,kRed};
				TH1D *hk[4];
				double ymn=1e30, ymx=-1e30;
				for (int ik=0;ik<4;ik++){
					hk[ik]	= hCQKT[ipaty]->ProjectionX(Form("hCQKTpx_%d_%d",ipaty,ik),ik+1,ik+1,"e");
					hk[ik]->SetDirectory(0);
					hk[ik]->Rebin(5); hk[ik]->Scale(1./5.);		// 10 MeV for display
					hk[ik]->SetLineColor(kcol[ik]); hk[ik]->SetMarkerColor(kcol[ik]);
					for (int ib=hk[ik]->FindBin(0.0201);ib<=hk[ik]->GetNbinsX();ib++){	// display range from Q>20 MeV
						double v	= hk[ik]->GetBinContent(ib);
						if (hk[ik]->GetBinError(ib)<=0.) continue;
						ymn	= std::min(ymn,v); ymx = std::max(ymx,v);
					}
				}
				hk[0]->SetTitle(Form("%s%s, C(Q) in k_{T} bins (10 MeV bins);Q_{inv} (GeV)",part1name.Data(),part2name.Data()));
				if (ymx>ymn){ hk[0]->SetMinimum(ymn-0.05*(ymx-ymn)); hk[0]->SetMaximum(ymx+0.05*(ymx-ymn)); }
				hk[0]->Draw("hist");
				for (int ik=1;ik<4;ik++) hk[ik]->Draw("hist same");
				TLegend *lgk	= new TLegend(0.55,0.65,0.89,0.89);
				lgk->SetBorderSize(0); lgk->SetFillStyle(0); lgk->SetTextSize(0.035);
				for (int ik=0;ik<4;ik++){
					double k1	= hCQKT[ipaty]->GetYaxis()->GetBinLowEdge(ik+1);
					double k2	= hCQKT[ipaty]->GetYaxis()->GetBinUpEdge(ik+1);
					TLegendEntry *lek	= lgk->AddEntry(hk[ik],Form("%.2f<k_{T}<%.2f GeV",k1,k2),"l");
					lek->SetLineColor(kcol[ik]); lek->SetLineWidth(2); lek->SetTextColor(kcol[ik]);	// legend line colors don't survive the .ps
				}
				lgk->Draw();
			}
			ccan[ican]->cd(3);
			{
				TH1D *hz	= (TH1D*)hCQ[ipaty]->Clone(Form("hCQzoom_%d",ipaty));
				hz->SetDirectory(0);
				hz->GetXaxis()->SetRangeUser(0.,0.3);
				hz->SetTitle(Form("%s (1 MeV bins, zoom)",hCQ[ipaty]->GetTitle()));
				double ymn=1e30, ymx=-1e30;			// display range from Q>20 MeV (lowest bins are noisy)
				for (int ib=hz->FindBin(0.0201);ib<=hz->FindBin(0.2999);ib++){
					if (hz->GetBinError(ib)<=0.) continue;
					ymn	= std::min(ymn,hz->GetBinContent(ib)); ymx = std::max(ymx,hz->GetBinContent(ib));
				}
				if (ymx>ymn){ hz->SetMinimum(ymn-0.05*(ymx-ymn)); hz->SetMaximum(ymx+0.05*(ymx-ymn)); }
				hz->Draw();
			}
			//---- pad 4: invariant mass near threshold, sibling (green) vs mixed (blue). Mixed is a
			//---- raw-count shape reference, scaled to the sibling integral over the top 150 MeV of the
			//---- range -- not expected to match the sibling background exactly.
			ccan[ican]->cd(4);
			{
				TH1D *ms	= hMinvFS[ipaty];
				TH1D *mm	= (TH1D*)hMinvFM[ipaty]->Clone(Form("hMinvFMdraw_%d",ipaty));
				mm->SetDirectory(0);
				int b1		= ms->GetXaxis()->FindBin(ms->GetXaxis()->GetXmax()-0.150+1e-6);
				int b2		= ms->GetNbinsX();
				double im	= mm->Integral(b1,b2);
				double fmix	= (im>0.) ? ms->Integral(b1,b2)/im : 0.;	// mixed is scaled; sibling is raw
				if (fmix>0.) mm->Scale(fmix);
				ms->SetLineColor(kGreen+2); ms->SetLineWidth(1);
				mm->SetLineColor(kBlue);    mm->SetLineWidth(1);
				ms->SetMinimum(0.);
				ms->SetMaximum(1.1*std::max(ms->GetMaximum(),mm->GetMaximum()));
				ms->Draw("hist");
				mm->Draw("hist same");
				TLegend *lgm	= new TLegend(0.30,0.11,0.89,0.21);
				lgm->SetBorderSize(0); lgm->SetFillColor(kWhite); lgm->SetTextSize(0.030);
				TLegendEntry *le1	= lgm->AddEntry(ms,"sibling (raw counts)","l");
				le1->SetLineColor(kGreen+2); le1->SetLineWidth(1);
				TLegendEntry *le2	= lgm->AddEntry(mm,Form("mixed (per event) #times %.4g (sib/mix, top 150 MeV)",fmix),"l");
				le2->SetLineColor(kBlue); le2->SetLineWidth(1);
				lgm->Draw();
			}
			ccan[ican]->cd(); ccan[ican]->Update();
			ccan[ican]->Print(OutputFileName.Data());
			//
			//---- crossing-correction page (README_Crossing decision 12, step 9), 3x2:
			//---- column 1: R2(dy,dphi) uncorrected (colz, red perimeter around dirty-side near bins that differ
			//---- from their dphi mirror by >3 sigma, either sign) over R2C(dy,dphi) crossing-corrected (colz).
			//---- columns 2,3: R2(dy) (top row) and R2(dphi) (bottom row) projected from the Zvtx-averaged 2D
			//---- maps, whole range (col 2: uncorrected black, corrected red; R2(dy) also R2yy(dy) from
			//---- R2(y1,y2) in green) | narrow window (col 3: uncorrected blue, corrected magenta).
			//---- Valid bin <=> Zvtx-averaged rho2(M)>0 (for R2C: its source bin(s), sec 0).
			//---- Projections: mean over valid bins, error sqrt(sum e^2)/n. Drawn from clones only.
			if (R[ipaty]->GetDoCrossing()){
				TH2D *hu	= (TH2D*)hR2[1][ipaty]->Clone(Form("hR2draw_%d",ipaty));	hu->SetDirectory(0);
				TH2D *hc	= (TH2D*)hR2C[ipaty]->Clone(Form("hR2Cdraw_%d",ipaty));	hc->SetDirectory(0);
				TH2D *hm	= hrho1rho1[1][ipaty];
				int ds		= R[ipaty]->GetDirtySide();
				int nx		= hu->GetNbinsX();
				int ny		= hu->GetNbinsY();
				double lo	= hu->GetYaxis()->GetXmin();
				//---- dphi mirror bin (same rule as CrossingCorrect.h) and wrapped bin center
				std::vector<int>	jmir(ny+1,0);
				std::vector<double>	cwr(ny+1,0.);
				for (int iy=1;iy<=ny;iy++){
					double c	= hu->GetYaxis()->GetBinCenter(iy);
					while (c>= 180.) c -= 360.;
					while (c< -180.) c += 360.;
					cwr[iy]		= c;
					double cm	= -c;
					while (cm<  lo      ) cm += 360.;
					while (cm>= lo+360. ) cm -= 360.;
					jmir[iy]	= hu->GetYaxis()->FindFixBin(cm);
				}
				auto okU	= [&](int ix,int iy){ return hm->GetBinContent(ix,iy)>0.; };
				auto okC	= [&](int ix,int iy){
					if (ds==0) return okU(ix,iy);															// not pt-ordered: R2C = R2
					if (fabs(cwr[iy])<90.) return okU(ix, (cwr[iy]*ds>0.) ? jmir[iy] : iy);			// near side: the clean source
					return okU(ix,iy) || okU(ix,jmir[iy]);											// away side: either partner
				};
				//---- common z range over valid bins of both maps
				double zmin	=  1e30, zmax = -1e30;
				for (int ix=1;ix<=nx;ix++) for (int iy=1;iy<=ny;iy++){
					if (okU(ix,iy)){ zmin = std::min(zmin,hu->GetBinContent(ix,iy)); zmax = std::max(zmax,hu->GetBinContent(ix,iy)); }
					if (okC(ix,iy)){ zmin = std::min(zmin,hc->GetBinContent(ix,iy)); zmax = std::max(zmax,hc->GetBinContent(ix,iy)); }
				}
				if (zmin<zmax){ hu->SetMinimum(zmin); hu->SetMaximum(zmax); hc->SetMinimum(zmin); hc->SetMaximum(zmax); }
				//---- flagged dirty-side near bins: |R2 - R2(mirror)| > 3 sigma
				std::vector<std::vector<bool>> flag(nx+2,std::vector<bool>(ny+2,false));
				int nflag	= 0;
				if (ds!=0){
					for (int ix=1;ix<=nx;ix++) for (int iy=1;iy<=ny;iy++){
						if (!(fabs(cwr[iy])<90. && cwr[iy]*ds>0.)) continue;
						int jy	= jmir[iy];
						if (!okU(ix,iy) || !okU(ix,jy)) continue;
						double d	= hu->GetBinContent(ix,iy) - hu->GetBinContent(ix,jy);
						double e	= sqrt(pow(hu->GetBinError(ix,iy),2) + pow(hu->GetBinError(ix,jy),2));
						if (e>0. && fabs(d)>3.*e){ flag[ix][iy] = true; ++nflag; }
					}
				}
				//---- projections from the 2D maps
				TH1D *hpu[2], *hpc[2];
				hpu[0]	= new TH1D(Form("hR2dyU_%d",ipaty)  ,Form("%s%s, R_{2}(dy) from R_{2}(dy,d#phi);dy;R_{2}"            ,part1name.Data(),part2name.Data()),nx,hu->GetXaxis()->GetXmin(),hu->GetXaxis()->GetXmax());
				hpu[1]	= new TH1D(Form("hR2dphiU_%d",ipaty),Form("%s%s, R_{2}(d#phi) from R_{2}(dy,d#phi);d#phi (deg);R_{2}",part1name.Data(),part2name.Data()),ny,lo,hu->GetYaxis()->GetXmax());
				hpc[0]	= (TH1D*)hpu[0]->Clone(Form("hR2dyC_%d",ipaty));
				hpc[1]	= (TH1D*)hpu[1]->Clone(Form("hR2dphiC_%d",ipaty));
				for (int k=0;k<2;k++){ hpu[k]->SetDirectory(0); hpc[k]->SetDirectory(0); }
				for (int ip=0;ip<2;ip++){
					TH2D *h2	= (ip==0) ? hu : hc;
					TH1D **hp	= (ip==0) ? hpu : hpc;
					for (int k=0;k<2;k++){
						int nb	= (k==0) ? nx : ny;
						for (int ib=1;ib<=nb;ib++){
							double v=0, e2=0; int n=0;
							int nb2	= (k==0) ? ny : nx;
							for (int jb=1;jb<=nb2;jb++){
								int ix	= (k==0) ? ib : jb;
								int iy	= (k==0) ? jb : ib;
								bool ok	= (ip==0) ? okU(ix,iy) : okC(ix,iy);
								if (!ok) continue;
								v	+= h2->GetBinContent(ix,iy);
								e2	+= pow(h2->GetBinError(ix,iy),2);
								++n;
							}
							if (n>0){ hp[k]->SetBinContent(ib,v/n); hp[k]->SetBinError(ib,sqrt(e2)/n); }
						}
					}
				}
				//---- narrow-window projections: the all-range unweighted means dilute the crossing damage, which
				//---- sits at |dy|<~0.2 and on the near side (README_Crossing sec 9).
				//---- hnu/hnc[0] = R2(dy)   over near-side |dphi|<DPHICUTPAGE (dphi bin centers)
				//---- hnu/hnc[1] = R2(dphi) over |dy|<DYCUTPAGE (dy bin centers)
				//---- Both cuts widen to 0.51 x bin width, so the wide V0 bins still keep their central bin(s).
				const double DYCUTPAGE		= 0.1;
				const double DPHICUTPAGE	= 30.;
				double dycut	= std::max(DYCUTPAGE,   0.51*hu->GetXaxis()->GetBinWidth(1));
				double dphicut	= std::max(DPHICUTPAGE, 0.51*hu->GetYaxis()->GetBinWidth(1));
				TH1D *hnu[2], *hnc[2];
				for (int k=0;k<2;k++){
					hnu[k]	= (TH1D*)hpu[k]->Clone(Form("hR2n%dU_%d",k,ipaty));	hnu[k]->Reset(); hnu[k]->SetDirectory(0);
					hnc[k]	= (TH1D*)hpu[k]->Clone(Form("hR2n%dC_%d",k,ipaty));	hnc[k]->Reset(); hnc[k]->SetDirectory(0);
				}
				for (int ip=0;ip<2;ip++){
					TH2D *h2	= (ip==0) ? hu : hc;
					for (int k=0;k<2;k++){
						TH1D *hn	= (ip==0) ? hnu[k] : hnc[k];
						int nb		= (k==0) ? nx : ny;
						for (int ib=1;ib<=nb;ib++){
							double v=0, e2=0; int n=0;
							int nb2	= (k==0) ? ny : nx;
							for (int jb=1;jb<=nb2;jb++){
								int ix	= (k==0) ? ib : jb;
								int iy	= (k==0) ? jb : ib;
								if (k==0 && fabs(cwr[iy])>=dphicut) continue;
								if (k==1 && fabs(hu->GetXaxis()->GetBinCenter(ix))>=dycut) continue;
								bool ok	= (ip==0) ? okU(ix,iy) : okC(ix,iy);
								if (!ok) continue;
								v	+= h2->GetBinContent(ix,iy);
								e2	+= pow(h2->GetBinError(ix,iy),2);
								++n;
							}
							if (n>0){ hn->SetBinContent(ib,v/n); hn->SetBinError(ib,sqrt(e2)/n); }
						}
					}
				}
				//
				++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
				ccan[ican]->cd(); ccan[ican]->Divide(3,2,0.0001,0.0001);
				ccan[ican]->cd(1); gPad->SetRightMargin(0.14);
				hu->SetTitle(Form("%s%s, R_{2}(dy,d#phi) uncorrected;dy;d#phi (deg)",part1name.Data(),part2name.Data()));
				hu->Draw("colz");
				for (int ix=1;ix<=nx;ix++) for (int iy=1;iy<=ny;iy++){
					if (!flag[ix][iy]) continue;
					double x1	= hu->GetXaxis()->GetBinLowEdge(ix), x2 = hu->GetXaxis()->GetBinUpEdge(ix);
					double y1	= hu->GetYaxis()->GetBinLowEdge(iy), y2 = hu->GetYaxis()->GetBinUpEdge(iy);
					TLine *ln[4]	= {0,0,0,0};
					if (!flag[ix-1][iy]) ln[0] = new TLine(x1,y1,x1,y2);
					if (!flag[ix+1][iy]) ln[1] = new TLine(x2,y1,x2,y2);
					if (!flag[ix][iy-1]) ln[2] = new TLine(x1,y1,x2,y1);
					if (!flag[ix][iy+1]) ln[3] = new TLine(x1,y2,x2,y2);
					for (int k=0;k<4;k++) if (ln[k]){ ln[k]->SetLineColor(kRed); ln[k]->SetLineWidth(2); ln[k]->Draw(); }
				}
				++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.035); text[itext]->SetTextColor(kRed); text[itext]->SetTextAlign(11);
				text[itext]->DrawLatex(0.02,0.015,Form("red: dirty |R_{2}-mirror|>3#sigma, %d bins (d#phi%s0)",nflag,ds>0?">":"<"));
				ccan[ican]->cd(4); gPad->SetRightMargin(0.14);
				hc->SetTitle(Form("%s%s, R_{2}(dy,d#phi) crossing-corrected;dy;d#phi (deg)",part1name.Data(),part2name.Data()));
				hc->Draw("colz");
				//---- R2yy(dy) from R2(y1,y2) (CalcRm, Zvtx-averaged): no dphi, so whole range only, not crossing-correctable
				TH1D *hyy	= (TH1D*)hR2yydy[ipaty]->Clone(Form("hR2yydydraw_%d",ipaty));	hyy->SetDirectory(0);
				hyy->SetLineColor(kGreen+2); hyy->SetMarkerColor(kGreen+2); hyy->SetMarkerStyle(21); hyy->SetMarkerSize(0.5);
				for (int k=0;k<2;k++){
					hpu[k]->SetLineColor(kBlack);   hpu[k]->SetMarkerColor(kBlack);   hpu[k]->SetMarkerStyle(20); hpu[k]->SetMarkerSize(0.5);
					hpc[k]->SetLineColor(kRed);     hpc[k]->SetMarkerColor(kRed);     hpc[k]->SetMarkerStyle(24); hpc[k]->SetMarkerSize(0.5);
					hnu[k]->SetLineColor(kBlue);    hnu[k]->SetMarkerColor(kBlue);    hnu[k]->SetMarkerStyle(20); hnu[k]->SetMarkerSize(0.5);
					hnc[k]->SetLineColor(kMagenta); hnc[k]->SetMarkerColor(kMagenta); hnc[k]->SetMarkerStyle(24); hnc[k]->SetMarkerSize(0.5);
					TString cutlab	= (k==0) ? Form("|d#phi|<%.0f#circ",dphicut) : Form("|dy|<%.2g",dycut);
					//---- iw=0: whole range (col 2), iw=1: narrow window (col 3); own y range per frame
					for (int iw=0;iw<2;iw++){
						ccan[ican]->cd(2+3*k+iw);
						TH1D *ha	= (iw==0) ? hpu[k] : hnu[k];
						TH1D *hb	= (iw==0) ? hpc[k] : hnc[k];
						bool doyy	= (iw==0 && k==0);
						double pmin	= std::min(ha->GetMinimum(), hb->GetMinimum());
						double pmax	= std::max(ha->GetMaximum(), hb->GetMaximum());
						if (doyy){ pmin = std::min(pmin,hyy->GetMinimum()); pmax = std::max(pmax,hyy->GetMaximum()); }
						if (iw==1) ha->SetTitle(Form("%s, %s",hpu[k]->GetTitle(),cutlab.Data()));
						ha->SetMinimum(pmin-0.1*(pmax-pmin)); ha->SetMaximum(pmax+0.30*(pmax-pmin));	// headroom for the legend
						ha->Draw("E1");
						if (k==1){		// thin grey dphi=0 line in the background, then the points again on top
							TLine *l0	= new TLine(0.,ha->GetMinimum(),0.,ha->GetMaximum());
							l0->SetLineColor(kGray); l0->SetLineWidth(1); l0->Draw();
							ha->Draw("E1 same");
						}
						hb->Draw("E1 same");
						if (doyy) hyy->Draw("E1 same");
						//---- legend centered just under the title, boxed, transparent
						TLegend *lgc	= new TLegend(0.25,doyy?0.74:0.78,0.75,0.89);
						lgc->SetBorderSize(1); lgc->SetFillStyle(0); lgc->SetTextSize(0.04);
						lgc->AddEntry(ha,(iw==0) ? "No Corr" : Form("No Corr %s",cutlab.Data()),"lp");
						lgc->AddEntry(hb,(iw==0) ? "Cross Corr" : Form("Cross Corr %s",cutlab.Data()),"lp");
						if (doyy) lgc->AddEntry(hyy,"from R_{2}(y_{1},y_{2}), No Corr","lp");
						lgc->Draw();
					}
				}
				ccan[ican]->cd(); ccan[ican]->Update();
				ccan[ican]->Print(OutputFileName.Data());
			}
		}	// end ipaty CF summary pages
	}	// end nocorrelations CF summary pages


	//---- close-out...
	//
	cout<<"Closing ps file "<<OutputFileName.Data()<<endl;
	ccan[ican]->Print(OutputFileNameC.Data());	// close ps file
	char buf[200];
	sprintf(buf,"/usr/bin/ps2pdf %s %s",OutputFileName.Data(),OutputFileNameP.Data());
	cout<<"Executing: "<<buf<<endl;
	int iSuccess	= gSystem->Exec(buf);
	if (iSuccess==0){
		sprintf(buf,"/bin/rm %s",OutputFileName.Data());
		gSystem->Exec(buf);
	} else {
		cout<<"PDF creation failed: "<<OutputFileNameP.Data()<<endl;
	}
	//
	cout<<"split-track pre-pass -- tracks flagged="<<nSplitFlagged_total<<" events w/ a flag="<<nSplitFlagged_evts
		<<" pairs: duplicate="<<nFlagged_duplicate<<" complementary="<<nFlagged_complementary<<" (sec 17.14)"
		<<" ULS(opp-charge)="<<nFlagged_ULS<<" (sec 18.10, track-level)"<<endl;
	cout<<"sec 18.22 -- doXTFClean="<<doXTFClean<<" strict="<<doXTFStrict<<" neither="<<nXTF_neither<<" duplicate pairs="<<nXTF_pairs<<" byINTT="<<nXTF_byINTT
		<<" byQuality="<<nXTF_byQuality<<" losers="<<nXTF_losers<<endl;
	cout<<"sec 18.21 -- ONLY_FIRST_TF="<<ONLY_FIRST_TF<<" rows skipped as same-TF duplicates="<<nSkippedSameTF<<endl;
	cout<<"doTFDup="<<doTFDup<<" -- rows skipped as overlapping-TF collision copies="<<nTFDup_rows<<endl;
	cout<<"sec 18.21 -- ONLY_CROSSING0="<<ONLY_CROSSING0<<" rows skipped with crossing!=0="<<nSkippedXingNot0<<endl;
	cout<<"sec 18.21 -- ONLY_FIRST_XINGPOS="<<ONLY_FIRST_XINGPOS<<" rows skipped (crossing<=0 or not first crossing>0 in TF)="<<nSkippedXingPos<<endl;
	if (!NOCORRELATIONS){
		for (int ipaty=0;ipaty<NPairTypes;ipaty++){
			long ntot = R[ipaty]->GetNMixedPairs_total();
			long nsame = R[ipaty]->GetNMixedPairs_sameTF();
			long nneigh = R[ipaty]->GetNMixedPairs_neighborTF();
			cout<<"sec 18.20 -- pairtype "<<ipaty<<": mixed pairs total="<<ntot
				<<" sameTF="<<nsame<<Form(" (%.3f%%)",ntot>0?100.0*nsame/ntot:0.0)
				<<" neighborTF="<<nneigh<<Form(" (%.3f%%)",ntot>0?100.0*nneigh/ntot:0.0)<<endl;
		}
	}
	cout<<"Writing root output file "<<RootFileName.Data()<<endl;
	fout->cd();
	fout->Write();
	//
	if (!NOCORRELATIONS){
		for (int ipaty=0;ipaty<NPairTypes;ipaty++){	
			//
			hzvtx[ipaty]			->Write();
			hmult[ipaty]			->Write();
			hmult1[ipaty]			->Write();
			hmult2[ipaty]			->Write();
			hy1[ipaty]				->Write();
			hy2[ipaty]				->Write();
			for (int ir2=0;ir2<NR2TYPES;ir2++){
				hrho2[ir2][ipaty]	->Write();		// uncorrected 
				hrho1rho1[ir2][ipaty]->Write();		// uncorrected 
				hC2[ir2][ipaty]		->Write();		// uncorrected C2
				hR2[ir2][ipaty]		->Write();		// uncorrected R2
			}
			hR2yydy[ipaty]			->Write();
			hR2dy[ipaty]			->Write();
			hR2dphi[ipaty]			->Write();
			hMinv_S[ipaty]			->Write();
			hMinv_M[ipaty]			->Write();
			hMinv[ipaty]			->Write();
			hCQ[ipaty]				->Write();
			hQsib[ipaty]			->Write();
			hQmix[ipaty]			->Write();
			hCQKT[ipaty]			->Write();
			hQsibKT[ipaty]			->Write();
			hQmixKT[ipaty]			->Write();
			hzoomS[ipaty]			->Write();
			hrho2C[ipaty]			->Write();		// crossing-corrected (dy,dphi)
			hC2C[ipaty]				->Write();		// crossing-corrected (dy,dphi)
			hR2C[ipaty]				->Write();		// crossing-corrected (dy,dphi)
			hMempty[ipaty]			->Write();		// empty-denominator diagnostic
			//---- README_Finalize: per-zvtx-bin ingredients for combining chunks. rho2(S), rho2(M) for
			//---- (y1,y2), (dy,dphi), (dy,dq) and hmult, per zvtx bin; plus nevt (hmult entries) and the
			//---- rho2(M) normalization (denomfactor_izv; 0 = zvtx bin skipped in Calculate) vs zvtx bin.
			{
				int nzv	= R[ipaty]->GetZVTXNB();
				TString part1name	= TString(ParticleIDNames[PairTypes_Info[ipaty][0]]);
				TString part2name	= TString(ParticleIDNames[PairTypes_Info[ipaty][1]]);
				TH1D *hnz	= new TH1D(Form("hnevtz_%d",ipaty) ,Form("%s%s, nevt (hmult entries) per zvtx bin;zvtx bin;events"   ,part1name.Data(),part2name.Data()),nzv,-0.5,nzv-0.5);
				TH1D *hdz	= new TH1D(Form("hdenomz_%d",ipaty),Form("%s%s, #rho_{2}(M) normalization per zvtx bin;zvtx bin;denomfactor",part1name.Data(),part2name.Data()),nzv,-0.5,nzv-0.5);
				for (int izv=0;izv<nzv;izv++){
					TH2D *hmz	= R[ipaty]->Gethmult(izv);
					hnz->SetBinContent(izv+1, hmz->GetEntries());
					hdz->SetBinContent(izv+1, R[ipaty]->GetDenomZ(izv));
					hmz->SetName(Form("hmult_z%02d_%d",izv,ipaty));
					hmz->Write();
					for (int ir2=0;ir2<NR2TYPES;ir2++){
						TH2D *hs	= R[ipaty]->Gethrho2_S(ir2,izv);
						TH2D *hm	= R[ipaty]->Gethrho2_M(ir2,izv);
						hs->SetName(Form("hrho2_%d_z%02d_%d"    ,ir2,izv,ipaty));
						hm->SetName(Form("hrho1rho1_%d_z%02d_%d",ir2,izv,ipaty));
						hs->Write();
						hm->Write();
					}
					//---- README_CQ: C(Q) ingredients per zvtx bin. hQsib is raw counts; hQmix is already
					//---- divided by denomfactor_izv (= hdenomz), exactly what CalcRm::Calculate divides.
					TH1D *hqs	= R[ipaty]->GethQsib(izv);
					TH1D *hqm	= R[ipaty]->GethQmix(izv);
					TH2D *hks	= R[ipaty]->GethQsibKT(izv);
					TH2D *hkm	= R[ipaty]->GethQmixKT(izv);
					hqs->SetName(Form("hQsib_z%02d_%d"  ,izv,ipaty));
					hqm->SetName(Form("hQmix_z%02d_%d"  ,izv,ipaty));
					hks->SetName(Form("hQsibKT_z%02d_%d",izv,ipaty));
					hkm->SetName(Form("hQmixKT_z%02d_%d",izv,ipaty));
					hqs->Write();
					hqm->Write();
					hks->Write();
					hkm->Write();
				}
				hnz->Write();
				hdz->Write();
			}
			hzoomM[ipaty]			->Write();
			hMinvFS[ipaty]			->Write();
			hMinvFM[ipaty]			->Write();
			//
		}	// end ipaty...
	}	// end nocorrelations
	//
	fout->Close();	
	//
	cout<<"done."<<endl;
	//
}

//---------------------------------------------------------------------------
bool corral::AcceptEvent(){
	if (      (*vtxx)  < -0.5 ) return false;
	if (      (*vtxx)  >  0.5 ) return false;
	if (      (*vtxy)  < -0.5 ) return false;
	if (      (*vtxy)  >  0.5 ) return false;
	if ( fabs((*vtxz)) > 16.  ) return false;
	return true;
}
//---------------------------------------------------------------------------
//------------------------------------------------------------
// sec 18.22: cross-crossing duplicate-track pre-pass (doXTFClean). Separate chain over the same
// files (same order as fChain, so chain entry == jentry in Loop), only the branches needed.
// Rows of one TF are consecutive (sec 18.20). Within one TF, siclukey IS unique per physical
// cluster (sec 18.11's aliasing is only BETWEEN events), so tracks in different rows sharing
// >=2 Si slots are copies of one silicon seed attached to different TPC seeds (matcher, sec
// 18.22). Loser = the copy whose row crossing != its own INTT time bucket
// ((cluskey>>32 & 0x3FF) - 200, INTT slots 3-6 only); if that doesn't decide, worse (higher)
// quality; if still tied, the later row. Exactly one copy of each pair survives.
void corral::BuildXTFLosers(Long64_t nentries){
	TChain *ch	= new TChain(fChain->GetName());
	TIter next(fChain->GetListOfFiles());
	while (TObject *el = next()) ch->Add(el->GetTitle());
	TTreeReader r(ch);
	TTreeReaderValue<Int_t>		r_run(r,"run"), r_evt(r,"evt"), r_xing(r,"crossing"), r_ntr(r,"ntr");
	TTreeReaderArray<Float_t>	r_pt(r,"pt"), r_dcaxy(r,"dcaxy"), r_dcaz(r,"dcaz"), r_quality(r,"quality");
	TTreeReaderArray<UChar_t>	r_ntpc(r,"ntpc");
	TTreeReaderArray<ULong64_t>	r_key(r,"siclukey");
	struct T { Long64_t ent; int idx; int xing; int intt; float q; ULong64_t k[7]; };
	std::vector<T> tf;
	auto inttXing = [](const ULong64_t *k){		// -9999 = no INTT cluster, -8888 = INTT clusters disagree
		int x=-9999;
		for (int s=3;s<7;s++){ if (k[s]==~0ULL) continue; int b=(int)((k[s]>>32)&0x3FF)-200;
			if (x==-9999) x=b; else if (b!=x) return -8888; }
		return x; };
	auto flush = [&](){
		for (size_t a=0;a<tf.size();a++) for (size_t b=a+1;b<tf.size();b++){
			if (tf[a].ent==tf[b].ent) continue;					// same row: LS/ULS paths handle it
			int ns=0; for (int s=0;s<7;s++) if (tf[a].k[s]!=~0ULL && tf[a].k[s]==tf[b].k[s]) ++ns;
			if (ns<2) continue;
			++nXTF_pairs;
			bool aok = (tf[a].intt==tf[a].xing), bok = (tf[b].intt==tf[b].xing);
			bool neither = !aok && !bok && tf[a].intt>-8000 && tf[b].intt>-8000;	// both have consistent INTT, both off
			if (neither) ++nXTF_neither;
			std::vector<const T*> los;
			if (aok!=bok){ los.push_back(aok ? &tf[b] : &tf[a]); ++nXTF_byINTT; }
			else if (neither && doXTFStrict){ los.push_back(&tf[a]); los.push_back(&tf[b]); }
			else { los.push_back((tf[a].q>tf[b].q) ? &tf[a] : &tf[b]); ++nXTF_byQuality; }	// tie -> later row (b)
			for (const T *l : los){
				std::vector<int> &v = xtfLosers[l->ent];
				if (std::find(v.begin(),v.end(),l->idx)==v.end()){ v.push_back(l->idx); ++nXTF_losers; }
			}
		}
		tf.clear(); };
	int pr=-1, pe=-1;
	cout<<"BuildXTFLosers -- pre-pass over "<<nentries<<" rows..."<<endl;
	for (Long64_t je=0; je<nentries && r.Next(); je++){
		if (je%5000000==0) cout<<"BuildXTFLosers -- "<<je<<endl;
		if ((*r_run)!=pr || (*r_evt)!=pe){ flush(); pr=(*r_run); pe=(*r_evt); }
		for (int it=0; it<(*r_ntr); it++){
			//---- same cuts as AcceptTrack() below -- keep in sync
			if (r_pt[it]<0.1 || r_pt[it]>20.1 || r_ntpc[it]<NTPCCUT
			 || fabs(r_dcaxy[it])>1.5 || fabs(r_dcaz[it])>1.5) continue;
			T t; t.ent=je; t.idx=it; t.xing=(*r_xing); t.q=r_quality[it];
			for (int s=0;s<7;s++) t.k[s]=r_key[it*7+s];
			t.intt=inttXing(t.k);
			tf.push_back(t);
		}
	}
	flush();
	delete ch;
	cout<<"BuildXTFLosers -- duplicate pairs="<<nXTF_pairs<<" (decided by INTT="<<nXTF_byINTT
		<<", by quality="<<nXTF_byQuality<<", neither="<<nXTF_neither<<(doXTFStrict?" [both dropped]":" [better quality kept]")<<") losers="<<nXTF_losers<<" in "<<xtfLosers.size()<<" rows"<<endl;
}

//------------------------------------------------------------
// Overlapping-TF collision copies (doTFDup). Separate chain over the same files, same order as
// fChain (chain entry == jentry in Loop), like BuildXTFLosers. When two triggers are closer in
// time than a TF's readout window (~540 crossings), both TFs reconstruct the collisions in the
// overlap: same vertex, crossing shifted by exactly the GL1 BCO difference of the two TFs
// (checked on ana573 79515 seg 0: 5 of 5 shared vertices of evt 45/46 shifted by dBCO=62).
// So bco+crossing is the absolute crossing, and (run, bco+crossing) identifies the bunch crossing.
// Smoke test (79515 seg 99-100): 11.9% of rows are copies, dEvt 1 (94%) and 2 (6%) - so the
// adjacent-TF mixing exclusion (|devt|==1) alone would miss some; 98.6% of copies agree in vtxz
// to <1 mm, 0.7% differ by >1 cm (in-bunch pileup: each TF kept a different one of the crossing's
// vertices - Collect keeps one vertex per crossing - but the rows hold the same crossing's tracks).
// First row with a key is kept (the earlier TF in chain order - neither copy is systematically
// better, ntr more/fewer/equal 30/31/38 on that test), later rows with the key go in tfDupRows.
// Rows with bco==~0 (TF without GL1RAWHIT) are never skipped. No bco branch -> nothing to do.
void corral::BuildTFDupRows(Long64_t nentries){
	TChain *ch	= new TChain(fChain->GetName());
	TIter next(fChain->GetListOfFiles());
	while (TObject *el = next()) ch->Add(el->GetTitle());
	ch->LoadTree(0);
	if (!ch->GetBranch("bco")){
		cout<<"BuildTFDupRows -- no bco branch in these trees: overlapping-TF duplicate removal not possible"<<endl;
		delete ch;
		return;
	}
	TTreeReader r(ch);
	TTreeReaderValue<Int_t>		r_run(r,"run"), r_evt(r,"evt"), r_xing(r,"crossing");
	TTreeReaderValue<ULong64_t>	r_bco(r,"bco");
	TTreeReaderValue<Double_t>	r_vtxz(r,"vtxz");
	//---- Copies are only ever a few TFs apart (dEvt<=3 seen), so only keys from the last TFDUP_WINDOW
	//---- TFs of the current run are kept - memory stays small even for the full-statistics job
	//---- (all chunks, ~1e9 rows). Rows arrive in TF order within a run (chunks are contiguous).
	const int TFDUP_WINDOW = 20;
	struct Kept { int evt; double vz; };
	std::unordered_map<Long64_t,Kept> seen;			// absolute crossing -> first row, current run only
	std::deque<std::pair<int,Long64_t>> order;		// (evt, key) in insertion order, for pruning
	int currun = -1;
	tfDupRows.assign(nentries, false);
	cout<<"BuildTFDupRows -- pre-pass over "<<nentries<<" rows..."<<endl;
	for (Long64_t je=0; je<nentries && r.Next(); je++){
		if ((*r_run)!=currun){ seen.clear(); order.clear(); currun = (*r_run); }
		while (!order.empty() && order.front().first < (*r_evt) - TFDUP_WINDOW){ seen.erase(order.front().second); order.pop_front(); }
		if ((*r_bco)==~0ULL) continue;
		Long64_t key = (Long64_t)(*r_bco) + (*r_xing);
		auto it = seen.find(key);
		if (it==seen.end()){ seen[key] = Kept{(*r_evt),(*r_vtxz)}; order.emplace_back((*r_evt),key); continue; }
		tfDupRows[je] = true;
		++nTFDup_rows;
		++nTFDup_byDevt[(*r_evt) - it->second.evt];
		TFDup_maxAbsDvz = std::max(TFDup_maxAbsDvz, fabs((*r_vtxz) - it->second.vz));
	}
	delete ch;
	cout<<"BuildTFDupRows -- rows skipped="<<nTFDup_rows<<" of "<<nentries<<", max |dvtxz| between copies="<<TFDup_maxAbsDvz<<" cm; by evt(copy)-evt(kept):";
	for (auto &p : nTFDup_byDevt) cout<<" "<<p.first<<":"<<p.second;
	cout<<endl;
}

bool corral::AcceptTrack(int it){
	//if ( !primary[it]              ) return false;
	if (  pt[it]         <     0.1 ) return false;
	if (  pt[it]         >    20.1 ) return false;
	if (  ntpc[it]       < NTPCCUT ) return false;
	if ( fabs(dcaxy[it]) >     1.5 ) return false;
	if ( fabs(dcaz[it])  >     1.5 ) return false;
	return true;
}







