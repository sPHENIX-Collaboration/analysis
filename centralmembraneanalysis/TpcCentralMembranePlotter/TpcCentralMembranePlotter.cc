#include "TpcCentralMembranePlotter.h"

#include <tpc/LaserEventInfo.h>


#include <trackbase/CMFlashDifferenceContainerv1.h>
#include <trackbase/CMFlashDifferencev1.h>
#include <trackbase/LaserClusterContainerv1.h>
#include <trackbase/LaserClusterv3.h>
#include <trackbase/TpcDefs.h>
#include <trackbase/TrkrHitTruthAssoc.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHit.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>

#include <g4detectors/PHG4TpcGeom.h>
#include <g4detectors/PHG4TpcGeomContainer.h>

#include <ffaobjects/EventHeader.h>

#include <cdbobjects/CDBTTree.h>

#include <fun4all/DBInterface.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>

#include <odbc++/resultset.h>
#include <odbc++/statement.h>

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH2.h>
#include <TH3.h>
#include <TLine.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TTree.h>
#include <TVector3.h>

#include <boost/format.hpp>
#include <boost/math/special_functions/lambert_w.hpp>


#include <cmath>
#include <format>
#include <iomanip>
#include <set>
#include <string>
#include <vector>

//________________________________
TpcCentralMembranePlotter::TpcCentralMembranePlotter(const std::string &name)
  : SubsysReco(name)
{
}

//____________________________________________
int TpcCentralMembranePlotter::InitRun(PHCompositeNode *topNode)
{
  for (int s = 0; s < 2; s++)
  {
    float zmin = (s == 1 ? -40 : 30);
    float zmax = (s == 1 ? -30 : 40);

    m_hits_hardware_layervsphibin[s] = new TH2D((boost::format("m_hits_hardware_layervsphibin_%s") %(s == 1 ? "North" : "South")).str().c_str(), (boost::format("TPC %s;#phi bin;Layer") %(s == 1 ? "North" : "South")).str().c_str(), 2600, 0, 2600, 50, 6, 56);
    m_hits_hardware_timebinvsphibin[s] = new TH2D((boost::format("m_hits_hardware_timebinvsphibin_%s") %(s == 1 ? "North" : "South")).str().c_str(), (boost::format("TPC %s;#phi bin;Time bin") %(s == 1 ? "North" : "South")).str().c_str(), 2600, 0, 2600, 40,310,350);
    m_hits_hardware_timebinvslayer[s] = new TH2D((boost::format("m_hits_hardware_timebinvslayer_%s") %(s == 1 ? "North" : "South")).str().c_str(), (boost::format("TPC %s;Layer;Time bin") %(s == 1 ? "North" : "South")).str().c_str(), 50, 6, 56, 40,310,350);
    
    m_clusters_r_vs_phi[s] = new TH2D((boost::format("m_clusters_r_vs_phi_%s") %(s == 1 ? "North" : "South")).str().c_str(), (boost::format("TPC %s;#phi;R [cm]") %(s == 1 ? "North" : "South")).str().c_str(), 6280, 0, 6.28, 200, 30, 80);
    m_clusters_z_vs_phi[s] = new TH2D((boost::format("m_clusters_z_vs_phi_%s") %(s == 1 ? "North" : "South")).str().c_str(), (boost::format("TPC %s;#phi;Z [cm]") %(s == 1 ? "North" : "South")).str().c_str(), 6280, 0, 6.28, 200, zmin, zmax);
    m_clusters_z_vs_r[s] = new TH2D((boost::format("m_clusters_z_vs_r_%s") %(s == 1 ? "North" : "South")).str().c_str(), (boost::format("TPC %s;R [cm];Z [cm]") %(s == 1 ? "North" : "South")).str().c_str(), 1000, 30, 80, 1000, zmin, zmax);

    m_hits_r_vs_phi[s] = new TH2D((boost::format("m_hits_r_vs_phi_%s") %(s == 1 ? "North" : "South")).str().c_str(), (boost::format("TPC %s;#phi;R [cm]") %(s == 1 ? "North" : "South")).str().c_str(), 6280, 0, 6.28, 200, 30, 80);
    m_hits_z_vs_phi[s] = new TH2D((boost::format("m_hits_z_vs_phi_%s") %(s == 1 ? "North" : "South")).str().c_str(), (boost::format("TPC %s;#phi;Z [cm]") %(s == 1 ? "North" : "South")).str().c_str(), 6280, 0, 6.28, 1000, zmin, zmax);
    m_hits_z_vs_r[s] = new TH2D((boost::format("m_hits_z_vs_r_%s") %(s == 1 ? "North" : "South")).str().c_str(), (boost::format("TPC %s;R [cm];Z [cm]") %(s == 1 ? "North" : "South")).str().c_str(), 1000, 30, 80, 1000, zmin, zmax);
    
    
  }


  int ret = GetNodes(topNode);
  return ret;
}

//__________________________________________
int TpcCentralMembranePlotter::process_event(PHCompositeNode *topNode)
{
  eventHeader = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventHeader)
  {
    std::cout << PHWHERE << " EventHeader Node missing, abort" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_runnumber = eventHeader->get_RunNumber();
  
  if (m_useHeader && eventHeader->get_EvtSequence() == 0)
  {
    m_useHeader = false;
  }

  if (m_useHeader)
  {
    m_event_index = eventHeader->get_EvtSequence();
  }

  if (!m_correctedCMcluster_map || m_correctedCMcluster_map->size() < 1000)
  {
    if (!m_useHeader)
    {
      m_event_index++;
    }
    return Fun4AllReturnCodes::EVENT_OK;
  }

  if ((eventHeader->get_RunNumber() > 66153 && !m_laserEventInfo->isGl1LaserEvent()) || (eventHeader->get_RunNumber() <= 66153 && !m_laserEventInfo->isLaserEvent()))
  {
    std::cout << "Not a laser event, skipping " << m_event_index << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  m_nClusters += m_correctedCMcluster_map->size();
  m_nEvents++;

  std::cout << "TpcCentralMembranePlotter::process_event - Processing event " << m_event_index << std::endl;
  std::cout << m_nClusters << std::endl;
  
  auto clusrange = m_correctedCMcluster_map->getClusters();

  for (auto cmitr = clusrange.first; cmitr != clusrange.second; ++cmitr)
  {
    const auto &[cmkey, cmclus_orig] = *cmitr;
    LaserCluster *cmclus = cmclus_orig;
    const unsigned int clusadc = cmclus->getAdc();
    bool side = (bool) TpcDefs::getSide(cmkey);

    if (cmclus->getNLayers() < m_nLayerCut)
    {
      continue;
    }

    // ******* Fill histogram of hits ******* //
    int nHitsInClus = cmclus->getNhits();
    for(int i=0; i< nHitsInClus; ++i)
    {
        const LaserClusterHitInfo hit= cmclus->getHit(i);

        TrkrHitTruthAssoc::MMap g4hit_map;
        m_hitTruthAssoc->getG4Hits(hit.hitsetkey, hit.hitkey, g4hit_map);

        // const bool has_truth = !g4hit_map.empty();

        // std::cout
        // << (has_truth ? "SIGNAL " : "NOISE  ")
        // << "hitsetkey " << hit.hitsetkey
        // << " trkrid " << static_cast<int>(TrkrDefs::getTrkrId(hit.hitsetkey))
        // << " layer " << static_cast<int>(TrkrDefs::getLayer(hit.hitsetkey))
        // << " side " << static_cast<int>(TpcDefs::getSide(hit.hitsetkey))
        // << " pad " << static_cast<int>(TpcDefs::getPad(hit.hitkey))
        // << " tbin " << static_cast<int>(TpcDefs::getTBin(hit.hitkey))
        // << " adc " << hit.adc
        // << " ng4 " << g4hit_map.size()
        // << std::endl;


        int hitside = TpcDefs::getSide(hit.hitsetkey);

        double hitadc = hit.adc;
        int hitlayer = TrkrDefs::getLayer(hit.hitsetkey);
        int hitiphi = TpcDefs::getPad(hit.hitkey);
        int hitit = TpcDefs::getTBin(hit.hitkey);

        PHG4TpcGeom *layer_geom = m_geom_container->GetLayerCellGeom(hitlayer);
        double hitenvradius = layer_geom->get_radius();
        double hitenvphi = layer_geom->get_phi(hitiphi, hitside);
        double hitenv_x = hitenvradius * cos(hitenvphi);
        double hitenv_y = hitenvradius * sin(hitenvphi);
        double hitenv_z = 0.0;
        Acts::Vector3 hitenv(hitenv_x, hitenv_y, hitenv_z);
        Acts::Vector3 global = m_tGeometry->transformTpcEnvelopeToWorld(hitenv);
        double hitglob_x = global[0];
        double hitglob_y = global[1];
        double hitglob_z = global[2];
        double hitglob_r = sqrt(pow(hitglob_x,2) + pow(hitglob_y,2));
        double hitglob_phi = atan2(hitglob_y,hitglob_x);

        if (hitenvphi < 0.0)
        {
          hitenvphi += 2 * M_PI;
        }
        if (hitglob_phi < 0.0)
        {
          hitglob_phi += 2 * M_PI;
        }
        
        m_hits_hardware_timebinvsphibin[hitside]->Fill(hitiphi, hitit, hitadc);
        m_hits_hardware_timebinvslayer[hitside]->Fill(hitlayer, hitit, hitadc);
        m_hits_hardware_layervsphibin[hitside]->Fill(hitiphi, hitlayer, hitadc);

        m_hits_z_vs_phi[hitside]->Fill(hitglob_phi, hitglob_z, hitadc);
        m_hits_z_vs_r[hitside]->Fill(hitglob_r, hitglob_z, hitadc);
        m_hits_r_vs_phi[hitside]->Fill(hitglob_phi, hitglob_r, hitadc);

        
    }
    
    Acts::Vector3 pos = m_laserClusterHelper.getClusterCentroid(cmclus);
    if (m_dcc_in_module_edge)
    {
      pos = m_distortionCorrection.get_corrected_position(pos, m_dcc_in_module_edge);
    }
    if (m_dcc_in_static)
    {
      pos = m_distortionCorrection.get_corrected_position(pos, m_dcc_in_static);
    }

    TVector3 tmp_pos(pos[0], pos[1], pos[2]);

    // if(cmclus->getSDWeightedLayer() > 0.5)
    // {
    //   continue;
    // }
    
    double phi2pimod = tmp_pos.Phi();
    if (phi2pimod < 0.0)
    {
      phi2pimod += 2 * M_PI;
    }
    m_clusters_r_vs_phi[side]->Fill(phi2pimod, tmp_pos.Perp(), clusadc);
    m_clusters_z_vs_phi[side]->Fill(phi2pimod, tmp_pos.Z(), clusadc);
    m_clusters_z_vs_r[side]->Fill(tmp_pos.Perp(), tmp_pos.Z(), clusadc);

  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int TpcCentralMembranePlotter::End(PHCompositeNode * /*topNode*/)
{ 
  TFile *outputfile = new TFile(m_outputfile.c_str(), "RECREATE");
  outputfile->cd();
  for (int s = 0; s < 2; s++)
  { 
    m_hits_hardware_layervsphibin[s]->Write();
    m_hits_hardware_timebinvsphibin[s]->Write();
    m_hits_hardware_timebinvslayer[s]->Write();

  }

  for (int s = 0; s < 2; s++)
  { 
    m_hits_r_vs_phi[s]->Write();
    m_hits_z_vs_phi[s]->Write();
    m_hits_z_vs_r[s]->Write();
  }

  for (int s = 0; s < 2; s++)
  { 
    m_clusters_r_vs_phi[s]->Write();
    m_clusters_z_vs_phi[s]->Write();
    m_clusters_z_vs_r[s]->Write();
  }

  outputfile->Close();
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//______________________________________
int TpcCentralMembranePlotter::GetNodes(PHCompositeNode *topNode)
{
  m_correctedCMcluster_map = findNode::getClass<LaserClusterContainer>(topNode, "LASER_CLUSTER");
  if (!m_correctedCMcluster_map)
  {
    std::cout << PHWHERE << "CORRECTED_CM_CLUSTER Node missing, abort." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_dcc_in_module_edge = findNode::getClass<TpcDistortionCorrectionContainer>(topNode, "TpcDistortionCorrectionContainerModuleEdge");
  if (m_dcc_in_module_edge)
  {
    std::cout << "TpcCentralMembranePlotter::GetNodes - found TPC distortion correction container module edge" << std::endl;
  }

  m_dcc_in_static = findNode::getClass<TpcDistortionCorrectionContainer>(topNode, "TpcDistortionCorrectionContainerStatic");
  if (m_dcc_in_static)
  {
    std::cout << "TpcCentralMembranePlotter::GetNodes - found TPC distortion correction container static" << std::endl;
  }

  PHNodeIterator iter(topNode);

  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    std::cout << PHWHERE << "DST Node missing, doing nothing." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  auto *flashDiffContainer = findNode::getClass<CMFlashDifferenceContainerv1>(topNode, "CM_FLASH_DIFFERENCES");
  if (!flashDiffContainer)
  {
    PHNodeIterator dstIter(dstNode);
    PHCompositeNode *DetNode = dynamic_cast<PHCompositeNode *>(dstIter.findFirst("PHCompositeNode", "TRKR"));
    if (!DetNode)
    {
      DetNode = new PHCompositeNode("TRKR");
      dstNode->addNode(DetNode);
    }

    flashDiffContainer = new CMFlashDifferenceContainerv1;
    PHIODataNode<PHObject> *CMFlashDifferenceNode = new PHIODataNode<PHObject>(flashDiffContainer, "CM_FLASH_DIFFERENCES", "PHObject");
    DetNode->addNode(CMFlashDifferenceNode);
  }

  const std::string dcc_out_node_name = "TpcDistortionCorrectionContainerAverage";
  m_dcc_out = findNode::getClass<TpcDistortionCorrectionContainer>(topNode, dcc_out_node_name);
  if (!m_dcc_out)
  {
    auto *runNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "RUN"));
    if (!runNode)
    {
      std::cout << "TpcCentralMembranePlotter::InitRun = RUN Node missing, quitting" << std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }

    std::cout << "TpcCentralMembranePlotter::GetNodes - creating TpcDistortionCorrectionContainer in node " << dcc_out_node_name << std::endl;
    m_dcc_out = new TpcDistortionCorrectionContainer;
    auto *node = new PHDataNode<TpcDistortionCorrectionContainer>(m_dcc_out, dcc_out_node_name);
    runNode->addNode(node);
  }

  m_tGeometry = findNode::getClass<ActsGeometry>(topNode,"ActsGeometry");
    if(!m_tGeometry)
    {
      std::cout << PHWHERE << "ERROR: Can't find node ActsGeometry" << std::endl;
    }

  m_geom_container = findNode::getClass<PHG4TpcGeomContainer>(topNode, "TPCGEOMCONTAINER");
  if (!m_geom_container)
  {
    std::cout << PHWHERE << "ERROR: Can't find node TPCGEOMCONTAINER" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_laserEventInfo = findNode::getClass<LaserEventInfo>(topNode, "LaserEventInfo");
  if (!m_laserEventInfo)
  {
    std::cout << PHWHERE << "ERROR: Can't find node LaserEventInfo" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_hitTruthAssoc = findNode::getClass<TrkrHitTruthAssoc>(topNode, "TRKR_HITTRUTHASSOC");
  if (!m_hitTruthAssoc)
  {
    std::cout << PHWHERE << "ERROR: Can't find TRKR_HITTRUTHASSOC" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_hits = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
  if (!m_hits)
  {
    std::cout << PHWHERE << "ERROR: Can't find TRKR_HITSET" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_laserClusterHelper.set_useZ(0);
  m_laserClusterHelper.loadNodes(topNode);

  return Fun4AllReturnCodes::EVENT_OK;
}

//_____________________________
void TpcCentralMembranePlotter::setOutputfile(const std::string &outputfile)
{
  m_outputfile = outputfile;
}
