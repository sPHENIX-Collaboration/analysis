//////////////////////////////////////////////////////////////////////////////////////////////////
//												//
//												//
//		Calorimeter Tower Jets N Point-Energy Correlator Study				//
//		Author: Skadi Grossberndt							//
//		Date of First Commit: 12 July 2024						//
//		Date of Last Update:  13 July 2024						//
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
float HerwigJetSpectra::getPt(HepMC::GenParticle* p)
{
	float pt=0;
	pt=pow(p->momentum().px(), 2) + pow(p->momentum().py(), 2);
	pt=sqrt(pt);
	return pt;
}
float HerwigJetSpectra::getR(HepMC::GenParticle* p1, HepMC::GenParticle* p2){
	float eta_dist=p1->momentum().pseudoRapidity() - p2->momentum().pseudoRapidity();
	float phi_dist=p1->momentum().phi() - p2->momentum().phi();
	phi_dist=abs(phi_dist);
	if(phi_dist > PI) phi_dist=2*PI-phi_dist;
	float R=sqrt(pow(eta_dist,2) + pow(phi_dist, 2));
	return R;
}
std::unordered_map<int, std::pair<std::pair<float, float>, std::pair <float, float>>> CalorimeterTowerENC::GetTowerMaps(RawTowerGeomContainer_Cylinder v1*, RawTowerDefs::Calorimenter ID)
{
	//ge thte geometry map for the towers in the first event to may it easy to serarc
	
	
int CalorimeterTowerENC::GetTowerNumber(std::pair<float, float> part)
{
	
int CalorimeterTowerENC::RecordHits( /*some data struct*/){
	//record where the particles are located and which towers are seeing hits
	std::unordered_set<std::pair<float, float>> particle_coords; //This will store the location of each particle which then I can use tho capture the relevant towe
	getE2C(jetparticles, 1); //mode 1 uses the reco particles, 2 for towers, 3 (eventually) for thruth
	getE3C(jetparticles, 1);
	for(auto p:jetparticles){
	       //take in the identified particles that make up the subjet and then we overap that in the towers
		std::pair<float, float> particle_coords; 
		particle_coords.first = p->get_eta();
		particle_coords.second = p->get_phi();
		particle_coord.insert(particle_coords);	
	}
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
