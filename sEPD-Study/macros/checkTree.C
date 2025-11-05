// ====================================================================
// Standard C++ Includes
// ====================================================================
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <numbers>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

// ====================================================================
// ROOT Includes
// ====================================================================
#include <TFile.h>
#include <TROOT.h>
#include <TTree.h>

// ====================================================================
// The Analysis Class
// ====================================================================
class CheckTree
{
 public:
  // The constructor takes the configuration
  CheckTree(std::string input_file_list, std::string output)
    : m_input_file_list(std::move(input_file_list))
    , m_output(std::move(output))
  {
  }

  void run()
  {
    process();
  }

 private:
  // Configuration stored as members
  std::string m_input_file_list;
  std::string m_output;

  // --- Private Helper Methods ---
  void process();
  static std::string check_tree(const std::string& filename);
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================

void CheckTree::process()
{
   std::ofstream output_file(m_output); 
   std::ifstream input_file(m_input_file_list);

  if (!output_file.is_open())
  {
    throw std::runtime_error(std::format("Error: Could not open file {}\n", m_output));
  }

  if (!input_file.is_open())
  {
    throw std::runtime_error(std::format("Error: Could not open file {}\n", m_input_file_list));
  }

  std::string line;

  output_file << "file,status" << std::endl;

  std::map<std::string, int> ctr;

  while (std::getline(input_file, line))
  {
    if (ctr["total_files"]++ % 20 == 0)
    {
      std::cout << std::format("Processed: {}\n", ctr["total_files"]-1);
    }
    std::filesystem::path p(line);
    std::string stem = p.stem().string();

    std::string status = check_tree(line);
    ++ctr[status];

    output_file << std::format("{},{}\n", stem, status);
  }

  // Print Stats
  std::cout << std::format("{:#<20}\n", "");
  for (const auto& [name, counts] : ctr)
  {
      std::cout << std::format("{}: {}\n", name, counts);
  }
  std::cout << std::format("{:#<20}\n", "");
}

std::string CheckTree::check_tree(const std::string& filename)
{
  std::unique_ptr<TFile> file(TFile::Open(filename.c_str(), "READ"));

  // 1. Check for basic corruption (Zombie state)
  if (!file || file->IsZombie())
  {
    return "corrupt";
  }

  // 2. Check for tree existence and emptiness
  TTree* tree = dynamic_cast<TTree*>(file->Get("T"));

  if (!tree)
  {
    return "missing";
  }

  if (tree->GetEntries() == 0)
  {
    return "empty";
  }

  return "ok";
}

// ====================================================================
// Main function remains clean and simple
// ====================================================================
int main(int argc, const char* const argv[])
{
  gROOT->SetBatch(true);

  if (argc < 2 || argc > 3)
  {
    std::cout << "Usage: " << argv[0] << " input_file_list [output]" << std::endl;
    return 1;
  }

  const std::string input_file_list = argv[1];
  const std::string output = (argc >= 3) ? argv[2] : "tree-check-log.txt"; 

  std::cout << std::format("{:#<20}\n", "");
  std::cout << std::format("Run Params\n");
  std::cout << std::format("Input: {}\n", input_file_list);
  std::cout << std::format("Output: {}\n", output);
  std::cout << std::format("{:#<20}\n", "");

  try
  {
    CheckTree analysis(input_file_list, output);
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
