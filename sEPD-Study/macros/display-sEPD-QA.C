// -- utils
#include "myUtils.C"

// ====================================================================
// sPHENIX Includes
// ====================================================================
#include <sPhenixStyle.C>
#include <calobase/TowerInfoDefs.h>
#include <cdbobjects/CDBTTree.h>

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
#include <unordered_map>
#include <algorithm>
#include <iterator> // for std::inserter

// ====================================================================
struct RGB {
    uint8_t r, g, b;
};

namespace Colors {
    inline constexpr RGB Red{255, 0, 0};
    inline constexpr RGB Green{0, 255, 0};
    inline constexpr RGB Blue{0, 0, 255};
    inline constexpr RGB Yellow{255, 255, 0};
    inline constexpr RGB Purple{128, 0, 128};
    inline constexpr RGB Cyan{0, 255, 255};
    inline constexpr RGB White{255, 255, 255};
    inline constexpr RGB Orange{255, 165, 0};
}

// ====================================================================
// The Analysis Class
// ====================================================================
class DisplaySEPDQA
{
 public:
  // The constructor takes the configuration
  DisplaySEPDQA(std::string output_dir, int verbosity, bool saveFig)
    : m_output_dir(std::move(output_dir))
    , m_verbosity(verbosity)
    , m_saveFig(saveFig)
  {
  }

  void run()
  {
    read_hists();
    draw_QA();
    draw_EP_Study();
  }

 private:
  // Configuration stored as members
  std::string m_output_dir;
  std::map<std::string, std::unique_ptr<TH1>> m_hists;
  std::unique_ptr<CDBTTree> m_sepd_calib;
  std::string m_calib_field{"sepd_calib"};

  int m_verbosity{0};
  bool m_saveFig{false};

  std::unordered_set<std::string> m_names;

  int m_nChannels{744};

  std::map<std::string, std::string> m_EP_files = {
      {"_default", "/gpfs/mnt/gpfs02/sphenix/user/anarde/sEPD-Calib/2026-03-16-run3auau-test/stage-QVecCalib-default-hotmap/ApplyFlattening/output/hist/QVecCalib-68144.root"},
      {"_0", "/gpfs/mnt/gpfs02/sphenix/user/anarde/sEPD-Calib/2026-03-16-run3auau-test/stage-QVecCalib-0.0-0.0/ApplyFlattening/output/hist/QVecCalib-68144.root"},
      {"_50", "/gpfs/mnt/gpfs02/sphenix/user/anarde/sEPD-Calib/2026-03-16-run3auau-test/stage-QVecCalib-50.0-0.0/ApplyFlattening/output/hist/QVecCalib-68144.root"},
      {"_10", "/gpfs/mnt/gpfs02/sphenix/user/anarde/sEPD-Calib/2026-03-16-run3auau-test/stage-QVecCalib-10.0-0.0/ApplyFlattening/output/hist/QVecCalib-68144.root"},
      {"_5", "/gpfs/mnt/gpfs02/sphenix/user/anarde/sEPD-Calib/2026-03-16-run3auau-test/stage-QVecCalib-5.0-0.0/ApplyFlattening/output/hist/QVecCalib-68144.root"}
  };

  // --- Private Helper Methods ---
  void read_hists();
  void init_hists();
  void draw_QA();
  void draw_EP_Study();

  template <typename... Args>
  void print_color(RGB color, std::string_view fmt, Args&&... args)
  {
    // The sequence for TrueColor foreground is: \033[38;2;<r>;<g>;<b>m
    std::string start_code = std::format("\033[38;2;{};{};{}m", color.r, color.g, color.b);
    std::string end_code = "\033[0m";

    // Format the message
    std::string message = std::vformat(fmt, std::make_format_args(args...));

    std::cout << start_code << message << end_code << std::endl;
  }
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void DisplaySEPDQA::read_hists()
{
  std::string input = "/gpfs02/sphenix/user/anarde/sEPD-Study/run3auau/merged/output/68144.root";
  std::string calib = "/cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/SEPD_NMIP_CALIB/86/6d/866db5abdca24d60cc47f8856c8e97f4_sEPD_CalibConstants_CDB_Run71504_v2.root";
  m_hists = myUtils::read_hists(input);
  m_sepd_calib = std::make_unique<CDBTTree>(calib);

  // Event Plane Resolution Study
  std::unordered_set<std::string> names = {"hEP_res_2", "h2_sEPD_Psi_S_2_corr2", "h2_sEPD_Psi_N_2_corr2"};

  for (const auto& [tag, file] : m_EP_files)
  {
    m_hists.merge(myUtils::read_hists(file, tag, &names));
  }
}

void DisplaySEPDQA::draw_EP_Study()
{
  // set sPHENIX plotting style
  SetsPhenixStyle();

  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>("c1");
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(1200, 1000);
  c1->SetLeftMargin(.1F);
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
  output = std::format("{}/pdf/plots-EP-Study.pdf", m_output_dir);

  c1->Print((output + "[").c_str(), "pdf portrait");

  // ---------------------------------------------------

  std::vector<short int> my_colors = {kBlack, kRed + 1, kAzure + 1, kGreen + 2, kMagenta + 1};
  std::vector<std::string> legend_names = {"Current [Tower Mask]",
                                           "0#leq N_{mip}",
                                           "0#leq N_{mip}#leq 50",
                                           "0#leq N_{mip}#leq 10",
                                           "0#leq N_{mip}#leq 5"};

  std::vector<std::string> tags = {"_default", "_0", "_50", "_10", "_5"};

  {
    c1->SetLeftMargin(.12F);

    size_t plot_idx = 0;

    double xshift = 0.05;
    double yshift = -0.25;

    std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .4 + yshift, 0.5 + xshift, .7 + yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.05F);

    for (const auto& tag : tags)
    {
      std::cout << std::format("Processing: {}\n", tag);

      std::string name = std::format("hEP_res_2{}", tag);
      std::string EP_res_name = std::format("hEP_res_sqrt_2{}", tag);

      auto* hProf = dynamic_cast<TProfile*>(m_hists[name].get());
      hProf->Rebin(10);

      auto* hist = hProf->ProjectionX(EP_res_name.c_str());

      std::cout << std::format("Hist: {}\n", hist->GetName());

      for(int bin = 1; bin <= hist->GetNbinsX(); ++bin)
      {
        double val = hist->GetBinContent(bin);
        double error = hist->GetBinError(bin);

        if(val <= 0) continue;

        double val_new = std::sqrt(2*val);
        double error_new = error / val_new;

        hist->SetBinContent(bin, val_new);
        hist->SetBinError(bin, error_new);
      }

      m_hists[EP_res_name] = std::unique_ptr<TH1>(hist);

      std::string name_ratio = std::format("hEP_res_sqrt_2_ratio{}",tag);
      auto* hist_ratio = dynamic_cast<TH1*>(hist->Clone(name_ratio.c_str()));

      auto* hDefault = m_hists["hEP_res_sqrt_2_default"].get();

      hist_ratio->Divide(hDefault);
      m_hists[name_ratio] = std::unique_ptr<TH1>(hist_ratio);

      if (plot_idx == 0)
      {
        hist->Draw("hist l p");
        hist->Draw("same p e X0");
        hist->SetTitle("Event Plane Resolution");
        // hist->GetYaxis()->SetTitle("#sqrt{2#LTcos(2(#Psi^{N}_{2}-#Psi^{S}_{2}))#GT} = #sqrt{2#LTRe(Q^{S}_{2} Q^{N*}_{2}) / (|Q^{S}_{2}||Q^{N}_{2}|)#GT}");
        hist->GetYaxis()->SetTitle("#sqrt{2#LTRe(Q^{S}_{2} Q^{N*}_{2}) / (|Q^{S}_{2}||Q^{N}_{2}|)#GT}");
      }
      else
      {
        hist->Draw("same hist l p");
        hist->Draw("same p e X0");
      }

      short int color = my_colors[plot_idx];

      hist->SetLineColor(color);
      hist->SetMarkerColor(color);
      hist->SetLineWidth(3);

      leg->AddEntry(hist, legend_names[plot_idx].c_str(), "lpe");

      ++plot_idx;
    }

    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig)
    {
      c1->Print(std::format("{}/images/{}.png", m_output_dir, "EP-res-overlay").c_str());
    }
  }

  // ---------------------------------------------------

  // Ratio Plots
  {
    c1->SetTopMargin(.02F);

    double xshift = 0.37;
    double yshift = -0.25;

    std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .4 + yshift, 0.5 + xshift, .7 + yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.05F);

    size_t plot_idx = 0;

    for (const auto& tag : tags)
    {
      if (plot_idx == 0)
      {
        ++plot_idx;
        continue;
      }

      std::string name_ratio = std::format("hEP_res_sqrt_2_ratio{}",tag);
      auto* hist = m_hists[name_ratio].get();

      if(plot_idx == 1)
      {
         hist->Draw();
         hist->GetYaxis()->SetTitle("Ratio (With Current [Tower Mask])");
      }
      else
      {
         hist->Draw("same");
      }

      short int color = my_colors[plot_idx];

      hist->SetLineColor(color);
      hist->SetMarkerColor(color);
      hist->SetLineWidth(3);

      leg->AddEntry(hist, legend_names[plot_idx].c_str(), "lpe");

      ++plot_idx;
    }

    leg->Draw("same");

    std::unique_ptr<TF1> f1 = std::make_unique<TF1>("f1", "1", -0.5, 79.5);
    f1->Draw("same");

    f1->SetLineColor(kBlack);
    f1->SetLineWidth(3);
    f1->SetLineStyle(kDashed);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig)
    {
      c1->Print(std::format("{}/images/{}.png", m_output_dir, "EP-res-ratio-overlay").c_str());
    }

    c1->SetTopMargin(.11F);
  }

  // ---------------------------------------------------

  // Psi2 Plots
  {

    struct PlotOptions
    {
      std::string title{}; // NOLINT(readability-redundant-member-init)
      double ylow{0};
      double yhigh{0};
    };

    auto plotAndSave = [&](std::string_view hist_name, PlotOptions opts = {}, std::string_view file_name = "")
    {
      size_t plot_idx = 0;

      double xshift = 0.3;
      double yshift = -0.3;

      std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .4 + yshift, 0.5 + xshift, .7 + yshift);
      leg->SetFillStyle(0);
      leg->SetTextSize(0.05F);

      for (const auto& tag : tags)
      {
        std::string name = std::format("{}{}", hist_name, tag);

        auto* hist = dynamic_cast<TH2*>(m_hists[name].get())->ProjectionY();

        hist->Scale(1. / hist->Integral());

        if (plot_idx == 0)
        {
          hist->Draw("HIST");
          hist->SetTitle(opts.title.c_str());
          hist->GetYaxis()->SetMaxDigits(3);
          hist->GetYaxis()->SetTitle("Normalized Events");
          hist->GetXaxis()->SetTitleOffset(1.F);
          hist->GetYaxis()->SetTitleOffset(1.F);

          if(opts.yhigh > opts.ylow)
          {
              hist->GetYaxis()->SetRangeUser(opts.ylow, opts.yhigh);
          }
          else
          {
              hist->GetYaxis()->SetRangeUser(0, 1e-2);
          }

        }
        else
        {
          hist->Draw("same HIST");
        }

        short int color = my_colors[plot_idx];

        hist->SetLineColor(color);
        hist->SetMarkerColor(color);
        hist->SetLineWidth(3);

        leg->AddEntry(hist, legend_names[plot_idx].c_str(), "l");

        ++plot_idx;
      }

      leg->Draw("same");

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig)
      {
        c1->Print(std::format("{}/images/{}.png", m_output_dir, file_name).c_str());
      }
    };

    plotAndSave("h2_sEPD_Psi_S_2_corr2", {.title = "sEPD South"}, "Psi-South-overlay");
    plotAndSave("h2_sEPD_Psi_N_2_corr2", {.title = "sEPD North"}, "Psi-North-overlay");

    plotAndSave("h2_sEPD_Psi_S_2_corr2", {.title = "sEPD South", .ylow=7.2e-3, .yhigh=8.4e-3}, "Psi-South-overlay-zoom");
    plotAndSave("h2_sEPD_Psi_N_2_corr2", {.title = "sEPD North", .ylow=7.2e-3, .yhigh=8.4e-3}, "Psi-North-overlay-zoom");
  }

  c1->Print((output + "]").c_str(), "pdf portrait");

}

void DisplaySEPDQA::draw_QA()
{
  // set sPHENIX plotting style
  SetsPhenixStyle();

  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>("c1");
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(1200, 1000);
  c1->SetLeftMargin(.1F);
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
    struct PlotOptions
    {
      double totalEvents{0};
      int bins{0};
      int maxDigits{3};
      float labelSize{0.05F};
      std::string title{}; // NOLINT(readability-redundant-member-init)
    };

    auto plotAndSave = [&](TH1* hist, const PlotOptions &opts = {}, std::string_view name = "")
    {
      c1->SetRightMargin(.1F);
      gPad->SetTicky(0);

      hist->Draw("HIST");

      if (!opts.title.empty())
      {
        hist->SetTitle(opts.title.c_str());
      }
      hist->GetYaxis()->SetMaxDigits(opts.maxDigits);
      hist->GetXaxis()->SetLabelSize(opts.labelSize);

      int bins = (opts.bins) ? opts.bins : hist->GetNbinsX();
      hist->GetXaxis()->SetRange(1, bins);

      hist->SetLineColor(kBlue);
      hist->SetLineWidth(3);

      double ymax = hist->GetMaximum() * 1.1;
      hist->GetYaxis()->SetRangeUser(0, ymax);

      // 1. Calculate the percentage scale
      double right_axis_max = (ymax / opts.totalEvents) * 100.0;

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
      for (int i = 1; i <= bins; ++i)
      {
        double binContent = hist->GetBinContent(i);
        double percentage = (binContent / opts.totalEvents) * 100.0;

        // Define the x position (center of the bin)
        // and y position (just slightly above the bar)
        double xPos = hist->GetBinCenter(i);
        double yPos = binContent + (ymax * 0.02);  // 2% offset above the bar

        // Create the label string (fixed to 1 decimal place)
        TString label = std::format("{:.1f}%", percentage).c_str();

        // Create and style the text object
        TLatex* tex = new TLatex(xPos, yPos, label);
        tex->SetTextAlign(21);  // Center-aligned horizontally, bottom-aligned vertically
        tex->SetTextSize(0.05F);
        tex->SetTextFont(42);          // Standard Helvetica
        tex->SetTextColor(kBlue + 2);  // Slightly darker blue to stand out
        tex->Draw();
      }

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig)
      {
        c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());
      }

      c1->SetRightMargin(.02F);
      gPad->SetTicky(1);
    };

    auto* hEvent = m_hists["hEvent"].get();
    auto* hEventMinBias = m_hists["hEventMinBias"].get();

    plotAndSave(hEvent, {.totalEvents = hEvent->GetBinContent(1), .bins = 3}, "hEvent");
    plotAndSave(hEventMinBias, {.totalEvents = hEventMinBias->Integral(), .maxDigits = 0, .labelSize = 0.04F, .title = "|z| < 10 cm and Fail MB"}, "hEventMinBias");
  }

  // ---------------------------------------------------

  {
    auto plotAndSave = [&](std::string_view name = "", const std::string& title = "")
    {
      auto* hist = m_hists[std::string(name)].get();

      hist->Draw("HIST");

      if (!title.empty())
      {
        hist->SetTitle(title.c_str());
      }

      hist->SetLineColor(kBlue);
      hist->SetLineWidth(3);

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig)
      {
        c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());
      }
    };

    plotAndSave("hCentrality");
    plotAndSave("hVtxZ_MB", "Min Bias");
  }

  // ---------------------------------------------------

  {
    gPad->SetLogz();
    c1->SetRightMargin(.12F);

    auto* h2SEPD_Charge = dynamic_cast<TH2*>(m_hists["h2SEPD_Charge"].get());

    h2SEPD_Charge->Draw("COLZ1");
    h2SEPD_Charge->GetXaxis()->SetMaxDigits(3);
    h2SEPD_Charge->GetYaxis()->SetMaxDigits(3);

    double high = 12e3;
    h2SEPD_Charge->GetXaxis()->SetRangeUser(0, high);
    h2SEPD_Charge->GetYaxis()->SetRangeUser(0, high);

    auto* px = h2SEPD_Charge->ProfileX();

    px->Draw("same");

    px->SetMarkerStyle(kFullDotLarge);
    px->SetMarkerColor(kBlack);
    px->SetLineColor(kBlack);

    std::unique_ptr<TF1> f1 = std::make_unique<TF1>("f1", "x", 0, high);
    f1->Draw("same");

    f1->SetLineColor(kRed);
    f1->SetLineWidth(3);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig)
    {
      c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2SEPD_Charge").c_str());
    }

    gPad->SetLogz(0);
    c1->SetRightMargin(.02F);
  }

  // ---------------------------------------------------

  {
    c1->SetTopMargin(.08F);
    c1->SetBottomMargin(.12F);
    c1->SetLeftMargin(.1F);
    c1->SetRightMargin(.04F);

    unsigned int bins_calib = 140;
    double calib_low = -1e3;
    double calib_high = 400;

    std::unique_ptr<TH1> hCalib = std::make_unique<TH1F>("hCalib", "sEPD Calib; MPV [ADC]; Towers", bins_calib, calib_low, calib_high);

    for (int channel = 0; channel < m_nChannels; ++channel)
    {
      double calib = m_sepd_calib->GetFloatValue(channel, m_calib_field);
      hCalib->Fill(calib);
    }

    hCalib->Draw();

    hCalib->SetLineColor(kBlue);
    hCalib->SetLineWidth(3);

    hCalib->GetYaxis()->SetTitleOffset(1.F);
    hCalib->GetXaxis()->SetTitleOffset(1.F);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig)
    {
      c1->Print(std::format("{}/images/{}.png", m_output_dir, "hSEPD_Calib").c_str());
    }

    c1->SetTopMargin(.11F);
    c1->SetBottomMargin(.09F);
    c1->SetLeftMargin(.1F);
    c1->SetRightMargin(.04F);
  }

  // ---------------------------------------------------

  {
    unsigned int nRbins = 16;

    unsigned int bins_charge = 50;
    double charge_data_low = 0;
    double charge_data_high = 50;

    std::string h2_title = "sEPD South; r_{bin}; Average Charge; Channels";
    std::unique_ptr<TH2> h2SEPD_South_AvgCharge = std::make_unique<TH2F>("h2SEPD_South_AvgCharge", h2_title.c_str(), nRbins, 0, nRbins, bins_charge, charge_data_low, charge_data_high);
    
    h2_title = "sEPD North; r_{bin}; Average Charge; Channels";
    std::unique_ptr<TH2> h2SEPD_North_AvgCharge = std::make_unique<TH2F>("h2SEPD_North_AvgCharge", h2_title.c_str(), nRbins, 0, nRbins, bins_charge, charge_data_low, charge_data_high);

    // Using the 3D approach to group North and South
    // Indexing: sepd[Side][Rbin][Channel] where Side 0=South, 1=North
    std::vector<std::vector<std::vector<int>>> sepd(2, std::vector<std::vector<int>>(nRbins));

    for (auto& side : sepd)
    {
      for (auto& row : side)
      {
        row.reserve(24);
      }
    }

    auto* hSEPD_Charge = m_hists["hSEPD_Charge"].get();

    for (int channel = 0; channel < m_nChannels; ++channel)
    {
      unsigned int key = TowerInfoDefs::encode_epd(static_cast<unsigned int>(channel));
      unsigned int rbin = TowerInfoDefs::get_epd_rbin(key);
      unsigned int arm = TowerInfoDefs::get_epd_arm(key);
      sepd[arm][rbin].push_back(channel);

      double avg_charge = hSEPD_Charge->GetBinContent(channel+1);

      // South
      if (arm == 0)
      {
        h2SEPD_South_AvgCharge->Fill(rbin, avg_charge);
      }
      // North
      else
      {
        h2SEPD_North_AvgCharge->Fill(rbin, avg_charge);
      }
    }

    struct PlotOptions
    {
      float xoffset = 0.8F;
      float yoffset = 1.F;
      float zoffset = 0.8F;
    };

    c1->SetRightMargin(.14F);
    c1->SetTopMargin(.08F);

    auto plotAndSave = [&](TH2* hist, PlotOptions opts = {}, std::string_view tag = "")
    {
      hist->Draw("COLZ1");

      hist->GetXaxis()->SetTitleOffset(opts.xoffset);
      hist->GetYaxis()->SetTitleOffset(opts.yoffset);
      hist->GetZaxis()->SetTitleOffset(opts.zoffset);

      auto* px = hist->ProfileX("px", 2, -1, "s");

      px->SetMarkerColor(kRed);
      px->SetLineColor(kRed);

      auto* px_sigma = px->ProjectionX("px_sigma");

      for (int rbin = 1; rbin <= px->GetNbinsX(); ++rbin)
      {
        double sigma = px->GetBinError(rbin);
        px_sigma->SetBinError(rbin, sigma * 3);
      }

      px_sigma->Draw("E2 same");
      px_sigma->SetFillColorAlpha(8, 0.3F);

      px->Draw("same");

      double xshift = 0.47;
      double yshift = 0.18;

      std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .55 + yshift, 0.5 + xshift, .7 + yshift);
      leg->SetFillStyle(0);
      leg->SetTextSize(0.08F);
      leg->AddEntry(px, "#sigma", "l");
      leg->AddEntry(px_sigma, "3#sigma", "f");
      leg->Draw("same");

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig)
      {
        c1->Print(std::format("{}/images/{}{}.png", m_output_dir, hist->GetName(), tag).c_str());
      }
    };

    plotAndSave(h2SEPD_South_AvgCharge.get());
    plotAndSave(h2SEPD_North_AvgCharge.get());

    // ---------------------------------------------------

    std::unordered_map<int, std::string> bad_channels;

    if (m_verbosity)
    {
      for (size_t side = 0; side < sepd.size(); ++side)
      {
        // Use std::format for the header
        std::string_view side_name = (side == 0) ? "SOUTH" : "NORTH";
        std::cout << std::format("=== SIDE: {:<5} (Idx: {}) ===\n", side_name, side);

        for (size_t rbin = 0; rbin < sepd[side].size(); ++rbin)
        {
          // Format the row label with 2-digit padding for alignment
          std::cout << std::format("  Rbin {:02d}: [ ", rbin);

          const auto& channels = sepd[side][rbin];
          for (size_t ch = 0; ch < channels.size(); ++ch)
          {
            // Format each channel value
            std::cout << std::format("{}", channels[ch]);

            if (ch < channels.size() - 1)
            {
              std::cout << ", ";
            }
          }
          std::cout << " ]\n";
        }
        std::cout << "\n";
      }

      auto* px_south = h2SEPD_South_AvgCharge->ProfileX("px_south", 2, -1, "s");
      auto* px_north = h2SEPD_North_AvgCharge->ProfileX("px_north", 2, -1, "s");

      struct ChannelIssue
      {
        std::string type;
        int channel;
        unsigned int arm;
        int rbin;
        double avg_charge;
        double threshold;
        double zscore;
        double mpv;
      };

      std::vector<ChannelIssue> issues;

      for (int channel = 0; channel < m_nChannels; ++channel)
      {
        unsigned int key = TowerInfoDefs::encode_epd(static_cast<unsigned int>(channel));
        int rbin = static_cast<int>(TowerInfoDefs::get_epd_rbin(key));
        unsigned int arm = TowerInfoDefs::get_epd_arm(key);

        auto* hprof = (arm == 0) ? px_south : px_north;
        double avg_charge = hSEPD_Charge->GetBinContent(channel + 1);
        double mean_charge = hprof->GetBinContent(rbin + 1);
        double sigma = hprof->GetBinError(rbin + 1);
        double zscore = (sigma > 0) ? (avg_charge - mean_charge) / sigma : 0.0;
        double mpv = m_sepd_calib->GetFloatValue(channel, m_calib_field);

        if (avg_charge < 2 || std::abs(zscore) > 3)
        {
          std::string label;
          if (avg_charge == 0)
          {
            label = "DEAD";
          }
          else if (avg_charge < 2 || zscore < -3)
          {
            label = "COLD";
          }
          else
          {
            label = "HOT";
          }

          bad_channels[channel] = label;
          issues.push_back({label, channel, arm, rbin, avg_charge, mean_charge + 3*sigma, zscore, mpv});
        }
      }

      std::sort(issues.begin(), issues.end(), [](const ChannelIssue& a, const ChannelIssue& b)
                {
                  if (a.arm != b.arm)
                  {
                    return a.arm < b.arm;  // Sort by arm (0 then 1)
                  }
                  if (a.rbin != b.rbin)
                  {
                    return a.rbin < b.rbin;  // Then by rbin
                  }
                  return a.channel < b.channel;  // Then by channel
                });

      // 3. Print the sorted list
      std::cout << "\n--- Summary of Problematic Channels (Sorted by Arm/Rbin) ---\n";
      for (const auto& i : issues)
      {
        RGB color{};

        if (i.type == "HOT")
        {
          color = Colors::Red;
        }
        if(i.type == "DEAD")
        {
          color = Colors::White;
        }
        if(i.type == "COLD")
        {
          color = Colors::Cyan;

          if (i.mpv == -999)
          {
            color = Colors::Yellow;
          }
        }

        std::string fmt = "{:<5} Channel {:>4}, arm: {}, rbin: {:>2}, avg charge: {:>6.2f}, threshold: {:>6.2f}, z-score: {:>6.2f}, calib (mpv): {:>6.2f}";

        print_color(color, fmt, i.type, i.channel, i.arm, i.rbin, i.avg_charge, i.threshold, i.zscore, i.mpv);
      }

      std::cout << std::format("{:#<20}\n", "");
      std::cout << std::format("Total Bad Channels: {}\n", bad_channels.size());
    }

    auto* h2SEPD_Channel_Charge = dynamic_cast<TH2*>(m_hists["h2SEPD_Channel_Charge"].get());

    std::unique_ptr<TCanvas> c_r0 = std::make_unique<TCanvas>("c_r0");
    c_r0->SetTickx();
    c_r0->SetTicky();

    c_r0->SetCanvasSize(2800, 2000);

    std::unique_ptr<TCanvas> c_r = std::make_unique<TCanvas>("c_r");
    c_r->SetTickx();
    c_r->SetTicky();

    c_r->SetCanvasSize(4200, 2700);

    std::string output_r0 = std::format("{}/pdf/plots-sepd-channels-r0.pdf", m_output_dir);
    std::string output_r = std::format("{}/pdf/plots-sepd-channels-r.pdf", m_output_dir);

    c_r0->Print((output_r0 + "[").c_str(), "pdf portrait");
    c_r->Print((output_r + "[").c_str(), "pdf portrait");

    for (size_t side = 0; side < sepd.size(); ++side)
    {
      std::string_view side_name = (side == 0) ? "sEPD South" : "sEPD North";
      for (size_t rbin = 0; rbin < sepd[side].size(); ++rbin)
      {
        const auto& channels = sepd[side][rbin];

        std::unique_ptr<TLatex> t = std::make_unique<TLatex>();
        t->SetNDC();           // Use Normalized Device Coordinates (0 to 1)
        t->SetTextAlign(22);   // Center alignment (horizontal and vertical)
        t->SetTextSize(0.035F);

        std::string canvas_title = std::format("{}, R_{{bin}} = {}", side_name, rbin);

        if (rbin == 0)
        {
          c_r0->SetTopMargin(.15F);
          c_r0->SetLeftMargin(.1F);
          c_r0->SetRightMargin(.1F);
          c_r0->SetBottomMargin(.1F);
          c_r0->Divide(4, 3, 0, 0);

          c_r0->cd(0);
        }
        else
        {
          c_r->SetTopMargin(.23F);
          c_r->SetLeftMargin(.1F);
          c_r->SetRightMargin(.1F);
          c_r->SetBottomMargin(.1F);
          c_r->Divide(6, 4, 0, 0);
          c_r->cd(0);
        }

        t->DrawLatex(0.5, 0.975, canvas_title.c_str()); 

        for (size_t ch = 0; ch < channels.size(); ++ch)
        {
          int pad_idx = static_cast<int>(ch) + 1;
          if (rbin == 0)
          {
            c_r0->cd(pad_idx);
          }
          else
          {
            c_r->cd(pad_idx);
          }

          std::string name = std::format("py_{}", ch);
          int channel = channels[ch];

          auto* hist = h2SEPD_Channel_Charge->ProjectionY(name.c_str(), channel + 1, channel + 1);

          int bin_low = hist->FindBin(0.5);
          int bin_high = hist->FindBin(3) - 1;
          double total_events = hist->Integral(bin_low, bin_high);

          if (total_events)
          {
            hist->Scale(1. / total_events);
          }

          hist->Draw("HIST");

          std::string title = std::format("#bf{{Channel: {}}}", channel);
          t->SetTextSize(0.1F);
          t->DrawLatex(0.7, 0.95, title.c_str());

          hist->SetTitle("");

          hist->GetYaxis()->SetTitle("Normalized Counts");
          hist->GetYaxis()->SetTitleOffset(1.F);

          hist->GetYaxis()->SetRangeUser(0, 7e-3);

          hist->GetXaxis()->SetRangeUser(0.5, 3);
          hist->GetYaxis()->SetMaxDigits(3);

          hist->GetXaxis()->SetTitleOffset(1.F);
          hist->GetXaxis()->SetLabelSize(0.05F);
          hist->GetXaxis()->SetLabelFont(62); // Bold Text
          hist->GetXaxis()->SetTitleFont(62); // Bold Text

          hist->SetLineColor(kBlue);
          hist->SetLineWidth(3);

          if (auto it = bad_channels.find(channel); it != bad_channels.end())
          {
            std::string bad_type = it->second;
            if (bad_type == "HOT")
            {
              gPad->SetFrameFillColor(kOrange);
            }
            else
            {
              gPad->SetFrameFillColor(kCyan);
            }
          }

          gPad->Update();  // Ensure the pad limits are calculated

          // Get the y-axis limits automatically
          double ymin = gPad->GetUymin();
          double ymax = gPad->GetUymax();

          // Create the vertical line at x = 1
          TLine* line = new TLine(1.0, ymin, 1.0, ymax);

          line->SetLineColor(kBlack);
          line->SetLineStyle(2);  // Dashed line
          line->SetLineWidth(2);

          line->Draw();
        }

        if(rbin == 0)
        {
          c_r0->Print(output_r0.c_str(), "pdf portrait");
          if (m_saveFig)
          {
            c_r0->Print(std::format("{}/images/{}-{}-{}.png", m_output_dir, "hSEPD_Charge", side, rbin).c_str());
          }
          c_r0->Clear();
        }
        else
        {
          c_r->Print(output_r.c_str(), "pdf portrait");
          if (m_saveFig)
          {
            c_r->Print(std::format("{}/images/{}-{}-{}.png", m_output_dir, "hSEPD_Charge", side, rbin).c_str());
          }
          c_r->Clear();
        }
      }
    }

    c_r->Print((output_r + "]").c_str(), "pdf portrait");
    c_r->Print((output_r0 + "]").c_str(), "pdf portrait");
  }

  // ---------------------------------------------------

  {
    c1->cd();
    gPad->SetLogy();

    c1->SetRightMargin(.03F);
    c1->SetLeftMargin(.15F);

    struct PlotOptions
    {
      int channel_bad{0};
      int channel_ref{0};
      double xlow{0};
      double xhigh{160};
      short int color_bad{kRed};
    };

    auto plotAndSave = [&](TH2* hist, PlotOptions opts = {}, std::string name = "")
    {
      auto* hBad = hist->ProjectionY("hBad", opts.channel_bad + 1, opts.channel_bad + 1);
      auto* hRef = hist->ProjectionY("hRef", opts.channel_ref + 1, opts.channel_ref + 1);

      int bin_low = hBad->FindBin(0);
      // Normalized
      hBad->Scale(1./hBad->Integral(bin_low, -1));
      hRef->Scale(1./hRef->Integral(bin_low, -1));

      hBad->Draw("HIST");
      hRef->Draw("HIST same");

      hBad->SetLineColor(opts.color_bad);
      hRef->SetLineColor(kBlack);

      hBad->SetLineWidth(3);
      hRef->SetLineWidth(3);

      hBad->GetYaxis()->SetTitle("Normalized Events");
      hBad->GetXaxis()->SetTitleOffset(1.F);
      hBad->GetXaxis()->SetLabelSize(0.04F);

      hBad->GetXaxis()->SetRangeUser(0, 200);

      double ymax = std::max(hBad->GetMaximum(), hRef->GetMaximum());
      hBad->GetYaxis()->SetRangeUser(3e-8, ymax*5);

      // std::cout << std::format("Integral: Bad: {:.2f}, Ref: {:.2f}\n", hBad->Integral(bin_low,-1), hRef->Integral(bin_low,-1));

      if (opts.xhigh > opts.xlow)
      {
        hBad->GetXaxis()->SetRangeUser(opts.xlow, opts.xhigh);
      }

      unsigned int key = TowerInfoDefs::encode_epd(static_cast<unsigned int>(opts.channel_bad));
      unsigned int rbin = TowerInfoDefs::get_epd_rbin(key);
      unsigned int arm = TowerInfoDefs::get_epd_arm(key);

      std::string side = (arm == 0) ? "South" : "North";
      std::string title = std::format("sEPD {}, R_{{bin}} = {}", side, rbin);

      hBad->SetTitle(title.c_str());

      double xshift = 0.15;
      double yshift = 0.18;

      std::string type = (opts.color_bad == kRed) ? "Hot" : "Cold";

      std::string leg_A = std::format("{} Channel: {}", type, opts.channel_bad);
      std::string leg_B = std::format("Ref Channel: {}", opts.channel_ref);

      std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .55 + yshift, 0.5 + xshift, .7 + yshift);
      leg->SetFillStyle(0);
      leg->SetTextSize(0.08F);
      leg->AddEntry(hBad, leg_A.c_str(), "l");
      leg->AddEntry(hRef, leg_B.c_str(), "l");
      leg->Draw("same");

      if (name.empty())
      {
        name = std::format("sEPD-Bad-{}", opts.channel_bad);
      }

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig)
      {
        c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());
      }
    };

    std::vector<PlotOptions> bad_channels = {
        {.channel_bad = 53, .channel_ref = 52, .xhigh = 120},
        {.channel_bad = 590, .channel_ref = 591, .xhigh = 120, .color_bad = kBlue},
        {.channel_bad = 469, .channel_ref = 468},
        {.channel_bad = 471, .channel_ref = 470, .xhigh = 140}, 
        {.channel_bad = 473, .channel_ref = 472, .xhigh = 100}, 
        {.channel_bad = 487, .channel_ref = 486, .xhigh = 120}
    };

    auto* h2SEPD_Channel_Chargev2 = dynamic_cast<TH2*>(m_hists["h2SEPD_Channel_Chargev2"].get());

    for (const auto& opts : bad_channels)
    {
      plotAndSave(h2SEPD_Channel_Chargev2, opts);
    }
  }

  c1->Print((output + "]").c_str(), "pdf portrait");
}

int main(int argc, const char* const argv[])
{
  gROOT->SetBatch(true);

  if (argc > 4)
  {
    std::cout << "Usage: " << argv[0] << "[output_directory] [verbosity] [saveFig]" << std::endl;
    return 1;
  }

  std::string output_dir = (argc >= 2) ? argv[1] : ".";
  int verbosity = (argc >= 3) ? std::atoi(argv[2]) : 0;
  bool saveFig = (argc >= 4) ? std::atoi(argv[3]) : false;

  try
  {
    DisplaySEPDQA analysis(output_dir, verbosity, saveFig);
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
