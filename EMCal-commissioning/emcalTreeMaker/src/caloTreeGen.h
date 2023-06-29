// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALOTREEGEN_H
#define CALOTREEGEN_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>
#include <TTree.h>

class TTree;
class PHCompositeNode;
class Fun4AllHistoManager;
class TFile;
class RawCluster;
class TowerInfoContainer;

class caloTreeGen : public SubsysReco
{
 public:

  caloTreeGen(const std::string &name = "caloTreeGen");

  ~caloTreeGen() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
  */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
  */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
  */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

  void setClusters(int clusters)  {doClusters = clusters;}

  void setFineClusters(int fineCluster) {doFineCluster = fineCluster;}
  
 private:

  TTree *T;
  
  std::vector<float> m_emcTowPhi;
  std::vector<float> m_emcTowEta;
  std::vector<float> m_emcTowE;
  std::vector<float> m_emcTiming;
  std::vector<float> m_emciEta;
  std::vector<float> m_emciPhi;

  std::vector<float> m_clusterE;
  std::vector<float> m_clusterPhi;
  std::vector<float> m_clusterEta;
  std::vector<float> m_clusterPt;
  std::vector<float> m_clusterChi;
  std::vector<float> m_clusterNtow;
  std::vector<float> m_clusterTowMax;
  std::vector<float> m_clusterECore;
  
  std::vector<std::vector<int> > m_clusTowEta;
  std::vector<std::vector<int> > m_clusTowPhi;
  std::vector<std::vector<float> > m_clusTowE;
  
    
  TFile *out;
  //Fun4AllHistoManager *hm = nullptr;
  std::string Outfile = "commissioning.root";

  unsigned int getCaloTowerPhiBin(const unsigned int key);
  unsigned int getCaloTowerEtaBin(const unsigned int key);
  float getMaxTowerE(RawCluster *cluster, TowerInfoContainer *towerContainer);
  std::vector<float> returnClusterTowE(RawCluster *cluster, TowerInfoContainer *towerContainer);
  std::vector<int> returnClusterTowPhi(RawCluster *cluster, TowerInfoContainer *towerContainer);
  std::vector<int> returnClusterTowEta(RawCluster *cluster, TowerInfoContainer *towerContainer);

;
  int doClusters;
  float totalCaloE;
  int doFineCluster;

};

#endif 
