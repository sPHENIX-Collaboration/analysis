/*!
 *  \file   BDiJetModule.C
 *  \brief    Gether information from Truth Jet and Tracks, DST -> NTuple
 *  \details  Gether information from Truth Jet and Tracks, DST -> NTuple
 *  \author   Dennis V. Perepelitsa
 */

#include "BDiJetModule.h"

#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>

#include <phgeom/PHGeomUtility.h>
#include <phfield/PHFieldUtility.h>

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


#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <memory>



#define LogDebug(exp)   std::cout<<"DEBUG: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogError(exp)   std::cout<<"ERROR: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogWarning(exp) std::cout<<"WARNING: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"


BDiJetModule::BDiJetModule(const std::string &name,
                           const std::string &ofName) :
  SubsysReco(name),
  _jetmap_truth(NULL),
  _clustermap(NULL),
  _trackmap(NULL),
  _vertexmap(NULL),
  _verbose(false),
  _write_tree(true),
  _ana_truth(true),
  _ana_reco(true),
  _do_evt_display(false),
  _use_ladder_geom(false),
  _cut_jet(true),
  _cut_Ncluster(false),
  _foutname(ofName),
  _f(NULL),
  _tree(NULL),
  _primary_pid_guess(211),
  _cut_min_pT(0.1),
  _cut_chi2_ndf(5.0),
  _cut_jet_pT(10.0),
  _cut_jet_eta(0.7),
  _cut_jet_R(0.4),
  _track_fitting_alg_name("DafRef"),
  _fitter(NULL),
  _vertexing_method("avf-smoothing:1"),
  _vertex_finder(NULL)
{


}

int BDiJetModule::Init(PHCompositeNode *topNode)
{

  //-- set up the tree
  if ( _write_tree )
  {
    ResetVariables();
    InitTree();
  }

  return 0;

}

int BDiJetModule::InitRun(PHCompositeNode *topNode)
{

  TGeoManager* tgeo_manager = PHGeomUtility::GetTGeoManager(topNode);
  PHField * field = PHFieldUtility::GetFieldMapNode(nullptr, topNode);

  _fitter = PHGenFit::Fitter::getInstance(tgeo_manager, field,
                                          _track_fitting_alg_name, "RKTrackRep", _do_evt_display);

  if (!_fitter) {
    std::cerr << PHWHERE << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  _vertex_finder = new genfit::GFRaveVertexFactory(verbosity);
  _vertex_finder->setMethod(_vertexing_method.data());

  if (!_vertex_finder) {
    std::cerr << PHWHERE << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }


  //-- set counters
  _ievent = -1; // since we count at the beginning of the event, start at -1
  _b_event = -1;


  return 0;

}

int BDiJetModule::process_event(PHCompositeNode *topNode)
{

  // count event
  _ievent++;
  _b_event = _ievent;

  //-- reset tree variables
  if ( _write_tree )
    ResetVariables();

  //------
  //-- Get the nodes
  //------
  int retval = GetNodes(topNode);
  if ( retval != Fun4AllReturnCodes::EVENT_OK )
    return retval;

  //------
  //-- Analyze truth jets
  //------
  if ( _ana_truth )
  {
    _truthjet_n = 0;

    int ijet_t = 0;
    for (JetMap::Iter iter = _jetmap_truth->begin(); iter != _jetmap_truth->end(); ++iter)
    {
      Jet* this_jet = iter->second;

      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();

      // this_jet->get_property(static_cast<Jet::PROPERTY>(prop_JetPartonFlavor)) != 5)
      if (this_jet->get_pt() < _cut_jet_pT || fabs(this_eta) > _cut_jet_eta)
        continue;

      if ( _write_tree )
      {
        _truthjet_parton_flavor[_truthjet_n] = this_jet->get_property(static_cast<Jet::PROPERTY>(prop_JetPartonFlavor));
        _truthjet_hadron_flavor[_truthjet_n] = this_jet->get_property(static_cast<Jet::PROPERTY>(prop_JetHadronFlavor));
        _truthjet_pt[_truthjet_n] = this_pt;
        _truthjet_phi[_truthjet_n] = this_phi;
        _truthjet_eta[_truthjet_n] = this_eta;
      }

      if (_verbose)
        std::cout << " truth jet #" << ijet_t << ", pt / eta / phi = "
                  << this_pt << " / " << this_eta << " / " << this_phi
                  << std::endl;

      _truthjet_n++;
    }

    // only care if the event has a jet of interest
    if ( _truthjet_n < 1 )
      return Fun4AllReturnCodes::ABORTEVENT;

  } // _ana_truth

  //------
  //-- Analyze reconstructed information
  //------
  if ( _ana_reco )
  {

    //! stands for Refit_GenFit_Tracks
    std::vector<genfit::Track*> rf_gf_tracks;
    rf_gf_tracks.clear();
    std::vector<PHGenFit::Track*> rf_phgf_tracks;
    rf_phgf_tracks.clear();

    std::map<unsigned int, unsigned int> svtxtrk_gftrk_map;
    std::map<unsigned int, unsigned int> svtxtrk_nmvtx_map;
    std::map<unsigned int, unsigned int> svtxtrk_nintt_map;

    SvtxVertex *vertex = _vertexmap->get(0);

    for (SvtxTrackMap::Iter iter = _trackmap->begin();
         iter != _trackmap->end();
         ++iter)
    {
      SvtxTrack* svtx_track = iter->second;
      if (!svtx_track)
        continue;
      if (!(svtx_track->get_pt() > _cut_min_pT))
        continue;
      if ((svtx_track->get_chisq() / svtx_track->get_ndf()) > _cut_chi2_ndf)
        continue;

      int n_MVTX = 0, n_INTT = 0;
      for (SvtxTrack::ConstClusterIter iter2 = svtx_track->begin_clusters();
           iter2 != svtx_track->end_clusters();
           iter2++)
      {

        SvtxCluster* cluster = _clustermap->get(*iter2);
        unsigned int layer = cluster->get_layer();

        if (layer < 3) n_MVTX++;
        else if (layer < 7) n_INTT++;
      } // iter2 over SvtxTrack::ConstClusterIter

      //cout << "n MVTX: " << n_MVTX << ", n INTT: " << n_INTT << std::endl;

      /*
      //if (n_MVTX<2 || n_INTT<2)
      if (n_MVTX<2)
        continue;
      */

      PHGenFit::Track* rf_phgf_track = MakeGenFitTrack(topNode, svtx_track, vertex);

      if (rf_phgf_track)
      {
        svtxtrk_gftrk_map.insert(std::pair<int, int>(svtx_track->get_id(), rf_phgf_tracks.size()));
        svtxtrk_nmvtx_map.insert(std::pair<int, int>(svtx_track->get_id(), n_MVTX));
        svtxtrk_nintt_map.insert(std::pair<int, int>(svtx_track->get_id(), n_INTT));
        rf_phgf_tracks.push_back(rf_phgf_track);

        if ( _cut_Ncluster )
        {
          if ( n_MVTX >= 1 && n_INTT >= 2 )
            rf_gf_tracks.push_back(rf_phgf_track->getGenFitTrack());
        }
        else
        {
          rf_gf_tracks.push_back(rf_phgf_track->getGenFitTrack());
        }

      }
    } // iter over SvtxTrackMap

    //! find vertex using tracks
    std::vector<genfit::GFRaveVertex*> rave_vertices;
    rave_vertices.clear();
    //!

    if ( _vertexing_method.compare("avr-smoothing:1") != 0 )
      _vertex_finder->setMethod(_vertexing_method.data());
    if (rf_gf_tracks.size() >= 2)
    {
      try {
        _vertex_finder->findVertices(&rave_vertices, rf_gf_tracks);
      } catch (...) {
        std::cout << PHWHERE << "GFRaveVertexFactory::findVertices failed!";
      }
    }

    FillVertexMap(rave_vertices, rf_gf_tracks);

    //! change the vertex finding method
    if ( _vertexing_method.compare("avr-smoothing:1") != 0 )
      _vertex_finder->setMethod("avr-smoothing:1");
    std::vector<genfit::GFRaveVertex*> rave_vertices_jet;
    rave_vertices_jet.clear();

    //! scan jetmap
    for (JetMap::ConstIter iter = _jetmap_truth->begin(); iter != _jetmap_truth->end(); iter++)
    {
      Jet *jet = iter->second;

      float jet_pT = jet->get_pt();
      float jet_eta = jet->get_eta();
      float jet_phi = jet->get_phi();

      if ( jet_pT < _cut_jet_pT ) continue;
      if ( fabs(jet_eta) > _cut_jet_eta ) continue;

      int counter_r10 = 0, counter_miss = 0;

      std::vector<genfit::Track*> rf_gf_tracks_jet;
      rf_gf_tracks_jet.clear();
      // vector<genfit::Track*> rf_gf_tracks_jet_pT05;
      // rf_gf_tracks_jet_pT05.clear();
      // vector<genfit::Track*> rf_gf_tracks_jet_pT10;
      // rf_gf_tracks_jet_pT10.clear();
      // vector<genfit::Track*> rf_gf_tracks_jet_pT15;
      // rf_gf_tracks_jet_pT15.clear();
      // vector<genfit::Track*> rf_gf_tracks_jet_pT20;
      // rf_gf_tracks_jet_pT20.clear();

      //cout << "JET ETA: " << jet_eta << ", JET PHI: " << jet_phi << std::endl;

      for (SvtxTrackMap::ConstIter iter2 = _trackmap->begin(); iter2 != _trackmap->end(); iter2++)
      {
        SvtxTrack* svtx_track = iter2->second;

        float trk_phi = svtx_track->get_phi();
        float trk_eta = svtx_track->get_eta();
        // float trk_pT = svtx_track->get_pt();

        float sin_phi = sin(jet_phi - trk_phi);
        float cos_phi = cos(jet_phi - trk_phi);
        float dphi = atan2(sin_phi, cos_phi);

        if (sqrt((jet_eta - trk_eta) * (jet_eta - trk_eta) + dphi * dphi) < 1.0)
        {
          counter_r10++;
        }

        if (sqrt((jet_eta - trk_eta) * (jet_eta - trk_eta) + dphi * dphi) > _cut_jet_R) continue;

        if (svtxtrk_gftrk_map.find(svtx_track->get_id()) != svtxtrk_gftrk_map.end()) {
          unsigned int trk_index = svtxtrk_gftrk_map[svtx_track->get_id()];
          unsigned int nclus_mvtx = svtxtrk_nmvtx_map[svtx_track->get_id()];
          unsigned int nclus_intt = svtxtrk_nintt_map[svtx_track->get_id()];

          //cout << "NCLUS MVTX: " << nclus_mvtx << std::endl;

          PHGenFit::Track* rf_phgf_track = rf_phgf_tracks.at(trk_index);

          if ( _cut_Ncluster ) {
            if ( nclus_mvtx >= 1 && nclus_intt >= 2 ) {
              rf_gf_tracks_jet.push_back(rf_phgf_track->getGenFitTrack());
              // if (trk_pT > 0.5) rf_gf_tracks_jet_pT05.push_back(rf_phgf_track->getGenFitTrack());
              // if (trk_pT > 1.0) rf_gf_tracks_jet_pT10.push_back(rf_phgf_track->getGenFitTrack());
              // if (trk_pT > 1.5) rf_gf_tracks_jet_pT15.push_back(rf_phgf_track->getGenFitTrack());
              // if (trk_pT > 2.0) rf_gf_tracks_jet_pT20.push_back(rf_phgf_track->getGenFitTrack());
            }
          } else {
            rf_gf_tracks_jet.push_back(rf_phgf_track->getGenFitTrack());
            // if (trk_pT > 0.5) rf_gf_tracks_jet_pT05.push_back(rf_phgf_track->getGenFitTrack());
            // if (trk_pT > 1.0) rf_gf_tracks_jet_pT10.push_back(rf_phgf_track->getGenFitTrack());
            // if (trk_pT > 1.5) rf_gf_tracks_jet_pT15.push_back(rf_phgf_track->getGenFitTrack());
            // if (trk_pT > 2.0) rf_gf_tracks_jet_pT20.push_back(rf_phgf_track->getGenFitTrack());
          }
        } else {
          counter_miss++;
        }
      }//trackmap

      //! SV reco
      //!
      if (rf_gf_tracks_jet.size() > 1) {
        try {
          _vertex_finder->findVertices(&rave_vertices_jet, rf_gf_tracks_jet);
        } catch (...) {
          std::cout << PHWHERE << "GFRaveVertexFactory::findVertices failed!";
        }
      }

      //cout << "N MISS: " << counter_miss << std::endl;
      if ( verbosity > 0 )
      {
        std::cout << "JET PT: " << jet_pT
                  << ", N TRK: " << int(jet->size_comp())
                  << ", CUT10: " << counter_r10
                  << ", CUT04: " << rf_gf_tracks_jet.size()
                  << ", N VTX: " << rave_vertices_jet.size()
                  << std::endl;
      }

      if ( _write_tree )
      {
        rv_sv_pT00_nvtx[rv_sv_njets] = rave_vertices_jet.size();
        for (int ivtx = 0; ivtx < int(rave_vertices_jet.size()); ivtx++) {
          genfit::GFRaveVertex* rave_vtx = rave_vertices_jet[ivtx];
          rv_sv_pT00_vtx_x[rv_sv_njets][ivtx] = rave_vtx->getPos().X();
          rv_sv_pT00_vtx_y[rv_sv_njets][ivtx] = rave_vtx->getPos().Y();
          rv_sv_pT00_vtx_z[rv_sv_njets][ivtx] = rave_vtx->getPos().Z();

          rv_sv_pT00_vtx_ex[rv_sv_njets][ivtx] = sqrt(rave_vtx->getCov()[0][0]);
          rv_sv_pT00_vtx_ey[rv_sv_njets][ivtx] = sqrt(rave_vtx->getCov()[1][1]);
          rv_sv_pT00_vtx_ez[rv_sv_njets][ivtx] = sqrt(rave_vtx->getCov()[2][2]);

          rv_sv_pT00_vtx_ntrk[rv_sv_njets][ivtx] = (int)rave_vtx->getNTracks();

          float vtx_mass, vtx_px, vtx_py, vtx_pz;
          rv_sv_pT00_vtx_ntrk_good[rv_sv_njets][ivtx] = GetSVMass_mom(rave_vtx, vtx_mass, vtx_px, vtx_py, vtx_pz);

          //cout << "N TRK: " << rv_sv_pT00_vtx_ntrk[rv_sv_njets][ivtx] << ", GOOD: " << rv_sv_pT00_vtx_ntrk_good[rv_sv_njets][ivtx] << std::endl;

          TVector3 vec1(vtx_px, vtx_py, vtx_pz);
          TVector3 vec2(rv_sv_pT00_vtx_x[rv_sv_njets][ivtx] - rv_prim_vtx[0], rv_sv_pT00_vtx_y[rv_sv_njets][ivtx] - rv_prim_vtx[1], rv_sv_pT00_vtx_z[rv_sv_njets][ivtx] - rv_prim_vtx[2]);
          float theta = vec1.Angle(vec2);
          float A = vec1.Mag() * sin(theta);
          float vtx_mass_corr = sqrt(vtx_mass * vtx_mass + A * A) + A;

          rv_sv_pT00_vtx_mass[rv_sv_njets][ivtx] = vtx_mass;
          rv_sv_pT00_vtx_mass_corr[rv_sv_njets][ivtx] = vtx_mass_corr;
          rv_sv_pT00_vtx_pT[rv_sv_njets][ivtx] = sqrt(vtx_px * vtx_px + vtx_py * vtx_py);
        }
      }

      for (genfit::GFRaveVertex* vertex : rave_vertices_jet) {
        delete vertex;
      }
      rave_vertices_jet.clear();


      //! jet information
      rv_sv_jet_id[rv_sv_njets] = jet->get_id();
      rv_sv_jet_pT[rv_sv_njets] = jet_pT;
      rv_sv_jet_phi[rv_sv_njets] = jet_phi;
      if (jet->has_property(static_cast<Jet::PROPERTY>(prop_JetPartonFlavor))) {
        rv_sv_jet_prop[rv_sv_njets][0] = int(jet->get_property(static_cast<Jet::PROPERTY>(prop_JetPartonFlavor)));
        rv_sv_jet_prop[rv_sv_njets][1] = int(jet->get_property(static_cast<Jet::PROPERTY>(prop_JetHadronFlavor)));
      }

      rv_sv_njets++;

    }




  } // _ana_reco


  //-- Cleanup

  // fill tree
  if ( _write_tree )
    _tree->Fill();

  //-- End
  return 0;

}

int BDiJetModule::End(PHCompositeNode * topNode)
{
  if ( _write_tree )
  {
    _f->Write();
    _f->Close();
  }

  return 0;
}

/*
 * Initialize the TTree
 */
void BDiJetModule::InitTree()
{
  _f = new TFile(_foutname.c_str(), "RECREATE");

  _tree = new TTree("ttree", "a withered deciduous tree");

  _tree->Branch("event", &_b_event, "event/I");

  if ( _ana_truth )
  {
    _tree->Branch("truthjet_n", &_truthjet_n, "truthjet_n/I");
    _tree->Branch("truthjet_parton_flavor", _truthjet_parton_flavor, "truthjet_parton_flavor[truthjet_n]/I");
    _tree->Branch("truthjet_hadron_flavor", _truthjet_hadron_flavor, "truthjet_hadron_flavor[truthjet_n]/I");
    _tree->Branch("truthjet_pt", _truthjet_pt, "truthjet_pt[truthjet_n]/F");
    _tree->Branch("truthjet_eta", _truthjet_eta,
                  "truthjet_eta[truthjet_n]/F");
    _tree->Branch("truthjet_phi", _truthjet_phi,
                  "truthjet_phi[truthjet_n]/F");
  }

  if ( _ana_reco )
  {
    _tree->Branch("gf_prim_vtx", gf_prim_vtx, "gf_prim_vtx[3]/F");
    _tree->Branch("gf_prim_vtx_err", gf_prim_vtx_err, "gf_prim_vtx_err[3]/F");
    _tree->Branch("gf_prim_vtx_ntrk", &gf_prim_vtx_ntrk, "gf_prim_vtx_ntrk/I");
    _tree->Branch("rv_prim_vtx", rv_prim_vtx, "rv_prim_vtx[3]/F");
    _tree->Branch("rv_prim_vtx_err", rv_prim_vtx_err, "rv_prim_vtx_err[3]/F");
    _tree->Branch("rv_prim_vtx_ntrk", &rv_prim_vtx_ntrk, "rv_prim_vtx_ntrk/I");

    _tree->Branch("rv_sv_njets", &rv_sv_njets, "rv_sv_njets/I");
    _tree->Branch("rv_sv_jet_id", rv_sv_jet_id, "rv_sv_jet_id[rv_sv_njets]/I");
    _tree->Branch("rv_sv_jet_prop", rv_sv_jet_prop, "rv_sv_jet_prop[rv_sv_njets][2]/I");
    _tree->Branch("rv_sv_jet_pT", rv_sv_jet_pT, "rv_sv_jet_pT[rv_sv_njets]/F");
    _tree->Branch("rv_sv_jet_phi", rv_sv_jet_phi, "rv_sv_jet_phi[rv_sv_njets]/F");

    _tree->Branch("rv_sv_pT00_nvtx", rv_sv_pT00_nvtx, "rv_sv_pT00_nvtx[rv_sv_njets]/I");
    _tree->Branch("rv_sv_pT00_vtx_x", rv_sv_pT00_vtx_x, "rv_sv_pT00_vtx_x[rv_sv_njets][30]/F");
    _tree->Branch("rv_sv_pT00_vtx_y", rv_sv_pT00_vtx_y, "rv_sv_pT00_vtx_y[rv_sv_njets][30]/F");
    _tree->Branch("rv_sv_pT00_vtx_z", rv_sv_pT00_vtx_z, "rv_sv_pT00_vtx_z[rv_sv_njets][30]/F");
    _tree->Branch("rv_sv_pT00_vtx_ex", rv_sv_pT00_vtx_ex, "rv_sv_pT00_vtx_ex[rv_sv_njets][30]/F");
    _tree->Branch("rv_sv_pT00_vtx_ey", rv_sv_pT00_vtx_ey, "rv_sv_pT00_vtx_ey[rv_sv_njets][30]/F");
    _tree->Branch("rv_sv_pT00_vtx_ez", rv_sv_pT00_vtx_ez, "rv_sv_pT00_vtx_ez[rv_sv_njets][30]/F");
    _tree->Branch("rv_sv_pT00_vtx_ntrk", rv_sv_pT00_vtx_ntrk, "rv_sv_pT00_vtx_ntrk[rv_sv_njets][30]/I");
    _tree->Branch("rv_sv_pT00_vtx_ntrk_good", rv_sv_pT00_vtx_ntrk_good, "rv_sv_pT00_vtx_ntrk_good[rv_sv_njets][30]/I");
    _tree->Branch("rv_sv_pT00_vtx_mass", rv_sv_pT00_vtx_mass, "rv_sv_pT00_vtx_mass[rv_sv_njets][30]/F");
    _tree->Branch("rv_sv_pT00_vtx_mass_corr", rv_sv_pT00_vtx_mass_corr, "rv_sv_pT00_vtx_mass_corr[rv_sv_njets][30]/F");
    _tree->Branch("rv_sv_pT00_vtx_pT", rv_sv_pT00_vtx_pT, "rv_sv_pT00_vtx_pT[rv_sv_njets][30]/F");
  }

}


/*
 * Reset all the TTree variables
 * Should be called for each event
 */
void BDiJetModule::ResetVariables()
{

  //-- Truth jet info
  _truthjet_n = 0;
  for (int n = 0; n < 10; n++)
  {
    _truthjet_parton_flavor[n] = -9999;
    _truthjet_hadron_flavor[n] = -9999;

    _truthjet_pt[n] = -1;
    _truthjet_eta[n] = -1;
    _truthjet_phi[n] = -1;
  }

  //-- Reco info
  gf_prim_vtx_ntrk = rv_prim_vtx_ntrk = 0;
  for (int i = 0; i < 3; i++)
  {
    gf_prim_vtx[i] = gf_prim_vtx_err[i] = -999;
    rv_prim_vtx[i] = rv_prim_vtx_err[i] = -999;
  }//i

  rv_sv_njets = 0;

  for (int ijet = 0; ijet < 10; ijet++)
  {
    rv_sv_jet_id[ijet] = -999;
    rv_sv_jet_prop[ijet][0] = -999;
    rv_sv_jet_prop[ijet][1] = -999;
    rv_sv_jet_pT[ijet] = -999;
    rv_sv_jet_phi[ijet] = -999;

    rv_sv_pT00_nvtx[ijet] = 0;

    for (int ivtx = 0; ivtx < 30; ivtx++) {
      rv_sv_pT00_vtx_ntrk[ijet][ivtx] = 0;
      rv_sv_pT00_vtx_ntrk_good[ijet][ivtx] = 0;

      rv_sv_pT00_vtx_mass[ijet][ivtx] = -999;
      rv_sv_pT00_vtx_mass_corr[ijet][ivtx] = -999;
      rv_sv_pT00_vtx_pT[ijet][ivtx] = -999;

      rv_sv_pT00_vtx_x[ijet][ivtx] = -999;
      rv_sv_pT00_vtx_y[ijet][ivtx] = -999;
      rv_sv_pT00_vtx_z[ijet][ivtx] = -999;
      rv_sv_pT00_vtx_ex[ijet][ivtx] = -999;
      rv_sv_pT00_vtx_ey[ijet][ivtx] = -999;
      rv_sv_pT00_vtx_ez[ijet][ivtx] = -999;

    }//ivtx
  }//ijet

}

/*
 * Get all the necessary nodes from the node tree
 */
int BDiJetModule::GetNodes(PHCompositeNode * topNode)
{

  // Truth jet node
  _jetmap_truth = findNode::getClass<JetMap>(topNode,
                  "AntiKt_Truth_r04");
  if ( !_jetmap_truth && _ana_truth && _ievent < 2)
  {
    std::cout << PHWHERE << " AntiKt_Truth_r04 node not found ... aborting" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // Input Svtx Clusters
  _clustermap = findNode::getClass<SvtxClusterMap>(topNode, "SvtxClusterMap");
  if (!_clustermap && _ana_reco && _ievent < 2) {
    std::cout << PHWHERE << " SvtxClusterMap node not found on node tree"
              << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // Input Svtx Tracks
  _trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if (!_trackmap && _ana_reco && _ievent < 2) {
    std::cout << PHWHERE << " SvtxClusterMap node not found on node tree"
              << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // Input Svtx Vertices
  _vertexmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
  if (!_vertexmap && _ana_reco && _ievent < 2) {
    std::cout << PHWHERE << " SvtxVertexrMap node not found on node tree"
              << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }



  return Fun4AllReturnCodes::EVENT_OK;

}


/*
 * Load reconstructed track into GenFit track for input to Rave
 */
PHGenFit::Track* BDiJetModule::MakeGenFitTrack(PHCompositeNode * topNode,
    const SvtxTrack * intrack,
    const SvtxVertex * vertex)
{


  if (!intrack) {
    std::cerr << PHWHERE << " Input SvtxTrack is NULL!" << std::endl;
    return NULL;
  }

  SvtxHitMap* hitsmap = NULL;
  hitsmap = findNode::getClass<SvtxHitMap>(topNode, "SvtxHitMap");
  if (!hitsmap) {
    std::cout << PHWHERE << "ERROR: Can't find node SvtxHitMap" << std::endl;
    return NULL;
  }

  PHG4CellContainer* cells_svtx = findNode::getClass<PHG4CellContainer>(topNode, "G4CELL_SVTX");
  PHG4CellContainer* cells_intt = findNode::getClass<PHG4CellContainer>(topNode, "G4CELL_SILICON_TRACKER");
  PHG4CellContainer* cells_maps = findNode::getClass<PHG4CellContainer>(topNode, "G4CELL_MAPS");

  if (_use_ladder_geom and !cells_svtx and !cells_intt and !cells_maps) {
    std::cout << PHWHERE << "No PHG4CellContainer found!" << std::endl;
    return NULL;
  }

  PHG4CylinderGeomContainer* geom_container_intt = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_SILICON_TRACKER");
  PHG4CylinderGeomContainer* geom_container_maps = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_MAPS");

  if (_use_ladder_geom and !geom_container_intt and !geom_container_maps) {
    std::cout << PHWHERE << "No PHG4CylinderGeomContainer found!" << std::endl;
    return NULL;
  }

  // prepare seed
  TVector3 seed_mom(100, 0, 0);
  TVector3 seed_pos(0, 0, 0);
  TMatrixDSym seed_cov(6);
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      seed_cov[i][j] = 100.;
    }
  }

  /*
  TVector3 seed_pos(intrack->get_x(), intrack->get_y(), intrack->get_z());
  TVector3 seed_mom(intrack->get_px(), intrack->get_py(), intrack->get_pz());
  TMatrixDSym seed_cov(6);
  for (int i=0; i<6; i++){
    for (int j=0; j<6; j++){
      seed_cov[i][j] = intrack->get_error(i,j);
    }
  }
  */

  // Create measurements
  std::vector<PHGenFit::Measurement*> measurements;


  std::map<float, unsigned int> m_r_cluster_id;

  for (auto iter = intrack->begin_clusters(); iter != intrack->end_clusters(); ++iter) {
    unsigned int cluster_id = *iter;
    SvtxCluster* cluster = _clustermap->get(cluster_id);
    float x = cluster->get_x();
    float y = cluster->get_y();
    float r = sqrt(x * x + y * y);
    m_r_cluster_id.insert(std::pair<float, unsigned int>(r, cluster_id));
  }

  for (auto iter = m_r_cluster_id.begin(); iter != m_r_cluster_id.end(); ++iter) {
    //for (SvtxTrack::ConstClusterIter iter = intrack->begin_clusters(); iter != intrack->end_clusters(); ++iter){
    unsigned int cluster_id = iter->second;
    //unsigned int cluster_id = *iter;
    SvtxCluster* cluster = _clustermap->get(cluster_id);
    if (!cluster) {
      LogError("No cluster Found!");
      continue;
    }

    TVector3 pos(cluster->get_x(), cluster->get_y(), cluster->get_z());

    seed_mom.SetPhi(pos.Phi());
    seed_mom.SetTheta(pos.Theta());

    //TODO use u, v explicitly?
    TVector3 n(cluster->get_x(), cluster->get_y(), 0);

    unsigned int begin_hit_id = *(cluster->begin_hits());
    SvtxHit* svtxhit = hitsmap->find(begin_hit_id)->second;

    PHG4Cell* cell_svtx = nullptr;
    PHG4Cell* cell_intt = nullptr;
    PHG4Cell* cell_maps = nullptr;

    if (_use_ladder_geom and cells_svtx) cell_svtx = cells_svtx->findCell(svtxhit->get_cellid());
    if (_use_ladder_geom and cells_intt) cell_intt = cells_intt->findCell(svtxhit->get_cellid());
    if (_use_ladder_geom and cells_maps) cell_maps = cells_maps->findCell(svtxhit->get_cellid());
    if (_use_ladder_geom and !(cell_svtx or cell_intt or cell_maps)) {
      if (verbosity >= 0)
        LogError("!(cell_svtx or cell_intt or cell_maps)");
      continue;
    }

    //float phi_tilt[7] = {0.304, 0.304, 0.304, 0.244, 0.244, 0.209, 0.201};
    unsigned int layer = cluster->get_layer();

    //NEW
    if (_use_ladder_geom) {
      if (cell_maps) {
        PHG4Cell* cell = cell_maps;

        int stave_index = cell->get_stave_index();
        int half_stave_index = cell->get_half_stave_index();
        int module_index = cell->get_module_index();
        int chip_index = cell->get_chip_index();

        double ladder_location[3] = { 0.0, 0.0, 0.0 };
        PHG4CylinderGeom_MAPS *geom = (PHG4CylinderGeom_MAPS*) geom_container_maps->GetLayerGeom(layer);
        // returns the center of the sensor in world coordinates - used to get the ladder phi location
        geom->find_sensor_center(stave_index, half_stave_index, module_index, chip_index, ladder_location);
        n.SetXYZ(ladder_location[0], ladder_location[1], 0);
        n.RotateZ(geom->get_stave_phi_tilt());
      } else if (cell_intt) {
        PHG4Cell* cell = cell_intt;
        PHG4CylinderGeom_Siladders* geom = (PHG4CylinderGeom_Siladders*) geom_container_intt->GetLayerGeom(layer);
        double hit_location[3] = { 0.0, 0.0, 0.0 };
        geom->find_segment_center(cell->get_ladder_z_index(), cell->get_ladder_phi_index(), hit_location);

        n.SetXYZ(hit_location[0], hit_location[1], 0);
        n.RotateZ(geom->get_strip_phi_tilt());
      }
    }

    /*
    //OLD
    if ((cells_maps and geom_container_maps) and layer < 3) {
      unsigned int begin_hit_id = *(cluster->begin_hits());
      SvtxHit* hit = hitsmap->find(begin_hit_id)->second;
      PHG4Cell* cell = cells_maps->findCell(hit->get_cellid());
      int stave_index = cell->get_stave_index();
      int half_stave_index = cell->get_half_stave_index();
      int module_index = cell->get_module_index();
      int chip_index = cell->get_chip_index();

      double ladder_location[3] = {0.0, 0.0, 0.0};
      PHG4CylinderGeom_MAPS *geom = (PHG4CylinderGeom_MAPS*) geom_container_maps->GetLayerGeom(layer);
      geom->find_sensor_center(stave_index, half_stave_index, module_index, chip_index, ladder_location);
      n.SetXYZ(ladder_location[0], ladder_location[1], 0);
      n.RotateZ(phi_tilt[layer]);
    } else if ((cells_intt and geom_container_intt) and pos.Perp() < 30.) {
      unsigned int begin_hit_id = *(cluster->begin_hits());
      SvtxHit* hit = hitsmap->find(begin_hit_id)->second;
      PHG4Cell* cell = cells_intt->findCell(hit->get_cellid());
      PHG4CylinderGeom_Siladders* geom = (PHG4CylinderGeom_Siladders*) geom_container_intt->GetLayerGeom(layer);
      double hit_location[3] = { 0.0, 0.0, 0.0 };
      geom->find_segment_center(cell->get_ladder_z_index(),cell->get_ladder_phi_index(), hit_location);

      n.SetXYZ(hit_location[0], hit_location[1], 0);
      n.RotateZ(phi_tilt[layer]);
    }
    */

    PHGenFit::Measurement* meas = new PHGenFit::PlanarMeasurement(pos, n,
        cluster->get_rphi_error(), cluster->get_z_error());

    measurements.push_back(meas);
  }


  //TODO Add multiple TrackRep choices.
  genfit::AbsTrackRep* rep = new genfit::RKTrackRep(_primary_pid_guess);
  PHGenFit::Track* track(new PHGenFit::Track(rep, seed_pos, seed_mom, seed_cov));
  track->addMeasurements(measurements);

  if (_fitter->processTrack(track, false) != 0) {
    if (verbosity >= 1)
      LogWarning("Track fitting failed");
    return NULL;
  }

  TVector3 vertex_position(0, 0, 0);
  if (vertex) {
    vertex_position.SetXYZ(vertex->get_x(), vertex->get_y(), vertex->get_z());
  }

  std::shared_ptr<genfit::MeasuredStateOnPlane> gf_state_vertex_ca = NULL;
  try {
    gf_state_vertex_ca = std::shared_ptr < genfit::MeasuredStateOnPlane> (track->extrapolateToPoint(vertex_position));
  } catch (...) {
    if (verbosity >= 2)
      LogWarning("extrapolateToPoint failed!");
    return NULL;
  }

  TVector3 mom = gf_state_vertex_ca->getMom();
  TMatrixDSym cov = gf_state_vertex_ca->get6DCov();

  //cout << "OUT Ex: " << sqrt(cov[0][0]) << ", Ey: " << sqrt(cov[1][1]) << ", Ez: " << sqrt(cov[2][2]) << std::endl;

  //cout << "IN Px: " << intrack->get_px() << ", Py: " << intrack->get_py() << ", Pz: " << intrack->get_pz() << std::endl;
  //cout << "OUT Px: " << mom.X() << ", Py: " << mom.Y() << ", Pz: " << mom.Z() << std::endl;

  return track;
}

/*
 * Fill SvtxVertexMap from GFRaveVertexes and Tracks
 */
void BDiJetModule::FillVertexMap(
  const std::vector<genfit::GFRaveVertex*>& rave_vertices,
  const std::vector<genfit::Track*>& gf_tracks) {

  for (unsigned int ivtx = 0; ivtx < rave_vertices.size(); ++ivtx)
  {
    genfit::GFRaveVertex* rave_vtx = rave_vertices[ivtx];

    //cout << "V0 x: " << rave_vtx->getPos().X() << ", y: " << rave_vtx->getPos().Y() << ", z: " << rave_vtx->getPos().Z() << std::endl;
    rv_prim_vtx[0] = rave_vtx->getPos().X();
    rv_prim_vtx[1] = rave_vtx->getPos().Y();
    rv_prim_vtx[2] = rave_vtx->getPos().Z();

    rv_prim_vtx_err[0] = sqrt(rave_vtx->getCov()[0][0]);
    rv_prim_vtx_err[1] = sqrt(rave_vtx->getCov()[1][1]);
    rv_prim_vtx_err[2] = sqrt(rave_vtx->getCov()[2][2]);

    rv_prim_vtx_ntrk = rave_vtx->getNTracks();
  } // ivtx

  for (SvtxVertexMap::Iter iter = _vertexmap->begin();
       iter != _vertexmap->end();
       ++iter)
  {
    SvtxVertex *svtx_vertex = iter->second;

    //cout << "V1 x: " << svtx_vertex->get_x() << ", y: " << svtx_vertex->get_y() << ", z: " << svtx_vertex->get_z() << std::endl;
    gf_prim_vtx[0] = svtx_vertex->get_x();
    gf_prim_vtx[1] = svtx_vertex->get_y();
    gf_prim_vtx[2] = svtx_vertex->get_z();

    gf_prim_vtx_err[0] = sqrt(svtx_vertex->get_error(0, 0));
    gf_prim_vtx_err[1] = sqrt(svtx_vertex->get_error(1, 1));
    gf_prim_vtx_err[2] = sqrt(svtx_vertex->get_error(2, 2));

    gf_prim_vtx_ntrk = int(svtx_vertex->size_tracks());
  } // iter on SvtxVertexMap

  return;
}

int BDiJetModule::GetSVMass_mom(
  const genfit::GFRaveVertex* rave_vtx,
  float & vtx_mass,
  float & vtx_px,
  float & vtx_py,
  float & vtx_pz
) {

  float sum_E = 0, sum_px = 0, sum_py = 0, sum_pz = 0;

  int N_good = 0;

  for (unsigned int itrk = 0; itrk < rave_vtx->getNTracks(); itrk++) {
    TVector3 mom_trk = rave_vtx->getParameters(itrk)->getMom();

    double w_trk = rave_vtx->getParameters(itrk)->getWeight();

    sum_px += mom_trk.X();
    sum_py += mom_trk.Y();
    sum_pz += mom_trk.Z();
    sum_E += sqrt(mom_trk.Mag2() + 0.140 * 0.140);

    //cout << "W: " << w_trk << std::endl;
    if ( w_trk > 0.6 ) {
      N_good++;
    }

  }//itrk

  vtx_mass =  sqrt(sum_E * sum_E - sum_px * sum_px - sum_py * sum_py - sum_pz * sum_pz);
  vtx_px = sum_px;
  vtx_py = sum_py;
  vtx_pz = sum_pz;

  //cout << "Mass: " << vtx_mass << ", pT: " << vtx_pT << std::endl;
  return N_good;
}



