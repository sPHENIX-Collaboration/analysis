#ifndef CQRESONANCES_H
#define CQRESONANCES_H
//---- CQResonances.h -- the two-body decays seen as peaks in C(Q), and the variable Q binning of the ir2=2 block
//----   (Q_inv, pT_pair) built from them (README_meanpT_from_CQ.md sec 11). One list for both, so the C(Q)
//----   signposts (DrawCQSignposts, corral_loop.cxx) and the fine Q bins always agree.
//---- Needs fluct_common.h (particle IDs) included before it.
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <cstdio>

//---- Q_inv (= dq = 2k*, CalcRm::PairInfo) at which a two-body decay R -> 1+2 lands:
//----   dq = sqrt((M^2-(m1+m2)^2)(M^2-(m1-m2)^2))/M; -1 below threshold. Order of the two legs doesn't matter.
inline double CQResonanceQ(double M, double m1, double m2){
	double a	= (M*M-(m1+m2)*(m1+m2))*(M*M-(m1-m2)*(m1-m2));
	return (a>0.) ? sqrt(a)/M : -1.;
}

//---- species a, b (either order), PDG mass M and width G (GeV; 0 = weak decay), leg masses m1, m2, label
struct CQResonance { int a, b; double M, G, m1, m2; const char* lab; };
namespace CQRes {
	const double mpi = 0.13957, mK0 = 0.497611, mL = 1.115683, mp = 0.938272, mK = 0.493677;		// PDG
	const int PIP = kParticleIDPionPlus, PIM = kParticleIDPionMinus;
	const int PP = kParticleIDProton, PM = kParticleIDAntiProton, KP = kParticleIDKaonPlus, KM = kParticleIDKaonMinus;
	const int KS = kParticleIDKshort, LA = kParticleIDLambda, AL = kParticleIDAntiLambda;
}
const CQResonance CQ_RESONANCES[]	= {
	{ CQRes::PIP, CQRes::PIM, 0.497611, 0.,       CQRes::mpi, CQRes::mpi, "K^{0}_{S}"        },
	{ CQRes::PIP, CQRes::PIM, 0.77526,  0.1474,   CQRes::mpi, CQRes::mpi, "#rho^{0}"         },
	{ CQRes::PIP, CQRes::PIM, 0.990,    0.055,    CQRes::mpi, CQRes::mpi, "f_{0}(980)"       },
	{ CQRes::PIP, CQRes::PIM, 1.2754,   0.1866,   CQRes::mpi, CQRes::mpi, "f_{2}(1270)"      },
	{ CQRes::KS,  CQRes::PIP, 0.89167,  0.0514,   CQRes::mK0, CQRes::mpi, "K*^{+}"           },
	{ CQRes::KS,  CQRes::PIM, 0.89167,  0.0514,   CQRes::mK0, CQRes::mpi, "K*^{-}"           },
	{ CQRes::LA,  CQRes::PIP, 1.38280,  0.0360,   CQRes::mL,  CQRes::mpi, "#Sigma*^{+}"      },
	{ CQRes::LA,  CQRes::PIM, 1.3872,   0.0394,   CQRes::mL,  CQRes::mpi, "#Sigma*^{-}"      },
	{ CQRes::LA,  CQRes::PIM, 1.32171,  0.,       CQRes::mL,  CQRes::mpi, "#Xi^{-}"          },
	{ CQRes::AL,  CQRes::PIM, 1.38280,  0.0360,   CQRes::mL,  CQRes::mpi, "#bar{#Sigma*}^{-}"},
	{ CQRes::AL,  CQRes::PIP, 1.3872,   0.0394,   CQRes::mL,  CQRes::mpi, "#bar{#Sigma*}^{+}"},
	{ CQRes::AL,  CQRes::PIP, 1.32171,  0.,       CQRes::mL,  CQRes::mpi, "#bar{#Xi}^{+}"    },
	{ CQRes::PP,  CQRes::PIM, 1.115683, 0.,       CQRes::mp,  CQRes::mpi, "#Lambda"          },		// Lambdas not reconstructed as V0s (README_PID.md)
	{ CQRes::PP,  CQRes::PIM, 1.232,    0.117,    CQRes::mp,  CQRes::mpi, "#Delta^{0}"       },
	{ CQRes::PP,  CQRes::PIP, 1.232,    0.117,    CQRes::mp,  CQRes::mpi, "#Delta^{++}"      },
	{ CQRes::PM,  CQRes::PIP, 1.115683, 0.,       CQRes::mp,  CQRes::mpi, "#bar{#Lambda}"    },
	{ CQRes::PM,  CQRes::PIP, 1.232,    0.117,    CQRes::mp,  CQRes::mpi, "#bar{#Delta}^{0}" },
	{ CQRes::PM,  CQRes::PIM, 1.232,    0.117,    CQRes::mp,  CQRes::mpi, "#bar{#Delta}^{--}"},
	{ CQRes::KP,  CQRes::KM,  1.019461, 0.004249, CQRes::mK,  CQRes::mK,  "#phi"             },
	//---- 2026-10-06: the 46-pairtype table (PDG masses; a0(980)->KK0s and f0(980)->K0sK0s sit below threshold)
	{ CQRes::KP,  CQRes::PIM, 0.89555,  0.0473,   CQRes::mK,  CQRes::mpi, "K*^{0}"           },
	{ CQRes::KM,  CQRes::PIP, 0.89555,  0.0473,   CQRes::mK,  CQRes::mpi, "#bar{K*}^{0}"     },
	{ CQRes::KP,  CQRes::PIM, 1.4324,   0.109,    CQRes::mK,  CQRes::mpi, "K_{2}*^{0}(1430)" },
	{ CQRes::KM,  CQRes::PIP, 1.4324,   0.109,    CQRes::mK,  CQRes::mpi, "#bar{K}_{2}*^{0}(1430)"},
	{ CQRes::KM,  CQRes::PIP, 1.86484,  0.,       CQRes::mK,  CQRes::mpi, "D^{0}"            },
	{ CQRes::KP,  CQRes::PIM, 1.86484,  0.,       CQRes::mK,  CQRes::mpi, "#bar{D}^{0}"      },
	{ CQRes::KP,  CQRes::KM,  1.86484,  0.,       CQRes::mK,  CQRes::mK,  "D^{0}"            },
	{ CQRes::KM,  CQRes::PP,  1.5194,   0.0157,   CQRes::mK,  CQRes::mp,  "#Lambda(1520)"    },
	{ CQRes::KP,  CQRes::PM,  1.5194,   0.0157,   CQRes::mK,  CQRes::mp,  "#bar{#Lambda}(1520)"},
	{ CQRes::KM,  CQRes::PP,  1.820,    0.080,    CQRes::mK,  CQRes::mp,  "#Lambda(1820)"    },
	{ CQRes::KP,  CQRes::PM,  1.820,    0.080,    CQRes::mK,  CQRes::mp,  "#bar{#Lambda}(1820)"},
	{ CQRes::KS,  CQRes::PIP, 1.86966,  0.,       CQRes::mK0, CQRes::mpi, "D^{+}"            },
	{ CQRes::KS,  CQRes::PIM, 1.86966,  0.,       CQRes::mK0, CQRes::mpi, "D^{-}"            },
	{ CQRes::KS,  CQRes::KP,  1.86966,  0.,       CQRes::mK0, CQRes::mK,  "D^{+}"            },
	{ CQRes::KS,  CQRes::KM,  1.86966,  0.,       CQRes::mK0, CQRes::mK,  "D^{-}"            },
	{ CQRes::KS,  CQRes::KP,  1.96835,  0.,       CQRes::mK0, CQRes::mK,  "D_{s}^{+}"        },
	{ CQRes::KS,  CQRes::KM,  1.96835,  0.,       CQRes::mK0, CQRes::mK,  "D_{s}^{-}"        },
	{ CQRes::KS,  CQRes::PP,  2.28646,  0.,       CQRes::mK0, CQRes::mp,  "#Lambda_{c}^{+}"  },
	{ CQRes::KS,  CQRes::PM,  2.28646,  0.,       CQRes::mK0, CQRes::mp,  "#bar{#Lambda}_{c}^{-}"},
	{ CQRes::LA,  CQRes::PIP, 2.28646,  0.,       CQRes::mL,  CQRes::mpi, "#Lambda_{c}^{+}"  },
	{ CQRes::AL,  CQRes::PIM, 2.28646,  0.,       CQRes::mL,  CQRes::mpi, "#bar{#Lambda}_{c}^{-}"},
	{ CQRes::LA,  CQRes::KM,  1.67245,  0.,       CQRes::mL,  CQRes::mK,  "#Omega^{-}"       },
	{ CQRes::AL,  CQRes::KP,  1.67245,  0.,       CQRes::mL,  CQRes::mK,  "#bar{#Omega}^{+}" },
	{ CQRes::LA,  CQRes::KM,  1.690,    0.020,    CQRes::mL,  CQRes::mK,  "#Xi(1690)^{-}"    },
	{ CQRes::AL,  CQRes::KP,  1.690,    0.020,    CQRes::mL,  CQRes::mK,  "#bar{#Xi}(1690)^{+}"},
	{ CQRes::LA,  CQRes::KM,  1.823,    0.024,    CQRes::mL,  CQRes::mK,  "#Xi(1820)^{-}"    },
	{ CQRes::AL,  CQRes::KP,  1.823,    0.024,    CQRes::mL,  CQRes::mK,  "#bar{#Xi}(1820)^{+}"},
	{ CQRes::LA,  CQRes::KS,  1.690,    0.020,    CQRes::mL,  CQRes::mK0, "#Xi(1690)^{0}"    },
	{ CQRes::AL,  CQRes::KS,  1.690,    0.020,    CQRes::mL,  CQRes::mK0, "#bar{#Xi}(1690)^{0}"},
	{ CQRes::LA,  CQRes::KS,  1.823,    0.024,    CQRes::mL,  CQRes::mK0, "#Xi(1820)^{0}"    },
	{ CQRes::AL,  CQRes::KS,  1.823,    0.024,    CQRes::mL,  CQRes::mK0, "#bar{#Xi}(1820)^{0}"},
	{ CQRes::KS,  CQRes::KS,  1.5174,   0.086,    CQRes::mK0, CQRes::mK0, "f'_{2}(1525)"     },
	{ CQRes::KS,  CQRes::KS,  1.733,    0.123,    CQRes::mK0, CQRes::mK0, "f_{0}(1710)"      },
};
const int NCQ_RESONANCES	= sizeof(CQ_RESONANCES)/sizeof(CQ_RESONANCES[0]);
//---- h+, h- (2026-10-11) carry the pion mass, so they take the pion entries: h+h- shows the pi+pi- signposts and bins
inline int CQResonancePid(int pid){ return (pid==kParticleIDhPlus) ? kParticleIDPionPlus : ((pid==kParticleIDhMinus) ? kParticleIDPionMinus : pid); }
inline bool CQResonanceMatches(const CQResonance &r, int pid1, int pid2){
	pid1 = CQResonancePid(pid1); pid2 = CQResonancePid(pid2);
	return (r.a==pid1 && r.b==pid2) || (r.a==pid2 && r.b==pid1);
}

//---- ir2=2 axes (README_meanpT_from_CQ.md sec 11, user 2026-10-10)
//---- Q_inv: 0-2 GeV (as hQsib), VARIABLE WIDTH, per species pair. Built on a 20 MeV grid: every 20 MeV cell is
//----   split into the finest bin width any of these rules asks for, so every bin nests inside the uniform
//----   20 MeV bins and every 1 MeV bin of hQsib nests inside it:
//----   - Q < 0.10 GeV: 2 MeV; 0.10-0.20 GeV: 5 MeV (Coulomb and short-range correlations)
//----   - each CQ_RESONANCES entry of this species pair at Q0: width_Q = max(Gamma_Q/2.355, sigma_Q) with the
//----     PLACEHOLDER resolution sigma_Q = max(2 MeV, 0.03 Q0) (no final tracking yet; written on the pages);
//----     window Q0 +- (5 width_Q + 0.03 Q0) (the last term for the momentum scale); bin = the largest of
//----     1, 2, 4, 5, 10, 20 MeV that is <= width_Q/4 (never underbin a sharp peak)
//----   - elsewhere 20 MeV
//---- pT_pair = |pT1+pT2| (= 2 kT): 0-5 GeV/c, 50 uniform bins.
const double QPT_QMAX		= 2.0;
const int    QPT_NPT		= 50;
const double QPT_PTMAX		= 5.0;
const char* const QPT_NOTE	= "variable width binning in Q";
inline std::vector<double> QBinEdges(int pid1, int pid2){
	const int NCELL	= (int)lround(QPT_QMAX/0.020);
	std::vector<int> w(NCELL,20);						// bin width (MeV) per 20 MeV cell
	for (int c=0;c<NCELL;c++){ double q = 0.020*c; if (q<0.0999) w[c] = 2; else if (q<0.1999) w[c] = 5; }
	const int allowed[6]	= {1,2,4,5,10,20};
	for (int i=0;i<NCQ_RESONANCES;i++){
		const CQResonance &r	= CQ_RESONANCES[i];
		if (!CQResonanceMatches(r,pid1,pid2)) continue;
		double q0	= CQResonanceQ(r.M,r.m1,r.m2);
		if (q0<=0. || q0>=QPT_QMAX) continue;
		double gq	= 0.;
		if (r.G>0.){ double qh = CQResonanceQ(r.M+r.G/2.,r.m1,r.m2), ql = CQResonanceQ(r.M-r.G/2.,r.m1,r.m2); gq = qh - std::max(ql,0.); }
		double sig	= std::max(0.002,0.03*q0);
		double wq	= std::max(gq/2.355,sig);
		double hw	= 5.*wq + 0.03*q0;
		int bw		= 1; for (int a : allowed) if (a<=1000.*wq/4.+1e-9) bw = a;
		int c1		= std::max(0,(int)floor((q0-hw)/0.020)), c2 = std::min(NCELL-1,(int)floor((q0+hw)/0.020));
		for (int c=c1;c<=c2;c++) w[c] = std::min(w[c],bw);
	}
	std::vector<double> e;
	for (int c=0;c<NCELL;c++) for (int k=0;k<20/w[c];k++) e.push_back(0.001*(20*c+k*w[c]));
	e.push_back(QPT_QMAX);
	return e;
}
//---- the Q edges as segments "0.000-0.100 @2" ... for the log
inline std::string QBinSummary(const std::vector<double> &e){
	std::string s; char buf[64];
	size_t i = 0;
	while (i+1<e.size()){
		int w	= (int)lround(1000.*(e[i+1]-e[i]));
		size_t j = i+1; while (j+1<e.size() && (int)lround(1000.*(e[j+1]-e[j]))==w) j++;
		snprintf(buf,sizeof(buf),"%s%.3f-%.3f @%d",s.empty()?"":", ",e[i],e[j],w); s += buf;
		i = j;
	}
	return s;
}
#endif
