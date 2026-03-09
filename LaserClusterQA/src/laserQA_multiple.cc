#include "laserQA_multiple.h"

#include <trackbase/LaserClusterContainer.h>
#include <trackbase/LaserCluster.h>
#include <trackbase/TpcDefs.h>


#include <ffaobjects/EventHeader.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>

//#include <TFile.h>
//#include <TTree.h>
#include <TVector3.h>

#include <string>



laserQA_multiple::laserQA_multiple(const std::string& name)
: SubsysReco(name)
{
}

int laserQA_multiple::InitRun(PHCompositeNode* topNode)
{

  outputFile = new TFile(m_output.c_str(),"RECREATE");

  //hitHist = new TH3D("hitHist",";layer;iphi;it;adc",48,6.5,57.5,1501,-0.5,1500.5,426,-0.5,425.5);
  
  T = new TTree("T","T");
  T->Branch("event",&event);
  T->Branch("hits",&hitHist);
  T->Branch("clusters",&clustHist);
  T->Branch("adc",&adc);
  T->Branch("layer",&layer);
  T->Branch("iphi",&iphi);
  T->Branch("it",&it);
  T->Branch("phi",&phi);
  T->Branch("R",&R);
  T->Branch("fitMode",&fitMode);
  
  //T->Branch("laserCluster",&lc);

  return Fun4AllReturnCodes::EVENT_OK;
  
}


int laserQA_multiple::process_event(PHCompositeNode* topNode)
{

  LaserClusterContainer *lcc = findNode::getClass<LaserClusterContainer>(topNode, "LASER_CLUSTER");
  if (!lcc)
  {
    std::cout << PHWHERE << "LASER_CLUSTER Node missing, abort." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  EventHeader *eventHeader = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventHeader)
  {
    std::cout << PHWHERE << " EventHeader Node missing, abort" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  event = eventHeader->get_EvtSequence();
  

  if(lcc->size() < 1000) return Fun4AllReturnCodes::EVENT_OK;

  //hitHist = nullptr;
  
  //bool fillTree = false;

  //std::cout << "good event " << std::endl;


  std::cout << "there are " << lcc->size() << " clusters in this event" << std::endl;
  
  std::vector<std::vector<TrkrDefs::cluskey>> clusters;
  
  auto clusrange = lcc->getClusters();
  for (auto cmitr = clusrange.first;
       cmitr != clusrange.second;
       ++cmitr)
  {

    const auto& [cmkey, cmclus_orig] = *cmitr;
    //LaserCluster* cmclus = cmclus_orig;
    //		bool side = (bool) TpcDefs::getSide(cmkey);
    //		const unsigned int nhits = cmclus->getNhits();
    
    //layer = cmclus->getLayer();
    //if(cmclus->getNIPhi() <= 2 || (cmclus->getNLayers() == 1 && (layer == 22 || layer == 38 || layer == 54))) continue;

    clusters.push_back(std::vector<TrkrDefs::cluskey>{cmkey});

    if(clusters.size() == 131)
    {
      std::cout << "Cluster 131 hits:" << std::endl;
      LaserCluster* cmclus = cmclus_orig;
      for(unsigned int i=0; i<cmclus->getNhits(); i++){
	std::cout << "   hit " << i << ": (" << cmclus->getHitLayer(i) << ", " << cmclus->getHitIPhi(i) << ", " << cmclus->getHitIT(i) << ", " << cmclus->getHitAdc(i) << ")" << std::endl;
      }
    }
    
  }

  for(unsigned int i=0; i<clusters.size(); i++)
  {

    LaserCluster* cmclus = lcc->findCluster(clusters[i][0]);
    float li = cmclus->getLayer();
    float pi = cmclus->getIPhi();
    float ti = cmclus->getIT();
    
    for(unsigned int j=0; j<clusters.size(); j++)
    {
      if(i == j) continue;
      if(TpcDefs::getSide(clusters[i][0]) != TpcDefs::getSide(clusters[j][0])) continue;
      if(TpcDefs::getSectorId(clusters[i][0]) != TpcDefs::getSectorId(clusters[j][0])) continue;
      LaserCluster* cmclus2 = lcc->findCluster(clusters[j][0]);
      float lj = cmclus2->getLayer();
      float pj = cmclus2->getIPhi();
      float tj = cmclus2->getIT();

      if(fabs(li - lj) < 5 && fabs(pi - pj) < 5 && fabs(ti - tj) < 8)
      {
	clusters[i].push_back(clusters[j][0]);
      }
      
    }
  }

  
  for(unsigned int i=0; i<clusters.size(); i++)
  {
    LaserCluster* cmclusI = lcc->findCluster(clusters[i][0]);
    layer = cmclusI->getLayer();
    iphi = cmclusI->getIPhi();
    it = cmclusI->getIT();
    adc = cmclusI->getAdc();

    fitMode = cmclusI->getFitMode();
    
    hitHist = new TH3D("hitHist",";layer;iphi;it;adc",9,round(layer)-4.5,round(layer)+4.5,9,round(iphi)-4.5,round(iphi)+4.5,15,round(it)-7.5,round(it)+7.5);
    clustHist = new TH3D("clustHist",";layer;iphi;it;adc",9,round(layer)-4.5,round(layer)+4.5,9,round(iphi)-4.5,round(iphi)+4.5,15,round(it)-7.5,round(it)+7.5);
  
    for(unsigned int j=0; j<clusters[i].size(); j++)
    {

      LaserCluster* cmclus = lcc->findCluster(clusters[i][j]);
    
      unsigned int nHits = cmclus->getNhits();
    
      for(unsigned int h=0; h<nHits; h++)
      {
	//std::cout << "   hit num: " << i << std::endl;
	hitHist->Fill(cmclus->getHitLayer(h), cmclus->getHitIPhi(h), cmclus->getHitIT(h), cmclus->getHitAdc(h));
	clustHist->Fill(cmclus->getHitLayer(h), cmclus->getHitIPhi(h), cmclus->getHitIT(h), j+1);
      }
      //hitHist = (TH3D*)tmpHitHist->Clone();

    }

    T->Fill();
    
    if(hitHist)
    {
      delete hitHist;
    }

    if(clustHist)
    {
      delete clustHist;
    }

  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int laserQA_multiple::End(PHCompositeNode* /*topNode*/)
{
	outputFile->cd();

	T->Write();

	outputFile->Close();

	return Fun4AllReturnCodes::EVENT_OK;
}
