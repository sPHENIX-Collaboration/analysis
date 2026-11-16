#include "../../util/HistogramTools.h"
#include "../../config/binning.h"

void get_geoAcceptance_syserr(std::string central_filename = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection/corrections/geo_acceptance_inclusive.root",
                              std::string minthreshold_filename = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection/corrections/geo_acceptance_inclusive_minthreshold.root",
                              std::string maxthreshold_filename = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection/corrections/geo_acceptance_inclusive_maxthreshold.root",
                              std::string outfilename = "./syserr_test.root", std::string numerator_name = "Lambda0", std::string denominator_name = "K_S0", 
                              std::string numerator_title = "(#Lambda+#bar{#Lambda})", std::string denominator_title = "K_{S}^{0}")
{
  TFile* f_central = TFile::Open(central_filename.c_str());
  TFile* f_minthreshold = TFile::Open(minthreshold_filename.c_str());
  TFile* f_maxthreshold = TFile::Open(maxthreshold_filename.c_str());
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
    TH1F* central_numerator_acceptance = (TH1F*)f_central->Get((numerator_name+"_acceptance_vs"+h.name).c_str());
    TH1F* central_denominator_acceptance = (TH1F*)f_central->Get((denominator_name+"_acceptance_vs"+h.name).c_str());
    TH1F* central_correction = (TH1F*)f_central->Get((numerator_name+"_over_"+denominator_name+"_geo_acceptance_correction_vs"+h.name).c_str());
    TH1F* central_inverse_correction = (TH1F*)f_central->Get((denominator_name+"_over_"+numerator_name+"_geo_acceptance_correction_vs"+h.name).c_str());

    TH1F* minthreshold_numerator_acceptance = (TH1F*)f_minthreshold->Get((numerator_name+"_acceptance_vs"+h.name).c_str());
    TH1F* minthreshold_denominator_acceptance = (TH1F*)f_minthreshold->Get((denominator_name+"_acceptance_vs"+h.name).c_str());
    TH1F* minthreshold_correction = (TH1F*)f_minthreshold->Get((numerator_name+"_over_"+denominator_name+"_geo_acceptance_correction_vs"+h.name).c_str());
    TH1F* minthreshold_inverse_correction = (TH1F*)f_minthreshold->Get((denominator_name+"_over_"+numerator_name+"_geo_acceptance_correction_vs"+h.name).c_str());

    TH1F* maxthreshold_numerator_acceptance = (TH1F*)f_maxthreshold->Get((numerator_name+"_acceptance_vs"+h.name).c_str());
    TH1F* maxthreshold_denominator_acceptance = (TH1F*)f_maxthreshold->Get((denominator_name+"_acceptance_vs"+h.name).c_str());
    TH1F* maxthreshold_correction = (TH1F*)f_maxthreshold->Get((numerator_name+"_over_"+denominator_name+"_geo_acceptance_correction_vs"+h.name).c_str());
    TH1F* maxthreshold_inverse_correction = (TH1F*)f_maxthreshold->Get((denominator_name+"_over_"+numerator_name+"_geo_acceptance_correction_vs"+h.name).c_str());

    f_out->cd();

    std::vector<std::string> syserr_names =
    {
      numerator_name+"_acceptance_syserr",
      denominator_name+"_acceptance_syserr",
      numerator_name+"_over_"+denominator_name+"_geo_acceptance_correction_syserr",
      denominator_name+"_over_"+numerator_name+"_geo_acceptance_correction_syserr"
    };

    std::vector<std::string> syserr_titles =
    {
      numerator_title+" geometric acceptance systematic uncertainty",
      denominator_title+" geometric acceptance systematic uncertainty",
      numerator_title+" / "+denominator_title+" geometric acceptance correction systematic uncertainty",
      denominator_title+" / "+numerator_title+" geometric acceptance correction systematic uncertainty"
    };

    std::vector<std::array<TH1F*,3>> syserr_triplets =
    {
      {central_numerator_acceptance,minthreshold_numerator_acceptance,maxthreshold_numerator_acceptance},
      {central_denominator_acceptance,minthreshold_denominator_acceptance,maxthreshold_denominator_acceptance},
      {central_correction,minthreshold_correction,maxthreshold_correction},
      {central_inverse_correction,minthreshold_inverse_correction,maxthreshold_inverse_correction}
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
