//////////////////////////////////////////////////////////////////////////////////////////////////
//											  	//
//			Large R_L Calorimeter ENC						//
//												//
//			Author: Skadi Grossberndt						//
//			Depends on: Calorimeter Tower ENC 					//
//			First Commit date: 18 Oct 2024						//
//			Most recent Commit: 23 Oct 2024						//
//			version: v0.1								//
//												//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "LargeRLENC.h"

LargeRLENC::LargeRLENC(const int n_run/*=0*/, const int n_segment/*=0*/, const float jet_min_pT/*=0*/, const bool data/*=false*/,const std::string vari/*="E"*/, const std::string& name/* = "LargeRLENC"*/) 
{
	std::vector<MethodHistograms*> hf {new MethodHistograms("Full_CAL", 3.9), new MethodHistograms("Full_EMCAL", 3.9), new MethodHistograms("Full_IHCAL", 3.9), new MethodHistorgams("Full_OHCAL", 3.9)}; //Full OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~pi)
	std::vector<MethodHistograms*> ht {new MethodHistograms("Towards_Region_CAL", 3.9), new MethodHistograms("Towards_Region_EMCAL", 3.9), new MethodHistograms("Towards_Region_IHCAL", 3.9), new MethodHistorgams("Towards_Region_OHCAL", 3.9)}; //Towards_Region OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~pi)
	std::vector<MethodHistograms*> ha {new MethodHistograms("Away_Region_CAL", 3.9), new MethodHistograms("Away_Region_EMCAL", 3.9), new MethodHistograms("Away_Region_IHCAL", 3.9), new MethodHistorgams("Away_Region_OHCAL", 3.9)}; //Away_Region OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~pi)
	std::vector<MethodHistograms*> htr {new MethodHistograms("Transverse_Region_CAL", 3.9), new MethodHistograms("Transverse_Region_EMCAL", 3.9), new MethodHistograms("Transverse_Region_IHCAL", 3.9), new MethodHistorgams("Transverse_Region_OHCAL", 3.9)}; //Transverse_Region OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~pi)

	this->FullCal=&hf;
	this->TowardRegion=&ht;
	this->AwayRegion=&ha;
	this->TransverseRegion=&htr;
	//while the Towards and away region arent't so extensive, haveing Rmax ~ 3, but close enough, transverse region is going to have some discontinuities
	this->isRealData=data; //check if the data is real data, if not run the truth as a cross check 
	this->nRun=n_run;
	this->nSegment=n_segment;
	this->jetMinpT=jet_min_pT;
	this->algo="Tower";
	this->radius="r04";
	this->eventCut=new EventSelectionCut;
	this->which_variable=vari;
	DijetQA=new TTree("DijetQA", "Dijet Event QA");
	DijetQA->Branch("N_jets", &m_Njets);
	DijetQA->Branch("x_j_L",  &m_xjl);
	DijetQA->Branch("A_jj_L", &m_Ajjl);
	DijetQA->Branch("Jet_PT", &m_pt, 128000, "pt/F");
	DijetQA->Branch("Dijet_sets", &m_dijets, 128000, "pt_l/F:Ajj/F:xj/F"); 
	EEC=new TTree("EEC", "Energy Correlator");
	EEC->Branch("E_Total",   &m_etotal);
	EEC->Branch("E_CEMC",    &m_eemcal);
	EEC->Branch("E_IHCAL",   &m_eihcal);
	EEC->Branch("E_OHCAL",   &m_eohcal);
	EEC->Branch("vertex",    &m_vertex);
	EEC->Branch("2_pt",      &m_e2c, 128000, "region/C:calo/C:r_l/F:e2c/F");
	EEC->Branch("3_pt",      &m_e3c, 128000, "region/C:calo/C:r_l/F:e3c/F");
	EEC->Branch("3_pt_full", &m_e3c_full, 128000, "region/C:calo/C:r_12/F:r_13/F:r_23/F:e3c/F");
	JetEvtObs=new TTree("JetEvtObs", "Tree for Event shape jet observable studies");
	JetEvtObs->Branch("p_T", m_pt_evt, 128000, "region/C:calo/C:eta/F:phi/F:pt/F");

}
JetContainerv2* LargeRLENC::getJets(std::string algo, std::string radius, std::array<float, 3> vertex, PHCompositeNode* topNode)
{
	//This is just running the Fastjet reco 
	fastjet::JetReco		
	
}
void LargeRLENC::addTower(int n, TowerInfoContainer* energies, RawTowerGeomContainer_Cylinderv1* geom, std::map<std::array<float, 3>, float>* towers, RawTowerDefs::CalorimeterId td)
{
	geom->set_calorimeter_id(td);
	auto key=energies->encode_key(n);
	auto tower=energies->get_tower_at_channel(n);
	int phibin=energies->getTowerPhiBin(key);
	int etabin=energies->getTowerEtabin(key);
	float phicenter=geom->get_phicenter(phibin);
	float etacenter=geom->get_etacenter(etabin);
	float r=geom->getradius();
	std::array<float, 3> center {etacenter, phicenter, radius};
	(*towers)[center]=tower->get_energy();
	return;
}
int LargeRLENC::process_event(PHCompositeNode* topNode)
{
	//This is where I can be a bit clever with allowing for the parrelization for easier crosschecks
	n_evts++;
	auto emcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_CEMC"   );
	auto emcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC"   );
	auto ihcal_geom= findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN");
	auto ihcal_tower_energy= findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
	auto ohcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");
	auto ohcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
	//look for the jet objects 
	JetContainerv2* jets=NULL;
	bool foundJetConts=false, isDijet=false;
	try{
		if(!isRealData) jets = findNode::getClass<JetContainer>(topNode, "AntiKt_Truth_r04"); //look for the r_04 truth jets
		else{
			std::string recoJetName="AntiKt_"+algo+radius;
			jets = findNode::getClass<JetContainer>(topNode, recoJetName); //check for already reconstructed jets
		}
	}
	catch(std::exception& e){ std::cout<<"Did not find a jet container object, will attempt to reconstruct the jets" <<std::endl;}
	std::array<float, 3> vertex={0.,0.,0.}; //set the initial vertex to origin 
	try{
		GlobalVertexMap* vertexmap=findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
		if(vertexmap){
			if(vertexmap->empty())
				std::cout<<"Empty Vertex Map. \n Setting vertex to origin" <<std::endl; 
			else{
				GlobalVertex* gl_vtx=nullptr;
				for(auto vertex_iter:vertexmap) gl_vtx=vertex_iter->second;
				if(gl_vtx){
					vertex[0]=gl_vtx->get_x();
					vertex[1]=gl_vtx->get_y();
					vertex[2]=gl_vtx->get_z();
				}
			}
		}
	}
	catch(std::exception& e){std::cout<<"Could not find the vertex. \n Setting to origin" <<std::endl;}
	if(jets==NULL) jets=getJets(algo, radius, vertex);
	if(!jets){
		std::cout<<"Didn't find any jets, skipping event" <<std::endl;
		return -1/*FUN4ALL::*/;
	}
	float emcal_energy=0., ihcal_energy=0, ohcal_energy=0, total_energy=0, ohcal_rat=0;
	std::map<std::array<float, 3>, float> *ihcal_towers, *emcal_towers, *ohcal_towers; //these are just to collect the non-zero towers to decrease the processing time 
	for(int n=0; n<(int) emcal_tower_energy->size(); n++){
		float energy=emcal_tower_energy->get_tower_at_channel(n)->get_energy();
		if(energy > 0.01)//put zero supression into effect
			addTower(n, emcal_tower_energy, emcal_geom, emcal_towers, RawTowerDefs::CEMC   );
		emcal_energy+=energy;
	}
	for(int n=0; n<(int) ihcal_tower_energy->size(); n++){
		float energy+=ihcal_tower_energy->get_tower_at_channel(n)->get_energy();
		if(energy > 0.01) //zero suppression is at 10 MeV in IHCAL
			addTower(n, ihcal_tower_energy, ihcal_geom, ihcal_towers, RawTowerDefs::HCALIN  );
		ihcal_energy+=energy;
	}
	for(int n=0; n<(int) ohcal_tower_energy->size(); n++){
		float energy+=ohcal_tower_energy->get_tower_at_channel(n)->get_energy();
		if(energy > 0.01) //10 MeV cutoff 
			addTower(n, ohcal_tower_energy, ohcal_geom, ohcal_towers, RawTowerDefs::HCALOUT );
		ohcal_energy+=energy;
	}
	total_energy=emcal_energy+ihcal_energy+ohcal_energy;
	ohcal_rat=ohcal_energy/total_energy; //take the ratio at the whole calo as that is the region of interest
	float emcal_occupancy=emcal_towers->size()/((float)emcal_tower_energy->size());
	float ihcal_occupancy=ihcal_towers->size()/((float)ihcal_tower_energy->size());
	float ohcal_occupancy=ohcal_towers->size()/((float)ohcal_tower_energy->size());
	emcal_occup->Fill(emcal_occupancy); //filling in the occupancy plots for easy QA while going 
	ihcal_occup->Fill(ihcal_occupancy);
	ohcal_occup->Fill(ohcal_occupancy);

	isDijet=eventCut->passesTheCut(jets, ohcal_rat, vertex);	
	if(!isDijet){ //stores some data about the bad cuts to look for any arrising structure
		if(ohcal_rat > 0.9){
		       ohcal_rat_occup->Fill(ohcal_rat, ohcal_occupancy);
		       for(auto p:ohcal_towers)ohcal_bad_hits->Fill(p.first.first, p.first.second, p.second);
		}
		bad_occ_em_oh->Fill(emcal_occupancy, ohcal_occupancy);
		bad_occ_em_h->Fill(emcal_occupancy, (ohcal_occupancy+ihcal_occupancy)/2.0);
	       	return -1;
	}
	else{
		float lead_phi=eventCut->GetLeadPhi();
		eventCut->getDijets(jets, &m_dijets);
		m_vertex=vertex;
		//this is running the actual analysis
		LargeRLENC::CaloRegion(*emcal_towers, *ihcal_towers, *ohcal_towers, 0, FullHCal,         jetMinpT, which_variable, vertex, lead_phi);
		LargeRLENC::CaloRegion(*emcal_towers, *ihcal_towers, *ohcal_towers, 1, TowardRegion,     jetMinpT, which_variable, vertex, lead_phi);
		LargeRLENC::CaloRegion(*emcal_towers, *ihcal_towers, *ohcal_towers, 2, AwayRegion,       jetMinpT, which_variible, vertex, lead_phi);
		LargeRLENC::CaloRegion(*emcal_towers, *ihcal_towers, *ohcal_towers, 3, TransverseRegion, jetMinpT, which_variable, vertex, lead_phi);
		DijetQA->Fill();

	}
	return 1;
}
void LargeRLENC::CaloRegion(std::map<std::array<float, 3>, float> emcal, std::map<std::array<float, 3>, float> ihcal, std::map<std::array<float, 3>, float> ohcal, int region, std::vector<MethodHistograms*> *Histos, float jetMinpT, std::string variable_of_intrest, std::array<float, 3> vertex, float lead_jet_center_phi)
{ 
	//this is the actual analysis object, the TTree for the event variables is actually just a dumy, no analysis, just save the variables into it for future usage
	bool transverse=false;
	bool energy=true;
	float phi_min=0., phi_max=2*PI;
	switch(variable_of_interest)
	{
		case "E":
			transverse=false;
			energy=true;
			break;
		case "p":
			transverse=false;
			energy=false;
			break;
		case "E_T":
			transverse=true;
			energy=true;
			break;
		case "p_T":
			transverse=true;
			energy=false;
			break;
	}
	switch(lead_jet_center_phi)
	{
		case 0: //go over the full calorimeter
			phi_min=0;
			phi_max=2*PI;
			break;
		case 1: //forward region of the detector
			phi_min=lead_jet_center_phi - PI/3;
			phi_max=lead_jet_center_phi + PI/3;
			break;
		case 2: //away region of the detector
			phi_min=lead_jet_center_phi + PI - PI/3;
			phi_max=lead_jet_center_phi + PI + PI/3;
			break;
		case 3: //transverse regions, needs to done cleverly to get both halfs
			phi_min=lead_jet_center_phi + PI/3;
			phi_max=lead_jet_centet_phi + PI - PI/3; 
			break;
	}
	//make sure the bounds are within the region of 0->2*PI
	if(phi_min < 0) phi_min+=2*PI;
	if(phi_max < 0) phi_max+=2*PI;
	if(phi_min > 2*PI) phi_min+=-2*PI;
	if(phi_max > 2*PI) phi_max+=-2*PI;
	float total_e=0.;
	SingleCaloENC(emcal, jetMinpT, vertex, region, transverse, energy, phi_min, phi_max, Histos, 1, &total_e);
	SingleCaloENC(ihcal, jetMinpT, vertex, region, transverse, energy, phi_min, phi_max, Histos, 2, &total_e);
	SingleCaloENC(ohcal, jetMinpT, vertex, region, transverse, energy, phi_min, phi_max, Histos, 3, &total_e);
	EEC->Fill();
	JetEvtObs->Fill();
	return;
}
void LargeRLENC::SingleCaloENC(std::map<std::array<float, 3>, float> cal, float jetMinpT, std::array<float, 3> vertex, int region, bool transverse, bool energy, float phi_min, float phi_max, std::vector<MethodHistograms*>* Histos, int which_calo, float* total_e_region)
{
	MethodHistograms* ha=(*Histos)[0], hc=(*Histos)[which_calo];
	auto i=cal.begin();
	std::map<std::pair<float, float>, float> p_T_save;
	float e_region=0.; //get the energy in the relevant region 
	while(i !=cal.end()){ //restrict the region to the region of intrest to reduce the data size ahead of fact
		float pT=0., eta_shift=i->first[0], r=i->first[2];
		float x=r*cos(i->first[1]), y=r*sin(i->first[1]);
		if(vertex[0]!=0) x+=vertex[0];
		if(vertex[1]!=0) y+=vertex[1];
		r=sqrt(pow(x,2) + pow(y,2));
		float z=r*tanh(eta_shift);
		if(vertex[2]!=0)z+=vertex[3];
		eta_shift=atanh2(z,r); //did the vertex shift
		i->first[0]=etashift;
		if(vertex[0] != 0 && vertex[1] !=0) i->first[1]=atan2(y,x);
		i->first[2]=r; //make sure the positions are properly aligned for all caluclations	
		if(i->first[1] < phi_min || i->first[1] > phi_max)i=cal.erase(i);
		else{
			e_region+=i->second;
		       	++i;
			pT=i->second; //right now using p approx E, can probably improve soon
			pT=pT/cosh(eta_shift);
			if(p_T > jetMinpT) p_T_save[i->first]=pT;
			hc->pt->Fill(p_T);
			float R=getR(i->first[0], i->first[1], eventCut->getLeadEta(), -eventCut->getLeadPhi());
			hc->R_pt->Fill(R,p_T);
		}
	}
	std::map<std::pair<float, float>, float> cal_2=cal;
	std::map<float, float> e2c, e3c;
	std::map<std::array<float, 3>, float> e3c_full;
	hc->N->Fill((int)cal.size());
	hc->E->Fill(e_region);
	(*total_e_region)+=e_region;
	std::cout<<"Doing over " <<cal.size() <<" events" <<std::endl;
	for(auto i=cal.begin(); i != cal.end(); ++i){
		auto j=cal_2.find(i->first);
		if(j != cal_2.end()) 
			cal_2.erase(j); //getting rid of double counting by removing the base value from the second map 
		auto cal_3=cal_2
		auto k=cal_3.find(j->first);
		if(k != cal_3.end()) 
			cal_3.erase(k); //getting rid of double counting by removing the base value from the second map 
		std::vector<std::pair<float, std::pair<float, float> > > point_correlator ((int)cal_2.size()); //allow for easy looping over the towers to parrelize
		std::vector< std::pair<float, std::vector< std::pair< std::pair<float, float>, float > > > > 3pt_full ((int) cal_2.size());
		std::vector<std::thread> calculating_threads; 
		for(int l=0; l<(int)cal_2.size(); k++)
		{
		 	//threading over all pairs to allow for faster calculation 
			if(cal_2[k] < MinpTComp) continue;
			calculating_threads.push_back(std::thread(&LargeRLENC::CalculateENC, this, *i, cal_2[k], cal_3, &point_correlators[k], &3pt_full[k], transverse, energy)); 
		}
		for(auto t:calculating_threads) t.join();
		for(auto v:point_correlator){
			auto l=e2c.find(v->first);
			auto m=e3c.find(v->first);
			if(l == e2c.end()) e2c[v->first]=v->second.first;
			if(l != e2c.end()) e2c[v->first]+=v->second.first;
			if(m == e3c.end()) e2c[v->first]=v->second.second;
			if(m != e3c.end()) e2c[v->first]+=v->second.second;
			ha->R->Fill(v->first);
			hc->R->Fill(v->first);
		}
		for(auto v:3pt_full){
			for(auto w:v->second){
				std::array<float, 3> key= {v->first, w->first.first, w->first.second};
				auto l=e3c_full.find(key);
				if(l == e3c_full.end()) e3c_full[key]=w->second;
				else e3c_full[key]+=w->second;
			}
		}
	}
	for(auto v:e2c){
		v->second=v->second*pow(e_region, -2);
	       	hc->E2C->Fill(v->first, v->second);
	}
	for(auto v:e3c){
		v->second=v->second*pow(e_region, -3);
	//	hc->E3C->Fill(v->first, v->second);
	}
	for(auto v:e3c_full)
	{
		v->second=v->second*pow(e_region, -3);
		float R_L=std::max(v->first[0], v->first[1]);
		R_L=std::max(R_L, v->first[2]);
		hc->E3C->Fill(R_L, v->second);
	}
	m_e2c[which_calo-1]=e2c;
	m_e3c[which_calo-1]=e3c;
	m_e3c_full[which_calo-1]=e3c_full;
	m_pt[which_calo-1]=p_T_save;
	return;
}
void LargeRLENC::CalculateENC(std::pair<std::array<float, 3>, float> point_1, std::pair<std::array<float, 3>, float> point_2, std::map<std::array<float, 3>, float> cal, std::pair<float, std::pair<float, float>>* enc_out, std::pair<float, std::vector< std::pair< std::pair<float, float>, float > > > > *3pt_full, bool transverse, bool energy)
{
	//this is the threaded object, I could try nesting the threads, but for now I am going to try to run without the nested threads for the 3pt
	float R_L=getR(point_1.first[0], point_1.first[1], point_2.first[0], point_2.first[1]);
	float e2c=0.;
	if(energy) e2c=(point_1->second) * (point_2->second);
	else e2c=(point_1->second * ptoE) * (point_2->second * ptoE); //use a basic swap of momenta ratio for each calo
	if(transverse) e2c=e2c/(cosh(point_1.first[0]) * cosh(point_2.second->first[0]));
	enc_out->first=R_L;
	enc_out->second.first=e2c;
	enc_out->second.second=0.;
	3pt_full->first=R_L;
	for(auto i:cal)
	{
		float R_13=getR(point_1.first[0], point_1.first[1], i->first[0], i->first[1]);
		float R_23=getR(point_2.first[0], point_2.first[1], i->first[0], i->first[1]);
		float maxRL=std::max(R_L, R_13); //so I need to change the output sturcutre because I'm not sure if I am matching correctly
		maxRL=std::max(maxRL, R_23);
		else{
			float e3c=e2c*i->second;
			if(!energy) e3c=e3c*ptoE;
			if(transverse) e3c=e3c/(cosh(i->first[0]));
			if(maxRL==R_L)enc_out->second.second+=e3c;
			std::pair<float, float> rs {R_13, R_23};
			std::pair<std::pair<float, float>, float> ec {rs, e3c};
			3pt_full->second.push_back(ec);
		}
	}
	return;
}
float LargeRLENC::getR(float eta1, float phi1, float eta2, float phi2)
{
	float deta=eta1-eta2; 
	float dphi=phi1-phi2;
	float rsq=pow(deta, 2)+ pow(dphi, 2);
	return sqrt(rsq);
}
