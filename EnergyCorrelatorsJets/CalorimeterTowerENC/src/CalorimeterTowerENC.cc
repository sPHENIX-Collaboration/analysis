//////////////////////////////////////////////////////////////////////////////////////////////////
//												//
//												//
//		Calorimeter Tower Jets N Point-Energy Correlator Study				//
//		Author: Skadi Grossberndt							//
//		Date of First Commit: 12 July 2024						//
//		Date of Last Update:  16 July 2024						//
//		version: v0.0									//
//												//
//												//
//		This project is a study on energy correlators using particle jets and 		//
//	jets  definied over calorimeter towers. Feasibility study to evaluate pp prospects	//
//	for sPHENIX in 2024 with minimal tracking and momentum resolution 			//
//												//
//////////////////////////////////////////////////////////////////////////////////////////////////


#include "CalorimeterTowerENC.h"

CalorimeterTowerENC::CalorimeterTowerENC(const std::string &name){
	//this is the constructor
}

int CalorimeterTowerENC::Init(PHCompositeNode *topNode)
{	
	//book histograms and TTrees
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
void CalorimeterTowerENC::GetE2C(PHCompositeNode* topNode, std::unordered_set<int> em, std::unordered_set<int> ih, std::unordered_set<int> oh, int version)
{
	//this is the processor that allows for read out of the tower energies given the set of towers in the phi-eta plane
	auto emcal_geom =  findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_CEMC");
	auto emcal_tower_energy =  findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
	auto ihcal_geom= findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN");
	auto ihcal_tower_energy= findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
	auto ohcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");
	auto ohcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
	for(auto i:em)
	{
		emcal_geom->set_calorimeter_id(RawTowerDefs::CEMC);
		try{
			auto tower_e=emcal_tower_energy->get_tower_at_key(i);
		}
		catch(std::exception& e){
			std::cout<<"Could not access the tower of that key" <<std::endl;
		}
		
}
void CalorimeterTowerENC::GetE3C(PHCompositeNode* topNode, std::unordered_set<int> em, std::unordered_set<int> ih, std::unordered_set<int> oh, int version)
{
	//this is the processor that allows for read out of the tower energies given the set of towers in the phi-eta plane
}
}
void CalorimeterTowerENC::GetE2C(PHCompositeNode* topNode, std::map<PHG4Particles*, std::pair<float, float>> jet, int version)
{
	//this is the processor that allows for particle read of jets phi-eta plane
}
}
void CalorimeterTowerENC::GetE2C(PHCompositeNode* topNode, std::map<PHG4Particles*, std::pair<float, float>> jet, int version)
{
	//this is the processor that allows for particle read of jet in the phi-eta plane
}
}	
std::pair<std::map<float, std::map<float, int>>, std::pair<float, float>> CalorimeterTowerENC::GetTowerMaps(RawTowerGeomContainer_Cylinderv1* geom, RawTowerDefs::CalorimenterID caloid, TowerInfoContainer* calokey)
{
	//ge thte geometry map for the towers in the first event to may it easy to serarc
	//the idea is put the menan in and the associate a key with it 
	geom->set_calorimeter_id(caloid);
	std::pair<float, float> deltas{0,0};
	std::map<float, std::map<float, int>> bins;
	std::vector<std::pair<float, float>> gs;
	geom->set_calorimeter_id(caloid);
	for(int i = 0; i<(int) caloevent->size(); i++){
		try{
			int key=calokey->encode_key(i);
			int phibin=calokey->getTowerPhiBin(key);
			int etabin=calokey->getTowerPhiBin(key);
			float eta=geom->get_etacenter(etabin);
			float phi=geom->get_phicenter(phibin);
			std::pair g { eta, phi};
			gs.push_back(g);
			if( deltas.first == 0){
				std::pair <double, double> etabounds=geom->get_etabounds(etabin);
				deltas.first=abs(etabounds.first - etabounds.second)/2.;
				std::pair <double, double> phibounds=geom->get_phibounds(etabin);
				deltas.second=abs(etabounds.first - etabounds.second)/2.;
			}
		}
		catch(std::exception& e){ 
			std::pair<float, float> g {-5.0, -20.0 };
			gs.push_back(g);
		} 
	}
	for(int i=0; i<(int) gs.size(); i++){
		auto g=gs.at(i);
		if(g.first == -5  && i > 1){
			if((gs.at(i-1).first*4.4/deltas.first) % 8 != 7 && ((3.1416/deltas.second)*gs.at(i-1).second) % 2 == 1  ){
				g.first=gs.at(i-1)+2*deltas.first; 
				g.second=gs.at(i-1).second;
			}
			else if (((3.1416/deltas.second)*gs.at(i-1).second) % 2 != 1 ){
				g.first=gs.at(i-1).first;
				g.second=gs.at(i-1).secont + 2*deltas.second;
			}
			else if (((4.4/deltas.first)*gs.at(i-1).first) % 8 == 7  && ((3.1416/deltas.second)*gs.at(i-1).second) % 2 == 1 && (4.4/deltas.first)*gs.at(i-1).first != 1.1 - deltas.first){
				g.first==gs.at(i-1).first+2*deltas.first;
				g.second=gs.at(i-1).second-16*deltas.second;
			}
			else if( ((4.4/deltas.first)*gs.at(i-1).first) == 1.1 - deltas.first && ((3.1416/deltas.second)*gs.at(i-1).second) % 2 == 1 ) {
				gs.at(i-1).first = -1.1 + deltas.first; 
				gs.at(i-1).second = gs.at(i-1) + 2*deltas.second;
			}
		}
		bins[g.first][g.second]=i;
	}
	return std::make_pair(bins, deltas);
}
int CalorimeterTowerENC::GetTowerNumber(std::pair<float, float> part, std::map<float, std::map< float, int>> Calomap, std::pair<float, float> delta)
{
	for(auto t:Calomap)
	{
		if( part.first < t.first + delta.first && part.first >= t.first - delta.first){
			//checking the eta bounds first
			for (auto tp:t.second){
				if(part.second < tp.first + delta.second && part.second >= tp.first - delta.second) return tp.second;
			}
		}
	}
	return -1;
}
int CalorimeterTowerENC::RecordHits(PHCompositeNode* topNode, Jet* truth_jet){
	//record where the particles are located and which towers are seeing hits
	std::unordered_set<std::pair<float, float>> particle_coords; //This will store the location of each particle which then I can use tho capture the relevant towe
	std::unordered_map<int, PHG4Particle*> jetparticles;
	std::unordered_map<int, PHG4Hit*> jethits;
	for( auto& iter:truth_jet->get_comp_vec()){
		Jet::SRC source = iter.first;
		unsigned int idx = iter.second;
		if( source != Jet::Particle){
			std::cout<<"This jet has data that is not a particle" <<std::endl;
			return -1;
		}
		PHG4Particle* truth_part = _truthinfo->GetParticles(idx);
		PHG4Hit* truth_hit =_truth_info->GetHits(idx);
		jetparticles.insert(truth_part); 
		jethits.insert(truth_hit);
	}
	std::map<PHG4Particle*, std::pair<float, float>> jet_particle_map;
	for(auto ph:truth_hit){
		auto p=ph.second;
		auto pt=jetparticles.at(ph.first).second;
	       //take in the identified particles that make up the subjet and then we overap that in the towers
		std::pair<float, float> particle_coords; 
		particle_coords.first = atanh(p->getz()/sqrt(pow(p->get_y(),2) + pow(p->getx(),2)));
		particle_coords.second = atan2(p->get_y(), p->get_x());
		particle_coord.insert(particle_coords);	
		jet_particle_map[pt]=particle_coords;
	}
	getE2C(jet_particle_map, 1, topNode); //mode 1 uses the reco particles, 2 for towers, 3 (eventually) for thruth
	getE3C(jet_particle_map, 1, topNode);
	std::unordered_set<int> jet_towers_ihcal, jet_towers_ohcal, jet_towers_emcal; 
	for(auto pc:jet_coord)
	{
		int tne=GetTowerNumber(pc, EMCALMAP.second, EMCALMAP.first);
		jet_towers_emcal.insert(tne);
		int tni=GetTowerNumber(pc, IHCALMAP.second, IHCALMAP.first);
		jet_towers_ihcal.insert(tni);
		int tno=GetTowerNumber(pc, OHCALMAP.second, OHCALMAP.first);
		jet_towers_ohcal.insert(tno);
	}
	getE2C(jet_towers_ihcal, jet_towers_ohcal, jet_towers_emcal, 2, topNode);
	getE3C(jet_towers_ihcal, jet_towers_ohcal, jet_towers_emcal, 2, topNode);
	return 1;
}
	       	
int CalorimeterTowerENC::process_event(PHCompositeNode *topNode){
	//need to process the events, this is really going to be a minor thing, need to pull my existing ENC code to do it better
}
int CalorimeterTowerENC::End(PHCompositeNode *topNode){

}
void CalorimeterTowerENC::Print(const std::string &what)
{
	//printing the stuff out 
}
