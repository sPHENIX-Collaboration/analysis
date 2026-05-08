
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

  for(int i=0; i<5; i++)
  {
    jets[i] = findNode::getClass<JetContainer>(topNode, std::format("AntiKt_r{}{}",jetRStr[i],(m_doCalib ? "_calib" : "")).c_str());
    if(m_doCalib) jetsUncalib[i] = findNode::getClass<JetContainer>(topNode, std::format("AntiKt_r{}",jetRStr[i]).c_str());

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

    for(int i=0; i<5; i++)
    {
      truthJets[i] = findNode::getClass<JetContainer>(topNode, std::format("AntiKt_Truth_r{}",jetRStr[i]).c_str());
      if (!truthJets[i])
      {
        std::cout << "VandyJetDSTSkimmer::Init - Error - Can't find Truth Jet Node " << std::format("AntiKt_Truth_r{}",jetRStr[i]).c_str() << " therefore no selection can be made" << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
      }
    }
    std::cout<<m_sampleName<<std::endl;
    for(int s=0; s<8; s++)
    {
      if(m_sampleName == sampleNames[s])
      {
        sampleNumber = s;
      }
      else if(m_sampleName.find("Herwig") != std::string::npos)
      {
	 if(m_sampleName == HerwigsampleNames[s])
	      {
		sampleNumber = s;
	      }
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
  for(int i=0; i<5; i++)
  {
    T->Branch(std::format("JetInfo_r{}",jetRStr[i]).c_str(),&m_jetInfo[i]);
  }
  if(m_doSim)
  {
    T->Branch("TruthParticles",&m_truthParticles);
    T->Branch("TruthTowers", &m_truthtowers);

    for(int i=0; i<5; i++)
    {
      T->Branch(std::format("TruthJetInfo_r{}",jetRStr[i]).c_str(),&m_truthJetInfo[i]);
      T->Branch(std::format("TruthTowerJetInfo_r{}",jetRStr[i]).c_str(),&m_truthtowerJetInfo[i]);
      
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
void VandyJetDSTSkimmer::getTruthTowers()
{
	//tower the truth particles into IHCAL tower sizes
	int nTowers = m_towerInfoContainiers[2]->size();
	std::map<int, std::pair<float, float>> etabinedges {};
	std::map<int, std::pair<float, float>> phibinedges {};
	std::map<int, float> etacenters {};
	std::map<int, float> phicenters {};
	float r = 0;
	for(int i=0; i<nTowers; i++)
	{
		auto key = towerInfoContainers[2]->encode_key(i);
		auto tower = towerInfoContainers[2]->get_tower_at_channel(i);
		int etaBin = towerInfoContainers[2]->getTowerEtaBin(key);
		int phiBin = towerInfoContainers[2]->getTowerPhiBin(key);
		if(etabinedges.find(etaBin) == etabinedges.end()){
		       	std::pair<float, float> etaEdges = geoms[2]->get_etaedges(etaBin);
			float etaCenter = geoms[2]->get_etacenter(etaBin);
			etabinedges[etaBin] = etaEdges;
			etacenters[etaBin]= etaCenter;
		}
		if(phibinedges.find(phiBin) == phibinedges.end()){
			std::pair<float, float> phiEdges = geoms[2]->get_phiedges(phiBin);
			float phiCenter = geoms[2]->getphicenter(phiBin)
			phibinedges[phiBin] = phiEdges;
			phicenters[phiBin] = phiCetner;
		}
		if(i==0) r = geoms[2]->get_radius();
	}
	std::map<std::pair<int, int>, Tower*> tt {};
	for(auto p:m_truthParticles)
	{
		float phi = std::atan2(p->py(), p->px());
		float eta = std::asinh(p->pz() / p->e());
		int phibin = -1;
		int etabin = -1;
		for(auto pb:phibinedges)
		{
			if(phi < pb.second.second && phi > pb.second.first){
			       	phibin = pb.first;
				break;
			}
		}
		for(auto eb:etabinedges)
		{
			if(eta < eb.second.second && eta > eb.second.first){
				etabin = eb.first;
				break;
			}
		}
		std::pair<int, int> bins {phibin, etabin}; 
		if(tt.find(bins) == tt.end()) tt[bins] = p;
		else{
			tt[bins]->set_e(tt[bins]->e() + p->e());
			tt[bins]->set_px(tt[bins]->px() + p->px());
			tt[bins]->set_px(tt[bins]->py() + p->py());
			tt[bins]->set_px(tt[bins]->pz() + p->pz());
		}	
	}
	for(auto t:tt) m_truthtowers->push_back(t.second);
	return;
}
void VandyJetDSTSkimmer::maketruthtowerJets()
{
	//use the truth towers to build jets
	std::vector<fastjet::PsuedoJet> jet02 {}; 
	fastjet::JetDefinition fj02 (fastjet::antikt_algorithm, 0.2);
	std::vector<fastjet::PsuedoJet> jet03 {}; 
	fastjet::JetDefinition fj03 (fastjet::antikt_algorithm, 0.3);
	std::vector<fastjet::PsuedoJet> jet04 {}; 
	fastjet::JetDefinition fj04 (fastjet::antikt_algorithm, 0.4);
	std::vector<fastjet::PsuedoJet> jet05 {}; 
	fastjet::JetDefinition fj05 (fastjet::antikt_algorithm, 0.5);
	std::vector<fastjet::PsuedoJet> jet06 {}; 
	fastjet::JetDefinition fj06 (fastjet::antikt_algorithm, 0.6);

	std::vector<std::vector<fastJet::PseudoJet>> jets {jet02, jet03, jet04, jet05, jet06};
	std::vector<fastJet::JetDefintion> jetdefs {fj02, fj03, fj04, fj05, fj06};

	std::vector<fastjet::PsuedoJet> truthinput {};
	for(auto p:m_truthtowers) truthinput.push_back(fastjet::PseudoJet(p->px(), p->py(), p->pz(), p->e()));
	for(int i=0; i<(int)jets.size(); i++)
	{
		fastjet::CluserSequence cs(truthinput, jetdefs[i]);
		jets[i]=cs.inclusivejets();
	}
	for(int i=0; i<(int)m_truthtowerJetInfo->size(); i++)
	{
		std::vector<JetInfo> jts {};
		for(auto p:jets[i])
		{
			JetInfo jt {};
			jt.set_px(p->get_px());
			jt.set_py(p->get_py());
			jt.set_pz(p->get_pz());
			jt.set_e(p->get_e());
		}
	}
	return;

}
//____________________________________________________________________________..
int VandyJetDSTSkimmer::process_event(PHCompositeNode *topNode)
{


  num++;
 // if(Verbosity())
 // {
     std::cout << "working on event " << num << "   number of removed events so far: " << nRem << std::endl;
 // }


  m_towerInfo.clear();
  m_truthParticles.clear();
  m_truthtowers.clear();
  m_towerInfo_map.clear();
  m_towerInfo_map2.clear();
  m_towerInfoTruth_map.clear();
  for(int i=0; i<5; i++)
  {
    m_jetInfo[i].clear();
    m_truthJetInfo[i].clear();
    m_truthtowerJetInfo[i].clear();
  }
  m_topoclusters.clear();

  if(vtxMap->empty())
  {
    if(Verbosity())  std::cout << "no vertex found" << std::endl;
    nRem++;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  std::vector<GlobalVertex*> vertices = vtxMap->get_gvtxs_with_type(vtxTypes);
  if(vertices.empty() || !vertices.at(0))
  {
    if(Verbosity()) std::cout << "no MBD vertex found" << std::endl;
    nRem++;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  m_vtx_z = vertices.at(0)->get_z();
  if (std::abs(m_vtx_z) > m_vtx_cut)
  {
    if(Verbosity()) std::cout << "reco vertex not in range \n vertex is " <<m_vtx_z<<" cm off of nominal 0"  << std::endl;
    nRem++;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  //set event info
  m_eventInfo->set_z_vtx(m_vtx_z);
  m_eventInfo->set_ZDC_rate(m_ZDC_coincidence);

  if(m_doSim)
  {
    //get leading truth pT and skip events where it is outside the range for each sample for ALL jet R
    bool goodTruthLeadJet[5] = {false, false, false, false, false};
    for(int r=0; r<5; r++)
    {
      float lead_pT = -999;
      for(auto jet : *truthJets[r])
      {
        double pT = jet->get_pt();
        if(pT > lead_pT) lead_pT = pT;
      }
      if(lead_pT >= truthJetR_pTMin[r][sampleNumber] && lead_pT < truthJetR_pTMin[r][sampleNumber+1]) goodTruthLeadJet[r] = true;
    }
    if(!goodTruthLeadJet[0] && !goodTruthLeadJet[1] && !goodTruthLeadJet[2] && !goodTruthLeadJet[3] && !goodTruthLeadJet[4])
    {
      if(Verbosity())
      {
        std::cout << "no leading truth jet for this sample in any jet radius" << std::endl;
      }
      nRem++;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
    
    m_eventInfo->set_cross_section(cs[sampleNumber]);
    if(m_sampleName.find("Herwig") != std::string::npos) m_eventInfo->set_cross_section(Herwigcs[sampleNumber]);
    int truthVtxIndex = truthParticles->GetPrimaryVertexIndex();
    if(truthParticles->GetPrimaryVtx(truthVtxIndex))
    {
      m_vtx_z_truth = truthParticles->GetPrimaryVtx(truthVtxIndex)->get_z();
    }
    m_eventInfo->set_z_vtx_truth(m_vtx_z_truth);

    for(int r=0; r<5; r++)
    {
      if(goodTruthLeadJet[r])
      {
        std::pair<float,float> dijetTruth = isGoodTruthDijet(r, topNode);
        m_eventInfo->set_dijetTruth_event(r, (dijetTruth.first >= 5.0 && dijetTruth.second >= 5.0 ? true : false));
        m_eventInfo->set_leadTruth_pT(r, dijetTruth.first);
        m_eventInfo->set_subleadTruth_pT(r, dijetTruth.second);
      }
      else
      {
        m_eventInfo->set_dijetTruth_event(r, false);
        m_eventInfo->set_leadTruth_pT(r, -999);
        m_eventInfo->set_subleadTruth_pT(r, -999);
      }
    }

    //Particles
    auto range = truthParticles->GetSPHENIXPrimaryParticleRange();
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
      m_towerInfoTruth_map[std::make_pair(4, p->get_track_id())] = m_truthParticles.size() - 1;
    }

    for(int r=0; r<5; r++)
    {
      for(auto jet : *truthJets[r])
      {
        if (jet->get_pt() < m_minJetPt)
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
	tmpJet.set_pt(jet->get_pt());
        tmpJet.set_pt_uncalib(jet->get_pt());
        tmpJet.set_hCaloFrac(getHCalFracTruth(jet, topNode));
        tmpJet.set_constituents(cons);
//	getJetParentParton(jet, &tmpJet, topNode);	
        m_truthJetInfo[r].push_back(tmpJet);
     }
    }
  }//end of all truth stuff
  else
  {
    m_eventInfo->set_z_vtx_truth(-999);
    for(int r=0; r<4; r++)
    {
      m_eventInfo->set_dijetTruth_event(r,true);
      m_eventInfo->set_leadTruth_pT(r,-999);
      m_eventInfo->set_subleadTruth_pT(r,-999);
    }
  }

  if(Verbosity()) std::cout << "done with sim" << std::endl;

  PHNodeIterator itNode(topNode);
  PHCompositeNode* parNode = dynamic_cast<PHCompositeNode*>(itNode.findFirst("PHCompositeNode","PAR"));
  PdbParameterMap* flagNode;

  //no timing cuts on reco, only data
  if(!m_doSim)
  {
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
    else
    {
      std::cout << "No flagNode for bbfqa - abort run" << std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }

    m_eventInfo->set_leadJetTime(m_cutParams.get_double_param("maxJett"));
    m_eventInfo->set_subJetTime(m_cutParams.get_double_param("subJett"));
    m_eventInfo->set_MBDTime(m_cutParams.get_double_param("mbd_time"));

    m_eventInfo->set_leadJetTimePass((bool) m_cutParams.get_int_param("passLeadtCut"));
    m_eventInfo->set_leadJetMBDDeltatPass((bool) m_cutParams.get_int_param("passMbdDtCut"));
    m_eventInfo->set_dijetDeltatPass((bool) m_cutParams.get_int_param("passDeltatCut"));
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
  	}
  }
  else goodTrigger = true; //trigger emulator not functioning on sim
  
  bool goodTiming = goodTrigger;
  if(goodTrigger && !m_doSim)
  {
    //timing cut
    if(!m_cutParams.get_int_param("passLeadtCut"))
    {
      if(Verbosity())
      {
        std::cout << "VandyJetDSTSkimmer::process_event - leading jet time cut failed, bad event" << std::endl;
      }
      goodTiming = false;
    }
    if(!m_cutParams.get_int_param("passMbdDtCut"))
    {
      if(Verbosity())
      {
        std::cout << "VandyJetDSTSkimmer::process_event - MBD time cut failed, bad event" << std::endl;
      }
      goodTiming = false;
    }
  }

  //Flag event as good only if one (or more) jets above pT threshold for each radius is found
  //If good jet is found at one R, entire event is flagged as good
  bool goodJet = false;
  if(goodTiming)
  {
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
  }

  if(m_doSim && !goodJet)
  {
    for(int r=0; r<5; r++)
    {
      m_eventInfo->set_dijet_event(r, false);
      m_eventInfo->set_lead_pT(r, -999);
      m_eventInfo->set_sublead_pT(r, -999);
    }
    T->Fill();

    return Fun4AllReturnCodes::EVENT_OK;
  }

  if(!m_doSim && !goodJet)
  {
    nRem++;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  if(Verbosity()) std::cout << "good reco event" << std::endl;

  for(int r=0; r<5; r++)
  {
    std::pair<float, float> dijet = isGoodDijet(r);
    m_eventInfo->set_dijet_event(r, (dijet.first >= jetR_pTMin[r] && dijet.second >= 5.0 ? true : false));
    m_eventInfo->set_lead_pT(r, dijet.first);
    m_eventInfo->set_sublead_pT(r, dijet.second);
  }

  if(Verbosity()) std::cout << "done with reco dijet" << std::endl;

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

  if(Verbosity()) std::cout << "done with towers" << std::endl;


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

  if(Verbosity()) std::cout << "done with clusters" << std::endl;


 
  // jet loop
  for(int r=0; r<5; r++)
  {
    int i=0;
    for(auto jet : *jets[r])
    {
      double posEta = 1.1 - jetR[r];
      double posEtaCorr = correct_eta(posEta, 90.0);
      double negEta = -1.1 + jetR[r];
      double negEtaCorr = correct_eta(negEta, 90.0);
      Jet *jetUncalib = jetsUncalib[r]->get_jet(i);
      if(Verbosity())
      {
        std::cout << "i: " << i << std::endl;
        std::cout << "   calib pT: " << jet->get_pt() << "   eta: " << jet->get_eta() << "   phi: " << jet->get_phi() << std::endl;
        std::cout << "  uncalib pT: " << jetUncalib->get_pt() << "   eta: " << jetUncalib->get_eta() << "   phi: " << jetUncalib->get_phi() << "   nCons: " << jetUncalib->get_comp_vec().size() << std::endl;
      }

      if (jet->get_pt() < m_minJetPt || jet->get_eta() > posEtaCorr || jet->get_eta() < negEtaCorr)
      {
        i++;
        continue;
      }

      double HCalE = 0.0;
      double totalE = 0.0;

      std::vector<int> cons;
      for(auto comp : jetUncalib->get_comp_vec())
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
          i++;
	        continue;
        }

        std::pair<int, int> lookup_key {calo, tower_id};
        if(m_towerInfo_map.find(lookup_key) != m_towerInfo_map.end())
        {
          cons.push_back(m_towerInfo_map[lookup_key]);
          totalE += m_towerInfo[m_towerInfo_map[lookup_key]].e();
          if(calo == 2 || calo == 3)
          {
            HCalE += m_towerInfo[m_towerInfo_map[lookup_key]].e();
          }
        }
      }
      JetInfo tmpJet;
      tmpJet.set_px(jet->get_px());
      tmpJet.set_py(jet->get_py());
      tmpJet.set_pz(jet->get_pz());
      tmpJet.set_e(sqrt(pow(jet->get_px(),2) + pow(jet->get_py(),2) + pow(jet->get_pz(),2)));
      tmpJet.set_pt(jet->get_pt()); 
      if(m_doCalib) tmpJet.set_pt_uncalib(jetUncalib->get_pt());
      else tmpJet.set_pt_uncalib(jet->get_pt()); 
      tmpJet.set_hCaloFrac(HCalE / totalE); 	//need to look at the TF doc to do this properly 
				//is it a jet by jet quanity or just an event quantity?
      tmpJet.set_constituents(cons);
      m_jetInfo[r].push_back(tmpJet);
      i++;
    }
  }

  if(Verbosity()) std::cout << "done with jets" << std::endl;


  T->Fill();

  if(Verbosity()) std::cout << "Filled tree" << std::endl;


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int VandyJetDSTSkimmer::End(PHCompositeNode * /*topNode*/)
{


  T->Print();

  outfile->cd();
  T->Write();
  outfile->Close();
  
  return Fun4AllReturnCodes::EVENT_OK;
}




//____________________________________________________________________________..
std::pair<float, float> VandyJetDSTSkimmer::isGoodDijet(int jetR_index)
{
  std::pair<float, float> pTs {-999, -999};

  if(!m_doSim && !m_cutParams.get_int_param("passDeltatCut"))
  {
    if(Verbosity())
    {
      std::cout << "VandyJetDSTSkimmer::process_event - delta t cut failed, bad event" << std::endl;
    }
    return pTs;
  }

  if(jets[jetR_index]->size() < 2)
  {
    return pTs;
  }

  Jet *leadJet = nullptr;
  Jet *subleadJet = nullptr;
  float lead_pT = 0.0;
  float sublead_pT = 0.0;
  for(int i=0; i<(int)jets[jetR_index]->size(); i++)
  {
    Jet *j = jets[jetR_index]->get_jet(i);
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
    else if(pT > sublead_pT)
    {
      subleadJet = j;
      sublead_pT = pT;
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

  if(lead_pT < jetR_pTMin[jetR_index] || sublead_pT < 5.0)
  {
    return pTs;
  }

  double posEtaCorr = correct_eta(1.1-jetR[jetR_index], 90.0);
  double negEtaCorr = correct_eta(-1.1+jetR[jetR_index], 90.0);

  float leadEta = leadJet->get_eta();
  float subleadEta = subleadJet->get_eta();

  if(leadEta > posEtaCorr || leadEta < negEtaCorr || subleadEta > posEtaCorr || subleadEta < negEtaCorr)
  {
    return pTs;
  }

  float dPhi = subleadJet->get_phi() - leadJet->get_phi();
  if(dPhi > M_PI) dPhi -= 2*M_PI;
  if(dPhi < -M_PI) dPhi += 2*M_PI;
  m_eventInfo->set_dijetDeltaPhi(jetR_index, dPhi);
  if(std::abs(dPhi) < 0.75*M_PI)
  {
    return pTs;
  }

  pTs.first = lead_pT;
  pTs.second = sublead_pT;

  return pTs;

}
float VandyJetDSTSkimmer::getHCalFracTruth(Jet* jet, PHCompositeNode *topNode) 
{
	//select the particle ID and match to the detector
	float hadronic_energy=0., electromagnetic_energy=0.;
//	float jet_phi=jet->get_phi(), jet_eta=jet->get_eta();
//	float i_e=0.;
	try{
		findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
	}
	catch(std::exception& e){
		std::cout<<"Could not find G4TruthInfo node"<<std::endl;
		return 0.;
	}
	auto truth_particles_p=findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
	try{
		truth_particles_p->GetMap();
	}
	catch(std::exception& e){
		std::cout<<"Could not find particle map" <<std::endl;
		return 0.;
	}
	std::map<int, PHG4Particle*> truth_particles;
	for(const auto& a:truth_particles_p->GetMap())
	       	truth_particles[a.first]=a.second;
	for(auto& iter:jet->get_comp_vec()){
		Jet::SRC source=iter.first;
		if(source != Jet::SRC::PARTICLE && source != Jet::SRC::CHARGED_PARTICLE && source != Jet::SRC::HEPMC_IMPORT){
			//don't have a source particle so skip it
			continue;
		}
		else{
			unsigned int id=iter.second;
			if(truth_particles.find(id) == truth_particles.end()){
				continue;
			}
			else{
				PHG4Particle* particle = truth_particles.at(id);
				int pid=particle->get_pid();
				if(abs(pid) == 11 || pid== 22){ 
					//electrons, positrons and photons get put in the emcal
					electromagnetic_energy+=particle->get_e();
//					float particle_phi=std::atan2(particle->get_py(), particle->get_px());
//					float particle_eta=std::atanh(particle->get_pz()/particle->get_e());
				}
				else if(abs(pid) > 11 && abs(pid) <= 18){
					//don't count neutrinos, muons, tau
					hadronic_energy+=particle->get_e();
//					float particle_phi=std::atan2(particle->get_py(), particle->get_px());
//					float particle_eta=std::atanh(particle->get_pz()/particle->get_e());
				}
			}
		}
	}
	//assume that the hcal energy split is consistent with the whole detector energy split 
	float ohcal_ratio=hadronic_energy/(hadronic_energy+electromagnetic_energy);
	return ohcal_ratio;
	//float emcal_ratio=electromagnetic_energy/(hadronic_energy+electromagnetic_energy);
	
}
float VandyJetDSTSkimmer::getDeltatTruth(float lead_ratio, float subl_ratio)
{
	float lead_t = OHCALrat2t(lead_ratio);
	float subl_t = OHCALrat2t(subl_ratio);
	float delta_t = lead_t - subl_t;
	return delta_t; //closest approximation using the TF1 report 	
}
void VandyJetDSTSkimmer::getJetParentParton(Jet* jet, JetInfo* jetinfo, PHCompositeNode* topNode)
{
	//find the earliest common ancestor of all the partons in the jet 
	std::vector<std::vector<HepMC::GenParticle*>> parton_parents; 
	auto hepMCParticles=findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
	
	if(!truthParticles || !hepMCParticles) return;
	
	HepMC::GenEvent*  hepMCevent {nullptr}; 
	for(PHHepMCGenEventMap::ConstIter eventIter = hepMCParticles->begin(); eventIter != hepMCParticles->end(); ++eventIter)
	{
		auto hc=eventIter->second;
		if(!hc) continue;
		hepMCevent=hc->getEvent();
	}
	if(!hepMCevent) return;


	std::map<int, PHG4Particle*> truthParticlesMap;
	std::map<int, HepMC::GenParticle*> truthHepMCMap;
	auto range = truthParticles->GetSPHENIXPrimaryParticleRange();
        for(auto it=range.first; it!=range.second; ++it)
	{
		auto a = it->second; 
		if(!a) continue;
		truthParticlesMap[it->first]=a;
	}
	std::vector<HepMC::GenParticle*> jet_final_state {};
       	std::vector<PHG4Particle*> jetPHg4 {};	
	for(auto p:jet->get_comp_vec())
	{
		Jet::SRC source = p.first;
		if( source == Jet::SRC::PARTICLE || 
				source == Jet::SRC::CHARGED_PARTICLE || 
				source == Jet::SRC::HEPMC_IMPORT)
		{
			unsigned int id = p.second;
			if( truthParticlesMap.find(id) != truthParticlesMap.end() ) jetPHg4.push_back(truthParticlesMap.at(id));
		}
	}
	for(HepMC::GenEvent::particle_const_iterator iter=hepMCevent->particles_begin(); iter !=hepMCevent->particles_end(); ++iter){
		if(!(*iter)) continue;
		int bc = (*iter)->barcode();
		truthHepMCMap[bc] = *iter;
	}
	
	for(auto p:jetPHg4)
	{
		auto bc = p->get_barcode();
		if( truthHepMCMap.find(bc) != truthHepMCMap.end() ) jet_final_state.push_back(truthHepMCMap[bc]);
		else{
			//attepmpt to position match if barcode doesnt work
			for(auto hp:truthHepMCMap)
			{
				auto etaHP 	= hp.second->momentum().pseudoRapidity();
			 	auto phiHP	= hp.second->momentum().phi();
				auto etaPG	= std::atanh(p->get_pz()/(std::sqrt( std::pow(p->get_px(),2) +std::pow(p->get_py(),2) + std::pow(p->get_pz(),2))));
				auto phiPG	= std::atan2(p->get_py(), p->get_px());	
				if( etaHP == etaPG && phiHP == phiPG ) jet_final_state.push_back(hp.second);
			}
		}
	}
	std::cout<<"There are " <<jet->get_comp_vec().size() <<" particles in the jet final state" <<std::endl;
	for(auto p: jet_final_state)
	{
		std::vector<HepMC::GenParticle*> ancestors {};
		ancestors = getFinalStateAncestors(p, hepMCevent);
		parton_parents.push_back(ancestors);
	}
	HepMC::GenParticle* parent = findCommonAncestor(parton_parents);
	if(!parent){
	       std::cout<<"Bad parent" <<std::endl;
	       jetinfo->set_parentPID(-999);
	       jetinfo->set_isQuark(false);
	       jetinfo->set_parent("");
	       return;
	}
	int pid = std::abs(parent->pdg_id());
	jetinfo->set_parentPID(pid);
	if(pid < 9 ) 
	{
		jetinfo->set_isQuark(true);
		std::string quark_label {""};
		if(pid == 1) quark_label = "down";
		else if(pid == 2) quark_label = "up";
		else if(pid == 3) quark_label = "up";
		else if(pid == 4) quark_label = "up";
		else if(pid == 5) quark_label = "up";
		else if(pid == 6) quark_label = "up";
	}
	else if (pid == 9 || pid == 21) 
	{
		jetinfo->set_isQuark(false);
		jetinfo->set_parent("gluon");
	}

	return;
}
std::vector<HepMC::GenParticle*> VandyJetDSTSkimmer::getFinalStateAncestors(HepMC::GenParticle* gp, HepMC::GenEvent* ev) 
{
	//get all particles in the parent line of a final state 
	HepMC::GenVertex* prodvtx = gp->production_vertex();
	std::map<float, HepMC::GenParticle*> tempparents {};
	HepMC::GenVertex* sig_proc = ev->signal_process_vertex();	
	for(HepMC::GenVertex::particles_in_const_iterator it = prodvtx->particles_in_const_begin(); 
			it != prodvtx->particles_in_const_end(); 
			++it )
	{
		float time = prodvtx->position().t();
		while (tempparents.find(time) != tempparents.end() ) time+=(time+1)/1000.; //just make sure that there are unique keys
		tempparents[time]=*it;
	}
	for(auto t: tempparents)
	{
		auto pt = t.second;
		prodvtx = pt->production_vertex();
		if(!prodvtx || !sig_proc ) continue;
		if(prodvtx->barcode() == sig_proc->barcode()) continue; //do not include the beam particles
		for(auto it = prodvtx->particles_in_const_begin();
				it != prodvtx->particles_in_const_end();
				++it)
		{
			if(!((*it)->end_vertex())) continue; //don't grab final state particles by accident
			float time = prodvtx->position().t();
			while(tempparents.find(time) != tempparents.end()) time+=time/1000.;
			tempparents[time]=(*it);
		}

	}
	std::vector<HepMC::GenParticle*> parents {};
	//sort all the ancestors in order from latest to earliest
	for(auto t=tempparents.rbegin(); t != tempparents.rend(); ++t)
	{
		parents.push_back(t->second); //preserve ordering of the time 
	}
	return parents;
}
HepMC::GenParticle* VandyJetDSTSkimmer::findCommonAncestor( std::vector<std::vector<HepMC::GenParticle*> > Jettree)
{
	//find the latest common ancestor of all final state particles
	HepMC::GenParticle* parent {nullptr};
	bool isCommon 	= false; 
	bool foundCommon= false;
	auto j = Jettree.begin();
	for(auto p:*j)
	{
		isCommon=true;
		int i=0;
		while(isCommon)
		{
			for(auto j2=Jettree.begin()+1; j2 != Jettree.end(); ++j2)
			{
				i++;
				for(auto p2:*j2)
				{
					if( p->barcode() == p2->barcode() )
					{
						foundCommon = true;
						break;
					}
				}
				if( !foundCommon || i > 10  )
				{
					isCommon = false;
					break;
				}
			}
			if(!isCommon || i > 10 ) break;
		}
		if(isCommon){
			std::cout<<p->pdg_id()<<std::endl;
			parent = p;
			break;
		}
	}
	return parent;
}
//____________________________________________________________________________..
std::pair<float, float> VandyJetDSTSkimmer::isGoodTruthDijet(int jetR_index, PHCompositeNode *topNode)
{
  std::pair<float, float> pTs {-999, -999};

  if(truthJets[jetR_index]->size() < 2)
  {
    return pTs;
  }

  Jet *leadJet = nullptr;
  Jet *subleadJet = nullptr;
  float lead_pT = 0.0;
  float sublead_pT = 0.0;
  for(int i=0; i<(int)truthJets[jetR_index]->size(); i++)
  {
    Jet *j = truthJets[jetR_index]->get_jet(i);
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
    else if(pT > sublead_pT)
    {
      subleadJet = j;
      sublead_pT = pT;
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

  if(std::abs(leadJet->get_eta()) > 1.1-jetR[jetR_index] || std::abs(subleadJet->get_eta()) > 1.1-jetR[jetR_index])
  {
    return pTs;
  }

  float dPhi = subleadJet->get_phi() - leadJet->get_phi();
  if(dPhi > M_PI) dPhi -= 2*M_PI;
  if(dPhi < -M_PI) dPhi += 2*M_PI;
  m_eventInfo->set_dijetDeltaPhiTruth(jetR_index, dPhi);
  if(std::abs(dPhi) < 0.75*M_PI)
  {
    return pTs;
  }
  float deltaT = getHCalFracTruth(leadJet, topNode) - getHCalFracTruth(subleadJet, topNode);
  m_eventInfo->set_dijetDeltatTruth(jetR_index, deltaT);
  m_eventInfo->set_dijetDeltatTruth(jetR_index, std::abs(deltaT) < 5 ? true : false ); 
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

