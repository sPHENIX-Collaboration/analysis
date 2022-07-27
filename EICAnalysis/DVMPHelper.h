#ifndef __DVMPHELPER__
#define __DVMPHELPER__

#include <vector>
#include <stdlib.h>
class DVMPHelper
{

 public:
  /* Primary constructor */
  DVMPHelper(std::vector<float> reco_eta, std::vector<float> reco_phi,
	     std::vector<float> reco_ptotal, std::vector<int> reco_charge,
	     std::vector<float> reco_cluster_e,
	     std::vector<bool> reco_is_scattered_lepton,
	     std::vector<float> true_eta, std::vector<float> true_phi,
	     std::vector<float> true_ptotal, std::vector<int> pid,
	     std::vector<bool> is_scattered_lepton);

  /* Default destructor */
  ~DVMPHelper() {}

  /* Reco Particle Struct */
  struct particle_reco{
    float eta;
    float phi;
    float ptotal;
    int charge;
    float e;
    bool is_scattered_lepton;
  };

  /* True Particle Struct */ 
  struct particle_truth{
    float eta;
    float phi;
    float ptotal;
    int pid;
    bool is_scattered_lepton;
  };

  /* Return invariant mass float from reco particles */
  std::vector<float> calculateInvariantMass_1();
  
  /* Return invariant mass float from truth particles */
  std::vector<float> calculateInvariantMass_2();

  /* Return invariant mass float from reco particles (decay particles) */
  std::vector<float> calculateInvariantMass_3();

  /* Return invariant mass float from reco particles (positron+scattered electron */
  std::vector<float> calculateInvariantMass_4();

  /* Return invariant mass float from truth particles (decay particles) */
  std::vector<float> calculateInvariantMass_5();
  
  /* Return invariant mass float from truth particles (positron+scattered e-) */
  std::vector<float> calculateInvariantMass_6();

  
  /* Size of reco/truth */
  int _size_reco;
  int _size_truth;

  /* Array of event particles */
  DVMPHelper::particle_reco * rparticles;
  DVMPHelper::particle_truth * tparticles;

  /* Return reco particle */
  particle_reco GetParticleReco(float eta, float phi, float ptotal, int charge, float e,bool is_scattered_lepton);

  /* Return truth particle */
  particle_truth GetParticleTruth(float eta, float phi, float ptotal, int pid, bool is_scattered_lepton);
  
  /* Return invariant mass from reco particles */
  float get_invariant_mass(DVMPHelper::particle_reco, DVMPHelper::particle_reco);

  /* Return invariant mass from truth particles */
  float get_invariant_mass(DVMPHelper::particle_truth, DVMPHelper::particle_truth);

  /* Quick method to grab transverse momentum */
  float get_pt(float eta, float ptotal);

  /* Test if a positron was found in the event */
  bool find_positron();

  /* Test if particle passes E/p cut */
  bool pass_cut(int index);

 private:
  
  
};

#endif
