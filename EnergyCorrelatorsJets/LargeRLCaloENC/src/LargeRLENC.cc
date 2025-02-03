//////////////////////////////////////////////////////////////////////////////////////////////////
//											  	//
//			Large R_L Calorimeter ENC						//
//												//
//			Author: Skadi Grossberndt						//
//			Depends on: Calorimeter Tower ENC 					//
//			First Commit date: 18 Oct 2024						//
//			Most recent Commit: 2 Feb 2025						//
//			version: v3.0 with Thresholds, unified binning and Jet IE  		//
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
	for(int i=0; i<20; i++){
		MethodHistograms* fc, *fe, *fi, *fo, *tc, *te, *ti, *to, *ac, *ae, *ai, *ao, *trc, *tre, *tri, *tro;
	//set bin widths to tower size
		float hcal_thresh= 10*(1+0.25*i), emcal_thresh=70*(1+0.25*i);
		std::string hcal_thresh_s="_"+std::to_string((int)hcal_thresh)+"_MeV_threshold";
		std::string emcal_thresh_s="_"+std::to_string((int)emcal_thresh)+"_MeV_threshold";
		fc=new MethodHistograms("Full_CAL"+hcal_thresh_s, 3.9, 0.1); 
		fe=new MethodHistograms("Full_EMCAL"+emcal_thresh_s, 3.9, 0.1);
		fi=new MethodHistograms("Full_IHCAL"+hcal_thresh_s, 3.9, 0.1);
		fo=new MethodHistograms("Full_OHCAL"+hcal_thresh_s, 3.9, 0.1); //Full OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~pi)
	
		tc=new MethodHistograms("Towards_Region_CAL"+hcal_thresh_s, 3.9, 0.1);
		te=new MethodHistograms("Towards_Region_EMCAL"+emcal_thresh_s, 3.9, 0.1);
		ti=new MethodHistograms("Towards_Region_IHCAL"+hcal_thresh_s, 3.9, 0.1);
		to=new MethodHistograms("Towards_Region_OHCAL"+hcal_thresh_s, 3.9, 0.1); //Towards_Region OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~2pi/3)

		ac=new MethodHistograms("Away_Region_CAL"+hcal_thresh_s, 3.9, 0.1);
		ae=new MethodHistograms("Away_Region_EMCAL"+emcal_thresh_s, 3.9, 0.1);
		ai=new MethodHistograms("Away_Region_IHCAL"+hcal_thresh_s, 3.9, 0.1);
		ao=new MethodHistograms("Away_Region_OHCAL"+hcal_thresh_s, 3.9, 0.1); //Away_Region OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~2pi/3)
	
		trc=new MethodHistograms("Transverse_Region_CAL"+hcal_thresh_s, 3.9, 0.1);
		tre=new MethodHistograms("Transverse_Region_EMCAL"+emcal_thresh_s, 3.9, 0.1);
		tri=new MethodHistograms("Transverse_Region_IHCAL"+hcal_thresh_s, 3.9, 0.1);
		tro=new MethodHistograms("Transverse_Region_OHCAL"+hcal_thresh_s, 3.9, 0.1); //Transverse_Region OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~pi)

		std::vector<MethodHistograms*> hf {fc, fe, fi, fo}, ht {tc, te, ti, to}, ha{ac, ae, ai, ao}, htr{trc, tre, tri, tro};
		std::vector<MethodHistograms*> FullCal=hf;
		std::vector<MethodHistograms*> TowardRegion=ht;
		std::vector<MethodHistograms*> AwayRegion=ha;
		std::vector<MethodHistograms*> TransverseRegion=htr;
		std::vector<std::vector<MethodHistograms*>> Region_vector_1;
		Region_vector_1.push_back(FullCal);
		Region_vector_1.push_back(TowardRegion);
		Region_vector_1.push_back(AwayRegion);
		Region_vector_1.push_back(TransverseRegion);
		this->Region_vector.push_back(Region_vector_1);
	}

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
	DijetQA->Branch("Dijet_sets", &m_dijets, "dijet[3]/F"); 
	EEC=new TTree("EEC", "Energy Correlator");
	EEC->Branch("E_Total",   &m_etotal);
	EEC->Branch("E_CEMC",    &m_eemcal);
	EEC->Branch("E_IHCAL",   &m_eihcal);
	EEC->Branch("E_OHCAL",   &m_eohcal);
	EEC->Branch("Region",    &m_region);
	EEC->Branch("Calo", 	 &m_calo);
	EEC->Branch("R_L", 	 &m_rl);
	EEC->Branch("R_min",     &m_rm);
	EEC->Branch("R_i", 	 &m_ri);
	EEC->Branch("3_pt",      &m_e3c /*, "region/C:calo/C:r_l/F:e3c/F"*/);
	emcal_occup=new TH1F("emcal_occup", "Occupancy in the emcal in individual runs; Percent of Towers; N_{evts}", 100, -0.05, 99.5);
	ihcal_occup=new TH1F("ihcal_occup", "Occupancy in the ihcal in individual runs; Percent of Towers; N_{evts}", 100, -0.05, 99.5);
	ohcal_occup=new TH1F("ohcal_occup", "Occupancy in the ohcal in individual runs; Percent of Towers; N_{evts}", 100, -0.05, 99.5);
	ohcal_rat_h=new TH1F("ohcal_rat", "Ratio of energy in ohcal compared to total calorimeter; Ratio of OHCAL Energy; N_{evts}", 200, -0.5, 1.5);
	ohcal_rat_occup=new TH2F("ohcal_rat_occup", "Ratio of energy in ohcal and occupancy in the ohcal in individual runs; Energy deposited in OHCAL; Percent of Towers; N_{evts}", 150, 0.045, 0.995, 100, -0.05, 99.5);
	ohcal_bad_hits=new TH2F("ohcal_bad_hits", "#eta-#varphi energy deposition of \" Bad Hit\" events; #eta; #varphi; E [GeV]", 24, -1.1, 1.1, 64, -0.0001, 2*PI);
	bad_occ_em_oh=new TH2F("bad_occ_em_oh", "EMCAL to OHCAL tower deposition of \" Bad Hit\" events; Percent EMCAL towers; Percent OHCAL Towers; N_{evts}", 100, -0.5, 99.5, 100, -0.5, 99.5);
	bad_occ_em_h=new TH2F("em_allh_bad_hits", "Emcal_occ to Average hcal energy deposition of \" Bad Hit\" events;Percent EMCAL Towers; Average Percent HCAL towers; N_{evts}", 100, -0.5, 99.5, 100, -0.5, 99.5);
	IE=new TH1F("IE", "Radial Energy distribution of jets; #sum_{Constit} E_{i} R^{2} /E_{j}; N_{jets}", 1000, -0.05, 1.45); 
	badIE=new TH1F("badIE", "Radial Energy distribution of jets in events that fail cuts; #sum_{Constit} E_{i} R^{2} /E_{j}; N_{jets}", 1000, -0.05, 1.45); 
	goodIE=new TH1F("goodIE", "Radial Energy distribution of jets that pass dijet cuts; #sum_{Constit} E_{i} R^{2} /E_{j}; N_{jets}", 1000, -0.05, 1.45); 
	
	E_IE=new TH2F("E_IE", "Radial Energy Distribution of jets as a function of jet energy; E [GeV]; #sum E_{i} R^{2}/E_{j}; N_{jets}", 100, -0.5, 99.5, 100, -0.05, 1.45);
	badE_IE=new TH2F("badE_IE", "Radial Energy Distribution of jets as a function of jet energy failing dijet cuts; E [GeV]; #sum E_{i} R^{2}/E_{j}; N_{jets}", 100, -0.5, 99.5, 100, -0.05, 1.45);
	goodE_IE=new TH2F("goodE_IE", "Radial Energy Distribution of jets as a function of jet energy passing dijet cuts; E [GeV]; #sum E_{i} R^{2}/E_{j}; N_{jets}", 100, -0.5, 99.5, 100, -0.05, 1.45);
	MinpTComp=0.01; //10 MeV cut on tower/components
	for(int ci=0; ci < (int) Et_miss_hists.size(); ci++){
		std::string Calo_name;
		auto EC=&Et_miss_hists[ci];
		if(ci == 0){
			Calo_name="EMCAL";
		}
		else if(ci == 1){
			Calo_name = "IHCAL";
		}
		else if(ci == 2) {
			Calo_name = "OHCAL";
		}
		else{
			Calo_name= "ERR_CAL_NOT_FOUND";
		}
		for(int ri=0; ri<(int)EC->size(); ri++){
			float Rval=0.;
			if(ri==0) Rval=0.1;
			if(ri==1) Rval=0.4;
			if(ri==2) Rval=1.0;
			int r_no_dec=ri+1;
			EC->at(ri)=new TH1F(Form("h_%s_ETmiss_r_%d", Calo_name.c_str(), r_no_dec), Form("#tilde{E}_{T} of Jet Event Observable for %s at R=%f; #tilde{E}_{T} [GeV]; N_{event}", Calo_name.c_str(), Rval), 100, -0.5, 99.5);
		}
	}
	std::cout<<"Setup complete"<<std::endl;
}
bool LargeRLENC::triggerCut(bool use_em, PHCompositeNode* topNode)
{
	bool good_trigger=false;
	TriggerAnalyzer* ta=new TriggerAnalyzer();
	ta->UseEmulator(use_em);
	ta->decodeTriggers(topNode);
	good_trigger = ta->didTriggerFire("Jet 10 GeV");
	return good_trigger;
}
		
JetContainerv1* LargeRLENC::getJets(std::string input, std::string radius, std::array<float, 3> vertex, float ohcal_rat, PHCompositeNode* topNode)
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
	if(input=="towers" || input.find("ower") != std::string::npos){
		TowerJetInput* ti_ohcal=new TowerJetInput(Jet::HCALOUT_TOWERINFO);
		TowerJetInput* ti_ihcal=new TowerJetInput(Jet::HCALIN_TOWERINFO);
		TowerJetInput* ti_emcal=new TowerJetInput(Jet::CEMC_TOWERINFO);
		auto psjets=ti_ohcal->get_input(topNode);
		auto psjets_ih=ti_ihcal->get_input(topNode);
		auto psjets_em=ti_emcal->get_input(topNode);
		for(auto p:psjets_ih) psjets.push_back(p);
		for(auto p:psjets_em) psjets.push_back(p);
		for(auto p:psjets) jet_objs.push_back(fastjet::PseudoJet(p->get_px(), p->get_py(), p->get_pz(), p->get_e()));
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
		for(auto cmp:j.constituents()){
			jet->insert_comp(Jet::SRC::HCALOUT_CLUSTER, cmp.user_index());
		}
	}
	return fastjetCont;
}	
std::array<float, 3> LargeRLENC::HadronicEnergyBalence(Jet* jet, float ohcal_ihcal_ratio, PHCompositeNode* topNode)
{
	//For the case where we don't already have the source pieces
	float hadronic_energy=0., electromagnetic_energy=0.;
	float ohcal_conversion=ohcal_ihcal_ratio-1.;
	float jet_phi=jet->get_phi(), jet_eta=jet->get_eta();
	float i_e=0.;
	ohcal_conversion=1./ohcal_conversion;
	try{
		findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
	}
	catch(std::exception& e){ return {0.,0., 0.};}
	auto truth_particles_p=findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
	auto truth_particles=truth_particles_p->GetMap();
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
				PHG4Particle* particle = truth_particles[id];
				int pid=particle->get_pid();
				if(abs(pid) == 11 || pid== 22){ 
					//electrons, positrons and photons get put in the emcal
					electromagnetic_energy+=particle->get_e();
					float particle_phi=std::atan2(particle->get_py(), particle->get_px());
					float particle_eta=std::atanh(particle->get_pz()/particle->get_e());
					i_e+=particle->get_e()*std::pow(getR(particle_eta, particle_phi, jet_eta, jet_phi),2);
				}
				else if(abs(pid) > 11 && abs(pid) <= 18){
					//don't count neutrinos, muons, tau
					hadronic_energy+=particle->get_e();
					float particle_phi=std::atan2(particle->get_py(), particle->get_px());
					float particle_eta=std::atanh(particle->get_pz()/particle->get_e());
					i_e+=particle->get_e()*std::pow(getR(particle_eta, particle_phi, jet_eta, jet_phi),2);
				}
			}
		}
	}
	//assume that the hcal energy split is consistent with the whole detector energy split 
	float ohcal_energy=hadronic_energy*ohcal_conversion;
	float ohcal_ratio=ohcal_energy/(hadronic_energy+electromagnetic_energy);
	float emcal_ratio=electromagnetic_energy/(hadronic_energy+electromagnetic_energy);
	i_e=i_e/(hadronic_energy+electromagnetic_energy);
	return {ohcal_ratio, emcal_ratio, i_e};
}
std::vector<std::array<float,3>> LargeRLENC::getJetEnergyRatios(JetContainerv1* jets, float ohcal_ihcal_ratio, PHCompositeNode* topNode)
{
	//get the energy ballence in each calorimeter for each jet
	std::vector<std::array<float, 3>> ohcal_ratio;
	auto emcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode,  emcal_energy_towers );
	auto ihcal_tower_energy= findNode::getClass<TowerInfoContainer>(topNode, ihcal_energy_towers );
	auto ohcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode,  ohcal_energy_towers );
	auto ohcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");
	auto emcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_CEMC"   );
	auto ihcal_geom= findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN");
	ohcal_geom->set_calorimeter_id(RawTowerDefs::HCALOUT);
	ihcal_geom->set_calorimeter_id(RawTowerDefs::HCALIN);
	emcal_geom->set_calorimeter_id(RawTowerDefs::CEMC);
	for(auto j: *jets)
	{
		if(!j) continue;
		bool has_particle=false;
		float ohcal_energy=0., allcal_energy=0., emcal_energy=0.; 
		float i_e=0;
		float jet_phi=j->get_phi(), jet_eta=j->get_eta();
		auto cmp_vec=j->get_comp_vec(); 
		for(auto iter:cmp_vec){
			Jet::SRC source=iter.first; //get source of object
			if(source == Jet::SRC::PARTICLE || source == Jet::SRC::CHARGED_PARTICLE || source == Jet::SRC::HEPMC_IMPORT){
				has_particle=true; 
				//if any particle source is found, we can use that. otherwise have to not use this cut
			}	
			if(  source==Jet::SRC::HEPMC_IMPORT || source > Jet::SRC::HCALOUT_TOWERINFO_SIM || source==Jet::SRC::HCALOUT_CLUSTER || source==Jet::SRC::HCALIN_CLUSTER)continue;
			else{
				if(source==Jet::SRC::HCALOUT_TOWER|| source == Jet::SRC::HCALOUT_CLUSTER){
					unsigned int tower_id=iter.second;
					float e=0.;
					try{
						ohcal_tower_energy->get_tower_at_channel(tower_id)->get_energy();
					}
					catch(std::exception& e){std::cout<<"Bad tower id found for source " <<Jet::SRC::HCALOUT_TOWER <<" despite actual source being " <<source <<" and tower id is " <<tower_id<<std::endl;}
					ohcal_energy+= e;//functionally the same as below, I just want to make things readable
					allcal_energy+= e;
					int phibin=ohcal_tower_energy->getTowerPhiBin(tower_id);
					int etabin=ohcal_tower_energy->getTowerEtaBin(tower_id);
					float phicenter=ohcal_geom->get_phicenter(phibin);
					float etacenter=ohcal_geom->get_etacenter(etabin);	
					i_e+=e*std::pow(getR( etacenter, phicenter, jet_eta, jet_phi),2);
				}
				else if( source == Jet::SRC::HCALOUT_TOWER_SUB1  || source == Jet::SRC::HCALOUT_TOWERINFO_SUB1){
					unsigned int tower_id=iter.second;
					ohcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT_SUB1");
					float e=0.;
					auto key_id=ohcal_tower_energy->encode_key(tower_id);
					e=ohcal_tower_energy->get_tower_at_channel(tower_id)->get_energy();
					ohcal_energy+= e;//functionally the same as below, I just want to make things readable
					allcal_energy+= e;
					int phibin=ohcal_tower_energy->getTowerPhiBin(key_id);
					int etabin=ohcal_tower_energy->getTowerEtaBin(key_id);
					float phicenter=ohcal_geom->get_phicenter(phibin);
					float etacenter=ohcal_geom->get_etacenter(etabin);	
					i_e+=e*std::pow(getR( etacenter, phicenter, jet_eta, jet_phi),2);
				}
				else if(source== Jet::SRC::HCALOUT_TOWERINFO || source == Jet::SRC::HCALOUT_TOWER_SUB1CS ){
					unsigned int tower_id=iter.second;
					float e=ohcal_tower_energy->get_tower_at_channel(tower_id)->get_energy();
					ohcal_energy+= e;//functionally the same as below, I just want to make things readable
					allcal_energy+= e;
					int phibin=ohcal_tower_energy->getTowerPhiBin(tower_id);
					int etabin=ohcal_tower_energy->getTowerEtaBin(tower_id);
					float phicenter=ohcal_geom->get_phicenter(phibin);
					float etacenter=ohcal_geom->get_etacenter(etabin);	
					i_e+=e*std::pow(getR( etacenter, phicenter, jet_eta, jet_phi),2);
				}
				else if(source== Jet::SRC::HCALOUT_TOWERINFO_SIM  || source==Jet::SRC::HCALOUT_TOWERINFO_EMBED ){
					unsigned int tower_id=iter.second;
					float e=ohcal_tower_energy->get_tower_at_channel(tower_id)->get_energy();
					ohcal_energy+= e;//functionally the same as below, I just want to make things readable
					allcal_energy+= e;
					int phibin=ohcal_tower_energy->getTowerPhiBin(tower_id);
					int etabin=ohcal_tower_energy->getTowerEtaBin(tower_id);
					float phicenter=ohcal_geom->get_phicenter(phibin);
					float etacenter=ohcal_geom->get_etacenter(etabin);	
					i_e+=e*std::pow(getR( etacenter, phicenter, jet_eta, jet_phi),2);
				}
				else if(source== Jet::SRC::HCALIN_TOWER || source == Jet::SRC::HCALIN_CLUSTER  ){
					unsigned int tower_id=iter.second;
					float e=ihcal_tower_energy->get_tower_at_channel(tower_id)->get_energy();
					allcal_energy+= e;
					int phibin=ihcal_tower_energy->getTowerPhiBin(tower_id);
					int etabin=ihcal_tower_energy->getTowerEtaBin(tower_id);
					float phicenter=ihcal_geom->get_phicenter(phibin);
					float etacenter=ihcal_geom->get_etacenter(etabin);	
					i_e+=e*std::pow(getR( etacenter, phicenter, jet_eta, jet_phi),2);
				}
				else if( source == Jet::SRC::HCALIN_TOWER_SUB1 || source == Jet::SRC::HCALIN_TOWERINFO_SUB1){
					unsigned int tower_id=iter.second;
					ihcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN_SUB1");
					float e=0.;
					e=ihcal_tower_energy->get_tower_at_channel(tower_id)->get_energy();
					allcal_energy+= e;
					auto key=ihcal_tower_energy->encode_key(tower_id);
					int phibin=ihcal_tower_energy->getTowerPhiBin(key);
					int etabin=ihcal_tower_energy->getTowerEtaBin(key);
					float phicenter=ihcal_geom->get_phicenter(phibin);
					float etacenter=ihcal_geom->get_etacenter(etabin);	
					i_e+=e*std::pow(getR( etacenter, phicenter, jet_eta, jet_phi),2);
				}
				else if(source== Jet::SRC::HCALIN_TOWERINFO || source == Jet::SRC::HCALIN_TOWER_SUB1CS ){
					unsigned int tower_id=iter.second;
					float e=ihcal_tower_energy->get_tower_at_channel(tower_id)->get_energy();
					int phibin=ihcal_tower_energy->getTowerPhiBin(tower_id);
					int etabin=ihcal_tower_energy->getTowerEtaBin(tower_id);
					float phicenter=ihcal_geom->get_phicenter(phibin);
					float etacenter=ihcal_geom->get_etacenter(etabin);	
					i_e+=e*std::pow(getR( etacenter, phicenter, jet_eta, jet_phi),2);
					allcal_energy+= e;
				}
				else if(source== Jet::SRC::HCALIN_TOWERINFO_SIM ||  source==Jet::SRC::HCALIN_TOWERINFO_EMBED ){
					unsigned int tower_id=iter.second;
					float e=ihcal_tower_energy->get_tower_at_channel(tower_id)->get_energy();
					int phibin=ihcal_tower_energy->getTowerPhiBin(tower_id);
					int etabin=ihcal_tower_energy->getTowerEtaBin(tower_id);
					float phicenter=ihcal_geom->get_phicenter(phibin);
					float etacenter=ihcal_geom->get_etacenter(etabin);	
					i_e+=e*std::pow(getR( etacenter, phicenter, jet_eta, jet_phi),2);
					allcal_energy+= e;
				}
				
				else if(source== Jet::SRC::CEMC_TOWER || source == Jet::SRC::CEMC_CLUSTER ){
					unsigned int tower_id=iter.second;
					float e=emcal_tower_energy->get_tower_at_channel(tower_id)->get_energy();
					int phibin=emcal_tower_energy->getTowerPhiBin(tower_id);
					int etabin=emcal_tower_energy->getTowerEtaBin(tower_id);
					float phicenter=emcal_geom->get_phicenter(phibin);
					float etacenter=emcal_geom->get_etacenter(etabin);	
					i_e+=e*std::pow(getR( etacenter, phicenter, jet_eta, jet_phi),2);
					allcal_energy+= e;
					emcal_energy+=e;
				}
				else if( source == Jet::SRC::CEMC_TOWER_SUB1 || source== Jet::SRC::CEMC_TOWERINFO_SUB1 ){
					unsigned int tower_id=iter.second;
					emcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER_SUB1");
					float e=0.;
					e=emcal_tower_energy->get_tower_at_channel(tower_id)->get_energy();
					allcal_energy+= e;
					emcal_energy+=e;
					auto key=emcal_tower_energy->encode_key(tower_id);
					int phibin=emcal_tower_energy->getTowerPhiBin(key);
					int etabin=emcal_tower_energy->getTowerEtaBin(key);
					float phicenter=emcal_geom->get_phicenter(phibin);
					float etacenter=emcal_geom->get_etacenter(etabin);	
					i_e+=e*std::pow(getR( etacenter, phicenter, jet_eta, jet_phi),2);
				}
				else if(source== Jet::SRC::CEMC_TOWERINFO || source == Jet::SRC::CEMC_TOWER_SUB1CS ){
					unsigned int tower_id=iter.second;
					float e=emcal_tower_energy->get_tower_at_channel(tower_id)->get_energy();
					int phibin=emcal_tower_energy->getTowerPhiBin(tower_id);
					int etabin=emcal_tower_energy->getTowerEtaBin(tower_id);
					float phicenter=emcal_geom->get_phicenter(phibin);
					float etacenter=emcal_geom->get_etacenter(etabin);	
					i_e+=e*std::pow(getR( etacenter, phicenter, jet_eta, jet_phi),2);
					allcal_energy+= e;
					emcal_energy+=e;
				}
				else if(source== Jet::SRC::CEMC_TOWERINFO_SIM ||  source==Jet::SRC::CEMC_TOWERINFO_EMBED ){
					unsigned int tower_id=iter.second;
					float e=emcal_tower_energy->get_tower_at_channel(tower_id)->get_energy();
					int phibin=emcal_tower_energy->getTowerPhiBin(tower_id);
					int etabin=emcal_tower_energy->getTowerEtaBin(tower_id);
					float phicenter=emcal_geom->get_phicenter(phibin);
					float etacenter=emcal_geom->get_etacenter(etabin);	
					i_e+=e*std::pow(getR( etacenter, phicenter, jet_eta, jet_phi),2);
					allcal_energy+= e;
					emcal_energy+=e;
				}
			}
		}
		if(ohcal_energy == 0. || allcal_energy == 0. || emcal_energy == 0.)
		{
			std::cout<<"There is an issue getting an energy value, please check status?"<<ohcal_energy <<"   " <<allcal_energy <<std::endl;
			if(has_particle) ohcal_ratio.push_back(HadronicEnergyBalence(j, ohcal_ihcal_ratio, topNode)); //if there is no tower source present, use the particles  
			else ohcal_ratio.push_back({0.,0.,0.});
		}
		else{
			i_e=(float)i_e/(float)allcal_energy;
			std::cout<<"The jet moment of inertia is " <<i_e <<std::endl;
			ohcal_energy=ohcal_energy/allcal_energy;
			emcal_energy=emcal_energy/allcal_energy;
			ohcal_ratio.push_back({ohcal_energy, emcal_energy, i_e});
		}
	}
	return ohcal_ratio;
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
	if(td != RawTowerDefs::CEMC) towers->insert(std::make_pair(center, tower->get_energy()));
	if(td==RawTowerDefs::CEMC){
		//retowering it by hand for right now to improve running speed 
		for(int j=0; j<24; j++){
			float hcalbinval=((j+1)*1.1/12.)-1.1;
			if(center[0] < hcalbinval){
			 	center[0]=hcalbinval;
			}
			else break;
		}
		for(auto j=0; j<64; j++)
		{
			float hcalbinval=(j+1)*2*PI/64.;
			if(center[1] < hcalbinval)  center[1]=hcalbinval;
			else break;
		}
		if(towers->find(center) != towers->end()) towers->at(center)+=tower->get_energy();
		else towers->insert(std::make_pair(center, tower->get_energy()));
	}
	return;
}
int LargeRLENC::process_event(PHCompositeNode* topNode)
{
	//This is where I can be a bit clever with allowing for the parrelization for easier crosschecks
	n_evts++;
	std::cout<<"Running on event : " <<n_evts<<std::endl;
	m_emcal.clear();
	m_ihcal.clear();
	m_ohcal.clear();
	std::cout<<"Cleared the vectors" <<std::endl;
	auto emcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_CEMC"   );
	auto emcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, emcal_energy_towers      );
	auto ihcal_geom= findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN");
	auto ihcal_tower_energy= findNode::getClass<TowerInfoContainer>(topNode, ihcal_energy_towers     );
	auto ohcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");
	auto ohcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, ohcal_energy_towers      );
	//look for the jet objects 
	JetContainerv1* jets=NULL;
	bool foundJetConts=false, isDijet=false;
	try{
		if(!isRealData) jets = findNode::getClass<JetContainerv1>(topNode, "AntiKt_Truth_r04"); //look for the r_04 truth jets
		else{
			std::string recoJetName="AntiKt_"+algo+radius+"_Sub1";
			jets = findNode::getClass<JetContainerv1>(topNode, recoJetName); //check for already reconstructed jets
			if(!jets || jets->size() == 0 ){
				jets = findNode::getClass<JetContainerv1>(topNode, "AntiKt_Tower_r04_Sub1"); //explicit backup check
				std::cout<<"Jet container for tower sub1 has size: " <<jets->size() <<std::endl;
				if(!jets || jets->size() == 0){
					jets = findNode::getClass<JetContainerv1>(topNode, "AntiKt_TowerInfo_r04");
					std::cout<<"Jet container for towerinfo has size: " <<jets->size() <<std::endl;
				}
				if(!jets || jets->size() == 0){
					jets = findNode::getClass<JetContainerv1>(topNode, "AntiKt_unsubtracted_r04");
					std::cout<<"Jet container for unsub has size: " <<jets->size() <<std::endl;
				}
				//try every possible style
			}
			else {
				foundJetConts=true;
				}
		}
	}
	catch(std::exception& e){ std::cout<<"Did not find a jet container object, will attempt to reconstruct the jets" <<std::endl;}
//	if(jets==NULL || !foundJetConts) jets=getJets("Tower", radius, vertex, ohcal_rat, topNode); //if needed will go back to this
	if(!jets){
		std::cout<<"Didn't find any jets, skipping event" <<std::endl;
		return Fun4AllReturnCodes::EVENT_OK;
	}
	else if(jets->size() == 0){
		std::cout<<"Jet Container empty, skipping event" <<std::endl;
		return Fun4AllReturnCodes::EVENT_OK;
	}
	if(foundJetConts) jets->size();
	n_with_jets++;
	std::array<float, 3> vertex={0.,0.,0.}; //set the initial vertex to origin 
	try{
		GlobalVertexMap* vertexmap=findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
		if(vertexmap){
			if(vertexmap->empty())
				std::cout<<"Empty Vertex Map. \n Setting vertex to origin" <<std::endl; 
			else{
				GlobalVertex* gl_vtx=nullptr;
				for(auto vertex_iter:*vertexmap){
					if(vertex_iter.first == GlobalVertex::VTXTYPE::MBD || vertex_iter.first == GlobalVertex::VTXTYPE::SVTX_MBD ) 
					{ 
						gl_vtx=vertex_iter.second;
					}
				}
				if(gl_vtx){
					vertex[0]=gl_vtx->get_x();
					m_vtx=vertex[0];
					vertex[1]=gl_vtx->get_y();
					m_vty=vertex[1];
					vertex[2]=gl_vtx->get_z();
					m_vtz=vertex[2];
				}
			}
		}
	}
	catch(std::exception& e){std::cout<<"Could not find the vertex. \n Setting to origin" <<std::endl;}
	int emcal_oc=0; //allow for occupancy to be calculated seperate from the other bits
	float emcal_energy=0., ihcal_energy=0., ohcal_energy=0., total_energy=0., ohcal_rat=0.;
	std::map<std::array<float, 3>, float> ihcal_towers, emcal_towers, ohcal_towers; //these are just to collect the non-zero towers to decrease the processing time 
	for(int n=0; n<(int) emcal_tower_energy->size(); n++){
		if(! emcal_tower_energy->get_tower_at_channel(n)->get_isGood()) continue;
		float energy=emcal_tower_energy->get_tower_at_channel(n)->get_energy();
		if(energy > 0.07)//put zero supression into effect
			addTower(n, emcal_tower_energy, emcal_geom, &emcal_towers, RawTowerDefs::CEMC   );
		if(energy > 0.01) emcal_oc++;
		emcal_energy+=energy;
	}
	for(int n=0; n<(int) ihcal_tower_energy->size(); n++){
		if(n >= (int)ihcal_tower_energy->size()) continue;
		if(! ihcal_tower_energy->get_tower_at_channel(n)->get_isGood()) continue;
		float energy=ihcal_tower_energy->get_tower_at_channel(n)->get_energy();
		if(energy > 0.01) //zero suppression is at 10 MeV in IHCAL
			addTower(n, ihcal_tower_energy, ihcal_geom, &ihcal_towers, RawTowerDefs::HCALIN  );
		ihcal_energy+=energy;
	}
	for(int n=0; n<(int) ohcal_tower_energy->size(); n++){
		if(! ohcal_tower_energy->get_tower_at_channel(n)->get_isGood()) continue;
		float energy=ohcal_tower_energy->get_tower_at_channel(n)->get_energy();
		if(energy > 0.01) //10 MeV cutoff 
			addTower(n, ohcal_tower_energy, ohcal_geom, &ohcal_towers, RawTowerDefs::HCALOUT );
		ohcal_energy+=energy;
	}
	total_energy=emcal_energy+ihcal_energy+ohcal_energy;
	ohcal_rat=ohcal_energy/(float)total_energy; //take the ratio at the whole calo as that is the region of interest
	float emcal_occupancy=emcal_oc/((float)emcal_tower_energy->size())*100.;
	float ihcal_occupancy=ihcal_towers.size()/((float)ihcal_tower_energy->size())*100.;
	float ohcal_occupancy=ohcal_towers.size()/((float)ohcal_tower_energy->size())*100.;
	emcal_occup->Fill(emcal_occupancy); //filling in the occupancy plots for easy QA while going 
	ihcal_occup->Fill(ihcal_occupancy);
	ohcal_occup->Fill(ohcal_occupancy);
	ohcal_rat_h->Fill(ohcal_rat);
	std::vector<float> ohcal_jet_rat, emcal_jet_rat, jet_ie;
	for(auto h:ohcal_jet_rat_and_ie){
		ohcal_jet_rat.push_back(h[0]);
		emcal_jet_rat.push_back(h[1]);
		jet_ie.push_back(h[2]);
	}
	int i1=0;
	for(auto j: *jets){
		IE->Fill(jet_ie.at(i1));
		E_IE->Fill(j->get_e(), jet_ie.at(i1));
		i1++;
	}
	bool triggered_event = true;
	if(isRealData){
		try{
			auto gl1packet = findNode::getClass<Gl1Packet>(topNode, "Gl1Packet");
			if(gl1packet){
				triggered_event = triggerCut(false, topNode);
			}
		}
		catch(std::exception& e){std::cout<<"Could not find the trigger object" <<std::endl;}
	}
		
	isDijet=eventCut->passesTheCut(jets, ohcal_jet_rat, emcal_jet_rat, ohcal_rat, vertex, jet_ie);	
	if(!isDijet || !triggered_event){ //stores some data about the bad cuts to look for any arrising structure
		ohcal_rat_occup->Fill(ohcal_rat, ohcal_occupancy);
		if(ohcal_rat > 0.9){
		       	for(auto p:ohcal_towers)ohcal_bad_hits->Fill(p.first[0], p.first[1], p.second);
		}
		bad_occ_em_oh->Fill(emcal_occupancy, ohcal_occupancy);
		bad_occ_em_h->Fill(emcal_occupancy, (ohcal_occupancy+ihcal_occupancy)/2.0);
		i1=0;
		for(auto j:*jets){
			badIE->Fill(jet_ie.at(i1));
			badE_IE->Fill(j->get_e(), jet_ie.at(i1));
			i1++;
		}
	       	return Fun4AllReturnCodes::EVENT_OK;
	}
	else{
		i1=0;
		for(auto j:*jets){
			goodIE->Fill(jet_ie.at(i1));
			goodE_IE->Fill(j->get_e(), jet_ie.at(i1));
			i1++;
		}
		float lead_phi=eventCut->getLeadPhi();
		eventCut->getDijets(jets, &m_dijets);
		m_vertex=vertex;
		m_etotal=total_energy;
		m_eemcal=emcal_energy;
		m_eihcal=ihcal_energy;
		m_eohcal=ohcal_energy;
		m_philead=eventCut->getLeadPhi();
		m_etalead=eventCut->getLeadEta();
		int i=0;
		for(auto j:*jets){
			if(!j) continue;
			Region_vector[0][0][0]->pt->Fill(j->get_pt());
			Region_vector[0][0][1]->pt->Fill(j->get_pt()*(emcal_jet_rat.at(i)));
			Region_vector[0][0][2]->pt->Fill(j->get_pt()*(1-emcal_jet_rat.at(i)-ohcal_jet_rat.at(i)));
			Region_vector[0][0][3]->pt->Fill(j->get_pt()*(ohcal_jet_rat.at(i)));
			i++;
		}
		//this is running the actual analysis
		LargeRLENC::CaloRegion(emcal_towers, ihcal_towers, ohcal_towers, jetMinpT, which_variable, vertex, lead_phi);
		DijetQA->Fill();
	}
	return Fun4AllReturnCodes::EVENT_OK;
}
void LargeRLENC::CaloRegion(std::map<std::array<float, 3>, float> emcal, std::map<std::array<float, 3>, float> ihcal, std::map<std::array<float, 3>, float> ohcal, float jetMinpT, std::string variable_of_interest, std::array<float, 3> vertex, float lead_jet_center_phi)
{ 
	bool transverse=false;
	bool energy=true;
	float phi_min=0., phi_max=2*PI;
	int v_o_i_code=0;
	std::map<int, std::pair<float, float>> region_ints;
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
	SingleCaloENC(emcal, jetMinpT, vertex, transverse, energy, region_ints, LargeRLENC::Calorimeter::EMCAL, &total_e);
	SingleCaloENC(ihcal, jetMinpT, vertex, transverse, energy, region_ints, LargeRLENC::Calorimeter::IHCAL, &total_e);
	SingleCaloENC(ohcal, jetMinpT, vertex, transverse, energy, region_ints, LargeRLENC::Calorimeter::OHCAL, &total_e);
	return;
}
void LargeRLENC::SingleCaloENC(std::map<std::array<float, 3>, float> cal, float jetMinpT, std::array<float, 3> vertex, /*int region,*/ bool transverse, bool energy, std::map<int, std::pair<float, float>> phi_edges,/* std::vector<MethodHistograms*>* Histos,*/ int which_calo, float* total_e_region)
{
	MethodHistograms* ha=NULL, *hc=NULL;
	float base_thresh=0.01;
	if(which_calo== 1) base_thresh= 0.07;
	auto i=cal.begin();
	std::map<std::array<float, 3>, float> p_T_save;
	int n=0;
	std::vector<std::array<float,4>> e_region; //get the energy in the relevant region 
	std::array<int, 20> thresh_size;
	for(int j=0; j<(int)Region_vector.size(); j++){
		thresh_size[j]=0;
		std::array<float, 4> e_region_1 { 0., 0., 0., 0. };
		e_region.push_back(e_region_1);
	} 
	while(i !=cal.end()){
		n++;
		auto j=i->first;
		auto j_val=i->second;
		float pT=0., eta_shift=i->first[0], r=i->first[2];
		float x=r*cos(i->first[1]), y=r*sin(i->first[1]);
		if(vertex[0]!=0) x+=vertex[0];
		if(vertex[1]!=0) y+=vertex[1];
		r=std::sqrt(std::pow(x,2) + std::pow(y,2));
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
				ha=Region_vector.at(0).at(region_index).at(0); 
				hc=Region_vector.at(0).at(region_index).at(which_calo);
				break;
			}
		}
		if(!ha || !hc){
			 continue;
		}
		else{
			for(int k = 0; k<(int)Region_vector.size(); k++){
				float thresh=base_thresh*(1+0.5*k);
				cal[j]=j_val;
				if(j_val < thresh || j_val != j_val) continue;
				e_region[k][region_index]+=j_val;
				e_region[k][0]+=j_val;
				thresh_size[k]++;
	//		       	++i;
				pT=j_val; //right now using p approx E, can probably improve soon
				pT=pT/cosh(eta_shift);
				if(pT > MinpTComp) p_T_save[j]=pT;
				Region_vector.at(k).at(region_index).at(which_calo)->pt->Fill(pT);
				Region_vector.at(k).at(0).at(which_calo)	   ->pt->Fill(pT);
				bool printout=false;
				float R=getR(j[0], j[1], eventCut->getLeadEta(), eventCut->getLeadPhi(), printout );
				if(n_evts < 5 && printout) std::cout<<"R val is " <<R <<" \n with inputs " <<j[0] <<", " <<j[1] <<" , " <<eventCut->getLeadEta() <<" , " <<eventCut->getLeadPhi() <<std::endl;
				Region_vector.at(k).at(region_index).at(which_calo)->R_pt->Fill(R,pT);
				Region_vector.at(k).at(0).at(which_calo)	   ->R_pt->Fill(R,pT);
			}
		}
	}
	for(int i=0; i<(int)e_region.size(); i++){
		for(int j=0; j<(int)e_region[i].size(); j++){
			if(e_region[i][j]==0.) 
				e_region[i][j]=1.; //just correcting for any nan inputs
			Region_vector.at(i).at(j).at(which_calo)->E->Fill(e_region[i][j]);
		}
		Region_vector.at(i).at(0).at(which_calo)->E->Fill(e_region[i][0]);
	}
	std::map<std::array<float, 3>, float> cal_2=cal;
	std::map<int, std::map<float, float>> e2c, e3c;
	std::map<int, std::map<std::array<float, 3>, float>> e3c_full;
	for(int i=0; i<4; i++){
		std::map<float, float> dummy {{0.,0.}};
		e2c[i]=dummy;
		e3c[i]=dummy;
		std::array<float, 3> dummy_array {0., 0., 0.};
		std::map<std::array<float, 3>, float> dummy_2{{dummy_array, 0.}}; 
		e3c_full[i]=dummy_2;
		for(int j=0; j<(int)Region_vector.size(); j++){
			hc=Region_vector.at(j).at(i).at(which_calo);
			hc->N->Fill(thresh_size[j]);
		}
	} //just make sure the maps are initialized to be in working order ahead of the calculations
	for(int i = 0; i< (int) Region_vector.size(); i++) Region_vector.at(i).at(0).at(which_calo)->N->Fill(thresh_size[i]);
	(*total_e_region)+=e_region[0][0];
	for(auto i=cal.begin(); i != cal.end(); ++i){
		std::array<float, 3> tower {i->first[0], i->first[1], i->second}; 
		if(which_calo == LargeRLENC::Calorimeter::EMCAL)m_emcal.push_back(tower);
		else if(which_calo == LargeRLENC::Calorimeter::IHCAL)m_ihcal.push_back(tower);
		else if(which_calo == LargeRLENC::Calorimeter::OHCAL)m_ohcal.push_back(tower);
		auto j=cal_2.find(i->first);
		if(j != cal_2.end()) 
			cal_2.erase(j); //getting rid of double counting by removing the base value from the second map 
		auto cal_3=cal_2;
		auto k=cal_3.find(j->first);
		if(k != cal_3.end()) 
			cal_3.erase(k); //getting rid of double counting by removing the base value from the third map 
		std::vector<std::pair<float,std::vector< std::pair<float, float> > >> point_correlator ((int)cal_2.size());
		for(int i=0; i<(int)point_correlator.size(); i++){
			std::vector< std::pair<float, float> > pt_c (20); //allow for easy looping over the towers to parrelize 
			point_correlator.at(i)=std::make_pair(0.,pt_c);
		}
		//now permits various thresholds
		std::vector< std::pair<float, std::vector< std::pair< std::pair<float, float>, float > > > > threept_full ((int) cal_2.size());
		std::vector<std::thread> calculating_threads; 
		int index=0;
		std::map<int, int> region_index; //gives a way to pull the relevant region after the fact to fill hc
		for(auto l:cal_2)
		{
		 	//threading over all pairs to allow for faster calculation 
			if(l.second < MinpTComp) continue;
			calculating_threads.push_back(std::thread(&LargeRLENC::CalculateENC, this, *i, l, cal_3, &point_correlator[index], &threept_full[index], transverse, energy, base_thresh)); 
			index++;
			for(auto x:phi_edges){
				int regions_val=x.first;
				if(regions_val >=3) regions_val = 3;
				if(regions_val==0) continue;
				if( l.first[1] < x.second.first || l.first[1] > x.second.second)continue;
				else{
					
					region_index[index]=regions_val;
					break;
				}
				}
		}
		for(int t=0; t<(int) calculating_threads.size(); t++) calculating_threads[t].join();
		index=0;
		int threshold_index=0;
		std::vector<std::vector<std::pair<float, std::pair<float, float> > > > point_correlator_1 (20); //change into a better threshold friendly format
		for(int j= 0; j< (int) point_correlator_1.size(); j++){
			 for(int i = 0; i<(int)point_correlator.size(); i++)
			{
				point_correlator_1.at(j).push_back(std::make_pair(point_correlator.at(i).first,point_correlator.at(i).second.at(j)));
			}
		}
		for(auto pc_1:point_correlator_1){
			for(auto v:pc_1){
			auto l=e2c[region_index[index]].find(v.first);
			auto m=e3c[region_index[index]].find(v.first);
			if(v.first != v.first) continue;
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
			Region_vector[threshold_index][region_index[index]][0]->R->Fill(v.first); //this needs the threshold
			Region_vector[threshold_index][region_index[index]][which_calo]->R->Fill(v.first);
			Region_vector[threshold_index][0][0]->R->Fill(v.first);
			Region_vector[threshold_index][0][which_calo]->R->Fill(v.first);
			index++;
			}
		
		index=0;
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
	for(auto es:e2c){
		std::vector<std::pair<float, float>> holding_2;
		for(auto v:es.second){
			auto vs=v.second*std::pow(e_region[threshold_index][es.first], -2);
	        	Region_vector[threshold_index][es.first][which_calo]->E2C->Fill(v.first, vs);
			holding_2.push_back(std::make_pair(v.first, vs)); 
		}
		t_e2c.push_back(std::make_pair(es.first, holding_2));
	}
	for(auto es:e3c){
		std::vector<std::pair<float, float>> holding_3;
		for(auto v:es.second){
			auto vf=v.second*std::pow(e_region[threshold_index][es.first], -3);
			Region_vector[threshold_index][es.first][which_calo]->E3C->Fill(v.first, vf);
			holding_3.push_back(std::make_pair(v.first, vf));
			Region_vector.at(threshold_index).at(0).at(which_calo)->E3C->Fill(v.first, vf);
		}
		t_e3c.push_back(std::make_pair(es.first, holding_3));
	}
	for(auto es:e3c_full)
	{
		std::vector< std::pair < std::array<float, 3>, float> > holding_3;
		for(auto v:es.second){
			auto vf=v.second*std::pow(e_region[threshold_index][es.first], -3);
			holding_3.push_back(make_pair(v.first, vf));
			
		}
		t_e3c_full.push_back(std::make_pair(es.first, holding_3));
	}
		threshold_index++;
	}
	return;
}
void LargeRLENC::CalculateENC(std::pair<std::array<float, 3>, float> point_1, std::pair<std::array<float, 3>, float> point_2, std::map<std::array<float, 3>, float> cal, std::pair<float, std::vector<std::pair<float, float>>>* enc_out, std::pair<float, std::vector< std::pair< std::pair<float, float>, float > > > *threept_full, bool transverse, bool energy, float calo_min)
{
	//this is the threaded object, I could try nesting the threads, but for now I am going to try to run without the nested threads for the 3pt
	float R_L=getR(point_1.first[0], point_1.first[1], point_2.first[0], point_2.first[1]);
	float e2c=0.;
	if(energy) e2c=(point_1.second) * (point_2.second);
	else e2c=(point_1.second * ptoE) * (point_2.second * ptoE); //use a basic swap of momenta ratio for each calo
	if(transverse) e2c=e2c/(cosh(point_1.first[0]) * cosh(point_2.first[0]));
	enc_out->first=R_L;
	for(int j = 0; j< (int)enc_out->second.size(); j++){
		if(point_1.second > calo_min * (1+0.25*j) && point_2.second > calo_min * (1+0.25*j) && e2c==e2c){
			enc_out->second.at(j)=std::make_pair(e2c, 0);
			threept_full->first=R_L;
			for(auto i:cal)
			{
				float R_13=getR(point_1.first[0], point_1.first[1], i.first[0], i.first[1]);
				float R_23=getR(point_2.first[0], point_2.first[1], i.first[0], i.first[1]);
				float maxRL=std::max(R_L, R_13); //so I need to change the output sturcutre because I'm not sure if I am matching correctly
				maxRL=std::max(maxRL, R_23);
				if(maxRL > R_L || i.second < calo_min * ( 1+ 0.25 * j) ) continue;
				else{ 
					float e3c=e2c*i.second;
					if(!energy) e3c=e3c*ptoE;
					if(transverse) e3c=e3c/(cosh(i.first[0]));
					if(e3c == e3c ) enc_out->second.at(j).second+=e3c;
					std::pair<float, float> rs {R_13, R_23};
					std::pair<std::pair<float, float>, float> ec {rs, e3c};
					if(j== (int) enc_out->second.size()) threept_full->second.push_back(ec);
				}
		
				if(i.second != 0 && e2c == e2c ) enc_out->second.at(j).second+=e2c*i.second;
			
				}
			} 
		}
	return;
}
void LargeRLENC::JetEventObservablesBuilding(std::array<float, 3> central_tower, std::map<std::array<float, 3>, float> calorimeter_input, std::map<float, float>* Etir_output )
{
	//This is the Jet Event observable. Ideally I would seperate this out 
	//Input is of the form <tower r, tower eta, tower phi>, tower E (vertex corrected)
	//Output is <tower r, tower eta, tower phi>, <R=0.1-1.0, ETir> 
	for(int r=0; r<3; r++)
	{
		float Rmax=0.1;
		if(r==1) Rmax=0.4;
		if(r==2) Rmax=1.0;
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
	return std::sqrt(rsq);
}
void LargeRLENC::Print(const std::string &what) const
{
	TFile* f1=new TFile(output_file_name.c_str(), "RECREATE");
	float pct=(float) n_with_jets / (float) n_evts;
	pct=pct*100; 
	TH1F* h_percent=new TH1F("h_pct", "Percentage of events with a non-empty jet container; Percent; N_{files}", 100, -0.5, 99.5);
	h_percent->Fill(pct);
	h_percent->Write();
	std::cout<<"Percentage of events where the jets were present: " <<pct <<std::endl; 
	emcal_occup->Write();
	ihcal_occup->Write();
	ohcal_occup->Write();
	ohcal_rat_h->Write();
	ohcal_rat_occup->Write();
	ohcal_bad_hits->Write();
	bad_occ_em_oh->Write();
	bad_occ_em_h->Write();
	IE->Write();
	badIE->Write();
	goodIE->Write();
	E_IE->Write();
	badE_IE->Write();
	goodE_IE->Write();
	eventCut->JetCuts->Write();
	for(int i = 0; i <(int)Region_vector.size(); i++){
		auto FullCal=Region_vector.at(i).at(0);
		auto TowardRegion=Region_vector.at(i).at(1);
		auto AwayRegion=Region_vector.at(i).at(2);
		auto TransverseRegion=Region_vector.at(i).at(3);
		 
	for(auto hv:FullCal){
		for( auto h:hv->histsvector){
			try{
				 if(h) h->Write();
			}
			catch(std::exception& e){continue;}
		}
		hv->R_pt->Write();
		hv->E2C->Write();
		hv->E3C->Write();
		hv->R->Write();
		hv->N->Write();
		hv->E->Write();
		hv->pt->Write();
	}
	for(auto hv:TransverseRegion){
		for( auto h:hv->histsvector)if(h) h->Write();	
		hv->R_pt->Write();
		hv->E2C->Write();
		hv->E3C->Write();
		hv->R->Write();
		hv->N->Write();
		hv->E->Write();
		hv->pt->Write();
	}
	for(auto hv:AwayRegion){
		for( auto h:hv->histsvector)if(h) h->Write();
		hv->R_pt->Write();
		hv->E2C->Write();
		hv->E3C->Write();
		hv->R->Write();
		hv->N->Write();
		hv->E->Write();
		hv->pt->Write();
	}
	for(auto hv:TowardRegion){	
//		for( auto h:hv->histsvector)if(h) h->Write(); //this stopped working for some reason
		hv->R_pt->Write();
		hv->E2C->Write();
		hv->E3C->Write();
		hv->R->Write();
		hv->N->Write();
		hv->E->Write();
		hv->pt->Write();
		}
	}
	f1->Write();
	f1->Close();
	return;	
}
