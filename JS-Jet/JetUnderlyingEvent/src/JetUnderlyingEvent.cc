//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in JetUnderlyingEvent.h.
//
// JetUnderlyingEvent(const std::string &name = "JetUnderlyingEvent")
// everything is keyed to JetUnderlyingEvent, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// JetUnderlyingEvent::~JetUnderlyingEvent()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int JetUnderlyingEvent::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int JetUnderlyingEvent::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int JetUnderlyingEvent::process_event(PHCompositeNode *topNode)
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
// int JetUnderlyingEvent::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int JetUnderlyingEvent::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int JetUnderlyingEvent::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int JetUnderlyingEvent::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void JetUnderlyingEvent::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "JetUnderlyingEvent.h"

#include <TH3.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <g4jets/JetMap.h>
#include <g4jets/Jetv1.h>

#include <centrality/CentralityInfo.h>
#include <TFile.h>
#include <TH3F.h>
#include <TH2.h>
#include <TH2F.h>
#include <TString.h>

#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>

#include <jetbackground/TowerBackground.h>




//____________________________________________________________________________..
JetUnderlyingEvent::JetUnderlyingEvent(const std::string& name, const std::string& outputfilename):
 SubsysReco(name)
 , m_outputFileName(outputfilename)
{
  std::cout << "JetUnderlyingEvent::JetUnderlyingEvent(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
JetUnderlyingEvent::~JetUnderlyingEvent()
{
  std::cout << "JetUnderlyingEvent::~JetUnderlyingEvent() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int JetUnderlyingEvent::Init(PHCompositeNode *topNode)
{
  std::cout << "JetUnderlyingEvent::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  PHTFileServer::get().open(m_outputFileName, "RECREATE");
  std::cout << "JetUnderlyingEvent::Init - Output to " << m_outputFileName << std::endl;

  // Histograms

  hsubtractedE = new TH3F("HsubtractedE", "", 100, 0.0, 100, 100, 0, 100, 10, 0, 100);
   hsubtractedE->GetXaxis()->SetTitle("E_{T} [GeV]");
   hsubtractedE->GetYaxis()->SetTitle("Subtracted E_{T} [GeV]");
   hsubtractedE->GetZaxis()->SetTitle("Centrality");
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetUnderlyingEvent::InitRun(PHCompositeNode *topNode)
{
  std::cout << "JetUnderlyingEvent::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetUnderlyingEvent::process_event(PHCompositeNode *topNode)
{
  // interface to reco jets
  JetMap* jets = findNode::getClass<JetMap>(topNode, "AntiKt_Tower_r04_Sub1");
  if (!jets){
      std::cout
	<< "MyJetAnalysis::process_event - Error can not find DST Reco JetMap node "
        << std::endl;
      exit(-1);
    }


  //calorimeter towers
  RawTowerContainer *towersEM3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC_RETOWER");
  RawTowerContainer *towersIH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
  RawTowerContainer *towersOH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");
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

  //centrality

  CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!cent_node)
    {
      std::cout
        << "MyJetAnalysis::process_event - Error can not find centrality node "
	<< std::endl;
      exit(-1);
    }

  int m_centrality =  cent_node->get_centile(CentralityInfo::PROP::mbd_NS);

  //underlying event
  TowerBackground *background = findNode::getClass<TowerBackground>(topNode, "TowerBackground_Sub2");
  if(!background){
    std::cout<<"Can't get background. Exiting"<<std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }


  //get reco jets
  float background_v2 = 0;
  float background_Psi2 = 0;
 
    {
      background_v2 = background->get_v2();
      background_Psi2 = background->get_Psi2();
    }
  for (JetMap::Iter iter = jets->begin(); iter != jets->end(); ++iter)
    {

      Jet* jet = iter->second;



      Jet* unsubjet = new Jetv1();
      float totalPx = 0;
      float totalPy = 0;
      float totalPz = 0;
      float totalE = 0;
      int nconst = 0;

      for (Jet::ConstIter comp = jet->begin_comp(); comp != jet->end_comp(); ++comp)
	{
	  RawTower *tower;
	  nconst++;
	  
	  if ((*comp).first == 15)
	    {
	      tower = towersIH3->getTower((*comp).second);
	      if(!tower || !tower_geom){
		continue;
	      }
	      float UE = background->get_UE(1).at(tower->get_bineta());
	      float tower_phi = tower_geom->get_tower_geometry(tower->get_key())->get_phi();
	      float tower_eta = tower_geom->get_tower_geometry(tower->get_key())->get_eta();
	      UE = UE * (1 + 2 * background_v2 * cos(2 * (tower_phi - background_Psi2)));
	      totalE += tower->get_energy() + UE;
	      double pt = tower->get_energy() / cosh(tower_eta);
	      totalPx += pt * cos(tower_phi);
	      totalPy += pt * sin(tower_phi);
	      totalPz += pt * sinh(tower_eta);
	    }
	  else if ((*comp).first == 16)
	    {
	      tower = towersOH3->getTower((*comp).second);
	      if(!tower || !tower_geomOH)
		{
		  continue;
		}
	      
	      float UE = background->get_UE(2).at(tower->get_bineta());
	      float tower_phi = tower_geomOH->get_tower_geometry(tower->get_key())->get_phi();
	      float tower_eta = tower_geomOH->get_tower_geometry(tower->get_key())->get_eta();
	      UE = UE * (1 + 2 * background_v2 * cos(2 * (tower_phi - background_Psi2)));
	      totalE +=tower->get_energy() + UE;
	      double pt = tower->get_energy() / cosh(tower_eta);
	      totalPx += pt * cos(tower_phi);
	      totalPy += pt * sin(tower_phi);
	      totalPz += pt * sinh(tower_eta);
	    }
	  else if ((*comp).first == 14)
	    {
	      tower = towersEM3->getTower((*comp).second);
	      if(!tower || !tower_geom)
		{
		  continue;
		}
	      float UE = background->get_UE(0).at(tower->get_bineta());
	      float tower_phi = tower_geom->get_tower_geometry(tower->get_key())->get_phi();
	      float tower_eta = tower_geom->get_tower_geometry(tower->get_key())->get_eta();
	      UE = UE * (1 + 2 * background_v2 * cos(2 * (tower_phi - background_Psi2)));
	      totalE +=tower->get_energy() + UE;
	      double pt = tower->get_energy() / cosh(tower_eta);
	      totalPx += pt * cos(tower_phi);
	      totalPy += pt * sin(tower_phi);
	      totalPz += pt * sinh(tower_eta);
	    }
	}

	  //get unsubtracted jet
	  unsubjet->set_px(totalPx);
	  unsubjet->set_py(totalPy);
	  unsubjet->set_pz(totalPz);
	  unsubjet->set_e(totalE);

	  // fill histograms
	  assert(hsubtractedE);
	  hsubtractedE->Fill(jet->get_pt(),unsubjet->get_pt() - jet->get_pt(),m_centrality);


	}




   return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
 int JetUnderlyingEvent::ResetEvent(PHCompositeNode *topNode)
 {
   return Fun4AllReturnCodes::EVENT_OK;
 }

//____________________________________________________________________________..
 int JetUnderlyingEvent::EndRun(const int runnumber)
 {
  std::cout << "JetUnderlyingEvent::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
   return Fun4AllReturnCodes::EVENT_OK;
 }

//____________________________________________________________________________..
int JetUnderlyingEvent::End(PHCompositeNode *topNode)
{
  std::cout << "JetValidation::End - Output to " << m_outputFileName << std::endl;
  PHTFileServer::get().cd(m_outputFileName);

  TH2 *Centrality;
  for(int i = 0; i < hsubtractedE ->GetNbinsZ(); i++)
    {
      hsubtractedE -> GetZaxis()->SetRange(i+1,i+1);
      Centrality = (TH2*)  hsubtractedE -> Project3D("yx");
      Centrality -> Write(Form("hsubtractedE%1.0f",hsubtractedE->GetZaxis()->GetBinLowEdge(i+1)));
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
 int JetUnderlyingEvent::Reset(PHCompositeNode *topNode)
 {
  std::cout << "JetUnderlyingEvent::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
 }

//____________________________________________________________________________..
 void JetUnderlyingEvent::Print(const std::string &what) const
 {
  std::cout << "JetUnderlyingEvent::Print(const std::string &what) const Printing info for " << what << std::endl;
 }
