
#include "VandyJetDSTSkimmer.h"

#include <pdbcalbase/PdbParameterMap.h>
#include <phparameter/PHParameters.h>

#include <fun4all/DBInterface.h>

#include <odbc++/resultset.h>
#include <odbc++/statement.h>

#include <iostream>                        // for operator<<, basic_ostream
#include <map>                             // for operator!=, _Rb_tree_iterator
#include <utility>                         // for pair
#include <format>

//____________________________________________________________________________..
VandyJetDSTSkimmer::VandyJetDSTSkimmer(const std::string &name)
  : SubsysReco(name)
{
}

//____________________________________________________________________________..
int VandyJetDSTSkimmer::InitRun(PHCompositeNode *topNode)
{
  towerInfoContainers[0] = findNode::getClass<TowerInfoContainer>(topNode,"TOWERINFO_CALIB_CEMC");
  towerInfoContainers[1] = findNode::getClass<TowerInfoContainer>(topNode,"TOWERINFO_CALIB_CEMC_RETOWER");
  towerInfoContainers[2] = findNode::getClass<TowerInfoContainer>(topNode,"TOWERINFO_CALIB_HCALIN");
  towerInfoContainers[3] = findNode::getClass<TowerInfoContainer>(topNode,"TOWERINFO_CALIB_HCALOUT");

  if(!towerInfoContainers[0] || !towerInfoContainers[1] || !towerInfoContainers[2] || !towerInfoContainers[3])
  {
   if(!towerInfoContainers[1] && m_doSim) std::cout<<"Missing the Retower node for the simulation" <<std::endl; 
   std::cout << "One or more TowerInfoContainers missing. Exiting" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  geoms[0] = findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_CEMC");
  geoms[1] = findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN"); //this is the retowered geom of the emcal
  geoms[2] = findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN");
  geoms[3] = findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");
  if(!geoms[0] || !geoms[1] || !geoms[2] || !geoms[3])
  {
    std::cout << "One or more Tower Geometry Containers missing. Exiting" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  geoms[0]->set_calorimeter_id(RawTowerDefs::CEMC);
  geoms[1]->set_calorimeter_id(RawTowerDefs::HCALIN);
  geoms[2]->set_calorimeter_id(RawTowerDefs::HCALOUT);

  vtxMap = findNode::getClass<GlobalVertexMap>(topNode,"GlobalVertexMap");
  if (!vtxMap)
  {
    vtxMap = findNode::getClass<GlobalVertexMap>(topNode, "MbdVertexMap");
    if(!vtxMap)
    {
      if(Verbosity()) std::cout << "VandyJetDSTSkimmer::Init(PHCompositeNode *topNode) Could not find global vertex map node" << std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }      
  }

  clusters = findNode::getClass<RawClusterContainer>(topNode, "TOPOCLUSTER_ALLCALO");
  if(!clusters)
  {
    std::cout << "VandyJetDSTSkimmer::Init - Error - Can't find TopoCluster Node TOPOCLUSTER_ALLCALO therefore no selection can be made" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  for(int i=0; i<4; i++)
  {
    jets[i] = findNode::getClass<JetContainer>(topNode, std::format("AntiKt_r{}{}",jetRStr[i],(m_doCalib ? "_calib" : "")).c_str());
    if (!jets[i])
    {
      std::cout << "VandyJetDSTSkimmer::Init - Error - Can't find Jet Node " << std::format("AntiKt_r{}{}",jetRStr[i],(m_doCalib ? "_calib" : "")).c_str() << " therefore no selection can be made" << std::endl;
//      return Fun4AllReturnCodes::ABORTRUN;
    }
  }

  if(m_doSim)
  {

    truthParticles = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
    if(!truthParticles)
    {
      std::cout << "VandyJetDSTSkimmer::Init - Error - Can't find G4TruthInfo Node therefore no selection can be made" << std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }

    for(int i=0; i<4; i++)
    {
      truthJets[i] = findNode::getClass<JetContainer>(topNode, std::format("AntiKt_Truth_r{}",jetRStr[i]).c_str());
      if (!truthJets[i])
      {
        std::cout << "VandyJetDSTSkimmer::Init - Error - Can't find Truth Jet Node " << std::format("AntiKt_Truth_r{}",jetRStr[i]).c_str() << " therefore no selection can be made" << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
      }
    }

    for(int s=0; s<7; s++)
    {
      if(m_sampleName == sampleNames[s])
      {
        sampleNumber = s;
      }
    }
    if(sampleNumber == -999)
    {
      std::cout << "VandyJetDSTSkimmer::Init - Error - Sample number for sim not set therefore no selection can be made" << std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }

  }
  

  std::cout << "VandyJetDSTSkimmer::Init - Got vertex and jets" << std::endl;

  outfile = new TFile(outfileName.c_str(),"RECREATE");
  T = new TTree("T","T");
  T->Branch("EventInfo",&m_eventInfo);
  T->Branch("TowerInfo",&m_towerInfo);
  T->Branch("TopoClusters",&m_topoclusters);
  for(int i=0; i<4; i++)
  {
    T->Branch(std::format("JetInfo_r{}",jetRStr[i]).c_str(),&m_jetInfo[i]);
  }
  if(m_doSim)
  {
    T->Branch("TruthParticles",&m_truthParticles);
    for(int i=0; i<4; i++)
    {
      T->Branch(std::format("TruthJetInfo_r{}",jetRStr[i]).c_str(),&m_truthJetInfo[i]);
    }
  }

  T->Print();

  std::string sql = "SELECT * FROM gl1_scalers WHERE runnumber = " + std::to_string(m_runnumber) + ";";
  odbc::Statement *stmt =  DBInterface::instance()->getStatement("daq");
  odbc::ResultSet *resultSet = stmt->executeQuery(sql);
  std::array<std::array<uint64_t, 3>, 64> scalers{};  // initialize to zero
  if (!resultSet)
  {
    std::cerr << "No db found for run number " << m_runnumber << ". Cannot get ZDC rate so aborting run" << std::endl;
    delete resultSet;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  while (resultSet->next())
  {
    int index = resultSet->getInt("index");
    // Iterate over the columns and fill the TriggerRunInfo object
    scalers[index][0] = resultSet->getLong("scaled");
    scalers[index][1] = resultSet->getLong("live");
    scalers[index][2] = resultSet->getLong("raw");
  }

  delete resultSet;

  m_ZDC_coincidence = (1.0*scalers[3][2]/scalers[0][2])/(106e-9);

  return Fun4AllReturnCodes::EVENT_OK;

}

//____________________________________________________________________________..
int VandyJetDSTSkimmer::process_event(PHCompositeNode *topNode)
{


  num++;
  std::cout << "working on event " << num << std::endl;


  m_towerInfo.clear();
  m_truthParticles.clear();
  m_towerInfo_map.clear();
  m_towerInfo_map2.clear();
  m_towerInfoTruth_map.clear();
  for(int i=0; i<4; i++)
  {
    m_jetInfo[i].clear();
    m_truthJetInfo[i].clear();
  }
  m_topoclusters.clear();

  PHNodeIterator itNode(topNode);
  PHCompositeNode* parNode = dynamic_cast<PHCompositeNode*>(itNode.findFirst("PHCompositeNode","PAR"));
  PdbParameterMap* flagNode;
  if(parNode)
  {
    flagNode = findNode::getClass<PdbParameterMap>(parNode, "TimingCutParams"); //note - needs to be the same as the "name" field in the object instantiation in the macro
  }
  else
  {
    std::cout << "No parNode! Abort run." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  if(flagNode)
  {
    m_cutParams.FillFrom(flagNode);
  }
  else if(!m_doSim)
  {
    std::cout << "No flagNode for bbfqa - abort run" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  bool goodTrigger = false;
  if(!m_doSim){
  	TriggerAnalyzer *ta = new TriggerAnalyzer();
  	ta->UseEmulator(m_doSim);
  	ta->decodeTriggers(topNode);
  	goodTrigger = ta->didTriggerFire("Jet 10 GeV");
 	 if(!goodTrigger)
  	{
   		 if(Verbosity())
    		{
     			std::cout << "VandyJetDSTSkimmer::process_event - Jet 10 GeV trigger not found, bad event" << std::endl;
    		}
    	return Fun4AllReturnCodes::ABORTEVENT;
  	}
  }
  else goodTrigger = true; //trigger emulator not functioning on sim
  if(vtxMap->empty())
  {
    if(Verbosity())  std::cout << "no vertex found" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  std::vector<GlobalVertex*> vertices = vtxMap->get_gvtxs_with_type(vtxTypes);
  if(vertices.empty() || !vertices.at(0))
  {
    if(Verbosity()) std::cout << "no MBD vertex found" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_vtx_z = vertices.at(0)->get_z();

  if (std::abs(m_vtx_z) > m_vtx_cut)
  {
  	 if(Verbosity())
		 std::cout << "vertex not in range \n vertex is " <<m_vtx_z<<" cm off of nominal 0"  << std::endl;
   return Fun4AllReturnCodes::ABORTEVENT;
  }

  if(m_doSim)
  {
    int truthVtxIndex = truthParticles->GetPrimaryVertexIndex();
    if(truthParticles->GetPrimaryVtx(truthVtxIndex))
    {
      m_vtx_z_truth = truthParticles->GetPrimaryVtx(truthVtxIndex)->get_z();
    }
  }


  //timing cut
 if(!m_doSim && !m_cutParams.get_int_param("passLeadtCut"))
  {
    if(Verbosity())
    {
      std::cout << "VandyJetDSTSkimmer::process_event - leading jet time cut failed, bad event" << std::endl;
    }
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  if(!m_doSim && !m_cutParams.get_int_param("passMbdDtCut"))
  {
    if(Verbosity())
    {
      std::cout << "VandyJetDSTSkimmer::process_event - MBD time cut failed, bad event" << std::endl;
    }
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  //Flag event as good only if one (or more) jets above pT threshold for each radius is found
  //If good jet is found at one R, entire event is flagged as good
  bool goodJet = false;
  for(int i=3; i>=0; i--)
  {
    for(int j=0; j<(int)jets[i]->size(); j++)
    {
      Jet *jet = jets[i]->get_jet(j);
      if(jet->get_pt() > jetR_pTMin[i])
      {
        goodJet = true;
        break;
      }
    }
    if(goodJet)
    {
      break;
    }
  }

  bool goodTruthJet = false;
  if(m_doSim)
  {
    for(int i=3; i>=0; i--)
    {
      for(int j=0; j<(int)truthJets[i]->size(); j++)
      {
        Jet *jet = truthJets[i]->get_jet(j);
        if(jet->get_pt() > truthJetR_pTMin[i][sampleNumber])
        {
          goodTruthJet = true;
          break;
        }
      }
      if(goodTruthJet)
      {
        break;
      }
    }
  }
  else
  {
    goodTruthJet = true;
  }

  if(!m_doSim && !goodJet) nRemNoSim++;
  if(m_doSim && !goodTruthJet && !goodJet) nRemSim++;

  if((!m_doSim && !goodJet) || (m_doSim && !goodTruthJet && !goodJet))
  {
    if(Verbosity())
    {
      std::cout << "VandyJetDSTSkimmer::process_event - No jets of any R with pT>20 GeV, bad event" << std::endl;
    }
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  //set event info
  m_eventInfo->set_z_vtx(m_vtx_z);
  m_eventInfo->set_ZDC_rate(m_ZDC_coincidence);

  std::pair<float, float> dijet = isGoodDijet();
  m_eventInfo->set_dijet_event((dijet.first >= 25.0 && dijet.second >= 8.4 ? true : false));
  m_eventInfo->set_lead_pT(dijet.first);
  m_eventInfo->set_sublead_pT(dijet.second);

  if(m_doSim)
  {  
    m_eventInfo->set_z_vtx_truth(m_vtx_z_truth);
    std::pair<float, float> dijetTruth = isGoodTruthDijet();
    m_eventInfo->set_dijetTruth_event((dijetTruth.first >= 5.0 && dijetTruth.second >= 5.0 ? true : false));
    m_eventInfo->set_leadTruth_pT(dijetTruth.first);
    m_eventInfo->set_subleadTruth_pT(dijetTruth.second);
  }
  else
  {
    m_eventInfo->set_z_vtx_truth(-999);
    m_eventInfo->set_dijetTruth_event(true);
    m_eventInfo->set_leadTruth_pT(-999);
    m_eventInfo->set_subleadTruth_pT(-999);
  }



  //store calorimeter towers in vector<Tower> object
  for(int calo = 0; calo < 4; calo++)
  {
    for(int i=0; i<(int)towerInfoContainers[calo]->size(); i++)
    {
      if(!towerInfoContainers[calo]->get_tower_at_channel(i)->get_isGood())
      {
        continue;
      }
      auto key = towerInfoContainers[calo]->encode_key(i);
      auto tower = towerInfoContainers[calo]->get_tower_at_channel(i);
      if(tower->get_energy() <= 0)
      {
        continue;
      }
      int etaBin = towerInfoContainers[calo]->getTowerEtaBin(key);
      int phiBin = towerInfoContainers[calo]->getTowerPhiBin(key);
      float etaCenter = geoms[calo]->get_etacenter(etaBin);
      float phiCenter = geoms[calo]->get_phicenter(phiBin);
      float r = geoms[calo]->get_radius();

      float etaCenterCorr = correct_eta(etaCenter, r);
      
      fastjet::PseudoJet tmp_pj;
      if(tower->get_energy() > 0) tmp_pj = get_PseudoJet(etaCenterCorr, phiCenter, tower->get_energy());
      else tmp_pj = get_PseudoJet(etaCenterCorr, phiCenter, 0.001);
      Tower tmpTower;
      tmpTower.set_px(tmp_pj.px());
      tmpTower.set_py(tmp_pj.py());
      tmpTower.set_pz(tmp_pj.pz());
      tmpTower.set_e(tmp_pj.e());
      tmpTower.set_calo(calo);

      m_towerInfo.push_back(tmpTower);
      m_towerInfo_map[std::make_pair(calo, i)] = m_towerInfo.size() - 1;
      m_towerInfo_map2[std::array<int,3> {calo, etaBin, phiBin}] = m_towerInfo.size() - 1;
    }
  }

  RawClusterContainer::Map clusterMap = clusters->getClustersMap();
  for(auto entry : clusterMap)
  {
    RawCluster *cluster = entry.second;
    
    std::vector<int> cons;
    for(const auto& [tower_id, tower_e] : cluster->get_towermap())
    {
      int calo = -999;
      int caloid = static_cast<int>(RawTowerDefs::decode_caloid(tower_id));
      if(caloid == RawTowerDefs::CalorimeterId::CEMC)
      {
        calo = 0;
      }
      else if(caloid == RawTowerDefs::CalorimeterId::HCALIN)
      {
        calo = 2;
      }
      else if(caloid == RawTowerDefs::CalorimeterId::HCALOUT)
      {
        calo = 3;
      }
        
      if(calo == -999)
      {
        continue;
      }

      int ieta = static_cast<int>(RawTowerDefs::decode_index1(tower_id));
      int iphi = static_cast<int>(RawTowerDefs::decode_index2(tower_id));
      double eta = geoms[calo]->get_etacenter(ieta);
      double phi = geoms[calo]->get_phicenter(iphi);
      float r = geoms[calo]->get_radius();

      float etaCorr = correct_eta(eta, r);

      fastjet::PseudoJet tmp_cons = get_PseudoJet(etaCorr, phi, tower_e);

      std::array<int, 3> lookup_key {calo, ieta, iphi};
      if(m_towerInfo_map2.find(lookup_key) != m_towerInfo_map2.end())
      {
        //Tower tmpTow = m_towerInfo.at(m_towerInfo_map2[lookup_key]);
        cons.push_back(m_towerInfo_map2[lookup_key]);
      }
    }

    //std::cout << "   constituents: " << cons.size() << std::endl;

    double clusEta = asinh(cluster->get_z()/cluster->get_r());
    double clusEtaCorr = correct_eta(clusEta, cluster->get_r());

    fastjet::PseudoJet tmp_pj = get_PseudoJet(clusEtaCorr, cluster->get_phi(), cluster->get_energy());
    JetInfo topocluster;
    topocluster.set_px(tmp_pj.px());
    topocluster.set_py(tmp_pj.py());
    topocluster.set_pz(tmp_pj.pz());
    topocluster.set_e(tmp_pj.e());
    topocluster.set_constituents(cons);
    m_topoclusters.push_back(topocluster);
  }

  // jet loop
  for(int r=0; r<4; r++)
  {
    for(auto jet : *jets[r])
    {
      double posEta = 1.1 - jetR[r];
      double posEtaCorr = correct_eta(posEta, 90.0);
      double negEta = -1.1 + jetR[r];
      double negEtaCorr = correct_eta(negEta, 90.0);

      if (jet->get_pt() < m_minJetPt || jet->get_eta() > posEtaCorr || jet->get_eta() < negEtaCorr)
      {
        continue;
      }

      std::vector<int> cons;
      for(auto comp : jet->get_comp_vec())
      {
        int calo = -999;
        
        Jet::SRC source = comp.first;
        //unsigned int unique_id = comp.second;
        int tower_id = static_cast<int>(comp.second);
        if(source == Jet::SRC::CEMC_TOWERINFO)
        {
          calo = 0;
        }
        else if(source == Jet::SRC::CEMC_TOWERINFO_RETOWER)
        {
          calo = 1;
        }
        else if(source == Jet::SRC::HCALIN_TOWERINFO)
        {
          calo = 2;
        }
        else if(source == Jet::SRC::HCALOUT_TOWERINFO)
        {
          calo = 3;
        }
        
        if(calo == -999)
        {
          continue;
        }

        //std::cout << "calo: " << calo << "   calo from id: " << unique_id / 10000 << "   channel: " << unique_id - (unique_id / 10000) << std::endl;
        std::pair<int, int> lookup_key {calo, tower_id};
        //std::pair<int, int> lookup_key {calo, static_cast<int>(unique_id - (unique_id / 10000))};
        if(m_towerInfo_map.find(lookup_key) != m_towerInfo_map.end())
        {
          cons.push_back(m_towerInfo_map[lookup_key]);
        }
      }
      JetInfo tmpJet;
      tmpJet.set_px(jet->get_px());
      tmpJet.set_py(jet->get_py());
      tmpJet.set_pz(jet->get_pz());
      tmpJet.set_e(jet->get_e());
      tmpJet.set_constituents(cons);
      m_jetInfo[r].push_back(tmpJet);
    }
  }

  //truth jets
  if(m_doSim)
  {
    auto range = truthParticles->GetPrimaryParticleRange();
    for(auto it=range.first; it!=range.second; ++it)
    {
      PHG4Particle* p = it->second;
      if(!p) continue;
      if(p->get_e() <= 0) continue;

      Tower tmpTower;
      tmpTower.set_px(p->get_px());
      tmpTower.set_py(p->get_py());
      tmpTower.set_pz(p->get_pz());
      tmpTower.set_e(p->get_e());
      tmpTower.set_calo(4);

      m_truthParticles.push_back(tmpTower);
      m_towerInfoTruth_map[std::make_pair(4, p->get_track_id())] = m_towerInfo.size() - 1;
    }


    // jet loop
    for(int r=0; r<4; r++)
    {
      for(auto jet : *truthJets[r])
      {
        if (jet->get_pt() < m_minJetPt || std::abs(jet->get_eta()) > 1.1 - jetR[r])
        {
          continue;
        }

        std::vector<int> cons;
        for(auto comp : jet->get_comp_vec())
        {
          int calo = -999;
        
          Jet::SRC source = comp.first;
          int tower_id = static_cast<int>(comp.second);
          if(source == Jet::SRC::HEPMC_IMPORT || source == Jet::SRC::PARTICLE || source == Jet::SRC::VOID)
          {
            calo = 4;
          }
        
          if(calo == -999)
          {
            continue;
          }
          std::pair<int, int> lookup_key {calo, tower_id};
          if(m_towerInfoTruth_map.find(lookup_key) != m_towerInfoTruth_map.end())
          {
            cons.push_back(m_towerInfoTruth_map[lookup_key]);
          }
        }
        JetInfo tmpJet;
        tmpJet.set_px(jet->get_px());
        tmpJet.set_py(jet->get_py());
        tmpJet.set_pz(jet->get_pz());
        tmpJet.set_e(jet->get_e());
        tmpJet.set_constituents(cons);
        m_truthJetInfo[r].push_back(tmpJet);
     }
    }

  }

  T->Fill();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int VandyJetDSTSkimmer::End(PHCompositeNode * /*topNode*/)
{

  std::cout << "Number of removed events no sim: " << nRemNoSim << std::endl;
  std::cout << "Number of removed events sim: " << nRemSim << std::endl;
  std::cout << "Number of removed events dT: " << nRem_dT << std::endl;


  T->Print();

  outfile->cd();
  T->Write();
  outfile->Close();
  
  return Fun4AllReturnCodes::EVENT_OK;
}


//____________________________________________________________________________..
std::pair<float, float> VandyJetDSTSkimmer::isGoodDijet()
{
  std::pair<float, float> pTs {-999, -999};

  if(!m_doSim && !m_cutParams.get_int_param("passDeltatCut"))
  {
    if(Verbosity())
    {
      std::cout << "VandyJetDSTSkimmer::process_event - delta t cut failed, bad event" << std::endl;
    }
    nRem_dT++;
    return pTs;
  }

  if(jets[2]->size() < 2)
  {
    return pTs;
  }

  Jet *leadJet = nullptr;
  Jet *subleadJet = nullptr;
  float lead_pT = 0.0;
  float sublead_pT = 0.0;
  for(int i=0; i<(int)jets[2]->size(); i++)
  {
    Jet *j = jets[2]->get_jet(i);
    float pT = j->get_pt();
    if(pT > lead_pT)
    {
      if(leadJet)
      {
        subleadJet = leadJet;
        sublead_pT = lead_pT;
      }
      leadJet = j;
      lead_pT = pT;
    }
  }

  if(!leadJet || !subleadJet)
  {
    return pTs;
  }

  if(lead_pT < sublead_pT)
  {
    return pTs;
  }

  if(lead_pT < 25.0 || sublead_pT < 8.4)
  {
    return pTs;
  }

  double posEtaCorr = correct_eta(0.7, 90.0);
  double negEtaCorr = correct_eta(-0.7, 90.0);

  double leadEta = leadJet->get_eta();
  double subleadEta = subleadJet->get_eta();

  if(leadEta > posEtaCorr || leadEta < negEtaCorr || subleadEta > posEtaCorr || subleadEta < negEtaCorr)
  {
    return pTs;
  }

  float dPhi = subleadJet->get_phi() - leadJet->get_phi();
  if(dPhi > M_PI) dPhi -= 2*M_PI;
  if(dPhi < -M_PI) dPhi += 2*M_PI;
  if(std::abs(dPhi) < 0.75*M_PI)
  {
    return pTs;
  }

  pTs.first = lead_pT;
  pTs.second = sublead_pT;

  return pTs;

}

//____________________________________________________________________________..
std::pair<float, float> VandyJetDSTSkimmer::isGoodTruthDijet()
{
  std::pair<float, float> pTs {-999, -999};

  if(truthJets[2]->size() < 2)
  {
    return pTs;
  }

  Jet *leadJet = nullptr;
  Jet *subleadJet = nullptr;
  float lead_pT = 0.0;
  float sublead_pT = 0.0;
  for(int i=0; i<(int)truthJets[2]->size(); i++)
  {
    Jet *j = truthJets[2]->get_jet(i);
    float pT = j->get_pt();
    if(pT > lead_pT)
    {
      if(leadJet)
      {
        subleadJet = leadJet;
        sublead_pT = lead_pT;
      }
      leadJet = j;
      lead_pT = pT;
    }
  }

  if(!leadJet || !subleadJet)
  {
    return pTs;
  }

  if(lead_pT < sublead_pT)
  {
    return pTs;
  }

  if(lead_pT < 5.0 || sublead_pT < 5.0)
  {
    return pTs;
  }

  if(std::abs(leadJet->get_eta()) > 0.7 || std::abs(subleadJet->get_eta()) > 0.7)
  {
    return pTs;
  }

  float dPhi = subleadJet->get_phi() - leadJet->get_phi();
  if(dPhi > M_PI) dPhi -= 2*M_PI;
  if(dPhi < -M_PI) dPhi += 2*M_PI;
  if(std::abs(dPhi) < 0.75*M_PI)
  {
    return pTs;
  }

  pTs.first = lead_pT;
  pTs.second = sublead_pT;

  return pTs;

}

float VandyJetDSTSkimmer::correct_eta(float eta, float r)
{
  float origZ = r * sinh(eta);
  float newEta = asinh((origZ - m_eventInfo->get_z_vtx())/r);
  if(std::isnan(newEta) || std::isinf(newEta))
  {
    return eta;
  }
  return newEta;

}

//____________________________________________________________________________..
fastjet::PseudoJet VandyJetDSTSkimmer::get_PseudoJet(double eta, double phi, double E)
{
  double pT = E/cosh(eta);
  
  double px = pT*cos(phi);
  double py = pT*sin(phi);
  double pz = pT*sinh(eta);

  fastjet::PseudoJet pj(px, py, pz, E);
  return pj;
}
