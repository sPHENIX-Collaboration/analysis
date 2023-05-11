

#ifndef __EpFinderEval_H__
#define __EpFinderEval_H__

#include <fun4all/SubsysReco.h>
#include <string>
#include <vector>

//Forward declerations
class PHCompositeNode;
class TFile; 
class TTree;
class EpFinder; 
class PHG4TruthInfoContainer;
class RawTowerContainer; 
class RawTowerGeomContainer; 

#define PHI_BINS 20

#define FPRIM_PHI_BINS 20
#define FPRIM_ETA_BINS 20
#define RFPRIM_ETA_BINS 5

//Brief: basic ntuple and histogram creation for sim evaluation
class EpFinderEval: public SubsysReco
{
 public: 
  //Default constructor
  EpFinderEval(const std::string &name="EpFinderEval");

  //Initialization, called for initialization
  int Init(PHCompositeNode *);

  int InitRun(PHCompositeNode *); 

  //Process Event, called for each event
  int process_event(PHCompositeNode *);

  //End, write and close files
  int End(PHCompositeNode *);

  //Change output filename
  void set_filename(const char* file)
  { if(file) _outfile_name = file; }

 private:
  //output filename
  std::string _outfile_name;
   
  //Event counter
  int _event;

  //User modules
  void fill_tree(PHCompositeNode*);

  //Get all the nodes
  int GetNodes(PHCompositeNode *);
  
  // Sum tower energy
  void sumTowerEnergy( PHCompositeNode *topNode, std::string detName ); 

  // Determine phi,eta bin
  int GetPhiBin(float tphi, float numPhiDivisions); 
  int GetEtaBin(float teta, float eta_low, float eta_high, float numEtaDivisions); 

  //TTrees
  TTree* _eval_tree_event;

  float rplane_angle; 
  float bimpact; 
  float prim_rplane_angle; 

  float fprim_rplane_angle; 
  float fprim_phiweighted_rplane_angle; 
  float fprim_phiweightedandshifted_rplane_angle; 

  float rfprim_rplane_angle; 
  float rfprim_phiweighted_rplane_angle; 
  float rfprim_phiweightedandshifted_rplane_angle; 

  float femc_raw_rplane_angle; 
  float femc_phiweighted_rplane_angle; 
  float femc_phiweightedandshifted_rplane_angle; 

  float rfemc_raw_rplane_angle; 
  float rfemc_phiweighted_rplane_angle; 
  float rfemc_phiweightedandshifted_rplane_angle; 

  float femcL_raw_rplane_angle; 
  float femcL_phiweighted_rplane_angle; 
  float femcL_phiweightedandshifted_rplane_angle; 

  float femcR_raw_rplane_angle; 
  float femcR_phiweighted_rplane_angle; 
  float femcR_phiweightedandshifted_rplane_angle; 

  float femc_h_clE; 

  PHG4TruthInfoContainer* _truth_container;
  RawTowerContainer* towers; 
  RawTowerGeomContainer *towergeom; 

  EpFinder *RpFinder; 
  EpFinder *rRpFinder; 
  EpFinder *RpFinderL; 
  EpFinder *RpFinderR; 
  EpFinder *primRpFinder; 
  EpFinder *fprimRpFinder; 
  EpFinder *rfprimRpFinder; 

  // phi binning

  std::vector<std::pair<int,int>> phi_list[PHI_BINS]; 
  std::vector<std::pair<int,int>> rphi_list[PHI_BINS]; 
  std::vector<std::pair<int,int>> fprim_phi_list[FPRIM_PHI_BINS]; 
  std::vector<std::pair<int,int>> rfprim_phi_list[FPRIM_PHI_BINS]; 

};

#endif //* __EpFinderEval_H__ *//
