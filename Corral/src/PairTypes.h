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
//
//---------------------------------------------------------------
//
//
// static const int NPairTypes		= 2;
// static const int PairTypes_Info[NPairTypes][3]= {	
// 					{ kParticleIDPionPlus,     kParticleIDPionPlus     ,2 },	// pi   pi		0
// 					{ kParticleIDPionPlus,     kParticleIDPionMinus    ,2 }		// pi   pi		0
// 					};
// static const int NPairTypes3	= 3;
// static const int PairTypes3_Info[NPairTypes3][3]= {	
// 					{ kParticleIDProton,      kParticleIDProton,     kParticleIDAntiProton  },	// 227 p-p-pbar 1
// 					{ kParticleIDProton,      kParticleIDAntiProton, kParticleIDProton      },	// 272 p-pbar-p	1
// 					{ kParticleIDAntiProton,  kParticleIDProton,     kParticleIDProton      }	// 722 pbar-p-p	1
// 					};
//


// static const int NPairTypes		= 6;
// static const int PairTypes_Info[NPairTypes][3]= {	
// 					{ kParticleIDPionPlus,     kParticleIDPionMinus   ,2 },		
// 					{ kParticleIDPionPlus,     kParticleIDPionPlus    ,2 },		
// 					{ kParticleIDPionMinus,    kParticleIDPionMinus   ,2 },
// 					{ kParticleIDKshort,       kParticleIDKshort      ,2 },
// 					{ kParticleIDKshort,       kParticleIDPionPlus    ,2 },		
// 					{ kParticleIDKshort,       kParticleIDPionMinus   ,2 }	
//  			};


// static const int NPairTypes		= 3;
// static const int PairTypes_Info[NPairTypes][3]= {
// 					{ kParticleIDPionPlus,     kParticleIDPionMinus   ,2 },
// 					{ kParticleIDPionPlus,     kParticleIDPionPlus    ,2 },
// 					{ kParticleIDPionMinus,    kParticleIDPionMinus   ,2 }
// 				};
// ---- 2026-09-22 (README_SplitTracks.md sec 15.9 closeout): reverted back to the full
// ---- 15-pairtype table -- both the LS (sec 13/14) and ULS (sec 15) split-track threads
// ---- are paused for now ("totally done with split tracks, LS & ULS," user's own framing).
// ---- A fresh full-stats, full-pairtype, no-RunString production pass is being run to
// ---- pick up all the new diagnostic instrumentation added during the ULS work (sec 15's
// ---- Q_inv/quality/pt histograms etc. never ran at full production scale before now).

// ---- 2026-09-24 (README sec 18.24): back to the full production table, now 16 pairtypes -- the
// ---- 15-pairtype table plus pi-pi+ (species order reversed vs pi+pi-, sec 18.18's sign test; kept as
// ---- a cross-check for the crossing correction). Pion pairtypes first, in the order +-, -+, ++, --.
// ---- NOTE indices shifted vs the 3/4-pairtype era: pi+pi+ was 1, now 2; pi-pi- was 2, now 3.
// ---- 2026-09-25 (README_Crossing.md): the 4-pion development table below was used while the
// ---- crossing correction was built; the full 16-pairtype table is restored (after it).
// static const int NPairTypes		= 4;
// static const int PairTypes_Info[NPairTypes][3]= {
// 					{ kParticleIDPionPlus,     kParticleIDPionMinus   ,2 },		// 0  pi+ pi-
// 					{ kParticleIDPionMinus,    kParticleIDPionPlus    ,2 },		// 1  pi- pi+  (sign-flip cross-check)
// 					{ kParticleIDPionPlus,     kParticleIDPionPlus    ,2 },		// 2  pi+ pi+
// 					{ kParticleIDPionMinus,    kParticleIDPionMinus   ,2 }		// 3  pi- pi-
// 				};
static const int NPairTypes		= 16;
static const int PairTypes_Info[NPairTypes][3]= {
					{ kParticleIDPionPlus,     kParticleIDPionMinus   ,10 },		// 0  pi+ pi-
					{ kParticleIDPionMinus,    kParticleIDPionPlus    ,10 },		// 1  pi- pi+  (sign-flip cross-check)
					{ kParticleIDPionPlus,     kParticleIDPionPlus    ,10 },		// 2  pi+ pi+
					{ kParticleIDPionMinus,    kParticleIDPionMinus   ,10 },		// 3  pi- pi-
					{ kParticleIDKshort,       kParticleIDKshort      , 1 },		// 4
					{ kParticleIDLambda,       kParticleIDLambda      , 1 },		// 5
					{ kParticleIDAntiLambda,   kParticleIDAntiLambda  , 1 },		// 6
					{ kParticleIDKshort,       kParticleIDLambda      , 1 },		// 7
					{ kParticleIDKshort,       kParticleIDAntiLambda  , 1 },		// 8
					{ kParticleIDLambda,       kParticleIDAntiLambda  , 1 },		// 9
					{ kParticleIDKshort,       kParticleIDPionPlus    ,10 },		// 10
					{ kParticleIDKshort,       kParticleIDPionMinus   ,10 },		// 11
					{ kParticleIDLambda,       kParticleIDPionPlus    ,10 },		// 12
					{ kParticleIDLambda,       kParticleIDPionMinus   ,10 },		// 13
					{ kParticleIDAntiLambda,   kParticleIDPionPlus    ,10 },		// 14
					{ kParticleIDAntiLambda,   kParticleIDPionMinus   ,10 }		// 15
				};
// ---- previous 4-pairtype diagnostic table (sec 16 / 18.18), kept for reference:
// ---- 2026-09-22 (README sec 16): cut to 3 pion pairtypes for the post-num/denom-fix LS and ULS
// ---- re-scans. MUST be reverted to 15 (uncomment below) before any production run.
// static const int NPairTypes		= 4;
// static const int PairTypes_Info[NPairTypes][3]= {
// 					{ kParticleIDPionPlus,     kParticleIDPionMinus   ,2 },
// 					{ kParticleIDPionPlus,     kParticleIDPionPlus    ,2 },
// 					{ kParticleIDPionMinus,    kParticleIDPionMinus   ,2 },
// 					// sec 18.18: species order REVERSED vs pairtype 0 -- sign-flip test. If the
// 					// mixed-density peak is a genuinely SIGNED/curvature-driven effect (dphi =
// 					// phi(species1)-phi(species2), NOT symmetrized, PairTypes_Info sec 18.4), this
// 					// pairtype's peak should land at dphi~-3..-4deg, the mirror of pairtype 0's.
// 					{ kParticleIDPionMinus,    kParticleIDPionPlus    ,2 }
// 				};
// static const int NPairTypes		= 15;
// static const int PairTypes_Info[NPairTypes][3]= {
// 					{ kParticleIDPionPlus,     kParticleIDPionMinus   ,2 },
// 					{ kParticleIDPionPlus,     kParticleIDPionPlus    ,2 },
// 					{ kParticleIDPionMinus,    kParticleIDPionMinus   ,2 },
// 					{ kParticleIDKshort,       kParticleIDKshort      ,2 },
// 					{ kParticleIDLambda,       kParticleIDLambda      ,2 },
// 					{ kParticleIDAntiLambda,   kParticleIDAntiLambda  ,2 },
// 					{ kParticleIDKshort,       kParticleIDLambda      ,2 },
// 					{ kParticleIDKshort,       kParticleIDAntiLambda  ,2 },
// 					{ kParticleIDLambda,       kParticleIDAntiLambda  ,2 },
// 					{ kParticleIDKshort,       kParticleIDPionPlus    ,2 },
// 					{ kParticleIDKshort,       kParticleIDPionMinus   ,2 },
// 					{ kParticleIDLambda,       kParticleIDPionPlus    ,2 },
// 					{ kParticleIDLambda,       kParticleIDPionMinus   ,2 },
// 					{ kParticleIDAntiLambda,   kParticleIDPionPlus    ,2 },
// 					{ kParticleIDAntiLambda,   kParticleIDPionMinus   ,2 }
//  				};


//
// static const int NPairTypes		= 22;
// static const int PairTypes_Info[NPairTypes][3]= {	
// 					{ kParticleIDPionPlus,     kParticleIDPionPlus     ,2 },	// pi   pi		0
// 					{ kParticleIDPionPlus,     kParticleIDPionMinus    ,2 },	// pi   pi		0
// 					{ kParticleIDKaonPlus,     kParticleIDKaonPlus     ,2 },	// K    K		0
// 					{ kParticleIDKaonPlus,     kParticleIDKaonMinus    ,2 },	// K    K		0
// 					//
// 					{ kParticleIDAntiProton,   kParticleIDAntiProton   ,2 },	// pbar pbar	-2
// 					{ kParticleIDAntiProton,   kParticleIDAntiNeutron  ,2 },	// pbar nbar	-2
// 					{ kParticleIDAntiNeutron,  kParticleIDAntiNeutron  ,2 },	// nbar nbar	-2
// 					{ kParticleIDAntiProton,   kParticleIDNeutron      ,2 },	// pbar n 		0
// 					{ kParticleIDProton,       kParticleIDAntiProton   ,2 },	// p    pbar 	0
// 					{ kParticleIDNeutron,      kParticleIDAntiNeutron  ,2 },	// n    nbar 	0
// 					{ kParticleIDProton,       kParticleIDAntiNeutron  ,2 },	// p    nbar	0
// 					{ kParticleIDNeutron,      kParticleIDNeutron      ,2 },	// n    n 		2
// 					{ kParticleIDProton,       kParticleIDNeutron      ,2 },	// p    n 		2
// 					{ kParticleIDProton,       kParticleIDProton       ,2 },	// p    p 		2
// 					//
// 					{ kParticleIDAntiProton,   kParticleIDAntiDeuteron ,2 },	// pbar Dbar	-3
// 					{ kParticleIDAntiNeutron,  kParticleIDAntiDeuteron ,2 },	// nbar Dbar	-3
// 					{ kParticleIDProton,       kParticleIDAntiDeuteron ,2 },	// p    Dbar	-1
// 					{ kParticleIDNeutron,      kParticleIDAntiDeuteron ,2 },	// n    Dbar	-1
// 					{ kParticleIDAntiProton,   kParticleIDDeuteron     ,2 },	// pbar D		1
// 					{ kParticleIDAntiNeutron,  kParticleIDDeuteron     ,2 },	// nbar D		1
// 					{ kParticleIDProton,       kParticleIDDeuteron     ,2 },	// p    D		3
// 					{ kParticleIDNeutron,      kParticleIDDeuteron     ,2 }		// n    D		3
// 			};

// static const int NPairTypes3	= 24;
// static const int PairTypes3_Info[NPairTypes3][3]= {	
// 					{ kParticleIDPionPlus,    kParticleIDPionPlus,   kParticleIDPionPlus    },	// PIONS
// 					{ kParticleIDPionPlus,    kParticleIDPionPlus,   kParticleIDPionMinus   },	// PIONS
// 					{ kParticleIDPionPlus,    kParticleIDPionMinus,  kParticleIDPionMinus   },	// PIONS
// 					{ kParticleIDPionMinus,   kParticleIDPionMinus,  kParticleIDPionMinus   },	// PIONS
// 					{ kParticleIDKaonPlus,    kParticleIDKaonPlus,   kParticleIDKaonPlus    },	// Kaons
// 					{ kParticleIDKaonPlus,    kParticleIDKaonPlus,   kParticleIDKaonMinus   },	// Kaons
// 					{ kParticleIDKaonPlus,    kParticleIDKaonMinus,  kParticleIDKaonMinus   },	// Kaons
// 					{ kParticleIDKaonMinus,   kParticleIDKaonMinus,  kParticleIDKaonMinus   },	// Kaons
// 					//
// 					{ kParticleIDProton,      kParticleIDProton,     kParticleIDAntiProton  },	// 227 p-p-pbar 1
// 					{ kParticleIDProton,      kParticleIDProton,     kParticleIDAntiNeutron },	// 228 p-p-nbar	1
// 					{ kParticleIDProton,      kParticleIDNeutron,    kParticleIDAntiProton  },	// 237 p-n-pbar 1
// 					{ kParticleIDProton,      kParticleIDNeutron,    kParticleIDAntiNeutron },	// 238 p-n-nbar 1
// 					{ kParticleIDNeutron,     kParticleIDNeutron,    kParticleIDAntiProton  },	// 337 n-n-pbar	1
// 					{ kParticleIDNeutron,     kParticleIDNeutron,    kParticleIDAntiNeutron },	// 338 n-n-nbar	1
// 					//
// 					{ kParticleIDNeutron,     kParticleIDAntiProton, kParticleIDNeutron     },	// n-pbar-n		1
// 					{ kParticleIDProton,      kParticleIDAntiNeutron,kParticleIDProton      },	// p-nbar-p		1
// 					{ kParticleIDAntiProton,  kParticleIDProton,     kParticleIDNeutron     },	// pbar	D		1
// 					{ kParticleIDAntiNeutron, kParticleIDProton,     kParticleIDNeutron     },	// nbar	D		1
// 					{ kParticleIDProton,      kParticleIDAntiProton, kParticleIDAntiNeutron },	// p	Dbar	-1
// 					{ kParticleIDNeutron,     kParticleIDAntiProton, kParticleIDAntiNeutron },	// n	Dbar	-1
// 					{ kParticleIDProton,      kParticleIDProton,     kParticleIDNeutron     },	// p	D		3
// 					{ kParticleIDNeutron,     kParticleIDProton,     kParticleIDNeutron     },	// n	D		3
// 					{ kParticleIDAntiProton,  kParticleIDAntiProton, kParticleIDAntiNeutron },	// pbar Dbar	-3
// 					{ kParticleIDAntiNeutron, kParticleIDAntiProton, kParticleIDAntiNeutron }	// nbar Dbar	-3
// 			};
