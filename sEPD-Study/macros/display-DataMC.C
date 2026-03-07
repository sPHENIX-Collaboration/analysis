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
#include <cmath>
#include <concepts>

// ====================================================================
// The Analysis Class
// ====================================================================
class DisplayDataMC
{
 public:
  // The constructor takes the configuration
  DisplayDataMC(std::string output_dir, bool saveFig)
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
void DisplayDataMC::read_hists()
{
  std::string input = "/gpfs02/sphenix/user/anarde/sEPD-Study/MC/current/test-54404.root";
  m_hists = myUtils::read_hists(input);
}

void DisplayDataMC::draw()
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

  // c1->Divide(3, 3, 0.00025F, 0.00025F);

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

    auto* hEvent = m_hists["hEvent"].get();

    hEvent->Draw("HIST");

    hEvent->GetYaxis()->SetMaxDigits(3);
    hEvent->GetXaxis()->SetLabelSize(0.06F);
    hEvent->SetLineColor(kBlue);
    hEvent->SetLineWidth(3);

    double ymax = hEvent->GetMaximum()*1.1;
    hEvent->GetYaxis()->SetRangeUser(0, ymax);

    // 1. Calculate the percentage scale
    // We want hEvent->GetBinContent(1) to equal 100%
    double totalEvents = hEvent->GetBinContent(1);
    double right_axis_max = (ymax / totalEvents) * 100.0;

    // 2. Force a pad update to ensure coordinates are ready
    gPad->Update();

    // 3. Define and draw the TGaxis
    // Parameters: x_start, y_start, x_end, y_end, val_min, val_max, ndiv, options
    TGaxis* axis = new TGaxis(gPad->GetUxmax(), gPad->GetUymin(),
                              gPad->GetUxmax(), gPad->GetUymax(),
                              0, right_axis_max, 510, "+L");

    axis->SetTitle("Percentage (%)");
    axis->SetLabelFont(hEvent->GetYaxis()->GetLabelFont());
    axis->SetLabelSize(hEvent->GetYaxis()->GetLabelSize());
    axis->SetTitleFont(hEvent->GetYaxis()->GetTitleFont());
    axis->SetTitleSize(hEvent->GetYaxis()->GetTitleSize());
    axis->SetTitleOffset(1.2F);  // Adjust so it doesn't overlap the numbers

    axis->Draw();

    // Loop through the bins to place text
    for (int i = 1; i <= hEvent->GetNbinsX(); ++i)
    {
      double binContent = hEvent->GetBinContent(i);
      double percentage = (binContent / totalEvents) * 100.0;

      // Define the x position (center of the bin)
      // and y position (just slightly above the bar)
      double xPos = hEvent->GetBinCenter(i);
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
    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hEvent").c_str());

    c1->SetRightMargin(.02F);
    gPad->SetTicky(1);
  }

  // ---------------------------------------------------

  {
    auto plotAndSave = [&](std::string_view name = "")
    {
      auto* hist = m_hists[std::string(name)].get();

      hist->Draw("HIST");

      hist->GetYaxis()->SetMaxDigits(3);
      hist->SetLineColor(kBlue);
      hist->SetLineWidth(3);

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());
    };

    plotAndSave("hCentrality");
    plotAndSave("hCentralityZ50");
    plotAndSave("hCentralityZOuter");
  }

  // ---------------------------------------------------

  {
    c1->SetRightMargin(.14F);
    c1->SetTopMargin(.08F);
    auto* h2ZVertexCentrality = dynamic_cast<TH2*>(m_hists["h2ZVertexCentrality"].get());

    h2ZVertexCentrality->Draw("COLZ1");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2ZVertexCentrality").c_str());

    h2ZVertexCentrality->GetYaxis()->SetRangeUser(-0.5, 10.5);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2ZVertexCentrality-zoom").c_str());

    c1->SetRightMargin(.02F);
    c1->SetTopMargin(.11F);
  }

  // ---------------------------------------------------

  {
    struct PlotOptions
    {
      int bin_low{0};
      int bin_high{0};
      std::string title;
    };

    auto* h2ZVertexCentrality = dynamic_cast<TH2*>(m_hists["h2ZVertexCentrality"].get());
    auto plotAndSave = [&](TH2* h2, PlotOptions opts = {}, std::string_view name = "")
    {
      auto* h1 = h2->ProjectionX("px", opts.bin_low, opts.bin_high);

      h1->Draw("HIST");

      h1->SetLineColor(kBlue);
      h1->SetLineWidth(3);
      h1->SetTitle(opts.title.c_str());
      h1->GetYaxis()->SetTitle("Events");
      h1->GetYaxis()->SetTitleOffset(1.2F);
      h1->GetYaxis()->SetMaxDigits(3);

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());
    };

    plotAndSave(h2ZVertexCentrality, {.bin_low = 2, .bin_high = 2, .title = "Min Bias & Centrality: 1%"}, "h2ZVertexCentrality_1");
    plotAndSave(h2ZVertexCentrality, {.bin_low = 3, .bin_high = 3, .title = "Min Bias & Centrality: 2%"}, "h2ZVertexCentrality_2");
    plotAndSave(h2ZVertexCentrality, {.bin_low = 4, .bin_high = 4, .title = "Min Bias & Centrality: 3%"}, "h2ZVertexCentrality_3");
    plotAndSave(h2ZVertexCentrality, {.bin_low = 5, .bin_high = 5, .title = "Min Bias & Centrality: 4%"}, "h2ZVertexCentrality_4");
    plotAndSave(h2ZVertexCentrality, {.bin_low = 31, .bin_high = 31, .title = "Min Bias & Centrality: 30%"}, "h2ZVertexCentrality_30");
  }

  // ---------------------------------------------------

  {
    auto* hZVertex = m_hists["hZVertex"].get();

    hZVertex->Draw("HIST");

    hZVertex->SetLineColor(kBlue);
    hZVertex->SetLineWidth(3);
    hZVertex->GetYaxis()->SetMaxDigits(3);

    int xlow = hZVertex->FindBin(-10);
    int xhigh = hZVertex->FindBin(10)-1;

    double events = hZVertex->Integral(xlow,xhigh);
    double total_events = hZVertex->Integral(1, hZVertex->GetNbinsX());

    std::cout << std::format("Events |z| < 10: {} ({:.2f}%), Total: {}\n", events, events*100/total_events, total_events);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hZVertex").c_str());
  }

  // ---------------------------------------------------

  {
    gPad->SetLogy();
    c1->SetRightMargin(.04F);

    struct PlotOptions
    {
      short int color{kBlue};
      double xlow{0};
      double xhigh{0};
      bool logy{true};
    };

    auto ceil_pow10 = []<std::floating_point T>(T val) -> T
    {
      if (val <= 0) return 1;
      // We use std::ceil(std::log10(val)) to find the exponent
      return std::pow(static_cast<T>(10), std::ceil(std::log10(val)));
    };

    auto plotAndSave = [&](std::string_view name, PlotOptions opts = {}, std::string_view tag = "")
    {
      auto* hist = m_hists[std::string(name)].get();
      if (!hist) return;  // Safety check

      hist->SetLineColor(opts.color);
      hist->SetLineWidth(3);

      double ymax{0};
      if (opts.xlow < opts.xhigh)
      {
        int xbin = hist->GetXaxis()->FindBin(1);
        ymax = hist->GetBinContent(xbin)*1.1;
        hist->GetXaxis()->SetRangeUser(opts.xlow, opts.xhigh);
      }

      if(opts.logy)
      {
         gPad->SetLogy();
      }
      else
      {
        gPad->SetLogy(0);
      }

      if(!ymax)
      {
        double ylow = 5e-1;
        double yhigh = ceil_pow10(hist->GetMaximum());

        hist->GetYaxis()->SetRangeUser(ylow, yhigh);
      }
      else
      {
        hist->GetYaxis()->SetRangeUser(0, ymax);
        hist->GetYaxis()->SetMaxDigits(3);
      }

      hist->Draw("HIST");

      c1->Print(output.c_str(), "pdf portrait");

      if (m_saveFig)
      {
        std::string imgPath = std::format("{}/images/{}{}.png", m_output_dir, name, tag);
        c1->Print(imgPath.c_str());
      }
    };

    plotAndSave("hSEPD_Charge_DataMC");
    plotAndSave("hSEPD_Charge_DataMC", {.xhigh = 5, .logy = false}, "-zoom");
    plotAndSave("hSEPD_Charge_MC");
    plotAndSave("hSEPD_Charge_MC", {.xhigh = 5, .logy = false}, "-zoom");

    gPad->SetLogy(0);
  }

  // ---------------------------------------------------

  {
    gPad->SetLogy();

    auto plotAndSave = [&](std::string_view name, std::string_view title)
    {
      auto* h2 = dynamic_cast<TH2*>(m_hists[std::string(name)].get());

      int ybins = h2->GetNbinsY();

      auto* h1_r0 = h2->ProjectionX("r0", 1, ybins);
      auto* h1_r1 = h2->ProjectionX("r1", 2, ybins);

      h1_r0->Draw("HIST");
      h1_r1->Draw("HIST same");

      h1_r0->SetLineColor(kBlue);
      h1_r0->SetLineWidth(3);

      h1_r1->SetLineColor(kRed);
      h1_r1->SetLineWidth(3);

      double xlow = h1_r0->GetXaxis()->GetXmin();
      double xhigh = h1_r0->GetXaxis()->GetXmax();
      h1_r0->GetXaxis()->SetRangeUser(xlow, xhigh);

      h1_r0->GetYaxis()->SetTitle("Towers");
      h1_r0->GetYaxis()->SetTitleOffset(1.1F);

      h1_r0->SetTitle(std::string(title).c_str());

      double xshift = 0.4;
      double yshift = 0.15;

      std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .55 + yshift, 0.4 + xshift, .7 + yshift);
      leg->SetFillStyle(0);
      leg->SetTextSize(0.05f);
      leg->AddEntry(h1_r0, "0#leq r_{bin}#leq 15", "l");
      leg->AddEntry(h1_r1, "1#leq r_{bin}#leq 15", "l");
      leg->Draw("same");

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}-overlay.png", m_output_dir, name).c_str());
    };

    plotAndSave("h2SEPD_South_Charge_DataMC", "sEPD South: |z| < 10 cm and MB");
    plotAndSave("h2SEPD_BadMasked_South_Charge_DataMC", "sEPD South: |z| < 10 cm and MB");
    plotAndSave("h2SEPD_North_Charge_DataMC", "sEPD North: |z| < 10 cm and MB");
    plotAndSave("h2SEPD_BadMasked_North_Charge_DataMC", "sEPD North: |z| < 10 cm and MB");

    gPad->SetLogy(0);
  }

  // ---------------------------------------------------

  {
    c1->SetRightMargin(.14F);
    c1->SetTopMargin(.08F);

    auto* pSEPD_Charge_DataMC = m_hists["pSEPD_Charge_DataMC"].get();
    auto* pSEPD_BadMasked_Charge_DataMC = m_hists["pSEPD_BadMasked_Charge_DataMC"].get();
    auto* pSEPD_Charge_MC = m_hists["pSEPD_Charge_MC"].get();
    auto* pSEPD_BadMasked_Charge_MC = m_hists["pSEPD_BadMasked_Charge_MC"].get();

    unsigned int bins_charge = 50;
    unsigned int bins_r = 16;

    double charge_data_low = 0;
    double charge_data_high = 50;

    double charge_mc_low = 0;
    double charge_mc_high = 0.25;

    std::string title = "sEPD South; r_{bin}; Average Charge; Channels";

    std::unique_ptr<TH2> h2SEPD_South_AvgCharge_Data = std::make_unique<TH2F>("h2SEPD_South_AvgCharge_Data", title.c_str(), bins_r, 0, bins_r, bins_charge, charge_data_low, charge_data_high);
    std::unique_ptr<TH2> h2SEPD_BadMasked_South_AvgCharge_Data = std::make_unique<TH2F>("h2SEPD_BadMasked_South_AvgCharge_Data", title.c_str(), bins_r, 0, bins_r, bins_charge, charge_data_low, charge_data_high);
    std::unique_ptr<TH2> h2SEPD_South_AvgCharge_MC = std::make_unique<TH2F>("h2SEPD_South_AvgCharge_MC", title.c_str(), bins_r, 0, bins_r, bins_charge, charge_mc_low, charge_mc_high);
    std::unique_ptr<TH2> h2SEPD_BadMasked_South_AvgCharge_MC = std::make_unique<TH2F>("h2SEPD_BadMasked_South_AvgCharge_MC", title.c_str(), bins_r, 0, bins_r, bins_charge, charge_mc_low, charge_mc_high);

    title = "sEPD North; r_{bin}; Average Charge; Channels";
    std::unique_ptr<TH2> h2SEPD_North_AvgCharge_Data = std::make_unique<TH2F>("h2SEPD_North_AvgCharge_Data", title.c_str(), bins_r, 0, bins_r, bins_charge, charge_data_low, charge_data_high);
    std::unique_ptr<TH2> h2SEPD_BadMasked_North_AvgCharge_Data = std::make_unique<TH2F>("h2SEPD_BadMasked_North_AvgCharge_Data", title.c_str(), bins_r, 0, bins_r, bins_charge, charge_data_low, charge_data_high);
    std::unique_ptr<TH2> h2SEPD_North_AvgCharge_MC = std::make_unique<TH2F>("h2SEPD_North_AvgCharge_MC", title.c_str(), bins_r, 0, bins_r, bins_charge, charge_mc_low, charge_mc_high);
    std::unique_ptr<TH2> h2SEPD_BadMasked_North_AvgCharge_MC = std::make_unique<TH2F>("h2SEPD_BadMasked_North_AvgCharge_MC", title.c_str(), bins_r, 0, bins_r, bins_charge, charge_mc_low, charge_mc_high);

    for (int channel = 0; channel < pSEPD_Charge_DataMC->GetNbinsX(); ++channel)
    {
      int bin = channel + 1;
      unsigned int key = TowerInfoDefs::encode_epd(static_cast<unsigned int>(channel));
      int rbin = static_cast<int>(TowerInfoDefs::get_epd_rbin(key));
      unsigned int arm = TowerInfoDefs::get_epd_arm(key);

      double avg_data = pSEPD_Charge_DataMC->GetBinContent(bin);
      double avg_BadMasked_data = pSEPD_BadMasked_Charge_DataMC->GetBinContent(bin);
      double avg_mc = pSEPD_Charge_MC->GetBinContent(bin);
      double avg_BadMasked_mc = pSEPD_BadMasked_Charge_MC->GetBinContent(bin);

      // South
      if (arm == 0)
      {
        h2SEPD_South_AvgCharge_Data->Fill(rbin, avg_data);
        h2SEPD_South_AvgCharge_MC->Fill(rbin, avg_mc);

        if (avg_BadMasked_data)
        {
          h2SEPD_BadMasked_South_AvgCharge_Data->Fill(rbin, avg_BadMasked_data);
        }

        if (avg_BadMasked_mc)
        {
          h2SEPD_BadMasked_South_AvgCharge_MC->Fill(rbin, avg_BadMasked_mc);
        }
      }
      // North
      else
      {
        h2SEPD_North_AvgCharge_Data->Fill(rbin, avg_data);
        h2SEPD_North_AvgCharge_MC->Fill(rbin, avg_mc);

        if (avg_BadMasked_data)
        {
          h2SEPD_BadMasked_North_AvgCharge_Data->Fill(rbin, avg_BadMasked_data);
        }

        if (avg_BadMasked_mc)
        {
          h2SEPD_BadMasked_North_AvgCharge_MC->Fill(rbin, avg_BadMasked_mc);
        }
      }
    }

    struct PlotOptions
    {
      float xoffset = 0.8F;
      float yoffset = 1.F;
      float zoffset = 0.8F;
    };

    auto plotAndSave = [&](TH2* hist, PlotOptions opts = {}, std::string_view tag = "")
    {
      hist->Draw("COLZ1");

      hist->GetXaxis()->SetTitleOffset(opts.xoffset);
      hist->GetYaxis()->SetTitleOffset(opts.yoffset);
      hist->GetZaxis()->SetTitleOffset(opts.zoffset);

      int bin_high = hist->GetYaxis()->FindBin(40)-1;

      auto* px = hist->ProfileX("px", 2, bin_high, "s");

      px->SetMarkerColor(kRed);
      px->SetLineColor(kRed);

      auto* px_sigma = px->ProjectionX("px_sigma");

      for (int rbin = 1; rbin <= px->GetNbinsX(); ++rbin)
      {
        double sigma = px->GetBinError(rbin);
        px_sigma->SetBinError(rbin, sigma * 3);
      }

      px_sigma->Draw("E2 same");
      px_sigma->SetFillColorAlpha(8, 0.3f);

      px->Draw("same");

      double xshift = 0.47;
      double yshift = 0.18;

      std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .55 + yshift, 0.5 + xshift, .7 + yshift);
      leg->SetFillStyle(0);
      leg->SetTextSize(0.08f);
      leg->AddEntry(px, "#sigma", "l");
      leg->AddEntry(px_sigma, "3#sigma", "f");
      leg->Draw("same");

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}{}.png", m_output_dir, hist->GetName(), tag).c_str());
    };

    plotAndSave(h2SEPD_South_AvgCharge_Data.get());
    plotAndSave(h2SEPD_BadMasked_South_AvgCharge_Data.get());
    plotAndSave(h2SEPD_North_AvgCharge_Data.get());
    plotAndSave(h2SEPD_BadMasked_North_AvgCharge_Data.get());

    plotAndSave(h2SEPD_South_AvgCharge_MC.get(), {.yoffset = 1.2F});
    plotAndSave(h2SEPD_BadMasked_South_AvgCharge_MC.get(), {.yoffset = 1.2F});
    plotAndSave(h2SEPD_North_AvgCharge_MC.get(), {.yoffset = 1.2F});
    plotAndSave(h2SEPD_BadMasked_North_AvgCharge_MC.get(), {.yoffset = 1.2F});
  }

  // ---------------------------------------------------

  {
    gPad->SetLogz();

    auto* h2SEPD_totalcharge_centrality = dynamic_cast<TH2*>(m_hists["h2SEPD_totalcharge_centrality"].get());

    h2SEPD_totalcharge_centrality->Draw("COLZ1");

    h2SEPD_totalcharge_centrality->GetXaxis()->SetMaxDigits(3);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2SEPD_totalcharge_centrality").c_str());

    gPad->SetLogz(0);
  }

  // ---------------------------------------------------

  {
    c1->SetRightMargin(.04F);

    auto* h2SEPD_Psi2_raw_N = dynamic_cast<TH2*>(m_hists["h2SEPD_Psi2_raw_data_mc_N"].get());
    auto* h2SEPD_Psi2_raw_S = dynamic_cast<TH2*>(m_hists["h2SEPD_Psi2_raw_data_mc_S"].get());

    auto* h2SEPD_Psi2_N = dynamic_cast<TH2*>(m_hists["h2SEPD_Psi2_data_mc_N"].get());
    auto* h2SEPD_Psi2_S = dynamic_cast<TH2*>(m_hists["h2SEPD_Psi2_data_mc_S"].get());

    int cent_bin_low = 1;
    int cent_bin_high = h2SEPD_Psi2_N->GetYaxis()->FindBin(60)-1;

    // Get Centrality 0-60%
    auto* hSEPD_Psi2_raw_N = h2SEPD_Psi2_raw_N->ProjectionX("px_raw_N", cent_bin_low, cent_bin_high);
    auto* hSEPD_Psi2_raw_S = h2SEPD_Psi2_raw_S->ProjectionX("px_raw_S", cent_bin_low, cent_bin_high);

    auto* hSEPD_Psi2_N = h2SEPD_Psi2_N->ProjectionX("px_N", cent_bin_low, cent_bin_high);
    auto* hSEPD_Psi2_S = h2SEPD_Psi2_S->ProjectionX("px_S", cent_bin_low, cent_bin_high);

    struct PlotOptions
    {
      std::string title{};
      double ymax{0};
    };

    auto plotOverlayAndSave = [&](TH1* h1, TH1* h2, PlotOptions opts = {}, std::string_view tag = "")
    {
      h1->Draw("HIST");
      h2->Draw("HIST same");

      h1->SetLineColor(kBlue);
      h1->SetLineWidth(3);
      h1->SetTitle(opts.title.c_str());
      h1->GetYaxis()->SetMaxDigits(3);
      h1->GetYaxis()->SetTitle("Events");
      h1->GetYaxis()->SetTitleOffset(1.F);

      if (opts.ymax)
      {
        h1->GetYaxis()->SetRangeUser(0, opts.ymax);
      }

      h2->SetLineColor(kRed);
      h2->SetLineWidth(3);

      double xshift = 0.4;
      double yshift = 0.19;

      std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .55 + yshift, 0.4 + xshift, .7 + yshift);
      leg->SetFillStyle(0);
      leg->SetTextSize(0.05f);
      leg->AddEntry(h1, "Raw", "l");
      leg->AddEntry(h2, "Corrected", "l");
      leg->Draw("same");

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, tag).c_str());
    };

    double ymax = std::max(hSEPD_Psi2_raw_S->GetMaximum(), hSEPD_Psi2_raw_N->GetMaximum()) * 1.1;

    plotOverlayAndSave(hSEPD_Psi2_raw_S, hSEPD_Psi2_S, {.title = "South", .ymax = ymax}, "hSEPD_Psi_South-overlay");
    plotOverlayAndSave(hSEPD_Psi2_raw_N, hSEPD_Psi2_N, {.title = "North", .ymax = ymax}, "hSEPD_Psi_North-overlay");
  }

  // ---------------------------------------------------

  {
    gPad->SetLogy();

    auto* hJetPt = m_hists["hJetPt"].get();

    hJetPt->Draw("HIST");

    hJetPt->SetLineColor(kBlue);
    hJetPt->SetLineWidth(3);
    hJetPt->GetXaxis()->SetRangeUser(0, 55);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hJetPt").c_str());

    gPad->SetLogy(0);
  }

  // ---------------------------------------------------

  {
    c1->SetRightMargin(.04F);
    // Data+MC
    auto* hRefFlow_squared_data_mc = dynamic_cast<TProfile*>(m_hists["hRefFlow_data_mc"].get());
    hRefFlow_squared_data_mc->Rebin(10);

    auto* hRefFlow_data_mc = hRefFlow_squared_data_mc->ProjectionX();
    auto* hScalarProduct_data_mc = dynamic_cast<TProfile*>(m_hists["hScalarProduct_data_mc"].get());
    hScalarProduct_data_mc->Rebin(10);

    auto* hJetV2_data_mc = hScalarProduct_data_mc->ProjectionX();

    // Data
    auto* hRefFlow_squared_data = dynamic_cast<TProfile*>(m_hists["hRefFlow_data"].get());
    hRefFlow_squared_data->Rebin(10);

    auto* hRefFlow_data = hRefFlow_squared_data->ProjectionX();
    auto* hScalarProduct_data = dynamic_cast<TProfile*>(m_hists["hScalarProduct_data"].get());
    hScalarProduct_data->Rebin(10);

    auto* hJetV2_data = hScalarProduct_data->ProjectionX();

    auto compute_vn_plots = [&](TH1* hRefFlow, TH1* hScalarProduct, TH1* hJetV2, int bin)
    {
      double val = hRefFlow->GetBinContent(bin);
      double sp = hScalarProduct->GetBinContent(bin);

      double error = hRefFlow->GetBinError(bin);
      double ref_flow_error_rel = error / val;

      double ref_flow = std::sqrt(val);
      double flow_error = error / (2 * ref_flow);
      double sp_error = hScalarProduct->GetBinError(bin);
      double sp_error_rel = sp_error / sp;

      double v2 = sp / ref_flow;
      double v2_error = std::abs(v2) * std::sqrt(sp_error_rel * sp_error_rel + ref_flow_error_rel * ref_flow_error_rel / 4.);

      hRefFlow->SetBinContent(bin, ref_flow);
      hRefFlow->SetBinError(bin, flow_error);

      hJetV2->SetBinContent(bin, v2);
      hJetV2->SetBinError(bin, v2_error);
    };

    for (int bin = 1; bin < hRefFlow_data_mc->GetNbinsX(); ++bin)
    {
      compute_vn_plots(hRefFlow_data_mc, hScalarProduct_data_mc, hJetV2_data_mc, bin);
      compute_vn_plots(hRefFlow_data, hScalarProduct_data, hJetV2_data, bin);
    }

    struct PlotOptions
    {
      std::string ytitle{};
      double ymax_frac{0};
      float yoffset{1.4F};
      int max_digits{3};
      double ymin{0};
      double ymax{0};
    };

    auto plotAndSave = [&](TH1* hist, PlotOptions opts = {}, std::string_view tag = "")
    {
      hist->Draw("pe");
      hist->SetLineColor(kBlue);
      hist->SetLineWidth(3);
      hist->SetMarkerStyle(kFullDotLarge);
      hist->SetMarkerColor(kBlue);
      hist->SetTitle("");

      hist->GetXaxis()->SetRangeUser(0, 59.5);

      if (!opts.ytitle.empty())
      {
        hist->GetYaxis()->SetTitle(opts.ytitle.c_str());
      }

      if (opts.max_digits)
      {
        hist->GetYaxis()->SetMaxDigits(opts.max_digits);
      }

      if (opts.ymax_frac)
      {
        double ymax = hist->GetMaximum() * opts.ymax_frac;
        hist->GetYaxis()->SetRangeUser(0, ymax);
      }

      if (opts.ymin < opts.ymax)
      {
        hist->GetYaxis()->SetRangeUser(opts.ymin, opts.ymax);
      }

      hist->GetYaxis()->SetTitleOffset(opts.yoffset);

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, tag).c_str());
    };

    plotAndSave(hRefFlow_squared_data_mc, {.ymax_frac = 1.1}, "hRefFlow_squared_data_mc");
    plotAndSave(hRefFlow_data_mc, {.ytitle = "#sqrt{Re(#LTQ^{S}_{2} Q^{N*}_{2}#GT)}", .ymax_frac = 1.05}, "hRefFlow_data_mc");
    plotAndSave(hScalarProduct_data_mc, {}, "hScalarProduct_data_mc");
    plotAndSave(hJetV2_data_mc, {.ytitle = "Jet v_{2} = Re(#LTq_{2} Q^{S|N*}_{2}#GT) / #sqrt{Re(#LTQ^{S}_{2} Q^{N*}_{2}#GT)}", .yoffset = 1.6F, .max_digits = 0}, "hJetV2_data_mc");
    plotAndSave(hJetV2_data_mc, {.ytitle = "Jet v_{2} = Re(#LTq_{2} Q^{S|N*}_{2}#GT) / #sqrt{Re(#LTQ^{S}_{2} Q^{N*}_{2}#GT)}", .yoffset = 1.6F, .max_digits = 0, .ymin = -0.1, .ymax = .18}, "hJetV2_data_mc-zoomout");

    auto plotOverlayAndSave = [&](TH1* h1, TH1* h2, PlotOptions opts = {}, std::string_view tag = "")
    {
      h1->Draw();
      h2->Draw("same");

      h1->SetLineColor(kBlue);
      h1->SetLineWidth(3);
      h1->SetMarkerStyle(kFullDotLarge);
      h1->SetMarkerColor(kBlue);
      h1->SetTitle("");

      h2->SetLineColor(kRed);
      h2->SetLineWidth(3);
      h2->SetMarkerStyle(kFullDotLarge);
      h2->SetMarkerColor(kRed);

      double xshift = -0.05;
      double yshift = 0.19;

      std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .55 + yshift, 0.4 + xshift, .7 + yshift);
      leg->SetFillStyle(0);
      leg->SetTextSize(0.05f);
      leg->AddEntry(h1, "sEPD: Data+MC", "pe");
      leg->AddEntry(h2, "sEPD: Data", "pe");
      leg->Draw("same");

      if (opts.max_digits)
      {
        h1->GetYaxis()->SetMaxDigits(opts.max_digits);
      }

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, tag).c_str());
    };

    plotOverlayAndSave(hRefFlow_squared_data_mc, hRefFlow_squared_data, {}, "hRefFlow_squared-overlay");
    plotOverlayAndSave(hRefFlow_data_mc, hRefFlow_data, {}, "hRefFlow-overlay");
    plotOverlayAndSave(hScalarProduct_data_mc, hScalarProduct_data, {}, "hScalarProduct-overlay");
    plotOverlayAndSave(hJetV2_data_mc, hJetV2_data, {}, "hJetV2-overlay");
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
    DisplayDataMC analysis(output_dir, saveFig);
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
