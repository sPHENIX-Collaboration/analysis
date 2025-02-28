//////////////////////////////////////////////////////////////////////////////////////////////////
//											  	//
//			Large R_L Calorimeter ENC						//
//												//
//			Author: Skadi Grossberndt						//
//			Depends on: Calorimeter Tower ENC 					//
//			First Commit date: 18 Oct 2024						//
//			Most recent Commit: 2 Dec 2024						//
//			version: v2.5 fix issue with method histograms, add bin width 		//
//												//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "LargeRLENC.h"

LargeRLENC::LargeRLENC(const int n_run/*=0*/, const int n_segment/*=0*/, const float jet_min_pT/*=0*/, const bool data/*=false*/, std::fstream* ofs/*=nullptr*/, const std::string vari/*="E"*/, const std::string& name/* = "LargeRLENC"*/) 
{

	n_evts=0;
	/*gInterpreter->GenerateDictionary("CaloTowers", "EEC_Branches.h");
	gInterpreter->GenerateDictionary("map<int, array<map<float, float>,3>>", "map");
	gInterpreter->GenerateDictionary("map< int, array<map<array<float, 3>, float>, 3>>", "map");
	gInterpreter->GenerateDictionary("map< string, array< map< std::pair< float, float >, float >, 3 > >", "map;string;utility");*/
	
	MethodHistograms* fc, *fe, *fi, *fo, *tc, *te, *ti, *to, *ac, *ae, *ai, *ao, *trc, *tre, *tri, *tro;
	//set bin widths to tower size
	fc=new MethodHistograms("Full_CAL", 3.9, 0.1); 
	fe=new MethodHistograms("Full_EMCAL", 3.9, 0.025);
	fi=new MethodHistograms("Full_IHCAL", 3.9, 0.1);
	fo=new MethodHistograms("Full_OHCAL", 3.9, 0.1); //Full OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~pi)
	
	tc=new MethodHistograms("Towards_Region_CAL", 3.9, 0.1);
	te=new MethodHistograms("Towards_Region_EMCAL", 3.9, 0.025);
	ti=new MethodHistograms("Towards_Region_IHCAL", 3.9, 0.1);
	to=new MethodHistograms("Towards_Region_OHCAL", 3.9, 0.1); //Towards_Region OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~2pi/3)

	ac=new MethodHistograms("Away_Region_CAL", 3.9, 0.1);
	ae=new MethodHistograms("Away_Region_EMCAL", 3.9, 0.025);
	ai=new MethodHistograms("Away_Region_IHCAL", 3.9, 0.1);
	ao=new MethodHistograms("Away_Region_OHCAL", 3.9, 0.1); //Away_Region OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~2pi/3)
	
	trc=new MethodHistograms("Transverse_Region_CAL", 3.9, 0.1);
	tre=new MethodHistograms("Transverse_Region_EMCAL", 3.9, 0.025);
	tri=new MethodHistograms("Transverse_Region_IHCAL", 3.9, 0.1);
	tro=new MethodHistograms("Transverse_Region_OHCAL", 3.9, 0.1); //Transverse_Region OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~pi)

	std::vector<MethodHistograms*> hf {fc, fe, fi, fo}, ht {tc, te, ti, to}, ha{ac, ae, ai, ao}, htr{trc, tre, tri, tro};
	this->FullCal=hf;
	this->TowardRegion=ht;
	this->AwayRegion=ha;
	this->TransverseRegion=htr;
	this->Region_vector.push_back(FullCal);
	this->Region_vector.push_back(TowardRegion);
	this->Region_vector.push_back(AwayRegion);
	this->Region_vector.push_back(TransverseRegion);

	std::cout<<"The Full Calo method are a vector of size " <<FullCal.size() <<std::endl;
	//while the Towards and away region arent't so extensive, haveing Rmax ~ 3, but close enough, transverse region is going to have some discontinuities
	this->isRealData=data; //check if the data is real data, if not run the truth as a cross check 
	this->nRun=n_run;
	this->nSegment=n_segment;
	this->jetMinpT=jet_min_pT;
	this->algo="Tower";
	this->radius="r04";
	this->eventCut=new DijetEventCuts();
	this->which_variable=vari;
	this->output_file_name="Large_RL_ENC_def_"+algo+"_dijets_anti_kT_"+radius+"-"+std::to_string(nRun)+"-"+std::to_string(nSegment)+".root";
	DijetQA=new TTree("DijetQA", "Dijet Event QA");
	DijetQA->Branch("N_jets", &m_Njets);
	DijetQA->Branch("x_j_L",  &m_xjl);
	DijetQA->Branch("A_jj_L", &m_Ajjl);
//	DijetQA->Branch("Jet_PT", &m_pt_dijet);
	DijetQA->Branch("Dijet_sets", &m_dijets, "dijet[3]/F"); 
	EEC=new TTree("EEC", "Energy Correlator");
	EEC->Branch("E_Total",   &m_etotal);
	EEC->Branch("E_CEMC",    &m_eemcal);
	EEC->Branch("E_IHCAL",   &m_eihcal);
	EEC->Branch("E_OHCAL",   &m_eohcal);
	EEC->Branch("vertex",    &m_vertex);
	EEC->Branch("vertex_x",  &m_vtx);
	EEC->Branch("vertex_y",  &m_vty);
	EEC->Branch("vertex_z",  &m_vtz);
	EEC->Branch("Region",    &m_region);
	EEC->Branch("Calo", 	 &m_calo);
	EEC->Branch("R_L", 	 &m_rl);
	EEC->Branch("R_min",     &m_rm);
	EEC->Branch("R_i", 	 &m_ri);
	EEC->Branch("2_pt",      &m_e2c/*"region/C:calo/C:r_l/F:e2c/F"*/);
	EEC->Branch("3_pt",      &m_e3c /*, "region/C:calo/C:r_l/F:e3c/F"*/);
	JetEvtObs=new TTree("JetEvtObs", "Tree for Event shape jet observable studies");
	JetEvtObs->Branch("emcal", &m_emcal, "emcal_tower[3]/F"/*, "region/C:calo/C:eta/F:phi/F:pt/F"*/); //need to fix like above, floatten it out by hand
	JetEvtObs->Branch("ihcal", &m_ihcal, "ihcal_tower[3]/F"/*, "region/C:calo/C:eta/F:phi/F:pt/F"*/); //need to fix like above, floatten it out by hand
	
	JetEvtObs->Branch("ohcal",  &m_ohcal, "ohcal_tower[3]/F"/*, "region/C:calo/C:eta/F:phi/F:pt/F"*/); //need to fix like above, floatten it out by hand
	JetEvtObs->Branch("vertex_x",  &m_vtx);
	JetEvtObs->Branch("vertex_y",  &m_vty);
	JetEvtObs->Branch("vertex_z",  &m_vtz);
	JetEvtObs->Branch("N_J", &m_Njets);
	JetEvtObs->Branch("eta_lead", &m_etalead);
	JetEvtObs->Branch("phi_lead", &m_philead);
	emcal_occup=new TH1F("emcal_occup", "Occupancy in the emcal in individual runs; Percent of Towers; N_{evts}", 100, -0.05, 99.5);
	ihcal_occup=new TH1F("ihcal_occup", "Occupancy in the ihcal in individual runs; Percent of Towers; N_{evts}", 100, -0.05, 99.5);
	ohcal_occup=new TH1F("ohcal_occup", "Occupancy in the ohcal in individual runs; Percent of Towers; N_{evts}", 100, -0.05, 99.5);
	ohcal_rat_h=new TH1F("ohcal_rat", "Ratio of energy in ohcal compared to total calorimeter; Ratio of OHCAL Energy; N_{evts}", 200, -0.5, 1.5);
	ohcal_rat_occup=new TH2F("ohcal_rat_occup", "Ratio of energy in ohcal and occupancy in the ohcal in individual runs; Energy deposited in OHCAL; Percent of Towers; N_{evts}", 150, 0.045, 0.995, 100, -0.05, 99.5);
	ohcal_bad_hits=new TH2F("ohcal_bad_hits", "#eta-#varphi energy deposition of \" Bad Hit\" events; #eta; #varphi; E [GeV]", 24, -1.1, 1.1, 64, -0.0001, 2*PI);
	bad_occ_em_oh=new TH2F("bad_occ_em_oh", "EMCAL to OHCAL tower deposition of \" Bad Hit\" events; Percent EMCAL towers; Percent OHCAL Towers; N_{evts}", 100, -0.5, 99.5, 100, -0.5, 99.5);
	bad_occ_em_h=new TH2F("em_allh_bad_hits", "Emcal_occ to Average hcal energy deposition of \" Bad Hit\" events;Percent EMCAL Towers; Average Percent HCAL towers; N_{evts}", 100, -0.5, 99.5, 100, -0.5, 99.5);
	MinpTComp=0.01; //10 MeV cut on tower/components
/*	if(nSegment >= 0 ){
		write_to_file=true;
		std::fstream test_file;
		test_file.open("/gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/LargeRLCaloENC/test.txt");
		this->text_out_file=ofs;
		std::cout<<"Trying to open text file" <<text_out_filename<<std::endl; 
		if(text_out_file->is_open() || test_file.is_open()) std::cout<<text_out_filename <<" File is open" <<std::endl;
		(*text_out_file)<<"Region, Calorimeter, tower eta, tower phi, tower p_T"<<std::endl;
	}
	else write_to_file=false;*/
	std::cout<<"Setup complete"<<std::endl;
}
JetContainer* LargeRLENC::getJets(std::string input, std::string radius, std::array<float, 3> vertex, float ohcal_rat, PHCompositeNode* topNode)
{
	//This is just running the Fastjet reco 
	JetContainerv1* fastjetCont=new JetContainerv1();
	std::vector<fastjet::PseudoJet> jet_objs;
	float radius_float=0.;
	std::string rs=""; //striped down string to convert to float
	for(auto c:radius)
		if(isdigit(c))
			rs+=c;
	radius_float=stof(rs);
	radius_float=radius_float*0.1; //put the value to the correct range
	fastjet::JetDefinition fjd (fastjet::antikt_algorithm,  radius_float);
	std::cout<<"Input style is " <<input <<std::endl;
	if(input=="towers" || input.find("ower") != std::string::npos){
//		TowerJetInput* ti=new TowerJetInput(Jet::HCALOUT_TOWER); //use the outer hcal to get the jets as that is the area I need anyway 
//		auto psjets=ti->get_input(topNode);
		std::vector<std::array<float, 4>> psjets;	
		auto ohcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");
		auto ohcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
		for(int n=0; n<(int) ohcal_tower_energy->size(); n++){
			if(! ohcal_tower_energy->get_tower_at_channel(n)->get_isGood()) continue;
			float energy=ohcal_tower_energy->get_tower_at_channel(n)->get_energy();
			if(energy > 0.01){ //10 MeV cutoff 
				ohcal_geom->set_calorimeter_id(RawTowerDefs::HCALOUT);
				auto key=ohcal_tower_energy->encode_key(n);
				int phibin=ohcal_tower_energy->getTowerPhiBin(key);
				int etabin=ohcal_tower_energy->getTowerEtaBin(key);
				float phicenter=ohcal_geom->get_phicenter(phibin);
				float etacenter=ohcal_geom->get_etacenter(etabin);
				energy=energy/ohcal_rat; //scale the energy to an approriate level to get ~total calo energy
				//use E=p for this as an approximation
				float px=energy*(1/(float)cosh(etacenter))*cos(phicenter);	
				float py=energy*(1/(float)cosh(etacenter))*sin(phicenter);
				float pz=sqrt(pow(energy,2) - pow(px,2) - pow(py,2));
				std::array<float, 4> lorentz_vect {px, py, pz, energy};
				psjets.push_back(lorentz_vect);
			}
		}
		std::cout<<"The tower jet input has n many objects " <<psjets.size() <<std::endl;
//		for(auto p:psjets) jet_objs.push_back(fastjet::PseudoJet(p->get_px(), p->get_py(), p->get_pz(), p->get_e()));
		for(auto p:psjets) jet_objs.push_back(fastjet::PseudoJet(p[0], p[1], p[2], p[3]));
	}
	else if(input.find("luster") != std::string::npos){
		ClusterJetInput* ci=new ClusterJetInput(Jet::HCALOUT_CLUSTER);
		auto psjets=ci->get_input(topNode);
		std::cout<<"The tower jet input has n many objects " <<psjets.size() <<std::endl;
		for(auto p:psjets) jet_objs.push_back(fastjet::PseudoJet(p->get_px(), p->get_py(), p->get_pz(), p->get_e()));
	}
	else{
		std::cout<<"Could not recognize input method" <<std::endl;
	}
	fastjet::ClusterSequence cs(jet_objs, fjd);	
	auto js=cs.inclusive_jets();
	std::cout<<"Fastjet Clusterizer found : " <<js.size() <<std::endl;
	for(auto j:js)
	{
		auto jet=fastjetCont->add_jet();
		jet->set_px(j.px());
		jet->set_py(j.py());
		jet->set_pz(j.pz());
		jet->set_e( j.e() );
	}
	return fastjetCont;
}	
void LargeRLENC::addTower(int n, TowerInfoContainer* energies, RawTowerGeomContainer_Cylinderv1* geom, std::map<std::array<float, 3>, float>* towers, RawTowerDefs::CalorimeterId td)
{
	if(!geom) return;
	geom->set_calorimeter_id(td);
//	std::cout<<"The map has size " <<towers->size() <<std::endl;
	auto key=energies->encode_key(n);
	auto tower=energies->get_tower_at_channel(n);
	int phibin=energies->getTowerPhiBin(key);
	int etabin=energies->getTowerEtaBin(key);
	float phicenter=geom->get_phicenter(phibin);
	float etacenter=geom->get_etacenter(etabin);
	float r=geom->get_radius();
//	std::cout<<"energy is " <<tower->get_energy()<<std::endl;
	std::array<float, 3> center {etacenter, phicenter, r};
	towers->insert(std::make_pair(center, tower->get_energy()));
	return;
}
int LargeRLENC::process_event(PHCompositeNode* topNode)
{
	//This is where I can be a bit clever with allowing for the parrelization for easier crosschecks
	n_evts++;
	std::cout<<"Running on event : " <<n_evts<<std::endl;
//	if(write_to_file) (*text_out_file)<<"\n \nEvent " <<n_evts <<std::endl;
	m_emcal.clear();
	m_ihcal.clear();
	m_ohcal.clear();
	std::cout<<"Cleared the vectors" <<std::endl;
	auto emcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_CEMC"   );
	auto emcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC"   );
	auto ihcal_geom= findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN");
	auto ihcal_tower_energy= findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
	auto ohcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");
	auto ohcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
	//look for the jet objects 
	////std::cout<<__LINE__<<std::endl;
	JetContainer* jets=NULL;
	bool foundJetConts=false, isDijet=false;
	try{
		if(!isRealData) jets = findNode::getClass<JetContainer>(topNode, "AntiKt_Truth_r04"); //look for the r_04 truth jets
		else{
			std::string recoJetName="AntiKt_"+algo+radius;
			jets = findNode::getClass<JetContainer>(topNode, recoJetName); //check for already reconstructed jets
		}
		foundJetConts=true;
	}
	catch(std::exception& e){ std::cout<<"Did not find a jet container object, will attempt to reconstruct the jets" <<std::endl;}
	//std::cout<<__LINE__<<std::endl;
	std::array<float, 3> vertex={0.,0.,0.}; //set the initial vertex to origin 
	try{
		GlobalVertexMap* vertexmap=findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
	//std::cout<<__LINE__<<std::endl;
		if(vertexmap){
			if(vertexmap->empty())
				std::cout<<"Empty Vertex Map. \n Setting vertex to origin" <<std::endl; 
			else{
	//std::cout<<__LINE__<<std::endl;
				GlobalVertex* gl_vtx=nullptr;
				for(auto vertex_iter:*vertexmap) gl_vtx=vertex_iter.second;
				if(gl_vtx){
	//std::cout<<__LINE__<<std::endl;
					vertex[0]=gl_vtx->get_x();
	//std::cout<<__LINE__<<std::endl;
					m_vtx=vertex[0];
	//std::cout<<__LINE__<<std::endl;
					vertex[1]=gl_vtx->get_y();
	//std::cout<<__LINE__<<std::endl;
					m_vty=vertex[1];
	//std::cout<<__LINE__<<std::endl;
					vertex[2]=gl_vtx->get_z();
	//std::cout<<__LINE__<<std::endl;
					m_vtz=vertex[2];
				}
			}
		}
	}
	catch(std::exception& e){std::cout<<"Could not find the vertex. \n Setting to origin" <<std::endl;}
	//std::cout<<__LINE__<<std::endl;
	//////////std::cout<<__LINE__<<std::endl;
	int emcal_oc=0; //allow for occupancy to be calculated seperate from the other bits
	float emcal_energy=0., ihcal_energy=0., ohcal_energy=0., total_energy=0., ohcal_rat=0.;
	std::map<std::array<float, 3>, float> ihcal_towers, emcal_towers, ohcal_towers; //these are just to collect the non-zero towers to decrease the processing time 
	for(int n=0; n<(int) emcal_tower_energy->size(); n++){
		if(! emcal_tower_energy->get_tower_at_channel(n)->get_isGood()) continue;
		float energy=emcal_tower_energy->get_tower_at_channel(n)->get_energy();
		if(energy > 0.1)//put zero supression into effect
			addTower(n, emcal_tower_energy, emcal_geom, &emcal_towers, RawTowerDefs::CEMC   );
		if(energy > 0.01) emcal_oc++;
		emcal_energy+=energy;
	}
	//std::cout<<__LINE__<<std::endl;
	for(int n=0; n<(int) ihcal_tower_energy->size(); n++){
		if(n >= (int)ihcal_tower_energy->size()) continue;
	//std::cout<<__LINE__<<std::endl;
		if(! ihcal_tower_energy->get_tower_at_channel(n)->get_isGood()) continue;
	//std::cout<<__LINE__<<std::endl;
		float energy=ihcal_tower_energy->get_tower_at_channel(n)->get_energy();
		if(energy > 0.01) //zero suppression is at 10 MeV in IHCAL
			addTower(n, ihcal_tower_energy, ihcal_geom, &ihcal_towers, RawTowerDefs::HCALIN  );
	//std::cout<<__LINE__<<std::endl;
		ihcal_energy+=energy;
	//std::cout<<__LINE__<<std::endl;
	}
	//std::cout<<__LINE__<<std::endl;
	for(int n=0; n<(int) ohcal_tower_energy->size(); n++){
		if(! ohcal_tower_energy->get_tower_at_channel(n)->get_isGood()) continue;
		float energy=ohcal_tower_energy->get_tower_at_channel(n)->get_energy();
		if(energy > 0.01) //10 MeV cutoff 
			addTower(n, ohcal_tower_energy, ohcal_geom, &ohcal_towers, RawTowerDefs::HCALOUT );
		ohcal_energy+=energy;
	}
	//////////std::cout<<__LINE__<<std::endl;
	total_energy=emcal_energy+ihcal_energy+ohcal_energy;
	ohcal_rat=ohcal_energy/(float)total_energy; //take the ratio at the whole calo as that is the region of interest
	float emcal_occupancy=emcal_oc/((float)emcal_tower_energy->size())*100.;
	float ihcal_occupancy=ihcal_towers.size()/((float)ihcal_tower_energy->size())*100.;
	float ohcal_occupancy=ohcal_towers.size()/((float)ohcal_tower_energy->size())*100.;
	emcal_occup->Fill(emcal_occupancy); //filling in the occupancy plots for easy QA while going 
	ihcal_occup->Fill(ihcal_occupancy);
	ohcal_occup->Fill(ohcal_occupancy);
	ohcal_rat_h->Fill(ohcal_rat);
	//////////std::cout<<__LINE__<<std::endl;
	if(jets==NULL || !foundJetConts) jets=getJets(algo, radius, vertex, ohcal_rat, topNode);
	if(!jets || jets->size() == 0 ){
		std::cout<<"Didn't find any jets, skipping event" <<std::endl;
		return Fun4AllReturnCodes::EVENT_OK;
	}
	if(ohcal_rat > 1 || ohcal_rat < 0 )std::cout<<"Strange values of hcal ratio ohcal energy " <<ohcal_energy << "\n ihcal energy : " <<ihcal_energy <<"\n emcal energy " <<emcal_energy <<std::endl;
	isDijet=eventCut->passesTheCut(jets, ohcal_rat, vertex);	
	//////////std::cout<<__LINE__<<std::endl;
	if(!isDijet){ //stores some data about the bad cuts to look for any arrising structure
	//////////std::cout<<__LINE__<<std::endl;
		ohcal_rat_occup->Fill(ohcal_rat, ohcal_occupancy);
		if(ohcal_rat > 0.9){
	//////////std::cout<<__LINE__<<std::endl;
		       for(auto p:ohcal_towers)ohcal_bad_hits->Fill(p.first[0], p.first[1], p.second);
		}
		bad_occ_em_oh->Fill(emcal_occupancy, ohcal_occupancy);
		bad_occ_em_h->Fill(emcal_occupancy, (ohcal_occupancy+ihcal_occupancy)/2.0);
	       	return Fun4AllReturnCodes::EVENT_OK;
	}
	else{
		//std::cout<<__LINE__<<std::endl;
		float lead_phi=eventCut->getLeadPhi();
	//////////std::cout<<__LINE__<<std::endl;
		eventCut->getDijets(jets, &m_dijets);
	//////////std::cout<<__LINE__<<std::endl;
		m_vertex=vertex;
		m_etotal=total_energy;
		m_eemcal=emcal_energy;
		m_eihcal=ihcal_energy;
		m_eohcal=ohcal_energy;
		m_philead=eventCut->getLeadPhi();
		m_etalead=eventCut->getLeadEta();
		//this is running the actual analysis
	//////////std::cout<<__LINE__<<std::endl;
	std::cout<<"The histovector has size " <<FullCal.size() <<std::endl;
		LargeRLENC::CaloRegion(emcal_towers, ihcal_towers, ohcal_towers, jetMinpT, which_variable, vertex, lead_phi);
	//////////std::cout<<__LINE__<<std::endl;
	//	LargeRLENC::CaloRegion(emcal_towers, ihcal_towers, ohcal_towers, 1, &TowardRegion,     jetMinpT, which_variable, vertex, lead_phi);
	//////////std::cout<<__LINE__<<std::endl;
	//	LargeRLENC::CaloRegion(emcal_towers, ihcal_towers, ohcal_towers, 2, &AwayRegion,       jetMinpT, which_variable, vertex, lead_phi);
	//////////std::cout<<__LINE__<<std::endl;
	//	LargeRLENC::CaloRegion(emcal_towers, ihcal_towers, ohcal_towers, 3, &TransverseRegion, jetMinpT, which_variable, vertex, lead_phi);
	//////////std::cout<<__LINE__<<std::endl;
	/*	for(auto pts:t_pt){
			m_calo_dijet=pts.first;
			for(auto pt_loc:pts.second){
				m_eta_dijet=pt_loc.first[0];
				m_phi_dijet=pt_loc.first[1];
				m_eta_dijet=pt_loc.first[2];
				m_pt=pt_log.second;	
			}
		}
	}*/
		DijetQA->Fill();
		//JetEvtObs->Fill();	
	}
	return Fun4AllReturnCodes::EVENT_OK;
}
void LargeRLENC::CaloRegion(std::map<std::array<float, 3>, float> emcal, std::map<std::array<float, 3>, float> ihcal, std::map<std::array<float, 3>, float> ohcal, /*int region, std::vector<MethodHistograms*> *Histos,*/ float jetMinpT, std::string variable_of_interest, std::array<float, 3> vertex, float lead_jet_center_phi)
{ 
	//this is the actual analysis object, the TTree for the event variables is actually just a dumy, no analysis, just save the variables into it for future usage
	bool transverse=false;
	bool energy=true;
	float phi_min=0., phi_max=2*PI;
	int v_o_i_code=0;
	std::map<int, std::pair<float, float>> region_ints;
//	std::cout<<"Emcal map size is " <<emcal.size() <<"\n ihcal is " <<ihcal.size() <<"\n ohcal is " <<ohcal.size() <<std::endl;
	if(variable_of_interest.find("T") != std::string::npos) v_o_i_code+=2;
	if(variable_of_interest.find("p") != std::string::npos) v_o_i_code+=1; 
	switch(v_o_i_code)
	{
		case 0:
			transverse=false;
			energy=true;
			break;
		case 1:
			transverse=false;
			energy=false;
			break;
		case 2:
			transverse=true;
			energy=true;
			break;
		case 3:
			transverse=true;
			energy=false;
			break;
	}
	for(int region=0; region< 5; region++)
	{
		switch(region)
		{
			case LargeRLENC::Regions::Full: //go over the full calorimeter
				phi_min=0;
				phi_max=2*PI;
				break;
			case LargeRLENC::Regions::Towards: //toward region of the detector
				phi_min=lead_jet_center_phi - PI/3.;
				phi_max=lead_jet_center_phi + PI/3.;
				break;
			case LargeRLENC::Regions::Away: //away region of the detector
				phi_min=lead_jet_center_phi + 2*PI/3.;
				phi_max=lead_jet_center_phi + 4*PI/3.;
				break;
			case LargeRLENC::Regions::TransverseMax: //transverse region1
				phi_min=lead_jet_center_phi + PI/3.;
				phi_max=lead_jet_center_phi + 2*PI/3.; 
				break;
			case LargeRLENC::Regions::TransverseMin: //transverse region 2 
				phi_min=lead_jet_center_phi - PI/3.;
				phi_max=lead_jet_center_phi - 2*PI/3.; 
		}
	//make sure the bounds are within the region of 0->2*PI
		if(phi_min < 0) phi_min+=2*PI;
		if(phi_max < 0) phi_max+=2*PI;
		if(phi_min > 2*PI) phi_min+=-2*PI;
		if(phi_max > 2*PI) phi_max+=-2*PI;
		float holding=std::max(phi_min, phi_max);
		float holding_2=std::min(phi_min, phi_max);
		phi_min=holding_2;
		phi_max=holding;		
		std::pair<float, float> phi_lim{phi_min, phi_max}; 
		region_ints[region]=phi_lim;
	}
	float total_e=0.;
	//////std::cout<<__LINE__<<std::endl;
	SingleCaloENC(emcal, jetMinpT, vertex, transverse, energy, region_ints, 1, &total_e);
	//the emcal has become a bit of a problem, I think I need to be more careful about it, potentially retower always?
	//std::cout<<__LINE__<<std::endl;
	SingleCaloENC(ihcal, jetMinpT, vertex, transverse, energy, region_ints, LargeRLENC::Calorimeter::IHCAL, &total_e);
	//std::cout<<__LINE__<<std::endl;
	SingleCaloENC(ohcal, jetMinpT, vertex, transverse, energy, region_ints, LargeRLENC::Calorimeter::OHCAL, &total_e);
	//std::cout<<__LINE__<<std::endl;
/*	if(write_to_file){
		for(auto t:m_pt_evt){
			int i=0;
			for(auto v:t.second){
				for(auto x:v){
					(*text_out_file)<<t.first<<", "<<i<<", "<<x.first.first<<", "<<x.first.second<<", "<<x.second<<std::endl;
				}
				i++;
			}
		}
	}*/
	return;
}
void LargeRLENC::SingleCaloENC(std::map<std::array<float, 3>, float> cal, float jetMinpT, std::array<float, 3> vertex, /*int region,*/ bool transverse, bool energy, std::map<int, std::pair<float, float>> phi_edges,/* std::vector<MethodHistograms*>* Histos,*/ int which_calo, float* total_e_region)
{
	MethodHistograms* ha=NULL, *hc=NULL;
	MethodHistograms* full_ha=FullCal.at(0), *full_hc=FullCal.at(which_calo);
	auto i=cal.begin();
	std::map<std::array<float, 3>, float> p_T_save;
	int n=0;
	std::array<float,4> e_region {0., 0., 0., 0.}; //get the energy in the relevant region 
	while(i !=cal.end()){
		n++;
		auto j=i->first;
		auto j_val=i->second;
		float pT=0., eta_shift=i->first[0], r=i->first[2];
		float x=r*cos(i->first[1]), y=r*sin(i->first[1]);
		if(vertex[0]!=0) x+=vertex[0];
		if(vertex[1]!=0) y+=vertex[1];
		r=sqrt(pow(x,2) + pow(y,2));
		float z=r*sinh(eta_shift);
		if(vertex[2]!=0)z+=vertex[2];
		eta_shift=asinh(z/r); //did the vertex shift
		if(eta_shift != eta_shift) std::cout<<"The eta shift returns nan, r="<<r <<" z=" <<z <<" ratio " <<z/r <<std::endl;
		j[0]=eta_shift;
		if(vertex[0] != 0 && vertex[1] !=0) j[1]=atan2(y,x);
		j[2]=r; //make sure the positions are properly aligned for all caluclations	
		i=cal.erase(i);
		int region_index=0;
		for(auto x:phi_edges){ //grab the correct region
			if(x.first==0) continue;
			if(j[1] < 0 ) j[1]=2*PI+j[1];
			if(j[1] > 2*PI) j[1]=j[1]-2*PI;
			if( j[1] < x.second.first || j[1] > x.second.second)continue;
			else{
				region_index=x.first;
				if(region_index >= 3) region_index=3;
				ha=Region_vector.at(region_index).at(0); 
				hc=Region_vector.at(region_index).at(which_calo);
//				if(region_index == 1 && which_calo == 3) std::cout<<"The identified region has a histogram named " <<ha->R->GetTitle() <<std::endl;
				break;
			}
		}
		if(!ha || !hc){
//			 std::cout<<"\n which failed??? have filled pt in towards??????\n Region index is " <<region_index <<std::endl;
			 continue;
		}
		else{
	//////std::cout<<__LINE__<<std::endl;
//			if( which_calo == 3) std::cout<<"\n should have filled pt in towards?????? \n Region histogram gives " << hc->pt->GetName() <<std::endl;
			cal[j]=j_val;
	////std::cout<<__LINE__<<std::endl;
			e_region[region_index]+=j_val;
			e_region[0]+=j_val;
//		       	++i;
	////std::cout<<__LINE__<<std::endl;
			pT=j_val; //right now using p approx E, can probably improve soon
	////std::cout<<__LINE__<<std::endl;
			pT=pT/cosh(eta_shift);
	////std::cout<<__LINE__<<std::endl;
			if(pT > MinpTComp) p_T_save[j]=pT;
	////std::cout<<__LINE__<<std::endl;
			hc->pt->Fill(pT);
			full_hc->pt->Fill(pT);
	////std::cout<<__LINE__<<std::endl;
			bool printout=false;
		//	if(n % 100000==0  && n_evts <=2 ) printout=true;
		//	else printout=false;
			float R=getR(j[0], j[1], eventCut->getLeadEta(), eventCut->getLeadPhi(), printout );
	////std::cout<<__LINE__<<std::endl;
			if(n_evts < 5 && printout) std::cout<<"R val is " <<R <<" \n with inputs " <<j[0] <<", " <<j[1] <<" , " <<eventCut->getLeadEta() <<" , " <<eventCut->getLeadPhi() <<std::endl;
			hc->R_pt->Fill(R,pT);
	////std::cout<<__LINE__<<std::endl;
			full_hc->R_pt->Fill(R,pT);
	////std::cout<<__LINE__<<std::endl;
		}
	}
	////std::cout<<__LINE__<<std::endl;
	for(int i=0; i<(int)e_region.size(); i++)
		if(e_region[i]==0.) 
			e_region[i]=1.; //just correcting for any nan inputs
	std::map<std::array<float, 3>, float> cal_2=cal;
	////std::cout<<__LINE__<<std::endl;
	std::map<int, std::map<float, float>> e2c, e3c;
	////std::cout<<__LINE__<<std::endl;
	std::map<int, std::map<std::array<float, 3>, float>> e3c_full;
	for(int i=0; i<4; i++){
		std::map<float, float> dummy {{0.,0.}};
		e2c[i]=dummy;
		e3c[i]=dummy;
		std::array<float, 3> dummy_array {0., 0., 0.};
		std::map<std::array<float, 3>, float> dummy_2{{dummy_array, 0.}}; 
		e3c_full[i]=dummy_2;
	} //just make sure the maps are initialized to be in working order ahead of the calculations
	////std::cout<<__LINE__<<std::endl;
	//hc->N->Fill((int)cal.size());
	full_hc->N->Fill((int)cal.size());
	//////std::cout<<__LINE__<<std::endl;
	//hc->E->Fill(e_region);
	full_hc->E->Fill(e_region[0]);
	////////std::cout<<__LINE__<<std::endl;
	(*total_e_region)+=e_region[0];
	//////std::cout<<__LINE__<<std::endl;
//	std::cout<<"Runing over " <<cal.size() <<" correlators options" <<std::endl;
	for(auto i=cal.begin(); i != cal.end(); ++i){
		std::array<float, 3> tower {i->first[0], i->first[1], i->second}; 
		if(which_calo == LargeRLENC::Calorimeter::EMCAL)m_emcal.push_back(tower);
		else if(which_calo == LargeRLENC::Calorimeter::IHCAL)m_ihcal.push_back(tower);
		else if(which_calo == LargeRLENC::Calorimeter::OHCAL)m_ohcal.push_back(tower);
		////////std::cout<<__LINE__<<std::endl;
		auto j=cal_2.find(i->first);
		if(j != cal_2.end()) 
			cal_2.erase(j); //getting rid of double counting by removing the base value from the second map 
		////////std::cout<<__LINE__<<std::endl;
		auto cal_3=cal_2;
		auto k=cal_3.find(j->first);
		if(k != cal_3.end()) 
			cal_3.erase(k); //getting rid of double counting by removing the base value from the third map 
		////////std::cout<<__LINE__<<std::endl;
		std::vector<std::pair<float, std::pair<float, float> > > point_correlator ((int)cal_2.size()); //allow for easy looping over the towers to parrelize
		////////std::cout<<__LINE__<<std::endl;
		std::vector< std::pair<float, std::vector< std::pair< std::pair<float, float>, float > > > > threept_full ((int) cal_2.size());
		////////std::cout<<__LINE__<<std::endl;
		std::vector<std::thread> calculating_threads; 
		int index=0;
		std::map<int, int> region_index; //gives a way to pull the relevant region after the fact to fill hc
		for(auto l:cal_2)
		{
		 	//threading over all pairs to allow for faster calculation 
			if(l.second < MinpTComp) continue;
			calculating_threads.push_back(std::thread(&LargeRLENC::CalculateENC, this, *i, l, cal_3, &point_correlator[index], &threept_full[index], transverse, energy)); 
			index++;
			for(auto x:phi_edges){
				int regions_val=x.first;
				if(regions_val >=3) regions_val = 3;
				if(regions_val==0) continue;
				if( l.first[1] < x.second.first || l.first[1] > x.second.second)continue;
				else{
					
					region_index[index]=regions_val;
				//	if(n_evts <= 7 ) std::cout<<"The selected region is " <<regions_val<<std::endl;
					break;
				}
				}
		}
//		std::cout<<"Need to run " <<calculating_threads.size() <<" computational threads" <<std::endl;
		for(int t=0; t<(int) calculating_threads.size(); t++) calculating_threads[t].join();
		index=0;
		for(auto v:point_correlator){
			auto l=e2c[region_index[index]].find(v.first);
			auto m=e3c[region_index[index]].find(v.first);
			if(l == e2c[region_index[index]].end()) e2c[region_index[index]][v.first]=v.second.first;
			if(l != e2c[region_index[index]].end()) e2c[region_index[index]][v.first]+=v.second.first;
			if(m == e3c[region_index[index]].end()) e2c[region_index[index]][v.first]=v.second.second;
			if(m != e3c[region_index[index]].end()) e2c[region_index[index]][v.first]+=v.second.second;
			l=e2c[0].find(v.first);
			m=e3c[0].find(v.first);
			if(l == e2c[0].end()) e2c[0][v.first]=v.second.first;
			if(l != e2c[0].end()) e2c[0][v.first]+=v.second.first;
			if(m == e3c[0].end()) e3c[0][v.first]=v.second.second;
			if(m != e3c[0].end()) e3c[0][v.first]+=v.second.second;
			Region_vector[region_index[index]][0]->R->Fill(v.first);
			Region_vector[region_index[index]][which_calo]->R->Fill(v.first);
			full_ha->R->Fill(v.first);
			full_hc->R->Fill(v.first);
			index++;
		}
		index=0;
		////////std::cout<<__LINE__<<std::endl;
		for(auto v:threept_full){
			for(auto w:v.second){
				std::array<float, 3> key= {v.first, w.first.first, w.first.second};
				auto l=e3c_full[region_index[index]].find(key);
				if(l == e3c_full[region_index[index]].end()) e3c_full[region_index[index]][key]=w.second;
				else e3c_full[region_index[index]][key]+=w.second;
				l=e3c_full[0].find(key);
				if(l == e3c_full[0].end()) e3c_full[0][key]=w.second;
				else e3c_full[0][key]+=w.second;
			}
		}
	}
		////////std::cout<<__LINE__<<std::endl;
	for(auto es:e2c){
		std::vector<std::pair<float, float>> holding_2;
		for(auto v:es.second){
			auto vs=v.second*pow(e_region[es.first], -2);
	        	Region_vector[es.first][which_calo]->E2C->Fill(v.first, vs);
			holding_2.push_back(std::make_pair(v.first, vs)); 
		}
		t_e2c.push_back(std::make_pair(es.first, holding_2));
	}
		////////std::cout<<__LINE__<<std::endl;
	for(auto es:e3c){
		std::vector<std::pair<float, float>> holding_3;
		for(auto v:es.second){
			auto vf=v.second*pow(e_region[es.first], -3);
			Region_vector[es.first][which_calo]->E3C->Fill(v.first, vf);
			holding_3.push_back(std::make_pair(v.first, vf));
			full_hc->E3C->Fill(v.first, vf);
		}
		t_e3c.push_back(std::make_pair(es.first, holding_3));
	}
		////////std::cout<<__LINE__<<std::endl;
	for(auto es:e3c_full)
	{
		std::vector< std::pair < std::array<float, 3>, float> > holding_3;
		for(auto v:es.second){
			auto vf=v.second*pow(e_region[es.first], -3);
			//float R_L=std::max(v.first[0], v.first[1]);
			//R_L=std::max(R_L, v.first[2]);
			holding_3.push_back(make_pair(v.first, vf));
			
//			Region_vector[es.first][which_calo]->E3C->Fill(R_L, vf);
//			full_hc->E3C->Fill(R_L, vf);
		}
		t_e3c_full.push_back(std::make_pair(es.first, holding_3));
	}
		////////std::cout<<__LINE__<<std::endl;
	/*for(auto e:e2c){
		 for(auto ei:e.second){
			t_e2c.push_back(e);
		////////std::cout<<__LINE__<<std::endl;
	for(auto e:e3c) t_e3c.push_back(e);
		////////std::cout<<__LINE__<<std::endl;
	for(auto e:e3c_full) t_e3c_full.push_back(e);
		////////std::cout<<__LINE__<<std::endl;
	t_pt[which_calo-1]=p_T_save;	*/
	for(int i=0; i<(int)t_e2c.size(); i++)
	{
		m_region=t_e2c[i].first;
		m_calo=which_calo; //ive made the class, maybe I will use eventually
		for(int j=0; j<(int)t_e2c[i].second.size(); j++){	
			m_rl=t_e2c[i].second[j].first;
			m_e2c=t_e2c[i].second[j].second;
			m_e3c=t_e3c[i].second[j].second;
			std::array<float, 3> hold_vals {0.,0.,0.};
			float mid=std::min(t_e3c_full[i].second[j].first[0],t_e3c_full[i].second[j].first[1]);
			float high=std::min(t_e3c_full[i].second[j].first[2],t_e3c_full[i].second[j].first[1]);
			if(mid==high){
				hold_vals[0]=mid;
				hold_vals[1]=std::min(t_e3c_full[i].second[j].first[0],t_e3c_full[i].second[j].first[2]); 
				hold_vals[2]=std::max(t_e3c_full[i].second[j].first[0],t_e3c_full[i].second[j].first[2]); 
			}
			else if(mid < high ) 
			{
				hold_vals[0]=mid;
				hold_vals[1]=high; 
				hold_vals[2]=std::max(t_e3c_full[i].second[j].first[1],t_e3c_full[i].second[j].first[2]); 
			}		
			else 
			{
				hold_vals[0]=high;
				hold_vals[1]=mid; 
				hold_vals[2]=std::max(t_e3c_full[i].second[j].first[0],t_e3c_full[i].second[j].first[1]); 
			}
			m_rm=hold_vals[0];
			m_ri=hold_vals[1];
		//	EEC->Fill();
		}
	}
			//JetEvtObs->Fill();
	return;
}
void LargeRLENC::CalculateENC(std::pair<std::array<float, 3>, float> point_1, std::pair<std::array<float, 3>, float> point_2, std::map<std::array<float, 3>, float> cal, std::pair<float, std::pair<float, float>>* enc_out, std::pair<float, std::vector< std::pair< std::pair<float, float>, float > > > *threept_full, bool transverse, bool energy)
{
	//this is the threaded object, I could try nesting the threads, but for now I am going to try to run without the nested threads for the 3pt
	float R_L=getR(point_1.first[0], point_1.first[1], point_2.first[0], point_2.first[1]);
	float e2c=0.;
	if(energy) e2c=(point_1.second) * (point_2.second);
	else e2c=(point_1.second * ptoE) * (point_2.second * ptoE); //use a basic swap of momenta ratio for each calo
	if(transverse) e2c=e2c/(cosh(point_1.first[0]) * cosh(point_2.first[0]));
	enc_out->first=R_L;
	enc_out->second.first=e2c;
	enc_out->second.second=0.;
	threept_full->first=R_L;
	for(auto i:cal)
	{
/*		float R_13=getR(point_1.first[0], point_1.first[1], i.first[0], i.first[1]);
		float R_23=getR(point_2.first[0], point_2.first[1], i.first[0], i.first[1]);
		float maxRL=std::max(R_L, R_13); //so I need to change the output sturcutre because I'm not sure if I am matching correctly
		maxRL=std::max(maxRL, R_23);
		if(maxRL > R_L) continue;
		else{ 
			float e3c=e2c*i.second;
			if(!energy) e3c=e3c*ptoE;
			if(transverse) e3c=e3c/(cosh(i.first[0]));
			enc_out->second.second+=e3c;
			std::pair<float, float> rs {R_13, R_23};
			std::pair<std::pair<float, float>, float> ec {rs, e3c};
			threept_full->second.push_back(ec);
		}
		*/
		if(i.second != 0 ) enc_out->second.second+=e2c*i.second;
		std::pair<float, float> rs {0,0};
		std::pair<std::pair<float, float>, float> ec{rs, 0};
		threept_full->second.push_back(ec);
	} //currently just fill 0 and worry about it later...need to deal with speed/memory concerns
	return;
}
void LargeRLENC::JetEventObservablesBuilding(std::array<float, 3> central_tower, std::map<std::array<float, 3>, float> calorimeter_input, std::map<float, float>* Etir_output )
{
	//This is the Jet Event observable. Ideally I would seperate this out 
	//Input is of the form <tower r, tower eta, tower phi>, tower E (vertex corrected)
	//Output is <tower r, tower eta, tower phi>, <R=0.1-1.0, ETir> 

	for(int r=0; r<10; r++)
	{
		float Rmax=(r+1)/10.;
		(*Etir_output)[Rmax]=0.;
	}
	(*Etir_output)[-1.0]=0; //full calorimeter holding 
	for(auto tower_energy:calorimeter_input){
		if(tower_energy.first==central_tower) continue; //don't double count
		float RVal=getR(tower_energy.first[1], tower_energy.first[2], central_tower[1], central_tower[2]);
		for(auto et=Etir_output->begin(); et != Etir_output->end(); ++et)
		{
			if(et->first > RVal)et->second+=tower_energy.second/((float) cosh(tower_energy.first[1]));
		}
	}
	return; 
}
float LargeRLENC::getR(float eta1, float phi1, float eta2, float phi2, bool print)
{
	float deta=eta1-eta2; 
	float dphi=abs(phi1-phi2);
	if(abs(dphi) > PI ) dphi+=-PI;
	float rsq=pow(deta, 2)+ pow(dphi, 2);
	if(print)std::cout<<" The value for R squared is square of "<<dphi <<" + square of " <<deta <<" = "<<rsq <<std::endl;;
	return sqrt(rsq);
}
void LargeRLENC::Print(const std::string &what) const
{
	TFile* f1=new TFile(output_file_name.c_str(), "RECREATE");
	DijetQA->Write();
	EEC->Write();
	JetEvtObs->Write();
	emcal_occup->Write();
	ihcal_occup->Write();
	ohcal_occup->Write();
	ohcal_rat_h->Write();
	ohcal_rat_occup->Write();
	ohcal_bad_hits->Write();
	bad_occ_em_oh->Write();
	bad_occ_em_h->Write();
	eventCut->JetCuts->Write();
	for(auto hv:FullCal){
		for( auto h:hv->histsvector) h->Write();
		hv->R_pt->Write();
	}
	for(auto hv:TransverseRegion){
		for( auto h:hv->histsvector) h->Write();	
		hv->R_pt->Write();
	}
	for(auto hv:AwayRegion){
		for( auto h:hv->histsvector) h->Write();
		hv->R_pt->Write();
	}
	for(auto hv:TowardRegion){	
		for( auto h:hv->histsvector) h->Write();
		hv->R_pt->Write();
	}
	f1->Write();
	f1->Close();
	return;	
}
