// ====================================================================
// My Includes
// ====================================================================
#include "myUtils.C"

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
#include <TH2.h>
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
class Displayv4
{
 public:
  // The constructor takes the configuration
  Displayv4(std::string input_list, std::string output_dir)
    : m_input_list(std::move(input_list))
    , m_output_dir(std::move(output_dir))
  {
  }

  void run()
  {
    read_file();
    init_hists();
    draw();
  }

 private:
  // Configuration stored as members
  std::string m_input_list;
  std::string m_output_dir;
  std::map<std::string, std::string> m_files;
  std::map<std::string, std::unique_ptr<TH1>> m_hists;

  // A helper struct to create a range of lines from an input stream.
  struct FileLine
  {
    std::string line;
    friend std::istream& operator>>(std::istream& s, FileLine& fl)
    {
      std::getline(s, fl.line);
      return s;
    }
  };

  // --- Private Helper Methods ---
  void read_file();
  void init_hists();
  void draw();
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void Displayv4::read_file()
{
  std::ifstream inputFile(m_input_list);

  if (!inputFile.is_open())
  {
    std::cout << std::format("Error: Could not open the file {}", m_input_list) << std::endl;
    return;
  }

  // Create a range of lines from the file.
  auto lines_range = std::views::istream<FileLine>(inputFile);
  std::regex pattern("(\\d+)");
  std::smatch matches;

  // Iterate over the range and process each line.
  for (const auto& fl : lines_range)
  {
    std::string stem = std::filesystem::path(fl.line).stem().string();

    if (std::regex_search(stem, matches, pattern))
    {
      if (matches.size() > 1)
      {
        std::string runnumber = matches[1].str();
        std::cout << std::format("File: {}, Run: {}", fl.line, runnumber) << std::endl;
        m_files[runnumber] = fl.line;
      }
    }
  }
}

void Displayv4::init_hists()
{
  TH1::AddDirectory(kFALSE);
  std::string name;

  for (const auto& [runnumber, path] : m_files)
  {
    auto file = std::unique_ptr<TFile>(TFile::Open(path.c_str()));

    // Check if the file was opened successfully.
    if (!file || file->IsZombie())
    {
      std::cout << std::format("Error: Could not open file '{}'\n", path);
      return;
    }

    name = std::format("hCEMC_{}", runnumber);
    m_hists[name] = std::unique_ptr<TH1>(static_cast<TH1*>(file->Get("hCEMC")->Clone(name.c_str())));

    name = std::format("h2CEMC_{}", runnumber);
    m_hists[name] = std::unique_ptr<TH2>(static_cast<TH2*>(file->Get("h2CEMC")->Clone(name.c_str())));
    myUtils::setEMCalDim(m_hists[name].get());
  }

  // dummy hists for labeling
  m_hists["h2DummySector"] = std::make_unique<TH2F>("h2DummySector", "", myUtils::m_nsector / 2, 0, myUtils::m_nsector / 2, 2, 0, 2);
  m_hists["h2DummyIB"] = std::make_unique<TH2F>("h2DummyIB", "", myUtils::m_nsector / 2, 0, myUtils::m_nsector / 2, myUtils::m_nib * 2 / myUtils::m_nsector, 0, myUtils::m_nib * 2 / myUtils::m_nsector);

  myUtils::setEMCalDim(m_hists["h2DummySector"].get());
  myUtils::setEMCalDim(m_hists["h2DummyIB"].get());

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

void Displayv4::draw()
{
  // set sPHENIX plotting style
  SetsPhenixStyle();

  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>("c1");
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(2900, 1000);
  c1->SetLeftMargin(.05f);
  c1->SetRightMargin(.1f);
  c1->SetTopMargin(.1f);
  c1->SetBottomMargin(.12f);

  gPad->SetGrid();

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
  output = std::format("{}/plots-h2CEMC.pdf", m_output_dir);

  c1->Print((output + "[").c_str(), "pdf portrait");

  for (const auto& [name, hist] : m_hists)
  {
    if (name.starts_with("h2CEMC"))
    {

      std::string runnumber = name.substr(name.find('_') + 1);
      std::string title = std::format("EMCal [ADC], Run: {}", runnumber);

      hist->Draw("COLZ1");
      m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
      m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

      hist->SetTitle(title.c_str());
      hist->SetMaximum(6e3);

      hist->GetXaxis()->SetTitleSize(0.06f);
      hist->GetYaxis()->SetTitleSize(0.06f);
      hist->GetXaxis()->SetTitleOffset(0.8f);
      hist->GetYaxis()->SetTitleOffset(0.35f);

      // Pedestal Run
      if(runnumber == "73079")
      {
          hist->SetMaximum(2e2);
      }
      // LED Run
      if(runnumber == "73172" || runnumber == "73463" || runnumber == "73571")
      {
          hist->SetMaximum(1.6e4);
          c1->Print(output.c_str(), "pdf portrait");
          c1->Print(std::format("{}/h2CEMC-{}-v1.png", m_output_dir, runnumber).c_str());

          hist->SetMaximum(2e3);
          c1->Print(output.c_str(), "pdf portrait");
          c1->Print(std::format("{}/h2CEMC-{}-v2.png", m_output_dir, runnumber).c_str());

          hist->SetMaximum(5e2);
          c1->Print(output.c_str(), "pdf portrait");
          c1->Print(std::format("{}/h2CEMC-{}-v3.png", m_output_dir, runnumber).c_str());

          hist->SetMaximum(1e2);
          c1->Print(output.c_str(), "pdf portrait");
          c1->Print(std::format("{}/h2CEMC-{}-v4.png", m_output_dir, runnumber).c_str());

          hist->SetMaximum(50);
          c1->Print(output.c_str(), "pdf portrait");
          c1->Print(std::format("{}/h2CEMC-{}-v5.png", m_output_dir, runnumber).c_str());
      }
      else
      {
          c1->Print(output.c_str(), "pdf portrait");
          c1->Print(std::format("{}/h2CEMC-{}.png", m_output_dir, runnumber).c_str());
      }
    }
  }

  c1->Print((output + "]").c_str(), "pdf portrait");

  output = std::format("{}/plots-hCEMC.pdf", m_output_dir);

  c1->SetCanvasSize(1200, 1000);
  c1->SetLeftMargin(.09f);
  c1->SetRightMargin(.04f);
  c1->SetTopMargin(.1f);
  c1->SetBottomMargin(.12f);

  gPad->SetGrid(0, 0);
  gPad->SetLogy();

  c1->Print((output + "[").c_str(), "pdf portrait");

  for (const auto& [name, hist] : m_hists)
  {
    if (name.starts_with("hCEMC"))
    {
      std::string runnumber = name.substr(name.find('_')+1);
      std::string title = std::format("EMCal, Run: {}", runnumber);

      hist->Draw("HIST");
      hist->SetTitle(title.c_str());
      hist->GetXaxis()->SetRangeUser(0, 6e3);
      // Pedestal Run
      if(runnumber == "73079")
      {
          hist->GetXaxis()->SetRangeUser(0, 5e2);
      }
      // LED Run
      if(runnumber == "73172" || runnumber == "73463" || runnumber == "73571")
      {
          hist->GetXaxis()->SetRangeUser(0, 1.6e4);
      }

      c1->Print(output.c_str(), "pdf portrait");
      c1->Print(std::format("{}/hCEMC-{}.png", m_output_dir, runnumber).c_str());
    }
  }

  c1->Print((output + "]").c_str(), "pdf portrait");
}

int main(int argc, const char* const argv[])
{
  gROOT->SetBatch(true);

  if (argc < 2 || argc > 3)
  {
    std::cout << "Usage: " << argv[0] << " <input_list_file> [output_directory]" << std::endl;
    return 1;
  }

  const std::string input_list = argv[1];
  std::string output_dir = (argc >= 3) ? argv[2] : ".";

  try
  {
    Displayv4 analysis(input_list, output_dir);
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
