// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef VandyJetDSTSkimmer_H
#define VandyJetDSTSkimmer_H

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

//fastjet
#include <fastjet/PseudoJet.hh>

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
    
    const float jetR[4]={0.2,0.3,0.4,0.5};
    const std::string jetRStr[4]={"02","03","04","05"};
    const float jetR_pTMin[4] = {12.0, 14.0, 15.5, 17.0};

    const std::string sampleNames[8] = {"MB","Jet5","Jet12","Jet20","Jet30","Jet40","Jet50","Jet60"};
    const float truthJetR_pTMin[4][8] = {{0,5, 12, 20, 30, 40, 50, 60}, {0,6, 13, 21, 31, 41, 51, 61}, {0,7, 14, 21, 32, 42, 52, 62}, {0,10, 19, 27, 38, 49, 58, 68}};

    float m_minJetPt{0.0};
    float m_vtx_cut{60.0};

    bool m_doCalib{true};

    bool m_doSim{false};
    std::string m_sampleName{"Jet20"};
    int sampleNumber{-999};

    int m_runnumber{0};
    double m_ZDC_coincidence{0.0};

    int nRemSim{0};
    int nRemNoSim{0};
    int nRem_dT{0};

    float m_vtx_z{-999};
    float m_vtx_z_truth{-999};

    GlobalVertexMap *vtxMap{nullptr};
    TowerInfoContainer *towerInfoContainers[4]{nullptr};
    RawTowerGeomContainer_Cylinderv1 *geoms[4]{nullptr};
    JetContainer *jets[4]{nullptr};
    RawClusterContainer *clusters{nullptr};

    PHG4TruthInfoContainer *truthParticles;
    JetContainer *truthJets[4]{nullptr};


    EventInfo *m_eventInfo = new EventInfo();
    std::vector<Tower> m_towerInfo;
    std::map<std::pair<int,int>, int> m_towerInfo_map;
    std::map<std::pair<int,int>, int> m_towerInfoTruth_map;
    std::map<std::array<int,3>, int> m_towerInfo_map2;
    std::vector<JetInfo> m_jetInfo[4];
    std::vector<JetInfo> m_topoclusters;

    std::vector<Tower> m_truthParticles;
    std::vector<JetInfo> m_truthJetInfo[4];

    const std::vector<GlobalVertex::VTXTYPE> vtxTypes {GlobalVertex::MBD};

    TFile *outfile{nullptr};
    std::string outfileName{"VandyJetDST.root"};
    TTree *T{nullptr};

    std::pair<float, float> isGoodDijet();
    std::pair<float, float> isGoodTruthDijet();
    float correct_eta(float eta, float r);
    fastjet::PseudoJet get_PseudoJet(double eta, double phi, double E);

    PHParameters m_cutParams{"TimingCutParams"}; //variable name is arbitrary

    int num{-1};
};

#endif // VandyJetDSTSkimmer_H
