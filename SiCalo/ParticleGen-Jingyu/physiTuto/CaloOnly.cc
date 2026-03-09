#include "CaloOnly.h"

#include <calobase/RawTowerContainer.h>
#include <calobase/RawTower.h> 
#include <calobase/RawClusterContainer.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/SvtxVertexMap.h>
#include <globalvertex/SvtxVertex.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrackState_v1.h>
#include <trackbase_historic/TrackSeedContainer.h>
#include <trackbase_historic/TrackSeed.h>
#include <trackreco/ActsPropagator.h>

#include <Acts/Geometry/GeometryIdentifier.hpp>
#include <Acts/MagneticField/ConstantBField.hpp>
#include <Acts/MagneticField/MagneticFieldProvider.hpp>
#include <Acts/Surfaces/CylinderSurface.hpp>
#include <Acts/Surfaces/PerigeeSurface.hpp>
#include <Acts/Geometry/TrackingGeometry.hpp>

#include <CLHEP/Vector/ThreeVector.h>
#include <math.h>
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>

//____________________________________________________________________________..
CaloOnly::CaloOnly(const std::string &name, const std::string &file):
    SubsysReco(name),
    _outfilename(file),
    _outfile(nullptr),
    _tree(nullptr)
{
    std::cout << "CaloOnly::CaloOnly(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
CaloOnly::~CaloOnly()
{
    std::cout << "CaloOnly::~CaloOnly() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int CaloOnly::Init(PHCompositeNode *topNode)
{
    std::cout << topNode << std::endl;
    std::cout << "CaloOnly::Init(PHCompositeNode *topNode) Initializing" << std::endl;
    _outfile = new TFile(_outfilename.c_str(), "RECREATE");
    delete _tree;

    _tree = new TTree("tree", "A tree with track/calo info");
    _tree->Branch("_run_test", &_run_test);
    // EMCal variables
    _tree->Branch("_emcalgeo_id", &_emcalgeo_id);
    _tree->Branch("_emcalgeo_phibin", &_emcalgeo_phibin);
    _tree->Branch("_emcalgeo_etabin", &_emcalgeo_etabin);

    _tree->Branch("_emcal_e", &_emcal_e);
    _tree->Branch("_emcal_phi", &_emcal_phi);
    _tree->Branch("_emcal_eta", &_emcal_eta);
    _tree->Branch("_emcal_iphi", &_emcal_iphi);
    _tree->Branch("_emcal_ieta", &_emcal_ieta);
    _tree->Branch("_emcal_time", &_emcal_time);
    _tree->Branch("_emcal_chi2", &_emcal_chi2);
    _tree->Branch("_emcal_pedestal", &_emcal_pedestal);
    // IHCAL variables
    _tree->Branch("_ihcal_e", &_ihcal_e);
    _tree->Branch("_ihcal_phi", &_ihcal_phi);
    _tree->Branch("_ihcal_eta", &_ihcal_eta);
    _tree->Branch("_ihcal_iphi", &_ihcal_iphi);
    _tree->Branch("_ihcal_ieta", &_ihcal_ieta);
    _tree->Branch("_ihcal_time", &_ihcal_time);
    _tree->Branch("_ihcal_chi2", &_ihcal_chi2);
    _tree->Branch("_ihcal_pedestal", &_ihcal_pedestal);
    // OHCAL variables
    _tree->Branch("_ohcal_e", &_ohcal_e);
    _tree->Branch("_ohcal_phi", &_ohcal_phi);
    _tree->Branch("_ohcal_eta", &_ohcal_eta);
    _tree->Branch("_ohcal_iphi", &_ohcal_iphi);
    _tree->Branch("_ohcal_ieta", &_ohcal_ieta);
    _tree->Branch("_ohcal_time", &_ohcal_time);
    _tree->Branch("_ohcal_chi2", &_ohcal_chi2);
    _tree->Branch("_ohcal_pedestal", &_ohcal_pedestal);

    // EMCal cluster information
    _tree->Branch("_emcal_cluster_id", &_emcal_cluster_id);
    _tree->Branch("_emcal_cluster_e", &_emcal_cluster_e);
    _tree->Branch("_emcal_cluster_phi", &_emcal_cluster_phi);
    _tree->Branch("_emcal_cluster_eta", &_emcal_cluster_eta);
    _tree->Branch("_emcal_cluster_x", &_emcal_cluster_x);
    _tree->Branch("_emcal_cluster_y", &_emcal_cluster_y);
    _tree->Branch("_emcal_cluster_z", &_emcal_cluster_z);
    _tree->Branch("_emcal_cluster_R", &_emcal_cluster_R);
    _tree->Branch("_emcal_cluster_ecore", &_emcal_cluster_ecore);
    _tree->Branch("_emcal_cluster_chi2", &_emcal_cluster_chi2);
    _tree->Branch("_emcal_cluster_prob", &_emcal_cluster_prob);
    // EMCal full cluster corrections
    _tree->Branch("_emcal_clusfull_e", &_emcal_clusfull_e);
    _tree->Branch("_emcal_clusfull_eta", &_emcal_clusfull_eta);
    _tree->Branch("_emcal_clusfull_phi", &_emcal_clusfull_phi);
    _tree->Branch("_emcal_clusfull_x", &_emcal_clusfull_x);
    _tree->Branch("_emcal_clusfull_y", &_emcal_clusfull_y);
    _tree->Branch("_emcal_clusfull_z", &_emcal_clusfull_z);
    _tree->Branch("_emcal_clusfull_R", &_emcal_clusfull_R);
    _tree->Branch("_emcal_clusfull_pt", &_emcal_clusfull_pt);
    // EMCal core cluster corrections
    _tree->Branch("_emcal_cluscore_e", &_emcal_cluscore_e);
    _tree->Branch("_emcal_cluscore_eta", &_emcal_cluscore_eta);
    _tree->Branch("_emcal_cluscore_phi", &_emcal_cluscore_phi);
    _tree->Branch("_emcal_cluscore_x", &_emcal_cluscore_x);
    _tree->Branch("_emcal_cluscore_y", &_emcal_cluscore_y);
    _tree->Branch("_emcal_cluscore_z", &_emcal_cluscore_z);
    _tree->Branch("_emcal_cluscore_R", &_emcal_cluscore_R);
    _tree->Branch("_emcal_cluscore_pt", &_emcal_cluscore_pt);

    _tree->Branch("_mbd_z", &_mbd_z);

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloOnly::process_event(PHCompositeNode *topNode)
{
    bool hasMBDvertex = true;

    GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
    if (!vertexmap)
    {
        std::cout << "CaloOnly::process_event - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main  macro in order to reconstruct the global vertex." << std::endl;
        //assert(vertexmap);  // force quit

        hasMBDvertex = false;
    }

    if (vertexmap->empty())
    {
        std::cout << "CaloOnly::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro  in order to reconstruct the global vertex." << std::endl;
        
        hasMBDvertex = false;
    }

    GlobalVertex *vtx = vertexmap->begin()->second;
    if (vtx == nullptr)
    {
        hasMBDvertex = false;
    }

    if(hasMBDvertex == true)
    {
        _mbd_z.push_back(vtx->get_z());
    }

    // ---------------------------- CALO info ---------------------------------------------------------------------
    // ---------------------------- CALO info ---------------------------------------------------------------------
    // ---------------------------- CALO info ---------------------------------------------------------------------
    
    // 获取 RawTowerGeomContainer
    towerGeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
    if (!towerGeom)
    {
        std::cout << "CaloOnly::process_event: Cannot find CEMC RawTowerGeomContainer!" << std::endl;
    }

    // calo cluster
    clustersEM = findNode::getClass<RawClusterContainer>(topNode, "CLUSTERINFO_CEMC");
    if (!clustersEM)
    {
        std::cout << "CaloOnly::process_event: cannot find cluster container " << "CLUSTERINFO_CEMC" << std::endl;
    }

    // clustersIH = findNode::getClass<RawClusterContainer>(topNode, "CLUSTERINFO_HCALIN");
    // if (!clustersIH)
    // {
    //     std::cout << "CaloOnly::process_event: cannot find cluster container " << "CLUSTERINFO_HCALIN" << std::endl;
    // }

    // clustersOH = findNode::getClass<RawClusterContainer>(topNode, "CLUSTERINFO_HCALOUT");
    // if (!clustersOH)
    // {
    //     std::cout << "CaloOnly::process_event: cannot find cluster container " << "CLUSTERINFO_HCALOUT" << std::endl;
    // }


    // calo tower
    EMCAL_Container = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
    if(!EMCAL_Container)
    {
        std::cout << "CaloOnly::process_event: TOWERINFO_CALIB_CEMC not found!!!" << std::endl;
    }

    IHCAL_Container = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
    if(!IHCAL_Container)
    {
        std::cout << "CaloOnly::process_event: TOWERINFO_CALIB_HCALIN not found! Aborting!" << std::endl;
        return Fun4AllReturnCodes::ABORTEVENT;
    }

    OHCAL_Container = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
    if(!OHCAL_Container)
    {
        std::cout << "OHCAL_Container not found! Aborting!" << std::endl;
        return Fun4AllReturnCodes::ABORTEVENT;
    }


    // calo geometry
    EMCalGeo = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
    if(!EMCalGeo)
    {
        std::cout << "CaloOnly::process_event: TOWERGEOM_CEMC not found!!!" << std::endl;
    }

    IHCalGeo = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
    if(!IHCalGeo)
    {
        std::cout << "CaloOnly::process_event: TOWERGEOM_HCALIN not found!!!" << std::endl;
    }

    OHCalGeo = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
    if(!OHCalGeo)
    {
        std::cout << "CaloOnly::process_event: TOWERGEOM_HCALOUT not found!!!" << std::endl;
    }

    // // Rawtowercontainer
    // EMCal_RawTowerContainer = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
    // if(!EMCal_RawTowerContainer)
    // {
    //     std::cout << "CaloOnly::process_event: TOWER_CALIB_CEMC not found!!!" << std::endl;
    // }

    ResetTreeVectors();
    
    _run_test.push_back(1);
    FillTree();

    // _tree->Fill();

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloOnly::End(PHCompositeNode *topNode)
{
    std::cout << topNode << std::endl;
    _outfile->cd();
    _outfile->Write();
    _outfile->Close();
    return Fun4AllReturnCodes::EVENT_OK;
}

void CaloOnly::ResetTreeVectors()
{
    _run_test.clear();
    // EMCal vectors
    _emcalgeo_id.clear();
    _emcalgeo_phibin.clear();
    _emcalgeo_etabin.clear();

    _emcal_e.clear();
    _emcal_phi.clear();
    _emcal_eta.clear();
    _emcal_iphi.clear();
    _emcal_ieta.clear();
    _emcal_time.clear();
    _emcal_chi2.clear();
    _emcal_pedestal.clear();

    // IHCAL vectors
    _ihcal_e.clear();
    _ihcal_phi.clear();
    _ihcal_eta.clear();
    _ihcal_iphi.clear();
    _ihcal_ieta.clear();
    _ihcal_time.clear();
    _ihcal_chi2.clear();
    _ihcal_pedestal.clear();

    // OHCAL vectors
    _ohcal_e.clear();
    _ohcal_phi.clear();
    _ohcal_eta.clear();
    _ohcal_iphi.clear();
    _ohcal_ieta.clear();
    _ohcal_time.clear();
    _ohcal_chi2.clear();
    _ohcal_pedestal.clear();

    // EMCal cluster information
    _emcal_cluster_id.clear();
    _emcal_cluster_e.clear();
    _emcal_cluster_phi.clear();
    _emcal_cluster_eta.clear();
    _emcal_cluster_x.clear();
    _emcal_cluster_y.clear();
    _emcal_cluster_z.clear();
    _emcal_cluster_ecore.clear();
    _emcal_cluster_chi2.clear();
    _emcal_cluster_prob.clear();

    // EMCal full cluster corrections
    _emcal_clusfull_e.clear();
    _emcal_clusfull_eta.clear();
    _emcal_clusfull_phi.clear();
    _emcal_clusfull_x.clear();
    _emcal_clusfull_y.clear();
    _emcal_clusfull_z.clear();
    _emcal_clusfull_pt.clear();

    // EMCal core cluster corrections
    _emcal_cluscore_e.clear();
    _emcal_cluscore_eta.clear();
    _emcal_cluscore_phi.clear();
    _emcal_cluscore_x.clear();
    _emcal_cluscore_y.clear();
    _emcal_cluscore_z.clear();
    _emcal_cluscore_pt.clear();
}


// //____________________________________________________________________________..
void CaloOnly::FillTree()
{
    if (!clustersEM || !EMCAL_Container || !EMCalGeo || !IHCalGeo || !OHCalGeo || !IHCAL_Container || !OHCAL_Container )
    {
        std::cout << PHWHERE << "missing node trees, can't continue with track calo matching"
              << std::endl;
        return;
    }

    // 遍历所有塔
    for (unsigned int i = 0; i < EMCalGeo->size(); i++)
    {
        RawTowerGeom* geom = EMCalGeo->get_tower_geometry(i);
        if (!geom) continue;
        std::cout<<"rawtower id is: "<<geom->get_id()<<std::endl;

        std::cout << "Tower ID: " << i  // 这里的 first 是 tower id
                  << std::endl;
    }

    // loop over all calo tower
    TowerInfo *tInfo = nullptr;

    for(unsigned int iem = 0; iem < EMCAL_Container->size(); iem++)
    {
        tInfo = EMCAL_Container->get_tower_at_channel(iem); 

        if(!tInfo)
        {
            continue;
        }

        if(!tInfo->get_isGood())
        {
            continue;
        }

        //if(tInfo->get_energy() < 0.2) continue;

        unsigned int towerinfo_key = EMCAL_Container->encode_key(iem);
        int ti_ieta = EMCAL_Container->getTowerEtaBin(towerinfo_key);
        int ti_iphi = EMCAL_Container->getTowerPhiBin(towerinfo_key);

        const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::CEMC, ti_ieta, ti_iphi);
        RawTowerGeom *tower_geom = EMCalGeo->get_tower_geometry(key);

        // _emcalgeo_id.push_back(tower_geom->get_id());
        // _emcalgeo_phibin.push_back(ti_iphi);
        // _emcalgeo_etabin.push_back(ti_ieta);
        // std::cout<<"rawtower id is: "<<tower_geom->get_id()<<std::endl;

        _emcal_e.push_back(tInfo->get_energy());
        _emcal_phi.push_back(tower_geom->get_phi());
        _emcal_eta.push_back(tower_geom->get_eta());
        _emcal_iphi.push_back(ti_iphi);
        _emcal_ieta.push_back(ti_ieta);
        _emcal_time.push_back(tInfo->get_time());
        _emcal_chi2.push_back(tInfo->get_chi2());
        _emcal_pedestal.push_back(tInfo->get_pedestal());
    }

    for(unsigned int ihcal = 0; ihcal < IHCAL_Container->size(); ihcal++)
    {
        tInfo = IHCAL_Container->get_tower_at_channel(ihcal);

        if(!tInfo)
        {
            continue;
        }

        if(!tInfo->get_isGood())
        {
            continue;
        }

        //if(tInfo->get_energy() < 0.2) continue;

        unsigned int towerinfo_key = IHCAL_Container->encode_key(ihcal);
        int ti_ieta = IHCAL_Container->getTowerEtaBin(towerinfo_key);
        int ti_iphi = IHCAL_Container->getTowerPhiBin(towerinfo_key);
        
        const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ti_ieta, ti_iphi);
        RawTowerGeom *tower_geom = IHCalGeo->get_tower_geometry(key);

        _ihcal_e.push_back(tInfo->get_energy());
        _ihcal_phi.push_back(tower_geom->get_phi());
        _ihcal_eta.push_back(tower_geom->get_eta());
        _ihcal_iphi.push_back(ti_iphi);
        _ihcal_ieta.push_back(ti_ieta);
        _ihcal_time.push_back(tInfo->get_time());
        _ihcal_chi2.push_back(tInfo->get_chi2());
        _ihcal_pedestal.push_back(tInfo->get_pedestal());
    }

    for(unsigned int ohcal = 0; ohcal < OHCAL_Container->size(); ohcal++)
    {
        tInfo = OHCAL_Container->get_tower_at_channel(ohcal);

        if(!tInfo)
        {
            continue;
        }

        if(!tInfo->get_isGood())
        {
            continue;
        }

        //if(tInfo->get_energy() < 0.2) continue;

        unsigned int towerinfo_key = OHCAL_Container->encode_key(ohcal);
        int ti_ieta = OHCAL_Container->getTowerEtaBin(towerinfo_key);
        int ti_iphi = OHCAL_Container->getTowerPhiBin(towerinfo_key);
        
        const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ti_ieta, ti_iphi);
        RawTowerGeom *tower_geom = OHCalGeo->get_tower_geometry(key);

        _ohcal_e.push_back(tInfo->get_energy());
        _ohcal_phi.push_back(tower_geom->get_phi());
        _ohcal_eta.push_back(tower_geom->get_eta());
        _ohcal_iphi.push_back(ti_iphi);
        _ohcal_ieta.push_back(ti_ieta);
        _ohcal_time.push_back(tInfo->get_time());
        _ohcal_chi2.push_back(tInfo->get_chi2());
        _ohcal_pedestal.push_back(tInfo->get_pedestal());
    }

    // Loop over the EMCal clusters
    CLHEP::Hep3Vector vertex(0., 0., 0.);

    RawCluster *cluster = nullptr;

    RawClusterContainer::Range begin_end_EMC = clustersEM->getClusters();
    RawClusterContainer::Iterator clusIter_EMC;    
    for (clusIter_EMC = begin_end_EMC.first; clusIter_EMC != begin_end_EMC.second; ++clusIter_EMC)
    {
        cluster = clusIter_EMC->second;
        if(cluster->get_energy() < m_emcal_e_low_cut) continue;

        // cluster info
        _emcal_cluster_id.push_back(clusIter_EMC->first);
        _emcal_cluster_e.push_back(cluster->get_energy());
        _emcal_cluster_phi.push_back(RawClusterUtility::GetAzimuthAngle(*cluster, vertex));
        _emcal_cluster_eta.push_back(RawClusterUtility::GetPseudorapidity(*cluster, vertex));
        _emcal_cluster_x.push_back(cluster->get_x());
        _emcal_cluster_y.push_back(cluster->get_y());
        _emcal_cluster_z.push_back(cluster->get_z());
        _emcal_cluster_ecore.push_back(cluster->get_ecore());
        _emcal_cluster_chi2.push_back(cluster->get_chi2());
        _emcal_cluster_prob.push_back(cluster->get_prob());

        // after correction full cluster total energy info 
        CLHEP::Hep3Vector E_vec_cluster_Full = RawClusterUtility::GetEVec(*cluster, vertex);
        float clusE = E_vec_cluster_Full.mag();
        float clus_eta = E_vec_cluster_Full.pseudoRapidity();
        float clus_phi = E_vec_cluster_Full.phi();
        float clus_x = E_vec_cluster_Full.x();
        float clus_y = E_vec_cluster_Full.y();
        float clus_z = E_vec_cluster_Full.z();
        float clus_pt = E_vec_cluster_Full.perp();

        _emcal_clusfull_e.push_back(clusE);
        _emcal_clusfull_eta.push_back(clus_eta);
        _emcal_clusfull_phi.push_back(clus_phi);
        _emcal_clusfull_x.push_back(clus_x);
        _emcal_clusfull_y.push_back(clus_y);
        _emcal_clusfull_z.push_back(clus_z);
        _emcal_clusfull_pt.push_back(clus_pt);

        // double phi_emcal_corr = clus_phi;

        // after correction cluster core energy info 
        CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster, vertex);
        float clusEcore = E_vec_cluster.mag();
        float clus_coreeta = E_vec_cluster.pseudoRapidity();
        float clus_corephi = E_vec_cluster.phi();
        float clus_corex = E_vec_cluster.x();
        float clus_corey = E_vec_cluster.y();
        float clus_corez = E_vec_cluster.z();
        float clus_corept = E_vec_cluster.perp();

        _emcal_cluscore_e.push_back(clusEcore);
        _emcal_cluscore_eta.push_back(clus_coreeta);
        _emcal_cluscore_phi.push_back(clus_corephi);
        _emcal_cluscore_x.push_back(clus_corex);
        _emcal_cluscore_y.push_back(clus_corey);
        _emcal_cluscore_z.push_back(clus_corez);
        _emcal_cluscore_pt.push_back(clus_corept);

        // // zi ji suan ge cluster
        // RawCluster::TowerConstRange towers = cluster->get_towers();
        // RawCluster::TowerConstIterator toweriter;

        // TowerInfo *towerInfo = nullptr;

        // for (toweriter = towers.first; toweriter != towers.second; ++toweriter)
        // {
        //     _emcal_tower_cluster_id.push_back(clusIter_EMC->first);
        //     RawTowerGeom *tower_geom = EMCalGeo->get_tower_geometry(toweriter->first);
        //     _emcal_tower_phi.push_back(tower_geom->get_phi());
        //     _emcal_tower_eta.push_back(tower_geom->get_eta());
        //     _emcal_tower_e.push_back(toweriter->second);
        //     unsigned int key = TowerInfoDefs::encode_emcal(tower_geom->get_bineta(), tower_geom->get_binphi());
        //     towerInfo = EMCAL_Container->get_tower_at_key(key);
        //     _emcal_tower_status.push_back(towerInfo->get_status());
        // }
    }

    // // loop over truth primary particles
    // PHG4TruthInfoContainer::ConstRange range = truthinfo->GetPrimaryParticleRange();
    // for (PHG4TruthInfoContainer::ConstIterator truth_itr = range.first; truth_itr != range.second; ++truth_itr)
    // {
    //     PHG4Particle *truth = truth_itr->second;
    //     _truth_px.push_back(truth->get_px());
    //     _truth_py.push_back(truth->get_py());
    //     _truth_pz.push_back(truth->get_pz());
    //     _truth_e.push_back(truth->get_e());
    //     _truth_pt.push_back(sqrt(truth->get_px() * truth->get_px() + truth->get_py() * truth->get_py()));
    //     _truth_eta.push_back(atanh(truth->get_pz() / sqrt(truth->get_px() * truth->get_px() + truth->get_py() * truth->get_py() + truth->get_pz() * truth->get_pz())));
    //     _truth_phi.push_back(atan2(truth->get_py(),truth->get_px()));
    //     _truth_pid.push_back(truth->get_pid());
    //     //std::cout<<"pid = "<<truth->get_pid()<<std::endl;
    // }

    // // map between track id and charge deposit
    // std::map<int, float> trkmap;

    // hits_CEMC = findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_CEMC");
    // if(!hits_CEMC)
    // {
    //   std::cout << "CaloOnly::process_event: G4HIT_CEMC not found!!!" << std::endl;
    // }

    // TLorentzVector v4;
    // PHG4HitContainer::ConstRange hit_range = hits_CEMC->getHits();
    // for (PHG4HitContainer::ConstIterator hit_iter = hit_range.first; hit_iter != hit_range.second; hit_iter++)
    // {
    //     PHG4Hit *this_hit = hit_iter->second;
    //     float light_yield = hit_iter->second->get_light_yield();
    //     float edep = hit_iter->second->get_edep();
    //     int ch = hit_iter->second->get_layer();
    //     float x = hit_iter->second->get_x(0);
    //     float y = hit_iter->second->get_y(0);
    //     float z = hit_iter->second->get_z(0);
    //     float t = hit_iter->second->get_t(0);
    //     int trkid = hit_iter->second->get_trkid();
    //     int truthtrkid = trkid;
    //     PHG4Particle *part = truthinfo->GetParticle(trkid);

    //     v4.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e());
    //     int pid = part->get_pid();
    //     _CEMC_Hit_pid.push_back(pid);

    //     int vtxid = part->get_vtx_id();
    //     PHG4VtxPoint *vtx = truthinfo->GetVtx(vtxid);

    //     // add trkid to a set
    //     _CEMC_Hit_Evis.push_back(light_yield);
    //     _CEMC_Hit_Edep.push_back(edep);
    //     _CEMC_Hit_ch.push_back(ch);
    //     _CEMC_Hit_x.push_back(x);
    //     _CEMC_Hit_y.push_back(y);
    //     _CEMC_Hit_z.push_back(z);
    //     _CEMC_Hit_t.push_back(t);
    //     _CEMC_Hit_particle_x.push_back(vtx->get_x());
    //     _CEMC_Hit_particle_y.push_back(vtx->get_y());
    //     _CEMC_Hit_particle_z.push_back(vtx->get_z());
    // }

    _tree->Fill();
}


// -----------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
// get some calo info:
// RawTowerContainer - RawTower : Tower of calorimeters, records a tower ID# -> GeV of calibrated energy 
// findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
// findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_EEMC");
// findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
// findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
// findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");

// findNode::getClass<RawTowerContainer>(topNode, "TOWER_RAW_SPILL_WARBLER");
// findNode::getClass<RawTowerContainer>(topNode, "TOWER_RAW_CEMC");
// findNode::getClass<RawTowerContainer>(topNode, "TOWER_RAW_LG_CEMC");

// findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC_RETOWER");


// RawClusterContainer - RawCluster : Cluster as groups of calorimeter towers 
// findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
// findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALIN");
// findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALOUT");

// findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC_MOD");
// findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALIN_MOD");
// findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALOUT_MOD");

// RawClusterContainer *EMCAL_RawClusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTERINFO_POS_COR_CEMC");
// RawClusterContainer *clustersEM = findNode::getClass<RawClusterContainer>(topNode, "TOPOCLUSTER_EMCAL");
// RawClusterContainer *clustersHAD = findNode::getClass<RawClusterContainer>(topNode, "TOPOCLUSTER_HCAL");

// findNode::getClass<RawClusterContainer>(topNode, "CLUSTERINFO_" + m_detector);