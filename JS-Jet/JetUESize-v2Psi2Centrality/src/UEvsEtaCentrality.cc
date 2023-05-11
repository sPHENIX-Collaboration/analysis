//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in UEvsEtaCentrality.h.
//
// UEvsEtaCentrality(const std::string &name = "UEvsEtaCentrality")
// everything is keyed to UEvsEtaCentrality, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// UEvsEtaCentrality::~UEvsEtaCentrality()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int UEvsEtaCentrality::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int UEvsEtaCentrality::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int UEvsEtaCentrality::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT; 
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT; 
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int UEvsEtaCentrality::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int UEvsEtaCentrality::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int UEvsEtaCentrality::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int UEvsEtaCentrality::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void UEvsEtaCentrality::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "UEvsEtaCentrality.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/PHCompositeNode.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <g4jets/JetMap.h>
#include <g4jets/Jetv1.h>

#include <centrality/CentralityInfo.h>

#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

#include <jetbackground/TowerBackground.h>

#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
//____________________________________________________________________________..
UEvsEtaCentrality::UEvsEtaCentrality(const std::string& name, const std::string& outputfilename)
: SubsysReco(name)
  , m_outputFileName(outputfilename)
{
  std::cout << "UEvsEtaCentrality::UEvsEtaCentrality(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
UEvsEtaCentrality::~UEvsEtaCentrality()
{
  std::cout << "UEvsEtaCentrality::~UEvsEtaCentrality() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int UEvsEtaCentrality::Init(PHCompositeNode *topNode)
{
  std::cout << "UEvsEtaCentrality::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  
  if (Verbosity() >=  UEvsEtaCentrality::VERBOSITY_SOME)
    std::cout << "MyJetAnalysis::Init - Outoput to " << m_outputFileName << std::endl;

  PHTFileServer::get().open(m_outputFileName, "RECREATE");

  hv2_cent = new TH2F("hv2_cent","",10,0,100,50,0,0.5);
  hPsi2_cent = new TH2F("hPsi2_cent","",10,0,100,50,-1.57,1.57);
  hUEiHcalEta_Cent0_20 = new TH2F("hUEiHcalEta_Cent0_20", "", 24, -1.1,1.1, 48, 0, 0.25);
  hUEoHcalEta_Cent0_20 = new TH2F("hUEoHcalEta_Cent0_20", "", 24, -1.1,1.1, 48, 0, 0.5);
  hUEemcalEta_Cent0_20 = new TH2F("hUEemcalEta_Cent0_20", "", 24, -1.1,1.1, 48, 0, 1.5);
  
  hUEiHcalEta_Cent20_50 = new TH2F("hUEiHcalEta_Cent20_50", "", 24, -1.1,1.1, 48, 0, 0.25);
  hUEoHcalEta_Cent20_50 = new TH2F("hUEoHcalEta_Cent20_50", "", 24, -1.1,1.1, 48, 0, 0.5);
  hUEemcalEta_Cent20_50 = new TH2F("hUEemcalEta_Cent20_50", "", 24, -1.1,1.1, 48, 0, 1.5);

  hUEiHcalEta_Cent50_100 = new TH2F("hUEiHcalEta_Cent50_100", "", 24, -1.1,1.1, 48, 0, 0.25);
  hUEoHcalEta_Cent50_100 = new TH2F("hUEoHcalEta_Cent50_100", "", 24, -1.1,1.1, 48, 0, 0.5);
  hUEemcalEta_Cent50_100 = new TH2F("hUEemcalEta_Cent50_100", "", 24, -1.1,1.1, 48, 0, 1.5);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int UEvsEtaCentrality::InitRun(PHCompositeNode *topNode)
{
  std::cout << "UEvsEtaCentrality::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int UEvsEtaCentrality::process_event(PHCompositeNode *topNode)
{ 

  //centrality
  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!cent_node)
    {
      std::cout
        << "MyJetAnalysis::process_event - Error can not find centrality node "
        << std::endl;
      exit(-1);
    }

  

  //calorimeter towers
  TowerInfoContainer *towersEM3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
  TowerInfoContainer *towersIH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  TowerInfoContainer *towersOH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
  RawTowerGeomContainer *tower_geom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *tower_geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
  if(!towersEM3 || !towersIH3 || !towersOH3){
    std::cout
      <<"MyJetAnalysis::process_event - Error can not find raw tower node "
      << std::endl;
    exit(-1);
  }

  if(!tower_geom || !tower_geomOH){
    std::cout
      <<"MyJetAnalysis::process_event - Error can not find raw tower geometry "
      << std::endl;
    exit(-1);
  }

  //underlying event
  TowerBackground *background = findNode::getClass<TowerBackground>(topNode, "TowerInfoBackground_Sub2");
  if(!background){
    std::cout<<"Can't get background. Exiting"<<std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }
   
    float background_v2 = 0;
    float background_Psi2 = 0;
    float m_centrality = 0;

    m_centrality =  cent_node->get_centile(CentralityInfo::PROP::epd_NS); 
    background_v2 = background->get_v2();
    background_Psi2 = background->get_Psi2();

    hv2_cent->Fill(m_centrality, background_v2);
    hPsi2_cent->Fill(m_centrality, background_Psi2);
    
    int i=0;    
    for (i=0;i<=23;i++){
    
      

    float UEi = background->get_UE(1).at(i); //24 eta bins for each detector Inner Hcal
    float UEo = background->get_UE(2).at(i); //24 eta bins for each detector, Outer Hcal
    float UEe = background->get_UE(0).at(i); //24 eta bins for each detector, EMCal
   
    double eta = tower_geom->get_etacenter(i);

    if (m_centrality > 0 && m_centrality <= 20){
    hUEiHcalEta_Cent0_20->Fill(eta, UEi);                                                                                  
    hUEoHcalEta_Cent0_20->Fill(eta, UEo);                                                                                  
    hUEemcalEta_Cent0_20->Fill(eta, UEe);
      }
    if (m_centrality > 20 && m_centrality <= 50){
    hUEiHcalEta_Cent20_50->Fill(eta, UEi);                                                                                 
    hUEoHcalEta_Cent20_50->Fill(eta, UEo);                                                                                 
    hUEemcalEta_Cent20_50->Fill(eta, UEe);
    }

    if (m_centrality > 50 && m_centrality <= 100){
    hUEiHcalEta_Cent50_100->Fill(eta, UEi);
    hUEoHcalEta_Cent50_100->Fill(eta, UEo);
    hUEemcalEta_Cent50_100->Fill(eta, UEe);
    }


    }

 

  // std::cout << "UEvsEtaCentrality::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int UEvsEtaCentrality::ResetEvent(PHCompositeNode *topNode)
{
  // std::cout << "UEvsEtaCentrality::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;

  // IMPORTANT!! CLEAR YOUR VECTORS AND RESET YOUR TREE VARIABLES HERE!!

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int UEvsEtaCentrality::EndRun(const int runnumber)
{
  std::cout << "UEvsEtaCentrality::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int UEvsEtaCentrality::End(PHCompositeNode *topNode)
{
  std::cout << "MyJetAnalysis::End - Output to " << m_outputFileName << std::endl;
  PHTFileServer::get().cd(m_outputFileName);
  
  hv2_cent->Write();
  hPsi2_cent->Write();
  hUEiHcalEta_Cent0_20->Write();
  hUEoHcalEta_Cent0_20->Write();
  hUEemcalEta_Cent0_20->Write();
  hUEiHcalEta_Cent20_50->Write();
  hUEoHcalEta_Cent20_50->Write();
  hUEemcalEta_Cent20_50->Write();
  hUEiHcalEta_Cent50_100->Write();
  hUEoHcalEta_Cent50_100->Write();
  hUEemcalEta_Cent50_100->Write();
  std::cout << "UEvsEtaCentrality::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int UEvsEtaCentrality::Reset(PHCompositeNode *topNode)
{
 std::cout << "UEvsEtaCentrality::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void UEvsEtaCentrality::Print(const std::string &what) const
{
  std::cout << "UEvsEtaCentrality::Print(const std::string &what) const Printing info for " << what << std::endl;
}
