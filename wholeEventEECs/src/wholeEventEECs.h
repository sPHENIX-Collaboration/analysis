#ifndef WHOLEEVENTEECS_H
#define WHOLEEVENTEECS_H

//fun4all
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllReturnCodes.h>

//phool 
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

//Calo towers 
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfov2.h>
#include <calobase/TowerInfov1.h>
#include <calobase/TowerInfo.h>
#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerGeomContainer_Cylinderv1.h>

//jetbase objects 
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <jetbase/JetMapv1.h>
#include <jetbase/JetMap.h>
#include <jetbase/Jetv1.h>
#include <jetbase/Jetv2.h>
#include <jetbase/Jet.h> 

//vertex stuff
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertexMapv1.h>
#include <globalvertex/MbdVertexMapv1.h>

//trigger
#include <ffarawobjects/Gl1Packetv2.h>
#include <ffarawobjects/Gl1Packetv1.h>
#include <calotrigger/TriggerAnalyzer.h>
#include <calotrigger/TriggerRunInfov1.h>

//root
#include <TH1.h>
#include <TFile.h>

//c++
#include <array>
#include <cmath>
#include <string>
#include <vector>

class PHCompositeNode;
class Jet;
class JetContainer;

class TriggerAnalyzer; 

class wholeEventEECs : public SubsysReco
{
    public:

    wholeEventEECs(const std::string &name = "wholeEventEECs");
    ~wholeEventEECs() override = default;

    int InitRun(PHCompositeNode *topNode) override;

    int process_event(PHCompositeNode *topNode) override;

    int End(PHCompositeNode *topNode) override;

    void set_outputName(const std::string &name)
    {
        m_outputName = name;
    }

    private:

    const std::string obsNames[5] = {"dR","dEta","dPhi","theta","z"};
    const std::string obsSymbols[5] = {"#DeltaR","#Delta#eta","#Delta#phi","#theta","z=(1-cos#theta)/2"};
    const float obsMax[5] = {static_cast<float>(sqrt(2.2*2.2 + M_PI*M_PI)), 2.2, M_PI, M_PI, 1.0};
    const std::string caloNames[4] = {"EMCal","IHCal","OHCal","All"};

    std::map<std::array<float, 3>, float> towers[4];
    std::map<std::array<float, 3>, float> towersCorrected[4];

    /*
    std::map<std::array<float, 3>, float> EMCalTowers;
    std::map<std::array<float, 3>, float> IHCalTowers;
    std::map<std::array<float, 3>, float> OHCalTowers;
    std::map<std::array<float, 3>, float> AllTowers;
    
    std::map<std::array<float, 3>, float> EMCalCorrectedTowers;
    std::map<std::array<float, 3>, float> IHCalCorrectedTowers;
    std::map<std::array<float, 3>, float> OHCalCorrectedTowers;
    std::map<std::array<float, 3>, float> AllCorrectedTowers;
    */

    std::string m_outputName = "wholeEventEEC.root";
    TFile *m_outputFile{nullptr};

    TH1D *nEvents{nullptr};
    TH1D *EEC[5][4]{nullptr};
    TH1D *EEC_corr[5][4]{nullptr};
    
    /*
    TH1D *EEC_RL[4]{nullptr};
    TH1D *EEC_eta[4]{nullptr};
    TH1D *EEC_phi[4]{nullptr};
    TH1D *EEC_theta[4]{nullptr};
    TH1D *EEC_z[4]{nullptr};

    TH1D *EEC_RL_corr[4]{nullptr};
    TH1D *EEC_eta_corr[4]{nullptr};
    TH1D *EEC_phi_corr[4]{nullptr};
    TH1D *EEC_theta_corr[4]{nullptr};
    TH1D *EEC_z_corr[4]{nullptr};
    */

    JetContainer *jets{nullptr};
    
    TowerInfoContainer *towerInfoContainers[3]{nullptr};
    RawTowerGeomContainer_Cylinderv1 *geoms[3]{nullptr};
    RawTowerGeomContainer_Cylinderv1 *emcal_geom{nullptr};

    /*
    TowerInfoContainer *emcalTowerInfoContainer{nullptr};
    TowerInfoContainer *ihcalTowerInfoContainer{nullptr};
    TowerInfoContainer *ohcalTowerInfoContainer{nullptr};

    RawTowerGeomContainer_Cylinderv1 *emcal_geom{nullptr};
    RawTowerGeomContainer_Cylinderv1 *ihcal_geom{nullptr};
    RawTowerGeomContainer_Cylinderv1 *ohcal_geom{nullptr};
    */

    GlobalVertexMap *vtxMap{nullptr};
    GlobalVertex *vtx{nullptr};

    float m_vtx_z{-999};
    
    int m_eventIndex{-1};
    int m_nGoodEvents{0};

    bool isGoodTrigger(PHCompositeNode *topNode);

    bool isGoodDijet();

    std::array<float, 3> correct_for_vertex(std::array<float, 3> center);

    std::array<float, 5> calc_observables(std::map<std::array<float, 3>, float>::iterator &iterA, std::map<std::array<float, 3>, float>::iterator &iterB);

};
#endif // WHOLEEVENTEECS_H
