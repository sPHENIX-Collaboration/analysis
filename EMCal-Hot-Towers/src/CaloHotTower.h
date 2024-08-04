// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALOHOTTOWER_H
#define CALOHOTTOWER_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

#include <TH1.h>

class PHCompositeNode;
class CDBTTree;

class CaloHotTower : public SubsysReco
{
 public:

  CaloHotTower(const std::string &name = "CaloHotTower");

  ~CaloHotTower() override;

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

  Int_t readHotTowerIndexFile();

  void setOutputFile(const std::string &outputFile) {
    this->m_outputFile = outputFile;
  }

  void setHotTowerIndexFile(const std::string &hotTowerIndexFile) {
    this->m_hotTowerIndexFile = hotTowerIndexFile;
  }

 private:

  UInt_t iEvent;

  Float_t energy_min;
  Float_t energy_max;

  UInt_t bins_energy;
  Float_t energy_low;
  Float_t energy_high;

  UInt_t bins_events;

  std::string m_emcTowerNode;
  std::string m_outputFile;
  std::string m_hotTowerIndexFile;
  std::string m_calibName_hotMap;

  std::vector<std::pair<UInt_t,UInt_t>> hotTowerIndex;

  std::vector<TH1F*> hRefTowerEnergy; // filled only for runs where tower is identified as good (status = 0)
  std::vector<TH1F*> hHotTowerEnergy; // filled for only runs where tower is identified as hot (status = 2)
  std::vector<TH1F*> hHotTowerComplementEnergy; // filled only for runs where tower is identified as not hot (status != 2)
  TH1F* hEvents;

  CDBTTree* m_cdbttree_hotMap;
};

#endif // CALOHOTTOWER_H
