#ifndef __CalcRm_h__
#define __CalcRm_h__

#include <iostream>
#include <iomanip>
#include <limits>
#include "TROOT.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TProfile.h"
#include "TMath.h"
#include "TVector3.h"
#include "TLorentzVector.h"
//#include "TStopwatch.h"

using namespace std;

class CalcRm{
	//
	public:
		//
		CalcRm();				// ctor
		virtual ~CalcRm();		// dtor
		//
		virtual void	SetDistinguishable(bool b);		
		virtual bool	GetDistinguishable();
		virtual void	SetDoMinvCut(bool b);
		virtual bool	GetDoMinvCut();		
		virtual void	SetDoMinvLLCut(bool b);		
		virtual bool	GetDoMinvLLCut();		
		virtual void	SetFlipDy(bool b);		
		virtual bool	GetFlipDy();		
		virtual void	SetDoCrossing(bool b);		
		virtual bool	GetDoCrossing();		
		virtual void	SetField(double f);		// needed before Book() for the dirty-side rule; Increment() also sets it
		virtual int		GetDirtySide();			// computed in Book(), see CrossingDirtySide() in CrossingCorrect.h
		virtual void	SetDoBaseline(bool b);		
		virtual bool	GetDoBaseline();		
		virtual void	SetDoQcut(bool b);		
		virtual void	SetInttSep(double dphistar){ fISepDphi=dphistar; }	// README_SplitTracks573 sec 29
		virtual void	SetTrackSep(double dy, double dphistar, bool useMin=false){ fTSepDy=dy; fTSepDphi=dphistar; fTSepMin=useMin; }	// README_SplitTracks573 sec 11
		virtual bool	GetDoQcut();		
		virtual void	SetQcut(double v);		
		virtual double	GetQcut();		
		virtual void	SetCurrentRunEvt(int r, int e){ fCurrentRun=r; fCurrentEvt=e; }	// sec 18.20
		virtual void	SetCurrentNch(int n){ fCurrentNch=n; }		// README_SplitTracks573 sec 34: accepted charged tracks
		virtual void	SetExcludeAdjTF(int m){ fExcludeAdjTF=m; }	// sec 18.23: 0 off, 1 skip |devt|==1, 2 skip |devt|<=1
		virtual int		GetExcludeAdjTF(){ return fExcludeAdjTF; }
		virtual long	GetNCombUsed(int izv){ return nComb_used[izv]; }
		virtual long	GetNCombPossible(int izv){ return nComb_possible[izv]; }
		virtual void	SetPid1(int m);
		virtual int		GetPid1();
		virtual void	SetPid2(int m);
		virtual int		GetPid2();
// 		virtual void	SetChg1(double m);
// 		virtual double	GetChg1();
// 		virtual void	SetChg2(double m);
// 		virtual double	GetChg2();
// 		virtual void	SetMass1(double m);
// 		virtual double	GetMass1();
// 		virtual void	SetMass2(double m);
// 		virtual double	GetMass2();
		virtual void	SetMAXMULT(int i);
		virtual int		GetMAXMULT();
		virtual void	SetNMIX(int i);
		virtual int		GetNMIX();
		virtual void	SetYNB1(int i);
		virtual int		GetYNB1();
		virtual void	SetYL1(double y);
		virtual double	GetYL1();
		virtual void	SetYU1(double y);
		virtual double	GetYU1();
		virtual void	SetYNB2(int i);
		virtual int		GetYNB2();
		virtual void	SetYL2(double y);
		virtual double	GetYL2();
		virtual void	SetYU2(double y);
		virtual void	SetDYBW(double w);		// 2026-09-30: dy bin width of the (dy,dphi) maps, per pair type (0 = the y bin width)
		virtual double	GetYU2();
		virtual void	SetPHINB(int i);
		virtual int		GetPHINB();
		virtual void	SetPHIL(double phi);
		virtual double	GetPHIL();
		virtual void	SetPHIU(double phi);
		virtual double	GetPHIU();
		virtual void	SetPTL1(double pt);		// setptnbX etc not needed !! UMMM NO, neeeded if save rho1 !!
		virtual double	GetPTL1();
		virtual void	SetPTU1(double pt);
		virtual double	GetPTU1();
		virtual void	SetPTL2(double pt);
		virtual double	GetPTL2();
		virtual void	SetPTU2(double pt);
		virtual double	GetPTU2();
		virtual void	SetZVTXNB(int i);
		virtual int		GetZVTXNB();
		virtual void	SetZVTXL(double y);
		virtual double	GetZVTXL();
		virtual void	SetZVTXU(double y);
		virtual double	GetZVTXU();
		//virtual void	SetQuiet(bool b);
		//virtual void	SetZero(bool b);
		//virtual void	SetMixing(bool b);
		//virtual void	SetUnlikeSign(bool b);		
		//virtual bool	GetUnlikeSign();		
		virtual void	UpdateBinningPars();
		virtual void 	Book();
// 		virtual void 	Increment(double zv=0, double field=0, 
// 									 int n1=0, TLorentzVector* v1=0, 
// 									 int n2=0, TLorentzVector* v2=0);
// 		virtual bool	PairInfo(TLorentzVector v1,
// 								 TLorentzVector v2,
// 								 double& y1,double& y2,double& dy,double& dphi,double& dpt,double& dq,
// 								 double& minv, bool& NearSide);
		virtual void 	Increment(double zv=0, double field=0,
									 int n1=0, double (*e1)=0, double (*v1)[3]=0,
									 int n2=0, double (*e2)=0, double (*v2)[3]=0);
		virtual bool	PairInfo(int i, int iev, 
								 int j, int jev,
								 int izv,
								 double& y1,double& y2,double& dy,double& dphi,double& dpt,double& dq,double& Q,
								 double& minv, bool& NearSide, double& kT);
		virtual void 	Calculate();
		virtual double	GetR2Baseline(TH2D* hm=0);
		//
		virtual int		GetSpecies(int ipid){
			int ival	= -1;
			if (ipid<  5){ ival = ipid;   } else
			if (ipid< 10){ ival = ipid-5; } else
			if (ipid==10){ ival = 5;      } else
			if (ipid==11){ ival = 6;      } else
			if (ipid==12){ ival = 7;      }
			return ival;
		}
		//
//		virtual double	GetTimeIncrement();		
//		virtual double	GetTimeCalculate();		
		//
		virtual TH1D*	Gethzvtx();						// zvtx distribution
		//
		virtual TH2D*	Gethmult();						// no parameters  -> returns Zvtx-averaged histogram
 		virtual TH3D*	Gethrho1_1();					// no parameters  -> returns Zvtx-averaged histogram
 		virtual TH3D*	Gethrho1_2();					// no parameters  -> returns Zvtx-averaged histogram
		virtual TH2D*	Gethrho2_S(int ir2);			//  1 parameter   -> returns Zvtx-averaged histogram
		virtual TH2D*	Gethrho2_M(int ir2);			//  1 parameter   -> returns Zvtx-averaged histogram
		virtual TH2D*	GethC2(int ir2);				//  1 parameter   -> returns Zvtx-averaged histogram
		virtual TH2D*	GethR2(int ir2);				//  1 parameter   -> returns Zvtx-averaged histogram
 		virtual TH1D*	Gethy1_();						// no parameters  -> returns Zvtx-averaged histogram
 		virtual TH1D*	Gethy2_();						// no parameters  -> returns Zvtx-averaged histogram
 		virtual TH1D*	Gethyc1_();						// no parameters  -> returns Zvtx-averaged CORR histogram
 		virtual TH1D*	Gethyc2_();						// no parameters  -> returns Zvtx-averaged CORR histogram
		//
		virtual TH2D*	Gethmult(int izv);				//  1 parameter  -> returns histogram in this Zvtx bin
 		virtual TH3D*	Gethrho1_1(int izv);			//  1 parameter  -> returns histogram in this Zvtx bin
 		virtual TH3D*	Gethrho1_2(int izv);			//  1 parameter  -> returns histogram in this Zvtx bin
		virtual TH2D*	Gethrho2_S(int ir2, int izv);	//  2 parameters -> returns histogram in this Zvtx bin
		virtual TH2D*	Gethrho2_M(int ir2, int izv);	//  2 parameters -> returns histogram in this Zvtx bin
		virtual TH2D*	GethC2(int ir2, int izv);		//  2 parameters -> returns histogram in this Zvtx bin
		virtual TH2D*	GethR2(int ir2, int izv);		//  2 parameters -> returns histogram in this Zvtx bin
		virtual TH2D*	Gethrho2C_S();					// crossing-corrected (dy,dphi) rho2(S), Zvtx-averaged (README_Crossing)
		virtual TH2D*	GethC2C();						// crossing-corrected (dy,dphi) C2,      Zvtx-averaged
		virtual TH2D*	GethR2C();						// crossing-corrected (dy,dphi) R2,      Zvtx-averaged
		virtual TH2D*	Gethrho2C_S(int izv);			// same, in this Zvtx bin
		virtual TH2D*	GethC2C(int izv);				// same, in this Zvtx bin
		virtual TH2D*	GethR2C(int izv);				// same, in this Zvtx bin
		virtual TH2D*	GethMempty(){ return hMempty; }	// # of used Zvtx slices with an empty (dy,dphi) denominator
		virtual double	GetDenomZ(int izv){ return fDenomZ[izv]; }	// README_Finalize: 0 = zvtx bin skipped in Calculate
 		virtual TH1D*	Gethy1_(int izv);				//  1 parameter  -> returns histogram in this Zvtx bin
 		virtual TH1D*	Gethy2_(int izv);				//  1 parameter  -> returns histogram in this Zvtx bin
 		virtual TH1D*	Gethyc1_(int izv);				//  1 parameter  -> returns CORR histogram in this Zvtx bin
 		virtual TH1D*	Gethyc2_(int izv);				//  1 parameter  -> returns CORR histogram in this Zvtx bin
		//
		virtual TH2D*	Gethrho2_S_dptP(int ir2);			//  1 parameter  -> returns Zvtx-averaged histogram
		virtual TH2D*	Gethrho2_M_dptP(int ir2);			//  1 parameter  -> returns Zvtx-averaged histogram
		virtual TH2D*	Gethrho2_S_dptN(int ir2);			//  1 parameter  -> returns Zvtx-averaged histogram
		virtual TH2D*	Gethrho2_M_dptN(int ir2);			//  1 parameter  -> returns Zvtx-averaged histogram
		virtual TH2D*	Gethrho2_S_dptP(int ir2, int izv);	//  2 parameters -> returns histogram in this Zvtx bin
		virtual TH2D*	Gethrho2_M_dptP(int ir2, int izv);	//  2 parameters -> returns histogram in this Zvtx bin
		virtual TH2D*	Gethrho2_S_dptN(int ir2, int izv);	//  2 parameters -> returns histogram in this Zvtx bin
		virtual TH2D*	Gethrho2_M_dptN(int ir2, int izv);	//  2 parameters -> returns histogram in this Zvtx bin
		//
		virtual TH1D*	GethR2yydy();					// no parameters  -> returns Zvtx-averaged histogram
		virtual TH1D*	GethR2yydy(int izv);			// one parameter  -> returns histogram in this Zvtx bin
		virtual TH1D*	GethR2dy();						// no parameters  -> returns Zvtx-averaged histogram
		virtual TH1D*	GethR2dy(int izv);				// one parameter  -> returns histogram in this Zvtx bin
		virtual TH1D*	GethR2dphi();					// no parameters  -> returns Zvtx-averaged histogram
		virtual TH1D*	GethR2dphi(int izv);			// one parameter  -> returns histogram in this Zvtx bin
		virtual TH1D*	GethR2dq();						// no parameters  -> returns Zvtx-averaged histogram
		virtual TH1D*	GethR2dq(int izv);				// one parameter  -> returns histogram in this Zvtx bin
		//
// 		virtual TH1D*	GethR2dyNS();					// no parameters  -> returns Zvtx-averaged histogram
// 		virtual TH1D*	GethR2dyNS(int izv);			// one parameter  -> returns histogram in this Zvtx bin
// 		virtual TH1D*	GethR2dphiNS();					// no parameters  -> returns Zvtx-averaged histogram
// 		virtual TH1D*	GethR2dphiNS(int izv);			// one parameter  -> returns histogram in this Zvtx bin
// 		virtual TH1D*	GethR2dqNS();					// no parameters  -> returns Zvtx-averaged histogram
// 		virtual TH1D*	GethR2dqNS(int izv);			// one parameter  -> returns histogram in this Zvtx bin
		//
		virtual TH1D*	GethMinv_S();					// no parameters  -> returns Zvtx-averaged histogram
		virtual TH1D*	GethMinv_S(int izv);			// one parameter  -> returns histogram in this Zvtx bin
		virtual TH1D*	GethMinv_M();					// no parameters  -> returns Zvtx-averaged histogram
		virtual TH1D*	GethMinv_M(int izv);			// one parameter  -> returns histogram in this Zvtx bin
		virtual TH1D*	GethMinv();						// no parameters  -> returns Zvtx-averaged histogram
		virtual TH1D*	GethMinv(int izv);				// one parameter  -> returns histogram in this Zvtx bin
//		virtual TH2D*	GethMinvdq();					// no parameters  -> returns Zvtx-averaged histogram
//		virtual TH2D*	GethMinvdq(int izv);			// one parameter  -> returns histogram in this Zvtx bin
		//
		//---- femtoscopic Qinv numerator (sibling), denominator (mixed), and C(Q)=sib/mix ratio
		virtual TH2D*	GethZoomS(){ return hzoom_S; }	// README sec 16.8 spike-extent zoom, sibling
		virtual TH2D*	GethZoomM(){ return hzoom_M; }	// README sec 16.8 spike-extent zoom, mixed
		virtual TH2D*	GethTTR(int sm, int is, int ir){ return hTTR[sm][is][ir]; }	// README_SplitTracks573 sec 11
		virtual TH2D*	GethTTRside(int sm, int sd){ return hTTRside[sm][sd]; }	// README_SplitTracks573 sec 20
		virtual void	FillTTR(int sm, int i, int iev, int j, int jev, int izv, double weight);
		virtual TH2D*	GethGap(int sm, int w){ return hGap[sm][w]; }	// README_SplitTracks573 sec 28
		virtual TH2D*	GethGapDy(int sm){ return hGapDy[sm]; }	// README_SplitTracks573 sec 28
		virtual TH2D*	GethGapPhi(int sm){ return hGapPhi[sm]; }	// README_SplitTracks573 sec 32 (the +-15 deg notch)
		virtual TH2D*	GethGapYbar(int sm){ return hGapYbar[sm]; }
		virtual TH2D*	GethGapPt(int sm){ return hGapPt[sm]; }		// sec 34: the notch vs pair mean pt
		virtual TH2D*	GethGapNch(int sm){ return hGapNch[sm]; }		// sec 34: the notch vs accepted charged tracks
		virtual TH2D*	GethGapRm(int sm){ return hGapRm[sm]; }		// sec 34.1: the notch vs the pair's meeting radius (where the two tracks meet in phi)
		virtual void	FillGap(int sm, int i, int iev, int j, int jev, int izv, double weight);
		virtual TH1D*	GethMinvFineS(){ return hMinvF_S; }	// fine Minv near threshold, sibling (all Zvtx, raw)
		virtual TH1D*	GethMinvFineM(){ return hMinvF_M; }	// same, mixed
		virtual long	GetNMixedPairs_total()      { return nMixedPairs_total; }
		virtual long	GetNMixedPairs_sameTF()     { return nMixedPairs_sameTF; }
		virtual long	GetNMixedPairs_neighborTF() { return nMixedPairs_neighborTF; }
		virtual TH1D*	GethQsib();						// no parameters  -> returns Zvtx-averaged histogram
		virtual TH1D*	GethQsib(int izv);				// one parameter  -> returns histogram in this Zvtx bin
		virtual TH1D*	GethQmix();						// no parameters  -> returns Zvtx-averaged histogram
		virtual TH1D*	GethQmix(int izv);				// one parameter  -> returns histogram in this Zvtx bin
		virtual TH1D*	GethQsibW(){ return hQsibW[ZVTXNB]; }	// long-Q test copies (0-8 GeV, 10 MeV), Zvtx-summed
		virtual TH1D*	GethQmixW(){ return hQmixW[ZVTXNB]; }
		virtual TH1D*	GethCQ();						// no parameters  -> returns Zvtx-averaged histogram
		virtual TH1D*	GethCQ(int izv);				// one parameter  -> returns histogram in this Zvtx bin
		virtual TH2D*	GethQsibKT(){ return hQsibKT[ZVTXNB]; }			// README_CQ: (Qinv,kT), Zvtx-summed
		virtual TH2D*	GethQsibKT(int izv){ return hQsibKT[izv]; }
		virtual TH2D*	GethQmixKT(){ return hQmixKT[ZVTXNB]; }
		virtual TH2D*	GethQmixKT(int izv){ return hQmixKT[izv]; }
		virtual TH2D*	GethCQKT(){ return hCQKT[ZVTXNB]; }				// Zvtx-averaged, like hCQ
		virtual TH2D*	GethCQKT(int izv){ return hCQKT[izv]; }
		//
	//
	private:
		//
// 		TStopwatch* TimerIncrement;
// 		TStopwatch* TimerCalculate;
// 		double	fTimeIncrement;
// 		double	fTimeCalculate;
		//
		//-------------------------------------
		int    NMIX;
		static const int NMIXMAX	=   50;
		static const int NTRKMAX	=  385;		// set same as other class and MAX_CALCR_N in loop...
		static const int NZVTXMAX	=  101;		// allows 1cm-wide bins over +/-50cm.... 
//		static const int NCOMBMAX	= 1000;		// comfortable for NMIX = 8 or less.... obsolete
//		static const int YNBMAX		=   40;		// needed for 4th dimension of R4 via TH3D...
		//
		static const int NR2TYPES	=   3;		// y1y2, dydphi, dydq
		//-------------------------------------
		//
		bool   fDistinguishable;
		bool   fLikeSign;
		bool   fDoCrossing;
		bool   fDoBaseline;
		bool   fDoQcut;
		double fTSepDy, fTSepDphi;
		double fISepDphi;		// sec 29: near-vertex two-track cut, abs(dphi*(ISEP_R)) < fISepDphi (deg) at abs(dy) < ISEP_DYMAX; OFF if <= 0
		int    fPairSM;			// sec 29: 0 while PairInfo sees a sibling pair, 1 a mixed pair (hISep)
	public:
		//---- sec 29 constants: the radius (from the radius scan, the net loss is deepest at 2-3 cm) and the dy gate
		//---- (the loss is gone by abs(dy) ~ 0.8)
		static constexpr double ISEP_R		= 0.03;		// m
		static constexpr double ISEP_DYMAX	= 1.0;
		virtual TH2D*	GethISep(int sm){ return hISep[sm]; }
		virtual double	GetISepDphi(){ return fISepDphi; }
	private:
		bool   fTSepMin;		// true: use min over R = 0.30-0.78 m of abs(dphi*) instead of R = 0.5 m ("tsepmYYPP")	// README_SplitTracks573 sec 11: two-track-resolution pair cut, OFF if fTSepDy<=0.
									// A charged like-sign pair (sibling AND mixed, in PairInfo) is rejected if
									// abs(y1-y2)<fTSepDy && abs(dphi*(0.5 m))<fTSepDphi (deg), dphi* with the sign "+" of FillTTR.
		bool   fDoDQ;		// set internally
		double fQcut;
		int    fExcludeAdjTF;	// sec 18.23: skip mixed EVENT pairs (iev,jev) from neighboring trigger frames
								// (same run, |evt1-evt2|==1; mode 2 also same-TF, |devt|==0 -- used when the
								// same-TF XTFclean cleaner is off). Whole event pairs are skipped, and the
								// mixed normalization becomes per-zvtx-bin: denomfactor_izv = (used event
								// pairs in izv)/nevt_izv instead of the fixed NCombinations2/NMIX.
		long   nComb_used[NZVTXMAX];		// sec 18.23: mixed event pairs actually filled, per zvtx bin
		double fDenomZ[NZVTXMAX];			// README_Finalize: rho2(M) normalization per zvtx bin (denomfactor_izv); 0 = bin not computed
		long   nComb_possible[NZVTXMAX];	// sec 18.23: mixed event pairs offered, per zvtx bin
		int    fCurrentRun;		// sec 18.20: this event's (run,evt), set once per event via
		int    fCurrentNch;		// sec 34: this event's accepted charged tracks (SetCurrentNch, before Increment)
		int    fCurrentEvt;		// SetCurrentRunEvt() right before Increment() -- (run,evt) uniquely
									// identifies the originating trigger frame (SDCC-claude confirmed
									// `evt` IS the TF's own EvtSequence, constant across every row from
									// that TF). Used to rebuild rho2(M) excluding same-TF mixed pairs.
		int    fPid1;
		int    fPid2;
		double fChg1;
		double fChg2;
		double fMass1;
		double fMass2;
		//
		double fField;
		int    fDirtySide;	// crossing correction: +1/-1 = dirty side is dphi>0/dphi<0, 0 = no correction (neutral leg)
		int    MAXMULT;
		int    YNB1;
		double YL1;
		double YU1;
		double YBW;
		double YBW1;
		int    YNB2;
		double YL2;
		double YU2;
		double YBW2;
		int    DYNB;
		double DYL;
		double DYU;
		double DYBW;
		double fDYBWset = 0.;
		int    PHINB;
		double PHIL;
		double PHIU;
		double PHIBW;
		int    DPHINB;
		double DPHIL;
		double DPHIU;
		double DPHIBW;
		int    PTNB1;	// set for BW=0.1 based on PTL,PTU so rho1 can be binned and saved (to allow convolution in finalize)
		int    PTNB2;	// set for BW=0.1 based on PTL,PTU so rho1 can be binned and saved (to allow convolution in finalize)
		double PTL1;		// conv class also has ptnbX, ptnbX etx, which are not needed for mixing!
		double PTU1;
		double PTL2;
		double PTU2;
		int    ZVTXNB;
		double ZVTXL;
		double ZVTXU;		
		double ZVTXBW;
		int    DQNB;
		double DQL;
		double DQU;
		//
		bool   fFlipDy;
		bool   fDoMinvCut;
		bool   fDoMinvCutb;
		bool   fDoMinvLLCut;
		double fMinvCut_m0;	
		double fMinvCutb_m0;	
		double fMinvCut_dm;
		double fMinvCut_mlower;	
		double fMinvCut_mupper;
		double fMinvCutb_mlower;	
		double fMinvCutb_mupper;
		double raddeg;
		//
		int				mix_nevt[NZVTXMAX];
		double			mix_field[NMIXMAX][NZVTXMAX];
		double			mix_zvtx[NMIXMAX][NZVTXMAX];
		int				mix_run[NMIXMAX][NZVTXMAX];	// sec 18.20: per-buffer-slot (run,evt) -- per
		int				mix_nch[NMIXMAX][NZVTXMAX];	// sec 34: accepted charged tracks of the buffered event
		int				mix_evt[NMIXMAX][NZVTXMAX];	// SDCC-claude, `evt` IS the trigger-frame's own
														// EvtSequence, constant across every row from
														// that TF -- (run,evt) uniquely IDs the TF, no
														// new branch needed. Used to rebuild rho2(M)
														// excluding any mixed pair drawn from the same TF.
		int				mix_mult1[NMIXMAX][NZVTXMAX];
		int				mix_mult2[NMIXMAX][NZVTXMAX];
		//TLorentzVector 	mix_part1[NTRKMAX][NMIXMAX][NZVTXMAX];
		//TLorentzVector 	mix_part2[NTRKMAX][NMIXMAX][NZVTXMAX];
		double		mix_part1[4][NTRKMAX][NMIXMAX][NZVTXMAX];	// 4: y,phi,pt,eff
		double		mix_part2[4][NTRKMAX][NMIXMAX][NZVTXMAX];	// 4: y,phi,pt,eff
		//
		//
		TH1D*	hzvtx;
		TAxis*	axiszvtx;
		//
//		virtual bool 	NextCombination(vector<int>& v, int k, int N);
//		int Combinations2[2][NCOMBMAX];			// indices of 2 tracks in each 2-combination 
//		int NCombinations2_Seen[NZVTXMAX];
		int NCombinations2;
		//
		TH2D*	hmult[NZVTXMAX];
		TH2D*	hmultc[NZVTXMAX];
		TH3D*	hrho1_1[NZVTXMAX];
		TH3D*	hrho1_2[NZVTXMAX];
//		TH3D*	hrho1c_1[NZVTXMAX];		// weighted by 1/eff...
//		TH3D*	hrho1c_2[NZVTXMAX];		// weighted by 1/eff...
		TH2D*	hrho2_S[NR2TYPES][NZVTXMAX];
		TH2D*	hrho2_M[NR2TYPES][NZVTXMAX];
		TH2D*	hC2[NR2TYPES][NZVTXMAX];
		TH2D*	hR2[NR2TYPES][NZVTXMAX];
		TH2D*	hrho2C_S[NZVTXMAX];		// README_Crossing: crossing-corrected (dy,dphi) only (ir2=1), no rho2(M) version
		TH2D*	hC2C[NZVTXMAX];
		TH2D*	hR2C[NZVTXMAX];
		TH2D*	hMempty;			// README_Crossing sec 8: per (dy,dphi) bin, # of used Zvtx slices with rho2(M)=0
		//TH2D*	hm2D[NR2TYPES];
		//
		TH1D*	hy1_[NZVTXMAX];
		TH1D*	hy2_[NZVTXMAX];
		TH1D*	hyc1_[NZVTXMAX];
		TH1D*	hyc2_[NZVTXMAX];
		//
		TH2D*	hrho2_S_dptP[NR2TYPES][NZVTXMAX];
		TH2D*	hrho2_M_dptP[NR2TYPES][NZVTXMAX];
		TH2D*	hrho2_S_dptN[NR2TYPES][NZVTXMAX];
		TH2D*	hrho2_M_dptN[NR2TYPES][NZVTXMAX];
		//
		TH1D*	hR2yydy[NZVTXMAX];			// from y1y2...
		TH1D*	hR2yydye[NZVTXMAX];			// from y1y2...
		TH1D*	hR2yydyN[NZVTXMAX];			// from y1y2...
		TH1D*	hR2dy[NZVTXMAX];			// from dydphi...
		TH1D*	hR2dye[NZVTXMAX];			// from dydphi...
		TH1D*	hR2dyN[NZVTXMAX];			// from dydphi...
		TH1D*	hR2dphi[NZVTXMAX];			// from dydphi...
		TH1D*	hR2dphie[NZVTXMAX];			// from dydphi...
		TH1D*	hR2dphiN[NZVTXMAX];			// from dydphi...
		TH1D*	hR2dq[NZVTXMAX];			// from dydq...
		TH1D*	hR2dqe[NZVTXMAX];			// from dydq...
		TH1D*	hR2dqN[NZVTXMAX];			// from dydq...
		//
// 		TH1D*	hR2dyNS[NZVTXMAX];			// from y1y2...
// 		TH1D*	hR2dyNSe[NZVTXMAX];			// from y1y2...
// 		TH1D*	hR2dyNSN[NZVTXMAX];			// from y1y2...
// 		TH1D*	hR2dphiNS[NZVTXMAX];			// from dydphi...
// 		TH1D*	hR2dphiNSe[NZVTXMAX];			// from dydphi...
// 		TH1D*	hR2dphiNSN[NZVTXMAX];			// from dydphi...
// 		TH1D*	hR2dqNS[NZVTXMAX];			// from dydq...
// 		TH1D*	hR2dqNSe[NZVTXMAX];			// from dydq...
// 		TH1D*	hR2dqNSN[NZVTXMAX];			// from dydq...
		//
		TH1D*	hMinv_S[NZVTXMAX];
		TH1D*	hMinv_M[NZVTXMAX];
		TH1D*	hMinv[NZVTXMAX];
		//
		TH2D*	hzoom_S;			// README sec 16.8: fine (deta,dphi) zoom around (0,0), same-event pairs, raw counts, all Zvtx
		TH2D*	hzoom_M;			// same, mixed pairs (smooth reference for the spike-extent measurement)
		//---- README_SplitTracks573 sec 11: two-track-resolution study, DIAGNOSTIC ONLY (no pair is cut).
		//---- dy vs dphi*(R) for [0 sibling | 1 mixed][sign hypothesis 0: +q*B, 1: -q*B]
		//---- [R = 0.30 | 0.50 | 0.70 m | the R in 0.30-0.78 m of smallest abs(dphi*)]. Charged-charged pairs only.
		TH2D*	hTTR[2][2][4];
		//---- sec 28 (sector-gap diagnostic): [0 sibling | 1 mixed][0: 0.4 <= abs(dy) < 1.0 | 1: abs(dy) < 0.4], x = vertex
		//---- dphi = phi1 - phi2 (index order, deg), y = s = q1*asin(aR/pt1) - q2*asin(aR/pt2) at R = 0.55 m (deg; the
		//---- sec 11 "+" sign), so dphi - s = dphi*(R), the pair's separation at the TPC. Raw counts, all Zvtx.
		TH2D*	hGap[2][2];
		TH2D*	hISep[2];
		long	nPairCut[2][3];		// Final state: [S|M][0 charged pairs reaching the pair cuts, 1 removed by tsep, 2 by isep]			// sec 29: [sibling|mixed] dphi*(ISEP_R) vs abs(dy), every charged pair before the isep cut
		TH2D*	hGapPhi[2];			// sec 32: [sibling|mixed] vertex dphi (1 deg, abs < 60) vs pair mean phi (5 deg), abs(dy) < 1
		TH2D*	hGapYbar[2];
		TH2D*	hGapPt[2];			// sec 34: [sibling|mixed] vertex dphi (1 deg, abs < 60) vs pair mean pt, abs(dy) < 1
		TH2D*	hGapRm[2];			// sec 34.1: [sibling|mixed] vertex dphi vs meeting radius R_m (m; 1.4 = the tracks never meet)
		TH2D*	hGapNch[2];			// sec 34: [sibling|mixed] vertex dphi vs accepted charged tracks (event of particle 1)		// sec 32: [sibling|mixed] vertex dphi (1 deg, abs < 60) vs ybar (0.1), abs(dy) < 1
		TH2D*	hGapDy[2];			// sec 28: [sibling|mixed] vertex dphi (1 deg, abs < 60) vs abs(dy) (0-2), raw counts, all Zvtx
		TH2D*	hTTRside[2][2];		// sec 20: [sibling|mixed][pt-ordered dphi side: 0 clean (dphi>0), 1 dirty (dphi<0)], sign +, min over R
		TH1D*	hMinvF_S;		// Minv, 500 x 1 MeV from m1+m2-5MeV, sibling pairs, raw counts summed over Zvtx (C(Q) page)
		TH1D*	hMinvF_M;		// same, mixed pairs
		long	nMixedPairs_total;
		long	nMixedPairs_sameTF;
		long	nMixedPairs_neighborTF;
		TH1D*	hQsib[NZVTXMAX];			// femtoscopic numerator: Qinv, same-event pairs
		TH1D*	hQmix[NZVTXMAX];			// femtoscopic denominator: Qinv, mixed-event pairs (NMIX-only normalization applied in Calculate)
		TH1D*	hQsibW[NZVTXMAX];			// long-Q test copies of hQsib / hQmix, 0-8 GeV in 10 MeV bins (README_CQcomparison sec 2.3)
		TH1D*	hQmixW[NZVTXMAX];
		TH1D*	hCQ[NZVTXMAX];				// femtoscopic C(Q) = hQsib/hQmix
		TH2D*	hQsibKT[NZVTXMAX];			// README_CQ: same as hQsib, vs (Qinv, kT) in STAR's 4 kT bins (flow bins = kT outside 0.15-0.60)
		TH2D*	hQmixKT[NZVTXMAX];			// same as hQmix, vs (Qinv, kT)
		TH2D*	hCQKT[NZVTXMAX];			// hQsibKT/hQmixKT
//		TH2D*	hMinvdq_S[NZVTXMAX];
//		TH2D*	hMinvdq_M[NZVTXMAX];
//		TH2D*	hMinvdq[NZVTXMAX];
		//
		long npair_all_S	;
		long npair_dylow1_S	;
		long npair_dylow2_S	;
		long npair_dylow3_S	;
		long npair_qlow_S	;
		long npair_mlow_S	;
		long npair_all_M	;	
		long npair_dylow1_M	;
		long npair_dylow2_M	;
		long npair_dylow3_M	;
		long npair_qlow_M	;
		long npair_mlow_M	;
		//
	//
	//ClassDef(CalcRm, 0)
};

inline void   CalcRm::SetDistinguishable(bool b){ fDistinguishable=b; return; }
inline bool   CalcRm::GetDistinguishable(){ return fDistinguishable; }
inline void   CalcRm::SetDoMinvCut(bool b){ fDoMinvCut=b; return; }
inline bool   CalcRm::GetDoMinvCut(){ return fDoMinvCut; }
inline void   CalcRm::SetDoMinvLLCut(bool b){ fDoMinvLLCut=b; return; }
inline bool   CalcRm::GetDoMinvLLCut(){ return fDoMinvLLCut; }
inline void   CalcRm::SetFlipDy(bool b){ fFlipDy=b; return; }
inline bool   CalcRm::GetFlipDy(){ return fFlipDy; }
inline void   CalcRm::SetDoCrossing(bool b){ fDoCrossing=b; return; }
inline bool   CalcRm::GetDoCrossing(){ return fDoCrossing; }
inline void   CalcRm::SetField(double f){ fField=f; return; }
inline int    CalcRm::GetDirtySide(){ return fDirtySide; }
inline void   CalcRm::SetDoBaseline(bool b){ fDoBaseline=b; return; }
inline bool   CalcRm::GetDoBaseline(){ return fDoBaseline; }
inline void   CalcRm::SetDoQcut(bool b){ fDoQcut=b; return; }
inline bool   CalcRm::GetDoQcut(){ return fDoQcut; }
inline void   CalcRm::SetQcut(double v){ fQcut=v; return; }
inline double CalcRm::GetQcut(){ return fQcut; }
inline void   CalcRm::SetPid1(int m){ fPid1=m;   return; }
inline int    CalcRm::GetPid1(){ return fPid1; }
inline void   CalcRm::SetPid2(int m){ fPid2=m;   return; }
inline int    CalcRm::GetPid2(){ return fPid2; }
// inline void  CalcRm::SetChg1(double m){ fChg1=m;   return; }
// inline double CalcRm::GetChg1(){ return fChg1; }
// inline void  CalcRm::SetChg2(double m){ fChg2=m;   return; }
// inline double CalcRm::GetChg2(){ return fChg2; }
// inline void  CalcRm::SetMass1(double m){ fMass1=m;   return; }
// inline double CalcRm::GetMass1(){ return fMass1; }
// inline void  CalcRm::SetMass2(double m){ fMass2=m;   return; }
// inline double CalcRm::GetMass2(){ return fMass2; }
inline void   CalcRm::SetMAXMULT(int i){ MAXMULT=i;   return; }
inline int    CalcRm::GetMAXMULT(){ return MAXMULT; }
inline void   CalcRm::SetNMIX(int i){ NMIX=i; return; }
inline int    CalcRm::GetNMIX(){ return NMIX; }
inline void   CalcRm::SetYNB1(int i){ YNB1=i;   return; }
inline void   CalcRm::SetYL1(double y){ YL1=y;  return; }
inline void   CalcRm::SetYU1(double y){ YU1=y;  return; }
inline int    CalcRm::GetYNB1(){ return YNB1; }
inline double CalcRm::GetYL1(){ return YL1; }
inline double CalcRm::GetYU1(){ return YU1; }
inline void   CalcRm::SetYNB2(int i){ YNB2=i;   return; }
inline void   CalcRm::SetYL2(double y){ YL2=y;  return; }
inline void   CalcRm::SetYU2(double y){ YU2=y;  return; }
inline void   CalcRm::SetDYBW(double w){ fDYBWset=w; return; }
inline int    CalcRm::GetYNB2(){ return YNB2; }
inline double CalcRm::GetYL2(){ return YL2; }
inline double CalcRm::GetYU2(){ return YU2; }
inline void   CalcRm::SetPHINB(int i){ PHINB=i;   return; }
inline void   CalcRm::SetPHIL(double phi){ PHIL=phi;  return; }
inline void   CalcRm::SetPHIU(double phi){ PHIU=phi;  return; }
inline int    CalcRm::GetPHINB(){ return PHINB; }
inline double CalcRm::GetPHIL(){ return PHIL; }
inline double CalcRm::GetPHIU(){ return PHIU; }
inline void   CalcRm::SetPTL1(double pt){ PTL1=pt;  return; }
inline void   CalcRm::SetPTU1(double pt){ PTU1=pt;  return; }
inline void   CalcRm::SetPTL2(double pt){ PTL2=pt;  return; }
inline void   CalcRm::SetPTU2(double pt){ PTU2=pt;  return; }
inline double CalcRm::GetPTL1(){ return PTL1; }				
inline double CalcRm::GetPTU1(){ return PTU1; }
inline double CalcRm::GetPTL2(){ return PTL2; }
inline double CalcRm::GetPTU2(){ return PTU2; }
inline void  CalcRm::SetZVTXNB(int i){ if(i+1>=NZVTXMAX){cout<<"no! max zvtx nbin is "<<NZVTXMAX<<endl; exit(0);} ZVTXNB=i; return; }
inline void  CalcRm::SetZVTXL(double y){ ZVTXL=y;  return; }
inline void  CalcRm::SetZVTXU(double y){ ZVTXU=y;  return; }
inline int   CalcRm::GetZVTXNB(){ return ZVTXNB; }
inline double CalcRm::GetZVTXL(){ return ZVTXL; }
inline double CalcRm::GetZVTXU(){ return ZVTXU; }
//inline void CalcRm::SetQuiet(bool b){ fQuiet=b; return; }
//inline void CalcRm::SetZero(bool b){ fZero=b; return; }
//inline void CalcRm::SetMixing(bool b){ fMixing=b; return; }
//inline void CalcRm::SetUnlikeSign(bool b){ fUnlikeSign=b; return; }
//inline bool CalcRm::GetUnlikeSign(){ return fUnlikeSign; }

//inline double CalcRm::GetTimeIncrement(){ return fTimeIncrement; }
//inline double CalcRm::GetTimeCalculate(){ return fTimeCalculate; }

inline TH1D* CalcRm::Gethzvtx(){ return hzvtx; }				// Zvtx distribution seen by this class
inline TH2D* CalcRm::Gethmult(){ return hmult[ZVTXNB]; }		// Zvtx-averaged hist returned if no izv given
inline TH2D* CalcRm::Gethmult(int izv){ return hmult[izv]; }	// if izv given then return hist for that zvtx bin

inline TH3D* CalcRm::Gethrho1_1(){ return hrho1_1[ZVTXNB]; }
inline TH3D* CalcRm::Gethrho1_2(){ return hrho1_2[ZVTXNB]; }
inline TH3D* CalcRm::Gethrho1_1(int izv){ return hrho1_1[izv]; }
inline TH3D* CalcRm::Gethrho1_2(int izv){ return hrho1_2[izv]; }

inline TH1D* CalcRm::Gethy1_(){ return hy1_[ZVTXNB]; }
inline TH1D* CalcRm::Gethy2_(){ return hy2_[ZVTXNB]; }
inline TH1D* CalcRm::Gethy1_(int izv){ return hy1_[izv]; }
inline TH1D* CalcRm::Gethy2_(int izv){ return hy2_[izv]; }
inline TH1D* CalcRm::Gethyc1_(){ return hyc1_[ZVTXNB]; }
inline TH1D* CalcRm::Gethyc2_(){ return hyc2_[ZVTXNB]; }
inline TH1D* CalcRm::Gethyc1_(int izv){ return hyc1_[izv]; }
inline TH1D* CalcRm::Gethyc2_(int izv){ return hyc2_[izv]; }

inline TH2D* CalcRm::Gethrho2_S(int ir2){ return hrho2_S[ir2][ZVTXNB]; }
inline TH2D* CalcRm::Gethrho2_S(int ir2, int izv){ return hrho2_S[ir2][izv]; }
inline TH2D* CalcRm::Gethrho2_M(int ir2){ return hrho2_M[ir2][ZVTXNB]; }
inline TH2D* CalcRm::Gethrho2_M(int ir2, int izv){ return hrho2_M[ir2][izv]; }

inline TH2D* CalcRm::Gethrho2_S_dptP(int ir2){ return hrho2_S_dptP[ir2][ZVTXNB]; }
inline TH2D* CalcRm::Gethrho2_S_dptN(int ir2){ return hrho2_S_dptN[ir2][ZVTXNB]; }
inline TH2D* CalcRm::Gethrho2_M_dptP(int ir2){ return hrho2_M_dptP[ir2][ZVTXNB]; }
inline TH2D* CalcRm::Gethrho2_M_dptN(int ir2){ return hrho2_M_dptN[ir2][ZVTXNB]; }
inline TH2D* CalcRm::Gethrho2_S_dptP(int ir2, int izv){ return hrho2_S_dptP[ir2][izv]; }
inline TH2D* CalcRm::Gethrho2_S_dptN(int ir2, int izv){ return hrho2_S_dptN[ir2][izv]; }
inline TH2D* CalcRm::Gethrho2_M_dptP(int ir2, int izv){ return hrho2_M_dptP[ir2][izv]; }
inline TH2D* CalcRm::Gethrho2_M_dptN(int ir2, int izv){ return hrho2_M_dptN[ir2][izv]; }

inline TH2D* CalcRm::GethC2(int ir2){ return hC2[ir2][ZVTXNB]; }
inline TH2D* CalcRm::GethC2(int ir2, int izv){ return hC2[ir2][izv]; }

inline TH2D* CalcRm::GethR2(int ir2){ return hR2[ir2][ZVTXNB]; }
inline TH2D* CalcRm::GethR2(int ir2, int izv){ return hR2[ir2][izv]; }
inline TH2D* CalcRm::Gethrho2C_S(){ return hrho2C_S[ZVTXNB]; }
inline TH2D* CalcRm::GethC2C(){ return hC2C[ZVTXNB]; }
inline TH2D* CalcRm::GethR2C(){ return hR2C[ZVTXNB]; }
inline TH2D* CalcRm::Gethrho2C_S(int izv){ return hrho2C_S[izv]; }
inline TH2D* CalcRm::GethC2C(int izv){ return hC2C[izv]; }
inline TH2D* CalcRm::GethR2C(int izv){ return hR2C[izv]; }
inline TH1D* CalcRm::GethR2yydy(){ return hR2yydy[ZVTXNB]; }
inline TH1D* CalcRm::GethR2yydy(int izv){ return hR2yydy[izv]; }
inline TH1D* CalcRm::GethR2dy(){ return hR2dy[ZVTXNB]; }
inline TH1D* CalcRm::GethR2dy(int izv){ return hR2dy[izv]; }
inline TH1D* CalcRm::GethR2dq(){ return hR2dq[ZVTXNB]; }
inline TH1D* CalcRm::GethR2dq(int izv){ return hR2dq[izv]; }
inline TH1D* CalcRm::GethR2dphi(){ return hR2dphi[ZVTXNB]; }
inline TH1D* CalcRm::GethR2dphi(int izv){ return hR2dphi[izv]; }

inline TH1D* CalcRm::GethMinv_S(){ return hMinv_S[ZVTXNB]; }
inline TH1D* CalcRm::GethMinv_S(int izv){ return hMinv_S[izv]; }
inline TH1D* CalcRm::GethMinv_M(){ return hMinv_M[ZVTXNB]; }
inline TH1D* CalcRm::GethMinv_M(int izv){ return hMinv_M[izv]; }
inline TH1D* CalcRm::GethMinv(){ return hMinv[ZVTXNB]; }
inline TH1D* CalcRm::GethMinv(int izv){ return hMinv[izv]; }

inline TH1D* CalcRm::GethQsib(){ return hQsib[ZVTXNB]; }
inline TH1D* CalcRm::GethQsib(int izv){ return hQsib[izv]; }
inline TH1D* CalcRm::GethQmix(){ return hQmix[ZVTXNB]; }
inline TH1D* CalcRm::GethQmix(int izv){ return hQmix[izv]; }
inline TH1D* CalcRm::GethCQ(){ return hCQ[ZVTXNB]; }
inline TH1D* CalcRm::GethCQ(int izv){ return hCQ[izv]; }

//inline TH2D* CalcRm::GethMinvdq(){ return hMinvdq[ZVTXNB]; }
//inline TH2D* CalcRm::GethMinvdq(int izv){ return hMinvdq[izv]; }

// inline TH1D* CalcRm::GethR2dyNS(){ return hR2dyNS[ZVTXNB]; }
// inline TH1D* CalcRm::GethR2dyNS(int izv){ return hR2dyNS[izv]; }
// inline TH1D* CalcRm::GethR2dqNS(){ return hR2dqNS[ZVTXNB]; }
// inline TH1D* CalcRm::GethR2dqNS(int izv){ return hR2dqNS[izv]; }
// inline TH1D* CalcRm::GethR2dphiNS(){ return hR2dphiNS[ZVTXNB]; }
// inline TH1D* CalcRm::GethR2dphiNS(int izv){ return hR2dphiNS[izv]; }

#endif




