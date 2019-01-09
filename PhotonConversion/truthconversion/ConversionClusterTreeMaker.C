#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <fun4all/Fun4AllServer.h>


#include "ConversionClusterTreeMaker.h"
#include "TLorentzVector.h"
#include <iostream>
#include <vector>

#ifndef __CINT__
#include <list>
#endif //CINT

#include <calotrigger/CaloTriggerInfo.h>

#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawClusterContainer.h>

#include <calobase/RawTower.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer.h>

#include <g4vertex/GlobalVertex.h>
#include <g4vertex/GlobalVertexMap.h>

#include <jetbackground/TowerBackground.h>

ChaseTower findMaxTower(std::vector<ChaseTower> towers)
{
  ChaseTower MaxTower;
  MaxTower.setKey(0);
  MaxTower.setEnergy(0);
  MaxTower.setPhi(0);
  MaxTower.setEta(0);

  for(unsigned int i = 0; i < towers.size(); i++)
  {
    if(towers.at(i).getEnergy() > MaxTower.getEnergy())
    {
      MaxTower.setKey(towers.at(i).getKey());
      MaxTower.setEnergy(towers.at(i).getEnergy());
      MaxTower.setPhi(towers.at(i).getPhi());
      MaxTower.setEta(towers.at(i).getEta());
    }
  }
  return MaxTower;
}

ConversionClusterTreeMaker::ConversionClusterTreeMaker(const std::string &name) : SubsysReco("ConversionClusterTree")
{
  _foutname = name;

}

ConversionClusterTreeMaker::ConversionClusterTreeMaker(const std::string &name, TruthConversionEval* conversionModule) : SubsysReco("ConversionClusterTree"),conversionModule(conversionModule)
{
  _foutname = name;
}
int ConversionClusterTreeMaker::Init(PHCompositeNode *topNode)
{
  _f = new TFile( _foutname.c_str(), "RECREATE");

  _tree = new TTree("ttree","a gentle baobab tree");

  //reco cluster information
  _tree->Branch("cluster_n", &_b_cluster_n);
  _tree->Branch("cluster_et", _b_cluster_et,"cluster_et[cluster_n]/D");
  _tree->Branch("cluster_eta",_b_cluster_eta,"cluster_eta[cluster_n]/D");
  _tree->Branch("cluster_phi",_b_cluster_phi,"cluster_phi[cluster_n]/D");
  _tree->Branch("cluster_prob",_b_cluster_prob,"cluster_prob[cluster_n]/D");


  _tree->Branch("NTowers",_b_NTowers,"NTowers[cluster_n]/I");

  _tree->Branch("clusterTower_towers",&_b_clusterTower_towers);
  _tree->Branch("clusterTower_eta",_b_clusterTower_eta,"clusterTower_eta[clusterTower_towers]/D");
  _tree->Branch("clusterTower_phi",_b_clusterTower_phi,"clusterTower_phi[clusterTower_towers]/D");
  _tree->Branch("clusterTower_energy",_b_clusterTower_energy,"clusterTower_energy[clusterTower_towers]/D");
  return 0;
}


int ConversionClusterTreeMaker::process_event(PHCompositeNode *topNode)
{

  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  vx = NAN;
  vy = NAN;
  vz = NAN;
  if (vertexmap)
  {
    if (!vertexmap->empty())
    {
      GlobalVertex* vertex = (vertexmap->begin()->second);

      vx = vertex->get_x();
      vy = vertex->get_y();
      vz = vertex->get_z();
    }
  }

  //////////////////////////////////////Find cluster information/////////////////////////////////////////////////////
  _b_cluster_n = 0;
  _b_clusterTower_towers = 0;

  RawTowerContainer *towersEM3old = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
  RawTowerGeomContainer *geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  const RawClusterContainer *clusters = conversionModule->getClusters();
  //find correct vertex
  vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap"); 
  vx=vy=vz=0;
  if (vertexmap&&!vertexmap->empty())
  {
    GlobalVertex* vertex = (vertexmap->begin()->second);
    vx = vertex->get_x();
    vy = vertex->get_y();
    vz = vertex->get_z();
  }
  for (RawClusterContainer::ConstIterator clusiter = clusters->getClusters().first; clusiter !=  clusters->getClusters().second; ++clusiter) 
  {
    RawCluster *cluster = clusiter->second;
    if(cluster){ //only process valid clusters
      CLHEP::Hep3Vector vertex( vx, vy, vz); //set new correct vertex for eta calculation
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*cluster, vertex);
      double cluster_energy = E_vec_cluster.mag();
      double cluster_eta = E_vec_cluster.pseudoRapidity(); 
      double cluster_phi = E_vec_cluster.phi();
      double et = cluster_energy / cosh( cluster_eta );
      double prob = cluster->get_prob();

      if (et < 1) continue;
      _b_cluster_eta[ _b_cluster_n ] = cluster_eta;
      _b_cluster_phi[ _b_cluster_n ] = cluster_phi;
      _b_cluster_et[ _b_cluster_n ] = et;
      _b_cluster_prob[ _b_cluster_n ] = prob;    

      _b_NTowers[_b_cluster_n] = cluster->getNTowers();

      std::vector <ChaseTower> clusterTowers;

      RawCluster::TowerConstRange clusterrange = cluster->get_towers();
      for (RawCluster::TowerConstIterator rtiter = clusterrange.first; rtiter != clusterrange.second; ++rtiter) 
      {
        RawTower *tower = towersEM3old->getTower(rtiter->first);
        RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());
        ChaseTower temp;
        temp.setEta(tower_geom->get_eta());
        _b_clusterTower_eta[_b_clusterTower_towers] = tower_geom->get_eta();
        temp.setPhi(tower_geom->get_phi());
        _b_clusterTower_phi[_b_clusterTower_towers] = tower_geom->get_phi();
        temp.setEnergy(tower->get_energy());
        _b_clusterTower_energy[_b_clusterTower_towers] = tower->get_energy();
        temp.setKey(tower->get_key());
        clusterTowers.push_back(temp);
        _b_clusterTower_towers++;
      }
      _b_cluster_n++;
    }
  }


  _tree->Fill();

  return 0; 
}

int ConversionClusterTreeMaker::End(PHCompositeNode *topNode)
{

  _f->Write();
  _f->Close();

  return 0;
}

