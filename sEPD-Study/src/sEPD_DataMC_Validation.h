// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

// -- sPHENIX
#include <fun4all/SubsysReco.h>

// -- c++
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

// -- ROOT
#include <TFile.h>
#include <TH1.h>

class PHCompositeNode;

class sEPD_DataMC_Validation : public SubsysReco
{
 public:
  explicit sEPD_DataMC_Validation(const std::string &name = "sEPD_DataMC_Validation");

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  // Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void set_filename(const std::string &file)
  {
    m_outfile_name = file;
  }

 private:
  int process_event_check(PHCompositeNode *topNode);
  int process_sEPD(PHCompositeNode *topNode);
  int process_centrality(PHCompositeNode *topNode);

  std::string m_outfile_name{"test.root"};

  unsigned int m_bins_cent{100};
  double m_cent_low{-0.5};
  double m_cent_high{99.5};

  // Event Vars
  double m_cent{9999};

  std::map<std::string, std::unique_ptr<TH1>> m_hists;
  std::map<std::string, int> m_ctr;
};
