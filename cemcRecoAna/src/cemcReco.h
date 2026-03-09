// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CEMCRECO_H
#define CEMCRECO_H

#include <fun4all/SubsysReco.h>
#include <HepMC/SimpleVector.h> 
#include <vector>
//TopNode
class PHCompositeNode;
//histos and stuff
class TH1F;
class TH3F;
class TH2F;
class TH2I;
class TString;
//emcal stuff
class RawClusterContainer;
class RawCluster;
class RawTowerGeomContainer;
class RawTowerContainer; 
class RawTower;
//tracking for iso cone
class SvtxTrackMap;
//vertex info
class GlobalVertexMap;
class GlobalVertex;
//Fun4all
class Fun4AllHistoManager;
class TFile;
class PHG4TruthInfoContainer;
class PHG4Particle;
class PHG4VtxPoint;
class CaloEvalStack;
//class std::vector;
//class FourVector;

const int nEtaBins = 5;


class cemcReco : public SubsysReco
{
 public:

  cemcReco(const std::string &name, const std::string &outName);

  ~cemcReco() override;

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

 private:
  //counters, etc.
  int nEvent;
  std::string Outfile;
  
  //functors
  float calculateTSP(RawCluster *cluster, RawClusterContainer *cluster_container, RawTowerContainer *towerContainer, RawTowerGeomContainer *towerGeo, GlobalVertex *vtx);
  float coneSum(RawCluster *cluster, RawClusterContainer *cluster_container, SvtxTrackMap *trackmap, float coneradius, GlobalVertex *vtx);
  void letsSumw2();
  float getpT(PHG4Particle *particle);
  float FindTruthPhoton(float cluster_eta, float cluster_phi, float cluster_energy, PHG4TruthInfoContainer* particle);
  float getEta(PHG4Particle *particle);
  float getPhi(PHG4Particle *particle);
  bool compareVertices(HepMC::FourVector hepMCvtx, PHG4VtxPoint *g4vtx );
  bool checkBarcode(int motherBarcode, std::vector<int> &motherBarcodes);
  bool checkBarcode(int motherBarcode, std::vector<PHG4Particle*> &motherBarcodes);

  int getEtaBin(float eta);

  //this thing
  CaloEvalStack *caloevalstack;
  
  //RawTower *getMaxTower(RawCluster *cluster);
  //histos
  TH3F *ePi0InvMassEcut[2][nEtaBins];
  TH1F *photonE;
  TH1F *clusterDisp;
  TH2F *clusterChi2;
  TH2F *clusterProbPhoton;
  TH1F *isoPhoE;
  TH2F *isoPhoChi2;
  TH2F *isoPhoProb;
  TH3F *deltaR_E_invMass;
  TH3F *asym_E_invMass;
  TH2F *tsp_E;
  TH2F *tsp_E_iso;
  TH1F *truth_pi0_E[nEtaBins];
  TH1F *truth_eta_E;
  TH1F *truth_dpho_E;
  TH1F *pi0E_truth_reco;
  TH2F *deltaR_E_truth_pi0;
  TH3F *invMass_eta;
  TH3F *dPhoChi2;
  TH3F *dPhoProb;
  TH3F *pi0Chi2;
  TH3F *pi0Prob;
  TH3F *etaChi2;
  TH3F *etaProb;
  TH3F *electronChi2;
  TH3F *electronProb; 
  TH3F *hadronChi2;
  TH3F *hadronProb;
  TH2F *etaFrac;
  TH2F *pi0Frac;
  TH3F *unmatchedLocale;
  TH1F *unmatchedE;
  TH3F *invMassPhoPi0;
  TH3F *invMassEPhi;
  //TH2F *deltaR_truth_eta;
  
  TH2F *eFrac_dr_primary;
  TH2F *eFrac_dr_secondary;
  
  TH2F *asym_E_truth_pi0;
  //TH2F *asym_E_truth_eta;
  
  //Histogram manager
  Fun4AllHistoManager *hm = nullptr;
  
  //output file
  TFile *out;
  //values
  const float pi = 3.1415926;
  float trackErrorCount;
};

#endif // CEMCRECO_H
