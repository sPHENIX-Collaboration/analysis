//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in DijetQA.h.
//
// DijetQA(const std::string &name = "DijetQA")
// everything is keyed to DijetQA, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// DijetQA::~DijetQA()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int DijetQA::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int DijetQA::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int DijetQA::process_event(PHCompositeNode *topNode)
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
// int DijetQA::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int DijetQA::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int DijetQA::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int DijetQA::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void DijetQA::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "DijetQA.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//____________________________________________________________________________..
DijetQA::DijetQA(const std::string &name, const std::string &outputfilename, int phismear/*=0*/): 
 SubsysReco(name)
 , m_outputFileName(outputfilename)
 , m_etaRange(-1.1, 1.1)
 , m_ptRange(0, 100)
 , m_T(nullptr)
 , m_event(-1)
 , m_nJet(-1)
 , m_nJetPair(-1)
 , m_centrality(-1)
 , m_zvtx(-1)
 , m_impactparam(-1)
 , m_Ajj(-1)
 , m_xj(-1)
 , m_ptl(-1)
 , m_ptsl(-1)
 , m_phil(-1)
 , m_dphi(-1)
 , m_etal(-1)
 , m_etasl(-1)
 , m_deltaeta(-1)
{
  std::cout << "DijetQA::DijetQA(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
DijetQA::~DijetQA()
{
  std::cout << "DijetQA::~DijetQA() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int DijetQA::Init(PHCompositeNode *topNode)
{
  	std::cout << "DijetQA::Init(PHCompositeNode *topNode) Initializing" << std::endl;
 	
	m_T=new TTree("T", "Dijet QA Tree");
	m_T->Branch("m_event", &m_event, "event/I");
	m_T->Branch("nJet", &m_nJet, "nJet/I");
	m_T->Branch("nJetPair", &m_nJetPair, "nJetPair/I");
	m_T->Branch("cent", &m_centrality);
	m_T->Branch("zvtx", &m_zvtx);
	m_T->Branch("b", &m_impactparam);
	m_T->Branch("A_{jj}", &m_Ajj);
	m_T->Branch("x_{j}", &m_xj);
	m_T->Branch("p_{T, l}", &m_ptl);
	m_T->Branch("p_{T, sl}", &m_ptsl);
	m_T->Branch("#phi_{l}", &m_phil);
	m_T->Branch("#Delta #phi", &m_dphi);
	m_T->Branch("#eta_{l}", &m_etal);
	m_T->Branch("#eta_{sl}", &m_etasl);
	m_T->Branch("#Delta #eta", &m_deltaeta);
	return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int DijetQA::InitRun(PHCompositeNode *topNode)
{
  std::cout << "DijetQA::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//This method actually does the event processing. The way that this is accessed is by going over JetContianers, identifying a leading jet and then the acommpamying pair 
int DijetQA::process_event(PHCompositeNode *topNode)
{
	std::cout << "DijetQA::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
	++m_event;  
	//Setup the node search
	JetContainer* jets=findNode::getClass< JetContainer>(topNode, "AntiKt_Tower_r04");
	if(!jets){
		std::cout<<"No jet container found" <<std::endl;
		--m_event;
		return -1; 
	}
	CentralityInfo* cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
	if(!cent_node){
		std::cout<<"No centrality info found" <<std::endl;
		--m_event;
		return -1;
	}
	else{
		m_centrality=cent_node->get_centile(CentralityInfo::PROP::bimp);
		m_impactparam=cent_node->get_quantity(CentralityInfo::PROP::bimp);
	}
	GlobalVertex *vtx=NULL;
	GlobalVertexMap *vtxmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
	if(!vtxmap || vtxmap->empty())
	{
		std::cout<<"No vertex map found, assuming the vertex has z=0" <<std::endl;
		m_zvtx=0;
	}	
	else{
		vtx=vtxmap->begin()->second;
		m_zvtx=vtx->get_z();
	}
	TowerInfoContainer *EMTowers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
	TowerInfoContainer *IHTowers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
	TowerInfoContainer *OHTowers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
//	RawTowerGeomContainer *ihtower_geom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
//	RawTowerGeomContainer *ohtower_geom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
	if( !EMTowers || !IHTowers || !OHTowers){
		std::cout<<"Could not find the calo towers" <<std::endl;
		--m_event;
		return -1;
	}
	//JetMap* jets=findNode::getClass<JetMap>(topNode, "AntiKt_Tower_r04");
	FindPairs(jets);
	m_T->Fill();
	return Fun4AllReturnCodes::EVENT_OK;
}
void DijetQA::FindPairs(JetContainer* jets)
{
	//find all pairs that are potenital dijets and measure the kinematics
	//prety sure I'm doing this right
	Jet* jet_leading=NULL, *jet_pair1=NULL, *jet_pair2=NULL;
	float pt_leading=0, pt1=0, pt2=0;
	for(auto j1: *jets)
	{
		assert(j1);
		if(j1->get_pt() > pt_leading){
			pt_leading=j1->get_pt();
			jet_leading=j1;
		}
		for(auto j2: *jets){
			if(j2 == j1 ) continue;
			if(abs(j2->get_phi() -j1->get_phi()) > 3 && abs(j2->get_phi() - j1->get_phi() ) < 3.3 )  {
				if(j2->get_pt() > j1->get_pt() ){
					jet_pair1=j2;
					jet_pair2=j1;
				}
				else{
					jet_pair1=j1;
					jet_pair2=j2;
				}
			}
		}
	}
	if(jet_pair1 && jet_pair2){
	pt1=jet_pair1->get_pt();
	pt2=jet_pair2->get_pt();
	m_Ajj=(pt1-pt2)/(pt1+pt2);
	m_xj=pt2/pt1;
	m_ptl=pt1;
	m_ptsl=pt2;
	m_phil=jet_pair1->get_phi();
	m_phisl=jet_pair2->get_phi();
	m_dphi=m_phil-m_phisl;
	m_etal=jet_pair1->get_eta();
	m_etasl=jet_pair2->get_eta();
	m_deltaeta=m_etal-m_etasl;
	std::cout<<"highest pt jet is " <<jet_leading->get_pt() <<" and highest pt in a pair is " <<jet_pair1->get_pt() <<std::endl;
	}
	return;
		
}
//____________________________________________________________________________..
int DijetQA::ResetEvent(PHCompositeNode *topNode)
{
  std::cout << "DijetQA::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int DijetQA::EndRun(const int runnumber)
{
  std::cout << "DijetQA::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int DijetQA::End(PHCompositeNode *topNode)
{
  std::cout << "DijetQA::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int DijetQA::Reset(PHCompositeNode *topNode)
{
 std::cout << "DijetQA::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void DijetQA::Print(const std::string &what) const
{
  	TFile* f=new TFile("Dijet_QA.root", "RECREATE");
	m_T->Write(); 
	f->Write();
	f->Close();
 	std::cout << "DijetQA::Print(const std::string &what) const Printing info for " << what << std::endl;
}
