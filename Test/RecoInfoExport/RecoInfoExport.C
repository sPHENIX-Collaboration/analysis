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

#include <g4cemc/RawTowerContainer.h>
#include <g4cemc/RawTowerGeomContainer.h>
#include <g4cemc/RawTower.h>
#include <g4cemc/RawClusterContainer.h>
#include <g4cemc/RawCluster.h>

#include <TH1D.h>
#include <TH2D.h>

#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <boost/format.hpp>

using namespace std;

RecoInfoExport::RecoInfoExport(const string &name) :
    SubsysReco(name), _event(0), _calo_names(
      { "CEMC", "HCALIN", "HCALOUT" })
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

      fdata << (boost::format("%1% (1..%2% hits)") % calo_name % towers->size())
          << endl;

  bool first = true;
      RawTowerContainer::ConstRange begin_end = towers->getTowers();
      RawTowerContainer::ConstIterator rtiter;
      for (rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter)
        {
          RawTower *tower = rtiter->second;
          assert(tower);

          float eta = towergeom->get_etacenter(tower->get_bineta());
          float phi = towergeom->get_phicenter(tower->get_binphi());

              if (first)
                {
                  first = false;
                }
              else
                fdata << ",";

              fdata <<(boost::format("[%1%,%2%,%3%]") % eta % phi % tower->get_energy());


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

          std::set<PHG4Hit*> g4hits = trutheval->all_truth_hits(g4particle);

          map<int, PHG4Hit *> layer_sort;
          for (auto & hit : g4hits)
            {
              if (hit->get_layer() != UINT_MAX)
                {
                  layer_sort[hit->get_layer()] = hit;
//              cout << "hit->get_layer() -> ";
//              hit->identify();
                }
            }

          if (layer_sort.size() > 5)
            {

              bool first = true;
              for (auto & hit_pair : layer_sort)
                {
                  if (first)
                    {
                      first = false;

                      fdata << "[";
                    }
                  else
                    fdata << ",";

                  fdata << "[";
                  fdata << hit_pair.second->get_avg_x();
                  fdata << ",";
                  fdata << hit_pair.second->get_avg_y();
                  fdata << ",";
                  fdata << hit_pair.second->get_avg_z();
                  fdata << "]";
                }

              if (first)
                {
                  fdata << "[";
                }
              fdata << "]" << endl;
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
