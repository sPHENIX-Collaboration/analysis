// ====================================================================
// sPHENIX Includes
// ====================================================================
#include <sPhenixStyle.C>

// ====================================================================
// ROOT Includes
// ====================================================================
#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TH3.h>
#include <TKey.h>
#include <TLegend.h>
#include <TLine.h>
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
class DisplayQVec
{
 public:
  // The constructor takes the configuration
  DisplayQVec(std::string input_file, std::string output_dir)
    : m_input_file(std::move(input_file))
    , m_output_dir(std::move(output_dir))
  {
  }

  void run()
  {
    read_hists();
    draw();
  }

 private:
  // Configuration stored as members
  std::string m_input_file;
  std::string m_output_dir;
  std::map<std::string, std::unique_ptr<TH1>> m_hists;

  bool m_saveFig{true};
  int m_cent_bins{8};
  double m_cent_high{79.5};

  enum class Subdetector
  {
    S,
    N
  };

  static constexpr std::array<int, 3> m_harmonics = {2, 3, 4};
  static constexpr std::array<Subdetector, 2> m_subdetectors = {Subdetector::S, Subdetector::N};

  // --- Private Helper Methods ---
  void read_hists();
  void draw();
  void plot_overall_psi(TCanvas* c1, int order, const std::string& side, const std::string& output);
  void plot_psi(TCanvas* c1, const std::string& name, int order, const std::string& side, short int color, const std::string& output);
  void plot_val1(TCanvas* c1, const std::string& name, const std::string& name_corr, const std::string& output);
  void plot_val2(TCanvas* c1, const std::string& side, int order, const std::string& output);
  double get_average(TH1* hist);
  double max_distance_from_average(TH1* hist, double avg);
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void DisplayQVec::read_hists()
{
  TH1::AddDirectory(kFALSE);

  auto file = std::unique_ptr<TFile>(TFile::Open(m_input_file.c_str()));

  // Check if the file was opened successfully.
  if (!file || file->IsZombie())
  {
    std::cout << std::format("Error: Could not open file '{}'\n", m_input_file);
    return;
  }

  // Get the list of keys (objects) in the file
  TList* keyList = file->GetListOfKeys();
  if (!keyList)
  {
    std::cout << std::format("Error: Could not get list of keys\n");
    return;
  }

  // Use a TIter to loop through the keys
  TIter next(keyList);
  TKey* key;

  std::cout << std::format("Histograms found in {}\n", m_input_file);

  while ((key = static_cast<TKey*>(next())))
  {
    // Get the class of the object from its key
    TClass* cl = gROOT->GetClass(key->GetClassName());

    // Check if the class inherits from TH1 (base histogram class)
    if (cl && cl->InheritsFrom("TH1"))
    {
        std::string name = key->GetName();
        std::cout << std::format("Reading Hist: {}\n", name);
        m_hists[name] = std::unique_ptr<TH1>(static_cast<TH1*>(file->Get(name.c_str())));
    }
  }
}

void DisplayQVec::plot_overall_psi(TCanvas* c1, int order, const std::string& side, const std::string& output)
{
  std::string name = std::format("h3_sEPD_Psi_{}", order);
  std::string direction = (side == "x") ? "South" : "North";
  std::string title = std::format("sEPD {} #Psi_{{{}}}: |z| < 10 cm and MB", direction, order);

  auto h = dynamic_cast<TH3*>(m_hists[name].get())->Project3D(side.c_str());
  auto h_corr = dynamic_cast<TH3 *>(m_hists[std::format("{}_corr", name)].get())->Project3D(side.c_str());
  auto h_corr2 = dynamic_cast<TH3 *>(m_hists[std::format("{}_corr2", name)].get())->Project3D(side.c_str());

  h->Draw();
  h_corr->Draw("same");
  h_corr2->Draw("same");

  h->SetLineColor(kRed);
  h_corr->SetLineColor(kBlue);
  h_corr2->SetLineColor(kGreen+3);

  h->SetTitle(title.c_str());
  h->GetYaxis()->SetMaxDigits(3);
  h->GetYaxis()->SetTitle("Events");
  h->GetYaxis()->SetTitleOffset(1.1f);
  h->GetXaxis()->SetTitleOffset(1.f);
  h->GetYaxis()->SetRangeUser(0, h->GetMaximum()*1.1);

  double xshift = 0.25;
  double yshift = 0;

  std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.06f);
  leg->AddEntry(h, "Default", "l");
  leg->AddEntry(h_corr, "1^{st} Order Corrected", "l");
  leg->AddEntry(h_corr2, "2^{nd} Order Corrected", "l");
  leg->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  if(m_saveFig) c1->Print(std::format("{}/images/{}-{}-overlay.png", m_output_dir, name, direction).c_str());
}

double DisplayQVec::get_average(TH1* hist)
{
    double avg = 0;

    for(int bin = 1; bin <= hist->GetNbinsX(); ++bin)
    {
        avg += hist->GetBinContent(bin);
    }

    return avg / hist->GetNbinsX();
}

double DisplayQVec::max_distance_from_average(TH1* hist, double avg)
{
    double dist = 0;

    for(int bin = 1; bin <= hist->GetNbinsX(); ++bin)
    {
        double val = hist->GetBinContent(bin);
        dist = std::max(dist, std::fabs(avg-val));
    }

    return dist;
}

void DisplayQVec::plot_psi(TCanvas* c1, const std::string& name, int order, const std::string& side, short int color,  const std::string& output)
{
    std::string direction = (side == "x") ? "South" : "North";
    std::string title = std::format("sEPD {} #Psi_{{{}}}: |z| < 10 cm and MB", direction, order);

    auto h = dynamic_cast<TH3*>(m_hists[name].get());

    // identify max
    double max = 0;
    for (int cent_bin = 1; cent_bin <= m_cent_bins; ++cent_bin)
    {
        h->GetZaxis()->SetRange(cent_bin, cent_bin);
        auto* hx = h->Project3D(side.c_str());

        double average = get_average(hx);
        double dist = max_distance_from_average(hx, average) / average;
        max = std::max(max, dist);
    }

    for (int cent_bin = 1; cent_bin <= m_cent_bins; ++cent_bin)
    {
      h->GetZaxis()->SetRange(cent_bin, cent_bin);
      std::string hist_name = std::format("{}", cent_bin);
      auto hx = dynamic_cast<TH1*>(h->Project3D(side.c_str())->Clone(hist_name.c_str()));

      double average = get_average(hx);

      hx->Scale(1./average);

      if (cent_bin == 1)
      {
        hx->Draw("HIST");
        hx->SetTitle(title.c_str());
        hx->GetYaxis()->SetTitle("Normalized Events");
        hx->GetYaxis()->SetTitleOffset(1.3f);
        hx->GetXaxis()->SetTitleOffset(1.f);
        hx->GetYaxis()->SetMaxDigits(3);

        double low = std::max(0., 1-max*1.1);
        double high = 1+max*1.1;
        hx->GetYaxis()->SetRangeUser(low, high);
      }
      else
      {
        hx->Draw("HIST same");
      }

      float alpha = static_cast<float>(1.1 - 0.1 * cent_bin);
      hx->SetLineColorAlpha(color, alpha);
    }

  c1->Print(output.c_str(), "pdf portrait");
  if(m_saveFig) c1->Print(std::format("{}/images/{}-{}.png", m_output_dir, name, side).c_str());
}

void DisplayQVec::plot_val1(TCanvas* c1, const std::string& name, const std::string& name_corr, const std::string& output)
{
  c1->SetLeftMargin(.08f);
  c1->SetRightMargin(.03f);

  auto h = m_hists[name].get();
  auto h2 = m_hists[name_corr].get();

  h->Draw("p e X0");
  h2->Draw("same p e X0");

  h->SetLineColor(kRed);
  h2->SetLineColor(kBlue);

  h->SetMarkerStyle(kFullDotLarge);
  h2->SetMarkerStyle(kFullDotLarge);

  h->SetMarkerColor(kRed);
  h2->SetMarkerColor(kBlue);

  if (h->GetBinContent(m_cent_bins) < 0)
  {
    h->GetYaxis()->SetRangeUser(h->GetMinimum() * 1.1, std::fabs(h->GetMinimum() * 0.35));
  }
  else{
    h->GetYaxis()->SetRangeUser(-h->GetMaximum() * 0.15, std::fabs(h->GetMaximum() * 1.1));
  }
  h->GetYaxis()->SetMaxDigits(1);

  h->GetYaxis()->SetTitleOffset(1.f);
  h->GetXaxis()->SetTitleOffset(1.f);

  double xshift = -0.1;
  double yshift = 0.05;

  std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.06f);
  leg->AddEntry(h, "Default", "pe");
  leg->AddEntry(h2, "Corrected", "pe");
  leg->Draw("same");

  std::unique_ptr<TLine> line = std::make_unique<TLine>(0, 0, m_cent_high, 0);

  line->SetLineColor(kBlack);
  line->SetLineStyle(kDashed);
  line->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());

  h2->Draw("HIST P");
  double max_dev = std::max(fabs(h2->GetMaximum()), fabs(h2->GetMinimum()))*1.5;

  h2->GetYaxis()->SetRangeUser(-max_dev, max_dev);
  h2->GetYaxis()->SetTitleOffset(1.f);

  line->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}-zoom.png", m_output_dir, name).c_str());
}

void DisplayQVec::plot_val2(TCanvas* c1, const std::string& side, int order, const std::string& output)
{
  // c1->SetLeftMargin(.11f);
  // c1->SetRightMargin(.03f);

  std::string namex = std::format("h_sEPD_Q_{}_xx_{}_avg", side, order);
  std::string namey = std::format("h_sEPD_Q_{}_yy_{}_avg", side, order);
  std::string namex_corr = std::format("h_sEPD_Q_{}_xx_{}_corr_avg", side, order);
  std::string namey_corr = std::format("h_sEPD_Q_{}_yy_{}_corr_avg", side, order);

  auto hx = m_hists[namex].get();
  auto hy = m_hists[namey].get();
  auto hx_corr = m_hists[namex_corr].get();
  auto hy_corr = m_hists[namey_corr].get();

  hx->Divide(hy);
  hx_corr->Divide(hy_corr);

  hx->Draw("HIST P");
  hx_corr->Draw("same HIST P");

  hx->SetLineColor(kRed);
  hx_corr->SetLineColor(kBlue);

  hx->SetMarkerStyle(kFullDotLarge);
  hx_corr->SetMarkerStyle(kFullDotLarge);

  hx->SetMarkerColor(kRed);
  hx_corr->SetMarkerColor(kBlue);

  std::string ytitle = std::format("<Q_{{{},x}}^{{2}}> / <Q_{{{},y}}^{{2}}>", order, order);
  hx->GetYaxis()->SetTitle(ytitle.c_str());
  hx->GetYaxis()->SetRangeUser(0, 1.4);

  hx->GetYaxis()->SetMaxDigits(1);

  hx->GetYaxis()->SetTitleOffset(1.f);
  hx->GetXaxis()->SetTitleOffset(1.f);

  double xshift = -0.1;
  double yshift = -0.5;

  std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.06f);
  leg->AddEntry(hx, "Default", "p");
  leg->AddEntry(hx_corr, "Corrected", "p");
  leg->Draw("same");

  std::unique_ptr<TLine> line = std::make_unique<TLine>(0, 1, m_cent_high, 1);

  line->SetLineColor(kBlack);
  line->SetLineStyle(kDashed);
  line->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, namex).c_str());

  hx_corr->Draw("HIST P");
  double max_dev = std::max(fabs(hx_corr->GetMaximum())-1, 1-fabs(hx_corr->GetMinimum()))*1.5;

  hx_corr->GetYaxis()->SetTitle(ytitle.c_str());
  hx_corr->GetYaxis()->SetTitleOffset(1.f);
  hx_corr->GetXaxis()->SetTitleOffset(1.f);

  hx_corr->GetYaxis()->SetRangeUser(1-max_dev, 1+max_dev);
  hx_corr->GetYaxis()->SetTitleOffset(1.f);
  hx_corr->GetYaxis()->SetMaxDigits(3);

  line->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}-zoom.png", m_output_dir, namex).c_str());
}

void DisplayQVec::draw()
{
  // set sPHENIX plotting style
  SetsPhenixStyle();

  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>("c1");
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(1200, 1000);
  c1->SetLeftMargin(.12f);
  c1->SetRightMargin(.02f);
  c1->SetTopMargin(.11f);
  c1->SetBottomMargin(.09f);

  gStyle->SetOptTitle(1);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleFontSize(0.08f);
  gStyle->SetTitleW(1);
  gStyle->SetTitleH(0.08f);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);

  std::string output;

  // create output directory
  std::filesystem::create_directories(m_output_dir);
  std::filesystem::create_directories(m_output_dir + "/images");
  output = std::format("{}/plots.pdf", m_output_dir);

  c1->Print((output + "[").c_str(), "pdf portrait");

  // Centrality
  m_hists["h_Cent"]->Draw();
  m_hists["h_Cent"]->SetTitle("Run 68144: |z| < 10 cm and MB");
  m_hists["h_Cent"]->GetYaxis()->SetTitle("Events");
  m_hists["h_Cent"]->GetXaxis()->SetTitle("Centrality [%]");
  m_hists["h_Cent"]->GetYaxis()->SetRangeUser(0, m_hists["h_Cent"]->GetMaximum()*1.1);
  m_hists["h_Cent"]->GetYaxis()->SetMaxDigits(4);
  m_hists["h_Cent"]->SetLineColor(kBlue);
  m_hists["h_Cent"]->SetLineWidth(3);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/Centrality.png", m_output_dir).c_str());

  // Overall Summary Plots
  for (int n : m_harmonics)
  {
    plot_overall_psi(c1.get(), n, "x", output);
  }

  // Individual
  for (int n : m_harmonics)
  {
    for (auto det : m_subdetectors)
    {
      std::string side = (det == Subdetector::S) ? "x" : "y";
      plot_psi(c1.get(), std::format("h3_sEPD_Psi_{}", n), n, side, kRed, output);
      plot_psi(c1.get(), std::format("h3_sEPD_Psi_{}_corr", n), n, side, kBlue, output);
      plot_psi(c1.get(), std::format("h3_sEPD_Psi_{}_corr2", n), n, side, kGreen + 3, output);
    }
  }

  std::vector<short int> colors{kRed, kBlue, kGreen+3};

  // Coefficient of Variation
  for (auto det : m_subdetectors)
  {
    std::string det_name = (det == Subdetector::S) ? "South" : "North";
    size_t ctr = 0;

    double xshift = 0.25;
    double yshift = 0;

    std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.06f);

    for (int n : m_harmonics)
    {
      std::string name = std::format("h_sEPD_CV_{}_{}", det_name, n);
      std::string title = std::format("sEPD {}: Coefficient of Variation", det_name);
      short int color = colors[ctr];

      auto hist = m_hists[name].get();

      if(ctr == 0)
      {
        hist->Draw("E2");
        hist->SetTitle(title.c_str());
        hist->GetYaxis()->SetRangeUser(0, 0.02);
      }
      else
      {
        hist->Draw("E2 same");
      }

      hist->SetLineColor(color);
      hist->SetMarkerColor(color);
      hist->SetFillColorAlpha(color, 0.2f);
      hist->SetMarkerStyle(kFullDotLarge);
      // hist->SetLineWidth(3);

      leg->AddEntry(hist, std::format("Order: {}", n).c_str(), "f");
      ++ctr;
    }

    leg->Draw("same");
    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print(std::format("{}/images/Psi-CV-{}-overlay.png", m_output_dir, det_name).c_str());
  }

  // // Val 1
  // for (int n : m_harmonics)
  // {
  //     plot_val1(c1.get(), std::format("h_sEPD_Q_S_{}_2_avg", n), std::format("h_sEPD_Q_S_x_{}_corr_avg", n), output);
  //     plot_val1(c1.get(), std::format("h_sEPD_Q_S_{}_2_avg", n), std::format("h_sEPD_Q_S_y_{}_corr_avg", n), output);
  //     plot_val1(c1.get(), std::format("h_sEPD_Q_N_{}_2_avg", n), std::format("h_sEPD_Q_N_x_{}_corr_avg", n), output);
  //     plot_val1(c1.get(), std::format("h_sEPD_Q_N_{}_2_avg", n), std::format("h_sEPD_Q_N_y_{}_corr_avg", n), output);
  // }

  // // Val 1 pt 2
  // for (int n : m_harmonics)
  // {
  //     plot_val1(c1.get(),std::format("h_sEPD_Q_S_xy_{}_avg", n),std::format("h_sEPD_Q_S_xy_{}_corr_avg", n), output);
  //     plot_val1(c1.get(),std::format("h_sEPD_Q_N_xy_{}_avg", n),std::format("h_sEPD_Q_N_xy_{}_corr_avg", n), output);
  // }

  // // 2nd Order Validation
  // for (int n : m_harmonics)
  // {
  //   plot_val2(c1.get(), "S", n, output);
  //   plot_val2(c1.get(), "N", n, output);
  // }

  c1->Print((output + "]").c_str(), "pdf portrait");

  // --------------------------------------

  c1 = std::make_unique<TCanvas>("c2");
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(2200, 1000);
  c1->Divide(4, 2, 0.00025f, 0.00025f);

  output = std::format("{}/plots-Psi.pdf", m_output_dir);

  c1->Print((output + "[").c_str(), "pdf portrait");

  for (int n : m_harmonics)
  {
    std::string name = std::format("h3_sEPD_Psi_{}_corr2", n);
    auto h3 = dynamic_cast<TH3*>(m_hists[name].get());

    for (auto det : m_subdetectors)
    {
      std::string side = (det == Subdetector::S) ? "x" : "y";
      std::string det_name = (det == Subdetector::S) ? "South" : "North";

      // identify max
      double max = 0;
      for (int cent_bin = 1; cent_bin <= m_cent_bins; ++cent_bin)
      {
        h3->GetZaxis()->SetRange(cent_bin, cent_bin);
        auto* hist = h3->Project3D(side.c_str());

        double average = get_average(hist);
        double dist = max_distance_from_average(hist, average) / average;
        max = std::max(max, dist);
      }

      for (int cent_bin = 1; cent_bin <= m_cent_bins; ++cent_bin)
      {
          c1->cd(cent_bin);

          gPad->SetLeftMargin(.14f);
          gPad->SetRightMargin(.05f);
          gPad->SetTopMargin(.1f);
          gPad->SetBottomMargin(.1f);

          h3->GetZaxis()->SetRange(cent_bin, cent_bin);
          std::string hist_name = std::format("{}", cent_bin);
          std::string title = std::format("sEPD {} #Psi_{{{}}}: {}-{}%", det_name, n, (cent_bin-1)*10, cent_bin*10);
          auto hist = dynamic_cast<TH1*>(h3->Project3D(side.c_str())->Clone(hist_name.c_str()));

          double average = get_average(hist);
          hist->Scale(1. / average);

          hist->Draw("HIST");
          hist->SetTitle(title.c_str());
          hist->GetYaxis()->SetTitle(std::format("Normalized Events", hist->GetNbinsX()).c_str());
          hist->GetYaxis()->SetTitleOffset(1.4f);
          hist->GetXaxis()->SetTitleOffset(1.f);
          hist->GetYaxis()->SetMaxDigits(3);

          double low = std::max(0., 1 - max * 1.1);
          double high = 1 + max * 1.1;
          hist->GetYaxis()->SetRangeUser(low, high);
          hist->SetLineColor(kBlue);
      }

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/Psi-corr2-{}-{}.png", m_output_dir, n, det_name).c_str());
    }
  }

  c1->Print((output + "]").c_str(), "pdf portrait");
}

int main(int argc, const char* const argv[])
{
  gROOT->SetBatch(true);

  if (argc < 2 || argc > 3)
  {
    std::cout << "Usage: " << argv[0] << " <input_file> [output_directory]" << std::endl;
    return 1;
  }

  const std::string input_file = argv[1];
  std::string output_dir = (argc >= 3) ? argv[2] : ".";

  try
  {
    DisplayQVec analysis(input_file, output_dir);
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
