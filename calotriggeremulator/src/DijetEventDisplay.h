#ifndef CALOTRIGGER_DIJET_H
#define CALOTRIGGER_DIJET_H
#include "TH2D.h"
#include <string>
#include <phool/PHCompositeNode.h>
#include <ffarawobjects/Gl1Packetv2.h>
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfoContainerv3.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfov1.h>
#include <calobase/TowerInfov2.h>
#include <calobase/TowerInfov3.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <fun4all/Fun4AllHistoManager.h>

class TH2D;
class Fun4AllHistoManager;

class DijetEventDisplay
{
 public:
  DijetEventDisplay(const std::string f);
  ~DijetEventDisplay();
  
  void Dump();
  void setFile(const std::string f){ file = f; };
  int FillEvent(PHCompositeNode *topNode, float aj, float dphi);

 private:
  std::string file;
  TH2D *h_emcal{nullptr};
  TH2D *h_hcalin{nullptr};
  TH2D *h_hcalout{nullptr};
  TH2D *h_all{nullptr};
  
  Fun4AllHistoManager *hm{nullptr};
};

#endif /* CALOTRIGGER_TRIGGERANALYZER_H */

