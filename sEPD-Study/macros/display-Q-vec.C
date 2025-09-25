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

  // --- Private Helper Methods ---
  void read_hists();
  void draw();
  void plot_overall_psi(TCanvas* c1, int order, const std::string& side, const std::string& output);
  void plot_psi(TCanvas* c1, const std::string& name, int order, const std::string& side, short int color, const std::string& output);
  void plot_val1(TCanvas* c1, const std::string& name, const std::string& name_corr, const std::string& output);
  void plot_val2(TCanvas* c1, const std::string& side, int order, const std::string& output);
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
        max = std::max(max, hx->GetMaximum());
    }

    for (int cent_bin = 1; cent_bin <= m_cent_bins; ++cent_bin)
    {
      h->GetZaxis()->SetRange(cent_bin, cent_bin);
      std::string hist_name = std::format("{}", cent_bin);
      auto hx = dynamic_cast<TH1*>(h->Project3D(side.c_str())->Clone(hist_name.c_str()));
      if (cent_bin == 1)
      {
        hx->Draw();
        hx->SetTitle(title.c_str());
        hx->GetYaxis()->SetTitle("Events");
        hx->GetYaxis()->SetTitleOffset(1.f);
        hx->GetXaxis()->SetTitleOffset(1.f);
        hx->GetYaxis()->SetMaxDigits(3);
        hx->GetYaxis()->SetRangeUser(0, max*1.1);
      }
      else
      {
        hx->Draw("same");
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

  // Overall Summary Plots
  plot_overall_psi(c1.get(), 2, "x", output);
  plot_overall_psi(c1.get(), 2, "y", output);
  plot_overall_psi(c1.get(), 3, "x", output);
  plot_overall_psi(c1.get(), 3, "y", output);
  plot_overall_psi(c1.get(), 4, "x", output);
  plot_overall_psi(c1.get(), 4, "y", output);

  // Individual
  plot_psi(c1.get(), "h3_sEPD_Psi_2", 2, "x", kRed, output);
  plot_psi(c1.get(), "h3_sEPD_Psi_2_corr", 2, "x", kBlue, output);
  plot_psi(c1.get(), "h3_sEPD_Psi_2_corr2", 2, "x", kGreen+3, output);

  plot_psi(c1.get(), "h3_sEPD_Psi_2", 2, "y", kRed, output);
  plot_psi(c1.get(), "h3_sEPD_Psi_2_corr", 2, "y", kBlue, output);
  plot_psi(c1.get(), "h3_sEPD_Psi_2_corr2", 2, "y", kGreen+3, output);

  plot_psi(c1.get(), "h3_sEPD_Psi_3", 3, "x", kRed, output);
  plot_psi(c1.get(), "h3_sEPD_Psi_3_corr", 3, "x", kBlue, output);
  plot_psi(c1.get(), "h3_sEPD_Psi_3_corr2", 3, "x", kGreen+3, output);

  plot_psi(c1.get(), "h3_sEPD_Psi_3", 3, "y", kRed, output);
  plot_psi(c1.get(), "h3_sEPD_Psi_3_corr", 3, "y", kBlue, output);
  plot_psi(c1.get(), "h3_sEPD_Psi_3_corr2", 3, "y", kGreen+3, output);

  plot_psi(c1.get(), "h3_sEPD_Psi_4", 4, "x", kRed, output);
  plot_psi(c1.get(), "h3_sEPD_Psi_4_corr", 4, "x", kBlue, output);
  plot_psi(c1.get(), "h3_sEPD_Psi_4_corr2", 4, "x", kGreen+3, output);

  plot_psi(c1.get(), "h3_sEPD_Psi_4", 4, "y", kRed, output);
  plot_psi(c1.get(), "h3_sEPD_Psi_4_corr", 4, "y", kBlue, output);
  plot_psi(c1.get(), "h3_sEPD_Psi_4_corr2", 4, "y", kGreen+3, output);

  // Val 1
  plot_val1(c1.get(), "h_sEPD_Q_S_x_2_avg", "h_sEPD_Q_S_x_2_corr_avg", output);
  plot_val1(c1.get(), "h_sEPD_Q_S_y_2_avg", "h_sEPD_Q_S_y_2_corr_avg", output);
  plot_val1(c1.get(), "h_sEPD_Q_N_x_2_avg", "h_sEPD_Q_N_x_2_corr_avg", output);
  plot_val1(c1.get(), "h_sEPD_Q_N_y_2_avg", "h_sEPD_Q_N_y_2_corr_avg", output);

  plot_val1(c1.get(), "h_sEPD_Q_S_x_3_avg", "h_sEPD_Q_S_x_3_corr_avg", output);
  plot_val1(c1.get(), "h_sEPD_Q_S_y_3_avg", "h_sEPD_Q_S_y_3_corr_avg", output);
  plot_val1(c1.get(), "h_sEPD_Q_N_x_3_avg", "h_sEPD_Q_N_x_3_corr_avg", output);
  plot_val1(c1.get(), "h_sEPD_Q_N_y_3_avg", "h_sEPD_Q_N_y_3_corr_avg", output);

  plot_val1(c1.get(), "h_sEPD_Q_S_x_4_avg", "h_sEPD_Q_S_x_4_corr_avg", output);
  plot_val1(c1.get(), "h_sEPD_Q_S_y_4_avg", "h_sEPD_Q_S_y_4_corr_avg", output);
  plot_val1(c1.get(), "h_sEPD_Q_N_x_4_avg", "h_sEPD_Q_N_x_4_corr_avg", output);
  plot_val1(c1.get(), "h_sEPD_Q_N_y_4_avg", "h_sEPD_Q_N_y_4_corr_avg", output);

  plot_val1(c1.get(), "h_sEPD_Q_S_xy_2_avg", "h_sEPD_Q_S_xy_2_corr_avg", output);
  plot_val1(c1.get(), "h_sEPD_Q_N_xy_2_avg", "h_sEPD_Q_N_xy_2_corr_avg", output);
  plot_val1(c1.get(), "h_sEPD_Q_S_xy_3_avg", "h_sEPD_Q_S_xy_3_corr_avg", output);
  plot_val1(c1.get(), "h_sEPD_Q_N_xy_3_avg", "h_sEPD_Q_N_xy_3_corr_avg", output);
  plot_val1(c1.get(), "h_sEPD_Q_S_xy_4_avg", "h_sEPD_Q_S_xy_4_corr_avg", output);
  plot_val1(c1.get(), "h_sEPD_Q_N_xy_4_avg", "h_sEPD_Q_N_xy_4_corr_avg", output);

  // 2nd Order Validation
  plot_val2(c1.get(), "S", 2, output);
  plot_val2(c1.get(), "N", 2, output);

  plot_val2(c1.get(), "S", 3, output);
  plot_val2(c1.get(), "N", 3, output);

  plot_val2(c1.get(), "S", 4, output);
  plot_val2(c1.get(), "N", 4, output);

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
