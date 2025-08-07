#include "laserQA.h"

#include <trackbase/LaserClusterContainerv1.h>
#include <trackbase/LaserClusterv1.h>
#include <trackbase/TpcDefs.h>
#include <trackbase/TrkrDefs.h>

#include <ffaobjects/EventHeader.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TVector3.h>

#include <string>



laserQA::laserQA(const std::string& name)
: SubsysReco(name)
{
}

int laserQA::InitRun(PHCompositeNode* topNode)
{

  m_nLaserEvents = new TH1D("nLaserEvents","Number of Laser Events",1,0.5,1.5);

  double rBinEdges[5] = {0.0, 0.256, 0.504, 0.752, 1.0};
  
  for(int side=0; side<2; side++){
    m_TPCWheel[side] = new TH2D(Form("TPCWheel_%s",(side == 1 ? "North" : "South")),Form("Laser Hit ADC per event %s",(side == 1 ? "North" : "South")),12,-TMath::Pi()/12.,23.*TMath::Pi()/12.,4,rBinEdges);
  }
  
  return Fun4AllReturnCodes::EVENT_OK;

}


int laserQA::process_event(PHCompositeNode* topNode)
{
	
  EventHeader *eventHeader = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if(!eventHeader)
  {
    std::cout << PHWHERE << " EventHeader Node missing, abort" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  
  LaserClusterContainer *lcc = findNode::getClass<LaserClusterContainer>(topNode, "LASER_CLUSTER");
  if (!lcc)
  {
    std::cout << PHWHERE << "LASER_CLUSTER Node missing, abort." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  
  std::cout << "working on event " << eventHeader->get_EvtSequence() << std::endl;
  
  if(lcc->size() < 1000) return Fun4AllReturnCodes::EVENT_OK;

  m_nLaserEvents->Fill(1.0);
  
  auto clusrange = lcc->getClusters();
  for (auto cmitr = clusrange.first;
       cmitr != clusrange.second;
       ++cmitr)
  {
    const auto& [cmkey, cmclus_orig] = *cmitr;
    LaserCluster* cmclus = cmclus_orig;
    int side = TpcDefs::getSide(cmkey);

    const unsigned int nhits = cmclus->getNhits();
    for (unsigned int i=0; i<nhits; i++){
      float layer = cmclus->getHitLayer(i);
      float hitAdc = cmclus->getHitAdc(i);

      double phi = atan2(cmclus->getHitY(i), cmclus->getHitX(i));
      if(phi < -TMath::Pi()/12.) phi += 2*TMath::Pi();
      
      int mod = -1;
      double RValue = -999;
      if(layer >= 7 && layer <= 22){
	mod = 0;
	RValue = 0.4;
      }
      else if(layer >= 23 && layer <= 38){
	mod = 1;
	RValue = 0.6;
      }
      else if(layer >= 39 && layer <= 54){
	mod = 2;
	RValue = 0.8;
      }

      if(mod == -1) continue;
      
      m_TPCWheel[side]->Fill(phi, RValue, hitAdc);
      
    }
    
    
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int laserQA::End(PHCompositeNode* /*topNode*/)
{
  TFile *outfile = new TFile(m_output.c_str(),"RECREATE");
  outfile->cd();
  
  m_nLaserEvents->Write();
  for(int side=0; side<2; side++){
    m_TPCWheel[side]->Write();
  }

  outfile->Close();
  
  return Fun4AllReturnCodes::EVENT_OK;
}
