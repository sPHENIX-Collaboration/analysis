#ifndef __CHARGE_RECALC_H__
#define __CHARGE_RECALC_H__

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <trackbase/ActsGeometry.h>
#include <trackbase/TrkrClusterContainer.h>

#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/TrackSeed.h> // explicit (avoid relying on indirect includes)

#include <globalvertex/SvtxVertexMap.h>
#include <globalvertex/SvtxVertex.h>

#include <cmath>
#include <limits>
#include <iostream>

static inline float WrapPhi(float x)
{
    while (x > M_PI)
        x -= 2.0f * M_PI;
    while (x < -M_PI)
        x += 2.0f * M_PI;
    return x;
}

static inline float DeltaPhi(float a, float b)
{
    return WrapPhi(a - b);
}

class ChargeRecalc : public SubsysReco
{
public:
    ChargeRecalc(const std::string &name = "ChargeRecalc")
        : SubsysReco(name) {}
    void setVerbosity(int v) { m_verb = v; }

    void setClusterContainerName(const std::string &name) { 
      m_clusterContainerName = name;
    }

    int process_event(PHCompositeNode *topNode) override
    {
        auto clustermap = findNode::getClass<TrkrClusterContainer>(topNode, m_clusterContainerName);
        auto geometry = findNode::getClass<ActsGeometry>(topNode, m_actsgeometryName);
        auto vertexmap = findNode::getClass<SvtxVertexMap>(topNode, m_vertexMapName);
        auto trackmap = findNode::getClass<SvtxTrackMap>(topNode, m_trackMapName);
        if (!trackmap)
        {
            std::cout << PHWHERE << "Missing trackmap, can't continue" << std::endl;
            return Fun4AllReturnCodes::EVENT_OK;
        }
        if (!clustermap)
        {
            std::cout << PHWHERE << "Missing clustermap, can't continue" << std::endl;
            return Fun4AllReturnCodes::EVENT_OK;
        }
        if (!geometry)
        {
            std::cout << PHWHERE << "Missing geometry, can't continue" << std::endl;
            return Fun4AllReturnCodes::EVENT_OK;
        }

        if (m_verb > 0)
        {
             if (vertexmap && !vertexmap->empty())
             {
               std::cout<<"Ntrk: "<<trackmap->size()<<", Nvtx: "<<vertexmap->size()<<"  "<<std::endl;
               int ivtx=0;
               for (const auto &[k, v] : *vertexmap)
               {
                  if (v){
                   std::cout<<"vtx : "<< ivtx  <<" " 
                       << v->get_id() <<" " 
                       << v->get_x() <<" " 
                       << v->get_y() <<" " 
                       << v->get_z() <<" " 
                       << v->get_beam_crossing() <<std::endl;
                  }
                  ivtx++;
               }
             }
        }

        int n_changed = 0;
        int n_skipped = 0;
        int n_total = 0;

        for (auto &iter : *trackmap)
        {
            SvtxTrack *track = iter.second;
            if (!track)
                continue;
            n_total++;

//--            int vertexid = track->get_vertex_id();
//--            std::cout<<"vertexID : "<<vertexid<<std::endl;

            TrackSeed *si_seed = track->get_silicon_seed();
            if (!si_seed)
            {
                n_skipped++;
                continue;
            }

//--
//--          float xvtx = 0.0f;
//--          float yvtx = 0.0f;
//--
//--          bool found_vtx = false;
//--          if (vertexmap && !vertexmap->empty())
//--          {
//--              SvtxVertex *vtx0 = nullptr;
//--
//--              // crossing==0 has high priority
//--              for (const auto &[k, v] : *vertexmap)
//--              {
//--                  if (v && v->get_beam_crossing() == 0)
//--                  {
//--                      vtx0 = v;
//--                      break;
//--                  }
//--              }
//--
//--              if (!vtx0)
//--                  vtx0 = vertexmap->begin()->second;
//--
//--              if (vtx0)
//--              {
//--                  xvtx = vtx0->get_x();
//--                  yvtx = vtx0->get_y();
//--                  //--float zvtx = vtx0->get_z();
//--                  //--std::cout<<"Vtx : "<<xvtx<<" "<<yvtx<<" "<<zvtx<<std::endl;
//--                  found_vtx = true;
//--              }
//--          }
//--
//--          if (m_verb > 0 && !found_vtx)
//--          {
//--              std::cout << "[ChargeRecalc] WARNING: no usable vertex (vertexmap="
//--                        << (void *)vertexmap
//--                        << ", size=" << (vertexmap ? (int)vertexmap->size() : -1)
//--                        << ")" << std::endl;
//--              //--n_skipped++;
//--              //--continue;
//--          }
//--

            // find innermost/outermost cluster phi from vertex
//--            float rmin = std::numeric_limits<float>::max();
//--            float rmax = -1.0f;
            float phi_rmin = 0.0f;
            float phi_rmax = 0.0f;

            std::vector< Acts::Vector3 > vClus;
            for (auto key_iter = si_seed->begin_cluster_keys(); key_iter != si_seed->end_cluster_keys(); ++key_iter)
            {
                const auto &cluster_key = *key_iter;
                auto trkrCluster = clustermap->findCluster(cluster_key);
                if (!trkrCluster)
                    continue;

                auto global = geometry->getGlobalPosition(cluster_key, trkrCluster);

                vClus.push_back(global);

                //--int  layer =  TrkrDefs::getLayer(cluster_key);
                //--float clus_r = sqrt(global[0]*global[0] + global[1]*global[1]);
                //--std::cout<<"clus : "<<layer<<" "<<global[0]<<" "<<global[1]<<" "<<clus_r<<std::endl;

                // original algorithm
                //--const float dx = global[0] - xvtx;
                //--const float dy = global[1] - yvtx;
                //--const float r = sqrt(dx * dx + dy * dy);
                //--const float phi = std::atan2(dy, dx);

                //--if (r < rmin)
                //--{
                //--    rmin = r;
                //--    phi_rmin = phi;
                //--}
                //--if (r > rmax)
                //--{
                //--    rmax = r;
                //--    phi_rmax = phi;
                //--}
            }

            // original algorithm
            //--if (rmax < 0.0f)
            //--{
            //--    n_skipped++;
            //--    continue;
            //--}

            if(vClus.size()<3) { std::cout<<"Ncluster is "<<vClus.size()<<" <= 3. skip"<<std::endl;  continue; }

            phi_rmin = atan2(vClus[1][1]-vClus[0][1], vClus[1][0]-vClus[0][0]);
            phi_rmax = atan2(vClus[vClus.size()-1][1]-vClus[0][1], vClus[vClus.size()-1][0]-vClus[0][0]);

            const float dphi = DeltaPhi(phi_rmax, phi_rmin);
            const int q_new = (dphi > 0.0f) ? -1 : +1; // if official is flipped, swap -1/+1
            const int q_old = track->get_charge();

            if (q_new != q_old)
            {
                if (m_verb > 2)
                {
                    std::cout << "[ChargeRecalc] trkid=" << track->get_id()
                              << " before=" << track->get_charge();
                }
                track->set_charge(q_new);
                si_seed->set_qOverR(q_new * fabs(si_seed->get_qOverR()));
                if (m_verb > 2)
                {
                    std::cout << " after=" << track->get_charge() << std::endl;
                }
                n_changed++;
            }
        }
        if (m_verb > 1)
        {
            std::cout << "[ChargeRecalc] total=" << n_total
                      << " changed=" << n_changed
                      << " skipped=" << n_skipped
                      << std::endl;
        }
        return Fun4AllReturnCodes::EVENT_OK;
    }

private:
    // node names (match SiliconSeedsAna default)
    std::string m_clusterContainerName = "TRKR_CLUSTER";
    std::string m_actsgeometryName = "ActsGeometry";
    std::string m_trackMapName = "SvtxTrackMap";
    std::string m_vertexMapName = "SvtxVertexMap";

    int m_verb = 0;
};

#endif
