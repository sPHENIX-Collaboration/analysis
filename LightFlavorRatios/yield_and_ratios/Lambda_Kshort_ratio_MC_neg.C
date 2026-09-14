#include "../corrections/EfficiencyCorrection.h"
#include "../corrections/TrivialLambdaFeedDownCorrection.h"
#include "../corrections/LambdaFeedDownCorrection.h"
#include "../corrections/GeoAcceptanceCorrection.h"
#include "../corrections/TrivialEfficiencyCorrection.h"
#include "../corrections/CutEfficiencyCorrection.h"

#include "../config/cuts.h"
#include "../config/binning.h"

#include "../util/HistogramTools.h"

#include "ResonanceRatio.h"

void Lambda_Kshort_ratio_MC_neg()
{
  TFile* lambda_file = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/merged_lambda_MC_neg.root");
  TFile* Ks_file = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/merged_Kshort_MC.root");

  //TFile* lambda_file = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/lambdaKshortMB/lambdaKshort_20260422_DetroitMB_CR_2_mode_pTref_1p4/ppi_reco/merged_lambda.root");
  //TFile* Ks_file = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/lambdaKshortMB/lambdaKshort_20260422_DetroitMB_CR_2_mode_pTref_1p4/pipi_reco/merged_kshort.root");

  //TFile* lambda_file = TFile::Open("/gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/LightFlavorRatios/geometric_acceptance/simulation/outputKFParticle_Lambda2ppi_reco_Usman_patch.root");
  //TFile* Ks_file = TFile::Open("/gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/LightFlavorRatios/geometric_acceptance/simulation/outputKFParticle_Kshort2pipi_reco_Usman_patch.root");

  //TFile* Ks_file = TFile::Open("/sphenix/tg/tg01/hf/cdean/LF_analysis/data_nTuples/output_Kshort_run3pp_looseCuts_20260608.root");
  //TFile* lambda_file = TFile::Open("/sphenix/tg/tg01/hf/cdean/LF_analysis/data_nTuples/output_Lambda0_run3pp_looseCuts_20260608.root");

  //TFile* Ks_file = TFile::Open("/sphenix/tg/tg01/hf/aopatton/SVLooseJun4/6RunsCombinedKShortSVLoose.root");
  //TFile* lambda_file = TFile::Open("/sphenix/tg/tg01/hf/aopatton/SVLooseJun4/6RunsCombinedLambdaSVLoose.root");

  //TFile* Ks_file = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorResults/KShort6RunCombined.root");
  //TFile* lambda_file = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorResults/Lambda6RunCombined.root");

  //TFile* Ks_file = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorResults/Kshort_3runs.root");
  //TFile* lambda_file = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorResults/Lambda_3runs.root");

  TH1F* integrated_lambda_mass = (TH1F*)lambda_file->Get("Lambda0_mass");
  TH1F* integrated_kshort_mass = (TH1F*)Ks_file->Get("K_S0_mass");

  std::vector<HistogramInfo> diff_variables =
  {
    BinInfo::final_pt_bins,
    BinInfo::final_eta_bins,
    BinInfo::final_rapidity_bins,
    BinInfo::final_phi_bins,
  };

  HistogramInfo Ks_massbins = BinInfo::K_S0_MC_mass_bins;
  HistogramInfo Lambda_massbins = BinInfo::Lambda0_MC_mass_bins;

  std::vector<DifferentialContainer> diff_lambda_data;
  std::vector<DifferentialContainer> diff_ks_data;

  for(HistogramInfo& hinfo : diff_variables)
  {
    diff_lambda_data.push_back(DifferentialContainer(lambda_file,"Lambda0",Lambda_massbins,hinfo));
    diff_ks_data.push_back(DifferentialContainer(Ks_file,"K_S0",Ks_massbins,hinfo));
  }

  CutSettings Ks_cuts = StandardCuts::MC_K_S0_cuts;
  CutSettings Lambda_cuts = StandardCuts::MC_Lambda0_neg_cuts;

  std::string fd_filename = "/sphenix/tg/tg01/hf/hjheng/HF-analysis/simulation/Pythia_ppMinBias/cascade_feeddown/Cascade_feeddown_fraction.root";
  std::string geoacc_filename = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection/corrections/geo_acceptance_lambdabaronly.root";
  std::string cuteff_filename = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/cut_efficiency_correction_neg.root";
  std::vector<std::vector<std::shared_ptr<CorrectionHistogram1D>>> corrections(diff_variables.size());
  // pT
  corrections[0].push_back(std::make_shared<TrivialLambdaFeedDownCorrection>(fd_filename,"h_feeddown_frac_xi_all"));
  corrections[0].push_back(std::make_shared<TrivialEfficiencyCorrection>(""));
//  corrections[0].push_back(std::make_shared<GeoAcceptanceCorrection>("/sphenix/u/cdean/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots/Lambda0_to_KS0_geometric_acceptance_ratio_pT.root","Lambda0_inGeo_pT"));
//  corrections[0].push_back(std::make_shared<GeoAcceptanceCorrection>("/sphenix/tg/tg01/hf/gregoryottino/lightFlavorPpg16/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots_systemtics/Lambda0_to_KS0_geometric_acceptance_ratio_pT.root","Lambda0_inGeo_pT"));
//  corrections[0].push_back(std::make_shared<CutEfficiencyCorrection>("../swimming_correction/LamdbaKsCutEfficiency_200MeV_hists.root","hEffRatio_pT"));
  corrections[0].push_back(std::make_shared<GeoAcceptanceCorrection>(geoacc_filename,"Lambda0_over_K_S0_geo_acceptance_correction_vspT"));
  corrections[0].push_back(std::make_shared<CutEfficiencyCorrection>(cuteff_filename,"Lambda0_over_K_S0_cuteff_correction_vspT"));
  // eta
  corrections[1].push_back(std::make_shared<TrivialLambdaFeedDownCorrection>(fd_filename,"h_feeddown_frac_xi_eta_all"));
  corrections[1].push_back(std::make_shared<TrivialEfficiencyCorrection>(""));
//  corrections[1].push_back(std::make_shared<GeoAcceptanceCorrection>("/sphenix/u/cdean/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots/Lambda0_to_KS0_geometric_acceptance_ratio_eta.root","Lambda0_inGeo_#eta"));
//  corrections[1].push_back(std::make_shared<GeoAcceptanceCorrection>("/sphenix/tg/tg01/hf/gregoryottino/lightFlavorPpg16/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots_systemtics/Lambda0_to_KS0_geometric_acceptance_ratio_eta.root","Lambda0_inGeo_#eta"));
//  corrections[1].push_back(std::make_shared<CutEfficiencyCorrection>("../swimming_correction/LamdbaKsCutEfficiency_200MeV_hists.root","hEffRatio_eta"));
  corrections[1].push_back(std::make_shared<GeoAcceptanceCorrection>(geoacc_filename,"Lambda0_over_K_S0_geo_acceptance_correction_vspseudorapidity"));
  corrections[1].push_back(std::make_shared<CutEfficiencyCorrection>(cuteff_filename,"Lambda0_over_K_S0_cuteff_correction_vspseudorapidity"));


  // rapidity
  corrections[2].push_back(std::make_shared<TrivialLambdaFeedDownCorrection>(fd_filename,"h_feeddown_frac_xi_rapidity_all"));
  corrections[2].push_back(std::make_shared<TrivialEfficiencyCorrection>(""));
//  corrections[2].push_back(std::make_shared<GeoAcceptanceCorrection>("/sphenix/u/cdean/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots/Lambda0_to_KS0_geometric_acceptance_ratio_rap.root","Lambda0_inGeo_y"));
//  corrections[2].push_back(std::make_shared<GeoAcceptanceCorrection>("/sphenix/tg/tg01/hf/gregoryottino/lightFlavorPpg16/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots_systemtics/Lambda0_to_KS0_geometric_acceptance_ratio_rap.root","Lambda0_inGeo_y"));
//  corrections[2].push_back(std::make_shared<CutEfficiencyCorrection>("../swimming_correction/LamdbaKsCutEfficiency_200MeV_hists.root","hEffRatio_y"));
  corrections[2].push_back(std::make_shared<GeoAcceptanceCorrection>(geoacc_filename,"Lambda0_over_K_S0_geo_acceptance_correction_vsrapidity"));
  corrections[2].push_back(std::make_shared<CutEfficiencyCorrection>(cuteff_filename,"Lambda0_over_K_S0_cuteff_correction_vsrapidity"));


  // phi
  corrections[3].push_back(std::make_shared<TrivialLambdaFeedDownCorrection>(fd_filename,"h_feeddown_frac_xi_phi_all"));
  corrections[3].push_back(std::make_shared<TrivialEfficiencyCorrection>(""));
//  corrections[3].push_back(std::make_shared<GeoAcceptanceCorrection>("/sphenix/u/cdean/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots/Lambda0_to_KS0_geometric_acceptance_ratio_phi.root","Lambda0_inGeo_#phi"));
//  corrections[3].push_back(std::make_shared<GeoAcceptanceCorrection>("/sphenix/tg/tg01/hf/gregoryottino/lightFlavorPpg16/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots_systemtics/Lambda0_to_KS0_geometric_acceptance_ratio_phi.root","Lambda0_inGeo_#phi"));
//  corrections[3].push_back(std::make_shared<CutEfficiencyCorrection>("../swimming_correction/LamdbaKsCutEfficiency_200MeV_hists.root","hEffRatio_phi"));
  corrections[3].push_back(std::make_shared<GeoAcceptanceCorrection>(geoacc_filename,"Lambda0_over_K_S0_geo_acceptance_correction_vsphi"));
  corrections[3].push_back(std::make_shared<CutEfficiencyCorrection>(cuteff_filename,"Lambda0_over_K_S0_cuteff_correction_vsphi"));

  TFile* fout = new TFile("fits_MC_neg.root","RECREATE");

  ResonanceRatio analyzer(3122,"Lambda0","#bar{#Lambda}",
                          310,"K_S0","K_{S}^{0}",
                          Lambda_massbins,Ks_massbins,Lambda_cuts,Ks_cuts,
                          fout,"lambdaKsratio","#bar{#Lambda^{0}}/K_{S}^{0} ratio",1.,false,
                          diff_variables,corrections);

  analyzer.calculate_ratios_binned(integrated_lambda_mass,diff_lambda_data,integrated_kshort_mass,diff_ks_data);
}
