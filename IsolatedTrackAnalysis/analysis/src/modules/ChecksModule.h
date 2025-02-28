#pragma once
#include "../IsotrackTreesAnalysis.h"
#include <TVector3.h>
#include <string>

void IsotrackTreesAnalysis::initChecksModule() {
	// check tower region number 
	std::string detector[] = {"cemc","ihcal","ohcal"};
	for (int i = 0; i < 3; i++) {
    	histTowerNumber[i] = new TH1F(TString::Format("tower_region_number_%s",detector[i].c_str()),"",100,-0.5,99.5);
  	}
}

void IsotrackTreesAnalysis::checksModule(MatchedClusterContainer cemcClusters, MatchedClusterContainer ihcalClusters, MatchedClusterContainer ohcalClusters) {
	histTowerNumber[0]->Fill(cemcClusters.getNumberOfClusters());
	histTowerNumber[1]->Fill(ihcalClusters.getNumberOfClusters());
	histTowerNumber[2]->Fill(ohcalClusters.getNumberOfClusters());
}