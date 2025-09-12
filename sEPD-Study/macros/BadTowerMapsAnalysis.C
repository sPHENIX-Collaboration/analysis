#include "myUtils.C"

// sPHENIX includes
#include <CDBUtils.C>
#include <calobase/TowerInfoDefs.h>
#include <cdbobjects/CDBTTree.h>

// ====================================================================
// Standard C++ Includes
// ====================================================================
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// ====================================================================
// ROOT Includes
// ====================================================================
#include <TDatime.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile2D.h>
#include <TROOT.h>

// ====================================================================
// The Analysis Class
// ====================================================================
class BadTowerMapsAnalysis
{
 public:
  // The constructor takes the configuration
  BadTowerMapsAnalysis(std::string input_list, std::string input_timestamp_list, std::string output_dir, std::string dbtag, int verbosity)
    : m_input_list(std::move(input_list))
    , m_input_timestamp_list(std::move(input_timestamp_list))
    , m_output_dir(std::move(output_dir))
    , m_verbosity(verbosity)
  {
    setGlobalTag(dbtag);
  }

  void run()
  {
    read();
    init_hists();
    analyze();
    fill_results();
    save_results();
  }

 private:
  struct RunInfo
  {
    int badTowersDead{0};
    int badTowersHot{0};
    int badTowersCold{0};
    int badTowersBadChi2{0};

    int getBadTowers()
    {
      return badTowersDead + badTowersHot + badTowersCold + badTowersBadChi2;
    }
  };

  // --- Member Variables ---
  std::map<std::string, std::unique_ptr<TH1>> m_hists;
  std::map<std::string, unsigned int> m_runInfo;  // timestamp
  std::map<std::string, int> m_ctr;
  int m_run_start{9999999};
  int m_run_end{0};
  std::string m_calibName_chi2{"CEMC_hotTowers_fracBadChi2"};
  std::string m_fieldname_chi2{"fraction"};
  double m_fraction_badChi2_threshold{0.01};

  // Configuration stored as members
  std::string m_input_list;
  std::string m_input_timestamp_list;
  std::string m_output_dir;
  int m_verbosity;

  // --- Private Helper Methods ---
  std::string getRun(const std::string& input);
  void read_hists();
  void read_time();
  void read();
  void init_hists();
  RunInfo getBadTowers(TH2* hist, std::string& run);
  void analyze();
  void save_results();
  void fill_results();
};

std::string BadTowerMapsAnalysis::getRun(const std::string& input)
{
  // This pattern looks for:
  //   _      : a literal underscore
  //   (      : the start of a capturing group
  //   \\d+   : one or more digits (the '\\' escapes the backslash for the C++ string)
  //   )      : the end of the capturing group
  //   $      : the end of the string
  std::regex runnumber_regex("_(\\d+)$");
  std::smatch match_results;
  std::string runnumber;

  if (std::regex_search(input, match_results, runnumber_regex))
  {
    if (match_results.size() > 1)
    {
      runnumber = match_results[1].str();
    }
  }
  else
  {
    std::cout << std::format("Could not find a run number in '{}'.\n", input);
  }

  return runnumber;
}

void BadTowerMapsAnalysis::read_hists()
{
  std::cout << std::format("Processing: Read Hists\n");
  TH1::AddDirectory(kFALSE);

  std::ifstream file_list(m_input_list);
  if (!file_list.is_open())
  {
    throw std::runtime_error(std::format("Error: Could not open the input file list: {}", m_input_list));
  }
  std::string root_file_path;

  while (std::getline(file_list, root_file_path))
  {
    if (!root_file_path.empty())
    {
      auto file = std::unique_ptr<TFile>(TFile::Open(root_file_path.c_str()));

      // Check if the file was opened successfully.
      if (!file || file->IsZombie())
      {
        std::cout << std::format("Error: Could not open file '{}'. Skipping!\n", root_file_path);
        continue;
      }

      std::string stem = std::filesystem::path(root_file_path).stem().string();
      std::string runnumber = getRun(stem);
      if (runnumber.empty())
      {
        continue;
      }

      m_run_start = std::min(m_run_start, std::stoi(runnumber));
      m_run_end = std::max(m_run_end, std::stoi(runnumber));

      std::string name = std::format("h_hot_{}", runnumber);
      m_hists[name] = std::unique_ptr<TH2>(static_cast<TH2*>(file->Get("h_hot")->Clone(name.c_str())));

      if (m_verbosity && m_ctr["ctr_read_hists"] % 20 == 0)
      {
        std::cout << std::format("Progress: {}\n", m_ctr["ctr_read_hists"]);
      }

      ++m_ctr["ctr_read_hists"];
    }
  }

  std::cout << std::format("Finished Reading Hists: {}\n", m_ctr["ctr_read_hists"]);
}

void BadTowerMapsAnalysis::read_time()
{
  std::cout << std::format("Processing: Read Timestamp\n");

  std::ifstream input_file(m_input_timestamp_list);

  // Check if the file was successfully opened
  if (!input_file.is_open())
  {
    throw std::runtime_error(std::format("Error: Could not open the input file list: {}", m_input_timestamp_list));
  }

  std::string line;

  // Skip the header line
  if (!std::getline(input_file, line))
  {
    // Handle case where the file is empty
    return;
  }

  while (std::getline(input_file, line))
  {
    std::stringstream ss(line);
    std::string runnumber;
    std::string timestamp;

    // Parse the line using the comma as a delimiter
    // std::getline reads from the stringstream 'ss' into the variables
    if (std::getline(ss, runnumber, ',') && std::getline(ss, timestamp))
    {
      TDatime d(timestamp.c_str());
      m_runInfo[runnumber] = d.Convert();

      if (runnumber == std::to_string(m_run_start) || runnumber == std::to_string(m_run_end))
      {
        std::cout << std::format("Run: {}, Stamp: {}, Time: {}\n", runnumber, d.AsString(), d.Convert());
      }
    }

    if (m_verbosity && m_ctr["ctr_read_time"] % 100 == 0)
    {
      std::cout << std::format("Progress: {}\n", m_ctr["ctr_read_time"]);
    }
    ++m_ctr["ctr_read_time"];
  }

  std::cout << std::format("Finished Reading Timestamps: {}\n", m_runInfo.size());
}

void BadTowerMapsAnalysis::read()
{
  std::cout << std::format("Processing: Read\n");
  read_hists();
  read_time();
}

void BadTowerMapsAnalysis::init_hists()
{
  std::cout << std::format("Processing: Init Hists\n");

  unsigned int delta_time = 120;  // 2 minutes
  unsigned int start = m_runInfo[std::to_string(m_run_start)];
  unsigned int end = m_runInfo[std::to_string(m_run_end)];
  unsigned int bins = (end - start) / delta_time;

  TDatime st(start);
  TDatime ed(end);

  std::cout << std::format("Start Run: {}, Time: {}, Stamp: {}\n", m_run_start, start, st.AsString());
  std::cout << std::format("End Run: {}, Time: {}, Stamp: {}\n", m_run_end, end, ed.AsString());
  std::cout << std::format("Bins: {}\n", bins);

  m_hists["hDeadVsTime"] = std::make_unique<TH1F>("hDeadVsTime", "EMCal Dead Towers; Time; Towers", bins, start, end);
  m_hists["hHotVsTime"] = std::make_unique<TH1F>("hHotVsTime", "EMCal Hot Towers; Time; Towers", bins, start, end);
  m_hists["hColdVsTime"] = std::make_unique<TH1F>("hColdVsTime", "EMCal Cold Towers; Time; Towers", bins, start, end);
  m_hists["hBadChi2VsTime"] = std::make_unique<TH1F>("hBadChi2VsTime", "EMCal Bad #chi^{2} Towers; Time; Towers", bins, start, end);
  m_hists["hBadVsTime"] = std::make_unique<TH1F>("hBadVsTime", "EMCal Bad Towers; Time; Towers", bins, start, end);

  m_hists["h2Dead"] = std::make_unique<TProfile2D>("h2Dead", "EMCal Dead Towers Frac; Tower Index #phi; Tower Index #eta", myUtils::m_nphi, -0.5, myUtils::m_nphi - 0.5, myUtils::m_neta, -0.5, myUtils::m_neta - 0.5);
  m_hists["h2Hot"] = std::make_unique<TProfile2D>("h2Hot", "EMCal Hot Towers Frac; Tower Index #phi; Tower Index #eta", myUtils::m_nphi, -0.5, myUtils::m_nphi - 0.5, myUtils::m_neta, -0.5, myUtils::m_neta - 0.5);
  m_hists["h2Cold"] = std::make_unique<TProfile2D>("h2Cold", "EMCal Cold Towers Frac; Tower Index #phi; Tower Index #eta", myUtils::m_nphi, -0.5, myUtils::m_nphi - 0.5, myUtils::m_neta, -0.5, myUtils::m_neta - 0.5);
  m_hists["h2BadChi2"] = std::make_unique<TProfile2D>("h2BadChi2", "EMCal Bad #chi^{2} Towers Frac; Tower Index #phi; Tower Index #eta", myUtils::m_nphi, -0.5, myUtils::m_nphi - 0.5, myUtils::m_neta, -0.5, myUtils::m_neta - 0.5);
  m_hists["h2Bad"] = std::make_unique<TProfile2D>("h2Bad", "EMCal Bad Towers Frac; Tower Index #phi; Tower Index #eta", myUtils::m_nphi, -0.5, myUtils::m_nphi - 0.5, myUtils::m_neta, -0.5, myUtils::m_neta - 0.5);

  unsigned int bins_frac = 101;
  double frac_low = 0;
  double frac_high = 101;

  m_hists["hDead"] = std::make_unique<TH1F>("hDead", "EMCal Dead Towers; Frac Dead [% of Runs]; Towers", bins_frac, frac_low, frac_high);
  m_hists["hHot"] = std::make_unique<TH1F>("hHot", "EMCal Hot Towers; Frac Hot [% of Runs]; Towers", bins_frac, frac_low, frac_high);
  m_hists["hCold"] = std::make_unique<TH1F>("hCold", "EMCal Cold Towers; Frac Cold [% of Runs]; Towers", bins_frac, frac_low, frac_high);
  m_hists["hBadChi2"] = std::make_unique<TH1F>("hBadChi2", "EMCal Bad #chi^{2} Towers; Frac Bad #chi^{2} [% of Runs]; Towers", bins_frac, frac_low, frac_high);
  m_hists["hBad"] = std::make_unique<TH1F>("hBad", "EMCal Bad Towers; Frac Bad [% of Runs]; Towers", bins_frac, frac_low, frac_high);

  for (const auto& [name, hist] : m_hists)
  {
    if (name.ends_with("VsTime"))
    {
      hist->GetXaxis()->SetTimeDisplay(1);  // The X axis is a time axis
      hist->GetXaxis()->SetTimeFormat("%m/%d");
    }
  }
}

BadTowerMapsAnalysis::RunInfo BadTowerMapsAnalysis::getBadTowers(TH2* hist, std::string& run)
{
  std::string calib_chi2 = getCalibration(m_calibName_chi2, std::stoul(run));
  bool doHotChi2 = !calib_chi2.starts_with("DataBaseException");
  if(!doHotChi2)
  {
    std::cout << std::format("Warning: No {} found for Run: {}\n", m_calibName_chi2, run);
  }

  std::unique_ptr<CDBTTree> cdbttree_chi2 = (doHotChi2) ? std::make_unique<CDBTTree>(calib_chi2) : nullptr;

  RunInfo rinfo;
  for (int ieta = 1; ieta <= hist->GetNbinsX(); ++ieta)
  {
    for (int iphi = 1; iphi <= hist->GetNbinsY(); ++iphi)
    {
      int val = static_cast<int>(hist->GetBinContent(ieta, iphi));
      int eta = ieta - 1;
      int phi = iphi - 1;

      int key = static_cast<int>(TowerInfoDefs::encode_emcal(static_cast<unsigned int>(eta), static_cast<unsigned int>(phi)));
      float fraction_badChi2 = (doHotChi2) ? cdbttree_chi2->GetFloatValue(key, m_fieldname_chi2) : 0;
      bool isBadChi2 = fraction_badChi2 > m_fraction_badChi2_threshold;

      if(isBadChi2)
      {
        ++rinfo.badTowersBadChi2;
        dynamic_cast<TProfile2D*>(m_hists["h2BadChi2"].get())->Fill(phi, eta, 1);
      }
      else
      {
        dynamic_cast<TProfile2D*>(m_hists["h2BadChi2"].get())->Fill(phi, eta, 0);
      }

      if (val == 1)
      {
        ++rinfo.badTowersDead;
        dynamic_cast<TProfile2D*>(m_hists["h2Dead"].get())->Fill(phi, eta, 1);
      }
      else
      {
        dynamic_cast<TProfile2D*>(m_hists["h2Dead"].get())->Fill(phi, eta, 0);
      }
      if (val == 2)
      {
        ++rinfo.badTowersHot;
        dynamic_cast<TProfile2D*>(m_hists["h2Hot"].get())->Fill(phi, eta, 1);
      }
      else
      {
        dynamic_cast<TProfile2D*>(m_hists["h2Hot"].get())->Fill(phi, eta, 0);
      }
      if (val == 3)
      {
        ++rinfo.badTowersCold;
        dynamic_cast<TProfile2D*>(m_hists["h2Cold"].get())->Fill(phi, eta, 1);
      }
      else
      {
        dynamic_cast<TProfile2D*>(m_hists["h2Cold"].get())->Fill(phi, eta, 0);
      }

      if (val != 0 || isBadChi2)
      {
        dynamic_cast<TProfile2D*>(m_hists["h2Bad"].get())->Fill(phi, eta, 1);
      }
      else
      {
        dynamic_cast<TProfile2D*>(m_hists["h2Bad"].get())->Fill(phi, eta, 0);
      }
    }
  }

  return rinfo;
}

void BadTowerMapsAnalysis::analyze()
{
  std::cout << std::format("Processing: Analyze\n");

  // Check the dead towers in this time range of interest
  TDatime t0(2025, 7, 9, 0, 0, 0);
  TDatime t1(2025, 7, 18, 0, 0, 0);

  for (const auto& [name, hist] : m_hists)
  {
    if (name.starts_with("h_hot"))
    {
      if(m_ctr["ctr_runs"]++ % 20 == 0)
      {
        std::cout << std::format("Processed: {:4d} Runs\n", m_ctr["ctr_runs"]-1);
      }
      std::string run = getRun(name);
      int time = static_cast<int>(m_runInfo[run]);
      TDatime tdat(static_cast<unsigned int>(time));
      RunInfo rinfo = getBadTowers(dynamic_cast<TH2*>(hist.get()), run);
      int bin = m_hists["hDeadVsTime"]->FindBin(time);
      int val = static_cast<int>(m_hists["hDeadVsTime"]->GetBinContent(bin));
      if (val != 0)
      {
        std::cout << std::format("Warning: Bin {} already contains data, Overwriting!\n", bin);
      }
      if (m_verbosity && tdat >= t0 && tdat < t1)
      {
        std::cout << std::format("Run {}, Dead Towers: {}\n", run, rinfo.badTowersDead);
      }
      m_hists["hDeadVsTime"]->Fill(time, rinfo.badTowersDead);
      m_hists["hHotVsTime"]->Fill(time, rinfo.badTowersHot);
      m_hists["hColdVsTime"]->Fill(time, rinfo.badTowersCold);
      m_hists["hBadChi2VsTime"]->Fill(time, rinfo.badTowersBadChi2);
      m_hists["hBadVsTime"]->Fill(time, rinfo.getBadTowers());
    }
  }
}

void BadTowerMapsAnalysis::fill_results()
{
  std::cout << std::format("Processing: Fill Results\n");

  for (int ieta = 1; ieta <= myUtils::m_neta; ++ieta)
  {
    for (int iphi = 1; iphi <= myUtils::m_nphi; ++iphi)
    {
      double fracDead = dynamic_cast<TProfile2D*>(m_hists["h2Dead"].get())->GetBinContent(iphi, ieta)*100;
      double fracHot = dynamic_cast<TProfile2D*>(m_hists["h2Hot"].get())->GetBinContent(iphi, ieta)*100;
      double fracCold = dynamic_cast<TProfile2D*>(m_hists["h2Cold"].get())->GetBinContent(iphi, ieta)*100;
      double fracBadChi2 = dynamic_cast<TProfile2D*>(m_hists["h2BadChi2"].get())->GetBinContent(iphi, ieta)*100;
      double fracBad = dynamic_cast<TProfile2D*>(m_hists["h2Bad"].get())->GetBinContent(iphi, ieta)*100;

      m_hists["hDead"]->Fill(fracDead);
      m_hists["hHot"]->Fill(fracHot);
      m_hists["hCold"]->Fill(fracCold);
      m_hists["hBadChi2"]->Fill(fracBadChi2);
      m_hists["hBad"]->Fill(fracBad);
    }
  }
}

void BadTowerMapsAnalysis::save_results()
{
  std::cout << std::format("Processing: Save Results\n");

  std::filesystem::create_directories(m_output_dir);
  std::string output_filename = m_output_dir + "/test.root";
  auto output_file = std::make_unique<TFile>(output_filename.c_str(), "RECREATE");

  for (const auto& [name, hist] : m_hists)
  {
    if (!name.starts_with("h_hot"))
    {
      hist->Write();
    }
  }

  output_file->Close();

  std::cout << std::format("Results saved to: {}\n", output_filename);
}

// ====================================================================
// Main function remains clean and simple
// ====================================================================
int main(int argc, const char* const argv[])
{
  gROOT->SetBatch(true);

  if (argc < 3 || argc > 6)
  {
    std::cout << "Usage: " << argv[0] << " <input_list_file> <input_timestamp_list_file> [output_directory] [dbtag] [verbosity] " << std::endl;
    return 1;
  }

  const std::string input_list = argv[1];
  const std::string input_timestamp_list = argv[2];
  std::string output_dir = (argc >= 4) ? argv[3] : ".";
  std::string dbtag = (argc >= 5) ? argv[4] : "newcdbtag";
  int verbosity = (argc >= 6) ? std::atoi(argv[5]) : 0;

  try
  {
    BadTowerMapsAnalysis analysis(input_list, input_timestamp_list, output_dir, dbtag, verbosity);
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
