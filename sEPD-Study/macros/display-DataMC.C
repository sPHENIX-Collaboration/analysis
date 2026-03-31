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

  std::vector<std::string> m_tags = {"_jet10","_jet20"};

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

  std::string base_path = "/gpfs02/sphenix/user/anarde/sEPD-Study/MC";
  std::string input_jet10 = std::format("{}/jet10/54404.root", base_path);
  std::string input_jet20 = std::format("{}/jet20/54404.root", base_path);

  m_hists = myUtils::read_hists(input_jet10, m_tags[0]);
  m_hists.merge(myUtils::read_hists(input_jet20, m_tags[1]));
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

    auto* hEvent_jet10 = m_hists["hEvent"+m_tags[0]].get();
    auto* hEvent_jet20 = m_hists["hEvent"+m_tags[1]].get();

    auto plotAndSave = [&](TH1* hist, std::string_view name = "")
    {
      hist->Draw("HIST");

      hist->GetYaxis()->SetMaxDigits(3);
      hist->GetXaxis()->SetLabelSize(0.05F);
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

    plotAndSave(hEvent_jet10, "hEvent_jet10");
    plotAndSave(hEvent_jet20, "hEvent_jet20");

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

    plotAndSave("hCentrality"+m_tags[0]);
    plotAndSave("hCentralityZ50"+m_tags[0]);
    plotAndSave("hCentralityZOuter"+m_tags[0]);
  }

  // ---------------------------------------------------

  {
    c1->SetRightMargin(.14F);
    c1->SetTopMargin(.08F);
    auto* h2ZVertexCentrality = dynamic_cast<TH2*>(m_hists["h2ZVertexCentrality"+m_tags[0]].get());

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

    auto* h2ZVertexCentrality = dynamic_cast<TH2*>(m_hists["h2ZVertexCentrality"+m_tags[0]].get());
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
    auto* hZVertex = m_hists["hZVertex"+m_tags[0]].get();

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

    plotAndSave("hSEPD_Charge_DataMC"+m_tags[0]);
    plotAndSave("hSEPD_Charge_DataMC"+m_tags[0], {.xhigh = 5, .logy = false}, "-zoom");
    plotAndSave("hSEPD_Charge_MC"+m_tags[0]);
    plotAndSave("hSEPD_Charge_MC"+m_tags[0], {.xhigh = 5, .logy = false}, "-zoom");

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

    plotAndSave("h2SEPD_South_Charge_DataMC"+m_tags[0], "sEPD South: |z| < 10 cm and MB");
    plotAndSave("h2SEPD_BadMasked_South_Charge_DataMC"+m_tags[0], "sEPD South: |z| < 10 cm and MB");
    plotAndSave("h2SEPD_North_Charge_DataMC"+m_tags[0], "sEPD North: |z| < 10 cm and MB");
    plotAndSave("h2SEPD_BadMasked_North_Charge_DataMC"+m_tags[0], "sEPD North: |z| < 10 cm and MB");

    gPad->SetLogy(0);
  }

  // ---------------------------------------------------

  {
    c1->SetRightMargin(.14F);
    c1->SetTopMargin(.08F);

    auto* pSEPD_Charge_DataMC = m_hists["pSEPD_Charge_DataMC"+m_tags[0]].get();
    auto* pSEPD_BadMasked_Charge_DataMC = m_hists["pSEPD_BadMasked_Charge_DataMC"+m_tags[0]].get();
    auto* pSEPD_Charge_MC = m_hists["pSEPD_Charge_MC"+m_tags[0]].get();
    auto* pSEPD_BadMasked_Charge_MC = m_hists["pSEPD_BadMasked_Charge_MC"+m_tags[0]].get();

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

    auto* h2SEPD_totalcharge_centrality = dynamic_cast<TH2*>(m_hists["h2SEPD_totalcharge_centrality"+m_tags[0]].get());

    h2SEPD_totalcharge_centrality->Draw("COLZ1");

    h2SEPD_totalcharge_centrality->GetXaxis()->SetMaxDigits(3);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2SEPD_totalcharge_centrality"+m_tags[0]).c_str());

    gPad->SetLogz(0);
  }

  // ---------------------------------------------------

  {
    c1->SetRightMargin(.04F);

    auto* h2SEPD_Psi2_raw_N = dynamic_cast<TH2*>(m_hists["h2SEPD_Psi2_raw_data_mc_N"+m_tags[0]].get());
    auto* h2SEPD_Psi2_raw_S = dynamic_cast<TH2*>(m_hists["h2SEPD_Psi2_raw_data_mc_S"+m_tags[0]].get());

    auto* h2SEPD_Psi2_N = dynamic_cast<TH2*>(m_hists["h2SEPD_Psi2_data_mc_N"+m_tags[0]].get());
    auto* h2SEPD_Psi2_S = dynamic_cast<TH2*>(m_hists["h2SEPD_Psi2_data_mc_S"+m_tags[0]].get());

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

    auto* hJetPt_jet10 = m_hists["hJetPt" + m_tags[0]].get();
    auto* hJetPt_jet20 = m_hists["hJetPt" + m_tags[1]].get();

    hJetPt_jet10->Draw("HIST");
    hJetPt_jet20->Draw("same HIST");

    hJetPt_jet10->SetLineColor(kBlue);
    hJetPt_jet10->SetLineWidth(3);

    hJetPt_jet20->SetLineColor(kRed);
    hJetPt_jet20->SetLineWidth(3);

    hJetPt_jet10->GetXaxis()->SetRangeUser(0, 70);

    double xshift = 0.55;
    double yshift = 0.19;

    std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .55 + yshift, 0.4 + xshift, .7 + yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.07f);
    leg->AddEntry(hJetPt_jet10, "Jet10", "l");
    leg->AddEntry(hJetPt_jet20, "Jet20", "l");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hJetPt-overlay").c_str());

    double cross_section_jet10 = 3.977e6; /*pb*/
    double cross_section_jet20 = 6.2623e4; /*pb*/

    auto* hEvent_jet10 = m_hists["hEvent" + m_tags[0]].get();
    auto* hEvent_jet20 = m_hists["hEvent" + m_tags[1]].get();

    double events_jet10 = hEvent_jet10->GetBinContent(1);
    double events_jet20 = hEvent_jet20->GetBinContent(1);

    double weight_jet10 = cross_section_jet10 / events_jet10;
    double weight_jet20 = cross_section_jet20 / events_jet20;

    std::cout << std::format("weight jet10: {:.2e}, jet20: {:.2e}\n", weight_jet10, weight_jet20);

    hJetPt_jet10->Scale(weight_jet10);
    hJetPt_jet20->Scale(weight_jet20);

    hJetPt_jet10->GetYaxis()->SetTitle("d#sigma/dp_{T} [pb/GeV]");
    hJetPt_jet10->GetYaxis()->SetRangeUser(1e-3, 2e5);

    std::unique_ptr<TLatex> t = std::make_unique<TLatex>();
    t->SetNDC();          // Use Normalized Device Coordinates (0 to 1)
    t->SetTextAlign(22);  // Center alignment (horizontal and vertical)
    t->SetTextSize(0.04F);

    double txshift = 0.22;
    double tyshift = 0.2;

    t->DrawLatex(0.5+txshift, 0.5+tyshift, std::format("Jet10 #sigma_{{pythia}}: {:.3e} pb", cross_section_jet10).c_str());
    t->DrawLatex(0.5+txshift, 0.4+tyshift, std::format("Jet20 #sigma_{{pythia}}: {:.4e} pb", cross_section_jet20).c_str());

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hJetPt-overlay-scaled").c_str());

    gPad->SetLogy(0);
  }

  // ---------------------------------------------------

  {
    gPad->SetLogz();
    c1->SetRightMargin(.12F);

    auto* h2RefFlow_data = dynamic_cast<TH2*>(m_hists["h2RefFlow_data" + m_tags[0]].get());
    auto* h2RefFlow_data_mc = dynamic_cast<TH2*>(m_hists["h2RefFlow_data_mc" + m_tags[0]].get());

    auto* h2ScalarProduct_data = dynamic_cast<TH2*>(m_hists["h2ScalarProduct_data" + m_tags[0]].get());
    auto* h2ScalarProduct_data_mc = dynamic_cast<TH2*>(m_hists["h2ScalarProduct_data_mc" + m_tags[0]].get());

    auto* h2ScalarProduct_data_jet20 = dynamic_cast<TH2*>(m_hists["h2ScalarProduct_data" + m_tags[1]].get());
    auto* h2ScalarProduct_data_mc_jet20 = dynamic_cast<TH2*>(m_hists["h2ScalarProduct_data_mc" + m_tags[1]].get());

    struct PlotOptions
    {
      std::string title{};
      double ylow{0};
      double yhigh{0};
      double xlow{-0.5};
      double xhigh{59.5};
    };

    auto plotAndSave = [&](TH2* hist, PlotOptions opts = {}, std::string_view tag = "")
    {
      auto* px = hist->ProfileX();

      hist->Draw("COLZ1");

      if (!opts.title.empty())
      {
        hist->SetTitle(opts.title.c_str());
      }

      hist->GetYaxis()->SetMaxDigits(3);
      hist->GetXaxis()->SetRangeUser(opts.xlow, opts.xhigh);
      hist->GetYaxis()->SetTitleOffset(1.4F);

      hist->SetMinimum(1);

      px->Draw("same");

      px->SetLineColor(kRed);
      px->SetMarkerColor(kRed);
      px->SetMarkerStyle(kFullDotLarge);
      px->SetLineWidth(3);

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, tag).c_str());

      if (opts.yhigh > opts.ylow)
      {
        hist->GetYaxis()->SetRangeUser(opts.ylow, opts.yhigh);

        c1->Print(output.c_str(), "pdf portrait");
        if (m_saveFig) c1->Print(std::format("{}/images/{}-zoom.png", m_output_dir, tag).c_str());
      }

      // Reset Hist Range
      hist->GetYaxis()->SetRange(0, 0);
    };

    plotAndSave(h2RefFlow_data, {.title = "sEPD Data", .yhigh = 3.5e-4}, "h2RefFlow_data");
    plotAndSave(h2RefFlow_data_mc, {.title = "sEPD Data+MC", .yhigh = 3.5e-4}, "h2RefFlow_data_mc");

    int pt_bins = 5;
    int pt_low = 10;
    int pt_width = 5;

    for (int pt_bin = 0; pt_bin < pt_bins; ++pt_bin)
    {
      int low = pt_low + pt_width * pt_bin;
      int high = low + pt_width;

      std::string tag = (low < 20) ? "jet10" : "jet20";

      std::string name_data = std::format("h2ScalarProduct_data_pt_{}_{}_{}", low, high, tag);
      std::string name_data_mc = std::format("h2ScalarProduct_data_mc_pt_{}_{}_{}", low, high, tag);

      auto* h2ScalarProduct_data_pt = dynamic_cast<TH2*>(m_hists[name_data].get());
      auto* h2ScalarProduct_data_mc_pt = dynamic_cast<TH2*>(m_hists[name_data_mc].get());

      std::string title_data = std::format("sEPD Data, p_{{T}} = {}-{} GeV", low, high);
      std::string title_data_mc = std::format("sEPD Data+MC, p_{{T}} = {}-{} GeV", low, high);

      double ylow = -1e-3;
      double yhigh = 1e-3;

      plotAndSave(h2ScalarProduct_data_pt, {.title = title_data, .ylow = ylow, .yhigh = yhigh}, name_data);
      plotAndSave(h2ScalarProduct_data_mc_pt, {.title = title_data_mc, .ylow = ylow, .yhigh = yhigh}, name_data_mc);
    }

    plotAndSave(h2ScalarProduct_data, {.title = "Jet10, sEPD Data", .ylow = -4e-4, .yhigh = 3e-4}, "h2ScalarProduct_data-jet10");
    plotAndSave(h2ScalarProduct_data_mc, {.title = "Jet10, sEPD Data+MC", .ylow = -4e-4, .yhigh = 3e-4}, "h2ScalarProduct_data_mc-jet10");

    plotAndSave(h2ScalarProduct_data_jet20, {.title = "Jet20, sEPD Data", .ylow = -4e-4, .yhigh = 3e-4}, "h2ScalarProduct_data-jet20");
    plotAndSave(h2ScalarProduct_data_mc_jet20, {.title = "Jet20, sEPD Data+MC", .ylow = -4e-4, .yhigh = 3e-4}, "h2ScalarProduct_data_mc-jet20");

    c1->SetRightMargin(.04F);
  }

  // ---------------------------------------------------

  {
    c1->SetLeftMargin(.14F);
    c1->SetRightMargin(.02F);

    auto* h2RefFlow_data = dynamic_cast<TH2*>(m_hists["h2RefFlow_data" + m_tags[0]].get());
    auto* h2RefFlow_data_mc = dynamic_cast<TH2*>(m_hists["h2RefFlow_data_mc" + m_tags[0]].get());

    auto* h2ScalarProduct_data = dynamic_cast<TH2*>(m_hists["h2ScalarProduct_data" + m_tags[0]].get());
    auto* h2ScalarProduct_data_mc = dynamic_cast<TH2*>(m_hists["h2ScalarProduct_data_mc" + m_tags[0]].get());

    auto* h2ScalarProduct_data_jet20 = dynamic_cast<TH2*>(m_hists["h2ScalarProduct_data" + m_tags[1]].get());
    auto* h2ScalarProduct_data_mc_jet20 = dynamic_cast<TH2*>(m_hists["h2ScalarProduct_data_mc" + m_tags[1]].get());

    auto* RefFlow_px_data = h2RefFlow_data->ProfileX();
    auto* RefFlow_px_data_mc = h2RefFlow_data_mc->ProfileX();

    auto* ScalarProduct_px_data = h2ScalarProduct_data->ProfileX();
    auto* ScalarProduct_px_data_mc = h2ScalarProduct_data_mc->ProfileX();

    auto* ScalarProduct_px_data_jet20 = h2ScalarProduct_data_jet20->ProfileX();
    auto* ScalarProduct_px_data_mc_jet20 = h2ScalarProduct_data_mc_jet20->ProfileX();

    struct PlotOptions
    {
      std::string ytitle;
      std::string title{};
      double ylow{0};
      double yhigh{0};
      float yoffset{1.2F};
    };

    auto plotAndSave = [&](TH1* px_data, TH1* px_data_mc, PlotOptions opts = {}, std::string_view tag = "")
    {
      px_data->Draw();
      px_data_mc->Draw("same");

      px_data->SetLineColor(kBlue);
      px_data_mc->SetLineColor(kRed);

      px_data->SetMarkerColor(kBlue);
      px_data_mc->SetMarkerColor(kRed);

      px_data->SetMarkerStyle(kFullDotLarge);
      px_data_mc->SetMarkerStyle(kFullDotLarge);

      px_data->SetLineWidth(3);
      px_data_mc->SetLineWidth(3);

      px_data->GetYaxis()->SetMaxDigits(3);

      std::string ytitle = std::format("#LT{}#GT", opts.ytitle);

      px_data->GetYaxis()->SetTitle(ytitle.c_str());
      px_data->GetYaxis()->SetTitleOffset(opts.yoffset);

      px_data->SetTitle(opts.title.c_str());

      double ymax = px_data->GetMaximum() * 1.1;
      px_data->GetYaxis()->SetRangeUser(0, ymax);

      if (opts.yhigh > opts.ylow)
      {
        px_data->GetYaxis()->SetRangeUser(opts.ylow, opts.yhigh);
      }

      double xshift = -0.05;
      double yshift = 0.19;

      std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .55 + yshift, 0.4 + xshift, .7 + yshift);
      leg->SetFillStyle(0);
      leg->SetTextSize(0.05f);
      leg->AddEntry(px_data, "sEPD: Data", "pe");
      leg->AddEntry(px_data_mc, "sEPD: Data+MC", "pe");
      leg->Draw("same");

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, tag).c_str());
    };

    std::string ref_title = h2RefFlow_data->GetYaxis()->GetTitle();
    std::string SP_title = h2ScalarProduct_data->GetYaxis()->GetTitle();

    plotAndSave(RefFlow_px_data, RefFlow_px_data_mc, {.ytitle = ref_title, .yoffset = 1.4F}, "hRefFlow-overlay");

    plotAndSave(ScalarProduct_px_data, ScalarProduct_px_data_mc,
                {.ytitle = SP_title, .title = "Jet10", .ylow = -4e-4, .yhigh = 4e-4}, "hScalarProduct-jet10-overlay");

    plotAndSave(ScalarProduct_px_data_jet20, ScalarProduct_px_data_mc_jet20,
                {.ytitle = SP_title, .title = "Jet20", .ylow = -4e-4, .yhigh = 4e-4}, "hScalarProduct-jet20-overlay");

    int pt_bins = 5;
    int pt_low = 10;
    int pt_width = 5;

    for (int pt_bin = 0; pt_bin < pt_bins; ++pt_bin)
    {
      int low = pt_low + pt_width * pt_bin;
      int high = low + pt_width;

      std::string tag = (low < 20) ? "jet10" : "jet20";

      std::string name_data = std::format("h2ScalarProduct_data_pt_{}_{}_{}", low, high, tag);
      std::string name_data_mc = std::format("h2ScalarProduct_data_mc_pt_{}_{}_{}", low, high, tag);

      auto* hScalarProduct_data_pt = dynamic_cast<TH2*>(m_hists[name_data].get())->ProfileX();
      auto* hScalarProduct_data_mc_pt = dynamic_cast<TH2*>(m_hists[name_data_mc].get())->ProfileX();

      std::string title = std::format("p_{{T}} = {}-{} GeV", low, high);

      double ylow = -1e-3;
      double yhigh = 1e-3;

      std::string name_output = std::format("hScalarProduct_pt_{}_{}-overlay", low, high);

      plotAndSave(hScalarProduct_data_pt, hScalarProduct_data_mc_pt,
                  {.ytitle = SP_title, .title = title, .ylow = ylow, .yhigh = yhigh}, name_output);
    }
  }

  // ---------------------------------------------------

  {
    auto* h2RefFlow_data = dynamic_cast<TH2*>(m_hists["h2RefFlow_data" + m_tags[0]].get());
    auto* h2RefFlow_data_mc = dynamic_cast<TH2*>(m_hists["h2RefFlow_data_mc" + m_tags[0]].get());

    h2RefFlow_data->SetTitle("");

    auto* px_data = h2RefFlow_data->ProfileX()->ProjectionX();
    auto* px_data_mc = h2RefFlow_data_mc->ProfileX()->ProjectionX();

    m_hists["hRefFlow_data_sqrt_jet10"] = std::unique_ptr<TH1>(px_data);

    for (int cent_bin = 1; cent_bin <= 6; ++cent_bin)
    {
      double val_data = px_data->GetBinContent(cent_bin);
      double val_data_mc = px_data_mc->GetBinContent(cent_bin);

      double error_data = px_data->GetBinError(cent_bin);
      double error_data_mc = px_data_mc->GetBinError(cent_bin);

      double ref_flow_data = std::sqrt(val_data);
      double ref_flow_data_mc = std::sqrt(val_data_mc);

      double ref_flow_error_data = error_data / (2 * ref_flow_data);
      double ref_flow_error_data_mc = error_data_mc / (2 * ref_flow_data_mc);

      px_data->SetBinContent(cent_bin, ref_flow_data);
      px_data->SetBinError(cent_bin, ref_flow_error_data);

      px_data_mc->SetBinContent(cent_bin, ref_flow_data_mc);
      px_data_mc->SetBinError(cent_bin, ref_flow_error_data_mc);
    }

    px_data->Draw();
    px_data_mc->Draw("same");

    px_data->SetLineColor(kBlue);
    px_data_mc->SetLineColor(kRed);

    px_data->SetMarkerColor(kBlue);
    px_data_mc->SetMarkerColor(kRed);

    px_data->SetMarkerStyle(kFullDotLarge);
    px_data_mc->SetMarkerStyle(kFullDotLarge);

    px_data->SetLineWidth(3);
    px_data_mc->SetLineWidth(3);

    px_data->GetYaxis()->SetMaxDigits(3);

    std::string title = std::format("#sqrt{{#LT{}#GT}}", h2RefFlow_data->GetYaxis()->GetTitle());
    px_data->GetYaxis()->SetTitle(title.c_str());
    px_data->GetYaxis()->SetTitleOffset(1.1F);

    double ymax = px_data->GetMaximum()*1.1;
    px_data->GetYaxis()->SetRangeUser(0, ymax);
    
    double xshift = 0.4;
    double yshift = -0.1;

    std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .55 + yshift, 0.4 + xshift, .7 + yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.05f);
    leg->AddEntry(px_data, "sEPD: Data", "pe");
    leg->AddEntry(px_data_mc, "sEPD: Data+MC", "pe");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hRefFlow-sqrt-overlay").c_str());
  }

  // ---------------------------------------------------

  // Vn
  {
    auto* hRefFlow_data = m_hists["hRefFlow_data_sqrt_jet10"].get();

    auto* h2ScalarProduct_data = dynamic_cast<TH2*>(m_hists["h2ScalarProduct_data" + m_tags[0]].get());
    auto* h2ScalarProduct_data_mc = dynamic_cast<TH2*>(m_hists["h2ScalarProduct_data_mc" + m_tags[0]].get());

    auto* h2ScalarProduct_data_jet20 = dynamic_cast<TH2*>(m_hists["h2ScalarProduct_data" + m_tags[1]].get());
    auto* h2ScalarProduct_data_mc_jet20 = dynamic_cast<TH2*>(m_hists["h2ScalarProduct_data_mc" + m_tags[1]].get());

    auto* ScalarProduct_px_data = h2ScalarProduct_data->ProfileX()->ProjectionX();
    auto* ScalarProduct_px_data_mc = h2ScalarProduct_data_mc->ProfileX()->ProjectionX();

    auto* ScalarProduct_px_data_jet20 = h2ScalarProduct_data_jet20->ProfileX()->ProjectionX();
    auto* ScalarProduct_px_data_mc_jet20 = h2ScalarProduct_data_mc_jet20->ProfileX()->ProjectionX();

    ScalarProduct_px_data->Divide(hRefFlow_data);
    ScalarProduct_px_data_mc->Divide(hRefFlow_data);

    ScalarProduct_px_data_jet20->Divide(hRefFlow_data);
    ScalarProduct_px_data_mc_jet20->Divide(hRefFlow_data);

    struct PlotOptions
    {
      std::string title{};
      double ylow{-3e-2};
      double yhigh{3e-2};
      float yoffset{1.4F};
    };

    auto plotAndSave = [&](TH1* px_data, TH1* px_data_mc, PlotOptions opts = {}, std::string_view tag = "")
    {
      px_data->Draw();
      px_data_mc->Draw("same");

      px_data->SetLineColor(kBlue);
      px_data_mc->SetLineColor(kRed);

      px_data->SetMarkerColor(kBlue);
      px_data_mc->SetMarkerColor(kRed);

      px_data->SetMarkerStyle(kFullDotLarge);
      px_data_mc->SetMarkerStyle(kFullDotLarge);

      px_data->SetLineWidth(3);
      px_data_mc->SetLineWidth(3);

      px_data->GetYaxis()->SetTitle("Jet v_{2}");
      px_data->GetYaxis()->SetTitleOffset(opts.yoffset);

      px_data->SetTitle(opts.title.c_str());

      double ymax = px_data->GetMaximum() * 1.1;
      px_data->GetYaxis()->SetRangeUser(0, ymax);

      if (opts.yhigh > opts.ylow)
      {
        px_data->GetYaxis()->SetRangeUser(opts.ylow, opts.yhigh);
      }

      double xshift = -0.05;
      double yshift = 0.19;

      std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .55 + yshift, 0.4 + xshift, .7 + yshift);
      leg->SetFillStyle(0);
      leg->SetTextSize(0.05f);
      leg->AddEntry(px_data, "sEPD: Data", "pe");
      leg->AddEntry(px_data_mc, "sEPD: Data+MC", "pe");
      leg->Draw("same");

      std::unique_ptr<TF1> f1 = std::make_unique<TF1>("f1", "0", px_data->GetXaxis()->GetXmin(), px_data->GetXaxis()->GetXmax());
      f1->SetLineColor(kBlack);
      f1->SetLineStyle(kDashed);
      f1->SetLineWidth(3);
      f1->Draw("same");

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, tag).c_str());
    };

    plotAndSave(ScalarProduct_px_data, ScalarProduct_px_data_mc, {.title = "Jet10"}, "jet-v2-jet10-overlay");

    plotAndSave(ScalarProduct_px_data_jet20, ScalarProduct_px_data_mc_jet20, {.title = "Jet20"}, "jet-v2-jet20-overlay");

    int pt_bins = 5;
    int pt_low = 10;
    int pt_width = 5;

    for (int pt_bin = 0; pt_bin < pt_bins; ++pt_bin)
    {
      int low = pt_low + pt_width * pt_bin;
      int high = low + pt_width;

      std::string tag = (low < 20) ? "jet10" : "jet20";

      std::string name_data = std::format("h2ScalarProduct_data_pt_{}_{}_{}", low, high, tag);
      std::string name_data_mc = std::format("h2ScalarProduct_data_mc_pt_{}_{}_{}", low, high, tag);

      auto* hScalarProduct_data_pt = dynamic_cast<TH2*>(m_hists[name_data].get())->ProfileX()->ProjectionX();
      auto* hScalarProduct_data_mc_pt = dynamic_cast<TH2*>(m_hists[name_data_mc].get())->ProfileX()->ProjectionX();

      hScalarProduct_data_pt->Divide(hRefFlow_data);
      hScalarProduct_data_mc_pt->Divide(hRefFlow_data);

      std::string title = std::format("p_{{T}} = {}-{} GeV", low, high);

      std::string name_output = std::format("jet-v2-pt_{}_{}-overlay", low, high);

      plotAndSave(hScalarProduct_data_pt, hScalarProduct_data_mc_pt, {.title = title, .yhigh = 0.07}, name_output);
    }
  }

  // ---------------------------------------------------

  {
    c1->Clear();
    c1->SetCanvasSize(2400, 1400);
    c1->Divide(3, 2);

    auto* h2RefFlow_data = dynamic_cast<TH2*>(m_hists["h2RefFlow_data" + m_tags[0]].get());
    auto* h2RefFlow_data_px = h2RefFlow_data->ProfileX();

    auto* h2ScalarProduct_data = dynamic_cast<TH2*>(m_hists["h2ScalarProduct_data" + m_tags[0]].get());
    h2ScalarProduct_data->SetTitle("");

    auto* h2ScalarProduct_data_px = h2ScalarProduct_data->ProfileX();

    auto* h2ScalarProduct_data_mc = dynamic_cast<TH2*>(m_hists["h2ScalarProduct_data_mc" + m_tags[0]].get());
    h2ScalarProduct_data_mc->SetTitle("");

    auto* h2ScalarProduct_data_mc_px = h2ScalarProduct_data_mc->ProfileX();

    std::unique_ptr<TLatex> t = std::make_unique<TLatex>();
    t->SetNDC();          // Use Normalized Device Coordinates (0 to 1)
    t->SetTextAlign(22);  // Center alignment (horizontal and vertical)
    t->SetTextSize(0.05F);

    struct PlotOptions
    {
      double xlow{0};
      double xhigh{0};
      int rebin{8};
    };

    auto plotAndSave = [&](TH2* hist, TH1* px, PlotOptions opts = {}, std::string_view tag = "")
    {
      for (int cent_bin = 1; cent_bin <= 6; ++cent_bin)
      {
        c1->cd(cent_bin);
        gPad->SetTopMargin(0.05F);
        gPad->SetBottomMargin(0.11F);
        gPad->SetRightMargin(0.08F);
        gPad->SetLeftMargin(0.1F);

        std::string name = std::format("hist_{}_{}", tag, cent_bin - 1);
        auto* hist_proj = hist->ProjectionY(name.c_str(), cent_bin, cent_bin);
        hist_proj->Rebin(opts.rebin);

        // hist_proj->Draw("HIST E");
        hist_proj->Draw();
        hist_proj->SetLineColor(kBlue);
        hist_proj->SetMarkerColor(kBlue);
        hist_proj->SetMarkerStyle(kFullDotLarge);
        hist_proj->GetXaxis()->SetMaxDigits(3);
        hist_proj->GetYaxis()->SetMaxDigits(3);

        hist_proj->GetYaxis()->SetTitle("Events");
        hist_proj->GetYaxis()->SetTitleOffset(1.F);

        if (opts.xhigh > opts.xlow)
        {
          hist_proj->GetXaxis()->SetRangeUser(opts.xlow, opts.xhigh);
        }

        double max = hist_proj->GetMaximum()*1.1;
        hist_proj->GetYaxis()->SetRangeUser(0, max);

        gPad->Update();  // Ensure the pad limits are calculated

        // Get the y-axis limits automatically
        double ymin = gPad->GetUymin();
        double ymax = gPad->GetUymax();

        double mean = px->GetBinContent(cent_bin);

        // Create the vertical line at x = 1
        TLine* line = new TLine(mean, ymin, mean, ymax);

        line->SetLineColor(kRed);
        line->SetLineStyle(2);  // Dashed line
        line->SetLineWidth(2);

        line->Draw();

        t->DrawLatex(0.5, 0.975, std::format("Centrality: {}-{}%", (cent_bin-1)*10, cent_bin*10).c_str()); 
      }

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, tag).c_str());
    };

    plotAndSave(h2RefFlow_data, h2RefFlow_data_px, {}, "h2RefFlow_data-individual");
    plotAndSave(h2RefFlow_data, h2RefFlow_data_px, {.xlow = -1e-3, .xhigh = 1e-3}, "h2RefFlow_data-individual-zoom");

    plotAndSave(h2ScalarProduct_data, h2ScalarProduct_data_px, {}, "h2ScalarProduct_data-individual");
    plotAndSave(h2ScalarProduct_data, h2ScalarProduct_data_px, {.xlow = -1e-3, .xhigh = 1e-3}, "h2ScalarProduct_data-individual-zoom");

    plotAndSave(h2ScalarProduct_data_mc, h2ScalarProduct_data_mc_px, {}, "h2ScalarProduct_data_mc-individual");
    plotAndSave(h2ScalarProduct_data_mc, h2ScalarProduct_data_mc_px, {.xlow = -1e-3, .xhigh = 1e-3}, "h2ScalarProduct_data_mc-individual-zoom");
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
