// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALOHOTTOWERSIM_H
#define CALOHOTTOWERSIM_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

#include <TH2.h>

class PHCompositeNode;
class CDBTTree;

class CaloHotTowerSim : public SubsysReco
{
 public:

  CaloHotTowerSim(const std::string &name = "CaloHotTowerSim");

  ~CaloHotTowerSim() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  Int_t Init(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  Int_t process_event(PHCompositeNode *topNode) override;

  /// Called at the end of all processing.
  Int_t End(PHCompositeNode *topNode) override;

  void setOutputFile(const std::string &outputFile) {
    this->m_outputFile = outputFile;
  }

 private:

  UInt_t m_iEvent;
  UInt_t m_bins_events;

  UInt_t m_bins_phi;
  UInt_t m_bins_eta;

  Float_t m_min_towerE;
  Float_t m_energy_min;
  Float_t m_energy_max;

  UInt_t m_avgBadTowers;

  std::string m_emcTowerNode;
  std::string m_outputFile;

  TH1* hEvents;
  TH2* h2TowerEnergy;
  TH2* h2TowerControlEnergy;
};

#endif // CALOHOTTOWERSIM_H
