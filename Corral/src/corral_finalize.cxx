#include "corral_class.h"
#include "fluct_common.h"
#include "PairTypes.h"
#include "finalize_hists.h"
#include "CrossingCorrect.h"	// same dirty-side rule and correction as CalcRm::Calculate
#include "ZvtxAverage.h"		// same Zvtx average as CalcRm::Calculate
#include <fstream>
#include <map>
#include <algorithm>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TStyle.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TLine.h>
#include <TBox.h>
#include <TLatex.h>
#include <TFitResult.h>
#include <sstream>

//------------------------------------------------------------
// Finalize (README_Finalize.md). Combine the chunk outputs of a chunked production.
//   CF histograms (hrho2_<ir2>, hrho1rho1_<ir2>, hC2_<ir2>, hR2_<ir2>, hrho2C_1, hC2C_1, hR2C_1):
//     step 2, per zvtx bin z: S = sum_c(nevt S)/sum_c(nevt), M = sum_c(nevt df M)/sum_c(nevt df) over the
//     chunks where z was computed (df>0, nevt>=2); then C2/R2 exactly as CalcRm::Calculate, crossing
//     correction (ir2=1) with CrossingCorrect.h, and the Zvtx average with ZvtxAverageValid (weights =
//     combined hzvtx fractions).
//     Zvtx-average validity (option A, user 2026-09-26): zvtx bin z counts for a bin iff rho2(M)>0 and
//     N_exp = nevt_z*rho2(M)_z >= N_min (PairTypes.h 3rd field, per pairtype) at full statistics (edge zvtx bins with O(1) pairs
//     otherwise dominate the noise of the average).
//     step 3, subgroup errors: the SAME chain run on each chunk alone (a subgroup = one chunk), with the
//     FULL-STATS zvtx weights and validity masks, so each subgroup is the same linear combination as the
//     central value. Per bin, error = s/sqrt(N_valid), s = sample std (N-1) over the chunks usable there
//     (every zvtx bin -- and, corrected, every source bin -- the full-stats value uses is defined in the
//     chunk); others are skipped; N_valid < NVALIDMIN -> error 0 (no reliable error, empty-bin rule).
//     The central value stays the full-stats combination.
//   Anything else in the list: plain sum over chunks (step 1).
// Every listed histogram is compared with the single-job reference FINALIZE_REF and written to
// root/<base>.root; one pdf page per name (4x4 pairtypes), plus subgroup-error pages for the CF names.
//
void DrawCQSignposts(int ipid1, int ipid2, double ymin, double ymax, double xmax);	// corral_loop.cxx

namespace {
	//---- is this base name a CF histogram Finalize rebuilds?  kind: 0 S, 1 M, 2 C2, 3 R2, 4 rho2C, 5 C2C, 6 R2C
	bool ParseCF(const TString& base, int& kind, int& ir2){
		const char* pre[4]	= {"hrho2_","hrho1rho1_","hC2_","hR2_"};
		if (base=="hrho2C_1"){ kind=4; ir2=1; return true; }
		if (base=="hC2C_1")  { kind=5; ir2=1; return true; }
		if (base=="hR2C_1")  { kind=6; ir2=1; return true; }
		for (int k=0;k<4;k++){
			TString p	= pre[k];
			if (base.BeginsWith(p) && base.Length()==p.Length()+1 && isdigit(base[p.Length()])){
				kind	= k; ir2 = base[p.Length()]-'0';
				return ir2>=0 && ir2<=2;
			}
		}
		return false;
	}
	//---- base name of each CF kind (same order as ParseCF), %d = ir2
	const char* KINDNAME[7]	= {"hrho2_%d","hrho1rho1_%d","hC2_%d","hR2_%d","hrho2C_%d","hC2C_%d","hR2C_%d"};
	//---- output of one run of the CF chain: Zvtx-averaged maps per kind, and their validity
	//---- (V: some used zvtx bin has rho2(M)>0; VC: same with the crossing-corrected mask; ir2=1 only)
	//---- keepMasks: also the per-zvtx-bin validity masks (VZ, uncorrected; MK, corrected) and the Zvtx
	//---- weights, so that the subgroups can be run with the full-stats ones (option A)
	struct CFSet {
		TH2D	*h[7];
		TH2D	*V, *VC;
		std::vector<TH2D*>	vz, mk;
		double	wz[64];
		bool	okz[64];
		CFSet(){ for (int k=0;k<7;k++) h[k]=0; V=0; VC=0; for (int z=0;z<64;z++){ wz[z]=0.; okz[z]=false; } }
		void Delete(){
			for (int k=0;k<7;k++){ delete h[k]; h[k]=0; } delete V; V=0; delete VC; VC=0;
			for (TH2D* h2 : vz) delete h2; vz.clear();
			for (TH2D* h2 : mk) delete h2; mk.clear();
		}
	};
	double Median(std::vector<double> v){
		if (v.empty()) return 0.;
		std::sort(v.begin(),v.end());
		return v[v.size()/2];
	}
}

void corral::Finalize(){
	//
	TString PROJ	= CorralDir();
	TString SET		= TString(FINALIZE_SET);
	const int NVALIDMIN	= 10;		// step 3 pick (a): fewer valid subgroups -> error 0
	//---- Zvtx-average validity threshold N_min per pairtype = PairTypes_Info[ipaty][2] (PairTypes.h);
	//---- the environment variable FINALIZE_NEXPMIN overrides it for ALL pairtypes (scans)
	const bool nexpOverride	= (getenv("FINALIZE_NEXPMIN")!=0);
	const double nexpGlobal	= nexpOverride ? atof(getenv("FINALIZE_NEXPMIN")) : -1.;
	if (nexpOverride) cout<<"corral::Finalize -- Zvtx-average validity: N_exp = nevt*rho2(M) >= "<<nexpGlobal<<" for ALL pairtypes (FINALIZE_NEXPMIN override)"<<endl;
	else              cout<<"corral::Finalize -- Zvtx-average validity: N_exp = nevt*rho2(M) >= N_min per pairtype (PairTypes.h 3rd field)"<<endl;
	//
	//---- number of chunks from the list manifest
	int nlists	= -1;
	{
		std::ifstream fm((PROJ+"lists/"+SET+"/manifest.txt").Data());
		std::string key; std::string val;
		while (fm>>key>>val){ if (key=="nlists") nlists = atoi(val.c_str()); }
	}
	if (nlists<=0){ cout<<"corral::Finalize -- no nlists in "<<PROJ<<"lists/"<<SET<<"/manifest.txt (set CORRAL_DIR?), exit"<<endl; exit(1); }
	cout<<"corral::Finalize -- set "<<SET<<": "<<nlists<<" chunks, "<<NFINALIZEHISTS<<" histogram base names x "<<NPairTypes<<" pairtypes"<<endl;
	//
	//---- open all chunk files (all must exist) and the reference
	std::vector<TFile*> fch;
	for (int ic=0;ic<nlists;ic++){
		TString fn	= PROJ+"root/"+SET+Form("/corral_m_%02d.root",ic);
		TFile *f	= TFile::Open(fn.Data());
		if (!f || f->IsZombie()){ cout<<"corral::Finalize -- cannot open "<<fn<<", exit"<<endl; exit(1); }
		fch.push_back(f);
	}
	TString REF	= PROJ+FINALIZE_REF;
	TFile *fref	= TFile::Open(REF.Data());
	if (!fref || fref->IsZombie()){ cout<<"corral::Finalize -- cannot open reference "<<REF<<", exit"<<endl; exit(1); }
	//
	bool anyCF	= false;
	for (int ih=0;ih<NFINALIZEHISTS;ih++){ int kd,ir; if (ParseCF(TString(FinalizeHists[ih]),kd,ir)) anyCF = true; }
	//
	//---- results: name -> histogram (all pairtypes), filled pairtype by pairtype; extra = written, not compared
	std::map<TString,TH1*> result;
	std::vector<TH1*> extra;
	std::vector<TObject*> extraW;		// written only (step 1b maps and graphs)
	//
	//---- README_Finalize step 1b: crossing loss = dphi-mirror asymmetry of C2 on the near side, per pairtype.
	//---- Tested half: the pions' dirty side (dirty side sign; dphi<0 here) for ALL pairtypes, so the V0
	//---- pairtypes (no correction) are the null test on the same half. Windows: 0 = whole near-side half,
	//---- 1 = near side |dy|<XLDYCUT, 2 = away-side half (internal null). Bins: valid at full stats in the
	//---- bin AND its mirror. D = sum[C2(mirror)-C2(bin)], f = D / sum rho2(S)(mirror); fM = the same with
	//---- rho2(M) (acceptance asymmetry check). D is built from avg(S)-avg(M) (= avg(C2), linear), so every
	//---- chunk contributes (no usability cut).
	const double XLDYCUT	= 0.2;
	struct XLoss {
		bool	have;
		int		ds, half;
		double	f[3], ef[3], fM[3], efM[3], fA[3], efA[3], fC, dycut, xlo, xhi;
		int		nuse[3];
		std::vector<double>	fc[3], efc[3], fMc[3], fAc[3];
		std::vector<bool>	usec[3];
		std::vector<double>	x;		// <n1><n2> per chunk (hmult)
		TH2D	*A;					// full stats C2(mirror)-C2
		TH1D	*Ady;				// D per dy column, near half, subgroup errors
		double	chi2c, ndfc, slopex, eslopex, slopec, eslopec;	// fits (filled when drawing)
		XLoss(){ have=false; ds=0; half=-1; fC=0; dycut=0; xlo=xhi=0; A=0; Ady=0; chi2c=ndfc=slopex=eslopex=slopec=eslopec=0;
			for (int w=0;w<3;w++){ f[w]=ef[w]=fM[w]=efM[w]=fA[w]=efA[w]=0; nuse[w]=0; } }
	};
	std::vector<XLoss> xl(NPairTypes);
	//
	//---- physics pages (user 2026-09-26): per pairtype, the corral_m pages rebuilt from the Finalize maps,
	//---- all projections with SUBGROUP errors (the projection is formed inside every chunk with the
	//---- full-stats validity; in a bin where a chunk is not usable it takes the full-stats value -- the
	//---- fraction of such fill-ins is logged). dC2 = C2C - C2 = what the crossing correction adds.
	const double DYCUTPAGE		= 0.1;		// as corral_m's crossing page
	const double DPHICUTPAGE	= 30.;
	struct Phys {
		TH1D	*R2yy;							// R2(dy) from R2(y1,y2)
		TH1D	*pU[2], *pC[2], *nU[2], *nC[2];	// [0] R2(dy), [1] R2(dphi): whole range / narrow window, uncorrected / corrected
		TH2D	*dC2;							// C2C - C2 (full stats; bins valid in both)
		TH1D	*dC2dy, *dC2dphi;				// its sums over dphi / over dy
		double	I, eI, Sfull, frac, efrac;		// integral of dC2 (pairs per event), sum rho2(S), I/S; subgroup errors
		std::vector<double>	Ic, fracc, efracc;	// per chunk
		TH1D	*hy1, *hy2;						// nevt-weighted averages over chunks
		double	fillin;							// fraction of chunk-bin values taken from full stats in the projections
		TH1D	*CQ, *CQ5;						// C(Q) at 1 MeV (written) and 5 MeV (display), ratio of zvtx sums
		TH2D	*CQKT, *CQKT10;					// C(Q,kT) at 2 MeV and 10 MeV (display)
		TH1D	*MinvS, *MinvM;					// hMinvFS/FM plain sums (counts)
		Phys(){ R2yy=0; for (int k=0;k<2;k++){ pU[k]=pC[k]=nU[k]=nC[k]=0; } dC2=0; dC2dy=dC2dphi=0; I=eI=Sfull=frac=efrac=0; hy1=hy2=0; fillin=0; CQ=CQ5=0; CQKT=CQKT10=0; MinvS=MinvM=0; }
	};
	std::vector<Phys> ph(NPairTypes);
	std::vector<long> fillN(NPairTypes,0), fillT(NPairTypes,0);
	std::vector<int> chunkRun(nlists,-1);
	{
		std::ifstream fs((PROJ+"lists/"+SET+"/lists_summary.txt").Data());
		std::string line; int nread=0;
		while (std::getline(fs,line)){
			std::istringstream is(line); int il=-1, run=-1; std::string tok;
			if (!(is>>il>>tok>>run) || tok!="run") continue;
			if (il>=0 && il<nlists){ chunkRun[il] = run; ++nread; }
		}
		cout<<"corral::Finalize -- step 1b: runs for "<<nread<<" of "<<nlists<<" chunks from lists_summary.txt"<<endl;
	}
	//
	for (int ipaty=0;ipaty<NPairTypes;ipaty++){
		//
		//---- plain sums (non-CF names)
		for (int ih=0;ih<NFINALIZEHISTS;ih++){
			TString base	= TString(FinalizeHists[ih]);
			int kd,ir;
			if (ParseCF(base,kd,ir)) continue;
			TString name	= Form("%s_%d",base.Data(),ipaty);
			TH1 *hsum		= 0;
			for (int ic=0;ic<nlists;ic++){
				TH1 *h	= (TH1*)fch[ic]->Get(name.Data());
				if (!h){ cout<<"corral::Finalize -- "<<name<<" missing in chunk "<<ic<<", exit"<<endl; exit(1); }
				if (!hsum){ hsum = (TH1*)h->Clone(name.Data()); hsum->SetDirectory(0); }
				else        hsum->Add(h);
			}
			result[name]	= hsum;
		}
		//---- hy1, hy2 are per event (dN/dy): average over chunks weighted by the chunk's events (hmult entries)
		for (int iyh=0;iyh<2;iyh++){
			TH1D *avg	= 0; double sw = 0.;
			for (int ic=0;ic<nlists;ic++){
				TH1D *h		= (TH1D*)fch[ic]->Get(Form("hy%d_%d",iyh+1,ipaty));
				TH1 *hm		= (TH1*)fch[ic]->Get(Form("hmult_%d",ipaty));
				if (!h || !hm){ cout<<"corral::Finalize -- hy"<<iyh+1<<"_"<<ipaty<<" or hmult missing in chunk "<<ic<<", exit"<<endl; exit(1); }
				double w	= hm->GetEntries();
				if (!avg){ avg = (TH1D*)h->Clone(Form("hy%d_%d",iyh+1,ipaty)); avg->SetDirectory(0); avg->Reset(); }
				avg->Add(h,w); sw += w;
				delete h; delete hm;
			}
			if (sw>0.) avg->Scale(1./sw);
			if (iyh==0) ph[ipaty].hy1 = avg; else ph[ipaty].hy2 = avg;
		}
		if (!anyCF) continue;
		//
		//---- per-chunk, per-zvtx-bin bookkeeping: nevt (hnevtz), rho2(M) normalization (hdenomz), hzvtx
		TH1D *hn0	= (TH1D*)fch[0]->Get(Form("hnevtz_%d",ipaty));
		if (!hn0){ cout<<"corral::Finalize -- hnevtz_"<<ipaty<<" missing (chunks made before README_Finalize step 1b?), exit"<<endl; exit(1); }
		const int NZ	= hn0->GetNbinsX();
		if (NZ>64){ cout<<"corral::Finalize -- NZ="<<NZ<<" > 64, exit"<<endl; exit(1); }
		std::vector<std::vector<double>> nc(nlists,std::vector<double>(NZ,0.)), dc(nlists,std::vector<double>(NZ,0.));
		std::vector<std::vector<double>> zc(nlists,std::vector<double>(NZ,0.));
		std::vector<double> zentc(nlists,0.), nevc(nlists,0.);
		for (int ic=0;ic<nlists;ic++){
			TH1D *hn	= (TH1D*)fch[ic]->Get(Form("hnevtz_%d",ipaty));
			TH1D *hd	= (TH1D*)fch[ic]->Get(Form("hdenomz_%d",ipaty));
			TH1D *hz	= (TH1D*)fch[ic]->Get(Form("hzvtx_%d",ipaty));
			for (int z=0;z<NZ;z++){
				nc[ic][z]	= hn->GetBinContent(z+1);
				dc[ic][z]	= hd->GetBinContent(z+1);
				zc[ic][z]	= hz->GetBinContent(z+1);
				nevc[ic]	+= nc[ic][z];
			}
			zentc[ic]	= hz->GetEntries();
			delete hn; delete hd; delete hz;
		}
		double nevtAll	= 0.;	// events of this pairtype, all chunks (sum of hnevtz)
		for (int ic=0;ic<nlists;ic++) nevtAll += nevc[ic];
		//---- C(Q) (README_Finalize step 4; user 2026-09-26: RATIO OF SUMS over zvtx bins, standard femtoscopy):
		//----   the mixed denominator is still formed within each zvtx bin, in expected-sibling-pair units:
		//----   D_z = (sum_c df_c*Qmix_c,z) * N_z/ND_z  (Qmix in the chunk files is already /df_c; N_z = sum nevt,
		//----   ND_z = sum nevt*df over the chunks where z was computed), C(Q) = sum_z Qsib_z / sum_z D_z.
		//----   Chunk pieces S_c = sum_z Qsib_c,z and D_c = sum_z df_c*Qmix_c,z*N_z/ND_z add up exactly, so the
		//----   error is the ratio-estimator subgroup error sigma^2 = n/(n-1) sum_c (S_c - C D_c)^2 / (sum_c D_c)^2
		//----   (no per-chunk ratio: sparse chunks are fine). S=0 over D>0: valid zero, one-pair error 1/D.
		{
			Phys &P	= ph[ipaty];
			double Nz[64], NDz[64]; bool okq[64];
			for (int z=0;z<NZ;z++){
				Nz[z]=0.; NDz[z]=0.;
				for (int ic=0;ic<nlists;ic++) if (nc[ic][z]>=2. && dc[ic][z]>0.){ Nz[z] += nc[ic][z]; NDz[z] += nc[ic][z]*dc[ic][z]; }
				okq[z]	= (Nz[z]>=2. && NDz[z]>0.);
			}
			auto buildCQ	= [&](const char* bs, const char* bm, int rebin, const char* outname, const char* title) -> TH1* {
				TH1 *t0	= (TH1*)fch[0]->Get(Form("%s_z00_%d",bs,ipaty));
				if (!t0){ cout<<"corral::Finalize -- "<<bs<<"_z00_"<<ipaty<<" missing, exit"<<endl; exit(1); }
				TH1 *out	= (TH1*)t0->Clone(outname); out->SetDirectory(0); delete t0;
				if (rebin>1){ if (out->GetDimension()==2) ((TH2*)out)->RebinX(rebin); else out->Rebin(rebin); }
				out->Reset(); out->SetTitle(title);
				int nce	= out->GetNcells();
				std::vector<std::vector<double>> Sc(nlists,std::vector<double>(nce,0.)), Dc(nlists,std::vector<double>(nce,0.));
				for (int ic=0;ic<nlists;ic++){
					for (int z=0;z<NZ;z++){
						if (!okq[z] || !(nc[ic][z]>=2. && dc[ic][z]>0.)) continue;
						TH1 *hs	= (TH1*)fch[ic]->Get(Form("%s_z%02d_%d",bs,z,ipaty));
						TH1 *hm	= (TH1*)fch[ic]->Get(Form("%s_z%02d_%d",bm,z,ipaty));
						if (!hs || !hm){ cout<<"corral::Finalize -- "<<bs<<"/"<<bm<<"_z"<<z<<"_"<<ipaty<<" missing in chunk "<<ic<<", exit"<<endl; exit(1); }
						hs->SetDirectory(0); hm->SetDirectory(0);
						if (rebin>1){
							if (hs->GetDimension()==2){ ((TH2*)hs)->RebinX(rebin); ((TH2*)hm)->RebinX(rebin); }
							else { hs->Rebin(rebin); hm->Rebin(rebin); }
						}
						double w	= dc[ic][z]*Nz[z]/NDz[z];
						for (int ib=0;ib<nce;ib++){ Sc[ic][ib] += hs->GetBinContent(ib); Dc[ic][ib] += w*hm->GetBinContent(ib); }
						delete hs; delete hm;
					}
				}
				for (int ib=0;ib<nce;ib++){
					double S=0, D=0;
					for (int ic=0;ic<nlists;ic++){ S += Sc[ic][ib]; D += Dc[ic][ib]; }
					if (D<=0.){ out->SetBinContent(ib,0.); out->SetBinError(ib,0.); continue; }
					double C	= S/D;
					double v	= 0.;
					for (int ic=0;ic<nlists;ic++) v += pow(Sc[ic][ib] - C*Dc[ic][ib],2);
					double e	= sqrt(v*nlists/(nlists-1.))/D;
					if (S<=0.) e = 1./D;
					out->SetBinContent(ib,C); out->SetBinError(ib,e);
				}
				extraW.push_back(out);
				return out;
			};
			TString pnm	= Form("%s%s",ParticleIDNames[PairTypes_Info[ipaty][0]],ParticleIDNames[PairTypes_Info[ipaty][1]]);
			P.CQ	= (TH1D*)buildCQ("hQsib","hQmix",1,Form("hFin_CQ_%d",ipaty),    Form("%s, C(Q) = #Sigma_{z}sib / #Sigma_{z}mix (1 MeV bins);Q_{inv} (GeV);C(Q)",pnm.Data()));
			P.CQ5	= (TH1D*)buildCQ("hQsib","hQmix",5,Form("hFin_CQ5_%d",ipaty),   Form("%s, C(Q) (5 MeV bins, #pm subgroup err);Q_{inv} (GeV);C(Q)",pnm.Data()));
			P.CQKT	= (TH2D*)buildCQ("hQsibKT","hQmixKT",1,Form("hFin_CQKT_%d",ipaty),  Form("%s, C(Q,k_{T});Q_{inv} (GeV);k_{T} (GeV)",pnm.Data()));
			P.CQKT10= (TH2D*)buildCQ("hQsibKT","hQmixKT",5,Form("hFin_CQKT10_%d",ipaty),Form("%s, C(Q,k_{T}) (10 MeV bins);Q_{inv} (GeV);k_{T} (GeV)",pnm.Data()));
			//---- Minv near threshold: plain sums (counts)
			for (int im=0;im<2;im++){
				TH1D *sum	= 0;
				for (int ic=0;ic<nlists;ic++){
					TH1D *h	= (TH1D*)fch[ic]->Get(Form("%s_%d",im==0?"hMinvFS":"hMinvFM",ipaty));
					if (!h) continue;
					if (!sum){ sum = (TH1D*)h->Clone(Form("hFin_%s_%d",im==0?"MinvFS":"MinvFM",ipaty)); sum->SetDirectory(0); }
					else sum->Add(h);
					delete h;
				}
				if (im==0) P.MinvS = sum; else P.MinvM = sum;
				if (sum) extraW.push_back(sum);
			}
			cout<<Form("corral::Finalize -- C(Q) pairtype %2d: ratio of zvtx sums built (%d zvtx bins used)",ipaty,(int)std::count(okq,okq+NZ,true))<<endl;
		}
		std::vector<int> allc(nlists);
		for (int ic=0;ic<nlists;ic++) allc[ic] = ic;
		//
		const double nexpmin	= nexpOverride ? nexpGlobal : (double)PairTypes_Info[ipaty][2];
		cout<<"corral::Finalize -- pairtype "<<ipaty<<": N_min = "<<nexpmin<<endl;
		int chg1	= ParticleCharge[PairTypes_Info[ipaty][0]];
		int chg2	= ParticleCharge[PairTypes_Info[ipaty][1]];
		int ds		= CrossingDirtySide(FINALIZE_FIELD,chg1,chg2);
		//
		for (int ir2=0;ir2<3;ir2++){
			//---- does the list want anything from this ir2?
			bool want	= false;
			for (int ih=0;ih<NFINALIZEHISTS;ih++){ int kd,ir; if (ParseCF(TString(FinalizeHists[ih]),kd,ir) && ir==ir2) want = true; }
			if (!want) continue;
			const int NK	= (ir2==1) ? 7 : 4;
			//
			//---- cache this pairtype's per-zvtx-bin maps: read once, used by the full chain and all subgroups
			std::vector<std::vector<TH2D*>> cS(nlists,std::vector<TH2D*>(NZ,(TH2D*)0)), cM(nlists,std::vector<TH2D*>(NZ,(TH2D*)0));
			for (int ic=0;ic<nlists;ic++){
				for (int z=0;z<NZ;z++){
					if (!(nc[ic][z]>=2. && dc[ic][z]>0.)) continue;
					cS[ic][z]	= (TH2D*)fch[ic]->Get(Form("hrho2_%d_z%02d_%d"    ,ir2,z,ipaty));
					cM[ic][z]	= (TH2D*)fch[ic]->Get(Form("hrho1rho1_%d_z%02d_%d",ir2,z,ipaty));
					if (!cS[ic][z] || !cM[ic][z]){ cout<<"corral::Finalize -- per-zvtx maps missing in chunk "<<ic<<", exit"<<endl; exit(1); }
					cS[ic][z]->SetDirectory(0); cM[ic][z]->SetDirectory(0);
				}
			}
			TH2D *tmpl	= (TH2D*)fch[0]->Get(Form("hrho2_%d_z%02d_%d",ir2,0,ipaty));	// binning template
			if (!tmpl){ cout<<"corral::Finalize -- hrho2_"<<ir2<<"_z00_"<<ipaty<<" missing in chunk 0, exit"<<endl; exit(1); }
			tmpl	= (TH2D*)tmpl->Clone(Form("tmpl_%d_%d",ir2,ipaty)); tmpl->SetDirectory(0); tmpl->Reset();
			//
			//---- the CF chain on a set of chunks (all = full stats, step 2; one = a subgroup, step 3)
			//----   own mode (fx==0): zvtx weights, zvtx-bin use and validity masks from these chunks;
			//----     VZ[z] = 1 where rho2(M)>0 and N_exp = nevt*rho2(M) >= nmin (nmin=0: CalcRm's M>0 rule)
			//----   fixed mode (fx = the full-stats set): the full-stats weights and masks VZ/MK, so a
			//----     subgroup is the same linear combination as the full-stats value. out.V/VC then say
			//----     where this chunk is USABLE: every zvtx bin (and, corrected, every source bin) that the
			//----     full-stats value uses is defined in this chunk (computed, rho2(M)>0).
			auto chain	= [&](const std::vector<int>& chs, bool report, const TString& tag, double nmin, const CFSet* fx, bool keepMasks) -> CFSet {
				double N[64], ND[64], wz[64]; bool okz[64], okn[64];
				double zent	= 0.;
				for (int z=0;z<NZ;z++){ N[z]=0.; ND[z]=0.; wz[z]=0.; }
				for (int ic : chs){
					zent	+= zentc[ic];
					for (int z=0;z<NZ;z++){
						wz[z]	+= zc[ic][z];
						if (nc[ic][z]>=2. && dc[ic][z]>0.){ N[z] += nc[ic][z]; ND[z] += nc[ic][z]*dc[ic][z]; }
					}
				}
				for (int z=0;z<NZ;z++){ wz[z] = (zent>0.) ? wz[z]/zent : 0.; okn[z] = (N[z]>=2. && ND[z]>0.); okz[z] = okn[z]; }
				if (fx) for (int z=0;z<NZ;z++){ wz[z] = fx->wz[z]; okz[z] = fx->okz[z]; }
				std::vector<TH2D*> S(NZ,0), M(NZ,0), C2(NZ,0), R2(NZ,0), SC(NZ,0), C2C(NZ,0), R2C(NZ,0), MK(NZ,0), VZ(NZ,0), BADU(NZ,0), BADC(NZ,0);
				long nBelow=0, nBelowS=0;		// bin-zvtx with rho2(M)>0 but N_exp<nmin (own mode)
				for (int z=0;z<NZ;z++){
					S[z]	= (TH2D*)tmpl->Clone(Form("S%d_%d_%d%s",ir2,z,ipaty,tag.Data())); S[z]->SetDirectory(0);
					M[z]	= (TH2D*)tmpl->Clone(Form("M%d_%d_%d%s",ir2,z,ipaty,tag.Data())); M[z]->SetDirectory(0);
					for (int ic : chs){
						if (!cS[ic][z]) continue;
						S[z]->Add(cS[ic][z], nc[ic][z]);
						M[z]->Add(cM[ic][z], nc[ic][z]*dc[ic][z]);
					}
					if (okn[z]){ S[z]->Scale(1./N[z]); M[z]->Scale(1./ND[z]); }
					//---- validity mask of this zvtx bin
					if (fx){
						VZ[z]	= (TH2D*)fx->vz[z]->Clone(Form("VZ_%d_%d_%d%s",ir2,z,ipaty,tag.Data())); VZ[z]->SetDirectory(0);
					} else {
						VZ[z]	= (TH2D*)tmpl->Clone(Form("VZ_%d_%d_%d%s",ir2,z,ipaty,tag.Data())); VZ[z]->SetDirectory(0);
						for (int ib=0;ib<VZ[z]->GetNcells();ib++){
							double m	= M[z]->GetBinContent(ib);
							if (!okn[z] || m<=0.) continue;
							if (N[z]*m>=nmin) VZ[z]->SetBinContent(ib,1.);
							else if (!VZ[z]->IsBinUnderflow(ib) && !VZ[z]->IsBinOverflow(ib)){ ++nBelow; if (S[z]->GetBinContent(ib)>0.) ++nBelowS; }
						}
					}
					//---- fixed mode: where the full-stats value uses this zvtx bin but this chunk has nothing
					if (fx){
						BADU[z]	= (TH2D*)tmpl->Clone(Form("BADU_%d_%d_%d%s",ir2,z,ipaty,tag.Data())); BADU[z]->SetDirectory(0);
						for (int ib=0;ib<BADU[z]->GetNcells();ib++){
							if (VZ[z]->GetBinContent(ib)<=0.) continue;
							if (!okn[z] || M[z]->GetBinContent(ib)<=0.) BADU[z]->SetBinContent(ib,1.);
						}
					}
					//---- C2 and R2 per zvtx bin: CalcRm::Calculate's rules (defined iff M>0; S=0 is a valid zero)
					C2[z]	= (TH2D*)S[z]->Clone(Form("C2_%d_%d_%d%s",ir2,z,ipaty,tag.Data())); C2[z]->SetDirectory(0); C2[z]->Reset();
					R2[z]	= (TH2D*)S[z]->Clone(Form("R2_%d_%d_%d%s",ir2,z,ipaty,tag.Data())); R2[z]->SetDirectory(0); R2[z]->Reset();
					if (okn[z]){
						for (int ibin=1;ibin<=S[z]->GetNbinsX();ibin++){
							for (int jbin=1;jbin<=S[z]->GetNbinsY();jbin++){
								double valn	= S[z]->GetBinContent(ibin,jbin), valne = S[z]->GetBinError(ibin,jbin);
								double vald	= M[z]->GetBinContent(ibin,jbin), valde = M[z]->GetBinError(ibin,jbin);
								double valC2,valC2e,valR2,valR2e;
								if (vald>0. && valn<=0.){
									double valne1	= 1./N[z];
									valC2	= -vald;	valC2e	= sqrt(valne1*valne1 + valde*valde);
									valR2	= -1.;		valR2e	= valne1/vald;
								} else if (valn>0. && vald>0.){
									valC2	= valn - vald;	valC2e	= sqrt(valne*valne + valde*valde);
									valR2	= valn/vald;	valR2e	= valR2*sqrt(pow(valde/vald,2)+pow(valne/valn,2));
									valR2	-= 1.;
								} else {
									valC2	= 0.; valC2e = 0.; valR2 = -1.; valR2e = 0.;
									//---- fixed mode (a subgroup): C2 = S - M stays linear where this chunk has M=0 but
									//---- the full-stats mask uses the bin, so S, M, C2, C2C of a chunk are defined
									//---- everywhere (only R2 needs the chunk to be usable)
									if (fx){ valC2 = valn - vald; valC2e = sqrt(valne*valne + valde*valde); }
								}
								C2[z]->SetBinContent(ibin,jbin,valC2); C2[z]->SetBinError(ibin,jbin,valC2e);
								R2[z]->SetBinContent(ibin,jbin,valR2); R2[z]->SetBinError(ibin,jbin,valR2e);
							}
						}
					}
					//---- crossing correction per zvtx bin (ir2=1), as in Calculate; validity = the VZ mask
					if (ir2==1){
						TH2D* src[3]	= {S[z], C2[z], R2[z]};
						TH2D** dst[3]	= {&SC[z], &C2C[z], &R2C[z]};
						for (int k=0;k<3;k++){
							*dst[k]	= (TH2D*)src[k]->Clone(Form("%s_C",src[k]->GetName())); (*dst[k])->SetDirectory(0);
							if (okz[z]) CrossingCorrect(*dst[k],ds,VZ[z]);
						}
						if (fx){
							MK[z]	= (TH2D*)fx->mk[z]->Clone(Form("MK_%d_%d%s",z,ipaty,tag.Data())); MK[z]->SetDirectory(0);
							//---- a corrected bin draws on itself and/or its dphi mirror exactly as a map does: correct
							//---- the flags with an all-valid mask (near: dirty takes the clean's flag; away: either flags both)
							BADC[z]	= (TH2D*)BADU[z]->Clone(Form("BADC_%d_%d%s",z,ipaty,tag.Data())); BADC[z]->SetDirectory(0);
							TH2D *ones	= (TH2D*)tmpl->Clone("ones_tmp"); ones->SetDirectory(0);
							for (int ib=0;ib<ones->GetNcells();ib++) ones->SetBinContent(ib,1.);
							if (okz[z]) CrossingCorrect(BADC[z],ds,ones);
							delete ones;
						} else {
							MK[z]	= (TH2D*)VZ[z]->Clone(Form("MK_%d_%d%s",z,ipaty,tag.Data())); MK[z]->SetDirectory(0);
							if (okz[z]) CrossingCorrect(MK[z],ds,MK[z]);
						}
					}
				}
				//---- empty-denominator report at full statistics (the squawk belongs here, README_Crossing sec 0)
				if (report){
					long nMz=0, nSMz=0; int nbinAny=0;
					for (int ib=0;ib<S[0]->GetNcells();ib++){
						if (S[0]->IsBinUnderflow(ib) || S[0]->IsBinOverflow(ib)) continue;
						int nzb=0;
						for (int z=0;z<NZ;z++){
							if (!okz[z] || wz[z]<=0.) continue;
							if (M[z]->GetBinContent(ib)>0.) continue;
							++nzb; ++nMz; if (S[z]->GetBinContent(ib)>0.) ++nSMz;
						}
						if (nzb>0) ++nbinAny;
					}
					cout<<Form("corral::Finalize -- pairtype %2d ir2=%d: %d bins have rho2(M)=0 in >=1 used zvtx bin (%ld bin-zvtx, %ld of them with rho2(S)>0); %ld more bin-zvtx below N_exp>=%g (%ld with rho2(S)>0) left out",
						ipaty,ir2,nbinAny,nMz,nSMz,nBelow,nmin,nBelowS)<<endl;
				}
				//---- Zvtx averages (masks: VZ uncorrected, MK corrected)
				auto zavg	= [&](std::vector<TH2D*>& v, std::vector<TH2D*>& mk, double sentinel, const char* nm){
					TH2D *avg	= (TH2D*)v[0]->Clone(nm); avg->SetDirectory(0);
					std::vector<TH1*> sl(NZ), ms(NZ);
					for (int z=0;z<NZ;z++){ sl[z] = v[z]; ms[z] = mk[z]; }
					ZvtxAverageValid(avg,sl.data(),ms.data(),wz,okz,NZ,sentinel);
					return avg;
				};
				//---- validity of the average: some used zvtx bin valid; fixed mode also: no used zvtx bin BAD
				auto zvalid	= [&](std::vector<TH2D*>& mk, std::vector<TH2D*>& bad, const char* nm){
					TH2D *hv	= (TH2D*)tmpl->Clone(nm); hv->SetDirectory(0); hv->Reset();
					for (int ib=0;ib<hv->GetNcells();ib++){
						bool any=false, isbad=false;
						for (int z=0;z<NZ;z++){
							if (!okz[z] || wz[z]<=0. || mk[z]->GetBinContent(ib)<=0.) continue;
							any	= true;
							if (fx && bad[z]->GetBinContent(ib)>0.) isbad = true;
						}
						if (any && !isbad) hv->SetBinContent(ib,1.);
					}
					return hv;
				};
				CFSet out;
				std::vector<TH2D*>* src[7]	= {&S,&M,&C2,&R2,&SC,&C2C,&R2C};
				for (int k=0;k<NK;k++){
					double sentinel	= (k==3 || k==6) ? -1. : 0.;
					out.h[k]	= zavg(*src[k], (k<4) ? VZ : MK, sentinel, Form("%s_%d%s",Form(KINDNAME[k],ir2),ipaty,tag.Data()));
				}
				out.V	= zvalid(VZ, BADU, Form("V_%d_%d%s",ir2,ipaty,tag.Data()));
				if (ir2==1) out.VC	= zvalid(MK, BADC, Form("VC_%d_%d%s",ir2,ipaty,tag.Data()));
				if (keepMasks){
					out.vz	= VZ; out.mk = MK;
					for (int z=0;z<NZ;z++){ out.wz[z] = wz[z]; out.okz[z] = okz[z]; }
					VZ.assign(NZ,0); MK.assign(NZ,0);
				}
				for (int z=0;z<NZ;z++){ delete S[z]; delete M[z]; delete C2[z]; delete R2[z]; delete SC[z]; delete C2C[z]; delete R2C[z]; delete MK[z]; delete VZ[z]; delete BADU[z]; delete BADC[z]; }
				return out;
			};
			//
			//---- step 2: full statistics, with the N_exp>=nmin validity; its masks and weights are kept
			CFSet full	= chain(allc, true, TString(""), nexpmin, 0, true);
			double chk1d[7]={0,0,0,0,0,0,0}, chk1e[7]={0,0,0,0,0,0,0}; int chk1miss[7]={0,0,0,0,0,0,0};
			//---- check 1: a one-chunk chain with the chunks' own rule (nmin=0) must reproduce that chunk's
			//---- saved Zvtx averages from CalcRm::Calculate
			for (int ic=0;ic<nlists;ic++){
				CFSet own	= chain(std::vector<int>(1,ic), false, TString(Form("_own%02d",ic)), 0., 0, false);
				for (int k=0;k<NK;k++){
					TString nm	= Form("%s_%d",Form(KINDNAME[k],ir2),ipaty);
					TH2D *hc	= (TH2D*)fch[ic]->Get(nm.Data());
					if (!hc){ ++chk1miss[k]; continue; }
					for (int ib=0;ib<hc->GetNcells();ib++){
						if (hc->IsBinUnderflow(ib) || hc->IsBinOverflow(ib)) continue;
						chk1d[k]	= std::max(chk1d[k], fabs(own.h[k]->GetBinContent(ib) - hc->GetBinContent(ib)));
						chk1e[k]	= std::max(chk1e[k], fabs(own.h[k]->GetBinError(ib)   - hc->GetBinError(ib)));
					}
					delete hc;
				}
				own.Delete();
			}
			for (int k=0;k<NK;k++){
				cout<<Form("corral::Finalize -- step 3 check 1 %-16s one-chunk chain (own rule, N_exp>=0) vs the chunk's own map: max |diff| content %.2e error %.2e (%d chunks missing it)",
					Form("%s_%d",Form(KINDNAME[k],ir2),ipaty),chk1d[k],chk1e[k],chk1miss[k])<<endl;
				chk1d[k]=0.; chk1e[k]=0.; chk1miss[k]=0;
			}
			//---- check 2: the fixed-mask chain on ALL chunks must reproduce the full-stats result exactly
			{
				CFSet fx2	= chain(allc, false, TString("_fx2"), nexpmin, &full, false);
				int nd=0;
				for (int k=0;k<NK;k++){
					for (int ib=0;ib<fx2.h[k]->GetNcells();ib++){
						if (fx2.h[k]->GetBinContent(ib)!=full.h[k]->GetBinContent(ib) || fx2.h[k]->GetBinError(ib)!=full.h[k]->GetBinError(ib)) ++nd;
					}
				}
				for (int ib=0;ib<full.V->GetNcells();ib++){
					if (fx2.V->GetBinContent(ib)!=full.V->GetBinContent(ib)) ++nd;
					if (ir2==1 && fx2.VC->GetBinContent(ib)!=full.VC->GetBinContent(ib)) ++nd;
				}
				cout<<Form("corral::Finalize -- step 3 check 2 pairtype %2d ir2=%d: fixed-mask chain on all chunks vs full: %d differing cells (expect 0)",ipaty,ir2,nd)<<endl;
				fx2.Delete();
			}
			//---- step 3: each chunk alone, with the full-stats masks and weights
			std::vector<CFSet> sub(nlists);
			for (int ic=0;ic<nlists;ic++) sub[ic] = chain(std::vector<int>(1,ic), false, TString(Form("_sg%02d",ic)), nexpmin, &full, false);
			//
			//---- step 3: subgroup errors on the full-stats values
			for (int k=0;k<NK;k++){
				TH2D *hf	= full.h[k];
				TH2D *vf	= (k<4) ? full.V : full.VC;
				std::vector<double> rsp, rw;	// sg/per-run-style error, weighted/unweighted error
				int nlow=0, nset=0;
				for (int ib=0;ib<hf->GetNcells();ib++){
					if (hf->IsBinUnderflow(ib) || hf->IsBinOverflow(ib)) continue;
					if (vf->GetBinContent(ib)<=0.) continue;			// not valid at full stats: sentinel, error 0
					double sx=0, sw=0, swx=0; int n=0;
					std::vector<double> xv, wv;
					for (int ic=0;ic<nlists;ic++){
						TH2D *vs	= (k<4) ? sub[ic].V : sub[ic].VC;
						bool linear	= (k!=3 && k!=6);				// S, M, C2, rho2C, C2C: defined in every chunk (fixed mode)
						if (!linear && vs->GetBinContent(ib)<=0.) continue;	// pick (a), R2/R2C only: undefined in this chunk -> skipped
						double x	= sub[ic].h[k]->GetBinContent(ib);
						xv.push_back(x); wv.push_back(nevc[ic]);
						sx += x; sw += nevc[ic]; swx += nevc[ic]*x; ++n;
					}
					double eold	= hf->GetBinError(ib);
					double err	= 0.;
					if (n>=NVALIDMIN){
						double m	= sx/n, mw = swx/sw, s2 = 0., sw2d2 = 0.;
						for (size_t j=0;j<xv.size();j++){ s2 += (xv[j]-m)*(xv[j]-m); sw2d2 += wv[j]*wv[j]*(xv[j]-mw)*(xv[j]-mw); }
						err	= sqrt(s2/(n-1.))/sqrt((double)n);
						double errw	= sqrt(sw2d2)/sw*sqrt(n/(n-1.));	// pick (b): event-weighted version, printed only
						if (err>0.){ rw.push_back(errw/err); if (eold>0.) rsp.push_back(err/eold); }
						++nset;
					} else {
						++nlow;
					}
					hf->SetBinError(ib,err);
				}
				cout<<Form("corral::Finalize -- step 3 %-16s subgroup errors in %d bins, %d valid bins with N_valid<%d -> error 0; median sg/per-run-style error %.3f, median weighted/unweighted %.4f",
					hf->GetName(),nset,nlow,NVALIDMIN,Median(rsp),Median(rw))<<endl;
			}
			//---- N_valid maps (same for kinds 0-3; kinds 4-6 use the corrected validity)
			for (int iv=0;iv<((ir2==1)?2:1);iv++){
				TH2D *hnv	= (TH2D*)tmpl->Clone(Form("%s_%d_%d",(iv==0)?"hNvalid":"hNvalidC",ir2,ipaty)); hnv->SetDirectory(0); hnv->Reset();
				hnv->SetTitle(Form("%s: N_{valid} subgroups%s;%s;%s",hnv->GetName(),(iv==0)?"":" (crossing-corrected)",tmpl->GetXaxis()->GetTitle(),tmpl->GetYaxis()->GetTitle()));
				for (int ic=0;ic<nlists;ic++) hnv->Add((iv==0) ? sub[ic].V : sub[ic].VC);
				extra.push_back(hnv);
			}
			//
			//---- physics pages: projections with subgroup errors (ir2=0: R2yy(dy); ir2=1: R2(dy), R2(dphi),
			//---- whole and narrow window, uncorrected and corrected) and dC2 = C2C - C2 (ir2=1)
			{
				Phys &P		= ph[ipaty];
				TAxis *ax	= tmpl->GetXaxis(), *ay = tmpl->GetYaxis();
				int nx		= ax->GetNbins(), ny = ay->GetNbins();
				//---- mean over the full-stats-valid bins mapped to each output bin; the same inside every chunk
				//---- (a chunk bin that is not usable takes the full-stats value); error = s/sqrt(N_chunks)
				auto project	= [&](int kind, bool corr, auto outbin, TH1D* hout){
					TH2D *vfull	= corr ? full.VC : full.V;
					int nb		= hout->GetNbinsX();
					std::vector<double> vf(nb+2,0.), nf(nb+2,0.);
					std::vector<std::vector<double>> vc(nlists,std::vector<double>(nb+2,0.));
					for (int ix=1;ix<=nx;ix++) for (int iy=1;iy<=ny;iy++){
						if (vfull->GetBinContent(ix,iy)<=0.) continue;
						int ob	= outbin(ix,iy);
						if (ob<1 || ob>nb) continue;
						double f	= full.h[kind]->GetBinContent(ix,iy);
						vf[ob] += f; nf[ob] += 1.;
						for (int ic=0;ic<nlists;ic++){
							bool u	= ((corr ? sub[ic].VC : sub[ic].V)->GetBinContent(ix,iy)>0.);
							vc[ic][ob]	+= u ? sub[ic].h[kind]->GetBinContent(ix,iy) : f;
							++fillT[ipaty]; if (!u) ++fillN[ipaty];
						}
					}
					for (int ob=1;ob<=nb;ob++){
						if (nf[ob]<=0.) continue;
						double s=0,s2=0;
						for (int ic=0;ic<nlists;ic++){ double x = vc[ic][ob]/nf[ob]; s += x; s2 += x*x; }
						double var	= (s2-s*s/nlists)/(nlists-1.);
						hout->SetBinContent(ob, vf[ob]/nf[ob]);
						hout->SetBinError(ob, sqrt(std::max(0.,var))/sqrt((double)nlists));
					}
				};
				if (ir2==0){
					TH1D *tyy	= (TH1D*)fch[0]->Get(Form("hR2yydy_%d",ipaty));
					if (!tyy){ cout<<"corral::Finalize -- hR2yydy_"<<ipaty<<" missing in chunk 0, exit"<<endl; exit(1); }
					P.R2yy	= (TH1D*)tyy->Clone(Form("hFin_R2yydy_%d",ipaty)); P.R2yy->SetDirectory(0); P.R2yy->Reset(); delete tyy;
					P.R2yy->SetTitle(Form("%s%s, R_{2}(dy) from R_{2}(y_{1},y_{2}) (#pm subgroup err);dy;R_{2}",
						ParticleIDNames[PairTypes_Info[ipaty][0]],ParticleIDNames[PairTypes_Info[ipaty][1]]));
					TAxis *ayy	= P.R2yy->GetXaxis();
					project(3,false,[&](int ix,int iy){ return ayy->FindFixBin(ax->GetBinCenter(ix)-ay->GetBinCenter(iy)); },P.R2yy);
					extraW.push_back(P.R2yy);
				}
				if (ir2==1){
					TString pn	= Form("%s%s",ParticleIDNames[PairTypes_Info[ipaty][0]],ParticleIDNames[PairTypes_Info[ipaty][1]]);
					std::vector<double> cwp(ny+1,0.);
					for (int iy=1;iy<=ny;iy++){ double c = ay->GetBinCenter(iy); while (c>=180.) c -= 360.; while (c<-180.) c += 360.; cwp[iy] = c; }
					double dycut	= std::max(DYCUTPAGE,   0.51*ax->GetBinWidth(1));
					double dphicut	= std::max(DPHICUTPAGE, 0.51*ay->GetBinWidth(1));
					for (int k=0;k<2;k++){
						TString axn	= (k==0) ? "dy" : "d#phi";
						TString cut	= (k==0) ? TString(Form("|d#phi|<%.0f#circ",dphicut)) : TString(Form("|dy|<%.2g",dycut));
						int nb		= (k==0) ? nx : ny;
						double lo_	= (k==0) ? ax->GetXmin() : ay->GetXmin(), hi_ = (k==0) ? ax->GetXmax() : ay->GetXmax();
						const char* xt	= (k==0) ? "dy" : "d#phi (deg)";
						P.pU[k]	= new TH1D(Form("hFin_R2%s_U_%d", k==0?"dy":"dphi",ipaty),Form("%s, R_{2}(%s) from R_{2}(dy,d#phi);%s;R_{2}",pn.Data(),axn.Data(),xt),nb,lo_,hi_);
						P.pC[k]	= new TH1D(Form("hFin_R2%s_C_%d", k==0?"dy":"dphi",ipaty),Form("%s, R_{2}(%s) from R_{2}(dy,d#phi);%s;R_{2}",pn.Data(),axn.Data(),xt),nb,lo_,hi_);
						P.nU[k]	= new TH1D(Form("hFin_R2%s_nU_%d",k==0?"dy":"dphi",ipaty),Form("%s, R_{2}(%s) from R_{2}(dy,d#phi), %s;%s;R_{2}",pn.Data(),axn.Data(),cut.Data(),xt),nb,lo_,hi_);
						P.nC[k]	= new TH1D(Form("hFin_R2%s_nC_%d",k==0?"dy":"dphi",ipaty),Form("%s, R_{2}(%s) from R_{2}(dy,d#phi), %s;%s;R_{2}",pn.Data(),axn.Data(),cut.Data(),xt),nb,lo_,hi_);
						TH1D *hh[4]	= {P.pU[k],P.pC[k],P.nU[k],P.nC[k]};
						for (int j=0;j<4;j++){ hh[j]->SetDirectory(0); extraW.push_back(hh[j]); }
					}
					project(3,false,[&](int ix,int iy){ return ix; },P.pU[0]);
					project(6,true ,[&](int ix,int iy){ return ix; },P.pC[0]);
					project(3,false,[&](int ix,int iy){ return iy; },P.pU[1]);
					project(6,true ,[&](int ix,int iy){ return iy; },P.pC[1]);
					project(3,false,[&](int ix,int iy){ return fabs(cwp[iy])<dphicut ? ix : -1; },P.nU[0]);
					project(6,true ,[&](int ix,int iy){ return fabs(cwp[iy])<dphicut ? ix : -1; },P.nC[0]);
					project(3,false,[&](int ix,int iy){ return fabs(ax->GetBinCenter(ix))<dycut ? iy : -1; },P.nU[1]);
					project(6,true ,[&](int ix,int iy){ return fabs(ax->GetBinCenter(ix))<dycut ? iy : -1; },P.nC[1]);
					//---- dC2 = C2C - C2 in bins valid in both; I = its integral = pairs per event added by the
					//---- correction; frac = I / sum rho2(S) over the valid bins. Per chunk: the same bins
					//---- C2 and C2C are linear in a chunk (fixed mode: C2 = S - M everywhere), so no fill-in.
					P.dC2	= (TH2D*)full.h[5]->Clone(Form("hFin_dC2_%d",ipaty)); P.dC2->SetDirectory(0); P.dC2->Reset();
					P.dC2->SetTitle(Form("%s, #DeltaC_{2} = C_{2C} - C_{2} (pairs added by the crossing correction);dy;d#phi (deg)",pn.Data()));
					P.dC2dy		= new TH1D(Form("hFin_dC2dy_%d",ipaty),  Form("%s, #Sigma_{d#phi} #DeltaC_{2} (#pm subgroup err);dy;pairs per event",pn.Data()),nx,ax->GetXmin(),ax->GetXmax());
					P.dC2dphi	= new TH1D(Form("hFin_dC2dphi_%d",ipaty),Form("%s, #Sigma_{dy} #DeltaC_{2} (#pm subgroup err);d#phi (deg);pairs per event",pn.Data()),ny,ay->GetXmin(),ay->GetXmax());
					P.dC2dy->SetDirectory(0); P.dC2dphi->SetDirectory(0);
					P.Ic.assign(nlists,0.); P.fracc.assign(nlists,0.); P.efracc.assign(nlists,0.);
					std::vector<double> Sc(nlists,0.), eI2c(nlists,0.);
					std::vector<std::vector<double>> cdy(nlists,std::vector<double>(nx+2,0.)), cdp(nlists,std::vector<double>(ny+2,0.));
					P.I = 0.; P.Sfull = 0.;
					for (int ix=1;ix<=nx;ix++) for (int iy=1;iy<=ny;iy++){
						if (full.V->GetBinContent(ix,iy)<=0.) continue;
						P.Sfull	+= full.h[0]->GetBinContent(ix,iy);
						for (int ic=0;ic<nlists;ic++) Sc[ic] += sub[ic].h[0]->GetBinContent(ix,iy);
						if (full.VC->GetBinContent(ix,iy)<=0.) continue;
						double d	= full.h[5]->GetBinContent(ix,iy) - full.h[2]->GetBinContent(ix,iy);
						P.dC2->SetBinContent(ix,iy,d);
						P.I	+= d;
						P.dC2dy->AddBinContent(ix,d); P.dC2dphi->AddBinContent(iy,d);
						for (int ic=0;ic<nlists;ic++){
							double dc	= sub[ic].h[5]->GetBinContent(ix,iy) - sub[ic].h[2]->GetBinContent(ix,iy);	// linear: no fill-in
							P.Ic[ic] += dc; cdy[ic][ix] += dc; cdp[ic][iy] += dc;
							eI2c[ic] += pow(sub[ic].h[5]->GetBinError(ix,iy),2);	// dirty bins: C2C error = the clean bin's
						}
					}
					P.frac	= (P.Sfull>0.) ? P.I/P.Sfull : 0.;
					double s=0,s2=0,sf=0,sf2=0;
					for (int ic=0;ic<nlists;ic++){
						double fr	= (Sc[ic]>0.) ? P.Ic[ic]/Sc[ic] : 0.;
						P.fracc[ic] = fr; P.efracc[ic] = (Sc[ic]>0.) ? sqrt(eI2c[ic])/Sc[ic] : 0.;
						s += P.Ic[ic]; s2 += P.Ic[ic]*P.Ic[ic]; sf += fr; sf2 += fr*fr;
					}
					P.eI	= sqrt(std::max(0.,(s2-s*s/nlists)/(nlists-1.)))/sqrt((double)nlists);
					P.efrac	= sqrt(std::max(0.,(sf2-sf*sf/nlists)/(nlists-1.)))/sqrt((double)nlists);
					for (int ix=1;ix<=nx;ix++){ double a=0,a2=0; for (int ic=0;ic<nlists;ic++){ a+=cdy[ic][ix]; a2+=cdy[ic][ix]*cdy[ic][ix]; }
						P.dC2dy->SetBinError(ix, sqrt(std::max(0.,(a2-a*a/nlists)/(nlists-1.)))/sqrt((double)nlists)); }
					for (int iy=1;iy<=ny;iy++){ double a=0,a2=0; for (int ic=0;ic<nlists;ic++){ a+=cdp[ic][iy]; a2+=cdp[ic][iy]*cdp[ic][iy]; }
						P.dC2dphi->SetBinError(iy, sqrt(std::max(0.,(a2-a*a/nlists)/(nlists-1.)))/sqrt((double)nlists)); }
					extraW.push_back(P.dC2); extraW.push_back(P.dC2dy); extraW.push_back(P.dC2dphi);
					cout<<Form("corral::Finalize -- dC2 pairtype %2d: integral %+.4e +- %.1e pairs/event (x %.0f events = %.4g pairs), sum rho2(S) %.4e, fraction %+.3e +- %.1e",
						ipaty,P.I,P.eI,nevtAll,P.I*nevtAll,P.Sfull,P.frac,P.efrac)<<endl;
				}
			}
			//---- step 1b: crossing loss / dphi-mirror asymmetry (dy,dphi only), full stats + every chunk
			if (ir2==1){
				XLoss &X	= xl[ipaty];
				X.have		= true; X.ds = ds; X.half = (ds!=0) ? ds : -1;
				TAxis *ax	= tmpl->GetXaxis(), *ay = tmpl->GetYaxis();
				int nx		= ax->GetNbins(), ny = ay->GetNbins();
				double lo	= ay->GetXmin();
				std::vector<int> jm(ny+1,0); std::vector<double> cw(ny+1,0.);
				for (int iy=1;iy<=ny;iy++){		// dphi mirror bin, same rule as CrossingCorrect.h
					double c	= ay->GetBinCenter(iy);
					while (c>= 180.) c -= 360.;
					while (c< -180.) c += 360.;
					cw[iy]		= c;
					double cm	= -c;
					while (cm<  lo      ) cm += 360.;
					while (cm>= lo+360. ) cm -= 360.;
					jm[iy]		= ay->FindFixBin(cm);
				}
				double dycut	= std::max(XLDYCUT, 0.51*ax->GetBinWidth(1));
				X.dycut		= dycut;
				auto inW	= [&](int w,int ix,int iy)->bool{
					double c	= cw[iy];
					if (c*X.half<=0.) return false;			// tested half only
					bool near	= fabs(c)<90.;
					if (w==2) return !near;
					if (!near) return false;
					return (w==0) || fabs(ax->GetBinCenter(ix))<dycut;
				};
				auto okF	= [&](int ix,int iy){ return full.V->GetBinContent(ix,iy)>0. && full.V->GetBinContent(ix,jm[iy])>0.; };
				//---- D from the Zvtx-averaged S and M: C2 = S - M in every valid zvtx bin and the average is linear
				//---- with the same masks and weights, so avg(C2) = avg(S) - avg(M) at full stats (checked below),
				//---- and a chunk's S and M are defined wherever the full-stats masks are (no chunk skipped)
				//---- DA = acceptance-corrected deficit: sum[S(mirror)*M(bin)/M(mirror) - S(bin)], M ratio from FULL
				//---- stats (precise, and keeps DA linear in a chunk's S) = sum M(bin)[R2(mirror)-R2(bin)] at full stats
				auto sums	= [&](const CFSet& cs, int w, double& D, double& eD2, double& Sc, double& DM, double& Mc, double& DA){
					D=0; eD2=0; Sc=0; DM=0; Mc=0; DA=0;
					for (int ix=1;ix<=nx;ix++) for (int iy=1;iy<=ny;iy++){
						if (!inW(w,ix,iy) || !okF(ix,iy)) continue;
						int jy	= jm[iy];
						double sj=cs.h[0]->GetBinContent(ix,jy), si=cs.h[0]->GetBinContent(ix,iy);
						double mj=cs.h[1]->GetBinContent(ix,jy), mi=cs.h[1]->GetBinContent(ix,iy);
						D	+= (sj-mj) - (si-mi);
						eD2	+= pow(cs.h[0]->GetBinError(ix,jy),2) + pow(cs.h[1]->GetBinError(ix,jy),2) + pow(cs.h[0]->GetBinError(ix,iy),2) + pow(cs.h[1]->GetBinError(ix,iy),2);
						Sc	+= sj;
						DM	+= mj - mi;
						Mc	+= mj;
						double mfj=full.h[1]->GetBinContent(ix,jy), mfi=full.h[1]->GetBinContent(ix,iy);
						if (mfj>0.) DA += sj*mfi/mfj - si;
					}
				};
				//---- check: avg(C2) == avg(S) - avg(M) at full stats over all valid bins
				{
					double dmx=0;
					for (int ix=1;ix<=nx;ix++) for (int iy=1;iy<=ny;iy++){
						if (full.V->GetBinContent(ix,iy)<=0.) continue;
						dmx	= std::max(dmx, fabs(full.h[2]->GetBinContent(ix,iy) - (full.h[0]->GetBinContent(ix,iy)-full.h[1]->GetBinContent(ix,iy))));
					}
					cout<<Form("corral::Finalize -- step 1b check pairtype %2d: max |avg(C2) - (avg(S)-avg(M))| = %.2e over valid bins",ipaty,dmx)<<endl;
				}
				for (int w=0;w<3;w++){
					double D,eD2,Sc,DM,Mc,DA;
					sums(full,w,D,eD2,Sc,DM,Mc,DA);
					X.f[w]	= (Sc>0.) ? D/Sc : 0.;
					X.fM[w]	= (Mc>0.) ? DM/Mc : 0.;
					X.fA[w]	= (Sc>0.) ? DA/Sc : 0.;
					X.fc[w].assign(nlists,0.); X.efc[w].assign(nlists,0.); X.fMc[w].assign(nlists,0.); X.fAc[w].assign(nlists,0.); X.usec[w].assign(nlists,false);
					double s=0,s2=0,sm=0,sm2=0,sa=0,sa2=0; int n=0;
					for (int ic=0;ic<nlists;ic++){
						sums(sub[ic],w,D,eD2,Sc,DM,Mc,DA);
						if (Sc<=0. || Mc<=0.) continue;
						X.usec[w][ic]	= true;
						X.fc[w][ic]		= D/Sc;  X.efc[w][ic] = sqrt(eD2)/Sc;  X.fMc[w][ic] = DM/Mc;  X.fAc[w][ic] = DA/Sc;
						s += D/Sc; s2 += pow(D/Sc,2); sm += DM/Mc; sm2 += pow(DM/Mc,2); sa += DA/Sc; sa2 += pow(DA/Sc,2); ++n;
					}
					X.nuse[w]	= n;
					if (n>=NVALIDMIN){
						X.ef[w]		= sqrt(std::max(0.,(s2-s*s/n)/(n-1.)))/sqrt((double)n);
						X.efM[w]	= sqrt(std::max(0.,(sm2-sm*sm/n)/(n-1.)))/sqrt((double)n);
						X.efA[w]	= sqrt(std::max(0.,(sa2-sa*sa/n)/(n-1.)))/sqrt((double)n);
					}
				}
				//---- check: on the near half, D = sum(C2C - C2) over the dirty bins (the correction copies the clean C2)
				{
					double DC=0, Sc=0;
					for (int ix=1;ix<=nx;ix++) for (int iy=1;iy<=ny;iy++){
						if (!inW(0,ix,iy) || !okF(ix,iy)) continue;
						DC	+= full.h[5]->GetBinContent(ix,iy) - full.h[2]->GetBinContent(ix,iy);
						Sc	+= full.h[0]->GetBinContent(ix,jm[iy]);
					}
					X.fC	= (Sc>0.) ? DC/Sc : 0.;
				}
				//---- maps: A = C2(mirror)-C2 (antisymmetric), and D per dy column over the near half
				X.A		= (TH2D*)full.h[2]->Clone(Form("hC2mirrorAsym_1_%d",ipaty)); X.A->SetDirectory(0); X.A->Reset();
				for (int ix=1;ix<=nx;ix++) for (int iy=1;iy<=ny;iy++){
					if (!okF(ix,iy)) continue;
					int jy	= jm[iy];
					X.A->SetBinContent(ix,iy, full.h[2]->GetBinContent(ix,jy) - full.h[2]->GetBinContent(ix,iy));
					X.A->SetBinError(  ix,iy, sqrt(pow(full.h[2]->GetBinError(ix,jy),2) + pow(full.h[2]->GetBinError(ix,iy),2)));
				}
				X.Ady	= new TH1D(Form("hC2mirrorAsymDy_1_%d",ipaty),"",nx,ax->GetXmin(),ax->GetXmax()); X.Ady->SetDirectory(0);
				for (int ix=1;ix<=nx;ix++){
					auto colD	= [&](const CFSet& cs, bool& ok){
						double D=0; ok = true; int nb=0;
						for (int iy=1;iy<=ny;iy++){
							if (!inW(0,ix,iy) || !okF(ix,iy)) continue;
							D	+= (cs.h[0]->GetBinContent(ix,jm[iy]) - cs.h[1]->GetBinContent(ix,jm[iy])) - (cs.h[0]->GetBinContent(ix,iy) - cs.h[1]->GetBinContent(ix,iy)); ++nb;
						}
						if (nb==0) ok = false;
						return D;
					};
					bool ok; double Dfull = colD(full,ok);
					if (!ok) continue;
					double s=0,s2=0; int n=0;
					for (int ic=0;ic<nlists;ic++){ bool okc; double d = colD(sub[ic],okc); if (!okc) continue; s+=d; s2+=d*d; ++n; }
					X.Ady->SetBinContent(ix,Dfull);
					X.Ady->SetBinError(ix, (n>=NVALIDMIN) ? sqrt(std::max(0.,(s2-s*s/n)/(n-1.)))/sqrt((double)n) : 0.);
				}
				//---- occupancy proxy per chunk: <n1><n2> from the chunk's hmult
				X.x.assign(nlists,0.);
				for (int ic=0;ic<nlists;ic++){
					TH2D *hm	= (TH2D*)fch[ic]->Get(Form("hmult_%d",ipaty));
					if (hm){ X.x[ic] = hm->GetMean(1)*hm->GetMean(2); delete hm; }
				}
				{ double xs=0; for (double v : X.x) xs += v; xs /= nlists; X.xlo = *std::min_element(X.x.begin(),X.x.end())/xs; X.xhi = *std::max_element(X.x.begin(),X.x.end())/xs; }
				cout<<Form("corral::Finalize -- step 1b pairtype %2d acceptance-corrected fA: near %+.3e +- %.1e | near |dy| %+.3e +- %.1e | away %+.3e +- %.1e ; <n1><n2>/mean over chunks %.3f..%.3f",
					ipaty,X.fA[0],X.efA[0],X.fA[1],X.efA[1],X.fA[2],X.efA[2],X.xlo,X.xhi)<<endl;
				cout<<Form("corral::Finalize -- step 1b pairtype %2d (ds %+d, tested half dphi%s0): f near %+.3e +- %.1e (%d chunks) | near |dy|<%.2g %+.3e +- %.1e | away %+.3e +- %.1e | fM near %+.3e +- %.1e | check sum(C2C-C2)/S %+.3e",
					ipaty,ds,X.half<0?"<":">",X.f[0],X.ef[0],X.nuse[0],dycut,X.f[1],X.ef[1],X.f[2],X.ef[2],X.fM[0],X.efM[0],X.fC)<<endl;
			}
			for (int k=0;k<NK;k++){ result[Form("%s_%d",Form(KINDNAME[k],ir2),ipaty)] = full.h[k]; full.h[k] = 0; }
			full.Delete();
			for (int ic=0;ic<nlists;ic++) sub[ic].Delete();		// README_Finalize step 1b will use sub[] here
			for (int ic=0;ic<nlists;ic++) for (int z=0;z<NZ;z++){ delete cS[ic][z]; delete cM[ic][z]; }
			delete tmpl;
		}
		cout<<"corral::Finalize -- pairtype "<<ipaty<<" done (dirty side "<<ds<<")"<<endl;
	}
	//
	//---- compare with the reference, write, draw
	TString OutputFileBase	= (OutputName!="") ? OutputName : TString(Form("corral_m_%s",RunString.Data()));
	TString RootFileName	= TString("./root/") + OutputFileBase + ".root";
	TString PdfFileName		= TString("./pdf/")  + OutputFileBase + ".pdf";
	TFile *fout	= new TFile(RootFileName.Data(),"RECREATE");
	gStyle->SetOptStat(0);
	TCanvas *cfin	= new TCanvas("cfin","cfin",1400,1000);
	cfin->Print((PdfFileName+"[").Data());
	//
	//==== PHYSICS PAGES FIRST (user 2026-09-26), per pairtype: 1 overview (as corral_m's first page, but the
	//==== (dy,dphi) row crossing-corrected), 2 C(Q) (ratio of zvtx sums), 3 crossing page (3x2, as corral_m),
	//==== 4 dC2 = C2C - C2 page. Diagnostic pages follow after the summary.
	{
		TCanvas *cph	= new TCanvas("cph","cph",1200,800);
		//---- V0 mass histograms (global, counts): plain sums over chunks
		std::map<TString,TH1*> v0m;
		for (const char* n : {"hKSmass","hLAmass","hLAmass_clean","hALmass","hALmass_clean"}){
			for (int ic=0;ic<nlists;ic++){
				TH1 *h	= (TH1*)fch[ic]->Get(n);
				if (!h) continue;
				if (!v0m[n]){ v0m[n] = (TH1*)h->Clone(Form("%s_fin",n)); v0m[n]->SetDirectory(0); }
				else v0m[n]->Add(h);
				delete h;
			}
		}
		auto pn	= [&](int ipaty){ return TString(Form("%s%s",ParticleIDNames[PairTypes_Info[ipaty][0]],ParticleIDNames[PairTypes_Info[ipaty][1]])); };
		auto clone2	= [&](const char* nm, const char* tit){
			TH2D *h	= (TH2D*)result[nm];
			if (!h) return (TH2D*)0;
			TH2D *c	= (TH2D*)h->Clone(Form("%s_page",nm)); c->SetDirectory(0); c->SetTitle(tit);
			return c;
		};
		auto zeroline	= [&](){ gPad->Update(); TLine *l0 = new TLine(gPad->GetUxmin(),0.,gPad->GetUxmax(),0.); l0->SetLineColor(kGray+1); l0->SetLineStyle(2); l0->Draw(); };
		auto runlines	= [&](double a, double b){ for (int k=1;k<nlists;k++){ if (chunkRun[k]==chunkRun[k-1]) continue; TLine *lr = new TLine(k-0.5,a,k-0.5,b); lr->SetLineStyle(2); lr->SetLineColor(kGray+1); lr->Draw(); } };
		std::vector<int> runsP;
		for (int ic=0;ic<nlists;ic++) if (std::find(runsP.begin(),runsP.end(),chunkRun[ic])==runsP.end()) runsP.push_back(chunkRun[ic]);
		const int runcolP[6]	= {kBlack,kRed+1,kBlue+1,kGreen+2,kMagenta+1,kOrange+1};
		std::vector<double> slopeX(NPairTypes,0.), eslopeX(NPairTypes,0.);
		for (int ipaty=0;ipaty<NPairTypes;ipaty++){
			Phys &P	= ph[ipaty];
			if (!P.pU[0] || !P.R2yy) continue;
			TString nm	= pn(ipaty);
			int pid1	= PairTypes_Info[ipaty][0], pid2 = PairTypes_Info[ipaty][1];
			//---------------- page 1: overview
			cph->Clear(); cph->Divide(4,3,0.0001,0.0001);
			TH1 *hm1	= result[Form("hmult1_%d",ipaty)], *hm2 = result[Form("hmult2_%d",ipaty)], *hmm = result[Form("hmult_%d",ipaty)];
			cph->cd(1); gPad->SetLogy(1);
			if (hm1){
				TH1 *d	= (TH1*)hm1->Clone(Form("hm1page_%d",ipaty)); d->SetDirectory(0);
				d->SetTitle(Form("%s, n_{1} per event (Finalize, all chunks);n_{1}",nm.Data()));
				d->Draw();
				TLatex tx; tx.SetNDC(); tx.SetTextFont(42); tx.SetTextSize(0.06); tx.SetTextAlign(31);
				tx.DrawLatex(0.88,0.85,Form("N_{evt}=%.0f",hmm?hmm->GetEntries():0.));
				tx.DrawLatex(0.88,0.78,Form("<N_{1}>=%.3g",hm1->GetMean()));
				if (P.hy1) tx.DrawLatex(0.88,0.71,Form("INT #rho_{1}(1)=%.3g",P.hy1->Integral()));
				if (hm2) tx.DrawLatex(0.88,0.60,Form("<N_{2}>=%.3g",hm2->GetMean()));
				if (P.hy2) tx.DrawLatex(0.88,0.53,Form("INT #rho_{1}(2)=%.3g",P.hy2->Integral()));
			}
			cph->cd(2); P.R2yy->SetMarkerStyle(20); P.R2yy->SetMarkerSize(0.5); P.R2yy->Draw("E1");
			cph->cd(3);
			{ TH1D *d = (TH1D*)P.pC[1]->Clone(Form("pC1page_%d",ipaty)); d->SetDirectory(0);
			  d->SetTitle(Form("%s, R_{2C}(d#phi) crossing-corrected (#pm subgroup err);d#phi (deg);R_{2}",nm.Data()));
			  d->SetMarkerStyle(20); d->SetMarkerSize(0.5); d->Draw("E1"); }
			//---- V0 mass pads (as corral_m: one per distinct V0 species of the pair)
			{
				std::vector<int> v0s;
				for (int pid : {pid1,pid2}){
					if (std::find(v0s.begin(),v0s.end(),pid)!=v0s.end()) continue;
					if (pid==kParticleIDKshort || pid==kParticleIDLambda || pid==kParticleIDAntiLambda) v0s.push_back(pid);
				}
				for (size_t iv=0;iv<v0s.size() && iv<2;iv++){
					cph->cd(iv==0 ? 4 : 8);
					const char* b	= (v0s[iv]==kParticleIDKshort) ? "hKSmass" : (v0s[iv]==kParticleIDLambda) ? "hLAmass" : "hALmass";
					if (v0m[b]) v0m[b]->Draw();
					TString cl	= TString(b)+"_clean";
					if (v0s[iv]!=kParticleIDKshort && v0m[cl]) v0m[cl]->Draw("same");
				}
			}
			//---- row 2: (y1,y2), not crossing-correctable
			{
				TH2D *a	= clone2(Form("hrho2_0_%d",ipaty),    Form("%s, #rho_{2}(y_{1},y_{2});y_{1};y_{2}",nm.Data()));
				TH2D *b	= clone2(Form("hrho1rho1_0_%d",ipaty),Form("%s, #rho_{1}#rho_{1}(y_{1},y_{2}) (mixed);y_{1};y_{2}",nm.Data()));
				TH2D *c	= clone2(Form("hR2_0_%d",ipaty),      Form("%s, R_{2}(y_{1},y_{2});y_{1};y_{2}",nm.Data()));
				cph->cd(5); if (a) a->Draw("surf3");
				cph->cd(6); if (b) b->Draw("surf3");
				cph->cd(7); if (c) c->Draw("lego2");
			}
			//---- row 3: (dy,dphi), crossing-corrected
			{
				TH2D *a	= clone2(Form("hrho2C_1_%d",ipaty),   Form("%s, #rho_{2C}(dy,d#phi) crossing-corrected;dy;d#phi",nm.Data()));
				TH2D *b	= clone2(Form("hrho1rho1_1_%d",ipaty),Form("%s, #rho_{1}#rho_{1}(dy,d#phi) (mixed);dy;d#phi",nm.Data()));
				TH2D *c	= clone2(Form("hR2C_1_%d",ipaty),     Form("%s, R_{2C}(dy,d#phi) crossing-corrected;dy;d#phi",nm.Data()));
				TH2D *e	= clone2(Form("hR2C_1_%d",ipaty),     Form("%s, R_{2C}(dy,d#phi) crossing-corrected;dy;d#phi (deg)",nm.Data()));
				cph->cd(9);  if (a) a->Draw("surf3");
				cph->cd(10); if (b) b->Draw("surf3");
				cph->cd(11); if (c) c->Draw("lego2");
				cph->cd(12); if (e){ gPad->SetRightMargin(0.14); e->Draw("colz"); }
			}
			cph->cd(); cph->Update(); cph->Print(PdfFileName.Data());
			//---------------- page 2: C(Q) (2x2, as corral_m): C(Q) 0-1.2 GeV (5 MeV) with signposts | C(Q) in the
			//---------------- 4 kT bins (10 MeV) | C(Q) 0-0.3 GeV (1 MeV) | Minv near threshold, sibling vs mixed
			if (P.CQ && P.CQ5 && P.CQKT10){
				cph->Clear(); cph->Divide(2,2,0.0001,0.0001);
				auto yrange	= [&](TH1D* h, double qlo, double qhi, double top){
					double ymn=1e30, ymx=-1e30;
					for (int ib=h->FindBin(qlo);ib<=h->FindBin(qhi);ib++){
						if (h->GetBinError(ib)<=0.) continue;
						ymn	= std::min(ymn,h->GetBinContent(ib)); ymx = std::max(ymx,h->GetBinContent(ib));
					}
					if (ymx>ymn){ h->SetMinimum(ymn-0.05*(ymx-ymn)); h->SetMaximum(ymx+top*(ymx-ymn)); }
				};
				cph->cd(1);
				{ TH1D *h = (TH1D*)P.CQ5->Clone(Form("CQ5page_%d",ipaty)); h->SetDirectory(0);
				  yrange(h,0.0201,1.2,0.25); h->Draw(); gPad->Update();
				  DrawCQSignposts(pid1,pid2,gPad->GetUymin(),gPad->GetUymax(),1.2); }
				cph->cd(2);
				{
					const int kcol[4]	= {kBlue,kGreen+2,kOrange+1,kRed};
					TH1D *hk[4]; double ymn=1e30, ymx=-1e30;
					for (int ik=0;ik<4;ik++){
						hk[ik]	= P.CQKT10->ProjectionX(Form("CQKTpage_%d_%d",ipaty,ik),ik+1,ik+1,"e"); hk[ik]->SetDirectory(0);
						hk[ik]->SetLineColor(kcol[ik]); hk[ik]->SetMarkerColor(kcol[ik]);
						for (int ib=hk[ik]->FindBin(0.0201);ib<=hk[ik]->GetNbinsX();ib++){
							if (hk[ik]->GetBinError(ib)<=0.) continue;
							ymn	= std::min(ymn,hk[ik]->GetBinContent(ib)); ymx = std::max(ymx,hk[ik]->GetBinContent(ib));
						}
					}
					hk[0]->SetTitle(Form("%s, C(Q) in k_{T} bins (10 MeV bins, #pm subgroup err);Q_{inv} (GeV)",nm.Data()));
					if (ymx>ymn){ hk[0]->SetMinimum(ymn-0.05*(ymx-ymn)); hk[0]->SetMaximum(ymx+0.05*(ymx-ymn)); }
					hk[0]->Draw("hist");
					for (int ik=1;ik<4;ik++) hk[ik]->Draw("hist same");
					TLegend *lgk	= new TLegend(0.55,0.65,0.89,0.89);
					lgk->SetBorderSize(0); lgk->SetFillStyle(0); lgk->SetTextSize(0.035);
					for (int ik=0;ik<4;ik++){
						double k1	= P.CQKT10->GetYaxis()->GetBinLowEdge(ik+1), k2 = P.CQKT10->GetYaxis()->GetBinUpEdge(ik+1);
						TLegendEntry *le	= lgk->AddEntry(hk[ik],Form("%.2f<k_{T}<%.2f GeV",k1,k2),"l");
						le->SetLineColor(kcol[ik]); le->SetLineWidth(2); le->SetTextColor(kcol[ik]);
					}
					lgk->Draw();
				}
				cph->cd(3);
				{ TH1D *h = (TH1D*)P.CQ->Clone(Form("CQzoompage_%d",ipaty)); h->SetDirectory(0);
				  h->GetXaxis()->SetRangeUser(0.,0.3);
				  h->SetTitle(Form("%s, C(Q) (1 MeV bins, zoom, #pm subgroup err);Q_{inv} (GeV);C(Q)",nm.Data()));
				  yrange(h,0.0201,0.2999,0.05); h->Draw(); }
				cph->cd(4);
				if (P.MinvS && P.MinvM){
					TH1D *ms	= (TH1D*)P.MinvS->Clone(Form("MinvSpage_%d",ipaty)); ms->SetDirectory(0);
					TH1D *mm	= (TH1D*)P.MinvM->Clone(Form("MinvMpage_%d",ipaty)); mm->SetDirectory(0);
					int b1		= ms->GetXaxis()->FindBin(ms->GetXaxis()->GetXmax()-0.150+1e-6), b2 = ms->GetNbinsX();
					double im	= mm->Integral(b1,b2);
					double fmix	= (im>0.) ? ms->Integral(b1,b2)/im : 0.;
					if (fmix>0.) mm->Scale(fmix);
					ms->SetLineColor(kGreen+2); mm->SetLineColor(kBlue);
					ms->SetMinimum(0.); ms->SetMaximum(1.1*std::max(ms->GetMaximum(),mm->GetMaximum()));
					ms->Draw("hist"); mm->Draw("hist same");
					TLegend *lgm	= new TLegend(0.30,0.11,0.89,0.21);
					lgm->SetBorderSize(0); lgm->SetFillColor(kWhite); lgm->SetTextSize(0.030);
					TLegendEntry *l1	= lgm->AddEntry(ms,"sibling (raw counts)","l"); l1->SetLineColor(kGreen+2);
					TLegendEntry *l2	= lgm->AddEntry(mm,Form("mixed #times %.4g (sib/mix, top 150 MeV)",fmix),"l"); l2->SetLineColor(kBlue);
					lgm->Draw();
				}
				cph->cd(); cph->Update(); cph->Print(PdfFileName.Data());
			}
			//---------------- page 3: crossing page (3x2), maps from Finalize, projections with subgroup errors
			{
				TH2D *hu	= clone2(Form("hR2_1_%d",ipaty), Form("%s, R_{2}(dy,d#phi) uncorrected;dy;d#phi (deg)",nm.Data()));
				TH2D *hc	= clone2(Form("hR2C_1_%d",ipaty),Form("%s, R_{2}(dy,d#phi) crossing-corrected;dy;d#phi (deg)",nm.Data()));
				TH2D *hmM	= (TH2D*)result[Form("hrho1rho1_1_%d",ipaty)];
				int ds		= xl[ipaty].ds;
				int nx=hu->GetNbinsX(), ny=hu->GetNbinsY(); double lo=hu->GetYaxis()->GetXmin();
				std::vector<int> jmir(ny+1,0); std::vector<double> cwr(ny+1,0.);
				for (int iy=1;iy<=ny;iy++){
					double c	= hu->GetYaxis()->GetBinCenter(iy);
					while (c>= 180.) c -= 360.; while (c< -180.) c += 360.;
					cwr[iy]		= c;
					double cm	= -c; while (cm<lo) cm += 360.; while (cm>=lo+360.) cm -= 360.;
					jmir[iy]	= hu->GetYaxis()->FindFixBin(cm);
				}
				auto okU	= [&](int ix,int iy){ return hmM->GetBinContent(ix,iy)>0.; };
				auto okC	= [&](int ix,int iy){
					if (ds==0) return okU(ix,iy);
					if (fabs(cwr[iy])<90.) return okU(ix, (cwr[iy]*ds>0.) ? jmir[iy] : iy);
					return okU(ix,iy) || okU(ix,jmir[iy]);
				};
				double zmin=1e30, zmax=-1e30;
				for (int ix=1;ix<=nx;ix++) for (int iy=1;iy<=ny;iy++){
					if (okU(ix,iy)){ zmin = std::min(zmin,hu->GetBinContent(ix,iy)); zmax = std::max(zmax,hu->GetBinContent(ix,iy)); }
					if (okC(ix,iy)){ zmin = std::min(zmin,hc->GetBinContent(ix,iy)); zmax = std::max(zmax,hc->GetBinContent(ix,iy)); }
				}
				if (zmin<zmax){ hu->SetMinimum(zmin); hu->SetMaximum(zmax); hc->SetMinimum(zmin); hc->SetMaximum(zmax); }
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
				cph->Clear(); cph->Divide(3,2,0.0001,0.0001);
				cph->cd(1); gPad->SetRightMargin(0.14); hu->Draw("colz");
				for (int ix=1;ix<=nx;ix++) for (int iy=1;iy<=ny;iy++){
					if (!flag[ix][iy]) continue;
					double x1=hu->GetXaxis()->GetBinLowEdge(ix), x2=hu->GetXaxis()->GetBinUpEdge(ix);
					double y1=hu->GetYaxis()->GetBinLowEdge(iy), y2=hu->GetYaxis()->GetBinUpEdge(iy);
					TLine *ln[4]	= {0,0,0,0};
					if (!flag[ix-1][iy]) ln[0] = new TLine(x1,y1,x1,y2);
					if (!flag[ix+1][iy]) ln[1] = new TLine(x2,y1,x2,y2);
					if (!flag[ix][iy-1]) ln[2] = new TLine(x1,y1,x2,y1);
					if (!flag[ix][iy+1]) ln[3] = new TLine(x1,y2,x2,y2);
					for (int k=0;k<4;k++) if (ln[k]){ ln[k]->SetLineColor(kRed); ln[k]->SetLineWidth(2); ln[k]->Draw(); }
				}
				{ TLatex tx; tx.SetNDC(); tx.SetTextFont(42); tx.SetTextSize(0.035); tx.SetTextColor(kRed); tx.SetTextAlign(11);
				  tx.DrawLatex(0.02,0.015,Form("red: dirty |R_{2}-mirror|>3#sigma_{sg}, %d bins (d#phi%s0)",nflag,ds>0?">":"<")); }
				cph->cd(4); gPad->SetRightMargin(0.14); hc->Draw("colz");
				TH1D *hyy	= (TH1D*)P.R2yy->Clone(Form("R2yypage_%d",ipaty)); hyy->SetDirectory(0);
				hyy->SetLineColor(kGreen+2); hyy->SetMarkerColor(kGreen+2); hyy->SetMarkerStyle(21); hyy->SetMarkerSize(0.5);
				for (int k=0;k<2;k++){
					TH1D *hs[4]	= {P.pU[k],P.pC[k],P.nU[k],P.nC[k]};
					const int col[4]	= {kBlack,kRed,kBlue,kMagenta};
					const int mst[4]	= {20,24,20,24};
					for (int j=0;j<4;j++){ hs[j]->SetLineColor(col[j]); hs[j]->SetMarkerColor(col[j]); hs[j]->SetMarkerStyle(mst[j]); hs[j]->SetMarkerSize(0.5); }
					for (int iw=0;iw<2;iw++){
						cph->cd(2+3*k+iw);
						TH1D *ha	= (iw==0) ? P.pU[k] : P.nU[k];
						TH1D *hb	= (iw==0) ? P.pC[k] : P.nC[k];
						bool doyy	= (iw==0 && k==0);
						double pmin	= std::min(ha->GetMinimum(), hb->GetMinimum());
						double pmax	= std::max(ha->GetMaximum(), hb->GetMaximum());
						if (doyy){ pmin = std::min(pmin,hyy->GetMinimum()); pmax = std::max(pmax,hyy->GetMaximum()); }
						TH1D *hf	= (TH1D*)ha->Clone(Form("%s_frame",ha->GetName())); hf->SetDirectory(0);
						hf->SetMinimum(pmin-0.1*(pmax-pmin)); hf->SetMaximum(pmax+0.30*(pmax-pmin));
						hf->Draw("E1");
						if (k==1){ TLine *l0 = new TLine(0.,hf->GetMinimum(),0.,hf->GetMaximum()); l0->SetLineColor(kGray); l0->Draw(); hf->Draw("E1 same"); }
						hb->Draw("E1 same");
						if (doyy) hyy->Draw("E1 same");
						TLegend *lgc	= new TLegend(0.25,doyy?0.74:0.78,0.75,0.89);
						lgc->SetBorderSize(1); lgc->SetFillStyle(0); lgc->SetTextSize(0.04);
						lgc->AddEntry(ha,"No Corr","lp");
						lgc->AddEntry(hb,"Cross Corr","lp");
						if (doyy) lgc->AddEntry(hyy,"from R_{2}(y_{1},y_{2}), No Corr","lp");
						lgc->Draw();
					}
				}
				cph->cd(); cph->Update(); cph->Print(PdfFileName.Data());
			}
			//---------------- page 4: dC2 = C2C - C2
			{
				XLoss &X	= xl[ipaty];
				cph->Clear(); cph->Divide(3,2,0.0001,0.0001);
				cph->cd(1); gPad->SetRightMargin(0.14);
				P.dC2->Draw("colz");
				if (X.ds==0){ TLatex tx; tx.SetNDC(); tx.SetTextFont(42); tx.SetTextSize(0.05); tx.DrawLatex(0.15,0.5,"no crossing correction (dirty side 0): C_{2C} = C_{2}"); }
				cph->cd(2); P.dC2dphi->SetMarkerStyle(20); P.dC2dphi->SetMarkerSize(0.5); P.dC2dphi->Draw("E0"); zeroline();	// E0: the clean side is identically 0 (content and error) and must show
				{	// thin gray verticals at dphi = 0 and 90 deg: the clean side between them is identically 0
					gPad->Update();
					for (double xl : {0.,90.}){ TLine *lv = new TLine(xl,gPad->GetUymin(),xl,gPad->GetUymax()); lv->SetLineColor(kGray); lv->SetLineWidth(1); lv->Draw(); }
					P.dC2dphi->Draw("E0 same");
				}
				cph->cd(3); P.dC2dy->SetMarkerStyle(20);   P.dC2dy->SetMarkerSize(0.5);   P.dC2dy->Draw("E0");   zeroline();
				//---- per chunk: fraction of pairs added vs chunk index, and vs occupancy
				double ymn=1e30, ymx=-1e30;
				TGraphErrors *gi	= new TGraphErrors(); gi->SetName(Form("gdC2chunk_%d",ipaty));
				for (int ic=0;ic<nlists;ic++){
					gi->SetPoint(gi->GetN(),ic,P.fracc[ic]); gi->SetPointError(gi->GetN()-1,0.,P.efracc[ic]);
					ymn	= std::min(ymn,P.fracc[ic]-P.efracc[ic]); ymx = std::max(ymx,P.fracc[ic]+P.efracc[ic]);
				}
				if (!(ymx>ymn)){ ymn=-1e-3; ymx=1e-3; }
				{ double dd=ymx-ymn; ymn -= 0.1*dd; ymx += 0.3*dd; }
				double chi2=0, ndf=0, slc=0, eslc=0, slx=0, eslx=0;
				cph->cd(4);
				gPad->DrawFrame(-1.,ymn,nlists,ymx,Form("%s, pairs added / #Sigma#rho_{2}(S), per chunk;chunk index (time order);fraction added",nm.Data()));
				runlines(ymn,ymx);
				{ TBox *bx = new TBox(-1.,P.frac-P.efrac,nlists,P.frac+P.efrac); bx->SetFillColorAlpha(kGray,0.5); bx->SetLineWidth(0); bx->Draw(); }
				gi->SetMarkerStyle(20); gi->SetMarkerSize(0.6);
				if (X.ds!=0 && gi->GetN()>2){
					TFitResultPtr r0	= gi->Fit("pol0","QNS"); if (r0.Get() && r0->IsValid()){ chi2 = r0->Chi2(); ndf = r0->Ndf(); }
					TFitResultPtr r1	= gi->Fit("pol1","QNS"); if (r1.Get() && r1->IsValid()){ slc = r1->Parameter(1); eslc = r1->ParError(1); }
				}
				gi->Draw("P");
				extraW.push_back(gi);
				cph->cd(5);
				{
					double xs=0; for (double v : X.x) xs += v; double xm = (nlists>0 && xs>0.) ? xs/nlists : 1.;
					double xmn=1e30, xmx=-1e30;
					TGraphErrors *ga	= new TGraphErrors(); ga->SetName(Form("gdC2occ_%d",ipaty));
					std::vector<TGraphErrors*> gr(runsP.size(),(TGraphErrors*)0);
					for (int ic=0;ic<nlists;ic++){
						double xx	= X.x[ic]/xm;
						ga->SetPoint(ga->GetN(),xx,P.fracc[ic]); ga->SetPointError(ga->GetN()-1,0.,P.efracc[ic]);
						int ir	= std::find(runsP.begin(),runsP.end(),chunkRun[ic]) - runsP.begin();
						if (!gr[ir]){ gr[ir] = new TGraphErrors(); gr[ir]->SetMarkerStyle(20); gr[ir]->SetMarkerSize(0.7); gr[ir]->SetMarkerColor(runcolP[ir%6]); gr[ir]->SetLineColor(runcolP[ir%6]); }
						gr[ir]->SetPoint(gr[ir]->GetN(),xx,P.fracc[ic]); gr[ir]->SetPointError(gr[ir]->GetN()-1,0.,P.efracc[ic]);
						xmn	= std::min(xmn,xx); xmx = std::max(xmx,xx);
					}
					if (xmx>xmn){ double dx=xmx-xmn; xmn -= 0.05*dx; xmx += 0.05*dx; } else { xmn -= 0.05; xmx += 0.05; }
					gPad->DrawFrame(xmn,ymn,xmx,ymx,Form("%s, fraction added per chunk vs occupancy;#LTn_{1}#GT#LTn_{2}#GT / mean;fraction added",nm.Data()));
					if (X.ds!=0 && ga->GetN()>2){
						TF1 *fl	= new TF1(Form("fdC2occ_%d",ipaty),"pol1",xmn,xmx);
						TFitResultPtr r	= ga->Fit(fl,"QNS");
						if (r.Get() && r->IsValid()){ slx = r->Parameter(1); eslx = r->ParError(1); }
						fl->SetLineColor(kGray+2); fl->SetLineWidth(1); fl->Draw("same");
					}
					TLegend *lg	= new TLegend(0.14,0.78,0.89,0.89);
					lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.035); lg->SetNColumns(2);
					for (size_t ir=0;ir<runsP.size();ir++) if (gr[ir]){ gr[ir]->Draw("P"); lg->AddEntry(gr[ir],Form("run %d",runsP[ir]),"p"); }
					lg->Draw();
					extraW.push_back(ga);
				}
				slopeX[ipaty] = slx; eslopeX[ipaty] = eslx;
				cph->cd(6);
				{
					TLatex tx; tx.SetNDC(); tx.SetTextFont(42); tx.SetTextSize(0.05); tx.SetTextAlign(11);
					double y	= 0.88;
					tx.DrawLatex(0.06,y,Form("%s, #DeltaC_{2} = C_{2C} - C_{2}, full statistics",nm.Data())); y -= 0.09;
					tx.DrawLatex(0.06,y,Form("integral: %+.4e #pm %.1e pairs/event",P.I,P.eI)); y -= 0.08;
					tx.DrawLatex(0.06,y,Form("#times N_{evt} = %.4g pairs added",P.I*(hmm?hmm->GetEntries():0.))); y -= 0.08;
					tx.DrawLatex(0.06,y,Form("#Sigma#rho_{2}(S) = %.4e pairs/event",P.Sfull)); y -= 0.08;
					tx.DrawLatex(0.06,y,Form("fraction added: %+.3e #pm %.1e",P.frac,P.efrac)); y -= 0.08;
					if (X.ds!=0){
						tx.DrawLatex(0.06,y,Form("const fit over chunks: #chi^{2}/ndf = %.1f/%.0f",chi2,ndf)); y -= 0.08;
						tx.DrawLatex(0.06,y,Form("slope vs chunk: %+.2e #pm %.1e per chunk",slc,eslc)); y -= 0.08;
						tx.DrawLatex(0.06,y,Form("slope vs occupancy: %+.2e #pm %.1e",slx,eslx)); y -= 0.08;
					} else {
						tx.DrawLatex(0.06,y,"not pt-ordered (neutral leg): not corrected"); y -= 0.08;
					}
					tx.SetTextSize(0.035);
					tx.DrawLatex(0.06,0.06,"errors: subgroup (52 chunks); per-chunk bars: propagated (approximate)");
				}
				cph->cd(); cph->Update(); cph->Print(PdfFileName.Data());
				cout<<Form("corral::Finalize -- dC2 fits pairtype %2d %-18s: const chi2/ndf %.1f/%.0f, slope vs chunk %+.2e +- %.1e, slope vs occupancy %+.2e +- %.1e; projection fill-ins %.2e",
					ipaty,nm.Data(),chi2,ndf,slc,eslc,slx,eslx,fillT[ipaty]>0?(double)fillN[ipaty]/fillT[ipaty]:0.)<<endl;
			}
		}
		//---------------- dC2 summary page: fraction of pairs added per pairtype; per-chunk overlay for the pions
		{
			cph->Clear(); cph->Divide(2,1,0.0001,0.0001);
			cph->cd(1); gPad->SetBottomMargin(0.18);
			TH1D *hf	= new TH1D("hFin_dC2frac","fraction of pairs added by the crossing correction (full stats #pm subgroup err);;#Sigma#DeltaC_{2} / #Sigma#rho_{2}(S)",NPairTypes,-0.5,NPairTypes-0.5);
			hf->SetDirectory(0);
			for (int ipaty=0;ipaty<NPairTypes;ipaty++){ hf->SetBinContent(ipaty+1,ph[ipaty].frac); hf->SetBinError(ipaty+1,ph[ipaty].efrac); hf->GetXaxis()->SetBinLabel(ipaty+1,pn(ipaty).Data()); }
			hf->SetMarkerStyle(20); hf->SetMarkerSize(0.9); hf->GetXaxis()->SetLabelSize(0.04);
			hf->Draw("E1 X0"); zeroline();
			extraW.push_back(hf);
			cph->cd(2);
			double a=1e30, b=-1e30;
			for (int ipaty=0;ipaty<4;ipaty++) for (int ic=0;ic<nlists && ic<(int)ph[ipaty].fracc.size();ic++){ a = std::min(a,ph[ipaty].fracc[ic]-ph[ipaty].efracc[ic]); b = std::max(b,ph[ipaty].fracc[ic]+ph[ipaty].efracc[ic]); }
			if (!(b>a)){ a=-1e-3; b=1e-3; }
			{ double dd=b-a; a -= 0.05*dd; b += 0.35*dd; }
			gPad->DrawFrame(-1.,a,nlists,b,"fraction of pairs added per chunk, pions;chunk index (time order);fraction added");
			runlines(a,b);
			TLegend *lg	= new TLegend(0.14,0.78,0.89,0.89); lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.035); lg->SetNColumns(4);
			const int pc[4]	= {kBlack,kRed+1,kBlue+1,kGreen+2};
			for (int ipaty=0;ipaty<4;ipaty++){
				if (ph[ipaty].fracc.empty()) continue;
				TGraphErrors *g	= new TGraphErrors();
				for (int ic=0;ic<nlists;ic++){ g->SetPoint(g->GetN(),ic+0.15*ipaty,ph[ipaty].fracc[ic]); g->SetPointError(g->GetN()-1,0.,ph[ipaty].efracc[ic]); }
				g->SetMarkerStyle(20); g->SetMarkerSize(0.5); g->SetMarkerColor(pc[ipaty]); g->SetLineColor(pc[ipaty]); g->Draw("P");
				lg->AddEntry(g,pn(ipaty).Data(),"p");
			}
			lg->Draw();
			cph->cd(); cph->Update(); cph->Print(PdfFileName.Data());
		}
		cfin->cd();
	}
	//==== DIAGNOSTIC PAGES from here on
	for (int ih=0;ih<NFINALIZEHISTS;ih++){
		TString base	= TString(FinalizeHists[ih]);
		int kd=-1,ir;
		bool isCF	= ParseCF(base,kd,ir);
		for (int ipage=0;ipage<(isCF?2:1);ipage++){		// page 0: vs reference; page 1 (CF): Finalize error / reference error
			cfin->Clear(); cfin->Divide(4,4,0.0001,0.0001);
			for (int ipaty=0;ipaty<NPairTypes;ipaty++){
				TString name	= Form("%s_%d",base.Data(),ipaty);
				TH1 *hf		= result[name];
				TH1 *href	= (TH1*)fref->Get(name.Data());
				if (!hf || !href){ if (ipage==0) cout<<"corral::Finalize -- "<<name<<" missing (result or reference), skipped"<<endl; continue; }
				cfin->cd(1+ipaty);
				if (ipage==1){
					//---- subgroup error / reference (single-job, per-run-style) error, bins where both are >0
					TH2D *hr	= (TH2D*)hf->Clone(Form("%s_errratio",name.Data())); hr->SetDirectory(0); hr->Reset();
					std::vector<double> rr;
					for (int ib=0;ib<hf->GetNcells();ib++){
						if (hf->IsBinUnderflow(ib) || hf->IsBinOverflow(ib)) continue;	// u/o keep the chain's per-run errors
						double ef=hf->GetBinError(ib), er=href->GetBinError(ib);
						if (ef>0. && er>0.){ hr->SetBinContent(ib,ef/er); rr.push_back(ef/er); }
					}
					cout<<Form("corral::Finalize -- %-17s subgroup err / ref err: median %.3f (%d bins)",name.Data(),Median(rr),(int)rr.size())<<endl;
					hr->SetTitle(Form("%s: #sigma_{Finalize}/#sigma_{ref} (median %.2f)",name.Data(),Median(rr)));
					hr->SetMinimum(0.); hr->SetMaximum(3.);
					hr->Draw("colz");
					continue;
				}
				//---- relative difference (in-range bins with ref!=0) and pull vs the reference error (bins with both errors>0)
				double sd=0,sd2=0,dmax=0,sp=0,sp2=0; int nb=0,np=0;
				for (int ib=0;ib<hf->GetNcells();ib++){
					if (hf->IsBinUnderflow(ib) || hf->IsBinOverflow(ib)) continue;
					double a=hf->GetBinContent(ib), r=href->GetBinContent(ib);
					if (r!=0.){ double d=(a-r)/fabs(r); sd+=d; sd2+=d*d; dmax=std::max(dmax,fabs(d)); ++nb; }
					double er=href->GetBinError(ib);
					if (er>0. && hf->GetBinError(ib)>0.){ double p=(a-r)/er; sp+=p; sp2+=p*p; ++np; }
				}
				double mrel	= nb?sd/nb:0., rrel = nb?sqrt(std::max(0.,sd2/nb-mrel*mrel)):0.;
				double mp	= np?sp/np:0., rp = np?sqrt(std::max(0.,sp2/np-mp*mp)):0.;
				cout<<Form("corral::Finalize -- %-17s %s  rel diff mean %+.2e rms %.2e max %.2e (%d bins)   pull vs ref err: mean %+.3f rms %.3f (%d bins)",
					name.Data(), isCF?"[CF] ":"[sum]", mrel,rrel,dmax,nb, mp,rp,np)<<endl;
				fout->cd(); hf->Write();
				if (hf->GetDimension()==1){
					href->SetLineColor(kBlack); hf->SetLineColor(kRed);
					href->Draw("hist"); hf->Draw("hist same");
				} else {
					TH2D *hp	= (TH2D*)hf->Clone(Form("%s_pull",name.Data())); hp->SetDirectory(0); hp->Reset();
					for (int ib=0;ib<hf->GetNcells();ib++){
						double er=href->GetBinError(ib);
						if (er>0.) hp->SetBinContent(ib,(hf->GetBinContent(ib)-href->GetBinContent(ib))/er);
					}
					hp->SetTitle(Form("%s: (Finalize - ref)/#sigma_{ref}",name.Data()));
					hp->SetMinimum(-5.); hp->SetMaximum(5.);
					hp->Draw("colz");
				}
			}
			cfin->Print(PdfFileName.Data());
		}
	}
	//---- step 1b pages. Per pairtype (2x2): A = C2(mirror)-C2 map | D(dy) over the near tested half |
	//---- f per chunk vs chunk index (run boundaries dashed; near half black, |dy| window blue; full-stats
	//---- value +- subgroup error as a band) | f per chunk vs <n1><n2>/mean, colored by run, linear fit.
	//---- Per-chunk error bars: propagated per-run C2 errors (approximate); pol0 chi2 says whether the
	//---- chunk-to-chunk variation exceeds them.
	auto pname	= [&](int ipaty){ return TString(Form("%s%s",ParticleIDNames[PairTypes_Info[ipaty][0]],ParticleIDNames[PairTypes_Info[ipaty][1]])); };
	std::vector<int> runs;
	for (int ic=0;ic<nlists;ic++) if (std::find(runs.begin(),runs.end(),chunkRun[ic])==runs.end()) runs.push_back(chunkRun[ic]);
	const int runcol[6]	= {kBlack,kRed+1,kBlue+1,kGreen+2,kMagenta+1,kOrange+1};
	for (int ipaty=0;ipaty<NPairTypes;ipaty++){
		XLoss &X	= xl[ipaty];
		if (!X.have) continue;
		cfin->Clear(); cfin->Divide(2,2,0.0001,0.0001);
		//---- pad 1: A map
		cfin->cd(1); gPad->SetRightMargin(0.14);
		double amax	= 0.;
		for (int ib=0;ib<X.A->GetNcells();ib++) amax = std::max(amax,fabs(X.A->GetBinContent(ib)));
		X.A->SetTitle(Form("%s, C_{2}(dy,-d#phi) - C_{2}(dy,d#phi) (full stats);dy;d#phi (deg)",pname(ipaty).Data()));
		if (amax>0.){ X.A->SetMinimum(-amax); X.A->SetMaximum(amax); }
		X.A->Draw("colz");
		//---- pad 2: D(dy)
		cfin->cd(2);
		X.Ady->SetTitle(Form("%s, D(dy) = #Sigma_{near, d#phi%s0}[C_{2}(mirror)-C_{2}], #pm subgroup err;dy;pairs per event",pname(ipaty).Data(),X.half<0?"<":">"));
		X.Ady->SetMarkerStyle(20); X.Ady->SetMarkerSize(0.6);
		X.Ady->Draw("E1");
		gPad->Update();
		{ TLine *l0 = new TLine(gPad->GetUxmin(),0.,gPad->GetUxmax(),0.); l0->SetLineColor(kGray+1); l0->SetLineStyle(2); l0->Draw(); }
		//---- pad 3: f vs chunk index
		cfin->cd(3);
		TGraphErrors *gc[2];
		double ymn=1e30, ymx=-1e30;
		for (int w=0;w<2;w++){
			gc[w]	= new TGraphErrors();
			gc[w]->SetName(Form("gXlossChunk_w%d_%d",w,ipaty));
			for (int ic=0;ic<nlists;ic++){
				if (!X.usec[w][ic]) continue;
				int ip	= gc[w]->GetN();
				gc[w]->SetPoint(ip, ic, X.fAc[w][ic]); gc[w]->SetPointError(ip, 0., X.efc[w][ic]);
				ymn	= std::min(ymn, X.fAc[w][ic]-X.efc[w][ic]); ymx = std::max(ymx, X.fAc[w][ic]+X.efc[w][ic]);
			}
			gc[w]->SetMarkerStyle(20); gc[w]->SetMarkerSize(0.6);
			gc[w]->SetMarkerColor(w==0?kBlack:kBlue); gc[w]->SetLineColor(w==0?kBlack:kBlue);
			extraW.push_back(gc[w]);
		}
		if (gc[0]->GetN()>1){
			gc[0]->Fit("pol0","Q");
			TF1 *f0	= gc[0]->GetFunction("pol0");
			if (f0){ f0->SetLineColor(kGray+2); f0->SetLineWidth(1); X.chi2c = f0->GetChisquare(); X.ndfc = f0->GetNDF(); }
			TFitResultPtr r1	= gc[0]->Fit("pol1","QNS");	// N: not stored, pol0 stays the drawn function
			if (r1.Get() && r1->IsValid()){ X.slopec = r1->Parameter(1); X.eslopec = r1->ParError(1); }
		}
		if (ymx>ymn){ double dd=ymx-ymn; ymn -= 0.1*dd; ymx += 0.35*dd; }
		TH1F *fr3	= gPad->DrawFrame(-1., ymn, nlists, ymx, Form("%s, f_{A} (acceptance-corrected mirror deficit / #Sigma#rho_{2}(S)_{mirror}) per chunk;chunk index (time order);f_{A}",pname(ipaty).Data()));
		(void)fr3;
		for (int k=1;k<nlists;k++){
			if (chunkRun[k]==chunkRun[k-1]) continue;
			TLine *lr	= new TLine(k-0.5,ymn,k-0.5,ymx); lr->SetLineStyle(2); lr->SetLineColor(kGray+1); lr->Draw();
		}
		{	// full-stats value +- subgroup error, near half
			TBox *bx	= new TBox(-1., X.fA[0]-X.efA[0], nlists, X.fA[0]+X.efA[0]);
			bx->SetFillColorAlpha(kGray,0.5); bx->SetLineWidth(0); bx->Draw();
			TLine *lz	= new TLine(-1.,0.,nlists,0.); lz->SetLineColor(kGray+1); lz->Draw();
		}
		gc[0]->Draw("P"); gc[1]->Draw("P");
		{
			TLegend *lg	= new TLegend(0.14,0.72,0.89,0.89);
			lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.033);
			lg->AddEntry(gc[0],Form("near half: full %+.2e #pm %.1e, #chi^{2}/ndf(const) %.1f/%.0f, slope %+.1e #pm %.1e /chunk",X.fA[0],X.efA[0],X.chi2c,X.ndfc,X.slopec,X.eslopec),"p");
			lg->AddEntry(gc[1],Form("near |dy|<%.2g: full %+.2e #pm %.1e",X.dycut,X.fA[1],X.efA[1]),"p");
			lg->Draw();
		}
		//---- pad 4: f vs <n1><n2>/mean, by run
		cfin->cd(4);
		{
			double xs=0; int nxs=0;
			for (int ic=0;ic<nlists;ic++) if (X.usec[0][ic]){ xs += X.x[ic]; ++nxs; }
			double xm	= (nxs>0) ? xs/nxs : 1.;
			TGraphErrors *ga	= new TGraphErrors(); ga->SetName(Form("gXlossOcc_%d",ipaty));
			std::vector<TGraphErrors*> gr(runs.size(),(TGraphErrors*)0);
			double xmn=1e30, xmx=-1e30;
			for (int ic=0;ic<nlists;ic++){
				if (!X.usec[0][ic] || xm<=0.) continue;
				double xx	= X.x[ic]/xm;
				ga->SetPoint(ga->GetN(), xx, X.fAc[0][ic]); ga->SetPointError(ga->GetN()-1, 0., X.efc[0][ic]);
				int ir	= std::find(runs.begin(),runs.end(),chunkRun[ic]) - runs.begin();
				if (!gr[ir]){ gr[ir] = new TGraphErrors(); gr[ir]->SetMarkerStyle(20); gr[ir]->SetMarkerSize(0.7); gr[ir]->SetMarkerColor(runcol[ir%6]); gr[ir]->SetLineColor(runcol[ir%6]); }
				gr[ir]->SetPoint(gr[ir]->GetN(), xx, X.fAc[0][ic]); gr[ir]->SetPointError(gr[ir]->GetN()-1, 0., X.efc[0][ic]);
				xmn	= std::min(xmn,xx); xmx = std::max(xmx,xx);
			}
			extraW.push_back(ga);
			if (ga->GetN()>1){
				TFitResultPtr r	= ga->Fit("pol1","QNS");
				if (r.Get() && r->IsValid()){ X.slopex = r->Parameter(1); X.eslopex = r->ParError(1); }
			}
			if (xmx>xmn){ double dx=xmx-xmn; xmn -= 0.05*dx; xmx += 0.05*dx; } else { xmn -= 0.05; xmx += 0.05; }
			gPad->DrawFrame(xmn, ymn, xmx, ymx, Form("%s, f_{A} per chunk vs occupancy (near half);#LTn_{1}#GT#LTn_{2}#GT / mean;f_{A}",pname(ipaty).Data()));
			TLine *lz	= new TLine(xmn,0.,xmx,0.); lz->SetLineColor(kGray+1); lz->Draw();
			if (ga->GetN()>1 && X.eslopex>0.){
				TF1 *fl	= new TF1(Form("fXlossOcc_%d",ipaty),"pol1",xmn,xmx);
				TFitResultPtr r	= ga->Fit(fl,"QNS");
				fl->SetLineColor(kGray+2); fl->SetLineWidth(1); fl->Draw("same");
				(void)r;
			}
			TLegend *lg	= new TLegend(0.14,0.72,0.89,0.89);
			lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.033); lg->SetNColumns(2);
			for (size_t ir=0;ir<runs.size();ir++) if (gr[ir]){ gr[ir]->Draw("P"); lg->AddEntry(gr[ir],Form("run %d",runs[ir]),"p"); }
			lg->AddEntry((TObject*)0,Form("slope df/d(x/#bar{x}) %+.2e #pm %.1e",X.slopex,X.eslopex),"");
			lg->Draw();
		}
		cfin->Print(PdfFileName.Data());
		extraW.push_back(X.A); extraW.push_back(X.Ady);
	}
	//---- step 1b summary pages: (1) full-stats f per pairtype, 3 windows + the rho2(M) asymmetry;
	//---- (2) f per chunk vs chunk index, overlaid per pairtype group, and the occupancy slope per pairtype
	{
		cfin->Clear(); cfin->Divide(1,1);
		cfin->cd(1); gPad->SetBottomMargin(0.18);
		const int NSM	= 5;
		TH1D *hs[NSM];
		const char* lab[NSM]	= {"f_{A}, near half","f_{A}, near |dy| window","f_{A}, away half (null)","f (C_{2}, not acc.-corrected), near half","f_{M} = same with #rho_{2}(M), near half"};
		const int col[NSM]	= {kBlack,kBlue,kGreen+2,kGray+1,kOrange+1};
		double ymn=1e30, ymx=-1e30;
		for (int k=0;k<NSM;k++){
			hs[k]	= new TH1D(Form("hXlossSummary_%d",k),"",NPairTypes,-0.5,NPairTypes-0.5); hs[k]->SetDirectory(0);
			for (int ipaty=0;ipaty<NPairTypes;ipaty++){
				XLoss &X	= xl[ipaty];
				if (!X.have) continue;
				double v	= (k<3) ? X.fA[k] : (k==3) ? X.f[0]  : X.fM[0];
				double e	= (k<3) ? X.efA[k] : (k==3) ? X.ef[0] : X.efM[0];
				hs[k]->SetBinContent(ipaty+1,v); hs[k]->SetBinError(ipaty+1,e);
				hs[k]->GetXaxis()->SetBinLabel(ipaty+1,pname(ipaty).Data());
				ymn	= std::min(ymn,v-e); ymx = std::max(ymx,v+e);
			}
			hs[k]->SetMarkerStyle(20+k); hs[k]->SetMarkerSize(0.9); hs[k]->SetMarkerColor(col[k]); hs[k]->SetLineColor(col[k]);
			extraW.push_back(hs[k]);
		}
		double dd	= ymx-ymn; if (dd<=0.) dd = 1.;
		hs[0]->SetMinimum(ymn-0.1*dd); hs[0]->SetMaximum(ymx+0.3*dd);
		hs[0]->SetTitle(Form("mirror deficit / #Sigma#rho_{2}(S)_{mirror}, tested half d#phi%s0, full stats #pm subgroup error;;fraction",xl[0].half<0?"<":">"));
		hs[0]->GetXaxis()->SetLabelSize(0.035);
		hs[0]->Draw("E1 X0");
		for (int k=1;k<NSM;k++) hs[k]->Draw("E1 X0 same");
		gPad->Update();
		{ TLine *l0 = new TLine(gPad->GetUxmin(),0.,gPad->GetUxmax(),0.); l0->SetLineColor(kGray+1); l0->SetLineStyle(2); l0->Draw(); }
		TLegend *lg	= new TLegend(0.55,0.70,0.89,0.89);
		lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.028);
		for (int k=0;k<NSM;k++) lg->AddEntry(hs[k],lab[k],"p");
		lg->Draw();
		cfin->Print(PdfFileName.Data());
		//
		cfin->Clear(); cfin->Divide(2,2,0.0001,0.0001);
		const char* grp[3]	= {"pions (0-3)","V0-V0 (4-9)","V0-#pi (10-15)"};
		const int glo[3]	= {0,4,10}, ghi[3] = {3,9,15};
		const int pcol[6]	= {kBlack,kRed+1,kBlue+1,kGreen+2,kMagenta+1,kOrange+1};
		for (int g=0;g<3;g++){
			cfin->cd(1+g);
			double a=1e30, b=-1e30;
			for (int ipaty=glo[g];ipaty<=ghi[g];ipaty++){ XLoss &X = xl[ipaty]; if (!X.have) continue;
				for (int ic=0;ic<nlists;ic++) if (X.usec[0][ic]){ a = std::min(a,X.fAc[0][ic]-X.efc[0][ic]); b = std::max(b,X.fAc[0][ic]+X.efc[0][ic]); } }
			if (!(b>a)){ a=-1; b=1; }
			double d2	= b-a; a -= 0.05*d2; b += 0.35*d2;
			gPad->DrawFrame(-1.,a,nlists,b,Form("f_{A} per chunk (near half), %s;chunk index (time order);f_{A}",grp[g]));
			for (int k=1;k<nlists;k++){ if (chunkRun[k]==chunkRun[k-1]) continue; TLine *lr = new TLine(k-0.5,a,k-0.5,b); lr->SetLineStyle(2); lr->SetLineColor(kGray+1); lr->Draw(); }
			TLine *lz	= new TLine(-1.,0.,nlists,0.); lz->SetLineColor(kGray+1); lz->Draw();
			TLegend *lgg	= new TLegend(0.14,0.74,0.89,0.89);
			lgg->SetBorderSize(0); lgg->SetFillStyle(0); lgg->SetTextSize(0.035); lgg->SetNColumns(3);
			for (int ipaty=glo[g];ipaty<=ghi[g];ipaty++){
				XLoss &X	= xl[ipaty]; if (!X.have) continue;
				TGraphErrors *gg	= new TGraphErrors();
				for (int ic=0;ic<nlists;ic++) if (X.usec[0][ic]){ gg->SetPoint(gg->GetN(), ic+0.12*(ipaty-glo[g]), X.fAc[0][ic]); gg->SetPointError(gg->GetN()-1,0.,X.efc[0][ic]); }
				int c	= pcol[(ipaty-glo[g])%6];
				gg->SetMarkerStyle(20); gg->SetMarkerSize(0.5); gg->SetMarkerColor(c); gg->SetLineColor(c);
				gg->Draw("P");
				lgg->AddEntry(gg,pname(ipaty).Data(),"p");
			}
			lgg->Draw();
		}
		cfin->cd(4); gPad->SetBottomMargin(0.18);
		TH1D *hsl	= new TH1D("hXlossSlopeOcc","",NPairTypes,-0.5,NPairTypes-0.5); hsl->SetDirectory(0);
		for (int ipaty=0;ipaty<NPairTypes;ipaty++){
			XLoss &X	= xl[ipaty]; if (!X.have) continue;
			hsl->SetBinContent(ipaty+1,X.slopex); hsl->SetBinError(ipaty+1,X.eslopex);
			hsl->GetXaxis()->SetBinLabel(ipaty+1,pname(ipaty).Data());
		}
		hsl->SetTitle("occupancy slope df_{A}/d(#LTn_{1}#GT#LTn_{2}#GT/mean) per pairtype (near half, per-chunk errors);;slope");
		hsl->SetMarkerStyle(20); hsl->SetMarkerSize(0.8);
		hsl->Draw("E1 X0");
		gPad->Update();
		{ TLine *l0 = new TLine(gPad->GetUxmin(),0.,gPad->GetUxmax(),0.); l0->SetLineColor(kGray+1); l0->SetLineStyle(2); l0->Draw(); }
		extraW.push_back(hsl);
		cfin->Print(PdfFileName.Data());
		for (int ipaty=0;ipaty<NPairTypes;ipaty++){
			XLoss &X	= xl[ipaty]; if (!X.have) continue;
			cout<<Form("corral::Finalize -- step 1b fits pairtype %2d %-18s: const chi2/ndf %.1f/%.0f; slope vs chunk %+.2e +- %.1e; slope vs occupancy %+.2e +- %.1e",
				ipaty,pname(ipaty).Data(),X.chi2c,X.ndfc,X.slopec,X.eslopec,X.slopex,X.eslopex)<<endl;
		}
	}
	//---- N_valid pages (4x4 pairtypes each): one per map name without the pairtype suffix
	{
		std::vector<TString> bases;
		for (TH1* h : extra){ TString b = h->GetName(); b.Remove(b.Last('_')); if (std::find(bases.begin(),bases.end(),b)==bases.end()) bases.push_back(b); }
		for (const TString& b : bases){
			cfin->Clear(); cfin->Divide(4,4,0.0001,0.0001);
			for (TH1* h : extra){
				TString nm	= h->GetName();
				TString bb	= nm; bb.Remove(bb.Last('_'));
				if (bb!=b) continue;
				int ipaty	= TString(nm(nm.Last('_')+1,nm.Length())).Atoi();
				cfin->cd(1+ipaty);
				h->SetMinimum(0.); h->SetMaximum(nlists);
				h->Draw("colz");
			}
			cfin->Print(PdfFileName.Data());
		}
	}
	cfin->Print((PdfFileName+"]").Data());
	fout->cd();
	for (TH1* h : extra) h->Write();
	for (TObject* o : extraW) o->Write();
	fout->Close();
	cout<<"corral::Finalize -- wrote "<<RootFileName<<" and "<<PdfFileName<<endl;
	cout<<"done."<<endl;
}
