//---- make_bad_segments.C -- the segment mask for corral "segveto" (README.md, README_ana573.md), from the segCells trees of a
//---- production's chunk files (the acceptance pass, run WITHOUT segveto).
//----   root -l -b -q 'make_bad_segments.C+("ana573_795xx,ana573_8156x,ana573_8158x")'   (from lists/; reads ../root/<set>/chunks/)
//---- Writes the ONE file of the production, lists/<production>/bad_segments.txt (<production> = the leading "anaNNN" of the
//---- first set's name, e.g. ana578A_Raw -> lists/ana578/; corral segveto reads it from there, 2026-10-26). Give ALL Raw sets
//---- of the production (e.g. "ana578A_Raw,ana578B_Raw"): every run gets its own band, the file covers every run. One
//---- "run segment" line per LOW segment; "# band" header lines give each run's band and threshold (SegCellsView draws them).
//---- Rule (user 2026-10-12; the live fraction is computed in src/SegLive.h, shared with Finalize):
//----   x_c = TPC hits in cell c per accepted track in the segment; r_c = 90th percentile of x_c over all segments; cells with
//----   r_c < 0.1 x the median r_c are never live and left out. A cell is dead in a segment if x_c < 0.5 r_c.
//----   live fraction = 1 - (sum of r_c over dead cells) / (sum of r_c over all cells): all, South (side 0), North (side 1).
//----   Per run and view, band = mode of the per-segment live fraction (0.001 bins) +- BANDW, sigma = rms inside the band.
//----   A segment is LOW if, in any view, live < mode - max(LOWMIN, LOWNS sigma).
//---- Cells: per FEE (branch "fee", tpcsector*26 + feeMap) or the older generic grid (branch "hits", 48 per sector-side).
#include "../src/SegLive.h"	// the live-fraction rule (shared with Finalize's vs-segment page)
#include <TH1D.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TSystem.h>
#include <cmath>

//---- write = false: print the per-run counts only, write no bad_segments.txt (a check)
//---- lowmin = LOWMIN, the smallest drop below the mode that counts as low (0.005 = ana573; a nearly fully live TPC, as
//----   ana578, has sigma ~0.001, so LOWMIN alone sets the threshold: 0.005 = two dead FEEs on one side)
void make_bad_segments(const char* setlist = "ana573_795xx,ana573_8156x,ana573_8158x", bool write = true, double lowmin = 0.005){
	const double BANDW = 0.02, LOWMIN = lowmin, LOWNS = 4.;
	TString ldir	= gSystem->DirName(gSystem->UnixPathName(__FILE__));	// lists/
	TObjArray *ts	= TString(setlist).Tokenize(",");
	const int NS	= ts->GetEntries();
	std::vector<TString> setName, files;
	for (int s=0;s<NS;s++){
		TString sn = ((TObjString*)ts->At(s))->GetName(); setName.push_back(sn);
		for (int c=0;c<1000;c++){ TString p = ldir+"/../root/"+sn+Form("/chunks/corral_%02d.root",c); if (gSystem->AccessPathName(p)) break; files.push_back(p); }
	}
	if (files.empty()){ printf("no chunk files\n"); return; }
	SegLiveResult SL	= SegLiveFractions(files);
	const bool feeMode	= SL.feeMode;
	auto &live			= SL.live;
	printf("%zu chunk files, cells %s\n",files.size(),feeMode ? "per FEE" : "generic grid");
	printf("%zu segments, %d never-live cells\n",live.size(),SL.nNever);
	//---- band per run and view, LOW segments
	std::map<int,std::vector<std::pair<int,std::array<float,3>>>> byRun;
	for (auto &p : live) byRun[p.first.first].push_back({p.first.second,p.second});
	std::vector<TString> header; std::vector<std::pair<int,int>> low;
	for (auto &R : byRun){
		std::array<double,3> thr;
		for (int a=0;a<3;a++){
			TH1D hb("hb","",1000,0.0,1.0); for (auto &s : R.second) hb.Fill(std::min(0.99999,(double)s.second[a]));	// full range (was 0.6-1: ana578B North ~0.556 fell below it)
			double mode = hb.GetBinCenter(hb.GetMaximumBin()), s1 = 0, s2 = 0; int n = 0;
			for (auto &s : R.second){ double v = s.second[a]; if (fabs(v-mode)<BANDW){ s1 += v; s2 += v*v; n++; } }
			double sig = n>1 ? sqrt(std::max(0.,s2/n-pow(s1/n,2))) : 0.;
			thr[a] = mode - std::max(LOWMIN,LOWNS*sig);
			header.push_back(Form("# band run %d view %s mode %.4f sigma %.4f thr %.4f",R.first,a==0?"all":a==1?"S":"N",mode,sig,thr[a]));
		}
		int nl = 0;
		for (auto &s : R.second){ bool l = false; for (int a=0;a<3;a++) if (s.second[a]<thr[a]) l = true; if (l){ low.push_back({R.first,s.first}); nl++; } }
		printf("run %d: %d of %zu segments low (%.1f%%)\n",R.first,nl,R.second.size(),100.*nl/R.second.size());
	}
	//---- write
	printf("%zu of %zu segments low\n",low.size(),live.size());
	if (!write){ printf("write = false: no bad_segments.txt written\n"); return; }
	TString prod = setName[0]; { int n = 3; while (n<prod.Length() && isdigit(prod[n])) n++; prod.Remove(n); }
	for (auto &d : std::vector<TString>{prod}){
		gSystem->mkdir(ldir+"/"+d,true);
		TString fn = ldir+"/"+d+"/bad_segments.txt";
		FILE *fo = fopen(fn.Data(),"w");
		fprintf(fo,"# run segment -- below the run's live-fraction band in any view (all/S/N): live < mode - max(%.3f, %.0f sigma), band = mode +- %.2f\n",LOWMIN,LOWNS,BANDW);
		fprintf(fo,"# make_bad_segments.C(\"%s\"), %s cells, %zu of %zu segments; read by corral \"segveto\"\n",setlist,feeMode ? "FEE" : "generic",low.size(),live.size());
		for (auto &h : header) fprintf(fo,"%s\n",h.Data());
		for (auto &l : low) fprintf(fo,"%d %d\n",l.first,l.second);
		fclose(fo); printf("wrote %s\n",fn.Data());
	}
}
