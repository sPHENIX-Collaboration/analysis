/*!
 *  \file     AnaMvtxTelescopeHits.C
 *  \brief    Analyze Mvtx 4 ladder cosmic telescope in simulations
 *  \details  Analyze simulations of hits in the 4 ladder
 *            Mvtx cosimic ray telescope
 *  \author   Darren McGlinchey
 */

#include "AnaMvtxTelescopeHits.h"

#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>

#include <phgeom/PHGeomUtility.h>

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>

#include <g4hough/SvtxCluster.h>
#include <g4hough/SvtxClusterMap.h>
#include <g4hough/SvtxHitMap.h>
#include <g4hough/SvtxTrack.h>
#include <g4hough/SvtxVertex.h>
#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxVertexMap.h>

#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>

#include <g4detectors/PHG4CellContainer.h>
#include <g4detectors/PHG4CylinderGeomContainer.h>
#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CylinderGeom_MAPS.h>
#include <g4detectors/PHG4CylinderGeom_Siladders.h>

#include <phgenfit/Fitter.h>
#include <phgenfit/PlanarMeasurement.h>
#include <phgenfit/Track.h>
#include <phgenfit/SpacepointMeasurement.h>

//GenFit
#include <GenFit/FieldManager.h>
#include <GenFit/GFRaveConverters.h>
#include <GenFit/GFRaveVertex.h>
#include <GenFit/GFRaveVertexFactory.h>
#include <GenFit/MeasuredStateOnPlane.h>
#include <GenFit/RKTrackRep.h>
#include <GenFit/StateOnPlane.h>
#include <GenFit/Track.h>

//Rave
#include <rave/Version.h>
#include <rave/Track.h>
#include <rave/VertexFactory.h>
#include <rave/ConstantMagneticField.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>

#include <HFJetTruthGeneration/HFJetDefs.h>

#include "TTree.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TH1.h"
#include "TH2.h"


#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <memory>



#define LogDebug(exp)   std::cout<<"DEBUG: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogError(exp)   std::cout<<"ERROR: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogWarning(exp) std::cout<<"WARNING: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"


AnaMvtxTelescopeHits::AnaMvtxTelescopeHits(const std::string &name,
                           const std::string &ofName) :
  SubsysReco(name),
  _clustermap(NULL),
  _verbose(false),
  _foutname(ofName),
  _f(NULL),
  _primary_pid_guess(211),
  _track_fitting_alg_name("DafRef"),
  _fitter(NULL)
{


}

int AnaMvtxTelescopeHits::Init(PHCompositeNode *topNode)
{

  //-- Open file
  _f = new TFile(_foutname.c_str(), "RECREATE");

  //-- Initialize histograms
  hlayer = new TH1D("hlayer", ";layer", 6, -0.5, 5.5);

  for (int il = 0; il < 4; il++)
  {
    hsize_phi[il] = new TH1D(Form("hsize_phi_l%i", il),
                             ";cluster size #phi [cm]",
                             100, 0, 0.1);

    hsize_z[il] = new TH1D(Form("hsize_z_l%i", il),
                             ";cluster size z [cm]",
                             100, 0, 0.1);

    hphiz[il] = new TH2D(Form("hphiz_l%i", il),
                             ";z [cm]; #phi",
                             1000, -10, 10,
                             100, -0.1, 0.1);


  } // il



  return 0;

}

int AnaMvtxTelescopeHits::InitRun(PHCompositeNode *topNode)
{

  // TGeoManager* tgeo_manager = PHGeomUtility::GetTGeoManager(topNode);

  // _fitter = PHGenFit::Fitter::getInstance(tgeo_manager, _mag_field_file_name.data(),
  //                                         (_reverse_mag_field) ? -1. * _mag_field_re_scaling_factor : _mag_field_re_scaling_factor,
  //                                         _track_fitting_alg_name, "RKTrackRep", _do_evt_display);

  // if (!_fitter) {
  //   std::cerr << PHWHERE << std::endl;
  //   return Fun4AllReturnCodes::ABORTRUN;
  // }

  //-- set counters
  _ievent = -1; // since we count at the beginning of the event, start at -1


  return 0;

}

int AnaMvtxTelescopeHits::process_event(PHCompositeNode *topNode)
{

  // count event
  _ievent++;

  //------
  //-- Get the nodes
  //------
  int retval = GetNodes(topNode);
  if ( retval != Fun4AllReturnCodes::EVENT_OK )
    return retval;

  //------
  //-- Loop over clusters
  //------
  for (SvtxClusterMap::Iter iter = _clustermap->begin();
       iter != _clustermap->end();
       ++iter)
  {
    SvtxCluster *clus = iter->second;

    hlayer->Fill(clus->get_layer());

    int lyr = clus->get_layer();
    if (lyr < 0 || lyr > 3)
      continue;
    hsize_phi[lyr]->Fill(clus->get_phi_size());
    hsize_z[lyr]->Fill(clus->get_z_size());

    double phi = TMath::ATan2(clus->get_y(), clus->get_x());
    hphiz[lyr]->Fill(clus->get_z(), phi);
  }

  //-- Cleanup


  //-- End
  return 0;

}

int AnaMvtxTelescopeHits::End(PHCompositeNode * topNode)
{
    _f->Write();
    _f->Close();

  return 0;
}

/*
 * Get all the necessary nodes from the node tree
 */
int AnaMvtxTelescopeHits::GetNodes(PHCompositeNode * topNode)
{

  // Input Svtx Clusters
  _clustermap = findNode::getClass<SvtxClusterMap>(topNode, "SvtxClusterMap");
  if (!_clustermap && _ievent < 2) {
    std::cout << PHWHERE << " SvtxClusterMap node not found on node tree"
              << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return Fun4AllReturnCodes::EVENT_OK;

}


