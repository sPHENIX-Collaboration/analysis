// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef ISOCLUSTER_H
#define ISOCLUSTER_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <set>
#include <vector>

#include <HepMC/GenParticle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4eval/CaloRawClusterEval.h>

class PHCompositeNode;
class TFile;
class TTree;
namespace HepMC {
    class GenEvent;
}
class RawClusterContainer;

class pythiaEMCalAna : public SubsysReco
{
 public:

  pythiaEMCalAna(const std::string &name = "pythiaEMCalAna", const std::string &oname = "pythiaEMCalAnaTrees.root", bool isMC = true, bool hasPythia = true);

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
  PHG4TruthInfoContainer* m_truthInfo;
  HepMC::GenEvent* m_theEvent;
  RawClusterContainer* m_clusterContainer;
  CaloRawClusterEval* m_clusterEval;

  //stuff for towers and clusters
  std::vector<float> m_tower_energy;
  std::vector<float> m_eta_center;
  std::vector<float> m_phi_center;
  std::vector<float> m_cluster_ID;
  std::vector<float> m_cluster_e;
  std::vector<float> m_cluster_eta;
  std::vector<float> m_cluster_phi;
  std::vector<float> m_cluster_ecore;
  std::vector<float> m_cluster_chi2;
  std::vector<float> m_cluster_prob;
  std::vector<float> m_cluster_nTowers;
  std::vector<std::vector<float>> m_cluster_allTowersE;
  std::vector<std::vector<float>> m_cluster_allTowersEta;
  std::vector<std::vector<float>> m_cluster_allTowersPhi;
  std::vector<float> m_cluster_nParticles;
  std::vector<float> m_cluster_primaryParticlePid;
  std::vector<std::vector<float>> m_cluster_allSecondaryPids;
  /* std::vector<float> m_cluster_maxE_trackID; */
  /* std::vector<float> m_cluster_maxE_PID; */
  /* std::vector<std::vector<float>> m_cluster_all_trackIDs; */
  
  //truth particle information
  /* std::vector<float> m_truthIsPrimary; */
  /* std::vector<float> m_truthTrackID; */
  /* std::vector<float> m_truthPid; */
  /* std::vector<float> m_truthE; */
  /* std::vector<float> m_truthEta; */
  /* std::vector<float> m_truthPhi; */
  /* std::vector<float> m_truthPt; */
  /* std::vector<float> m_truthMass; */
  /* std::vector<float> m_truthEndVtx_x; */
  /* std::vector<float> m_truthEndVtx_y; */
  /* std::vector<float> m_truthEndVtx_z; */
  /* std::vector<float> m_truthEndVtx_t; */
  /* std::vector<float> m_truthEndVtx_r; */
  /* std::vector<std::vector<float>> m_truth_all_clusterIDs; */

  // V3
  std::vector<float> m_truth_Parent_Barcode;
  std::vector<float> m_truth_Parent_Pid;
  std::vector<float> m_truth_Parent_M;
  std::vector<float> m_truth_Parent_E;
  std::vector<float> m_truth_Parent_Eta;
  std::vector<float> m_truth_Parent_Phi;
  std::vector<float> m_truth_Parent_Pt;
  std::vector<float> m_truth_Parent_xF;
  std::vector<float> m_truth_Parent_EndVtx_x;
  std::vector<float> m_truth_Parent_EndVtx_y;
  std::vector<float> m_truth_Parent_EndVtx_z;
  /* std::vector<float> m_truth_Parent_EndVtx_t; */
  std::vector<float> m_truth_Parent_EndVtx_r;
  /* std::vector<float> m_truth_Parent_TotalNDaughters; */
  /* std::vector<std::vector<float>> m_truth_Parent_AllDaughterPids; */
  /* std::vector<std::vector<float>> m_truth_Parent_AllDaughterEnergyFractions; */
  std::vector<float> m_truth_Parent_TotalNClusters;
  std::vector<std::vector<float>> m_truth_Parent_AllClusterIDs;
  std::vector<std::vector<float>> m_truth_Parent_AllClusterEnergyFractions;
  std::vector<float> m_truth_Decay1_Barcode;
  std::vector<float> m_truth_Decay1_Pid;
  std::vector<float> m_truth_Decay1_M;
  std::vector<float> m_truth_Decay1_E;
  std::vector<float> m_truth_Decay1_Eta;
  std::vector<float> m_truth_Decay1_Phi;
  std::vector<float> m_truth_Decay1_Pt;
  std::vector<float> m_truth_Decay1_xF;
  std::vector<float> m_truth_Decay1_EndVtx_x;
  std::vector<float> m_truth_Decay1_EndVtx_y;
  std::vector<float> m_truth_Decay1_EndVtx_z;
  /* std::vector<float> m_truth_Decay1_EndVtx_t; */
  std::vector<float> m_truth_Decay1_EndVtx_r;
  std::vector<float> m_truth_Decay1_TotalNClusters;
  std::vector<float> m_truth_Decay1_BestClusterID;
  std::vector<float> m_truth_Decay1_BestClusterEfraction;
  /* std::vector<std::vector<float>> m_truth_Decay1_AllClusterIDs; */
  /* std::vector<std::vector<float>> m_truth_Decay1_AllClusterEnergyFractions; */
  std::vector<float> m_truth_Decay2_Barcode;
  std::vector<float> m_truth_Decay2_Pid;
  std::vector<float> m_truth_Decay2_M;
  std::vector<float> m_truth_Decay2_E;
  std::vector<float> m_truth_Decay2_Eta;
  std::vector<float> m_truth_Decay2_Phi;
  std::vector<float> m_truth_Decay2_Pt;
  std::vector<float> m_truth_Decay2_xF;
  std::vector<float> m_truth_Decay2_EndVtx_x;
  std::vector<float> m_truth_Decay2_EndVtx_y;
  std::vector<float> m_truth_Decay2_EndVtx_z;
  /* std::vector<float> m_truth_Decay2_EndVtx_t; */
  std::vector<float> m_truth_Decay2_EndVtx_r;
  std::vector<float> m_truth_Decay2_TotalNClusters;
  std::vector<float> m_truth_Decay2_BestClusterID;
  std::vector<float> m_truth_Decay2_BestClusterEfraction;
  /* std::vector<std::vector<float>> m_truth_Decay2_AllClusterIDs; */
  /* std::vector<std::vector<float>> m_truth_Decay2_AllClusterEnergyFractions; */
  /* std::vector<float> m_truth_Cluster1_Id; */
  /* std::vector<float> m_truth_Cluster1_E; */
  /* std::vector<float> m_truth_Cluster1_Ecore; */
  /* std::vector<float> m_truth_Cluster1_Eta; */
  /* std::vector<float> m_truth_Cluster1_Phi; */
  /* std::vector<float> m_truth_Cluster1_Pt; */
  /* std::vector<float> m_truth_Cluster1_xF; */
  /* std::vector<float> m_truth_Cluster1_Chi2; */
  /* std::vector<float> m_truth_Cluster1_Decay1EnergyFraction; */
  /* std::vector<float> m_truth_Cluster1_Decay2EnergyFraction; */
  /* std::vector<float> m_truth_Cluster2_Id; */
  /* std::vector<float> m_truth_Cluster2_E; */
  /* std::vector<float> m_truth_Cluster2_Ecore; */
  /* std::vector<float> m_truth_Cluster2_Eta; */
  /* std::vector<float> m_truth_Cluster2_Phi; */
  /* std::vector<float> m_truth_Cluster2_Pt; */
  /* std::vector<float> m_truth_Cluster2_xF; */
  /* std::vector<float> m_truth_Cluster2_Chi2; */
  /* std::vector<float> m_truth_Cluster2_Decay1EnergyFraction; */
  /* std::vector<float> m_truth_Cluster2_Decay2EnergyFraction; */

  TFile *fout;
  std::string outname;
  int getEvent;
  /* bool hasHIJING; // needed to handle HIJING embedded samples correctly */
  bool isMonteCarlo; // if input is RD we obviously need to skip the truth info
  bool hasPYTHIA; // used to determine if primary photons are truly direct photons
  // counters for events
  long int n_events_total;
  long int n_events_minbias;
  long int n_events_with_vertex;
  long int n_events_with_good_vertex;
  long int n_events_positiveCaloE;
  // counters for each primary particle type
  long int n_primaries;
  long int n_primary_photons;
  long int n_direct_photons;
  long int n_leptons;
  long int n_neutral_hadrons;
  long int n_neutral_hadrons_geant;
  long int n_neutral_hadrons_pythia;
  long int n_charged_hadrons;
  long int n_charged_hadrons_geant;
  long int n_charged_hadrons_pythia;
  long int n_pythia_decayed_hadrons;
  std::set<int> allTrackIDs; // keep track of all truth particles that produced a shower
  std::set<int> primaryBarcodes; // keep track of all PYTHIA-decayed primaries

  /* std::vector<std::pair<int,int>> primaryBarcodes; */
  /* std::vector<std::pair<int,int>> secondaryBarcodes; */
  HepMC::GenParticle* getGenParticle(int barcode);
  PHG4Particle* getG4Particle(int barcode);
  bool withinAcceptance(PHG4Particle* part);
  bool withinAcceptance(HepMC::GenParticle* part);
  PHG4VtxPoint* getG4EndVtx(int id);
  bool isDirectPhoton(PHG4Particle* part);
  /* void addPrimaryFromGeant(PHG4Particle* part); */
  /* void addPrimaryFromPythia(HepMC::GenParticle* part); */
  void PrintParent(PHG4Particle* par);
  void FillTruthParticle(std::string which, PHG4Particle* par);
  void FillAllClustersFromParent(PHG4Particle* par);
  void FillParent(PHG4Particle* par);
  HepMC::GenParticle* GetHepMCParent(PHG4Particle* par);
  void FillTruth(HepMC::GenParticle* par);
  std::vector<PHG4Particle*> GetAllDaughters(PHG4Particle* parent);
  void GetBestDaughters(PHG4Particle* parent, PHG4Particle* &decay1, PHG4Particle* &decay2);
  void FillDecay(std::string which, PHG4Particle* decay, PHG4Particle* parent);
  RawCluster* FindBestCluster(PHG4Particle* decay, PHG4Particle* parent);
};

#endif // ISOCLUSTER_H
