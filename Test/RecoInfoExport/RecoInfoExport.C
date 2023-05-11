#include "RecoInfoExport.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>
#include <phool/PHCompositeNode.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4Hit.h>

#include <g4hough/SvtxVertexMap.h>
#include <g4hough/SvtxVertex.h>
#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxTrack.h>

#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxTrackEval.h>
#include <g4eval/SvtxVertexEval.h>
#include <g4eval/SvtxTruthEval.h>

#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>

#include <TH1D.h>
#include <TH2D.h>
#include <TDatabasePDG.h>
#include <TVector3.h>

#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <boost/format.hpp>
#include <boost/math/special_functions/sign.hpp>

using namespace std;

RecoInfoExport::RecoInfoExport(const string &name) :
    SubsysReco(name), _event(0), _calo_names(
      { "CEMC", "HCALIN", "HCALOUT" }), _tower_threshold(0), _pT_threshold(0), _min_track_hit_dist(
        0)
{
}

int
RecoInfoExport::Init(PHCompositeNode *topNode)
{

  return 0;
}

int
RecoInfoExport::process_event(PHCompositeNode *topNode)
{
  ++_event;

  stringstream fname;
  fname << _file_prefix << "_Event" << _event << ".dat";
  fstream fdata(fname.str(), ios_base::out);

  for (auto & calo_name : _calo_names)
    {
      string towernodename = "TOWER_CALIB_" + calo_name;
      // Grab the towers
      RawTowerContainer* towers = findNode::getClass<RawTowerContainer>(topNode,
          towernodename.c_str());
      if (!towers)
        {
          std::cout << PHWHERE << ": Could not find node "
              << towernodename.c_str() << std::endl;
          return Fun4AllReturnCodes::ABORTRUN;
        }
      string towergeomnodename = "TOWERGEOM_" + calo_name;
      RawTowerGeomContainer *towergeom = findNode::getClass<
          RawTowerGeomContainer>(topNode, towergeomnodename.c_str());
      if (!towergeom)
        {
          cout << PHWHERE << ": Could not find node "
              << towergeomnodename.c_str() << endl;
          return Fun4AllReturnCodes::ABORTRUN;
        }

      set<const RawTower *> good_towers;

      RawTowerContainer::ConstRange begin_end = towers->getTowers();
      RawTowerContainer::ConstIterator rtiter;
      for (rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter)
        {
          const RawTower *tower = rtiter->second;
          assert(tower);
          if (tower->get_energy() > _tower_threshold)
            {
              good_towers.insert(tower);
            }
        }

      fdata
          << (boost::format("%1% (1..%2% hits)") % calo_name
              % good_towers.size()) << endl;

      bool first = true;
      for (const auto & tower : good_towers)
        {
          assert(tower);

          float eta = towergeom->get_etacenter(tower->get_bineta());
          float phi = towergeom->get_phicenter(tower->get_binphi());

          phi = atan2(cos(phi), sin(phi));

          if (first)
            {
              first = false;
            }
          else
            fdata << ",";

          fdata
              << (boost::format("[%1%,%2%,%3%]") % eta % phi
                  % tower->get_energy());

        }

      fdata << endl;
    }

    {
      fdata << "Track list" << endl;

      // need things off of the DST...
      PHG4TruthInfoContainer* truthinfo = findNode::getClass<
          PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
      if (!truthinfo)
        {
          cerr << PHWHERE << " ERROR: Can't find G4TruthInfo" << endl;
          exit(-1);
        }

      // create SVTX eval stack
      SvtxEvalStack svtxevalstack(topNode);

//  SvtxVertexEval* vertexeval = svtxevalstack.get_vertex_eval();
//  SvtxTrackEval* trackeval = svtxevalstack.get_track_eval();
      SvtxTruthEval* trutheval = svtxevalstack.get_truth_eval();

      // loop over all truth particles
      PHG4TruthInfoContainer::Range range =
          truthinfo->GetPrimaryParticleRange();
      for (PHG4TruthInfoContainer::ConstIterator iter = range.first;
          iter != range.second; ++iter)
        {
          PHG4Particle* g4particle = iter->second;

          const TVector3 mom(g4particle->get_px(), g4particle->get_py(),
              g4particle->get_pz());

          std::set<PHG4Hit*> g4hits = trutheval->all_truth_hits(g4particle);

          map<float, PHG4Hit *> time_sort;
          map<float, PHG4Hit *> layer_sort;
          for (auto & hit : g4hits)
            {
              if (hit)
                {
                  time_sort[hit->get_avg_t()] = hit;
                }
            }

          for (auto & hit_pair : time_sort)
            {

              if (hit_pair.second->get_layer() != UINT_MAX
                  and layer_sort.find(hit_pair.second->get_layer())
                      == layer_sort.end())
                {
                  layer_sort[hit_pair.second->get_layer()] = hit_pair.second;
                }
            }

          if (layer_sort.size() > 5 and mom.Pt() > _pT_threshold) // minimal track length cut
            {

              stringstream spts;

              TVector3 last_pos(0, 0, 0);

              bool first = true;
              for (auto & hit_pair : layer_sort)
                {
                  TVector3 pos(hit_pair.second->get_avg_x(),
                      hit_pair.second->get_avg_y(),
                      hit_pair.second->get_avg_z());

                  // hit step cuts
                  if ((pos - last_pos).Mag() < _min_track_hit_dist
                      and hit_pair.first != (layer_sort.rbegin()->first)
                      and hit_pair.first != (layer_sort.begin()->first))
                    continue;

                  last_pos = pos;

                  if (first)
                    {
                      first = false;
                    }
                  else
                    spts << ",";

                  spts << "[";
                  spts << pos.x();
                  spts << ",";
                  spts << pos.y();
                  spts << ",";
                  spts << pos.z();
                  spts << "]";
                }

              const int abs_pid = abs(g4particle->get_pid());
              int t = 5;
              if (abs_pid
                  == TDatabasePDG::Instance()->GetParticle("pi+")->PdgCode())
                {
                  t = 1;
                }
              else if (abs_pid
                  == TDatabasePDG::Instance()->GetParticle("proton")->PdgCode())
                {
                  t = 2;
                }
              else if (abs_pid
                  == TDatabasePDG::Instance()->GetParticle("K+")->PdgCode())
                {
                  t = 3;
                }
              else if (abs_pid
                  == TDatabasePDG::Instance()->GetParticle("e-")->PdgCode())
                {
                  t = 3;
                }

              const TParticlePDG * pdg_part =
                  TDatabasePDG::Instance()->GetParticle(11);
              const int c =
                  (pdg_part != nullptr) ? (copysign(1, pdg_part->Charge())) : 0;

              fdata
                  << (boost::format(
                      "{ \"pt\": %1%, \"t\": %2%, \"e\": %3%, \"p\": %4%, \"c\": %5%, \"pts\":[ %6% ]}")
                      % mom.Pt() % t % mom.PseudoRapidity() % mom.Phi() % c
                      % spts.str()) << endl;

            }
        }
    }

  fdata.close();
  return 0;
}

int
RecoInfoExport::End(PHCompositeNode *topNode)
{
  return 0;
}
