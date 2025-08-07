//////////////////////////////////////////////////////////////////////////////////////////////////
//											  	//
//			Large R_L Calorimeter ENC						//
//												//
//			Author: Skaydi Grossberndt						//
//			Depends on: Calorimeter Tower ENC 					//
//			First Commit date: 18 Oct 2024						//
//			Most recent Commit: 17 Apr 2025						//
//			version: v5 single threshold value as derived from the values 	 	//

//												//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "LargeRLENC.h"


LargeRLENC::LargeRLENC(const int n_run/*=0*/, const int n_segment/*=0*/, const float jet_min_pT/*=0*/, const bool data/*=false*/, const bool pedestal, std::fstream* ofs/*=nullptr*/, const std::string vari/*="E"*/, const std::string& name/* = "LargeRLENC"*/) 
{

	n_evts=0;
	this->pedestalData=pedestal;
	if(pedestal){
		ohcal_min=0.014;
		emcal_min=0.062;
		ihcal_min=0.004;
		all_min=0.1;
	}
	thresh_mins[0]=all_min;
	thresh_mins[1]=emcal_min;
	thresh_mins[2]=ihcal_min;
	thresh_mins[3]=ohcal_min;
	thresh_mins[4]=0.1;
	MethodHistograms* fc, *fe, *fi, *fo, *tc, *te, *ti, *to, *ac, *ae, *ai, *ao, *trc, *tre, *tri, *tro;
//set bin widths to tower size
	float allcal_thresh=1000*all_min;
	float emcal_thresh=1000*emcal_min;
	float ohcal_thresh= 1000*ohcal_min;
	float ihcal_thresh=1000*ihcal_min; //has much smaller gaps
	this->Thresholds=thresh_mins;
	std::string ihcal_thresh_s="_"+std::to_string((int)ihcal_thresh)+"_MeV_threshold";
	std::string ohcal_thresh_s="_"+std::to_string((int)ohcal_thresh)+"_MeV_threshold";
	std::string emcal_thresh_s="_"+std::to_string((int)emcal_thresh)+"_MeV_threshold";
	std::string allcal_thresh_s="_"+std::to_string((int)allcal_thresh)+"_MeV_threshold";
	
	fc=new MethodHistograms("Full_CAL"+allcal_thresh_s, 4*PI, 0.1); 
	fe=new MethodHistograms("Full_EMCAL"+emcal_thresh_s, 4*PI, 0.1);
	fi=new MethodHistograms("Full_IHCAL"+ihcal_thresh_s, 4*PI, 0.1);
	fo=new MethodHistograms("Full_OHCAL"+ohcal_thresh_s, 4*PI, 0.1); //Full OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~pi)

	tc=new MethodHistograms("Towards_Region_CAL"+allcal_thresh_s, 4*PI, 0.1);
	te=new MethodHistograms("Towards_Region_EMCAL"+emcal_thresh_s, 4*PI, 0.1);
	ti=new MethodHistograms("Towards_Region_IHCAL"+ihcal_thresh_s, 4*PI, 0.1);
	to=new MethodHistograms("Towards_Region_OHCAL"+ohcal_thresh_s, 4*PI, 0.1); //Towards_Region OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~2pi/3)

	ac=new MethodHistograms("Away_Region_CAL"+allcal_thresh_s, 4*PI, 0.1);
	ae=new MethodHistograms("Away_Region_EMCAL"+emcal_thresh_s, 4*PI, 0.1);
	ai=new MethodHistograms("Away_Region_IHCAL"+ihcal_thresh_s, 4*PI, 0.1);
	ao=new MethodHistograms("Away_Region_OHCAL"+ohcal_thresh_s, 4*PI, 0.1); //Away_Region OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~2pi/3)

	trc=new MethodHistograms("Transverse_Region_CAL"+allcal_thresh_s, 4*PI, 0.1);
	tre=new MethodHistograms("Transverse_Region_EMCAL"+emcal_thresh_s, 4*PI, 0.1);
	tri=new MethodHistograms("Transverse_Region_IHCAL"+ihcal_thresh_s, 4*PI, 0.1);
	tro=new MethodHistograms("Transverse_Region_OHCAL"+ohcal_thresh_s, 4*PI, 0.1); //Transverse_Region OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~pi)
	if(!pedestalData && !data){
		MethodHistograms* fc1=new MethodHistograms("Truth_Full_CAL"+allcal_thresh_s, 4*PI, 0.1); 
	
		MethodHistograms* tc1=new MethodHistograms("Truth_Towards_Region_CAL"+allcal_thresh_s, 4*PI, 0.1);

		MethodHistograms* ac1=new MethodHistograms("Truth_Away_Region_CAL"+allcal_thresh_s, 4*PI , 0.1);
	
		MethodHistograms* trc1=new MethodHistograms("Truth_Transverse_Region_CAL"+allcal_thresh_s, 4*PI, 0.1);
		std::vector<std::vector<MethodHistograms*>> Region_truth {std::vector<MethodHistograms*>{fc1}, std::vector<MethodHistograms*>{tc1}, std::vector<MethodHistograms*>{ac1}, std::vector<MethodHistograms*>{trc1}};
		this->Truth_Region_vector=Region_truth;	
	}	
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
	if(pedestal || !data )
	{
		for(auto a:Region_vector_1)
			for(auto b:a){
				b->E->SetCanExtend(TH1::kAllAxes);
				b->N->SetCanExtend(TH1::kAllAxes);
				b->pt->SetCanExtend(TH1::kAllAxes);
				b->R_pt->SetCanExtend(TH1::kAllAxes);
			}
	} //led just does kinda odd things with the energy
	this->Region_vector=Region_vector_1;
	


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
	float ohcal_conversion=ohcal_ihcal_ratio/(float)(ohcal_ihcal_ratio+1.);
	float jet_phi=jet->get_phi(), jet_eta=jet->get_eta();
	float i_e=0.;
	try{
		findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
	}
	catch(std::exception& e){ return {0.,0., 0.};}
	auto truth_particles_p=findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
	try{
		truth_particles_p->GetMap();
	}
	catch(std::exception& e){std::cout<<"Could not find particle map" <<std::endl;
		return {0.,0.,0.};
	}
	std::map<int, PHG4Particle*> truth_particles;
	for(const auto& a:truth_particles_p->GetMap()) truth_particles[a.first]=a.second;
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
	//		std::cout<<"There is an issue getting an energy value, please check status?"<<ohcal_energy <<"   " <<allcal_energy <<std::endl;
			if(has_particle) ohcal_ratio.push_back(HadronicEnergyBalence(j, ohcal_ihcal_ratio, topNode)); //if there is no tower source present, use the particles  
			else ohcal_ratio.push_back({0.,0.,0.});
		}
		else{
			i_e=(float)i_e/(float)allcal_energy;
	//		std::cout<<"The jet moment of inertia is " <<i_e <<std::endl;
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

	if(td != RawTowerDefs::CEMC && (td != RawTowerDefs::HCALOUT && this->pedestalData)) towers->insert(std::make_pair(center, tower->get_energy()));
	else if (td == RawTowerDefs::HCALOUT && this->pedestalData) towers->insert(std::make_pair(center, tower->get_energy()/100.));

	else if(td==RawTowerDefs::CEMC){
		//retowering it by hand for right now to improve running speed 
	/*	for(int j=0; j<24; j++){
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
		}*/
		center[0]=emcal_lookup_table[n].first;
		center[1]=emcal_lookup_table[n].second;
		if(towers->find(center) != towers->end()) towers->at(center)+=tower->get_energy();
		else towers->insert(std::make_pair(center, tower->get_energy()));
	}
	return;
}
void LargeRLENC::MakeEMCALRetowerMap(RawTowerGeomContainer_Cylinderv1* em_geom, TowerInfoContainer* emcal, RawTowerGeomContainer_Cylinderv1* h_geom, TowerInfoContainer* hcal )
{
	em_geom->set_calorimeter_id(RawTowerDefs::CEMC);
	h_geom->set_calorimeter_id(RawTowerDefs::HCALOUT);

	for(int n=0; n<(int) emcal->size(); n++){
		auto key=emcal->encode_key(n);
		int phibin=emcal->getTowerPhiBin(key);
		int etabin=emcal->getTowerEtaBin(key);
		float phicenter=em_geom->get_phicenter(phibin);
		float etacenter=em_geom->get_etacenter(etabin);
		for(int j=0; j<(int) hcal->size(); j++)
		{
			bool goodPhi=false, goodEta=false;
			auto key_h=hcal->encode_key(j);
			int phibin_h=hcal->getTowerPhiBin(key_h);
			int etabin_h=hcal->getTowerEtaBin(key_h);
			float phicenter_h=h_geom->get_phicenter(phibin_h);
			float etacenter_h=h_geom->get_etacenter(etabin_h);	
			std::pair<double, double> phi_bounds=h_geom->get_phibounds(phibin_h);
			std::pair<double, double> eta_bounds=h_geom->get_etabounds(etabin_h);
			if(phicenter >= phi_bounds.first && phicenter < phi_bounds.second) goodPhi=true; 
			if(etacenter >= eta_bounds.first && etacenter < eta_bounds.second) goodEta=true;
			if(goodPhi && goodEta){
				this->emcal_lookup_table[n]=std::make_pair(etacenter_h, phicenter_h);
				break;
			}
			else continue;
		}
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
	bool /*foundJetConts=false, */isDijet=false;
	if(this->emcal_lookup_table.size() == 0  || n_evts==1)
	{
		MakeEMCALRetowerMap(emcal_geom, emcal_tower_energy, ohcal_geom, ohcal_tower_energy);
		std::cout<<"Lookup table has size: " <<this->emcal_lookup_table.size() <<std::endl;
	}
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
					if(jets) std::cout<<"Jet container for towerinfo has size: " <<jets->size() <<std::endl;
				}
				if(!jets || jets->size() == 0){
					jets = findNode::getClass<JetContainerv1>(topNode, "AntiKt_unsubtracted_r04");
					if(jets) std::cout<<"Jet container for unsub has size: " <<jets->size() <<std::endl;
				}
				//try every possible style
			}
	/*		else {
				foundJetConts=true;
				}*/
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
	std::map<std::array<float, 3>, float> ihcal_towers, emcal_towers, ohcal_towers, truth_pts; //these are just to collect the non-zero towers to decrease the processing time 
	for(int n=0; n<(int) emcal_tower_energy->size(); n++){
		if(! emcal_tower_energy->get_tower_at_channel(n)->get_isGood()) continue;
		float energy=emcal_tower_energy->get_tower_at_channel(n)->get_energy();
		if(energy > emcal_min )//put zero supression into effect
			addTower(n, emcal_tower_energy, emcal_geom, &emcal_towers, RawTowerDefs::CEMC   );
		if(energy > emcal_min) emcal_oc++;
		emcal_energy+=energy;
	}
	for(int n=0; n<(int) ihcal_tower_energy->size(); n++){
		if(n >= (int)ihcal_tower_energy->size()) continue;
		if(! ihcal_tower_energy->get_tower_at_channel(n)->get_isGood()) continue;
		float energy=ihcal_tower_energy->get_tower_at_channel(n)->get_energy();
		if(energy > ihcal_min) //zero suppression is at 10 MeV in IHCAL
			addTower(n, ihcal_tower_energy, ihcal_geom, &ihcal_towers, RawTowerDefs::HCALIN  );
		ihcal_energy+=energy;
	}
	for(int n=0; n<(int) ohcal_tower_energy->size(); n++){
		if(! ohcal_tower_energy->get_tower_at_channel(n)->get_isGood()) continue;
		float energy=ohcal_tower_energy->get_tower_at_channel(n)->get_energy();
		if(energy > ohcal_min ) //10 MeV cutoff 
			addTower(n, ohcal_tower_energy, ohcal_geom, &ohcal_towers, RawTowerDefs::HCALOUT );
		ohcal_energy+=energy;
	}
	
	if(!isRealData && !pedestalData){
		auto hepmc_gen_event= findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
		if(hepmc_gen_event){
			for( PHHepMCGenEventMap::ConstIter evtIter=hepmc_gen_event->begin(); evtIter != hepmc_gen_event->end(); ++evtIter)
			{
				PHHepMCGenEvent* hpev=evtIter->second;
				if(hpev){
					HepMC::GenEvent* ev=hpev->getEvent();	
					if(ev)
					{
						for(HepMC::GenEvent::particle_const_iterator iter=ev->particles_begin(); iter !=ev->particles_end(); ++iter){
							if((*iter))
							{
								if(!(*iter)->end_vertex() && (*iter)->status() == 1){
									float px=(*iter)->momentum().px();
									float py=(*iter)->momentum().py();
									float pz=(*iter)->momentum().pz();
									float E=(*iter)->momentum().e();
									float phi=atan2(py, px);
									float eta=atanh(px/(std::sqrt(px*px+py*py+pz*pz)));
									float r=1.;
									std::array<float, 3> loc {eta, phi, r};
									truth_pts[loc]=E;

								}
							}
						}
					}
				}
			}
		}
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
	std::vector<std::array<float, 3>> ohcal_jet_rat_and_ie=getJetEnergyRatios(jets, ohcal_energy/(float)ihcal_energy, topNode);
	for(auto h:ohcal_jet_rat_and_ie){
		ohcal_jet_rat.push_back(h[0]);
		emcal_jet_rat.push_back(h[1]);
		jet_ie.push_back(h[2]);
	}
	int i1=0;
	for(auto j: *jets){
		if(jet_ie.at(i1) > 0 )IE->Fill(jet_ie.at(i1));
		if(jet_ie.at(i1) > 0 )E_IE->Fill(j->get_e(), jet_ie.at(i1));
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
			if(jet_ie.at(i1) > 0 ) badIE->Fill(jet_ie.at(i1));
			if(jet_ie.at(i1) > 0 )badE_IE->Fill(j->get_e(), jet_ie.at(i1));
			i1++;
		}
	       	return Fun4AllReturnCodes::EVENT_OK;
	}
	else{
		i1=0;
		for(auto j:*jets){
			if(jet_ie.at(i1) > 0 )goodIE->Fill(jet_ie.at(i1));
			if(jet_ie.at(i1) > 0 )goodE_IE->Fill(j->get_e(), jet_ie.at(i1));
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
			Region_vector[0][0]->pt->Fill(j->get_pt());
			Region_vector[0][1]->pt->Fill(j->get_pt()*(emcal_jet_rat.at(i)));
			Region_vector[0][2]->pt->Fill(j->get_pt()*(1-emcal_jet_rat.at(i)-ohcal_jet_rat.at(i)));
			Region_vector[0][3]->pt->Fill(j->get_pt()*(ohcal_jet_rat.at(i)));
			i++;
		}
		if(isDijet) n_with_jets++;
		//this is running the actual analysis
		LargeRLENC::CaloRegion(emcal_towers, ihcal_towers, ohcal_towers, jetMinpT, which_variable, vertex, lead_phi);
		if(!isRealData && !pedestalData) LargeRLENC::TruthRegion(truth_pts, jetMinpT, which_variable, vertex, lead_phi); 
		DijetQA->Fill();
	}
	return Fun4AllReturnCodes::EVENT_OK;
}
void LargeRLENC::TruthRegion(std::map<std::array<float, 3>, float> truth, float jetMinpT, std::string variable_of_interest, std::array<float, 3> vertex, float lead_jet_center_phi)
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
		SingleCaloENC(truth, jetMinpT, vertex, transverse, energy, region_ints, LargeRLENC::Calorimeter::TRUTH);
		truth.clear();
		return;
}
void LargeRLENC::CaloRegion(std::map<std::array<float, 3>, float> emcal, std::map<std::array<float, 3>, float> ihcal, std::map<std::array<float, 3>, float> ohcal, float jetMinpT, std::string variable_of_interest, std::array<float, 3> vertex, float lead_jet_center_phi)
{ 
	std::map<std::array<float, 3>, float>allcal;
	for(auto t:emcal) allcal[t.first]=t.second;
	for(auto t:ihcal){
		if(allcal.find(t.first) != allcal.end()) allcal[t.first]+=t.second;
		else allcal[t.first]=t.second;
	}
	for(auto t:ohcal){
		if(allcal.find(t.first) != allcal.end()) allcal[t.first]+=t.second;
		else allcal[t.first]=t.second;
	}

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

//	std::vector<std::thread> CaloThreads;
//	CaloThreads.push_back(std::thread(&LargeRLENC::SingleCaloENC, this, emcal, jetMinpT, vertex, transverse, energy, region_ints, LargeRLENC::Calorimeter::EMCAL));
	//CaloThreads.push_back(std::thread(&LargeRLENC::SingleCaloENC, this, ihcal, jetMinpT, vertex, transverse, energy, region_ints, LargeRLENC::Calorimeter::IHCAL));
//	CaloThreads.push_back(std::thread(&LargeRLENC::SingleCaloENC, this, ohcal, jetMinpT, vertex, transverse, energy, region_ints, LargeRLENC::Calorimeter::OHCAL));
//	if(! this->pedestalData) 
	//	CaloThreads.push_back(std::thread(&LargeRLENC::c
	if(!pedestalData) SingleCaloENC(allcal, jetMinpT, vertex, transverse, energy, region_ints, LargeRLENC::Calorimeter::All);
//	for(int ct=0; ct<(int)CaloThreads.size(); ct++) CaloThreads[ct].join();
	allcal.clear();
		SingleCaloENC(emcal, jetMinpT, vertex, transverse, energy, region_ints, LargeRLENC::Calorimeter::EMCAL);
	emcal.clear();
		SingleCaloENC(ihcal, jetMinpT, vertex, transverse, energy, region_ints, LargeRLENC::Calorimeter::IHCAL);
	ihcal.clear();
		SingleCaloENC(ohcal, jetMinpT, vertex, transverse, energy, region_ints, LargeRLENC::Calorimeter::OHCAL);
	ohcal.clear();
	
	return;
}
void LargeRLENC::SingleCaloENC(std::map<std::array<float, 3>, float> cal, float jetMinpT, std::array<float, 3> vertex, bool transverse, bool energy, std::map<int, std::pair<float, float>> phi_edges, LargeRLENC::Calorimeter which_calo)
{
	//MethodHistograms* ha=NULL, *hc=NULL;
	float base_thresh=thresh_mins[which_calo];
	auto i=cal.begin();
	std::vector<StrippedDownTower*> strippedCalo;
	if(which_calo == LargeRLENC::Calorimeter::EMCAL) base_thresh=base_thresh*16.; //correct for the fact that the emcal has ~16 actual towers going to a single tower object
	std::array<float, 4> e_region { 0., 0., 0., 0. };
	
	
	//Processs the calorimeter data into my analysis class
	//Do the vertex shift and add it into the output 
	std::cout<<"Incoming prefiltered towers : " <<cal.size() <<std::endl;
	if(cal.size() == 0 ) return; 
	while(i !=cal.end()){
		auto j=i->first;
		auto j_val=i->second;
		if(j_val < base_thresh){
			++i;
	//		std::cout<<"Failed with tower energy " <<j_val <<std::endl;
			continue;
		}
		float eta_shift=i->first[0], r=i->first[2];
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
		StrippedDownTower* t =new StrippedDownTower(base_thresh, (StrippedDownTower::Calorimeter) which_calo); 
		for(auto x: phi_edges){
			float phi_low=x.second.first;
			float phi_up=x.second.second;
			if(j[1] >= phi_low && j[1] < phi_up){
				t->tag=(StrippedDownTower::Regions)x.first;
				break;
			}
			else continue;
		}
		t->r=r;
		t->phi=j[1];
		t->eta=j[0];
		t->E=j_val;
		strippedCalo.push_back(t);
		++i;
		//std::cout<<"Towers is now : " <<(int)strippedCalo.size() <<std::endl; 
	}
	//Now get the energy regions
	int n_valid_towers=0;
	for(auto* t:strippedCalo)
	{
		StrippedDownTower::Regions region_tag=t->tag;
		if(t->E < t->RegionOutput->threshold) break; //because it is ordered
		else{
			e_region.at(region_tag)+=t->E;
			e_region.at(StrippedDownTower::Regions::Full)+=t->E;
			n_valid_towers++;	
		}
	}
	std::cout<<"The thresholds and their number of towers are \n Threshold     Towers" <<std::endl;
	if(n_valid_towers <= 0 ) return; 
	std::cout<<strippedCalo.at(0)->RegionOutput->threshold <<":    " <<n_valid_towers <<std::endl;
	//now I need to actualy run the dataset 
	std::vector<std::thread> CalculatingThreads;
	//std::cout<<"Calo object has size " <<strippedCalo.size() <<std::endl;
	for(int i=0; i<(int)strippedCalo.size()-1; i++)
	{
		if(i%200 == 0 ) std::cout<<"Prepping tower for threading : " <<i <<std::endl;
		StrippedDownTower* t = strippedCalo.at(i);
		std::vector<StrippedDownTower> CaloCopy;
		for(int j=i+1; j<(int)strippedCalo.size(); j++) CaloCopy.push_back(*(strippedCalo[j])); //avoid double counting
//		CalculateENC(t, CaloCopy, transverse, energy);
		CalculatingThreads.push_back(std::thread(&LargeRLENC::CalculateENC, this, t, CaloCopy, transverse, energy));
	}
	std::cout<<"Performing caluclation" <<std::endl;
	for(int k=0; k<(int)CalculatingThreads.size(); k++) CalculatingThreads.at(k).join();
	//now need to sum over the relevant towers
	CalculatingThreads.clear(); //just trying to find memmory leaks
/*	std::array<std::vector<TowerOutput*>, 5> AllTowOutput;
	for(int i=0; i< 5; i++) {
		std::vector<TowerOutput*> a;
		for(auto t:thresholds){
			a.push_back(new TowerOutput(t));
		}
		AllTowOutput[i]=a;
	} //just get it the right range */
	std::array<int, 5> n_entries_arr;
//	std::array<std::vector<std::vector<TowerOutput*>>,5> ToMerge; 
//	std::array<std::vector<std::set<float>>,5> ToMergeRl; 
//	std::array<std::vector<std::set<std::array<float,3>>>,5> ToMergeRlRmRs; 
	for(int n=0; n<5; n++)
		n_entries_arr[n]=0;
//	for(int n=0; n<(int)ToMerge.size(); n++)
	//	for(int i=0; i < (int) arr_size; i++) 
		//	ToMerge[n].push_back(std::vector<TowerOutput*>());
/*	for(int n=0; n<(int)ToMergeRl.size(); n++)
		for(int i=0; i < (int) arr_size; i++) 
			ToMergeRl[n].push_back(std::set<float>());
	for(int n=0; n< (int)ToMergeRlRmRs.size(); n++)
		for(int i=0; i < (int) arr_size; i++) */
	//		ToMergeRlRmRs[n].push_back(std::set<std::array<float, 3>>());
	std::cout<<"Max elements: " <<strippedCalo.size()<<std::endl;
	for(int i=0; i<(int)strippedCalo.size(); i++)
	{
			if(strippedCalo.at(i)->FullOutput->RL.size() > 0 )
				 n_entries_arr[0]++;
			//ToMerge[0][j].push_back(strippedCalo.at(i)->FullOutput->at(j));
		//	for(auto r:strippedCalo.at(i)->FullOutput->at(j)->RL)ToMergeRl[0][j].insert(r);
		//	for(auto r:strippedCalo.at(i)->FullOutput->at(j)->RL_RM_RS)ToMergeRlRmRs[0][j].insert(r);
					
		
		if(strippedCalo.at(i)->RegionOutput->RL.size() > 0 )
			 n_entries_arr[(int)strippedCalo.at(i)->tag]++;
		//	int tagn=strippedCalo.at(i)->tag;
		//	ToMerge[tagn][j].push_back(strippedCalo.at(i)->RegionOutput->at(j));
		//	for(auto r:strippedCalo.at(i)->RegionOutput->at(j)->RL)ToMergeRl[tagn][j].insert(r);
		//	for(auto r:strippedCalo.at(i)->RegionOutput->at(j)->RL_RM_RS)ToMergeRlRmRs[tagn][j].insert(r);
			
		
	}
	std::cout<<"Plotting Energy " <<std::endl;
	if(which_calo == LargeRLENC::Calorimeter::TRUTH)
	{
		for(int region=0; region < 4; region++)
		{
			Truth_Region_vector[region][0]->E->Fill(e_region[region]);
			if(region < 3 )
				Truth_Region_vector[region][0]->N->Fill(n_entries_arr[region]);
			else if(region == 3)
				Truth_Region_vector[region][0]->N->Fill(n_entries_arr[region]+n_entries_arr[region+1]);
		}
		for(auto Tow:strippedCalo)
		{
			int region=Tow->tag;
			//std::cout<<"Threshold index " <<threshold_index<<std::endl;
			TowerOutput* TF=Tow->FullOutput;
			TowerOutput* TR=Tow->RegionOutput;
			int region_shift=region;
			if(region_shift > 3 ) region_shift=3;
			TF->Normalize(e_region[region_shift]);
			TR->Normalize(e_region[region_shift]);
			auto Hists=Truth_Region_vector[region_shift][0];
			auto FullHists=Truth_Region_vector[0][0];
			if((int)TF->RL.size() > 0 ){
				for(int i=0; i<(int)TF->RL.size(); i++){
					FullHists->R->Fill(TF->RL.at(i));
					FullHists->E2C->Fill(TF->RL.at(i), TF->E2C.at(i));
					if((int) TF->E3C.size() <= i ) continue; 
						else FullHists->E3C->Fill(TF->RL.at(i), TF->E3C.at(i));
				}
			}
			if((int)TR->RL.size() > 0 ){
				for(int i=0; i<(int)TR->RL.size(); i++){
					Hists->R->Fill(TR->RL.at(i));
					Hists->E2C->Fill(TR->RL.at(i), TR->E2C.at(i));
						if((int) TR->E3C.size() <= i ) continue; 
						else Hists->E3C->Fill(TR->RL.at(i), TR->E3C.at(i));
				}
			}	
		}
		strippedCalo.clear();
		return;
	}
	if(which_calo == LargeRLENC::Calorimeter::TRUTH) return;
	for(int region=0; region < 4; region++)
	{
		Region_vector[region][which_calo]->E->Fill(e_region[region]);
		if(region < 3 )
			 Region_vector[region][which_calo]->N->Fill(n_entries_arr[region]);
		else if(region == 3)
			Region_vector[region][which_calo]->N->Fill(n_entries_arr[region]+n_entries_arr[region+1]);
	}
	std::cout<<"Plotting all the values" <<std::endl;
	
	for(auto Tow:strippedCalo)
	{
		int region=Tow->tag;
		TowerOutput* F=Tow->FullOutput;
		TowerOutput* R=Tow->RegionOutput;
		int region_shift=region;
		if(region_shift > 3 ) region_shift=3;
		F->Normalize(e_region[region_shift]);
		R->Normalize(e_region[region_shift]);
		auto Hists=Region_vector[region_shift][which_calo];
		auto FullHists=Region_vector[0][which_calo];
		if((int)F->RL.size() > 0 ){
			for(int i=0; i<(int)F->RL.size(); i++){
				FullHists->R->Fill(F->RL.at(i));
				FullHists->E2C->Fill(F->RL.at(i), F->E2C.at(i));
				if((int) F->E3C.size() <= i ) continue; 
				else FullHists->E3C->Fill(F->RL.at(i), F->E3C.at(i));
				}
		}
		if((int)R->RL.size() > 0 ){
			for(int i=0; i<(int)R->RL.size(); i++){
				Hists->R->Fill(R->RL.at(i));
				Hists->E2C->Fill(R->RL.at(i), R->E2C.at(i));
				if((int) R->E3C.size() <= i ) continue; 
				else Hists->E3C->Fill(R->RL.at(i), R->E3C.at(i));
			}
		}	
	}
	std::cout<<"Plotted all the values" <<std::endl;
	strippedCalo.clear();
/*	std::vector<std::thread> Merger_thread; //not merging, it was too slow, trying to just fill direct now
	for(int i=0; i<5; i++)
	{
		for(int j=0; j<(int)arr_size; j++)
		{
			Merger_thread.push_back(std::thread(&LargeRLENC::Merger, this, AllTowOutput[i][j], ToMerge[i][j], ToMergeRl[i][j], ToMergeRlRmRs[i][j]));
		}
	}
	std::cout<<"Merging output" <<std::endl;	
	//for(int k=0; k<(int)Merger_thread.size(); k++) Merger_thread.at(k).join(); 
	std::cout<<"Saving to plots" <<std::endl;	
	//now push the data out to the plots
	for(int region_index=0; region_index<(int)AllTowOutput.size(); region_index++)
	{
		for(int threshold_index=0; threshold_index<(int)AllTowOutput[region_index].size(); threshold_index++)
		{
			if(threshold_index >= (int)Region_vector.size()) continue;
			int region_shift=region_index;
		//	std::cout<<"Threshold index is : " <<threshold_index <<" Region: " << region_shift <<std::endl;
			if(region_shift >= 3 ) region_shift=3;
			AllTowOutput[region_index]->CalculateFlatE3C();
			AllTowOutput[region_index]->Normalize(e_region[region_shift]);
			auto Hists=Region_vector[region_shift][which_calo];
			auto Output=AllTowOutput[region_index];
			if(region_index < 3) Hists->N->Fill(n_entries_arr[region_index]);
			else if (region_index==3)Hists->N->Fill(n_entries_arr[region_index]+n_entries_arr[region_index + 1]);
			else n_entries_arr[region_index]=0;
			int tower_number=(int)Output->RL.size();
//			std::cout<<"number of entries to fill is " <<(int)Output->E2C.size() <<std::endl;
			if(tower_number > 0){
				for(int rl_index=0; rl_index < tower_number; rl_index++)
				{
					Hists->R->Fill(Output->RL.at(rl_index));
					Hists->E2C->Fill(Output->RL.at(rl_index), Output->E2C.at(rl_index));
					if((int)Output->E3C.size() > rl_index) Hists->E3C->Fill(Output->RL.at(rl_index), Output->E3C.at(rl_index));
				}
			}
			Hists->E->Fill(e_region[region_shift]);
		}
	}*/
	 
	return;
	
}
void LargeRLENC::Merger(TowerOutput* output, std::vector<TowerOutput*> inputs, std::set<float> RLs, std::set<std::array<float, 3>> Rlmss)
{
	std::map<float, float> e2c;
	std::map<std::array<float, 3>, float> e3c;
	for(auto r:RLs) e2c[r]=0.;
	for(auto r:Rlmss) e3c[r]=0.;
	for(auto t:inputs)
	{
		for(int i=0; i <(int) t->RL.size(); i++)
		{
			e2c[t->RL.at(i)]=t->E2C.at(i);
		}
		for(int i=0; i< (int) t->RL_RM_RS.size(); i++)
		{
			e3c[t->RL_RM_RS.at(i)]=t->E3C_full_shape.at(i);
		}
	}
	for(auto er:e2c) output->AddE2CValues(er.second, er.first);
	for(auto er:e3c) output->AddE3CValues(er.second, er.first);
	
	return;
}
void LargeRLENC::CalculateENC(StrippedDownTower* tower1, std::vector<StrippedDownTower> towerSet, bool transverse, bool energy)
{
	//this is the threaded object, I could try nesting the threads, but for now I am going to try to run without the nested threads for the 3pt
	float thresh=tower1->RegionOutput->threshold;
	for(int i = 0; i<(int) towerSet.size(); i++){
		//run over all other towers
		StrippedDownTower* tower2=&towerSet.at(i); 
		float R_L=getR(tower1->eta, tower1->phi, tower2->eta, tower2->phi);
		float e2c=0.;
		if(energy) e2c=(tower1->E) * (tower2->E);
		else e2c=(tower1->E * ptoE) * (tower2->E * ptoE); //use a basic swap of momenta ratio for each calo
		if(transverse) e2c=e2c/(cosh(tower1->eta) * cosh(tower2->eta));
		float energy1=tower1->E, energy2=tower2->E;
		int region1=tower1->tag, region2=tower2->tag;
		bool sameRegion = ( region1 == region2);
		std::vector<std::pair<std::pair<std::array<float, 3>, std::pair<float, float>>, bool>> e3c_relevant;
		if(i != (int) towerSet.size() - 1){
			for(int j=i+1; j<(int) towerSet.size(); j++){
				StrippedDownTower* tower3=&towerSet.at(j);
				float R_13=getR(tower1->eta, tower1->phi, tower3->eta, tower3->phi);
				float R_23=getR(tower2->eta, tower2->phi, tower3->eta, tower3->phi);
				if(R_13 > R_L || R_23 > R_L ) continue;
				else{
					float Rs = std::min(R_13, R_23);
					float Rm = std::max(R_13, R_23);
					std::array<float, 3> R {R_L, Rm, Rs}; 
					float energy3=tower3->E;
					float e3c=1;
					if(energy) e3c=e2c*energy3;
					else e3c=e2c* (energy3*	ptoE);
					if(transverse) e3c=e3c/cosh(tower3->eta);
					std::pair<float, float> energy_e3 {energy3, e3c};
					std::pair<std::array<float, 3>, std::pair<float, float>> e3c_sing {R, energy_e3};
					bool sameRegion3 = ( region1 == tower3->tag);
					e3c_relevant.push_back(std::make_pair(e3c_sing, sameRegion3));
				}
			}
		} //caluclate and store the e3c to easily iterate over
		if(energy1 > thresh && energy2 > thresh){
			tower1->FullOutput->AddE2CValues(e2c, R_L);
			//std::cout<<"Adding pair " <<R_L <<", "<<e2c <<std::endl;
			if(sameRegion)tower1->RegionOutput->AddE2CValues(e2c, R_L); //this is the two point done noew 
			for(auto t3:e3c_relevant)
			{
				if(t3.first.second.first > thresh){
					tower1->FullOutput->AddE3CValues(t3.first.second.second, t3.first.first);
					if(t3.second) tower1->RegionOutput->AddE3CValues(t3.first.second.second, t3.first.first);
					}
				}	
			}	
		
	}//caluclate the flattend e3c
	tower1->FullOutput->CalculateFlatE3C();
	tower1->RegionOutput->CalculateFlatE3C();

	return;
}
void LargeRLENC::JetEventObservablesBuilding(std::array<float, 3> central_tower, std::map<std::array<float, 3>, float> calorimeter_input, std::map<float, float>* Etir_output )
{
	//This is the Jet Event observable. Ideally I would seperate this out 
	//Input is of the form <tower r, tower eta, tower phi>, tower E (vertex corrected)
	//Output is <tower r, tower eta, tower phi>, <R=0.1-1.0, ETir> 
<
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

	float dphi=std::abs(phi1-phi2);
	if(std::abs(dphi) > PI ) dphi+=-PI;
	float rsq=std::pow(deta, 2)+ std::pow(dphi, 2);
	if(print)std::cout<<" The value for R squared is square of "<<dphi <<" + square of " <<deta <<" = "<<rsq <<std::endl;
	return std::sqrt(rsq);

}
void LargeRLENC::Print(const std::string &what) const
{
	TFile* f1=new TFile(output_file_name.c_str(), "RECREATE");

	float pct=(float) n_with_jets / (float) n_evts;
	pct=pct*100.; 
	TH1F* h_percent=new TH1F("h_pct", "Percentage of events with a non-empty jet container; Percent; N_{files}", 100, -0.5, 99.5);
	h_percent->Fill(pct);
//	h_percent->Write();
	TDirectory* dir_evt=f1->mkdir("event_categorization");
	dir_evt->cd();
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
	std::vector<TDirectory*> region_dirs;
	std::cout<<__LINE__<<std::endl;
	auto FullCal=Region_vector.at(0);
	auto TowardRegion=Region_vector.at(1);
	auto AwayRegion=Region_vector.at(2);
	auto TransverseRegion=Region_vector.at(3);
	TDirectory* dir_full=f1->mkdir("Full_Calorimeter");
	TDirectory* dir_towrd=f1->mkdir("Towards_Region");
	TDirectory* dir_away=f1->mkdir("Away_Region");
	TDirectory* dir_trans=f1->mkdir("Transverse_Region");
	std::cout<<__LINE__<<std::endl;
	std::vector<TDirectory*> tds {dir_full, dir_towrd, dir_away, dir_trans};
	if(!isRealData && !pedestalData)
	{
		for(int i=0; i<(int)Truth_Region_vector.size(); i++)
		{
			tds.at(i)->cd();
			auto hr=Truth_Region_vector.at(i);
			for(auto hv:hr){
				hv->R_pt->Write();
				hv->E2C->Write();
				hv->E3C->Write();
				hv->R->Write();
				hv->N->Write();
				hv->E->Write();
				hv->pt->Write();
			}
		}
	}	
		dir_full->cd();
		std::cout<<__LINE__<<std::endl;
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
		dir_trans->cd();
		std::cout<<__LINE__<<std::endl;
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
		dir_away->cd();
		std::cout<<__LINE__<<std::endl;
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
		dir_towrd->cd();
		std::cout<<__LINE__<<std::endl;
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
		
		
	std::cout<<__LINE__<<std::endl;
	f1->cd();
	f1->Write();
	std::cout<<__LINE__<<std::endl;

	f1->Close();
	return;	
}
