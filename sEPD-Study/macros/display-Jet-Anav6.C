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

// ====================================================================
// The Analysis Class
// ====================================================================
class DisplayJetAnav6
{
 public:
  // The constructor takes the configuration
  DisplayJetAnav6(std::string output_dir, bool saveFig)
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
void DisplayJetAnav6::read_hists()
{
  std::string base_path_jetAna = "/gpfs02/sphenix/user/anarde/sEPD-Study/jetAna";
  std::string base_path_f4a = "/gpfs02/sphenix/user/anarde/sEPD-Study/f4a";

  std::string input = std::format("{}/07-08-26-r03/test.root", base_path_jetAna);
  std::string input_f4a = std::format("{}/07-05-26/test.root", base_path_f4a);
  std::string input_68144 = std::format("{}/07-07-26/merged/output/68144.root", base_path_jetAna);

  std::unordered_set<std::string> names = {"hCentrality", "h2CaloECentrality_default", "h2CaloECentrality", "h2CentralityCaloV2",
                                           "h2CaloV2JetPt", "h2CentralityJetPt",
                                           "hJetPt", "hJetPtv2", "hJetPtv3",
                                           "hJetPt_raw", "hJetPtv2_raw", "hJetPtv3_raw",
                                           "h2ScalarProduct_2_pt_15_20", "h2ScalarProduct_2_pt_20_25", "h2ScalarProduct_2_pt_25_30",
                                           "h2ScalarProduct_2_pt_30_35", "h2ScalarProduct_2_pt_35_40", "h2ScalarProduct_2_pt_40_45",
                                           "h2ScalarProduct_2_pt_45_50", "h2RefFlow_2",
                                           "h2CaloV2JetPt", "h2CentralityJetPt",
                                           "h2EvtRes_2",
                                           "h2_sEPD_Psi_S_2_raw", "h2_sEPD_Psi_N_2_raw",
                                           "h2_sEPD_Psi_S_2_recentered", "h2_sEPD_Psi_N_2_recentered",
                                           "h2_sEPD_Psi_S_2_flat", "h2_sEPD_Psi_N_2_flat"};

  std::unordered_set<std::string> names_f4a = {"hEvent", "hEventMinBias", "hCentrality"};

  std::unordered_set<std::string> names_68144 = {"h2JetPhiEta_f4a", "h2Weights", "h2Weights_closure", "h2EvtRes_2",
                                                 "h2_sEPD_Psi_S_2_raw", "h2_sEPD_Psi_N_2_raw",
                                                 "h2_sEPD_Psi_S_2_recentered", "h2_sEPD_Psi_N_2_recentered",
                                                 "h2_sEPD_Psi_S_2_flat", "h2_sEPD_Psi_N_2_flat"};

  m_hists = myUtils::read_hists(input, "", &names);

  m_hists.merge(myUtils::read_hists(input_f4a, "_f4a", &names_f4a));

  m_hists.merge(myUtils::read_hists(input_68144, "_68144", &names_68144));
}

void DisplayJetAnav6::draw()
{
  // set sPHENIX plotting style
  SetsPhenixStyle();

  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>("c1");
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(2000, 1000);
  c1->SetLeftMargin(.07F);
  c1->SetRightMargin(.005F);
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

  auto margins_TH1 = [&]()
  {
    c1->SetLeftMargin(.12F);
    c1->SetRightMargin(.03F);
    c1->SetTopMargin(.11F);
    c1->SetBottomMargin(.09F);
  };

  auto margins_TH2 = [&]()
  {
    c1->SetLeftMargin(.09F);
    c1->SetRightMargin(.11F);
    c1->SetTopMargin(.08F);
    c1->SetBottomMargin(.09F);
  };

  auto resetCanvas = [&]()
  {
    c1->Clear();
    c1->SetCanvasSize(1200, 1000);
    c1->SetLeftMargin(.12F);
    c1->SetRightMargin(.02F);
    c1->SetTopMargin(.11F);
    c1->SetBottomMargin(.09F);
  };

  // ---------------------------------------------------

  struct PlotOptionsTH2
  {
    float yoffset{1.F};
    bool show_default_title{true};
    double ylow{0};
    double yhigh{0};
    double zlow{0};
    int ydigits{0};
    bool add_tprof{false};
  };

  auto plotAndSaveTH2 = [&](TH2* hist, const std::string name, PlotOptionsTH2 opts = {})
  {
    hist->Draw("COLZ1");
    hist->GetYaxis()->SetRange(0,0);

    hist->GetYaxis()->SetTitleOffset(opts.yoffset);

    if (!opts.show_default_title)
    {
      hist->SetTitle("");
    }

    if(opts.add_tprof)
    {
      auto* hProf = hist->ProfileX();
      hProf->SetLineColor(kRed);
      hProf->SetMarkerColor(kRed);
      hProf->SetLineWidth(3);

      hProf->Draw("same");
    }

    if (opts.yhigh > opts.ylow)
    {
      hist->GetYaxis()->SetRangeUser(opts.ylow, opts.yhigh);
    }

    if (opts.ydigits)
    {
      hist->GetYaxis()->SetMaxDigits(opts.ydigits);
    }

    if (opts.zlow != 0)
    {
      hist->SetMinimum(opts.zlow);
    }
    else
    {
      hist->SetMinimum(1);
    }

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());
  };

  // ---------------------------------------------------

  struct PlotOptionsTH1
  {
    std::string draw_type{"HIST"};
    short int color{kBlue};
    short int width{3};
    std::string ytitle{};
    float yoffset{1.F};
    double xlow{0};
    double xhigh{0};
    double ylow{0};
    double yhigh{0};
    int maxDigits{3};
  };

  auto plotAndSaveTH1 = [&](TH1* hist, const std::string name, PlotOptionsTH1 opts = {})
  {
    hist->Draw(opts.draw_type.c_str());

    hist->GetYaxis()->SetTitleOffset(opts.yoffset);
    hist->SetLineColor(opts.color);
    hist->SetMarkerColor(opts.color);
    hist->SetMarkerStyle(kFullDotLarge);
    hist->SetLineWidth(opts.width);

    hist->GetYaxis()->SetMaxDigits(opts.maxDigits);

    if (!opts.ytitle.empty())
    {
      hist->GetYaxis()->SetTitle(opts.ytitle.c_str());
    }

    if (opts.xhigh > opts.xlow)
    {
      hist->GetXaxis()->SetRangeUser(opts.xlow, opts.xhigh);
    }

    if (opts.yhigh > opts.ylow)
    {
      hist->GetYaxis()->SetRangeUser(opts.ylow, opts.yhigh);
    }

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());
  };

  // ---------------------------------------------------

  {
    auto* hEvent_f4a = m_hists["hEvent_f4a"].get();

    int bins_event = hEvent_f4a->GetNbinsX()-2;

    m_hists["hEvent"] = std::make_unique<TH1F>("hEvent", "Event Type; Type; Events", bins_event, 0, bins_event);
    auto* hEvent = m_hists["hEvent"].get();

    for (int i = 1; i <= bins_event; ++i)
    {
      int f4a_bin = i;
      // skip SEPD & EP bins
      if (i > 6)
      {
        f4a_bin = i + 2;
      }

      std::string label = hEvent_f4a->GetXaxis()->GetBinLabel(f4a_bin);
      double val = hEvent_f4a->GetBinContent(f4a_bin);

      hEvent->GetXaxis()->SetBinLabel(i, label.c_str());
      hEvent->SetBinContent(i, val);
    }

    struct PlotOptions
    {
      double totalEvents{0};
      int bins{0};
      int maxDigits{3};
      float labelSize{0.05F};
      std::string title{""};
      float rightMargin{0.1F};
      float topMargin{0.11F};
      float xoffset{0.8F};
      float yoffset{0.6F};
      float zoffset{0.7F};
    };

    auto plotAndSave = [&](TH1* hist, const PlotOptions &opts = {}, std::string_view name = "")
    {
      c1->SetTopMargin(opts.topMargin);
      c1->SetRightMargin(opts.rightMargin);

      gPad->SetTicky(0);

      hist->Draw("HIST");

      hist->SetTitle(opts.title.c_str());
      hist->GetYaxis()->SetMaxDigits(opts.maxDigits);
      hist->GetXaxis()->SetLabelSize(opts.labelSize);

      int bins = (opts.bins) ? opts.bins : hist->GetNbinsX();
      hist->GetXaxis()->SetRange(1, bins);

      hist->SetLineColor(kBlue);
      hist->SetLineWidth(3);

      double ymax = hist->GetMaximum() * 1.1;
      hist->GetYaxis()->SetRangeUser(0, ymax);

      hist->GetXaxis()->SetTitleOffset(opts.xoffset);
      hist->GetYaxis()->SetTitleOffset(opts.yoffset);

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
      axis->SetTitleOffset(opts.zoffset);  // Adjust so it doesn't overlap the numbers

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

    auto* hEventMinBias = m_hists["hEventMinBias_f4a"].get();

    plotAndSave(hEvent, {.totalEvents = hEvent->GetBinContent(1), .rightMargin = 0.07F, .topMargin = 0.07F}, "hEvent");

    resetCanvas();

    plotAndSave(hEventMinBias, {.totalEvents = hEventMinBias->Integral(), .maxDigits = 0, .labelSize = 0.04F,
                                .title = "|z| < 10 cm and Fail MB",
                                .xoffset=1.1F, .yoffset=1.5F, .zoffset=1.1F}, "hEventMinBias");
  }

  // ---------------------------------------------------

  {
    auto* hCentrality = m_hists["hCentrality_f4a"].get();
    plotAndSaveTH1(hCentrality, "hCentrality", {.yoffset = 1.5F});
  }

  // ---------------------------------------------------

  {
    gPad->SetLogz();
    margins_TH2();

    auto* h2CaloECentrality_default = dynamic_cast<TH2*>(m_hists["h2CaloECentrality_default"].get());
    auto* h2CaloECentrality = dynamic_cast<TH2*>(m_hists["h2CaloECentrality"].get());
    auto* h2CentralityCaloV2 = dynamic_cast<TH2*>(m_hists["h2CentralityCaloV2"].get());
    auto* h2CaloV2JetPt = dynamic_cast<TH2*>(m_hists["h2CaloV2JetPt"].get());
    auto* h2CentralityJetPt = dynamic_cast<TH2*>(m_hists["h2CentralityJetPt"].get());

    double all_evts = h2CaloECentrality_default->Integral();
    double filtered_evts = h2CaloECentrality->Integral();
    double frac_cut = (all_evts-filtered_evts) / all_evts * 100;

    double all_evts_calov2 = h2CentralityCaloV2->Integral();
    double threshold = 0.2;
    int bin_low = h2CentralityCaloV2->GetYaxis()->FindBin(-threshold)+1;
    int bin_high = h2CentralityCaloV2->GetYaxis()->FindBin(threshold)-1;
    double threshold_evts_calov2 = h2CentralityCaloV2->Integral(1,-1, bin_low, bin_high);
    double frac_cut_calov2 = (all_evts_calov2-threshold_evts_calov2)/ all_evts * 100;

    double all_jets = h2CaloV2JetPt->Integral();
    double threshold_v2 = 0.48;
    int bin_low_v2 = h2CaloV2JetPt->GetXaxis()->FindBin(-threshold_v2)+1;
    int bin_high_v2 = h2CaloV2JetPt->GetXaxis()->FindBin(threshold_v2)-1;
    double threshold_jets = h2CaloV2JetPt->Integral(bin_low_v2, bin_high_v2, 1, -1);
    double frac_jets_bad_calov2 = (all_jets-threshold_jets)/ all_jets * 100;

    std::cout << std::format("{:#<20}\n", "");
    std::cout << "CaloE vs Centrality Filter" << std::endl;
    std::cout << std::format("Before: {}, After: {}, Cut Frac: {:.2f}%\n", all_evts, filtered_evts, frac_cut);
    std::cout << std::format("{:#<20}\n", "");

    std::cout << "Calo V2 vs Centrality Stats" << std::endl;
    std::cout << std::format("All: {}, |v2| < 0.2: {}, Cut Frac: {:.2f}%\n", all_evts_calov2, threshold_evts_calov2, frac_cut_calov2);
    std::cout << std::format("{:#<20}\n", "");

    std::cout << "Calo V2 vs Jet pT Stats" << std::endl;
    std::cout << std::format("All: {}, |v2| < 0.48: {}, Cut Frac: {:.4f}%\n", all_jets, threshold_jets, frac_jets_bad_calov2);
    std::cout << std::format("{:#<20}\n", "");

    plotAndSaveTH2(h2CaloECentrality_default, "h2CaloECentrality_default");
    plotAndSaveTH2(h2CaloECentrality, "h2CaloECentrality");
    plotAndSaveTH2(h2CentralityCaloV2, "h2CentralityCaloV2", {.show_default_title = false, .add_tprof=true});
    plotAndSaveTH2(h2CentralityCaloV2, "h2CentralityCaloV2-zoom", {.show_default_title = false, .yhigh = 0.03, .add_tprof=true});
    plotAndSaveTH2(h2CaloV2JetPt, "h2CaloV2JetPt", {.yoffset = 1.2F, .show_default_title = false});
    plotAndSaveTH2(h2CaloV2JetPt, "h2CaloV2JetPt-zoom", {.yoffset = 1.2F, .show_default_title = false, .yhigh = 70});
    plotAndSaveTH2(h2CentralityJetPt, "h2CentralityJetPt", {.yoffset = 1.2F, .show_default_title = false, .yhigh = 200});
    plotAndSaveTH2(h2CentralityJetPt, "h2CentralityJetPt-zoom", {.yoffset = 1.2F, .show_default_title = false, .yhigh = 70});
  }

  // ---------------------------------------------------

  {
    margins_TH1();
    c1->SetTopMargin(.03F);
    gPad->SetLogy();

    auto* hJetPt = m_hists["hJetPt"].get();
    auto* hJetPtv2 = m_hists["hJetPtv2"].get();
    auto* hJetPtv3 = m_hists["hJetPtv3"].get();

    auto* hJetPt_raw = m_hists["hJetPt_raw"].get();
    auto* hJetPtv2_raw = m_hists["hJetPtv2_raw"].get();
    auto* hJetPtv3_raw = m_hists["hJetPtv3_raw"].get();

    hJetPt->Draw("HIST");
    hJetPtv2->Draw("same HIST");
    hJetPtv3->Draw("same HIST");

    hJetPt_raw->Draw("same HIST");
    hJetPtv2_raw->Draw("same HIST");
    hJetPtv3_raw->Draw("same HIST");

    hJetPt->SetLineColor(kRed);
    hJetPtv2->SetLineColor(kBlue);
    hJetPtv3->SetLineColor(kGreen+3);

    hJetPt_raw->SetLineColor(kRed);
    hJetPtv2_raw->SetLineColor(kBlue);
    hJetPtv3_raw->SetLineColor(kGreen+3);

    hJetPt->SetLineWidth(3);
    hJetPtv2->SetLineWidth(3);
    hJetPtv3->SetLineWidth(3);

    hJetPt_raw->SetLineWidth(3);
    hJetPtv2_raw->SetLineWidth(3);
    hJetPtv3_raw->SetLineWidth(3);

    hJetPt_raw->SetLineStyle(kDashed);
    hJetPtv2_raw->SetLineStyle(kDashed);
    hJetPtv3_raw->SetLineStyle(kDashed);

    hJetPt->SetTitle("");

    hJetPt->GetYaxis()->SetTitle("Jets / 1 GeV");
    hJetPt->GetYaxis()->SetTitleOffset(1.1F);
    hJetPt->GetXaxis()->SetTitleOffset(1.1F);

    double ymax = hJetPt->GetMaximum()*2;
    double ymin = 5e-1;
    hJetPt->GetYaxis()->SetRangeUser(ymin, ymax);

    double xshift = 0.2;
    double yshift = 0.25;

    std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .5 + yshift, 0.4 + xshift, .7 + yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04F);
    leg->AddEntry(hJetPt, "All", "l");
    leg->AddEntry(hJetPtv2, "Energy > 0 GeV", "l");
    leg->AddEntry(hJetPtv3, "|calo v_{2}| < 0.48", "l");
    leg->Draw("same");

    std::vector<std::unique_ptr<TLatex>> labels;
    double lx = 0.7;
    double ly = 0.9;

    std::vector<std::string> lines = {
        "Events Selections",
        "- |z| < 10 cm",
        "- MB",
        "- Centrality: 0-60%",
        "- Good Calo-Cent",
        "- No Flow Failure"
    };

    float line_spacing = 0.06F;
    for (size_t i = 0; i < lines.size(); ++i)
    {
      labels.push_back(myUtils::draw_text(lx, ly - (static_cast<float>(i) * line_spacing), lines[i], 0.04F));
    }

    double jets_20GeV_above = hJetPtv3->Integral(hJetPtv3->FindBin(20), -1);
    double jets_40GeV_above = hJetPtv3->Integral(hJetPtv3->FindBin(40), -1);

    std::cout << std::format("Jet pT >= 20 GeV: {}, Jet pT >= 40 GeV: {}\n", jets_20GeV_above, jets_40GeV_above);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hJetPt-overlay").c_str());

    hJetPt->GetXaxis()->SetRangeUser(0, 100);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hJetPt-overlay-zoom").c_str());

    gPad->SetLogy(0);
  }

  // ---------------------------------------------------

  {
    margins_TH1();
    c1->SetTopMargin(.06F);

    auto* h2RefFlow = dynamic_cast<TH2*>(m_hists["h2RefFlow_2"].get());

    auto* hRefFlow = h2RefFlow->ProfileX()->ProjectionX();

    auto computeRefFlow = [&](TH1* hist, const std::string name)
    {
      for (int bin = 1; bin <= hist->GetNbinsX(); ++bin)
      {
        double val = hist->GetBinContent(bin);
        double error = hist->GetBinError(bin);

        double new_val = std::sqrt(val);
        double new_error = error / (2 * new_val);

        hist->SetBinContent(bin, new_val);
        hist->SetBinError(bin, new_error);
      }

      m_hists[name] = std::unique_ptr<TH1>(hist);
    };

    computeRefFlow(hRefFlow, "hRefFlow");

    // hRefFlow->Draw();
    // hRefFlow->SetLineColor(kBlue);
    // hRefFlow->SetMarkerColor(kBlue);
    // hRefFlow->SetMarkerStyle(kFullDotLarge);

    // hRefFlow->GetYaxis()->SetMaxDigits(3);

    double ymax = hRefFlow->GetMaximum() * 1.1;
    // hRefFlow->GetYaxis()->SetRangeUser(0, ymax);

    std::string ytitle = "#sqrt{#LT" + std::string(h2RefFlow->GetYaxis()->GetTitle()) + "#GT}";
    // hRefFlow->GetYaxis()->SetTitle(ytitle.c_str());
    // hRefFlow->GetYaxis()->SetTitleOffset(1.F);

    plotAndSaveTH1(hRefFlow, "hRefFlow", {.draw_type="", .ytitle=ytitle, .yhigh=ymax});

    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hRefFlow").c_str());
  }

  // ---------------------------------------------------

  {
    margins_TH2();
    c1->SetTopMargin(0.09F);
    c1->SetLeftMargin(1.4F);

    auto* h2RefFlow = dynamic_cast<TH2*>(m_hists["h2RefFlow_2"].get());

    plotAndSaveTH2(h2RefFlow, "h2RefFlow", {.yoffset = 1.4F, .ydigits = 3, .add_tprof = true});
    plotAndSaveTH2(h2RefFlow, "h2RefFlow-zoom", {.yoffset = 1.3F, .yhigh = 4e-4, .ydigits = 3, .add_tprof = true});

    for (const auto& type : {"15_20", "20_25", "25_30", "30_35", "35_40", "40_45", "45_50"})
    {
      std::string name = std::format("h2ScalarProduct_2_pt_{}", type);
      std::string name_zoom = std::format("h2ScalarProduct_2_pt_{}-zoom", type);
      auto* h2 = dynamic_cast<TH2*>(m_hists[name].get());

      plotAndSaveTH2(h2, name, {.yoffset = 1.3F, .ydigits = 3, .add_tprof = true});
      plotAndSaveTH2(h2, name_zoom, {.yoffset = 1.3F, .yhigh = 8e-3, .ydigits = 3, .add_tprof = true});
    }
  }

  // ---------------------------------------------------

  {
    margins_TH1();
    c1->SetTopMargin(.09F);

    struct PlotOptions
    {
      std::string ytitle{};
      double yhigh{8e-3};
      float yoffset{1.F};
    };

    auto plotAndSave = [&](TH1* hist, const std::string& name, PlotOptions opts = {})
    {
      hist->Draw();

      hist->SetLineColor(kBlue);
      hist->SetMarkerColor(kBlue);
      hist->SetMarkerStyle(kFullDotLarge);
      hist->SetLineWidth(3);

      hist->GetYaxis()->SetMaxDigits(3);
      hist->GetYaxis()->SetTitle(opts.ytitle.c_str());
      hist->GetYaxis()->SetTitleOffset(opts.yoffset);

      gPad->Update();  // Forces ROOT to generate the title object

      TPaveText* title = dynamic_cast<TPaveText*>(gPad->GetPrimitive("title"));
      if (title)
      {
        title->SetX1NDC(0.1);  // Left edge of the title box
        // title->SetX2NDC(0.9);  // Right edge of the title box
        gPad->Modified();      // Tell the pad something changed
        gPad->Update();        // Redraw
      }

      if (opts.yhigh)
      {
        hist->GetYaxis()->SetRangeUser(0, opts.yhigh);
      }
      else
      {
        double ymax = hist->GetMaximum() * 1.1;
        hist->GetYaxis()->SetRangeUser(0, ymax);
      }

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());
    };


    auto* hRefFlow = m_hists["hRefFlow"].get();

    for (const auto& type : {"15_20", "20_25", "25_30", "30_35", "35_40", "40_45", "45_50"})
    {
      std::string name = std::format("h2ScalarProduct_2_pt_{}", type);
      auto* h2 = dynamic_cast<TH2*>(m_hists[name].get());
      h2->GetYaxis()->SetRange(0,0);

      std::string ytitle = h2->GetYaxis()->GetTitle();

      auto* hist = h2->ProfileX();

      std::string name_v2 = std::format("hJetV2_pt_{}", type);
      auto* hist_v2 = hist->ProjectionX(name_v2.c_str());

      hist_v2->Divide(hRefFlow);

      ytitle = "#LT" + ytitle + "#GT";
      name = std::format("hScalarProduct-pt-{}", type);

      plotAndSave(hist, name, {.ytitle = ytitle});
      plotAndSave(hist_v2, name_v2, {.ytitle = "Jet v_{2}", .yhigh = 0.5, .yoffset = 1.2F});
    }
  }

  // ---------------------------------------------------

  {
    gPad->SetLogz();
    margins_TH2();

    auto* h2EvtRes = dynamic_cast<TH2*>(m_hists["h2EvtRes_2_68144"].get());
    plotAndSaveTH2(h2EvtRes, "h2EvtRes", {.ylow = -1, .yhigh = 1, .zlow = 1e2, .add_tprof = true});

    c1->Clear();
    c1->Divide(3, 2, 0.00025F, 0.00025F);
    c1->SetCanvasSize(2500, 1600);

    h2EvtRes->GetYaxis()->SetRange(0,0);
    std::vector<std::unique_ptr<TLatex>> labels;
    std::vector<std::unique_ptr<TLine>> lines;

    double ymin = 0;
    double ymax = 2e4;

    for (int idx = 1; idx <= h2EvtRes->GetNbinsX(); ++idx)
    {
      c1->cd(idx);

      std::string name = std::format("h2EvtRes_{}", idx);
      auto* h1EvtRes = h2EvtRes->ProjectionY(name.c_str(), idx, idx);

      h1EvtRes->Draw("HIST");
      h1EvtRes->SetLineColor(kBlue);
      h1EvtRes->SetLineWidth(3);

      h1EvtRes->GetYaxis()->SetRangeUser(ymin, ymax);
      h1EvtRes->GetYaxis()->SetTitle("Events");
      h1EvtRes->GetYaxis()->SetMaxDigits(3);

      h1EvtRes->GetXaxis()->SetLabelSize(0.05F);
      h1EvtRes->GetXaxis()->SetTitleSize(0.05F);

      std::string cent_label = std::format("{}-{}%", (idx - 1) * 10, idx * 10);
      labels.push_back(myUtils::draw_text(0.25, 0.88, cent_label, 0.08F));

      double avg = h1EvtRes->GetMean();

      std::unique_ptr<TLine> line = std::make_unique<TLine>(avg, ymin, avg, ymax);
      line->SetLineColor(kRed);
      line->SetLineStyle(kDashed);
      line->SetLineWidth(3);
      line->Draw("same");

      lines.push_back(std::move(line));
    }

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h1EvtRes").c_str());

    gPad->SetLogy(0);

    resetCanvas();
  }

  // ---------------------------------------------------

  {
    c1->Clear();
    c1->Divide(2, 1, 0.00025F, 0.00025F);
    c1->SetCanvasSize(2500, 1200);

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
      float margin_left{0.12F};
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

      hist_raw->SetTitle("");
      hist_raw->GetYaxis()->SetTitle("Events");
      hist_raw->GetYaxis()->SetTitleOffset(1.2F);
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

      labels.push_back(myUtils::draw_text(0.25, 0.88, opts.det, 0.08F));

      if (opts.show_labels)
      {
        labels.push_back(myUtils::draw_text(0.53, 0.8, "Au+Au #sqrt{s_{NN}} = 200 GeV", 0.045F));
        // labels.push_back(myUtils::draw_text(0.53, 0.7, "|z| < 10 cm & MB", 0.06F));
        labels.push_back(myUtils::draw_text(0.53, 0.7, "Centrality: 0-60%", 0.06F));
      }
    };

    int ctr = 1;

    double ymax = std::max(hPsiS_raw->GetMaximum(), hPsiN_raw->GetMaximum()) * 1.1;

    plot(hPsiS_raw, hPsiS_recentered, hPsiS_flat, ctr++, {.ymax = ymax, .margin_right = 0.F, .show_legend = false, .det = "sEPD South"});
    plot(hPsiN_raw, hPsiN_recentered, hPsiN_flat, ctr, {.ymax = ymax, .margin_left = 0.F, .show_ylabels = false, .show_labels = false, .det = "sEPD North"});

    labels.push_back(myUtils::draw_text(0.75, 0.97, "mm/dd/yyyy", 0.05F));

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hPsi-overlay").c_str());

    resetCanvas();
  }

  // ---------------------------------------------------

  {
    c1->Clear();
    c1->SetCanvasSize(2900, 1400);
    c1->SetLeftMargin(.06F);
    c1->SetRightMargin(.1F);
    c1->SetTopMargin(.04F);
    c1->SetBottomMargin(.09F);

    gPad->SetLogz(0);

    auto* h2Weights = dynamic_cast<TH2*>(m_hists["h2Weights_68144"].get());
    for (int i = 1; i <= h2Weights->GetNbinsX(); ++i)
    {
      for (int j = 1; j <= h2Weights->GetNbinsY(); ++j)
      {
        double error = h2Weights->GetBinError(i, j);
        if (error == 0)
        {
          h2Weights->SetBinContent(i, j, 0);
        }
      }
    }

    struct PlotOptions
    {
      double zmax{0};
    };

    auto plotAndSave = [&](TH2* hist, const std::string& name, PlotOptions opts = {})
    {
      hist->Draw("COLZ1");

      hist->SetTitle("Run 68144");
      gPad->Update();

      TPaveText* title = dynamic_cast<TPaveText*>(gPad->GetPrimitive("title"));
      if (title)
      {
        // Set the vertical bounds (Y1 = bottom of box, Y2 = top of box)
        title->SetY1NDC(0.85);
        title->SetY2NDC(0.95);
        gPad->Modified();
        gPad->Update();
      }

      hist->GetYaxis()->SetTitleOffset(0.4F);
      hist->GetXaxis()->SetTitleOffset(0.6F);

      hist->GetYaxis()->SetTitleSize(0.06F);
      hist->GetXaxis()->SetTitleSize(0.06F);

      hist->GetYaxis()->SetLabelSize(0.04F);
      hist->GetXaxis()->SetLabelSize(0.04F);

      hist->GetZaxis()->SetMaxDigits(3);

      if (opts.zmax)
      {
        hist->SetMaximum(opts.zmax);
      }

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());
    };

    auto* h2JetPhiEta = dynamic_cast<TH2*>(m_hists["h2JetPhiEta_f4a_68144"].get());
    auto* h2Weights_closure = dynamic_cast<TH2*>(m_hists["h2Weights_closure_68144"].get());

    plotAndSave(h2JetPhiEta, "h2JetPhiEta-68144");
    plotAndSave(h2Weights, "h2Weights-68144");
    plotAndSave(h2Weights_closure, "h2Weights_closure-68144");
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
    DisplayJetAnav6 analysis(output_dir, saveFig);
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
