#include "TrackAndClusterMatchingQA.h"

/// Cluster/Calorimeter includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calotrigger/CaloTriggerInfo.h>

#include <phool/phool.h>

/// Tracking includes
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/SvtxVertexMap.h>
#include <globalvertex/SvtxVertex.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrackState.h>
#include <trackbase_historic/TrackAnalysisUtils.h>
#include <trackbase_historic/TrackSeed.h>


#include <Acts/Definitions/Algebra.hpp>

#include <CLHEP/Vector/ThreeVector.h>

/// Fun4All includes
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>


#include <g4main/PHG4Particle.h>            // for PHG4Particle
#include <g4main/PHG4TruthInfoContainer.h>  // for PHG4TruthInfoContainer
#include <g4main/PHG4VtxPoint.h>            // for PHG4VtxPoint
#include <trackbase_historic/SvtxPHG4ParticleMap_v1.h>
#include <kfparticle_sphenix/KFParticle_truthAndDetTools.h>

/// ROOT includes
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TNtuple.h>
#include <TTree.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>

/// C++ includes
#include <cassert>
#include <sstream>
#include <string>

using namespace std;

/**
 * TrackAndClusterMatchingQA is a class developed for jet QA
 * Author: Antonio Silva (antonio.sphenix@gmail.com)
 */

/**
 * Constructor of module
 */
TrackAndClusterMatchingQA::TrackAndClusterMatchingQA(const std::string &name, const std::string &filename)
  : SubsysReco(name)
  , _outfilename(filename)
{
  /// Initialize variables and trees so we don't accidentally access
  /// memory that was never allocated
}

/**
 * Destructor of module
 */
TrackAndClusterMatchingQA::~TrackAndClusterMatchingQA()
{

}

/**
 * Initialize the module and prepare looping over events
 */
int TrackAndClusterMatchingQA::Init(PHCompositeNode *topNode)
{
  if (Verbosity() > 5)
  {
    cout << "Beginning Init in TrackAndClusterMatchingQA" << endl;
  }

  _outfile = new TFile(_outfilename.c_str(), "RECREATE");

  _h2trackPt_vs_clusterEt = new TH2F("h2trackPt_vs_clusterEt", ";track #it{p}_{T} (GeV/#it{c});EMCal cluster #it{E}_{T} (GeV)", 40, 0., 20., 40, 0., 20.);
  _h1EMCal_TowerEnergy = new TH1F("_h1EMCal_TowerEnergy", ";EMCal tower #it{E} (GeV);Entries", 40, 0., 20.);
  _h1EMCal_TowerEnergy_Retowered = new TH1F("_h1EMCal_TowerEnergy_Retowered", ";EMCal retorewed tower #it{E} (GeV);Entries", 40, 0., 20.);
  _h1HCalIN_TowerEnergy = new TH1F("_h1HCalIN_TowerEnergy", ";HCalIN tower #it{E} (GeV);Entries", 40, 0., 20.);
  _h1HCalOUT_TowerEnergy = new TH1F("_h1HCalOUT_TowerEnergy", ";HCalOUT tower #it{E} (GeV);Entries", 40, 0., 20.);
  _h1EMCal_TowerEnergy_afterSelection = new TH1F("_h1EMCal_TowerEnergy_afterSelection", ";EMCal tower #it{E} (GeV);Entries", 40, 0., 20.);
  _h1EMCal_TowerEnergy_Retowered_afterSelection = new TH1F("_h1EMCal_TowerEnergy_Retowered_afterSelection", ";EMCal retorewed tower #it{E} (GeV);Entries", 40, 0., 20.);
  _h1HCalIN_TowerEnergy_afterSelection = new TH1F("_h1HCalIN_TowerEnergy_afterSelection", ";HCalIN tower #it{E} (GeV);Entries", 40, 0., 20.);
  _h1HCalOUT_TowerEnergy_afterSelection = new TH1F("_h1HCalOUT_TowerEnergy_afterSelection", ";HCalOUT tower #it{E} (GeV);Entries", 40, 0., 20.);
  _h2TowerEnergy_EMCal_vs_HCalIN = new TH2F("_h2TowerEnergy_EMCal_vs_HCalIN", ";EMCal Retowered Towers #it{E} (GeV);Inner HCal Towers #it{E} (GeV)", 40, 0., 20., 40, 0., 20.);
  _h2TowerEnergy_EMCal_vs_HCalOUT = new TH2F("_h2TowerEnergy_EMCal_vs_HCalOUT", ";EMCal Retowered Towers #it{E} (GeV);Outer HCal Towers #it{E} (GeV)", 40, 0., 20., 40, 0., 20.);
  _h2TowerEnergy_HCalIN_vs_HCalOUT = new TH2F("_h2TowerEnergy_HCalIN_vs_HCalOUT", ";EMCal Retowered Towers #it{E} (GeV);Outer HCal Towers #it{E} (GeV)", 40, 0., 20., 40, 0., 20.);
  _h1EMCal_RetowerEnergyFraction = new TH1F("_h1EMCal_RetowerEnergyFraction", ";#it{E}^{max}_{tower}/#it{E}_{retowered};Entries", 20, 0., 1.);

  _h1Track_Quality = new TH1F("_h1Track_Quality", ";Quality;Entries", 100, 0., 50);
  _h1Track_DCAxy = new TH1F("_h1Track_DCAxy", ";DCA xy;Entries", 200, -0.02, 0.02);
  _h1Track_DCAz = new TH1F("_h1Track_DCAz", ";DCA z;Entries", 200, -0.02, 0.02);
  _h1Track_TPC_Hits = new TH1F("_h1Track_TPC_Hits", ";Number of TPC hits;Entries", 50, -0.5, 49.5);
  _h1Track_Silicon_Hits = new TH1F("_h1Track_Silicon_Hits", ";Number of TPC hits;Entries", 50, -0.5, 49.5);
  _h1Track_Pt_beforeSelections = new TH1F("_h1Track_Pt_beforeSelections", ";track #it{p}_{T};Entries", 100, 0., 50);
  _h1Track_Pt_afterSelections = new TH1F("_h1Track_Pt_afterSelections", ";track #it{p}_{T};Entries", 100, 0., 50);

  _h1Track_TPC_Hits_Selected = new TH1F("_h1Track_TPC_Hits_Selected", ";Number of TPC hits;Entries", 50, -0.5, 49.5);
  _h2Track_TPC_Hits_vs_Phi = new TH2F("_h2Track_TPC_Hits_vs_Phi", ";Number of TPC hits;track #phi", 50, -0.5, 49.5, 63, -M_PI, M_PI);
  _h2Track_TPC_Hits_vs_Eta = new TH2F("_h2Track_TPC_Hits_vs_Eta", ";Number of TPC hits;track #eta", 50, -0.5, 49.5, 22, -1.1, 1.1);
  _h2Track_TPC_Hits_vs_Pt = new TH2F("_h2Track_TPC_Hits_vs_Pt", ";Number of TPC hits;track #it{p}_{T} (GeV/#it{c})", 50, -0.5, 49.5, 40, 0., 20);

  _h1deta = new TH1F("hdeta", "Cluster #deta; #deta; Entries", 20, -0.2, 0.2);    //deta distribution
  _h1dphi  = new TH1F("hdphi", "Cluster #dphi; #dphi; Entries", 50, -0.15, 0.15);   //dphi distribution
  _h1min_dR  = new TH1F("hdR", "Cluster #dR; #dR; Entries", 100,0.,5.);   //jet delta radius
  _h2phi_vs_deta = new TH2F("_h2phi_vs_deta", ";#dphi; #deta",  50, -0.15, 0.15, 20, -0.2, 0.2); // deta Vs. dphi distribution

  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int TrackAndClusterMatchingQA::process_event(PHCompositeNode *topNode)
{
  if (Verbosity() > 5)
  {
    cout << "Beginning process_event in TrackAndClusterMatchingQA" << endl;
  }

  GlobalVertex *vtx = nullptr;
  CLHEP::Hep3Vector vertex(0., 0., 0.);

  bool has_vertex = false;

  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (vertexmap)
  {
    if (vertexmap->empty())
    {
      if(Verbosity() > 5) std::cout << "GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
    }
    else
    {
      vtx = vertexmap->begin()->second;
      vertex.setX(vtx->get_x());
      vertex.setY(vtx->get_y());
      vertex.setZ(vtx->get_z());
      has_vertex = true;
    }
  }
  else
  {
    if(Verbosity() > 5) std::cout << "GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
  }

  SvtxVertex *svtx_vtx = nullptr;

  SvtxVertexMap *vertexMap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");

  if(vertexMap)
  {
    if(!vertexMap->empty())
    {
      svtx_vtx = vertexMap->begin()->second;
      vertex.setX(svtx_vtx->get_x());
      vertex.setY(svtx_vtx->get_y());
      vertex.setZ(svtx_vtx->get_z());
      has_vertex = true;
    }
  }

  if(has_vertex == false && (!_use_origin_when_no_vertex))
  {
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  _track_map = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");

  if (!_track_map)
  {
    cout << PHWHERE
         << "SvtxTrackMap node is missing, can't collect tracks"
         << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  _em_clusters = findNode::getClass<RawClusterContainer>(topNode, _em_clusters_name);

  // read in tower geometries
  RawTowerGeomContainer *geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  RawTowerGeomContainer *geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  if(!geomEM || !geomIH || !geomOH)
  {
    std::cout << "FATAL ERROR, cannot find tower geometry containers" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  //Radius of the front face of the EMCal
  float cemcradius = geomEM->get_radius();

  if (!_em_clusters)
  {
    std::cout << PHWHERE << _em_clusters_name << " node is missing, can't collect clusters" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  TowerInfoContainer *towerinfosEM_retower = findNode::getClass<TowerInfoContainer>(topNode, _em_retowered_towers_name);
  TowerInfoContainer *towerinfosEM = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
  TowerInfoContainer *towerinfosIH = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  TowerInfoContainer *towerinfosOH = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");

  if (!towerinfosEM)
  {
    std::cout << " TrackAndClusterMatchingQA::process_event : container with EM towers does not exist, aborting " << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  if (!towerinfosIH)
  {
    std::cout << " TrackAndClusterMatchingQA::process_event : container with HCalIN towers does not exist, aborting " << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  if (!towerinfosOH)
  {
    std::cout << " TrackAndClusterMatchingQA::process_event : container with HCalOUT towers does not exist, aborting " << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  SvtxTrack *track = nullptr;

  for (SvtxTrackMap::Iter iter = _track_map->begin(); iter != _track_map->end();++iter)
  {
    track = iter->second;

    if(!track) continue;

    if(track->get_pt() < _track_min_pt)
    {
      continue;
    }

    _h1Track_Pt_beforeSelections->Fill(track->get_pt());

    if(!IsAcceptableTrack(track, vtx))
    {
      continue;
    }

    _h1Track_Pt_afterSelections->Fill(track->get_pt());

    //Get track projection to the front face of the EMCal
    SvtxTrackState* cemcstate = track->get_state(cemcradius);

    float track_phi = track->get_phi();
    float track_eta = track->get_eta();

    //Calculate eta and phi of the track projection to the EMCal
    if(cemcstate)
    {
      track_phi = atan2(cemcstate->get_y(), cemcstate->get_x());
      track_eta = asinh(cemcstate->get_z()/sqrt(cemcstate->get_x()*cemcstate->get_x() + cemcstate->get_y()*cemcstate->get_y()));
    }

    float min_dR = 999.;
    RawCluster *cluster_match = nullptr;

    RawClusterContainer::ConstRange begin_end_EMC = _em_clusters->getClusters();
    RawClusterContainer::ConstIterator clusIter_EMC;

    /// Loop over the EMCal clusters
    for (clusIter_EMC = begin_end_EMC.first; clusIter_EMC != begin_end_EMC.second; ++clusIter_EMC)
    {
      RawCluster *cluster = clusIter_EMC->second;

      float cluster_phi = RawClusterUtility::GetAzimuthAngle(*cluster, vertex);
      float cluster_eta = RawClusterUtility::GetPseudorapidity(*cluster, vertex);

      float dR = GetDeltaR(track_phi, cluster_phi, track_eta, cluster_eta);

      if(dR > 0.2)
      {
        continue;
      }

      if(dR < min_dR)
      {
        min_dR = dR;
        cluster_match = cluster;
      }
    }

    if(cluster_match)
    {
      _h2trackPt_vs_clusterEt->Fill(track->get_pt(), RawClusterUtility::GetET(*cluster_match, vertex));

      float cluster_phi = RawClusterUtility::GetAzimuthAngle(*cluster_match, vertex);
      float cluster_eta = RawClusterUtility::GetPseudorapidity(*cluster_match, vertex);
      float deta = track_eta - cluster_eta;   //added for eta analysis
      float dphi = track_phi - cluster_phi;   //added for phi analysis

      _h1deta->Fill(deta);
      _h1dphi->Fill(dphi);
      _h2phi_vs_deta->Fill(dphi,deta);

    }

      _h1min_dR->Fill(min_dR);

  }

  TowerInfo *EMCal_towerInfo = nullptr;
  TowerInfo *HCalIN_towerInfo = nullptr;
  TowerInfo *HCalOUT_towerInfo = nullptr;
  for(unsigned int i = 0; i < towerinfosEM_retower->size(); i++)
  {
    EMCal_towerInfo = towerinfosEM_retower->get_tower_at_channel(i);
    HCalIN_towerInfo = towerinfosIH->get_tower_at_channel(i);
    HCalOUT_towerInfo = towerinfosOH->get_tower_at_channel(i);

    _h1EMCal_TowerEnergy_Retowered->Fill(EMCal_towerInfo->get_energy());
    _h1HCalIN_TowerEnergy->Fill(HCalIN_towerInfo->get_energy());
    _h1HCalOUT_TowerEnergy->Fill(HCalOUT_towerInfo->get_energy());

    if(_apply_tower_selection)
    {
      if((!EMCal_towerInfo->get_isGood()) || (!HCalIN_towerInfo->get_isGood()) || (!HCalOUT_towerInfo->get_isGood()))
      {
        continue;
      }
    }

    _h1EMCal_TowerEnergy_Retowered_afterSelection->Fill(EMCal_towerInfo->get_energy());
    _h1HCalIN_TowerEnergy_afterSelection->Fill(HCalIN_towerInfo->get_energy());
    _h1HCalOUT_TowerEnergy_afterSelection->Fill(HCalOUT_towerInfo->get_energy());

    _h2TowerEnergy_EMCal_vs_HCalIN->Fill(EMCal_towerInfo->get_energy(), HCalIN_towerInfo->get_energy());
    _h2TowerEnergy_EMCal_vs_HCalOUT->Fill(EMCal_towerInfo->get_energy(), HCalOUT_towerInfo->get_energy());
    _h2TowerEnergy_HCalIN_vs_HCalOUT->Fill(HCalIN_towerInfo->get_energy(), HCalOUT_towerInfo->get_energy());
  }

  const int NETA = geomIH->get_etabins();
  const int NPHI = geomIH->get_phibins();

  float EMCAL_RETOWER_E[NETA][NPHI];
  float EMCAL_RETOWER_HIGHEST_E[NETA][NPHI];

  for (int eta = 0; eta < NETA; eta++)
  {
    for (int phi = 0; phi < NPHI; phi++)
    {
      EMCAL_RETOWER_E[eta][phi] = 0;
      EMCAL_RETOWER_HIGHEST_E[eta][phi] = 0;
    }
  }

  for(unsigned int i = 0; i < towerinfosEM->size(); i++)
  {
    EMCal_towerInfo = towerinfosEM->get_tower_at_channel(i);
    _h1EMCal_TowerEnergy->Fill(EMCal_towerInfo->get_energy());

    if(_apply_tower_selection && (!EMCal_towerInfo->get_isGood()))
    {
      continue;
    }

    _h1EMCal_TowerEnergy_afterSelection->Fill(EMCal_towerInfo->get_energy());

    //Reproducing what is done in the RetowerCEMC module
    unsigned int channelkey = towerinfosEM->encode_key(i);
	  int ieta = towerinfosEM->getTowerEtaBin(channelkey);
	  int iphi = towerinfosEM->getTowerPhiBin(channelkey);
	  const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::CEMC, ieta, iphi);
	  RawTowerGeom *tower_geom = geomEM->get_tower_geometry(key);
	  int this_IHetabin = geomIH->get_etabin(tower_geom->get_eta());

    double fractionalcontribution[3] = {0};

    std::pair<double, double> range_embin= geomEM->get_etabounds(tower_geom->get_bineta());
    for(int etabin_iter = -1;etabin_iter <= 1;etabin_iter++)
    {
      if(this_IHetabin+etabin_iter < 0 || this_IHetabin+etabin_iter >= NETA){continue;}
		  std::pair<double, double> range_ihbin= geomIH->get_etabounds(this_IHetabin + etabin_iter);
		  if(range_ihbin.first <= range_embin.first && range_ihbin.second >= range_embin.second)
      {
        fractionalcontribution[etabin_iter+1] = 1;
      }
		  else if(range_ihbin.first <= range_embin.first && range_ihbin.second < range_embin.second  && range_embin.first < range_ihbin.second)
      {
        fractionalcontribution[etabin_iter+1] = (range_ihbin.second - range_embin.first) / (range_embin.second- range_embin.first);
      }
		  else if(range_ihbin.first > range_embin.first && range_ihbin.second >= range_embin.second && range_embin.second > range_ihbin.first)
      {
        fractionalcontribution[etabin_iter+1] = (range_embin.second - range_ihbin.first) / (range_embin.second- range_embin.first);
      }
		  else
      {
        fractionalcontribution[etabin_iter+1] = 0;
      }
    }

    int this_IHphibin = geomIH->get_phibin(tower_geom->get_phi());
	  float this_E = EMCal_towerInfo->get_energy();
	  for (int etabin_iter = -1 ; etabin_iter <= 1;etabin_iter++)
    {
      if (this_IHetabin+etabin_iter < 0 || this_IHetabin+etabin_iter >= NETA)
      {
        continue;
      }
      EMCAL_RETOWER_E[this_IHetabin+etabin_iter][this_IHphibin] += this_E * fractionalcontribution[etabin_iter+1];
      if(this_E * fractionalcontribution[etabin_iter+1] > EMCAL_RETOWER_HIGHEST_E[this_IHetabin+etabin_iter][this_IHphibin])
      {
        EMCAL_RETOWER_HIGHEST_E[this_IHetabin+etabin_iter][this_IHphibin] = this_E * fractionalcontribution[etabin_iter+1];
      }
    }

  }

  //Calculate the ratio between the highest energy tower in the retowered tower
  for (int eta = 0; eta < NETA; eta++)
  {
    for (int phi = 0; phi < NPHI; phi++)
    {
      if(EMCAL_RETOWER_E[eta][phi] > 0)
      {
        _h1EMCal_RetowerEnergyFraction->Fill(EMCAL_RETOWER_HIGHEST_E[eta][phi]/EMCAL_RETOWER_E[eta][phi]);
      }
    }
  }


  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int TrackAndClusterMatchingQA::End(PHCompositeNode *topNode)
{
  if (Verbosity() > 1)
  {
    cout << "Ending TrackAndClusterMatchingQA analysis package" << endl;
  }

  /// Change to the outfile
  _outfile->cd();

  /// Write and close the outfile
  _outfile->Write();
  _outfile->Close();

  /// Clean up pointers and associated histos/trees in TFile
  delete _outfile;

  if (Verbosity() > 1)
  {
    cout << "Finished TrackAndClusterMatchingQA analysis package" << endl;
  }

  return 0;
}

float TrackAndClusterMatchingQA::GetDeltaR(float track_phi, float cluster_phi, float track_eta, float cluster_eta)
{
  float deta = track_eta - cluster_eta;
  float dphi = track_phi - cluster_phi;
  if(dphi > M_PI) dphi -= 2*M_PI;
  if(dphi < -M_PI) dphi += 2*M_PI;

  return sqrt(dphi*dphi + deta*deta);
}

bool TrackAndClusterMatchingQA::IsAcceptableTrack(SvtxTrack *track, GlobalVertex *vtx)
{
  Acts::Vector3 vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());

  auto dcapair = TrackAnalysisUtils::get_dca(track, vertex);

  float dcaxy = dcapair.first.first;
  //float dcaxy_err = dcapair.first.second;
  _h1Track_DCAxy->Fill(dcaxy);
  float dcaz = dcapair.second.first;
  //float dcaz_err = dcapair.second.second;
  _h1Track_DCAz->Fill(dcaz);

  float quality = track->get_quality();
  _h1Track_Quality->Fill(quality);


  auto silicon_seed = track->get_silicon_seed();
  auto tpc_seed = track->get_tpc_seed();
  int nsiliconhits = 0;
  int nTPChits = 0;

  // Getting number of TPC hits as done in SvtxEvaluator.h
  if(tpc_seed)
  {
    for (TrackSeed::ConstClusterKeyIter iter = tpc_seed->begin_cluster_keys(); iter != tpc_seed->end_cluster_keys(); ++iter)
    {
      TrkrDefs::cluskey cluster_key = *iter;
      unsigned int layer = TrkrDefs::getLayer(cluster_key);
      if (_nlayers_tpc > 0 && layer >= (_nlayers_maps + _nlayers_intt) && layer < (_nlayers_maps + _nlayers_intt + _nlayers_tpc))
      {
        nTPChits++;
      }
    }
  }

  if (silicon_seed)
  {
    for (TrackSeed::ConstClusterKeyIter iter = silicon_seed->begin_cluster_keys(); iter != silicon_seed->end_cluster_keys(); ++iter)
    {
      TrkrDefs::cluskey cluster_key = *iter;
      unsigned int layer = TrkrDefs::getLayer(cluster_key);
      if (_nlayers_tpc > 0 && layer >= (_nlayers_maps + _nlayers_intt) && layer < (_nlayers_maps + _nlayers_intt + _nlayers_tpc))
      {
        nTPChits++;
      }
    }
    nsiliconhits = silicon_seed->size_cluster_keys(); // Get number of hits on silicon detectors (INTT+MVTX)
  }

  _h1Track_TPC_Hits->Fill(nTPChits);
  _h2Track_TPC_Hits_vs_Phi->Fill(nTPChits, track->get_phi());
  _h2Track_TPC_Hits_vs_Eta->Fill(nTPChits, track->get_eta());
  _h2Track_TPC_Hits_vs_Pt->Fill(nTPChits, track->get_pt());
  _h1Track_Silicon_Hits->Fill(nsiliconhits);

  if(quality > _track_quality) return false;

  // DCA units and values have to be cross-checked
  //std::cout << "DCAxy: " << dcaxy << std::endl;
  if(std::fabs(dcaxy) > _track_max_dcaxy) return false; // will accept everything, need to check units (microns?)
  //std::cout << "DCAz: " << dcaz << std::endl;
  if(std::fabs(dcaz) > _track_max_dcaz) return false; // will accept everything, need to check units (microns?)

  if (nsiliconhits < _track_min_silicon_hits) return false;

  _h1Track_TPC_Hits_Selected->Fill(nTPChits);

  if(nTPChits < _track_min_tpc_hits) return false;


  return true;
}
