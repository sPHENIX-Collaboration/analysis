#include "caloTreeGen.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>
#include <ffaobjects/EventHeader.h>

//ROOT stuff
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TTree.h>

//For clusters and geometry
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerGeomContainer.h>

//Tower stuff
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

//thingy
#include <CLHEP/Geometry/Point3D.h>


//____________________________________________________________________________..
caloTreeGen::caloTreeGen(const std::string &name):
SubsysReco(name)
  ,T(nullptr)
  ,Outfile(name)
  ,doOHCal(1)
  ,doIHCal(1)
  ,doClusters(1)
,totalCaloE(0)
{
  std::cout << "caloTreeGen::caloTreeGen(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
caloTreeGen::~caloTreeGen()
{
  std::cout << "caloTreeGen::~caloTreeGen() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int caloTreeGen::Init(PHCompositeNode *topNode)
{
  
  out = new TFile(Outfile.c_str(),"RECREATE");

  
  T = new TTree("T","T");
  

  //emc
  T -> Branch("emcTowE",&m_emcTowE);
  T -> Branch("emcTowEta",&m_emcTowEta);
  T -> Branch("emcTowPhi",&m_emcTowPhi);
  T -> Branch("emcTiming",&m_emcTiming);
  T -> Branch("emciEta",&m_emciEta);
  T -> Branch("emciPhi",&m_emciPhi);
  
  T -> Branch("ihcTowE",&m_ihcTowE);
  T -> Branch("ihcTowEta",&m_ihcTowEta);
  T -> Branch("ihcTowPhi",&m_ihcTowPhi);
  T -> Branch("ihcTiming",&m_ihcTiming);
  T -> Branch("ihciEta",&m_ihciEta);
  T -> Branch("ihciPhi",&m_ihciPhi);
  
  T -> Branch("ohcTowE",&m_ohcTowE);
  T -> Branch("ohcTowEta",&m_ohcTowEta);
  T -> Branch("ohcTowPhi",&m_ohcTowPhi);
  T -> Branch("ohcTiming",&m_ohcTiming);
  T -> Branch("ohciEta",&m_ihciEta);
  T -> Branch("ohciPhi",&m_ihciPhi);
  
  T -> Branch("clusterE",&m_clusterE);
  T -> Branch("clusterPhi",&m_clusterPhi);
  T -> Branch("clusterEta", &m_clusterEta);
  T -> Branch("clustrPt", &m_clusterPt);
  T -> Branch("clusterChi", &m_clusterChi);
  T -> Branch("clusterNtow",&m_clusterNtow);
  T -> Branch("clusterTowMax",&m_clusterTowMax);
  T -> Branch("clusterECore",&m_clusterECore);
  
  T -> Branch("totalCaloE",&totalCaloE);
 //so that the histos actually get written out
  Fun4AllServer *se = Fun4AllServer::instance();
  se -> Print("NODETREE"); 
  //hm = new Fun4AllHistoManager("MYHISTOS");

  //se -> registerHistoManager(hm);

  //se -> registerHisto(T -> GetName(), T);

  
  std::cout << "caloTreeGen::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::InitRun(PHCompositeNode *topNode)
{
  std::cout << "caloTreeGen::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::process_event(PHCompositeNode *topNode)
{

   //Information on clusters
  RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTERINFO_CEMC");
  if(!clusterContainer && doClusters)
    {
      std::cout << PHWHERE << "caloTreeGen::process_event - Fatal Error - CLUSTER_CEMC node is missing. " << std::endl;
      return 0;
    }
  
  //tower information
  TowerInfoContainer *emcTowerContainer;
  if(doClusters)emcTowerContainer = findNode::getClass<TowerInfoContainer>(topNode,"TOWERINFO_CALIB_CEMC");
  else emcTowerContainer =  findNode::getClass<TowerInfoContainer>(topNode,"TOWERS_CEMC");
  if(!emcTowerContainer)
    {
      std::cout << PHWHERE << "caloTreeGen::process_event Could not find node TOWERS_CEMC"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  
  
  //Tower geometry node for location information
  RawTowerGeomContainer *towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  if (!towergeom && doClusters)
    {
      std::cout << PHWHERE << "caloTreeGen::process_event Could not find node TOWERGEOM_CEMC"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  
  float calib = 1.;

  //grab all the towers and fill their energies. 
  unsigned int tower_range = emcTowerContainer->size();
  totalCaloE = 0;
  for(unsigned int iter = 0; iter < tower_range; iter++)
    {
      unsigned int towerkey = emcTowerContainer->encode_key(iter);
      unsigned int ieta = getCaloTowerEtaBin(towerkey);
      unsigned int iphi = getCaloTowerPhiBin(towerkey);
      m_emciEta.push_back(ieta);
      m_emciPhi.push_back(iphi);
      
      double energy = emcTowerContainer -> get_tower_at_channel(iter) -> get_energy()/calib;
      totalCaloE += energy;
      double time = emcTowerContainer -> get_tower_at_channel(iter) -> get_time();
      
      if(doClusters)
	{
	  double phi = towergeom -> get_phicenter(iphi);
	  double eta = towergeom -> get_etacenter(ieta);
	  m_emcTowPhi.push_back(phi);
	  m_emcTowEta.push_back(eta);
	}
      m_emcTowE.push_back(energy);
      m_emcTiming.push_back(time);
      
    } 

  //Tower geometry node for location information
  if(doOHCal)
    {
    

      TowerInfoContainer *ohcTowerContainer = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
      if(!ohcTowerContainer)
	{
	  std::cout << PHWHERE << "caloTreeGen::process_event Could not find node TOWERS_HCALOUT"  << std::endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
	}
      
      towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
      if (!towergeom && doClusters)
	{
	  std::cout << PHWHERE << "caloTreeGen::process_event Could not find node TOWERGEOM_HCALOUT"  << std::endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
	}
      tower_range = ohcTowerContainer->size();
      for(unsigned int iter = 0; iter < tower_range; iter++)
	{
	  unsigned int towerkey = ohcTowerContainer->encode_key(iter);
	  unsigned int ieta = getCaloTowerEtaBin(towerkey);
	  unsigned int iphi = getCaloTowerPhiBin(towerkey);
	  m_ohciEta.push_back(ieta);
	  m_ohciPhi.push_back(iphi);
	  
	  double energy = ohcTowerContainer -> get_tower_at_channel(iter) -> get_energy()/calib;
	  double time = ohcTowerContainer -> get_tower_at_channel(iter) -> get_time();

	  if(doClusters)
	    {
	      double phi = towergeom -> get_phicenter(iphi);
	      double eta = towergeom -> get_etacenter(ieta);
	      m_ohcTowPhi.push_back(phi);
	      m_ohcTowEta.push_back(eta);
	      
	    }
	  m_ohcTowE.push_back(energy);
	  m_ohcTiming.push_back(time);
	}
    }
  //Tower geometry node for location information
  if(doIHCal)
    {
      TowerInfoContainer *ihcTowerContainer = findNode::getClass<TowerInfoContainer>(topNode,"TOWERINFO_CALIB_HCALIN");
      if(!ihcTowerContainer)
	{
	  std::cout << PHWHERE << "caloTreeGen::process_event Could not find node TOWERS_HCALIN"  << std::endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
	}
      towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
      if (!towergeom && doClusters)
	{
	  std::cout << PHWHERE << "caloTreeGen::process_event Could not find node TOWERGEOM_HCALIN"  << std::endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
	}
      tower_range = ihcTowerContainer->size();
      for(unsigned int iter = 0; iter < tower_range; iter++)
	{
	  unsigned int towerkey = ihcTowerContainer->encode_key(iter);
	  unsigned int ieta = getCaloTowerEtaBin(towerkey);
	  unsigned int iphi = getCaloTowerPhiBin(towerkey);
	  m_ihciEta.push_back(ieta);
	  m_ihciPhi.push_back(iphi);
	 
	  double energy = ihcTowerContainer -> get_tower_at_channel(iter) -> get_energy()/calib;
	  double time = ihcTowerContainer -> get_tower_at_channel(iter) -> get_time();
	  
	  if(doClusters)
	    {
	      double phi = towergeom -> get_phicenter(iphi);
	      double eta = towergeom -> get_etacenter(ieta);
	      m_ihcTowPhi.push_back(phi);
	      m_ihcTowEta.push_back(eta);
	    }
	  m_ihcTowE.push_back(energy);
	  m_ihcTiming.push_back(time);
	}
    }
  
  if(doClusters)
    {
      RawClusterContainer::ConstRange clusterEnd = clusterContainer -> getClusters();
      RawClusterContainer::ConstIterator clusterIter;
  
      for(clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++)
	{
	  RawCluster *recoCluster = clusterIter -> second;

	  CLHEP::Hep3Vector vertex(0,0,0);
	  CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);
	  CLHEP::Hep3Vector E_vec_cluster_Full = RawClusterUtility::GetEVec(*recoCluster, vertex);

	  float clusE = E_vec_cluster.mag();
	  float clus_eta = E_vec_cluster.pseudoRapidity();
	  float clus_phi = E_vec_cluster.phi();
	  float clus_pt = E_vec_cluster.perp();
	  float clus_chi = recoCluster -> get_chi2();
	  float nTowers = recoCluster ->getNTowers(); 
	  float maxTowerEnergy = getMaxTowerE(recoCluster,emcTowerContainer);

	  m_clusterE.push_back(E_vec_cluster_Full.mag());
	  m_clusterECore.push_back(clusE);
	  m_clusterPhi.push_back(clus_phi);
	  m_clusterEta.push_back(clus_eta);
	  m_clusterPt.push_back(clus_pt);
	  m_clusterChi.push_back(clus_chi);
	  m_clusterNtow.push_back(nTowers);
	  m_clusterTowMax.push_back(maxTowerEnergy);
	  
	}
    }
  T -> Fill();
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::ResetEvent(PHCompositeNode *topNode)
{
  m_clusterE.clear();
  m_clusterPhi.clear();
  m_clusterEta.clear();
  m_clusterPt.clear();
  m_clusterChi.clear();
  m_clusterTowMax.clear();
  m_clusterNtow.clear();
  m_clusterECore.clear();

  m_emcTowPhi.clear();
  m_emcTowEta.clear();
  m_emcTowE.clear();
  m_emcTiming.clear();
  m_emciEta.clear();
  m_emciPhi.clear();
      
  m_ohcTowPhi.clear();
  m_ohcTowEta.clear();
  m_ohcTowE.clear();
  m_ohcTiming.clear();
  m_ohciEta.clear();
  m_ohciPhi.clear();

  m_ihcTowPhi.clear();
  m_ihcTowEta.clear();
  m_ihcTowE.clear();
  m_ihcTiming.clear();
  m_ihciEta.clear();
  m_ihciPhi.clear();
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::EndRun(const int runnumber)
{
  std::cout << "caloTreeGen::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::End(PHCompositeNode *topNode)
{
  std::cout << "caloTreeGen::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  out -> cd();
  T -> Write();
  out -> Close();
  delete out;

  //hm -> dumpHistos(Outfile.c_str(), "UPDATE");
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::Reset(PHCompositeNode *topNode)
{
 std::cout << "caloTreeGen::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void caloTreeGen::Print(const std::string &what) const
{
  std::cout << "caloTreeGen::Print(const std::string &what) const Printing info for " << what << std::endl;
}
// convert from calorimeter key to phi bin
//____________________________________________________________________________..
unsigned int caloTreeGen::getCaloTowerPhiBin(const unsigned int key)
{
  unsigned int etabin = key >> 16U;
  unsigned int phibin = key - (etabin << 16U);
  return phibin;
}

//____________________________________________________________________________..
// convert from calorimeter key to eta bin
unsigned int caloTreeGen::getCaloTowerEtaBin(const unsigned int key)
{
  unsigned int etabin = key >> 16U;
  return etabin;
}
//____________________________________________________________________________..
// convert from calorimeter key to eta bin
float caloTreeGen::getMaxTowerE(RawCluster *cluster, TowerInfoContainer *towerContainer)
{
  RawCluster::TowerConstRange towers = cluster -> get_towers();
  RawCluster::TowerConstIterator toweriter;
  
  float maxEnergy = 0;
  for(toweriter = towers.first; toweriter != towers.second; toweriter++)
    {
      
      //std::cout << "toweriter -> second: " << toweriter -> second << std::endl;
      //float towE = towerContainer -> get_tower_at_channel(toweriter->first) -> get_energy();
      float towE = toweriter -> second;
      if( towE > maxEnergy)  maxEnergy = towE;
    }
  return maxEnergy;
}
