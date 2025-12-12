// -- utils
#include "myUtils.C"

// ====================================================================
// sPHENIX Includes
// ====================================================================
#include <sPhenixStyle.C>

// ====================================================================
// ROOT Includes
// ====================================================================
#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TKey.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TMultiGraph.h>
#include <TPaletteAxis.h>
#include <TProfile.h>
#include <TROOT.h>

// ====================================================================
// Standard C++ Includes
// ====================================================================
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

// ====================================================================
// The Analysis Class
// ====================================================================
class DisplayJetAnav3
{
 public:
  // The constructor takes the configuration
  DisplayJetAnav3(std::string input_file, std::string output_dir, bool saveFig, bool saveIndividualFig)
    : m_input_file(std::move(input_file))
    , m_output_dir(std::move(output_dir))
    , m_saveFig(saveFig)
    , m_saveIndividualFig(saveIndividualFig)
  {
  }

  void run()
  {
    process();
  }

 private:
  // Configuration stored as members
  std::string m_input_file;
  std::string m_output_dir;
  std::map<std::string, std::unique_ptr<TH1>> m_hists;
  std::map<std::string, std::unique_ptr<TLegend>> m_leg;

  bool m_saveFig{false};
  bool m_saveIndividualFig{false};

  // --- Private Helper Methods ---
  void process();
  void draw(const std::string& run);
  void draw_calo(const std::string& run);
  void draw_UE(const std::string& run);

  // plots
  void plot_jet(TCanvas* c1, TCanvas* c2, const std::string& run);
  void plot_centrality(TCanvas* c1, TCanvas* c2, const std::string& run);

  void plot_psi(TCanvas* c1, TCanvas* c2, const std::string& run);
  void plot_psi(TCanvas* c1, TCanvas* c2, const std::string& run, const std::string& side, int id);

  void plot_sEPD_NS(TCanvas* c1, TCanvas* c2, const std::string& run);
  void plot_sEPD_NS_Norm(TCanvas* c1, TCanvas* c2, const std::string& run);

  void plot_calo(TCanvas* c1, TCanvas* c2, const std::string& run);
  void plot_calo(TCanvas* c1, TCanvas* c2, const std::string& run, int canvas_idx, TH1* hist, const std::string& tag, double x_min, double x_max, double y_min, double y_max, float x_label_size);

  void plot_UE(TCanvas* c1, TCanvas* c2, const std::string& run);
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================

void DisplayJetAnav3::plot_jet(TCanvas* c1, TCanvas* c2, const std::string& run)
{
  c1->cd(1);
  gPad->SetTopMargin(0.08F);
  gPad->SetLogy();

  auto* hJetPt = dynamic_cast<TH3*>(m_hists["h3JetPhiEtaPt"].get())->Project3D("z");
  auto* hJetPtv2 = dynamic_cast<TH3*>(m_hists["h3JetPhiEtaPtv2"].get())->Project3D("z");

  hJetPt->Draw();
  hJetPt->SetTitle(std::format("Run: {}", run).c_str());
  hJetPt->SetLineColor(kBlue);
  hJetPt->SetLineWidth(3);
  hJetPt->GetYaxis()->SetMaxDigits(3);
  hJetPt->GetYaxis()->SetTitle("Jet Yield / 1 GeV");
  hJetPt->GetXaxis()->SetLabelSize(0.06F);
  hJetPt->GetXaxis()->SetTitleSize(0.05F);
  hJetPt->GetXaxis()->SetTitleOffset(1.1F);
  hJetPt->GetYaxis()->SetTitleOffset(1.2F);

  double exponent = std::ceil(std::log10(hJetPt->GetMaximum()));
  double upper_bound = std::pow(10.0, exponent);

  hJetPt->GetYaxis()->SetRangeUser(5e-1, upper_bound);

  hJetPtv2->Draw("same");
  hJetPtv2->SetLineColor(kRed);
  hJetPtv2->SetLineWidth(3);

  double xshift = 0.25;
  double yshift = 0.05;

  m_leg["jet"] = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
  auto* leg = m_leg["jet"].get();
  leg->SetFillStyle(0);
  leg->SetTextSize(0.06F);
  leg->AddEntry(hJetPt, "Default", "l");
  leg->AddEntry(hJetPtv2, "Dead IB Mask", "l");
  leg->Draw("same");

  if (m_saveIndividualFig)
  {
    c2->cd();
    gPad->SetTopMargin(0.08F);

    gPad->SetLogy();
    hJetPt->Draw();
    hJetPtv2->Draw("same");
    leg->Draw("same");

    c2->Print(std::format("{}/images/Jet-pT-{}.png", m_output_dir, run).c_str());

    hJetPt->GetXaxis()->SetRangeUser(0, 45);

    c2->Print(std::format("{}/images/Jet-pT-{}-zoom.png", m_output_dir, run).c_str());

    hJetPt->GetXaxis()->SetRangeUser(0, hJetPt->GetXaxis()->GetXmax());
  }
}

void DisplayJetAnav3::plot_centrality(TCanvas* c1, TCanvas* c2, const std::string& run)
{
  c1->cd(2);
  gPad->SetTopMargin(0.08F);

  auto* hCentrality = m_hists["hCentrality"].get();

  hCentrality->Draw();
  hCentrality->SetTitle(std::format("Run: {}", run).c_str());
  hCentrality->SetLineColor(kBlue);
  hCentrality->SetLineWidth(3);
  hCentrality->GetYaxis()->SetRangeUser(0, hCentrality->GetMaximum() * 1.1);
  hCentrality->GetYaxis()->SetMaxDigits(3);
  hCentrality->GetXaxis()->SetLabelSize(0.06F);
  hCentrality->GetXaxis()->SetTitleSize(0.06F);
  hCentrality->GetYaxis()->SetLabelSize(0.06F);
  hCentrality->GetYaxis()->SetTitleSize(0.06F);
  hCentrality->GetXaxis()->SetTitleOffset(0.9F);

  if (m_saveIndividualFig)
  {
    c2->cd();
    gPad->SetTopMargin(0.08F);

    hCentrality->Draw();

    c2->Print(std::format("{}/images/Centrality-{}.png", m_output_dir, run).c_str());
  }
}

void DisplayJetAnav3::plot_psi(TCanvas* c1, TCanvas* c2, const std::string& run)
{
   plot_psi(c1, c2, run, "S", 3);
   plot_psi(c1, c2, run, "N", 4);
}

void DisplayJetAnav3::plot_psi(TCanvas* c1, TCanvas* c2, const std::string& run, const std::string& side, int id)
{
  c1->cd(id);
  gPad->SetTopMargin(0.08F);

  std::string proj = (side == "S") ? "x" : "y";

  auto* hPsi_raw = dynamic_cast<TH3*>(m_hists["h3_sEPD_Psi_2_raw"].get())->Project3D(proj.c_str());
  auto* hPsi_corr = dynamic_cast<TH3*>(m_hists["h3_sEPD_Psi_2_corr2"].get())->Project3D(proj.c_str());

  hPsi_raw->Draw();
  hPsi_raw->SetTitle(std::format("Run: {}", run).c_str());
  hPsi_raw->GetYaxis()->SetTitle("Events / (2#pi / 126)");
  hPsi_raw->SetLineColor(kBlue);
  hPsi_raw->SetLineWidth(3);
  hPsi_raw->GetYaxis()->SetRangeUser(0, hPsi_raw->GetMaximum() * 1.1);
  hPsi_raw->GetXaxis()->SetLabelSize(0.06F);
  hPsi_raw->GetXaxis()->SetTitleSize(0.06F);
  hPsi_raw->GetYaxis()->SetLabelSize(0.06F);
  hPsi_raw->GetYaxis()->SetTitleSize(0.06F);
  hPsi_raw->GetXaxis()->SetTitleOffset(0.95F);
  hPsi_raw->GetYaxis()->SetTitleOffset(1.2F);

  hPsi_corr->Draw("same");
  hPsi_corr->SetTitle("");
  hPsi_corr->SetLineColor(kRed);
  hPsi_corr->SetLineWidth(3);

  double xshift = 0.25;
  double yshift = 0.05;

  std::string leg_key = std::format("Psi_{}", side);
  m_leg[leg_key] = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
  auto* leg = m_leg[leg_key].get();
  leg->SetFillStyle(0);
  leg->SetTextSize(0.06F);
  leg->AddEntry(hPsi_raw, "Raw", "l");
  leg->AddEntry(hPsi_corr, "Corrected", "l");
  leg->Draw("same");

  if (m_saveIndividualFig)
  {
    c2->cd();
    gPad->SetTopMargin(0.08F);

    hPsi_raw->Draw();
    hPsi_corr->Draw("same");
    leg->Draw("same");

    c2->Print(std::format("{}/images/Psi-{}-{}.png", m_output_dir, side, run).c_str());
  }
}

void DisplayJetAnav3::plot_sEPD_NS(TCanvas* c1, TCanvas* c2, const std::string& run)
{
  c1->cd(5);
  gPad->SetTopMargin(0.08F);
  gPad->SetLeftMargin(0.17F);

  auto* hSP = m_hists["hSP_res_prof_2"].get();

  hSP->Draw("p e X0");
  hSP->Draw("same hist l p");
  hSP->SetTitle(std::format("Run: {}", run).c_str());
  hSP->SetLineColor(kBlue);
  hSP->SetLineColor(kBlue);
  hSP->SetLineWidth(3);
  hSP->GetYaxis()->SetRangeUser(0, hSP->GetMaximum() * 1.1);
  hSP->GetXaxis()->SetLabelSize(0.06F);
  hSP->GetXaxis()->SetTitleSize(0.06F);
  hSP->GetYaxis()->SetLabelSize(0.06F);
  hSP->GetYaxis()->SetTitleSize(0.06F);
  hSP->GetXaxis()->SetTitleOffset(0.9F);

  if (m_saveIndividualFig)
  {
    c2->cd();
    gPad->SetTopMargin(0.08F);
    gPad->SetLeftMargin(0.17F);

    hSP->Draw("p e X0");
    hSP->Draw("same hist l p");

    c2->Print(std::format("{}/images/hSP-NS-{}.png", m_output_dir, run).c_str());
  }
}

void DisplayJetAnav3::plot_sEPD_NS_Norm(TCanvas* c1, TCanvas* c2, const std::string& run)
{
  c1->cd(6);
  gPad->SetTopMargin(0.08F);
  gPad->SetLeftMargin(0.18F);

  auto* hSP = m_hists["hSP_evt_res_prof_2"].get();

  hSP->Draw("p e X0");
  hSP->Draw("same hist l p");
  hSP->SetTitle(std::format("Run: {}", run).c_str());
  hSP->SetLineColor(kBlue);
  hSP->SetLineColor(kBlue);
  hSP->SetLineWidth(3);
  hSP->GetYaxis()->SetRangeUser(0, hSP->GetMaximum() * 1.1);
  hSP->GetXaxis()->SetLabelSize(0.06F);
  hSP->GetXaxis()->SetTitleSize(0.06F);
  hSP->GetYaxis()->SetLabelSize(0.06F);
  hSP->GetYaxis()->SetTitleSize(0.06F);
  hSP->GetXaxis()->SetTitleOffset(0.9F);
  hSP->GetYaxis()->SetTitleOffset(1.5F);

  if (m_saveIndividualFig)
  {
    c2->cd();
    gPad->SetTopMargin(0.08F);
    gPad->SetLeftMargin(0.18F);

    hSP->Draw("p e X0");
    hSP->Draw("same hist l p");

    c2->Print(std::format("{}/images/hSP-NS-Norm-{}.png", m_output_dir, run).c_str());
  }
}

void DisplayJetAnav3::draw(const std::string& run)
{
  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>();
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(1700, 1000);

  std::unique_ptr<TCanvas> c2 = std::make_unique<TCanvas>();
  c2->SetTickx();
  c2->SetTicky();

  c2->SetCanvasSize(1200, 1000);

  gStyle->SetOptTitle(1);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleW(1);
  gStyle->SetTitleH(0.08F);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);

  c1->Divide(3, 2, 0.00025F, 0.00025F);

  std::string output = std::format("{}/pdf/plots-{}.pdf", m_output_dir, run);

  // Jet pT
  plot_jet(c1.get(), c2.get(), run);

  gPad->SetLogy(0);
  // Centrality
  plot_centrality(c1.get(), c2.get(), run);

  // Psi
  plot_psi(c1.get(), c2.get(), run);

  // sEPD Q NS
  plot_sEPD_NS(c1.get(), c2.get(), run);

  // sEPD Q NS Norm
  plot_sEPD_NS_Norm(c1.get(), c2.get(), run);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig)
  {
    c1->Print(std::format("{}/images/{}.png", m_output_dir, run).c_str());
  }
}

void DisplayJetAnav3::plot_calo(TCanvas* c1, TCanvas* c2, const std::string& run)
{
    auto* h2EMCal_MBD = dynamic_cast<TH3*>(m_hists["h3EMCal_MBD_sEPD"].get())->Project3D("yx");
    auto* h2IHCal_MBD = dynamic_cast<TH3*>(m_hists["h3IHCal_MBD_sEPD"].get())->Project3D("yx");
    auto* h2OHCal_MBD = dynamic_cast<TH3*>(m_hists["h3OHCal_MBD_sEPD"].get())->Project3D("yx");

    auto* h2EMCal_sEPD = dynamic_cast<TH3*>(m_hists["h3EMCal_MBD_sEPD"].get())->Project3D("zx");
    auto* h2IHCal_sEPD = dynamic_cast<TH3*>(m_hists["h3IHCal_MBD_sEPD"].get())->Project3D("zx");
    auto* h2OHCal_sEPD = dynamic_cast<TH3*>(m_hists["h3OHCal_MBD_sEPD"].get())->Project3D("zx");

    auto* h2EMCal_IHCal = dynamic_cast<TH3*>(m_hists["h3EMCal_IHCal_OHCal"].get())->Project3D("yx");
    auto* h2IHCal_OHCal = dynamic_cast<TH3*>(m_hists["h3EMCal_IHCal_OHCal"].get())->Project3D("zy");
    auto* h2MBD_sEPD = dynamic_cast<TH3*>(m_hists["h3EMCal_MBD_sEPD"].get())->Project3D("zy");

    auto* h2CaloE_Centrality = dynamic_cast<TH2*>(m_hists["h2CaloECentrality"].get());
    auto* h2SumE_Centrality = dynamic_cast<TH2*>(m_hists["h2SumECentrality"].get());

    float x_label_size = 0.03F;

    double MBD_max = 2.1e3;
    double sEPD_max = 2.5e4;
    double EMCal_min = -2e2;
    double EMCal_max = 2e3;
    double IHCal_min = -50;
    double IHCal_max = 2e2;
    double OHCal_min = -1e2;
    double OHCal_max = 5e2;

    double cent_min = -0.5;
    double cent_max = 79.5;

    double CaloE_min = 0;
    double CaloE_max = 2.5e3;

    int canvas_idx = 1;
    
    plot_calo(c1, c2, run, canvas_idx++, h2EMCal_MBD, "EMCal-MBD", EMCal_min, EMCal_max, 0, MBD_max, x_label_size);
    plot_calo(c1, c2, run, canvas_idx++, h2IHCal_MBD, "IHCal-MBD", IHCal_min, IHCal_max, 0, MBD_max, x_label_size);
    plot_calo(c1, c2, run, canvas_idx++, h2OHCal_MBD, "OHCal-MBD", OHCal_min, OHCal_max, 0, MBD_max, x_label_size);

    plot_calo(c1, c2, run, canvas_idx++, h2EMCal_sEPD, "EMCal-sEPD", EMCal_min, EMCal_max, 0, sEPD_max, x_label_size);
    plot_calo(c1, c2, run, canvas_idx++, h2IHCal_sEPD, "IHCal-sEPD", IHCal_min, IHCal_max, 0, sEPD_max, x_label_size);
    plot_calo(c1, c2, run, canvas_idx++, h2OHCal_sEPD, "OHCal-sEPD", OHCal_min, OHCal_max, 0, sEPD_max, x_label_size);

    plot_calo(c1, c2, run, canvas_idx++, h2EMCal_IHCal, "EMCal-IHCal", EMCal_min, EMCal_max, IHCal_min, IHCal_max, x_label_size);
    plot_calo(c1, c2, run, canvas_idx++, h2IHCal_OHCal, "IHCal-OHCal", IHCal_min, IHCal_max, OHCal_min, OHCal_max, x_label_size);
    plot_calo(c1, c2, run, canvas_idx++, h2MBD_sEPD, "MBD-sEPD", 0, MBD_max, 0,sEPD_max, x_label_size);

    plot_calo(c1, c2, run, canvas_idx++, h2CaloE_Centrality, "CaloE-Centrality", CaloE_min, CaloE_max, cent_min, cent_max, x_label_size);
    plot_calo(c1, c2, run, canvas_idx++, h2SumE_Centrality, "SumE-Centrality-v2", CaloE_min, CaloE_max, cent_min, cent_max, x_label_size);
}

void DisplayJetAnav3::plot_calo(TCanvas* c1, TCanvas* c2, const std::string& run, int canvas_idx, TH1* hist, const std::string& tag, double x_min, double x_max, double y_min, double y_max, float x_label_size)
{
  c1->cd(canvas_idx);
  gPad->SetTopMargin(0.08F);
  gPad->SetLeftMargin(0.18F);
  gPad->SetRightMargin(0.13F);
  gPad->SetLogz();

  hist->Draw("COLZ1");
  hist->SetTitle(std::format("Run: {}", run).c_str());
  hist->GetXaxis()->SetLabelSize(x_label_size);
  hist->GetXaxis()->SetTitleSize(0.06F);
  hist->GetYaxis()->SetLabelSize(0.06F);
  hist->GetYaxis()->SetTitleSize(0.06F);
  hist->GetXaxis()->SetTitleOffset(0.95F);
  hist->GetYaxis()->SetTitleOffset(1.4F);
  hist->GetYaxis()->SetMaxDigits(3);
  hist->GetXaxis()->SetRangeUser(x_min, x_max);
  hist->GetYaxis()->SetRangeUser(y_min, y_max);

  if (m_saveIndividualFig)
  {
    c2->cd();
    gPad->SetTopMargin(0.08F);
    gPad->SetLeftMargin(0.18F);
    gPad->SetRightMargin(0.13F);
    gPad->SetLogz();

    hist->Draw("COLZ1");

    c2->Print(std::format("{}/images/{}-{}.png", m_output_dir, tag, run).c_str());
  }
}

void DisplayJetAnav3::draw_calo(const std::string& run)
{
  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>();
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(2300, 1500);

  std::unique_ptr<TCanvas> c2 = std::make_unique<TCanvas>();
  c2->SetTickx();
  c2->SetTicky();

  c2->SetCanvasSize(1200, 1000);

  gStyle->SetOptTitle(1);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleW(1);
  gStyle->SetTitleH(0.08F);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);

  c1->Divide(4, 3, 0.00025F, 0.00025F);

  std::string output = std::format("{}/pdf-calo/plots-{}.pdf", m_output_dir, run);

  plot_calo(c1.get(), c2.get(), run);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig)
  {
    c1->Print(std::format("{}/images/calo-{}.png", m_output_dir, run).c_str());
  }
}

void DisplayJetAnav3::plot_UE(TCanvas* c1, TCanvas* c2, const std::string& run)
{
  auto* h2CaloV2_Centrality = dynamic_cast<TH3*>(m_hists["h3SumECaloV2Centrality"].get())->Project3D("yz");
  auto* h2SumE_Centrality = dynamic_cast<TH3*>(m_hists["h3SumECaloV2Centrality"].get())->Project3D("xz");
  auto* h2SumE_CaloV2 = dynamic_cast<TH3*>(m_hists["h3SumECaloV2Centrality"].get())->Project3D("yx");
  auto* h2SumE_CaloV2_zoom = dynamic_cast<TH2*>(h2SumE_CaloV2->Clone("h2SumE_CaloV2_zoom"));

  // ------------------------------------------
  // QA for cases where |v2| >= 0.5
  // ------------------------------------------
  auto* h3SumE_CaloV2_v21 = dynamic_cast<TH3*>(m_hists["h3SumECaloV2Centrality"].get());
  auto* h3SumE_CaloV2_v22 = dynamic_cast<TH3*>(m_hists["h3SumECaloV2Centrality"].get());

  int nBins = h3SumE_CaloV2_v21->GetNbinsY();
  int bin_low = h3SumE_CaloV2_v21->GetYaxis()->FindBin(-0.5);
  int bin_high = h3SumE_CaloV2_v22->GetYaxis()->FindBin(0.5);

  h3SumE_CaloV2_v21->GetYaxis()->SetRange(1, bin_low-1);
  h3SumE_CaloV2_v22->GetYaxis()->SetRange(bin_high, nBins);

  h3SumE_CaloV2_v21->Add(h3SumE_CaloV2_v22);

  auto* h2SumE_Centrality_v2 = h3SumE_CaloV2_v21->Project3D("xz");
  // ------------------------------------------

  auto* h2CaloE_SumE = m_hists["h3CaloESumETowMedE_yx"].get();
  auto* h2CaloE_SumE_zoom = dynamic_cast<TH2*>(h2CaloE_SumE->Clone("h2CaloE_SumE_zoom"));

  auto* h2CaloE_TowMedE = m_hists["h3CaloESumETowMedE_xz"].get();
  auto* h2CaloE_TowMedE_EMCal = dynamic_cast<TH2*>(m_hists["h2CaloETowMedE_EMCal"].get());

  auto* h2SumE_TowMedE = m_hists["h3CaloESumETowMedE_yz"].get();
  auto* h2SumE_TowMedE_zoom = dynamic_cast<TH2*>(h2SumE_TowMedE->Clone("h2SumE_TowMedE_zoom"));

  auto* h2SumE_TowMedE_EMCal = dynamic_cast<TH2*>(m_hists["h2SumETowMedE_EMCal"].get());
  auto* h2SumE_TowMedE_EMCal_zoom = dynamic_cast<TH2*>(h2SumE_TowMedE->Clone("h2SumE_TowMedE_EMCal_zoom"));

  auto* h2TowMedE_CaloV2 = m_hists["h2TowMedECaloV2"].get();
  auto* h2TowMedE_CaloV2_zoom = dynamic_cast<TH2*>(h2TowMedE_CaloV2->Clone("h2TowMedE_CaloV2_zoom"));

  auto* h2TowMedE_EMCal_CaloV2 = m_hists["h2TowMedE_EMCal_CaloV2"].get();
  auto* h2TowMedE_EMCal_CaloV2_zoom = dynamic_cast<TH2*>(h2TowMedE_EMCal_CaloV2->Clone("h2TowMedE_EMCal_CaloV2_zoom"));

  auto* h2SumE_JetPt = dynamic_cast<TH3*>(m_hists["h3JetPtEnergySumE"].get())->Project3D("xz");
  auto* h2SumE_JetPt_zoom = dynamic_cast<TH2*>(h2SumE_JetPt->Clone("h2SumE_JetPt_zoom"));

  auto* h2SumE_JetEnergy = dynamic_cast<TH3*>(m_hists["h3JetPtEnergySumE"].get())->Project3D("yz");
  auto* h2SumE_JetEnergy_zoom = dynamic_cast<TH2*>(h2SumE_JetEnergy->Clone("h2SumE_JetEnergy_zoom"));

  auto* h2CaloE_JetPt = dynamic_cast<TH3*>(m_hists["h3JetPtEnergyCaloE"].get())->Project3D("xz");
  auto* h2CaloE_JetPt_zoom = dynamic_cast<TH2*>(h2CaloE_JetPt->Clone("h2CaloE_JetPt_zoom"));

  auto* h2CaloE_JetEnergy = dynamic_cast<TH3*>(m_hists["h3JetPtEnergyCaloE"].get())->Project3D("yz");
  auto* h2CaloE_JetEnergy_zoom = dynamic_cast<TH2*>(h2CaloE_JetEnergy->Clone("h2CaloE_JetEnergy_zoom"));

  auto* h2Seeds_Centrality = dynamic_cast<TH2*>(m_hists["h2SeedsCentrality"].get());
  auto* h2Seeds_CaloE = dynamic_cast<TH2*>(m_hists["h3SeedsCaloESumE_yx"].get());
  auto* h2Seeds_SumE = dynamic_cast<TH2*>(m_hists["h3SeedsCaloESumE_zx"].get());

  float x_label_size = 0.06F;

  double cent_min = -0.5;
  double cent_max = 79.5;

  double v2_min = -1;
  double v2_max = 1;

  double TowMedE_min = 0;
  double TowMedE_max = 200;

  double TowMedE_EMCal_min = -10;
  double TowMedE_EMCal_max = 1e3;

  double SumE_min = -2e2;
  double SumE_max = 2.5e3;

  double SumE_v2_min = 0;
  double SumE_v2_max = 100;

  double CaloE_min = -50;
  double CaloE_max = 2.5e3;

  double jetPt_min = 0;
  double jetPt_max = 5e2;

  double jetEnergy_min = -5e2;
  double jetEnergy_max = 5e2;

  double seeds_min = 0;
  double seeds_max = 100;

  int canvas_idx = 1;

  plot_calo(c1, c2, run, canvas_idx++, h2CaloV2_Centrality, "CaloV2-Centrality", cent_min, cent_max, v2_min, v2_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2SumE_Centrality, "SumE-Centrality", cent_min, cent_max, SumE_min, SumE_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2SumE_Centrality_v2, "SumE-Centrality-high-v2", cent_min, cent_max, SumE_v2_min, SumE_v2_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2SumE_CaloV2, "SumE-CaloV2", SumE_min, SumE_max, v2_min, v2_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2SumE_CaloV2_zoom, "SumE-CaloV2-zoom", -50, 100, v2_min, v2_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2CaloE_SumE, "CaloE-SumE", SumE_min, SumE_max, SumE_min, SumE_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2CaloE_SumE_zoom, "CaloE-SumE-zoom", -10, 50, SumE_min, SumE_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2CaloE_TowMedE, "CaloE-TowMedE", TowMedE_min, TowMedE_max, CaloE_min, CaloE_max, 0.04F);
  plot_calo(c1, c2, run, canvas_idx++, h2CaloE_TowMedE_EMCal, "CaloE-TowMedE-EMCal", TowMedE_EMCal_min, TowMedE_EMCal_max, CaloE_min, CaloE_max, 0.04F);
  plot_calo(c1, c2, run, canvas_idx++, h2SumE_TowMedE, "SumE-TowMedE", TowMedE_min, TowMedE_max, SumE_min, SumE_max, 0.04F);
  plot_calo(c1, c2, run, canvas_idx++, h2SumE_TowMedE_zoom, "SumE-TowMedE-zoom", TowMedE_min, 5, SumE_min, SumE_max, 0.04F);
  plot_calo(c1, c2, run, canvas_idx++, h2SumE_TowMedE_EMCal, "SumE-TowMedE-EMCal", TowMedE_EMCal_min, TowMedE_EMCal_max, SumE_min, SumE_max, 0.04F);
  plot_calo(c1, c2, run, canvas_idx++, h2SumE_TowMedE_EMCal_zoom, "SumE-TowMedE-EMCal-zoom", TowMedE_EMCal_min, 20, SumE_min, SumE_max, 0.04F);
  plot_calo(c1, c2, run, canvas_idx++, h2TowMedE_CaloV2, "TowMedE-CaloV2", TowMedE_min, TowMedE_max, v2_min, v2_max, 0.04F);
  plot_calo(c1, c2, run, canvas_idx++, h2TowMedE_CaloV2_zoom, "TowMedE-CaloV2-zoom", TowMedE_min, 5, v2_min, v2_max, 0.04F);
  plot_calo(c1, c2, run, canvas_idx++, h2TowMedE_EMCal_CaloV2, "TowMedE-EMCal-CaloV2", TowMedE_EMCal_min, TowMedE_EMCal_max, v2_min, v2_max, 0.04F);
  plot_calo(c1, c2, run, canvas_idx++, h2TowMedE_EMCal_CaloV2_zoom, "TowMedE-EMCal-CaloV2-zoom", TowMedE_EMCal_min, 40, v2_min, v2_max, 0.04F);
  plot_calo(c1, c2, run, canvas_idx++, h2SumE_JetPt, "SumE-JetPt", SumE_min, SumE_max, jetPt_min, jetPt_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2SumE_JetPt_zoom, "SumE-JetPt-zoom", -80, 100, jetPt_min, jetPt_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2SumE_JetEnergy, "SumE-JetEnergy", SumE_min, SumE_max, jetEnergy_min, jetEnergy_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2SumE_JetEnergy_zoom, "SumE-JetEnergy-zoom", -40, 100, jetEnergy_min, jetEnergy_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2CaloE_JetPt, "CaloE-JetPt", CaloE_min, CaloE_max, jetPt_min, jetPt_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2CaloE_JetPt_zoom, "CaloE-JetPt-zoom", -50, 150, jetPt_min, jetPt_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2CaloE_JetEnergy, "CaloE-JetEnergy", CaloE_min, CaloE_max, jetEnergy_min, jetEnergy_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2CaloE_JetEnergy_zoom, "CaloE-JetEnergy-zoom", -50, 150, jetEnergy_min, jetEnergy_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2Seeds_Centrality, "Seeds-Centrality", seeds_min, seeds_max, cent_min, cent_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2Seeds_CaloE, "Seeds-CaloE", seeds_min, seeds_max, CaloE_min, CaloE_max, x_label_size);
  plot_calo(c1, c2, run, canvas_idx++, h2Seeds_SumE, "Seeds-SumE", seeds_min, seeds_max, SumE_min, SumE_max, x_label_size);
}

void DisplayJetAnav3::draw_UE(const std::string& run)
{
  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>();
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(3500, 2500);

  std::unique_ptr<TCanvas> c2 = std::make_unique<TCanvas>();
  c2->SetTickx();
  c2->SetTicky();

  c2->SetCanvasSize(1200, 1000);

  gStyle->SetOptTitle(1);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleW(1);
  gStyle->SetTitleH(0.08F);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);

  c1->Divide(6, 5, 0.00025F, 0.00025F);

  std::string output = std::format("{}/pdf-UE/plots-{}.pdf", m_output_dir, run);

  plot_UE(c1.get(), c2.get(), run);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig)
  {
    c1->Print(std::format("{}/images/UE-{}.png", m_output_dir, run).c_str());
  }
}

void DisplayJetAnav3::process()
{
  // set sPHENIX plotting style
  SetsPhenixStyle();

  TH1::AddDirectory(kFALSE);

  std::ifstream input_file(m_input_file);

  if (!input_file.is_open())
  {
    std::cout << std::format("Error: Could not open file {}\n", m_input_file);
    return;
  }

  std::string line;

  // create output directories
  std::filesystem::create_directories(m_output_dir + "/images");
  std::filesystem::create_directories(m_output_dir + "/pdf");
  std::filesystem::create_directories(m_output_dir + "/pdf-calo");
  std::filesystem::create_directories(m_output_dir + "/pdf-UE");

  while (std::getline(input_file, line))
  {
    std::filesystem::path p(line);
    std::string run = p.stem().string();
    std::cout << std::format("File: {}, Run: {}\n", line, run);
    m_hists = myUtils::read_hists(line);

    draw(run);
    draw_calo(run);
    draw_UE(run);
  }
}

int main(int argc, const char* const argv[])
{
  gROOT->SetBatch(true);

  if (argc < 2 || argc > 5)
  {
    std::cout << "Usage: " << argv[0] << " <input_file> [output_directory] [saveFig] [saveIndividualFig]" << std::endl;
    return 1;
  }

  const std::string input_file = argv[1];
  std::string output_dir = (argc >= 3) ? argv[2] : ".";
  bool saveFig = (argc >= 4) ? std::atoi(argv[3]) : false;
  bool saveIndividualFig = (argc >= 5) ? std::atoi(argv[4]) : false;

  // Print Inputs
  std::cout << std::format("{:#<20}\n", "");
  std::cout << std::format("Input: {}\n", input_file);
  std::cout << std::format("Output Dir: {}\n", output_dir);
  std::cout << std::format("Save Fig: {}\n", saveFig);
  std::cout << std::format("Save Fig (Individual): {}\n", saveIndividualFig);
  std::cout << std::format("{:#<20}\n", "");

  try
  {
    DisplayJetAnav3 analysis(input_file, output_dir, saveFig, saveIndividualFig);
    analysis.run();
  }
  catch (const std::exception& e)
  {
    std::cout << std::format("An exception occurred: {}", e.what()) << std::endl;
    return 1;
  }

  std::cout << "Analysis complete." << std::endl;
  return 0;
}
