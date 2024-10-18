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
}

int LargeRLENC::process_event(PHCompositeNode* topNode)
{
	//This is where I can be a bit clever with allowing for the parrelization for easier crosschecks
	n_evts++;
	auto ihcal_geom= findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALIN");
	auto ihcal_tower_energy= findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
	auto ohcal_geom=findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_HCALOUT");
	auto ohcal_tower_energy=findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
	
	//look for the jet objects 
	JetContainerv2* jets;
	bool foundJetConts=false, isDijet=false;
	try{
		if(!isRealData) jets = findNode::getClass<JetContainer>(topNode, "AntiKt_Truth_r04"); //look for the r_04 truth jets
		else{
			jet
