// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef ISOCLUSTER_H
#define ISOCLUSTER_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

#include <HepMC/GenParticle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>

class PHCompositeNode;
class TFile;
class TTree;

class pythiaEMCalAna : public SubsysReco
{
 public:

  pythiaEMCalAna(const std::string &name = "pythiaEMCalAna");

  ~pythiaEMCalAna() override;

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

  void setGenEvent(int eventGet)     {getEvent = eventGet;}

 private:

  TTree *clusters_Towers;
  TTree *truth_particles;

  //stuff for towers and clusters
  std::vector<float> m_tower_energy;
  std::vector<float> m_eta_center;
  std::vector<float> m_phi_center;
  std::vector<float> m_cluster_e;
  std::vector<float> m_cluster_eta;
  std::vector<float> m_cluster_phi;
  std::vector<float> m_cluster_ecore;
  std::vector<float> m_cluster_chi2;
  std::vector<float> m_cluster_prob;
  std::vector<float> m_cluster_nTowers;
  
  //truth particle information
  std::vector<float> m_truthBarcode;
  std::vector<float> m_truthParentBarcode;
  std::vector<float> m_truthIsPrimary;
  std::vector<float> m_truthPid;
  std::vector<float> m_truthE;
  std::vector<float> m_truthEta;
  std::vector<float> m_truthPhi;
  std::vector<float> m_truthPt;
  std::vector<float> m_truthMass;
  std::vector<float> m_truthEndVtx_x;
  std::vector<float> m_truthEndVtx_y;
  std::vector<float> m_truthEndVtx_z;
  std::vector<float> m_truthEndVtx_t;
  std::vector<float> m_truthEndVtx_r;

  TFile *fout;
  std::string outname;
  int getEvent;
  long int n_direct_photons;
  long int n_direct_photons_in_acceptance;
  long int n_pythia_direct_photons;
  long int n_decay_photons;
  long int n_pythia_decays;
  long int n_geant_decays;
  long int n_primary;
  long int n_pythia_decay_photons;
  long int n_pythia_decayed_pi0s;
  long int n_pythia_nondecayed_pi0s;
  long int n_geant_primary_pi0s;
  std::vector<int> pythia_primary_barcodes;

  std::vector<int> primaryBarcodes;
  std::vector<int> secondaryBarcodes;
  HepMC::GenParticle* getGenParticle(int barcode, HepMC::GenEvent* theEvent);
  PHG4VtxPoint* getG4EndVtx(int id, PHG4TruthInfoContainer* truthInfo);
  bool vector_contains(int val, std::vector<int> vec);
  /* // case 1: primary that geant knows about, decay handled by geant */
  /* void addPrimaryFromGeant(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo); */
  /* // case 2: primary that geant doesn't know about, decay handled by pythia */
  /* void addPrimaryFromPythia(HepMC::GenParticle* part); */
  /* // case 3: secondary that geant knows about */
  /* void addSecondary(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo); */
  /* // edge case: geant knows about secondary, but not its parent */
  /* void addSecondaryWithoutParent(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo, HepMC::GenParticle* genParent); */

  // case 1: pythia handles decay
  /* void addPrimaryFromPythia(HepMC::GenParticle* part); */
  /* void addSecondaryFromPythia(HepMC::GenParticle* part, HepMC::GenParticle* parent); */
  /* void addPrimaryFromGeant(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo); */
  /* void addSecondaryFromGeant(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo); */


  bool isDirectPhoton(PHG4Particle* part, HepMC::GenEvent* theEvent);
  void addDirectPhoton(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo);
  void addDecayPhoton(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo, HepMC::GenEvent* theEvent);
  void addPrimaryHadronFromPythia(HepMC::GenParticle* part);
  void addPrimaryHadronFromGeant(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo);
};

#endif // ISOCLUSTER_H
