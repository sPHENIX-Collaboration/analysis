#include "../config/binning.h"
#include "../config/cuts.h"
#include "PVSVUncertainty.h"

void get_data_PVSVUncertainty(const std::string& numerator_infile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/bcoSkimmedDataSample/Lambda_run3pp_looseCuts_20260608_bco_skimmed.root",
                              const CutSettings& numerator_cuts = StandardCuts::data_Lambda0_cuts,
                              const std::string& denominator_infile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/bcoSkimmedDataSample/Kshort_run3pp_looseCuts_20260608_bco_skimmed.root",
                              const CutSettings& denominator_cuts = StandardCuts::data_K_S0_cuts,
                              const std::string& outfile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/data_pvsv_uncertainty.root")
{
  gROOT->SetBatch();

  TFile* f_numerator = TFile::Open(numerator_infile.c_str());
  TFile* f_denominator = TFile::Open(denominator_infile.c_str());

  TTree* t_numerator = (TTree*)f_numerator->Get("DecayTree");
  TTree* t_denominator = (TTree*)f_denominator->Get("DecayTree");

  TFile* f_out = new TFile(outfile.c_str(),"RECREATE");
  PVSVUncertainty pvsv_numerator(numerator_cuts.mother_name,numerator_cuts.mother_name,300,0.,0.05);
  PVSVUncertainty pvsv_denominator(denominator_cuts.mother_name,denominator_cuts.mother_name,300,0.,0.05);

  std::vector<HistogramInfo> variables =
  {
    BinInfo::final_pt_bins,
    BinInfo::final_eta_bins,
    BinInfo::final_phi_bins,
    BinInfo::final_rapidity_bins
  };

  std::string numerator_cutstring = generate_selection_cutstring(numerator_cuts,variables);
  std::string denominator_cutstring = generate_selection_cutstring(denominator_cuts,variables);

  t_numerator->Draw(("pow(primary_vertex_volume,1./3.)>>"+std::string(pvsv_numerator.PV_sigma->GetName())).c_str(),numerator_cutstring.c_str(),"goff");
  t_numerator->Draw(("sqrt(primary_vertex_Covariance[0]+primary_vertex_Covariance[2]-2.*primary_vertex_Covariance[1])>>"+std::string(pvsv_numerator.PV_sigma_xy->GetName())).c_str(),numerator_cutstring.c_str(),"goff");
  t_numerator->Draw(("pow("+numerator_cuts.mother_name+"_vertex_volume,1./3.)>>"+std::string(pvsv_numerator.SV_sigma->GetName())).c_str(),numerator_cutstring.c_str(),"goff");
  t_numerator->Draw(("track_1_IPErr>>"+std::string(pvsv_numerator.track_1_PV_DCA_sigma->GetName())).c_str(),numerator_cutstring.c_str(),"goff");
  t_numerator->Draw(("track_2_IPErr>>"+std::string(pvsv_numerator.track_2_PV_DCA_sigma->GetName())).c_str(),numerator_cutstring.c_str(),"goff");
  t_numerator->Draw(("sqrt(track_1_Covariance[0]+track_1_Covariance[2]-2.*track_1_Covariance[1])>>"+std::string(pvsv_numerator.track_1_PV_DCA_xy_sigma->GetName())).c_str(),numerator_cutstring.c_str(),"goff");
  t_numerator->Draw(("sqrt(track_2_Covariance[0]+track_2_Covariance[2]-2.*track_2_Covariance[1])>>"+std::string(pvsv_numerator.track_2_PV_DCA_xy_sigma->GetName())).c_str(),numerator_cutstring.c_str(),"goff");

  t_denominator->Draw(("pow(primary_vertex_volume,1./3.)>>"+std::string(pvsv_denominator.PV_sigma->GetName())).c_str(),denominator_cutstring.c_str(),"goff");
  t_denominator->Draw(("sqrt(primary_vertex_Covariance[0]+primary_vertex_Covariance[2]-2.*primary_vertex_Covariance[1])>>"+std::string(pvsv_denominator.PV_sigma_xy->GetName())).c_str(),denominator_cutstring.c_str(),"goff");
  t_denominator->Draw(("pow("+denominator_cuts.mother_name+"_vertex_volume,1./3.)>>"+std::string(pvsv_denominator.SV_sigma->GetName())).c_str(),denominator_cutstring.c_str(),"goff");
  t_denominator->Draw(("track_1_IPErr>>"+std::string(pvsv_denominator.track_1_PV_DCA_sigma->GetName())).c_str(),denominator_cutstring.c_str(),"goff");
  t_denominator->Draw(("track_2_IPErr>>"+std::string(pvsv_denominator.track_2_PV_DCA_sigma->GetName())).c_str(),denominator_cutstring.c_str(),"goff");
  t_denominator->Draw(("sqrt(track_1_Covariance[0]+track_1_Covariance[2]-2.*track_1_Covariance[1])>>"+std::string(pvsv_denominator.track_1_PV_DCA_xy_sigma->GetName())).c_str(),denominator_cutstring.c_str(),"goff");
  t_denominator->Draw(("sqrt(track_2_Covariance[0]+track_2_Covariance[2]-2.*track_2_Covariance[1])>>"+std::string(pvsv_denominator.track_2_PV_DCA_xy_sigma->GetName())).c_str(),denominator_cutstring.c_str(),"goff");

  pvsv_numerator.Write();
  pvsv_denominator.Write();
}
