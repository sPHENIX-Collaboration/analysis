#ifndef QPTPAGE_H
#define QPTPAGE_H
//---- QPTPage.h -- the ir2=2 (Q_inv, pT_pair) summary page, drawn the same way in the corral and the Finalize PDFs
//----   (README_meanpT_from_CQ.md sec 11). One page per pair type, 2x2:
//----   (1) C(Q) over all pT_pair, with the CQResonances.h signposts and the variable-binning note
//----   (2) C(Q, pT_pair) map
//----   (3) C(Q) in four pT_pair slices
//----   (4) <pT_pair> per Q bin, sibling and mixed (the ingredient of the <pT> moment method)
//---- S, M: sibling and mixed in any common normalisation (corral: zvtx-averaged rho2(S), rho2(M); Finalize: zvtx
//----   sums, pairs and mixed pairs in expected-sibling units); only ratios and means are drawn, so this does not matter.
//---- C1: optional C(Q) over all pT_pair with better errors (Finalize: subgroup errors); 0 = built from S and M.
//---- Needs CQResonances.h (QPT_NOTE) included before it.
#include <TCanvas.h>
#include <TH2.h>
#include <TH1D.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <vector>
#include <algorithm>

void DrawCQSignposts(int ipid1, int ipid2, double ymin, double ymax, double xmax);	// corral_loop.cxx

//---- C = s/m bin by bin; error from both (relative errors in quadrature); bins with s or m empty stay empty
inline TH1D* QPTRatio(TH1D *s, TH1D *m, const char* name){
	TH1D *c	= (TH1D*)s->Clone(name); c->SetDirectory(0); c->Reset();
	for (int b=1;b<=s->GetNbinsX();b++){
		double vs = s->GetBinContent(b), vm = m->GetBinContent(b);
		if (vs<=0. || vm<=0.) continue;
		double C = vs/vm, es = s->GetBinError(b), em = m->GetBinError(b);
		c->SetBinContent(b,C); c->SetBinError(b,C*sqrt(pow(es/vs,2)+pow(em/vm,2)));
	}
	return c;
}
//---- y range of a C(Q): 1st-99th percentile of the bins above Q = qlo that have an error (the lowest-Q and
//---- the sparse high-pT_pair bins are noisy and would set the range otherwise)
inline void QPTYRange(std::vector<TH1D*> hs, double qlo, double top){
	std::vector<double> v;
	for (TH1D *h : hs) for (int b=h->FindBin(qlo+1e-6);b<=h->GetNbinsX();b++) if (h->GetBinError(b)>0.) v.push_back(h->GetBinContent(b));
	if (v.size()<5) return;
	std::sort(v.begin(),v.end());
	double ymn = v[(size_t)(0.01*v.size())], ymx = v[(size_t)(0.99*v.size())];
	if (ymx>ymn){ hs[0]->SetMinimum(ymn-0.10*(ymx-ymn)); hs[0]->SetMaximum(ymx+top*(ymx-ymn)); }
}

inline void DrawQPTPage(TCanvas *c, TH2 *S, TH2 *M, TH1 *C1, int pid1, int pid2, const TString &pair, const TString &src, const TString &tag){
	c->Clear(); c->Divide(2,2,0.0001,0.0001);
	const double QLO	= 0.02;		// display ranges from Q > 20 MeV
	//---- (1) C(Q) over all pT_pair (under- and overflow included)
	c->cd(1);
	{
		TH1D *h;
		if (C1){ h = (TH1D*)C1->Clone(Form("QPT1_%s",tag.Data())); h->SetDirectory(0); }
		else {
			TH1D *s = S->ProjectionX(Form("QPT1s_%s",tag.Data()),0,-1,"e"), *m = M->ProjectionX(Form("QPT1m_%s",tag.Data()),0,-1,"e");
			h = QPTRatio(s,m,Form("QPT1_%s",tag.Data())); delete s; delete m;
		}
		h->SetTitle(Form("%s, C(Q), all p_{T,pair} (%s);Q_{inv} (GeV);C(Q)",pair.Data(),src.Data()));
		h->SetMarkerStyle(20); h->SetMarkerSize(0.3);
		QPTYRange({h},QLO,0.30); h->Draw("HIST"); h->Draw("E SAME"); gPad->Update();
		DrawCQSignposts(pid1,pid2,gPad->GetUymin(),gPad->GetUymax(),h->GetXaxis()->GetXmax());
		TLatex t; t.SetNDC(); t.SetTextFont(42); t.SetTextSize(0.032); t.SetTextColor(kRed+1);
		t.DrawLatex(0.15,0.87,QPT_NOTE);
	}
	//---- (2) C(Q, pT_pair) map, z range from the 2nd-98th percentile of the filled cells
	c->cd(2);
	{
		gPad->SetRightMargin(0.13);
		TH2 *h	= (TH2*)S->Clone(Form("QPT2_%s",tag.Data())); h->SetDirectory(0); h->Reset();
		std::vector<double> v;
		for (int i=1;i<=S->GetNbinsX();i++) for (int j=1;j<=S->GetNbinsY();j++){
			double vs = S->GetBinContent(i,j), vm = M->GetBinContent(i,j);
			if (vs>0. && vm>0.){ h->SetBinContent(i,j,vs/vm); v.push_back(vs/vm); }
		}
		h->SetTitle(Form("%s, C(Q,p_{T,pair}) (%s, white: no sibling or no mixed pairs);Q_{inv} (GeV);p_{T,pair} (GeV/c)",pair.Data(),src.Data()));
		if (v.size()>10){ std::sort(v.begin(),v.end()); h->SetMinimum(v[(size_t)(0.02*v.size())]); h->SetMaximum(v[(size_t)(0.98*v.size())]); }
		h->Draw("colz");
	}
	//---- (3) C(Q) in four pT_pair slices
	c->cd(3);
	{
		const double edge[5]	= {0.,0.5,1.,2.,5.};
		const int kcol[4]		= {kBlue,kGreen+2,kOrange+4,kRed};
		std::vector<TH1D*> hk;
		for (int k=0;k<4;k++){
			int j1 = S->GetYaxis()->FindBin(edge[k]+1e-6), j2 = S->GetYaxis()->FindBin(edge[k+1]-1e-6);
			TH1D *s = S->ProjectionX(Form("QPT3s_%s_%d",tag.Data(),k),j1,j2,"e"), *m = M->ProjectionX(Form("QPT3m_%s_%d",tag.Data(),k),j1,j2,"e");
			TH1D *h	= QPTRatio(s,m,Form("QPT3_%s_%d",tag.Data(),k)); delete s; delete m;
			h->SetLineColor(kcol[k]); h->SetMarkerColor(kcol[k]); h->SetMarkerStyle(20); h->SetMarkerSize(0.3);
			hk.push_back(h);
		}
		hk[0]->SetTitle(Form("%s, C(Q) in p_{T,pair} slices (%s);Q_{inv} (GeV);C(Q)",pair.Data(),src.Data()));
		QPTYRange(hk,QLO,0.30);
		hk[0]->Draw("E1"); for (int k=1;k<4;k++) hk[k]->Draw("E1 same");
		TLegend *lg	= new TLegend(0.55,0.68,0.89,0.89); lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.035);
		for (int k=0;k<4;k++){ TLegendEntry *le = lg->AddEntry(hk[k],Form("%.1f < p_{T,pair} < %.1f GeV/c",edge[k],edge[k+1]),"l"); le->SetTextColor(kcol[k]); }
		lg->Draw();
	}
	//---- (4) <pT_pair> per Q bin, sibling and mixed; error = rms / sqrt(effective entries) (= counts where S or M carry no errors)
	c->cd(4);
	{
		TH1D *mp[2];
		for (int sm=0;sm<2;sm++){
			TH2 *H	= sm ? M : S;
			mp[sm]	= H->ProjectionX(Form("QPT4_%s_%d",tag.Data(),sm),0,-1); mp[sm]->SetDirectory(0); mp[sm]->Reset();
			for (int i=1;i<=H->GetNbinsX();i++){
				double sw=0, swy=0, swy2=0, se2=0;
				for (int j=1;j<=H->GetNbinsY();j++){
					double w = H->GetBinContent(i,j), y = H->GetYaxis()->GetBinCenter(j);
					sw += w; swy += w*y; swy2 += w*y*y; se2 += pow(H->GetBinError(i,j),2);
				}
				if (sw<=0.) continue;
				double mean = swy/sw, rms = sqrt(std::max(0.,swy2/sw-mean*mean)), neff = (se2>0.) ? sw*sw/se2 : sw;	// no errors (Finalize mixed sums): counts
				mp[sm]->SetBinContent(i,mean); mp[sm]->SetBinError(i,rms/sqrt(neff));
			}
			mp[sm]->SetLineColor(sm ? kRed : kBlack); mp[sm]->SetMarkerColor(sm ? kRed : kBlack); mp[sm]->SetMarkerStyle(20); mp[sm]->SetMarkerSize(0.3);
		}
		mp[0]->SetTitle(Form("%s, #LTp_{T,pair}#GT per Q bin (%s, p_{T,pair} bin centres);Q_{inv} (GeV);#LTp_{T,pair}#GT (GeV/c)",pair.Data(),src.Data()));
		QPTYRange({mp[0],mp[1]},QLO,0.25);
		mp[0]->Draw("E1"); mp[1]->Draw("E1 same");
		TLegend *lg	= new TLegend(0.55,0.75,0.89,0.89); lg->SetBorderSize(0); lg->SetFillStyle(0); lg->SetTextSize(0.035);
		lg->AddEntry(mp[0],"sibling","l"); TLegendEntry *le = lg->AddEntry(mp[1],"mixed","l"); le->SetTextColor(kRed);
		lg->Draw();
	}
	c->cd(); c->Update();
}
#endif
