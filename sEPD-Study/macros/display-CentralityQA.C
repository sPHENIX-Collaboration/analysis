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
#include <TMultiGraph.h>
#include <TH1.h>
#include <TH2.h>
#include <TKey.h>
#include <TLegend.h>
#include <TLine.h>
#include <TLatex.h>
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
#include <unordered_set>
#include <algorithm>
#include <iterator> // for std::inserter

// ====================================================================
// The Analysis Class
// ====================================================================
class DisplayCentralityQA
{
 public:
  // The constructor takes the configuration
  DisplayCentralityQA(std::string output_dir, bool saveFig)
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

  std::unordered_set<std::string> m_names = {};

  // --- Private Helper Methods ---
  void read_hists();
  void init_hists();
  void draw();
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void DisplayCentralityQA::read_hists()
{
  std::string input_default = "/gpfs02/sphenix/user/anarde/sEPD-Study/CentralityQA/default/test.root";
  std::string input_new = "/gpfs02/sphenix/user/anarde/sEPD-Study/CentralityQA/new/test.root";

  m_hists = myUtils::read_hists(input_default, "_default");
  m_hists.merge(myUtils::read_hists(input_new, "_new"));
}

void DisplayCentralityQA::draw()
{
  // set sPHENIX plotting style
  SetsPhenixStyle();

  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>("c1");
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(1200, 1000);
  c1->SetLeftMargin(.12F);
  c1->SetRightMargin(.02F);
  c1->SetTopMargin(.11F);
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

  // ---------------------------------------------------

  {
    c1->SetRightMargin(.1F);
    gPad->SetTicky(0);

    auto plotAndSave = [&](TH1* hist, std::string_view name)
    {
      hist->Draw("HIST");

      hist->GetYaxis()->SetMaxDigits(3);
      hist->GetXaxis()->SetLabelSize(0.05F);
      hist->GetXaxis()->SetTitle("Type");
      hist->SetLineColor(kBlue);
      hist->SetLineWidth(3);

      double ymax = hist->GetMaximum() * 1.1;
      hist->GetYaxis()->SetRangeUser(0, ymax);

      // 1. Calculate the percentage scale
      // We want hist->GetBinContent(1) to equal 100%
      double totalEvents = hist->GetBinContent(1);
      double right_axis_max = (ymax / totalEvents) * 100.0;

      // 2. Force a pad update to ensure coordinates are ready
      gPad->Update();

      // 3. Define and draw the TGaxis
      // Parameters: x_start, y_start, x_end, y_end, val_min, val_max, ndiv, options
      TGaxis* axis = new TGaxis(gPad->GetUxmax(), gPad->GetUymin(),
                                gPad->GetUxmax(), gPad->GetUymax(),
                                0, right_axis_max, 510, "+L");

      axis->SetTitle("Percentage (%)");
      axis->SetLabelFont(hist->GetYaxis()->GetLabelFont());
      axis->SetLabelSize(hist->GetYaxis()->GetLabelSize());
      axis->SetTitleFont(hist->GetYaxis()->GetTitleFont());
      axis->SetTitleSize(hist->GetYaxis()->GetTitleSize());
      axis->SetTitleOffset(1.2F);  // Adjust so it doesn't overlap the numbers

      axis->Draw();

      // Loop through the bins to place text
      for (int i = 1; i <= hist->GetNbinsX(); ++i)
      {
        double binContent = hist->GetBinContent(i);
        double percentage = (binContent / totalEvents) * 100.0;

        // Define the x position (center of the bin)
        // and y position (just slightly above the bar)
        double xPos = hist->GetBinCenter(i);
        double yPos = binContent + (ymax * 0.02);  // 2% offset above the bar

        // Create the label string (fixed to 1 decimal place)
        TString label = TString::Format("%.1f%%", percentage);

        // Create and style the text object
        TLatex* tex = new TLatex(xPos, yPos, label);
        tex->SetTextAlign(21);  // Center-aligned horizontally, bottom-aligned vertically
        tex->SetTextSize(0.05F);
        tex->SetTextFont(42);          // Standard Helvetica
        tex->SetTextColor(kBlue + 2);  // Slightly darker blue to stand out
        tex->Draw();
      }

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());
    };

    auto* hEvent_default = m_hists["hEvent_default"].get();
    auto* hEvent_new = m_hists["hEvent_new"].get();

    plotAndSave(hEvent_default, "hEvent_default");
    plotAndSave(hEvent_new, "hEvent_new");

    c1->SetRightMargin(.02F);
    gPad->SetTicky(1);
  }

  // ---------------------------------------------------

  {
    struct PlotOptions
    {
      std::string title{};
    };

    auto plotAndSave = [&](TH1* hist_default, TH1* hist_new, std::string_view name, PlotOptions opts = {})
    {
      hist_default->Draw("HIST");
      hist_new->Draw("same HIST");

      hist_default->SetLineColor(kBlue);
      hist_new->SetLineColor(kRed);

      hist_default->SetLineWidth(3);
      hist_new->SetLineWidth(2);

      double ymax = std::max(hist_default->GetMaximum(), hist_new->GetMaximum())*1.1;

      hist_default->GetYaxis()->SetRangeUser(0, ymax);

      hist_default->GetYaxis()->SetMaxDigits(3);

      if (!opts.title.empty())
      {
        hist_default->SetTitle(opts.title.c_str());
      }

      double xshift = 0.57;
      double yshift = 0.16;

      std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .6 + yshift, 0.4 + xshift, .7 + yshift);
      leg->SetFillStyle(0);
      leg->SetTextSize(0.05f);
      leg->AddEntry(hist_default, "Default", "l");
      leg->AddEntry(hist_new, "New", "l");
      leg->Draw("same");

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());
    };

    auto* hZVtx_default = m_hists["hZVertex_default"].get();
    auto* hZVtx_new = m_hists["hZVertex_new"].get();

    auto* hCentrality_default = m_hists["hCentrality_default"].get();
    auto* hCentrality_new = m_hists["hCentrality_new"].get();

    auto* hCentralityZ50_default = m_hists["hCentralityZ50_default"].get();
    auto* hCentralityZ50_new = m_hists["hCentralityZ50_new"].get();

    auto* hCentralityZOuter_default = m_hists["hCentralityZOuter_default"].get();
    auto* hCentralityZOuter_new = m_hists["hCentralityZOuter_new"].get();

    plotAndSave(hZVtx_default, hZVtx_new, "hZVertex-Overlay");
    plotAndSave(hCentrality_default, hCentrality_new, "hCentrality-Overlay");
    plotAndSave(hCentralityZ50_default, hCentralityZ50_new, "hCentralityZ50-Overlay");
    plotAndSave(hCentralityZOuter_default, hCentralityZOuter_new, "hCentralityZOuter-Overlay");

    auto* h2ZVertexCentrality_default = dynamic_cast<TH2*>(m_hists["h2ZVertexCentrality_default"].get());
    auto* h2ZVertexCentrality_new = dynamic_cast<TH2*>(m_hists["h2ZVertexCentrality_new"].get());

    auto plotAndSaveCent = [&](TH2* hist2D_default, TH2* hist2D_new, int cent)
    {
      std::string name_default = std::format("default_{}", cent);
      std::string name_new = std::format("new_{}", cent);

      std::string name = std::format("hZVertexCentrality-{}-Overlay", cent);
      std::string title = std::format("Centrality {}% and MB", cent);

      auto* hist_default = hist2D_default->ProjectionX(name_default.c_str(), cent + 1, cent + 1);
      auto* hist_new = hist2D_new->ProjectionX(name_new.c_str(), cent + 1, cent + 1);

      // Draw Config
      hist_default->GetYaxis()->SetTitle("Events");
      hist_default->GetYaxis()->SetTitleOffset(1.F);

      plotAndSave(hist_default, hist_new, name, {.title = title});
    };

    plotAndSaveCent(h2ZVertexCentrality_default, h2ZVertexCentrality_new, 1);
    plotAndSaveCent(h2ZVertexCentrality_default, h2ZVertexCentrality_new, 2);
    plotAndSaveCent(h2ZVertexCentrality_default, h2ZVertexCentrality_new, 3);
    plotAndSaveCent(h2ZVertexCentrality_default, h2ZVertexCentrality_new, 4);
  }

  // ---------------------------------------------------

  {
    c1->SetLeftMargin(.1F);
    c1->SetRightMargin(.12F);
    c1->SetTopMargin(.11F);
    c1->SetBottomMargin(.09F);

    auto plotAndSave = [&](TH2* hist, std::string_view name)
    {
      hist->Draw("COLZ1");
      hist->GetZaxis()->SetMaxDigits(3);

      hist->SetMaximum(7e3);

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());

      hist->GetYaxis()->SetRangeUser(-0.5, 5.5);

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}-zoom.png", m_output_dir, name).c_str());
    };

    auto* h2ZVertexCentrality_default = dynamic_cast<TH2*>(m_hists["h2ZVertexCentrality_default"].get());
    auto* h2ZVertexCentrality_new = dynamic_cast<TH2*>(m_hists["h2ZVertexCentrality_new"].get());

    plotAndSave(h2ZVertexCentrality_default, "h2ZVertexCentrality_default");
    plotAndSave(h2ZVertexCentrality_new, "h2ZVertexCentrality_new");

    c1->SetLeftMargin(.12F);
    c1->SetRightMargin(.02F);
    c1->SetTopMargin(.11F);
    c1->SetBottomMargin(.09F);
  }

  // ---------------------------------------------------

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
    DisplayCentralityQA analysis(output_dir, saveFig);
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
