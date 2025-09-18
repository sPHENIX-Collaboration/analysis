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
class FillGaps
{
 public:
  // The constructor takes the configuration
  FillGaps(std::string input_cdbttree, std::string output_cdbttree, float calib_default)
    : m_input_cdbttree(std::move(input_cdbttree))
    , m_output_cdbttree(std::move(output_cdbttree))
    , m_calib_default(calib_default)
  {
  }

  void run()
  {
    fillGaps();
  }

 private:
  // Configuration stored as members
  std::filesystem::path m_input_cdbttree;
  std::filesystem::path m_output_cdbttree;
  float m_calib_default{0.003f};  // [GeV/ADC]

  std::map<std::string, int> m_ctr;
  std::string m_fieldname{"CEMC_calib_ADC_to_ETower"};

  // --- Private Helper Methods ---
  void fillGaps();
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void FillGaps::fillGaps()
{
  std::filesystem::create_directories(m_output_cdbttree.parent_path());

  std::unique_ptr<CDBTTree> cdbttree_input = std::make_unique<CDBTTree>(m_input_cdbttree);
  std::unique_ptr<CDBTTree> cdbttree_output = std::make_unique<CDBTTree>(m_output_cdbttree);

  unsigned int ntowers = static_cast<unsigned int>(myUtils::m_nphi * myUtils::m_neta);

  for (unsigned int channel = 0; channel < ntowers; ++channel)
  {
    int key = static_cast<int>(TowerInfoDefs::encode_emcal(channel));

    float val = cdbttree_input->GetFloatValue(key, m_fieldname);

    // if calib is nonzero then copy it
    if (val)
    {
      cdbttree_output->SetFloatValue(key, m_fieldname, val);
    }
    // Fill the Gaps with default if no calib value exists
    else
    {
      cdbttree_output->SetFloatValue(key, m_fieldname, m_calib_default);
    }
  }

  cdbttree_output->Commit();
  cdbttree_output->WriteCDBTTree();
}

int main(int argc, const char *const argv[])
{
  gROOT->SetBatch(true);

  if (argc < 2 || argc > 4)
  {
    std::cout << "Usage: " << argv[0] << " <input_cdbttree> [output_cdbttree] [calib_default]" << std::endl;
    return 1;
  }

  std::string input_cdbttree = argv[1];
  std::string output_cdbttree = (argc >= 3) ? argv[2] : "test.root";
  float calib_default = (argc >= 4) ? std::stof(argv[3]) : 0.003f;

  try
  {
    FillGaps analysis(input_cdbttree, output_cdbttree, calib_default);
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
