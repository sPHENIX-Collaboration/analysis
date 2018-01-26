#ifndef __HCALIB_H__
#define __HCALIB_H__

#include <fun4all/SubsysReco.h>
#include <string>

class PHCompositeNode;
class RawTowerContainer;
class RawTowerGeomContainer;
class PHG4CellContainer;
class PHG4ScintillatorSlatContainer;
class TFile;
class TH1F;
class RawTower;

#include <g4detectors/PHG4ScintillatorSlatDefs.h>
#include <calobase/RawTowerDefs.h>
#include <map>

class HCalib : public SubsysReco
{
 public:
  typedef std::map<PHG4ScintillatorSlatDefs::keytype,TH1F *> SlatMap;
  typedef std::map<RawTowerDefs::keytype, TH1F *> TowerMap;
  
  HCalib();
  virtual ~HCalib() {}

  int Init(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);
  int genkey(const int detid, const int etabin, const int phibin);
  enum detid { HCALIN=0, HCALOUT=1 };
 protected:
  bool is_proto;
  void GetNodes(PHCompositeNode *topNode, const std::string &det = "None");

  PHCompositeNode *topNode;
  RawTowerContainer *towers;
  PHG4CellContainer* slats;
  PHG4ScintillatorSlatContainer* proto_slats;
  float threshold;
  TFile *outfile;
  SlatMap slat_hists;
  TowerMap tower_hists;
  bool fill_towers;
  bool fill_slats;
};

#endif
