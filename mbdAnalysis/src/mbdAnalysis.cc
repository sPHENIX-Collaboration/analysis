//#include <bbc/BbcPmtContainerV1.h>
//#include <bbc/BbcGeom.h>
#include <mbd/MbdPmtContainer.h>
#include <mbd/MbdGeomV1.h>
#include <mbd/MbdPmtHit.h>
#include <phool/getClass.h>
#include <mbd/MbdPmtContainerV1.h>

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

  //pmt information
  MbdPmtContainer *bbcpmts = findNode::getClass<MbdPmtContainer>(topNode,"MbdPmtContainer"); // bbc info
  if(!bbcpmts)
    {
      std::cout << "makeMBDTrees::process_event: Could not find MbdPmtContainer, aborting" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  
  //pmg geometry information
  MbdGeom *mbdgeom = findNode::getClass<MbdGeom>(topNode, "MbdGeom");
  if(!mbdgeom)
    {
      std::cout << "makeMBDTrees::process_event: Could not find MbdGeom, aborting" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  int nPMTs = bbcpmts -> get_npmt();        //size (should always be 128)
  for(int i = 0; i < nPMTs; i++)
    {
      MbdPmtHit* mbdpmt = bbcpmts -> get_pmt(i);        // grab ith pmt (changed from pmtID)
      
      //store quantities of interest
      pmtcharge.push_back(mbdpmt -> get_q());
      pmttime.push_back(mbdpmt -> get_time());
      pmtx.push_back(mbdgeom->get_x(i));
      pmty.push_back(mbdgeom->get_y(i));
      pmtz.push_back(mbdgeom->get_z(i));
      pmtr.push_back(mbdgeom->get_r(i));
      pmtphi.push_back(mbdgeom->get_phi(i));

    }
  
  T -> Fill();
  
   
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int mbdAnalysis::ResetEvent(PHCompositeNode *topNode)
{
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
