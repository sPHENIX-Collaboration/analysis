// Code for basic track selection

#include <TVector3.h>
#include "../IsotrackTreesAnalysis.h"

bool IsotrackTreesAnalysis::basicTrackSelection(int id){
    
    // Primary vertex selection condition
    // d0 < 0.15, z0 < 0.15
    float d0 = sqrt((m_tr_x[id]-m_vx[0])*(m_tr_x[id]-m_vx[0])+(m_tr_y[id]-m_vy[0])*(m_tr_y[id]-m_vy[0]));
    float z0 = fabs(m_tr_z[id]-m_vz[0]);

    if(!USE_PARTICLE_GUN && !(d0 < D0_CUT && z0 < Z0_CUT)){
        return false;
    }

    if (m_tr_cemc_eta[id] < -998 || m_tr_cemc_phi[id] < -998) {
        return false;
    }
    
    // Track momentum cut
    if(m_tr_pt[id] < PT_CUT || fabs(m_tr_cemc_eta[id]) > 1.0){
        return false;
    }

    // Track isolation condition
    TVector3 v1, v2;
    v1.SetPtEtaPhi(1.0, m_tr_cemc_eta[id], m_tr_cemc_phi[id]);

    for(int j = 0; j < m_trkmult; j++){
        if(id == j){ continue; }

        v2.SetPtEtaPhi(1.0, m_tr_cemc_eta[j], m_tr_cemc_phi[j]);

        if(v2.Pt() > MATCHED_PT_CUT && v1.DeltaR(v2) < MATCHED_DR_CUT){
            return false;
        }
    }

    return true;
}
