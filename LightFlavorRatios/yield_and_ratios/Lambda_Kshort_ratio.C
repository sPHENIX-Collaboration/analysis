#include "../bco_correction/V0DuplicateReader_mod.h"

#include "../util/RooFit_import_TTree.h"

#include "../corrections/EfficiencyCorrection.h"
#include "../corrections/LambdaFeedDownCorrection.h"
#include "../corrections/GeoAcceptanceCorrection.h"
#include "../corrections/TrivialEfficiencyCorrection.h"
#include "../corrections/CutEfficiencyCorrection.h"

#include "ResonanceRatio.h"
//#include "calculate_ratios.C"
#include "LambdaModel.h"
#include "KshortModel.h"

void Lambda_Kshort_ratio()
{
  TFile* Ks_file = TFile::Open("/sphenix/tg/tg01/hf/aopatton/SVLooseJun4/6RunsCombinedKShortSVLoose.root");
  TFile* lambda_file = TFile::Open("/sphenix/tg/tg01/hf/aopatton/SVLooseJun4/6RunsCombinedLambdaSVLoose.root");

  //TFile* Ks_file = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorResults/KShort6RunCombined.root");
  //TFile* lambda_file = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorResults/Lambda6RunCombined.root");

  //TFile* Ks_file = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorResults/Kshort_3runs.root");
  //TFile* lambda_file = TFile::Open("/sphenix/tg/tg01/hf/mjpeters/LightFlavorResults/Lambda_3runs.root");

  TTree* Ks_tree = (TTree*)Ks_file->Get("DecayTree");
  TTree* lambda_tree = (TTree*)lambda_file->Get("DecayTree");

  std::vector<HistogramInfo> diff_variables =
  {
    BinInfo::final_pt_bins,
    BinInfo::final_eta_bins,
    BinInfo::final_rapidity_bins,
    BinInfo::final_phi_bins,
  };

  RooArgList Ks_args;
  RooArgList lambda_args;

  RooRealVar m_ks("K_S0_mass","K_S0_mass",0.4,0.6);
  RooRealVar m_lambda("Lambda0_mass","Lambda0_mass",1.08,1.15);

  Ks_args.add(m_ks);
  lambda_args.add(m_lambda);

  std::vector<RooRealVar> Ks_diffvars;
  std::vector<RooRealVar> lambda_diffvars;

  std::vector<RooRealVar> Ks_cutvars;
  std::vector<RooRealVar> lambda_cutvars;

  for(HistogramInfo& hinfo : diff_variables)
  {
    std::string Ks_branchname = "K_S0_"+hinfo.name;
    std::string lambda_branchname = "Lambda0_"+hinfo.name;
    std::cout << Ks_branchname << " " << lambda_branchname << std::endl;
    Ks_diffvars.push_back(make_var(Ks_branchname,Ks_branchname,Ks_tree));
    lambda_diffvars.push_back(make_var(lambda_branchname,lambda_branchname,lambda_tree));
  }

  for(int i=0; i<Ks_diffvars.size(); i++)
  {
    Ks_args.add(Ks_diffvars[i]);
    lambda_args.add(lambda_diffvars[i]);
  }

  HistogramInfo Ks_massbins = BinInfo::mass_bins.at("K_S0");
  HistogramInfo Lambda_massbins = BinInfo::mass_bins.at("Lambda0");

  for(const std::string& cutvar : Ks_massbins.cut_vars)
  {
    Ks_cutvars.push_back(make_var(cutvar,cutvar,Ks_tree));
  }

  for(const std::string& cutvar : Lambda_massbins.cut_vars)
  {
    lambda_cutvars.push_back(make_var(cutvar,cutvar,lambda_tree));
  }

  for(int i=0; i<Ks_cutvars.size(); i++)
  {
    Ks_args.add(Ks_cutvars[i]);
  }

  for(int i=0; i<lambda_cutvars.size(); i++)
  {
    lambda_args.add(lambda_cutvars[i]);
  }

  std::string Ks_cuts = Ks_massbins.cut_string;
  std::string Lambda_cuts = BinInfo::mass_bins.at("Lambda0").cut_string;

  Ks_args.Print();
  lambda_args.Print();

  RooDataSet* Ks_ds = new RooDataSet("K_S0","K_S0",Ks_args,RooFit::Import(*Ks_tree));
  RooDataSet* lambda_ds = new RooDataSet("Lambda0","Lambda0",lambda_args,RooFit::Import(*lambda_tree));
/*
  V0DuplicateReader ks_reader(Ks_tree, V0DuplicateReader::ParticleType::K0s);
  V0DuplicateReader lambda_reader(lambda_tree, V0DuplicateReader::ParticleType::Lambda);

  ks_reader.enableDeltaBCOCut(0, 350);
  lambda_reader.enableDeltaBCOCut(0, 350);

  for (Long64_t i = 0; i < ks_reader.entries(); ++i)
  {
    if(i % 10000 == 0) std::cout << "processing BCO for Kshorts entry " << i << " / " << ks_reader.entries() << std::endl;
    ks_reader.loadEntry(i);

    if (!ks_reader.passesDeltaBCOCut()) continue;
    if (!ks_reader.isCurrentEntryUnique()) continue;

    m_ks.setVal(ks_reader.get<float>("K_S0_mass"));

    for(size_t idiff = 0; idiff < diff_variables.size(); idiff++)
    {
      Ks_diffvars[idiff].setVal(ks_reader.get<float>(Ks_diffvars[idiff].GetName()));
    }

    for(size_t icut = 0; icut < Ks_cutvars.size(); icut++)
    {
      Ks_cutvars[icut].setVal(ks_reader.get<float>(Ks_cutvars[icut].GetName()));
    }

    Ks_ds->add(Ks_args);
  }

  for (Long64_t i = 0; i < lambda_reader.entries(); ++i)
  {
    if(i % 10000 == 0) std::cout << "processing BCO for lambda entry " << i << " / " << lambda_reader.entries() << std::endl;
    lambda_reader.loadEntry(i);

    if (!lambda_reader.passesDeltaBCOCut()) continue;
    if (!lambda_reader.isCurrentEntryUnique()) continue;

    m_lambda.setVal(lambda_reader.get<float>("Lambda0_mass"));

    for(size_t idiff = 0; idiff < diff_variables.size(); idiff++)
    {
      lambda_diffvars[idiff].setVal(lambda_reader.get<float>(lambda_diffvars[idiff].GetName()));
    }

    for(size_t icut = 0; icut < lambda_cutvars.size(); icut++)
    {
      lambda_cutvars[icut].setVal(lambda_reader.get<float>(lambda_cutvars[icut].GetName()));
    }

    lambda_ds->add(lambda_args);
  }
*/
  RooDataSet* Ks_ds_withcuts = (RooDataSet*)Ks_ds->reduce(Ks_args,Ks_cuts.c_str());
  RooDataSet* lambda_ds_withcuts = (RooDataSet*)lambda_ds->reduce(lambda_args,Lambda_cuts.c_str());

  std::cout << "Ks_cuts " << Ks_cuts << std::endl;
  std::cout << "Lamdba_cuts " << Lambda_cuts << std::endl;

  KshortModel kshort_model;
  LambdaModel lambda_model;

  std::string fd_filename = "/sphenix/tg/tg01/hf/hjheng/HF-analysis/simulation/Pythia_ppMinBias/cascade_feeddown/Cascade_feeddown_fraction.root";
  std::vector<std::vector<std::shared_ptr<CorrectionHistogram1D>>> corrections(diff_variables.size());
  // pT
  corrections[0].push_back(std::make_shared<LambdaFeedDownCorrection>(fd_filename,"h_feeddown_frac_xi_all"));
  corrections[0].push_back(std::make_shared<EfficiencyCorrection>());
//  corrections[0].push_back(std::make_shared<GeoAcceptanceCorrection>("/sphenix/u/cdean/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots/Lambda0_to_KS0_geometric_acceptance_ratio_pT.root","Lambda0_inGeo_pT"));
  corrections[0].push_back(std::make_shared<GeoAcceptanceCorrection>("/sphenix/tg/tg01/hf/gregoryottino/lightFlavorPpg16/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots_systemtics/Lambda0_to_KS0_geometric_acceptance_ratio_pT.root","Lambda0_inGeo_pT"));
  corrections[0].push_back(std::make_shared<CutEfficiencyCorrection>("../swimming_correction/LamdbaKsCutEfficiency_200MeV_hists.root","hEffRatio_pT"));

  // eta
  corrections[1].push_back(std::make_shared<LambdaFeedDownCorrection>(fd_filename,"h_feeddown_frac_xi_eta_all"));
  corrections[1].push_back(std::make_shared<TrivialEfficiencyCorrection>(""));
//  corrections[1].push_back(std::make_shared<GeoAcceptanceCorrection>("/sphenix/u/cdean/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots/Lambda0_to_KS0_geometric_acceptance_ratio_eta.root","Lambda0_inGeo_#eta"));
  corrections[1].push_back(std::make_shared<GeoAcceptanceCorrection>("/sphenix/tg/tg01/hf/gregoryottino/lightFlavorPpg16/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots_systemtics/Lambda0_to_KS0_geometric_acceptance_ratio_eta.root","Lambda0_inGeo_#eta"));
  corrections[1].push_back(std::make_shared<CutEfficiencyCorrection>("../swimming_correction/LamdbaKsCutEfficiency_200MeV_hists.root","hEffRatio_eta"));

  // rapidity
  corrections[2].push_back(std::make_shared<LambdaFeedDownCorrection>(fd_filename,"h_feeddown_frac_xi_rapidity_all"));
  corrections[2].push_back(std::make_shared<TrivialEfficiencyCorrection>(""));
//  corrections[2].push_back(std::make_shared<GeoAcceptanceCorrection>("/sphenix/u/cdean/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots/Lambda0_to_KS0_geometric_acceptance_ratio_rap.root","Lambda0_inGeo_y"));
  corrections[2].push_back(std::make_shared<GeoAcceptanceCorrection>("/sphenix/tg/tg01/hf/gregoryottino/lightFlavorPpg16/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots_systemtics/Lambda0_to_KS0_geometric_acceptance_ratio_rap.root","Lambda0_inGeo_y"));
  corrections[2].push_back(std::make_shared<CutEfficiencyCorrection>("../swimming_correction/LamdbaKsCutEfficiency_200MeV_hists.root","hEffRatio_y"));

  // phi
  corrections[3].push_back(std::make_shared<LambdaFeedDownCorrection>(fd_filename,"h_feeddown_frac_xi_phi_all"));
  corrections[3].push_back(std::make_shared<TrivialEfficiencyCorrection>(""));
//  corrections[3].push_back(std::make_shared<GeoAcceptanceCorrection>("/sphenix/u/cdean/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots/Lambda0_to_KS0_geometric_acceptance_ratio_phi.root","Lambda0_inGeo_#phi"));
  corrections[3].push_back(std::make_shared<GeoAcceptanceCorrection>("/sphenix/tg/tg01/hf/gregoryottino/lightFlavorPpg16/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots_systemtics/Lambda0_to_KS0_geometric_acceptance_ratio_phi.root","Lambda0_inGeo_#phi"));
  corrections[3].push_back(std::make_shared<CutEfficiencyCorrection>("../swimming_correction/LamdbaKsCutEfficiency_200MeV_hists.root","hEffRatio_phi"));

  TFile* fout = new TFile("fits.root","RECREATE");

  ResonanceRatio analyzer(lambda_model,kshort_model,
                          fout,"lambdaKsratio","#Lambda/2K_{S}^{0} ratio (daughter pT > 200 MeV)",1./2.,false,
                          diff_variables,corrections);

  analyzer.calculate_ratios_unbinned(lambda_ds_withcuts,Ks_ds_withcuts);
}
