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
#include <TFrame.h>
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
class DisplayQA
{
 public:
  // The constructor takes the configuration
  DisplayQA(std::filesystem::path output_dir, bool saveFig)
    : m_output_dir(std::move(output_dir))
    , m_saveFig(saveFig)
  {
    m_pearson_thresholds["EMCal_MBD"] = 0.95;
    m_pearson_thresholds["EMCal_sEPD"] = 0.98;
    m_pearson_thresholds["IHCal_sEPD"] = 0.98;
    m_pearson_thresholds["OHCal_sEPD"] = 0.97;
    m_pearson_thresholds["EMCal_OHCal"] = 0.98;
    m_pearson_thresholds["IHCal_OHCal"] = 0.98;
    m_pearson_thresholds["sEPD_MBD"] = 0.97;
  }

  void run()
  {
    init_hists();
    draw();
  }

 private:
  // Configuration stored as members
  std::filesystem::path m_output_dir;
  bool m_saveFig{false};
  bool m_savePDF{false};

  // Dynamic storage for scalable QA
  std::unordered_map<std::string, std::unique_ptr<TH1>> m_hists;

  // Configuration for plots
  const std::vector<std::string> m_expected_hists = {
      "hEvent", "hCentrality", "hVtxZ", "hVtxZ_MB",
      "h2EMCal_MBD", "h2IHCal_MBD", "h2OHCal_MBD",
      "h2EMCal_sEPD", "h2IHCal_sEPD", "h2OHCal_sEPD",
      "h2EMCal_OHCal", "h2IHCal_OHCal", "h2SEPD_MBD_Total_Charge",
      "h2EMCal_Energy", "h2OHCal_Energy"
  };

  struct RunInfo
  {
    double avg_z;
    double mb_frac;
    double EMCal_MBD;
    double IHCal_MBD;
    double OHCal_MBD;

    double EMCal_sEPD;
    double IHCal_sEPD;
    double OHCal_sEPD;

    double EMCal_OHCal;
    double IHCal_OHCal;
    double sEPD_MBD;

    int EMCal_BadTowers;
    int OHCal_BadTowers;
  };

  std::map<std::string, RunInfo> m_runInfo;

  // Centralized thresholds
  std::map<std::string, double> m_pearson_thresholds;
  const double m_default_pearson = 0.95;
  const double m_mb_frac_threshold = 0.35;
  const double m_zvtx_threshold = 10.0;
  const int m_bad_tower_emcal_threshold = 15;
  const int m_bad_tower_ohcal_threshold = 10;

  // Helper to get specific or default threshold
  double get_threshold(const std::string& name) const
  {
    return m_pearson_thresholds.count(name) ? m_pearson_thresholds.at(name) : m_default_pearson;
  }

  [[nodiscard]] std::unique_ptr<TLatex>
  draw_text(double x, double y, const std::string& text, float size) const;

  // --- Private Helper Methods ---
  void init_hists();
  void read_hists(const std::filesystem::path& input);
  void draw();

  // Passing the canvas by reference avoids recreating it
  void plot_event(TCanvas& c1, const std::string& run);
  void plot_centrality(TCanvas& c1, const std::string& run) const;
  void plot_zvtx(TCanvas& c1, const std::string& run);
  void plot_calo(TCanvas& c1, const std::string& run);

  void calc_bad_towers(const std::string& run);

  void plot_summary(TCanvas& c1) const;
  void plot_summary_calo(TCanvas& c1) const;
  void plot_summary_bad_towers(TCanvas& c1) const;
  void plot_summary_zvtx(TCanvas& c1) const;
  void plot_summary_mb_frac(TCanvas& c1) const;

  void combine_pdfs() const;

  void write_CSV() const;

  void print_summary_report() const;
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================

std::unique_ptr<TLatex> DisplayQA::draw_text(double x, double y, const std::string& text, float size) const
{
  auto tex = std::make_unique<TLatex>(x, y, text.c_str());
  tex->SetNDC();
  tex->SetTextAlign(22);
  tex->SetTextSize(size);
  tex->Draw();

  return tex;  // Transfers ownership to the calling function
}

void DisplayQA::init_hists()
{
    int bins_pearson = 50;
    double peason_low = 0.5;
    double peason_high = 1;

    auto init_pearson = [&](const std::string& detA, const std::string& detB)
    {
      std::string name = std::format("{}_{}", detA, detB);
      std::string title = std::format("#rho({},{}); #rho; Runs", detA, detB);
      m_hists[name] = std::make_unique<TH1F>(name.c_str(), title.c_str(), bins_pearson, peason_low, peason_high);
    };

    init_pearson("EMCal","MBD");
    init_pearson("IHCal","MBD");
    init_pearson("OHCal","MBD");

    init_pearson("EMCal","sEPD");
    init_pearson("IHCal","sEPD");
    init_pearson("OHCal","sEPD");

    init_pearson("EMCal","OHCal");
    init_pearson("IHCal","OHCal");
    init_pearson("sEPD","MBD");

    int bins_zvtx = 80;
    double zvtx_low = -20;
    double zvtx_high = 20;

    m_hists["Avg_ZVtx"] = std::make_unique<TH1F>("Avg_ZVtx", "; Average Z [cm]; Runs", bins_zvtx, zvtx_low, zvtx_high);

    int bins_mb_frac = 100;
    double mb_frac_low = 0;
    double mb_frac_high = 1;

    m_hists["MB_frac"] = std::make_unique<TH1F>("MB_frac", "; Events(|z| < 10 cm and MB) / Total; Runs", bins_mb_frac, mb_frac_low, mb_frac_high);

    int bins_towers = 1537;

    m_hists["EMCal_BadTowers"] = std::make_unique<TH1F>("EMCal_BadTowers", "; EMCal Retowered Bad Towers; Runs", bins_towers, 0, bins_towers);
    m_hists["OHCal_BadTowers"] = std::make_unique<TH1F>("OHCal_BadTowers", "; OHCal Bad Towers; Runs", bins_towers, 0, bins_towers);
}

void DisplayQA::read_hists(const std::filesystem::path& input)
{
  std::unique_ptr<TFile> tfile(TFile::Open(input.c_str()));
  if (!tfile || tfile->IsZombie())
  {
     throw std::runtime_error(std::format("Error: Could not open ROOT file: {}", input.string()));
  }

  for (const auto& name : m_expected_hists)
  {
    TH1* h = tfile->Get<TH1>(name.c_str());
    if (h)
    {
       // Transfer ownership to the map since TH1::AddDirectory(false) is active
       m_hists[name] = std::unique_ptr<TH1>(h);
    }
    else
    {
       std::cerr << std::format("Warning: Histogram {} not found in {}\n", name, input.string());
    }
  }
}

void DisplayQA::calc_bad_towers(const std::string& run)
{
  auto* hist_emcal = dynamic_cast<TH2*>(m_hists.at("h2EMCal_Energy").get());
  auto* hist_ohcal = dynamic_cast<TH2*>(m_hists.at("h2OHCal_Energy").get());

  int badTowers_emcal = 0;
  int badTowers_ohcal = 0;

  for (int i = 1; i < hist_emcal->GetNbinsX(); ++i)
  {
    for (int j = 1; j < hist_emcal->GetNbinsY(); ++j)
    {
       double val_emcal = hist_emcal->GetBinContent(i, j);
       double val_ohcal = hist_ohcal->GetBinContent(i, j);
       if (val_emcal == 0)
       {
         ++badTowers_emcal;
       }
       if (val_ohcal == 0)
       {
         ++badTowers_ohcal;
       }
    }
  }

  m_hists.at("EMCal_BadTowers")->Fill(badTowers_emcal);
  m_hists.at("OHCal_BadTowers")->Fill(badTowers_ohcal);

  m_runInfo[run].EMCal_BadTowers = badTowers_emcal;
  m_runInfo[run].OHCal_BadTowers = badTowers_ohcal;
}

void DisplayQA::plot_event(TCanvas& c1, const std::string& run)
{
  c1.SetCanvasSize(2000, 1000);
  c1.SetLeftMargin(.07F);
  c1.SetRightMargin(.08F);
  c1.SetTopMargin(.05F);
  c1.SetBottomMargin(.09F);

  gPad->SetTicky(0);

  auto* hist = m_hists.at("hEvent").get();

  hist->Draw("HIST");
  hist->SetTitle("");

  hist->SetLineColor(kBlue);
  hist->SetLineWidth(3);

  hist->GetYaxis()->SetTitleOffset(0.6F);
  hist->GetYaxis()->SetMaxDigits(3);

  double ymax = hist->GetMaximum() * 1.1;
  hist->GetYaxis()->SetRangeUser(0, ymax);

  double totalEvents = hist->GetBinContent(1);

  // 1. Calculate the percentage scale
  double right_axis_max = (ymax / totalEvents) * 100.0;

  // 2. Force a pad update to ensure coordinates are ready
  gPad->Update();

  // 3. Define and draw the TGaxis
  // Parameters: x_start, y_start, x_end, y_end, val_min, val_max, ndiv, options
  std::unique_ptr<TGaxis> axis = std::make_unique<TGaxis>(gPad->GetUxmax(), gPad->GetUymin(),
                                                          gPad->GetUxmax(), gPad->GetUymax(),
                                                          0, right_axis_max, 510, "+L");

  axis->SetTitle("Percentage (%)");
  axis->SetLabelFont(hist->GetYaxis()->GetLabelFont());
  axis->SetLabelSize(hist->GetYaxis()->GetLabelSize());
  axis->SetTitleFont(hist->GetYaxis()->GetTitleFont());
  axis->SetTitleSize(hist->GetYaxis()->GetTitleSize());
  axis->SetTitleOffset(0.78F);  // Adjust so it doesn't overlap the numbers

  axis->Draw();

  std::vector<std::unique_ptr<TLatex>> bin_labels;

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
    TString label = std::format("{:.1f}%", percentage).c_str();

    // Create and style the text object
    auto tex = std::make_unique<TLatex>(xPos, yPos, label);
    tex->SetTextAlign(21);  // Center-aligned horizontally, bottom-aligned vertically
    tex->SetTextSize(0.05F);
    tex->SetTextFont(42);          // Standard Helvetica
    tex->SetTextColor(kBlue + 2);  // Slightly darker blue to stand out
    tex->Draw();

    bin_labels.push_back(std::move(tex));

    std::string label_name = hist->GetXaxis()->GetBinLabel(i);
    if (label_name == "MB")
    {
       m_runInfo[run].mb_frac = percentage / 100.0;
       m_hists.at("MB_frac")->Fill(m_runInfo[run].mb_frac);
    }
  }

  std::vector<std::unique_ptr<TLatex>> labels;
  labels.push_back(draw_text(0.78, 0.87, std::format("Run: {}", run), 0.08F));

  std::string figure = "event";
  std::string hist_name = "hEvent";

  auto output_pdf = m_output_dir / "pdf" / figure / std::format("{}-{}.pdf", hist_name, run);
  auto output_img = m_output_dir / "images" / figure / std::format("{}-{}.png", hist_name, run);

  if(m_savePDF) c1.Print(output_pdf.c_str(), "pdf portrait");
  if (m_saveFig) c1.Print(output_img.c_str());

  gPad->SetTicky(1);

  // Clear the pad so ROOT releases the pointers before the smart pointers destroy them
  c1.Clear();
}

void DisplayQA::plot_centrality(TCanvas& c1, const std::string& run) const
{
  c1.SetCanvasSize(1200, 1000);
  c1.SetLeftMargin(.12F);
  c1.SetRightMargin(.02F);
  c1.SetTopMargin(.05F);
  c1.SetBottomMargin(.09F);

  auto* hist = m_hists.at("hCentrality").get();

  hist->Draw("HIST");

  hist->SetLineColor(kBlue);
  hist->SetLineWidth(3);

  hist->GetXaxis()->SetTitleOffset(0.8F);
  hist->GetYaxis()->SetTitleOffset(1.2F);

  hist->GetYaxis()->SetMaxDigits(3);

  hist->SetTitle("");

  std::vector<std::unique_ptr<TLatex>> labels;
  labels.push_back(draw_text(0.8, 0.88, std::format("Run: {}", run), 0.08F));
  labels.push_back(draw_text(0.8, 0.8, "|z| < 10 cm & MB", 0.06F));

  std::string figure = "centrality";
  std::string hist_name = "hCentrality";

  auto output_pdf = m_output_dir / "pdf" / figure / std::format("{}-{}.pdf", hist_name, run);
  auto output_img = m_output_dir / "images" / figure / std::format("{}-{}.png", hist_name, run);

  if(m_savePDF) c1.Print(output_pdf.c_str(), "pdf portrait");
  if (m_saveFig) c1.Print(output_img.c_str());

  // Clear the pad so ROOT releases the pointers before the smart pointers destroy them
  c1.Clear();
}

void DisplayQA::plot_zvtx(TCanvas& c1, const std::string& run)
{
  c1.SetLeftMargin(.12F);
  c1.SetRightMargin(.03F);
  c1.SetTopMargin(.05F);
  c1.SetBottomMargin(.09F);

  auto* hist = m_hists.at("hVtxZ").get();
  auto* hist_MB = m_hists.at("hVtxZ_MB").get();

  hist->Draw("HIST");
  hist_MB->Draw("HIST same");

  hist->SetLineColor(kBlue);
  hist->SetLineWidth(3);

  hist_MB->SetLineColor(kRed);
  hist_MB->SetLineWidth(3);

  hist->SetTitle("");

  hist->GetXaxis()->SetTitleOffset(0.8F);
  hist->GetYaxis()->SetTitleOffset(1.2F);

  hist->GetYaxis()->SetMaxDigits(3);

  std::vector<std::unique_ptr<TLatex>> labels;
  labels.push_back(draw_text(0.8, 0.88, std::format("Run: {}", run), 0.08F));

  double xshift = 0.55;
  double yshift = 0.12;

  std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .55 + yshift, 0.4 + xshift, .7 + yshift);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.05f);
  leg->AddEntry(hist, "All", "l");
  leg->AddEntry(hist_MB, "MB", "l");
  leg->Draw("same");

  double avg = hist_MB->GetXaxis()->GetBinCenter(hist_MB->GetMaximumBin());
  m_hists.at("Avg_ZVtx")->Fill(avg);
  m_runInfo[run].avg_z = avg;

  // Flag Bad Runs
  if (std::abs(avg) > m_zvtx_threshold)
  {
    labels.push_back(draw_text(0.35, 0.88, std::format("#color[2]{{Avg Z = {:.1f} cm}}", avg), 0.07F));
  }

  gPad->Update(); // Ensure the pad is updated to get correct axis limits
  std::unique_ptr<TLine> line = std::make_unique<TLine>(avg, gPad->GetUymin(), avg, gPad->GetUymax());
  line->SetLineColor(kRed);
  line->SetLineStyle(kDashed);
  line->SetLineWidth(3);
  line->Draw("same");

  std::string figure = "ZVertex";
  std::string hist_name = "hZVertex";

  auto output_pdf = m_output_dir / "pdf" / figure / std::format("{}-{}.pdf", hist_name, run);
  auto output_img = m_output_dir / "images" / figure / std::format("{}-{}.png", hist_name, run);

  if(m_savePDF) c1.Print(output_pdf.c_str(), "pdf portrait");
  if (m_saveFig) c1.Print(output_img.c_str());

  // Clear the pad so ROOT releases the pointers before the smart pointers destroy them
  c1.Clear();
}

void DisplayQA::plot_calo(TCanvas& c1, const std::string& run)
{
  c1.Divide(3,3, 0.00025F, 0.00025F);

  c1.SetCanvasSize(2500, 2000);

  struct PlotOptions
  {
    double xlow{0};
    double xhigh{0};

    double ylow{0};
    double yhigh{0};
  };

  std::vector<std::unique_ptr<TLatex>> labels;

  auto plot = [&](TH2* hist, const std::string& pearson_name, double& pearson_corr, int idx, PlotOptions opts = {})
  {
    c1.cd(idx);
    gPad->SetTopMargin(0.1F);
    gPad->SetLeftMargin(0.15F);
    gPad->SetRightMargin(0.13F);
    gPad->SetLogz();

    hist->Draw("COLZ1");
    hist->SetTitle("");

    hist->GetXaxis()->SetMaxDigits(3);
    hist->GetYaxis()->SetMaxDigits(3);

    hist->GetXaxis()->SetTitleOffset(1.F);
    hist->GetYaxis()->SetTitleOffset(1.2F);

    if (opts.xhigh > opts.xlow)
    {
      hist->GetXaxis()->SetRangeUser(opts.xlow, opts.xhigh);
    }

    if (opts.yhigh > opts.ylow)
    {
      hist->GetYaxis()->SetRangeUser(opts.ylow, opts.yhigh);
    }

    labels.push_back(draw_text(0.5, 0.94, std::format("Run: {}", run), 0.08F));

    pearson_corr = hist->GetCorrelationFactor();

    m_hists.at(pearson_name)->Fill(pearson_corr);
  };

  auto* h2EMCal_MBD = dynamic_cast<TH2*>(m_hists.at("h2EMCal_MBD").get());
  auto* h2IHCal_MBD = dynamic_cast<TH2*>(m_hists.at("h2IHCal_MBD").get());
  auto* h2OHCal_MBD = dynamic_cast<TH2*>(m_hists.at("h2OHCal_MBD").get());

  auto* h2EMCal_sEPD = dynamic_cast<TH2*>(m_hists.at("h2EMCal_sEPD").get());
  auto* h2IHCal_sEPD = dynamic_cast<TH2*>(m_hists.at("h2IHCal_sEPD").get());
  auto* h2OHCal_sEPD = dynamic_cast<TH2*>(m_hists.at("h2OHCal_sEPD").get());

  auto* h2EMCal_OHCal = dynamic_cast<TH2*>(m_hists.at("h2EMCal_OHCal").get());
  auto* h2IHCal_OHCal = dynamic_cast<TH2*>(m_hists.at("h2IHCal_OHCal").get());
  auto* h2SEPD_MBD = dynamic_cast<TH2*>(m_hists.at("h2SEPD_MBD_Total_Charge").get());

  int idx = 1;

  double emcal_low = -1e2;
  double emcal_high = 2e3;

  double ihcal_low = -50;
  double ihcal_high = 2e2;

  double ohcal_low = -50;
  double ohcal_high = 5e2;

  double sepd_high = 2.5e4;

  RunInfo& runInfo = m_runInfo[run];

  plot(h2EMCal_MBD, "EMCal_MBD", runInfo.EMCal_MBD, idx++, {.xlow = emcal_low, .xhigh = emcal_high});
  plot(h2IHCal_MBD, "IHCal_MBD", runInfo.IHCal_MBD, idx++, {.xlow = ihcal_low, .xhigh = ihcal_high});
  plot(h2OHCal_MBD, "OHCal_MBD", runInfo.OHCal_MBD, idx++, {.xlow = ohcal_low, .xhigh = ohcal_high});

  plot(h2EMCal_sEPD, "EMCal_sEPD", runInfo.EMCal_sEPD, idx++, {.xlow = emcal_low, .xhigh = emcal_high, .yhigh = sepd_high});
  plot(h2IHCal_sEPD, "IHCal_sEPD", runInfo.IHCal_sEPD, idx++, {.xlow = ihcal_low, .xhigh = ihcal_high, .yhigh = sepd_high});
  plot(h2OHCal_sEPD, "OHCal_sEPD", runInfo.OHCal_sEPD, idx++, {.xlow = ohcal_low, .xhigh = ohcal_high, .yhigh = sepd_high});

  plot(h2EMCal_OHCal, "EMCal_OHCal", runInfo.EMCal_OHCal, idx++, {.xlow = emcal_low, .xhigh = emcal_high, .ylow = ohcal_low, .yhigh = ohcal_high});
  plot(h2IHCal_OHCal, "IHCal_OHCal", runInfo.IHCal_OHCal, idx++, {.xlow = ihcal_low, .xhigh = ihcal_high, .ylow = ohcal_low, .yhigh = ohcal_high});
  plot(h2SEPD_MBD, "sEPD_MBD", runInfo.sEPD_MBD, idx++, {.yhigh = sepd_high});

  std::string figure = "calo";
  std::string hist_name = "hCalo";

  auto output_pdf = m_output_dir / "pdf" / figure / std::format("{}-{}.pdf", hist_name, run);
  auto output_img = m_output_dir / "images" / figure / std::format("{}-{}.png", hist_name, run);

  if(m_savePDF) c1.Print(output_pdf.c_str(), "pdf portrait");
  if (m_saveFig) c1.Print(output_img.c_str());

  // Clear the pad so ROOT releases the pointers before the smart pointers destroy them
  c1.Clear();
}

void DisplayQA::plot_summary(TCanvas& c1) const
{
  std::string figure = "summary";

  auto img_dir = m_output_dir / "images" / figure;
  auto pdf_dir = m_output_dir / "pdf" / figure;

  std::filesystem::create_directories(img_dir);
  std::filesystem::create_directories(pdf_dir);

  plot_summary_calo(c1);
  plot_summary_bad_towers(c1);
  plot_summary_zvtx(c1);
  plot_summary_mb_frac(c1);
}

void DisplayQA::plot_summary_calo(TCanvas& c1) const
{
  c1.Divide(3,3, 0.00025F, 0.00025F);
  c1.SetCanvasSize(2500, 2400);

  struct PlotOptions
  {
    double xlow{0.8};
    double xhigh{1};

    double ylow{5e-1};
    double yhigh{0};
  };

  std::vector<std::unique_ptr<TLatex>> labels;

  auto plot = [&](const std::string& name, int idx, PlotOptions opts = {})
  {
    c1.cd(idx);
    gPad->SetTopMargin(0.1F);
    gPad->SetLeftMargin(0.07F);
    gPad->SetRightMargin(0.05F);
    gPad->SetLogy();

    auto* hist = m_hists.at(name).get();

    hist->Draw();
    hist->SetLineColor(kBlue);
    hist->SetLineWidth(3);

    hist->GetYaxis()->SetMaxDigits(3);

    hist->GetXaxis()->SetTitleOffset(1.F);
    hist->GetYaxis()->SetTitleOffset(1.F);

    hist->GetXaxis()->SetTitleSize(0.05F);
    hist->GetXaxis()->SetLabelSize(0.04F);

    if (opts.xhigh > opts.xlow)
    {
      hist->GetXaxis()->SetRangeUser(opts.xlow, opts.xhigh);
    }

    if (opts.yhigh > opts.ylow)
    {
      hist->GetYaxis()->SetRangeUser(opts.ylow, opts.yhigh);
    }
    else
    {
      opts.yhigh = hist->GetMaximum() * 2;
      hist->GetYaxis()->SetRangeUser(opts.ylow, opts.yhigh);
    }

    labels.push_back(draw_text(0.5, 0.94, hist->GetTitle(), 0.08F));
    hist->SetTitle("");

    // --- DRAW THRESHOLD LINE ---
    double threshold = get_threshold(name);

    // Get the current pad limits for y-axis
    gPad->Update();
    auto line = std::make_unique<TLine>(threshold, opts.ylow, threshold, opts.yhigh);
    line->SetLineColor(kRed);
    line->SetLineStyle(kDashed);
    line->SetLineWidth(3);
    line->DrawClone("same");  // DrawClone handles memory within the pad
  };

  int idx = 1;
  plot("EMCal_MBD", idx++);
  plot("IHCal_MBD", idx++);
  plot("OHCal_MBD", idx++);
  plot("EMCal_sEPD", idx++);
  plot("IHCal_sEPD", idx++);
  plot("OHCal_sEPD", idx++);
  plot("EMCal_OHCal", idx++);
  plot("IHCal_OHCal", idx++);
  plot("sEPD_MBD", idx++);

  // ----------------------------------------

  std::string figure = "summary";
  std::string hist_name = "Calo-Correlation";

  auto img_dir = m_output_dir / "images" / figure;
  auto pdf_dir = m_output_dir / "pdf" / figure;

  auto output_pdf = pdf_dir / std::format("{}.pdf", hist_name);
  auto output_img = img_dir / std::format("{}.png", hist_name);

  c1.Print(output_pdf.c_str(), "pdf portrait");
  if (m_saveFig) c1.Print(output_img.c_str());

  // Clear the pad so ROOT releases the pointers before the smart pointers destroy them
  c1.Clear();
}

void DisplayQA::plot_summary_bad_towers(TCanvas& c1) const
{
  c1.Divide(2,1, 0.00025F, 0.00025F);
  c1.SetCanvasSize(2500, 1000);

  struct PlotOptions
  {
    double xlow{0.5};
    double xhigh{1537};
    int badTowerThreshold{0};
  };

  auto plot = [&](TH1* hist, int idx, PlotOptions opts = {})
  {
    c1.cd(idx);
    gPad->SetTopMargin(0.05F);
    gPad->SetLeftMargin(0.1F);
    gPad->SetRightMargin(0.05F);

    // 1. Calculate Y limits first
    double ymin = 5e-1;
    double ymax = hist->GetMaximum() * 2;

    // 2. Set Log scales before drawing the frame
    gPad->SetLogy();
    gPad->SetLogx();

    // 3. Draw a Frame to define the exact visual axis range
    TH1* frame = gPad->DrawFrame(opts.xlow, ymin, opts.xhigh, ymax);

    // 4. Style the frame (since the frame now "owns" the axes)
    frame->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
    frame->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
    frame->GetXaxis()->SetTitleOffset(1.4F);
    frame->GetYaxis()->SetTitleOffset(1.F);
    frame->GetXaxis()->SetTitleSize(0.05F);
    frame->GetXaxis()->SetLabelSize(0.05F);
    frame->GetYaxis()->SetMaxDigits(3);

    // This adds extra labels (like 0.5, 2, 5) to make the log scale clearer
    frame->GetXaxis()->SetMoreLogLabels();

    // 5. Draw your histogram on top of the frame
    hist->SetLineColor(kBlue);
    hist->SetLineWidth(3);
    hist->Draw("HIST SAME");

    gPad->Update();

    // 6. Draw the threshold line
    auto line = std::make_unique<TLine>(opts.badTowerThreshold, ymin, opts.badTowerThreshold, ymax);
    line->SetLineColor(kRed);
    line->SetLineStyle(kDashed);
    line->SetLineWidth(3);
    line->DrawClone("same");
  };

  auto* hist_emcal = m_hists.at("EMCal_BadTowers").get();
  auto* hist_ohcal = m_hists.at("OHCal_BadTowers").get();

  int idx = 1;
  plot(hist_emcal, idx++, {.badTowerThreshold = m_bad_tower_emcal_threshold});
  plot(hist_ohcal, idx++, {.badTowerThreshold = m_bad_tower_ohcal_threshold});

  std::string figure = "summary";
  std::string hist_name = "Calo-BadTowers";

  auto img_dir = m_output_dir / "images" / figure;
  auto pdf_dir = m_output_dir / "pdf" / figure;

  auto output_pdf = pdf_dir / std::format("{}.pdf", hist_name);
  auto output_img = img_dir / std::format("{}.png", hist_name);

  c1.Print(output_pdf.c_str(), "pdf portrait");
  if (m_saveFig) c1.Print(output_img.c_str());

  gPad->SetLogx(0);

  // Clear the pad so ROOT releases the pointers before the smart pointers destroy them
  c1.Clear();
}

void DisplayQA::plot_summary_zvtx(TCanvas& c1) const
{
  c1.SetCanvasSize(1200, 1000);
  c1.SetLeftMargin(.07F);
  c1.SetRightMargin(.03F);
  c1.SetTopMargin(.05F);
  c1.SetBottomMargin(.11F);

  gPad->SetLogy();

  auto* hist = m_hists.at("Avg_ZVtx").get();

  hist->Draw();
  hist->SetLineColor(kBlue);
  hist->SetLineWidth(3);

  hist->GetYaxis()->SetMaxDigits(3);

  hist->GetXaxis()->SetTitleOffset(1.F);
  hist->GetYaxis()->SetTitleOffset(1.F);

  hist->GetXaxis()->SetTitleSize(0.05F);
  hist->GetXaxis()->SetLabelSize(0.05F);

  double ymin = 5e-1;
  double ymax = hist->GetMaximum() * 2;
  hist->GetYaxis()->SetRangeUser(ymin, ymax);

  gPad->Update(); // Ensure the pad is updated to get correct axis limits

  std::unique_ptr<TLine> line_low = std::make_unique<TLine>(-m_zvtx_threshold, ymin, -m_zvtx_threshold, ymax);
  line_low->SetLineColor(kRed);
  line_low->SetLineStyle(kDashed);
  line_low->SetLineWidth(3);
  line_low->Draw("same");

  std::unique_ptr<TLine> line_high = std::make_unique<TLine>(m_zvtx_threshold, ymin, m_zvtx_threshold, ymax);
  line_high->SetLineColor(kRed);
  line_high->SetLineStyle(kDashed);
  line_high->SetLineWidth(3);
  line_high->Draw("same");

  std::string figure = "summary";
  std::string hist_name = "Avg-ZVtx";

  auto img_dir = m_output_dir / "images" / figure;
  auto pdf_dir = m_output_dir / "pdf" / figure;

  auto output_pdf = pdf_dir / std::format("{}.pdf", hist_name);
  auto output_img = img_dir / std::format("{}.png", hist_name);

  c1.Print(output_pdf.c_str(), "pdf portrait");
  if (m_saveFig) c1.Print(output_img.c_str());

  // Clear the pad so ROOT releases the pointers before the smart pointers destroy them
  c1.Clear();
}

void DisplayQA::plot_summary_mb_frac(TCanvas& c1) const
{
  c1.SetCanvasSize(1200, 1000);
  c1.SetLeftMargin(.07F);
  c1.SetRightMargin(.03F);
  c1.SetTopMargin(.05F);
  c1.SetBottomMargin(.11F);

  gPad->SetLogy();

  auto* hist = m_hists.at("MB_frac").get();

  hist->Draw();
  hist->SetLineColor(kBlue);
  hist->SetLineWidth(3);

  hist->GetYaxis()->SetMaxDigits(3);

  hist->GetXaxis()->SetTitleOffset(1.F);
  hist->GetYaxis()->SetTitleOffset(1.F);

  hist->GetXaxis()->SetTitleSize(0.05F);
  hist->GetXaxis()->SetLabelSize(0.05F);

  double ymin = 5e-1;
  double ymax = hist->GetMaximum() * 2;
  hist->GetYaxis()->SetRangeUser(ymin, ymax);

  gPad->Update(); // Ensure the pad is updated to get correct axis limits

  std::unique_ptr<TLine> line = std::make_unique<TLine>(m_mb_frac_threshold, ymin, m_mb_frac_threshold, ymax);
  line->SetLineColor(kRed);
  line->SetLineStyle(kDashed);
  line->SetLineWidth(3);
  line->Draw("same");

  std::string figure = "summary";
  std::string hist_name = "MB-Frac";

  auto img_dir = m_output_dir / "images" / figure;
  auto pdf_dir = m_output_dir / "pdf" / figure;

  auto output_pdf = pdf_dir / std::format("{}.pdf", hist_name);
  auto output_img = img_dir / std::format("{}.png", hist_name);

  c1.Print(output_pdf.c_str(), "pdf portrait");
  if (m_saveFig) c1.Print(output_img.c_str());

  // Clear the pad so ROOT releases the pointers before the smart pointers destroy them
  c1.Clear();
}

void DisplayQA::combine_pdfs() const
{
  std::cout << "\n" << std::string(50, '=') << "\n";
  std::cout << "Starting Batched PDF Merging (Ordered)\n";
  std::cout << std::string(50, '=') << std::endl;

  std::filesystem::path merged_pdf_dir = m_output_dir / "pdf" / "merged";
  std::filesystem::create_directories(merged_pdf_dir);

  std::map<std::string, std::string> prefix_map = {{"event", "hEvent"}, {"centrality", "hCentrality"}, {"ZVertex", "hZVertex"}, {"calo", "hCalo"}};

  const std::vector<std::string> hist_types = {"event", "centrality", "ZVertex", "calo"};
  const int batch_size = 100;  // Merge 100 files at a time

  for (const auto& type : hist_types)
  {
    std::vector<std::string> all_files;
    for (const auto& [run, info] : m_runInfo)
    {
       std::filesystem::path src = m_output_dir / "pdf" / type / std::format("{}-{}.pdf", prefix_map.at(type), run);
       if (std::filesystem::exists(src)) all_files.push_back(src.string());
    }

    if (all_files.empty()) continue;

    std::cout << std::format("Processing '{}' ({} files)...\n", type, all_files.size());
    std::vector<std::string> intermediate_pdfs;

    // 1. Merge in Batches
    for (size_t i = 0; i < all_files.size(); i += batch_size)
    {
       size_t end = std::min(i + batch_size, all_files.size());
       std::string batch_list;
       for (size_t j = i; j < end; ++j) batch_list += all_files[j] + " ";

       std::string part_name = (merged_pdf_dir / std::format("{}_part_{}.pdf", type, i / batch_size)).string();
       std::string cmd = std::format("gs -dBATCH -dNOPAUSE -q -sDEVICE=pdfwrite -sOutputFile={} {}", part_name, batch_list);

       std::cout << std::format("  -> Batch {}: Runs {}-{} ", i / batch_size, i, end - 1) << std::flush;
       std::system(cmd.c_str());
       std::cout << "[DONE]" << std::endl;

       intermediate_pdfs.push_back(part_name);
    }

    // 2. Final Merge of Intermediate Files
    std::string final_output = (merged_pdf_dir / std::format("{}.pdf", type)).string();
    std::string final_list;
    for (const auto& f : intermediate_pdfs) final_list += f + " ";

    std::cout << "  -> Creating Final Merged File... " << std::flush;
    std::string final_cmd = std::format("gs -dBATCH -dNOPAUSE -q -sDEVICE=pdfwrite -sOutputFile={} {}", final_output, final_list);
    std::system(final_cmd.c_str());
    std::cout << "[COMPLETE]" << std::endl;

    // 3. Cleanup Intermediates
    for (const auto& f : intermediate_pdfs) std::filesystem::remove(f);
  }
}

void DisplayQA::write_CSV() const
{
  std::filesystem::path qa_file_path = m_output_dir / "QA.csv";

  // Open the CSV file for writing
  std::ofstream qa_file(qa_file_path);

  // Check if the file opened successfully
  if (!qa_file.is_open())
  {
    std::cout << "Error: Could not open CSV for writing!" << std::endl;
    return;
  }

  qa_file << "Run,Avg_Z,ZVtx_Status,MB_frac,MB_frac_Status,"
             "EMCal_BadTowers,EMCal_BadTowers_Status,OHCal_BadTowers,OHCal_BadTowers_Status,"
             "EMCal_MBD,IHCal_MBD,OHCal_MBD,"
             "EMCal_sEPD,IHCal_sEPD,OHCal_sEPD,"
             "EMCal_OHCal,IHCal_OHCal,sEPD_MBD,Calo_Status,Overall_Status" << std::endl;

  for (const auto& [run, info] : m_runInfo)
  {
    // Z-Vertex status based on the 10cm threshold used in plot_zvtx
    std::string zvtx_stat = (std::abs(info.avg_z) < m_zvtx_threshold) ? "GOOD" : "BAD";

    // All coefficients must be > threshold to pass
    bool calo_ok = (info.EMCal_MBD > get_threshold("EMCal_MBD") &&
                    info.IHCal_MBD > get_threshold("IHCal_MBD") &&
                    info.OHCal_MBD > get_threshold("OHCal_MBD") &&
                    info.EMCal_sEPD > get_threshold("EMCal_sEPD") &&
                    info.IHCal_sEPD > get_threshold("IHCal_sEPD") &&
                    info.OHCal_sEPD > get_threshold("OHCal_sEPD") &&
                    info.EMCal_OHCal > get_threshold("EMCal_OHCal") &&
                    info.IHCal_OHCal > get_threshold("IHCal_OHCal") &&
                    info.sEPD_MBD   > get_threshold("sEPD_MBD"));

    std::string calo_stat = calo_ok ? "GOOD" : "BAD";

    std::string mb_frac_stat = (info.mb_frac >= m_mb_frac_threshold) ? "GOOD" : "BAD";

    std::string badTower_emcal_stat = (info.EMCal_BadTowers < m_bad_tower_emcal_threshold) ? "GOOD" : "BAD";
    std::string badTower_ohcal_stat = (info.OHCal_BadTowers < m_bad_tower_ohcal_threshold) ? "GOOD" : "BAD";

    std::string overall_stat = (badTower_emcal_stat == "GOOD" && badTower_ohcal_stat == "GOOD" && zvtx_stat == "GOOD" &&
                                calo_stat == "GOOD" && mb_frac_stat == "GOOD") ? "GOOD" : "BAD";

    // 3. Write formatted row
    qa_file << std::format("{},{:.3f},{},{:.3f},{},{},{},{},{},{:.3f},{:.3f},{:.3f},{:.3f},{:.3f},{:.3f},{:.3f},{:.3f},{:.3f},{},{}\n",
                           run,
                           info.avg_z, zvtx_stat, info.mb_frac, mb_frac_stat,
                           info.EMCal_BadTowers, badTower_emcal_stat,
                           info.OHCal_BadTowers, badTower_ohcal_stat,
                           info.EMCal_MBD, info.IHCal_MBD, info.OHCal_MBD,
                           info.EMCal_sEPD, info.IHCal_sEPD, info.OHCal_sEPD,
                           info.EMCal_OHCal, info.IHCal_OHCal, info.sEPD_MBD, calo_stat, overall_stat);
  }

  qa_file.close();
  std::cout << "QA Summary written to: " << qa_file_path << std::endl;
}

void DisplayQA::print_summary_report() const
{
  int total_runs = static_cast<int>(m_runInfo.size());
  int good_runs = 0;

  // Independent failure counters
  int failed_z = 0;
  int failed_calo = 0;
  int failed_mb = 0;
  int failed_bad_emcal_towers = 0;
  int failed_bad_ohcal_towers = 0;

  for (const auto& [run, info] : m_runInfo)
  {
    bool z_ok    = (std::abs(info.avg_z) < m_zvtx_threshold);
    bool mb_ok   = (info.mb_frac >= m_mb_frac_threshold);
    // All coefficients must be > threshold to pass
    bool calo_ok = (info.EMCal_MBD > get_threshold("EMCal_MBD") &&
                    info.IHCal_MBD > get_threshold("IHCal_MBD") &&
                    info.OHCal_MBD > get_threshold("OHCal_MBD") &&
                    info.EMCal_sEPD > get_threshold("EMCal_sEPD") &&
                    info.IHCal_sEPD > get_threshold("IHCal_sEPD") &&
                    info.OHCal_sEPD > get_threshold("OHCal_sEPD") &&
                    info.EMCal_OHCal > get_threshold("EMCal_OHCal") &&
                    info.IHCal_OHCal > get_threshold("IHCal_OHCal") &&
                    info.sEPD_MBD   > get_threshold("sEPD_MBD"));

    bool badTowers_emcal_ok = (info.EMCal_BadTowers < m_bad_tower_emcal_threshold);
    bool badTowers_ohcal_ok = (info.OHCal_BadTowers < m_bad_tower_ohcal_threshold);

    if (z_ok && calo_ok && mb_ok && badTowers_emcal_ok && badTowers_ohcal_ok)
    {
       good_runs++;
    }
    else
    {
       // Record why the run was bad (a run can have multiple reasons)
       if (!z_ok)    failed_z++;
       if (!calo_ok) failed_calo++;
       if (!mb_ok)   failed_mb++;
       if (!badTowers_emcal_ok) failed_bad_emcal_towers++;
       if (!badTowers_ohcal_ok) failed_bad_ohcal_towers++;
    }
  }

  // Print the Table
  std::cout << "\n" << std::string(55, '=') << "\n";
  std::cout << std::format("{:^55}\n", "FINAL QA SUMMARY REPORT");
  std::cout << std::string(55, '=') << "\n";
  std::cout << std::format("{:<40} : {:>10}\n", "Total Runs Processed", total_runs);
  std::cout << std::format("{:<40} : {:>10}\n", "Overall GOOD Runs (Passed All)", good_runs);
  std::cout << std::format("{:<40} : {:>10}\n", "Overall BAD Runs", total_runs - good_runs);
  std::cout << std::string(55, '-') << "\n";
  std::cout << std::format("{:^55}\n", "PRIMARY REASONS FOR FAILURE");
  std::cout << std::format("{:<40} : {:>10}\n", "Failed Z-Vertex (|Avg Z| > 10 cm)", failed_z);
  std::cout << std::format("{:<40} : {:>10}\n", "Failed Calorimeter (Pearson Threshold)", failed_calo);
  std::cout << std::format("{:<40} : {:>10}\n", "Failed EMCal Bad Towers (> 15)", failed_bad_emcal_towers);
  std::cout << std::format("{:<40} : {:>10}\n", "Failed OHCal Bad Towers (> 10)", failed_bad_ohcal_towers);
  std::cout << std::format("{:<40} : {:>10}\n", "Failed MB Fraction (< 35%)", failed_mb);
  std::cout << std::string(55, '=') << "\n\n";
}

void DisplayQA::draw()
{
  // set sPHENIX plotting style
  SetsPhenixStyle();
  TH1::AddDirectory(false);

  TCanvas c1("c1");
  c1.SetTickx();
  c1.SetTicky();

  c1.SetCanvasSize(1200, 1000);
  c1.SetLeftMargin(.12F);
  c1.SetRightMargin(.02F);
  c1.SetTopMargin(.11F);
  c1.SetBottomMargin(.09F);

  gStyle->SetOptTitle(1);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleFontSize(0.08F);
  gStyle->SetTitleW(1);
  gStyle->SetTitleH(0.08F);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);

  std::filesystem::path input_list = "/gpfs02/sphenix/user/anarde/sEPD-Study/f4a/07-05-26/test.list";

  std::vector<std::string> hist_types = {"event", "centrality", "ZVertex", "calo"};
  // std::vector<std::string> hist_types = {};

  // Create directories
  for (const auto& type : hist_types)
  {
     std::filesystem::path img_dir = m_output_dir / "images" / type;
     std::filesystem::path pdf_dir = m_output_dir / "pdf" / type;

     std::filesystem::create_directories(img_dir);
     std::filesystem::create_directories(pdf_dir);
  }

  if (std::ifstream file{input_list})
  {
    std::string line;

    int test_ctr = 3000;
    int run_ctr = 0;

    while (std::getline(file, line) && run_ctr < test_ctr)
    {
      std::string run = std::filesystem::path(line).stem().string();
      std::cout << std::format("Read: {}, Run: {}\n", line, run);

      read_hists(line);

      plot_event(c1, run);
      plot_centrality(c1, run);
      plot_zvtx(c1, run);
      plot_calo(c1, run);
      calc_bad_towers(run);

      ++run_ctr;
    }
  }
  else
  {
    std::cout << "Could not open the file!\n";
  }

  // combine_pdfs();          // Merge Run-by-Run QA
  plot_summary(c1);        // Generate final plots
  write_CSV();             // Write the detailed per-run file
  print_summary_report();  // Print the final console breakdown
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
    DisplayQA analysis(output_dir, saveFig);
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
