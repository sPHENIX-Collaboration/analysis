#include "SiliconCaloMatching.h"
#include "SiliconCaloTrack_v1.h"
#include "SiliconCaloTrackMap_v1.h"

#include <trackbase/TrkrDefs.h>
#include <trackbase/ActsGeometry.h>
#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainer.h>

//track map include
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>

//vertex include
#include <globalvertex/SvtxVertex.h>
#include <globalvertex/SvtxVertexMap.h>

//calo include
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawTowerGeomContainer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <TVector3.h>

// for sort algorithm
bool compLayer(TrkrDefs::cluskey a, TrkrDefs::cluskey b)
{
  int layer_a = TrkrDefs::getLayer(a);
  int layer_b = TrkrDefs::getLayer(b);
  return (layer_a < layer_b);
}

//____________________________________________________________________________..
SiliconCaloMatching::SiliconCaloMatching(const std::string &name)
    : SubsysReco(name)
{
}

#define LOG(msg) std::cout << "[SiliconCaloMatching] " << msg << std::endl;


//____________________________________________________________________________..
int SiliconCaloMatching::InitRun(PHCompositeNode * topNode)
{

  PHNodeIterator iter(topNode);

  //Looking for the DST node
  PHCompositeNode* dstNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    std::cout << PHWHERE << "DST Node missing, doing nothing." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  PHNodeIterator iter_dst(dstNode);

  // Create the SiliconCalo and association nodes if required
  _sicalomap = findNode::getClass<SiliconCaloTrackMap>(dstNode, "SiliconCaloTrack");
  if (!_sicalomap)
  {
    PHNodeIterator dstiter(dstNode);
    PHCompositeNode* DetNode =
      dynamic_cast<PHCompositeNode*>(dstiter.findFirst("PHCompositeNode", "SICALO"));
    if (!DetNode)
    {
      DetNode = new PHCompositeNode("SICALO");
      dstNode->addNode(DetNode);
    }
    
    _sicalomap = new SiliconCaloTrackMap_v1;
    PHIODataNode<PHObject>* SiliconCaloTrackMapNode =
      new PHIODataNode<PHObject>(_sicalomap, "SiliconCaloTrack", "PHObject");
    DetNode->addNode(SiliconCaloTrackMapNode);
  }

  // for logging
  std::cout<<"SiliconCaloMatching track low pT cut = " << _track_lowpt_cut <<std::endl;
  std::cout<<"SiliconCaloMatching emcal lowE cut   = " << _emcal_low_cut   <<std::endl;
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SiliconCaloMatching::process_event(PHCompositeNode* topNode)
{
  if(Verbosity()>0)
    std::cout<<"SiliconCaloMatching::process_event(PHCompositeNode* topNode)"<<std::endl;
  
  if(!getNodes(topNode)) 
    return Fun4AllReturnCodes::EVENT_OK;

  //if(false)
    //if((evt%1000)==0) std::cout << "start track map  EVENT " << evt << " is OK" << std::endl;
  
  // vertex position from crossing=0
  TVector3 vtxPos(0,0,0);
  //if (!b_skipvtx)
  {
    if (!_vertexmap->empty())
    {
      // crossing==0 has high priority
      for (const auto &[k, v] : *_vertexmap)
      {
        if (v && v->get_beam_crossing() == 0)
        {
          vtxPos = TVector3(v->get_x(), v->get_y(), v->get_z()) ;
          break;
        }
      }
    }
  }

///////////////////////////////////////////////
  int ntrk_crossing = 0, ntrk_noemc=0;
  for (auto &iter : *_trackmap)
  {
    auto track = iter.second;
    if (!track)
      continue;

    int trkcrossing = track->get_crossing();
    if (trkcrossing != 0 && !_isMC)
      continue;

    //--int   t_id      = track->get_id();
    float t_pt      = track->get_pt();
    if (t_pt < _track_lowpt_cut )
      continue;

    float t_eta     = track->get_eta();
    float t_phi     = track->get_phi();
    //--int   t_charge  = track->get_charge();
    //--float t_chi2ndf = track->get_quality();
    float t_x       = track->get_x();
    float t_y       = track->get_y();
    float t_z       = track->get_z();
    //--float t_px      = track->get_px();
    //--float t_py      = track->get_py();
    //--float t_pz      = track->get_pz();
    //--int t_crossing  = trkcrossing;
    //--if(t_crossing==0) evt_nsiseed0++;

//    int t_nmaps = 0, t_nintt = 0, t_inner = 0, t_outer = 0;


    if (Verbosity()>2)
      std::cout << "track_x : " << t_x << ", track_y: " << t_y << ", track_z: " << t_z 
                << ", track_eta: " << t_eta << ", track_phi: " << t_phi << ", track_pt: " << t_pt << std::endl;

    // track projection to EMCAL
    SvtxTrackState *emcalState    = track->get_state(_caloRadiusEMCal);
 //   SvtxTrackState *emcalOutState = track->get_state(_caloRadiusEMCal+_caloThicknessEMCal);

    
    if(!emcalState){
      std::cout<<"No TrackState (EMC projection object), pT = "<<t_pt<<std::endl;
      continue;
    }

    ntrk_crossing++; // good track (crossing & state_ok)

    // track projection to EMC surface
    TVector3 projPos(emcalState->get_x(), emcalState->get_y(), emcalState->get_z());
    //float phi_proj = projPos.phi();
    //float z_proj =   projPos.z();

    // --- EMCal cluster matching ---
    // Find the closest EMCal cluster in eta/phi to this state
    float best_dR  = 9999.0;
    int   best_idx = -1;
    
    float best_dphi=9999., best_dz=9999.;
    
    const float dphi_sigma = 3.; // dphi_sigma is factor 3 wider than dz_sigma


    //search closest EMCAL cluster
    /*******************
     need to speed up
    *******************/
    //--std::cout<<"start EMCalClusmap " << std::endl;
    //--std::cout << "EMCalClusmap size: " << EMCalClusmap->size() << std::endl;
    for (unsigned int i = 0; i < _emcClusmap->size(); i++)
    { 
      auto *emc = _emcClusmap->getCluster(i);
      if (!emc)
        continue;
      if (emc->get_energy() < _emcal_low_cut)
        continue;

      //TVector3 emcPos(emc->get_x(), emc->get_y(), emc->get_z());

      float dz   = emc->get_z()   - projPos.z(); // cm unit

      float dphi = emc->get_phi() - projPos.Phi(); // radian
      if (dphi >  M_PI) dphi -= 2 * M_PI;
      if (dphi < -M_PI) dphi += 2 * M_PI;


      float dphi_cm = dphi * _caloRadiusEMCal; // (Radius=93.5cm) cm

      float dR = sqrt(pow( (dphi_cm/dphi_sigma), 2) + pow(dz, 2));

      if(dR<best_dR){
        best_dR  = dR;
        best_idx =  i;

        best_dphi = dphi;
        best_dz   = dz;
      }
    }

    //
    if(best_idx==-1) {
      if(Verbosity()>0) std::cout<<"associated cluster not found, pt="<<t_pt<<std::endl;
      ntrk_noemc++;
      continue;
    }

    // associated cluster
    RawCluster *emcClus = _emcClusmap->getCluster(best_idx);

    // momentum update
    //
    //
    float pt_calo = calculatePt(track, emcClus);
    //--std::cout << "caloPt : " << pt_calo<<std::endl;


    // Fill updated value to SvtxTrack and SiliconCaloTrack object
    if(_update_pTinSvtxTrack){
      track->set_px( pt_calo*cos(track->get_phi()) );
      track->set_py( pt_calo*sin(track->get_phi()) );
    }

    
    auto sicalo = std::make_unique<SiliconCaloTrack_v1>();

    sicalo->set_id(track->get_id());
    sicalo->set_calo_id(best_idx);
    sicalo->set_pt(pt_calo);
    sicalo->set_phi(track->get_phi());
    sicalo->set_eta(track->get_eta());
    sicalo->set_cal_dphi(best_dphi);
    sicalo->set_cal_dz(best_dz);

    //_sicalomap->insert(track->get_id(), sicalo.release());
    _sicalomap->insertWithKey(sicalo.release(), track->get_id());

  } // track loop

  if(Verbosity()>0) {
    std::cout<<"SiliconCaloMatching : Ntrack : "<<_trackmap->size()<<" "<<ntrk_crossing<<", bad= "<<ntrk_noemc<<std::endl;
    std::cout<<"SiliconCaloMatching : NEMC   : "<<_emcClusmap->size() << std::endl;
    
    if(ntrk_noemc>0){
      for (unsigned int i = 0; i < _emcClusmap->size(); i++)
      { 
        auto *emc = _emcClusmap->getCluster(i);
        if (!emc) {
          std::cout<<"   no emc object"<<std::endl;
          continue;
        }
        if (emc->get_energy() < _emcal_low_cut) {
          std::cout<<"   small emc E "<< emc->get_energy()<<std::endl;
          continue;
        }

        //TVector3 emcPos(emc->get_x(), emc->get_y(), emc->get_z());

        float z_emc   = emc->get_z();
        float phi_emc = emc->get_phi();
        float r_emc   = emc->get_r();
        float x_emc   = r_emc * cos(phi_emc);
        float y_emc   = r_emc * sin(phi_emc);
        
        std::cout<<"     emc xyz = "<<x_emc<<" "<<y_emc<<" "<<z_emc<<", "<<phi_emc<<" "<<r_emc<<std::endl;
      }
    }
  }

  if(ntrk_noemc>0)
    std::cout<<"SiliconCaloMatching : No EMC track : "<<ntrk_noemc<<std::endl;

  //--std::cout << "EVENT " << evt << " is OK" << std::endl;

///////////////////////////////////////////////

  return Fun4AllReturnCodes::EVENT_OK;
}

float SiliconCaloMatching::calculatePt(SvtxTrack* track, RawCluster* emc)
{
  if(track==nullptr||emc==nullptr) {
    return -9999;
  }

  TrackSeed* si_seed = track->get_silicon_seed();

  // Si position
  if(si_seed!=nullptr&& si_seed->size_cluster_keys()<=2) return -9999.;
  
  // checking cluster list to get clusters from 2 outer layers
  // loop over SiClusters with the track
  std::vector<TrkrDefs::cluskey> vClusKey;
  for (auto key_iter = si_seed->begin_cluster_keys(); key_iter != si_seed->end_cluster_keys(); ++key_iter)
  {
    const auto &cluster_key  = *key_iter;
    TrkrCluster* trkrCluster = _clustermap->findCluster(cluster_key);
    if (!trkrCluster)
    {
      continue;
    }
    vClusKey.push_back(cluster_key);
  }
  std::sort(vClusKey.begin(), vClusKey.end(), compLayer); // sort by layer
  //std::cout<<"Nclus : "<<si_seed->size_cluster_keys()<<" "<<vClusKey.size()<<std::endl;
  //
  
  std::vector<TrkrDefs::cluskey>::reverse_iterator ritr = vClusKey.rbegin();
  TrkrDefs::cluskey ckey_outer = *ritr; ritr++;
  TrkrDefs::cluskey ckey_inner = *ritr;
  if(Verbosity()>2)
    std::cout<<"c layer "<<(int)TrkrDefs::getLayer(ckey_inner)<<" "<<(int)TrkrDefs::getLayer(ckey_outer)<<std::endl;

  TrkrCluster* oClus = _clustermap->findCluster(ckey_outer);
  TrkrCluster* iClus = _clustermap->findCluster(ckey_inner);

  Acts::Vector3 oCpos = _geometry->getGlobalPosition(ckey_outer, oClus);
  Acts::Vector3 iCpos = _geometry->getGlobalPosition(ckey_inner, iClus);

  //float phi_intt = (oCpos-iCpos).phi();

  
  // EMC position
  float emc_x, emc_y, emc_z;
  getCaloPosition(emc, emc_x, emc_y, emc_z);


  // pT calculation
  float phi_intt = atan2(oCpos.y()-iCpos.y(), oCpos.x()-iCpos.x());
  float phi_calo = atan2(emc_y - oCpos.y(), emc_x - oCpos.x());

  int charge = track->get_charge();

  float dphi = phi_calo - phi_intt;

  //float pt_calo = 0.21*pow((double)(-charge*dphi), -0.986);//cal_CaloPt(dphi);
  float pt_calo = 0.21*pow(fabs(dphi), -0.986);//cal_CaloPt(dphi);
  //float pt_inv = charge*0.02+4.9*(-charge*dphi)-0.6*pow(-charge*dphi, 2);
  //float pt_calo = 1./pt_inv;
  
  // cout
  float pt = track->get_pt();

  if(Verbosity()>2)
    std::cout<<"phi : "<<pt_calo<<" "<<phi_intt<<" "<<phi_calo<<" "<<dphi<<", "<<charge<<", "<<pt<<" "<<emc_x<<" "<<emc_y<<" "<<emc_z<<std::endl;

  return pt_calo;
}


//____________________________________________________________________________..
int SiliconCaloMatching::EndRun(const int /*runnumber*/)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SiliconCaloMatching::End(PHCompositeNode * /*unused*/)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

bool SiliconCaloMatching::getNodes(PHCompositeNode* topNode)
{

  _trackmap   = findNode::getClass<SvtxTrackMap>(        topNode, _trackMapName);
  _vertexmap  = findNode::getClass<SvtxVertexMap>(       topNode, _vertexMapName);
  _geometry   = findNode::getClass<ActsGeometry>(        topNode, _actsgeometryName);
  _clustermap = findNode::getClass<TrkrClusterContainer>(topNode, _clusterContainerName);
  _emcClusmap = findNode::getClass<RawClusterContainer>( topNode, _emcalClusName);


  if (!_trackmap)
  {
    std::cout << PHWHERE << "Missing trackmap, can't continue" << std::endl;
    return false;
  }
  if (!_vertexmap)
  {
    std::cout << PHWHERE << "Missing vertexmap, can't continue" << std::endl;
    return false;
  }
  if (!_geometry)
  {
    std::cout << PHWHERE << "Missing geometry, can't continue" << std::endl;
    return false;
  }
  if (!_clustermap)
  {
    std::cout << PHWHERE << "Missing clustermap, can't continue" << std::endl;
    return false;
  }
  if (!_emcClusmap)
  {
    std::cout << PHWHERE << "Missing EMC clustermap, can't continue" << std::endl;
    return false;
  }

  //for debug
  bool debug=false;
  if(debug){
    // get tower geometry container
    const auto towerGeomContainer = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
    
    if( !towerGeomContainer )
    {
      std::cout << PHWHERE << "-"
            << " Calo tower geometry container for CEMC" 
            << " not found on node tree. Track projections to calos won't be filled."
            << std::endl;
      return false;
    }
    
    // get calorimeter inner radius and store
    double caloRadius = towerGeomContainer->get_radius();
    std::cout<<" CEMC radius : "<<caloRadius<<std::endl;
  }

  return true;
}

void SiliconCaloMatching::getCaloPosition(RawCluster *calo, float &x, float &y, float &z)
{
  if(calo==nullptr) {
    x = y = z = std::numeric_limits<float>::quiet_NaN();
    return;
  }

  x   = calo->get_x();
  y   = calo->get_y();
  z   = calo->get_z();
  float r   = calo->get_r();
  float phi = calo->get_phi();
  float rr = sqrt(x*x + y*y);
  //std::cout<<"emc_x, y: "<<emc_x<<" "<<emc_y<<std::endl;
  if(fabs(rr - r)>1) 
  {
    //std::cout<<"no emc_x, y: "<<emc_x<<" "<<emc_y<<" "<<rr
    //         <<", replaced by r + phi"<<emc_r<<" "<<emc_phi<<" : ";
    x = r * cos(phi);
    y = r * sin(phi);
    //std::cout<<emc_x<<" "<<emc_y<<std::endl;
  }
}

