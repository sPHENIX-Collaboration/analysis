// ====================================================================
// sPHENIX Includes
// ====================================================================
#include <calobase/TowerInfoDefs.h>

// ====================================================================
// ROOT Includes
// ====================================================================
#include <TChain.h>
#include <TF1.h>
#include <TFile.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TH1.h>
#include <TKey.h>
#include <TROOT.h>

// ====================================================================
// Standard C++ Includes
// ====================================================================
#include <concepts>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <unordered_set>

namespace myUtils
{
  void setEMCalDim(TH1* hist);

  std::pair<Int_t, Int_t> getSectorIB(Int_t iphi, Int_t ieta);
  std::pair<Int_t, Int_t> getSectorIB(Int_t towerIndex);
  std::vector<std::string> split(const std::string& s, char delimiter);
  TFitResultPtr doGausFit(TH1* hist, Double_t start, Double_t end, const std::string& name = "fitFunc");
  std::unique_ptr<TChain> setupTChain(const std::string& input_filepath, const std::string& tree_name_in_file);
  std::map<std::string, std::unique_ptr<TH1>> read_hists(const std::string& file_name, const std::string& tag = "", std::unordered_set<std::string>* names = nullptr);

  template <typename Func>
  concept InvocableWithString = std::invocable<Func, const std::string&>;

  template <InvocableWithString Callable>
  bool readCSV(const std::filesystem::path& filePath, Callable lineHandler, bool skipHeader = true);

  enum class QVecAna
  {
    DEFAULT,  // All rbins
    HALF,     // ONLY RBINS 0 to 7
    HALF1,    // ONLY RBINS 1 to 7
    HALF2     // ONLY RBINS 8 to 15
  };

  const std::map<std::string, QVecAna> q_vec_ana_map{{"DEFAULT", QVecAna::DEFAULT},
                                                     {"HALF", QVecAna::HALF},
                                                     {"HALF1", QVecAna::HALF1},
                                                     {"HALF2", QVecAna::HALF2}};

  bool filter_sEPD(int rbin, QVecAna ana);

  Int_t m_nsector = 64;
  Int_t m_nchannel_per_sector = 384;
  Int_t m_nchannel_per_ib = 64;
  Int_t m_nphi = 256;
  Int_t m_neta = 96;
  Int_t m_nib = 384;
  Int_t m_nib_per_sector = 6;
  Int_t m_ntowIBSide = 8;
}  // namespace myUtils

bool myUtils::filter_sEPD(int rbin, QVecAna ana)
{
  return (ana == QVecAna::DEFAULT) ||
         (ana == QVecAna::HALF && rbin <= 7) ||
         (ana == QVecAna::HALF1 && rbin <= 7 && rbin >= 1) || 
         (ana == QVecAna::HALF2 && rbin >= 8);
}

// Function to encapsulate the TChain setup with error checking
std::unique_ptr<TChain> myUtils::setupTChain(const std::string& input_filepath, const std::string& tree_name_in_file)
{
  // 1. Pre-check: Does the file exist at all? (C++17 filesystem or traditional fstream)
  if (!std::filesystem::exists(input_filepath))
  {  // Using fs::exists
    std::cout << "Error: Input file does not exist: " << input_filepath << std::endl;
    return nullptr;  // Return a null unique_ptr indicating failure
  }

  // 2. Open the file to check for the TTree directly
  // Use TFile::Open and unique_ptr for robust file handling (RAII)
  std::unique_ptr<TFile> file_checker(TFile::Open(input_filepath.c_str(), "READ"));

  if (!file_checker || file_checker->IsZombie())
  {
    std::cout << "Error: Could not open file " << input_filepath << " to check for TTree." << std::endl;
    return nullptr;
  }

  // Check if the TTree exists in the file
  // Get() returns a TObject*, which can be cast to TTree*.
  // If the object doesn't exist or isn't a TTree, Get() returns nullptr.
  TTree* tree_obj = dynamic_cast<TTree*>(file_checker->Get(tree_name_in_file.c_str()));
  if (!tree_obj)
  {
    std::cout << "Error: TTree '" << tree_name_in_file << "' not found in file " << input_filepath << std::endl;
    return nullptr;
  }
  // File will be automatically closed by file_checker's unique_ptr destructor

  // 3. If everything checks out, create and configure the TChain
  std::unique_ptr<TChain> chain = std::make_unique<TChain>(tree_name_in_file.c_str());
  if (!chain)
  {  // Check if make_unique failed (e.g. out of memory)
    std::cout << "Error: Could not create TChain object." << std::endl;
    return nullptr;
  }

  chain->Add(input_filepath.c_str());

  // 4. Verify TChain's state (optional but good final check)
  // GetEntries() will be -1 if no valid trees were added.
  if (chain->GetEntries() == 0)
  {
    std::cout << "Warning: TChain has 0 entries after adding file. This might indicate a problem." << std::endl;
    // Depending on your logic, you might return nullptr here too.
  }
  else
  {
    std::cout << "Successfully set up TChain for tree '" << tree_name_in_file
              << "' from file '" << input_filepath << "'. Entries: " << chain->GetEntries() << std::endl;
  }

  return chain;  // Return the successfully created and configured TChain
}

/**
 * @brief Reads all objects inheriting from TH1 from a ROOT file.
 *
 * @param file_name The path to the ROOT file.
 * @param tag The tag to append to the histogram name (ex: runnumber).
 * @return A map where the key is the histogram name (TKey name) and the value
 * is a std::unique_ptr<TH1> managing the histogram object.
 * Returns an empty map if the file cannot be opened.
 */
std::map<std::string, std::unique_ptr<TH1>> myUtils::read_hists(const std::string& file_name, const std::string& tag, std::unordered_set<std::string>* names)
{
  // This is crucial. It tells ROOT not to automatically add histograms
  // to the gDirectory (which is usually the TFile). This ensures that
  // when the TFile is closed, it doesn't delete the histograms,
  // allowing our std::unique_ptr to manage their lifetime.
  TH1::AddDirectory(kFALSE);

  std::map<std::string, std::unique_ptr<TH1>> hists_map;

  auto file = std::unique_ptr<TFile>(TFile::Open(file_name.c_str()));

  // Check if the file was opened successfully.
  if (!file || file->IsZombie())
  {
    std::cout << std::format("Error: Could not open file '{}'\n", file_name);
    return {};  // Return an empty map
  }

  // Get the list of keys (objects) in the file
  TList* keyList = file->GetListOfKeys();
  if (!keyList)
  {
    std::cout << std::format("Error: Could not get list of keys\n");
    return {};  // Return an empty map
  }

  // Use a TIter to loop through the keys
  TIter next(keyList);
  TKey* key;

  std::cout << std::format("Reading histograms from {}\n", file_name);

  while ((key = static_cast<TKey*>(next())))
  {
    // Get the class of the object from its key
    TClass* cl = gROOT->GetClass(key->GetClassName());

    // Check if the class inherits from TH1 (base histogram class)
    if (cl && cl->InheritsFrom("TH1"))
    {
      std::string name = key->GetName();

      if (names && !names->contains(name))
      {
        continue;
      }

      std::string new_name = name + tag;
      std::cout << std::format("... Reading Hist: {}\n", name);

      // Read the object, cast it, and emplace it into the map.
      // The std::unique_ptr takes ownership of the raw pointer.
      hists_map.emplace(new_name, std::unique_ptr<TH1>(static_cast<TH1*>(file->Get(name.c_str())->Clone(new_name.c_str()))));
    }
  }

  // The TFile unique_ptr goes out of scope here and closes the file,
  // but the histograms remain in memory, managed by the unique_ptrs
  // in the map we are about to return.

  return hists_map;
}

TFitResultPtr myUtils::doGausFit(TH1* hist, Double_t start, Double_t end, const std::string& name)
{
  // fit calib hist
  TF1* fitFunc = new TF1(name.c_str(), "gaus", start, end);
  Double_t initialAmplitude = hist->GetMaximum();
  Double_t initialMean = hist->GetMean();
  Double_t initialSigma = hist->GetRMS();

  fitFunc->SetParameter(0, initialAmplitude);
  fitFunc->SetParameter(1, initialMean);
  fitFunc->SetParameter(2, initialSigma);

  // You can also set parameter names for better readability in the stats box
  fitFunc->SetParName(0, "Amplitude");
  fitFunc->SetParName(1, "Mean");
  fitFunc->SetParName(2, "Sigma");

  // Set some visual properties for the fit line
  fitFunc->SetLineColor(kRed);
  fitFunc->SetLineWidth(2);
  fitFunc->SetLineStyle(kDashed);  // Optional: make it dashed

  TFitResultPtr fitResult = hist->Fit(fitFunc, "RS");  // Fit within range, store result, quiet

  if (fitResult.Get())
  {  // Check if TFitResultPtr is valid
    std::cout << "\n----------------------------------------------------" << std::endl;
    std::cout << "Fit Results for function: " << fitFunc->GetName() << std::endl;
    std::cout << "----------------------------------------------------" << std::endl;
    std::cout << "Fit Status: " << fitResult->Status() << " (0 means successful)" << std::endl;
    if (fitResult->IsValid())
    {  // Check if the fit is valid (e.g., covariance matrix is good)
      std::cout << "Fit is Valid." << std::endl;
      for (UInt_t i = 0; i < static_cast<UInt_t>(fitFunc->GetNpar()); ++i)
      {
        std::cout << "Parameter " << fitFunc->GetParName(static_cast<Int_t>(i)) << " (" << i << "): "
                  << fitResult->Parameter(i) << " +/- " << fitResult->ParError(i) << std::endl;
      }
      std::cout << "Resolution: Sigma/Mean = " << fitResult->Parameter(2) / fitResult->Parameter(1) << std::endl;
      std::cout << "Chi^2 / NDF: " << fitResult->Chi2() << " / " << fitResult->Ndf()
                << " = " << (fitResult->Ndf() > 0 ? fitResult->Chi2() / fitResult->Ndf() : 0) << std::endl;
      std::cout << "Probability: " << TMath::Prob(fitResult->Chi2(), static_cast<Int_t>(fitResult->Ndf())) << std::endl;
    }
    else
    {
      std::cout << "Fit is NOT Valid." << std::endl;
    }
    std::cout << "----------------------------------------------------" << std::endl;
  }
  else
  {
    std::cout << "Fit did not return a valid TFitResultPtr." << std::endl;
  }

  return fitResult;
}

/**
 * @brief Reads a CSV (or any line-delimited) file and applies a handler function to each line.
 *
 * @tparam Callable The type of the function/lambda to be called for each line.
 * Must be invocable with a 'const std::string&'.
 * @param filePath The path to the input file.
 * @param lineHandler A function, lambda, or functor that takes a 'const std::string&' (the line)
 * and processes it.
 * @param skipHeader If true, the first line of the file will be read and discarded.
 * @return true if the file was successfully opened and read, false otherwise.
 */
template <myUtils::InvocableWithString Callable>  // Using the more general concept for wider applicability
bool myUtils::readCSV(const std::filesystem::path& filePath, Callable lineHandler, bool skipHeader)
{
  std::ifstream file(filePath);

  if (!file.is_open())
  {
    std::cout << "Error: [" << filePath.string() << "] Could not open file." << std::endl;
    return false;
  }

  std::string line;

  if (skipHeader && std::getline(file, line))
  {
    // First line read and discarded (header)
  }

  while (std::getline(file, line))
  {
    // Optional: Handle potential Windows CRLF (\r\n) issues if the file might
    // have them and you're on a system that only expects \n.
    // std::getline usually handles this, but if \r remains:
    // if (!line.empty() && line.back() == '\r') {
    //     line.pop_back();
    // }
    lineHandler(line);  // Call the user-provided function for each line
  }

  // Check for errors during read operations (other than EOF)
  if (file.bad())
  {
    std::cout << "Error: [" << filePath.string() << "] I/O error while reading file." << std::endl;
    return false;
  }
  // file.eof() will be true if EOF was reached.
  // file.fail() might be true if getline failed not due to eof (e.g., badbit also set).
  // If badbit is not set and eof() is true, it's a successful read to the end.

  return true;  // Successfully processed or reached EOF
}

std::vector<std::string> myUtils::split(const std::string& s, const char delimiter)
{
  std::vector<std::string> result;

  std::stringstream ss(s);
  std::string temp;

  while (getline(ss, temp, delimiter))
  {
    if (!temp.empty())
    {
      result.push_back(temp);
    }
  }

  return result;
}

void myUtils::setEMCalDim(TH1* hist)
{
  hist->GetXaxis()->SetLimits(0, m_nphi);
  hist->GetXaxis()->SetNdivisions(32, false);
  hist->GetXaxis()->SetLabelSize(0.04F);
  hist->GetXaxis()->SetTickSize(0.01F);
  hist->GetYaxis()->SetTickSize(0.01F);
  hist->GetYaxis()->SetLabelSize(0.04F);
  hist->GetYaxis()->SetLimits(0, m_neta);
  hist->GetYaxis()->SetNdivisions(12, false);
  hist->GetYaxis()->SetTitleOffset(0.5);
  hist->GetXaxis()->SetTitleOffset(1);
}

std::pair<Int_t, Int_t> myUtils::getSectorIB(Int_t iphi, Int_t ieta)
{
  unsigned int key = TowerInfoDefs::encode_emcal(static_cast<unsigned int>(ieta), static_cast<unsigned int>(iphi));
  unsigned int towerIndex = TowerInfoDefs::decode_emcal(key);
  return TowerInfoDefs::getEMCalSectorIB(towerIndex);
}
