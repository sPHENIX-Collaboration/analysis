// -- c++ includes --
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <map>
#include <format>

// -- root includes --
#include <TFile.h>

#include <calobase/TowerInfoDefs.h>
#include <cdbobjects/CDBTTree.h>

// sPHENIX includes
#include <CDBUtils.C>

R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libcdbobjects.so)
R__LOAD_LIBRARY(libsphenixnpc.so)

void fixMCMaps(const std::string &payload, const std::string &output)
{
  std::map<int, int> status_to_zscore =
  {
      {0, 0}
    , {1, 0}
    , {2, 6}
    , {3, -6}
  };

  int nTowers = 24576;
  std::string fieldname_hotMap = "status";
  std::string fieldname_z_score = "CEMC_sigma";

  std::unique_ptr<CDBTTree> cdb_hot = std::make_unique<CDBTTree>(payload);
  std::unique_ptr<CDBTTree> cdb_hot_new = std::make_unique<CDBTTree>(output);

  for (int towerIndex = 0; towerIndex < nTowers; ++towerIndex)
  {
    unsigned int key = TowerInfoDefs::encode_emcal(towerIndex);
    int hotMap_val = cdb_hot->GetIntValue(key, fieldname_hotMap);
    float hotMap_sigma = status_to_zscore[hotMap_val];

    cdb_hot_new->SetIntValue(key, fieldname_hotMap, hotMap_val);
    cdb_hot_new->SetFloatValue(key, fieldname_z_score, hotMap_sigma);
  }

  cdb_hot_new->Commit();
  cdb_hot_new->WriteCDBTTree();

  std::cout << std::format("Successfully Created: {}", output) << std::endl;
}

void fixMCMaps()
{
  setGlobalTag("MDC2");

  unsigned int runnumber = 1;

  std::filesystem::path cemc_hot_default = getCalibration("CEMC_hotTowers_status", runnumber);
  std::filesystem::path cemc_hot_40 = getCalibration("CEMC_hotTowers_status_40", runnumber);
  std::filesystem::path cemc_hot_60 = getCalibration("CEMC_hotTowers_status_60", runnumber);

  std::cout << std::format("default: {}", cemc_hot_default.stem().string()) << std::endl;
  std::cout << std::format("40: {}", cemc_hot_40.stem().string()) << std::endl;
  std::cout << std::format("60: {}", cemc_hot_60.stem().string()) << std::endl;

  std::string outputDir = "cdb-MC";
  std::filesystem::create_directories(outputDir);

  std::string output_default = std::format("{}/{}.root", outputDir, cemc_hot_default.stem().string());
  std::string output_40 = std::format("{}/{}.root", outputDir, cemc_hot_40.stem().string());
  std::string output_60 = std::format("{}/{}.root", outputDir, cemc_hot_60.stem().string());

  fixMCMaps(cemc_hot_default, output_default);
  fixMCMaps(cemc_hot_40, output_40);
  fixMCMaps(cemc_hot_60, output_60);
}
