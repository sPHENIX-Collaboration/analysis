#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

void trim_1Dhist_meas(TH1D* h, std::string dataset);
void trim_1Dhist_truth(TH1D* h, std::string dataset);
void trim_2Dhist(TH2D* h, std::string dataset);
void combine_truthjet(TFile* f_MB,  TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out);
void combine_1Dhist(string histname, string surfix, std::string datatype, TFile* f_MB, TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out);
void combine_2Dhist(string histname, string surfix, TFile* f_MB, TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out);
void combine_1Dhist_noall(string histname, string surfix, std::string datatype, TFile* f_MB, TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out);
void combine_2Dhist_noall(string histname, string surfix, TFile* f_MB, TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out);
void combine_1Dhist_closure(string surfix, string tag, TFile* f_MB, TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out);
void combine_2Dhist_closure(string surfix, string tag, TFile* f_MB, TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out);

const double mb_cross_section = 4.197e+10;
const double jet5_cross_section = 1.369e+08;
const double jet10_cross_section = 3.997e+06;
const double jet15_cross_section = 4.073e+05;
const double jet20_cross_section = 6.218e+04;
const double jet30_cross_section = 2.502e+03;
const double jet50_cross_section = 7.269;
const double jet70_cross_section = 1.034e-02;

void get_combinedoutput(int radius_index = 4) {

  TFile* f_MB = new TFile(Form("output_sim_hadd/output_sim_r0%d_MB.root", radius_index), "READ");
  TFile* f_Jet5GeV = new TFile(Form("output_sim_hadd/output_sim_r0%d_Jet5GeV.root", radius_index), "READ");
  TFile* f_Jet10GeV = new TFile(Form("output_sim_hadd/output_sim_r0%d_Jet10GeV.root", radius_index), "READ");
  TFile* f_Jet15GeV = new TFile(Form("output_sim_hadd/output_sim_r0%d_Jet15GeV.root", radius_index), "READ");
  TFile* f_Jet20GeV = new TFile(Form("output_sim_hadd/output_sim_r0%d_Jet20GeV.root", radius_index), "READ");
  TFile* f_Jet30GeV = new TFile(Form("output_sim_hadd/output_sim_r0%d_Jet30GeV.root", radius_index), "READ");
  TFile* f_Jet50GeV = new TFile(Form("output_sim_hadd/output_sim_r0%d_Jet50GeV.root", radius_index), "READ");
  TFile* f_Jet70GeV = new TFile(Form("output_sim_hadd/output_sim_r0%d_Jet70GeV.root", radius_index), "READ");
  if (!f_MB || !f_Jet5GeV || !f_Jet10GeV || !f_Jet15GeV || !f_Jet20GeV || !f_Jet30GeV || !f_Jet50GeV || !f_Jet70GeV) {
    std::cout << "Error: cannot open one or more input files." << std::endl;
    return;
  }
  TFile* f_combined = new TFile(Form("output_sim_r0%d.root", radius_index), "RECREATE");

  combine_truthjet(f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  combine_1Dhist("h_recojet_pt_record_nocut", "", "reco", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_recojet_pt_record", "", "reco", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  // Combine nominal histograms
  combine_1Dhist("h_truth", "", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure", "", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist("h_respmatrix", "", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_fake", "", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_miss", "", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_matchedtruth_weighted", "", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_matchedtruth_unweighted", "", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure_unweighted", "", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  // Combine JES up variation histograms
  combine_1Dhist("h_truth", "_jesup", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure", "_jesup", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist("h_respmatrix", "_jesup", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_fake", "_jesup", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_miss", "_jesup", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_matchedtruth_weighted", "_jesup", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_matchedtruth_unweighted", "_jesup", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure_unweighted", "_jesup", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  // Combine JES down variation histograms
  combine_1Dhist("h_truth", "_jesdown", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure", "_jesdown", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist("h_respmatrix", "_jesdown", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_fake", "_jesdown", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_miss", "_jesdown", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_matchedtruth_weighted", "_jesdown", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_matchedtruth_unweighted", "_jesdown", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure_unweighted", "_jesdown", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  // Combine JER up variation histograms
  combine_1Dhist("h_truth", "_jerup", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure", "_jerup", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist("h_respmatrix", "_jerup", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_fake", "_jerup", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_miss", "_jerup", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_matchedtruth_weighted", "_jerup", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_matchedtruth_unweighted", "_jerup", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure_unweighted", "_jerup", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  // Combine JER down variation histograms
  combine_1Dhist("h_truth", "_jerdown", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure", "_jerdown", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist("h_respmatrix", "_jerdown", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_fake", "_jerdown", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_miss", "_jerdown", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_matchedtruth_weighted", "_jerdown", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_matchedtruth_unweighted", "_jerdown", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure_unweighted", "_jerdown", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  // Combine jet trigger up variation histograms
  combine_1Dhist("h_truth", "_jetup", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure", "_jetup", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist("h_respmatrix", "_jetup", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_fake", "_jetup", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_miss", "_jetup", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_matchedtruth_weighted", "_jetup", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_matchedtruth_unweighted", "_jetup", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure_unweighted", "_jetup", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  // Combine jet trigger down variation histograms
  combine_1Dhist("h_truth", "_jetdown", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure", "_jetdown", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist("h_respmatrix", "_jetdown", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_fake", "_jetdown", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_miss", "_jetdown", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_matchedtruth_weighted", "_jetdown", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_matchedtruth_unweighted", "_jetdown", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure_unweighted", "_jetdown", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  // Combine mbd trigger up variation histograms
  combine_1Dhist_noall("h_truth", "_mbdup", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_noall("h_measure", "_mbdup", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist_noall("h_respmatrix", "_mbdup", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_noall("h_fake", "_mbdup", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_noall("h_miss", "_mbdup", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_noall("h_matchedtruth_weighted", "_mbdup", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_noall("h_matchedtruth_unweighted", "_mbdup", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_noall("h_measure_unweighted", "_mbdup", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  // Combine mbd trigger down variation histograms
  combine_1Dhist_noall("h_truth", "_mbddown", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_noall("h_measure", "_mbddown", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist_noall("h_respmatrix", "_mbddown", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_noall("h_fake", "_mbddown", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_noall("h_miss", "_mbddown", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_noall("h_matchedtruth_weighted", "_mbddown", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_noall("h_matchedtruth_unweighted", "_mbddown", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_noall("h_measure_unweighted", "_mbddown", "meas", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  // full closure test
  combine_1Dhist_closure("h_fullclosure_", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_closure("h_fullclosure_", "measure", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist_closure("h_fullclosure_", "respmatrix", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_closure("h_fullclosure_", "fake", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_closure("h_fullclosure_", "miss", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  combine_1Dhist_closure("h_halfclosure_", "inputmeasure", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_closure("h_halfclosure_", "truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_closure("h_halfclosure_", "measure", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist_closure("h_halfclosure_", "respmatrix", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_closure("h_halfclosure_", "fake", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist_closure("h_halfclosure_", "miss", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
}

void combine_truthjet(TFile* f_MB,  TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out) {
  TH1D *h_MB_event_all = (TH1D*)f_MB->Get("h_event_all"); int mb_event_all = h_MB_event_all->GetBinContent(1); double mb_scale_all = mb_cross_section / (double)mb_event_all;
  TH1D *h_Jet5GeV_event_all = (TH1D*)f_Jet5GeV->Get("h_event_all"); int jet5_event_all = h_Jet5GeV_event_all->GetBinContent(1); double jet5_scale_all = jet5_cross_section / (double)jet5_event_all;
  TH1D *h_Jet10GeV_event_all = (TH1D*)f_Jet10GeV->Get("h_event_all"); int jet10_event_all = h_Jet10GeV_event_all->GetBinContent(1); double jet10_scale_all = jet10_cross_section / (double)jet10_event_all;
  TH1D *h_Jet15GeV_event_all = (TH1D*)f_Jet15GeV->Get("h_event_all"); int jet15_event_all = h_Jet15GeV_event_all->GetBinContent(1); double jet15_scale_all = jet15_cross_section / (double)jet15_event_all;
  TH1D *h_Jet20GeV_event_all = (TH1D*)f_Jet20GeV->Get("h_event_all"); int jet20_event_all = h_Jet20GeV_event_all->GetBinContent(1); double jet20_scale_all = jet20_cross_section / (double)jet20_event_all;
  TH1D *h_Jet30GeV_event_all = (TH1D*)f_Jet30GeV->Get("h_event_all"); int jet30_event_all = h_Jet30GeV_event_all->GetBinContent(1); double jet30_scale_all = jet30_cross_section / (double)jet30_event_all;
  TH1D *h_Jet50GeV_event_all = (TH1D*)f_Jet50GeV->Get("h_event_all"); int jet50_event_all = h_Jet50GeV_event_all->GetBinContent(1); double jet50_scale_all = jet50_cross_section / (double)jet50_event_all;
  TH1D *h_Jet70GeV_event_all = (TH1D*)f_Jet70GeV->Get("h_event_all"); int jet70_event_all = h_Jet70GeV_event_all->GetBinContent(1); double jet70_scale_all = jet70_cross_section / (double)jet70_event_all;

  string histname_all = "h_truthjet_pt_record_all";

  TH1D* h_MB_all_forcombine = (TH1D*)f_MB->Get(histname_all.c_str());
  TH1D* h_Jet5GeV_all_forcombine = (TH1D*)f_Jet5GeV->Get(histname_all.c_str());
  TH1D* h_Jet10GeV_all_forcombine = (TH1D*)f_Jet10GeV->Get(histname_all.c_str());
  TH1D* h_Jet15GeV_all_forcombine = (TH1D*)f_Jet15GeV->Get(histname_all.c_str());
  TH1D* h_Jet20GeV_all_forcombine = (TH1D*)f_Jet20GeV->Get(histname_all.c_str());
  TH1D* h_Jet30GeV_all_forcombine = (TH1D*)f_Jet30GeV->Get(histname_all.c_str());
  TH1D* h_Jet50GeV_all_forcombine = (TH1D*)f_Jet50GeV->Get(histname_all.c_str());
  TH1D* h_Jet70GeV_all_forcombine = (TH1D*)f_Jet70GeV->Get(histname_all.c_str());

  TH1D* h_all_combined = (TH1D*)h_MB_all_forcombine->Clone(histname_all.c_str());
  h_all_combined->Scale(mb_scale_all);
  h_all_combined->Add(h_Jet5GeV_all_forcombine, jet5_scale_all);
  h_all_combined->Add(h_Jet10GeV_all_forcombine, jet10_scale_all);
  h_all_combined->Add(h_Jet15GeV_all_forcombine, jet15_scale_all);
  h_all_combined->Add(h_Jet20GeV_all_forcombine, jet20_scale_all);
  h_all_combined->Add(h_Jet30GeV_all_forcombine, jet30_scale_all);
  h_all_combined->Add(h_Jet50GeV_all_forcombine, jet50_scale_all);
  h_all_combined->Add(h_Jet70GeV_all_forcombine, jet70_scale_all);

  f_out->cd();
  h_all_combined->Write();
}

void combine_1Dhist(string histname, string surfix, std::string datatype, TFile* f_MB,  TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out) {
  TH1D *h_MB_event_all = (TH1D*)f_MB->Get("h_event_all"); int mb_event_all = h_MB_event_all->GetBinContent(1); double mb_scale_all = mb_cross_section / (double)mb_event_all;
  TH1D *h_Jet5GeV_event_all = (TH1D*)f_Jet5GeV->Get("h_event_all"); int jet5_event_all = h_Jet5GeV_event_all->GetBinContent(1); double jet5_scale_all = jet5_cross_section / (double)jet5_event_all;
  TH1D *h_Jet10GeV_event_all = (TH1D*)f_Jet10GeV->Get("h_event_all"); int jet10_event_all = h_Jet10GeV_event_all->GetBinContent(1); double jet10_scale_all = jet10_cross_section / (double)jet10_event_all;
  TH1D *h_Jet15GeV_event_all = (TH1D*)f_Jet15GeV->Get("h_event_all"); int jet15_event_all = h_Jet15GeV_event_all->GetBinContent(1); double jet15_scale_all = jet15_cross_section / (double)jet15_event_all;
  TH1D *h_Jet20GeV_event_all = (TH1D*)f_Jet20GeV->Get("h_event_all"); int jet20_event_all = h_Jet20GeV_event_all->GetBinContent(1); double jet20_scale_all = jet20_cross_section / (double)jet20_event_all;
  TH1D *h_Jet30GeV_event_all = (TH1D*)f_Jet30GeV->Get("h_event_all"); int jet30_event_all = h_Jet30GeV_event_all->GetBinContent(1); double jet30_scale_all = jet30_cross_section / (double)jet30_event_all;
  TH1D *h_Jet50GeV_event_all = (TH1D*)f_Jet50GeV->Get("h_event_all"); int jet50_event_all = h_Jet50GeV_event_all->GetBinContent(1); double jet50_scale_all = jet50_cross_section / (double)jet50_event_all;
  TH1D *h_Jet70GeV_event_all = (TH1D*)f_Jet70GeV->Get("h_event_all"); int jet70_event_all = h_Jet70GeV_event_all->GetBinContent(1); double jet70_scale_all = jet70_cross_section / (double)jet70_event_all;

  string histname_all = histname + "_all" + surfix;
  string histname_zvertex30 = histname + "_zvertex30" + surfix;
  string histname_zvertex60 = histname + "_zvertex60" + surfix;

  TH1D* h_MB_all_forcombine = (TH1D*)f_MB->Get(histname_all.c_str());
  TH1D* h_Jet5GeV_all_forcombine = (TH1D*)f_Jet5GeV->Get(histname_all.c_str());
  TH1D* h_Jet10GeV_all_forcombine = (TH1D*)f_Jet10GeV->Get(histname_all.c_str());
  TH1D* h_Jet15GeV_all_forcombine = (TH1D*)f_Jet15GeV->Get(histname_all.c_str());
  TH1D* h_Jet20GeV_all_forcombine = (TH1D*)f_Jet20GeV->Get(histname_all.c_str());
  TH1D* h_Jet30GeV_all_forcombine = (TH1D*)f_Jet30GeV->Get(histname_all.c_str());
  TH1D* h_Jet50GeV_all_forcombine = (TH1D*)f_Jet50GeV->Get(histname_all.c_str());
  TH1D* h_Jet70GeV_all_forcombine = (TH1D*)f_Jet70GeV->Get(histname_all.c_str());
  TH1D* h_all_combined = (TH1D*)h_MB_all_forcombine->Clone(histname_all.c_str());
  h_all_combined->Scale(mb_scale_all);
  h_all_combined->Add(h_Jet5GeV_all_forcombine, jet5_scale_all);
  h_all_combined->Add(h_Jet10GeV_all_forcombine, jet10_scale_all);
  h_all_combined->Add(h_Jet15GeV_all_forcombine, jet15_scale_all);
  h_all_combined->Add(h_Jet20GeV_all_forcombine, jet20_scale_all);
  h_all_combined->Add(h_Jet30GeV_all_forcombine, jet30_scale_all);
  h_all_combined->Add(h_Jet50GeV_all_forcombine, jet50_scale_all);
  h_all_combined->Add(h_Jet70GeV_all_forcombine, jet70_scale_all);

  TH1D* h_MB_zvertex30_forcombine = (TH1D*)f_MB->Get(histname_zvertex30.c_str());
  TH1D* h_Jet5GeV_zvertex30_forcombine = (TH1D*)f_Jet5GeV->Get(histname_zvertex30.c_str());
  TH1D* h_Jet10GeV_zvertex30_forcombine = (TH1D*)f_Jet10GeV->Get(histname_zvertex30.c_str());
  TH1D* h_Jet15GeV_zvertex30_forcombine = (TH1D*)f_Jet15GeV->Get(histname_zvertex30.c_str());
  TH1D* h_Jet20GeV_zvertex30_forcombine = (TH1D*)f_Jet20GeV->Get(histname_zvertex30.c_str());
  TH1D* h_Jet30GeV_zvertex30_forcombine = (TH1D*)f_Jet30GeV->Get(histname_zvertex30.c_str());
  TH1D* h_Jet50GeV_zvertex30_forcombine = (TH1D*)f_Jet50GeV->Get(histname_zvertex30.c_str());
  TH1D* h_Jet70GeV_zvertex30_forcombine = (TH1D*)f_Jet70GeV->Get(histname_zvertex30.c_str());
  TH1D* h_zvertex30_combined = (TH1D*)h_MB_zvertex30_forcombine->Clone(histname_zvertex30.c_str());
  h_zvertex30_combined->Scale(mb_scale_all);
  h_zvertex30_combined->Add(h_Jet5GeV_zvertex30_forcombine, jet5_scale_all);
  h_zvertex30_combined->Add(h_Jet10GeV_zvertex30_forcombine, jet10_scale_all);
  h_zvertex30_combined->Add(h_Jet15GeV_zvertex30_forcombine, jet15_scale_all);
  h_zvertex30_combined->Add(h_Jet20GeV_zvertex30_forcombine, jet20_scale_all);
  h_zvertex30_combined->Add(h_Jet30GeV_zvertex30_forcombine, jet30_scale_all);
  h_zvertex30_combined->Add(h_Jet50GeV_zvertex30_forcombine, jet50_scale_all);
  h_zvertex30_combined->Add(h_Jet70GeV_zvertex30_forcombine, jet70_scale_all);

  TH1D* h_MB_zvertex60_forcombine = (TH1D*)f_MB->Get(histname_zvertex60.c_str());
  TH1D* h_Jet5GeV_zvertex60_forcombine = (TH1D*)f_Jet5GeV->Get(histname_zvertex60.c_str());
  TH1D* h_Jet10GeV_zvertex60_forcombine = (TH1D*)f_Jet10GeV->Get(histname_zvertex60.c_str());
  TH1D* h_Jet15GeV_zvertex60_forcombine = (TH1D*)f_Jet15GeV->Get(histname_zvertex60.c_str());
  TH1D* h_Jet20GeV_zvertex60_forcombine = (TH1D*)f_Jet20GeV->Get(histname_zvertex60.c_str());
  TH1D* h_Jet30GeV_zvertex60_forcombine = (TH1D*)f_Jet30GeV->Get(histname_zvertex60.c_str());
  TH1D* h_Jet50GeV_zvertex60_forcombine = (TH1D*)f_Jet50GeV->Get(histname_zvertex60.c_str());
  TH1D* h_Jet70GeV_zvertex60_forcombine = (TH1D*)f_Jet70GeV->Get(histname_zvertex60.c_str());
  TH1D* h_zvertex60_combined = (TH1D*)h_MB_zvertex60_forcombine->Clone(histname_zvertex60.c_str());
  h_zvertex60_combined->Scale(mb_scale_all);
  h_zvertex60_combined->Add(h_Jet5GeV_zvertex60_forcombine, jet5_scale_all);
  h_zvertex60_combined->Add(h_Jet10GeV_zvertex60_forcombine, jet10_scale_all);
  h_zvertex60_combined->Add(h_Jet15GeV_zvertex60_forcombine, jet15_scale_all);
  h_zvertex60_combined->Add(h_Jet20GeV_zvertex60_forcombine, jet20_scale_all);
  h_zvertex60_combined->Add(h_Jet30GeV_zvertex60_forcombine, jet30_scale_all);
  h_zvertex60_combined->Add(h_Jet50GeV_zvertex60_forcombine, jet50_scale_all);
  h_zvertex60_combined->Add(h_Jet70GeV_zvertex60_forcombine, jet70_scale_all);

  f_out->cd();
  h_all_combined->Write();
  h_zvertex30_combined->Write();
  h_zvertex60_combined->Write();
}

void combine_2Dhist(string histname, string surfix, TFile* f_MB, TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out) {
  TH1D *h_MB_event_all = (TH1D*)f_MB->Get("h_event_all"); int mb_event_all = h_MB_event_all->GetBinContent(1); double mb_scale_all = mb_cross_section / (double)mb_event_all;
  TH1D *h_Jet5GeV_event_all = (TH1D*)f_Jet5GeV->Get("h_event_all"); int jet5_event_all = h_Jet5GeV_event_all->GetBinContent(1); double jet5_scale_all = jet5_cross_section / (double)jet5_event_all;
  TH1D *h_Jet10GeV_event_all = (TH1D*)f_Jet10GeV->Get("h_event_all"); int jet10_event_all = h_Jet10GeV_event_all->GetBinContent(1); double jet10_scale_all = jet10_cross_section / (double)jet10_event_all;
  TH1D *h_Jet15GeV_event_all = (TH1D*)f_Jet15GeV->Get("h_event_all"); int jet15_event_all = h_Jet15GeV_event_all->GetBinContent(1); double jet15_scale_all = jet15_cross_section / (double)jet15_event_all;
  TH1D *h_Jet20GeV_event_all = (TH1D*)f_Jet20GeV->Get("h_event_all"); int jet20_event_all = h_Jet20GeV_event_all->GetBinContent(1); double jet20_scale_all = jet20_cross_section / (double)jet20_event_all;
  TH1D *h_Jet30GeV_event_all = (TH1D*)f_Jet30GeV->Get("h_event_all"); int jet30_event_all = h_Jet30GeV_event_all->GetBinContent(1); double jet30_scale_all = jet30_cross_section / (double)jet30_event_all;
  TH1D *h_Jet50GeV_event_all = (TH1D*)f_Jet50GeV->Get("h_event_all"); int jet50_event_all = h_Jet50GeV_event_all->GetBinContent(1); double jet50_scale_all = jet50_cross_section / (double)jet50_event_all;
  TH1D *h_Jet70GeV_event_all = (TH1D*)f_Jet70GeV->Get("h_event_all"); int jet70_event_all = h_Jet70GeV_event_all->GetBinContent(1); double jet70_scale_all = jet70_cross_section / (double)jet70_event_all;

  string histname_all = histname + "_all" + surfix;
  string histname_zvertex30 = histname + "_zvertex30" + surfix;
  string histname_zvertex60 = histname + "_zvertex60" + surfix;

  string histname_nentry_all = histname + "_nentry_all" + surfix;
  string histname_nentry_zvertex30 = histname + "_nentry_zvertex30" + surfix;
  string histname_nentry_zvertex60 = histname + "_nentry_zvertex60" + surfix;

  TH2D* h_MB_all_forcombine = (TH2D*)f_MB->Get(histname_all.c_str());
  TH2D* h_Jet5GeV_all_forcombine = (TH2D*)f_Jet5GeV->Get(histname_all.c_str());
  TH2D* h_Jet10GeV_all_forcombine = (TH2D*)f_Jet10GeV->Get(histname_all.c_str());
  TH2D* h_Jet15GeV_all_forcombine = (TH2D*)f_Jet15GeV->Get(histname_all.c_str());
  TH2D* h_Jet20GeV_all_forcombine = (TH2D*)f_Jet20GeV->Get(histname_all.c_str());
  TH2D* h_Jet30GeV_all_forcombine = (TH2D*)f_Jet30GeV->Get(histname_all.c_str());
  TH2D* h_Jet50GeV_all_forcombine = (TH2D*)f_Jet50GeV->Get(histname_all.c_str());
  TH2D* h_Jet70GeV_all_forcombine = (TH2D*)f_Jet70GeV->Get(histname_all.c_str());
  TH2D* h_all_combined = (TH2D*)h_MB_all_forcombine->Clone(histname_all.c_str());
  TH2D* h_nentry_all_combined = (TH2D*)h_MB_all_forcombine->Clone(histname_all.c_str()); h_nentry_all_combined->SetName(histname_nentry_all.c_str());
  h_all_combined->Scale(mb_scale_all);
  h_all_combined->Add(h_Jet5GeV_all_forcombine, jet5_scale_all);
  h_all_combined->Add(h_Jet10GeV_all_forcombine, jet10_scale_all);
  h_all_combined->Add(h_Jet15GeV_all_forcombine, jet15_scale_all);
  h_all_combined->Add(h_Jet20GeV_all_forcombine, jet20_scale_all);
  h_all_combined->Add(h_Jet30GeV_all_forcombine, jet30_scale_all);
  h_all_combined->Add(h_Jet50GeV_all_forcombine, jet50_scale_all);
  h_all_combined->Add(h_Jet70GeV_all_forcombine, jet70_scale_all);
  h_nentry_all_combined->Add(h_Jet5GeV_all_forcombine);
  h_nentry_all_combined->Add(h_Jet10GeV_all_forcombine);
  h_nentry_all_combined->Add(h_Jet15GeV_all_forcombine);
  h_nentry_all_combined->Add(h_Jet20GeV_all_forcombine);
  h_nentry_all_combined->Add(h_Jet30GeV_all_forcombine);
  h_nentry_all_combined->Add(h_Jet50GeV_all_forcombine);
  h_nentry_all_combined->Add(h_Jet70GeV_all_forcombine);

  TH2D* h_MB_zvertex30_forcombine = (TH2D*)f_MB->Get(histname_zvertex30.c_str());
  TH2D* h_Jet5GeV_zvertex30_forcombine = (TH2D*)f_Jet5GeV->Get(histname_zvertex30.c_str());
  TH2D* h_Jet10GeV_zvertex30_forcombine = (TH2D*)f_Jet10GeV->Get(histname_zvertex30.c_str());
  TH2D* h_Jet15GeV_zvertex30_forcombine = (TH2D*)f_Jet15GeV->Get(histname_zvertex30.c_str());
  TH2D* h_Jet20GeV_zvertex30_forcombine = (TH2D*)f_Jet20GeV->Get(histname_zvertex30.c_str());
  TH2D* h_Jet30GeV_zvertex30_forcombine = (TH2D*)f_Jet30GeV->Get(histname_zvertex30.c_str());
  TH2D* h_Jet50GeV_zvertex30_forcombine = (TH2D*)f_Jet50GeV->Get(histname_zvertex30.c_str());
  TH2D* h_Jet70GeV_zvertex30_forcombine = (TH2D*)f_Jet70GeV->Get(histname_zvertex30.c_str());
  TH2D* h_zvertex30_combined = (TH2D*)h_MB_zvertex30_forcombine->Clone(histname_zvertex30.c_str());
  TH2D* h_nentry_zvertex30_combined = (TH2D*)h_MB_zvertex30_forcombine->Clone(histname_zvertex30.c_str()); h_nentry_zvertex30_combined->SetName(histname_nentry_zvertex30.c_str());
  h_zvertex30_combined->Scale(mb_scale_all);
  h_zvertex30_combined->Add(h_Jet5GeV_zvertex30_forcombine, jet5_scale_all);
  h_zvertex30_combined->Add(h_Jet10GeV_zvertex30_forcombine, jet10_scale_all);
  h_zvertex30_combined->Add(h_Jet15GeV_zvertex30_forcombine, jet15_scale_all);
  h_zvertex30_combined->Add(h_Jet20GeV_zvertex30_forcombine, jet20_scale_all);
  h_zvertex30_combined->Add(h_Jet30GeV_zvertex30_forcombine, jet30_scale_all);
  h_zvertex30_combined->Add(h_Jet50GeV_zvertex30_forcombine, jet50_scale_all);
  h_zvertex30_combined->Add(h_Jet70GeV_zvertex30_forcombine, jet70_scale_all);
  h_nentry_zvertex30_combined->Add(h_Jet5GeV_zvertex30_forcombine);
  h_nentry_zvertex30_combined->Add(h_Jet10GeV_zvertex30_forcombine);
  h_nentry_zvertex30_combined->Add(h_Jet15GeV_zvertex30_forcombine);
  h_nentry_zvertex30_combined->Add(h_Jet20GeV_zvertex30_forcombine);
  h_nentry_zvertex30_combined->Add(h_Jet30GeV_zvertex30_forcombine);
  h_nentry_zvertex30_combined->Add(h_Jet50GeV_zvertex30_forcombine);
  h_nentry_zvertex30_combined->Add(h_Jet70GeV_zvertex30_forcombine);

  TH2D* h_MB_zvertex60_forcombine = (TH2D*)f_MB->Get(histname_zvertex60.c_str());
  TH2D* h_Jet5GeV_zvertex60_forcombine = (TH2D*)f_Jet5GeV->Get(histname_zvertex60.c_str());
  TH2D* h_Jet10GeV_zvertex60_forcombine = (TH2D*)f_Jet10GeV->Get(histname_zvertex60.c_str());
  TH2D* h_Jet15GeV_zvertex60_forcombine = (TH2D*)f_Jet15GeV->Get(histname_zvertex60.c_str());
  TH2D* h_Jet20GeV_zvertex60_forcombine = (TH2D*)f_Jet20GeV->Get(histname_zvertex60.c_str());
  TH2D* h_Jet30GeV_zvertex60_forcombine = (TH2D*)f_Jet30GeV->Get(histname_zvertex60.c_str());
  TH2D* h_Jet50GeV_zvertex60_forcombine = (TH2D*)f_Jet50GeV->Get(histname_zvertex60.c_str());
  TH2D* h_Jet70GeV_zvertex60_forcombine = (TH2D*)f_Jet70GeV->Get(histname_zvertex60.c_str());
  TH2D* h_zvertex60_combined = (TH2D*)h_MB_zvertex60_forcombine->Clone(histname_zvertex60.c_str());
  TH2D* h_nentry_zvertex60_combined = (TH2D*)h_MB_zvertex60_forcombine->Clone(histname_zvertex60.c_str()); h_nentry_zvertex60_combined->SetName(histname_nentry_zvertex60.c_str());
  h_zvertex60_combined->Scale(mb_scale_all);
  h_zvertex60_combined->Add(h_Jet5GeV_zvertex60_forcombine, jet5_scale_all);
  h_zvertex60_combined->Add(h_Jet10GeV_zvertex60_forcombine, jet10_scale_all);
  h_zvertex60_combined->Add(h_Jet15GeV_zvertex60_forcombine, jet15_scale_all);
  h_zvertex60_combined->Add(h_Jet20GeV_zvertex60_forcombine, jet20_scale_all);
  h_zvertex60_combined->Add(h_Jet30GeV_zvertex60_forcombine, jet30_scale_all);
  h_zvertex60_combined->Add(h_Jet50GeV_zvertex60_forcombine, jet50_scale_all);
  h_zvertex60_combined->Add(h_Jet70GeV_zvertex60_forcombine, jet70_scale_all);
  h_nentry_zvertex60_combined->Add(h_Jet5GeV_zvertex60_forcombine);
  h_nentry_zvertex60_combined->Add(h_Jet10GeV_zvertex60_forcombine);
  h_nentry_zvertex60_combined->Add(h_Jet15GeV_zvertex60_forcombine);
  h_nentry_zvertex60_combined->Add(h_Jet20GeV_zvertex60_forcombine);
  h_nentry_zvertex60_combined->Add(h_Jet30GeV_zvertex60_forcombine);
  h_nentry_zvertex60_combined->Add(h_Jet50GeV_zvertex60_forcombine);
  h_nentry_zvertex60_combined->Add(h_Jet70GeV_zvertex60_forcombine);

  f_out->cd();
  h_all_combined->Write();
  h_zvertex30_combined->Write();
  h_zvertex60_combined->Write();
  h_nentry_all_combined->Write();
  h_nentry_zvertex30_combined->Write();
  h_nentry_zvertex60_combined->Write();
}

void combine_1Dhist_noall(string histname, string surfix, std::string datatype, TFile* f_MB,  TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out) {
  TH1D *h_MB_event_all = (TH1D*)f_MB->Get("h_event_all"); int mb_event_all = h_MB_event_all->GetBinContent(1); double mb_scale_all = mb_cross_section / (double)mb_event_all;
  TH1D *h_Jet5GeV_event_all = (TH1D*)f_Jet5GeV->Get("h_event_all"); int jet5_event_all = h_Jet5GeV_event_all->GetBinContent(1); double jet5_scale_all = jet5_cross_section / (double)jet5_event_all;
  TH1D *h_Jet10GeV_event_all = (TH1D*)f_Jet10GeV->Get("h_event_all"); int jet10_event_all = h_Jet10GeV_event_all->GetBinContent(1); double jet10_scale_all = jet10_cross_section / (double)jet10_event_all;
  TH1D *h_Jet15GeV_event_all = (TH1D*)f_Jet15GeV->Get("h_event_all"); int jet15_event_all = h_Jet15GeV_event_all->GetBinContent(1); double jet15_scale_all = jet15_cross_section / (double)jet15_event_all;
  TH1D *h_Jet20GeV_event_all = (TH1D*)f_Jet20GeV->Get("h_event_all"); int jet20_event_all = h_Jet20GeV_event_all->GetBinContent(1); double jet20_scale_all = jet20_cross_section / (double)jet20_event_all;
  TH1D *h_Jet30GeV_event_all = (TH1D*)f_Jet30GeV->Get("h_event_all"); int jet30_event_all = h_Jet30GeV_event_all->GetBinContent(1); double jet30_scale_all = jet30_cross_section / (double)jet30_event_all;
  TH1D *h_Jet50GeV_event_all = (TH1D*)f_Jet50GeV->Get("h_event_all"); int jet50_event_all = h_Jet50GeV_event_all->GetBinContent(1); double jet50_scale_all = jet50_cross_section / (double)jet50_event_all;
  TH1D *h_Jet70GeV_event_all = (TH1D*)f_Jet70GeV->Get("h_event_all"); int jet70_event_all = h_Jet70GeV_event_all->GetBinContent(1); double jet70_scale_all = jet70_cross_section / (double)jet70_event_all;

  string histname_zvertex30 = histname + "_zvertex30" + surfix;
  string histname_zvertex60 = histname + "_zvertex60" + surfix;

  TH1D* h_MB_zvertex30_forcombine = (TH1D*)f_MB->Get(histname_zvertex30.c_str());
  TH1D* h_Jet5GeV_zvertex30_forcombine = (TH1D*)f_Jet5GeV->Get(histname_zvertex30.c_str());
  TH1D* h_Jet10GeV_zvertex30_forcombine = (TH1D*)f_Jet10GeV->Get(histname_zvertex30.c_str());
  TH1D* h_Jet15GeV_zvertex30_forcombine = (TH1D*)f_Jet15GeV->Get(histname_zvertex30.c_str());
  TH1D* h_Jet20GeV_zvertex30_forcombine = (TH1D*)f_Jet20GeV->Get(histname_zvertex30.c_str());
  TH1D* h_Jet30GeV_zvertex30_forcombine = (TH1D*)f_Jet30GeV->Get(histname_zvertex30.c_str());
  TH1D* h_Jet50GeV_zvertex30_forcombine = (TH1D*)f_Jet50GeV->Get(histname_zvertex30.c_str());
  TH1D* h_Jet70GeV_zvertex30_forcombine = (TH1D*)f_Jet70GeV->Get(histname_zvertex30.c_str());
  TH1D* h_zvertex30_combined = (TH1D*)h_MB_zvertex30_forcombine->Clone(histname_zvertex30.c_str());
  h_zvertex30_combined->Scale(mb_scale_all);
  h_zvertex30_combined->Add(h_Jet5GeV_zvertex30_forcombine, jet5_scale_all);
  h_zvertex30_combined->Add(h_Jet10GeV_zvertex30_forcombine, jet10_scale_all);
  h_zvertex30_combined->Add(h_Jet15GeV_zvertex30_forcombine, jet15_scale_all);
  h_zvertex30_combined->Add(h_Jet20GeV_zvertex30_forcombine, jet20_scale_all);
  h_zvertex30_combined->Add(h_Jet30GeV_zvertex30_forcombine, jet30_scale_all);
  h_zvertex30_combined->Add(h_Jet50GeV_zvertex30_forcombine, jet50_scale_all);
  h_zvertex30_combined->Add(h_Jet70GeV_zvertex30_forcombine, jet70_scale_all);

  TH1D* h_MB_zvertex60_forcombine = (TH1D*)f_MB->Get(histname_zvertex60.c_str());
  TH1D* h_Jet5GeV_zvertex60_forcombine = (TH1D*)f_Jet5GeV->Get(histname_zvertex60.c_str());
  TH1D* h_Jet10GeV_zvertex60_forcombine = (TH1D*)f_Jet10GeV->Get(histname_zvertex60.c_str());
  TH1D* h_Jet15GeV_zvertex60_forcombine = (TH1D*)f_Jet15GeV->Get(histname_zvertex60.c_str());
  TH1D* h_Jet20GeV_zvertex60_forcombine = (TH1D*)f_Jet20GeV->Get(histname_zvertex60.c_str());
  TH1D* h_Jet30GeV_zvertex60_forcombine = (TH1D*)f_Jet30GeV->Get(histname_zvertex60.c_str());
  TH1D* h_Jet50GeV_zvertex60_forcombine = (TH1D*)f_Jet50GeV->Get(histname_zvertex60.c_str());
  TH1D* h_Jet70GeV_zvertex60_forcombine = (TH1D*)f_Jet70GeV->Get(histname_zvertex60.c_str());
  TH1D* h_zvertex60_combined = (TH1D*)h_MB_zvertex60_forcombine->Clone(histname_zvertex60.c_str());
  h_zvertex60_combined->Scale(mb_scale_all);
  h_zvertex60_combined->Add(h_Jet5GeV_zvertex60_forcombine, jet5_scale_all);
  h_zvertex60_combined->Add(h_Jet10GeV_zvertex60_forcombine, jet10_scale_all);
  h_zvertex60_combined->Add(h_Jet15GeV_zvertex60_forcombine, jet15_scale_all);
  h_zvertex60_combined->Add(h_Jet20GeV_zvertex60_forcombine, jet20_scale_all);
  h_zvertex60_combined->Add(h_Jet30GeV_zvertex60_forcombine, jet30_scale_all);
  h_zvertex60_combined->Add(h_Jet50GeV_zvertex60_forcombine, jet50_scale_all);
  h_zvertex60_combined->Add(h_Jet70GeV_zvertex60_forcombine, jet70_scale_all);

  f_out->cd();
  h_zvertex30_combined->Write();
  h_zvertex60_combined->Write();
}

void combine_2Dhist_noall(string histname, string surfix, TFile* f_MB, TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out) {
  TH1D *h_MB_event_all = (TH1D*)f_MB->Get("h_event_all"); int mb_event_all = h_MB_event_all->GetBinContent(1); double mb_scale_all = mb_cross_section / (double)mb_event_all;
  TH1D *h_Jet5GeV_event_all = (TH1D*)f_Jet5GeV->Get("h_event_all"); int jet5_event_all = h_Jet5GeV_event_all->GetBinContent(1); double jet5_scale_all = jet5_cross_section / (double)jet5_event_all;
  TH1D *h_Jet10GeV_event_all = (TH1D*)f_Jet10GeV->Get("h_event_all"); int jet10_event_all = h_Jet10GeV_event_all->GetBinContent(1); double jet10_scale_all = jet10_cross_section / (double)jet10_event_all;
  TH1D *h_Jet15GeV_event_all = (TH1D*)f_Jet15GeV->Get("h_event_all"); int jet15_event_all = h_Jet15GeV_event_all->GetBinContent(1); double jet15_scale_all = jet15_cross_section / (double)jet15_event_all;
  TH1D *h_Jet20GeV_event_all = (TH1D*)f_Jet20GeV->Get("h_event_all"); int jet20_event_all = h_Jet20GeV_event_all->GetBinContent(1); double jet20_scale_all = jet20_cross_section / (double)jet20_event_all;
  TH1D *h_Jet30GeV_event_all = (TH1D*)f_Jet30GeV->Get("h_event_all"); int jet30_event_all = h_Jet30GeV_event_all->GetBinContent(1); double jet30_scale_all = jet30_cross_section / (double)jet30_event_all;
  TH1D *h_Jet50GeV_event_all = (TH1D*)f_Jet50GeV->Get("h_event_all"); int jet50_event_all = h_Jet50GeV_event_all->GetBinContent(1); double jet50_scale_all = jet50_cross_section / (double)jet50_event_all;
  TH1D *h_Jet70GeV_event_all = (TH1D*)f_Jet70GeV->Get("h_event_all"); int jet70_event_all = h_Jet70GeV_event_all->GetBinContent(1); double jet70_scale_all = jet70_cross_section / (double)jet70_event_all;

  string histname_zvertex30 = histname + "_zvertex30" + surfix;
  string histname_zvertex60 = histname + "_zvertex60" + surfix;

  string histname_nentry_zvertex30 = histname + "_nentry_zvertex30" + surfix;
  string histname_nentry_zvertex60 = histname + "_nentry_zvertex60" + surfix;

  TH2D* h_MB_zvertex30_forcombine = (TH2D*)f_MB->Get(histname_zvertex30.c_str());
  TH2D* h_Jet5GeV_zvertex30_forcombine = (TH2D*)f_Jet5GeV->Get(histname_zvertex30.c_str());
  TH2D* h_Jet10GeV_zvertex30_forcombine = (TH2D*)f_Jet10GeV->Get(histname_zvertex30.c_str());
  TH2D* h_Jet15GeV_zvertex30_forcombine = (TH2D*)f_Jet15GeV->Get(histname_zvertex30.c_str());
  TH2D* h_Jet20GeV_zvertex30_forcombine = (TH2D*)f_Jet20GeV->Get(histname_zvertex30.c_str());
  TH2D* h_Jet30GeV_zvertex30_forcombine = (TH2D*)f_Jet30GeV->Get(histname_zvertex30.c_str());
  TH2D* h_Jet50GeV_zvertex30_forcombine = (TH2D*)f_Jet50GeV->Get(histname_zvertex30.c_str());
  TH2D* h_Jet70GeV_zvertex30_forcombine = (TH2D*)f_Jet70GeV->Get(histname_zvertex30.c_str());
  TH2D* h_zvertex30_combined = (TH2D*)h_MB_zvertex30_forcombine->Clone(histname_zvertex30.c_str());
  TH2D* h_nentry_zvertex30_combined = (TH2D*)h_MB_zvertex30_forcombine->Clone(histname_zvertex30.c_str()); h_nentry_zvertex30_combined->SetName(histname_nentry_zvertex30.c_str());
  h_zvertex30_combined->Scale(mb_scale_all);
  h_zvertex30_combined->Add(h_Jet5GeV_zvertex30_forcombine, jet5_scale_all);
  h_zvertex30_combined->Add(h_Jet10GeV_zvertex30_forcombine, jet10_scale_all);
  h_zvertex30_combined->Add(h_Jet15GeV_zvertex30_forcombine, jet15_scale_all);
  h_zvertex30_combined->Add(h_Jet20GeV_zvertex30_forcombine, jet20_scale_all);
  h_zvertex30_combined->Add(h_Jet30GeV_zvertex30_forcombine, jet30_scale_all);
  h_zvertex30_combined->Add(h_Jet50GeV_zvertex30_forcombine, jet50_scale_all);
  h_zvertex30_combined->Add(h_Jet70GeV_zvertex30_forcombine, jet70_scale_all);
  h_nentry_zvertex30_combined->Add(h_Jet5GeV_zvertex30_forcombine);
  h_nentry_zvertex30_combined->Add(h_Jet10GeV_zvertex30_forcombine);
  h_nentry_zvertex30_combined->Add(h_Jet15GeV_zvertex30_forcombine);
  h_nentry_zvertex30_combined->Add(h_Jet20GeV_zvertex30_forcombine);
  h_nentry_zvertex30_combined->Add(h_Jet30GeV_zvertex30_forcombine);
  h_nentry_zvertex30_combined->Add(h_Jet50GeV_zvertex30_forcombine);
  h_nentry_zvertex30_combined->Add(h_Jet70GeV_zvertex30_forcombine);

  TH2D* h_MB_zvertex60_forcombine = (TH2D*)f_MB->Get(histname_zvertex60.c_str());
  TH2D* h_Jet5GeV_zvertex60_forcombine = (TH2D*)f_Jet5GeV->Get(histname_zvertex60.c_str());
  TH2D* h_Jet10GeV_zvertex60_forcombine = (TH2D*)f_Jet10GeV->Get(histname_zvertex60.c_str());
  TH2D* h_Jet15GeV_zvertex60_forcombine = (TH2D*)f_Jet15GeV->Get(histname_zvertex60.c_str());
  TH2D* h_Jet20GeV_zvertex60_forcombine = (TH2D*)f_Jet20GeV->Get(histname_zvertex60.c_str());
  TH2D* h_Jet30GeV_zvertex60_forcombine = (TH2D*)f_Jet30GeV->Get(histname_zvertex60.c_str());
  TH2D* h_Jet50GeV_zvertex60_forcombine = (TH2D*)f_Jet50GeV->Get(histname_zvertex60.c_str());
  TH2D* h_Jet70GeV_zvertex60_forcombine = (TH2D*)f_Jet70GeV->Get(histname_zvertex60.c_str());
  TH2D* h_zvertex60_combined = (TH2D*)h_MB_zvertex60_forcombine->Clone(histname_zvertex60.c_str());
  TH2D* h_nentry_zvertex60_combined = (TH2D*)h_MB_zvertex60_forcombine->Clone(histname_zvertex60.c_str()); h_nentry_zvertex60_combined->SetName(histname_nentry_zvertex60.c_str());
  h_zvertex60_combined->Scale(mb_scale_all);
  h_zvertex60_combined->Add(h_Jet5GeV_zvertex60_forcombine, jet5_scale_all);
  h_zvertex60_combined->Add(h_Jet10GeV_zvertex60_forcombine, jet10_scale_all);
  h_zvertex60_combined->Add(h_Jet15GeV_zvertex60_forcombine, jet15_scale_all);
  h_zvertex60_combined->Add(h_Jet20GeV_zvertex60_forcombine, jet20_scale_all);
  h_zvertex60_combined->Add(h_Jet30GeV_zvertex60_forcombine, jet30_scale_all);
  h_zvertex60_combined->Add(h_Jet50GeV_zvertex60_forcombine, jet50_scale_all);
  h_zvertex60_combined->Add(h_Jet70GeV_zvertex60_forcombine, jet70_scale_all);
  h_nentry_zvertex60_combined->Add(h_Jet5GeV_zvertex60_forcombine);
  h_nentry_zvertex60_combined->Add(h_Jet10GeV_zvertex60_forcombine);
  h_nentry_zvertex60_combined->Add(h_Jet15GeV_zvertex60_forcombine);
  h_nentry_zvertex60_combined->Add(h_Jet20GeV_zvertex60_forcombine);
  h_nentry_zvertex60_combined->Add(h_Jet30GeV_zvertex60_forcombine);
  h_nentry_zvertex60_combined->Add(h_Jet50GeV_zvertex60_forcombine);
  h_nentry_zvertex60_combined->Add(h_Jet70GeV_zvertex60_forcombine);

  f_out->cd();
  h_zvertex30_combined->Write();
  h_zvertex60_combined->Write();
  h_nentry_zvertex30_combined->Write();
  h_nentry_zvertex60_combined->Write();
}


void combine_1Dhist_closure(string surfix, string tag, TFile* f_MB, TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out) {
  TH1D *h_MB_event_all = (TH1D*)f_MB->Get("h_event_all"); int mb_event_all = h_MB_event_all->GetBinContent(1); double mb_scale_all = mb_cross_section / (double)mb_event_all;
  TH1D *h_Jet5GeV_event_all = (TH1D*)f_Jet5GeV->Get("h_event_all"); int jet5_event_all = h_Jet5GeV_event_all->GetBinContent(1); double jet5_scale_all = jet5_cross_section / (double)jet5_event_all;
  TH1D *h_Jet10GeV_event_all = (TH1D*)f_Jet10GeV->Get("h_event_all"); int jet10_event_all = h_Jet10GeV_event_all->GetBinContent(1); double jet10_scale_all = jet10_cross_section / (double)jet10_event_all;
  TH1D *h_Jet15GeV_event_all = (TH1D*)f_Jet15GeV->Get("h_event_all"); int jet15_event_all = h_Jet15GeV_event_all->GetBinContent(1); double jet15_scale_all = jet15_cross_section / (double)jet15_event_all;
  TH1D *h_Jet20GeV_event_all = (TH1D*)f_Jet20GeV->Get("h_event_all"); int jet20_event_all = h_Jet20GeV_event_all->GetBinContent(1); double jet20_scale_all = jet20_cross_section / (double)jet20_event_all;
  TH1D *h_Jet30GeV_event_all = (TH1D*)f_Jet30GeV->Get("h_event_all"); int jet30_event_all = h_Jet30GeV_event_all->GetBinContent(1); double jet30_scale_all = jet30_cross_section / (double)jet30_event_all;
  TH1D *h_Jet50GeV_event_all = (TH1D*)f_Jet50GeV->Get("h_event_all"); int jet50_event_all = h_Jet50GeV_event_all->GetBinContent(1); double jet50_scale_all = jet50_cross_section / (double)jet50_event_all;
  TH1D *h_Jet70GeV_event_all = (TH1D*)f_Jet70GeV->Get("h_event_all"); int jet70_event_all = h_Jet70GeV_event_all->GetBinContent(1); double jet70_scale_all = jet70_cross_section / (double)jet70_event_all;

  string histname = surfix + tag + "_zvertex60";

  TH1D* h_MB_zvertex60_forcombine = (TH1D*)f_MB->Get(histname.c_str());
  TH1D* h_Jet5GeV_zvertex60_forcombine = (TH1D*)f_Jet5GeV->Get(histname.c_str());
  TH1D* h_Jet10GeV_zvertex60_forcombine = (TH1D*)f_Jet10GeV->Get(histname.c_str());
  TH1D* h_Jet15GeV_zvertex60_forcombine = (TH1D*)f_Jet15GeV->Get(histname.c_str());
  TH1D* h_Jet20GeV_zvertex60_forcombine = (TH1D*)f_Jet20GeV->Get(histname.c_str());
  TH1D* h_Jet30GeV_zvertex60_forcombine = (TH1D*)f_Jet30GeV->Get(histname.c_str());
  TH1D* h_Jet50GeV_zvertex60_forcombine = (TH1D*)f_Jet50GeV->Get(histname.c_str());
  TH1D* h_Jet70GeV_zvertex60_forcombine = (TH1D*)f_Jet70GeV->Get(histname.c_str());
  TH1D* h_zvertex60_combined = (TH1D*)h_MB_zvertex60_forcombine->Clone(histname.c_str());
  h_zvertex60_combined->Scale(mb_scale_all);
  h_zvertex60_combined->Add(h_Jet5GeV_zvertex60_forcombine, jet5_scale_all);
  h_zvertex60_combined->Add(h_Jet10GeV_zvertex60_forcombine, jet10_scale_all);
  h_zvertex60_combined->Add(h_Jet15GeV_zvertex60_forcombine, jet15_scale_all);
  h_zvertex60_combined->Add(h_Jet20GeV_zvertex60_forcombine, jet20_scale_all);
  h_zvertex60_combined->Add(h_Jet30GeV_zvertex60_forcombine, jet30_scale_all);
  h_zvertex60_combined->Add(h_Jet50GeV_zvertex60_forcombine, jet50_scale_all);
  h_zvertex60_combined->Add(h_Jet70GeV_zvertex60_forcombine, jet70_scale_all);

  f_out->cd();
  h_zvertex60_combined->Write();
}

void combine_2Dhist_closure(string surfix, string tag, TFile* f_MB,  TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out) {
  TH1D *h_MB_event_all = (TH1D*)f_MB->Get("h_event_all"); int mb_event_all = h_MB_event_all->GetBinContent(1); double mb_scale_all = mb_cross_section / (double)mb_event_all;
  TH1D *h_Jet5GeV_event_all = (TH1D*)f_Jet5GeV->Get("h_event_all"); int jet5_event_all = h_Jet5GeV_event_all->GetBinContent(1); double jet5_scale_all = jet5_cross_section / (double)jet5_event_all;
  TH1D *h_Jet10GeV_event_all = (TH1D*)f_Jet10GeV->Get("h_event_all"); int jet10_event_all = h_Jet10GeV_event_all->GetBinContent(1); double jet10_scale_all = jet10_cross_section / (double)jet10_event_all;
  TH1D *h_Jet15GeV_event_all = (TH1D*)f_Jet15GeV->Get("h_event_all"); int jet15_event_all = h_Jet15GeV_event_all->GetBinContent(1); double jet15_scale_all = jet15_cross_section / (double)jet15_event_all;
  TH1D *h_Jet20GeV_event_all = (TH1D*)f_Jet20GeV->Get("h_event_all"); int jet20_event_all = h_Jet20GeV_event_all->GetBinContent(1); double jet20_scale_all = jet20_cross_section / (double)jet20_event_all;
  TH1D *h_Jet30GeV_event_all = (TH1D*)f_Jet30GeV->Get("h_event_all"); int jet30_event_all = h_Jet30GeV_event_all->GetBinContent(1); double jet30_scale_all = jet30_cross_section / (double)jet30_event_all;
  TH1D *h_Jet50GeV_event_all = (TH1D*)f_Jet50GeV->Get("h_event_all"); int jet50_event_all = h_Jet50GeV_event_all->GetBinContent(1); double jet50_scale_all = jet50_cross_section / (double)jet50_event_all;
  TH1D *h_Jet70GeV_event_all = (TH1D*)f_Jet70GeV->Get("h_event_all"); int jet70_event_all = h_Jet70GeV_event_all->GetBinContent(1); double jet70_scale_all = jet70_cross_section / (double)jet70_event_all;

  string histname = surfix + tag + "_zvertex60";

  TH2D* h_MB_zvertex60_forcombine = (TH2D*)f_MB->Get(histname.c_str());
  TH2D* h_Jet5GeV_zvertex60_forcombine = (TH2D*)f_Jet5GeV->Get(histname.c_str());
  TH2D* h_Jet10GeV_zvertex60_forcombine = (TH2D*)f_Jet10GeV->Get(histname.c_str());
  TH2D* h_Jet15GeV_zvertex60_forcombine = (TH2D*)f_Jet15GeV->Get(histname.c_str());
  TH2D* h_Jet20GeV_zvertex60_forcombine = (TH2D*)f_Jet20GeV->Get(histname.c_str());
  TH2D* h_Jet30GeV_zvertex60_forcombine = (TH2D*)f_Jet30GeV->Get(histname.c_str());
  TH2D* h_Jet50GeV_zvertex60_forcombine = (TH2D*)f_Jet50GeV->Get(histname.c_str());
  TH2D* h_Jet70GeV_zvertex60_forcombine = (TH2D*)f_Jet70GeV->Get(histname.c_str());
  TH2D* h_zvertex60_combined = (TH2D*)h_MB_zvertex60_forcombine->Clone(histname.c_str());
  h_zvertex60_combined->Scale(mb_scale_all);
  h_zvertex60_combined->Add(h_Jet5GeV_zvertex60_forcombine, jet5_scale_all);
  h_zvertex60_combined->Add(h_Jet10GeV_zvertex60_forcombine, jet10_scale_all);
  h_zvertex60_combined->Add(h_Jet15GeV_zvertex60_forcombine, jet15_scale_all);
  h_zvertex60_combined->Add(h_Jet20GeV_zvertex60_forcombine, jet20_scale_all);
  h_zvertex60_combined->Add(h_Jet30GeV_zvertex60_forcombine, jet30_scale_all);
  h_zvertex60_combined->Add(h_Jet50GeV_zvertex60_forcombine, jet50_scale_all);
  h_zvertex60_combined->Add(h_Jet70GeV_zvertex60_forcombine, jet70_scale_all);

  f_out->cd();
  h_zvertex60_combined->Write();
}