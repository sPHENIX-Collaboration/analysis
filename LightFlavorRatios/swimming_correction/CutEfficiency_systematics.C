#include "CutEfficiency_mjp.C"
#include "PVSVUncertainty.h"

// cuts producing the minimum and maximum cut efficiency allowed by the PV and SV uncertainty
std::pair<CutSettings,CutSettings> uncertainty_modified_cuts(const CutSettings& cuts, const PVSVUncertainty& pvsv)
{
  CutSettings new_cuts_min = cuts;
  CutSettings new_cuts_max = cuts;

  double mean_PV_sigma = pvsv.PV_sigma->GetMean();
  double mean_PV_sigma_xy = pvsv.PV_sigma_xy->GetMean();
  double mean_SV_sigma = pvsv.SV_sigma->GetMean();
  double mean_track_1_PV_DCA_sigma = pvsv.track_1_PV_DCA_sigma->GetMean();
  double mean_track_2_PV_DCA_sigma = pvsv.track_2_PV_DCA_sigma->GetMean();
  double mean_track_1_PV_DCA_xy_sigma = pvsv.track_1_PV_DCA_xy_sigma->GetMean();
  double mean_track_2_PV_DCA_xy_sigma = pvsv.track_2_PV_DCA_xy_sigma->GetMean();

  new_cuts_min.track_1_PV_min_DCA_xy = cuts.track_1_PV_min_DCA_xy + mean_PV_sigma_xy + mean_track_1_PV_DCA_xy_sigma;
  new_cuts_min.track_2_PV_min_DCA_xy = cuts.track_2_PV_min_DCA_xy + mean_PV_sigma_xy + mean_track_2_PV_DCA_xy_sigma;
  new_cuts_min.track_1_track_2_max_DCA = cuts.track_1_track_2_max_DCA - mean_track_1_PV_DCA_sigma - mean_track_2_PV_DCA_sigma;
  new_cuts_min.track_1_track_2_max_DCA_xy = cuts.track_1_track_2_max_DCA_xy - mean_track_1_PV_DCA_xy_sigma - mean_track_2_PV_DCA_xy_sigma;
  // DIRA cut is more complicated than just the changing of a limit, so we put it in the misc_cutstring
  new_cuts_min.min_DIRA = -FLT_MAX;
  std::string min_min_DIRA_cut = "cos(acos("+cuts.mother_name+"_DIRA) + atan("+std::to_string(mean_PV_sigma)+"/"+cuts.mother_name+"_decayLength) + atan("+std::to_string(mean_SV_sigma)+"/"+cuts.mother_name+"_decayLength)) >= "+std::to_string(cuts.min_DIRA);
  if(!cuts.misc_cutstring.empty()) new_cuts_min.misc_cutstring += " && ";
  new_cuts_min.misc_cutstring += min_min_DIRA_cut;

  new_cuts_max.track_1_PV_min_DCA_xy = cuts.track_1_PV_min_DCA_xy - mean_PV_sigma_xy - mean_track_1_PV_DCA_xy_sigma;
  new_cuts_max.track_2_PV_min_DCA_xy = cuts.track_2_PV_min_DCA_xy - mean_PV_sigma_xy - mean_track_1_PV_DCA_xy_sigma;
  new_cuts_max.track_1_track_2_max_DCA = cuts.track_1_track_2_max_DCA + mean_track_1_PV_DCA_sigma + mean_track_2_PV_DCA_sigma;
  new_cuts_max.track_1_track_2_max_DCA_xy = cuts.track_1_track_2_max_DCA_xy + mean_track_1_PV_DCA_sigma + mean_track_2_PV_DCA_sigma;
  new_cuts_max.min_DIRA = -FLT_MAX;
  std::string max_min_DIRA_cut = "cos(acos("+cuts.mother_name+"_DIRA) - atan("+std::to_string(mean_PV_sigma)+"/"+cuts.mother_name+"_decayLength) - atan("+std::to_string(mean_SV_sigma)+"/"+cuts.mother_name+"_decayLength)) >= "+std::to_string(cuts.min_DIRA);
  if(!cuts.misc_cutstring.empty()) new_cuts_max.misc_cutstring += " && ";
  new_cuts_max.misc_cutstring += max_min_DIRA_cut;

  return {new_cuts_min, new_cuts_max};
}

void CutEfficiency_systematics(const std::string& merged_infile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/merged_cut_efficiency.root",
                               const std::string& numerator_name = "Lambda0",
                               const int numerator_pdgid = 3122,
                               const std::vector<int> numerator_daughters = {-211,2212},
                               const std::string& numerator_infile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/ppi_reco/outputKFParticle_ppi_reco_000001.root",
                               const bool numerator_include_opposite = true,
                               const CutSettings& numerator_cuts = StandardCuts::MC_Lambda0_cuts,
                               const HistogramInfo& numerator_massbins = BinInfo::Lambda0_MC_mass_bins,
                               const std::string& denominator_name = "K_S0",
                               const int denominator_pdgid = 310,
                               const std::vector<int> denominator_daughters = {211, -211},
                               const std::string& denominator_infile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/pipi_reco/outputKFParticle_pipi_reco_000001.root",
                               const bool denominator_include_opposite = false,
                               const CutSettings& denominator_cuts = StandardCuts::MC_K_S0_cuts,
                               const HistogramInfo& denominator_massbins = BinInfo::K_S0_MC_mass_bins,
                               const std::string& min_outfile = "test_min.root",
                               const std::string& max_outfile = "test_max.root")
{
  TFile* infile = TFile::Open(merged_infile.c_str());

  if(!infile)
  {
    std::cout << "ERROR: no nominal cut efficiency merged file detected; cut efficiency systematics determination requires that the nominal cut effficiency calculation already exists!" << std::endl;
    return;
  }

  PVSVUncertainty numerator_pvsv(infile,numerator_name);
  PVSVUncertainty denominator_pvsv(infile,denominator_name);

  std::pair<CutSettings,CutSettings> numerator_modified_cuts = uncertainty_modified_cuts(numerator_cuts,numerator_pvsv);
  std::pair<CutSettings,CutSettings> denominator_modified_cuts = uncertainty_modified_cuts(denominator_cuts,denominator_pvsv);

  CutEfficiency_mjp(numerator_name,numerator_pdgid,numerator_daughters,numerator_infile,numerator_include_opposite,
                    numerator_modified_cuts.first,numerator_massbins,
                    denominator_name,denominator_pdgid,denominator_daughters,denominator_infile,denominator_include_opposite,
                    denominator_modified_cuts.first,denominator_massbins,
                    min_outfile);

  CutEfficiency_mjp(numerator_name,numerator_pdgid,numerator_daughters,numerator_infile,numerator_include_opposite,
                    numerator_modified_cuts.second,numerator_massbins,
                    denominator_name,denominator_pdgid,denominator_daughters,denominator_infile,denominator_include_opposite,
                    denominator_modified_cuts.second,denominator_massbins,
                    max_outfile);
}
