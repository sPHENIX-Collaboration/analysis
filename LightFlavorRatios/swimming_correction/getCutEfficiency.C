#include "../util/binning.h"

void getCutEfficiency(const std::string& infile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/merged_cutefficiency.root", const std::string& outfile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection/cut_efficiency_correction.root",const std::string& numerator_name = "Lambda0", const std::string& denominator_name = "K_S0")
{
  std::vector<HistogramInfo> variables = 
  {
    BinInfo::final_pt_bins,
    BinInfo::final_eta_bins,
    BinInfo::final_phi_bins,
    BinInfo::final_rapidity_bins
  };

  TFile* f = TFile::Open(infile.c_str());

  TFile* fout = new TFile(outfile.c_str(),"RECREATE");

  for(const HistogramInfo& var : variables)
  {
    TH1F* numerator_truth_h = (TH1F*)f->Get((numerator_name+"_truth_vs"+var.name).c_str());
    TH1F* numerator_reco_h = (TH1F*)f->Get((numerator_name+"_reco_vs"+var.name).c_str());
    TH1F* denominator_truth_h = (TH1F*)f->Get((denominator_name+"_truth_vs"+var.name).c_str());
    TH1F* denominator_reco_h = (TH1F*)f->Get((denominator_name+"_reco_vs"+var.name).c_str());

    TH1F* numerator_cuteff = makeHistogram((numerator_name+"_cuteff").c_str(),(numerator_name+" cut efficiency").c_str(),var);
    TH1F* denominator_cuteff = makeHistogram((denominator_name+"_cuteff").c_str(),(denominator_name+" cut efficiency").c_str(),var);

    TH1F* cuteff_correction = makeHistogram((numerator_name+"_over_"+denominator_name+"_cuteff_correction").c_str(),(numerator_name+"/"+denominator_name+" cut efficiency correction").c_str(),var);
    TH1F* cuteff_inverse_correction = makeHistogram((denominator_name+"_over_"+numerator_name+"_cuteff_correction").c_str(),(denominator_name+"/"+numerator_name+" cut efficiency correction").c_str(),var);

    numerator_cuteff->Divide(numerator_reco_h,numerator_truth_h);
    denominator_cuteff->Divide(denominator_reco_h,denominator_truth_h);

    cuteff_correction->Divide(numerator_cuteff,denominator_cuteff);
    cuteff_inverse_correction->Divide(denominator_cuteff,numerator_cuteff);

    numerator_truth_h->Write();
    numerator_reco_h->Write();
    denominator_truth_h->Write();
    denominator_reco_h->Write();
    numerator_cuteff->Write();
    denominator_cuteff->Write();
    cuteff_correction->Write();
    cuteff_inverse_correction->Write();
  }
}
