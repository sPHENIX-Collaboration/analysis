#ifndef SEGLIVE_H
#define SEGLIVE_H
//---- SegLive.h -- TPC live fraction per (run, DST segment) from the segCells trees of chunk files. One copy of the rule,
//---- used by lists/make_bad_segments.C (the segveto mask) and by Finalize (the vs-segment QA page). Rule (user 2026-10-12):
//----   x_c = TPC hits in cell c per accepted track in the segment; r_c = 90th percentile of x_c over all segments; cells with
//----   r_c < 0.1 x the median r_c are never live and left out. A cell is dead in a segment if x_c < 0.5 r_c.
//----   live fraction = 1 - (sum of r_c over dead cells) / (sum of r_c over all cells): all, South (side 0), North (side 1).
//---- Cells: per FEE (branch "fee", tpcsector*26 + feeMap) or the older generic grid (branch "hits", 48 per sector-side).
//---- Segments without accepted tracks (ntrk = 0, e.g. skipped by segveto) get no live fraction.
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TString.h>
#include <map>
#include <vector>
#include <array>
#include <functional>
#include <algorithm>
#include <cstdio>

struct SegLiveResult {
	std::map<std::pair<int,int>,std::array<float,3>>	live;	// (run, segment) -> live fraction all, South, North
	int		nNever	= 0;		// cells never live
	bool	feeMode	= false;	// per-FEE cells (else the generic grid)
};

inline SegLiveResult SegLiveFractions(const std::vector<TString> &files){
	const double DEAD = 0.5, REFQ = 0.90;
	SegLiveResult R;
	if (files.empty()) return R;
	{ TFile *f = TFile::Open(files[0]); TTree *t = f ? (TTree*)f->Get("segCells") : 0; if (t && t->GetBranch("fee")) R.feeMode = true; if (f) f->Close(); }
	const int CPS = R.feeMode ? 26 : 48, NC = 24*CPS;
	auto loop = [&](std::function<void(int,int,int,const float*)> fn){
		Int_t run, seg, ntrk; std::vector<Float_t> hits(NC);
		for (auto &p : files){
			TFile *f = TFile::Open(p); TTree *t = f ? (TTree*)f->Get("segCells") : 0;
			if (!t || !t->GetBranch(R.feeMode ? "fee" : "hits")){ printf("SegLive -- no segCells (or the other cell layout: fee vs hits) in %s, skipped\n",p.Data()); if (f) f->Close(); continue; }
			t->SetBranchStatus("*",0);
			for (const char* b : {"run","segment","ntrk",R.feeMode ? "fee" : "hits"}) t->SetBranchStatus(b,1);
			t->SetBranchAddress("run",&run); t->SetBranchAddress("segment",&seg); t->SetBranchAddress("ntrk",&ntrk); t->SetBranchAddress(R.feeMode ? "fee" : "hits",hits.data());
			for (Long64_t e=0;e<t->GetEntries();e++){ t->GetEntry(e); if (ntrk>0) fn(run,seg,ntrk,hits.data()); }
			f->Close();
		}
	};
	//---- pass 1: cell means; pass 2: reference r_c; pass 3: live fractions per segment
	std::vector<double> sumH(NC,0.); double sumT = 0;
	loop([&](int,int,int ntrk,const float *h){ sumT += ntrk; for (int c=0;c<NC;c++) sumH[c] += h[c]; });
	if (sumT<=0) return R;
	std::vector<double> mean(NC); for (int c=0;c<NC;c++) mean[c] = sumH[c]/sumT;
	std::vector<TH1F*> hq(NC); for (int c=0;c<NC;c++){ hq[c] = new TH1F(Form("hSegLiveQ%d",c),"",400,0.,2.); hq[c]->SetDirectory(0); }
	loop([&](int,int,int ntrk,const float *h){ for (int c=0;c<NC;c++) if (mean[c]>0) hq[c]->Fill(h[c]/ntrk/mean[c]); });
	std::vector<double> ref(NC,0.);
	for (int c=0;c<NC;c++){ if (mean[c]>0){ double q = REFQ, x; hq[c]->GetQuantiles(1,&x,&q); ref[c] = x*mean[c]; } delete hq[c]; }
	std::vector<double> rs; for (int c=0;c<NC;c++) if (ref[c]>0) rs.push_back(ref[c]); std::sort(rs.begin(),rs.end());
	const double rmed = rs.empty() ? 0 : rs[rs.size()/2];
	std::vector<bool> use(NC); double rsum[3] = {0,0,0};
	for (int c=0;c<NC;c++){ use[c] = ref[c] > 0.1*rmed; if (!use[c]){ R.nNever++; continue; } rsum[0] += ref[c]; rsum[1+((c/CPS)&1)] += ref[c]; }
	loop([&](int run,int seg,int ntrk,const float *h){
		double d[3] = {0,0,0};
		for (int c=0;c<NC;c++) if (use[c] && h[c]/ntrk < DEAD*ref[c]){ d[0] += ref[c]; d[1+((c/CPS)&1)] += ref[c]; }
		R.live[{run,seg}] = {(float)(1.-d[0]/rsum[0]),(float)(1.-d[1]/rsum[1]),(float)(1.-d[2]/rsum[2])}; });
	return R;
}
#endif
