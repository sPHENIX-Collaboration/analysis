#include "caloTreeGen.h"

//Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>

//ROOT stuff
#include <TH1F.h>
#include <TFile.h>

// MBD
#include <mbd/MbdOut.h>

//Tower stuff
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>

// z-vertex stuff
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

//____________________________________________________________________________..
caloTreeGen::caloTreeGen(const std::string &name):
  SubsysReco(name)
  ,out(nullptr)
{
  std::cout << "caloTreeGen::caloTreeGen(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
caloTreeGen::~caloTreeGen() {
  std::cout << "caloTreeGen::~caloTreeGen() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
Int_t caloTreeGen::Init(PHCompositeNode *topNode) {
  out = new TFile(Outfile.c_str(),"RECREATE");

  hVtxZ = new TH1F("hVtxZ", "Event z-vertex; z [cm]; Counts", bins_vtx_z, low_vtx_z, high_vtx_z);
  hVtxZv2 = new TH1F("hVtxZv2", "Event z-vertex with MinBias only; z [cm]; Counts", bins_vtx_z, low_vtx_z, high_vtx_z);
  hTotalCaloE = new TH1F("hTotalCaloE", "Total Calo E; Total Calo E [GeV]; Counts", bins_totalcaloE, low_totalcaloE, high_totalcaloE);

  h2TowEtaPhiWeighted = new TH2F("h2TowEtaPhiWeighted", "Tower Energy; Towerid #phi; Towerid #eta",  bins_phi, -0.5, bins_phi-0.5, bins_eta, -0.5, bins_eta-0.5);
  h2TowEtaPhiWeightedDefault = new TH2F("h2TowEtaPhiWeightedDefault", "Tower Energy; Towerid #phi; Towerid #eta",  bins_phi, -0.5, bins_phi-0.5, bins_eta, -0.5, bins_eta-0.5);
  h2TowEtaPhiWeightedRelativeDiff = new TH2F("h2TowEtaPhiWeightedRelativeDiff", "Tower Energy; Towerid #phi; Towerid #eta",  bins_phi, -0.5, bins_phi-0.5, bins_eta, -0.5, bins_eta-0.5);

  //so that the histos actually get written out
  Fun4AllServer *se = Fun4AllServer::instance();
  se -> Print("NODETREE");

  std::cout << "towerNode Default: " << towerNodeDefault << std::endl;
  std::cout << "towerNode: " << towerNode << std::endl;

  std::cout << "caloTreeGen::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t caloTreeGen::process_event(PHCompositeNode *topNode)
{
  if(iEvent%20 == 0) std::cout << "Progress: " << iEvent << std::endl;
  iEvent++;

  //tower information
  TowerInfoContainer* emcTowerContainer        = findNode::getClass<TowerInfoContainer>(topNode,towerNode.c_str());
  TowerInfoContainer* emcTowerContainerDefault = findNode::getClass<TowerInfoContainer>(topNode,towerNodeDefault.c_str());

  if(!emcTowerContainer || !emcTowerContainerDefault)
  {
    std::cout << PHWHERE << "caloTreeGen::process_event Could not find node: " <<  towerNode << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  MbdOut* _mbd_out = findNode::getClass<MbdOut>(topNode, "MbdOut");

  if (!_mbd_out)
  {
    std::cout << PHWHERE << "caloTreeGen::process_event: Could not find node MbdOut" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  //----------------------------------vertex------------------------------------------------------//
  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap || vertexmap->empty() || !(vertexmap->begin()->second) || !(vertexmap->begin()->second->isValid()))
  {
    if(!vertexmap)                         std::cout << PHWHERE << "caloTreeGen::process_event: Could not find node GlobalVertexMap" << std::endl;
    else if(vertexmap->empty())            std::cout << PHWHERE << "caloTreeGen::process_event: GlobalVertexMap is empty" << std::endl;
    else if(!(vertexmap->begin()->second)) std::cout << PHWHERE << "caloTreeGen::process_event: GlobalVertex is null" << std::endl;
    else                                   std::cout << PHWHERE << "caloTreeGen::process_event: GlobalVertex is not valid" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  GlobalVertex* vtx = vertexmap->begin()->second;
  Float_t vtx_z = vtx->get_z();
  min_vtx_z = std::min(min_vtx_z, vtx_z);
  max_vtx_z = std::max(max_vtx_z, vtx_z);
  hVtxZ->Fill(vtx_z);
  //----------------------------------vertex------------------------------------------------------//

  //----------------------------------Minimum Bias Classifier------------------------------------------------------//
  Bool_t isMinBias = true;
  for (int i = 0; i < 2; i++)
  {
    if (_mbd_out->get_npmt(i) < _mbd_tube_cut)
    {
      isMinBias = false;
      std::cout << "i: " << i << ", _mbd_out->get_npmt: " << _mbd_out->get_npmt(i) << std::endl;
    }
  }

  if (_mbd_out->get_q(1) < _mbd_north_cut && _mbd_out->get_q(0) > _mbd_south_cut)
  {
    isMinBias = false;
    std::cout << "_mbd_out->get_q(0): "   << _mbd_out->get_q(0)
              << ", _mbd_out->get_q(1): " << _mbd_out->get_q(1) << std::endl;
  }

  if(abs(vtx_z) >= _z_vtx_cut) {
    isMinBias = false;
  }

  if(!isMinBias)
  {
    std::cout << PHWHERE << "caloTreeGen::process_event: " << iEvent << " is not MinimumBias" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  //----------------------------------Minimum Bias Classifier------------------------------------------------------//

  hVtxZv2->Fill(vtx_z);

  // reject events that do not pass the z-vertex cuts
  if(abs(vtx_z) >= vtx_z_max) {
    std::cout << PHWHERE << "|z-vertex| >= " << vtx_z_max << ": " << vtx_z << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  //grab all the towers and fill their energies.
  UInt_t tower_range = emcTowerContainer->size();
  Float_t totalCaloE = 0;
  Int_t goodTowerCtr = 0;
  for(UInt_t iter = 0; iter < tower_range; ++iter)
  {
    UInt_t key  = TowerInfoDefs::encode_emcal(iter);
    UInt_t ieta = TowerInfoDefs::getCaloTowerEtaBin(key);
    UInt_t iphi = TowerInfoDefs::getCaloTowerPhiBin(key);

    TowerInfo* tower        = emcTowerContainer        -> get_tower_at_channel(iter);
    TowerInfo* towerDefault = emcTowerContainerDefault -> get_tower_at_channel(iter);
    Double_t energy        = tower        -> get_energy();
    Double_t energyDefault = towerDefault -> get_energy();
    Double_t relativeDiff  = (energyDefault) ? fabs((energy-energyDefault)/energyDefault) : 0;

    // check if tower is good
    if(!tower->get_isGood() || !towerDefault->get_isGood()) continue;

    ++goodTowerCtr;

    totalCaloE += energy;

    min_towE = std::min(min_towE, energy);
    max_towE = std::max(max_towE, energy);

    // check to make sure that ieta and iphi are in range
    if(ieta >= bins_eta || iphi >= bins_phi) std::cout << "ieta: " << ieta << ", iphi: " << iphi << std::endl;

    // if(!isSim || (isSim && ieta >= 8)) h2TowEtaPhiWeighted->Fill(ieta, iphi, energy);
    h2TowEtaPhiWeighted->Fill(iphi, ieta, energy);
    h2TowEtaPhiWeightedDefault->Fill(iphi, ieta, energyDefault);

    if(relativeDiff) {
      min_relativeDiff = std::min(min_relativeDiff, relativeDiff);
      max_relativeDiff = std::max(max_relativeDiff, relativeDiff);

      h2TowEtaPhiWeightedRelativeDiff->Fill(iphi, ieta, relativeDiff);
    }
  }

  avg_goodTowers += goodTowerCtr*1./tower_range;

  max_totalCaloE = std::max(max_totalCaloE, totalCaloE);
  min_totalCaloE = std::min(min_totalCaloE, totalCaloE);

  hTotalCaloE->Fill(totalCaloE);

  ++iEventGood;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t caloTreeGen::End(PHCompositeNode *topNode)
{

  std::cout << "Total Events: " << iEvent << ", Accepted Events: " << iEventGood << ", " << iEventGood*100./iEvent << " %" << std::endl;
  std::cout << "min z-vertex: " << min_vtx_z << ", max z-vertex: " << max_vtx_z << std::endl;
  std::cout << "min totalCaloE: " << min_totalCaloE << ", max totalCaloE: " << max_totalCaloE << std::endl;
  std::cout << "min tower energy: " << min_towE << ", max tower energy: " << max_towE << std::endl;
  std::cout << "avg_goodTowers: " << avg_goodTowers*100./iEventGood << " %" << std::endl;
  std::cout << "min relativeDiff: " << min_relativeDiff << ", max relativeDiff: " << max_relativeDiff << std::endl;

  std::cout << "caloTreeGen::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  out -> cd();
  hVtxZ->Write();
  hVtxZv2->Write();
  hTotalCaloE->Write();
  h2TowEtaPhiWeighted->Write();
  h2TowEtaPhiWeightedDefault->Write();
  h2TowEtaPhiWeightedRelativeDiff->Write();
  out -> Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

