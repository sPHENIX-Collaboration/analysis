// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALOTOWER_H
#define CALOTOWER_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>
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

  void setDoAllWaveforms(const Bool_t doAllWaveforms) {
    this->m_doAllWaveforms = doAllWaveforms;
  }

  void add_nphi_neta_low(Int_t nphi_low, Int_t neta_low) {
    this->m_nphi_neta_low.push_back(std::make_pair(nphi_low,neta_low));
  }

 private:
  std::string m_outputFile;
  std::string m_emcTowerNode;

  Int_t m_Event;

  Int_t m_nphi;
  Int_t m_neta;

  Double_t m_min_energy;
  Double_t m_max_energy;

  std::vector<std::pair<Int_t,Int_t>> m_nphi_neta_low;
  Int_t m_ntowIBSide;

  Int_t m_nsamples;

  Int_t m_bins_adc;
  Double_t m_adc_low;
  Double_t m_adc_high;

  Int_t m_bins_ADC;
  Double_t m_ADC_low;
  Double_t m_ADC_high;

  Int_t m_min_adc;
  Int_t m_max_adc;

  Bool_t m_doAllWaveforms;

  std::unordered_map<std::string,TH1*> m_hists;
};

#endif // CALOTOWER_H
