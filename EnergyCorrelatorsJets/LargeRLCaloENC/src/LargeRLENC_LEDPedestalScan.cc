#include "LargeRLENC_LEDPedestalScan.h"
// Just run a version of the LargeRL enc that only calls the processing and no cuts
LEDPedestalScan::LEDPedestalScan(const int n_run, const int n_segment, const std::string& name)
{
	encCalc=new LargeRLENC(n_run, n_segment, 1.0, false, true);
	this->run=n_run;
	this->segement=n_segment;
	jet_loc=new TH2F("jet_loc", "Fake Jet locations all jets; #eta; #varphi; #N_{hits}", 24, -1.1, 1.1, 64, 0, 2*PI); 
	jet_loc_center=new TH2F("jet_loc_center", "Fake Jet center all jets; #eta; #varphi; #N_{hits}", 24, -1.1, 1.1, 64, 0, 2*PI); 
	lead_jet_loc=new TH2F("lead_jet_loc", "Fake Jet locations leading jets; #eta; #varphi; #N_{hits}", 24, -1.1, 1.1, 64, 0, 2*PI); 
	lead_jet_loc_center=new TH2F("lead_jet_loc_center", "Fake Jet center leading jets; #eta; #varphi; #N_{hits}", 24, -1.1, 1.1, 64, 0, 2*PI); 
	this->energy_thresholds = encCalc->Thresholds;
	for(int i= 0; i<(int) energy_thresholds.size(); i++){	
		ohcal_energy.push_back(new TH2F(Form("ohcal_energy_%d_MeV",(int)(energy_thresholds[i][3]*1000)), Form("OHCAL energy, E_{tow} #geq %d MeV; #eta; #varphi; E [GeV]",(int)(energy_thresholds[i][3]*1000)), 24, -1.1, 1.1, 64, 0, 2*PI));
		ihcal_energy.push_back(new TH2F(Form("ihcal_energy_%d_MeV",(int)(energy_thresholds[i][2]*1000)), Form("IHCAL energy, E_{tow} #geq %d MeV; #eta; #varphi; E [GeV]",(int)(energy_thresholds[i][2]*1000)), 24, -1.1, 1.1, 64, 0, 2*PI));
		emcal_energy.push_back(new TH2F(Form("emcal_energy_%d_MeV",(int)(energy_thresholds[i][1]*1000)), Form("EMCAL energy, E_{tow} #geq %d MeV; #eta; #varphi; E [GeV]",(int)(energy_thresholds[i][1]*1000)), 96, -1.1, 1.1, 256, 0, 2*PI));
		ohcal_energy_node.push_back(new TH2F(Form("ohcal_energy_node_%d_MeV",(int)(energy_thresholds[i][3]*1000)), Form("OHCAL energy, E_{tow} #geq %d MeV; #eta; #varphi; E [GeV]",(int)(energy_thresholds[i][3]*1000)), 24, -1.1, 1.1, 64, 0, 2*PI));
		ihcal_energy_node.push_back(new TH2F(Form("ihcal_energy_node_%d_MeV",(int)(energy_thresholds[i][2]*1000)), Form("IHCAL energy, E_{tow} #geq %d MeV; #eta; #varphi; E [GeV]", (int)(energy_thresholds[i][2]*1000)),24, -1.1, 1.1, 64, 0, 2*PI));
		emcal_energy_node.push_back(new TH2F(Form("emcal_energy_node_%d_MeV",(int)( energy_thresholds[i][1]*1000)), Form("EMCAL energy, E_{tow} #geq %d MeV; #eta; #varphi; E [GeV]", (int)(energy_thresholds[i][1]*1000)), 96, -1.1, 1.1, 256, 0, 2*PI));
	}
	n_evts=0;
}
int LEDPedestalScan::process_event(PHCompositeNode* topNode)
{
	n_evts++;
	std::cout<<"Running Event " <<n_evts<<std::endl;
	std::array<float, 3> vertex {0., 0., 0.};
	std::map<std::array<float, 3>, float> emcal_towers, ihcal_towers, ohcal_towers;
	std::string ohcal_energy_towers="TOWERINFO_CALIB_HCALOUT", ihcal_energy_towers="TOWERINFO_CALIB_HCALIN", emcal_energy_towers="TOWERINFO_CALIB_CEMC";
	auto emcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_CEMC"   );
	auto emcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, emcal_energy_towers      );
	auto ihcal_geom= findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN");
	auto ihcal_tower_energy= findNode::getClass<TowerInfoContainer>(topNode, ihcal_energy_towers     );
	auto ohcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");
	auto ohcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, ohcal_energy_towers      );
	if(n_evts==1) encCalc->MakeEMCALRetowerMap(emcal_geom,emcal_tower_energy, ohcal_geom, ohcal_tower_energy);
	for(int n=0; n<(int) emcal_tower_energy->size(); n++){
		emcal_geom->set_calorimeter_id(RawTowerDefs::CEMC);
		auto key=emcal_tower_energy->encode_key(n);
		auto tower=emcal_tower_energy->get_tower_at_channel(n);
		int phibin=emcal_tower_energy->getTowerPhiBin(key);
		int etabin=emcal_tower_energy->getTowerEtaBin(key);
		float phicenter=emcal_geom->get_phicenter(phibin);
		float etacenter=emcal_geom->get_etacenter(etabin);
		float te=tower->get_energy();
		for(int i=0; i<(int)energy_thresholds.size(); i++){
			if(te > energy_thresholds[i][1]) 
				emcal_energy_node[i]->Fill(etacenter, phicenter, te);
			}
		encCalc->addTower(n, emcal_tower_energy, emcal_geom, &emcal_towers, RawTowerDefs::CEMC);
	}
	for(int n=0; n<(int) ihcal_tower_energy->size(); n++){
		ihcal_geom->set_calorimeter_id(RawTowerDefs::HCALIN);
		auto key=ihcal_tower_energy->encode_key(n);
		auto tower=ihcal_tower_energy->get_tower_at_channel(n);
		int phibin=ihcal_tower_energy->getTowerPhiBin(key);
		int etabin=ihcal_tower_energy->getTowerEtaBin(key);
		float phicenter=ihcal_geom->get_phicenter(phibin);
		float etacenter=ihcal_geom->get_etacenter(etabin);
		for(int i=0; i<(int)ihcal_energy_node.size(); i++)
			if(tower->get_energy() > energy_thresholds[i][2]) 
				ihcal_energy_node[i]->Fill(etacenter, phicenter, tower->get_energy());
		encCalc->addTower(n, ihcal_tower_energy, ihcal_geom, &ihcal_towers, RawTowerDefs::HCALIN);
	}
	for(int n=0; n<(int) ohcal_tower_energy->size(); n++){
		ohcal_geom->set_calorimeter_id(RawTowerDefs::HCALOUT);
		auto key=ohcal_tower_energy->encode_key(n);
		auto tower=ohcal_tower_energy->get_tower_at_channel(n);
		int phibin=ohcal_tower_energy->getTowerPhiBin(key);
		int etabin=ohcal_tower_energy->getTowerEtaBin(key);
		float phicenter=ohcal_geom->get_phicenter(phibin);
		float etacenter=ohcal_geom->get_etacenter(etabin);
		for(int i=0; i<(int)ohcal_energy_node.size(); i++)
			if(tower->get_energy() > energy_thresholds[i][3]) 
				ohcal_energy_node[i]->Fill(etacenter, phicenter, tower->get_energy());
		encCalc->addTower(n, ohcal_tower_energy, ohcal_geom, &ohcal_towers, RawTowerDefs::HCALOUT);
	}
	//try to build a jet like object
	std::map<std::array<float, 3>, float> total_array;
	for(auto e: emcal_towers)
	{
		for(int j=0; j<(int)emcal_energy.size(); j++)
			if(e.second > energy_thresholds[j][1]) 
				emcal_energy[j]->Fill(e.first[0], e.first[1], e.second);
		total_array[e.first]=e.second; //retower allready handeled
	}
	for(auto i: ihcal_towers)
	{
		for(int j=0; j<(int)ihcal_energy.size(); j++)
			if(i.second > energy_thresholds[j][2]) 
				ihcal_energy[j]->Fill(i.first[0], i.first[1], i.second);
		if(total_array.find(i.first) != total_array.end()) total_array[i.first]+=i.second;
		else total_array[i.first]=i.second;
	}
	for(auto o: ohcal_towers)
	{
		for(int j=0; j<(int)ohcal_energy.size(); j++)
			if(o.second > energy_thresholds[j][3]) 
				ohcal_energy[j]->Fill(o.first[0], o.first[1], o.second);
		if(total_array.find(o.first) != total_array.end()) total_array[o.first]+=o.second;
		else total_array[o.first]=o.second;
	}
	
	JetContainerv1* jets=getJets(0.4, vertex, total_array);
	float lead_phi=0., max_e=0.;
	if(jets->size() > 0)
	{
		std::vector<float> fake_ratio;
		for(int i=0; i<(int)jets->size(); i++)fake_ratio.push_back(0.5);
		encCalc->eventCut->passesTheCut(jets, fake_ratio, fake_ratio, 0.5, vertex, fake_ratio);
		lead_phi = encCalc->eventCut->getLeadPhi();
	}
	else{
		for(auto t: total_array)
		{
			if(t.second > max_e)
			{
				max_e = t.second;
				lead_phi = t.first.at(1);
			}
			else continue;
		}
	}	
	encCalc->CaloRegion(emcal_towers, ihcal_towers, ohcal_towers, 0.05, "E",  vertex, lead_phi);
	return Fun4AllReturnCodes::EVENT_OK;
}
JetContainerv1* LEDPedestalScan::getJets(float radius, std::array<float, 3> vertex, std::map<std::array<float, 3>, float> towers)
{
	JetContainerv1* fastjetCont=new JetContainerv1();
	std::vector<fastjet::PseudoJet> jet_objs;
	fastjet::JetDefinition fjd(fastjet::antikt_algorithm, radius);
	for(auto t:towers){
		float E=t.second;
		if(E < 0.03) continue; //min 10 MeV per tower per detector
		float pz=E*tanh(t.first.at(0));
		float pt=E/(float)cosh(t.first.at(0));
		float px=pt*cos(t.first.at(1));
		float py=pt*sin(t.first.at(1));
		jet_objs.push_back(fastjet::PseudoJet(px, py, pz, E));
	}
	fastjet::ClusterSequence cs(jet_objs, fjd);
	auto js=cs.inclusive_jets();
	bool is_max=true;
	for(auto j:js){
		auto jet=fastjetCont->add_jet();
		jet->set_px(j.px());
		jet->set_py(j.py());
		jet->set_pz(j.pz());
		jet->set_e(j.e());
		for(auto cmp:j.constituents())
		{
			jet->insert_comp(Jet::SRC::CEMC_TOWER, cmp.user_index());
			jet_loc->Fill(cmp.eta(), cmp.phi());
			if(is_max) lead_jet_loc->Fill(cmp.eta(), cmp.phi());
		}
		jet_loc_center->Fill(j.eta(), j.phi());
		if(is_max){
			lead_jet_loc_center->Fill(j.eta(), j.phi());
			is_max=false;
		}
	}
	return fastjetCont;
}
void LEDPedestalScan::Print(const std::string &what) const
{
	encCalc->Print(what);
	TFile* f1=new TFile(Form("LEDPedestal_scan_run_%d_segement_%d.root", run, segement), "RECREATE");
	jet_loc->Write();
	jet_loc_center->Write();
	lead_jet_loc->Write();
	lead_jet_loc_center->Write();
	for(int i= 0; i<(int)emcal_energy.size(); i++){
		emcal_energy[i]->Write();
		ihcal_energy[i]->Write();
		ohcal_energy[i]->Write();
		emcal_energy_node[i]->Write();
		ihcal_energy_node[i]->Write();
		ohcal_energy_node[i]->Write();
	}
	f1->Close();
	return;
}
	
