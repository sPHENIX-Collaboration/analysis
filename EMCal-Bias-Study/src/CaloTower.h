// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALOTOWER_H
#define CALOTOWER_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <unordered_map>

#include <TH1.h>

class PHCompositeNode;

class CaloTower : public SubsysReco
{
 public:

  CaloTower(const std::string &name = "CaloTower");

  ~CaloTower() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  void setOutputFile(const std::string &outputFile) {
    this->m_outputFile = outputFile;
  }

  void set_nphi_range(const Int_t nphi_low, const Int_t nphi_high) {
    this->m_nphi_low = std::max(0, nphi_low);
    this->m_nphi_high = std::min(m_nphi-1, nphi_low);
  }

  void set_neta_range(const Int_t neta_low, const Int_t neta_high) {
    this->m_neta_low = std::max(0, neta_low);
    this->m_neta_high = std::min(m_neta-1, neta_low);
  }

 private:
  std::string m_outputFile;
  std::string m_emcTowerNode;

  Int_t m_Event;

  Int_t m_nphi;
  Int_t m_neta;

  Float_t m_min_energy;
  Float_t m_max_energy;

  Int_t m_nphi_low;
  Int_t m_nphi_high;

  Int_t m_neta_low;
  Int_t m_neta_high;

  Int_t m_nsamples;

  Int_t m_bins_adc;
  Float_t m_adc_low;
  Float_t m_adc_high;

  Int_t m_min_adc;
  Int_t m_max_adc;

  std::unordered_map<std::string,TH1*> m_hists;
};

#endif // CALOTOWER_H
