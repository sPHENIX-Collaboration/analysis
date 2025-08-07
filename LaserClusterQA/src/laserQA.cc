#include "laserQA.h"

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



laserQA::laserQA(const std::string& name)
: SubsysReco(name)
{
}

int laserQA::InitRun(PHCompositeNode* topNode)
{

  outputFile = new TFile(m_output.c_str(),"RECREATE");

  //hitHist = new TH3D("hitHist",";layer;iphi;it;adc",48,6.5,57.5,1501,-0.5,1500.5,426,-0.5,425.5);
  
  T = new TTree("T","T");
  T->Branch("hits",&hitHist);
  T->Branch("adc",&adc);
  T->Branch("layer",&layer);
  T->Branch("iphi",&iphi);
  T->Branch("it",&it);
  T->Branch("phi",&phi);
  T->Branch("R",&R);
  
  //T->Branch("laserCluster",&lc);

  return Fun4AllReturnCodes::EVENT_OK;
  
}


int laserQA::process_event(PHCompositeNode* topNode)
{

  LaserClusterContainer *lcc = findNode::getClass<LaserClusterContainer>(topNode, "LASER_CLUSTER");
  if (!lcc)
  {
    std::cout << PHWHERE << "LASER_CLUSTER Node missing, abort." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  

  if(lcc->size() < 1000) return Fun4AllReturnCodes::EVENT_OK;

  //hitHist = nullptr;
  
  //bool fillTree = false;

  //std::cout << "good event " << std::endl;


  int clusNum = -1;
  
  auto clusrange = lcc->getClusters();
  for (auto cmitr = clusrange.first;
       cmitr != clusrange.second;
       ++cmitr)
  {

    //hitHist->Reset();

    
    clusNum++;
    //std::cout << "working on cluster " << clusNum << std::endl;
    
    const auto& [cmkey, cmclus_orig] = *cmitr;
    LaserCluster* cmclus = cmclus_orig;
    //		bool side = (bool) TpcDefs::getSide(cmkey);
    //		const unsigned int nhits = cmclus->getNhits();

    layer = cmclus->getLayer();
    //if(cmclus->getNIPhi() <= 2 || (cmclus->getNLayers() == 1 && (layer == 22 || layer == 38 || layer == 54))) continue;


    
    adc = cmclus->getAdc();
    iphi = cmclus->getIPhi();
    it = cmclus->getIT();

    float X = cmclus->getX();
    float Y = cmclus->getY();
    
    R = sqrt(X*X + Y*Y);
    phi = atan2(Y,X);
    
    hitHist = new TH3D("hitHist",";layer;iphi;it;adc",9,round(layer)-4.5,round(layer)+4.5,9,round(iphi)-4.5,round(iphi)+4.5,15,round(it)-7.5,round(it)+7.5);

    
    unsigned int nHits = cmclus->getNhits();
    
    for(unsigned int i=0; i<nHits; i++)
    {
      //std::cout << "   hit num: " << i << std::endl;
      hitHist->Fill(cmclus->getHitLayer(i), cmclus->getHitIPhi(i), cmclus->getHitIT(i), cmclus->getHitAdc(i));
    }
    //hitHist = (TH3D*)tmpHitHist->Clone();
    
    T->Fill();

    if(hitHist)
    {
      delete hitHist;
    }

  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int laserQA::End(PHCompositeNode* /*topNode*/)
{
	outputFile->cd();

	T->Write();

	outputFile->Close();

	return Fun4AllReturnCodes::EVENT_OK;
}
