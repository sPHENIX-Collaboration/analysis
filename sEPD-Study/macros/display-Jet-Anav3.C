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

  // plots
  void plot_jet(TCanvas* c1, TCanvas* c2, const std::string& run);
  void plot_centrality(TCanvas* c1, TCanvas* c2, const std::string& run);

  void plot_psi(TCanvas* c1, TCanvas* c2, const std::string& run);
  void plot_psi(TCanvas* c1, TCanvas* c2, const std::string& run, const std::string& side, int id);

  void plot_sEPD_NS(TCanvas* c1, TCanvas* c2, const std::string& run);
  void plot_sEPD_NS_Norm(TCanvas* c1, TCanvas* c2, const std::string& run);
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================

void DisplayJetAnav3::plot_jet(TCanvas* c1, TCanvas* c2, const std::string& run)
{
  c1->cd(1);
  gPad->SetTopMargin(0.08f);
  gPad->SetLogy();

  auto* hJetPt = dynamic_cast<TH3*>(m_hists["h3JetPhiEtaPt"].get())->Project3D("z");
  auto* hJetPtv2 = dynamic_cast<TH3*>(m_hists["h3JetPhiEtaPtv2"].get())->Project3D("z");

  hJetPt->Draw();
  hJetPt->SetTitle(std::format("Run: {}", run).c_str());
  hJetPt->SetLineColor(kBlue);
  hJetPt->SetLineWidth(3);
  hJetPt->GetYaxis()->SetMaxDigits(3);
  hJetPt->GetYaxis()->SetTitle("Jet Yield / 1 GeV");
  hJetPt->GetXaxis()->SetLabelSize(0.06f);
  hJetPt->GetXaxis()->SetTitleSize(0.05f);
  hJetPt->GetXaxis()->SetTitleOffset(1.1f);
  hJetPt->GetYaxis()->SetTitleOffset(1.2f);
  hJetPt->GetXaxis()->SetRangeUser(0, 45);

  hJetPtv2->Draw("same");
  hJetPtv2->SetLineColor(kRed);
  hJetPtv2->SetLineWidth(3);

  double xshift = 0.25;
  double yshift = 0.05;

  m_leg["jet"] = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
  auto* leg = m_leg["jet"].get();
  leg->SetFillStyle(0);
  leg->SetTextSize(0.06f);
  leg->AddEntry(hJetPt, "Default", "l");
  leg->AddEntry(hJetPtv2, "Dead IB Mask", "l");
  leg->Draw("same");

  if (m_saveIndividualFig)
  {
    c2->cd();
    gPad->SetTopMargin(0.08f);

    gPad->SetLogy();
    hJetPt->Draw();
    hJetPtv2->Draw("same");
    leg->Draw("same");

    c2->Print(std::format("{}/images/Jet-pT-{}.png", m_output_dir, run).c_str());
  }

  gPad->SetLogy(0);
}

void DisplayJetAnav3::plot_centrality(TCanvas* c1, TCanvas* c2, const std::string& run)
{
  c1->cd(2);
  gPad->SetTopMargin(0.08f);

  auto* hCentrality = m_hists["hCentrality"].get();

  hCentrality->Draw();
  hCentrality->SetTitle(std::format("Run: {}", run).c_str());
  hCentrality->SetLineColor(kBlue);
  hCentrality->SetLineWidth(3);
  hCentrality->GetYaxis()->SetRangeUser(0, hCentrality->GetMaximum() * 1.1);
  hCentrality->GetYaxis()->SetMaxDigits(3);
  hCentrality->GetXaxis()->SetLabelSize(0.06f);
  hCentrality->GetXaxis()->SetTitleSize(0.06f);
  hCentrality->GetYaxis()->SetLabelSize(0.06f);
  hCentrality->GetYaxis()->SetTitleSize(0.06f);
  hCentrality->GetXaxis()->SetTitleOffset(0.9f);

  if (m_saveIndividualFig)
  {
    c2->cd();
    gPad->SetTopMargin(0.08f);

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
  gPad->SetTopMargin(0.08f);

  std::string proj = (side == "S") ? "x" : "y";

  auto* hPsi_raw = dynamic_cast<TH3*>(m_hists["h3_sEPD_Psi_2_raw"].get())->Project3D(proj.c_str());
  auto* hPsi_corr = dynamic_cast<TH3*>(m_hists["h3_sEPD_Psi_2_corr2"].get())->Project3D(proj.c_str());

  hPsi_raw->Draw();
  hPsi_raw->SetTitle(std::format("Run: {}", run).c_str());
  hPsi_raw->GetYaxis()->SetTitle("Events / (2#pi / 126)");
  hPsi_raw->SetLineColor(kBlue);
  hPsi_raw->SetLineWidth(3);
  hPsi_raw->GetYaxis()->SetRangeUser(0, hPsi_raw->GetMaximum() * 1.1);
  hPsi_raw->GetXaxis()->SetLabelSize(0.06f);
  hPsi_raw->GetXaxis()->SetTitleSize(0.06f);
  hPsi_raw->GetYaxis()->SetLabelSize(0.06f);
  hPsi_raw->GetYaxis()->SetTitleSize(0.06f);
  hPsi_raw->GetXaxis()->SetTitleOffset(0.95f);
  hPsi_raw->GetYaxis()->SetTitleOffset(1.2f);

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
  leg->SetTextSize(0.06f);
  leg->AddEntry(hPsi_raw, "Raw", "l");
  leg->AddEntry(hPsi_corr, "Corrected", "l");
  leg->Draw("same");

  if (m_saveIndividualFig)
  {
    c2->cd();
    gPad->SetTopMargin(0.08f);

    hPsi_raw->Draw();
    hPsi_corr->Draw("same");
    leg->Draw("same");

    c2->Print(std::format("{}/images/Psi-{}-{}.png", m_output_dir, side, run).c_str());
  }
}

void DisplayJetAnav3::plot_sEPD_NS(TCanvas* c1, TCanvas* c2, const std::string& run)
{
  c1->cd(5);
  gPad->SetTopMargin(0.08f);
  gPad->SetLeftMargin(0.17f);

  auto* hSP = m_hists["hSP_res_prof_2"].get();

  hSP->Draw("p e X0");
  hSP->Draw("same hist l p");
  hSP->SetTitle(std::format("Run: {}", run).c_str());
  hSP->SetLineColor(kBlue);
  hSP->SetLineColor(kBlue);
  hSP->SetLineWidth(3);
  hSP->GetYaxis()->SetRangeUser(0, hSP->GetMaximum() * 1.1);
  hSP->GetXaxis()->SetLabelSize(0.06f);
  hSP->GetXaxis()->SetTitleSize(0.06f);
  hSP->GetYaxis()->SetLabelSize(0.06f);
  hSP->GetYaxis()->SetTitleSize(0.06f);
  hSP->GetXaxis()->SetTitleOffset(0.9f);

  if (m_saveIndividualFig)
  {
    c2->cd();
    gPad->SetTopMargin(0.08f);
    gPad->SetLeftMargin(0.17f);

    hSP->Draw("p e X0");
    hSP->Draw("same hist l p");

    c2->Print(std::format("{}/images/hSP-NS-{}.png", m_output_dir, run).c_str());
  }
}

void DisplayJetAnav3::plot_sEPD_NS_Norm(TCanvas* c1, TCanvas* c2, const std::string& run)
{
  c1->cd(6);
  gPad->SetTopMargin(0.08f);
  gPad->SetLeftMargin(0.18f);

  auto* hSP = m_hists["hSP_evt_res_prof_2"].get();

  hSP->Draw("p e X0");
  hSP->Draw("same hist l p");
  hSP->SetTitle(std::format("Run: {}", run).c_str());
  hSP->SetLineColor(kBlue);
  hSP->SetLineColor(kBlue);
  hSP->SetLineWidth(3);
  hSP->GetYaxis()->SetRangeUser(0, hSP->GetMaximum() * 1.1);
  hSP->GetXaxis()->SetLabelSize(0.06f);
  hSP->GetXaxis()->SetTitleSize(0.06f);
  hSP->GetYaxis()->SetLabelSize(0.06f);
  hSP->GetYaxis()->SetTitleSize(0.06f);
  hSP->GetXaxis()->SetTitleOffset(0.9f);
  hSP->GetYaxis()->SetTitleOffset(1.5f);

  if (m_saveIndividualFig)
  {
    c2->cd();
    gPad->SetTopMargin(0.08f);
    gPad->SetLeftMargin(0.18f);

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
  gStyle->SetTitleH(0.08f);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);

  c1->Divide(3, 2, 0.00025f, 0.00025f);

  std::string output = std::format("{}/pdf/plots-{}.pdf", m_output_dir, run);

  // Jet pT
  plot_jet(c1.get(), c2.get(), run);

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

  while (std::getline(input_file, line))
  {
    std::filesystem::path p(line);
    std::string stem = p.stem().string();
    std::string run = myUtils::split(stem, '-')[1];
    std::cout << std::format("File: {}, Run: {}\n", line, run);
    m_hists = myUtils::read_hists(line);

    draw(run);
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
