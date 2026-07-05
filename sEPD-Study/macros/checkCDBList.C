// sPHENIX includes
#include <CDBUtils.C>

// c++ includes
#include <format>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <vector>

void checkCDBList(const std::vector<unsigned int>& runnumbers, const std::filesystem::path& outDir = "test", const std::string& dbtag = "newcdbtag")
{
  if (runnumbers.empty())
  {
    std::cout << "Error: No runs provided." << std::endl;
    return;
  }

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
      {"EMCal Calib", "CEMC_calib_ADC_to_ETower"},
      {"EMCal Bad Tower Map", "CEMC_BadTowerMap"},
      {"EMCal Frac Bad Chi2", "CEMC_hotTowers_fracBadChi2"},
      {"EMCal Mean Time", "CEMC_meanTime"},
      {"HCALIN Frac Bad Chi2", "HCALIN_hotTowers_fracBadChi2"},
      {"HCALIN Mean Time", "HCALIN_meanTime"},
      {"HCALOUT Frac Bad Chi2", "HCALOUT_hotTowers_fracBadChi2"},
      {"HCALOUT Mean Time", "HCALOUT_meanTime"},
      {"sEPD Calib", "SEPD_NMIP_CALIB"},
      {"sEPD Event Plane", "SEPD_EventPlaneCalib"}
  };

  // Map to store lists of successful runs for each calibration
  std::map<std::string, std::vector<unsigned int>> valid_runs;

  // Map to store boolean calibration status for each run and calib type
  std::map<unsigned int, std::map<std::string, bool>> run_calib_status;

  // Initialize the vectors so every domain shows up in the summary, even if empty
  for (const auto& [description, name] : cdb)
  {
    valid_runs[name] = std::vector<unsigned int>();
  }

  size_t total_runs = runnumbers.size();
  std::cout << "Checking " << total_runs << " runs against DB tag: " << dbtag << "...\n";

  int progress_interval = 50;

  // Check all runs
  for (int progress = 0; unsigned int runnumber : runnumbers)
  {
    if (progress++ % progress_interval == 0)
    {
      std::cout << std::format("Processed: {}, {:.2f}%\n", progress, progress * 100. / static_cast<double>(total_runs));
    }

    for (const auto& [description, name] : cdb)
    {
      std::string path = getCalibration(name, runnumber);

      // If it doesn't start with the exception, the calibration exists
      bool has_calib = !path.starts_with("DataBaseException");
      run_calib_status[runnumber][name] = has_calib;
      if (has_calib)
      {
        valid_runs[name].push_back(runnumber);
      }
    }
  }

  std::cout << "\nWriting individual run lists to disk...\n";

  std::filesystem::path validDir = outDir / "valid_runs";
  std::filesystem::path missingDir = outDir / "missing_runs";
  std::filesystem::create_directories(validDir);
  std::filesystem::create_directories(missingDir);

  // Write out the run lists
  for (const auto& [description, name] : cdb)
  {
    std::filesystem::path validFilename = validDir / (name + "_runs.txt");
    std::ofstream validOut(validFilename);

    if (validOut.is_open())
    {
      for (unsigned int r : valid_runs[name])
      {
        validOut << r << "\n";
      }
      validOut.close();
    }
    else
    {
      std::cout << "Failed to open " << validFilename << " for writing.\n";
    }

    std::vector<unsigned int> missing_runs;
    for (unsigned int run : runnumbers)
    {
      if (!run_calib_status[run][name])
      {
        missing_runs.push_back(run);
      }
    }

    if (!missing_runs.empty())
    {
      std::filesystem::path missingFilename = missingDir / (name + "_runs_missing.txt");
      std::ofstream missingOut(missingFilename);

      if (missingOut.is_open())
      {
        for (unsigned int r : missing_runs)
        {
          missingOut << r << "\n";
        }
        missingOut.close();
      }
      else
      {
        std::cout << "Failed to open " << missingFilename << " for writing.\n";
      }
    }
  }

  // Write out the CSV file
  std::filesystem::path csvFilename = outDir / "calibrations.csv";
  std::ofstream csvFile(csvFilename);

  if (csvFile.is_open())
  {
    csvFile << "Run";
    for (const auto& [description, name] : cdb)
    {
      csvFile << "," << name;
    }
    csvFile << "\n";

    for (unsigned int run : runnumbers)
    {
      csvFile << run;
      for (const auto& [description, name] : cdb)
      {
        csvFile << "," << (run_calib_status[run][name] ? 1 : 0);
      }
      csvFile << "\n";
    }
    csvFile.close();
  }
  else
  {
    std::cout << "Failed to open " << csvFilename << " for writing.\n";
  }

  // Print Summary
  std::cout << "\n======================================\n";
  std::cout << "          CALIBRATION SUMMARY         \n";
  std::cout << "======================================\n";
  for (const auto& [description, name] : cdb)
  {
    size_t count = valid_runs[name].size();
    double percentage = (static_cast<double>(count) / static_cast<double>(total_runs)) * 100.0;

    // {:<35} pads the description with spaces to align the columns cleanly
    std::cout << std::format("{:<35} : {:>4} / {} ({:>5.1f}%)\n",
                             description, count, total_runs, percentage);
  }
}

#ifndef __CINT__
int main(int argc, const char* const argv[])
{
  const std::vector<std::string> args(argv, argv + argc);

  if (args.size() < 2 || args.size() > 4)
  {
    std::cout << "usage: " << args[0] << " <run_list_file> [outDir] [dbtag]" << std::endl;
    std::cout << "  run_list_file: Text file containing run numbers (one per line)" << std::endl;
    std::cout << "  outDir: (optional) output directory (default: test)" << std::endl;
    std::cout << "  dbtag: (optional) database tag (default: newcdbtag)" << std::endl;
    return 1;
  }

  // Read run numbers from the provided file
  std::vector<unsigned int> runnumbers;
  std::ifstream infile(args[1]);

  if (!infile.is_open())
  {
    std::cout << "Error: Could not open file " << args[1] << std::endl;
    return 1;
  }

  unsigned int run;
  while (infile >> run)
  {
    runnumbers.push_back(run);
  }
  infile.close();

  std::filesystem::path outDir = "test";
  std::string dbtag = "newcdbtag";

  if (args.size() >= 3)
  {
    outDir = args[2];
  }

  if (args.size() >= 4)
  {
    dbtag = args[3];
  }

  checkCDBList(runnumbers, outDir, dbtag);

  std::cout << "======================================" << std::endl;
  std::cout << "done" << std::endl;
  return 0;
}
#endif
