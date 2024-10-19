#pragma once
#include <numeric>
#include <vector>
#include <TVector3.h>

struct MatchedClusterContainer{
    // member variables
    std::vector<float> e;
    std::vector<float> eta;
    std::vector<float> phi;
    std::vector<float> dR;

    // add a new cluster to the container
    void addCluster(TVector3 cluster, float dR_track){
        e.push_back(cluster.Mag());
        eta.push_back(cluster.Eta());
        phi.push_back(cluster.Phi());
        dR.push_back(dR_track);
    }

    // get the number of clusters in the container
    int getNumberOfClusters(){
        return e.size();
    }

    // return total energy of all clusters in the container
    float getTotalEnergy(){
        return std::accumulate(e.begin(),e.end(), 0.0);
    }

    // return total energy of all clusters in the cluster within a delta_r cone 
    float getTotalEnergy(float clusterDr) {
        float e_temp = 0.0;
        for (int j = 0; j < dR.size(); j++) {
            if (dR[j] < clusterDr) {e_temp += e[j];}
        }
        return e_temp;
    }
};
