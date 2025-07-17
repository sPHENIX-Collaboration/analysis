// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#pragma once

// -- sPHENIX
#include <fun4all/SubsysReco.h>

// -- c++
#include <cstdint> // Required for std::uint8_t, std::uint16_t, etc.
#include <map>
#include <memory>
#include <string>
#include <vector>

// -- ROOT
#include <TH1.h>
#include <TMath.h>

class PHCompositeNode;

class JetValidation : public SubsysReco
{
 public:
  explicit JetValidation(const std::string &name = "JetValidation");

  int Init(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;

  void set_filename(const std::string &file)
  {
    m_outfile_name = file;
  }

private:
  int process_event_check(PHCompositeNode *topNode);
  int process_centrality(PHCompositeNode *topNode);

  int m_event;

  std::string m_outfile_name;

  unsigned int m_bins_zvtx;
  double m_zvtx_low;
  double m_zvtx_high;

  unsigned int m_bins_cent;
  double m_cent_low;
  double m_cent_high;

  enum class EventType : std::uint8_t
  {
    ALL,
    ZVTX10,
    ZVTX10_MB
  };

  std::vector<std::string> m_eventType = {"All", "|z| < 10 cm", "|z| < 10 cm & MB"};

  // Event Vars
  double m_zvtx;
  double m_cent;

  // Cuts
  double m_zvtx_max;

  // Logging Info
  double m_cent_min;
  double m_cent_max;

  std::map<std::string, std::unique_ptr<TH1>> m_hists;
  std::map<std::string, int> m_ctr;
};
