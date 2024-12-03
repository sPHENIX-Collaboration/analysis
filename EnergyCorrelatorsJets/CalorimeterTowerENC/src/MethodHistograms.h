#ifndef __METHODHISTOGRAMS_H__
#define __METHODHISTOGRAMS_H__

#include <TH1.h>
#include <string>
#include <vector>
struct MethodHistograms{
	MethodHistograms(std::string vers, float max=0.85, float bin=0.): typelabel(vers), maxR(max), binwidth(bin){
		if(binwidth > 0 ) nbins=maxR/binwidth;
		E2C=new TH1F(Form("e2c_%s", typelabel.c_str()), Form("2 point energy correlator measured from %s; R_{L}; #frac{ d #varepsilon_{2} }{d R_{L}}", typelabel.c_str()),nbins, -0.05, maxR); 
		E3C=new TH1F(Form("e3c_%s", typelabel.c_str()), Form("3 point energy correlator measured from %s; R_{L}; #frac{ d #varepsilon_{3} }{d R_{L}}", typelabel.c_str()), nbins, -0.05, maxR); 
		E2C_pt=new TH1F(Form("e2c_%s_jetpt", typelabel.c_str()), Form("2 point energy correlator measured from %s; R_{L}; #frac{ d #varepsilon_{2} }{d R_{L}}", typelabel.c_str()), nbins, -0.05, maxR); 
		E3C_pt=new TH1F(Form("e3c_%s_jetpt", typelabel.c_str()), Form("3 point energy correlator measured from %s; R_{L}; #frac{ d #varepsilon_{3} }{d R_{L}}", typelabel.c_str()), nbins, -0.05, maxR); 
		R=new TH1F(Form("R_%s", typelabel.c_str()), Form("#Delta R_{12} between compotents in jet from %s; #Delta R_{12}; < N >", typelabel.c_str()), nbins, -0.05, maxR);
		R_geom=new TH1F(Form("R_%s_geom", typelabel.c_str()), Form("#Delta R_{12} between canidate geometry in jet from %s; #Delta R_{12}; < N >", typelabel.c_str()), nbins, -0.05, maxR);
		E=new TH1F(Form("e_%s", typelabel.c_str()), Form("Jet energy from %s; E [GeV]; N_{jet}", typelabel.c_str()), 5000, -0.5, 49.5); 
		N=new TH1F(Form("n_%s", typelabel.c_str()), Form("N compoents from %s; N_{components}; N_{jet}", typelabel.c_str()), 50, -0.5, 49.5); 
		pt=new TH1F(Form("pt_%s", typelabel.c_str()), Form("Jet transverse energy from %s; p_{T} [GeV]; N_{jet}", typelabel.c_str()), 5000, -0.5, 49.5); 
		R_pt=new TH2F(Form("R_pt_%s", typelabel.c_str()), Form("p_{T} of consitituents relative to relative distance from jet axis from %s; R/R^{jet}_{max}; #frac{p_{T}^{consitituent}}{p_{T}^{jet}}; N_{consituent}", typelabel.c_str()), 20, -0.05, 0.95, 50, -0.05, 0.95);
	}
	std::string typelabel="NONE";
	float maxR=0.85;
	float binwidth=0.;
	int nbins=(maxR)/0.85*60; //set 60 bins for the 0.4 jet
	TH1F  *E2C=nullptr, *E3C=nullptr, *E2C_pt=nullptr, *E3C_pt=nullptr, *R=nullptr, *R_geom=nullptr, *E=nullptr, *N=nullptr, *pt=nullptr;
	TH2F* R_pt=nullptr;
	std::vector<TH1F*> histsvector {E2C, E3C,E2C_pt, E3C_pt, R,R_geom, E, N, pt};
};
#endif
