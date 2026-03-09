#pragma once
#include "../IsotrackTreesAnalysis.h"
#include <TVector3.h>
#include <string>

void IsotrackTreesAnalysis::initTrackRatesModule() {
  std::string eta_list[] = {"central","forward"};
  int centrality_list[] = {30,50,70,90};
  histTrackTotal = new TH1F("total_track_rate","",20,0,20);

  /*
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 4; j++) {
      histTrackRate[4*i+j] = new TH1F(TString::Format("track_rate_%s_eta_centrality_%d",eta_list[i].c_str(),centrality_list[j]),"",20,0,20);
    }
  }
  */
}

void IsotrackTreesAnalysis::trackRatesModule(int id) {

  histTrackTotal->Fill(m_tr_p[id]);
  
  float centrality_array[] = {20,40,60,80,100};

  /*
  if (fabs(m_tr_cemc_eta[id]) < 0.5) { 
    for (int j = 1; j < 5; j++) {
      if (!USE_CENTRALITY || (centrality > centrality_array[j-1] && centrality < centrality_array[j])) {
        histTrackRate[j-1]->Fill(m_tr_p[id]);
      }
    }
  } else if (fabs(m_tr_cemc_eta[id]) < 1.0) {
    for (int j = 1; j < 5; j++) {
      if (!USE_CENTRALITY || (centrality > centrality_array[j-1]&& centrality < centrality_array[j])) {
        histTrackRate[3+j]->Fill(m_tr_p[id]);
      }
    }
  }
  */
}