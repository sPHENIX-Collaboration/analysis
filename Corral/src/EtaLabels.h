#ifndef ETALABELS_H
#define ETALABELS_H
//---- 2026-10-11: h+, h- pair types are filled in pseudorapidity (CalcRm fUseEta). Their pages and maps keep the
//---- code's y labels, so these rename them: dy -> d#eta, y1/y_{1} -> #eta_{1}, y2/y_{2} -> #eta_{2}, #Deltay -> #Delta#eta,
//---- R_{2yy} -> R_{2#eta#eta}. Whole tokens only (not inside a word or a name).
#include <TString.h>
#include <TVirtualPad.h>
#include <TList.h>
#include <TH1.h>
#include <TGraph.h>
#include <TText.h>
#include <TPaveText.h>
#include <TLegend.h>
#include <TLegendEntry.h>
// needs fluct_common.h and PairTypes.h included before it (they have no include guards)

inline bool IsHPairType(int ipaty){ return IsHadronParticleID(PairTypes_Info[ipaty][0]) || IsHadronParticleID(PairTypes_Info[ipaty][1]); }

inline TString YToEta(const TString& in){
	static const char* MAP[][2]	= { {"R_{2yy}","R_{2#eta#eta}"}, {"#Deltay","#Delta#eta"}, {"y_{1}","#eta_{1}"}, {"y_{2}","#eta_{2}"},
									{"dy","d#eta"}, {"y1","#eta_{1}"}, {"y2","#eta_{2}"} };
	auto word	= [](char c){ return isalnum((unsigned char)c) || c=='_'; };
	TString out;
	const int n	= in.Length();
	for (int i=0;i<n;){
		bool done	= false;
		for (auto &m : MAP){
			const int l	= strlen(m[0]);
			if (i+l>n || TString(in(i,l))!=m[0]) continue;
			const bool startOK	= (m[0][0]=='#' || m[0][0]=='R') || i==0 || !word(in[i-1]);
			const bool endOK	= (m[0][l-1]=='}') || i+l==n || !word(in[i+l]);
			if (!startOK || !endOK) continue;
			out += m[1]; i += l; done = true; break;
		}
		if (!done){ out += in[i]; ++i; }
	}
	return out;
}

inline void EtaLabelsH(TH1* h){
	if (!h) return;
	h->SetTitle(YToEta(h->GetTitle()));
	h->GetXaxis()->SetTitle(YToEta(h->GetXaxis()->GetTitle()));
	h->GetYaxis()->SetTitle(YToEta(h->GetYaxis()->GetTitle()));
}

//---- every histogram, graph, text, pave and legend label on the pad and its sub-pads
inline void EtaLabels(TVirtualPad* pad){
	if (!pad || !pad->GetListOfPrimitives()) return;
	TIter next(pad->GetListOfPrimitives());
	while (TObject* o = next()){
		if (o->InheritsFrom(TVirtualPad::Class()))	EtaLabels((TVirtualPad*)o);
		else if (o->InheritsFrom(TH1::Class()))		EtaLabelsH((TH1*)o);
		else if (o->InheritsFrom(TGraph::Class())){	TGraph *g = (TGraph*)o; g->SetTitle(YToEta(g->GetTitle())); if (g->GetHistogram()) EtaLabelsH(g->GetHistogram()); }
		else if (o->InheritsFrom(TLegend::Class())){	TIter nl(((TLegend*)o)->GetListOfPrimitives()); while (TObject* e = nl()) ((TLegendEntry*)e)->SetLabel(YToEta(((TLegendEntry*)e)->GetLabel())); }
		else if (o->InheritsFrom(TPaveText::Class())){	TIter nl(((TPaveText*)o)->GetListOfLines()); while (TObject* t = nl()) if (t->InheritsFrom(TText::Class())) ((TText*)t)->SetTitle(YToEta(((TText*)t)->GetTitle())); }
		else if (o->InheritsFrom(TText::Class()))		((TText*)o)->SetTitle(YToEta(((TText*)o)->GetTitle()));
	}
	pad->Modified();
}
inline void EtaLabelsIf(TVirtualPad* pad, int ipaty){ if (IsHPairType(ipaty)){ EtaLabels(pad); pad->Update(); } }
#endif
