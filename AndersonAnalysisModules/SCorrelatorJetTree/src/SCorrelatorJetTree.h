// ----------------------------------------------------------------------------
// 'SCorrelatorJetTree.h'
// Derek Anderson
// 12.04.2022
//
// Class to construct a tree of
// jets from a specified set of
// events.
//
// Derived from code by Antonio
// Silva (thanks!!)
// ----------------------------------------------------------------------------

#ifndef SCORRELATORJETTREE_H
#define SCORRELATORJETTREE_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// standard c include
#include <string>
#include <vector>
#include <cassert>
#include <sstream>
#include <cstdlib>
#include <utility>
// f4a include
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllHistoManager.h>
// phool includes
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHCompositeNode.h>
// g4 includes
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4jets/Jet.h>
#include <g4jets/Jetv1.h>
#include <g4jets/JetMap.h>
#include <g4jets/JetMapv1.h>
#include <g4jets/FastJetAlgo.h>
#include <g4eval/SvtxTrackEval.h>
#include <g4eval/SvtxEvalStack.h>
// vtx includes
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
// tracking includes
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertexMap.h>
// calo includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calotrigger/CaloTriggerInfo.h>
// particle flow includes
#include <particleflowreco/ParticleFlowElement.h>
#include <particleflowreco/ParticleFlowElementContainer.h>
// fastjet includes
#include <fastjet/PseudoJet.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/ClusterSequence.hh>
#include <fastjet/FunctionOfPseudoJet.hh>
// hepmc includes
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <HepMC/GenParticle.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
// root includes
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TNtuple.h"
#include "TDirectory.h"

// TEST [05.12.2023] ----------------------------------------------------------
#include "g4eval/SvtxEvaluator.h"

#include "g4eval/SvtxClusterEval.h"
#include "g4eval/SvtxHitEval.h"
#include "g4eval/SvtxTruthEval.h"
#include "g4eval/SvtxVertexEval.h"

#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterv3.h>
#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrClusterv5.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/ActsGeometry.h>
#include <trackbase/ClusterErrorPara.h>

#include <trackbase/TpcDefs.h>

#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrClusterHitAssoc.h>
#include <trackbase/TrkrClusterIterationMapv1.h>

#include <trackbase_historic/ActsTransformations.h>
#include <trackbase_historic/TrackSeed.h>

#include <g4main/PHG4VtxPoint.h>

#include <g4detectors/PHG4TpcCylinderGeom.h>
#include <g4detectors/PHG4TpcCylinderGeomContainer.h>

#include <phool/PHTimer.h>
#include <phool/recoConsts.h>

#include <TVector3.h>

#include <cmath>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <map>
#include <memory>                                       // for shared_ptr
#include <set>                                          // for _Rb_tree_cons...
// ----------------------------------------------------------------------------

#pragma GCC diagnostic pop

using namespace std;
using namespace fastjet;
using namespace findNode;

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
static const size_t NPart(2);
static const size_t NComp(3);
static const size_t NRange(2);
static const size_t NMoment(2);
static const size_t NInfoQA(4);
static const size_t NJetType(2);
static const size_t NCstType(5);
static const size_t NObjType(9);
static const size_t NDirectory(NObjType - 3);
static const double MassPion(0.140);



// SCorrelatorJetTree definition ----------------------------------------------

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
    enum OBJECT {
      TRACK  = 0,
      ECLUST = 1,
      HCLUST = 2,
      FLOW   = 3,
      PART   = 4,
      TJET   = 5,
      RJET   = 6,
      TCST   = 7,
      RCST   = 8
    };
    enum CST_TYPE {
      PART_CST  = 0,
      TRACK_CST = 1,
      FLOW_CST  = 2,
      ECAL_CST  = 3,
      HCAL_CST  = 4
    };
    enum INFO {
      PT  = 0,
      ETA = 1,
      PHI = 2,
      ENE = 3
    };

    // ctor/dtor
    SCorrelatorJetTree(const string &name = "SCorrelatorJetTree", const string &outFile = "correlator_jet_tree.root", const bool isMC = false, const bool debug = false);
    ~SCorrelatorJetTree() override;

    // F4A methods
    int Init(PHCompositeNode *)          override;
    int process_event(PHCompositeNode *) override;
    int End(PHCompositeNode *)           override;

    // setters (inline)
    void SetAddTracks(const bool addTracks) {m_addTracks      = addTracks;}
    void SetAddFlow(const bool addFlow)     {m_addFlow        = addFlow;}
    void SetAddECal(const bool addECal)     {m_addECal        = addECal;}
    void SetAddHCal(const bool addHCal)     {m_addHCal        = addHCal;}
    void SetDoQualityPlots(const bool doQA) {m_doQualityPlots = doQA;}
    void SetDoMatching(const bool doMatch)  {m_doMatching     = doMatch;}
    void SetSaveDST(const bool doSave)      {m_saveDST        = doSave;}
    void SetIsMC(const bool isMC)           {m_isMC           = isMC;}
    void SetJetR(const double jetR)         {m_jetR           = jetR;}
    void SetJetType(const uint32_t type)    {m_jetType        = type;}
    void SetJetTreeName(const string name)  {m_jetTreeName    = name;}

    // setters (*.io.h)
    void SetParPtRange(const pair<double, double> ptRange);
    void SetParEtaRange(const pair<double, double> etaRange);
    void SetTrackPtRange(const pair<double, double> ptRange);
    void SetTrackEtaRange(const pair<double, double> etaRange);
    void SetFlowPtRange(const pair<double, double> ptRange);
    void SetFlowEtaRange(const pair<double, double> etaRange);
    void SetECalPtRange(const pair<double, double> ptRange);
    void SetECalEtaRange(const pair<double, double> etaRange);
    void SetHCalPtRange(const pair<double, double> ptRange);
    void SetHCalEtaRange(const pair<double, double> etaRange);
    void SetJetAlgo(const ALGO jetAlgo);
    void SetRecombScheme(const RECOMB recombScheme);
    void SetJetParameters(const double rJet, const uint32_t jetType, const ALGO jetAlgo, const RECOMB recombScheme);

    // system getters
    bool   GetAddFlow()        {return m_addFlow;}
    bool   GetAddTracks()      {return m_addTracks;}
    bool   GetAddECal()        {return m_addECal;}
    bool   GetAddHCal()        {return m_addHCal;}
    bool   GetDoQualityPlots() {return m_doQualityPlots;}
    bool   GetDoMatching()     {return m_doMatching;}
    bool   GetSaveDST()        {return m_saveDST;}
    bool   GetIsMC()           {return m_isMC;}
    string GetJetTreeName()    {return m_jetTreeName;}

    // acceptance getters
    double GetParMinPt()    {return m_parPtRange[0];}
    double GetParMaxPt()    {return m_parPtRange[1];}
    double GetParMinEta()   {return m_parEtaRange[0];}
    double GetParMaxEta()   {return m_parEtaRange[1];}
    double GetTrackMinPt()  {return m_trkPtRange[0];}
    double GetTrackMaxPt()  {return m_trkPtRange[1];}
    double GetTrackMinEta() {return m_trkEtaRange[0];}
    double GetTrackMaxEta() {return m_trkEtaRange[1];}
    double GetFlowMinPt()   {return m_flowPtRange[0];}
    double GetFlowMaxPt()   {return m_flowPtRange[1];}
    double GetFlowMinEta()  {return m_flowEtaRange[0];}
    double GetFlowMaxEta()  {return m_flowEtaRange[1];}
    double GetECalMinPt()   {return m_ecalPtRange[0];}
    double GetECalMaxPt()   {return m_ecalPtRange[1];}
    double GetECalMinEta()  {return m_ecalEtaRange[0];}
    double GetECalMaxEta()  {return m_ecalEtaRange[1];}
    double GetHCalMinPt()   {return m_hcalPtRange[0];}
    double GetHCalMaxPt()   {return m_hcalPtRange[1];}
    double GetHCalMinEta()  {return m_hcalEtaRange[0];}
    double GetHCalMaxEta()  {return m_hcalEtaRange[1];}

    // jet getters
    double              GetJetR()         {return m_jetR;}
    uint32_t            GetJetType()      {return m_jetType;}
    JetAlgorithm        GetJetAlgo()      {return m_jetAlgo;}
    RecombinationScheme GetRecombScheme() {return m_recombScheme;}

  private:

    // event methods (*.evt.h)
    void              GetEventVariables(PHCompositeNode *topNode);
    void              GetPartonInfo(PHCompositeNode *topNode);
    long              GetNumTrks(PHCompositeNode *topNode);
    long              GetNumChrgPars(PHCompositeNode *topNode);
    double            GetSumECalEne(PHCompositeNode *topNode);
    double            GetSumHCalEne(PHCompositeNode *topNode);
    double            GetSumParEne(PHCompositeNode *topNode);
    CLHEP::Hep3Vector GetRecoVtx(PHCompositeNode *topNode);

    // jet methods (*.jet.h)
    void FindTrueJets(PHCompositeNode *topNode);
    void FindRecoJets(PHCompositeNode *topNode);
    void AddParticles(PHCompositeNode *topNode, vector<PseudoJet> &particles, map<int, pair<Jet::SRC, int>> &fjMap);
    void AddTracks(PHCompositeNode *topNode, vector<PseudoJet> &particles, map<int, pair<Jet::SRC, int>> &fjMap);
    void AddFlow(PHCompositeNode *topNode, vector<PseudoJet> &particles, map<int, pair<Jet::SRC, int>> &fjMap);
    void AddECal(PHCompositeNode *topNode, vector<PseudoJet> &particles, map<int, pair<Jet::SRC, int>> &fjMap);
    void AddHCal(PHCompositeNode *topNode, vector<PseudoJet> &particles, map<int, pair<Jet::SRC, int>> &fjMap);

    // constituent methods (*.cst.h)
    int   GetMatchID(SvtxTrack *track);
    bool  IsGoodParticle(HepMC::GenParticle *par, const bool ignoreCharge=false);
    bool  IsGoodTrack(SvtxTrack *track);
    bool  IsGoodFlow(ParticleFlowElement *flow);
    bool  IsGoodECal(CLHEP::Hep3Vector &hepVecECal);
    bool  IsGoodHCal(CLHEP::Hep3Vector &hepVecHCal);
    bool  IsOutgoingParton(HepMC::GenParticle *par);
    float GetParticleCharge(const int pid);

    // system methods (*.sys.h)
    void                          InitVariables();
    void                          InitHists();
    void                          InitTrees();
    void                          InitEvals(PHCompositeNode *topNode);
    void                          FillTrueTree();
    void                          FillRecoTree();
    void                          SaveOutput();
    void                          ResetVariables();
    int                           CreateJetNode(PHCompositeNode *topNode);
    SvtxTrackMap*                 GetTrackMap(PHCompositeNode *topNode);
    GlobalVertex*                 GetGlobalVertex(PHCompositeNode *topNode);
    HepMC::GenEvent*              GetMcEvent(PHCompositeNode *topNode);
    RawClusterContainer*          GetClusterStore(PHCompositeNode *topNode, const TString sNodeName);
    ParticleFlowElementContainer* GetFlowStore(PHCompositeNode *topNode);

    // F4A/utility members
    Fun4AllHistoManager *m_histMan;
    SvtxEvalStack       *m_evalStack;
    SvtxTrackEval       *m_trackEval;

    // io members
    TFile    *m_outFile;
    TTree    *m_trueTree;
    TTree    *m_recoTree;
    TTree    *m_matchTree;
    string    m_outFileName;
    string    m_jetTreeName;
    JetMapv1 *m_recoJetMap;
    JetMapv1 *m_trueJetMap;

    // QA members
    TH1D *m_hJetArea[NJetType];
    TH1D *m_hJetNumCst[NJetType];
    TH1D *m_hNumObject[NObjType];
    TH1D *m_hSumCstEne[NCstType];
    TH1D *m_hObjectQA[NObjType][NInfoQA];
    TH1D *m_hNumCstAccept[NCstType][NMoment];

    // system members
    bool m_doQualityPlots;
    bool m_saveDST;
    bool m_isMC;
    bool m_doDebug;
    bool m_doMatching;
    bool m_addTracks;
    bool m_addFlow;
    bool m_addECal;
    bool m_addHCal;

    // acceptance parameters
    double m_parPtRange[NRange];
    double m_parEtaRange[NRange];
    double m_trkPtRange[NRange];
    double m_trkEtaRange[NRange];
    double m_flowPtRange[NRange];
    double m_flowEtaRange[NRange];
    double m_ecalPtRange[NRange];
    double m_ecalEtaRange[NRange];
    double m_hcalPtRange[NRange];
    double m_hcalEtaRange[NRange];

    // jet parameters
    double               m_jetR;
    uint32_t             m_jetType;
    JetAlgorithm         m_jetAlgo;
    JetDefinition       *m_trueJetDef;
    JetDefinition       *m_recoJetDef;
    ClusterSequence     *m_trueClust;
    ClusterSequence     *m_recoClust;
    RecombinationScheme  m_recombScheme;

    // event, jet members
    long long         m_partonID[NPart];
    CLHEP::Hep3Vector m_partonMom[NPart];
    CLHEP::Hep3Vector m_recoVtx;
    CLHEP::Hep3Vector m_trueVtx;
    vector<PseudoJet> m_recoJets;
    vector<PseudoJet> m_trueJets;

    // output reco event variables
    unsigned long          m_recoNumJets;
    double                 m_recoVtxX;
    double                 m_recoVtxY;
    double                 m_recoVtxZ;
    double                 m_recoSumECal;
    double                 m_recoSumHCal;
    long                   m_recoNumTrks;
    // output reco jet variables
    vector<unsigned long>  m_recoJetNCst;
    vector<unsigned int>   m_recoJetID;
    vector<double>         m_recoJetE;
    vector<double>         m_recoJetPt;
    vector<double>         m_recoJetEta;
    vector<double>         m_recoJetPhi;
    vector<double>         m_recoJetArea;
    // output reco constituent variables
    vector<vector<int>>    m_recoCstMatchID;
    vector<vector<double>> m_recoCstZ;
    vector<vector<double>> m_recoCstDr;
    vector<vector<double>> m_recoCstE;
    vector<vector<double>> m_recoCstJt;
    vector<vector<double>> m_recoCstEta;
    vector<vector<double>> m_recoCstPhi;

    // output truth event variables
    unsigned long         m_trueNumJets;
    long long             m_truePartonID[NPart];
    double                m_truePartonMomX[NPart];
    double                m_truePartonMomY[NPart];
    double                m_truePartonMomZ[NPart];
    double                m_trueVtxX;
    double                m_trueVtxY;
    double                m_trueVtxZ;
    double                m_trueSumPar;
    long                  m_trueNumChrgPars;
    // output truth jet variables
    vector<unsigned long> m_trueJetNCst;
    vector<unsigned int>  m_trueJetID;
    vector<double>        m_trueJetE;
    vector<double>        m_trueJetPt;
    vector<double>        m_trueJetEta;
    vector<double>        m_trueJetPhi;
    vector<double>        m_trueJetArea;
    // output truth constituent variables
    vector<vector<int>>    m_trueCstID;
    vector<vector<double>> m_trueCstZ;
    vector<vector<double>> m_trueCstDr;
    vector<vector<double>> m_trueCstE;
    vector<vector<double>> m_trueCstJt;
    vector<vector<double>> m_trueCstEta;
    vector<vector<double>> m_trueCstPhi;

};

#endif

// end ------------------------------------------------------------------------
