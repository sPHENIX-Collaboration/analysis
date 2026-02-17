#ifndef __HERWIGQAPLOTTINGCONFIG_H__
#define __HERWIGQAPLOTTIGNCONFIG_H__

#include <TH1.h>
#include <TH2.h>
#include <TLegend.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TPad.h>
#include "sPHENIXStyle.C"
#include "sPHENIXStyle.h"
#include "Special_colors.h"
#include <string>
#include <vector>
#include <format>

class HerwigQAPlottingConfig
{
	public:
		HerwigQAPlottingConfig(float herwig_xs=1., float pythia_xs=1.);
		~HerwigQAPlottongConfig(){};
		void ExtractType(TFile*);
		void DumpHistos(std::string);
		std::vector<TPad*>* AddPads(TCanvas*);
		void SetLegend(TLegend*);
		void SetsPhenixHeaderLegend(TLegend*);
		TH1F* GetRatioPlot(TH1F*, TH1F*);
		void ScaleXS(std::vector<TH1F*>*, bool);
	private:
		bool jet=false;
		bool photon=false;
		bool herwig=false;
		bool pythia=false;
		float HW_XS=1.;
		float PY_XS=1.;

};
#endif
