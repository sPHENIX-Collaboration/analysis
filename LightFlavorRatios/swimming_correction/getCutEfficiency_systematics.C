#include "../util/HistogramTools.h"
#include "../config/binning.h"

void getCutEfficiency_systematics(std::string central_filename = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/cut_efficiency_correction.root",
                              std::string min_filename = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/systematics_min/cut_efficiency_correction.root",
                              std::string max_filename = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/systematics_max/cut_efficiency_correction.root",
                              std::string outfilename = "./syserr_test.root", std::string numerator_name = "Lambda0", std::string denominator_name = "K_S0", 
                              std::string numerator_title = "(#Lambda+#bar{#Lambda})", std::string denominator_title = "K_{S}^{0}")
{
  TFile* f_central = TFile::Open(central_filename.c_str());
  TFile* f_minthreshold = TFile::Open(min_filename.c_str());
  TFile* f_maxthreshold = TFile::Open(max_filename.c_str());
  TFile* f_out = new TFile(outfilename.c_str(),"RECREATE");

  std::vector<HistogramInfo> variables = 
  {
    BinInfo::final_pt_bins,
    BinInfo::final_eta_bins,
    BinInfo::final_phi_bins,
    BinInfo::final_rapidity_bins
  };

  for(HistogramInfo& h : variables)
  {
    TH1F* central_numerator_cuteff = (TH1F*)f_central->Get((numerator_name+"_cuteff_vs"+h.name).c_str());
    TH1F* central_denominator_cuteff = (TH1F*)f_central->Get((denominator_name+"_cuteff_vs"+h.name).c_str());
    TH1F* central_correction = (TH1F*)f_central->Get((numerator_name+"_over_"+denominator_name+"_cuteff_correction_vs"+h.name).c_str());
    TH1F* central_inverse_correction = (TH1F*)f_central->Get((denominator_name+"_over_"+numerator_name+"_cuteff_correction_vs"+h.name).c_str());

    TH1F* min_numerator_cuteff = (TH1F*)f_minthreshold->Get((numerator_name+"_cuteff_vs"+h.name).c_str());
    TH1F* min_denominator_cuteff = (TH1F*)f_minthreshold->Get((denominator_name+"_cuteff_vs"+h.name).c_str());
    TH1F* min_correction = (TH1F*)f_minthreshold->Get((numerator_name+"_over_"+denominator_name+"_cuteff_correction_vs"+h.name).c_str());
    TH1F* min_inverse_correction = (TH1F*)f_minthreshold->Get((denominator_name+"_over_"+numerator_name+"_cuteff_correction_vs"+h.name).c_str());

    TH1F* max_numerator_cuteff = (TH1F*)f_maxthreshold->Get((numerator_name+"_cuteff_vs"+h.name).c_str());
    TH1F* max_denominator_cuteff = (TH1F*)f_maxthreshold->Get((denominator_name+"_cuteff_vs"+h.name).c_str());
    TH1F* max_correction = (TH1F*)f_maxthreshold->Get((numerator_name+"_over_"+denominator_name+"_cuteff_correction_vs"+h.name).c_str());
    TH1F* max_inverse_correction = (TH1F*)f_maxthreshold->Get((denominator_name+"_over_"+numerator_name+"_cuteff_correction_vs"+h.name).c_str());

    f_out->cd();

    std::vector<std::string> syserr_names =
    {
      numerator_name+"_cuteff_syserr",
      denominator_name+"_cuteff_syserr",
      numerator_name+"_over_"+denominator_name+"_cuteff_correction_syserr",
      denominator_name+"_over_"+numerator_name+"_cuteff_correction_syserr"
    };

    std::vector<std::string> syserr_titles =
    {
      numerator_title+" cut efficiency systematic uncertainty",
      denominator_title+" cut efficiency systematic uncertainty",
      numerator_title+" / "+denominator_title+" cut efficiency correction systematic uncertainty",
      denominator_title+" / "+numerator_title+" cut efficiency correction systematic uncertainty"
    };

    std::vector<std::array<TH1F*,3>> syserr_triplets =
    {
      {central_numerator_cuteff,min_numerator_cuteff,max_numerator_cuteff},
      {central_denominator_cuteff,min_denominator_cuteff,max_denominator_cuteff},
      {central_correction,min_correction,max_correction},
      {central_inverse_correction,min_inverse_correction,max_inverse_correction}
    };

    for(int i=0;i<syserr_triplets.size();i++)
    {
      TH1F* syserr = makeHistogram(syserr_names[i].c_str(),syserr_titles[i].c_str(),h);
      for(int bin=1;bin<=syserr_triplets[i][0]->GetNbinsX();bin++)
      {
        std::vector<double> vals;
        for(TH1F* th : syserr_triplets[i])
        {
          vals.push_back(th->GetBinContent(bin));
        }
        std::sort(vals.begin(),vals.end());
        syserr->SetBinContent(bin,fabs(vals.back()-vals.front())/2.);
      }
      syserr->Write();
    }
  }
}
