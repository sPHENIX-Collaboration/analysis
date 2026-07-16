// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef VandyJetDSTSkimmer_H
#define VandyJetDSTSkimmer_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

//jetbase
#include <jetbase/Jet.h>
#include <jetbase/JetContainer.h>

//vertex
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertexMapv1.h>

//calos
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfov2.h>
#include <calobase/TowerInfov1.h>
#include <calobase/TowerInfo.h>
#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerGeomContainer_Cylinderv1.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>

//trigger
#include <ffarawobjects/Gl1Packetv2.h>
#include <ffarawobjects/Gl1Packetv1.h>
#include <calotrigger/TriggerAnalyzer.h>
#include <calotrigger/TriggerRunInfov1.h>

//jet timing cut module
#include <phparameter/PHParameters.h>

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>

//HepMC
#include <phhepmc/PHHepMCGenEvent.h>  
#include <phhepmc/PHHepMCGenEventMap.h>
#include <HepMC/GenEvent.h>
//fastjet
#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <vandyclasses/EventInfo.h>
#include <vandyclasses/JetInfo.h>
#include <vandyclasses/Tower.h>

#include "TFile.h"
#include "TTree.h"

#include <vector>
#include <string>

class PHCompositeNode;
class Jet;
class JetContainer;
class RawCluster;
class RawClusterContainer;

class TriggerAnalyzer; 

class VandyJetDSTSkimmer : public SubsysReco
{
 public:

  VandyJetDSTSkimmer(const std::string &name = "VandyJetDSTSkimmer");

// please declare default dtor/ctors in the header file
  ~VandyJetDSTSkimmer() override = default;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  int End(PHCompositeNode *topNode) override;

  void SetRunnumber(int runNum) { m_runnumber = runNum; };

  void SetMinJetPt(float minJetPt) { m_minJetPt = minJetPt; };

  void SetVtxCut(float vtxCut) { m_vtx_cut = vtxCut; };

  void SetOutfileName(std::string name) { outfileName = name; };

  void SetDoSim(bool flag) { m_doSim = flag; };

  void SetSimSample(std::string sampleName) { m_sampleName = sampleName; };

  void SetDoCalib(bool flag) {m_doCalib = flag; };

  std::string GetSimSample() { return m_sampleName;}
 private:
    
    const float jetR[5]={0.2,0.3,0.4,0.5, 0.6};
    const std::string jetRStr[5]={"02","03","04","05", "06"};
    const float jetR_pTMin[5] = {12.0, 14.0, 15.5, 17.0, 22.0};
    const std::string sampleNames[8] = {"MB","Jet5","Jet12","Jet20","Jet30","Jet40","Jet50","Jet60"};
    const std::string HerwigsampleNames[8] = {"HerwigMB","HerwigJet5","HerwigJet12","HerwigJet20","HerwigJet30","HerwigJet40","HerwigJet50","HerwigJet60"};
    const float truthJetR_pTMin[5][9] = {{0, 5, 12, 20, 30, 40, 50, 60, 100}, {0, 6, 13, 21, 31, 41, 51, 61, 100}, {0, 7, 14, 21, 32, 42, 52, 62, 100}, {0, 10, 19, 27, 38, 49, 58, 68, 100}, {0, 12, 22, 29, 41, 53, 63, 72, 100}};
    const float cs[8] = {4.1970e+10, 1.3878e+08, 1.4903e+06, 6.2623e+04, 2.5298e+03, 1.3553e+02, 7.3113, 3.3261e-01};
    const float Herwigcs[8] = {3.1909e+10, 1.8437e+08, 1.1326e+06, 5.2613e+04, 2.0694e+03, 1.0510e+02, 5.2089, 3.3261e-01};


    float m_minJetPt{5.0};
    float m_vtx_cut{60.0};

    bool m_doCalib{true};

    bool m_doSim{false};
    std::string m_sampleName{"Jet20"};
    int sampleNumber{-999};

    int m_runnumber{0};
    float m_ZDC_coincidence{0.0};

    int nRem{0};

    float m_vtx_z{-999};
    float m_vtx_z_truth{-999};

    GlobalVertexMap *vtxMap{nullptr};
    TowerInfoContainer *towerInfoContainers[4]{nullptr};
    RawTowerGeomContainer_Cylinderv1 *geoms[4]{nullptr};
    JetContainer *jets[5]{nullptr};
    JetContainer *jetsUncalib[5]{nullptr};
    RawClusterContainer *clusters{nullptr};

    PHG4TruthInfoContainer *truthParticles;
    JetContainer *truthJets[5]{nullptr};


    EventInfo *m_eventInfo = new EventInfo();
    std::vector<Tower> m_towerInfo;
    std::map<std::pair<int,int>, int> m_towerInfo_map;
    std::map<std::pair<int,int>, int> m_towerInfoTruth_map;
    std::map<std::array<int,3>, int> m_towerInfo_map2;
    std::vector<JetInfo> m_jetInfo[5];
    std::vector<JetInfo> m_topoclusters;

    std::vector<Tower> m_truthParticles;
    std::vector<JetInfo> m_truthJetInfo[5];

    std::vector<Tower> m_truthtowers;
    std::vector<JetInfo> m_truthtowerJetInfo[5];
    
    const std::vector<GlobalVertex::VTXTYPE> vtxTypes {GlobalVertex::MBD};

    TFile *outfile{nullptr};
    std::string outfileName{"VandyJetDST.root"};
    TTree *T{nullptr};

    std::pair<float, float> isGoodDijet(int jetR_index);
    std::pair<float, float> isGoodTruthDijet(int jetR_index, PHCompositeNode* topNode);
    float correct_eta(float eta, float r);
    fastjet::PseudoJet get_PseudoJet(double eta, double phi, double E);
    float getHCalFracTruth(Jet* jet, PHCompositeNode* topNode);
    float getDeltatTruth(float lead_ratio, float subl_ratio);
    float OHCALrat2t(float ohcal_ratio){
	    float t = -1.059;
	    t += -(2.015+0.784*ohcal_ratio)*ohcal_ratio;
	    return t;
    }
    void getJetParentParton(Jet*, JetInfo*, PHCompositeNode*);
    std::vector<HepMC::GenParticle*> getFinalStateAncestors(HepMC::GenParticle*, HepMC::GenEvent*);
    HepMC::GenParticle* findCommonAncestor(std::vector< std::vector< HepMC::GenParticle*>>);
    void getTruthTowers();
    void maketruthtowerJets();
    PHParameters m_cutParams{"TimingCutParams"}; //variable name is arbitrary

    int num{-1};
};

#endif // VandyJetDSTSkimmer_H
