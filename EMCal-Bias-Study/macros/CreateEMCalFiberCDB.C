// -- sPHENIX includes
#include <calobase/TowerInfoDefs.h>

// for CDBTTree
#include <cdbobjects/CDBTTree.h>

// -- root includes --
#include <TFile.h>
#include <TH2.h>

#include <filesystem>
#include <memory>
#include <format>
#include <string>
#include <iostream>

R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libcdbobjects.so)

void CreateEMCalFiberCDB(const std::filesystem::path &outputDir = ".")
{
  std::cout << std::format("Creating EMCal Fiber CDB in directory: {}\n", outputDir.string());

  // create output directories
  std::filesystem::create_directories(outputDir);

  std::string cemc_fiber = (outputDir / "CEMC_FiberType_default.root").string();

  std::unique_ptr<CDBTTree> cdbttree_fiber_cemc = std::make_unique<CDBTTree>(cemc_fiber);

  std::string emcal_block_info = "/direct/sphenix+u/anarde/Documents/sPHENIX/analysis-EMCal-Bias-Study/EMCal-Bias-Study/output/EMCal-block-info.root";

  std::cout << std::format("Reading EMCal block info from: {}\n", emcal_block_info);
  auto file = std::unique_ptr<TFile>(TFile::Open(emcal_block_info.c_str()));

  // Check if the file was opened successfully.
  if (!file || file->IsZombie())
  {
    std::cout << std::format("Error: Could not open file '{}'\n", emcal_block_info);
    return;
  }

  auto* h2FiberType = file->Get<TH2>("h2FiberType");

  int nTowers = 24576;
  std::string field_fiber = "fiber_type";

  for(int channel = 0; channel < nTowers; ++channel)
  {
    unsigned int key = TowerInfoDefs::encode_emcal(channel);
    int phi_bin = TowerInfoDefs::getCaloTowerPhiBin(key)+1;
    int eta_bin = TowerInfoDefs::getCaloTowerEtaBin(key)+1;

    int fiber_type = h2FiberType->GetBinContent(phi_bin, eta_bin);

    cdbttree_fiber_cemc->SetIntValue(key, field_fiber, fiber_type);
  }

  std::cout << std::format("Writing CDBTTree to: {}\n", cemc_fiber);
  cdbttree_fiber_cemc->Commit();
  cdbttree_fiber_cemc->WriteCDBTTree();

  std::cout << "Successfully created EMCal Fiber CDB.\n";
}
