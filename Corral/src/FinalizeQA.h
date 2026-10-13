#ifndef FINALIZEQA_H
#define FINALIZEQA_H
//---- FinalizeQA.h -- the front of the Finalize PDF and the run record of corral_Finalize.root (2026-10-12, README.md "info/"):
//----   title page (how corral was run: info/ of the chunks, left; contents, right), QA pages of the histograms summed over
//----   the chunks with every cut drawn (events and vertices, tracks, dE/dx PID, V0s), and the vs-segment pages (segCells of
//----   the chunks: one entry per DST segment in time order, segveto-skipped segments shaded).
//---- Included by corral_finalize.cxx only, after fluct_common.h, PairTypes.h, finalize_hists.h and SegLive.h.
#include <TCanvas.h>
#include <TPad.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TBox.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TKey.h>
#include <TNamed.h>
#include <TParameter.h>
#include <TTree.h>
#include <TDatime.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TSystem.h>
#include <TFile.h>
#include <TH1F.h>
#include <functional>
#include <map>
#include <vector>
#include <set>

namespace FinQA {

//---- info/ of the chunks: texts from the first chunk that has them, counters summed, cut values from the first chunk;
//---- every chunk must agree on the run string, the settings and the code (mismatches are listed on the title page)
struct Info {
	int			nchunks = 0, nwith = 0;
	TString		runstring, settings, code, segveto, listdir;
	std::vector<TString>			mismatch;
	std::map<TString,double>		counts, cuts;
	bool have() const { return nwith>0; }
	double cut(const char* k, double def) const { auto it = cuts.find(k); return it==cuts.end() ? def : it->second; }
	double count(const char* k) const { auto it = counts.find(k); return it==counts.end() ? -1. : it->second; }
};

inline Info ReadChunkInfo(const std::vector<TFile*> &fch){
	Info I; I.nchunks = fch.size();
	int nold	= 0;
	for (size_t ic=0;ic<fch.size();ic++){
		TDirectory *d	= fch[ic]->GetDirectory("info");
		if (!d){ ++nold; continue; }
		auto txt	= [&](const char* n){ TNamed *o = (TNamed*)d->Get(n); TString t = o ? o->GetTitle() : ""; delete o; return t; };
		TString rs = txt("runstring"), st = txt("settings"), cd = txt("code"), sv = txt("segveto"), ls = gSystem->DirName(txt("list"));
		if (I.nwith==0){ I.runstring = rs; I.settings = st; I.code = cd; I.segveto = sv; I.listdir = ls; }
		else {
			TString w;
			if (rs!=I.runstring) w += " run string";
			if (st!=I.settings)  w += " settings";
			if (cd!=I.code)      w += " code";
			if (sv!=I.segveto)   w += " segveto";
			if (w!="" && I.mismatch.size()<8) I.mismatch.push_back(Form("chunk %02zu differs from the first in:%s",ic,w.Data()));
		}
		TIter next(d->GetListOfKeys()); TKey *k;
		while ((k = (TKey*)next())){
			TString n = k->GetName();
			if (!n.BeginsWith("count_") && !n.BeginsWith("cut_")) continue;
			TParameter<double> *p = (TParameter<double>*)k->ReadObj(); if (!p) continue;
			if (n.BeginsWith("count_")) I.counts[n(6,n.Length())] += p->GetVal();
			else if (!I.cuts.count(n(4,n.Length()))) I.cuts[n(4,n.Length())] = p->GetVal();
			delete p;
		}
		++I.nwith;
	}
	if (nold>0) I.mismatch.insert(I.mismatch.begin(),Form("%d of %d chunks have no info/ (made before 2026-10-12)",nold,I.nchunks));
	return I;
}

//---- sum of a histogram over the chunks (0 if no chunk has it)
inline TH1* Sum(const std::vector<TFile*> &fch, const char* name){
	TH1 *s	= 0;
	for (TFile *f : fch){
		TH1 *h	= (TH1*)f->Get(name); if (!h) continue;
		if (!s){ s = (TH1*)h->Clone(Form("%s_fin",name)); s->SetDirectory(0); } else s->Add(h);
		delete h;
	}
	return s;
}

//---- drawing helpers: a vertical cut line (dashed), a shaded x window, a legend in the top right
inline TLine* VLine(double x, int col, int style=2){
	gPad->Update();
	double y0 = gPad->GetUymin(), y1 = gPad->GetUymax();
	if (gPad->GetLogy()){ y0 = pow(10,y0); y1 = pow(10,y1); }
	TLine *l = new TLine(x,y0,x,y1); l->SetLineColor(col); l->SetLineStyle(style); l->SetLineWidth(2); l->Draw(); return l;
}
inline TBox* XBand(double x0, double x1, int col, double alpha=0.25){
	gPad->Update();
	double y0 = gPad->GetUymin(), y1 = gPad->GetUymax();
	if (gPad->GetLogy()){ y0 = pow(10,y0); y1 = pow(10,y1); }
	TBox *b = new TBox(x0,y0,x1,y1); b->SetFillColorAlpha(col,alpha); b->SetLineWidth(0); b->Draw(); return b;
}
inline TLegend* Leg(double x0=0.50, double y0=0.72, double x1=0.89, double y1=0.89){
	TLegend *l = new TLegend(x0,y0,x1,y1); l->SetBorderSize(0); l->SetFillColorAlpha(kWhite,0.85); l->SetTextSize(0.040); return l;
}
//---- raw (black) and kept (red) overlay of two summed 1-D histograms
inline void Pair(TH1 *h0, TH1 *h1, const char* title, const char* l0, const char* l1, bool logy, TLegend *lg){
	if (!h0 && !h1) return;
	TH1 *f	= h0 ? h0 : h1;
	gPad->SetLogy(logy);
	f->SetTitle(title); f->SetStats(0);
	for (TH1 *h : {h0,h1}) if (h){ h->SetFillStyle(0); h->SetLineWidth(1); }
	if (h0){ h0->SetLineColor(kBlack); if (h1) h0->SetMaximum((logy ? 3. : 1.15)*std::max(h0->GetMaximum(),h1->GetMaximum())); h0->Draw("hist"); lg->AddEntry(h0,l0,"l"); }
	if (h1){ h1->SetLineColor(kRed); h1->Draw(h0 ? "hist same" : "hist"); lg->AddEntry(h1,l1,"l"); }
}

//---- the outline of the PDF (right side of the title page): sections, then the pair types in their order
inline void TitlePage(TCanvas *c, const TString &set, const Info &I, const TString &finRunString, double fz, const char* finCode,
                      const std::vector<int> &runs, double nevtPhys, const std::vector<TString> &pairNames, bool haveRef){
	c->Clear(); c->cd();
	TLatex t; t.SetNDC(); t.SetTextFont(42); t.SetTextAlign(13);
	double y = 0.965;
	auto line	= [&](const TString &s, double sz, int col=kBlack, double dy=0.){ t.SetTextSize(sz); t.SetTextColor(col); t.DrawLatex(0.025,y,s.Data()); y -= (dy>0 ? dy : 1.45*sz); };
	TDatime now;
	line(Form("Corral Finalize: %s",set.Data()),0.034);
	line(Form("%s;  Finalize run string \"%s\": physics |z_{vtx}| < %.0f cm;  Finalize code %s",now.AsSQLString(),finRunString.Data(),fz,finCode),0.0145);
	TString rl; for (int r : runs) rl += Form(" %d",r);
	line(Form("%d chunks (%s), runs:%s",I.nchunks,I.listdir!="" ? I.listdir.Data() : Form("lists/%s",set.Data()),rl.Data()),0.0145);
	if (!I.have()){
		line("the chunks carry no info/ (made before 2026-10-12): run string, settings and code not recorded",0.0145,kRed+1);
	} else {
		line(Form("chunk run string \"%s\";  chunk code %s",I.runstring.Data(),I.code.Data()),0.0145);
		line(Form("segment veto: %s",I.segveto.Data()),0.0145);
		y -= 0.006;
		line("Events",0.018);
		auto cnt	= [&](const char* k){ double v = I.count(k); return v<0 ? TString("?") : TString(Form("%.4g",v)); };
		line(Form("  rows read (vertices in the trees) %s;  skipped: overlapping-TF copies %s, segment veto %s, isolated-vertex %s",
			cnt("rows_read").Data(),cnt("rows_skipped_tfdup").Data(),cnt("rows_skipped_segveto").Data(),cnt("rows_skipped_isovtx").Data()),0.0145);
		line(Form("  outside the crossing selection %s;  events reaching the track loop (|z_{vtx}| < %.0f cm) %s",
			cnt("events_skipped_crossing").Data(),I.cut("vzmax",16.),cnt("events_tracks").Data()),0.0145);
		line(Form("  events in CalcRm with |z_{vtx}| < %.0f cm (the physics) %.4g",fz,nevtPhys),0.0145);
		y -= 0.006;
		line("Settings in force (identical in every chunk unless listed in red below)",0.018);
		TObjArray *a	= I.settings.Tokenize("\n");
		for (int i=0;i<a->GetEntries();i++) line(TString("  ")+((TObjString*)a->At(i))->GetName(),0.0135);
		delete a;
	}
	if (!I.mismatch.empty()){ y -= 0.006; for (auto &m : I.mismatch) line(m,0.0145,kRed+1); }
	else if (I.have()) line(Form("all %d chunks agree on run string, settings and code",I.nchunks),0.0145,kGreen+3);
	//---- right: contents
	const double xr	= 0.645;
	TLine *sep	= new TLine(xr-0.012,0.03,xr-0.012,0.97); sep->SetNDC(); sep->SetLineColor(kGray); sep->Draw();
	t.SetTextColor(kBlack); t.SetTextSize(0.020); t.DrawLatex(xr,0.965,"Contents");
	const char* sec[]	= {
		"Title page (this page)",
		"Events and vertices (summed over the chunks, cuts drawn)",
		"Tracks: kinematics, ntpc, DCA, acceptance",
		"dE/dx PID: KFP gates and the identified species",
		"V0 candidates: mass peaks (pair window shaded), p_{T}",
		"Versus DST segment (time order): events, vertices,",
		"     tracks, DCA, TPC live fraction (segveto shaded)",
		"Versus DST segment: identified particles and V0s per event",
		"Per pair type, in the order below: overview, C(Q),",
		"     resonance CF C(Q,p_{T}), crossing page",
		"Crossing-correction summary",
		haveRef ? "Pages vs the whole-dataset reference" : "(no whole-dataset reference: no reference pages)",
		"Crossing-loss (step 1b) pages per pair type, summaries",
		"N_{valid} maps" };
	double yr	= 0.93; t.SetTextSize(0.0135);
	for (const char* s : sec){ t.DrawLatex(xr,yr,s); yr -= 0.021; }
	yr -= 0.010; t.SetTextSize(0.016); t.DrawLatex(xr,yr,Form("Pair types (%zu)",pairNames.size())); yr -= 0.026;
	const int ncol	= 3, nrow = (pairNames.size()+ncol-1)/ncol;
	t.SetTextSize(0.0125);
	for (size_t k=0;k<pairNames.size();k++){
		int col = k/nrow, row = k%nrow;
		t.DrawLatex(xr+col*0.118, yr-row*0.0175, Form("%zu  %s",k,pairNames[k].Data()));
	}
}

//---- QA pages from the summed chunk histograms. Cut values: info/cut_* of the chunks (defaults if the chunks have none).
//---- out: the summed histograms (written to work/qa/ by the caller)
inline void QAPages(TCanvas *c, const char* pdf, const std::vector<TFile*> &fch, const Info &I, double fz, std::vector<TH1*> &out){
	const double ptmin = I.cut("ptmin",0.1), ntpcc = I.cut("ntpc",18), dxy = I.cut("dcaxy",1.5), dz = I.cut("dcaz",1.5);
	const double vz = I.cut("vzmax",16), vxy = I.cut("vxymax",0.5);
	const int xsel = (int)I.cut("xingsel",3), nmask = (int)I.cut("siphimask",NSIPHIMASK_MAIN);
	std::map<TString,TH1*> got;		// each histogram summed once (some are drawn on two pads)
	auto S	= [&](const char* n){ if (got.count(n)) return got[n]; TH1 *h = Sum(fch,n); if (h){ out.push_back(h); if (h->GetDimension()==1) h->SetFillStyle(0); } got[n] = h; return h; };
	const TString cutsrc	= I.have() ? "" : " (default cuts: the chunks record none)";
	//---------------- events and vertices
	c->Clear(); c->Divide(3,3,0.0001,0.0001);
	{
		TLegend *lg;
		const char* xyz[3]	= {"x","y","z"};
		for (int k=0;k<3;k++){
			c->cd(1+k); lg = Leg();
			TH1 *h0 = S(Form("hvtx%s0",xyz[k])), *h1 = S(Form("hvtx%s",xyz[k]));
			Pair(h0,h1,Form("vertex %s%s;vertex %s (cm);events",xyz[k],cutsrc.Data(),xyz[k]),"all rows","accepted events",true,lg);
			if (k<2){ VLine(-vxy,kBlue); TLine *l = VLine(vxy,kBlue); lg->AddEntry(l,Form("|%s| < %.1f cm",xyz[k],vxy),"l"); }
			else { VLine(-vz,kBlue); TLine *l = VLine(vz,kBlue); lg->AddEntry(l,Form("|z| < %.0f cm (jobs)",vz),"l");
				TBox *b = XBand(-fz,fz,kGreen+1); lg->AddEntry(b,Form("|z| < %.0f cm (physics)",fz),"f"); }
			lg->Draw();
		}
		c->cd(4); lg = Leg();
		Pair(S("hntrk0"),S("hntrk"),"tracks per event;n tracks;events","all tracks (rows)","accepted tracks",true,lg); lg->Draw();
		c->cd(5); lg = Leg(0.40,0.72,0.89,0.89);
		{ TH1 *h = S("hxing"); if (h){ gPad->SetLogy(1); h->SetTitle("crossing of the vertex, all rows;crossing;rows"); h->SetStats(0); h->SetLineColor(kBlack); h->Draw("hist");
			if (xsel==3){ TBox *b = XBand(corral::ZB_LO-0.5,corral::ZB_HI+0.5,kGreen+1); TBox *n = XBand(corral::XNEAR_LO-0.5,corral::XNEAR_HI+0.5,kWhite,1.);
				h->Draw("hist same"); lg->AddEntry(b,Form("ZB %d..%d (kept)",corral::ZB_LO,corral::ZB_HI),"f"); lg->AddEntry(n,Form("NearX0 %d..%d (out)",corral::XNEAR_LO,corral::XNEAR_HI),"f"); (void)n; }
			else lg->AddEntry((TObject*)0,xsel==0?"all crossings kept":xsel==1?"crossing 0 only":"NearX0 only","");
			lg->Draw(); } }
		c->cd(6); lg = Leg();
		{ TH2 *h2 = (TH2*)S("hnvtxCR"); if (h2){ TH1D *p = h2->ProjectionY("hnvtxCR_py_fin",h2->GetXaxis()->FindBin(-fz+1e-6),h2->GetXaxis()->FindBin(fz-1e-6)); p->SetDirectory(0);
			gPad->SetLogy(1); p->SetTitle(Form("vertices per TF (events in CalcRm, |z| < %.0f cm);vertices per TF;events",fz)); p->SetStats(0); p->SetLineColor(kBlack);
			p->GetXaxis()->SetRangeUser(0.,std::min(199.,p->GetMean()*4+10)); p->Draw("hist"); lg->AddEntry(p,Form("mean %.2f",p->GetMean()),"l"); lg->Draw(); } }
		c->cd(7); gPad->SetRightMargin(0.13);
		{ TH2 *h = (TH2*)S("hetazvtx"); if (h){ gPad->SetLogz(1); h->SetTitle("accepted tracks: #eta vs z_{vtx};z_{vtx} (cm);#eta"); h->SetStats(0); h->Draw("colz");
			VLine(-fz,kGreen+2,1); VLine(fz,kGreen+2,1); } }
		c->cd(8); lg = Leg(0.30,0.15,0.70,0.25);
		{ TH2 *h2 = (TH2*)S("hnvtxCR"); if (h2){ TH1D *p = h2->ProjectionX("hnvtxCR_px_fin"); p->SetDirectory(0); p->SetTitle("z_{vtx} of the events reaching CalcRm;z_{vtx} (cm);events"); p->SetStats(0); p->SetLineColor(kBlack); p->SetMinimum(0); p->SetMaximum(1.15*p->GetMaximum()); p->Draw("hist");
			TBox *b = XBand(-fz,fz,kGreen+1); p->Draw("hist same"); lg->AddEntry(b,Form("physics |z| < %.0f cm",fz),"f"); lg->Draw(); } }
		c->cd(9);
		{ TLatex t; t.SetNDC(); t.SetTextFont(42); t.SetTextSize(0.055); t.SetTextAlign(13); double y = 0.92;
		  auto L = [&](const TString &s){ t.DrawLatex(0.05,y,s.Data()); y -= 0.075; };
		  auto v = [&](const char* k){ double x = I.count(k); return x<0 ? TString("?") : TString(Form("%.4g",x)); };
		  L("#bf{Event flow} (summed over the chunks)");
		  L(Form("rows read: %s",v("rows_read").Data()));
		  L(Form("  - overlapping-TF copies: %s",v("rows_skipped_tfdup").Data()));
		  L(Form("  - segment veto: %s",v("rows_skipped_segveto").Data()));
		  L(Form("  - isolated-vertex selection: %s",v("rows_skipped_isovtx").Data()));
		  L(Form("  - crossing selection: %s",v("events_skipped_crossing").Data()));
		  L(Form("  - vertex x, y, z cuts (rest)"));
		  L(Form("events reaching the track loop: %s",v("events_tracks").Data()));
		  TH2 *h2 = (TH2*)S("hnvtxCR");
		  if (h2) L(Form("events in CalcRm, |z| < %.0f cm: %.4g",fz,h2->Integral(h2->GetXaxis()->FindBin(-fz+1e-6),h2->GetXaxis()->FindBin(fz-1e-6),0,-1))); }
	}
	c->cd(); c->Print(pdf);
	//---------------- tracks
	c->Clear(); c->Divide(3,3,0.0001,0.0001);
	{
		TLegend *lg;
		c->cd(1); lg = Leg(); Pair(S("hpt0"),S("hpt"),"p_{T};p_{T} (GeV/c);tracks","all tracks","accepted",true,lg);
		{ TLine *l = VLine(ptmin,kBlue); lg->AddEntry(l,Form("p_{T} > %.2f GeV/c",ptmin),"l"); } lg->Draw();
		c->cd(2); lg = Leg(0.35,0.15,0.75,0.32); Pair(S("heta0"),S("heta"),"#eta;#eta;tracks","all tracks","accepted",false,lg); lg->Draw();
		c->cd(3); lg = Leg(0.35,0.15,0.75,0.32); Pair(S("hphi0"),S("hphi"),"#phi at the vertex;#phi (rad);tracks","all tracks","accepted",false,lg);
		if (nmask>0){ TBox *b = 0; for (int k=0;k<nmask;k++) b = XBand(SIPHIMASK_LO[k]*M_PI/180.,SIPHIMASK_HI[k]*M_PI/180.,kGray+1,0.35); lg->AddEntry(b,"SiPhiMask (out)","f"); }
		lg->Draw();
		c->cd(4); lg = Leg(0.15,0.72,0.55,0.89); Pair(S("hntpc0"),S("hntpc"),"TPC clusters;ntpc;tracks","all tracks","accepted",false,lg);
		{ TLine *l = VLine(ntpcc-0.5,kBlue); lg->AddEntry(l,Form("ntpc #geq %.0f",ntpcc),"l"); } lg->Draw();
		c->cd(5); lg = Leg(); Pair(S("hdcaxy0"),S("hdcaxy"),"DCA_{xy};dca_{xy} (cm);tracks","all tracks","accepted",true,lg);
		{ VLine(-dxy,kBlue); TLine *l = VLine(dxy,kBlue); lg->AddEntry(l,Form("|dca_{xy}| < %.2f cm",dxy),"l"); } lg->Draw();
		c->cd(6); lg = Leg(); Pair(S("hdcaz0"),S("hdcaz"),"DCA_{z};dca_{z} (cm);tracks","all tracks","accepted",true,lg);
		{ VLine(-dz,kBlue); TLine *l = VLine(dz,kBlue); lg->AddEntry(l,Form("|dca_{z}| < %.2f cm",dz),"l"); } lg->Draw();
		c->cd(7); lg = Leg(); Pair(S("hnmvtx0"),S("hnmvtx"),"MVTX clusters;nmvtx;tracks","all tracks","accepted",false,lg); lg->Draw();
		c->cd(8); gPad->SetRightMargin(0.13);
		{ TH2 *h = (TH2*)S("hphieta"); if (h){ h->SetTitle("accepted tracks: #phi vs #eta;#eta;#phi (rad)"); h->SetStats(0); h->Draw("colz");
			if (nmask>0) for (int k=0;k<nmask;k++) for (double x : {SIPHIMASK_LO[k],SIPHIMASK_HI[k]}){ TLine *l = new TLine(-1.5,x*M_PI/180.,1.5,x*M_PI/180.); l->SetLineColor(kGray+2); l->SetLineStyle(2); l->Draw(); } } }
		c->cd(9); lg = Leg(0.15,0.72,0.55,0.89); Pair(S("hnintt0"),S("hnintt"),"INTT clusters;nintt;tracks","all tracks","accepted",false,lg); lg->Draw();
	}
	c->cd(); c->Print(pdf);
	//---------------- dE/dx PID: the KFP gates (dedxGates_KFP.root, as the chunks)
	c->Clear(); c->Divide(2,2,0.0001,0.0001);
	{
		const char* sp[4]	= {"pi","K","p","d"}; const int gcol[4] = {kBlue,kGreen+2,kRed,kGray+2};
		std::vector<TGraph*> g;
		TFile *fg	= TFile::Open((CorralDir()+"dedxGates_KFP.root").Data());
		if (fg && !fg->IsZombie()) for (int i=0;i<4;i++) for (const char* s : {"lo","hi"}){
			TGraph *x = (TGraph*)fg->Get(Form("g_%s_%s",sp[i],s)); if (!x) continue; x = (TGraph*)x->Clone(); x->SetLineColor(gcol[i]); x->SetLineStyle(2); x->SetLineWidth(2); g.push_back(x); }
		if (fg) fg->Close();
		const char* nm[4]	= {"hdedxKFPp","hdedxKFPp_id0","hdedxKFPp_id1","hdedxKFPp_id2"};
		const char* tt[4]	= {"all accepted tracks","PID #pi","PID K","PID p"};
		for (int k=0;k<4;k++){
			c->cd(1+k); gPad->SetLogz(1); gPad->SetRightMargin(0.13);
			TH1 *h = S(nm[k]); if (!h) continue;
			h->SetTitle(Form("dE/dx (KFP) vs p, %s;p (GeV/c);dE/dx (KFP)",tt[k])); h->SetStats(0); h->Draw("colz");
			for (TGraph *x : g) x->Draw("L same");
			if (k==0){ TLegend *lg = Leg(0.55,0.65,0.86,0.89); for (int i=0;i<4;i++){ TLine *l = new TLine(); l->SetLineColor(gcol[i]); l->SetLineStyle(2); l->SetLineWidth(2); lg->AddEntry(l,Form("%s gate",sp[i]),"l"); } lg->Draw(); }
		}
	}
	c->cd(); c->Print(pdf);
	//---------------- V0s: mass with the pair window (+-V0PEAK_NSIG sigma), p_T
	c->Clear(); c->Divide(3,2,0.0001,0.0001);
	{
		const char* mn[3]	= {"hKSmass","hLAmass","hALmass"}, *pn[3] = {"hKSpt","hLApt","hALpt"};
		const char* vn[3]	= {"K^{0}_{S}","#Lambda","#bar{#Lambda}"};
		for (int k=0;k<3;k++){
			c->cd(1+k); TLegend *lg = Leg(0.55,0.72,0.89,0.89);
			TH1 *h = S(mn[k]); if (!h) continue;
			h->SetTitle(Form("%s candidates;mass (GeV/c^{2});candidates",vn[k])); h->SetStats(0); h->SetLineColor(kBlack); h->SetMinimum(0); h->Draw("hist");
			TBox *b = XBand(V0PEAK_MU[k]-V0PEAK_NSIG*V0PEAK_SIG[k],V0PEAK_MU[k]+V0PEAK_NSIG*V0PEAK_SIG[k],kGreen+1); h->Draw("hist same");
			lg->AddEntry(h,Form("%.4g candidates",h->Integral()),"l"); lg->AddEntry(b,Form("in pairs: #pm%.0f#sigma",V0PEAK_NSIG),"f");
			if (k>0){ TH1 *hc = S(Form("%s_clean",mn[k])); if (hc){ hc->SetLineColor(kRed); hc->Draw("hist same"); lg->AddEntry(hc,"worse-dau. DCA cut","l"); } }
			lg->Draw();
			c->cd(4+k); gPad->SetLogy(1);
			TH1 *p = S(pn[k]); if (p){ p->SetTitle(Form("%s candidates, all masses;p_{T} (GeV/c);candidates",vn[k])); p->SetStats(0); p->SetLineColor(kBlack); p->Draw("hist"); }
		}
	}
	c->cd(); c->Print(pdf);
}

//---- vs-segment pages from the segCells trees (one entry per DST segment read, time order = chunk order, then entry order).
//---- Returns the per-segment table work/segQA (owned by the caller).
inline TTree* SegmentPages(TCanvas *c, const char* pdf, const std::vector<TFile*> &fch){
	struct Seg { int run, seg, chunk, veto, nraw, nevt, ntrk; float vtx, pt, ntpc, dxy, dz, live[3], nid[6], nv0[3]; };
	std::vector<Seg> v;
	bool haveQA = false;
	for (size_t ic=0;ic<fch.size();ic++){
		TTree *t	= (TTree*)fch[ic]->Get("segCells"); if (!t) continue;
		const bool qa	= t->GetBranch("nraw")!=0; haveQA |= qa;
		t->SetBranchStatus("*",0);
		Int_t run, seg, nevt, ntrk, nraw=0, veto=0, nid[6]={0}, nv0[3]={0}; Float_t nvtx=0, spt=0, sntpc=0, sxy=0, sz=0;
		for (const char* b : {"run","segment","nevt","ntrk"}) t->SetBranchStatus(b,1);
		t->SetBranchAddress("run",&run); t->SetBranchAddress("segment",&seg); t->SetBranchAddress("nevt",&nevt); t->SetBranchAddress("ntrk",&ntrk);
		if (qa){
			for (const char* b : {"nraw","veto","nvtx","sumpt","sumntpc","sumadcaxy","sumadcaz","nid","nv0"}) t->SetBranchStatus(b,1);
			t->SetBranchAddress("nraw",&nraw); t->SetBranchAddress("veto",&veto); t->SetBranchAddress("nvtx",&nvtx); t->SetBranchAddress("sumpt",&spt);
			t->SetBranchAddress("sumntpc",&sntpc); t->SetBranchAddress("sumadcaxy",&sxy); t->SetBranchAddress("sumadcaz",&sz);
			t->SetBranchAddress("nid",nid); t->SetBranchAddress("nv0",nv0);
		}
		for (Long64_t e=0;e<t->GetEntries();e++){
			t->GetEntry(e);
			Seg s; s.run = run; s.seg = seg; s.chunk = ic; s.veto = veto; s.nraw = qa ? nraw : -1; s.nevt = nevt; s.ntrk = ntrk;
			const double ne = nevt>0 ? nevt : 1, nt = ntrk>0 ? ntrk : 1;
			s.vtx = qa ? nvtx/ne : -1; s.pt = qa ? spt/nt : -1; s.ntpc = qa ? sntpc/nt : -1; s.dxy = qa ? sxy/nt : -1; s.dz = qa ? sz/nt : -1;
			for (int i=0;i<6;i++) s.nid[i] = qa ? nid[i]/ne : -1;
			for (int i=0;i<3;i++) s.nv0[i] = qa ? nv0[i]/ne : -1;
			for (int i=0;i<3;i++) s.live[i] = -1;
			v.push_back(s);
		}
		delete t;
	}
	if (v.empty()){ cout<<"corral::Finalize -- no segCells in the chunks: no vs-segment pages"<<endl; return 0; }
	{
		std::vector<TString> files; for (TFile *f : fch) files.push_back(f->GetName());
		SegLiveResult L	= SegLiveFractions(files);
		for (auto &s : v){ auto it = L.live.find({s.run,s.seg}); if (it!=L.live.end()) for (int i=0;i<3;i++) s.live[i] = it->second[i]; }
		cout<<"corral::Finalize -- vs-segment pages: "<<v.size()<<" segments, live fraction for "<<L.live.size()<<" ("<<(L.feeMode?"FEE":"generic")<<" cells)"<<endl;
	}
	const int N	= v.size();
	//---- one panel: quantity f(s) vs segment index, valid where f >= 0 and the segment has events; y range = 0.5-99.5% quantiles
	auto panel	= [&](std::vector<std::function<double(const Seg&)>> fs, std::vector<int> cols, std::vector<const char*> labs, const char* title, bool needEvt = true){
		std::vector<double> all;
		std::vector<TGraph*> g;
		for (size_t q=0;q<fs.size();q++){
			TGraph *x = new TGraph(); x->SetMarkerStyle(1); x->SetMarkerColor(cols[q]);
			for (int i=0;i<N;i++){ const Seg &s = v[i]; if (needEvt && s.nevt<=0) continue; double y = fs[q](s); if (y<0) continue; x->SetPoint(x->GetN(),i,y); all.push_back(y); }
			g.push_back(x);
		}
		if (all.empty()){ gPad->DrawFrame(0.,0.,1.,1.,""); TLatex t; t.SetNDC(); t.SetTextSize(0.06); t.DrawLatex(0.1,0.5,Form("%s: not in these chunks",title)); return; }
		std::sort(all.begin(),all.end());
		double lo = all[(size_t)(0.005*(all.size()-1))], hi = all[(size_t)(0.995*(all.size()-1))], d = hi-lo; if (d<=0) d = fabs(hi)*0.1+1e-6;
		lo -= 0.15*d; hi += 0.30*d;
		TH1F *fr = gPad->DrawFrame(-0.5,lo,N-0.5,hi,Form("%s;DST segment index (time order);",title));
		fr->GetXaxis()->SetLabelSize(0.045); fr->GetYaxis()->SetLabelSize(0.045);
		//---- segveto-skipped segments: gray strip at the bottom of the frame
		TH1D *hv	= new TH1D(Form("hveto_%p",(void*)fr),"",N,-0.5,N-0.5); hv->SetDirectory(0);
		int nv = 0; for (int i=0;i<N;i++){ hv->SetBinContent(i+1,lo); if (v[i].veto){ hv->SetBinContent(i+1,lo+0.08*(hi-lo)); ++nv; } }
		if (nv){ hv->SetFillColorAlpha(kGray+1,0.6); hv->SetLineWidth(0); hv->SetMinimum(lo); hv->Draw("hist same"); }
		for (int i=1;i<N;i++) if (v[i].run!=v[i-1].run){ TLine *l = new TLine(i-0.5,lo,i-0.5,hi); l->SetLineColor(kGray+2); l->SetLineStyle(2); l->Draw(); }
		for (auto x : g) x->Draw("P");
		TLegend *lg = Leg(0.12,0.80,0.89,0.89); lg->SetNColumns(std::min<int>(6,labs.size()+(nv?1:0)));
		for (size_t q=0;q<labs.size();q++){ TGraph *m = new TGraph(); m->SetMarkerStyle(20); m->SetMarkerColor(cols[q]); lg->AddEntry(m,labs[q],"p"); }
		if (nv){ lg->AddEntry(hv,Form("segveto (%d)",nv),"f"); }
		lg->Draw();
	};
	auto runLabels	= [&](){ TLatex t; t.SetTextSize(0.035); t.SetTextColor(kGray+2); t.SetTextAlign(13); gPad->Update();
		int i0 = 0; for (int i=1;i<=N;i++) if (i==N || v[i].run!=v[i-1].run){ t.DrawLatex(i0+0.01*N,gPad->GetUymin()+0.20*(gPad->GetUymax()-gPad->GetUymin()),Form("%d",v[i0].run)); i0 = i; } };
	//---------------- page 1: events, vertices, tracks, DCA, live fraction
	c->Clear(); c->Divide(3,3,0.0001,0.0001);
	c->cd(1); panel({[](const Seg &s){ return s.nraw>0 ? (double)s.nevt/s.nraw : -1.; }},{kBlack},{"events / rows"},"events reaching the track loop / rows read"); runLabels();
	c->cd(2); panel({[](const Seg &s){ return (double)s.vtx; }},{kBlack},{"#LTvertices per TF#GT"},"vertices in the trigger frame (occupancy)");
	c->cd(3); panel({[](const Seg &s){ return s.nevt>0 ? (double)s.ntrk/s.nevt : -1.; }},{kBlack},{"#LTn accepted tracks#GT"},"accepted tracks per event");
	c->cd(4); panel({[](const Seg &s){ return s.ntrk>0 ? (double)s.pt : -1.; }},{kBlack},{"#LTp_{T}#GT"},"#LTp_{T}#GT of accepted tracks (GeV/c)");
	c->cd(5); panel({[](const Seg &s){ return s.ntrk>0 ? (double)s.ntpc : -1.; }},{kBlack},{"#LTntpc#GT"},"#LTntpc#GT of accepted tracks");
	c->cd(6); panel({[](const Seg &s){ return s.ntrk>0 ? (double)s.dxy : -1.; }},{kBlack},{"#LT|dca_{xy}|#GT"},"#LT|dca_{xy}|#GT of accepted tracks (cm)");
	c->cd(7); panel({[](const Seg &s){ return s.ntrk>0 ? (double)s.dz : -1.; }},{kBlack},{"#LT|dca_{z}|#GT"},"#LT|dca_{z}|#GT of accepted tracks (cm)");
	c->cd(8); panel({[](const Seg &s){ return (double)s.live[0]; }},{kBlack},{"all"},"TPC live fraction (FEE cells, segveto rule)");
	c->cd(9); panel({[](const Seg &s){ return (double)s.live[1]; },[](const Seg &s){ return (double)s.live[2]; }},{kBlue,kRed},{"South","North"},"TPC live fraction, South and North");
	c->cd(); c->Print(pdf);
	//---------------- page 2: identified particles and V0s per event
	c->Clear(); c->Divide(2,3,0.0001,0.0001);
	c->cd(1); panel({[](const Seg &s){ return (double)s.nid[0]; },[](const Seg &s){ return (double)s.nid[1]; }},{kRed,kBlue},{"#pi^{+}","#pi^{-}"},"identified pions per event"); runLabels();
	c->cd(2); panel({[](const Seg &s){ return (double)s.nid[2]; },[](const Seg &s){ return (double)s.nid[3]; }},{kRed,kBlue},{"K^{+}","K^{-}"},"identified kaons per event");
	c->cd(3); panel({[](const Seg &s){ return (double)s.nid[4]; },[](const Seg &s){ return (double)s.nid[5]; }},{kRed,kBlue},{"p","#bar{p}"},"identified (anti)protons per event");
	c->cd(4); panel({[](const Seg &s){ return (double)s.nv0[0]; }},{kBlack},{"K^{0}_{S}"},"K^{0}_{S} in the mass window per event");
	c->cd(5); panel({[](const Seg &s){ return (double)s.nv0[1]; },[](const Seg &s){ return (double)s.nv0[2]; }},{kRed,kBlue},{"#Lambda","#bar{#Lambda}"},"#Lambda, #bar{#Lambda} in the mass window per event");
	c->cd(6); panel({[](const Seg &s){ return (double)s.nevt; }},{kBlack},{"events"},"events per segment");
	c->cd(); c->Print(pdf);
	//---- the table
	TTree *t	= new TTree("segQA","per DST segment, time order: QA means from the chunks' segCells (Finalize, README.md)");
	Seg s;
	t->Branch("run",&s.run,"run/I"); t->Branch("segment",&s.seg,"segment/I"); t->Branch("chunk",&s.chunk,"chunk/I"); t->Branch("veto",&s.veto,"veto/I");
	t->Branch("nraw",&s.nraw,"nraw/I"); t->Branch("nevt",&s.nevt,"nevt/I"); t->Branch("ntrk",&s.ntrk,"ntrk/I");
	t->Branch("vtx",&s.vtx,"vtx/F"); t->Branch("pt",&s.pt,"pt/F"); t->Branch("ntpc",&s.ntpc,"ntpc/F"); t->Branch("adcaxy",&s.dxy,"adcaxy/F"); t->Branch("adcaz",&s.dz,"adcaz/F");
	t->Branch("live",s.live,"live[3]/F"); t->Branch("nid",s.nid,"nid[6]/F"); t->Branch("nv0",s.nv0,"nv0[3]/F");
	for (auto &x : v){ s = x; t->Fill(); }
	t->SetDirectory(0);
	if (!haveQA) cout<<"corral::Finalize -- segCells without the QA branches (chunks made before 2026-10-12): events, tracks and live fraction only"<<endl;
	return t;
}

}	// namespace FinQA
#endif
