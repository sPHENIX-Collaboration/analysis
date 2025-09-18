// ====================================================================
// My Includes
// ====================================================================
#include "myUtils.C"

// ====================================================================
// sPHENIX Includes
// ====================================================================
#include <calobase/TowerInfoDefs.h>
#include <cdbobjects/CDBTTree.h>
#include <sPhenixStyle.C>

// ====================================================================
// ROOT Includes
// ====================================================================
#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TF1.h>
#include <TH2.h>
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
class Displayv2
{
 public:
  // The constructor takes the configuration
  Displayv2(std::string output_dir)
    : m_output_dir(std::move(output_dir))
  {
  }

  void run()
  {
    read_hists();
    init_hists();
    read_calib();
    draw();
  }

 private:
  // Configuration stored as members
  std::string m_output_dir;
  std::map<std::string, std::unique_ptr<TH1>> m_hists;
  std::map<std::string, int> m_ctr;

  bool m_saveFig{true};
  bool m_debug{false};

  const std::string m_calib_default{"/cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/CEMC_calib_ADC_to_ETower_default/da/74/da74db11788df02ed879fa373c50c66c_EMCAL_ADC_to_Etower_2025_initial_v3.root"};
  const std::string m_calib_new{"output/calib/EMCAL_ADC_to_Etower-new_newcdbtag_v008-73839-raw.root"};
  const std::string m_fieldname{"CEMC_calib_ADC_to_ETower"};

  const std::string m_hist_default{"output/QA/test-2025.root"};
  const std::string m_hist_new{"output/QA/test-73839.root"};

  const std::string m_fitout_default{"output/QA/fitout-66580.root"};
  const std::string m_fitout_new{"output/QA/fitout-73839.root"};

  // --- Private Helper Methods ---
  void read_hists();
  void init_hists();
  void draw();
  void read_calib();
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void Displayv2::read_hists()
{
  TH1::AddDirectory(kFALSE);

  auto file_default = std::unique_ptr<TFile>(TFile::Open(m_hist_default.c_str()));
  auto file_new = std::unique_ptr<TFile>(TFile::Open(m_hist_new.c_str()));

  auto fitout_default = std::unique_ptr<TFile>(TFile::Open(m_fitout_default.c_str()));
  auto fitout_new = std::unique_ptr<TFile>(TFile::Open(m_fitout_new.c_str()));

  // Check if the file was opened successfully.
  if (!file_default || file_default->IsZombie())
  {
    std::cout << std::format("Error: Could not open file '{}'\n", m_hist_default);
    return;
  }

  if (!file_new || file_new->IsZombie())
  {
    std::cout << std::format("Error: Could not open file '{}'\n", m_hist_new);
    return;
  }

  if (!fitout_default || fitout_default->IsZombie())
  {
    std::cout << std::format("Error: Could not open file '{}'\n", m_fitout_default);
    return;
  }

  if (!fitout_new || fitout_new->IsZombie())
  {
    std::cout << std::format("Error: Could not open file '{}'\n", m_fitout_new);
    return;
  }

  m_hists["hInvMass_default"] = std::unique_ptr<TH1>(static_cast<TH1*>(file_default->Get("h_InvMass")->Clone("hInvMass_default")));
  m_hists["hInvMass_new"] = std::unique_ptr<TH1>(static_cast<TH1*>(file_new->Get("h_InvMass")->Clone("hInvMass_new")));
  m_hists["hSigma_default"] = std::unique_ptr<TH1>(static_cast<TH1*>(fitout_default->Get("h_sigma_eta")->Clone("hSigma_default")));
  m_hists["hSigma_new"] = std::unique_ptr<TH1>(static_cast<TH1*>(fitout_new->Get("h_sigma_eta")->Clone("hSigma_new")));
  m_hists["hPeak_default"] = std::unique_ptr<TH1>(static_cast<TH1*>(fitout_default->Get("h_peak_eta")->Clone("hPeak_default")));
  m_hists["hPeak_new"] = std::unique_ptr<TH1>(static_cast<TH1*>(fitout_new->Get("h_peak_eta")->Clone("hPeak_new")));
}

void Displayv2::init_hists()
{
  // dummy hists for labeling
  m_hists["h2DummySector"] = std::make_unique<TH2F>("h2DummySector", "", myUtils::m_nsector / 2, 0, myUtils::m_nsector / 2, 2, 0, 2);
  m_hists["h2DummyIB"] = std::make_unique<TH2F>("h2DummyIB", "", myUtils::m_nsector / 2, 0, myUtils::m_nsector / 2, myUtils::m_nib * 2 / myUtils::m_nsector, 0, myUtils::m_nib * 2 / myUtils::m_nsector);

  // setup Dummy hists
  for (int i = 0; i < myUtils::m_nsector; ++i)
  {
    int x = i % (myUtils::m_nsector / 2) + 1;
    int y = (i < (myUtils::m_nsector / 2)) ? 2 : 1;
    m_hists["h2DummySector"]->SetBinContent(x, y, i);
  }

  for (int i = 0; i < myUtils::m_nib; ++i)
  {
    int val = i % myUtils::m_nib_per_sector;
    int x = (i / myUtils::m_nib_per_sector) % (myUtils::m_nsector / 2) + 1;
    int y = (i < (myUtils::m_nib / 2)) ? myUtils::m_nib_per_sector - val : myUtils::m_nib_per_sector + val + 1;
    m_hists["h2DummyIB"]->SetBinContent(x, y, val);
  }
}

void Displayv2::read_calib()
{
  std::unique_ptr<CDBTTree> cdbttree_default = std::make_unique<CDBTTree>(m_calib_default);
  std::unique_ptr<CDBTTree> cdbttree_new = std::make_unique<CDBTTree>(m_calib_new);

  unsigned int ntowers = static_cast<unsigned int>(myUtils::m_nphi * myUtils::m_neta);

  std::string title;
  title = "EMCal Calibration [MeV/ADC]; Tower Index #phi; Tower Index #eta";
  m_hists["h2Calib_default"] = std::make_unique<TH2F>("h2Calib_default", title.c_str(), myUtils::m_nphi, 0, myUtils::m_nphi, myUtils::m_neta, 0, myUtils::m_neta);
  m_hists["h2Calib_new"] = std::make_unique<TH2F>("h2Calib_new", title.c_str(), myUtils::m_nphi, 0, myUtils::m_nphi, myUtils::m_neta, 0, myUtils::m_neta);

  title = "EMCal Calibration [MeV/ADC]; Calibration Default [MeV/ADC]; Calibration New [MeV/ADC]";
  m_hists["hCalib_corr"] = std::make_unique<TH2F>("hCalib_corr", title.c_str(), 100, 0, 10, 100, 0, 10);

  title = "EMCal Calibration Ratio [New/Default]; New / Default; Counts";
  m_hists["hCalib_ratio"] = std::make_unique<TH1F>("hCalib_ratio", title.c_str(), 50, 0, 2.5);

  title = "EMCal Calibration; Calibration [MeV/ADC]; Counts";
  m_hists["hCalib_default"] = std::make_unique<TH1F>("hCalib_default", title.c_str(), 100, 0, 10);
  m_hists["hCalib_new"] = std::make_unique<TH1F>("hCalib_new", title.c_str(), 100, 0, 10);

  m_hists["hCalib_default"]->Sumw2();
  m_hists["hCalib_new"]->Sumw2();

  for (unsigned int channel = 0; channel < ntowers; ++channel)
  {
    unsigned int key = TowerInfoDefs::encode_emcal(channel);
    int phi = static_cast<int>(TowerInfoDefs::getCaloTowerPhiBin(key));
    int eta = static_cast<int>(TowerInfoDefs::getCaloTowerEtaBin(key));

    double calib_default = cdbttree_default->GetFloatValue(static_cast<int>(key), m_fieldname) * 1e3;
    double calib_new = cdbttree_new->GetFloatValue(static_cast<int>(key), m_fieldname) * 1e3;

    dynamic_cast<TH2 *>(m_hists["h2Calib_default"].get())->SetBinContent(phi + 1, eta + 1, calib_default);
    dynamic_cast<TH2 *>(m_hists["h2Calib_new"].get())->SetBinContent(phi + 1, eta + 1, calib_new);
    dynamic_cast<TH2 *>(m_hists["hCalib_corr"].get())->Fill(calib_default, calib_new);

    m_hists["hCalib_default"]->Fill(calib_default);
    m_hists["hCalib_new"]->Fill(calib_new);
    if(calib_default)
    {
      m_hists["hCalib_ratio"]->Fill(calib_new/calib_default);
    }

    if(m_debug && m_ctr["debug"]++ < 10)
    {
        std::cout << std::format("Calib Default: {:.2f}, New: {:.2f}\n", calib_default, calib_new);
    }
  }
}

void Displayv2::draw()
{
  // set sPHENIX plotting style
  SetsPhenixStyle();

  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>("c1");
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(2900, 1000);
  c1->SetLeftMargin(.04f);
  c1->SetRightMargin(.08f);
  c1->SetTopMargin(.11f);
  c1->SetBottomMargin(.09f);

  gStyle->SetOptTitle(1);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleFontSize(0.08f);
  gStyle->SetTitleW(1);
  gStyle->SetTitleH(0.08f);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);

  gPad->SetGrid();

  for (const auto &[name, hist] : m_hists)
  {
    if (name.starts_with("h2"))
    {
      myUtils::setEMCalDim(hist.get());
    }
  }

  std::string output;

  // create output directory
  std::filesystem::create_directories(m_output_dir);
  std::filesystem::create_directories(m_output_dir + "/images");

  output = std::format("{}/plots-Calib.pdf", m_output_dir);

  c1->Print((output + "[").c_str(), "pdf portrait");

  for (const auto &[name, hist] : m_hists)
  {
    if (name.starts_with("h2Calib"))
    {
      hist->Draw("COLZ");
      m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
      m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

      hist->SetMaximum(5);

      c1->Print(output.c_str(), "pdf portrait");
      c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());
    }
  }

  c1->Print((output + "]").c_str(), "pdf portrait");

  // -----------------------------------

  c1->SetCanvasSize(1200, 1000);
  c1->SetLeftMargin(.1f);
  c1->SetRightMargin(.03f);
  c1->SetTopMargin(.11f);
  c1->SetBottomMargin(.09f);

  gPad->SetGrid(0,0);
  gPad->SetLogy();

  output = std::format("{}/plots-Calib-Comp.pdf", m_output_dir);

  c1->Print((output + "[").c_str(), "pdf portrait");

  // --------------------------------------------

  m_hists["hCalib_default"]->Draw("HIST");
  m_hists["hCalib_new"]->Draw("HIST same");

  m_hists["hCalib_default"]->SetLineColor(kBlue);
  m_hists["hCalib_new"]->SetLineColor(kRed);

  m_hists["hCalib_default"]->SetLineWidth(3);
  m_hists["hCalib_new"]->SetLineWidth(3);

  m_hists["hCalib_default"]->GetYaxis()->SetTitleSize(0.06f);
  m_hists["hCalib_default"]->GetYaxis()->SetTitleOffset(0.8f);

  double xshift = 0.5;
  double yshift = 0;

  std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.06f);
  leg->AddEntry(m_hists["hCalib_default"].get(), "Default", "l");
  leg->AddEntry(m_hists["hCalib_new"].get(), "New", "l");
  leg->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  c1->Print(std::format("{}/images/{}.png", m_output_dir, "hCalib-overlay").c_str());

  // --------------------------------------------

  m_hists["hCalib_ratio"]->Draw();
  m_hists["hCalib_ratio"]->SetLineColor(kBlue);
  m_hists["hCalib_ratio"]->SetLineWidth(3);

  m_hists["hCalib_ratio"]->GetYaxis()->SetTitleSize(0.06f);
  m_hists["hCalib_ratio"]->GetYaxis()->SetTitleOffset(0.8f);

  c1->Print(output.c_str(), "pdf portrait");
  c1->Print(std::format("{}/images/{}.png", m_output_dir, "hCalib-ratio").c_str());

  // --------------------------------------------

  c1->SetTopMargin(.02f);

  gPad->SetLogy(0);

  m_hists["hSigma_default"]->Draw();
  m_hists["hSigma_new"]->Draw("same");

  m_hists["hSigma_default"]->SetLineColor(kBlue);
  m_hists["hSigma_new"]->SetLineColor(kRed);

  m_hists["hSigma_default"]->SetMarkerColor(kBlue);
  m_hists["hSigma_new"]->SetMarkerColor(kRed);

  m_hists["hSigma_default"]->SetMarkerStyle(kFullDotLarge);
  m_hists["hSigma_new"]->SetMarkerStyle(kFullDotLarge);

  m_hists["hSigma_default"]->GetYaxis()->SetTitle("#sigma [GeV]");
  m_hists["hSigma_default"]->GetYaxis()->SetTitleSize(0.06f);
  m_hists["hSigma_default"]->GetYaxis()->SetTitleOffset(0.8f);

  m_hists["hSigma_default"]->GetXaxis()->SetTitle("Tower Index #eta");

  xshift = 0.5;
  yshift = 0;

  leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.06f);
  leg->AddEntry(m_hists["hSigma_default"].get(), "Default", "l");
  leg->AddEntry(m_hists["hSigma_new"].get(), "New", "l");
  leg->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  c1->Print(std::format("{}/images/{}.png", m_output_dir, "hSigma-overlay").c_str());

  // --------------------------------------------

  m_hists["hSigma_new"]->Draw();

  m_hists["hSigma_new"]->Divide(m_hists["hSigma_default"].get());

  m_hists["hSigma_new"]->GetYaxis()->SetTitle("Ratio #sigma: New / Default");
  m_hists["hSigma_new"]->GetXaxis()->SetTitle("Tower Index #eta");
  m_hists["hSigma_new"]->GetYaxis()->SetTitleSize(0.06f);
  m_hists["hSigma_new"]->GetYaxis()->SetTitleOffset(0.8f);
  m_hists["hSigma_new"]->GetYaxis()->SetRangeUser(0.4,1.2);

  auto tf1 = std::make_unique<TF1>("fa1", "1", 0, 95);

  tf1->Draw("same");
  tf1->SetLineColor(kBlue);
  tf1->SetLineWidth(3);
  tf1->SetLineStyle(kDashed);

  c1->Print(output.c_str(), "pdf portrait");
  c1->Print(std::format("{}/images/{}.png", m_output_dir, "hSigma-ratio").c_str());

  // --------------------------------------------

  c1->SetLeftMargin(.13f);

  m_hists["hPeak_default"]->Draw();
  m_hists["hPeak_new"]->Draw("same");

  m_hists["hPeak_default"]->SetLineColor(kBlue);
  m_hists["hPeak_new"]->SetLineColor(kRed);

  m_hists["hPeak_default"]->SetMarkerColor(kBlue);
  m_hists["hPeak_new"]->SetMarkerColor(kRed);

  m_hists["hPeak_default"]->SetMarkerStyle(kFullDotLarge);
  m_hists["hPeak_new"]->SetMarkerStyle(kFullDotLarge);

  m_hists["hPeak_default"]->GetYaxis()->SetTitle("m_{#gamma#gamma} [GeV]");
  m_hists["hPeak_default"]->GetYaxis()->SetTitleSize(0.06f);
  m_hists["hPeak_default"]->GetYaxis()->SetTitleOffset(1.f);
  m_hists["hPeak_default"]->GetYaxis()->SetRangeUser(0.144, 0.156);

  m_hists["hPeak_default"]->GetXaxis()->SetTitle("Tower Index #eta");

  xshift = 0.5;
  yshift = -0.5;

  leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.06f);
  leg->AddEntry(m_hists["hPeak_default"].get(), "Default", "l");
  leg->AddEntry(m_hists["hPeak_new"].get(), "New", "l");
  leg->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  c1->Print(std::format("{}/images/{}.png", m_output_dir, "hPeak-overlay").c_str());

  // --------------------------------------------

  m_hists["hPeak_new"]->Draw();

  m_hists["hPeak_new"]->Divide(m_hists["hPeak_default"].get());

  m_hists["hPeak_new"]->GetYaxis()->SetTitle("Ratio m_{#gamma#gamma}: New / Default");
  m_hists["hPeak_new"]->GetXaxis()->SetTitle("Tower Index #eta");
  m_hists["hPeak_new"]->GetYaxis()->SetTitleSize(0.06f);
  m_hists["hPeak_new"]->GetYaxis()->SetTitleOffset(1.f);
  m_hists["hPeak_new"]->GetYaxis()->SetRangeUser(0.99,1.01);

  tf1->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  c1->Print(std::format("{}/images/{}.png", m_output_dir, "hPeak-ratio").c_str());

  // --------------------------------------------

  c1->SetLeftMargin(.12f);

  int end_bin = m_hists["hInvMass_default"]->FindBin(0.5)-1;

  m_hists["hInvMass_default"]->Scale(1./m_hists["hInvMass_default"]->Integral(1, end_bin));
  m_hists["hInvMass_new"]->Scale(1./m_hists["hInvMass_new"]->Integral(1, end_bin));

  m_hists["hInvMass_default"]->Draw("HIST");
  m_hists["hInvMass_new"]->Draw("HIST same");

  m_hists["hInvMass_default"]->GetXaxis()->SetRangeUser(0, 0.5);
  m_hists["hInvMass_default"]->GetYaxis()->SetRangeUser(0, 2.4e-2);

  m_hists["hInvMass_default"]->SetLineColor(kBlue);
  m_hists["hInvMass_new"]->SetLineColor(kRed);

  m_hists["hInvMass_default"]->SetLineWidth(3);
  m_hists["hInvMass_new"]->SetLineWidth(3);

  m_hists["hInvMass_default"]->SetTitle("");
  m_hists["hInvMass_default"]->GetYaxis()->SetTitle("Normalized Counts");
  m_hists["hInvMass_default"]->GetXaxis()->SetTitle("m_{#gamma#gamma} [GeV]");

  xshift = 0.5;
  yshift = 0;

  leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.06f);
  leg->AddEntry(m_hists["hInvMass_default"].get(), "Default", "l");
  leg->AddEntry(m_hists["hInvMass_new"].get(), "New", "l");
  leg->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  c1->Print(std::format("{}/images/{}.png", m_output_dir, "hInvMass-overlay").c_str());

  // --------------------------------------------

  c1->SetLeftMargin(.08f);
  c1->SetRightMargin(.12f);

  gPad->SetLogz();

  m_hists["hCalib_corr"]->Draw("COLZ1");
  m_hists["hCalib_corr"]->GetYaxis()->SetTitleOffset(1.f);
  m_hists["hCalib_corr"]->SetTitle("");

  tf1 = std::make_unique<TF1>("fa1", "x", 0, 10);

  tf1->Draw("same");
  tf1->SetLineColor(kRed);
  tf1->SetLineWidth(3);

  c1->Print(output.c_str(), "pdf portrait");
  c1->Print(std::format("{}/images/{}.png", m_output_dir, "hCalib-Correlation").c_str());

  c1->Print((output + "]").c_str(), "pdf portrait");
}

int main(int argc, const char *const argv[])
{
  gROOT->SetBatch(true);

  if (argc > 2)
  {
    std::cout << "Usage: " << argv[0] << " [output_directory]" << std::endl;
    return 1;
  }

  std::string output_dir = (argc >= 2) ? argv[1] : ".";

  try
  {
    Displayv2 analysis(output_dir);
    analysis.run();
  }
  catch (const std::exception &e)
  {
    std::cout << std::format("An exception occurred: {}", e.what()) << std::endl;
    return 1;
  }

  std::cout << "Analysis complete." << std::endl;
  return 0;
}
