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
class Displayv2
{
 public:
  // The constructor takes the configuration
  Displayv2(std::string input_file, std::string output_dir)
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

  // --- Private Helper Methods ---
  void read_hists();
  void draw();
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void Displayv2::read_hists()
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

void Displayv2::draw()
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

  gPad->SetLogy();

  auto hJetPt = dynamic_cast<TH3*>(m_hists["h3Jet_pT_Phi"].get())->Project3D("x");
  auto hJetPtv2 = dynamic_cast<TH3*>(m_hists["h3Jet_pT_Phiv2"].get())->Project3D("x");

  hJetPt->Draw();
  hJetPtv2->Draw("same");

  hJetPt->SetLineColor(kBlue);
  hJetPtv2->SetLineColor(kRed);

  hJetPt->SetLineWidth(3);
  hJetPtv2->SetLineWidth(3);

  hJetPt->SetTitle("Jets: |z| < 10 cm and MB");
  hJetPt->GetYaxis()->SetTitle("Counts / 1 GeV");
  hJetPt->GetYaxis()->SetTitleOffset(1.f);
  hJetPt->GetXaxis()->SetRangeUser(0, 40);

  double xshift = 0.25;
  double yshift = 0;

  std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.06f);
  leg->AddEntry(hJetPt, "Default", "l");
  leg->AddEntry(hJetPtv2, "Beam Bkg Cuts", "l");
  leg->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "jet-pt-overlay").c_str());

  // --------------------------------

  gPad->SetLogy(0);
  gPad->SetGrid();

  hJetPtv2->Divide(hJetPt);

  hJetPtv2->Draw("P");

  hJetPtv2->SetTitle("Jets: |z| < 10 cm and MB");
  hJetPtv2->GetYaxis()->SetTitle("Ratio: Beam Bkg Cuts / Default");

  hJetPtv2->SetMarkerStyle(kFullDotLarge);
  hJetPtv2->SetMarkerColor(kRed);

  hJetPtv2->GetYaxis()->SetTitleOffset(1.f);
  hJetPtv2->GetXaxis()->SetRangeUser(0, 40);
  hJetPtv2->GetYaxis()->SetRangeUser(0.4, 1.05);

  auto tf1 = std::make_unique<TF1>("fa1", "1", 0, 40);
  tf1->SetLineStyle(kDashed);
  tf1->SetLineColor(kBlue);
  tf1->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "jet-pt-ratio").c_str());

  // --------------------------------

  c1->SetLeftMargin(.11f);
  c1->SetRightMargin(.11f);

  gPad->SetLogz();
  m_hists["h2frcem"]->Draw("COLZ1");
  m_hists["h2frcem"]->GetYaxis()->SetRangeUser(0, 40);

  auto l1 = std::make_unique<TLine>(0.1, 25, 0.1, 40);
  auto l2 = std::make_unique<TLine>(0.9, 25, 0.9, 40);
  auto l3 = std::make_unique<TF1>("fa1", "50*x+20", -0.2, 0.1);
  auto l4 = std::make_unique<TF1>("fa2", "-50*x+70", 0.9, 1.2);

  l1->SetLineColor(kRed);
  l2->SetLineColor(kRed);
  l3->SetLineColor(kRed);
  l4->SetLineColor(kRed);

  l1->SetLineWidth(3);
  l2->SetLineWidth(3);
  l3->SetLineWidth(3);
  l4->SetLineWidth(3);

  l1->Draw("same");
  l2->Draw("same");
  l3->Draw("same");
  l4->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "frcem").c_str());

  // --------------------------------

  m_hists["h2frcemv2"]->Draw("COLZ1");
  m_hists["h2frcemv2"]->GetYaxis()->SetRangeUser(0, 40);

  l1->Draw("same");
  l2->Draw("same");
  l3->Draw("same");
  l4->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "frcemv2").c_str());

  // --------------------------------

  m_hists["h2frcem_vs_frcoh"]->Draw("COLZ1");

  l3 = std::make_unique<TF1>("fa1", "0.65-x", -0.2, 1.2);

  l3->SetLineColor(kRed);

  l3->SetLineWidth(3);

  l3->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "frcem_vs_frcoh").c_str());

  // --------------------------------

  c1->SetRightMargin(.05f);
  gPad->SetGrid(0,0);

  m_hists["hEventJetBkg"]->Scale(100./m_hists["hEvent"]->GetBinContent(4));

  m_hists["hEventJetBkg"]->Draw("HIST");
  m_hists["hEventJetBkg"]->SetTitle("Jet Backgrounds Event Type");
  m_hists["hEventJetBkg"]->GetYaxis()->SetTitle("% of Events: Has Jet");
  m_hists["hEventJetBkg"]->SetLineColor(kBlue);
  m_hists["hEventJetBkg"]->SetLineWidth(3);
  m_hists["hEventJetBkg"]->GetXaxis()->SetLabelSize(0.07f);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hEventJetBkg").c_str());

  // --------------------------------

  auto hJetPhiEta = dynamic_cast<TH3*>(m_hists["h3Jet_PhiEta"].get())->Project3D("yx");
  auto hJetPhiEtav2 = dynamic_cast<TH3*>(m_hists["h3Jet_PhiEtav2"].get())->Project3D("yx");

  auto hJetPhi = dynamic_cast<TH3*>(m_hists["h3Jet_PhiEta"].get())->Project3D("x");
  auto hJetPhiv2 = dynamic_cast<TH3*>(m_hists["h3Jet_PhiEtav2"].get())->Project3D("x");

  hJetPhi->Draw();
  hJetPhiv2->Draw("same");

  hJetPhi->SetLineColor(kBlue);
  hJetPhiv2->SetLineColor(kRed);

  hJetPhi->SetLineWidth(3);
  hJetPhiv2->SetLineWidth(3);

  hJetPhi->SetTitle("Jets: |z| < 10 cm and MB");
  hJetPhi->GetYaxis()->SetTitle("Counts");
  hJetPhi->GetYaxis()->SetTitleOffset(1.f);
  hJetPhi->GetYaxis()->SetRangeUser(0, 6e4);
  hJetPhi->GetYaxis()->SetMaxDigits(3);

  xshift = 0.3;
  yshift = 0.055;

  leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.06f);
  leg->AddEntry(hJetPhi, "Default", "l");
  leg->AddEntry(hJetPhiv2, "Beam Bkg Cuts", "l");
  leg->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hJetPhi-overlay").c_str());

  // --------------------------------

  c1->SetRightMargin(.11f);
  gPad->SetLogz(0);

  m_hists["h2SEPD_South_BelowThresh"]->Scale(100.);
  m_hists["h2SEPD_South_BelowThresh"]->Draw("COLZ1");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2sEPD_South_BelowThresh").c_str());

  // --------------------------------

  m_hists["h2SEPD_North_BelowThresh"]->Scale(100.);
  m_hists["h2SEPD_North_BelowThresh"]->Draw("COLZ1");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2sEPD_North_BelowThresh").c_str());

  // --------------------------------

  m_hists["h2SEPD_South_Charge"]->Draw("COLZ1");
  m_hists["h2SEPD_South_Charge"]->SetMaximum(35);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2sEPD_South_Charge").c_str());

  // --------------------------------

  m_hists["h2SEPD_North_Charge"]->Draw("COLZ1");
  m_hists["h2SEPD_North_Charge"]->SetMaximum(35);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2sEPD_North_Charge").c_str());

  c1->Print((output + "]").c_str(), "pdf portrait");

  // --------------------------------

  c1->SetCanvasSize(2900, 1000);
  c1->SetLeftMargin(.04f);
  c1->SetRightMargin(.11f);
  c1->SetTopMargin(.11f);
  c1->SetBottomMargin(.09f);

  output = std::format("{}/plots-phi-eta.pdf", m_output_dir);

  c1->Print((output + "[").c_str(), "pdf portrait");

  gPad->SetLogz(0);

  hJetPhiEta->Draw("COLZ1");
  hJetPhiEta->SetMaximum(5e3);
  hJetPhiEta->SetTitle("Jet: |z| < 10 cm and MB");
  hJetPhiEta->GetYaxis()->SetTitleOffset(0.5f);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hJetPhiEta").c_str());

  // --------------------------------

  hJetPhiEtav2->Draw("COLZ1");
  hJetPhiEtav2->SetTitle("Jet: |z| < 10 cm and MB");
  hJetPhiEtav2->GetYaxis()->SetTitleOffset(0.5f);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hJetPhiEtav2").c_str());

  hJetPhiEtav2->SetMaximum(5e3);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hJetPhiEtav2-zoom").c_str());

  // --------------------------------

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
    Displayv2 analysis(input_file, output_dir);
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
