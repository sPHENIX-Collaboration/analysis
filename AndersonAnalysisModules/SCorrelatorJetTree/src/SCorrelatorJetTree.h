// 'SCorrelatorJetTree.cc'
// Derek Anderson
// 12.04.2022
//
// Class to construct a tree of
// jets from a specified set of
// events.
//
// Derived from code by Antonio
// Silva (thanks!!)

#ifndef SCORRELATORJETTREE_H
#define SCORRELATORJETTREE_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// f4a include
#include <fun4all/SubsysReco.h>
// phool includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calotrigger/CaloTriggerInfo.h>
// fastjet includes
#include <fastjet/ClusterSequence.hh>
#include <fastjet/FunctionOfPseudoJet.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/PseudoJet.hh>
// misc includes
#include <HepMC/GenEvent.h>
#include <HepMC/GenParticle.h>
#include <g4jets/Jetv1.h>
#include <g4jets/JetMapv1.h>
// standard c include
#include <string>
#include <vector>

#pragma GCC diagnostic pop

// forward declarations
class TH1;
class TFile;
class TTree;
class PHG4Particle;
class PHCompositeNode;
class PHHepMCGenEvent;
class PHHepMCGenEventMap;
class PHG4TruthInfoContainer;
class Fun4AllHistoManager;
class RawClusterContainer;
class RawCluster;
class GlobalVertex;
class SvtxTrackMap;
class JetRecoEval;
class SvtxTrackEval;
class SvtxTrack;
class ParticleFlowElement;

// global constants
static const unsigned long NPart(2);
static const unsigned long NComp(3);



class SCorrelatorJetTree : public SubsysReco {

  public:

    // enums
    enum ALGO {
      ANTIKT    = 0,
      KT        = 1,
      CAMBRIDGE = 2
    };
    enum RECOMB {
      E_SCHEME   = 0,
      PT_SCHEME  = 1,
      PT2_SCHEME = 2,
      ET_SCHEME  = 3,
      ET2_SCHEME = 4
    };

    // ctor/dtor
    SCorrelatorJetTree(const std::string &name = "SCorrelatorJetTree", const std::string &outfile = "correlator_jet_tree.root", const bool isMC = false, const bool debug = false);
    ~SCorrelatorJetTree() override;

    // F4A methods
    int Init(PHCompositeNode *)          override;
    int process_event(PHCompositeNode *) override;
    int End(PHCompositeNode *)           override;

    // particle flow setters
    void setParticleFlowMinEta(double etamin) {m_particleflow_mineta = etamin;}
    void setParticleFlowMaxEta(double etamax) {m_particleflow_maxeta = etamax;}
    void setParticleFlowEtaAcc(double etamin, double etamax);
    // particle flow getters
    double getParticleFlowMinEta() {return m_particleflow_mineta;}
    double getParticleFlowMaxEta() {return m_particleflow_maxeta;}

    // track setters
    void setTrackMinPt(double ptmin)   {m_track_minpt = ptmin;}
    void setTrackMaxPt(double ptmax)   {m_track_maxpt = ptmax;}
    void setTrackMinEta(double etamin) {m_track_mineta = etamin;}
    void setTrackMaxEta(double etamax) {m_track_maxeta = etamax;}
    void setTrackPtAcc(double ptmin, double ptmax);
    void setTrackEtaAcc(double etamin, double etamax);
    // track getters
    double getTrackMinPt()  {return m_track_minpt;}
    double getTrackMaxPt()  {return m_track_maxpt;}
    double getTrackMinEta() {return m_track_mineta;}
    double getTrackMaxEta() {return m_track_maxeta;}

    // emcal setters
    void setEMCalClusterMinPt(double ptmin)   {m_EMCal_cluster_minpt = ptmin;}
    void setEMCalClusterMaxPt(double ptmax)   {m_EMCal_cluster_maxpt = ptmax;}
    void setEMCalClusterMinEta(double etamin) {m_EMCal_cluster_mineta = etamin;}
    void setEMCalClusterMaxEta(double etamax) {m_EMCal_cluster_maxeta = etamax;}
    void setEMCalClusterPtAcc(double ptmin, double ptmax);
    void setEMCalClusterEtaAcc(double etamin, double etamax);
    // emcal getters
    double getEMCalClusterMinPt()  {return m_EMCal_cluster_minpt;}
    double getEMCalClusterMaxPt()  {return m_EMCal_cluster_maxpt;}
    double getEMCalClusterMinEta() {return m_EMCal_cluster_mineta;}
    double getEMCalClusterMaxEta() {return m_EMCal_cluster_maxeta;}

    // hcal setters
    void setHCalClusterMinPt(double ptmin)   {m_HCal_cluster_minpt = ptmin;}
    void setHCalClusterMaxPt(double ptmax)   {m_HCal_cluster_maxpt = ptmax;}
    void setHCalClusterMinEta(double etamin) {m_HCal_cluster_mineta = etamin;}
    void setHCalClusterMaxEta(double etamax) {m_HCal_cluster_maxeta = etamax;}
    void setHCalClusterPtAcc(double ptmin, double ptmax);
    void setHCalClusterEtaAcc(double etamin, double etamax);
    // hcal getters
    double getHCalClusterMinPt()  {return m_HCal_cluster_minpt;}
    double getHCalClusterMaxPt()  {return m_HCal_cluster_maxpt;}
    double getHCalClusterMinEta() {return m_HCal_cluster_mineta;}
    double getHCalClusterMaxEta() {return m_HCal_cluster_maxeta;}

    // particle setters
    void setParticleMinPt(double ptmin)   {m_MC_particle_minpt = ptmin;}
    void setParticleMaxPt(double ptmax)   {m_MC_particle_maxpt = ptmax;}
    void setParticleMinEta(double etamin) {m_MC_particle_mineta = etamin;}
    void setParticleMaxEta(double etamax) {m_MC_particle_maxeta = etamax;}
    void setParticlePtAcc(double ptmin, double ptmax);
    void setParticleEtaAcc(double etamin, double etamx);

    // constituent setters
    void setAddParticleFlow(bool b)  {m_add_particleflow = b;}
    void setAddTracks(bool b)        {m_add_tracks = b;}
    void setAddEMCalClusters(bool b) {m_add_EMCal_clusters = b;}
    void setAddHCalClusters(bool b)  {m_add_HCal_clusters = b;}
    // constituent getters
    bool getAddParticleFlow()  {return m_add_particleflow;}
    bool getAddTracks()        {return m_add_tracks;}
    bool getAddEMCalClusters() {return m_add_EMCal_clusters;}
    bool getAddHCalClusters()  {return m_add_HCal_clusters;}

    // jet setters
    void setR(double r) {m_jetr = r;}
    void setJetAlgo(ALGO jetalgo);
    void setRecombScheme(RECOMB recomb_scheme);
    void setJetParameters(double r, ALGO jetalgo, RECOMB recomb_scheme);
    // jet getters
    double                       getR()            {return m_jetr;}
    fastjet::JetAlgorithm        getJetAlgo()      {return m_jetalgo;}
    fastjet::RecombinationScheme getRecombScheme() {return m_recomb_scheme;}

    // i/o setters
    void setMakeQualityPlots(bool q)        {m_qualy_plots = q;}
    void setJetContainerName(std::string n) {m_jetcontainer_name = n;}
    void setSaveDST(bool s)                 {m_save_dst = s;}
    void setIsMC(bool b)                    {m_ismc = b;}
    void setSaveDSTMC(bool s)               {m_save_truth_dst = s;}
    // i/o getters
    bool        getMakeQualityPlots() {return m_qualy_plots;}
    std::string getJetContainerName() {return m_jetcontainer_name;}
    bool        getSaveDST()          {return m_save_dst;}
    bool        getIsMC()             {return m_ismc;}
    bool        getSaveDSTMC()        {return m_save_truth_dst;}

  private:

    // jet methods
    void findJets(PHCompositeNode *topNode);
    void findMcJets(PHCompositeNode *topNode);
    void addParticleFlow(PHCompositeNode *topNode, std::vector<fastjet::PseudoJet> &particles, std::map<int, std::pair<Jet::SRC, int>> &fjMap);
    void addTracks(PHCompositeNode *topNode, std::vector<fastjet::PseudoJet> &particles, std::map<int, std::pair<Jet::SRC, int>> &fjMap);
    void addClusters(PHCompositeNode *topNode, std::vector<fastjet::PseudoJet> &particles, std::map<int, std::pair<Jet::SRC, int>> &fjMap);
    void addParticles(PHCompositeNode *topNode, std::vector<fastjet::PseudoJet> &particles, std::map<int, std::pair<Jet::SRC, int>> &fjMap);
    // constituent methods
    bool isAcceptableParticleFlow(ParticleFlowElement *pfPart);
    bool isAcceptableTrack(SvtxTrack *track);
    bool isAcceptableEMCalCluster(CLHEP::Hep3Vector &E_vec_cluster);
    bool isAcceptableHCalCluster(CLHEP::Hep3Vector &E_vec_cluster);
    bool isAcceptableParticle(HepMC::GenParticle *part);
    // i/o methods
    void initializeVariables();
    void initializeTrees();
    int  createJetNode(PHCompositeNode* topNode);
    void resetTreeVariables();

    // F4A histogram manager
    Fun4AllHistoManager *m_hm;

    // particle flow variables
    double m_particleflow_mineta;
    double m_particleflow_maxeta;
    // track variables
    double m_track_minpt;
    double m_track_maxpt;
    double m_track_mineta;
    double m_track_maxeta;
    // emcal variables
    double m_EMCal_cluster_minpt;
    double m_EMCal_cluster_maxpt;
    double m_EMCal_cluster_mineta;
    double m_EMCal_cluster_maxeta;
    // hcal variables
    double m_HCal_cluster_minpt;
    double m_HCal_cluster_maxpt;
    double m_HCal_cluster_mineta;
    double m_HCal_cluster_maxeta;
    // particle variables
    double m_MC_particle_minpt;
    double m_MC_particle_maxpt;
    double m_MC_particle_mineta;
    double m_MC_particle_maxeta;

    // constituent parameters
    bool m_add_particleflow;
    bool m_add_tracks;
    bool m_add_EMCal_clusters;
    bool m_add_HCal_clusters;
    // jet parameters
    double                        m_jetr;
    fastjet::JetAlgorithm         m_jetalgo;
    fastjet::RecombinationScheme  m_recomb_scheme;
    JetMapv1                     *m_jetMap;
    JetMapv1                     *m_truth_jetMap;
    // i/o parameters
    std::string  m_outfilename;
    std::string  m_jetcontainer_name;
    bool         m_qualy_plots;
    bool         m_save_dst;
    bool         m_save_truth_dst;
    bool         m_ismc;
    bool         m_doDebug;

    // output file & trees
    TFile *m_outFile;
    TTree *m_recTree;
    TTree *m_truTree;

    // output reco event variables
    unsigned long m_recNumJets           = 0;
    long long     m_recPartonID[NPart]   = {-9999,  -9999};
    double        m_recPartonMomX[NPart] = {-9999., -9999.};
    double        m_recPartonMomY[NPart] = {-9999., -9999.};
    double        m_recPartonMomZ[NPart] = {-9999., -9999.};
    // output reco jet variables
    std::vector<unsigned long> m_recJetNCst;
    std::vector<unsigned int>  m_recJetId;
    std::vector<unsigned int>  m_recJetTruId;
    std::vector<double>        m_recJetE;
    std::vector<double>        m_recJetPt;
    std::vector<double>        m_recJetEta;
    std::vector<double>        m_recJetPhi;
    // output reco constituent variables
    std::vector<std::vector<double>> m_recCstZ;
    std::vector<std::vector<double>> m_recCstDr;
    std::vector<std::vector<double>> m_recCstE;
    std::vector<std::vector<double>> m_recCstJt;
    std::vector<std::vector<double>> m_recCstEta;
    std::vector<std::vector<double>> m_recCstPhi;

    // output truth event variables
    unsigned long m_truNumJets           = 0;
    long long     m_truPartonID[NPart]   = {-9999,  -9999};
    double        m_truPartonMomX[NPart] = {-9999., -9999.};
    double        m_truPartonMomY[NPart] = {-9999., -9999.};
    double        m_truPartonMomZ[NPart] = {-9999., -9999.};
    // output truth jet variables
    std::vector<unsigned long> m_truJetNCst;
    std::vector<unsigned int>  m_truJetId;
    std::vector<unsigned int>  m_truJetTruId;
    std::vector<double>        m_truJetE;
    std::vector<double>        m_truJetPt;
    std::vector<double>        m_truJetEta;
    std::vector<double>        m_truJetPhi;
    // output truth constituent variables
    std::vector<std::vector<double>> m_truCstZ;
    std::vector<std::vector<double>> m_truCstDr;
    std::vector<std::vector<double>> m_truCstE;
    std::vector<std::vector<double>> m_truCstJt;
    std::vector<std::vector<double>> m_truCstEta;
    std::vector<std::vector<double>> m_truCstPhi;

};

#endif

// end ------------------------------------------------------------------------
