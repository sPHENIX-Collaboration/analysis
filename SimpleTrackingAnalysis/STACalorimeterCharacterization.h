#ifndef __STACALORIMETERCHARACTERIZATION_H__
#define __STACALORIMETERCHARACTERIZATION_H__


// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <vector>

class PHCompositeNode;
//class PHG4HoughTransform;
class TH1D;
class TH2D;
//class TProfile;
class TProfile2D;
class RawTower;
class RawCluster;
class RawTowerContainer;
class RawClusterContainer;

class STACalorimeterCharacterization: public SubsysReco
{

 public:

  STACalorimeterCharacterization(const std::string &name="STACalorimeterCharacterization");

  int Init(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);

  void set_nlayers(unsigned int x) {nlayers = x;}

  void set_verbosity(int x) {verbosity = x;}
  //void set_docalocuts(bool x) {docalocuts = x;}

  std::vector<RawTower*> get_ordered_towers(RawCluster*, RawTowerContainer*);
  std::vector<RawTower*> get_ordered_towers(const RawTowerContainer*);
  std::vector<RawCluster*> get_ordered_clusters(const RawClusterContainer*);
  void inspect_ordered_towers(const std::vector<RawTower*>&);
  void inspect_ordered_towers(const std::vector<RawTower*>&, int);
  void inspect_ordered_towers(const std::vector<RawTower*>&, double, int);


 private:

  // all values taken from sPHENIX pCDR
  // note the somewhat mysterious overlap
  // between the outer edge of the EMC
  // and the inner edge of the inner HCal
  // that shouldn't be a problem here, though
  const double emc_radius_inner =  90.0;
  const double hci_radius_inner = 115.7;
  const double hco_radius_inner = 182.0;
  const double emc_radius_outer = 116.1;
  const double hci_radius_outer = 137.0;
  const double hco_radius_outer = 268.5;

  // PHG4HoughTransform _hough; ERROR

  double magneticfield;

  int verbosity;
  //bool docalocuts;

  // event counter
  unsigned long long nevents;
  unsigned long long nerrors;
  unsigned long long nwarnings;

  // number of layers
  unsigned int nlayers;

  // output histograms ---------------------------------------------------------



  TH2D* _energy_ratio_emc;
  TH2D* _energy_ratio_hci;
  TH2D* _energy_ratio_hco;
  TH2D* _energy_ratio_hct;
  TH2D* _energy_ratio_tot_dumb;
  TH2D* _energy_ratio_tot_smart;

  TH2D* _energy_ratio_elb_emc;
  TH2D* _energy_ratio_elb_hci;
  TH2D* _energy_ratio_elb_hco;
  TH2D* _energy_ratio_elb_hct;
  TH2D* _energy_ratio_elb_tot_dumb;
  TH2D* _energy_ratio_elb_tot_smart;

  TH2D* _energy_ratio_eub_emc;
  TH2D* _energy_ratio_eub_hci;
  TH2D* _energy_ratio_eub_hco;
  TH2D* _energy_ratio_eub_hct;
  TH2D* _energy_ratio_eub_tot_dumb;
  TH2D* _energy_ratio_eub_tot_smart;

  TH2D* _energy_dphi_emc;
  TH2D* _energy_dphi_hci;
  TH2D* _energy_dphi_hco;

  TH2D* _energy_deta_emc;
  TH2D* _energy_deta_hci;
  TH2D* _energy_deta_hco;

  TProfile2D* _towers_3x3_emc;
  TProfile2D* _towers_5x5_emc;
  TProfile2D* _towers_7x7_emc;
  TProfile2D* _towers_9x9_emc;

  TH2D* _tower_energy_emc[10];
  TH2D* _towersum_energy_emc[10];

  TProfile2D* _towers_3x3_hci;
  TProfile2D* _towers_5x5_hci;
  TProfile2D* _towers_7x7_hci;
  TProfile2D* _towers_9x9_hci;

  TH2D* _tower_energy_hci[10];
  TH2D* _towersum_energy_hci[10];

  TProfile2D* _towers_3x3_hco;
  TProfile2D* _towers_5x5_hco;
  TProfile2D* _towers_7x7_hco;
  TProfile2D* _towers_9x9_hco;

  TH2D* _tower_energy_hco[10];
  TH2D* _towersum_energy_hco[10];

};

#endif // __STACALORIMETERCHARACTERIZATION_H__
