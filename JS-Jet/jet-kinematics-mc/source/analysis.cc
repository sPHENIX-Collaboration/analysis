#include "analysis.h"
#include <fun4all/SubsysReco.h>  // for SubsysReco

#include <fun4all/Fun4AllReturnCodes.h>
#include <TFile.h>
#include <TH1F.h>
#include <fun4all/Fun4AllHistoManager.h>

#include <phool/PHCompositeNode.h>
#include <g4jets/JetMap.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <g4main/PHG4VtxPoint.h>
#include <TTree.h>
#include <phool/PHCompositeNode.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/PHNode.h>          // for PHNode
#include <phool/PHNodeIterator.h>  // for PHNodeIterator
#include <phool/PHObject.h>        // for PHObject
#include <phool/getClass.h>

#include <iostream>
#include <sstream>
#include <string>
#include <phool/phool.h>                    // for PHWHERE

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>

//____________________________________________________________________________..
analysis::analysis(const std::string &name):
 SubsysReco(name)
{
  std::cout << "analysis::analysis(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
analysis::~analysis()
{
  std::cout << "analysis::~analysis() Calling dtor" << std::endl;
}
//____________________________________________________________________________..

int analysis::Init(PHCompositeNode *topNode)
{
  //-------------------------------------------------------------
  //print the list of nodes available in memory
  //-------------------------------------------------------------
  topNode->print();
  return Fun4AllReturnCodes::EVENT_OK;
}

 Fun4AllHistoManager *analysis::get_HistoManager()
{
  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager("analysis_HISTOS");

  if (not hm)
  {
    std:: cout
        << "analysis::get_HistoManager - Making Fun4AllHistoManager analysis_HISTOS"
        << std::endl;
    hm = new Fun4AllHistoManager("analysis_HISTOS");
    se->registerHistoManager(hm);
  }
  assert(hm);
  return hm;
}

//____________________________________________________________________________..
int analysis::InitRun(PHCompositeNode *topNode)
{
  std::cout << "analysis::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  
  //-------------------------------
  //Create the output file
  //-------------------------------
  PHTFileServer::get().open(m_outfilename.c_str(), "RECREATE");


  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  //------------------------------------------------------------------------------------------------
  // create the output tree where jets will be stored and register it to
  // the histogram manager
  //------------------------------------------------------------------------------------------------

  TTree* tree=  new TTree("tree","tree");
  tree->Print();
  tree->Branch("tjet_pt",&m_tjet_pt);
  tree->Branch("tjet_phi",&m_tjet_phi);
  tree->Branch("tjet_eta",&m_tjet_eta);
  tree->Branch("rjet_pt",&m_rjet_pt);
  tree->Branch("rjet_phi",&m_rjet_phi);
  tree->Branch("rjet_eta",&m_rjet_eta);

  hm->registerHisto(tree);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int analysis::process_event(PHCompositeNode *topNode)
{
  double pi = TMath::Pi();

  //-----------------------------------------------------------------------------------------------------------------
  // load in the containers which hold the truth and reconstructed jet information
  //-----------------------------------------------------------------------------------------------------------------

  JetMap* tjets = findNode::getClass<JetMap>(topNode, "AntiKt_Truth_r04");
  if (!tjets)
  {
    std::cout
        << "MyJetAnalysis::process_event - Error can not find DST JetMap node "
        << std::endl;
    exit(-1);
  }


  JetMap* rjets = findNode::getClass<JetMap>(topNode, "AntiKt_Tower_r04");
  if (!rjets)
  {
    std::cout
        << "MyJetAnalysis::process_event - Error can not find DST JetMap node "
        << std::endl;
    exit(-1);
  }

  //-------------------------------------------------------------
  //Call the histogram manager in order to 
  //pull the tree into the perview of the event 
  //-------------------------------------------------------------

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);
  //---------------------------------------------------
  // loop over all truth jets in the event
  //---------------------------------------------------
  for (JetMap::Iter iter = tjets->begin(); iter != tjets->end(); ++iter)
  {
    Jet* tjet = iter->second;
    assert(tjet);
    float tjet_phi = tjet->get_phi();
    float tjet_eta = tjet->get_eta();
    float tjet_pt = tjet->get_pt();
    float drmin = 0.3;
    float matched_pt = -999;
    float matched_eta = -999;
    float matched_phi = -999;
    //---------------------------------------------------------------------------------------------------
    //loop over reconstructed jets and match each truth jet to the closest 
    //reconstructed jet
    //----------------------------------------------------------------------------------------------------
    for (JetMap::Iter riter = rjets->begin(); riter != rjets->end(); ++riter)
      {
	Jet* rjet = riter->second;
	float rjet_phi = rjet->get_phi();
	float rjet_eta = rjet->get_eta();
	float rjet_pt = rjet->get_pt();
	
        float deta = fabs(rjet_eta - tjet_eta);
	float dphi = fabs(rjet_phi - tjet_phi);
	if (dphi > pi)
	  {
	    dphi = 2*pi-dphi;
	  }

	float dr = TMath::Sqrt(dphi*dphi + deta*deta);
	if (dr < drmin)//truth-reco jet matching
	  {
	    matched_pt = rjet_pt;
	    matched_eta = rjet_eta;
	    matched_phi = rjet_phi;
	    drmin = dr;
	  }
      }
    //-------------------------------------------------------------------------
    //append the truth and matched reconstructed jet
    // to the end of the corresponding vectors
    //-------------------------------------------------------------------------
    if (tjet_pt > 0)
      {	
	m_tjet_pt.push_back(tjet_pt);
	m_tjet_phi.push_back(tjet_phi);
	m_tjet_eta.push_back(tjet_eta);
	m_rjet_pt.push_back(matched_pt);
	m_rjet_phi.push_back(matched_phi);
	m_rjet_eta.push_back(matched_eta);
	m_dr.push_back(drmin);
	
	
	
      }
    }

  //----------------------------------------------------------
  //Record the vectors of jet kinematic info
  //to the ttree.
  //----------------------------------------------------------

  TTree *tree = dynamic_cast<TTree *>(hm->getHisto("tree"));
  assert(tree);
  tree->Fill();


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int analysis::ResetEvent(PHCompositeNode *topNode)
{
  //-----------------------------------------------------
  //Clear every vector inbetween events
  // this is CRITICAL!! Otherwise you will 
  // have runaway behaviors
  //------------------------------------------------------
  m_tjet_pt.clear();
  m_tjet_phi.clear();
  m_tjet_eta.clear();

  m_rjet_pt.clear();
  m_rjet_phi.clear();
  m_rjet_eta.clear();
  m_dr.clear();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int analysis::EndRun(const int runnumber)
{
  std::cout << "analysis::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int analysis::End(PHCompositeNode *topNode)
{
  std::cout << "analysis::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  //----------------------------------------------------------------------
  //Enter the created output file and write the ttree 
  //----------------------------------------------------------------------

  PHTFileServer::get().cd(m_outfilename.c_str());

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  for (unsigned int i = 0; i < hm->nHistos(); i++)
    hm->getHisto(i)->Write();


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int analysis::Reset(PHCompositeNode *topNode)
{
 std::cout << "analysis::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void analysis::Print(const std::string &what) const
{
  std::cout << "analysis::Print(const std::string &what) const Printing info for " << what << std::endl;
}
