#include "caloTreeGen.h"

// Jet.
#include <jetbase/Jetv1.h>
#include <jetbase/Jetv2.h>
#include <jetbase/JetContainer.h>

// Tower.
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfov1.h>
#include <calobase/TowerInfov2.h>
#include <calobase/TowerInfov3.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfoContainerv3.h>
#include <calobase/TowerInfoContainerv4.h>
#include <calobase/TowerInfoDefs.h>

// Vertex.
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/MbdVertex.h>
#include <globalvertex/MbdVertexMap.h>

// GL1 Information.
#include <ffarawobjects/Gl1Packet.h>

// Fun4All.
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

// Nodes.
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>

#include <pdbcalbase/PdbParameterMap.h>
#include <phparameter/PHParameters.h>

// General.
#include <cstdint>
#include <iostream>
#include <map>
#include <utility>

class PHCompositeNode;
class TowerInfoContainer;
class caloTreeGen;

caloTreeGen::caloTreeGen(const std::string &name, const std::string &outfilename)
  :SubsysReco(name)
{
  verbosity = 0;
  doRecoJet_radius = {0.4};
  foutname = outfilename;
  std::cout << "caloTreeGen::caloTreeGen(const std::string &name) Calling ctor" << std::endl;
}

int caloTreeGen::Init(PHCompositeNode * /*topNode*/) {
  if (verbosity > 0) std::cout << "Processing initialization: CaloEmulatorTreeMaker::Init(PHCompositeNode *topNode)" << std::endl;
  file = new TFile( foutname.c_str(), "RECREATE");
  tree = new TTree("ttree","TTree for JES calibration");

  // Vertex information.
  Initialize_z_vertex();
  // Jet information.
  for (int ir = 0; ir < static_cast<int>(doRecoJet_radius.size()); ir++) {
    Initialize_jet("unsubjet", doRecoJet_radius[ir]);
  }
  // Trigger information.
  Initialize_trigger_vector();

  ievent = 0;
  return Fun4AllReturnCodes::EVENT_OK;
}

int caloTreeGen::process_event(PHCompositeNode *topNode) {
  if (verbosity >= 0) {
    if (ievent%100 == 0) std::cout << "Processing event " << ievent << std::endl;
  }

  // Vertex information.
  Fill_z_vertex(topNode);
  // Jet information.
  for (int ir = 0; ir < static_cast<int>(doRecoJet_radius.size()); ir++) {
    Fill_recojet(topNode, "unsubjet", "unsubtracted", doRecoJet_radius[ir]);
  }
  // Trigger information.
  Fill_trigger_vector(topNode);

  tree->Fill();
  ievent++;
  return Fun4AllReturnCodes::EVENT_OK;
}

int caloTreeGen::ResetEvent(PHCompositeNode * /*topNode*/) {
  if (Verbosity() > 1) std::cout << "Resetting the tree branches" << std::endl;

  Reset_jet();

  return Fun4AllReturnCodes::EVENT_OK;
}

int caloTreeGen::End(PHCompositeNode * /*topNode*/) {
  std::cout << "caloTreeGen::End(PHCompositeNode *topNode) Saving TTree" << std::endl;
  std::cout<<"Total events: "<<ievent<<std::endl;
  file->cd();
  tree->Write();
  file->Close();
  delete file;
  std::cout << "CaloEmulatorTreeMaker complete." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

////////// ********** Initialize functions ********** //////////
void caloTreeGen::Initialize_z_vertex() {
  tree->Branch("z_vertex", &z_vertex, "z_vertex/F");
}

void caloTreeGen::Initialize_jet(std::string jet_prefix , float radius) {
  // Prefix for the jet.
  std::string vector_prefix = jet_prefix + "0" + std::to_string((int)(radius * 10)) + "_";
  // Jet general information.
  std::string jet_e_vectorname = vector_prefix + "e"; jet_e_map[jet_e_vectorname] = std::vector<float>(); tree->Branch(jet_e_vectorname.c_str(), &jet_e_map[jet_e_vectorname]);
  std::string jet_et_vectorname = vector_prefix + "et"; jet_et_map[jet_et_vectorname] = std::vector<float>(); tree->Branch(jet_et_vectorname.c_str(), &jet_et_map[jet_et_vectorname]);
  std::string jet_pt_vectorname = vector_prefix + "pt"; jet_pt_map[jet_pt_vectorname] = std::vector<float>(); tree->Branch(jet_pt_vectorname.c_str(), &jet_pt_map[jet_pt_vectorname]);
  std::string jet_eta_vectorname = vector_prefix + "eta"; jet_eta_map[jet_eta_vectorname] = std::vector<float>(); tree->Branch(jet_eta_vectorname.c_str(), &jet_eta_map[jet_eta_vectorname]);
  std::string jet_phi_vectorname = vector_prefix + "phi"; jet_phi_map[jet_phi_vectorname] = std::vector<float>(); tree->Branch(jet_phi_vectorname.c_str(), &jet_phi_map[jet_phi_vectorname]);
  std::string jet_emcal_calo_e_vectorname = vector_prefix + "emcal_calo_e"; jet_calo_e_map[jet_emcal_calo_e_vectorname] = std::vector<float>(); tree->Branch(jet_emcal_calo_e_vectorname.c_str(), &jet_calo_e_map[jet_emcal_calo_e_vectorname]); // iHCaL tower information.
  std::string jet_ihcal_calo_e_vectorname = vector_prefix + "ihcal_calo_e"; jet_calo_e_map[jet_ihcal_calo_e_vectorname] = std::vector<float>(); tree->Branch(jet_ihcal_calo_e_vectorname.c_str(), &jet_calo_e_map[jet_ihcal_calo_e_vectorname]);
  std::string jet_ohcal_calo_e_vectorname = vector_prefix + "ohcal_calo_e"; jet_calo_e_map[jet_ohcal_calo_e_vectorname] = std::vector<float>(); tree->Branch(jet_ohcal_calo_e_vectorname.c_str(), &jet_calo_e_map[jet_ohcal_calo_e_vectorname]);
}

void caloTreeGen::Initialize_trigger_vector() {
  tree->Branch("gl1_trigger_vector_live", gl1_trigger_vector_live, "gl1_trigger_vector_live[64]/I");
  tree->Branch("gl1_trigger_vector_scaled", gl1_trigger_vector_scaled, "gl1_trigger_vector_scaled[64]/I");
  tree->Branch("gl1_trigger_scalar_live", gl1_trigger_scalar_live, "gl1_trigger_scalar_live[64]/l");
  tree->Branch("gl1_trigger_scalar_scaled", gl1_trigger_scalar_scaled, "gl1_trigger_scalar_scaled[64]/l");
}

////////// ********** Fill functions ********** //////////
void caloTreeGen::Fill_z_vertex(PHCompositeNode *topNode) {
  z_vertex = -999;
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (vertexmap) {
    if (!vertexmap->empty()) {
      GlobalVertex *vtx = vertexmap->begin()->second;
      if (vtx) {
        z_vertex = vtx->get_z();
      }
    } else {
      std::cout << "GlobalVertexMap is empty" << std::endl;
    }
  } else {
    std::cout << "GlobalVertexMap is missing" << std::endl;
  }
}

void caloTreeGen::Fill_recojet(PHCompositeNode *topNode, std::string jet_prefix, std::string node_prefix, float radius) {
  // Set vector name prefix for the jet map.
  std::string vector_prefix = jet_prefix + "0" + std::to_string((int)(radius * 10)) + "_";
  // Get the jet container.
  std::string nodename = "AntiKt_" + node_prefix + "_r0" + std::to_string((int)(radius * 10));
  if (node_prefix == "subtracted") {
    nodename = "AntiKt_Tower_r0" + std::to_string((int)(radius * 10)) + "_Sub1";
  }
  JetContainer* _jets = findNode::getClass<JetContainer>(topNode, nodename);
  if (!_jets) std::cout << "JetContainer for " << nodename << " is missing" << std::endl;
  // Get the towerinfo container.
  TowerInfoContainer *_emcal_towers = nullptr;
  TowerInfoContainer *_ihcal_towers = nullptr;
  TowerInfoContainer *_ohcal_towers = nullptr;
  if (node_prefix == "unsubtracted") {
    _emcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
    _ihcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
    _ohcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
  } else {
    _emcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER_SUB1");
    _ihcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN_SUB1");
    _ohcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT_SUB1");
  }
  if (!_emcal_towers || !_ihcal_towers || !_ohcal_towers) std::cout << "TowerInfoContainer is missing" << std::endl;
  // Fill the jet information.
  for (auto _jet : *_jets) {
    if (_jet->get_pt() < 1) continue;
    jet_e_map[vector_prefix+"e"].push_back(_jet->get_e());
    jet_et_map[vector_prefix+"et"].push_back(_jet->get_et());
    jet_pt_map[vector_prefix+"pt"].push_back(_jet->get_pt());
    jet_eta_map[vector_prefix+"eta"].push_back(_jet->get_eta());
    jet_phi_map[vector_prefix+"phi"].push_back(_jet->get_phi());

    float emcal_calo_e = 0;
    float ihcal_calo_e = 0;
    float ohcal_calo_e = 0;
    for (auto comp: _jet->get_comp_vec()) {
	    unsigned int channel = comp.second;
      if (comp.first == 14 || comp.first == 29 || comp.first == 25 || comp.first == 28) {
        TowerInfo *_tower = _emcal_towers->get_tower_at_channel(channel);
	      float jet_tower_e = _tower->get_energy();
        emcal_calo_e += jet_tower_e;
	    } else if (comp.first == 15 ||  comp.first == 30 || comp.first == 26) {
        TowerInfo *_tower = _ihcal_towers->get_tower_at_channel(channel);
	      float jet_tower_e = _tower->get_energy();
        ihcal_calo_e += jet_tower_e;
	    } else if (comp.first == 16 || comp.first == 31 || comp.first == 27) {
        TowerInfo *_tower = _ohcal_towers->get_tower_at_channel(channel);
	      float jet_tower_e = _tower->get_energy();
        ohcal_calo_e += jet_tower_e;
      }
	  }
    jet_calo_e_map[vector_prefix+"emcal_calo_e"].push_back(emcal_calo_e);
    jet_calo_e_map[vector_prefix+"ihcal_calo_e"].push_back(ihcal_calo_e);
    jet_calo_e_map[vector_prefix+"ohcal_calo_e"].push_back(ohcal_calo_e);
  }
}

void caloTreeGen::Fill_trigger_vector(PHCompositeNode *topNode) {
  Gl1Packet *_gl1PacketInfo = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");
  if (_gl1PacketInfo) {
    uint64_t livetriggervec = _gl1PacketInfo->lValue(0, "TriggerVector");
    uint64_t scaledtriggervec = _gl1PacketInfo->lValue(0, "ScaledVector");
   
    for (int iv = 0; iv < n_trigger_vec; ++iv) {
      gl1_trigger_vector_live[iv] = ((livetriggervec & 0x1U) == 0x1U);
      gl1_trigger_vector_scaled[iv] = ((scaledtriggervec & 0x1U) == 0x1U);
      livetriggervec = (livetriggervec >> 1U) & 0xffffffffU;
      scaledtriggervec = (scaledtriggervec >> 1U) & 0xffffffffU;

	    gl1_trigger_scalar_live[iv] = _gl1PacketInfo->lValue(iv, 1);
      gl1_trigger_scalar_scaled[iv] = _gl1PacketInfo->lValue(iv, 2);
	  }
  } else {
    std::cout << "GL1Packet is missing" << std::endl;
  }
}

////////// ********** Reset functions ********** //////////
void caloTreeGen::Reset_jet() {
  for (auto& entry : jet_e_map) entry.second.clear();
  for (auto& entry : jet_et_map) entry.second.clear();
  for (auto& entry : jet_pt_map) entry.second.clear();
  for (auto& entry : jet_eta_map) entry.second.clear();
  for (auto& entry : jet_phi_map) entry.second.clear();
  for (auto& entry : jet_calo_e_map) entry.second.clear();
}