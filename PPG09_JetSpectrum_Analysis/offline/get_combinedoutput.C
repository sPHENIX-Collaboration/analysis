#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

void combine_1Dhist(string histname, TFile* f_MB, TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out);
void combine_2Dhist(string histname, TFile* f_MB, TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out);

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

  combine_1Dhist("h_recojet_pt_record", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_recojet_pt_passcut_record", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_truthjet_pt_record", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_truthjet_pt_passcut_record", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  combine_1Dhist("h_truth_nominal", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure_nominal", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist("h_respmatrix_nominal", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_fake_nominal", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_miss_nominal", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  combine_1Dhist("h_truth_jesup", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure_jesup", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist("h_respmatrix_jesup", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_fake_jesup", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_miss_jesup", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  combine_1Dhist("h_truth_jesdown", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure_jesdown", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist("h_respmatrix_jesdown", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_fake_jesdown", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_miss_jesdown", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  combine_1Dhist("h_truth_jerup", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure_jerup", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist("h_respmatrix_jerup", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_fake_jerup", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_miss_jerup", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  combine_1Dhist("h_truth_jerdown", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_measure_jerdown", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist("h_respmatrix_jerdown", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_fake_jerdown", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_miss_jerdown", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  combine_1Dhist("h_fullclosure_measure", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_fullclosure_truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist("h_fullclosure_respmatrix", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_fullclosure_fake", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_fullclosure_miss", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  combine_1Dhist("h_halfclosure_measure", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_halfclosure_truth", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist("h_halfclosure_respmatrix", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_halfclosure_fake", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_halfclosure_miss", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  combine_1Dhist("h_halfclosure_measure_check", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_halfclosure_truth_check", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_2Dhist("h_halfclosure_respmatrix_check", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_halfclosure_fake_check", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);
  combine_1Dhist("h_halfclosure_miss_check", f_MB, f_Jet5GeV, f_Jet10GeV, f_Jet15GeV, f_Jet20GeV, f_Jet30GeV, f_Jet50GeV, f_Jet70GeV, f_combined);

  f_combined->Close();
  std::cout << "Combined output file created: " << Form("output_sim_r0%d.root", radius_index) << std::endl;
}

void combine_1Dhist(string histname, TFile* f_MB, TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out) {
  TH1D *h_MB_event = (TH1D*)f_MB->Get("h_event"); int mb_event = h_MB_event->GetBinContent(1); double mb_scale = mb_cross_section / (double)mb_event;
  TH1D *h_Jet5GeV_event = (TH1D*)f_Jet5GeV->Get("h_event"); int jet5_event = h_Jet5GeV_event->GetBinContent(1); double jet5_scale = jet5_cross_section / (double)jet5_event;
  TH1D *h_Jet10GeV_event = (TH1D*)f_Jet10GeV->Get("h_event"); int jet10_event = h_Jet10GeV_event->GetBinContent(1); double jet10_scale = jet10_cross_section / (double)jet10_event;
  TH1D *h_Jet15GeV_event = (TH1D*)f_Jet15GeV->Get("h_event"); int jet15_event = h_Jet15GeV_event->GetBinContent(1); double jet15_scale = jet15_cross_section / (double)jet15_event;
  TH1D *h_Jet20GeV_event = (TH1D*)f_Jet20GeV->Get("h_event"); int jet20_event = h_Jet20GeV_event->GetBinContent(1); double jet20_scale = jet20_cross_section / (double)jet20_event;
  TH1D *h_Jet30GeV_event = (TH1D*)f_Jet30GeV->Get("h_event"); int jet30_event = h_Jet30GeV_event->GetBinContent(1); double jet30_scale = jet30_cross_section / (double)jet30_event;
  TH1D *h_Jet50GeV_event = (TH1D*)f_Jet50GeV->Get("h_event"); int jet50_event = h_Jet50GeV_event->GetBinContent(1); double jet50_scale = jet50_cross_section / (double)jet50_event;
  TH1D *h_Jet70GeV_event = (TH1D*)f_Jet70GeV->Get("h_event"); int jet70_event = h_Jet70GeV_event->GetBinContent(1); double jet70_scale = jet70_cross_section / (double)jet70_event;

  TH1D* h_MB_forcombine = (TH1D*)f_MB->Get(histname.c_str());
  TH1D* h_Jet5GeV_forcombine = (TH1D*)f_Jet5GeV->Get(histname.c_str());
  TH1D* h_Jet10GeV_forcombine = (TH1D*)f_Jet10GeV->Get(histname.c_str());
  TH1D* h_Jet15GeV_forcombine = (TH1D*)f_Jet15GeV->Get(histname.c_str());
  TH1D* h_Jet20GeV_forcombine = (TH1D*)f_Jet20GeV->Get(histname.c_str());
  TH1D* h_Jet30GeV_forcombine = (TH1D*)f_Jet30GeV->Get(histname.c_str());
  TH1D* h_Jet50GeV_forcombine = (TH1D*)f_Jet50GeV->Get(histname.c_str());
  TH1D* h_Jet70GeV_forcombine = (TH1D*)f_Jet70GeV->Get(histname.c_str());

  TH1D* h_combined = (TH1D*)h_MB_forcombine->Clone(histname.c_str());
  h_combined->Scale(mb_scale);
  h_combined->Add(h_Jet5GeV_forcombine, jet5_scale);
  h_combined->Add(h_Jet10GeV_forcombine, jet10_scale);
  h_combined->Add(h_Jet15GeV_forcombine, jet15_scale);
  h_combined->Add(h_Jet20GeV_forcombine, jet20_scale);
  h_combined->Add(h_Jet30GeV_forcombine, jet30_scale);
  h_combined->Add(h_Jet50GeV_forcombine, jet50_scale);
  h_combined->Add(h_Jet70GeV_forcombine, jet70_scale);

  f_out->cd();
  h_combined->Write();
}

void combine_2Dhist(string histname, TFile* f_MB, TFile* f_Jet5GeV, TFile* f_Jet10GeV, TFile* f_Jet15GeV, TFile* f_Jet20GeV, TFile* f_Jet30GeV, TFile* f_Jet50GeV, TFile* f_Jet70GeV, TFile* f_out) {
  TH1D *h_MB_event = (TH1D*)f_MB->Get("h_event"); int mb_event = h_MB_event->GetBinContent(1); double mb_scale = mb_cross_section / (double)mb_event;
  TH1D *h_Jet5GeV_event = (TH1D*)f_Jet5GeV->Get("h_event"); int jet5_event = h_Jet5GeV_event->GetBinContent(1); double jet5_scale = jet5_cross_section / (double)jet5_event;
  TH1D *h_Jet10GeV_event = (TH1D*)f_Jet10GeV->Get("h_event"); int jet10_event = h_Jet10GeV_event->GetBinContent(1); double jet10_scale = jet10_cross_section / (double)jet10_event;
  TH1D *h_Jet15GeV_event = (TH1D*)f_Jet15GeV->Get("h_event"); int jet15_event = h_Jet15GeV_event->GetBinContent(1); double jet15_scale = jet15_cross_section / (double)jet15_event;
  TH1D *h_Jet20GeV_event = (TH1D*)f_Jet20GeV->Get("h_event"); int jet20_event = h_Jet20GeV_event->GetBinContent(1); double jet20_scale = jet20_cross_section / (double)jet20_event;
  TH1D *h_Jet30GeV_event = (TH1D*)f_Jet30GeV->Get("h_event"); int jet30_event = h_Jet30GeV_event->GetBinContent(1); double jet30_scale = jet30_cross_section / (double)jet30_event;
  TH1D *h_Jet50GeV_event = (TH1D*)f_Jet50GeV->Get("h_event"); int jet50_event = h_Jet50GeV_event->GetBinContent(1); double jet50_scale = jet50_cross_section / (double)jet50_event;
  TH1D *h_Jet70GeV_event = (TH1D*)f_Jet70GeV->Get("h_event"); int jet70_event = h_Jet70GeV_event->GetBinContent(1); double jet70_scale = jet70_cross_section / (double)jet70_event;

  TH2D* h_MB_forcombine = (TH2D*)f_MB->Get(histname.c_str());
  TH2D* h_Jet5GeV_forcombine = (TH2D*)f_Jet5GeV->Get(histname.c_str());
  TH2D* h_Jet10GeV_forcombine = (TH2D*)f_Jet10GeV->Get(histname.c_str());
  TH2D* h_Jet15GeV_forcombine = (TH2D*)f_Jet15GeV->Get(histname.c_str());
  TH2D* h_Jet20GeV_forcombine = (TH2D*)f_Jet20GeV->Get(histname.c_str());
  TH2D* h_Jet30GeV_forcombine = (TH2D*)f_Jet30GeV->Get(histname.c_str());
  TH2D* h_Jet50GeV_forcombine = (TH2D*)f_Jet50GeV->Get(histname.c_str());
  TH2D* h_Jet70GeV_forcombine = (TH2D*)f_Jet70GeV->Get(histname.c_str());

  TH2D* h_combined = (TH2D*)h_MB_forcombine->Clone(histname.c_str());
  h_combined->Scale(mb_scale);
  h_combined->Add(h_Jet5GeV_forcombine, jet5_scale);
  h_combined->Add(h_Jet10GeV_forcombine, jet10_scale);
  h_combined->Add(h_Jet15GeV_forcombine, jet15_scale);
  h_combined->Add(h_Jet20GeV_forcombine, jet20_scale);
  h_combined->Add(h_Jet30GeV_forcombine, jet30_scale);
  h_combined->Add(h_Jet50GeV_forcombine, jet50_scale);
  h_combined->Add(h_Jet70GeV_forcombine, jet70_scale);

  f_out->cd();
  h_combined->Write();
}