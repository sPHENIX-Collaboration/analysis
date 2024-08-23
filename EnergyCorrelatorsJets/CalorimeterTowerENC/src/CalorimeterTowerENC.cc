//////////////////////////////////////////////////////////////////////////////////////////////////
//												//
//												//
//		Calorimeter Tower Jets N Point-Energy Correlator Study				//
//		Author: Skadi Grossberndt							//
//		Date of First Commit: 12 July 2024						//
//		Date of Last Update:  22 August 2024						//
//		version: v1.1									//
//												//
//												//
//		This project is a study on energy correlators using particle jets and 		//
//	jets  definied over calorimeter towers. Feasibility study to evaluate pp prospects	//
//	for sPHENIX in 2024 with minimal tracking and momentum resolution 			//
//												//
//////////////////////////////////////////////////////////////////////////////////////////////////


#include "CalorimeterTowerENC.h"
 
CalorimeterTowerENC::CalorimeterTowerENC(int n_run, int n_segment, const std::string &name){
	//this is the constructor
	n_evts=0;
	std::stringstream run, seg;
	run.width(10);
	run.fill('0');
	run <<n_run;
	seg.width(5);
	seg.fill('0');
	seg << n_segment;
	outfilename="Calorimeter_tower_ENC-"+run.str()+"-"+seg.str()+".root";
	jethits=new TH2F("jethits", "Location of particle energy deposition from truth jets; #eta; #varphi; N_{hits}", 200, -1.15, 1.05, 200, -3.1416, 3.1415);
	comptotows=new TH2F("comp_to_towers", "Particle energy deposition versus EMCal tower energy per EMCAL tower; E_{particle} [GeV]; E_{emcal} [GeV]; N", 50, -0.5, 49.5, 50, -0.05, 4.95 ); 
	number_of_jets=new TH1F("nJ", "Number of Jets per event; N_{jet}", 100, -0.5, 99.5); 
	Particles=new MethodHistograms("Particles");
	EMCal=new MethodHistograms("EMCAL");
	IHCal=new MethodHistograms("IHCAL");
	OHCal=new MethodHistograms("OHCAL");
	histogram_map["parts"]=Particles;
	histogram_map["emcal"]=EMCal;
	histogram_map["ihcal"]=IHCal;
	histogram_map["ohcal"]=OHCal;
	for(auto h:this->histogram_map){
		std::string typelabel = h.first;
		auto hists=h.second;
		std::cout<<"Making the histograms for " <<h.first <<std::endl;
		for(auto h1:hists->histsvector) std::cout<<"Have a histogram with that has name " <<h1->GetName()  <<std::endl;
	}
}

int CalorimeterTowerENC::Init(PHCompositeNode *topNode)
{	
	//book histograms and TTrees
	histogram_map["parts"]->E->GetName();	
	return 0; 
}

float CalorimeterTowerENC::getPt(PHG4Particle* p)
{
	float pt=0;
	pt=pow(p->get_px(), 2) + pow(p->get_py(), 2);
	pt=sqrt(pt);
	return pt;
}

float CalorimeterTowerENC::getR(std::pair<float, float> p1, std::pair<float,float> p2){
	float eta_dist=p1.first - p2.first;
	float phi_dist=p1.second - p2.second;
	phi_dist=abs(phi_dist);
	if(phi_dist > PI) phi_dist=2*PI-phi_dist;
	float R=sqrt(pow(eta_dist,2) + pow(phi_dist, 2));
	return R;
}
void CalorimeterTowerENC::GetENCCalo(PHCompositeNode* topNode, std::unordered_set<int> tower_set, TowerInfoContainer* data, RawTowerGeomContainer_Cylinderv1* geom, RawTowerDefs::CalorimeterId calo, float jete, std::string caloh, int npt)
{
	geom->set_calorimeter_id(calo);
	MethodHistograms* histograms=histogram_map.at(caloh);
	histograms->E->Fill(jete);
	histograms->N->Fill(tower_set.size());
	for(auto i:tower_set){
		if(!geom) continue;
		if(i < 0 || i >= (int) data->size() ) continue;
		auto tower_1 = data->get_tower_at_key(i);
		if(!tower_1) continue;
		auto key_1 = data->encode_key(i);
		int phibin_1 = data->getTowerPhiBin(key_1);
		int etabin_1 = data->getTowerEtaBin(key_1);
		//std::cout<<"Phibin: " <<phibin_1 <<" Etabin: " <<etabin_1 <<std::endl;
		if(etabin_1 < 0 || phibin_1 < 0 ||  phibin_1 >= geom->get_phibins() || etabin_1 >= geom->get_etabins() ) continue;
		float phi_center_1 = geom->get_phicenter(phibin_1);
		float eta_center_1 = geom->get_etacenter(etabin_1);
		float energy_1= tower_1->get_energy(); 
		//there is probably a smart way to collect the group of sizes n, but I can really just do it by hand for rn 
		for(auto j:tower_set){
			if (i >= j) continue;
			if(j < 0 || j >= (int) data->size() ) continue;
			auto tower_2 = data->get_tower_at_channel(j);
			auto key_2 = data->encode_key(j);
			int phibin_2 = data->getTowerPhiBin(key_2);
			int etabin_2 = data->getTowerEtaBin(key_2);
			float phi_center_2 = geom->get_phicenter(phibin_2);
			float eta_center_2 = geom->get_etacenter(etabin_2);
			if(etabin_2 < 0 || phibin_2 < 0 ||  phibin_2 >= geom->get_phibins() || etabin_2 >= geom->get_etabins() ) continue;
			float energy_2 = tower_2->get_energy();
			std::pair<float, float> tower_center_1 {eta_center_1, phi_center_1}, tower_center_2 {eta_center_2, phi_center_2};
			float R_12=getR(tower_center_1, tower_center_2);
			float e2c=energy_1*energy_2 / ((float) pow(jete,2));
			histograms->R->Fill(R_12);
			histograms->E2C->Fill(R_12, e2c/(float) Nj);
			if( npt >= 3){
				for( auto k:tower_set){
					if( j >= k || i >= k) continue;
					if(k < 0 || k >= (int) data->size() ) continue;
					auto tower_3 = data->get_tower_at_channel(k);
					auto key_3 = data->encode_key(k);
					int phibin_3 = data->getTowerPhiBin(key_3);
					int etabin_3 = data->getTowerEtaBin(key_3);
					if(etabin_3 < 0 || phibin_3 < 0 ||  phibin_3 >= geom->get_phibins() || etabin_3 >= geom->get_etabins() ) continue;
					float phi_center_3 = geom->get_phicenter(phibin_3);
					float eta_center_3 = geom->get_etacenter(etabin_3);
					float energy_3 = tower_3->get_energy();
					std::pair<float, float> tower_center_3 {eta_center_3, phi_center_3};
					float R_13=getR(tower_center_1, tower_center_3);
					float R_23=getR(tower_center_2, tower_center_3);
					float e3c=energy_3*e2c/(float)jete;
					float R_L = std::max(R_12, R_13);
					R_L=std::max(R_L, R_23);
					histograms->E3C->Fill(R_L, e3c/(float) Nj);
				}
			}
		}
	}
}
void CalorimeterTowerENC::GetE2C(PHCompositeNode* topNode, std::unordered_set<int> em, std::unordered_set<int> ih, std::unordered_set<int> oh)
{
	//this is the processor that allows for read out of the tower energies given the set of towers in the phi-eta plane
	auto emcal_geom =  findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_CEMC");
	auto emcal_tower_energy =  findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
	auto ihcal_geom= findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN");
	auto ihcal_tower_energy= findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
	auto ohcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");
	auto ohcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
	float jete_em = 0, jete_ih=0, jete_oh=0;
	for(auto i:em)
	{
		try{
			auto tower_e=emcal_tower_energy->get_tower_at_key(i);
			jete_em+=tower_e->get_energy();
		}
		catch(std::exception& e) { continue; }
	}
	for(auto i:ih)
	{
		try{
			auto tower_e=ihcal_tower_energy->get_tower_at_key(i);
			jete_ih+=tower_e->get_energy();
		}
		catch(std::exception& e) { continue; }
	}
	for(auto i:oh)
	{
		try{
			auto tower_e=ohcal_tower_energy->get_tower_at_key(i);
			jete_oh+=tower_e->get_energy();
		}
		catch(std::exception& e) { continue; }
	}
	histogram_map["emcal"]->E->Fill(jete_em);
	histogram_map["ihcal"]->E->Fill(jete_ih);
	histogram_map["ohcal"]->E->Fill(jete_oh);
	histogram_map["emcal"]->N->Fill(em.size());
	histogram_map["ihcal"]->N->Fill(ih.size());
	histogram_map["ohcal"]->N->Fill(oh.size());
	GetENCCalo(topNode,em, emcal_tower_energy, emcal_geom, RawTowerDefs::CEMC,    jete_em, "emcal", 2);
	GetENCCalo(topNode,ih, ihcal_tower_energy, ihcal_geom, RawTowerDefs::HCALIN,  jete_ih, "ihcal", 2);
	GetENCCalo(topNode,oh, ohcal_tower_energy, ohcal_geom, RawTowerDefs::HCALOUT, jete_oh, "ohcal", 2);
	return;
}
void CalorimeterTowerENC::GetE3C(PHCompositeNode* topNode, std::unordered_set<int> em, std::unordered_set<int> ih, std::unordered_set<int> oh, std::map<int, float>* emtowere)
{
	//this is the processor that allows for read out of the tower energies given the set of towers in the phi-eta plane
	auto emcal_geom =  findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_CEMC");
	auto emcal_tower_energy =  findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_SIM_CEMC");
	auto ihcal_geom= findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN");
	auto ihcal_tower_energy= findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_SIM_HCALIN");
	auto ohcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");
	auto ohcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_SIM_HCALOUT");
	float jete_em = 0, jete_ih=0, jete_oh=0;
	for(int n=0; n<(int) ohcal_tower_energy->size(); n++){
		 jete_oh+=ohcal_tower_energy->get_tower_at_channel(n)->get_energy();
	//	std::cout<<"For tower at channel " <<n <<" the energy is : " <<ohcal_tower_energy->get_tower_at_channel(n)->get_energy() <<std::endl;
	}
	std::cout<<"Total energy found in the ohcal is " <<jete_oh <<std::endl;
	jete_oh=0;
	for(auto i:em)
	{
		try{
			if( i <= 0 ) continue;
			if(n_evts <10 ) std::cout<<"The tower number in use is " <<i <<std::endl;
			auto tower_e=emcal_tower_energy->get_tower_at_channel(i);
			if(tower_e){
				auto et=tower_e->get_energy();
				if(et == 0 && i > 1) et+=emcal_tower_energy->get_tower_at_channel(i-1)->get_energy();
				if(et == 0 && i > 1) et+=emcal_tower_energy->get_tower_at_channel(i+1)->get_energy();
				jete_em+=et;
				std::cout<<"The added energy is " <<et <<" total energy " <<jete_em <<std::endl;; 
				(*emtowere)[i]+=tower_e->get_energy();
			}
		}
		catch(std::exception& e) { continue; }
	}
	for(auto i:ih)
	{
		try{
			if( i <= 0 ) continue;
			auto tower_e=ihcal_tower_energy->get_tower_at_key(i);
			if(tower_e) jete_ih+=tower_e->get_energy();
		}
		catch(std::exception& e) { continue; }
	}
	for(auto i:oh)
	{
		try{
			if( i <= 0 ) continue;
			auto tower_e=ohcal_tower_energy->get_tower_at_channel(i);
	//		std::cout<<"Jet Tower: We get the energy for the tower at channel " <<i <<" as " <<tower_e->get_energy() <<std::endl;
			if(tower_e) jete_oh+=tower_e->get_energy();
		}
		catch(std::exception& e) { continue; }
	}
	//std::cout<<"Checked " <<em.size() <<" towers in the outer hcal \n Got an energy in the ohcal part of the jet of " <<jete_oh <<std::endl; 
//	histogram_map["ohcal"]->E->Fill(jete_oh);
	GetENCCalo(topNode,em, emcal_tower_energy, emcal_geom, RawTowerDefs::CEMC,    jete_em, "emcal", 3);
	GetENCCalo(topNode,ih, ihcal_tower_energy, ihcal_geom, RawTowerDefs::HCALIN,  jete_ih, "ihcal", 3);
	GetENCCalo(topNode,oh, ohcal_tower_energy, ohcal_geom, RawTowerDefs::HCALOUT, jete_oh, "ohcal", 3);
	return;
}

void CalorimeterTowerENC::GetE2C(PHCompositeNode* topNode, std::map<PHG4Particle*, std::pair<float, float>> jet)
{
	//this is the processor that allows for particle read of jets phi-eta plane
	float jet_total_e=0;
	for(auto p1:jet) jet_total_e+=p1.first->get_e();
	histogram_map["parts"]->E->Fill(jet_total_e);
	histogram_map["parts"]->N->Fill(jet.size());
	for(auto p1it=jet.begin(); p1it != jet.end(); ++p1it )
	{
		auto p1=(*p1it);
		for(auto p2it=p1it; p2it != jet.end(); ++p2it){
			auto p2=(*p2it);
			if(p1it == p2it) continue;
			double pe1=p1.first->get_e();
			double pe2=p2.first->get_e();
			float R12=getR(p1.second, p2.second);
			histogram_map["parts"]->R->Fill(R12);
			float e2c=pe1*pe2/jet_total_e;
			e2c=e2c/(float)Nj;
			histogram_map["parts"]->E2C->Fill(R12, e2c);
		}
	}
	return;
			
}

void CalorimeterTowerENC::GetE3C(PHCompositeNode* topNode, std::map<PHG4Particle*, std::pair<float, float>> jet)
{
	//this is the processor that allows for particle read of jet in the phi-eta plane
	float jet_total_e=0;
	for(auto p1:jet) jet_total_e+=p1.first->get_e();
//	std::cout<<"The histogram map has size " <<histogram_map.size() <<std::endl;
//	for(auto h:this->histogram_map) for(auto h1:h.second->histsvector) std::cout<<"The histogram map for " <<h.first <<" has a histogram with name " <<h1->GetName() <<std::endl;
	this->histogram_map["parts"]->E->Fill(jet_total_e);
	this->histogram_map["parts"]->N->Fill(jet.size());
	for(auto p1it=jet.begin(); p1it != jet.end(); ++p1it )
	{
		auto p1=(*p1it);
		for(auto p2it=p1it; p2it != jet.end(); ++p2it){
			auto p2=(*p2it);
			if(p1it == p2it) continue;
			double pe1=p1.first->get_e();
			double pe2=p2.first->get_e();
			float R12=getR(p1.second, p2.second);
			histogram_map["parts"]->R->Fill(R12);
			float e2c=pe1*pe2/(float) pow(jet_total_e, 2);
			e2c=e2c/(float)Nj;
			histogram_map["parts"]->E2C->Fill(R12, e2c);
			for(auto p3it=p2it; p3it != jet.end() ; ++p3it){
				auto p3=(*p3it);
				if(p1it == p3it || p2it == p3it) continue;
				double pe3=p3.first->get_e();
				float R13=getR(p1.second, p3.second);
				float R23=getR(p2.second, p3.second);
				float e3c=e2c*pe3/(float)jet_total_e;
				float R_L=std::max(R12, R23);
				R_L=std::max(R_L, R13);
				histogram_map["parts"]->E3C->Fill(R_L, e3c);
			}
		}
	}
	return;
}
	
std::pair<std::map<float, std::map<float, int>>, std::pair<float, float>> CalorimeterTowerENC::GetTowerMaps(RawTowerGeomContainer_Cylinderv1* geom, RawTowerDefs::CalorimeterId caloid, TowerInfoContainer* calokey)
{
	//ge thte geometry map for the towers in the first event to may it easy to serarc
	//the idea is put the menan in and the associate a key with it 
	geom->set_calorimeter_id(caloid);
	std::pair<float, float> deltas{0,0};
	std::map<float, std::map<float, int>> bins;
	std::map<int, std::pair<float, float>> gs;
	geom->set_calorimeter_id(caloid);
	for(int i = 0; i<(int) calokey->size(); i++){
		try{
	//		std::cout<<"The channel number is " <<i <<std::endl;
			int key=calokey->encode_key(i);
			//int tower=calokey->get_tower_at_key(key);
			int phibin=calokey->getTowerPhiBin(key);
			int etabin=calokey->getTowerEtaBin(key);
			float eta=geom->get_etacenter(etabin);
			float phi=geom->get_phicenter(phibin);
			std::pair g { eta, phi};
			if( deltas.first == 0){
				std::pair <double, double> etabounds=geom->get_etabounds(etabin);
				deltas.first=abs(etabounds.first - etabounds.second)/2.;
				std::pair <double, double> phibounds=geom->get_phibounds(etabin);
				deltas.second=abs(phibounds.first - phibounds.second)/2.;
			}
			gs[i]=g;
		}
		catch(std::exception& e){ 
			std::pair<float, float> g {-5.0, -20.0 };
			gs[i]=g;
		} 
	}
	int i=0;
	for(auto g1: gs){
		auto g=g1.second;
		if(g.first == -5  && i > 1){
			if((int)(gs.at(i-1).first*4.4/deltas.first) % 8 != 7 && (int)((3.1416/deltas.second)*gs.at(i-1).second) % 2 == 1  ){
				g.first=gs.at(i-1).first+2*deltas.first; 
				g.second=gs.at(i-1).second;
			}
			else if ((int)((3.1416/deltas.second)*gs.at(i-1).second) % 2 != 1 ){
				g.first=gs.at(i-1).first;
				g.second=gs.at(i-1).second + 2*deltas.second;
			}
			else if ((int)((4.4/deltas.first)*gs.at(i-1).first) % 8 == 7  && (int)((3.1416/deltas.second)*gs.at(i-1).second) % 2 == 1 && (4.4/deltas.first)*gs.at(i-1).first != 1.1 - deltas.first){
				g.first=gs.at(i-1).first+2*deltas.first;
				g.second=gs.at(i-1).second-16*deltas.second;
			}
			else if( (int) ((4.4/deltas.first)*gs.at(i-1).first) == 1.1 - deltas.first && (int)((3.1416/deltas.second)*gs.at(i-1).second) % 2 == 1 ) {
				gs.at(i-1).first = -1.1 + deltas.first; 
				gs.at(i-1).second = gs.at(i-1).second + 2*deltas.second;
			}
		}
		bins[g.first][g.second]=g1.first;
		i++;
	}
	return std::make_pair(bins, deltas);
}
int CalorimeterTowerENC::GetTowerNumber(std::pair<float, float> part, std::map<float, std::map< float, int>> Calomap, std::pair<float, float> delta)
{
	int keynumber=-1;
	part.second+=PI;
//	std::cout<<"The particle is centered around eta = " <<part.first <<" phi = " <<part.second <<std::endl;
	if (abs(part.first) > 1.2 ) return keynumber;
	for(auto t:Calomap)
	{
		if( part.first < t.first + delta.first && part.first >= t.first - delta.first){
			//checking the eta bounds first
			for (auto tp:t.second){
				if(part.second < tp.first + delta.second && part.second >= tp.first - delta.second){
					if(n_evts < 10 ) std::cout<<"Found a good bin centered in eta at " <<t.first <<" in phi at "<<tp.first <<" with bin number " <<tp.second <<std::endl;
				 	keynumber=tp.second;
					break;
				}
			}
		}
	}
//	if(n_evts < 10 ) std::cout<<" The particle is at eta= " <<part.first <<" and phi = " <<part.second <<std::endl;
	return keynumber;
}
int CalorimeterTowerENC::RecordHits(PHCompositeNode* topNode, Jet* truth_jet){
	//record where the particles are located and which towers are seeing hits
	std::set<std::pair<float, float>> particle_coords; //This will store the location of each particle which then I can use tho capture the relevant towe
	std::unordered_set<PHG4Particle*> jetparticles;
	std::undorder_set<PHG4Hits*> jethits;
	std::map<int, float> jettowenergy, emtowerenergy;
	auto _truthinfo=findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
//	auto _truthhits=findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_BH_1");
	auto particles=_truthinfo->GetMap();	
	//std::cout<<"Indicies for the truth map are ";
	//for(auto p:particles) std::cout<<p.first <<std::endl; 
	for( auto& iter:truth_jet->get_comp_vec()){
		Jet::SRC source = iter.first;
		unsigned int idx = iter.second;
		if( source != Jet::PARTICLE){
			std::cout<<"This jet has data that is not a particle" <<std::endl;
			return -1;
		}
			if (particles.find(idx) == particles.end()) std::cout<<"Index is not in map for index " <<idx <<std::endl;	
			else { 
				PHG4Particle* truth_part = _truthinfo->GetParticle(idx);
				PHG4Shower* truth_shower = _truthinfo->GetShower(idx);
				jetparticles.insert(truth_part);
				auto showerhits=truth_shower->g4Hit_ids()
			}
		//jethits.insert(truth_hit);
	}
	std::map<PHG4Particle*, std::pair<float, float>> jet_particle_map;
	for(auto p:jetparticles){
	       //take in the identified particles that make up the subjet and then we overap that in the towers
		std::pair<float, float> particle_coord; 
		particle_coord.first = atanh(p->get_pz()/sqrt(pow(p->get_py(),2) + pow(p->get_px(),2) + pow(p->get_pz(), 2)));
		particle_coord.second = atan2(p->get_py(), p->get_px());
		particle_coords.insert(particle_coord);	
		jet_particle_map[p]=particle_coord;
		jethits->Fill(particle_coord.first, particle_coord.second);
		if ( n_evts < 10 ) std::cout<<"The particle we are looking for is located at eta = " <<particle_coord.first 
				<<" phi = " << particle_coord.second <<std::endl;
		int towernumberemcal=GetTowerNumber(particle_coord, EMCALMAP.first, EMCALMAP.second);
		if(jettowenergy.find(towernumberemcal) == jettowenergy.end())jettowenergy[towernumberemcal]=0; 
		jettowenergy[towernumberemcal]+=p->get_e();
		
	}
//	getE2C(jet_particle_map, topNode); 
	GetE3C(topNode, jet_particle_map); //get both in one call
	std::unordered_set<int> jet_towers_ihcal, jet_towers_ohcal, jet_towers_emcal; 
	for(auto pc:particle_coords)
	{
		if(abs(pc.first) > 1.2) continue;
		if ( n_evts < 10 ) std::cout<<"The particle we are looking for is located at eta = " <<pc.first 
				<<" phi = " << pc.second <<std::endl;
		int tne=GetTowerNumber(pc, EMCALMAP.first, EMCALMAP.second);
		if(tne != -1){ jet_towers_emcal.insert(tne);
			std::cout<<"The identified tower number for the emcal is " <<tne <<std::endl;
		}
		emtowerenergy[tne]=0;
		int tni=GetTowerNumber(pc, IHCALMAP.first, IHCALMAP.second);
		if(tni != -1) jet_towers_ihcal.insert(tni);
		std::cout<<"Particle added to the ihcal in bin " <<tni <<std::endl;
		int tno=GetTowerNumber(pc, OHCALMAP.first, OHCALMAP.second);
		if (tno != -1) jet_towers_ohcal.insert(tno);
		std::cout<<"Particle added to the ohcal in bin " <<tno <<std::endl;
	}
	std::cout<<"We have loaded in particles " <<particle_coords.size() <<std::endl;
//	getE2C(topNode, jet_towers_ihcal, jet_towers_ohcal, jet_towers_emcal);
	GetE3C(topNode, jet_towers_emcal, jet_towers_ihcal, jet_towers_ohcal, &emtowerenergy); //get both values filled in one 
	for(auto m:jettowenergy) comptotows->Fill(m.second, emtowerenergy[m.first]);
		
	return 1;
}
	       	
int CalorimeterTowerENC::process_event(PHCompositeNode *topNode){
	//need to process the events, this is really going to be a minor thing, need to pull my existing ENC code to do it better
	//for the first event build the  tower number map that is needed
	n_evts++;
	std::cout<<"Running on event " <<n_evts<<std::endl;
	try{
		auto emcal_geom =  findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_CEMC");
		auto emcal_tower_energy =  findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_SIM_CEMC");
		auto ihcal_geom= findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN");
		auto ihcal_tower_energy= findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_SIM_HCALIN");
		auto ohcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");
		auto ohcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_SIM_HCALOUT");
		if(EMCALMAP.first.size() == 0 )  EMCALMAP=GetTowerMaps(emcal_geom, RawTowerDefs::CEMC, emcal_tower_energy);
		if(IHCALMAP.first.size() == 0 )  IHCALMAP=GetTowerMaps(ihcal_geom, RawTowerDefs::HCALIN, ihcal_tower_energy);
		if(OHCALMAP.first.size() == 0 )  OHCALMAP=GetTowerMaps(ohcal_geom, RawTowerDefs::HCALOUT, ohcal_tower_energy);
		auto jets = findNode::getClass<JetContainer> (topNode, "AntiKt_Truth_r04");
		Nj=jets->size();
		for(auto j:*jets) RecordHits(topNode, j);
		number_of_jets->Fill(Nj);
		
	}
	catch(std::exception& e ) {
		std::cout<<"Failed to run on event " <<n_evts <<"\n Skipping Event" <<std::endl;	
		n_evts--;
	}
	return Fun4AllReturnCodes::EVENT_OK;
	
	
}
int CalorimeterTowerENC::End(PHCompositeNode *topNode){
	return 1;
}
void CalorimeterTowerENC::Print(const std::string &what) const
{
	std::cout<<"Printing the files out now to a file named: " <<outfilename <<std::endl;
	TFile* f=new TFile(outfilename.c_str(), "RECREATE");
	//printing the stuff out 
	for(auto hs:histogram_map){
		hs.second->R->Scale(1/(float)hs.second->N->GetEntries()); //average over number of jets
		hs.second->E2C->Scale(1/(float)hs.second->E2C->GetBinWidth(5)); //correct for binning
		hs.second->E3C->Scale(1/(float)hs.second->E2C->GetBinWidth(5)); //correct for binning
		hs.second->E2C->Scale(1/(float)n_evts); //average over events
		hs.second->E3C->Scale(1/(float)n_evts); //average over events
		for(auto h:hs.second->histsvector){
			h->Write();
		}
	}
	jethits->Write();
	comptotows->Write();
	number_of_jets->Write();
	f->Write();
	f->Close();
	return;
}
