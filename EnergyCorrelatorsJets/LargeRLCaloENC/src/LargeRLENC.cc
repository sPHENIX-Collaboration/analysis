//////////////////////////////////////////////////////////////////////////////////////////////////
//											  	//
//			Large R_L Calorimeter ENC						//
//												//
//			Author: Skadi Grossberndt						//
//			Depends on: Calorimeter Tower ENC 					//
//			First Commit date: 18 Oct 2024						//
//			Most recent Commit: 18 Oct 2024						//
//			version: v0.1								//
//												//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "LargeRLENC.h"

LargeRLENC::LargeRLENC(const int n_run/*=0*/, const int n_segment/*=0*/, const float jet_min_pT/*=0*/, const bool onlyconst bool data/*=false*/,const std::string& name/* = "LargeRLENC"*/) 
{
	this->FullHcal=new MethodHistograms("Full_HCAL", 3.9); //Full OHCAL has R ~ 3.83 (delta eta ~ 2.2, delta phi ~pi)
	this->TowardRegion=new MethodHistogram("Toward_Region", 3.9);
	this->AwayRegion=new MethodHistogram("Away_region", 3.9);
	this->TransverseRegion=new MethodHistogram("Transverse_Region", 3.9);
	//while the Towards and away region arent't so extensive, haveing Rmax ~ 3, but close enough, transverse region is going to have some discontinuities
	this->isRealData=data; //check if the data is real data, if not run the truth as a cross check 
	this->nRun=n_run;
	this->nSegment=n_segment;
	this->jetMinpT=jet_min_pT;
	this->algo="Tower";
	this->radius="r04";
	this->eventCut=new EventSelectionCut;
}
JetContainerv2* LargeRLENC::getJets(std::string algo, std::string radius, PHCompositeNode* topNode)
{
	//This is just running the Fastjet reco 
	
}
void LargeRLENC::addTower(int n, TowerInfoContainer* energies, RawTowerGeomContainer_Cylinderv1* geom, std::map<std::pair<float, float>, float>* towers)
{
	auto key=energies->encode_key(n);
	auto tower=energies->get_tower_at_channel(n);
	int phibin=energies->getTowerPhiBin(key);
	int etabin=energies->getTowerEtabin(key);
	float phicenter=geom->get_phicenter(phibin);
	float etacenter=geom->get_etacenter(etabin);
	std::pair<float, float> center {etacenter, phicenter};
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
	if(jets==NULL) jets=getJets(algo, radius);
	if(!jets){
		std::cout<<"Didn't find any jets, skipping event" <<std::endl;
		return -1/*FUN4ALL::*/;
	}
	float emcal_energy=0., ihcal_energy=0, ohcal_energy=0, total_energy=0, ohcal_rat=0;
	std::map<std::pair<float, float>, float> *ihcal_towers, *emcal_towers, *ohcal_towers; //these are just to collect the non-zero towers to decrease the processing time 
	for(int n=0; n<(int) emcal_tower_energy->size(); n++){
		float energy=emcal_tower_energy->get_tower_at_channel(n)->get_energy();
		if(energy > 0.07)//put zero supression into effect
			addTower(n, emcal_tower_energy, emcal_geom, emcal_towers);
		emcal_energy+=energy;
	}
	for(int n=0; n<(int) ihcal_tower_energy->size(); n++){
		float energy+=ihcal_tower_energy->get_tower_at_channel(n)->get_energy();
		if(energy > 0.1) //zero suppression is at 100 MeV in IHCAL
			addTower(n, ihcal_tower_energy, ihcal_geom, ihcal_towers);
		ihcal_energy+=energy;
	}
	for(int n=0; n<(int) ohcal_tower_energy->size(); n++){
		float energy+=ohcal_tower_energy->get_tower_at_channel(n)->get_energy();
		if(energy > 0.5) 
			addTower(n, ohcal_tower_energy, ohcal_geom, ohcal_towers);
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
	isDijet=eventCut->passesTheCut(jets, ohcal_rat);	
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
		
		
}
