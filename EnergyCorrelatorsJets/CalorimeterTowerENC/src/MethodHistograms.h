#ifndef __METHODHISTOGRAMS_H__
#define __METHODHISTOGRAMS_H__

#include <TH1.h>
#include <string>
#include <vector>
struct MethodHistograms{
	MethodHistograms(std::string vers): typelabel(vers){};
	std::string typelabel="NONE";
	TH1F* E2C=new TH1F(Form("e2c_%s", typelabel.c_str()), Form("2 point energy correlator measured from %s; R_{L}; #frac{ d #varepsilon_{2} }{d R_{L}}", typelabel.c_str()), 30, -0.05, 0.45); 
	TH1F* E3C=new TH1F(Form("e3c_%s", typelabel.c_str()), Form("3 point energy correlator measured from %s; R_{L}; #frac{ d #varepsilon_{3} }{d R_{L}}", typelabel.c_str()), 30, -0.05, 0.45); 
	TH1F* R=new TH1F(Form("R_%s", typelabel.c_str()), Form("#Delta R_{12} between compotents in jet from %s; #delta R_{12}; < N >", typelabel.c_str()), 30, -0.05, 0.45);
	TH1F* E=new TH1F(Form("e_%s", typelabel.c_str()), Form("Jet energy from %s; E [GeV]; N_{jet}", typelabel.c_str()), 50, -0.5, 49.5); 
	TH1F* N=new TH1F(Form("n_%s", typelabel.c_str()), Form("N compoents from %s; N_{components}; N_{jet}", typelabel.c_str()), 500, -0.5, 499.5); 
	std::vector<TH1F*> histsvector {E2C, E3C, R, E, N};
};
#endif
