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

 private:
  std::string m_outputFile;
  std::string m_emcTowerNode;

  Int_t m_Event;

  Int_t m_nphi;
  Int_t m_neta;

  Float_t m_min_energy;
  Float_t m_max_energy;

  std::unordered_map<std::string,TH1*> m_hists;
};

#endif // CALOTOWER_H
