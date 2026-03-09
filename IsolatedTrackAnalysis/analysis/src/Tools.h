#pragma once
#include "ClusterContainer.h"
//#include "IsotrackTreesAnalysis.h"
#include <TVector.h>

// Match clusters to tracks
MatchedClusterContainer IsotrackTreesAnalysis::getMatchedClusters(int id, caloType type, float dRThreshold){
    
    MatchedClusterContainer clusterContainer;
    TVector3 track, cluster;

    switch(type){

        case cemc:
            // Check if EMCal is reached
            for(int i = 0; i < m_clsmult_cemc; i++){ 
                if (m_cl_cemc_e[i] < 0.1) continue;
                if(m_tr_cemc_eta[id] > -998){
                    track.SetPtEtaPhi(1.0, m_tr_cemc_eta[id], m_tr_cemc_phi[id]); // for the track we only need the direction!
                    cluster.SetPtEtaPhi(m_cl_cemc_e[i]/cosh(m_cl_cemc_eta[i]), m_cl_cemc_eta[i], m_cl_cemc_phi[i]);
                    float dR = track.DeltaR(cluster);
                    if(dR < dRThreshold){
                        clusterContainer.addCluster(cluster, dR);
                    }
                }
            }
            break;

        case ihcal:
            // Check if IHCal is reached
            for(int i = 0; i < m_clsmult_ihcal; i++){ 
                if (m_cl_ihcal_e[i] < 0.015) continue;
                if(m_tr_ihcal_eta[id] > -998){
                    track.SetPtEtaPhi(1.0, m_tr_ihcal_eta[id], m_tr_ihcal_phi[id]); // for the track we only need the direction!
                    cluster.SetPtEtaPhi(m_cl_ihcal_e[i]/cosh(m_cl_ihcal_eta[i]), m_cl_ihcal_eta[i], m_cl_ihcal_phi[i]);
                    float dR = track.DeltaR(cluster);
                    if(dR < dRThreshold){
                        clusterContainer.addCluster(cluster, dR);
                    }
                }
            }
            break;

        case ohcal:
            // Check if OHCal is reached
            for(int i = 0; i < m_clsmult_ohcal; i++){
            
                if (m_cl_ohcal_e[i] < 0.025) continue;
                
                if(m_tr_ohcal_eta[id] > -998){
                    track.SetPtEtaPhi(1.0, m_tr_ohcal_eta[id], m_tr_ohcal_phi[id]); // for the track we only need the direction!
                    cluster.SetPtEtaPhi(m_cl_ohcal_e[i]/cosh(m_cl_ohcal_eta[i]), m_cl_ohcal_eta[i], m_cl_ohcal_phi[i]);
                    float dR = track.DeltaR(cluster);
                    if(dR < dRThreshold){
                        clusterContainer.addCluster(cluster, dR);
                    }
                }
            }
            break;
    }

    return clusterContainer;
}

MatchedClusterContainer IsotrackTreesAnalysis::getMatchedTowers(int id, caloType type, float dRThreshold){
    
    MatchedClusterContainer clusterContainer;
    TVector3 track, cluster;

    switch(type){

        case cemc:
            // Check if EMCal is reached
            for(int i = 0; i < m_twrmult_cemc; i++){ 
                if (m_twr_cemc_e[i] < 0.04) continue;
                if(m_tr_cemc_eta[id] > -998){
                    track.SetPtEtaPhi(1.0, m_tr_cemc_eta[id], m_tr_cemc_phi[id]); // for the track we only need the direction!
                    cluster.SetPtEtaPhi(m_twr_cemc_e[i]/cosh(m_twr_cemc_eta[i]), m_twr_cemc_eta[i], m_twr_cemc_phi[i]);
                    float dR = track.DeltaR(cluster);
                    if(dR < dRThreshold){
                        clusterContainer.addCluster(cluster, dR);
                    }
                }
            }
            break;

        case ihcal:
            // Check if IHCal is reached
            for(int i = 0; i < m_twrmult_ihcal; i++){ 
                if (m_twr_ihcal_e[i] < 0.006) continue;
                if(m_tr_ihcal_eta[id] > -998){
                    track.SetPtEtaPhi(1.0, m_tr_ihcal_eta[id], m_tr_ihcal_phi[id]); // for the track we only need the direction!
                    cluster.SetPtEtaPhi(m_twr_ihcal_e[i]/cosh(m_twr_ihcal_eta[i]), m_twr_ihcal_eta[i], m_twr_ihcal_phi[i]);
                    float dR = track.DeltaR(cluster);
                    if(dR < dRThreshold){
                        clusterContainer.addCluster(cluster, dR);
                    }
                }
            }
            break;

        case ohcal:
            // Check if OHCal is reached
            for(int i = 0; i < m_twrmult_ohcal; i++){
                if (m_twr_ohcal_e[i] < 0.006) continue;
                if(m_tr_ohcal_eta[id] > -998){
                    track.SetPtEtaPhi(1.0, m_tr_ohcal_eta[id], m_tr_ohcal_phi[id]); // for the track we only need the direction!
                    cluster.SetPtEtaPhi(m_twr_ohcal_e[i]/cosh(m_twr_ohcal_eta[i]), m_twr_ohcal_eta[i], m_twr_ohcal_phi[i]);
                    float dR = track.DeltaR(cluster);
                    if(dR < dRThreshold){
                        clusterContainer.addCluster(cluster, dR);
                    }
                }
            }
            break;
    }

    return clusterContainer;
}

MatchedClusterContainer IsotrackTreesAnalysis::getMatchedSimTowers(int id, caloType type, float dRThreshold){
    
    MatchedClusterContainer clusterContainer;
    TVector3 track, cluster;
    float cemc_sf = 0.02;
    float ihcal_sf = 0.162166;
    float ohcal_sf = 0.0338021;

    switch(type){

        case cemc:
            // Check if EMCal is reached
            for(int i = 0; i < m_simtwrmult_cemc; i++){ 
                if(m_tr_cemc_eta[id] > -998){
                    track.SetPtEtaPhi(1.0, m_tr_cemc_eta[id], m_tr_cemc_phi[id]); // for the track we only need the direction!
                    cluster.SetPtEtaPhi(m_simtwr_cemc_e[i]/(cemc_sf*cosh(m_simtwr_cemc_eta[i])), m_simtwr_cemc_eta[i], m_simtwr_cemc_phi[i]);
                    float dR = track.DeltaR(cluster);
                    if(dR < dRThreshold){
                        clusterContainer.addCluster(cluster, dR);
                    }
                }
            }
            break;

        case ihcal:
            // Check if IHCal is reached
            for(int i = 0; i < m_simtwrmult_ihcal; i++){ 
                if(m_tr_ihcal_eta[id] > -998){
                    track.SetPtEtaPhi(1.0, m_tr_ihcal_eta[id], m_tr_ihcal_phi[id]); // for the track we only need the direction!
                    cluster.SetPtEtaPhi(m_simtwr_ihcal_e[i]/(ihcal_sf*cosh(m_simtwr_ihcal_eta[i])), m_simtwr_ihcal_eta[i], m_simtwr_ihcal_phi[i]);
                    float dR = track.DeltaR(cluster);
                    if(dR < dRThreshold){
                        clusterContainer.addCluster(cluster, dR);
                    }
                }
            }
            break;

        case ohcal:
            // Check if OHCal is reached
            for(int i = 0; i < m_simtwrmult_ohcal; i++){
                if(m_tr_ohcal_eta[id] > -998){
                    track.SetPtEtaPhi(1.0, m_tr_ohcal_eta[id], m_tr_ohcal_phi[id]); // for the track we only need the direction!
                    cluster.SetPtEtaPhi(m_simtwr_ohcal_e[i]/(ohcal_sf*cosh(m_simtwr_ohcal_eta[i])), m_simtwr_ohcal_eta[i], m_simtwr_ohcal_phi[i]);
                    float dR = track.DeltaR(cluster);
                    if(dR < dRThreshold){
                        clusterContainer.addCluster(cluster, dR);
                    }
                }
            }
            break;
    }

    return clusterContainer;
}