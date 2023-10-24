#include <bbc/BbcPmtContainerV1.h>
#include <bbc/BbcGeom.h>

#include <phool/PHCompositeNode.h>

//Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>
#include <ffaobjects/EventHeader.h>

//ROOT stuff
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TTree.h>

#include "mbdAnalysis.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//____________________________________________________________________________..
mbdAnalysis::mbdAnalysis(const std::string &name):
 SubsysReco(name)
{
  std::cout << "mbdAnalysis::mbdAnalysis(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
mbdAnalysis::~mbdAnalysis()
{
  std::cout << "mbdAnalysis::~mbdAnalysis() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int mbdAnalysis::Init(PHCompositeNode *topNode)
{

  out = new TFile("output.root","RECREATE");
  
  T = new TTree("T","T");
  T -> Branch("adc",&adc);
  T -> Branch("tdc0",&tdc0);
  T -> Branch("tdc1",&tdc1);
  T -> Branch("pmtx",&pmtx);
  T -> Branch("pmty",&pmty);
  T -> Branch("pmtz",&pmtz);
  T -> Branch("pmtr",&pmtr);
  
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int mbdAnalysis::InitRun(PHCompositeNode *topNode)
{
  std::cout << "mbdAnalysis::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int mbdAnalysis::process_event(PHCompositeNode *topNode)
{

  //tower information
  //mbdinfo
  BbcPmtContainerV1 *mbdTowerContainer = findNode::getClass<BbcPmtContainerV1>(topNode,"BbcPmtContainer");
  if(!mbdTowerContainer)
    {
      std::cout << "makeMBDTrees::process_event: Could not find BbcPmtContainer, aborting" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  
  BbcGeom *mbdGeom = new BbcGeom();
  int nPMTs = mbdTowerContainer -> get_npmt();
  for(int i = 0; i < nPMTs; i++)
    {
      short pmtID = mbdTowerContainer -> get_pmt(i);
      
      float pmtadc =  mbdTowerContainer -> get_adc(i);
      std::cout << "adc for tower i: " << i << " is: " <<pmtadc << std::endl;
      
      pmtadc =  mbdTowerContainer -> get_adc(pmtID);
      std::cout << "adc for tower pmtID: " << pmtID << " is: " <<pmtadc << std::endl;
	    
      adc.push_back(pmtadc);
      float pmttdc0 = mbdTowerContainer -> get_tdc0(i);
      std::cout << "pmttdc0: " << pmttdc0 << std::endl;
      tdc0.push_back(pmttdc0);
      float pmttdc1 = mbdTowerContainer -> get_tdc1(i);
      tdc1.push_back(pmttdc1);

      pmtx.push_back(mbdGeom->get_x(i));
      std::cout << "mbdGeom->get_x(pmtID): " << mbdGeom->get_x(pmtID) << std::endl;
      pmty.push_back(mbdGeom->get_y(i));
      pmtz.push_back(mbdGeom->get_z(i));
      pmtr.push_back(mbdGeom->get_r(i));

    }
  
  T -> Fill();
  
   
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int mbdAnalysis::ResetEvent(PHCompositeNode *topNode)
{
  std::cout << "mbdAnalysis::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int mbdAnalysis::EndRun(const int runnumber)
{
  std::cout << "mbdAnalysis::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int mbdAnalysis::End(PHCompositeNode *topNode)
{
  std::cout << "mbdAnalysis::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  out -> cd();
  T -> Write();
  out -> Close();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int mbdAnalysis::Reset(PHCompositeNode *topNode)
{
  adc.clear();
  tdc0.clear();
  tdc1.clear();
  pmtx.clear();
  pmty.clear();
  pmtz.clear();
  pmtr.clear();

  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void mbdAnalysis::Print(const std::string &what) const
{
  std::cout << "mbdAnalysis::Print(const std::string &what) const Printing info for " << what << std::endl;
}
