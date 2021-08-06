#ifndef __EpFinderReco_H__
#define __EpFinderReco_H__

#include <fun4all/SubsysReco.h>
#include <string>
#include <vector>

//Forward declerations
class PHCompositeNode;
class TFile; 
class TTree;
class EpFinder; 
class EpInfo; 
class PHG4TruthInfoContainer;
class RawTowerContainer; 
class RawTowerGeomContainer; 

#define PHI_BINS 20
#define FPRIM_PHI_BINS 20
#define FPRIM_ETA_BINS 20

//Brief: basic ntuple and histogram creation for sim evaluation
class EpFinderReco: public SubsysReco
{
 public: 
  //Default constructor
  EpFinderReco(const std::string &name="EpFinderReco");

  //Initialization, called for initialization
  int Init(PHCompositeNode *);

  int InitRun(PHCompositeNode *); 

  //Process Event, called for each event
  int process_event(PHCompositeNode *);

  //End, write and close files
  int EndRun(PHCompositeNode *);
  int End(PHCompositeNode *);

  void set_algo_node(const std::string &algonode) { _algonode = algonode; }
  void set_input_node(const std::string &inputnode) { _inputnode = inputnode; }

 private:
   
  //User modules
  void GetEventPlanes(PHCompositeNode*);

  //Get all the nodes
  int GetNodes(PHCompositeNode *);

  //Get all the nodes
  int CreateNodes(PHCompositeNode *);
  
  // Determine phi,eta bin
  int GetPhiBin(float tphi, float numPhiDivisions); 
  int GetEtaBin(float teta, float eta_low, float eta_high, float numEtaDivisions); 

  // Nodes
  PHG4TruthInfoContainer* _truth_container;
  RawTowerContainer *towers; 
  RawTowerGeomContainer *towergeom;

  RawTowerContainer *cemctowers;
  RawTowerContainer *hcalotowers; 
  RawTowerContainer *hcalitowers; 
  
  RawTowerGeomContainer *cemctowergeom;
  RawTowerGeomContainer *hcalotowergeom;
  RawTowerGeomContainer *hcalitowergeom;

  EpInfo *_prim_EpInfo;
  EpInfo *_fprim_EpInfo; 

  EpInfo *_CEMC_EpInfo; 
  EpInfo *_CEMCHCAL_EpInfo; 

  EpFinder *CEMC_EpFinder;
  EpFinder *CEMC_HCAL_EpFinder; 
  EpFinder *prim_EpFinder; 
  EpFinder *fprim_EpFinder;
    

  // phi binning

  std::vector<std::pair<int,int>> phi_list[PHI_BINS]; 
  std::vector<std::pair<int,int>> fprim_phi_list[FPRIM_PHI_BINS]; 
  std::vector<std::pair<int,int>> cemc_phi_list[256]; 
  std::vector<std::pair<int,int>> hcalout_phi_list[64]; 
  std::vector<std::pair<int,int>> hcalin_phi_list[64]; 

  
  // Node names
  std::string _algonode;
  std::string _inputnode;

};

#endif //* __EpFinderReco_H__ *//

