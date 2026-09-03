#include "../../util/binning.h"

void get_geoAcceptance(std::string numerator_infile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/geometricAcceptance/merged_inclusive_parity.root",
                       std::string denominator_infile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/geometricAcceptance/merged_kshort.root",
                       std::string outfile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection/corrections/geo_acceptance_inclusive.root",
                       std::string numerator_name = "Lambda0", std::string numerator_title = "(#Lambda+#bar{#Lambda})",
                       std::string denominator_name = "K_S0", std::string denominator_title = "K_{S}^{0}")
{
  std::vector<HistogramInfo> variables = 
  {
    BinInfo::final_pt_bins,
    BinInfo::final_eta_bins,
    BinInfo::final_phi_bins,
    BinInfo::final_rapidity_bins
  };

  TFile* f_numerator = TFile::Open(numerator_infile.c_str());
  TFile* f_denominator = TFile::Open(denominator_infile.c_str());
  TFile* fout = new TFile(outfile.c_str(),"RECREATE");

  for(HistogramInfo& h : variables)
  {
    TH1F* numerator_acceptance = makeHistogram((numerator_name+"_acceptance").c_str(),(numerator_title+" geometric acceptance").c_str(),h);
    TH1F* denominator_acceptance = makeHistogram((denominator_name+"_acceptance").c_str(),(denominator_title+" geometric acceptance").c_str(),h);

    std::string ratio_name = numerator_name+"_over_"+denominator_name+"_geo_acceptance_correction";
    std::string inverse_ratio_name = denominator_name+"_over_"+numerator_name+"_geo_acceptance_correction";
    std::string ratio_title = numerator_title+"/"+denominator_title+" geometric acceptance correction";
    std::string inverse_ratio_title = denominator_title+"/"+numerator_title+" geometric acceptance correction";
    TH1F* acceptance_correction = makeHistogram(ratio_name.c_str(),ratio_title.c_str(),h);
    TH1F* inverse_acceptance_correction = makeHistogram(inverse_ratio_name.c_str(),inverse_ratio_title.c_str(),h);

    TH1F* numerator_all = (TH1F*)f_numerator->Get((numerator_name+"_all_candidates_vs"+h.name).c_str());
    TH1F* numerator_reco = (TH1F*)f_numerator->Get((numerator_name+"_passing_candidates_vs"+h.name).c_str());
    TH1F* denominator_all = (TH1F*)f_denominator->Get((denominator_name+"_all_candidates_vs"+h.name).c_str());
    TH1F* denominator_reco = (TH1F*)f_denominator->Get((denominator_name+"_passing_candidates_vs"+h.name).c_str());

    numerator_all->Sumw2();
    numerator_reco->Sumw2();
    denominator_all->Sumw2();
    denominator_reco->Sumw2();

    numerator_all->Write();
    numerator_reco->Write();
    denominator_all->Write();
    denominator_reco->Write();

    numerator_acceptance->Divide(numerator_reco,numerator_all);
    denominator_acceptance->Divide(denominator_reco,denominator_all);

    acceptance_correction->Divide(numerator_acceptance,denominator_acceptance);
    inverse_acceptance_correction->Divide(denominator_acceptance,numerator_acceptance);

    numerator_acceptance->Write();
    denominator_acceptance->Write();
    acceptance_correction->Write();
    inverse_acceptance_correction->Write();
  }
}
