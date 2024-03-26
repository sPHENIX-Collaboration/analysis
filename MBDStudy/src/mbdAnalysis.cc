//#include <bbc/BbcPmtContainerV1.h>
//#include <bbc/BbcGeom.h>
#include <bbc/MbdOut.h>
#include <bbc/MbdPmtContainer.h>
#include <bbc/MbdGeom.h>
#include <bbc/MbdPmtHit.h>
#include <phool/getClass.h>

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
//  T -> Branch("adc",&adc);
//  T -> Branch("tdc0",&tdc0);
//  T -> Branch("tdc1",&tdc1);
  T -> Branch("pmtcharge",&pmtcharge);
  T -> Branch("pmttime",&pmttime);
  T -> Branch("pmtx",&pmtx);
  T -> Branch("pmty",&pmty);
  T -> Branch("pmtz",&pmtz);
  T -> Branch("pmtr",&pmtr);
  T -> Branch("pmtphi",&pmtphi);

  
  
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
  //these two lines copied directly from wiki:
  //PHNodeIterator iter(topNode);
  //PHCompositeNode *mbdNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "MBD"));
  //MbdOut *mbdout = findNode::getClass<MbdOut>(mbdNode, "MbdOut");

  MbdPmtContainer *mbdpmts = findNode::getClass<MbdPmtContainer>(topNode,"MbdPmtContainer"); // mbd info
  if(!mbdpmts)
    {
      std::cout << "makeMBDTrees::process_event: Could not find MbdPmtContainer, aborting" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  
  //MbdGeom *mbdGeom = new MbdGeom();
  MbdGeom *mbdgeom = findNode::getClass<MbdGeom>(topNode, "MbdGeom");




  int nPMTs = mbdpmts -> get_npmt();        //size (should always be 128)
  for(int i = 0; i < nPMTs; i++)
    {
      //short pmtID = mbdTowerContainer -> get_pmt(i);
      MbdPmtHit* mbdpmt = mbdpmts -> get_pmt(i);        // grab 1 pmt (changed from pmtID)
      
      //float pmtadc =  mbdpmts -> get_adc(i);        // grab adc from the ith pmt in mbdpmts
      //std::cout << "adc for tower i: " << i << " is: " <<pmtadc << std::endl;
      
     // pmtadc =  mbdpmts -> get_adc(mbdpmt);
      //std::cout << "adc for tower mbdpmt: " << mbdpmt << " is: " <<pmtadc << std::endl;
      //int pmtch =  mbdpmt->get_pmtch();        //pmt charge 
      float charge =  mbdpmt->get_q();        //pmt charge 
      float time =  mbdpmt->get_time();        //pmt charge 
      //std::cout << " pmt channel number: " << pmtch << "; pmtcharge: " << charge <<"; pmttime: "<< time <<  std::endl;
      
      pmttime.push_back(charge);
      pmtcharge.push_back(time);
	/*    
      adc.push_back(pmtadc);
      float pmttdc0 = mbdpmts -> get_tdc0(i);
      std::cout << "pmttdc0: " << pmttdc0 << std::endl;
      tdc0.push_back(pmttdc0);
      float pmttdc1 = mbdpmts -> get_tdc1(i);
      tdc1.push_back(pmttdc1);
*/
// according to the wiki https://wiki.sphenix.bnl.gov/index.php/MBD_DST_Output these should be get_x(i) rather tahn
// get_x(mbdpmt
      pmtx.push_back(mbdgeom->get_x(i));
      pmty.push_back(mbdgeom->get_y(i));
      pmtz.push_back(mbdgeom->get_z(i));
      pmtr.push_back(mbdgeom->get_r(i));
      pmtphi.push_back(mbdgeom->get_phi(i));

      std::cout << "phi angle of pmt is mbdgeom->get_phi(i)): " << mbdgeom->get_phi(i) << std::endl;
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
 // adc.clear();
  //tdc0.clear();
  //tdc1.clear();
  pmtcharge.clear();
  pmttime.clear();
  pmtx.clear();
  pmty.clear();
  pmtz.clear();
  pmtr.clear();
  pmtphi.clear();

  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void mbdAnalysis::Print(const std::string &what) const
{
  std::cout << "mbdAnalysis::Print(const std::string &what) const Printing info for " << what << std::endl;
}
