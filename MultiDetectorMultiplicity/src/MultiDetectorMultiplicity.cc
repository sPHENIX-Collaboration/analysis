#include "MultiDetectorMultiplicity.h"

// Centrality
#include <centrality/CentralityInfov1.h>

// Calo includes
#include <calobase/TowerInfov1.h>
#include <calobase/TowerInfoContainerv1.h>

#include <phool/getClass.h>
#include <phool/phool.h>

// Vertex includes
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

// Tracking includes
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/TrackSeedContainer.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>

/// Fun4All includes
#include <fun4all/Fun4AllReturnCodes.h>

/// ROOT includes
#include <TFile.h>
#include <TH2F.h>
#include <TH1I.h>
#include <TTree.h>

/// C++ includes
#include <string>

/**
 * MultiDetectorMultiplicity is a class developed to cross-check multiplicity and energy correlation in different subsystems
 * Author: Antonio Silva (antonio.sphenix@gmail.com)
 */

/**
 * Constructor of module
 */
MultiDetectorMultiplicity::MultiDetectorMultiplicity(const std::string &name, const std::string &filename)
  : SubsysReco(name)
  , _outfilename(filename)
{
  /// Initialize variables and trees so we don't accidentally access
  /// memory that was never allocated
}

/**
 * Destructor of module
 */

MultiDetectorMultiplicity::~MultiDetectorMultiplicity()
{

}

/**
 * Initialize the module and prepare looping over events
 */
int MultiDetectorMultiplicity::Init(PHCompositeNode *topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in MultiDetectorMultiplicity" << std::endl;
  }

  initializeObjects();

  return 0;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int MultiDetectorMultiplicity::process_event(PHCompositeNode *topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning process_event in MultiDetectorMultiplicity" << std::endl;
  }

  float Ntracks = getFilteredNtracks(topNode);
  float MVTXclusters = getMVTXnclusters(topNode);
  float EMCalTotalEnergy = getEMCalTotalEnergy(topNode);
  float IHCalTotalEnergy = getIHCalTotalEnergy(topNode);
  float OHCalTotalEnergy = getOHCalTotalEnergy(topNode);
  float EPDTotalEnergy = getsEPDTotalEnergy(topNode);

  bool acceptVertex = isVertexAccepted(topNode);

  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!cent_node)
  {
    std::cout << "MultiDetectorMultiplicity::process_event - Error can not find centrality node " << std::endl;
    _SubsystemObjectsFail->Fill(OBJECT::CENTRALITY);
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  if(!acceptVertex) return Fun4AllReturnCodes::ABORTEVENT;

  _mbd_centrality = cent_node->get_centile(CentralityInfo::PROP::mbd_NS);
  _epd_centrality = cent_node->get_centile(CentralityInfo::PROP::epd_NS);

  _tracks_EMCALenergy->Fill(Ntracks, EMCalTotalEnergy);
  _tracks_IHCALenergy->Fill(Ntracks, IHCalTotalEnergy);
  _tracks_OHCALenergy->Fill(Ntracks, OHCalTotalEnergy);
  _tracks_AllCALenergy->Fill(Ntracks, EMCalTotalEnergy+IHCalTotalEnergy+OHCalTotalEnergy);
  _tracks_MVTXclusters->Fill(Ntracks, MVTXclusters);

  _MVTXclusters_EMCALenergy->Fill(MVTXclusters, EMCalTotalEnergy);
  _MVTXclusters_IHCALenergy->Fill(MVTXclusters, IHCalTotalEnergy);
  _MVTXclusters_OHCALenergy->Fill(MVTXclusters, OHCalTotalEnergy);
  _MVTXclusters_AllCALenergy->Fill(MVTXclusters, EMCalTotalEnergy+IHCalTotalEnergy+OHCalTotalEnergy);

  _EMCALenergy_IHCALenergy->Fill(EMCalTotalEnergy, IHCalTotalEnergy);
  _EMCALenergy_OHCALenergy->Fill(EMCalTotalEnergy, OHCalTotalEnergy);
  _IHCALenergy_OHCALenergy->Fill(IHCalTotalEnergy, OHCalTotalEnergy);

  float centBinMBD = 100.-_mbd_centrality;

  _MBDcentrality_tracks->Fill(centBinMBD, Ntracks);
  _MBDcentrality_MVTXclusters->Fill(centBinMBD, MVTXclusters);
  _MBDcentrality_EMCALenergy->Fill(centBinMBD, EMCalTotalEnergy);
  _MBDcentrality_IHCALenergy->Fill(centBinMBD, IHCalTotalEnergy);
  _MBDcentrality_OHCALenergy->Fill(centBinMBD, OHCalTotalEnergy);
  _MBDcentrality_AllCALenergy->Fill(centBinMBD, EMCalTotalEnergy+IHCalTotalEnergy+OHCalTotalEnergy);
  _MBDcentrality_EPDenergy->Fill(centBinMBD, EPDTotalEnergy);

  float centBinEPD = 100.-_epd_centrality;

  _EPDcentrality_tracks->Fill(centBinEPD, Ntracks);
  _EPDcentrality_MVTXclusters->Fill(centBinEPD, MVTXclusters);
  _EPDcentrality_EMCALenergy->Fill(centBinEPD, EMCalTotalEnergy);
  _EPDcentrality_IHCALenergy->Fill(centBinEPD, IHCalTotalEnergy);
  _EPDcentrality_OHCALenergy->Fill(centBinEPD, OHCalTotalEnergy);
  _EPDcentrality_AllCALenergy->Fill(centBinEPD, EMCalTotalEnergy+IHCalTotalEnergy+OHCalTotalEnergy);
  _EPDcentrality_EPDenergy->Fill(centBinEPD, EPDTotalEnergy);

  _EPDcentrality_MBDcentrality->Fill(centBinEPD, centBinMBD);

  _EPDenergy_tracks->Fill(EPDTotalEnergy, Ntracks);
  _EPDenergy_MVTXclusters->Fill(EPDTotalEnergy, MVTXclusters);
  _EPDenergy_EMCALenergy->Fill(EPDTotalEnergy, EMCalTotalEnergy);
  _EPDenergy_IHCALenergy->Fill(EPDTotalEnergy, IHCalTotalEnergy);
  _EPDenergy_OHCALenergy->Fill(EPDTotalEnergy, OHCalTotalEnergy);
  _EPDenergy_AllCALenergy->Fill(EPDTotalEnergy, EMCalTotalEnergy+IHCalTotalEnergy+OHCalTotalEnergy);

  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int MultiDetectorMultiplicity::End(PHCompositeNode *topNode)
{
  if (Verbosity() > 1)
  {
    std::cout << "Ending MultiDetectorMultiplicity analysis package" << std::endl;
  }

  /// Change to the outfile
  _outfile->cd();

  /// Write and close the outfile
  _tracks_EMCALenergy->Write();
  _tracks_IHCALenergy->Write();
  _tracks_OHCALenergy->Write();
  _tracks_AllCALenergy->Write();
  _tracks_MVTXclusters->Write();

  _MVTXclusters_EMCALenergy->Write();
  _MVTXclusters_IHCALenergy->Write();
  _MVTXclusters_OHCALenergy->Write();
  _MVTXclusters_AllCALenergy->Write();

  _EMCALenergy_IHCALenergy->Write();
  _EMCALenergy_OHCALenergy->Write();
  _IHCALenergy_OHCALenergy->Write();

  _MBDcentrality_tracks->Write();
  _MBDcentrality_MVTXclusters->Write();
  _MBDcentrality_EMCALenergy->Write();
  _MBDcentrality_IHCALenergy->Write();
  _MBDcentrality_OHCALenergy->Write();
  _MBDcentrality_AllCALenergy->Write();
  _MBDcentrality_EPDenergy->Write();

  _EPDcentrality_tracks->Write();
  _EPDcentrality_MVTXclusters->Write();
  _EPDcentrality_EMCALenergy->Write();
  _EPDcentrality_IHCALenergy->Write();
  _EPDcentrality_OHCALenergy->Write();
  _EPDcentrality_AllCALenergy->Write();
  _EPDcentrality_EPDenergy->Write();

  _EPDcentrality_MBDcentrality->Write();

  _EPDenergy_tracks->Write();
  _EPDenergy_MVTXclusters->Write();
  _EPDenergy_EMCALenergy->Write();
  _EPDenergy_IHCALenergy->Write();
  _EPDenergy_OHCALenergy->Write();
  _EPDenergy_AllCALenergy->Write();

  _SubsystemObjectsFail->Write();

  _outfile->Close();

  /// Clean up pointers and associated histos/trees in TFile
  delete _outfile;

  if (Verbosity() > 1)
  {
    std::cout << "Finished MultiDetectorMultiplicity analysis package" << std::endl;
  }

  return 0;
}

void MultiDetectorMultiplicity::initializeObjects()
{
  _outfile = new TFile(_outfilename.c_str(), "RECREATE");

  _tracks_EMCALenergy = new TH2F("tracks_EMCALenergy", ";Track multiplicity;EMCal Energy (GeV)",_nbins,0.,_track_bin_max,_nbins,0.,_emcal_bin_max);
  _tracks_IHCALenergy = new TH2F("tracks_IHCALenergy", ";Track multiplicity;IHCal Energy (GeV)",_nbins,0.,_track_bin_max,_nbins,0.,_ihcal_bin_max);
  _tracks_OHCALenergy = new TH2F("tracks_OHCALenergy", ";Track multiplicity;OHCal Energy (GeV)",_nbins,0.,_track_bin_max,_nbins,0.,_ohcal_bin_max);
  _tracks_AllCALenergy = new TH2F("tracks_AllCALenergy", ";Track multiplicity;EMCal+IHCal+OHCal Energy (GeV)",_nbins,0.,_track_bin_max,_nbins,0.,_allcal_bin_max);
  _tracks_MVTXclusters = new TH2F("tracks_MVTXclusters", ";Track multiplicity;MVTX cluster multiplicity",_nbins,0.,_track_bin_max,_nbins,0.,_mvtx_bin_max);

  _MVTXclusters_EMCALenergy = new TH2F("MVTXclusters_EMCALenergy", ";MVTX cluster multiplicity;EMCal Energy (GeV)",_nbins,0.,_mvtx_bin_max,_nbins,0.,_emcal_bin_max);
  _MVTXclusters_IHCALenergy = new TH2F("MVTXclusters_IHCALenergy", ";MVTX cluster multiplicity;IHCal Energy (GeV)",_nbins,0.,_mvtx_bin_max,_nbins,0.,_ihcal_bin_max);
  _MVTXclusters_OHCALenergy = new TH2F("MVTXclusters_OHCALenergy", ";MVTX cluster multiplicity;OHCal Energy (GeV)",_nbins,0.,_mvtx_bin_max,_nbins,0.,_ohcal_bin_max);
  _MVTXclusters_AllCALenergy = new TH2F("MVTXclusters_AllCALenergy", ";MVTX cluster multiplicity;EMCal+IHCal+OHCal Energy (GeV)",_nbins,0.,_mvtx_bin_max,_nbins,0.,_allcal_bin_max);

  _EMCALenergy_IHCALenergy = new TH2F("EMCALenergy_IHCALenergy", ";EMCal Energy (GeV);IHCal Energy (GeV)",_nbins,0.,_emcal_bin_max,_nbins,0.,_ihcal_bin_max);
  _EMCALenergy_OHCALenergy = new TH2F("EMCALenergy_OHCALenergy", ";EMCal Energy (GeV);OHCal Energy (GeV)",_nbins,0.,_emcal_bin_max,_nbins,0.,_ohcal_bin_max);
  _IHCALenergy_OHCALenergy = new TH2F("IHCALenergy_OHCALenergy", ";IHCal Energy (GeV);OHCal Energy (GeV)",_nbins,0.,_ihcal_bin_max,_nbins,0.,_ohcal_bin_max);

  _MBDcentrality_tracks = new TH2F("MBDcentrality_tracks", ";MBD Centrality;Track multiplicity",10,0.,100.,_nbins,0.,_track_bin_max);
  setCentralityHistoLabel(_MBDcentrality_tracks);

  _MBDcentrality_MVTXclusters = new TH2F("MBDcentrality_MVTXclusters", ";MBD Centrality;MVTX cluster multiplicity",10,0.,100.,_nbins,0.,_mvtx_bin_max);
  setCentralityHistoLabel(_MBDcentrality_MVTXclusters);

  _MBDcentrality_EMCALenergy = new TH2F("MBDcentrality_EMCALenergy", ";MBD Centrality;EMCal Energy (GeV)",10,0.,100.,_nbins,0.,_emcal_bin_max);
  setCentralityHistoLabel(_MBDcentrality_EMCALenergy);

  _MBDcentrality_IHCALenergy = new TH2F("MBDcentrality_IHCALenergy", ";MBD Centrality;IHCal Energy (GeV)",10,0.,100.,_nbins,0.,_ihcal_bin_max);
  setCentralityHistoLabel(_MBDcentrality_IHCALenergy);

  _MBDcentrality_OHCALenergy = new TH2F("MBDcentrality_OHCALenergy", ";MBD Centrality;OHCal Energy (GeV)",10,0.,100.,_nbins,0.,_ohcal_bin_max);
  setCentralityHistoLabel(_MBDcentrality_OHCALenergy);

  _MBDcentrality_AllCALenergy = new TH2F("MBDcentrality_AllCALenergy", ";MBD Centrality;EMCal+IHCal+OHCal Energy (GeV)",10,0.,100.,_nbins,0.,_allcal_bin_max);
  setCentralityHistoLabel(_MBDcentrality_AllCALenergy);

  _MBDcentrality_EPDenergy = new TH2F("MBDcentrality_EPDenergy", ";MBD Centrality;EPD Energy (GeV)",10,0.,100.,_nbins,0.,_epd_bin_max);
  setCentralityHistoLabel(_MBDcentrality_EPDenergy);

  _EPDcentrality_tracks = new TH2F("EPDcentrality_tracks", ";EPD Centrality;Track multiplicity",10,0.,100.,_nbins,0.,_track_bin_max);
  setCentralityHistoLabel(_EPDcentrality_tracks);

  _EPDcentrality_MVTXclusters = new TH2F("EPDcentrality_MVTXclusters", ";EPD Centrality;MVTX cluster multiplicity",10,0.,100.,_nbins,0.,_mvtx_bin_max);
  setCentralityHistoLabel(_EPDcentrality_MVTXclusters);

  _EPDcentrality_EMCALenergy = new TH2F("EPDcentrality_EMCALenergy", ";EPD Centrality;EMCal Energy (GeV)",10,0.,100.,_nbins,0.,_emcal_bin_max);
  setCentralityHistoLabel(_EPDcentrality_EMCALenergy);

  _EPDcentrality_IHCALenergy = new TH2F("EPDcentrality_IHCALenergy", ";EPD Centrality;IHCal Energy (GeV)",10,0.,100.,_nbins,0.,_ihcal_bin_max);
  setCentralityHistoLabel(_EPDcentrality_IHCALenergy);

  _EPDcentrality_OHCALenergy = new TH2F("EPDcentrality_OHCALenergy", ";EPD Centrality;OHCal Energy (GeV)",10,0.,100.,_nbins,0.,_ohcal_bin_max);
  setCentralityHistoLabel(_EPDcentrality_OHCALenergy);

  _EPDcentrality_AllCALenergy = new TH2F("EPDcentrality_AllCALenergy", ";EPD Centrality;EMCal+IHCal+OHCal Energy (GeV)",10,0.,100.,_nbins,0.,_allcal_bin_max);
  setCentralityHistoLabel(_EPDcentrality_AllCALenergy);

  _EPDcentrality_EPDenergy = new TH2F("EPDcentrality_EPDenergy", ";EPD Centrality;EPD Energy (GeV)",10,0.,100.,_nbins,0.,_epd_bin_max);
  setCentralityHistoLabel(_EPDcentrality_EPDenergy);

  _EPDcentrality_MBDcentrality = new TH2F("EPDcentrality_MBDcentrality", ";EPD Centrality;MBD Centrality",10,0.,100.,10,0.,100);
  setCentralityHistoLabel(_EPDcentrality_MBDcentrality, true);

  _EPDenergy_tracks = new TH2F("EPDenergy_tracks", ";EPD Energy (GeV);Track multiplicity",_nbins,0.,_epd_bin_max,_nbins,0.,_track_bin_max);
  _EPDenergy_MVTXclusters = new TH2F("EPDenergy_MVTXclusters", ";EPD Energy (GeV);MVTX cluster multiplicity",_nbins,0.,_epd_bin_max,_nbins,0.,_mvtx_bin_max);
  _EPDenergy_EMCALenergy = new TH2F("EPDenergy_EMCALenergy", ";EPD Energy (GeV);EMCal Energy (GeV)",_nbins,0.,_epd_bin_max,_nbins,0.,_emcal_bin_max);
  _EPDenergy_IHCALenergy = new TH2F("EPDenergy_IHCALenergy", ";EPD Energy (GeV);IHCal Energy (GeV)",_nbins,0.,_epd_bin_max,_nbins,0.,_ihcal_bin_max);
  _EPDenergy_OHCALenergy = new TH2F("EPDenergy_OHCALenergy", ";EPD Energy (GeV);OHCal Energy (GeV)",_nbins,0.,_epd_bin_max,_nbins,0.,_ohcal_bin_max);
  _EPDenergy_AllCALenergy = new TH2F("EPDenergy_AllCALenergy", ";EPD Energy (GeV);EMCal+IHCal+OHCal Energy (GeV)",_nbins,0.,_epd_bin_max,_nbins,0.,_allcal_bin_max);

  // This histo keeps track of the number of times certain objects were not found.
  // Ideally this histogram should be EMPTY!!!
  // For future updates including other objects, be sure to not reject the event before all objects are checked
  _SubsystemObjectsFail = new TH1I("SubsystemObjectsFail", "Times objects were NOT found;;Entries",8,0.,8.);

  _SubsystemObjectsFail->GetXaxis()->SetBinLabel(1,"Vertex");
  _SubsystemObjectsFail->GetXaxis()->SetBinLabel(2,"Centrality");
  _SubsystemObjectsFail->GetXaxis()->SetBinLabel(3,"Tracks");
  _SubsystemObjectsFail->GetXaxis()->SetBinLabel(4,"MVTX clusters");
  _SubsystemObjectsFail->GetXaxis()->SetBinLabel(5,"EMCal towers");
  _SubsystemObjectsFail->GetXaxis()->SetBinLabel(6,"IHCal towers");
  _SubsystemObjectsFail->GetXaxis()->SetBinLabel(7,"OHCal towers");
  _SubsystemObjectsFail->GetXaxis()->SetBinLabel(8,"sEPD towers");

}

float MultiDetectorMultiplicity::getFilteredNtracks(PHCompositeNode *topNode)
{
  SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");

  if (!trackmap)
  {
    std::cout << PHWHERE
         << "SvtxTrackMap node is missing, can't collect tracks"
         << std::endl;
    _SubsystemObjectsFail->Fill(OBJECT::TRACKS);
    return -1;
  }

  float Ntracks = 0;

  for(SvtxTrackMap::Iter iter = trackmap->begin(); iter != trackmap->end(); ++iter)
	{
	  SvtxTrack* track = iter->second;
    float quality = track->get_quality();

    if (quality > 10) continue;

    // DCA units and values have to be cross-checked
    //float DCAxy = track->get_dca3d_xy(); //DCAxy in micrometer
    //if(std::abs(DCAxy) > 20) continue;
    //float DCAz = track->get_dca3d_z() * 10000; //DCAz in cm?, so multipli by 10^4
    //if(std::abs(DCAz) > 10) continue;

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

    if(nTPChits < 20) continue; // Require at least 20 TPC hits

    if (nsiliconhits < 2) continue; // Require at least 2 hits on silicon detectors

    Ntracks += 1.;
  }

  return Ntracks;
}

float MultiDetectorMultiplicity::getMVTXnclusters(PHCompositeNode *topNode)
{
  TrkrClusterContainer *MVTXclusterMap = findNode::getClass<TrkrClusterContainer>(topNode,"TRKR_CLUSTER");

  if(!MVTXclusterMap)
  {
    std::cout << PHWHERE
         << "TRKR_CLUSTER node is missing, can't collect hits"
         << std::endl;
    _SubsystemObjectsFail->Fill(OBJECT::MVTXCLUSTERS);
    return -1;
  }

  float MVTXclusters = 0.;

  for(const auto& hitsetkey : MVTXclusterMap->getHitSetKeys(TrkrDefs::TrkrId::mvtxId))
  {
    auto range = MVTXclusterMap->getClusters(hitsetkey);
    for( auto clusIter = range.first; clusIter != range.second; ++clusIter )
    {
      const auto cluster = clusIter->second;

      if(!cluster) continue;

      MVTXclusters += 1.;

    }
  }

  return MVTXclusters;

}

float MultiDetectorMultiplicity::getEMCalTotalEnergy(PHCompositeNode *topNode)
{
  TowerInfoContainer *towerinfosEM = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");

  if(!towerinfosEM)
  {
    std::cout << PHWHERE
         << "TOWERINFO_CALIB_CEMC node is missing, can't collect EMCal towers"
         << std::endl;
    _SubsystemObjectsFail->Fill(OBJECT::EMCALTOWERS);
    return -1.;
  }

  float EMCalTotalEnergy = 0.;

  TowerInfo *towerInfo = nullptr;
  for(unsigned int i = 0; i < towerinfosEM->size(); i++)
  {
    towerInfo = towerinfosEM->get_tower_at_channel(i);
    EMCalTotalEnergy += towerInfo->get_energy();
  }

  return EMCalTotalEnergy;
}

float MultiDetectorMultiplicity::getIHCalTotalEnergy(PHCompositeNode *topNode)
{
  TowerInfoContainer *towerinfosIH = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");

  if(!towerinfosIH)
  {
    std::cout << PHWHERE
         << "TOWERINFO_CALIB_HCALIN node is missing, can't collect IHCal towers"
         << std::endl;
    _SubsystemObjectsFail->Fill(OBJECT::IHCALTOWERS);
    return -1.;
  }

  float IHCalTotalEnergy = 0.;

  TowerInfo *towerInfo = nullptr;
  for(unsigned int i = 0; i < towerinfosIH->size(); i++)
  {
    towerInfo = towerinfosIH->get_tower_at_channel(i);
    IHCalTotalEnergy += towerInfo->get_energy();
  }

  return IHCalTotalEnergy;
}

float MultiDetectorMultiplicity::getOHCalTotalEnergy(PHCompositeNode *topNode)
{
  TowerInfoContainer *towerinfosOH = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");

  if(!towerinfosOH)
  {
    std::cout << PHWHERE
         << "TOWERINFO_CALIB_HCALOUT node is missing, can't collect OHCal towers"
         << std::endl;
    _SubsystemObjectsFail->Fill(OBJECT::OHCALTOWERS);
    return -1.;
  }

  float OHCalTotalEnergy = 0.;

  TowerInfo *towerInfo = nullptr;
  for(unsigned int i = 0; i < towerinfosOH->size(); i++)
  {
    towerInfo = towerinfosOH->get_tower_at_channel(i);
    OHCalTotalEnergy += towerInfo->get_energy();
  }

  return OHCalTotalEnergy;
}

float MultiDetectorMultiplicity::getsEPDTotalEnergy(PHCompositeNode *topNode)
{
  TowerInfoContainer *towerinfosEPD = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_EPD");

  if(!towerinfosEPD)
  {
    std::cout << PHWHERE
         << "TOWERINFO_CALIB_EPD node is missing, can't collect sEPD towers"
         << std::endl;
    _SubsystemObjectsFail->Fill(OBJECT::EPDTOWERS);
    return -1.;
  }

  float EPDTotalEnergy = 0.;

  TowerInfo *towerInfo = nullptr;
  for(unsigned int i = 0; i < towerinfosEPD->size(); i++)
  {
    towerInfo = towerinfosEPD->get_tower_at_channel(i);
    EPDTotalEnergy += towerInfo->get_energy();
  }

  return EPDTotalEnergy;
}

void MultiDetectorMultiplicity::setCentralityHistoLabel(TH2 *histo, bool doYaxisLabels)
{
  for(int i = 0; i < 10; i++)
  {
    histo->GetXaxis()->SetBinLabel(i+1,Form("%d-%d%%",90-(i*10),100-(i*10)));
    if(doYaxisLabels) histo->GetYaxis()->SetBinLabel(i+1,Form("%d-%d%%",90-(i*10),100-(i*10)));
  }
}

bool MultiDetectorMultiplicity::isVertexAccepted(PHCompositeNode *topNode)
{
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap)
  {
    std::cout << "GlobalVertexMap node is missing. Event aborted!" << std::endl;
    _SubsystemObjectsFail->Fill(OBJECT::VERTEX);
    return false;
  }

  if (vertexmap->empty())
  {
    std::cout << "GlobalVertexMap node is empty. Event aborted!" << std::endl;
    _SubsystemObjectsFail->Fill(OBJECT::VERTEX);
    return false;
  }

  GlobalVertex *vtx = vertexmap->begin()->second;
  if (vtx == nullptr)
  {
    _SubsystemObjectsFail->Fill(OBJECT::VERTEX);
    return false;
  }

  if(std::abs(vtx->get_z()) > _zVertexAcceptance) return false;

  return true;
}
