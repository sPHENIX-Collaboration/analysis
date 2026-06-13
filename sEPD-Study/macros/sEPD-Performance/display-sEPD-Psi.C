// -- utils
#include "myUtils.C"

// ====================================================================
// sPHENIX Includes
// ====================================================================
#include <sPhenixStyle.C>
#include <calobase/TowerInfoDefs.h>

// ====================================================================
// ROOT Includes
// ====================================================================
#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TH1.h>
#include <TH2.h>
#include <TKey.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TLine.h>
#include <TLatex.h>
#include <TPaletteAxis.h>
#include <TPaveText.h>
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
#include <unordered_set>
#include <algorithm>
#include <iterator> // for std::inserter
#include <cmath>
#include <concepts>
#include <span>

// ====================================================================
// The Analysis Class
// ====================================================================
class displaySEPDPsi
{
 public:
  // The constructor takes the configuration
  displaySEPDPsi(std::string output_dir, bool saveFig)
    : m_output_dir(std::move(output_dir))
    , m_saveFig(saveFig)
  {
  }

  void run()
  {
    read_hists();
    draw();
  }

 private:
  // Configuration stored as members
  std::string m_output_dir;
  std::map<std::string, std::unique_ptr<TH1>> m_hists;

  bool m_saveFig{false};

  // --- Private Helper Methods ---
  void read_hists();
  void draw();
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void displaySEPDPsi::read_hists()
{
  std::filesystem::path base_path = "/gpfs02/sphenix/user/anarde/sEPD-Calib";

  // Type
  // A: Default (No Dead Channels)
  // B: With Dead Map (Dead Channels masked to match acceptance from data)

  std::filesystem::path mc_A = base_path / "2026-03-29-Run-31-sHijing_0_20fm/stage-QVecCalib-0-0.5";
  std::filesystem::path mc_B = base_path / "2026-06-12-Run-31-sHijing_0_20fm/stage-QVecCalib-50-0.5";

  std::filesystem::path mc_A_raw = mc_A / "ComputeRecentering/output/hist/QVecCalib-31.root";
  std::filesystem::path mc_A_flat = mc_A / "ApplyFlattening/output/hist/QVecCalib-31.root";

  std::filesystem::path mc_B_raw = mc_B / "ComputeRecentering/output/hist/QVecCalib-31.root";
  std::filesystem::path mc_B_flat = mc_B / "ApplyFlattening/output/hist/QVecCalib-31.root";

  std::unordered_set<std::string> names = {"h2_sEPD_Psi_S_2", "h2_sEPD_Psi_N_2", "h2_sEPD_Psi_S_2_corr2", "h2_sEPD_Psi_N_2_corr2"};

   for (auto&& [file, tag] : {std::pair{mc_A_raw,  "A"},
                           std::pair{mc_A_flat, "A"},
                           std::pair{mc_B_raw,  "B"},
                           std::pair{mc_B_flat, "B"}})
   {
    m_hists.merge(myUtils::read_hists(file, std::format("_{}", tag), &names));
   }
}

void displaySEPDPsi::draw()
{
  // set sPHENIX plotting style
  SetsPhenixStyle();

  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>("c1");
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(1200, 1000);
  c1->SetLeftMargin(.08F);
  c1->SetRightMargin(.02F);
  c1->SetTopMargin(.05F);
  c1->SetBottomMargin(.09F);

  gStyle->SetOptTitle(1);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleFontSize(0.08F);
  gStyle->SetTitleW(1);
  gStyle->SetTitleH(0.08F);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);

  std::string output;

  // create output directory
  std::filesystem::create_directories(std::format("{}/images", m_output_dir));
  std::filesystem::create_directories(std::format("{}/pdf", m_output_dir));
  output = std::format("{}/pdf/plots.pdf", m_output_dir);

  c1->Print((output + "[").c_str(), "pdf portrait");

  {
    int bin_low = 1;
    int bin_high = 60;

    struct PlotOptions
    {
     std::string det{"S"};
     double ymax{16e3};
    };

    auto plotAndSave = [&](std::span<TH1* const> hist, const std::string name, PlotOptions opts = {})
    {
      auto* h_raw_A = hist[0];
      auto* h_flat_A = hist[1];

      auto* h_raw_B = hist[2];
      auto* h_flat_B = hist[3];

      h_raw_A->Draw("HIST");
      h_raw_B->Draw("same HIST");
      h_flat_A->Draw("same HIST");
      h_flat_B->Draw("same HIST");

      std::string title = std::format("; 2#Psi_{{2}}^{{{}}}; Events", opts.det);

      h_raw_A->SetTitle(title.c_str());

      h_raw_A->SetLineWidth(6);
      h_raw_B->SetLineWidth(6);
      h_flat_A->SetLineWidth(6);
      h_flat_B->SetLineWidth(6);

      h_raw_A->SetLineColor(kRed+1);
      h_raw_B->SetLineColor(kAzure+1);

      h_flat_A->SetLineColor(kGreen+2);
      h_flat_B->SetLineColor(kOrange+7);

      double ymax = std::max(h_raw_A->GetMaximum(), h_raw_B->GetMaximum())*1.1;

      if(opts.ymax)
      {
        h_raw_A->GetYaxis()->SetRangeUser(0, opts.ymax);
      }
      else
      {
        h_raw_A->GetYaxis()->SetRangeUser(0, ymax);
      }

      h_raw_A->GetYaxis()->SetMaxDigits(3);
      h_raw_A->GetYaxis()->SetTitleSize(0.04F);
      h_raw_A->GetYaxis()->SetLabelSize(0.04F);
      h_raw_A->GetXaxis()->SetTitleSize(0.04F);
      h_raw_A->GetXaxis()->SetLabelSize(0.04F);

      h_raw_A->GetYaxis()->SetTitleOffset(1.F);
      h_raw_A->GetXaxis()->SetTitleOffset(1.F);

      double xshift = -0.1;
      double yshift = -0.3;

      std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .4 + yshift, 0.4 + xshift, .6 + yshift);
      leg->SetFillStyle(0);
      leg->SetTextSize(0.05F);
      leg->AddEntry(h_raw_A, "Raw: Default", "l");
      leg->AddEntry(h_raw_B, "Raw: Dead Map", "l");
      leg->AddEntry(h_flat_A, "Flat: Default", "l");
      leg->AddEntry(h_flat_B, "Flat: Dead Map", "l");
      leg->Draw("same");

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());
    };

    TH1* hPsi_S_raw_A = dynamic_cast<TH2*>(m_hists["h2_sEPD_Psi_S_2_A"].get())->ProjectionY("S_raw_A", bin_low, bin_high);
    TH1* hPsi_S_raw_B = dynamic_cast<TH2*>(m_hists["h2_sEPD_Psi_S_2_B"].get())->ProjectionY("S_raw_B", bin_low, bin_high);
    TH1* hPsi_N_raw_A = dynamic_cast<TH2*>(m_hists["h2_sEPD_Psi_N_2_A"].get())->ProjectionY("N_raw_A", bin_low, bin_high);
    TH1* hPsi_N_raw_B = dynamic_cast<TH2*>(m_hists["h2_sEPD_Psi_N_2_B"].get())->ProjectionY("N_raw_B", bin_low, bin_high);

    TH1* hPsi_S_flat_A = dynamic_cast<TH2*>(m_hists["h2_sEPD_Psi_S_2_corr2_A"].get())->ProjectionY("S_flat_A", bin_low, bin_high);
    TH1* hPsi_S_flat_B = dynamic_cast<TH2*>(m_hists["h2_sEPD_Psi_S_2_corr2_B"].get())->ProjectionY("S_flat_B", bin_low, bin_high);
    TH1* hPsi_N_flat_A = dynamic_cast<TH2*>(m_hists["h2_sEPD_Psi_N_2_corr2_A"].get())->ProjectionY("N_flat_A", bin_low, bin_high);
    TH1* hPsi_N_flat_B = dynamic_cast<TH2*>(m_hists["h2_sEPD_Psi_N_2_corr2_B"].get())->ProjectionY("N_flat_B", bin_low, bin_high);

    plotAndSave({{hPsi_S_raw_A, hPsi_S_flat_A, hPsi_S_raw_B, hPsi_S_flat_B}}, "Psi-S");
    plotAndSave({{hPsi_N_raw_A, hPsi_N_flat_A, hPsi_N_raw_B, hPsi_N_flat_B}}, "Psi-N", {.det = "N"});
  }

  c1->Print((output + "]").c_str(), "pdf portrait");
}

int main(int argc, const char* const argv[])
{
  gROOT->SetBatch(true);

  if (argc > 3)
  {
    std::cout << "Usage: " << argv[0] << " [output_directory] [saveFig]" << std::endl;
    return 1;
  }

  std::string output_dir = (argc >= 2) ? argv[1] : ".";
  bool saveFig = (argc >= 3) ? std::atoi(argv[2]) : false;

  try
  {
    displaySEPDPsi analysis(output_dir, saveFig);
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
