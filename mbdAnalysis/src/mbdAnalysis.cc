//#include <bbc/BbcPmtContainerV1.h>
//#include <bbc/BbcGeom.h>
#include <mbd/MbdPmtContainer.h>
#include <mbd/MbdGeom.h>
#include <mbd/MbdPmtHit.h>
#include <phool/getClass.h>
#include <mbd/MbdPmtContainerV1.h>
#include <mbd/MbdOut.h>

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
// Centrality
#include <centrality/CentralityInfo.h>
// Minimum Bias
#include <calotrigger/MinimumBiasInfo.h>


#include <ffaobjects/EventHeader.h>
#include <ffaobjects/EventHeaderv1.h>

//____________________________________________________________________________..
mbdAnalysis::mbdAnalysis(const std::string &name):
 SubsysReco("poo")
    ,T(nullptr)
    ,Outfile(name)
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

  out = new TFile(Outfile.c_str(),"RECREATE");
  
  T = new TTree("T","T");
  T -> Branch("pmtcharge",&pmtcharge);
  T -> Branch("pmttime",&pmttime);
  T -> Branch("pmtx",&pmtx);
  T -> Branch("pmty",&pmty);
  T -> Branch("pmtz",&pmtz);
  T -> Branch("pmtr",&pmtr);
  T -> Branch("pmtphi",&pmtphi);
  T -> Branch("vertex",&vertex);
  T->Branch("cent",&cent);
  T -> Branch("evtPlaneAngle",&evtPlaneAngle);
  
  
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

  //MbdOut I think needed for vertex
  MbdOut *mbdout = findNode::getClass<MbdOut>(topNode,"MbdOut"); 
  if(!mbdout)
    {
      std::cout << "makeMBDTrees::process_event: Could not find MbdOut, aborting" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //Truth Informaiton 
  EventHeader *evtHeader = findNode::getClass<EventHeader>(topNode,"EventHeader"); 
  if(!evtHeader)
    {
      std::cout << "makeMBDTrees::process_event: Could not find EventHeader, aborting" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
/*
  CentralityInfo *centInfo = findNode::getClass<CentralityInfo>(topNode,"CentralityInfo");
  if(!centInfo)
    {
      std::cout << PHWHERE << "caloTreeGen::process_event: Could not find node CentralityInfo" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  cent = centInfo->get_centile(CentralityInfo::PROP::mbd_NS);
*/
/*
  MinimumBiasInfo *minBiasInfo = findNode::getClass<MinimumBiasInfo>(topNode,"MinimumBiasInfo");
  Bool_t isMinBias = (minBiasInfo) ? minBiasInfo->isAuAuMinimumBias() : false;
  if(!minBiasInfo || !isMinBias)
  {
    std::cout << PHWHERE << "caloTreeGen::process_event: is not MinimumBias" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  */
  //pmg geometry information
  MbdGeom *mbdgeom = findNode::getClass<MbdGeom>(topNode, "MbdGeom");
  if(!mbdgeom)
    {
      std::cout << "makeMBDTrees::process_event: Could not find MbdGeom, aborting" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //int nPMTs = bbcpmts -> get_npmt();        //size (should always be 128)
  int nPMTs = 128;        //size (should always be 128)
  vertex = mbdout->get_zvtx();
  std::cout << "NPMTS: "<< nPMTs << std::endl;
  evtPlaneAngle = evtHeader -> get_EventPlaneAngle();
  double m_cent = evtHeader->get_ImpactParameter();
  if(m_cent >= 0 && m_cent < 4.88) cent = 0.05;
  if(m_cent >= 4.88 && m_cent < 6.81) cent = 0.15;
  if(m_cent >= 6.81 && m_cent < 8.40) cent = 0.25;
  if(m_cent >= 8.40 && m_cent < 9.71) cent = 0.35;
  if(m_cent >= 9.71 && m_cent < 10.81) cent = 0.45;
  if(m_cent >= 10.81 && m_cent < 11.84) cent = 0.55;
  if(m_cent >= 11.84 && m_cent < 12.76) cent = 0.65;
  if(m_cent >= 12.76 && m_cent < 13.68) cent = 0.75;
  if(m_cent >= 13.68 && m_cent < 14.65) cent = 0.85;

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
