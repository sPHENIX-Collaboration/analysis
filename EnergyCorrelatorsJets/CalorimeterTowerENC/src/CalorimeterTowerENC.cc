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
int CalorimeterTowerENC::RecordHits( /*some data struct*/){
	//record where the particles are located and which towers are seeing hits
	for(auto p: 
int CalorimeterTowerENC::process_event(PHCompositeNode *topNode){
	//need to process the events, this is really going to be a minor thing, need to pull my existing ENC code to do it better
}
int CalorimeterTowerENC::End(PHCompositeNode *topNode){

}
void CalorimeterTowerENC::Print(const std::string &what)
{
	//printing the stuff out 
}
