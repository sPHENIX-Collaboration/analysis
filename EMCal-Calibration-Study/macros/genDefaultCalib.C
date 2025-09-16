// ====================================================================
// My Includes
// ====================================================================
#include "myUtils.C"

// ====================================================================
// sPHENIX Includes
// ====================================================================
#include <calobase/TowerInfoDefs.h>
#include <cdbobjects/CDBTTree.h>

// ====================================================================
// ROOT Includes
// ====================================================================
#include <TFile.h>
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
class GenDefaultCalib
{
 public:
  // The constructor takes the configuration
  GenDefaultCalib(std::string output_dir)
    : m_output_dir(std::move(output_dir))
  {
  }

  void run()
  {
      genCalib();
  }

 private:
  // Configuration stored as members
  std::string m_output_dir;
  std::map<std::string, std::unique_ptr<TH1>> m_hists;
  std::map<std::string, int> m_ctr;

  std::string m_fieldname = "CEMC_calib_ADC_to_ETower";
  float m_calib_default = 0.003f; // [GeV/ADC]

  // --- Private Helper Methods ---
  void genCalib();
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void GenDefaultCalib::genCalib()
{

  std::filesystem::create_directories(m_output_dir);

  std::string output = std::format("{}/local_calib_copy.root", m_output_dir);
  std::unique_ptr<CDBTTree> cdbttree = std::make_unique<CDBTTree>(output);

  unsigned int ntowers = static_cast<unsigned int>(myUtils::m_nphi * myUtils::m_neta);

  for (unsigned int channel = 0; channel < ntowers; ++channel)
  {
    int key = static_cast<int>(TowerInfoDefs::encode_emcal(channel));

    cdbttree->SetFloatValue(key, m_fieldname, m_calib_default);
  }

  cdbttree->Commit();
  cdbttree->WriteCDBTTree();
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
    GenDefaultCalib analysis(output_dir);
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
