// sPHENIX includes
#include <CDBUtils.C>

// c++ includes
#include <format>
#include <iostream>
#include <map>
#include <string>

void checkCDB(unsigned int runnumber, const std::string& dbtag = "newcdbtag")
{
  setGlobalTag(dbtag);

  std::map<std::string, std::string> cdb{
      {"Centrality", "Centrality"},
      {"Centrality Default", "Centrality_default"},
      {"Centrality Scale", "CentralityScale"},
      {"Centrality Scale Default", "CentralityScale_default"},
      {"Centrality Vertex Scale", "CentralityVertexScale"},
      {"Centrality Vertex Scale Default", "CentralityVertexScale_default"},
      {"MBD QFIT", "MBD_QFIT"},
      {"MBD QFIT Default", "MBD_QFIT_default"},
      {"EMCal Calib Default", "CEMC_calib_ADC_to_ETower_default"},
      {"EMCal Bad Tower Map", "CEMC_BadTowerMap"},
      {"EMCal Frac Bad Chi2", "CEMC_hotTowers_fracBadChi2"},
      {"EMCal Mean Time", "CEMC_meanTime"},
      {"HCALIN Frac Bad Chi2", "HCALIN_hotTowers_fracBadChi2"},
      {"HCALIN Mean Time", "HCALIN_meanTime"},
      {"HCALOUT Frac Bad Chi2", "HCALOUT_hotTowers_fracBadChi2"},
      {"HCALOUT Mean Time", "HCALOUT_meanTime"}
      };

  for (const auto& [description, name] : cdb)
  {
    std::string path = getCalibration(name, runnumber);
    std::cout << std::format("{}: {}, {}\n", description, name, path);
  }
}

#ifndef __CINT__
int main(int argc, const char* const argv[])
{
  const std::vector<std::string> args(argv, argv + argc);

  if (args.size() < 2 || args.size() > 3)
  {
    std::cerr << "usage: " << args[0] << " <runnumber> [dbtag]" << std::endl;
    std::cerr << "  runnumber: Run" << std::endl;
    std::cerr << "  dbtag: (optional) database tag (default: newcdbtag)" << std::endl;
    return 1;  // Indicate error
  }

  unsigned int runnumber = static_cast<unsigned int>(std::stoul(args[1]));
  std::string dbtag = "newcdbtag";

  if (args.size() >= 3)
  {
    dbtag = args[2];
  }

  checkCDB(runnumber, dbtag);

  std::cout << "======================================" << std::endl;
  std::cout << "done" << std::endl;
  return 0;
}
#endif
