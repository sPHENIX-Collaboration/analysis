
#include <TEllipse.h>
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
#include <TBox.h>
#include <TPaveText.h>
#include <TLegendEntry.h>
#include <TCanvas.h>
#include <TProfile.h>
#include <TSystem.h>

#include "fluct_common.h"
#include "CalcRm.h"
//#include "CalcR3mid.h"
#include "PairTypes.h"
#include "finalize_hists.h"	// CorralDir(): where dedxGates_KFP.root lives

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
	const double mpi = 0.13957, mK0 = 0.497611, mL = 1.115683, mp = 0.938272, mK = 0.493677;		// PDG
	const int PIP = kParticleIDPionPlus, PIM = kParticleIDPionMinus;
	const int PP = kParticleIDProton, PM = kParticleIDAntiProton, KP = kParticleIDKaonPlus, KM = kParticleIDKaonMinus;
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
		{ PP,  PIM, 1.115683, mp,  mpi, "#Lambda"          },		// Lambdas not reconstructed as V0s (README_PID.md)
		{ PP,  PIM, 1.232,    mp,  mpi, "#Delta^{0}"       },
		{ PP,  PIP, 1.232,    mp,  mpi, "#Delta^{++}"      },
		{ PM,  PIP, 1.115683, mp,  mpi, "#bar{#Lambda}"    },
		{ PM,  PIP, 1.232,    mp,  mpi, "#bar{#Delta}^{0}" },
		{ PM,  PIM, 1.232,    mp,  mpi, "#bar{#Delta}^{--}"},
		{ KP,  KM,  1.019461, mK,  mK,  "#phi"             },
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
	long nv0fixEta=0,nv0fixPhi=0,nv0fixMass=0,nv0fixCtau=0;
	long nv0Paired[3]={0,0,0}, nv0All[3]={0,0,0}, nPeakDauNotIndv0=0;	// README_PID.md sec 12: V0s in the pairs, daughters indv0 missed	// README_v0etaSpike.md: V0s whose eta/phi/mass/ctau branch was a 0 sentinel, recomputed
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
	//---- README_PID.md (2026-09-28): KFP dE/dx gates (dedxGates_KFP.root, README_dedxGates.md), g_<sp>_lo/_hi
	//---- vs |p| (GeV/c). Applied to dedxKFP (what KFP itself cuts on), not dedx70s. Undefined outside
	//---- 0.1<|p|<5 GeV/c -> unidentified there. Order pi, then p, then K: a track takes the first gate it is in.
	const int NGATE				= 4;					// 0 pi, 1 K, 2 p, 3 d (d: QA overlay only)
	const char* gateSp[NGATE]	= {"pi","K","p","d"};
	TGraph *gGateLo[NGATE]		= {0}, *gGateHi[NGATE] = {0};
	const double GATE_PMIN		= 0.1, GATE_PMAX = 5.0;
	if (!doOldPID){
		TString gname	= CorralDir() + "dedxGates_KFP.root";
		TDirectory *dsave	= gDirectory;
		TFile *fgate	= TFile::Open(gname.Data(),"READ");
		if (!fgate || fgate->IsZombie()){ cout<<"corral::Loop -- cannot open dE/dx gates "<<gname<<" (\"oldpid\" runs without them), exit"<<endl; exit(1); }
		for (int ig=0;ig<NGATE;ig++){
			TGraph *lo	= (TGraph*)fgate->Get(Form("g_%s_lo",gateSp[ig]));
			TGraph *hi	= (TGraph*)fgate->Get(Form("g_%s_hi",gateSp[ig]));
			if (!lo || !hi){ cout<<"corral::Loop -- "<<gname<<" has no g_"<<gateSp[ig]<<"_lo/_hi, exit"<<endl; exit(1); }
			gGateLo[ig]	= (TGraph*)lo->Clone(Form("gGateLo_%s",gateSp[ig]));
			gGateHi[ig]	= (TGraph*)hi->Clone(Form("gGateHi_%s",gateSp[ig]));
		}
		fgate->Close(); delete fgate;
		dsave->cd();
		cout<<"corral::Loop -- PID: KFP dE/dx gates from "<<gname<<" (pi, then p, then K, on dedxKFP, "<<GATE_PMIN<<"<p<"<<GATE_PMAX<<")"<<endl;
	} else {
		cout<<"corral::Loop -- PID: legacy (oldpid) -- pi = dedx70s<400 at any momentum, no p or K"<<endl;
	}
	auto inGate	= [&](int ig, double p, double dedx){
		return dedx>=gGateLo[ig]->Eval(p) && dedx<=gGateHi[ig]->Eval(p);
	};
	//---- README_PID.md sec 11: is V0 iv0 in its mass peak? (v0mass can be an exact-0 sentinel: rebuild it from E, p)
	auto v0InPeak	= [&](int iv0){
		double m	= v0mass[iv0];
		if (m==0.0){ double m2 = v0ene[iv0]*v0ene[iv0] - v0ptot[iv0]*v0ptot[iv0]; m = (m2>0.) ? sqrt(m2) : 0.; }
		int k	= (v0pid[iv0]==310) ? 0 : (v0pid[iv0]==3122) ? 1 : (v0pid[iv0]==-3122) ? 2 : -1;
		if (k<0) return true;
		return fabs(m-V0PEAK_MU[k]) < V0PEAK_NSIG*V0PEAK_SIG[k];
	};
	//---- PID of track it: 0 pi, 1 K, 2 p, 3 unidentified (README_PID.md sec 1)
	auto pidOf	= [&](int it){
		int k	= 3;
		if (doOldPID){
			if (dedx70s[it]<400.) k = 0;
		} else if (ptot[it]>GATE_PMIN && ptot[it]<GATE_PMAX && dedxKFP[it]>0.){
			double pp	= ptot[it];											// each species only below its ptot cap (fluct_common.h)
			if		(pp<Species_pmax_pid[0] && inGate(0,pp,dedxKFP[it])) k = 0;		// pions first,
			else if	(pp<Species_pmax_pid[2] && inGate(2,pp,dedxKFP[it])) k = 2;		// then protons,
			else if	(pp<Species_pmax_pid[1] && inGate(1,pp,dedxKFP[it])) k = 1;		// then kaons
		}
		return k;
	};
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
	//---- output base name (2026-09-27): "corral" (was "corral_m", a holdover from the removed convolution
	//---- class); "corral_x" in the NOCORRELATIONS build. Older output files keep their corral_m names.
	TString ClassStr	= NOCORRELATIONS ? TString("corral_x") : TString("corral");
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
	//---- 2026-09-30 (user): acceptance = the eta fiducial abs(eta) < 1.1 (the detector edge) and, per species, the rapidity
	//---- window abs(y) < Species_yu (fluct_common.h: pi 1.0, K 0.65, p 0.6, K0s/Lambda/Lbar 0.7). Binning per pair type
	//---- (PairTypes_YBins, PairTypes.h): y bins across each window (the two widths need not match) and the dy bin width.
	double pairEtaEdge[NPairTypes], pairYL1[NPairTypes], pairYU1[NPairTypes], pairYL2[NPairTypes], pairYU2[NPairTypes], pairDYBW[NPairTypes];
	int pairYNB1[NPairTypes], pairYNB2[NPairTypes];
	for (int ip=0;ip<NPairTypes;ip++){
		pairEtaEdge[ip]	= thisYU;
		pairYU1[ip]		= Species_yu[GetSpecies(PairTypes_Info[ip][0])]; pairYL1[ip] = -pairYU1[ip]; pairYNB1[ip] = (int)PairTypes_YBins[ip][0];
		pairYU2[ip]		= Species_yu[GetSpecies(PairTypes_Info[ip][1])]; pairYL2[ip] = -pairYU2[ip]; pairYNB2[ip] = (int)PairTypes_YBins[ip][1];
		pairDYBW[ip]	= PairTypes_YBins[ip][2];
		cout<<"corral::Loop -- pair type "<<ip<<" "<<ParticleIDNames[PairTypes_Info[ip][0]]<<" "<<ParticleIDNames[PairTypes_Info[ip][1]]
			<<": abs(eta) < "<<pairEtaEdge[ip]<<", abs(y1) < "<<pairYU1[ip]<<" ("<<pairYNB1[ip]<<" bins of "<<2.*pairYU1[ip]/pairYNB1[ip]
			<<"), abs(y2) < "<<pairYU2[ip]<<" ("<<pairYNB2[ip]<<" bins of "<<2.*pairYU2[ip]/pairYNB2[ip]<<"), dy bin width "<<pairDYBW[ip]<<endl;
	}
	if (!doOldPID) cout<<"corral::Loop -- PID ptot caps: pi "<<Species_pmax_pid[0]<<", p "<<Species_pmax_pid[2]<<", K "<<Species_pmax_pid[1]<<" GeV/c"<<endl;
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
			R[ipaty]	->SetTrackSep(valTSepDy,valTSepDphi,true);
			R[ipaty]	->SetInttSep(valISepDphi);		// README_SplitTracks573 sec 29 (off if <= 0)	// min over R (the R = 0.5 m variant is dropped)	// README_SplitTracks573 sec 11 (off if valTSepDy<=0)
			R[ipaty]	->SetQcut( valQCut);		// set Q cut (a lower limit)
			//R[ipaty]	->SetDoMinvCut( doMcut);	// Minv cut only (mixing num & denom, convolution num only)
			//R[ipaty]	->SetDoMinvLLCut(false);	// Minv LL cut only (mixing num & denom, convolution num only)
			R[ipaty]	->SetPid1(ipid1);			//
			R[ipaty]	->SetPid2(ipid2);			//
			R[ipaty]	->SetYNB1(pairYNB1[ipaty]);		// 2026-09-30: per-species y windows, bins per pair type
			R[ipaty]	->SetYL1( pairYL1[ipaty] );
			R[ipaty]	->SetYU1( pairYU1[ipaty] );
			R[ipaty]	->SetYNB2(pairYNB2[ipaty]);
			R[ipaty]	->SetYL2( pairYL2[ipaty] );
			R[ipaty]	->SetYU2( pairYU2[ipaty] );
			R[ipaty]	->SetDYBW(pairDYBW[ipaty]);		// 2026-09-30: dy width per pair type
			R[ipaty]	->SetPHINB(thisPHINB);
			R[ipaty]	->SetPTL1( thisPTL  );
			R[ipaty]	->SetPTU1( thisPTU  );
			R[ipaty]	->SetPTL2( thisPTL  );
			R[ipaty]	->SetPTU2( thisPTU  );
			R[ipaty]	->SetZVTXNB(valZvtxNB);	// 16 (sec 38)
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
		OutputFileBase	= TString(Form("%s_%s",ClassStr.Data(),RunString.Data()));
	} else {
		OutputFileBase	= ClassStr;
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
	//---- README_SplitTracks573 sec 6 (2026-09-27): DIAGNOSTIC ONLY, no cut reads these. Same-charge
	//---- pion pairs near (0,0), split by charge (0: ++, 1: --) and pair pT class (0: min pt < 0.2,
	//---- 1: min pt >= 0.2). "surv" = both tracks survive every pre-pass removal (XTF, LS, ULS).
	//---- central = |deta|<0.034 && |dphi|<10deg (the hR2_1 central bin), side = |deta|<0.034 && 10<=|dphi|<20deg.
	//---- outcome, pairs in central with SKF >= valSiKeyCut: 0 removed (one track flagged), 1 surv, outside
	//---- the pregate ellipse, 2 surv, in ellipse, SL and RG both fail, 3 surv, in ellipse, flagged but tied
	//---- (no loser), 4 surv, in ellipse, other (e.g. doSplitRemoval off).
	TH1D *hD573_SKF_surv_cen[2][2], *hD573_SKF_surv_side[2][2], *hD573_outcome[2][2], *hD573_SL_survInEll[2][2];
	TH2D *hD573_pos_survHi[2][2], *hD573_pos_allHi[2][2], *hD573_dphiPtinv_allHi[2], *hD573_dphiDinvpt_allHi[2], *hD573_dphiDinvpt_survHi[2];
	//---- README_SplitTracks573 sec 10: LS pregate visualization (one PDF page, LS pions only, both charges).
	//---- dphi(deg) vs deta in bins of abs(1/pt1-1/pt2): all LS pion pairs / SKF>=cut pairs / pairs whose two
	//---- tracks both survive every pre-pass removal.
	static const int NGATEVIS	= 6;
	static const double GATEVIS_EDGE[NGATEVIS+1]	= {0.,1.,2.,3.,4.,6.,10.};
	TH2D *hGateVis_all[NGATEVIS], *hGateVis_hi[NGATEVIS], *hGateVis_surv[NGATEVIS];
	//---- the same SKF>=cut / kept pairs in the dps gate's own frame: abs(dphi) - dphi0 (each pair's own
	//---- dphi0 = DPsCentre, R = valDPsR), where the dps gate is ONE fixed ellipse centred at 0 (user, sec 10)
	TH2D *hGateVis_hiRes[NGATEVIS], *hGateVis_survRes[NGATEVIS];
	TH1D *hGateVis_dinvHi[NGATEVIS];
	//---- user 2026-09-28: the same populations in FINE slices of each pair's own dphi0 (= DPsCentre, the gate's
	//---- centre), plotted as abs(dphi) vs deta: inside a slice every pair's gate is the drawn ellipse to within
	//---- half the slice width, so the red curve is the cut Corral applies (three extra PDF pages after the gate page).
	static const int NGATEFINE	= 16;
	static const double GATEFINE_W	= 0.4;		// slice width in dphi0 (deg): slices 0-0.4, ..., 6.0-6.4 (dphi0 < ~6.3 at pt > 0.1)
	TH2D *hGateFine_hi[NGATEFINE], *hGateFine_hiSurv[NGATEFINE], *hGateFine_surv[NGATEFINE];
	//---- user 2026-09-28: gate page row 1 (all LS pairs, signed dphi, index order) in the same fine dphi0 slices
	TH2D *hGateFine_all[NGATEFINE];
	for (int k=0;k<NGATEFINE;k++){
		const char* rng	= Form("%.1f#leq#Delta#phi_{0}<%.1f#circ",k*GATEFINE_W,(k+1)*GATEFINE_W);
		hGateFine_all[k]	= new TH2D(Form("hGateFine_all_%d",k),Form("all, %s;#Delta#eta;#Delta#phi (deg)",rng),64,-0.08,0.08,192,-12.,12.);
		hGateFine_hi[k]		= new TH2D(Form("hGateFine_hi_%d",k),Form("SKF#geqcut, %s;#Delta#eta;|#Delta#phi| (deg)",rng),64,-0.08,0.08,96,0.,12.);
		hGateFine_hiSurv[k]	= new TH2D(Form("hGateFine_hiSurv_%d",k),Form("SKF#geqcut, both kept, %s;#Delta#eta;|#Delta#phi| (deg)",rng),64,-0.08,0.08,96,0.,12.);
		hGateFine_surv[k]	= new TH2D(Form("hGateFine_surv_%d",k),Form("all kept, %s;#Delta#eta;|#Delta#phi| (deg)",rng),64,-0.08,0.08,96,0.,12.);
	}
	//---- README_SplitTracks573 sec 25: cosmic-ray test of the pi+pi- away-side spike at dy ~ 0 (diagnostic only, no cut reads
	//---- these). Accepted, surviving pions; pairs with abs(dy) < 0.1 in three windows: [0] opposite charge, abs(dphi) >= 170 deg
	//---- (the spike); [1] opposite charge, 150 <= abs(dphi) < 160 (control); [2] same charge, abs(dphi) >= 170 (control).
	//---- A cosmic muon through the TPC, reconstructed as two tracks out of the vertex: opposite charges, pt1 ~ pt2,
	//---- a near-vertical axis (phi ~ +-90 deg), eta ~ 0, mirrored DCAs, few other tracks.
	//---- README_SplitTracks573 sec 38: the central-membrane (CM) hole in R2(y1,y2) (diagnostic only, no cut reads these).
	//---- Surviving accepted charged tracks, abs(vtxz) < 8 cm. w = (eta + CM_K vtxz) * (vtxz<0 ? +1 : -1): the hole centre at
	//---- w ~ 0 in every slice (hetazvtx: minimum at eta ~ -0.018 vtxz), the sharp edge at w < 0, the slow recovery at w > 0.
	//---- Pairs with both tracks in the band (CM_BLO < w < CM_BHI, [0]) or both in the control (CM_CLO < abs(w) < CM_CHI, [1]);
	//---- second index 0 = opposite charge, 1 = same charge. A particle crossing the CM rebuilt as two TPC pieces: dphi ~ deta ~ 0,
	//---- short ntpc each, inner / outer TPC layers, shared silicon.
	static const double CM_K = 0.018, CM_BLO = -0.03, CM_BHI = 0.05, CM_CLO = 0.20, CM_CHI = 0.35;
	TH1D *hCM_w		= new TH1D("hCM_w","CM study: surviving tracks, abs(vtxz)<8;w = (#eta + 0.018 z_{vtx}) sign(-z_{vtx});tracks",200,-0.5,0.5);
	TH2D *hCM_w_ntpc	= new TH2D("hCM_w_ntpc","CM study: ntpc vs w;w;ntpc",100,-0.5,0.5,50,0.,50.);
	TH2D *hCM_w_xing	= new TH2D("hCM_w_xing","CM study: w vs the event's crossing;w;crossing",100,-0.5,0.5,24,-100.,500.);
	TH2D *hCM_nbnc	= new TH2D("hCM_nbnc","CM study: per event, tracks in the band vs in the control;n_{control};n_{band}",40,0.,40.,10,0.,10.);	// event-by-event hole depth
	TH2D *hCM_w_tpcfl[2];		// first vs last TPC layer (0-47) of tracks in the band [0] / control [1]
	TH2D *hCM_dphideta[2][2], *hCM_ntpc12[2][2], *hCM_nsi12[2][2], *hCM_pt12[2][2];
	TH1D *hCM_ntpcsum[2][2], *hCM_skf[2][2], *hCM_dw[2][2];
	{
		const char* cn[2]	= {"band","control"};
		const char* qn[2]	= {"OS","SS"};
		for (int c=0;c<2;c++){
			hCM_w_tpcfl[c]	= new TH2D(Form("hCM_w_tpcfl_%d",c),Form("CM study, %s tracks;first TPC layer;last TPC layer",cn[c]),48,0.,48.,48,0.,48.);
			for (int q=0;q<2;q++){
				hCM_dphideta[c][q]	= new TH2D(Form("hCM_dphideta_%d_%d",c,q),Form("CM study, %s %s pairs;#Delta#eta;#Delta#phi (deg)",cn[c],qn[q]),80,-0.2,0.2,120,-30.,30.);
				hCM_ntpc12[c][q]	= new TH2D(Form("hCM_ntpc12_%d_%d",c,q),Form("CM study, %s %s pairs;ntpc_{1};ntpc_{2}",cn[c],qn[q]),50,0.,50.,50,0.,50.);
				hCM_nsi12[c][q]		= new TH2D(Form("hCM_nsi12_%d_%d",c,q),Form("CM study, %s %s pairs;n_{Si,1};n_{Si,2}",cn[c],qn[q]),8,0.,8.,8,0.,8.);
				hCM_pt12[c][q]		= new TH2D(Form("hCM_pt12_%d_%d",c,q),Form("CM study, %s %s pairs;p_{T,1};p_{T,2}",cn[c],qn[q]),40,0.,2.,40,0.,2.);
				hCM_ntpcsum[c][q]	= new TH1D(Form("hCM_ntpcsum_%d_%d",c,q),Form("CM study, %s %s pairs;ntpc_{1}+ntpc_{2};pairs",cn[c],qn[q]),100,0.,100.);
				hCM_skf[c][q]		= new TH1D(Form("hCM_skf_%d_%d",c,q),Form("CM study, %s %s pairs;SiSplitScore;pairs",cn[c],qn[q]),50,0.,1.0001);
				hCM_dw[c][q]		= new TH1D(Form("hCM_dw_%d_%d",c,q),Form("CM study, %s %s pairs;w_{1}-w_{2};pairs",cn[c],qn[q]),80,-0.2,0.2);
			}
		}
	}
	static const int NCOS	= 3;
	TH1D *hCos_psum[NCOS], *hCos_minv[NCOS], *hCos_axisBB[NCOS], *hCos_minvBB[NCOS];
	TH1D *hCos_axis[NCOS], *hCos_ptAsym[NCOS], *hCos_eta[NCOS], *hCos_npi[NCOS], *hCos_vtxntr[NCOS], *hCos_pt[NCOS], *hCos_etotoh[NCOS];
	TH2D *hCos_dcaxy[NCOS], *hCos_dcaz[NCOS];
	{ const char* wn[NCOS]	= {"ULS, |d#phi|#geq170#circ","ULS, 150#leq|d#phi|<160#circ","LS, |d#phi|#geq170#circ"};
	  for (int w=0;w<NCOS;w++){
		hCos_axis[w]	= new TH1D(Form("hCos_axis_%d",w),Form("%s, |dy|<0.1;#phi of the pair axis, folded to [0,180) (deg)",wn[w]),36,0.,180.);
		hCos_ptAsym[w]	= new TH1D(Form("hCos_ptAsym_%d",w),Form("%s, |dy|<0.1;(p_{T1}-p_{T2})/(p_{T1}+p_{T2})",wn[w]),50,-1.,1.);
		hCos_eta[w]		= new TH1D(Form("hCos_eta_%d",w),Form("%s, |dy|<0.1;#eta_{1}",wn[w]),44,-1.1,1.1);
		hCos_npi[w]		= new TH1D(Form("hCos_npi_%d",w),Form("%s, |dy|<0.1;accepted pions in the event",wn[w]),40,-0.5,39.5);
		hCos_vtxntr[w]	= new TH1D(Form("hCos_vtxntr_%d",w),Form("%s, |dy|<0.1;vtxntr",wn[w]),60,-0.5,59.5);
		hCos_pt[w]		= new TH1D(Form("hCos_pt_%d",w),Form("%s, |dy|<0.1;mean p_{T} of the pair",wn[w]),40,0.,4.);
		hCos_etotoh[w]	= new TH1D(Form("hCos_etotoh_%d",w),Form("%s, |dy|<0.1;etotoh",wn[w]),50,0.,25.);
		hCos_dcaxy[w]	= new TH2D(Form("hCos_dcaxy_%d",w),Form("%s, |dy|<0.1;dcaxy_{1};dcaxy_{2}",wn[w]),60,-0.3,0.3,60,-0.3,0.3);
		hCos_dcaz[w]	= new TH2D(Form("hCos_dcaz_%d",w),Form("%s, |dy|<0.1;dcaz_{1};dcaz_{2}",wn[w]),60,-0.3,0.3,60,-0.3,0.3);
		//---- sec 25.1 (2026-09-29): p1 = -p2 pairs (cosmic halves or decays at rest): relative 3-momentum sum, Minv (pion
		//---- masses), and axis / Minv of the clean subset (relative sum < 0.05)
		hCos_psum[w]	= new TH1D(Form("hCos_psum_%d",w),Form("%s, |dy|<0.1;|#vec{p}_{1}+#vec{p}_{2}| / (|p_{1}|+|p_{2}|)",wn[w]),50,0.,0.5);
		hCos_minv[w]	= new TH1D(Form("hCos_minv_%d",w),Form("%s, |dy|<0.1;M_{inv}(#pi#pi) (GeV)",wn[w]),80,0.,4.);
		hCos_axisBB[w]	= new TH1D(Form("hCos_axisBB_%d",w),Form("%s, |dy|<0.1, rel. p sum < 0.05;#phi of the pair axis, folded to [0,180) (deg)",wn[w]),36,0.,180.);
		hCos_minvBB[w]	= new TH1D(Form("hCos_minvBB_%d",w),Form("%s, |dy|<0.1, rel. p sum < 0.05;M_{inv}(#pi#pi) (GeV)",wn[w]),80,0.,4.);
	  } }
	//---- README_SplitTracks573 sec 28: TPC sector-gap "spokes". Accepted charged tracks (the sec 6 list, before the
	//---- pre-pass removals), [charge 0:+ 1:-][0: vertex phi | 1: phi at R = 0.55 m, phi + q*asin(aR/pt), the sign the
	//---- data pick (sec 28)] vs pt. At the vertex a gap fixed in the detector is a curve phi_gap + q*asin(aR/pt) (a spoke bent
	//---- oppositely for the two charges); at R it is a vertical line.
	TH2D *hSpoke[2][2];
	for (int q=0;q<2;q++) for (int f=0;f<2;f++)
		hSpoke[q][f]	= new TH2D(Form("hSpoke_%d_%d",q,f),Form("%s, %s;%s (deg);p_{T} (GeV/c)",q?"negative":"positive",f?"#phi at R = 0.55 m":"vertex #phi",f?"#phi(R=0.55 m)":"#phi"),180,-180.,180.,28,0.1,1.5);
	//---- README_SplitTracks573 sec 15: residual and charge-asymmetry diagnostics (no cut reads these).
	//---- Silicon match code per pair: x = MVTX (denM*4 + numM: layers either / both-matching track has,
	//---- 0-15), y = INTT (denI*5 + numI, 0-24). For SKF>=cut LS pion pairs in the central bin:
	//---- removed / surviving in gate / at 0.04 <= abs(deta) < 0.05 (the tail seen on the gate page).
	TH2D *hD573_match[3];
	const char* matchName[3]	= {"removed","survInGate","detaTail"};
	for (int k=0;k<3;k++) hD573_match[k] = new TH2D(Form("hD573_match_%s",matchName[k]),Form("LS pairs, SKF#geqcut, %s;MVTX 4#timesden+num;INTT 5#timesden+num",matchName[k]),16,-0.5,15.5,25,-0.5,24.5);
	//---- sec 19: losers per removal path and charge (accepted pions only), vs pt. Path: 0 LS duplicate (SL),
	//---- 1 LS complementary (RadialGap), 2 LS MVTX match (path 3), 3 ULS veto, 4 XTF cleaner, 5 looper veto (sec 35).
	TH1D *hLoserPath_pt[6][2];
	{ const char* pn[6]={"dup","comp","mvtx","uls","xtf","loop"};
	  for (int k=0;k<6;k++) for (int c=0;c<2;c++) hLoserPath_pt[k][c] = new TH1D(Form("hLoserPath_pt_%s_%s",pn[k],c==0?"pos":"neg"),Form("losers, path %s, %s;p_{T}",pn[k],c==0?"#pi+":"#pi-"),40,0,2); }
	//---- per-charge accepted pions and LS-path losers vs pt, eta, phi, ntpc ([0] pi+, [1] pi-)
	TH1D *hAsym_all_pt[2], *hAsym_los_pt[2], *hAsym_all_eta[2], *hAsym_los_eta[2], *hAsym_all_phi[2], *hAsym_los_phi[2], *hAsym_all_ntpc[2], *hAsym_los_ntpc[2];
	for (int c=0;c<2;c++){
		const char* cn	= c==0 ? "pos" : "neg";
		hAsym_all_pt[c]		= new TH1D(Form("hAsym_all_pt_%s",cn),Form("accepted pions %s;p_{T}",cn),40,0,2);
		hAsym_los_pt[c]		= new TH1D(Form("hAsym_los_pt_%s",cn),Form("LS-path losers %s;p_{T}",cn),40,0,2);
		hAsym_all_eta[c]	= new TH1D(Form("hAsym_all_eta_%s",cn),Form("accepted pions %s;#eta",cn),22,-1.1,1.1);
		hAsym_los_eta[c]	= new TH1D(Form("hAsym_los_eta_%s",cn),Form("LS-path losers %s;#eta",cn),22,-1.1,1.1);
		hAsym_all_phi[c]	= new TH1D(Form("hAsym_all_phi_%s",cn),Form("accepted pions %s;#phi",cn),72,-M_PI,M_PI);
		hAsym_los_phi[c]	= new TH1D(Form("hAsym_los_phi_%s",cn),Form("LS-path losers %s;#phi",cn),72,-M_PI,M_PI);
		hAsym_all_ntpc[c]	= new TH1D(Form("hAsym_all_ntpc_%s",cn),Form("accepted pions %s;ntpc",cn),49,-0.5,48.5);
		hAsym_los_ntpc[c]	= new TH1D(Form("hAsym_los_ntpc_%s",cn),Form("LS-path losers %s;ntpc",cn),49,-0.5,48.5);
	}	// abs(1/pt1-1/pt2) of the SKF>=cut pairs per column: its mean places the row-1 ellipses
	for (int ib=0;ib<NGATEVIS;ib++){
		const char* rng	= Form("%.0f<|1/p_{T1}-1/p_{T2}|<%.0f",GATEVIS_EDGE[ib],GATEVIS_EDGE[ib+1]);
		hGateVis_all[ib]	= new TH2D(Form("hGateVis_all_%d",ib),Form("LS #pi pairs, all, %s;#Delta#eta;#Delta#phi (deg)",rng),50,-0.05,0.05,96,-12,12);
		hGateVis_hi[ib]		= new TH2D(Form("hGateVis_hi_%d",ib),Form("LS #pi pairs, SiSplitScore#geqcut, %s;#Delta#eta;#Delta#phi (deg)",rng),50,-0.05,0.05,96,-12,12);
		hGateVis_surv[ib]	= new TH2D(Form("hGateVis_surv_%d",ib),Form("LS #pi pairs, both tracks kept, %s;#Delta#eta;#Delta#phi (deg)",rng),50,-0.05,0.05,96,-12,12);
		hGateVis_dinvHi[ib]	= new TH1D(Form("hGateVis_dinvHi_%d",ib),Form("SiSplitScore#geqcut, %s;|1/p_{T1}-1/p_{T2}|",rng),100,GATEVIS_EDGE[ib],GATEVIS_EDGE[ib+1]);
		hGateVis_hiRes[ib]	= new TH2D(Form("hGateVis_hiRes_%d",ib),Form("SiSplitScore#geqcut, %s;#Delta#eta;|#Delta#phi|-#Delta#phi_{0} (deg)",rng),50,-0.05,0.05,128,-8,8);
		hGateVis_survRes[ib]	= new TH2D(Form("hGateVis_survRes_%d",ib),Form("both tracks kept, %s;#Delta#eta;|#Delta#phi|-#Delta#phi_{0} (deg)",rng),50,-0.05,0.05,128,-8,8);
	}
	for (int ic=0;ic<2;ic++){
		const char* cn = ic==0 ? "pp" : "mm";
		for (int ipc=0;ipc<2;ipc++){
			const char* pn = ipc==0 ? "lopt" : "hipt";
			hD573_SKF_surv_cen[ic][ipc]	= new TH1D(Form("hD573_SKF_surv_cen_%s_%s",cn,pn),Form("surviving %s pairs, central, %s -- SiSplitScore",cn,pn),44,-1.05,1.05);
			hD573_SKF_surv_side[ic][ipc]	= new TH1D(Form("hD573_SKF_surv_side_%s_%s",cn,pn),Form("surviving %s pairs, dphi sideband, %s -- SiSplitScore",cn,pn),44,-1.05,1.05);
			hD573_outcome[ic][ipc]		= new TH1D(Form("hD573_outcome_%s_%s",cn,pn),Form("%s pairs, central, SKF>=cut, %s -- 0 removed 1 outEll 2 SL+RGfail 3 tie 4 other",cn,pn),5,-0.5,4.5);
			hD573_SL_survInEll[ic][ipc]	= new TH1D(Form("hD573_SL_survInEll_%s_%s",cn,pn),Form("surviving %s pairs in ellipse, SKF>=cut, %s -- SL",cn,pn),120,-1.1,1.1);
			hD573_pos_survHi[ic][ipc]	= new TH2D(Form("hD573_pos_survHi_%s_%s",cn,pn),Form("surviving %s pairs, SKF>=cut, %s -- dphi(deg) vs deta",cn,pn),50,-0.05,0.05,80,-20,20);
			hD573_pos_allHi[ic][ipc]	= new TH2D(Form("hD573_pos_allHi_%s_%s",cn,pn),Form("all %s pairs, SKF>=cut, %s -- dphi(deg) vs deta",cn,pn),50,-0.05,0.05,80,-20,20);
		}
		hD573_dphiDinvpt_allHi[ic]	= new TH2D(Form("hD573_dphiDinvpt_allHi_%s",cn),Form("all %s pairs, |deta|<0.05, SKF>=cut -- |dphi|(deg) vs |1/pt1-1/pt2|",cn),120,0,12,120,0,30);
		hD573_dphiDinvpt_survHi[ic]	= new TH2D(Form("hD573_dphiDinvpt_survHi_%s",cn),Form("surviving %s pairs, |deta|<0.05, SKF>=cut -- |dphi|(deg) vs |1/pt1-1/pt2|",cn),120,0,12,120,0,30);
		hD573_dphiPtinv_allHi[ic]	= new TH2D(Form("hD573_dphiPtinv_allHi_%s",cn),Form("all %s pairs, |deta|<0.05, SKF>=cut -- dphi(deg) vs 2/(pt1+pt2)",cn),50,0,10,80,-20,20);
	}
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
	long nFlagged_fullmvtx		= 0;	// path 3 (full MVTX match, README_SplitTracks573 sec 8)
	long nFlagged_Looper		= 0;	// README_SplitTracks573 sec 35: looper veto, tracks flagged
	TH1D *hLooper_p		= new TH1D("hLooper_p","looper veto: |p| of the removed track;|p| (GeV/c);tracks",40,0.,0.4);
	TH1D *hLooper_rs	= new TH1D("hLooper_rs","looper veto: OS pairs, |p|<LOOPER_PMAX, #Delta#phi#geq170#circ;|#vec{p}_{1}+#vec{p}_{2}|/(|p_{1}|+|p_{2}|);pairs",50,0.,0.25);
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
	//---- README_PID.md: dedxKFP (the value the gates are defined on), and its match to our dedx70s
	TH2D *hdedxKFPp		= new TH2D("hdedxKFPp"   ,"dedxKFP vs p, all tracks;p (GeV/c);dedxKFP",200,0,2.,200,0.,4000.);
	TH2D *hdedxKFPp_id[4];									// 0 pi, 1 K, 2 p, 3 unidentified
	const char* idName[4]	= {"#pi","K","p","unidentified"};
	for (int k=0;k<4;k++) hdedxKFPp_id[k] = new TH2D(Form("hdedxKFPp_id%d",k),Form("dedxKFP vs p, PID = %s;p (GeV/c);dedxKFP",idName[k]),200,0,2.,200,0.,4000.);
	TProfile *hdedxratio_p	= new TProfile("hdedxratio_p","dedxKFP/dedx70s vs p;p (GeV/c);<dedxKFP/dedx70s>",100,0.,2.,0.,5.);
	TH2D *hdedxratio2_p		= new TH2D("hdedxratio2_p","dedxKFP/dedx70s vs p;p (GeV/c);dedxKFP/dedx70s",100,0.,2.,200,0.5,1.5);
	TH2D *hdedxKFPp_tagged[2][3];							// V0 daughters, as hdedxp_tagged but on dedxKFP
	for (int iv=0;iv<3;iv++) for (int ic=0;ic<2;ic++)
		hdedxKFPp_tagged[ic][iv]	= new TH2D(Form("hdedxKFPp_tagged_%d_%d",ic,iv),Form("dedxKFP vs p, %s#rightarrow%s;p (GeV/c);dedxKFP",strv0[iv],strch[ic]),200,0.,2.,200,0.,4000.);
	//---- gate efficiency on V0 daughters: [0] Lambda->p & Lbar->pbar (true protons), [1] all K0s->pi (true pions);
	//---- y bin = PID given (0 pi, 1 K, 2 p, 3 none)
	TH2D *hPIDtagged[2];
	hPIDtagged[0]	= new TH2D("hPIDtagged_p" ,"V0-daughter protons: PID given vs p;p (GeV/c);PID (0 #pi,1 K,2 p,3 none)",40,0.,2.,4,-0.5,3.5);
	hPIDtagged[1]	= new TH2D("hPIDtagged_pi","K^{0}_{S}-daughter pions: PID given vs p;p (GeV/c);PID (0 #pi,1 K,2 p,3 none)",40,0.,2.,4,-0.5,3.5);
	TH1D *hPIDcount	= new TH1D("hPIDcount","accepted tracks by PID;;tracks",8,-0.5,7.5);
	const char* pidLab[8]	= {"#pi+","#pi-","K+","K-","p","#bar{p}","none +","none -"};
	for (int k=0;k<8;k++) hPIDcount->GetXaxis()->SetBinLabel(k+1,pidLab[k]);
	TH1D *hpt_id[3][2];										// pt of identified pi/K/p, pos/neg
	//---- README_PID.md sec 8: the p-pi ULS ridge just below dphi=0 (dphi = phi(-)-phi(+), the class's ULS convention).
	//---- Sibling p-pi pairs (identified, as they enter the pair types), abs(dy)<0.7: [box][cc], box 0 = ridge
	//---- (-10<=dphi<0 deg), 1 = its mirror (0<=dphi<10, the clean side); cc 0 = p pi-, 1 = pbar pi+.
	TH1D *hRidge_dphi[2], *hRidge_Mppi[2][2], *hRidge_Mee[2][2], *hRidge_open[2][2], *hRidge_prat[2][2], *hRidge_skf[2][2];
	TH1D *hRidge_dcap[2][2], *hRidge_dcapi[2][2], *hRidge_ptp[2][2];
	{
		const char* bx[2]	= {"ridge","mirror"};
		const char* cc[2]	= {"p#pi^{-}","#bar{p}#pi^{+}"};
		for (int c=0;c<2;c++){
			hRidge_dphi[c]	= new TH1D(Form("hRidge_dphi_%d",c),Form("%s sibling, abs(dy)<0.7;#phi(-)-#phi(+) (deg);pairs",cc[c]),160,-20.,20.);
			for (int b=0;b<2;b++){
				hRidge_Mppi[b][c]	= new TH1D(Form("hRidge_Mppi_%d_%d",b,c),Form("%s %s: M_{inv} as p#pi;M_{p#pi} (GeV)",cc[c],bx[b]),100,1.07,1.27);
				hRidge_Mee[b][c]	= new TH1D(Form("hRidge_Mee_%d_%d",b,c),Form("%s %s: M_{inv} as e^{+}e^{-};M_{ee} (GeV)",cc[c],bx[b]),100,0.,0.2);
				hRidge_open[b][c]	= new TH1D(Form("hRidge_open_%d_%d",b,c),Form("%s %s: 3D opening angle;#theta_{open} (deg)",cc[c],bx[b]),90,0.,45.);
				hRidge_prat[b][c]	= new TH1D(Form("hRidge_prat_%d_%d",b,c),Form("%s %s: p(#pi)/p(p);ratio",cc[c],bx[b]),50,0.,2.5);
				hRidge_skf[b][c]	= new TH1D(Form("hRidge_skf_%d_%d",b,c),Form("%s %s: SiSplitScore;SKF",cc[c],bx[b]),22,-0.05,1.05);
				hRidge_dcap[b][c]	= new TH1D(Form("hRidge_dcap_%d_%d",b,c),Form("%s %s: 3D DCA of the (anti)proton;DCA (cm)",cc[c],bx[b]),100,0.,2.);
				hRidge_dcapi[b][c]	= new TH1D(Form("hRidge_dcapi_%d_%d",b,c),Form("%s %s: 3D DCA of the pion;DCA (cm)",cc[c],bx[b]),100,0.,2.);
				hRidge_ptp[b][c]	= new TH1D(Form("hRidge_ptp_%d_%d",b,c),Form("%s %s: p_{T} of the (anti)proton;p_{T} (GeV/c)",cc[c],bx[b]),50,0.,1.25);
			}
		}
	}
	//---- README_CQcomparison.md sec 2.1-2.2: V0 daughter sharing / duplicates, and <pT> vs N_ch (sibling only, all events).
	//---- kv: 0 K0s, 1 Lambda, 2 Lbar. "In the lists" = as the pair types take them (ridgePID, Species_ptmin, Species_yu).
	//---- dR = sqrt(deta^2+dphi^2) (rad); CQC_DRDUP = the near-duplicate radius used for the categories below.
	const double CQC_DRDUP	= 0.05;
	const char* v0nm[3]		= {"K^{0}_{S}","#Lambda","#bar{#Lambda}"};
	TH2D *hV0share		= new TH2D("hV0share","in-peak V0 pairs sharing a daughter index;V0 1;V0 2",3,-0.5,2.5,3,-0.5,2.5);
	TH2D *hV0pairs		= new TH2D("hV0pairs","all in-peak V0 pairs;V0 1;V0 2",3,-0.5,2.5,3,-0.5,2.5);
	TH2D *hV0shareOff	= new TH2D("hV0shareOff","in-peak V0 sharing a daughter with an off-peak V0;in-peak V0;off-peak V0",3,-0.5,2.5,3,-0.5,2.5);
	TH1D *hDauNear_dR[3][2], *hDauNear_skf[3][2];	// [kv][daughter charge 0 +, 1 -]: nearest same-charge, same-PID track in the lists
	TH2D *hDauNear_dRdpt[3][2];
	for (int kv=0;kv<3;kv++) for (int q=0;q<2;q++){
		const char* dn	= (kv==0) ? (q?"#pi^{-}":"#pi^{+}") : (kv==1) ? (q?"#pi^{-}":"p") : (q?"#bar{p}":"#pi^{+}");
		hDauNear_dR[kv][q]		= new TH1D(Form("hDauNear_dR_%d_%d",kv,q),Form("in-peak %s, daughter %s: nearest same-PID track in the lists;#DeltaR (rad);daughters",v0nm[kv],dn),150,0.,0.3);
		hDauNear_skf[kv][q]		= new TH1D(Form("hDauNear_skf_%d_%d",kv,q),Form("in-peak %s, daughter %s: SiSplitScore with the nearest, #DeltaR<%.2f;SKF",v0nm[kv],dn,CQC_DRDUP),22,-0.05,1.05);
		hDauNear_dRdpt[kv][q]	= new TH2D(Form("hDauNear_dRdpt_%d_%d",kv,q),Form("in-peak %s, daughter %s: nearest;#DeltaR (rad);abs(#Deltap_{T})/p_{T}",v0nm[kv],dn),50,0.,0.1,50,0.,0.5);
	}
	//---- p-Lambda [0] and pbar-Lbar [1] sibling Q = 2k*, by the (anti)proton: 0 within CQC_DRDUP of the V0's own
	//---- (anti)proton daughter, 1 daughter of an off-peak V0, 2 neither
	const char* pLcat[3]	= {"near-dup of own daughter","off-peak V0 daughter","neither"};
	TH1D *hpLa_Q[2][3];
	TH2D *hpLa_QdR[2];
	TH1D *hLaLa_Q[2][3];		// Lambda-Lambda [0], Lbar-Lbar [1]: 0 share a daughter, 1 same-charge daughters within CQC_DRDUP, 2 neither
	const char* LLcat[3]	= {"share a daughter","daughters #DeltaR<0.05","neither"};
	TH1D *hnLApeak[2];
	for (int a=0;a<2;a++){
		const char* pn	= a ? "#bar{p}#bar{#Lambda}" : "p#Lambda";
		const char* ln	= a ? "#bar{#Lambda}#bar{#Lambda}" : "#Lambda#Lambda";
		for (int c=0;c<3;c++){
			hpLa_Q[a][c]	= new TH1D(Form("hpLa_Q_%d_%d",a,c),Form("%s sibling, (anti)proton: %s;Q = 2k* (GeV/c);pairs",pn,pLcat[c]),50,0.,1.);
			hLaLa_Q[a][c]	= new TH1D(Form("hLaLa_Q_%d_%d",a,c),Form("%s sibling: %s;Q (GeV/c);pairs",ln,LLcat[c]),40,0.,4.);
		}
		hpLa_QdR[a]	= new TH2D(Form("hpLa_QdR_%d",a),Form("%s sibling;Q = 2k* (GeV/c);#DeltaR((anti)proton, V0's own daughter) (rad)",pn),50,0.,1.,50,0.,0.5);
		hnLApeak[a]	= new TH1D(Form("hnLApeak_%d",a),Form("in-peak %s per event, in the pair window;n;events",a?"#bar{#Lambda}":"#Lambda"),8,-0.5,7.5);
	}
	TProfile *hptNch[3][2], *hptNchV0[3];	// <pT> vs N_ch (ntrkept) of what enters the pair types: [pi,K,p][+,-], V0 [kv]
	for (int k=0;k<3;k++) for (int q=0;q<2;q++)
		hptNch[k][q]	= new TProfile(Form("hptNch_%d_%d",k,q),Form("<p_{T}> vs N_{ch}, %s %s;N_{ch} (accepted tracks);<p_{T}> (GeV/c)",idName[k],q?"-":"+"),40,-0.5,39.5);
	for (int kv=0;kv<3;kv++)
		hptNchV0[kv]	= new TProfile(Form("hptNchV0_%d",kv),Form("<p_{T}> vs N_{ch}, in-peak %s;N_{ch} (accepted tracks);<p_{T}> (GeV/c)",v0nm[kv]),40,-0.5,39.5);
	long nNchKept	= 0, nNchSeen = 0;	// events inside / seen by the nchLLHH class (sec 2.1)
	TH2D *hypt_id[3];										// y vs pt of identified pi/K/p inside the charged eta fiducial:
	for (int k=0;k<3;k++) hypt_id[k] = new TH2D(Form("hypt_id%d",k),Form("y vs p_{T}, PID = %s, abs(#eta) fiducial;y;p_{T} (GeV/c)",idName[k]),88,-1.1,1.1,100,0.,2.);	// with the Species_yu windows drawn
	for (int k=0;k<3;k++) for (int ic=0;ic<2;ic++) hpt_id[k][ic] = new TH1D(Form("hpt_id%d_%d",k,ic),Form("p_{T}, PID = %s, %s;p_{T} (GeV/c)",idName[k],strch[ic]),200,0.,2.);
	TH2D *hdcaxy_id[3][2], *hdcaz_id[3][2];				// README_PID.md sec 10: DCA vs pt of identified pi/K/p, pos/neg (spallation protons)
	for (int k=0;k<3;k++) for (int ic=0;ic<2;ic++){
		hdcaxy_id[k][ic]	= new TH2D(Form("hdcaxy_id%d_%d",k,ic),Form("dcaxy vs p_{T}, PID = %s, %s;p_{T} (GeV/c);dcaxy (cm)",idName[k],strch[ic]),40,0.,2.,300,-1.5,1.5);
		hdcaz_id[k][ic]		= new TH2D(Form("hdcaz_id%d_%d" ,k,ic),Form("dcaz vs p_{T}, PID = %s, %s;p_{T} (GeV/c);dcaz (cm)"  ,idName[k],strch[ic]),40,0.,2.,300,-1.5,1.5);
	}
	TH2D *hadca_phi[2][2][2];								// README_PID.md sec 10.2: the phi ~ 105 deg strip. [xy|z][charge][vertex phi | phi at
	for (int d=0;d<2;d++) for (int q=0;q<2;q++) for (int f=0;f<2;f++)	// R = 0.55 m as hSpoke] vs abs(dca), accepted charged tracks
		hadca_phi[d][q][f]	= new TH2D(Form("hadca_phi_%d_%d_%d",d,q,f),Form("abs(%s), %s;%s (deg);abs(%s) (cm)",d?"dcaz":"dcaxy",q?"negative":"positive",f?"#phi(R=0.55 m)":"vertex #phi",d?"dcaz":"dcaxy"),360,-180.,180.,75,0.,1.5);
	//---- README_PID.md sec 10.3: the vertex-phi mask page. Tracks passing AcceptTrackBase, before the mask: [q] vertex phi
	//---- vs abs(dcaxy) and vs abs(dcaz), and PID (0-3) x charge x [0 outside | 1 inside a mask window]
	TH2D *hmask_dcaxy[2], *hmask_dcaz[2];
	for (int q=0;q<2;q++){
		hmask_dcaxy[q]	= new TH2D(Form("hmask_dcaxy_%d",q),Form("before the #phi mask, %s;vertex #phi (deg);abs(dcaxy) (cm)",q?"negative":"positive"),360,-180.,180.,75,0.,1.5);
		hmask_dcaz[q]	= new TH2D(Form("hmask_dcaz_%d",q) ,Form("before the #phi mask, %s;vertex #phi (deg);abs(dcaz) (cm)" ,q?"negative":"positive"),360,-180.,180.,75,0.,1.5);
	}
	TH2D *hmask_pid	= new TH2D("hmask_pid","tracks before the #phi mask;PID x charge;0 outside, 1 inside a window",8,-0.5,7.5,2,-0.5,1.5);
	TH2D *hphieta_id[3][2];									// README_PID.md sec 10: (eta,phi) of identified pi/K/p, pos/neg
	for (int k=0;k<3;k++) for (int ic=0;ic<2;ic++)
		hphieta_id[k][ic]	= new TH2D(Form("hphieta_id%d_%d",k,ic),Form("(#eta,#phi), PID = %s, %s;#eta;#phi",idName[k],strch[ic]),30,-1.5,1.5,36,-M_PI,M_PI);
	//---- same, vs (eta,phi): [0] mean dcaxy, [1] mean abs(dcaxy), [2] mean dcaz, [3] mean abs(dcaz) (alignment/distortions by region)
	TProfile2D *pdca_etaphi_id[3][2][4];
	const char* dcaVar[4]	= {"dcaxy","|dcaxy|","dcaz","|dcaz|"};
	for (int k=0;k<3;k++) for (int ic=0;ic<2;ic++) for (int iv=0;iv<4;iv++)
		pdca_etaphi_id[k][ic][iv]	= new TProfile2D(Form("pdca_etaphi_id%d_%d_%d",k,ic,iv),Form("<%s> vs (#eta,#phi), PID = %s, %s;#eta;#phi;<%s> (cm)",dcaVar[iv],idName[k],strch[ic],dcaVar[iv]),30,-1.5,1.5,36,-M_PI,M_PI);
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
		//---- README_PID.md sec 12: the daughters of every in-peak V0, from the V0's own track indices (indv0 holds
		//---- one V0 per track, so a track shared by an in-peak and an off-peak candidate could escape through it)
		std::vector<int> peakV0Of((*ntr),-1);
		for (int iv0=0;iv0<(*nv0);iv0++){
			if (!v0InPeak(iv0)) continue;
			const int itd[2]	= {v0indtr1[iv0], v0indtr2[iv0]};
			for (int kd=0;kd<2;kd++){
				if (itd[kd]<0 || itd[kd]>=(*ntr) || peakV0Of[itd[kd]]>=0) continue;
				peakV0Of[itd[kd]]	= iv0;
				if (!(indv0[itd[kd]]>=0 && v0InPeak(indv0[itd[kd]]))) ++nPeakDauNotIndv0;
			}
		}
		auto isPeakDaughter	= [&](int it){ return peakV0Of[it]>=0; };
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
		std::vector<int> lsLosers_thisevt;	// README_SplitTracks573 sec 15: LS-path losers only (charge-asymmetry diagnostic)
		std::vector<std::pair<int,int>> loserPath_thisevt;	// sec 19: (track, removal path) -- 0 LS duplicate (SL),
								// 1 LS complementary (RadialGap), 2 LS path 3 (MVTX match), 3 ULS veto, 4 XTF cleaner
		//---- sec 18.22: cross-crossing duplicates found by BuildXTFLosers() -- same track-level
		//---- removal as the LS/ULS paths below (they skip anything already in this list).
		if (doXTFClean){
			auto itx = xtfLosers.find(jentry);
			if (itx!=xtfLosers.end()){ nSplitFlagged_thisevt = itx->second; for (int t : itx->second) loserPath_thisevt.push_back({t,4}); }
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
				if (isPeakDaughter(jt)) continue;		// README_PID.md sec 11/12: only peak V0s claim daughters
				if (!AcceptTrack(jt)) continue;
				if (doOldPID && dedx70s[jt]>=400.) continue;	// README_PID.md sec 8: with the dE/dx PID, every accepted
				if (chg[jt]==0) continue;						// charged track is a candidate (protons split too); was pions only
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
					//---- elliptical pregate, sec 16.9; the dphi semi-axis is PREGATE_DPHI unless the
					//---- (the pregate itself: corral_class.h InLSPregate, README_SplitTracks573 sec 12/18).
					//---- (deta semi-axis: valPregateDEta = PREGATE_DETA 0.022 unless "pgdetaNNN"; "dpsNN" centres
					//---- the ellipse on the split peak, README_SplitTracks573 sec 12 -- all in InLSPregate)
					bool inBox		= InLSPregate(deta,dphi,pt[i],pt[j]);
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
									loserPath_thisevt.push_back({loserv,3});
								}
							}
						}
					}
					//---- README_SplitTracks573 sec 35: LOOPER veto. A track with pt < ~0.17 GeV/c curls back inside the TPC; the
					//---- returning half is reconstructed as a second, opposite-charge track exactly back to back with the same
					//---- |p| (sec 33.4). OS pairs with both |p| < LOOPER_PMAX and |p1+p2|/(|p1|+|p2|) < valLooperRSum: one track,
					//---- remove the worse one (fewer ntpc loses; tie: larger abs(dca) sum). No angular gate beyond the sum.
					//---- "nolooper" turns it off.
					if (!sameCharge && doLooperVeto){
						double px1=pt[i]*cos(phi[i]), py1=pt[i]*sin(phi[i]), pz1=pt[i]*sinh(eta[i]);
						double px2=pt[j]*cos(phi[j]), py2=pt[j]*sin(phi[j]), pz2=pt[j]*sinh(eta[j]);
						double p1=sqrt(px1*px1+py1*py1+pz1*pz1), p2=sqrt(px2*px2+py2*py2+pz2*pz2);
						if (p1<LOOPER_PMAX && p2<LOOPER_PMAX && fabs(dphi)>=170.0*M_PI/180.0){
							double rs	= sqrt(pow(px1+px2,2)+pow(py1+py2,2)+pow(pz1+pz2,2))/(p1+p2);
							hLooper_rs->Fill(rs);
							if (rs<valLooperRSum){
								int loserv	= (ntpc[i]!=ntpc[j]) ? ((ntpc[i]<ntpc[j]) ? i : j)
											: ((fabs(dcaxy[i])+fabs(dcaz[i]) > fabs(dcaxy[j])+fabs(dcaz[j])) ? i : j);
								++nFlagged_Looper;
								hLooper_p->Fill(loserv==i ? p1 : p2);
								if (std::find(nSplitFlagged_thisevt.begin(),nSplitFlagged_thisevt.end(),loserv)==nSplitFlagged_thisevt.end()){
									nSplitFlagged_thisevt.push_back(loserv);
									loserPath_thisevt.push_back({loserv,5});
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
					//---- path 3 (README_SplitTracks573 sec 8, opt-in): exact MVTX match on >= valFullMVTX common
					//---- layers, and abs(1/pt1-1/pt2) >= valFullMVTXdinv, whatever SL/RG say.
					//---- sec 15: ">= valFullMVTX MATCHED MVTX layers" (was: all layers matched and >= N of them;
					//---- identical for N = 3). Two shared MVTX clusters plus the common vertex already fix the
					//---- trajectory, so N = 2 is still one particle unless abs(1/pt1-1/pt2) is tiny.
					int nMatchMVTX	= (int)std::lround(mvtxFrac*mvtxDen);
					bool flagFullMVTX	= (!flagDuplicate && !flagComplementary && valFullMVTX>0
										&& mvtxDen>0 && nMatchMVTX>=valFullMVTX
										&& fabs(1.0/pt[i]-1.0/pt[j])>=valFullMVTXdinv);
					if (!flagDuplicate && !flagComplementary && !flagFullMVTX) continue;	// not flagged by any path
					if (flagDuplicate) ++nFlagged_duplicate; else if (flagComplementary) ++nFlagged_complementary; else ++nFlagged_fullmvtx;
					int loser = -1;
					if (ntpc[i]!=ntpc[j]){
						loser	= (ntpc[i]<ntpc[j]) ? i : j;
					} else if (quality[i]!=quality[j]){
						loser	= (quality[i]>quality[j]) ? i : j;	// worse (higher) quality loses, sec 9.5
					}													// else: still tied -> no loser, sec 9.5 pt.3
					if (loser>=0
					 && std::find(nSplitFlagged_thisevt.begin(),nSplitFlagged_thisevt.end(),loser)==nSplitFlagged_thisevt.end()){
						nSplitFlagged_thisevt.push_back(loser);	// don't double-count if flagged by >1 partner
						lsLosers_thisevt.push_back(loser);
						loserPath_thisevt.push_back({loser, flagDuplicate ? 0 : (flagComplementary ? 1 : 2)});
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
			//---- README_SplitTracks573 sec 6: diagnostic only (see the booking). Runs after all
			//---- pre-pass flagging, so "surv" is final. Same pregate and cuts as the LS path above.
			{
				static const double D_DETA	= 0.022, D_DPHI = 4.1*M_PI/180.0;	// = PREGATE_DETA/DPHI
				auto flagged = [&](int t){ return std::find(nSplitFlagged_thisevt.begin(),nSplitFlagged_thisevt.end(),t)!=nSplitFlagged_thisevt.end(); };
				std::vector<int> ls;
				for (int jt=0;jt<(*ntr);jt++){
					if (isPeakDaughter(jt) || !AcceptTrack(jt) || (doOldPID && dedx70s[jt]>=400.) || chg[jt]==0) continue;	// README_PID.md sec 8
					ls.push_back(jt);
				}
				for (int jt : ls){	// sec 28: sector-gap spokes (booking above)
					const double aR	= 0.3*1.4/2.0*0.55;
					int q		= chg[jt]>0 ? 0 : 1;
					double pd	= phi[jt]*180.0/M_PI;
					hSpoke[q][0]->Fill(pd,pt[jt]);
					if (aR/pt[jt]<1.0){
						double pr	= pd + (chg[jt]>0 ? 1.0 : -1.0)*asin(aR/pt[jt])*180.0/M_PI;	// sign from the data (sec 28): the gaps straighten
						while (pr>= 180.) pr -= 360.;
						while (pr< -180.) pr += 360.;
						hSpoke[q][1]->Fill(pr,pt[jt]);
					}
				}
				{	// sec 25: cosmic test (booking above)
					const double mpi	= 0.13957;
					auto rap = [&](int t){ double pz=pt[t]*sinh(eta[t]), p2=pt[t]*pt[t]+pz*pz, E=sqrt(p2+mpi*mpi); return 0.5*log((E+pz)/(E-pz)); };
					std::vector<int> kp;
					for (int t : ls) if (!flagged(t)) kp.push_back(t);
					for (size_t ii=0;ii<kp.size();ii++) for (size_t jj=ii+1;jj<kp.size();jj++){
						int i=kp[ii], j=kp[jj];
						if (fabs(rap(i)-rap(j))>=0.1) continue;
						double adp	= fabs(phi[i]-phi[j]); if (adp>M_PI) adp = 2*M_PI-adp;
						adp *= 180.0/M_PI;
						bool os		= chg[i]*chg[j]<0;
						int w	= -1;
						if (os && adp>=170.) w = 0; else if (os && adp>=150. && adp<160.) w = 1; else if (!os && adp>=170.) w = 2;
						if (w<0) continue;
						double ax	= phi[i]*180.0/M_PI; ax = fmod(ax+360.,180.);
						hCos_axis[w]	->Fill(ax);
						hCos_ptAsym[w]	->Fill((pt[i]-pt[j])/(pt[i]+pt[j]));
						hCos_eta[w]		->Fill(eta[i]);
						hCos_npi[w]		->Fill(kp.size());
						hCos_vtxntr[w]	->Fill(*vtxntr);
						hCos_pt[w]		->Fill(0.5*(pt[i]+pt[j]));
						hCos_etotoh[w]	->Fill(*etotoh);
						hCos_dcaxy[w]	->Fill(dcaxy[i],dcaxy[j]);
						hCos_dcaz[w]	->Fill(dcaz[i],dcaz[j]);
						{	// sec 25.1
							double px1=pt[i]*cos(phi[i]), py1=pt[i]*sin(phi[i]), pz1=pt[i]*sinh(eta[i]);
							double px2=pt[j]*cos(phi[j]), py2=pt[j]*sin(phi[j]), pz2=pt[j]*sinh(eta[j]);
							double p1=sqrt(px1*px1+py1*py1+pz1*pz1), p2=sqrt(px2*px2+py2*py2+pz2*pz2);
							double rs	= sqrt(pow(px1+px2,2)+pow(py1+py2,2)+pow(pz1+pz2,2))/(p1+p2);
							double E	= sqrt(p1*p1+mpi*mpi)+sqrt(p2*p2+mpi*mpi);
							double m2	= E*E - (pow(px1+px2,2)+pow(py1+py2,2)+pow(pz1+pz2,2));
							double mi	= m2>0. ? sqrt(m2) : 0.;
							hCos_psum[w]->Fill(rs);
							hCos_minv[w]->Fill(mi);
							if (rs<0.05){ hCos_axisBB[w]->Fill(ax); hCos_minvBB[w]->Fill(mi); }
						}
					}
				}
				if (fabs(*vtxz)<8.){	// sec 38: CM study (booking above)
					double sg	= (*vtxz<0.) ? 1. : -1.;
					auto wof	= [&](int t){ return (eta[t] + CM_K*(*vtxz))*sg; };
					auto tpcfl	= [&](int t, int& f, int& l){ f=-1; l=-1; for (int b=7;b<55;b++) if ((layermask[t]>>b)&1ULL){ if (f<0) f=b-7; l=b-7; } };
					auto nsi	= [&](int t){ int n=0; for (int b=0;b<7;b++) if ((layermask[t]>>b)&1ULL) ++n; return n; };
					std::vector<int> kc; std::vector<int> cc;
					for (int t : ls){
						if (flagged(t)) continue;
						double w	= wof(t);
						hCM_w->Fill(w); hCM_w_ntpc->Fill(w,ntpc[t]); hCM_w_xing->Fill(w,*crossing);
						int c	= (w>CM_BLO && w<CM_BHI) ? 0 : (fabs(w)>CM_CLO && fabs(w)<CM_CHI) ? 1 : -1;
						if (c<0) continue;
						int f,l; tpcfl(t,f,l); hCM_w_tpcfl[c]->Fill(f,l);
						(c==0 ? kc : cc).push_back(t);
					}
					hCM_nbnc->Fill(cc.size(),kc.size());
					for (int c=0;c<2;c++){
						std::vector<int>& v	= (c==0) ? kc : cc;
						for (size_t ii=0;ii<v.size();ii++) for (size_t jj=ii+1;jj<v.size();jj++){
							int i=v[ii], j=v[jj];
							if (c==1 && wof(i)*wof(j)<0.) continue;	// control: same side of the hole
							int q	= (chg[i]*chg[j]<0) ? 0 : 1;
							double dphi	= phi[i]-phi[j]; if (dphi> M_PI) dphi -= 2*M_PI; if (dphi<-M_PI) dphi += 2*M_PI;
							hCM_dphideta[c][q]->Fill(eta[i]-eta[j],dphi*180.0/M_PI);
							hCM_ntpc12[c][q]->Fill(ntpc[i],ntpc[j]);
							hCM_nsi12[c][q]->Fill(nsi(i),nsi(j));
							hCM_pt12[c][q]->Fill(pt[i],pt[j]);
							hCM_ntpcsum[c][q]->Fill(ntpc[i]+ntpc[j]);
							hCM_skf[c][q]->Fill(SiSplitScore(&siclukey[i*7],&siclukey[j*7],layermask[i],layermask[j]));
							hCM_dw[c][q]->Fill(wof(i)-wof(j));
						}
					}
				}
				for (auto& lp : loserPath_thisevt){	// sec 19: per-path losers among accepted pions
					if (std::find(ls.begin(),ls.end(),lp.first)==ls.end()) continue;
					hLoserPath_pt[lp.second][chg[lp.first]>0?0:1]->Fill(pt[lp.first]);
				}
				for (int jt : ls){	// sec 15: charge asymmetry
					int c	= chg[jt]>0 ? 0 : 1;
					bool los	= std::find(lsLosers_thisevt.begin(),lsLosers_thisevt.end(),jt)!=lsLosers_thisevt.end();
					hAsym_all_pt[c]->Fill(pt[jt]); hAsym_all_eta[c]->Fill(eta[jt]); hAsym_all_phi[c]->Fill(phi[jt]); hAsym_all_ntpc[c]->Fill(ntpc[jt]);
					if (los){ hAsym_los_pt[c]->Fill(pt[jt]); hAsym_los_eta[c]->Fill(eta[jt]); hAsym_los_phi[c]->Fill(phi[jt]); hAsym_los_ntpc[c]->Fill(ntpc[jt]); }
				}
				for (size_t ii=0;ii<ls.size();ii++){
					for (size_t jj=ii+1;jj<ls.size();jj++){
						int i=ls[ii], j=ls[jj];
						if (chg[i]*chg[j]<=0) continue;
						double deta	= eta[i]-eta[j];
						double dphi	= phi[i]-phi[j];
						if (dphi> M_PI) dphi -= 2*M_PI;
						if (dphi<-M_PI) dphi += 2*M_PI;
						double dphideg	= dphi*180.0/M_PI;
						if (fabs(deta)>=0.08 || fabs(dphideg)>=30.) continue;
						int ic	= chg[i]>0 ? 0 : 1;
						int ipc	= std::min(pt[i],pt[j])<0.2 ? 0 : 1;
						bool surv	= !flagged(i) && !flagged(j);
						double SKF	= SiSplitScore(&siclukey[i*7],&siclukey[j*7],layermask[i],layermask[j]);
						bool hi		= (SKF>=valSiKeyCut);
						{	// fine dphi0 slices (booking above); abs(deta) out to 0.08
							int k	= (int)(DPsCentre(pt[i],pt[j])*180.0/M_PI/GATEFINE_W);
							if (k>=0 && k<NGATEFINE){
								hGateFine_all[k]->Fill(deta,dphideg);
								if (hi)         hGateFine_hi[k]    ->Fill(deta,fabs(dphideg));
								if (hi && surv) hGateFine_hiSurv[k]->Fill(deta,fabs(dphideg));
								if (surv)       hGateFine_surv[k]  ->Fill(deta,fabs(dphideg));
							}
						}
						if (fabs(deta)>=0.05) continue;
						{
							double dinv	= fabs(1.0/pt[i]-1.0/pt[j]);
							int ib		= -1;
							for (int k=0;k<NGATEVIS;k++) if (dinv>=GATEVIS_EDGE[k] && dinv<GATEVIS_EDGE[k+1]) ib = k;
							if (ib>=0){
								hGateVis_all[ib]->Fill(deta,dphideg);
								if (hi)   hGateVis_hi[ib]  ->Fill(deta,dphideg);
								if (surv) hGateVis_surv[ib]->Fill(deta,dphideg);
								double res	= fabs(dphideg) - DPsCentre(pt[i],pt[j])*180.0/M_PI;
								if (hi)   hGateVis_hiRes[ib]  ->Fill(deta,res);
								if (hi)   hGateVis_dinvHi[ib] ->Fill(dinv);
								if (surv) hGateVis_survRes[ib]->Fill(deta,res);
							}
						}
						bool cen	= (fabs(deta)<0.034 && fabs(dphideg)<10.);
						if (hi){	// sec 15: silicon match code
							ULong64_t mA=layermask[i], mB=layermask[j]; int nM=0,dM=0,nI=0,dI=0;
							for (int L=0;L<7;L++){ bool h=((mA>>L)&1ULL)||((mB>>L)&1ULL); bool mt=(siclukey[i*7+L]==siclukey[j*7+L] && siclukey[i*7+L]!=kSiKeySentinel);
								if (L<3){ if(h)++dM; if(mt)++nM; } else { if(h)++dI; if(mt)++nI; } }
							double xm=dM*4+nM, yi=dI*5+nI;
							bool inG	= InLSPregate(deta,dphi,pt[i],pt[j]);
							if (cen && !surv)        hD573_match[0]->Fill(xm,yi);
							if (cen && surv && inG)  hD573_match[1]->Fill(xm,yi);
							if (fabs(deta)>=0.04)    hD573_match[2]->Fill(xm,yi);
						}
						bool side	= (fabs(deta)<0.034 && fabs(dphideg)>=10.);
						if (hi){
							hD573_pos_allHi[ic][ipc]	->Fill(deta,dphideg);
							hD573_dphiPtinv_allHi[ic]	->Fill(2.0/(pt[i]+pt[j]),dphideg*chg[i]);
							if (surv) hD573_pos_survHi[ic][ipc]->Fill(deta,dphideg);
							hD573_dphiDinvpt_allHi[ic]	->Fill(fabs(1.0/pt[i]-1.0/pt[j]),fabs(dphideg));
							if (surv) hD573_dphiDinvpt_survHi[ic]->Fill(fabs(1.0/pt[i]-1.0/pt[j]),fabs(dphideg));
						}
						if (surv && cen)  hD573_SKF_surv_cen[ic][ipc] ->Fill(SKF);
						if (surv && side) hD573_SKF_surv_side[ic][ipc]->Fill(SKF);
						if (!(cen && hi)) continue;
						int oc;
						bool inEll	= InLSPregate(deta,dphi,pt[i],pt[j]);	// same gate as the LS path
						if (!surv)			oc = 0;
						else if (!inEll)	oc = 1;
						else {
							double SL	= ComputeSplitSL(layermask[i],layermask[j]);
							double RG	= ComputeRadialGap(layermask[i],layermask[j]);
							double mF,iF; int mD,iD;
							SiKeyFracSplit(&siclukey[i*7],&siclukey[j*7],layermask[i],layermask[j],mF,mD,iF,iD);
							bool fl		= (SL<valSLCut) || (!DISABLE_RG && RG>=0.0 && RG>=valRadialGapCut)
										|| (valFullMVTX>0 && mD>0 && std::lround(mF*mD)>=valFullMVTX && fabs(1.0/pt[i]-1.0/pt[j])>=valFullMVTXdinv);
							hD573_SL_survInEll[ic][ipc]->Fill(SL);
							if (!doSplitRemoval)	oc = 4;
							else if (!fl)			oc = 2;
							else if (ntpc[i]==ntpc[j] && quality[i]==quality[j]) oc = 3;
							else					oc = 4;
						}
						hD573_outcome[ic][ipc]->Fill(oc);
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
		std::vector<int> ridgePID((*ntr),-1);		// README_PID.md sec 8: PID (0 pi,1 K,2 p) of tracks entering the pair types
		std::vector<double> ridgeY((*ntr),0.);
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
			//---- README_PID.md sec 10.3: the phi-mask page (before the mask)
			if (AcceptTrackBase(it) && chg[it]!=0){
				int q	= chg[it]>0 ? 0 : 1;
				double pd	= phi[it]*180.0/M_PI;
				hmask_dcaxy[q]->Fill(pd,fabs(dcaxy[it]));
				hmask_dcaz[q] ->Fill(pd,fabs(dcaz[it]));
				hmask_pid->Fill(2*pidOf(it)+q, InSiPhiMask(it) ? 1 : 0);
			}
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
			int thisPIDk		= pidOf(it);	// README_PID.md: 0 pi, 1 K, 2 p, 3 unidentified
			if (thisPIDk<3 && chg[it]!=0){
				const int pidPos[3]	= {kParticleIDPionPlus ,kParticleIDKaonPlus ,kParticleIDProton    };
				const int pidNeg[3]	= {kParticleIDPionMinus,kParticleIDKaonMinus,kParticleIDAntiProton};
				thisParticleID	= (chg[it]>0) ? pidPos[thisPIDk] : pidNeg[thisPIDk];
				thisSpecies		= GetSpecies(thisParticleID);
			}
			thisindV0			= peakV0Of[it];		// README_PID.md sec 11/12: peak V0s only (tagged QA)
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
			if (chg[it]!=0){	// README_PID.md sec 10.2
				const double aR	= 0.3*1.4/2.0*0.55;
				int q		= chg[it]>0 ? 0 : 1;
				double pd	= phi[it]*180.0/M_PI, pr = -999.;
				if (aR/pt[it]<1.0){ pr = pd + (chg[it]>0 ? 1.0 : -1.0)*asin(aR/pt[it])*180.0/M_PI; while (pr>=180.) pr -= 360.; while (pr<-180.) pr += 360.; }
				const double ad[2]	= {fabs(dcaxy[it]),fabs(dcaz[it])};
				for (int d=0;d<2;d++){ hadca_phi[d][q][0]->Fill(pd,ad[d]); if (pr>-900.) hadca_phi[d][q][1]->Fill(pr,ad[d]); }
			}
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
				            hdedxKFPp_tagged[kch][kv0]	->Fill(ptot[it],dedxKFP[it]);
				//---- README_PID.md: the proton is the + daughter of a Lambda, the - daughter of a Lbar
				if ((kv0==1 && kch==0) || (kv0==2 && kch==1))	hPIDtagged[0]->Fill(ptot[it],thisPIDk);
				if  (kv0==0)									hPIDtagged[1]->Fill(ptot[it],thisPIDk);
			}
			//
			if (nmvtx[it]>0) heta_mvt	->Fill(eta[it]);
			if (nintt[it]>0) heta_int	->Fill(eta[it]);
			if (nmvtx[it]>0&&nintt[it]>0) heta_mvtint	->Fill(eta[it]);
			hpteta		->Fill(eta[it],pt[it]);
			hptphi		->Fill(phi[it],pt[it]);
			hdedxp		->Fill(ptot[it],dedx70s[it]);
			hdedxpz		->Fill(ptot[it],dedx70s[it]);
			hdedxKFPp	->Fill(ptot[it],dedxKFP[it]);
			hdedxKFPp_id[thisPIDk]	->Fill(ptot[it],dedxKFP[it]);
			if (dedx70s[it]>0.){ hdedxratio_p->Fill(ptot[it],dedxKFP[it]/dedx70s[it]); hdedxratio2_p->Fill(ptot[it],dedxKFP[it]/dedx70s[it]); }
			hPIDcount	->Fill(2*thisPIDk + kch);
			if (thisPIDk<3){ hpt_id[thisPIDk][kch]->Fill(pt[it]); hdcaxy_id[thisPIDk][kch]->Fill(pt[it],dcaxy[it]); hdcaz_id[thisPIDk][kch]->Fill(pt[it],dcaz[it]);
				const double dv[4]	= {dcaxy[it],fabs(dcaxy[it]),dcaz[it],fabs(dcaz[it])};
				for (int iv=0;iv<4;iv++) pdca_etaphi_id[thisPIDk][kch][iv]->Fill(eta[it],phi[it],dv[iv]);
				hphieta_id[thisPIDk][kch]->Fill(eta[it],phi[it]); }
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
			//---- README_PID.md: CalcRm wants RAPIDITY (it builds pz = mt*sinh(y)). eta -> y with the PID mass;
			//---- the acceptance cut below stays on eta (the detector edge), and abs(y) <= abs(eta) always.
			double thismt	= sqrt(thispt*thispt + Species_mass[thisSpecies]*Species_mass[thisSpecies]);
			double thisy	= asinh(thispt*sinh(thiseta)/thismt);
			if (thisPIDk<3 && thiseta>=thisYL && thiseta<thisYU) hypt_id[thisPIDk]->Fill(thisy,thispt);
			if (!isPeakDaughter(it) && thiseta>=-1.1 && thiseta<1.1){ ridgePID[it] = thisPIDk; ridgeY[it] = thisy; }
			//---- add this track to all PairType indices that this species is included in...
			if (!NOCORRELATIONS){
				for (int ipaty=0;ipaty<NPairTypes;ipaty++){
					//
					//---- protect against keeping a track for correlations that is a daughter of a V0 IN THIS SPECIFIC PAIR
					//		keep track if not known as a V0 daughter
					//		COULD keep track if a known daughter of V0, but that V0 is NOT part of this pair
					if (isPeakDaughter(it)){		// README_PID.md sec 11/12: only peak V0s claim daughters
						continue;
					}
					//
					//---- check if this track is particle 1 for this pair type
					bool accept1	= false;
					if (thisParticleID==PairTypes_Info[ipaty][0]) accept1 = true;
					if (accept1){
						if (thispt < Species_ptmin[thisSpecies]) accept1	= false;
						if (thispt >=Species_ptmax[thisSpecies]) accept1	= false;
						if (thiseta< -pairEtaEdge[ipaty] || thiseta>=pairEtaEdge[ipaty]) accept1	= false;	// detector edge
						if (thisy  <  pairYL1[ipaty]   || thisy  >= pairYU1[ipaty]   ) accept1	= false;	// class window (README_PID.md)
					}
					//---- check if this track is particle 2 for this pair type
					bool accept2	= false;
					if (thisParticleID==PairTypes_Info[ipaty][1]) accept2 = true;
					if (accept2){
						if (thispt < Species_ptmin[thisSpecies]) accept2	= false;
						if (thispt >=Species_ptmax[thisSpecies]) accept2	= false;
						if (thiseta< -pairEtaEdge[ipaty] || thiseta>=pairEtaEdge[ipaty]) accept2	= false;	// detector edge
						if (thisy  <  pairYL2[ipaty]   || thisy  >= pairYU2[ipaty]   ) accept2	= false;	// class window (README_PID.md)
					}
					if (accept1){		// this track is "Particle 1" in this PairType...
						int kk = calcr_n_1[ipaty];
						if (kk== MAX_CALCR_N-1){ 
							cout<<"reader::Loop -- PVEC1 FULL .. max="<<MAX_CALCR_N<<"\t ipaty="<<ipaty<<endl; 
							exit(0);
						}
						Pvec_1[ipaty][kk][0]	= thisy;		// rapidity (README_PID.md)
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
						Pvec_2[ipaty][kk][0]	= thisy;		// rapidity (README_PID.md)
						Pvec_2[ipaty][kk][1]	= thisphi;
						Pvec_2[ipaty][kk][2]	= thispt;
						++calcr_n_2[ipaty];
					} 	// end accept2
				}	// end pairtypes loop
			}	// end nocorrelations
			//
		}	// end track loop
		//---- README_PID.md sec 8: p-pi ULS ridge diagnostic (booking above)
		for (int ip=0;ip<(*ntr);ip++){
			if (ridgePID[ip]!=2) continue;
			for (int jp=0;jp<(*ntr);jp++){
				if (ridgePID[jp]!=0 || chg[ip]*chg[jp]>=0) continue;
				int c		= (chg[ip]>0) ? 0 : 1;
				if (fabs(ridgeY[ip]-ridgeY[jp])>=0.7) continue;
				int ineg	= (chg[ip]<0) ? ip : jp, ipos = (chg[ip]<0) ? jp : ip;
				double dphiD	= phi[ineg]-phi[ipos];
				while (dphiD> M_PI) dphiD -= 2*M_PI;
				while (dphiD<-M_PI) dphiD += 2*M_PI;
				dphiD	*= 180./M_PI;
				hRidge_dphi[c]->Fill(dphiD);
				int b	= (dphiD>=-10. && dphiD<0.) ? 0 : ((dphiD>=0. && dphiD<10.) ? 1 : -1);
				if (b<0) continue;
				double px1=pt[ip]*cos(phi[ip]), py1=pt[ip]*sin(phi[ip]), pz1=pt[ip]*sinh(eta[ip]);
				double px2=pt[jp]*cos(phi[jp]), py2=pt[jp]*sin(phi[jp]), pz2=pt[jp]*sinh(eta[jp]);
				double p1=sqrt(px1*px1+py1*py1+pz1*pz1), p2=sqrt(px2*px2+py2*py2+pz2*pz2);
				auto minv	= [&](double m1, double m2){
					double e1=sqrt(p1*p1+m1*m1), e2=sqrt(p2*p2+m2*m2);
					double m2t=(e1+e2)*(e1+e2)-(px1+px2)*(px1+px2)-(py1+py2)*(py1+py2)-(pz1+pz2)*(pz1+pz2);
					return m2t>0. ? sqrt(m2t) : 0.;
				};
				const double me	= 0.000511;
				hRidge_Mppi[b][c]	->Fill(minv(Species_mass[2],Species_mass[0]));
				hRidge_Mee[b][c]	->Fill(minv(me,me));
				double cosop	= (px1*px2+py1*py2+pz1*pz2)/(p1*p2);
				hRidge_open[b][c]	->Fill(acos(std::max(-1.,std::min(1.,cosop)))*180./M_PI);
				hRidge_prat[b][c]	->Fill(p2/p1);
				hRidge_skf[b][c]	->Fill(SiSplitScore(&siclukey[ip*7],&siclukey[jp*7],layermask[ip],layermask[jp]));
				hRidge_dcap[b][c]	->Fill(sqrt(dcaxy[ip]*dcaxy[ip]+dcaz[ip]*dcaz[ip]));
				hRidge_dcapi[b][c]	->Fill(sqrt(dcaxy[jp]*dcaxy[jp]+dcaz[jp]*dcaz[jp]));
				hRidge_ptp[b][c]	->Fill(pt[ip]);
			}
		}
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
			//---- README_PID.md: rapidity for CalcRm, from the V0 4-vector (px,py,pz,ene are always good)
			double thisy	= 0.5*log((v0ene[iv0]+v0pz[iv0])/(v0ene[iv0]-v0pz[iv0]));
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
			//---- README_PID.md sec 12: only V0s in their mass peak enter the pairs (their daughters are out of the
			//---- track lists); an off-peak candidate would be paired with its own daughter
			const int kv	= (v0pid[iv0]==310) ? 0 : (v0pid[iv0]==3122) ? 1 : (v0pid[iv0]==-3122) ? 2 : -1;
			const bool v0pair	= v0InPeak(iv0);
			if (kv>=0){ ++nv0All[kv]; if (v0pair) ++nv0Paired[kv]; }
			if (!NOCORRELATIONS && v0pair){
				for (int ipaty=0;ipaty<NPairTypes;ipaty++){	
					//---- check if this track is particle 1 for this pair type
					bool accept1	= false;
					if (thisParticleID==PairTypes_Info[ipaty][0]) accept1 = true;
					if (accept1){
						if (thispt < Species_ptmin[thisSpecies]) accept1	= false;
						if (thispt >=Species_ptmax[thisSpecies]) accept1	= false;
						if (thiseta< -pairEtaEdge[ipaty] || thiseta>=pairEtaEdge[ipaty]) accept1	= false;	// detector edge
						if (thisy  <  pairYL1[ipaty]   || thisy  >= pairYU1[ipaty]   ) accept1	= false;	// class window (README_PID.md)
					}
					//---- check if this track is particle 2 for this pair type
					bool accept2	= false;
					if (thisParticleID==PairTypes_Info[ipaty][1]) accept2 = true;
					if (accept2){
						if (thispt < Species_ptmin[thisSpecies]) accept2	= false;
						if (thispt >=Species_ptmax[thisSpecies]) accept2	= false;
						if (thiseta< -pairEtaEdge[ipaty] || thiseta>=pairEtaEdge[ipaty]) accept2	= false;	// detector edge
						if (thisy  <  pairYL2[ipaty]   || thisy  >= pairYU2[ipaty]   ) accept2	= false;	// class window (README_PID.md)
					}
					if (accept1){		// this track is "Particle 1" in this PairType...
						int kk = calcr_n_1[ipaty];
						if (kk== MAX_CALCR_N-1){ 
							cout<<"reader::Loop -- PVEC1 FULL .. max="<<MAX_CALCR_N<<"\t ipaty="<<ipaty<<endl; 
							exit(0);
						}
						//cout<<"Incrementing 1... ipaty="<<ipaty<<"  k="<<kk<<"  n1="<<calcr_n_1[ipaty]<<" \t "<<ety<<endl;
						Pvec_1[ipaty][kk][0]	= thisy;		// rapidity (README_PID.md)
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
						Pvec_2[ipaty][kk][0]	= thisy;		// rapidity (README_PID.md)
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
		//---- README_CQcomparison.md sec 2.1-2.2: V0 daughter sharing / duplicates and <pT> vs N_ch (sibling only, every event,
		//---- independent of the nchLLHH class). Q = 2k* exactly as CalcRm::PairInfo's dq, 4-vectors from (pT, y, phi) and the
		//---- assigned masses.
		{
			auto v0kind	= [&](int iv0){ return (v0pid[iv0]==310) ? 0 : (v0pid[iv0]==3122) ? 1 : (v0pid[iv0]==-3122) ? 2 : -1; };
			auto v0yOf	= [&](int iv0){ return 0.5*log((v0ene[iv0]+v0pz[iv0])/(v0ene[iv0]-v0pz[iv0])); };
			auto v0ptOf	= [&](int iv0){ return sqrt(v0px[iv0]*v0px[iv0]+v0py[iv0]*v0py[iv0]); };
			auto inList	= [&](int it, int k){	// track it enters the pair types as PID k (0 pi, 1 K, 2 p)
				return it>=0 && it<(*ntr) && ridgePID[it]==k && pt[it]>=Species_ptmin[k] && fabs(ridgeY[it])<Species_yu[k];
			};
			auto v0InList	= [&](int iv0){ int kv = v0kind(iv0); return kv>=0 && v0InPeak(iv0) && v0ptOf(iv0)>=Species_ptmin[5+kv] && fabs(v0yOf(iv0))<Species_yu[5+kv]; };
			auto dRof	= [&](int a, int b){
				double dp	= phi[a]-phi[b];
				while (dp> M_PI) dp -= 2*M_PI;
				while (dp<-M_PI) dp += 2*M_PI;
				double de	= eta[a]-eta[b];
				return sqrt(dp*dp+de*de);
			};
			auto dqOf	= [&](double pt1, double y1, double ph1, double m1, double pt2, double y2, double ph2, double m2){
				double mt1=sqrt(pt1*pt1+m1*m1), mt2=sqrt(pt2*pt2+m2*m2);
				double px1=pt1*cos(ph1), py1=pt1*sin(ph1), pz1=mt1*sinh(y1), e1=mt1*cosh(y1);
				double px2=pt2*cos(ph2), py2=pt2*sin(ph2), pz2=mt2*sinh(y2), e2=mt2*cosh(y2);
				double qinv2	= (e1-e2)*(e1-e2) - (px1-px2)*(px1-px2) - (py1-py2)*(py1-py2) - (pz1-pz2)*(pz1-pz2);
				double minv2	= (e1+e2)*(e1+e2) - (px1+px2)*(px1+px2) - (py1+py2)*(py1+py2) - (pz1+pz2)*(pz1+pz2);
				if (minv2<=0.) return 0.;
				double Q	= (m1*m1-m2*m2)/sqrt(minv2);
				return sqrt(std::max(0.,Q*Q-qinv2));
			};
			//---- daughter index of V0 iv0 with charge sign q (0 +, 1 -), or -1
			auto dauOf	= [&](int iv0, int q){
				const int itd[2]	= {v0indtr1[iv0], v0indtr2[iv0]};
				for (int kd=0;kd<2;kd++){
					int it	= itd[kd];
					if (it<0 || it>=(*ntr)) continue;
					if ((q==0 && chg[it]>0) || (q==1 && chg[it]<0)) return it;
				}
				return -1;
			};
			auto shareDau	= [&](int a, int b){
				const int ia[2]	= {v0indtr1[a], v0indtr2[a]}, ib[2] = {v0indtr1[b], v0indtr2[b]};
				for (int i=0;i<2;i++) for (int j=0;j<2;j++) if (ia[i]>=0 && ia[i]==ib[j]) return true;
				return false;
			};
			//---- daughters of off-peak V0 candidates (they stay in the track lists)
			std::vector<char> offDau((*ntr),0);
			for (int iv0=0;iv0<(*nv0);iv0++){
				if (v0kind(iv0)<0 || v0InPeak(iv0)) continue;
				const int itd[2]	= {v0indtr1[iv0], v0indtr2[iv0]};
				for (int kd=0;kd<2;kd++) if (itd[kd]>=0 && itd[kd]<(*ntr)) offDau[itd[kd]] = 1;
			}
			//---- 1. V0-V0 sharing
			for (int iv0=0;iv0<(*nv0);iv0++){
				int k1	= v0kind(iv0);
				if (k1<0) continue;
				bool p1	= v0InPeak(iv0);
				for (int jv0=0;jv0<(*nv0);jv0++){
					if (jv0==iv0) continue;
					int k2	= v0kind(jv0);
					if (k2<0) continue;
					bool p2	= v0InPeak(jv0);
					if (p1 && p2 && jv0>iv0){
						hV0pairs->Fill(std::min(k1,k2),std::max(k1,k2));
						if (shareDau(iv0,jv0)) hV0share->Fill(std::min(k1,k2),std::max(k1,k2));
					}
					if (p1 && !p2 && shareDau(iv0,jv0)) hV0shareOff->Fill(k1,k2);
				}
			}
			//---- 2. nearest same-charge, same-PID track in the lists to each in-peak daughter
			for (int iv0=0;iv0<(*nv0);iv0++){
				int kv	= v0kind(iv0);
				if (kv<0 || !v0InPeak(iv0)) continue;
				for (int q=0;q<2;q++){
					int d	= dauOf(iv0,q);
					if (d<0) continue;
					int sp	= (kv==1 && q==0) || (kv==2 && q==1) ? 2 : 0;	// the (anti)proton, else a pion
					double best	= 1e9; int jb = -1;
					for (int it=0;it<(*ntr);it++){
						if (it==d || !inList(it,sp) || chg[it]*chg[d]<=0) continue;
						double r	= dRof(it,d);
						if (r<best){ best = r; jb = it; }
					}
					if (jb<0) continue;
					hDauNear_dR[kv][q]	->Fill(std::min(best,0.2999));
					hDauNear_dRdpt[kv][q]->Fill(best,fabs(pt[jb]-pt[d])/pt[d]);
					if (best<CQC_DRDUP) hDauNear_skf[kv][q]->Fill(SiSplitScore(&siclukey[jb*7],&siclukey[d*7],layermask[jb],layermask[d]));
				}
			}
			//---- 3. p-Lambda / pbar-Lbar sibling Q by the (anti)proton's category; 4. Lambda-Lambda
			int nLin[2]	= {0,0};
			for (int iv0=0;iv0<(*nv0);iv0++){
				int kv	= v0kind(iv0);
				if (kv<1 || !v0InList(iv0)) continue;
				int a	= kv-1;
				++nLin[a];
				int own	= dauOf(iv0,a);		// Lambda: the + daughter (p); Lbar: the - daughter (pbar)
				double vpt=v0ptOf(iv0), vy=v0yOf(iv0), vph=atan2(v0py[iv0],v0px[iv0]);
				for (int ip=0;ip<(*ntr);ip++){
					if (!inList(ip,2) || (a==0 && chg[ip]<=0) || (a==1 && chg[ip]>=0)) continue;
					double Q	= dqOf(pt[ip],ridgeY[ip],phi[ip],Species_mass[2],vpt,vy,vph,Species_mass[6+a]);
					double r	= (own>=0) ? dRof(ip,own) : 9.;
					int c		= (r<CQC_DRDUP) ? 0 : (offDau[ip] ? 1 : 2);
					hpLa_Q[a][c]	->Fill(Q);
					hpLa_QdR[a]		->Fill(Q,std::min(r,0.4999));
				}
				for (int jv0=iv0+1;jv0<(*nv0);jv0++){
					if (v0kind(jv0)!=kv || !v0InList(jv0)) continue;
					double Q	= dqOf(vpt,vy,vph,Species_mass[6+a],v0ptOf(jv0),v0yOf(jv0),atan2(v0py[jv0],v0px[jv0]),Species_mass[6+a]);
					bool nearD	= false;
					for (int q=0;q<2;q++){
						int d1=dauOf(iv0,q), d2=dauOf(jv0,q);
						if (d1>=0 && d2>=0 && d1!=d2 && dRof(d1,d2)<CQC_DRDUP) nearD = true;
					}
					int c		= shareDau(iv0,jv0) ? 0 : (nearD ? 1 : 2);
					hLaLa_Q[a][c]->Fill(std::min(Q,3.999));
				}
			}
			for (int a=0;a<2;a++) hnLApeak[a]->Fill(nLin[a]);
			//---- <pT> vs N_ch
			for (int it=0;it<(*ntr);it++){
				int k	= ridgePID[it];
				if (k<0 || k>2 || !inList(it,k)) continue;
				hptNch[k][chg[it]>0?0:1]->Fill(ntrkept,pt[it]);
			}
			for (int iv0=0;iv0<(*nv0);iv0++) if (v0InList(iv0)) hptNchV0[v0kind(iv0)]->Fill(ntrkept,v0ptOf(iv0));
		}
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
		//---- README_CQcomparison.md sec 2.1: only events in the nchLLHH multiplicity class reach CalcRm
		++nNchSeen;
		const bool inNchClass	= (ntrkept>=valNchLo && ntrkept<=valNchHi);
		if (inNchClass) ++nNchKept;
		if (!NOCORRELATIONS && inNchClass){
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
					R[ipaty]	->SetCurrentNch(ntrkept);			// README_SplitTracks573 sec 34
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
	cout<<"V0s in the pair types (README_PID.md sec 12, in their mass peak): K0s "<<nv0Paired[0]<<" of "<<nv0All[0]
		<<", Lambda "<<nv0Paired[1]<<" of "<<nv0All[1]<<", Lbar "<<nv0Paired[2]<<" of "<<nv0All[2]
		<<"; in-peak daughters that indv0 did not flag: "<<nPeakDauNotIndv0<<endl;
	cout<<"multiplicity class (README_CQcomparison.md sec 2.1): "<<valNchLo<<" <= N_ch <= "<<valNchHi<<": "<<nNchKept<<" of "<<nNchSeen<<" events reached CalcRm"<<endl;
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
	TH1D *hQsibW[NPairTypes]				= {0};		// long-Q test copies, 0-8 GeV, Zvtx-summed (README_CQcomparison sec 2.3)
	TH1D *hQmixW[NPairTypes]				= {0};
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
			hQsibW[ipaty]	= (TH1D*)R[ipaty]->GethQsibW();
			hQsibW[ipaty]	->SetTitle(Form("%s%s, Q_{sib} 0-8 GeV (long-Q test copy);Q_{inv} (GeV)",part1name.Data(),part2name.Data()));
			hQsibW[ipaty]	->SetName(Form("hQsibW_%d",ipaty));
			hQmixW[ipaty]	= (TH1D*)R[ipaty]->GethQmixW();
			hQmixW[ipaty]	->SetTitle(Form("%s%s, Q_{mix} 0-8 GeV (long-Q test copy, #Sigma_{z});Q_{inv} (GeV)",part1name.Data(),part2name.Data()));
			hQmixW[ipaty]	->SetName(Form("hQmixW_%d",ipaty));
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
			for (int sm=0;sm<2;sm++) for (int is=0;is<2;is++) for (int ir=0;ir<4;ir++){	// README_SplitTracks573 sec 11
				TH2D* ht	= R[ipaty]->GethTTR(sm,is,ir);
				ht->SetName(Form("hTTR_%s_s%d_r%d_%d",sm?"M":"S",is,ir,ipaty));
				ht->SetTitle(Form("%s%s, %s pairs, sign %s, %s;dy;#Delta#phi* (deg)",part1name.Data(),part2name.Data(),sm?"mixed":"sibling",is?"-":"+",
					ir==0?"R=0.30 m":ir==1?"R=0.50 m":ir==2?"R=0.70 m":"min over R=0.30-0.78 m"));
			}
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
	++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.07);
	text[itext]->DrawLatex(0.88,0.27,Form("looper veto %s: tracks flagged=%ld (sec 35)",doLooperVeto?"ON":"OFF",nFlagged_Looper));
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileNameO.Data());

	//---- README_SplitTracks573 sec 10: LS pregate before/after, in bins of abs(1/pt1-1/pt2). Rows: all LS
	//---- pion pairs, SiSplitScore>=cut pairs (the split candidates), pairs with both tracks kept. Red: the
	//---- gate at the bin's two edges (the current gate; with the fixed default both are the same ellipse);
	//---- grey dashed: the old fixed ellipse (0.022 x 4.1 deg).
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1400,900);
	ccan[ican]->cd();
	TPad* gvTop	= new TPad("gvTop","",0.,0.95,1.,1.);	// strip for the gate definition
	TPad* gvMain	= new TPad("gvMain","",0.,0.,1.,0.95);
	gvTop->Draw(); gvMain->Draw();
	gvMain->Divide(NGATEVIS,3,0.0001,0.0001);
	{
		//---- row 1: all LS pairs in plain dphi (index order, so the split lobes are mirrored), old
		//---- ellipse for reference. Rows 2-3: SKF>=cut and kept pairs in the dps frame (abs(dphi) - dphi0),
		//---- where the dps gate is one ellipse centred at 0. A gate centred at dphi=0 (fixed / dpg) is
		//---- drawn in the same frame at the bin's central abs(1/pt1-1/pt2): the curve
		//---- abs(dphi) = W sqrt(1-(deta/a)^2), shifted by -dphi0 (it reaches down to abs(dphi)=0, i.e. -dphi0).
		TH2D** rows[3]	= { hGateVis_all, hGateVis_hiRes, hGateVis_survRes };
		for (int ir=0;ir<3;ir++) for (int ib=0;ib<NGATEVIS;ib++){
			gvMain->cd(1+ir*NGATEVIS+ib);
			gPad->SetLogz(1); gPad->SetRightMargin(0.13); gPad->SetTopMargin(0.08);
			rows[ir][ib]->SetMinimum(0.5);
			if (ir==0 && hGateVis_dinvHi[ib]->GetEntries()>0)
				rows[ir][ib]->SetTitle(Form("%s, mean %.2f",rows[ir][ib]->GetTitle(),hGateVis_dinvHi[ib]->GetMean()));
			rows[ir][ib]->Draw("colz");
			if (ir==0){
				//---- the pregate in plain dphi, at the MEAN abs(1/pt1-1/pt2) of this column's split candidates
				//---- (each pair's own gate differs slightly; rows 2-3 show it exactly, per pair)
				double dm	= hGateVis_dinvHi[ib]->GetEntries()>0 ? hGateVis_dinvHi[ib]->GetMean() : 0.5*(GATEVIS_EDGE[ib]+GATEVIS_EDGE[ib+1]);
				double pA	= 1.0/(1.0+dm);
				{
					//---- user 2026-09-27: centre each ellipse on the MEASURED peak (mean abs(dphi) of this column's
					//---- SKF>=cut pairs) and draw it at the envelope of the column's per-pair gates: half-height
					//---- w + half the spread of dphi0 across the column (dphi0 at the two bin edges).
					double pk	= 0.;
					{ TH2D* hh = hGateVis_hi[ib]; double sw=0, sy=0;
					  for (int ix=1;ix<=hh->GetNbinsX();ix++) for (int iy=1;iy<=hh->GetNbinsY();iy++){ double v=hh->GetBinContent(ix,iy); sw+=v; sy+=v*fabs(hh->GetYaxis()->GetBinCenter(iy)); }
					  pk = sw>0 ? sy/sw : DPsCentre(pA,1.0)*180.0/M_PI; }
					double cLo	= DPsCentre(1.0/(1.0+GATEVIS_EDGE[ib]),1.0)*180.0/M_PI;
					double cHi	= DPsCentre(1.0/(1.0+GATEVIS_EDGE[ib+1]),1.0)*180.0/M_PI;
					double hw	= DPsWidth(cHi*M_PI/180.0)*180.0/M_PI + 0.5*(cHi-cLo);	// sec 33: width grows with dphi0
					for (int sg=-1;sg<=1;sg+=2){
						TEllipse* el	= new TEllipse(0.,sg*pk,valPregateDEta,hw);
						el->SetFillStyle(0); el->SetLineColor(2); el->SetLineWidth(2); el->Draw();
					}
				}
				TEllipse* el0	= new TEllipse(0.,0.,0.022,4.1);
				el0->SetFillStyle(0); el0->SetLineColor(kGray+2); el0->SetLineWidth(2); el0->SetLineStyle(2); el0->Draw();
				continue;
			}
			TLine* l0	= new TLine(-0.05,0.,0.05,0.); l0->SetLineColor(kGray+2); l0->SetLineStyle(3); l0->Draw();
			double dmid	= 0.5*(GATEVIS_EDGE[ib]+GATEVIS_EDGE[ib+1]);
			double ptA	= 1.0/(1.0+dmid);						// abs(1/ptA - 1/1) = dmid
			double c	= DPsCentre(ptA,1.0)*180.0/M_PI;
			{
				TEllipse* el	= new TEllipse(0.,0.,valPregateDEta,DPsWidth(c*M_PI/180.0)*180.0/M_PI);
				el->SetFillStyle(0); el->SetLineColor(2); el->SetLineWidth(2); el->Draw();
			}
			// the old fixed ellipse in this frame (grey dashed), at the same central abs(1/pt1-1/pt2)
			TGraph* g0	= new TGraph();
			for (int k=0;k<=100;k++){ double x=-0.022+0.044*k/100.; double u=std::max(0.0,1.0-(x/0.022)*(x/0.022)); g0->SetPoint(g0->GetN(),x,4.1*sqrt(u)-c); }
			g0->SetPoint(g0->GetN(),0.022,-c); g0->SetPoint(g0->GetN(),-0.022,-c);
			g0->SetLineColor(kGray+2); g0->SetLineWidth(2); g0->SetLineStyle(2); g0->Draw("L");
		}
		gvTop->cd();
		++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.40); text[itext]->SetTextAlign(22);
		TString sgate	= Form("#pm max(%.1f, %.0f(%.2f+%.2f#Delta#phi_{0})) around #Delta#phi_{0}(R=%.0f cm)",valDPsW,PREGATE_NSIG,PREGATE_S0,PREGATE_S1,100*valDPsR);
		text[itext]->DrawLatex(0.5,0.5,Form("LS #pi pregate: #Delta#phi %s deg, #Delta#eta_{max}=%.3f.  Row 1: all pairs, gate at column mean.  Rows 2-3: SKF#geq%.2f / kept, per pair in |#Delta#phi|-#Delta#phi_{0}.  Grey: old",sgate.Data(),valPregateDEta,valSiKeyCut));
	}
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());

	//---- user 2026-09-28: the pregate in fine slices of dphi0 (booking: hGateFine_*), three pages: SKF>=cut pairs,
	//---- SKF>=cut pairs whose tracks both survive, all kept pairs. y = abs(dphi), the variable the gate cuts on,
	//---- so the gate of a pair with centre dphi0 is exactly (deta/a)^2 + ((abs(dphi)-dphi0)/w)^2 < 1, cut off at
	//---- abs(dphi) = 0 where dphi0 < w. Solid red: the gate at the slice centre; thin dashed: at the slice edges
	//---- (every pair's own gate lies between them). Grey dashed: the old fixed ellipse (0.022 x 4.1 deg).
	{
		auto gateCurve	= [&](double a, double c, double w, int col, int lw, int ls){
			TGraph* g	= new TGraph();
			for (int k=0;k<=200;k++){ double t=M_PI*k/100.; double x=a*cos(t), y=c+w*sin(t); g->SetPoint(g->GetN(),x,std::max(0.0,y)); }
			g->SetLineColor(col); g->SetLineWidth(lw); g->SetLineStyle(ls); g->Draw("L");
		};
		//---- page 0 = the gate page's row 1 (all LS pairs, signed dphi) in fine slices: the gate is the two
		//---- ellipses at +-dphi0 (it cuts abs(dphi)), each pair's own gate between the dashed slice-edge curves
		{
			auto gateEll	= [&](double a, double c, double w, int col, int lw, int ls){
				TGraph* g	= new TGraph();
				for (int k=0;k<=200;k++){ double t=2*M_PI*k/200.; g->SetPoint(g->GetN(),a*cos(t),c+w*sin(t)); }
				g->SetLineColor(col); g->SetLineWidth(lw); g->SetLineStyle(ls); g->Draw("L");
			};
			++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1400,900);
			ccan[ican]->cd();
			TPad* gaTop		= new TPad("gaTop","",0.,0.95,1.,1.);
			TPad* gaMain	= new TPad("gaMain","",0.,0.,1.,0.95);
			gaTop->Draw(); gaMain->Draw();
			gaMain->Divide(4,4,0.0001,0.0001);
			for (int k=0;k<NGATEFINE;k++){
				gaMain->cd(1+k);
				gPad->SetLogz(1); gPad->SetRightMargin(0.13); gPad->SetTopMargin(0.09);
				TH2D* h	= hGateFine_all[k];
				h->SetMinimum(0.5); h->SetStats(0);
				h->SetTitle(Form("%s, N=%.0f",h->GetTitle(),h->GetEntries()));
				h->Draw("colz");
				double c0	= k*GATEFINE_W, c1 = (k+1)*GATEFINE_W;
				for (int sg=-1;sg<=1;sg+=2){
					gateEll(valPregateDEta,sg*c0,DPsWidth(c0*M_PI/180.0)*180.0/M_PI,2,1,2);
					gateEll(valPregateDEta,sg*c1,DPsWidth(c1*M_PI/180.0)*180.0/M_PI,2,1,2);
					gateEll(valPregateDEta,sg*0.5*(c0+c1),DPsWidth(0.5*(c0+c1)*M_PI/180.0)*180.0/M_PI,2,2,1);
				}
				gateEll(0.022,0.,4.1,kGray+2,2,2);
			}
			gaTop->cd();
			++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.36); text[itext]->SetTextAlign(22);
			text[itext]->DrawLatex(0.5,0.5,Form("LS #pi pregate, %.1f#circ slices of #Delta#phi_{0}(R=%.0f cm): all LS pairs, signed #Delta#phi.  Red: (#Delta#eta/%.3f)^{2}+((|#Delta#phi|-#Delta#phi_{0})/w)^{2}<1, w = max(%.1f#circ, 3#sigma(#Delta#phi_{0})) at #pm#Delta#phi_{0}, slice centre / edges.  Grey: old",GATEFINE_W,100*valDPsR,valPregateDEta,valDPsW));
			ccan[ican]->cd(); ccan[ican]->Update();
			ccan[ican]->Print(OutputFileName.Data());
		}
		TH2D** pages[3]		= { hGateFine_hi, hGateFine_hiSurv, hGateFine_surv };
		const char* pname[3]	= { "SiSplitScore #geq cut (split candidates)", "SiSplitScore #geq cut, both tracks kept (missed)", "all kept pairs" };
		for (int ip=0;ip<3;ip++){
			++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1400,900);
			ccan[ican]->cd();
			TPad* gfTop		= new TPad(Form("gfTop%d",ip),"",0.,0.95,1.,1.);
			TPad* gfMain	= new TPad(Form("gfMain%d",ip),"",0.,0.,1.,0.95);
			gfTop->Draw(); gfMain->Draw();
			gfMain->Divide(4,4,0.0001,0.0001);
			for (int k=0;k<NGATEFINE;k++){
				gfMain->cd(1+k);
				gPad->SetLogz(1); gPad->SetRightMargin(0.13); gPad->SetTopMargin(0.09);
				TH2D* h	= pages[ip][k];
				h->SetMinimum(0.5); h->SetStats(0);
				h->SetTitle(Form("%s, N=%.0f",h->GetTitle(),h->GetEntries()));
				h->Draw("colz");
				double c0	= k*GATEFINE_W, c1 = (k+1)*GATEFINE_W;
				gateCurve(valPregateDEta,c0,DPsWidth(c0*M_PI/180.0)*180.0/M_PI,2,1,2);
				gateCurve(valPregateDEta,c1,DPsWidth(c1*M_PI/180.0)*180.0/M_PI,2,1,2);
				gateCurve(valPregateDEta,0.5*(c0+c1),DPsWidth(0.5*(c0+c1)*M_PI/180.0)*180.0/M_PI,2,2,1);
				gateCurve(0.022,0.,4.1,kGray+2,2,2);
			}
			gfTop->cd();
			++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.36); text[itext]->SetTextAlign(22);
			text[itext]->DrawLatex(0.5,0.5,Form("LS #pi pregate, %.1f#circ slices of #Delta#phi_{0}(R=%.0f cm): %s.  Red: (#Delta#eta/%.3f)^{2}+((|#Delta#phi|-#Delta#phi_{0})/w)^{2}<1, w = max(%.1f#circ, 3#sigma(#Delta#phi_{0})), slice centre / edges.  Grey: old",GATEFINE_W,100*valDPsR,pname[ip],valPregateDEta,valDPsW));
			ccan[ican]->cd(); ccan[ican]->Update();
			ccan[ican]->Print(OutputFileName.Data());
		}
	}

	//---- README_SplitTracks573 sec 25: cosmic test page (hCos_*): signal window (black), ULS control (blue), LS control (red),
	//---- 1-D panels normalized to the signal window's entries
	{
		++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1400,900);
		ccan[ican]->cd(); ccan[ican]->Divide(4,3,0.0001,0.0001);
		TH1D** one[7]	= { hCos_axis, hCos_ptAsym, hCos_eta, hCos_npi, hCos_vtxntr, hCos_pt, hCos_etotoh };
		const int col[NCOS]	= { 1, 4, 2 };
		for (int k=0;k<7;k++){
			ccan[ican]->cd(1+k);
			double n0	= one[k][0]->Integral();
			TH1D* hd[NCOS];	// drawn copies: the stored histograms stay raw counts
			for (int w=0;w<NCOS;w++){
				hd[w]	= (TH1D*)one[k][w]->Clone(Form("%s_draw",one[k][w]->GetName())); hd[w]->SetDirectory(0);
				if (w>0 && hd[w]->Integral()>0 && n0>0) hd[w]->Scale(n0/hd[w]->Integral());
				hd[w]->SetLineColor(col[w]); hd[w]->SetMarkerColor(col[w]); hd[w]->SetStats(0);
			}
			double mx=0; for (int w=0;w<NCOS;w++) mx=std::max(mx,hd[w]->GetMaximum()); hd[0]->SetMaximum(1.15*mx);
			for (int w=0;w<NCOS;w++) hd[w]->Draw(w==0 ? "hist" : "hist same");
		}
		for (int w=0;w<NCOS;w++){ ccan[ican]->cd(8+w); gPad->SetLogz(1); hCos_dcaxy[w]->SetStats(0); hCos_dcaxy[w]->Draw("colz"); }
		ccan[ican]->cd(11); gPad->SetLogz(1); hCos_dcaz[0]->SetStats(0); hCos_dcaz[0]->Draw("colz");
		ccan[ican]->cd(12); gPad->SetLogz(1); hCos_dcaz[1]->SetStats(0); hCos_dcaz[1]->Draw("colz");
		ccan[ican]->cd(); ccan[ican]->Update();
		ccan[ican]->Print(OutputFileName.Data());
	}
	//---- sec 25.1: p1 = -p2 pairs. Top: overlays as above (black w0 = OS >= 170 deg, blue w1 = OS 150-160, red w2 = LS >= 170;
	//---- w1, w2 scaled to w0's entries). Bottom: w0/w1 (raw counts, / the overall w0/w1). Cosmic halves: rel. p sum ~ 0,
	//---- broad Minv at 2|p| (> 2 GeV for most muons), axis peaked at 90 deg (vertical). A decay at rest: a fixed Minv.
	{
		++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1400,800);
		ccan[ican]->cd(); ccan[ican]->Divide(4,2,0.0001,0.0001);
		TH1D** one[4]	= { hCos_psum, hCos_minv, hCos_axisBB, hCos_minvBB };
		const int col[NCOS]	= { 1, 4, 2 };
		const char* wl[NCOS]	= { "OS, |#Delta#phi|#geq170#circ", "OS, 150-160#circ (control)", "LS, #geq170#circ (control)" };
		double r01	= (hCos_axis[1]->Integral()>0) ? hCos_axis[0]->Integral()/hCos_axis[1]->Integral() : 1.;
		for (int k=0;k<4;k++){
			ccan[ican]->cd(1+k);
			double n0	= one[k][0]->Integral();
			TH1D* hd[NCOS];
			for (int w=0;w<NCOS;w++){
				hd[w]	= (TH1D*)one[k][w]->Clone(Form("%s_draw",one[k][w]->GetName())); hd[w]->SetDirectory(0);
				if (w>0 && hd[w]->Integral()>0 && n0>0) hd[w]->Scale(n0/hd[w]->Integral());
				hd[w]->SetLineColor(col[w]); hd[w]->SetStats(0);
			}
			double mx=0; for (int w=0;w<NCOS;w++) mx=std::max(mx,hd[w]->GetMaximum()); hd[0]->SetMaximum(1.15*mx); hd[0]->SetMinimum(0.);
			for (int w=0;w<NCOS;w++) hd[w]->Draw(w==0 ? "hist" : "hist same");
			if (k==0){ TLegend *lg = new TLegend(0.35,0.62,0.89,0.89); lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.042);
				for (int w=0;w<NCOS;w++) lg->AddEntry(hd[w],wl[w],"l"); lg->Draw(); }
			ccan[ican]->cd(5+k);
			TH1D *r	= (TH1D*)one[k][0]->Clone(Form("%s_r01",one[k][0]->GetName())); r->SetDirectory(0);
			r->Divide(one[k][1]); if (r01>0) r->Scale(1./r01);
			r->SetTitle(Form("w0/w1 / overall (%.3f);%s;ratio",r01,one[k][0]->GetXaxis()->GetTitle()));
			r->SetStats(0); r->SetMarkerStyle(20); r->SetMarkerSize(0.5); r->SetMinimum(0.5); r->SetMaximum(2.0);
			r->Draw("E1");
			TLine *l1	= new TLine(r->GetXaxis()->GetXmin(),1.,r->GetXaxis()->GetXmax(),1.); l1->SetLineColor(kGray); l1->Draw();
			r->Draw("E1 same");
		}
		ccan[ican]->cd(); ccan[ican]->Update();
		ccan[ican]->Print(OutputFileName.Data());
	}

	//---- README_SplitTracks573 sec 34: the +-15 deg ULS notch MONITOR (a ULS sibling-pair deficit at 10-20 deg vertex dphi,
	//---- not made by any Corral cleaner, sec 32-33). Pions, abs(dy) < 1, S/M each / its 25 <= abs(dphi) < 50 deg level.
	//---- Top: ULS (0+1) and LS (2+3) S/M vs dphi, the notch band shaded; ULS S and M separately. Bottom: the notch band
	//---- (10-20 deg / 25-50 deg) vs pair <pt> and vs accepted N_ch, ULS (red) and LS (black); ULS S/M vs dphi in N_ch groups.
	if (!NOCORRELATIONS){
		gStyle->SetOptTitle(1);
		++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1600,800);
		ccan[ican]->cd(); ccan[ican]->Divide(4,2,0.0001,0.0001);
		auto sum2	= [&](TH2D* (CalcRm::*get)(int), int sm, int a, int b, const char* nm){
			TH2D *h	= (TH2D*)(R[a]->*get)(sm)->Clone(nm); h->SetDirectory(0); h->Add((R[b]->*get)(sm)); return h; };
		//---- S/M vs dphi (2 deg) of the y rows [y0,y1] of S and M, / its 25-50 deg level
		auto shapeDphi	= [&](TH2D *S, TH2D *M, int y0, int y1, const char* nm){
			TH1D *s1 = S->ProjectionX(Form("%s_s",nm),y0,y1), *m1 = M->ProjectionX(Form("%s_m",nm),y0,y1); s1->Rebin(2); m1->Rebin(2);
			double sr=0,mr=0; for (int i=1;i<=s1->GetNbinsX();i++){ double x=fabs(s1->GetBinCenter(i)); if (x>=25&&x<50){ sr+=s1->GetBinContent(i); mr+=m1->GetBinContent(i); } }
			TH1D *r	= (TH1D*)s1->Clone(nm); r->Reset(); r->SetDirectory(0);
			for (int i=1;i<=s1->GetNbinsX();i++){ double sv=s1->GetBinContent(i), mv=m1->GetBinContent(i); if (sv>0&&mv>0&&sr>0&&mr>0){ double v=(sv/mv)/(sr/mr); r->SetBinContent(i,v); r->SetBinError(i,v*sqrt(1/sv+1/sr)); } }
			delete s1; delete m1; r->SetStats(0); return r; };
		//---- the notch band ratio vs the y axis of (S,M)
		auto bandY	= [&](TH2D *S, TH2D *M, const char* nm){
			TH1D *h	= S->ProjectionY(nm); h->Reset(); h->SetDirectory(0);
			for (int iy=1;iy<=S->GetNbinsY();iy++){ double sb=0,mb=0,sr=0,mr=0;
				for (int ix=1;ix<=S->GetNbinsX();ix++){ double x=fabs(S->GetXaxis()->GetBinCenter(ix)), sv=S->GetBinContent(ix,iy), mv=M->GetBinContent(ix,iy);
					if (x>=10&&x<20){ sb+=sv; mb+=mv; } if (x>=25&&x<50){ sr+=sv; mr+=mv; } }
				if (sb>0&&mb>0&&sr>0&&mr>0){ double v=(sb/mb)/(sr/mr); h->SetBinContent(iy,v); h->SetBinError(iy,v*sqrt(1/sb+1/sr)); } }
			h->SetStats(0); return h; };
		auto one	= [&](TH1D *h, int col, const char* opt){ h->SetLineColor(col); h->SetMarkerColor(col); h->SetMarkerStyle(20); h->SetMarkerSize(0.5); h->Draw(opt); };
		auto band	= [&](double y0, double y1){ for (int sg=-1;sg<=1;sg+=2){ TBox *b = new TBox(sg>0?10.:-20.,y0,sg>0?20.:-10.,y1); b->SetFillColorAlpha(kGray,0.35); b->SetLineWidth(0); b->Draw(); } };
		auto unity	= [&](double x0, double x1){ TLine *l = new TLine(x0,1.,x1,1.); l->SetLineColor(kGray+1); l->Draw(); };
		TH2D *uS = sum2(&CalcRm::GethGapPt,0,0,1,"nm_uS"), *uM = sum2(&CalcRm::GethGapPt,1,0,1,"nm_uM");
		TH2D *lS = sum2(&CalcRm::GethGapPt,0,2,3,"nm_lS"), *lM = sum2(&CalcRm::GethGapPt,1,2,3,"nm_lM");
		TH2D *uSn = sum2(&CalcRm::GethGapNch,0,0,1,"nm_uSn"), *uMn = sum2(&CalcRm::GethGapNch,1,0,1,"nm_uMn");
		TH2D *lSn = sum2(&CalcRm::GethGapNch,0,2,3,"nm_lSn"), *lMn = sum2(&CalcRm::GethGapNch,1,2,3,"nm_lMn");
		int npt	= uS->GetNbinsY(), nnc = uSn->GetNbinsY();
		//---- pad 1: ULS and LS shapes
		ccan[ican]->cd(1);
		TH1D *ru = shapeDphi(uS,uM,1,npt,"nm_ru"), *rl = shapeDphi(lS,lM,1,npt,"nm_rl");
		double notchU	= 0, notchL = 0;
		{ TH1D *bu = bandY(uS,uM,"nm_bu_all"); TH1D *bl = bandY(lS,lM,"nm_bl_all");
		  // all pt: from the summed rows
		  TH2D *uS1 = (TH2D*)uS->RebinY(npt,"nm_uS1"), *uM1 = (TH2D*)uM->RebinY(npt,"nm_uM1"), *lS1 = (TH2D*)lS->RebinY(npt,"nm_lS1"), *lM1 = (TH2D*)lM->RebinY(npt,"nm_lM1");
		  notchU = bandY(uS1,uM1,"nm_nu")->GetBinContent(1); notchL = bandY(lS1,lM1,"nm_nl")->GetBinContent(1); delete bu; delete bl; }
		ru->SetTitle(Form("#pi#pi S/M / (25-50#circ), |dy|<1: notch band 10-20#circ = %.3f (ULS), %.3f (LS);#Delta#phi_{vtx} (deg);ratio",notchU,notchL));
		ru->SetMinimum(0.9); ru->SetMaximum(1.1); ru->Draw("E1"); band(0.9,1.1); unity(-60,60);
		one(ru,kRed,"E1 same"); one(rl,kBlack,"E1 same");
		{ TLegend *lg = new TLegend(0.14,0.14,0.60,0.30); lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.04);
		  lg->AddEntry(ru,"ULS (#pi^{+}#pi^{-}, #pi^{-}#pi^{+})","lp"); lg->AddEntry(rl,"LS (#pi^{+}#pi^{+}, #pi^{-}#pi^{-})","lp"); lg->AddEntry((TObject*)0,"gray: the notch band","");
		  lg->Draw(); }
		//---- pad 2: ULS S and M separately (each / its 25-50 deg mean): which one has the notch
		ccan[ican]->cd(2);
		{ TH1D *s1 = uS->ProjectionX("nm_s1"), *m1 = uM->ProjectionX("nm_m1"); s1->Rebin(2); m1->Rebin(2); s1->SetDirectory(0); m1->SetDirectory(0);
		  double sr=0,mr=0; int nr=0; for (int i=1;i<=s1->GetNbinsX();i++){ double x=fabs(s1->GetBinCenter(i)); if (x>=25&&x<50){ sr+=s1->GetBinContent(i); mr+=m1->GetBinContent(i); ++nr; } }
		  if (sr>0&&mr>0){ s1->Scale(nr/sr); m1->Scale(nr/mr); }
		  s1->SetStats(0); s1->SetTitle("ULS #pi#pi: sibling S (red) and mixed M (blue), each / its 25-50#circ mean;#Delta#phi_{vtx} (deg);/ 25-50#circ mean");
		  s1->SetMinimum(0.4); s1->SetMaximum(1.2); s1->Draw("E1"); band(0.4,1.2); unity(-60,60); one(s1,kRed,"hist same"); one(m1,kBlue,"hist same"); }
		//---- pad 3: ULS S/M in N_ch groups
		ccan[ican]->cd(3);
		{ const int grp[4][2]	= {{1,3},{4,6},{7,8},{9,nnc}}; const int gc[4] = {kBlue,kGreen+2,kOrange+7,kRed};
		  TLegend *lg = new TLegend(0.14,0.14,0.60,0.34); lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.04);
		  for (int g=0;g<4;g++){ TH1D *r = shapeDphi(uSn,uMn,grp[g][0],grp[g][1],Form("nm_rn%d",g));
			if (g==0){ r->SetTitle("ULS #pi#pi S/M / (25-50#circ) in N_{ch} groups;#Delta#phi_{vtx} (deg);ratio"); r->SetMinimum(0.85); r->SetMaximum(1.15); r->Draw("E1"); band(0.85,1.15); unity(-60,60); }
			one(r,gc[g],"E1 same");
			lg->AddEntry(r,Form("N_{ch} %.0f-%.0f",uSn->GetYaxis()->GetBinLowEdge(grp[g][0])+0.5,uSn->GetYaxis()->GetBinUpEdge(grp[g][1])-0.5),"lp"); }
		  lg->Draw(); }
		//---- pads 4 and 7 (sec 34.2): S/M vs the pair's meeting radius R_m (where the tracks meet in phi), / the overall S/M, in the notch band (pad 4)
		//---- and at every dphi (pad 7); gray = the INTT-TPC gap (0.1-0.3 m), where pairs that meet there are lost
		{
			TH2D *xS[2], *xM[2];
			for (int k=0;k<2;k++){ xS[k] = sum2(&CalcRm::GethGapRm,0,k?2:0,k?3:1,Form("nm_xS%d",k)); xM[k] = sum2(&CalcRm::GethGapRm,1,k?2:0,k?3:1,Form("nm_xM%d",k)); }
			for (int pad=0;pad<2;pad++){
				ccan[ican]->cd(pad==0?4:7);
				TH1D *h[2];
				for (int k=0;k<2;k++){
					TH2D *S = xS[k], *M = xM[k];
					double sT = S->Integral(), mT = M->Integral();
					int a1 = S->GetXaxis()->FindBin(-19.99), a2 = S->GetXaxis()->FindBin(-10.01), b1 = S->GetXaxis()->FindBin(10.01), b2 = S->GetXaxis()->FindBin(19.99);
					h[k]	= S->ProjectionY(Form("nm_rx%d%d",pad,k)); h[k]->Reset(); h[k]->SetDirectory(0);
					for (int iy=1;iy<S->GetNbinsY();iy++){	// the last bin = no crossing, left out
						double sv = pad==0 ? S->Integral(a1,a2,iy,iy)+S->Integral(b1,b2,iy,iy) : S->Integral(1,S->GetNbinsX(),iy,iy);
						double mv = pad==0 ? M->Integral(a1,a2,iy,iy)+M->Integral(b1,b2,iy,iy) : M->Integral(1,M->GetNbinsX(),iy,iy);
						if (sv<200 || mv<=0 || sT<=0 || mT<=0) continue;
						double v = (sv/mv)/(sT/mT); h[k]->SetBinContent(iy,v); h[k]->SetBinError(iy,v/sqrt(sv));
					}
					h[k]->SetStats(0);
				}
				h[0]->SetTitle(Form("S/M vs meeting radius R_{m} (tracks meet in #phi), %s: ULS (red), LS (black);R_{m} (m);S/M / overall",pad==0?"10-20#circ (notch band)":"all #Delta#phi"));
				h[0]->GetXaxis()->SetRangeUser(0.,1.0); h[0]->SetMinimum(0.85); h[0]->SetMaximum(pad==0?1.25:1.10);
				one(h[0],kRed,"E1");
				TBox *gap = new TBox(0.10,0.85,0.30,pad==0?1.25:1.10); gap->SetFillColorAlpha(kGray,0.35); gap->SetLineWidth(0); gap->Draw();
				unity(0.,1.0); one(h[0],kRed,"E1 same"); one(h[1],kBlack,"E1 same");
				if (pad==0){ TLegend *lg = new TLegend(0.40,0.70,0.89,0.89); lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.04);
					lg->AddEntry(h[0],"ULS","lp"); lg->AddEntry(h[1],"LS","lp"); lg->AddEntry((TObject*)0,"gray: INTT-TPC gap, 0.1-0.3 m",""); lg->Draw(); }
			}
		}
		//---- pads 5-6: the notch band vs <pt> and vs N_ch, ULS red, LS black
		for (int k=0;k<2;k++){
			ccan[ican]->cd(5+k);
			TH1D *bu = k ? bandY(uSn,uMn,"nm_bun") : bandY(uS,uM,"nm_bup");
			TH1D *bl = k ? bandY(lSn,lMn,"nm_bln") : bandY(lS,lM,"nm_blp");
			bu->SetTitle(Form("notch band (10-20#circ / 25-50#circ) vs %s: ULS (red), LS (black);%s;ratio",k?"N_{ch}":"pair #LTp_{T}#GT",k?"N_{ch} accepted":"#LTp_{T}#GT (GeV/c)"));
			bu->SetMinimum(0.85); bu->SetMaximum(1.10); bu->Draw("E1"); unity(bu->GetXaxis()->GetXmin(),bu->GetXaxis()->GetXmax());
			one(bu,kRed,"E1 same"); one(bl,kBlack,"E1 same");
		}
		//---- pad 8: text
		ccan[ican]->cd(8);
		TPaveText *pt	= new TPaveText(0.03,0.03,0.97,0.97,"NDC"); pt->SetFillColor(0); pt->SetBorderSize(1); pt->SetTextAlign(12); pt->SetTextSize(0.032);
		pt->AddText("The #pm15#circ ULS notch: WATCH (README_SplitTracks573 sec 32-34)");
		pt->AddText(Form("  notch band / 25-50#circ, all pairs: ULS %.3f, LS %.3f",notchU,notchL));
		pt->AddText("  a deficit of ULS SIBLING pairs (pad 2); LS has none");
		pt->AddText("  not made by any Corral cleaner: ULS veto, crossing,");
		pt->AddText("    V0 daughters, LS split removal, XTF, TFdup");
		pt->AddText("  flat in pair #LT#phi#GT (no stave/ladder period) and #LTy#GT");
		pt->AddText("  also in the raw Collect trees and in ana532 (sec 28)");
		pt->AddText("  CAUSE (sec 34.2): pairs whose tracks meet in #phi at R 0.1-0.3 m");
		pt->AddText("    (INTT-TPC gap: silicon-TPC seed matching) are lost (pad 4)");
		pt->Draw();
		ccan[ican]->cd(); ccan[ican]->Update();
		ccan[ican]->Print(OutputFileName.Data());
	}

	//---- README_SplitTracks573 sec 28: the TPC sector gaps. Page 1: the single-track "spokes" (hSpoke, each pt row
	//---- divided by its mean): bent at the vertex, straight at R = 0.55 m. Page 2: the pair-level smoking gun (hGap,
	//---- CalcRm): sibling and mixed pairs vs (dphi, s), s = the pair's bending-offset difference at R; the gaps line
	//---- up on the diagonals dphi - s = 0, +-30, ... in both. R2(dphi) summed over s (= the standard projection)
	//---- vs R2 formed in each s bin and averaged with weights w_s = sum over dphi of M_s (no dphi dependence).
	{
		auto rowNorm	= [&](TH2D* h, const char* nm){
			TH2D* c	= (TH2D*)h->Clone(nm); c->SetDirectory(0);
			for (int iy=1;iy<=c->GetNbinsY();iy++){
				double sw=0; int n=0;
				for (int ix=1;ix<=c->GetNbinsX();ix++){ sw+=c->GetBinContent(ix,iy); ++n; }
				double m	= n>0 ? sw/n : 0.;
				for (int ix=1;ix<=c->GetNbinsX();ix++) c->SetBinContent(ix,iy, m>0 ? c->GetBinContent(ix,iy)/m : 0.);
			}
			c->SetStats(0); c->SetMinimum(0.); c->SetMaximum(1.6);
			return c;
		};
		++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1400,900);
		ccan[ican]->cd();
		TPad* spTop		= new TPad("spTop","",0.,0.95,1.,1.);
		TPad* spMain	= new TPad("spMain","",0.,0.,1.,0.95);
		spTop->Draw(); spMain->Draw();
		spMain->Divide(3,2,0.0001,0.0001);
		for (int q=0;q<2;q++){
			for (int f=0;f<2;f++){
				spMain->cd(1+3*q+f); gPad->SetRightMargin(0.13);
				rowNorm(hSpoke[q][f],Form("hSpoke_draw_%d_%d",q,f))->Draw("colz");
			}
			spMain->cd(3+3*q);
			int b0	= hSpoke[q][0]->GetYaxis()->FindBin(0.301), b1 = hSpoke[q][0]->GetYaxis()->FindBin(0.599);
			TH1D* pv	= hSpoke[q][0]->ProjectionX(Form("hSpokeV_%d",q),b0,b1); pv->SetDirectory(0);
			TH1D* pr	= hSpoke[q][1]->ProjectionX(Form("hSpokeR_%d",q),b0,b1); pr->SetDirectory(0);
			for (TH1D* h : {pv,pr}){ double m=h->Integral()/h->GetNbinsX(); if (m>0) h->Scale(1.0/m); h->SetStats(0); }
			pv->SetTitle(Form("%s, 0.3<p_{T}<0.6 GeV/c: vertex #phi (black), #phi at R=0.55 m (red);#phi (deg);relative yield",q?"negative":"positive"));
			pv->SetLineColor(1); pr->SetLineColor(2); pv->SetMinimum(0.); pv->SetMaximum(1.6);
			pv->Draw("hist"); pr->Draw("hist same");
		}
		spTop->cd();
		++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.36); text[itext]->SetTextAlign(22);
		text[itext]->DrawLatex(0.5,0.5,"TPC sector gaps (accepted charged tracks, each p_{T} row / its mean): at the vertex a gap is a spoke #phi_{gap}+q asin(aR/p_{T}), bent oppositely for + and -; at R=0.55 m it is a vertical line");
		ccan[ican]->cd(); ccan[ican]->Update();
		ccan[ican]->Print(OutputFileName.Data());
	}
	if (!NOCORRELATIONS){
		//---- page 2: rows pi+pi+ and pi+pi- (0.4 <= abs(dy) < 1.0): S, M, R2_s maps and the 1-D comparison; row 3:
		//---- the 1-D comparison for pi-pi-, pi-pi+ (far) and pi+pi+, pi+pi- (abs(dy) < 0.4). M is normalized so that
		//---- the s-summed R2 over abs(dphi) < 60 deg equals the mean of the standard R2(dy,dphi) map in the same window.
		struct GapOut { TH2D *S,*M,*R; TH1D *plain,*corr; };
		auto gapBuild	= [&](int ip, int w)->GapOut{
			GapOut g;
			g.S	= (TH2D*)R[ip]->GethGap(0,w)->Clone(Form("hGapS_draw_%d_%d",ip,w)); g.S->SetDirectory(0); g.S->RebinX(3);
			g.M	= (TH2D*)R[ip]->GethGap(1,w)->Clone(Form("hGapM_draw_%d_%d",ip,w)); g.M->SetDirectory(0); g.M->RebinX(3);
			double r2ref=0; int nref=0;
			TH2D* hm	= hR2[1][ip];
			for (int ix=1;ix<=hm->GetNbinsX();ix++) for (int iy=1;iy<=hm->GetNbinsY();iy++){
				double ady=fabs(hm->GetXaxis()->GetBinCenter(ix)), dp=hm->GetYaxis()->GetBinCenter(iy);
				bool in	= (w==0) ? (ady>=0.4 && ady<1.0) : (ady<0.4);
				if (in && fabs(dp)<60.){ r2ref+=hm->GetBinContent(ix,iy); ++nref; }
			}
			r2ref	= nref>0 ? r2ref/nref : 0.;
			double sS=g.S->Integral(), sM=g.M->Integral();
			double kM	= (sM>0) ? sS/((1.0+r2ref)*sM) : 1.;
			g.M->Scale(kM);
			int nx=g.S->GetNbinsX(), ny=g.S->GetNbinsY();
			g.R	= (TH2D*)g.S->Clone(Form("hGapR_draw_%d_%d",ip,w)); g.R->Reset(); g.R->SetDirectory(0);
			g.plain	= g.S->ProjectionX(Form("hGapP_%d_%d",ip,w)); g.plain->Reset(); g.plain->SetDirectory(0);
			g.corr	= (TH1D*)g.plain->Clone(Form("hGapC_%d_%d",ip,w)); g.corr->SetDirectory(0);
			std::vector<double> ws(ny+1,0.);
			for (int iy=1;iy<=ny;iy++) for (int ix=1;ix<=nx;ix++) ws[iy]+=g.M->GetBinContent(ix,iy);
			for (int ix=1;ix<=nx;ix++){
				double ss=0, mm=0, cn=0, ce2=0, cw=0;
				for (int iy=1;iy<=ny;iy++){
					double sv=g.S->GetBinContent(ix,iy), mv=g.M->GetBinContent(ix,iy);
					ss+=sv; mm+=mv;
					if (mv<=0.) continue;
					if (mv/kM>=10.) g.R->SetBinContent(ix,iy,sv/mv-1.);	// map only: cells with >= 10 raw mixed pairs
					cn	+= ws[iy]*sv/mv; ce2 += ws[iy]*ws[iy]*sv/(mv*mv); cw += ws[iy];
				}
				if (mm>0){ g.plain->SetBinContent(ix,ss/mm-1.); g.plain->SetBinError(ix,sqrt(ss)/mm); }
				if (cw>0){ g.corr->SetBinContent(ix,cn/cw-1.); g.corr->SetBinError(ix,sqrt(ce2)/cw); }
			}
			return g;
		};
		auto gapGrid	= [&](double lo, double hi){
			for (int k=-2;k<=2;k++){ double x=15.*(2*k+1); if (x<=-60.||x>=60.) continue;
				TLine* l=new TLine(x,lo,x,hi); l->SetLineColor(kGray+1); l->SetLineStyle(2); l->Draw(); }
			for (int k=-1;k<=1;k++){ TLine* l=new TLine(30.*k,lo,30.*k,hi); l->SetLineColor(kGray+1); l->SetLineStyle(3); l->Draw(); }
		};
		auto gap1D	= [&](GapOut& g, const char* title){
			g.plain->SetTitle(Form("%s;#Delta#phi (deg);R_{2}",title)); g.plain->SetStats(0);
			g.plain->SetLineColor(1); g.plain->SetMarkerColor(1); g.plain->SetMarkerStyle(20); g.plain->SetMarkerSize(0.6);
			g.corr->SetLineColor(2); g.corr->SetMarkerColor(2); g.corr->SetMarkerStyle(24); g.corr->SetMarkerSize(0.6);
			double lo=std::min(g.plain->GetMinimum(),g.corr->GetMinimum()), hi=std::max(g.plain->GetMaximum(),g.corr->GetMaximum());
			double pad	= 0.15*(hi-lo); g.plain->SetMinimum(lo-pad); g.plain->SetMaximum(hi+pad);
			g.plain->Draw("E1"); g.corr->Draw("E1 same"); gapGrid(lo-pad,hi+pad);
			TLegend* lg	= new TLegend(0.14,0.78,0.70,0.90); lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.045);
			lg->AddEntry(g.plain,"summed over s (standard)","lp"); lg->AddEntry(g.corr,"R_{2} per s bin, then averaged","lp"); lg->Draw();
		};
		++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1400,900);
		ccan[ican]->cd();
		TPad* gpTop		= new TPad("gpTop","",0.,0.95,1.,1.);
		TPad* gpMain	= new TPad("gpMain","",0.,0.,1.,0.95);
		gpTop->Draw(); gpMain->Draw();
		gpMain->Divide(4,3,0.0001,0.0001);
		const int iprow[2]	= { 2, 0 };
		const char* pn[4]	= { "#pi^{+}#pi^{-}", "#pi^{-}#pi^{+}", "#pi^{+}#pi^{+}", "#pi^{-}#pi^{-}" };
		for (int r=0;r<2;r++){
			int ip	= iprow[r];
			if (ip>=NPairTypes) continue;
			GapOut g	= gapBuild(ip,0);
			gpMain->cd(1+4*r); gPad->SetRightMargin(0.13); g.S->SetStats(0); g.S->SetTitle(Form("%s sibling, 0.4#leq|dy|<1;#Delta#phi (deg);s (deg)",pn[ip])); g.S->Draw("colz");
			gpMain->cd(2+4*r); gPad->SetRightMargin(0.13); g.M->SetStats(0); g.M->SetTitle(Form("%s mixed, 0.4#leq|dy|<1;#Delta#phi (deg);s (deg)",pn[ip])); g.M->Draw("colz");
			gpMain->cd(3+4*r); gPad->SetRightMargin(0.13); g.R->SetStats(0); g.R->SetTitle(Form("%s R_{2} per (#Delta#phi,s) bin;#Delta#phi (deg);s (deg)",pn[ip]));
			{ double m=g.plain->GetBinContent(g.plain->GetMaximumBin()); g.R->SetMinimum(-0.3); g.R->SetMaximum(std::max(0.5,2.*m)); }
			g.R->Draw("colz");
			for (int k=-3;k<=3;k++){ TLine* l=new TLine(-60.,-60.+30.*k,60.,60.+30.*k); l->SetLineColor(kGray+2); l->SetLineStyle(3); l->Draw(); }
			gpMain->cd(4+4*r); gap1D(g,Form("%s, 0.4#leq|dy|<1",pn[ip]));
		}
		const int ip3[4]	= { 3, 1, 2, 0 };
		const int w3[4]		= { 0, 0, 1, 1 };
		for (int c=0;c<4;c++){
			if (ip3[c]>=NPairTypes) continue;
			GapOut g	= gapBuild(ip3[c],w3[c]);
			gpMain->cd(9+c); gap1D(g,Form("%s, %s",pn[ip3[c]],w3[c]?"|dy|<0.4":"0.4#leq|dy|<1"));
		}
		gpTop->cd();
		++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.36); text[itext]->SetTextAlign(22);
		text[itext]->DrawLatex(0.5,0.5,"Sector gaps in pairs: s = q_{1}asin(aR/p_{T1}) - q_{2}asin(aR/p_{T2}) at R=0.55 m, so #Delta#phi - s = pair separation at the TPC; gaps align on #Delta#phi - s = 0, #pm30#circ (dotted). Grey dashed: #pm15, #pm45#circ");
		ccan[ican]->cd(); ccan[ican]->Update();
		ccan[ican]->Print(OutputFileName.Data());
	}
	//---- README_SplitTracks573 sec 29: the near-vertex two-track loss and the isep cut drawn on it. hISep (CalcRm) holds
	//---- every charged pair before the cut (after tsep): sibling / mixed vs (dphi*(R = 3 cm), abs(dy)). Left: S/M, each
	//---- abs(dy) row divided by its own value at 4 < abs(dphi*) < 10 deg, with the cut box (red; dashed at the
	//---- default 1 deg when isep is off). Right: the same ratio in abs(dy) windows, cut edges marked. LS = ++ and --
	//---- summed, ULS = +- and -+ summed.
	if (!NOCORRELATIONS && NPairTypes>=4){
		const double wcut	= (R[0]->GetISepDphi()>0.) ? R[0]->GetISepDphi() : 1.0;
		const bool on		= (R[0]->GetISepDphi()>0.);
		++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1400,900);
		ccan[ican]->cd();
		TPad* isTop		= new TPad("isTop","",0.,0.95,1.,1.);
		TPad* isMain	= new TPad("isMain","",0.,0.,1.,0.95);
		isTop->Draw(); isMain->Draw();
		isMain->Divide(2,2,0.0001,0.0001);
		const int ipa[2][2]	= { {2,3}, {0,1} };
		const char* cname[2]	= { "LS (#pi^{+}#pi^{+} + #pi^{-}#pi^{-})", "ULS (#pi^{+}#pi^{-} + #pi^{-}#pi^{+})" };
		for (int c=0;c<2;c++){
			TH2D* S	= (TH2D*)R[ipa[c][0]]->GethISep(0)->Clone(Form("hISepS_draw_%d",c)); S->SetDirectory(0); S->Add(R[ipa[c][1]]->GethISep(0));
			TH2D* M	= (TH2D*)R[ipa[c][0]]->GethISep(1)->Clone(Form("hISepM_draw_%d",c)); M->SetDirectory(0); M->Add(R[ipa[c][1]]->GethISep(1));
			TH2D* Q	= (TH2D*)S->Clone(Form("hISepQ_draw_%d",c)); Q->Reset(); Q->SetDirectory(0);
			for (int iy=1;iy<=S->GetNbinsY();iy++){
				double rs=0, rm=0;
				for (int ix=1;ix<=S->GetNbinsX();ix++){ double a=fabs(S->GetXaxis()->GetBinCenter(ix)); if (a>4.&&a<10.){ rs+=S->GetBinContent(ix,iy); rm+=M->GetBinContent(ix,iy); } }
				if (rs<=0.||rm<=0.) continue;
				for (int ix=1;ix<=S->GetNbinsX();ix++){ double m=M->GetBinContent(ix,iy); if (m>0.) Q->SetBinContent(ix,iy,(S->GetBinContent(ix,iy)/m)/(rs/rm)); }
			}
			isMain->cd(1+2*c); gPad->SetRightMargin(0.13);
			Q->SetStats(0); Q->SetMinimum(0.7); Q->SetMaximum(1.1);
			Q->SetTitle(Form("%s: sibling/mixed, each |dy| row / its 4<|#Delta#phi*|<10#circ value;#Delta#phi*(R=3 cm) (deg);|dy|",cname[c]));
			Q->Draw("colz");
			TBox* bx	= new TBox(-wcut,0.,wcut,CalcRm::ISEP_DYMAX); bx->SetFillStyle(0); bx->SetLineColor(2); bx->SetLineWidth(2); bx->SetLineStyle(on?1:2); bx->Draw();
			isMain->cd(2+2*c);
			const double dw[5]	= { 0., 0.2, 0.4, 0.8, 1.5 };
			const int dcol[4]	= { 1, 4, 8, kGray+1 };
			TLegend* lg	= new TLegend(0.14,0.14,0.50,0.36); lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.04);
			for (int k=0;k<4;k++){
				int b0	= S->GetYaxis()->FindBin(dw[k]+1e-4), b1 = S->GetYaxis()->FindBin(dw[k+1]-1e-4);
				TH1D* s	= S->ProjectionX(Form("hISepSp_%d_%d",c,k),b0,b1); s->SetDirectory(0);
				TH1D* m	= M->ProjectionX(Form("hISepMp_%d_%d",c,k),b0,b1); m->SetDirectory(0);
				double rs=0, rm=0;
				for (int ix=1;ix<=s->GetNbinsX();ix++){ double a=fabs(s->GetBinCenter(ix)); if (a>4.&&a<10.){ rs+=s->GetBinContent(ix); rm+=m->GetBinContent(ix); } }
				TH1D* r	= (TH1D*)s->Clone(Form("hISepR_%d_%d",c,k)); r->SetDirectory(0); r->Reset();
				for (int ix=1;ix<=s->GetNbinsX();ix++){ double sv=s->GetBinContent(ix), mv=m->GetBinContent(ix);
					if (sv>0.&&mv>0.&&rs>0.&&rm>0.){ double v=(sv/mv)/(rs/rm); r->SetBinContent(ix,v); r->SetBinError(ix,v*sqrt(1./sv+1./rs)); } }
				r->SetStats(0); r->SetLineColor(dcol[k]); r->SetMarkerColor(dcol[k]); r->SetMarkerStyle(20); r->SetMarkerSize(0.5);
				r->SetMinimum(0.75); r->SetMaximum(1.15);
				r->SetTitle(Form("%s: sibling/mixed in |dy| windows (1 = no loss);#Delta#phi*(R=3 cm) (deg);S/M, relative",cname[c]));
				r->Draw(k==0 ? "E1" : "E1 same");
				lg->AddEntry(r,Form("%.1f#leq|dy|<%.1f",dw[k],dw[k+1]),"lp");
			}
			lg->Draw();
			for (int sg=-1;sg<=1;sg+=2){ TLine* l=new TLine(sg*wcut,0.75,sg*wcut,1.15); l->SetLineColor(2); l->SetLineWidth(2); l->SetLineStyle(on?1:2); l->Draw(); }
			TLine* l1=new TLine(-10.,1.,10.,1.); l1->SetLineColor(kGray+2); l1->SetLineStyle(3); l1->Draw();
		}
		isTop->cd();
		++itext; text[itext]->SetTextFont(42); text[itext]->SetTextSize(0.36); text[itext]->SetTextAlign(22);
		text[itext]->DrawLatex(0.5,0.5,Form("Near-vertex two-track loss (sibling tracks at the same #phi near the vertex, out to |dy|~0.8; all pairs, before the cut).  Red: isep cut |#Delta#phi*(3 cm)|<%.1f#circ, |dy|<%.1f: %s",wcut,CalcRm::ISEP_DYMAX,on?"ON":"OFF (dashed = proposed)"));
		ccan[ican]->cd(); ccan[ican]->Update();
		ccan[ican]->Print(OutputFileName.Data());
	}

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

	//---- README_PID.md: dE/dx PID QA. Top: dedxKFP vs p with the KFP gates (pi blue, K green, p red, d gray; dashed
	//---- = lo/hi), dedxKFP/dedx70s, tagged-proton PID. Bottom: dedxKFP of what became pi, K, p; y vs pt of p.
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
	ccan[ican]->cd(); ccan[ican]->Divide(4,2,0.0001,0.0001);
	{
		const int gcol[NGATE]	= {kBlue,kGreen+2,kRed,kGray+2};
		auto drawGates	= [&](){
			if (doOldPID) return;
			for (int ig=0;ig<NGATE;ig++) for (TGraph *g : {gGateLo[ig],gGateHi[ig]}){
				g->SetLineColor(gcol[ig]); g->SetLineStyle(2); g->SetLineWidth(1); g->Draw("L same");
			}
		};
		ccan[ican]->cd(1); gPad->SetLogz(1); hdedxKFPp->Draw("colz"); drawGates();
		ccan[ican]->cd(2); gPad->SetLogz(1); hdedxratio2_p->Draw("colz"); hdedxratio_p->SetLineColor(kRed); hdedxratio_p->Draw("same");
		ccan[ican]->cd(3); gPad->SetLogz(1); hdedxKFPp_tagged[0][1]->Draw("colz"); drawGates();	// Lambda -> p (true protons)
		ccan[ican]->cd(4);
		{	//---- fraction of V0-daughter protons (red) and K0s-daughter pions (blue) given the p PID, vs p
			TH1D *hn	= hPIDtagged[0]->ProjectionX("hPIDtagged_p_all",1,4);
			TH1D *hf	= hPIDtagged[0]->ProjectionX("hPIDtagged_p_asp",3,3);
			hf->Divide(hf,hn,1,1,"B"); hf->SetLineColor(kRed); hf->SetMarkerColor(kRed); hf->SetMarkerStyle(20); hf->SetMarkerSize(0.6);
			hf->SetTitle("fraction given PID p: V0 protons (red), K^{0}_{S} pions (blue);p (GeV/c);fraction"); hf->SetMinimum(0.); hf->SetMaximum(1.05); hf->Draw("e");
			TH1D *hnp	= hPIDtagged[1]->ProjectionX("hPIDtagged_pi_all",1,4);
			TH1D *hfp	= hPIDtagged[1]->ProjectionX("hPIDtagged_pi_asp",3,3);
			hfp->Divide(hfp,hnp,1,1,"B"); hfp->SetLineColor(kBlue); hfp->SetMarkerColor(kBlue); hfp->SetMarkerStyle(24); hfp->SetMarkerSize(0.6); hfp->Draw("e same");
		}
		for (int k=0;k<3;k++){ ccan[ican]->cd(5+k); gPad->SetLogz(1); hdedxKFPp_id[k]->Draw("colz"); drawGates(); }
		ccan[ican]->cd(8); gPad->SetLogz(1); hypt_id[2]->Draw("colz");
		for (double yy : {-Species_yu[2], Species_yu[2]}){ TLine *l = new TLine(yy,0.,yy,2.); l->SetLineColor(kRed); l->SetLineWidth(2); l->Draw(); }
		{ TLatex *t = new TLatex(0.,1.85,Form("|y| < %.2f (Species_yu)",Species_yu[2])); t->SetTextAlign(21); t->SetTextSize(0.05); t->SetTextColor(kRed); t->Draw(); }
	}
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());

	//---- README_PID.md sec 10.3: the vertex-phi mask, and every track cut in force (so none is forgotten). Before the mask =
	//---- AcceptTrackBase (black), after = the accepted tracks (red); gray boxes = the mask windows.
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
	ccan[ican]->cd(); ccan[ican]->Divide(3,2,0.0001,0.0001);
	{
		TH2D *bxy	= (TH2D*)hmask_dcaxy[0]->Clone("pm_bxy");	bxy->Add(hmask_dcaxy[1]);
		TH2D *bz	= (TH2D*)hmask_dcaz[0] ->Clone("pm_bz");	bz ->Add(hmask_dcaz[1]);
		TH2D *axy	= (TH2D*)hadca_phi[0][0][0]->Clone("pm_axy");	axy->Add(hadca_phi[0][1][0]);
		TH2D *az	= (TH2D*)hadca_phi[1][0][0]->Clone("pm_az");	az ->Add(hadca_phi[1][1][0]);
		auto meanX	= [&](TH2D *h, const char* nm){
			TH1D *m	= h->ProjectionX(nm); m->Reset();
			for (int ix=1;ix<=h->GetNbinsX();ix++){ double n=0,sy=0,sy2=0;
				for (int iy=1;iy<=h->GetNbinsY()+1;iy++){ double w=h->GetBinContent(ix,iy), y=h->GetYaxis()->GetBinCenter(iy); n+=w; sy+=w*y; sy2+=w*y*y; }
				if (n>1){ double mu=sy/n; m->SetBinContent(ix,mu); m->SetBinError(ix,sqrt(std::max(0.,sy2/n-mu*mu)/n)); } }
			return m;
		};
		auto boxes	= [&](double y0, double y1){
			if (!doSiPhiMask) return;
			for (int k=0;k<nSiPhiMask;k++){ TBox *b = new TBox(SIPHIMASK_LO[k],y0,SIPHIMASK_HI[k],y1); b->SetFillColorAlpha(kGray,0.35); b->SetLineWidth(0); b->Draw(); }
		};
		auto pair2	= [&](TH1D *hb, TH1D *ha, const char* ttl){
			hb->SetTitle(ttl); hb->SetLineColor(kBlack); ha->SetLineColor(kRed); hb->SetMinimum(0.); hb->SetMaximum(1.15*std::max(hb->GetMaximum(),ha->GetMaximum()));
			hb->Draw("hist"); boxes(0.,hb->GetMaximum()); hb->Draw("hist same"); ha->Draw("hist same");
		};
		ccan[ican]->cd(1);
		pair2(bxy->ProjectionX("pm_nb"),axy->ProjectionX("pm_na"),Form("tracks vs vertex #phi: before SiPhiMask (black), accepted (red, also after split removal)%s;vertex #phi (deg);tracks",doSiPhiMask?"":" (mask OFF)"));
		ccan[ican]->cd(2);
		pair2(meanX(bxy,"pm_mxyb"),meanX(axy,"pm_mxya"),"<|dcaxy|> vs vertex #phi, before (black) / after (red);vertex #phi (deg);<|dcaxy|> (cm)");
		ccan[ican]->cd(3);
		pair2(meanX(bz,"pm_mzb"),meanX(az,"pm_mza"),"<|dcaz|> vs vertex #phi, before (black) / after (red);vertex #phi (deg);<|dcaz|> (cm)");
		ccan[ican]->cd(4); gPad->SetLogz(1);
		bxy->SetTitle("|dcaxy| vs vertex #phi, before the mask;vertex #phi (deg);|dcaxy| (cm)"); bxy->GetYaxis()->SetRangeUser(0.,1.0); bxy->Draw("colz");
		if (doSiPhiMask) for (int k=0;k<nSiPhiMask;k++) for (double x : {SIPHIMASK_LO[k],SIPHIMASK_HI[k]}){ TLine *l = new TLine(x,0.,x,1.0); l->SetLineColor(kRed); l->SetLineWidth(2); l->SetLineStyle(2); l->Draw(); }
		ccan[ican]->cd(5);
		TH1D *hall	= hmask_pid->ProjectionX("pm_pall",1,2);
		TH1D *hin	= hmask_pid->ProjectionX("pm_pin",2,2);
		hin->Divide(hin,hall,1,1,"B");
		const char* pl[8]	= {"#pi+","#pi-","K+","K-","p","#bar{p}","none +","none -"};
		for (int k=0;k<8;k++) hin->GetXaxis()->SetBinLabel(k+1,pl[k]);
		hin->SetTitle("fraction of tracks inside the mask windows, by PID;;fraction"); hin->SetMinimum(0.); hin->SetMaximum(1.3*std::max(0.01,hin->GetMaximum()));
		hin->SetMarkerStyle(20); hin->Draw("E1");
		ccan[ican]->cd(6);
		TPaveText *pt	= new TPaveText(0.03,0.03,0.97,0.97,"NDC"); pt->SetFillColor(0); pt->SetBorderSize(1); pt->SetTextAlign(12); pt->SetTextSize(0.034);
		pt->AddText("Track cuts in force (AcceptTrack, PID, pair building)");
		pt->AddText(Form("  %.2f < p_{T} < 20.1 GeV/c,  ntpc #geq %d", PTMINCUT, NTPCCUT));
		pt->AddText("  |dcaxy| < 1.5 cm,  |dcaz| < 1.5 cm");
		if (doSiPhiMask){ TString w; for (int k=0;k<nSiPhiMask;k++) w += Form(" [%.0f,%.0f)",SIPHIMASK_LO[k],SIPHIMASK_HI[k]); pt->AddText(Form("  SiPhiMask ON, vertex #phi (deg):%s",w.Data())); }
		else pt->AddText("  SiPhiMask OFF (nosiphimask)");
		if (doCMMask) pt->AddText(Form("  CM mask ON: w = (#eta+%.3f z_{c}) sign(-z_{c}) in [%.2f,%.2f), z_{c} = zvtx-slice centre",CMMASK_K,valCMMaskLo,valCMMaskHi));
		else pt->AddText("  CM mask OFF (nocmmask)");
		if (doOldPID) pt->AddText("  PID: legacy (oldpid), #pi = dedx70s < 400");
		else pt->AddText(Form("  PID: KFP gates (dedxKFP), #pi>p>K; p_{max} %.1f/%.1f/%.1f", Species_pmax_pid[0], Species_pmax_pid[2], Species_pmax_pid[1]));
		pt->AddText(Form("  pairs: p_{T} min #pi %.2f, K %.2f, p/#bar{p} %.2f GeV/c", Species_ptmin[0], Species_ptmin[1], Species_ptmin[2]));
		pt->AddText("  V0 daughters in no pair type"); pt->AddText(Form("  split-track removal %s", doSplitRemoval ? "ON" : "OFF"));
		pt->AddText(Form("  looper veto %s (OS, |p|<%.2f, p-sum<%.2f): %ld tracks", doLooperVeto ? "ON" : "OFF", LOOPER_PMAX, valLooperRSum, nFlagged_Looper));
		double nin	= hmask_pid->Integral(1,8,2,2), nall = hmask_pid->Integral(1,8,1,2);
		pt->AddText(Form("  tracks inside the windows: %.2f%%%s", nall>0 ? 100.*nin/nall : 0., doSiPhiMask ? " (removed)" : " (kept)"));
		pt->Draw();
	}
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());

	//---- README_PID.md sec 10: DCA of the identified species (accepted tracks), as ana573/DCAid_ana573.C. Pages 1-2 (dcaxy,
	//---- dcaz), per pt slice: shapes of p, pbar, pi+, pi- (unit area, log y); p/pbar vs dca; fraction kept by abs(dca) < X.
	for (int iv=0;iv<2;iv++){
		++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,900);
		ccan[ican]->cd(); ccan[ican]->Divide(4,3,0.0001,0.0001);
		const int NS=4; const double pl[NS+1]={0.1,0.25,0.4,0.7,1.0};
		const char* var	= iv ? "dcaz" : "dcaxy";
		const int kk[4]={2,2,0,0}, cc[4]={0,1,0,1}, col[4]={kRed,kBlue,kGray+2,kGreen+2};
		const char* lab[4]={"p","#bar{p}","#pi^{+}","#pi^{-}"};
		for (int is=0;is<NS;is++){
			TH1D *h[4];
			for (int j=0;j<4;j++){
				TH2D *m	= iv ? hdcaz_id[kk[j]][cc[j]] : hdcaxy_id[kk[j]][cc[j]];
				int b0=m->GetXaxis()->FindBin(pl[is]+1e-6), b1=m->GetXaxis()->FindBin(pl[is+1]-1e-6);
				h[j]	= m->ProjectionY(Form("dcaid_%d_%d_%d",iv,is,j),b0,b1); h[j]->Rebin(2);
				h[j]->SetLineColor(col[j]); h[j]->SetMarkerColor(col[j]);
			}
			ccan[ican]->cd(1+is); gPad->SetLogy(1);
			for (int j=0;j<4;j++){
				TH1D *sh	= (TH1D*)h[j]->Clone(Form("dcaids_%d_%d_%d",iv,is,j)); if (sh->Integral()>0) sh->Scale(1./sh->Integral());
				sh->SetTitle(Form("%s, %.2f<p_{T}<%.2f GeV/c;%s (cm);unit area",var,pl[is],pl[is+1],var)); sh->SetMinimum(1e-6);
				sh->Draw(j?"hist same":"hist");
			}
			if (is==0){ TLegend *lg = new TLegend(0.62,0.62,0.89,0.89); lg->SetBorderSize(0); lg->SetFillStyle(0);
				for (int j=0;j<4;j++) lg->AddEntry(h[j],lab[j],"l"); lg->Draw(); }
			ccan[ican]->cd(1+NS+is);
			TH1D *r		= (TH1D*)h[0]->Clone(Form("dcaidr_%d_%d",iv,is)); TH1D *rb = (TH1D*)h[1]->Clone(Form("dcaidrb_%d_%d",iv,is));
			r->Rebin(3); rb->Rebin(3); r->Divide(rb); r->SetLineColor(1); r->SetMarkerColor(1); r->SetMarkerStyle(20); r->SetMarkerSize(0.5);
			r->SetTitle(Form("p / #bar{p}, %.2f<p_{T}<%.2f;%s (cm);p/#bar{p}",pl[is],pl[is+1],var)); r->SetMinimum(0); r->SetMaximum(20.);
			r->Draw("E1");
			ccan[ican]->cd(1+2*NS+is);
			for (int j=0;j<4;j++){
				TH1D *k	= new TH1D(Form("dcaidk_%d_%d_%d",iv,is,j),Form("kept by |%s|<X, %.2f<p_{T}<%.2f;X (cm);fraction kept",var,pl[is],pl[is+1]),75,0,1.5);
				double tot	= h[j]->Integral();
				for (int ib=1;ib<=75 && tot>0;ib++){ double X=k->GetBinCenter(ib); k->SetBinContent(ib,h[j]->Integral(h[j]->FindBin(-X),h[j]->FindBin(X-1e-6))/tot); }
				k->SetLineColor(col[j]); k->SetMinimum(0.5); k->SetMaximum(1.1);
				if (j==0){ k->Draw("E1"); TLine *l1 = new TLine(0.,1.,1.5,1.); l1->SetLineColor(kGray); l1->Draw(); }
				k->Draw("hist same");
			}
		}
		ccan[ican]->cd(); ccan[ican]->Update();
		ccan[ican]->Print(OutputFileName.Data());
	}
	//---- page 3: (eta,phi) maps. Rows pi+, pi- (mean dcaxy, abs(dcaxy), dcaz, abs(dcaz)); row 3 abs(dcaxy), abs(dcaz) of p, pbar
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,900);
	ccan[ican]->cd(); ccan[ican]->Divide(4,3,0.0001,0.0001);
	{
		const int rk[12]={0,0,0,0, 0,0,0,0, 2,2,2,2}, rc[12]={0,0,0,0, 1,1,1,1, 0,1,0,1}, rv[12]={0,1,2,3, 0,1,2,3, 1,1,3,3};
		for (int i=0;i<12;i++){
			ccan[ican]->cd(i+1); gPad->SetRightMargin(0.16);
			TProfile2D *m	= pdca_etaphi_id[rk[i]][rc[i]][rv[i]];
			TH2D *h	= m->ProjectionXY(Form("dcaidmap_%d",i)); h->SetTitle(m->GetTitle());
			for (int ix=1;ix<=h->GetNbinsX();ix++) for (int iy=1;iy<=h->GetNbinsY();iy++) if (m->GetBinEntries(m->GetBin(ix,iy))<(rk[i]==2?20:200)) h->SetBinContent(ix,iy,-99);
			h->GetXaxis()->SetRangeUser(-1.2,1.2);
			if (rv[i]==0||rv[i]==2){ h->SetMinimum(-0.1); h->SetMaximum(0.1); } else { h->SetMinimum(0.); h->SetMaximum(rv[i]==1?0.3:0.4); }
			h->Draw("colz");
		}
	}
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());
	//---- page 3b (README_PID.md sec 10.6): 1D profiles of the pion (eta,phi) maps, + (red) and - (blue). Row 1 vs eta: signed
	//---- dcaxy, signed dcaz, abs(dcaz). Row 2 vs vertex phi: signed dcaxy, signed dcaz; and abs(dcaz) vs abs(dcaxy) per 1-deg
	//---- phi bin (hadca_phi, accepted tracks; do they go bad at the same places?)
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
	ccan[ican]->cd(); ccan[ican]->Divide(3,2,0.0001,0.0001);
	{
		const int vv[5]	= {0,2,3,0,2};
		const char* vn[4]	= {"dcaxy","|dcaxy|","dcaz","|dcaz|"};
		for (int ip=0;ip<5;ip++){
			ccan[ican]->cd(1+ip);
			TProfile *pr[2];
			for (int q=0;q<2;q++){
				TProfile2D *m	= pdca_etaphi_id[0][q][vv[ip]];
				pr[q]	= (ip<3) ? m->ProfileX(Form("dcaprof_%d_%d",ip,q)) : m->ProfileY(Form("dcaprof_%d_%d",ip,q));
				pr[q]->SetLineColor(q?kBlue:kRed); pr[q]->SetMarkerColor(q?kBlue:kRed); pr[q]->SetMarkerStyle(20); pr[q]->SetMarkerSize(0.5);
			}
			double lo=1e9,hi=-1e9;
			for (int q=0;q<2;q++) for (int i=1;i<=pr[q]->GetNbinsX();i++){ if (pr[q]->GetBinEntries(i)<100) continue; lo=std::min(lo,pr[q]->GetBinContent(i)); hi=std::max(hi,pr[q]->GetBinContent(i)); }
			double pd	= 0.2*(hi-lo); if (vv[ip]==3){ lo = 0.; } else { lo -= pd; }
			pr[0]->SetMinimum(lo); pr[0]->SetMaximum(hi+pd);
			pr[0]->SetTitle(Form("#pi: <%s> vs %s, + (red), - (blue);%s;<%s> (cm)",vn[vv[ip]],ip<3?"#eta":"vertex #phi",ip<3?"#eta":"vertex #phi (rad)",vn[vv[ip]]));
			if (ip<3) pr[0]->GetXaxis()->SetRangeUser(-1.2,1.2);
			pr[0]->Draw("E1");
			if (vv[ip]!=3){ TLine *l = new TLine(pr[0]->GetXaxis()->GetBinLowEdge(pr[0]->GetXaxis()->GetFirst()),0.,pr[0]->GetXaxis()->GetBinUpEdge(pr[0]->GetXaxis()->GetLast()),0.); l->SetLineColor(kGray); l->Draw(); }
			pr[0]->Draw("E1 same"); pr[1]->Draw("E1 same");
		}
		ccan[ican]->cd(6);
		TH2D *x	= (TH2D*)hadca_phi[0][0][0]->Clone("dcacorr_x"); x->Add(hadca_phi[0][1][0]);
		TH2D *z	= (TH2D*)hadca_phi[1][0][0]->Clone("dcacorr_z"); z->Add(hadca_phi[1][1][0]);
		TProfile *px	= x->ProfileX("dcacorr_px"), *pz = z->ProfileX("dcacorr_pz");
		TGraph *g	= new TGraph();
		for (int i=1;i<=px->GetNbinsX();i++) if (px->GetBinEntries(i)>100) g->SetPoint(g->GetN(),px->GetBinContent(i),pz->GetBinContent(i));
		g->SetTitle(Form("per 1#circ vertex-#phi bin, accepted tracks: r = %.2f;<|dcaxy|> (cm);<|dcaz|> (cm)",g->GetCorrelationFactor()));
		g->SetMarkerStyle(20); g->SetMarkerSize(0.4); g->Draw("AP");
	}
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());
	//---- page 4 (README_PID.md sec 10.2, as ana573/DCAstrip_ana573.C): accepted tracks, 1 deg in phi, + (red) and - (blue).
	//---- Rows: vertex phi, phi at R = 0.55 m. Columns: mean abs(dcaxy), fraction abs(dcaxy) > 0.3 cm, mean abs(dcaz).
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,900);
	ccan[ican]->cd(); ccan[ican]->Divide(3,2,0.0001,0.0001);
	{
		auto meanOf	= [&](TH2D *h, const char* nm){ TH1D *m = h->ProjectionX(nm); m->Reset();
			for (int ix=1;ix<=h->GetNbinsX();ix++){ double n=0,sy=0,sy2=0;
				for (int iy=1;iy<=h->GetNbinsY()+1;iy++){ double w=h->GetBinContent(ix,iy), y=h->GetYaxis()->GetBinCenter(iy); n+=w; sy+=w*y; sy2+=w*y*y; }
				if (n>1){ double mu=sy/n; m->SetBinContent(ix,mu); m->SetBinError(ix,sqrt(std::max(0.,sy2/n-mu*mu)/n)); } }
			return m; };
		auto fracAbove	= [&](TH2D *h, double x, const char* nm){ TH1D *m = h->ProjectionX(nm); m->Reset();
			int b	= h->GetYaxis()->FindBin(x+1e-6);
			for (int ix=1;ix<=h->GetNbinsX();ix++){ double n=h->Integral(ix,ix,1,h->GetNbinsY()+1), a=h->Integral(ix,ix,b,h->GetNbinsY()+1);
				if (n>0){ double f=a/n; m->SetBinContent(ix,f); m->SetBinError(ix,sqrt(f*(1-f)/n)); } }
			return m; };
		const char* fl[2]	= {"vertex #phi","#phi at R = 0.55 m"};
		const char* yt[3]	= {"<|dcaxy|> (cm)","fraction |dcaxy| > 0.3 cm","<|dcaz|> (cm)"};
		for (int f=0;f<2;f++){
			TH1D *h[3][2];
			for (int q=0;q<2;q++){
				h[0][q]	= meanOf(hadca_phi[0][q][f],Form("strip_mxy_%d_%d",f,q));
				h[1][q]	= fracAbove(hadca_phi[0][q][f],0.3,Form("strip_fxy_%d_%d",f,q));
				h[2][q]	= meanOf(hadca_phi[1][q][f],Form("strip_mz_%d_%d",f,q));
			}
			for (int r=0;r<3;r++){
				ccan[ican]->cd(1+r+3*f);
				for (int q=0;q<2;q++){
					h[r][q]->SetLineColor(q?kBlue:kRed);
					h[r][q]->SetTitle(Form("%s vs %s, accepted: + (red), - (blue);%s (deg);%s",yt[r],fl[f],fl[f],yt[r]));
					h[r][q]->SetMinimum(0); h[r][q]->SetMaximum(1.2*std::max(h[r][0]->GetMaximum(),h[r][1]->GetMaximum()));
					h[r][q]->Draw(q?"hist same":"hist");
				}
			}
		}
	}
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());

	//---- README_PID.md sec 8: p-pi ULS ridge diagnostic. Pad 1: dphi, p pi- (black) and pbar pi+ (magenta).
	//---- Others: p pi- + pbar pi+ summed, ridge box (red) vs its clean mirror (blue), mirror scaled to the ridge's pairs.
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
	ccan[ican]->cd(); ccan[ican]->Divide(4,2,0.0001,0.0001);
	{
		ccan[ican]->cd(1); hRidge_dphi[0]->SetLineColor(kBlack); hRidge_dphi[0]->Draw("hist");
		hRidge_dphi[1]->SetLineColor(kMagenta+1); hRidge_dphi[1]->Draw("hist same");
		TH1D **hs[7]	= {hRidge_Mppi[0],hRidge_Mee[0],hRidge_open[0],hRidge_prat[0],hRidge_skf[0],hRidge_dcap[0],hRidge_dcapi[0]};
		TH1D **hm[7]	= {hRidge_Mppi[1],hRidge_Mee[1],hRidge_open[1],hRidge_prat[1],hRidge_skf[1],hRidge_dcap[1],hRidge_dcapi[1]};
		for (int k=0;k<7;k++){
			ccan[ican]->cd(2+k); if (k>=4) gPad->SetLogy(1);
			TH1D *r	= (TH1D*)hs[k][0]->Clone(Form("%s_sum",hs[k][0]->GetName())); r->Add(hs[k][1]);
			TH1D *m	= (TH1D*)hm[k][0]->Clone(Form("%s_sum",hm[k][0]->GetName())); m->Add(hm[k][1]);
			r->SetTitle(TString(r->GetTitle()).ReplaceAll("p#pi^{-} ridge","ridge (red) vs mirror (blue)"));
			if (m->Integral()>0.) m->Scale(r->Integral()/m->Integral());
			r->SetLineColor(kRed); m->SetLineColor(kBlue);
			r->SetMaximum(1.15*std::max(r->GetMaximum(),m->GetMaximum()));
			r->Draw("hist"); m->Draw("hist same");
		}
	}
	ccan[ican]->cd(); ccan[ican]->Update();
	ccan[ican]->Print(OutputFileName.Data());

	//---- README_CQcomparison.md sec 2.1-2.2: V0 daughter sharing / duplicates, and <pT> vs N_ch.
	//---- Row 1: in-peak V0 pairs sharing a daughter (counts, of all in-peak pairs in the title); in-peak V0 sharing with
	//---- off-peak candidates; nearest list track to the Lambda p / Lbar pbar daughter (dashed: the CQC_DRDUP radius).
	//---- Row 2: p-Lambda, pbar-Lbar sibling Q by proton category (dashed: daughter-parent Q = 0.10 GeV); Lambda-Lambda,
	//---- Lbar-Lbar sibling Q by daughter category. Row 3: SKF of near daughters; n_Lambda per event vs Poisson; <pT> vs N_ch.
	++ican; ccan[ican]	= new TCanvas(Form("ccan%d",ican),Form("ccan%d",ican),ican*30,30+ican*30,1200,800);
	ccan[ican]->cd(); ccan[ican]->Divide(4,3,0.0001,0.0001);
	{
		const char* v0lab[3]	= {"K0s","Lambda","Lbar"};
		for (int k=1;k<=3;k++){
			hV0share->GetXaxis()->SetBinLabel(k,v0lab[k-1]); hV0share->GetYaxis()->SetBinLabel(k,v0lab[k-1]);
			hV0shareOff->GetXaxis()->SetBinLabel(k,v0lab[k-1]); hV0shareOff->GetYaxis()->SetBinLabel(k,v0lab[k-1]);
		}
		gStyle->SetPaintTextFormat(".0f");
		ccan[ican]->cd(1);
		hV0share->SetTitle(Form("in-peak V0 pairs sharing a daughter: %.0f of %.0f in-peak pairs",hV0share->Integral(),hV0pairs->Integral()));
		hV0share->SetMarkerSize(2.0); hV0share->Draw("text");
		ccan[ican]->cd(2);
		hV0shareOff->SetTitle("in-peak V0 sharing a daughter with an off-peak candidate");
		hV0shareOff->SetMarkerSize(2.0); hV0shareOff->Draw("text");
		auto vline	= [&](double x){ gPad->Update(); TLine *l = new TLine(x,gPad->GetUymin(),x,gPad->GetUymax());
			if (gPad->GetLogy()) l = new TLine(x,pow(10,gPad->GetUymin()),x,pow(10,gPad->GetUymax()));
			l->SetLineStyle(2); l->SetLineColor(kGray+2); l->Draw(); };
		const int kvq[2][2]	= {{1,0},{2,1}};	// Lambda p, Lbar pbar
		for (int i=0;i<2;i++){
			ccan[ican]->cd(3+i); gPad->SetLogy(1);
			TH1D *h	= hDauNear_dR[kvq[i][0]][kvq[i][1]];
			h->SetLineColor(kBlack); h->Draw("hist"); vline(CQC_DRDUP);
		}
		const int col[3]	= {kRed, kBlue, kBlack};
		for (int a=0;a<2;a++){
			ccan[ican]->cd(5+a);
			double mx	= 0;
			for (int c=0;c<3;c++) mx = std::max(mx,hpLa_Q[a][c]->GetMaximum());
			TLegend *lg	= new TLegend(0.45,0.62,0.89,0.89); lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.035);
			for (int c=0;c<3;c++){
				hpLa_Q[a][c]->SetLineColor(col[c]); hpLa_Q[a][c]->SetMaximum(1.15*mx); hpLa_Q[a][c]->SetMinimum(0);
				if (c==0) hpLa_Q[a][c]->SetTitle(Form("%s sibling Q, by the (anti)proton",a?"#bar{p}#bar{#Lambda}":"p#Lambda"));
				hpLa_Q[a][c]->Draw(c?"hist same":"hist");
				lg->AddEntry(hpLa_Q[a][c],Form("%s (%.0f)",pLcat[c],hpLa_Q[a][c]->Integral()),"l");
			}
			vline(0.10); lg->AddEntry((TObject*)0,"dashed: daughter-parent Q","");
			lg->Draw();
		}
		for (int a=0;a<2;a++){
			ccan[ican]->cd(7+a); gPad->SetLogy(1);
			double mx	= 0;
			for (int c=0;c<3;c++) mx = std::max(mx,hLaLa_Q[a][c]->GetMaximum());
			TLegend *lg	= new TLegend(0.45,0.70,0.89,0.89); lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.035);
			for (int c=0;c<3;c++){
				hLaLa_Q[a][c]->SetLineColor(col[c]); hLaLa_Q[a][c]->SetMaximum(3.*std::max(mx,1.)); hLaLa_Q[a][c]->SetMinimum(0.5);
				if (c==0) hLaLa_Q[a][c]->SetTitle(Form("%s sibling Q, by the daughters",a?"#bar{#Lambda}#bar{#Lambda}":"#Lambda#Lambda"));
				hLaLa_Q[a][c]->Draw(c?"hist same":"hist");
				lg->AddEntry(hLaLa_Q[a][c],Form("%s (%.0f)",LLcat[c],hLaLa_Q[a][c]->Integral()),"l");
			}
			lg->Draw();
		}
		ccan[ican]->cd(9);
		{
			TH1D *h0	= hDauNear_skf[1][0], *h1 = hDauNear_skf[2][1];
			h0->SetLineColor(kRed); h1->SetLineColor(kBlue);
			h0->SetTitle(Form("SiSplitScore, (anti)proton daughter vs its nearest (#DeltaR<%.2f)",CQC_DRDUP));
			h0->SetMaximum(1.15*std::max(1.,std::max(h0->GetMaximum(),h1->GetMaximum()))); h0->SetMinimum(0);
			h0->Draw("hist"); h1->Draw("hist same");
			TLegend *lg	= new TLegend(0.45,0.75,0.89,0.89); lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.04);
			lg->AddEntry(h0,Form("#Lambda: p (%.0f)",h0->Integral()),"l"); lg->AddEntry(h1,Form("#bar{#Lambda}: #bar{p} (%.0f)",h1->Integral()),"l"); lg->Draw();
		}
		ccan[ican]->cd(10); gPad->SetLogy(1);
		{
			TLegend *lg	= new TLegend(0.40,0.70,0.89,0.89); lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.04);
			const int cl[2]	= {kRed, kBlue};
			for (int a=0;a<2;a++){
				TH1D *h	= hnLApeak[a];
				double N=h->Integral(), mu=(N>0.)?h->GetMean():0.;
				TH1D *pz	= (TH1D*)h->Clone(Form("%s_poisson",h->GetName())); pz->Reset();
				for (int b=1;b<=pz->GetNbinsX();b++) pz->SetBinContent(b,N*TMath::Poisson(b-1,mu));
				h->SetLineColor(cl[a]); pz->SetLineColor(cl[a]); pz->SetLineStyle(2);
				if (a==0){ h->SetTitle("in-peak #Lambda, #bar{#Lambda} per event (dashed: Poisson, same mean)"); h->SetMinimum(0.5); h->SetMaximum(3.*N); }
				h->Draw(a?"hist same":"hist"); pz->Draw("hist same");
				double s2	= 0;
				for (int b=1;b<=h->GetNbinsX();b++){ double n = b-1; s2 += h->GetBinContent(b)*n*(n-1); }
				double f2	= (mu>0.) ? s2/N/(mu*mu) : 0.;
				lg->AddEntry(h,Form("%s: <n(n-1)>/<n>^{2} = %.2f",a?"#bar{#Lambda}":"#Lambda",f2),"l");
			}
			lg->AddEntry((TObject*)0,"dashed: Poisson (= 1.00)","");
			lg->Draw();
		}
		for (int j=0;j<2;j++){
			ccan[ican]->cd(11+j);
			TLegend *lg	= new TLegend(0.15,0.70,0.60,0.89); lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.04);
			const int cs[3]	= {kRed, kGreen+2, kBlue};
			bool first	= true;
			for (int k=0;k<3;k++){
				TProfile *h	= (j==0) ? hptNch[k][0] : hptNchV0[k];
				if (j==0){ TProfile *hn = hptNch[k][1]; h = (TProfile*)h->Clone(Form("%s_both",h->GetName())); h->Add(hn); }
				h->SetLineColor(cs[k]); h->SetMarkerColor(cs[k]); h->SetMarkerStyle(20); h->SetMarkerSize(0.6);
				h->SetMinimum(0.); h->SetMaximum(j?2.0:1.2);
				if (first){ h->SetTitle(j?"<p_{T}> vs N_{ch}, in-peak V0s in the pair types":"<p_{T}> vs N_{ch}, #pi K p (both charges) in the pair types"); }
				h->GetXaxis()->SetRangeUser(0.,30.);
				h->Draw(first?"pe":"pe same"); first = false;
				lg->AddEntry(h,j?v0nm[k]:idName[k],"lp");
			}
			if (valNchHi<9999){
				gPad->Update();
				TBox *bx	= new TBox(valNchLo-0.5,gPad->GetUymin(),valNchHi+0.5,gPad->GetUymax());
				bx->SetFillStyle(3354); bx->SetFillColor(kOrange+1); bx->SetLineColor(kOrange+1); bx->Draw();
				lg->AddEntry(bx,Form("this run's class: %d-%d",valNchLo,valNchHi),"f");
			}
			lg->Draw();
		}
		gStyle->SetPaintTextFormat("g");
	}
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
	//---- 5 MeV bins, raw levels (per-event norm, i.e. the 1+R2 baseline). Left 0-2.0 GeV with the
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
				double xmax	= (ipad==0) ? 2.0 : 0.3;
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
			//---- (README_CQ): C(Q) 0-2.0 GeV with decay signposts | C(Q) in STAR's 4 kT bins |
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
				DrawCQSignposts(ipid1,ipid2,gPad->GetUymin(),gPad->GetUymax(),hf->GetXaxis()->GetXmax());
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
		<<" pairs: duplicate="<<nFlagged_duplicate<<" complementary="<<nFlagged_complementary<<" (sec 17.14) fullmvtx="<<nFlagged_fullmvtx
		<<" ULS(opp-charge)="<<nFlagged_ULS<<" (sec 18.10, track-level)"<<" looper="<<nFlagged_Looper<<" (sec 35)"<<endl;
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
			hQsibW[ipaty]			->Write();
			hQmixW[ipaty]			->Write();
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
			if (ipaty<=3) for (int sm=0;sm<2;sm++) for (int is=0;is<2;is++) for (int ir=0;ir<4;ir++) R[ipaty]->GethTTR(sm,is,ir)->Write();
			if (ipaty<=3) for (int sm=0;sm<2;sm++){ TH2D* h=R[ipaty]->GethISep(sm); h->SetName(Form("hISep_%s_%d",sm?"M":"S",ipaty)); h->Write(); }	// sec 29
			if (ipaty<=3) for (int sm=0;sm<2;sm++){ TH2D* h=R[ipaty]->GethGapDy(sm); h->SetName(Form("hGapDy_%s_%d",sm?"M":"S",ipaty)); h->Write(); }	// sec 28
			if (ipaty<=3) for (int sm=0;sm<2;sm++){	// sec 32
				TH2D* h=R[ipaty]->GethGapPhi(sm);  h->SetName(Form("hGapPhi_%s_%d" ,sm?"M":"S",ipaty)); h->Write();
				h=R[ipaty]->GethGapYbar(sm);       h->SetName(Form("hGapYbar_%s_%d",sm?"M":"S",ipaty)); h->Write();
				h=R[ipaty]->GethGapPt(sm);         h->SetName(Form("hGapPt_%s_%d"  ,sm?"M":"S",ipaty)); h->Write();
				h=R[ipaty]->GethGapNch(sm);        h->SetName(Form("hGapNch_%s_%d" ,sm?"M":"S",ipaty)); h->Write();
				h=R[ipaty]->GethGapRm(sm);         h->SetName(Form("hGapRm_%s_%d"  ,sm?"M":"S",ipaty)); h->Write(); }
			if (ipaty<=3) for (int sm=0;sm<2;sm++) for (int w=0;w<2;w++){ TH2D* h=R[ipaty]->GethGap(sm,w); h->SetName(Form("hGap_%s_%s_%d",sm?"M":"S",w?"near":"far",ipaty)); h->Write(); }	// sec 28
			if (ipaty<=3) for (int sm=0;sm<2;sm++) for (int sd=0;sd<2;sd++){ TH2D* h=R[ipaty]->GethTTRside(sm,sd); h->SetName(Form("hTTRside_%s_%s_%d",sm?"M":"S",sd?"dirty":"clean",ipaty)); h->Write(); }
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
	if ( fabs((*vtxz)) > valVzMax ) return false;	// default 16 cm ("vzNN" overrides), README_SplitTracks573 sec 30
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
	if (!AcceptTrackBase(it)) return false;
	if (doSiPhiMask && InSiPhiMask(it)) return false;
	if (doCMMask && InCMMask(it)) return false;
	return true;
}
bool corral::InCMMask(int it){
	//---- sec 38: the mask is FIXED within a zvtx slice (it uses the slice centre, not the event's zvtx): every event of
	//---- a slice then has the same acceptance, which cancels in S/M. A mask that follows each event's zvtx moves its sharp
	//---- edge by 0.036 in eta across a 2-cm slice, and S (one zvtx) and M (two) see different edges (CMscan, 1st round).
	const double zl = -16., zu = 16.;
	double zc	= *vtxz;
	if (zc>zl && zc<zu){ double bw = (zu-zl)/valZvtxNB; zc = zl + (floor((zc-zl)/bw)+0.5)*bw; }
	double w	= (eta[it] + CMMASK_K*zc) * (zc<0. ? 1. : -1.);
	return (w>=valCMMaskLo && w<valCMMaskHi);
}
bool corral::InSiPhiMask(int it){
	double pd	= phi[it]*180.0/M_PI;
	for (int k=0;k<nSiPhiMask;k++) if (pd>=SIPHIMASK_LO[k] && pd<SIPHIMASK_HI[k]) return true;
	return false;
}
bool corral::AcceptTrackBase(int it){
	//if ( !primary[it]              ) return false;
	if (  pt[it]         < PTMINCUT ) return false;
	if (  pt[it]         >    20.1 ) return false;
	if (  ntpc[it]       < NTPCCUT ) return false;
	if ( fabs(dcaxy[it]) >     1.5 ) return false;
	if ( fabs(dcaz[it])  >     1.5 ) return false;
	return true;
}







