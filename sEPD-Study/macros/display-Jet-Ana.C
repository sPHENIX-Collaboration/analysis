// -- utils
#include "myUtils.C"

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
#include <TH2.h>
#include <TH3.h>
#include <TKey.h>
#include <TLegend.h>
#include <TLine.h>
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

// ====================================================================
// The Analysis Class
// ====================================================================
class DisplayJetAna
{
 public:
  // The constructor takes the configuration
  DisplayJetAna(std::string input_file, std::string output_dir)
    : m_input_file(std::move(input_file))
    , m_output_dir(std::move(output_dir))
  {
  }

  void run()
  {
    read_hists();
    init_hists();
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
  void init_hists();
  void draw();
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void DisplayJetAna::read_hists()
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

void DisplayJetAna::init_hists()
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

  myUtils::setEMCalDim(m_hists["h2DummySector"].get());
  myUtils::setEMCalDim(m_hists["h2DummyIB"].get());
}

void DisplayJetAna::draw()
{
  // set sPHENIX plotting style
  SetsPhenixStyle();

  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>("c1");
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(2900, 1000);
  c1->SetLeftMargin(.06f);
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
  output = std::format("{}/plots-phi-eta.pdf", m_output_dir);

  c1->Print((output + "[").c_str(), "pdf portrait");

  // ---------------------------------------------
  gPad->SetGrid();

  auto h2Dead = dynamic_cast<TH2*>(m_hists["h2EMCalBadTowersDead"].get());
  auto h2Deadv2 = dynamic_cast<TH2*>(m_hists["h2EMCalBadTowersDeadv2"].get());

  myUtils::setEMCalDim(h2Dead);
  myUtils::setEMCalDim(h2Deadv2);

  h2Dead->Draw("COL");
  m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
  m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2EMCalBadTowersDead").c_str());

  h2Deadv2->Draw("COL");
  m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
  m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2EMCalBadTowersDeadv2").c_str());

  // ---------------------------------------------

  gPad->SetGrid(0, 0);
  c1->SetLeftMargin(.03f);
  c1->SetRightMargin(.07f);

  auto h2JetPhiEta = dynamic_cast<TH2*>(dynamic_cast<TH3*>(m_hists["h3JetPhiEtaPt"].get())->Project3D("yx"));
  auto h2JetPhiEtav2 = dynamic_cast<TH2*>(dynamic_cast<TH3*>(m_hists["h3JetPhiEtaPtv2"].get())->Project3D("yx"));

  h2JetPhiEta->Draw("COLZ1");
  gPad->Update();

  TPaletteAxis* palette = dynamic_cast<TPaletteAxis*>(h2JetPhiEta->GetListOfFunctions()->FindObject("palette"));
  palette->SetX2NDC(.97f);

  h2JetPhiEta->SetMaximum(5e3);
  h2JetPhiEta->GetYaxis()->SetTitleOffset(0.4f);
  h2JetPhiEta->GetZaxis()->SetMaxDigits(2);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2JetPhiEta").c_str());

  h2JetPhiEtav2->Draw("COLZ1");
  gPad->Update();

  palette = dynamic_cast<TPaletteAxis*>(h2JetPhiEtav2->GetListOfFunctions()->FindObject("palette"));
  palette->SetX2NDC(.97f);

  h2JetPhiEtav2->SetMaximum(5e3);
  h2JetPhiEtav2->GetYaxis()->SetTitleOffset(0.4f);
  h2JetPhiEtav2->GetZaxis()->SetMaxDigits(2);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2JetPhiEtav2").c_str());

  c1->Print((output + "]").c_str(), "pdf portrait");

  // -------------------------------------------

  c1->SetCanvasSize(1200, 1000);
  c1->SetLeftMargin(.12f);
  c1->SetRightMargin(.02f);
  c1->SetTopMargin(.11f);
  c1->SetBottomMargin(.09f);

  output = std::format("{}/plots.pdf", m_output_dir);

  c1->Print((output + "[").c_str(), "pdf portrait");

  auto hJetPhi = h2JetPhiEta->ProjectionX();
  auto hJetPhiv2 = h2JetPhiEtav2->ProjectionX();

  hJetPhi->Draw();
  hJetPhi->SetLineColor(kBlue);
  hJetPhi->SetLineWidth(3);
  hJetPhi->GetYaxis()->SetRangeUser(0, 5.5e4);
  hJetPhi->GetYaxis()->SetMaxDigits(3);
  hJetPhi->GetYaxis()->SetTitle("Jet Yield / (2#pi / 64)");
  hJetPhi->GetYaxis()->SetTitleOffset(1.f);

  hJetPhiv2->Draw("same");
  hJetPhiv2->SetLineColor(kRed);
  hJetPhiv2->SetLineWidth(3);

  double xshift = 0.17;
  double yshift = 0.05;

  std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.06f);
  leg->AddEntry(hJetPhi, "Default", "l");
  leg->AddEntry(hJetPhiv2, "Dead IB Mask", "l");
  leg->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hJetPhi-overlay").c_str());

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
    DisplayJetAna analysis(input_file, output_dir);
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
