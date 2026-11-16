// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef KFP_SINGLESPINASYMMETRIES_H
#define KFP_SINGLESPINASYMMETRIES_H

#include <fun4all/SubsysReco.h>

#include <string>

class PHCompositeNode;

class TTree;

class KFP_singlespinasymmetries : public SubsysReco
{
 public:

  KFP_singlespinasymmetries(const std::string &name = "KFP_singlespinasymmetries");

  ~KFP_singlespinasymmetries() override;
  int Init(PHCompositeNode *topNode) override;
  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;
  int Reset(PHCompositeNode * /*topNode*/) override;

  // what you ganna find? ex: K_S0, rho0, D0, ..., i mean, who is your mother
  void setMotherParticleName(std::string name){input_mother_particle = name;}
  void setMotherParticlePDG(int pdg){input_mother_particle_pdg = pdg;} // ...... it would be good to know what is the pdg number of your mother
  void setKFPContainerName(std::string name){m_kfpcontainername = name + "_KFParticle_Container";}
  void setOutputName(std::string name){m_outputfilename = name;}


 private:

  // output tree
  TTree *ks_tree;

  // spin stuff
  int   bluepattern[120];
  int   yellowpattern[120];
  float bluepolarization;
  float bluepolarizationerr;
  float yellowpolarization;
  float yellowpolarizationerr;
  int   xingshift;
  int   bluespin;
  int   yellspin;
  int   candidate_crossing;


  // candidate stuff

  /* mother */
  int   mother_pdg;
  float mother_mass;
  float mother_pt;
  float mother_pt_err;
  float mother_pz;
  float mother_xf;
  float mother_e;
  float mother_eta;
  float mother_eta_err;
  float mother_phi;
  float mother_phi_err;
  float mother_chi2;
  int   mother_ndf;
  float mother_decaylength;
  float mother_decaylengthxy;
  

  /* daughters */
  // just simply add two daughters. 
  int   daughter_1_id;
  float daughter_1_pt;
  float daughter_1_pz;
  // float daughter_1_xf; // i dont know what is this
  float daughter_1_eta;
  float daughter_1_phi;
  float daughter_1_chi2;
  int   daughter_1_ndf;
  int   daughter_1_charge;
  float daughter_1_dca_XY;
  int   daughter_1_nMVTX;
  int   daughter_1_nINTT;
  int   daughter_1_nTPC;
  

  int   daughter_2_id;
  float daughter_2_pt;
  float daughter_2_pz;
  // float daughter_2_xf;
  float daughter_2_eta;
  float daughter_2_phi;
  float daughter_2_chi2;
  int   daughter_2_ndf;
  int   daughter_2_charge;
  float daughter_2_dca_XY;
  int   daughter_2_nMVTX;
  int   daughter_2_nINTT;
  int   daughter_2_nTPC;


  std::string m_kfpcontainername;
  std::string m_outputfilename;
  std::string input_mother_particle; // "K_S0"
  int input_mother_particle_pdg;
};

#endif // KFP_SINGLESPINASYMMETRIES_H
