#ifndef __METHODHISTOGRAMS_H__
#define __METHODHISTOGRAMS_H__

#include <TH1.h>
#include <string>
#include <vector>
struct MethodHistograms{
	MethodHistograms(std::string vers): typelabel(vers){};
	std::string typelabel="NONE";
	TH1F* E2C=new TH1F(Form("e2c_%s", typelabel.c_str()), Form("2 point energy correlator measured from %s; R_{L}; #frac{ d #varepsilon_{2} }{d R_{L}}", typelabel.c_str()), 60, -0.05, 0.85); 
	TH1F* E3C=new TH1F(Form("e3c_%s", typelabel.c_str()), Form("3 point energy correlator measured from %s; R_{L}; #frac{ d #varepsilon_{3} }{d R_{L}}", typelabel.c_str()), 60, -0.05, 0.85); 
	TH1F* E2C_pt=new TH1F(Form("e2c_%s_jetpt", typelabel.c_str()), Form("2 point energy correlator measured from %s; R_{L}; #frac{ d #varepsilon_{2} }{d R_{L}}", typelabel.c_str()), 60, -0.05, 0.85); 
	TH1F* E3C_pt=new TH1F(Form("e3c_%s_jetpt", typelabel.c_str()), Form("3 point energy correlator measured from %s; R_{L}; #frac{ d #varepsilon_{3} }{d R_{L}}", typelabel.c_str()), 60, -0.05, 0.85); 
	TH1F* R=new TH1F(Form("R_%s", typelabel.c_str()), Form("#Delta R_{12} between compotents in jet from %s; #Delta R_{12}; < N >", typelabel.c_str()), 60, -0.05, 0.85);
	TH1F* R_geom=new TH1F(Form("R_%s_geom", typelabel.c_str()), Form("#Delta R_{12} between canidate geometry in jet from %s; #Delta R_{12}; < N >", typelabel.c_str()), 60, -0.05, 0.85);
	TH1F* E=new TH1F(Form("e_%s", typelabel.c_str()), Form("Jet energy from %s; E [GeV]; N_{jet}", typelabel.c_str()), 5000, -0.5, 49.5); 
	TH1F* N=new TH1F(Form("n_%s", typelabel.c_str()), Form("N compoents from %s; N_{components}; N_{jet}", typelabel.c_str()), 50, -0.5, 49.5); 
	TH1F* pt=new TH1F(Form("pt_%s", typelabel.c_str()), Form("Jet transverse energy from %s; p_{T} [GeV]; N_{jet}", typelabel.c_str()), 5000, -0.5, 49.5); 
	TH2F* R_pt=new TH2F(Form("R_pt_%s", typelabel.c_str()), Form("p_{T} of consitituents relative to relative distance from jet axis from %s; R/R^{jet}_{max}; #frac{p_{T}^{consitituent}}{p_{T}^{jet}}; N_{consituent}", typelabel.c_str()), 20, -0.05, 0.95, 50, -0.05, 0.95);
	std::vector<TH1F*> histsvector {E2C, E3C,E2C_pt, E3C_pt, R,R_geom, E, N, pt};
};
#endif
