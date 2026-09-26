#include "CalcRm.h"
#include "fluct_common.h"
#include "CrossingCorrect.h"	// README_Crossing: dirty-side rule + correction, shared verbatim with Finalize
#include "ZvtxAverage.h"		// README_Finalize: Zvtx average over valid bins, shared with Finalize

//------------------------------------------------------------
void Swap(double &a, double &b){
	double a0=a; a=b; b=a0;
}

//------------------------------------------------------------
CalcRm::CalcRm(){
	//
	fDoCrossing			= false;	// set externally
	fDirtySide			= 0;		// set in Book()
	fField				= 0.;		// set externally (SetField) and in Increment()
	fDoBaseline			= false;	// set externally
	fDoMinvCut			= false;	// set externally (option does not presently exist for convolution)
	fDoMinvCutb			= false;	// set internally (2nd parent mass for ULS Kaons)
	fDoMinvLLCut		= false;	// set externally (option does not presently exist for convolution)
	fDoQcut				= false;	// set externally
	fExcludeAdjTF		= 0;		// sec 18.23, OFF by default
	for (int iz=0;iz<NZVTXMAX;iz++){ nComb_used[iz]=0; nComb_possible[iz]=0; }
	fCurrentRun			= 0;		// sec 18.20, set externally each event via SetCurrentRunEvt()
	fCurrentEvt			= 0;
	nMixedPairs_total		= 0;
	nMixedPairs_sameTF		= 0;
	nMixedPairs_neighborTF	= 0;
	fDoDQ				= true;		// if false: speeds up code a lot but dQ & Minv will always equal 0...
									// if true, enable the calculation of dQ and Minv even if not enabled by calling routine
	fDistinguishable	= false;
	fLikeSign			= false;
	fFlipDy				= false;		// set externally
 	//
	fQcut	= 0.15;
	//
	YNB1	= 20;
	YL1		= -0.5;
	YU1		=  0.5;
	YBW1	= (YU1-YL1)/YNB1;
	YNB2	= 20;
	YL2		= -0.5;
	YU2		=  0.5;
	YBW2	= (YU2-YL2)/YNB2;
	//
	DYBW	= YBW1;			// code will require that YBW1=YBW2...
	DYL		= YL1 - YU2 + DYBW/2.;
	DYU		= YU1 - YL2 - DYBW/2.;
	DYNB	= (DYU-DYL)/DYBW;
	//
	PHINB	=  12 ;		
	PHIL	=   0.;		
	PHIU	= 360.;		
	DPHINB	= PHINB;
	DPHIL	= -90.;
	DPHIU	= 270.;
	PTNB1	=    1;		// needed only for rho1 (saved to allow convolution in finalize) calculated in code!
	PTNB2	=    1;		// needed only for rho1 (saved to allow convolution in finalize) calculated in code!
	PTL1	=   0.2000000;
	PTU1	=   2.0000000;		
	PTL2	=   0.2000000;		
	PTU2	=   2.0000000;		
	DQNB	= 100;		//Special LOGX binning used here! Also not yet settable in calling routine, don't forget to modify UpdateBinningPars() too!
	DQL		=   1.0e-2;	//Special LOGX binning used here! Also not yet settable in calling routine, don't forget to modify UpdateBinningPars() too!
	DQU		=  10.0;	//Special LOGX binning used here! Also not yet settable in calling routine, don't forget to modify UpdateBinningPars() too!
	ZVTXNB	=  30;
	ZVTXL	= -30.;
	ZVTXU	= +30.;
	//
	fPid1	= -1 ;
	fPid2	= -1 ;
	fChg1	=  0.;
	fChg2	=  0.;
	fMass1	=  0.;
	fMass2	=  0.;
	//
	MAXMULT	= 100;
	//
    NMIX			= 5;	// can be changed via inline function SetNMIX
	//
	//npair_all_S		= 0;
	//npair_dylow1_S	= 0;
	//npair_dylow2_S	= 0;
	//npair_dylow3_S	= 0;
	//npair_qlow_S		= 0;
	//npair_mlow_S		= 0;
	//npair_all_M		= 0;
	//npair_dylow1_M	= 0;
	//npair_dylow2_M	= 0;
	//npair_dylow3_M	= 0;
	//npair_qlow_M		= 0;
	//npair_mlow_M		= 0;
	//
	raddeg	= 180./M_PI;
	//
// 	TimerIncrement	= new TStopwatch();
// 	TimerIncrement	->Stop();
// 	TimerIncrement	->Reset();
// 	TimerCalculate	= new TStopwatch();
// 	TimerCalculate	->Stop();
// 	TimerCalculate	->Reset();
// 	fTimeIncrement	= 0;
// 	fTimeCalculate	= 0;
	//
}

//------------------------------------------------------------
CalcRm::~CalcRm(){
	//
	//delete[] hmult;		//hmult=0;
	//delete[] hrho2;		//hrho2=0;
	//delete[] hR2;			//hR2=0;
	//delete hm2D;			hm2D=0;
	//delete[] hR2dy;		//hR2dy=0;
	//delete[] hR2dye;		//hR2dye=0;
	//delete[] hR2dyN;		//hR2dyN=0;
	//
}

//------------------------------------------------------------
void CalcRm::UpdateBinningPars(){
	YBW1	= (YU1-YL1)/YNB1;
	YBW2	= (YU2-YL2)/YNB2;
	if (10000.*fabs(YBW1-YBW2)>1.0){
		cout<<"CalcRm::UpdateBinningPars .. Binning issue: YBW1!=YBW2 "
			<<YBW1<<" "<<YBW2<<" \t "
			<<YL1<<" "<<YU1<<" "<<YNB1<<" "<<YL2<<" "<<YU2<<" "<<YNB2
			<<endl;
		exit(0);
	}
	YBW		= YBW1;
	DYBW	= YBW1;
	DYL		= YL1-YU2+DYBW/2.;
	DYU		= YU1-YL2-DYBW/2.;
	DYNB	= (DYU-DYL)/DYBW;
	//cout<<"CalcRm::UpdateBinningPars .. "
	//	<<YL1<<" "<<YU1<<" "<<YNB1<<" bw="<<YBW1<<" \t"
	//	<<YL2<<" "<<YU2<<" "<<YNB2<<" bw="<<YBW2<<" \t"
	//	<<DYL<<" "<<DYU<<" "<<DYNB<<" bw="<<DYBW<<" \t"
	//	<<endl;
	//
	DPHINB	= PHINB;
	PHIBW	= (PHIU-PHIL)/PHINB;
 	PTNB1	= (PTU1-PTL1)/0.1;		// used only to bin rho1 (not needed for mixing, just saving it)
 	PTNB2	= (PTU2-PTL2)/0.1;		// used only to bin rho1 (not needed for mixing, just saving it)
	ZVTXBW	= (ZVTXU-ZVTXL)/ZVTXNB;	
	//
	if (fDoQcut     ) fDoDQ = true;
	if (fDoMinvCut  ) fDoDQ = true;
	if (fDoMinvLLCut) fDoDQ = true;
	//
}

//------------------------------------------------------------
void CalcRm::Book(){
	//
	UpdateBinningPars();
	//cout<<"CalcRm::Book .. Y1-limits = "<<YL1<<" "<<YU1<<endl; 
	//cout<<"CalcRm::Book .. Y2-limits = "<<YL2<<" "<<YU2<<endl;
	//cout<<"CalcRm::Book ..  f-limits = "<<PHIL<<" "<<PHIU<<endl; 
	//cout<<"CalcRm::Book .. DY-limits = "<<DYL<<" "<<DYU<<endl;
	//cout<<"CalcRm::Book .. Df-limits = "<<DPHIL<<" "<<DPHIU<<endl;
 	//
	if (fPid1<0. || fPid2<0.){
		cout<<"CalcRm::Book .. Pid undefined! exit...."<<fPid1<<" "<<fPid2<<endl; exit(0); 
	}
	fDistinguishable					= true; 
	if (fPid1==fPid2) fDistinguishable 	= false;
	int ispe1	= GetSpecies(fPid1);
	int ispe2	= GetSpecies(fPid2);
	fChg1		= ParticleCharge[fPid1];
	fChg2		= ParticleCharge[fPid2];
	fMass1		= Species_mass[ispe1];
	fMass2		= Species_mass[ispe2];
//	if (fChg1==0. || fChg2==0.){
//		cout<<"CalcRm::Book .. Chg undefined! exit...."<< endl; exit(0); 
//	}
	if (fMass1==0. || fMass2==0.){
		cout<<"CalcRm::Book .. Masses undefined! exit...."<< endl; exit(0); 
	}
	if (fChg1*fChg2>0.) fLikeSign	= true;		// allow +e and +2e particles to be crossing corrected
	//
	//---- crossing correction: dirty side (README_Crossing sec 8, step 3)
	fDirtySide	= CrossingDirtySide(fField,fChg1,fChg2);
	cout<<"CalcRm::Book .. crossing: doCrossing="<<(int)fDoCrossing<<"  field="<<fField
		<<"  chg1="<<fChg1<<" chg2="<<fChg2<<"  dirty side = "<<fDirtySide
		<<(fDirtySide>0?" (dphi>0)":(fDirtySide<0?" (dphi<0)":" (none)"))<<endl;
	//
	//
	if (NMIX>NMIXMAX){ cout<<"CalcRm::Book .. NMIX too large .. "<<NMIX<<" "<<NMIXMAX<<endl; exit(0); }
	//
	//---- Minv cut setup...
	bool enable			= false;
	fMinvCut_m0			= 0;
	fMinvCutb_m0		= 0;
	fMinvCut_dm			= 0;
	fMinvCut_mlower		= 0;
	fMinvCut_mupper		= 0;
	fMinvCutb_mlower	= 0;
	fMinvCutb_mupper	= 0;
	if (fDoMinvCut){
		// Ks = pi+ + pi-
		if ( (fPid1==kParticleIDPionPlus    && fPid2==kParticleIDPionMinus   )
		  || (fPid1==kParticleIDPionMinus   && fPid2==kParticleIDPionPlus    ) ){ enable=true; fMinvCut_m0=0.493677; fMinvCut_dm=0.01; } else
		// phi = K+ + K-
		if ( (fPid1==kParticleIDKaonPlus    && fPid2==kParticleIDKaonMinus   )
		  || (fPid1==kParticleIDKaonMinus   && fPid2==kParticleIDKaonPlus    ) ){ enable=true; fMinvCut_m0=1.019461; fDoMinvCutb=true; fMinvCutb_m0=1.86484; fMinvCut_dm=0.01; } else
		// L = p+ + pi-
		if ( (fPid1==kParticleIDProton      && fPid2==kParticleIDPionMinus   )
		  || (fPid1==kParticleIDPionMinus   && fPid2==kParticleIDProton      ) ){ enable=true; fMinvCut_m0=1.115683; fMinvCut_dm=0.01; } else
		// Lbar = p- + pi+
		if ( (fPid1==kParticleIDAntiProton  && fPid2==kParticleIDPionPlus    )
		  || (fPid1==kParticleIDPionPlus    && fPid2==kParticleIDAntiProton  ) ){ enable=true; fMinvCut_m0=1.115683; fMinvCut_dm=0.01; }
	}
	if (!enable){ 	// disable flag if enabled in loop but this is not a pair that has a decay in it... 
		fDoMinvCut  = false;
		fDoMinvCutb = false;
	} else {
		//cout<<"CalcRm::Book .. Minv Cut Enabled."<<endl;
		fMinvCut_mlower			= fMinvCut_m0 - fMinvCut_dm;
		fMinvCut_mupper			= fMinvCut_m0 + fMinvCut_dm;
		cout<<"CalcRm::Book .. Pairs with inv Mass of "<<fMinvCut_m0<<" GeV +/- "<<1000.*fMinvCut_dm<<" MeV will be removed."<<endl;
		if (fDoMinvCutb){
			cout<<"CalcRm::Book .. Pairs with inv Mass of "<<fMinvCutb_m0<<" GeV +/- "<<1000.*fMinvCut_dm<<" MeV will ALSO be removed."<<endl;
			fMinvCutb_mlower	= fMinvCutb_m0 - fMinvCut_dm;
			fMinvCutb_mupper	= fMinvCutb_m0 + fMinvCut_dm;
		}
	} 
	if (fDoMinvLLCut){
		cout<<"CalcRm::Book .. Minv Lower Limit Cut Enabled. Pairs with inv Mass less <=4 MeV above minimum will be rejected."<<endl;
	} 
	//---- end Minv cut setup...
	//
	NCombinations2	= 0;
	for (int iev=0;iev<NMIX;iev++){
	for (int jev=iev+1;jev<NMIX;jev++){			// as defined, jev cannot equal iev... 
		++NCombinations2;
	}
	}
	//cout<<"CalcRm::Book .. NMIX, NCombinations2 = "<<NMIX<<", "<<NCombinations2<<endl;
	//
	//
	//---- If mixing using combinations of events (instead of brute-force looping), set that up
// 	int ai2[2]	= {0,1};
// 	vector<int>	v2(ai2,ai2+2);
// 	//for (int izv=0;izv<ZVTXNB;izv++){
// 	//	NCombinations2_Seen[izv] = 0;	// total number of equivalent mixed events seen over all events incremented to this class
// 	//}
// 	//---- calculate the number of unique k-combinations for the given value of NMIX...
// 	for (int kmix=2;kmix<=NMIXMAX;kmix++){
// 		int kcomb	= 2;
// 		if (kmix>=kcomb){
// 			//cout<<kcomb<<" of "<<kmix<<" \t ";
// 			if (kmix==NMIX){
// 				int thisval	= factorial(kmix)/factorial(kcomb)/factorial(kmix-kcomb);
// 				if (kcomb==2)NCombinations2	= thisval;
// 				//cout<<thisval<<" \t ";
// 			}
// 		} else {
// 			//cout<<"X \t ";
// 		}
// 		//cout<<endl;
// 	}
// 	//
// 	//cout<<"CalcRm::Book .. NMIX="<<NMIX<<" \t NCombinations2="<<NCombinations2
// 	//	<<" \t DenomStatsFactor="<<((double)NCombinations2)/NMIX<<endl;
// 	//
// 	//---- find the event IDs (modulo NMIX) for all of the the unique 2-combinations of NMIX events
// 	int icomb	= 0;
// 	for (int i=0;i<NCOMBMAX;i++){
// 		if (i==0){ for (int j=0;j<2;j++){ Combinations2[j][icomb]=v2[j]; } ++icomb; }
// 		if (NextCombination(v2,2,NMIX)){
// 			for (int j=0;j<2;j++){ Combinations2[j][icomb]=v2[j]; } ++icomb;
// 		} else { break; }
// 	}
// 	if (icomb!=NCombinations2){
// 		cout<<"CalcR4M::UpdateBinningParameters .. Combinations indexing problem... NCOMBMAX="<<NCOMBMAX
// 			<<" .. NComb2: "<<NCombinations2<<" "<<icomb<<endl; 
// 		exit(0);
// 	}
	//
	//------------------------------------------
	//
	hzvtx		= new TH1D("hzvtx","hzvtx",ZVTXNB,ZVTXL,ZVTXU);
	axiszvtx	= hzvtx->GetXaxis();
	//
	//---- variable width binning for the dq axes...
	const Int_t nbins	= DQNB;
	Double_t xmin		= DQL;
	Double_t xmax		= DQU;
	Double_t logxmin	= TMath::Log10(xmin);
	Double_t logxmax	= TMath::Log10(xmax);
	Double_t binwidth	= (logxmax-logxmin)/nbins;
	Double_t xbins[nbins+1];
	xbins[0] = xmin;
	for (Int_t i=1;i<=nbins;i++) {
		xbins[i] = TMath::Power(10,logxmin+i*binwidth);
	}
	int YNB1_rho1	= YNB1;		// can be integer multiples of these parameters!
	int YNB2_rho1	= YNB2;		// can be integer multiples of these parameters!
	int PHINB_rho1	= PHINB;	// can be integer multiples of these parameters!
	int PTNB1_rho1	= PTNB1;	// this one is calculated based on PTL and PTU!!!
	int PTNB2_rho1	= PTNB2;	// this one is calculated based on PTL and PTU!!!
	//
	//---- start booking...
	for (int izv=0;izv<ZVTXNB+1;izv++){			// note one additional histogram here
		//
		mix_nevt[izv]	= 0;
		//
		hmult[izv]		= new TH2D( Form("hmult%d"  ,izv), Form("hmult%d"  ,izv), MAXMULT+1,-0.5,((double)MAXMULT)+0.5,MAXMULT+1,-0.5,((double)MAXMULT)+0.5);
		//hmultc[izv]	= new TH2D( Form("hmultc%d" ,izv), Form("hmultc%d" ,izv), MAXMULT+1,-0.5,((double)MAXMULT)+0.5,MAXMULT+1,-0.5,((double)MAXMULT)+0.5);
		//
		hrho1_1[izv]	= new TH3D(Form("hrho1_1%d",izv),Form("hrho1_1%d",izv),YNB1_rho1,YL1,YU1,PHINB_rho1,PHIL,PHIU,PTNB1_rho1,PTL1,PTU1);
		hrho1_2[izv]	= new TH3D(Form("hrho1_2%d",izv),Form("hrho1_2%d",izv),YNB2_rho1,YL2,YU2,PHINB_rho1,PHIL,PHIU,PTNB2_rho1,PTL2,PTU2);
		hrho1_1[izv]	->Sumw2();
		hrho1_2[izv]	->Sumw2();
		//
		hy1_[izv]		= new TH1D(Form("hy1_%d" ,izv),Form("hy1_%d" ,izv),YNB1_rho1,YL1,YU1);
		hy2_[izv]		= new TH1D(Form("hy2_%d" ,izv),Form("hy2_%d" ,izv),YNB2_rho1,YL2,YU2);
		hyc1_[izv]		= new TH1D(Form("hyc1_%d",izv),Form("hyc1_%d",izv),YNB1_rho1,YL1,YU1);
		hyc2_[izv]		= new TH1D(Form("hyc2_%d",izv),Form("hyc2_%d",izv),YNB2_rho1,YL2,YU2);
		//
		for (int ir2=0;ir2<NR2TYPES;ir2++){
			if (ir2==0){						//----- (y1,y2) ------
				hrho2_S[ir2][izv]		= new TH2D( Form("hrho2_S%d%d",ir2,izv), Form("hrho2_S%d%d",ir2,izv),YNB1,YL1,YU1,YNB2,YL2,YU2);
				hrho2_M[ir2][izv]		= new TH2D( Form("hrho2_M%d%d",ir2,izv), Form("hrho2_M%d%d",ir2,izv),YNB1,YL1,YU1,YNB2,YL2,YU2);
				hC2[ir2][izv]			= new TH2D( Form("hC2%d%d"    ,ir2,izv), Form("hC2%d%d"    ,ir2,izv),YNB1,YL1,YU1,YNB2,YL2,YU2);
				hR2[ir2][izv]			= new TH2D( Form("hR2%d%d"    ,ir2,izv), Form("hR2%d%d"    ,ir2,izv),YNB1,YL1,YU1,YNB2,YL2,YU2);
				//if(izv==0)hm2D[ir2]		= new TH2D( Form("hm2D%d"     ,ir2    ), Form("hm2D%d"     ,ir2    ),YNB1,YL1,YU1,YNB2,YL2,YU2);
//dptM				hrho2_S_dptP[ir2][izv]	= new TH2D( Form("hrho2_S_dptP%d%d",ir2,izv), Form("hrho2_S_dptP%d%d",ir2,izv),YNB1,YL1,YU1,YNB2,YL2,YU2);
//dptM				hrho2_M_dptP[ir2][izv]	= new TH2D( Form("hrho2_M_dptP%d%d",ir2,izv), Form("hrho2_M_dptP%d%d",ir2,izv),YNB1,YL1,YU1,YNB2,YL2,YU2);
//dptM				hrho2_S_dptN[ir2][izv]	= new TH2D( Form("hrho2_S_dptN%d%d",ir2,izv), Form("hrho2_S_dptN%d%d",ir2,izv),YNB1,YL1,YU1,YNB2,YL2,YU2);
//dptM				hrho2_M_dptN[ir2][izv]	= new TH2D( Form("hrho2_M_dptN%d%d",ir2,izv), Form("hrho2_M_dptN%d%d",ir2,izv),YNB1,YL1,YU1,YNB2,YL2,YU2);
			} else if (ir2==1){					//----- (dy,dphi) ------
				hrho2_S[ir2][izv]	= new TH2D( Form("hrho2_S%d%d",ir2,izv), Form("hrho2_S%d%d",ir2,izv),DYNB,DYL,DYU,DPHINB,DPHIL,DPHIU);
				hrho2_M[ir2][izv]	= new TH2D( Form("hrho2_M%d%d",ir2,izv), Form("hrho2_M%d%d",ir2,izv),DYNB,DYL,DYU,DPHINB,DPHIL,DPHIU);
				hC2[ir2][izv]		= new TH2D( Form("hC2%d%d"    ,ir2,izv), Form("hC2%d%d"    ,ir2,izv),DYNB,DYL,DYU,DPHINB,DPHIL,DPHIU);
				hR2[ir2][izv]		= new TH2D( Form("hR2%d%d"    ,ir2,izv), Form("hR2%d%d"    ,ir2,izv),DYNB,DYL,DYU,DPHINB,DPHIL,DPHIU);
				//---- README_Crossing: crossing-corrected copies, (dy,dphi) only. Filled in Calculate().
				hrho2C_S[izv]		= new TH2D( Form("hrho2C_S%d",izv), Form("hrho2C_S%d",izv),DYNB,DYL,DYU,DPHINB,DPHIL,DPHIU);
				hC2C[izv]			= new TH2D( Form("hC2C%d"    ,izv), Form("hC2C%d"    ,izv),DYNB,DYL,DYU,DPHINB,DPHIL,DPHIU);
				hR2C[izv]			= new TH2D( Form("hR2C%d"    ,izv), Form("hR2C%d"    ,izv),DYNB,DYL,DYU,DPHINB,DPHIL,DPHIU);
				if (izv==0) hMempty	= new TH2D( "hMempty", "hMempty",DYNB,DYL,DYU,DPHINB,DPHIL,DPHIU);
				//if(izv==0)hm2D[ir2]	= new TH2D( Form("hm2D%d"     ,ir2    ), Form("hm2D%d"     ,ir2    ),DYNB,DYL,DYU,DPHINB,DPHIL,DPHIU);
//dptM				hrho2_S_dptP[ir2][izv]	= new TH2D( Form("hrho2_S_dptP%d%d",ir2,izv), Form("hrho2_S_dptP%d%d",ir2,izv),DYNB,DYL,DYU,DPHINB,DPHIL,DPHIU);
//dptM				hrho2_M_dptP[ir2][izv]	= new TH2D( Form("hrho2_M_dptP%d%d",ir2,izv), Form("hrho2_M_dptP%d%d",ir2,izv),DYNB,DYL,DYU,DPHINB,DPHIL,DPHIU);
//dptM				hrho2_S_dptN[ir2][izv]	= new TH2D( Form("hrho2_S_dptN%d%d",ir2,izv), Form("hrho2_S_dptN%d%d",ir2,izv),DYNB,DYL,DYU,DPHINB,DPHIL,DPHIU);
//dptM				hrho2_M_dptN[ir2][izv]	= new TH2D( Form("hrho2_M_dptN%d%d",ir2,izv), Form("hrho2_M_dptN%d%d",ir2,izv),DYNB,DYL,DYU,DPHINB,DPHIL,DPHIU);
			} else if (ir2==2){					//----- (dy,dq) ------
				hrho2_S[ir2][izv]	= new TH2D( Form("hrho2_S%d%d",ir2,izv), Form("hrho2_S%d%d",ir2,izv),DYNB,DYL,DYU,nbins,xbins);
				hrho2_M[ir2][izv]	= new TH2D( Form("hrho2_M%d%d",ir2,izv), Form("hrho2_M%d%d",ir2,izv),DYNB,DYL,DYU,nbins,xbins);
				hC2[ir2][izv]		= new TH2D( Form("hC2%d%d"    ,ir2,izv), Form("hC2%d%d"    ,ir2,izv),DYNB,DYL,DYU,nbins,xbins);
				hR2[ir2][izv]		= new TH2D( Form("hR2%d%d"    ,ir2,izv), Form("hR2%d%d"    ,ir2,izv),DYNB,DYL,DYU,nbins,xbins);
				//if(izv==0)hm2D[ir2]	= new TH2D( Form("hm2D%d"     ,ir2    ), Form("hm2D%d"     ,ir2    ),DYNB,DYL,DYU,nbins,xbins);
//dptM				hrho2_S_dptP[ir2][izv]	= new TH2D( Form("hrho2_S_dptP%d%d",ir2,izv), Form("hrho2_S_dptP%d%d",ir2,izv),DYNB,DYL,DYU,nbins,xbins);
//dptM				hrho2_M_dptP[ir2][izv]	= new TH2D( Form("hrho2_M_dptP%d%d",ir2,izv), Form("hrho2_M_dptP%d%d",ir2,izv),DYNB,DYL,DYU,nbins,xbins);
//dptM				hrho2_S_dptN[ir2][izv]	= new TH2D( Form("hrho2_S_dptN%d%d",ir2,izv), Form("hrho2_S_dptN%d%d",ir2,izv),DYNB,DYL,DYU,nbins,xbins);
//dptM				hrho2_M_dptN[ir2][izv]	= new TH2D( Form("hrho2_M_dptN%d%d",ir2,izv), Form("hrho2_M_dptN%d%d",ir2,izv),DYNB,DYL,DYU,nbins,xbins);
			} else {
				cout<<"problem!"<<endl; exit(0); 
			}
			hrho2_S[ir2][izv]->Sumw2();
			hrho2_M[ir2][izv]->Sumw2();
//dptM			hrho2_S_dptP[ir2][izv]->Sumw2();
//dptM			hrho2_M_dptP[ir2][izv]->Sumw2();
//dptM			hrho2_S_dptN[ir2][izv]->Sumw2();
//dptM			hrho2_M_dptN[ir2][izv]->Sumw2();
			//
			//if (izv==0){
			//	for (int ibin=1;ibin<=hrho2_S[ir2][izv]->GetNbinsX();ibin++){
			//		for (int jbin=1;jbin<=hrho2_S[ir2][izv]->GetNbinsY();jbin++){
			//			hm2D[ir2]->SetBinContent(ibin,jbin,-1.0);
			//			hm2D[ir2]->SetBinError(ibin,jbin, 0.0);
			//		}
			//	}
			//}
			//
		}
		//
		hR2yydy[izv]	= new TH1D( Form("hR2yydy%d" ,izv), Form("hR2yydy%d" ,izv), DYNB,DYL,DYU);
		hR2yydye[izv]	= new TH1D( Form("hR2yydye%d",izv), Form("hR2yydye%d",izv), DYNB,DYL,DYU);
		hR2yydyN[izv]	= new TH1D( Form("hR2yydyN%d",izv), Form("hR2yydyN%d",izv), DYNB,DYL,DYU);
		hR2dy[izv]		= new TH1D( Form("hR2dy%d"   ,izv), Form("hR2dy%d"   ,izv), DYNB,DYL,DYU);
		hR2dye[izv]		= new TH1D( Form("hR2dye%d"  ,izv), Form("hR2dye%d"  ,izv), DYNB,DYL,DYU);
		hR2dyN[izv]		= new TH1D( Form("hR2dyN%d"  ,izv), Form("hR2dyN%d"  ,izv), DYNB,DYL,DYU);
		hR2dphi[izv]	= new TH1D( Form("hR2dphi%d" ,izv), Form("hR2dphi%d" ,izv), DPHINB,DPHIL,DPHIU);
		hR2dphie[izv]	= new TH1D( Form("hR2dphie%d",izv), Form("hR2dphie%d",izv), DPHINB,DPHIL,DPHIU);
		hR2dphiN[izv]	= new TH1D( Form("hR2dphiN%d",izv), Form("hR2dphiN%d",izv), DPHINB,DPHIL,DPHIU);
		hR2dq[izv]		= new TH1D( Form("hR2dq%d"   ,izv), Form("hR2dq%d"   ,izv), nbins,xbins);
		hR2dqe[izv]		= new TH1D( Form("hR2dqe%d"  ,izv), Form("hR2dqe%d"  ,izv), nbins,xbins);
		hR2dqN[izv]		= new TH1D( Form("hR2dqN%d"  ,izv), Form("hR2dqN%d"  ,izv), nbins,xbins);
		//
// 		hR2dyNS[izv]	= new TH1D( Form("hR2dyNS%d"   ,izv), Form("hR2dyNS%d"   ,izv), DYNB,DYL,DYU);
// 		hR2dyNSe[izv]	= new TH1D( Form("hR2dyNSe%d"  ,izv), Form("hR2dyNSe%d"  ,izv), DYNB,DYL,DYU);
// 		hR2dyNSN[izv]	= new TH1D( Form("hR2dyNSN%d"  ,izv), Form("hR2dyNSN%d"  ,izv), DYNB,DYL,DYU);
// 		hR2dphiNS[izv]	= new TH1D( Form("hR2dphiNS%d" ,izv), Form("hR2dphiNS%d" ,izv), DPHINB,DPHIL,DPHIU);
// 		hR2dphiNSe[izv]	= new TH1D( Form("hR2dphiNSe%d",izv), Form("hR2dphiNSe%d",izv), DPHINB,DPHIL,DPHIU);
// 		hR2dphiNSN[izv]	= new TH1D( Form("hR2dphiNSN%d",izv), Form("hR2dphiNSN%d",izv), DPHINB,DPHIL,DPHIU);
// 		hR2dqNS[izv]	= new TH1D( Form("hR2dqNS%d"   ,izv), Form("hR2dqNS%d"   ,izv), nbins,xbins);
// 		hR2dqNSe[izv]	= new TH1D( Form("hR2dqNSe%d"  ,izv), Form("hR2dqNSe%d"  ,izv), nbins,xbins);
// 		hR2dqNSN[izv]	= new TH1D( Form("hR2dqNSN%d"  ,izv), Form("hR2dqNSN%d"  ,izv), nbins,xbins);
		//
		double	xl		= fMass1 + fMass2;
		hMinv_S[izv]	= new TH1D( Form("hMinv_S%d",izv), Form("hMinv_S%d",izv), 2500,xl,xl+5. );	// 2 MeV Bin Width
		hMinv_M[izv]	= new TH1D( Form("hMinv_M%d",izv), Form("hMinv_M%d",izv), 2500,xl,xl+5. );	// 2 MeV Bin Width
		hMinv[izv]		= new TH1D( Form("hMinv%d"  ,izv), Form("hMinv%d"  ,izv), 2500,xl,xl+5. );	// 2 MeV Bin Width
		//
		//---- README_CQ step 2: 0-1.2 GeV (was 0-0.3; the pp-sized femtoscopic bump reaches ~0.4 GeV), 1 MeV bins
		hQsib[izv]		= new TH1D( Form("hQsib%d",izv), Form("hQsib%d",izv), 1200,0.,1.2 );
		hQmix[izv]		= new TH1D( Form("hQmix%d",izv), Form("hQmix%d",izv), 1200,0.,1.2 );
		hCQ[izv]		= new TH1D( Form("hCQ%d"  ,izv), Form("hCQ%d"  ,izv), 1200,0.,1.2 );
		hQsib[izv]		->Sumw2();
		hQmix[izv]		->Sumw2();
		//---- README_CQ step 3: (Qinv, kT), 2 MeV in Q, STAR's kT bins (arXiv:1004.0925); kT outside
		//---- 0.15-0.60 goes to the kT under/overflow, so summing all kT bins gives back hQsib (rebinned x2)
		{
			const int	nkt			= 4;
			double		ktbins[nkt+1]	= {0.15,0.25,0.35,0.45,0.60};
			hQsibKT[izv]	= new TH2D( Form("hQsibKT%d",izv), Form("hQsibKT%d",izv), 600,0.,1.2, nkt,ktbins );
			hQmixKT[izv]	= new TH2D( Form("hQmixKT%d",izv), Form("hQmixKT%d",izv), 600,0.,1.2, nkt,ktbins );
			hCQKT[izv]		= new TH2D( Form("hCQKT%d"  ,izv), Form("hCQKT%d"  ,izv), 600,0.,1.2, nkt,ktbins );
			hQsibKT[izv]	->Sumw2();
			hQmixKT[izv]	->Sumw2();
		}
//		hMinvdq_S[izv]	= new TH2D( Form("hMinvdq_S%d",izv), Form("hMinvdq_S%d",izv), nbins,xbins, 150,xl,xl+0.3);
//		hMinvdq_M[izv]	= new TH2D( Form("hMinvdq_M%d",izv), Form("hMinvdq_M%d",izv), nbins,xbins, 150,xl,xl+0.3);
//		hMinvdq[izv]	= new TH2D( Form("hMinvdq%d"  ,izv), Form("hMinvdq%d"  ,izv), nbins,xbins, 150,xl,xl+0.3);
		//
	}	// end izv
	//
	//---- README sec 16.8: spike-extent zoom. 0.001 in deta x 0.1deg in dphi, (0,0) at a bin CENTRE,
	//---- deliberately far finer than any binning used to set the old box (whose "edges" were just
	//---- bin widths, i.e. upper limits). Raw counts, summed over Zvtx, filled after PairInfo.
	hzoom_S	= new TH2D("hzoom_S","hzoom_S;#Delta#eta;#Delta#phi (deg)",101,-0.0505,0.0505,121,-6.05,6.05);
	hzoom_M	= new TH2D("hzoom_M","hzoom_M;#Delta#eta;#Delta#phi (deg)",101,-0.0505,0.0505,121,-6.05,6.05);
	//---- fine invariant mass near threshold, drawn next to C(Q): 500 x 1 MeV from m1+m2-5 MeV
	{
		double xth	= fMass1 + fMass2;
		hMinvF_S	= new TH1D("hMinvF_S","hMinvF_S;M_{inv} (GeV);pairs",500,xth-0.005,xth+0.495);
		hMinvF_M	= new TH1D("hMinvF_M","hMinvF_M;M_{inv} (GeV);pairs",500,xth-0.005,xth+0.495);
	}
}

//------------------------------------------------------------
void CalcRm::Increment(double zv, double field,
						int n1, double eff_1[], double Pvec_1[][3],
                        int n2, double eff_2[], double Pvec_2[][3]){
	//
	//cout<<"Increment "<<zv<<endl;
//	TimerIncrement->Start(kTRUE);		// start and zero...
	//
//NONLL	if (!(n1>0&&n2>0)) return;				// must have at least 1 of each particle in this event
//NONLL	if (!fDistinguishable && n1<2) return;	// if identical particles in pair (pid1=pid2), require there's at least 2 of these particles in this event
	//
	//---- protection and Zvtx assignment...
	if (n1>MAXMULT){ cout<<"CalcRm::Increment .. mult n1 too large! Increase MAXMULT... "<<n1<<" "<<MAXMULT<<endl; exit(0); }
	if (n2>MAXMULT){ cout<<"CalcRm::Increment .. mult n2 too large! Increase MAXMULT... "<<n2<<" "<<MAXMULT<<endl; exit(0); }
	if (n1>NTRKMAX){ cout<<"CalcRm::Increment .. mult n1 too large! Increase NTRKMAX... "<<n1<<" "<<NTRKMAX<<endl; exit(0); }
	if (n2>NTRKMAX){ cout<<"CalcRm::Increment .. mult n2 too large! Increase NTRKMAX... "<<n2<<" "<<NTRKMAX<<endl; exit(0); }
	if (zv<ZVTXL || zv>=ZVTXU) return;
	hzvtx		->Fill(zv);
	int izv		= axiszvtx->FindBin(zv) - 1; 
	//
	fField		= field;
	//
	//---- fill mixing buffers...
	bool fillHists				 = false;
	int n1k,n2k; n1k=n2k=0;
//NONLL	if (n1>0 && n2>0) {				// require all events to have both particles!! 
        int evtid		 		 = mix_nevt[izv] % NMIX;
 		mix_mult1[evtid][izv]	 = n1;
		mix_mult2[evtid][izv]	 = n2;
		mix_field[evtid][izv]	 = field;
		mix_zvtx[evtid][izv]	 = zv;
		mix_run[evtid][izv]		 = fCurrentRun;			// sec 18.20
		mix_evt[evtid][izv]		 = fCurrentEvt;
 		if (evtid==NMIX-1){
 			fillHists			 = true;
 		}
		//cout<<"evtid = "<<evtid<<" "<<izv<<" "<<mix_nevt[izv]<<endl;
		for (int i=0;i<n1;i++) {
			double eff	= eff_1[i];
			double y	= Pvec_1[i][0];
			double phi	= Pvec_1[i][1]*raddeg;		// degrees!
			double pt	= Pvec_1[i][2];
			if (phi <    0.) phi +=  360.;
			if (phi >= 360.) phi -=  360.;
			mix_part1[0][i][evtid][izv]	= y;
			mix_part1[1][i][evtid][izv]	= phi;
			mix_part1[2][i][evtid][izv]	= pt;
			mix_part1[3][i][evtid][izv]	= eff;
			//cout<<"part1 \t"<<y<<" "<<pt<<" \t "<<YL1<<" "<<YU1<<" "<<PTL1<<" "<<PTU1<<" "<<endl;
			if (y>=YL1&&y<YU1&&pt>=PTL1&&pt<PTU1){
				++n1k;
			} else {
				cout<<"PART1 FAILED IN CLASS .. "<<y<<" "<<phi<<" "<<pt<<" "<<endl;			
				cout<<"\t \t"<<std::setprecision(10)<<pt<<" \t "<<PTL1<<" "<<PTU1<<endl;		
			}
		}
		for (int j=0;j<n2;j++) {
			double eff	= eff_2[j];
			double y	= Pvec_2[j][0];
			double phi	= Pvec_2[j][1]*raddeg;		// degrees!
			double pt	= Pvec_2[j][2];
			if (phi <    0.) phi +=  360.;
			if (phi >= 360.) phi -=  360.;
			mix_part2[0][j][evtid][izv]	= y;
			mix_part2[1][j][evtid][izv]	= phi;
			mix_part2[2][j][evtid][izv]	= pt;
			mix_part2[3][j][evtid][izv]	= eff;
			//cout<<"part2 \t"<<y<<" "<<pt<<" \t "<<YL2<<" "<<YU2<<" "<<PTL2<<" "<<PTU2<<" "<<endl;
			if (y>=YL2&&y<YU2&&pt>=PTL2&&pt<PTU2){
				++n2k;
			} else {
				cout<<"PART2 FAILED IN CLASS .. "<<y<<" "<<phi<<" "<<pt<<" "<<endl;			
				cout<<"\t \t"<<std::setprecision(10)<<pt<<" \t "<<PTL2<<" "<<PTU2<<endl;		
			}
		}
		mix_nevt[izv]			+= 1;
		//if (izv==16)cout<<"checking if mix .. "<<nevt<<" \t "<<mix_nevt[izv]<<"  "<<evtid<<"  "<<mix_mult[evtid][izv]<<endl;
//NONLL 	}
 	//
	if ((n1k!=n1)||(n2k!=n2)){
		cout<<"CalcRm::Increment -- class cuts issue! "<<n1<<" "<<n1k<<" \t "<<n2<<" "<<n2k<<endl;
	}
	//
	if (fillHists){
		//
		//int nfillnum	= 0;
		//int nfillden	= 0;
		//
		//---- fill SIBLING 2Ds...			
		//		Better to fill numerator for all NMIX events only once all NMIX events are in hand... 
//cout<<"fillHists! iev="<<-1<<" zv="<<zv<<" izv="<<izv<<endl;
		for (int iev=0;iev<NMIX;iev++){
			//++nfillnum;
			//
			//cout<<izv<<" "<<mix_mult1[iev][izv]<<endl;
			// if (mix_mult1[iev][izv]<=5||mix_mult2[iev][izv]<=5){
			// 	cout<<"FILLHISTS .. "<<mix_mult1[iev][izv]<<" "<<mix_mult2[iev][izv]<<" \t "<<n1<<" "<<n2<<" \t "
			// 		<<iev<<" "<<izv<<endl;
			// }
			//
//			double zvtx	= mix_zvtx[iev][izv];
//			int izv		= axiszvtx->FindBin(zvtx) - 1; 
//cout<<"fillHists! iev="<<iev<<" zvtx="<<zvtx<<" izv="<<izv<<endl;
			hmult[izv]->Fill(mix_mult1[iev][izv],mix_mult2[iev][izv]);
			//
			//double multfactor = 1.; 
			//int n1 = mix_mult1[iev][izv];
			//int n2 = mix_mult2[iev][izv];
			//if(!fDistinguishable){ multfactor=((double)n1)*(n1-1.)/n1/n1; }
			//
			double multc1=0.,multc2=0.;
			for (int i=0;i<mix_mult1[iev][izv];i++){	// particle1
				double y	= mix_part1[0][i][iev][izv];
				double phi	= mix_part1[1][i][iev][izv];
				double pt	= mix_part1[2][i][iev][izv];
				double eff	= mix_part1[3][i][iev][izv];
				hy1_[izv]	->Fill(y,1.    );
				hyc1_[izv]	->Fill(y,1./eff);
				hrho1_1[izv]->Fill(y,phi,pt,1./eff);
				multc1		+=1./eff;
			}
			for (int j=0;j<mix_mult2[iev][izv];j++){	// particle2
				double y	= mix_part2[0][j][iev][izv];
				double phi	= mix_part2[1][j][iev][izv];
				double pt	= mix_part2[2][j][iev][izv];
				double eff	= mix_part2[3][j][iev][izv];
				hy2_[izv]	->Fill(y,1.    );
				hyc2_[izv]	->Fill(y,1./eff);
				hrho1_2[izv]->Fill(y,phi,pt,1./eff);
				multc2		+=1./eff;
			}
			//hmultc[izv]->Fill(multc1,multc2);
			//
			for (int i=0;i<mix_mult1[iev][izv];i++){		// event A	iev
				for (int j=0;j<mix_mult2[iev][izv];j++){	// event A	iev
					//
					if (!fDistinguishable && (i==j)) continue;
					//
					//----- NUM simpler filling (by dpt)
//					double	ty1,ty2,tphi1,tphi2,tpt1,tpt2,dty,dtpt,dtphi;
//					ty1		= mix_part1[0][i][iev][izv];	// event A
//					tphi1	= mix_part1[1][i][iev][izv];	// event A
//					tpt1	= mix_part1[2][i][iev][izv];	// event A
//					ty2		= mix_part2[0][j][iev][izv];	// event A
//					tphi2	= mix_part2[1][j][iev][izv];	// event A
//					tpt2	= mix_part2[2][j][iev][izv];	// event A
//					dtpt	=  tpt1 -  tpt2;
//					dty		=   ty1 -   ty2;
//					dtphi	= tphi1 - tphi2;
//					if (dtphi   <  DPHIL) dtphi +=  360.;
//					if (dtphi   >= DPHIU) dtphi -=  360.;
					//
					double eff1		= mix_part1[3][i][iev][izv];	// event A
					double eff2		= mix_part2[3][j][iev][izv];	// event A
					double weight	= 1.;
					if (eff1*eff2>0.){
						weight		= 1./eff1/eff2;
					} else {
						cout<<"numerator zero efficiency!!!"<<endl;
					}
					//
					//cout<<dtpt<<" "<<ty1<<" "<<ty2<<" "<<dty<<" "<<dtphi<<endl;
//					if (dtpt>=0.){
//						hrho2_S_dptP[0][izv]	->Fill(ty1,ty2  ,weight);
//						hrho2_S_dptP[1][izv]	->Fill(dty,dtphi,weight);
//						//hrho2_M_dptP[2][izv]	->Fill(dty,dtq  ,weight);	// dQ not available in simple fill...
//					} else {
//						hrho2_S_dptN[0][izv]	->Fill(ty1,ty2  ,weight);
//						hrho2_S_dptN[1][izv]	->Fill(dty,dtphi,weight);
//						//hrho2_M_dptN[2][izv]	->Fill(dty,dtq  ,weight);	// dQ not available in simple fill...
//					}
					//
					//----- via pairinfo..
					double y1,y2,dy,dphi,dpt,dq,Qinv,Minv,Minvr,kT; bool NearSide;
					bool keep = PairInfo(i,iev,j,iev,izv,
										 y1,y2,dy,dphi,dpt,dq,Qinv,Minv,NearSide,kT);
					if (!keep) continue;
					//
					hrho2_S[0][izv]	->Fill(y1,y2  ,weight);
					hrho2_S[1][izv]	->Fill(dy,dphi,weight);
					hzoom_S			->Fill(dy,dphi,weight);	// sec 16.8
					hrho2_S[2][izv]	->Fill(y1-y2,dq,weight);	// README_Crossing decision 11: (dy,dq) gets the UNflipped dy (dy above is pt-ordered, for (dy,dphi) only)
					hMinv_S[izv]	->Fill(Minv   ,weight);
					hMinvF_S		->Fill(Minv   ,weight);
					hQsib[izv]		->Fill(dq     ,weight);	// dq is the STAR-style qInv (see PairInfo); the local "Qinv" is q^2's own byproduct and is negative-definite for identical species
					hQsibKT[izv]	->Fill(dq,kT  ,weight);	// README_CQ step 3
					//hMinvdq_S[izv]->Fill(dq,Minv,weight);
					//cout<<izv<<" "<<dq<<" "<<Minv<<" "<<weight<<" "<<Minv-dq<<endl;
					//
					//++npair_all_S;
					//if (fabs(dy)  <0.1 ) ++npair_dylow1_S;
					//if (fabs(dy)  <0.3 ) ++npair_dylow2_S;
					//if (fabs(dy)  <0.5 ) ++npair_dylow3_S;
					//if (fabs(dq)  <0.15) ++npair_qlow_S;
					//if (Minv      <1.5 ) ++npair_mlow_S;
					//
				}
			}
		}
		//
		//---- fill MIXED 2Ds...
//		for (int icomb=0;icomb<NCombinations2;icomb++){
//			int iev	= Combinations2[0][icomb];		// index of event A from range [0,...,NMIX-1]
//			int jev	= Combinations2[1][icomb];		// index of event B from range [0,...,NMIX-1]
		//---- brute force loop over all unique 2-combinations...
		for (int iev=0;iev<NMIX;iev++){
		for (int jev=iev+1;jev<NMIX;jev++){			// as defined, jev cannot equal iev... 
			//++nfillden;
			//
			if (mix_field[iev][izv]*mix_field[jev][izv] < 0.){
				cout<<"Field Flip! .. "<<mix_field[iev][izv]<<" "<<mix_field[jev][izv]<<" "
					<<iev<<" "<<jev
					<<endl;
				cout<<"Exiting! You must analyze different fields separately!"<<endl;
				exit(0);
			}
			//++NCombinations2_Seen[izv];
			//for (int j=0;j<2;j++){ cout<<Combinations2[j][icomb]<<" "; } cout<<endl;		
			//---- sec 18.23: skip whole mixed EVENT pairs from neighboring trigger frames -- copies of one
			//---- silicon seed can land in adjacent TFs (sec 18.22), which the same-TF XTFclean cleaner
			//---- cannot see (cluster keys are not comparable across events). Bookkept per zvtx bin so
			//---- each bin's rho2(M) is normalized by the event pairs it actually used (see Calculate()).
			++nComb_possible[izv];
			if (fExcludeAdjTF>0 && mix_run[iev][izv]==mix_run[jev][izv]){
				int devt = abs(mix_evt[iev][izv]-mix_evt[jev][izv]);
				if (devt==1 || (fExcludeAdjTF==2 && devt==0)) continue;
			}
			++nComb_used[izv];
			//
			//double multfactor=1.; 
			//int n1 = mix_mult1[iev][izv];
			//int n2 = mix_mult2[jev][izv];
			//multfactor=((double)n1)*n2/n1/n2;	
			//double weight	= 1./multfactor;
			//
			for (int i=0;i<mix_mult1[iev][izv];i++){		// event A	iev
				for (int j=0;j<mix_mult2[jev][izv];j++){	// event B	jev
					//
					//-------------------------------------------------
					// DENOM simpler filling (by dpt)
//					double	ty1,ty2,tphi1,tphi2,tpt1,tpt2,dty,dtpt,dtphi;
//					ty1		= mix_part1[0][i][iev][izv];	// event A
//					tphi1	= mix_part1[1][i][iev][izv];	// event A
//					tpt1	= mix_part1[2][i][iev][izv];	// event A
//					ty2		= mix_part2[0][j][jev][izv];	// event B
//					tphi2	= mix_part2[1][j][jev][izv];	// event B
//					tpt2	= mix_part2[2][j][jev][izv];	// event B
//					dtpt	=  tpt1 -  tpt2;
//					dty		=   ty1 -   ty2;
//					dtphi	= tphi1 - tphi2;
//					if (dtphi   <  DPHIL) dtphi +=  360.;
//					if (dtphi   >= DPHIU) dtphi -=  360.;
					//
					double eff1		= mix_part1[3][i][iev][izv];	// event A
					double eff2		= mix_part2[3][j][jev][izv];	// event B
					double weight	= 1.;
					if (eff1*eff2>0.){
						weight		= 1./eff1/eff2;
					} else {
						cout<<"denominator zero efficiency!!!"<<endl;
					}
//					if (dtpt>=0.){
//						hrho2_M_dptP[0][izv]	->Fill(ty1,ty2  ,weight);
//						hrho2_M_dptP[1][izv]	->Fill(dty,dtphi,weight);
//						//hrho2_M_dptP[2][izv]	->Fill(dty,dtq  ,weight);	// dQ not avail in simple fill!
//					} else {
//						hrho2_M_dptN[0][izv]	->Fill(ty1,ty2  ,weight);
//						hrho2_M_dptN[1][izv]	->Fill(dty,dtphi,weight);
//						//hrho2_M_dptN[2][izv]	->Fill(dty,dtq  ,weight);	// dQ not avail in simple fill!
//					}
					//-------------------------------------------------
					//
					double y1,y2,dy,dphi,dpt,dq,Qinv,Minv,kT; bool NearSide;
					bool keep = PairInfo(i,iev,j,jev,izv,
										 y1,y2,dy,dphi,dpt,dq,Qinv,Minv,NearSide,kT);
					if (!keep) continue;
					//
					hrho2_M[0][izv]		->Fill(y1,y2  ,weight);
					hrho2_M[1][izv]		->Fill(dy,dphi,weight);
					hzoom_M				->Fill(dy,dphi,weight);	// sec 16.8
					//---- sec 18.20: same-TF / neighboring-TF mixed-pair counters (cheap per-production
					//---- monitor of the trigger-frame structure; see README sec 18.20-18.23)
					{
						++nMixedPairs_total;
						bool sameTF	= (mix_run[iev][izv]==mix_run[jev][izv] && mix_evt[iev][izv]==mix_evt[jev][izv]);
						bool neighborTF	= (!sameTF && mix_run[iev][izv]==mix_run[jev][izv]
						                 && abs(mix_evt[iev][izv]-mix_evt[jev][izv])<=1);
						if (sameTF) ++nMixedPairs_sameTF;
						if (neighborTF) ++nMixedPairs_neighborTF;
					}
					hrho2_M[2][izv]		->Fill(y1-y2,dq,weight);	// README_Crossing decision 11: UNflipped dy, same as sibling
					hMinv_M[izv]		->Fill(Minv   ,weight);
					hMinvF_M			->Fill(Minv   ,weight);
					hQmix[izv]			->Fill(dq     ,weight);	// dq is the STAR-style qInv (see PairInfo); the local "Qinv" is q^2's own byproduct and is negative-definite for identical species
					hQmixKT[izv]		->Fill(dq,kT  ,weight);	// README_CQ step 3, same kT as sibling
					//
					//hMinvdq_M[izv]	->Fill(dq,Minv,weight);
					//cout<<izv<<" "<<dq<<" "<<Minv<<" "<<weight<<" "<<Minv-dq<<endl;
					//
					//++npair_all_M;
					//if (fabs(dy)  <0.1 ) ++npair_dylow1_M;
					//if (fabs(dy)  <0.3 ) ++npair_dylow2_M;
					//if (fabs(dy)  <0.5 ) ++npair_dylow3_M;
					//if (fabs(dq)  <0.15) ++npair_qlow_M;
					//if (Minv      <1.5 ) ++npair_mlow_M;
					//
				}
			}
			//
		}	// end jev
		}	// end iev
//		}	// end loop over 2-combinations...
		//
		//cout<<"NFILL: "<<nfillnum<<" "<<nfillden<<" "<<(double)nfillden/nfillnum<<endl;
		//
	}	// end fillHists
	//
//	TimerIncrement	-> Stop();
//	fTimeIncrement	+= TimerIncrement->CpuTime();
	//
}

//------------------------------------------------------------
bool CalcRm::PairInfo(int i, int iev, int j, int jev, int izv,
					  double& y1,double& y2,double& dy,double& dphi,
					  double& dpt,double& dq,double& Qinv,double& Minv,bool& NearSide,double& kT){
	//
	// default is to accept all pairs in the acceptance
	// in this code, pairs can be rejected due to a dQ cut, or an Minv cut (or both)
	bool keep	= true;
	//
	double	phi1,phi2,pt1,pt2;
	y1		= mix_part1[0][i][iev][izv];
	phi1	= mix_part1[1][i][iev][izv];
	pt1		= mix_part1[2][i][iev][izv];
	y2		= mix_part2[0][j][jev][izv];
	phi2	= mix_part2[1][j][jev][izv];
	pt2		= mix_part2[2][j][jev][izv];
	//
	dy		= y1 - y2;
	dpt		= pt1 - pt2;
	dphi	= phi1 - phi2;				// deg!!
	if (fDoCrossing){
		//---- 2026-09-25 (README_Crossing sec 8, user): dphi sign convention chosen so the crossing
		//---- damage lands just BELOW dphi=0 (dirty side dphi<0 at field>0, as in STAR); 0..180 is clean.
		//---- These are the complements of the first conditions used (which put it at dphi>0).
		if (fLikeSign){
			if (fChg1>0 && dpt<0.0){		// LS Pos
				if (fFlipDy) dy = -dy;
				dphi	= -dphi;
			}
			if (fChg1<0 && dpt>0.0){		// LS Neg
				if (fFlipDy) dy = -dy;
				dphi	= -dphi;
			}	
		} else if (!fLikeSign){	
			if (fChg1>0 && fChg2<0){		// ULS +- (a neutral leg is never flipped, in either order)
				if (fFlipDy) dy = -dy;
				dphi = -dphi;
			}
		} 
	}
//STAR	if (fField >    0.0) dphi  = -dphi;
	if (dphi   <  DPHIL) dphi +=  360.;
	if (dphi   >= DPHIU) dphi -=  360.;
	//
	//NearSide	= false;
	//if (fabs(dy)<0.25 && fabs(dphi)<60.) NearSide = true;
	//
	//------------------
	// https://www.star.bnl.gov/cgi-bin/protected/cvsweb.cgi/StRoot/StHbtMaker/Infrastructure/StHbtPair.cc?rev=1.29
	//
	dq = 0;
	Minv = 0;
	kT = 0;
	if (fDoDQ || fDoMinvCut){
		double px1 		= pt1*cos(phi1/raddeg);		// class phi angles are in degrees
		double py1 		= pt1*sin(phi1/raddeg);		// class phi angles are in degrees
		double mt1		= sqrt(pt1*pt1 + fMass1*fMass1);
		double pz1		= mt1*sinh(y1);
		double pE1		= mt1*cosh(y1);
		double px2 		= pt2*cos(phi2/raddeg);		// class phi angles are in degrees
		double py2 		= pt2*sin(phi2/raddeg);		// class phi angles are in degrees
		double mt2		= sqrt(pt2*pt2 + fMass2*fMass2);
		double pz2		= mt2*sinh(y2);
		double pE2		= mt2*cosh(y2);
		double Px 		= px1+px2;
		double Py 		= py1+py2;
		double Pz 		= pz1+pz2;
		double PE 		= pE1+pE2;
		double dPx 		= px1-px2;
		double dPy 		= py1-py2;
		double dPz 		= pz1-pz2;
		double dPE 		= pE1-pE2;
		double Ptrans	= Px*Px + Py*Py;
			   kT		= 0.5*std::sqrt(Ptrans);	// README_CQ: STAR kT = |pT1+pT2|/2, from the unflipped momenta
		double Mtrans	= PE*PE - Pz*Pz;
		double Pinv 	= std::sqrt(Mtrans - Ptrans);
		//	   Mtrans	= std::sqrt(Mtrans);
		//	   Ptrans	= std::sqrt(Ptrans);
		double Qinv2 	= dPE*dPE - dPx*dPx - dPy*dPy - dPz*dPz;
		double Q 		= (fMass1*fMass1 - fMass2*fMass2)/Pinv;
			   dq		= std::sqrt(Q*Q - Qinv2);
		       Qinv		= sqrt(Qinv2);
		double Minv2	= PE*PE - Px*Px - Py*Py - Pz*Pz;
		if (Minv2<0){ cout<<"neg minv!"<<endl; exit(0); }
		Minv	= sqrt(Minv2);
		//
		// 
	}
	//
	if (fDoMinvCut){
		if ( Minv >= fMinvCut_mlower 
		  && Minv <  fMinvCut_mupper ){
			keep	= false;		// This Pair has Minv consistent with Ks, Phi, L, or Lbar so reject it (if requested).. 
		}
		if (fDoMinvCutb){			// K+K- can come from TWO different decay parents!  Ks and D0, this is the 2nd one... 
			if ( Minv >= fMinvCutb_mlower 
			  && Minv <  fMinvCutb_mupper ){
				keep	= false;	// This Pair has Minv consistent with D0 so reject it (if requested).. 
			}
		}
	}
	if (fDoMinvLLCut){
		if ( Minv < fMass1 + fMass2 + 0.004 ){		// lowest 2 bins (lowest 4 MeV of Minv distribution)
			keep	= false;	// This Pair has Minv near lower limit (this pair is a split track?) so reject it (if requested).. 
		}
	}
	if (fDoQcut){
		if ( dq < fQcut ){
			keep	= false; 	// This Pair has dQ below the cut value so reject it (if requested)..
		}
	}
	return keep;
	//
}

//------------------------------------------------------------
//------------------------------------------------------------
void CalcRm::Calculate(){
	//
	//---- README_Crossing sec 8: the crossing correction symmetrizes in dphi ONLY, which is consistent
	//---- only with a dphi-only pt-ordering in Increment. A dy flip would need a different correction.
	if (fDoCrossing && fFlipDy){
		cout<<"CalcRm::Calculate -- fFlipDy=true is not supported by the crossing correction. Exiting."<<endl;
		exit(0);
	}
	//
//	TimerCalculate->Start(kTRUE);	// start and zero...
	//
// 	cout<<"npair_all_S		= "<<npair_all_S	<<" \t "<<(double)npair_all_S	/npair_all_S	<<endl;
// 	cout<<"npair_dylow1_S	= "<<npair_dylow1_S	<<" \t "<<(double)npair_dylow1_S/npair_all_S	<<endl;
// 	cout<<"npair_dylow2_S	= "<<npair_dylow2_S	<<" \t "<<(double)npair_dylow2_S/npair_all_S	<<endl;
// 	cout<<"npair_dylow3_S	= "<<npair_dylow3_S	<<" \t "<<(double)npair_dylow3_S/npair_all_S	<<endl;
// 	cout<<"npair_qlow_S		= "<<npair_qlow_S	<<" \t "<<(double)npair_qlow_S	/npair_all_S	<<endl;
// 	cout<<"npair_mlow_S		= "<<npair_mlow_S	<<" \t "<<(double)npair_mlow_S	/npair_all_S	<<endl;
// 	cout<<"npair_all_M		= "<<npair_all_M	<<" \t "<<(double)npair_all_M	/npair_all_M	<<endl;
// 	cout<<"npair_dylow1_M	= "<<npair_dylow1_M	<<" \t "<<(double)npair_dylow1_M/npair_all_M	<<endl;
// 	cout<<"npair_dylow2_M	= "<<npair_dylow2_M	<<" \t "<<(double)npair_dylow2_M/npair_all_M	<<endl;
// 	cout<<"npair_dylow3_M	= "<<npair_dylow3_M	<<" \t "<<(double)npair_dylow3_M/npair_all_M	<<endl;
// 	cout<<"npair_qlow_M		= "<<npair_qlow_M	<<" \t "<<(double)npair_qlow_M	/npair_all_M	<<endl;
// 	cout<<"npair_mlow_M		= "<<npair_mlow_M	<<" \t "<<(double)npair_mlow_M	/npair_all_M	<<endl;
	//
	double denomfactor	= ((double)NCombinations2)/NMIX;
	//cout<<"CalcRm::Calculate -- Nentries in denominator relative to numerator = "<<denomfactor<<endl;
	//
	//cout<<"CalcRm::Calculate -- Normalizing... "<<endl;
	//---- fine Minv (all Zvtx summed): bring the MIXED histogram to the same per-sibling-event footing
	//---- as hMinv_M below (divide by mixed event pairs used per sibling event, summed over Zvtx), so
	//---- the shape-matching factor shown on the C(Q) page is only the residual sib/mix level (~1),
	//---- not the NCombinations2/NMIX combinatorics. The sibling fine histogram stays raw counts.
	{
		double nevtTot	= 0., ncombTot = 0.;
		for (int izv=0;izv<ZVTXNB;izv++){
			double ne	= hmult[izv]->GetEntries();
			nevtTot		+= ne;
			ncombTot	+= (fExcludeAdjTF>0) ? (double)nComb_used[izv] : ne*denomfactor;
		}
		if (ncombTot>0.) hMinvF_M->Scale(nevtTot/ncombTot);
	}
	//---- empty-bin protection (README_Crossing sec 8, 2026-09-25): a slice that is skipped below is
	//---- left out of the Zvtx average; a bin is valid in a slice iff its rho2(M) > 0.
	bool  sliceOK[NZVTXMAX];	for (int k=0;k<NZVTXMAX;k++) sliceOK[k] = false;
	for (int k=0;k<NZVTXMAX;k++) fDenomZ[k] = 0.;
	TH2D* hMaskC[NZVTXMAX];		for (int k=0;k<NZVTXMAX;k++) hMaskC[k]  = 0;	// validity of the crossing-corrected maps
	for (int izv=0;izv<ZVTXNB;izv++){
		//
		double nevt		= hmult[izv]->GetEntries();
		if (nevt<2.){ continue; }
		TH1D *hmult1	= (TH1D*)hmult[izv]->ProjectionX("hmult1");
		TH1D *hmult2	= (TH1D*)hmult[izv]->ProjectionY("hmult2");
			  hmult1	->Scale(1./nevt);
			  hmult2	->Scale(1./nevt);
		double f1_1		= hmult1->GetMean();		// <n1>
		double f1_2		= hmult2->GetMean();		// <n2>
		delete hmult1; hmult1=0;
		delete hmult2; hmult2=0;
		//---- sec 18.23: per-zvtx-bin mixed normalization. nevt = (full buffers)*NMIX exactly (hmult is
		//---- filled once per buffered event at fillHists), so with no exclusion nComb_used/nevt ==
		//---- NCombinations2/NMIX; the old expression is kept verbatim when the exclusion is off.
		double denomfactor_izv	= denomfactor;
		if (fExcludeAdjTF>0){
			if (nComb_used[izv]<=0){ continue; }
			denomfactor_izv	= ((double)nComb_used[izv])/nevt;
			cout<<"CalcRm::Calculate -- sec 18.23 izv="<<izv<<"  mixed event pairs used/possible="<<nComb_used[izv]<<"/"<<nComb_possible[izv]
				<<Form(" (%.2f%%)",100.*nComb_used[izv]/std::max(1L,nComb_possible[izv]))<<"  denomfactor="<<denomfactor_izv<<" (NMIX-only: "<<denomfactor<<")"<<endl;
		}
		cout<<"CalcRm::Calculate -- izv="<<izv<<"  nev="<<nevt<<"  NpairSibling="<<hrho2_S[0][izv]->GetEntries()<<"  NpairMixed="<<hrho2_M[0][izv]->GetEntries()<<endl;
		sliceOK[izv]	= true;
		fDenomZ[izv]	= denomfactor_izv;		// README_Finalize: saved per zvtx bin for combining chunks
		//
		//cout<<"---------------------------------------"<<endl;
		//cout<<izv<<" \t <n1>="<<hmult[izv]->ProjectionX()->GetMean()
		//		<<" <n2>="<<hmult[izv]->ProjectionY()->GetMean()
		//		<<" \t "<<endl;
		//
		hy1_[izv]	->Scale(1./nevt);
		hy2_[izv]	->Scale(1./nevt);
		hyc1_[izv]	->Scale(1./nevt);
		hyc2_[izv]	->Scale(1./nevt);
		hrho1_1[izv]->Scale(1./nevt);
		hrho1_2[izv]->Scale(1./nevt);
		//
		double ainte_1,ainte_2;
		double aint_1	= hy1_[izv]->IntegralAndError(1,0,ainte_1,"");
		double aint_2	= hy2_[izv]->IntegralAndError(1,0,ainte_2,"");
		double binte_1,binte_2;
		double bint_1	= hrho1_1[izv]->IntegralAndError(1,0,1,0,1,0,binte_1,"");
		double bint_2	= hrho1_2[izv]->IntegralAndError(1,0,1,0,1,0,binte_2,"");
		double icheck1	= int(10000.*fabs(f1_1-aint_1));
		double icheck2	= int(10000.*fabs(f1_2-aint_2));
		if ( icheck1>1 || icheck2>1 ){				// <0.5/10000 accuracy or better
			cout<<"INTTEST FAIL .. izv="<<izv
				<<" \t part1: "<<f1_1<<" "<<aint_1<<" "<<bint_1<<" chk="<<icheck1
				<<" \t part2: "<<f1_2<<" "<<aint_2<<" "<<bint_2<<" chk="<<icheck2
				<<endl;	
		}
		//
		for (int ir2=0;ir2<NR2TYPES;ir2++){
			double npair_S		= hrho2_S[0][izv]->GetEntries();
			double npair_M		= hrho2_M[0][izv]->GetEntries();
			hrho2_S[ir2][izv]	->Scale(1.0/nevt            );
			hrho2_M[ir2][izv]	->Scale(1.0/nevt/denomfactor_izv);	// sec 18.23
			//
//dptM			hrho2_S_dptP[ir2][izv]	->Scale(1.0/nevt            );	// ir2=2 (dy,dq) isn't actually filled at the moment...
//dptM			hrho2_M_dptP[ir2][izv]	->Scale(1.0/nevt/denomfactor);
//dptM			hrho2_S_dptN[ir2][izv]	->Scale(1.0/nevt            );
//dptM			hrho2_M_dptN[ir2][izv]	->Scale(1.0/nevt/denomfactor);
			//
			//BuildR2( hrho2_S[ir2][izv], hrho2_M[ir2][izv], hR2[ir2][izv] );	// this method defined in READER CLASS
			//
			hC2[ir2][izv]		->Reset();
			hR2[ir2][izv]		->Reset();
			for (int ibin=1;ibin<=hrho2_S[ir2][izv]->GetNbinsX();ibin++){
				for (int jbin=1;jbin<=hrho2_S[ir2][izv]->GetNbinsY();jbin++){
					double valn		= hrho2_S[ir2][izv]->GetBinContent(ibin,jbin);	// rho2
					double valne	= hrho2_S[ir2][izv]->GetBinError(  ibin,jbin);	// rho2e
					double vald		= hrho2_M[ir2][izv]->GetBinContent(ibin,jbin);	// rho1rho1
					double valde	= hrho2_M[ir2][izv]->GetBinError(  ibin,jbin);	// rho1rho1e
					double valC2,valC2e,valR2,valR2e;
					//---- valid iff the denominator is filled. rho2(S)=0 over a filled denominator is a valid
					//---- zero (R2=-1, C2=-rho2(M)); its error uses one pair's worth of density (1/nevt) for
					//---- rho2(S). Approximate -- these per-run errors are replaced by subgroup errors in Finalize.
					if (vald>0. && valn<=0.){
						double valne1	= 1./nevt;
						valC2	 = -vald;
						valC2e	 = sqrt(valne1*valne1 + valde*valde);
						valR2	 = -1.;
						valR2e	 = valne1/vald;
					} else if (valn>0.&&vald>0.){
						valC2	 = valn - vald;
						valC2e	 = sqrt(valne*valne + valde*valde);
						valR2	 = valn/vald;
						valR2e	 = valR2*sqrt(pow(valde/vald,2)+pow(valne/valn,2));	// uncertainty of ratio
						valR2	-= 1.;	// note must get errors BEFORE SUBTRACTING 1!! (this 1 has no error!)
//						if (fPid1==2&&fPid2==2&&ir2==1&&izv==16&&ibin==20&&jbin==7){
//							cout<<"R2CLASS: "<<valn<<"+-"<<valne<<"\t"<<vald<<"+-"<<valde<<"\t"<<valR2<<"+-"<<valR2e<<endl;
//						}
						//if (izv==16 && (ibin==1&&jbin==1)){cout<<ir2<<" "<<valR2<<" "<<valR2e<<endl;}
						//
					//} else if (valn>0){
					//	cout<<"rho2_S nonzero & rho2_M zero "<<ibin<<" "<<jbin<<" "<<vald<<" "<<valn<<endl;	
					} else {
						valC2	=  0.;	//?
						valC2e	=  0.;	//?
						valR2	= -1.;
						valR2e	=  0.;
					}
					hC2[ir2][izv]	->SetBinContent(ibin,jbin,valC2 );
					hC2[ir2][izv]	->SetBinError(  ibin,jbin,valC2e);						 
					hR2[ir2][izv]	->SetBinContent(ibin,jbin,valR2 );
					hR2[ir2][izv]	->SetBinError(  ibin,jbin,valR2e);						 
					//
					if (TMath::IsNaN(valC2)){cout<<"c2 nan"<<endl;}
					if (TMath::IsNaN(valC2e)){cout<<"c2e nan"<<endl;}
					if (TMath::IsNaN(valR2)){cout<<"r2 nan"<<endl;}
					if (TMath::IsNaN(valR2e)){cout<<"r2e nan"<<endl;}
				}
			}	
			//
		}	// end r2 types
		//
		hMinv_S[izv]	->ResetBit(TH1::kIsAverage);
		hMinv_M[izv]	->ResetBit(TH1::kIsAverage);
		hMinv_S[izv]	->Scale(1.0);
		hMinv_M[izv]	->Scale(1.0/denomfactor_izv);	// sec 18.23
		double entvals	= hMinv_S[izv]->GetEntries();
		double entvalm	= hMinv_M[izv]->GetEntries();
		for (int ibin=1;ibin<=hMinv_S[izv]->GetNbinsX();ibin++){	// copy hMinv_S into hMinv
			double val	= hMinv_S[izv]->GetBinContent(ibin);
			double vale	= hMinv_S[izv]->GetBinError(  ibin);
			hMinv[izv]	->SetBinContent(ibin,val );
			hMinv[izv]	->SetBinError(  ibin,vale);
		}
		hMinv[izv]	->SetEntries(entvals);		// this is Minv_S - Minv_M
		hMinv[izv]	->Add(hMinv_M[izv],-1.0);
		//
		//---- Femtoscopic C(Q) = hQsib/hQmix in this Zvtx bin. hQsib is left as raw (per-NMIX-events)
		//---- counts, matching how hMinv_S is left un-normalized; hQmix must be brought down to the
		//---- same "per-event" footing before the ratio is meaningful, since (per Increment()) each
		//---- fillHists cycle deposits NMIX sibling-events'-worth of pairs into hQsib but
		//---- NCombinations2=NMIX*(NMIX-1)/2 mixed-event-pairs'-worth into hQmix. denomfactor
		//---- (=NCombinations2/NMIX=(NMIX-1)/2) removes exactly that excess -- it depends only on NMIX.
		hQmix[izv]	->Scale(1.0/denomfactor_izv);	// sec 18.23
		hQmixKT[izv]->Scale(1.0/denomfactor_izv);	// README_CQ step 3, same normalization
		//---- same bin-by-bin ratio for the 1D and the (Qinv,kT) versions; GetNcells covers every bin
		//---- including flow (the kT under/overflow are real kT ranges, <0.15 and >0.60)
		TH1* qn[2]	= {hQsib[izv], hQsibKT[izv]};
		TH1* qd[2]	= {hQmix[izv], hQmixKT[izv]};
		TH1* qc[2]	= {hCQ[izv],   hCQKT[izv]  };
		for (int k=0;k<2;k++){
			qc[k]	->Reset();
			for (int ibin=0;ibin<qn[k]->GetNcells();ibin++){
				double valn		= qn[k]->GetBinContent(ibin);
				double valne	= qn[k]->GetBinError(  ibin);
				double vald		= qd[k]->GetBinContent(ibin);
				double valde	= qd[k]->GetBinError(  ibin);
				double valCQ,valCQe;
				if (vald>0. && valn<=0.){		// valid zero; one-pair error (approximate, replaced in Finalize)
					valCQ	= 0.;
					valCQe	= 1./vald;
				} else if (valn>0. && vald>0.){
					valCQ	= valn/vald;
					valCQe	= valCQ*sqrt(pow(valne/valn,2)+pow(valde/vald,2));
				} else {
					valCQ	= 0.;
					valCQe	= 0.;
				}
				qc[k]->SetBinContent(ibin,valCQ );
				qc[k]->SetBinError(  ibin,valCQe);
			}
		}
		//
		//		hMinvdq_S[izv]	->Scale(1.0);
		//		hMinvdq_M[izv]	->Scale(1.0/denomfactor);
		//if (izv==15){
		//	cout<<hMinvdq_S[izv]->GetNbinsX()<<" "<<hMinvdq_S[izv]->GetNbinsY()<<" "
		//		<<hMinvdq_S[izv]->GetEntries()<<" "
		//		<<hMinvdq_M[izv]->GetNbinsX()<<" "<<hMinvdq_M[izv]->GetNbinsY()<<" "
		//		<<hMinvdq_M[izv]->GetEntries()<<" "
		//		<<endl;
		//}
		//		for (int ibin=1;ibin<=hMinvdq_S[izv]->GetNbinsX();ibin++){
		//			for (int jbin=1;jbin<=hMinvdq_S[izv]->GetNbinsY();jbin++){
		//				double val		= hMinvdq_S[izv]->GetBinContent(ibin,jbin);
		//				double vale		= hMinvdq_S[izv]->GetBinError(  ibin,jbin);
		//if (izv==15 && val!=0){
		//double tmpdq= hMinvdq_S[izv]->GetXaxis()->GetBinCenter(ibin);
		//double tmpm	= hMinvdq_S[izv]->GetYaxis()->GetBinCenter(jbin);
		//cout<<tmpdq<<" "<<tmpm<<" \t "<<val<<" "<<vale<<" \t "
		//	<<    hMinvdq_M[izv]->GetBinContent(ibin,jbin)<<" \t "
		//	<<val-hMinvdq_M[izv]->GetBinContent(ibin,jbin)
		//	<<endl;
		//}
		//				hMinvdq[izv]	->SetBinContent(ibin,jbin,val);
		//				hMinvdq[izv]	->SetBinError(  ibin,jbin,vale);
		//			}
		//		}
		//		hMinvdq[izv]->Add(hMinvdq_M[izv],-1.0);
		//
		//cout<<"CalcRm::Calculate -- Calculating R2..."<<endl;
		//
		//---- get and apply Baseline
		if (fDoBaseline){
			double baseR2 		= GetR2Baseline(hmult[izv]);
			//cout<<"CalcRc::Calculate -- Baseline: "<<baseR2<<endl;
			for (int ir2=0;ir2<NR2TYPES;ir2++){		
				for (int ibx=1;ibx<=hR2[ir2][izv]->GetNbinsX();ibx++){
					for (int iby=1;iby<=hR2[ir2][izv]->GetNbinsY();iby++){
						double oval2 = hR2[ir2][izv]->GetBinContent(ibx,iby);	// shift itself assumed known perfectly for now
						if (hrho2_M[ir2][izv]->GetBinContent(ibx,iby)>0.){		// valid bin (README_Crossing sec 8)
							hR2[ir2][izv]->SetBinContent(ibx,iby,oval2-baseR2);		// and keep same errors
						} else {
							hR2[ir2][izv]->SetBinContent(ibx,iby,0.);				// no entries here!
							hR2[ir2][izv]->SetBinError(ibx,iby,0.);					// clamp error to zero for safety! 
						}
					}	// end iby
				}	// end ibx
			}	// end r2 type
		}	// end doBaseline
		//
		//---- README_Crossing step 6: crossing correction, (dy,dphi) only, per Zvtx bin, applied ONCE
		//---- to the final uncorrected rho2(S), C2, R2 (after the baseline shift). rho2(M) is never
		//---- corrected. With "nocross" the C histograms stay empty.
		if (fDoCrossing){
			TH2D* src[3]	= { hrho2_S[1][izv], hC2[1][izv], hR2[1][izv] };
			TH2D* dst[3]	= { hrho2C_S[izv],   hC2C[izv],   hR2C[izv]   };
			for (int k=0;k<3;k++){
				dst[k]->Reset();
				for (int ib=0;ib<(src[k]->GetNbinsX()+2)*(src[k]->GetNbinsY()+2);ib++){
					dst[k]->SetBinContent(ib,src[k]->GetBinContent(ib));
					dst[k]->SetBinError(  ib,src[k]->GetBinError(ib));
				}
				dst[k]->SetEntries(src[k]->GetEntries());
				CrossingCorrect(dst[k],fDirtySide,hrho2_M[1][izv]);
			}
			//---- validity of the corrected maps in this slice: correct a 0/1 mask of rho2(M)>0 with itself
			hMaskC[izv]	= (TH2D*)hrho2_M[1][izv]->Clone(Form("hMaskC%d",izv));
			hMaskC[izv]->Reset();
			for (int ib=0;ib<hMaskC[izv]->GetNcells();ib++){		// all cells, incl. under/overflow (never touched by CrossingCorrect)
				hMaskC[izv]->SetBinContent(ib, hrho2_M[1][izv]->GetBinContent(ib)>0. ? 1. : 0.);
			}
			CrossingCorrect(hMaskC[izv],fDirtySide,hMaskC[izv]);
		}

		//double dAvg_dy[2*YNB-1];	memset( dAvg_dy, 0, (2*YNB-1)*sizeof(int) );
		//double dAvg_N[2*YNB-1];	memset( dAvg_N,    0, (2*YNB-1)*sizeof(int) );
		//double dAvg_S[2*YNB-1];	memset( dAvg_S,    0, (2*YNB-1)*sizeof(int) );
		//cout<<"CalcRm::Calculate -- Averaging in dy slices..."<<endl;

		//for (int ibx=1;ibx<=hrho2_S[0][izv]->GetXaxis()->GetNbins();ibx++){		// x-bin is y1
		//	for (int iby=1;iby<=hrho2_S[0][izv]->GetYaxis()->GetNbins();iby++){		// y-bin is y2
		//		double dy,ay,val,vale;
		//		dy		= hrho2_S[0][izv]->GetXaxis()->GetBinCenter(ibx) 
		//				- hrho2_S[0][izv]->GetYaxis()->GetBinCenter(iby);
		//		//ay	=(hrho2_S[0][izv]->GetXaxis()->GetBinCenter(ibx)
		//		//		+ hrho2_S[0][izv]->GetYaxis()->GetBinCenter(iby))/2.;
		//		val		= hrho2_S[0][izv]->GetBinContent(ibx,iby);
		//		vale	= hrho2_S[0][izv]->GetBinError(ibx,iby);
		//		hrho2_Sdy[izv]	->Fill(dy, val);
		//		hrho2_Sdye[izv]	->Fill(dy, vale*vale);
		//		hrho2_SdyN[izv]	->Fill(dy, 1.0);
		//	}	// iby
		//}	// ibx
		//for (int ibi=1;ibi<=hrho2_Sdy[izv]->GetNbinsX();ibi++){
		//	double v,e,n;
		//	v	=       hrho2_Sdy[izv]	->GetBinContent(ibi);
		//	e	= sqrt( hrho2_Sdye[izv]	->GetBinContent(ibi));
		//	n	=       hrho2_SdyN[izv]	->GetBinContent(ibi);
		//	if (n>0){
		//		hrho2_Sdy[izv]->SetBinContent(ibi,v/n);					
		//		hrho2_Sdy[izv]->SetBinError(ibi,e/n);
		//	}
		//}
		//delete[] hrho2_Sdye; //hrho2_Sdye[izv]=0;
		//delete[] hrho2_SdyN; //hrho2_SdyN[izv]=0;
	
		//---- Project R2(y1,y2) onto R2(dy)...
		//		we now get R2(dy) by projecting R2(dy,dphi), NOT from R2(y1,y2) as before
		//		...because R2(dy,dphi) includes the complete correction for track crossing
		//
		//for (int ibx=1;ibx<=hR2[0][izv]->GetXaxis()->GetNbins();ibx++){		// x-bin is y1
		//	for (int iby=1;iby<=hR2[0][izv]->GetYaxis()->GetNbins();iby++){		// y-bin is y2
		//		double dy,ay,val,vale;
		//		dy		= hR2[0][izv]->GetXaxis()->GetBinCenter(ibx) 
		//				- hR2[0][izv]->GetYaxis()->GetBinCenter(iby);
		//		//ay	=(hR2[0][izv]->GetXaxis()->GetBinCenter(ibx)
		//		//		+ hR2[0][izv]->GetYaxis()->GetBinCenter(iby))/2.;
		//		val		= hR2[0][izv]->GetBinContent(ibx,iby);
		//		vale	= hR2[0][izv]->GetBinError(ibx,iby);
		//		hR2dy[izv]	->Fill(dy, val);
		//		hR2dye[izv]	->Fill(dy, vale*vale);
		//		hR2dyN[izv]	->Fill(dy, 1.0);
		//	}	// iby
		//}	// ibx
		//for (int ibi=1;ibi<=hR2dy[izv]->GetNbinsX();ibi++){
		//	double v,e,n;
		//	v	=       hR2dy[izv]	->GetBinContent(ibi);
		//	e	= sqrt( hR2dye[izv]	->GetBinContent(ibi));
		//	n	=       hR2dyN[izv]	->GetBinContent(ibi);
		//	if (n>0){
		//		hR2dy[izv]->SetBinContent(ibi,v/n);					
		//		hR2dy[izv]->SetBinError(ibi,e/n);
		//	}
		//}
		//delete[] hR2dye; //hR2dye[izv]=0;
		//delete[] hR2dyN; //hR2dyN[izv]=0;
		//
		//---- Project R2(y1,y2) onto R2yy(dy)...
		for (int ibx=1;ibx<=hR2[0][izv]->GetXaxis()->GetNbins();ibx++){			// x-bin is y1
			for (int iby=1;iby<=hR2[0][izv]->GetYaxis()->GetNbins();iby++){		// y-bin is y2
				double y1,y2,dy,ay,val,vale;
				y1		= hR2[0][izv]->GetXaxis()->GetBinCenter(ibx);
				y2		= hR2[0][izv]->GetYaxis()->GetBinCenter(iby);
				dy		= y1 - y2;
				val		= hR2[0][izv]->GetBinContent(ibx,iby);
				vale	= hR2[0][izv]->GetBinError(ibx,iby);
				if (hrho2_M[0][izv]->GetBinContent(ibx,iby)>0.){	// valid bin (README_Crossing sec 8); R2=-1 is valid
					hR2yydy[izv]	->Fill(dy, val);
					hR2yydye[izv]	->Fill(dy, vale*vale);
					hR2yydyN[izv]	->Fill(dy, 1.0);
				}
			}	// iby
		}	// ibx
		for (int ibi=1;ibi<=hR2dy[izv]->GetNbinsX();ibi++){
			double v,e,n;
			v	=       hR2yydy[izv]	->GetBinContent(ibi);
			e	= sqrt( hR2yydye[izv]	->GetBinContent(ibi));
			n	=       hR2yydyN[izv]	->GetBinContent(ibi);
			if (n>0){
				hR2yydy[izv]->SetBinContent(ibi,v/n);					
				hR2yydy[izv]->SetBinError(ibi,e/n);
			}
		}
		//delete[] hR2dye; //hR2dye[izv]=0;
		//delete[] hR2dyN; //hR2dyN[izv]=0;
		//
		//---- Project R2(dy,dphi) onto R2(dy)...
		for (int ibx=1;ibx<=hR2[1][izv]->GetXaxis()->GetNbins();ibx++){			// x-bin is dy
			for (int iby=1;iby<=hR2[1][izv]->GetYaxis()->GetNbins();iby++){		// y-bin is dphi
				double dy,ay,val,vale;
				dy		= hR2[1][izv]->GetXaxis()->GetBinCenter(ibx);
				val		= hR2[1][izv]->GetBinContent(ibx,iby);
				vale	= hR2[1][izv]->GetBinError(ibx,iby);
				if (hrho2_M[1][izv]->GetBinContent(ibx,iby)>0.){	// valid bin (README_Crossing sec 8); R2=-1 is valid
					hR2dy[izv]	->Fill(dy, val);
					hR2dye[izv]	->Fill(dy, vale*vale);
					hR2dyN[izv]	->Fill(dy, 1.0);
				}
			}	// iby
		}	// ibx
		for (int ibi=1;ibi<=hR2dy[izv]->GetNbinsX();ibi++){
			double v,e,n;
			v	=       hR2dy[izv]	->GetBinContent(ibi);
			e	= sqrt( hR2dye[izv]	->GetBinContent(ibi));
			n	=       hR2dyN[izv]	->GetBinContent(ibi);
			if (n>0){
				hR2dy[izv]->SetBinContent(ibi,v/n);					
				hR2dy[izv]->SetBinError(ibi,e/n);
			}
		}
		//delete[] hR2dye; //hR2dye[izv]=0;
		//delete[] hR2dyN; //hR2dyN[izv]=0;
		//
		//---- Project R2(dy,dphi) onto R2(dphi)...
		for (int iby=1;iby<=hR2[1][izv]->GetYaxis()->GetNbins();iby++){			// y-bin is dphi
			for (int ibx=1;ibx<=hR2[1][izv]->GetXaxis()->GetNbins();ibx++){		// x-bin is dy
				double dphi	= hR2[1][izv]->GetYaxis()->GetBinCenter(iby);
				double val	= hR2[1][izv]->GetBinContent(ibx,iby);
				double vale	= hR2[1][izv]->GetBinError(ibx,iby);
				if (hrho2_M[1][izv]->GetBinContent(ibx,iby)>0.){	// valid bin (README_Crossing sec 8); R2=-1 is valid
					hR2dphi[izv]	->Fill(dphi,val);
					hR2dphie[izv]	->Fill(dphi,vale*vale);
					hR2dphiN[izv]	->Fill(dphi,1.0);
				}
			}	// iby
		}	// ibx
		for (int ibi=1;ibi<=hR2dphi[izv]->GetNbinsX();ibi++){
			double v,e,n;
			v	=       hR2dphi[izv]	->GetBinContent(ibi);
			e	= sqrt( hR2dphie[izv]	->GetBinContent(ibi));
			n	=       hR2dphiN[izv]	->GetBinContent(ibi);
			if (n>0){
				hR2dphi[izv]->SetBinContent(ibi,v/n);					
				hR2dphi[izv]->SetBinError(ibi,e/n);
			}
		}
		//delete[] hR2dye; //hR2dye[izv]=0;
		//delete[] hR2dyN; //hR2dyN[izv]=0;
		//
		//---- Project R2(dy,dq) onto R2(dq)...
		for (int iby=1;iby<=hR2[2][izv]->GetYaxis()->GetNbins();iby++){				// y-bin is dq
			for (int ibx=1;ibx<=hR2[2][izv]->GetXaxis()->GetNbins();ibx++){			// x-bin is dy
				double dq	= hR2[2][izv]->GetYaxis()->GetBinCenter(iby);
				double val	= hR2[2][izv]->GetBinContent(ibx,iby);
				double vale	= hR2[2][izv]->GetBinError(ibx,iby);
				if (hrho2_M[2][izv]->GetBinContent(ibx,iby)>0.){	// valid bin (README_Crossing sec 8); R2=-1 is valid
					hR2dq[izv]	->Fill(dq,val);
					hR2dqe[izv]	->Fill(dq,vale*vale);
					hR2dqN[izv]	->Fill(dq,1.0);
				}
			}	// iby
		}	// ibx
		for (int ibi=1;ibi<=hR2dq[izv]->GetNbinsX();ibi++){
			double v,e,n;
			v	=       hR2dq[izv]	->GetBinContent(ibi);
			e	= sqrt( hR2dqe[izv]	->GetBinContent(ibi));
			n	=       hR2dqN[izv]	->GetBinContent(ibi);
			if (n>0){
				hR2dq[izv]->SetBinContent(ibi,v/n);					
				hR2dq[izv]->SetBinError(ibi,e/n);
			}
		}
		//delete[] hR2dye; //hR2dye[izv]=0;
		//delete[] hR2dyN; //hR2dyN[izv]=0;
		//
	}
	//delete[] hm2D; //hm2D=0;

	//---- now get the zvtx-averaged histograms...
	//
	//	Note; you ONLY average histograms that are *averages* (i.e. hR2xxx... )
	//		 histograms that are counts are *summed* here (i.e. hmult, hMinv... ) 
	//
	double znent	= hzvtx->GetEntries();
	TH1D* hzvtxprob	= (TH1D*)hzvtx->Clone("hzvtxprob");
	if (znent>0){ hzvtxprob->Scale(1./znent); } else { hzvtxprob->Reset(); }
	double weightsum = 0;
	for (int izv=0;izv<ZVTXNB;izv++){
		//
		double weight		 = hzvtxprob->GetBinContent(izv+1);
		weightsum			+= weight;
		//cout<<"Zvtx-Avg ... "<<izv<<" "<<weight<<" "<<weightsum<<endl;	// had better equal 1 by the last zvtx bin !!!!
		//
		hmult[ZVTXNB]				->Add(hmult[izv]   ,   1.0);
		hy1_[ZVTXNB]				->Add(hy1_[izv]    ,weight);		// Average
		hy2_[ZVTXNB]				->Add(hy2_[izv]    ,weight);		// Average
		hyc1_[ZVTXNB]				->Add(hyc1_[izv]   ,weight);		// Average
		hyc2_[ZVTXNB]				->Add(hyc2_[izv]   ,weight);		// Average
		hrho1_1[ZVTXNB]				->Add(hrho1_1[izv] ,weight);
		hrho1_2[ZVTXNB]				->Add(hrho1_2[izv] ,weight);
		for (int ir2=0;ir2<NR2TYPES;ir2++){
			//---- rho2(S), rho2(M), C2, R2: averaged over VALID slices only, after this loop
			//
//dptM			hrho2_S_dptP[ir2][ZVTXNB]	->Add(hrho2_S_dptP[ir2][izv] ,weight);	// average...
//dptM			hrho2_M_dptP[ir2][ZVTXNB]	->Add(hrho2_M_dptP[ir2][izv] ,weight);	// average...
//dptM			hrho2_S_dptN[ir2][ZVTXNB]	->Add(hrho2_S_dptN[ir2][izv] ,weight);	// average...
//dptM			hrho2_M_dptN[ir2][ZVTXNB]	->Add(hrho2_M_dptN[ir2][izv] ,weight);	// average...
			//
		}
		//---- crossing-corrected maps, R2 projections, C(Q): averaged over VALID slices only, after this loop
		hMinv_S[ZVTXNB]		->Add(hMinv_S[izv]  ,   1.0);
		hMinv_M[ZVTXNB]		->Add(hMinv_M[izv]  ,   1.0);
		hMinv[ZVTXNB]		->Add(hMinv[izv]    ,   1.0);
		hQsib[ZVTXNB]		->Add(hQsib[izv]    ,   1.0);
		hQmix[ZVTXNB]		->Add(hQmix[izv]    ,   1.0);
		hQsibKT[ZVTXNB]		->Add(hQsibKT[izv]  ,   1.0);
		hQmixKT[ZVTXNB]		->Add(hQmixKT[izv]  ,   1.0);
		//
		//double nev	= hmult[izv]->GetEntries();
		//double np	= hmult[izv]->ProjectionX()->GetMean();
		//double npi	= hmult[izv]->ProjectionY()->GetMean();
		//double npair= hMinv_S[izv]->GetEntries();
		//cout<<izv<<" "<<nev<<" "<<np<<" "<<npi<<" "<<npair<<" "<<npair/nev<<" "<<np*npi<<endl;
		//
	}	// end Zvtx averaging
	//
	//---- CF-type histograms: Zvtx average over valid slices only (README_Crossing sec 8, 2026-09-25).
	//---- An empty denominator is never averaged in; it is counted (hMempty, one log line). The real check
	//---- (squawk, reduce binning) belongs in Finalize at full stats.
	{
		double wz[NZVTXMAX];
		for (int izv=0;izv<ZVTXNB;izv++) wz[izv] = hzvtxprob->GetBinContent(izv+1);
		TH1* sl[NZVTXMAX]; TH1* mk[NZVTXMAX];
		for (int ir2=0;ir2<NR2TYPES;ir2++){
			for (int izv=0;izv<ZVTXNB;izv++) mk[izv] = hrho2_M[ir2][izv];
			for (int izv=0;izv<ZVTXNB;izv++) sl[izv] = hrho2_S[ir2][izv];	ZvtxAverageValid(hrho2_S[ir2][ZVTXNB],sl,mk,wz,sliceOK,ZVTXNB, 0.);
			for (int izv=0;izv<ZVTXNB;izv++) sl[izv] = hC2[ir2][izv];		ZvtxAverageValid(hC2[ir2][ZVTXNB],    sl,mk,wz,sliceOK,ZVTXNB, 0.);
			for (int izv=0;izv<ZVTXNB;izv++) sl[izv] = hR2[ir2][izv];		ZvtxAverageValid(hR2[ir2][ZVTXNB],    sl,mk,wz,sliceOK,ZVTXNB,-1.);
			//---- rho2(M) last: it is its own mask
			for (int izv=0;izv<ZVTXNB;izv++) sl[izv] = hrho2_M[ir2][izv];	ZvtxAverageValid(hrho2_M[ir2][ZVTXNB],sl,mk,wz,sliceOK,ZVTXNB, 0.);
			//---- report empty denominators: (bin,slice) pairs with rho2(M)=0 in slices that were used
			long nMz=0, nSMz=0; int nbinAny=0;
			TH2D* hm	= hrho2_M[ir2][0];
			for (int ix=1;ix<=hm->GetNbinsX();ix++) for (int iy=1;iy<=hm->GetNbinsY();iy++){
				int nz=0;
				for (int izv=0;izv<ZVTXNB;izv++){
					if (!sliceOK[izv] || wz[izv]<=0.) continue;
					if (hrho2_M[ir2][izv]->GetBinContent(ix,iy)>0.) continue;
					++nz; ++nMz;
					if (hrho2_S[ir2][izv]->GetBinContent(ix,iy)>0.) ++nSMz;
				}
				if (nz>0) ++nbinAny;
				if (ir2==1) hMempty->SetBinContent(ix,iy,nz);
			}
			cout<<"CalcRm::Calculate -- empty denominators (pid "<<fPid1<<","<<fPid2<<", ir2="<<ir2<<"): "
				<<nbinAny<<" of "<<hm->GetNbinsX()*hm->GetNbinsY()<<" bins have rho2(M)=0 in >=1 used Zvtx slice ("
				<<nMz<<" bin-slices, left out of the Zvtx average; "<<nSMz<<" of them with rho2(S)>0). Finalize checks this at full stats."<<endl;
		}
		if (fDoCrossing){
			for (int izv=0;izv<ZVTXNB;izv++) mk[izv] = hMaskC[izv] ? (TH1*)hMaskC[izv] : (TH1*)hrho2_M[1][izv];	// unused slices have no mask
			for (int izv=0;izv<ZVTXNB;izv++) sl[izv] = hrho2C_S[izv];	ZvtxAverageValid(hrho2C_S[ZVTXNB],sl,mk,wz,sliceOK,ZVTXNB, 0.);
			for (int izv=0;izv<ZVTXNB;izv++) sl[izv] = hC2C[izv];		ZvtxAverageValid(hC2C[ZVTXNB],    sl,mk,wz,sliceOK,ZVTXNB, 0.);
			for (int izv=0;izv<ZVTXNB;izv++) sl[izv] = hR2C[izv];		ZvtxAverageValid(hR2C[ZVTXNB],    sl,mk,wz,sliceOK,ZVTXNB,-1.);
		}
		for (int izv=0;izv<ZVTXNB;izv++){ delete hMaskC[izv]; hMaskC[izv] = 0; }
		for (int izv=0;izv<ZVTXNB;izv++) sl[izv] = hR2yydy[izv];	ZvtxAverageValid(hR2yydy[ZVTXNB],sl,0,wz,sliceOK,ZVTXNB,0.);
		for (int izv=0;izv<ZVTXNB;izv++) sl[izv] = hR2dy[izv];		ZvtxAverageValid(hR2dy[ZVTXNB],  sl,0,wz,sliceOK,ZVTXNB,0.);
		for (int izv=0;izv<ZVTXNB;izv++) sl[izv] = hR2dphi[izv];	ZvtxAverageValid(hR2dphi[ZVTXNB],sl,0,wz,sliceOK,ZVTXNB,0.);
		for (int izv=0;izv<ZVTXNB;izv++) sl[izv] = hR2dq[izv];		ZvtxAverageValid(hR2dq[ZVTXNB],  sl,0,wz,sliceOK,ZVTXNB,0.);
		for (int izv=0;izv<ZVTXNB;izv++){ sl[izv] = hCQ[izv]; mk[izv] = hQmix[izv]; }
		ZvtxAverageValid(hCQ[ZVTXNB],sl,mk,wz,sliceOK,ZVTXNB,0.);
		for (int izv=0;izv<ZVTXNB;izv++){ sl[izv] = hCQKT[izv]; mk[izv] = hQmixKT[izv]; }
		ZvtxAverageValid(hCQKT[ZVTXNB],sl,mk,wz,sliceOK,ZVTXNB,0.);
	}
	delete hzvtxprob; hzvtxprob = 0;
	//
	if (fDoCrossing && fDirtySide==0){
		cout<<"CalcRm::Calculate -- crossing correction (pid "<<fPid1<<","<<fPid2<<", dirty side 0): not pt-ordered, not corrected (C maps = uncorrected)"<<endl;
	} else if (fDoCrossing){	// README_Crossing step 7: log check that the corrected maps are dphi-symmetric
		cout<<"CalcRm::Calculate -- crossing correction (pid "<<fPid1<<","<<fPid2<<", dirty side "<<fDirtySide
			<<"): Zvtx-avg dphi-asymmetric bins: rho2C="<<CrossingAsymmetry(hrho2C_S[ZVTXNB])
			<<" C2C="<<CrossingAsymmetry(hC2C[ZVTXNB])<<" R2C="<<CrossingAsymmetry(hR2C[ZVTXNB])
			<<"  (uncorrected R2: "<<CrossingAsymmetry(hR2[1][ZVTXNB])<<" of "
			<<hR2C[ZVTXNB]->GetNbinsX()*hR2C[ZVTXNB]->GetNbinsY()<<")"<<endl;
	}
	//
	//double nev	= hmult[ZVTXNB]->GetEntries();
	//double np	= hmult[ZVTXNB]->ProjectionX()->GetMean();
	//double npi	= hmult[ZVTXNB]->ProjectionY()->GetMean();
	//double npair= hMinv_S[ZVTXNB]->GetEntries();
	//cout<<ZVTXNB<<" "<<nev<<" "<<np<<" "<<npi<<" "<<npair<<" "<<npair/nev<<" "<<np*npi<<endl;
	//
//	TimerCalculate->Stop();
//	fTimeCalculate	+= TimerCalculate->CpuTime();
	//
}

//------------------------------------------------------------
// bool CalcRm::NextCombination(vector<int>& v, int k, int N) {
//   // We want to find the index of the least significant element
//   // in v that can be increased.  Let's call that index 'pivot'.
//   int pivot = k - 1;
//   while (pivot >= 0 && v[pivot] == N - k + pivot)
//     --pivot;
// 
//   // pivot will be -1 iff v == {N - k, N - k + 1, ..., N - 1},
//   // in which case, there is no next combination.
//   if (pivot == -1)
//     return false;
// 
//   ++v[pivot];
//   for (int i = pivot + 1; i < k; ++i)
//     v[i] = v[pivot] + i - pivot;
//   return true;
// }

//----------------------------------------------------------------------------
double CalcRm::GetR2Baseline(TH2D *hmult){
	double result 	= -1.;
	if (!hmult){ cout<<"CalcRm::GetR2Baseline -- no hmult... exit"<<endl; exit(0); }
	double nent 	= (double)hmult->GetEntries();
//	if (nent<5){ cout<<"CalcRm::GetR2Baseline -- no entries..."<<endl; return result; }
	TH2D *hmultwork	= (TH2D*)hmult->Clone("hmultwork");
	hmultwork		->Scale(1./nent);
	double sumnum	= 0;
	double sumden1	= 0;
	double sumden2	= 0;
	TAxis* multaxisx	= (TAxis*)hmultwork->GetXaxis();
	TAxis* multaxisy	= (TAxis*)hmultwork->GetYaxis();
	TH1D* hmultworkx	= (TH1D*)hmultwork->ProjectionX();
	double meanmult		= hmultworkx->GetMean();
	if (!fDistinguishable){
		for (int ibx=1;ibx<=multaxisx->GetNbins();ibx++){
			double n1i	 = multaxisx->GetBinCenter(ibx);
			double pi	 = hmultworkx->GetBinContent(ibx);		
			sumnum		+= pi*n1i*(n1i-1.);
			sumden1		+= pi*n1i;
			//cout<<ibx<<" "<<n1i<<" "<<pi<<" "<<sumnum<<" "<<sumden1<<endl;
		}
		if ( sumnum>0.0 && sumden1>0.0 ){
			//result	= sumden1*sumden1/sumnum;			// rho1rho1/rho2
			result		= (sumnum/sumden1/sumden1) - 1.0;	// R2integral
			//cout<<">>--> "<<meanmult<<" "<<sumden1<<" "<<sumnum<<" \t "<<result<<" \t "<<sumnum/meanmult/meanmult<<endl;
		}
	} else if (fDistinguishable){
		for (int ibx=1;ibx<=multaxisx->GetNbins();ibx++){
			for (int iby=1;iby<=multaxisy->GetNbins();iby++){
				double n1i	 = multaxisx->GetBinCenter(ibx);
				double n2i	 = multaxisy->GetBinCenter(iby);
				double pi	 = hmultwork->GetBinContent(ibx,iby);		
				sumnum		+= pi*n1i*n2i;
				sumden1		+= pi*n1i;
				sumden2		+= pi*n2i;
			}
		}
		if (sumnum>0.0 && sumden1>0.0 && sumden2>0.0 ){
			//result	= sumden1*sumden2/sumnum;			// rho1rho1/rho2
			result		= (sumnum/sumden1/sumden2) - 1.0;	// R2integral
		}
	}	// end fDistinguishable...
	//
	delete hmultwork; hmultwork=0;
	return result;
}

//---- Ayeh code...
//invariant mass and dq calculation
//double px_p1, px_p2, py_p1, py_p2, etot_p1, etot_p2, 
//double dpx_lsp, dpy_lsp, dpz_lsp, detot_lsp, dq_lsp, spx_lsp, spy_lsp, spz_lsp, setot_lsp,invm_lsp;
////px1=ptpmix[ievt][ii]*cos(Deg2Rad(phipmix[ievt][ii]-180));//arg cos and sin should be radian
////py1=ptpmix[ievt][ii]*sin(Deg2Rad(phipmix[ievt][ii]-180));
////px2=ptpmix[ievt2][kk]*cos(Deg2Rad(phipmix[ievt2][kk]-180));
////py2=ptpmix[ievt2][kk]*sin(Deg2Rad(phipmix[ievt2][kk]-180));
////etot_p1=sqrt((mass*mass)+(ptpmix[ievt][ii]*ptpmix[ievt][ii] + pzpmix[ievt][ii]*pzpmix[ievt][ii]));
////etot_p2=sqrt((mass*mass)+(ptpmix[ievt2][kk]*ptpmix[ievt2][kk] + pzpmix[ievt2][kk]*pzpmix[ievt2][kk]));
//dpx_lsp=px1-px2;
//dpy_lsp=py1-py2;
//dpz_lsp=pz1-pz2;
//detot_lsp=pE1-pE2;
//spx_lsp=px1+px2;
//spy_lsp=py1+py2;
//spz_lsp=pz1+pz2;
//setot_lsp=pE1+pE2;
//dq_lsp=sqrt( ((dpx_lsp*dpx_lsp)+(dpy_lsp*dpy_lsp)+(dpz_lsp*dpz_lsp))-(detot_lsp*detot_lsp) );
////invm_lsp=sqrt( (setot_lsp*setot_lsp)-((spx_lsp*spx_lsp)+(spy_lsp*spy_lsp)+(spz_lsp*spz_lsp)) );
////---- end Ayeh code...

