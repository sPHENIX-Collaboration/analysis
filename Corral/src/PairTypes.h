//---------------------------------------------------------------
//
//	PairTypes:	specifies 2 particles to calculate R2 for... 
//			PairTypes[0][...] is ParticleID for particle 1 in this pair
//			PairTypes[1][...] is ParticleID for particle 2 in this pair
//			PairTypes[2][...] is N_min for this pair (README_Finalize step 3, option A, user 2026-09-26):
//				a zvtx bin counts in the Zvtx average of a (dy,dphi) bin only if N_exp = nevt*rho2(M) >= N_min
//				there (evaluated at full statistics in Finalize). 0 = the old rule (rho2(M)>0).
//				(Legacy STAR use of this field, now free: kCentralityStyle =2 refmult2, =3 refmult3.)
//				2026-09-26 values: pions and V0-pi 10 (signed off); V0-V0 1 (too sparse for 10 with the
//				current production: N_min>=5 leaves no valid bin) -- revisit with the bigger dataset.
//			PairTypes[3][...] is the y bin width in units of 0.01 (2026-10-05, user; 5 = 0.05): y1, y2 and dy all use it.
//				Each species window (abs(y) < Species_yu, fluct_common.h: full widths pi 2.0, K/K0s/Lambda/Lbar
//				1.4, p/pbar 1.2) must hold a whole number of bins, so only the values listed after "bw allowed"
//				on each row are valid (common divisors of the two windows, >= 2 bins;
//				finer values are there for when the datasets grow); corral stops
//				at startup on any other value. Starting values from the ana573_v5 canary (empty mixed bins);
//				2026-10-06 retune from the first 46-pairtype ana573 canary: K-K, K-p coarser; p-pbar, pbar-pbar 0.08;
//				pi-pi, pi-K, pi-p 0.04, pi-V0 0.05, K-Lambda, K-K0s 0.20 on trial (back off if the next canary shows empty bins).
//
//---------------------------------------------------------------
//
// ---- 2026-09-28 (README_PID.md): 28 pair types, rearranged by the user: pions 0-3 (unchanged -- code writing
// ---- pion-only histograms assumes ipaty<=3), then K+K-, p-pbar, p-p, pbar-pbar, p-pi, p-V0, pi-V0, V0-V0.
// ---- pi-V0 now has the pion FIRST (was V0 first): dphi = phi(pi)-phi(V0), the opposite sign of pre-2026-09-28 outputs.
// ---- N_min: pion-containing 10 (pi-K too, 2026-10-05); the sparser K-K, p-p, p-V0, V0-V0 types 1 for now.
// ---- 2026-10-05 (user): 46 pair types: + K+K+, K-K-, pi-K, K-p, K-V0, K-K0s, p-K0s. pi-p now has the pion FIRST
// ---- (was p first): dphi = phi(pi)-phi(p), the opposite sign of pre-2026-10-05 outputs.
static const int NPairTypes		= 46;
static const int PairTypes_Info[NPairTypes][4]= {
	{ kParticleIDPionPlus,     kParticleIDPionMinus   ,10,   4 },	// 0  pi+ pi-                  bw: 1 2 4 5 8 10 20 25 40 50 100
	{ kParticleIDPionMinus,    kParticleIDPionPlus    ,10,   4 },	// 1  pi- pi+  	               bw: 1 2 4 5 8 10 20 25 40 50 100
	{ kParticleIDPionPlus,     kParticleIDPionPlus    ,10,   4 },	// 2  pi+ pi+                  bw: 1 2 4 5 8 10 20 25 40 50 100
	{ kParticleIDPionMinus,    kParticleIDPionMinus   ,10,   4 },	// 3  pi- pi-                  bw: 1 2 4 5 8 10 20 25 40 50 100
	{ kParticleIDKaonPlus,     kParticleIDKaonMinus   , 1,  10 },	// 4  K+ K- (phi)              bw: 1 2 4 5 7 10 14 20 28 35 70
	{ kParticleIDKaonPlus,     kParticleIDKaonPlus    , 1,  10 },	// 5  K+ K+                    bw: 1 2 4 5 7 10 14 20 28 35 70
	{ kParticleIDKaonMinus,    kParticleIDKaonMinus   , 1,  10 },	// 6  K- K-                    bw: 1 2 4 5 7 10 14 20 28 35 70
	{ kParticleIDProton,       kParticleIDAntiProton  , 1,   8 },	// 7  p pbar                   bw: 1 2 3 4 5 6 8 10 12 15 20 24 30 40 60
	{ kParticleIDProton,       kParticleIDProton      , 1,  6 },	// 8  p p                      bw: 1 2 3 4 5 6 8 10 12 15 20 24 30 40 60
	{ kParticleIDAntiProton,   kParticleIDAntiProton  , 1,   8 },	// 9  pbar pbar                bw: 1 2 3 4 5 6 8 10 12 15 20 24 30 40 60
	{ kParticleIDPionPlus,     kParticleIDKaonMinus   ,10,   4 },	// 10 pi+ K-                   bw: 1 2 4 5 10 20
	{ kParticleIDPionMinus,    kParticleIDKaonPlus    ,10,   4 },	// 11 pi- K+                   bw: 1 2 4 5 10 20
	{ kParticleIDPionPlus,     kParticleIDKaonPlus    ,10,   4 },	// 12 pi+ K+                   bw: 1 2 4 5 10 20
	{ kParticleIDPionMinus,    kParticleIDKaonMinus   ,10,   4 },	// 13 pi- K-                   bw: 1 2 4 5 10 20
	{ kParticleIDPionPlus,     kParticleIDAntiProton  ,10,   4 },	// 14 pi+ pbar                 bw: 1 2 4 5 8 10 20 40
	{ kParticleIDPionMinus,    kParticleIDProton      ,10,   4 },	// 15 pi- p (Lambda, Delta0)   bw: 1 2 4 5 8 10 20 40
	{ kParticleIDPionPlus,     kParticleIDProton      ,10,   4 },	// 16 pi+ p (Delta++)          bw: 1 2 4 5 8 10 20 40
	{ kParticleIDPionMinus,    kParticleIDAntiProton  ,10,   4 },	// 17 pi- pbar                 bw: 1 2 4 5 8 10 20 40
	{ kParticleIDKaonPlus,     kParticleIDAntiProton  , 1,  10 },	// 18 K+ pbar                  bw: 1 2 4 5 10 20
	{ kParticleIDKaonMinus,    kParticleIDProton      , 1,  10 },	// 19 K- p                     bw: 1 2 4 5 10 20
	{ kParticleIDKaonPlus,     kParticleIDProton      , 1,  10 },	// 20 K+ p                     bw: 1 2 4 5 10 20
	{ kParticleIDKaonMinus,    kParticleIDAntiProton  , 1,  10 },	// 21 K- pbar                  bw: 1 2 4 5 10 20
	{ kParticleIDKaonPlus,     kParticleIDLambda      , 1, 20 },	// 22 K+ Lambda                bw: 1 2 4 5 7 10 14 20 28 35 70
	{ kParticleIDKaonPlus,     kParticleIDAntiLambda  , 1, 20 },	// 23 K+ Lbar                  bw: 1 2 4 5 7 10 14 20 28 35 70
	{ kParticleIDKaonMinus,    kParticleIDLambda      , 1, 20 },	// 24 K- Lambda                bw: 1 2 4 5 7 10 14 20 28 35 70
	{ kParticleIDKaonMinus,    kParticleIDAntiLambda  , 1, 20 },	// 25 K- Lbar                  bw: 1 2 4 5 7 10 14 20 28 35 70
	{ kParticleIDProton,       kParticleIDLambda      , 1, 20 },	// 26 p Lambda                 bw: 1 2 4 5 10 20
	{ kParticleIDProton,       kParticleIDAntiLambda  , 1, 20 },	// 27 p Lbar                   bw: 1 2 4 5 10 20
	{ kParticleIDAntiProton,   kParticleIDLambda      , 1, 20 },	// 28 pbar Lambda              bw: 1 2 4 5 10 20
	{ kParticleIDAntiProton,   kParticleIDAntiLambda  , 1, 20 },	// 29 pbar Lbar                bw: 1 2 4 5 10 20
	{ kParticleIDPionPlus,     kParticleIDKshort      ,10,  5 },	// 30 pi+ K0s (K*+)            bw: 1 2 4 5 10 20
	{ kParticleIDPionMinus,    kParticleIDKshort      ,10,  5 },	// 31 pi- K0s (K*-)            bw: 1 2 4 5 10 20
	{ kParticleIDPionPlus,     kParticleIDLambda      ,10,  5 },	// 32 pi+ Lambda (Sigma*+)     bw: 1 2 4 5 10 20
	{ kParticleIDPionMinus,    kParticleIDLambda      ,10,  5 },	// 33 pi- Lambda (Sigma*-,Xi-) bw: 1 2 4 5 10 20
	{ kParticleIDPionPlus,     kParticleIDAntiLambda  ,10,  5 },	// 34 pi+ Lbar                 bw: 1 2 4 5 10 20
	{ kParticleIDPionMinus,    kParticleIDAntiLambda  ,10,  5 },	// 35 pi- Lbar                 bw: 1 2 4 5 10 20
	{ kParticleIDKaonPlus,     kParticleIDKshort      , 1, 20 },	// 36 K+ K0s                   bw: 1 2 4 5 7 10 14 20 28 35 70
	{ kParticleIDKaonMinus,    kParticleIDKshort      , 1, 20 },	// 37 K- K0s                   bw: 1 2 4 5 7 10 14 20 28 35 70
	{ kParticleIDProton,       kParticleIDKshort      , 1, 20 },	// 38 p K0s                    bw: 1 2 4 5 10 20
	{ kParticleIDAntiProton,   kParticleIDKshort      , 1, 20 },	// 39 pbar K0s                 bw: 1 2 4 5 10 20
	{ kParticleIDKshort,       kParticleIDKshort      , 1, 35 },	// 40 K0s K0s                  bw: 1 2 4 5 7 10 14 20 28 35 70
	{ kParticleIDLambda,       kParticleIDLambda      , 1, 35 },	// 41 Lambda Lambda            bw: 1 2 4 5 7 10 14 20 28 35 70
	{ kParticleIDAntiLambda,   kParticleIDAntiLambda  , 1, 35 },	// 42 Lbar Lbar                bw: 1 2 4 5 7 10 14 20 28 35 70
	{ kParticleIDKshort,       kParticleIDLambda      , 1, 35 },	// 43 K0s Lambda               bw: 1 2 4 5 7 10 14 20 28 35 70
	{ kParticleIDKshort,       kParticleIDAntiLambda  , 1, 35 },	// 44 K0s Lbar                 bw: 1 2 4 5 7 10 14 20 28 35 70
	{ kParticleIDLambda,       kParticleIDAntiLambda  , 1, 35 }		// 45 Lambda Lbar              bw: 1 2 4 5 7 10 14 20 28 35 70
};

//---- end of PairTypes.h

