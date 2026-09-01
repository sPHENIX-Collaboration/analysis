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
#include <TSystem.h>

R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libcdbobjects.so)

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
class UpdateChannelCalib
{
 public:
  // The constructor takes the configuration
  UpdateChannelCalib(std::string input_cdbttree, std::string output_cdbttree,
                     unsigned int ieta, unsigned int iphi, float calib_value,
                     std::string fieldname = "CEMC_calib_ADC_to_ETower")
    : m_input_cdbttree(std::move(input_cdbttree))
    , m_output_cdbttree(std::move(output_cdbttree))
    , m_target_ieta(ieta)
    , m_target_iphi(iphi)
    , m_calib_value(calib_value)
    , m_fieldname(std::move(fieldname))
  {
  }

  void run()
  {
    updateChannelCalib();
  }

 private:
  // Configuration stored as members
  std::filesystem::path m_input_cdbttree;
  std::filesystem::path m_output_cdbttree;
  unsigned int m_target_ieta{0};
  unsigned int m_target_iphi{0};
  float m_calib_value{0.0f};

  std::string m_fieldname{"CEMC_calib_ADC_to_ETower"};

  // --- Private Helper Methods ---
  void updateChannelCalib();
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void UpdateChannelCalib::updateChannelCalib()
{
  if (!std::filesystem::exists(m_input_cdbttree))
  {
    throw std::runtime_error(std::format("Input CDBTTree file does not exist: {}", m_input_cdbttree.string()));
  }

  if (m_target_ieta >= static_cast<unsigned int>(myUtils::m_neta) ||
      m_target_iphi >= static_cast<unsigned int>(myUtils::m_nphi))
  {
    throw std::out_of_range(std::format(
        "Target tower (ieta={}, iphi={}) is out of range. Valid bounds: ieta in [0, {}], iphi in [0, {}].",
        m_target_ieta, m_target_iphi, myUtils::m_neta - 1, myUtils::m_nphi - 1));
  }

  if (m_output_cdbttree.has_parent_path())
  {
    std::filesystem::create_directories(m_output_cdbttree.parent_path());
  }

  std::unique_ptr<CDBTTree> cdbttree_input = std::make_unique<CDBTTree>(m_input_cdbttree.string());
  std::unique_ptr<CDBTTree> cdbttree_output = std::make_unique<CDBTTree>(m_output_cdbttree.string());

  unsigned int target_key = TowerInfoDefs::encode_emcal(m_target_ieta, m_target_iphi);
  unsigned int ntowers = static_cast<unsigned int>(myUtils::m_nphi * myUtils::m_neta);

  float old_val = 0.0f;
  bool found = false;

  for (unsigned int channel = 0; channel < ntowers; ++channel)
  {
    int key = static_cast<int>(TowerInfoDefs::encode_emcal(channel));

    if (static_cast<unsigned int>(key) == target_key)
    {
      old_val = cdbttree_input->GetFloatValue(key, m_fieldname);
      cdbttree_output->SetFloatValue(key, m_fieldname, m_calib_value);
      found = true;
    }
    else
    {
      float val = cdbttree_input->GetFloatValue(key, m_fieldname);
      cdbttree_output->SetFloatValue(key, m_fieldname, val);
    }
  }

  cdbttree_output->Commit();
  cdbttree_output->WriteCDBTTree();

  if (found)
  {
    std::cout << std::format("Updated tower (ieta={}, iphi={}, key={}): old calib = {}, new calib = {}",
                             m_target_ieta, m_target_iphi, target_key, old_val, m_calib_value)
              << std::endl;
    std::cout << std::format("Saved output CDBTTree to {}", m_output_cdbttree.string()) << std::endl;
  }
}

// ====================================================================
// Standalone ROOT macro entry point
// ====================================================================
void updateChannelCalib(const std::string &input_cdbttree,
                        const std::string &output_cdbttree,
                        unsigned int ieta,
                        unsigned int iphi,
                        float calib_value,
                        const std::string &fieldname = "CEMC_calib_ADC_to_ETower")
{
  UpdateChannelCalib updater(input_cdbttree, output_cdbttree, ieta, iphi, calib_value, fieldname);
  updater.run();
}

// ====================================================================
// Main entry point for standalone executable
// ====================================================================
int main(int argc, const char *const argv[])
{
  gROOT->SetBatch(true);

  if (argc < 6 || argc > 7)
  {
    std::cout << "Usage: " << argv[0]
              << " <input_cdbttree> <output_cdbttree> <ieta> <iphi> <new_calib_value> [fieldname]"
              << std::endl;
    return 1;
  }

  std::string input_cdbttree = argv[1];
  std::string output_cdbttree = argv[2];
  unsigned int ieta = static_cast<unsigned int>(std::stoul(argv[3]));
  unsigned int iphi = static_cast<unsigned int>(std::stoul(argv[4]));
  float calib_value = std::stof(argv[5]);
  std::string fieldname = (argc >= 7) ? argv[6] : "CEMC_calib_ADC_to_ETower";

  try
  {
    UpdateChannelCalib updater(input_cdbttree, output_cdbttree, ieta, iphi, calib_value, fieldname);
    updater.run();
  }
  catch (const std::exception &e)
  {
    std::cout << std::format("An exception occurred: {}", e.what()) << std::endl;
    return 1;
  }

  std::cout << "Analysis complete." << std::endl;
  return 0;
}
