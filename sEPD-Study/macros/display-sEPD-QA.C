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
#include <TLegendEntry.h>
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
    draw_psi();
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

  // --- Private Helper Methods ---
  void read_hists();
  void init_hists();
  void draw_psi();
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
  std::string input = "/sphenix/user/anarde/sEPD-Study/f4a/05-08-26-run3auau/merged/output/68144.root";
  std::string calib = "/cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/SEPD_NMIP_CALIB/86/6d/866db5abdca24d60cc47f8856c8e97f4_sEPD_CalibConstants_CDB_Run71504_v2.root";
  std::string input_jetAna = "/gpfs02/sphenix/user/anarde/sEPD-Study/jetAna/05-22-26-jetAna/merged/output/68144.root";

  m_hists = myUtils::read_hists(input);
  m_sepd_calib = std::make_unique<CDBTTree>(calib);

  std::unordered_set<std::string> names_jetAna = {"h2_sEPD_Psi_S_2_raw", "h2_sEPD_Psi_N_2_raw",
                                                 "h2_sEPD_Psi_S_2_recentered", "h2_sEPD_Psi_N_2_recentered",
                                                 "h2_sEPD_Psi_S_2_flat", "h2_sEPD_Psi_N_2_flat"};

  m_hists.merge(myUtils::read_hists(input_jetAna, "_68144", &names_jetAna));

  // Event Plane Resolution Study
  std::string EP_base_output_dir = "/gpfs02/sphenix/user/anarde/sEPD-Calib/2026-06-01-run3auau-test";

  std::string EP_base_output_suffix = "ApplyFlattening/output/hist/QVecCalib-68144.root";

  std::map<std::string, std::string> EP_files = {
      {"_0", std::format("{}/stage-QVecCalib-0.0-0.5/{}", EP_base_output_dir, EP_base_output_suffix)},
      {"_50", std::format("{}/stage-QVecCalib-50.0-0.5/{}", EP_base_output_dir, EP_base_output_suffix)},
      {"_25", std::format("{}/stage-QVecCalib-25.0-0.5/{}", EP_base_output_dir, EP_base_output_suffix)},
      {"_10", std::format("{}/stage-QVecCalib-10.0-0.5/{}", EP_base_output_dir, EP_base_output_suffix)},
      {"_5", std::format("{}/stage-QVecCalib-5.0-0.5/{}", EP_base_output_dir, EP_base_output_suffix)}
  };

  std::unordered_set<std::string> names = {"hEP_res_2", "h2_sEPD_Psi_S_2_corr2", "h2_sEPD_Psi_N_2_corr2"};

  for (const auto& [tag, file] : EP_files)
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

  std::vector<short int> my_colors = {kRed + 1, kAzure + 1, kOrange + 7, kGreen + 2, kMagenta + 1};

  std::vector<std::string> legend_names = {"0.5#leq N_{mip}",
                                           "0.5#leq N_{mip}#leq50",
                                           "0.5#leq N_{mip}#leq25",
                                           "0.5#leq N_{mip}#leq10",
                                           "0.5#leq N_{mip}#leq5"};

  std::vector<std::string> tags = {"_0", "_50", "_25", "_10", "_5"};

  {
    c1->SetTopMargin(.02F);
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

      // auto* hDefault = m_hists["hEP_res_sqrt_2_default"].get();
      auto* hDefault = m_hists["hEP_res_sqrt_2_0"].get();

      hist_ratio->Divide(hDefault);
      m_hists[name_ratio] = std::unique_ptr<TH1>(hist_ratio);

      if (plot_idx == 0)
      {
        hist->Draw("hist l p");
        hist->Draw("same p e X0");
        hist->SetTitle("; Centrality [%]; sEPD #Psi_{2} Resolution");
        hist->GetYaxis()->SetTitleSize(0.06F);
        hist->GetXaxis()->SetTitleSize(0.05F);

        hist->GetYaxis()->SetTitleOffset(0.9F);
        hist->GetXaxis()->SetTitleOffset(0.8F);

        hist->GetYaxis()->SetLabelSize(0.04F);
        hist->GetXaxis()->SetLabelSize(0.04F);
        hist->GetYaxis()->SetRangeUser(0, 0.6);
        hist->GetXaxis()->SetRangeUser(0, 59.5);
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

  {
    auto* hEP_thresh_50 = m_hists["hEP_res_sqrt_2_50"].get();

    hEP_thresh_50->Draw("hist l p");
    hEP_thresh_50->Draw("same p e X0");

    hEP_thresh_50->SetTitle("; Centrality [%]; sEPD #Psi_{2} Resolution");

    hEP_thresh_50->GetYaxis()->SetRangeUser(0, 0.6);
    hEP_thresh_50->GetXaxis()->SetRangeUser(0, 59.5);

    hEP_thresh_50->GetYaxis()->SetTitleSize(0.06F);
    hEP_thresh_50->GetXaxis()->SetTitleSize(0.05F);

    hEP_thresh_50->GetYaxis()->SetTitleOffset(0.9F);
    hEP_thresh_50->GetXaxis()->SetTitleOffset(0.8F);

    hEP_thresh_50->GetYaxis()->SetLabelSize(0.04F);
    hEP_thresh_50->GetXaxis()->SetLabelSize(0.04F);

    hEP_thresh_50->SetLineColor(kAzure+1);
    hEP_thresh_50->SetMarkerColor(kAzure+1);
    hEP_thresh_50->SetLineWidth(3);

    double xshift = 0.05;
    double yshift = -0.25;

    std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .6 + yshift, 0.5 + xshift, .7 + yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.05F);

    leg->AddEntry(hEP_thresh_50, "0.5#leq N_{mip}#leq50", "lpe");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig)
    {
      c1->Print(std::format("{}/images/{}.png", m_output_dir, "EP-res-thresh-50").c_str());
    }
  }

  // ---------------------------------------------------

  // Ratio Plots
  {
    c1->SetTopMargin(.02F);
    c1->SetLeftMargin(.14F);

    double xshift = -0.05;
    double yshift = 0.28;

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
         hist->GetYaxis()->SetTitle("Ratio (With 0.5#leq N_{mip})");
         hist->GetXaxis()->SetRangeUser(0, 59.5);
         hist->GetYaxis()->SetRangeUser(0.9, 1.15);
         hist->GetYaxis()->SetLabelSize(0.05F);
         hist->GetXaxis()->SetLabelSize(0.05F);
         hist->GetYaxis()->SetTitleSize(0.05F);
         hist->GetXaxis()->SetTitleSize(0.05F);

         hist->GetYaxis()->SetTitleOffset(1.3F);
         hist->GetXaxis()->SetTitleOffset(0.83F);
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

  {
    c1->SetLeftMargin(0.11F);
    c1->SetRightMargin(.03F);
    c1->SetTopMargin(0.06F);
    c1->SetBottomMargin(0.1F);

    // -------------------------------------------
    // Event Plane Resolution
    // -------------------------------------------

    // -------------------------------------------
    //  Presentation by Rosi at WWND2022
    //  https://indico.cern.ch/event/1039540/contributions/4737615/attachments/2401701/4107451/RReedWWND2022v1.pdf
    //  Data points from Slide 24
    //  Exact Data Points from Rosi
    // -------------------------------------------

    // sPHENIX MC
    // std::vector<double> evt_res_sPHENIX_MC = {0.34, 0.5, 0.57, 0.55, 0.5, 0.42};
    // std::vector<double> evt_res_sPHENIX_MC = {0.3358, 0.5031, 0.5671, 0.5540, 0.4991, 0.4171, 0.3161, 0.2143, 0.1501, 0.1236};
    std::vector<double> evt_res_sPHENIX_MC = {0.3358, 0.5031, 0.5671, 0.5540, 0.4991, 0.4171};

    // STAR EPD
    // Centrality binning uneven thus requiring x,y points
    std::vector<std::pair<double, double>> evt_res_star = {
        std::make_pair(2.5, 0.293188),
        std::make_pair(7.5, 0.449476),
        std::make_pair(15, 0.522967),
        std::make_pair(25, 0.558795),
        std::make_pair(35, 0.53074),
        std::make_pair(45, 0.438996),
        std::make_pair(55, 0.332305)
        // std::make_pair(65, 0.205977),
        // std::make_pair(75, 0.0571169)
    };

    std::vector<std::pair<double, double>> evt_res_star_error = {
        std::make_pair(2.5, 0.0174575),
        std::make_pair(7.5, 0.0132602),
        std::make_pair(15, 0.00724971),
        std::make_pair(25, 0.00650445),
        std::make_pair(35, 0.0068557),
        std::make_pair(45, 0.0089328),
        std::make_pair(55, 0.0128119)
        // std::make_pair(65, 0.0221074),
        // std::make_pair(75, 0.0574402)
    };

    // sEPD Run 2 Au+Au
    // Centrality binning uneven thus requiring x,y points
    std::vector<std::pair<double, double>> evt_res_sepd_run2 = {
        std::make_pair(2.5, 0.1930),
        std::make_pair(7.5, 0.2989),
        std::make_pair(15, 0.4485),
        std::make_pair(25, 0.5347),
        std::make_pair(35, 0.5267),
        std::make_pair(45, 0.4444),
        std::make_pair(55, 0.3227)
        // std::make_pair(65, 0.2132),
    };


    std::unique_ptr<TGraphErrors> graph_evt_res_star = std::make_unique<TGraphErrors>(evt_res_star.size());
    graph_evt_res_star->SetTitle("; Centrality [%]; sEPD #Psi_{2} Resolution");

    std::unique_ptr<TGraphErrors> graph_evt_res_sepd_run2 = std::make_unique<TGraphErrors>(evt_res_sepd_run2.size());

    for (size_t point = 0; point < evt_res_star.size(); ++point)
    {
      double x = evt_res_star[point].first;
      double y = evt_res_star[point].second;
      double yerr = evt_res_star_error[point].second;
      graph_evt_res_star->SetPoint(static_cast<int>(point), x, y);
      graph_evt_res_star->SetPointError(static_cast<int>(point), 0, yerr);

      // -----------------------------------

      double x_sepd = evt_res_sepd_run2[point].first;
      double y_sepd = evt_res_sepd_run2[point].second;
      graph_evt_res_sepd_run2->SetPoint(static_cast<int>(point), x_sepd, y_sepd);
    }

    int bins_cent{6};
    double cent_low{0};
    double cent_high{60};

    std::string hist_name = "evt_res_sPHENIX_MC";
    std::string title = "; Centrality [%]; sEPD #Psi_{2} Resolution";
    m_hists[hist_name] = std::make_unique<TH1F>(hist_name.c_str(), title.c_str(), bins_cent, cent_low, cent_high);
    auto* hSP_res_sPHENIX_MC = m_hists[hist_name].get();

    for (int bin_cent = 1; bin_cent <= bins_cent; ++bin_cent)
    {
      hSP_res_sPHENIX_MC->SetBinContent(bin_cent, evt_res_sPHENIX_MC[static_cast<size_t>(bin_cent - 1)]);
      // std::cout << std::format("sPHENIX Bin: {}, x: {:.2f}\n", bin_cent, hSP_res_sPHENIX_MC->GetBinCenter(bin_cent));
    }

    auto* hSP_evt_res_50_temp = m_hists["hEP_res_sqrt_2_50"].get();
    auto* hSP_evt_res_10_temp = m_hists["hEP_res_sqrt_2_10"].get();

    // Create new hists to match the bin centers of STAR and other comparisons
    auto hSP_evt_res_50 = std::make_unique<TH1F>("hSP_evt_res_50", title.c_str(), bins_cent, cent_low, cent_high);
    auto hSP_evt_res_10 = std::make_unique<TH1F>("hSP_evt_res_10", title.c_str(), bins_cent, cent_low, cent_high);

    auto hSP_evt_res_combined = std::make_unique<TH1F>("hSP_evt_res_combined", title.c_str(), bins_cent, cent_low, cent_high);

    for (int bin = 1; bin <= hSP_evt_res_combined->GetNbinsX(); ++bin)
    {
      double val = 0;
      double err = 0;

      double val_50 = hSP_evt_res_50_temp->GetBinContent(bin);
      double val_10 = hSP_evt_res_10_temp->GetBinContent(bin);

      double err_50 = hSP_evt_res_50_temp->GetBinError(bin);
      double err_10 = hSP_evt_res_10_temp->GetBinError(bin);

      if (bin <= 3)
      {
        val = hSP_evt_res_50_temp->GetBinContent(bin);
        err = hSP_evt_res_50_temp->GetBinError(bin);
      }
      else
      {
        val = hSP_evt_res_10_temp->GetBinContent(bin);
        err = hSP_evt_res_10_temp->GetBinError(bin);
      }

      hSP_evt_res_combined->SetBinContent(bin, val);
      hSP_evt_res_combined->SetBinError(bin, err);

      hSP_evt_res_10->SetBinContent(bin, val_10);
      hSP_evt_res_10->SetBinError(bin, err_10);

      hSP_evt_res_50->SetBinContent(bin, val_50);
      hSP_evt_res_50->SetBinError(bin, err_50);
    }

    hSP_evt_res_50->SetMarkerColor(kBlue);
    hSP_evt_res_50->SetLineColor(kBlue);
    hSP_evt_res_50->SetMarkerStyle(kFullDotLarge);
    hSP_evt_res_50->SetLineStyle(kDashed);
    hSP_evt_res_50->SetMarkerSize(3);
    hSP_evt_res_50->SetLineWidth(3);

    hSP_evt_res_10->SetMarkerColor(kCyan+1);
    hSP_evt_res_10->SetMarkerStyle(kFullDotLarge);
    hSP_evt_res_10->SetMarkerSize(3);
    hSP_evt_res_10->SetLineColor(kCyan+1);
    hSP_evt_res_10->SetLineWidth(3);
    hSP_evt_res_10->SetLineStyle(kDashed);

    hSP_evt_res_combined->SetMarkerColor(kBlue);
    hSP_evt_res_combined->SetMarkerStyle(kFullDotLarge);
    hSP_evt_res_combined->SetMarkerSize(3);
    hSP_evt_res_combined->SetLineColor(kBlue);
    hSP_evt_res_combined->SetLineWidth(3);
    hSP_evt_res_combined->SetLineStyle(kDashed);
    hSP_evt_res_combined->GetYaxis()->SetRangeUser(0, 0.6);
    hSP_evt_res_combined->GetYaxis()->SetTitleOffset(1.F);
    hSP_evt_res_combined->GetXaxis()->SetTitleOffset(0.92F);

    // 1. Configure the original graph for markers and SOLID error bars
    graph_evt_res_star->SetMarkerColor(kRed);
    graph_evt_res_star->SetLineColor(kRed);
    graph_evt_res_star->SetMarkerStyle(kFullStar);
    graph_evt_res_star->SetMarkerSize(4);
    graph_evt_res_star->SetLineWidth(3);
    graph_evt_res_star->SetLineStyle(kSolid);  // Keep error bars solid

    graph_evt_res_star->GetYaxis()->SetRangeUser(0, 0.6);
    graph_evt_res_star->GetXaxis()->SetLimits(0, 60);
    graph_evt_res_star->GetYaxis()->SetTitleOffset(1.F);
    graph_evt_res_star->GetXaxis()->SetTitleOffset(0.92F);

    // sEPD Run 2 Au+Au (Ejiro)
    graph_evt_res_sepd_run2->SetMarkerColor(kViolet+2);
    graph_evt_res_sepd_run2->SetLineColor(kViolet+2);
    graph_evt_res_sepd_run2->SetMarkerStyle(kFullDiamond);
    graph_evt_res_sepd_run2->SetMarkerSize(3);
    graph_evt_res_sepd_run2->SetLineWidth(3);

    // 2. Clone the graph to handle the dashed connecting line
    TGraphErrors* graph_line = dynamic_cast<TGraphErrors*>(graph_evt_res_star->Clone("graph_line"));
    graph_line->SetLineStyle(kDashed);

    TGraphErrors* graph_line_sepd_run2 = dynamic_cast<TGraphErrors*>(graph_evt_res_sepd_run2->Clone("graph_line"));
    graph_line_sepd_run2->SetLineStyle(kDashed);

    hSP_res_sPHENIX_MC->SetMarkerColor(kGreen+3);
    hSP_res_sPHENIX_MC->SetLineColor(kGreen+3);
    hSP_res_sPHENIX_MC->SetMarkerStyle(kFullSquare);
    hSP_res_sPHENIX_MC->SetLineWidth(3);
    hSP_res_sPHENIX_MC->SetLineStyle(kDashed);
    hSP_res_sPHENIX_MC->SetMarkerSize(2);

    double xshift = -0.04;
    double yshift = -0.55;

    auto leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .95 + yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.05F);
    auto* legEntry_sPHENIX_MC = leg->AddEntry(hSP_res_sPHENIX_MC, "Legacy sEPD MC (Optimized Truncation)", "lp");
    auto* legEntry_sPHENIX_50 = leg->AddEntry(hSP_evt_res_50.get(), "sEPD: Run 3 Au+Au (0.5#leqN_{mip}#leq50)", "p");
    auto* legEntry_sPHENIX_10 = leg->AddEntry(hSP_evt_res_10.get(), "sEPD: Run 3 Au+Au (0.5#leqN_{mip}#leq10)", "lp");
    auto* legEntry_sPHENIX_Run2 = leg->AddEntry(graph_line_sepd_run2, "sEPD: Run 2 Au+Au (By Ejiro)", "lp");
    auto* legEntry_STAR = leg->AddEntry(graph_line, "STAR EPD", "lp");

    // Draw ONLY the points and error bars (using option "p")
    graph_evt_res_star->Draw("ap");
    // Draw ONLY the connecting line (using option "l")
    graph_line->Draw("same l");

    hSP_res_sPHENIX_MC->Draw("same l p");

    leg->Draw("same");

    graph_evt_res_sepd_run2->Draw("same p");
    graph_line_sepd_run2->Draw("same l");

    hSP_evt_res_50->Draw("same hist l p");
    hSP_evt_res_50->Draw("same p e X0");

    hSP_evt_res_10->Draw("same p e X0");
    hSP_evt_res_10->Draw("same hist l p");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/Event-Plane-Resolution-Overlay-v1.png", m_output_dir).c_str());

    // sEPD Data Only

    leg->GetListOfPrimitives()->Remove(legEntry_sPHENIX_10);
    leg->GetListOfPrimitives()->Remove(legEntry_sPHENIX_MC);
    leg->GetListOfPrimitives()->Remove(legEntry_STAR);
    leg->GetListOfPrimitives()->Remove(legEntry_sPHENIX_Run2);

    xshift = 0.01;
    yshift = -0.61;

    leg->SetX1NDC(0.2 + xshift);
    leg->SetY1NDC(0.8 + yshift);
    leg->SetX2NDC(0.54 + xshift);
    leg->SetY2NDC(0.95 + yshift);

    // Update the canvas
    c1->Modified();
    c1->Update();

    legEntry_sPHENIX_50->SetLabel("sEPD: Run 68144");

    hSP_evt_res_combined->Draw("p e X0");

    leg->Draw("same");

    c1->Modified();
    c1->Update();

    std::vector<std::unique_ptr<TLatex>> labels;
    labels.push_back(myUtils::draw_text(0.13, 0.97, "Au+Au #sqrt{s_{NN}} = 200 GeV", 0.05F));
    labels.push_back(myUtils::draw_text(0.62, 0.97, "Run 25", 0.05F));
    labels.push_back(myUtils::draw_text(0.8, 0.97, "6/12/2026", 0.05F));

    yshift = -0.33;

    labels.push_back(myUtils::draw_text(0.3, 0.85+yshift, "#bf{#it{sPHENIX}} Performance", 0.05F));
    labels.push_back(myUtils::draw_text(0.3, 0.75 + yshift, "0-30%: 0.5#kern[0.7]{#leq}#kern[0.2]{N_{mip}}#kern[0.6]{#leq}#kern[0.3]{50}", 0.05F));
    labels.push_back(myUtils::draw_text(0.3, 0.67 + yshift, "30-60%: 0.5#kern[0.7]{#leq}#kern[0.2]{N_{mip}}#kern[0.6]{#leq}#kern[0.2]{10}", 0.05F));

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/Event-Plane-Resolution-Overlay-v2.png", m_output_dir).c_str());

    leg->GetListOfPrimitives()->Remove(legEntry_sPHENIX_50);
    c1->GetListOfPrimitives()->Remove(hSP_evt_res_combined.get());

    c1->Modified();
    c1->Update();

    // sEPD Data vs STAR
    graph_evt_res_star->Draw("ap");

    hSP_evt_res_combined->Draw("same p e X0");

    labels.push_back(myUtils::draw_text(0.13, 0.97, "Au+Au #sqrt{s_{NN}} = 200 GeV", 0.05F));
    labels.push_back(myUtils::draw_text(0.62, 0.97, "Run 25", 0.05F));
    labels.push_back(myUtils::draw_text(0.8, 0.97, "6/12/2026", 0.05F));
    labels.push_back(myUtils::draw_text(0.3, 0.85+yshift, "#bf{#it{sPHENIX}} Performance", 0.05F));
    labels.push_back(myUtils::draw_text(0.3, 0.75 + yshift, "0-30%: 0.5#kern[0.7]{#leq}#kern[0.2]{N_{mip}}#kern[0.6]{#leq}#kern[0.3]{50}", 0.05F));
    labels.push_back(myUtils::draw_text(0.3, 0.67 + yshift, "30-60%: 0.5#kern[0.7]{#leq}#kern[0.2]{N_{mip}}#kern[0.6]{#leq}#kern[0.2]{10}", 0.05F));

    xshift = 0.01;
    yshift = -0.65;

    leg = std::make_unique<TLegend>(0.2 + xshift, .8 + yshift, 0.54 + xshift, .95 + yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.05F);
    leg->AddEntry(hSP_evt_res_combined.get(), "sEPD: Run 68144", "p");
    leg->AddEntry(graph_evt_res_star.get(), "STAR EPD (Private Communication)", "p");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/Event-Plane-Resolution-Overlay-v3.png", m_output_dir).c_str());
  }

  c1->Print((output + "]").c_str(), "pdf portrait");

}

void DisplaySEPDQA::draw_psi()
{
  // set sPHENIX plotting style
  SetsPhenixStyle();

  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>("c1");
  c1->SetTickx();
  c1->SetTicky();

  c1->Divide(2, 1, 0.00025F, 0.00025F);
  c1->SetCanvasSize(2500, 1200);

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
  output = std::format("{}/pdf/plots-Psi.pdf", m_output_dir);

  c1->Print((output + "[").c_str(), "pdf portrait");

  auto* hPsiS_raw = dynamic_cast<TH2*>(m_hists["h2_sEPD_Psi_S_2_raw_68144"].get())->ProjectionX();
  auto* hPsiN_raw = dynamic_cast<TH2*>(m_hists["h2_sEPD_Psi_N_2_raw_68144"].get())->ProjectionX();

  auto* hPsiS_recentered = dynamic_cast<TH2*>(m_hists["h2_sEPD_Psi_S_2_recentered_68144"].get())->ProjectionX();
  auto* hPsiN_recentered = dynamic_cast<TH2*>(m_hists["h2_sEPD_Psi_N_2_recentered_68144"].get())->ProjectionX();

  auto* hPsiS_flat = dynamic_cast<TH2*>(m_hists["h2_sEPD_Psi_S_2_flat_68144"].get())->ProjectionX();
  auto* hPsiN_flat = dynamic_cast<TH2*>(m_hists["h2_sEPD_Psi_N_2_flat_68144"].get())->ProjectionX();

  struct PlotOptions
  {
    double ymax{0};
    float margin_bottom{0.12F};
    float margin_left{0.14F};
    float margin_right{0.01F};
    bool show_ylabels{true};
    bool show_legend{true};
    bool show_labels{true};
    std::string det{};
    short int line_width{6};
  };

  std::vector<std::unique_ptr<TLegend>> legends;
  std::vector<std::unique_ptr<TLatex>> labels;

  auto plot = [&](TH1* hist_raw, TH1* hist_recentered, TH1* hist_flat, int idx, PlotOptions opts = {})
  {
    c1->cd(idx);
    gPad->SetBottomMargin(opts.margin_bottom);
    gPad->SetLeftMargin(opts.margin_left);
    gPad->SetRightMargin(opts.margin_right);

    hist_raw->Draw("HIST");
    hist_recentered->Draw("HIST same");
    hist_flat->Draw("HIST same");

    hist_raw->SetLineColor(kRed);
    hist_raw->SetLineWidth(opts.line_width);

    hist_recentered->SetLineColor(kBlue);
    hist_recentered->SetLineWidth(opts.line_width);

    hist_flat->SetLineColor(kGreen + 3);
    hist_flat->SetLineWidth(opts.line_width);

    double ymax = hist_raw->GetMaximum() * 1.1;
    opts.ymax = (opts.ymax) ? opts.ymax : ymax;
    hist_raw->GetYaxis()->SetRangeUser(0, opts.ymax);

    hist_raw->GetXaxis()->SetTitle(std::format("{} 2#Psi_{{2}}", opts.det).c_str());

    hist_raw->SetTitle("");
    hist_raw->GetYaxis()->SetTitle("Events");
    hist_raw->GetYaxis()->SetTitleOffset(1.4F);
    hist_raw->GetXaxis()->SetTitleSize(0.05F);
    hist_raw->GetXaxis()->SetLabelSize(0.05F);

    if (!opts.show_ylabels)
    {
      hist_raw->GetYaxis()->SetLabelSize(0.F);
    }

    double xshift = 0.45;
    double yshift = -0.02;

    if (opts.show_legend)
    {
      auto leg = std::make_unique<TLegend>(0.2 + xshift, .75 + yshift, 0.54 + xshift, .95 + yshift);
      leg->SetFillStyle(0);
      leg->SetTextSize(0.05F);
      leg->AddEntry(hist_raw, "Raw", "l");
      leg->AddEntry(hist_recentered, "Recentered", "l");
      leg->AddEntry(hist_flat, "Flat", "l");
      leg->Draw("same");

      legends.push_back(std::move(leg));
    }

    if (opts.show_labels)
    {
      labels.push_back(myUtils::draw_text(0.18, 0.9, "#bf{#it{sPHENIX}} Performance", 0.05F));
      labels.push_back(myUtils::draw_text(0.7, 0.9, "Run 25", 0.05F));
      labels.push_back(myUtils::draw_text(0.5, 0.8, "Au+Au #sqrt{s_{NN}} = 200 GeV", 0.05F));
      labels.push_back(myUtils::draw_text(0.5, 0.7, "Centrality: 0-60%", 0.05F));
    }
  };

  int ctr = 1;

  double ymax = std::max(hPsiS_raw->GetMaximum(), hPsiN_raw->GetMaximum()) * 1.1;

  plot(hPsiS_raw, hPsiS_recentered, hPsiS_flat, ctr++, {.ymax = ymax, .margin_right = 0.F, .show_legend = false, .det = "sEPD South"});
  plot(hPsiN_raw, hPsiN_recentered, hPsiN_flat, ctr, {.ymax = ymax, .margin_left = 0.F, .show_ylabels = false, .show_labels = false, .det = "sEPD North"});

  labels.push_back(myUtils::draw_text(0.75, 0.97, "6/12/2026", 0.05F));

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hPsi-overlay").c_str());

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
    c1->SetTopMargin(.05F);

    auto plotAndSave = [&](std::string_view name = "", const std::string& title = "")
    {
      auto* hist = m_hists[std::string(name)].get();

      hist->Draw("HIST");

      hist->SetTitle(title.c_str());

      hist->SetLineColor(kBlue);
      hist->SetLineWidth(3);

      hist->GetYaxis()->SetTitleOffset(1.2F);

      hist->GetYaxis()->SetTitleSize(0.04F);
      hist->GetXaxis()->SetTitleSize(0.04F);
      hist->GetYaxis()->SetLabelSize(0.04F);
      hist->GetXaxis()->SetLabelSize(0.04F);

      std::vector<std::unique_ptr<TLatex>> labels;
      labels.push_back(myUtils::draw_text(0.5, 0.8, "Run 68144", 0.06F));
      labels.push_back(myUtils::draw_text(0.5, 0.7, "|z| < 10 cm and MB", 0.06F));

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig)
      {
        c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());
      }
    };

    plotAndSave("hCentrality");
  }

  {
    auto* hVtxZ = m_hists["hVtxZ"].get();
    auto* hVtxZ_MB = m_hists["hVtxZ_MB"].get();

    hVtxZ->Draw("HIST");
    hVtxZ_MB->Draw("HIST same");

    hVtxZ->SetTitle("");

    hVtxZ->SetLineColor(kBlue);
    hVtxZ_MB->SetLineColor(kRed);

    hVtxZ->SetLineWidth(3);
    hVtxZ_MB->SetLineWidth(3);

    hVtxZ->GetYaxis()->SetTitleOffset(1.F);

    hVtxZ->GetYaxis()->SetTitleSize(0.04F);
    hVtxZ->GetXaxis()->SetTitleSize(0.04F);
    hVtxZ->GetYaxis()->SetLabelSize(0.04F);
    hVtxZ->GetXaxis()->SetLabelSize(0.04F);

    double xshift = 0.43;
    double yshift = 0.23;

    std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .55 + yshift, 0.5 + xshift, .7 + yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.05F);
    leg->AddEntry(hVtxZ, "All", "l");
    leg->AddEntry(hVtxZ_MB, "Minimum Bias", "l");
    leg->Draw("same");

    std::vector<std::unique_ptr<TLatex>> labels;
    labels.push_back(myUtils::draw_text(0.15, 0.88, "Run 68144", 0.06F));

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig)
    {
      c1->Print(std::format("{}/images/{}.png", m_output_dir, "hZVtx-Overlay").c_str());
    }
  }

  // ---------------------------------------------------

  {
    gPad->SetLogz();
    c1->SetRightMargin(.12F);
    c1->SetLeftMargin(.08F);

    auto* h2SEPD_Charge = dynamic_cast<TH2*>(m_hists["h2SEPD_Charge"].get());

    h2SEPD_Charge->Draw("COLZ1");
    h2SEPD_Charge->SetTitle("");
    h2SEPD_Charge->GetXaxis()->SetMaxDigits(3);
    h2SEPD_Charge->GetYaxis()->SetMaxDigits(3);

    h2SEPD_Charge->GetYaxis()->SetTitle("sEPD Total Charge North");
    h2SEPD_Charge->GetXaxis()->SetTitle("sEPD Total Charge South");

    h2SEPD_Charge->GetYaxis()->SetTitleOffset(0.9F);
    h2SEPD_Charge->GetXaxis()->SetTitleOffset(1.F);

    h2SEPD_Charge->GetYaxis()->SetLabelSize(0.04F);
    h2SEPD_Charge->GetXaxis()->SetLabelSize(0.04F);

    h2SEPD_Charge->GetYaxis()->SetTitleSize(0.04F);
    h2SEPD_Charge->GetXaxis()->SetTitleSize(0.04F);

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
    f1->SetLineStyle(kDashed);
    f1->SetLineWidth(3);

    std::vector<std::unique_ptr<TLatex>> labels;
    labels.push_back(myUtils::draw_text(0.15, 0.9, "Run 68144", 0.06F));
    labels.push_back(myUtils::draw_text(0.15, 0.84, "|z| < 10 cm and MB", 0.06F));

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig)
    {
      c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2SEPD_Charge").c_str());
    }

    gPad->SetLogz(0);
  }

  // ---------------------------------------------------

  {
    c1->SetTopMargin(.03F);
    c1->SetBottomMargin(.12F);
    c1->SetLeftMargin(.1F);
    c1->SetRightMargin(.04F);

    unsigned int bins_calib = 140;
    double calib_low = -1e3;
    double calib_high = 400;

    std::unique_ptr<TH1> hCalib = std::make_unique<TH1F>("hCalib", "; MPV [ADC]; Channels", bins_calib, calib_low, calib_high);

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
      c1->Print(std::format("{}/images/{}.png", m_output_dir, "hSEPD_Calib-full").c_str());
    }

    hCalib->GetXaxis()->SetRangeUser(0,400);

    std::vector<std::unique_ptr<TLatex>> labels;
    labels.push_back(myUtils::draw_text(0.6, 0.88, "Run 71504", 0.06F));

    c1->Print(output.c_str(), "pdf portrait");

    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hSEPD_Calib").c_str());

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

      auto* px = hist->ProfileX("px", 3, -1, "s");

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

      auto* px_south = h2SEPD_South_AvgCharge->ProfileX("px_south", 3, -1, "s");
      auto* px_north = h2SEPD_North_AvgCharge->ProfileX("px_north", 3, -1, "s");

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
